#include "rtw_capi.h"
#ifdef HOST_CAPI_BUILD
#include "single_cell_battery_capi_host.h"
#define sizeof(...) ((size_t)(0xFFFF))
#undef rt_offsetof
#define rt_offsetof(s,el) ((uint16_T)(0xFFFF))
#define TARGET_CONST
#define TARGET_STRING(s) (s)
#ifndef SS_UINT64
#define SS_UINT64 17
#endif
#ifndef SS_INT64
#define SS_INT64 18
#endif
#else
#include "builtin_typeid_types.h"
#include "single_cell_battery.h"
#include "single_cell_battery_capi.h"
#include "single_cell_battery_private.h"
#ifdef LIGHT_WEIGHT_CAPI
#define TARGET_CONST
#define TARGET_STRING(s)               ((NULL))
#else
#define TARGET_CONST                   const
#define TARGET_STRING(s)               (s)
#endif
#endif
static const rtwCAPI_Signals rtBlockSignals [ ] = { { 0 , 0 , TARGET_STRING ( "single_cell_battery/Clock" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 1 , 0 , TARGET_STRING ( "single_cell_battery/Vp1_Integrator1" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 2 , 0 , TARGET_STRING ( "single_cell_battery/Vp2_Integrator" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 3 , 0 , TARGET_STRING ( "single_cell_battery/Vp2_feedback" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 4 , 0 , TARGET_STRING ( "single_cell_battery/dSoC_per_step" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 5 , 0 , TARGET_STRING ( "single_cell_battery/C1_LUT" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 6 , 0 , TARGET_STRING ( "single_cell_battery/OCV_LUT" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 7 , 0 , TARGET_STRING ( "single_cell_battery/R0_LUT" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 8 , 0 , TARGET_STRING ( "single_cell_battery/Product1" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 9 , 0 , TARGET_STRING ( "single_cell_battery/SoC_Limit" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 10 , 0 , TARGET_STRING ( "single_cell_battery/Vcell_Calc" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 11 , 0 , TARGET_STRING ( "single_cell_battery/Vp1_dot" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 12 , 0 , TARGET_STRING ( "single_cell_battery/Vp2_dot" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 0 , 0 , ( NULL ) , ( NULL ) , 0 , 0 , 0 , 0 , 0 } } ; static const rtwCAPI_BlockParameters rtBlockParameters [ ] = { { 13 , TARGET_STRING ( "single_cell_battery/T_cell" ) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 14 , TARGET_STRING ( "single_cell_battery/SoC_Integrator" ) , TARGET_STRING ( "gainval" ) , 0 , 0 , 0 } , { 15 , TARGET_STRING ( "single_cell_battery/Vp1_Integrator1" ) , TARGET_STRING ( "gainval" ) , 0 , 0 , 0 } , { 16 , TARGET_STRING ( "single_cell_battery/Vp1_Integrator1" ) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 17 , TARGET_STRING ( "single_cell_battery/Vp2_Integrator" ) , TARGET_STRING ( "gainval" ) , 0 , 0 , 0 } , { 18 , TARGET_STRING ( "single_cell_battery/Vp2_Integrator" ) , TARGET_STRING ( "InitialCondition" ) , 0 , 0 , 0 } , { 19 , TARGET_STRING ( "single_cell_battery/From Workspace" ) , TARGET_STRING ( "Time0" ) , 0 , 1 , 0 } , { 20 , TARGET_STRING ( "single_cell_battery/From Workspace" ) , TARGET_STRING ( "Data0" ) , 0 , 1 , 0 } , { 21 , TARGET_STRING ( "single_cell_battery/C1_LUT" ) , TARGET_STRING ( "maxIndex" ) , 1 , 2 , 0 } , { 22 , TARGET_STRING ( "single_cell_battery/R0_LUT" ) , TARGET_STRING ( "maxIndex" ) , 1 , 2 , 0 } , { 23 , TARGET_STRING ( "single_cell_battery/R1C1_LUT" ) , TARGET_STRING ( "maxIndex" ) , 1 , 2 , 0 } , { 24 , TARGET_STRING ( "single_cell_battery/SoC_Limit" ) , TARGET_STRING ( "UpperLimit" ) , 0 , 0 , 0 } , { 25 , TARGET_STRING ( "single_cell_battery/SoC_Limit" ) , TARGET_STRING ( "LowerLimit" ) , 0 , 0 , 0 } , { 0 , ( NULL ) , ( NULL ) , 0 , 0 , 0 } } ; static int_T rt_LoggedStateIdxList [ ] = { - 1 } ; static const rtwCAPI_Signals rtRootInputs [ ] = { { 0 , 0 , ( NULL ) , ( NULL ) , 0 , 0 , 0 , 0 , 0 } } ; static const rtwCAPI_Signals rtRootOutputs [ ] = { { 0 , 0 , ( NULL ) , ( NULL ) , 0 , 0 , 0 , 0 , 0 } } ; static const rtwCAPI_ModelParameters rtModelParameters [ ] = { { 26 , TARGET_STRING ( "C2" ) , 0 , 0 , 0 } , { 27 , TARGET_STRING ( "Q_nom" ) , 0 , 0 , 0 } , { 28 , TARGET_STRING ( "R0_2d_fix" ) , 0 , 3 , 0 } , { 29 , TARGET_STRING ( "R2" ) , 0 , 0 , 0 } , { 30 , TARGET_STRING ( "SoC_init" ) , 0 , 0 , 0 } , { 31 , TARGET_STRING ( "Ts" ) , 0 , 0 , 0 } , { 32 , TARGET_STRING ( "inv_C1_2d_fixed" ) , 0 , 3 , 0 } , { 33 , TARGET_STRING ( "neg_inv_R1C1_2d_fixed" ) , 0 , 3 , 0 } , { 34 , TARGET_STRING ( "ocv_tbl" ) , 0 , 4 , 0 } , { 35 , TARGET_STRING ( "soc_bp" ) , 0 , 4 , 0 } , { 36 , TARGET_STRING ( "soc_common" ) , 0 , 5 , 0 } , { 37 , TARGET_STRING ( "temp_bp" ) , 0 , 6 , 0 } , { 0 , ( NULL ) , 0 , 0 , 0 } } ;
#ifndef HOST_CAPI_BUILD
static void * rtDataAddrMap [ ] = { & rtB . n24bl5rrpa , & rtB . mfdtfolo3m ,
& rtB . lvxiepm0a3 , & rtB . jbsvi3ud4a , & rtB . edd3psk1lf , & rtB .
d2mux5pgvk , & rtB . ljosjquhii , & rtB . e3eeyyf5wy , & rtB . big3stfbru , &
rtB . kehulh2wu3 , & rtB . njs2105xii , & rtB . mhl1hybhar , & rtB .
ioavimueej , & rtP . T_cell_Value , & rtP . SoC_Integrator_gainval , & rtP .
Vp1_Integrator1_gainval , & rtP . Vp1_Integrator1_IC , & rtP .
Vp2_Integrator_gainval , & rtP . Vp2_Integrator_IC , & rtP .
FromWorkspace_Time0 [ 0 ] , & rtP . FromWorkspace_Data0 [ 0 ] , & rtP .
C1_LUT_maxIndex [ 0 ] , & rtP . R0_LUT_maxIndex [ 0 ] , & rtP .
R1C1_LUT_maxIndex [ 0 ] , & rtP . SoC_Limit_UpperSat , & rtP .
SoC_Limit_LowerSat , & rtP . C2 , & rtP . Q_nom , & rtP . R0_2d_fix [ 0 ] , &
rtP . R2 , & rtP . SoC_init , & rtP . Ts , & rtP . inv_C1_2d_fixed [ 0 ] , &
rtP . neg_inv_R1C1_2d_fixed [ 0 ] , & rtP . ocv_tbl [ 0 ] , & rtP . soc_bp [
0 ] , & rtP . soc_common [ 0 ] , & rtP . temp_bp [ 0 ] , } ; static int32_T *
rtVarDimsAddrMap [ ] = { ( NULL ) } ;
#endif
static TARGET_CONST rtwCAPI_DataTypeMap rtDataTypeMap [ ] = { { "double" ,
"real_T" , 0 , 0 , sizeof ( real_T ) , ( uint8_T ) SS_DOUBLE , 0 , 0 , 0 } ,
{ "unsigned int" , "uint32_T" , 0 , 0 , sizeof ( uint32_T ) , ( uint8_T )
SS_UINT32 , 0 , 0 , 0 } } ;
#ifdef HOST_CAPI_BUILD
#undef sizeof
#endif
static TARGET_CONST rtwCAPI_ElementMap rtElementMap [ ] = { { ( NULL ) , 0 ,
0 , 0 , 0 } , } ; static const rtwCAPI_DimensionMap rtDimensionMap [ ] = { {
rtwCAPI_SCALAR , 0 , 2 , 0 } , { rtwCAPI_VECTOR , 2 , 2 , 0 } , {
rtwCAPI_VECTOR , 4 , 2 , 0 } , { rtwCAPI_MATRIX_COL_MAJOR , 6 , 2 , 0 } , {
rtwCAPI_VECTOR , 8 , 2 , 0 } , { rtwCAPI_VECTOR , 10 , 2 , 0 } , {
rtwCAPI_VECTOR , 12 , 2 , 0 } } ; static const uint_T rtDimensionArray [ ] =
{ 1 , 1 , 13999 , 1 , 2 , 1 , 12 , 3 , 21 , 1 , 12 , 1 , 1 , 3 } ; static
const real_T rtcapiStoredFloats [ ] = { 0.0 , 0.1 } ; static const
rtwCAPI_FixPtMap rtFixPtMap [ ] = { { ( NULL ) , ( NULL ) ,
rtwCAPI_FIX_RESERVED , 0 , 0 , ( boolean_T ) 0 } , } ; static const
rtwCAPI_SampleTimeMap rtSampleTimeMap [ ] = { { ( const void * ) &
rtcapiStoredFloats [ 0 ] , ( const void * ) & rtcapiStoredFloats [ 0 ] , ( int8_T ) 0 , ( uint8_T ) 0 } , { ( const void * ) & rtcapiStoredFloats [ 1 ] , ( const void * ) & rtcapiStoredFloats [ 0 ] , ( int8_T ) 1 , ( uint8_T ) 0 } } ; static rtwCAPI_ModelMappingStaticInfo mmiStatic = { { rtBlockSignals , 13 , rtRootInputs , 0 , rtRootOutputs , 0 } , { rtBlockParameters , 13 , rtModelParameters , 12 } , { ( NULL ) , 0 } , { rtDataTypeMap , rtDimensionMap , rtFixPtMap , rtElementMap , rtSampleTimeMap , rtDimensionArray } , "float" , { 1548344874U , 3894418177U , 829326392U , 4263970587U } , ( NULL ) , 0 , ( boolean_T ) 0 , rt_LoggedStateIdxList } ; const rtwCAPI_ModelMappingStaticInfo * single_cell_battery_GetCAPIStaticMap ( void ) { return & mmiStatic ; }
#ifndef HOST_CAPI_BUILD
void single_cell_battery_InitializeDataMapInfo ( void ) { rtwCAPI_SetVersion
( ( * rt_dataMapInfoPtr ) . mmi , 1 ) ; rtwCAPI_SetStaticMap ( ( *
rt_dataMapInfoPtr ) . mmi , & mmiStatic ) ; rtwCAPI_SetLoggingStaticMap ( ( *
rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ; rtwCAPI_SetDataAddressMap ( ( *
rt_dataMapInfoPtr ) . mmi , rtDataAddrMap ) ; rtwCAPI_SetVarDimsAddressMap ( ( *
rt_dataMapInfoPtr ) . mmi , rtVarDimsAddrMap ) ;
rtwCAPI_SetInstanceLoggingInfo ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArray ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArrayLen ( ( * rt_dataMapInfoPtr ) . mmi , 0 ) ; }
#else
#ifdef __cplusplus
extern "C" {
#endif
void single_cell_battery_host_InitializeDataMapInfo ( single_cell_battery_host_DataMapInfo_T * dataMap , const char * path ) { rtwCAPI_SetVersion ( dataMap -> mmi , 1 ) ; rtwCAPI_SetStaticMap ( dataMap -> mmi , & mmiStatic ) ; rtwCAPI_SetDataAddressMap ( dataMap -> mmi , ( NULL ) ) ; rtwCAPI_SetVarDimsAddressMap ( dataMap -> mmi , ( NULL ) ) ; rtwCAPI_SetPath ( dataMap -> mmi , path ) ; rtwCAPI_SetFullPath ( dataMap -> mmi , ( NULL ) ) ; rtwCAPI_SetChildMMIArray ( dataMap -> mmi , ( NULL ) ) ; rtwCAPI_SetChildMMIArrayLen ( dataMap -> mmi , 0 ) ; }
#ifdef __cplusplus
}
#endif
#endif
