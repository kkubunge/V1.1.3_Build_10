#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
#include "kutletc.h"
#include "kutlanal.h"
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/* Revision History
	1. 2007/10/01 by JJY
		(1) PMC 관련 내용 추가

			
*/


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum	{ TMC , PM12 , PM34 , PM56 } ; // Source & Target Station

//... Alarm Recovery Enum of Engine
// 	0 : ALM_CLEAR
// 	1 : ALM_RETRY
// 	2 : ALM_WAIT
// 	3 : ALM_IGNORE
// 	4 : ALM_DISABLE
// 	5 : ALM_ENABLE
// 	6 : ALM_STOP
// 	7 : ALM_ABORT

#define MAX_ALARM_QUEUE		200
#define MAX_ALARM_MODULE	6

//... 2015.02.07 Add Process Pause Alarm Clear Skip
#define ALARM_PM_PROC_PAUSE		1050

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {

	//{ "CTC_ALARM_STATION"		, _K_D_IO  ,	CTC_ALARM_STATION		,	0	} ,
		
	""
};

//////////////////////////////////////////////////////////////////////////
//... System All Alarm Clear
Module_Status Alarm_Clear_All() 
{
	int		i;	
	int		nTotalAlmCount = 0;
	int		nAlarmID[MAX_ALARM_QUEUE];
	int		nModuleNo[MAX_ALARM_QUEUE];
	int		nCurrentAlmCount = 0;
    //... 2016.02.28
	nTotalAlmCount = (ALARMQUEUECOUNT() > MAX_ALARM_QUEUE) ? MAX_ALARM_QUEUE : ALARMQUEUECOUNT();
	printf("[ALARM] Current Total Count[%d]\n", nTotalAlmCount);	

	//... Read Alarm ID & Module Number
	for(i = 0; i < nTotalAlmCount; i++) 
	{
		nAlarmID[i]		= ALARMQUEUEDATA(i, &nCurrentAlmCount);
		nModuleNo[i]	= ALARMQUEUEMODULE(i, &nCurrentAlmCount);

		printf("[ALARM] (%d/%d)Read AlarmID[%d] Module[%d]\n", i, nTotalAlmCount, nAlarmID[i], nModuleNo[i]);
	}

	//... Clear Alarm
	for(i = 0; i < nTotalAlmCount; i++)
	{
		if(nModuleNo[i] > 0 && nModuleNo[i] <= MAX_ALARM_MODULE)
		{
			//... 2015.02.07 Add Process Pause Alarm Clear Skip
			if(nAlarmID[i] != ALARM_PM_PROC_PAUSE)
			{
				ALARMRECOVER(nModuleNo[i], nAlarmID[i], ALM_CLEAR);
				printf("[ALARM] (%d/%d)Module[%d] AlarmID[%d] Cleared\n", i, nTotalAlmCount, nModuleNo[i], nAlarmID[i]);
			}
		}
		else
		{
			ALARMRECOVER(0, nAlarmID[i], ALM_CLEAR);
			printf("[ALARM] (%d/%d)Module[%d] AlarmID[%d] Cleared\n", i, nTotalAlmCount, nModuleNo[i], nAlarmID[i]);
		}
	}
	printf("[ALARM] Current Total Count[%d]\n", nTotalAlmCount);

	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//... 
Module_Status Program_Main() {

	char RunStr[21] , ParamStr[21] , TBuffer[9] ;
	Module_Status Result ;
	SYSTEMTIME st ;

	if(strlen(PROGRAM_PARAMETER_READ()) < 1)
	{
		printf("[ERR] Alarm Clear Error -Invalid Parameter Length\n");
		return SYS_ABORTED;
	}
	
	STR_SEPERATE(PROGRAM_PARAMETER_READ(), RunStr, ParamStr, 20);

	_strtime(TBuffer);
	GetSystemTime(&st);
	printf("[%s.%03d] : [%s %s] Alarm Clear Function Start\n", TBuffer, st.wMilliseconds, RunStr, ParamStr);

	//.....
	if(! STRCMP_L(RunStr, "CLEAR"))
	{
		printf("[ERR] Alarm Clear Error - Invalid Parameter String[%s]\n", PROGRAM_PARAMETER_READ());
		return SYS_ABORTED;
	}
	else 
	{

		Result = Alarm_Clear_All();	

	}

	_strtime(TBuffer);
	GetSystemTime(&st);
	printf("[%s.%03d] : [%s %s] Alarm Clear Function End\n", TBuffer, st.wMilliseconds, RunStr, ParamStr);

	return Result;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}