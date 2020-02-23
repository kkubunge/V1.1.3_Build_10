#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <string.h>

//Version History//////////////////////////////////////////////////////////////////////////
//Date : 2008.03.18
//       Modified from Wuxi Bluetain
//       BM Slot Control --> BM1/BM2 Slot Control Change
//       Wafer Slide IO Station별로 6개씩으로 증가됨
//       PM Slot State 기존 IO Station별로 2개 --> 1개로 변경 



//==========================================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		ALARM_ISOGATE_COMM_ERR		6001
#define		ALARM_ISO_CLOSE_TMO			6002
#define		ALARM_ISO_OPEN_TMO			6003
#define		ALARM_ISO_CLOSE_RB_RET		6004
#define     ALARM_ISO_CLOSE_WAFER_SLIDE 6005

#define     ALARM_ISO_OPEN_NOT_ALL_CLOSE	6006

#define     ALARM_BM1ISO_CLOSE_L1_WAFER_SLIDE 6010
#define     ALARM_BM1ISO_CLOSE_L2_WAFER_SLIDE 6011
#define     ALARM_BM1ISO_CLOSE_L3_WAFER_SLIDE 6012
#define     ALARM_BM1ISO_CLOSE_R1_WAFER_SLIDE 6013
#define     ALARM_BM1ISO_CLOSE_R2_WAFER_SLIDE 6014
#define     ALARM_BM1ISO_CLOSE_R3_WAFER_SLIDE 6015

#define     ALARM_BM2ISO_CLOSE_L1_WAFER_SLIDE 6020
#define     ALARM_BM2ISO_CLOSE_L2_WAFER_SLIDE 6021
#define     ALARM_BM2ISO_CLOSE_L3_WAFER_SLIDE 6022
#define     ALARM_BM2ISO_CLOSE_R1_WAFER_SLIDE 6023
#define     ALARM_BM2ISO_CLOSE_R2_WAFER_SLIDE 6024
#define     ALARM_BM2ISO_CLOSE_R3_WAFER_SLIDE 6025

#define     ALARM_PM1ISO_CLOSE_L1_WAFER_SLIDE 6030
#define     ALARM_PM1ISO_CLOSE_L2_WAFER_SLIDE 6031
#define     ALARM_PM1ISO_CLOSE_L3_WAFER_SLIDE 6032
#define     ALARM_PM1ISO_CLOSE_R1_WAFER_SLIDE 6033
#define     ALARM_PM1ISO_CLOSE_R2_WAFER_SLIDE 6034
#define     ALARM_PM1ISO_CLOSE_R3_WAFER_SLIDE 6035

#define     ALARM_PM2ISO_CLOSE_L1_WAFER_SLIDE 6040 
#define     ALARM_PM2ISO_CLOSE_L2_WAFER_SLIDE 6041 
#define     ALARM_PM2ISO_CLOSE_L3_WAFER_SLIDE 6042 
#define     ALARM_PM2ISO_CLOSE_R1_WAFER_SLIDE 6043 
#define     ALARM_PM2ISO_CLOSE_R2_WAFER_SLIDE 6044 
#define     ALARM_PM2ISO_CLOSE_R3_WAFER_SLIDE 6045 

#define     ALARM_PM3ISO_CLOSE_L1_WAFER_SLIDE 6050  
#define     ALARM_PM3ISO_CLOSE_L2_WAFER_SLIDE 6051  
#define     ALARM_PM3ISO_CLOSE_L3_WAFER_SLIDE 6052  
#define     ALARM_PM3ISO_CLOSE_R1_WAFER_SLIDE 6053  
#define     ALARM_PM3ISO_CLOSE_R2_WAFER_SLIDE 6054  
#define     ALARM_PM3ISO_CLOSE_R3_WAFER_SLIDE 6055  

//... 2015.09.28 Add Slot Valve OPEN Pressure Check & Swap Purge
#define		ALARM_OPEN_PM_SWAP_PURGE_FAILED		6060
#define		ALARM_OPEN_PRESS_RANGE_OVER			6061
#define		ALARM_OPEN_PRESS_DIFF_OVER			6062
//... 2016.02.10
#define		ALARM_SLIT_CYCLE_TEST_FAIL			6070

#define		MAX_RETRY_COUNT			5
//... 2016.02.10
#define		MAX_SLIT_VALVE			5
//------------------------------------------------------------------------------------------
enum {BM1 , BM2 , PM1 , PM2 , PM3 , PM4 , PM5 , PM6 };
enum {None , ATM , VAC , Err };
enum {CLOSE, OPEN };
enum {ROUGH_CLOSE, ROUGH_OPENSLOW, ROUGH_OPENFAST };
enum {SLIDE = 0 , NORMAL = 1};

enum { enCTRLOPT_ON, enCTRLOPT_OFF};

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

int g_nStation;
//2014.03.10 MAHA UlVAC Robot Retract Signal
enum { NONE_0	, EXTEND_1 , RETRACT_2 , ERROR_3 } ; // TM Robot Arm Status.
//... 2016.02.10
enum { eIDLE,	eABORT,	 eABORTING } ;
enum { eSTOP,	eRUNNING } ; 
enum { eNORMAL,	eINTERLOCK } ; 
enum { eOFF,	eON };
enum { eVS_ATM, eVS_VACUUM, eVS_GOATM, eVS_GOVAC, eVS_ABORT };

typedef enum
{ 
	LS_IDLE,
	LS_RUNNING,
	LS_PAUSING,
	LS_PAUSED,
	LS_ABORTING,
	LS_DISABLE,
	LS_WAITING,
	LS_WAIT_HANDOFF
}ELOTSTATUS;

//... 2016.02.10
enum { eUnuse, eLocal, eDisConn, eStandby, eMaint, eUninit, eEnablePM, eCTCinUSE, eCTCDis, eCTCDis2, eCTCEnaPM, eCTCDisHW, eCTCDIsHW2 };
//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE = FALSE;


#define	MAX_VTM_ROBOT_ARM		4

//... 2016.02.10
BOOL M_SVlv_CycTest	= FALSE;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	{ "FC_TM_ISO_CONTROL"		, _K_D_IO	,	CONTROL				,	0	} ,
	{ "ISO_CONTROL_OPTION"		, _K_D_IO	,	IsoControlType		,	0	} ,

	{ "TM.BM1.SlotVvLXO"		, _K_D_IO	,	BM1_SlotVvLXO		,	0	} ,
	{ "TM.BM1.SlotVvRXO"		, _K_D_IO	,	BM1_SlotVvRXO		,	0	} ,	
	{ "TM.BM2.SlotVvLXO"		, _K_D_IO	,	BM2_SlotVvLXO		,	0	} ,
	{ "TM.BM2.SlotVvRXO"		, _K_D_IO	,	BM2_SlotVvRXO		,	0	} ,	
	{ "TM.PM1.SlotVv1XO"	    , _K_D_IO	,	PM1_SlotValve1XO	,	0	} ,
	{ "TM.PM1.SlotVv2XO"	    , _K_D_IO	,	PM1_SlotValve2XO	,	0	} ,
	{ "TM.PM2.SlotVv1XO"	    , _K_D_IO	,	PM2_SlotValve1XO	,	0	} ,
	{ "TM.PM2.SlotVv2XO"	    , _K_D_IO	,	PM2_SlotValve2XO	,	0	} ,
	{ "TM.PM3.SlotVv1XO"	    , _K_D_IO	,	PM3_SlotValve1XO	,	0	} ,
	{ "TM.PM3.SlotVv2XO"	    , _K_D_IO	,	PM3_SlotValve2XO	,	0	} ,

	{ "TM.BM1.SlotVvLXI"		, _K_D_IO	,	BM1_SlotVvLXI		,	0	} ,
	{ "TM.BM1.SlotVvRXI"		, _K_D_IO	,	BM1_SlotVvRXI		,	0	} ,	
	{ "TM.BM2.SlotVvLXI"		, _K_D_IO	,	BM2_SlotVvLXI		,	0	} ,
	{ "TM.BM2.SlotVvRXI"		, _K_D_IO	,	BM2_SlotVvRXI		,	0	} ,	
	{ "TM.PM1.SlotVv1XI"	    , _K_D_IO	,	PM1_SlotValve1XI	,	0	} ,
	{ "TM.PM1.SlotVv2XI"	    , _K_D_IO	,	PM1_SlotValve2XI	,	0	} ,
	{ "TM.PM2.SlotVv1XI"	    , _K_D_IO	,	PM2_SlotValve1XI	,	0	} ,
	{ "TM.PM2.SlotVv2XI"	    , _K_D_IO	,	PM2_SlotValve2XI	,	0	} ,
	{ "TM.PM3.SlotVv1XI"	    , _K_D_IO	,	PM3_SlotValve1XI	,	0	} ,
	{ "TM.PM3.SlotVv2XI"	    , _K_D_IO	,	PM3_SlotValve2XI	,	0	} ,
 
// MAHA BB Modify 2014.03.10 by hyuk	
	{ "BM.WaferSlideL1DI"		, _K_D_IO	,	BM_WaferSlideL1DI	,	0	} ,	
	{ "BM.WaferSlideR1DI"		, _K_D_IO	,	BM_WaferSlideR1DI	,	0	} ,

	{ "TM.PM1.WaferSlide1DI"	, _K_D_IO	,	PM1_WaferSlide1DI	,	0	} ,
	{ "TM.PM1.WaferSlide2DI"	, _K_D_IO	,	PM1_WaferSlide2DI	,	0	} ,
	{ "TM.PM2.WaferSlide1DI"	, _K_D_IO	,	PM2_WaferSlide1DI	,	0	} ,
	{ "TM.PM2.WaferSlide2DI"	, _K_D_IO	,	PM2_WaferSlide2DI	,	0	} ,
	{ "TM.PM3.WaferSlide1DI"	, _K_D_IO	,	PM3_WaferSlide1DI	,	0	} ,
	{ "TM.PM3.WaferSlide2DI"	, _K_D_IO	,	PM3_WaferSlide2DI	,	0	} ,

	{ "TM.VacRbRet_ARMADI"		, _K_D_IO	,	TM_VacRbRet_ARMADI	,	0	} ,
	{ "TM.VacRbRet_ARMBDI"		, _K_D_IO	,	TM_VacRbRet_ARMBDI	,	0	} ,
	{ "TM.VacRbRet_ARMCDI"		, _K_D_IO	,	TM_VacRbRet_ARMCDI	,	0	} ,
	{ "TM.VacRbRet_ARMDDI"		, _K_D_IO	,	TM_VacRbRet_ARMDDI	,	0	} ,
// MAHA BB Modify 2014.03.10 by hyuk	

    { "TM.BM_Gate_UseNoUse"		, _K_D_IO	,	BM_Gate_UseNoUse	,	0	} ,
	{ "TM.PM1_Gate_UseNoUse"	, _K_D_IO	,	PM1_Gate_UseNoUse	,	0	} ,
	{ "TM.PM2_Gate_UseNoUse"	, _K_D_IO	,	PM2_Gate_UseNoUse	,	0	} ,
	{ "TM.PM3_Gate_UseNoUse"	, _K_D_IO	,	PM3_Gate_UseNoUse	,	0	} ,

	// Over Pressure IO .....
	{ "PRMD_OVER_PRES_OPT"		, _K_D_IO	,	PRMD_OVER_PRES_OPT	,	0	} ,	

	{ "TM.OverPresVvDO"			, _K_D_IO	,	TM_OverPresVvDO		,	0	} ,
	{ "TM.RoughVvXO"			, _K_D_IO	,	TM_RoughVvXO		,	0	} ,	
	{ "TM.ATMVACSensorXI"		, _K_D_IO	,	TM_ATMVACSensorXI	,	0	} ,	
	{ "TM.CGChmPrsAI"			, _K_A_IO	,	TM_CGChmPrsAI		,	0	} ,
	{ "BM1.OverPresVvDO"	   	, _K_D_IO	,	BM1_OverPresVvDO	,	0	} ,
	{ "BM1.RoughVvXO"			, _K_D_IO	,	BM1_RoughVvXO		,	0	} , //Close,OpenSlow,OpenFast
	{ "BM1.ATMVACSensorXI"		, _K_D_IO	,	BM1_ATMVACSensorXI	,	0	} ,
	{ "BM2.OverPresVvDO"	   	, _K_D_IO	,	BM2_OverPresVvDO	,	0	} ,
	{ "BM2.RoughVvXO"			, _K_D_IO	,	BM2_RoughVvXO		,	0	} , //Close,OpenSlow,OpenFast
	{ "BM2.ATMVACSensorXI"		, _K_D_IO	,	BM2_ATMVACSensorXI	,	0	} ,
	// Over Pressure IO .....

	{ "PRM_TM_ISO_OPCL_TIMEOUT"	, _K_D_IO	,	ISO_OPCL_TIMEOUT	,	0	} ,
	{ "PRM_TM_ISO_SENSOR_CHECK"	, _K_D_IO	,	SENSOR_CHECK		,	0	} ,

	{ "CTC.MAIN_CONTROL"		, _K_D_IO	,	LOT_CONTROL			,	0	} ,
	{ "CTC.MAIN_CONTROL2"		, _K_D_IO	,	LOT2_CONTROL		,	0	} ,
	{ "CTC.MAIN_CONTROL3"		, _K_D_IO	,	LOT3_CONTROL		,	0	} ,
	{ "CTC_EVENT_LOG_SVR"		, _K_F_IO	,	CTC_EVENT_LOG_SVR	,	0	} ,		// 2013.11.15

	//... 2015.09.28 Add Slot Valve OPEN Pressure Check & Swap Purge
	{ "FC_TM_SV_OPEN_CHECK"		, _K_D_IO	,	FC_TM_SV_OPEN_CHECK	,	0	} ,

	{ "PRMA_SV_LLOpPrsAtm"		, _K_A_IO	,	PRMA_SV_LLOpPrsAtm	,	0	} ,
	{ "PRMA_SV_TMOpPrsAtm"		, _K_A_IO	,	PRMA_SV_TMOpPrsAtm	,	0	} ,
	{ "PRMA_SV_PMOpPrsAtm"		, _K_A_IO	,	PRMA_SV_PMOpPrsAtm	,	0	} ,

	{ "PRMA_SV_LLOpPrsLow"		, _K_A_IO	,	PRMA_SV_LLOpPrsLow	,	0	} ,
	{ "PRMA_SV_TMOpPrsLow"		, _K_A_IO	,	PRMA_SV_TMOpPrsLow	,	0	} ,
	{ "PRMA_SV_PMOpPrsLow"		, _K_A_IO	,	PRMA_SV_PMOpPrsLow	,	0	} ,

	{ "PRMA_SV_LLOpPrsHigh"		, _K_A_IO	,	PRMA_SV_LLOpPrsHigh	,	0	} ,
	{ "PRMA_SV_TMOpPrsHigh"		, _K_A_IO	,	PRMA_SV_TMOpPrsHigh	,	0	} ,
	{ "PRMA_SV_PMOpPrsHigh"		, _K_A_IO	,	PRMA_SV_PMOpPrsHigh	,	0	} ,

	{ "PRMA_SV_LLOpDiffPrs"		, _K_A_IO	,	PRMA_SV_LLOpDiffPrs	,	0	} ,
	{ "PRMA_SV_PMOpDiffPrs"		, _K_A_IO	,	PRMA_SV_PMOpDiffPrs	,	0	} ,

	{ "PRMD_SV_LLPrsChkTMO"		, _K_D_IO	,	PRMD_SV_LLPrsChkTMO	,	0	} ,
	{ "PRMD_SV_TMPrsChkTMO"		, _K_D_IO	,	PRMD_SV_TMPrsChkTMO	,	0	} ,
	{ "PRMD_SV_PMPrsChkTMO"		, _K_D_IO	,	PRMD_SV_PMPrsChkTMO	,	0	} ,

	{ "PRMA_SV_LLOpDelay"		, _K_A_IO	,	PRMA_SV_LLOpDelay	,	0	} ,
	{ "PRMA_SV_PMOpDelay"		, _K_A_IO	,	PRMA_SV_PMOpDelay	,	0	} ,

	{ "PRMA_SV_LLOpDelay2"		, _K_A_IO	,	PRMA_SV_LLOpDelay2	,	0	} ,
	{ "PRMA_SV_PMOpDelay2"		, _K_A_IO	,	PRMA_SV_PMOpDelay2	,	0	} ,

	{ "PM1.ChmPrsAI"			, _K_A_IO	,	PM1_ChmPrsAI		,	0	} ,
	{ "PM2.ChmPrsAI"			, _K_A_IO	,	PM2_ChmPrsAI		,	0	} ,
	{ "PM3.ChmPrsAI"			, _K_A_IO	,	PM3_ChmPrsAI		,	0	} ,

	{ "PM1.M651_Pressure"		, _K_A_IO	,	PM1_M651_Pressure	,	0	} ,
	{ "PM2.M651_Pressure"		, _K_A_IO	,	PM2_M651_Pressure	,	0	} ,
	{ "PM3.M651_Pressure"		, _K_A_IO	,	PM3_M651_Pressure	,	0	} ,

	{ "BM1.CGChmPrsAI"			, _K_A_IO	,	BM1_CGChmPrsAI		,	0	} ,
	{ "BM2.CGChmPrsAI"			, _K_A_IO	,	BM2_CGChmPrsAI		,	0	} ,

	{ "PM1.TMC_PM_ABORT_GAS"	, _K_F_IO	,	PM1_ABORT_GAS		,	0	} ,
	{ "PM2.TMC_PM_ABORT_GAS"	, _K_F_IO	,	PM2_ABORT_GAS		,	0	} ,
	{ "PM3.TMC_PM_ABORT_GAS"	, _K_F_IO	,	PM3_ABORT_GAS		,	0	} ,
	//... 2016.02.10  Open Time
	{ "BM1_Slit_OPTm_AM"		, _K_A_IO	,	BM1_Slit_OPTm_AM	,	0	} ,
	{ "BM2_Slit_OPTm_AM"		, _K_A_IO	,	BM2_Slit_OPTm_AM	,	0	} ,
	{ "PM1_Slit_OPTm_AM"		, _K_A_IO	,	PM1_Slit_OPTm_AM	,	0	} ,
	{ "PM2_Slit_OPTm_AM"		, _K_A_IO	,	PM2_Slit_OPTm_AM	,	0	} ,
	{ "PM3_Slit_OPTm_AM"		, _K_A_IO	,	PM3_Slit_OPTm_AM	,	0	} ,
	//... Close Time
	{ "BM1_Slit_CLTm_AM"		, _K_A_IO	,	BM1_Slit_CLTm_AM	,	0	} ,
	{ "BM2_Slit_CLTm_AM"		, _K_A_IO	,	BM2_Slit_CLTm_AM	,	0	} ,
	{ "PM1_Slit_CLTm_AM"		, _K_A_IO	,	PM1_Slit_CLTm_AM	,	0	} ,
	{ "PM2_Slit_CLTm_AM"		, _K_A_IO	,	PM2_Slit_CLTm_AM	,	0	} ,
	{ "PM3_Slit_CLTm_AM"		, _K_A_IO	,	PM3_Slit_CLTm_AM	,	0	} ,

	{ "M_SVlv_ParmSaveDM"		, _K_D_IO	,	M_SVlv_ParmSaveDM	,	0	} ,
	{ "M_SVlv_CycTgtMdlDM"		, _K_D_IO	,	M_SVlv_CycTgtMdlDM	,	0	} ,

	{ "M_SVlv_CycTgtCntAM"		, _K_A_IO	,	M_SVlv_CycTgtCntAM	,	0	} ,
	{ "M_SVlv_CycCurCntAM"		, _K_A_IO	,	M_SVlv_CycCurCntAM	,	0	} ,
	{ "M_SVlvBM1_OP_MinTmAM"	, _K_A_IO	,	M_SVlvBM1_OP_MinTmAM,	0	} ,
	{ "M_SVlvBM2_OP_MinTmAM"	, _K_A_IO	,	M_SVlvBM2_OP_MinTmAM,	0	} ,
	{ "M_SVlvPM1_OP_MinTmAM"	, _K_A_IO	,	M_SVlvPM1_OP_MinTmAM,	0	} ,
	{ "M_SVlvPM2_OP_MinTmAM"	, _K_A_IO	,	M_SVlvPM2_OP_MinTmAM,	0	} ,
	{ "M_SVlvPM3_OP_MinTmAM"	, _K_A_IO	,	M_SVlvPM3_OP_MinTmAM,	0	} ,

	{ "M_SVlvBM1_OP_MaxTmAM"	, _K_A_IO	,	M_SVlvBM1_OP_MaxTmAM,	0	} ,
	{ "M_SVlvBM2_OP_MaxTmAM"	, _K_A_IO	,	M_SVlvBM2_OP_MaxTmAM,	0	} ,
	{ "M_SVlvPM1_OP_MaxTmAM"	, _K_A_IO	,	M_SVlvPM1_OP_MaxTmAM,	0	} ,
	{ "M_SVlvPM2_OP_MaxTmAM"	, _K_A_IO	,	M_SVlvPM2_OP_MaxTmAM,	0	} ,
	{ "M_SVlvPM3_OP_MaxTmAM"	, _K_A_IO	,	M_SVlvPM3_OP_MaxTmAM,	0	} ,

	{ "M_SVlvBM1_OP_AvgTmAM"	, _K_A_IO	,	M_SVlvBM1_OP_AvgTmAM,	0	} ,
	{ "M_SVlvBM2_OP_AvgTmAM"	, _K_A_IO	,	M_SVlvBM2_OP_AvgTmAM,	0	} ,
	{ "M_SVlvPM1_OP_AvgTmAM"	, _K_A_IO	,	M_SVlvPM1_OP_AvgTmAM,	0	} ,
	{ "M_SVlvPM2_OP_AvgTmAM"	, _K_A_IO	,	M_SVlvPM2_OP_AvgTmAM,	0	} ,
	{ "M_SVlvPM3_OP_AvgTmAM"	, _K_A_IO	,	M_SVlvPM3_OP_AvgTmAM,	0	} ,

	{ "M_SVlvBM1_CL_MinTmAM"	, _K_A_IO	,	M_SVlvBM1_CL_MinTmAM,	0	} ,
	{ "M_SVlvBM2_CL_MinTmAM"	, _K_A_IO	,	M_SVlvBM2_CL_MinTmAM,	0	} ,
	{ "M_SVlvPM1_CL_MinTmAM"	, _K_A_IO	,	M_SVlvPM1_CL_MinTmAM,	0	} ,
	{ "M_SVlvPM2_CL_MinTmAM"	, _K_A_IO	,	M_SVlvPM2_CL_MinTmAM,	0	} ,
	{ "M_SVlvPM3_CL_MinTmAM"	, _K_A_IO	,	M_SVlvPM3_CL_MinTmAM,	0	} ,

	{ "M_SVlvBM1_CL_MaxTmAM"	, _K_A_IO	,	M_SVlvBM1_CL_MaxTmAM,	0	} ,
	{ "M_SVlvBM2_CL_MaxTmAM"	, _K_A_IO	,	M_SVlvBM2_CL_MaxTmAM,	0	} ,
	{ "M_SVlvPM1_CL_MaxTmAM"	, _K_A_IO	,	M_SVlvPM1_CL_MaxTmAM,	0	} ,
	{ "M_SVlvPM2_CL_MaxTmAM"	, _K_A_IO	,	M_SVlvPM2_CL_MaxTmAM,	0	} ,
	{ "M_SVlvPM3_CL_MaxTmAM"	, _K_A_IO	,	M_SVlvPM3_CL_MaxTmAM,	0	} ,

	{ "M_SVlvBM1_CL_AvgTmAM"	, _K_A_IO	,	M_SVlvBM1_CL_AvgTmAM,	0	} ,
	{ "M_SVlvBM2_CL_AvgTmAM"	, _K_A_IO	,	M_SVlvBM2_CL_AvgTmAM,	0	} ,
	{ "M_SVlvPM1_CL_AvgTmAM"	, _K_A_IO	,	M_SVlvPM1_CL_AvgTmAM,	0	} ,
	{ "M_SVlvPM2_CL_AvgTmAM"	, _K_A_IO	,	M_SVlvPM2_CL_AvgTmAM,	0	} ,
	{ "M_SVlvPM3_CL_AvgTmAM"	, _K_A_IO	,	M_SVlvPM3_CL_AvgTmAM,	0	} ,

	{ "CTC.AbortControl"		, _K_D_IO	,	CTC_AbortControl	,	0	} ,
	{ "BM1.VacuumStatusDM"		, _K_D_IO	,	BM1_VacuumStatusDM	,	0	} ,
	{ "BM2.VacuumStatusDM"		, _K_D_IO	,	BM2_VacuumStatusDM	,	0	} ,
	{ "PM1.PRO_RunStsDM"		, _K_D_IO	,	PM1_PRO_RunStsDM	,	0	} ,
	{ "PM2.PRO_RunStsDM"		, _K_D_IO	,	PM2_PRO_RunStsDM	,	0	} ,
	{ "PM3.PRO_RunStsDM"		, _K_D_IO	,	PM3_PRO_RunStsDM	,	0	} ,
	{ "PM1.SW_GbIntlckDM"		, _K_D_IO	,	PM1_SW_GbIntlckDM	,	0	} ,
	{ "PM2.SW_GbIntlckDM"		, _K_D_IO	,	PM2_SW_GbIntlckDM	,	0	} ,
	{ "PM3.SW_GbIntlckDM"		, _K_D_IO	,	PM3_SW_GbIntlckDM	,	0	} ,
	{ "M_SVlv_Chk_PrsDM"		, _K_D_IO	,	M_SVlv_Chk_PrsDM	,	0	} ,
	{ "M_SVlv_StartStopDM"		, _K_D_IO	,	M_SVlv_StartStopDM	,	0	} ,
	{ "CTC.PM1_CommStatus"		, _K_D_IO	,	CTC_PM1_CommStatus	,	0	} ,
	{ "CTC.PM2_CommStatus"		, _K_D_IO	,	CTC_PM2_CommStatus	,	0	} ,
	{ "CTC.PM3_CommStatus"		, _K_D_IO	,	CTC_PM3_CommStatus	,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// 2005.12.20
#include "DynamicIO.h"
CDynamicIO g_DynIO;
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	TMSIM_ISO_GATE,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"TMSIM_ISO_GATE",				_K_F_IO, 0},
	""
};


//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.11.15
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

// 2013.11.15
void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szData)
{
	char szEventID[20];

	do {
		if(STRCMP_L(szRunStr , "OPEN")) strcpy(szEventID, "SlitValveOpen");
		else if(STRCMP_L(szRunStr , "CLOSE")) strcpy(szEventID, "SlitValveClose");
		else break;

		_MLOG("FNC %s %s %s %s", szModule, szEventID, szStatus, szData);
	} while(0);
}


//////////////////////////////////////////////////////////////////////////
void WRITE_ACTION(int Mode , int Station) {
	//2006.11.30 mgsong
	//Isolation Valve For PM Function Set & Check Both Slot Valve of PM
	int CommStatus;

	switch (Mode) {
	case 0 :// Close
		WRITE_DIGITAL(BM1_SlotVvLXO + Station*2     , 2 , &CommStatus);
		WRITE_DIGITAL(BM1_SlotVvLXO + Station*2 + 1 , 2 , &CommStatus);
		break;
	case 1 :// Open
		WRITE_DIGITAL(BM1_SlotVvLXO + Station*2     , 1 , &CommStatus);
		WRITE_DIGITAL(BM1_SlotVvLXO + Station*2 + 1 , 1 , &CommStatus);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
int READ_ACTION( int Station , int *CommStatus ) {
	//2006.11.30 mgsong
	//Isolation Valve For PM Function Set & Check Both Slot Valve of PM
	int nResult , nResult2;

	nResult  = READ_DIGITAL(BM1_SlotVvLXI + Station*2     , CommStatus );
	nResult2 = READ_DIGITAL(BM1_SlotVvLXI + Station*2 + 1 , CommStatus );
	
	if (1 == nResult && 1 == nResult2) return 1;
	if (2 == nResult && 2 == nResult2) return 0;
	
	return 2;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsGateUsed(int Station)
{
	BOOL	bRet = TRUE;
	int		nIOStatus;
	int		nData = -1;

	enum {	eGATE_USED, eGATE_NOT_USED};

	switch (Station) {
	case BM1 :	nData = READ_DIGITAL(BM_Gate_UseNoUse,	&nIOStatus);	break;
	case BM2 :	nData = READ_DIGITAL(BM_Gate_UseNoUse,	&nIOStatus);	break;
	case PM1 :	nData = READ_DIGITAL(PM1_Gate_UseNoUse, &nIOStatus);	break;
	case PM2 :	nData = READ_DIGITAL(PM2_Gate_UseNoUse, &nIOStatus);	break;
	case PM3 :	nData = READ_DIGITAL(PM3_Gate_UseNoUse, &nIOStatus);	break;	
	}

	if(eGATE_USED != nData)		bRet = FALSE;

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsWaferSlideOK(int Station)
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	int		nWfrSlideIoIdx_L = -1;
	int		nWfrSlideIoIdx_R = -1;
	int		nAlarmIdx_L = -1;
	int		nAlarmIdx_R = -1;
	int		nSlideSts_L = 0;
	int		nSlideSts_R = 0;
	int		nRetryCount = 0;
	int		nAlmRecovery = 0;

	switch (Station) {
	case BM1  : 
		nWfrSlideIoIdx_L = BM_WaferSlideL1DI;	nAlarmIdx_L = ALARM_BM1ISO_CLOSE_L1_WAFER_SLIDE;
		nWfrSlideIoIdx_R = BM_WaferSlideR1DI;	nAlarmIdx_R = ALARM_BM1ISO_CLOSE_R1_WAFER_SLIDE;
		break;
	case BM2  :
		nWfrSlideIoIdx_L = BM_WaferSlideL1DI;	nAlarmIdx_L = ALARM_BM2ISO_CLOSE_L1_WAFER_SLIDE;
		nWfrSlideIoIdx_R = BM_WaferSlideR1DI;	nAlarmIdx_R = ALARM_BM2ISO_CLOSE_R1_WAFER_SLIDE;
		break;
	case PM1 :
		nWfrSlideIoIdx_L = PM1_WaferSlide1DI;	nAlarmIdx_L = ALARM_PM1ISO_CLOSE_L1_WAFER_SLIDE;
		nWfrSlideIoIdx_R = PM1_WaferSlide2DI;	nAlarmIdx_R = ALARM_PM1ISO_CLOSE_R1_WAFER_SLIDE;
		break;
	case PM2 :
		nWfrSlideIoIdx_L = PM2_WaferSlide1DI;	nAlarmIdx_L = ALARM_PM2ISO_CLOSE_L1_WAFER_SLIDE;
		nWfrSlideIoIdx_R = PM2_WaferSlide2DI;	nAlarmIdx_R = ALARM_PM2ISO_CLOSE_R1_WAFER_SLIDE;
		break;
	case PM3 :
		nWfrSlideIoIdx_L = PM3_WaferSlide1DI;	nAlarmIdx_L = ALARM_PM3ISO_CLOSE_L1_WAFER_SLIDE;
		nWfrSlideIoIdx_R = PM3_WaferSlide2DI;	nAlarmIdx_R = ALARM_PM3ISO_CLOSE_R1_WAFER_SLIDE;
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	//... Check & Retry
	while (1)
	{
		nSlideSts_L = READ_DIGITAL(nWfrSlideIoIdx_L, &nIOStatus);
		nSlideSts_R = READ_DIGITAL(nWfrSlideIoIdx_R, &nIOStatus);

		if(NORMAL == nSlideSts_L && NORMAL == nSlideSts_R)
		{	//... Check Completed
			bRet = TRUE;
			break;
		}

		//... Check Retry
		if(! WAIT_SECONDS(1))	break;

		nRetryCount++;
		_LOG("[WARN] Wafer Slide Check Retry L[%d] R[%d] Cnt[%d]", nSlideSts_L, nSlideSts_R, nRetryCount);

		//... Alarm Post
		if(nRetryCount > 5)
		{
			//... Left Slide Sensor
			if(NORMAL != nSlideSts_L)
			{
				nAlmRecovery = ALARM_MANAGE(nAlarmIdx_L);
				if     (ALM_RETRY  == nAlmRecovery)		nRetryCount = 0;
				else if(ALM_IGNORE == nAlmRecovery)
				{	//... Check Right Status
					if(NORMAL == nSlideSts_R)	{	bRet = TRUE;	break;	}
				}
				else	break;
			}

			//... Right Slide Sensor
			if(NORMAL != nSlideSts_R)
			{
				nAlmRecovery = ALARM_MANAGE(nAlarmIdx_R);
				if     (ALM_RETRY  == nAlmRecovery)		nRetryCount = 0;
				else if(ALM_IGNORE == nAlmRecovery)		{	bRet = TRUE;	break;	}
				else									break;
			}
		}
	}

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsAllArmRetract()
{
	BOOL	bRet = TRUE;
	int		nIOStatus;
	int		i;
	int		nRetryCount = 0;
	int		nArmStatus	= 0;
	int		nAlmRecovery = 0;

	//////////////////////////////////////////////////////////////////////////
	//... Check & Retry
	while (1)
	{
		bRet = TRUE;

		for (i = 0; i < MAX_VTM_ROBOT_ARM; i++)
		{
			nArmStatus = READ_DIGITAL(TM_VacRbRet_ARMADI + i, &nIOStatus);
			if(RETRACT_2 != nArmStatus)		{	bRet = FALSE;	break;	}
		}

		//... Check Completed
		if(TRUE == bRet)		break;

		//... Check Retry
		if(! WAIT_SECONDS(1))	break;

		nRetryCount++;
		_LOG("[WARN] Robot Arm[%d] NotRet[%d] Retry[%d]", i+1, nArmStatus, nRetryCount);

		//... Alarm Post
		if(nRetryCount > 5)
		{
			nAlmRecovery = ALARM_MANAGE(ALARM_ISO_CLOSE_RB_RET);
			if     (ALM_RETRY  == nAlmRecovery)		nRetryCount = 0;
			else if(ALM_IGNORE == nAlmRecovery)
			{
				bRet = TRUE;
				break;
			}
			else
			{	
				bRet = FALSE;
				break;
			}
		}
	}

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL SetPMSwapGasBeforeOpen(int Station, char *szModule)
{
	BOOL	bRet = TRUE;
	int		nIOStatus;
	
	Module_Status	msSubRet = SYS_SUCCESS;
	int		nAlmRecovery = ALM_CLEAR;

	char	szAlarm[256]	= "";
	char	szExtAlarm[256] = "";

	double	dbCfgATMPrs = 0;
	double	dbCurPrs	= 0;

	//... 2016.02.10  Skip Swap Gas While Slit Valve Cycle Test
	if(M_SVlv_CycTest == TRUE) return TRUE;

	while (1)
	{
		//... Check Other Station
		if(PM1 != Station && PM2 != Station && PM3 != Station)		break;

		//... Check TM ATM Pressure
		dbCfgATMPrs = READ_ANALOG(PRMA_SV_TMOpPrsAtm, &nIOStatus);
		dbCurPrs	= READ_ANALOG(TM_CGChmPrsAI, &nIOStatus);
		if(dbCfgATMPrs < dbCurPrs)	break;		//... TM ATM Pressure

		//... Check PM ATM Pressure
		dbCfgATMPrs = READ_ANALOG(PRMA_SV_PMOpPrsAtm, &nIOStatus);

		if     (PM1 == Station)		dbCurPrs = READ_ANALOG(PM1_ChmPrsAI, &nIOStatus);
		else if(PM2 == Station)		dbCurPrs = READ_ANALOG(PM2_ChmPrsAI, &nIOStatus);
		else if(PM3 == Station)		dbCurPrs = READ_ANALOG(PM3_ChmPrsAI, &nIOStatus);
		else						break;		//... Invalid Station

		if(dbCfgATMPrs < dbCurPrs)	break;		//... PM ATM Pressure

		//... Set Swap Purge Gas Flow
		if     (PM1 == Station)		msSubRet = RUN_FUNCTION(PM1_ABORT_GAS, "SWAP_PURGE");
		else if(PM2 == Station)		msSubRet = RUN_FUNCTION(PM2_ABORT_GAS, "SWAP_PURGE");
		else if(PM3 == Station)		msSubRet = RUN_FUNCTION(PM3_ABORT_GAS, "SWAP_PURGE");

		//... Check Result & Retry
		if(SYS_SUCCESS != msSubRet)
		{
			memset(szAlarm,		0, sizeof(szAlarm));
			memset(szExtAlarm,	0, sizeof(szExtAlarm));
			
			ALARM_MESSAGE_GET(ALARM_OPEN_PM_SWAP_PURGE_FAILED, szAlarm);			
			sprintf(szExtAlarm, "%s[%d] %s", szModule, (int) msSubRet, szAlarm);
			ALARM_MESSAGE_SET(ALARM_OPEN_PM_SWAP_PURGE_FAILED, szExtAlarm);
			
			nAlmRecovery = ALARM_MANAGE(ALARM_OPEN_PM_SWAP_PURGE_FAILED);
			if     (ALM_RETRY  == nAlmRecovery)		continue;
			else if(ALM_IGNORE == nAlmRecovery)		break;
			else
			{	//... Case. Aborted by User
				bRet = FALSE;
				break;
			}
		}

		//... Completed
		break;
	}

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Check_TM_PressRange()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;

	double	dbCfgPrs	= 0;
	double	dbCurPrs	= 0;

	do 
	{
		//... Read Pressure
		dbCurPrs = READ_ANALOG(TM_CGChmPrsAI, &nIOStatus);

		//... Check Skip Case in ATM Pressure
		dbCfgPrs = READ_ANALOG(PRMA_SV_TMOpPrsAtm, &nIOStatus);
		if(dbCfgPrs < dbCurPrs)		{	bRet = TRUE;	break;	}

		//... Check Pressure Range (Low & High)
		dbCfgPrs = READ_ANALOG(PRMA_SV_TMOpPrsLow, &nIOStatus);
		if(dbCurPrs < dbCfgPrs)		break;

		dbCfgPrs = READ_ANALOG(PRMA_SV_TMOpPrsHigh, &nIOStatus);
		if(dbCurPrs > dbCfgPrs)		break;

		bRet = TRUE;
	} while (0);

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Check_LL_PressRange(int Station)
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	
	double	dbCfgPrs	= 0;	
	double	dbCurPrs	= 0;
	
	do 
	{
		//... Read Pressure
		if     (BM1 == Station)		dbCurPrs = READ_ANALOG(BM1_CGChmPrsAI, &nIOStatus);
		else if(BM2 == Station)		dbCurPrs = READ_ANALOG(BM2_CGChmPrsAI, &nIOStatus);	
		else
		{	//.. Invalid Station
			bRet = TRUE;
			break;
		}

		//... Check Skip Case in ATM Pressure
		dbCfgPrs = READ_ANALOG(PRMA_SV_LLOpPrsAtm, &nIOStatus);
		if(dbCfgPrs < dbCurPrs)		{	bRet = TRUE;	break;	}
				
		//... Check Pressure Range (Low & High)
		dbCfgPrs = READ_ANALOG(PRMA_SV_LLOpPrsLow, &nIOStatus);
		if(dbCurPrs < dbCfgPrs)		break;
		
		dbCfgPrs = READ_ANALOG(PRMA_SV_LLOpPrsHigh, &nIOStatus);
		if(dbCurPrs > dbCfgPrs)		break;

		bRet = TRUE;
	} while (0);
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Check_PM_PressRange(int Station)
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	
	double	dbCfgPrs	= 0;	
	double	dbCurPrs	= 0;
	
	do 
	{
		//... Read Pressure
		if     (PM1 == Station)		dbCurPrs = READ_ANALOG(PM1_ChmPrsAI, &nIOStatus);
		else if(PM2 == Station)		dbCurPrs = READ_ANALOG(PM2_ChmPrsAI, &nIOStatus);
		else if(PM3 == Station)		dbCurPrs = READ_ANALOG(PM3_ChmPrsAI, &nIOStatus);
		else 
		{	//.. Invalid Station
			bRet = TRUE;
			break;
		}
		
		//... Check Skip Case in ATM Pressure
		dbCfgPrs = READ_ANALOG(PRMA_SV_PMOpPrsAtm, &nIOStatus);
		if(dbCfgPrs < dbCurPrs)		{	bRet = TRUE;	break;	}
		
		//... Check Pressure Range (Low & High)
		//.. PM Process Pressure
		if     (PM1 == Station)		dbCurPrs = READ_ANALOG(PM1_M651_Pressure, &nIOStatus);
		else if(PM2 == Station)		dbCurPrs = READ_ANALOG(PM2_M651_Pressure, &nIOStatus);
		else if(PM3 == Station)		dbCurPrs = READ_ANALOG(PM3_M651_Pressure, &nIOStatus);
		else 
		{	//.. Invalid Station
			bRet = TRUE;
			break;
		}
		
		dbCfgPrs = READ_ANALOG(PRMA_SV_PMOpPrsLow, &nIOStatus);
		if(dbCurPrs < dbCfgPrs)		break;
		
		dbCfgPrs = READ_ANALOG(PRMA_SV_PMOpPrsHigh, &nIOStatus);
		if(dbCurPrs > dbCfgPrs)		break;
		
		bRet = TRUE;
	} while (0);
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Check_PressDiff(int Station)
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	
	double	dbCfgPrs	= 0;	
	double	dbCurTMPrs	= 0;
	double	dbCurPrs	= 0;
	
	do 
	{
		//... Read TM Pressure
		dbCurTMPrs = READ_ANALOG(TM_CGChmPrsAI, &nIOStatus);

		//... Case. LL Swap
		if(BM1 == Station || BM2 == Station)
		{
			if     (BM1 == Station)		dbCurPrs = READ_ANALOG(BM1_CGChmPrsAI, &nIOStatus);
			else if(BM2 == Station)		dbCurPrs = READ_ANALOG(BM2_CGChmPrsAI, &nIOStatus);

			//... Check Skip Case in ATM Pressure
			dbCfgPrs = READ_ANALOG(PRMA_SV_LLOpPrsAtm, &nIOStatus);
			if(dbCfgPrs < dbCurPrs)		{	bRet = TRUE;	break;	}
			
			//... Check Diff Pressure
			if(dbCurTMPrs > dbCurPrs)						break;		//.. TM > LL (N.G)

			dbCfgPrs = READ_ANALOG(PRMA_SV_LLOpDiffPrs, &nIOStatus);
			if(fabs(dbCurPrs - dbCurTMPrs) > dbCfgPrs)		break;		//.. Different Pressure > Config
		}

		//... Case. PM Swap
		if(PM1 == Station || PM2 == Station || PM3 == Station)
		{
			//... Check ATM Pararmter
			if     (PM1 == Station)		dbCurPrs = READ_ANALOG(PM1_ChmPrsAI, &nIOStatus);
			else if(PM2 == Station)		dbCurPrs = READ_ANALOG(PM2_ChmPrsAI, &nIOStatus);
			else if(PM3 == Station)		dbCurPrs = READ_ANALOG(PM3_ChmPrsAI, &nIOStatus);

			//... Check Skip Case in ATM Pressure
			dbCfgPrs = READ_ANALOG(PRMA_SV_PMOpPrsAtm, &nIOStatus);
			if(dbCfgPrs < dbCurPrs)		{	bRet = TRUE;	break;	}

			//... Check VTM Parameter
			if     (PM1 == Station)		dbCurPrs = READ_ANALOG(PM1_M651_Pressure, &nIOStatus);
			else if(PM2 == Station)		dbCurPrs = READ_ANALOG(PM2_M651_Pressure, &nIOStatus);
			else if(PM3 == Station)		dbCurPrs = READ_ANALOG(PM3_M651_Pressure, &nIOStatus);

			//... Check Diff Pressure
			if(dbCurTMPrs < dbCurPrs)						break;		//.. TM < PM (N.G)

			dbCfgPrs = READ_ANALOG(PRMA_SV_PMOpDiffPrs, &nIOStatus);
			if(fabs(dbCurPrs - dbCurTMPrs) > dbCfgPrs)		break;		//.. Different Pressure > Config
		}
		
		bRet = TRUE;
	} while (0);
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL CheckPressforSlotOpen(int Station, char *szModule)
{
	BOOL	bRet = TRUE;
	int		nIOStatus;
	double	dbCheckTimeOut	= 0;
	double	dbElapsedTime	= 0;

	int		nAlmRecovery	= ALM_CLEAR;
	char	szAlarm[256]	= "";
	char	szExtAlarm[256] = "";

	double	dbAlmCurValue	= 0;
	double	dbAlmCurValue2	= 0;

	BOOL	bCheck_TM_PressRange = TRUE;
	BOOL	bCheck_LL_PressRange = TRUE;
	BOOL	bCheck_PM_PressRange = TRUE;
	BOOL	bCheck_PressDiff	 = TRUE;

	S_TIMER_READY();	//... Reset Timer
	
	while (1)
	{
		//////////////////////////////////////////////////////////////////////////
		//... 1. Check TM Pressure Range
		bCheck_TM_PressRange = Check_TM_PressRange();
		if(FALSE == bCheck_TM_PressRange)
		{
			dbElapsedTime	= S_TIMER_ELAPSED();
			dbCheckTimeOut	= READ_DIGITAL(PRMD_SV_TMPrsChkTMO, &nIOStatus); 

			if(dbElapsedTime > dbCheckTimeOut)
			{
				memset(szAlarm,		0, sizeof(szAlarm));
				memset(szExtAlarm,	0, sizeof(szExtAlarm));
				
				ALARM_MESSAGE_GET(ALARM_OPEN_PRESS_RANGE_OVER, szAlarm);
				
				dbAlmCurValue = READ_ANALOG(TM_CGChmPrsAI, &nIOStatus);
				sprintf(szExtAlarm, "TM %s [%.3f Torr]", szAlarm, dbAlmCurValue);
				
				ALARM_MESSAGE_SET(ALARM_OPEN_PRESS_RANGE_OVER, szExtAlarm);
				
				nAlmRecovery = ALARM_MANAGE(ALARM_OPEN_PRESS_RANGE_OVER);
				if(ALM_RETRY == nAlmRecovery)
				{
					S_TIMER_READY();	//... Reset Timer
					continue;
				}
				else if(ALM_IGNORE == nAlmRecovery)		break;
				else
				{	//... Case. Aborted by User
					bRet = FALSE;
					break;
				}
			}
		}
		
		//////////////////////////////////////////////////////////////////////////
		//... 2. Check Target Module LL Pressure Range
		if(BM1 == Station || BM2 == Station)
		{
			bCheck_LL_PressRange = Check_LL_PressRange(Station);
			if(FALSE == bCheck_LL_PressRange)
			{
				dbElapsedTime	= S_TIMER_ELAPSED();
				dbCheckTimeOut	= READ_DIGITAL(PRMD_SV_LLPrsChkTMO, &nIOStatus); 
				
				if(dbElapsedTime > dbCheckTimeOut)
				{
					memset(szAlarm,		0, sizeof(szAlarm));
					memset(szExtAlarm,	0, sizeof(szExtAlarm));
					
					ALARM_MESSAGE_GET(ALARM_OPEN_PRESS_RANGE_OVER, szAlarm);					
					
					if     (BM1 == Station)		dbAlmCurValue = READ_ANALOG(BM1_CGChmPrsAI, &nIOStatus);
					else if(BM2 == Station)		dbAlmCurValue = READ_ANALOG(BM2_CGChmPrsAI, &nIOStatus);
					sprintf(szExtAlarm, "%s %s [%.3f Torr]", szModule, szAlarm, dbAlmCurValue);					
					
					ALARM_MESSAGE_SET(ALARM_OPEN_PRESS_RANGE_OVER, szExtAlarm);					
					nAlmRecovery = ALARM_MANAGE(ALARM_OPEN_PRESS_RANGE_OVER);
					if(ALM_RETRY == nAlmRecovery)
					{
						S_TIMER_READY();	//... Reset Timer
						continue;
					}
					else if(ALM_IGNORE == nAlmRecovery)		break;
					else
					{	//... Case. Aborted by User
						bRet = FALSE;
						break;
					}
				}				
			}
		}
		
		//////////////////////////////////////////////////////////////////////////
		//... 3. Check Target Module PM Pressure Range
		if(PM1 == Station || PM2 == Station || PM3 == Station)
		{
			bCheck_PM_PressRange = Check_PM_PressRange(Station);
			if(FALSE == bCheck_PM_PressRange)
			{
				dbElapsedTime	= S_TIMER_ELAPSED();
				dbCheckTimeOut	= READ_DIGITAL(PRMD_SV_PMPrsChkTMO, &nIOStatus); 
				
				if(dbElapsedTime > dbCheckTimeOut)
				{
					memset(szAlarm,		0, sizeof(szAlarm));
					memset(szExtAlarm,	0, sizeof(szExtAlarm));
					
					ALARM_MESSAGE_GET(ALARM_OPEN_PRESS_RANGE_OVER, szAlarm);
					
					if     (PM1 == Station)		dbAlmCurValue = READ_ANALOG(PM1_M651_Pressure, &nIOStatus);
					else if(PM2 == Station)		dbAlmCurValue = READ_ANALOG(PM2_M651_Pressure, &nIOStatus);
					else if(PM3 == Station)		dbAlmCurValue = READ_ANALOG(PM3_M651_Pressure, &nIOStatus);
					sprintf(szExtAlarm, "%s %s [%.3f Torr]", szModule, szAlarm, dbAlmCurValue);
					
					ALARM_MESSAGE_SET(ALARM_OPEN_PRESS_RANGE_OVER, szExtAlarm);
					
					nAlmRecovery = ALARM_MANAGE(ALARM_OPEN_PRESS_RANGE_OVER);
					if(ALM_RETRY == nAlmRecovery)
					{
						S_TIMER_READY();	//... Reset Timer
						continue;
					}
					else if(ALM_IGNORE == nAlmRecovery)		break;
					else
					{	//... Case. Aborted by User
						bRet = FALSE;
						break;
					}
				}				
			}
		}
		
		//////////////////////////////////////////////////////////////////////////
		//... 4. Check Different Pressure
		bCheck_PressDiff = Check_PressDiff(Station);
		if(FALSE == bCheck_PressDiff)
		{
			if(BM1 == Station || BM2 == Station)
			{
				dbCheckTimeOut = READ_DIGITAL(PRMD_SV_LLPrsChkTMO, &nIOStatus);
			}
			else if(PM1 == Station || PM2 == Station || PM3 == Station)
			{
				dbCheckTimeOut = READ_DIGITAL(PRMD_SV_PMPrsChkTMO, &nIOStatus);
			}

			dbElapsedTime = S_TIMER_ELAPSED();			
			if(dbElapsedTime > dbCheckTimeOut)
			{
				memset(szAlarm,		0, sizeof(szAlarm));
				memset(szExtAlarm,	0, sizeof(szExtAlarm));
				
				ALARM_MESSAGE_GET(ALARM_OPEN_PRESS_DIFF_OVER, szAlarm);
							
				//... Create Alarm Message
				dbAlmCurValue2 = READ_ANALOG(TM_CGChmPrsAI, &nIOStatus);		//... Read TM Pressure
				if     (BM1 == Station)		dbAlmCurValue = READ_ANALOG(BM1_CGChmPrsAI, &nIOStatus);
				else if(BM2 == Station)		dbAlmCurValue = READ_ANALOG(BM2_CGChmPrsAI, &nIOStatus);
				else if(PM1 == Station)		dbAlmCurValue = READ_ANALOG(PM1_M651_Pressure, &nIOStatus);
				else if(PM2 == Station)		dbAlmCurValue = READ_ANALOG(PM2_M651_Pressure, &nIOStatus);
				else if(PM3 == Station)		dbAlmCurValue = READ_ANALOG(PM3_M651_Pressure, &nIOStatus);
				sprintf(szExtAlarm, "%s %s [%s %.3f][TM %.3f]", szModule, szAlarm, szModule, dbAlmCurValue, dbAlmCurValue2);
				
				ALARM_MESSAGE_SET(ALARM_OPEN_PRESS_DIFF_OVER, szExtAlarm);
				
				nAlmRecovery = ALARM_MANAGE(ALARM_OPEN_PRESS_DIFF_OVER);
				if(ALM_RETRY == nAlmRecovery)
				{
					S_TIMER_READY();	//... Reset Timer
					continue;
				}
				else if(ALM_IGNORE == nAlmRecovery)		break;
				else
				{	//... Case. Aborted by User
					bRet = FALSE;
					break;
				}
			}				
		}
		
		//... Check Result
		if( FALSE == bCheck_TM_PressRange ||
			FALSE == bCheck_LL_PressRange ||
			FALSE == bCheck_PM_PressRange ||
			FALSE == bCheck_PressDiff)
		{	//... Not Completed
			if(! WAIT_SECONDS(0.1))		break;
			continue;
		}

		//... Completed
		break;
	}
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
void DelayTimeBeforeOpen(int Station)
{
	int		nIOStatus;
	double	dbCfgTime = 0;

	switch (Station) {
	case BM1 :
	case BM2 :
		dbCfgTime = READ_ANALOG(PRMA_SV_LLOpDelay, &nIOStatus);
		break;
	case PM1 :
	case PM2 :
	case PM3 :
		dbCfgTime = READ_ANALOG(PRMA_SV_PMOpDelay, &nIOStatus);
		break;
	}

	if(dbCfgTime > 0)	WAIT_SECONDS(dbCfgTime);
}

//////////////////////////////////////////////////////////////////////////
void DelayTimeAfterOpen(int Station)
{
	int		nIOStatus;
	double	dbCfgTime = 0;

	switch (Station) {
	case BM1 :
	case BM2 :
		dbCfgTime = READ_ANALOG(PRMA_SV_LLOpDelay2, &nIOStatus);
		break;
	case PM1 :
	case PM2 :
	case PM3 :
		dbCfgTime = READ_ANALOG(PRMA_SV_PMOpDelay2, &nIOStatus);
		break;
	}
	
	if(dbCfgTime > 0)	WAIT_SECONDS(dbCfgTime);
}

//////////////////////////////////////////////////////////////////////////
BOOL SetPMCloseGasAfterClose(int Station)
{
	BOOL	bRet = TRUE;
	int		nIOStatus;
	
	Module_Status	msFuncRet = SYS_SUCCESS;

	double	dbCfgATMPrs = 0;
	double	dbCurPrs	= 0;
	
	do 
	{
		//... Check Other Station
		if(PM1 != Station && PM2 != Station && PM3 != Station)		break;
		
		//... Check TM ATM Pressure
		dbCfgATMPrs = READ_ANALOG(PRMA_SV_TMOpPrsAtm, &nIOStatus);
		dbCurPrs	= READ_ANALOG(TM_CGChmPrsAI, &nIOStatus);
		if(dbCfgATMPrs < dbCurPrs)	break;		//... TM ATM Pressure
		
		//... Check PM ATM Pressure
		dbCfgATMPrs = READ_ANALOG(PRMA_SV_PMOpPrsAtm, &nIOStatus);
		
		if     (PM1 == Station)		dbCurPrs = READ_ANALOG(PM1_ChmPrsAI, &nIOStatus);
		else if(PM2 == Station)		dbCurPrs = READ_ANALOG(PM2_ChmPrsAI, &nIOStatus);
		else if(PM3 == Station)		dbCurPrs = READ_ANALOG(PM3_ChmPrsAI, &nIOStatus);
		else						break;		//... Invalid Station
		
		if(dbCfgATMPrs < dbCurPrs)	break;		//... PM ATM Pressure
		
		//... Set Swap Purge Gas Flow
		if     (PM1 == Station)		msFuncRet = RUN_FUNCTION(PM1_ABORT_GAS, "");
		else if(PM2 == Station)		msFuncRet = RUN_FUNCTION(PM2_ABORT_GAS, "");
		else if(PM3 == Station)		msFuncRet = RUN_FUNCTION(PM3_ABORT_GAS, "");
		
	} while (0);
	
	if(SYS_SUCCESS != msFuncRet)	bRet = FALSE;
	
	return	bRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2015.10.08 Add Check Other Station Slot Valve Closed
BOOL IsAllSlotValveClosed(int Station)
{
	BOOL	bRet = TRUE;
	int		nIOStatus;
	int		i = 0;
	int		nAlarmRecovery;
	int		nRetryCnt = 0;
	int		nS_Index = BM1;
	
	while(1)
	{
		bRet = TRUE;
		
		//... Check Slot Valve CLOSE in Vacuum
		if(VAC != READ_DIGITAL(TM_ATMVACSensorXI, &nIOStatus))		break;
		
		//... 2015.10.15 
		for(nS_Index = BM1; nS_Index < PM3; nS_Index++)
		{
			if(nS_Index == Station)
			{
				continue;  //... Skip Open Station
			}
			if(CLOSE != READ_ACTION(nS_Index, &nIOStatus))
			{
				_LOG("[INFO] IsAllSlotValveClosed Option Open Station [%d]", nS_Index);
				bRet = FALSE;
			}
		}
		
		if(FALSE == bRet)
		{
			nRetryCnt++;
			if(nRetryCnt < MAX_RETRY_COUNT)
			{	//... Auto Retry : 5
				if(! WAIT_SECONDS(1))		break;
				_LOG("[INFO] IsAllSlotValveClosed AutoRetry[%d]", nRetryCnt);
				
				continue;
			}
			
			nRetryCnt = 0;
			nAlarmRecovery = ALARM_MANAGE(ALARM_ISO_OPEN_NOT_ALL_CLOSE);
			if(ALM_RETRY == nAlarmRecovery)
			{
				if(! WAIT_SECONDS(2))		break;
				continue;	//... RETRY
			}
			else if(ALM_IGNORE == nAlarmRecovery)		bRet = TRUE;
		}
		
		//... Fixed
		break;
	}
	
	return	bRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2016.02.10
void CalculateControlTime(int Station, int Mode, double dbEndTime)
{
	int nIOStatus;
	//////////////////////////////////////////////////////////////////////////
	//... Update Open/Close Time
	if( Mode == OPEN)
	{
		switch (Station) {
		case BM1 :	WRITE_ANALOG(BM1_Slit_OPTm_AM, dbEndTime, &nIOStatus); break;
		case BM2 :	WRITE_ANALOG(BM2_Slit_OPTm_AM, dbEndTime, &nIOStatus); break;
		case PM1 :	WRITE_ANALOG(PM1_Slit_OPTm_AM, dbEndTime, &nIOStatus); break;
		case PM2 :	WRITE_ANALOG(PM2_Slit_OPTm_AM, dbEndTime, &nIOStatus); break;
		case PM3 :	WRITE_ANALOG(PM3_Slit_OPTm_AM, dbEndTime, &nIOStatus); break;
		}
	}
	else if( Mode == CLOSE)
	{
		switch (Station) {
		case BM1 :	WRITE_ANALOG(BM1_Slit_CLTm_AM, dbEndTime, &nIOStatus); break;
		case BM2 :	WRITE_ANALOG(BM2_Slit_CLTm_AM, dbEndTime, &nIOStatus); break;
		case PM1 :	WRITE_ANALOG(PM1_Slit_CLTm_AM, dbEndTime, &nIOStatus); break;
		case PM2 :	WRITE_ANALOG(PM2_Slit_CLTm_AM, dbEndTime, &nIOStatus); break;
		case PM3 :	WRITE_ANALOG(PM3_Slit_CLTm_AM, dbEndTime, &nIOStatus); break;
		}
	}
	else
	{
		printf("[TM ISO Function] Unknown Mode \n");
	}
}
//////////////////////////////////////////////////////////////////////////
Module_Status IsoGateControl(int Station, int Mode) {
	int CommStatus , AlmResult , Data;
	double Timer;
	double dbEndTime = 0;

	char StationStr[40] = "";
	char RunStr[40] = "";

	//////////////////////////////////////////////////////////////////////////
	//... Check Gate Valve Used
	if(FALSE == IsGateUsed(Station))					return SYS_SUCCESS;	

	//////////////////////////////////////////////////////////////////////////
	//... Check Already Status (OPEN or CLOSE)
	if(READ_ACTION(Station, &CommStatus) == Mode)		return SYS_SUCCESS;

	//////////////////////////////////////////////////////////////////////////
	//... Pre Check CLOSE Action
	if(CLOSE == Mode)
	{
		//... Check Wafer Slide Sensor
		if(FALSE == IsWaferSlideOK(Station))			return SYS_ABORTED;

		//... Check Arm Retract Status
		if(FALSE == IsAllArmRetract())					return SYS_ABORTED;
	}
	//////////////////////////////////////////////////////////////////////////
	//... 2015.10.08 Add Check Other Station Slot Valve All Closed
	if(OPEN == Mode)
	{
		if(FALSE == IsAllSlotValveClosed(Station))		return SYS_ABORTED;
	}
	//////////////////////////////////////////////////////////////////////////
	//... Check Already Status (OPEN or CLOSE)
	if(READ_ACTION(Station, &CommStatus) == Mode)		return SYS_SUCCESS;

	//////////////////////////////////////////////////////////////////////////
	//... Mars Event
	switch (Station) {
	case BM1 :	strcpy(StationStr, "BM1");		break;
	case BM2 :	strcpy(StationStr, "BM2");		break;
	case PM1 :	strcpy(StationStr, "PM1");		break;
	case PM2 :	strcpy(StationStr, "PM2");		break;
	case PM3 :	strcpy(StationStr, "PM3");		break;
	}

	if(CLOSE == Mode)		strcpy(RunStr, "Close");
	else					strcpy(RunStr, "Open");

	LogEvent(StationStr, RunStr, "Start", "");	// 2014.07.25
	//////////////////////////////////////////////////////////////////////////
	//... 2016.02.10 Start to Calculate Slit Valve Open/Close Time 
	S_TIMER_READY();

	//////////////////////////////////////////////////////////////////////////
	//... Run Action for Available Retry
	while(TRUE) 
	{
		Timer = 0;

		//... 2015.10.08 Add Slot Valve OPEN Pressure Check & Swap Purge
		if( OPEN == Mode && 
			CLOSE == READ_ACTION(Station, &CommStatus) &&
			enCTRLOPT_ON == READ_DIGITAL(FC_TM_SV_OPEN_CHECK, &CommStatus))
		{	//... Action OPEN Before CLOSE
			//... 1. Set Swap Purge
			if(FALSE == SetPMSwapGasBeforeOpen(Station, StationStr))	return SYS_ABORTED;
			
			//... 2. Check Pressure Range
			if(FALSE == CheckPressforSlotOpen(Station, StationStr))		return SYS_ABORTED;
			
			//... Delay Before Open
			DelayTimeBeforeOpen(Station);
		}

		/////////////////////////////
		//... Run Slot Valve Control
		WRITE_ACTION(Mode, Station);

		//... Case Sensor Check Skip
		if(READ_DIGITAL(SENSOR_CHECK, &CommStatus) == 0)
		{	//... Sensor Not Check Fixed Time 3 sec
			if(! WAIT_SECONDS(3))		return SYS_ABORTED;
			break;
		}
		
		while(1) 
		{
			//... Check Abort
			if(! WAIT_SECONDS(0.1))		return SYS_ABORTED;

			/////////////////////////////
			//... Check Completed
			Data = READ_ACTION(Station, &CommStatus);

			if(! CommStatus)
			{	//... Check IO Read Fail
				AlmResult = ALARM_MANAGE(ALARM_ISOGATE_COMM_ERR);
				if     (AlmResult == ALM_IGNORE)	return SYS_SUCCESS;
				else if(AlmResult != ALM_RETRY)		return SYS_ABORTED;
				else                                break;		//... Reaction
			}
			else if(Data == Mode) 
			{
				//... Completed Action
				break;
			}
			else if((int)Timer > READ_DIGITAL(ISO_OPCL_TIMEOUT, &CommStatus)) 
			{	
				//... Control Timeout
				if(Mode == 0)		AlmResult = ALARM_MANAGE(ALARM_ISO_CLOSE_TMO);
				else				AlmResult = ALARM_MANAGE(ALARM_ISO_OPEN_TMO);

				//... Alarm Recovery
				if     (AlmResult == ALM_IGNORE)	return SYS_SUCCESS;
				else if(AlmResult != ALM_RETRY)		return SYS_ABORTED;
				else                                break;
			}
			else Timer += 0.1;
		}

		if(Data == Mode)		break;
	}
	//////////////////////////////////////////////////////////////////////////
	//... 2016.02.10 Stop to Calculate Slit Valve Open/Close Time 
	dbEndTime = S_TIMER_ELAPSED();
	
	CalculateControlTime(Station,Mode,dbEndTime);
	//////////////////////////////////////////////////////////////////////////
	//... Delay After Open
	if( OPEN == Mode &&
		enCTRLOPT_ON == READ_DIGITAL(FC_TM_SV_OPEN_CHECK, &CommStatus))
	{
		DelayTimeAfterOpen(Station);
	}

	LogEvent(StationStr, RunStr, "End", "");	// 2014.07.25

	return SYS_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////	
//... Check CTC & PM Interlock ,PM Process Running
BOOL CheckInterlock(int nStation)
{
	BOOL	bRet		= FALSE;
	int		nIOStatus	= 0;
	char    szExtAlarm[256];
	char	szAlarm[256];

	memset(szAlarm,		0, sizeof(szAlarm));
	memset(szExtAlarm,0,sizeof(szExtAlarm));

	do 
	{
		//... Check CTC Interlock
		if(eIDLE != READ_DIGITAL(CTC_AbortControl,&nIOStatus))
		{
		    strcpy(szExtAlarm, "CTC Abort");	
			break;
		}

		if(PM1 == nStation)
		{
			//... Check PM1 Maint Mode
			if(eMaint != READ_DIGITAL(CTC_PM1_CommStatus,&nIOStatus)) { strcpy(szExtAlarm, "(Need to PM1 maint)"); break;}
			//... Check PM1 Process Stop 
			if(eSTOP != READ_DIGITAL(PM1_PRO_RunStsDM,&nIOStatus))	  { strcpy(szExtAlarm, "(PM1 Run)"); break;}
			//... Check PM1 Interlock
			if(eNORMAL != READ_DIGITAL(PM1_SW_GbIntlckDM,&nIOStatus)) { strcpy(szExtAlarm, "(PM1 Ink)"); break;}
		}
		else if(PM2 == nStation)
		{
			//... Check PM2 Maint Mode
			if(eMaint != READ_DIGITAL(CTC_PM2_CommStatus,&nIOStatus)) { strcpy(szExtAlarm, "(Need to PM2 maint)"); break;}
			//... Check PM2 Process Stop 
			if(eSTOP != READ_DIGITAL(PM2_PRO_RunStsDM,&nIOStatus))	  { strcpy(szExtAlarm, "(PM2 Run)"); break;}
			//... Check PM2 Interlock
			if(eNORMAL != READ_DIGITAL(PM2_SW_GbIntlckDM,&nIOStatus)) { strcpy(szExtAlarm, "(PM2 Ink)"); break;} 
		}
		else if(PM3 == nStation)
		{
			//... Check PM3 Maint Mode
			if(eMaint != READ_DIGITAL(CTC_PM3_CommStatus,&nIOStatus)) { strcpy(szExtAlarm, "(Need to PM3 maint)"); break;}
			//... Check PM3 Process Stop 
			if(eSTOP != READ_DIGITAL(PM3_PRO_RunStsDM,&nIOStatus))	  { strcpy(szExtAlarm, "(PM3 Run)"); break;}
			//... Check PM3 Interlock
			if(eNORMAL != READ_DIGITAL(PM3_SW_GbIntlckDM,&nIOStatus)) { strcpy(szExtAlarm, "(PM3 Ink)"); break;}
		}
		else if(BM1 == nStation)
		{
			//... Check LL1 Venting & Pumping
			if(eVS_GOATM == READ_DIGITAL(BM1_VacuumStatusDM,&nIOStatus)
				|| eVS_GOVAC == READ_DIGITAL(BM1_VacuumStatusDM,&nIOStatus)) { strcpy(szExtAlarm, "(LL1 Run)"); break;}
		}
		else if(BM2 == nStation)
		{
			//... Check LL1 Venting & Pumping
			if(eVS_GOATM == READ_DIGITAL(BM2_VacuumStatusDM,&nIOStatus)
				|| eVS_GOVAC == READ_DIGITAL(BM2_VacuumStatusDM,&nIOStatus)) { strcpy(szExtAlarm, "(LL2 Run)"); break;}
		}
		bRet = TRUE;
	} while (0);
	
	if(bRet == FALSE)
	{	
		ALARM_MESSAGE_GET(ALARM_SLIT_CYCLE_TEST_FAIL, szAlarm);			
		strcat(szAlarm,szExtAlarm);
		ALARM_MESSAGE_SET(ALARM_SLIT_CYCLE_TEST_FAIL, szAlarm);
		
		ALARM_POST(ALARM_SLIT_CYCLE_TEST_FAIL);
	}
	return	bRet;
}
//////////////////////////////////////////////////////////////////////////
//... Check TM,LL,PM Pressure
BOOL CheckPressure(int nStation)
{
	BOOL bRet = FALSE;
	int nIOStatus;
	char szStationStr[8];
    
	memset(szStationStr,0,sizeof(szStationStr));

	//... for Alarm Detail
	switch (nStation) 
	{
		case BM1 :	strcpy(szStationStr, "BM1");		break;
		case BM2 :	strcpy(szStationStr, "BM2");		break;
		case PM1 :	strcpy(szStationStr, "PM1");		break;
		case PM2 :	strcpy(szStationStr, "PM2");		break;
		case PM3 :	strcpy(szStationStr, "PM3");		break;
	}
	do 
	{
		//... Option Check
		if(eOFF == READ_DIGITAL(M_SVlv_Chk_PrsDM,&nIOStatus)) {bRet = TRUE; break;}
		//... Transfer Pressure Check
		if(FALSE == CheckPressforSlotOpen(nStation, szStationStr))	break;		
		bRet = TRUE;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CYCLE_TEST()
{
	Module_Status msRet = SYS_ABORTED;

	int		nIOStatus	= 0;
	int		nTargetCnt	= 0;
	int		nCurrentCnt	= 0;
	int		i			= 0;
	int		nStation	= BM1;

	double	dbOpenSumTime[MAX_SLIT_VALVE];
	double	dbOpenMinTime[MAX_SLIT_VALVE];
	double	dbOpenMaxTime[MAX_SLIT_VALVE];

	double	dbCloseSumTime[MAX_SLIT_VALVE];
	double	dbCloseMinTime[MAX_SLIT_VALVE];
	double	dbCloseMaxTime[MAX_SLIT_VALVE];

	double	dbCurTime	= 0;
	double	dbAvrTime	= 0;

	enum { eVLV_CLOSE, eVLV_OPEN };
	enum { eOFFON_OFF, eOFFON_ON };
	enum { eRESULT_NONE, eRESULT_ERROR, eRESULT_OK };

	//////////////////////////////////////////////////////////////////////////
	//... Check Target Cycle Count
	nTargetCnt = (int) READ_ANALOG(M_SVlv_CycTgtCntAM, &nIOStatus);
	if(nTargetCnt < 1)		return SYS_SUCCESS;

	//////////////////////////////////////////////////////////////////////////
	//... Init Valve Time
	for(i = 0; i < MAX_SLIT_VALVE; i++)
	{
		//... Open Info
		dbOpenSumTime[i] = 0;
		dbOpenMinTime[i] = 0;
		dbOpenMaxTime[i] = 0;
		
		WRITE_ANALOG(M_SVlvBM1_OP_MinTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_SVlvBM1_OP_MaxTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_SVlvBM1_OP_AvgTmAM + i, 0, &nIOStatus);

		//... Close Info
		dbCloseSumTime[i] = 0;
		dbCloseMinTime[i] = 0;
		dbCloseMaxTime[i] = 0;

		WRITE_ANALOG(M_SVlvBM1_CL_MinTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_SVlvBM1_CL_MaxTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_SVlvBM1_CL_AvgTmAM + i, 0, &nIOStatus);
	}

	//... Init Result Status
	nStation = READ_DIGITAL(M_SVlv_CycTgtMdlDM, &nIOStatus);

	if(nStation >= PM4) return SYS_SUCCESS;

	//////////////////////////////////////////////////////////////////////////
	//... Slit Valve CLOSE Before Test
	msRet = IsoGateControl(nStation, CLOSE);
	if(msRet != SYS_SUCCESS) return msRet;

	//////////////////////////////////////////////////////////////////////////
	//... Run Valve Verification	
	do 
	{
		//... Check Cycle Count
		nCurrentCnt++;
		if(nCurrentCnt > nTargetCnt)
		{
			msRet = SYS_SUCCESS;
			break;		//... Completed
		}

		//... Update Current Count
		WRITE_ANALOG(M_SVlv_CycCurCntAM, nCurrentCnt, &nIOStatus);


		//////////////////////////////////////////////////////////////////////////
		//... Check Interlock & PM Process Running
		if(FALSE == CheckInterlock(nStation))
		{
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check Interlock
		if(FALSE == CheckPressure(nStation))
		{
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		//... Set Valve OPEN
		msRet = IsoGateControl(nStation, OPEN);
	
		if(msRet != SYS_SUCCESS)	break;
		
		if(! WAIT_SECONDS(1))		break;

		//////////////////////////////////////////////////////////////////////////
		//... Update OPEN Min/Max Time
		dbCurTime = READ_ANALOG(BM1_Slit_OPTm_AM + nStation, &nIOStatus);
		if(dbCurTime < dbOpenMinTime[nStation] || nCurrentCnt == 1)
		{
			dbOpenMinTime[nStation] = dbCurTime;
			WRITE_ANALOG(M_SVlvBM1_OP_MinTmAM + nStation, dbCurTime, &nIOStatus);
		}
		if(dbCurTime > dbOpenMaxTime[nStation] || nCurrentCnt == 1)
		{
			dbOpenMaxTime[nStation] = dbCurTime;
			WRITE_ANALOG(M_SVlvBM1_OP_MaxTmAM + nStation, dbCurTime, &nIOStatus);
		}
		
		//... Update OPEN Avg Time
		dbOpenSumTime[nStation] = dbOpenSumTime[nStation] + dbCurTime;
		if(dbOpenSumTime[nStation] > 0)	dbAvrTime = dbOpenSumTime[nStation] / nCurrentCnt;
		else						dbAvrTime = 0;
		WRITE_ANALOG(M_SVlvBM1_OP_AvgTmAM + nStation, dbAvrTime, &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		//... Set Valve CLOSE
		msRet = IsoGateControl(nStation, CLOSE);
		
		if(msRet != SYS_SUCCESS)	break;
		
		if(! WAIT_SECONDS(1))		break;
		
		//////////////////////////////////////////////////////////////////////////
		//... Update CLOSE Min/Max Time
		dbCurTime = READ_ANALOG(BM1_Slit_CLTm_AM + nStation, &nIOStatus);
		if(dbCurTime < dbCloseMinTime[nStation] || nCurrentCnt == 1)
		{
			dbCloseMinTime[nStation] = dbCurTime;
			WRITE_ANALOG(M_SVlvBM1_CL_MinTmAM + nStation, dbCurTime, &nIOStatus);
		}
		if(dbCurTime > dbCloseMaxTime[nStation] || nCurrentCnt == 1)
		{
			dbCloseMaxTime[nStation] = dbCurTime;
			WRITE_ANALOG(M_SVlvBM1_CL_MaxTmAM + nStation, dbCurTime, &nIOStatus);
		}
		
		//... Update CLOSE Avg Time
		dbCloseSumTime[nStation] = dbCloseSumTime[nStation] + dbCurTime;
		
		if(dbCloseSumTime[nStation] > 0)	dbAvrTime = dbCloseSumTime[nStation] / nCurrentCnt;
		else						dbAvrTime = 0;
		WRITE_ANALOG(M_SVlvBM1_CL_AvgTmAM + nStation, dbAvrTime, &nIOStatus);

	} while (1);	// Unlimited Loop

	//////////////////////////////////////////////////////////////////////////
	//... Slit Valve CLOSE
	msRet = IsoGateControl(nStation, CLOSE);

	//... Toggle Save Parameter
	WRITE_DIGITAL(M_SVlv_ParmSaveDM, eOFFON_ON, &nIOStatus);

	return msRet; 
}


BOOL IsPMSlitValveClose(char *RunStr, int nSwitch)
{
	BOOL bRet = TRUE;
	int  nIOStatus;
	do 
	{
		//... Skip Close Command
		if(STRCMP_L(RunStr, "CLOSE")) break;

		if(nSwitch == TRUE) break;	

		//... Job Running Check
		if(READ_DIGITAL(LOT_CONTROL,&nIOStatus) ==LS_WAIT_HANDOFF
			&&READ_DIGITAL(LOT2_CONTROL,&nIOStatus) ==LS_WAIT_HANDOFF
			&&READ_DIGITAL(LOT3_CONTROL,&nIOStatus) ==LS_WAIT_HANDOFF) break;

		//... Check PM1 Slit Valve Status
		if(READ_ACTION(PM1 , &nIOStatus) == OPEN)
		{
			//... Slit Valve Close Only At PM Maint,CTCDis,CTCDis2 
			if(eMaint == READ_DIGITAL(CTC_PM1_CommStatus,&nIOStatus)
				|| eCTCDis  == READ_DIGITAL(CTC_PM1_CommStatus,&nIOStatus)
				|| eCTCDis2  == READ_DIGITAL(CTC_PM1_CommStatus,&nIOStatus))
			{
				//... Action Slit Valve Close
				printf("[ISOGATE] PM1 Slit Valve Open So Valve Close\n");
					if(SYS_SUCCESS != IsoGateControl(PM1, CLOSE)) {bRet = FALSE; break;}
			}
		}

		//... Check PM2 Slit Valve Status
		if(READ_ACTION(PM2 , &nIOStatus) == OPEN)
		{
			//... Slit Valve Close Only At PM Maint,CTCDis,CTCDis2 
			if(eMaint == READ_DIGITAL(CTC_PM2_CommStatus,&nIOStatus)
				|| eCTCDis  == READ_DIGITAL(CTC_PM2_CommStatus,&nIOStatus)
				|| eCTCDis2  == READ_DIGITAL(CTC_PM2_CommStatus,&nIOStatus))
			{
				//... Action Slit Valve Close
				printf("[ISOGATE] PM1 Slit Valve Open So Valve Close\n");
					if(SYS_SUCCESS != IsoGateControl(PM2, CLOSE)) {bRet = FALSE; break;}
			}			
		}

		//... Check PM3 Slit Valve Status
		if(READ_ACTION(PM3 , &nIOStatus) == OPEN)
		{
			//... Slit Valve Close Only At PM Maint,CTCDis,CTCDis2 
			if(eMaint == READ_DIGITAL(CTC_PM3_CommStatus,&nIOStatus)
				|| eCTCDis  == READ_DIGITAL(CTC_PM3_CommStatus,&nIOStatus)
				|| eCTCDis2  == READ_DIGITAL(CTC_PM3_CommStatus,&nIOStatus))
			{
				//... Action Slit Valve Close
				printf("[ISOGATE] PM3 Slit Valve Open So Valve Close\n");
					if(SYS_SUCCESS != IsoGateControl(PM3, CLOSE)) {bRet = FALSE; break;}
			}		
		}
		
	} while (0);

	return bRet;
}

//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	int	Station, CommStatus;
	int nTimeOut = 0;
	BOOL Switch = FALSE;
	char RunStr[41] , Temp[41] , StationStr[41] , Slot[21] , TempStr[41] , MsgStr[21];
	char szCmd[80];
	Module_Status msRet = SYS_SUCCESS;
	int Mode;

	//... Read Parameter
	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , Temp , 40 );
	STR_SEPERATE( Temp , StationStr , TempStr , 40 );
	STR_SEPERATE( TempStr , Slot , MsgStr , 20 );

	if      ( STRCMP_L( MsgStr , "SWITCH"   ) ) { Switch = TRUE; }
	else if ( STRCMP_L( MsgStr , "TRANSFER" ) ) {}

	//... Interlock For Slit Valve Cycle Test
	WRITE_DIGITAL(M_SVlv_StartStopDM,eON,&CommStatus);

	// 2005.12.12
	if ( SIMULATION_MODE ) 
	{
		_LOG("-----> %s %s Started in TM_Isolation_Valve_Control Func(Sim)", StationStr, RunStr);

		if (STRCMP_L( RunStr , "CLOSE" ) || STRCMP_L( RunStr , "CLOSE_EX_BM" )) {
			if(Switch) return SYS_SUCCESS;
		}

		// MARS 2014.07.25
		if      ( STRCMP_L( StationStr , "A_BM1" ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "B_BM1" ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "T_BM1" ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "BM1"   ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "A_BM2" ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "B_BM2" ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "T_BM2" ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "BM2"   ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "A_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "A_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "A_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "A_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "A_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "A_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "B_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "B_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "B_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "B_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "B_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "B_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "S_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "S_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "S_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "S_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "S_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "S_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "T_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "T_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "T_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "T_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "T_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "T_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "PM1"   ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "PM2"   ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "PM3"   ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "PM4"   ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "PM5"   ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "PM6"   ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "P1"    ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "P2"    ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "P3"    ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "P4"    ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "P5"    ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "P6"    ) ) Station = PM6;
		else {
			printf( "---> IsoGate Control Error - Not Support Station %s\n" , PROGRAM_PARAMETER_READ() );
			//... Interlock For Slit Valve Cycle Test
			WRITE_DIGITAL(M_SVlv_StartStopDM,eOFF,&CommStatus);
			msRet = SYS_ABORTED;
		}

		if(strcmp(RunStr, "OPEN") == 0) Mode = OPEN;
		else if(strcmp(RunStr, "CLOSE") == 0) Mode = CLOSE;
		
		if (READ_ACTION(Station , &CommStatus) == Mode)
		{
			//... Interlock For Slit Valve Cycle Test
			WRITE_DIGITAL(M_SVlv_StartStopDM,eOFF,&CommStatus);
			return SYS_SUCCESS;
		}

		LogEvent(StationStr, RunStr, "Start", "");		// 2013.11.15

		if(strlen(StationStr) > 3)		sprintf(szCmd,"ISO_GATE %s %s", RunStr, StationStr+2);
		else							sprintf(szCmd,"ISO_GATE %s %s", RunStr, StationStr);

		if(SYS_SUCCESS != g_DynIO.dRUN_FUNCTION(TMSIM_ISO_GATE, szCmd))		msRet = SYS_ABORTED;

		_LOG("-----> %s %s Finished in TM_Isolation_Valve_Control Func(Sim)", StationStr, RunStr);
		 LogEvent(StationStr, RunStr, "End", "");		// 2013.11.15

		return msRet;
	}

	//////////////////////////////////////////////////////////////////////////
	//... Check Slot Valve Control
	if(READ_DIGITAL(CONTROL, &CommStatus) == 1)
	{
		//... Interlock For Slit Valve Cycle Test
		WRITE_DIGITAL(M_SVlv_StartStopDM,eOFF,&CommStatus);
		return SYS_SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////////
	//... Started Real Function
	_LOG("-----> %s %s Started in TM_Isolation_Valve_Control Func", StationStr, RunStr);
	
	do {
		if      ( STRCMP_L( StationStr , "A_BM1" ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "B_BM1" ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "T_BM1" ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "BM1"   ) ) Station = BM1;
		else if ( STRCMP_L( StationStr , "A_BM2" ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "B_BM2" ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "T_BM2" ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "BM2"   ) ) Station = BM2;
		else if ( STRCMP_L( StationStr , "A_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "A_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "A_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "A_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "A_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "A_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "B_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "B_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "B_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "B_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "B_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "B_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "S_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "S_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "S_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "S_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "S_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "S_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "T_PM1" ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "T_PM2" ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "T_PM3" ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "T_PM4" ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "T_PM5" ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "T_PM6" ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "PM1"   ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "PM2"   ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "PM3"   ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "PM4"   ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "PM5"   ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "PM6"   ) ) Station = PM6;
		else if ( STRCMP_L( StationStr , "P1"    ) ) Station = PM1;
		else if ( STRCMP_L( StationStr , "P2"    ) ) Station = PM2;
		else if ( STRCMP_L( StationStr , "P3"    ) ) Station = PM3;
		else if ( STRCMP_L( StationStr , "P4"    ) ) Station = PM4;
		else if ( STRCMP_L( StationStr , "P5"    ) ) Station = PM5;
		else if ( STRCMP_L( StationStr , "P6"    ) ) Station = PM6;
		else if ( STRCMP_L( RunStr , "CYCLE_TEST") ) {}
		else {
			printf( "---> IsoGate Control Error - Not Support Station %s\n" , PROGRAM_PARAMETER_READ() );
			//... Interlock For Slit Valve Cycle Test
			WRITE_DIGITAL(M_SVlv_StartStopDM,eOFF,&CommStatus);
			msRet = SYS_ABORTED;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		//... 2016.02.16 
		//... When Host Execute RCMD (PM Disable) at Wafer Swap, Keeping Slit Valve Open Status
        //... So Slit Valve is Closed at this Status 
		if(TRUE != IsPMSlitValveClose(RunStr,Switch))
		{
			msRet = SYS_ABORTED;
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		//
		if(STRCMP_L(RunStr, "OPEN")) 
		{
			///////////////////////////////////////
			msRet = IsoGateControl(Station, OPEN);
			///////////////////////////////////////
			break;
		}
		else if(STRCMP_L(RunStr, "CLOSE"))
		{
			if(Switch)		msRet = SYS_SUCCESS;				
			else 
			{
				///////////////////////////////////////
				msRet = IsoGateControl(Station, CLOSE);
				///////////////////////////////////////
			}
			break;
		}
		else if(RunStr, "CYCLE_TEST")
		{
			//... Toggle For Slit Cycle Test (Swap Gas Skip)
			M_SVlv_CycTest = TRUE;
			///////////////////////////////////////
			msRet = CYCLE_TEST();
			///////////////////////////////////////
			M_SVlv_CycTest = FALSE;
		}
		else 
		{
			printf("[ABORT] Invalid Parameter[%s]\n", PROGRAM_PARAMETER_READ());
			msRet = SYS_ABORTED;
		}

	} while(0);

	//... Interlock For Slit Valve Cycle Test
	WRITE_DIGITAL(M_SVlv_StartStopDM,eOFF,&CommStatus);

	//... Save last Station
	g_nStation = Station;

	_LOG("-----> %s %s Finished in TM_Isolation_Valve_Control Func", StationStr, RunStr);
	
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	g_DynIO.InitIOList(g_pIOList, false);
	return TRUE;
}

//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {

	//--------------------------------------
	// 2005.10.07 for Checking System Exit
	if(no == -1)
	{
		// Deinitialize Function
		_gCLOSE_LOG();
	}
	//--------------------------------------
}

} // End of [extern "C"]
//==========================================================================================
