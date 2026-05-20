/*
 * bms_nn_residual_fp32.h
 *
 * Runtime FP32 inference wrapper for the v9 bounded BMS NN voltage-residual model with clean-neutral innovation safeguard.
 *
 * The generated weight file must be present in the include path:
 *     bms_nn_residual_weights_fp32_c99_fixed.h
 *
 * Model:
 *     mlp_tdnn_48x24
 *     inputs: 15
 *     output: dV_NN residual voltage correction [V]
 *
 * Sign convention:
 *     dV_NN predicts approximately (V_meas - Vpred_physics).
 *     For hybrid EKF update:
 *         Vpred_corr     = Vpred_physics + dV_NN_safe;
 *         innovation_used = innovation_raw - dV_NN_safe;
 */

#ifndef BMS_NN_RESIDUAL_FP32_H_
#define BMS_NN_RESIDUAL_FP32_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Flags for bms_nn_output_t.flags */
#define BMS_NN_FLAG_GATE_WARMUP       (1UL << 0)
#define BMS_NN_FLAG_GATE_SOC          (1UL << 1)
#define BMS_NN_FLAG_GATE_TCORE        (1UL << 2)
#define BMS_NN_FLAG_GATE_CURRENT      (1UL << 3)
#define BMS_NN_FLAG_GATE_INNOV        (1UL << 4)
#define BMS_NN_FLAG_GATE_R0           (1UL << 5)
#define BMS_NN_FLAG_DEADBAND          (1UL << 6)
#define BMS_NN_FLAG_DYN_CLAMP         (1UL << 7)
#define BMS_NN_FLAG_RESET             (1UL << 8)
#define BMS_NN_FLAG_SIGN_GATE         (1UL << 9)
#define BMS_NN_FLAG_INNOV_RATIO_CLAMP (1UL << 10)
#define BMS_NN_FLAG_INNOV_MIN         (1UL << 11)

/* Current history length must cover max tap = 30 samples. */
#define BMS_NN_I_HIST_LEN             31U

typedef struct
{
    float i_hist[BMS_NN_I_HIST_LEN];   /* previous measured currents [A] */
    uint32_t head;                     /* next write index */
    uint32_t sample_count;             /* saturating-ish counter */
    float i_bias_60s;                  /* EMA current bias feature [A] */
} bms_nn_ctx_t;

typedef struct
{
    float pred_base_v;                 /* MLP output before output gain [V] */
    float pred_gain_v;                 /* after output gain + hard clamp [V] */
    float pred_safe_v;                 /* final deployment output [V] */
    float vlim_v;                      /* dynamic voltage limit [V] */
    uint32_t flags;                    /* BMS_NN_FLAG_* */
    uint8_t gate_allowed;              /* 1 if all gates pass */
} bms_nn_output_t;

void bms_nn_residual_reset(bms_nn_ctx_t *ctx);

/*
 * Run one v9 NN residual step.
 *
 * Inputs must be estimator-side values using the same units as training:
 *   i_pack_a          pack current [A], positive = discharge
 *   dI_pack_a         current delta [A] = I[k] - I[k-1]
 *   prev_soc          previous/current estimator SoC fraction [0,1]
 *   prev_tcore_c      estimator core temperature [degC]
 *   prev_r0_ohm       estimator R0 [ohm]
 *   prev_vp1_v        estimator Vp1 [V]
 *   prev_vp2_v        estimator Vp2 [V]
 *   innovation_raw_v  raw physics innovation [V] = V_meas - Vpred_physics
 *   plant_step        plant step counter from truth frame
 *
 * For shadow mode, call and log this output, but do not modify ADEKF update.
 * Clean-neutral safeguard:
 *   if out.pred_safe_v would increase |innovation| or exceed 65% of
 *   |innovation_raw_v|, it is zeroed/clamped before return.
 *
 * For hybrid mode:
 *   innovation_used = innovation_raw_v - out.pred_safe_v;
 */
bms_nn_output_t bms_nn_residual_step(bms_nn_ctx_t *ctx,
                                     float i_pack_a,
                                     float dI_pack_a,
                                     float prev_soc,
                                     float prev_tcore_c,
                                     float prev_r0_ohm,
                                     float prev_vp1_v,
                                     float prev_vp2_v,
                                     float innovation_raw_v,
                                     uint32_t plant_step);

#ifdef __cplusplus
}
#endif

#endif /* BMS_NN_RESIDUAL_FP32_H_ */
