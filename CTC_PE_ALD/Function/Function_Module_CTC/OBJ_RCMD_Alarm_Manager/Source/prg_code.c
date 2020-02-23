#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <KutlAnal.h>
#include <kutlgui.h>
#include <kutlStr.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "..\resource.h"

#define MAX_ALARM_OBJECTS 1000

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//#define		MAX_PORT_NUMS		2
//------------------------------------------------------------------------------------------
//enum	{ PORT1 , PORT2 , PORT3 , PORT4 };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{	"SCHEDULER"						,	_K_F_IO	,	SCHEDULER				,	0		} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
	int nALID, nAlarm_Count, nCount, nCount_Module[ MAX_ALARM_OBJECTS ], nCount_Buffer[ MAX_ALARM_OBJECTS ] , i;
	int nIOResult = 0;
	char *pszED , szControlOption[512], szProcessModule[512] , szAlarmNum[512], szRemainString[512];
	char szToken = '|';

	memset(szControlOption,		0,	512);
	memset(szRemainString,		0,	512);
	memset(szProcessModule,		0,	512);
	memset(szAlarmNum,			0,	512);

	nAlarm_Count = ALARMQUEUECOUNT();
	for ( i = 0 ; i < nAlarm_Count ; i++ )
	{
		nCount_Module[ i ] = ALARMQUEUEMODULE( i , &nCount );
		nCount_Buffer[ i ] = ALARMQUEUEDATA( i , &nCount );
	}

	pszED = PROGRAM_EVENT_READ();
	STR_SEPERATE_CHAR( pszED , szToken , szControlOption , szRemainString  , 511 );
	STR_SEPERATE_CHAR( szRemainString , szToken , szProcessModule , szAlarmNum  , 511 );
	if ( STRCMP_L( szControlOption , "Alarm_Clear"))
	{
		nALID = atoi(szAlarmNum);
		if( STRCMP_L( szProcessModule , "CTC"))
		{		
			ALARMRECOVER_DIRECT( 0, nALID, ALM_CLEAR );
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM1"))
		{
			ALARMRECOVER_DIRECT( 2, nALID, ALM_CLEAR );
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM2"))
		{
			ALARMRECOVER_DIRECT( 2, nALID, ALM_CLEAR );
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM3"))
		{
			ALARMRECOVER_DIRECT( 3, nALID, ALM_CLEAR );
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM4"))
		{
			ALARMRECOVER_DIRECT( 4, nALID, ALM_CLEAR );
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM5"))
		{
			ALARMRECOVER_DIRECT( 5, nALID, ALM_CLEAR );
			return;
		}
		else
		{
			printf("ProcessModule is Invalid Error --> [%s]\n", szProcessModule);
			return;
		}
	}
	else if ( STRCMP_L( szControlOption , "Alarm_Retry"))
	{
		if ( nAlarm_Count == nCount )
		{
			for ( i = 0 ; i < nAlarm_Count ; i++ )
			{
				ALARMRECOVER_DIRECT( nCount_Module[ i ], nCount_Buffer[ i ], ALM_RETRY );
			}
		}
		else
		{
			printf("Alarm Count is Invalid Error --> [%d]\n", nCount);
			return;
		}
	}
	else if ( STRCMP_L( szControlOption , "Alarm_Post"))
	{
		nALID = atoi(szAlarmNum);
		if( STRCMP_L( szProcessModule , "CTC"))
		{		
			ALARM_POST(nALID);
			return;
		}
		else
		{
			printf("ProcessModule is Invalid Error --> [%s]\n", szProcessModule);
			return;
		}
	}
	else
	{
		printf("ControlOption is Invalid Error --> [%s]\n", szControlOption);
		return;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
