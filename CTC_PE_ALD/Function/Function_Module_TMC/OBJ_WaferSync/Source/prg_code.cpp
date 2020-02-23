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

#include "dyn_iodefine.h"
#include "TextParser.h"
#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
// Thread Type Definition for Using thread

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

//------------------------------------------------------------------------------------------
#define APP_NAME	"Wafer Sync Func"

//------------------------------------------------------------------------------------------
// Enumeration  Definition

enum { WF_ABSENT , WF_PRESENT };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding",		_K_D_IO,	IO_DRIVER_LOAD,			0	} ,
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int GetWaferChannel(char *pModule, int nSlot)
{
	int nChannel = -1;

	if(strcmp(pModule, "ATM_RB_A") == 0) nChannel = ATM_RB_A_WAFER;
	else if(strcmp(pModule, "ATM_RB_B") == 0) nChannel = ATM_RB_B_WAFER;
	else if(strcmp(pModule, "VAC_RB_A") == 0) nChannel = VAC_RB_A_WAFER;
	else if(strcmp(pModule, "VAC_RB_B") == 0) nChannel = VAC_RB_B_WAFER;

	else if(strcmp(pModule, "AL") == 0) nChannel = AL_WAFER;
	else if(strcmp(pModule, "CM1") == 0 && nSlot > 0) nChannel = CM1_01_WAFER + nSlot - 1;
	else if(strcmp(pModule, "CM2") == 0 && nSlot > 0) nChannel = CM2_01_WAFER + nSlot - 1;
	else if(strcmp(pModule, "CM3") == 0 && nSlot > 0) nChannel = CM3_01_WAFER + nSlot - 1;

	else if(strcmp(pModule, "BM1") == 0 && nSlot > 0) nChannel = BM1_01_WAFER + nSlot - 1;
	else if(strcmp(pModule, "BM2") == 0 && nSlot > 0) nChannel = BM2_01_WAFER + nSlot - 1;

	else if(strcmp(pModule, "PM1") == 0) nChannel = PM1_WAFER;
	else if(strcmp(pModule, "PM2") == 0) nChannel = PM2_WAFER;
	else if(strcmp(pModule, "PM3") == 0) nChannel = PM3_WAFER;
	else if(strcmp(pModule, "PM4") == 0) nChannel = PM4_WAFER;
	else if(strcmp(pModule, "PM5") == 0) nChannel = PM5_WAFER;
	else if(strcmp(pModule, "PM6") == 0) nChannel = PM6_WAFER;
	else if(strcmp(pModule, "PM7") == 0) nChannel = PM7_WAFER;
	else if(strcmp(pModule, "PM8") == 0) nChannel = PM8_WAFER;
	else if(strcmp(pModule, "PM9") == 0) nChannel = PM9_WAFER;

	return nChannel;
}

int GetWafSrcChannel(char *pModule, int nSlot)
{
	int nChannel = -1;

	if(strcmp(pModule, "ATM_RB_A") == 0) nChannel = ATM_RB_A_WF_SRC;
	else if(strcmp(pModule, "ATM_RB_B") == 0) nChannel = ATM_RB_B_WF_SRC;
	else if(strcmp(pModule, "VAC_RB_A") == 0) nChannel = VAC_RB_A_WF_SRC;
	else if(strcmp(pModule, "VAC_RB_B") == 0) nChannel = VAC_RB_B_WF_SRC;

	else if(strcmp(pModule, "AL") == 0) nChannel = AL_WF_SRC;

	else if(strcmp(pModule, "BM1") == 0 && nSlot > 0) nChannel = BM1_01_WF_SRC + nSlot - 1;
	else if(strcmp(pModule, "BM2") == 0 && nSlot > 0) nChannel = BM2_01_WF_SRC + nSlot - 1;

	else if(strcmp(pModule, "PM1") == 0) nChannel = PM1_WF_SRC;
	else if(strcmp(pModule, "PM2") == 0) nChannel = PM2_WF_SRC;
	else if(strcmp(pModule, "PM3") == 0) nChannel = PM3_WF_SRC;
	else if(strcmp(pModule, "PM4") == 0) nChannel = PM4_WF_SRC;
	else if(strcmp(pModule, "PM5") == 0) nChannel = PM5_WF_SRC;
	else if(strcmp(pModule, "PM6") == 0) nChannel = PM6_WF_SRC;
	else if(strcmp(pModule, "PM7") == 0) nChannel = PM7_WF_SRC;
	else if(strcmp(pModule, "PM8") == 0) nChannel = PM8_WF_SRC;
	else if(strcmp(pModule, "PM9") == 0) nChannel = PM9_WF_SRC;

	return nChannel;
}

int GetWaferNumber(char *pModule, int nSlot)
{
	BOOL bIOStatus;
	int nChannel = -1;
	int nWaferN = 0;
	if(strncmp(pModule, "CM", 2) == 0)
	{
		nChannel = GetWaferChannel(pModule, nSlot);
		if(nChannel >= 0)
			if(dyn_READ_DIGITAL(nChannel, &bIOStatus) >= 2) nWaferN = nSlot;
	}
	else
	{
		nChannel = GetWaferChannel(pModule, nSlot);
		if(nChannel >= 0)
			if(nChannel >= 0) nWaferN = dyn_READ_DIGITAL(nChannel, &bIOStatus);
	}
	return nWaferN;
}

int GetWaferSource(char *pModule, int nSlot)
{
	BOOL bIOStatus;
	int nChannel = -1;
	int nWafSrc = -1;
	if(strncmp(pModule, "CM", 2) == 0) nWafSrc = atoi(pModule+2) - 1;
	else
	{
		nChannel = GetWafSrcChannel(pModule, nSlot);
		if(nChannel >= 0) nWafSrc = dyn_READ_DIGITAL(nChannel, &bIOStatus);
	}
	return nWafSrc;
}

void SetWaferNumber(char *pModule, int nSlot, int nWaferN)
{
	BOOL bIOStatus;
	int nChannel = -1;
	if(strncmp(pModule, "CM", 2) == 0)
	{
		nChannel = GetWaferChannel(pModule, nSlot);
		if(nChannel >= 0)
		{
			if(nWaferN > 0) dyn_WRITE_DIGITAL(nChannel, 2, &bIOStatus);
			else dyn_WRITE_DIGITAL(nChannel, 1, &bIOStatus); 
		}
	}
	else
	{
		nChannel = GetWaferChannel(pModule, nSlot);
		if(nChannel >= 0) dyn_WRITE_DIGITAL(nChannel, nWaferN, &bIOStatus);
	}
}

void SetWaferSource(char *pModule, int nSlot, int nWafSrc)
{
	BOOL bIOStatus;
	int nChannel = -1;
	if(strncmp(pModule, "CM", 2) == 0) return;
	nChannel = GetWafSrcChannel(pModule, nSlot);
	if(nChannel >= 0) dyn_WRITE_DIGITAL(nChannel, nWafSrc, &bIOStatus);
}

//==========================================================================================
extern "C"
{

Module_Status Program_Main()
{	
	char *pParam;
	char szCmd[20];
	char szRobot[20];
	char szModule[20];
	int nSlot;
	int nWaferN = 0;
	int nWafSrc = 0;

	// printf( "%s Entered\n", APP_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		CTextParser txtP(pParam);
		txtP.GetWord(szCmd);
		txtP.GetWord(szRobot);
		txtP.GetWord(szModule);
		txtP.GetInt(&nSlot);
		printf("-----> Wafer Sync Forced Change : [%s %s %s %d]\n", szCmd, szRobot, szModule, nSlot);
		_LOG("-----> Wafer Sync Forced Change : [%s %s %s %d]", szCmd, szRobot, szModule, nSlot);

		if(strcmp(szCmd, "SEND") == 0)
		{
			nWaferN = GetWaferNumber(szRobot, 0);
			nWafSrc = GetWaferSource(szRobot, 0);
			if(nWaferN > 0 && nWafSrc >= 0)
			{
				SetWaferNumber(szModule, nSlot, nWaferN);
				SetWaferSource(szModule, nSlot, nWafSrc);
				SetWaferNumber(szRobot, 0, 0);
			}
		}
		else if(strcmp(szCmd, "RECV") == 0)
		{
			nWaferN = GetWaferNumber(szModule, nSlot);
			nWafSrc = GetWaferSource(szModule, nSlot);
			if(nWaferN > 0 && nWafSrc >= 0)
			{
				SetWaferNumber(szRobot, 0, nWaferN);
				SetWaferSource(szRobot, 0, nWafSrc);
				SetWaferNumber(szModule, nSlot, 0);
			}
		}
	} while(0);
	// printf( "%s Leaved\n", APP_NAME);
	return SYS_SUCCESS;	
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	dyn_InitIOList();
	return TRUE;
}

} // End of [extern "C"]
//==========================================================================================
