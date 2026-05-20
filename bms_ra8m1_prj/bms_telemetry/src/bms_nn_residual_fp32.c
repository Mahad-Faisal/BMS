/*
 * bms_nn_residual_fp32.c
 *
 * FP32 runtime wrapper for the v9 bounded BMS NN voltage-residual model with clean-neutral innovation safeguard.
 */

#include "bms_nn_residual_fp32.h"
#include "bms_nn_residual_weights_fp32_c99_fixed.h"

#include <math.h>
#include <string.h>

#define BMS_NN_I_NORM_A          30.0f
#define BMS_NN_I_GATE_MAX_A      35.0f
#define BMS_NN_SOC_MIN           0.02f
#define BMS_NN_SOC_MAX           0.98f
#define BMS_NN_TCORE_MIN_C       (-10.0f)
#define BMS_NN_TCORE_MAX_C       60.0f
#define BMS_NN_R0_MIN_OHM        0.005f
#define BMS_NN_R0_MAX_OHM        0.080f
#define BMS_NN_INNOV_GATE_MAX_V  0.300f
#define BMS_NN_WARMUP_STEPS      60U

/* Clean-neutral safety overlay: only apply correction if it is aligned
 * with the current raw innovation and limit it to a fraction of that
 * innovation. This guarantees the instantaneous update cannot increase
 * |innovation| through overshoot or sign error.
 */
#define BMS_NN_INNOV_MIN_APPLY_V  0.00075f
#define BMS_NN_INNOV_RATIO_MAX    0.65f // tested 0.75 it is worse

static float bms_nn_absf(float x)
{
    return (x < 0.0f) ? -x : x;
}

static float bms_nn_clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
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

    /*
     * ctx->head is the next write location. Before the current sample is
     * written, head-1 is I[k-1], head-2 is I[k-2], etc.
     *
     * During warmup, exact tap value does not matter because output is gated
     * until plant_step >= 60. Using current value avoids artificial startup
     * spikes in normalized features.
     */
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

static void bms_nn_normalize(const float raw[BMS_NN_N_IN], float norm[BMS_NN_N_IN])
{
    for (uint32_t i = 0U; i < (uint32_t)BMS_NN_N_IN; i++)
    {
        float x = (raw[i] - bms_nn_input_offset[i]) / bms_nn_input_scale[i];
        norm[i] = bms_nn_clampf(x, -BMS_NN_INPUT_CLIP, BMS_NN_INPUT_CLIP);
    }
}

static float bms_nn_forward_norm(const float x_in[BMS_NN_N_IN])
{
    float x[BMS_NN_N_IN];
    float h1[BMS_NN_H1];
    float h2[BMS_NN_H2];

    for (uint32_t i = 0U; i < (uint32_t)BMS_NN_N_IN; i++)
    {
        x[i] = bms_nn_clampf(x_in[i], -BMS_NN_INPUT_CLIP, BMS_NN_INPUT_CLIP);
    }

    for (uint32_t j = 0U; j < (uint32_t)BMS_NN_H1; j++)
    {
        float acc = bms_nn_b1[j];

        for (uint32_t i = 0U; i < (uint32_t)BMS_NN_N_IN; i++)
        {
            acc += bms_nn_w1[(j * (uint32_t)BMS_NN_N_IN) + i] * x[i];
        }

        h1[j] = tanhf(acc);
    }

    for (uint32_t j = 0U; j < (uint32_t)BMS_NN_H2; j++)
    {
        float acc = bms_nn_b2[j];

        for (uint32_t i = 0U; i < (uint32_t)BMS_NN_H1; i++)
        {
            acc += bms_nn_w2[(j * (uint32_t)BMS_NN_H1) + i] * h1[i];
        }

        h2[j] = tanhf(acc);
    }

    float acc = bms_nn_b3;

    for (uint32_t i = 0U; i < (uint32_t)BMS_NN_H2; i++)
    {
        acc += bms_nn_w3[i] * h2[i];
    }

    return BMS_NN_V_CLAMP_V * tanhf(acc);
}

static float bms_nn_dynamic_vlim(float i_pack_a, float tcore_c)
{
    float i_factor = bms_nn_clampf(bms_nn_absf(i_pack_a) / BMS_NN_I_NORM_A, 0.0f, 1.0f);
    float t_factor = bms_nn_clampf((25.0f - tcore_c) / 25.0f, 0.0f, 1.0f);

    float vlim = 0.010f + (0.020f * i_factor) + (0.010f * t_factor);
    return bms_nn_clampf(vlim, 0.010f, BMS_NN_V_CLAMP_V);
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

    /*
     * Python training used pandas ewm(adjust=False), so the first EMA value
     * equals the first current sample.
     */
    if (ctx->sample_count == 0U)
    {
        ctx->i_bias_60s = i_pack_a;
    }
    else
    {
        ctx->i_bias_60s += BMS_NN_I_BIAS_EMA_ALPHA * (i_pack_a - ctx->i_bias_60s);
    }

    float raw[BMS_NN_N_IN];
    float norm[BMS_NN_N_IN];

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

    out.pred_base_v = bms_nn_forward_norm(norm);
    out.pred_gain_v = bms_nn_clampf(out.pred_base_v * BMS_NN_OUTPUT_GAIN,
                                    -BMS_NN_V_CLAMP_V,
                                    BMS_NN_V_CLAMP_V);

    out.vlim_v = bms_nn_dynamic_vlim(i_pack_a, prev_tcore_c);

    if (bms_nn_absf(out.pred_gain_v) >= (0.995f * out.vlim_v))
    {
        out.flags |= BMS_NN_FLAG_DYN_CLAMP;
    }

    out.pred_safe_v = bms_nn_clampf(out.pred_gain_v, -out.vlim_v, out.vlim_v);

    if (bms_nn_absf(out.pred_safe_v) < BMS_NN_OUTPUT_DEADBAND_V)
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
    else if (out.pred_safe_v != 0.0f)
    {
        float innov_abs = bms_nn_absf(innovation_raw_v);

        if (innov_abs < BMS_NN_INNOV_MIN_APPLY_V)
        {
            out.pred_safe_v = 0.0f;
            out.flags |= BMS_NN_FLAG_INNOV_MIN;
        }
        else if ((innovation_raw_v * out.pred_safe_v) <= 0.0f)
        {
            /*
             * NN correction must have the same sign as raw innovation.
             * Otherwise, innovation_used = innovation_raw - dV_NN would
             * increase magnitude instead of reducing it.
             */
            out.pred_safe_v = 0.0f;
            out.flags |= BMS_NN_FLAG_SIGN_GATE;
        }
        else
        {
            float max_corr = BMS_NN_INNOV_RATIO_MAX * innov_abs;

            if (bms_nn_absf(out.pred_safe_v) > max_corr)
            {
                out.pred_safe_v = (innovation_raw_v >= 0.0f) ? max_corr : -max_corr;
                out.flags |= BMS_NN_FLAG_INNOV_RATIO_CLAMP;
            }

            if (bms_nn_absf(out.pred_safe_v) < BMS_NN_OUTPUT_DEADBAND_V)
            {
                out.pred_safe_v = 0.0f;
                out.flags |= BMS_NN_FLAG_DEADBAND;
            }
        }
    }

    bms_nn_push_current(ctx, i_pack_a);

    return out;
}
