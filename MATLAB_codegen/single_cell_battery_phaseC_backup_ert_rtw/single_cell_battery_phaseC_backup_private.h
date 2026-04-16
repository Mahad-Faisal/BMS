/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: single_cell_battery_phaseC_backup_private.h
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

#ifndef single_cell_battery_phaseC_backup_private_h_
#define single_cell_battery_phaseC_backup_private_h_
#include "rtwtypes.h"
#include "single_cell_battery_phaseC_backup_types.h"

extern const real32_T rtCP_pooled_uwvSToCbsBYq[36];
extern const real32_T rtCP_pooled_PxQEIk43GBr8[12];
extern const real32_T rtCP_pooled_3IVAVtd7XRjw[3];
extern const real32_T rtCP_pooled_BLIyxuXI4XGc[303];
extern const real32_T rtCP_pooled_LAE26UlapTkv[101];
extern const real32_T rtCP_pooled_ssO8X8RGT7pD[36];
extern const real32_T rtCP_pooled_eLcorhlMmJdH[36];
extern const real32_T rtCP_pooled_cRHyoZnfFYsm[36];
extern const uint32_T rtCP_pooled_0bbcgCIiHXCR[2];
extern const uint32_T rtCP_pooled_npkPQKZ7Jhkk[2];

#define rtCP_R0_LUT_tableData          rtCP_pooled_uwvSToCbsBYq  /* Expression: R0_2d_fix
                                                                  * Referenced by: '<Root>/R0_LUT'
                                                                  */
#define rtCP_R0_LUT_bp01Data           rtCP_pooled_PxQEIk43GBr8  /* Expression: soc_common
                                                                  * Referenced by: '<Root>/R0_LUT'
                                                                  */
#define rtCP_R0_LUT_bp02Data           rtCP_pooled_3IVAVtd7XRjw  /* Expression: temp_bp
                                                                  * Referenced by: '<Root>/R0_LUT'
                                                                  */
#define rtCP_OCV_LUT_tableData         rtCP_pooled_BLIyxuXI4XGc  /* Expression: OCV_2d
                                                                  * Referenced by: '<Root>/OCV_LUT'
                                                                  */
#define rtCP_OCV_LUT_bp01Data          rtCP_pooled_LAE26UlapTkv  /* Expression: soc_ocv_common
                                                                  * Referenced by: '<Root>/OCV_LUT'
                                                                  */
#define rtCP_OCV_LUT_bp02Data          rtCP_pooled_3IVAVtd7XRjw  /* Expression: temp_bp_ocv
                                                                  * Referenced by: '<Root>/OCV_LUT'
                                                                  */
#define rtCP_nDLookupTable_tableData   rtCP_pooled_ssO8X8RGT7pD  /* Expression: R1_2d
                                                                  * Referenced by: '<Root>/n-D Lookup Table'
                                                                  */
#define rtCP_nDLookupTable_bp01Data    rtCP_pooled_PxQEIk43GBr8  /* Expression: soc_common
                                                                  * Referenced by: '<Root>/n-D Lookup Table'
                                                                  */
#define rtCP_nDLookupTable_bp02Data    rtCP_pooled_3IVAVtd7XRjw  /* Expression: temp_bp
                                                                  * Referenced by: '<Root>/n-D Lookup Table'
                                                                  */
#define rtCP_R1C1_LUT_tableData        rtCP_pooled_eLcorhlMmJdH  /* Expression: neg_inv_R1C1_2d_fixed
                                                                  * Referenced by: '<Root>/R1C1_LUT'
                                                                  */
#define rtCP_R1C1_LUT_bp01Data         rtCP_pooled_PxQEIk43GBr8  /* Expression: soc_common
                                                                  * Referenced by: '<Root>/R1C1_LUT'
                                                                  */
#define rtCP_R1C1_LUT_bp02Data         rtCP_pooled_3IVAVtd7XRjw  /* Expression: temp_bp
                                                                  * Referenced by: '<Root>/R1C1_LUT'
                                                                  */
#define rtCP_C1_LUT_tableData          rtCP_pooled_cRHyoZnfFYsm  /* Expression: inv_C1_2d_fixed
                                                                  * Referenced by: '<Root>/C1_LUT'
                                                                  */
#define rtCP_C1_LUT_bp01Data           rtCP_pooled_PxQEIk43GBr8  /* Expression: soc_common
                                                                  * Referenced by: '<Root>/C1_LUT'
                                                                  */
#define rtCP_C1_LUT_bp02Data           rtCP_pooled_3IVAVtd7XRjw  /* Expression: temp_bp
                                                                  * Referenced by: '<Root>/C1_LUT'
                                                                  */
#define rtCP_R0_LUT_maxIndex           rtCP_pooled_0bbcgCIiHXCR  /* Computed Parameter: rtCP_R0_LUT_maxIndex
                                                                  * Referenced by: '<Root>/R0_LUT'
                                                                  */
#define rtCP_OCV_LUT_maxIndex          rtCP_pooled_npkPQKZ7Jhkk  /* Computed Parameter: rtCP_OCV_LUT_maxIndex
                                                                  * Referenced by: '<Root>/OCV_LUT'
                                                                  */
#define rtCP_nDLookupTable_maxIndex    rtCP_pooled_0bbcgCIiHXCR  /* Computed Parameter: rtCP_nDLookupTable_maxIndex
                                                                  * Referenced by: '<Root>/n-D Lookup Table'
                                                                  */
#define rtCP_R1C1_LUT_maxIndex         rtCP_pooled_0bbcgCIiHXCR  /* Computed Parameter: rtCP_R1C1_LUT_maxIndex
                                                                  * Referenced by: '<Root>/R1C1_LUT'
                                                                  */
#define rtCP_C1_LUT_maxIndex           rtCP_pooled_0bbcgCIiHXCR  /* Computed Parameter: rtCP_C1_LUT_maxIndex
                                                                  * Referenced by: '<Root>/C1_LUT'
                                                                  */
#endif                        /* single_cell_battery_phaseC_backup_private_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
