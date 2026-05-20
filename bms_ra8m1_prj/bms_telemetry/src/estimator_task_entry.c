/*
 * estimator_task_entry.c — ADEKF estimator task (RA8M1)
 *
 * Rate: 10 Hz (matches ESP32 plant TX rate, must be synchronous)
 *
 * Each cycle:
 *   1. Read plant_mailbox under mutex — capture fresh flags BEFORE copy
 *   2. Stale check
 *   3. Run adekf_step() only if mutex was acquired and meas was fresh
 *      (guards against feeding zeroed struct on mutex timeout)
 *   4. Write bms_data under bms_mutex
 *   5. Send UART telemetry (outside WCET window)
 *
 * ── WCET measurement ─────────────────────────────────────────────────────────
 *
 *  Uses ARM DWT cycle counter (available on Cortex-M85 RA8M1).
 *  DWT must be enabled via CoreDebug before first use — done in task init.
 *  Cycle count → microseconds: us = cycles / CPU_MHz
 *  RA8M1 runs at 480 MHz → divide by 480.
 *  WCET accumulates worst-case only; reset by writing dbg_wcet_us = 0.
 *
 * ── UART log format (CSV, 115200 baud) ───────────────────────────────────────
 *
 *  t_ms,V_mV,I_mA,Tsurf_cC,SoC_ppm,Vp1_uV,Vp2_uV,R0_uohm,
 *  innov_uV,P_soc_e9,Tcore_cC,SoCtrue_ppm,Tcoretrue_cC,
 *  plant_step,wcet_us,Vpred_mV,dI_mA,Radapt_e9,dVnn_mV,nn_flags
 *
 *  T_core_est / Vpred_mV are estimator-side values.
 *  SoC_true / T_core_true are from 0x201 truth frame, NOT fed into EKF.
 *  Vpred_mV is the physics voltage prediction used to form innovation.
 *  dI_mA and Radapt_e9 are logged for AI residual feature generation.
 *  dVnn_mV and nn_flags are reserved for the future NN residual layer.
 */

#include "new_thread0.h"
#include "bms.h"
#include "plant_mailbox.h"
#include "adekf.h"
#include "hal_data.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "r_sci_b_uart.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "bms_nn_residual_fp32.h"

#define BMS_NN_MODE_SHADOW 0
#define BMS_NN_MODE_HYBRID 0

/* ARM DWT cycle counter — available on Cortex-M7/M33/M85 */
#define DWT_CYCCNT      (*((volatile uint32_t *)0xE0001004U))
#define DWT_CTRL        (*((volatile uint32_t *)0xE0001000U))
#define DEMCR           (*((volatile uint32_t *)0xE000EDFCU))
#define DWT_ENABLE()    do { DEMCR |= (1U << 24); DWT_CYCCNT = 0; DWT_CTRL |= 1U; } while(0)
#define DWT_READ()      (DWT_CYCCNT)

/* RA8M1 CPU frequency — must match actual SystemCoreClock */
#define CPU_MHZ         480U

/* ── Task rate ───────────────────────────────────────────────────────────── */
#define EST_PERIOD_MS   100U

/* ── Initial conditions ──────────────────────────────────────────────────── */
#define EST_SOC_INIT    1.0f
#define EST_R0_INIT     0.023f

/*
 * Alternate bring-up cases:
 *   Wrong SoC init: #define EST_SOC_INIT 0.80f
 *   Wrong R0 init:  #define EST_R0_INIT  0.060f
 */

/* ── Stale frame limit ───────────────────────────────────────────────────── */
#define EST_MAX_STALE   5U

/* ── Externals ───────────────────────────────────────────────────────────── */
extern SemaphoreHandle_t bms_mutex;
extern bms_data_t        bms_data;
extern const uart_instance_t g_uart0;
extern volatile bool uart_tx_complete;

/* ── Debug telemetry — inspect in Expressions view ──────────────────────── */
volatile uint32_t dbg_est_steps     = 0;
volatile uint32_t dbg_est_stale     = 0;
volatile uint32_t dbg_est_skipped   = 0;   /* adekf_step skipped (mutex fail) */
volatile uint32_t dbg_wcet_us       = 0;
volatile uint32_t dbg_wcet_cyc      = 0;
volatile float    dbg_soc_est       = 0.0f;
volatile float    dbg_R0_est        = 0.0f;
volatile float    dbg_innov         = 0.0f;
volatile float    dbg_T_core_est    = 0.0f;
volatile float    dbg_dI_pack       = 0.0f;


volatile uint32_t dbg_uart_ok        = 0;
volatile uint32_t dbg_uart_write_err = 0;
volatile uint32_t dbg_uart_timeout   = 0;


/* ── UART non-blocking write ─────────────────────────────────────────────── */
#define UART_TX_TIMEOUT_MS 25U

static void uart_send(const char *str)
{
    size_t len = strlen(str);
    if (len == 0U)
    {
        return;
    }

    uart_tx_complete = false;

    fsp_err_t err = R_SCI_B_UART_Write(&g_uart0_ctrl,
                                       (uint8_t const *) str,
                                       (uint32_t) len);

    if (err != FSP_SUCCESS)
    {
        dbg_uart_write_err++;
        return;
    }

    uint32_t waited_ms = 0U;

    while ((!uart_tx_complete) && (waited_ms < UART_TX_TIMEOUT_MS))
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        waited_ms++;
    }

    if (uart_tx_complete)
    {
        dbg_uart_ok++;
    }
    else
    {
        dbg_uart_timeout++;
    }
}

/* ══════════════════════════════════════════════════════════════════════════
 * estimator_task_entry
 * ══════════════════════════════════════════════════════════════════════════ */
void estimator_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    vTaskDelay(pdMS_TO_TICKS(500));

    DWT_ENABLE();

    /* Wait for ESP32 plant reset confirmation: fresh truth with small plant_step. */
    for (uint32_t wait_ms = 0U; wait_ms < 3000U; wait_ms += 50U)
    {
        bool synced = false;

        if (xSemaphoreTake(plant_mailbox_mutex, pdMS_TO_TICKS(5)) == pdTRUE)
        {
            if ((plant_mailbox.truth.fresh != 0U) &&
                (plant_mailbox.truth.plant_step <= 20U))
            {
                synced = true;
            }

            xSemaphoreGive(plant_mailbox_mutex);
        }

        if (synced)
        {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }

    static adekf_state_t s_ekf;
    static bms_nn_ctx_t s_nn_ctx;
    bms_nn_residual_reset(&s_nn_ctx);
    adekf_init(&s_ekf, EST_SOC_INIT, EST_R0_INIT);

    uart_send("t_ms,V_mV,I_mA,Tsurf_cC,"
              "SoC_ppm,Vp1_uV,Vp2_uV,R0_uohm,"
              "innov_raw_uV,innov_used_uV,"
              "P_soc_e9,P_vp1_e9,P_vp2_e9,P_R0_e12,"
              "Tcore_cC,SoCtrue_ppm,Tcoretrue_cC,"
              "plant_step,wcet_us,Vpred_base_mV,Vpred_corr_mV,"
              "dI_mA,Radapt_e9,dVnn_uV,nn_flags\r\n");

    TickType_t xLastWake   = xTaskGetTickCount();
    uint32_t   t_ms        = 0U;
    uint32_t   stale_count = 0U;
    uint32_t   wcet_cyc    = 0U;
    float      prev_I_pack = 0.0f;
    bool       prev_I_valid = false;
    float      dI_pack = 0.0f;
    bms_nn_output_t nn = {0};


    for (;;)
    {
        /* ── 1. Read mailbox ─────────────────────────────────────────────── */
        plant_meas_t  meas  = { 0 };
        plant_truth_t truth = { 0 };
        bool got_mailbox    = false;

        if (xSemaphoreTake(plant_mailbox_mutex, pdMS_TO_TICKS(5)) == pdTRUE)
        {
            /*
             * Capture fresh flags BEFORE struct copy.
             * The copy preserves .fresh as it was at take time, then we
             * clear the mailbox. Capturing separately makes intent explicit
             * and ensures we use the pre-clear value regardless of
             * compiler reordering of struct member copies.
             */
            uint8_t meas_fresh  = plant_mailbox.meas.fresh;
            uint8_t truth_fresh = plant_mailbox.truth.fresh;

            meas  = plant_mailbox.meas;
            truth = plant_mailbox.truth;

            plant_mailbox.meas.fresh  = 0U;
            plant_mailbox.truth.fresh = 0U;

            /* Restore captured flags into local copies for stale check */
            meas.fresh  = meas_fresh;
            truth.fresh = truth_fresh;

            xSemaphoreGive(plant_mailbox_mutex);
            got_mailbox = true;
        }

        /* ── 2. Stale check ──────────────────────────────────────────────── */
        if (!got_mailbox || !meas.fresh)
        {
            stale_count++;
            if (stale_count >= EST_MAX_STALE)
            {
                /* uart_send("WARN: stale CAN input\r\n"); */
            }
        }
        else
        {
            stale_count = 0U;
        }
        dbg_est_stale = stale_count;

        /* ── 3. Run ADEKF — WCET window ─────────────────────────────────── */
        /*
         * Guard: only step the filter if we got a valid measurement.
         * Skipping on mutex failure prevents zeroed inputs {0A, 0V, 0°C}
         * from injecting a large spurious innovation into the adaptive R
         * window and corrupting R0 adaptation.
         */
        if (got_mailbox && meas.fresh)
        {
            /*
             * Current difference for AI residual features.
             * Reset the delta at the beginning of a plant run so the first
             * sample after ESP32 reset does not create a false large dI.
             */
            if (truth.plant_step == 0U)
            {
                bms_nn_residual_reset(&s_nn_ctx);
                dI_pack = 0.0f;
                prev_I_valid = true;
            }
            else if (!prev_I_valid)
            {
                dI_pack = 0.0f;
                prev_I_valid = true;
            }
            else
            {
                dI_pack = meas.I_pack - prev_I_pack;
            }

            prev_I_pack = meas.I_pack;
            dbg_dI_pack = dI_pack;


            uint32_t t0 = DWT_READ();

            #if BMS_NN_MODE_HYBRID

            adekf_step_hybrid_nn(&s_ekf,
                                 &s_nn_ctx,
                                 meas.I_pack,
                                 meas.V_pack,
                                 meas.T_surf,
                                 dI_pack,
                                 truth.plant_step,
                                 &nn);

            #elif BMS_NN_MODE_SHADOW

            float prev_soc     = s_ekf.SoC;
            float prev_tcore_c = s_ekf.T_core_est;
            float prev_r0_ohm  = s_ekf.R0;
            float prev_vp1_v   = s_ekf.Vp1;
            float prev_vp2_v   = s_ekf.Vp2;

            adekf_step(&s_ekf, meas.I_pack, meas.V_pack, meas.T_surf);

            nn = bms_nn_residual_step(&s_nn_ctx,
                                      meas.I_pack,
                                      dI_pack,
                                      prev_soc,
                                      prev_tcore_c,
                                      prev_r0_ohm,
                                      prev_vp1_v,
                                      prev_vp2_v,
                                      s_ekf.innovation_raw,
                                      truth.plant_step);

            #else

            memset(&nn, 0, sizeof(nn));
            adekf_step(&s_ekf, meas.I_pack, meas.V_pack, meas.T_surf);

            #endif

            uint32_t cyc = DWT_READ() - t0;
            if (cyc > wcet_cyc) wcet_cyc = cyc;

            dbg_wcet_cyc  = wcet_cyc;
            dbg_wcet_us   = wcet_cyc / CPU_MHZ;
            dbg_soc_est   = s_ekf.SoC;
            dbg_R0_est    = s_ekf.R0;
            dbg_innov     = s_ekf.innovation;
            dbg_T_core_est = s_ekf.T_core_est;
            dbg_est_steps++;
        }
        else
        {
            dbg_est_skipped++;
        }

        /* ── 4. Write bms_data ───────────────────────────────────────────── */
        if (xSemaphoreTake(bms_mutex, pdMS_TO_TICKS(5)) == pdTRUE)
        {
            if (got_mailbox && meas.fresh)
            {
                bms_data.voltage_v       = meas.V_pack;
                bms_data.current_a       = meas.I_pack;
                bms_data.power_w         = meas.V_pack * meas.I_pack;
                bms_data.soc_pct         = s_ekf.SoC * 100.0f;
                bms_data.soh_pct         = 100.0f;
                bms_data.sop_w           = 0.0f;
                bms_data.max_charge_a    = 15.0f;
                bms_data.max_discharge_a = 30.0f;
                bms_data.state           = (meas.I_pack > 0.0f) ? BMS_STATE_DISCHARGE :
                                           (meas.I_pack < 0.0f) ? BMS_STATE_CHARGE :
                                                                   BMS_STATE_STANDBY;
                bms_data.mode            = BMS_MODE_HIL;
            }

            bms_data.fault_flags = (stale_count >= EST_MAX_STALE) ?
                                   BMS_FAULT_CAN_TIMEOUT : 0U;

            xSemaphoreGive(bms_mutex);
        }

        /* ── 5. UART telemetry — outside WCET window ─────────────────────── */
        uint32_t us_this = (wcet_cyc > 0U) ? (wcet_cyc / CPU_MHZ) : 0U;

        if (got_mailbox && meas.fresh)
        {
            char line[512];

            int32_t V_mV        = (int32_t)(meas.V_pack * 1000.0f);
            int32_t I_mA        = (int32_t)(meas.I_pack * 1000.0f);
            int32_t Tsurf_cC    = (int32_t)(meas.T_surf * 100.0f);

            int32_t SoC_ppm     = (int32_t)(s_ekf.SoC * 1000000.0f);
            int32_t Vp1_uV      = (int32_t)(s_ekf.Vp1 * 1000000.0f);
            int32_t Vp2_uV      = (int32_t)(s_ekf.Vp2 * 1000000.0f);
            int32_t R0_uohm     = (int32_t)(s_ekf.R0 * 1000000.0f);

            int32_t innov_raw_uV  = (int32_t)(s_ekf.innovation_raw  * 1000000.0f);
            int32_t innov_used_uV = (int32_t)(s_ekf.innovation_used * 1000000.0f);

            int32_t Psoc_e9     = (int32_t)(s_ekf.P_soc * 1000000000.0f);
            int32_t Pvp1_e9     = (int32_t)(s_ekf.P_vp1 * 1000000000.0f);
            int32_t Pvp2_e9     = (int32_t)(s_ekf.P_vp2 * 1000000000.0f);
            int32_t PR0_e12     = (int32_t)(s_ekf.P_R0  * 1000000000000.0f);

            int32_t Tcore_cC    = (int32_t)(s_ekf.T_core_est * 100.0f);
            int32_t SoCtr_ppm   = (int32_t)(truth.SoC_true * 1000000.0f);
            int32_t Tcoretr_cC  = (int32_t)(truth.T_core * 100.0f);

            int32_t Vpred_base_mV = (int32_t)(s_ekf.V_pack_est      * 1000.0f);
            int32_t Vpred_corr_mV = (int32_t)(s_ekf.V_pack_est_corr * 1000.0f);

            int32_t dI_mA       = (int32_t)(dI_pack * 1000.0f);
            int32_t Radapt_e9   = (int32_t)(s_ekf.R_adapt * 1000000000.0f);
            int32_t dVnn_uV     = (int32_t)(s_ekf.dV_nn_safe * 1000000.0f);
            uint32_t nn_flags   = nn.flags;

            snprintf(line, sizeof(line),
                     "%lu,%ld,%ld,%ld,"
                     "%ld,%ld,%ld,%ld,"
                     "%ld,%ld,"
                     "%ld,%ld,%ld,%ld,"
                     "%ld,%ld,%ld,"
                     "%lu,%lu,%ld,%ld,"
                     "%ld,%ld,%ld,%lu\r\n",
                     (unsigned long)t_ms,
                     (long)V_mV,
                     (long)I_mA,
                     (long)Tsurf_cC,
                     (long)SoC_ppm,
                     (long)Vp1_uV,
                     (long)Vp2_uV,
                     (long)R0_uohm,
                     (long)innov_raw_uV,
                     (long)innov_used_uV,
                     (long)Psoc_e9,
                     (long)Pvp1_e9,
                     (long)Pvp2_e9,
                     (long)PR0_e12,
                     (long)Tcore_cC,
                     (long)SoCtr_ppm,
                     (long)Tcoretr_cC,
                     (unsigned long)truth.plant_step,
                     (unsigned long)us_this,
                     (long)Vpred_base_mV,
                     (long)Vpred_corr_mV,
                     (long)dI_mA,
                     (long)Radapt_e9,
                     (long)dVnn_uV,
                     (unsigned long)nn_flags);

            uart_send(line);
        }
        t_ms += EST_PERIOD_MS;
        vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(EST_PERIOD_MS));
    }
}
