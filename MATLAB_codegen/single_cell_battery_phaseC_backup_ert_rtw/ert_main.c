/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: ert_main.c
 *
 * Code generated for Simulink model 'single_cell_battery_phaseC_backup'.
 *
 * Model version                  : 1.60
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Wed Apr 15 19:56:24 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom Processor
 * Emulation hardware selection:
 *    Differs from embedded hardware (Custom Processor->MATLAB Host Computer)
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. ROM efficiency
 * Validation result: All passed
 */

#include <stddef.h>
#include <stdio.h>            /* This example main program uses printf/fflush */
#include "single_cell_battery_phaseC_backup.h" /* Model header file */

static RT_MODEL_single_cell_battery__T single_cell_battery_phaseC_b_M_;
static RT_MODEL_single_cell_battery__T *const single_cell_battery_phaseC_MPtr =
  &single_cell_battery_phaseC_b_M_;    /* Real-time model */
static DW_single_cell_battery_phaseC_T single_cell_battery_phaseC_b_DW;/* Observable states */

/* '<Root>/I_pack' */
static real32_T single_cell_battery_phaseC_ba_U_I_pack;

/* '<Root>/T_amb' */
static real32_T single_cell_battery_phaseC_ba_U_T_amb;

/* '<Root>/V_pack' */
static real32_T single_cell_battery_phaseC_ba_Y_V_pack;

/* '<Root>/T_core' */
static real32_T single_cell_battery_phaseC_ba_Y_T_core;

/* '<Root>/T_surf' */
static real32_T single_cell_battery_phaseC_ba_Y_T_surf;

/* '<Root>/SoC_true' */
static real32_T single_cell_battery_phaseC_ba_Y_SoC_true;

/*
 * Associating rt_OneStep with a real-time clock or interrupt service routine
 * is what makes the generated code "real-time".  The function rt_OneStep is
 * always associated with the base rate of the model.  Subrates are managed
 * by the base rate from inside the generated code.  Enabling/disabling
 * interrupts and floating point context switches are target specific.  This
 * example code indicates where these should take place relative to executing
 * the generated code step function.  Overrun behavior should be tailored to
 * your application needs.  This example simply sets an error status in the
 * real-time model and returns from rt_OneStep.
 */
void rt_OneStep(RT_MODEL_single_cell_battery__T *const
                single_cell_battery_phaseC_b_M);
void rt_OneStep(RT_MODEL_single_cell_battery__T *const
                single_cell_battery_phaseC_b_M)
{
  static boolean_T OverrunFlag = false;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    return;
  }

  OverrunFlag = true;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */

  /* Step the model */
  single_cell_battery_phaseC_backup_step(single_cell_battery_phaseC_b_M,
    single_cell_battery_phaseC_ba_U_I_pack,
    single_cell_battery_phaseC_ba_U_T_amb,
    &single_cell_battery_phaseC_ba_Y_V_pack,
    &single_cell_battery_phaseC_ba_Y_T_core,
    &single_cell_battery_phaseC_ba_Y_T_surf,
    &single_cell_battery_phaseC_ba_Y_SoC_true);

  /* Get model outputs here */

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

/*
 * The example main function illustrates what is required by your
 * application code to initialize, execute, and terminate the generated code.
 * Attaching rt_OneStep to a real-time clock is target specific. This example
 * illustrates how you do this relative to initializing the model.
 */
int_T main(int_T argc, const char *argv[])
{
  RT_MODEL_single_cell_battery__T *const single_cell_battery_phaseC_b_M =
    single_cell_battery_phaseC_MPtr;

  /* Unused arguments */
  (void)(argc);
  (void)(argv);

  /* Pack model data into RTM */
  single_cell_battery_phaseC_b_M->dwork = &single_cell_battery_phaseC_b_DW;

  /* Initialize model */
  single_cell_battery_phaseC_backup_initialize(single_cell_battery_phaseC_b_M);

  /* Attach rt_OneStep to a timer or interrupt service routine with
   * period 0.1 seconds (base rate of the model) here.
   * The call syntax for rt_OneStep is
   *
   *  rt_OneStep(single_cell_battery_phaseC_b_M);
   */
  printf("Warning: The simulation will run forever. "
         "Generated ERT main won't simulate model step behavior. "
         "To change this behavior select the 'MAT-file logging' option.\n");
  fflush((NULL));
  while (1) {
    /*  Perform application tasks here */
  }

  /* The option 'Remove error status field in real-time model data structure'
   * is selected, therefore the following code does not need to execute.
   */

  /* Terminate model */
  single_cell_battery_phaseC_backup_terminate(single_cell_battery_phaseC_b_M);
  return 0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
