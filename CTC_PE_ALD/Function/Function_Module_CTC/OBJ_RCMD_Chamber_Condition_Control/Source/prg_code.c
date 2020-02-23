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
//#define		MAX_PORT_NUMS		2
//------------------------------------------------------------------------------------------
//enum	{ PORT1 , PORT2 , PORT3 , PORT4 };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { OFF_0, ON_1 };

IO_Name_String_Map IO_STR_Map_Table[] = {
	{	"SCHEDULER"						,	_K_F_IO	,	SCHEDULER				,	0		} ,
	{	"CHAMBER_CON_CTL_PM1"			,	_K_F_IO	,	PM1_CHAMBER_CON_CTL		,	0		} ,
	{	"CHAMBER_CON_CTL_PM2"			,	_K_F_IO	,	PM2_CHAMBER_CON_CTL		,	0		} ,
	{	"CHAMBER_CON_CTL_PM3"			,	_K_F_IO	,	PM3_CHAMBER_CON_CTL		,	0		} ,
	{	"CHAMBER_CON_CTL_PM4"			,	_K_F_IO	,	PM4_CHAMBER_CON_CTL		,	0		} ,
	{	"CHAMBER_CON_CTL_PM5"			,	_K_F_IO	,	PM5_CHAMBER_CON_CTL		,	0		} ,
	{	"CHAMBER_CON_ABORT_PM1"			,	_K_F_IO	,	PM1_CHAMBER_CON_ABORT	,	0		} ,
	{	"CHAMBER_CON_ABORT_PM2"			,	_K_F_IO	,	PM2_CHAMBER_CON_ABORT	,	0		} ,
	{	"CHAMBER_CON_ABORT_PM3"			,	_K_F_IO	,	PM3_CHAMBER_CON_ABORT	,	0		} ,
	{	"CHAMBER_CON_ABORT_PM4"			,	_K_F_IO	,	PM4_CHAMBER_CON_ABORT	,	0		} ,
	{	"CHAMBER_CON_ABORT_PM5"			,	_K_F_IO	,	PM5_CHAMBER_CON_ABORT	,	0		} ,
	{	"PM1_ModeChangeIgnore"			,	_K_D_IO	,	PM1_ModeChangeIgnore	,	0		} ,
	{	"PM2_ModeChangeIgnore"			,	_K_D_IO	,	PM2_ModeChangeIgnore	,	0		} ,
	{	"PM3_ModeChangeIgnore"			,	_K_D_IO	,	PM3_ModeChangeIgnore	,	0		} ,
	{	"PM4_ModeChangeIgnore"			,	_K_D_IO	,	PM4_ModeChangeIgnore	,	0		} ,
	{	"PM5_ModeChangeIgnore"			,	_K_D_IO	,	PM5_ModeChangeIgnore	,	0		} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*BOOL Port_Name_Check( char *Module , int *PortID ) {
	*PortID = -1;
	if		( STRNCMP_L( Module , "PORT", 4 ) ) *PortID = atoi(Module+4)-1;
	else if ( STRNCMP_L( Module , "CM"	, 2 ) ) *PortID = atoi(Module+2)-1;
	else return FALSE;
	if ( *PortID < PORT1 || *PortID > PORT4 || *PortID >= MAX_PORT_NUMS ) return FALSE;
	return TRUE;
}
*/
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
	char *pszED , szProcessModule[512] , szControlOption[512] , szExecutionOption[512], szRemainStr1[512];
	char szToken = '|';

	memset(szProcessModule,		0,	512);
	memset(szControlOption,		0,	512);
	memset(szExecutionOption,	0,	512);
	memset(szRemainStr1,		0,	512);

	pszED = PROGRAM_EVENT_READ(); // MAKE_LOAD_REQUEST_CF CM1 or CM2
	STR_SEPERATE_CHAR( pszED , szToken , szProcessModule , szRemainStr1  , 511 );
	STR_SEPERATE_CHAR( szRemainStr1 , szToken , szControlOption , szExecutionOption , 511 );

	if(STRCMP_L( szProcessModule , "PMJobCreate")
		||STRCMP_L( szProcessModule , "PMJobAbort")){
		return;
	}

	if ( STRCMP_L( szProcessModule , "PM1"))
	{
		if(STRCMP_L(szControlOption, "DryCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM1_CHAMBER_CON_CTL, "RUN 00 01 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{
				RUN_FUNCTION(PM1_CHAMBER_CON_CTL, "ABORT 00 01 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}			
		}
		else if(STRCMP_L(szControlOption, "WetCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{ 
				WRITE_DIGITAL(PM1_ModeChangeIgnore, ON_1, &nIOResult);
				RUN_FUNCTION(PM1_CHAMBER_CON_CTL, "RUN 00 02 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{
				WRITE_DIGITAL(PM1_ModeChangeIgnore, OFF_0, &nIOResult);
				RUN_FUNCTION(PM1_CHAMBER_CON_CTL, "ABORT 00 02 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}	
		}
		else if(STRCMP_L(szControlOption, "IdleControl"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM1_CHAMBER_CON_CTL, "RUN 00 03 00");
			}
			else if (STRCMP_L(szExecutionOption, "Stop"))
			{
				RUN_FUNCTION(PM1_CHAMBER_CON_CTL, "ABORT 00 03 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else
		{
			printf("Control Option is Invalid Error --> [%s]\n", szControlOption);
			return;
		}
	}
	else if ( STRCMP_L( szProcessModule , "PM2"))
	{
		if(STRCMP_L(szControlOption, "DryCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM2_CHAMBER_CON_CTL, "RUN 00 01 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{
				RUN_FUNCTION(PM2_CHAMBER_CON_CTL, "ABORT 00 01 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}			
		}
		else if(STRCMP_L(szControlOption, "WetCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				WRITE_DIGITAL(PM2_ModeChangeIgnore, ON_1, &nIOResult);
				RUN_FUNCTION(PM2_CHAMBER_CON_CTL, "RUN 00 02 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{ 
				WRITE_DIGITAL(PM2_ModeChangeIgnore, OFF_0, &nIOResult);
				RUN_FUNCTION(PM2_CHAMBER_CON_CTL, "ABORT 00 02 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}	
		}
		else if(STRCMP_L(szControlOption, "IdleControl"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM2_CHAMBER_CON_CTL, "RUN 00 03 00");
			}
			else if (STRCMP_L(szExecutionOption, "Stop"))
			{
				RUN_FUNCTION(PM2_CHAMBER_CON_CTL, "ABORT 00 03 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else
		{
			printf("Control Option is Invalid Error --> [%s]\n", szControlOption);
			return;
		}
	}
	else if ( STRCMP_L( szProcessModule , "PM3"))
	{
		if(STRCMP_L(szControlOption, "DryCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM3_CHAMBER_CON_CTL, "RUN 00 01 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{
				RUN_FUNCTION(PM3_CHAMBER_CON_CTL, "ABORT 00 01 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}			
		}
		else if(STRCMP_L(szControlOption, "WetCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{   
				WRITE_DIGITAL(PM3_ModeChangeIgnore, ON_1, &nIOResult);
				RUN_FUNCTION(PM3_CHAMBER_CON_CTL, "RUN 00 02 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{  
				WRITE_DIGITAL(PM3_ModeChangeIgnore, OFF_0, &nIOResult);
				RUN_FUNCTION(PM3_CHAMBER_CON_CTL, "ABORT 00 02 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else if(STRCMP_L(szControlOption, "IdleControl"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM3_CHAMBER_CON_CTL, "RUN 00 03 00");
			}
			else if (STRCMP_L(szExecutionOption, "Stop"))
			{
				RUN_FUNCTION(PM3_CHAMBER_CON_CTL, "ABORT 00 03 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else
		{
			printf("Control Option is Invalid Error --> [%s]\n", szControlOption);
			return;
		}
	}
	else if ( STRCMP_L( szProcessModule , "PM4"))
	{
		if(STRCMP_L(szControlOption, "DryCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM4_CHAMBER_CON_CTL, "RUN 00 01 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{
				RUN_FUNCTION(PM4_CHAMBER_CON_CTL, "ABORT 00 01 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else if(STRCMP_L(szControlOption, "WetCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{   
				WRITE_DIGITAL(PM4_ModeChangeIgnore, ON_1, &nIOResult);
				RUN_FUNCTION(PM4_CHAMBER_CON_CTL, "RUN 00 02 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{
				WRITE_DIGITAL(PM4_ModeChangeIgnore, OFF_0, &nIOResult);
				RUN_FUNCTION(PM4_CHAMBER_CON_CTL, "ABORT 00 02 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else if(STRCMP_L(szControlOption, "IdleControl"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM4_CHAMBER_CON_CTL, "RUN 00 03 00");
			}
			else if (STRCMP_L(szExecutionOption, "Stop"))
			{
				RUN_FUNCTION(PM4_CHAMBER_CON_CTL, "ABORT 00 03 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else
		{
			printf("Control Option is Invalid Error --> [%s]\n", szControlOption);
			return;
		}
	}
	else if ( STRCMP_L( szProcessModule , "PM5"))
	{
		if(STRCMP_L(szControlOption, "DryCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM5_CHAMBER_CON_CTL, "RUN 00 01 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{
				RUN_FUNCTION(PM5_CHAMBER_CON_CTL, "ABORT 00 01 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else if(STRCMP_L(szControlOption, "WetCleaning"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{ 
				WRITE_DIGITAL(PM5_ModeChangeIgnore, ON_1, &nIOResult);
				RUN_FUNCTION(PM5_CHAMBER_CON_CTL, "RUN 00 02 00");
			}
			else if (STRCMP_L(szExecutionOption, "Abort"))
			{ 
				WRITE_DIGITAL(PM5_ModeChangeIgnore, OFF_0, &nIOResult);
				RUN_FUNCTION(PM5_CHAMBER_CON_CTL, "ABORT 00 02 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else if(STRCMP_L(szControlOption, "IdleControl"))
		{
			if(STRCMP_L(szExecutionOption, "Run"))
			{
				RUN_FUNCTION(PM5_CHAMBER_CON_CTL, "RUN 00 03 00");
			}
			else if (STRCMP_L(szExecutionOption, "Stop"))
			{
				RUN_FUNCTION(PM5_CHAMBER_CON_CTL, "ABORT 00 03 00");
			}
			else
			{
				printf("Execution Option is Invalid Error --> [%s]\n", szExecutionOption);
				return;
			}
		}
		else
		{
			printf("Control Option is Invalid Error --> [%s]\n", szControlOption);
			return;
		}
	}
	else 
	{
		printf("Process Module is Invalid Error --> [%s]\n", szProcessModule);
		return;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
