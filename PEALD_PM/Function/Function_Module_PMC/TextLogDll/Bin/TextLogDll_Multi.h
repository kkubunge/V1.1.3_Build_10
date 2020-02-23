// TextLogDll_Multi.h: interface for the 'TextLog.DLL'
// - 'TextLog.DLL' Ver 2.5.6.0 or upper version needed
// - Muli Log Files Supported
// Date : 2004.04.06
// Last Update : 2007.07.02
// Programed by Chang-su,Kim.
//////////////////////////////////////////////////////////////////////

#ifndef _TEXTLOGDLL_H_
#define _TEXTLOGDLL_H_

// Log File Name List for FileID(0 ~ 99)
const static char *g_szLogPath[100] =
{
	"A_TestLog.log",		// FileID = 0
	"B_TestLog.log",		// FileID = 1
	"",						// FileID = 2
};
static BOOL g_pbLogFileInit[100];

// Configration File Path
#define CFG_FILE_PATH	"TestLog.cfg"

// Configration Log Enable/Disable
const static BOOL g_bConfigLog = TRUE;

//////////////////////////////////////////////////////////////////////
// Log Type 1
// - Line Log
//--------------------------------------------------------------------
/*
#define _LOG(FILE_ID, SZ_LOG)		gLogType1(FILE_ID, SZ_LOG);
*/

//////////////////////////////////////////////////////////////////////
// Log Type 1-2
// - Line Log and Using Format
//--------------------------------------------------------------------
/*
static void gLogType1(int nFileID, const char *szLog);
static void _LOG(FILE_ID, const char *pFormat, ...)
{
	char szLog[256];
	va_list args;
	va_start(args, pFormat);
	vsprintf(szLog, pFormat, args);
	va_end(args);
	gLogType1(FILE_ID, szLog);
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
static void gLogType2(int nFileID, const char *szLog, int nLogID);
#define _LOG_V_LIST(FILE_ID, P_FORMAT, LOG_ID)			\
{														\
	char _szLog[256];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szLog, P_FORMAT, args);					\
	va_end(args);										\
	gLogType2(FILE_ID, _szLog, LOG_ID);					\
}

//--------------------------------------------------------------------
// _LOG Function Definition (LogID : 0 ~ 99)
static void _A_LOG(const char *pFormat, ...)		{ _LOG_V_LIST(0, pFormat, 0) }
static void _B_LOG(const char *pFormat, ...)		{ _LOG_V_LIST(1, pFormat, 0) }
//.......
//.......
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Log Type 3
// - Source Code Information Log
//--------------------------------------------------------------------
/*
static void gLogType3(int nFileID, const char *szLog, const char *szSrcFile, int nSrcLine);
#define _LOG(FILE_ID, SZ_LOG)							\
{														\
	const char* __szLog = SZ_LOG;						\
	const char* __file = __FILE__;						\
	int __line = __LINE__;								\
	gLogType3(FILE_ID, __szLog, __file, __line);		\
}
*/

//////////////////////////////////////////////////////////////////////
// Log Type 4
// - Source Code Information Log
// - Using CFG_FILE_PATH for LogID and Log-Enable/Disable
//--------------------------------------------------------------------
/*
static void gLogType4(int nFileID, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
#define _LOG(FILE_ID, SZ_LOG, INT_LOG_ID)				\
{														\
	const char* __szLog = SZ_LOG;						\
	const char* __file = __FILE__;						\
	int __line = __LINE__;								\
	gLogType4(FILE_ID, __szLog, __file, __line, INT_LOG_ID); \
}
#define _LOG_1(SZ_LOG)			{ _LOG(SZ_LOG, 1) }
*/

//////////////////////////////////////////////////////////////////////
// #include <window.h>

typedef DWORD (*LP_INIT_LOG_DLLFUNC)(int nFileID, const char* szLogFile);
typedef DWORD (*LP_CLOSE_LOG_DLLFUNC)(int nFileID);
typedef DWORD (*LP_INIT_CFG_DLLFUNC)(int nFileID, const char* szCfgFile);
typedef DWORD (*LP_LOG_TEXT_DLLFUNC)(int nFileID, const char* szLog, int nLogID);
typedef DWORD (*LP_LOG_INFO_DLLFUNC)(int nFileID, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID);
static LP_INIT_LOG_DLLFUNC _gInitLogM;
static LP_INIT_LOG_DLLFUNC _gInitLogM2;
static LP_CLOSE_LOG_DLLFUNC _gCloseLogM;
static LP_INIT_CFG_DLLFUNC _gInitLogCfgM;
static LP_LOG_TEXT_DLLFUNC _gLogTextM;
static LP_LOG_INFO_DLLFUNC _gLogInfoM;

static HMODULE g_hLogDllModule = NULL;
static BOOL g_bTryInitLogDll = FALSE;

static void gLoadLogDll()
{
	if(NULL == g_hLogDllModule)
	{
		g_hLogDllModule = LoadLibrary("TextLog.dll");
		if(NULL != g_hLogDllModule)
		{
			_gInitLogM = (LP_INIT_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLogM");
			_gInitLogM2 = (LP_INIT_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLogM2");
			_gCloseLogM = (LP_CLOSE_LOG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gCloseLogM");
			_gInitLogCfgM = (LP_INIT_CFG_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gInitLogCfgM");
			_gLogTextM = (LP_LOG_TEXT_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gLogTextM");
			_gLogInfoM = (LP_LOG_INFO_DLLFUNC) GetProcAddress(g_hLogDllModule, "_gLogInfoM");
			if(_gInitLogM == NULL || _gCloseLogM == NULL || _gInitLogCfgM == NULL
				|| _gLogTextM == NULL || _gLogInfoM == NULL)
			{
				FreeLibrary(g_hLogDllModule);
				g_hLogDllModule = NULL;
			}
		}
	}
}

static BOOL gInitLogDll()
{
	BOOL bRet = FALSE;
	do {
		gLoadLogDll();
		if(NULL == g_hLogDllModule)
		{
		//	MessageBox(NULL, "'TextLog.dll' Loading failed !",
		//		"TextLogDll.h", MB_OK| MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_TOPMOST);
			break;
		}

		for(int i=0; i<100; i++) g_pbLogFileInit[i] = FALSE;
		bRet = TRUE;
	} while(0);

	g_bTryInitLogDll = TRUE;
	return bRet;
}

static void gInitLogFile(int nFileID, const char *szLogFile, const char *szCfgFile)
{
	if(NULL != g_hLogDllModule)
	{
		if(g_bConfigLog || _gInitLogM2 == NULL) _gInitLogM(nFileID, szLogFile);
		else _gInitLogM2(nFileID, szLogFile);
		if(szCfgFile != 0)
		{
			if(*szCfgFile != 0)	 _gInitLogCfgM(nFileID, szCfgFile);
			g_pbLogFileInit[nFileID] = TRUE;
		}
	}
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

static void gLogType1(int nFileID, const char *szLog)
{
	if(! g_bTryInitLogDll) gInitLogDll();
	if(g_hLogDllModule == NULL) return;
	if(! g_pbLogFileInit[nFileID]) gInitLogFile(nFileID, g_szLogPath[nFileID], NULL);
	_gLogTextM(nFileID, szLog, 0);
}

static void gLogType2(int nFileID, const char *szLog, int nLogID)
{
	if(! g_bTryInitLogDll) gInitLogDll();
	if(g_hLogDllModule == NULL) return;
	if(! g_pbLogFileInit[nFileID]) gInitLogFile(nFileID, g_szLogPath[nFileID], CFG_FILE_PATH);
	_gLogTextM(nFileID, szLog, nLogID);
}

static void gLogType3(int nFileID, const char *szLog, const char *szSrcFile, int nSrcLine)
{
	if(! g_bTryInitLogDll) gInitLogDll();
	if(g_hLogDllModule == NULL) return;
	if(! g_pbLogFileInit[nFileID]) gInitLogFile(nFileID, g_szLogPath[nFileID], NULL);
	_gLogInfoM(nFileID, szLog, szSrcFile, nSrcLine, 0);
}

static void gLogType4(int nFileID, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(! g_bTryInitLogDll) gInitLogDll();
	if(g_hLogDllModule == NULL) return;
	if(! g_pbLogFileInit[nFileID]) gInitLogFile(nFileID, g_szLogPath[nFileID], CFG_FILE_PATH);
	_gLogInfoM(nFileID, szLog, szSrcFile, nSrcLine, nLogID);
}

//////////////////////////////////////////////////////////////////////
#endif
