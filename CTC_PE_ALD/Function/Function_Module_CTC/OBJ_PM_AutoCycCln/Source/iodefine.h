#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {

	FUNC_SCHEDULER,
	Sys_Min,

	CTC_PM_CommStatus,
	PM_PRO_RunStsDM,
	PM_PRO_RcpTypeDM,
	PM_PRO_IdleRcpStsDM,

	PM_MAINT_CONTROL,
	PM_MAIN_CONTROL,

	PM_PROCESS_IN,
	PM_PROCESS_OUT_BRG,
	PM_PROCESS_OUT,
	PM_M_PROCESS,
	SCHEDULER_MAINT_PM,

	PM_RECIPE_FILE,

	CTC_LPM1_LotPMPath,
	CTC_LPM2_LotPMPath,
	CTC_LPM3_LotPMPath,
	
	CTC_MAIN_CONTROL,
	CTC_MAIN_CONTROL2,
	CTC_MAIN_CONTROL3,

	PRMD_PM_AutoCycUse,
	PRMD_PM_AutoCycRun,
	PRMD_PM_AutoCycTmH,
	PRMD_PM_AutoCycTmM,
	PRMD_PM_AutoCycStop,

	PRMS_PM_AutoCycRcp,

	PRMD_PM_IDLE_TimeD,
	PRMD_PM_IDLE_TimeH,
	PRMD_PM_IDLE_TimeM,

	PRMD_PM_Leaf_RunSt_05,
	CHAMBER_CON_ABORT_PM,

	CTC_AutoLeakCheckSts,
	
	CTC_PM_Wafer_Status,
	CTC_PM_Wafer_Status2,

	PRO_ClnStsDM,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
