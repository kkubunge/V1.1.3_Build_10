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

#include "TextParser.h"
#include "Definition.h"

#include "StgHtrControl.h"				// for Omron E5ER
#include "StgHtrControlType2.h"			// for Watlow 988
#include "StgHtrControlType3.h"			// for Eurotherm-3508(2 Zone Control)

CStgHtrControl *g_pStgHtrCtrl = NULL;
char g_szSHTNum[256];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "PRMA_TL_SHtr$0Tmp"	, _K_A_IO	,	PRMA_TL_SHtrTmp,	0	} ,//2014.03.25
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

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

//	printf("Entered %s\n", FUNC_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if(nParLen <= 0)
		{
			printf("Invalid Parameter in %s\n", FUNC_NAME);
			msRet = SYS_ABORTED;
		}
		else
		{
			STR_SEPERATE(pParam , szCmd, szSubParam, 255 );
			//...
			if     (strcmp(szCmd, "CHECK_SET") == 0)		msRet = g_pStgHtrCtrl->CheckSetPoint(szSubParam);
			else if(strcmp(szCmd, "CHECK_SET2") == 0)		msRet = g_pStgHtrCtrl->CheckSetPoint2(szSubParam);	// 2007.02.15
			else if(strcmp(szCmd, "CHECK_TEMP") == 0)		msRet = g_pStgHtrCtrl->CheckTemp(szSubParam);
			else if(strcmp(szCmd, "CHECK_UPPER") == 0)		msRet = g_pStgHtrCtrl->CheckUpper(szSubParam);
			else if(strcmp(szCmd, "CHECK_LOWER") == 0)		msRet = g_pStgHtrCtrl->CheckLower(szSubParam);
			else if(strcmp(szCmd, "SET_TEMP") == 0)			msRet = g_pStgHtrCtrl->SetTemp(szSubParam);
			else if(strcmp(szCmd, "SET_IF_UPPER") == 0)		msRet = g_pStgHtrCtrl->SetIfUpper(szSubParam);
			else if(strcmp(szCmd, "SET_IF_LOWER") == 0)		msRet = g_pStgHtrCtrl->SetIfLower(szSubParam);
			else if(strcmp(szCmd, "IS_UPPER") == 0)			msRet = g_pStgHtrCtrl->IsUpper(szSubParam);
			else if(strcmp(szCmd, "IS_LOWER") == 0)			msRet = g_pStgHtrCtrl->IsLower(szSubParam);
			else if(strcmp(szCmd, "COOL_DOWN") == 0)		msRet = g_pStgHtrCtrl->CoolDown(szSubParam);
			else if(strcmp(szCmd, "RESET_SP") == 0)			msRet = g_pStgHtrCtrl->ResetSetPoint(szSubParam);
			else if(strcmp(szCmd, "SET_RELAY") == 0)		msRet = g_pStgHtrCtrl->SetRelay(szSubParam);
			else
			{
				printf("Invalid Parameter in %s\n", FUNC_NAME);
				msRet = SYS_ABORTED;
			}
		}
	} while(0);

//	printf("Leave %s\n", FUNC_NAME);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	g_pStgHtrCtrl->Initialize(g_szSHTNum);

	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		if(g_pStgHtrCtrl != NULL) delete g_pStgHtrCtrl;
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
		g_pStgHtrCtrl = new CStgHtrControl;			// Default Type for E5ER Controller
		printf("Stage Heater Control Function[Type1] is created\n");
	}
	else if(szParam[4] == '2')
	{
		g_pStgHtrCtrl = new CStgHtrControlType2;	// Type2 for Watlow 988 Controller
		printf("Stage Heater Control Function[Type2] is created\n");
	}
	else if(szParam[4] == '3')
	{
		g_pStgHtrCtrl = new CStgHtrControlType3;	// 2007.03.07
		printf("Stage Heater Control Function[Type3] is created\n");
	}
	else
	{
		g_pStgHtrCtrl = new CStgHtrControl;
		printf("Argument is invalid. So Stage Heater Control Function[Default Type] is created\n");
	}
}

} // End of [extern "C"]
//==========================================================================================
