/*
 * bms_nn_residual_int8_hardtanh.h
 *
 * Scalar INT8 HardTanh backend for the bounded BMS neural voltage-residual
 * layer.
 *
 * This is the validation backend before CMSIS-NN/MVE replacement:
 *   - INT8 input activations
 *   - INT8 weights
 *   - INT32 accumulators/biases
 *   - HardTanh as Q7 clamp [-127, 127]
 *
 * Public API intentionally matches the FP32/tanh wrapper so adekf.c can switch
 * backend by changing the included header/source file.
 */

#ifndef BMS_NN_RESIDUAL_INT8_HARDTANH_H_
#define BMS_NN_RESIDUAL_INT8_HARDTANH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "bms_nn_residual_weights_int8_hardtanh.h"

/* Flags for bms_nn_output_t.flags. Keep identical to the FP32 wrapper. */
#define BMS_NN_FLAG_GATE_WARMUP       (1UL << 0)
#define BMS_NN_FLAG_GATE_SOC          (1UL << 1)
#define BMS_NN_FLAG_GATE_TCORE        (1UL << 2)
#define BMS_NN_FLAG_GATE_CURRENT      (1UL << 3)
#define BMS_NN_FLAG_GATE_INNOV        (1UL << 4)
#define BMS_NN_FLAG_GATE_R0           (1UL << 5)
#define BMS_NN_FLAG_DEADBAND          (1UL << 6)
#define BMS_NN_FLAG_DYN_CLAMP         (1UL << 7)
#define BMS_NN_FLAG_RESET             (1UL << 8)
#define BMS_NN_FLAG_RATIO_CLAMP       (1UL << 9)
#define BMS_NN_FLAG_SIGN_MISMATCH     (1UL << 10)

/* Current history length must cover max tap = 30 samples. */
#define BMS_NN_I_HIST_LEN             31U

/* Public shape/complexity for telemetry and compile-time checks. */
#define BMS_NN_INT8_HT_N_IN           ((uint32_t)BMS_NN_Q_N_IN)
#define BMS_NN_INT8_HT_H1             ((uint32_t)BMS_NN_Q_H1)
#define BMS_NN_INT8_HT_H2             ((uint32_t)BMS_NN_Q_H2)
#define BMS_NN_INT8_HT_PARAMETERS     ((uint32_t)((BMS_NN_Q_N_IN * BMS_NN_Q_H1) + BMS_NN_Q_H1 + \
                                                  (BMS_NN_Q_H1 * BMS_NN_Q_H2) + BMS_NN_Q_H2 + \
                                                  BMS_NN_Q_H2 + 1))
#define BMS_NN_INT8_HT_MACS           ((uint32_t)((BMS_NN_Q_N_IN * BMS_NN_Q_H1) + \
                                                  (BMS_NN_Q_H1 * BMS_NN_Q_H2) + \
                                                  BMS_NN_Q_H2))

typedef struct
{
    float i_hist[BMS_NN_I_HIST_LEN];
    uint32_t head;
    uint32_t sample_count;
    float i_bias_60s;
} bms_nn_ctx_t;

typedef struct
{
    float pred_base_v;                 /* INT8 NN output before output gain [V] */
    float pred_gain_v;                 /* after output gain + static clamp [V] */
    float pred_safe_v;                 /* final deployment output [V] */
    float vlim_v;                      /* dynamic voltage authority limit [V] */
    uint32_t flags;
    uint8_t gate_allowed;
} bms_nn_output_t;

typedef struct
{
    int8_t xq[BMS_NN_Q_N_IN];
    int8_t a1q[BMS_NN_Q_H1];
    int8_t a2q[BMS_NN_Q_H2];
    int32_t acc3;
    float pred_base_v;
    float pred_raw_v;
} bms_nn_int8_hardtanh_debug_t;

void bms_nn_residual_reset(bms_nn_ctx_t *ctx);

/*
 * Direct normalized-vector inference helper for C/Python validation.
 *
 * x_norm must already be normalized and clipped the same way as the exported
 * test-vector CSV columns.
 *
 * Returns raw post-output-gain prediction in volts. pred_base_v is optional.
 */
float bms_nn_residual_int8_hardtanh_forward_norm(const float x_norm[BMS_NN_Q_N_IN],
                                                 float *pred_base_v,
                                                 bms_nn_int8_hardtanh_debug_t *debug);

/* Runtime wrapper matching the FP32 API. */
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

#endif /* BMS_NN_RESIDUAL_INT8_HARDTANH_H_ */
