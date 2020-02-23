// ProcessSequence.cpp: implementation of the CProcessSequence class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <string.h>

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
#include "ProcessSequence.h"
#include "TextParser.h"
#include "Timer.h"

#include "AlarmDefine.h"
#include "EnumDefine.h"

//-----------------------------------------------------------------------
// #include "TextLogDll.h"
extern void _LOG_RCP(const char *pFormat, ...);
extern void _LOG_PROC(const char *pFormat, ...);
extern void _LOG_RMAP(const char *pFormat, ...);
extern void _LOG_RCP_T(const char *pFormat,...);
extern void _LOG_ERR(const char *pFormat, ...);
#define _LOG_ERR_P(_szLog)	{_LOG_ERR(_szLog);printf("%s\n",_szLog);}

//------------------------------------------------------------------------------------------
#define APP_NAME	"ProcessControl"

// Thread Type Function
typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);

const int MAX_DOWNLOAD_RETRY		= 3;
const int MAX_EMPTY_MAILBOX_CHECK	= 100;
const int MAX_GET_MESSAGE_CHECK		= 100;
const double MAX_MAILBOX_SLEEP		= 0.1;

#define PROC_TIME_STEP_DELAY	0 // 0.24
#define PROC_TIME_CYC_DELAY		0 // 0.13

#define VSP_SERVICE_CODE		0x1E
#define VSP_CLASS_ID			0x2F
#define VSP_INSTANCE_ID			0x03

#define VRCP_PAGE0_ADDR			0
#define VRCP_PAGE1_ADDR			3000

// Parameter String Size
#define MAX_PARAM_SIZE		1000
//-----------------------------------------------------------------------
// Definition for 48 Channel Valve Recipe
// Valve Recipe Header Packet
#define VR_HDR_BASE			2
#define VRHD_MFC_SP_IDX		10
#define VRHD_MFC_VL_IDX		50
#define VRHD_RF_SP_IDX		56
#define VRHD_RF2_SP_IDX		66
#define VRHD_HTE_SP_IDX		68	//2014.05.22
#define STEP_TIME_SP_IDX    98

// Valve Recipe Step
#define	BASE_IDX			2
#define LINE_STEP_BASE		12
#define LINE_STEP_SIZE		8

//... 2014.10.13 Add Post Run Flag Check for Dry Clean Reset Count
#define DRYCLEAN_CNT_RESET_TIMEOUT	30		// Fixed (Unit: sec)

//-----------------------------------------------------------------------
enum { VLV_CLOSE, VLV_OPEN } ;
enum { RF_OFF, RF_ON};
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED, THREAD_STEP1, THREAD_STEP2};
enum { WAFER_ABSENT, WAFER_PRESENT, WAFER_UNKNOWN };

enum { VRCP_NORMAL, VRCP_ERROR};
enum { LeakCHK_OFF, LeakCHK_ON};     //2014.12.01
enum { OFFON_OFF  , OFFON_ON};

enum {eNotUse, eNorLeakchk, eGasLeakchk, eALDLeakchk};
// 2019.04.16
enum { Unuse, Local, DisConn, Standby, Maint, Uninit, EnablePM, CTCinUSE, CTCDis, CTCDis2, CTCEnaPM, CTCDisHW, CTCDIsHW2 } ;
//... 2016.04.03
#define MAX_LK_MACRO	2
#define MAX_MACRO_IO	2
typedef struct _LEAK_CHECK_MACRO
{
	int nMacroIndex;
	int nIOLeakChkSts;
	char *pLeakCheckCmd;
} LEAK_CHECK_MACRO;

#define NormalLeakCheck	{eNorLeakchk, ALECHK_NormDM,"LEAK_CHECK"}
#define GASLeakCheck	{eGasLeakchk, ALECHK_GASLnDM,"GAS_CHECK"}

LEAK_CHECK_MACRO	g_LeakCheckMacro[] = {NormalLeakCheck,GASLeakCheck};

//--------------------------------------------------------------------
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{"NotDefined", _K_D_IO, 0},
	//-----------------Fixed I/O End -------------------
	// 2007.01.31
	{"PRO_LotPreCtrlDM"		, _K_D_IO, 0},
	{"PRO_PostRunDM"		, _K_D_IO, 0},
	{"PRO_PostRunDM1"		, _K_D_IO, 0},
	{"PRO_PostRunDM2"		, _K_D_IO, 0},
	{"PRO_PostRunDM3"		, _K_D_IO, 0},
	{"PRO_PostRunDM4"		, _K_D_IO, 0},
	{"PRO_PostRunDM5"		, _K_D_IO, 0},
	{"PRO_PostRunDM6"		, _K_D_IO, 0},
	{"PRO_PostRunDM7"		, _K_D_IO, 0},
	{"PRO_PostRunDM8"		, _K_D_IO, 0},
	{"PRO_PostRunDM9"		, _K_D_IO, 0},
	{"PRO_PostCmptDM"		, _K_D_IO, 0},
	{"PRO_PostCmptDM1"		, _K_D_IO, 0},
	{"PRO_PostCmptDM2"		, _K_D_IO, 0},
	{"PRO_PostCmptDM3"		, _K_D_IO, 0},
	{"PRO_PostCmptDM4"		, _K_D_IO, 0},
	{"PRO_PostCmptDM5"		, _K_D_IO, 0},
	{"PRO_PostCmptDM6"		, _K_D_IO, 0},
	{"PRO_PostCmptDM7"		, _K_D_IO, 0},
	{"PRO_PostCmptDM8"		, _K_D_IO, 0},
	{"PRO_PostCmptDM9"		, _K_D_IO, 0},
	//2007.12.06
	{"PRO_ChamberStatus"	, _K_D_IO, 0},

	{"PRMD_Refill_Opt"		, _K_D_IO, 0},
	// 2005.10.19, Process Step Sync between Modules
	{"PRO_PMStepSyncDM"		, _K_D_IO, 0},
	{"PRO_PMStepWaitDM"		, _K_D_IO, 0},
	// 2006.05.12
	{"PRO_StdStpTmAM"		, _K_A_IO, 0},

	// 2006.01.24
	{"PRVD_DRYCWfrCnt"		, _K_D_IO, 0},
	{"PRVA_DRYCSrcFeed"		, _K_A_IO, 0},
	{"PRVA_DRYCThick"		, _K_A_IO, 0},
	// 2006.01.26
	{"PRO_PrvThickAM"		, _K_A_IO, 0},
	// 2006.06.14
	{"PRO_FlowControlDM"	, _K_D_IO, 0},
	// 2019.04.16
	{"CTC_CommStatus"	, _K_D_IO, 0},
	""
};
//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.11.15

// LOG_TYPE
enum { LTYPE_PRC, LTYPE_XFR, LTYPE_FNC, LTYPE_LEH, LTYPE_USER };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2014.10.13 MARS
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);
	WRITE_FUNCTION_EVENT(PMC_EVENT_LOG_CLI, _szString);
}

// 2014.10.13 MARS
void LogEvent(char *szEvent, char *szStatus, int nStep, char *szRcpName, char *szStepType, char *szData)
{
	char szRcpNameA[256]="-";
	int nLogType = LTYPE_PRC;
	int nRecipeLength = 0;

	do {
		if(STRCMP_L(szEvent , "Rf1PowerOnOff") || STRCMP_L(szEvent , "Rf2PowerOnOff"))	nLogType = LTYPE_FNC;

		if(nLogType == LTYPE_PRC)
		{
			if(0==nStep && STRCMP_L(szStatus , "Start"))
			{
				strcpy(szRcpNameA, szRcpName);
				nRecipeLength = strlen(szRcpNameA);
			}
			_MLOG("PRC %s %s %d %d %s %s %s", szEvent, szStatus, nStep, nRecipeLength, szRcpNameA, szStepType, szData);
		}
		else if(nLogType = LTYPE_FNC)
		{
			_MLOG("FNC %s %s %s", szEvent, szStatus, szData);
		}

	} while(0);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessSequence::CProcessSequence()
{
	m_nMaxValve = 48;			// Valve Recipe 48 Channel
	m_nMaxMfc = 20;				// MFC 20 Channel
	m_pStepTimer = new CTimer;
	// 2007.01.31
	m_bLotPreAutoDisable = FALSE;
	// 2007.02.09
	m_nOnlineAutoPurgeType = ONLINE_PURGE_SINGLE;
	m_nSimMode = SM_NOT_SIM;
	m_nRF_UseDM = 0;
	m_nHTE_UseDM = 0; // 2014.04.01
	// m_nVRCP_DnMethodDM => 0:Channel 1:SharedMemory 2:Enther Net
	m_nVRCP_DnMethodDM = 1;	// 2004.09.21, VRCP Download Method
	m_nVRCP_DnLdWait = 0;	// 0:NotUse 1:Use
	m_nProMfcSetCtrl = 0;	// 0:PLC 1:SW
	m_nProRfSetCtrl = 0;	// 0:PLC 1:SW
	m_nCVGIsoUseDM = 0;		// CVGIsoUseDM => 0:NotUse 1:Use
	m_nProHteSetCtrl = 0;	// 0:PLC 1:SW  2014.05.22

	// 2013.06.25
	m_nVRCPBufPageMax = 2;
	m_nQStepPreDownload = QSTEP_RD2;
	m_nQStepDownload = QSTEP_RD1;
	m_anVRCcpBufPageAddr[0] = VRCP_PAGE0_ADDR;
	m_anVRCcpBufPageAddr[1] = VRCP_PAGE1_ADDR;
	InitializeCriticalSection(&m_csQueueLock);
	m_bRcpInit = FALSE;
	m_nLastStep = 0;
	m_bRcpMapRead = FALSE;

	m_nStpGrpCCyc = 0;
	m_nPreTreatEnable = 0;		// PreTreat => 0:Disable 1:Enable

	int i;
	for(i=0; i<MAX_MFC; i++) { m_pnMFC_Scale[i] = 0; m_pnMFC_PhyMax[i] = 4095; }
	m_bMfcMonitor = FALSE;
	m_bMfcSetEnable = FALSE;
	m_bMfcMonEnable = FALSE;

	m_nRF_Scale = 1250;
	m_nRF_PhyMax = 4095;
	// 2005.07.22, for Step Time Interlock
	// 2006.02.02
	m_dbStepTimeTol = 0;		// Step Time Tolerance
	m_dbStepTimeTolRate = 0;		// 2006.07.20 by cskim
	m_bProcTimeInterlock = true;
	// 2014.05.22
	m_nHTE_Scale = 100;
	m_nHTE_PhyMax = 2047;


	m_pDynIO = new CDynamicIO;
	InitializeCriticalSection(&m_csLock);

	// 2006.05.17
	m_nVR_HDR_BASE			= VR_HDR_BASE;
	m_nVRHD_MFC_SP_IDX		= VRHD_MFC_SP_IDX;
	m_nVRHD_MFC_VL_IDX		= VRHD_MFC_VL_IDX;
	m_nVRHD_RF_SP_IDX		= VRHD_RF_SP_IDX;
	m_nVRHD_RF2_SP_IDX		= VRHD_RF2_SP_IDX;
	m_nVRHD_HTE_SP_IDX		= VRHD_HTE_SP_IDX;		// 2014.05.22
	m_nBASE_IDX				= BASE_IDX;
	m_nLINE_STEP_BASE		= LINE_STEP_BASE;
	m_nLINE_STEP_SIZE		= LINE_STEP_SIZE;

	// 2006.09.05
	m_nMfcControlMode = MFCCM_PARAM_TOL;
	m_bStepTimeCompenMaxAlarm = FALSE;		// 2007.01.19
	m_nStepTimeCompenMode = STCM_Normal;	// 2007.02.02
	m_nRecipeIndex = -1;					// 2007.01.25
	m_DummyWafer = FALSE;					// 2014.04.24
	m_nContDL_Thread = THREAD_STOP;			// 2014.09.11

	m_nRF1_OnCountBitNum = -1;				//... 2014.09.29
	m_nRF2_OnCountBitNum = -1;				//... 2014.09.29

	m_nFunctionSetCnt	= 0;
}

CProcessSequence::~CProcessSequence()
{
	delete m_pStepTimer;
	DeleteCriticalSection(&m_csLock);
	DeleteCriticalSection(&m_csQueueLock);							// 2013.06.25
	delete m_pDynIO;
	#ifdef _DEBUG
	printf("Process Sequence Instance is deleted\n");
	#endif // _DEBUG
}

void CProcessSequence::Initialize()
{
	int nIOStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &nIOStatus ) != 0 )		// 0:NotLoading
		m_nSimMode = READ_DIGITAL(SimModeDM, &nIOStatus);
	else m_nSimMode = SM_SIMUL;

	m_nRF_UseDM = READ_DIGITAL(RF_UseDM, &nIOStatus);
	m_nHTE_UseDM = READ_DIGITAL(HTE_UseDM, &nIOStatus);             // 2014.04.01
	m_nCVGIsoUseDM = READ_DIGITAL(CVGIsoUseDM, &nIOStatus);			// CVGIsoUseDM => 0:NotUse 1:Use

	// Initialize and Read PMC Configration
	int i;
	for(i=0; i<5; i++)
	{
		m_SrcFeedMfc[i].nMfcN = 0;
		m_SrcFeedMfc[i].nValveN1 = -1;
		m_SrcFeedMfc[i].nValveN2 = -1;
		m_SrcFeedMfc[i].nValveN3 = -1;
		m_SrcFeedMfc[i].fDrift = 0;
	}
	m_nValveTimeRes = VTS_10MS;
	m_bZM_PreLoadPos = TRUE;
	m_bAbortNotif = TRUE;
	m_bFastRoughBeforeProc = FALSE;
	m_bFastRoughAfterProc = TRUE;	// 2005.06.10 by cskim, Abort Gas Before Process
	m_bAbortGasBeforeProc = FALSE;
	m_Clean_nACVControl = 0;		// 0:No 1:Clean 2:CleanAuto
	m_Clean_bACVCheck = FALSE;
	m_bPostSkipEvent = FALSE;
	m_nStepTimeMode = 0;			// 0:TotalTime 1:StepTime 2:StepTime*2
	m_nLotPreFailOption = 0;		// 0:LotAbort 1:LotContinuous
	m_bProcFailInterlock = false;
	// 2006.02.13
	m_nPrevAmountOption = 0;		// 0:AllRecipe 1:OnlyProcessRecipe

	// 2013.06.02
	m_bStablePurgeAfterProc = FALSE;

	//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
	for(i = 0; i < MAX_INTLK_VALVE_LIST; i++)
	{
		m_DBOpenIntlkVlv[i].nValveNo_A = -1;
		m_DBOpenIntlkVlv[i].nValveNo_B = -1;
	}

	//... 2017.12.13 initialize member variable
	for(i = 0; i < MAX_ALD_VALVE_LIST; i++)
	{
		m_ALDVlvList[i].nALDOpenCnt = 0;
		m_ALDVlvList[i].nIOEnum = 0;
		m_ALDVlvList[i].nVlvNum = 0;
		m_ALDVlvList[i].bALDVlvSumReady = FALSE;
		m_ALDVlvList[i].nStepFinalVlvSts = 0;
		m_ALDVlvList[i].szPrmIO = 0;
		m_ALDVlvList[i].nRealVlvNum = 0;
	}
	//...

	ReadPmcConfig();
	m_pDynIO->InitIOList(g_pIOList, false);
	// 2013.06.25
	if(5 == m_nVRCPBufPageMax)
	{
		m_nQStepPreDownload = QSTEP_NEW;
		m_nQStepDownload = QSTEP_RD4;

		// 2013.08.19
		m_anVRCcpBufPageAddr[0] = VRCP_PAGE0_ADDR;
		m_anVRCcpBufPageAddr[1] = VRCP_PAGE0_ADDR + 200;
		m_anVRCcpBufPageAddr[2] = VRCP_PAGE0_ADDR + 400;
		m_anVRCcpBufPageAddr[3] = VRCP_PAGE0_ADDR + 600;
		m_anVRCcpBufPageAddr[4] = VRCP_PAGE0_ADDR + 800;
	}

	//... 2015.01.02 by wjkim, Add Lot Pre Skip Case
	memset(m_szOldLotPreRcpName, 0, sizeof(m_szOldLotPreRcpName));

	//... 2017.03.07
	m_nZCtrl_RCPType		   = RT_Unknown;
	m_nZCtrl_PrcStartDelayTime = 0;
	m_nZCtrl_RCPSkipOption	   = OFFON_OFF;
}

Module_Status CProcessSequence::Main()
{
	Module_Status msRet = SYS_ERROR;
	BOOL bIsRcpAppended;
	int nIOStatus = 0;
	//... 2016.04.01 For Test
	int nFIdx1 = 0, nFIdx2 = 0;
	int nFSts1 = 0, nFSts2 = 0;

	if(m_bRcpMapRead != TRUE)
	{
		printf("-----> Read RecipeMapData\n");
		m_bRcpMapRead = ReadRcpMapData();
		if(FALSE == m_bRcpMapRead)
		{
			ALARM_POST(ALARM_SW_FUNC_ERROR_OCCURED);
			printf("---> RecipeMap Data Reading Failed in %s\n", APP_NAME);
			_LOG_ERR("-----> Recipe Map Data Reading Failed");
			return SYS_ABORTED;
		}
	}

	// 2004.04.12 by cskim
	if(m_bRcpInit != TRUE)
	{
		_LOG_PROC("Recipe Data Initialize");
		printf("-----> Initialize Recipe Data\n");
		InitRecipe();
		m_bRcpInit = TRUE;
		_LOG_RCP_T("-----> Recipe Data Start from Kornic Engine");
	}

	//////////////////////////////////////////////////////////////////////////
	if ( Recipe_Reading( &bIsRcpAppended ) != SYS_SUCCESS )
	{
		LogRecipeInform();
		m_bRcpInit = FALSE;
		printf("-----> Recipe Reading failed %s\n", APP_NAME);
		_LOG_ERR("-----> Recipe Data Reading Failed");

		//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
		if(m_bDBOpenIntlkVlv)
		{
			//... 2015.06.06 Init Recipe Information (Rcp Name, Step, Progress)
			WRITE_STRING(PRO_RcpNameSM,		RECIPE_NAME,	&nIOStatus);
			WRITE_STRING(PRO_StepNameSM,	"",				&nIOStatus);
			WRITE_ANALOG(PRO_CurStepAM,		(double) 0,		&nIOStatus);
			WRITE_ANALOG(PRO_TotalStepAM,	(double) 0,		&nIOStatus);
			WRITE_ANALOG(PRO_TotalTimeAM,	(double) 0,		&nIOStatus);

			//... 2015.06.06 Add Post Skip Event
			if(RT_ProcRcp == m_RcpHeader.nRecipeType && TRUE == m_bPostSkipEvent)
			{
				if(WRITE_UPLOAD_MESSAGE("POSTSKIP"))
				{
					printf(">>>>> Write Upload Message :POSTSKIP:write success <<<<<\n");
					_LOG_PROC("-----> Write Upload Message :POSTSKIP:write success");
				}
				else
				{
					printf(">>>>> Write Upload Message :POSTSKIP:write failed !!! <<<<<\n");
					_LOG_PROC("-----> Write Upload Message :POSTSKIP:write failed !!!");
				}
			}

			return SYS_ERROR;	//... Process Failed
		}

		return SYS_ABORTED;
	}

	if ( bIsRcpAppended )
	{
		printf("-----> Recipe is appended continuously %s\n", APP_NAME);
		return SYS_SUCCESS;
	}
	//////////////////////////////////////////////////////////////////////////
	//... 2016.04.01 Add Check Other side Process When Manual Cycle Clean is Running
	if(RT_CleanRcp == m_RcpHeader.nRecipeType && m_nProcessType == eM_Process)
	{
		nFIdx1 = _FIND_FROM_STRING(4,"PROCESS_OUT");
		nFIdx2 = _FIND_FROM_STRING(4,"PROCESS_IN");
		nFSts1 = _dREAD_FUNCTION(nFIdx1);
		nFSts2 = _dREAD_FUNCTION(nFIdx2);
		printf("M Process Fail Because Process Out,In is Running\n");
		if(nFSts1 == SYS_RUNNING || nFSts2 == SYS_RUNNING )
		{
			m_bRcpInit = FALSE;			//... Fixed Memory UnInit Status
			m_nRecipeIndex = -1;		//... Init Recipe Index
			return SYS_SUCCESS;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//... 2014.10.23 Add Check Other Process Running
	if( RT_LotPreRcp == m_RcpHeader.nRecipeType ||
		RT_LotPostRcp == m_RcpHeader.nRecipeType ||
		RT_IdleRcp == m_RcpHeader.nRecipeType)
	{
		if(RUNSTS_Running == READ_DIGITAL(PRO_RunStsDM, &nIOStatus))
		{
			ALARM_POST(ALARM_PM_ALREADY_PROCESS_RUNNING);
			_LOG_PROC("-----> LotPre or LotPost Recipe Skipped [RcpType:%d]", m_RcpHeader.nRecipeType);

			//////////////////////////////////////////////////////////////////////////
			//... 2015.05.18 Fixed Bug Recipe Memory Init Skip
			m_bRcpInit = FALSE;			//... Fixed Memory UnInit Status
			m_nRecipeIndex = -1;		//... Init Recipe Index
			//////////////////////////////////////////////////////////////////////////

			return SYS_SUCCESS;
		}
	}

	//... 2015.01.02 by wjkim, Add Lot Pre Skip Case
	if(RT_LotPreRcp == m_RcpHeader.nRecipeType)
	{
		if(IsLotPreSkipCase())
		{
			_LOG_PROC("[INFO] LotPre Skipped by Timer");

			//////////////////////////////////////////////////////////////////////////
			//... 2015.05.18 Fixed Bug Recipe Memory Init Skip
			m_bRcpInit = FALSE;			//... Fixed Memory UnInit Status
			m_nRecipeIndex = -1;		//... Init Recipe Index
			//////////////////////////////////////////////////////////////////////////

			return SYS_SUCCESS;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	for(int i = 0; i < MAX_ALD_VALVE_LIST; i++)
	{
		m_ALDVlvList[i].nALDOpenCnt = 0;
	}

	// 2015.06.26
	switch(m_RcpHeader.nRecipeType)
	{
	case RT_PreProRcp:		strcpy(m_szEventID, "PreProcess"); break;
	case RT_ProcRcp:		strcpy(m_szEventID, "Process"); break;
	case RT_PostProRcp:		strcpy(m_szEventID, "PostProcess"); break;
	case RT_IdleRcp:		strcpy(m_szEventID, "IdleRun"); break;
	case RT_CleanRcp:
		if(m_pDynIO->dREAD_DIGITAL(CTC_CommStatus, &nIOStatus) < CTCinUSE)
		{
			strcpy(m_szEventID, "Clean");
		}
		if(m_pDynIO->dREAD_DIGITAL(CTC_CommStatus, &nIOStatus) >= CTCinUSE)
		{
			strcpy(m_szEventID, "PostProcess");
		}
		else
		{
		}
		break;
	case RT_PurgeRcp:		strcpy(m_szEventID, "Purge"); break;
	case RT_LotPreRcp:		strcpy(m_szEventID, "PreLotRun"); break;
	case RT_LotPostRcp:		strcpy(m_szEventID, "PostLotRun"); break;
	default:				strcpy(m_szEventID, "Unknown"); break;
	}

	LogEvent(m_szEventID, "Start", 0, m_RcpHeader.szRecipeName, "", "");  // 2015.06.26

	//... 2015.01.02 by wjkim, Add Lot Pre Skip Case
	if(RT_ProcRcp == m_RcpHeader.nRecipeType
		|| (enOFFON_ON == READ_DIGITAL(SysLeakChkDM,&nIOStatus) && RT_CleanRcp == m_RcpHeader.nRecipeType))  //... 2016.08.16
	{
		RUN_SET_FUNCTION(LOTPRE_SKIP_TIMER, "TIMER_STOP");
	}

	//////////////////////////////////////////////////////////////////////////
	switch(m_nRecipeIndex)
	{
	case 1:
	case 6:	msRet = Main_LeakCheck(); break;
	default: msRet = Main_Process(); break;
	}
	//////////////////////////////////////////////////////////////////////////

	//... 2015.01.02 by wjkim, Add Lot Pre Skip Case
	if(RT_ProcRcp == m_RcpHeader.nRecipeType
		|| (enOFFON_ON == READ_DIGITAL(SysLeakChkDM,&nIOStatus) && RT_CleanRcp == m_RcpHeader.nRecipeType))  //... 2016.08.16
	{
		//... 2016.05.14 Lot Pre Skip Time Start After System Leak Check
		WRITE_DIGITAL(SysLeakChkDM,enOFFON_OFF,&nIOStatus);
		RUN_SET_FUNCTION(LOTPRE_SKIP_TIMER, "TIMER_START");
	}

	m_nRecipeIndex = -1;

	LogEvent(m_szEventID, "End", 0, "", "", "");		// 2015.06.26
	return msRet;
}

Module_Status CProcessSequence::Main_Process()
{
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	char Buffer[256] = {0,};
	char Buffer2[256] = {0,};

	printf("----------------------------------------------------------\n");
	_LOG_PROC("----------------------------------------------------------");
	printf("Entered %s\n", APP_NAME);

	do {
		if(m_nSimMode == SM_SIMUL) _LOG_PROC("< Process Simulation Started(Name:%s, Type:%d) >",
				m_RcpHeader.szRecipeName, m_RcpHeader.nRecipeType);
		else _LOG_PROC("< Process Started (Name: %s, Type:%d) >",
				m_RcpHeader.szRecipeName, m_RcpHeader.nRecipeType);

		WRITE_STRING(PRO_RcpNameSM, m_RcpHeader.szRecipeName, &nIOStatus);
		WRITE_DIGITAL(PRO_RcpTypeDM, m_RcpHeader.nRecipeType, &nIOStatus);
		WRITE_DIGITAL(PRO_RunStsDM,	RUNSTS_Running, &nIOStatus);
		WRITE_DIGITAL(PRO_StopCtrlDM, enOFFON_OFF, &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		//... 2017.03.07 Parameter Reading
		m_nZCtrl_RCPType		   = READ_DIGITAL(PRMD_ZCtrl_RCP_TYPE,&nIOStatus);
		m_nZCtrl_PrcStartDelayTime = READ_DIGITAL(PRMD_ZCtrl_DLY_TIME,&nIOStatus);
		m_nZCtrl_RCPSkipOption	   = READ_DIGITAL(PRMD_ZCtrl_RCP_SKIP,&nIOStatus);

		//... 2016.01.19 PRO_PurgeRcpStsDM => 0:Stop 1:Running
		if(m_RcpHeader.nRecipeType == RT_PurgeRcp)
			WRITE_DIGITAL(PRO_PurgeRcpStsDM, RUNSTS_Running, &nIOStatus);


		if(m_RcpHeader.nRecipeType == RT_IdleRcp)
			WRITE_DIGITAL(PRO_IdleRcpStsDM, 1, &nIOStatus);

		WRITE_DIGITAL(PRO_ProcStsDM, 1, &nIOStatus);

		// Recipe Data Information Display
		LogRecipeInform();

		if(CheckRecipeData() == FALSE)
		{
			printf("-----> Invalid Recipe Data in %s\n", APP_NAME);
			break;
		}

		switch(m_RcpHeader.nRecipeType)
		{
		case RT_PreProRcp:		msRet = RunPreProcess(); break;
		case RT_ProcRcp:		msRet = RunProcess(); break;
		case RT_PostProRcp:		msRet = RunPostProcess(); break;
		case RT_IdleRcp:		msRet = RunIdleRcp(); break;
		case RT_CleanRcp:		msRet = RunCleanRcp(); break;
		case RT_PurgeRcp:		msRet = RunPurgeRcp(); break;
		case RT_LotPreRcp:		msRet = RunLotPreRcp(); break;
		case RT_LotPostRcp:		msRet = RunLotPostRcp(); break;
		default:
			char szAlarm[256];
			char szExtAlarm[256];
			ALARM_MESSAGE_GET(ALARM_INVALID_RECIPE_DATA, szAlarm);
			sprintf(szExtAlarm, "%s(Recipe Type Is Invalid => %d)", szAlarm, m_RcpHeader.nRecipeType);
			ALARM_MESSAGE_SET(ALARM_INVALID_RECIPE_DATA, szExtAlarm);
			ALARM_POST(ALARM_INVALID_RECIPE_DATA);
			printf("---> Invalid Recipe Type [%d] in %s !\n", m_RcpHeader.nRecipeType, APP_NAME);
			_LOG_ERR("-----> Invalid Recipe Type [%d]", m_RcpHeader.nRecipeType);
			msRet = SYS_ERROR;
			break;
		}
	} while(0);

	// 2005.04.18 by cskim
	_LOG_PROC("[UPEH-END] StopStepTimer Started");
	StopStepTimer();
	RUN_FUNCTION_FREE(TIME_MONITOR, "STOP");
	m_bRcpInit = FALSE;
	// 2015.01.25
	RUN_SET_FUNCTION_FREE(ALDIOMon,"ALD_VLV_USESUM");

	//... Idle Recipe 제외 하고 나머지 Recipe Type 성공시 Check 한다. 가성 알람 방지
	if(msRet == SYS_SUCCESS && m_RcpHeader.nRecipeType != RT_IdleRcp)
	{
		//////////////////////////////////////////////////////////////////////////
		//... 2017.12.13 Check AlD Valve Count
		for(int i = 0; i < MAX_ALD_VALVE_LIST; i++)
		{
			if(_dREAD_DIGITAL(m_ALDVlvList[i].szPrmIO,&nIOStatus) != ON) continue;
			if(m_ALDVlvList[i].nALDOpenCnt > 0)
			{
				if((m_ALDVlvList[i].nALDOpenCnt - 1) > _dREAD_ANALOG(m_ALDVlvList[i].nIOEnum,&nIOStatus))
				{
					WRITE_DIGITAL(SW_GbIntlckDM, OFFON_ON,&nIOStatus);

					ALARM_MESSAGE_GET(ALARM_ALD_VALVE_COUNT_FAIL, Buffer);
					sprintf(Buffer2," %d Valve: Recipe Set Cnt: %d Real Cnt: %d",m_ALDVlvList[i].nRealVlvNum, m_ALDVlvList[i].nALDOpenCnt - 1, _dREAD_ANALOG(m_ALDVlvList[i].nIOEnum,&nIOStatus));
					strcat(Buffer, Buffer2);
					ALARM_MESSAGE_SET(ALARM_ALD_VALVE_COUNT_FAIL, Buffer);

					ALARM_POST(ALARM_ALD_VALVE_COUNT_FAIL);
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

	do
	{
		_LOG_PROC("[UPEH-END] CHUCK_CONTROL[HOME 2] Started");
		RUN_SET_FUNCTION_FREE(CHUCK_CONTROL, "HOME 2");
		if(SYS_SUCCESS != RUN_WAIT_FUNCTION(CHUCK_CONTROL))
		{
			_LOG_ERR("-----> Z-Motion Control Waiting Error !");
			break;
		}
		_LOG_PROC("[UPEH-END] CHUCK_CONTROL[HOME 2] End");

	}while(0);
	// ACV_UseDM => 0:NotUse 1:Use
	if(READ_DIGITAL(ACV_UseDM, &nIOStatus) == 1) SetACVMode(RT_Unknown);
	if(SYS_ABORTED == msRet || SYS_ERROR == msRet)
	{
		if(m_bAbortNotif) ALARM_POST(ALARM_PROCESS_ABORTED);
		printf("---> Process is aborted in %s\n", APP_NAME);
		_LOG_PROC("< Process Aborted >");

		/*
		// 2005.05.04 by cskim
		if( (m_RcpHeader.nRecipeType == RT_LotPreRcp
				|| m_RcpHeader.nRecipeType == RT_LotPostRcp)
				&& m_nLotPreFailOption == 0) msRet = SYS_ABORTED;
		// if(m_RcpHeader.nRecipeType <= 0) msRet = SYS_ABORTED;
		*/

		//... 2015.02.06 Lot Pre Abort 시 해당 PM만 Disable 처리 (Lot Abort 방지)
		//mgsong modify : 이 경우는 Abort없다.
		if(m_RcpHeader.nRecipeType == RT_LotPreRcp || m_RcpHeader.nRecipeType == RT_LotPostRcp)
			msRet = SYS_ERROR;
	}
	else
	{
		printf("---> Process finished in %s\n", APP_NAME);
		_LOG_PROC("< Process Finished >");
	}

	// PRO_ProcStsDM => 0:Idle 1:Running 2:Aborted 3:SeqErr
	if(msRet == SYS_SUCCESS) WRITE_DIGITAL(PRO_ProcStsDM, 0, &nIOStatus);
	else
	{
		WRITE_DIGITAL(PRO_ProcStsDM, 2, &nIOStatus);
		if(m_bProcFailInterlock)
		{	//... Check Abnormal Failed
			if( READ_DIGITAL(PRO_RunStsDM, &nIOStatus) == RUNSTS_Running &&
				READ_DIGITAL(SW_GbIntlckDM, &nIOStatus) == 0 &&
				READ_DIGITAL(PRO_StopCtrlDM, &nIOStatus) == enOFFON_OFF)
			{
				WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
				Sleep(5000);

				SetSwapPurge();
			}
		}
	}

	// PRO_IdleRcpStsDM => 0:Stop 1:Running
	if(m_RcpHeader.nRecipeType == RT_IdleRcp)
		WRITE_DIGITAL(PRO_IdleRcpStsDM, 0, &nIOStatus);

	//... 2016.01.19 PRO_PurgeRcpStsDM => 0:Stop 1:Running
	if(m_RcpHeader.nRecipeType == RT_PurgeRcp)
		WRITE_DIGITAL(PRO_PurgeRcpStsDM, RUNSTS_Stop, &nIOStatus);

	// PRO_RunStsDM => 0:Stop 1:Running
	WRITE_DIGITAL(PRO_RunStsDM,		RUNSTS_Stop, &nIOStatus);
	WRITE_DIGITAL(PRO_StopCtrlDM,	enOFFON_OFF, &nIOStatus);

	// 2005.10.19, Process Step Sync between Modules
	m_pDynIO->dWRITE_DIGITAL(PRO_PMStepSyncDM, 0, &nIOStatus);
	m_pDynIO->dWRITE_DIGITAL(PRO_PMStepWaitDM, 0, &nIOStatus);

	// 2007.01.25
	m_nRecipeIndex = -1;
	printf("Leaved %s\n", APP_NAME);
	printf("----------------------------------------------------------\n");
	_LOG_PROC("----------------------------------------------------------");
	return msRet;
}

Module_Status CProcessSequence::Main_LeakCheck()
{
	Module_Status msRet = SYS_ERROR;
	BOOL bIOStatus;

	// 2007.01.25
	if(m_nRecipeIndex >= 0)
	{
		if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM+m_nRecipeIndex, &bIOStatus) != POST_RUN_START) return SYS_SUCCESS;
	}
	msRet = RUN_FUNCTION(VACUUM_CONTROL, "LEAK_CHECK");
	if(msRet != SYS_SUCCESS) msRet = SYS_ERROR;
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
void CProcessSequence::Init_Process()
{
	int i;
	int nIOStatus;

	MakeVRCPStepHeader();
	// 2013.08.19
	m_bFirstStepDownload = FALSE;

	// 2005.04.18 by cskim
	WRITE_ANALOG(PRO_StepTimeAM, 0, &nIOStatus);
	WRITE_ANALOG(PRO_StpTotTmAM, 0, &nIOStatus);
	WRITE_ANALOG(PRO_StpCurTmAM, 0, &nIOStatus);

	// 2005.10.19, Process Step Sync between Modules
	m_pDynIO->dWRITE_DIGITAL(PRO_PMStepSyncDM, 0, &nIOStatus);
	m_pDynIO->dWRITE_DIGITAL(PRO_PMStepWaitDM, 0, &nIOStatus);

	m_bProcTimeInterlock = true;
	m_bIdleStopEnable = FALSE;
	m_nMainStepCount = 0;

	for(i=0; i<MAX_MAIN_STEP; i++)
	{
		m_pnMainStep[i] = 0;
		m_pdbMainStepTime[i] = 0;
	}

	for(i=0; i<m_RcpHeader.nTotalStep; i++)
	{
		if(m_RcpStep[i].bIsMain)
		{
			if(m_nMainStepCount < MAX_MAIN_STEP)
			{
				m_pnMainStep[m_nMainStepCount] = i+1;
				m_nMainStepCount ++;
			}
		}

		// 2005.10.19, Process Step Sync between Modules
		if(m_RcpStep[i].nStepFlow_Option == SFL_WAIT_STEPSYNC)
		{
			if(m_pDynIO->IOValid(PRO_PMStepSyncDM) && m_pDynIO->IOValid(PRO_PMStepWaitDM))
					m_pDynIO->dWRITE_DIGITAL(PRO_PMStepSyncDM, 1, &nIOStatus);
			else
			{
				ALARM_POST(ALARM_PM_STEP_SYNC_CAN_NOT_BE_USED);
				_LOG_ERR("-----> PRO_PMStepSyncDM or PRO_PMStepWaitDM channel do not exist but StepFlowOption(4) is used !");
				m_RcpStep[i].nStepFlow_Option = SFL_NORMAL_CYC;
			}
		}
	}

	SetRecipeInform();
	InitStepTimer();
	// Calculate Process Total Time
	WRITE_ANALOG(PRO_TotalTimeAM, GetProcTotalTime(), &nIOStatus);
	RUN_FUNCTION(TIME_MONITOR, "INIT");

	if(m_nSimMode == SM_NOT_SIM || m_nSimMode == SM_PLC_TEST)	ReadyPLC();
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CProcessSequence::AbortAllFunc()
{
	int nIOStatus;
	// 2014.05.22
	StopSubFunction();

	// Stop Monitoring
	RUN_FUNCTION_ABORT(MFC_CONTROL);
	RUN_FUNCTION_ABORT(PRS_MONITOR);
	RUN_FUNCTION_ABORT(PROCBGRUN_CONTROL);
	// Heater Temp Control Stop
	RUN_FUNCTION(HTR_CONTROL, "STOP");

	if(m_nRF_UseDM == 1)
	{
		RUN_FUNCTION_ABORT(RF_CONTROL);
		_LOG_PROC("-----> Killed RF Monitoring Function");
		//2008.09.29
		RUN_FUNCTION_ABORT(RF_CONTROL2);
		_LOG_PROC("-----> Killed RF2 Monitoring Function");
	}

	if(m_nHTE_UseDM == 1)  //2014.04.01
	{
		RUN_FUNCTION(HT_ExchgControl, "STOP"); // 2014.04.01
		_LOG_PROC("-----> Killed Heat Exchange Monitoring Function");
	}

	RUN_FUNCTION_FREE(TIME_MONITOR, "PAUSE");
	if(m_nSimMode == SM_SIMUL)
	{
		RUN_FUNCTION_FREE(PM_SIMULATOR, "STOP_VRCP");
	}

	// Stop Valve Recipe Confirm for Aborting
	WRITE_DIGITAL(VRCP_ResumeDO, 0, &nIOStatus);	// 0:Off 1:On
}

void CProcessSequence::StopProcess()
{
	int nIOStatus;
	m_nContDL_Thread = THREAD_STOP;							// 2013.06.25
	WRITE_DIGITAL(VRCP_StaStpDO, 0, &nIOStatus);	// 0:Stop 1:Start
	WRITE_DIGITAL(PRO_IsMainDM, 0, &nIOStatus);		// 0:NoMain 1:Main
}

void CProcessSequence::RunThreadFunc(LPTHREAD_START_ROUTINE threadFunc, LPVOID lParam)
{
	HANDLE	hThread;
	DWORD	dwThreadId;
	hThread = CreateThread(0, 0, threadFunc, lParam, 0, &dwThreadId);
	CloseHandle(hThread);
}

//---------------------------------------------------------------------------------------
// 2004.08.02 by cskim, 'Preventive Maintance Sequence' Added
// 2005.12.14 Modified by cskim

BOOL CProcessSequence::CheckFeedingValve(int nVlvN, unsigned char *pData)
{
	BOOL bRet = FALSE;
	byte byByteN;
	byte byBitN;
	int nIdx;
	byte byMask;

	if(nVlvN < 0) return TRUE;
	byByteN = nVlvN / 8;
	byBitN = nVlvN % 8;
	switch(byByteN)
	{
	case 0: nIdx = 1; break;
	case 1: nIdx = 0; break;
	case 2: nIdx = 3; break;
	case 3: nIdx = 2; break;
	case 4: nIdx = 5; break;
	case 5: nIdx = 4; break;
	case 6: nIdx = 7; break;
	case 7: nIdx = 6; break;
	case 8: nIdx = 9; break;
	case 9: nIdx = 8; break;
	case 10: nIdx = 11; break;
	case 11: nIdx = 10; break;
	case 12: nIdx = 13; break;
	case 13: nIdx = 12; break;
	}
	byMask = (byte)0x01 << byBitN;
	if((pData[nIdx] & byMask) != 0) bRet = TRUE;

	return bRet;
}

double CProcessSequence::GetSourceFeeding(int nStep)
{
	double dbSourceFeedingTotal = 0;
	int nIdx, i, j, k, n;
	unsigned char *pData;
	double dbSrcFeedingTime = 0;
	double dbSrcFeedingSccm = 0;
	int nStepCycle;
	int nTotalLineStep;
	int nMFC_n;
	BOOL bSrcFdValve1, bSrcFdValve2, bSrcFdValve3;

	nIdx = nStep - 1;
	for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++)
	{
		nStepCycle = m_RcpStep[nIdx].ppVR_Step[i][8]*256 + m_RcpStep[nIdx].ppVR_Step[i][9];
		nTotalLineStep = m_RcpStep[nIdx].ppVR_Step[i][11];
		for(j=0; j<nStepCycle; j++)
		{
			for(k=0; k<nTotalLineStep; k++)
			{
				for(n=0; n<5; n++)
				{
					nMFC_n = m_SrcFeedMfc[n].nMfcN;
					if(nMFC_n > 0)
					{
						pData = m_RcpStep[nIdx].ppVR_Step[i]+m_nLINE_STEP_BASE+k*m_nLINE_STEP_SIZE;
						bSrcFdValve1 = CheckFeedingValve(m_SrcFeedMfc[n].nValveN1, pData+2);
						bSrcFdValve2 = CheckFeedingValve(m_SrcFeedMfc[n].nValveN2, pData+2);
						bSrcFdValve3 = CheckFeedingValve(m_SrcFeedMfc[n].nValveN3, pData+2);
						if(bSrcFdValve1 && bSrcFdValve2 && bSrcFdValve3)
						{
							if(m_nValveTimeRes == VTS_10MS)
								dbSrcFeedingTime = ((double) (pData[0]*256 + pData[1])) /100;
							else dbSrcFeedingTime = ((double) (pData[0]*256 + pData[1])) /10;
							dbSrcFeedingSccm = (double) m_RcpStep[nIdx].pdbMFC_Sp[nMFC_n-1];
							dbSourceFeedingTotal += (dbSrcFeedingSccm * dbSrcFeedingTime / 60 + m_SrcFeedMfc[n].fDrift);
						}
						#ifdef _DEBUG
						printf("-----> Step[%d,%d] Source Feeding Valve[%d,%d,%d] Mfc[%7.2f] Time[%7.2f]\n",
							nStep, k+1, bSrcFdValve1, bSrcFdValve2, bSrcFdValve3, dbSrcFeedingSccm, dbSrcFeedingTime);
						#endif
					}
				}
			}
		}
	}
	#ifdef _DEBUG
	printf("-----> Step[%d] Source Feeding Total = %7.2f\n", nStep, dbSourceFeedingTotal);
	#endif
	return dbSourceFeedingTotal;
}

double CProcessSequence::GetValveCycleAcc(int nStep)
{
	double dbValveCycleAcc = 0;
	int nIdx, i;
	int nStepCycle;

	nIdx = nStep - 1;
	for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++)
	{
		nStepCycle = m_RcpStep[nIdx].ppVR_Step[i][8]*256 + m_RcpStep[nIdx].ppVR_Step[i][9];
		dbValveCycleAcc	+= nStepCycle;
	}
	return dbValveCycleAcc;
}

void CProcessSequence::AccPrvMaintItem(int nStep, int nStepCycle)
{
	m_dbVlvCycSum += ((double) nStepCycle * GetValveCycleAcc(nStep));
	m_dbSrcFeedingSum += ((double) nStepCycle * GetSourceFeeding(nStep));
}

//---------------------------------------------------------------------------------------
// 2004.09.13 by cskim, Total Time of Process

double CProcessSequence::GetProcStepTime(int nStep)
{
	double dbProcStepTime = 0;
	int nIdx, i, j, k;
	unsigned char *pData;
	double dbTimeAcc = 0;
	int nStepCycle;
	int nTotalLineStep;

	nIdx = nStep - 1;

	if(m_RcpStep[nIdx].nStepFlow_Option != 0) return 0;

	for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++)
	{
		nStepCycle		= m_RcpStep[nIdx].ppVR_Step[i][8]*256 + m_RcpStep[nIdx].ppVR_Step[i][9];
		nTotalLineStep	= m_RcpStep[nIdx].ppVR_Step[i][11];

		for(j = 0; j < nStepCycle; j++)
		{
			for(k = 0; k < nTotalLineStep; k++)
			{
				pData = m_RcpStep[nIdx].ppVR_Step[i]+m_nLINE_STEP_BASE+k*m_nLINE_STEP_SIZE;
				if(m_nValveTimeRes == VTS_10MS)		dbTimeAcc = ((double) (pData[0]*256 + pData[1])) /100;
				else								dbTimeAcc = ((double) (pData[0]*256 + pData[1])) /10;	 // if m_nValveTimeRes == VTS_100ms

				dbProcStepTime += dbTimeAcc;
			}
		}
	}

	dbProcStepTime = dbProcStepTime * m_RcpStep[nIdx].nVR_CycleCount + PROC_TIME_CYC_DELAY * (m_RcpStep[nIdx].nVR_CycleCount-1);
	dbProcStepTime += PROC_TIME_STEP_DELAY;

	return dbProcStepTime;
}

//---------------------------------------------------------------------------------------
// 2004.09.13 by cskim, Total Time of Process
double CProcessSequence::GetProcStepTimeForPLC(int nStep)
{
	double dbProcStepTime = 0;
	int nIdx, i, j, k;
	unsigned char *pData;
	double dbTimeAcc = 0;
	int nStepCycle;
	int nTotalLineStep;

	nIdx = nStep;

	if(m_RcpStep[nIdx].nStepFlow_Option != 0) return 0;

	for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++)
	{
		nStepCycle = m_RcpStep[nIdx].ppVR_Step[i][8]*256 + m_RcpStep[nIdx].ppVR_Step[i][9];
		nTotalLineStep = m_RcpStep[nIdx].ppVR_Step[i][11];
		for(j=0; j<nStepCycle; j++)
		{
			for(k=0; k<nTotalLineStep; k++)
			{
				pData = m_RcpStep[nIdx].ppVR_Step[i]+m_nLINE_STEP_BASE+k*m_nLINE_STEP_SIZE;
				dbTimeAcc = ((double) (pData[0]*256 + pData[1])) / 100;
				dbProcStepTime += dbTimeAcc;
			}
		}
	}

	dbProcStepTime = dbProcStepTime * m_RcpStep[nIdx].nVR_CycleCount;

	return dbProcStepTime;
}
double CProcessSequence::GetNormalProcTime()
{
	double dbProcTotalTime = 0;
	int i;
	for(i=1; i<=m_RcpHeader.nTotalStep; i++)
	{
		dbProcTotalTime += GetProcStepTime(i);
	}
	return dbProcTotalTime;
}

double CProcessSequence::GetGrpCycProcTime()
{
	double dbProcTotalTime = 0;
	int i, j;
	for(i=1; i<m_RcpHeader.nStepGrpBegin; i++)
	{
		dbProcTotalTime += GetProcStepTime(i);
	}

	for(j=0; j<m_RcpHeader.nStepGrpCyc; j++)
	{
		for(i=m_RcpHeader.nStepGrpBegin; i<=m_RcpHeader.nStepGrpEnd; i++)
		{
			dbProcTotalTime += GetProcStepTime(i);
		}
	}
	for(i=m_RcpHeader.nStepGrpEnd+1; i<=m_RcpHeader.nTotalStep; i++)
	{
		dbProcTotalTime += GetProcStepTime(i);
	}
	return dbProcTotalTime;
}

double CProcessSequence::GetProcTotalTime()
{
	double dbProcTotalTime = 0;
	if(m_RcpHeader.bUseStepGrp == FALSE) dbProcTotalTime = GetNormalProcTime();
	else dbProcTotalTime = GetGrpCycProcTime();
	return dbProcTotalTime;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Sub Functions

void CProcessSequence::InitRecipe()
{
	memset(&m_RcpHeader, 0, sizeof(RECIPE_HEADER_STRUC));

	int i = 0;
	for(i = 0; i < MAX_RECIPE_STEP; i ++)
	{
		memset(&m_RcpStep[i], 0, sizeof(RECIPE_STEP_STRUC));
	}

}

void CProcessSequence::SetRecipeInform()
{
	int nIOStatus;
	char szStepGrpSM[40] = "";

	WRITE_STRING (PRO_RcpNameSM  , m_RcpHeader.szRecipeName, &nIOStatus);
	WRITE_STRING (PRO_StepNameSM , ""     , &nIOStatus);				//... 2014.07.22 Add StepName Init for FDC
	WRITE_ANALOG (PRO_TotalStepAM, (double) m_RcpHeader.nTotalStep, &nIOStatus);
	WRITE_ANALOG (PRO_CurStepAM  , (double) 0 , &nIOStatus);
	WRITE_DIGITAL(PRO_RcpTypeDM  , m_RcpHeader.nRecipeType, &nIOStatus);
	WRITE_DIGITAL(PRO_IsMainDM   , 0          , &nIOStatus);			// 0:NoMain 1:Main
	WRITE_ANALOG (PRO_TotalCycAM , (double) 0 , &nIOStatus);
	WRITE_ANALOG (PRO_CurCycAM   , (double) 0 , &nIOStatus);

	if(m_RcpHeader.bUseStepGrp == TRUE)
	{
		sprintf(szStepGrpSM, "%d ~ %d", m_RcpHeader.nStepGrpBegin, m_RcpHeader.nStepGrpEnd);
	}
	WRITE_STRING(PRO_StepGrpSM, szStepGrpSM, &nIOStatus);
	WRITE_ANALOG(PRO_StpGrpTCycAM, m_RcpHeader.nStepGrpCyc, &nIOStatus);

	m_nStpGrpCCyc = 0;
	WRITE_ANALOG(PRO_StpGrpCCycAM, m_nStpGrpCCyc, &nIOStatus);
}

//////////////////////////////////////////////////////////////////////////
void CProcessSequence::LogRecipeInform()
{
	char szLog[256];
	int i, j;

	sprintf(szLog, "--------------------> Recipe Data Begin");
	_LOG_RCP(szLog); printf("%s\n", szLog);
	sprintf(szLog, "-> RcpType:%d  TotalStep:%d   Name:%s",
		m_RcpHeader.nRecipeType, m_RcpHeader.nTotalStep, m_RcpHeader.szRecipeName);
	_LOG_RCP(szLog); printf("%s\n", szLog);
	sprintf(szLog, "-> StepGrpBegin:%d  GrpEnd:%d  GrpCycle:%d",
		m_RcpHeader.nStepGrpBegin, m_RcpHeader.nStepGrpEnd, m_RcpHeader.nStepGrpCyc);
	_LOG_RCP(szLog); printf("%s\n", szLog);

	for(i=0; i<m_RcpHeader.nTotalStep; i++)
	{
		sprintf(szLog, "-> StepNumber:%d   StepName:%s   MainStep:%d", i+1,
			m_RcpStep[i].szStepName, m_RcpStep[i].bIsMain);
		_LOG_RCP(szLog); printf("%s\n", szLog);
		sprintf(szLog, "-> Chamber Pressure Hi:%6.3f Lo:%6.3f  Opt:%d",
			m_RcpStep[i].dblCHMPRS_Pres, m_RcpStep[i].dblCHMPRS_PresL, m_RcpStep[i].nCHMPRS_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> Throttle Valve Sp:%6.3f   Opt:%d",
			m_RcpStep[i].dblTVPRS_Sp, m_RcpStep[i].nTVPRS_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> ZMotion  Sp:%8.3f   Opt:%d", m_RcpStep[i].dbSTGZPOS_Sp, m_RcpStep[i].nSTGZPOS_WaitOpt);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> ZMotion2 Sp:%8.3f   Opt:%d", m_RcpStep[i].dbSTGZPOS2_Sp, m_RcpStep[i].nSTGZPOS_WaitOpt);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> RF Sp:%d    Opt:%d",
			m_RcpStep[i].nRF_FwSp, m_RcpStep[i].nRF_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);
		//2008.09.29
		sprintf(szLog, "-> RF2 Sp:%d   Opt:%d",
			m_RcpStep[i].nRF2_FwSp, m_RcpStep[i].nRF2_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		// 2014.05.22
		sprintf(szLog, "-> Preset1 Mode:%d	Sel:%d	Type:%d",
			m_RcpStep[i].nPrstMode_Option, m_RcpStep[i].nPrstSel_Option, m_RcpStep[i].nRFLdPrst_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);
		sprintf(szLog, "-> Preset2 Mode:%d	Sel:%d	Type:%d",
			m_RcpStep[i].nPrst2Mode_Option, m_RcpStep[i].nPrst2Sel_Option, m_RcpStep[i].nRF2LdPrst_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);
		//

		sprintf(szLog, "-> StageHeater Sp:%d   Opt:%d",
			(int) m_RcpStep[i].dbStgHtr1_SetPoint, m_RcpStep[i].nStgHtr1_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> StageHeater2 Sp:%d   Opt:%d",
			(int) m_RcpStep[i].dbStgHtr2_SetPoint, m_RcpStep[i].nStgHtr2_Option);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> ValveRcp CycleCount:%d  TotalStep:%d  En:%d",
			m_RcpStep[i].nVR_CycleCount, m_RcpStep[i].nVR_TotalStep,
			m_RcpStep[i].nVR_Enable);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> Preventive Maint Option:%d", m_RcpStep[i].bValveCycleAccOption);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		sprintf(szLog, "-> PreHeat Time:%d", m_RcpStep[i].nPreHeatTime);
		_LOG_RCP(szLog); printf("%s\n", szLog);

		for(j=0; j<m_nMaxMfc; j++)
		{
			sprintf(szLog, "-> MFC[%d]   Sp:%3.1f  Opt:%d", j+1,
				m_RcpStep[i].pdbMFC_Sp[j], m_RcpStep[i].pnMFC_Option[j]);
			_LOG_RCP(szLog);
		}

		for(j = 0; j < MAX_HEATER; j++)
		{
			if(m_RcpStep[i].HTR_nOpt[j] > 0)
			{
				sprintf(szLog, "-> HTR[%d]  SetPoint:%3.1f   Option:%d", j+1,
					m_RcpStep[i].HTR_dbSp[j], m_RcpStep[i].HTR_nOpt[j]);
				_LOG_RCP(szLog);
			}
		}
	}

	sprintf(szLog, "--------------------> Recipe Data End");
	_LOG_RCP(szLog); printf("%s\n", szLog);
}

void CProcessSequence::DisplayStepInfo(int nStep)
{
	int nIOStatus;
	int nIdx;

	nIdx = nStep-1;
	WRITE_STRING(PRO_StepNameSM, m_RcpStep[nIdx].szStepName, &nIOStatus);
	WRITE_ANALOG(PRO_CurStepAM, nStep, &nIOStatus);
	WRITE_ANALOG(PRO_TotalCycAM, (double) m_RcpStep[nIdx].nVR_CycleCount, &nIOStatus);
	WRITE_ANALOG(PRO_CurCycAM, (double) 0, &nIOStatus);
}

void CProcessSequence::AddString(char *szSrc, const char *szDest)
{
	char szTemp[MAX_PARAM_SIZE];
	sprintf(szTemp, "%s%s", szSrc, szDest);
	strcpy(szSrc, szTemp);
}

Module_Status CProcessSequence::CVGSafetyWait()
{
	char pszLocFtnName[] = "CVGSafetyWait";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus;
	int nWaitT = 0;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		WRITE_DIGITAL(FastRoughVlvDO, VLV_OPEN, &nIOStatus);
		RUN_FUNCTION(PRS_CONTROL, "OPEN");
		nWaitT = (int) READ_ANALOG(PRMA_WT_CVGSafeWait, &nIOStatus);
		if(! WAIT_SECONDS(nWaitT)) { msRet = SYS_ABORTED; break; }
		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

void CProcessSequence::SetACVMode(int nRcpType)
{
	int nIOStatus;
	switch(nRcpType)
	{
	case RT_Unknown: WRITE_DIGITAL(ACV_ModeDM, 2, &nIOStatus); break;
	case RT_CleanRcp:
		if(m_Clean_nACVControl == 1) WRITE_DIGITAL(ACV_ModeDM, 1, &nIOStatus);
		else if(m_Clean_nACVControl == 2) WRITE_DIGITAL(ACV_ModeDM, 3, &nIOStatus);
		break;
	default: WRITE_DIGITAL(ACV_ModeDM, 0, &nIOStatus); break;
	}
	// ACV_ModeDM => 0:Proc 1:Clean 2:Auto 3:CleanAuto
}

BOOL CProcessSequence::CheckACVStatus()
{
	BOOL bRet = FALSE;
	int nIOStatus;
	int nErrCnt = 0;
	// int nAlarmRecovery;

	if(! m_Clean_bACVCheck) return TRUE;
	do {
		do {
			switch(m_RcpHeader.nRecipeType)
			{
			case RT_Unknown: bRet = TRUE; break;
			case RT_CleanRcp:
				if(READ_DIGITAL(ACVClnPosDI, &nIOStatus) == 1) bRet = TRUE;
				break;
			default:
				if(READ_DIGITAL(ACVPrcPosDI, &nIOStatus) == 1) bRet = TRUE;
				break;
			}
			if(bRet == TRUE) break;
			if(! WAIT_SECONDS(1)) break;
			if(++nErrCnt > 5) break;
		} while(bRet == FALSE);
		if(bRet == FALSE)
		{
			// 2006.01.18
			ALARM_POST(ACV_POSITION_INCORRECT);
			break;
		}
		else break;
	} while(1);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2015.03.23 Add PLC Interlock Retry
BOOL CProcessSequence::CheckPLCGlobalInterlock()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	int		nErrCnt = 0;
	int		nAlarmRecovery = ALM_CLEAR;

	do {
		if(enOFFON_OFF == READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus))		// 0:Normal 1:Interlock Present
		{
			bRet = TRUE;
			break;
		}

		if(! WAIT_SECONDS(1))	break;

		if(++nErrCnt > 5)
		{
			nAlarmRecovery = ALARM_MANAGE(ALARM_PLC_GBL_INTERLOCK_CHK);
			if(ALM_RETRY  == nAlarmRecovery)		nErrCnt = 0;	//... Retry
			else									break;		//... Abort
		}
	} while(1);

	return bRet;
}

BOOL CProcessSequence::CheckSWInterlock()
{
	BOOL bRet = FALSE;
	int nIOStatus;
	int nErrCnt = 0;
	int nSW_GbIntlckDM;

	do {
		do {
			nSW_GbIntlckDM = READ_DIGITAL(SW_GbIntlckDM, &nIOStatus); 	// 0:Normal 1:Interlock Present
			if(nSW_GbIntlckDM == 1) bRet = FALSE;
			else { bRet = TRUE; break; }
			if(! WAIT_SECONDS(1)) break;
			if(++nErrCnt > 5) break;
		} while(bRet == FALSE);
		if(bRet == FALSE)
		{
			// 2006.01.18
			ALARM_POST(ALARM_SW_INTERLOCK_CHK);
			break;
		}
		else break;
	} while(1);
	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Write valve recipe to driver through shared memory
// 2004.09.10 by cskim

BOOL CProcessSequence::WriteVRCP2Driver(int nStep)
{
	BOOL bRet = TRUE;
	int nIdx;
	HANDLE hFileHandle = NULL;
	LPVOID pViewOfFile = NULL;

	if(hFileHandle != NULL)
	{
		sprintf(m_szErrDesc, "FileMapping is already opened");
		return FALSE;
	}

	hFileHandle = OpenFileMapping(		FILE_MAP_WRITE,                     // protection
										(int) NULL,
										SM_FILE_NAME	  	                    // object name
									 );

	if (NULL == hFileHandle)
	{
		sprintf(m_szErrDesc, "Unable to open file mapping. Error is %d", GetLastError());
		return FALSE;
	}

	pViewOfFile = MapViewOfFile(	hFileHandle,							// handle to file-mapping object
									FILE_MAP_ALL_ACCESS,					// access mode
									0,										// high-order DWORD of offset
									0,										// low-order DWORD of offset
									0										// number of bytes to map
								);

	do {
		if (NULL == pViewOfFile)
		{
			sprintf(m_szErrDesc, "Unable to map to the current address space. Error is %d", GetLastError());
			bRet = FALSE;
			break;
		}

		int nStepLength;
		nIdx = nStep - 1;
		nStepLength = VR_MAX_STEP * VR_STEP_SIZE;
		memcpy((char*)pViewOfFile, m_RcpStep[nIdx].ppVR_Step, nStepLength);
		memcpy((char*)pViewOfFile + nStepLength, m_RcpStep[nIdx].pVR_Header, VR_HEADER_SIZE);

		if (pViewOfFile != NULL)
		{
			if(FALSE == UnmapViewOfFile(pViewOfFile))
			{
				sprintf(m_szErrDesc, "Unable to unmap view of file. Error is %d", GetLastError());
			}
		}
		if (NULL != hFileHandle) CloseHandle(hFileHandle);

	} while(0);

	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL CProcessSequence::DownloadValveRecipe(int nStep, const unsigned char *pData, int nSize)
{
	// for Simulation
	if(m_nSimMode != SM_NOT_SIM && m_nSimMode != SM_PLC_TEST)
	{
		return DownloadValveRecipe_Sim(nStep, pData, nSize);
	}

	// m_nVRCP_DnMethodDM => 0:Channel 1:SharedMemory 2:Enther Net
	if(0 == m_nVRCP_DnMethodDM || 1 == m_nVRCP_DnMethodDM)
	{
		return DownloadValveRecipe_DNET(nStep, pData, nSize);
	}
	else // if 2 == m_nVRCP_DnMethodDM
	{
		return DownloadValveRecipe_EIP(nStep, pData, nSize);
	}
}

//////////////////////////////////////////////////////////////////////////
//---> ADD 2014.08.25
BOOL CProcessSequence::DownloadValveRecipe_EIP(int nStep, const unsigned char *pData, int nSize)
{
	BOOL bRet = FALSE;
	int nIOStatus = FALSE;
	int	nCount = 0;
	int nVRetryCount = 0;
	BOOL bMainCheck = FALSE;
	Module_Status msFlag = SYS_SUCCESS;
	char szLog[256];

	_LOG_PROC("-----=====> Entered DownloadValveRecipe");

	do {

		bMainCheck = TRUE;
		for( nVRetryCount = 0; nVRetryCount < MAX_DOWNLOAD_RETRY ; nVRetryCount++)
		{
			bMainCheck = TRUE;

			WRITE_DIGITAL(DN_SetSMData_DO, nStep, &nIOStatus);
			_LOG_PROC("-----=====> Set Shared Memory Data");


			// 3.1 Send Packet
			WRITE_DIGITAL(DN_SendPacket_DO, 1, &nIOStatus);
			if(FALSE == nIOStatus)
			{
				sprintf(szLog,"-----=====> WRITE_DIGITAL[DN_SendPacket_DO, 1] Failed");
				_LOG_PROC(szLog); printf("%s\n",szLog);
				bMainCheck = FALSE;
				continue;
			}
			_LOG_PROC("-----=====> Wrote Send Command");
			bMainCheck = TRUE;

			if(! WAIT_SECONDS(0.1))
			{
				msFlag = SYS_ABORTED;
				sprintf(szLog, "-----=====> Sys Aborted while Send Packet");
				_LOG_PROC(szLog); printf("%s\n",szLog);
				break;
			}

			break;
		}

		if( SYS_SUCCESS != msFlag)
		{
			// ALARM_POST(ALARM_VALVE_RECIPE_DOWNLOAD_FAILED);
			sprintf(szLog, "-----=====> Valve Recipe DownLoad Failed by SYS_ABORTED");
			_LOG_PROC(szLog); printf("%s\n",szLog);
			break;
		}
		if( FALSE == bMainCheck )
		{
			ALARM_POST(ALARM_VALVE_RECIPE_DOWNLOAD_FAILED);
			sprintf(szLog, "-----=====> Valve Recipe DownLoad Failed! Tried to download valve recipe at [%d] times", nVRetryCount);
			_LOG_PROC(szLog); printf("%s\n",szLog);
			break;
		}

		bRet = TRUE;
	} while(0);
	_LOG_PROC("-----=====> Leaved DownloadValveRecipe");
	return bRet;
}
//---------------------------------------------------------------------------------------
// Valve Recipe Download Function
//---> Modify 2014.08.25
//BOOL CProcessSequence::DownloadValveRecipe(int nStep, const unsigned char *pData, int nSize)
BOOL CProcessSequence::DownloadValveRecipe_DNET(int nStep, const unsigned char *pData, int nSize)
//---> End    2014.08.25
{
	char pszLocFtnName[] = "DownloadValveRecipe";
	// char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = FALSE;
	int i;
	int	nCount = 0;
	int nVRetryCount = 0;
	BOOL bMainCheck = FALSE;
	BOOL bSubCheck  = FALSE;
	Module_Status msFlag = SYS_SUCCESS;
	int	nReadServiceCode = 0;
	char szLog[256];
	// char szBuf[256];

	// for Simulation
	if(m_nSimMode != SM_NOT_SIM && m_nSimMode != SM_PLC_TEST)
	{
		return DownloadValveRecipe_Sim(nStep, pData, nSize);
	}

	// sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	// printf("Entered %s\n", pszFtnName);
	_LOG_PROC("-----=====> Entered DownloadValveRecipe");

	// Packet Data Log
	/*
	strcpy(szLog, "-----=====> Valve Packet Data :");
	_LOG_PROC(szLog);
	strcpy(szLog, "");
	for(i=0; i<(m_nBASE_IDX+10); i++)
	{
		sprintf(szBuf, " %02X", *(pData+i));
		strcat(szLog, szBuf);
	}
	for(i=(m_nBASE_IDX+10); i<nSize; i++)
	{
		if(((i-(m_nBASE_IDX+10)) % 24) == 0)
		{
			_LOG_PROC(szLog);
			strcpy(szLog, "");
		}
		sprintf(szBuf, " %02X", *(pData+i));
		strcat(szLog, szBuf);
	}
	_LOG_PROC(szLog);
	*/

	do
	{
		bMainCheck = TRUE;
		for( nVRetryCount = 0; nVRetryCount < MAX_DOWNLOAD_RETRY; nVRetryCount++)
		{
			bMainCheck = TRUE;
			bSubCheck = TRUE;

			// m_nVRCP_DnMethodDM => 0:Channel 1:SharedMemory 2:Enther Net
			if(m_nVRCP_DnMethodDM == 1)
			{
				WRITE_DIGITAL(DN_SetSMData_DO, nStep, &nIOStatus);
				_LOG_PROC("-----=====> Set Shared Memory Data");
			}
			else
			{
				// Packing Packet with WRITE_DIGITAL[DN_SendByte_DO]
				for( i=0; i < nSize; i++)
				{
					WRITE_DIGITAL( DN_SendByte_DO, pData[i], &nIOStatus);
					if( FALSE == nIOStatus )
					{
						sprintf(szLog, "-----=====> WRITE_DIGITAL[DN_SendByte_DO, %d, 0x%02x] Failed",
							i, pData[i]);
						_LOG_PROC(szLog); printf("%s\n",szLog);
						bSubCheck = FALSE;
						break;
					}
				}
				_LOG_PROC("-----=====> Write Valve Recipe Data To Channel");
			}

			// LOG(pszFtnName, _ELT_FileConsole, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
            //                pData[0], pData[1],pData[2],pData[3],pData[4],pData[5],pData[6],pData[7],pData[8],pData[9]);
			if( FALSE == bSubCheck )
			{
				bMainCheck = FALSE;
				continue;
			}
			bMainCheck = TRUE;

			// 3.1 Send Packet
			WRITE_DIGITAL( DN_SendPacket_DO, 1, &nIOStatus );
			if( FALSE == nIOStatus )
			{
				sprintf(szLog,"-----=====> WRITE_DIGITAL[DN_SendPacket_DO, 1] Failed");
				_LOG_PROC(szLog); printf("%s\n",szLog);
				bMainCheck = FALSE;
				continue;
			}
			_LOG_PROC("-----=====> Wrote Send Command");

			if(! WAIT_SECONDS(0.3))
			{
				msFlag = SYS_ABORTED;
				sprintf(szLog, "-----=====> Sys Aborted while Send Packet");
				_LOG_PROC(szLog); printf("%s\n",szLog);
				break;
			}

			// 4 Get Message
			bSubCheck = TRUE;
			for( nCount = 0; nCount < MAX_GET_MESSAGE_CHECK; nCount++)
			{
				WRITE_DIGITAL( DN_GetMessage_DO, 1, &nIOStatus);
				if( FALSE == nIOStatus )
				{
					bSubCheck = FALSE;
					// LOG( pszFtnName, _ELT_FileConsole, "WRITE_DIGITAL[DN_GetMessage_DO] Failed. Retry[%d]", nCount);
					if( FALSE == WAIT_SECONDS(0.05))
					{
						msFlag = SYS_ABORTED;
						sprintf(szLog, "-----=====> Sys Aborted while Mail box Check");
						_LOG_PROC(szLog); printf("%s\n",szLog);
						break;
					}
					continue;
				}
				bSubCheck = TRUE;
				break;
			}//for( nCount = 0; nCount < MAX_EMPTY_MAILBOX_CHECK; nCount++)
			if( SYS_SUCCESS != msFlag ) break;
			if( FALSE == bSubCheck )
			{
				bMainCheck = FALSE;
				continue;
			}
			bMainCheck = TRUE;
			_LOG_PROC("-----=====> Get Return Message");

			// 5. Service Code Check
			// 5.1 Service Code Read
			nReadServiceCode = READ_DIGITAL(DN_ReadGetSvcCode_DI, &nIOStatus);
			if( FALSE == nIOStatus)
			{
				sprintf(szLog, "-----=====> READ_DIGITAL[DN_ReadGetSvcCode_DI] Failed! Retry[%d]", nVRetryCount);
				_LOG_PROC(szLog); printf("%s\n",szLog);
				bMainCheck = FALSE;
				continue;
			}
			bMainCheck = TRUE;

			// 5.2 Service Code Read
			if( VSP_SERVICE_CODE != nReadServiceCode )
			{
				sprintf(szLog, "-----=====> Service Code differs[%x != Read[0x%x]], Retry[%d]", VSP_SERVICE_CODE, nReadServiceCode, nVRetryCount);
				_LOG_PROC(szLog); printf("%s\n",szLog);
				bMainCheck = FALSE;
				continue;
			}
			bMainCheck = TRUE;
			msFlag = SYS_SUCCESS;
			sprintf(szLog, "-----=====> Service Code Check OK!");
			_LOG_PROC(szLog); printf("%s\n",szLog);
			break;
		}//for( nVRetryCount = 0; nVRetryCount < MAX_VALVE_DOWNLOAD; nVRetryCount++)
		if( SYS_SUCCESS != msFlag)
		{
			// ALARM_POST(ALARM_VALVE_RECIPE_DOWNLOAD_FAILED);
			sprintf(szLog, "-----=====> Valve Recipe DownLoad Failed by SYS_ABORTED");
			_LOG_PROC(szLog); printf("%s\n",szLog);
			break;
		}
		if( FALSE == bMainCheck )
		{
			ALARM_POST(ALARM_VALVE_RECIPE_DOWNLOAD_FAILED);
			sprintf(szLog, "-----=====> Valve Recipe DownLoad Failed! Tried to download valve recipe at [%d] times", nVRetryCount);
			_LOG_PROC(szLog); printf("%s\n",szLog);
			break;
		}

		bRet = TRUE;
	} while(0);
	// printf("Leaved %s\n", pszFtnName);
	_LOG_PROC("-----=====> Leaved DownloadValveRecipe");
	return bRet;
}

BOOL CProcessSequence::DownloadValveRecipe_Sim(int nStep, const unsigned char *pData, int nSize)
{
	char pszLocFtnName[] = "DownloadValveRecipe_Sim";
	// char pszFtnName[256];
	BOOL bRet = FALSE;
	int i, j;
	char szDownBuf[1000];
	char szBuf[40];
	int nIdx;

	// sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	// printf("Entered %s\n", pszFtnName);
	_LOG_PROC("-----=====> Simulation ValveRcp Download Start");
	do {
		sprintf(szDownBuf, "DOWNLOAD %d %d 0 12 DAT : ", m_nVRCPBufPage, nStep);
		for(i=0; i<12; i++)
		{
			sprintf(szBuf, " %02X", *(pData+i));
			strcat(szDownBuf, szBuf);
		}
		RUN_FUNCTION(PM_SIMULATOR, szDownBuf);

		printf("[DEBUG] SIM_VRCP DOWNLOAD Page[%d] Step[%d]\n[%s]\n",
			m_nVRCPBufPage, nStep, szDownBuf);

		for(i=12; i<(nSize-12); i += 24)
		{
			sprintf(szDownBuf, "DOWNLOAD %d %d %d %d DAT : ", m_nVRCPBufPage, nStep, i, 24);
			for(j=0; j<24; j++)
			{
				nIdx = i + j;
				if(nIdx < nSize) {
					sprintf(szBuf, " %02X", *(pData+nIdx));
					strcat(szDownBuf, szBuf);
				}
			}
			RUN_FUNCTION(PM_SIMULATOR, szDownBuf);

			printf("[DEBUG] SIM_VRCP DOWNLOAD Page[%d] Step[%d]\n[%s]\n",
				m_nVRCPBufPage, nStep, szDownBuf);
		}

		bRet = TRUE;
	} while(0);

	// Download Delay
	if(! WAIT_SECONDS(0.5)) return false;
	if(bRet) _LOG_PROC("-----=====> Simulation ValveRcp Download finished successfly");
	else _LOG_PROC("-----=====> Simulation ValveRcp Download failed !");
	// printf("Leaved %s\n", pszFtnName);
	return bRet;
}

BOOL CProcessSequence::DownloadProcStepVRCP(int nStep, BOOL bRetry)
{
	BOOL bRet = FALSE;
	BOOL bRslt = TRUE;
	int nIdx;
	int i;
	char szLog[200];
	int nIOStatus;
	int nVrcpBaseAddr;
	int nVRCPMode;

	nIdx = nStep - 1;
	nVRCPMode = m_RcpStep[nIdx].nVRCPMode;

	do {
		m_nDL_Thread = THREAD_STEP1;

		// 0:VRM_CONT 1:VRM_PAUSE 2:VRM_NOT_DEF 3:VRM_NO_VRCP
		m_RcpStep[nIdx].pVR_Header[m_nVR_HDR_BASE+3] = nVRCPMode;

		// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop
		if(m_RcpStep[nIdx].nStepFlow_Option != SFL_NORMAL_CYC)
				m_RcpStep[nIdx].pVR_Header[m_nVR_HDR_BASE+3] |= 0x04;

		// 2006.01.16
		if(m_RcpStep[nIdx].nMFC_RampOpt == 1)
				m_RcpStep[nIdx].pVR_Header[m_nVR_HDR_BASE+3] |= 0x08;
		//2014.08.25
		m_RcpStep[nIdx].pVR_Header[m_nVR_HDR_BASE+4] = m_nRunStepCount / 256;	//0			//1	  //1              //DataID(EIP추가)	High
		m_RcpStep[nIdx].pVR_Header[m_nVR_HDR_BASE+5] = m_nRunStepCount % 256;	//1~255	    //0   //1~255   ...	   //DataID(EIP추가)	Low		(GroupCycle 진행시  증가 수치로 처리 필요)
		m_nRunStepCount++;		//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)
		//

		// Set VRCP Page Address
		// 2013.06.13
		if(bRetry)
		{
			m_nVRCPBufPage--;
			if(m_nVRCPBufPage < 0)  m_nVRCPBufPage = m_nVRCPBufPageMax-1;
		}

		// 2014.08.25
		if(0 == m_nVRCP_DnMethodDM || 1 == m_nVRCP_DnMethodDM)
		{
			nVrcpBaseAddr = m_anVRCcpBufPageAddr[m_nVRCPBufPage];

			if(2 == m_nVRCPBufPageMax)
			{
				for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++)
				{
					m_RcpStep[nIdx].ppVR_Step[i][0] = (i*VR_STEP_SIZE/2 + nVrcpBaseAddr) % 256;
					m_RcpStep[nIdx].ppVR_Step[i][1] = (i*VR_STEP_SIZE/2 + nVrcpBaseAddr) / 256;
				}
				m_RcpStep[nIdx].pVR_Header[0] = (10*VR_STEP_SIZE/2 + nVrcpBaseAddr) % 256;
				m_RcpStep[nIdx].pVR_Header[1] = (10*VR_STEP_SIZE/2 + nVrcpBaseAddr) / 256;
			}
			else		// 2013.06.25
			{
				m_RcpStep[nIdx].ppVR_Step[0][0] = nVrcpBaseAddr % 256;
				m_RcpStep[nIdx].ppVR_Step[0][1] = nVrcpBaseAddr / 256;
				m_RcpStep[nIdx].pVR_Header[0] = (VR_STEP_SIZE/2 + nVrcpBaseAddr) % 256;
				m_RcpStep[nIdx].pVR_Header[1] = (VR_STEP_SIZE/2 + nVrcpBaseAddr) / 256;
				_LOG_PROC("-----=====> Step(%d) Head Down Addr : D%d (Page:%d)",
						nStep, VR_STEP_SIZE/2 + nVrcpBaseAddr, m_nVRCPBufPage);
			}
		}
		//---> ADD 2014.08.25   (중요)  (Address 대신  PageNo 를 설정한다)
		else // if 2 == m_nVRCP_DnMethodDM
		{
			for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++) m_RcpStep[nIdx].ppVR_Step[i][0] = m_nVRCPBufPage;
			m_RcpStep[nIdx].pVR_Header[0] = m_nVRCPBufPage;
		}
		//---> END 2014.03.25


		// m_nVRCP_DnMethodDM => 0:Channel 1:SharedMemory 2:Enther Net
		//if(m_nVRCP_DnMethodDM == 1 && (m_nSimMode == SM_NOT_SIM || m_nSimMode == SM_PLC_TEST) )
		if((m_nVRCP_DnMethodDM == 1 || m_nVRCP_DnMethodDM == 2 ) && (m_nSimMode == SM_NOT_SIM || m_nSimMode == SM_PLC_TEST) )
		//---> End    2014.08.25
		{
			sprintf(szLog, "-----=====> [DnLdStep:%d] Download VRCP Data To Driver", nStep);
			_LOG_PROC(szLog);

			// Copy ValveRecipe To Shared Memory
			if(WriteVRCP2Driver(nStep) == FALSE)
			{
				ALARM_POST(ALARM_SW_FUNC_ERROR_OCCURED);
				sprintf(szLog, "-----=====> [DnLdStep:%d] Failed to write VRCP to Driver as shared memory in %s", nStep, APP_NAME);
				_LOG_ERR_P(szLog)
				break;
			}
			sprintf(szLog, "-----=====> [DnLdStep:%d] Moved VRCP Data To Shared Memory", nStep);
			_LOG_PROC(szLog);

			// DN_SMCmd_DO => ReadSM(Read Shared Memory)
			WRITE_DIGITAL(DN_SMCmd_DO, 0, &nIOStatus);
			if(nIOStatus == FALSE)
			{
				ALARM_POST(ALARM_SW_FUNC_ERROR_OCCURED);
				sprintf(szLog, "-----=====> [DnLdStep:%d] Shared Memory Recipe Download Failed in %s", nStep, APP_NAME);
				_LOG_ERR_P(szLog)
				break;
			}
			sprintf(szLog, "-----=====> [DnLdStep:%d] Finished writing VRCP to Driver through shared memory", nStep);
			_LOG_PROC(szLog);
		}

		if(ED_Enable == m_RcpStep[nIdx].nVR_Enable)
		{
			//2014.08.25
			if(0 == m_nVRCP_DnMethodDM || 1 == m_nVRCP_DnMethodDM)
			{
				//
				for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++)
				{
					bRslt = DownloadValveRecipe(i+1, m_RcpStep[nIdx].ppVR_Step[i], VR_STEP_SIZE);
					if(m_nDL_Thread == THREAD_STOP) { bRslt = FALSE; break; }

					// Download VRCP Header after Downloading First VRCP Step
					if(i == 0 && bRslt != FALSE)
					{
						bRslt = DownloadValveRecipe(0, m_RcpStep[nIdx].pVR_Header, VR_HEADER_SIZE);
						m_nDL_Thread = THREAD_STEP2;
						sprintf(szLog, "-----=====> [DnLdStep:%d] Download Header Checked", nStep);
						_LOG_PROC(szLog);

					}
					if(! bRslt) break;
				}
			if(i < m_RcpStep[nIdx].nVR_TotalStep) break;
			}
			// 2014.08.25
			else // if 2 == m_nVRCP_DnMethodDM
			{
				// 2015.01.25 for Simulation
				if(m_nSimMode != SM_NOT_SIM && m_nSimMode != SM_PLC_TEST)
				{
					for(i=0; i<m_RcpStep[nIdx].nVR_TotalStep; i++)
					{
						bRslt = DownloadValveRecipe(i+1, m_RcpStep[nIdx].ppVR_Step[i], VR_STEP_SIZE);
						if(m_nDL_Thread == THREAD_STOP) { bRslt = FALSE; break; }

						// Download VRCP Header after Downloading First VRCP Step
						if(i == 0 && bRslt != FALSE)
						{
							bRslt = DownloadValveRecipe(0, m_RcpStep[nIdx].pVR_Header, VR_HEADER_SIZE);
							m_nDL_Thread = THREAD_STEP2;
							sprintf(szLog, "-----=====> [DnLdStep:%d] Download Header Checked", nStep);
							_LOG_PROC(szLog);

						}
						if(! bRslt) break;
					}
						if(i < m_RcpStep[nIdx].nVR_TotalStep) break;
				}
				else
				{
					//2014.08.25  Comment      EIP는 한번만 Write 하면되므로,  1,2,3번 Parameter는 의미없음.  (ValveStep No 는  0 으로 Fix한다(의미없음))
					bRslt = DownloadValveRecipe(0, m_RcpStep[nIdx].pVR_Header, VR_HEADER_SIZE);
					m_nDL_Thread = THREAD_STEP2;
					sprintf(szLog, "-----=====> [DnLdStep:%d] Download Header Checked", nStep);
					_LOG_PROC(szLog);
					if(! bRslt) break;
				}
			}
			// 2014.08.25
		}
		else
		{
			bRslt = DownloadValveRecipe(0, m_RcpStep[nIdx].pVR_Header, VR_HEADER_SIZE);
			m_nDL_Thread = THREAD_STEP2;
			sprintf(szLog, "-----=====> [DnLdStep:%d] Download Header Checked", nStep);
			_LOG_PROC(szLog);
		}

		bRet = TRUE;
	} while(0);

	// Download Page Changed
	// 2013.06.25
	m_nVRCPBufPage++;
	if(m_nVRCPBufPage >= m_nVRCPBufPageMax) m_nVRCPBufPage = 0;

	if(! bRet)
	{
		sprintf(szLog, "-----=====> [DnLdStep:%d] DownloadValveRecipe failed in %s", nStep, APP_NAME);
		_LOG_ERR_P(szLog)
	}
	return bRet;
}

DWORD WINAPI CProcessSequence::DL_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *)lParam;
	EnterCriticalSection(& pBaseApp->m_csLock);
	BOOL bRslt = TRUE;
	int nStep;
	char szLog[200];
	do {
		if(pBaseApp->m_nDL_Thread == THREAD_FAILED) break;
		pBaseApp->m_nDL_Thread = THREAD_RUN;

		// Increase Downloading Step Count
		pBaseApp->m_nDnLdStepCount++;

		nStep = pBaseApp->m_nQueuedDnLdStep;
		sprintf(szLog, "-----===> [DnLdStep:%d] Entered DL_THREAD", nStep);
		_LOG_PROC(szLog);

		bRslt = pBaseApp->DownloadProcStepVRCP(nStep);
		if(bRslt == TRUE)
		{
			if(pBaseApp->m_nDnLdStepCount == pBaseApp->m_nDnRdyStepCount)
					pBaseApp->m_nDL_Thread = THREAD_STOP;
			else pBaseApp->m_nDL_Thread = THREAD_RUN;
		}
		else pBaseApp->m_nDL_Thread = THREAD_FAILED;

		sprintf(szLog, "-----===> [DnLdStep:%d] Leaved DL_THREAD", nStep);
		_LOG_PROC(szLog);
	} while(0);
	LeaveCriticalSection(& pBaseApp->m_csLock);
	pBaseApp->m_bFirstStepDownload = TRUE;
	return 0;
}

BOOL CProcessSequence::Start_DownloadThread(int nStep)
{
	_LOG_PROC("-----> Start_DownloadThread called");

	char szLog[200];
	if(m_nDnRdyStepCount > m_nDnLdStepCount)
	{
		ALARM_POST(ALARM_SW_FUNC_ERROR_OCCURED);
		sprintf(szLog, "-----> [Step:%d] SW Function Error (Download queue is full but called)", nStep);
		_LOG_ERR_P(szLog)
		return FALSE;
	}
	m_nDnRdyStepCount++;

	BOOL bRet = TRUE;
	m_nQueuedDnLdStep = nStep;
	RunThreadFunc(DL_ThreadProc, this);

	while(m_nDL_Thread == THREAD_STOP) WAIT_SECONDS(0.03);
	if(m_nDL_Thread == THREAD_FAILED) bRet = FALSE;
	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Throttle Valve Setting Function and Thread
// 2004.09.10 by cskim

void CProcessSequence::SetTV()
{
	BOOL bRslt = TRUE;
	int nIdx;
	char szTVParam[MAX_PARAM_SIZE];
	double dblValue;
	char szLog[200];

	nIdx = m_nTV_Step - 1;

	// Pressure Setting
	_LOG_PROC("-----===> Started TV Setting");
	dblValue = m_RcpStep[nIdx].dblTVPRS_Sp;
	if(TVM_POSITION == m_RcpStep[nIdx].nTVPRS_Option)
	{
		sprintf(szTVParam, "POSITION %7.3f", dblValue);
		RUN_FUNCTION(PRS_CONTROL, szTVParam);
	}
	else if(TVM_PRESSURE <= m_RcpStep[nIdx].nTVPRS_Option)
	{
		sprintf(szTVParam, "PRESSURE %7.3f", dblValue,
				m_RcpStep[nIdx].nTVPRS_Option);
		RUN_FUNCTION(PRS_CONTROL, szTVParam);
	}
	else
	{
		ALARM_POST(ALARM_SW_FUNC_ERROR_OCCURED);
		sprintf(szLog, "-----=====> Invalid Throttle Valve Control Mode");
		_LOG_ERR_P(szLog)
		bRslt = FALSE;
	}
	_LOG_PROC("-----===> Finished TV Setting");

	if(bRslt == TRUE) m_nTV_Thread = THREAD_STOP;
	else m_nTV_Thread = THREAD_FAILED;
}

DWORD WINAPI CProcessSequence::TV_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *)lParam;
	pBaseApp->SetTV();
	return 0;
}

void CProcessSequence::StartTVSetting(int nStep)
{
	m_nTV_Thread = THREAD_RUN;
	m_nTV_Step = nStep;
	RunThreadFunc(TV_ThreadProc, this);
}

void CProcessSequence::StartTVMonitor(int nStep)
{
	int nIdx;
	char szTVParam[MAX_PARAM_SIZE];
	double dblValue;
	char szLog[200];

	// Pressure Setting
	_LOG_PROC("-----===> Started TV Monitoring");
	nIdx = nStep - 1;
	dblValue = m_RcpStep[nIdx].dblTVPRS_Sp;
	if(TVM_POSITION == m_RcpStep[nIdx].nTVPRS_Option)
	{
		sprintf(szTVParam, "MON_POS %d 3", (int) dblValue);
		RUN_SET_FUNCTION(PRS_MONITOR, szTVParam);
	}
	else if(TVM_PRESSURE <= m_RcpStep[nIdx].nTVPRS_Option)
	{
		sprintf(szTVParam, "MON_PRS %7.3f %d", dblValue,
				m_RcpStep[nIdx].nTVPRS_Option);
		RUN_SET_FUNCTION(PRS_MONITOR, szTVParam);
	}
	else
	{
		ALARM_POST(ALARM_SW_FUNC_ERROR_OCCURED);
		sprintf(szLog, "-----=====> Invalid Throttle Valve Control Mode");
		_LOG_ERR_P(szLog)
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL CProcessSequence::SetStagePositionNoWait(double dbPos, double dbPos2)
{
	BOOL bRet = TRUE;
	char szTemp[40];

	printf("[DEBUG] Entered SetStagePositionNoWait Pos[%.0f] Pos2[%.0f]\n", dbPos, dbPos2);

	sprintf(szTemp, "RUN 2 %0.2f %0.2f", dbPos, dbPos2);
	_LOG_PROC("[UPEH-STEP] SET CHUCK_CONTROL[%s] Started", szTemp);

	RUN_SET_FUNCTION(CHUCK_CONTROL, szTemp);

	_LOG_PROC("[UPEH-STEP] SET CHUCK_CONTROL[%s] End", szTemp);
	printf("[DEBUG] Leaved SetStagePositionNoWait bRet[%d]\n", bRet);

	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Z-Motion Setting Function
BOOL CProcessSequence::SetStagePositionEx(double dbPos, double dbPos2)
{
	BOOL bRet = TRUE;
	char szTemp[40];

	printf("[DEBUG] Entered SetStagePositionEx Pos[%.0f] Pos2[%.0f]\n", dbPos, dbPos2);

	sprintf(szTemp, "RUN 2 %0.2f %0.2f", dbPos, dbPos2);
	_LOG_PROC("[UPEH-STEP] RUN CHUCK_CONTROL[%s] Started", szTemp);

	if(SYS_SUCCESS != RUN_FUNCTION(CHUCK_CONTROL, szTemp))		bRet = FALSE;

	_LOG_PROC("[UPEH-STEP] RUN CHUCK_CONTROL[%s] End", szTemp);
	printf("[DEBUG] Leaved SetStagePositionEx bRet[%d]\n", bRet);

	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Stage Heater Temp Setting Function
// 2004.11.17 by cskim

void CProcessSequence::SetStgHtr()
{
	BOOL bRslt = TRUE;
	Module_Status msRslt = SYS_SUCCESS;
	char szTemp[40] = "";
	char szStgOffs[10] = "";
	int nHtrTemp;
	int nRampRate;
	int nIdx;
	nIdx = m_nStgHtr_Step - 1;

	_LOG_PROC("-----===> Started StgHtr Temp Setting");
	nHtrTemp = (int) m_RcpStep[nIdx].dbStgHtr1_SetPoint;

	if(m_RcpStep[nIdx].nRamp1_Option == 1)		nRampRate = (int) m_RcpStep[nIdx].dbRamp1_SetPoint;
	else										nRampRate = -1;

	// 2006.01.16
	if(m_RcpStep[nIdx].StgOffs_nOpt == 1)		sprintf(szStgOffs, " %d", (int) m_RcpStep[nIdx].StgOffs_dbSp);

	do {
		// 0:No 1:Check 2:SetTemp 3:Set_If_Upper 4:Set_If_Lower
		if(m_RcpStep[nIdx].nStgHtr1_Option == 0) break;

		switch(m_RcpStep[nIdx].nStgHtr1_Option)
		{
		case 1:
			sprintf(szTemp, "CHECK_SET %d %s", nHtrTemp, szStgOffs);	// 2006.02.27
			msRslt = RUN_FUNCTION(STGHTR_CONTROL, szTemp);
			if(msRslt != SYS_SUCCESS)
			{
				_LOG_PROC("-----=====> Stage Heater Temp Check Failed");
				ALARM_POST(ALARM_STGHTR_TEMP_CHECK_FAILED);
				bRslt = FALSE;
			}
			break;
		case 2:
			sprintf(szTemp, "SET_TEMP %d %d %s", nHtrTemp, nRampRate, szStgOffs);	// 2006.01.16
			RUN_FUNCTION(STGHTR_CONTROL, szTemp);
			break;
		case 3:
			sprintf(szTemp, "SET_IF_UPPER %d %d", nHtrTemp, nRampRate);
			RUN_FUNCTION(STGHTR_CONTROL, szTemp);
			break;
		case 4:
			sprintf(szTemp, "SET_IF_LOWER %d %d", nHtrTemp, nRampRate);
			RUN_FUNCTION(STGHTR_CONTROL, szTemp);
			break;
		}
		_LOG_PROC("-----=====> SetCommand:%s", szTemp);
	} while(0);
	_LOG_PROC("-----===> Finished StgHtr Temp Setting");

	if(bRslt == TRUE) m_nStgHtr_Thread = THREAD_STOP;
	else m_nStgHtr_Thread = THREAD_FAILED;
}

DWORD WINAPI CProcessSequence::StgHtr_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *) lParam;
	pBaseApp->SetStgHtr();
	return 0;
}

void CProcessSequence::StartStgHtrSetting(int nStep)
{
	m_nStgHtr_Thread = THREAD_RUN;
	m_nStgHtr_Step = nStep;
	RunThreadFunc(StgHtr_ThreadProc, this);
}

//////////////////////////////////////////////////////////////////////////
void CProcessSequence::StartHeaterSetting(int nStep)
{
	char szParam[MAX_PARAM_SIZE];
	char szTemp[40];
	int i;
	int nIdx = nStep - 1;

	memset(szParam, 0, sizeof(szParam));
	strcpy(szParam, "SET_TEMP");

	for(i = 0; i < MAX_HEATER; i++)
	{
		if(m_RcpStep[nIdx].HTR_nOpt[i] == 1)
		{
			sprintf(szTemp, " %d %.3f", i+1, m_RcpStep[nIdx].HTR_dbSp[i]);
			strcat(szParam, szTemp);
		}
	}

	RUN_FUNCTION(HTR_CONTROL, szParam);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Mfc Setting Function and Thread
// 2004.09.10 by cskim

void CProcessSequence::SetMFC()
{
	BOOL bRslt = TRUE;
	char szTemp[40];
	char szLog[1000];
	int nIdx;
	char szMfcParam[MAX_PARAM_SIZE] = "";
	int nMfcCount = 0;
	int i;
	BOOL bMfcMonitor = FALSE;
	memset(szLog, 0, sizeof(szLog));
	memset(szMfcParam, 0, sizeof(szMfcParam));

	// 2006.06.14
	enum { ALMOPT_IGNORE, ALMOPT_WARNING, ALMOPT_CHMDIS, ALMOPT_ABORT, ALMOPT_RETRY };
	static int anMfcAlarmOption[] = {
		ALMOPT_IGNORE,			// MFC_OPT_IGNORE
		ALMOPT_WARNING,			// MFC_OPT_WARNING
		ALMOPT_CHMDIS,			// MFC_OPT_CHMDIS
		ALMOPT_ABORT,			// MFC_OPT_ABORT
		0,					// MFC_OPT_OPEN
		0,					// MFC_OPT_U_WARNING
		0,					// MFC_OPT_U_CHMDIS
		0,					// MFC_OPT_U_ABORT
		ALMOPT_RETRY,			// MFC_OPT_RETRY
	};

	nIdx = m_nMFC_Step - 1;
	_LOG_PROC("-----===> Started MFC Setting");

	nMfcCount = 0;
	if(m_bMfcSetEnable)
	{
		if(m_bMfcMonEnable) sprintf(szMfcParam, "SET_VALUE_P_O");
		else sprintf(szMfcParam, "SET_VALUE_P");

		for(i=0; i<m_nMaxMfc; i++)
		{
			if(m_pnMFC_Scale[i] != 0)
			{
				if(m_RcpStep[nIdx].pdbMFC_Sp[i] > 0
					|| m_RcpStep[nIdx].pnMFC_Option[i] == MFC_OPT_OPEN)
				{
					// 2006.04.17
					if(m_bMfcMonEnable &&
							(m_RcpStep[nIdx].pnMFC_Option[i] <= MFC_OPT_OPEN
							|| MFC_OPT_RETRY == m_RcpStep[nIdx].pnMFC_Option[i])
						)
					{	//... Valid MFC Option
						// 2006.01.18
						sprintf(szTemp, " %d %6.1f %d",
								i+1, m_RcpStep[nIdx].pdbMFC_Sp[i],
								anMfcAlarmOption[m_RcpStep[nIdx].pnMFC_Option[i]]);
						AddString(szMfcParam, szTemp);
						nMfcCount++;
						if(MFC_OPT_WARNING == m_RcpStep[nIdx].pnMFC_Option[i]
							|| MFC_OPT_CHMDIS == m_RcpStep[nIdx].pnMFC_Option[i]
							|| MFC_OPT_ABORT == m_RcpStep[nIdx].pnMFC_Option[i]
							|| MFC_OPT_RETRY == m_RcpStep[nIdx].pnMFC_Option[i])
							bMfcMonitor = TRUE;
					}
					else
					{
						// 2006.01.18
						sprintf(szTemp, " %d %6.1f F",
								i+1, m_RcpStep[nIdx].pdbMFC_Sp[i]);
						AddString(szMfcParam, szTemp);
						nMfcCount++;
					}
				}
			}
		}
	}
	else
	{
		if(m_nMfcControlMode == MFCCM_PARAM_TOL)
		{
			sprintf(szMfcParam, "START_MON_P_O2");
			for(i=0; i<m_nMaxMfc; i++)
			{
				// 2006.06.14
				if(m_pnMFC_Scale[i] != 0 && m_RcpStep[nIdx].pdbMFC_Sp[i] > 0)
				{
					if( (MFC_OPT_WARNING == m_RcpStep[nIdx].pnMFC_Option[i]
						|| MFC_OPT_CHMDIS == m_RcpStep[nIdx].pnMFC_Option[i]
						|| MFC_OPT_ABORT == m_RcpStep[nIdx].pnMFC_Option[i]
						|| MFC_OPT_RETRY == m_RcpStep[nIdx].pnMFC_Option[i])
						)
					{
						bMfcMonitor = TRUE;
						// 2006.01.18
						sprintf(szTemp, " %d %.1f %d", i+1, m_RcpStep[nIdx].pdbMFC_Sp[i],
								anMfcAlarmOption[m_RcpStep[nIdx].pnMFC_Option[i]]);
						AddString(szMfcParam, szTemp);
						nMfcCount++;
					}
				}
			}
		}
		else // if(m_nMfcControlMode == MFCCM_RECIPE_TOL)
		{
			// 2006.09.05
			sprintf(szMfcParam, "START_MON_P_O3");
			for(i=0; i<m_nMaxMfc; i++)
			{
				if(m_pnMFC_Scale[i] != 0 && m_RcpStep[nIdx].pdbMFC_Sp[i] > 0)
				{
					if( (MFC_OPT_WARNING == m_RcpStep[nIdx].pnMFC_Option[i]
						|| MFC_OPT_CHMDIS == m_RcpStep[nIdx].pnMFC_Option[i]
						|| MFC_OPT_ABORT == m_RcpStep[nIdx].pnMFC_Option[i]
						|| MFC_OPT_RETRY == m_RcpStep[nIdx].pnMFC_Option[i])
						)
					{
						bMfcMonitor = TRUE;
						// 2006.01.18
						sprintf(szTemp, " %d %.1f %.1f %d", i+1, m_RcpStep[nIdx].pdbMFC_Sp[i],
								m_RcpStep[nIdx].pdbMFC_Tol[i],
								anMfcAlarmOption[m_RcpStep[nIdx].pnMFC_Option[i]]);
						AddString(szMfcParam, szTemp);
						nMfcCount++;
					}
				}
			}
		}
	}

	if(nMfcCount > 0)
	{
		RUN_SET_FUNCTION(MFC_CONTROL, szMfcParam);
		sprintf(szLog, "-----=====> MFC Set : %s", szMfcParam);
		_LOG_PROC(szLog);
	}
	m_bMfcMonitor = bMfcMonitor;
	_LOG_PROC("-----===> Finished MFC Setting");

	if(bRslt == TRUE) m_nMFC_Thread = THREAD_STOP;
	else m_nMFC_Thread = THREAD_FAILED;
}

DWORD WINAPI CProcessSequence::MFC_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *) lParam;
	pBaseApp->SetMFC();
	return 0;
}

void CProcessSequence::StartMFCSetting(int nStep, BOOL bSetEnable, BOOL bMonEnable)
{
	m_nMFC_Thread = THREAD_RUN;
	m_nMFC_Step = nStep;
	m_bMfcMonitor = FALSE;
	m_bMfcSetEnable = bSetEnable;
	m_bMfcMonEnable = bMonEnable;
	RunThreadFunc(MFC_ThreadProc, this);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Timer Thread
// 2005.04.18 by cskim

DWORD WINAPI CProcessSequence::Tmr_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *) lParam;
	while(pBaseApp->m_nTmr_Thread == THREAD_RUN)
	{
		Sleep(250);
		pBaseApp->UpdateStepTime();
	}
	return 0;
}

void CProcessSequence::InitStepTimer()
{
	m_nTmr_Thread = THREAD_RUN;
	m_pStepTimer->InitTimer();
	UpdateStepTime();
	RunThreadFunc(Tmr_ThreadProc, this);
}

void CProcessSequence::StopStepTimer()
{
	m_nTmr_Thread = THREAD_STOP;
}

void CProcessSequence::UpdateStepTime()
{
	double dbStepTime;
	double dbStpCurTm;
	int nIOStatus;

	dbStpCurTm = m_pStepTimer->GetProgressTime();
	dbStpCurTm /= 1000;
	switch(m_nStepTimeMode)
	{
	case 1:	dbStepTime = dbStpCurTm;
			WRITE_ANALOG(PRO_StepTimeAM, dbStepTime, &nIOStatus);
			break;
	case 2: dbStepTime = dbStpCurTm * 2;
			WRITE_ANALOG(PRO_StepTimeAM, dbStepTime, &nIOStatus);
			break;
	}
	WRITE_ANALOG(PRO_StpCurTmAM, dbStpCurTm, &nIOStatus);
}

void CProcessSequence::UpdateStepTotalTime(int nStep)
{
	int nIOStatus;
	double dbStepTime;
	if(nStep > 0) dbStepTime = GetProcStepTime(nStep);
	else dbStepTime = 0;
	WRITE_ANALOG(PRO_StpTotTmAM, dbStepTime, &nIOStatus);
	if(m_nStepTimeMode == 0) WRITE_ANALOG(PRO_StepTimeAM, dbStepTime, &nIOStatus);

	//--------------------------------------------------------
	// 2006.05.12
	int nIdx;
	if(m_pDynIO->IOValid(PRO_StdStpTmAM))
	{
		nIdx = nStep - 1;
		m_pDynIO->dWRITE_ANALOG(PRO_StdStpTmAM, m_RcpStep[nIdx].StepTimeCompen_dbStdStepTime, &nIOStatus);
	}
	//--------------------------------------------------------
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Timer Thread
// 2005.07.14, Data Log Timming

DWORD WINAPI CProcessSequence::DCOPLog_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *) lParam;
	pBaseApp->DoDCOPLog();
	return 0;
}

void CProcessSequence::StartDCOPLog()
{
	RunThreadFunc(DCOPLog_ThreadProc, this);
}

void CProcessSequence::DoDCOPLog()
{
	int nStep, nIdx;
	int nIOStatus;
	char szParam[256] = "";
	int nMainStepNumber;
	int i;

	nStep = m_ProcQ[QSTEP_RUN].nStep;
	nIdx = nStep - 1;
	nMainStepNumber = GetMainStepNumber(nStep);

	// 2005.02.18 by cskim, Multi DCOP Log
	sprintf( szParam, "STEP %d|%s", nMainStepNumber, m_RcpStep[nIdx].szStepName);
	WRITE_STRING(PROCESS_LOG_EVENT, szParam, &nIOStatus);

	for(i=0; i<5; i++)
	{
		// PROCESS_LOG_STS => 0:Available 1:Running 2:Aborted 3:Success 4:Error 5:Kill 6:Unknown
		if(READ_DIGITAL(PROCESS_LOG_STS, &nIOStatus) != 1) break;
		if(! WAIT_SECONDS(0.1)) break;
	}

	WRITE_DIGITAL(PRO_IsMainDM, 1, &nIOStatus);		// 0:NoMain 1:Main
}

//---------------------------------------------------------------------------------------
//... 2016.01.19 PM Leak Check Recipe Stop
BOOL CProcessSequence::IsPurgeStop()
{
	BOOL bRet = FALSE;
	int nIOStatus;
	// 0:Stop 1:Running
	if(READ_DIGITAL(PRO_PurgeRcpStsDM, &nIOStatus) == 0) bRet = TRUE;
	return bRet;
}
//---------------------------------------------------------------------------------------
// Process Monitoring
BOOL CProcessSequence::CheckMonitoringStatus()
{
	enum { PRO_ABORT, PRO_RETRY };		//... 2015.03.23 Add PLC Interlock Retry

	BOOL bRet = FALSE;
	int nIOStatus;
	int nSW_GbIntlckDM;
	Module_Status msRslt;
	char szLog[200];

	//... 2015.02.26 RPG Alarm Auto Retry
	int	nRPGAlmDesc = eRPGALM_NONE;

	do {
		if(CheckSettingThread() == FALSE) break;

		if(m_bMfcMonitor == TRUE)
		{
			msRslt = (Module_Status) READ_FUNCTION(MFC_CONTROL);
			if(msRslt == SYS_ABORTED || msRslt == SYS_ERROR)
			{
				sprintf(szLog, "-----> MFC_CONTROL Function Failed");
				_LOG_PROC(szLog); printf("%s\n", szLog);
				break;
			}
		}

		msRslt = (Module_Status) READ_FUNCTION(PROCBGRUN_CONTROL);
		if(msRslt == SYS_ABORTED || msRslt == SYS_ERROR)
		{
			sprintf(szLog, "-----> PROCBGRUN_CONTROL Function Failed");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		msRslt = (Module_Status) READ_FUNCTION(PRS_MONITOR);
		if(msRslt == SYS_ABORTED || msRslt == SYS_ERROR)
		{
			sprintf(szLog, "-----> PRS_MONITOR Function Failed");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}
		//2010.12.10 by mgsong

		if(m_nRF_UseDM == 1)
		{
			msRslt = (Module_Status) READ_FUNCTION(RF_CONTROL);
			if(msRslt == SYS_ABORTED || msRslt == SYS_ERROR)
			{
				sprintf(szLog, "-----> RF_CONTROL Function Failed");
				_LOG_PROC(szLog); printf("%s\n", szLog);
				break;
			}
			//2008.09.29
			msRslt = (Module_Status) READ_FUNCTION(RF_CONTROL2);
			if(msRslt == SYS_ABORTED || msRslt == SYS_ERROR)
			{
				sprintf(szLog, "-----> RF_CONTROL2 Function Failed");
				_LOG_PROC(szLog); printf("%s\n", szLog);
				break;
			}
		}

		// Check PLC Global Interlock Monitoring
		if(enOFFON_ON == READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus))	// 0:Normal 1:Interlock Present
		{
			//... 2015.03.23 Add PLC Interlock Retry
			if(RT_IdleRcp != m_RcpHeader.nRecipeType)
				m_pDynIO->dWRITE_DIGITAL(PRO_FlowControlDM, PRO_RETRY, &nIOStatus);

			sprintf(szLog, "-----> PLC_Interlock Detected");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		// Check SW Interlock Monitoring
		nSW_GbIntlckDM = READ_DIGITAL(SW_GbIntlckDM, &nIOStatus); 	// 0:Normal 1:Interlock Present
		if(nSW_GbIntlckDM == 1)
		{
			sprintf(szLog, "-----> SW_Interlock Detected");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		// Idle Stop Signal Monitoring
		if(m_bIdleStopEnable && IsIdleStop())
		{
			m_bProcTimeInterlock = false;
			sprintf(szLog, "-----> IdleRecipe Stopped in CheckMonitoringStatus");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		//... 2016.01.19 Purge Stop Signal Monitoring
		if(m_RcpHeader.nRecipeType == RT_PurgeRcp){
			if(IsPurgeStop())
			{
				m_bProcTimeInterlock = false;
				sprintf(szLog, "-----> Purge Recipe Stopped in CheckMonitoringStatus");
				_LOG_PROC(szLog); printf("%s\n", szLog);
				break;
			}
		}

		// Check TM Isoloation Valve
		// 2004.12.29 by cskim
		// 0:False 1:True
		if(0 == READ_DIGITAL(TMIsoVlvClosedDI, &nIOStatus))
		{
			int nAlarm = ALARM_TM_ISO_VALVE_NOT_CLOSE;
			if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
			sprintf(szLog, "-----> TM Isolation Valve Is Opened When Running");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		if(0 == READ_DIGITAL(TMIsoVlv2ClosedDI, &nIOStatus))
		{
			int nAlarm = ALARM_TM_ISO_VALVE_NOT_CLOSE;
			if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
			sprintf(szLog, "-----> TM Isolation Valve Is Opened When Running");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		// Check Process Running Status
		if(READ_DIGITAL(PRO_RunStsDM, &nIOStatus) == RUNSTS_Stop)		// 0:Stop 1:Running
		{
			sprintf(szLog, "-----> Process Running Status Channel Drop Checked in CheckMonitoringStatus");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		//... 2015.01.06 Add Process Stop Channel
		if(READ_DIGITAL(PRO_StopCtrlDM, &nIOStatus) == enOFFON_ON)
		{
			sprintf(szLog, "-----> Process Stop Channel On Checked in CheckMonitoringStatus");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}

		//... 2015.02.26 RPG Alarm Auto Retry (Only Lot Pre & Dry Clean)
		if(RT_LotPreRcp == m_RcpHeader.nRecipeType || RT_CleanRcp == m_RcpHeader.nRecipeType)
		{
			nRPGAlmDesc = READ_DIGITAL(RPGAlmPrcRetryDM, &nIOStatus);
			if(eRPGALM_NONE != nRPGAlmDesc)
			{	//... Checked RPG Auto Retry Alarm
				sprintf(szLog, "[ERROR] Process Stop by RPG Auto Retry AlmDesc[%d]", nRPGAlmDesc);
				_LOG_ERR_P(szLog);

				//... Set Process Flow Retry On
				enum { PRO_ABORT, PRO_RETRY };
				m_pDynIO->dWRITE_DIGITAL(PRO_FlowControlDM, PRO_RETRY, &nIOStatus);

				break;
			}
		}

		// Manager Abort
		if(MANAGER_ABORT())
		{
			sprintf(szLog, "-----> Manager Abort in CheckMonitoringStatus");
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}
		bRet = TRUE;
	} while(0);

	return bRet;
}

BOOL CProcessSequence::CheckSettingThread()
{
	BOOL bRet = TRUE;

	if(m_nStgHtr_Thread == THREAD_FAILED)
	{
		bRet = FALSE;
	}

	if(m_nStgHtr2_Thread == THREAD_FAILED)
	{
		bRet = FALSE;
	}

	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Sub Functions 2

BOOL CProcessSequence::ReadyPLC()
{
	BOOL bRet = FALSE;
	int nIOStatus;
	BOOL bMainCheck = FALSE;
	BOOL bSubCheck = FALSE;
	Module_Status msFlag = SYS_SUCCESS;
	int nCount;

	_LOG_PROC("---> Entered ReadyPLC");
	do {
		// 0:Stop 1:Start
		WRITE_DIGITAL(VRCP_StaStpDO, 0, &nIOStatus);

		//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)
		WRITE_DIGITAL(VRCP_ResumeDO, 0, &nIOStatus);
		WRITE_DIGITAL(VRCP_GoNextDO, 0, &nIOStatus);

		if ( 2 == m_nVRCP_DnMethodDM )	{		//0:Channel 1:SharedMemory 2:EtherNet
			bRet = TRUE;
			break;
		}
		//
		bSubCheck = FALSE;
		for( nCount = 0; nCount < MAX_EMPTY_MAILBOX_CHECK; nCount++)
		{
			WRITE_DIGITAL( DN_GetMessage_DO, 1, &nIOStatus);
			if( FALSE == nIOStatus )
			{
				bSubCheck = TRUE;
				break;
			}
			else
			{
				_LOG_PROC("-----> Clear DN_MAIL_BOX");
				if( FALSE == WAIT_SECONDS(0.05))
				{
					msFlag = SYS_ABORTED;
					_LOG_PROC("-----> Sys Aborted while Mail box Check !");
					break;
				}
			}
		}//for( nCount = 0; nCount < MAX_EMPTY_MAILBOX_CHECK; nCount++)
		if( SYS_SUCCESS != msFlag) break;
		if( FALSE == bSubCheck )
		{
			_LOG_PROC("-----> DN_MAIN_BOX Clear Failed !");
			break;
		}
		// _LOG_PROC("-----> Checked Empty Mail Box");
		// LOG( pszFtnName, _ELT_FileConsole, "1. Empty Mail Box OK!");

		// 2 Set Header Info
		// 2.1 Service Code
		//WRITE_DIGITAL( DN_SetSvcCode_DO, VSP_SERVICE_CODE, &nIOStatus);
		WRITE_DIGITAL( DN_SetSvcCode_DO, 0x1E, &nIOStatus);
		if( FALSE == nIOStatus )
		{
			_LOG_PROC("WRITE_DIGITAL[DN_SetSvcCode_DO, VSP_SERVICE_CODE] Failed");
			bMainCheck = FALSE;
			break;
		}
		bMainCheck = TRUE;
		// LOG( pszFtnName, _ELT_FileConsole, "2.1 Service Code OK !");
		// _LOG_PROC("-----> Service Code Write OK");


		// 2.2 Class ID
		//WRITE_DIGITAL( DN_SetClsID_DO, VSP_CLASS_ID, &nIOStatus);
		WRITE_DIGITAL( DN_SetClsID_DO, 0x2F, &nIOStatus);
		if( FALSE == nIOStatus )
		{
			_LOG_PROC("WRITE_DIGITAL[DN_SetClsID_DO, VSP_CLASS_ID] Failed");
			bMainCheck = FALSE;
			break;
		}
		bMainCheck = TRUE;
		// _LOG_PROC("-----> Class ID Write OK");
		// LOG( pszFtnName, _ELT_FileConsole, "2.2 Class ID Code OK !");

		// 2.3 Instance ID
		//WRITE_DIGITAL( DN_SetInstID_DO, VSP_INSTANCE_ID, &nIOStatus);
		WRITE_DIGITAL( DN_SetInstID_DO, 0x03, &nIOStatus);
		if( FALSE == nIOStatus )
		{
			_LOG_PROC("WRITE_DIGITAL[DN_SetInstID_DO, VSP_INSTANCE_ID] Failed");
			bMainCheck = FALSE;
			break;
		}
		// LOG( pszFtnName, _ELT_FileConsole, "2.3 Instance ID Code OK !");
		// _LOG_PROC("-----> Instance ID Write OK");

		// LOG( pszFtnName, _ELT_FileConsole, "2. Set Header Info OK!");
		// _LOG_PROC("-----> MESSAGE Header Data Setup with Packet OK");

		bRet = TRUE;
	} while(0);
	_LOG_PROC("---> Leaved ReadyPLC");
	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Process Step Queue
// 2004.09.21 by cskim

void CProcessSequence::Init_ProcStepQueue()
{
	int i;
	for(i=0; i<MAX_QUEUE; i++)				// 2013.06.25
	{
		m_ProcQ[i].nStep = 0;
		m_ProcQ[i].nCmd = 0;
		m_ProcQ[i].bDownload = FALSE;		// 2013.06.25
		// m_ProcQ[i].nMode = VRM_CONT;
	}

	// Init Recipe Buffer Page
	m_nVRCPBufPage = 0;

	// Init SettingThread
	m_nDL_Thread = THREAD_STOP;
	m_nTV_Thread = THREAD_STOP;
	m_nMFC_Thread = THREAD_STOP;

	m_nStgHtr_Thread = THREAD_STOP;
	m_nStgHtr2_Thread = THREAD_STOP;
	m_nQueuedDnLdStep = 0;

	m_nDnLdStepCount = 0;
	m_nDnRdyStepCount = 0;
	m_nRunStepCount = 1;  //2014.08.25
}

//////////////////////////////////////////////////////////////////////////
BOOL CProcessSequence::Start_DownloadValveRecipe()
{
	BOOL bRet = TRUE;
	int nStep, nIdx;
	int nNextStep, nNextIdx;
	int nVRCPMode;
	char szLog[200];

	nNextStep = m_ProcQ[m_nQStepPreDownload].nStep;
	nStep = m_ProcQ[m_nQStepDownload].nStep;
	if(nStep == 0) return TRUE;		// Step is not existed
	sprintf(szLog, "---> [Step:%d] Start_DownloadValveRecipe Called", nStep);
	_LOG_PROC(szLog);

	nIdx = nStep - 1;
	nNextIdx = nNextStep - 1;

	nVRCPMode = VRM_CONT;
	// When not use ValveRecipe
	if(ED_Disable == m_RcpStep[nIdx].nVR_Enable)			nVRCPMode = VRM_NO_VRCP;
	// When need to pause ValveRecipe
	else if(nNextStep == 0)									nVRCPMode = VRM_PAUSE;
	else if(ED_Disable == m_RcpStep[nNextIdx].nVR_Enable)	nVRCPMode = VRM_PAUSE;
	else
	{
		//////////////////////////////////////////////////////////////////////////
		//... 2015.03.21 Add Position Move Option
		//... Next Download Step VRCP_PAUSE 사용 조건 Fix (Next Step 기준)
		if(ZPOSOPT_Wait == m_RcpStep[nNextIdx].nSTGZPOS_WaitOpt)
		{
			//... Check Before Step Wait Option & Position
			if(ZPOSOPT_NoWait == m_RcpStep[nIdx].nSTGZPOS_WaitOpt)
			{	//... Set Pause by Different Wait Option
				nVRCPMode = VRM_PAUSE;
				_LOG_PROC("[INFO] DN_STEP[%d] PAUSE SET by Different WaitOpt[%d][%d]",
					nStep,
					m_RcpStep[nIdx].nSTGZPOS_WaitOpt, m_RcpStep[nNextIdx].nSTGZPOS_WaitOpt);
			}
			else
			{	//... Set Pause by Different Position
				if( (m_RcpStep[nIdx].dbSTGZPOS_Sp  != m_RcpStep[nNextIdx].dbSTGZPOS_Sp) ||
					(m_RcpStep[nIdx].dbSTGZPOS2_Sp != m_RcpStep[nNextIdx].dbSTGZPOS2_Sp) )
				{
					nVRCPMode = VRM_PAUSE;
					_LOG_PROC("[INFO] DN_STEP[%d] PAUSE SET by Different Pos Cur[%.2f][%.2f] Nxt[%.2f][%.2f]",
						nStep,
						m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp,
						m_RcpStep[nNextIdx].dbSTGZPOS_Sp, m_RcpStep[nNextIdx].dbSTGZPOS2_Sp);
				}
			}
		}
	}

	m_RcpStep[nIdx].nVRCPMode = nVRCPMode;
	bRet = Start_DownloadThread(nStep);

	if(! bRet)
	{
		sprintf(szLog, "---> [Step:%d] Start_DownloadValveRecipe Failed", nStep);
		_LOG_ERR_P(szLog)
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2017.02.27
BOOL CProcessSequence::ZmotionCtrlBaseOnParameter()
{
	BOOL bRet = TRUE;
	int nStep, nIdx;
	int nPrevStep, nPrevIdx;

	nPrevStep = m_ProcQ[QSTEP_END].nStep;
	nPrevIdx = nPrevStep - 1;

	nStep = m_ProcQ[QSTEP_RUN].nStep;
	nIdx = nStep - 1;
	if(nStep == 0) return TRUE;

	if( (m_RcpStep[nIdx].dbSTGZPOS_Sp  != m_RcpStep[nPrevIdx].dbSTGZPOS_Sp) ||
		(m_RcpStep[nIdx].dbSTGZPOS2_Sp != m_RcpStep[nPrevIdx].dbSTGZPOS2_Sp))
	{
		if( SYS_RUNNING == READ_FUNCTION(CHUCK_CONTROL)) m_nFunctionSetCnt++;
		else m_nFunctionSetCnt = 0;

		if(m_nFunctionSetCnt >= 2)
		{
			ALARM_POST(ALARM_CHUCK_MOVEING_STEP_SETTING);
			return FALSE;
		}

		_LOG_PROC("[INFO] Chuck Move NoWait by Parameter m_FuncCnt[%d]",m_nFunctionSetCnt);
		bRet = SetStagePositionNoWait(m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp);
	}
	else
	{
		// Same Option & Same Position
		_LOG_PROC("[INFO] Chuck Move Skip by Same NoWait Opt and Same Pos");
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CProcessSequence::Preset_Controller()
{
	BOOL bRet = TRUE;
	int nStep, nIdx;
	int nPrevStep, nPrevIdx;
	int nIOStatus;
	char szLog[200];
	int nTimeout = 0;

	nPrevStep = m_ProcQ[QSTEP_END].nStep;
	nPrevIdx = nPrevStep - 1;

	if(nPrevStep != 0)
	{
		// Stop Monitoring
		_LOG_PROC("-----> Start Killing Process Monitoring");
		RUN_FUNCTION_ABORT(MFC_CONTROL);
		RUN_FUNCTION_ABORT(PROCBGRUN_CONTROL);
		RUN_FUNCTION_ABORT(PRS_MONITOR);
		_LOG_PROC("-----> Killed Monitoring Function");

		if(m_nRF_UseDM == 1)
		{
			RUN_FUNCTION_ABORT(RF_CONTROL);
			_LOG_PROC("-----> Killed RF Monitoring Function");
			//2008.09.29
			RUN_FUNCTION_ABORT(RF_CONTROL2);
			_LOG_PROC("-----> Killed RF Monitoring Function");

		}
	}

	nStep = m_ProcQ[QSTEP_RUN].nStep;
	nIdx = nStep - 1;
	if(nStep == 0) return TRUE;		// If Step is not existed

	sprintf(szLog, "---> [Step:%d] Preset_Controller Called", nStep);
	_LOG_PROC(szLog);

	//////////////////////////////////////////////////////////////////////////
	do {
		if(m_RcpStep[nIdx].nStgHtr1_Option == 1) StartStgHtrSetting(nStep);
		if(m_RcpStep[nIdx].nStgHtr2_Option == 1) StartStgHtr2Setting(nStep);

		//////////////////////////////////////////////////////////////////////////
		// StagePosition Setting
		//... 2015.03.21 Add Position Move Option
		//		Recipe Download 시 VRCP Pause 조건과 동일하게 확인이 필요함
		if(nPrevStep == 0)
		{

			//////////////////////////////////////////////////////////////////////////
			//... 2017.03.07 add parameter option for z-motion control first
			if(OFFON_ON == m_nZCtrl_RCPSkipOption && m_RcpHeader.nRecipeType == m_nZCtrl_RCPType)
			{
				_LOG_PROC("[INFO] Chuck Move NoWait by Parameter FirstStep");
				bRet = SetStagePositionNoWait(m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp);
			}
			//....
			else
			{
				//... First Step (Not Pause Check)
				if(ZPOSOPT_Wait == m_RcpStep[nIdx].nSTGZPOS_WaitOpt)
				{
					_LOG_PROC("[INFO] Chuck Move Wait by Opt in First Step");
					bRet = SetStagePositionEx(m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp);
				}
				else
				{
					_LOG_PROC("[INFO] Chuck Move NoWait by Opt in First Step");
					bRet = SetStagePositionNoWait(m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp);
				}
			}

			while(1)
			{
				if(0 >= m_nZCtrl_PrcStartDelayTime) break;
				if(! WAIT_SECONDS(1)) break;
				nTimeout++;
				if(nTimeout >= m_nZCtrl_PrcStartDelayTime) break;
			}
			if(0 < m_nZCtrl_PrcStartDelayTime)
				_LOG_PROC("[INFO] Chuck Move NoWait by Parameter(ZCtrl_Delay) Time[%d]",nTimeout);
		}
		else //... Not First Step
		{
			//... 2017.03.07 add parameter option for z-motion control first
			if(OFFON_ON == m_nZCtrl_RCPSkipOption && m_RcpHeader.nRecipeType == m_nZCtrl_RCPType)
			{
				bRet = ZmotionCtrlBaseOnParameter();
				if(bRet == FALSE) return FALSE;
				break;
			}

			if(ZPOSOPT_Wait == m_RcpStep[nIdx].nSTGZPOS_WaitOpt)
			{
				//... Check Before Step Wait Option & Position
				if(ZPOSOPT_NoWait == m_RcpStep[nPrevIdx].nSTGZPOS_WaitOpt)
				{	//... Set Pause by Different Wait Option
					if(0 == READ_DIGITAL(VRCP_PauseDI, &nIOStatus))		// VRCP_PauseDI => 0:Normal 1:Paused
					{
						ALARM_POST(ALARM_SW_AND_PLC_SYNC_FAILED);
						sprintf(szLog, "-----> [Step:%d] SW and PLC Sync Failed (PLC not paused when Z-Motion Setting)", nStep);
						_LOG_ERR_P(szLog)
							return FALSE;
					}

					_LOG_PROC("[INFO] Chuck Move Wait by Diff Opt");
					bRet = SetStagePositionEx(m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp);
				}
				else
				{	//... Set Pause by Different Position
					if( (m_RcpStep[nIdx].dbSTGZPOS_Sp  != m_RcpStep[nPrevIdx].dbSTGZPOS_Sp) ||
						(m_RcpStep[nIdx].dbSTGZPOS2_Sp != m_RcpStep[nPrevIdx].dbSTGZPOS2_Sp) )
					{	//... VRCP PAUSED
						if(0 == READ_DIGITAL(VRCP_PauseDI, &nIOStatus))		// VRCP_PauseDI => 0:Normal 1:Paused
						{
							ALARM_POST(ALARM_SW_AND_PLC_SYNC_FAILED);
							sprintf(szLog, "-----> [Step:%d] SW and PLC Sync Failed (PLC not paused when Z-Motion Setting)", nStep);
							_LOG_ERR_P(szLog)
								return FALSE;
						}
						_LOG_PROC("[INFO] Chuck Move Wait by Diff Pos");
						bRet = SetStagePositionEx(m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp);
					}
					else
					{	//... NOT VRCP PAUSED
						// Same Option & Same Position
						_LOG_PROC("[INFO] Chuck Move Skip by Same Wait Opt and Same Pos");
					}
				}
			}
			else
			{	//... NOT VRCP PAUSED
				if( (m_RcpStep[nIdx].dbSTGZPOS_Sp  != m_RcpStep[nPrevIdx].dbSTGZPOS_Sp) ||
					(m_RcpStep[nIdx].dbSTGZPOS2_Sp != m_RcpStep[nPrevIdx].dbSTGZPOS2_Sp))
				{
					_LOG_PROC("[INFO] Chuck Move NoWait by Diff Pos");
					bRet = SetStagePositionNoWait(m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp);
				}
				else
				{
					// Same Option & Same Position
					_LOG_PROC("[INFO] Chuck Move Skip by Same NoWait Opt and Same Pos");
				}
			}
		}

	} while(0);

	if(! bRet )
	{
		sprintf(szLog, "---> [Step:%d] Preset_Controller Failed", nStep);
		_LOG_PROC(szLog);
	}
	return bRet;
}

BOOL CProcessSequence::Start_ProcessStep()
{
	BOOL bRet = TRUE;
	int nStep, nIdx, nMode;
	int nPrevStep;
	int nNextStep;
	char szLog[100];
	int nIOStatus;

	memset(szLog, 0, sizeof(szLog));
	nStep = m_ProcQ[QSTEP_RUN].nStep;
	nPrevStep = m_ProcQ[QSTEP_END].nStep;
	nNextStep = m_ProcQ[QSTEP_RD1].nStep;

	// Step is not existed
	if(nStep == 0 && nPrevStep ==0 ) return TRUE;

	// NowStep is not existed
	sprintf(szLog, "---> [Step:%d] Start_ProcessStep Called", nStep);
	_LOG_PROC(szLog); printf("%s\n", szLog);
	if(nStep == 0)
	{
		// Last Step Ending is Checked
		if(nPrevStep != 0)
		{
			WRITE_DIGITAL(PRO_IsMainDM, 0, &nIOStatus);		// 0:NoMain 1:Main

			// VRCP_StaStpDO => 0:Stop 1:Start
			WRITE_DIGITAL(VRCP_StaStpDO, 0, &nIOStatus);
			// Stop Timer
			RUN_FUNCTION_FREE(TIME_MONITOR, "PAUSE");

			WRITE_DIGITAL(VRCP_ResumeDO, 0, &nIOStatus);	// 0:Off 1:On

			if(m_bProcTimeInterlock)
			{
				// If Progress Time and Target Time are deferent, Error is logged
				int nTtlTime = (int) READ_ANALOG(PRO_TotalTimeAM, &nIOStatus);
				int nCurTime = (int) READ_ANALOG(PRO_CurrTimeAM, &nIOStatus);
				if(abs(nTtlTime - nCurTime) >= 2)
				{
					_LOG_ERR("-----> Progress Time and Target Time are deferent [ProgressTime:%d, TargetTime:%d]",
							nCurTime, nTtlTime);
				}
			}
		}
		return TRUE;
	}

	// 2005.10.05 by cskim, Crash Point Fixed
	nIdx = nStep - 1;
	nMode = m_RcpStep[nIdx].nVRCPMode;

// 	// Increase Running Step Count
// 	m_nRunStepCount++;	//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)

	UpdateStepTotalTime(nStep);
	m_pStepTimer->StartTimer();
	UpdateStepTime();

	// Check MainStep
	if(m_RcpStep[nIdx].bIsMain)
	{
		// 2005.02.16 by cskim
		int nMainStepNumber;
		nMainStepNumber = GetMainStepNumber(nStep);
		if(nMainStepNumber > 0	&& !m_DummyWafer)	//2014.04.24 m_DummyWafer가 FALSE이면 DCOPLog를 Start 한다.
		{
			StartDCOPLog();
			sprintf(szLog, "-----> [Step:%d] Main Step Checked", nStep);
			_LOG_PROC(szLog); printf("%s\n", szLog);
		}
		else
		{
			sprintf(szLog, "-----> [Step:%d] Main Step Checked but Range is Over for MainStep Number", nStep);
			_LOG_ERR_P(szLog)
		}
	}
	else
	{
		WRITE_DIGITAL(PRO_IsMainDM, 0, &nIOStatus);		// 0:NoMain 1:Main
		sprintf(szLog, "-----> [Step:%d] Normal(Not Main) Step Checked", nStep);
		_LOG_PROC(szLog); printf("%s\n", szLog);
	}

	// Check Unlimited Cycle Step
	if(m_RcpStep[nIdx].nStepFlow_Option != SFL_NORMAL_CYC)
	{
		_LOG_PROC("-----> [Step:%d] Unlimited Cycle Step Checked", nStep);
	}

	// 2005.10.19, Process Step Sync between Modules
	if(m_RcpStep[nIdx].nStepFlow_Option == SFL_WAIT_STEPSYNC)
	{
		m_pDynIO->dWRITE_DIGITAL(PRO_PMStepWaitDM, 1, &nIOStatus);
		_LOG_PROC("-----> [Step:%d] PRO_PMStepWaitDM channel set to 1", nStep);
	}

	// Display Step Info
	if(m_RcpStep[nIdx].nLoopEnd != 0) IncreaseGrpCyc();
	DisplayStepInfo(nStep);

	// Now Step is not First Step
	if(nPrevStep != 0)
	{
		// MARS 2014.10.13 MARS
		if(m_RcpStep[nIdx].nStepFlow_Option == SFL_NORMAL_CYC)
		{
			LogEvent("StepProcess", "Start", nStep, "", "Time", m_RcpStep[nIdx].szStepName);
		}
		else
		{
			LogEvent("StepProcess", "Start", nStep, "", "Condition", m_RcpStep[nIdx].szStepName);
		}
		//...
		// if PLC is paused, . . . . .
		// VRCP_PauseDI => 0:Normal 1:Paused
		if(1 == READ_DIGITAL(VRCP_PauseDI, &nIOStatus))
		{
			if(2 == m_nVRCPBufPageMax)	// 2013.08.27
			{
				// Wait for Download Completed
				while(m_nDL_Thread != THREAD_STOP && m_nDL_Thread != THREAD_FAILED) WAIT_SECONDS(0.1);
				if(m_nDL_Thread == THREAD_FAILED) return FALSE;
			}

			sprintf(szLog, "> [Step:%d] Pausing Step Resumed ", nStep);
			_LOG_PROC(szLog); printf("%s\n", szLog);

			WRITE_DIGITAL(VRCP_ResumeDO, 1, &nIOStatus);

			if(nMode != VRM_NO_VRCP)
			{
				if(m_RcpStep[nIdx].nStepFlow_Option == SFL_NORMAL_CYC)
				{
					RUN_FUNCTION(TIME_MONITOR, "RESUME");
				}

				if(m_nSimMode == SM_SIMUL)
				{
					RUN_FUNCTION(PM_SIMULATOR, "RESUME_VRCP");
				}
			}
		}
		else
		{
			// if Previous Step is Unlimited Cycle Step
			int nPrevIdx;
			nPrevIdx = nPrevStep - 1;
			// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop
			if(m_RcpStep[nPrevIdx].nStepFlow_Option != SFL_NORMAL_CYC
				&& m_RcpStep[nIdx].nStepFlow_Option == SFL_NORMAL_CYC)
			{
				_LOG_PROC("-----> [Step:%d] Timer Resumed", nStep);
				RUN_FUNCTION(TIME_MONITOR, "RESUME");
			}
		}
		return TRUE;
	}

	// Now Step is First Step
	// 0:Off 1:On => Resume when raise etch
	if(READ_DIGITAL(VRCP_ResumeDO, &nIOStatus) == 1)
	{
		if(! WAIT_SECONDS(0.1)) return FALSE;
		WRITE_DIGITAL(VRCP_ResumeDO, 0, &nIOStatus);
	}

	// Wait for Download Completed
	// 2013.08.19
	if(2 == m_nVRCPBufPageMax)
	{
		while(m_nDL_Thread != THREAD_STOP && m_nDL_Thread != THREAD_FAILED)
		{
			if(! WAIT_SECONDS(0.1)) break;
		}
	}
	else
	{
		while(FALSE == m_bFirstStepDownload)
		{
			if(! WAIT_SECONDS(0.1)) break;
		}
	}
	if(MANAGER_ABORT())
	{
		_LOG_PROC("-----> Manager Abort in Start_ProcessStep");
		return FALSE;
	}
	if(m_nDL_Thread == THREAD_FAILED) return FALSE;

	// 2014.10.13 MARS
	if(m_RcpStep[nIdx].nStepFlow_Option == SFL_NORMAL_CYC)
	{
		LogEvent("StepProcess", "Start", nStep, "", "Time", m_RcpStep[nIdx].szStepName);
	}
	else
	{
		LogEvent("StepProcess", "Start", nStep, "", "Condition", m_RcpStep[nIdx].szStepName);
	}

	// Start Valve Recipe
	// 0:Stop 1:Start
	WRITE_DIGITAL(VRCP_StaStpDO, 1, &nIOStatus);
	sprintf(szLog, "-----> [Step:%d] VRCP Start Bit Set", nStep);
	_LOG_PROC(szLog); printf("%s\n", szLog);

	if(m_nSimMode == SM_SIMUL)
	{
		RUN_FUNCTION(PM_SIMULATOR, "START_VRCP");
		sprintf(szLog, "-----> [Step:%d] PLC Simulator VRCP Started ", nStep);
		_LOG_PROC(szLog); printf("%s\n", szLog);
	}

	sprintf(szLog, "> [Step:%d] First Step Started ", nStep);
	_LOG_PROC(szLog); printf("%s\n", szLog);

	// Start Timer
	// 2006.06.14
	if(m_ProcQ[QSTEP_RUN].nCmd != PCMD_CONT && m_ProcQ[QSTEP_RUN].nCmd != PCMD_RESTART)
	{
		RUN_FUNCTION(TIME_MONITOR, "START");
	}
	else
	{
		RUN_FUNCTION(TIME_MONITOR, "RESUME");
	}

	_LOG_PROC("-----> [Step:%d] Timer Started", nStep);

	// Rough Valve Option => 0:NotCtrl, 1:Open, 2:Close
	if(m_RcpStep[nIdx].nRoughValve == 1) WRITE_DIGITAL(FastRoughVlvDO, 1, &nIOStatus);
	else if(m_RcpStep[nIdx].nRoughValve == 2) WRITE_DIGITAL(FastRoughVlvDO, 0, &nIOStatus);
	_LOG_PROC("-----> [Step:%d] Rough Valve Controled", nStep);

	// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop
	if(nMode == VRM_NO_VRCP)
	{
		RUN_FUNCTION(TIME_MONITOR, "PAUSE");
		sprintf(szLog, "> [Step:%d] Step is Paused", nStep);
		_LOG_PROC(szLog); printf("%s\n", szLog);
	}
	else if(m_RcpStep[nIdx].nStepFlow_Option != SFL_NORMAL_CYC)
	{
		_LOG_PROC("-----> [Step:%d] Timer Paused", nStep);
		RUN_FUNCTION(TIME_MONITOR, "PAUSE");
	}

	if(! bRet )
	{
		sprintf(szLog, "---> [Step:%d] Start_ProcessStep Failed", nStep);
		_LOG_PROC(szLog); printf("%s\n", szLog);
	}
	return bRet;
}

BOOL CProcessSequence::Set_Controller()
{
	BOOL bRet = FALSE;
	BOOL bRslt = TRUE;
	int nStep, nIdx;
	int nIOStatus;
	char szLog[200];
	char szTemp[100];
	char szParam[MAX_PARAM_SIZE];
	int nNextStep;

	nStep = m_ProcQ[QSTEP_RUN].nStep;
	if(nStep == 0) return TRUE;		// Step is not existed
	sprintf(szLog, "---> [Step:%d] Set_Controlller Called", nStep);
	_LOG_PROC(szLog); printf("%s\n", szLog);

	nIdx = nStep - 1;
	do {
		// Rough Valve Option => 0:NotCtrl, 1:Open, 2:Close
		if(m_RcpStep[nIdx].nRoughValve == 1) WRITE_DIGITAL(FastRoughVlvDO, 1, &nIOStatus);
		else if(m_RcpStep[nIdx].nRoughValve == 2) WRITE_DIGITAL(FastRoughVlvDO, 0, &nIOStatus);
		_LOG_PROC("-----> [Step:%d] Rough Valve Controled", nStep);

		// Set Throttle Valve
		StartTVSetting(nStep);
		_LOG_PROC("-----> [Step:%d] Throttle Valve Setting Started", nStep);

		// Set RF Control
		if(m_nRF_UseDM == 1)
		{
			if(m_nProRfSetCtrl == 1)	// 0:NotUse 1:Use
			{
				sprintf(szTemp, "SET_PW_VALUE %d", m_RcpStep[nIdx].nRF_FwSp);
				RUN_SET_FUNCTION(RF_CONTROL, szTemp); //2014.04.01
				_LOG_PROC("-----> [Step:%d] RF Power Set", nStep);
				//2008.09.29
				sprintf(szTemp, "SET_PW_VALUE %d", m_RcpStep[nIdx].nRF2_FwSp);
				RUN_SET_FUNCTION(RF_CONTROL2, szTemp); //2014.04.01
				_LOG_PROC("-----> [Step:%d] RF2 Power Set", nStep);
			}
		}

		// Set Controller as Controller Setting Option
		// 0:NotUse 1:Use
		if( 1 == m_nProMfcSetCtrl )
		{
			// MFC Setting
			StartMFCSetting(nStep, TRUE, TRUE);
			_LOG_PROC("-----> [Step:%d] MFC Setting Started", nStep);
		}
		else
		{
			StartMFCSetting(nStep, FALSE, TRUE);
			_LOG_PROC("-----> [Step:%d] MFC Monitoring Started", nStep);
		}

		// RF Preset Control
		if(m_nRF_UseDM == 1)
		{
			if(m_RcpStep[nIdx].nPrstMode_Option > 0
				||m_RcpStep[nIdx].nPrstSel_Option > 0
				||m_RcpStep[nIdx].nRFLdPrst_Option > 0 )	// 2014.05.16
			{
				sprintf(szTemp, "PRESET_POS LT %d %d %d",
					m_RcpStep[nIdx].nPrstMode_Option
					,m_RcpStep[nIdx].nPrstSel_Option
					,m_RcpStep[nIdx].nRFLdPrst_Option);
				RUN_SET_FUNCTION(RF_CONTROL, szTemp);  //2014.04.01
				_LOG_PROC("-----> [Step:%d] RF Preset", nStep);
				/*
				//2008.09.29
				sprintf(szTemp, "PRESET_POS L %d", (int) m_RcpStep[nIdx].dbRF2LdPrst_SetPoint);
				RUN_SET_FUNCTION(RF_CONTROL2, szTemp); //2014.04.01
				_LOG_PROC("-----> [Step:%d] RF2 Power Set", nStep);
				*/
			}

			if(m_RcpStep[nIdx].nPrst2Mode_Option > 0
				||m_RcpStep[nIdx].nPrst2Sel_Option > 0
				||m_RcpStep[nIdx].nRF2LdPrst_Option > 0)	// 2014.05.16
			{
				sprintf(szTemp, "PRESET_POS LT %d %d %d",
					m_RcpStep[nIdx].nPrst2Mode_Option
					,m_RcpStep[nIdx].nPrst2Sel_Option
					,m_RcpStep[nIdx].nRF2LdPrst_Option);
				RUN_SET_FUNCTION(RF_CONTROL2, szTemp);  //2014.04.01
				_LOG_PROC("-----> [Step:%d] RF2 Preset", nStep);
				/*
				//2008.09.29
				sprintf(szTemp, "PRESET_POS T %d", (int) m_RcpStep[nIdx].dbRF2TnPrst_SetPoint);
				RUN_SET_FUNCTION(RF_CONTROL2, szTemp);  //2014.04.01
				_LOG_PROC("-----> [Step:%d] RF2 Power Set", nStep);
				*/
			}
		}
		// Heat Exchange 2014.04.01
		if(m_nHTE_UseDM == 1)
		{
			if(m_RcpStep[nIdx].nHte_Option == 1 && m_nProHteSetCtrl == 1)
			{
				sprintf(szTemp, "HT_EXCHG_SETTEMP %d", (int) m_RcpStep[nIdx].dbHte_SetPoint);
				RUN_SET_FUNCTION(HT_ExchgControl, szTemp);
				_LOG_PROC("-----> [Step:%d] Heat Exchange Set", nStep);
			}
		}

		// Start Throttle Valve Monitoring
		StartTVMonitor(nStep);

		// Stage Heater Temperature


		if(m_RcpStep[nIdx].nStgHtr1_Option != 1) StartStgHtrSetting(nStep);
		if(m_RcpStep[nIdx].nStgHtr2_Option != 1) StartStgHtr2Setting(nStep);

		// Heater Temperature
		StartHeaterSetting(nStep);

		// Start Process Background Run Function
		// 2006.01.18
		sprintf(szParam, "START2 %6.4f %6.4f %d %d",
				m_RcpStep[nIdx].dblCHMPRS_Pres,
				m_RcpStep[nIdx].dblCHMPRS_PresL,
				m_RcpStep[nIdx].nCHMPRS_Option,
				m_RcpStep[nIdx].nCHMPRS_Timeout);
		if(m_RcpStep[nIdx].nBacksideArCheck == 1)
				strcat(szParam, " BACKSIDE_AR_CHK");
		else strcat(szParam, " NO");

		RUN_SET_FUNCTION(PROCBGRUN_CONTROL, szParam);
		_LOG_PROC("-----> [Step:%d] ProBgRun Function Started", nStep);

		// Start RF Monitoring
		if(m_nRF_UseDM == 1)
		{
			RUN_SET_FUNCTION(RF_CONTROL, "START_MON");
			_LOG_PROC("-----> [Step:%d] RF Monitoring Started", nStep);
			//2008.09.29
			RUN_SET_FUNCTION(RF_CONTROL2, "START_MON");
			_LOG_PROC("-----> [Step:%d] RF2 Monitoring Started", nStep);
		}
		if(! WAIT_SECONDS(0.1)) { bRslt = FALSE; break; }

		// 2014.10.13 MARS
		if(m_RcpStep[nIdx].bIsMain)
		{
			LogEvent("RF1PowerOnOff", "Start", 0, "", "", "");
			LogEvent("RF2PowerOnOff", "Start", 0, "", "", "");
		}
		//

		// Resume Channel Drop
		if(READ_DIGITAL(VRCP_ResumeDO, &nIOStatus) == 1)
		{
			WRITE_DIGITAL(VRCP_ResumeDO, 0, &nIOStatus);
		}

		// Step GoNextStep Channel Drop
		if(READ_DIGITAL(VRCP_GoNextDO, &nIOStatus) == 1)
		{
			WRITE_DIGITAL(VRCP_GoNextDO, 0, &nIOStatus);
		}

		if(bRslt == FALSE) break;

		// 2005.05.30 by cskim, for Keep Last Step
		nNextStep = m_ProcQ[QSTEP_RD1].nStep;
		if(m_RcpHeader.bKeepLastCon && nNextStep == 0)
		{
			_LOG_PROC("-----> [Step:%d] KeepLastCon Started", nStep);
			KeepLastCon(nStep);
		}
		bRet = TRUE;
	} while(0);

	if(! bRet )
	{
		sprintf(szLog, "---> [Step:%d] Set_Controlller Failed", nStep);
		_LOG_PROC(szLog); printf("%s\n", szLog);
	}
	return bRet;
}

BOOL CProcessSequence::IsProcStepFinished(BOOL *pbStatus)	// 2006.07.07
{
	BOOL bRet = FALSE;
	int nIOStatus;
	int nRunStep, nCurStep, nNextStep;
	char szBuf[16] = {0,};   //2014.08.25

	*pbStatus = TRUE;
	nCurStep = m_ProcQ[QSTEP_RUN].nStep;
	if(nCurStep == 0) return TRUE;

	//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)
	nRunStep = (int) READ_ANALOG(PLC_CurStepAI, &nIOStatus);

	if(0 == READ_DIGITAL(VRCP_RunningDI, &nIOStatus) || 1 == READ_DIGITAL(VRCP_PauseDI, &nIOStatus))
	{
		bRet = TRUE;
		_LOG_ERR("-----> Step Sync Change! [ChangedStep(PLC):%d, nCurStep:%d] [PauseDI:%d, RunDI:%d, GbInt:%d, DnErr1~5:%d,%d,%d,%d,%d]", nRunStep, nCurStep,
												READ_MEM_DIGITAL(VRCP_PauseDI, szBuf), READ_MEM_DIGITAL(VRCP_RunningDI, szBuf),
												READ_MEM_DIGITAL(PLC_GbIntlckDI, szBuf),
												READ_MEM_DIGITAL(VRCP_DnLdErr1DI, szBuf),
												READ_MEM_DIGITAL(VRCP_DnLdErr2DI, szBuf),
												READ_MEM_DIGITAL(VRCP_DnLdErr3DI, szBuf),
												READ_MEM_DIGITAL(VRCP_DnLdErr4DI, szBuf),
												READ_MEM_DIGITAL(VRCP_DnLdErr5DI, szBuf));
	}
	else
	{
		//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)
		//nRunStep = (int) READ_ANALOG(PLC_CurStepAI, &nIOStatus);
		if(nRunStep != nCurStep)
		{
			//2014.08.25
			_LOG_ERR("-----> Step Sync Change! [ChangedStep(PLC):%d, nCurStep:%d]", nRunStep, nCurStep);
			//
			nNextStep = m_ProcQ[QSTEP_RD1].nStep;
			if(nNextStep != 0 && nRunStep != nNextStep)
			{
				ALARM_POST(ALARM_DOWNSTEP_RUNSTEP_DIFF);
				//_LOG_ERR("-----> Step Sync Error ! [DownloadStep:%d, ChangedStep:%d]", nNextStep, nRunStep);
				_LOG_ERR("-----> Step Sync Error ! [DownloadStep:%d, ChangedStep(PLC):%d, nCurStep:%d]", nNextStep, nRunStep, nCurStep);
				*pbStatus = FALSE;
			}
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL CProcessSequence::IsProcStepStarted()
{
	enum { VSTS_STOP, VSTS_RUN };
	BOOL bRet = TRUE;
	int nIOStatus;
	int nCurStep, nRunStep = -1; //2014.08.25
	int nPrevStep;
	int nVrcpRunSts;
	int	nCnt = 0;   //2014.08.25

	nCurStep = m_ProcQ[QSTEP_RUN].nStep;
	//2014.08.25
	nRunStep = (int) READ_ANALOG(PLC_CurStepAI, &nIOStatus);
	_LOG_ERR("-----> Step Sync1! [ChangedStep(PLC):%d, nCurStep:%d]", nRunStep, nCurStep);
	//
	if(nCurStep == 0) return TRUE;

	nPrevStep = m_ProcQ[QSTEP_END].nStep;
	nVrcpRunSts = READ_DIGITAL(VRCP_RunningDI, &nIOStatus);
	//start 2014.08.25
	nCnt = 0;
	do {

		nRunStep = (int) READ_ANALOG(PLC_CurStepAI, &nIOStatus);
		if (nCurStep == nRunStep) {
			_LOG_ERR("-----> Step Sync2! [ChangedStep(PLC):%d, nCurStep:%d, nCnt:%d]", nRunStep, nCurStep, nCnt);
			break;
		}
		nCnt++;
		Sleep(10);

		if ( nCnt > 100 ) break;
	}while(1);
	//end 2014.08.25

	if(nPrevStep == 0)		// 2006.07.07
	{
		//nRunStep = (int) READ_ANALOG(PLC_CurStepAI, &nIOStatus);  2014.08.25
		if(nCurStep != nRunStep) bRet = FALSE;
		if(VSTS_STOP == nVrcpRunSts) bRet = FALSE;
	}
	else if(VSTS_STOP == nVrcpRunSts)
	{
		ALARM_POST(ALARM_PLC_IS_ABNORMAL_STOPED);		// 2006.07.07
		bRet = FALSE;
	}
	return bRet;
}

BOOL CProcessSequence::Wait_ProcStepStarted()
{
	BOOL bRet = FALSE;
	int nRunStep, nRunMode, nIdx;
	char szLog[200];

	nRunStep = m_ProcQ[QSTEP_RUN].nStep;
	if(nRunStep == 0) return TRUE;
	nIdx = nRunStep - 1;
	nRunMode = m_RcpStep[nIdx].nVRCPMode;
	if(nRunMode == VRM_NO_VRCP) return TRUE;

	// Wait to Start Process Step
	sprintf(szLog, "---> [Step:%d] Waiting for Starting ValveRecipe ...", nRunStep);
	_LOG_PROC(szLog); printf("%s\n", szLog);

	do {
		int nTimeout = 0;
		while(IsProcStepStarted() == FALSE)
		{
			if(! WAIT_SECONDS(0.03)) break;
			nTimeout++;
			if(nTimeout >= 100) break;
		}
		if(MANAGER_ABORT())
		{
			_LOG_PROC("-----> Manager Abort in Wait_ProcStepStarted");
			break;
		}

		BOOL bStartErr = FALSE;
		if(nTimeout >= 100)
		{
			ALARM_POST(ALARM_VALVE_RECIPE_START_TIMEOUT);
			sprintf(szLog, "-----> [Step:%d] Valve Recipe Start Timeout ", nRunStep);
			_LOG_ERR_P(szLog)
			break;
		}

		// Valve Recipe Started
		_LOG_PROC("-----> [Step:%d] Valve Recipe Started", nRunStep);

		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CProcessSequence::Wait_ProcStepFinished()
{
	BOOL bRet = FALSE;
	BOOL bRslt = TRUE;

	int nNextStep, nNextIdx;
	int nRunStep, nRunMode, nIdx;
	char szLog[200];
	double dbStepTimeTol;	// 2006.07.20 by cskim;

	nRunStep = m_ProcQ[QSTEP_RUN].nStep;
	nNextStep = m_ProcQ[QSTEP_RD1].nStep;
	if(nRunStep == 0) return TRUE;
	nIdx = nRunStep - 1;
	nRunMode = m_RcpStep[nIdx].nVRCPMode;
	if(nRunMode == VRM_NO_VRCP) return TRUE;

	// Check Now Step Finished
	sprintf(szLog, "---> [Step:%d] Waiting for Finishing Step ...", nRunStep);
	_LOG_PROC(szLog); printf("%s\n", szLog);

	do {
		int nCount = 0;
		while( IsProcStepFinished(&bRslt) == FALSE )
		{
			if(! bRslt) break;
			if(! WAIT_SECONDS(0.03)) break;
			if( (nCount % 30) == 0)
			{
				if(CheckMonitoringStatus() == FALSE)
				{
					sprintf(szLog, "-----> [Step:%d] Monitoring Status failed", nRunStep);
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}

				if(m_nDL_Thread == THREAD_FAILED) { bRslt = FALSE; break; }
			}
			nCount++;
			if(nCount >= 100000) nCount = 0;
		}
		if(MANAGER_ABORT())
		{
			_LOG_PROC("-----> Manager Abort in Wait_ProcStepFinished");
			break;
		}

		// 2005.04.18 by cskim
		m_pStepTimer->StopTimer();
		UpdateStepTime();
		if(bRslt == FALSE) break;

		if(m_bProcTimeInterlock)
		{
			// 2005.07.22, for Step Time Interlock
			double dbStepTimeDiff = 0;
			double dbStepTotTime = GetProcStepTime(nRunStep);
			double dbStepCurTime = m_pStepTimer->GetProgressTime();

			// 2006.02.02
			if(dbStepTotTime > 0)
			{
				dbStepCurTime /= 1000;
				dbStepTimeDiff = fabs(dbStepTotTime - dbStepCurTime);
				if(m_dbStepTimeTol > 0)
				{
					// 2006.07.20 by cskim
					dbStepTimeTol = m_dbStepTimeTol+(dbStepTotTime*m_dbStepTimeTolRate/100);
					if(dbStepTimeDiff > dbStepTimeTol)  //... 2017.03.07 Bug Fix for Tolerance alarm
					{
						ALARM_POST(ALARM_PROGRESS_STEP_TIME_IS_INVALID);
						_LOG_ERR("-----> Step Time Is Invalid(Alarm Raised) !(Current:%.2f <-> Target:%.2f)",
									dbStepCurTime, dbStepTotTime);
						break;
					}
				}
				else
				{
					// 2006.07.20 by cskim
					dbStepTimeTol = 3+(dbStepTotTime*0.1/100);
					if(dbStepTimeDiff > dbStepTimeTol)
					{
						_LOG_ERR("-----> Step Time Is Invalid(Warning Log) !(Current:%.2f <-> Target:%.2f)",
									dbStepCurTime, dbStepTotTime);
					}
				}
			}
		}

		if(CheckMonitoringStatus() == FALSE)
		{
			sprintf(szLog, "-----> [Step:%d] Monitoring Status failed", nRunStep);
			_LOG_PROC(szLog); printf("%s\n", szLog);
			break;
		}
		if(m_nDL_Thread == THREAD_FAILED) break;

		// Step Exchange is checked
		sprintf(szLog, "> [Step:%d -> Step:%d] Step Exchage is Checked ", nRunStep, nNextStep);
		_LOG_PROC(szLog); printf("%s\n", szLog);

		int nIOStatus;
		if(nRunStep !=0 && nNextStep != 0)
		{
			nNextIdx = nNextStep - 1;
			// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop
			if(1 == READ_DIGITAL(VRCP_PauseDI, &nIOStatus))
			{
				RUN_FUNCTION(TIME_MONITOR, "PAUSE");
				sprintf(szLog, "> [Step:%d] Step is Paused", nRunStep);
				_LOG_PROC(szLog); printf("%s\n", szLog);
			}
			else if(m_RcpStep[nNextIdx].nStepFlow_Option != SFL_NORMAL_CYC)
			{
				RUN_FUNCTION(TIME_MONITOR, "PAUSE");
			}

			// VRCP_DnLdErrorDI => 0:Normal 1:Error
			if( READ_DIGITAL(VRCP_DnLdErr1DI, &nIOStatus) == 1 ||
				READ_DIGITAL(VRCP_DnLdErr2DI, &nIOStatus) == 1 ||
				READ_DIGITAL(VRCP_DnLdErr3DI, &nIOStatus) == 1 ||
				READ_DIGITAL(VRCP_DnLdErr4DI, &nIOStatus) == 1 ||
				READ_DIGITAL(VRCP_DnLdErr5DI, &nIOStatus) == 1 )
			{
				if(! WAIT_SECONDS(0.1)) break;
				sprintf(szLog, "-----> [Step:%d] SW and PLC Sync Failed (NextStep is not downloaded to PLC)", nNextStep);
				_LOG_PROC(szLog); printf("%s\n", szLog);

				sprintf(szLog, "-----> NextStep is not downloaded to PLC in request time [DownStep:%d(%d), RunStep:%d(%d)] ",
						nNextStep, m_nDnLdStepCount, nRunStep, m_nRunStepCount);
				_LOG_ERR(szLog);

				if(m_nVRCP_DnLdWait == 0)
				{
					ALARM_POST(ALARM_SW_AND_PLC_SYNC_FAILED);
					break;
				}
				else
				{
					sprintf(szLog, "-----> [Step:%d] Download Waiting Option is On", nNextStep);
					_LOG_PROC(szLog); printf("%s\n", szLog);

					// Wait for Download Completed
					if(2 == m_nVRCPBufPageMax)		// 2013.08.27
					{
						while(m_nDL_Thread != THREAD_STOP && m_nDL_Thread != THREAD_FAILED) WAIT_SECONDS(0.1);
						if(m_nDL_Thread == THREAD_FAILED) break;
					}
				}
			}
		}

		bRet = TRUE;
	} while(0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL CProcessSequence::Wait_ConditionIsOK()
{
	enum { PRO_ABORT, PRO_RETRY };

	BOOL bRet = FALSE;
	int nRunStep, nRunMode, nIdx;
	char szLog[200];

	//... 2015.03.07 Add RPG ON Step Skip
	unsigned char *pData;
	double	dbRPGOnTimeout = 0;

	int		nRPGOnOKCheckCnt	= 0;

	nRunStep = m_ProcQ[QSTEP_RUN].nStep;
	if(nRunStep == 0) return TRUE;
	nIdx = nRunStep - 1;
	nRunMode = m_RcpStep[nIdx].nVRCPMode;

	if(m_RcpStep[nIdx].nStepFlow_Option == SFL_NORMAL_CYC)
	{
		if(nRunMode != VRM_NO_VRCP) return TRUE;
	}

	sprintf(szLog, "---> [Step:%d] Waiting until Condition Is OK ...", nRunStep);
	_LOG_PROC(szLog); printf("%s\n", szLog);

	do {
		BOOL bRslt = TRUE;
		int nIOStatus;
		int nPreHeatTime = m_RcpStep[nIdx].nPreHeatTime;

		if(nPreHeatTime > 0)
		{
			_LOG_PROC("-----> Wait PreHeatTime ...");
			if(! WAIT_SECONDS(nPreHeatTime)) break;
		}

		// StepFlow => 0:Normal 1:StageHeaterTempMonitor 2:CheckIdleStop 3:HtrTempMonitor
		else if(m_RcpStep[nIdx].nStepFlow_Option == SFL_WAIT_STGTEMP)
		{
			int nMaxTimeout = 0;

			nMaxTimeout = (int)m_RcpStep[nIdx].dbStgHtr_Timeout;

			//_LOG_PROC("-----> Stage Heater Temp Waiting(%d min) ...", (int) m_RcpStep[nIdx].dbStgHtr1_Timeout);
			_LOG_PROC("-----> Stage Heater Temp Waiting(%d min) ...", nMaxTimeout);

			int nTimeoutSec = 0;
			// Stage Heater Temp Set Monitoring
			do {
				if(! WAIT_SECONDS(3)) break;
				if(CheckMonitoringStatus() == FALSE)
				{
					sprintf(szLog, "-----> [Step:%d] Monitoring Status failed", nRunStep);
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}

				nTimeoutSec += 3;
				//if(nTimeoutSec/60 >= (int) m_RcpStep[nIdx].dbStgHtr_Timeout)
				if(nTimeoutSec/60 >= nMaxTimeout)
				{
					ALARM_POST(ALARM_STGHTR_CONTROL_TIMEOUT);
					sprintf(szLog, "-----> Stage Heater Control Timeout Error");
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}
			} while(! IsStgHtrCondiOK(nRunStep));
			if(bRslt == FALSE) break;
			if(MANAGER_ABORT()) break;
		}
		// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop
		else if(m_RcpStep[nIdx].nStepFlow_Option == SFL_WAIT_IDLESTOP)
		{
			_LOG_PROC("-----> Idle Stop Signal Waiting");
			m_bIdleStopEnable = TRUE;
			while(WAIT_SECONDS(3) && IsIdleStop() == FALSE)
			{
				if(CheckMonitoringStatus() == FALSE)
				{
					sprintf(szLog, "-----> [Step:%d] Monitoring Status failed", nRunStep);
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}
			}
			if(bRslt == FALSE) break;
			if(MANAGER_ABORT()) break;
		}
		// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop 3:CheckHeaterTemp
		else if(m_RcpStep[nIdx].nStepFlow_Option == SFL_WAIT_HTRTEMP)
		{
			int nMaxTimeout = 0;
			nMaxTimeout = (int)m_RcpStep[nIdx].dbStgHtr_Timeout;

			//_LOG_PROC("-----> Heater Temp Waiting(%d min) ...", (int) m_RcpStep[nIdx].dbStgHtr_Timeout);
			_LOG_PROC("-----> Heater Temp Waiting(%d min) ...", nMaxTimeout);

			int nTimeoutSec = 0;
			// Stage Heater Temp Set Monitoring
			do {
				if(! WAIT_SECONDS(3)) break;
				if(CheckMonitoringStatus() == FALSE)
				{
					sprintf(szLog, "-----> [Step:%d] Monitoring Status failed", nRunStep);
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}

				nTimeoutSec += 3;
				//if(nTimeoutSec/60 >= (int) m_RcpStep[nIdx].dbStgHtr_Timeout)
				if(nTimeoutSec/60 >= nMaxTimeout)
				{
					ALARM_POST(ALARM_HEATER_CONTROL_TIMEOUT);
					sprintf(szLog, "-----> Heater Control Timeout Error");
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}
			} while(! IsStgHtrCondiOK(nRunStep) || ! IsHeaterCondiOK(nRunStep));
			RUN_FUNCTION(HTR_CONTROL, "STOP");
			RUN_FUNCTION(HT_ExchgControl, "STOP"); //2014.04.01
			if(bRslt == FALSE) break;
			if(MANAGER_ABORT()) break;
		}
		//--------------------------------------------------------------------
		// 2005.10.19, Process Step Sync between Modules
		// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop 3:CheckHeaterTemp 4:CheckProcStepSync
		else if(m_RcpStep[nIdx].nStepFlow_Option == SFL_WAIT_STEPSYNC)
		{
			_LOG_PROC("-----> PM Proc Step Sync Waiting ...");
			do {
				if(! WAIT_SECONDS(3)) break;
			} while(m_pDynIO->dREAD_DIGITAL(PRO_PMStepSyncDM, &nIOStatus) == 1);
			if(MANAGER_ABORT()) break;

			m_pDynIO->dWRITE_DIGITAL(PRO_PMStepWaitDM, 0, &nIOStatus);
			_LOG_PROC("-----> [Step:%d] PRO_PMStepWaitDM channel set to 0", nRunStep);
		}
		// 2014.05.22
		else if(m_RcpStep[nIdx].nStepFlow_Option == SFL_FUNCTION)
		{
			_LOG_PROC("-----> PM Sub Function Waiting ...");
			Module_Status msRslt = SYS_SUCCESS;
			do {
				if(! WAIT_SECONDS(3)) break;
				msRslt = GetSubFuncStatus(nRunStep);
			} while(SYS_RUNNING == msRslt);

			if(SYS_SUCCESS != msRslt) break;
			StopSubFunction(nRunStep);
			if(MANAGER_ABORT()) break;
		}
		//////////////////////////////////////////////////////////////////////////
		//... 2015.03.07 Add RPG ON Step Skip
		else if(m_RcpStep[nIdx].nStepFlow_Option == SFL_WAIT_RPG_ON)
		{
			//... Calculate Step Time
			pData = m_RcpStep[nIdx].ppVR_Step[0]+m_nLINE_STEP_BASE;
			if(m_nValveTimeRes == VTS_10MS)		dbRPGOnTimeout = ((double) (pData[0]*256 + pData[1])) /100;
			else								dbRPGOnTimeout = ((double) (pData[0]*256 + pData[1])) /10;	 // if m_nValveTimeRes == VTS_100ms

			_LOG_PROC("-----> PM RPG Ignition Waiting (%.2f) ...", dbRPGOnTimeout);

			do {
				//... Default Wait
				if(! WAIT_SECONDS(1))					break;

				//... Check Interlock Status
				if(CheckMonitoringStatus() == FALSE)
				{
					sprintf(szLog, "-----> [Step:%d] Monitoring Status failed", nRunStep);
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}

				//... Check RPG On Ignition Completed
				if(EO_ON == READ_DIGITAL(RPGPlasmaDetectDI, &nIOStatus))
				{
					nRPGOnOKCheckCnt++;
					_LOG_PROC("[INFO] [Step:%d] PM RPG Ignition [%d]Checked", nRunStep, nRPGOnOKCheckCnt);

					if(nRPGOnOKCheckCnt >= 2)	break;		//... Fixed Next Step Go Count
				}
				else	nRPGOnOKCheckCnt = 0;	// Reset RPG Ignition OK Check Count

				//... Check Timeout for Alarm
				if(dbRPGOnTimeout <= READ_ANALOG(PLC_RCPRunTMAI, &nIOStatus))
				{
					WRITE_DIGITAL(RPGAlmPrcRetryDM, eRPGALM_IGNITION, &nIOStatus);			//... Case. Ignition Failed
					m_pDynIO->dWRITE_DIGITAL(PRO_FlowControlDM, PRO_RETRY, &nIOStatus);

					sprintf(szLog, "[ERR] [Step:%d] RPG Ignition Wait Timeout(%.2f)", nRunStep, dbRPGOnTimeout);
					_LOG_PROC(szLog); printf("%s\n", szLog);
					bRslt = FALSE;
					break;
				}
			} while(1);

			if(bRslt == FALSE)		break;
			if(MANAGER_ABORT())		break;
		}
		//--------------------------------------------------------------------
		else
		{
			char szAlarm[256];
			char szExtAlarm[256];
			ALARM_MESSAGE_GET(ALARM_INVALID_RECIPE_DATA, szAlarm);
			sprintf(szExtAlarm, "%s(Step[%d]:StepFlow Value Is Invalid => %d)",
						szAlarm, nRunStep, m_RcpStep[nIdx].nStepFlow_Option);
			ALARM_MESSAGE_SET(ALARM_INVALID_RECIPE_DATA, szExtAlarm);
			ALARM_POST(ALARM_INVALID_RECIPE_DATA);
			_LOG_ERR("-----> Valve Recipe is Nothing and Waiting Condition Is Nothing !");
			if(! WAIT_SECONDS(3)) break;
		}

		// Go Next Step
		if(m_RcpStep[nIdx].nStepFlow_Option != SFL_NORMAL_CYC)
		{
			WRITE_DIGITAL(VRCP_GoNextDO, 1, &nIOStatus);
			if(m_nSimMode == SM_SIMUL)
			{
				RUN_FUNCTION(PM_SIMULATOR, "GO_NEXT_STEP");
				sprintf(szLog, "-----> [Step:%d] PLC Simulator VRCP Goto NextStep ", nRunStep);
				_LOG_PROC(szLog); printf("%s\n", szLog);
			}
		}

		bRet = TRUE;

	} while(0);

	return bRet;
}

BOOL CProcessSequence::IsStgHtrCondiOK(int nStep)
{
	BOOL bRet = TRUE;
	//BOOL bRet = FALSE;
	char szTemp[40];

	int nIdx = 0;
	nIdx = nStep - 1;


	do
	{
		memset(szTemp, 0, sizeof(szTemp));

		// 0:No 1:Check 2:SetTemp 3:Set_If_Upper 4:Set_If_Lower
		switch(m_RcpStep[nIdx].nStgHtr1_Option)
		{
		case 0:
		case 1:	break;
		case 2:
			sprintf(szTemp, "CHECK_TEMP STABLE");
			break;
		case 3:
			sprintf(szTemp, "CHECK_LOWER");
			break;
		case 4:
			sprintf(szTemp, "CHECK_UPPER");
			break;
		}
		if(m_RcpStep[nIdx].nStgHtr1_Option > 1)
		{
			if(SYS_RUNNING != READ_FUNCTION(STGHTR_CONTROL))
			{
				if(SYS_SUCCESS != RUN_FUNCTION(STGHTR_CONTROL, szTemp))
				{
					bRet = FALSE;
					break;
				}

			}
			else
			{
				bRet = FALSE;
				break;
			}
		}

		memset(szTemp, 0, sizeof(szTemp));

		// 0:No 1:Check 2:SetTemp 3:Set_If_Upper 4:Set_If_Lower
		switch(m_RcpStep[nIdx].nStgHtr2_Option)
		{
		case 0:
		case 1:	break;
		case 2:
			sprintf(szTemp, "CHECK_TEMP STABLE");
			break;
		case 3:
			sprintf(szTemp, "CHECK_LOWER");
			break;
		case 4:
			sprintf(szTemp, "CHECK_UPPER");
			break;
		}
		if(m_RcpStep[nIdx].nStgHtr2_Option > 1)
		{
			if(SYS_RUNNING != READ_FUNCTION(STGHTR_CONTROL2))
			{
				if(SYS_SUCCESS != RUN_FUNCTION(STGHTR_CONTROL2, szTemp))
				{
					bRet = FALSE;
					break;
				}

			}
			else
			{
				bRet = FALSE;
				break;
			}
		}
	}while(0);
	return bRet;
}

BOOL CProcessSequence::IsStgHtrTempOK()
{
	BOOL bRet = TRUE;
	char szTemp[40];
	sprintf(szTemp, "CHECK_TEMP STABLE");
	if(SYS_SUCCESS != RUN_FUNCTION(STGHTR_CONTROL, szTemp)) bRet = FALSE;
	return bRet;
}

BOOL CProcessSequence::IsHeaterCondiOK(int nStep)
{
	BOOL bRet = FALSE;
	do {
		if(SYS_RUNNING == (Module_Status) READ_FUNCTION(HTR_CONTROL)) break;
		if(SYS_SUCCESS != RUN_FUNCTION(HTR_CONTROL, "CHECK_TEMP")) break;
		// 2014.04.01
		if(m_nHTE_UseDM == 1)
		{
			if(SYS_RUNNING == (Module_Status) READ_FUNCTION(HT_ExchgControl))break;
			if(SYS_SUCCESS != (Module_Status) RUN_FUNCTION(HT_ExchgControl, "CHECK_TEMP")) break;
		}
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CProcessSequence::IsIdleStop()
{
	BOOL bRet = FALSE;
	int nIOStatus;
	// 0:Stop 1:Running
	if(READ_DIGITAL(PRO_IdleRcpStsDM, &nIOStatus) == 0) bRet = TRUE;
	return bRet;
}

void CProcessSequence::Set_PrvMaintItem()
{
	int nStep, nIdx;
	int nIOStatus;
	nStep = m_ProcQ[QSTEP_RUN].nStep;
	if(nStep <= 0) return;

	nIdx = nStep - 1;
	if(m_RcpStep[nIdx].bValveCycleAccOption == 1)
		AccPrvMaintItem(nStep, m_RcpStep[nIdx].nVR_CycleCount);
	//------------------------------------------------------------
	// 2007.01.30 by cskim
	//----------------------------------------------------
	// 2004.12.08 by cskim, for Preventive
	WRITE_ANALOG(PRO_PrvVlvCycAM, m_dbVlvCycSum, &nIOStatus);
	WRITE_ANALOG(PRO_PrvSrcFdAM, m_dbSrcFeedingSum, &nIOStatus);
	//-----------------------------------------------------------
	// 2006.02.23
	m_pDynIO->dWRITE_ANALOG(PRO_PrvThickAM, m_RcpHeader.dbThickness, &nIOStatus);
	//-----------------------------------------------------------
	//------------------------------------------------------------
}

Module_Status CProcessSequence::Queue_ProcStep(int nStep, int nCmd)
{
	Module_Status msRet = SYS_ABORTED;
	int nDownStep;
	int nRunStep;

	nRunStep = m_ProcQ[QSTEP_RUN].nStep;
	nDownStep = m_ProcQ[m_nQStepDownload].nStep;
	_LOG_PROC("> [Step:%d] Queue_ProcStep Called", nStep, nCmd);

	do {
		Set_PrvMaintItem();
		Push_ProcStep(nStep, nCmd);
		msRet = Exec_ProcStep();
	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_LOG_PROC("---> [Step:%d] Queue_ProcStep Failed", nStep);
	}
	return msRet;
}

void CProcessSequence::Push_ProcStep(int nStep, int nCmd)
{
	// 2013.06.25
	int i;

	EnterCriticalSection(&m_csQueueLock);
	for(i=0; i<MAX_QUEUE-1; i++)
		memcpy((void *) &m_ProcQ[MAX_QUEUE-1-i], (void *) &m_ProcQ[MAX_QUEUE-2-i], sizeof(PROC_QUEUE));
	m_ProcQ[QSTEP_NEW].nStep = nStep;
	m_ProcQ[QSTEP_NEW].nCmd = nCmd;
	m_ProcQ[QSTEP_NEW].bDownload = FALSE;	// 2013.06.25
	LeaveCriticalSection(&m_csQueueLock);

	// 2013.08.19
	_LOG_PROC("-----> Push:%d, %d %d %d %d %d %d %d %d",
		nStep, m_ProcQ[0].nStep,m_ProcQ[1].nStep,m_ProcQ[2].nStep,m_ProcQ[3].nStep,
		m_ProcQ[4].nStep,m_ProcQ[5].nStep,m_ProcQ[6].nStep,m_ProcQ[7].nStep);
}

void CProcessSequence::Queue_BackShift()
{
	// 2013.06.25
	int i;

	EnterCriticalSection(&m_csQueueLock);
	for(i=0; i<MAX_QUEUE-1; i++)
		memcpy((void *) &m_ProcQ[i], (void *) &m_ProcQ[i+1], sizeof(PROC_QUEUE));
	m_ProcQ[QSTEP_END].nStep = 0;
	LeaveCriticalSection(&m_csQueueLock);
}

// 2013.06.25
BOOL CProcessSequence::IsQueueEmpty()
{
	BOOL bRet = TRUE;
	int i;
	for(i=0; i<MAX_QUEUE; i++)
	{
		if(m_ProcQ[i].nStep != 0) bRet = FALSE;
	}
	return bRet;
}

Module_Status CProcessSequence::Exec_ProcStep()
{
	Module_Status msRet = SYS_ABORTED;
	int nDownStep;
	int nRunStep;

	do {
		nRunStep = m_ProcQ[QSTEP_RUN].nStep;
		nDownStep = m_ProcQ[m_nQStepDownload].nStep;

		_LOG_PROC("-----> PreDownload:%d, Download:%d, Run:%d, End:%d",
			m_ProcQ[m_nQStepPreDownload].nStep, m_ProcQ[m_nQStepDownload].nStep, m_ProcQ[QSTEP_RUN].nStep, m_ProcQ[QSTEP_END].nStep);

		if(Preset_Controller() == FALSE )		break;
		if(Start_ProcessStep() == FALSE )		break;
		if(Wait_ProcStepStarted() == FALSE)		break;
		if(2 == m_nVRCPBufPageMax)							// 2013.06.25
		{
			if(Start_DownloadValveRecipe() == FALSE ) break;
		}
		if(Set_Controller() == FALSE)			break;
		if(RunSubFunction(nRunStep) == FALSE)	break;       // 2014.05.22
		_LOG_PROC("-----> DnLdReadyStepCount:%d DnLdStepCount:%d RunStepCount:%d",
			m_nDnRdyStepCount, m_nDnLdStepCount, m_nRunStepCount);
		if(Wait_ConditionIsOK() == FALSE)		break;
		if(Wait_ProcStepFinished() == FALSE)	break;

		CheckRFPostCount(nRunStep);			//... 2014.09.29 wjkim, RF Count Monitor
		GetALDVlvOnCount(nRunStep);

		// 2014.10.13 MARS
		if(nRunStep != 0)
		{
			if(m_RcpStep[nRunStep-1].bIsMain)
			{
				LogEvent("RF1PowerOnOff", "End", 0, "", "", "");
				LogEvent("RF2PowerOnOff", "End", 0, "", "", "");
			}

			if(m_RcpStep[nRunStep-1].nStepFlow_Option == SFL_NORMAL_CYC)
			{
				LogEvent("StepProcess", "End", nRunStep, "", "Time", m_RcpStep[nRunStep-1].szStepName);
			}
			else
			{
				LogEvent("StepProcess", "End", nRunStep, "", "Condition", m_RcpStep[nRunStep-1].szStepName);
			}
		}
		//...
		msRet = SYS_SUCCESS;
	} while(0);

	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Run Process Sub Functions

// 2006.06.14
void CProcessSequence::InitStepGroupRange()
{
	if(! m_RcpHeader.bUseStepGrp) return;
	int nBeginIdx, nEndIdx;
	nBeginIdx = m_RcpHeader.nStepGrpBegin - 1;
	nEndIdx = m_RcpHeader.nStepGrpEnd - 1;

	m_RcpStep[nBeginIdx].nLoopEnd = m_RcpHeader.nStepGrpEnd;
	m_RcpStep[nEndIdx].nLoopCount = 0;
	m_RcpStep[nEndIdx].nTotalLoopCycle = m_RcpHeader.nStepGrpCyc;
	m_RcpStep[nEndIdx].nLoopStart = m_RcpHeader.nStepGrpBegin;

	m_nStpGrpCCyc = 0;
}

BOOL CProcessSequence::PauseProcess()
{
	BOOL bRet = TRUE;
	m_pStepTimer->PauseTimer();
	AbortAllFunc();
	StopProcess();
	BackupCurrentProcess();
	if(SYS_SUCCESS != RUN_FUNCTION(VACUUM_CONTROL, "FAST_ROUGH")) bRet = FALSE;		// 2006.08.03

	//... 2014.10.20 Modify Chuck Home Skip for Discharge
//	if(SYS_SUCCESS != RUN_FUNCTION(CHUCK_CONTROL, "HOME 2")) bRet = FALSE;

	if(bRet == FALSE) _LOG_ERR("--------> PauseProcess Failed!");
	return bRet;
}

BOOL CProcessSequence::ResumeProcess(int nAlarmRecovery)
{
	BOOL bRet = TRUE;
	if(SYS_SUCCESS != RUN_FUNCTION(VACUUM_CONTROL, "FAST_ROUGH")) bRet = FALSE;

	m_ProcQ[QSTEP_RUN].nStep = 0;
	if(bRet == FALSE) _LOG_ERR("--------> ResumeProcess Failed!");
	return bRet;
}

void CProcessSequence::BackupCurrentProcess()
{
	BOOL bIOStatus;
	double dbStepTotalTime;
	int nIdx;

	m_Backup_dbProgressTime = m_pStepTimer->GetProgressTime();
	m_Backup_dbProgressTime /= 1000;
	m_Backup_nStepNum = m_ProcQ[QSTEP_RUN].nStep;
	m_Backup_nCurCycle = (int) READ_ANALOG(PLC_CurCycAI, &bIOStatus);

	if(m_Backup_nStepNum < 1) return;
	nIdx = m_Backup_nStepNum - 1;

	memcpy(&m_Backup_RcpStep, &m_RcpStep[nIdx], sizeof(RECIPE_STEP_STRUC));
	dbStepTotalTime = READ_ANALOG(PRO_StpTotTmAM, &bIOStatus);
	m_Backup_dbRemainTime = dbStepTotalTime - m_Backup_dbProgressTime;
	if(m_Backup_dbRemainTime <= 0) m_Backup_dbRemainTime = 3;
	else m_Backup_dbRemainTime += 3;
	if(m_Backup_dbRemainTime > dbStepTotalTime) m_Backup_dbRemainTime = dbStepTotalTime;
}

void CProcessSequence::TrimProcessTime()
{
	int nIdx;
	int nStepCycle;
	int nTotalLineStep;

	memcpy(&m_Remake_RcpStep, &m_Backup_RcpStep, sizeof(RECIPE_STEP_STRUC));
	nStepCycle = m_Backup_RcpStep.ppVR_Step[0][8]*256 + m_Backup_RcpStep.ppVR_Step[0][9];
	nTotalLineStep = m_Backup_RcpStep.ppVR_Step[0][11];

	if(nTotalLineStep == 1 && m_Remake_RcpStep.nVR_CycleCount == 1)
	{
		// Valve Time
		if(m_nValveTimeRes == VTS_10MS)
		{
			m_Remake_RcpStep.ppVR_Step[0][m_nLINE_STEP_BASE] =  (int) (m_Backup_dbRemainTime * 100 / 256);
			m_Remake_RcpStep.ppVR_Step[0][m_nLINE_STEP_BASE+1] = (int) (m_Backup_dbRemainTime * 100) % 256;
		}
		else	// if m_nValveTimeRes == VTS_100ms
		{
			m_Remake_RcpStep.ppVR_Step[0][m_nLINE_STEP_BASE] =  (int) (m_Backup_dbRemainTime * 10 / 256);
			m_Remake_RcpStep.ppVR_Step[0][m_nLINE_STEP_BASE+1] = (int) (m_Backup_dbRemainTime * 10) % 256;
		}
	}
	else
	{
		m_Remake_RcpStep.nVR_CycleCount = m_Backup_RcpStep.nVR_CycleCount - m_Backup_nCurCycle +1;
		if(m_Remake_RcpStep.nVR_CycleCount < 1) m_Remake_RcpStep.nVR_CycleCount = 1;
	}

	nIdx = m_Backup_nStepNum - 1;
	memcpy(&m_RcpStep[nIdx], &m_Remake_RcpStep, sizeof(RECIPE_STEP_STRUC));
}

void CProcessSequence::RecoverProcessTime()
{
	int nIdx;
	nIdx = m_Backup_nStepNum - 1;
	memcpy(&m_RcpStep[nIdx], &m_Backup_RcpStep, sizeof(RECIPE_STEP_STRUC));
}

void CProcessSequence::IncreaseGrpCyc()
{
	int nIOStatus;
	m_nStpGrpCCyc++;
	WRITE_ANALOG(PRO_StpGrpCCycAM, m_nStpGrpCCyc, &nIOStatus);
}

//////////////////////////////////////////////////////////////////////////
//... 2015.02.26 RPG Alarm Auto Retry (Only Lot Pre & Dry Clean)
//... Init RPG Retry Virtual IO
void CProcessSequence::InitRPGAlarmAutoRetry()
{
	int		nIOStatus;

	m_nRPGPrcRetryCnt = 0;

	if(RT_LotPreRcp == m_RcpHeader.nRecipeType || RT_CleanRcp == m_RcpHeader.nRecipeType)
	{
		WRITE_ANALOG(RPGAlmPrcRetryAM,	m_nRPGPrcRetryCnt,	&nIOStatus);
		WRITE_DIGITAL(RPGAlmPrcRetryDM, eRPGALM_NONE,		&nIOStatus);
	}
}

//... Check RPG Retry Count & Option
int CProcessSequence::CheckRPGAlarmAutoRetry()
{
	enum { ALMOPT_RETRY_1, ALMOPT_RETRY_2, ALMOPT_RETRY_3, ALMOPT_RETRY_4, ALMOPT_SPARE1, ALMOPT_SPARE2, ALMOPT_SPARE3, ALMOPT_ABORT };
	int		nRPGAlarmRecovery = -1;		//... Fixed Init Recovery Value

	int		nIOStatus;
	int		nRPGAlmPrcRetryFlag = eRPGALM_NONE;

	int		nRPGPrcRetryCntLmt	= 3;

	enum { eRPGALM_RETRY_PAUSE, eRPGALM_RETRY_CURRENT_STEP, eRPGALM_RETRY_GO_FIRST_STEP};
	int		nRPGAlmPrcRetryOpt	= eRPGALM_RETRY_PAUSE;

	BOOL	bDetailAlmPost		= FALSE;
	char	szAlarm[256]		= "";
	char	szExtAlarm[256]		= "";

	do
	{
		//... Check Recipe Type (Clean or Lot Pre)
		if(RT_LotPreRcp != m_RcpHeader.nRecipeType && RT_CleanRcp != m_RcpHeader.nRecipeType)	break;

		//... Check RPG Auto Process Retry Flag
		nRPGAlmPrcRetryFlag = READ_DIGITAL(RPGAlmPrcRetryDM, &nIOStatus);
		if(eRPGALM_NONE == nRPGAlmPrcRetryFlag)			break;

		//... Check RPG Auto Retry Count
		m_nRPGPrcRetryCnt++;
		WRITE_ANALOG(RPGAlmPrcRetryAM, m_nRPGPrcRetryCnt, &nIOStatus);

		nRPGPrcRetryCntLmt = (int) READ_ANALOG(PRMA_RPGPrcRetryCnt, &nIOStatus);
		if(nRPGPrcRetryCntLmt < 1)
		{	//... Not Use Auto Retry
			bDetailAlmPost = TRUE;
			break;
		}

		if(m_nRPGPrcRetryCnt > nRPGPrcRetryCntLmt)
		{	//... Check Max Count
			ALARM_MESSAGE_GET(ALARM_RPG_AUTO_RETRY_MAX_COUNT_OVER, szAlarm);
			sprintf(szExtAlarm, "%s : Current[%d] Limit[%d]", szAlarm, m_nRPGPrcRetryCnt, nRPGPrcRetryCntLmt);
			ALARM_MESSAGE_SET(ALARM_RPG_AUTO_RETRY_MAX_COUNT_OVER, szExtAlarm);
			ALARM_POST(ALARM_RPG_AUTO_RETRY_MAX_COUNT_OVER);

			bDetailAlmPost = TRUE;
			break;
		}

		//... Check Auto Retry Option by User Config
		nRPGAlmPrcRetryOpt = READ_DIGITAL(PRMD_RPGPrcRetryOpt, &nIOStatus);
		if(eRPGALM_RETRY_PAUSE == nRPGAlmPrcRetryOpt)
		{	//... Process Paused by User Retry
			nRPGAlarmRecovery = -1;
			bDetailAlmPost = TRUE;
		}
		else if(eRPGALM_RETRY_CURRENT_STEP == nRPGAlmPrcRetryOpt)
		{	//... Retry Current Step
			nRPGAlarmRecovery = ALMOPT_RETRY_1;
			Sleep(3000);
		}
		else if(eRPGALM_RETRY_GO_FIRST_STEP == nRPGAlmPrcRetryOpt)
		{	//... Go First Step
			nRPGAlarmRecovery = ALMOPT_RETRY_4;
			Sleep(3000);
		}

		_LOG_PROC("[INFO] RPG Auto Retry Cnt[%d] Lmit[%d] Opt[%d] AlmFlag[%d] RecvRet[%d]",
			m_nRPGPrcRetryCnt, nRPGPrcRetryCntLmt, nRPGAlmPrcRetryOpt,
			nRPGAlmPrcRetryFlag, nRPGAlarmRecovery);

	} while (0);

	if(bDetailAlmPost)
	{
		if     (eRPGALM_REFLECT  == nRPGAlmPrcRetryFlag)		ALARM_POST(ALARM_RPG_REFLECT_IS_HIGH);
		else if(eRPGALM_IGNITION == nRPGAlmPrcRetryFlag)		ALARM_POST(ALARM_RPG_PLASMA_NOT_DETECTED);
	}

	//... Init Auto Retry Flag
	WRITE_DIGITAL(RPGAlmPrcRetryDM, eRPGALM_NONE, &nIOStatus);

	return	nRPGAlarmRecovery;
}

// 2004.07.19 by cskim, 'Step Group Cycle' Added
// 2006.06.14 Modified
Module_Status CProcessSequence::RunNormalProcess()
{
	enum { PRO_ABORT, PRO_RETRY };
	enum { ALMOPT_RETRY_1, ALMOPT_RETRY_2, ALMOPT_RETRY_3, ALMOPT_RETRY_4, ALMOPT_SPARE1, ALMOPT_SPARE2, ALMOPT_SPARE3, ALMOPT_ABORT };
	int nAlarmRecovery = -1;
	int nCmd = 0;

	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	int i, nStepIdx;
	int nStartStep, nStepCount;
	int nStep;

	//... 2015.02.26 RPG Alarm Auto Retry (Only Lot Pre & Dry Clean)
	InitRPGAlarmAutoRetry();

	nStartStep = 1;
	do {
		InitStepGroupRange();
		Init_ProcStepQueue();
		nStepCount = 0;

		if(m_nVRCPBufPageMax >= 5) Start_ContDownloadValveRecipe();				// 2013.06.25

		m_pDynIO->dWRITE_DIGITAL(PRO_FlowControlDM, PRO_ABORT,&nIOStatus);		// 2006.06.29

		do {
			msRet = SYS_SUCCESS;
			for(i = 1; i <= m_RcpHeader.nTotalStep+MAX_QUEUE; i++)
			{
				nStepCount++;
				if(i <= m_RcpHeader.nTotalStep)
				{
					nStep		= i;
					nStepIdx	= i - 1;
				}
				else nStep = 0;

				if(nStepCount >= nStartStep)
				{
					if(IsProcStepValid(nStep))
					{
						msRet = Queue_ProcStep(nStep, nCmd);
						if(nCmd == PCMD_RESTART) nCmd = 0;
						if(nAlarmRecovery == ALMOPT_RETRY_3)
						{
							if(m_ProcQ[QSTEP_RUN].nStep == m_Backup_nStepNum)
							{
								RecoverProcessTime();
								nAlarmRecovery = -1;
							}
						}
					}
					if(msRet != SYS_SUCCESS) break;
				}

				if(nStep > 0)
					if(m_RcpStep[nStepIdx].nTotalLoopCycle > 1)
				{
					printf("-----> m_RcpStep[nStepIdx].nLoopCount : %d\n", m_RcpStep[nStepIdx].nLoopCount);
					m_RcpStep[nStepIdx].nLoopCount++;
					if(m_RcpStep[nStepIdx].nLoopCount < m_RcpStep[nStepIdx].nTotalLoopCycle)
							i = m_RcpStep[nStepIdx].nLoopStart - 1;
				}
			}

			// Modify 2015.01.02 Resume Bug Fixed for 5 Page
			// if(msRet != SYS_SUCCESS) { nStartStep = nStepCount-2; break; }
			// if(msRet != SYS_SUCCESS) { nStartStep = nStepCount-m_nVRCPBufPageMax; break; }
			// End    2014.10.14 Resume Bug Fixed for 5 Page

			if(msRet != SYS_SUCCESS)
			{
				nStartStep = m_ProcQ[QSTEP_RUN].nStep;
				if(nStartStep < 1)		nStartStep = 1;		//... Case. Invalid Step Number
				break;
			}

		} while(0);

		if( (! MANAGER_ABORT())
			&& msRet != SYS_SUCCESS
			&& m_pDynIO->IOValid(PRO_FlowControlDM)
			&& m_pDynIO->dREAD_DIGITAL(PRO_FlowControlDM, &nIOStatus) == PRO_RETRY)
		{
			//... 2014.07.22 Add StepName Init for FDC
			WRITE_STRING(PRO_StepNameSM, "", &nIOStatus);

			PauseProcess();
			_LOG_PROC("-----> Process Paused And Wait For Alarm Recovery");

			//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)
			//---------------------------------------------------------------------------
			WRITE_DIGITAL(DN_ClearSMData_DO, 1, &nIOStatus);
			m_bFirstStepDownload = FALSE;
			//---------------------------------------------------------------------------

			//... Init Recovery Alarm
			nAlarmRecovery = -1;

			//////////////////////////////////////////////////////////////////////////
			//... 2015.02.26 RPG Alarm Auto Retry (Only Lot Pre & Dry Clean)
			if(RT_LotPreRcp == m_RcpHeader.nRecipeType || RT_CleanRcp == m_RcpHeader.nRecipeType)
			{
				nAlarmRecovery = CheckRPGAlarmAutoRetry();
			}

			//////////////////////////////////////////////////////////////////////////
			//... None Selected Recovery Alarm (Normal Retry Alarm)
			if(nAlarmRecovery < 0)		nAlarmRecovery = ALARM_MANAGE(ALARM_PROCESS_PAUSED);

			msRet = SYS_RUNNING;
			nCmd = PCMD_RESTART;
			switch(nAlarmRecovery)
			{
			case ALMOPT_RETRY_1:		// Retry Current Step
				_LOG_PROC("-----> Alarm Recover = RETRY_1(Current Step)");
				break;
			case ALMOPT_RETRY_2:		// Go Next Step
				nStartStep++;
				_LOG_PROC("-----> Alarm Recover = RETRY_2(Next Step)");
				break;
			case ALMOPT_RETRY_3:		// Continue Current Step For Remaining Time
				TrimProcessTime();
				_LOG_PROC("-----> Alarm Recover = RETRY_3(Remaining Time)");
				break;
			case ALMOPT_RETRY_4:		// Go First Step
				nStartStep = 1;
				_LOG_PROC("-----> Alarm Recover = RETRY_4(First Step)");
				break;
			case ALMOPT_ABORT:			// Abort Process
				msRet = SYS_ABORTED;
				_LOG_PROC("-----> Alarm Recover = ABORT(Abort)");
				break;
			}

			if(msRet == SYS_RUNNING) ResumeProcess(nAlarmRecovery);
		}
	} while(msRet == SYS_RUNNING);

	//... 2014.07.22 Add StepName Init for FDC
	WRITE_STRING(PRO_StepNameSM, "", &nIOStatus);

	_LOG_PROC("[UPEH-END] AbortAllFunc Started");
	AbortAllFunc();

	_LOG_PROC("[UPEH-END] StopProcess Started");
	StopProcess();

	//2014.06.02
	if( m_bStablePurgeAfterProc == TRUE &&
		msRet == SYS_SUCCESS &&
		m_RcpHeader.bKeepLastCon == FALSE)
	{
		RUN_FUNCTION_FREE(ABORT_GAS, "STABLE_PURGE");
	}

	// 2007.02.12
	if(m_RcpHeader.nRecipeType == RT_ProcRcp)
	{
		_LOG_PROC("[UPEH-END] CheckPostRunReservation Started");
		CheckPostRunReservation();
	}

	// 2006.03.22 by cskim : Check Module Fail
	if(READ_DIGITAL(SW_ModuleFailDM, &nIOStatus) == 1)		// 0:Off 1:On
	{
		ALARM_POST(ALARM_MODULE_ABNORMAL_FINISHED);
		printf("---> Module failed in %s\n", APP_NAME);
		_LOG_PROC("---> Module failed");
		WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
		msRet = SYS_ERROR;
	}

	if     (SYS_ABORTED == msRet)		printf("-----> Step(%d) aborted in RunNormalProcess\n", i);
	else if(SYS_ERROR == msRet)			printf("-----> Step(%d) error in RunNormalProcess\n", i);

	// 2006.02.13
	if(m_nPrevAmountOption == 0
		|| (m_nPrevAmountOption == 1 && m_RcpHeader.nRecipeType == RT_ProcRcp))
	{
		// 2004.11.26 by cskim, Notice To CTC
		if(READ_DIGITAL(PRO_ComptDM, &nIOStatus) != 0)	// 0:Off 1:On
		{
			char szLog[100];
			memset(szLog, 0, sizeof(szLog));
			sprintf(szLog, "-----> PRO_ComptDM Channel is not dropped");
			_LOG_ERR_P(szLog)
		}
		WRITE_DIGITAL(PRO_ComptDM, 1, &nIOStatus);		// 0:Off 1:On
	}

	return msRet;
}

// 2004.11.18 by cskim, Idle Recipe Cycle
Module_Status CProcessSequence::RunIdleCycProcess()
{
	Module_Status msRet = SYS_SUCCESS;
	int i;
	int	nIOStatus;
	char szLog[80];

	// Idle Purge And Stage Heater Temp make Idle
	int nTargetStep;
	int nStep, nStepIdx;

	if(m_RcpHeader.nIdleStopBegin > 0) nTargetStep = m_RcpHeader.nIdleStopBegin - 1;
	else nTargetStep = m_RcpHeader.nTotalStep;

	m_bIdleStopEnable = TRUE;
	InitStepGroupRange();		//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)
	Init_ProcStepQueue();
	if(m_nVRCPBufPageMax >= 5) Start_ContDownloadValveRecipe();				// 2013.06.25
	do {
		for(i=1; i<=m_RcpHeader.nTotalStep+MAX_QUEUE; i++)
		{
			//... 2015.02.06 Process Retry 기능 수정 (5 Page / EIP)
			if(i <= m_RcpHeader.nTotalStep)
			{
				nStep = i;
				nStepIdx = i - 1;
			}
			else nStep = 0;

			if(IsProcStepValid(nStep))
			{
				msRet = Queue_ProcStep(nStep, 0);
			}
			if(msRet != SYS_SUCCESS) break;

			if(nStep > 0) {
				if(m_RcpStep[nStepIdx].nTotalLoopCycle > 1)
				{
					printf("-----> m_RcpStep[nStepIdx].nLoopCount : %d\n", m_RcpStep[nStepIdx].nLoopCount);
					m_RcpStep[nStepIdx].nLoopCount++;
					if(m_RcpStep[nStepIdx].nLoopCount < m_RcpStep[nStepIdx].nTotalLoopCycle)
						i = m_RcpStep[nStepIdx].nLoopStart - 1;
				}
			}
		}
	} while(0);

	//... 2014.07.22 Add StepName Init for FDC
	WRITE_STRING(PRO_StepNameSM, "", &nIOStatus);

	AbortAllFunc();
	StopProcess();

	//2014.06.02
	if( m_bStablePurgeAfterProc == TRUE &&
		msRet == SYS_SUCCESS &&
		m_RcpHeader.bKeepLastCon == FALSE)
	{
		RUN_FUNCTION_FREE(ABORT_GAS, "STABLE_PURGE");
	}

	if(msRet != SYS_SUCCESS && IsIdleStop() == FALSE)
	{
		if(SYS_ABORTED == msRet)
		{
			sprintf(szLog, "-----> Step(%d) Sequence Aborted in RunIdleCycProcess", i);
			_LOG_PROC(szLog); printf("%s\n", szLog);
		}
		else if(SYS_ERROR == msRet)
		{
			sprintf(szLog, "-----> Step(%d) error in RunIdleCycProcess", i);
			_LOG_PROC(szLog); printf("%s\n", szLog);
		}
		return msRet;
	}

	if(! WAIT_SECONDS(3))
	{
		sprintf(szLog, "-----> Step(%d) Function Aborted in RunIdleCycProcess", i);
		_LOG_PROC(szLog); printf("%s\n", szLog);
		return SYS_ABORTED;
	}

	sprintf(szLog, "> Idle Cycle Stopping.... in RunIdleCycProcess");
	_LOG_PROC(szLog); printf("%s\n", szLog);

	msRet = SYS_SUCCESS;
	m_bIdleStopEnable = FALSE;
	if(m_RcpHeader.nIdleStopBegin > 0) do
	{
		m_bFirstStepDownload = FALSE;											// 2013.09.09
		Init_ProcStepQueue();
		if(m_nVRCPBufPageMax >= 5) Start_ContDownloadValveRecipe();				// 2013.06.25
		for(i=m_RcpHeader.nIdleStopBegin; i<=m_RcpHeader.nTotalStep; i++)
		{
			if(IsProcStepValid(i))
			{
				if(i == m_RcpHeader.nIdleStopBegin) msRet = Queue_ProcStep(i, PCMD_CONT);
				else msRet = Queue_ProcStep(i, 0);
			}
			if(msRet != SYS_SUCCESS) break;
		}
		if(msRet != SYS_SUCCESS) break;

		// 2013.06.25
		do {
			msRet = Queue_ProcStep(0, 0);
			if(msRet != SYS_SUCCESS) break;
		} while(FALSE == IsQueueEmpty());
	} while(0);

	//... 2014.07.22 Add StepName Init for FDC
	WRITE_STRING(PRO_StepNameSM, "", &nIOStatus);

	AbortAllFunc();
	StopProcess();

	if(msRet != SYS_SUCCESS)
	{
		if(SYS_ABORTED == msRet)
		{
			sprintf(szLog, "-----> Step(%d) aborted when Idle Stoppping in RunIdleCycProcess", i);
			_LOG_PROC(szLog); printf("%s\n", szLog);
		}
		else if(SYS_ERROR == msRet)
		{
			sprintf(szLog, "-----> Step(%d) error when Idle Stoppping in RunIdleCycProcess", i);
			_LOG_PROC(szLog); printf("%s\n", szLog);
		}
	}
	return msRet;
}

BOOL CProcessSequence::IsProcStepValid(int nStep)
{
	BOOL bRet = TRUE;
	if(nStep == 0) return TRUE;
	if(FALSE == IsPreTreatEnabled(nStep)) bRet = FALSE;

	return bRet;
}

BOOL CProcessSequence::IsPreTreatEnabled(int nStep)
{
	BOOL bRet = TRUE;
	int nIdx;
	nIdx = nStep - 1;
	if(m_RcpHeader.nRecipeType == RT_PreProRcp && m_RcpStep[nIdx].bPreTreat == TRUE
		&& m_nPreTreatEnable == 0)	// PreTreat => 0:Disable 1:Enable
		bRet = FALSE;
	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Run Process Main Functions

Module_Status CProcessSequence::RunProcess()
{
	char pszLocFtnName[] = "RunProcess";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	Module_Status msStepResult = SYS_ERROR;
	int nIOStatus;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	do {
		_LOG_PROC("[UPEH] Init_Process Started");
		Init_Process();

		_LOG_PROC("[UPEH] ReadyProcess Started");
		if(! ReadyProcess()) break;

		// 2005.06.10 by cskim, Abort Gas Before Process
		if(m_bAbortGasBeforeProc)
		{
			_LOG_PROC("[UPEH] ABORT_GAS Started");
			RUN_FUNCTION(ABORT_GAS, "");
		}

		if(m_bFastRoughBeforeProc)
		{
			_LOG_PROC("[UPEH] FastRough Started");
			if(RUN_FUNCTION(VACUUM_CONTROL, "FAST_ROUGH") != SYS_SUCCESS) break;
		}

		// CVGIsoUseDM => 0:NotUse 1:Use
		if(m_nCVGIsoUseDM == 1) WRITE_DIGITAL(CVGIsoVlvDO, VLV_CLOSE, &nIOStatus);
		WRITE_DIGITAL(PgSwIsoVlvDO, VLV_CLOSE, &nIOStatus);
		WRITE_DIGITAL(FastRoughVlvDO, VLV_OPEN, &nIOStatus);

		if(READ_DIGITAL(ACV_UseDM, &nIOStatus) == 1)
		{
			// 0:NotUse 1:Use
			if(FALSE == CheckACVStatus())
			{
				printf("-----> ACV Control failed in %s\n", pszFtnName);
				break;
			}
		}

		_LOG_PROC("[UPEH] RunNormalProcess Started");
		msStepResult = RunNormalProcess();		// 2006.06.14
		_LOG_PROC("[UPEH-END] RunNormalProcess End");

		// 2004,09.17 by cskim, for Throughput
		if(m_nRF_UseDM == 1)
		{
			_LOG_PROC("[UPEH-END] RF Set 0");
			RUN_FUNCTION_FREE(RF_CONTROL, "SET_PW_VALUE 0");
			_LOG_PROC("[UPEH-END] RF2 Set 0");
			RUN_FUNCTION_FREE(RF_CONTROL2, "SET_PW_VALUE 0");
		}

		// 2005.05.30 by cskim, for Keep Last Step
		if(m_RcpHeader.bKeepLastCon == FALSE || msStepResult != SYS_SUCCESS)
		{
			_LOG_PROC("[UPEH-END] AbortGas Started");
			if(!m_bStablePurgeAfterProc || msStepResult != SYS_SUCCESS) RUN_FUNCTION_FREE(ABORT_GAS, "");
		}

		if(m_nSimMode == SM_SIMUL
			&& m_RcpHeader.bKeepLastCon && msStepResult == SYS_SUCCESS)
		{
			_LOG_PROC("-----> Call KeepLastCon for Simulation");
			WAIT_SECONDS(0.1);
			KeepLastCon(m_RcpHeader.nTotalStep);
		}

		// 2004.12.04 by cskim, To increase throughput
		if(m_bZM_PreLoadPos)
		{
			_LOG_PROC("[UPEH-END] RUN_SET CHUCK_CONTROL[HOME 2] Started");
			RUN_SET_FUNCTION_FREE(CHUCK_CONTROL, "HOME 2");
		}

		// CVGIsoUseDM => 0:NotUse 1:Use
		if(m_nCVGIsoUseDM == 1) CVGSafetyWait();

		if(m_bFastRoughAfterProc && m_RcpHeader.bKeepLastCon == FALSE)
		{
			_LOG_PROC("[UPEH-END] FastRough Started");
			//RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH");
			RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH_&_CARRIER_AR_FLOW");
		}

		SetSwapPurge();      //... 2015.09.28

		// 2004.12.04 by cskim, To increase throughput
		if(m_bZM_PreLoadPos)
		{
			if(SYS_SUCCESS != RUN_WAIT_FUNCTION(CHUCK_CONTROL))
			{
				_LOG_ERR("-----> Z-Motion Control Waiting Error !");
				break;
			}
			_LOG_PROC("[UPEH-END] RUN_WAIT CHUCK_CONTROL[HOME 2] End");
		}

		if(msStepResult != SYS_SUCCESS) break;

		msRet = SYS_SUCCESS;
	}while(0);

	// 2006.02.08
	if(m_bPostSkipEvent)
	{
		// 2007.01.25
		if(CheckPostRunStart() == FALSE || msRet != SYS_SUCCESS)
		{
			if(WRITE_UPLOAD_MESSAGE("POSTSKIP"))
			{
				printf(">>>>> Write Upload Message :POSTSKIP:write success <<<<<\n");
				_LOG_PROC("-----> Write Upload Message :POSTSKIP:write success");
			}
			else
			{
				printf(">>>>> Write Upload Message :POSTSKIP:write failed !!! <<<<<\n");
				_LOG_PROC("-----> Write Upload Message :POSTSKIP:write failed !!!");
			}
		}
		else
		{
			if(WRITE_UPLOAD_MESSAGE("POSTRUN"))
			{
				printf(">>>>> Reservation Post Running succeeded <<<<<\n");
				_LOG_PROC("-----> Reservation Post Running succeeded");
			}
			else
			{
				printf(">>>>> Reservation Post Running failed !!! <<<<<\n");
				_LOG_PROC("-----> Reservation Post Running failed");
			}
		}
	}

	printf("Leaved %s\n", pszFtnName);

	return msRet;
}

//---------------------------------------------------------------------------------------

Module_Status CProcessSequence::RunProcessOut()
{
	char pszLocFtnName[] = "RunProcessOut";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	Module_Status msStepResult = SYS_ERROR;
	int nIOStatus;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		// 2007.01.31
		Init_Process();
		if(! ReadyProcess()) break;

		// 2005.06.10 by cskim, Abort Gas Before Process
		if(m_bAbortGasBeforeProc) RUN_FUNCTION(ABORT_GAS, "");
		if(m_bFastRoughBeforeProc)
		{
			_LOG_PROC("-----> FastRough Started");
			if(RUN_FUNCTION(VACUUM_CONTROL, "FAST_ROUGH") != SYS_SUCCESS) break;
		}

		// CVGIsoUseDM => 0:NotUse 1:Use
		if(m_nCVGIsoUseDM == 1) WRITE_DIGITAL(CVGIsoVlvDO, VLV_CLOSE, &nIOStatus);
		WRITE_DIGITAL(PgSwIsoVlvDO, VLV_CLOSE, &nIOStatus);
		WRITE_DIGITAL(FastRoughVlvDO, VLV_OPEN, &nIOStatus);

		if(READ_DIGITAL(ACV_UseDM, &nIOStatus) == 1) // 0:NotUse 1:Use
			if(FALSE == CheckACVStatus())
		{
			printf("-----> ACV Control failed in %s\n", pszFtnName);
			break;
		}

		msStepResult = RunNormalProcess();		// 2006.06.14

		// 2004,09.17 by cskim, for Throughput
		if(m_nRF_UseDM == 1)
		{
			_LOG_PROC("-----> RF Set 0");
			RUN_FUNCTION_FREE(RF_CONTROL, "SET_PW_VALUE 0");
			//2008.09.29
			_LOG_PROC("-----> RF2 Set 0");
			RUN_FUNCTION_FREE(RF_CONTROL2, "SET_PW_VALUE 0");
		}

		// 2005.05.30 by cskim, for Keep Last Step
		if(m_RcpHeader.bKeepLastCon == FALSE || msStepResult != SYS_SUCCESS)
		{
			_LOG_PROC("-----> AbortGas Started");
			RUN_FUNCTION_FREE(ABORT_GAS, "");
		}
		if(m_nSimMode == SM_SIMUL
			&& m_RcpHeader.bKeepLastCon && msStepResult == SYS_SUCCESS)
		{
			_LOG_PROC("-----> Call KeepLastCon for Simulation");
			WAIT_SECONDS(0.1);
			KeepLastCon(m_RcpHeader.nTotalStep);
		}

		// CVGIsoUseDM => 0:NotUse 1:Use
		if(m_nCVGIsoUseDM == 1) CVGSafetyWait();

		if(m_bFastRoughAfterProc && m_RcpHeader.bKeepLastCon == FALSE)
		{
			_LOG_PROC("-----> FastRough Started");
			//RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH");
			RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH_&_CARRIER_AR_FLOW");
		}

		SetSwapPurge();         //2015.09.28

		if(msStepResult != SYS_SUCCESS) break;
		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//---------------------------------------------------------------------------------------

BOOL CProcessSequence::IsNeededPreProc()
{
	BOOL bRet = FALSE;
	int i;

	// PreTreat => 0:Disable 1:Enable
	if(m_nPreTreatEnable == 1) bRet = TRUE;
	else
	{
		for(i=0; i<m_RcpHeader.nTotalStep; i++)
		{
			if(m_RcpStep[i].bPreTreat == FALSE)
			{
				bRet = TRUE;
				break;
			}
		}
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2014.09.13 by wjkim, Add Recheck Wafer Absent Status
BOOL CProcessSequence::IsWaferAbsent()
{
	BOOL bRet = FALSE;
	int	nIOStatus;
	int nTimeout = 0;

	do
	{
		//... Check Wafer Absent Status
		if(READ_DIGITAL(WaferDI, &nIOStatus) != WAFER_PRESENT)
		{
			bRet = TRUE;
			break;
		}

		//... Check Process Function Abort
		if(! WAIT_SECONDS(0.5))		break;

		//... Check Timeout
		if(++nTimeout > 20)			break;

	} while(1);		//... Unlimited Cycle

	if(! bRet)	ALARM_POST(ALARM_WAFER_DETECTED_CAN_NOT_RUN);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2015.01.02 by wjkim, Add Lot Pre Skip Case
BOOL CProcessSequence::IsLotPreSkipCase()
{
	enum{
		enEasyCon_DisCon,
		enEasyCon_Con,		//.. Standby
		enEasyCon_ConUse,	//.. CTC In Use
		enEasyCon_Dis,		//.. Maint
		enEasyCon_DisHW
	};

	BOOL	bRet = FALSE;	//... Lot Pre Run
	int		nIOStatus;
	int		nEasyClusterSts = enEasyCon_DisCon;

	//... Case System Leak Check
	if(enOFFON_ON == READ_DIGITAL(SysLeakChkDM,&nIOStatus)) return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//... Case. Only Auto Lot Pre Recipe
	nEasyClusterSts = READ_DIGITAL(EasyCluster_Connect, &nIOStatus);
	if(enEasyCon_Con == nEasyClusterSts || enEasyCon_ConUse == nEasyClusterSts)
	{
		//... Before Recipe Saved
		if(strlen(m_szOldLotPreRcpName) > 0 && strlen(m_RcpHeader.szRecipeName) > 0)
		{
			//... Check Lot Recipe Name
			if(strcmp(m_RcpHeader.szRecipeName, m_szOldLotPreRcpName) == 0)
			{	//... Same Lot Pre Recipe Name
				if(SYS_SUCCESS == RUN_FUNCTION(LOTPRE_SKIP_TIMER, "TIMER_CHECK"))		bRet = TRUE;
			}
		}
	}

	memset(m_szOldLotPreRcpName, 0, sizeof(m_szOldLotPreRcpName));
	if(strlen(m_RcpHeader.szRecipeName) > 0)	strcpy(m_szOldLotPreRcpName, m_RcpHeader.szRecipeName);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CProcessSequence::RunPreProcess()
{
	char pszLocFtnName[] = "RunPreProcess";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int	nIOStatus;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	do {
		// PreTreat => 0:Disable 1:Enable
		m_nPreTreatEnable = READ_DIGITAL(PRO_PreTreatDM, &nIOStatus);
		if(IsNeededPreProc() == FALSE)
		{
			msRet = SYS_SUCCESS;
			break;
		}

		msRet = RunProcessOut();


		// PRO_PreTreatDM => 0:Disable 1:Enable
		WRITE_DIGITAL(PRO_PreTreatDM, 0, &nIOStatus);
	}while(0);

	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//---------------------------------------------------------------------------------------

Module_Status CProcessSequence::RunPostProcess()
{
	char pszLocFtnName[] = "RunPostProcess";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
//	int	nIOStatus;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		msRet = RunProcessOut();
	}while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

Module_Status CProcessSequence::RunIdleRcp()
{
	char pszLocFtnName[] = "RunIdleRcp";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	Module_Status msStepResult = SYS_ERROR;
	int nIOStatus;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		// 2007.01.31
		Init_Process();
		if(! ReadyProcess()) break;

		//... 2014.09.13 by wjkim, Add Recheck Wafer Absent Status
		if(! IsWaferAbsent())	break;

		/*
		// 2005.05.05 by cskim, Auto Refill
		// Read Refill Option => 0:No 1:When_DryClean 2:When_RunIdle
		if(m_pDynIO->IOValid(PRMD_Refill_Opt)
				&& 2 == m_pDynIO->dREAD_DIGITAL(PRMD_Refill_Opt, &nIOStatus)) DoRefill();
		*/

		// 2005.06.10 by cskim, Abort Gas Before Process
		if(m_bAbortGasBeforeProc) RUN_FUNCTION(ABORT_GAS, "");
		if(m_bFastRoughBeforeProc)
		{
			_LOG_PROC("-----> FastRough Started");
			if(RUN_FUNCTION(VACUUM_CONTROL, "FAST_ROUGH") != SYS_SUCCESS)
			{
				msRet = SYS_ABORTED;
				break;
			}
		}

		// CVGIsoUseDM => 0:NotUse 1:Use
		if(m_nCVGIsoUseDM == 1) WRITE_DIGITAL(CVGIsoVlvDO, VLV_CLOSE, &nIOStatus);
		WRITE_DIGITAL(PgSwIsoVlvDO, VLV_CLOSE, &nIOStatus);
		WRITE_DIGITAL(FastRoughVlvDO, VLV_OPEN, &nIOStatus);

		if(READ_DIGITAL(ACV_UseDM, &nIOStatus) == 1) // 0:NotUse 1:Use
			if(FALSE == CheckACVStatus())
		{
			printf("-----> ACV Control failed in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		// 2004.11.18 by cskim, Idle Recipe Cycle
		msStepResult = RunIdleCycProcess();

		// 2004,09.17 by cskim, for Throughput
		if(m_nRF_UseDM == 1)
		{
			_LOG_PROC("-----> RF Set 0");
			RUN_FUNCTION_FREE(RF_CONTROL, "SET_PW_VALUE 0");
			//2008.09.29
			_LOG_PROC("-----> RF2 Set 0");
			RUN_FUNCTION_FREE(RF_CONTROL2, "SET_PW_VALUE 0");
		}

		// 2005.05.30 by cskim, for Keep Last Step
		if(m_RcpHeader.bKeepLastCon == FALSE || msStepResult != SYS_SUCCESS)
		{
			_LOG_PROC("-----> AbortGas Started");
			if(!m_bStablePurgeAfterProc || msStepResult != SYS_SUCCESS)		RUN_FUNCTION_FREE(ABORT_GAS, "");
		}
		if(m_nSimMode == SM_SIMUL
			&& m_RcpHeader.bKeepLastCon && msStepResult == SYS_SUCCESS)
		{
			_LOG_PROC("-----> Call KeepLastCon for Simulation");
			WAIT_SECONDS(0.1);
			KeepLastCon(m_RcpHeader.nTotalStep);
		}

		// CVGIsoUseDM => 0:NotUse 1:Use
		if(m_nCVGIsoUseDM == 1) CVGSafetyWait();

		if(m_bFastRoughAfterProc && m_RcpHeader.bKeepLastCon == FALSE)
		{
			_LOG_PROC("-----> FastRough Started");
			//RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH");
			RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH_&_CARRIER_AR_FLOW");
		}

		SetSwapPurge();      //... 2015.09.28

		if(msStepResult != SYS_SUCCESS )
		{
			msRet = SYS_ABORTED;
			break;
		}
		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

Module_Status CProcessSequence::RunCleanRcp()
{
	char pszLocFtnName[] = "RunCleanRcp";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	BOOL bIOStatus;

	int	 nTimeout = 0;

	// 2007.01.25
	if(m_nRecipeIndex >= 0)
	{
		if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM+m_nRecipeIndex, &bIOStatus) != POST_RUN_START) return SYS_SUCCESS;
	}

	m_pDynIO->dWRITE_DIGITAL(PRO_ChamberStatus, DRYCLN_RUN, &bIOStatus);
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);

	printf("Entered %s\n", pszFtnName);
	do {

		//... 2014.09.13 by wjkim, Add Recheck Wafer Absent Status
		if(! IsWaferAbsent())	break;

		//... 2014.07.21 for MAHA-DPT
		// Read Refill Option => 0:No 1:Auto 2:Li_Man
		if(m_pDynIO->IOValid(PRMD_Refill_Opt) && 1 == m_pDynIO->dREAD_DIGITAL(PRMD_Refill_Opt, &nIOStatus))
		{
			DoRefill();
		}

		msRet = RunProcessOut();
	}while(0);

	// 2007.01.25
	if(msRet == SYS_SUCCESS)
	{
		SetPostRunCompleted();

		//////////////////////////////////////////////////////////////////////////
		//... 2014.10.13 Add Post Run Flag Check for Dry Clean Reset Count
		do
		{
			if(m_nRecipeIndex != 0)		break;	//... Only Auto Dry Clean

			//... Check PostRun Flag
			if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM, &bIOStatus) == POST_RUN_OFF)	break;

			if(! WAIT_SECONDS(1))		break;

			if(++nTimeout > DRYCLEAN_CNT_RESET_TIMEOUT)
			{
				ALARM_POST(ALARM_DRY_CLEAN_COUNT_RESET_TIMEOUT);
				break;
			}
		} while (1);

		if(nTimeout > 0)	_LOG_PROC("[CLEAN] Reset Timeout[%d]", nTimeout);
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{	//... 2014.10.22 Add Dry Clean Chamber Status Failed
		if(DRYCLN_RUN == m_pDynIO->dREAD_DIGITAL(PRO_ChamberStatus, &bIOStatus))
			m_pDynIO->dWRITE_DIGITAL(PRO_ChamberStatus, DRYCLN_FAILED, &bIOStatus);
	}

	printf("Leaved %s\n", pszFtnName);
	return msRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2015.01.19
//... Search Leak check Macro Type And Call Leak Check Function
Module_Status CProcessSequence::LeakCheckManager(char *pszFlow)
{
	Module_Status msRet = SYS_ERROR;
	int nIOStatus;
	int i;
	int nMacroType = 0;
	int nIdex = 0;

	//... Toggle System Leak check I/O
	WRITE_DIGITAL(SysLeakChkDM,enOFFON_ON,&nIOStatus);
	printf("Entered LeakCheckManager %s Option\n",pszFlow);
	do
	{
		for(i = 0; i < MAX_MACRO_IO; i++ )
		{
			nMacroType = READ_DIGITAL(LKMacroType1DM + i,&nIOStatus);
			printf("Select LeakCheck Macro%d Option\n",nMacroType+1);

			for( nIdex = 0; nIdex < MAX_LK_MACRO; nIdex++)
			{
				if(nMacroType == g_LeakCheckMacro[nIdex].nMacroIndex)
				{
					if(LeakCHK_ON == READ_DIGITAL(g_LeakCheckMacro[nIdex].nIOLeakChkSts,&nIOStatus))
					{
						printf("Start LeakCheck Macro%d Cmd %s \n",nMacroType,g_LeakCheckMacro[nIdex].pLeakCheckCmd);
						_LOG_PROC("Start LeakCheck Macro%d Cmd %s",nMacroType,g_LeakCheckMacro[nIdex].pLeakCheckCmd);

						msRet = DoLeakChk(g_LeakCheckMacro[nIdex].pLeakCheckCmd);

						//... Only Call Leak check Function at System Leak check
						//... so initialize IO after Leak check
						WRITE_DIGITAL(g_LeakCheckMacro[nIdex].nIOLeakChkSts,LeakCHK_OFF,&nIOStatus);
					}
					if(msRet != SYS_SUCCESS)
					{
						_LOG_PROC("Fail LeakCheck Macro%d Cmd %s",nMacroType,g_LeakCheckMacro[nIdex].pLeakCheckCmd);
						return msRet;
					}
				}
			}
		}

		msRet = SYS_SUCCESS;

	} while (0);

	return msRet;
}

Module_Status CProcessSequence::RunPurgeRcp()
{
	char pszLocFtnName[] = "RunPurgeRcp";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	BOOL bIOStatus;

	// 2007.01.25
	if(m_nRecipeIndex >= 0)
	{
		if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM+m_nRecipeIndex, &bIOStatus) != POST_RUN_START) return SYS_SUCCESS;
	}
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	//... 2015.01.19 Purge Recipe Post Run
	//if(eFlow2 == READ_DIGITAL(ALECHK_RCPDM,&nIOStatus)) msRet = LeakCheckManager("Flow2"); //...2016.05.13

	do {
		msRet = RunProcessOut();
	}while(0);
	// 2007.01.25
	if(msRet == SYS_SUCCESS && m_nRecipeIndex >= 0) SetPostRunCompleted();

	//... 2015.01.19 Purge Recipe Pre Run
	//if(eFlow1 == READ_DIGITAL(ALECHK_RCPDM,&nIOStatus)) msRet = LeakCheckManager("Flow1"); //...2016.05.13

	//... 2016.01.19 PM Leak Check Recipe Stop
	if(IsPurgeStop()){
		msRet = SYS_SUCCESS;
	}

	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CProcessSequence::RunLotPreRcp()
{
	char pszLocFtnName[] = "RunLotPreRcp";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	BOOL bIOStatus;
	int nData     = -1;
	char szTemp[40];
	int nHtrTemp;
	enum { LOTPRE_ENABLE, LOTPRE_DISABLE };

	if(m_nRecipeIndex >= 1)
	{
		if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM+m_nRecipeIndex, &bIOStatus) != POST_RUN_START)		return SYS_SUCCESS;
	}

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	do {
		// 2007.01.31
		if(m_bLotPreAutoDisable && m_pDynIO->IOValid(PRO_LotPreCtrlDM))
		{
			nHtrTemp = (int) GetStgHtrTempSet();
			sprintf(szTemp, "CHECK_SET %d", nHtrTemp);
			if(LOTPRE_DISABLE == m_pDynIO->dREAD_DIGITAL(PRO_LotPreCtrlDM, &bIOStatus)
				&& RUN_FUNCTION(STGHTR_CONTROL, szTemp) == SYS_SUCCESS)
			{
				_LOG_PROC("-----> LotPre Skipped by ProcessControl");
				msRet = SYS_SUCCESS;
				break;
			}
		}

		//... 2014.09.13 by wjkim, Add Recheck Wafer Absent Status
		if(! IsWaferAbsent())	break;

		msRet = RunProcessOut();
	}while(0);
	// 2007.01.25

	if(msRet == SYS_SUCCESS && m_nRecipeIndex >= 1)		SetPostRunCompleted();

	printf("Leaved %s\n", pszFtnName);

	return msRet;
}

Module_Status CProcessSequence::RunLotPostRcp()
{
	char pszLocFtnName[] = "RunLotPostRcp";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		//... 2014.09.13 by wjkim, Add Recheck Wafer Absent Status
		if(! IsWaferAbsent())	break;

		msRet = RunProcessOut();
	}while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//--------------------------------------------------------------------
// 2007.01.31
double CProcessSequence::GetStgHtrTempSet()
{
	int i;
	double dbStgHtrTempSet = -1;
	for(i=m_RcpHeader.nTotalStep; i>=1; i--)
	{
		if(m_RcpStep[i].nStgHtr1_Option == 2
			|| m_RcpStep[i].nStgHtr1_Option == 3
			|| m_RcpStep[i].nStgHtr1_Option == 4)
		{
			dbStgHtrTempSet = m_RcpStep[i].dbStgHtr1_SetPoint;
			break;
		}
	}
	return dbStgHtrTempSet;
}

//--------------------------------------------------------------------
// 2006.12.08
BOOL CProcessSequence::ReadyProcess()
{
	BOOL bRet = FALSE;
	char pszLocFtnName[] = "RunProcessOut";
	int nIOStatus;
	char szTemp[40] = "";  //... 2015.11.25

	enum {E_False, E_True};
	enum {E_NotUse, E_Use};

	do {
		// 2007.02.02
		if (m_RcpHeader.nRecipeType == RT_CleanRcp) {
			if (!CalcStepCycleCompensation()) break;
		}
		else if(m_nStepTimeCompenMode == STCM_Normal)
		{
			// 2007.01.19
			if(!CalcStepTimeCompensation()) break;
		}
		else if(!CalcStepTimeCompensation2()) break;

		// Check Module Fail
		if(READ_DIGITAL(SW_ModuleFailDM, &nIOStatus) == 1)		// 0:Off 1:On
		{
			printf("---> Module fail is detected before process in %s\n", APP_NAME);
			_LOG_PROC("---> Module fail is detected before process");
			break;
		}

		// Check TM Isolation Valve
		// 2004.12.29 by cskim
		int nErrorCount = 0;
		do {
			// 0:False 1:True
			if(1 == READ_DIGITAL(TMIsoVlvClosedDI, &nIOStatus)) {
				if(1 == READ_DIGITAL(TMIsoVlv2ClosedDI, &nIOStatus)) break;
			}
			if(! WAIT_SECONDS(0.5)) break;
		} while(++nErrorCount < 10);
		if(nErrorCount >= 10)
		{
			ALARM_POST(ALARM_TM_ISO_VALVE_NOT_CLOSE);
			break;
		}

		_LOG_PROC("[UPEH] VACUUM_CONTROL[STOP_CARRIER_AR_FLOW] Started");
		RUN_FUNCTION(VACUUM_CONTROL, "STOP_CARRIER_AR_FLOW");

 		// Check SW Interlock
		if(FALSE == CheckSWInterlock())
		{
			printf("-----> SW Global Interlock Detected in %s\n", APP_NAME);
			break;
		}

		// Check PLC Interlock
		if(FALSE == CheckPLCGlobalInterlock())
		{
			printf("-----> PLC Global Interlock Detected in %s\n", APP_NAME);
			break;
		}

		// ACV_UseDM => 0:NotUse 1:Use
		if(READ_DIGITAL(ACV_UseDM, &nIOStatus) == 1) SetACVMode(m_RcpHeader.nRecipeType);


		//... 2015.11.25
		sprintf(szTemp, "READ_CONFIG %d", m_RcpHeader.nRecipeType);

		//... 2014.09.29 wjkim, RF Count Monitor
		if(SYS_SUCCESS != RUN_FUNCTION(RF_COUNT_MONITOR, szTemp))
		{
			printf("[ABORT] RF Count Parameter Read Failed in %s\n", APP_NAME);
			break;
		}

		//... 2016.01.06 Add Baratron Gauge Isolation Valve OPEN
		WRITE_DIGITAL(ProcBGIsoVlvDO,	VLV_OPEN, &nIOStatus);
		WRITE_DIGITAL(VacSwIsoVlvDO,	VLV_OPEN, &nIOStatus);

		// Calculate Process Total Time
		WRITE_ANALOG(PRO_TotalTimeAM, GetProcTotalTime(), &nIOStatus);
		RUN_FUNCTION(TIME_MONITOR, "INIT");
		if(m_nSimMode == SM_NOT_SIM || m_nSimMode == SM_PLC_TEST)
		{
			_LOG_PROC("[UPEH] ReadyPLC Started");
			ReadyPLC();
		}

		// 2004.11.26 by cskim, Init Preventive Maintenance Item
		m_dbVlvCycSum     = 0;
		m_dbSrcFeedingSum = 0;

		bRet = TRUE;
	} while(0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CProcessSequence::DoRefill()
{
	Module_Status msRet = SYS_ABORTED;

	do {
		//... 2014.07.21 for MAHA-DPT
		msRet = RUN_FUNCTION(REFILL_CONTROL,"AUTO_ALL");
	} while(0);

	return msRet;
}
//////////////////////////////////////////////////////////////////////////
/*
//2016.01.19
Module_Status CProcessSequence::DoLineLeakChk()
{
	Module_Status msRet = SYS_ABORTED;

	do {
		if(! WAIT_SECONDS(3)) { msRet = SYS_ABORTED; break;}
		if(READ_FUNCTION(VACUUM_CONTROL) != SYS_RUNNING)
		{
			msRet = RUN_FUNCTION(VACUUM_CONTROL,"GAS_CHECK");
		}
		else
		{
			_LOG_PROC("----->Starting ALD Valve Leak Check, Vacuum Function Running");
		}
	} while(0);

	return msRet;
}
//////////////////////////////////////////////////////////////////////////
//2014.10.30
Module_Status CProcessSequence::DoALDLeakChk()
{
	Module_Status msRet = SYS_ABORTED;

	do {
		if(! WAIT_SECONDS(3)) { msRet = SYS_ABORTED; break;}
		if(READ_FUNCTION(VACUUM_CONTROL) != SYS_RUNNING)
		{
			msRet = RUN_FUNCTION(VACUUM_CONTROL,"ALD_CHECK");
		}
		else
		{
			_LOG_PROC("----->Starting ALD Valve Leak Check, Vacuum Function Running");
		}
	} while(0);

	return msRet;
}
*/
//////////////////////////////////////////////////////////////////////////
//2015.09.17
Module_Status CProcessSequence::DoLeakChk(char *szCmd)
{
	Module_Status msRet = SYS_ABORTED;

	do {
		if(! WAIT_SECONDS(3)) { msRet = SYS_ABORTED; break;}
		if(READ_FUNCTION(VACUUM_CONTROL) != SYS_RUNNING)
		{
			msRet = RUN_FUNCTION(VACUUM_CONTROL,szCmd);		//... 2016.04.03
		}
		else
		{
			_LOG_PROC("----->Starting ALD Valve Leak Check, Vacuum Function Running");
		}
	} while(0);

	return msRet;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
// MFC Mapping Data Reading
//

void CProcessSequence::ParseMFCMap(char *szMapData)
{
	char szBuf[256];
	int nMfcNum;
	int nIdx;

	CTextParser tp(szMapData);
	// Read Tag
	if(FALSE == tp.GetWord(szBuf, 0)) return;
	nMfcNum = atoi(szBuf+3);
	nIdx = nMfcNum - 1;
	// Read Mfc Number
	tp.GetInt(&m_pnMfcMap[nIdx], 0);

	int nRealIdx = m_pnMfcMap[nIdx] - 1;
	tp.GetInt(&m_pnMFC_Scale[nRealIdx]);
	tp.GetInt(&m_pnMFC_PhyMax[nRealIdx]);
}

void CProcessSequence::ParseRFMap(char *szMapData)
{
	char szBuf[256];

	CTextParser tp(szMapData);
	// Read Tag
	if(FALSE == tp.GetWord(szBuf, 0)) return;
	tp.GetInt(&m_nRF_Scale);
	tp.GetInt(&m_nRF_PhyMax);
}

void CProcessSequence::ParseHTEMap(char *szMapData)
{
	char szBuf[256];

	CTextParser tp(szMapData);
	// Read Tag
	if(FALSE == tp.GetWord(szBuf, 0)) return;
	tp.GetInt(&m_nHTE_Scale);
	tp.GetInt(&m_nHTE_PhyMax);
}

void CProcessSequence::ParseValveRecipeMap(char *szMapData)
{
	char szBuf[256];
	int nValveNum;
	int nIdx;

	CTextParser tp(szMapData);
	// Read Tag
	if(FALSE == tp.GetWord(szBuf, 0)) return;
	nValveNum = atoi(szBuf+5);
	nIdx = nValveNum - 1;
	// Read Mfc Number
	tp.GetInt(&m_VRcpMap[nIdx].nAddr, 0);
	tp.GetInt(&m_VRcpMap[nIdx].nBitNum, 0);
}

void CProcessSequence::InitRcpMapData()
{
	int i;
	for(i=0; i<m_nMaxMfc; i++)
	{
		m_pnMfcMap[i] = 0;
	}
	for(i=0; i<m_nMaxValve; i++)
	{
		m_VRcpMap[i].nAddr = 0;
		m_VRcpMap[i].nBitNum = 16;
	}
}

void CProcessSequence::LogRcpMapData()
{
	int i;
	char szLog[256];

	for(i=0; i<m_nMaxMfc; i++)
	{
		sprintf(szLog, "Mfc Idx:%d  MfcNo:%d", i, m_pnMfcMap[i]);
		_LOG_RMAP(szLog);
	}
	for(i=0; i<m_nMaxValve; i++)
	{
		sprintf(szLog, "Valve No:%d Addr:%d BitNum:%d", i+1, m_VRcpMap[i].nAddr,
			m_VRcpMap[i].nBitNum);
		_LOG_RMAP(szLog);
	}
}

BOOL CProcessSequence::ReadRcpMapData()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		InitRcpMapData();
		fp = fopen(RECIPE_MAP_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("MFC", szItem) == 0)				nTitle = 1;
				else if(strcmp("RF", szItem) == 0)			nTitle = 2;
				else if(strcmp("ValveRecipe", szItem) == 0) nTitle = 3;
				else if(strcmp("HeatExchanger", szItem) == 0) nTitle = 4;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseMFCMap(szRead); break;
					case 2: ParseRFMap(szRead); break;
					case 3: ParseValveRecipeMap(szRead); break;
					case 4: ParseHTEMap(szRead); break;				// 2014.05.22
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else
			{
				if(NULL == fgets(szRead, 255, fp)) break;
			}
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	LogRcpMapData();
	return bRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
//	Recipe Reading and Parsing
//

#define _IF_STR(ITEM_NAME, ITEM_VALUE) if(strcmp(szItemName, ITEM_NAME) == 0) \
	strncpy(ITEM_VALUE, szValue, 255);
#define _IF_INT(ITEM_NAME, ITEM_VALUE) if(strcmp(szItemName, ITEM_NAME) == 0) \
	{ if(INT_NUM_CHECK(szValue, 0, 0, &nValue)) ITEM_VALUE = nValue; }

// 2006.01.12
#define _IF_DBL(ITEM_NAME, ITEM_VALUE) if(strcmp(szItemName, ITEM_NAME) == 0) \
	{ if(DB_NUM_CHECK(szValue, &dblValue)) ITEM_VALUE = dblValue; }

#define _IF_DBL_INT(ITEM_NAME, ITEM_VALUE) if(strcmp(szItemName, ITEM_NAME) == 0) \
	{ if(DB_NUM_CHECK(szValue, &dblValue)) ITEM_VALUE = (int) dblValue; }
#define _ELSE_IF_STR(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
	strncpy(ITEM_VALUE, szValue, 255);
#define _ELSE_IF_INT(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
	{ if(INT_NUM_CHECK(szValue, 0, 0, &nValue)) ITEM_VALUE = nValue; }
#define _ELSE_IF_DBL(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
	{ if(DB_NUM_CHECK(szValue, &dblValue)) ITEM_VALUE = dblValue; }
#define _ELSE_IF_DBL_INT(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
	{ if(DB_NUM_CHECK(szValue, &dblValue)) ITEM_VALUE = (int) dblValue; }
#define _ELSE		else
#define _IF			if
#define _ELSE_IF	else if

//---------------------------------------------------------------------------------------

BOOL CProcessSequence::ParseMFC(int nStep, const char* szItem, char* szValue)
{
	BOOL bRet = FALSE;
	int nIdx = nStep-1;
	char szItemName[40];
	int nValue;
	double dblValue;
	int nMfcNum;
	int nMfcIdx;
	int nMapIdx;
	char szLog[256];
	//2014.04.24
	if(m_DummyWafer)
	{
		if(strncmp("MFC_", szItem, 4) == 0) return bRet;
		strncpy(szItemName, szItem+4, strlen(szItem)-1);
		szItem = szItemName;
		strcpy(szItemName, szItem);
	}
	else
	{
		if(strncmp("DMY_MFC_", szItem, 8) == 0) return bRet;
		strcpy(szItemName, szItem);
	}


	do {
		nMapIdx = atoi(szItem + 4);
		nMfcNum = m_pnMfcMap[nMapIdx];
		if(nMfcNum <= 0)
		{
			sprintf(szLog, "Invalid MFC MapData of [Step:%d %s %s]", nStep, szItem, szValue);
			_LOG_RMAP(szLog);
			break;
		}
		else
		{
			nMfcIdx = nMfcNum-1;
			strcpy(szItemName, szItem + 7);
			_IF_DBL("SETPOINT", m_RcpStep[nIdx].pdbMFC_Sp[nMfcIdx])	// 2006.01.12
			_ELSE_IF_DBL("TOLERANCE", m_RcpStep[nIdx].pdbMFC_Tol[nMfcIdx])	// 2006.09.05
			_ELSE_IF_INT("OPTION", m_RcpStep[nIdx].pnMFC_Option[nMfcIdx])
			_ELSE
			{
				if(strcmp("TIMEOUT", szItemName) != 0)
				{
					printf("-----> Unknown Recipe MFC Item [Step:%d %s %s] in Recipe_Reading:ParseMFC\n",
								nStep, szItem, szValue);
					break;
				}
			}
		}
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CProcessSequence::ParseHeater(int nStep, const char* szItem, char* szValue)
{
	BOOL bRet = FALSE;
	int nIdx = nStep-1;
	int nHtrNum;
	int nHtrIdx;
	char szLog[256];

	do {
		nHtrIdx = atoi(szItem+7);
		nHtrNum = nHtrIdx + 1;
		if(strncmp(szItem+10, "SP", 2) == 0)
				m_RcpStep[nIdx].HTR_dbSp[nHtrIdx] = atof(szValue);
		else if(strncmp(szItem+10, "OPT", 3) == 0)
				m_RcpStep[nIdx].HTR_nOpt[nHtrIdx] = atoi(szValue);
		else
		{
			sprintf(szLog, "Heater[%d] Sp:%5.1f Opt:%d", nHtrNum,
					(float) m_RcpStep[nIdx].HTR_dbSp[nHtrIdx],
					m_RcpStep[nIdx].HTR_nOpt[nHtrIdx]);
			_LOG_RCP(szLog);
			break;
		}
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CProcessSequence::ParseValveRecipe(int nStep, const char* szItem, char* szValue)
{
	BOOL bRet = TRUE;
	int nIdx = nStep-1;
	char szItemName[40];
	int nValue;
	double dblValue;
	int nVStep;
	int nVIdx;
	int nLStep;
	int nLIdx;

	int nVlvIdx;
	int nDataIdx;
	int nBitIdx;
	unsigned short usBitMask;
	char szLog[256];
	int i, nMaxValve;

	//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
	char	szValveStatus[1025];	//... szValue Size
	int		nValveA_Idx		= 0;
	int		nValveB_Idx		= 0;
	char	szAlarm[256]	= "";
	char	szExtAlarm[256] = "";

	// 2014.04.24
	if(m_DummyWafer)
	{
		if(strncmp("VAL_", szItem, 4) == 0) return bRet;
		strncpy(szItemName, szItem+4, strlen(szItem)-1);
		szItem = szItemName;
		strcpy(szItemName, szItem);
	}
	else
	{
		if(strncmp("DMY_VAL_", szItem, 8) == 0) return bRet;
		strcpy(szItemName, szItem);
	}

	//strcpy(szItemName, szItem);	2014.04.24
	_IF_INT("VAL_RCP_CYC_CNT", m_RcpStep[nIdx].nVR_CycleCount)
	_ELSE_IF_INT("VAL_TOT_STEP", m_RcpStep[nIdx].nVR_TotalStep)
	_ELSE_IF(strncmp("VAL_CYCCNT", szItem, 10) == 0)
	{
		nVStep = atoi(szItem+11) + 1;		// Valve Step Number
		nVIdx = nVStep - 1;
		if(INT_NUM_CHECK(szValue, 0, 0, &nValue))
		{
			sprintf(szLog,"Item:%s    Value:%s", szItem, szValue);
			_LOG_RCP(szLog);
			sprintf(szLog, "VAL_CYCCNT['%s'] = %d", szValue, nValue);
			_LOG_RCP(szLog);
			m_RcpStep[nIdx].ppVR_Step[nVIdx][m_nBASE_IDX+6] = nValue / 256;
			m_RcpStep[nIdx].ppVR_Step[nVIdx][m_nBASE_IDX+7] = nValue % 256;
		}

		if(nVStep <= m_RcpStep[nIdx].nVR_TotalStep)
		{
			m_RcpStep[nIdx].ppVR_Step[nVIdx][m_nBASE_IDX+0] = nVStep;
			m_RcpStep[nIdx].ppVR_Step[nVIdx][m_nBASE_IDX+1] = m_RcpStep[nIdx].nVR_TotalStep;

			m_RcpStep[nIdx].ppVR_Step[nVIdx][0] = 0;
			m_RcpStep[nIdx].ppVR_Step[nVIdx][1] = 0;
		}
	}
	_ELSE_IF(strncmp("VAL_TIME", szItem, 8) == 0)
	{
		//////////////////////////////////////////////////////////////////////////
		nVStep = atoi(szItem+9) + 1;		// Valve Step Number
		nVIdx = nVStep - 1;
		nLStep = atoi(szItem+12) + 1;		// Step Line Number
		nLIdx = m_nLINE_STEP_BASE+(nLStep-1)*m_nLINE_STEP_SIZE;

		//////////////////////////////////////////////////////////////////////////
		dblValue = 0;
		if(DB_NUM_CHECK(szValue, &dblValue))
		{
			// Valve Time
			if(m_nValveTimeRes == VTS_10MS)
			{
				m_RcpStep[nIdx].ppVR_Step[nVIdx][nLIdx] =  (int) (dblValue * 100 / 256);
				m_RcpStep[nIdx].ppVR_Step[nVIdx][nLIdx+1] = (int) (dblValue * 100) % 256;
			}
			else	// if m_nValveTimeRes == VTS_100ms
			{
				m_RcpStep[nIdx].ppVR_Step[nVIdx][nLIdx] =  (int) (dblValue * 10 / 256);
				m_RcpStep[nIdx].ppVR_Step[nVIdx][nLIdx+1] = (int) (dblValue * 10) % 256;
			}
		}

		if(dblValue > 0 && nLStep > m_RcpStep[nIdx].ppVR_Step[nVIdx][m_nBASE_IDX+8])
		{
			// Total Line Step
			m_RcpStep[nIdx].ppVR_Step[nVIdx][m_nBASE_IDX+9] = nLStep;
		}

		sprintf(szLog, "-> ValveRcp Data : %s %6.3f", szItem, dblValue);
		_LOG_RCP(szLog);
	}
	_ELSE_IF(strncmp("VAL_STATUS", szItem, 10) == 0)
	{
		//////////////////////////////////////////////////////////////////////////
		nVStep = atoi(szItem+11) + 1;		// Valve Step Number
		nVIdx = nVStep - 1;
		nLStep = atoi(szItem+14) + 1;		// Step Line Number
		nLIdx = m_nLINE_STEP_BASE+(nLStep-1)*m_nLINE_STEP_SIZE;

		//////////////////////////////////////////////////////////////////////////
		// 2004.05.12 by cskim
		nMaxValve = strlen(szValue);
		if(nMaxValve > m_nMaxValve)		nMaxValve = m_nMaxValve;
		for(i=0; i<nMaxValve; i++)
		{
			nVlvIdx = i;
			nValue = szValue[i] - '0';
			if(nValue == 1 && m_VRcpMap[nVlvIdx].nAddr >= 1
							&& m_VRcpMap[nVlvIdx].nAddr <= (m_nMaxValve/16))
			{
				nDataIdx = nLIdx + 2 + (m_VRcpMap[nVlvIdx].nAddr-1)*2;
				nBitIdx = m_VRcpMap[nVlvIdx].nBitNum;	// Bit Index
				usBitMask = 0x01;
				usBitMask <<= nBitIdx;
				m_RcpStep[nIdx].ppVR_Step[nVIdx][nDataIdx] |= (usBitMask >> 8);
				m_RcpStep[nIdx].ppVR_Step[nVIdx][nDataIdx+1] |= usBitMask;
			}
		}
		sprintf(szLog, "-> ValveRcp Data : %s %s", szItem, szValue);

		if(m_RcpStep[nIdx].ppVR_Step[nVIdx][nLIdx] != 0
			|| m_RcpStep[nIdx].ppVR_Step[nVIdx][nLIdx+1] != 0)
				strcpy(m_szLastValveStatus, szValue);
		_LOG_RCP(szLog);

		//////////////////////////////////////////////////////////////////////////
		//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
		memset(szValveStatus, 0, sizeof(szValveStatus));
		strcpy(szValveStatus, szValue);
		for(i = 0; i < MAX_INTLK_VALVE_LIST; i++)
		{
			//... Check Valid Valve Number
			if( m_DBOpenIntlkVlv[i].nValveNo_A > 0 &&
				m_DBOpenIntlkVlv[i].nValveNo_B > 0 &&
				m_DBOpenIntlkVlv[i].nValveNo_A <= nMaxValve &&
				m_DBOpenIntlkVlv[i].nValveNo_B <= nMaxValve)
			{	//... Check Case
				nValveA_Idx = m_DBOpenIntlkVlv[i].nValveNo_A - 1;
				nValveB_Idx = m_DBOpenIntlkVlv[i].nValveNo_B - 1;

				// TODO. CHECK CHRACTER POINT
				if( szValveStatus[nValveA_Idx] == '1' &&
					szValveStatus[nValveB_Idx] == '1')
				{	//... Check Valve Interlock
					m_bDBOpenIntlkVlv = TRUE;
					_LOG_ERR("[ERR] INVALID VALVE STEP[%d] VALVE[%s]", nStep, szValveStatus);
					break;
				}
			}
		}
	}
	_ELSE
	{
		if(strcmp("TIMEOUT", szItemName) != 0)
		{
			printf("-----> Unknown Valve Recipe Item [Step:%d %s %s] in Recipe_Reading:ParseValveRecipe\n",
						nStep, szItem, szValue);
			bRet = FALSE;
		}
	}

	//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
	if(m_bDBOpenIntlkVlv)
	{
		ALARM_MESSAGE_GET(ALARM_INVALID_VLVSTS_IN_CTC_RCP, szAlarm);
		sprintf(szExtAlarm, "%s : STEP[%d] Valve[%d] vs Valve[%d]", szAlarm, nStep, nValveA_Idx + 1, nValveB_Idx + 1);
		ALARM_MESSAGE_SET(ALARM_INVALID_VLVSTS_IN_CTC_RCP, szExtAlarm);
		ALARM_POST(ALARM_INVALID_VLVSTS_IN_CTC_RCP);
	}

	return bRet;
}

BOOL CProcessSequence::ParseRecipeStepItem(int nStep, const char* szItem, char* szValue)
{
	BOOL bRet = TRUE;
	int nIdx = nStep-1;
	char szItemName[40];
	int nValue;
	double dblValue;

	strcpy(szItemName, szItem);
	_IF_STR("STEP_NAME", m_RcpStep[nIdx].szStepName)
	_ELSE_IF_INT("MAIN_RCP", m_RcpStep[nIdx].bIsMain)
	_ELSE_IF_DBL("CH_PRES_SETPOINT", m_RcpStep[nIdx].dblCHMPRS_Pres)
	_ELSE_IF_DBL("CH_PRES_SETPOINT_L", m_RcpStep[nIdx].dblCHMPRS_PresL)
	_ELSE_IF_INT("CH_PRES_OPTION", m_RcpStep[nIdx].nCHMPRS_Option)
	// 2006.01.18
	_ELSE_IF_INT("CH_PRES_TIMEOUT", m_RcpStep[nIdx].nCHMPRS_Timeout)

	_ELSE_IF_DBL("THROTTLE_SETPOINT", m_RcpStep[nIdx].dblTVPRS_Sp)
	_ELSE_IF_INT("THROTTLE_OPTION", m_RcpStep[nIdx].nTVPRS_Option)

	_ELSE_IF_DBL_INT("RF_SETPOINT", m_RcpStep[nIdx].nRF_FwSp)
	_ELSE_IF_INT("RF_OPTION", m_RcpStep[nIdx].nRF_Option)
	_ELSE_IF_DBL_INT("RF2_SETPOINT", m_RcpStep[nIdx].nRF2_FwSp)
	_ELSE_IF_INT("RF2_OPTION", m_RcpStep[nIdx].nRF2_Option)

	_ELSE_IF_INT("HAS_VALVE_DATA", m_RcpStep[nIdx].nVR_Enable)
	_ELSE_IF_INT("DMY_HAS_VALVE_DATA", m_RcpStep[nIdx].nVR_Enable_DMY)		//2014.04.24

	_ELSE_IF_DBL("STG_POS_SETPOINT", m_RcpStep[nIdx].dbSTGZPOS_Sp)
	_ELSE_IF_DBL("STG2_POS_SETPOINT", m_RcpStep[nIdx].dbSTGZPOS2_Sp)
	_ELSE_IF_INT("STG_POS_WAIT_OPT", m_RcpStep[nIdx].nSTGZPOS_WaitOpt)

	_ELSE_IF_INT("VAL_CYC_PLUS_OPT", m_RcpStep[nIdx].bValveCycleAccOption)
	_ELSE_IF_INT("PRE_HEAT_TIME", m_RcpStep[nIdx].nPreHeatTime)

	// 2004.07.19 by cskim, 'Step Group Cycle' Added
	_ELSE_IF_INT("ROUGH_VLV", m_RcpStep[nIdx].nRoughValve)

	// 2005.05.31 by cskim, for Backside Ar Check
	_ELSE_IF_INT("BACKSIDE_AR_CHK", m_RcpStep[nIdx].nBacksideArCheck)

	//---------------------------------------------------------------------
	// 2004.11.11 by cskim
	_ELSE_IF_INT("PRE_TREAT", m_RcpStep[nIdx].bPreTreat)
	_ELSE_IF_INT("SUB_ITEM", m_RcpStep[nIdx].bSubItemEnable)
	_ELSE_IF_INT("STEP_FLOW_OPTION", m_RcpStep[nIdx].nStepFlow_Option)

	_ELSE_IF_DBL("STGHTR1_SETPOINT", m_RcpStep[nIdx].dbStgHtr1_SetPoint)
	_ELSE_IF_DBL("RAMP1_SETPOINT", m_RcpStep[nIdx].dbRamp1_SetPoint)
	_ELSE_IF_INT("STGHTR1_OPTION", m_RcpStep[nIdx].nStgHtr1_Option)
	_ELSE_IF_INT("RAMP1_OPTION", m_RcpStep[nIdx].nRamp1_Option)
	_ELSE_IF_DBL("STGHTR1_TOL", m_RcpStep[nIdx].dbStgHtr1_Tol)
	_ELSE_IF_DBL("STGHTR_TIMEOUT", m_RcpStep[nIdx].dbStgHtr_Timeout)

	_ELSE_IF_DBL("STGHTR2_SETPOINT", m_RcpStep[nIdx].dbStgHtr2_SetPoint)
	_ELSE_IF_DBL("RAMP2_SETPOINT", m_RcpStep[nIdx].dbRamp2_SetPoint)
	_ELSE_IF_INT("STGHTR2_OPTION", m_RcpStep[nIdx].nStgHtr2_Option)
	_ELSE_IF_INT("RAMP2_OPTION", m_RcpStep[nIdx].nRamp2_Option)
	_ELSE_IF_DBL("STGHTR2_TOL", m_RcpStep[nIdx].dbStgHtr2_Tol)

	//---------------------------------------------------------------------
	// 2006.01.16
	_ELSE_IF_INT("_MFC_RAMP_OPT", m_RcpStep[nIdx].nMFC_RampOpt)
	_ELSE_IF_DBL("STGTMP_OFFSET1_SP", m_RcpStep[nIdx].StgOffs_dbSp)
	_ELSE_IF_INT("STGTMP_OFFSET1_OPT", m_RcpStep[nIdx].StgOffs_nOpt)

	_ELSE_IF_DBL("STGTMP_OFFSET2_SP", m_RcpStep[nIdx].StgOffs2_dbSp)
	_ELSE_IF_INT("STGTMP_OFFSET2_OPT", m_RcpStep[nIdx].StgOffs2_nOpt)
	// 2006.01.24
	_ELSE_IF_INT("STEP_TM_CMP_OPT", m_RcpStep[nIdx].StepTimeCompen_nOpt)
	_ELSE_IF_INT("STEP_TM_CMP_PREV", m_RcpStep[nIdx].StepTimeCompen_nPrev)
	_ELSE_IF_DBL("STEP_TM_CMP_RATE", m_RcpStep[nIdx].StepTimeCompen_dbRate)
	_ELSE_IF_DBL("STEP_TM_CMP_MAX", m_RcpStep[nIdx].StepTimeCompen_dbMaxTime)	// 2006.05.18
	_ELSE_IF_INT("CLEAN_CYCLE_TYPE", m_RcpStep[nIdx].nCleanCycType)             // 2007.12.06
	// 2014.04.01
	_ELSE_IF_DBL("RF_LDPRST_SETPOINT", m_RcpStep[nIdx].dbRFLdPrst_SetPoint)
	_ELSE_IF_INT("RF_LDPRST_OPTION", m_RcpStep[nIdx].nRFLdPrst_Option)
	_ELSE_IF_DBL("RF_TNPRST_SETPOINT", m_RcpStep[nIdx].dbRFTnPrst_SetPoint)
	_ELSE_IF_INT("RF_TNPRST_OPTION", m_RcpStep[nIdx].nRFTnPrst_Option)
	_ELSE_IF_DBL("RF2_LDPRST_SETPOINT", m_RcpStep[nIdx].dbRF2LdPrst_SetPoint)
	_ELSE_IF_INT("RF2_LDPRST_OPTION", m_RcpStep[nIdx].nRF2LdPrst_Option)
	_ELSE_IF_DBL("RF2_TNPRST_SETPOINT", m_RcpStep[nIdx].dbRF2TnPrst_SetPoint)
	_ELSE_IF_INT("RF2_TNPRST_OPTION", m_RcpStep[nIdx].nRF2TnPrst_Option)
	// 2014.05.22
	_ELSE_IF_INT("PRSTMODE", m_RcpStep[nIdx].nPrstMode_Option)
	_ELSE_IF_INT("PRST2MODE", m_RcpStep[nIdx].nPrst2Mode_Option)
	_ELSE_IF_INT("PRSTSELMODE", m_RcpStep[nIdx].nPrstSel_Option)
	_ELSE_IF_INT("PRST2SELMODE", m_RcpStep[nIdx].nPrst2Sel_Option)

	_ELSE_IF_DBL("HTE_SETPOINT", m_RcpStep[nIdx].dbHte_SetPoint)
	_ELSE_IF_INT("HTE_OPTION", m_RcpStep[nIdx].nHte_Option)
	// 2014.05.22
	_ELSE_IF_INT("SUB_FUNCTION", m_RcpStep[nIdx].nSubFunc)
	//-------------------------------------------------------------------------
	_ELSE if(strncmp("MFC", szItemName, 3) == 0)		bRet = ParseMFC(nStep, szItemName, szValue);
	//2014.04.24
	_ELSE if(strncmp("DMY_MFC", szItemName, 7) == 0)
			bRet = ParseMFC(nStep, szItemName, szValue);
	_ELSE if(strncmp("HEATER", szItemName, 6) == 0)		bRet = ParseHeater(nStep, szItemName, szValue);
	_ELSE if(strncmp("VAL", szItemName, 3) == 0)		bRet = ParseValveRecipe(nStep, szItemName, szValue);
	//2014.04.24
	_ELSE if(strncmp("DMY_VAL", szItemName, 7) == 0)
			bRet = ParseValveRecipe(nStep, szItemName, szValue);
	_ELSE
	{
		// guabage data
		if(strcmp("STG_POS_TOLERANCE", szItemName) != 0
			&& strcmp("STG_POS_OPTION", szItemName) != 0
			&& strcmp("STG_POS_TIMEOUT", szItemName) != 0
			&& strcmp("CH_PRES_TIMEOUT", szItemName) != 0
			&& strcmp("THROTTLE_TIMEOUT", szItemName) != 0
			&& strcmp("RF_TIMEOUT", szItemName) != 0
			&& strcmp("CVD_STEP_TIME", szItemName) != 0
			&& strcmp("SUB_ITEM2", szItemName) != 0
			&& strcmp("STEP_CYC_CMP_MAX", szItemName) != 0
			)
		{
			printf("-----> Unknown Recipe Item [Step:%d %s %s] in Recipe_Reading:ParseRecipeStepItem\n",
						nStep, szItemName, szValue);
			bRet = FALSE;
		}
	}
	//2014.04.24
	if(m_DummyWafer)
	{
		m_RcpStep[nIdx].nVR_Enable = m_RcpStep[nIdx].nVR_Enable_DMY;
	}
	return bRet;
}

Module_Status CProcessSequence::Recipe_Reading(BOOL *appenddata) {
	char pszLocFtnName[] = "Recipe_Reading";
	char pszFtnName[256];
	Module_Status msRet = SYS_SUCCESS;
	int	Dl , iii , i = 0 , j , l , Total_Step_Count;
	int ii_return;
	double db_return;
	char *Ed;
	char	NameList[ 1025 ];
	char	ValueList[ 1025 ];
	BOOL	FindName = FALSE , DataStart = FALSE;
	char	szLog[256];
	int nIOStatus = 0, nDummyWaferID, nDummyRecipeUse , nWaferStatus2 = 0;		//2014.04.24

	_LOG_PROC("Recipe Reading");
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );

//	STEP_COUNT = 0;

	// 2004.04.12 by cskim
	*appenddata = FALSE;
	if ( ( Ed[0] != SEP_LEFT_BLACKET ) || ( Ed[1] != '$' ) || ( Ed[2] != 'A' ) || ( Ed[3] != SEP_NAME_DATA ) ) {
		STEP_COUNT = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	for ( iii = 0 ; iii < Dl ; iii++ )
	{
		if ( MANAGER_ABORT() )
		{
			_LOG_PROC("-----> Manager Abort in Recipe_Reading");
			return SYS_ABORTED;
		}

		if      ( Ed[ iii ] == SEP_LEFT_BLACKET ) { DataStart = TRUE; FindName  = FALSE; i = 0; }
		else if ( Ed[ iii ] == SEP_RIGHT_BLACKET )
		{
			i = 0;
			DataStart = FALSE;
			if ( FindName )
			{
				// 2004.05.12 by cskim
				sprintf(szLog, "%s   %s", NameList, ValueList);
				_LOG_RCP_T(szLog);

				if ( strcmp( "$A" , NameList ) == 0 ) {
				}
				// 2004.04.12 by cskim      $B : Append Data
				else if ( strcmp( "$B" , NameList ) == 0 ) {
					*appenddata = TRUE;  // 이게 TRUE이면 이어지는 데이타가 있는것으로 판별하여 아래의 Program_Main에서 SYS_SUCCESS로 보내고 다시 Data를 받도록함
				}
				else if ( strcmp( "$I" , NameList ) == 0 ) {
					if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) ) {
						WAFER_INFORMATION = ii_return;
					}
				}
				else if ( strcmp( "$R" , NameList ) == 0 ) {
					// Parse RecipeName
					// 2014.04.23 RcpHandler2 Update for Recipe Download by Hyuk
					l = strlen( ValueList );
					for ( j = l - 1 ; j >= 0 ; j-- ) {
						if ( ValueList[j] == '\\' || ValueList[j] == '/' || ValueList[j] =='$' ) break;
					}
					strncpy( RECIPE_NAME , ValueList + j + 1 , MAX_RECIPENAME_LEN );
				}
				else if ( strcmp( "$T" , NameList ) == 0 ) {
					// Parse RecipeName Total Step
					if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) ) {
						Total_Step_Count = ii_return;
					}
				}
				else if ( strcmp( "$S" , NameList ) == 0 ) {
					// Parse Ordered Step(Current Reading Step)
					if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) ) {
						STEP_COUNT = ii_return;
					}
				}
				else if ( strcmp( NameList , "$E" ) == 0 ) {
				}
				else if ( strcmp( NameList, "$C" ) == 0 ) {
				}
				else if ( strcmp( NameList, "$F" ) == 0 ) {
				}
				else if ( strcmp( NameList, "$M" ) == 0 ) {
				}
				else if ( strcmp( NameList, "$P" ) == 0 ) {
				}
				else if ( strcmp( NameList, "$X" ) == 0 ) {
					if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) ) {
						m_nRecipeIndex = ii_return;
						_LOG_PROC("-----> Recipe Index : %d", m_nRecipeIndex);
					}
				}
				else
				{
					// Parse RecipeType
					if( STEP_COUNT == 0 ) {
						if( strcmp( NameList , "RECIPE_TYPE" ) == 0 ) {
							if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) )
							{
								m_RcpHeader.nRecipeType = ii_return;
								// printf("-----> Recipe Type = %d\n", m_RcpHeader.nRecipeType);

								// 2005.05.30 by cskim, for Keep Last Step
								m_szLastValveStatus[0] = 0;
								//2014.04.24
								m_DummyWafer = FALSE;
								nDummyWaferID   = READ_DIGITAL(Wafer_Source2 , &nIOStatus);
								nDummyRecipeUse = READ_DIGITAL(DummyRecipeUse, &nIOStatus);
								nWaferStatus2   = READ_DIGITAL(Wafer_Status2 , &nIOStatus);
								if((nDummyWaferID >= DUMMY && m_RcpHeader.nRecipeType == RT_ProcRcp) ||(nWaferStatus2 <= 0 && m_RcpHeader.nRecipeType == RT_ProcRcp))
								{
									if(nDummyRecipeUse == DMYRcp_Use)
										m_DummyWafer = TRUE;
								}
							}
						}
						// printf("-----> STEP_COUNT = 0,  [%s] = [%s]\n", NameList, ValueList);
						else if( strcmp( NameList , "STEP_GRP_BGN" ) == 0 ) {
							if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) )
								m_RcpHeader.nStepGrpBegin = ii_return;
						}
						else if( strcmp( NameList , "STEP_GRP_END" ) == 0 ) {
							if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) )
								m_RcpHeader.nStepGrpEnd = ii_return;
						}
						else if( strcmp( NameList , "STEP_GRP_CYC" ) == 0 ) {
							if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) )
								m_RcpHeader.nStepGrpCyc = ii_return;
						}
						else if( strcmp( NameList , "IDLE_STOP_BGN" ) == 0 ) {
							if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) )
								m_RcpHeader.nIdleStopBegin = ii_return;
						}
						else if( strcmp( NameList , "KEEP_LAST_CON" ) == 0 ) {
							if ( INT_NUM_CHECK( ValueList , 0 , 0 , &ii_return ) )
								m_RcpHeader.bKeepLastCon = ii_return;
						}
						// 2006.01.26
						else if( strcmp( NameList , "PRO_THICKNESS" ) == 0 ) {
							if ( DB_NUM_CHECK( ValueList, &db_return) )
								m_RcpHeader.dbThickness = db_return;
						}
					}
					// Parse RecipeStepItem
					else if ( STEP_COUNT <= MAX_RECIPE_STEP )
					{
						//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
						m_bDBOpenIntlkVlv = FALSE;	//... Init

						//.....
						ParseRecipeStepItem(STEP_COUNT, NameList, ValueList);
						//.....

						//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
						if(m_bDBOpenIntlkVlv)
						{
							msRet = SYS_ERROR;
							break;
						}
					}
					else
					{
						printf("-----> STEP_COUNT is over\n");
						msRet = SYS_ERROR;
						break;
					}
				}
			}
		}
		else
		{
			if ( DataStart ) {
				if ( FindName ) { ValueList[ i ] = Ed[ iii ]; ValueList[ i+1 ] = 0x00;  i++; }
				else {
					if ( Ed[ iii ] == SEP_NAME_DATA ) { NameList[ i ] = 0x00; i = 0; FindName = TRUE; }
					else                    { NameList[ i ] = Ed[ iii ]; i++; }
				}
			}
		}
	}

	if( !(*appenddata) && SYS_SUCCESS == msRet )
	{
		// 2004.04.12 by cskim
		/*
		if(Total_Step_Count != STEP_COUNT) {
			printf("Total_Step = %d     Red_Step = %d\n", Total_Step_Count, STEP_COUNT);
			printf( "Recipe Data has a Error , therefore STEP_COUNT = 0 in %s\n", pszFtnName);
			STEP_COUNT = 0;
			msRet = SYS_ERROR;
		}
		else
		*/

		{
			strcpy(m_RcpHeader.szRecipeName, RECIPE_NAME);
			m_RcpHeader.nTotalStep = STEP_COUNT;

			// 2004.07.19 by cskim 'Step Group Cycle' Added
			if(m_RcpHeader.nStepGrpBegin > 0
				&& m_RcpHeader.nStepGrpEnd > 0
				&& m_RcpHeader.nStepGrpCyc > 1)
			{
				m_RcpHeader.bUseStepGrp = TRUE;
			}
		}
	}

	printf("Leaved %s\n", pszFtnName);
	if(msRet != SYS_SUCCESS) ALARM_POST(ALARM_RECIPE_READING_FAILED);
	return msRet;
}

//---------------------------------------------------------------------------------------
// 2004.09.24
// Valve Recipe Step Header Packet

void CProcessSequence::MakeVRCPStepHeader()
{
	int i, j;
	unsigned char byBitMask;
	int nMfcPhyValue;
	double dbMfcMaxSp;	// 2006.01.12
	int nRFPhyValue;
	int nRF2PhyValue;
	int nTotalStepTime;
	int nHteSetTemp;	// 2014.05.22

	//2014.08.25
	int nCommStatus;
	WRITE_DIGITAL(DN_ClearSMData_DO, 1, &nCommStatus);
	//

	for(i=0; i<m_RcpHeader.nTotalStep; i++)
	{
		// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop
		if(m_RcpStep[i].nStepFlow_Option != SFL_NORMAL_CYC)		m_RcpStep[i].nVR_CycleCount = 0;

		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+0] = 0;				//ProcessStepID	High
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+1] = i+1;			//ProcessStepID	Low
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+2] = 0;				//Flag	High
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+3] = 0;				//Flag	Low
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+4] = 0;				//DataID(EIP추가)	High
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+5] = 0;				//DataID(EIP추가)	Low
		// StepFlow => 0:Normal 1:CheckStgTemp 2:CheckIdleStop
		if(m_RcpStep[i].nStepFlow_Option == SFL_NORMAL_CYC)
		{
			m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+6] = m_RcpStep[i].nVR_CycleCount / 256;		//Cycle	High
			m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+7] = m_RcpStep[i].nVR_CycleCount % 256;		//Cycle	Low
		}
		else
		{
			m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+6] = 10000 / 256;		//Cycle	High
			m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+7] = 10000 % 256;		//Cycle	Low
		}

		//////////////////////////////////////////////////////////////////////////
		//... 2015.02.06 Only MFC SetPoint Control (NotUse Open/Close)
		for(j = 0; j < m_nMaxMfc; j++)
		{
			if(m_pnMFC_Scale[j] != 0)
			{
				/* 2015.02.06 NotUse CLOSE/OPEN Only Setpoint Mode
				if(m_RcpStep[i].pnMFC_Option[j] == MFC_OPT_OPEN)	byBitMask = 0x01;	// Full Open
				else if(m_RcpStep[i].pdbMFC_Sp[j] == 0)				byBitMask = 0x02;	// Close
				else												byBitMask = 0x00;	// SetPoint
				*/

				if(m_RcpStep[i].pnMFC_Option[j] == MFC_OPT_OPEN)	nMfcPhyValue = m_pnMFC_PhyMax[j];	//... Max Flow

				//////////////////////////////////////////////////////////////////////////
				//... Only SetPoint
				byBitMask = 0x00;		// SetPoint

				//...
				dbMfcMaxSp = m_pnMFC_Scale[j];
				// 2006.01.12
				nMfcPhyValue = (int) (m_RcpStep[i].pdbMFC_Sp[j] * m_pnMFC_PhyMax[j] / dbMfcMaxSp);

				//... 2015.02.06 Check Physical Value (Exception Case)
				if     (nMfcPhyValue > m_pnMFC_PhyMax[j])	nMfcPhyValue = m_pnMFC_PhyMax[j];
				else if(nMfcPhyValue < 0)					nMfcPhyValue = 0;

				//...
				m_RcpStep[i].pVR_Header[m_nVRHD_MFC_SP_IDX + j*2]     = nMfcPhyValue / 256;
				m_RcpStep[i].pVR_Header[m_nVRHD_MFC_SP_IDX + j*2 + 1] = nMfcPhyValue % 256;

				int nBufIdx = j/4;
				int nBitIdx = j%4;
				if((nBufIdx & 0x01) == 0)	nBufIdx++;
				else						nBufIdx--;

				m_RcpStep[i].pVR_Header[m_nVRHD_MFC_VL_IDX + nBufIdx] |= (byBitMask << (nBitIdx * 2));
			}
		}

		//
		nRFPhyValue = m_RcpStep[i].nRF_FwSp * m_nRF_PhyMax / m_nRF_Scale;
		m_RcpStep[i].pVR_Header[m_nVRHD_RF_SP_IDX]		= nRFPhyValue / 256;
		m_RcpStep[i].pVR_Header[m_nVRHD_RF_SP_IDX + 1]	= nRFPhyValue % 256;
		//
		nRF2PhyValue = m_RcpStep[i].nRF2_FwSp * m_nRF_PhyMax / m_nRF_Scale;
		m_RcpStep[i].pVR_Header[m_nVRHD_RF2_SP_IDX]		= nRF2PhyValue / 256;
		m_RcpStep[i].pVR_Header[m_nVRHD_RF2_SP_IDX + 1] = nRF2PhyValue % 256;
		// 2014.05.22
		if(m_nHTE_UseDM == 1)
		{
			if(m_RcpStep[i].nHte_Option == 1 && m_nProHteSetCtrl == 0)
			{
				nHteSetTemp = (int)m_RcpStep[i].dbHte_SetPoint * m_nHTE_PhyMax / m_nHTE_Scale;
				m_RcpStep[i].pVR_Header[m_nVRHD_HTE_SP_IDX]		= nHteSetTemp / 256;
				m_RcpStep[i].pVR_Header[m_nVRHD_HTE_SP_IDX + 1]	= nHteSetTemp % 256;
			}
		}
		//
        nTotalStepTime = (int) GetProcStepTimeForPLC(i);
		m_RcpStep[i].pVR_Header[STEP_TIME_SP_IDX]   = nTotalStepTime / 256;
		m_RcpStep[i].pVR_Header[STEP_TIME_SP_IDX+1] = nTotalStepTime % 256;

		printf("DN %d Total Step Time : %d \n", STEP_TIME_SP_IDX , nTotalStepTime);
	}
}

//---------------------------------------------------------------------------------------
// 2006.01.24
BOOL CProcessSequence::CalcStepTimeCompensation()
{
	BOOL bRet = TRUE;
	int nVStep;
	int nVIdx;
	int nLStep;
	int nLIdx;
	int i;
	double dbStepTime;
	double db_A, db_X, db_B;
	double dbPRVD_DRYCWfrCnt;
	double dbPRVA_DRYCSrcFeed;
	double dbPRVA_DRYCThick;
	BOOL bIOStatus;

	dbPRVD_DRYCWfrCnt = m_pDynIO->dREAD_DIGITAL(PRVD_DRYCWfrCnt, &bIOStatus);
	if(! bIOStatus) return TRUE;
	dbPRVA_DRYCSrcFeed = m_pDynIO->dREAD_ANALOG(PRVA_DRYCSrcFeed, &bIOStatus);
	if(! bIOStatus) return TRUE;
	dbPRVA_DRYCThick = m_pDynIO->dREAD_ANALOG(PRVA_DRYCThick, &bIOStatus);
	if(! bIOStatus) return TRUE;
	for(i=0; i<m_RcpHeader.nTotalStep; i++)
	{
		nVStep = 1;		// Valve Step Number
		nVIdx = nVStep - 1;
		nLStep = 1;		// Step Line Number
		nLIdx = m_nLINE_STEP_BASE+(nLStep-1)*m_nLINE_STEP_SIZE;

		db_A = m_RcpStep[i].StepTimeCompen_dbRate;
		db_B = m_RcpStep[i].ppVR_Step[nVIdx][nLIdx]*256 + m_RcpStep[i].ppVR_Step[nVIdx][nLIdx+1];

		if(m_nValveTimeRes == VTS_10MS) db_B /= 100;
		else db_B /= 10;		// if m_nValveTimeRes == VTS_100ms
		db_B *= m_RcpStep[i].nVR_CycleCount;
		m_RcpStep[i].StepTimeCompen_dbStdStepTime = db_B;			// 2006.05.12

		if(m_RcpStep[i].StepTimeCompen_nOpt == 0) continue;
		switch(m_RcpStep[i].StepTimeCompen_nPrev)
		{
		case 0:	// Wafer Count
			db_X = dbPRVD_DRYCWfrCnt;
			break;
		case 1: // Source Feeding
			db_X = dbPRVA_DRYCSrcFeed;
			break;
		case 2: // Thickness
			db_X = dbPRVA_DRYCThick;
			break;
		default:
			db_X = 0;
		}

		#ifdef _DEBUG
		printf("-----> TimeCompen A:%.6f X:%.1f B:%.1f\n", db_A, db_X, db_B);
		#endif
		dbStepTime = db_A * db_X + db_B;

		//--------------------------------------------------------------------------
		// 2006.05.12
		if(m_RcpStep[i].StepTimeCompen_dbMaxTime > db_B && m_RcpStep[i].StepTimeCompen_dbMaxTime > 0)
		{
			if(dbStepTime > m_RcpStep[i].StepTimeCompen_dbMaxTime)
			{
				if(m_bStepTimeCompenMaxAlarm)		// 2007.01.19
				{
					ALARM_POST(ALARM_STEP_TIME_COMPEN_OVER);
					_LOG_PROC("-----> Step[%d] Time Compensation Over Interlock Alarm Raised ![ComT:%.2f, MaxT:%.2f]",
							i+1, dbStepTime, m_RcpStep[i].StepTimeCompen_dbMaxTime);
					bRet = FALSE;
				}
				dbStepTime = m_RcpStep[i].StepTimeCompen_dbMaxTime;
			}
		}

		//--------------------------------------------------------------------------
		// Valve Time
		if(m_nValveTimeRes == VTS_10MS)
		{
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx] =  (int) (dbStepTime * 100 / 256);
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx+1] = (int) (dbStepTime * 100) % 256;
		}
		else	// if m_nValveTimeRes == VTS_100ms
		{
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx] =  (int) (dbStepTime * 10 / 256);
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx+1] = (int) (dbStepTime * 10) % 256;
		}
		_LOG_PROC("-----> Step[%d] Time Compensation: %.2f + %.2f sec", i+1, db_B, dbStepTime-db_B);	// 2006.03.17
	}

	return bRet;
}

//--------------------------------------------------------------------
// 2007.02.02
BOOL CProcessSequence::CalcStepTimeCompensation2()
{
	BOOL bRet = TRUE;
	int nVStep;
	int nVIdx;
	int nLStep;
	int nLIdx;
	int i;
	double dbStepTime;
	double dbValveTime;
	double dbValveTimeMax = 600;
	double db_A, db_X, db_B;
	double dbPRVD_DRYCWfrCnt;
	double dbPRVA_DRYCSrcFeed;
	double dbPRVA_DRYCThick;
	BOOL bIOStatus;

	dbPRVD_DRYCWfrCnt = m_pDynIO->dREAD_DIGITAL(PRVD_DRYCWfrCnt, &bIOStatus);
	if(! bIOStatus) return TRUE;
	dbPRVA_DRYCSrcFeed = m_pDynIO->dREAD_ANALOG(PRVA_DRYCSrcFeed, &bIOStatus);
	if(! bIOStatus) return TRUE;
	dbPRVA_DRYCThick = m_pDynIO->dREAD_ANALOG(PRVA_DRYCThick, &bIOStatus);
	if(! bIOStatus) return TRUE;
	for(i=0; i<m_RcpHeader.nTotalStep; i++)
	{
		nVStep = 1;		// Valve Step Number
		nVIdx = nVStep - 1;
		nLStep = 1;		// Step Line Number
		nLIdx = m_nLINE_STEP_BASE+(nLStep-1)*m_nLINE_STEP_SIZE;

		db_A = m_RcpStep[i].StepTimeCompen_dbRate;
		db_B = m_RcpStep[i].ppVR_Step[nVIdx][nLIdx]*256 + m_RcpStep[i].ppVR_Step[nVIdx][nLIdx+1];

		if(m_nValveTimeRes == VTS_10MS) db_B /= 100;
		else db_B /= 10;		// if m_nValveTimeRes == VTS_100ms

		// 2007.02.02
		db_B *= m_RcpStep[i].nVR_CycleCount;
		m_RcpStep[i].StepTimeCompen_dbStdStepTime = db_B;			// 2006.05.12

		if(m_RcpStep[i].StepTimeCompen_nOpt == 0) continue;
		switch(m_RcpStep[i].StepTimeCompen_nPrev)
		{
		case 0:	// Wafer Count
			db_X = dbPRVD_DRYCWfrCnt;
			break;
		case 1: // Source Feeding
			db_X = dbPRVA_DRYCSrcFeed;
			break;
		case 2: // Thickness
			db_X = dbPRVA_DRYCThick;
			break;
		default:
			db_X = 0;
		}

		#ifdef _DEBUG
		printf("-----> TimeCompen A:%.6f X:%.1f B:%.1f\n", db_A, db_X, db_B);
		#endif
		dbStepTime = db_A * db_X + db_B;

		//--------------------------------------------------------------------------
		// 2006.05.12
		if(m_RcpStep[i].StepTimeCompen_dbMaxTime > db_B && m_RcpStep[i].StepTimeCompen_dbMaxTime > 0)
		{
			if(dbStepTime > m_RcpStep[i].StepTimeCompen_dbMaxTime)
			{
				if(m_bStepTimeCompenMaxAlarm)		// 2007.01.19
				{
					ALARM_POST(ALARM_STEP_TIME_COMPEN_OVER);
					_LOG_PROC("-----> Step[%d] Time Compensation Over Interlock Alarm Raised ![ComT:%.2f, MaxT:%.2f]",
							i+1, dbStepTime, m_RcpStep[i].StepTimeCompen_dbMaxTime);
					bRet = FALSE;
				}
				dbStepTime = m_RcpStep[i].StepTimeCompen_dbMaxTime;
			}
		}

		// Step Time & Step Cycle Calc
		if(m_nValveTimeRes == VTS_10MS) dbValveTimeMax = 600;
		else dbValveTimeMax = 6000;
		m_RcpStep[i].nVR_CycleCount = 1;
		dbValveTime = dbStepTime;
		while(dbValveTime > dbValveTimeMax)
		{
			dbValveTime /= 2;
			m_RcpStep[i].nVR_CycleCount *= 2;
		}

		//--------------------------------------------------------------------------
		// Valve Time
		if(m_nValveTimeRes == VTS_10MS)
		{
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx] =  (int) (dbValveTime * 100 / 256);
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx+1] = (int) (dbValveTime * 100) % 256;
		}
		else	// if m_nValveTimeRes == VTS_100ms
		{
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx] =  (int) (dbValveTime * 10 / 256);
			m_RcpStep[i].ppVR_Step[nVIdx][nLIdx+1] = (int) (dbValveTime * 10) % 256;
		}
		// Valve Cycle
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+6] = m_RcpStep[i].nVR_CycleCount / 256;
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+7] = m_RcpStep[i].nVR_CycleCount % 256;

		_LOG_PROC("-----> Step[%d] Time Compensation: %.2f + %.2f => %.2f sec", i+1, db_B, dbStepTime-db_B, dbStepTime);	// 2006.03.17
		_LOG_PROC("-----> Step[%d] Cycle:%d, ValveTime:%.2f", i+1, m_RcpStep[i].nVR_CycleCount, dbValveTime);				// 2007.02.06
	}

	return bRet;
}

//---------------------------------------------------------------------------------------
// 2007.123.06
BOOL CProcessSequence::CalcStepCycleCompensation()
{
	BOOL   bRet = TRUE;
	double dbPRVA_DRYCThick;
	BOOL   bIOStatus;

	dbPRVA_DRYCThick = m_pDynIO->dREAD_ANALOG(PRVA_DRYCThick, &bIOStatus);
	if(! bIOStatus) return TRUE;
	for(int i=0; i<m_RcpHeader.nTotalStep; i++) {
		if (m_RcpStep[i].StepTimeCompen_nOpt == 0)  continue;
		if (m_RcpStep[i].StepTimeCompen_nPrev != 2) continue;

		int    nCompCycle = 0;
		switch (m_RcpStep[i].nCleanCycType) {
		case TYPE_A:
//			nCompCycle = (int)(((0.09 * (dbPRVA_DRYCThick / 10) + 230) / 60) * 1.5);
			nCompCycle = (int)((0.0002077 * dbPRVA_DRYCThick) + 7.846);
			break;
		case TYPE_B:
//			nCompCycle = (int)(((0.04 * (dbPRVA_DRYCThick / 10) + 1230) / 60) * 1.5);
			nCompCycle = (int)((0.0002077 * dbPRVA_DRYCThick) + 7.846);
			break;
		default: return FALSE;
		}

		if (nCompCycle < 0) return FALSE;
		m_RcpStep[i].nVR_CycleCount = nCompCycle+1;

		// 2008.01.03 correction for plc data
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+6] = m_RcpStep[i].nVR_CycleCount / 256;
		m_RcpStep[i].pVR_Header[m_nVR_HDR_BASE+7] = m_RcpStep[i].nVR_CycleCount % 256;
		_LOG_PROC("-----> Step[%d] Cur Thick %.1f Cycle Compensation Rslt for dry cleaning : %d \n" , i+1 , dbPRVA_DRYCThick , nCompCycle+1);	// 2007.12.06
	}

	return bRet;
}

//--------------------------------------------------------------------
BOOL CProcessSequence::CheckRecipeData()
{
	BOOL bRet = FALSE;
	char szLog[256];
	char szAlarm[256];
	char szExtAlarm[256];

	do {
		// Check Recipe Type
		if(m_RcpHeader.nRecipeType <= 0)
		{
			ALARM_MESSAGE_GET(ALARM_UNKNOWN_RECIPE_TYPE, szAlarm);
			sprintf(szExtAlarm, "%s(Type:%d)", szAlarm, m_RcpHeader.nRecipeType);
			ALARM_MESSAGE_SET(ALARM_UNKNOWN_RECIPE_TYPE, szExtAlarm);
			ALARM_POST(ALARM_UNKNOWN_RECIPE_TYPE);
			sprintf(szLog, "-----> Recipe Type is Invalid (=%d)",
					m_RcpHeader.nRecipeType);
			_LOG_ERR_P(szLog)
			break;
		}

		// Check Step Group Range
		if(m_RcpHeader.nStepGrpBegin == 0 || m_RcpHeader.nStepGrpEnd == 0
			|| m_RcpHeader.nStepGrpCyc == 0)
		{
			if(m_RcpHeader.nStepGrpBegin != 0 || m_RcpHeader.nStepGrpEnd != 0
				|| m_RcpHeader.nStepGrpCyc != 0)
			{
				ALARM_MESSAGE_GET(ALARM_STEP_GROUP_RANGE_IS_INVALID, szAlarm);
				sprintf(szExtAlarm, "%s(GrpStepBegin:%d, GrpStepEnd:%d, Cycle:%d)",
							szAlarm, m_RcpHeader.nStepGrpBegin, m_RcpHeader.nStepGrpEnd, m_RcpHeader.nStepGrpCyc);
				ALARM_MESSAGE_SET(ALARM_STEP_GROUP_RANGE_IS_INVALID, szExtAlarm);
				ALARM_POST(ALARM_STEP_GROUP_RANGE_IS_INVALID);
				sprintf(szLog, "-----> The range of Step Group Recipe is invalid");
				_LOG_PROC(szLog); printf("%s\n", szLog);
				break;
			}
		}
		else
		{
			if(m_RcpHeader.nStepGrpBegin > m_RcpHeader.nStepGrpEnd)
			{
				ALARM_MESSAGE_GET(ALARM_STEP_GROUP_RANGE_IS_INVALID, szAlarm);
				sprintf(szExtAlarm, "%s(GrpStepBegin:%d, GrpStepEnd:%d)",
							szAlarm, m_RcpHeader.nStepGrpBegin, m_RcpHeader.nStepGrpEnd);
				ALARM_MESSAGE_SET(ALARM_STEP_GROUP_RANGE_IS_INVALID, szExtAlarm);
				ALARM_POST(ALARM_STEP_GROUP_RANGE_IS_INVALID);
				sprintf(szLog, "-----> The range of Step Group Recipe is invalid");
				_LOG_PROC(szLog); printf("%s\n", szLog);
				break;
			}
			if(m_RcpHeader.nStepGrpEnd > m_RcpHeader.nTotalStep)
			{
				ALARM_MESSAGE_GET(ALARM_STEP_GROUP_RANGE_IS_INVALID, szAlarm);
				sprintf(szExtAlarm, "%s(GrpStepEnd:%d, TotalStep:%d)",
							szAlarm, m_RcpHeader.nStepGrpEnd, m_RcpHeader.nTotalStep);
				ALARM_MESSAGE_SET(ALARM_STEP_GROUP_RANGE_IS_INVALID, szExtAlarm);
				ALARM_POST(ALARM_STEP_GROUP_RANGE_IS_INVALID);
				sprintf(szLog, "-----> The range of Step Group Recipe is invalid");
				_LOG_PROC(szLog); printf("%s\n", szLog);
				break;
			}
		}

		// Check PreHeatTime and ValveRecipeEnable
		BOOL bRslt = TRUE;
		int i;
		for(i=0; i<m_RcpHeader.nTotalStep; i++)
		{
			if(m_RcpStep[i].nPreHeatTime > 0 && m_RcpStep[i].nVR_Enable == TRUE)
			{
				ALARM_POST(ALARM_PRE_HEAT_CAN_NOT_BE_USED);
				sprintf(szLog, "-----> PreHeatTime is more than 0 and VRCP is enabled [Step:%d]", i+1);
				_LOG_PROC(szLog); printf("%s\n", szLog);
				bRslt = FALSE;
				break;
			}
			if(m_RcpStep[i].nPreHeatTime <= 0 && m_RcpStep[i].nVR_Enable == FALSE)
			{
				ALARM_MESSAGE_GET(ALARM_VALVE_TIME_IS_INVALID, szAlarm);
				sprintf(szExtAlarm, "%s(Step[%d]:StepTime Is 0)", szAlarm, i+1);
				ALARM_MESSAGE_SET(ALARM_VALVE_TIME_IS_INVALID, szExtAlarm);
				ALARM_POST(ALARM_VALVE_TIME_IS_INVALID);
				sprintf(szLog, "-----> PreHeatTime is  0 and VRCP is Disabled [Step:%d]", i+1);
				_LOG_PROC(szLog); printf("%s\n", szLog);
				bRslt = FALSE;
				break;
			}
		}
		if(bRslt == FALSE) break;
		bRet = TRUE;
	} while(0);
	// if(bRet == FALSE) ALARM_POST(ALARM_INVALID_RECIPE_DATA);
	return bRet;
}

//---------------------------------------------------------------------------------------
// 2004.11.25
// PMC Configration Data

void CProcessSequence::ParseSrcFeedMfc(char *szData)
{
	int nListN, nIdx;
	char szBuf[40];

	CTextParser tp(szData);
	do {
		if(! tp.GetInt(&nListN)) break;
		if(nListN < 1 || nListN > 5)
		{
			printf("-----> Invalied PMC Configration : %s\n", szData);
			break;
		}

		nIdx = nListN - 1;
		if(! tp.GetInt(&m_SrcFeedMfc[nIdx].nMfcN)) break;
		if(! tp.GetInt(&m_SrcFeedMfc[nIdx].nValveN1)) break;
		tp.GetWord(szBuf); if(szBuf[0] != '-') m_SrcFeedMfc[nIdx].nValveN2 = atoi(szBuf);
		tp.GetWord(szBuf); if(szBuf[0] != '-') m_SrcFeedMfc[nIdx].nValveN3 = atoi(szBuf);
		tp.GetFloat(&m_SrcFeedMfc[nIdx].fDrift);
		/*
		printf("-----> MfcN:%d VlvN:%d VlvN:%d VlvN:%d Drift:%.2f\n",
				m_SrcFeedMfc[nIdx].nMfcN,
				m_SrcFeedMfc[nIdx].nValveN1,
				m_SrcFeedMfc[nIdx].nValveN2,
				m_SrcFeedMfc[nIdx].nValveN3,
				m_SrcFeedMfc[nIdx].fDrift);
		*/
	} while(0);
}

void CProcessSequence::ParseVrcpConfig(char *szData)
{
	char szItem[40];
	char szValue[40];

	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "ValveTimeResolution") == 0)
		{
			tp.GetWord(szValue);
			tp.MakeLower(szValue);
			if     (strcmp(szValue, "10ms") == 0)		m_nValveTimeRes = VTS_10MS;
			else if(strcmp(szValue, "100ms") == 0)		m_nValveTimeRes = VTS_100MS;
		}
		else if(strcmp(szItem, "ValveRecipeDownloadMethod") == 0)		tp.GetInt(&m_nVRCP_DnMethodDM);
		else if(strcmp(szItem, "ValveRecipeDownloadWaitOption") == 0)	tp.GetInt(&m_nVRCP_DnLdWait);
		else if(strcmp(szItem, "MFCSetControl") == 0)					tp.GetInt(&m_nProMfcSetCtrl);
		else if(strcmp(szItem, "RFSetControl") == 0)					tp.GetInt(&m_nProRfSetCtrl);
		else if(strcmp(szItem, "HTE_SetContorl") == 0)					tp.GetInt(&m_nProHteSetCtrl);	// 2014.05.22
		else if(strcmp(szItem, "VRCP_BufferPage") == 0)					tp.GetInt(&m_nVRCPBufPageMax);	// 2013.06.25
		//
		else if(strcmp(szItem, "RF1_OnCountBitNum") == 0)				tp.GetInt(&m_nRF1_OnCountBitNum);	//... 2014.09.29
		else if(strcmp(szItem, "RF2_OnCountBitNum") == 0)				tp.GetInt(&m_nRF2_OnCountBitNum);	//... 2014.09.29
	} while(0);
}

void CProcessSequence::ParseZMConfig(char *szData)
{
	char szItem[40];
	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "PreLoadPosControl") == 0) tp.GetBOOL(&m_bZM_PreLoadPos);
	} while(0);
}

void CProcessSequence::ParseProcessConfig(char *szData)
{
	char szItem[40];
	char szValue[40];
	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "ProcessAbortNotification") == 0) tp.GetBOOL(&m_bAbortNotif);
		else if(strcmp(szItem, "ProcessFail_Interlock") == 0) tp.GetBOOL(&m_bProcFailInterlock);
		else if(strcmp(szItem, "FastRoughBeforeProcess") == 0) tp.GetBOOL(&m_bFastRoughBeforeProc);
		else if(strcmp(szItem, "FastRoughAfterProcess") == 0) tp.GetBOOL(&m_bFastRoughAfterProc);
		// 2005.06.10 by cskim, Abort Gas Before Process
		else if(strcmp(szItem, "AbortGasBeforeProcess") == 0) tp.GetBOOL(&m_bAbortGasBeforeProc);
		else if(strcmp(szItem, "CleanRecipe_ACVControl") == 0)
		{
			tp.GetWord(szValue);
			if(strcmp(szValue, "No") == 0) m_Clean_nACVControl = 0;				// No
			else if(strcmp(szValue, "Clean") == 0) m_Clean_nACVControl = 1;		// Clean
			else if(strcmp(szValue, "CleanAuto") == 0) m_Clean_nACVControl = 2;	// CleanAuto
		}
		else if(strcmp(szItem, "CleanRecipe_ACVCheck") == 0) tp.GetBOOL(&m_Clean_bACVCheck);
		else if(strcmp(szItem, "PostSkipEvent_Use") == 0) tp.GetBOOL(&m_bPostSkipEvent);
		else if(strcmp(szItem, "StepTimeMode") == 0) tp.GetInt(&m_nStepTimeMode);
		else if(strcmp(szItem, "StepTimeInterlock_Tolerance") == 0) tp.GetDbFloat(&m_dbStepTimeTol);
		else if(strcmp(szItem, "StepTimeInterlock_TolRate") == 0) tp.GetDbFloat(&m_dbStepTimeTolRate);	// 2006.07.20 by cskim
		else if(strcmp(szItem, "LotPreFailOption") == 0) tp.GetInt(&m_nLotPreFailOption);
		else if(strcmp(szItem, "LotPreAutoDisable") == 0) tp.GetBOOL(&m_bLotPreAutoDisable);	// 2007.01.31
		else if(strcmp(szItem, "OnlineAutoPurgeType") == 0) tp.GetInt(&m_nOnlineAutoPurgeType);	// 2007.02.09
		// 2006.02.13
		else if(strcmp(szItem, "PrevAmountOption") == 0) tp.GetInt(&m_nPrevAmountOption);
		// 2006.09.05
		else if(strcmp(szItem, "MfcControlMode") == 0) tp.GetInt(&m_nMfcControlMode);
		// 2007.01.19
		else if(strcmp(szItem, "StepTimeCompenMaxAlarm") == 0) tp.GetBOOL(&m_bStepTimeCompenMaxAlarm);
		// 2007.02.02
		else if(strcmp(szItem, "StepTimeCompenMode") == 0) tp.GetInt(&m_nStepTimeCompenMode);
		// 2013.06.02
		else if(strcmp(szItem, "StablePurgeAfterProcess") == 0) tp.GetBOOL(&m_bStablePurgeAfterProc);
	} while(0);
}

//////////////////////////////////////////////////////////////////////////
//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
void CProcessSequence::ParseDoubleOpenIntlkConfig(char *szData)
{
	int nListN, nIdx;

	CTextParser tp(szData);
	do {
		if(! tp.GetInt(&nListN))		break;

		if(nListN < 1 || nListN > MAX_INTLK_VALVE_LIST)
		{
			printf("[ERROR] Invalid Double Open Interlock Config [%s]\n", szData);
			break;
		}

		//...
		nIdx = nListN - 1;
		if(! tp.GetInt(&m_DBOpenIntlkVlv[nIdx].nValveNo_A))		break;
		if(! tp.GetInt(&m_DBOpenIntlkVlv[nIdx].nValveNo_B))		break;

// 		printf("[INFO] OPEN INTERLOCK [%03d] Valve_A[%d] Valve_B[%d]\n",
// 				nListN,
// 				m_DBOpenIntlkVlv[nIdx].nValveNo_A,
// 				m_DBOpenIntlkVlv[nIdx].nValveNo_B);
	} while(0);
}
//////////////////////////////////////////////////////////////////////////
//... 2017.12.13 Get ALD Valve Information
void CProcessSequence::ParseALDVlvConfig(char *szData)
{
	int nListN = 0, nRealValveNum = 0,nIdx = 0, nIOResult = 0;
	char szIOName[32] = {0,};
	char szPrmIO[32]  = {0,};
	CTextParser tp(szData);
	do {
		if(! tp.GetInt(&nListN))		break;

		if(nListN < 1 || nListN > MAX_ALD_VALVE_LIST)
		{
			printf("[ERROR] Invalid ALD Valve List Config [%s]\n", szData);
			break;
		}

		//...
		nIdx = nListN - 1;
		if(!tp.GetInt(&m_ALDVlvList[nIdx].nVlvNum)) break;

		if(tp.GetInt(&nRealValveNum))
		{
			m_ALDVlvList[nIdx].nRealVlvNum = nRealValveNum;
			sprintf(szIOName,"ALD%d",nRealValveNum);
			strcat(szIOName,"OpenCnAI");
			m_ALDVlvList[nIdx].nIOEnum = _FIND_FROM_STRING(1,szIOName);

			sprintf(szPrmIO,"PRMD%d",nRealValveNum);
			strcat(szPrmIO,"_CNT_INT");
			printf("[ALD Valve Info] Valve Number: %d IO Name [%s]\n", nRealValveNum,szPrmIO);
			m_ALDVlvList[nIdx].szPrmIO = _FIND_FROM_STRING(0,szPrmIO);

		}
		else
		{
			break;
		}


	} while(0);
}
//////////////////////////////////////////////////////////////////////////
BOOL CProcessSequence::ReadPmcConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(PMC_CONFIG_FILE, "rt");
		if(fp == NULL) break;
		// printf("-----> Read PMC Configration Data\n");

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if     (strcmp("Source Feeding MFC List", szItem) == 0)			nTitle = 1;
				else if(strcmp("Valve Recipe Configration", szItem) == 0)		nTitle = 2;
				else if(strcmp("Z-Motion Control", szItem) == 0)				nTitle = 3;
				else if(strcmp("Process Configration", szItem) == 0)			nTitle = 4;
				//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
				else if(strcmp("Double Open Interlock", szItem) == 0)			nTitle = 5;
				//... 2017.12.13 Get ALD Valve Information
				else if(strcmp("ALD Valve List", szItem) == 0)					nTitle = 6;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseSrcFeedMfc(szRead);				break;
					case 2: ParseVrcpConfig(szRead);				break;
					case 3: ParseZMConfig(szRead);					break;
					case 4: ParseProcessConfig(szRead);				break;
					//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
					case 5: ParseDoubleOpenIntlkConfig(szRead);		break;
					//... 2017.12.13 Get ALD Valve Information
					case 6: ParseALDVlvConfig(szRead);				break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s", PMC_CONFIG_FILE);
	}
	return bRet;
}

int CProcessSequence::GetMainStepNumber(int nStep)
{
	int nMainStepNumber = 0;
	int i;
	for(i=0; i<MAX_MAIN_STEP; i++) if(m_pnMainStep[i] == nStep) break;
	if(i < MAX_MAIN_STEP) nMainStepNumber = i+1;
	return nMainStepNumber;
}

// 2005.05.30 by cskim, for Keep Last Step
void CProcessSequence::KeepLastCon(int nStep)
{
	char szParam[256];
	printf("-----> KeepLastCon Function called\n");
	_LOG_PROC("-----> KeepLastCon Function called");
	sprintf(szParam,"SET_VALVE_ARRAY %s", m_szLastValveStatus);
	_LOG_PROC("--------> SET_VALVE_ARRAY %s", m_szLastValveStatus);
	RUN_FUNCTION(VALVE_CONTROL, szParam);

	if(0 == m_nProMfcSetCtrl && nStep != 0)
	{
		_LOG_PROC("--------> Step[%d] Manual MFC Setting Started", nStep);
		StartMFCSetting(nStep, TRUE, FALSE);
	}
	// StartTVSetting(nStep);
}

DWORD WINAPI CProcessSequence::StgHtr2_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *) lParam;
	pBaseApp->SetStgHtr2();
	return 0;
}

void CProcessSequence::StartStgHtr2Setting(int nStep)
{
	m_nStgHtr2_Thread = THREAD_RUN;
	m_nStgHtr2_Step = nStep;
	RunThreadFunc(StgHtr2_ThreadProc, this);
}

void CProcessSequence::SetStgHtr2()
{
	BOOL bRslt = TRUE;
	Module_Status msRslt = SYS_SUCCESS;
	char szTemp[40] = "";
	char szStgOffs[10] = "";
	int nHtrTemp;
	int nRampRate;
	int nIdx;
	nIdx = m_nStgHtr2_Step - 1;

	_LOG_PROC("-----===> Started StgHtr Temp Setting");
	nHtrTemp = (int) m_RcpStep[nIdx].dbStgHtr2_SetPoint;
	if(m_RcpStep[nIdx].nRamp2_Option == 1)
		nRampRate = (int) m_RcpStep[nIdx].dbRamp2_SetPoint;
	else nRampRate = -1;

	// 2006.01.16
	if(m_RcpStep[nIdx].StgOffs2_nOpt == 1) sprintf(szStgOffs, " %d", (int) m_RcpStep[nIdx].StgOffs2_dbSp);

	do {
		// 0:No 1:Check 2:SetTemp 3:Set_If_Upper 4:Set_If_Lower
		if(m_RcpStep[nIdx].nStgHtr2_Option == 0) break;

		switch(m_RcpStep[nIdx].nStgHtr2_Option)
		{
		case 1:
			sprintf(szTemp, "CHECK_SET %d %s", nHtrTemp, szStgOffs);	// 2006.02.27
			msRslt = RUN_FUNCTION(STGHTR_CONTROL2, szTemp);
			if(msRslt != SYS_SUCCESS)
			{
				_LOG_PROC("-----=====> Stage Heater Temp Check Failed");
				ALARM_POST(ALARM_STGHTR_TEMP_CHECK_FAILED);
				bRslt = FALSE;
			}
			break;
		case 2:
			sprintf(szTemp, "SET_TEMP %d %d %s", nHtrTemp, nRampRate, szStgOffs);	// 2006.01.16
			RUN_FUNCTION(STGHTR_CONTROL2, szTemp);
			break;
		case 3:
			sprintf(szTemp, "SET_IF_UPPER %d %d", nHtrTemp, nRampRate);
			RUN_FUNCTION(STGHTR_CONTROL2, szTemp);
			break;
		case 4:
			sprintf(szTemp, "SET_IF_LOWER %d %d", nHtrTemp, nRampRate);
			RUN_FUNCTION(STGHTR_CONTROL2, szTemp);
			break;
		}
		_LOG_PROC("-----=====> SetCommand:%s", szTemp);
	} while(0);
	_LOG_PROC("-----===> Finished StgHtr Temp Setting");

	if(bRslt == TRUE) m_nStgHtr2_Thread = THREAD_STOP;
	else m_nStgHtr2_Thread = THREAD_FAILED;
}


// 2007.01.25
void CProcessSequence::CheckPostRunReservation()
{
	int i;
	BOOL bIOStatus;

	if(m_nOnlineAutoPurgeType == ONLINE_PURGE_SINGLE) return;
	for(i=0; i<POST_RUN_RECIPE_MAX; i++)
	{
		if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM+i, &bIOStatus) == POST_RUN_RESERVE)
					m_pDynIO->dWRITE_DIGITAL(PRO_PostRunDM+i, POST_RUN_START, &bIOStatus);
	}
}

// 2007.02.09
BOOL CProcessSequence::CheckPostRunStart()
{
	BOOL bRet = FALSE;
	int i;
	BOOL bIOStatus;
	for(i=0; i<POST_RUN_RECIPE_MAX; i++)
	{
		if(m_nOnlineAutoPurgeType == ONLINE_PURGE_MULTI)
		{
			if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM+i, &bIOStatus) == POST_RUN_START) { bRet = TRUE; break; }
		}
		else
		{
			if(m_pDynIO->dREAD_DIGITAL(PRO_PostRunDM+i, &bIOStatus) == POST_RUN_RESERVE) { bRet = TRUE; break; }
		}
	}
	return bRet;
}

void CProcessSequence::SetPostRunCompleted()
{
	BOOL bIOStatus;
	enum { POST_NOT_COMPLETED, POST_COMPLETED };
	if(m_nRecipeIndex >= 0)
	{
		m_pDynIO->dWRITE_DIGITAL(PRO_PostCmptDM+m_nRecipeIndex, POST_COMPLETED, &bIOStatus);
		_LOG_PROC("-----> Auto PostRun Completed and 'PRO_PostCmptDM[%d] = 1(Completed)' Set", m_nRecipeIndex);
	}
	else
	{
		m_pDynIO->dWRITE_DIGITAL(PRO_PostCmptDM, POST_COMPLETED, &bIOStatus);
		_LOG_PROC("-----> Manual PostRun Completed and 'PRO_PostCmptDM = 1(Completed)' Set");
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL CProcessSequence::GetStepRFOnCount(int nStep, int *pnRF1Count, int *pnRF2Count)
{
	BOOL	bRet = FALSE;
	int		nIdx = 0;
	int		i, k;
	int		nTotalLineStep = 0;
	double	dbLineStepTime = 0;

	int		nRF1Count	= 0;
	int		nRF2Count	= 0;

	BOOL	bRF1_OnSts		= FALSE;
	BOOL	bRF2_OnSts		= FALSE;
	BOOL	bRF1_OnSts_OLD	= FALSE;
	BOOL	bRF2_OnSts_OLD	= FALSE;
	BOOL	bRF1_OnSts_1st	= FALSE;
	BOOL	bRF2_OnSts_1st	= FALSE;

	int		nVR_CycleCount	= 1;

	unsigned char *pData;

	*pnRF1Count = 0;
	*pnRF2Count = 0;

	//...
	nIdx = nStep - 1;

	//... Init RF On Status
	bRF1_OnSts = FALSE;		bRF1_OnSts_OLD = FALSE;
	bRF2_OnSts = FALSE;		bRF2_OnSts_OLD = FALSE;

	//////////////////////////////////////////////////////////////////////////
	//... Check Current Step
	for(i = 0; i < m_RcpStep[nIdx].nVR_TotalStep; i++)
	{
		nTotalLineStep = m_RcpStep[nIdx].ppVR_Step[i][11];
		for(k = 0; k < nTotalLineStep; k++)
		{
			//... Read VRCP Data
			pData = m_RcpStep[nIdx].ppVR_Step[i]+m_nLINE_STEP_BASE+k*m_nLINE_STEP_SIZE;

			//... Check Line Step
			dbLineStepTime = ((double)(pData[0]*256 + pData[1])) * 0.01;	// 10 ms
			if(dbLineStepTime < 0.001)		continue;			//... NotUse Line Step

			//... RF1 ON Status (RF On Bit)
			if(m_nRF1_OnCountBitNum < 0 || m_nRF1_OnCountBitNum >= m_nMaxValve)		m_nRF1_OnCountBitNum = 94;	// Invalid Case. Fixed
			bRF1_OnSts = CheckFeedingValve(m_nRF1_OnCountBitNum, pData+2);

			//... RF2 ON Status (RF On Bit)
			if(m_nRF2_OnCountBitNum < 0 || m_nRF2_OnCountBitNum >= m_nMaxValve)		m_nRF2_OnCountBitNum = 95;	// Invalid Case. Fixed
			bRF2_OnSts = CheckFeedingValve(m_nRF2_OnCountBitNum, pData+2);

			//...
			if(TRUE == bRF1_OnSts && FALSE == bRF1_OnSts_OLD)			nRF1Count++;
			if(TRUE == bRF2_OnSts && FALSE == bRF2_OnSts_OLD)			nRF2Count++;

			//...
			bRF1_OnSts_OLD = bRF1_OnSts;
			bRF2_OnSts_OLD = bRF2_OnSts;

			//... Check First Step
			if(k == 0)
			{
				bRF1_OnSts_1st = bRF1_OnSts;
				bRF2_OnSts_1st = bRF2_OnSts;
			}

// 			printf("Step[%d] Line[%d] Time[%.2f] RF On[%d][%d] Cnt[%d][%d]\n",
// 				nStep, k+1, dbLineStepTime, bRF1_OnSts, bRF2_OnSts, nRF1Count, nRF2Count);
		}

	}

	//////////////////////////////////////////////////////////////////////////
	//... Final Result
	nVR_CycleCount = m_RcpStep[nIdx].nVR_CycleCount;	//.. Step Cycle
	if(nVR_CycleCount > 1)
	{
		//...
		nRF1Count = nVR_CycleCount * nRF1Count;
		nRF2Count = nVR_CycleCount * nRF2Count;

		//... Case. First & Last RF On Continuous
		if(nRF1Count > 0 && TRUE == bRF1_OnSts && bRF1_OnSts_1st == bRF1_OnSts)
			nRF1Count = nRF1Count - nVR_CycleCount + 1;

		if(nRF2Count > 0 && TRUE == bRF2_OnSts && bRF2_OnSts_1st == bRF2_OnSts)
			nRF2Count = nRF2Count - nVR_CycleCount + 1;
	}

	printf("[INFO] Step[%d] TotStep[%d] StepCyc[%d] TotLine[%d] Cnt[%d][%d]\n",
		nStep, m_RcpStep[nIdx].nVR_TotalStep, nVR_CycleCount, nTotalLineStep, nRF1Count, nRF2Count);

	//... NotUse RF Step
	if(nRF1Count == 0 && nRF2Count == 0)
	{
		*pnRF1Count = 0;
		*pnRF2Count = 0;
		bRet = FALSE;
	}
	else
	{
		*pnRF1Count = nRF1Count;
		*pnRF2Count = nRF2Count;
		bRet = TRUE;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
void CProcessSequence::GetALDVlvOnCount(int nStep)
{
	BOOL	bRet = FALSE;
	int		nIdx = 0;
	int		i, k;
	int		nTotalLineStep = 0;
	double	dbLineStepTime = 0;

	int		nALDVlvOpenCnt	= 0;
	int		nALDVlvIndex	= 0;
	int		nALDVlvList		= 0;
	//... Init ALD Valve Open Status
	int		szALDVlvOpenSts[MAX_ALD_VALVE_LIST]		= {0,};
	int		szALDVlvOpenSts_OLD[MAX_ALD_VALVE_LIST] = {0,};
	int		szALDVlvOpenSts_1st[MAX_ALD_VALVE_LIST] = {0,};
	int		szALDVlvOpenCnt[MAX_ALD_VALVE_LIST] = {0,};
	int		szALDFinalValveSts[MAX_ALD_VALVE_LIST] = {0,};


	int		nVR_CycleCount	= 1;

	unsigned char *pData;

	//...
	nIdx = nStep - 1 ;
	//////////////////////////////////////////////////////////////////////////
	//... Read Final ALD Valve Status
	for(nALDVlvList = 0; nALDVlvList < MAX_ALD_VALVE_LIST; nALDVlvList++)
	{
		szALDFinalValveSts[nALDVlvList] = m_ALDVlvList[nALDVlvList].nStepFinalVlvSts;
	}

	//////////////////////////////////////////////////////////////////////////
	//... Check Current Step
	for(i = 0; i < m_RcpStep[nIdx].nVR_TotalStep; i++)
	{
		nTotalLineStep = m_RcpStep[nIdx].ppVR_Step[i][11];
		for(k = 0; k < nTotalLineStep; k++)
		{
			//... Read VRCP Data
			pData = m_RcpStep[nIdx].ppVR_Step[i]+m_nLINE_STEP_BASE+k*m_nLINE_STEP_SIZE;

			//... Check Line Step
			dbLineStepTime = ((double)(pData[0]*256 + pData[1])) * 0.01;	// 10 ms
			if(dbLineStepTime < 0.001)		continue;			//... NotUse Line Step

			//... ALD Valve Open Status (ALD Valve Open Bit)
			for(nALDVlvList = 0; nALDVlvList < MAX_ALD_VALVE_LIST; nALDVlvList++)
			{
				nALDVlvOpenCnt = 0;
				nALDVlvIndex = m_ALDVlvList[nALDVlvList].nVlvNum - 1;
				if(nALDVlvIndex < 0 || nALDVlvIndex > m_nMaxValve)		continue;
				else szALDVlvOpenSts[nALDVlvList] = (int)CheckFeedingValve(nALDVlvIndex, pData+2);
				printf("[info ald]%d %d sts %d\n",nALDVlvIndex,m_ALDVlvList[nALDVlvList].nVlvNum,szALDVlvOpenSts[nALDVlvList]);
				//////////////////////////////////////////////////////////////////////////
				//... Save Final Valve Open Status
				m_ALDVlvList[nALDVlvList].nStepFinalVlvSts = szALDVlvOpenSts[nALDVlvList];
				/////////////////////////////////////////////////////////////////////////
				if(1 == szALDVlvOpenSts[nALDVlvList] && 0 == szALDVlvOpenSts_OLD[nALDVlvList])
				{
					nALDVlvOpenCnt++;
					szALDVlvOpenCnt[nALDVlvList] = nALDVlvOpenCnt;
					printf("[info]%d %d %d\n",nALDVlvList,m_ALDVlvList[nALDVlvList].nVlvNum,szALDVlvOpenCnt[nALDVlvList]);
				}

				szALDVlvOpenSts_OLD[nALDVlvList] = szALDVlvOpenSts[nALDVlvList];

				//... Check First Step
				if(k == 0)
				{
					szALDVlvOpenSts_1st[nALDVlvList] = szALDVlvOpenSts[nALDVlvList];
				}
			}
		}

	}

	//////////////////////////////////////////////////////////////////////////
	//... Final Result
	nVR_CycleCount = m_RcpStep[nIdx].nVR_CycleCount;	//.. Step Cycle
	if(nVR_CycleCount > 1)
	{
		//...
		for(nALDVlvList = 0; nALDVlvList < MAX_ALD_VALVE_LIST; nALDVlvList++)
		{
			if(nIdx > 0
				&& szALDFinalValveSts[nALDVlvList] == 1
				&& szALDVlvOpenSts_1st[nALDVlvList] == 1)
			{

				m_ALDVlvList[nALDVlvList].nALDOpenCnt -= 1;
			}

			m_ALDVlvList[nALDVlvList].nALDOpenCnt += nVR_CycleCount * szALDVlvOpenCnt[nALDVlvList];


			//... Case. First & Last ALD Valve Continuous
			if(m_ALDVlvList[nALDVlvList].nALDOpenCnt > 0
				&& 1 == szALDVlvOpenSts[nALDVlvList]
				&& szALDVlvOpenSts_1st[nALDVlvList] == szALDVlvOpenSts[nALDVlvList])
			{
				m_ALDVlvList[nALDVlvList].nALDOpenCnt = (m_ALDVlvList[nALDVlvList].nALDOpenCnt - nVR_CycleCount + 1);
			}

			printf("[INFO] Step[%d] TotStep[%d] StepCyc[%d] TotLine[%d] ALD Valve[%d] Cnt[%d]\n",
				nStep, m_RcpStep[nIdx].nVR_TotalStep, nVR_CycleCount, nTotalLineStep,m_ALDVlvList[nALDVlvList].nVlvNum, m_ALDVlvList[nALDVlvList].nALDOpenCnt);
		}

	}
	else
	{
		for(nALDVlvList = 0; nALDVlvList < MAX_ALD_VALVE_LIST; nALDVlvList++)
		{
			if(nIdx > 0
				&& szALDFinalValveSts[nALDVlvList] == 1
				&& szALDVlvOpenSts_1st[nALDVlvList] == 1)
			{

				m_ALDVlvList[nALDVlvList].nALDOpenCnt -= 1;
			}

			m_ALDVlvList[nALDVlvList].nALDOpenCnt += szALDVlvOpenCnt[nALDVlvList];

	        printf("[INFO] Step[%d] TotStep[%d] StepCyc[%d] TotLine[%d] ALD Valve[%d] Cnt[%d]\n",
				nStep, m_RcpStep[nIdx].nVR_TotalStep, nVR_CycleCount, nTotalLineStep,m_ALDVlvList[nALDVlvList].nVlvNum, m_ALDVlvList[nALDVlvList].nALDOpenCnt);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//... 2014.09.29 wjkim, RF Count Monitor
void CProcessSequence::CheckRFPostCount(int nStep)
{
	char	szTemp[40] = "";

	int		nIdx		= 0;
	int		nRF1Count	= 0;
	int		nRF2Count	= 0;

	nIdx = nStep - 1;

	//////////////////////////////////////////////////////////////////////////
	//... Case. Check Skip RF All Not Set
	if(m_RcpStep[nIdx].nRF_FwSp < 1 && m_RcpStep[nIdx].nRF2_FwSp < 1)		return;

	//////////////////////////////////////////////////////////////////////////
	//... Check RF On Count
	if(TRUE == GetStepRFOnCount(nStep, &nRF1Count, &nRF2Count))
	{	//...Case. RF On Step
		sprintf(szTemp, "CHECK_COUNT %d %d %d", m_RcpHeader.nRecipeType, nRF1Count, nRF2Count);  //...2015.11.25
		RUN_SET_FUNCTION(RF_COUNT_MONITOR, szTemp);

		printf("[RF_COUNT_MONITOR] %s\n", szTemp);
		_LOG_PROC("[RF_COUNT_MONITOR] %s", szTemp);
	}
}

//--------------------------------------------------------------------------------
// 2013.06.25
BOOL CProcessSequence::Start_ContDownloadValveRecipe()
{
	BOOL bRet = TRUE;

	if(THREAD_STOP == m_nContDL_Thread) RunThreadFunc(ContDL_ThreadProc, this);

	return bRet;
}

DWORD WINAPI CProcessSequence::ContDL_ThreadProc(LPVOID lParam)
{
	CProcessSequence *pBaseApp = (CProcessSequence *)lParam;

	pBaseApp->m_nContDL_Thread = THREAD_RUN;
	pBaseApp->ContDownloadValveRecipe();
	pBaseApp->m_nContDL_Thread = THREAD_STOP;

	return 0;
}

BOOL CProcessSequence::ContDownloadValveRecipe()
{
	int i;
	int nDownloadStep;
	int nErrCnt;

	// 2013.08.27
	int nIdx;
	int nNextStep;
	int nNextIdx;
	int nVRCPMode;

	int nIOStatus;

	m_nVRCPBufPage = 0;
	while(THREAD_RUN == m_nContDL_Thread)
	{
		nDownloadStep = 0;

		EnterCriticalSection(&m_csQueueLock);
		for(i=QSTEP_RUN; i>=QSTEP_RD4; i--)
		{
			//2013.11.25
			if(QSTEP_RD4 == i)
			{
				if (1== READ_DIGITAL(VRCP_PauseDI, &nIOStatus)) break;
			}

			// 2013.08.19
			if(m_ProcQ[i].nStep != 0 &&  FALSE == m_ProcQ[i].bDownload)
			{
				m_ProcQ[i].bDownload = TRUE;
				nDownloadStep = m_ProcQ[i].nStep;
				nNextStep = m_ProcQ[i-1].nStep;		// 2013.08.27
				break;
			}
		}
		LeaveCriticalSection(&m_csQueueLock);

		// 2013.08.19
		// _LOG_PROC("-----> i:%d, DownStep:%d", i, nDownloadStep);

		if(0 != nDownloadStep)
		{
			_LOG_PROC("-----=> Valve Recipe Download Started [Step:%d]", nDownloadStep);

			// 2013.08.27
			nVRCPMode = VRM_CONT;
			nIdx = nDownloadStep-1;
			nNextIdx = nNextStep-1;
			if(nNextStep == 0) nVRCPMode = VRM_PAUSE;
			else if(ED_Disable == m_RcpStep[nNextIdx].nVR_Enable)  nVRCPMode = VRM_PAUSE;
			else
			{
				//////////////////////////////////////////////////////////////////////////
				//... 2015.03.21 Add Position Move Option
				//... Next Download Step VRCP_PAUSE 사용 조건 Fix (Next Step 기준)
				if(ZPOSOPT_Wait == m_RcpStep[nNextIdx].nSTGZPOS_WaitOpt)
				{
					//... Check Before Step Wait Option & Position
					if(ZPOSOPT_NoWait == m_RcpStep[nIdx].nSTGZPOS_WaitOpt)
					{	//... Set Pause by Different Wait Option
						nVRCPMode = VRM_PAUSE;
						_LOG_PROC("[INFO] DN_STEP[%d] PAUSE SET by Different WaitOpt[%d][%d]",
							nDownloadStep,
							m_RcpStep[nIdx].nSTGZPOS_WaitOpt, m_RcpStep[nNextIdx].nSTGZPOS_WaitOpt);
					}
					else
					{	//... Set Pause by Different Position
						if( (m_RcpStep[nIdx].dbSTGZPOS_Sp  != m_RcpStep[nNextIdx].dbSTGZPOS_Sp) ||
							(m_RcpStep[nIdx].dbSTGZPOS2_Sp != m_RcpStep[nNextIdx].dbSTGZPOS2_Sp) )
						{
							nVRCPMode = VRM_PAUSE;
							_LOG_PROC("[INFO] DN_STEP[%d] PAUSE SET by Different Pos Cur[%.2f][%.2f] Nxt[%.2f][%.2f]",
								nDownloadStep,
								m_RcpStep[nIdx].dbSTGZPOS_Sp, m_RcpStep[nIdx].dbSTGZPOS2_Sp,
								m_RcpStep[nNextIdx].dbSTGZPOS_Sp, m_RcpStep[nNextIdx].dbSTGZPOS2_Sp);
						}
					}
				}
			}

			m_RcpStep[nIdx].nVRCPMode = nVRCPMode;

			if(FALSE == DownloadProcStepVRCP(nDownloadStep))
			{
				nErrCnt = 0;
				do {
					if(++nErrCnt >= 3)
					{
						ALARM_POST(ALARM_VALVE_RECIPE_DOWNLOAD_FAILED);
						break;
					}
					if(THREAD_RUN != m_nContDL_Thread) break;
					Sleep(100);
					_LOG_ERR("---> Continuous Recipe Download failed & Retry !");
				} while(FALSE == DownloadProcStepVRCP(nDownloadStep, TRUE));
			}
			m_nDnLdStepCount++;
			m_bFirstStepDownload = TRUE;
			_LOG_PROC("-----=> Valve Recipe Download Finished [Step:%d]", nDownloadStep);
		}

		if(THREAD_RUN != m_nContDL_Thread) break;

		Sleep(100);
	}
	return TRUE;
}
//--------------------------------------------------------------------------------
// 2014.05.22
Module_Status CProcessSequence::GetSubFuncStatus(int nStep)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIdx;

	nIdx = nStep-1;
	switch(m_RcpStep[nIdx].nSubFunc)
	{
	case SF_NONE: break;
	case SF_LEAK_CHECK:
		msRet = (Module_Status)READ_FUNCTION(VACUUM_CONTROL);
		break;
	case SF_REFILL_ALL:
		msRet = (Module_Status)READ_FUNCTION(REFILL_CONTROL);
		break;
	}

	return msRet;
}

// 2014.05.22
BOOL CProcessSequence::RunSubFunction(int nStep)
{
	BOOL bRet = TRUE;
	int nIdx;

	nIdx = nStep-1;
	switch(m_RcpStep[nIdx].nSubFunc)
	{
	case SF_NONE: break;
	case SF_LEAK_CHECK:
		if(! WAIT_SECONDS(3)) { bRet = FALSE; break;}
		if(READ_FUNCTION(VACUUM_CONTROL) != SYS_RUNNING)
			RUN_SET_FUNCTION(VACUUM_CONTROL, "LEAK_CHECK");

		break;
	case SF_REFILL_ALL:
		if(! WAIT_SECONDS(3)) { bRet = FALSE; break; }
		if(READ_FUNCTION(REFILL_CONTROL) != SYS_RUNNING)
			RUN_SET_FUNCTION(REFILL_CONTROL, "AUTO_ALL");
		break;


	}
	return bRet;
}

// 2014.05.22
void CProcessSequence::StopSubFunction(int nStep)
{
	int nIdx;

	if(nStep == 0)
	{
		RUN_FUNCTION_ABORT(VACUUM_CONTROL);
		return;
	}

	nIdx = nStep-1;
	switch(m_RcpStep[nIdx].nSubFunc)
	{
	case SF_NONE: break;
	case SF_LEAK_CHECK:
		RUN_FUNCTION_ABORT(VACUUM_CONTROL);
		break;
	case SF_REFILL_ALL:
		RUN_FUNCTION_ABORT(REFILL_CONTROL);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//... 2015.09.28
void CProcessSequence::SetSwapPurge()
{
	int nIOStatus;
	Module_Status msSubRet = SYS_ABORTED;

	if(enOFFON_ON == READ_DIGITAL(PRMD_SwpPug_RcpEnd, &nIOStatus))
	{
		msSubRet = RUN_FUNCTION_FREE(ABORT_GAS, "SWAP_PURGE");
		_LOG_PROC("[INFO] SET SWAP_PURGE[%d]", msSubRet);
	}
}
