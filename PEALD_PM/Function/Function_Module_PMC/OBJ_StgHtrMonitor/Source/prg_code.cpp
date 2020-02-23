///////////////////////////////////////////////////////////////////////////////
// StgHtr Monitor
// Create : 2004.08.18
// Last Update : 2005.11.02

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

#include "Timer.h"
#include "Definition.h"

#include "StgHtrMonitor.h"				// for Omron E5ER
#include "StgHtrMonitorType2.h"			// for Watlow 988
#include "StgHtrMonitorType3.h"			// for Eurotherm-3508

CStgHtrMonitor *g_pStgHtrMon = NULL;
char g_szSHTNum[256];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_GbIntlckDM"	, _K_D_IO	,	SW_GbIntlckDM,		0	} ,
	{ "SW_ModuleFailDM"	, _K_D_IO	,	SW_ModuleFailDM,	0	} ,
	{ "SHT_PARM_Save"	, _K_D_IO	,	SHT_PARM_Save,		0	} ,
	{ "PRMA_TL_SHtr$0Tmp"	, _K_A_IO	,	PRMA_TL_SHtrTmp,	0	} ,
	{ "PRMA_TO_SHtr$0Stb"	, _K_A_IO	,	PRMA_TO_SHtrStb,	0	} ,
	""
};

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
	char szCmd[41], szSubParam[256];

	printf("Entered %s\n", FUNC_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if(nParLen > 0)
		{
			STR_SEPERATE(pParam , szCmd, szSubParam, 255 );
			if(strcmp(szCmd, "SW_MON") == 0)
			{
				printf("-----> Heater Temp Monitoring by SW\n");
				g_pStgHtrMon->m_nMonMethod = 1;	// 0:HW Monitoring 1:SW Monitoring
			}
			else
			{
				printf("Invalid Parameter in %s\n", FUNC_NAME);
				// msRet = SYS_ERROR;
			}
		}

		msRet = g_pStgHtrMon->StartMonitor();
	} while(0);

	printf("Leave %s\n", FUNC_NAME);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	g_pStgHtrMon->Initialize(g_szSHTNum);
	
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		if(g_pStgHtrMon != NULL) delete g_pStgHtrMon;
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char szParam[256];
	char szType[256];

	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	// printf("Argument = %s in Program_Enter_Code\n", szParam);

	sscanf(szParam, "%s %s", szType, g_szSHTNum);
	REPLACE_CHAR_SET(0, g_szSHTNum);   //2014.03.25
	if(strlen(szParam) == 0 || szParam[4] == '1')
	{
		g_pStgHtrMon = new CStgHtrMonitor;			// Default Type for E5ER Controller
		printf("Stage Heater Monitor Function[Type1] is created\n");
	}
	else if(szParam[4] == '2')
	{
		g_pStgHtrMon = new CStgHtrMonitorType2;		// Type2 for Watlow 988 Controller
		printf("Stage Heater Monitor Function[Type2] is created\n");
	}
	else if(szParam[4] == '3')
	{
		g_pStgHtrMon = new CStgHtrMonitorType3;		// 2007.03.09
		printf("Stage Heater Monitor Function[Type3] is created\n");
	}
	else
	{
		g_pStgHtrMon = new CStgHtrMonitor;
		printf("Argument is invalid. So Stage Heater Monitor Function[Default Type] is created\n");
	}
}

} // End of [extern "C"]
//==========================================================================================
