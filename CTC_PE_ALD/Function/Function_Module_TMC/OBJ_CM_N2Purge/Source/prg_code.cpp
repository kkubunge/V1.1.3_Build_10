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
//#include <vector>

using namespace std;

//------------------------------------------------------------------------------------------
#define APP_NAME	"CM_N2Purge"

#define ALARM_N2_BLOW_CONTROL_FAIL	140
#define ALARM_N2_PURGE_CONTROL_FAIL	141

#define ALARM_N2_POINTER			300

int nModule = 0;

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{"IO_Driver_Loding"				, _K_D_IO, IO_DRIVER_LODING			, 0}, 	
	//... 2017.06.21
	//... 2017.11.08 Update kmj
	//	4 Nozzle
	{"PRMD_LP$_N2Mode"				, _K_D_IO	, PRMD_N2Mode			, 0},
	{"PRMA_LP$_Pre_Nozzle1"			, _K_A_IO	, PRMA_Pre_Nozzle1		, 0},
	{"PRMA_LP$_Pre_Nozzle2"			, _K_A_IO	, PRMA_Pre_Nozzle2		, 0},
	{"PRMD_LP$_Pre_ExhNozzle"		, _K_D_IO	, PRMD_Pre_ExhNozzle	, 0},
	{"PRMA_LP$_Pre_Time"			, _K_A_IO	, PRMA_Pre_Time			, 0},
	{"PRMA_LP$_T_Pre_Nozzle1"		, _K_A_IO	, PRMA_TPre_Nozzle1		, 0},
	{"PRMA_LP$_T_Pre_Nozzle2"		, _K_A_IO	, PRMA_TPre_Nozzle2		, 0},
	{"PRMD_LP$_T_Pre_ExhNozzle"		, _K_D_IO	, PRMD_TPre_ExhNozzle	, 0},
	{"PRMA_LP$_T_Pre_Time"			, _K_A_IO	, PRMA_TPre_Time		, 0},
	{"PRMA_LP$_Proc_Nozzle1"		, _K_A_IO	, PRMA_Proc_Nozzle1		, 0},
	{"PRMA_LP$_Proc_Nozzle2"		, _K_A_IO	, PRMA_Proc_Nozzle2		, 0},
	{"PRMD_LP$_Proc_ExhNozzle"		, _K_D_IO	, PRMD_Proc_ExhNozzle	, 0},
	{"PRMA_LP$_Proc_Time"			, _K_A_IO	, PRMA_Proc_Time		, 0},	
	{"PRMA_LP$_Post_Nozzle1"		, _K_A_IO	, PRMA_Post_Nozzle1		, 0},
	{"PRMA_LP$_Post_Nozzle2"		, _K_A_IO	, PRMA_Post_Nozzle2		, 0},
	{"PRMD_LP$_Post_ExhNozzle"		, _K_D_IO	, PRMD_Post_ExhNozzle	, 0},
	{"PRMA_LP$_Post_Time"			, _K_A_IO	, PRMA_Post_Time		, 0},
	{"PRMA_LP$_P_Post_Nozzle1"		, _K_A_IO	, PRMA_PPost_Nozzle1	, 0},
	{"PRMA_LP$_P_Post_Nozzle2"		, _K_A_IO	, PRMA_PPost_Nozzle2	, 0},
	{"PRMD_LP$_P_Post_ExhNozzle"	, _K_D_IO	, PRMD_PPost_ExhNozzle	, 0},
	{"PRMA_LP$_P_Post_Time"			, _K_A_IO	, PRMA_PPost_Time		, 0},
	//	2 Nozzle
	{"PRMA_LP$_Pre_2Nozzle1"		, _K_A_IO	, PRMA_Pre_2Nozzle1		, 0},
	{"PRMD_LP$_Pre_2ExhNozzle"		, _K_D_IO	, PRMD_Pre_2ExhNozzle	, 0},
	{"PRMA_LP$_Pre_2Time"			, _K_A_IO	, PRMA_Pre_2Time		, 0},
	{"PRMA_LP$_T_Pre_2Nozzle1"		, _K_A_IO	, PRMA_TPre_2Nozzle1	, 0},
	{"PRMD_LP$_T_Pre_2ExhNozzle"	, _K_D_IO	, PRMD_TPre_2ExhNozzle	, 0},
	{"PRMA_LP$_T_Pre_2Time"			, _K_A_IO	, PRMA_TPre_2Time		, 0},
	{"PRMA_LP$_Proc_2Nozzle1"		, _K_A_IO	, PRMA_Proc_2Nozzle1	, 0},
	{"PRMD_LP$_Proc_2ExhNozzle"		, _K_D_IO	, PRMD_Proc_2ExhNozzle	, 0},
	{"PRMA_LP$_Proc_2Time"			, _K_A_IO	, PRMA_Proc_2Time		, 0},	
	{"PRMA_LP$_Post_2Nozzle1"		, _K_A_IO	, PRMA_Post_2Nozzle1	, 0},
	{"PRMD_LP$_Post_2ExhNozzle"		, _K_D_IO	, PRMD_Post_2ExhNozzle	, 0},
	{"PRMA_LP$_Post_2Time"			, _K_A_IO	, PRMA_Post_2Time		, 0},
	{"PRMA_LP$_P_Post_2Nozzle1"		, _K_A_IO	, PRMA_PPost_2Nozzle1	, 0},
	{"PRMD_LP$_P_Post_2ExhNozzle"	, _K_D_IO	, PRMD_PPost_2ExhNozzle	, 0},
	{"PRMA_LP$_P_Post_2Time"		, _K_A_IO	, PRMA_PPost_2Time		, 0},

	//	Manual Function "SET_MANUAL_FLOW"
	{"PRMD_LP$_Man_Flow_Mode"		, _K_D_IO	, PRMD_Man_Flow_Mode	, 0},
	{"PRMA_LP$_Purge_Nozzle1"		, _K_A_IO	, PRMA_Purge_Nozzle1	, 0},
	{"PRMA_LP$_Purge_Nozzle2"		, _K_A_IO	, PRMA_Purge_Nozzle2	, 0},
	{"PRMD_LP$_Purge_ExhNozzle"		, _K_D_IO	, PRMA_Purge_ExhNozzle	, 0},
	{"PRMA_LP$_Purge_PurgeTime"		, _K_A_IO	, PRMA_Purge_PurgeTime	, 0},
	{"PRMA_LP$_Purge_ElapseTime"	, _K_A_IO	, PRMA_Purge_ElapseTime	, 0},
	{"PRMA_LP$_Purge_IdleTime"		, _K_A_IO	, PRMA_Purge_IdleTime	, 0},
	{"PRMA_LP$_N2_Chk_Delay"		, _K_A_IO	, PRMA_N2_Chk_Delay		, 0},
	{"PRMA_LP$_Nozzle1_WarnTol"		, _K_A_IO	, PRMA_Nozzle1_WarnTol	, 0},
	{"PRMA_LP$_Nozzle1_AbortTol"	, _K_A_IO	, PRMA_Nozzle1_AbortTol	, 0},
	{"PRMA_LP$_Nozzle1_ChkCnt"		, _K_A_IO	, PRMA_Nozzle1_ChkCnt	, 0},
	{"PRMA_LP$_Nozzle2_WarnTol"		, _K_A_IO	, PRMA_Nozzle2_WarnTol	, 0},
	{"PRMA_LP$_Nozzle2_AbortTol"	, _K_A_IO	, PRMA_Nozzle2_AbortTol	, 0},
	{"PRMA_LP$_Nozzle2_ChkCnt"		, _K_A_IO	, PRMA_Nozzle2_ChkCnt	, 0},
	{"PRMA_LP$_MainPrs_LowLmt"		, _K_A_IO	, PRMA_MainPrs_LowLmt	, 0},
	{"PRMA_LP$_MainPrs_HighLmt"		, _K_A_IO	, PRMA_MainPrs_HighLmt	, 0},
	{"PRMA_LP$_MainPrs_ChkCnt"		, _K_A_IO	, PRMA_MainPrs_ChkCnt	, 0},
	{"PRMD_LP$_TPre_Repeat"			, _K_D_IO	, PRMD_TPre_Repeat		, 0},
	{"PRMA_LP$_TPre_OnTime"			, _K_A_IO	, PRMA_TPre_OnTime		, 0},
	{"PRMA_LP$_TPre_OffTime"		, _K_A_IO	, PRMA_TPre_OffTime		, 0},
	//...
	//	DuraPort I/O
	{"LPM$.N2PrgStep"				, _K_D_IO	, PRMD_N2PrgStep		, 0},
	{"LPM$.F_ProperlyPlaceDI"		, _K_D_IO	, PLACEMENT_STS			, 0},
	{"LPM$.FOUP_PresentDI"			, _K_D_IO	, PRESENT_STS			, 0},
	{"LPM$.ClampUnclampXI"			, _K_D_IO	, CLAMP_STS				, 0},

	{"LPM$.N2_BLOW_START"			, _K_D_IO	, N2_BLOW_START			, 0},
	{"LPM$.ManPurgeStatus"			, _K_D_IO	, ManPurgeStatus		, 0},
	{"LPM$.N2PurgeSet"				, _K_D_IO	, N2PurgeSet			, 0},
	//...
	{"LPM$.Request"					, _K_S_IO	, Request				, 0},
	{"LPM$.Response"				, _K_S_IO	, Response				, 0},	
	""


};

//------------------------------------------------------------------------------------------
void ReplaceIOName(char before, char after)
{
	int nIoIndex = 0;
	int nLen;

	while( strcmp(IO_STR_Map_Table[nIoIndex].IO_Name, "") != 0 ) 
	{
		nLen = strlen(IO_STR_Map_Table[nIoIndex].IO_Name);
		
		for( int i=0; i<nLen; i++ ) 
		{
			if( IO_STR_Map_Table[nIoIndex].IO_Name[i] == before ) 
			{
				IO_STR_Map_Table[nIoIndex].IO_Name[i] = after;

				printf("[OBJ_N2Purge Load] Parameter[%d](%s)\n", nIoIndex, IO_STR_Map_Table[nIoIndex].IO_Name);

				break;
			}
		}

		nIoIndex++;
	}
}

void SetN2Purge()
{
	//... 2016.02.23 N2 Purge
	//... 2017.11.08 Update
	char szDataToSend[100] = {0,}, szPreCmd[40] = {0,}, szTPreCmd[40] = {0,}, szProcCmd[40] = {0,};
	char szPostCmd[40] = {0,}, szPPostCmd[40] = {0,};
	int nIOResult;
	int n2PrgStep;
	int n2Mode;	
	int n2IdleTime;
	int nTPreRepeat;
	int nTPreOnTime, nTPreOffTime;

	n2Mode = READ_DIGITAL(PRMD_N2Mode, &nIOResult);
	n2PrgStep = READ_DIGITAL(PRMD_N2PrgStep, &nIOResult);
	if( n2Mode == Nozzle2 ) 
	{
		//... N2 Blow시 Parameter Setting 하면 Fail 되는 현상 보강
		//... N2 Blow Stop 후 Setting 한다.
		if (ON_1 == READ_DIGITAL(N2_BLOW_START, &nIOResult))
		{
			strcpy(szDataToSend, "N2_STOP");
			Sleep(1000);
		}

		strcpy(szDataToSend, "CFG_N2_USE ON");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "CFG_N2_INFO OFF");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "N2_MODE 2");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
	} 
	else if( n2Mode == Nozzle4 ) 
	{
		//... N2 Blow시 Parameter Setting 하면 Fail 되는 현상 보강
		//... N2 Blow Stop 후 Setting 한다.
		if (ON_1 == READ_DIGITAL(N2_BLOW_START, &nIOResult))
		{
			strcpy(szDataToSend, "N2_STOP");
			Sleep(1000);
		}

		strcpy(szDataToSend, "CFG_N2_USE ON");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "CFG_N2_INFO OFF");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "N2_MODE 4");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
	} 
	else if( n2Mode == Auto )
	{
		//... N2 Blow시 Parameter Setting 하면 Fail 되는 현상 보강
		//... N2 Blow Stop 후 Setting 한다.
		if (ON_1 == READ_DIGITAL(N2_BLOW_START, &nIOResult))
		{
			strcpy(szDataToSend, "N2_STOP");
			Sleep(1000);
		}

		strcpy(szDataToSend, "CFG_N2_INFO ON");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "CFG_N2_USE ON");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
	}
	else
	{
		// Suspend N2 or Function Off
		strcpy(szDataToSend, "CFG_N2_USE ON");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "CFG_N2_INFO ON");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "N2_STOP");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);

		// Off N2 Purge
		strcpy(szDataToSend, "CFG_N2_USE OFF");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
		strcpy(szDataToSend, "N2_MODE 0");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		return;
	}
	Sleep(10);

	strcpy(szPreCmd, "CFG_N2_PRE");
	strcpy(szTPreCmd, "CFG_N2_TPRE");
	strcpy(szProcCmd, "CFG_N2_PROC");
	strcpy(szPostCmd, "CFG_N2_POST");
	strcpy(szPPostCmd, "CFG_N2_PPOST");
	
	
	SetPurgeParam(szPreCmd, PRMA_Pre_Nozzle1, PRMA_Pre_Nozzle2, PRMD_Pre_ExhNozzle, PRMA_Pre_Time);	
	SetPurgeParam(szProcCmd, PRMA_Proc_Nozzle1, PRMA_Proc_Nozzle2, PRMD_Proc_ExhNozzle, PRMA_Proc_Time);	
	SetPurgeParam(szPostCmd, PRMA_Post_Nozzle1, PRMA_Post_Nozzle2, PRMD_Post_ExhNozzle, PRMA_Post_Time);
	SetPurgeParam(szTPreCmd, PRMA_TPre_Nozzle1, PRMA_TPre_Nozzle2, PRMD_TPre_ExhNozzle, PRMA_TPre_Time);	  


	strcpy(szPreCmd, "CFG_N2_PRE1");
	strcpy(szTPreCmd, "CFG_N2_TPRE1");
	strcpy(szProcCmd, "CFG_N2_PROC1");
	strcpy(szPostCmd, "CFG_N2_POST1");
	strcpy(szPPostCmd, "CFG_N2_PPOST1");


	SetPurgeParam(szPreCmd, PRMA_Pre_2Nozzle1, 0, PRMD_Pre_2ExhNozzle, PRMA_Pre_2Time);
	SetPurgeParam(szProcCmd, PRMA_Proc_2Nozzle1, 0, PRMD_Proc_2ExhNozzle, PRMA_Proc_2Time);	
	SetPurgeParam(szPostCmd, PRMA_Post_2Nozzle1, 0, PRMD_Post_2ExhNozzle, PRMA_Post_2Time);
	SetPurgeParam(szTPreCmd, PRMA_TPre_2Nozzle1, 0, PRMD_TPre_2ExhNozzle, PRMA_TPre_2Time);	
	SetPurgeParam(szPPostCmd, PRMA_PPost_2Nozzle1, 0, PRMD_PPost_2ExhNozzle, PRMA_PPost_2Time);

	n2IdleTime = (int)READ_ANALOG(PRMA_Purge_IdleTime, &nIOResult);
	if ( n2IdleTime < 0 ) n2IdleTime = 0;
	sprintf(szDataToSend, "CFG_SELF_BLOW %d", n2IdleTime);
	WRITE_STRING(Request, szDataToSend, &nIOResult);
	Sleep(10);

	
	nTPreRepeat = (int)READ_DIGITAL(PRMD_TPre_Repeat, &nIOResult);
	if( nTPreRepeat == ON_1 )
	{
		strcpy(szDataToSend, "CFG_TPRE_MODE ON");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);

		nTPreOnTime = (int)READ_ANALOG(PRMA_TPre_OnTime, &nIOResult);
		if ( nTPreOnTime <= 0 ) nTPreOnTime = 0;
		sprintf(szDataToSend, "CFG_TPRE_ONTIME %d", nTPreOnTime);
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);

		nTPreOffTime = (int)READ_ANALOG(PRMA_TPre_OffTime, &nIOResult);
		if ( nTPreOffTime <= 0 ) nTPreOffTime =0;
		sprintf(szDataToSend, "CFG_TPRE_OFFTIME %d", nTPreOffTime);
		WRITE_STRING(Request, szDataToSend, &nIOResult);
	}
	else if ( nTPreRepeat == OFF_0 )
	{
		strcpy(szDataToSend, "CFG_TPRE_MODE OFF");
		WRITE_STRING(Request, szDataToSend, &nIOResult);
		Sleep(10);
	}

	// DuraPort Configures Save
	strcpy(szDataToSend, "SAVE_ALL");
	WRITE_STRING(Request, szDataToSend, &nIOResult);

	Sleep(100);

	WRITE_DIGITAL(N2PurgeSet, OFF_0, &nIOResult);
}

void SetN2ManPurge()
{
	char szDataToSend[100] = {0,};
	int nIOResult  = 0;
	int nPurgeTime = 0;
	int nTempCnt   = 0;
	double dbCount = 0.0;

	if ( IdleMode_0 == READ_DIGITAL(PRMD_Man_Flow_Mode, &nIOResult) )
	{
		if (( NONEXIST_0 == READ_DIGITAL(PLACEMENT_STS, &nIOResult) ) &&
		    ( NONEXIST_0 == READ_DIGITAL(PRESENT_STS, &nIOResult) )	 &&
		    ( Unclamp_2 == READ_DIGITAL(CLAMP_STS, &nIOResult) ))
		{
			strcpy(szDataToSend, "N2_STOP");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
			Sleep(10);
			SetPurgeParam("CFG_N2_BLOW", PRMA_Purge_Nozzle1, PRMA_Purge_Nozzle2, PRMA_Purge_ExhNozzle, PRMA_Purge_PurgeTime);
			Sleep(50);		
			strcpy(szDataToSend, "N2_BLOW");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
		}
		else
		{
			// ..Alarm Post
			ALARM_POST( ALARM_N2_BLOW_CONTROL_FAIL + (nModule * ALARM_N2_POINTER) );
			strcpy(szDataToSend, "CFG_N2_MANUAL OFF");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
			WRITE_DIGITAL(ManPurgeStatus, OFF_0, &nIOResult);
			return;
		}
	}
	else	// Foup Mode
	{
		if (( EXIST_1 == READ_DIGITAL(PLACEMENT_STS, &nIOResult) ) &&
		    ( EXIST_1 == READ_DIGITAL(PRESENT_STS, &nIOResult) )   &&
		    ( Clamp_1 == READ_DIGITAL(CLAMP_STS, &nIOResult) ))
		{
			strcpy(szDataToSend, "CFG_N2_MANUAL ON");
			Sleep(50);
			SetPurgeParam("N2_PURGE", PRMA_Purge_Nozzle1, PRMA_Purge_Nozzle2, PRMA_Purge_ExhNozzle, PRMA_Purge_PurgeTime);
		}
		else
		{
			// ..Alarm Post
			ALARM_POST( ALARM_N2_PURGE_CONTROL_FAIL + (nModule * ALARM_N2_POINTER) );
			strcpy(szDataToSend, "CFG_N2_MANUAL OFF");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
			WRITE_DIGITAL(ManPurgeStatus, OFF_0, &nIOResult);
			return;
		}
	}

	// Manual Purge가 동작일 경우 Status를 On한다.
	WRITE_DIGITAL(ManPurgeStatus, ON_1, &nIOResult);

	// Blow & Purge 상태 전환 확인.	[LoadPort Idle 상태 확인]
	do 	
	{
		if ( !WAIT_SECONDS(1) )
		{
			strcpy(szDataToSend, "N2_STOP");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
			Sleep(10);
			strcpy(szDataToSend, "CFG_N2_MANUAL OFF");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
			WRITE_DIGITAL(ManPurgeStatus, OFF_0, &nIOResult);
			return;
		}

		if ( Idle_6 != READ_DIGITAL(PRMD_N2PrgStep, &nIOResult) )	break;
		
	} while(1);

	nPurgeTime = (int)READ_ANALOG(PRMA_Purge_PurgeTime, &nIOResult); 
	WRITE_ANALOG(PRMA_Purge_ElapseTime, 0, &nIOResult);

	do 
	{
		if ( !WAIT_SECONDS(0.1) )
		{
			printf("[N2 Purge Function] Manual N2 Purge Stop Target[%d] Count[%d]\n", nPurgeTime, (int)dbCount);
			strcpy(szDataToSend, "N2_STOP");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
			Sleep(100);
			break;
		}

		// Foup 감지 시 Manual Function 탈출
		if (ON_1 == READ_DIGITAL(PLACEMENT_STS, &nIOResult) || ON_1 == READ_DIGITAL(PRESENT_STS, &nIOResult))	break;

		// Error시 탈출
		if (Error_8 == READ_DIGITAL(PRMD_N2PrgStep, &nIOResult))	break;

		if (++nTempCnt >= 10)
		{
			nTempCnt = 0;
			dbCount += 1;
			WRITE_ANALOG(PRMA_Purge_ElapseTime, dbCount, &nIOResult);
		}

		if (nPurgeTime <= (int)dbCount)
		{
			printf("[N2 Purge Function] Manual N2 Purge is Completed Target[%d] Count[%d]\n", nPurgeTime, (int)dbCount);
			strcpy(szDataToSend, "N2_STOP");
			WRITE_STRING(Request, szDataToSend, &nIOResult);
			Sleep(10);
			break;
		}

	} while (1);
	Sleep(50);
	
	strcpy(szDataToSend, "CFG_N2_MANUAL OFF");
	WRITE_STRING(Request, szDataToSend, &nIOResult);

	WRITE_ANALOG(PRMA_Purge_ElapseTime, 0, &nIOResult);

	// Manual Purge가 종료될 경우 Status를 Off한다.
	WRITE_DIGITAL(ManPurgeStatus, OFF_0, &nIOResult);
}

void SetN2ManStop()
{
	char szDataToSend[30] = {0,};
	int nIOResult = 0;

	strcpy(szDataToSend, "N2_STOP");
	WRITE_STRING(Request, szDataToSend, &nIOResult);
	Sleep(10);
	strcpy(szDataToSend, "CFG_N2_MANUAL OFF");
	WRITE_STRING(Request, szDataToSend, &nIOResult);
	Sleep(10);

	// Manual Purge가 종료될 경우 Status를 Off한다.
	WRITE_DIGITAL(ManPurgeStatus, OFF_0, &nIOResult);

	return;
}

void SetPurgeParam(char *cmd, int ioNozzle1, int ioNozzle2, int ioExhNozzle, int ioPurgeTime)
{
	int nIOResult;
	double nozzle1, nozzle2, exhNozzle, purgeTime;
	char szDataToSend[100] = {0,}; 
	
	nozzle1 = READ_ANALOG(ioNozzle1, &nIOResult);
	if (ioNozzle2 != 0)	nozzle2 = READ_ANALOG(ioNozzle2, &nIOResult);
	else				nozzle2 = 0;
	exhNozzle = READ_DIGITAL(ioExhNozzle, &nIOResult);
	purgeTime = READ_ANALOG(ioPurgeTime, &nIOResult);

	sprintf(szDataToSend, "%s %.0f %.0f %d %.0f", cmd, nozzle1, nozzle2, exhNozzle, purgeTime);
	WRITE_STRING(Request, szDataToSend, &nIOResult);

	Sleep(20);
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() 
{
	Module_Status msRet = SYS_SUCCESS;
	char *pParam = PROGRAM_PARAMETER_READ();
	int nIOResult = 0;
	
	printf("Enter %s(%s)\n", APP_NAME, pParam);

	if( strcmp(pParam, "SET_N2_PURGE") == 0 )
	{
		WRITE_DIGITAL(N2PurgeSet, ON_1, &nIOResult);
		SetN2Purge();
		WRITE_DIGITAL(N2PurgeSet, OFF_0, &nIOResult);
	}
	else if( strcmp(pParam, "START_MANUAL_FLOW") == 0 )
	{
		SetN2ManPurge();
	}
	else if( strcmp(pParam, "STOP_MANUAL_FLOW") == 0 )
	{
		SetN2ManStop();
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
	}
}

//---------------------------------------------------------------------------------------
void Program_Enter_Code() 
{
	// Before io driver loading.	
	char *pParam = NULL;
	char replace = 0;

	pParam = PROGRAM_ARGUMENT_READ();	

	printf("[OBJ_N2Purge Load] Parameter(%s)\n", pParam);

	if( strcmp(pParam, "CM1") == 0 ) 
	{
		replace = 'A';
		nModule	= 0;
		printf("[OBJ_N2Purge Load] Parameter(%c)\n", replace);
	} 
	else if( strcmp(pParam, "CM2") == 0 ) 
	{
		replace = 'B';
		nModule = 1;
	}
	else if( strcmp(pParam, "CM3") == 0 ) 
	{
		replace = 'C';
		nModule = 2;
	} 
	else 
	{
		replace = 'A';
		nModule = 0;
	}

	ReplaceIOName('$', replace);	
}

//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() 
{
	// When program loaded, Set the loadport n2 purge
	SetN2Purge();

	return TRUE;
}



//==========================================================================================
