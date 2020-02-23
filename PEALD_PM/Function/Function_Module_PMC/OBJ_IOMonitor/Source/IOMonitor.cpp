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

	// 2011.09.29
	strcpy(m_szConfigPath, szConfigPath);
	strcpy(m_szLogFilePath, szLogFilePath);

	if(szLogFilePath == NULL) _gINIT_LOG();
	else _gInitLogDll(m_szLogFilePath, CFG_FILE_PATH, FALSE);

	if(! ReadPmcConfig(m_szConfigPath)) bRet = false;
	if(m_nIOCount > 0) InitIOList(m_pIOList);
	CreateThread();
	return bRet;
}

void CIOMonitor::Deinitialize()
{
	DestroyThread();
	_gCLOSE_LOG();
}


Module_Status CIOMonitor::Main()
{
	Module_Status msRet = SYS_ABORTED;
	char szParam[80], szCmd[40], szSubParam[40];
	CTextParser tp;

	strcpy(szParam, PROGRAM_PARAMETER_READ());
	tp.SetText(szParam);

	do {
		tp.GetWord(szCmd);
		tp.GetWord(szSubParam);

		// 2011.09.29
		if(tp.BeEqual(szCmd, "RELOAD_CFG")) msRet = ReloadCfg();
		// else if(tp.BeEqual(szCmd, "SET_PARAM")) msRet = SetParam(szSubParam);

	} while(0);

	return msRet;
}

Module_Status CIOMonitor::ReloadCfg()								// 2011.09.29
{
	Module_Status msRet = SYS_SUCCESS;

	printf("-----> Reload Configuration File\n");
	do {
		DestroyThread();
		m_nIOCount = 0;
		if(! ReadPmcConfig(m_szConfigPath))
		{
			printf("-----> Reload failed\n");
			msRet = SYS_ABORTED;
			break;
		}
		if(m_nIOCount > 0) InitIOList(m_pIOList);
		CreateThread();
		printf("-----> Reload OK\n");
	} while(0);

	return msRet;
}

Module_Status CIOMonitor::SetParam(char *szParam)					// 2011.09.29
{
	Module_Status msRet = SYS_SUCCESS;


	return msRet;
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
	char szIOName[80];
	char szIOType[8];
	char szTemp[80];

	// 2011.09.29
	int nFileCreatePeriod = 0;
	int nKeepDays = 0;

	CTextParser txtP(szData);

	txtP.GetWord(szIOName);
	if(txtP.BeEqual(szIOName, "MonitoringPeriod"))
	{
		txtP.GetInt('=', &m_nMonitoringPeriold);
		if(m_nMonitoringPeriold < 100) m_nMonitoringPeriold = 100;
		printf("--------> MonPeriod = %d ms (in IOMonitor)\n", m_nMonitoringPeriold);
	}

	// 2011.09.29
	else if(txtP.BeEqual(szIOName, "FileCreatePeriod"))
	{
		txtP.GetInt('=', &nFileCreatePeriod);
		if(nFileCreatePeriod > 0)
		{
			sprintf(szTemp, "SET_FILE_CR_P_HOURS %d %d", g_nInstanceID, nFileCreatePeriod);
			_gTextLogDll_Command(szTemp, NULL);	
			printf("--------> FileCreatePeriod = %d hours (in IOMonitor)\n", nFileCreatePeriod);
		}
	}
	else if(txtP.BeEqual(szIOName, "KeepDays"))
	{
		txtP.GetInt('=', &nKeepDays);
		if(nKeepDays > 0)
		{
			sprintf(szTemp, "SET_KEEP_DAYS %d %d", g_nInstanceID, nKeepDays);
			_gTextLogDll_Command(szTemp, NULL);
			printf("--------> KeepDays = %d days (in IOMonitor)\n", nKeepDays);
		}
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
	char szBuf[256] = "";
	char szTemp[40] = "";
	int i;
	int nCount;

	nCount = 0;
	for(i=0; i<m_nIOCount; i++)
	{
		sprintf(szTemp, "\t%s", m_pIOList[i].szIOName);
		strcat(szBuf, szTemp);
		if(++nCount >= 10)				// 2010.03.17
		{
			_LOG(szBuf);
			nCount = 0;
		}
	}
	if(nCount > 0) _LOG(szBuf);
}

void CIOMonitor::LogIOValue()
{
	char szBuf[256] = "";
	char szStrBuf[256];
	char szTemp[40] = "";
	BOOL bIOStatus;
	int i;

	for(i=0; i<m_nIOCount; i++)
	{
		switch(m_pIOList[i].nIOType)
		{
		case _K_D_IO: sprintf(szTemp, "\t%d", dREAD_DIGITAL(i, &bIOStatus)); break;
		case _K_A_IO: sprintf(szTemp, "\t%.3f", dREAD_ANALOG(i, &bIOStatus)); break;
		case _K_S_IO: dREAD_STRING(i, szStrBuf, &bIOStatus);
				szStrBuf[39] = 0;
				if(strlen(szStrBuf) > 0) sprintf(szTemp, "\t%s", szStrBuf);
				else strcpy(szTemp, "\t---");
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
			if(_gIS_LOG_FILE_CHANGED()) LogIOName();					// 2009.04.01
			LogIOValue();
			// if(++i >= 60) { LogIOName(); i = 0; }
		}
	}
}