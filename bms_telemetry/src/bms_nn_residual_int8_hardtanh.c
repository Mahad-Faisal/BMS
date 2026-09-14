/*
 * bms_nn_residual_int8_hardtanh.c
 *
 * Scalar INT8 HardTanh backend for the bounded BMS neural voltage-residual
 * layer.
 */

#include "bms_nn_residual_int8_hardtanh.h"
#include "arm_nnfunctions.h"
#include "arm_nn_types.h"
#include <math.h>
#include <stddef.h>
#include <string.h>
#include "arm_nnfunctions.h"
#include "arm_nn_types.h"

#ifndef ARM_MATH_MVEI
#warning "ARM_MATH_MVEI not enabled"
#endif

#ifndef ARM_MATH_DSP
#warning "ARM_MATH_DSP not enabled"
#endif

#ifndef __ARM_FEATURE_MVE
#warning "__ARM_FEATURE_MVE not enabled"
#endif

#ifndef __ARM_FEATURE_MVE
#error "Compiler is not emitting MVE/Helium target macros"
#endif
/* Runtime gate constants from the training/export configuration. */
#define BMS_NN_I_NORM_A          30.0f
#define BMS_NN_I_GATE_MAX_A      35.0f
#define BMS_NN_SOC_MIN           0.02f
#define BMS_NN_SOC_MAX           0.98f
#define BMS_NN_TCORE_MIN_C       (-10.0f)
#define BMS_NN_TCORE_MAX_C       60.0f
#define BMS_NN_R0_MIN_OHM        0.005f
#define BMS_NN_R0_MAX_OHM        0.080f
#define BMS_NN_INNOV_GATE_MAX_V  0.300f
#define BMS_NN_INNOV_RATIO_MAX   0.65f
#define BMS_NN_INNOV_EPS_V       1.0e-6f
#define BMS_NN_WARMUP_STEPS      60U

/*
 * Training used pandas ewm(alpha=TS/(tau+TS), adjust=False), with TS=0.1 s
 * and tau=60 s.
 */
#ifndef BMS_NN_I_BIAS_EMA_ALPHA
#define BMS_NN_I_BIAS_EMA_ALPHA  0.00166389351f
#endif

/* Feature normalization for v11 HardTanh 15-feature TDNN. */
static const float bms_nn_ht_input_offset[BMS_NN_Q_N_IN] =
{
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 25.0f, 0.023f, 0.0f, 0.0f
};

static const float bms_nn_ht_input_scale[BMS_NN_Q_N_IN] =
{
    30.0f, 30.0f, 30.0f, 30.0f, 30.0f,
    30.0f, 30.0f, 30.0f, 1.0f, 30.0f,
    1.0f, 25.0f, 0.00999999978f, 0.5f, 0.5f
};

static float bms_nn_absf(float x)
{
    return (x < 0.0f) ? -x : x;
}

static float bms_nn_clampf(float x, float lo, float hi)
{
    if (x < lo)
    {
        return lo;
    }

    if (x > hi)
    {
        return hi;
    }

    return x;
}

static int32_t bms_nn_clamp_i32(int32_t x, int32_t lo, int32_t hi)
{
    if (x < lo)
    {
        return lo;
    }

    if (x > hi)
    {
        return hi;
    }

    return x;
}

static int8_t bms_nn_clamp_i8_from_i32(int32_t x)
{
    x = bms_nn_clamp_i32(x, -127, 127);
    return (int8_t)x;
}

static int32_t bms_nn_round_to_i32(float x)
{
    /*
     * roundf() matches the generated Python reference closely for non-tie
     * values. Exact x.5 ties are not expected in this weight/vector path.
     */
    if (x >= 2147483520.0f)
    {
        return 2147483647L;
    }

    if (x <= -2147483520.0f)
    {
        return (int32_t)(-2147483647L - 1L);
    }

    return (int32_t)roundf(x);
}

static int8_t bms_nn_quantize_input(float x_norm)
{
    float x = bms_nn_clampf(x_norm, -BMS_NN_Q_INPUT_CLIP, BMS_NN_Q_INPUT_CLIP);
    int32_t q = bms_nn_round_to_i32(x / BMS_NN_Q_INPUT_SCALE);
    return bms_nn_clamp_i8_from_i32(q);
}

static int8_t bms_nn_requantize_act(int32_t acc, float real_multiplier)
{
    int32_t q = bms_nn_round_to_i32(((float)acc) * real_multiplier);
    return bms_nn_clamp_i8_from_i32(q);
}

void bms_nn_residual_reset(bms_nn_ctx_t *ctx)
{
    if (ctx == (bms_nn_ctx_t *)0)
    {
        return;
    }

    memset(ctx, 0, sizeof(*ctx));
}

static float bms_nn_get_i_tap(const bms_nn_ctx_t *ctx, float i_now_a, uint32_t tap)
{
    if ((ctx == (const bms_nn_ctx_t *)0) || (tap == 0U))
    {
        return i_now_a;
    }

    if (ctx->sample_count < tap)
    {
        return i_now_a;
    }

    uint32_t idx = (ctx->head + BMS_NN_I_HIST_LEN - (tap % BMS_NN_I_HIST_LEN)) %
                   BMS_NN_I_HIST_LEN;

    return ctx->i_hist[idx];
}

static void bms_nn_push_current(bms_nn_ctx_t *ctx, float i_now_a)
{
    ctx->i_hist[ctx->head] = i_now_a;

    ctx->head++;
    if (ctx->head >= BMS_NN_I_HIST_LEN)
    {
        ctx->head = 0U;
    }

    if (ctx->sample_count < 0xFFFFFFFFUL)
    {
        ctx->sample_count++;
    }
}

static void bms_nn_normalize(const float raw[BMS_NN_Q_N_IN], float norm[BMS_NN_Q_N_IN])
{
    for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_N_IN; i++)
    {
        float x = (raw[i] - bms_nn_ht_input_offset[i]) / bms_nn_ht_input_scale[i];
        norm[i] = bms_nn_clampf(x, -BMS_NN_Q_INPUT_CLIP, BMS_NN_Q_INPUT_CLIP);
    }
}

float bms_nn_residual_int8_hardtanh_forward_norm(const float x_norm[BMS_NN_Q_N_IN],
                                                 float *pred_base_v,
                                                 bms_nn_int8_hardtanh_debug_t *debug)
{
    int8_t xq[BMS_NN_Q_N_IN];
    int8_t a1q[BMS_NN_Q_H1];
    int8_t a2q[BMS_NN_Q_H2];

    for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_N_IN; i++)
    {
        xq[i] = bms_nn_quantize_input(x_norm[i]);
    }

    for (uint32_t j = 0U; j < (uint32_t)BMS_NN_Q_H1; j++)
    {
        int32_t acc = bms_nn_q_b1[j];

        for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_N_IN; i++)
        {
            acc += ((int32_t)bms_nn_q_w1[(j * (uint32_t)BMS_NN_Q_N_IN) + i]) *
                   ((int32_t)xq[i]);
        }

        a1q[j] = bms_nn_requantize_act(acc, bms_nn_q_l1_to_act_real_multiplier[j]);
    }

    for (uint32_t j = 0U; j < (uint32_t)BMS_NN_Q_H2; j++)
    {
        int32_t acc = bms_nn_q_b2[j];

        for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_H1; i++)
        {
            acc += ((int32_t)bms_nn_q_w2[(j * (uint32_t)BMS_NN_Q_H1) + i]) *
                   ((int32_t)a1q[i]);
        }

        a2q[j] = bms_nn_requantize_act(acc, bms_nn_q_l2_to_act_real_multiplier[j]);
    }

    int32_t acc3 = bms_nn_q_b3[0];

    for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_H2; i++)
    {
        acc3 += ((int32_t)bms_nn_q_w3[i]) * ((int32_t)a2q[i]);
    }

    float raw_v = ((float)acc3) * BMS_NN_Q_OUTPUT_V_PER_ACC;
    float base_v = raw_v / BMS_NN_Q_OUTPUT_GAIN;

    if (pred_base_v != (float *)0)
    {
        *pred_base_v = base_v;
    }

    if (debug != (bms_nn_int8_hardtanh_debug_t *)0)
    {
        for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_N_IN; i++)
        {
            debug->xq[i] = xq[i];
        }

        for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_H1; i++)
        {
            debug->a1q[i] = a1q[i];
        }

        for (uint32_t i = 0U; i < (uint32_t)BMS_NN_Q_H2; i++)
        {
            debug->a2q[i] = a2q[i];
        }

        debug->acc3 = acc3;
        debug->pred_base_v = base_v;
        debug->pred_raw_v = raw_v;
    }

    return raw_v;
}

static float bms_nn_dynamic_vlim(float i_pack_a, float tcore_c)
{
    float i_factor = bms_nn_clampf(bms_nn_absf(i_pack_a) / BMS_NN_I_NORM_A, 0.0f, 1.0f);
    float t_factor = bms_nn_clampf((25.0f - tcore_c) / 25.0f, 0.0f, 1.0f);

    float vlim = 0.010f + (0.020f * i_factor) + (0.010f * t_factor);
    return bms_nn_clampf(vlim, 0.010f, BMS_NN_Q_V_CLAMP_V);
}

static uint8_t bms_nn_gate_allowed(float i_pack_a,
                                   float soc,
                                   float tcore_c,
                                   float r0_ohm,
                                   float innovation_raw_v,
                                   uint32_t plant_step,
                                   uint32_t *flags)
{
    uint8_t allowed = 1U;

    if (plant_step < BMS_NN_WARMUP_STEPS)
    {
        allowed = 0U;
        *flags |= BMS_NN_FLAG_GATE_WARMUP;
    }

    if ((soc < BMS_NN_SOC_MIN) || (soc > BMS_NN_SOC_MAX))
    {
        allowed = 0U;
        *flags |= BMS_NN_FLAG_GATE_SOC;
    }

    if ((tcore_c < BMS_NN_TCORE_MIN_C) || (tcore_c > BMS_NN_TCORE_MAX_C))
    {
        allowed = 0U;
        *flags |= BMS_NN_FLAG_GATE_TCORE;
    }

    if (bms_nn_absf(i_pack_a) > BMS_NN_I_GATE_MAX_A)
    {
        allowed = 0U;
        *flags |= BMS_NN_FLAG_GATE_CURRENT;
    }

    if (bms_nn_absf(innovation_raw_v) > BMS_NN_INNOV_GATE_MAX_V)
    {
        allowed = 0U;
        *flags |= BMS_NN_FLAG_GATE_INNOV;
    }

    if ((r0_ohm < BMS_NN_R0_MIN_OHM) || (r0_ohm > BMS_NN_R0_MAX_OHM))
    {
        allowed = 0U;
        *flags |= BMS_NN_FLAG_GATE_R0;
    }

    return allowed;
}

bms_nn_output_t bms_nn_residual_step(bms_nn_ctx_t *ctx,
                                     float i_pack_a,
                                     float dI_pack_a,
                                     float prev_soc,
                                     float prev_tcore_c,
                                     float prev_r0_ohm,
                                     float prev_vp1_v,
                                     float prev_vp2_v,
                                     float innovation_raw_v,
                                     uint32_t plant_step)
{
    bms_nn_output_t out;
    memset(&out, 0, sizeof(out));

    if (ctx == (bms_nn_ctx_t *)0)
    {
        out.flags = BMS_NN_FLAG_GATE_WARMUP;
        out.gate_allowed = 0U;
        return out;
    }

    if (plant_step == 0U)
    {
        bms_nn_residual_reset(ctx);
        out.flags |= BMS_NN_FLAG_RESET;
    }

    if (ctx->sample_count == 0U)
    {
        ctx->i_bias_60s = i_pack_a;
    }
    else
    {
        ctx->i_bias_60s += BMS_NN_I_BIAS_EMA_ALPHA * (i_pack_a - ctx->i_bias_60s);
    }

    float raw[BMS_NN_Q_N_IN];
    float norm[BMS_NN_Q_N_IN];

    raw[0]  = bms_nn_get_i_tap(ctx, i_pack_a, 0U);
    raw[1]  = bms_nn_get_i_tap(ctx, i_pack_a, 1U);
    raw[2]  = bms_nn_get_i_tap(ctx, i_pack_a, 2U);
    raw[3]  = bms_nn_get_i_tap(ctx, i_pack_a, 5U);
    raw[4]  = bms_nn_get_i_tap(ctx, i_pack_a, 10U);
    raw[5]  = bms_nn_get_i_tap(ctx, i_pack_a, 20U);
    raw[6]  = bms_nn_get_i_tap(ctx, i_pack_a, 30U);
    raw[7]  = ctx->i_bias_60s;
    raw[8]  = (ctx->i_bias_60s / BMS_NN_I_NORM_A) * (i_pack_a / BMS_NN_I_NORM_A);
    raw[9]  = dI_pack_a;
    raw[10] = prev_soc;
    raw[11] = prev_tcore_c;
    raw[12] = prev_r0_ohm;
    raw[13] = prev_vp1_v;
    raw[14] = prev_vp2_v;

    bms_nn_normalize(raw, norm);

    out.pred_gain_v = bms_nn_residual_int8_hardtanh_forward_norm(norm,
                                                                  &out.pred_base_v,
                                                                  (bms_nn_int8_hardtanh_debug_t *)0);

    out.pred_gain_v = bms_nn_clampf(out.pred_gain_v,
                                    -BMS_NN_Q_V_CLAMP_V,
                                    BMS_NN_Q_V_CLAMP_V);

    /*
     * Closed-loop safety guard.
     *
     * The residual correction is applied upstream as:
     *
     *     innovation_used = innovation_raw - dVnn
     *
     * Therefore dVnn must have the same sign as innovation_raw and must not
     * consume more than a bounded fraction of the raw innovation. This restores
     * the gate065 behavior used in the FP32 paper runs and prevents the NN from
     * worsening a healthy physics estimate.
     */
    float abs_innov = bms_nn_absf(innovation_raw_v);

    if (abs_innov < BMS_NN_INNOV_EPS_V)
    {
        out.pred_gain_v = 0.0f;
        out.flags |= BMS_NN_FLAG_DEADBAND;
    }
    else if ((out.pred_gain_v * innovation_raw_v) <= 0.0f)
    {
        out.pred_gain_v = 0.0f;
        out.flags |= BMS_NN_FLAG_SIGN_MISMATCH;
    }
    else
    {
        float ratio_lim = BMS_NN_INNOV_RATIO_MAX * abs_innov;

        if (bms_nn_absf(out.pred_gain_v) > ratio_lim)
        {
            out.pred_gain_v = (out.pred_gain_v > 0.0f) ? ratio_lim : -ratio_lim;
            out.flags |= BMS_NN_FLAG_RATIO_CLAMP;
        }
    }

    out.vlim_v = bms_nn_dynamic_vlim(i_pack_a, prev_tcore_c);

    if (bms_nn_absf(out.pred_gain_v) >= (0.995f * out.vlim_v))
    {
        out.flags |= BMS_NN_FLAG_DYN_CLAMP;
    }

    out.pred_safe_v = bms_nn_clampf(out.pred_gain_v, -out.vlim_v, out.vlim_v);

    if (bms_nn_absf(out.pred_safe_v) < BMS_NN_Q_OUTPUT_DEADBAND_V)
    {
        out.pred_safe_v = 0.0f;
        out.flags |= BMS_NN_FLAG_DEADBAND;
    }

    out.gate_allowed = bms_nn_gate_allowed(i_pack_a,
                                           prev_soc,
                                           prev_tcore_c,
                                           prev_r0_ohm,
                                           innovation_raw_v,
                                           plant_step,
                                           &out.flags);

    if (out.gate_allowed == 0U)
    {
        out.pred_safe_v = 0.0f;
    }

    bms_nn_push_current(ctx, i_pack_a);

    return out;
}
