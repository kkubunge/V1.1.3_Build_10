///////////////////////////////////////////////////////////////////////////////
// AbortProcess

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include "Timer.h"

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

//------------------------------------------------------------------------------------------

#define FUNC_NAME	"LotPreSkipTimer"

//------------------------------------------------------------------------------------------
// Alarm Definition(  )

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "PRO_CurIdleTimeAM"	,	_K_A_IO	,	PRO_CurIdleTimeAM,	0 } ,
	{ "PRMA_TO_LotPreSkip"	,	_K_A_IO	,	PRMA_TO_LotPreSkip,	0 } ,
	{ "PRO_LotPreSkipDM"	,	_K_D_IO	,	PRO_LotPreSkipDM,	0 } ,

	""
};

//------------------------------------------------------------------------------------------

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Process Time Function
enum { enOFFON_OFF, enOFFON_ON };

double	g_dbCurIdleTime		= 0;
double	g_dbLotPreSkipTime	= 0;
int		g_nLotPreSkipFlag	= enOFFON_OFF;

int g_nTimer_Thread = THREAD_STOP;
CTimer g_tmr;

enum { TMR_STOP, TMR_RUNNING, TMR_PAUSED };
int g_nTimerStatus = TMR_STOP;

//////////////////////////////////////////////////////////////////////////
//... 
void Timer_Thread()
{
	enum { TYPE_MILLISECOND, TYPE_SECOND, TYPE_MINUTE, TYPE_HOUR, TYPE_DAY };

	int nIOStatus;
	BOOL bRslt = TRUE;

	g_tmr.StartTimer();

	g_nLotPreSkipFlag = enOFFON_ON;
	WRITE_DIGITAL(PRO_LotPreSkipDM, g_nLotPreSkipFlag, &nIOStatus);

	while(g_nTimerStatus != TMR_STOP)
	{
		Sleep(300);

		//... Update Idle Timer
		g_dbCurIdleTime	= (double) g_tmr.GetProgressTime(TYPE_SECOND);
		WRITE_ANALOG(PRO_CurIdleTimeAM, g_dbCurIdleTime, &nIOStatus);

		//... Check Skip Timer
		g_dbLotPreSkipTime = READ_ANALOG(PRMA_TO_LotPreSkip, &nIOStatus);
		if(g_dbCurIdleTime >= g_dbLotPreSkipTime)		break;
	}
	
	//...
	g_nLotPreSkipFlag = enOFFON_OFF;
	WRITE_DIGITAL(PRO_LotPreSkipDM, g_nLotPreSkipFlag, &nIOStatus);
	
	g_nTimer_Thread = THREAD_STOP;
	g_nTimerStatus = TMR_STOP;

	g_tmr.StopTimer();

	_endthread();
}

//////////////////////////////////////////////////////////////////////////
Module_Status StartTimer()
{
	Module_Status msRet = SYS_ERROR;

	if(g_nTimer_Thread != THREAD_RUN)
	{
		g_nTimer_Thread = THREAD_RUN;
		g_nTimerStatus = TMR_RUNNING;
		//...
		_beginthread((_THREAD_FUNC) Timer_Thread, 0, NULL);
		//...
		msRet = SYS_SUCCESS;
	}

	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status StopTimer()
{
	g_nTimerStatus = TMR_STOP;

	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CheckTimer()
{
	Module_Status msRet = SYS_ERROR;
	
	//... Case. Lot Pre Skip
	if(enOFFON_ON == g_nLotPreSkipFlag)		msRet = SYS_SUCCESS;	//... Lot Pre Skip
	
	g_nTimerStatus = TMR_STOP;

	return msRet;
}

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int		nParLen = 0;
	char	*pParam;
	char	szCmd[41], szSubParam[256];

	printf("Entered %s\n", FUNC_NAME);

	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		STR_SEPERATE(pParam , szCmd, szSubParam, 255);

		if     (strcmp(szCmd, "TIMER_START") == 0)		msRet = StartTimer();
		else if(strcmp(szCmd, "TIMER_STOP") == 0)		msRet = StopTimer();
		else if(strcmp(szCmd, "TIMER_CHECK") == 0)		msRet = CheckTimer();
		else
		{
			printf("Invalid Parameter in %s\n", FUNC_NAME);
			msRet = SYS_ERROR;
		}

	} while(0);

	printf("Leaved %s[%d]\n", FUNC_NAME, msRet);

	return msRet;
}

} // End of [extern "C"]
//==========================================================================================
