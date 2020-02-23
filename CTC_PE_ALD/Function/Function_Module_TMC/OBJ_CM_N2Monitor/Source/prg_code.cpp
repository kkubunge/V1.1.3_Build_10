///////////////////////////////////////////////////////////////////////////////

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

#include <string>
#include <vector>

using namespace std;

//------------------------------------------------------------------------------------------
#define APP_NAME	"CM_N2Monitor"

#define ALARM_N2_FRONTABROT_TOL	131
#define ALARM_N2_REARABROT_TOL	132
#define ALARM_N2_FRONTWARN_TOL	133
#define ALARM_N2_REARWARN_TOL	134

#define ALARM_N2_MAINPRS_LOW	135
#define ALARM_N2_MAINPRS_HIGH	136	

#define ALARM_N2_PURGE_ABNORMAL_STOP	137

#define ALARM_N2_POINTER		300

int nModule = 0;

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{"IO_Driver_Loding"	, _K_D_IO, IO_DRIVER_LODING	, 0}, 	
	//... 2017.06.21
	//... 2017.11.08 Update kmj
	{"PRMD_LP$_N2Mode"				, _K_D_IO	, PRMD_N2Mode			, 0},
	// 4 Nozzle
	// Front MFC
	{"PRMA_LP$_Pre_Nozzle1"			, _K_A_IO	, PRMA_Pre_Nozzle1		, 0},
	{"PRMA_LP$_T_Pre_Nozzle1"		, _K_A_IO	, PRMA_TPre_Nozzle1		, 0},
	{"PRMA_LP$_Proc_Nozzle1"		, _K_A_IO	, PRMA_Proc_Nozzle1		, 0},
	{"PRMA_LP$_Post_Nozzle1"		, _K_A_IO	, PRMA_Post_Nozzle1		, 0},
	{"PRMA_LP$_P_Post_Nozzle1"		, _K_A_IO	, PRMA_PPost_Nozzle1	, 0},
	// Rear MFC
	{"PRMA_LP$_Pre_Nozzle2"			, _K_A_IO	, PRMA_Pre_Nozzle2		, 0},
	{"PRMA_LP$_T_Pre_Nozzle2"		, _K_A_IO	, PRMA_TPre_Nozzle2		, 0},
	{"PRMA_LP$_Proc_Nozzle2"		, _K_A_IO	, PRMA_Proc_Nozzle2		, 0},
	{"PRMA_LP$_Post_Nozzle2"		, _K_A_IO	, PRMA_Post_Nozzle2		, 0},
	{"PRMA_LP$_P_Post_Nozzle2"		, _K_A_IO	, PRMA_PPost_Nozzle2	, 0},
	// Exhaust Valve
	{"PRMD_LP$_Pre_ExhNozzle"		, _K_D_IO	, PRMD_Pre_ExhNozzle	, 0},
	{"PRMD_LP$_T_Pre_ExhNozzle"		, _K_D_IO	, PRMD_TPre_ExhNozzle	, 0},
	{"PRMD_LP$_Proc_ExhNozzle"		, _K_D_IO	, PRMD_Proc_ExhNozzle	, 0},
	{"PRMD_LP$_Post_ExhNozzle"		, _K_D_IO	, PRMD_Post_ExhNozzle	, 0},
	{"PRMD_LP$_P_Post_ExhNozzle"	, _K_D_IO	, PRMD_PPost_ExhNozzle	, 0},
	// Purge Time
	{"PRMA_LP$_Pre_Time"			, _K_A_IO	, PRMA_Pre_Time			, 0},	
	{"PRMA_LP$_T_Pre_Time"			, _K_A_IO	, PRMA_TPre_Time		, 0},	
	{"PRMA_LP$_Proc_Time"			, _K_A_IO	, PRMA_Proc_Time		, 0},	
	{"PRMA_LP$_Post_Time"			, _K_A_IO	, PRMA_Post_Time		, 0},	
	{"PRMA_LP$_P_Post_Time"			, _K_A_IO	, PRMA_PPost_Time		, 0},
	// 2 Nozzle
	// Front MFC
	{"PRMA_LP$_Pre_2Nozzle1"		, _K_A_IO	, PRMA_Pre_2Nozzle1		, 0},
	{"PRMA_LP$_T_Pre_2Nozzle1"		, _K_A_IO	, PRMA_TPre_2Nozzle1	, 0},
	{"PRMA_LP$_Proc_2Nozzle1"		, _K_A_IO	, PRMA_Proc_2Nozzle1	, 0},
	{"PRMA_LP$_Post_2Nozzle1"		, _K_A_IO	, PRMA_Post_2Nozzle1	, 0},
	{"PRMA_LP$_P_Post_2Nozzle1"		, _K_A_IO	, PRMA_PPost_2Nozzle1	, 0},
	// Exhaust Valve
	{"PRMD_LP$_Pre_2ExhNozzle"		, _K_D_IO	, PRMD_Pre_2ExhNozzle	, 0},
	{"PRMD_LP$_T_Pre_2ExhNozzle"	, _K_D_IO	, PRMD_TPre_2ExhNozzle	, 0},
	{"PRMD_LP$_Proc_2ExhNozzle"		, _K_D_IO	, PRMD_Proc_2ExhNozzle	, 0},
	{"PRMD_LP$_Post_2ExhNozzle"		, _K_D_IO	, PRMD_Post_2ExhNozzle	, 0},
	{"PRMD_LP$_P_Post_2ExhNozzle"	, _K_D_IO	, PRMD_PPost_2ExhNozzle	, 0},
	// Purge Time
	{"PRMA_LP$_Pre_2Time"			, _K_A_IO	, PRMA_Pre_2Time		, 0},	
	{"PRMA_LP$_T_Pre_2Time"			, _K_A_IO	, PRMA_TPre_2Time		, 0},	
	{"PRMA_LP$_Proc_2Time"			, _K_A_IO	, PRMA_Proc_2Time		, 0},	
	{"PRMA_LP$_Post_2Time"			, _K_A_IO	, PRMA_Post_2Time		, 0},	
	{"PRMA_LP$_P_Post_2Time"		, _K_A_IO	, PRMA_PPost_2Time		, 0},
	//	MFC Warn/Abort Band
	{"PRMA_LP$_Nozzle1_WarnTol"		, _K_A_IO	, PRMA_Nozzle1_WarnTol	, 0},
	{"PRMA_LP$_Nozzle1_AbortTol"	, _K_A_IO	, PRMA_Nozzle1_AbortTol	, 0},
	{"PRMA_LP$_Nozzle1_ChkCnt"		, _K_A_IO	, PRMA_Nozzle1_ChkCnt	, 0},
	{"PRMA_LP$_Nozzle2_WarnTol"		, _K_A_IO	, PRMA_Nozzle2_WarnTol	, 0},
	{"PRMA_LP$_Nozzle2_AbortTol"	, _K_A_IO	, PRMA_Nozzle2_AbortTol	, 0},
	{"PRMA_LP$_Nozzle2_ChkCnt"		, _K_A_IO	, PRMA_Nozzle2_ChkCnt	, 0},
	// Main Pressure Warn/Abort Band
	{"PRMA_LP$_MainPrs_LowLmt"		, _K_A_IO	, PRMA_MainPrs_LowLmt	, 0},
	{"PRMA_LP$_MainPrs_HighLmt"		, _K_A_IO	, PRMA_MainPrs_HighLmt	, 0},
	{"PRMA_LP$_MainPrs_ChkCnt"		, _K_A_IO	, PRMA_MainPrs_ChkCnt	, 0},
	// Real Air I/O
	{"LPM$.N2Flow1"					, _K_A_IO	, PRMA_N2Flow1			, 0},
	{"LPM$.N2Flow2"					, _K_A_IO	, PRMA_N2Flow2			, 0},
	{"LPM$.N2MainPrs"				, _K_A_IO	, PRMA_N2MainPrs		, 0},
	{"LPM$.N2ExhPrs"				, _K_A_IO	, PRMA_N2ExhPrs			, 0},
	{"LPM$.OxygenDensity"			, _K_A_IO	, PRMA_OxygenDensity	, 0},
	// Real I/O
	{"LPM$.FoupDoor"				, _K_D_IO	, DOOR_CLOSE			, 0},
	{"LPM$.UndockDO"				, _K_D_IO	, UNDOCK_STS			, 0},
	{"LPM$.DockDO"					, _K_D_IO	, DOCK_STS				, 0},
	{"LPM$.F_ProperlyPlaceDI"		, _K_D_IO	, PLACEMENT_STS			, 0},
	{"LPM$.FOUP_PresentDI"			, _K_D_IO	, PRESENT_STS			, 0},
	{"LPM$.Foup_ExistDI"			, _K_D_IO	, FOUP_EXIST_STS		, 0},
	// Status I/O
	{"LPM$.N2_PRE_START"			, _K_D_IO	, N2_PRE_START			, 0},
	{"LPM$.N2_PROC_START"			, _K_D_IO	, N2_PROC_START			, 0},
	{"LPM$.N2_POST_START"			, _K_D_IO	, N2_POST_START			, 0},
	{"LPM$.N2_TPRE_START"			, _K_D_IO	, N2_TPRE_START			, 0},
	{"LPM$.N2_PPOST_START"			, _K_D_IO	, N2_PPOST_START		, 0},
	// ...
	{"LPM$.N2_PRE_END"				, _K_D_IO	, N2_PRE_END			, 0},
	{"LPM$.N2_PROC_END"				, _K_D_IO	, N2_PROC_END			, 0},
	{"LPM$.N2_POST_END"				, _K_D_IO	, N2_POST_END			, 0},
	{"LPM$.N2_TPRE_END"				, _K_D_IO	, N2_TPRE_END			, 0},
	{"LPM$.N2_PPOST_END"			, _K_D_IO	, N2_PPOST_END			, 0},
	// ...
	{"LPM$.N2_PRE_STOP"				, _K_D_IO	, N2_PRE_STOP			, 0},
	{"LPM$.N2_PROC_STOP"			, _K_D_IO	, N2_PROC_STOP			, 0},
	{"LPM$.N2_POST_STOP"			, _K_D_IO	, N2_POST_STOP			, 0},
	{"LPM$.N2_TPRE_STOP"			, _K_D_IO	, N2_TPRE_STOP			, 0},
	{"LPM$.N2_PPOST_STOP"			, _K_D_IO	, N2_PPOST_STOP			, 0},
	// ...
	{"LPM$.N2_PRE_WARNING"			, _K_D_IO	, N2_PRE_WARNING		, 0},	
	{"LPM$.N2_PROC_WARNING"			, _K_D_IO	, N2_PROC_WARNING		, 0},	
	{"LPM$.N2_POST_WARNING"			, _K_D_IO	, N2_POST_WARNING		, 0},	
	{"LPM$.N2_TPRE_WARNING"			, _K_D_IO	, N2_TPRE_WARNING		, 0},	
	{"LPM$.N2_PPOST_WARNING"		, _K_D_IO	, N2_PPOST_WARNING		, 0},
	// ...
	{"LPM$.N2_BLOW_START"			, _K_D_IO	, N2_BLOW_START			, 0},
	{"LPM$.N2_BLOW_END"				, _K_D_IO	, N2_BLOW_END			, 0},
	{"LPM$.N2_BLOW_STOP"			, _K_D_IO	, N2_BLOW_STOP			, 0},
	{"LPM$.N2_BLOW_WARNING"			, _K_D_IO	, N2_BLOW_WARNING		, 0},
	{"LPM$.N2_PURGE_START"			, _K_D_IO	, N2_PURGE_START		, 0},
	{"LPM$.N2_PURGE_END"			, _K_D_IO	, N2_PURGE_END			, 0},
	{"LPM$.N2_PURGE_STOP"			, _K_D_IO	, N2_PURGE_STOP			, 0},
	{"LPM$.N2_PURGE_WARNING"		, _K_D_IO	, N2_PURGE_WARNING		, 0},
	// ...
	{"LPM$.FoupSts"					, _K_D_IO	, PRMD_N2FOUPSts		, 0},
	{"PRMA_LP$_Purge_ElapseTime"	, _K_A_IO	, PRMA_Purge_ElapseTime	, 0},
	{"PRMA_LP$_N2_Chk_Delay"		, _K_A_IO	, PRMA_PRM_CHECK_DELAY	, 0},
	// ...
	{"LPM$.N2PrgStep"				, _K_D_IO	, PRMA_N2PrgStep		, 0},
	{"LPM$.ErrorResetDO"			, _K_D_IO	, RESET_CTRL			, 0},
	{"PRMD_LP$_MODE_STATUS"			, _K_D_IO	, PRMD_MODE_STATUS		, 0},
	// ...
	{"LPM$.Request"					, _K_S_IO	, Request				, 0},
	{"LPM$.Response"				, _K_S_IO	, Response				, 0},
	{"LPM$.PurgeEventID"			, _K_S_IO	, PurgeEventID			, 0},
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
				break;
			}
		}

		nIoIndex++;
	}
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() 
{
	char Tbuffer[9];
	SYSTEMTIME	st;
	HANDLE hThread = 0;
	DWORD dwThreadID = 0;
	Module_Status msRet = SYS_SUCCESS;
	int	nIOIndex = 0;
	char *pParam = PROGRAM_PARAMETER_READ();	
	
	_strtime( Tbuffer );
	GetSystemTime( &st );
	printf("[%s.%03d] : Enter %s(%s)\n", Tbuffer, st.wMilliseconds, APP_NAME, pParam);

	for (nIOIndex =0; nIOIndex < 5; nIOIndex++)
	{
		m_pN2LPStsIO->nStepStartIO = N2_PRE_START	+ nIOIndex;
		m_pN2LPStsIO->nStepEndIO   = N2_PRE_END		+ nIOIndex;	
		m_pN2LPStsIO->nStepStopIO  = N2_PRE_STOP	+ nIOIndex;
		m_pN2LPStsIO->nStepWarnIO  = N2_PRE_WARNING + nIOIndex;
	}

	hThread = CreateThread(NULL, 0, StartMonitor, NULL, 0, &dwThreadID);
	if (NULL == hThread)	return SYS_ABORTED;
	CloseHandle(hThread);

	hThread = CreateThread(NULL, 0, StartPrsMonitor, NULL, 0, &dwThreadID);
	if (NULL == hThread)	return SYS_ABORTED;
	CloseHandle(hThread);

	// SET_IO_TIMEOUT(5); => Wings API 질문...?

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
	char *pParam;
	char replace;

	pParam = PROGRAM_ARGUMENT_READ();	

	printf("[OBJ_N2Purge Load] Parameter(%s)\n", pParam);

	if( strcmp(pParam, "CM1") == 0 ) 
	{
		replace = 'A';
		nModule = 0;
		strcpy(m_szModuleName, "LPA");
	} 
	else if( strcmp(pParam, "CM2") == 0 ) 
	{
		replace = 'B';
		nModule	= 1;
		strcpy(m_szModuleName, "LPB");
	}
	else if( strcmp(pParam, "CM3") == 0 ) 
	{
		replace = 'C';
		nModule	= 2;
		strcpy(m_szModuleName, "LPC");
	} 
	else 
	{
		replace = 'A';
		nModule	= 0;
		strcpy(m_szModuleName, "LPA");
	}

	ReplaceIOName('$', replace);	
}

//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() 
{
	// When program loaded, Set the loadport n2 purge Monitor

	for (int i = 0; i < 5; i++)
	{
		memset(m_pN2LPStsIO, 0, sizeof(N2LPStsIO));
	}
	for (int j = 0; j < 5; j++)
	{
		memset(m_pN2LPInfo[j], 0, sizeof(N2LPInfo) * 5);
	}

	memset(m_szModuleName, 0, sizeof(m_szModuleName));

	return TRUE;
}



//==========================================================================================
void Read_Parameter()
{
	int nIOResult = 0;
	int nIndex = 0;

	// 4 Nozzle
	for (nIndex = 0; nIndex < 5; nIndex++)
	{
		m_pN2LPInfo[Nozzle4_1][nIndex].dbFrontMFCSet		= READ_DIGITAL(PRMA_Pre_Nozzle1		 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].dbRearMFCSet			= READ_DIGITAL(PRMA_Pre_Nozzle2		 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].dbPurgeTime			= READ_DIGITAL(PRMA_Pre_Time		 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].dbFrontMFCWarnTol	= READ_DIGITAL(PRMA_Nozzle1_WarnTol	 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].dbFrontMFCAbortTol	= READ_DIGITAL(PRMA_Nozzle1_AbortTol + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].nFrontErrorCnt		= READ_DIGITAL(PRMA_Nozzle1_ChkCnt	 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].dbRearMFCWarnTol		= READ_DIGITAL(PRMA_Nozzle2_WarnTol	 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].dbRearMFCAbortTol	= READ_DIGITAL(PRMA_Nozzle2_AbortTol + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle4_1][nIndex].nRearErrorCnt		= READ_DIGITAL(PRMA_Nozzle2_ChkCnt	 + nIndex, &nIOResult);
	}
	// 2 Nozzle
	for (nIndex = 0; nIndex < 5; nIndex++)
	{
		m_pN2LPInfo[Nozzle2_0][nIndex].dbFrontMFCSet		= READ_DIGITAL(PRMA_Pre_2Nozzle1		 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle2_0][nIndex].dbRearMFCSet			= 0;
		m_pN2LPInfo[Nozzle2_0][nIndex].dbPurgeTime			= READ_DIGITAL(PRMA_Pre_2Time		 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle2_0][nIndex].dbFrontMFCWarnTol	= READ_DIGITAL(PRMA_Nozzle1_WarnTol	 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle2_0][nIndex].dbFrontMFCAbortTol	= READ_DIGITAL(PRMA_Nozzle1_AbortTol + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle2_0][nIndex].nFrontErrorCnt		= READ_DIGITAL(PRMA_Nozzle1_ChkCnt	 + nIndex, &nIOResult);
		m_pN2LPInfo[Nozzle2_0][nIndex].dbRearMFCWarnTol		= 0;
		m_pN2LPInfo[Nozzle2_0][nIndex].dbRearMFCAbortTol	= 0;
		m_pN2LPInfo[Nozzle2_0][nIndex].nRearErrorCnt		= 0;
	}
}

void IsMFCMonOK(int nFoupSts)
{	
	int nIOResult		= 0;
	int nTimeCnt		= 0;
	int nTimeTemp		= 0;
	int nFrontAbrotCnt	= 0;
	int nFrontWarnCnt	= 0;
	int nRearAbortCnt	= 0;
	int nRearWarnCnt	= 0;
	int nCheckDelay		= 0;
	double dbValue		= 0.0;
	double dbDiff		= 0.0;
	double dbWarnTol	= 0.0;
	double dbAbortTol	= 0.0;
	char szAlarmDec[256] = {0,};
	Module_Status msRet = SYS_SUCCESS;

	printf("[%s N2 Monitor]  ==> IsMFCMonOK Step[%d] Start \n", m_szModuleName, m_nEventInfo);

	if (nFoupSts == Nozzle4_1)
	{
		if((int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCSet == 0 &&
		   (int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCSet == 0)			return;
	}
	else
	{
		if(m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCSet == 0)	return;
	}

	// Stable Time
	nCheckDelay = (int)READ_ANALOG(PRMA_PRM_CHECK_DELAY, &nIOResult);
	nTimeCnt = nCheckDelay;

	if (nCheckDelay <= 0)
	{
		nCheckDelay = 1;
		nTimeCnt = 1;
	}

	if (nCheckDelay >= (int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbPurgeTime &&
		0 != (int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbPurgeTime)
	{
		nCheckDelay = (int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbPurgeTime;
	}

	printf("[%s N2 Monitor]  ==> IsMFCMonOK Step[%d] DelayTime[%d] \n", m_szModuleName, m_nEventInfo, nCheckDelay);

	if ( !WAIT_SECONDS(nCheckDelay * 1) )	return;

	Read_Parameter();
	
	while (TRUE)
	{
		// 상위 Function에서 중단 명령
		if ( !WAIT_SECONDS(0.1) )
		{
			printf("[%s N2 Monitor]  ==> IsMFCMonOK Abnormal Stop\n");
			break;
		}

		// 경과 시간 화면에 표시 [ 1초 단위로 Time Count 위해 ]
		nTimeTemp++;
		if(nTimeTemp >= 10)
		{
			nTimeTemp = 0;
			nTimeCnt++;
			WRITE_ANALOG(PRMA_Purge_ElapseTime, nTimeCnt, &nIOResult);
		}

		// 설정된 시간 만큼 진행?
		if ((nTimeCnt + nCheckDelay) >= (int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbPurgeTime &&
			0 != (int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbPurgeTime)
		{
			printf("[%s N2 Monitor]  ==> IsMFCMonOK Step[%d] Time Over[%d] Set Time[%d] \n", 
				m_szModuleName, m_nEventInfo, (nTimeCnt + nCheckDelay), m_pN2LPInfo[nFoupSts][m_nEventInfo].dbPurgeTime);
			break;
		}

		// 종료?
		if (OFF_0 == READ_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepStartIO, &nIOResult))	break;

		// Front MFC Check
		if ((int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCSet != 0)
		{
			dbValue		= READ_ANALOG(PRMA_N2Flow1, &nIOResult);
			dbDiff		= fabs(m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCSet - dbValue);
			dbWarnTol	= m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCSet * m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCWarnTol / 100;
			dbAbortTol	= m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCSet * m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCAbortTol / 100;

			if (dbAbortTol < dbDiff)	 nFrontWarnCnt++;
			else if (dbWarnTol < dbDiff) nFrontAbrotCnt++;
			else
			{
				nFrontWarnCnt = 0;
				nFrontAbrotCnt = 0;
			}
			
			if (SIMULATION_MODE == TRUE)	WRITE_ANALOG(PRMA_N2Flow1, m_pN2LPInfo[nFoupSts][m_nEventInfo].dbFrontMFCSet, &nIOResult);

			if (nFrontWarnCnt > m_pN2LPInfo[nFoupSts][m_nEventInfo].nFrontErrorCnt * 10)
			{
				sprintf(szAlarmDec, "Diff: %0.1f, WarnTol: %0.1f, Cnt: %d", dbDiff, dbWarnTol, nFrontWarnCnt);
				break;
			}
			else if (nFrontAbrotCnt > m_pN2LPInfo[nFoupSts][m_nEventInfo].nFrontErrorCnt * 10)
			{
				sprintf(szAlarmDec, "Diff: %0.1f, AbortTol: %0.1f, Cnt: %d", dbDiff, dbAbortTol, nFrontAbrotCnt);
				break;
			}
		}

		if (nFoupSts == Nozzle2_2) continue;

		// Rear MFC Check
		if ((int)m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCSet != 0)
		{
			dbValue		= READ_ANALOG(PRMA_N2Flow2, &nIOResult);
			dbDiff		= fabs(m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCSet - dbValue);
			dbWarnTol	= m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCSet * m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCWarnTol / 100;
			dbAbortTol	= m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCAbortTol * m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCAbortTol / 100;

			if (dbAbortTol < dbDiff)	 nFrontAbrotCnt++;
			else if (dbWarnTol < dbDiff) nFrontWarnCnt++;	
			else
			{
				nFrontWarnCnt = 0;
				nFrontAbrotCnt = 0;
			}

			if (SIMULATION_MODE == TRUE)
			{
				WRITE_ANALOG(PRMA_N2Flow2, m_pN2LPInfo[nFoupSts][m_nEventInfo].dbRearMFCSet, &nIOResult);
			}
			
			if (nRearAbortCnt > m_pN2LPInfo[nFoupSts][m_nEventInfo].nRearErrorCnt * 10)
			{
				sprintf(szAlarmDec, "Diff: %0.1f, WarnTol: %0.1f, Cnt: %d", dbDiff, dbWarnTol, nFrontWarnCnt);
				break;
			}
			else if (nRearWarnCnt > m_pN2LPInfo[nFoupSts][m_nEventInfo].nRearErrorCnt * 10)
			{
				sprintf(szAlarmDec, "Diff: %0.1f, AbrotTol: %01.f, Cnt: %d", dbDiff, dbAbortTol, nFrontAbrotCnt);
				break;
			}
		}
	}

	// Alarm Post
	if (nFrontAbrotCnt > (m_pN2LPInfo[nFoupSts][m_nEventInfo].nFrontErrorCnt * 10))
	{
		AlarmMessageAttach( ALARM_N2_FRONTABROT_TOL + (nModule * ALARM_N2_POINTER), szAlarmDec );
		ALARM_POST( ALARM_N2_FRONTABROT_TOL + (nModule * ALARM_N2_POINTER) );
	}
	else if (nFrontWarnCnt > (m_pN2LPInfo[nFoupSts][m_nEventInfo].nFrontErrorCnt * 10))
	{
		AlarmMessageAttach( ALARM_N2_FRONTWARN_TOL + (nModule * ALARM_N2_POINTER), szAlarmDec );
		ALARM_POST( ALARM_N2_FRONTWARN_TOL + (nModule * ALARM_N2_POINTER) );
	}
	else if (nRearAbortCnt > (m_pN2LPInfo[nFoupSts][m_nEventInfo].nRearErrorCnt * 10))
	{
		AlarmMessageAttach( ALARM_N2_REARABROT_TOL + (nModule * ALARM_N2_POINTER), szAlarmDec );
		ALARM_POST( ALARM_N2_REARABROT_TOL + (nModule * ALARM_N2_POINTER) );
	}
	else if (nRearAbortCnt > (m_pN2LPInfo[nFoupSts][m_nEventInfo].nRearErrorCnt * 10))
	{
		AlarmMessageAttach( ALARM_N2_REARWARN_TOL + (nModule * ALARM_N2_POINTER), szAlarmDec );
		ALARM_POST( ALARM_N2_REARWARN_TOL + (nModule * ALARM_N2_POINTER) );
	}
}

void IsMainPressureOK()
{	
	int nIOResult	   = 0;
	int nTimeCnt	   = 0;
	int nTimeTemp	   = 0;
	int nErrorLowCnt   = 0;
	int nErrorHighCnt  = 0;
	int nErrorCnt	   = 0;
	double dbValue	   = 0;
	double dbDiff	   = 0;
	double dbLowLimit  = 0;
	double dbHighLimit = 0;
	char szAlarmDec[256] = {0,};
	
	while (TRUE)
	{
		// 상위 Function 중단 명령?
		if (!WAIT_SECONDS(0.1))	break;
		
		nTimeCnt++;
		if(nTimeCnt >= 10)
		{
			dbLowLimit  = READ_ANALOG(PRMA_MainPrs_LowLmt, &nIOResult);
			dbHighLimit = READ_ANALOG(PRMA_MainPrs_HighLmt, &nIOResult);
			nErrorCnt	= (int)READ_ANALOG(PRMA_MainPrs_ChkCnt, &nIOResult);
			nTimeCnt = 0;
		}
		
		dbValue = READ_ANALOG(PRMA_N2MainPrs, &nIOResult);
		
		if (m_bIsMonitorStart == FALSE)	break;

		if (dbLowLimit > dbValue)		nErrorLowCnt++;
		else if (dbHighLimit < dbValue) nErrorHighCnt++;
		else
		{
			nErrorLowCnt = 0;
			nErrorHighCnt = 0;
		}

		if (nErrorLowCnt > (nErrorCnt * 10))
		{
			sprintf(szAlarmDec, "Cur: %0.1f, LowLimit: %0.1f ErrorCnt %d LimitCnt %d", dbValue, dbLowLimit, nErrorLowCnt, nErrorCnt);
			break;
		}
		else if (nErrorHighCnt >> (nErrorCnt * 10))
		{
			sprintf(szAlarmDec, "Cur: %0.1f, LowLimit: %0.1f ErrorCnt %d LimitCnt %d", dbValue, dbLowLimit, nErrorLowCnt, nErrorCnt);
			break;
		}
	}

	// Alarm Post
	if (nErrorLowCnt > nErrorCnt)	
	{
		AlarmMessageAttach(ALARM_N2_MAINPRS_LOW + (nModule * ALARM_N2_POINTER) , szAlarmDec);
		ALARM_POST( ALARM_N2_MAINPRS_LOW + (nModule * ALARM_N2_POINTER) );
	}
	else if (nErrorHighCnt > nErrorCnt)
	{
		AlarmMessageAttach(ALARM_N2_MAINPRS_HIGH + (nModule * ALARM_N2_POINTER) , szAlarmDec);
		ALARM_POST( ALARM_N2_MAINPRS_HIGH + (nModule * ALARM_N2_POINTER) );
	}

	return;
}

BOOL IsProcessStart()
{
	int nIOResult = 0;
	int nCheckCnt = 0;
	int nIsEventOn = 0;
	int nIndex = 0;
	BOOL bRet = FALSE;

	if (!m_bIsMonitorStart)
	{
		Sleep(500);
		return FALSE;
	}
	
	printf("[%s N2 Monitor]  ==> IsProcessStart\n", m_szModuleName);

	do 
	{
		for(nIndex = 0; nIndex < 5; nIndex++)
		{
			nIsEventOn = READ_DIGITAL(m_pN2LPStsIO[nIndex].nStepStartIO, &nIOResult);
			if (nIsEventOn == ON_1)	break;
			Sleep(100);
		}

		if(nIsEventOn == ON_1)
		{
			bRet = TRUE;
			m_nEventInfo = nIndex;
			printf("[%s N2 Monitor]  ==> IsProcessStart Step[%d]\n", m_szModuleName, m_nEventInfo);
			break;
		}

		if ( !WAIT_SECONDS(0.1) )
		{
			bRet = FALSE;
			break;
		}

	} while (1);
	
	if (bRet == FALSE)	return bRet;

	// ReCheck 
	do 
	{
		nIsEventOn = READ_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepStartIO, &nIOResult);

		if (nIsEventOn == ON_1)
		{
			printf("[%s N2 Monitor]  ==> IsProcessStart ReCheck IO Step[%d]\n", m_szModuleName, m_nEventInfo);
			return TRUE;
		}
		else
		{
			Sleep(1000);
			nCheckCnt++;
		}

	} while (nCheckCnt <= 5);

	return bRet;
}

void IsProcComplete()
{
	int nIOResult = 0;
	int nCheckCnt = 0;
	int nIsEventOn = 0;

	do 
	{
		if (SIMULATION_MODE == TRUE)
		{
			WRITE_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepEndIO, ON_1, &nIOResult);
			WRITE_ANALOG(PRMA_N2Flow1, 0, &nIOResult);
			WRITE_ANALOG(PRMA_N2Flow2, 0, &nIOResult);
		}

		nIsEventOn = READ_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepEndIO, &nIOResult);
		
		if ( nIsEventOn == ON_1 )
		{
			printf("[%s N2 Monitor] ==> IsProcComplete Step[%d] \n", m_szModuleName, m_nEventInfo);
			return;
		}
		else
		{
			if (ON_1 == READ_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepStopIO, &nIOResult) ||
				ON_1 == READ_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepWarnIO, &nIOResult))
			{
				printf("[%s N2 Monitor]  ==> IsProcComplete Alarm Step[%d]	Stop[%d] Warn[%d] \n", 
					m_szModuleName, m_nEventInfo, READ_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepStopIO, &nIOResult), READ_DIGITAL(m_pN2LPStsIO[m_nEventInfo].nStepWarnIO, &nIOResult));				

				// Alarm Post
				ALARM_POST( ALARM_N2_PURGE_ABNORMAL_STOP + (nModule * ALARM_N2_POINTER)	);
			}
		}

	} while (0);

}

int WhatIsFoup()
{
	int nIOResult = 0;
	int nIOStatus = Unknown_0;
	int nRet = Unknown_2;

	if (SIMULATION_MODE == TRUE)	WRITE_DIGITAL(PRMD_N2FOUPSts, Nozzle4_3, &nIOResult);

	nIOStatus = READ_DIGITAL(PRMD_N2FOUPSts, &nIOResult);
	printf("[%s N2 Monitor]  ==> WhatIsFoup Step[%d] RealFoupSts[%d] \n", m_szModuleName, m_nEventInfo, &nIOStatus);

	if (nIOStatus == Nozzle4_3)			nRet = Nozzle4_1;
	else if (nIOStatus == Nozzle2_2)	nRet = Nozzle2_0;
	else								nRet = Unknown_2;

	return	nRet;
}

void AlarmMessageAttach(int AlarmID, char *AlarmAttach)
{
	char Buffer[256] = {0,};

	ALARM_MESSAGE_GET( AlarmID, Buffer );
	strcat(Buffer, AlarmAttach);
	ALARM_MESSAGE_SET( AlarmID, Buffer );
}

DWORD WINAPI StartMonitor(LPVOID pVoid)
{
	int nIOStatus = 0;
	int nFoupSts = 0;
	int nInterval = 0;
	int nElapseTimeCnt = 0;
	int nTempCnt = 0;

	while (TRUE)
	{
		Sleep(500);

		// Parameter Read
		if (nInterval = 0)
		{
			// 30초마다 한번 씩 N2 Status Error 상태 시, Auto LPM Reset Sequence 수행
			if ( Error_8 == READ_DIGITAL(PRMA_N2PrgStep, &nIOStatus) )
			{
				if (( ON_1 != READ_DIGITAL(PLACEMENT_STS, &nIOStatus))		  &&
				    ( ON_1 != READ_DIGITAL(PRESENT_STS, &nIOStatus)))
				{
					WRITE_DIGITAL(RESET_CTRL, Reset_1, &nIOStatus);
				}

				Read_Parameter();
			}
		}

		if (nInterval >= 60)	nInterval = 0;
		else					nInterval++;

		// Check LPM Mode (NotUse:0	=> Skip)
		if (NotUse_0 == READ_DIGITAL(PRMD_N2Mode, &nIOStatus))	continue;
		
		// Check Monitor if Start or SKip
		if (!IsProcessStart())	continue;

		// Check FOUP Status
		nFoupSts = WhatIsFoup();
		printf("[%s N2 Monitor]	==> WhatIsFoup Step[%d] FoupSts[%d]\n", m_szModuleName, m_nEventInfo, nFoupSts);

		if ( nFoupSts == Unknown_2 )
		{
			m_bIsMonitorStart = FALSE;
			m_nEventInfo	  =	Pre_Step_0;	
			continue;
		}
			
		// Start MFC Monitor
		IsMFCMonOK(nFoupSts);

		// Check Complete
		IsProcComplete();
		continue;
		
	}

	return 0;
}

DWORD WINAPI StartPrsMonitor(LPVOID pVoid)
{
	int nIOResult = 0;
	
	while (TRUE)
	{
		Sleep(500);

		if (NotUse_0 == READ_DIGITAL(PRMD_N2Mode, &nIOResult))	continue;

		if (FALSE == m_bIsMonitorStart)	continue;

		// Start Main Pressure Monitor
		IsMainPressureOK();
		continue;		
	}

	return 0;
}


void Event_Message_Received()
{
	char TBuffer[9];
	SYSTEMTIME st;
	char *pParam = PROGRAM_EVENT_READ();

	_strtime(TBuffer);
	GetSystemTime(&st);
	printf("[%s.%03d] : SEQ_EVT[%s] Start[%s]\n", TBuffer, st.wMilliseconds, APP_NAME, pParam);

	if (pParam != NULL)
	{
		if( strcmp(pParam, "N2_MONITOR_START") == 0 )
		{
			m_bIsMonitorStart = TRUE;
		}
		else if( strcmp(pParam, "N2_MONITOR_STOP") == 0 )
		{
			m_bIsMonitorStart = FALSE;
			m_nEventInfo	  = Pre_Step_0;
		}
	}

	printf("[%s.03d] : SEQ_EVT[%s] m_bIsMonitorStart[%d]\n", TBuffer, st.wMilliseconds, APP_NAME, m_bIsMonitorStart);
}