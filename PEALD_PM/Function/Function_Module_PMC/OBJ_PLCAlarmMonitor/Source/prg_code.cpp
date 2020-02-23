///////////////////////////////////////////////////////////////////////////////
// AbortProcess

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
//--------------------------------------------------------------------
#ifndef _DEBUG
// #define _DEBUG
#endif

//------------------------------------------------------------------------------------------
#define APP_NAME	"PLCAlarmMonitor"

#define	MAX_PLC_ALARM_COUNT		160
#define	MAX_PLC_ALARM_CHANNEL	10

//------------------------------------------------------------------------------------------
// Alarm Definition
#define ALARM_PLC_INTERLOCK_ACT				1308

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "PLC_GbIntlckDI",		_K_D_IO	,	PLC_GbIntlckDI		, 0 } ,
	{ "PLC_RstIntLckDO",	_K_D_IO	,	PLC_RstIntLckDO		, 0 } ,

	{ "PLC_Int01AI",		_K_A_IO	,	PLC_Int01AI			, 0 } ,
	{ "PLC_Int02AI",		_K_A_IO	,	PLC_Int02AI			, 0 } ,
	{ "PLC_Int03AI",		_K_A_IO	,	PLC_Int03AI			, 0 } ,
	{ "PLC_Int04AI",		_K_A_IO	,	PLC_Int04AI			, 0 } ,
	{ "PLC_Int05AI",		_K_A_IO	,	PLC_Int05AI			, 0 } ,
	{ "PLC_Int06AI",		_K_A_IO	,	PLC_Int06AI			, 0 } ,
	{ "PLC_Int07AI",		_K_A_IO	,	PLC_Int07AI			, 0 } ,
	{ "PLC_Int08AI",		_K_A_IO	,	PLC_Int08AI			, 0 } ,
	{ "PLC_Int09AI",		_K_A_IO	,	PLC_Int09AI			, 0 } ,
	{ "PLC_Int10AI",		_K_A_IO	,	PLC_Int10AI			, 0 } ,

	{ "ABORT_GAS"  ,		_K_F_IO	,	ABORT_GAS			, 0 } ,

	//... 2015.05.27 Add PLC Interlock Forced ON (Change to PM Enable or Recipe Run)
	{ "EasyCluster_Connect",	_K_D_IO	,	EasyCluster_Connect	, 0 } ,
	{ "VRCP_RunningDI",			_K_D_IO	,	VRCP_RunningDI		, 0 } ,
	{ "PLCIntRemDO",			_K_D_IO	,	PLCIntRemDO			, 0 } ,

	""
};

//------------------------------------------------------------------------------------------
// Thread Type Definition for Using thread

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

enum { eOFFON_OFF,   eOFFON_ON };

enum { enCONSTS_DISCON, enCONSTS_CONNECT, enCONSTS_CONUSE, enCONSTS_CONDIS, enCONSTS_CONDISHW };
enum { enRUNSTS_IDLE,   enRUNSTS_RUN };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Interlock Monitor Thread and Function

typedef struct _PLC_INT_LIST
{
	int nChannel;
	int nBitNum;
	int nAlarm;
	int nAlarmOpt;
} PLC_INT_LIST;
PLC_INT_LIST g_PLCIntList[MAX_PLC_ALARM_COUNT];

//////////////////////////////////////////////////////////////////////////
BOOL CheckPLCInterlockAbortGas()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;
	int		i;
	unsigned int punAlarmCh[10];
	unsigned int unBitInfo;
	unsigned int unBitMask;
	int nAlarmID;

	//... Read Alarm Channel
	for(i = 0; i < MAX_PLC_ALARM_CHANNEL; i++) 
	{
		punAlarmCh[i] = (unsigned int) READ_ANALOG(PLC_Int01AI + i, &nIOStatus);
//		printf("[INFO] PLC Interlock[%d] = %d\n", i, punAlarmCh[i]);
	}

	//... Check Alarm Bit
	for(i = 0; i < MAX_PLC_ALARM_COUNT; i++)
	{
		if(g_PLCIntList[i].nChannel > 0)
		{
			unBitInfo = (unsigned int) punAlarmCh[g_PLCIntList[i].nChannel - 1];
			unBitMask = 0x01;
			unBitMask <<= g_PLCIntList[i].nBitNum;
			if((unBitInfo & unBitMask) == unBitMask)
			{
				nAlarmID = g_PLCIntList[i].nAlarm;
				if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
				
				//... Check AlarmOpt (0:AbortGas 1:Warning)
				if(0 == g_PLCIntList[i].nAlarmOpt)		bRet = TRUE;
			}
		}
	}

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
void PLCInterlockThread()
{
	int nIOStatus;

	BOOL bAlarmPost = FALSE;
	BOOL bAbortGas	= FALSE;

	//... 2015.05.27 Add PLC Interlock Forced ON (Change to PM Enable or Recipe Run)
	BOOL bPLCForceOn = FALSE;

	int	 nOld_Commst  = enRUNSTS_IDLE;
	int	 nNew_Commst  = enRUNSTS_IDLE;

	int	 nOld_VRCPRun = enCONSTS_DISCON;
	int	 nNew_VRCPRun = enCONSTS_DISCON;

	do {
		//////////////////////////////////////////////////////////////////////////
		//... 2015.05.27 Add PLC Interlock Forced ON (Change to PM Enable or Recipe Run)
		bPLCForceOn = FALSE;

		nNew_Commst = READ_DIGITAL(EasyCluster_Connect, &nIOStatus);
		if( (enCONSTS_CONNECT == nNew_Commst || enCONSTS_CONUSE == nNew_Commst) && 
			(enCONSTS_CONNECT != nOld_Commst && enCONSTS_CONUSE != nOld_Commst && enCONSTS_DISCON != nOld_Commst))
		{	//... Changed Standby or CTCinUSE
			bPLCForceOn = TRUE;
		}
		nOld_Commst = nNew_Commst;		//... Keep Last Comm Status
		
		nNew_VRCPRun = READ_DIGITAL(VRCP_RunningDI, &nIOStatus);
		if(enRUNSTS_RUN == nNew_VRCPRun && nOld_VRCPRun != nNew_VRCPRun)
		{	//... Changed Valve Recipe Running
			bPLCForceOn = TRUE;
		}
		nOld_VRCPRun = nNew_VRCPRun;	//... Keep Last Valve Recipe Run Status

		if(bPLCForceOn)
		{
			WRITE_DIGITAL(PLCIntRemDO, eOFFON_ON, &nIOStatus);
			printf("[INFO] PLC INTERLOCK FORCED ON[%d]\n", nIOStatus);

			Sleep(500);
			WRITE_DIGITAL(PLCIntRemDO, eOFFON_OFF, &nIOStatus);
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check PLC Global Interlock
		if(eOFFON_ON == READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus))	// 0:Normal 1:Interlock
		{	//... PLC Alarm ON
			
			//... Check PLC Interlock Item Detail Alarm Post
			if(TRUE == CheckPLCInterlockAbortGas())
			{	//... Abort Gas One
				if(FALSE == bAbortGas)
				{
					printf("[INFO] ABORT_GAS BY PLC INTERLOCK...\n");
					RUN_FUNCTION_FREE(ABORT_GAS, "");
					bAbortGas = TRUE;
				}
			}

			//... Alarm Post
			if(ALM_PAUSED != ALARM_STATUS(ALARM_PLC_INTERLOCK_ACT))
			{
				ALARM_POST(ALARM_PLC_INTERLOCK_ACT);
				bAlarmPost = TRUE;
			}
		}
		else
		{	//... PLC Alarm OFF
			bAbortGas = FALSE;		//... Init Abort Gas Flag
		}
		
		//////////////////////////////////////////////////////////////////////////
		//... Check Alarm Recovery Action
		if(bAlarmPost)
		{
			//... 2015.05.18 Wait Alarm Recovery Case by User
			Sleep(3000);

			if(ALM_PAUSED != ALARM_STATUS(ALARM_PLC_INTERLOCK_ACT))
			{	//... After Alarm Clear

				printf("[INFO] PLC INTERLOCK RESET by Alarm Clear\n");

				WRITE_DIGITAL(PLC_RstIntLckDO, eOFFON_ON, &nIOStatus);
				Sleep(500);
				WRITE_DIGITAL(PLC_RstIntLckDO, eOFFON_OFF, &nIOStatus);
				Sleep(1000);
				
				bAlarmPost = FALSE;
			}
		}
		else
		{
			Sleep(1000);
		}

	} while(1);

	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Initialize Function
#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\PMCConfig.ini"

//////////////////////////////////////////////////////////////////////////
void ParsePLCInterlockList(char *szData)
{
	int nListNo, nIdx;
	int nChannel;
	int nBitNum;
	int nAlarm;
	int	nAlarmOpt;

	CTextParser tp(szData);

	do {
		if (!tp.GetInt(&nListNo))		break;
		if (!tp.GetInt(&nChannel))		break;
		if (!tp.GetInt(&nBitNum))		break;
		if (!tp.GetInt(&nAlarm))		break;
		if (!tp.GetInt(&nAlarmOpt))		break;

		if (nListNo < 1 || nListNo > MAX_PLC_ALARM_COUNT)
		{
			printf("-----> PLC Interlock List[%d] Configuration is Invalid\n", nListNo);
			break;
		}

		nIdx = nListNo - 1;
		g_PLCIntList[nIdx].nChannel		= nChannel;
		g_PLCIntList[nIdx].nBitNum		= nBitNum;
		g_PLCIntList[nIdx].nAlarm		= nAlarm;
		g_PLCIntList[nIdx].nAlarmOpt	= nAlarmOpt;	// 0:AbortGas 1:Warning

// 		printf("[PLC Alarm] No[%02d] Ch[%d] Bit[%02d] Alarm[%04d] Opt[%d]\n",
// 			nListNo, nChannel, nBitNum, nAlarm, nAlarmOpt);

	} while(0);
}

//////////////////////////////////////////////////////////////////////////
void InitPmcConfigData()
{
	for(int i = 0; i < MAX_PLC_ALARM_COUNT; i++)
	{
		g_PLCIntList[i].nChannel	= 0;
		g_PLCIntList[i].nBitNum		= 0;
		g_PLCIntList[i].nAlarm		= 0;
		g_PLCIntList[i].nAlarmOpt	= 0;	// 0:AbortGas 1:Warning
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL ReadPmcConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(PMC_CONFIG_FILE, "rt");
		if(fp == NULL)						break;
		if(NULL == fgets(szRead, 255, fp))	break;

		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("PLC Interlock List", szItem) == 0)		nTitle = 1;
				else												nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParsePLCInterlockList(szRead);  break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);

		fclose(fp);
		bRet = TRUE;

	} while(0);

	if(bRet == FALSE)	printf("[ERROR] Config File Reading Failed : %s\n", PMC_CONFIG_FILE);
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Initialize()
{
	BOOL bRet = TRUE;

	printf("Start Initializing in %s\n", APP_NAME);

	//... Init PLC List Data
	InitPmcConfigData();

	//... Read Configuration File
	ReadPmcConfig();

	//... PLC Interlock Monitoring
	_beginthread((_THREAD_FUNC)PLCInterlockThread, 0, NULL);

	return bRet;
}

//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;

	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		// Deinitialize
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
		
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	BOOL	bRet = FALSE;
	
	bRet = Initialize();
	
	return bRet;
}

} // End of [extern "C"]
//==========================================================================================
