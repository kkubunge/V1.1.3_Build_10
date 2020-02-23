#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "MFCFlowTest.h"

#define FUNC_NAME		"MFC_FlowTest"
CMFCFlowTest		g_SeqInterface;

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	{ "INTLKS_CONTROL"			, _K_D_IO	,	INTLKS_CONTROL		,	0	} ,
	{ "VRCP_RunningDI"			, _K_D_IO	,	VRCP_RunningDI		,	0	} ,
	{ "PLC_GbIntlckDI"			, _K_D_IO	,	PLC_GbIntlckDI		,	0	} ,
	{ "SW_GbIntlckDM"			, _K_D_IO	,	SW_GbIntlckDM		,	0	} ,

	{ "MFC_FLOW_TEST"			, _K_D_IO	, 	MFC_FLOW_TEST	 	,	0	} ,
	{ "MFC_PUMP_TIME"			, _K_A_IO	, 	MFC_PUMP_TIME	 	,	0	} ,	
	{ "MFC_CHECK_DELAY"			, _K_A_IO	, 	MFC_CHECK_DELAY	 	,	0	} ,	
	{ "MFC_CHECK_TIME"			, _K_A_IO	, 	MFC_CHECK_TIME	 	,	0	} ,	
	{ "MFC_FL0W_TOL"			, _K_A_IO	, 	MFC_FL0W_TOL	 	,	0	} ,	
	{ "MFlow_FlwErrLmtDM"		, _K_D_IO	, 	MFlow_FlwErrLmtDM	,	0	} ,	

	{ "MFlow_StartChmPrs"		, _K_A_IO	, 	MFlow_StartChmPrs	,	0	} ,
	{ "MFlow_StartPumpPrs"		, _K_A_IO	, 	MFlow_StartPumpPrs	,	0	} ,

	{ "MFC01StPtAO"				, _K_A_IO	, 	MFC01StPtAO		 	,	0	} ,
	{ "MFC02StPtAO"				, _K_A_IO	, 	MFC02StPtAO		 	,	0	} ,
	{ "MFC03StPtAO"				, _K_A_IO	, 	MFC03StPtAO		 	,	0	} ,
	{ "MFC04StPtAO"				, _K_A_IO	, 	MFC04StPtAO		 	,	0	} ,
	{ "MFC05StPtAO"				, _K_A_IO	, 	MFC05StPtAO		 	,	0	} ,
	{ "MFC06StPtAO"				, _K_A_IO	, 	MFC06StPtAO		 	,	0	} ,
	{ "MFC07StPtAO"				, _K_A_IO	, 	MFC07StPtAO		 	,	0	} ,
	{ "MFC08StPtAO"				, _K_A_IO	, 	MFC08StPtAO		 	,	0	} ,
	{ "MFC09StPtAO"				, _K_A_IO	, 	MFC09StPtAO		 	,	0	} ,
	{ "MFC10StPtAO"				, _K_A_IO	, 	MFC10StPtAO		 	,	0	} ,
	{ "MFC11StPtAO"				, _K_A_IO	, 	MFC11StPtAO		 	,	0	} ,
	{ "MFC12StPtAO"				, _K_A_IO	, 	MFC12StPtAO		 	,	0	} ,
	{ "MFC13StPtAO"				, _K_A_IO	, 	MFC13StPtAO		 	,	0	} ,
	{ "MFC14StPtAO"				, _K_A_IO	, 	MFC14StPtAO		 	,	0	} ,
	{ "MFC15StPtAO"				, _K_A_IO	, 	MFC15StPtAO		 	,	0	} ,
	{ "MFC16StPtAO"				, _K_A_IO	, 	MFC16StPtAO		 	,	0	} ,
	{ "MFC17StPtAO"				, _K_A_IO	, 	MFC17StPtAO		 	,	0	} ,

	{ "MFC01FlwAI"				, _K_A_IO	, 	MFC01FlwAI		 	,	0	} ,
	{ "MFC02FlwAI"				, _K_A_IO	, 	MFC02FlwAI		 	,	0	} ,
	{ "MFC03FlwAI"				, _K_A_IO	, 	MFC03FlwAI		 	,	0	} ,
	{ "MFC04FlwAI"				, _K_A_IO	, 	MFC04FlwAI		 	,	0	} ,
	{ "MFC05FlwAI"				, _K_A_IO	, 	MFC05FlwAI		 	,	0	} ,
	{ "MFC06FlwAI"				, _K_A_IO	, 	MFC06FlwAI		 	,	0	} ,
	{ "MFC07FlwAI"				, _K_A_IO	, 	MFC07FlwAI		 	,	0	} ,
	{ "MFC08FlwAI"				, _K_A_IO	, 	MFC08FlwAI		 	,	0	} ,
	{ "MFC09FlwAI"				, _K_A_IO	, 	MFC09FlwAI		 	,	0	} ,
	{ "MFC10FlwAI"				, _K_A_IO	, 	MFC10FlwAI		 	,	0	} ,
	{ "MFC11FlwAI"				, _K_A_IO	, 	MFC11FlwAI		 	,	0	} ,
	{ "MFC12FlwAI"				, _K_A_IO	, 	MFC12FlwAI		 	,	0	} ,
	{ "MFC13FlwAI"				, _K_A_IO	, 	MFC13FlwAI		 	,	0	} ,
	{ "MFC14FlwAI"				, _K_A_IO	, 	MFC14FlwAI		 	,	0	} ,
	{ "MFC15FlwAI"				, _K_A_IO	, 	MFC15FlwAI		 	,	0	} ,
	{ "MFC16FlwAI"				, _K_A_IO	, 	MFC16FlwAI		 	,	0	} ,
	{ "MFC17FlwAI"				, _K_A_IO	, 	MFC17FlwAI		 	,	0	} ,

	{ "M651_Pressure"		 	, _K_A_IO	, 	M651_Pressure		,	0	} ,
	{ "M651_Position"			, _K_A_IO	,	M651_Position		,	0	} ,
	{ "PumpPrsAI"				, _K_A_IO	,	PumpPrsAI			,	0	} ,

	//---> Virtual IO
	{ "MFlow_MFC01_Name"		, _K_S_IO	,	MFC01_Name			,	0	} ,
	{ "MFlow_MFC02_Name"		, _K_S_IO	,	MFC02_Name			,	0	} ,
	{ "MFlow_MFC03_Name"		, _K_S_IO	,	MFC03_Name			,	0	} ,
	{ "MFlow_MFC04_Name"		, _K_S_IO	,	MFC04_Name			,	0	} ,
	{ "MFlow_MFC05_Name"		, _K_S_IO	,	MFC05_Name			,	0	} ,
	{ "MFlow_MFC06_Name"		, _K_S_IO	,	MFC06_Name			,	0	} ,
	{ "MFlow_MFC07_Name"		, _K_S_IO	,	MFC07_Name			,	0	} ,
	{ "MFlow_MFC08_Name"		, _K_S_IO	,	MFC08_Name			,	0	} ,
	{ "MFlow_MFC09_Name"		, _K_S_IO	,	MFC09_Name			,	0	} ,
	{ "MFlow_MFC10_Name"		, _K_S_IO	,	MFC10_Name			,	0	} ,
	{ "MFlow_MFC11_Name"		, _K_S_IO	,	MFC11_Name			,	0	} ,
	{ "MFlow_MFC12_Name"		, _K_S_IO	,	MFC12_Name			,	0	} ,
	{ "MFlow_MFC13_Name"		, _K_S_IO	,	MFC13_Name			,	0	} ,
	{ "MFlow_MFC14_Name"		, _K_S_IO	,	MFC14_Name			,	0	} ,
	{ "MFlow_MFC15_Name"		, _K_S_IO	,	MFC15_Name			,	0	} ,
	{ "MFlow_MFC16_Name"		, _K_S_IO	,	MFC16_Name			,	0	} ,
	{ "MFlow_MFC17_Name"		, _K_S_IO	,	MFC17_Name			,	0	} ,

	{ "MFC_TEST_INFO"			, _K_S_IO	,	MFC_TEST_INFO		,	0	} ,

	{ "MFlow_MFC01"				, _K_D_IO	,	MFlow_MFC01			,	0	} ,
	{ "MFlow_MFC02"				, _K_D_IO	,	MFlow_MFC02			,	0	} ,
	{ "MFlow_MFC03"				, _K_D_IO	,	MFlow_MFC03			,	0	} ,
	{ "MFlow_MFC04"				, _K_D_IO	,	MFlow_MFC04			,	0	} ,
	{ "MFlow_MFC05"				, _K_D_IO	,	MFlow_MFC05			,	0	} ,
	{ "MFlow_MFC06"				, _K_D_IO	,	MFlow_MFC06			,	0	} ,
	{ "MFlow_MFC07"				, _K_D_IO	,	MFlow_MFC07			,	0	} ,
	{ "MFlow_MFC08"				, _K_D_IO	,	MFlow_MFC08			,	0	} ,
	{ "MFlow_MFC09"				, _K_D_IO	,	MFlow_MFC09			,	0	} ,
	{ "MFlow_MFC10"				, _K_D_IO	,	MFlow_MFC10			,	0	} ,
	{ "MFlow_MFC11"				, _K_D_IO	,	MFlow_MFC11			,	0	} ,
	{ "MFlow_MFC12"				, _K_D_IO	,	MFlow_MFC12			,	0	} ,
	{ "MFlow_MFC13"				, _K_D_IO	,	MFlow_MFC13			,	0	} ,
	{ "MFlow_MFC14"				, _K_D_IO	,	MFlow_MFC14			,	0	} ,
	{ "MFlow_MFC15"				, _K_D_IO	,	MFlow_MFC15			,	0	} ,
	{ "MFlow_MFC16"				, _K_D_IO	,	MFlow_MFC16			,	0	} ,
	{ "MFlow_MFC17"				, _K_D_IO	,	MFlow_MFC17			,	0	} ,

	{ "MFlow_MFC01_Result"		, _K_D_IO	,	MFlow_MFC01_Result	,	0	} ,
	{ "MFlow_MFC02_Result"		, _K_D_IO	,	MFlow_MFC02_Result	,	0	} ,
	{ "MFlow_MFC03_Result"		, _K_D_IO	,	MFlow_MFC03_Result	,	0	} ,
	{ "MFlow_MFC04_Result"		, _K_D_IO	,	MFlow_MFC04_Result	,	0	} ,
	{ "MFlow_MFC05_Result"		, _K_D_IO	,	MFlow_MFC05_Result	,	0	} ,
	{ "MFlow_MFC06_Result"		, _K_D_IO	,	MFlow_MFC06_Result	,	0	} ,
	{ "MFlow_MFC07_Result"		, _K_D_IO	,	MFlow_MFC07_Result	,	0	} ,
	{ "MFlow_MFC08_Result"		, _K_D_IO	,	MFlow_MFC08_Result	,	0	} ,
	{ "MFlow_MFC09_Result"		, _K_D_IO	,	MFlow_MFC09_Result	,	0	} ,
	{ "MFlow_MFC10_Result"		, _K_D_IO	,	MFlow_MFC10_Result	,	0	} ,
	{ "MFlow_MFC11_Result"		, _K_D_IO	,	MFlow_MFC11_Result	,	0	} ,
	{ "MFlow_MFC12_Result"		, _K_D_IO	,	MFlow_MFC12_Result	,	0	} ,
	{ "MFlow_MFC13_Result"		, _K_D_IO	,	MFlow_MFC13_Result	,	0	} ,
	{ "MFlow_MFC14_Result"		, _K_D_IO	,	MFlow_MFC14_Result	,	0	} ,
	{ "MFlow_MFC15_Result"		, _K_D_IO	,	MFlow_MFC15_Result	,	0	} ,
	{ "MFlow_MFC16_Result"		, _K_D_IO	,	MFlow_MFC16_Result	,	0	} ,
	{ "MFlow_MFC17_Result"		, _K_D_IO	,	MFlow_MFC17_Result	,	0	} ,

	{ "MFlow_MFC01_SET1"		, _K_A_IO	,	MFlow_MFC01_SET1		,	0	} ,
	{ "MFlow_MFC01_SET2"		, _K_A_IO	,	MFlow_MFC01_SET2		,	0	} ,
	{ "MFlow_MFC01_SET3"		, _K_A_IO	,	MFlow_MFC01_SET3		,	0	} ,
	{ "MFlow_MFC01_SET4"		, _K_A_IO	,	MFlow_MFC01_SET4		,	0	} ,
	{ "MFlow_MFC01_SET5"		, _K_A_IO	,	MFlow_MFC01_SET5		,	0	} ,

	{ "MFlow_MFC02_SET1"		, _K_A_IO	,	MFlow_MFC02_SET1		,	0	} ,
	{ "MFlow_MFC02_SET2"		, _K_A_IO	,	MFlow_MFC02_SET2		,	0	} ,
	{ "MFlow_MFC02_SET3"		, _K_A_IO	,	MFlow_MFC02_SET3		,	0	} ,
	{ "MFlow_MFC02_SET4"		, _K_A_IO	,	MFlow_MFC02_SET4		,	0	} ,
	{ "MFlow_MFC02_SET5"		, _K_A_IO	,	MFlow_MFC02_SET5		,	0	} ,
	
	{ "MFlow_MFC03_SET1"		, _K_A_IO	,	MFlow_MFC03_SET1		,	0	} ,
	{ "MFlow_MFC03_SET2"		, _K_A_IO	,	MFlow_MFC03_SET2		,	0	} ,
	{ "MFlow_MFC03_SET3"		, _K_A_IO	,	MFlow_MFC03_SET3		,	0	} ,
	{ "MFlow_MFC03_SET4"		, _K_A_IO	,	MFlow_MFC03_SET4		,	0	} ,
	{ "MFlow_MFC03_SET5"		, _K_A_IO	,	MFlow_MFC03_SET5		,	0	} ,	
	
	{ "MFlow_MFC04_SET1"		, _K_A_IO	,	MFlow_MFC04_SET1		,	0	} ,
	{ "MFlow_MFC04_SET2"		, _K_A_IO	,	MFlow_MFC04_SET2		,	0	} ,
	{ "MFlow_MFC04_SET3"		, _K_A_IO	,	MFlow_MFC04_SET3		,	0	} ,
	{ "MFlow_MFC04_SET4"		, _K_A_IO	,	MFlow_MFC04_SET4		,	0	} ,
	{ "MFlow_MFC04_SET5"		, _K_A_IO	,	MFlow_MFC04_SET5		,	0	} ,

	{ "MFlow_MFC05_SET1"		, _K_A_IO	,	MFlow_MFC05_SET1		,	0	} ,
	{ "MFlow_MFC05_SET2"		, _K_A_IO	,	MFlow_MFC05_SET2		,	0	} ,
	{ "MFlow_MFC05_SET3"		, _K_A_IO	,	MFlow_MFC05_SET3		,	0	} ,
	{ "MFlow_MFC05_SET4"		, _K_A_IO	,	MFlow_MFC05_SET4		,	0	} ,
	{ "MFlow_MFC05_SET5"		, _K_A_IO	,	MFlow_MFC05_SET5		,	0	} ,
	
	{ "MFlow_MFC06_SET1"		, _K_A_IO	,	MFlow_MFC06_SET1		,	0	} ,     
	{ "MFlow_MFC06_SET2"		, _K_A_IO	,	MFlow_MFC06_SET2		,	0	} ,     
	{ "MFlow_MFC06_SET3"		, _K_A_IO	,	MFlow_MFC06_SET3		,	0	} ,     
	{ "MFlow_MFC06_SET4"		, _K_A_IO	,	MFlow_MFC06_SET4		,	0	} ,     
	{ "MFlow_MFC06_SET5"		, _K_A_IO	,	MFlow_MFC06_SET5		,	0	} ,	

	{ "MFlow_MFC07_SET1"		, _K_A_IO	,	MFlow_MFC07_SET1		,	0	} ,     
	{ "MFlow_MFC07_SET2"		, _K_A_IO	,	MFlow_MFC07_SET2		,	0	} ,     
	{ "MFlow_MFC07_SET3"		, _K_A_IO	,	MFlow_MFC07_SET3		,	0	} ,     
	{ "MFlow_MFC07_SET4"		, _K_A_IO	,	MFlow_MFC07_SET4		,	0	} ,     
	{ "MFlow_MFC07_SET5"		, _K_A_IO	,	MFlow_MFC07_SET5		,	0	} ,	

	{ "MFlow_MFC08_SET1"		, _K_A_IO	,	MFlow_MFC08_SET1		,	0	} ,
	{ "MFlow_MFC08_SET2"		, _K_A_IO	,	MFlow_MFC08_SET2		,	0	} ,
	{ "MFlow_MFC08_SET3"		, _K_A_IO	,	MFlow_MFC08_SET3		,	0	} ,
	{ "MFlow_MFC08_SET4"		, _K_A_IO	,	MFlow_MFC08_SET4		,	0	} ,
	{ "MFlow_MFC08_SET5"		, _K_A_IO	,	MFlow_MFC08_SET5		,	0	} ,

	{ "MFlow_MFC09_SET1"		, _K_A_IO	,	MFlow_MFC09_SET1		,	0	} ,
	{ "MFlow_MFC09_SET2"		, _K_A_IO	,	MFlow_MFC09_SET2		,	0	} ,
	{ "MFlow_MFC09_SET3"		, _K_A_IO	,	MFlow_MFC09_SET3		,	0	} ,
	{ "MFlow_MFC09_SET4"		, _K_A_IO	,	MFlow_MFC09_SET4		,	0	} ,
	{ "MFlow_MFC09_SET5"		, _K_A_IO	,	MFlow_MFC09_SET5		,	0	} ,

	{ "MFlow_MFC10_SET1"		, _K_A_IO	,	MFlow_MFC10_SET1		,	0	} ,
	{ "MFlow_MFC10_SET2"		, _K_A_IO	,	MFlow_MFC10_SET2		,	0	} ,
	{ "MFlow_MFC10_SET3"		, _K_A_IO	,	MFlow_MFC10_SET3		,	0	} ,
	{ "MFlow_MFC10_SET4"		, _K_A_IO	,	MFlow_MFC10_SET4		,	0	} ,
	{ "MFlow_MFC10_SET5"		, _K_A_IO	,	MFlow_MFC10_SET5		,	0	} ,

	{ "MFlow_MFC11_SET1"		, _K_A_IO	,	MFlow_MFC11_SET1		,	0	} ,
	{ "MFlow_MFC11_SET2"		, _K_A_IO	,	MFlow_MFC11_SET2		,	0	} ,
	{ "MFlow_MFC11_SET3"		, _K_A_IO	,	MFlow_MFC11_SET3		,	0	} ,
	{ "MFlow_MFC11_SET4"		, _K_A_IO	,	MFlow_MFC11_SET4		,	0	} ,
	{ "MFlow_MFC11_SET5"		, _K_A_IO	,	MFlow_MFC11_SET5		,	0	} ,

	{ "MFlow_MFC12_SET1"		, _K_A_IO	,	MFlow_MFC12_SET1		,	0	} ,
	{ "MFlow_MFC12_SET2"		, _K_A_IO	,	MFlow_MFC12_SET2		,	0	} ,
	{ "MFlow_MFC12_SET3"		, _K_A_IO	,	MFlow_MFC12_SET3		,	0	} ,
	{ "MFlow_MFC12_SET4"		, _K_A_IO	,	MFlow_MFC12_SET4		,	0	} ,
	{ "MFlow_MFC12_SET5"		, _K_A_IO	,	MFlow_MFC12_SET5		,	0	} ,
	
	{ "MFlow_MFC13_SET1"		, _K_A_IO	,	MFlow_MFC13_SET1		,	0	} ,
	{ "MFlow_MFC13_SET2"		, _K_A_IO	,	MFlow_MFC13_SET2		,	0	} ,
	{ "MFlow_MFC13_SET3"		, _K_A_IO	,	MFlow_MFC13_SET3		,	0	} ,
	{ "MFlow_MFC13_SET4"		, _K_A_IO	,	MFlow_MFC13_SET4		,	0	} ,
	{ "MFlow_MFC13_SET5"		, _K_A_IO	,	MFlow_MFC13_SET5		,	0	} ,
	
	{ "MFlow_MFC14_SET1"		, _K_A_IO	,	MFlow_MFC14_SET1		,	0	} ,
	{ "MFlow_MFC14_SET2"		, _K_A_IO	,	MFlow_MFC14_SET2		,	0	} ,
	{ "MFlow_MFC14_SET3"		, _K_A_IO	,	MFlow_MFC14_SET3		,	0	} ,
	{ "MFlow_MFC14_SET4"		, _K_A_IO	,	MFlow_MFC14_SET4		,	0	} ,
	{ "MFlow_MFC14_SET5"		, _K_A_IO	,	MFlow_MFC14_SET5		,	0	} ,

	{ "MFlow_MFC15_SET1"		, _K_A_IO	,	MFlow_MFC15_SET1		,	0	} ,
	{ "MFlow_MFC15_SET2"		, _K_A_IO	,	MFlow_MFC15_SET2		,	0	} ,
	{ "MFlow_MFC15_SET3"		, _K_A_IO	,	MFlow_MFC15_SET3		,	0	} ,
	{ "MFlow_MFC15_SET4"		, _K_A_IO	,	MFlow_MFC15_SET4		,	0	} ,
	{ "MFlow_MFC15_SET5"		, _K_A_IO	,	MFlow_MFC15_SET5		,	0	} ,

	{ "MFlow_MFC16_SET1"		, _K_A_IO	,	MFlow_MFC16_SET1		,	0	} ,
	{ "MFlow_MFC16_SET2"		, _K_A_IO	,	MFlow_MFC16_SET2		,	0	} ,
	{ "MFlow_MFC16_SET3"		, _K_A_IO	,	MFlow_MFC16_SET3		,	0	} ,
	{ "MFlow_MFC16_SET4"		, _K_A_IO	,	MFlow_MFC16_SET4		,	0	} ,
	{ "MFlow_MFC16_SET5"		, _K_A_IO	,	MFlow_MFC16_SET5		,	0	} ,
	
	{ "MFlow_MFC17_SET1"		, _K_A_IO	,	MFlow_MFC17_SET1		,	0	} ,
	{ "MFlow_MFC17_SET2"		, _K_A_IO	,	MFlow_MFC17_SET2		,	0	} ,
	{ "MFlow_MFC17_SET3"		, _K_A_IO	,	MFlow_MFC17_SET3		,	0	} ,
	{ "MFlow_MFC17_SET4"		, _K_A_IO	,	MFlow_MFC17_SET4		,	0	} ,
	{ "MFlow_MFC17_SET5"		, _K_A_IO	,	MFlow_MFC17_SET5		,	0	} ,

	{ "MFlow_MFC01_FLOW1"		, _K_A_IO	,	MFlow_MFC01_FLOW1		,	0	} ,
	{ "MFlow_MFC01_FLOW2"		, _K_A_IO	,	MFlow_MFC01_FLOW2		,	0	} ,
	{ "MFlow_MFC01_FLOW3"		, _K_A_IO	,	MFlow_MFC01_FLOW3		,	0	} ,
	{ "MFlow_MFC01_FLOW4"		, _K_A_IO	,	MFlow_MFC01_FLOW4		,	0	} ,
	{ "MFlow_MFC01_FLOW5"		, _K_A_IO	,	MFlow_MFC01_FLOW5		,	0	} ,

	{ "MFlow_MFC02_FLOW1"		, _K_A_IO	,	MFlow_MFC02_FLOW1		,	0	} , 
	{ "MFlow_MFC02_FLOW2"		, _K_A_IO	,	MFlow_MFC02_FLOW2		,	0	} , 
	{ "MFlow_MFC02_FLOW3"		, _K_A_IO	,	MFlow_MFC02_FLOW3		,	0	} , 
	{ "MFlow_MFC02_FLOW4"		, _K_A_IO	,	MFlow_MFC02_FLOW4		,	0	} , 
	{ "MFlow_MFC02_FLOW5"		, _K_A_IO	,	MFlow_MFC02_FLOW5		,	0	} , 

	{ "MFlow_MFC03_FLOW1"		, _K_A_IO	,	MFlow_MFC03_FLOW1		,	0	} ,  
	{ "MFlow_MFC03_FLOW2"		, _K_A_IO	,	MFlow_MFC03_FLOW2		,	0	} ,  
	{ "MFlow_MFC03_FLOW3"		, _K_A_IO	,	MFlow_MFC03_FLOW3		,	0	} ,  
	{ "MFlow_MFC03_FLOW4"		, _K_A_IO	,	MFlow_MFC03_FLOW4		,	0	} ,  
	{ "MFlow_MFC03_FLOW5"		, _K_A_IO	,	MFlow_MFC03_FLOW5		,	0	} ,  

	{ "MFlow_MFC04_FLOW1"		, _K_A_IO	,	MFlow_MFC04_FLOW1		,	0	} , 
	{ "MFlow_MFC04_FLOW2"		, _K_A_IO	,	MFlow_MFC04_FLOW2		,	0	} , 
	{ "MFlow_MFC04_FLOW3"		, _K_A_IO	,	MFlow_MFC04_FLOW3		,	0	} , 
	{ "MFlow_MFC04_FLOW4"		, _K_A_IO	,	MFlow_MFC04_FLOW4		,	0	} , 
	{ "MFlow_MFC04_FLOW5"		, _K_A_IO	,	MFlow_MFC04_FLOW5		,	0	} , 

	{ "MFlow_MFC05_FLOW1"		, _K_A_IO	,	MFlow_MFC05_FLOW1		,	0	} , 
	{ "MFlow_MFC05_FLOW2"		, _K_A_IO	,	MFlow_MFC05_FLOW2		,	0	} , 
	{ "MFlow_MFC05_FLOW3"		, _K_A_IO	,	MFlow_MFC05_FLOW3		,	0	} , 
	{ "MFlow_MFC05_FLOW4"		, _K_A_IO	,	MFlow_MFC05_FLOW4		,	0	} , 
	{ "MFlow_MFC05_FLOW5"		, _K_A_IO	,	MFlow_MFC05_FLOW5		,	0	} , 

	{ "MFlow_MFC06_FLOW1"		, _K_A_IO	,	MFlow_MFC06_FLOW1		,	0	} ,  
	{ "MFlow_MFC06_FLOW2"		, _K_A_IO	,	MFlow_MFC06_FLOW2		,	0	} ,  
	{ "MFlow_MFC06_FLOW3"		, _K_A_IO	,	MFlow_MFC06_FLOW3		,	0	} ,  
	{ "MFlow_MFC06_FLOW4"		, _K_A_IO	,	MFlow_MFC06_FLOW4		,	0	} ,  
	{ "MFlow_MFC06_FLOW5"		, _K_A_IO	,	MFlow_MFC06_FLOW5		,	0	} ,  

	{ "MFlow_MFC07_FLOW1"		, _K_A_IO	,	MFlow_MFC07_FLOW1		,	0	} ,
	{ "MFlow_MFC07_FLOW2"		, _K_A_IO	,	MFlow_MFC07_FLOW2		,	0	} ,
	{ "MFlow_MFC07_FLOW3"		, _K_A_IO	,	MFlow_MFC07_FLOW3		,	0	} ,
	{ "MFlow_MFC07_FLOW4"		, _K_A_IO	,	MFlow_MFC07_FLOW4		,	0	} ,
	{ "MFlow_MFC07_FLOW5"		, _K_A_IO	,	MFlow_MFC07_FLOW5		,	0	} ,

	{ "MFlow_MFC08_FLOW1"		, _K_A_IO	,	MFlow_MFC08_FLOW1		,	0	} ,  
	{ "MFlow_MFC08_FLOW2"		, _K_A_IO	,	MFlow_MFC08_FLOW2		,	0	} ,  
	{ "MFlow_MFC08_FLOW3"		, _K_A_IO	,	MFlow_MFC08_FLOW3		,	0	} ,  
	{ "MFlow_MFC08_FLOW4"		, _K_A_IO	,	MFlow_MFC08_FLOW4		,	0	} ,  
	{ "MFlow_MFC08_FLOW5"		, _K_A_IO	,	MFlow_MFC08_FLOW5		,	0	} , 
	
	{ "MFlow_MFC09_FLOW1"		, _K_A_IO	,	MFlow_MFC09_FLOW1		,	0	} ,  
	{ "MFlow_MFC09_FLOW2"		, _K_A_IO	,	MFlow_MFC09_FLOW2		,	0	} ,  
	{ "MFlow_MFC09_FLOW3"		, _K_A_IO	,	MFlow_MFC09_FLOW3		,	0	} ,  
	{ "MFlow_MFC09_FLOW4"		, _K_A_IO	,	MFlow_MFC09_FLOW4		,	0	} ,  
	{ "MFlow_MFC09_FLOW5"		, _K_A_IO	,	MFlow_MFC09_FLOW5		,	0	} , 

	{ "MFlow_MFC10_FLOW1"		, _K_A_IO	,	MFlow_MFC10_FLOW1		,	0	} ,  
	{ "MFlow_MFC10_FLOW2"		, _K_A_IO	,	MFlow_MFC10_FLOW2		,	0	} ,  
	{ "MFlow_MFC10_FLOW3"		, _K_A_IO	,	MFlow_MFC10_FLOW3		,	0	} ,  
	{ "MFlow_MFC10_FLOW4"		, _K_A_IO	,	MFlow_MFC10_FLOW4		,	0	} ,  
	{ "MFlow_MFC10_FLOW5"		, _K_A_IO	,	MFlow_MFC10_FLOW5		,	0	} , 

	{ "MFlow_MFC11_FLOW1"		, _K_A_IO	,	MFlow_MFC11_FLOW1		,	0	} ,  
	{ "MFlow_MFC11_FLOW2"		, _K_A_IO	,	MFlow_MFC11_FLOW2		,	0	} ,  
	{ "MFlow_MFC11_FLOW3"		, _K_A_IO	,	MFlow_MFC11_FLOW3		,	0	} ,  
	{ "MFlow_MFC11_FLOW4"		, _K_A_IO	,	MFlow_MFC11_FLOW4		,	0	} ,  
	{ "MFlow_MFC11_FLOW5"		, _K_A_IO	,	MFlow_MFC11_FLOW5		,	0	} , 

	{ "MFlow_MFC12_FLOW1"		, _K_A_IO	,	MFlow_MFC12_FLOW1		,	0	} ,  
	{ "MFlow_MFC12_FLOW2"		, _K_A_IO	,	MFlow_MFC12_FLOW2		,	0	} ,  
	{ "MFlow_MFC12_FLOW3"		, _K_A_IO	,	MFlow_MFC12_FLOW3		,	0	} ,  
	{ "MFlow_MFC12_FLOW4"		, _K_A_IO	,	MFlow_MFC12_FLOW4		,	0	} ,  
	{ "MFlow_MFC12_FLOW5"		, _K_A_IO	,	MFlow_MFC12_FLOW5		,	0	} , 

	{ "MFlow_MFC13_FLOW1"		, _K_A_IO	,	MFlow_MFC13_FLOW1		,	0	} ,  
	{ "MFlow_MFC13_FLOW2"		, _K_A_IO	,	MFlow_MFC13_FLOW2		,	0	} ,  
	{ "MFlow_MFC13_FLOW3"		, _K_A_IO	,	MFlow_MFC13_FLOW3		,	0	} ,  
	{ "MFlow_MFC13_FLOW4"		, _K_A_IO	,	MFlow_MFC13_FLOW4		,	0	} ,  
	{ "MFlow_MFC13_FLOW5"		, _K_A_IO	,	MFlow_MFC13_FLOW5		,	0	} , 

	{ "MFlow_MFC14_FLOW1"		, _K_A_IO	,	MFlow_MFC14_FLOW1		,	0	} ,		
	{ "MFlow_MFC14_FLOW2"		, _K_A_IO	,	MFlow_MFC14_FLOW2		,	0	} ,  
	{ "MFlow_MFC14_FLOW3"		, _K_A_IO	,	MFlow_MFC14_FLOW3		,	0	} ,  
	{ "MFlow_MFC14_FLOW4"		, _K_A_IO	,	MFlow_MFC14_FLOW4		,	0	} ,  
	{ "MFlow_MFC14_FLOW5"		, _K_A_IO	,	MFlow_MFC14_FLOW5		,	0	} , 

	{ "MFlow_MFC15_FLOW1"		, _K_A_IO	,	MFlow_MFC15_FLOW1		,	0	} ,  
	{ "MFlow_MFC15_FLOW2"		, _K_A_IO	,	MFlow_MFC15_FLOW2		,	0	} ,  
	{ "MFlow_MFC15_FLOW3"		, _K_A_IO	,	MFlow_MFC15_FLOW3		,	0	} ,  
	{ "MFlow_MFC15_FLOW4"		, _K_A_IO	,	MFlow_MFC15_FLOW4		,	0	} ,  
	{ "MFlow_MFC15_FLOW5"		, _K_A_IO	,	MFlow_MFC15_FLOW5		,	0	} , 

	{ "MFlow_MFC16_FLOW1"		, _K_A_IO	,	MFlow_MFC16_FLOW1		,	0	} ,  
	{ "MFlow_MFC16_FLOW2"		, _K_A_IO	,	MFlow_MFC16_FLOW2		,	0	} ,  
	{ "MFlow_MFC16_FLOW3"		, _K_A_IO	,	MFlow_MFC16_FLOW3		,	0	} ,  
	{ "MFlow_MFC16_FLOW4"		, _K_A_IO	,	MFlow_MFC16_FLOW4		,	0	} ,  
	{ "MFlow_MFC16_FLOW5"		, _K_A_IO	,	MFlow_MFC16_FLOW5		,	0	} , 

	{ "MFlow_MFC17_FLOW1"		, _K_A_IO	,	MFlow_MFC17_FLOW1		,	0	} ,  
	{ "MFlow_MFC17_FLOW2"		, _K_A_IO	,	MFlow_MFC17_FLOW2		,	0	} ,  
	{ "MFlow_MFC17_FLOW3"		, _K_A_IO	,	MFlow_MFC17_FLOW3		,	0	} ,  
	{ "MFlow_MFC17_FLOW4"		, _K_A_IO	,	MFlow_MFC17_FLOW4		,	0	} ,  
	{ "MFlow_MFC17_FLOW5"		, _K_A_IO	,	MFlow_MFC17_FLOW5		,	0	} , 

	{ "MFlow_MFC01_ResultPrs1"	, _K_A_IO	,	MFlow_MFC01_ResultPrs1	,	0	} , 
	{ "MFlow_MFC01_ResultPrs2"	, _K_A_IO	,	MFlow_MFC01_ResultPrs2	,	0	} , 
	{ "MFlow_MFC01_ResultPrs3"	, _K_A_IO	,	MFlow_MFC01_ResultPrs3	,	0	} , 
	{ "MFlow_MFC01_ResultPrs4"	, _K_A_IO	,	MFlow_MFC01_ResultPrs4	,	0	} , 
	{ "MFlow_MFC01_ResultPrs5"	, _K_A_IO	,	MFlow_MFC01_ResultPrs5	,	0	} , 

	{ "MFlow_MFC02_ResultPrs1"	, _K_A_IO	,	MFlow_MFC02_ResultPrs1	,	0	} , 
	{ "MFlow_MFC02_ResultPrs2"	, _K_A_IO	,	MFlow_MFC02_ResultPrs2	,	0	} , 
	{ "MFlow_MFC02_ResultPrs3"	, _K_A_IO	,	MFlow_MFC02_ResultPrs3	,	0	} , 
	{ "MFlow_MFC02_ResultPrs4"	, _K_A_IO	,	MFlow_MFC02_ResultPrs4	,	0	} , 
	{ "MFlow_MFC02_ResultPrs5"	, _K_A_IO	,	MFlow_MFC02_ResultPrs5	,	0	} , 

	{ "MFlow_MFC03_ResultPrs1"	, _K_A_IO	,	MFlow_MFC03_ResultPrs1	,	0	} ,
	{ "MFlow_MFC03_ResultPrs2"	, _K_A_IO	,	MFlow_MFC03_ResultPrs2	,	0	} ,
	{ "MFlow_MFC03_ResultPrs3"	, _K_A_IO	,	MFlow_MFC03_ResultPrs3	,	0	} ,
	{ "MFlow_MFC03_ResultPrs4"	, _K_A_IO	,	MFlow_MFC03_ResultPrs4	,	0	} ,
	{ "MFlow_MFC03_ResultPrs5"	, _K_A_IO	,	MFlow_MFC03_ResultPrs5	,	0	} ,

	{ "MFlow_MFC04_ResultPrs1"	, _K_A_IO	,	MFlow_MFC04_ResultPrs1	,	0	} ,    
	{ "MFlow_MFC04_ResultPrs2"	, _K_A_IO	,	MFlow_MFC04_ResultPrs2	,	0	} ,    
	{ "MFlow_MFC04_ResultPrs3"	, _K_A_IO	,	MFlow_MFC04_ResultPrs3	,	0	} ,    
	{ "MFlow_MFC04_ResultPrs4"	, _K_A_IO	,	MFlow_MFC04_ResultPrs4	,	0	} ,    
	{ "MFlow_MFC04_ResultPrs5"	, _K_A_IO	,	MFlow_MFC04_ResultPrs5	,	0	} ,    

	{ "MFlow_MFC05_ResultPrs1"	, _K_A_IO	,	MFlow_MFC05_ResultPrs1	,	0	} , 
	{ "MFlow_MFC05_ResultPrs2"	, _K_A_IO	,	MFlow_MFC05_ResultPrs2	,	0	} , 
	{ "MFlow_MFC05_ResultPrs3"	, _K_A_IO	,	MFlow_MFC05_ResultPrs3	,	0	} , 
	{ "MFlow_MFC05_ResultPrs4"	, _K_A_IO	,	MFlow_MFC05_ResultPrs4	,	0	} , 
	{ "MFlow_MFC05_ResultPrs5"	, _K_A_IO	,	MFlow_MFC05_ResultPrs5	,	0	} , 

	{ "MFlow_MFC06_ResultPrs1"	, _K_A_IO	,	MFlow_MFC06_ResultPrs1	,	0	} ,  
	{ "MFlow_MFC06_ResultPrs2"	, _K_A_IO	,	MFlow_MFC06_ResultPrs2	,	0	} ,  
	{ "MFlow_MFC06_ResultPrs3"	, _K_A_IO	,	MFlow_MFC06_ResultPrs3	,	0	} ,  
	{ "MFlow_MFC06_ResultPrs4"	, _K_A_IO	,	MFlow_MFC06_ResultPrs4	,	0	} ,  
	{ "MFlow_MFC06_ResultPrs5"	, _K_A_IO	,	MFlow_MFC06_ResultPrs5	,	0	} ,  

	{ "MFlow_MFC07_ResultPrs1"	, _K_A_IO	,	MFlow_MFC07_ResultPrs1	,	0	} , 
	{ "MFlow_MFC07_ResultPrs2"	, _K_A_IO	,	MFlow_MFC07_ResultPrs2	,	0	} , 
	{ "MFlow_MFC07_ResultPrs3"	, _K_A_IO	,	MFlow_MFC07_ResultPrs3	,	0	} , 
	{ "MFlow_MFC07_ResultPrs4"	, _K_A_IO	,	MFlow_MFC07_ResultPrs4	,	0	} , 
	{ "MFlow_MFC07_ResultPrs5"	, _K_A_IO	,	MFlow_MFC07_ResultPrs5	,	0	} , 

	{ "MFlow_MFC08_ResultPrs1"	, _K_A_IO	,	MFlow_MFC08_ResultPrs1	,	0	} ,
	{ "MFlow_MFC08_ResultPrs2"	, _K_A_IO	,	MFlow_MFC08_ResultPrs2	,	0	} ,
	{ "MFlow_MFC08_ResultPrs3"	, _K_A_IO	,	MFlow_MFC08_ResultPrs3	,	0	} ,
	{ "MFlow_MFC08_ResultPrs4"	, _K_A_IO	,	MFlow_MFC08_ResultPrs4	,	0	} ,
	{ "MFlow_MFC08_ResultPrs5"	, _K_A_IO	,	MFlow_MFC08_ResultPrs5	,	0	} ,
	
	{ "MFlow_MFC09_ResultPrs1"	, _K_A_IO	,	MFlow_MFC09_ResultPrs1	,	0	} ,
	{ "MFlow_MFC09_ResultPrs2"	, _K_A_IO	,	MFlow_MFC09_ResultPrs2	,	0	} ,
	{ "MFlow_MFC09_ResultPrs3"	, _K_A_IO	,	MFlow_MFC09_ResultPrs3	,	0	} ,
	{ "MFlow_MFC09_ResultPrs4"	, _K_A_IO	,	MFlow_MFC09_ResultPrs4	,	0	} ,
	{ "MFlow_MFC09_ResultPrs5"	, _K_A_IO	,	MFlow_MFC09_ResultPrs5	,	0	} ,
	
	{ "MFlow_MFC10_ResultPrs1"	, _K_A_IO	,	MFlow_MFC10_ResultPrs1	,	0	} ,
	{ "MFlow_MFC10_ResultPrs2"	, _K_A_IO	,	MFlow_MFC10_ResultPrs2	,	0	} ,
	{ "MFlow_MFC10_ResultPrs3"	, _K_A_IO	,	MFlow_MFC10_ResultPrs3	,	0	} ,
	{ "MFlow_MFC10_ResultPrs4"	, _K_A_IO	,	MFlow_MFC10_ResultPrs4	,	0	} ,
	{ "MFlow_MFC10_ResultPrs5"	, _K_A_IO	,	MFlow_MFC10_ResultPrs5	,	0	} ,

	{ "MFlow_MFC11_ResultPrs1"	, _K_A_IO	,	MFlow_MFC11_ResultPrs1	,	0	} ,
	{ "MFlow_MFC11_ResultPrs2"	, _K_A_IO	,	MFlow_MFC11_ResultPrs2	,	0	} ,
	{ "MFlow_MFC11_ResultPrs3"	, _K_A_IO	,	MFlow_MFC11_ResultPrs3	,	0	} ,
	{ "MFlow_MFC11_ResultPrs4"	, _K_A_IO	,	MFlow_MFC11_ResultPrs4	,	0	} ,
	{ "MFlow_MFC11_ResultPrs5"	, _K_A_IO	,	MFlow_MFC11_ResultPrs5	,	0	} ,

	{ "MFlow_MFC12_ResultPrs1"	, _K_A_IO	,	MFlow_MFC12_ResultPrs1	,	0	} ,
	{ "MFlow_MFC12_ResultPrs2"	, _K_A_IO	,	MFlow_MFC12_ResultPrs2	,	0	} ,
	{ "MFlow_MFC12_ResultPrs3"	, _K_A_IO	,	MFlow_MFC12_ResultPrs3	,	0	} ,
	{ "MFlow_MFC12_ResultPrs4"	, _K_A_IO	,	MFlow_MFC12_ResultPrs4	,	0	} ,
	{ "MFlow_MFC12_ResultPrs5"	, _K_A_IO	,	MFlow_MFC12_ResultPrs5	,	0	} ,

	{ "MFlow_MFC13_ResultPrs1"	, _K_A_IO	,	MFlow_MFC13_ResultPrs1	,	0	} ,
	{ "MFlow_MFC13_ResultPrs2"	, _K_A_IO	,	MFlow_MFC13_ResultPrs2	,	0	} ,
	{ "MFlow_MFC13_ResultPrs3"	, _K_A_IO	,	MFlow_MFC13_ResultPrs3	,	0	} ,
	{ "MFlow_MFC13_ResultPrs4"	, _K_A_IO	,	MFlow_MFC13_ResultPrs4	,	0	} ,
	{ "MFlow_MFC13_ResultPrs5"	, _K_A_IO	,	MFlow_MFC13_ResultPrs5	,	0	} ,

	{ "MFlow_MFC14_ResultPrs1"	, _K_A_IO	,	MFlow_MFC14_ResultPrs1	,	0	} ,
	{ "MFlow_MFC14_ResultPrs2"	, _K_A_IO	,	MFlow_MFC14_ResultPrs2	,	0	} ,
	{ "MFlow_MFC14_ResultPrs3"	, _K_A_IO	,	MFlow_MFC14_ResultPrs3	,	0	} ,
	{ "MFlow_MFC14_ResultPrs4"	, _K_A_IO	,	MFlow_MFC14_ResultPrs4	,	0	} ,
	{ "MFlow_MFC14_ResultPrs5"	, _K_A_IO	,	MFlow_MFC14_ResultPrs5	,	0	} ,

	{ "MFlow_MFC15_ResultPrs1"	, _K_A_IO	,	MFlow_MFC15_ResultPrs1	,	0	} ,
	{ "MFlow_MFC15_ResultPrs2"	, _K_A_IO	,	MFlow_MFC15_ResultPrs2	,	0	} ,
	{ "MFlow_MFC15_ResultPrs3"	, _K_A_IO	,	MFlow_MFC15_ResultPrs3	,	0	} ,
	{ "MFlow_MFC15_ResultPrs4"	, _K_A_IO	,	MFlow_MFC15_ResultPrs4	,	0	} ,
	{ "MFlow_MFC15_ResultPrs5"	, _K_A_IO	,	MFlow_MFC15_ResultPrs5	,	0	} ,

	{ "MFlow_MFC16_ResultPrs1"	, _K_A_IO	,	MFlow_MFC16_ResultPrs1	,	0	} ,
	{ "MFlow_MFC16_ResultPrs2"	, _K_A_IO	,	MFlow_MFC16_ResultPrs2	,	0	} ,
	{ "MFlow_MFC16_ResultPrs3"	, _K_A_IO	,	MFlow_MFC16_ResultPrs3	,	0	} ,
	{ "MFlow_MFC16_ResultPrs4"	, _K_A_IO	,	MFlow_MFC16_ResultPrs4	,	0	} ,
	{ "MFlow_MFC16_ResultPrs5"	, _K_A_IO	,	MFlow_MFC16_ResultPrs5	,	0	} ,

	{ "MFlow_MFC17_ResultPrs1"	, _K_A_IO	,	MFlow_MFC17_ResultPrs1	,	0	} ,
	{ "MFlow_MFC17_ResultPrs2"	, _K_A_IO	,	MFlow_MFC17_ResultPrs2	,	0	} ,
	{ "MFlow_MFC17_ResultPrs3"	, _K_A_IO	,	MFlow_MFC17_ResultPrs3	,	0	} ,
	{ "MFlow_MFC17_ResultPrs4"	, _K_A_IO	,	MFlow_MFC17_ResultPrs4	,	0	} ,
	{ "MFlow_MFC17_ResultPrs5"	, _K_A_IO	,	MFlow_MFC17_ResultPrs5	,	0	} ,

	{ "ABORT_GAS"				, _K_F_IO	, 	ABORT_GAS			,	0	} ,
	{ "VACUUM_CONTROL"			, _K_F_IO	, 	VACUUM_CONTROL		,	0	} ,

	//... 2015.02.21 Add MFC Flow Hunting Count
	{ "MFlow_MFC01_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC01_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC01_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC01_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC01_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC01_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC01_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC01_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC01_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC01_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC02_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC02_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC02_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC02_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC02_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC02_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC02_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC02_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC02_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC02_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC03_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC03_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC03_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC03_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC03_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC03_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC03_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC03_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC03_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC03_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC04_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC04_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC04_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC04_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC04_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC04_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC04_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC04_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC04_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC04_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC05_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC05_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC05_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC05_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC05_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC05_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC05_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC05_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC05_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC05_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC06_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC06_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC06_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC06_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC06_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC06_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC06_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC06_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC06_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC06_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC07_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC07_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC07_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC07_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC07_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC07_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC07_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC07_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC07_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC07_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC08_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC08_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC08_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC08_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC08_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC08_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC08_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC08_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC08_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC08_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC09_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC09_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC09_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC09_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC09_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC09_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC09_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC09_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC09_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC09_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC10_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC10_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC10_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC10_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC10_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC10_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC10_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC10_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC10_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC10_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC11_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC11_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC11_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC11_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC11_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC11_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC11_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC11_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC11_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC11_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC12_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC12_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC12_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC12_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC12_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC12_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC12_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC12_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC12_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC12_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC13_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC13_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC13_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC13_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC13_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC13_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC13_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC13_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC13_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC13_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC14_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC14_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC14_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC14_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC14_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC14_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC14_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC14_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC14_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC14_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC15_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC15_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC15_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC15_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC15_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC15_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC15_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC15_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC15_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC15_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC16_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC16_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC16_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC16_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC16_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC16_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC16_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC16_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC16_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC16_FlwErr5DM	,	0	} ,

	{ "MFlow_MFC17_FlwErr1DM"	, _K_D_IO	,	MFlow_MFC17_FlwErr1DM	,	0	} ,
	{ "MFlow_MFC17_FlwErr2DM"	, _K_D_IO	,	MFlow_MFC17_FlwErr2DM	,	0	} ,
	{ "MFlow_MFC17_FlwErr3DM"	, _K_D_IO	,	MFlow_MFC17_FlwErr3DM	,	0	} ,
	{ "MFlow_MFC17_FlwErr4DM"	, _K_D_IO	,	MFlow_MFC17_FlwErr4DM	,	0	} ,
	{ "MFlow_MFC17_FlwErr5DM"	, _K_D_IO	,	MFlow_MFC17_FlwErr5DM	,	0	} ,

	//... 2015.02.21 Add Parameter Save Flag for GUI Parameter
	{ "MFlow_MFC_ParmSaveDM"	, _K_D_IO	,	MFlow_MFC_ParmSaveDM	,	0	} ,
	//... 2018.06.24
	{ "MANUAL_SET_MFC01"		, _K_A_IO	,	MANUAL_SET_MFC01		,	0	} ,
	{ "MANUAL_SET_MFC02"		, _K_A_IO	,	MANUAL_SET_MFC02		,	0	} ,
	{ "MANUAL_SET_MFC03"		, _K_A_IO	,	MANUAL_SET_MFC03		,	0	} ,
	{ "MANUAL_SET_MFC04"		, _K_A_IO	,	MANUAL_SET_MFC04		,	0	} ,
	{ "MANUAL_SET_MFC05"		, _K_A_IO	,	MANUAL_SET_MFC05		,	0	} ,
	{ "MANUAL_SET_MFC06"		, _K_A_IO	,	MANUAL_SET_MFC06		,	0	} ,
	{ "MANUAL_SET_MFC07"		, _K_A_IO	,	MANUAL_SET_MFC07		,	0	} ,
	{ "MANUAL_SET_MFC08"		, _K_A_IO	,	MANUAL_SET_MFC08		,	0	} ,
	{ "MANUAL_SET_MFC09"		, _K_A_IO	,	MANUAL_SET_MFC09		,	0	} ,
	{ "MANUAL_SET_MFC_SEQ"		, _K_D_IO	,	MANUAL_SET_MFC_SEQ		,	0	} ,
	""
};

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	return g_SeqInterface.Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	g_SeqInterface.Enter_Code();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	g_SeqInterface.m_bSimulMode = TRUE;
	else														g_SeqInterface.m_bSimulMode = FALSE;
	return g_SeqInterface.CSeqInterface::Initialize(FUNC_NAME);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		g_SeqInterface.Deinitialize();
	}
}

} // End of [extern "C"]
//==========================================================================================
