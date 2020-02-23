// ValveSeq.cpp: implementation of the CValveSeq class.
//
//////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <string.h>
//====================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
// #include "iodefine.h"
} // End of [extern "C"]
//====================================================================

#include "ValveSeq.h"
#include "Timer.h"
#include "TextParser.h"

#define DEFAULT_INI_FILE	"Parameter\\PM_SEQ_PARM\\ValveSeq.ini"
#define DEFAULT_SEQ_NAME	"AbortGas"

// extern void _LOG(const char *pFormat, ...);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
enum { VLV_CLOSE, VLV_OPEN };

//--------------------------------------------------------------------
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	RoughVlvDO,
	VALVE_CONTROL,
	MFC_CONTROL,
	PRS_CONTROL,
	ZMOTION_CONTROL,	//... Not Used
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"FastRoughVlvDO",		_K_D_IO, 0},
	{"VALVE_CONTROL",		_K_F_IO, 0},
	{"MFC_CONTROL",			_K_F_IO, 0},
	{"PRS_CONTROL",			_K_F_IO, 0},
	{"CHUCK_CONTROL",		_K_F_IO, 0},	//... Not Used
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValveSeq::CValveSeq()
{
	m_pSeqTimer = new CTimer;
	m_nStepCount = 0;
	m_nValveSeqSts = VSEQ_IDLE;

	strcpy(m_szFilePath, DEFAULT_INI_FILE);
	strcpy(m_szSeqName, DEFAULT_SEQ_NAME);
}

CValveSeq::~CValveSeq()
{
	delete m_pSeqTimer;
}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CValveSeq::Initialize(const char *szSeqName, const char *szFilePath)
{
	BOOL bRet = TRUE;
	int i;

	if(szFilePath != NULL) strcpy(m_szFilePath, szFilePath);
	if(szSeqName != NULL) strcpy(m_szSeqName, szSeqName);

	for(i=0; i<MAX_STEP; i++)
	{
		m_adbStepTime[i] = 0;
		m_anMFCNo[i][0] = 0;
		m_anMFCNo[i][1] = 0;
		m_anMFCNo[i][2] = 0;
		m_adbTVSet[i] = -1;
		m_adbZMSet[i][0] = -1;
		m_adbZMSet[i][1] = -1;
		strcpy(m_aszValveStr[i], "");
		strcpy(m_aszCtrlCmd[i], "STEP_END");
		strcpy(m_aszDescript[i], "");
	}

	m_nStepCount = 0;
	m_nLastStepForBreak = 0;
	m_nNowStep = 0;
	m_bNextStepFlag = FALSE;
	m_nValveSeqSts = VSEQ_IDLE;

	InitIOList(g_pIOList);
	if(FALSE == ReadConfig()) bRet = FALSE;

	// printf("-----> ValveSeq Initialized\n");
	return bRet;
}

BOOL CValveSeq::ReadConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle;

	do {
		fp = fopen(m_szFilePath, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp(m_szSeqName, szItem) == 0)						nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseValveSeq(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : [%s]\n", m_szFilePath);
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
void CValveSeq::ParseValveSeq(char *szData)
{
	BOOL bRslt = TRUE;
	int nStep, nIdx;
	int i;
	char szTemp[40];
	CTextParser tp(szData);

	// printf("-----> VlvCfg : %s\n", szData);
	do {
		if(! tp.GetInt(&nStep)) break;
		if(nStep > MAX_STEP) break;
		nIdx = nStep - 1;
		if(! tp.GetWord(m_aszDescript[nIdx])) break;
		if(! tp.GetDbFloat(&m_adbStepTime[nIdx])) break;
		for(i=0; i<MAX_MFC_PER_LINE; i++)
		{
			if(! tp.GetInt(&m_anMFCNo[nIdx][i])) { bRslt = FALSE; break; }
			if(! tp.GetWord(szTemp)) { bRslt = FALSE; break; }
			else
			{
				if(strcmp(szTemp, "OPEN") == 0) m_adbMFCSet[nIdx][i] = -1;
				else if(strcmp(szTemp, "CLOSE") == 0) m_adbMFCSet[nIdx][i] = 0;
				else
				{
					m_adbMFCSet[nIdx][i] = atof(szTemp);
					if(-1 == m_adbMFCSet[nIdx][i]) m_adbMFCSet[nIdx][i] = -10;		// 2009.12.22
				}
			}
		}
		if(! bRslt) break;
		if(! tp.GetDbFloat(&m_adbTVSet[nIdx])) break;
		if(! tp.GetDbFloat(&m_adbZMSet[nIdx][0])) break;
		if(! tp.GetDbFloat(&m_adbZMSet[nIdx][1])) break;
		if(! tp.GetWord(m_aszCtrlCmd[nIdx])) break;
		if(! tp.GetString(m_aszValveStr[nIdx])) break;
		if(m_nStepCount < nStep) m_nStepCount = nStep;
	} while(0);
}

//////////////////////////////////////////////////////////////////////////
BOOL CValveSeq::StartValveSeq(const char *szDescript, BOOL bWait)
{
	int nStartStep;

	printf("-----> Start ValveSeq %s\n", szDescript);

	nStartStep = FindStep(szDescript);
	if(nStartStep > 0) return StartValveSeq(nStartStep, MAX_STEP, bWait);
	
	printf("-----> Invalid Seq Name ![%s]\n", szDescript);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CValveSeq::StartValveSeq(int nStartStep, int nEndStep, BOOL bWait)
{
	BOOL bRet = TRUE;
	int i, nStep;
	long lnTimeCount;
	BOOL bStopFlag = FALSE;															// 2010.01.29

	// Init;
	m_nNowStep = 0;
	lnTimeCount = 0;

	// Calc Step End Time
	for(i=nStartStep-1; i<nEndStep; i++)
	{
		nStep = i+1;
		if(nStep > MAX_STEP) break;

		if(strcmp(m_aszDescript[i], "") == 0) continue;								// 2010.08.09
		if(strcmp(m_aszDescript[i], "-------->STEP_END") == 0) bStopFlag = TRUE;	// 2010.01.29
		
		if(strcmp(m_aszCtrlCmd[i], "STEP_END") == 0) break;
		if(strcmp(m_aszCtrlCmd[i], "STEP_SKIP") == 0)
		{
			if(TRUE == bStopFlag) break;											// 2010.01.29
			continue;
		}
		if(m_adbStepTime[i] > 0) lnTimeCount += (long)(m_adbStepTime[i] * 1000);
		m_alnStepEndTime[i] = lnTimeCount;

		if(TRUE == bStopFlag) break;												// 2010.01.29
	}

	// Start Valve Sequence
	m_nValveSeqSts = VSEQ_RUN;
	if(! StartThreadFunc(nStartStep, nEndStep)) m_nValveSeqSts = VSEQ_FAILED;		// 2010.01.26
	if(bWait)
	{
		while(VSEQ_RUN == m_nValveSeqSts)
		{
			if(! WAIT_SECONDS(0.1)) break;
		}
	}
	return bRet;
}

void CValveSeq::StopValveSeq(int nEndStep)
{
	m_nLastStepForBreak = nEndStep;
	StopThreadFunc();
}

// 2010.05.17
BOOL CValveSeq::ExecValveStep(const char *szDescript, BOOL bFuncFree)
{
	BOOL bRet = FALSE;;
	int nStartStep;

	do {
		nStartStep = FindStep(szDescript);
		if(nStartStep <= 0)
		{
			printf("-----> Invalid Seq Name ![%s]\n", szDescript);
			break;
		}
		if(bFuncFree) ExecValveStep_Free(nStartStep);
		else ExecValveStep(nStartStep);
		bRet = TRUE;
	} while(0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL CValveSeq::ExecValveStep(int nStep)
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	int nIdx;
	int i;
	char szTemp[80];
	char szMFCParam[80];
	char szValveArray[256];
	BOOL bMFCSet;

	m_nNowStep = nStep;
	nIdx = nStep - 1;

	// _LOG("-----> ExecValveStep : %d %s\n", nStep, m_aszDescript[nIdx]);
	if(strcmp(m_aszCtrlCmd[nIdx], "MFC_ALL_CLOSE") == 0						// 2010.01.26
		|| strcmp(m_aszCtrlCmd[nIdx], "CLOSE_ALL_MFC") == 0) dRUN_FUNCTION(MFC_CONTROL, "CLOSE_ALL");
	else
	{
		bMFCSet = FALSE;
		for(i=0; i<MAX_MFC_PER_LINE; i++)
		{
			if(m_anMFCNo[nIdx][i] <= 0) continue;
			if(-10 == m_adbMFCSet[nIdx][i]) continue;						// 2009.12.22
			if(FALSE == bMFCSet)
			{
				sprintf(szMFCParam, "SET_VALUE 0 %d %.3f N 0", m_anMFCNo[nIdx][i], (float) m_adbMFCSet[nIdx][i]);
				bMFCSet = TRUE;
			}
			else 
			{
				sprintf(szTemp, " %d %.3f N 0", m_anMFCNo[nIdx][i], (float) m_adbMFCSet[nIdx][i]);
				strcat(szMFCParam, szTemp);
			}
		}
		if(bMFCSet) dRUN_FUNCTION(MFC_CONTROL, szMFCParam);
	}

	//////////////////////////////////////////////////////////////////////////
	//... Set Rough Valve
	if     (strcmp(m_aszCtrlCmd[nIdx], "ROUGH1_OPEN") == 0)		dWRITE_DIGITAL(RoughVlvDO, VLV_OPEN, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_OPEN") == 0)		dWRITE_DIGITAL(RoughVlvDO, VLV_OPEN, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_ALL_OPEN") == 0)	dWRITE_DIGITAL(RoughVlvDO, VLV_OPEN, &bIOStatus);
	//
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH1_CLOSE") == 0)	dWRITE_DIGITAL(RoughVlvDO, VLV_CLOSE, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_CLOSE") == 0)		dWRITE_DIGITAL(RoughVlvDO, VLV_CLOSE, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_ALL_CLOSE") == 0)	dWRITE_DIGITAL(RoughVlvDO, VLV_CLOSE, &bIOStatus);

	//////////////////////////////////////////////////////////////////////////
	//... Set Throttle Valve
	if(m_adbTVSet[nIdx] >= 0)
	{
		sprintf(szTemp, "POSITION %d", (int)m_adbTVSet[nIdx]);
		dRUN_FUNCTION(PRS_CONTROL, szTemp);
	}

	//////////////////////////////////////////////////////////////////////////
	//... Not Use Chuck Move by wjkim
	// 	if(m_adbZMSet[nIdx][0] >= 0)
	// 	{
	// 		sprintf(szTemp, "RUNP 0 %.3f", (float)m_adbZMSet[nIdx][0]);
	// 		dRUN_FUNCTION(ZMOTION_CONTROL, szTemp);
	// 	}
	// 
	// 	if(m_adbZMSet[nIdx][1] >= 0)
	// 	{
	// 		sprintf(szTemp, "RUNP  1 %.3f", (float)m_adbZMSet[nIdx][1]);
	// 		dRUN_FUNCTION(ZMOTION_CONTROL, szTemp);
	// 	}
	//////////////////////////////////////////////////////////////////////////

	if(strcmp(m_aszCtrlCmd[nIdx], "WRITE_IO") != 0)
	{
		sprintf(szValveArray, "SET_VALVE_ARRAY %s", m_aszValveStr[nIdx]);
		dRUN_FUNCTION(VALVE_CONTROL, szValveArray);
	}
	else
	{
		WriteIO(m_aszValveStr[nIdx]);
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL CValveSeq::ExecValveStep_Free(int nStep)
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	int nIdx;
	int i;
	char szTemp[80];
	char szMFCParam[80];
	char szValveArray[256];
	BOOL bMFCSet;

	m_nNowStep = nStep;
	nIdx = nStep - 1;

	// _LOG("-----> ExecValveStep_Free : %d %s\n", nStep, m_aszDescript[nIdx]);
	if(strcmp(m_aszCtrlCmd[nIdx], "MFC_ALL_CLOSE") == 0						// 2010.01.26
		|| strcmp(m_aszCtrlCmd[nIdx], "CLOSE_ALL_MFC") == 0) dRUN_FUNCTION_FREE(MFC_CONTROL, "CLOSE_ALL");
	else
	{
		bMFCSet = FALSE;
		for(i=0; i<MAX_MFC_PER_LINE; i++)
		{
			if(m_anMFCNo[nIdx][i] <= 0) continue;
			if(-10 == m_adbMFCSet[nIdx][i]) continue;						// 2009.12.22
			if(FALSE == bMFCSet)
			{
				sprintf(szMFCParam, "SET_VALUE 0 %d %.3f N 0", m_anMFCNo[nIdx][i], (float) m_adbMFCSet[nIdx][i]);
				bMFCSet = TRUE;
			}
			else 
			{
				sprintf(szTemp, " %d %.3f N 0", m_anMFCNo[nIdx][i], (float) m_adbMFCSet[nIdx][i]);
				strcat(szMFCParam, szTemp);
			}
		}
		if(bMFCSet) dRUN_FUNCTION_FREE(MFC_CONTROL, szMFCParam);
	}

	//////////////////////////////////////////////////////////////////////////
	//... Set Rough Valve
	if     (strcmp(m_aszCtrlCmd[nIdx], "ROUGH1_OPEN") == 0)		dWRITE_DIGITAL(RoughVlvDO, VLV_OPEN, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_OPEN") == 0)		dWRITE_DIGITAL(RoughVlvDO, VLV_OPEN, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_ALL_OPEN") == 0)	dWRITE_DIGITAL(RoughVlvDO, VLV_OPEN, &bIOStatus);
	//
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH1_CLOSE") == 0)	dWRITE_DIGITAL(RoughVlvDO, VLV_CLOSE, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_CLOSE") == 0)		dWRITE_DIGITAL(RoughVlvDO, VLV_CLOSE, &bIOStatus);
	else if(strcmp(m_aszCtrlCmd[nIdx], "ROUGH_ALL_CLOSE") == 0)	dWRITE_DIGITAL(RoughVlvDO, VLV_CLOSE, &bIOStatus);
	
	//////////////////////////////////////////////////////////////////////////
	//... Set Throttle Valve
	if(m_adbTVSet[nIdx] >= 0)
	{
		sprintf(szTemp, "POSITION %d", (int)m_adbTVSet[nIdx]);
		dRUN_FUNCTION_FREE(PRS_CONTROL, szTemp);
	}

	//////////////////////////////////////////////////////////////////////////
	//... Not Use Chuck Move by wjkim
	// 	if(m_adbZMSet[nIdx][0] >= 0)
	// 	{
	// 		sprintf(szTemp, "RUNP 0 %.3f", (float)m_adbZMSet[nIdx][0]);
	// 		dRUN_FUNCTION(ZMOTION_CONTROL, szTemp);
	// 	}
	// 
	// 	if(m_adbZMSet[nIdx][1] >= 0)
	// 	{
	// 		sprintf(szTemp, "RUNP  1 %.3f", (float)m_adbZMSet[nIdx][1]);
	// 		dRUN_FUNCTION(ZMOTION_CONTROL, szTemp);
	// 	}
	//////////////////////////////////////////////////////////////////////////

	if(strcmp(m_aszCtrlCmd[nIdx], "WRITE_IO") != 0)
	{
		sprintf(szValveArray, "SET_VALVE_ARRAY %s", m_aszValveStr[nIdx]);
		dRUN_FUNCTION_FREE(VALVE_CONTROL, szValveArray);
	}
	else
	{
		WriteIO(m_aszValveStr[nIdx]);
	}
	return bRet;
}

void CValveSeq::GoNextStep()
{
	m_bNextStepFlag = TRUE;
}

BOOL CValveSeq::IsValveSeqRunning()
{
	if(VSEQ_RUN == m_nValveSeqSts) return TRUE;
	return FALSE;
}

int  CValveSeq::FindStep(const char *szDescript)
{
	int nRet = -1;
	int i;

	for(i=0; i<MAX_STEP; i++)
	{
		if(strcmp(m_aszDescript[i], szDescript) == 0)
		{
			nRet = i + 1;
			break;
		}
	}

	return nRet;
}

int CValveSeq::FindStepIdx(const char *szDescript)
{
	int nIdx = -1;
	int nStep;

	nStep = FindStep(szDescript);
	if(nStep > 0) nIdx = nStep - 1;
	return nIdx;
}

void CValveSeq::WriteIO(const char *szIOString)
{
	char szIOName[40];
	char szIOType[40];
	char szValue[40];
	int nIOType;
	BOOL bIOStatus;

	CTextParser tp(szIOString);

	do {
		if(! tp.GetWord(szIOName)) break;
		if(! tp.GetWord(szIOType)) break;
		if(! tp.GetWord(szValue)) break;

		if     (strcmp(szIOType, "D") == 0)		nIOType = _K_D_IO;
		else if(strcmp(szIOType, "A") == 0)		nIOType = _K_A_IO;
		else if(strcmp(szIOType, "S") == 0)		nIOType = _K_S_IO;
		else break;

		// _LOG("-----> Write_IO : %s %d %s\n",szIOName, nIOType, szValue);
		dWRITE_IO_FROM_NAME(szIOName, nIOType, szValue, &bIOStatus);
	} while(1);
}

//////////////////////////////////////////////////////////////////////
// Thread

void CValveSeq::DoFunction(WPARAM wParam, LPARAM lParam)
{
	int i;
	int nStartStep, nEndStep;
	int nStep;
	BOOL bStopFlag = FALSE;

	m_bThreadFuncRun = true;

	nStartStep = wParam;
	nEndStep = lParam;
	m_pSeqTimer->StartTimer();

	for(i=nStartStep-1; i<nEndStep; i++)
	{
		nStep = i+1;
		if(nStep > MAX_STEP) break;

		if(strcmp(m_aszDescript[i], "") == 0) continue;								// 2010.08.09
		if(strcmp(m_aszDescript[i], "-------->STEP_END") == 0) bStopFlag = TRUE;	// 2010.01.26

		if(strcmp(m_aszCtrlCmd[i], "STEP_END") == 0) break;
		if(strcmp(m_aszCtrlCmd[i], "STEP_SKIP") == 0)
		{
			if(TRUE == bStopFlag) break;											// 2010.01.26
			continue;
		}
		if(FALSE == ExecValveStep(nStep)) break;

		if(m_alnStepEndTime[i] == -1)
		{
			m_pSeqTimer->PauseTimer();
			while(FALSE == m_bNextStepFlag)
			{
				if(false == m_bThreadFuncRun) break;
				if(TRUE == m_bNextStepFlag) { m_bNextStepFlag = FALSE; break; }
			}
			m_pSeqTimer->ResumeTimer();
		}
		else while(m_pSeqTimer->GetProgressTime() < m_alnStepEndTime[i])
		{
			if(MANAGER_ABORT()) {m_bThreadFuncRun = false; break;}
			if(false == m_bThreadFuncRun) break;
			if(TRUE == m_bNextStepFlag) { m_bNextStepFlag = FALSE; break; }
			Sleep(30);
		}
		if(false == m_bThreadFuncRun) break;
		if(TRUE == bStopFlag) break;												// 2010.01.26
	}

	if(m_nLastStepForBreak != 0) ExecValveStep(m_nLastStepForBreak);
	m_pSeqTimer->StopTimer();
	m_bThreadFuncRun = false;
	m_nValveSeqSts = VSEQ_IDLE;
}
