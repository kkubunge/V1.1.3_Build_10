///////////////////////////////////////////////////////////////////////////////
// StgHtr Monitor
// Create : 2004.08.18

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>
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
#define FUNC_NAME	"HeatExchanger"
#define HTE_NO      1
//------------------------------------------------------------------------------------------
#define ALARM_HTE_SETTEMP_CTRL_FAIL		1090
#define ALARM_HTE2_SETTEMP_CTRL_FAIL	1091	
#define ALARM_HTE_SETTEMP_CHECK_FAIL	1092
#define ALARM_HTE2_SETTEMP_CHECK_FAIL	1093					
//------------------------------------------------------------------------------------------
// Data Type

typedef struct _HEATER_STRUCT
{
	double dbSetPoint;		
	BOOL bCheck;
} HEATER_STRUCT;
HEATER_STRUCT	g_HtrList[HTE_NO];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_GbIntlckDM"	, _K_D_IO	,	SW_GbIntlckDM,		0	} ,
	{ "SW_ModuleFailDM"	, _K_D_IO	,	SW_ModuleFailDM,	0	} ,

	{ "HTE_SrtStpDO"	, _K_D_IO	,	HTE_StaStpDIO,		0	} ,
	{ "HTE_SetTempAO"	, _K_A_IO	,	HTE_SetTempAIO,		0	} ,
	{ "HTE_ReadTempAI"	, _K_A_IO	,	HTE_ReadTempAI,		0	} ,
	{ "HTE_SrtStpDI"	, _K_D_IO	,	HTE_StaStpDI,		0	} ,
	""
};

//------------------------------------------------------------------------------------------

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

//2012.01.03
enum { HTR_SET_STOP, HTR_SET_START, HTR_SETTING, HTR_WAITING, HTR_SET_END, HTR_SET_FAILED };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int g_nHtrSet_Thread = THREAD_STOP;
int g_nHtrSet_Status = HTR_SET_END;
int g_dbHtrTempLimit_Offset = 0;

//////////////////////////////////////////////////////////////////////////
void HtrSet_Thread()
{
	enum { HTE_STOP = 0, HTE_RUN = 1 };
	enum { LOCAL = 0, REMOTE = 1 };
	enum { HTE_NOTUSE, HTE_USE };
	enum { HCTRL_STABLE, HCTRL_SETTEMP, HCTRL_UNSTABLE };
	enum { NORMAL = 0 , ALARM = 1};
	
	int    nIOStatus;
	int    nHtrCnt;
	double dbNowSp;

	do {
		g_nHtrSet_Status = HTR_SETTING;
		for(nHtrCnt = 0; nHtrCnt < HTE_NO; nHtrCnt++)
		{
			if(g_HtrList[nHtrCnt].dbSetPoint  > 0){
				dbNowSp = READ_ANALOG(HTE_SetTempAIO + nHtrCnt,&nIOStatus);
				if(g_HtrList[nHtrCnt].dbSetPoint != dbNowSp){
					WRITE_ANALOG(HTE_SetTempAIO + nHtrCnt, g_HtrList[nHtrCnt].dbSetPoint, &nIOStatus);
				}
				Sleep(500);
				if(HTE_STOP == READ_DIGITAL(HTE_StaStpDIO + nHtrCnt,&nIOStatus)){
					WRITE_DIGITAL(HTE_StaStpDIO + nHtrCnt,HTE_RUN, &nIOStatus);
				}
			}else if(g_HtrList[nHtrCnt].dbSetPoint == 0){
					WRITE_DIGITAL(HTE_StaStpDIO + nHtrCnt,HTE_STOP, &nIOStatus);
			}

			if(! nIOStatus)	{
				ALARM_POST(ALARM_HTE_SETTEMP_CTRL_FAIL + nHtrCnt); 
				g_nHtrSet_Status = HTR_SET_FAILED; break;
			}
		}

		if(g_nHtrSet_Status != HTR_SETTING) {g_nHtrSet_Status = HTR_SET_FAILED; break;}
		
		g_nHtrSet_Status = HTR_WAITING;
		do{
			for(nHtrCnt = 0; nHtrCnt < HTE_NO; nHtrCnt++)
			{
				if(g_HtrList[nHtrCnt].dbSetPoint > 0){
					if(g_HtrList[nHtrCnt].dbSetPoint - 5 < READ_ANALOG(HTE_ReadTempAI + nHtrCnt, &nIOStatus) 
						&& g_HtrList[nHtrCnt].dbSetPoint + 5 > READ_ANALOG(HTE_ReadTempAI + nHtrCnt, &nIOStatus))
						g_nHtrSet_Status = HTR_SET_END;
				}else if(g_HtrList[nHtrCnt].dbSetPoint == 0){
					if(HTE_STOP == READ_DIGITAL(HTE_StaStpDIO + nHtrCnt,&nIOStatus))
						g_nHtrSet_Status = HTR_SET_END;
				}
			}	
			if(! nIOStatus)	{
				ALARM_POST(ALARM_HTE_SETTEMP_CTRL_FAIL + nHtrCnt); 
				g_nHtrSet_Status = HTR_SET_FAILED; break;
			}
			Sleep(100);
		}while(g_nHtrSet_Status == HTR_WAITING);
		Sleep(500);
		if(g_nHtrSet_Status == HTR_SET_STOP || g_nHtrSet_Status == HTR_SET_FAILED) break;
	} while(g_nHtrSet_Status != HTR_SET_END);

	g_nHtrSet_Thread = THREAD_STOP;
	_endthread();
}

BOOL StartHeaterSetting()
{
	g_nHtrSet_Status = HTR_SET_STOP;
	while(g_nHtrSet_Thread == THREAD_RUN)
	{
		g_nHtrSet_Status = HTR_SET_STOP;
		if(! WAIT_SECONDS(1)) return FALSE;
	}

	g_nHtrSet_Thread = THREAD_RUN;
	g_nHtrSet_Status = HTR_SET_START;
	_beginthread((_THREAD_FUNC) HtrSet_Thread, 0, NULL);
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status SetTemp(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nHtETOP = 0;
	int nHtEBTM = 0;
	double dbSetPointTOP;

	CTextParser tp(szParam);
	do {
		if(! tp.GetDbFloat(&dbSetPointTOP)) break;
		g_HtrList[0].dbSetPoint = dbSetPointTOP;
	} while(! tp.IsEnd());
	
	if(! StartHeaterSetting()) msRet = SYS_ABORTED;
	return msRet;
}

Module_Status CheckTemp(char *szParam)
{
	Module_Status msRet = SYS_ABORTED;

	do {
		if(g_nHtrSet_Status != HTR_SET_END) break;
		msRet = SYS_SUCCESS;
	} while(0);
	return msRet;
}

Module_Status StopControl(char *szParam)
{
	g_nHtrSet_Status = HTR_SET_STOP;
	while(g_nHtrSet_Thread == THREAD_RUN)
	{
		g_nHtrSet_Status = HTR_SET_STOP;
		if(! WAIT_SECONDS(1)) { return SYS_ABORTED; }
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//==========================================================================================

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
	char szCmd[41], szSubParam[1024];

	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if(nParLen <= 0)
		{
			printf("Invalid Parameter in %s\n", FUNC_NAME);
			msRet = SYS_ABORTED;
			break;
		}
		else
		{
			STR_SEPERATE(pParam , szCmd, szSubParam, 900 );
			if     (strcmp(szCmd, "HT_EXCHG_SETTEMP") == 0)		msRet = SetTemp(szSubParam);
			else if(strcmp(szCmd, "CHECK_TEMP") == 0)	msRet = CheckTemp(szSubParam);
			else if(strcmp(szCmd, "STOP") == 0)			msRet = StopControl(szSubParam);
			else{
				printf("Invalid Parameter in %s\n", FUNC_NAME);
				msRet = SYS_ABORTED;
				break;
			}
		}
	} while(0);

	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	int i;

	//-------------------------------------------------------------
	char szParam[256];
	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	#ifdef _DEBUG
	printf("Argument = %s in Program_Enter_Code [Heat Exchanger Control]\n", szParam);
	#endif
	if(strlen(szParam) > 0) g_dbHtrTempLimit_Offset = atoi(szParam);
	else g_dbHtrTempLimit_Offset = 0;
	//-------------------------------------------------------------
	for(i = 0; i < HTE_NO; i++) 
	{
		g_HtrList[i].dbSetPoint = 0; 
		g_HtrList[i].bCheck = FALSE; 
	}
	//-------------------------------------------------------------
}


} // End of [extern "C"]
//==========================================================================================
