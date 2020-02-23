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

#include "TextParser.h"
#include "TextLogDll_M.h"
#include "Definition.h"
#include "PLCSimulator.h"
#include "PLCSimulatorType2.h"
#include "PMMainSimulator.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	{ "SimModeDM"				, _K_D_IO	,	SimModeDM	,	0	} ,
	""
};

CPMMainSimulator *g_pPMMainSimator = NULL;
CPLCSimulator *g_pPLCSimator = NULL;

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int nParLen = 0;
	char *pParam;
	char szCmd[41], szSubParam[256];
	int nIOStatus;

	do {
		// 2006.01.18
		// SimModeDM => 0:NotSim, 1:Simul, 2:PLCTest
		if ( READ_DIGITAL( IO_Driver_Loding , &nIOStatus ) != 0
					&& READ_DIGITAL( SimModeDM , &nIOStatus ) == 0 ) break;

		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if ( nParLen < 1 )	{
			printf( "---> Parameter Error, Len = %d in %s !\n", nParLen, APP_NAME);
			msRet = SYS_ERROR;
			break;
		}

		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );

		// PMMainSimulator Command


		// PLCSimulator Command
		if ( READ_DIGITAL( SimModeDM , &nIOStatus ) == 2 ) break;
		if( STRCMP_L(szCmd, "SET_VALVE_DATA") ) {
			msRet = g_pPLCSimator->SetValveData(szSubParam);
		}
		else if( STRCMP_L(szCmd, "DOWNLOAD") ) {
			msRet = g_pPLCSimator->Download(szSubParam);
		}
		else if( STRCMP_L(szCmd, "CLOSE_ALL_VALVE") ) {
			msRet = g_pPLCSimator->CloseAllValve();
		}
		else if( STRCMP_L(szCmd, "START_VRCP") ) {
			msRet = g_pPLCSimator->StartValveRecipe();
		}
		else if( STRCMP_L(szCmd, "RESUME_VRCP") ) {
			msRet = g_pPLCSimator->ResumeValveRecipe();
		}
		else if( STRCMP_L(szCmd, "STOP_VRCP") ) {
			msRet = g_pPLCSimator->StopValveRecipe();
		}
		else if( STRCMP_L(szCmd, "GO_NEXT_STEP") ) {
			msRet = g_pPLCSimator->GoNextStep();
		}
		else {
			printf("---> Invalid Command in %s !\n", APP_NAME);
			msRet = SYS_ERROR;
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
		if(g_pPLCSimator != NULL)		delete g_pPLCSimator;
		if(g_pPMMainSimator != NULL)	delete g_pPMMainSimator;

		_gCLOSE_LOG();
	}
}

//////////////////////////////////////////////////////////////////////////
void Program_Enter_Code() {
	char	szParam[256];
	char	szTemp[80];
	int		nPLCSimType = 1;
	int		nStgHtrType = 1;

	//...
	strcpy(szParam, PROGRAM_ARGUMENT_READ());

	//...
	CTextParser tp(szParam);
	while(! tp.IsEnd())
	{
		tp.GetWord(szTemp, '|');
		if     (tp.BeEqual("PLCSimType1", szTemp))	nPLCSimType = 1;
		else if(tp.BeEqual("PLCSimType2", szTemp))	nPLCSimType = 2;
		else if(tp.BeEqual("StgHtrType1", szTemp))	nStgHtrType = 1;
		else if(tp.BeEqual("StgHtrType2", szTemp))	nStgHtrType = 2;
	}

	g_pPMMainSimator = new CPMMainSimulator(nStgHtrType);

	if(nPLCSimType == 1)	g_pPLCSimator = new CPLCSimulator;
	else					g_pPLCSimator = new CPLCSimulatorType2;

	printf("-----> PMSimulator[PLCType:%d, StgHtrType:%d] started... \n", nPLCSimType, nStgHtrType);
}

//////////////////////////////////////////////////////////////////////////
BOOL Program_Init_Code() {
	int nIOStatus;

	do {
		if(READ_DIGITAL(IO_Driver_Loding, &nIOStatus) != 0 && READ_DIGITAL(SimModeDM, &nIOStatus) == 0)
			break;

		//.....
		g_pPMMainSimator->Initialize();

		if(READ_DIGITAL(SimModeDM, &nIOStatus) == 2)	break;
		
		//.....
		g_pPLCSimator->Initialize();

	} while(0);
	
	return	TRUE;
}

} // End of [extern "C"]
//==========================================================================================
