// PMMainSimulator.cpp: implementation of the CPMMainSimulator class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//====================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//====================================================================

#include "TextParser.h"
#include "Definition.h"
#include "PMMainSimulator.h"

#include "PMChmPrsSimulator.h"
#include "PMHeaterSimulator.h"
#include "PMMfcSimulator.h"
#include "PMDryPumpSimulator.h"
#include "PMStgHtrSimulator.h"
#include "PMStgHtrSimulatorType2.h"
#include "IOSyncMonitor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPMMainSimulator::CPMMainSimulator(int nStgHtrType)
{
	int i;
	for(i=0; i<MAX_IO_LIST; i++)
	{
		m_pIOList[i].szIOName[0] = 0;
		m_pIOList[i].szValue[0] = 0;
		m_pIOList[i].nIOType = 0;
		m_pIOList[i].nIOPoint = -1;
	}
	m_IOList_nCount = 0;

	m_pPMChmPrsSimulator = new CPMChmPrsSimulator;
	m_pPMHeaterSimulator = new CPMHeaterSimulator;
	m_pPMMfcSimulator = new CPMMfcSimulator;
	m_pPMDryPumpSimulator = new CPMDryPumpSimulator;
	m_pIOSyncMonitor = new CIOSyncMonitor;			// 2008.08.25

	m_nStgHtrType = nStgHtrType;
	if(m_nStgHtrType == 1) 		m_pPMStgHtrSimulator = new CPMStgHtrSimulator;
	else 						m_pPMStgHtrSimulator = new CPMStgHtrSimulatorType2;

	for(i=0; i<MAX_ALARM_CLEAR_LIST; i++) m_pnAlarmClearList[i] = 0;
	m_nAlarmClearCount = 0;
}

CPMMainSimulator::~CPMMainSimulator()
{
	delete m_pPMStgHtrSimulator;
	delete m_pPMDryPumpSimulator;
	delete m_pPMMfcSimulator;
	delete m_pPMHeaterSimulator;
	delete m_pPMChmPrsSimulator;
	delete m_pIOSyncMonitor;		// 2008.08.25
}

//////////////////////////////////////////////////////////////////////////
BOOL CPMMainSimulator::Initialize()
{
	ReadSimCfg();
	InitIOValue();

	m_pPMChmPrsSimulator->Initialize();
	m_pPMHeaterSimulator->Initialize();
	m_pPMMfcSimulator->Initialize(&m_pPMChmPrsSimulator->m_dbTargetPress);
	m_pPMDryPumpSimulator->Initialize();
	m_pPMStgHtrSimulator->Initialize();

	m_pIOSyncMonitor->Initialize();							// 2008.08.25
	CreateThread();

	return	TRUE;
}

void CPMMainSimulator::DoMonitor()
{
	int i;
	WaitSec(5);
	for(i=0; i<m_nAlarmClearCount; i++)
	{
		if(ALM_PAUSED == ALARM_STATUS(m_pnAlarmClearList[i])) ALARM_CLEAR(m_pnAlarmClearList[i]);
	}
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CPMMainSimulator::InitIOValue()
{
	int nIOStatus;
	int i;
	int nValue;
	double dbValue;
	for(i=0; i<m_IOList_nCount; i++)
	{
		// 2006.02.27
		switch(m_pIOList[i].nIOType)
		{
		case _K_A_IO:
			dbValue = atof(m_pIOList[i].szValue);
			_dWRITE_ANALOG(m_pIOList[i].nIOPoint, dbValue, &nIOStatus);
			break;
		case _K_D_IO:
			nValue = atoi(m_pIOList[i].szValue);
			_dWRITE_DIGITAL(m_pIOList[i].nIOPoint, nValue, &nIOStatus);
			break;
		case _K_S_IO:
			_dWRITE_STRING(m_pIOList[i].nIOPoint, m_pIOList[i].szValue, &nIOStatus);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL CPMMainSimulator::ReadSimCfg()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(SIM_CFG_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if     (strcmp("StageHeater", szItem) == 0)						nTitle = 1;
				else if(strcmp("MFC", szItem) == 0)								nTitle = 2;
				else if(strcmp("Pressure Control Configuration", szItem) == 0)	nTitle = 3;
				else if(strcmp("Heater", szItem) == 0)							nTitle = 4;
				else if(strcmp("Init I/O List", szItem) == 0)					nTitle = 5;
				else if(strcmp("Alarm Clear List", szItem) == 0)				nTitle = 6;
				else if(strcmp("I/O Sync List", szItem) == 0)					nTitle = 7;		// 2008.08.25
				else															nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseStageHeaterCfg(szRead);	break;
					case 2: ParseMfcCfg(szRead);			break;
					case 3: ParsePrsCtrlCfg(szRead);		break;
					case 4: ParseHeaterCfg(szRead);			break;
					case 5: ParseInitIOCfg(szRead);			break;
					case 6: ParseAlarmClearCfg(szRead);		break;
					case 7: ParseIOSyncList(szRead);		break;		// 2008.01.17
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else
			{
				if(NULL == fgets(szRead, 255, fp)) break;
			}
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	return bRet;
}

void CPMMainSimulator::ParseHeaterCfg(char *szRead)
{
	char szItem[40] = "";
	CTextParser tp(szRead);
	if(0 == strlen(szRead)) return;
	tp.GetWord(szItem, 0);
	if(strcmp(szItem, "SimulationEnable") == 0) tp.GetBool(&m_pPMHeaterSimulator->m_bHtr_SimEnable, 0);
}

//////////////////////////////////////////////////////////////////////////
void CPMMainSimulator::ParseMfcCfg(char *szRead)
{
	char szItem[40] = "";
	char szValveName[40];
	int nValue, nValue2;
	int nMfcNo;

	CTextParser tp(szRead);
	if(0 == strlen(szRead))		return;

	tp.GetWord(szItem, 0);
	if(strcmp(szItem, "MfcError") == 0)
	{
		tp.GetInt(&nValue, 0);
		tp.GetInt(&nValue2, 0);
		if(nValue > 0) m_pPMMfcSimulator->m_pnMFCErr[nValue-1] = nValue2;
	}
	else if(strcmp(szItem, "SimulationEnable") == 0) tp.GetBool(&m_pPMMfcSimulator->m_bMFC_SimEnable, 0);
	else if(strcmp(szItem, "MfcConfig") == 0)		// 2006.09.07
	{
		tp.GetInt(&nMfcNo, 0);		// Mfc Number
		tp.GetInt(&nValue, 0);		// Skip
		tp.GetInt(&nValue2, 0);		// Skip
		do {
			if(tp.GetWord(szValveName)) m_pPMMfcSimulator->SetMfcFeedValve(nMfcNo, 0, szValveName);
			else break;
			if(tp.GetWord(szValveName)) m_pPMMfcSimulator->SetMfcFeedValve(nMfcNo, 1, szValveName);
			else break;
			if(tp.GetWord(szValveName)) m_pPMMfcSimulator->SetMfcFeedValve(nMfcNo, 2, szValveName);
			else break;
		} while(0);
	}
}

void CPMMainSimulator::ParsePrsCtrlCfg(char *szRead)
{
	char szItem[40] = "";
	CTextParser tp(szRead);
	if(0 == strlen(szRead)) return;
	tp.GetWord(szItem, 0);
	if(strcmp(szItem, "MksPrsSimEnable") == 0) tp.GetBool(&m_pPMChmPrsSimulator->m_bChm_SimEnable, 0);
}

//////////////////////////////////////////////////////////////////////////
void CPMMainSimulator::ParseInitIOCfg(char *szRead)
{
	char szItem[40] = "";
	CTextParser tp(szRead);

	if(0 == strlen(szRead))										return;
	if(! tp.GetWord(m_pIOList[m_IOList_nCount].szIOName, 0))	return;

	if(! tp.GetWord(szItem, 0))									return;
	if     (szItem[0] == 'A')		m_pIOList[m_IOList_nCount].nIOType = _K_A_IO;
	else if(szItem[0] == 'S')		m_pIOList[m_IOList_nCount].nIOType = _K_S_IO;
	else							m_pIOList[m_IOList_nCount].nIOType = _K_D_IO;

	tp.GetWord(m_pIOList[m_IOList_nCount].szValue, 0);
	m_pIOList[m_IOList_nCount].nIOPoint = _FIND_FROM_STRING(m_pIOList[m_IOList_nCount].nIOType, m_pIOList[m_IOList_nCount].szIOName);
	
	if(m_pIOList[m_IOList_nCount].nIOPoint != -1)				m_IOList_nCount++;
	else
	{
		printf("-----> Invalid IO List (%s %s %s) in %s\n", m_pIOList[m_IOList_nCount].szIOName,
				szItem, m_pIOList[m_IOList_nCount].szValue, APP_NAME);
	}
	
}

void CPMMainSimulator::ParseStageHeaterCfg(char *szRead)
{
	char szItem[40] = "";
	CTextParser tp(szRead);
	if(0 == strlen(szRead)) return;
	tp.GetWord(szItem, 0);
	
	if(strcmp(szItem, "TempError") == 0) tp.GetFloat(&m_pPMStgHtrSimulator->m_fStgHtrError, 0);
	else if(strcmp(szItem, "TempRamp") == 0) tp.GetFloat(&m_pPMStgHtrSimulator->m_fStgHtrRamp, 0);
	else if(strcmp(szItem, "SimulationEnable") == 0) tp.GetBool(&m_pPMStgHtrSimulator->m_bSimEnable, 0);
}

void CPMMainSimulator::ParseAlarmClearCfg(char *szRead)
{
	char szItem[40] = "";
	int nAlarmID;
	nAlarmID = atoi(szRead);
	if(nAlarmID > 0 && m_nAlarmClearCount < MAX_ALARM_CLEAR_LIST)
	{
		m_pnAlarmClearList[m_nAlarmClearCount]  = nAlarmID;
		m_nAlarmClearCount++;
	}
}

// 2008.08.25
void CPMMainSimulator::ParseIOSyncList(char *szRead)
{
	char szChName1[40] = "";
	char szChName2[40] = "";
	char szIOType[40] = "";

	CTextParser tp(szRead);
	if(! tp.GetWord(szChName1)) return;
	tp.GetWord(szIOType);
	tp.GetWord(szChName2);
	m_pIOSyncMonitor->SetIOSyncList(szChName1, szChName2, szIOType);
}