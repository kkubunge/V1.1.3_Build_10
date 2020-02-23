// TextLogDll.h: interface for the 'TextLog.DLL'
// Date : 2004.04.06
// Modi : 2004.10.07
// Programed by Chang-su,Kim.
//////////////////////////////////////////////////////////////////////

#ifndef _TEXTLOGDLL_H_
#define _TEXTLOGDLL_H_

//////////////////////////////////////////////////////////////////////
// Log Type 2-2
#define LOG_FILE_PATH			"SeqLog\\Mks651Control.log"
#define CFG_FILE_PATH			"SeqLog\\SeqLog.cfg"
static void gLogType2(const char *szLog, int nLogID);
#define _LOG_V_LIST(P_FORMAT, LOG_ID)					\
{														\
	char szLog[256];									\
	va_list args;										\
	va_start(args, pFormat);							\
	vsprintf(szLog, pFormat, args);						\
	va_end(args);										\
	gLogType2(szLog, LOG_ID);							\
}
static void _LOG(const char *pFormat, ...)	{ _LOG_V_LIST(pFormat, 0) }		// Recipe Data

//////////////////////////////////////////////////////////////////////
// #include <window.h>

typedef DWORD (*LP_INIT_LOG_DLLFUNC)(const char* szLogFile);
typedef DWORD (*LP_CLOSE_LOG_DLLFUNC)();
typedef DWORD (*LP_INIT_CFG_DLLFUNC)(const char* szCfgFile);
typedef DWORD (*LP_LOG_TEXT_DLLFUNC)(const char* szLog, int nLogID);
typedef DWORD (*LP_LOG_INFO_DLLFUNC)(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID);
static LP_INIT_LOG_DLLFUNC _gInitLog;
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
			_gInitLog(szLogFile);
			if(szCfgFile != NULL)
			{
				if(*szCfgFile != 0) _gInitLogCfg(szCfgFile);
			}
		}
		else
		{
		//	if(! bRet) MessageBox(NULL, "'TextLog.dll' Loading failed !",
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
