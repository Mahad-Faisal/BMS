#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "single_cell_battery_phaseC_backup.h"
#include <stdio.h>

static RT_MODEL_single_cell_battery__T plant_M;
static DW_single_cell_battery_phaseC_T plant_DW;

volatile float g_V_pack   = 0.0f;
volatile float g_T_core   = 25.0f;
volatile float g_T_surf   = 25.0f;
volatile float g_SoC_true = 1.0f;

static void plant_task(void *pvParameters)
{
    plant_M.dwork = &plant_DW;
    single_cell_battery_phaseC_backup_initialize(&plant_M);

    const TickType_t period = pdMS_TO_TICKS(100);
    TickType_t xLastWakeTime = xTaskGetTickCount();

  /*  while (1) {
        single_cell_battery_phaseC_backup_step(
            &plant_M,
            5.0f,   // I_pack: 5A discharge
            25.0f,  // T_amb
            (float*)&g_V_pack,
            (float*)&g_T_core,
            (float*)&g_T_surf,
            (float*)&g_SoC_true
        );

        printf("V=%.4f SoC=%.4f Tc=%.2f Ts=%.2f\n",
               g_V_pack, g_SoC_true, g_T_core, g_T_surf);

        vTaskDelayUntil(&xLastWakeTime, period);
    }*/
	
	while (1) {
	    static uint32_t step = 0;
	    float I_pack;

	    if      (step < 100)  I_pack = 0.0f;
	    else if (step < 110)  I_pack = 30.0f;
	    else if (step < 150)  I_pack = 0.0f;
	    else if (step < 160)  I_pack = -15.0f;
	    else                  I_pack = 0.0f;

	    single_cell_battery_phaseC_backup_step(
	        &plant_M,
	        I_pack,
	        25.0f,
	        (float*)&g_V_pack,
	        (float*)&g_T_core,
	        (float*)&g_T_surf,
	        (float*)&g_SoC_true
	    );

	    printf("t=%lu I=%.1f V=%.4f SoC=%.4f Tc=%.2f Ts=%.2f\n",
	           step, I_pack, g_V_pack, g_SoC_true, g_T_core, g_T_surf);

	    step++;
	    vTaskDelayUntil(&xLastWakeTime, period);
	}
}

void app_main(void)
{
    xTaskCreatePinnedToCore(plant_task, "plant", 4096, NULL, 5, NULL, 0);
}