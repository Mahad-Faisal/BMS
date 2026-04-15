/*
 * Code generation for system model 'single_cell_battery'
 * For more details, see corresponding source file single_cell_battery.c
 *
 */

#ifndef single_cell_battery_h_
#define single_cell_battery_h_
#ifndef single_cell_battery_COMMON_INCLUDES_
#define single_cell_battery_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "math.h"
#endif                                /* single_cell_battery_COMMON_INCLUDES_ */

#include "single_cell_battery_types.h"

/* Real-time Model Data Structure */
struct tag_RTM_single_cell_battery_T {
  const char_T **errorStatus;
};

typedef struct {
  RT_MODEL_single_cell_battery_T rtm;
} MdlrefDW_single_cell_battery_T;

/* Model reference registration function */
extern void single_cell_battery_initialize(const char_T **rt_errorStatus,
  RT_MODEL_single_cell_battery_T *const single_cell_battery_M);

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
 */
#endif                                 /* single_cell_battery_h_ */
