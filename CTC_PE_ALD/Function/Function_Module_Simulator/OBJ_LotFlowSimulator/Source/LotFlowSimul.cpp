// LotFlowSimul.cpp: implementation of the CLotFlowSimul class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

//--------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//--------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "Timer.h"
#include "TextParser.h"
#include "DynamicIO.h"
#include "LotFlowSimul.h"

//--------------------------------------------------------------------
#define SEQ_FILE_NAME	"Function\\Function_Module_Simulator\\LotFlowSimul_Seq.txt"
#define SEQ_FILE_PATH	"Function\\Function_Module_Simulator"

#define INFINITE_LOOP	999999

#define ASCII_TAB		0x09
#define ASCII_ENTER		0x0d
#define ASCII_CR		0x0d
#define ASCII_LF		0x0a

//--------------------------------------------------------------------
enum { LPM_Idle, LPM_Running, LPM_Pausing, LPM_Paused, LPM_NA5, LPM_Waitting, LPM_WaitHandOff };

enum {
	PMC_Unuse, PMC_Local, PMC_DisCon,  PMC_Standby, PMC_Maint,
	PMC_Uninit, PMC_EnablePM, PMC_CTCInUse, PMC_CTCDis, PMC_CTCDis2,
	PMC_CTCEnaPM, PMC_CTCDisHW, PMC_CTCDisHW2
};

enum { EN_LPM1, EN_LPM2, EN_LPM3, EN_LPM4 };
enum { CW_UNKNOWN, CW_ABSENT, CW_PRESENT, CW_PROCESS, CW_FAILURE };

enum { EN_TM1, EN_TM2 };

//--------------------------------------------------------------------
enum { PRO_STOP, PRO_RUNNING };
enum RecipeTypeEnum
{
	RT_Unknown,
	RT_ProcRcp,
	RT_PreProRcp,
	RT_PostProRcp,
	RT_IdleRcp,
	RT_CleanRcp,
	RT_PurgeRcp,
	RT_LotPreRcp,
	RT_LotPostRcp
};

//--------------------------------------------------------------------
enum	{ OFF,		ON };
enum	{ DISABLE,	ENABLE };
enum	{ UNCLAMP,	CLAMP,	UNKNOWN1, ERROR1 };
enum	{ OUT1,		IN1,	UNKNOWN2, ERROR2 };
enum	{ DOWNED,	UPPED,	UNKNOWN3, ERROR3 };

//--------------------------------------------------------------------
char *EVENT_STRING_LOAD_COMPLETE		= "EVENT_ON LOAD_COMPLETE_F PORT%d";
char *EVENT_STRING_CLAMP_COMPLETE		= "EVENT_ON LL_CLAMP_COMPLETE PORT%d";
char *EVENT_STRING_UNLOAD_COMPLETE		= "EVENT_ON UNLOAD_COMPLETE_F PORT%d";
char *EVENT_STRING_UNCLAMP_COMPLETE		= "EVENT_ON LL_UNCLAMP_COMPLETE PORT%d";
char *EVENT_STRING_STS_UNOCCUPIED		= "EVENT_ON STS_UNOCCUPIED_F PORT%d";
char *EVENT_STRING_CARR_DELETED			= "EVENT_ON CARR_DELETED_F PORT%d";
char *EVENT_STRING_STS_DELETED			= "EVENT_ON STS_DELETED_F PORT%d";
char *EVENT_STRING_CARR_NOTASSOCIATED	= "EVENT_ON CARR_NOTASSOCIATED_F PORT%d";
char *EVENT_STRING_TR_LOAD_RECV			= "EVENT_ON LOAD_TB_F PORT%d";
char *EVENT_STRING_TR_LOAD_RECV2		= "EVENT_ON LOAD_TB2_F CM";
char *EVENT_STRING_TR_UNLOAD_RECV		= "EVENT_ON UNLOAD_TB_F PORT%d";
char *EVENT_LOAD_REQUEST				= "EVENT_ON LOAD_REQUEST_F PORT%d";
char *EVENT_UNLOAD_REQUEST				= "EVENT_ON UNLOAD_REQUEST_F PORT%d";
char *EVENT_STRING_UNDOCKING			= "EVENT_ON LL_UNDOCKING_COMPLETE CM";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLotFlowSimul::CLotFlowSimul()
{
	int i;
	for(i=0; i<STOP_ALARM_LIST_MAX; i++) m_anStopAlarmList[i] = -1;

	m_bStopCon_SchStop = TRUE;
	m_bStopCon_Alarm = FALSE;
	m_bStopCon_IO = FALSE;
	m_bAbortAllLot = FALSE;
	m_bStopCon_Unload_Lock = TRUE;			// 2011.08.23
}

CLotFlowSimul::~CLotFlowSimul()
{

}


//////////////////////////////////////////////////////////////////////
// Thread

void CLotFlowSimul::DoMonitor()	/* Default Monitor Function */
{
	int nLoopCount = 0;
	char szLine[256];
	char szCmd[40];
	char szParam[256];

	CTextParser txtP_Seq;
	CTextParser txtP_Line;

	printf_LOG("LotFlowSimul Started\n");

	m_bLoopStart = FALSE;
	m_bLoopEnd = FALSE;
	txtP_Seq.SetText(m_szSequence);
	m_szLoopStart = m_szSequence;

	do {
		if(m_bLoopEnd)
		{
			nLoopCount++;
			printf_LOG("---> Lot Flow Finished [Count=%d]\n", nLoopCount);
			_LOG("--------------------------------------------------------\n");
			if(nLoopCount >= m_nTargetLoopCount) break;
			m_bLoopEnd = FALSE;
			txtP_Seq.SetText(m_szLoopStart);
			continue;
		}

		if(! txtP_Seq.GetLine(szLine)) break;
		if(strlen(szLine) == 0 || '#' == szLine[0] || ';' == szLine[0]
				|| ' ' == szLine[0] || ASCII_TAB == szLine[0]) continue;

		txtP_Line.SetText(szLine);
		txtP_Line.GetWord(szCmd);
		txtP_Line.GetString(szParam);

		if(! ExecFunction(szCmd, szParam)) break;
		if(m_bLoopStart)
		{
			m_szLoopStart = txtP_Seq.GetCurrPosition();
			m_bLoopStart = FALSE;
		}

		if(! CheckSimStopCondition())
		{
			printf_LOG("-----> Simulation Stop Condition Matched\n");
			break;
		}
	} while(WaitSec());

	if(m_bAbortAllLot)
	{
		AbortAllLot();
	}

	printf_LOG("LotFlowSimul Finished [LoopCount = %d]\n", nLoopCount);
}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CLotFlowSimul::Initialize()
{
	CSeqInterface::Initialize();

	return TRUE;
}

Module_Status CLotFlowSimul::Main()
{
	Module_Status msRet = SYS_SUCCESS;
	char szParam[80], szCmd[40], szSubParam[40];
	CTextParser tp;

	strcpy(szParam, PROGRAM_PARAMETER_READ());
	tp.SetText(szParam);

	do {
		tp.GetWord(szCmd);
		tp.GetString(szSubParam);

		if(tp.Comp(szCmd, "START_SIM") || tp.Comp(szCmd, "START"))
		{
			StartSimulation(szSubParam);
		}
		else if(tp.Comp(szCmd, "STOP_SIM") || tp.Comp(szCmd, "STOP"))
		{
			StopSimulation();
		}
		else
		{
			if(! ExecFunction(szCmd, szSubParam)) msRet = SYS_ABORTED;
		}

	} while(0);

	return msRet;
}

void CLotFlowSimul::Enter_Code()
{
	char szParam[256];

	strcpy(szParam, PROGRAM_ARGUMENT_READ());


}

//////////////////////////////////////////////////////////////////////
BOOL CLotFlowSimul::ReadSequence()
{
	BOOL bRet = FALSE;
	int nCount = 0;
	FILE *fp;

	do {
		printf_LOG("-----> Sequence File Name : %s\n", m_szSeqFileName);
		fp = fopen(m_szSeqFileName, "rt");
		if(fp == NULL) break;
		while(! feof(fp))
		{
			m_szSequence[nCount] = fgetc(fp);
			nCount++;
			if(nCount >= SEQ_MAX_SIZE) break;
		}
		fclose(fp);
		m_szSequence[nCount] = NULL;
		printf_LOG("-----> Read Sequence File [Size=%d]\n", nCount);

		bRet = TRUE;
	} while(0);
	
	if(! bRet) printf_LOG("-----> Sequence File Reading Failed !\n");
	return bRet;
}

void CLotFlowSimul::StartSimulation(char *szParam)
{
	int i;
	char szSeqFileName[80] = "";			// 2011.08.23

	CTextParser txtP;
	txtP.SetText(szParam);
	txtP.GetWord(szSeqFileName);			

	m_nTargetLoopCount = INFINITE_LOOP;

	for(i=0; i<STOP_ALARM_LIST_MAX; i++) m_anStopAlarmList[i] = -1;

	do {
		_LOG("--------------------------------------------------------\n");
		if(IsThreadRunning())
		{
			printf_LOG("-----> Simulation Is Running Already !\n");
			printf_LOG("-----> START_SIM Failed !\n");
			break;
		}

		// 2011.08.23
		if(strlen(szSeqFileName) <= 2) strcpy(m_szSeqFileName, SEQ_FILE_NAME);
		else
		{
			sprintf(m_szSeqFileName,"%s\\%s", SEQ_FILE_PATH, szSeqFileName);
		}

		if(! ReadSequence()) break;
		CreateThread();
	} while(0);
}

void CLotFlowSimul::StopSimulation()
{
	DestroyThread();
}

BOOL CLotFlowSimul::ExecFunction(char *szCmd, char *szParam)
{
	BOOL bRet = FALSE;
	int nValue;
	CTextParser txtP;

	do {
		if(txtP.BeEqual(szCmd, "LOAD_FOUP"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! LoadFoup(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "UNLOAD_FOUP"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! UnloadFoup(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "START_LOT"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! StartLot(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "CANCEL_LOT"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! CancelLot(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "STOP_LOT"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! StopLot(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "WAIT_LOT_END"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! WaitLotEnd(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "WAIT_MIN"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! Wait_Minutes(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "WAIT_SEC"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! Wait_Seconds(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "WRITE_IO"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! Write_IO(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "WAIT_WHILE_IO"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! Wait_While_IO(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "PM_MODE"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! ExchangePMMode(szParam)) break;
		}
		else if(txtP.BeEqual(szCmd, "ABORT_ALL_LOT"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			if(! AbortAllLot()) break;
		}
		else if(txtP.BeEqual(szCmd, "LOOP_END"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			m_bLoopEnd = TRUE;
		}
		else if(txtP.BeEqual(szCmd, "LOOP_START"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			nValue = atoi(szParam);
			if(nValue > 0) m_nTargetLoopCount = nValue;
			m_bLoopStart = TRUE;
		}
		else if(txtP.BeEqual(szCmd, "SET_STOP_CON"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			SetSimulStopCondition(szParam);
		}
		// 2011.08.23
		else if(txtP.BeEqual(szCmd, "MID_CONTROL_ON"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			MID_Control_On(szParam);
		}
		// 2011.08.23
		else if(txtP.BeEqual(szCmd, "MID_CONTROL_OFF"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			MID_Control_Off(szParam);
		}
		// 2011.08.23
		else if(txtP.BeEqual(szCmd, "CLEAR_ALARMS"))
		{
			printf_LOG("-----> %s %s\n", szCmd, szParam);
			ClearAlarms(szParam);
		}		
		else
		{
			printf_LOG("-----> Unknown Command [%s %s]\n", szCmd, szParam);
			break;
		}
		bRet = TRUE;
	} while(0);

	if(! bRet) printf_LOG("-----> Command Failed\n");
	return bRet;
}

BOOL CLotFlowSimul::LoadFoup(char *szParam)
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	char szTemp[40];
	int nLPM;
	int nLPMIdx;
	int nWaferCount = 25;
	int nValue;
	int nCount = 0;;

	CTextParser txtP;

	txtP.SetText(szParam);
	nLPM = szParam[3] - '0';
	nLPMIdx = nLPM - 1;


	txtP.MoveNextWord();
	if(txtP.GetInt(&nValue)) nWaferCount = nValue;

	WRITE_DIGITAL(CM1_CASS_STATUS+nLPMIdx, ON, &bIOStatus);
	WRITE_DIGITAL(CM1_CASS_SenSor_STATUS+nLPMIdx, ON, &bIOStatus);
	WAIT_SECONDS(0.5);

	WRITE_DIGITAL(CM1_CLAMP_STATUS+nLPMIdx, CLAMP, &bIOStatus);
	WRITE_DIGITAL(CM1_CLAMP+nLPMIdx, CLAMP, &bIOStatus);
	WAIT_SECONDS(1);

	sprintf( szTemp , EVENT_STRING_LOAD_COMPLETE, nLPM );
	WRITE_STRING( S_SCHEDULER , szTemp , &bIOStatus );					// 2011.08.23
	WAIT_SECONDS(1);

	WRITE_DIGITAL(CM1_OPEN_STATUS+nLPMIdx, DOWNED, &bIOStatus);
	WAIT_SECONDS(1);

	while(! IsLPMIdle(nLPMIdx))
	{
		if(! WaitSec(1)) break;
		if(! WAIT_SECONDS(1)) break;
		if(++nCount > 100) break;
	}

	sprintf(szTemp, "MAPPING_F CM%d", nLPM);
	WRITE_STRING( S_SCHEDULER , szTemp , &bIOStatus );
	WAIT_SECONDS(10);

	SetWafer(nLPM, nWaferCount);
	WAIT_SECONDS(1);

	sprintf(szTemp, "CONTROLJOB MAPVERIFY|CM%d", nLPM);
	WRITE_STRING( S_SCHEDULER , szTemp , &bIOStatus );

	while(! IsLPMIdle(nLPMIdx))
	{
		if(! WaitSec(1)) break;
		if(! WAIT_SECONDS(1)) break;
		if(++nCount > 100) break;
	}

	return bRet;
}

BOOL CLotFlowSimul::UnloadFoup(char *szParam)
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	char szTemp[40];
	int nLPM;
	int nLPMIdx;

	nLPM = szParam[3] - '0';
	nLPMIdx = nLPM - 1;

	sprintf(szTemp, "CANCEL%d", nLPM);
	WRITE_STRING( S_SCHEDULER , szTemp , &bIOStatus );

	WRITE_DIGITAL(CM1_OPEN_STATUS+nLPMIdx, UPPED, &bIOStatus);
	WAIT_SECONDS(1);

	WRITE_DIGITAL(CM1_CLAMP_STATUS+nLPMIdx, UNCLAMP, &bIOStatus);
	WRITE_DIGITAL(CM1_CLAMP+nLPMIdx, UNCLAMP, &bIOStatus);
	WAIT_SECONDS(1);

	sprintf( szTemp , EVENT_STRING_UNLOAD_COMPLETE, nLPM );
	WRITE_STRING( S_SCHEDULER , szTemp , &bIOStatus );				// 2011.08.23
	WAIT_SECONDS(1);

	WRITE_DIGITAL(CM1_CASS_STATUS+nLPMIdx, OFF, &bIOStatus);
	WRITE_DIGITAL(CM1_CASS_SenSor_STATUS+nLPMIdx, OFF, &bIOStatus);

	return bRet;
}

BOOL CLotFlowSimul::StartLot(char *szParam)
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	char szLPM[40];
	char szJobID[40];
	char szPPID[256];
	int nLPM;
	int nLPMIdx;

	CTextParser txtP;
	txtP.SetText(szParam);
	txtP.GetWord(szLPM);			// LPM1, LPM2, LPM3
	txtP.GetWord(szJobID);
	txtP.GetWord(szPPID);

	nLPM = szLPM[3] - '0';
	nLPMIdx = nLPM - 1;

	WRITE_STRING( CTC_RECIPE_FILE+nLPMIdx , szPPID , &bIOStatus );
	WRITE_STRING( CTC_JID_READ+nLPMIdx , szJobID , &bIOStatus );
	// WRITE_STRING( CTC_MID_NAME+nLPMIdx , szJobID , &bIOStatus );
	WAIT_SECONDS(1);

	switch(nLPMIdx)
	{
	case EN_LPM1: RUN_FUNCTION(GEM_START_CTRL, "START(F)"); break;
	case EN_LPM2: RUN_FUNCTION(GEM_START_CTRL2, "START2(F)"); break;
	case EN_LPM3: RUN_FUNCTION(GEM_START_CTRL3, "START3(F)"); break;
	}
	
	return bRet;
}

BOOL CLotFlowSimul::CancelLot(char *szParam)
{
	BOOL bRet = TRUE;
	char szTemp[40];
	int nLPM;
	BOOL bIOStatus;

	nLPM = szParam[3] - '0';
	sprintf(szTemp, "CANCEL%d", nLPM);
	WRITE_STRING( S_SCHEDULER , szTemp , &bIOStatus );

	return bRet;
}

BOOL CLotFlowSimul::StopLot(char *szParam)
{
	BOOL bRet = TRUE;
	char szTemp[40];
	int nLPM;
	BOOL bIOStatus;

	nLPM = szParam[3] - '0';
	sprintf(szTemp, "STOP%d", nLPM);
	WRITE_STRING( S_SCHEDULER , szTemp , &bIOStatus );

	return bRet;
}

BOOL CLotFlowSimul::FinishLot(char *szParam)
{
	BOOL bRet = TRUE;
	char szTemp[40];
	int nLPM;

	nLPM = szParam[3] - '0';
	sprintf(szTemp, "UNLOAD_STYLE%d 4", nLPM);
	WRITE_FUNCTION_EVENT(SCHEDULER, szTemp);
	WAIT_SECONDS(1);
	sprintf(szTemp, "FINISH%d", nLPM);
	WRITE_FUNCTION_EVENT(SCHEDULER, szTemp);

	return bRet;
}

BOOL CLotFlowSimul::WaitLotEnd(char *szParam)
{
	BOOL bRet = TRUE;
	int nLPM;
	int nLPMIdx;

	nLPM = szParam[3] - '0';
	nLPMIdx = nLPM - 1;

	WAIT_SECONDS(30);
	while(WaitSec(1))
	{
		if(! IsLPMRunning(nLPMIdx)) break;
		if(! WAIT_SECONDS(1)) break;
	}

	return bRet;
}

BOOL CLotFlowSimul::Wait_Minutes(char *szParam)
{
	BOOL bRet = TRUE;
	int nMinutes;
	CTimer tmr;

	nMinutes = atoi(szParam);
	tmr.StartTimer();
	while(tmr.GetProgressTime(UNIT_MINUTE) < nMinutes)
	{
		if(! WaitSec()) { bRet= FALSE; break; }
	}

	return bRet;
}

BOOL CLotFlowSimul::Wait_Seconds(char *szParam)
{
	BOOL bRet = TRUE;
	int nSeconds;

	nSeconds = atoi(szParam);
	WaitSec(nSeconds);

	return bRet;
}

BOOL CLotFlowSimul::Write_IO(char *szParam)
{
	BOOL bRet = TRUE;
	char szChannelName[40];
	char szType[40];
	char szValue[40];
	int nType = _K_D_IO;
	BOOL bIOStatus;

	CDynamicIO dynIO;
	CTextParser txtP;

	txtP.SetText(szParam);
	txtP.GetWord(szType);
	txtP.GetWord(szChannelName);
	txtP.GetWord(szValue);

	if(txtP.BeEqual(szType, "D")) nType = _K_D_IO;
	else if(txtP.BeEqual(szType, "A")) nType = _K_A_IO;
	else if(txtP.BeEqual(szType, "S")) nType = _K_S_IO;

	dynIO.dWRITE_IO_FROM_NAME(szChannelName, nType, szValue, &bIOStatus);

	return bRet;
}

BOOL CLotFlowSimul::Wait_While_IO(char *szParam)
{
	BOOL bRet = TRUE;
	char szChannelName[40];
	char szType[40];
	char szValue[40];
	char szReadValue[40];
	char szLogic[40];
	int nType = _K_D_IO;
	BOOL bIOStatus;
	BOOL bExit = FALSE;

	CDynamicIO dynIO;
	CTextParser txtP;

	txtP.SetText(szParam);
	txtP.GetWord(szType);
	txtP.GetWord(szChannelName);
	txtP.GetWord(szLogic);
	txtP.GetWord(szValue);

	if(txtP.BeEqual(szType, "D")) nType = _K_D_IO;
	else if(txtP.BeEqual(szType, "A")) nType = _K_A_IO;
	else if(txtP.BeEqual(szType, "S")) nType = _K_S_IO;

	while(FALSE == bExit)
	{
		dynIO.dREAD_IO_FROM_NAME(szChannelName, nType, szReadValue, &bIOStatus);

		if(! IsLogicValid(szValue, szLogic, szReadValue)) bExit = TRUE;

		if(! WaitSec(1)) break;
		if(! WAIT_SECONDS(1)) break;
	}

	dynIO.dWRITE_IO_FROM_NAME(szChannelName, nType, szValue, &bIOStatus);

	return bRet;
}

BOOL CLotFlowSimul::ExchangePMMode(char *szParam)
{
	BOOL bRet = TRUE;
	char szCmd[80];
	char szModule[40];
	char szEvent[80];

	CTextParser txtP;
	txtP.SetText(szParam);
	txtP.GetWord(szCmd);
	txtP.GetWord(szModule);

	if(txtP.BeEqual(szCmd, "ENABLE"))
	{
		sprintf(szEvent, "SET_MODULE_INFO_LOCAL %s|ENABLE", szModule);
		WRITE_FUNCTION_EVENT(SCHEDULER, szEvent);
	}
	else if(txtP.BeEqual(szCmd, "DISABLE"))
	{
		sprintf(szEvent, "SET_MODULE_INFO_LOCAL %s|DISABLE", szModule);
		WRITE_FUNCTION_EVENT(SCHEDULER, szEvent);
	}
	else if(txtP.BeEqual(szCmd, "DISSTOP"))
	{
		sprintf(szEvent, "SET_MODULE_INFO_LOCAL %s|DISABLE.STOP", szModule);
		WRITE_FUNCTION_EVENT(SCHEDULER, szEvent);
	}
	else bRet = FALSE;

	return bRet;
}

BOOL CLotFlowSimul::AbortAllLot()
{
	BOOL bRet = TRUE;

	RUN_FUNCTION_ABORT(SCHEDULER);
	printf_LOG("-----> Scheduler Aborted in AbortAllLot\n");

	return bRet;
}

void CLotFlowSimul::SetWafer(int nLPM, int nWaferCount)
{
	int nLPMIdx;
	int nC01_Wafer;
	int nEnd_Wafer;
	BOOL bIOStatus;
	int i;

	nLPMIdx = nLPM - 1;
	switch(nLPMIdx)
	{
	case EN_LPM1: {nC01_Wafer = CM1_C01_Wafer;nEnd_Wafer = CM1_END_SLOTT; break;}
	case EN_LPM2: {nC01_Wafer = CM2_C01_Wafer;nEnd_Wafer = CM2_END_SLOTT; break;}
	case EN_LPM3: {nC01_Wafer = CM3_C01_Wafer;nEnd_Wafer = CM3_END_SLOTT; break;}
	}

	for(i=0; i<25; i++)
	{
		if(i < nWaferCount) WRITE_DIGITAL(nC01_Wafer+i, CW_PRESENT, &bIOStatus);
		else WRITE_DIGITAL(nC01_Wafer+i, CW_ABSENT, &bIOStatus);
	}
	WRITE_DIGITAL(nEnd_Wafer,nWaferCount, &bIOStatus);
}

BOOL CLotFlowSimul::IsSchedulerStoped()
{
	BOOL bRet = FALSE;

	if(FALSE == IsLPMRunning(EN_LPM1) && FALSE == IsLPMRunning(EN_LPM2)
		&& FALSE == IsLPMRunning(EN_LPM3) && FALSE == IsLPMRunning(EN_LPM4)
		&& (TRUE == IsTMWaferPresent(EN_TM1) || TRUE == IsFEMWaferPresent()) 
		)
	{
		bRet = TRUE;
	}
	return bRet;
}

BOOL CLotFlowSimul::IsTMWaferPresent(int nTMIdx)
{
	BOOL bRet = FALSE;
	BOOL bIOStatus;
	int i;

	for(i=0; i<18; i++)
	{
		if(READ_DIGITAL(CTC_BM1_Wafer_Status+i, &bIOStatus) != 0) { bRet = TRUE; break; }
	}

	return bRet;
}

BOOL CLotFlowSimul::IsFEMWaferPresent()
{
	BOOL bRet = FALSE;
	BOOL bIOStatus;
	
	if(0 != READ_DIGITAL(CTC_FA_Wafer_Status, &bIOStatus)) bRet = TRUE;
	if(0 != READ_DIGITAL(CTC_FB_Wafer_Status, &bIOStatus)) bRet = TRUE;
	if(0 != READ_DIGITAL(CTC_FM_AL_Wafer_Status, &bIOStatus)) bRet = TRUE;

	return bRet;
}

BOOL CLotFlowSimul::IsLPMRunning(int nLPMIdx)
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	int nLPM_RunSts;

	nLPM_RunSts = READ_DIGITAL(CM1_MAIN_CONTROL+nLPMIdx, &bIOStatus);
	if(LPM_Running != nLPM_RunSts && LPM_Waitting != nLPM_RunSts
		&& LPM_Pausing != nLPM_RunSts && LPM_Paused != nLPM_RunSts)
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL CLotFlowSimul::IsLPMIdle(int nLPMIdx)
{
	BOOL bRet = FALSE;
	BOOL bIOStatus;
	int nLPM_RunSts;

	nLPM_RunSts = READ_DIGITAL(CM1_MAIN_CONTROL+nLPMIdx, &bIOStatus);
	if(LPM_Idle == nLPM_RunSts) bRet = TRUE;

	return bRet;
}

BOOL CLotFlowSimul::IsLogicValid(char *szValue_1, char *szLogic, char *szValue_2)
{
	BOOL bRet = FALSE;
	double dbValue_1;
	double dbValue_2;
	CTextParser txtP;

	if(txtP.BeEqual(szLogic, "=="))
	{
		if(txtP.BeEqual(szValue_1, szValue_2)) bRet = TRUE;
	}
	else if(txtP.BeEqual(szLogic, "!="))
	{
		if(! txtP.BeEqual(szValue_1, szValue_2)) bRet = TRUE;
	}
	else if(txtP.BeEqual(szLogic, ">"))
	{
		dbValue_1 = atof(szValue_2);
		dbValue_2 = atof(szValue_1);
		if(dbValue_1 > dbValue_2) bRet = TRUE; 
	}
	else if(txtP.BeEqual(szLogic, "<"))
	{
		dbValue_1 = atof(szValue_2);
		dbValue_2 = atof(szValue_1);
		if(dbValue_1 < dbValue_2) bRet = TRUE; 
	}
	else if(txtP.BeEqual(szLogic, ">="))
	{
		dbValue_1 = atof(szValue_2);
		dbValue_2 = atof(szValue_1);
		if(dbValue_1 >= dbValue_2) bRet = TRUE; 
	}
	else if(txtP.BeEqual(szLogic, "<="))
	{
		dbValue_1 = atof(szValue_2);
		dbValue_2 = atof(szValue_1);
		if(dbValue_1 <= dbValue_2) bRet = TRUE; 
	}
	else
	{
		printf_LOG("-----> Invalid Logic [%s]\n", szLogic);
		bRet = FALSE;
	}

	return bRet;
}

BOOL CLotFlowSimul::CheckSimStopCondition()
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	int i;
	char szReadValue[40];
	int nCount;

	CDynamicIO dynIO;

	if(m_bStopCon_SchStop)
	{
		if(IsSchedulerStoped())
		{
			bRet = FALSE;
			printf_LOG("-----> Scheduler Is Stoped\n");
		}
	}

	if(m_bStopCon_Alarm)
	{
		for(i=0; i<STOP_ALARM_LIST_MAX; i++)
		{
			if(m_anStopAlarmList[i] == -1) break;
			if(ALM_PAUSED == ALARM_STATUS(m_anStopAlarmList[i]))
			{
				printf_LOG("-----> Alarm Rised [%d]\n", m_anStopAlarmList[i]);
				bRet = FALSE;
				break;
			}
		}
	}

	if(m_bStopCon_IO)
	{
		dynIO.dREAD_IO_FROM_NAME(m_aStopIoList[0].szChName, m_aStopIoList[0].nType, szReadValue, &bIOStatus);
		if( IsLogicValid(szReadValue, m_aStopIoList[0].szLogic, m_aStopIoList[0].szValue) )
		{
			bRet = FALSE;
			printf_LOG("-----> IO Stop Condition Is Checked [%s %s %s]\n",
						szReadValue, m_aStopIoList[0].szLogic, m_aStopIoList[0].szValue);
		}
	}

	if(m_bStopCon_Unload_Lock)
	{
		for(i=0; i<3; i++)
		{
			if( IsWaferComplete(i) )
			{
				nCount = 0;
				while( IsLPMRunning(i) )
				{
					WAIT_SECONDS(1);
					if(! IsThreadRunning()) break;
					if(++nCount > LPM_UNLOAD_LOCK_ERROR_TM) break;
					{
						bRet = FALSE;
						printf_LOG("-----> LPM%d Unload Lock Error Is Ocurred\n", i+1);
						break;
					}
				}
			}
			if(FALSE == bRet) break;
		}
	}

	return bRet;
}

BOOL CLotFlowSimul::IsWaferComplete(int nLPMIdx)
{
	BOOL bRet = TRUE;
	int nC01_Wafer;
	BOOL bIOStatus;
	int nWaferSts;
	int i;

	switch(nLPMIdx)
	{
	case EN_LPM1: nC01_Wafer = CM1_C01_Wafer; break;
	case EN_LPM2: nC01_Wafer = CM2_C01_Wafer; break;
	case EN_LPM3: nC01_Wafer = CM3_C01_Wafer; break;
	}

	for(i=0; i<25; i++)
	{
		nWaferSts = READ_DIGITAL(nC01_Wafer+i, &bIOStatus);
		if(CW_PROCESS != nWaferSts && CW_FAILURE != nWaferSts) { bRet = FALSE; break; }
	}
	return bRet;
}

BOOL CLotFlowSimul::SetSimulStopCondition(char *szParam)
{
	BOOL bRet = TRUE;
	char szItem[40];
	char szType[40];
	int nAlarmCode;
	int nCount = 0;

	CTextParser txtP;

	txtP.SetText(szParam);
	txtP.GetWord(szItem);

	if(txtP.BeEqual(szItem, "SCHEDULER_STOP"))
	{
		txtP.GetBOOL(&m_bStopCon_SchStop);
	}
	else if(txtP.BeEqual(szItem, "ABORT_ALL_LOT"))
	{
		txtP.GetBOOL(&m_bAbortAllLot);
	}
	else if(txtP.BeEqual(szItem, "UNLOAD_LOCK_ERR"))
	{
		txtP.GetBOOL(&m_bStopCon_Unload_Lock);
	}
	else if(txtP.BeEqual(szItem, "ALARM"))
	{
		while(txtP.GetInt(&nAlarmCode))
		{
			if(nAlarmCode == -1)
			{
				m_bStopCon_Alarm = FALSE;
				break;
			}
			m_anStopAlarmList[nCount] = nAlarmCode;
			nCount++;
			if(nCount >= STOP_ALARM_LIST_MAX) break;
		}
		m_bStopCon_Alarm = TRUE;
	}
	else if(txtP.BeEqual(szItem, "IO"))
	{
		txtP.GetWord(szType);
		if(txtP.BeEqual(szType, "D")) m_aStopIoList[0].nType = _K_D_IO;
		else if(txtP.BeEqual(szType, "A")) m_aStopIoList[0].nType = _K_A_IO;
		else if(txtP.BeEqual(szType, "S")) m_aStopIoList[0].nType = _K_S_IO;

		txtP.GetWord(m_aStopIoList[0].szChName);
		txtP.GetWord(m_aStopIoList[0].szLogic);
		txtP.GetWord(m_aStopIoList[0].szValue);
		m_bStopCon_IO = TRUE;
	}

	return bRet;
}

// 2011.08.23
BOOL CLotFlowSimul::MID_Control_On(char *szParam)
{
	enum { EN_ON, EN_OFF };
	BOOL bRet = TRUE;
	int nLPM;
	int nLPMIdx;
	BOOL bIOStatus;

	nLPM = szParam[3] - '0';
	nLPMIdx = nLPM - 1;
	WRITE_DIGITAL(FC_CM1_MID_CONTROL+nLPMIdx, EN_ON, &bIOStatus);

	return bRet;
}

// 2011.08.23
BOOL CLotFlowSimul::MID_Control_Off(char *szParam)
{
	enum { EN_ON, EN_OFF };
	BOOL bRet = TRUE;
	int nLPM;
	int nLPMIdx;
	BOOL bIOStatus;

	nLPM = szParam[3] - '0';
	nLPMIdx = nLPM - 1;
	WRITE_DIGITAL(FC_CM1_MID_CONTROL+nLPMIdx, EN_OFF, &bIOStatus);

	return bRet;
}


// 2011.08.23
BOOL CLotFlowSimul::ClearAlarms(const char *szParam)
{
	BOOL bRet = TRUE;
	int i;
	int nAlarmID;
	char szModule[40];
	char szAlarmID[80];
	int nModule = 0;

	CTextParser txtP;
	txtP.SetText(szParam);
	txtP.GetWord(szModule);

	if(txtP.BeEqual(szModule, "TM")) nModule = 0;
	else if(txtP.BeEqual(szModule, "PM1")) nModule = 1;
	else if(txtP.BeEqual(szModule, "PM2")) nModule = 2;

	for(i=0; txtP.GetWord(szAlarmID); i++)
	{
		nAlarmID = atoi(szAlarmID);
		if(nAlarmID > 0)
		{
			// if(ALM_PAUSED == ALARM_STATUS(nAlarmID)) ALARM_CLEAR(nAlarmID);
			ALARMRECOVER(nModule, nAlarmID, 0);			// 2011.08.23
		}
	}

	return bRet;
}
