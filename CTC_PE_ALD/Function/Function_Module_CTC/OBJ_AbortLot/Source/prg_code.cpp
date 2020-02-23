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

#include "TextParser.h"
#include "dyn_iodefine.h"

//------------------------------------------------------------------------------------------
// Thread Type Definition for Using thread

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

//------------------------------------------------------------------------------------------

#define APP_NAME	"AbortLot"

//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Enumeration  Definition

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding",		_K_D_IO,	IO_DRIVER_LOAD,			0	} ,
	{ "SCHEDULER",				_K_F_IO,	_SCHEDULER,		0	},
	{ "CTC.MAIN_CONTROL",		_K_D_IO,	CTC_MAIN_CONTROL,		0	},
	{ "CTC.MAIN_CONTROL2",		_K_D_IO,	CTC_MAIN_CONTROL2,		0	},
	{ "CTC.MAIN_CONTROL3",		_K_D_IO,	CTC_MAIN_CONTROL3,		0	},
	{ "CTC.MAIN_CONTROL4",		_K_D_IO,	CTC_MAIN_CONTROL4,		0	},
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#define PAUSE_WAIT_TIMEOUT		300

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status AbortProc(char *pParam)
{
	BOOL bResult = FALSE;
	dyn_WRITE_DIGITAL(PM1_RunStsDM, 0, &bResult);
	dyn_WRITE_DIGITAL(PM2_RunStsDM, 0, &bResult);
	dyn_WRITE_DIGITAL(PM3_RunStsDM, 0, &bResult);
	dyn_WRITE_DIGITAL(PM4_RunStsDM, 0, &bResult);
	dyn_WRITE_DIGITAL(PM5_RunStsDM, 0, &bResult);
	return SYS_SUCCESS;
}

Module_Status AbortLot(char *pParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus, nChannel;
	int i, j;
	BOOL bLotRun = FALSE;

	do {
		for(i=0; i<4; i++)
		{
			// CTC_MAIN_CONTROL => 0:Idle 1:Running 2:Pausing 3:Paused 4:Aborting 5:Disable
			//						6:Waiting 7:Wait-HandOff
			nChannel = i + CTC_MAIN_CONTROL;
			if(READ_DIGITAL(nChannel, &nIOStatus) == 1)
			{
				bLotRun = TRUE;
				WRITE_FUNCTION_EVENT(_SCHEDULER, "PAUSE(F)");

				for(j=0; j<PAUSE_WAIT_TIMEOUT; j++)
				{
					if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
					if(READ_DIGITAL(nChannel, &nIOStatus) != 1) break;
				}

				WRITE_DIGITAL(nChannel, 4, &nIOStatus);
				if(j >= PAUSE_WAIT_TIMEOUT)
				{
					printf("-----> Timeout Waiting Pause(MAINT_CONTROL:%d) in %s\n",
							i+1, APP_NAME);
				}
			}
			if(msRet != SYS_SUCCESS) break;
		}
		if(msRet != SYS_SUCCESS) break;

		if(bLotRun)
		{
			if(! WAIT_SECONDS(5)) { msRet = SYS_ABORTED; break; }
			WRITE_FUNCTION_EVENT(_SCHEDULER, "ABORT(F)");
		}
	} while(0);
	return msRet;
}

//==========================================================================================
// Initialize Function
BOOL g_bInit = FALSE;

BOOL Initialize()
{
	BOOL bRet = TRUE;
	if(g_bInit == TRUE) return bRet;
	dyn_InitIOList();

	g_bInit = bRet;
	return bRet;
}

//==========================================================================================
extern "C"
{

Module_Status Program_Main()
{	
	Module_Status msRet = SYS_ERROR;
	int nParLen = 0;
	char *pParam;
	char szCmd[41], szSubParam[256];

	printf( "Entered %s\n", APP_NAME);
	if(! g_bInit) Initialize();

	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);
		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );

		if( STRCMP_L(szCmd, "ABORT_PM") )
		{
			printf("-----> Abort Lot and PM\n");
			AbortProc(szSubParam);
			msRet = AbortLot(szSubParam);
			AbortProc(szSubParam);
		}
		else
		{
			printf("-----> Abort Lot\n");
			msRet = AbortLot(szSubParam);
		}
	} while(0);

	printf( "Leaved %s\n", APP_NAME);
	return SYS_SUCCESS;	
}

} // End of [extern "C"]
//==========================================================================================
