#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding,
	CTC_EVENT_LOG_SVR,		// 2013.11.15
	FC_Vacuum_BM2			,	
	LockPumpDM_BM2			,
	VacuumStatusDM_BM2		,
//	SlowRoughVvDO_BM2		,	
//	FastRoughVvDO_BM2		,
	RoughVvXO_BM2			,
//	SlowVentVvDO_BM2		,	
//	FastVentVvDO_BM2		,
	VentVvXO_BM2			,	
	ATMVACSensorXI			,

	CGChmPrsAI				,
	CGPumpPrsAI				,	

	BM2_PUMP_CONTROL		,
	DOOR_CONTROL			,
	BM_DoorLXI				,
	BM_DoorRXI				,
//--------------------------
	PRMA_BM2_SVENT_PRES		,
	PRMA_BM2_FVENT_PRES		,	
	PRMA_BM2_SPUMP_PRES		,
	PRMA_BM2_FPUMP_PRES		,
	PRMA_BM2_RPUMP_PRES		,	
	PRMD_BM2_SVENT_TMO		,
	PRMD_BM2_FVENT_TMO		,	
	PRMD_BM2_SPUMP_TMO		,
	PRMD_BM2_FPUMP_TMO		,	
	PRMD_BM2_RPUMP_TMO		,	
	PRMD_BM2_OVENT_TIME		,
	LL_VENT_DELAYTIME		,
//--------------------------
//--------------------------
	BM2_Leak_Test_Number	,	
	BM2_Leak_Test_Time		,
	BM2_Leak_End			,	
	
	BM2_Year				,	
	BM2_Month 				,
	BM2_Day 				,	
	BM2_Hour 				,	
	BM2_Min 				,	
	BM2_Sec 				,	
	
	BM2_Leak_Rate1			,
	BM2_Leak_Rate2			,
	BM2_Leak_Rate3			,
	BM2_Leak_Rate4			,
	BM2_Leak_Rate5			,
	BM2_Leak_Rate6			,
	BM2_Leak_Rate7			,
	BM2_Leak_Rate8			,
	BM2_Leak_Rate9			,
	BM2_Leak_Rate10			,
	
	BM2_Leak_LastRate		,	
	
	BM2_Leak_PumpingTime_H	,
	BM2_Leak_PumpingTime_M	,
	BM2_Leak_WaitingTime_H	,
	BM2_Leak_WaitingTime_M	,
	BM_VentType				,
	
	BM2_LeakChk_Message		,
	BM2_Leak_PumpRemain_Time,	
	BM2_Leak_WaitRemain_Time,
	
	BM2_PC_LEAKAVE			,
	BM2_PC_LKCHK_LASTTIME	,	
	BM2_LOGGING				,
	TMC_BM2_OVERPRESSURE	,
	TMC_ISOGATE_CONTROL     ,
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
	FC_CYCLE_PURGE_BM2,
	PRMA_BM2_CP_SVENT_PRES,
	PRMA_BM2_CP_FVENT_PRES,
	PRMA_BM2_CP_SPUMP_PRES,
	PRMA_BM2_CP_FPUMP_PRES,

	PRMA_BM2_CP_RPUMP_PRES,

	PRMD_BM2_CP_SVENT_TMO,
	PRMD_BM2_CP_FVENT_TMO,

	PRMD_BM2_CP_SPUMP_TMO,
	PRMD_BM2_CP_FPUMP_TMO,

	PRMD_BM2_CP_RPUMP_TMO,
	PRMD_BM2_CP_OVENT_TIME,

	LL_CP_VENT_DELAYTIME,
	PRMD_BM2_CYCLE_PURGE_CNT,
	BM2_CYCLE_PURGE_RCNT_DM,
	BM2_CYCLE_PURGE_STS_DM,
	//... 2017.03.06
	PRMA_BM2_TOTAL_VENT_TIME,
	PRMD_BM2_STABLE_OVERVENT,
	//... 2017.03.07
	PRMD_BM2_FINAL_PUMP,
	MANUAL_TR_CONTROL
	//...
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
