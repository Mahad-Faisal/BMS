/*
 * can_task_entry.c
 *
 *  Created on: Mar 31, 2026
 *      Author: mahad
 *
 * CAN message architecture (Classic CAN 2.0, 8-byte frames):
 *
 *  ID    Name            Rate    Contents
 *  ────────────────────────────────────────────────────────────────
 *  0x100 BMS_Pack        10Hz    voltage_mv[2], current_ma[2],
 *                                power_mw[2], soc_d1[1], alive[1]
 *  0x101 BMS_Status      10Hz    state[1], faults[1], mode[1],
 *                                reserved[5]
 *  0x102 BMS_Health     100ms    soh_d1[2], sop_w[2],
 *                                reserved[4]
 *  0x103 BMS_Limits     100ms    max_chg_ma[2], max_dis_ma[2],
 *                                reserved[4]
 *
 * Scaling:
 *  voltage  → mV    (uint16, 0–65535mV)
 *  current  → mA    (int16,  ±32767mA)
 *  power    → mW    (int16,  ±32767mW — saturate if needed)
 *  SoC      → 0.1%  (uint8,  0–1000, divide by 10 on receiver)
 *  SoH      → 0.1%  (uint16, 0–1000)
 *  SoP      → W     (uint16, 0–65535W)
 *  currents → mA    (uint16, 0–65535mA)
 */

#include "new_thread0.h"
#include "bms.h"
#include "hal_data.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>

/* ── CAN message IDs ─────────────────────────────────────────────────────── */
#define CAN_ID_BMS_PACK     0x100U
#define CAN_ID_BMS_STATUS   0x101U
#define CAN_ID_BMS_HEALTH   0x102U
#define CAN_ID_BMS_LIMITS   0x103U

/* ── Task timing ─────────────────────────────────────────────────────────── */
#define CAN_FAST_PERIOD_MS   100U   /* 10 Hz — Pack + Status    */
#define CAN_SLOW_PERIOD_MS   100U   /* 10 Hz — Health + Limits  */
#define CAN_SLOW_DIVIDER       1U   /* set to 10 for true 1Hz   */

/* ── CAN FD Acceptance Filter List ──────────────────────────────────────── */
const canfd_afl_entry_t p_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM] =
{
    {
        .id =
        {
            .id         = 0x000,
            .frame_type = CAN_FRAME_TYPE_DATA,
            .id_mode    = CAN_ID_MODE_STANDARD,
        },
        .mask =
        {
            .mask_id         = 0x000,
            .mask_frame_type = 0,
            .mask_id_mode    = 1,
        },
        .destination =
        {
            .minimum_dlc = CANFD_MINIMUM_DLC_0,
            .rx_buffer   = CANFD_RX_MB_0,
        },
    },
};

extern SemaphoreHandle_t bms_mutex;
extern bms_data_t        bms_data;

static volatile bool canfd_tx_done = false;

/* ── TX complete callback ────────────────────────────────────────────────── */
void canfd0_callback(can_callback_args_t *p_args)
{
    if (p_args->event == CAN_EVENT_TX_COMPLETE)
        canfd_tx_done = true;

    if (p_args->event == CAN_EVENT_ERR_BUS_OFF ||
        p_args->event == CAN_EVENT_ERR_CHANNEL)
    {
        R_CANFD_Close(&g_canfd0_ctrl);
        R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);
    }
}
/* ── Helper: saturate float to int16 range ───────────────────────────────── */
static inline int16_t saturate_i16(float val)
{
    if (val >  32767.0f) return  32767;
    if (val < -32767.0f) return -32767;
    return (int16_t)val;
}

/* ── Helper: saturate float to uint16 range ──────────────────────────────── */
static inline uint16_t saturate_u16(float val)
{
    if (val < 0.0f)      return 0;
    if (val > 65535.0f)  return 65535;
    return (uint16_t)val;
}

/* ── Send a classic CAN 2.0 frame ────────────────────────────────────────── */
static fsp_err_t can_send(uint32_t id, uint8_t *data, uint8_t len)
{
    can_frame_t frame;
    memset(&frame, 0, sizeof(frame));

    frame.id               = id;
    frame.id_mode          = CAN_ID_MODE_STANDARD;
    frame.type             = CAN_FRAME_TYPE_DATA;
    frame.data_length_code = len;
    frame.options          = 0;

    memcpy(frame.data, data, len);

    canfd_tx_done = false;
    fsp_err_t err = R_CANFD_Write(&g_canfd0_ctrl, 0, &frame);

    /* ADD BREAKPOINT HERE — check err value */
    if (err != FSP_SUCCESS) return err;

    for (uint32_t t = 0; t < 100 && !canfd_tx_done; t++)
        vTaskDelay(pdMS_TO_TICKS(1));

    return canfd_tx_done ? FSP_SUCCESS : FSP_ERR_TIMEOUT;
}

/* ── Pack and send BMS_Pack (0x100) ──────────────────────────────────────── */
static void send_bms_pack(const bms_data_t *d)
{
    uint8_t buf[8];

    uint16_t voltage_mv = saturate_u16(d->voltage_v * 1000.0f);
    int16_t  current_ma = saturate_i16(d->current_a * 1000.0f);
    int16_t  power_mw   = saturate_i16(d->power_w   * 1000.0f);
    uint8_t  soc_d1     = (uint8_t)(d->soc_pct * 10.0f > 255.0f ? 255 :
                                     d->soc_pct * 10.0f);

    buf[0] = (uint8_t)(voltage_mv >> 8);
    buf[1] = (uint8_t)(voltage_mv & 0xFF);
    buf[2] = (uint8_t)((uint16_t)current_ma >> 8);
    buf[3] = (uint8_t)((uint16_t)current_ma & 0xFF);
    buf[4] = (uint8_t)((uint16_t)power_mw >> 8);
    buf[5] = (uint8_t)((uint16_t)power_mw & 0xFF);
    buf[6] = soc_d1;
    buf[7] = d->alive_ctr;

    can_send(CAN_ID_BMS_PACK, buf, 8);
}

/* ── Pack and send BMS_Status (0x101) ────────────────────────────────────── */
static void send_bms_status(const bms_data_t *d)
{
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));

    buf[0] = (uint8_t)d->state;
    buf[1] = d->fault_flags;
    buf[2] = (uint8_t)d->mode;
    /* buf[3..7] reserved */

    can_send(CAN_ID_BMS_STATUS, buf, 8);
}

/* ── Pack and send BMS_Health (0x102) ────────────────────────────────────── */
static void send_bms_health(const bms_data_t *d)
{
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));

    uint16_t soh_d1 = saturate_u16(d->soh_pct * 10.0f);
    uint16_t sop_w  = saturate_u16(d->sop_w);

    buf[0] = (uint8_t)(soh_d1 >> 8);
    buf[1] = (uint8_t)(soh_d1 & 0xFF);
    buf[2] = (uint8_t)(sop_w  >> 8);
    buf[3] = (uint8_t)(sop_w  & 0xFF);
    /* buf[4..7] reserved for cycle count later */

    can_send(CAN_ID_BMS_HEALTH, buf, 8);
}

/* ── Pack and send BMS_Limits (0x103) ────────────────────────────────────── */
static void send_bms_limits(const bms_data_t *d)
{
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));

    uint16_t max_chg_ma = saturate_u16(d->max_charge_a    * 1000.0f);
    uint16_t max_dis_ma = saturate_u16(d->max_discharge_a * 1000.0f);

    buf[0] = (uint8_t)(max_chg_ma >> 8);
    buf[1] = (uint8_t)(max_chg_ma & 0xFF);
    buf[2] = (uint8_t)(max_dis_ma >> 8);
    buf[3] = (uint8_t)(max_dis_ma & 0xFF);
    /* buf[4..7] reserved */

    can_send(CAN_ID_BMS_LIMITS, buf, 8);
}

/* ── CAN task ─────────────────────────────────────────────────────────────── */
void can_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);

    TickType_t xLastWake  = xTaskGetTickCount();
    uint32_t   slow_count = 0;

    for (;;)
    {
        /* Force bus-off recovery every cycle */
        R_CANFD_Close(&g_canfd0_ctrl);
        R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);
        bms_data_t local;


        if (xSemaphoreTake(bms_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            local = bms_data;
            bms_data.alive_ctr++;   /* increment under mutex */
            xSemaphoreGive(bms_mutex);
        }
        else
        {
            vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(CAN_FAST_PERIOD_MS));
            continue;
        }

        /* Fast messages — every cycle */
        send_bms_pack(&local);
        send_bms_status(&local);

        /* Slow messages — every CAN_SLOW_DIVIDER cycles */
        slow_count++;
        if (slow_count >= CAN_SLOW_DIVIDER)
        {
            send_bms_health(&local);
            send_bms_limits(&local);
            slow_count = 0;
        }

        vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(CAN_FAST_PERIOD_MS));
    }
}
