/*
 * bms_data.c
 *
 *  Created on: Mar 30, 2026
 *      Author: mahad
 */

#include "bms.h"

StaticSemaphore_t bms_mutex_buffer;
SemaphoreHandle_t bms_mutex;

bms_data_t bms_data =
{
    .voltage_v       = 0.0f,
    .current_a       = 0.0f,
    .power_w         = 0.0f,
    .soc_pct         = 100.0f,
    .soh_pct         = 100.0f,
    .sop_w           = 0.0f,
    .max_charge_a    = 0.0f,
    .max_discharge_a = 0.0f,
    .state           = BMS_STATE_INIT,
    .mode            = BMS_MODE_REAL,
    .fault_flags     = 0,
    .alive_ctr       = 0,
};
