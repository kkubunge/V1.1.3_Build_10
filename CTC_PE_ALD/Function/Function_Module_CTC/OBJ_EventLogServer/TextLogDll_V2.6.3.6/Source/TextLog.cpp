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
#include "Timer.h"
#include "DataQueue.h"		// 2006.07.24

#include "MsgBoxDll.h"
#define _ERRMSG(_SZ_MSG)	_OUTMSG(_SZ_MSG)

// Thread Func Definition
enum {
	THRD0_DATE_MONITOR,
	THRD1_MSG_SENDER,
	THRD2_FILE_SAVER,
	THRD3_TIME_MONITOR,
	THRD4_ZIP_MONITOR,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextLog::CTextLog(const char* szFilePath, int nOption, bool bMsgEnable, int nBufferSize, bool bBufferingEnable)
{
	SYSTEMTIME sysTime;
	char szMsg[80];
	int i;
	char szFile[FILE_PATH_SIZE], szExt[80];
	const char* szLogStart = "------- Log started -------";
	CTextFile txtF;

	_gINIT_MSGBOX("CTextLog");
	InitializeCriticalSection(&m_csLock);
	InitializeCriticalSection(&m_csQueueDataSaveLock);		// 2006.07.26

	// 2013.06.13
	m_nLogType = LOG_TYPE1;
	strcpy(m_szDelimiter, " ");

	// 2011.09.30
	strcpy(m_szLastLogFileName, "");
	m_nLastHour = -1;

	//----------------------------------------------------
	// 2006.08.23
	m_bBufferingEnable = bBufferingEnable;
	if((!m_bBufferingEnable) || nBufferSize<8) nBufferSize = 8;

	// 2006.07.24
	m_pFileQueue = new CDataQueue(LOG_LINE_MAX_SIZE, nBufferSize);

	//----------------------------------------------------
	// 2005.10.10, for Log Message Send
	strcpy(m_szLogPath, szFilePath);
	if(! ParseFileFullName())
	{
		m_bLogEnable = false;
		_ERRMSG("Log File Name is invalid !");
	}

	CTextParser txtP;
	ParseFileName(m_szLogFile, szFile, szExt);
	txtP.MakeUpper(szFile);
	txtP.MakeUpper(szExt);

	m_szClientName[0] = 0;
	m_nClientStatus = INTF_DISCONNECTED;
	m_bMsgDelay = false;
	m_szMsgErr[0] = 0;
	sprintf(m_szClientName, "TEXT_LOG_GB_MSG_CLIENT_%s_%s", szFile, szExt);
	if(bMsgEnable)
	{
		// 2006.03.16
		if(InitMsgMngr())
		{
			Command("NAME_AUTO_CREATION YES", NULL);
			if(CreateInterface(m_szClientName, "TEXT_LOG_GB_MSG_SERVER"))
				Command("GET_SERVER_NAME", m_szClientName);
		}
	}
	if(m_nInstanceID >= 0) 
	{
		m_nClientStatus = GetClientStatus();
		if(m_nClientStatus != INTF_DISCONNECTED)
		{
			sprintf(szMsg, "#Client_Name = %s", m_szClientName);
			SendMsg(szMsg);
		}
	}
	//----------------------------------------------------

	m_pMsgTimer = new CTimer;
	m_pMsgTimer->StartTimer();
	m_bLogEnable = true;

	m_nFileOption = nOption & FILE_OPTION_BIT;
	m_nConfigLogOption = nOption & START_T_OPTION_BIT;
	m_nFileCreatePeriod_Hours = 3;		// 2011.07.27
	m_bInitFlag = false;				// 2008.08.25

	// 2009.04.01
	GetLocalTime(&sysTime);
	SetLastLogTime(&sysTime);

	// 2008.08.22
	m_nKeepDays = 10;
	m_nKeepMonths = 12;

	m_nDeleteRangeDays = 365;
	m_nDeleteRangeMonths = 12;

	// Init Log ID Enable/Disable
	for(i=1; i<MAX_LOG_ID; i++)
	{
		m_bLogIDEnable[i] = false;
		m_bMsgIDEnable[i] = false;
		m_szLogDesc[i][0] = 0;
	}
	m_bLogIDEnable[0] = true;		// Default Log ID Enabled
	m_bMsgIDEnable[0] = false;

	switch(m_nFileOption)
	{
	case 0:
		if(LG_OPT_CONFIG_LOG == m_nConfigLogOption)
		{
			if(txtF.FileExist(m_szLogPath))
			{
				m_pFileQueue->Push("");
			}
			LogConfig(szLogStart);
		}
		break;
	case FILE_OPT_CREATE:
		txtF.DeleteFile(m_szLogPath);
		LogConfig(szLogStart);
		break;
	case FILE_OPT_CHECK:
		if(txtF.FileExist(m_szLogPath))
		{
			if(LG_OPT_CONFIG_LOG == m_nConfigLogOption)
			{
				m_pFileQueue->Push("");
				LogConfig(szLogStart);
			}
		}
		else
		{
			txtF.DeleteFile(m_szLogPath);
			LogConfig(szLogStart);
		}
		break;
	case FILE_OPT_CREATE_PER_DAY:
	case FILE_OPT_CREATE_PER_MONTH:
	case FILE_OPT_CREATE_PER_HOURS:					// 2011.07.27
		ParseFileName(m_szLogPath, m_szFile, m_szExt);
		break;
	}

	if(m_nInstanceID >= 0) CreateThread(THRD1_MSG_SENDER);
	CreateThread(THRD2_FILE_SAVER);					// 2006.07.24
	CreateThread(THRD3_TIME_MONITOR);				// 2013.06.03
	CreateThread(THRD4_ZIP_MONITOR);				// 2014.08.03

}

CTextLog::~CTextLog()
{
	DestroyAllThread();
	DeinitMsgMngr();
	DeleteCriticalSection(&m_csLock);
	DeleteCriticalSection(&m_csQueueDataSaveLock);	// 2006.07.26
	delete m_pMsgTimer;
	delete m_pFileQueue;					// 2006.07.24

	_gDEINIT_MSGBOX();
}

//////////////////////////////////////////////////////////////////////
// Thread Function

void CTextLog::DoMonitor(int nThreadNo)
{
	switch(nThreadNo)
	{
	case THRD0_DATE_MONITOR:	ThrdDateMonitor(nThreadNo);  break;
	case THRD1_MSG_SENDER:		ThrdMsgSender(nThreadNo);    break;
	case THRD2_FILE_SAVER:		ThrdFileSaver(nThreadNo);    break;
	case THRD3_TIME_MONITOR:	ThrdTimeMonitor(nThreadNo);  break;
	case THRD4_ZIP_MONITOR:		ThrdZipMonitor(nThreadNo);   break;
	}
}

void CTextLog::ThrdDateMonitor(int nThreadNo)
{
	SYSTEMTIME sysTime;
	int i;

	while(WAIT_SEC(10))
	{
		GetLocalTime(&sysTime);
		if(m_nFileOption == FILE_OPT_CREATE_PER_DAY)
		{
			LastDateByDays(&sysTime, m_nKeepDays);
			for(i=0; i<m_nDeleteRangeDays; i++)
			{
				LastDateByDays(&sysTime, 1);
				DeleteLogFile(&sysTime);
				if(! WAIT_SEC(10)) break;
			}
			m_nDeleteRangeDays = 2;
			WAIT_SEC(20*60*60);
		}
		else if(m_nFileOption == FILE_OPT_CREATE_PER_MONTH)
		{
			LastDateByMonths(&sysTime, m_nKeepMonths);
			for(i=0; i<m_nDeleteRangeMonths; i++)
			{
				LastDateByMonths(&sysTime, 1);
				DeleteLogFile(&sysTime);
				if(! WAIT_SEC(10)) break;
			}
			m_nDeleteRangeMonths = 1;
			WAIT_SEC(25*20*60*60);
		}
		else if(m_nFileOption == FILE_OPT_CREATE_PER_HOURS)				// 2011.07.27
		{
			LastDateByDays(&sysTime, m_nKeepDays);
			for(i=0; i<m_nDeleteRangeDays; i++)
			{
				LastDateByDays(&sysTime, 1);
				DeleteLogFile(&sysTime);								// 2011.09.30
				DeleteLogFile_Hours(&sysTime, nThreadNo);
				if(! WAIT_SEC(10)) break;
			}
			m_nDeleteRangeDays = 2;
			WAIT_SEC(20*60*60);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Member Function

// 2011.07.27
void CTextLog::SetFileCreatePeriodPerHours(int nHours)
{
//	char szLog[LOG_LINE_MAX_SIZE];
//	LogConfig("Set File Create Period");

	// 2011.09.29
	if(nHours < 24)
	{
		m_nFileOption = FILE_OPT_CREATE_PER_HOURS;
		if(nHours >= 1 && nHours <= 12)
		{
			m_nFileCreatePeriod_Hours = nHours;
		}
	}
	else
	{
		m_nFileOption = FILE_OPT_CREATE_PER_DAY;
	}

	//	sprintf(szLog, "Period = %d hours", nHours);
//	LogConfig(szLog);
}

// 2011.07.27
void CTextLog::SetKeepDays(int nKeepDays)
{
//	char szLog[LOG_LINE_MAX_SIZE];
//	LogConfig("Set Log File KeepDays From Set Func Call");

	m_nKeepDays = nKeepDays;

//	sprintf(szLog, "KeepDays = %d days", m_nKeepDays);
//	LogConfig(szLog);
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
	char szFilePath[FILE_PATH_SIZE];
	GetLogFileName(szFilePath, pSysTime);
	DeleteFile(szFilePath);
}

// 2011.7.27
void CTextLog::DeleteLogFile_Hours(SYSTEMTIME *pSysTime, int nThreadNo)
{
	int i;
	char szFilePath[FILE_PATH_SIZE];

	for(i=0; i<24; i+=m_nFileCreatePeriod_Hours)
	{
		GetLogFileName_Hour(szFilePath, pSysTime, i);
		DeleteFile(szFilePath);
		if(! WAIT_SEC(5)) break;
	}
}

//--------------------------------------------------------------------
// 2006.07.24
void CTextLog::ThrdFileSaver(int nThreadNo)
{
	while(WAIT_SEC(3))
	{
		if(m_pFileQueue->IsEmpty()) continue;
		SaveQueueData();
	}
	if(! m_pFileQueue->IsEmpty()) SaveQueueData();
}

// 2006.07.26
bool CTextLog::SaveQueueData(bool bRecreateLogFile)
{
	bool bRet = false;
	char szBuf[LOG_LINE_MAX_SIZE];
	char szFilePath[FILE_PATH_SIZE];
	CTextFile txtF;
	SYSTEMTIME sysTime;

	EnterCriticalSection(&m_csQueueDataSaveLock);
	do {
		if(m_pFileQueue->IsEmpty()) { bRet = true; break; }

		// 2011.09.30
		if(true == bRecreateLogFile || strlen(m_szLastLogFileName) == 0)
		{
			GetLocalTime(&sysTime);
			GetLogFileName(szFilePath, &sysTime);
			strcpy(m_szLastLogFileName, szFilePath);
		}
		else
		{
			strcpy(szFilePath, m_szLastLogFileName);
		}

		if(! txtF.Open(szFilePath, TEXT_FILE_APPEND)) break;
		do  {
			m_pFileQueue->Pop(szBuf);
			txtF.WriteLine(szBuf);
		} while(! m_pFileQueue->IsEmpty());
		txtF.Close();
		bRet = true;
	} while(0);

	LeaveCriticalSection(&m_csQueueDataSaveLock);
	return bRet;
}

// 2006.07.26
bool CTextLog::PushDataToQueue(char *szData, bool bHourChanged)	// 2011.09.30
{
	char szMsg[MSG_BUFFER_SIZE];								// 2011.07.27

	if(m_bBufferingEnable)
	{
		if(m_pFileQueue->IsFull() || bHourChanged)
		{
			if(! SaveQueueData(false))							// 2006.08.07
			{
				strcpy(szMsg, "Log Data Saving Failed so Log Data Was Lost !");
				_ERRMSG(szMsg);
				m_pFileQueue->ClearBuffer();
				m_pFileQueue->Push(szMsg);
			}
		}
		return m_pFileQueue->Push(szData);
	}
	else
	{
		bool bRet = true;
		if(! m_pFileQueue->Push(szData)) bRet = false;
		if(! SaveQueueData()) bRet = false;
		return bRet;
	}
}

void CTextLog::ParseCfgLogID(const char *szCfg)
{
	int nLogID;
	char szLogDesc[256];
	char szLog[LOG_LINE_MAX_SIZE];
	char szItem[100];
	char szLogEnable[100];
	CTextParser txtP(szCfg);
	txtP.GetWord(szItem);

	nLogID = atoi(szItem+6);
	txtP.GetWord('=', szLogEnable);
	txtP.GetString(szLogDesc);
	if(nLogID<0 || nLogID>=MAX_LOG_ID)
	{
		LogTextF("Invalid LogID(%d) !", nLogID);
		return; 
	}

	if('E' == szLogEnable[0] || 'e' == szLogEnable[0])
	{
		if(szLogDesc[0] == '/')
			strncpy(m_szLogDesc[nLogID], szLogDesc, 79);
		m_bLogIDEnable[nLogID] = true;
		sprintf(szLog, "LogID(%d) => Enabled		%s",
					nLogID, m_szLogDesc[nLogID]);
		LogConfig(szLog);
	}
	else
	{
		m_bLogIDEnable[nLogID] = false;
		if(szLogDesc[0] == '/')
			strncpy(m_szLogDesc[nLogID], szLogDesc, 79);
		sprintf(szLog, "LogID(%d) => Disabled		%s",
					nLogID, m_szLogDesc[nLogID]);
		LogConfig(szLog);
	}
}

void CTextLog::ParseCfgMsgID(const char *szCfg)
{
	int nLogID;
	char szComment[256];
	char szLog[LOG_LINE_MAX_SIZE];
	char szItem[100];
	char szMsgEnable[100];
	CTextParser txtP(szCfg);
	txtP.GetWord(szItem);

	nLogID = atoi(szItem+6);
	txtP.GetWord('=', szMsgEnable);
	txtP.GetString(szComment);
	if(nLogID<0 || nLogID>=MAX_LOG_ID)
	{
		LogTextF("Invalid MsgID(%d) !", nLogID);
		return; 
	}

	if('E' == szMsgEnable[0] || 'e' == szMsgEnable[0])
	{
		m_bMsgIDEnable[nLogID] = true;
		sprintf(szLog, "MsgID(%d) => Enabled		%s",
					nLogID, szComment);
		LogConfig(szLog);
	}
	else
	{
		m_bMsgIDEnable[nLogID] = false;
		sprintf(szLog, "MsgID(%d) => Disabled		%s",
					nLogID, szComment);
		LogConfig(szLog);
	}
}

void CTextLog::ParseCfgKeepDays(const char *szCfg)
{
	CTextParser txtP(szCfg);
	txtP.GetInt('=', &m_nKeepDays);

	char szLog[100];
	sprintf(szLog, "KeepDays = %d", m_nKeepDays);
	LogConfig(szLog);
}

void CTextLog::ParseCfgKeepMonths(const char *szCfg)
{
	CTextParser txtP(szCfg);
	txtP.GetInt('=', &m_nKeepMonths);

	char szLog[100];
	sprintf(szLog, "KeepMonths = %d", m_nKeepMonths);
	LogConfig(szLog);
}

// 2008.08.25
void CTextLog::InitConfiguration()
{
	if(m_nFileOption == FILE_OPT_CREATE_PER_DAY)
	{
		// 2008.08.22
		if(m_nKeepDays == 10)
				LogConfig("Default KeepDays is 10(days).");
		if(m_nKeepDays > 0) CreateThread(THRD0_DATE_MONITOR);
	}
	if(m_nFileOption == FILE_OPT_CREATE_PER_MONTH)
	{
		// 2008.08.22
		if(m_nKeepMonths == 12)
			LogConfig("Default KeepMonths is 12(months).");
		if(m_nKeepMonths > 0) CreateThread(THRD0_DATE_MONITOR);
	}
	if(m_nFileOption == FILE_OPT_CREATE_PER_HOURS)
	{
		// 2008.08.22
		if(m_nKeepDays == 10)
				LogConfig("Default KeepDays is 10(days).");
		if(m_nKeepDays > 0) CreateThread(THRD0_DATE_MONITOR);
	}

	LogConfig("Configration Finished");
	LogConfig("============================================================");
	if(m_nInstanceID >= 0)
	{
		m_nClientStatus = GetClientStatus();
		if(m_nClientStatus != INTF_DISCONNECTED) LogConfig("Global Message Interface is connected !");
	}

	// 2013.06.13
	switch(m_nLogType)
	{
	case LOG_TYPE1: strcpy(m_szDelimiter, " "); break;
	case LOG_TYPE2: strcpy(m_szDelimiter, "\t");break;
	default: strcpy(m_szDelimiter, " "); break;
	}
	m_bInitFlag = true;
}

void CTextLog::InitConfiguration(const char* szCfgFile)
{
	LogConfig("============================================================");
	LogConfig("Configration Started");

	char szLogFile[100];
	char szBuf[LOG_LINE_MAX_SIZE];
	
	CTextFile txtF;
	// 2008.08.25
	if(szCfgFile != NULL && szCfgFile[0] != 0) do {
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
						if(';' == szBuf[0] || '#' == szBuf[0] || NULL == szBuf[0])	continue;
						if('[' == szBuf[0]) break;
						if(0 == strncmp("LogID", szBuf, 5)) ParseCfgLogID(szBuf);
						else if(0 == strncmp("MsgID", szBuf, 5)) ParseCfgMsgID(szBuf);
						else if(0 == strncmp("KeepDays", szBuf, 7)) ParseCfgKeepDays(szBuf);
						else if(0 == strncmp("KeepMonths", szBuf, 9)) ParseCfgKeepMonths(szBuf);
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
		// 2008.08.22
		if(m_nKeepDays == 10)
				LogConfig("Default KeepDays is 10(days).");
		if(m_nKeepDays > 0) CreateThread(THRD0_DATE_MONITOR);
	}
	else if(m_nFileOption == FILE_OPT_CREATE_PER_MONTH)
	{
		// 2008.08.22
		if(m_nKeepMonths == 12)
			LogConfig("Default KeepMonths is 12(months).");
		if(m_nKeepMonths > 0) CreateThread(THRD0_DATE_MONITOR);
	}
	// 2011.07.27
	else if(m_nFileOption == FILE_OPT_CREATE_PER_HOURS)
	{
		if(m_nKeepDays == 10)
				LogConfig("Default KeepDays is 10(days).");
		if(m_nKeepDays > 0) CreateThread(THRD0_DATE_MONITOR);
	}

	LogConfig("Configration Finished");
	LogConfig("============================================================");
	if(m_nInstanceID >= 0)
	{
		m_nClientStatus = GetClientStatus();
		if(m_nClientStatus != INTF_DISCONNECTED) LogConfig("Global Message Interface is connected !");
	}

	// 2013.06.13
	switch(m_nLogType)
	{
	case LOG_TYPE1: strcpy(m_szDelimiter, " "); break;
	case LOG_TYPE2: strcpy(m_szDelimiter, "\t");break;
	default: strcpy(m_szDelimiter, " "); break;
	}
	m_bInitFlag = true;
}

bool CTextLog::LogConfig(const char *szLog)
{
	bool bRet = false;

	// 2011.07.27
	char szBuf[LOG_LINE_MAX_SIZE];
	char szTimeTag[40];

	if(! m_bLogEnable || LG_OPT_CONFIG_LOG != m_nConfigLogOption)
	{
		if(! SendLogMsg(szLog)) WriteLogToFile(m_szMsgErr);
		return false;
	}
	EnterCriticalSection(&m_csLock);
	do{ 
		CreateTimeTag(szTimeTag);
		sprintf(szBuf, "%s %s", szTimeTag, szLog);
		bRet = m_pFileQueue->Push(szBuf);

		if(! SendMssge(szBuf))
		{
			sprintf(szBuf, "%s %s", szTimeTag, m_szMsgErr);
			m_pFileQueue->Push(szBuf);
		}
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

bool CTextLog::LogText(const char *szLog, int nLogID)
{
	bool bRet = false;
	//----------------------------------------------
	// 2006.07.24
	char szLogBuf[LOG_LINE_MAX_SIZE];
	const char *pLog;
	int nLen;

	// 2011.07.27
	char szBuf[LOG_LINE_MAX_SIZE];
	char szTimeTag[40];

	bool bHourChanged;		// 2011.09.30

	pLog = szLog;
	nLen = strlen(szLog);
	if(nLen > 1)
	{
		if(szLog[nLen-1] == '\n')
		{
			strcpy(szLogBuf, szLog);
			szLogBuf[nLen-1] = 0;
			pLog = szLogBuf;
		}
	}
	//----------------------------------------------

	if(! m_bInitFlag) InitConfiguration();
	if(! m_bLogEnable || ! m_bLogIDEnable[nLogID])
	{
		if(! SendLogMsg(pLog, nLogID)) WriteLogToFile(m_szMsgErr);
		return false;
	}
	EnterCriticalSection(&m_csLock);
	do{
		bHourChanged = CreateTimeTag(szTimeTag);
		sprintf(szBuf, "%s%s%s", szTimeTag, m_szDelimiter, pLog);
		bRet = PushDataToQueue(szBuf, bHourChanged);

		if(! SendMssge(szBuf))
		{
			sprintf(szBuf, "%s %s", szTimeTag, m_szMsgErr);
			PushDataToQueue(szBuf);
		}
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

bool CTextLog::LogTextF(const char *pFormat, ...)
{
	char _szLog[LOG_LINE_MAX_SIZE];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szLog, pFormat, args);
	va_end(args);
	return LogText(_szLog);
}

bool CTextLog::LogInfo(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	bool bRet = false;
	//----------------------------------------------
	// 2006.07.24
	char szLogBuf[LOG_LINE_MAX_SIZE];
	const char *pLog;
	int nLen;

	// 2011.07.27
	char szBuf[LOG_LINE_MAX_SIZE];
	char szTimeTag[40];
	
	pLog = szLog;
	nLen = strlen(szLog);
	if(nLen > 1)
	{
		if(szLog[nLen-1] == '\n')
		{
			strcpy(szLogBuf, szLog);
			szLogBuf[nLen-1] = 0;
			pLog = szLogBuf;
		}
	}
	//----------------------------------------------
	
	if(! m_bInitFlag) InitConfiguration();
	if(! m_bLogEnable) return false;
	if(! m_bLogIDEnable[nLogID]) return false;
	EnterCriticalSection(&m_csLock);
	do{ 
		CreateTimeTag(szTimeTag);
		sprintf(szBuf, "\nLogID: %d", nLogID);
		m_pFileQueue->Push(szBuf);
		sprintf(szBuf, "Time : %s", szTimeTag);
		m_pFileQueue->Push(szBuf);
		sprintf(szBuf, "File : %s", szSrcFile);
		m_pFileQueue->Push(szBuf);
		sprintf(szBuf, "Line : %d", nSrcLine);
		m_pFileQueue->Push(szBuf);
		sprintf(szBuf, "Log  : %s", pLog); 
		m_pFileQueue->Push(szBuf);
		bRet = true;
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

bool CTextLog::IsLogFileChanged()
{
	bool bRet = false;
	SYSTEMTIME sysTime;

	// 2011.07.27
	int nLastHour;
	int nCurrHour;

	GetLocalTime(&sysTime);

	switch(m_nFileOption)
	{
	case FILE_OPT_CREATE_PER_DAY:
		if(m_nLastLogYear != sysTime.wYear
				|| m_nLastLogMonth != sysTime.wMonth
				|| m_nLastLogDay != sysTime.wDay
				) bRet = true;
		break;
	case FILE_OPT_CREATE_PER_MONTH:
		if(m_nLastLogYear != sysTime.wYear
				|| m_nLastLogMonth != sysTime.wMonth
				) bRet = true;
		break;
	case FILE_OPT_CREATE_PER_HOURS:									// 2011.07.27
		if(m_nLastLogYear != sysTime.wYear
				|| m_nLastLogMonth != sysTime.wMonth
				|| m_nLastLogDay != sysTime.wDay
				) bRet = true;
		else // 2011.09.29
		{
			nLastHour = m_nLastLogHour - (m_nLastLogHour % m_nFileCreatePeriod_Hours);
			nCurrHour = sysTime.wHour - (sysTime.wHour % m_nFileCreatePeriod_Hours);
			if(nLastHour != nCurrHour) bRet = true;
		}
		break;
	}

	// SetLastLogTime(&sysTime);	// 2013.06.03
	return bRet;
}

bool CTextLog::WriteLogToFile(const char *szText)
{
	// 2011.07.27
	char szBuf[LOG_LINE_MAX_SIZE];
	char szTimeTag[40];

	bool bRet = false;
	do {
		CreateTimeTag(szTimeTag);
		sprintf(szBuf, "%s%s%s", szTimeTag, m_szDelimiter, szText);
		if(false == m_pFileQueue->Push(szBuf)) break;
		bRet = true;
	} while(0);
	return bRet;
}

void CTextLog::GetLogFileName(char* szFilePath, SYSTEMTIME *pSysTime)
{
	char szBuf[FILE_PATH_SIZE];							// 2011.07.27
	int nHours;											// 2011.07.27

	switch(m_nFileOption)
	{
	case 0:
	case FILE_OPT_CREATE:
	case FILE_OPT_CHECK:
		strcpy(szFilePath, m_szLogPath);
		break;
	case FILE_OPT_CREATE_PER_DAY:
		sprintf(szBuf, "%s_%d_%02d_%02d.%s",
			m_szFile, pSysTime->wYear, pSysTime->wMonth, pSysTime->wDay, m_szExt);
		strcpy(szFilePath, szBuf);
		break;
	case FILE_OPT_CREATE_PER_MONTH:
		sprintf(szBuf, "%s_%d_%02d.%s", m_szFile, pSysTime->wYear, pSysTime->wMonth, m_szExt);
		strcpy(szFilePath, szBuf);
		break;
	case FILE_OPT_CREATE_PER_HOURS:						// 2011.07.27
		nHours = pSysTime->wHour - (pSysTime->wHour % m_nFileCreatePeriod_Hours);
		GetLogFileName_Hour(szFilePath, pSysTime, nHours);
		break;
	}
}

// 2011.07.27
void CTextLog::GetLogFileName_Hour(char* szFilePath, SYSTEMTIME *pSysTime, int nHour)
{
	char szBuf[FILE_PATH_SIZE];

	// 2013.06.13
	if(LOG_TYPE2 == m_nLogType)
	{
		sprintf(szBuf, "%s%d%02d%02d%02d.%s",
			m_szFile, pSysTime->wYear, pSysTime->wMonth, pSysTime->wDay, nHour, m_szExt);
		strcpy(szFilePath, szBuf);
	}
	else // LOG_TYPE1
	{
		sprintf(szBuf, "%s_%d_%02d_%02d_%02d.%s",
			m_szFile, pSysTime->wYear, pSysTime->wMonth, pSysTime->wDay, nHour, m_szExt);
		strcpy(szFilePath, szBuf);
	}
}

void CTextLog::SetLastLogTime(SYSTEMTIME *pSysTime)
{
	m_nLastLogDay = pSysTime->wDay;
	m_nLastLogMonth = pSysTime->wMonth;
	m_nLastLogYear = pSysTime->wYear;
	m_nLastLogHour = pSysTime->wHour;						// 2011.07.27
}

bool CTextLog::ParseFileName(char *szFileFullName, char *szFile, char *szExt)
{
	bool bRet = false;
	do {
		int nSize = strlen(szFileFullName);
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
			if('.' == szFileFullName[nIdx]) break;
		}
		if('.' == szFileFullName[nIdx])
		{
			strncpy(szFile, szFileFullName, nIdx);
			szFile[nIdx] = 0;
			strcpy(szExt, szFileFullName + nIdx + 1);
		}
		else strcpy(m_szFile, szFileFullName);
		bRet = true;
	} while(0);
	return bRet;
}

bool CTextLog::ParseFileFullName()
{
	int nSize = strlen(m_szLogPath);
	if(nSize <= 0)
	{
		strcpy(m_szLogFile, "InvalidLogFileName");
		return false;
	}
	int i;
	for(i=0; i<FILE_NAME_MAX; i++) m_szLogFile[i] = 0;
	for(i=nSize-1; i>=0; i--)
	{
		if( m_szLogPath[i] == '\\' ) break;
	}
	i++;
	strcpy(m_szLogFile, m_szLogPath+i);
	if(strlen(m_szLogFile) <= 0)
	{
		strcpy(m_szLogFile, "InvalidLogFileName");
		return false;
	}
	return true;
}

bool CTextLog::CreateTimeTag(char *szTimeTag)
{
	bool bHourChanged = false;				// 2011.09.30
	char szYear[8] = "";					// 2013.06.03
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	// 2013.06.13
	if(LOG_TYPE2 == m_nLogType) sprintf(szYear, "%04d/", sysTime.wYear);

	sprintf(szTimeTag, "%s%02d/%02d%s%02d:%02d:%02d.%03d",
		szYear,								// 2013.06.03
		sysTime.wMonth, sysTime.wDay,		// 2006.08.30
		m_szDelimiter,						// 2013.06.03
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

	// 2011.09.30
	if(m_nLastHour != sysTime.wHour)
	{
		bHourChanged = true;
		m_nLastHour = sysTime.wHour;
	}

	SetLastLogTime(&sysTime);				// 2013.06.03
	return bHourChanged;
}

bool CTextLog::CreateTimeTag(char *szTimeTag, SYSTEMTIME *pSysTime)
{
	bool bHourChanged = false;				// 2011.09.30
	char szYear[8] = "";					// 2013.06.03
	GetLocalTime(pSysTime);

	// 2013.06.13
	if(LOG_TYPE2 == m_nLogType) sprintf(szYear, "%04d/", pSysTime->wYear);
	
	sprintf(szTimeTag, "%s%02d/%02d%s%02d:%02d:%02d.%03d",
		szYear,								// 2013.06.03
		pSysTime->wMonth, pSysTime->wDay,	// 2006.08.30
		m_szDelimiter,						// 2013.06.03
		pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond, pSysTime->wMilliseconds);

	// 2011.09.30
	if(m_nLastHour != pSysTime->wHour)
	{
		bHourChanged = true;
		m_nLastHour = pSysTime->wHour;
	}

	SetLastLogTime(pSysTime);				// 2013.06.03
	return bHourChanged;
}

//--------------------------------------------------------------------
// 2005.09.03, for Log Message Send
void CTextLog::ThrdMsgSender(int nThreadNo)
{
	char szRcvMsg[LOG_LINE_MAX_SIZE];
	char szReply[LOG_LINE_MAX_SIZE];

	while(WAIT_SEC(1))
	{
		if(m_nInstanceID < 0) { WAIT_SEC(3); continue; }
		else if(m_nClientStatus == INTF_DISCONNECTED)
		{
			m_nClientStatus = GetClientStatus();
			if(m_nClientStatus != INTF_DISCONNECTED)
			{
				sprintf(szReply, "#Client_Name = %s", m_szClientName);
				SendMsg(szReply);

				m_bMsgDelay = false;
				LogText("Global Message Interface is connected !");
				m_pMsgTimer->StartTimer();
			}
			WAIT_SEC(3);
		}
		else if(m_bMsgDelay)
		{
			WAIT_SEC(5);
			m_bMsgDelay = false;
		}
		else
		{
			if(m_pMsgTimer->GetProgressTime() > 5000)
			{
				m_nClientStatus = GetClientStatus();
				if(m_nClientStatus == INTF_DISCONNECTED)
				{
					strcpy(m_szMsgErr, "Global Message Interface is disconnected !");
					WriteLogToFile(m_szMsgErr);
				}
				m_pMsgTimer->StartTimer();
			}
			else WAIT_SEC(3);
		}

		if(IS_THREAD_STOPPING()) break;
		if(RecvMsg(szRcvMsg))
		{
			sprintf(szReply, "#Received_Message = %s", szRcvMsg);
			SendMsg(szReply);
			LogTextF("Message_Received From MsgServer : %s", szRcvMsg);
			ParseRcvMsg(szRcvMsg);
		}
	}
}

bool CTextLog::SendLogMsg(const char *szLog, int nLogID)
{
	char szBuf[LOG_LINE_MAX_SIZE];
	char szTimeTag[40];
	if(! m_bMsgIDEnable[nLogID]) return true;

	CreateTimeTag(szTimeTag);
	sprintf(szBuf, "%s %s", szTimeTag, szLog);
	return SendMssge(szBuf, nLogID);
}

bool CTextLog::SendMssge(const char *szLog, int nLogID)
{
	bool bRet = true;
	if(! m_bMsgIDEnable[nLogID]) return true;
	if(m_nInstanceID < 0) return true;
	if(m_bMsgDelay) return true;
	if(m_nClientStatus == INTF_DISCONNECTED) return true;

	char szBuf[LOG_LINE_MAX_SIZE];
	sprintf(szBuf, "%s %3d   %s", m_szLogFile, nLogID, szLog); 

	szBuf[MSG_BUFFER_SIZE-1] = 0;								// 2011.07.11
	if(! SendMsg((char *) szBuf))
	{
		m_nClientStatus = GetClientStatus();
		if(m_nClientStatus == INTF_DISCONNECTED)
		{
			strcpy(m_szMsgErr, "Global Message Interface is disconnected !");
		}
		else if(m_nClientStatus == INTF_OVERFLOW)
			strcpy(m_szMsgErr, "Global Message Interface Buffer is overflow !");
		else if(m_nClientStatus == INTF_TIMEOUT)
			strcpy(m_szMsgErr, "Global Message Interface Access Timeout !");
		else
		{
			sprintf(szBuf, "Global Log Message Sending failed !(%s)", GetLastError());
			strcpy(m_szMsgErr, szBuf);
		}
		m_bMsgDelay = true;
		bRet = false;
	}
	m_pMsgTimer->StartTimer();
	return bRet;
}

void CTextLog::ParseRcvMsg(char *szRcvMsg)
{
	int i, n;
	char ppParam[3][256];										// 2011.07.27
	CTextParser txtP(szRcvMsg);

	txtP.GetWords(ppParam, 3);
	n = atoi(ppParam[2]);
	if(n >= MAX_LOG_ID || n < 0) n = MAX_LOG_ID-1;
	if(txtP.BeEqual(ppParam[0], "msg"))
	{
		if(txtP.BeEqual(ppParam[1], "on"))
		{
			if(txtP.BeEqual(ppParam[2], "all") || strlen(ppParam[2]) == 0)
					for(i=0; i<MAX_LOG_ID; i++) m_bMsgIDEnable[i] = true;
			else m_bMsgIDEnable[n] = true;
		}
		else if(txtP.BeEqual(ppParam[1], "off"))
		{
			if(txtP.BeEqual(ppParam[2], "all") || strlen(ppParam[2]) == 0)
					for(i=0; i<MAX_LOG_ID; i++) m_bMsgIDEnable[i] = false;
			else m_bMsgIDEnable[n] = false;
		}
	}
}

// 2013.06.03
void CTextLog::ThrdTimeMonitor(int nThreadNo)
{
	CTimer tmAutoCreateFile;

	tmAutoCreateFile.StartTimer();
	while(WAIT_SEC(10))
	{
		// 2013.06.13
		if(LOG_TYPE2 == m_nLogType && tmAutoCreateFile.GetProgressTime(UNIT_MINUTE) > 45)
		{
			if(IsLogFileChanged()) LogText(" ");
			tmAutoCreateFile.StartTimer();
		}
	}
}

// 2014.08.03
void CTextLog::ThrdZipMonitor(int nThreadNo)
{
	CTimer tmZipCreateFile;
	
	char szCurrFilePath[FILE_PATH_SIZE] = "";
	char szOldFilePath[FILE_PATH_SIZE] = "";	
	char szZipPath[1024] = "";
	
	WORD Old_Hour, Curr_Hour;
	SYSTEMTIME SysTime;

	WAIT_SEC(60);

    if( LOG_TYPE2 == m_nLogType )
	{
		printf("TextDll::MARS Zip Thread Start...\n");
		
		GetLocalTime(&SysTime);	
		GetLogFileName(szOldFilePath, &SysTime);	
		Old_Hour = SysTime.wHour;
				
		while(WAIT_SEC(10))
		{
			GetLocalTime(&SysTime);	
			GetLogFileName(szCurrFilePath, &SysTime);
			Curr_Hour = SysTime.wHour;
			
			if( Old_Hour != Curr_Hour )
			{				
				if(strlen(szOldFilePath) > 3)
				{
					sprintf(szZipPath, "7z a %s.zip %s", szOldFilePath, szOldFilePath);
					WinExec(szZipPath, SW_HIDE);
					WAIT_SEC(30);
					remove(szOldFilePath);
					Old_Hour = Curr_Hour;
					strcpy(szOldFilePath, szCurrFilePath);
				}
				else
				{
					strcpy(szOldFilePath, szCurrFilePath);
					printf("TextDll::Invalid Zip File Path...\n");
				}
			}
		}
	}
}
