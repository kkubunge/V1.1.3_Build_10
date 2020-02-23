#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	FC_Vacuum_TM			,
	
	LockPumpDM_TM			,
	VacuumStatusDM_TM		,
	RoughVvXO_TM			,
	VentVvXO_TM				,	
	ATMVACSensorXI			,

	CGChmPrsAI				,
	CGPumpPrsAI				,
	
	TM_PUMP_CONTROL			,
	GATE_CONTROL			,
	PM_GATE_CONTROL			,

	PRMA_TM_SVENT_PRES		,
	PRMA_TM_FVENT_PRES		,	
	PRMA_TM_SPUMP_PRES		,
	PRMA_TM_FPUMP_PRES		,
	PRMA_TM_RPUMP_PRES		,

	PRMD_TM_SVENT_TMO		,
	PRMD_TM_FVENT_TMO		,
	PRMD_TM_SPUMP_TMO		,
	PRMD_TM_FPUMP_TMO		,	
	PRMD_TM_RPUMP_TMO		,	
	PRMD_TM_OVENT_TIME		,
//--------------------------
	TM_Leak_Test_Number	    ,	
	TM_Leak_Test_Time		,
	TM_Leak_End			    ,	
	
	TM_Year				    ,	
	TM_Month 				,
	TM_Day 				    ,	
	TM_Hour 				,	
	TM_Min 				    ,	
	TM_Sec 				    ,	
	
	TM_Leak_Rate1			,
	TM_Leak_Rate2			,
	TM_Leak_Rate3			,
	TM_Leak_Rate4			,
	TM_Leak_Rate5			,
	TM_Leak_Rate6			,
	TM_Leak_Rate7			,
	TM_Leak_Rate8			,
	TM_Leak_Rate9			,
	TM_Leak_Rate10			,
	
	TM_Leak_LastRate		,	
	
	TM_Leak_PumpingTime_H	,
	TM_Leak_PumpingTime_M	,
	TM_Leak_WaitingTime_H	,
	TM_Leak_WaitingTime_M	,
	
	TM_LeakChk_Message		,
	TM_Leak_PumpRemain_Time	,	
	TM_Leak_WaitRemain_Time	,
	
	TM_PC_LEAKAVE			,
	TM_PC_LKCHK_LASTTIME	,	
	TM_LOGGING				,

	TM_OverPresVvDO,
	// 2006.01.19
	PRMD_OVER_PRES_OPT,
	PRMA_TM_TransPrs,

	LOT_CONTROL,	
	LOT2_CONTROL,
	LOT3_CONTROL,

	//Auto Leak Check Associated 2008.04.04
	LeakCheckSts			,
	LeakRateLimit			,
	LeakAvgLimit			,

	Log_Rate1				,
	Log_Rate2				,
	Log_Rate3				,
	Log_Rate4				,
	Log_Rate5				,
	Log_Avg1				,
	Log_Avg2				,
	Log_Avg3				,
	Log_Avg4				,
	Log_Avg5				,
	Log_Time1				,
	Log_Time2				,
	Log_Time3				,
	Log_Time4				,
	Log_Time5				,

// 2008.11.17 by jonghyuk (Cycle Purge)
	FC_CYCLE_PURGE_TM,

//	PRMA_TM_CP_SVENT_PRES,
	PRMA_TM_CP_FVENT_PRES,

	PRMA_TM_CP_SPUMP_PRES,
	PRMA_TM_CP_FPUMP_PRES,

	PRMA_TM_CP_RPUMP_PRES,

//	PRMD_TM_CP_SVENT_TMO,
	PRMD_TM_CP_FVENT_TMO,

	PRMD_TM_CP_SPUMP_TMO,
	PRMD_TM_CP_FPUMP_TMO,

	PRMD_TM_CP_RPUMP_TMO,

	PRMD_TM_CP_OVENT_TIME,

	PRMD_TM_CYCLE_PURGE_CNT,
	TM_CYCLE_PURGE_RCNT_DM,
	TM_CYCLE_PURGE_STS_DM,
	TMC_RBLOW_CONTROL,
	PRMD_TM_ALECHK_DYN,
	TMC_TM_OVERPRESSURE,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
