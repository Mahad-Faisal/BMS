#ifndef BMS_NN_RESIDUAL_FP32_H_
#define BMS_NN_RESIDUAL_FP32_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * FP32 runtime API for the bounded Hybrid Gate065 / Gate065-DFN residual model.
 *
 * The DFN-aged training script uses:
 *   I_tap_0, I_tap_1, I_tap_2, I_tap_5, I_tap_10, I_tap_20, I_tap_30,
 *   I_bias_60s, I_drift_load_interaction, dI,
 *   prev_SoC, prev_Tcore_est, prev_R0, prev_Vp1, prev_Vp2
 *
 * I_bias_60s is implemented here as the exact 600-sample rolling mean used by
 * the v12 DFN-aged training script, not the old EMA approximation.
 */
#define BMS_NN_I_HIST_LEN       64U
#define BMS_NN_I_BIAS_WIN_LEN   600U

#define BMS_NN_FLAG_GATE_WARMUP       (1UL << 0)
#define BMS_NN_FLAG_GATE_SOC          (1UL << 1)
#define BMS_NN_FLAG_GATE_TCORE        (1UL << 2)
#define BMS_NN_FLAG_GATE_CURRENT      (1UL << 3)
#define BMS_NN_FLAG_GATE_INNOV        (1UL << 4)
#define BMS_NN_FLAG_GATE_R0           (1UL << 5)
#define BMS_NN_FLAG_DYN_CLAMP         (1UL << 6)
#define BMS_NN_FLAG_DEADBAND          (1UL << 7)
#define BMS_NN_FLAG_SIGN_GATE         (1UL << 8)
#define BMS_NN_FLAG_INNOV_RATIO_CLAMP (1UL << 9)
#define BMS_NN_FLAG_INNOV_MIN         (1UL << 10)
#define BMS_NN_FLAG_RESET             (1UL << 11)

typedef struct
{
    float i_hist[BMS_NN_I_HIST_LEN];
    uint32_t head;
    uint32_t sample_count;

    float i_bias_hist[BMS_NN_I_BIAS_WIN_LEN];
    uint32_t i_bias_head;
    uint32_t i_bias_count;
    float i_bias_sum;
    float i_bias_60s;
} bms_nn_ctx_t;

typedef struct
{
    float pred_base_v;   /* raw NN base output before output gain [V] */
    float pred_gain_v;   /* after BMS_NN_OUTPUT_GAIN and V_CLAMP [V] */
    float pred_safe_v;   /* after dynamic/gate/sign/ratio safety [V] */
    float vlim_v;        /* dynamic voltage authority [V] */
    uint32_t flags;
    uint8_t gate_allowed;
} bms_nn_output_t;

void bms_nn_residual_reset(bms_nn_ctx_t *ctx);

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
