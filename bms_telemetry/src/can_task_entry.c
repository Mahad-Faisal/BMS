/*
 * can_task_entry.c
 *
 * TX (RA8M1 → bus):
 *   0x100 BMS_Pack    10Hz
 *   0x101 BMS_Status  10Hz
 *   0x102 BMS_Health  10Hz
 *   0x103 BMS_Limits  10Hz
 *
 * RX (ESP32 → RA8M1):
 *   0x200 MEAS   V_pack, I_pack, T_surf  → plant_mailbox (estimator input)
 *   0x201 TRUTH  SoC_true, T_core        → plant_mailbox (debug only)
 */

#include "new_thread0.h"
#include "bms.h"
#include "plant_mailbox.h"
#include "hal_data.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include <string.h>

/* ── CAN IDs ─────────────────────────────────────────────────────────────── */
#define CAN_ID_MEAS         0x200U
#define CAN_ID_TRUTH        0x201U
#define CAN_ID_BMS_PACK     0x100U
#define CAN_ID_BMS_STATUS   0x101U
#define CAN_ID_BMS_HEALTH   0x102U
#define CAN_ID_BMS_LIMITS   0x103U

#define CAN_FAST_PERIOD_MS  100U
#define CAN_SLOW_DIVIDER      1U

#define CAN_ID_PLANT_CTRL  0x300U
volatile uint32_t dbg_plant_reset_tx_ok  = 0;
volatile uint32_t dbg_plant_reset_tx_err = 0;


static fsp_err_t can_send(uint32_t id, uint8_t *data, uint8_t len);

static void send_plant_reset_cmd(void)
{
    uint8_t buf[8] = {
        0xA5U, 0x5AU, 0x52U,  /* reset magic */
        0x00U, 0x00U, 0x00U, 0x00U, 0x00U
    };

    fsp_err_t err = can_send(CAN_ID_PLANT_CTRL, buf, 8U);

    if (err == FSP_SUCCESS)
    {
        dbg_plant_reset_tx_ok++;
    }
    else
    {
        dbg_plant_reset_tx_err++;
    }
}

/* ── AFL: accept all standard frames ────────────────────────────────────── */
const canfd_afl_entry_t p_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM] =
{
    {
        .id   = { .id = 0x000, .frame_type = CAN_FRAME_TYPE_DATA,
                  .id_mode = CAN_ID_MODE_STANDARD },
        .mask = { .mask_id = 0x000, .mask_frame_type = 0, .mask_id_mode = 1 },
        .destination = { .minimum_dlc = CANFD_MINIMUM_DLC_0,
                         .rx_buffer   = CANFD_RX_MB_NONE,
                         .fifo_select_flags = CANFD_RX_FIFO_0 },
    },
};

/* ── Mailbox (extern'd in plant_mailbox.h) ───────────────────────────────── */
SemaphoreHandle_t    plant_mailbox_mutex = NULL;
plant_mailbox_t      plant_mailbox       = { 0 };
//static StaticSemaphore_t plant_mailbox_mutex_buf;

/* ── ISR → task RX queue ─────────────────────────────────────────────────── */
typedef struct { uint32_t id; uint8_t len; uint8_t data[8]; } can_raw_frame_t;

#define RX_QUEUE_DEPTH 8U
static StaticQueue_t s_rx_queue_buf;
static uint8_t       s_rx_queue_storage[RX_QUEUE_DEPTH * sizeof(can_raw_frame_t)];
static QueueHandle_t can_rx_queue;

/* ── BMS externals ───────────────────────────────────────────────────────── */
extern SemaphoreHandle_t bms_mutex;
extern bms_data_t        bms_data;

/* ── TX done flag ────────────────────────────────────────────────────────── */
static volatile bool canfd_tx_done = false;

/* ── Debug counters ──────────────────────────────────────────────────────── */
volatile uint32_t dbg_rx_events   = 0;
volatile uint32_t dbg_rx_matched  = 0;
volatile uint32_t dbg_rx_enq_ok   = 0;
volatile uint32_t dbg_rx_enq_fail = 0;
volatile uint32_t dbg_rx_other    = 0;
volatile uint32_t dbg_last_id     = 0;

/* ══════════════════════════════════════════════════════════════════════════
 * canfd0_callback — ISR
 * ══════════════════════════════════════════════════════════════════════════ */
void canfd0_callback(can_callback_args_t *p_args)
{
    BaseType_t hp_woken = pdFALSE;

    switch (p_args->event)
    {
        case CAN_EVENT_TX_COMPLETE:
            canfd_tx_done = true;
            break;

        case CAN_EVENT_RX_COMPLETE:
        {
            dbg_rx_events++;
            can_raw_frame_t f;
            f.id  = p_args->frame.id & 0x7FFU;
            f.len = p_args->frame.data_length_code;
            if (f.len > 8U) f.len = 8U;
            memcpy(f.data, p_args->frame.data, f.len);
            dbg_last_id = f.id;

            if (f.id == CAN_ID_MEAS || f.id == CAN_ID_TRUTH)
            {
                dbg_rx_matched++;
                if (xQueueSendFromISR(can_rx_queue, &f, &hp_woken) == pdPASS)
                    dbg_rx_enq_ok++;
                else
                    dbg_rx_enq_fail++;
            }
            else
            {
                dbg_rx_other++;
            }
            break;
        }

        case CAN_EVENT_ERR_BUS_OFF:
        case CAN_EVENT_ERR_CHANNEL:
            R_CANFD_Close(&g_canfd0_ctrl);
            R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);
            break;

        default:
            break;
    }

    portYIELD_FROM_ISR(hp_woken);
}

/* ══════════════════════════════════════════════════════════════════════════
 * process_rx_queue — drain ISR queue, decode into plant_mailbox
 * ══════════════════════════════════════════════════════════════════════════ */
static void process_rx_queue(void)
{
    can_raw_frame_t f;
    while (xQueueReceive(can_rx_queue, &f, 0) == pdTRUE)
    {
        if (xSemaphoreTake(plant_mailbox_mutex, 0) != pdTRUE)
            continue;

        if (f.id == CAN_ID_MEAS && f.len >= 7U)
            plant_mailbox_decode_meas(f.data, &plant_mailbox.meas);
        else if (f.id == CAN_ID_TRUTH && f.len >= 8U)
            plant_mailbox_decode_truth(f.data, &plant_mailbox.truth);

        xSemaphoreGive(plant_mailbox_mutex);
    }
}

/* ── TX helpers ──────────────────────────────────────────────────────────── */
static inline int16_t  sat_i16(float v) { return v>32767.0f?32767:v<-32767.0f?-32767:(int16_t)v; }
static inline uint16_t sat_u16(float v) { return v<0.0f?0:v>65535.0f?65535:(uint16_t)v; }

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
    if (err != FSP_SUCCESS) return err;

    for (uint32_t t = 0; t < 100U && !canfd_tx_done; t++)
        vTaskDelay(pdMS_TO_TICKS(1));

    return canfd_tx_done ? FSP_SUCCESS : FSP_ERR_TIMEOUT;
}

static void send_bms_pack(const bms_data_t *d)
{
    uint8_t buf[8];
    uint16_t v = sat_u16(d->voltage_v * 1000.0f);
    int16_t  i = sat_i16(d->current_a * 1000.0f);
    int16_t  p = sat_i16(d->power_w   * 1000.0f);
    uint8_t  s = (uint8_t)(d->soc_pct * 10.0f > 255.0f ? 255 : d->soc_pct * 10.0f);
    buf[0]=(uint8_t)(v>>8); buf[1]=(uint8_t)(v&0xFF);
    buf[2]=(uint8_t)((uint16_t)i>>8); buf[3]=(uint8_t)((uint16_t)i&0xFF);
    buf[4]=(uint8_t)((uint16_t)p>>8); buf[5]=(uint8_t)((uint16_t)p&0xFF);
    buf[6]=s; buf[7]=d->alive_ctr;
    can_send(CAN_ID_BMS_PACK, buf, 8);
}

static void send_bms_status(const bms_data_t *d)
{
    uint8_t buf[8] = {0};
    buf[0]=(uint8_t)d->state; buf[1]=d->fault_flags; buf[2]=(uint8_t)d->mode;
    can_send(CAN_ID_BMS_STATUS, buf, 8);
}

static void send_bms_health(const bms_data_t *d)
{
    uint8_t buf[8] = {0};
    uint16_t soh = sat_u16(d->soh_pct * 10.0f);
    uint16_t sop = sat_u16(d->sop_w);
    buf[0]=(uint8_t)(soh>>8); buf[1]=(uint8_t)(soh&0xFF);
    buf[2]=(uint8_t)(sop>>8); buf[3]=(uint8_t)(sop&0xFF);
    can_send(CAN_ID_BMS_HEALTH, buf, 8);
}

static void send_bms_limits(const bms_data_t *d)
{
    uint8_t buf[8] = {0};
    uint16_t chg = sat_u16(d->max_charge_a    * 1000.0f);
    uint16_t dis = sat_u16(d->max_discharge_a * 1000.0f);
    buf[0]=(uint8_t)(chg>>8); buf[1]=(uint8_t)(chg&0xFF);
    buf[2]=(uint8_t)(dis>>8); buf[3]=(uint8_t)(dis&0xFF);
    can_send(CAN_ID_BMS_LIMITS, buf, 8);
}

/* ══════════════════════════════════════════════════════════════════════════
 * can_task_entry — 10 Hz
 * ══════════════════════════════════════════════════════════════════════════ */
void can_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Create RX queue and mailbox mutex before opening peripheral */
    can_rx_queue = xQueueCreateStatic(RX_QUEUE_DEPTH,
                                      sizeof(can_raw_frame_t),
                                      s_rx_queue_storage,
                                      &s_rx_queue_buf);
    configASSERT(can_rx_queue);

  //  plant_mailbox_mutex = xSemaphoreCreateMutexStatic(&plant_mailbox_mutex_buf);
 //   configASSERT(plant_mailbox_mutex);

    R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);

    /* Give ESP32/MCP2515 side a moment, then send reset command several times. */
    vTaskDelay(pdMS_TO_TICKS(200));

    send_plant_reset_cmd();
    vTaskDelay(pdMS_TO_TICKS(50));
    send_plant_reset_cmd();
    vTaskDelay(pdMS_TO_TICKS(50));
    send_plant_reset_cmd();

    TickType_t xLastWake  = xTaskGetTickCount();
    uint32_t   slow_count = 0U;

    for (;;)
    {
        /* 1. Decode incoming ESP32 frames into plant_mailbox */
        process_rx_queue();

        /* 2. Snapshot estimator outputs */
        bms_data_t local;
        if (xSemaphoreTake(bms_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            local = bms_data;
            bms_data.alive_ctr++;
            xSemaphoreGive(bms_mutex);
        }
        else
        {
            vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(CAN_FAST_PERIOD_MS));
            continue;
        }

        /* 3. TX */
        send_bms_pack(&local);
        send_bms_status(&local);

        if (++slow_count >= CAN_SLOW_DIVIDER)
        {
            send_bms_health(&local);
            send_bms_limits(&local);
            slow_count = 0U;
        }

        vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(CAN_FAST_PERIOD_MS));
    }
}
