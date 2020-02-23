// IOMonitor.cpp: implementation of the CIOMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>

//====================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//====================================================================

#include "TextLogDll_M.h"
#include "TextParser.h"
#include "IOMonitor.h"

#ifndef _DEBUG
// #define _DEBUG
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOMonitor::CIOMonitor()
{
	int i;
	for(i=0; i<MAX_MON_IO_LIST; i++)
	{
		m_pIOList[i].szIOName[0] = 0;
		m_pIOList[i].nIOPoint = -1;
	}
	m_nIOCount = 0;
	m_nMonitoringPeriold = 1000; /*ms*/
}

CIOMonitor::~CIOMonitor()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

bool CIOMonitor::Initialize(char *szConfigPath, char *szLogFilePath)
{
	bool bRet = true;

	if(szLogFilePath == NULL) _gINIT_LOG();
	else _gInitLogDll(szLogFilePath, CFG_FILE_PATH, FALSE);

	if(! ReadPmcConfig(szConfigPath)) bRet = false;
	if(m_nIOCount > 0) InitIOList(m_pIOList);
	CreateThread();
	return bRet;
}

void CIOMonitor::Deinitialize()
{
	DestroyThread();
	_gCLOSE_LOG();
}

bool CIOMonitor::ReadPmcConfig(char *szConfigPath)
{
	bool bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(szConfigPath, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("IO Monitor", szItem) == 0)							nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseIOMonitorCfg(szRead); break;
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
		printf("-----> Config File Reading Failed[%s] in CIOMonitor::Initialize\n", szConfigPath);
	}
	return bRet;
}

void CIOMonitor::ParseIOMonitorCfg(char *szData)
{
	char szIOName[40];
	char szIOType[8];
	CTextParser txtP(szData);


	txtP.GetWord(szIOName);
	if(txtP.BeEqual(szIOName, "MonitoringPeriod"))
	{
		txtP.GetInt('=', &m_nMonitoringPeriold);
		if(m_nMonitoringPeriold < 100) m_nMonitoringPeriold = 100;
	}
	else if(m_nIOCount < MAX_MON_IO_LIST)
	{
		do {
			if(strlen(szIOName) <= 0) break;
			strcpy(m_pIOList[m_nIOCount].szIOName, szIOName);
			if(! txtP.GetWord(szIOType)) break;
			switch(szIOType[0])
			{
			case 'D': case 'd': m_pIOList[m_nIOCount].nIOType = _K_D_IO; break;
			case 'A': case 'a': m_pIOList[m_nIOCount].nIOType = _K_A_IO; break;
			case 'S': case 's': m_pIOList[m_nIOCount].nIOType = _K_S_IO; break;
			default: m_pIOList[m_nIOCount].nIOType = -1; break;
			}

			#ifdef _DEBUG
			printf("-----> IOMonitor List: %s %s\n", m_pIOList[m_nIOCount].szIOName, szIOType);
			#endif
			m_nIOCount++;
		} while(0);
	}
}

void CIOMonitor::LogIOName()
{
	char szBuf[256] = "IONameList:";
	char szTemp[40] = "";
	int i;

	for(i=0; i<m_nIOCount; i++)
	{
		sprintf(szTemp, " %s", m_pIOList[i].szIOName);
		strcat(szBuf, szTemp);
	}
	_LOG(szBuf);
}

void CIOMonitor::LogIOValue()
{
	char szBuf[256] = "----->";
	char szStrBuf[256];
	char szTemp[40] = "";
	BOOL bIOStatus;
	int i;

	for(i=0; i<m_nIOCount; i++)
	{
		switch(m_pIOList[i].nIOType)
		{
		case _K_D_IO: sprintf(szTemp, "     %d", dREAD_DIGITAL(i, &bIOStatus)); break;
		case _K_A_IO: sprintf(szTemp, "   %.3f", dREAD_ANALOG(i, &bIOStatus)); break;
		case _K_S_IO: dREAD_STRING(i, szStrBuf, &bIOStatus);
				szStrBuf[39] = 0;
				if(strlen(szStrBuf) > 0) sprintf(szTemp, "  %s", szStrBuf);
				else strcpy(szTemp, "   ---");
				break;
		default : strcpy(szTemp, "???");
		}
		strcat(szBuf, szTemp);
	}
	_LOG(szBuf);
}

//////////////////////////////////////////////////////////////////////
// Virtual Function for Thread

void CIOMonitor::DoMonitor()
{
	int i = 0;
	if(m_nIOCount > 0)
	{
		LogIOName();
		while(WaitMillSec(m_nMonitoringPeriold))
		{
			LogIOValue();
			if(++i >= 60) { LogIOName(); i = 0; }
		}
	}
}