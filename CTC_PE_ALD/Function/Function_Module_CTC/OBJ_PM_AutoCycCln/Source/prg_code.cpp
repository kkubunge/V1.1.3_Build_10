///////////////////////////////////////////////////////////////////////////////
// PM Thickness Fault Interlock
// Create : 2014.10.15

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
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

#include "TextLogDll.h"

//////////////////////////////////////////////////////////////////////////
char	g_szModuleName[8] = "PM1";
int		g_nAlarmOffset = 0;		//... 2015.01.06 Add PM Separate Alarm

//////////////////////////////////////////////////////////////////////////
#define FUNC_NAME	"AutoCycleClean"

//////////////////////////////////////////////////////////////////////////
//... Alarm
#define ALARM_PM_IDLE_STOP_FAILED		3450
#define ALARM_PM_IDLE_STOP_TIMEOUT		3451
#define ALARM_PM_PROC_START_TIMEOUT		3452

//////////////////////////////////////////////////////////////////////////
//... Define
#define IDLE_STOP_TIMEOUT		300		// sec
#define PROC_START_TIMEOUT		30		// sec
#define MAX_LOADPORT			3

#define MAX_IDLE_STOP_COUNT		5

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {

	//... Scheduler for PM Disable
	{ "SCHEDULER"				, _K_F_IO	,	FUNC_SCHEDULER,			0	} ,
	{ "Sys.Min"					, _K_D_IO	,	Sys_Min,				0	} ,

	//... PM Status
	{ "CTC.$0_CommStatus"		, _K_D_IO	,	CTC_PM_CommStatus,		0	} ,		// PM Mode Status
	{ "$0.PRO_RunStsDM"			, _K_D_IO	,	PM_PRO_RunStsDM,		0	} ,		// PM Recipe Run Status
	{ "$0.PRO_RcpTypeDM"		, _K_D_IO	,	PM_PRO_RcpTypeDM,		0	} ,		// PM Recipe Type
	{ "$0.PRO_IdleRcpStsDM"		, _K_D_IO	,	PM_PRO_IdleRcpStsDM,	0	} ,		// 0:Stop, 1:Running

	{ "$0.MAINT_CONTROL"		, _K_D_IO	,	PM_MAINT_CONTROL,		0	} ,
	{ "$0.MAIN_CONTROL"			, _K_D_IO	,	PM_MAIN_CONTROL,		0	} ,

	{ "$0.PROCESS_IN"			, _K_F_IO	,	PM_PROCESS_IN,			0	} ,
	{ "$0.PROCESS_OUT_BRG"		, _K_F_IO	,	PM_PROCESS_OUT_BRG,		0	} ,
	{ "$0.PROCESS_OUT"			, _K_F_IO	,	PM_PROCESS_OUT,			0	} ,
	{ "$0.M_PROCESS"			, _K_F_IO	,	PM_M_PROCESS,			0	} ,
	{ "SCHEDULER_MAINT_$0"		, _K_F_IO	,	SCHEDULER_MAINT_PM,		0	} ,

	{ "CTC.$0_RECIPE_FILE"		, _K_S_IO	,	PM_RECIPE_FILE,			0	} ,

	{ "CTC.LPM1_LotPMPath"		, _K_S_IO	,	CTC_LPM1_LotPMPath,		0	} ,
	{ "CTC.LPM2_LotPMPath"		, _K_S_IO	,	CTC_LPM2_LotPMPath,		0	} ,
	{ "CTC.LPM3_LotPMPath"		, _K_S_IO	,	CTC_LPM3_LotPMPath,		0	} ,

	{ "CTC.MAIN_CONTROL"		, _K_D_IO	,	CTC_MAIN_CONTROL,		0	} ,
	{ "CTC.MAIN_CONTROL2"		, _K_D_IO	,	CTC_MAIN_CONTROL2,		0	} ,
	{ "CTC.MAIN_CONTROL3"		, _K_D_IO	,	CTC_MAIN_CONTROL3,		0	} ,

	//... Chamber Condition Control Parameter for Auto Cycle Clean
	{ "PRMD_$0_AutoCycUse"		, _K_D_IO	,	PRMD_PM_AutoCycUse,		0	} ,		// 0:Disable, 1:Enable
	{ "PRMD_$0_AutoCycRun"		, _K_D_IO	,	PRMD_PM_AutoCycRun,		0	} ,		// 0:Idle, 1:Running, 2:Failed
	{ "PRMD_$0_AutoCycTmH"		, _K_D_IO	,	PRMD_PM_AutoCycTmH,		0	} ,		// Cycle Time Hour
	{ "PRMD_$0_AutoCycTmM"		, _K_D_IO	,	PRMD_PM_AutoCycTmM,		0	} ,		// Cycle Time Min
	{ "PRMD_$0_AutoCycStop"		, _K_D_IO	,	PRMD_PM_AutoCycStop,	0	} ,		// 0:Off, 1:On

	{ "PRMS_$0_AutoCycRcp"		, _K_S_IO	,	PRMS_PM_AutoCycRcp,		0	} ,		// Cycle Clean Recipe

	//... Chamber Condition Control Parameter for Reference
	{ "PRMD_$0_IDLE_TimeD"		, _K_D_IO	,	PRMD_PM_IDLE_TimeD,		0	} ,		// Idle Day
	{ "PRMD_$0_IDLE_TimeH"		, _K_D_IO	,	PRMD_PM_IDLE_TimeH,		0	} ,		// Idle Hour
	{ "PRMD_$0_IDLE_TimeM"		, _K_D_IO	,	PRMD_PM_IDLE_TimeM,		0	} ,		// Idle Min

	{ "PRMD_$0_Leaf_RunSt_05"	, _K_D_IO	,	PRMD_PM_Leaf_RunSt_05,	0	} ,		// Idle Running Sts (0:Stop, 1:Run, 2:Fail)
	{ "CHAMBER_CON_ABORT_$0"	, _K_F_IO	,	CHAMBER_CON_ABORT_PM,	0	} ,

	//... 2015.01.05 Add Auto System Leak Check Condition
	{ "CTC.AutoLeakCheckSts"	, _K_D_IO	,	CTC_AutoLeakCheckSts,	0	} ,

	//... 2015.04.18 Add Check PM Wafer
	{ "CTC.$0_Wafer_Status"		, _K_D_IO	,	CTC_PM_Wafer_Status,	0	} ,
	{ "CTC.$0_Wafer_Status2"	, _K_D_IO	,	CTC_PM_Wafer_Status2,	0	} ,
	//... SSP 진행 시 Auto Cyc Clean 진행 하지 않음 
	{ "$0_PRO_ClnStsDM"			, _K_D_IO	,	PRO_ClnStsDM,			0	} ,

	""
};

//////////////////////////////////////////////////////////////////////////
//... Define Enum
enum { 
	PMCOMM_UNUSE,
	PMCOMM_LOCAL,
	PMCOMM_DISCONN,
	PMCOMM_STANDBY,
	PMCOMM_MAINT,
	PMCOMM_UNINIT,
	PMCOMM_ENABLEPM,
	PMCOMM_CTCINUSE,
	PMCOMM_CTCDIS,
	PMCOMM_CTCDIS2,
	PMCOMM_CTCENAPM,
	PMCOMM_DISHW,
	PMCOMM_DISHW2
};

enum { 
	enOPT_DISABLE,
	enOPT_ENABLE
};

enum { 
	enPRO_STOP,
	enPRO_RUNNING
};

enum { 
	enRCPTYPE_UNKNOWN,
	enRCPTYPE_PROC,
	enRCPTYPE_PREPROC,
	enRCPTYPE_PSTPROC,
	enRCPTYPE_IDLE,
	enRCPTYPE_CLEAN,
	enRCPTYPE_PURGE,
	enRCPTYPE_LOTPRE,
	enRCPTYPE_LOTPST
};

enum { 
	enMAIN_CTR_IDLE,
	enMAIN_CTR_RUN,
	enMAIN_CTR_PAUSE,
	enMAIN_CTR_PAUSING,
	enMAIN_CTR_PAUSED,
	enMAIN_CTR_ABORTING,
	enMAIN_CTR_DISABLE
};

enum { 
	enMAINT_CTR_IDLE,
	enMAINT_CTR_RUN,
	enMAINT_CTR_ENABLE
};

enum { 
	enSTS_STOPPED,
	enSTS_RUNNING,
	enSTS_FAILED,
	enSTS_RESERVE	//... 2015.04.18 Add Dry Clean Reserve for Idle Timer Hold
};

enum { 
	enOFFON_OFF,
	enOFFON_ON
};

enum { 
	enSTOP_IDLE,
	enSTOP_READY,
	enSTOP_STOPING
};

enum { 
	enLOT_IDLE,
	enLOT_RUNNING,
	enLOT_PAUSING,
	enLOT_PAUSED,
	enLOT_ABORTING,
	enLOT_DISABLE,
	enLOT_WAITING,
	enLOT_WAIT_HANDOFF
};

//... 2015.01.05 Add Auto System Leak Check Condition
enum {
	enSYSLC_IDLE,
	enSYSLC_RESERVE,
	enSYSLC_RUNNING
};

//////////////////////////////////////////////////////////////////////////
BOOL IsPMEnable()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;

	if(PMCOMM_STANDBY == READ_DIGITAL(CTC_PM_CommStatus, &nIOStatus))	bRet = TRUE;

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL AbsentPMWafer()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;

	do 
	{
		if(0 < READ_DIGITAL(CTC_PM_Wafer_Status, &nIOStatus))		break;
		if(0 < READ_DIGITAL(CTC_PM_Wafer_Status2, &nIOStatus))		break;

		bRet = TRUE;
	} while (0);
	
	return	bRet;
}


//////////////////////////////////////////////////////////////////////////
BOOL IsReachedAutoCycleTime()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	int		nData			= 0;
	int		nCfgTimeMin		= 0;
	int		nProgTimeMin	= 0;

	do 
	{
		//... Read Auto Cycle Parameter
		if(enOPT_DISABLE == READ_DIGITAL(PRMD_PM_AutoCycUse, &nIOStatus))		break;		//... NotUse

		nData = READ_DIGITAL(PRMD_PM_AutoCycTmH, &nIOStatus);		nCfgTimeMin  = nData * 60;		// Hour -> Min
		nData = READ_DIGITAL(PRMD_PM_AutoCycTmM, &nIOStatus);		nCfgTimeMin += nData;
		if(nCfgTimeMin <= 0)				break;		//... NotUse

		//... Read Current Time
		nData = READ_DIGITAL(PRMD_PM_IDLE_TimeD, &nIOStatus);		nProgTimeMin  = nData * 60 * 24;	// Day -> Min
		nData = READ_DIGITAL(PRMD_PM_IDLE_TimeH, &nIOStatus);		nProgTimeMin += nData * 60;		// Hour -> Min
		nData = READ_DIGITAL(PRMD_PM_IDLE_TimeM, &nIOStatus);		nProgTimeMin += nData;
		if(nCfgTimeMin > nProgTimeMin)		break;		//... Not Case

		bRet = TRUE;
	} while(0);
		
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsPMRecipeRun()
{
	BOOL	bRet = TRUE;

	do 
	{
		//
		if(SYS_RUNNING == READ_FUNCTION(PM_PROCESS_IN))						break;
		if(SYS_RUNNING == READ_FUNCTION(PM_PROCESS_OUT_BRG))				break;
		if(SYS_RUNNING == READ_FUNCTION(PM_PROCESS_OUT))					break;
		if(SYS_RUNNING == READ_FUNCTION(PM_M_PROCESS))						break;
		
		bRet = FALSE;
	} while(0);


	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsIdleRecipe()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;

	if( enPRO_RUNNING  == READ_DIGITAL(PM_PRO_IdleRcpStsDM, &nIOStatus) && 
		enRCPTYPE_IDLE == READ_DIGITAL(PM_PRO_RcpTypeDM, &nIOStatus))
	{
		bRet = TRUE;
	}

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsPMRecipeStop()
{
	BOOL	bRet = FALSE;
	
	do 
	{
		//
		if(SYS_RUNNING == READ_FUNCTION(PM_PROCESS_IN))						break;
		if(SYS_RUNNING == READ_FUNCTION(PM_PROCESS_OUT_BRG))				break;
		if(SYS_RUNNING == READ_FUNCTION(PM_PROCESS_OUT))					break;
		if(SYS_RUNNING == READ_FUNCTION(PM_M_PROCESS))						break;
		
		bRet = TRUE;
	} while(0);
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsPMMaintIdle()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	
	do 
	{
		if(enMAINT_CTR_ENABLE != READ_DIGITAL(PM_MAINT_CONTROL, &nIOStatus))		break;	//... PM Standby
		if(enMAIN_CTR_IDLE    != READ_DIGITAL(PM_MAIN_CONTROL, &nIOStatus))			break;
		
		bRet = TRUE;
	} while(0);
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsRunSystemLeakCheck()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	
	if(enSYSLC_IDLE != READ_DIGITAL(CTC_AutoLeakCheckSts, &nIOStatus))		bRet = TRUE;	
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsValidPMConditionforAutoCycle()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;

	//... PM Standby (Idle Recipe Running or Recipe Stop)
	do 
	{
		//...2019.03.21 SSP 진행 시
		if(0 !=READ_DIGITAL(PRO_ClnStsDM,&nIOStatus))break;

		//... 2015.01.05 Case Add Auto System Leak Check (Fixed PosItion)
		if(IsRunSystemLeakCheck())				break;		//... Invalid Condition

		//... Idle Recipe Running
		if(IsPMRecipeRun() && IsIdleRecipe())	{	bRet = TRUE;	break;	}
		
		//... Idle Maint Control
		if(IsPMRecipeStop() && IsPMMaintIdle())	{	bRet = TRUE;	break;	}

	} while(0);
		
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL StopIdleRecipe()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	
	//... 2015.04.04 Add Idle Stop Retry
	Module_Status msRet = SYS_SUCCESS;
	char	szParam[256] = "";
	int		i = 0;

	do 
	{
		if(enSTS_RUNNING == READ_DIGITAL(PRMD_PM_Leaf_RunSt_05, &nIOStatus))
		{	//... Case. Running Idle in Chamber Condition Control
			//... 2015.04.04 Add Idle Stop Retry
			for(i = 0; i < MAX_IDLE_STOP_COUNT; i++)
			{
				//... 1. Normal Abort "%s"
				memset(szParam, 0, sizeof(szParam));
				sprintf(szParam, "%s", g_szModuleName);
				//... 2015.04.17  Modify Function Abort Control for System All Abort
				msRet = RUN_FUNCTION_FREE(CHAMBER_CON_ABORT_PM, szParam);
				printf("[INFO] %s Idle Stop %s[%d] for AutoCycleClean...\n", g_szModuleName, szParam, msRet);
				_LOG("[INFO] %s Idle Stop %s[%d]", g_szModuleName, szParam, msRet);
				
				Sleep(1000);
				
				//... 2. Wait Until Abort "WAIT_TILL_RUN_STOPPED_%s"
				memset(szParam, 0, sizeof(szParam));
				sprintf(szParam, "WAIT_TILL_RUN_STOPPED_%s", g_szModuleName);
				//... 2015.04.17  Modify Function Abort Control for System All Abort
				msRet = RUN_FUNCTION_FREE(CHAMBER_CON_ABORT_PM, szParam);
				printf("[INFO] %s Idle Stop %s[%d] for AutoCycleClean...\n", g_szModuleName, szParam, msRet);
				_LOG("[INFO] %s Idle Stop %s[%d]", g_szModuleName, szParam, msRet);
				
				//... Completed Idle Recipe Stopped
				if(SYS_SUCCESS == msRet)	{	bRet = TRUE;	break;	}
				
				Sleep(1000);
			}

			//... Final Check
			if(SYS_SUCCESS != msRet || enPRO_RUNNING == READ_DIGITAL(PM_PRO_IdleRcpStsDM,&nIOStatus))
			{
				WRITE_DIGITAL(PM_PRO_IdleRcpStsDM, enPRO_STOP, &nIOStatus);
				printf("[INFO] %s Idle Stop PRO_IdleRcpStsDM OFF[%d] for AutoCycleClean...\n", g_szModuleName, nIOStatus);
				_LOG("[INFO] %s Idle Stop PRO_IdleRcpStsDM OFF[%d]", g_szModuleName, nIOStatus);
			}
		}
		else if(enPRO_RUNNING == READ_DIGITAL(PM_PRO_IdleRcpStsDM,&nIOStatus))
		{	//... Case. Running Idle in Manual
			WRITE_DIGITAL(PM_PRO_IdleRcpStsDM, enPRO_STOP, &nIOStatus);
		}
		else
		{
			ALARM_POST(ALARM_PM_IDLE_STOP_FAILED + g_nAlarmOffset);			//... 2015.01.06 Add PM Separate Alarm
			printf("[ERR] %s Invalid Idle Case for AutoCycleClean...\n", g_szModuleName);
			_LOG("[ERR] %s Invalid Idle Case", g_szModuleName);
			break;
		}

		bRet = TRUE;
	} while (0);

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL WaitIdleRecipeStop()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	int		nTimeout = 0;
	
	do 
	{
		//... 2015.04.17  Modify Function Abort Control for System All Abort
		if(enSTOP_STOPING == READ_DIGITAL(PRMD_PM_AutoCycStop, &nIOStatus))		break;
		Sleep(1000);

		//... Case Idle Stopped
		if(IsPMRecipeStop())
		{
			bRet = TRUE;
			break;
		}

		//... Wait
		if(++nTimeout >= IDLE_STOP_TIMEOUT)
		{
			ALARM_POST(ALARM_PM_IDLE_STOP_TIMEOUT + g_nAlarmOffset);		//... 2015.01.06 Add PM Separate Alarm
			break;
		}
		
	} while (1);
	
	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL WaitAndStartAutoCycCleanRcp()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	char	szPMRecipe[256] = "";

	int		nTimeout = 0;
	
	do 
	{
		//... Wait
		if(SYS_RUNNING == READ_FUNCTION(SCHEDULER_MAINT_PM))
		{
			//... 2015.04.17  Modify Function Abort Control for System All Abort
			if(enSTOP_STOPING == READ_DIGITAL(PRMD_PM_AutoCycStop, &nIOStatus))		break;
			Sleep(1000);

			if(++nTimeout >= PROC_START_TIMEOUT)
			{
				ALARM_POST(ALARM_PM_PROC_START_TIMEOUT + g_nAlarmOffset);	//... 2015.01.06 Add PM Separate Alarm
				break;
			}
			
			continue;	//... Wait
		}
		//...2016.04.01 Protect Double Recipe Running (LotPre & Clean)
		if(! IsPMEnable()) {bRet = TRUE; break;}

		//...
		READ_STRING(PRMS_PM_AutoCycRcp, szPMRecipe, &nIOStatus);
		WRITE_STRING(PM_RECIPE_FILE, szPMRecipe, &nIOStatus);
		RUN_SET_FUNCTION_FREE(SCHEDULER_MAINT_PM, "PROCESS");

		//... 2015.04.17  Modify Function Abort Control for System All Abort
		if(enSTOP_STOPING == READ_DIGITAL(PRMD_PM_AutoCycStop, &nIOStatus))		break;
		Sleep(2000);		//... Started Interval

		bRet = TRUE;
		break;

	} while (1);

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL WaitFinishedAutoCycCleanRcp()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	Module_Status msRet = SYS_RUNNING;

	do 
	{
		//... 2015.04.17  Modify Function Abort Control for System All Abort
		if(enSTOP_STOPING == READ_DIGITAL(PRMD_PM_AutoCycStop, &nIOStatus))		break;
		Sleep(1000);		//... Started Interval

		//...
		msRet = (Module_Status) READ_FUNCTION(SCHEDULER_MAINT_PM);

		if(enSTOP_STOPING == READ_DIGITAL(PRMD_PM_AutoCycStop, &nIOStatus) && SYS_RUNNING == msRet)
		{	//... Case. Auto Cycle Stop by User	
			RUN_FUNCTION_ABORT(SCHEDULER_MAINT_PM);
			msRet = SYS_ABORTED;
			break;
		}

		if(SYS_RUNNING != msRet)	break;		//... Waiting Process Idle

	} while (1);

	if(SYS_SUCCESS == msRet)		bRet = TRUE;

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsLotStarted()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	int		nLotStatus = enLOT_IDLE;
	char	szLotPMPath[256], szTemp[31], szTemp2[31], szTemp3[31];

	//...
	memset(szLotPMPath, 0, sizeof(szLotPMPath));
	memset(szTemp,		0, sizeof(szTemp));
	memset(szTemp2,		0, sizeof(szTemp2));
	memset(szTemp3,		0, sizeof(szTemp3));

	for(int i = 0; i < MAX_LOADPORT; i++)
	{
		nLotStatus = READ_DIGITAL(CTC_MAIN_CONTROL + i, &nIOStatus);
		if( enLOT_RUNNING == nLotStatus || enLOT_PAUSING == nLotStatus ||
			enLOT_PAUSED == nLotStatus /* || enLOT_WAITING == nLotStatus */)
		{
			//...
			memset(szLotPMPath, 0, sizeof(szLotPMPath));
			memset(szTemp,		0, sizeof(szTemp));
			memset(szTemp2,		0, sizeof(szTemp2));
			memset(szTemp3,		0, sizeof(szTemp3));

			//...
			READ_STRING(CTC_LPM1_LotPMPath + i, szLotPMPath, &nIOStatus);
			if(strlen(szLotPMPath) > 1)
			{
				STR_SEPERATE_CHAR(szLotPMPath, '|', szTemp, szTemp3, 30);
				if(STRCMP_L(szTemp, g_szModuleName))	{	bRet = TRUE;	break; }
				//
				if(strlen(szTemp3) < 3)		continue;
				STR_SEPERATE_CHAR(szTemp3, '|', szTemp, szTemp2, 30);
				if(STRCMP_L(szTemp, g_szModuleName))	{	bRet = TRUE;	break; }
				//
				if(strlen(szTemp2) < 3)		continue;
				STR_SEPERATE_CHAR(szTemp2, '|', szTemp, szTemp3, 30);
				if(STRCMP_L(szTemp, g_szModuleName))	{	bRet = TRUE;	break; }
			}
		}
	}

	printf("[INFO] %s IsLotStarted PM Path[%s] ... Started[%d]\n", g_szModuleName, szLotPMPath, bRet);
	_LOG("[INFO] %s IsLotStarted PM Path[%s] ... Started[%d]", g_szModuleName, szLotPMPath, bRet);

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
void ChangeMode_Disable()
{
	char	szEvent[256] = "";
	
	sprintf(szEvent, "SET_MODULE_INFO_LOCAL %s|DISABLE", g_szModuleName);
	WRITE_FUNCTION_EVENT(FUNC_SCHEDULER, szEvent);
	
	printf("[INFO] %s Mode Change Disable for AutoCycleClean...\n", g_szModuleName);
	_LOG("[INFO] %s Mode Change Disable", g_szModuleName);
	
	//...
	Sleep(2000);	// Mode Change Interval
}

//////////////////////////////////////////////////////////////////////////
BOOL IsUserAborted()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;

	if(enSTOP_STOPING == READ_DIGITAL(PRMD_PM_AutoCycStop, &nIOStatus))		bRet = TRUE;

	return	bRet;
}


//////////////////////////////////////////////////////////////////////////
void StartMonitor()
{
	int		nIOStatus;
	int		i = 0;
	int		nOldSysMin	= 0;
	int		nCurSysMin	= 0;
	BOOL	bIdleRun	= FALSE;
	BOOL	bRet		= FALSE;

	int		nAutoClnSts = 0;

	Sleep(5000);

	_LOG("Started StartMonitor...[%s]", g_szModuleName);

	//... Check Interval 1 min
	while(1)
	{
		//... Init Run Flag IO
		nAutoClnSts = READ_DIGITAL(PRMD_PM_AutoCycRun, &nIOStatus);
		if(enSTS_RUNNING == nAutoClnSts || enSTS_RESERVE == nAutoClnSts)
			WRITE_DIGITAL(PRMD_PM_AutoCycRun, enSTS_STOPPED, &nIOStatus);
		
		//... Init Stop Flag IO
		if(enSTOP_IDLE != READ_DIGITAL(PRMD_PM_AutoCycStop, &nIOStatus))
			WRITE_DIGITAL(PRMD_PM_AutoCycStop, enSTOP_IDLE, &nIOStatus);

		Sleep(3000);

		//... Check Reached Interval Period (min)
		nCurSysMin = READ_DIGITAL(Sys_Min, &nIOStatus);
		if(nCurSysMin == nOldSysMin)		continue;

		nOldSysMin = nCurSysMin;

		//////////////////////////////////////////////////////////////////////////
		//... Check PM CommStatus
		if(! IsPMEnable())					continue;		//... Not Condition

		//////////////////////////////////////////////////////////////////////////
		//... Check PM Wafer Absent
		if(! AbsentPMWafer())				continue;		//... Not Condition

		//////////////////////////////////////////////////////////////////////////
		//... Check Auto Cycle Clean Period
		if(! IsReachedAutoCycleTime())		continue;		//... Not Condition
		printf("[INFO] %s IsReachedAutoCycleTime...OK \n", g_szModuleName);
		_LOG("[INFO] %s IsReachedAutoCycleTime...OK", g_szModuleName);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//... Check Idle Recipe Before Auto Cycle Clean
		bIdleRun	= FALSE;
		bRet		= TRUE;
		printf("[INFO] %s Auto Cycle Started...\n", g_szModuleName);
		_LOG("[INFO] %s Auto Cycle Started", g_szModuleName);

		if(IsPMRecipeRun())
		{
			if(IsIdleRecipe())		bIdleRun = TRUE;
			else					continue;				//... Other Recipe Invalid Condition
		}

		printf("[INFO] %s IsPMRecipeRun...OK [IdleRun:%d] \n", g_szModuleName, bIdleRun);
		_LOG("[INFO] %s IsPMRecipeRun...OK [IdleRun:%d]", g_szModuleName, bIdleRun);

		//////////////////////////////////////////////////////////////////////////
		//... Check Condition for Auto Cycle Clean
		if(! IsValidPMConditionforAutoCycle())		continue;
		printf("[INFO] %s IsValidPMConditionforAutoCycle...OK\n", g_szModuleName);
		_LOG("[INFO] %s IsValidPMConditionforAutoCycle...OK", g_szModuleName);

		//////////////////////////////////////////////////////////////////////////
		//... Change Mode Enable PM for Auto Cycle Clean
		WRITE_DIGITAL(PRMD_PM_AutoCycStop, enSTOP_READY, &nIOStatus);
		WRITE_DIGITAL(PRMD_PM_AutoCycRun, enSTS_RESERVE, &nIOStatus);	//... 2015.04.18 Add Dry Clean Reserve for Idle Timer Hold

		//////////////////////////////////////////////////////////////////////////
		//... Stop Idle Recipe & Wait
		if(bIdleRun)
		{
			//... Idle Recipe Stop
			if(! StopIdleRecipe())		continue;
			printf("[INFO] %s StopIdleRecipe...OK\n", g_szModuleName);
			_LOG("[INFO] %s StopIdleRecipe...OK", g_szModuleName);

			//... Wait for Recipe Stop
			if(! WaitIdleRecipeStop())	continue;
			printf("[INFO] %s WaitIdleRecipeStop...OK\n", g_szModuleName);
			_LOG("[INFO] %s WaitIdleRecipeStop...OK", g_szModuleName);
		}

		//////////////////////////////////////////////////////////////////////////
		//... Auto Cycle Stop by User
		if(IsUserAborted())				{	ChangeMode_Disable();	continue;	}

		//////////////////////////////////////////////////////////////////////////
		//... Check Lot Started
		if(IsLotStarted())				continue;
		printf("[INFO] %s IsLotStarted...OK\n", g_szModuleName);
		_LOG("[INFO] %s IsLotStarted...OK", g_szModuleName);

		//////////////////////////////////////////////////////////////////////////
		//... Check PM CommStatus
		if(! IsPMEnable())					continue;		//... Not Condition

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//... Start Auto Cycle Clean
		WRITE_DIGITAL(PRMD_PM_AutoCycRun, enSTS_RUNNING, &nIOStatus);	//... 2015.04.18 Add Dry Clean Reserve for Idle Timer Hold

		if(! WaitAndStartAutoCycCleanRcp())		bRet = FALSE;

		//////////////////////////////////////////////////////////////////////////
		//...2016.04.01 Protect Double Recipe Running (LotPre & Clean)
		if(! IsPMEnable())					continue;		//... Not Condition
		//////////////////////////////////////////////////////////////////////////	
		
		printf("[INFO] %s WaitAndStartAutoCycCleanRcp...OK\n", g_szModuleName);
		_LOG("[INFO] %s WaitAndStartAutoCycCleanRcp...OK", g_szModuleName);

		// Auto Cycle Stop by User or Clean Start Failed
		if(IsUserAborted() || FALSE == bRet)
		{
			if(SYS_RUNNING == READ_FUNCTION(SCHEDULER_MAINT_PM))
			{	//... Before Maint or Clean Recipe
				RUN_FUNCTION_ABORT(SCHEDULER_MAINT_PM);
			}
			ChangeMode_Disable();
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Wait Auto Cycle Clean Finished
		if(! WaitFinishedAutoCycCleanRcp())		bRet = FALSE;
		printf("[INFO] %s WaitFinishedAutoCycCleanRcp...OK\n", g_szModuleName);
		_LOG("[INFO] %s WaitFinishedAutoCycCleanRcp...OK", g_szModuleName);

		//////////////////////////////////////////////////////////////////////////
		//...
		if(bRet)
		{
			WRITE_DIGITAL(PRMD_PM_AutoCycRun, enSTS_STOPPED, &nIOStatus);
		}
		else
		{
			WRITE_DIGITAL(PRMD_PM_AutoCycRun, enSTS_FAILED, &nIOStatus);
			ChangeMode_Disable();
			printf("[WARN] %s ChangeMode_Disable...OK\n", g_szModuleName);
			_LOG("[WARN] %s ChangeMode_Disable...OK", g_szModuleName);
		}
	}
}

//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;

	printf("Entered %s_%s\n", g_szModuleName, FUNC_NAME);

	StartMonitor();

	printf("Leave %s_%s\n", g_szModuleName, FUNC_NAME);

	return msRet;
}

//////////////////////////////////////////////////////////////////////////
void Program_Enter_Code() {
	char szTemp[8] = "";
	
	strcpy(szTemp, PROGRAM_ARGUMENT_READ());
	
	if(strlen(szTemp) > 1)		strcpy(g_szModuleName, szTemp);
	REPLACE_CHAR_SET(0, g_szModuleName);
	
	//... 2015.01.06 Add PM Separate Alarm
	if     (strcmp(g_szModuleName, "PM1") == 0)		g_nAlarmOffset = 0;
	else if(strcmp(g_szModuleName, "PM2") == 0)		g_nAlarmOffset = 10;
	else if(strcmp(g_szModuleName, "PM3") == 0)		g_nAlarmOffset = 20;
	else											g_nAlarmOffset = 0;

	printf("%s Auto Cycle Clean Function Created... AlmOffset[%d]\n", g_szModuleName, g_nAlarmOffset);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		_gCLOSE_LOG();
	}
}

} // End of [extern "C"]
//==========================================================================================


