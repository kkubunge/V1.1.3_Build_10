#include "StdAfx.h"
#include "LeakCheckControl.h"
#include "cimseqnc.h"

char APP_NAME[] = "AutoLeakCheck";
CLeakCheckControl*     g_pLeakCheckCtrl = NULL;

#define		ALARM_AUTOLEAKCHECK_ABORT	6156

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SCHEDULER"				, _K_F_IO	,	F_SCHEDULER			,	0	} ,
	{ "CTC.AutoLeakCheckSts"    , _K_D_IO	,	LeakCheckSts		,	0	} ,
	{ "CTC.AutoLeakAbortCtrl"   , _K_D_IO	,	LeakCheckAbortSts	,	0	} ,
	
	{ "CTC.MAIN_CONTROL"		, _K_D_IO	,	MAIN_CONTROL		,	0	} ,
	{ "CTC.MAIN_CONTROL2"		, _K_D_IO	,	MAIN_CONTROL2		,	0	} ,
	{ "CTC.MAIN_CONTROL3"		, _K_D_IO	,	MAIN_CONTROL3		,	0	} ,
	
	//Idle Run Time : Idle Wait Time Change & Restore
	{ "PRMD_PM1_Comp_TimeH_03"	, _K_D_IO	,	PM1IdleHour			,	0	} ,
	{ "PRMD_PM1_Comp_TimeM_03"	, _K_D_IO	,	PM1IdleMin			,	0	} ,
	{ "PRMD_PM2_Comp_TimeH_03"	, _K_D_IO	,	PM2IdleHour			,	0	} ,
	{ "PRMD_PM2_Comp_TimeM_03"	, _K_D_IO	,	PM2IdleMin			,	0	} ,
	{ "PRMD_PM3_Comp_TimeH_03"	, _K_D_IO	,	PM3IdleHour			,	0	} ,
	{ "PRMD_PM3_Comp_TimeM_03"	, _K_D_IO	,	PM3IdleMin			,	0	} ,

	//Auto Leak Check Associated Paramater IO
	{ "PRMD_ALECHK_CONTROL"		, _K_D_IO	,	AUTO_LEAKCHECK_USE	,	0	} ,
	{ "PRMD_ALECHK_SHOUR"		, _K_D_IO	,	START_TIME_HOUR		,	0	} ,
	{ "PRMD_ALECHK_SMIN"		, _K_D_IO	,	START_TIME_MINUTE	,	0	} ,
	{ "PRMD_ALECHK_PUMP_HOUR"	, _K_D_IO	,	PUMP_TIME_HOUR		,	0	} ,
	{ "PRMD_ALECHK_PUMP_MIN"	, _K_D_IO	,	PUMP_TIME_MINUTE	,	0	} ,
	{ "PRMD_ALECHK_PUMPW_HOUR"	, _K_D_IO	,	PUMP_WAIT_HOUR		,	0	} ,
	{ "PRMD_ALECHK_PUMPW_MIN"	, _K_D_IO	,	PUMP_WAIT_MINUTE	,	0	} ,
	{ "PRMD_ALECHK_COUNT"		, _K_D_IO	,	LEAK_CHECK_COUNT	,	0	} ,
//	{ "PRMD_ALECHK_RATE_LIMIT"	, _K_D_IO	,	ALARM_RATE		,	0	} ,
//	{ "PRMD_ALECHK_AVG_LIMIT"	, _K_D_IO	,	ALARM_AVG			,	0	} ,

	//2009.04.01
	{ "PRMD_ALECHK_MODE"		, _K_D_IO	,	START_MODE			,	0	} ,
	{ "PRMD_ALECHK_SDAY"		, _K_D_IO	,	START_TIME_SDAY		,	0	} ,
	{ "PRMD_ALECHK_SWEEK"		, _K_D_IO	,	START_TIME_SWEEK	,	0	} ,
	//2011.12.09 Added by mgsong
	{ "PRMD_ALECHK_PERIOD"		, _K_D_IO	,	START_DAY_PERIOD	,	0	} ,
	{ "vAutoLeak.Logging"       , _K_D_IO   ,   ALC_LOGGING			,	0	} ,
	{ "ALeakCheck.LastYear"		, _K_D_IO	,	LAST_ALC_YEAR		,	0	} ,
	{ "ALeakCheck.LastMonth"	, _K_D_IO	,	LAST_ALC_MONTH		,	0	} ,
	{ "ALeakCheck.LastDay"		, _K_D_IO	,	LAST_ALC_DAY		,	0	} ,
	//2012.03.22 Added by mgsong : Wafer Count Base, Lot Count Base¥ìi
	{ "PRMD_ALECHK_LOTCOUNT"	, _K_D_IO	,	START_LOT_COUNT		,	0	} ,
	{ "PRMD_ALECHK_WFRCOUNT"	, _K_D_IO	,	START_WAFER_COUNT	,	0	} ,
	{ "PRMD_ALECHK_IDLETIME"	, _K_D_IO	,	START_IDLE_TIME		,	0	} ,
	{ "CTC.CM1_CASS_COUNT"		, _K_D_IO	,	CM1_LOT_COUNT		,	0	} ,
	{ "CTC.CM2_CASS_COUNT"		, _K_D_IO	,	CM2_LOT_COUNT		,	0	} ,
	{ "CTC.CM3_CASS_COUNT"		, _K_D_IO	,	CM3_LOT_COUNT		,	0	} ,
	{ "CTC.CM1_PLACE_COUNT"		, _K_D_IO	,	CM1_WAFER_COUNT		,	0	} ,
	{ "CTC.CM2_PLACE_COUNT"		, _K_D_IO	,	CM2_WAFER_COUNT		,	0	} ,
	{ "CTC.CM3_PLACE_COUNT"		, _K_D_IO	,	CM3_WAFER_COUNT		,	0	} ,
	{ "ALeakCheck.AccWfrCnt"	, _K_D_IO	,	ACC_ALC_WAFER		,	0	} ,
	{ "ALeakCheck.AccLotCnt"	, _K_D_IO	,	ACC_ALC_LOT			,	0	} ,
	{ "ALeakCheck.AccIdleTime"	, _K_D_IO	,	ACC_ALC_IDLETIME	,	0	} ,
		

	//TM Leak Check IO which is need to setting before start leak check
	{ "TM.Leak_PumpingTime_H"	, _K_D_IO	,	TM_PUMP_TIME_HOUR	,	0	} ,
	{ "TM.Leak_PumpingTime_M"	, _K_D_IO	,	TM_PUMP_TIME_MINUTE	,	0	} ,
	{ "TM.Leak_WaitingTime_H"	, _K_D_IO	,	TM_PUMP_WAIT_HOUR	,	0	} ,
	{ "TM.Leak_WaitingTime_M"	, _K_D_IO	,	TM_PUMP_WAIT_MINUTE	,	0	} ,
	{ "TM.Leak_Test_Number"		, _K_D_IO	,	TM_LEAK_CHECK_COUNT	,	0	} ,
	//BM1 Leak Check IO which is need to setting before start leak check
	{ "BM1.Leak_PumpingTime_H"	, _K_D_IO	,	BM1_PUMP_TIME_HOUR	,	0	} ,
	{ "BM1.Leak_PumpingTime_M"	, _K_D_IO	,	BM1_PUMP_TIME_MINUTE,	0	} ,
	{ "BM1.Leak_WaitingTime_H"	, _K_D_IO	,	BM1_PUMP_WAIT_HOUR	,	0	} ,
	{ "BM1.Leak_WaitingTime_M"	, _K_D_IO	,	BM1_PUMP_WAIT_MINUTE,	0	} ,
	{ "BM1.Leak_Test_Number"	, _K_D_IO	,	BM1_LEAK_CHECK_COUNT,	0	} ,
	//BM2 Leak Check IO which is need to setting before start leak check
	{ "BM2.Leak_PumpingTime_H"	, _K_D_IO	,	BM2_PUMP_TIME_HOUR	,	0	} ,
	{ "BM2.Leak_PumpingTime_M"	, _K_D_IO	,	BM2_PUMP_TIME_MINUTE,	0	} ,
	{ "BM2.Leak_WaitingTime_H"	, _K_D_IO	,	BM2_PUMP_WAIT_HOUR	,	0	} ,
	{ "BM2.Leak_WaitingTime_M"	, _K_D_IO	,	BM2_PUMP_WAIT_MINUTE,	0	} ,
	{ "BM2.Leak_Test_Number"	, _K_D_IO	,	BM2_LEAK_CHECK_COUNT,	0	} ,

	//Control blocking
	{ "CTC.TM_M_CONTROL"		, _K_D_IO	,	TM_CONTROL			,	0	} ,
	{ "CTC.BM1M_CONTROL"		, _K_D_IO	,	BM1_CONTROL			,	0	} ,
	{ "CTC.BM2M_CONTROL"		, _K_D_IO	,	BM2_CONTROL			,	0	} ,

	//2011.12.19 mgsong : Over Pressure Valve Status Restore
	{ "TM.OverPresVvDO"			, _K_D_IO	,	TM_OverPresVlv		,	0	} ,
	
	//Function
	{ "TMC_BM1_VAC_CONTROL"		, _K_F_IO	,	BM1_VAC_CONTROL		,	0	} ,
	{ "TMC_BM2_VAC_CONTROL"		, _K_F_IO	,	BM2_VAC_CONTROL		,	0	} ,
	{ "TMC_TM_VAC_CONTROL"		, _K_F_IO	,	TM_VAC_CONTROL		,	0	} ,
	{ "TMC_BM1_OVERPRESSURE"	, _K_F_IO	,	BM1_OVERPRESSURE	,	0	} ,
	{ "TMC_BM2_OVERPRESSURE"	, _K_F_IO	,	BM2_OVERPRESSURE	,	0	} ,
	{ "TMC_TM_OVERPRESSURE"		, _K_F_IO	,	TM_OVERPRESSURE		,	0	} ,

	// 2012.09.14 TM Over Pressure Valve Option
	{ "PRMD_OVER_PRES_OPT"		, _K_D_IO	,	PRMD_OVER_PRES_OPT	,	0	} ,

	// 2012.10.29 Lot Pre Issue

	{ "PM1.PRO_RunStsDM"		, _K_D_IO	,	PM1_PRO_RunStsDM,	0	} ,
	{ "PM2.PRO_RunStsDM"		, _K_D_IO	,	PM2_PRO_RunStsDM,	0	} ,
	{ "PM3.PRO_RunStsDM"		, _K_D_IO	,	PM3_PRO_RunStsDM,	0	} ,

	{ "CTC.PM1_CommStatus"		, _K_D_IO	,	PM1_CommStatus,		0	} ,
	{ "CTC.PM2_CommStatus"		, _K_D_IO	,	PM2_CommStatus,		0	} ,
	{ "CTC.PM3_CommStatus"		, _K_D_IO	,	PM3_CommStatus,		0	} ,

	{ "CTC.FA_STATUS_MDL_PM1"	, _K_D_IO	,	FA_STATUS_MDL_PM1,	0	} ,
	{ "CTC.FA_STATUS_MDL_PM2"	, _K_D_IO	,	FA_STATUS_MDL_PM2,	0	} ,
	{ "CTC.FA_STATUS_MDL_PM3"	, _K_D_IO	,	FA_STATUS_MDL_PM3,	0	} ,

	{ "PM1.PRO_IdleRcpStsDM"	, _K_D_IO	,	PM1_PRO_IdleRcpStsDM,	0	} ,
	{ "PM2.PRO_IdleRcpStsDM"	, _K_D_IO	,	PM2_PRO_IdleRcpStsDM,	0	} ,
	{ "PM3.PRO_IdleRcpStsDM"	, _K_D_IO	,	PM3_PRO_IdleRcpStsDM,	0	} ,

	{ "PM1.PRO_PurgeRcpStsDM"	, _K_D_IO	,	PM1_PRO_PurgeRcpStsDM,	0	} ,
	{ "PM2.PRO_PurgeRcpStsDM"	, _K_D_IO	,	PM2_PRO_PurgeRcpStsDM,	0	} ,
	{ "PM3.PRO_PurgeRcpStsDM"	, _K_D_IO	,	PM3_PRO_PurgeRcpStsDM,	0	} ,

	{ "PM1.PRO_ChamberStatus"	, _K_D_IO	,	PM1_PRO_ChamberStatus,	0	} ,
	{ "PM2.PRO_ChamberStatus"	, _K_D_IO	,	PM2_PRO_ChamberStatus,	0	} ,
	{ "PM3.PRO_ChamberStatus"	, _K_D_IO	,	PM3_PRO_ChamberStatus,	0	} ,

	{ "CTC.PM1_RECIPE_FILE"		, _K_S_IO   ,	PM1_RECIPE_FILE		 ,	0	} ,	
	{ "CTC.PM2_RECIPE_FILE"		, _K_S_IO   ,	PM2_RECIPE_FILE		 ,	0	} ,	
	{ "CTC.PM3_RECIPE_FILE"		, _K_S_IO   ,	PM3_RECIPE_FILE		 ,	0	} ,	

	{ "CTC.PM1_LC_RECIPE_FILE"	, _K_S_IO   ,	PM1_LC_RECIPE_FILE	 ,	0	} ,	
	{ "CTC.PM2_LC_RECIPE_FILE"	, _K_S_IO   ,	PM2_LC_RECIPE_FILE	 ,	0	} ,	
	{ "CTC.PM3_LC_RECIPE_FILE"	, _K_S_IO   ,	PM3_LC_RECIPE_FILE	 ,	0	} ,	

	{ "SCHEDULER_MAINT_PM1"		, _K_F_IO  ,	SCHEDULER_MAINT_PM1  ,	0	} ,
	{ "SCHEDULER_MAINT_PM2"		, _K_F_IO  ,	SCHEDULER_MAINT_PM2  ,	0	} ,
	{ "SCHEDULER_MAINT_PM3"		, _K_F_IO  ,	SCHEDULER_MAINT_PM3  ,	0	} ,

	{ "CHAMBER_CON_ABORT_PM1"	, _K_F_IO  ,	CHAMBER_CON_ABORT_PM1,	0	} ,
	{ "CHAMBER_CON_ABORT_PM2"	, _K_F_IO  ,	CHAMBER_CON_ABORT_PM2,	0	} ,
	{ "CHAMBER_CON_ABORT_PM3"	, _K_F_IO  ,	CHAMBER_CON_ABORT_PM3,	0	} ,

	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER			,	0	} ,

	{ "PM1.WaferDI"				, _K_D_IO	,	PM1_WaferDI			,	0	} ,
	{ "PM2.WaferDI"				, _K_D_IO	,	PM2_WaferDI			,	0	} ,	
	{ "PM3.WaferDI"				, _K_D_IO	,	PM3_WaferDI			,	0	} ,

	{ "GEM.Control_State"		, _K_D_IO	,	GEM_Control_State	,	0	} ,
	{ "CTC_EVENT_LOG_SVR"		, _K_F_IO	,	CTC_EVENT_LOG_SVR	,	0	} ,	// 2014.07.20

	//2014.11.20 Wafer Status Check
	{ "CTC.FM_AL_Wafer_Status"	, _K_D_IO	,	CTC_FM_AL_Wafer_Status,	0	} ,	
	{ "CTC.FA_Wafer_Status"		, _K_D_IO	,	CTC_FA_Wafer_Status,	0	} ,	
	{ "CTC.FB_Wafer_Status"		, _K_D_IO	,	CTC_FB_Wafer_Status,	0	} ,	
	{ "CTC.TA_Wafer_Status"		, _K_D_IO	,	CTC_TA_Wafer_Status,	0	} ,	
	{ "CTC.TA_Wafer_Status2"	, _K_D_IO	,	CTC_TA_Wafer_Status2,	0	} ,	
	{ "CTC.TB_Wafer_Status"		, _K_D_IO	,	CTC_TB_Wafer_Status,	0	} ,	
	{ "CTC.TB_Wafer_Status2"	, _K_D_IO	,	CTC_TB_Wafer_Status2,	0	} ,	
	{ "CTC.BM1_Wafer_Status"	, _K_D_IO	,	CTC_BM1_Wafer_Status,	0	} ,	
	{ "CTC.BM1_Wafer_Status2"	, _K_D_IO	,	CTC_BM1_Wafer_Status2,	0	} ,	
	{ "CTC.BM1_Wafer_Status3"	, _K_D_IO	,	CTC_BM1_Wafer_Status3,	0	} ,	
	{ "CTC.BM1_Wafer_Status4"	, _K_D_IO	,	CTC_BM1_Wafer_Status4,	0	} ,	
	{ "CTC.BM2_Wafer_Status"	, _K_D_IO	,	CTC_BM2_Wafer_Status,	0	} ,	
	{ "CTC.BM2_Wafer_Status2"	, _K_D_IO	,	CTC_BM2_Wafer_Status2,	0	} ,	
	{ "CTC.BM2_Wafer_Status3"	, _K_D_IO	,	CTC_BM2_Wafer_Status3,	0	} ,	
	{ "CTC.BM2_Wafer_Status4"	, _K_D_IO	,	CTC_BM2_Wafer_Status4,	0	} ,
	{ "CTC.PM1_Wafer_Status"	, _K_D_IO	,	CTC_PM1_Wafer_Status,	0	} ,	
	{ "CTC.PM1_Wafer_Status2"	, _K_D_IO	,	CTC_PM1_Wafer_Status2,	0	} ,	
	{ "CTC.PM2_Wafer_Status"	, _K_D_IO	,	CTC_PM2_Wafer_Status,	0	} ,	
	{ "CTC.PM2_Wafer_Status2"	, _K_D_IO	,	CTC_PM2_Wafer_Status2,	0	} ,	
	{ "CTC.PM3_Wafer_Status"	, _K_D_IO	,	CTC_PM3_Wafer_Status,	0	} ,	
	{ "CTC.PM3_Wafer_Status2"	, _K_D_IO	,	CTC_PM3_Wafer_Status2,	0	} ,	

	//... 2015.01.06 Add Recipe Type Check
	{ "PM1.PRO_RcpTypeDM"		, _K_D_IO	,	PM1_PRO_RcpTypeDM,		0	} ,	
	{ "PM2.PRO_RcpTypeDM"		, _K_D_IO	,	PM2_PRO_RcpTypeDM,		0	} ,	
	{ "PM3.PRO_RcpTypeDM"		, _K_D_IO	,	PM3_PRO_RcpTypeDM,		0	} ,
	
	//... 2016.01.19
	//... CTC Parameter
	{ "PRMD_PMLK_MACRO1"		, _K_D_IO	,	PRMD_PMLK_MACRO1,		0	} ,	
	{ "PRMD_PMLK_MACRO2"		, _K_D_IO	,	PRMD_PMLK_MACRO2,		0	} ,	

	{ "PRMD_PM_ALECHK_RCP"		, _K_D_IO	,	PRMD_PM_ALECHK_RCP,		0	} ,	
	{ "PRMD_CHUCK_CYCNT"		, _K_D_IO	,	PRMD_CHUCK_CYCNT,		0	} ,
	{ "PRMA_CHUCK_UPPOS"		, _K_A_IO	,	PRMA_CHUCK_UPPOS,		0	} ,
	{ "PRMA_CHUCK_DNPOS"		, _K_A_IO	,	PRMA_CHUCK_DNPOS,		0	} ,	
	{ "PRMD_ALDVLV_CYCNT"		, _K_D_IO	,	PRMD_ALDVLV_CYCNT,		0	} ,		
	
	{ "PRMD_PM1_ALECHK_NORM"	, _K_D_IO	,	PRMD_PM1_ALECHK_NORM,	0	} ,	
	{ "PRMD_PM2_ALECHK_NORM"	, _K_D_IO	,	PRMD_PM2_ALECHK_NORM,	0	} ,	
	{ "PRMD_PM3_ALECHK_NORM"	, _K_D_IO	,	PRMD_PM3_ALECHK_NORM,	0	} ,	
	
	{ "PRMD_PM1_ALECHK_GASLN"	, _K_D_IO	,	PRMD_PM1_ALECHK_GASLN,	0	} ,	
	{ "PRMD_PM2_ALECHK_GASLN"	, _K_D_IO	,	PRMD_PM2_ALECHK_GASLN,	0	} ,	
	{ "PRMD_PM3_ALECHK_GASLN"	, _K_D_IO	,	PRMD_PM3_ALECHK_GASLN,	0	} ,	
	
	{ "PRMD_PM1_ALECHK_MTR"		, _K_D_IO	,	PRMD_PM1_ALECHK_MTR,	0	} ,		
	{ "PRMD_PM2_ALECHK_MTR"		, _K_D_IO	,	PRMD_PM2_ALECHK_MTR,	0	} ,	
	{ "PRMD_PM3_ALECHK_MTR"		, _K_D_IO	,	PRMD_PM3_ALECHK_MTR,	0	} ,	
	
	{ "PRMD_PM1_ALECHK_VLV"		, _K_D_IO	,	PRMD_PM1_ALECHK_VLV,	0	} ,		
	{ "PRMD_PM2_ALECHK_VLV"		, _K_D_IO	,	PRMD_PM2_ALECHK_VLV,	0	} ,	
	{ "PRMD_PM3_ALECHK_VLV"		, _K_D_IO	,	PRMD_PM3_ALECHK_VLV,	0	} ,
	//... 2016.04.11	
	{ "PRMA_PM1_BASEPRES"		, _K_A_IO	,	PRMA_PM1_BASEPRES		,	0	},		
	{ "PRMA_PM2_BASEPRES"		, _K_A_IO	,	PRMA_PM2_BASEPRES		,	0	},
	{ "PRMA_PM3_BASEPRES"		, _K_A_IO	,	PRMA_PM3_BASEPRES		,	0	},
	
	{ "PRMA_PM1_PumpTimeOT"		, _K_A_IO	,	PRMA_PM1_PumpTimeOT		,	0	},
	{ "PRMA_PM2_PumpTimeOT"		, _K_A_IO	,	PRMA_PM2_PumpTimeOT		,	0	},
	{ "PRMA_PM3_PumpTimeOT"		, _K_A_IO	,	PRMA_PM3_PumpTimeOT		,	0	},
	
	{ "PRMA_PM1_WaitTimeOP"		, _K_A_IO	,	PRMA_PM1_WaitTimeOP		,	0	},
	{ "PRMA_PM2_WaitTimeOP"		, _K_A_IO	,	PRMA_PM2_WaitTimeOP		,	0	},
	{ "PRMA_PM3_WaitTimeOP"		, _K_A_IO	,	PRMA_PM3_WaitTimeOP		,	0	},
	
	{ "PRMA_PM1_WaitTimeCL"		, _K_A_IO	,	PRMA_PM1_WaitTimeCL		,	0	},
	{ "PRMA_PM2_WaitTimeCL"		, _K_A_IO	,	PRMA_PM2_WaitTimeCL		,	0	},
	{ "PRMA_PM3_WaitTimeCL"		, _K_A_IO	,	PRMA_PM3_WaitTimeCL		,	0	},
	
	{ "PRMA_PM1_LKCount"		, _K_A_IO	,	PRMA_PM1_LKCount		,	0	},
	{ "PRMA_PM2_LKCount"		, _K_A_IO	,	PRMA_PM2_LKCount		,	0	},
	{ "PRMA_PM3_LKCount"		, _K_A_IO	,	PRMA_PM3_LKCount		,	0	},
	
	{ "PRMA_PM1_LKRateLimit"	, _K_A_IO	,	PRMA_PM1_LKRateLimit	,	0	},	
	{ "PRMA_PM2_LKRateLimit"	, _K_A_IO	,	PRMA_PM2_LKRateLimit	,	0	},	
	{ "PRMA_PM3_LKRateLimit"	, _K_A_IO	,	PRMA_PM3_LKRateLimit	,	0	},	
	
	{ "PRMA_PM1_LnBASEPRES"		, _K_A_IO	,	PRMA_PM1_LnBASEPRES		,	0	},
	{ "PRMA_PM2_LnBASEPRES"		, _K_A_IO	,	PRMA_PM2_LnBASEPRES		,	0	},
	{ "PRMA_PM3_LnBASEPRES"		, _K_A_IO	,	PRMA_PM3_LnBASEPRES		,	0	},
	
	{ "PRMA_PM1_LnPumpTimeOT"	, _K_A_IO	,	PRMA_PM1_LnPumpTimeOT	,	0	},	
	{ "PRMA_PM2_LnPumpTimeOT"	, _K_A_IO	,	PRMA_PM2_LnPumpTimeOT	,	0	},	
	{ "PRMA_PM3_LnPumpTimeOT"	, _K_A_IO	,	PRMA_PM3_LnPumpTimeOT	,	0	},	
	
	{ "PRMA_PM1_LnWaitTimeOP"	, _K_A_IO	,	PRMA_PM1_LnWaitTimeOP	,	0	},	
	{ "PRMA_PM2_LnWaitTimeOP"	, _K_A_IO	,	PRMA_PM2_LnWaitTimeOP	,	0	},	
	{ "PRMA_PM3_LnWaitTimeOP"	, _K_A_IO	,	PRMA_PM3_LnWaitTimeOP	,	0	},	
	
	{ "PRMA_PM1_LnWaitTimeCL"	, _K_A_IO	,	PRMA_PM1_LnWaitTimeCL	,	0	},	
	{ "PRMA_PM2_LnWaitTimeCL"	, _K_A_IO	,	PRMA_PM2_LnWaitTimeCL	,	0	},	
	{ "PRMA_PM3_LnWaitTimeCL"	, _K_A_IO	,	PRMA_PM3_LnWaitTimeCL	,	0	},	
	
	{ "PRMA_PM1_LnLKCount"		, _K_A_IO	,	PRMA_PM1_LnLKCount		,	0	},
	{ "PRMA_PM2_LnLKCount"		, _K_A_IO	,	PRMA_PM2_LnLKCount		,	0	},
	{ "PRMA_PM3_LnLKCount"		, _K_A_IO	,	PRMA_PM3_LnLKCount		,	0	},
	
	{ "PRMA_PM1_LnLKRateLimit"	, _K_A_IO	,	PRMA_PM1_LnLKRateLimit	,	0	},	
	{ "PRMA_PM2_LnLKRateLimit"	, _K_A_IO	,	PRMA_PM2_LnLKRateLimit	,	0	},	
	{ "PRMA_PM3_LnLKRateLimit"	, _K_A_IO	,	PRMA_PM3_LnLKRateLimit	,	0	},
	//... 2016.05.13	
	{ "PRMA_PM1_MFC_PurgeTmAM"	, _K_A_IO	,	PRMA_PM1_MFC_PurgeTmAM	,	0	},
	{ "PRMA_PM2_MFC_PurgeTmAM"	, _K_A_IO	,	PRMA_PM2_MFC_PurgeTmAM	,	0	},
	{ "PRMA_PM3_MFC_PurgeTmAM"	, _K_A_IO	,	PRMA_PM3_MFC_PurgeTmAM	,	0	},
	//... 2016.08.16
	{ "PRMA_PM1_RoRLimitAM"		, _K_A_IO	,	PRMA_PM1_RoRLimitAM		,	0	},
	{ "PRMA_PM2_RoRLimitAM"		, _K_A_IO	,	PRMA_PM2_RoRLimitAM		,	0	},
	{ "PRMA_PM3_RoRLimitAM"		, _K_A_IO	,	PRMA_PM3_RoRLimitAM		,	0	},
	{ "PRMA_PM1_LnRoRLimitAM"	, _K_A_IO	,	PRMA_PM1_LnRoRLimitAM	,	0	},
	{ "PRMA_PM2_LnRoRLimitAM"	, _K_A_IO	,	PRMA_PM2_LnRoRLimitAM	,	0	},
	{ "PRMA_PM3_LnRoRLimitAM"	, _K_A_IO	,	PRMA_PM3_LnRoRLimitAM	,	0	},
	//...
	//... PM IO
	{ "PM1.LKMacroType1DM"		, _K_D_IO	,	PM1_LK_MACRO1,			0	} ,	
	{ "PM2.LKMacroType1DM"		, _K_D_IO	,	PM2_LK_MACRO1,			0	} ,	
	{ "PM3.LKMacroType1DM"		, _K_D_IO	,	PM3_LK_MACRO1,			0	} ,	
	{ "PM1.LKMacroType2DM"		, _K_D_IO	,	PM1_LK_MACRO2,			0	} ,	
	{ "PM2.LKMacroType2DM"		, _K_D_IO	,	PM2_LK_MACRO2,			0	} ,	
	{ "PM3.LKMacroType2DM"		, _K_D_IO	,	PM3_LK_MACRO2,			0	} ,	

	{ "PM1.ALECHK_RCPDM"		, _K_D_IO	,	PM1_ALECHK_RCPDM,		0	} ,	
	{ "PM2.ALECHK_RCPDM"		, _K_D_IO	,	PM2_ALECHK_RCPDM,		0	} ,	
	{ "PM3.ALECHK_RCPDM"		, _K_D_IO	,	PM3_ALECHK_RCPDM,		0	} ,	

	{ "PM1.ALECHK_DynCntDM"		, _K_D_IO	,	PM1_ALECHK_DynCntDM,	0	} ,			
	{ "PM2.ALECHK_DynCntDM"		, _K_D_IO	,	PM2_ALECHK_DynCntDM,	0	} ,		
	{ "PM3.ALECHK_DynCntDM"		, _K_D_IO	,	PM3_ALECHK_DynCntDM,	0	} ,		
	
	{ "PM1.Chuck_UpPos_AM"		, _K_A_IO	,	PM1_CHUCK_UPPOS_AM,		0	} ,			
	{ "PM2.Chuck_UpPos_AM"		, _K_A_IO	,	PM2_CHUCK_UPPOS_AM,		0	} ,		
	{ "PM3.Chuck_UpPos_AM"		, _K_A_IO	,	PM3_CHUCK_UPPOS_AM,		0	} ,		
	
	{ "PM1.Chuck_DnPos_AM"		, _K_A_IO	,	PM1_CHUCK_DNPOS_AM,		0	} ,	
	{ "PM2.Chuck_DnPos_AM"		, _K_A_IO	,	PM2_CHUCK_DNPOS_AM,		0	} ,	
	{ "PM3.Chuck_DnPos_AM"		, _K_A_IO	,	PM3_CHUCK_DNPOS_AM,		0	} ,	
	
	{ "PM1.M_AVlv_CycTgtCntAM"	, _K_A_IO	,	PM1_ALDVLV_CntAM,		0	} ,
	{ "PM2.M_AVlv_CycTgtCntAM"	, _K_A_IO	,	PM2_ALDVLV_CntAM,		0	} ,
	{ "PM3.M_AVlv_CycTgtCntAM"	, _K_A_IO	,	PM3_ALDVLV_CntAM,		0	} ,
	
	{ "PM1.ALECHK_NormDM"		, _K_D_IO	,	PM1_ALECHK_NormDM,		0	} ,	
	{ "PM2.ALECHK_NormDM"		, _K_D_IO	,	PM2_ALECHK_NormDM,		0	} ,	
	{ "PM3.ALECHK_NormDM"		, _K_D_IO	,	PM3_ALECHK_NormDM,		0	} ,	

	{ "PM1.ALECHK_GASLnDM"		, _K_D_IO	,	PM1_ALECHK_GASLnDM,		0	} ,	
	{ "PM2.ALECHK_GASLnDM"		, _K_D_IO	,	PM2_ALECHK_GASLnDM,		0	} ,	
	{ "PM3.ALECHK_GASLnDM"		, _K_D_IO	,	PM3_ALECHK_GASLnDM,		0	} ,	

	{ "PM1.ALECHK_MTRDM"		, _K_D_IO	,	PM1_ALECHK_MTRDM,		0	} ,		
	{ "PM2.ALECHK_MTRDM"		, _K_D_IO	,	PM2_ALECHK_MTRDM,		0	} ,	
	{ "PM3.ALECHK_MTRDM"		, _K_D_IO	,	PM3_ALECHK_MTRDM,		0	} ,	

	{ "PM1.ALECHK_VLVDM"		, _K_D_IO	,	PM1_ALECHK_VLVDM,		0	} ,		
	{ "PM2.ALECHK_VLVDM"		, _K_D_IO	,	PM2_ALECHK_VLVDM,		0	} ,	
	{ "PM3.ALECHK_VLVDM"		, _K_D_IO	,	PM3_ALECHK_VLVDM,		0	} ,

	{ "PM1.LeakChkBasePressAM"	, _K_A_IO	,	PM1_LeakChkBasePressAM	,	0	},
	{ "PM2.LeakChkBasePressAM"  , _K_A_IO	,	PM2_LeakChkBasePressAM  ,	0	},
	{ "PM3.LeakChkBasePressAM"  , _K_A_IO	,	PM3_LeakChkBasePressAM  ,	0	},
	
	{ "PM1.LeakChkPumpTimeOutAM", _K_A_IO	,	PM1_LeakChkPumpTimeOutAM,	0	},
	{ "PM2.LeakChkPumpTimeOutAM", _K_A_IO	,	PM2_LeakChkPumpTimeOutAM,	0	},
	{ "PM3.LeakChkPumpTimeOutAM", _K_A_IO	,	PM3_LeakChkPumpTimeOutAM,	0	},
	
	{ "PM1.BaseDelayTimeAM"	    , _K_A_IO	,	PM1_BaseDelayTimeAM	    ,	0	},
	{ "PM2.BaseDelayTimeAM"	    , _K_A_IO	,	PM2_BaseDelayTimeAM	    ,	0	},
	{ "PM3.BaseDelayTimeAM"	    , _K_A_IO	,	PM3_BaseDelayTimeAM	    ,	0	},
	
	{ "PM1.LeakChkDelayTimeAM"	, _K_A_IO	,	PM1_LeakChkDelayTimeAM	,	0	},
	{ "PM2.LeakChkDelayTimeAM"	, _K_A_IO	,	PM2_LeakChkDelayTimeAM	,	0	},
	{ "PM3.LeakChkDelayTimeAM"	, _K_A_IO	,	PM3_LeakChkDelayTimeAM	,	0	},
	
	{ "PM1.LeakChkCountSPAM"	, _K_A_IO	,	PM1_LeakChkCountAM	    ,	0	},
	{ "PM2.LeakChkCountSPAM"	, _K_A_IO	,	PM2_LeakChkCountAM	    ,	0	},
	{ "PM3.LeakChkCountSPAM"	, _K_A_IO	,	PM3_LeakChkCountAM	    ,	0	},
	
	{ "PM1.LeakChkLimitPressAM"	, _K_A_IO	,	PM1_LeakChkLimitPressAM ,	0	},	
	{ "PM2.LeakChkLimitPressAM"	, _K_A_IO	,	PM2_LeakChkLimitPressAM	,	0	},	
	{ "PM3.LeakChkLimitPressAM"	, _K_A_IO	,	PM3_LeakChkLimitPressAM	,	0	},		
	
	{ "PM1.LnLkChkBasePressAM"	, _K_A_IO	,	PM1_LnLkChkBasePressAM	,	0	},
	{ "PM2.LnLkChkBasePressAM"  , _K_A_IO	,	PM2_LnLkChkBasePressAM  ,	0	},
	{ "PM3.LnLkChkBasePressAM"  , _K_A_IO	,	PM3_LnLkChkBasePressAM  ,	0	},
	
	{ "PM1.LnLkChkPumpTimeOutAM", _K_A_IO	,	PM1_LnLkChkPumpTimeOutAM,	0	},
	{ "PM2.LnLkChkPumpTimeOutAM", _K_A_IO	,	PM2_LnLkChkPumpTimeOutAM,	0	},
	{ "PM3.LnLkChkPumpTimeOutAM", _K_A_IO	,	PM3_LnLkChkPumpTimeOutAM,	0	},
	
	{ "PM1.LnBaseDelayTimeAM"	, _K_A_IO	,	PM1_LnBaseDelayTimeAM	,	0	},
	{ "PM2.LnBaseDelayTimeAM"	, _K_A_IO	,	PM2_LnBaseDelayTimeAM	,	0	},
	{ "PM3.LnBaseDelayTimeAM"	, _K_A_IO	,	PM3_LnBaseDelayTimeAM	,	0	},
	
	{ "PM1.LnLkChkDelayTimeAM"	, _K_A_IO	,	PM1_LnLkChkDelayTimeAM	,	0	},
	{ "PM2.LnLkChkDelayTimeAM"	, _K_A_IO	,	PM2_LnLkChkDelayTimeAM	,	0	},
	{ "PM3.LnLkChkDelayTimeAM"	, _K_A_IO	,	PM3_LnLkChkDelayTimeAM	,	0	},
	
	{ "PM1.LnLkChkCountSPAM"	, _K_A_IO	,	PM1_LnLkChkCountAM	    ,	0	},
	{ "PM2.LnLkChkCountSPAM"	, _K_A_IO	,	PM2_LnLkChkCountAM	    ,	0	},
	{ "PM3.LnLkChkCountSPAM"	, _K_A_IO	,	PM3_LnLkChkCountAM	    ,	0	},
	
	{ "PM1.LnLkChkLimitPressAM"	, _K_A_IO	,	PM1_LnLkChkLimitPressAM	,	0	},	
	{ "PM2.LnLkChkLimitPressAM"	, _K_A_IO	,	PM2_LnLkChkLimitPressAM ,	0	},	
	{ "PM3.LnLkChkLimitPressAM"	, _K_A_IO	,	PM3_LnLkChkLimitPressAM	,	0	},	
	//... 2016.05.13
	{ "PM1.MFC_PurgeTmAM"		, _K_A_IO	,	PM1_MFC_PurgeTmAM		,	0	},
	{ "PM2.MFC_PurgeTmAM"		, _K_A_IO	,	PM2_MFC_PurgeTmAM		,	0	},
	{ "PM3.MFC_PurgeTmAM"		, _K_A_IO	,	PM3_MFC_PurgeTmAM		,	0	},
	//... 2016.08.16
	{ "PM1.RoRLimitAM"			, _K_A_IO	,	PM1_RoRLimitAM			,	0	},
	{ "PM2.RoRLimitAM"			, _K_A_IO	,	PM2_RoRLimitAM			,	0	},
	{ "PM3.RoRLimitAM"			, _K_A_IO	,	PM3_RoRLimitAM			,	0	},
	{ "PM1.LnRoRLimitAM"		, _K_A_IO	,	PM1_LnRoRLimitAM		,	0	},
	{ "PM2.LnRoRLimitAM"		, _K_A_IO	,	PM2_LnRoRLimitAM		,	0	},
	{ "PM3.LnRoRLimitAM"		, _K_A_IO	,	PM3_LnRoRLimitAM		,	0	},
	//...
	{ "PM1.PurgeLineLK_DM"		, _K_D_IO	,	PM1_PurgeLineLK_DM,		0	} ,
	{ "PM2.PurgeLineLK_DM"		, _K_D_IO	,	PM2_PurgeLineLK_DM,		0	} ,
	{ "PM3.PurgeLineLK_DM"		, _K_D_IO	,	PM3_PurgeLineLK_DM,		0	} ,
	{ "PM1.CleanLineLK_DM"		, _K_D_IO	,	PM1_CleanLineLK_DM,		0	} ,
	{ "PM2.CleanLineLK_DM"		, _K_D_IO	,	PM2_CleanLineLK_DM,		0	} ,
	{ "PM3.CleanLineLK_DM"		, _K_D_IO	,	PM3_CleanLineLK_DM,		0	} ,

	//... 2016.04.03 
	{ "PRMA_PM1_DRYCThick"		, _K_A_IO	,	PRMA_PM1_DRYCThick,		0	} ,
	{ "PRMA_PM2_DRYCThick"		, _K_A_IO	,	PRMA_PM2_DRYCThick,		0	} ,
	{ "PRMA_PM3_DRYCThick"		, _K_A_IO	,	PRMA_PM3_DRYCThick,		0	} ,

	{ "PRMD_PM1_AutoCycRun"		, _K_D_IO	,	PRMD_PM1_AutoCycRun,	0	} ,
	{ "PRMD_PM2_AutoCycRun"		, _K_D_IO	,	PRMD_PM2_AutoCycRun,	0	} ,
	{ "PRMD_PM3_AutoCycRun"		, _K_D_IO	,	PRMD_PM3_AutoCycRun,	0	} ,

	{ "CTC.PM1_CLN_RECIPE_FILE"	, _K_S_IO   ,	PM1_CLN_RECIPE_FILE,	0	} ,
	{ "CTC.PM2_CLN_RECIPE_FILE"	, _K_S_IO   ,	PM2_CLN_RECIPE_FILE,	0	} ,
	{ "CTC.PM3_CLN_RECIPE_FILE"	, _K_S_IO   ,	PM3_CLN_RECIPE_FILE,	0	} ,

	{ "PM1.SysLeakChkDM"		, _K_D_IO	,	PM1_SysLeakChkDM,		0	} ,
	{ "PM2.SysLeakChkDM"		, _K_D_IO	,	PM2_SysLeakChkDM,		0	} ,
	{ "PM3.SysLeakChkDM"		, _K_D_IO	,	PM3_SysLeakChkDM,		0	} ,
	""
};

//2014.07.20
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);	
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szData)
{
	char szEventID[20];
	
	do {
		if(STRCMP_L(szRunStr , "LeakCheck")) strcpy(szEventID, "AutoLeakCheck");
		else
		{
			_MLOG("-----> %s %s %s %s", szModule, szRunStr, szStatus, szData);
			break;
		}
		_MLOG("FNC %s %s %s %s", szModule, szEventID, szStatus, szData);
	} while(0);
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern "C" void Screen_Nav_Info( int no )
{
	//Class destruction here
	if (no == -1) {
		if (NULL != g_pLeakCheckCtrl)
			delete g_pLeakCheckCtrl;
		
	}
}

extern "C" BOOL Program_Init_Code()
{
	//Class construct & Initialize here
	g_pLeakCheckCtrl = new CLeakCheckControl();
	if (NULL != g_pLeakCheckCtrl) {
		return g_pLeakCheckCtrl->Initialize();
	} else return FALSE;
	
	return TRUE;
}

extern "C" void Program_Enter_Code()
{

}

extern "C" void Event_Message_Received()
{
	
}

//-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	Module_Status msRet = SYS_ABORTED;
	
	int nParLen = 0;
	int nIOStatus;
	char *pParam;
	char szCmd[41], szSubParam[256];

	printf("Entered %s\n", APP_NAME);

	if (NULL == g_pLeakCheckCtrl) return SYS_ABORTED;
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);
		if ( nParLen < 1 )	{
			printf( "---> Parameter Error, Len = %d in %s !\n", nParLen, APP_NAME);
			msRet = SYS_ABORTED;
			break;
		}

		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );	
		if (STRCMP_L(szCmd, "ABORT")) {
			//Just Write to IO then Auto Leak Check Control Function will be Reset the IO
			WRITE_DIGITAL(LeakCheckAbortSts , 1/*Abort*/ , &nIOStatus);
			msRet = SYS_SUCCESS;
		} else {
		
		}
	} while(0);

	printf("Leave %s\n", APP_NAME);
	return msRet;
}

