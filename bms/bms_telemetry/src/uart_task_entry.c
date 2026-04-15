/*
 * uart_task_entry.c
 *
 *  Created on: Mar 31, 2026
 *      Author: mahad
 */
#include "new_thread0.h"
#include "bms.h"
#include "hal_data.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>

extern SemaphoreHandle_t bms_mutex;
extern bms_data_t        bms_data;
extern volatile bool     uart_tx_complete;

static void uart_print(const char *str)
{
    uart_tx_complete = false;
    R_SCI_B_UART_Write(&g_uart0_ctrl, (uint8_t *)str, strlen(str));
    while (!uart_tx_complete) { vTaskDelay(1); }
}

void uart_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    TickType_t xLastWake = xTaskGetTickCount();
    char buf[96];

    for (;;)
    {
        bms_data_t local;

        if (xSemaphoreTake(bms_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            local = bms_data;
            xSemaphoreGive(bms_mutex);
        }
        else
        {
            vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(250));
            continue;
        }

        /* CSV: V,I,P,SoC,SoH,SoP,state,faults,alive */
        snprintf(buf, sizeof(buf),
                 "%.3f,%.4f,%.3f,%.1f,%.1f,%.1f,%u,%u,%u\r\n",
                 local.voltage_v,
                 local.current_a,
                 local.power_w,
                 local.soc_pct,
                 local.soh_pct,
                 local.sop_w,
                 (unsigned)local.state,
                 local.fault_flags,
                 local.alive_ctr);

        uart_print(buf);

        vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(250));  /* 4 Hz */
    }
}
