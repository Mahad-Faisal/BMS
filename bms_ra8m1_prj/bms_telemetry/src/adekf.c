/*
 * adekf.c — Adaptive Dual Extended Kalman Filter
 *
 * ── Discretization ───────────────────────────────────────────────────────────
 * All states use forward Euler to match the Simulink plant exactly.
 *
 * ── Temperature handling ─────────────────────────────────────────────────────
 * The estimator maintains T_core_est via a forward thermal observer to eliminate
 * the T_surf/T_core mismatch present in naive implementations.
 *
 * ── Covariance propagation ────────────────────────────────────────────────────
 * A diagonal covariance approximation is used for embedded efficiency, while the
 * measurement update uses Joseph-form diagonal variance propagation to preserve
 * nonnegative stored variances under finite-precision arithmetic.
 *
 * ── Adaptive R ────────────────────────────────────────────────────────────────
 * Practical sliding-window innovation covariance adaptation is used to track
 * measurement noise variance dynamically without full MMAE overhead.
 *
 * ── AI residual logging readiness ───────────────────────────────────────────
 * ekf->V_pack_est is the pre-update physics voltage prediction used for the
 * innovation. Log it as Vpred_mV from the caller for NN residual training:
 *     dV_target = V_meas - Vpred_physics = ekf->innovation.
 */

#include "adekf.h"
#include <string.h>
#include <math.h>

#include "estimator_variant.h"
#include <stdbool.h>
#include "bms_nn_residual_fp32.h"


#define ESTIMATOR_VARIANT ESTIMATOR_VARIANT_DAEKF

#define ADEKF_INIT_T_CORE_C 25.0f

/* ── LUT data — exact copy of const_params.c pooled arrays ──────────────── */
static const float s_T_bp[3] = { 0.0f, 10.0f, 25.0f };

static const float s_SoC_bp_101[101] = {
    0.0f,0.01f,0.02f,0.03f,0.04f,0.05f,0.06f,0.07f,0.08f,0.09f,
    0.1f,0.11f,0.12f,0.13f,0.14f,0.15f,0.16f,0.17f,0.18f,0.19f,
    0.2f,0.21f,0.22f,0.23f,0.24f,0.25f,0.26f,0.27f,0.28f,0.29f,
    0.3f,0.31f,0.32f,0.33f,0.34f,0.35f,0.36f,0.37f,0.38f,0.39f,
    0.4f,0.41f,0.42f,0.43f,0.44f,0.45f,0.46f,0.47f,0.48f,0.49f,
    0.5f,0.51f,0.52f,0.53f,0.54f,0.55f,0.56f,0.57f,0.58f,0.59f,
    0.6f,0.61f,0.62f,0.63f,0.64f,0.65f,0.66f,0.67f,0.68f,0.69f,
    0.7f,0.71f,0.72f,0.73f,0.74f,0.75f,0.76f,0.77f,0.78f,0.79f,
    0.8f,0.81f,0.82f,0.83f,0.84f,0.85f,0.86f,0.87f,0.88f,0.89f,
    0.9f,0.91f,0.92f,0.93f,0.94f,0.95f,0.96f,0.97f,0.98f,0.99f,1.0f
};

static const float s_OCV_table[303] = {
    /* T=0°C (101 values) */
    3.15119815f,3.16335249f,3.17550707f,3.18766141f,3.19981575f,
    3.21197033f,3.22412467f,3.23627925f,3.24843359f,3.26058817f,
    3.27274251f,3.28489685f,3.29705143f,3.30920577f,3.32136035f,
    3.33351469f,3.34566903f,3.35782361f,3.36997795f,3.38213253f,
    3.39428687f,3.40644121f,3.41859579f,3.43075013f,3.44290471f,
    3.45505905f,3.46516132f,3.47526336f,3.48536563f,3.49546766f,
    3.50557000f,3.51277518f,3.51998043f,3.52718592f,3.53439116f,
    3.54159641f,3.54797578f,3.55435514f,3.56073451f,3.56711388f,
    3.57349324f,3.57978988f,3.58608651f,3.59238338f,3.59868000f,
    3.60497665f,3.61122251f,3.61746860f,3.62371445f,3.62996030f,
    3.63620615f,3.64444804f,3.65269000f,3.66093183f,3.66917372f,
    3.67741561f,3.68658447f,3.69575334f,3.70492220f,3.71409106f,
    3.72326000f,3.73381805f,3.74437618f,3.75493455f,3.76549268f,
    3.77605104f,3.78708148f,3.79811192f,3.80914259f,3.82017303f,
    3.83120346f,3.84072781f,3.85025191f,3.85977626f,3.86930037f,
    3.87882471f,3.88799477f,3.89716506f,3.90633512f,3.91550541f,
    3.92467570f,3.93581796f,3.94696045f,3.95810294f,3.96924543f,
    3.98038793f,3.99181676f,4.00324583f,4.01467466f,4.02610350f,
    4.03753233f,4.04659557f,4.05565882f,4.06472158f,4.07378483f,
    4.08284807f,4.09702635f,4.11120462f,4.12538338f,4.13956165f,4.15374000f,
    /* T=10°C (101 values) */
    3.10965061f,3.12426090f,3.13887119f,3.15348148f,3.16809154f,
    3.18270183f,3.19731212f,3.21192241f,3.22653270f,3.24114299f,
    3.25575304f,3.27036333f,3.28497362f,3.29958391f,3.31419420f,
    3.32880425f,3.34341455f,3.35802484f,3.37263513f,3.38724542f,
    3.40185547f,3.41522694f,3.42859817f,3.44196939f,3.45534062f,
    3.46871185f,3.47857618f,3.48844051f,3.49830484f,3.50816917f,
    3.51803327f,3.52472925f,3.53142500f,3.53812099f,3.54481673f,
    3.55151272f,3.55729413f,3.56307578f,3.56885743f,3.57463884f,
    3.58042049f,3.58644819f,3.59247589f,3.59850335f,3.60453105f,
    3.61055875f,3.61673760f,3.62291622f,3.62909508f,3.63527393f,
    3.64145255f,3.64999199f,3.65853143f,3.66707087f,3.67561000f,
    3.68414950f,3.69378519f,3.70342088f,3.71305633f,3.72269201f,
    3.73232770f,3.74305344f,3.75377917f,3.76450491f,3.77523088f,
    3.78595662f,3.79705286f,3.80814934f,3.81924558f,3.83034205f,
    3.84143829f,3.85049000f,3.85954189f,3.86859369f,3.87764525f,
    3.88669705f,3.89526868f,3.90384030f,3.91241193f,3.92098355f,
    3.92955518f,3.94073796f,3.95192099f,3.96310377f,3.97428679f,
    3.98546958f,3.99703169f,4.00859404f,4.02015591f,4.03171825f,
    4.04328000f,4.05285000f,4.06241894f,4.07198811f,4.08155775f,
    4.09112692f,4.10390759f,4.11668825f,4.12946892f,4.14224958f,4.15503000f,
    /* T=25°C (101 values) */
    3.00794721f,3.03406835f,3.06018949f,3.08631063f,3.11243153f,
    3.13855267f,3.16467381f,3.19079494f,3.21691585f,3.24303699f,
    3.26915812f,3.28806090f,3.30696368f,3.32586646f,3.34476948f,
    3.36367226f,3.37518000f,3.38668752f,3.39819527f,3.40970302f,
    3.42121077f,3.43398762f,3.44676471f,3.45954180f,3.47231865f,
    3.48509574f,3.49464273f,3.50418973f,3.51373672f,3.52328372f,
    3.53283072f,3.53930521f,3.54577971f,3.55225420f,3.55872869f,
    3.56520295f,3.57058620f,3.57596922f,3.58135247f,3.58673549f,
    3.59211850f,3.59803581f,3.60395288f,3.60987000f,3.61578703f,
    3.62170410f,3.62794924f,3.63419437f,3.64043951f,3.64668441f,
    3.65292954f,3.66244316f,3.67195654f,3.68147016f,3.69098353f,
    3.70049715f,3.71152806f,3.72255898f,3.73359000f,3.74462056f,
    3.75565147f,3.76554036f,3.77542925f,3.78531790f,3.79520679f,
    3.80509567f,3.81459641f,3.82409716f,3.83359790f,3.84309840f,
    3.85259914f,3.86140132f,3.87020373f,3.87900591f,3.88780808f,
    3.89661026f,3.90524817f,3.91388607f,3.92252421f,3.93116212f,
    3.93980000f,3.95099115f,3.96218228f,3.97337341f,3.98456454f,
    3.99575567f,4.00731802f,4.01888000f,4.03044224f,4.04200411f,
    4.05356646f,4.06326151f,4.07295704f,4.08265209f,4.09234715f,
    4.10204220f,4.11598587f,4.12992954f,4.14387274f,4.15781641f,4.17176000f
};

static const float s_SoC_bp_12[12] = {
    0.17f,0.24545455f,0.32090908f,0.39636365f,0.47181818f,0.54727274f,
    0.62272728f,0.69818181f,0.77363634f,0.84909093f,0.92454547f,1.0f
};

static const float s_R0_table[36] = {
    0.05106587f,0.05020133f,0.04181795f,0.04094532f,0.04138768f,0.04111945f,
    0.03964384f,0.03852182f,0.04193546f,0.04571731f,0.05040148f,0.05011997f,
    0.04142398f,0.03934896f,0.03222800f,0.02997955f,0.02975151f,0.03010294f,
    0.03020120f,0.02959669f,0.02985020f,0.03180491f,0.03486196f,0.03658538f,
    0.02590232f,0.02306531f,0.02229200f,0.02171673f,0.02074237f,0.02021998f,
    0.02051975f,0.02052523f,0.02081944f,0.02155843f,0.02249892f,0.02318704f
};

static const float s_C1_table[36] = {
    0.00750000f,0.00750000f,0.00610389f,0.00499172f,0.00432587f,0.00413168f,
    0.00435093f,0.00477217f,0.00457792f,0.00466053f,0.00504960f,0.00734666f,
    0.00923077f,0.00696355f,0.00376456f,0.00347199f,0.00318920f,0.00304106f,
    0.00312050f,0.00354617f,0.00364764f,0.00372251f,0.00401730f,0.00562341f,
    0.00571496f,0.00324942f,0.00277753f,0.00262954f,0.00264243f,0.00308280f,
    0.00387684f,0.00368729f,0.00360587f,0.00347121f,0.00337158f,0.00350060f
};

static const float s_R1C1_table[36] = {
    -0.12500000f,-0.12500000f,-0.12500000f,-0.12500000f,-0.12500000f,-0.12500000f,
    -0.12500000f,-0.12500000f,-0.12500000f,-0.12500000f,-0.12500000f,-0.12500000f,
    -0.15384616f,-0.15384616f,-0.15384616f,-0.15384616f,-0.15384616f,-0.15384616f,
    -0.15384616f,-0.15384616f,-0.15384616f,-0.15384616f,-0.15384616f,-0.15384616f,
    -0.18181819f,-0.18181819f,-0.18181819f,-0.18181819f,-0.18181819f,-0.18181819f,
    -0.18181819f,-0.18181819f,-0.18181819f,-0.18181819f,-0.18181819f,-0.18181819f
};

/* ── 2D bilinear interpolation ───────────────────────────────────────────── */
static float lut2d(float u0, float u1,
                   const float *bp0, uint32_t n0,
                   const float *bp1, uint32_t n1,
                   const float *table)
{
    uint32_t i0 = 0;
    float f0;
    if (u0 <= bp0[0]) {
        i0 = 0; f0 = 0.0f;
    } else if (u0 >= bp0[n0-1]) {
        i0 = n0 - 2; f0 = 1.0f;
    } else {
        for (i0 = 0; i0 < n0 - 2; i0++)
            if (u0 < bp0[i0+1]) break;
        f0 = (u0 - bp0[i0]) / (bp0[i0+1] - bp0[i0]);
    }

    uint32_t i1 = 0;
    float f1;
    if (u1 <= bp1[0]) {
        i1 = 0; f1 = 0.0f;
    } else if (u1 >= bp1[n1-1]) {
        i1 = n1 - 2; f1 = 1.0f;
    } else {
        for (i1 = 0; i1 < n1 - 2; i1++)
            if (u1 < bp1[i1+1]) break;
        f1 = (u1 - bp1[i1]) / (bp1[i1+1] - bp1[i1]);
    }

    float v00 = table[i0     + i1*n0];
    float v10 = table[i0 + 1 + i1*n0];
    float v01 = table[i0     + (i1+1)*n0];
    float v11 = table[i0 + 1 + (i1+1)*n0];

    return (v00 + (v10-v00)*f0) * (1.0f-f1)
         + (v01 + (v11-v01)*f0) * f1;
}

/* ── LUT accessors ───────────────────────────────────────────────────────── */
static inline float ocv_lut   (float soc, float T) { return lut2d(soc, T, s_SoC_bp_101, 101, s_T_bp, 3, s_OCV_table);  }
static inline float r0_lut   (float soc, float T) { return lut2d(soc, T, s_SoC_bp_12,   12, s_T_bp, 3, s_R0_table);   }
static inline float c1_lut   (float soc, float T) { return lut2d(soc, T, s_SoC_bp_12,   12, s_T_bp, 3, s_C1_table);   }
static inline float r1c1_lut (float soc, float T) { return lut2d(soc, T, s_SoC_bp_12,   12, s_T_bp, 3, s_R1C1_table); }

static inline float safe_inv_r1_from_luts(float inv_C1, float inv_tau1)
{
    float inv_C1_safe = inv_C1;

    if (inv_C1_safe < 1.0e-6f)
    {
        inv_C1_safe = 1.0e-6f;
    }

    float inv_R1 = (-inv_tau1) / inv_C1_safe;

    if (inv_R1 < 0.0f)
    {
        inv_R1 = 0.0f;
    }
    else if (inv_R1 > 1000.0f)
    {
        inv_R1 = 1000.0f;
    }

    return inv_R1;
}

/* ── Adaptive R update ───────────────────────────────────────────────────── */
#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_ADAPTIVE_R_EKF) || \
    (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_DAEKF)
static void update_R_adapt(adekf_state_t *ekf, float innov)
{
    ekf->innov_buf[ekf->innov_idx] = innov * innov;
    ekf->innov_idx = (uint8_t)((ekf->innov_idx + 1U) % ADEKF_ADAPT_WIN);

    float sum = 0.0f;
    for (uint8_t i = 0; i < ADEKF_ADAPT_WIN; i++)
        sum += ekf->innov_buf[i];

    ekf->R_adapt = sum / (float)ADEKF_ADAPT_WIN;
    if (ekf->R_adapt < 1e-6f) ekf->R_adapt = 1e-6f;
}
#endif

/* ══════════════════════════════════════════════════════════════════════════
 * adekf_init
 * ══════════════════════════════════════════════════════════════════════════ */

void adekf_init(adekf_state_t *ekf, float SoC_init, float R0_init)
{
    memset(ekf, 0, sizeof(*ekf));

    ekf->SoC   = SoC_init;
    ekf->Vp1   = 0.0f;
    ekf->Vp2   = 0.0f;

    ekf->P_soc = ADEKF_P0_SOC;
    ekf->P_vp1 = ADEKF_P0_VP1;
    ekf->P_vp2 = ADEKF_P0_VP2;

    ekf->R0    = R0_init;
    ekf->P_R0  = ADEKF_P0_R0;

    ekf->T_core_est = ADEKF_INIT_T_CORE_C;

    ekf->R_adapt = ADEKF_R_INIT;
    for (uint8_t i = 0; i < ADEKF_ADAPT_WIN; i++)
        ekf->innov_buf[i] = ADEKF_R_INIT;
}

#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_UKF)
/* ══════════════════════════════════════════════════════════════════════════
 * ukf_step_fixed_r0 (Labeled: diagonal-covariance UKF baseline)
 * ══════════════════════════════════════════════════════════════════════════ */
static void ukf_step_fixed_r0(adekf_state_t *ekf,
                              float I_pack,
                              float V_pack_meas,
                              float T_surf)
{
    enum {
        UKF_NS = 7
    };

    const float gamma = 1.0392304845413265f;
    const float wm0   = -1.7777777777777777f;
    const float wc0   =  0.8622222222222222f;
    const float wi    =  0.4629629629629630f;

    float T = ekf->T_core_est;
    if (T < 0.0f)  T = 0.0f;
    if (T > 25.0f) T = 25.0f;

    const float Ic = I_pack / ADEKF_PACK_P;

    float x_soc = ekf->SoC;
    if (x_soc < 0.0f) x_soc = 0.0f;
    if (x_soc > 1.0f) x_soc = 1.0f;

    const float UKF_SOC_EPS = 1.0e-4f;

    if (x_soc > (1.0f - UKF_SOC_EPS))
    {
        x_soc = 1.0f - UKF_SOC_EPS;
    }
    else if (x_soc < UKF_SOC_EPS)
    {
        x_soc = UKF_SOC_EPS;
    }

    const float x_vp1 = ekf->Vp1;
    const float x_vp2 = ekf->Vp2;

    float P_soc = ekf->P_soc;
    float P_vp1 = ekf->P_vp1;
    float P_vp2 = ekf->P_vp2;

    if (P_soc < 1e-12f) P_soc = 1e-12f;
    if (P_vp1 < 1e-12f) P_vp1 = 1e-12f;
    if (P_vp2 < 1e-12f) P_vp2 = 1e-12f;

    float sx_soc[UKF_NS];
    float sx_vp1[UKF_NS];
    float sx_vp2[UKF_NS];

    float d_soc = gamma * sqrtf(P_soc);
    const float d_vp1 = gamma * sqrtf(P_vp1);
    const float d_vp2 = gamma * sqrtf(P_vp2);

    float soc_room_up = 1.0f - x_soc;
    float soc_room_dn = x_soc;
    float soc_room = (soc_room_up < soc_room_dn) ? soc_room_up : soc_room_dn;
    float d_soc_max = 0.80f * soc_room;

    if (d_soc > d_soc_max)
    {
        d_soc = d_soc_max;
        float P_soc_limited = (d_soc / gamma) * (d_soc / gamma);
        if (P_soc_limited > 1.0e-12f)
        {
            P_soc = P_soc_limited;
        }
    }

    if (d_soc < 1.0e-7f)
    {
        d_soc = 1.0e-7f;
    }

    sx_soc[0] = x_soc;
    sx_vp1[0] = x_vp1;
    sx_vp2[0] = x_vp2;

    sx_soc[1] = x_soc + d_soc;
    sx_vp1[1] = x_vp1;
    sx_vp2[1] = x_vp2;

    sx_soc[2] = x_soc - d_soc;
    sx_vp1[2] = x_vp1;
    sx_vp2[2] = x_vp2;

    sx_soc[3] = x_soc;
    sx_vp1[3] = x_vp1 + d_vp1;
    sx_vp2[3] = x_vp2;

    sx_soc[4] = x_soc;
    sx_vp1[4] = x_vp1 - d_vp1;
    sx_vp2[4] = x_vp2;

    sx_soc[5] = x_soc;
    sx_vp1[5] = x_vp1;
    sx_vp2[5] = x_vp2 + d_vp2;

    sx_soc[6] = x_soc;
    sx_vp1[6] = x_vp1;
    sx_vp2[6] = x_vp2 - d_vp2;

    float xp_soc[UKF_NS];
    float xp_vp1[UKF_NS];
    float xp_vp2[UKF_NS];

    for (uint32_t i = 0U; i < UKF_NS; i++)
    {
        float s = sx_soc[i];
        if (s < 0.0f) s = 0.0f;
        if (s > 1.0f) s = 1.0f;

        float inv_C1   = c1_lut(s, T);
        float inv_tau1 = r1c1_lut(s, T);

        float soc_p = s - ADEKF_Q_NOM_INV * Ic * ADEKF_DT;
        if (soc_p < 0.0f) soc_p = 0.0f;
        if (soc_p > 1.0f) soc_p = 1.0f;

        xp_soc[i] = soc_p;
        xp_vp1[i] = sx_vp1[i] + (inv_C1 * Ic + inv_tau1 * sx_vp1[i]) * ADEKF_DT;
        xp_vp2[i] = sx_vp2[i] + (ADEKF_INV_C2 * Ic - ADEKF_INV_TAU2 * sx_vp2[i]) * ADEKF_DT;
    }

    float soc_m = wm0 * xp_soc[0];
    float vp1_m = wm0 * xp_vp1[0];
    float vp2_m = wm0 * xp_vp2[0];

    for (uint32_t i = 1U; i < UKF_NS; i++)
    {
        soc_m += wi * xp_soc[i];
        vp1_m += wi * xp_vp1[i];
        vp2_m += wi * xp_vp2[i];
    }

    if (soc_m < 0.0f) soc_m = 0.0f;
    if (soc_m > 1.0f) soc_m = 1.0f;

    float Psoc_p = ADEKF_Q_SOC;
    float Pvp1_p = ADEKF_Q_VP1;
    float Pvp2_p = ADEKF_Q_VP2;

    for (uint32_t i = 0U; i < UKF_NS; i++)
    {
        float w = (i == 0U) ? wc0 : wi;

        float ds = xp_soc[i] - soc_m;
        float d1 = xp_vp1[i] - vp1_m;
        float d2 = xp_vp2[i] - vp2_m;

        Psoc_p += w * ds * ds;
        Pvp1_p += w * d1 * d1;
        Pvp2_p += w * d2 * d2;
    }

    if (Psoc_p < 1e-12f) Psoc_p = 1e-12f;
    if (Pvp1_p < 1e-12f) Pvp1_p = 1e-12f;
    if (Pvp2_p < 1e-12f) Pvp2_p = 1e-12f;

    float z_sigma[UKF_NS];

    for (uint32_t i = 0U; i < UKF_NS; i++)
    {
        float s = xp_soc[i];
        if (s < 0.0f) s = 0.0f;
        if (s > 1.0f) s = 1.0f;

        z_sigma[i] = ADEKF_PACK_S *
                     (ocv_lut(s, T) - ekf->R0 * Ic - xp_vp1[i] - xp_vp2[i]);
    }

    float z_m = wm0 * z_sigma[0];

    for (uint32_t i = 1U; i < UKF_NS; i++)
    {
        z_m += wi * z_sigma[i];
    }

    float innov = V_pack_meas - z_m;

    const float R_meas = ADEKF_R_INIT;

    float Pzz = R_meas;
    float Pxz_soc = 0.0f;
    float Pxz_vp1 = 0.0f;
    float Pxz_vp2 = 0.0f;

    for (uint32_t i = 0U; i < UKF_NS; i++)
    {
        float w = (i == 0U) ? wc0 : wi;
        float dz = z_sigma[i] - z_m;

        Pzz += w * dz * dz;

        Pxz_soc += w * (xp_soc[i] - soc_m) * dz;
        Pxz_vp1 += w * (xp_vp1[i] - vp1_m) * dz;
        Pxz_vp2 += w * (xp_vp2[i] - vp2_m) * dz;
    }

    float inv_Pzz = (Pzz > 1e-10f) ? (1.0f / Pzz) : 0.0f;

    float K_soc = Pxz_soc * inv_Pzz;
    float K_vp1 = Pxz_vp1 * inv_Pzz;
    float K_vp2 = Pxz_vp2 * inv_Pzz;

    ekf->SoC = soc_m + K_soc * innov;
    ekf->Vp1 = vp1_m + K_vp1 * innov;
    ekf->Vp2 = vp2_m + K_vp2 * innov;

    if (ekf->SoC < 0.0f) ekf->SoC = 0.0f;
    if (ekf->SoC > 1.0f) ekf->SoC = 1.0f;

    ekf->P_soc = Psoc_p - K_soc * Pzz * K_soc;
    ekf->P_vp1 = Pvp1_p - K_vp1 * Pzz * K_vp1;
    ekf->P_vp2 = Pvp2_p - K_vp2 * Pzz * K_vp2;

    if (ekf->P_soc < 1e-12f) ekf->P_soc = 1e-12f;
    if (ekf->P_vp1 < 1e-12f) ekf->P_vp1 = 1e-12f;
    if (ekf->P_vp2 < 1e-12f) ekf->P_vp2 = 1e-12f;

    ekf->P_R0 = 0.0f;
    ekf->R_adapt = ADEKF_R_INIT;

    ekf->innovation = innov;
    ekf->V_pack_est = z_m;

    float s_th = soc_m;
    if (s_th < 0.0f) s_th = 0.0f;
    if (s_th > 1.0f) s_th = 1.0f;

    float inv_C1_th   = c1_lut(s_th, T);
    float inv_tau1_th = r1c1_lut(s_th, T);

    float inv_R1 = safe_inv_r1_from_luts(inv_C1_th, inv_tau1_th);


    float q_gen = Ic * Ic * ekf->R0
                + vp1_m * vp1_m * inv_R1
                + vp2_m * vp2_m * ADEKF_INV_R2;

    float q_cs = (ekf->T_core_est - T_surf) * ADEKF_INV_RCS;
    ekf->T_core_est += (q_gen - q_cs) * ADEKF_INV_CC * ADEKF_DT;

    if (ekf->T_core_est < -10.0f) ekf->T_core_est = -10.0f;
    if (ekf->T_core_est >  60.0f) ekf->T_core_est =  60.0f;
}
#endif

/* ══════════════════════════════════════════════════════════════════════════
 * adekf_step
 * ══════════════════════════════════════════════════════════════════════════ */
static void adekf_step_core(adekf_state_t *ekf,
                            float I_pack,
                            float V_pack_meas,
                            float T_surf,
                            bool hybrid_enable,
                            bms_nn_ctx_t *nn_ctx,
                            float dI_pack,
                            uint32_t plant_step,
                            bms_nn_output_t *nn_out)
{
    if (nn_out != NULL)
    {
        memset(nn_out, 0, sizeof(*nn_out));
    }

#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_UKF)
    ukf_step_fixed_r0(ekf, I_pack, V_pack_meas, T_surf);
    return;
#endif

    float T = ekf->T_core_est;
    if (T < 0.0f)  T = 0.0f;
    if (T > 25.0f) T = 25.0f;

    float Ic = I_pack / ADEKF_PACK_P;

    float soc = ekf->SoC;
    if (soc < 0.0f) soc = 0.0f;
    if (soc > 1.0f) soc = 1.0f;

    float inv_C1   = c1_lut(soc, T);
    float inv_tau1 = r1c1_lut(soc, T);

    /* ── CC Baseline Variant ────────────────────────────────────────────── */
#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_CC)

    float SoC_p = ekf->SoC - ADEKF_Q_NOM_INV * Ic * ADEKF_DT;
    if (SoC_p < 0.0f) SoC_p = 0.0f;
    if (SoC_p > 1.0f) SoC_p = 1.0f;

    float Vp1_p = ekf->Vp1 + (inv_C1 * Ic + inv_tau1 * ekf->Vp1) * ADEKF_DT;
    float Vp2_p = ekf->Vp2 + (ADEKF_INV_C2 * Ic - ADEKF_INV_TAU2 * ekf->Vp2) * ADEKF_DT;

    ekf->SoC = SoC_p;
    ekf->Vp1 = Vp1_p;
    ekf->Vp2 = Vp2_p;

    ekf->V_pack_est = ADEKF_PACK_S *
        (ocv_lut(ekf->SoC, T) - ekf->R0 * Ic - ekf->Vp1 - ekf->Vp2);

    ekf->innovation = V_pack_meas - ekf->V_pack_est;

    ekf->P_soc = 0.0f;
    ekf->P_vp1 = 0.0f;
    ekf->P_vp2 = 0.0f;
    ekf->P_R0  = 0.0f;
    ekf->R_adapt = ADEKF_R_INIT;

    float inv_R1 = safe_inv_r1_from_luts(inv_C1, inv_tau1);
    float q_gen  = Ic * Ic * ekf->R0
                 + Vp1_p * Vp1_p * inv_R1
                 + Vp2_p * Vp2_p * ADEKF_INV_R2;

    float q_cs = (ekf->T_core_est - T_surf) * ADEKF_INV_RCS;
    ekf->T_core_est += (q_gen - q_cs) * ADEKF_INV_CC * ADEKF_DT;

    if (ekf->T_core_est < -10.0f) ekf->T_core_est = -10.0f;
    if (ekf->T_core_est >  60.0f) ekf->T_core_est =  60.0f;

    return;

#else

    /* ── EKF-Family Variants (BASIC, ADAPTIVE_R, DAEKF) ─────────────────── */
#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_DAEKF)
    ekf->P_R0 += ADEKF_Q_R0;
#endif

    float SoC_p = ekf->SoC - ADEKF_Q_NOM_INV * Ic * ADEKF_DT;
    if (SoC_p < 0.0f) SoC_p = 0.0f;
    if (SoC_p > 1.0f) SoC_p = 1.0f;

    float Vp1_p = ekf->Vp1 + (inv_C1 * Ic + inv_tau1 * ekf->Vp1) * ADEKF_DT;
    float Vp2_p = ekf->Vp2 + (ADEKF_INV_C2 * Ic - ADEKF_INV_TAU2 * ekf->Vp2) * ADEKF_DT;

    float F_soc = 1.0f;
    float F_vp1 = 1.0f + inv_tau1 * ADEKF_DT;
    float F_vp2 = 1.0f - ADEKF_INV_TAU2 * ADEKF_DT;

    float Psoc_p = F_soc * F_soc * ekf->P_soc + ADEKF_Q_SOC;
    float Pvp1_p = F_vp1 * F_vp1 * ekf->P_vp1 + ADEKF_Q_VP1;
    float Pvp2_p = F_vp2 * F_vp2 * ekf->P_vp2 + ADEKF_Q_VP2;

    float V_est = ADEKF_PACK_S *
        (ocv_lut(SoC_p, T) - ekf->R0 * Ic - Vp1_p - Vp2_p);

    float innov_raw = V_pack_meas - V_est;
    float innov = innov_raw;

    bms_nn_output_t nn_local = {0};

    if (hybrid_enable && (nn_ctx != NULL))
    {
        /*
         * Important:
         * These are previous/pre-update EKF states because the state update
         * has not happened yet inside this function.
         */
        nn_local = bms_nn_residual_step(
            nn_ctx,
            I_pack,
            dI_pack,
            ekf->SoC,
            ekf->T_core_est,
            ekf->R0,
            ekf->Vp1,
            ekf->Vp2,
            innov_raw,
            plant_step
        );

        /*
         * NN predicts approximately V_meas - Vpred_base.
         * Corrected innovation = raw innovation - predicted residual.
         */
        innov = innov_raw - nn_local.pred_safe_v;
    }

    if (nn_out != NULL)
    {
        *nn_out = nn_local;
    }

    ekf->innovation_raw  = innov_raw;
    ekf->innovation_used = innov;
    ekf->innovation      = innov;
    ekf->dV_nn_safe      = nn_local.pred_safe_v;
    ekf->V_pack_est      = V_est;
    ekf->V_pack_est_corr = V_est + nn_local.pred_safe_v;


#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_BASIC_EKF)
    float R_meas = ADEKF_R_INIT;
    ekf->R_adapt = ADEKF_R_INIT;
#elif (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_ADAPTIVE_R_EKF) || \
      (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_DAEKF)
    update_R_adapt(ekf, innov);
    float R_meas = ekf->R_adapt;
#else
    float R_meas = ADEKF_R_INIT;
    ekf->R_adapt = ADEKF_R_INIT;
#endif

    /* ── Outer R0 Loop (DAEKF specific) ─────────────────────────────────── */
#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_DAEKF)
    float H_R0 = -ADEKF_PACK_S * Ic;
    float S_R0 = H_R0 * H_R0 * ekf->P_R0 + R_meas;
    float K_R0 = (S_R0 > 1e-10f) ? (ekf->P_R0 * H_R0 / S_R0) : 0.0f;

    ekf->R0  += K_R0 * innov;

    /* Scalar Joseph form for outer state covariance */
    float a_R0 = 1.0f - K_R0 * H_R0;
    ekf->P_R0 = (a_R0 * a_R0 * ekf->P_R0) + (K_R0 * K_R0 * R_meas);

    if (ekf->R0 < 0.001f) ekf->R0 = 0.001f;
    if (ekf->R0 > 0.200f) ekf->R0 = 0.200f;
#else
    ekf->P_R0 = 0.0f;
#endif

    /* ── Inner State Loop Update ────────────────────────────────────────── */
    float s_hi = SoC_p + ADEKF_DSOC;
    if (s_hi > 1.0f) s_hi = 1.0f;

    float s_lo = SoC_p - ADEKF_DSOC;
    if (s_lo < 0.0f) s_lo = 0.0f;

    float dOCV = (ocv_lut(s_hi, T) - ocv_lut(s_lo, T)) /
                 (s_hi - s_lo + 1e-10f);

    float H_soc = ADEKF_PACK_S * dOCV;
    float H_vp1 = -ADEKF_PACK_S;
    float H_vp2 = -ADEKF_PACK_S;

    float S_x = H_soc * H_soc * Psoc_p
              + H_vp1 * H_vp1 * Pvp1_p
              + H_vp2 * H_vp2 * Pvp2_p
              + R_meas;

    float inv_S = (S_x > 1e-10f) ? (1.0f / S_x) : 0.0f;

    float K_soc = Psoc_p * H_soc * inv_S;
    float K_vp1 = Pvp1_p * H_vp1 * inv_S;
    float K_vp2 = Pvp2_p * H_vp2 * inv_S;

    ekf->SoC = SoC_p + K_soc * innov;
    ekf->Vp1 = Vp1_p + K_vp1 * innov;
    ekf->Vp2 = Vp2_p + K_vp2 * innov;

    if (ekf->SoC < 0.0f) ekf->SoC = 0.0f;
    if (ekf->SoC > 1.0f) ekf->SoC = 1.0f;

    /* ── Diagonal Joseph-Form Covariance Measurement Update ────────────── */
    float a00 = 1.0f - K_soc * H_soc;
    float a01 =      - K_soc * H_vp1;
    float a02 =      - K_soc * H_vp2;

    float a10 =      - K_vp1 * H_soc;
    float a11 = 1.0f - K_vp1 * H_vp1;
    float a12 =      - K_vp1 * H_vp2;

    float a20 =      - K_vp2 * H_soc;
    float a21 =      - K_vp2 * H_vp1;
    float a22 = 1.0f - K_vp2 * H_vp2;

    ekf->P_soc = (a00 * a00 * Psoc_p) + (a01 * a01 * Pvp1_p) + (a02 * a02 * Pvp2_p)
               + (K_soc * K_soc * R_meas);

    ekf->P_vp1 = (a10 * a10 * Psoc_p) + (a11 * a11 * Pvp1_p) + (a12 * a12 * Pvp2_p)
               + (K_vp1 * K_vp1 * R_meas);

    ekf->P_vp2 = (a20 * a20 * Psoc_p) + (a21 * a21 * Pvp1_p) + (a22 * a22 * Pvp2_p)
               + (K_vp2 * K_vp2 * R_meas);

    if (ekf->P_soc < 1e-12f) ekf->P_soc = 1e-12f;
    if (ekf->P_vp1 < 1e-12f) ekf->P_vp1 = 1e-12f;
    if (ekf->P_vp2 < 1e-12f) ekf->P_vp2 = 1e-12f;

#if (ESTIMATOR_VARIANT == ESTIMATOR_VARIANT_DAEKF)
    if (ekf->P_R0 < 1e-14f) ekf->P_R0 = 1e-14f;
#endif

    /* ── Electro-Thermal Observer Back-Update ───────────────────────────── */
    float inv_R1 = safe_inv_r1_from_luts(inv_C1, inv_tau1);

    float q_gen = Ic * Ic * ekf->R0
                + Vp1_p * Vp1_p * inv_R1
                + Vp2_p * Vp2_p * ADEKF_INV_R2;

    float q_cs = (ekf->T_core_est - T_surf) * ADEKF_INV_RCS;
    ekf->T_core_est += (q_gen - q_cs) * ADEKF_INV_CC * ADEKF_DT;

    if (ekf->T_core_est < -10.0f) ekf->T_core_est = -10.0f;
    if (ekf->T_core_est >  60.0f) ekf->T_core_est =  60.0f;
#endif
}

void adekf_step(adekf_state_t *ekf,
                float I_pack,
                float V_pack_meas,
                float T_surf)
{
    adekf_step_core(ekf,
                    I_pack,
                    V_pack_meas,
                    T_surf,
                    false,
                    NULL,
                    0.0f,
                    0U,
                    NULL);
}

void adekf_step_hybrid_nn(adekf_state_t *ekf,
                          bms_nn_ctx_t *nn_ctx,
                          float I_pack,
                          float V_pack_meas,
                          float T_surf,
                          float dI_pack,
                          uint32_t plant_step,
                          bms_nn_output_t *nn_out)
{
    adekf_step_core(ekf,
                    I_pack,
                    V_pack_meas,
                    T_surf,
                    true,
                    nn_ctx,
                    dI_pack,
                    plant_step,
                    nn_out);
}
