/*
 * adekf.h — Adaptive Dual Extended Kalman Filter
 *
 * Lumped 8s2p pack model (single equivalent cell scaled to pack level).
 *
 * ── State vector (inner EKF) ────────────────────────────────────────────────
 *
 *   x = [ SoC, Vp1, Vp2 ]^T        (3 states)
 *
 * ── Parameter vector (outer EKF) ────────────────────────────────────────────
 *
 *   θ = [ R0 ]                      (1 parameter, scalar)
 *   R1, C1, R2, C2 : fixed (R2/C2 not reliably identifiable from HPPC)
 *
 * ── Measurement ─────────────────────────────────────────────────────────────
 *
 *   z = V_pack  [V]
 *   h(x,θ) = 8 * (OCV(SoC, T_core_est) - R0*I_cell - Vp1 - Vp2)
 *
 * ── Inputs ──────────────────────────────────────────────────────────────────
 *
 *   I_pack  : pack current [A]  (+ve = discharge)
 *   T_surf  : surface temperature [°C] (measured, used by thermal observer)
 *
 * ── Temperature handling ─────────────────────────────────────────────────────
 *
 *   The plant drives all electrical LUTs with T_core (clamped to [0,25]°C),
 *   not T_surf. To match this, the estimator runs a forward thermal observer:
 *
 *     q_gen = I²R0 + Vp1²/R1 + Vp2²/R2          (heat generation)
 *     dT_cs = T_core_est - T_surf_meas             (core-to-surface ΔT)
 *     T_core_est[k+1] = T_core_est[k]
 *                     + (q_gen - dT_cs) * inv_Cc * dt
 *
 *   T_surf is measured directly from CAN so only T_core_est needs to be
 *   tracked. All electrical LUTs receive T_core_est clamped to [0,25]°C.
 *   This eliminates the T_surf / T_core mismatch of a naive implementation.
 * Thermal constants match the final tuned plant:
 *   Cc  = 180 J/K  -> inv_Cc  = 0.0055555556
 *   Rcs = 1.2 K/W  -> inv_Rcs = 0.8333333333
 *   R2  = 0.01 ohm -> inv_R2  = 100
 *
 * Thermal observer:
 *   q_gen = I^2*R0 + Vp1^2/R1 + Vp2^2/R2
 *   q_cs  = (T_core_est - T_surf_meas) / Rcs
 *   T_core_est[k+1] = T_core_est[k] + (q_gen - q_cs) * inv_Cc * dt

 * ── Adaptive component ──────────────────────────────────────────────────────
 *
 *   Measurement noise covariance R is updated online using a sliding window
 *   of recent innovations (innovation-energy based adaptive R).
 *   Process noise Q is fixed for version 1.
 */

#ifndef ESTIMATOR_ADEKF_ADEKF_H_
#define ESTIMATOR_ADEKF_ADEKF_H_

#include <stdint.h>
#include "bms_nn_residual_fp32.h"

/* ── Tuning — adjust during bring-up ────────────────────────────────────── */

/* Process noise (state EKF) */
#define ADEKF_Q_SOC     5e-7f   /* SoC random walk variance              */
#define ADEKF_Q_VP1     1e-5f   /* Vp1 process noise                     */
#define ADEKF_Q_VP2     1e-6f   /* Vp2 process noise                     */

/* Process noise (parameter EKF) */
#define ADEKF_Q_R0      1e-8f   /* R0 random walk variance               */

/* Measurement noise — initial value, adapted online */
#define ADEKF_R_INIT    0.01f   /* V^2 — fixed for Basic EKF, seed for adaptive modes */

/* Adaptive R window length (number of innovations to average) */
#define ADEKF_ADAPT_WIN 10U

/* Initial covariance */
#define ADEKF_P0_SOC    0.01f
#define ADEKF_P0_VP1    0.001f
#define ADEKF_P0_VP2    0.001f
#define ADEKF_P0_R0     1e-4f

/* OCV derivative delta for numerical Jacobian */
#define ADEKF_DSOC      1e-4f

/* ── Fixed plant parameters (from offline HPPC identification) ───────────── */

#define ADEKF_DT        0.1f
#define ADEKF_Q_NOM_INV 9.678668e-5f  /* 1/(3600*2.87 Ah), cell-capacity inverse */
#define ADEKF_PACK_S    8.0f
#define ADEKF_PACK_P    2.0f

#define ADEKF_INV_C2    0.000125f
#define ADEKF_INV_TAU2  0.0125f

#define ADEKF_INV_CC    0.0055555556f  /* 1/Cc, Cc = 180 J/K */
#define ADEKF_INV_RCS   0.8333333333f  /* 1/Rcs, Rcs = 1.2 K/W */
#define ADEKF_INV_R2    100.0f         /* 1/R2, R2 = 0.01 ohm */
/* ── State struct ────────────────────────────────────────────────────────── */
typedef struct {
    /* State estimate */
    float SoC;          /* [0, 1]  */
    float Vp1;          /* [V]     */
    float Vp2;          /* [V]     */

    /* State error covariance (diagonal only — 3x3 stored as 3 floats) */
    float P_soc;
    float P_vp1;
    float P_vp2;

    /* Parameter estimate */
    float R0;           /* [Ω]     */
    float P_R0;         /* parameter error covariance */

    /*
     * Thermal observer state.
     * T_core_est tracks the plant's core temperature using a forward model
     * driven by estimated electrical states and measured T_surf.
     * All electrical LUTs (OCV, R0, C1, R1C1) receive T_core_est clamped
     * to [0, 25]°C — matching how the plant internally drives its LUTs.
     * Init to 25.0f (matches plant DiscreteIntegrator IC).
     */
    float T_core_est;   /* [°C] estimated core temperature */

    /* Adaptive R state */
    float innov_buf[ADEKF_ADAPT_WIN];  /* sliding innovation window */
    uint8_t innov_idx;                  /* ring buffer index         */
    float R_adapt;                      /* current adapted R         */

    /* Outputs for logging */
    float innovation;        /* used innovation [V] */
    float innovation_raw;    /* raw physics innovation [V] = V_meas - Vpred_base */
    float innovation_used;   /* innovation after NN correction [V] */
    float dV_nn_safe;        /* final bounded NN correction [V] */
    float V_pack_est;        /* base physics predicted voltage [V] */
    float V_pack_est_corr;   /* V_pack_est + dV_nn_safe [V] */
} adekf_state_t;

/* ── API ─────────────────────────────────────────────────────────────────── */

/*
 * adekf_init()
 * Initialize state, covariance, parameters, and thermal observer.
 * SoC_init: initial SoC guess [0,1]
 * R0_init:  initial R0 guess [Ω]  (~0.035 Ω typical from HPPC)
 */
void adekf_init(adekf_state_t *ekf, float SoC_init, float R0_init);

/*
 * adekf_step()
 * Run one predict + update cycle.
 *
 * Inputs:
 *   I_pack      : pack current [A], positive = discharge
 *   V_pack_meas : measured pack terminal voltage [V]
 *   T_surf      : measured surface temperature [°C]
 *                 Used by thermal observer to derive T_core_est.
 *                 NOT directly used for LUT inputs.
 *
 * All LUT inputs use ekf->T_core_est (clamped), not T_surf.
 * ekf->T_core_est is updated each step from the thermal forward model.
 */
void adekf_step(adekf_state_t *ekf,
                float I_pack,
                float V_pack_meas,
                float T_surf);

void adekf_step_hybrid_nn(adekf_state_t *ekf,
                          bms_nn_ctx_t *nn_ctx,
                          float I_pack,
                          float V_pack_meas,
                          float T_surf,
                          float dI_pack,
                          uint32_t plant_step,
                          bms_nn_output_t *nn_out);

#endif /* ESTIMATOR_ADEKF_ADEKF_H_ */
