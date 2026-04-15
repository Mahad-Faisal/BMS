#ifndef single_cell_battery_h_
#define single_cell_battery_h_
#ifndef single_cell_battery_COMMON_INCLUDES_
#define single_cell_battery_COMMON_INCLUDES_
#include <stdlib.h>
#include "sl_AsyncioQueue/AsyncioQueueCAPI.h"
#include "rtwtypes.h"
#include "sigstream_rtw.h"
#include "simtarget/slSimTgtSigstreamRTW.h"
#include "simtarget/slSimTgtSlioCoreRTW.h"
#include "simtarget/slSimTgtSlioClientsRTW.h"
#include "simtarget/slSimTgtSlioSdiRTW.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "raccel.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "rt_logging_simtarget.h"
#include "rt_nonfinite.h"
#include "math.h"
#include "dt_info.h"
#include "ext_work.h"
#endif
#include "single_cell_battery_types.h"
#include "mwmathutil.h"
#include <stddef.h>
#include "rtw_modelmap_simtarget.h"
#include "rt_defines.h"
#include <string.h>
#define MODEL_NAME single_cell_battery
#define NSAMPLE_TIMES (3) 
#define NINPUTS (0)       
#define NOUTPUTS (0)     
#define NBLOCKIO (13) 
#define NUM_ZC_EVENTS (0) 
#ifndef NCSTATES
#define NCSTATES (0)   
#elif NCSTATES != 0
#error Invalid specification of NCSTATES defined in compiler command
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm) (*rt_dataMapInfoPtr)
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val) (rt_dataMapInfoPtr = &val)
#endif
#ifndef IN_RACCEL_MAIN
#endif
typedef struct { real_T kehulh2wu3 ; real_T ljosjquhii ; real_T e3eeyyf5wy ;
real_T mfdtfolo3m ; real_T lvxiepm0a3 ; real_T njs2105xii ; real_T d2mux5pgvk
; real_T big3stfbru ; real_T mhl1hybhar ; real_T jbsvi3ud4a ; real_T
ioavimueej ; real_T edd3psk1lf ; real_T n24bl5rrpa ; } B ; typedef struct {
real_T jeh3n4o4tq ; real_T g2nxhtulxs ; real_T na3yvoldgm ; struct { void *
LoggedData ; } glb3z2swig ; struct { void * TimePtr ; void * DataPtr ; void *
RSimInfoPtr ; } kg3edejg25 ; struct { void * LoggedData ; } gd0mp3yj00 ;
struct { void * LoggedData ; } p0xxvfssv0 ; struct { void * AQHandles ; }
dnkslxlunw ; struct { void * LoggedData ; } eq441zyepa ; struct { void *
AQHandles ; } kccth0tgsi ; uint32_T pk5clu0nyh ; struct { int_T PrevIndex ; }
oti50icfwb ; } DW ; typedef struct { rtwCAPI_ModelMappingInfo mmi ; }
DataMapInfo ; struct P_ { real_T C2 ; real_T Q_nom ; real_T R0_2d_fix [ 36 ]
; real_T R2 ; real_T SoC_init ; real_T Ts ; real_T inv_C1_2d_fixed [ 36 ] ;
real_T neg_inv_R1C1_2d_fixed [ 36 ] ; real_T ocv_tbl [ 21 ] ; real_T soc_bp [
21 ] ; real_T soc_common [ 12 ] ; real_T temp_bp [ 3 ] ; real_T
SoC_Integrator_gainval ; real_T SoC_Limit_UpperSat ; real_T
SoC_Limit_LowerSat ; real_T FromWorkspace_Time0 [ 13999 ] ; real_T
FromWorkspace_Data0 [ 13999 ] ; real_T Vp1_Integrator1_gainval ; real_T
Vp1_Integrator1_IC ; real_T Vp2_Integrator_gainval ; real_T Vp2_Integrator_IC
; real_T T_cell_Value ; uint32_T R0_LUT_maxIndex [ 2 ] ; uint32_T
C1_LUT_maxIndex [ 2 ] ; uint32_T R1C1_LUT_maxIndex [ 2 ] ; } ; extern const
char_T * RT_MEMORY_ALLOCATION_ERROR ; extern B rtB ; extern DW rtDW ; extern
P rtP ; extern mxArray * mr_single_cell_battery_GetDWork ( ) ; extern void
mr_single_cell_battery_SetDWork ( const mxArray * ssDW ) ; extern mxArray *
mr_single_cell_battery_GetSimStateDisallowedBlocks ( ) ; extern const
rtwCAPI_ModelMappingStaticInfo * single_cell_battery_GetCAPIStaticMap ( void
) ; extern SimStruct * const rtS ; extern DataMapInfo * rt_dataMapInfoPtr ;
extern rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr ; void MdlOutputs ( int_T
tid ) ; void MdlOutputsParameterSampleTime ( int_T tid ) ; void MdlUpdate ( int_T tid ) ; void MdlTerminate ( void ) ; void MdlInitializeSizes ( void ) ; void MdlInitializeSampleTimes ( void ) ; SimStruct * raccel_register_model ( ssExecutionInfo * executionInfo ) ;
#endif
