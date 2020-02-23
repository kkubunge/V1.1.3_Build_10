// InterlockMonitor.cpp: implementation of the CInterlockMonitor class.
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
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

#include "InterlockMonitor.h"
#include "TextLogDll_M.h"
#include "Timer.h"
#include "TextParser.h"

//------------------------------------------------------------------------------------------
#define CONFIG_FILE_NAME	".\\Function\\Function_INT_Monitor\\INT_Monitor.cfg"	

//------------------------------------------------------------------------------------------
typedef enum { LG_EQUAL, LG_NOT_EQUAL, LG_UPPER, LG_LOWER, LG_E_UPPER, LG_E_LOWER } LOGIC_ENUM;

static ENUM_STRING g_aLogEnumString[] = 
{
	"==",	LG_EQUAL,
	"!=",	LG_NOT_EQUAL,
	">",	LG_UPPER,
	"<",	LG_LOWER,
	">=",	LG_E_UPPER,
	"<=",	LG_E_LOWER,
	"=>",	LG_E_UPPER,
	"=<",	LG_E_LOWER,
	""
};

//------------------------------------------------------------------------------------------
static enum { EN_OFF, EN_ON };
static enum { EN_STOP, EN_RUN };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInterlockMonitor::CInterlockMonitor()
{
	int i;
	m_nSgIntCount = 0;

	for(i=0; i<MAX_INTERLOCK_TYPE; i++)
	{
		m_IoCtrlList[i].nCount = 0;
		m_IoCtrlList[i].nIntrCtrl = INTCT_NO;
	}
}

CInterlockMonitor::~CInterlockMonitor()
{
	_gCLOSE_LOG();
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CInterlockMonitor::Enter_Code()
{
	char szParam[40] = "";
	strcpy(szParam, PROGRAM_ARGUMENT_READ());
}

BOOL CInterlockMonitor::Initialize()
{
	m_IOVar.Initialize();
	ReadConfiguration();

	CreateThread();
	return SYS_SUCCESS;
}

void CInterlockMonitor::Deinitialize()
{
	DestroyThread();
}

//------------------------------------------------------------------------------------------
BOOL CInterlockMonitor::ReadConfiguration()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle,i = 0;
	CTextParser txtP;

	_LOG("------------------------------------------------------------");
	_LOG("---> Read Configration Data\n");

	do {
		fp = fopen(CONFIG_FILE_NAME, "rt");
		if(fp == NULL)
		{
			_LOG("-----> '%s' Open Fail", CONFIG_FILE_NAME);
			break;
		}

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
				if(szRead[0] == '[')
				{
					txtP.SetText(szRead);
					txtP.GetChar();
					txtP.GetString(szItem, ']');

					if(strcmp("Single Interlock List", szItem) == 0)			    nTitle = 1;
					else if(strcmp("Interlock Type List", szItem) == 0)				nTitle = 2;
					else nTitle = 0;

					if(nTitle > 0)
					{
						do{
							if(NULL == fgets(szRead, 255, fp)) break;
							if(szRead[0] == '[') break;
							if(szRead[0] !=';' && szRead[0] !='#' && szRead[0] !='\r' && szRead[0] !='\n' && szRead[0] !=' ') 
							{
								switch(nTitle)
								{
									case 1: ParseSingInterlockList(szRead); break;
									case 2: ParseInterlockTypeList(szRead); break;
								}
							}
						} while(feof(fp) == 0);
					}
					else if(NULL == fgets(szRead, 255, fp)) break;
				}
				else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);

	_LOG("------------------------------------------------------------");
	return bRet;
}

BOOL CInterlockMonitor::ParseSingInterlockList(char *szData)
{

	BOOL bRet = FALSE;
	char szTemp[40];
	int nIOType;
	int nVarIndex = 1;
	CTextParser txtP;

	_LOG("-----> Int[%d] %s", m_nSgIntCount+1, szData);
	txtP.SetText(szData);
	do {
		if(m_nSgIntCount >= MAX_SINGLE_INTERLOCK-1) break;
		if(! txtP.GetWord()) break;
		if(txtP.BeEqual("D")) nIOType = _K_D_IO;
		else if(txtP.BeEqual("A")) nIOType = _K_A_IO;
		else break;

		if(! txtP.GetWord(szTemp)) break;
		nVarIndex = m_IOVar.AddVariable(szTemp, nIOType);
		if(nVarIndex < 0) break;
		m_SgIntList[m_nSgIntCount].nVarIdx1 = nVarIndex;

		if(! txtP.GetWord(szTemp)) break;
		m_SgIntList[m_nSgIntCount].nLogic = txtP._GetEnumValue(szTemp, g_aLogEnumString);

		if(! txtP.GetWord(szTemp)) break;
		nVarIndex = m_IOVar.AddVariable(szTemp, nIOType);
		if(nVarIndex < 0) break;
		m_SgIntList[m_nSgIntCount].nVarIdx2 = nVarIndex;

		m_SgIntList[m_nSgIntCount].nIOType = nIOType;
		if(! txtP.GetInt(&m_SgIntList[m_nSgIntCount].nTimeout)) break;
		if(! txtP.GetInt(&m_SgIntList[m_nSgIntCount].nAlarmID)) break;
		if(! txtP.GetInt(&m_SgIntList[m_nSgIntCount].nInterlockType)) break;

		m_nSgIntCount++;
		bRet = TRUE;
	} while(0);

	if(! bRet)
	{
		_LOG("-----> Int[%d] Creation Failed !", m_nSgIntCount+1);
	}
	return bRet;
}

BOOL CInterlockMonitor::ParseInterlockTypeList(char *szData)
{
	static int nInterlockType = 0;
	BOOL bRet = FALSE;
	int nIdx;
	CTextParser txtP;

	txtP.SetText(szData);
	do {
		if(! txtP.GetWord()) break;
		if(txtP.BeEqual("InterlockType"))
		{
			txtP.GetInt('=', &nInterlockType);
			bRet = TRUE;
			break;
		}

		if(0 == nInterlockType || nInterlockType > MAX_INTERLOCK_TYPE) break;
		nIdx = nInterlockType - 1;

		if(txtP.BeEqual("InterlockCtrl"))
		{
			txtP.GetInt('=', &m_IoCtrlList[nIdx].nIntrCtrl);
			bRet = TRUE;
			break;
		}

		if(m_IoCtrlList[nIdx].nCount >= MAX_IO_CONTROL) break;

		if(txtP.BeEqual("D")) m_IoCtrlList[nIdx].aIOType[m_IoCtrlList[nIdx].nCount] = _K_D_IO;
		else if(txtP.BeEqual("A")) m_IoCtrlList[nIdx].aIOType[m_IoCtrlList[nIdx].nCount] = _K_A_IO;
		else break;
		
		if(! txtP.GetWord(m_IoCtrlList[nIdx].aIONameList[m_IoCtrlList[nIdx].nCount])) break;

		switch(m_IoCtrlList[nIdx].aIOType[m_IoCtrlList[nIdx].nCount])
		{
		case _K_D_IO:	txtP.GetInt('=', &m_IoCtrlList[nIdx].anValue[m_IoCtrlList[nIdx].nCount]); break;
		case _K_A_IO:	txtP.GetDbFloat('=', &m_IoCtrlList[nIdx].adbValue[m_IoCtrlList[nIdx].nCount]); break;
		}

		m_IoCtrlList[nIdx].nCount++;
		bRet = TRUE;
	} while(0);

	return TRUE;
}

//------------------------------------------------------------------------------------------
Module_Status CInterlockMonitor::Sim_Main()
{
	Module_Status msRet = SYS_ABORTED;
	do {

		//...
		msRet = SYS_SUCCESS;
	} while(0);
	return msRet;
}

Module_Status CInterlockMonitor::Real_Main()
{
	Module_Status msRet = SYS_ABORTED;
	char *pParam;
	pParam = PROGRAM_PARAMETER_READ();

	do {

		msRet = SYS_SUCCESS;
	} while(0);
	return msRet;
}

void CInterlockMonitor::LOG(const char *szLog)
{
	_LOG(szLog);
}


//-----------------------------------------------------------------------------------
void CInterlockMonitor::DoMonitor()
{
	int i;
	int nThreadCount;

	nThreadCount = 1 + (m_nSgIntCount-1)/10 ;
	for(i=0; i<nThreadCount; i++)
	{
		if(! WaitSec(0.1)) break;
		CreateThread(i+1);
	}

	while(WaitSec(1))
	{
	}
}

void CInterlockMonitor::DoMonitor(int nThreadNo)
{
	switch(nThreadNo)
	{
	case 0: DoMonitor(); break;
	default :
			DoSingleInterlockMonitor(nThreadNo, (nThreadNo-1)*10);
			break;
	}
}

//-----------------------------------------------------------------------------------
void CInterlockMonitor::DoSingleInterlockMonitor(int nThreadNo, int nIntIndex)
{
	CTimer tmr[10];
	CTextParser txtP;
	int i;
	int nIdx;
	double dbValue1, dbValue2;
	char szVName1[40], szVName2[40];

	/*
	for(i=0; i<10; i++)
	{
		nIdx = nIntIndex + i;
		if(nIdx >= m_nSgIntCount) break;
		_LOG("-----> Created Interlock[ThreadNo:%d, IntrNo:%d]", nThreadNo, nIdx+1);
	}
	*/

	while(IS_THREAD_RUNNING())
	{
		for(i=0; i<10; i++)
		{
			if(! WAIT_SEC(0.1)) break;
			nIdx = nIntIndex + i;
			if(nIdx >= m_nSgIntCount)
			{
				WAIT_SEC(0.1 * (9-i));
				break;
			}

			if(ValidateInterlock(nIdx, &dbValue1, &dbValue2))
			{
				if(tmr[i].m_nTimerStatus != TIMER_RUN) tmr[i].StartTimer();
				if(tmr[i].GetProgressTime(UNIT_SECOND) > m_SgIntList[nIdx].nTimeout)
				{
					tmr[i].StopTimer();
					if(ALARM_STATUS(m_SgIntList[nIdx].nAlarmID) == 0)
					{
						ALARM_POST(m_SgIntList[nIdx].nAlarmID);
						m_IOVar.GetIoName(m_SgIntList[nIdx].nVarIdx1, szVName1);
						m_IOVar.GetIoName(m_SgIntList[nIdx].nVarIdx2, szVName2);
						_LOG("-----> Alarm Raised[%d] { %s[%.3f] %s %s[%.3f] }",
								m_SgIntList[nIdx].nAlarmID,
								szVName1,
								dbValue1,
								txtP._GetEnumString(m_SgIntList[nIdx].nLogic, g_aLogEnumString),
								szVName2,
								dbValue2
								);
						ExecInterlock(m_SgIntList[nIdx].nInterlockType);
					}
				}
			}
			else
			{
				if(tmr[i].m_nTimerStatus == TIMER_RUN) tmr[i].StopTimer();
			}
		}
	}
}

void CInterlockMonitor::ExecInterlock(int nIntType)
{
	int i;
	int nIdx;
	BOOL bIOStatus;

	if(nIntType <= 0) return;
	_LOG("-----> Interlock Activated [Type:%d]", nIntType);

	nIdx = nIntType - 1;
	for(i=0; i<m_IoCtrlList[nIdx].nCount; i++)
	{
		m_SgIntIO.dWRITE_IO_FROM_NAME(m_IoCtrlList[nIdx].aIONameList[i], m_IoCtrlList[nIdx].aIOType[i], 
					m_IoCtrlList[nIdx].anValue[i], m_IoCtrlList[nIdx].adbValue[i], &bIOStatus);
		switch(m_IoCtrlList[nIdx].aIOType[i])
		{
		case _K_D_IO:
			_LOG("-----> WriteIO (%s = [%d])", m_IoCtrlList[nIdx].aIONameList[i],
						m_IoCtrlList[nIdx].anValue[i]);
			break;
		case _K_A_IO:
			_LOG("-----> WriteIO (%s = [%.3f])", m_IoCtrlList[nIdx].aIONameList[i],
						m_IoCtrlList[nIdx].adbValue[i]);
			break;
		}
	}
}

BOOL CInterlockMonitor::ValidateInterlock(int nIntIndex, double *pValue1, double *pValue2)
{
	BOOL bRet = FALSE;
	double dbValue1 = 0, dbValue2 = 0;
	int nValue1, nValue2;

	switch(m_SgIntList[nIntIndex].nIOType)
	{
	case _K_D_IO:
				nValue1 = m_IOVar.ReadInteger(m_SgIntList[nIntIndex].nVarIdx1);
				nValue2 = m_IOVar.ReadInteger(m_SgIntList[nIntIndex].nVarIdx2);
				bRet = ValidateDec(nValue1, nValue2, m_SgIntList[nIntIndex].nLogic);

				dbValue1 = nValue1;
				dbValue2 = nValue2;
				break;
	case _K_A_IO:
				dbValue1 = m_IOVar.ReadDouble(m_SgIntList[nIntIndex].nVarIdx1);
				dbValue2 = m_IOVar.ReadDouble(m_SgIntList[nIntIndex].nVarIdx2);
				bRet = ValidateDbl(dbValue1, dbValue2, m_SgIntList[nIntIndex].nLogic);
				break;
	}

	if(pValue1 != NULL) *pValue1 = dbValue1;
	if(pValue2 != NULL) *pValue2 = dbValue2;
	return bRet;
}

BOOL CInterlockMonitor::ValidateDec(int nValue1, int nValue2, int nLogic)
{
	BOOL bRet = FALSE;
	switch(nLogic)
	{
	case LG_EQUAL:			if(nValue1 == nValue2) bRet = TRUE; break;
	case LG_NOT_EQUAL:		if(nValue1 != nValue2) bRet = TRUE; break;
	case LG_UPPER:			if(nValue1 > nValue2) bRet = TRUE; break;
	case LG_LOWER:			if(nValue1 < nValue2) bRet = TRUE; break;
	case LG_E_UPPER:		if(nValue1 >= nValue2) bRet = TRUE; break;
	case LG_E_LOWER:		if(nValue1 <= nValue2) bRet = TRUE; break;
	}
	return bRet;
}

BOOL CInterlockMonitor::ValidateDbl(double dbValue1, double dbValue2, int nLogic)
{
	BOOL bRet = FALSE;
	switch(nLogic)
	{
	case LG_EQUAL:			if(dbValue1 == dbValue2) bRet = TRUE; break;
	case LG_NOT_EQUAL:		if(dbValue1 != dbValue2) bRet = TRUE; break;
	case LG_UPPER:			if(dbValue1 > dbValue2) bRet = TRUE; break;
	case LG_LOWER:			if(dbValue1 < dbValue2) bRet = TRUE; break;
	case LG_E_UPPER:		if(dbValue1 >= dbValue2) bRet = TRUE; break;
	case LG_E_LOWER:		if(dbValue1 <= dbValue2) bRet = TRUE; break;
	}
	return bRet;
}
