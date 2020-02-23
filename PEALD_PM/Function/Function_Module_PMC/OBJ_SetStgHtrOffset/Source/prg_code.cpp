///////////////////////////////////////////////////////////////////////////////
// StgHtr Monitor
// Create : 2004.08.18

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
#define FUNC_NAME	"SetStgHtrOffset"

//------------------------------------------------------------------------------------------
// Alarm Definition(2700 ~ 2799)

#define ALARM_STAGE_HTR_CONTROLL_FAIL	2750
#define ALARM_STAGE_TEMP_HIGH_LIMIT		2751
#define ALARM_STAGE_TEMP_LOW_LIMIT		2752

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SHT_TmpSpAIO"		, _K_A_IO	,	SHT_TmpSpAIO,		0	} ,
	{ "SHT_TmpRdAI"			, _K_A_IO	,	SHT_TmpRdAI,		0	} ,
	{ "SHT_CalOffsAIO"		, _K_A_IO	,	SHT_CalOffsAIO,		0	} ,

	{ "PRMD_Offset_Opt"		, _K_D_IO	,	PRMD_Offset_Opt,	0	} ,
	{ "PRMA_Std_Temp"		, _K_A_IO	,	PRMA_Std_Temp,		0	} ,
	{ "PRMA_UpTemp_Offset"	, _K_A_IO	,	PRMA_UpTemp_Offset,	0	} ,
	{ "PRMA_DnTemp_Offset"	, _K_A_IO	,	PRMA_DnTemp_Offset,	0	} ,
	""
};

//------------------------------------------------------------------------------------------
typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int g_StgHtrOffset_Thread		= 0;

void StgHtrOffsetThread()
{
	int nIOStatus;
	int nStgTempSp;
	int nStgTempRb;
	int nStgTempOffsetRb;
	int nStgOffsetOpt;
	int nStgStdTemp;
	int nStgUpTempOffset;
	int nStgDnTempOffset;
	int nStgTempTol = 100;
	do{

			WAIT_SECONDS(1);
			g_StgHtrOffset_Thread = THREAD_RUN;

			while(1)
			{
				nStgOffsetOpt    = (int) READ_DIGITAL(PRMD_Offset_Opt , &nIOStatus );
				
				if(nStgOffsetOpt > 0)
				{
					nStgTempSp		 = (int) READ_ANALOG(SHT_TmpSpAIO, &nIOStatus);
					WAIT_SECONDS(1);
					nStgTempRb		 = (int) READ_ANALOG(SHT_TmpRdAI, &nIOStatus);
					WAIT_SECONDS(1);
					nStgStdTemp		 = (int) READ_ANALOG(PRMA_Std_Temp, &nIOStatus);
					WAIT_SECONDS(1);
					nStgTempOffsetRb = (int) READ_ANALOG(SHT_CalOffsAIO, &nIOStatus);

					if(abs(nStgTempSp-nStgTempRb) > nStgTempTol)
					{
						nStgTempSp = nStgTempRb;
					}

					WAIT_SECONDS(1);
					if(nStgTempSp >= nStgStdTemp)
					{
						nStgUpTempOffset = (int) READ_ANALOG(PRMA_UpTemp_Offset, &nIOStatus);
						if(SHT_CalOffsAIO != PRMA_UpTemp_Offset)
						{
							WRITE_ANALOG(SHT_CalOffsAIO, nStgUpTempOffset, &nIOStatus);
						}
					}
					else
					{
						nStgDnTempOffset = (int) READ_ANALOG(PRMA_DnTemp_Offset, &nIOStatus);
						if(SHT_CalOffsAIO != PRMA_DnTemp_Offset)
						{
							WRITE_ANALOG(SHT_CalOffsAIO, nStgDnTempOffset, &nIOStatus);
						}						
					}	
				}

				WAIT_SECONDS(5);
			};

	}while(0);

	g_StgHtrOffset_Thread = THREAD_STOP;
	_endthread();
}

void Start_StgHtrOffset_Thread()
{
	if(g_StgHtrOffset_Thread != THREAD_RUN)
	{
		_beginthread((_THREAD_FUNC) StgHtrOffsetThread, 0, NULL);
	}
}
//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int nParLen = 0;
	char *pParam;

	printf("Entered %s\n", FUNC_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);
		
		Start_StgHtrOffset_Thread(); // Start StgHtrOffset Thread

	} while(0);

	printf("Leave %s\n", FUNC_NAME);
	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
