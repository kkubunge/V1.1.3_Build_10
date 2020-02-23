//////////////////////////////////////////////////////////////////////
// TextLogDll
// Create : 2004.04.06
// Last Update : 2005.07.01 (for Ver 2.5.6.0)
// Writer : Chang-su, Kim
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "TextLog.h"

static CTextLog *g_pLog = NULL;

extern "C" __declspec(dllexport) DWORD _gInitLog(const char* szLogFile)
{
	if(NULL == g_pLog) g_pLog = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY | LG_OPT_CONFIG_LOG);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gInitLog2(const char* szLogFile)
{
	if(NULL == g_pLog) g_pLog = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY);
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
	if(g_pLog != NULL) g_pLog->InitConfigration(szCfgFile);
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
// nFileID : 0 ~ 99

static CTextLog *g_pLogM[] =
{
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

extern "C" __declspec(dllexport) DWORD _gInitLogM(int nFileID, const char* szLogFile)
{
	if(NULL == g_pLogM[nFileID])
		g_pLogM[nFileID] = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY | LG_OPT_CONFIG_LOG);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gInitLogM2(int nFileID, const char* szLogFile)
{
	if(NULL == g_pLogM[nFileID]) 
		g_pLogM[nFileID] = new CTextLog(szLogFile, FILE_OPT_CREATE_PER_DAY);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gCloseLogM(int nFileID)
{
	delete g_pLogM[nFileID];
	g_pLogM[nFileID] = NULL;
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gInitLogCfgM(int nFileID, const char* szCfgFile)
{
	if(g_pLogM[nFileID] != NULL) g_pLogM[nFileID]->InitConfigration(szCfgFile);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gLogTextM(int nFileID, const char* szLog, int nLogID)
{
	if(g_pLogM[nFileID] != NULL) g_pLogM[nFileID]->LogText(szLog, nLogID);
	return 0;
}

extern "C" __declspec(dllexport) DWORD _gLogInfoM(int nFileID, const char *szLog, const char *szSrcFile, int nSrcLine, int nLogID)
{
	if(g_pLogM[nFileID] != NULL)
			g_pLogM[nFileID]->LogInfo(szLog, szSrcFile, nSrcLine, nLogID);
	return 0;
}
