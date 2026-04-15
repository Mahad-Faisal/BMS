/*
 * single_cell_battery.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "single_cell_battery".
 *
 * Model version              : 1.45
 * Simulink Coder version : 25.2 (R2025b) 28-Jul-2025
 * C source code generated on : Tue Apr 14 22:27:15 2026
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "single_cell_battery.h"
#include "rtwtypes.h"
#include "single_cell_battery_private.h"
#include <string.h>

/* Block signals (default storage) */
B_single_cell_battery_T single_cell_battery_B;

/* Block states (default storage) */
DW_single_cell_battery_T single_cell_battery_DW;

/* Real-time model */
static RT_MODEL_single_cell_battery_T single_cell_battery_M_;
RT_MODEL_single_cell_battery_T *const single_cell_battery_M =
  &single_cell_battery_M_;
real_T look2_pbinlcpw(real_T u0, real_T u1, const real_T bp0[], const real_T
                      bp1[], const real_T table[], uint32_T prevIndex[], const
                      uint32_T maxIndex[], uint32_T stride)
{
  real_T fractions[2];
  real_T frac;
  real_T yL_0d0;
  real_T yL_0d1;
  uint32_T bpIndices[2];
  uint32_T bpIdx;
  uint32_T found;
  uint32_T iLeft;
  uint32_T iRght;

  /* Column-major Lookup 2-D
     Search method: 'binary'
     Use previous index: 'on'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Clip'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'on'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    bpIdx = 0U;
    frac = 0.0;
  } else if (u0 < bp0[maxIndex[0U]]) {
    /* Binary Search using Previous Index */
    bpIdx = prevIndex[0U];
    iLeft = 0U;
    iRght = maxIndex[0U];
    found = 0U;
    while (found == 0U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx - 1U;
        bpIdx = ((bpIdx + iLeft) - 1U) >> 1U;
      } else if (u0 < bp0[bpIdx + 1U]) {
        found = 1U;
      } else {
        iLeft = bpIdx + 1U;
        bpIdx = ((bpIdx + iRght) + 1U) >> 1U;
      }
    }

    frac = (u0 - bp0[bpIdx]) / (bp0[bpIdx + 1U] - bp0[bpIdx]);
  } else {
    bpIdx = maxIndex[0U] - 1U;
    frac = 1.0;
  }

  prevIndex[0U] = bpIdx;
  fractions[0U] = frac;
  bpIndices[0U] = bpIdx;

  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'on'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u1 <= bp1[0U]) {
    bpIdx = 0U;
    frac = 0.0;
  } else if (u1 < bp1[maxIndex[1U]]) {
    /* Binary Search using Previous Index */
    bpIdx = prevIndex[1U];
    iLeft = 0U;
    iRght = maxIndex[1U];
    found = 0U;
    while (found == 0U) {
      if (u1 < bp1[bpIdx]) {
        iRght = bpIdx - 1U;
        bpIdx = ((bpIdx + iLeft) - 1U) >> 1U;
      } else if (u1 < bp1[bpIdx + 1U]) {
        found = 1U;
      } else {
        iLeft = bpIdx + 1U;
        bpIdx = ((bpIdx + iRght) + 1U) >> 1U;
      }
    }

    frac = (u1 - bp1[bpIdx]) / (bp1[bpIdx + 1U] - bp1[bpIdx]);
  } else {
    bpIdx = maxIndex[1U] - 1U;
    frac = 1.0;
  }

  prevIndex[1U] = bpIdx;

  /* Column-major Interpolation 2-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  iLeft = bpIdx * stride + bpIndices[0U];
  yL_0d0 = table[iLeft];
  yL_0d0 += (table[iLeft + 1U] - yL_0d0) * fractions[0U];
  iLeft += stride;
  yL_0d1 = table[iLeft];
  return (((table[iLeft + 1U] - yL_0d1) * fractions[0U] + yL_0d1) - yL_0d0) *
    frac + yL_0d0;
}

real_T look2_binlcpw(real_T u0, real_T u1, const real_T bp0[], const real_T bp1[],
                     const real_T table[], const uint32_T maxIndex[], uint32_T
                     stride)
{
  real_T fractions[2];
  real_T frac;
  real_T yL_0d0;
  real_T yL_0d1;
  uint32_T bpIndices[2];
  uint32_T bpIdx;
  uint32_T iLeft;
  uint32_T iRght;

  /* Column-major Lookup 2-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Clip'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = 0.0;
  } else if (u0 < bp0[maxIndex[0U]]) {
    /* Binary Search */
    bpIdx = maxIndex[0U] >> 1U;
    iLeft = 0U;
    iRght = maxIndex[0U];
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex[0U] - 1U;
    frac = 1.0;
  }

  fractions[0U] = frac;
  bpIndices[0U] = iLeft;

  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u1 <= bp1[0U]) {
    iLeft = 0U;
    frac = 0.0;
  } else if (u1 < bp1[maxIndex[1U]]) {
    /* Binary Search */
    bpIdx = maxIndex[1U] >> 1U;
    iLeft = 0U;
    iRght = maxIndex[1U];
    while (iRght - iLeft > 1U) {
      if (u1 < bp1[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u1 - bp1[iLeft]) / (bp1[iLeft + 1U] - bp1[iLeft]);
  } else {
    iLeft = maxIndex[1U] - 1U;
    frac = 1.0;
  }

  /* Column-major Interpolation 2-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  bpIdx = iLeft * stride + bpIndices[0U];
  yL_0d0 = table[bpIdx];
  yL_0d0 += (table[bpIdx + 1U] - yL_0d0) * fractions[0U];
  bpIdx += stride;
  yL_0d1 = table[bpIdx];
  return (((table[bpIdx + 1U] - yL_0d1) * fractions[0U] + yL_0d1) - yL_0d0) *
    frac + yL_0d0;
}

/* Model step function */
void single_cell_battery_step(void)
{
  /* local block i/o variables */
  real_T rtb_inv_Cc;
  real_T rtb_inv_Cs;
  real_T rtb_Vp1_dot;
  real_T rtb_Vp2_dot;
  real_T rtb_dSoC_per_step;
  real_T rtb_C1_LUT;
  real_T rtb_I_cmd;
  real_T rtb_R1C1_LUT;
  real_T rtb_T_core_int;
  real_T rtb_T_surf_int;
  real_T rtb_Vp1_Integrator1;
  real_T rtb_Vp2_Integrator;
  real_T rtb_inv_Rcs;
  real_T rtb_q_gen_calc;

  /* Saturate: '<Root>/SoC_Limit' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   */
  if (single_cell_battery_DW.SoC_Integrator_DSTATE >
      single_cell_battery_P.SoC_Limit_UpperSat) {
    rtb_C1_LUT = single_cell_battery_P.SoC_Limit_UpperSat;
  } else if (single_cell_battery_DW.SoC_Integrator_DSTATE <
             single_cell_battery_P.SoC_Limit_LowerSat) {
    rtb_C1_LUT = single_cell_battery_P.SoC_Limit_LowerSat;
  } else {
    rtb_C1_LUT = single_cell_battery_DW.SoC_Integrator_DSTATE;
  }

  /* End of Saturate: '<Root>/SoC_Limit' */
  /* DiscreteIntegrator: '<S1>/T_core_int' */
  rtb_T_core_int = single_cell_battery_DW.T_core_int_DSTATE;

  /* DiscreteIntegrator: '<S1>/T_surf_int' */
  rtb_T_surf_int = single_cell_battery_DW.T_surf_int_DSTATE;

  /* Saturate: '<Root>/ T_clamp' */
  if (rtb_T_core_int > single_cell_battery_P.T_clamp_UpperSat) {
    rtb_R1C1_LUT = single_cell_battery_P.T_clamp_UpperSat;
  } else if (rtb_T_core_int < single_cell_battery_P.T_clamp_LowerSat) {
    rtb_R1C1_LUT = single_cell_battery_P.T_clamp_LowerSat;
  } else {
    rtb_R1C1_LUT = rtb_T_core_int;
  }

  /* End of Saturate: '<Root>/ T_clamp' */

  /* Lookup_n-D: '<Root>/OCV_LUT' incorporates:
   *  Lookup_n-D: '<Root>/C1_LUT'
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   */
  rtb_C1_LUT = look2_pbinlcpw(rtb_C1_LUT, rtb_R1C1_LUT,
    single_cell_battery_P.soc_ocv_common, single_cell_battery_P.temp_bp_ocv,
    single_cell_battery_P.OCV_2d, single_cell_battery_DW.m_bpIndex,
    single_cell_battery_P.OCV_LUT_maxIndex, 101U);

  /* Lookup_n-D: '<Root>/R0_LUT' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   */
  rtb_inv_Rcs = look2_binlcpw(single_cell_battery_DW.SoC_Integrator_DSTATE,
    rtb_R1C1_LUT, single_cell_battery_P.soc_common,
    single_cell_battery_P.temp_bp, single_cell_battery_P.R0_2d_fix,
    single_cell_battery_P.R0_LUT_maxIndex, 12U);

  /* Step: '<Root>/I_pack' incorporates:
   *  Step: '<Root>/I_pack_off'
   */
  rtb_I_cmd = (((single_cell_battery_M->Timing.clockTick1+
                 single_cell_battery_M->Timing.clockTickH1* 4294967296.0)) * 0.1);
  if (rtb_I_cmd < single_cell_battery_P.I_pack_Time) {
    rtb_Vp1_Integrator1 = single_cell_battery_P.I_pack_Y0;
  } else {
    rtb_Vp1_Integrator1 = single_cell_battery_P.I_test;
  }

  /* End of Step: '<Root>/I_pack' */

  /* Step: '<Root>/I_pack_off' */
  if (rtb_I_cmd < single_cell_battery_P.I_pack_off_Time) {
    rtb_I_cmd = single_cell_battery_P.I_pack_off_Y0;
  } else {
    rtb_I_cmd = -single_cell_battery_P.I_test;
  }

  /* Sum: '<Root>/I_cmd' incorporates:
   *  Step: '<Root>/I_pack_off'
   */
  rtb_I_cmd += rtb_Vp1_Integrator1;

  /* DiscreteIntegrator: '<Root>/Vp1_Integrator1' */
  rtb_Vp1_Integrator1 = single_cell_battery_DW.Vp1_Integrator1_DSTATE;

  /* DiscreteIntegrator: '<Root>/Vp2_Integrator' */
  rtb_Vp2_Integrator = single_cell_battery_DW.Vp2_Integrator_DSTATE;

  /* Sum: '<Root>/Vcell_Calc' incorporates:
   *  Product: '<Root>/Product2'
   */
  rtb_C1_LUT = ((rtb_C1_LUT - rtb_inv_Rcs * rtb_I_cmd) - rtb_Vp1_Integrator1) -
    rtb_Vp2_Integrator;

  /* ToWorkspace: '<Root>/To Workspace' */
  rt_UpdateLogVar((LogVar *)(LogVar*)
                  (single_cell_battery_DW.ToWorkspace_PWORK.LoggedData),
                  &rtb_C1_LUT, 0);

  /* Lookup_n-D: '<Root>/C1_LUT' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   */
  rtb_C1_LUT = look2_binlcpw(single_cell_battery_DW.SoC_Integrator_DSTATE,
    rtb_R1C1_LUT, single_cell_battery_P.soc_common,
    single_cell_battery_P.temp_bp, single_cell_battery_P.inv_C1_2d_fixed,
    single_cell_battery_P.C1_LUT_maxIndex, 12U);

  /* Product: '<S1>/q_gen_calc' incorporates:
   *  Product: '<S1>/I_squared'
   */
  rtb_q_gen_calc = rtb_I_cmd * rtb_I_cmd * rtb_inv_Rcs;

  /* Gain: '<S1>/inv_Rcs' incorporates:
   *  Sum: '<S1>/dT_cs'
   */
  rtb_inv_Rcs = 1.0 / single_cell_battery_P.Rcs * (rtb_T_core_int
    - rtb_T_surf_int);

  /* Gain: '<S1>/inv_Cc' incorporates:
   *  Sum: '<S1>/Core_Heat_Balance'
   */
  rtb_inv_Cc = 1.0 / single_cell_battery_P.Cc * (rtb_q_gen_calc - rtb_inv_Rcs);

  /* Gain: '<S1>/inv_Cs' incorporates:
   *  Constant: '<S1>/T_amb_const'
   *  Gain: '<S1>/inv_Rsa'
   *  Sum: '<S1>/Surf_Heat_Balance'
   *  Sum: '<S1>/dT_sa'
   */
  rtb_inv_Cs = (rtb_inv_Rcs - 1.0 / single_cell_battery_P.Rsa * (rtb_T_surf_int
    - single_cell_battery_P.T_amb)) * (1.0 / single_cell_battery_P.Cs);

  /* Sum: '<Root>/Vp1_dot' incorporates:
   *  DiscreteIntegrator: '<Root>/SoC_Integrator'
   *  Lookup_n-D: '<Root>/R1C1_LUT'
   *  Product: '<Root>/Product'
   *  Product: '<Root>/Product1'
   */
  rtb_Vp1_dot = rtb_C1_LUT * rtb_I_cmd + look2_binlcpw
    (single_cell_battery_DW.SoC_Integrator_DSTATE, rtb_R1C1_LUT,
     single_cell_battery_P.soc_common, single_cell_battery_P.temp_bp,
     single_cell_battery_P.neg_inv_R1C1_2d_fixed,
     single_cell_battery_P.R1C1_LUT_maxIndex, 12U) * rtb_Vp1_Integrator1;

  /* Sum: '<Root>/Vp2_dot' incorporates:
   *  Gain: '<Root>/I_to_Vp2'
   *  Gain: '<Root>/Vp2_feedback'
   */
  rtb_Vp2_dot = -1.0 / (single_cell_battery_P.R2 * single_cell_battery_P.C2) *
    rtb_Vp2_Integrator + 1.0 / single_cell_battery_P.C2 * rtb_I_cmd;

  /* Gain: '<Root>/dSoC_per_step' */
  rtb_dSoC_per_step = -single_cell_battery_P.Ts / (3600.0 *
    single_cell_battery_P.Q_nom) * rtb_I_cmd;

  /* Clock: '<Root>/Clock' */
  single_cell_battery_B.Clock = single_cell_battery_M->Timing.t[0];

  /* ToWorkspace: '<Root>/To Workspace1' */
  rt_UpdateLogVar((LogVar *)(LogVar*)
                  (single_cell_battery_DW.ToWorkspace1_PWORK.LoggedData),
                  &single_cell_battery_B.Clock, 0);

  /* Matfile logging */
  rt_UpdateTXYLogVars(single_cell_battery_M->rtwLogInfo,
                      (single_cell_battery_M->Timing.t));

  /* Update for DiscreteIntegrator: '<Root>/SoC_Integrator' */
  single_cell_battery_DW.SoC_Integrator_DSTATE +=
    single_cell_battery_P.SoC_Integrator_gainval * rtb_dSoC_per_step;

  /* Update for DiscreteIntegrator: '<S1>/T_core_int' */
  single_cell_battery_DW.T_core_int_DSTATE +=
    single_cell_battery_P.T_core_int_gainval * rtb_inv_Cc;

  /* Update for DiscreteIntegrator: '<S1>/T_surf_int' */
  single_cell_battery_DW.T_surf_int_DSTATE +=
    single_cell_battery_P.T_surf_int_gainval * rtb_inv_Cs;

  /* Update for DiscreteIntegrator: '<Root>/Vp1_Integrator1' */
  single_cell_battery_DW.Vp1_Integrator1_DSTATE +=
    single_cell_battery_P.Vp1_Integrator1_gainval * rtb_Vp1_dot;

  /* Update for DiscreteIntegrator: '<Root>/Vp2_Integrator' */
  single_cell_battery_DW.Vp2_Integrator_DSTATE +=
    single_cell_battery_P.Vp2_Integrator_gainval * rtb_Vp2_dot;

  /* signal main to stop simulation */
  {                                    /* Sample time: [0.0s, 0.0s] */
    if ((rtmGetTFinal(single_cell_battery_M)!=-1) &&
        !((rtmGetTFinal(single_cell_battery_M)-single_cell_battery_M->Timing.t[0])
          > single_cell_battery_M->Timing.t[0] * (DBL_EPSILON))) {
      rtmSetErrorStatus(single_cell_battery_M, "Simulation finished");
    }
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++single_cell_battery_M->Timing.clockTick0)) {
    ++single_cell_battery_M->Timing.clockTickH0;
  }

  single_cell_battery_M->Timing.t[0] = single_cell_battery_M->Timing.clockTick0 *
    single_cell_battery_M->Timing.stepSize0 +
    single_cell_battery_M->Timing.clockTickH0 *
    single_cell_battery_M->Timing.stepSize0 * 4294967296.0;

  {
    /* Update absolute timer for sample time: [0.1s, 0.0s] */
    /* The "clockTick1" counts the number of times the code of this task has
     * been executed. The resolution of this integer timer is 0.1, which is the step size
     * of the task. Size of "clockTick1" ensures timer will not overflow during the
     * application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick1 and the high bits
     * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
     */
    single_cell_battery_M->Timing.clockTick1++;
    if (!single_cell_battery_M->Timing.clockTick1) {
      single_cell_battery_M->Timing.clockTickH1++;
    }
  }
}

/* Model initialize function */
void single_cell_battery_initialize(void)
{
  /* Registration code */

  /* initialize real-time model */
  (void) memset((void *)single_cell_battery_M, 0,
                sizeof(RT_MODEL_single_cell_battery_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&single_cell_battery_M->solverInfo,
                          &single_cell_battery_M->Timing.simTimeStep);
    rtsiSetTPtr(&single_cell_battery_M->solverInfo, &rtmGetTPtr
                (single_cell_battery_M));
    rtsiSetStepSizePtr(&single_cell_battery_M->solverInfo,
                       &single_cell_battery_M->Timing.stepSize0);
    rtsiSetErrorStatusPtr(&single_cell_battery_M->solverInfo,
                          (&rtmGetErrorStatus(single_cell_battery_M)));
    rtsiSetRTModelPtr(&single_cell_battery_M->solverInfo, single_cell_battery_M);
  }

  rtsiSetSimTimeStep(&single_cell_battery_M->solverInfo, MAJOR_TIME_STEP);
  rtsiSetIsMinorTimeStepWithModeChange(&single_cell_battery_M->solverInfo, false);
  rtsiSetIsContModeFrozen(&single_cell_battery_M->solverInfo, false);
  rtsiSetSolverName(&single_cell_battery_M->solverInfo,"FixedStepDiscrete");
  rtmSetTPtr(single_cell_battery_M, &single_cell_battery_M->Timing.tArray[0]);
  rtmSetTFinal(single_cell_battery_M, 1400.0);
  single_cell_battery_M->Timing.stepSize0 = 0.1;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = (NULL);
    single_cell_battery_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(single_cell_battery_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(single_cell_battery_M->rtwLogInfo, (NULL));
    rtliSetLogT(single_cell_battery_M->rtwLogInfo, "");
    rtliSetLogX(single_cell_battery_M->rtwLogInfo, "");
    rtliSetLogXFinal(single_cell_battery_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(single_cell_battery_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(single_cell_battery_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(single_cell_battery_M->rtwLogInfo, 0);
    rtliSetLogDecimation(single_cell_battery_M->rtwLogInfo, 1);
    rtliSetLogY(single_cell_battery_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(single_cell_battery_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(single_cell_battery_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &single_cell_battery_B), 0,
                sizeof(B_single_cell_battery_T));

  /* states (dwork) */
  (void) memset((void *)&single_cell_battery_DW, 0,
                sizeof(DW_single_cell_battery_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(single_cell_battery_M->rtwLogInfo, 0.0,
    rtmGetTFinal(single_cell_battery_M), single_cell_battery_M->Timing.stepSize0,
    (&rtmGetErrorStatus(single_cell_battery_M)));

  /* SetupRuntimeResources for ToWorkspace: '<Root>/To Workspace' */
  {
    int_T dimensions[1] = { 1 };

    single_cell_battery_DW.ToWorkspace_PWORK.LoggedData = rt_CreateLogVar(
      single_cell_battery_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(single_cell_battery_M),
      single_cell_battery_M->Timing.stepSize0,
      (&rtmGetErrorStatus(single_cell_battery_M)),
      "V_sim",
      SS_DOUBLE,
      0,
      0,
      0,
      1,
      1,
      dimensions,
      NO_LOGVALDIMS,
      (NULL),
      (NULL),
      0,
      1,
      0.1,
      1);
    if (single_cell_battery_DW.ToWorkspace_PWORK.LoggedData == (NULL))
      return;
  }

  /* SetupRuntimeResources for ToWorkspace: '<Root>/To Workspace1' */
  {
    int_T dimensions[1] = { 1 };

    single_cell_battery_DW.ToWorkspace1_PWORK.LoggedData = rt_CreateLogVar(
      single_cell_battery_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(single_cell_battery_M),
      single_cell_battery_M->Timing.stepSize0,
      (&rtmGetErrorStatus(single_cell_battery_M)),
      "t_sim",
      SS_DOUBLE,
      0,
      0,
      0,
      1,
      1,
      dimensions,
      NO_LOGVALDIMS,
      (NULL),
      (NULL),
      0,
      1,
      0.1,
      1);
    if (single_cell_battery_DW.ToWorkspace1_PWORK.LoggedData == (NULL))
      return;
  }

  /* InitializeConditions for DiscreteIntegrator: '<Root>/SoC_Integrator' */
  single_cell_battery_DW.SoC_Integrator_DSTATE = single_cell_battery_P.SoC_init;

  /* InitializeConditions for DiscreteIntegrator: '<S1>/T_core_int' */
  single_cell_battery_DW.T_core_int_DSTATE = single_cell_battery_P.T_init;

  /* InitializeConditions for DiscreteIntegrator: '<S1>/T_surf_int' */
  single_cell_battery_DW.T_surf_int_DSTATE = single_cell_battery_P.T_init;

  /* InitializeConditions for DiscreteIntegrator: '<Root>/Vp1_Integrator1' */
  single_cell_battery_DW.Vp1_Integrator1_DSTATE =
    single_cell_battery_P.Vp1_Integrator1_IC;

  /* InitializeConditions for DiscreteIntegrator: '<Root>/Vp2_Integrator' */
  single_cell_battery_DW.Vp2_Integrator_DSTATE =
    single_cell_battery_P.Vp2_Integrator_IC;
}

/* Model terminate function */
void single_cell_battery_terminate(void)
{
  /* (no terminate code required) */
}
