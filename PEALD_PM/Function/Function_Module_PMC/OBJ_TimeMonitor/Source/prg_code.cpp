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

#define FUNC_NAME	"TimeMonitor"

//------------------------------------------------------------------------------------------
// Alarm Definition(  )

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "Tick_MinDM"		, _K_D_IO	,	Tick_MinDM,	0	} ,
	{ "Tick_10MinDM"	, _K_D_IO	,	Tick_10MinDM,	0	} ,
	{ "Tick_30MinDM"	, _K_D_IO	,	Tick_30MinDM,	0	} ,
	{ "Tick_HourDM"		, _K_D_IO	,	Tick_HourDM,	0	} ,
	{ "Tick_2HourDM"	, _K_D_IO	,	Tick_2HourDM,	0	} ,

	{ "PRO_TotalTimeAM"	, _K_A_IO	,	PRO_TotalTimeAM,	0	} ,
	{ "PRO_CurrTimeAM"	, _K_A_IO	,	PRO_CurrTimeAM,	0	} ,
	{ "PRO_ProgressDM"	, _K_D_IO	,	PRO_ProgressDM,	0	} ,
	{ "ProTimeLmtDM"	, _K_D_IO	,	ProTimeLmtDM,	0	} ,
	""
};

//------------------------------------------------------------------------------------------

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Time Tick Function

int g_nTick_Thread = THREAD_STOP;

void TimeTick_Thread()
{
	int nIOStatus;

	CTimer tmr_Min;
	CTimer tmr_10Min;
	CTimer tmr_30Min;
	CTimer tmr_Hour;
	CTimer tmr_2Hour;

	tmr_Min.SetPeriod(60 * 1000);
	tmr_10Min.SetPeriod(10 * 60 * 1000);
	tmr_30Min.SetPeriod(30 * 60 * 1000);
	tmr_Hour.SetPeriod(60 * 60 * 1000);
	tmr_2Hour.SetPeriod(120 * 60 * 1000);

	tmr_Min.StartTimer();
	tmr_10Min.StartTimer();
	tmr_30Min.StartTimer();
	tmr_Hour.StartTimer();
	tmr_2Hour.StartTimer();

	while(1)
	{
		if(tmr_Min.GetRestTime() < 0)
		{
			if(READ_DIGITAL(Tick_MinDM, &nIOStatus) == 0)
			{
				tmr_Min.StartTimer();
				WRITE_DIGITAL(Tick_MinDM, 1, &nIOStatus);
				WAIT_SECONDS(3);
				WRITE_DIGITAL(Tick_MinDM, 0, &nIOStatus);
			}
		}

		if(tmr_10Min.GetRestTime() < 0)
		{
			if(READ_DIGITAL(Tick_10MinDM, &nIOStatus) == 0)
			{
				tmr_10Min.StartTimer();
				WRITE_DIGITAL(Tick_10MinDM, 1, &nIOStatus);
				WAIT_SECONDS(3);
				WRITE_DIGITAL(Tick_10MinDM, 0, &nIOStatus);
			}
		}

		if(tmr_30Min.GetRestTime() < 0)
		{
			if(READ_DIGITAL(Tick_30MinDM, &nIOStatus) == 0)
			{
				tmr_30Min.StartTimer();
				WRITE_DIGITAL(Tick_30MinDM, 1, &nIOStatus);
				WAIT_SECONDS(3);
				WRITE_DIGITAL(Tick_30MinDM, 0, &nIOStatus);
			}
		}

		if(tmr_Hour.GetRestTime() < 0)
		{
			if(READ_DIGITAL(Tick_HourDM, &nIOStatus) == 0)
			{
				tmr_Hour.StartTimer();
				WRITE_DIGITAL(Tick_HourDM, 1, &nIOStatus);
				WAIT_SECONDS(3);
				WRITE_DIGITAL(Tick_HourDM, 0, &nIOStatus);
			}
		}

		if(tmr_2Hour.GetRestTime() < 0)
		{
			if(READ_DIGITAL(Tick_2HourDM, &nIOStatus) == 0)
			{
				tmr_Hour.StartTimer();
				WRITE_DIGITAL(Tick_2HourDM, 1, &nIOStatus);
				WAIT_SECONDS(3);
				WRITE_DIGITAL(Tick_2HourDM, 0, &nIOStatus);
			}
		}

		WAIT_SECONDS(10);
	}

	g_nTick_Thread = THREAD_STOP;
	_endthread();
}	

void StartTimeTick()
{
	if(g_nTick_Thread != THREAD_RUN)
	{
		g_nTick_Thread = THREAD_RUN;
		_beginthread((_THREAD_FUNC) TimeTick_Thread, 0, NULL);
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Process Time Function

double g_dbCurrTime;
double g_dbTotalTime;
int g_nProgress;
int g_nProTimeLmtDM;
int g_nTimer_Thread = THREAD_STOP;
CTimer g_tmr;

enum { TMR_STOP, TMR_RUNNING, TMR_PAUSED };
int g_nTimerStatus = TMR_STOP;

void Timer_Thread()
{
	int nIOStatus;
	BOOL bRslt = TRUE;

	g_tmr.StartTimer();
	while( g_nTimerStatus != TMR_STOP )
	{
		WAIT_SECONDS(0.3);
		g_dbCurrTime = (double) g_tmr.GetProgressTime() / 1000;
		if(g_nProTimeLmtDM == 1)		// 0:Off 1:On
		{	
			if(g_dbCurrTime > g_dbTotalTime) g_dbCurrTime = g_dbTotalTime;
		}
		WRITE_ANALOG(PRO_CurrTimeAM, g_dbCurrTime, &nIOStatus);

		if(g_dbTotalTime > 0)
		{
			g_nProgress = (int) (g_dbCurrTime / g_dbTotalTime * 100);
			if(g_nProgress > 100) g_nProgress = 100;
		}
		WRITE_DIGITAL(PRO_ProgressDM, g_nProgress, &nIOStatus);

		if(g_nTimerStatus == TMR_PAUSED)
		{
		}
	}
	g_nTimer_Thread = THREAD_STOP;
	g_nTimerStatus = TMR_STOP;
	_endthread();
}

void InitTimer()
{
	int nIOStatus;
	g_nTimerStatus = TMR_STOP;
	WRITE_ANALOG(PRO_CurrTimeAM, 0, &nIOStatus);
	WRITE_DIGITAL(PRO_ProgressDM, 0, &nIOStatus);

	g_dbTotalTime = READ_ANALOG(PRO_TotalTimeAM, &nIOStatus);
	g_dbCurrTime = 0;
	WRITE_ANALOG(PRO_CurrTimeAM, g_dbCurrTime, &nIOStatus);
	g_nProgress = 0;
	WRITE_DIGITAL(PRO_ProgressDM, g_nProgress, &nIOStatus);
	g_nProTimeLmtDM = READ_DIGITAL(ProTimeLmtDM, &nIOStatus); // 0:Off 1:On
}

void StartTimer()
{
	if(g_nTimer_Thread != THREAD_RUN)
	{
		g_nTimer_Thread = THREAD_RUN;
		g_nTimerStatus = TMR_RUNNING;
		_beginthread((_THREAD_FUNC) Timer_Thread, 0, NULL);
	}
}

void StopTimer()
{
	g_nTimerStatus = TMR_STOP;
}

void PauseTimer()
{
	if(g_nTimerStatus == TMR_RUNNING)
	{
		g_tmr.PauseTimer();
		g_nTimerStatus = TMR_PAUSED;
	}
}

void ResumeTimer()
{
	if(g_nTimerStatus == TMR_PAUSED)
	{
		g_tmr.ResumeTimer();
		g_nTimerStatus = TMR_RUNNING;
	}
}

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int nParLen = 0;
	char *pParam;
	char szCmd[41], szSubParam[256];

	printf("Entered %s\n", FUNC_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if(nParLen <= 0)
		{
			StartTimeTick();
		}
		else
		{
			STR_SEPERATE(pParam , szCmd, szSubParam, 255 );
			if(strcmp(szCmd, "INIT") == 0)
			{
				InitTimer();
			}
			else if(strcmp(szCmd, "START") == 0)
			{
				StartTimer();
			}
			else if(strcmp(szCmd, "STOP") == 0)
			{
				StopTimer();
			}
			else if(strcmp(szCmd, "PAUSE") == 0)
			{
				PauseTimer();
			}
			else if(strcmp(szCmd, "RESUME") == 0)
			{
				ResumeTimer();
			}
			else
			{
				printf("Invalid Parameter in %s\n", FUNC_NAME);
				msRet = SYS_ERROR;
			}
		}
	} while(0);

	printf("Leaved %s\n", FUNC_NAME);
	return msRet;
}

} // End of [extern "C"]
//==========================================================================================
