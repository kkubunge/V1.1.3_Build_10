// TextLogDll_M2.h: interface for the 'TextLog.DLL'
//   - 'Single Object to Multi Log' Supported
//   - TextLog.Dll Version 2.6.1.5 or Upper
// Date : 2005.07.04
// Last Update : 2013.06.13
// Programed by Chang-su,Kim.
//////////////////////////////////////////////////////////////////////

#ifndef _TEXTLOGDLL_H_
#define _TEXTLOGDLL_H_

//--------------------------------------------------------------------
// 2013.06.13
enum {
	LOG_TYPE1,		// Default
	LOT_TYPE2		// for SEC MARS System
};

typedef struct LOG_FILE_LIST
{
	char *szLogPath;
	int nInstID;
	int nKeepingDays;			// 2013.05.27
	int nLogType;				// 2013.06.13
} _LOG_FILE_LIST;

// Log File Name List
#define MAX_LOG_FILE	10
static _LOG_FILE_LIST g_LogList[MAX_LOG_FILE] =
{
	{ "_Log\\MCC\\MCC_Ex.log",			-1, 10, LOG_TYPE1 },			// All Log
	{ "_Log\\EventLog\\EventLog.log",	-1, 10, LOT_TYPE2 },			// Run Log
	""
};

//--------------------------------------------------------------------
// Configration File Path
#define CFG_FILE_PATH	""

//--------------------------------------------------------------------
// Initialization
#define	_gINIT_LOG()	_gInitLogDll(CFG_FILE_PATH, FALSE)

//--------------------------------------------------------------------
// Deinitialization
#define _gCLOSE_LOG()	_gFreeLogDll()

//--------------------------------------------------------------------
static BOOL _gIS_LOG_FILE_CHANGED(int nInstID);									// 2009.04.01

//--------------------------------------------------------------------
// Initialization for Multi Object Multi File Log in C++ Code
// : If use in C Code, Above sentence is made to comment
/*
static BOOL _gInitLogDll(const char *szCfgFile = CFG_FILE_PATH,
						 BOOL bConfigLog = TRUE);
*/

//--------------------------------------------------------------------
// Low Function Definition
static void _gLogType1(int nFileIdx, const char *szLog);
static void _gLogType2(int nFileIdx, const char *szLog, int nLogID);
static void _gLogType3(int nFileIdx, const char *szLog, const char *szSrcFile, int nSrcLine);
static void _gLogType4(int nFileIdx, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID);

//////////////////////////////////////////////////////////////////////
// Log Type 1
//   - Line Log
//   - FILE_LIST_IDX : 0 ~ 9
//--------------------------------------------------------------------
/*
#define _LOG(FILE_LIST_IDX, SZ_LOG)		_gLogType1(g_LogList[FILE_LIST_IDX].nInstID, SZ_LOG);
*/

//////////////////////////////////////////////////////////////////////
// Log Type 1-2
//   - Line Log and Using Format
//   - FILE_LIST_IDX : 0 ~ 9
//--------------------------------------------------------------------
/*
static void _LOG(FILE_LIST_IDX, const char *pFormat, ...)
{
	char szLog[256];
	va_list args;
	va_start(args, pFormat);
	vsprintf(szLog, pFormat, args);
	va_end(args);
	_gLogType1(FILE_LIST_IDX, szLog);
}
*/

//////////////////////////////////////////////////////////////////////
// Log Type 2
//   - Line Log
//   - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
//   - FILE_LIST_IDX : 0 ~ 9
//--------------------------------------------------------------------
/*
#define _LOG(SZ_LOG)				_gLogType2(SZ_LOG, 0)
#define _LOG_1(SZ_LOG)				_gLogType2(SZ_LOG, 1)
*/

//////////////////////////////////////////////////////////////////////
// Log Type 2-2
//   - Line Log and Using Format
//   - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
//   - LogID : 0 ~ 19, 0:Default Enabled, 1 ~ 19:Default Disabled
//   - FILE_LIST_IDX : 0 ~ 9
//--------------------------------------------------------------------
#define _LOG_V_LIST(FILE_LIST_IDX, P_FORMAT, LOG_ID)			\
{														\
	char _szLog[256];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szLog, P_FORMAT, args);					\
	va_end(args);										\
	_gLogType2(FILE_LIST_IDX, _szLog, LOG_ID);					\
}

//--------------------------------------------------------------------
// _LOG Function Definition (LogID : 0 ~ 19)
extern void _LOG(const char *pFormat, ...)		{ _LOG_V_LIST(0, pFormat, 0) }
static void _RUN_LOG(const char *pFormat, ...)	{ _LOG_V_LIST(1, pFormat, 0) }
extern void __LOG(const char *szLog)			{ _gLogType2(0, szLog, 0); }		// 2013.06.10
//.......
//.......
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Log Type 3
//   - Source Code Information Log
//   - FILE_LIST_IDX : 0 ~ 9
//--------------------------------------------------------------------
/*
#define _LOG(FILE_LIST_IDX, SZ_LOG)							\
{														\
	const char* __szLog = SZ_LOG;						\
	const char* __file = __FILE__;						\
	int __line = __LINE__;								\
	_gLogType3(FILE_LIST_IDX, __szLog, __file, __line);		\
}
*/

//////////////////////////////////////////////////////////////////////
// Log Type 4
//   - Source Code Information Log
//   - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
//   - FILE_LIST_IDX : 0 ~ 9
//--------------------------------------------------------------------
/*
#define _LOG(FILE_LIST_IDX, SZ_LOG, INT_LOG_ID)				\
{														\
	const char* __szLog = SZ_LOG;						\
	const char* __file = __FILE__;						\
	int __line = __LINE__;								\
	_gLogType4(FILE_LIST_IDX, __szLog, __file, __line, INT_LOG_ID); \
}
#define _LOG_1(SZ_LOG)			{ _LOG(SZ_LOG, 1) }
*/

//////////////////////////////////////////////////////////////////////
// #include <window.h>

// 2009.04.01
typedef BOOL (*LP_IS_LOG_FILE_CHANGED)(int nInstID);
static LP_IS_LOG_FILE_CHANGED _gIsLogFileChanged;

typedef DWORD (*LP_INIT_LOG_DLLFUNC)(int nInstID, const char* szLogFile);
typedef DWORD (*LP_CLOSE_LOG_DLLFUNC)(int nInstID);
typedef DWORD (*LP_INIT_CFG_DLLFUNC)(int nInstID, const char* szCfgFile);
typedef DWORD (*LP_LOG_TEXT_DLLFUNC)(int nInstID, const char* szLog, int nLogID);
typedef DWORD (*LP_LOG_INFO_DLLFUNC)(int nInstID, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID);
typedef DWORD (*LP_gTextLogDll_Command)(const char *szCmd, char *szReply = NULL);

// 2006.03.13
typedef int  (*LP_gGetLogInstance)();
typedef void (*LP_gReleaseLogInst)(int nInstID);
typedef void (*LP_gReleaseAllLogInst)();

static LP_INIT_LOG_DLLFUNC _gInitLogM;
static LP_INIT_LOG_DLLFUNC _gInitLogM2;
static LP_CLOSE_LOG_DLLFUNC _gCloseLogM;
static LP_INIT_CFG_DLLFUNC _gInitLogCfgM;
static LP_LOG_TEXT_DLLFUNC _gLogTextM;
static LP_LOG_INFO_DLLFUNC _gLogInfoM;
static LP_gTextLogDll_Command _gTextLogDll_Command;

// 2006.03.13
static LP_gGetLogInstance _gGetLogInstance;
static LP_gReleaseLogInst _gReleaseLogInst;
static LP_gReleaseAllLogInst _gReleaseAllLogInst;

static BOOL g_bLogDllLoaded = FALSE;
static HMODULE g_hLogDllModule = NULL;
static BOOL g_bTryToInit = FALSE;


static void _gLoadLogDll()
{
	if(NULL == g_hLogDllModule)
	{
		g_hLogDllModule = LoadLibrary("TextLog_MARS.dll");
		if(NULL != g_hLogDllModule)
		{
			_gInitLogM = (LP_INIT_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLogM");
			_gInitLogM2 = (LP_INIT_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLogM2");
			_gCloseLogM = (LP_CLOSE_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gCloseLogM");
			_gInitLogCfgM = (LP_INIT_CFG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLogCfgM");
			_gLogTextM = (LP_LOG_TEXT_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gLogTextM");
			_gLogInfoM = (LP_LOG_INFO_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gLogInfoM");
			_gTextLogDll_Command = (LP_gTextLogDll_Command) GetProcAddress(g_hLogDllModule, "_gTextLogDll_Command");

			//---------------------------------------------------------------------
			// 2006.03.13
			_gGetLogInstance = (LP_gGetLogInstance) GetProcAddress(g_hLogDllModule, "_gGetLogInstance");
			_gReleaseLogInst = (LP_gReleaseLogInst) GetProcAddress(g_hLogDllModule, "_gReleaseLogInst");
			_gReleaseAllLogInst = (LP_gReleaseAllLogInst) GetProcAddress(g_hLogDllModule, "_gReleaseAllLogInst");
			//---------------------------------------------------------------------

			// 2009.04.01
			_gIsLogFileChanged = (LP_IS_LOG_FILE_CHANGED) GetProcAddress(g_hLogDllModule, "_gIsLogFileChanged");

			if(_gInitLogM == NULL || _gCloseLogM == NULL || _gInitLogCfgM == NULL
				|| _gLogTextM == NULL || _gLogInfoM == NULL || _gTextLogDll_Command == NULL
				|| _gGetLogInstance == NULL || _gReleaseLogInst == NULL || _gReleaseAllLogInst == NULL)
			{
				FreeLibrary(g_hLogDllModule);
				g_hLogDllModule = NULL;
			}
		}
	}
}

static void _gInitLogFile(int nFileIdx, const char *szCfgFile, BOOL bConfigLog)
{
	char szTemp[80];
	g_LogList[nFileIdx].nInstID = _gGetLogInstance();		// 2006.03.13
	if(g_LogList[nFileIdx].nInstID >= 0)
	{
		if(bConfigLog || _gInitLogM2 == NULL) _gInitLogM(g_LogList[nFileIdx].nInstID, g_LogList[nFileIdx].szLogPath);
		else _gInitLogM2(g_LogList[nFileIdx].nInstID, g_LogList[nFileIdx].szLogPath);
		if(szCfgFile != 0)
		{
			if(*szCfgFile != 0)	 _gInitLogCfgM(g_LogList[nFileIdx].nInstID, szCfgFile);
		}

		// 2012.03.14
		// 2013.05.27
		sprintf(szTemp, "SET_KEEP_DAYS %d %d", g_LogList[nFileIdx].nInstID, g_LogList[nFileIdx].nKeepingDays);
		 _gTextLogDll_Command(szTemp, NULL);
		sprintf(szTemp, "SET_FILE_CR_P_HOURS %d 1", g_LogList[nFileIdx].nInstID);
		_gTextLogDll_Command(szTemp, NULL);
		// 2013.06.13
		sprintf(szTemp, "SET_LOG_TYPE %d %d", g_LogList[nFileIdx].nInstID,  g_LogList[nFileIdx].nLogType);
		_gTextLogDll_Command(szTemp, NULL);
	}
}

static BOOL _gInitLogDll(const char *szCfgFile, BOOL bConfigLog)
{
	BOOL bRet = FALSE;

	g_bTryToInit = TRUE;
	do {
		_gLoadLogDll();
		if(NULL == g_hLogDllModule)
		{
		//	MessageBox(NULL, "'TextLog.dll' Loading failed !",
		//		"TextLogDll.h", MB_OK| MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_TOPMOST);
			break;
		}

		// _gTextLogDll_Command("MSG_ENABLE", NULL);			// 2006.03.07
		_gTextLogDll_Command("BUFFER_SIZE 240", NULL);			// 2006.07.25
		_gTextLogDll_Command("BUFFERING_ENABLE YES", NULL);		// 2006.08.23


		for(int i=0; g_LogList[i].szLogPath[0] != 0 && i < MAX_LOG_FILE; i++)
		{
			_gInitLogFile(i, szCfgFile, bConfigLog);
		}
		g_bLogDllLoaded = TRUE;
		bRet = TRUE;
	} while(0);
	return bRet;
}

static void _gFreeLogDll()
{
	if(g_hLogDllModule != NULL)
	{
		g_bLogDllLoaded = FALSE;
		for(int i=0; g_LogList[i].szLogPath[0] != 0 && i < MAX_LOG_FILE; i++)
		{
			if(g_LogList[i].nInstID >= 0)
			{
				_gReleaseLogInst(g_LogList[i].nInstID);		// 2006.03.13
				g_LogList[i].nInstID = -1;
			}
		}
		FreeLibrary(g_hLogDllModule);
		g_hLogDllModule = NULL;
	}
}

static void _gLogType1(int nFileIdx, const char *szLog)
{
	if(! g_bLogDllLoaded) return;
	if(nFileIdx >= 0) _gLogTextM(g_LogList[nFileIdx].nInstID, szLog, 0);
}

static void _gLogType2(int nFileIdx, const char *szLog, int nLogID)
{
	if(! g_bLogDllLoaded) return;
	if(nFileIdx >= 0) _gLogTextM(g_LogList[nFileIdx].nInstID, szLog, nLogID);
}

static void _gLogType3(int nFileIdx, const char *szLog, const char *szSrcFile, int nSrcLine)
{
	if(! g_bLogDllLoaded) return;
	if(nFileIdx >= 0) _gLogInfoM(g_LogList[nFileIdx].nInstID, szLog, szSrcFile, nSrcLine, 0);
}

static void _gLogType4(int nFileIdx, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(! g_bLogDllLoaded) return;
	if(nFileIdx >= 0) _gLogInfoM(g_LogList[nFileIdx].nInstID, szLog, szSrcFile, nSrcLine, nLogID);
}

// 2009.04.01
static BOOL _gIS_LOG_FILE_CHANGED(int nFileIdx)
{
	BOOL bRet = FALSE;
	if(! g_bLogDllLoaded) return FALSE;
	if(_gIsLogFileChanged != NULL) bRet = _gIsLogFileChanged(g_LogList[nFileIdx].nInstID);
	return bRet;
}

#endif
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/* Update History
2005.07.04	Created for Single Object Multi File Log in C++ Code.
2005.10.12	'TextLogDll.h'에 Init 과 Deinit Function인 '_gInitLogDll'
			과 '_gFreeLogDll' Function 추가함. Log를 사용하기 위해서는
			초기에 '_gInitLogDll' Function을 호출해 주어야 함.
2005.12.05	Update History 기록 시작
2006.03.07	TextLog.Dll Version 2.6.1.4 이상에서 Log Message 기능을 Enable
			하기 위한 Command Line 추가
			=> _gTextLogDll_Command("MSG_ENABLE", NULL);
2006.03.13	Multi Log 기능 관련 Instance 관리 Routine 추가
			TextLog.Dll Version 2.6.1.5 이상에서 사용 가능
2006.03.17	Low Function Definition 추가
			기능상 또는 Sequence 상 변경점은 없음.
2006.07.25	TextLog.Dll Version 2.6.2.1 이상에서 Buffer Size 조정 기능 추가
2009.04.01	TextLog.Dll Version 2.6.2.9 이상에서 Log File 변경 시점을 Check 하는 Function 추가
				=> BOOL _gIS_LOG_FILE_CHANGED(...);
				=> return 값이 TRUE 이면 날짜가 넘어감에 따라 File이 변경됨.
2012.03.14	KeepDays를 Func Call로 지정할 수 있는 기능 추가
2013.05.27	초기화시 KeepingDays 를 지정하도록 항목 추가
			Multi에서 Log File 생성 주기를 시간단위로 설정하는 Description 추가
2013.06.03	날짜영역 과 Log 영역 구분자를 TAB 문자 사용할 수 있는 설정 추가
2013.06.10	_LOG Function 효율화를 위한 Func 추가
2013.06.13	LogType 설정추가 (SEC MARS System 대응)
			LOG_TYPE2로 설정시 구분자가 TAB으로 바뀌고 Log 연도 표시기능 추가됨
--------------------------------------------------------------------*/
