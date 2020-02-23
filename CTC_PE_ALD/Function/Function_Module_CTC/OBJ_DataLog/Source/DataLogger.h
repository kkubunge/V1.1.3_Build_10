// DataLogger.h: interface for the CDataLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATALOGGER_H__746AAF35_434A_4811_B0D5_70E48FB7A2AF__INCLUDED_)
#define AFX_DATALOGGER_H__746AAF35_434A_4811_B0D5_70E48FB7A2AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_IO_LIST		100

typedef struct _IO_LIST
{
	char szIOName[40];
	int nIOType;
	int nIOPoint;

} IO_LIST;


// 2006.09.18
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

class CDataLogger  
{
	enum{MSG_LOG_START = WM_USER+100 , MSG_LOG_STOP};

// Member Function
public:
	CDataLogger(char* szName , int nLogPeriod = 1000 , bool bLogOption = false ,int nDel = 30 , char* szFilePath = NULL);
	virtual ~CDataLogger();

	void Initialize      ();
	bool SetLogItem      (char* szIOName , int nIOType);
	int  GetItemCount    ();
	int  GetRunStsIOPoint();

	void StartLogging    ();
	void StopLogging     ();

	bool IsLoggingStartReady();	// 2006.12.17 by ycchoi < 1.0.0.9 >

private:
	void _Log(FILE* pLogFile);

	bool WriteLogStart(FILE* pLogFile);
	void WriteLogEnd  (FILE* pLogFile);

	void DoLogging();
	void DoAutoLogDelete();

	void DoWaitLogSpanTime(DWORD dwStartTick);

	static DWORD WINAPI ThreadProc   (LPVOID pVoid);
	static DWORD WINAPI AutoDelThread(LPVOID pVoid);
	DWORD  m_dwThreadId;
	DWORD  m_dwAutoDelThreadID;

// Member Variable
public:
	bool m_abRcpLogEnable[9];

private:
	bool m_bLogRunning;
	bool m_bLoggingStartReady;	// 2006.12.17 by ycchoi < 1.0.0.9 >

	char m_szName    [256];		// 2006.04.26
	char m_szFilePath[512];		// 2006.04.26
	char m_szRcpType [64];
	char m_szRcpName [64];
	int  m_nIOCount;

	int  m_nDeletePeriod;
	int  m_nLogPeriod;
	bool m_bLogOption;

	int  m_nRusStsIOPoint;
	int  m_nRcpTypeIOPoint;
	int  m_nRcpNameIOPoint;
	int  m_nPMInusedIOPoint; //Log print when process && pm is not in use

	IO_LIST* m_IOList[MAX_IO_LIST];
};

#endif // !defined(AFX_DATALOGGER_H__746AAF35_434A_4811_B0D5_70E48FB7A2AF__INCLUDED_)
