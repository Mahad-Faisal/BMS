/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: single_cell_battery_phaseC_backup.c
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

#include "single_cell_battery_phaseC_backup.h"
#include "rtwtypes.h"
#include "look2_iflf_binlc.h"
#include "look2_iflf_pbinlc.h"
#include "single_cell_battery_phaseC_backup_private.h"

/* Model step function */
void single_cell_battery_phaseC_backup_step(RT_MODEL_single_cell_battery__T *
  const single_cell_battery_phaseC_b_M, real32_T
  single_cell_battery_phaseC_ba_U_I_pack, real32_T
  single_cell_battery_phaseC_ba_U_T_amb, real32_T
  *single_cell_battery_phaseC_ba_Y_V_pack, real32_T
  *single_cell_battery_phaseC_ba_Y_T_core, real32_T
  *single_cell_battery_phaseC_ba_Y_T_surf, real32_T
  *single_cell_battery_phaseC_ba_Y_SoC_true)
{
  DW_single_cell_battery_phaseC_T *single_cell_battery_phaseC_b_DW =
    single_cell_battery_phaseC_b_M->dwork;
  real32_T rtb_C1_LUT;
  real32_T rtb_I_pack_to_cell;
  real32_T rtb_Product1;
  real32_T rtb_R1C1_LUT;
  real32_T rtb_SoC_Limit;
  real32_T rtb_T_surf_int;
  real32_T rtb_q_gen_calc;

  /* Saturate: '<Root>/ T_clamp' incorporates:
   *  DiscreteIntegrator: '<S1>/T_core_int'
   */
  if (single_cell_battery_phaseC_b_DW->T_core_int_DSTATE > 25.0F) {
    rtb_C1_LUT = 25.0F;
  } else if (single_cell_battery_phaseC_b_DW->T_core_int_DSTATE < 0.0F) {
    rtb_C1_LUT = 0.0F;
  } else {
    rtb_C1_LUT = single_cell_battery_phaseC_b_DW->T_core_int_DSTATE;
  }

  /* End of Saturate: '<Root>/ T_clamp' */

  /* Lookup_n-D: '<Root>/R0_LUT' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   *  Lookup_n-D: '<Root>/C1_LUT'
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   */
  rtb_R1C1_LUT = look2_iflf_binlc
    (single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE, rtb_C1_LUT,
     rtCP_R0_LUT_bp01Data, rtCP_R0_LUT_bp02Data, rtCP_R0_LUT_tableData,
     rtCP_R0_LUT_maxIndex, 12U);

  /* Gain: '<Root>/I_pack_to_cell' incorporates:
   *  Inport: '<Root>/I_pack'
   */
  rtb_I_pack_to_cell = 0.5F * single_cell_battery_phaseC_ba_U_I_pack;

  /* Saturate: '<Root>/SoC_Limit' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   */
  if (single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE > 1.0F) {
    rtb_SoC_Limit = 1.0F;
  } else if (single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE < 0.0F) {
    rtb_SoC_Limit = 0.0F;
  } else {
    rtb_SoC_Limit = single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE;
  }

  /* End of Saturate: '<Root>/SoC_Limit' */

  /* Lookup_n-D: '<Root>/OCV_LUT' incorporates:
   *  DiscreteIntegrator: '<S1>/T_surf_int'
   *  Lookup_n-D: '<Root>/C1_LUT'
   *  Saturate: '<Root>/SoC_Limit'
   */
  rtb_T_surf_int = look2_iflf_pbinlc(rtb_SoC_Limit, rtb_C1_LUT,
    rtCP_OCV_LUT_bp01Data, rtCP_OCV_LUT_bp02Data, rtCP_OCV_LUT_tableData,
    single_cell_battery_phaseC_b_DW->m_bpIndex, rtCP_OCV_LUT_maxIndex, 101U);

  /* Outport: '<Root>/V_pack' incorporates:
   *  DiscreteIntegrator: '<Root>/Vp1_Integrator1'
   *  DiscreteIntegrator: '<Root>/Vp2_Integrator'
   *  DiscreteIntegrator: '<S1>/T_surf_int'
   *  Gain: '<Root>/V_cell_to_pack'
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   *  Product: '<Root>/Product2'
   *  Sum: '<Root>/Vcell_Calc'
   */
  *single_cell_battery_phaseC_ba_Y_V_pack = (((rtb_T_surf_int - rtb_R1C1_LUT *
    rtb_I_pack_to_cell) -
    single_cell_battery_phaseC_b_DW->Vp1_Integrator1_DSTATE) -
    single_cell_battery_phaseC_b_DW->Vp2_Integrator_DSTATE) * 8.0F;

  /* Product: '<S1>/Divide' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   *  DiscreteIntegrator: '<Root>/Vp1_Integrator1'
   *  Lookup_n-D: '<Root>/C1_LUT'
   *  Lookup_n-D: '<Root>/n-D Lookup Table'
   *  Product: '<S1>/Vp1_sq'
   */
  rtb_T_surf_int = single_cell_battery_phaseC_b_DW->Vp1_Integrator1_DSTATE *
    single_cell_battery_phaseC_b_DW->Vp1_Integrator1_DSTATE / look2_iflf_binlc
    (single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE, rtb_C1_LUT,
     rtCP_nDLookupTable_bp01Data, rtCP_nDLookupTable_bp02Data,
     rtCP_nDLookupTable_tableData, rtCP_nDLookupTable_maxIndex, 12U);

  /* Product: '<S1>/q_gen_calc' incorporates:
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   *  Product: '<S1>/I_squared'
   */
  rtb_q_gen_calc = rtb_I_pack_to_cell * rtb_I_pack_to_cell * rtb_R1C1_LUT;

  /* Sum: '<S1>/dT_cs' incorporates:
   *  DiscreteIntegrator: '<S1>/T_core_int'
   *  DiscreteIntegrator: '<S1>/T_surf_int'
   */
  rtb_R1C1_LUT = single_cell_battery_phaseC_b_DW->T_core_int_DSTATE -
    single_cell_battery_phaseC_b_DW->T_surf_int_DSTATE;

  /* Outport: '<Root>/T_surf' incorporates:
   *  DiscreteIntegrator: '<S1>/T_surf_int'
   */
  *single_cell_battery_phaseC_ba_Y_T_surf =
    single_cell_battery_phaseC_b_DW->T_surf_int_DSTATE;

  /* Product: '<Root>/Product1' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   *  DiscreteIntegrator: '<Root>/Vp1_Integrator1'
   *  Lookup_n-D: '<Root>/C1_LUT'
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   */
  rtb_Product1 = look2_iflf_binlc
    (single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE, rtb_C1_LUT,
     rtCP_R1C1_LUT_bp01Data, rtCP_R1C1_LUT_bp02Data, rtCP_R1C1_LUT_tableData,
     rtCP_R1C1_LUT_maxIndex, 12U) *
    single_cell_battery_phaseC_b_DW->Vp1_Integrator1_DSTATE;

  /* Product: '<Root>/Product' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   *  Lookup_n-D: '<Root>/C1_LUT'
   */
  rtb_C1_LUT = look2_iflf_binlc
    (single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE, rtb_C1_LUT,
     rtCP_C1_LUT_bp01Data, rtCP_C1_LUT_bp02Data, rtCP_C1_LUT_tableData,
     rtCP_C1_LUT_maxIndex, 12U) * rtb_I_pack_to_cell;

  /* Outport: '<Root>/SoC_true' */
  *single_cell_battery_phaseC_ba_Y_SoC_true = rtb_SoC_Limit;

  /* Outport: '<Root>/T_core' incorporates:
   *  DiscreteIntegrator: '<S1>/T_core_int'
   */
  *single_cell_battery_phaseC_ba_Y_T_core =
    single_cell_battery_phaseC_b_DW->T_core_int_DSTATE;

  /* Update for DiscreteIntegrator: '<Root>/SoC_Integrator' incorporates:
   *  Gain: '<Root>/dSoC_per_step'
   */
  single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE += -9.27471683E-6F *
    rtb_I_pack_to_cell * 0.1F;

  /* Update for DiscreteIntegrator: '<S1>/T_core_int' incorporates:
   *  DiscreteIntegrator: '<Root>/Vp2_Integrator'
   *  Gain: '<S1>/Gain'
   *  Gain: '<S1>/inv_Cc'
   *  Product: '<S1>/Vp2_sq'
   *  Sum: '<S1>/Core_Heat_Balance'
   *  Sum: '<S1>/Sum'
   */
  single_cell_battery_phaseC_b_DW->T_core_int_DSTATE +=
    (((single_cell_battery_phaseC_b_DW->Vp2_Integrator_DSTATE *
       single_cell_battery_phaseC_b_DW->Vp2_Integrator_DSTATE * 100.0F +
       rtb_T_surf_int) + rtb_q_gen_calc) - rtb_R1C1_LUT) * 0.0025F * 0.1F;

  /* Update for DiscreteIntegrator: '<Root>/Vp1_Integrator1' incorporates:
   *  Sum: '<Root>/Vp1_dot'
   */
  single_cell_battery_phaseC_b_DW->Vp1_Integrator1_DSTATE += (rtb_C1_LUT +
    rtb_Product1) * 0.1F;

  /* Update for DiscreteIntegrator: '<Root>/Vp2_Integrator' incorporates:
   *  Gain: '<Root>/I_to_Vp2'
   *  Gain: '<Root>/Vp2_feedback'
   *  Sum: '<Root>/Vp2_dot'
   */
  single_cell_battery_phaseC_b_DW->Vp2_Integrator_DSTATE += (0.000125F *
    rtb_I_pack_to_cell + -0.0125F *
    single_cell_battery_phaseC_b_DW->Vp2_Integrator_DSTATE) * 0.1F;

  /* Update for DiscreteIntegrator: '<S1>/T_surf_int' incorporates:
   *  Gain: '<S1>/inv_Cs'
   *  Gain: '<S1>/inv_Rsa'
   *  Inport: '<Root>/T_amb'
   *  Sum: '<S1>/Surf_Heat_Balance'
   *  Sum: '<S1>/dT_sa'
   */
  single_cell_battery_phaseC_b_DW->T_surf_int_DSTATE += (rtb_R1C1_LUT -
    (single_cell_battery_phaseC_b_DW->T_surf_int_DSTATE -
     single_cell_battery_phaseC_ba_U_T_amb) * 0.333333343F) * 0.01F * 0.1F;
}

/* Model initialize function */
void single_cell_battery_phaseC_backup_initialize
  (RT_MODEL_single_cell_battery__T *const single_cell_battery_phaseC_b_M)
{
  DW_single_cell_battery_phaseC_T *single_cell_battery_phaseC_b_DW =
    single_cell_battery_phaseC_b_M->dwork;

  /* InitializeConditions for DiscreteIntegrator: '<Root>/SoC_Integrator' */
  single_cell_battery_phaseC_b_DW->SoC_Integrator_DSTATE = 1.0F;

  /* InitializeConditions for DiscreteIntegrator: '<S1>/T_core_int' */
  single_cell_battery_phaseC_b_DW->T_core_int_DSTATE = 25.0F;

  /* InitializeConditions for DiscreteIntegrator: '<S1>/T_surf_int' */
  single_cell_battery_phaseC_b_DW->T_surf_int_DSTATE = 25.0F;
}

/* Model terminate function */
void single_cell_battery_phaseC_backup_terminate(RT_MODEL_single_cell_battery__T
  *const single_cell_battery_phaseC_b_M)
{
  /* (no terminate code required) */
  UNUSED_PARAMETER(single_cell_battery_phaseC_b_M);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
