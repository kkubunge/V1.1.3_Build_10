// TextLog.h: interface for the CTextLog class.
// Create : 2003.03.
// Last Update : 2006.08.23
// Writer : Chang-su, Kim
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTLOG_H__7D728897_8125_11D7_B6AD_009027E90DD2__INCLUDED_)
#define AFX_TEXTLOG_H__7D728897_8125_11D7_B6AD_009027E90DD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Constructor Log Option
// nOption = 'Log File Option' | 'Log Option(1)' | 'Log Option(2)' . . . .

// Definition
#define FILE_OPTION_BIT				0xf0
#define START_T_OPTION_BIT			0x01

// Log File Option
#define FILE_OPT_CREATE					0x10		// Logging File created new always
#define FILE_OPT_CHECK					0x20		// Logging File check and create, if no exist
#define FILE_OPT_CREATE_PER_DAY			0x30
#define FILE_OPT_CREATE_PER_MONTH		0x40
#define FILE_OPT_CREATE_PER_HOURS		0x50		// 2011.07.27

// Log Option
#define LG_OPT_CONFIG_LOG				0x01		// Log 'Logging Start Time and Config' from LogDLL

// Max Log ID
#define MAX_LOG_ID						20

// 2011.07.27
//#define LOG_LINE_MAX_SIZE				1024
#define LOG_LINE_MAX_SIZE				4096
#define FILE_PATH_SIZE					256
#define MSG_BUFFER_SIZE					256		

//--------------------------------------------------------------------
#include "MonThread.h"
#include "GblMsgMngr.h"

//////////////////////////////////////////////////////////////////////
// Class Definition

class CTextLog : public CMonThread, protected CGblMsgMngr
{
public:
	CTextLog(const char* szFilePath, int nOption = 0, bool bMsgEnable = false,
				int nBufferSize = 1024, bool bBufferingEnable = true);
	virtual ~CTextLog();
	// nOption = 'Log File Option' | 'Log Option(1)' | 'Log Option(2)' . . . .

// Member Function
public:
	void InitConfiguration(const char* szCfgFile);
	void InitConfiguration();						// 2008.08.25
	void ParseCfgLogID(const char* szCfg);
	void ParseCfgMsgID(const char* szCfg);
	void ParseCfgKeepDays(const char* szCfg);
	void ParseCfgKeepMonths(const char* szCfg);

	bool LogConfig(const char *szLog);
	bool LogText(const char* szLog, int nLogID = 0);
	bool LogTextF(const char *pFormat, ...);
	bool LogInfo(const char* szLog, const char* szSrcFile, int nSrcLine, int nLogID = 0);
	bool IsLogFileChanged();

	// 2011.07.27
	void SetFileCreatePeriodPerHours(int nHours);
	void SetKeepDays(int nKeepDays);

private:
	bool WriteLogToFile(const char *szLog);
	void GetLogFileName(char* szFilePath, SYSTEMTIME *pSysTime);
	void GetLogFileName_Hour(char* szFilePath, SYSTEMTIME *pSysTime, int nHour);
	bool ParseFileName(char *szFileFullName, char *szFile, char *szExt);
	bool ParseFileFullName();
	bool CreateTimeTag(char *szTimeTag);								// 2011.09.30
	bool CreateTimeTag(char *szTimeTag, SYSTEMTIME *pSysTime);			// 2011.09.30
	void ParseRcvMsg(char *szRcvMsg);
	void SetLastLogTime(SYSTEMTIME *pSysTime);						// 2009.04.01

	void ThrdDateMonitor(int nThreadNo);
	void LastDateByDays(SYSTEMTIME *pSysTime, int nPDays);
	void LastDateByMonths(SYSTEMTIME *pSysTime, int nPMonths);
	void DeleteLogFile(SYSTEMTIME *pSysTime);
	void DeleteLogFile_Hours(SYSTEMTIME *pSysTime, int nThreadNo);	// 2011.07.27

	void ThrdFileSaver(int nThreadNo);								// 2006.07.24
	//--------------------------------------------
	// 2006.07.26
	bool SaveQueueData(bool bRecreateLogFile = true);					// 2011.09.30
	bool PushDataToQueue(char *szData, bool bHourChanged =false);		// 2011.09.30
	CRITICAL_SECTION m_csQueueDataSaveLock;
	//--------------------------------------------

	//----------------------------------------------------
	// 2005.09.03, for Log Message Send
	void ThrdMsgSender(int nThreadNo);
	bool SendLogMsg(const char* szLog, int nLogID = 0);
	bool SendMssge(const char *szLog, int nLogID = 0);

	int m_nClientStatus;
	bool m_bMsgDelay;
	char m_szMsgErr[LOG_LINE_MAX_SIZE];
	char m_szClientName[80];
	//----------------------------------------------------

	// 2013.06.03
	void ThrdTimeMonitor(int nThreadNo);

	void ThrdZipMonitor(int nThreadNo);

	// Virtual Function
protected:
	virtual void DoMonitor(int nThreadNo);

// Member Variable
public:
	// 2013.06.13
	enum { LOG_TYPE1, LOG_TYPE2 };
	int m_nLogType;

private:
	enum { FILE_NAME_MAX = 256 };
	char m_szLogPath[FILE_NAME_MAX];
	char m_szLogFile[FILE_NAME_MAX];
	char m_szFile[FILE_NAME_MAX];
	char m_szExt[FILE_NAME_MAX];
	int m_nFileOption;
	int m_nConfigLogOption;

	bool m_bLogIDEnable[MAX_LOG_ID];
	bool m_bMsgIDEnable[MAX_LOG_ID];
	char m_szLogDesc[MAX_LOG_ID][80];

	int m_nKeepDays;
	int m_nKeepMonths;

	int m_nDeleteRangeDays;
	int m_nDeleteRangeMonths;

	CRITICAL_SECTION m_csLock;
	bool m_bLogEnable;

	class CTimer *m_pMsgTimer;
	// 2006.07.24
	class CDataQueue *m_pFileQueue;
	
	bool m_bBufferingEnable;		// 2006.08.23
	bool m_bInitFlag;				// 2008.08.25
	
	// 2009.04.01
	int m_nLastLogDay;
	int m_nLastLogMonth;
	int m_nLastLogYear;
	int m_nLastLogHour;				// 2011.07.27

	// 2011.07.27
	int m_nFileCreatePeriod_Hours;

	// 2011.09.30
	char m_szLastLogFileName[FILE_PATH_SIZE];
	int m_nLastHour;

	// 2013.06.13
	char m_szDelimiter[8];
};

#define LOG_INFO(C_TEXT_LOG, LOG_STRING, LOG_ID)			\
{															\
	const char* __szLog = LOG_STRING;						\
	const char* __file = __FILE__;							\
	int __line = __LINE__;									\
	C_TEXT_LOG.LogInfo(__szLog, __file, __line, LOG_ID);	\
}

#define LOG_INFO_(C_TEXT_LOG, LOG_STRING)					\
{															\
	const char* __szLog = LOG_STRING;						\
	const char* __file = __FILE__;							\
	int __line = __LINE__;									\
	C_TEXT_LOG.LogInfo(__szLog, __file, __line);			\
}

#endif // !defined(AFX_TEXTLOG_H__7D728897_8125_11D7_B6AD_009027E90DD2__INCLUDED_)
