#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding,
	CTC_EVENT_LOG_SVR,		// 2013.11.15
	FC_Vacuum_BM1			,
	LockPumpDM_BM1			,
	VacuumStatusDM_BM1		,
//	SlowRoughVvDO_BM1		,	
//	FastRoughVvDO_BM1		,
	RoughVvXO_BM1			,
//	SlowVentVvDO_BM1		,	
//	FastVentVvDO_BM1		,
	VentVvXO_BM1			,
	ATMVACSensorXI			,
	CGChmPrsAI				,
	CGPumpPrsAI				,	
	BM1_PUMP_CONTROL		,
	DOOR_CONTROL			,
	BM_DoorLXI				,
	BM_DoorRXI				,
//--------------------------
	PRMA_BM1_SVENT_PRES		,
	PRMA_BM1_FVENT_PRES		,	
	PRMA_BM1_SPUMP_PRES		,
	PRMA_BM1_FPUMP_PRES		,
	PRMA_BM1_RPUMP_PRES		,	
	PRMD_BM1_SVENT_TMO		,
	PRMD_BM1_FVENT_TMO		,	
	PRMD_BM1_SPUMP_TMO		,
	PRMD_BM1_FPUMP_TMO		,	
	PRMD_BM1_RPUMP_TMO		,	
	PRMD_BM1_OVENT_TIME		,
	LL_VENT_DELAYTIME		,
//--------------------------
//--------------------------
	BM1_Leak_Test_Number	,	
	BM1_Leak_Test_Time		,
	BM1_Leak_End			,	
	BM1_Year				,	
	BM1_Month 				,
	BM1_Day 				,	
	BM1_Hour 				,	
	BM1_Min 				,	
	BM1_Sec 				,	
	BM1_Leak_Rate1			,
	BM1_Leak_Rate2			,
	BM1_Leak_Rate3			,
	BM1_Leak_Rate4			,
	BM1_Leak_Rate5			,
	BM1_Leak_Rate6			,
	BM1_Leak_Rate7			,
	BM1_Leak_Rate8			,
	BM1_Leak_Rate9			,
	BM1_Leak_Rate10			,	
	BM1_Leak_LastRate		,	
	BM1_Leak_PumpingTime_H	,
	BM1_Leak_PumpingTime_M	,
	BM1_Leak_WaitingTime_H	,
	BM1_Leak_WaitingTime_M	,	
	BM_VentType				,
	BM1_LeakChk_Message		,
	BM1_Leak_PumpRemain_Time,	
	BM1_Leak_WaitRemain_Time,	
	BM1_PC_LEAKAVE			,
	BM1_PC_LKCHK_LASTTIME	,	
	BM1_LOGGING				,	
	TMC_BM1_OVERPRESSURE    ,
	TMC_ISOGATE_CONTROL     ,
	LOT_CONTROL             ,	
	LOT2_CONTROL            ,
	LOT3_CONTROL            ,
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
	FC_CYCLE_PURGE_BM1,
	PRMA_BM1_CP_SVENT_PRES,
	PRMA_BM1_CP_FVENT_PRES,
	PRMA_BM1_CP_SPUMP_PRES,
	PRMA_BM1_CP_FPUMP_PRES,

	PRMA_BM1_CP_RPUMP_PRES,

	PRMD_BM1_CP_SVENT_TMO,
	PRMD_BM1_CP_FVENT_TMO,

	PRMD_BM1_CP_SPUMP_TMO,
	PRMD_BM1_CP_FPUMP_TMO,

	PRMD_BM1_CP_RPUMP_TMO,
	PRMD_BM1_CP_OVENT_TIME,

	LL_CP_VENT_DELAYTIME,
	PRMD_BM1_CYCLE_PURGE_CNT,
	BM1_CYCLE_PURGE_RCNT_DM,
	BM1_CYCLE_PURGE_STS_DM,
	//... 2017.03.06
	PRMA_BM1_TOTAL_VENT_TIME,
	PRMD_BM1_STABLE_OVERVENT,
	//... 2017.03.07
	PRMD_BM1_FINAL_PUMP,
	MANUAL_TR_CONTROL
	//...
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
