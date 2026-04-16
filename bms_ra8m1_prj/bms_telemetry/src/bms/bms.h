/*
 * bms.h
 *
 *  Created on: Mar 30, 2026
 *      Author: mahad
 */

#ifndef BMS_BMS_H_
#define BMS_BMS_H_

#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern StaticSemaphore_t bms_mutex_buffer;

/* ── Fault flags (bit field) ─────────────────────────────────────────────── */
typedef enum
{
    BMS_FAULT_OVERVOLTAGE   = (1 << 0),
    BMS_FAULT_OVERCURRENT   = (1 << 1),
    BMS_FAULT_SENSOR_LOSS   = (1 << 2),
    BMS_FAULT_UNDERVOLATGE  = (1 << 3),
    BMS_FAULT_OVERTEMP      = (1 << 4),
    BMS_FAULT_UNDERTEMP     = (1 << 5),
    BMS_FAULT_CAN_TIMEOUT   = (1 << 6),
} bms_fault_t;

/* ── BMS operating state machine ─────────────────────────────────────────── */
typedef enum
{
    BMS_STATE_INIT      = 0,
    BMS_STATE_STANDBY   = 1,
    BMS_STATE_DISCHARGE = 2,
    BMS_STATE_CHARGE    = 3,
    BMS_STATE_FAULT     = 4,
    BMS_STATE_SHUTDOWN  = 5,
} bms_state_t;

/* ── BMS operating mode ──────────────────────────────────────────────────── */
typedef enum
{
    BMS_MODE_REAL = 0,   /* INA226 hardware sensing */
    BMS_MODE_HIL  = 1,   /* Simulink ESP32 HIL      */
} bms_mode_t;

/* ── Shared BMS data struct ──────────────────────────────────────────────── */
typedef struct
{
    /* Measurements */
    float    voltage_v;       /* Pack terminal voltage       [V]  */
    float    current_a;       /* Pack current (+ = discharge)[A]  */
    float    power_w;         /* Pack power                  [W]  */

    /* Estimation */
    float    soc_pct;         /* State of Charge             [%]  */
    float    soh_pct;         /* State of Health             [%]  */
    float    sop_w;           /* State of Power (max)        [W]  */

    /* Limits */
    float    max_charge_a;    /* Max allowed charge current  [A]  */
    float    max_discharge_a; /* Max allowed discharge curr  [A]  */

    /* Status */
    bms_state_t state;        /* BMS state machine state          */
    bms_mode_t  mode;         /* REAL or HIL                      */
    uint8_t     fault_flags;  /* Bitfield of active faults        */
    uint8_t     alive_ctr;    /* Rolling counter 0-255            */

} bms_data_t;

extern bms_data_t bms_data;
extern SemaphoreHandle_t bms_mutex;

#endif /* BMS_BMS_H_ */
