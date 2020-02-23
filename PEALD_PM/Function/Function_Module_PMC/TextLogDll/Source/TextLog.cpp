// TextLog.cpp : Defines the entry point for the DLL application.
//

//////////////////////////////////////////////////////////////////////
// CTextLog Class
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "TextLog.h"
#include "TextParser.h"
#include "TextFile.h"

// Definition
#define FILE_OPTION_BIT				0xf0
#define START_T_OPTION_BIT			0x01

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextLog::CTextLog(const char* szFilePath, int nOption)
{
	InitializeCriticalSection(&m_csLock);

	m_nFileOption = nOption & FILE_OPTION_BIT;
	m_nConfigLogOption = nOption & START_T_OPTION_BIT;

	m_nKeepDays = -1;
	m_nKeepMonths = -1;

	m_nDeleteRangeDays = 365;
	m_nDeleteRangeMonths = 12;

	// Init Log ID Enable/Disable
	int i;
	for(i=1; i<MAX_LOG_ID; i++)
	{
		m_bLogEnable[i] = false;
		m_szLogDesc[i][0] = 0;
	}
	m_bLogEnable[0] = true;		// Default Log ID Enabled

	strcpy(m_szLogPath, szFilePath);
	const char* szLogStart = "------- Log started -------";
	CTextFile txtF;

	switch(m_nFileOption)
	{
	case 0:
		if(LG_OPT_CONFIG_LOG == m_nConfigLogOption)
		{
			if(txtF.FileExist(m_szLogPath))
			{
				if(txtF.Open(m_szLogPath, TEXT_FILE_APPEND))
				{
					txtF.WriteString("\n");
					txtF.Close();
				}
			}
			LogConfig(szLogStart);
		}
		break;
	case FILE_OPT_CREATE:
		txtF.Open(m_szLogPath, TEXT_FILE_CREATE);
		txtF.Close();
		LogConfig(szLogStart);
		break;
	case FILE_OPT_CHECK:
		if(txtF.FileExist(m_szLogPath))
		{
			if(LG_OPT_CONFIG_LOG == m_nConfigLogOption)
			{
				txtF.Open(m_szLogPath, TEXT_FILE_APPEND);
				txtF.WriteString("\n");
				txtF.Close();
				LogConfig(szLogStart);
			}
		}
		else
		{
			txtF.Open(m_szLogPath, TEXT_FILE_CREATE);
			txtF.Close();
			LogConfig(szLogStart);
		}
		break;
	case FILE_OPT_CREATE_PER_DAY:
	case FILE_OPT_CREATE_PER_MONTH:
		ParseFileName();
		break;
	}
}

CTextLog::~CTextLog()
{
	DestroyThrd();
	DeleteCriticalSection(&m_csLock);
}

//////////////////////////////////////////////////////////////////////
// Thread Function

void CTextLog::DoMonitor()
{
	SYSTEMTIME sysTime;
	int i, j;
	long lTime = 0;

	GetLocalTime(&sysTime);
	for(j=0; j<10 && m_bThreadRun; j++) { Sleep(1000); lTime++; }

	if(m_nFileOption == FILE_OPT_CREATE_PER_DAY)
	{
		LastDateByDays(&sysTime, m_nKeepDays);
		for(i=0; i<m_nDeleteRangeDays && m_bThreadRun; i++)
		{
			LastDateByDays(&sysTime, 1);
			DeleteLogFile(&sysTime);
			for(j=0; j<10 && m_bThreadRun; j++) { Sleep(1000); lTime++; }
		}
		m_nDeleteRangeDays = 2;
		while(lTime < 20*60*60 && m_bThreadRun) { Sleep(1000); lTime++; }
	}
	else if(m_nFileOption == FILE_OPT_CREATE_PER_MONTH)
	{
		LastDateByMonths(&sysTime, m_nKeepMonths);
		for(i=0; i<m_nDeleteRangeMonths && m_bThreadRun; i++)
		{
			LastDateByMonths(&sysTime, 1);
			DeleteLogFile(&sysTime);
			for(j=0; j<10 && m_bThreadRun; j++) { Sleep(1000); lTime++; }
		}
		m_nDeleteRangeMonths = 1;
		while(lTime < 25*20*60*60 && m_bThreadRun) { Sleep(1000); lTime++; }
	}
}

void CTextLog::LastDateByDays(SYSTEMTIME *pSysTime, int nPDays)
{
	int nYear, nMonth, nDay, nLastMonth;
	nYear = pSysTime->wYear;
	nMonth = pSysTime->wMonth;
	nDay = pSysTime->wDay;

	nDay -= nPDays;
	while(nDay < 1)
	{
		nLastMonth = nMonth - 1;
		if(nLastMonth < 1) nLastMonth = 12;
		switch(nLastMonth)
		{
		case 4: case 6: case 9: case 11: nDay += 30; break;
		case 2: nDay += 29; break;
		default: nDay += 31; break;
		}
		nMonth--;
		if(nMonth < 1)
		{
			nMonth += 12;
			nYear--;
		}
	}

	pSysTime->wYear = nYear;
	pSysTime->wMonth = nMonth;
	pSysTime->wDay = nDay;
}

void CTextLog::LastDateByMonths(SYSTEMTIME *pSysTime, int nPMonths)
{
	int nYear, nMonth, nDay;
	nYear = pSysTime->wYear;
	nMonth = pSysTime->wMonth;
	nDay = pSysTime->wDay;

	nMonth -= nPMonths;
	while(nMonth < 1)
	{
		nMonth += 12;
		nYear--;
	}

	pSysTime->wYear = nYear;
	pSysTime->wMonth = nMonth;
	pSysTime->wDay = nDay;
}

void CTextLog::DeleteLogFile(SYSTEMTIME *pSysTime)
{
	char szFilePath[256];
	GetLogFileName(szFilePath, pSysTime);
	DeleteFile(szFilePath);
}

//////////////////////////////////////////////////////////////////////
// Member Variable

void CTextLog::SetCfgLogID(const char *szCfg)
{
	int nLogID;
	char szLogDesc[256];
	char szLog[100];
	char szItem[100];
	char szLogEnable[100];
	CTextParser txtP(szCfg);
	txtP.GetWord(szItem);

	nLogID = atoi(szItem+6);
	txtP.GetWord('=', szLogEnable);
	txtP.GetString(szLogDesc);
	if(nLogID<0 || nLogID>=MAX_LOG_ID)
	{
		LogTextF("Invalid LogDI(%d) !", nLogID);
		return; 
	}

	if('E' == szLogEnable[0] || 'e' == szLogEnable[0])
	{
		if(szLogDesc[0] == '/')
			strncpy(m_szLogDesc[nLogID], szLogDesc, 79);
		m_bLogEnable[nLogID] = true;
		sprintf(szLog, "LogID(%d) => Enabled		%s",
					nLogID, m_szLogDesc[nLogID]);
		LogConfig(szLog);
	}
	else
	{
		m_bLogEnable[nLogID] = false;
		if(szLogDesc[0] == '/')
			strncpy(m_szLogDesc[nLogID], szLogDesc, 79);
		sprintf(szLog, "LogID(%d) => Disabled		%s",
					nLogID, m_szLogDesc[nLogID]);
		LogConfig(szLog);
	}
}

void CTextLog::SetCfgKeepDays(const char *szCfg)
{
	CTextParser txtP(szCfg);
	txtP.GetInt('=', &m_nKeepDays);

	char szLog[100];
	sprintf(szLog, "KeepDays = %d", m_nKeepDays);
	LogConfig(szLog);
}

void CTextLog::SetCfgKeepMonths(const char *szCfg)
{
	CTextParser txtP(szCfg);
	txtP.GetInt('=', &m_nKeepMonths);

	char szLog[100];
	sprintf(szLog, "KeepMonths = %d", m_nKeepMonths);
	LogConfig(szLog);
}

void CTextLog::InitConfigration(const char* szCfgFile)
{
	LogConfig("============================================================");
	LogConfig("Configration Started");

	char szLogFile[100];
	char szBuf[256];
	CTextFile txtF;
	do {
		if(ParseFileFullName() == false)
		{
			LogText("LogFileName is invalid");
			break;
		}

		if(! txtF.Open(szCfgFile))
		{
			sprintf(szBuf, "Can't find Configration File('%s')", szCfgFile);
			LogConfig(szBuf);
			break;
		}
		else
		{
			LogConfig("Found Configration File and Read Configration Data");
		}

		txtF.ReadLine(szBuf);
		while(! txtF.IsEOF())
		{
			if(szBuf[0] == '[')
			{
				CTextParser txtP(szBuf+1);
				txtP.GetWord(szLogFile, ']');
				if(0 == strcmp(szLogFile, m_szLogFile))
				{
					while(! txtF.IsEOF())
					{
						txtF.ReadLine(szBuf);
						if(';' == szBuf[0] || NULL == szBuf[0])	continue;
						if('[' == szBuf[0]) break;
						if(0 == strncmp("LogID", szBuf, 5)) SetCfgLogID(szBuf);
						else if(0 == strncmp("KeepDays", szBuf, 7)) SetCfgKeepDays(szBuf);
						else if(0 == strncmp("KeepMonths", szBuf, 9)) SetCfgKeepMonths(szBuf);
						else LogTextF("Invalid Config Item : %s", szBuf);
					}
				}
				else txtF.ReadLine(szBuf);
			}
			else txtF.ReadLine(szBuf);
		}
		txtF.Close();
	}while(0);
	
	if(m_nFileOption == FILE_OPT_CREATE_PER_DAY)
	{
		if(m_nKeepDays == -1)
		{
			m_nKeepDays = 10;
			LogConfig("KeepDays is not defined. Default KeepDays is 10(days).");
		}
		if(m_nKeepDays > 0) CreateThrd();
	}
	if(m_nFileOption == FILE_OPT_CREATE_PER_MONTH)
	{
		if(m_nKeepMonths == -1)
		{
			m_nKeepMonths = 12;
			LogConfig("KeepMonths is not defined. Default KeepMonths is 12(months).");
		}
		if(m_nKeepMonths > 0) CreateThrd();
	}

	LogConfig("Configration Finished");
	LogConfig("============================================================");
}

bool CTextLog::LogConfig(const char *szLog)
{
	if(LG_OPT_CONFIG_LOG == m_nConfigLogOption) return LogText(szLog);
	else return false;
}

bool CTextLog::LogText(const char *szLog, int nLogID)
{
	if(! m_bLogEnable[nLogID]) return false;
	return LogText(szLog);
}

bool CTextLog::LogText(const char *szLog)
{
	bool bRet = false;
	EnterCriticalSection(&m_csLock);
	do{ 
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		char szBuf[256];
		char szFilePath[256];

		GetLogFileName(szFilePath, &sysTime);

		CTextFile txtF;
		if(false == txtF.Open(szFilePath, TEXT_FILE_APPEND)) break;
		sprintf(szBuf, "%02d/%02d %02d:%02d:%02d:%03d %s\n", sysTime.wMonth, sysTime.wDay,
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds,
			szLog);
		txtF.WriteString(szBuf);
		txtF.Close();
		bRet = true;
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}


bool CTextLog::LogTextF(const char *pFormat, ...)
{
	char _szLog[256];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szLog, pFormat, args);
	va_end(args);
	return LogText(_szLog);
}

bool CTextLog::LogInfo(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	bool bRet = false;
	EnterCriticalSection(&m_csLock);
	do{ 
		if(! m_bLogEnable[nLogID]) break;
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		char szBuf[256];
		char szFilePath[256];
		GetLogFileName(szFilePath, &sysTime);

		CTextFile txtF;
		if(false == txtF.Open(szFilePath, TEXT_FILE_APPEND)) break;
		sprintf(szBuf, "\nLogID: %d\n", nLogID);
		txtF.WriteString(szBuf);
		sprintf(szBuf, "Time : %02d/%02d %02d:%02d:%02d:%03d\n", sysTime.wMonth, sysTime.wDay,
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
		txtF.WriteString(szBuf);
		sprintf(szBuf, "File : %s\n", szSrcFile);
		txtF.WriteString(szBuf);
		sprintf(szBuf, "Line : %d\n", nSrcLine);
		txtF.WriteString(szBuf);
		sprintf(szBuf, "Log  : %s\n", szLog); 
		txtF.WriteString(szBuf);
		txtF.Close();
		bRet = true;
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

void CTextLog::GetLogFileName(char* szFilePath, SYSTEMTIME *pSysTime)
{
	switch(m_nFileOption)
	{
	case 0:
	case FILE_OPT_CREATE:
	case FILE_OPT_CHECK:
		strcpy(szFilePath, m_szLogPath);
		break;
	case FILE_OPT_CREATE_PER_DAY:
		char szBuf[256];
		sprintf(szBuf, "%s_%d_%02d_%02d.%s",
			m_szFile, pSysTime->wYear, pSysTime->wMonth, pSysTime->wDay, m_szExt);
		strcpy(szFilePath, szBuf);
		break;
	case FILE_OPT_CREATE_PER_MONTH:
		sprintf(szBuf, "%s_%d_%02d.%s", m_szFile, pSysTime->wYear, pSysTime->wMonth, m_szExt);
		strcpy(szFilePath, szBuf);
		break;
	}
}

bool CTextLog::ParseFileName()
{
	bool bRet = false;
	do {
		int nSize = strlen(m_szLogPath);
		if(nSize <= 0) break;
		int i, nIdx;
		for(i=0; i<FILE_NAME_MAX; i++)
		{
			m_szFile[i] = NULL;
			m_szExt[i] = NULL;
		}
		for(i=0; i<5; i++)
		{
			nIdx = nSize - 1 - i;
			if(nIdx <= 0) { nIdx = 0; break; }
			if('.' == m_szLogPath[nIdx]) break;
		}
		if('.' == m_szLogPath[nIdx])
		{
			strncpy(m_szFile, m_szLogPath, nIdx);
			strcpy(m_szExt, m_szLogPath + nIdx + 1);
		}
		else strcpy(m_szFile, m_szLogPath);
		bRet = true;
	} while(0);
	return bRet;
}

bool CTextLog::ParseFileFullName()
{
	int nSize = strlen(m_szLogPath);
	if(nSize <= 0) return false;

	int i;
	for(i=0; i<FILE_NAME_MAX; i++) m_szLogFile[i] = 0;
	for(i=nSize-1; i>=0; i--)
	{
		if( m_szLogPath[i] == '\\' ) break;
	}
	i++;
	strcpy(m_szLogFile, m_szLogPath+i);
	if(strlen(m_szLogFile) <= 0) return false;
	return true;
}
