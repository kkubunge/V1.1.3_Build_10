// TextLog.h: interface for the CTextLog class.
// Version : 2.5.1.1
// Create : 2003.03.
// Last Modi : 2004.11.04
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

// Log File Option
#define FILE_OPT_CREATE					0x10		// Logging File created new always
#define FILE_OPT_CHECK					0x20		// Logging File check and create, if no exist
#define FILE_OPT_CREATE_PER_DAY			0x30
#define FILE_OPT_CREATE_PER_MONTH		0x40

// Log Option
#define LG_OPT_CONFIG_LOG				0x01		// Log 'Logging Start Time and Config' from LogDLL

// Max Log ID
#define MAX_LOG_ID						20

#include "MonThread.h"
//////////////////////////////////////////////////////////////////////
// Class Definition
class CTextLog : public CMonThread
{
public:
	CTextLog(const char* szFilePath, int nOption = 0);
	virtual ~CTextLog();
	// nOption = 'Log File Option' | 'Log Option(1)' | 'Log Option(2)' . . . .

// Member Function
public:
	void InitConfigration(const char* szCfgFile);
	void SetCfgLogID(const char* szCfg);
	void SetCfgKeepDays(const char* szCfg);
	void SetCfgKeepMonths(const char* szCfg);

	bool LogConfig(const char *szLog);
	bool LogText(const char* szLog, int nLogID);
	bool LogText(const char* szLog);
	bool LogTextF(const char *pFormat, ...);
	bool LogInfo(const char* szLog, const char* szSrcFile, int nSrcLine, int nLogID = 0);

private:
	void GetLogFileName(char* szFilePath, SYSTEMTIME *pSysTime);
	bool ParseFileName();
	bool ParseFileFullName();

// Virtual Function
protected:
	virtual void DoMonitor();
	void LastDateByDays(SYSTEMTIME *pSysTime, int nPDays);
	void LastDateByMonths(SYSTEMTIME *pSysTime, int nPMonths);
	void DeleteLogFile(SYSTEMTIME *pSysTime);

// Member Variable
public:

private:
	enum { FILE_NAME_MAX = 256 };
	char m_szLogPath[FILE_NAME_MAX];
	char m_szLogFile[FILE_NAME_MAX];
	char m_szFile[FILE_NAME_MAX];
	char m_szExt[FILE_NAME_MAX];
	int m_nFileOption;
	int m_nConfigLogOption;

	bool m_bLogEnable[MAX_LOG_ID];
	char m_szLogDesc[MAX_LOG_ID][80];

	int m_nKeepDays;
	int m_nKeepMonths;

	int m_nDeleteRangeDays;
	int m_nDeleteRangeMonths;

	CRITICAL_SECTION m_csLock;
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
