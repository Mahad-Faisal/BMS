/*
 * single_cell_battery.h
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

#ifndef single_cell_battery_h_
#define single_cell_battery_h_
#ifndef single_cell_battery_COMMON_INCLUDES_
#define single_cell_battery_COMMON_INCLUDES_
#include <stdlib.h>
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                /* single_cell_battery_COMMON_INCLUDES_ */

#include "single_cell_battery_types.h"
#include <stddef.h>
#include <float.h>
#include <string.h>

/* Macros for accessing real-time model data structure */
#ifndef rtmGetFinalTime
#define rtmGetFinalTime(rtm)           ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetRTWLogInfo
#define rtmGetRTWLogInfo(rtm)          ((rtm)->rtwLogInfo)
#endif

#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
#define rtmGetStopRequested(rtm)       ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
#define rtmSetStopRequested(rtm, val)  ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
#define rtmGetStopRequestedPtr(rtm)    (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
#define rtmGetT(rtm)                   (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTFinal
#define rtmGetTFinal(rtm)              ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetTPtr
#define rtmGetTPtr(rtm)                ((rtm)->Timing.t)
#endif

/* Block signals (default storage) */
typedef struct {
  real_T Clock;                        /* '<Root>/Clock' */
} B_single_cell_battery_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T SoC_Integrator_DSTATE;        /* '<Root>/SoC_Integrator' */
  real_T T_core_int_DSTATE;            /* '<S1>/T_core_int' */
  real_T T_surf_int_DSTATE;            /* '<S1>/T_surf_int' */
  real_T Vp1_Integrator1_DSTATE;       /* '<Root>/Vp1_Integrator1' */
  real_T Vp2_Integrator_DSTATE;        /* '<Root>/Vp2_Integrator' */
  struct {
    void *LoggedData;
  } Scope_SoC_PWORK;                   /* '<Root>/Scope_SoC' */

  struct {
    void *LoggedData;
  } ToWorkspace_PWORK;                 /* '<Root>/To Workspace' */

  struct {
    void *LoggedData;
  } ToWorkspace1_PWORK;                /* '<Root>/To Workspace1' */

  uint32_T m_bpIndex[2];               /* '<Root>/OCV_LUT' */
} DW_single_cell_battery_T;

/* Parameters (default storage) */
struct P_single_cell_battery_T_ {
  real_T C2;                           /* Variable: C2
                                        * Referenced by:
                                        *   '<Root>/I_to_Vp2'
                                        *   '<Root>/Vp2_feedback'
                                        */
  real_T Cc;                           /* Variable: Cc
                                        * Referenced by: '<S1>/inv_Cc'
                                        */
  real_T Cs;                           /* Variable: Cs
                                        * Referenced by: '<S1>/inv_Cs'
                                        */
  real_T I_test;                       /* Variable: I_test
                                        * Referenced by:
                                        *   '<Root>/I_pack'
                                        *   '<Root>/I_pack_off'
                                        */
  real_T OCV_2d[303];                  /* Variable: OCV_2d
                                        * Referenced by: '<Root>/OCV_LUT'
                                        */
  real_T Q_nom;                        /* Variable: Q_nom
                                        * Referenced by: '<Root>/dSoC_per_step'
                                        */
  real_T R0_2d_fix[36];                /* Variable: R0_2d_fix
                                        * Referenced by: '<Root>/R0_LUT'
                                        */
  real_T R2;                           /* Variable: R2
                                        * Referenced by: '<Root>/Vp2_feedback'
                                        */
  real_T Rcs;                          /* Variable: Rcs
                                        * Referenced by: '<S1>/inv_Rcs'
                                        */
  real_T Rsa;                          /* Variable: Rsa
                                        * Referenced by: '<S1>/inv_Rsa'
                                        */
  real_T SoC_init;                     /* Variable: SoC_init
                                        * Referenced by: '<Root>/SoC_Integrator'
                                        */
  real_T T_amb;                        /* Variable: T_amb
                                        * Referenced by: '<S1>/T_amb_const'
                                        */
  real_T T_init;                       /* Variable: T_init
                                        * Referenced by:
                                        *   '<S1>/T_core_int'
                                        *   '<S1>/T_surf_int'
                                        */
  real_T Ts;                           /* Variable: Ts
                                        * Referenced by: '<Root>/dSoC_per_step'
                                        */
  real_T inv_C1_2d_fixed[36];          /* Variable: inv_C1_2d_fixed
                                        * Referenced by: '<Root>/C1_LUT'
                                        */
  real_T neg_inv_R1C1_2d_fixed[36];    /* Variable: neg_inv_R1C1_2d_fixed
                                        * Referenced by: '<Root>/R1C1_LUT'
                                        */
  real_T soc_common[12];               /* Variable: soc_common
                                        * Referenced by:
                                        *   '<Root>/C1_LUT'
                                        *   '<Root>/R0_LUT'
                                        *   '<Root>/R1C1_LUT'
                                        */
  real_T soc_ocv_common[101];          /* Variable: soc_ocv_common
                                        * Referenced by: '<Root>/OCV_LUT'
                                        */
  real_T temp_bp[3];                   /* Variable: temp_bp
                                        * Referenced by:
                                        *   '<Root>/C1_LUT'
                                        *   '<Root>/R0_LUT'
                                        *   '<Root>/R1C1_LUT'
                                        */
  real_T temp_bp_ocv[3];               /* Variable: temp_bp_ocv
                                        * Referenced by: '<Root>/OCV_LUT'
                                        */
  real_T SoC_Integrator_gainval;   /* Computed Parameter: SoC_Integrator_gainval
                                    * Referenced by: '<Root>/SoC_Integrator'
                                    */
  real_T SoC_Limit_UpperSat;           /* Expression: 1
                                        * Referenced by: '<Root>/SoC_Limit'
                                        */
  real_T SoC_Limit_LowerSat;           /* Expression: 0
                                        * Referenced by: '<Root>/SoC_Limit'
                                        */
  real_T T_core_int_gainval;           /* Computed Parameter: T_core_int_gainval
                                        * Referenced by: '<S1>/T_core_int'
                                        */
  real_T T_surf_int_gainval;           /* Computed Parameter: T_surf_int_gainval
                                        * Referenced by: '<S1>/T_surf_int'
                                        */
  real_T T_clamp_UpperSat;             /* Expression: 25
                                        * Referenced by: '<Root>/ T_clamp'
                                        */
  real_T T_clamp_LowerSat;             /* Expression: 0
                                        * Referenced by: '<Root>/ T_clamp'
                                        */
  real_T I_pack_off_Time;              /* Expression: 10
                                        * Referenced by: '<Root>/I_pack_off'
                                        */
  real_T I_pack_off_Y0;                /* Expression: 0
                                        * Referenced by: '<Root>/I_pack_off'
                                        */
  real_T I_pack_Time;                  /* Expression: 0
                                        * Referenced by: '<Root>/I_pack'
                                        */
  real_T I_pack_Y0;                    /* Expression: 0
                                        * Referenced by: '<Root>/I_pack'
                                        */
  real_T Vp1_Integrator1_gainval; /* Computed Parameter: Vp1_Integrator1_gainval
                                   * Referenced by: '<Root>/Vp1_Integrator1'
                                   */
  real_T Vp1_Integrator1_IC;           /* Expression: 0
                                        * Referenced by: '<Root>/Vp1_Integrator1'
                                        */
  real_T Vp2_Integrator_gainval;   /* Computed Parameter: Vp2_Integrator_gainval
                                    * Referenced by: '<Root>/Vp2_Integrator'
                                    */
  real_T Vp2_Integrator_IC;            /* Expression: 0
                                        * Referenced by: '<Root>/Vp2_Integrator'
                                        */
  uint32_T OCV_LUT_maxIndex[2];        /* Computed Parameter: OCV_LUT_maxIndex
                                        * Referenced by: '<Root>/OCV_LUT'
                                        */
  uint32_T R0_LUT_maxIndex[2];         /* Computed Parameter: R0_LUT_maxIndex
                                        * Referenced by: '<Root>/R0_LUT'
                                        */
  uint32_T C1_LUT_maxIndex[2];         /* Computed Parameter: C1_LUT_maxIndex
                                        * Referenced by: '<Root>/C1_LUT'
                                        */
  uint32_T R1C1_LUT_maxIndex[2];       /* Computed Parameter: R1C1_LUT_maxIndex
                                        * Referenced by: '<Root>/R1C1_LUT'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_single_cell_battery_T {
  const char_T *errorStatus;
  RTWLogInfo *rtwLogInfo;
  RTWSolverInfo solverInfo;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    time_T tFinal;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Block parameters (default storage) */
extern P_single_cell_battery_T single_cell_battery_P;

/* Block signals (default storage) */
extern B_single_cell_battery_T single_cell_battery_B;

/* Block states (default storage) */
extern DW_single_cell_battery_T single_cell_battery_DW;

/* Model entry point functions */
extern void single_cell_battery_initialize(void);
extern void single_cell_battery_step(void);
extern void single_cell_battery_terminate(void);

/* Real-time Model object */
extern RT_MODEL_single_cell_battery_T *const single_cell_battery_M;

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
 * '<Root>' : 'single_cell_battery'
 * '<S1>'   : 'single_cell_battery/Thermal_2Node'
 */
#endif                                 /* single_cell_battery_h_ */
