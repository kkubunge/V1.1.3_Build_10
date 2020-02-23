// TextLogDll_M.h: Multi Log Interface for the 'TextLog.DLL'
//   - 'Multi Object to Multi Log' Supported
//   - TextLog.Dll Version 2.6.1.5 or Upper
// Create : 2004.04.06
// Last Update : 2012.02.26
// Programed by Chang-su,Kim.
//////////////////////////////////////////////////////////////////////

#ifndef _TEXTLOGDLL_H_
#define _TEXTLOGDLL_H_

// Log File Name
#define LOG_FILE_PATH				"_Log\\MFC_Vefification.log"

// Configration File Path
#define CFG_FILE_PATH				""

// DLL File Name
#define DLL_FILE_NAME				"TextLog.dll"

//--------------------------------------------------------------------
// Initialization
#define	_gINIT_LOG()	_gInitLogDll(LOG_FILE_PATH, CFG_FILE_PATH, FALSE)

//--------------------------------------------------------------------
// Deinitialization
#define _gCLOSE_LOG()	_gFreeLogDll()

//--------------------------------------------------------------------
static BOOL _gIS_LOG_FILE_CHANGED();										// 2009.04.01

//--------------------------------------------------------------------
// Initialization for Multi Object Multi File Log in C++ Code
// : If use in C Code, Above sentence is made to comment
/*
static BOOL _gInitLogDll(const char *szLogFile = LOG_FILE_PATH,
						 const char *szCfgFile = CFG_FILE_PATH,
						 BOOL bConfigLog = TRUE);	// Configration Information Log Enable/Disable
*/

//--------------------------------------------------------------------
// Low Function Definition
static void _gLogType1(const char *szLog);
static void _gLogType2(const char *szLog, int nLogID);
static void _gLogType3(const char *szLog, const char *szSrcFile, int nSrcLine);
static void _gLogType4(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID);

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
static void _LOG(const char *pFormat, ...)
{
	char szLog[1024];
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
#define _LOG_V_LIST(P_FORMAT, LOG_ID)					\
{														\
	char _szLog[1024];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szLog, P_FORMAT, args);					\
	va_end(args);										\
	_gLogType2(_szLog, LOG_ID);							\
}
//--------------------------------------------------------------------
// _LOG Function Definition (LogID : 0 ~ 19)
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

// 2009.04.01
typedef BOOL (*LP_IS_LOG_FILE_CHANGED)(int nInstID);
static LP_IS_LOG_FILE_CHANGED _gIsLogFileChanged;

typedef DWORD (*LP_INIT_LOG_DLLFUNC)(int nInstID, const char* szLogFile);
typedef DWORD (*LP_CLOSE_LOG_DLLFUNC)(int nInstID);
typedef DWORD (*LP_INIT_CFG_DLLFUNC)(int nInstID, const char* szCfgFile);
typedef DWORD (*LP_LOG_TEXT_DLLFUNC)(int nInstID, const char* szLog, int nLogID);
typedef DWORD (*LP_LOG_INFO_DLLFUNC)(int nInstID, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID);
typedef DWORD (*LP_gTextLogDll_Command)(const char *szCmd, char *szReply /*= NULL*/);

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

//--------------------------------------------------------------------
// Global Status Variable
static BOOL g_bLogDllLoaded = FALSE;
static BOOL g_bTryToInit = FALSE;
static HMODULE g_hLogDllModule = NULL;
static int g_nInstanceID = -1;		// 2006.03.13

//--------------------------------------------------------------------
// Global Static Function
static void _gLoadLogDll()
{
	if(NULL == g_hLogDllModule)
	{
		g_hLogDllModule = LoadLibrary(DLL_FILE_NAME);
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
			
			if(_gGetLogInstance != NULL)
			{
				g_nInstanceID = _gGetLogInstance();
				if(g_nInstanceID < 0)
				{
				//	MessageBox(NULL, "'_gGetLogInstance() Failed in 'TextLog.dll' !",
				//		"TextLogDll.h", MB_OK| MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_TOPMOST);
				}
			}
			//---------------------------------------------------------------------

			if(_gInitLogM == NULL || _gCloseLogM == NULL || _gInitLogCfgM == NULL
				|| _gLogTextM == NULL || _gLogInfoM == NULL || _gTextLogDll_Command == NULL
				|| _gGetLogInstance == NULL || _gReleaseLogInst == NULL || _gReleaseAllLogInst == NULL
				|| g_nInstanceID < 0)
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
	// char szTemp[80];

	g_bTryToInit = TRUE;
	do {
		_gLoadLogDll();
		if(NULL != g_hLogDllModule)
		{
			// _gTextLogDll_Command("MSG_ENABLE YES", NULL);		// 2006.08.23
			// _gTextLogDll_Command("BUFFER_SIZE 240", NULL);			// 2006.07.25
			// _gTextLogDll_Command("BUFFERING_ENABLE YES", NULL);		// 2006.08.23

			if(bConfigLog || _gInitLogM2 == NULL) _gInitLogM(g_nInstanceID, szLogFile);
			else _gInitLogM2(g_nInstanceID, szLogFile);
			if(szCfgFile != NULL && szCfgFile[0] != 0)
			{
				if(*szCfgFile != 0)	 _gInitLogCfgM(g_nInstanceID, szCfgFile);
			}

			// 2011.07.27
			// sprintf(szTemp, "SET_FILE_CR_P_HOURS %d 4", g_nInstanceID);
			// _gTextLogDll_Command(szTemp, NULL);	
			// sprintf(szTemp, "SET_KEEP_DAYS %d 30", g_nInstanceID);
			// _gTextLogDll_Command(szTemp, NULL);
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
		_gReleaseLogInst(g_nInstanceID);
		FreeLibrary(g_hLogDllModule);
		g_hLogDllModule = NULL;
	}
}

static void _gLogType1(const char *szLog)
{
	if(g_bTryToInit==FALSE && g_bLogDllLoaded==FALSE) _gINIT_LOG();
	if(g_bLogDllLoaded) _gLogTextM(g_nInstanceID, szLog, 0);
}

static void _gLogType2(const char *szLog, int nLogID)
{
	if(g_bTryToInit==FALSE && g_bLogDllLoaded==FALSE) _gINIT_LOG();
	if(g_bLogDllLoaded) _gLogTextM(g_nInstanceID, szLog, nLogID);
}

static void _gLogType3(const char *szLog, const char *szSrcFile, int nSrcLine)
{
	if(g_bTryToInit==FALSE && g_bLogDllLoaded==FALSE) _gINIT_LOG();
	if(g_bLogDllLoaded) _gLogInfoM(g_nInstanceID, szLog, szSrcFile, nSrcLine, 0);
}

static void _gLogType4(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(g_bTryToInit==FALSE && g_bLogDllLoaded==FALSE) _gINIT_LOG();
	if(g_bLogDllLoaded) _gLogInfoM(g_nInstanceID, szLog, szSrcFile, nSrcLine, nLogID);
}

// 2009.04.01
static BOOL _gIS_LOG_FILE_CHANGED()
{
	BOOL bRet = FALSE;
	if(g_bTryToInit==FALSE && g_bLogDllLoaded==FALSE) _gINIT_LOG();
	if(g_bLogDllLoaded && _gIsLogFileChanged != NULL) bRet = _gIsLogFileChanged(g_nInstanceID);
	return bRet;
}

#endif

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/* Update History
2004.04.06	Created.
2005.10.12	'TextLogDll.h'에 Init 과 Deinit Function인 '_gInitLogDll'
			과 '_gFreeLogDll' Function 추가함. Log 긴을 사용하기 위해서는
			초기에 '_gInitLogDll' Function을 호출해 주어야 함.
2005.10.20	C Code에서 'TextLogDll.h' New Type 가능 하도록 수정.
2005.12.05	Update History 기록 시작
2005.12.05	'_gInitLogDll'을 초기에 호출하지 않을 경우 자동 호출되도록 수정
			Log 초기화 및 Instance 해제 Function을 '#define' 문으로 정의하여
			C Code와 C++ Code에서 혼용이 용이하도록 수정.
				=> #define _gINIT_LOG()		_gInitLogDll(...)
				=> #define _gCLOSE_LOG()	_gFreeLogDll(...)
2005.12.08	C Code에서 해석되지 않는 일부 코드 주석처리.
			C++ Code에서 사용이 필요할 시 주석을 풀고 사용할 것.
				=> '_gTextLogDll_Command Function'
2006.01.24	Global Status Variable 위치 변경
			기능상 또는 Sequence 상 변경점은 없음.
2006.03.07	TextLog.Dll Version 2.6.1.4 이상에서 Log Message 기능을 Enable
			하기 위한 Command Line 추가
			=> _gTextLogDll_Command("MSG_ENABLE", NULL);
2006.03.13	Multi Log 기능 관련 Instance 관리 Routine 추가
			TextLog.Dll Version 2.6.1.5 이상에서 사용 가능
2006.03.17	Low Function Definition 추가
			기능상 또는 Sequence 상 변경점은 없음.
2006.07.25	TextLog.Dll Version 2.6.2.1 이상에서 Buffer Size 조정 기능 추가
2006.08.23	TextLog.Dll Version 2.6.2.4 이상에서 Buffering을 Enable/Disable
			할 수 있는 기능 추가
				=> _gTextLogDll_Command("BUFFER_SIZE 240", NULL);
				=> _gTextLogDll_Command("BUFFERING_ENABLE YES", NULL);
			TextLog.Dll Version 2.6.2.4 이상에서 Message Enable/Disable
			할 수 있는 Command Paremter 방식 수정
				=> _gTextLogDll_Command("MSG_ENABLE YES", NULL);
2009.04.01	TextLog.Dll Version 2.6.2.9 이상에서 Log File 변경 시점을 Check 하는 Function 추가
				=> BOOL _gIS_LOG_FILE_CHANGED();
				=> return 값이 TRUE 이면 날짜가 넘어감에 따라 File이 변경됨.
2010.04.07	Buffer Size를 256에서 1024로 확장
2010.04.29	DLL_FILE_NAME Definition 추가
2011.07.27	지정 시간 단위로 Log 할 수 있는 기능 추가
			KeepDays를 Func Call로 지정할 수 있는 기능 추가
2012.02.26	Version History 누락 부분 추가 
--------------------------------------------------------------------*/
