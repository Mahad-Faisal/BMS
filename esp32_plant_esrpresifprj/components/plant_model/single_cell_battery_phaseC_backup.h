/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: single_cell_battery_phaseC_backup.h
 *
 * Code generated for Simulink model 'single_cell_battery_phaseC_backup'.
 *
 * Model version                  : 1.60
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Wed Apr 15 18:47:07 2026
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

#ifndef single_cell_battery_phaseC_backup_h_
#define single_cell_battery_phaseC_backup_h_
#ifndef single_cell_battery_phaseC_backup_COMMON_INCLUDES_
#define single_cell_battery_phaseC_backup_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                  /* single_cell_battery_phaseC_backup_COMMON_INCLUDES_ */

#include "single_cell_battery_phaseC_backup_types.h"
#include "rt_defines.h"

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real32_T SoC_Integrator_DSTATE;      /* '<Root>/SoC_Integrator' */
  real32_T T_core_int_DSTATE;          /* '<S1>/T_core_int' */
  real32_T Vp1_Integrator1_DSTATE;     /* '<Root>/Vp1_Integrator1' */
  real32_T Vp2_Integrator_DSTATE;      /* '<Root>/Vp2_Integrator' */
  real32_T T_surf_int_DSTATE;          /* '<S1>/T_surf_int' */
  uint32_T m_bpIndex[2];               /* '<Root>/OCV_LUT' */
} DW_single_cell_battery_phaseC_T;

/* Real-time Model Data Structure */
struct tag_RTM_single_cell_battery_p_T {
  DW_single_cell_battery_phaseC_T *dwork;
};

/* Model entry point functions */
extern void single_cell_battery_phaseC_backup_initialize
  (RT_MODEL_single_cell_battery__T *const single_cell_battery_phaseC_b_M);
extern void single_cell_battery_phaseC_backup_step
  (RT_MODEL_single_cell_battery__T *const single_cell_battery_phaseC_b_M,
   real32_T single_cell_battery_phaseC_ba_U_I_pack, real32_T
   single_cell_battery_phaseC_ba_U_T_amb, real32_T
   *single_cell_battery_phaseC_ba_Y_V_pack, real32_T
   *single_cell_battery_phaseC_ba_Y_T_core, real32_T
   *single_cell_battery_phaseC_ba_Y_T_surf, real32_T
   *single_cell_battery_phaseC_ba_Y_SoC_true);
extern void single_cell_battery_phaseC_backup_terminate
  (RT_MODEL_single_cell_battery__T *const single_cell_battery_phaseC_b_M);

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/inv_Rcs' : Eliminated nontunable gain of 1
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'single_cell_battery_phaseC_backup'
 * '<S1>'   : 'single_cell_battery_phaseC_backup/Thermal_2Node'
 */
#endif                                /* single_cell_battery_phaseC_backup_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
