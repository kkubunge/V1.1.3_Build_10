///////////////////////////////////////////////////////////////////////////////////
// Process Control

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


#include "RecipeStruct.h"
#include "ProcessSequence.h"			// 48 Channel Valve Control
#include "ProcessSequenceType2.h"		// 112 Channel Valve Control
#include "TextParser.h"
#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
#define APP_NAME	"ProcessControl"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		,	_K_D_IO	,	IO_Driver_Loding	,	0	} ,
	{ "SW_GbIntlckDM"			,	_K_D_IO	,	SW_GbIntlckDM		,	0	} ,
	{ "SW_ModuleFailDM"			,	_K_D_IO	,	SW_ModuleFailDM		,	0	} ,
	{ "SimModeDM"				,	_K_D_IO	,	SimModeDM			,	0	} ,
	// 2006.09.15
	{ "WaferDI"					,	_K_D_IO	,	WaferDI				,	0	} ,
	// 2005.01.14 by cskim
	{ "PRO_ProcStsDM"			,	_K_D_IO	,	PRO_ProcStsDM		,	0	} ,

	{ "PRO_RcpNameSM"			,	_K_S_IO	,	PRO_RcpNameSM		,	0	} ,
	{ "PRO_StepNameSM"			,	_K_S_IO	,	PRO_StepNameSM		,	0	} ,
	{ "PRO_TotalStepAM"			,	_K_A_IO	,	PRO_TotalStepAM		,	0	} ,
	{ "PRO_CurStepAM"			,	_K_A_IO	,	PRO_CurStepAM		,	0	} ,
	{ "PRO_RcpTypeDM"			,	_K_D_IO	,	PRO_RcpTypeDM		,	0	} ,
	{ "PRO_IsMainDM"			,	_K_D_IO	,	PRO_IsMainDM		,	0	} ,
	{ "PRO_TotalCycAM"			,	_K_A_IO	,	PRO_TotalCycAM		,	0	} ,
	{ "PRO_CurCycAM"			,	_K_A_IO	,	PRO_CurCycAM		,	0	} ,
	{ "PRO_IdleRcpStsDM"		,	_K_D_IO	,	PRO_IdleRcpStsDM	,	0	} ,
	{ "PRO_RunStsDM"			,	_K_D_IO	,	PRO_RunStsDM		,	0	} ,
	{ "PRO_StopCtrlDM"			,	_K_D_IO	,	PRO_StopCtrlDM		,	0	} ,		//... 2015.01.06 Add Process Stop Channel

	// 2004.07.19 by cskim, 'Step Group Cycle' Added
	{ "PRO_StepGrpSM"			,	_K_S_IO	,	PRO_StepGrpSM		,	0	} ,
	{ "PRO_StpGrpTCycAM"		,	_K_A_IO	,	PRO_StpGrpTCycAM	,	0	} ,
	{ "PRO_StpGrpCCycAM"		,	_K_A_IO	,	PRO_StpGrpCCycAM	,	0	} ,

	// 2004.09.13 by cskim
	{ "PRO_TotalTimeAM"			,	_K_A_IO	,	PRO_TotalTimeAM		,	0	} ,
	{ "PRO_CurrTimeAM"			,	_K_A_IO	,	PRO_CurrTimeAM		,	0	} ,

	// 2005.04.18 by cskim
	{ "PRO_StepTimeAM"			,	_K_A_IO	,	PRO_StepTimeAM		,	0	} ,
	{ "PRO_StpTotTmAM"			,	_K_A_IO	,	PRO_StpTotTmAM		,	0	} ,
	{ "PRO_StpCurTmAM"			,	_K_A_IO	,	PRO_StpCurTmAM		,	0	} ,

	// 2004.11.16 by cskim
	{ "PRO_PreTreatDM"			,	_K_D_IO	,	PRO_PreTreatDM		,	0	} ,

	// 2004.08.02 by cskim, 'Previetive Maintance Sequence' Added
	{ "PRO_ComptDM"				,	_K_D_IO	,	PRO_ComptDM			,	0	} ,
	{ "PRO_PrvVlvCycAM"			,	_K_A_IO	,	PRO_PrvVlvCycAM		,	0	} ,
	{ "PRO_PrvSrcFdAM"			,	_K_A_IO	,	PRO_PrvSrcFdAM		,	0	} ,

	{ "DN_GetMessage_DO"		,	_K_D_IO	,	DN_GetMessage_DO	,	0	} ,
	{ "DN_SetSvcCode_DO"		,	_K_D_IO	,	DN_SetSvcCode_DO	,	0	} ,
	{ "DN_SetClsID_DO"			,	_K_D_IO	,	DN_SetClsID_DO		,	0	} ,
	{ "DN_SetInstID_DO"			,	_K_D_IO	,	DN_SetInstID_DO		,	0	} ,
	{ "DN_SendByte_DO"			,	_K_D_IO	,	DN_SendByte_DO		,	0	} ,
	{ "DN_ReadGetSvcCode_DI"	,	_K_D_IO	,	DN_ReadGetSvcCode_DI,	0	} ,
	{ "DN_SendPacket_DO"		,	_K_D_IO	,	DN_SendPacket_DO	,	0	} ,

	// 2004.09.10  by cskim
	{ "DN_SMCmd_DO"				,	_K_D_IO	,	DN_SMCmd_DO			,	0	} ,
	{ "DN_SetSMData_DO"			,	_K_D_IO	,	DN_SetSMData_DO		,	0	} ,
	{ "DN_ClearSMData_DO"		, _K_D_IO	,	DN_ClearSMData_DO	,	0	} ,			//2014.08.25

	{ "ACVPrcPosDI"				,	_K_D_IO	,	ACVPrcPosDI			,	0	} ,
	{ "ACVClnPosDI"				,	_K_D_IO	,	ACVClnPosDI			,	0	} ,
	{ "ACVCtrlDO"				,	_K_D_IO	,	ACVCtrlDO			,	0	} ,
	{ "ACV_UseDM"				,	_K_D_IO	,	ACV_UseDM			,	0	},
	{ "ACV_ModeDM"				,	_K_D_IO	,	ACV_ModeDM			,	0	},

	{ "FastRoughVlvDO"			,	_K_D_IO	,	FastRoughVlvDO		,	0	} ,
	{ "ProcBGIsoVlvDO"			,	_K_D_IO	,	ProcBGIsoVlvDO		,	0	} ,
	{ "VacSwIsoVlvDO"			,	_K_D_IO	,	VacSwIsoVlvDO		,	0	} ,
	{ "CVGIsoVlvDO"				,	_K_D_IO	,	CVGIsoVlvDO			,	0	} ,
	{ "PgSwIsoVlvDO"			,	_K_D_IO	,	PgSwIsoVlvDO		,	0	} ,
	{ "CVGIsoUseDM"				,	_K_D_IO	,	CVGIsoUseDM			,	0	} ,

	{ "TMIsoVlvClosedDI"		,	_K_D_IO	,	TMIsoVlvClosedDI	,	0	} ,
	{ "TMIsoVlv2ClosedDI"		,	_K_D_IO	,	TMIsoVlv2ClosedDI	,	0	} ,
	{ "ChmPrsAI"				,	_K_A_IO	,	ChmPrsAI			,	0	} ,

	// 2004.09.22 by cskim
	{ "PLC_CurStepAI"			,	_K_A_IO	,	PLC_CurStepAI		,	0	} ,
	{ "PLC_CurCycAI"			,	_K_A_IO	,	PLC_CurCycAI		,	0	} ,

	{ "VRCP_StaStpDO"			,	_K_D_IO	,	VRCP_StaStpDO		,	0	} ,
	{ "VRCP_ResumeDO"			,	_K_D_IO	,	VRCP_ResumeDO		,	0	} ,
	// 2004.11.16
	{ "VRCP_GoNextDO"			,	_K_D_IO	,	VRCP_GoNextDO		,	0	} ,

	{ "VRCP_RunningDI"			,	_K_D_IO	,	VRCP_RunningDI		,	0	} ,
	{ "VRCP_CycCntAI"			,	_K_A_IO	,	VRCP_CycCntAI		,	0	} ,
	{ "VRCP_TgCycAI"			,	_K_A_IO	,	VRCP_TgCycAI		,	0	} ,
	{ "VRCP_StpCntAI"			,	_K_A_IO	,	VRCP_StpCntAI		,	0	} ,
	{ "VRCP_TgStpAI"			,	_K_A_IO	,	VRCP_TgStpAI		,	0	} ,

	{ "PLC_RstIntLckDO"			,	_K_D_IO	,	PLC_RstIntLckDO		,	0	} ,
	{ "PLC_GbIntlckDI"			,	_K_D_IO	,	PLC_GbIntlckDI		,	0	} ,

	{ "VRCP_PauseDI"			,	_K_D_IO	,	VRCP_PauseDI		,	0	} ,
	{ "VRCP_DnLdErr1DI"			,	_K_D_IO	,	VRCP_DnLdErr1DI		,	0	} ,
	{ "VRCP_DnLdErr2DI"			,	_K_D_IO	,	VRCP_DnLdErr2DI		,	0	} ,
	{ "VRCP_DnLdErr3DI"			, _K_D_IO	,	VRCP_DnLdErr3DI		,	0	} ,
	{ "VRCP_DnLdErr4DI"			, _K_D_IO	,	VRCP_DnLdErr4DI		,	0	} ,
	{ "VRCP_DnLdErr5DI"			, _K_D_IO	,	VRCP_DnLdErr5DI		,	0	} ,

	{ "RF_UseDM"				,	_K_D_IO	,	RF_UseDM			,	0	} ,

	{ "PRMA_WT_CVGSafeWait"		,	_K_A_IO	,	PRMA_WT_CVGSafeWait	,	0	} ,
	{ "PRMA_TL_SHtrTmp"			,	_K_A_IO	,	PRMA_TL_SHtrTmp		,	0	} ,
	{ "PRMA_TO_SHtrStb"			,	_K_A_IO	,	PRMA_TO_SHtrStb		,	0	} ,

	// 2005.02.18 by cskim, Multi DCOP Log
	{ "e:PROCESS_LOG"			, _K_S_IO	,	PROCESS_LOG_EVENT	,	0	} ,		// 2005.02.18 by cskim, Multi DCOP Log
	// 2005.07.14, Data Log Timming
	{ "v:PROCESS_LOG"			,	_K_D_IO	,	PROCESS_LOG_STS		,	0	} ,

	//2009.05.20
	{ "PROCBGRUN_CONTROL"		,	_K_F_IO	,	PROCBGRUN_CONTROL	,	0	} ,
	{ "ABORT_GAS"				,	_K_F_IO	,	ABORT_GAS			,	0	} ,
	{ "MFC_CONTROL"				,	_K_F_IO	,	MFC_CONTROL			,	0	} ,
	{ "PRESS_CONTROL"			,	_K_F_IO	,	PRESS_CONTROL		,	0	} ,
	{ "VACUUM_CONTROL"			,	_K_F_IO	,	VACUUM_CONTROL		,	0	} ,

	{ "CHUCK_CONTROL"			,   _K_F_IO	,	CHUCK_CONTROL		,	0	} ,
	{ "RF_CONTROL"				,	_K_F_IO	,	RF_CONTROL			,	0	} ,
	{ "RF_CONTROL2"				,   _K_F_IO	,	RF_CONTROL2			,	0	} ,
	{ "STGHTR_CONTROL"			,	_K_F_IO	,	STGHTR_CONTROL		,	0	} ,
	{ "STGHTR_CONTROL2"			,	_K_F_IO	,	STGHTR_CONTROL2		,	0	} ,
	{ "HTR_CONTROL"				,	_K_F_IO	,	HTR_CONTROL			,	0	} ,

	{ "TIME_MONITOR"			,	_K_F_IO	,	TIME_MONITOR		,	0	} ,
	{ "PM_SIMULATOR"			,	_K_F_IO	,	PM_SIMULATOR		,	0	} ,

	{ "PRS_CONTROL"				,	_K_F_IO	,	PRS_CONTROL			,	0	} ,
	{ "PRS_MONITOR"				,	_K_F_IO	,	PRS_MONITOR			,	0	} ,
	// 2005.05.05 by cskim, Auto Refill
	{ "REFILL_CONTROL"			,	_K_F_IO	,	REFILL_CONTROL		,	0	} ,
	// 2005.05.30 by cskim, for Keep Last Condition
	{ "VALVE_CONTROL"			,	_K_F_IO	,	VALVE_CONTROL		,	0	} ,
	// 2014.04.01
	{ "HTE_UseDM"			    ,   _K_D_IO ,   HTE_UseDM			,	0	} ,
	{ "HT_ExchgControl"			,	_K_F_IO ,	HT_ExchgControl		,   0   } ,
	// 2014.04.24
	{ "Wafer_Source"			,	_K_D_IO	,	Wafer_Source        ,	0	} ,
	{ "Wafer_Source2"			,	_K_D_IO	,	Wafer_Source2       ,	0	} ,
	{ "Wafer_Status"			,	_K_D_IO	,	Wafer_Status        ,	0	} ,
	{ "Wafer_Status2"			,	_K_D_IO	,	Wafer_Status2       ,	0	} ,
	{ "PRMD_DummyRecipe"		,	_K_D_IO	,	DummyRecipeUse       ,	0	} ,

	{ "RF_COUNT_MONITOR"		,	_K_F_IO ,	RF_COUNT_MONITOR	,   0   } ,		//... 2014.09.29 RF Count Monitor
	{ "PMC_EVENT_LOG_CLI"		,   _K_F_IO	,	PMC_EVENT_LOG_CLI	,	0	} ,		//... 2014.10.13 MARS
	{ "LOTPRE_SKIP_TIMER"		,   _K_F_IO	,	LOTPRE_SKIP_TIMER	,	0	} ,		//... 2014.11.03 Lot Pre Skip Case
	{ "EasyCluster_Connect"		,	_K_D_IO	,	EasyCluster_Connect ,	0	} ,		//... 2014.11.03 Lot Pre Skip Case

	{ "ALECHK_GASLnDM"			,	_K_D_IO	,	ALECHK_GASLnDM		,	0	} ,		// 2016.01.19

	// 2015.01.25
	{ "ALDIOMon"				,	_K_F_IO	,	ALDIOMon			,	0	} ,

	//... 2015.02.26 RPG Alarm Auto Retry
	{ "RPGAlmPrcRetryDM"		,	_K_D_IO	,	RPGAlmPrcRetryDM	,	0	} ,		//... Retry Flag
	{ "PRMD_RPGPrcRetryOpt"		,	_K_D_IO	,	PRMD_RPGPrcRetryOpt	,	0	} ,		//... Retry Option by User Config

	{ "RPGAlmPrcRetryAM"		,	_K_A_IO	,	RPGAlmPrcRetryAM	,	0	} ,		//... Current Retry Count
	{ "PRMA_RPGPrcRetryCnt"		,	_K_A_IO	,	PRMA_RPGPrcRetryCnt	,	0	} ,		//... Retry Count Limit by User Config

	{ "PLC_RCPRunTMAI"			,	_K_A_IO	,	PLC_RCPRunTMAI		,	0	} ,		//... 2015.03.07 Add RPG ON Step Skip
	{ "RPGPlasmaDetectDI"		,	_K_D_IO	,	RPGPlasmaDetectDI	,	0	} ,		//... 2015.03.07 Add RPG ON Step Skip

	{ "ALECHK_NormDM"			,	_K_D_IO	,	ALECHK_NormDM		,	0	} ,		//... 2015.09.17
	{ "PRMD_SwpPug_RcpEnd"		,	_K_D_IO	,	PRMD_SwpPug_RcpEnd  ,	0	} ,		//... 2015.09.28
	{ "ALECHK_RCPDM"			,	_K_D_IO	,	ALECHK_RCPDM		,	0	} ,		//... 2016.01.19
	{ "PRO_PurgeRcpStsDM"		,	_K_D_IO	,	PRO_PurgeRcpStsDM	,	0	} ,		//... 2016.01.19
	{ "SysLeakChkDM"			,	_K_D_IO	,	SysLeakChkDM		,	0	} ,		//... 2016.01.19
	//
	{ "LKMacroType1DM"			,	_K_D_IO	,	LKMacroType1DM		,	0	} ,
	{ "LKMacroType2DM"			,	_K_D_IO	,	LKMacroType2DM		,	0	} ,
	//...
	{ "PRMD_ZCtrl_RCP_SKIP"		,	_K_D_IO	,	PRMD_ZCtrl_RCP_SKIP	,	0	} ,
	{ "PRMD_ZCtrl_DLY_TIME"		,	_K_D_IO	,	PRMD_ZCtrl_DLY_TIME	,	0	} ,
	{ "PRMD_ZCtrl_RCP_TYPE"		,	_K_D_IO	,	PRMD_ZCtrl_RCP_TYPE	,	0	} ,
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum {eUnknownType, eProcess_In, eProcess_Out, eM_Process};
CProcessSequence *g_pProcSeq = NULL;
int g_nProcessType = eUnknownType;
//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	if(g_pProcSeq != NULL) msRet = g_pProcSeq->Main();
	return msRet;
}

//---------------------------------------------------------------------------------------
BOOL Program_Init_Code()
{
	BOOL bRet = TRUE;
	//--------------------------------------
	// 2005.10.07 for Checking System Exit
	SYSTEM_KILL_WITH_MESSAGE();

	// Initialize Function
	// _gINIT_LOG();	// 2006.01.12
	if(g_pProcSeq != NULL)
	{
		g_pProcSeq->Initialize();
		// printf("Process Sequence initialized\n");
	}
	else
	{
		bRet = FALSE;
		// printf("Process Sequence initialization is failed\n");
	}
	//--------------------------------------
	return bRet;
}

//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {

	//--------------------------------------
	// 2005.10.07 for Checking System Exit
	if(no == -1)
	{
		// Deinitialize Function
		delete g_pProcSeq;
		_gCLOSE_LOG();
	}
	//--------------------------------------
}

//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char szParam[256];
	char szValveType[40] = {"0",};
	char szProcessType[40] = {"0",};

	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	#ifdef _DEBUG
	printf("Argument = %s in Program_Enter_Code[%s]\n", szParam, APP_NAME);
	#endif // _DEBUG

	CTextParser tp(szParam);
	tp.GetWord(szValveType);

	if(strlen(szValveType) == 0 || szValveType[4] == '1')
	{
		g_pProcSeq = new CProcessSequence;			// 48 Channel Valve Recipe
		printf("Process Control Function[Type1] is created\n");
	}
	else if(szValveType[4] == '2' || szValveType[0] == 'E')
	{
		g_pProcSeq = new CProcessSequenceType2;		// 112 Channel Valve Recipe
		printf("Process Control Function[Type2] is created\n");
	}
	else
	{
		g_pProcSeq = new CProcessSequence;
		printf("Argument is invalid. So Process Control Function[Type1] is created\n", szValveType[4]);
	}

	tp.GetWord(szProcessType);

	if(STRCMP_L(szProcessType,"PROCESS_IN"))
	{
		g_pProcSeq->m_nProcessType = eProcess_In;
		printf("Process Type is Process_In \n");
	}
	else if(STRCMP_L(szProcessType,"PROCESS_OUT"))
	{
		g_pProcSeq->m_nProcessType = eProcess_Out;
		printf("Process Type is Process_out \n");
	}
	else if(STRCMP_L(szProcessType,"M_PROCESS"))
	{
		g_pProcSeq->m_nProcessType = eM_Process;
		printf("Process Type is M_Process \n");
	}
	else
	{
		g_pProcSeq->m_nProcessType = eUnknownType;
		printf("Argument is invalid, Process Type Unknown \n");
	}
}

} // End of [extern "C"]
//==========================================================================================
