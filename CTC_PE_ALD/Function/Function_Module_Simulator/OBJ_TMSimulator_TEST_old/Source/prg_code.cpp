//////////////////////////////////////////////////////////////////////////////////////////
// PLC Simulation Control for Metal

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

#include "TextLogDll.h"
#include "TMCtrlSimulator.h"
#include "TMSimulator.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	""
};

CTMCtrlSimulator *g_pTMCtrlSimulator = NULL;
CTMSimulator *g_pTMSimulator = NULL;

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int nParLen = 0;
	char *pParam;
//	int nIOStatus;

	do {
//		if(READ_DIGITAL( IO_Driver_Loding , &nIOStatus ) != 0 )		break;
		pParam = PROGRAM_PARAMETER_READ();
		if(g_pTMCtrlSimulator != NULL)
		{
			if(! g_pTMCtrlSimulator->Main(pParam)) msRet = SYS_ABORTED;
		}
	} while(0);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		// Deinitialize
		if(g_pTMCtrlSimulator != NULL) delete g_pTMCtrlSimulator;
		if(g_pTMSimulator != NULL) delete g_pTMSimulator;
		_gCLOSE_LOG();
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char szParam[256];
	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	#ifdef _DEBUG
	printf("Argument = %s in Program_Enter_Code\n", szParam);
	#endif

	if(strcmp(szParam, "MONITOR") == 0) {
		g_pTMSimulator = new CTMSimulator();
		printf("TM Simulator Created(%s)\n", szParam);
	}
	else {
		g_pTMCtrlSimulator = new CTMCtrlSimulator(szParam);
		printf("TM Control Simulator Created(%s)\n", szParam);
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	// Initialize
//	int nIOStatus;
	do {
//		if ( READ_DIGITAL( IO_Driver_Loding , &nIOStatus ) != 0 ) break;
		_gINIT_LOG();
		if(g_pTMSimulator != NULL) g_pTMSimulator->Initialize();
		if(g_pTMCtrlSimulator != NULL) g_pTMCtrlSimulator->Initialize();
	} while(0);
	return TRUE;
}

} // End of [extern "C"]
//==========================================================================================
