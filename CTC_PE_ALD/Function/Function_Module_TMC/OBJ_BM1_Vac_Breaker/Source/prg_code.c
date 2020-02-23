#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };
int g_nVacBreaker_Thread = THREAD_STOP;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "BM1.VacuumStatusDM"		, _K_D_IO	,	BM1_VacuumStatusDM 	,	0	} ,
	{ "CTC.BM1_Wafer_Status"	, _K_D_IO	,	CTC_BM1_Wafer_Status,	0	} ,
	{ "CTC.BM1_Wafer_Result"	, _K_D_IO	,	CTC_BM1_Wafer_Result,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status2"	, _K_D_IO	,	CTC_BM1_Wafer_Status2,	0	} ,
	{ "CTC.BM1_Wafer_Result2"	, _K_D_IO	,	CTC_BM1_Wafer_Result2,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status3"	, _K_D_IO	,	CTC_BM1_Wafer_Status3,	0	} ,
	{ "CTC.BM1_Wafer_Result3"	, _K_D_IO	,	CTC_BM1_Wafer_Result3,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status4"	, _K_D_IO	,	CTC_BM1_Wafer_Status4,	0	} ,
	{ "CTC.BM1_Wafer_Result4"	, _K_D_IO	,	CTC_BM1_Wafer_Result4,	0	} , //Present, Success, Fail,Processing
	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void VacBreaker_Thread()
{
	int CommStatus;
	int nCount = 0;

	while (1) {
			if (THREAD_STOP == g_nVacBreaker_Thread) break;
			// Check
			if (   ((READ_DIGITAL(CTC_BM1_Wafer_Status  , &CommStatus ) >= 1 ) && ( READ_DIGITAL(CTC_BM1_Wafer_Status  , &CommStatus ) <= 25 )) 
				&& ((READ_DIGITAL(CTC_BM1_Wafer_Status2 , &CommStatus ) >= 1 ) && ( READ_DIGITAL(CTC_BM1_Wafer_Status2 , &CommStatus ) <= 25 ))
				&& ((READ_DIGITAL(CTC_BM1_Wafer_Status3 , &CommStatus ) >= 1 ) && ( READ_DIGITAL(CTC_BM1_Wafer_Status3 , &CommStatus ) <= 25 ))
				&& ((READ_DIGITAL(CTC_BM1_Wafer_Status4 , &CommStatus ) >= 1 ) && ( READ_DIGITAL(CTC_BM1_Wafer_Status4 , &CommStatus ) <= 25 )))
			{
				if (   READ_DIGITAL(CTC_BM1_Wafer_Result	, &CommStatus ) != 0
					&& READ_DIGITAL(CTC_BM1_Wafer_Result2	, &CommStatus ) != 0) {
					if (READ_DIGITAL(BM1_VacuumStatusDM	, &CommStatus ) == 1 /*Vacuum*/) {
						
					} else {
						nCount = 0;
					}
				} else {
					nCount = 0;
				}
			}	else {
				nCount = 0;
			}
			
			WAIT_SECONDS(1);

			nCount++;
			if (nCount >= 100) {
				WRITE_STRING( SCHEDULER , "SET_FORCE_MAKE_FMSIDE BM1" , &CommStatus );
				printf("BM1 Send forced vacuum break \n");
				break;
			}
	}

	g_nVacBreaker_Thread = THREAD_STOP;
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	char RunStr[21] , TempStr[21];

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , TempStr , 20 );

	if	( STRCMP_L( RunStr , "ACTIVATE"		) ) {
		if (g_nVacBreaker_Thread != THREAD_RUN) {
			g_nVacBreaker_Thread = THREAD_RUN;
			printf ("BM1 Vac Breaker Activated... \n");
			_beginthread((_THREAD_FUNC) VacBreaker_Thread, 0, NULL);
		} else {
			g_nVacBreaker_Thread = THREAD_STOP;
			WAIT_SECONDS(10);
			if (g_nVacBreaker_Thread != THREAD_RUN) {
				g_nVacBreaker_Thread = THREAD_RUN;
				printf ("VM1 Vac Breaker Activated... \n");
				_beginthread((_THREAD_FUNC) VacBreaker_Thread, 0, NULL);
			}
		}
	} else if (STRCMP_L( RunStr , "ABORT")) {
		g_nVacBreaker_Thread = THREAD_STOP;
		//WAIT_SECONDS(3);
		printf ("BM1 Vac Breaker Aborted... \n");
	}

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

