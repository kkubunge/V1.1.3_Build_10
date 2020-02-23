//////////////////////////////////////////////////////////////////////
// TextLogDll
// Create : 2004.04.06
// Last Update : 2006.
// Writer : Chang-su, Kim
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include "TextLog.h"
#include "TextParser.h"
#include "MutexLock.h"

static CTextLog *g_pLog = NULL;
static bool g_bMsgEnable = false;
static int g_nBufferSize = 1024;
static bool g_bBufferingEnable = false;		// 2006.08.23

extern "C" __declspec(dllexport) DWORD _gInitLog(const char* szLogFile)
{
	if(NULL == g_pLog) g_pLog = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY | LG_OPT_CONFIG_LOG,
					g_bMsgEnable, g_nBufferSize, g_bBufferingEnable);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gInitLog2(const char* szLogFile)
{
	if(NULL == g_pLog) g_pLog = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY,
					g_bMsgEnable, g_nBufferSize, g_bBufferingEnable);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gCloseLog()
{
	delete g_pLog;
	g_pLog = NULL;
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gInitLogCfg(const char* szCfgFile)
{
	if(g_pLog != NULL) g_pLog->InitConfiguration(szCfgFile);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gLogText(const char* szLog, int nLogID)
{
	if(g_pLog != NULL) g_pLog->LogText(szLog, nLogID);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gLogInfo(const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(g_pLog != NULL) g_pLog->LogInfo(szLog, szSrcFile, nSrcLine, nLogID);
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Multi Log File Function
// nInstID : 0 ~ 31

//-------------------------------------
// 2006.03.13
typedef struct LOG_INST
{
	CTextLog *pTextLog;
	bool bLock;
} _LOG_INST;

static _LOG_INST g_pInstList[] =
{
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false },
	{ NULL, false }
};
//-------------------------------------

extern "C" __declspec(dllexport) int _gGetLogInstance()
{
	int i;
	int nMaxInstance;
	int nInstID = -1;

	char szMutexName[80];
	sprintf(szMutexName, "TEXT_LOG_INSTANCE_SHARING_LOCK_%d", GetCurrentProcessId());
	CMutexLock mutexLock(szMutexName);

	nMaxInstance = sizeof(g_pInstList) / sizeof(_LOG_INST);
	for(i=0; i<nMaxInstance; i++)
	{
		if(! g_pInstList[i].bLock)
		{
			g_pInstList[i].bLock = true;
			nInstID = i;
			break;
		}
	}
	return nInstID;
}

extern "C" __declspec(dllexport) void _gReleaseLogInst(int nInstID)
{
	DWORD _gCloseLogM(int nInstID);
	_gCloseLogM(nInstID);
	g_pInstList[nInstID].bLock = false;
}

extern "C" __declspec(dllexport) DWORD _gInitLogM(int nInstID, const char* szLogFile)
{
	if(NULL == g_pInstList[nInstID].pTextLog)
	{
		g_pInstList[nInstID].pTextLog = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY | LG_OPT_CONFIG_LOG,
						g_bMsgEnable, g_nBufferSize, g_bBufferingEnable);
	}
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gInitLogM2(int nInstID, const char* szLogFile)
{
	if(NULL == g_pInstList[nInstID].pTextLog) 
	{
		g_pInstList[nInstID].pTextLog = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY,
						g_bMsgEnable, g_nBufferSize, g_bBufferingEnable);
	}
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gCloseLogM(int nInstID)
{
	if(g_pInstList[nInstID].pTextLog != NULL)
	{
		delete g_pInstList[nInstID].pTextLog;
		g_pInstList[nInstID].pTextLog = NULL;
	}
	return 0;
}

extern "C" __declspec(dllexport) void _gReleaseAllLogInst()
{
	int i;
	int nMaxInstance = sizeof(g_pInstList) / sizeof(_LOG_INST);
	for(i=0; i<nMaxInstance; i++) _gReleaseLogInst(i);
}

extern "C" __declspec(dllexport) DWORD _gInitLogCfgM(int nInstID, const char* szCfgFile)
{
	if(g_pInstList[nInstID].pTextLog != NULL) g_pInstList[nInstID].pTextLog->InitConfiguration(szCfgFile);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gLogTextM(int nInstID, const char* szLog, int nLogID)
{
	if(g_pInstList[nInstID].pTextLog != NULL) g_pInstList[nInstID].pTextLog->LogText(szLog, nLogID);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gLogInfoM(int nInstID, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(g_pInstList[nInstID].pTextLog != NULL)
			g_pInstList[nInstID].pTextLog->LogInfo(szLog, szSrcFile, nSrcLine, nLogID);
	return 0;
}

extern "C" __declspec(dllexport) BOOL _gIsLogFileChanged(int nInstID)
{
	BOOL bRet = FALSE;
	if(g_pInstList[nInstID].pTextLog != NULL)
	{
		if(g_pInstList[nInstID].pTextLog->IsLogFileChanged()) bRet = TRUE;
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////
// Common Function

extern "C" __declspec(dllexport) DWORD _gTextLogDll_Command(const char *szCmd, char *szReply)
{
	// 2006.03.07
	CTextParser tp;
	char szTemp[80];
	int nValue = 0;
	int nInstID = -1;

	strcpy(szTemp, szCmd);
	tp.MakeUpper(szTemp);
	tp.SetText(szTemp);
	tp.GetWord();
	if(tp.BeEqual("MSG_ENABLE")) tp.GetBool(&g_bMsgEnable);							// 2006.08.23
	else if(tp.BeEqual("BUFFER_SIZE")) tp.GetInt(&g_nBufferSize);					// 2006.07.25
	else if(tp.BeEqual("BUFFERING_ENABLE")) tp.GetBool(&g_bBufferingEnable);		// 2006.08.23

	// 2011.07.27
	else if(tp.BeEqual("SET_FILE_CR_P_HOURS"))
	{
		tp.GetInt(&nInstID);
		tp.GetInt(&nValue);
		if(nInstID >= 0) g_pInstList[nInstID].pTextLog->SetFileCreatePeriodPerHours(nValue);
	}
	else if(tp.BeEqual("SET_KEEP_DAYS"))
	{
		tp.GetInt(&nInstID);
		tp.GetInt(&nValue);
		if(nInstID >= 0) g_pInstList[nInstID].pTextLog->SetKeepDays(nValue);
	}

	// 2013.06.13
	else if(tp.BeEqual("SET_LOG_TYPE"))
	{
		tp.GetInt(&nInstID);
		tp.GetInt(&g_pInstList[nInstID].pTextLog->m_nLogType);
	}

	return 0;
}
