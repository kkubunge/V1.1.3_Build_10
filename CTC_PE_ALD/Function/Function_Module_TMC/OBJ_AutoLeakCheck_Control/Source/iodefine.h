#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
extern "C"
{
#include "k:\h\Kutlstr.h"
#include "k:\h\Kutltime.h"
#include "k:\h\kutletc.h"
#include "k:\h\kutlanal.h"
#include "k:\h\cimseqnc.h"
}

typedef enum {
	F_SCHEDULER			    ,
	LeakCheckSts			,
	LeakCheckAbortSts       ,
	MAIN_CONTROL            ,
	MAIN_CONTROL2           ,
	MAIN_CONTROL3			,

    PM1IdleHour             ,
    PM1IdleMin				,
	PM2IdleHour             ,
	PM2IdleMin				,
	PM3IdleHour             ,
	PM3IdleMin				,

	//Auto Leak Check Param
	AUTO_LEAKCHECK_USE		,
	START_TIME_HOUR			,
	START_TIME_MINUTE		,
	PUMP_TIME_HOUR			,
	PUMP_TIME_MINUTE		,
	PUMP_WAIT_HOUR			,
	PUMP_WAIT_MINUTE		,
	LEAK_CHECK_COUNT		,
	//ALARM_RATE				,
	//ALARM_AVG				,

	//2009.04.01
	START_MODE				,
	START_TIME_SDAY			,
	START_TIME_SWEEK		,
	//2011.12.09 Added by mgsong
	START_DAY_PERIOD ,
	ALC_LOGGING,
	LAST_ALC_YEAR,
	LAST_ALC_MONTH,
	LAST_ALC_DAY,

	//2012.03.22 Added by mgsong
	START_LOT_COUNT,
	START_WAFER_COUNT,
	START_IDLE_TIME,
	CM1_LOT_COUNT,
	CM2_LOT_COUNT,
	CM3_LOT_COUNT,
	CM1_WAFER_COUNT,
	CM2_WAFER_COUNT,
	CM3_WAFER_COUNT,
	ACC_ALC_WAFER,
	ACC_ALC_LOT,
	ACC_ALC_IDLETIME,

	//TM Leak Check IO
	TM_PUMP_TIME_HOUR		,
	TM_PUMP_TIME_MINUTE		,
	TM_PUMP_WAIT_HOUR		,
	TM_PUMP_WAIT_MINUTE		,
	TM_LEAK_CHECK_COUNT		,
	//BM1 Leak Check IO
	BM1_PUMP_TIME_HOUR		,
	BM1_PUMP_TIME_MINUTE	,
	BM1_PUMP_WAIT_HOUR		,
	BM1_PUMP_WAIT_MINUTE	,
	BM1_LEAK_CHECK_COUNT	,
	//BM2 Leak Check IO
	BM2_PUMP_TIME_HOUR		,
	BM2_PUMP_TIME_MINUTE	,
	BM2_PUMP_WAIT_HOUR		,
	BM2_PUMP_WAIT_MINUTE	,
	BM2_LEAK_CHECK_COUNT	,

	//Control Blocking
	TM_CONTROL				,
    BM1_CONTROL				,
	BM2_CONTROL				,
	//2011.12.19 mgsong : Over Pressure Valve Status Restore
	TM_OverPresVlv ,

	BM1_VAC_CONTROL         ,
	BM2_VAC_CONTROL         ,
	TM_VAC_CONTROL			,
	BM1_OVERPRESSURE        ,
	BM2_OVERPRESSURE        ,
	TM_OVERPRESSURE			,

	PRMD_OVER_PRES_OPT		,

	// 2012.10.29 Lot Pre Issue
	PM1_PRO_RunStsDM		,
	PM2_PRO_RunStsDM		,
	PM3_PRO_RunStsDM		,

	PM1_CommStatus,
	PM2_CommStatus,
	PM3_CommStatus,

	FA_STATUS_MDL_PM1,
	FA_STATUS_MDL_PM2,
	FA_STATUS_MDL_PM3,

	PM1_PRO_IdleRcpStsDM	,
	PM2_PRO_IdleRcpStsDM	,
	PM3_PRO_IdleRcpStsDM	,

	PM1_PRO_PurgeRcpStsDM	,
	PM2_PRO_PurgeRcpStsDM	,
	PM3_PRO_PurgeRcpStsDM	,

	PM1_PRO_ChamberStatus	,
	PM2_PRO_ChamberStatus	,
	PM3_PRO_ChamberStatus	,

	PM1_RECIPE_FILE,
	PM2_RECIPE_FILE,
	PM3_RECIPE_FILE,

	PM1_LC_RECIPE_FILE,
	PM2_LC_RECIPE_FILE,
	PM3_LC_RECIPE_FILE,
	
	SCHEDULER_MAINT_PM1,
	SCHEDULER_MAINT_PM2,
	SCHEDULER_MAINT_PM3,

	CHAMBER_CON_ABORT_PM1,
	CHAMBER_CON_ABORT_PM2,
	CHAMBER_CON_ABORT_PM3,

	SCHEDULER,

	PM1_WaferDI,
	PM2_WaferDI,
	PM3_WaferDI,

	GEM_Control_State,
	CTC_EVENT_LOG_SVR,

	CTC_FM_AL_Wafer_Status,	
	CTC_FA_Wafer_Status,	
	CTC_FB_Wafer_Status,	
	CTC_TA_Wafer_Status,	
	CTC_TA_Wafer_Status2,	
	CTC_TB_Wafer_Status,	
	CTC_TB_Wafer_Status2,	
	CTC_BM1_Wafer_Status,	
	CTC_BM1_Wafer_Status2,	
	CTC_BM1_Wafer_Status3,	
	CTC_BM1_Wafer_Status4,	
	CTC_BM2_Wafer_Status,	
	CTC_BM2_Wafer_Status2,	
	CTC_BM2_Wafer_Status3,	
	CTC_BM2_Wafer_Status4,	
	CTC_PM1_Wafer_Status,	
	CTC_PM1_Wafer_Status2,	
	CTC_PM2_Wafer_Status,	
	CTC_PM2_Wafer_Status2,	
	CTC_PM3_Wafer_Status,	
	CTC_PM3_Wafer_Status2,	

	PM1_PRO_RcpTypeDM,		//... 2015.01.06 Add Recipe Type Check
	PM2_PRO_RcpTypeDM,
	PM3_PRO_RcpTypeDM,
	
	//... 2016.01.19
	//... CTC I/O
	PRMD_PMLK_MACRO1,		
	PRMD_PMLK_MACRO2,		

	PRMD_PM_ALECHK_RCP,	
	PRMD_CHUCK_CYCNT,	
	PRMA_CHUCK_UPPOS,	
	PRMA_CHUCK_DNPOS,	
	PRMD_ALDVLV_CYCNT,		
	
	PRMD_PM1_ALECHK_NORM,	
	PRMD_PM2_ALECHK_NORM,	
	PRMD_PM3_ALECHK_NORM,	
	
	PRMD_PM1_ALECHK_GASLN,	
	PRMD_PM2_ALECHK_GASLN,	
	PRMD_PM3_ALECHK_GASLN,	
	
	PRMD_PM1_ALECHK_MTR,	
	PRMD_PM2_ALECHK_MTR,	
	PRMD_PM3_ALECHK_MTR,	
	
	PRMD_PM1_ALECHK_VLV,	
	PRMD_PM2_ALECHK_VLV,	
	PRMD_PM3_ALECHK_VLV,

	PRMA_PM1_BASEPRES,	
	PRMA_PM2_BASEPRES,	
	PRMA_PM3_BASEPRES,	

	PRMA_PM1_PumpTimeOT,	
	PRMA_PM2_PumpTimeOT,	
	PRMA_PM3_PumpTimeOT,	
	
	PRMA_PM1_WaitTimeOP,	
	PRMA_PM2_WaitTimeOP,	
	PRMA_PM3_WaitTimeOP,	
	
	PRMA_PM1_WaitTimeCL,	
	PRMA_PM2_WaitTimeCL,	
	PRMA_PM3_WaitTimeCL,	
	
	PRMA_PM1_LKCount,	
	PRMA_PM2_LKCount,	
	PRMA_PM3_LKCount,	
	
	PRMA_PM1_LKRateLimit,	
	PRMA_PM2_LKRateLimit,	
	PRMA_PM3_LKRateLimit,	
	
	PRMA_PM1_LnBASEPRES,	
	PRMA_PM2_LnBASEPRES,	
	PRMA_PM3_LnBASEPRES,	
	
	PRMA_PM1_LnPumpTimeOT,	
	PRMA_PM2_LnPumpTimeOT,	
	PRMA_PM3_LnPumpTimeOT,	
	
	PRMA_PM1_LnWaitTimeOP,	
	PRMA_PM2_LnWaitTimeOP,	
	PRMA_PM3_LnWaitTimeOP,	
	
	PRMA_PM1_LnWaitTimeCL,	
	PRMA_PM2_LnWaitTimeCL,	
	PRMA_PM3_LnWaitTimeCL,	
	
	PRMA_PM1_LnLKCount,	
	PRMA_PM2_LnLKCount,	
	PRMA_PM3_LnLKCount,	
	
	PRMA_PM1_LnLKRateLimit,	
	PRMA_PM2_LnLKRateLimit,	
	PRMA_PM3_LnLKRateLimit,
	//... 2016.05.13
	PRMA_PM1_MFC_PurgeTmAM,
	PRMA_PM2_MFC_PurgeTmAM,
	PRMA_PM3_MFC_PurgeTmAM,
	//... 2016.08.16
	PRMA_PM1_RoRLimitAM,
	PRMA_PM2_RoRLimitAM,
	PRMA_PM3_RoRLimitAM,
	PRMA_PM1_LnRoRLimitAM,
	PRMA_PM2_LnRoRLimitAM,
	PRMA_PM3_LnRoRLimitAM,
	
	//... PM I/O
	PM1_LK_MACRO1,
	PM2_LK_MACRO1,
	PM3_LK_MACRO1,
	PM1_LK_MACRO2,
	PM2_LK_MACRO2,
	PM3_LK_MACRO2,

	PM1_ALECHK_RCPDM,	
	PM2_ALECHK_RCPDM,	
	PM3_ALECHK_RCPDM,	
	
	PM1_ALECHK_DynCntDM,		
	PM2_ALECHK_DynCntDM,	
	PM3_ALECHK_DynCntDM,	
	
	PM1_CHUCK_UPPOS_AM,			
	PM2_CHUCK_UPPOS_AM,		
	PM3_CHUCK_UPPOS_AM,		
	
	PM1_CHUCK_DNPOS_AM,	
	PM2_CHUCK_DNPOS_AM,	
	PM3_CHUCK_DNPOS_AM,	
	
	PM1_ALDVLV_CntAM,	
	PM2_ALDVLV_CntAM,	
	PM3_ALDVLV_CntAM,	
	
	PM1_ALECHK_NormDM,	
	PM2_ALECHK_NormDM,	
	PM3_ALECHK_NormDM,	
	
	PM1_ALECHK_GASLnDM,	
	PM2_ALECHK_GASLnDM,	
	PM3_ALECHK_GASLnDM,	
	
	PM1_ALECHK_MTRDM,		
	PM2_ALECHK_MTRDM,	
	PM3_ALECHK_MTRDM,	
	
	PM1_ALECHK_VLVDM,		
	PM2_ALECHK_VLVDM,	
	PM3_ALECHK_VLVDM,	
	                                                      
	PM1_LeakChkBasePressAM,	
	PM2_LeakChkBasePressAM,  	
	PM3_LeakChkBasePressAM,  	
	
	PM1_LeakChkPumpTimeOutAM,	
	PM2_LeakChkPumpTimeOutAM,	
	PM3_LeakChkPumpTimeOutAM,	
	
	PM1_BaseDelayTimeAM,	
	PM2_BaseDelayTimeAM,	
	PM3_BaseDelayTimeAM,	
	
	PM1_LeakChkDelayTimeAM,	
	PM2_LeakChkDelayTimeAM,	
	PM3_LeakChkDelayTimeAM,	
	
	PM1_LeakChkCountAM,	
	PM2_LeakChkCountAM,	
	PM3_LeakChkCountAM,	
	
	PM1_LeakChkLimitPressAM,	
	PM2_LeakChkLimitPressAM,	
	PM3_LeakChkLimitPressAM,	
	
	PM1_LnLkChkBasePressAM,	
	PM2_LnLkChkBasePressAM,	
	PM3_LnLkChkBasePressAM,	
	
	PM1_LnLkChkPumpTimeOutAM,	
	PM2_LnLkChkPumpTimeOutAM,	
	PM3_LnLkChkPumpTimeOutAM,	
	
	PM1_LnBaseDelayTimeAM,	
	PM2_LnBaseDelayTimeAM,	
	PM3_LnBaseDelayTimeAM,	
	
	PM1_LnLkChkDelayTimeAM,	
	PM2_LnLkChkDelayTimeAM,	
	PM3_LnLkChkDelayTimeAM,	
	
	PM1_LnLkChkCountAM,	
	PM2_LnLkChkCountAM,	
	PM3_LnLkChkCountAM,	
	
	PM1_LnLkChkLimitPressAM,	
	PM2_LnLkChkLimitPressAM,	
	PM3_LnLkChkLimitPressAM,
	//... 2016.05.13
	PM1_MFC_PurgeTmAM,
	PM2_MFC_PurgeTmAM,
	PM3_MFC_PurgeTmAM,
	//... 2016.08.16
	PM1_RoRLimitAM,
	PM2_RoRLimitAM,
	PM3_RoRLimitAM,
	PM1_LnRoRLimitAM,
	PM2_LnRoRLimitAM,
	PM3_LnRoRLimitAM,
	//...
	PM1_PurgeLineLK_DM,	
	PM2_PurgeLineLK_DM,		
	PM3_PurgeLineLK_DM,		
	PM1_CleanLineLK_DM,	
	PM2_CleanLineLK_DM,		
	PM3_CleanLineLK_DM,	

	PRMA_PM1_DRYCThick,	
	PRMA_PM2_DRYCThick,	
	PRMA_PM3_DRYCThick,	
	
	PRMD_PM1_AutoCycRun,	
	PRMD_PM2_AutoCycRun,	
	PRMD_PM3_AutoCycRun,	
	
	PM1_CLN_RECIPE_FILE,	
	PM2_CLN_RECIPE_FILE,	
	PM3_CLN_RECIPE_FILE,	
	
	PM1_SysLeakChkDM,
	PM2_SysLeakChkDM,
	PM3_SysLeakChkDM,
	//...
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
