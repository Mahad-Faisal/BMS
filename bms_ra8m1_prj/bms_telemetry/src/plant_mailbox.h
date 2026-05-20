/*
 * plant_mailbox.h
 *
 * Shared mailbox populated by CAN RX (0x200/0x201 from ESP32 plant node).
 * Read by estimator task. Protected by plant_mailbox_mutex.
 *
 * Frame decode mirrors ESP32 pack functions exactly:
 *
 *   0x200 MEAS:
 *     [0:1]  V_pack  uint16  ×0.001  → V
 *     [2:3]  I_pack  int16   ×0.01   → A
 *     [4:5]  T_surf  int16   ×0.1    → °C
 *     [6]    counter
 *     [7]    reserved
 *
 *   0x201 TRUTH:
 *     [0:1]  SoC_true  uint16  ×0.0001 → fraction 0–1
 *     [2:3]  T_core    int16   ×0.1    → °C
 *     [4]    counter
 *     [5:7]  plant_step uint24 big-endian, 10 Hz plant step counter
 */

#ifndef PLANT_MAILBOX_H_
#define PLANT_MAILBOX_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

/* ── Measurement frame 0x200 — estimator input ─────────────────────────── */
typedef struct {
    float    V_pack;       /* V   — pack terminal voltage */
    float    I_pack;       /* A   — pack current          */
    float    T_surf;       /* °C  — surface temperature   */
    uint8_t  counter;
    uint32_t rx_tick;      /* xTaskGetTickCount() at RX   */
    uint8_t  fresh;        /* 1 = new data since last estimator read */
} plant_meas_t;

/* ── Truth frame 0x201 — logging/paper plots only ──────────────────────── */
typedef struct {
    float    SoC_true;
    float    T_core;
    uint8_t  counter;
    uint32_t plant_step;
    uint32_t rx_tick;
    uint8_t  fresh;
} plant_truth_t;

/* ── Combined mailbox ───────────────────────────────────────────────────── */
typedef struct {
    plant_meas_t  meas;
    plant_truth_t truth;
} plant_mailbox_t;

/* ── Globals — defined in can_task_entry.c ──────────────────────────────── */
extern SemaphoreHandle_t plant_mailbox_mutex;
extern plant_mailbox_t   plant_mailbox;

/* ── Decode helpers ─────────────────────────────────────────────────────── */
static inline void plant_mailbox_decode_meas(const uint8_t *buf, plant_meas_t *out)
{
    uint16_t v_raw  = ((uint16_t)buf[0] << 8) | buf[1];
    int16_t  i_raw  = (int16_t)(((uint16_t)buf[2] << 8) | buf[3]);
    int16_t  ts_raw = (int16_t)(((uint16_t)buf[4] << 8) | buf[5]);

    out->V_pack   = (float)v_raw  * 0.001f;  /* mV -> V */
    out->I_pack   = (float)i_raw  * 0.01f;   /* 10 mA -> A */
    out->T_surf   = (float)ts_raw * 0.01f;   /* centi-C -> C */
    out->counter  = buf[6];
    out->rx_tick  = xTaskGetTickCount();
    out->fresh    = 1U;
}

static inline void plant_mailbox_decode_truth(const uint8_t *buf, plant_truth_t *out)
{
    uint16_t soc_raw = ((uint16_t)buf[0] << 8) | buf[1];
    int16_t  tc_raw  = (int16_t)(((uint16_t)buf[2] << 8) | buf[3]);

    uint32_t step24 =
        ((uint32_t)buf[5] << 16) |
        ((uint32_t)buf[6] << 8)  |
        ((uint32_t)buf[7]);

    out->SoC_true   = (float)soc_raw * 0.0001f;
    out->T_core     = (float)tc_raw  * 0.01f;   /* centi-C -> C */
    out->counter    = buf[4];
    out->plant_step = step24;
    out->rx_tick    = xTaskGetTickCount();
    out->fresh      = 1U;
}

#endif /* PLANT_MAILBOX_H_ */
