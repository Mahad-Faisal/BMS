#include "new_thread0.h"
#include "ina226.h"
#include "bms.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "estimator_task_entry.h"
#include "r_sci_b_uart.h"
#include <stdbool.h>

/* ── Globals used by estimator UART send ─────────────────────────────── */
volatile bool uart_tx_complete = false;

/* ── Externs defined elsewhere ───────────────────────────────────────── */
extern SemaphoreHandle_t bms_mutex;
extern SemaphoreHandle_t plant_mailbox_mutex;  /* defined in can_task_entry.c */

/* ── Task entry prototypes ───────────────────────────────────────────── */
void sensor_task_entry(void *pvParameters);
void can_task_entry(void *pvParameters);
void uart_task_entry(void *pvParameters);      /* declared but NOT used */

/* ── Static mutex buffer for plant mailbox ───────────────────────────── */
static StaticSemaphore_t plant_mailbox_mutex_buf;

/* ── Stack overflow hook ─────────────────────────────────────────────── */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    volatile char *name = pcTaskName;
    (void) name;
    (void) xTask;

    __BKPT(0);

    while (1)
    {
        /* trap */
    }
}

/* ── UART callback ───────────────────────────────────────────────────── */
void uart_callback(uart_callback_args_t *p_args)
{
    if (p_args->event == UART_EVENT_TX_COMPLETE)
    {
        uart_tx_complete = true;
    }
}

/* ── Task stacks / TCBs ──────────────────────────────────────────────── */
static StackType_t can_stack[2048];
static StaticTask_t can_tcb;
TaskHandle_t can_task_handle;

static StaticTask_t s_est_tcb;
static StackType_t  s_est_stack[8192];
TaskHandle_t estimator_task_handle;

/* Optional / currently unused */
static StackType_t sensor_stack[1024];
static StaticTask_t sensor_tcb;
TaskHandle_t sensor_task_handle;

static StackType_t uart_stack[1024];
static StaticTask_t uart_tcb;
TaskHandle_t uart_task_handle;

/* ── Main FSP thread entry ───────────────────────────────────────────── */
void new_thread0_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Create BMS mutex */
    bms_mutex = xSemaphoreCreateMutexStatic(&bms_mutex_buffer);
    configASSERT(bms_mutex != NULL);

    /* Create plant mailbox mutex before CAN/estimator tasks start */
    plant_mailbox_mutex = xSemaphoreCreateMutexStatic(&plant_mailbox_mutex_buf);
    configASSERT(plant_mailbox_mutex != NULL);

    /* Open UART before estimator attempts CSV logging */
    fsp_err_t uart_err = R_SCI_B_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    configASSERT(uart_err == FSP_SUCCESS);

    /* Start CAN task first so mailbox begins filling */
    can_task_handle = xTaskCreateStatic(can_task_entry,
                                        "CAN",
                                        2048,
                                        NULL,
                                        4,
                                        can_stack,
                                        &can_tcb);
    configASSERT(can_task_handle != NULL);

    /* Start estimator task with larger stack for float snprintf */
    estimator_task_handle = xTaskCreateStatic(estimator_task_entry,
                                              "__estimator__",
                                              8192,
                                              NULL,
                                              3,
                                              s_est_stack,
                                              &s_est_tcb);
    configASSERT(estimator_task_handle != NULL);

    /* Do NOT start uart_task_entry while estimator owns UART */

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
