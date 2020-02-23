///////////////////////////////////////////////////////////////////////////////

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

#include <string>
#include <vector>

using namespace std;

#include "ParamItem.h"
#include "ParamMonitor.h"

#include "StopWatch.h"

CStopWatch g_Watch;


//------------------------------------------------------------------------------------------
#define APP_NAME	"RF_MAT_Parameter_Monitor"

#define CONFIG_RF1	".\\Function\\Function_Module_PMC\\OBJ_RF_MAT_ParamMon\\RF1.cfg"
#define CONFIG_RF2	".\\Function\\Function_Module_PMC\\OBJ_RF_MAT_ParamMon\\RF2.cfg"
#define CONFIG_MATCH1 ".\\Function\\Function_Module_PMC\\OBJ_RF_MAT_ParamMon\\MATCH1.cfg"
#define CONFIG_MATCH2 ".\\Function\\Function_Module_PMC\\OBJ_RF_MAT_ParamMon\\MATCH2.cfg"

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{"IO_Driver_Loding"	, _K_D_IO, IO_DRIVER_LODING	, 0}, 
	{"PRO_RunStsDM"		, _K_D_IO, PRO_RunStsDM		, 0}, 
	{"RF_MAT_ParmSaveDM", _K_D_IO, RF_MAT_ParmSaveDM, 0}, 
	""
};
//------------------------------------------------------------------------------------------

typedef enum { Stop, Running } RUN_STATUS;
//... 2016.03.11
enum {eOFF_0, eON_1};

CRITICAL_SECTION g_csLock;

//==========================================================================================
Module_Status CheckParameters(const char *szFileName)
{
	Module_Status msRet;
	CParamMonitor *pParamMon = NULL;	

	g_Watch.Start();
	g_Watch.CheckStart(szFileName);
	
	pParamMon = CParamMonitor::CreateInstance(szFileName);
	
	if( pParamMon == NULL ) 
	{
		return SYS_ERROR;
	}
	
	if( pParamMon->UpdateParameters() )
	{
		pParamMon->CompareParameters();
		msRet = SYS_SUCCESS;
	} 
	else 
	{
		msRet = SYS_ERROR;
	}
	delete pParamMon;

	g_Watch.CheckEnd(szFileName);

	g_Watch.Stop();
	g_Watch.Report(stdout);
	g_Watch.Reset();
	
	return msRet;	
}

unsigned __stdcall CheckParameters_Thread(void *pParam)
{
	EnterCriticalSection(&g_csLock);

	CheckParameters((char*)pParam);

	LeaveCriticalSection(&g_csLock);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() 
{
	Module_Status msRet = SYS_SUCCESS;

	int nIOResult;
	char *pParam = PROGRAM_PARAMETER_READ();

	printf("Enter %s(%s)\n", APP_NAME, pParam);

	if( READ_DIGITAL(PRO_RunStsDM, &nIOResult) == Running ) 
	{
		printf("[%s] Can not be run when process running...\n", APP_NAME);
		msRet = SYS_ERROR;
	} 
	else 
	{	
		g_Watch.Start();

		if( strcmp(pParam, "RF1") == 0 )
		{
			msRet = CheckParameters(CONFIG_RF1);
		}
		else if( strcmp( pParam, "RF2") == 0 )
		{
			msRet = CheckParameters(CONFIG_RF2);
		}
		else if( strcmp( pParam, "MATCH1") == 0 )
		{
			msRet = CheckParameters(CONFIG_MATCH1);
		}
		else if( strcmp( pParam, "MATCH2") == 0 )
		{
			msRet = CheckParameters(CONFIG_MATCH2);
		} 
		else 
		{
			printf("[%s] Wrong Parameter", APP_NAME);
			msRet = SYS_ERROR;
		}
		//... 2016.03.11 Update IO Save
		WRITE_DIGITAL(RF_MAT_ParmSaveDM, eON_1 ,&nIOResult);

		g_Watch.Stop();
		g_Watch.Report(stdout);
		g_Watch.Reset();
	}

	printf("Leave %s\n", APP_NAME);

	return msRet;
}

//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) 
{
	if(no == -1)
	{
		// Deinitialize
		DeleteCriticalSection(&g_csLock);
	}
}

//---------------------------------------------------------------------------------------
void Program_Enter_Code() 
{
	InitializeCriticalSection(&g_csLock);

	// 여기서 최초 체크를 한다.	
	_beginthreadex(NULL, 0, CheckParameters_Thread, CONFIG_RF1, 0, NULL);
	_beginthreadex(NULL, 0, CheckParameters_Thread, CONFIG_RF2, 0, NULL);
	_beginthreadex(NULL, 0, CheckParameters_Thread, CONFIG_MATCH1, 0, NULL);
	_beginthreadex(NULL, 0, CheckParameters_Thread, CONFIG_MATCH2, 0, NULL);
}

//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() 
{
	return TRUE;
}



//==========================================================================================
