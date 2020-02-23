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

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{	"SCHEDULER"						,	_K_F_IO	,	F_SCHEDULER				,	0		} ,
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
	int nIOResult = 0;
	char *pszED , szControlOption[128], szProcessModule[128], szBuffer[128];
	char szToken = '|';

	memset(szControlOption,		0,	128);
	memset(szProcessModule,		0,	128);
	memset(szBuffer,			0,	128);

	pszED = PROGRAM_EVENT_READ();
	STR_SEPERATE_CHAR( pszED , szToken , szControlOption , szProcessModule  , 127 );
	if ( STRCMP_L( szControlOption , "ENABLENX"))
	{
		if(STRCMP_L( szProcessModule , "PM1"))
		{
			sprintf( szBuffer, "SET_MODULE_INFO_LOCAL %s|%s", szProcessModule, szControlOption);
			WRITE_FUNCTION_EVENT(F_SCHEDULER, szBuffer);
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM2"))
		{
			sprintf( szBuffer, "SET_MODULE_INFO_LOCAL %s|%s", szProcessModule, szControlOption);
			WRITE_FUNCTION_EVENT(F_SCHEDULER, szBuffer);
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM3"))
		{
			sprintf( szBuffer, "SET_MODULE_INFO_LOCAL %s|%s", szProcessModule, szControlOption);
			WRITE_FUNCTION_EVENT(F_SCHEDULER, szBuffer);
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM4"))
		{
			sprintf( szBuffer, "SET_MODULE_INFO_LOCAL %s|%s", szProcessModule, szControlOption);
			WRITE_FUNCTION_EVENT(F_SCHEDULER, szBuffer);
			return;
		}
		else if(STRCMP_L( szProcessModule , "PM5"))
		{
			sprintf( szBuffer, "SET_MODULE_INFO_LOCAL %s|%s", szProcessModule, szControlOption);
			WRITE_FUNCTION_EVENT(F_SCHEDULER, szBuffer);
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
