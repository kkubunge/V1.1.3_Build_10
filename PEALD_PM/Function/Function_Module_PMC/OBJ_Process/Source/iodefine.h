#ifndef IODEFINE_H
#define IODEFINE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
// IO List
//

typedef enum {
	IO_Driver_Loding,
	SW_GbIntlckDM,
	SW_ModuleFailDM,
	SimModeDM,
	WaferDI,
	PRO_ProcStsDM,

	PRO_RcpNameSM,
	PRO_StepNameSM,
	PRO_TotalStepAM,
	PRO_CurStepAM,
	PRO_RcpTypeDM,
	PRO_IsMainDM,
	PRO_TotalCycAM,
	PRO_CurCycAM,
	PRO_IdleRcpStsDM,
	PRO_RunStsDM,
	PRO_StopCtrlDM,

	// 2004.07.19 by cskim, 'Step Group Cycle' Added
	PRO_StepGrpSM,
	PRO_StpGrpTCycAM,
	PRO_StpGrpCCycAM,

	// 2004.09.13 by cskim
	PRO_TotalTimeAM,
	PRO_CurrTimeAM,

	// 2005.04.18 by cskim
	PRO_StepTimeAM,
	PRO_StpTotTmAM,
	PRO_StpCurTmAM,

	// 2004.11.16 by cskim
	PRO_PreTreatDM,

	// 2004.08.02 by cskim, 'Previetive Maintance Sequence' Added
	PRO_ComptDM,
	PRO_PrvVlvCycAM,
	PRO_PrvSrcFdAM,

	DN_GetMessage_DO,
	DN_SetSvcCode_DO,
	DN_SetClsID_DO,
	DN_SetInstID_DO,
	DN_SendByte_DO,
	DN_ReadGetSvcCode_DI,
	DN_SendPacket_DO,

	DN_SMCmd_DO,
	DN_SetSMData_DO,
	DN_ClearSMData_DO,  // 2014.08.25

	ACVPrcPosDI,
	ACVClnPosDI,
	ACVCtrlDO,
	ACV_UseDM,
	ACV_ModeDM,

	FastRoughVlvDO,
	ProcBGIsoVlvDO,
	VacSwIsoVlvDO,
	CVGIsoVlvDO,
	PgSwIsoVlvDO,
	CVGIsoUseDM,

	TMIsoVlvClosedDI,
	TMIsoVlv2ClosedDI,
	ChmPrsAI,

	// 2004.09.22 by cskim
	PLC_CurStepAI,
	PLC_CurCycAI,

	VRCP_StaStpDO,
	VRCP_ResumeDO,
	VRCP_GoNextDO,

	VRCP_RunningDI		,
	VRCP_CycCntAI		,
	VRCP_TgCycAI		,
	VRCP_StpCntAI		,
	VRCP_TgStpAI		,

	PLC_RstIntLckDO,
	PLC_GbIntlckDI,

	VRCP_PauseDI,
	VRCP_DnLdErr1DI,
	VRCP_DnLdErr2DI,
	VRCP_DnLdErr3DI,
	VRCP_DnLdErr4DI,
	VRCP_DnLdErr5DI,

	RF_UseDM,

	PRMA_WT_CVGSafeWait,
	PRMA_TL_SHtrTmp,
	PRMA_TO_SHtrStb,


	PROCESS_LOG_EVENT,		// 2005.02.18 by cskim, Multi DCOP Log
	PROCESS_LOG_STS,		// 2005.07.14, Data Log Timming

	PROCBGRUN_CONTROL,
	ABORT_GAS,
	MFC_CONTROL	,
	PRESS_CONTROL,
	VACUUM_CONTROL	,

	CHUCK_CONTROL,
	RF_CONTROL,
	RF_CONTROL2,
	STGHTR_CONTROL,
	STGHTR_CONTROL2,
	HTR_CONTROL,
	TIME_MONITOR,
	PM_SIMULATOR,
	PRS_CONTROL,
	PRS_MONITOR,
	REFILL_CONTROL,
	VALVE_CONTROL,
	HTE_UseDM,    //2014.04.01
	HT_ExchgControl,
	//2014.04.24
	Wafer_Source,
	Wafer_Source2,
	Wafer_Status,
	Wafer_Status2,
	DummyRecipeUse,

	RF_COUNT_MONITOR,		//... 2014.09.29
	PMC_EVENT_LOG_CLI,		//... 2014.10.13 MARS
	LOTPRE_SKIP_TIMER,		//... 2015.01.02
	EasyCluster_Connect,	//... 2015.01.02

	ALECHK_GASLnDM,		// 2016.01.19

	ALDIOMon, //2015.01.06

	//... 2015.02.26 RPG Alarm Auto Retry
	RPGAlmPrcRetryDM,		//... Retry Flag
	PRMD_RPGPrcRetryOpt,	//... Retry Option by User Config

	RPGAlmPrcRetryAM,		//... Current Retry Count
	PRMA_RPGPrcRetryCnt,	//... Retry Count Limit by User Config

	PLC_RCPRunTMAI,			//... 2015.03.07 Add RPG ON Step Skip
	RPGPlasmaDetectDI,		//... 2015.03.07 Add RPG ON Step Skip

	ALECHK_NormDM,			//... 2015.09.17
	PRMD_SwpPug_RcpEnd,		//... 2015.09.28
	ALECHK_RCPDM,			//... 2016.01.19
	PRO_PurgeRcpStsDM,		//... 2016.01.19
	SysLeakChkDM,			//... 2016.01.19
	LKMacroType1DM,
	LKMacroType2DM,
	LKMacroType3DM,
	PRMD_ZCtrl_RCP_SKIP,
	PRMD_ZCtrl_DLY_TIME,
	PRMD_ZCtrl_RCP_TYPE,
} IOPointIndex;

//------------------------------------------------------------------------------------------

#endif
