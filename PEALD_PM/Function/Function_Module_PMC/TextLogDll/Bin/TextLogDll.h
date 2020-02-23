// TextLogDll.h: interface for the 'TextLog.DLL'
// Date : 2004.04.06
// Modi : 2005.07.02
// Programed by Chang-su,Kim.
//////////////////////////////////////////////////////////////////////

#ifndef _TEXTLOGDLL_H_
#define _TEXTLOGDLL_H_

// Log File Name
#define LOG_FILE_PATH				"DNet.log"

// Configration File Path
#define CFG_FILE_PATH				"DNet.cfg"

// Configration Log Enable/Disable
const static BOOL g_bConfigLog = TRUE;

//////////////////////////////////////////////////////////////////////
// Log Type 1
// - Line Log
//--------------------------------------------------------------------
/*
#define _LOG(SZ_LOG)				gLogType1(SZ_LOG);
*/

//////////////////////////////////////////////////////////////////////
// Log Type 1-2
// - Line Log and Using Format
//--------------------------------------------------------------------
/*
static void gLogType1(const char *szLog);
static void _LOG(const char *pFormat, ...)
{
	char szLog[256];
	va_list args;
	va_start(args, pFormat);
	vsprintf(szLog, pFormat, args);
	va_end(args);
	gLogType1(szLog);
}
*/

//////////////////////////////////////////////////////////////////////
// Log Type 2
// - Line Log
// - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
//--------------------------------------------------------------------
/*
#define _LOG(SZ_LOG)				gLogType2(SZ_LOG, 0)
#define _LOG_1(SZ_LOG)				gLogType2(SZ_LOG, 1)
*/

//////////////////////////////////////////////////////////////////////
// Log Type 2-2
// - Line Log and Using Format
// - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
// - LogID : 0 ~ 99, 0:Default Enabled, 1~99:Default Disabled
//--------------------------------------------------------------------
static void gLogType2(const char *szLog, int nLogID);
#define _LOG_V_LIST(P_FORMAT, LOG_ID)					\
{														\
	char _szLog[256];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szLog, P_FORMAT, args);					\
	va_end(args);										\
	gLogType2(_szLog, LOG_ID);							\
}
//--------------------------------------------------------------------
// _LOG Function Definition (LogID : 0 ~ 99)
static void _LOG(const char *pFormat, ...)		{ _LOG_V_LIST(pFormat, 0) }		// LogID 0:Default Enable
// static void _LOG_1(const char *pFormat, ...)	{ _LOG_V_LIST(pFormat, 1) }		// LogID 1:Default Disable
// static void _LOG_2(const char *pFormat, ...)	{ _LOG_V_LIST(pFormat, 2) }		// LogID 2:Default Disable
//.......
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
	gLogType3(__szLog, __file, __line);					\
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
	gLogType4(__szLog, __file, __line, INT_LOG_ID);		\
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
static LP_INIT_LOG_DLLFUNC _gInitLog;
static LP_INIT_LOG_DLLFUNC _gInitLog2;
static LP_CLOSE_LOG_DLLFUNC _gCloseLog;
static LP_INIT_CFG_DLLFUNC _gInitLogCfg;
static LP_LOG_TEXT_DLLFUNC _gLogText;
static LP_LOG_INFO_DLLFUNC _gLogInfo;

static HMODULE g_hLogDllModule = NULL;
static BOOL g_bTryInitLogDll = FALSE;

static void gLoadLogDll()
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
			if(_gInitLog == NULL || _gCloseLog == NULL || _gInitLogCfg == NULL
				|| _gLogText == NULL || _gLogInfo == NULL)
			{
				FreeLibrary(g_hLogDllModule);
				g_hLogDllModule = NULL;
			}
		}
	}
}

static BOOL gInitLog(const char *szLogFile, const char *szCfgFile)
{
	BOOL bRet = FALSE;
	do {
		gLoadLogDll();
		if(NULL != g_hLogDllModule)
		{
			if(g_bConfigLog || _gInitLog2 == NULL) _gInitLog(szLogFile);
			else _gInitLog2(szLogFile);
			if(szCfgFile != 0)
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
		bRet = TRUE;
	} while(0);

	g_bTryInitLogDll = TRUE;
	return bRet;
}

static void gFreeLogDll()
{
	if(g_hLogDllModule != NULL)
	{
		FreeLibrary(g_hLogDllModule);
		g_hLogDllModule = NULL;
		g_bTryInitLogDll = FALSE;
	}
}

static void gLogType1(const char *szLog)
{
	if(! g_bTryInitLogDll) gInitLog(LOG_FILE_PATH, NULL);
	if(g_hLogDllModule != NULL) _gLogText(szLog, 0);
}

static void gLogType2(const char *szLog, int nLogID)
{
	if(! g_bTryInitLogDll) gInitLog(LOG_FILE_PATH, CFG_FILE_PATH);
	if(g_hLogDllModule != NULL) _gLogText(szLog, nLogID);
}

static void gLogType3(const char *szLog, const char *szSrcFile, int nSrcLine)
{
	if(! g_bTryInitLogDll) gInitLog(LOG_FILE_PATH, NULL);
	if(g_hLogDllModule != NULL) _gLogInfo(szLog, szSrcFile, nSrcLine, 0);
}

static void gLogType4(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(! g_bTryInitLogDll) gInitLog(LOG_FILE_PATH, CFG_FILE_PATH);
	if(g_hLogDllModule != NULL) _gLogInfo(szLog, szSrcFile, nSrcLine, nLogID);
}

//////////////////////////////////////////////////////////////////////
#endif
