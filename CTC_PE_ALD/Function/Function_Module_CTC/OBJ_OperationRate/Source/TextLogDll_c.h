// TextLogDll_c.h: interface for the 'TextLog.DLL'
// Create : 2004.04.06
// Last Update : 2005.10.20
// Programed by Chang-su,Kim.
//////////////////////////////////////////////////////////////////////

#ifndef _TEXTLOGDLL_H_
#define _TEXTLOGDLL_H_

// Log File Name
#define LOG_FILE_PATH			""

// Configration File Path
#define CFG_FILE_PATH			""

// Init Function
#define _gInitLogDll_c() _gInitLogDll(LOG_FILE_PATH, CFG_FILE_PATH, FALSE);
static BOOL _gInitLogDll(const char *szLogFile /*= LOG_FILE_PATH*/,
						 const char *szCfgFile /*= CFG_FILE_PATH*/,
						 BOOL bConfigLog /*= TRUE*/);	// Configration Information Log Enable/Disable
static void _gFreeLogDll();
static BOOL g_bLogDllLoaded = FALSE;

//////////////////////////////////////////////////////////////////////
// Log Type 1
// - Line Log
//--------------------------------------------------------------------
/*
#define _LOG(SZ_LOG)				_gLogType1(SZ_LOG);
*/

//////////////////////////////////////////////////////////////////////
// Log Type 1-2
// - Line Log and Using Format
//--------------------------------------------------------------------
/*
static void _gLogType1(const char *szLog);
static void _LOG(const char *pFormat, ...)
{
	char szLog[256];
	va_list args;
	va_start(args, pFormat);
	vsprintf(szLog, pFormat, args);
	va_end(args);
	_gLogType1(szLog);
}
*/

//////////////////////////////////////////////////////////////////////
// Log Type 2
// - Line Log
// - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
//--------------------------------------------------------------------
/*
#define _LOG(SZ_LOG)				_gLogType2(SZ_LOG, 0)
#define _LOG_1(SZ_LOG)				_gLogType2(SZ_LOG, 1)
*/

//////////////////////////////////////////////////////////////////////
// Log Type 2-2
// - Line Log and Using Format
// - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
// - LogID : 0~19, 0:Default Enabled, 1~19:Default Disabled
//--------------------------------------------------------------------
static void _gLogType2(const char *szLog, int nLogID);
#define _LOG_V_LIST(P_FORMAT, LOG_ID)					\
{														\
	char _szLog[256];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szLog, P_FORMAT, args);					\
	va_end(args);										\
	_gLogType2(_szLog, LOG_ID);							\
}
//--------------------------------------------------------------------
// _LOG Function Definition (LogID : 0 ~ 19)
extern void _LOG(const char *pFormat, ...)		{ _LOG_V_LIST(pFormat, 0) }		// LogID 0:Default Enable
/*
extern void _LOG_RCP(const char *pFormat, ...)	{ _LOG_V_LIST(pFormat, 1) }		// Recipe Data
extern void _LOG_PROC(const char *pFormat, ...)	{ _LOG_V_LIST(pFormat, 2) }		// Process Progress
extern void _LOG_RMAP(const char *pFormat, ...)	{ _LOG_V_LIST(pFormat, 3) }		// Recipe Map Data
extern void _LOG_RCP_T(const char *pFormat,...)	{ _LOG_V_LIST(pFormat, 4) }		// Recipe Data from Kornic Engine
extern void _LOG_ERR(const char *pFormat, ...)	{ _LOG_V_LIST(pFormat, 5) }		// Error Logging
*/
//.......
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Log Type 3
// - Source Code Information Log
//--------------------------------------------------------------------
/*
#define _LOG(SZ_LOG)									\
{														\
	const char* __szLog = SZ_LOG;						\
	const char* __file = __FILE__;						\
	int __line = __LINE__;								\
	_gLogType3(__szLog, __file, __line);					\
}
*/

//////////////////////////////////////////////////////////////////////
// Log Type 4
// - Source Code Information Log
// - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
//--------------------------------------------------------------------
/*
#define _LOG(SZ_LOG, INT_LOG_ID)						\
{														\
	const char* __szLog = SZ_LOG;						\
	const char* __file = __FILE__;						\
	int __line = __LINE__;								\
	_gLogType4(__szLog, __file, __line, INT_LOG_ID);		\
}
#define _LOG_1(SZ_LOG)			{ _LOG(SZ_LOG, 1) }
*/

//////////////////////////////////////////////////////////////////////
// #include <window.h>

typedef DWORD (*LP_INIT_LOG_DLLFUNC)(const char* szLogFile);
typedef DWORD (*LP_CLOSE_LOG_DLLFUNC)();
typedef DWORD (*LP_INIT_CFG_DLLFUNC)(const char* szCfgFile);
typedef DWORD (*LP_LOG_TEXT_DLLFUNC)(const char* szLog, int nLogID);
typedef DWORD (*LP_LOG_INFO_DLLFUNC)(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID);
typedef DWORD (*LP_gTextLogDll_Command)(const char *szCmd, char *szReply /*= NULL*/);

static LP_INIT_LOG_DLLFUNC _gInitLog = NULL;
static LP_INIT_LOG_DLLFUNC _gInitLog2 = NULL;
static LP_CLOSE_LOG_DLLFUNC _gCloseLog = NULL;
static LP_INIT_CFG_DLLFUNC _gInitLogCfg = NULL;
static LP_LOG_TEXT_DLLFUNC _gLogText = NULL;
static LP_LOG_INFO_DLLFUNC _gLogInfo = NULL;
static LP_gTextLogDll_Command _gTextLogDll_Command = NULL;

static HMODULE g_hLogDllModule = NULL;
static void _gLoadLogDll()
{
	if(NULL == g_hLogDllModule)
	{
		g_hLogDllModule = LoadLibrary("TextLog.dll");
		if(NULL != g_hLogDllModule)
		{
			_gInitLog = (LP_INIT_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLog");
			_gInitLog2 = (LP_INIT_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLog2");
			_gCloseLog = (LP_CLOSE_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gCloseLog");
			_gInitLogCfg = (LP_INIT_CFG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLogCfg");
			_gLogText = (LP_LOG_TEXT_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gLogText");
			_gLogInfo = (LP_LOG_INFO_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gLogInfo");
			_gTextLogDll_Command = (LP_gTextLogDll_Command) GetProcAddress(g_hLogDllModule, "_gTextLogDll_Command");
			if(_gInitLog == NULL || _gCloseLog == NULL || _gInitLogCfg == NULL
				|| _gLogText == NULL || _gLogInfo == NULL /*|| _gTextLogDll_Command == NULL*/)
			{
				FreeLibrary(g_hLogDllModule);
				g_hLogDllModule = NULL;
			}
		}
	}
}

static BOOL _gInitLogDll(const char *szLogFile, const char *szCfgFile, BOOL bConfigLog)
{
	BOOL bRet = FALSE;
	do {
		_gLoadLogDll();
		if(NULL != g_hLogDllModule)
		{
			if(bConfigLog || _gInitLog2 == NULL) _gInitLog(szLogFile);
			else _gInitLog2(szLogFile);
			if(szCfgFile != NULL && szCfgFile[0] != 0)
			{
				if(*szCfgFile != 0)	 _gInitLogCfg(szCfgFile);
			}
		}
		else
		{
		//	MessageBox(NULL, "'TextLog.dll' Loading failed !",
		//		"TextLogDll.h", MB_OK| MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_TOPMOST);
			break;
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
		_gCloseLog();
		FreeLibrary(g_hLogDllModule);
		g_hLogDllModule = NULL;
	}
}

static void _gLogType1(const char *szLog)
{
	if(g_bLogDllLoaded) _gLogText(szLog, 0);
}

static void _gLogType2(const char *szLog, int nLogID)
{
	if(g_bLogDllLoaded) _gLogText(szLog, nLogID);
}

static void _gLogType3(const char *szLog, const char *szSrcFile, int nSrcLine)
{
	if(g_bLogDllLoaded) _gLogInfo(szLog, szSrcFile, nSrcLine, 0);
}

static void _gLogType4(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(g_bLogDllLoaded) _gLogInfo(szLog, szSrcFile, nSrcLine, nLogID);
}

//////////////////////////////////////////////////////////////////////
#endif
