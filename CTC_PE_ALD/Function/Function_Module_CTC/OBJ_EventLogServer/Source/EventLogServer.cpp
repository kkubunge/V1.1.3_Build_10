// EventLogServer.cpp: implementation of the CEventLogServer class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

//--------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//--------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll_M2.h"
#include "TextParser.h"
#include "MsgBoxDll.h"
#include "EventLogServer.h"

//--------------------------------------------------------------------
#define CONFIG_FILE	".\\Parameter\\MARS\\EventLogServer.ini"

#define DEFAULT_RUN_LOG_PATH	".\\_Log\\EventLog\\EventLog.log"
#define DEFAULT_MCC_LOG_PATH	".\\_Log\\MCC\\MCC_Ex.log"

#define LOG_MAX_SIZE	256 

//--------------------------------------------------------------------
enum { LTYPE_PRC, LTYPE_XFR, LTYPE_FNC, LTYPE_LEH, LTYPE_USER, LTYPE_CFG };

enum {
	PMC_Unuse, PMC_Local, PMC_DisCon,  PMC_Standby, PMC_Maint,
	PMC_Uninit, PMC_EnablePM, PMC_CTCInUse, PMC_CTCDis, PMC_CTCDis2,
	PMC_CTCEnaPM, PMC_CTCDisHW, PMC_CTCDisHW2
};

enum {
	LPM_Idle, LPM_Running, LPM_Pausing, LPM_Paused, LPM_Aborting,
	LPM_Disable, LPM_Waiting, LPM_WaitHandOff
 };

enum { ATYPE_READY, ATYPE_GET, ATYPE_PUT, ATYPE_MOVE, ATYPE_ROT };

enum { EN_PM1, EN_PM2, EN_PM3, EN_PM4, EN_PM5 };
enum { EN_LPM1, EN_LPM2, EN_LPM3, EN_LPM4 };
enum { WS_LPM1, WS_LPM2, WS_LPM3, WS_LPM4, WS_LPMY, WS_DUMMY, WS_MON };

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

//--------------------------------------------------------------------
// Standard Module Name
typedef struct _STARDARD_MODULE_NAME_TABLE
{
	char szPreModuleName[16];
	char szStdModuleName[16];
} STARDARD_MODULE_NAME_TABLE;

#define MAX_MODULE_NAME_TABLE		30
static STARDARD_MODULE_NAME_TABLE g_StdModNameTbl[MAX_MODULE_NAME_TABLE] =
{
	{"CM1", "Port1"}, {"CM2", "Port2"}, {"CM3", "Port3"},  {"CM4", "Port4"},
	{"BM1", "LL1"},	{"A_BM1", "LL1"}, {"B_BM1", "LL1"},
	{"BM2", "LL2"}, {"A_BM2", "LL2"}, {"B_BM2", "LL2"},
	{"ATM", "ATM1"}, {"TM", "TM1"},
	{"AL", "Aligner"},
	{"PORT1", "Port1"}, {"PORT2", "Port2"}, {"PORT3", "Port3"},  {"PORT4", "Port4"},
	{"A_PM1", "PM1"}, {"B_PM1", "PM1"},
	{"A_PM2", "PM2"}, {"B_PM2", "PM2"},
	{"A_PM3", "PM3"}, {"B_PM3", "PM3"},
	{"", ""}, {"", ""},
	{"", ""}, {"", ""}, {"", ""},  {"", ""},  {"", ""}
};

//--------------------------------------------------------------------
// Module Run Definition
#define MAX_RUN_MODULE				20
typedef struct _MODULE_RUN_DEF_TABLE
{
	int nPMIdx;
	char aszRunModule[MAX_RUN_MODULE][16];
} MODULE_RUN_DEF_TABLE;

#define MAX_MOD_RUN_DEF_TABLE		10
static MODULE_RUN_DEF_TABLE g_ModuleRunDefTbl[MAX_MOD_RUN_DEF_TABLE] =
{
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
	{ -1, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },
};

//--------------------------------------------------------------------
// 2013.07.16
// Module Information
#define MAX_MODULE_INFO				20
#define MAX_ROBOT_INFO				4
#define MAX_LLSLOT_INFO				8

typedef struct _MODULE_INFO
{
	char szModule[16];
	int nSlotCount;
	char szWaferInfo[40];			// 2013.11.18
} MODULE_INFO;
static MODULE_INFO g_ModuleInfo[MAX_MODULE_INFO] =
{
	{ "PM1", 6, ""}, { "PM2", 6, ""},
	{ "LL1", 6, ""}, { "LL2", 6, ""},
	{ "", 0, "" }, { "", 0, "" },
	{ "", 0, "" }, { "", 0, "" },
	{ "", 0, "" }, { "", 0, "" },
	{ "", 0, "" }, { "", 0, "" },
	{ "", 0, "" }, { "", 0, "" },
	{ "", 0, "" }, { "", 0, "" },
	{ "", 0, "" }, { "", 0, "" },
	{ "", 0, "" }, { "", 0, "" },	
};


typedef struct _Pre_MODULE_INFO
{
	char szModule[16];
	char szPreModule[16];
	char szPreModuleSlot[5];
} Pre_MODULE_INFO;

static Pre_MODULE_INFO g_PreModuleInfo[MAX_ROBOT_INFO] = 
{
	{ "ATM1", "", "" }, //Module, Station, Slot
	{ "ATM2", "", "" },
	{ "TM1",  "", "" }, 
	{ "TM2",  "", "" },
};

typedef struct _LL_Wafer_INFO
{
	char szModule[16];
	int  nSlot;
	char szWaferInfo[40];
} LL_Wafer_INFO;

static LL_Wafer_INFO g_LLWaferInfo[MAX_LLSLOT_INFO] = 
{
	{ "LL1", 1, "" }, //Module, Slot, WaferInfo
	{ "LL1", 2, "" }, 
	{ "LL1", 3, "" }, 
	{ "LL1", 4, "" }, 
	{ "LL2", 1, "" }, //Module, Slot, WaferInfo
	{ "LL2", 2, "" }, 
	{ "LL2", 3, "" }, 
	{ "LL2", 4, "" }, 	
};
 
int g_nStartLot = 0;

//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	CTC_PM1_Wafer_Source,
	CTC_PM1_Wafer_Source2,
	CTC_PM1_Wafer_Source3,
	CTC_PM1_Wafer_Source4,
	CTC_PM1_Wafer_Source5,
	CTC_PM1_Wafer_Source6,
	CTC_PM1_Wafer_Status,
	CTC_PM1_Wafer_Status2,
	CTC_PM1_Wafer_Status3,
	CTC_PM1_Wafer_Status4,
	CTC_PM1_Wafer_Status5,
	CTC_PM1_Wafer_Status6,

	CTC_PM2_Wafer_Source,
	CTC_PM2_Wafer_Source2,
	CTC_PM2_Wafer_Source3,
	CTC_PM2_Wafer_Source4,
	CTC_PM2_Wafer_Source5,
	CTC_PM2_Wafer_Source6,
	CTC_PM2_Wafer_Status,
	CTC_PM2_Wafer_Status2,
	CTC_PM2_Wafer_Status3,
	CTC_PM2_Wafer_Status4,
	CTC_PM2_Wafer_Status5,
	CTC_PM2_Wafer_Status6,

	CTC_PM3_Wafer_Source,
	CTC_PM3_Wafer_Source2,
	CTC_PM3_Wafer_Status,
	CTC_PM3_Wafer_Status2,

	CTC_PM4_Wafer_Source,
	CTC_PM4_Wafer_Source2,
	CTC_PM4_Wafer_Status,
	CTC_PM4_Wafer_Status2,

	CTC_PM5_Wafer_Source,
	CTC_PM5_Wafer_Source2,
	CTC_PM5_Wafer_Status,
	CTC_PM5_Wafer_Status2,

	CTC_BM1_Wafer_Source,
	CTC_BM1_Wafer_Source2,
	CTC_BM1_Wafer_Source3,
	CTC_BM1_Wafer_Source4,
	CTC_BM1_Wafer_Source5,
	CTC_BM1_Wafer_Source6,
	CTC_BM1_Wafer_Source7,
	CTC_BM1_Wafer_Source8,
	CTC_BM1_Wafer_Status,
	CTC_BM1_Wafer_Status2,
	CTC_BM1_Wafer_Status3,
	CTC_BM1_Wafer_Status4,
	CTC_BM1_Wafer_Status5,
	CTC_BM1_Wafer_Status6,
	CTC_BM1_Wafer_Status7,
	CTC_BM1_Wafer_Status8,

	CTC_BM2_Wafer_Source,
	CTC_BM2_Wafer_Source2,
	CTC_BM2_Wafer_Source3,
	CTC_BM2_Wafer_Source4,
	CTC_BM2_Wafer_Source5,
	CTC_BM2_Wafer_Source6,
	CTC_BM2_Wafer_Source7,
	CTC_BM2_Wafer_Source8,
	CTC_BM2_Wafer_Status,
	CTC_BM2_Wafer_Status2,
	CTC_BM2_Wafer_Status3,
	CTC_BM2_Wafer_Status4,
	CTC_BM2_Wafer_Status5,
	CTC_BM2_Wafer_Status6,
	CTC_BM2_Wafer_Status7,
	CTC_BM2_Wafer_Status8,

	CTC_TA_Wafer_Source,
	CTC_TA_Wafer_Source2,
	CTC_TB_Wafer_Source,
	CTC_TB_Wafer_Source2,
	CTC_TA_Wafer_Status,
	CTC_TA_Wafer_Status2,
	CTC_TB_Wafer_Status,
	CTC_TB_Wafer_Status2,

	CTC_TA2_Wafer_Source,
	CTC_TB2_Wafer_Source,
	CTC_TA2_Wafer_Status,
	CTC_TB2_Wafer_Status,

	CTC_FA_Wafer_Source,
	CTC_FB_Wafer_Source,
	CTC_FA_Wafer_Status,
	CTC_FB_Wafer_Status,

	CTC_FM_AL_Wafer_Source,
	CTC_FM_AL_Wafer_Status,

} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"CTC.PM1_Wafer_Source",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Source2",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Source3",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Source4",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Source5",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Source6",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Status",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Status2",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Status3",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Status4",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Status5",		_K_D_IO, 0},
	{"CTC.PM1_Wafer_Status6",		_K_D_IO, 0},

	{"CTC.PM2_Wafer_Source",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Source2",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Source3",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Source4",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Source5",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Source6",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Status",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Status2",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Status3",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Status4",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Status5",		_K_D_IO, 0},
	{"CTC.PM2_Wafer_Status6",		_K_D_IO, 0},

	{"CTC.PM3_Wafer_Source",		_K_D_IO, 0},
	{"CTC.PM3_Wafer_Source2",		_K_D_IO, 0},
	{"CTC.PM3_Wafer_Status",		_K_D_IO, 0},
	{"CTC.PM3_Wafer_Status2",		_K_D_IO, 0},

	{"CTC.PM4_Wafer_Source",		_K_D_IO, 0},
	{"CTC.PM4_Wafer_Source2",		_K_D_IO, 0},
	{"CTC.PM4_Wafer_Status",		_K_D_IO, 0},
	{"CTC.PM4_Wafer_Status2",		_K_D_IO, 0},

	{"CTC.PM5_Wafer_Source",		_K_D_IO, 0},
	{"CTC.PM5_Wafer_Source2",		_K_D_IO, 0},
	{"CTC.PM5_Wafer_Status",		_K_D_IO, 0},
	{"CTC.PM5_Wafer_Status2",		_K_D_IO, 0},

	{"CTC.BM1_Wafer_Source",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Source2",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Source3",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Source4",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Source5",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Source6",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Source7",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Source8",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status2",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status3",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status4",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status5",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status6",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status7",		_K_D_IO, 0},
	{"CTC.BM1_Wafer_Status8",		_K_D_IO, 0},

	{"CTC.BM2_Wafer_Source",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Source2",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Source3",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Source4",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Source5",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Source6",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Source7",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Source8",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status2",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status3",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status4",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status5",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status6",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status7",		_K_D_IO, 0},
	{"CTC.BM2_Wafer_Status8",		_K_D_IO, 0},

	{"CTC.TA_Wafer_Source",			_K_D_IO, 0},
	{"CTC.TA_Wafer_Source2",			_K_D_IO, 0},			// 2013.11.18
	{"CTC.TB_Wafer_Source",			_K_D_IO, 0},
	{"CTC.TB_Wafer_Source2",			_K_D_IO, 0},			// 2013.11.18
	{"CTC.TA_Wafer_Status",			_K_D_IO, 0},
	{"CTC.TA_Wafer_Status2",			_K_D_IO, 0},			// 2013.11.18
	{"CTC.TB_Wafer_Status",			_K_D_IO, 0},
	{"CTC.TB_Wafer_Status2",			_K_D_IO, 0},			// 2013.11.18

	{"CTC.TA2_Wafer_Source",		_K_D_IO, 0},
	{"CTC.TB2_Wafer_Source",		_K_D_IO, 0},
	{"CTC.TA2_Wafer_Status",		_K_D_IO, 0},
	{"CTC.TB2_Wafer_Status",		_K_D_IO, 0},

	{"CTC.FA_Wafer_Source",			_K_D_IO, 0},
	{"CTC.FB_Wafer_Source",			_K_D_IO, 0},
	{"CTC.FA_Wafer_Status",			_K_D_IO, 0},
	{"CTC.FB_Wafer_Status",			_K_D_IO, 0},

	{"CTC.FM_AL_Wafer_Source",		_K_D_IO, 0},
	{"CTC.FM_AL_Wafer_Status",		_K_D_IO, 0},
	""
};
//--------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventLogServer::CEventLogServer()
{
	int i;

	strcpy(m_szRunLogPath, DEFAULT_RUN_LOG_PATH);
	strcpy(m_szMccLogPath, DEFAULT_MCC_LOG_PATH);
	m_nKeepingDays = 10;

	for(i=0; i<MAX_LPM; i++)
	{
		m_LPMInfo[i].bUse = FALSE;
		sprintf(m_LPMInfo[i].szDeviceID,"Port%d", i+1);
		m_LPMInfo[i].nLPMStatus = -1;
	}

	for(i=0; i<MAX_PM; i++)
	{
		m_PMInfo[i].bUse = FALSE;
		sprintf(m_PMInfo[i].szDeviceID,"PM%d", i+1);
		m_PMInfo[i].nCommStatus = -1;
		m_PMInfo[i].bIsProcRunning = FALSE;
	}

	m_LPMInfo[0].bUse = TRUE;		// Default LPM1 Enable
	m_LPMInfo[1].bUse = TRUE;		// Default LPM2 Enable
	m_PMInfo[0].bUse = TRUE;		// Default PM1 Enable
	m_PMInfo[1].bUse = TRUE;		// Default PM2 Enable

	// 2013.06.03
	strcpy(m_ProcessLog.szLogType, "PRC");
	strcpy(m_TransferLog.szLogType, "XFR");
	strcpy(m_FunctionLog.szLogType, "FNC");
	strcpy(m_LotEventLog.szLogType, "LEH");		// 2013.07.30
	strcpy(m_CfgEventLog.szLogType, "CFG");
	m_nDelimiter = D_TAB;
	strcpy(m_szDelimiter, "\t");

	// 2013.07.02
	strcpy(m_szEquipNameIO, "");
}

CEventLogServer::~CEventLogServer()
{
	_gCLOSE_LOG();
}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CEventLogServer::Initialize()
{
	// 2013.07.02
	char szEqName[40];
	int nIOStatus;

	CSeqInterface::Initialize();
	CreateThread();
	ReadConfig();

	// Init Log Config
	g_LogList[0].szLogPath = m_szMccLogPath;
	g_LogList[1].szLogPath = m_szRunLogPath;

	g_LogList[0].nKeepingDays = m_nKeepingDays;
	g_LogList[1].nKeepingDays = m_nKeepingDays;

	InitIOList(g_pIOList, false);
	_gINIT_LOG();

	_RUN_LOG("Log Server Restart");
	_LOG("Log Server Restart");

	// 2013.07.02	
	if(strlen(m_szEquipNameIO) > 1)
	{
		dREAD_STRING_FROM_NAME(m_szEquipNameIO, szEqName, &nIOStatus);
		strcat(m_szRunLogPath, szEqName);
		strcat(m_szRunLogPath, m_szRunLogFile);
	}
	return TRUE;
}

Module_Status CEventLogServer::Main()
{
	Module_Status msRet = SYS_ABORTED;
	char szParam[80];
	CTextParser tp;

	strcpy(szParam, PROGRAM_PARAMETER_READ());
	tp.SetText(szParam);
	// printf("FuncCall:%s\n", szParam);

	return msRet;
}

void CEventLogServer::Event_Message_Received()
{
	int nLogType;
	char szMessage[LOG_MAX_SIZE];
	char szLog[1000];
	BOOL bIsRunLog;
	BOOL bRslt;

	// #ifdef _DEBUG
	// _LOG("-----> Event Log Start");
	// #endif
	strcpy(szMessage, PROGRAM_EVENT_READ());
	#ifdef _DEBUG
	// _LOG("OrgMsg: %s\n", szMessage);
	#endif

	nLogType = GetLogType(szMessage);
	switch(nLogType)
	{
	case LTYPE_PRC: bRslt = MakeProcLogStr(szMessage, szLog, &bIsRunLog); break;
	case LTYPE_XFR: bRslt = MakeTransLogStr(szMessage, szLog, &bIsRunLog); break;
	case LTYPE_FNC: bRslt = MakeFuncLogStr(szMessage, szLog, &bIsRunLog); break;
	case LTYPE_LEH: bRslt = MakeLotLogStr(szMessage, szLog, &bIsRunLog); break;
	case LTYPE_USER: bRslt = MakeUserLogStr(szMessage, szLog, &bIsRunLog); break;
	case LTYPE_CFG: bRslt = MakeCfgLogStr(szMessage, szLog, &bIsRunLog); break;
	}

	if(bRslt)
	{
		if(bIsRunLog && nLogType!=LTYPE_USER)
		{
			ConvertToMARSStd(szLog);		// 2013.07.16
			_RUN_LOG(szLog);
		}
		_LOG("%s\n", szLog);
	}
	else
	{
		_LOG("-----> Invalid Event Msg => %s\n", szMessage);
	}

	PROGRAM_EVENT_STATUS_SET(SYS_SUCCESS);
	// #ifdef _DEBUG
	// _LOG("-----> Event Log End");
	// #endif
}

void CEventLogServer::Enter_Code()
{
	char szParam[LOG_MAX_SIZE];

	strcpy(szParam, PROGRAM_ARGUMENT_READ());
}

BOOL CEventLogServer::ReadConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(CONFIG_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("Event Log Configuration", szItem) == 0) nTitle = 1;
				else if(strcmp("Module Information", szItem) == 0) nTitle = 2;
//				else if(strcmp("CTC Option", szItem) == 0) nTitle = 3;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseConfig(szRead); break;
					case 2: ParseModuleInfo(szRead); break;
//					case 3: ParseCtcOption(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s\n", CONFIG_FILE);
	}
	return bRet;
}

void CEventLogServer::ParseConfig(char *szData)
{
	char szItem[80];
	CTextParser tp(szData);
	int nIdx, nNumber;
	int i;

	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "RunLogPath") == 0) tp.GetString(m_szRunLogPath);
		else if(strcmp(szItem, "RunLogFile") == 0) tp.GetString(m_szRunLogFile);
		else if(strcmp(szItem, "EuipNameIO") == 0)	tp.GetString(m_szEquipNameIO);	// 2013.07.02
		else if(strcmp(szItem, "MccLogPath") == 0) tp.GetString(m_szMccLogPath);
		else if(strcmp(szItem, "KeepingDays") == 0) tp.GetInt(&m_nKeepingDays);
		else if(strcmp(szItem, "Delimiter") == 0)
		{
			tp.GetInt(&m_nDelimiter);
			if(D_TAB == m_nDelimiter) strcpy(m_szDelimiter, "\t");
			else if(D_SPACE == m_nDelimiter) strcpy(m_szDelimiter, " ");
		}

		else if(strcmp(szItem, "LPM1_Use") == 0) tp.GetBOOL(&m_LPMInfo[EN_LPM1].bUse); 
		else if(strcmp(szItem, "LPM2_Use") == 0) tp.GetBOOL(&m_LPMInfo[EN_LPM2].bUse); 
		else if(strcmp(szItem, "LPM3_Use") == 0) tp.GetBOOL(&m_LPMInfo[EN_LPM3].bUse); 
		else if(strcmp(szItem, "LPM4_Use") == 0) tp.GetBOOL(&m_LPMInfo[EN_LPM4].bUse); 

		else if(strcmp(szItem, "PM1_Use") == 0) tp.GetBOOL(&m_PMInfo[EN_PM1].bUse); 
		else if(strcmp(szItem, "PM2_Use") == 0) tp.GetBOOL(&m_PMInfo[EN_PM2].bUse); 
		else if(strcmp(szItem, "PM3_Use") == 0) tp.GetBOOL(&m_PMInfo[EN_PM3].bUse); 
		else if(strcmp(szItem, "PM4_Use") == 0) tp.GetBOOL(&m_PMInfo[EN_PM4].bUse); 
		else if(strcmp(szItem, "PM5_Use") == 0) tp.GetBOOL(&m_PMInfo[EN_PM5].bUse);

		else if(strcmp(szItem, "STD_MODULE_NAME") == 0)
		{
			tp.GetInt(&nNumber);
			if(nNumber<1 || nNumber>MAX_MODULE_NAME_TABLE) break; 
			nIdx = nNumber-1;
			tp.GetWord(g_StdModNameTbl[nIdx].szPreModuleName);
			tp.GetWord(g_StdModNameTbl[nIdx].szStdModuleName);
		}
		else if(strcmp(szItem, "MODULE_RUN_DEF") == 0)
		{
			tp.GetInt(&nNumber);
			if(nNumber<1 || nNumber>MAX_MOD_RUN_DEF_TABLE) break; 
			nIdx = nNumber-1;
			// 2013.07.02
			tp.GetInt(&g_ModuleRunDefTbl[nIdx].nPMIdx);
			for(i=0; tp.GetWord(g_ModuleRunDefTbl[nIdx].aszRunModule[i]); i++) ;
		}
		else
		{
			printf("-----> Invalid Configuration Item[%s] in EventLogServer\n", szItem);
			break;
		}

		// printf("-----> FileName : %s %s\n", m_szRunLogPath, m_szMccLogPath);
	} while(0);
}

// 2013.11.18
void CEventLogServer::ParseModuleInfo(char *szData)
{
	CTextParser tp(szData);
	int nNum, nIdx;

	do {
		if(! tp.GetInt(&nNum)) break;
		nIdx = nNum - 1;
		if(! tp.GetWord(g_ModuleInfo[nIdx].szModule)) break;
		if(! tp.GetInt(&g_ModuleInfo[nIdx].nSlotCount)) break;

		// printf("----->%d %s %d\n", nIdx, g_ModuleInfo[nIdx].szModule, g_ModuleInfo[nIdx].nSlotCount);
	} while(0);

}

//////////////////////////////////////////////////////////////////////
// Member Function(Event Sub Function)

int CEventLogServer::GetLogType(char *szMessage)
{
	int nLogType = LTYPE_USER;
	char szLogType[20];
	CTextParser tp(szMessage);

	do {
		if(! tp.GetWord(szLogType)) break;
		if(strcmp(szLogType, "PRC") == 0) nLogType = LTYPE_PRC;
		else if(strcmp(szLogType, "XFR") == 0) nLogType = LTYPE_XFR;
		else if(strcmp(szLogType, "FNC") == 0) nLogType = LTYPE_FNC;
		else if(strcmp(szLogType, "LEH") == 0) nLogType = LTYPE_LEH;
		else if(strcmp(szLogType, "CFG") == 0) nLogType = LTYPE_CFG;
	} while(0);

	return nLogType;
}

BOOL CEventLogServer::MakeProcLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog)
{
	BOOL bRet = FALSE;
	int nPMIdx;
	char szRecipeName[256];
	char szStepType[80];
	char szData[80];
	char szTemp[512];
	int nRecipeLength;
	CTextParser tp(szMessage);

	_LOG("-----> ProcLog Msg : %s", szMessage);

	do {
		if(! tp.GetWord()) break;
		if(! tp.GetWord(m_ProcessLog.szDeviceID)) break;
		if(! tp.GetWord(m_ProcessLog.szEventID)) break;
		if(! tp.GetWord(m_ProcessLog.szStatus)) break;
		if(! tp.GetInt(&m_ProcessLog.nStepNum)) break;
		if(! tp.GetInt(&nRecipeLength)) break;
		//if(! tp.GetWord(szRecipeName)) break;
		if(nRecipeLength>0)
		{
			tp.GetLine(szTemp);
			strncpy( szRecipeName, szTemp, nRecipeLength );
		}
		else
		{
			if(! tp.GetWord(szRecipeName)) break;
		}
		
		tp.GetWord(szStepType);
		tp.GetString(szData);
		ChangeToStandardModuleName(m_ProcessLog.szDeviceID);
		nPMIdx = GetPMIdx(m_ProcessLog.szDeviceID);

		if(nPMIdx < 0) break;
		if(strcmp(m_ProcessLog.szEventID, "StepProcess") != 0
				&& strcmp(m_ProcessLog.szStatus, "Start") == 0)
		{
			strcpy(m_PMInfo[nPMIdx].szRecipeID, szRecipeName);
			strcpy(m_ProcessLog.szData, "");
			m_PMInfo[nPMIdx].bIsProcRunning = TRUE;
			m_PMInfo[nPMIdx].nRcpType = GetRcpType(m_ProcessLog.szEventID, szRecipeName);
			UpdatePMInfo(nPMIdx);
		}
		else if(strcmp(m_ProcessLog.szEventID, "StepProcess") != 0
				&& strcmp(m_ProcessLog.szStatus, "End") == 0)
		{
			m_PMInfo[nPMIdx].bIsProcRunning = FALSE;
			strcpy(m_ProcessLog.szData, "");
		}
		else if(strcmp(m_ProcessLog.szEventID, "StepProcess") == 0)
		{
			sprintf(m_ProcessLog.szData, "('StepName', '%s')\t('StepType', '%s')", szData, szStepType);
		}

		strcpy(m_ProcessLog.szMaterialID, m_PMInfo[nPMIdx].szWaferInfo);
		strcpy(m_ProcessLog.szSlotNo, m_PMInfo[nPMIdx].szSlotInfo);

		/*
		if(strcmp(m_ProcessLog.szEventID, "PreLotRun") == 0)
		{
			strcpy(m_ProcessLog.szLotID, m_LPMInfo[g_nStartLot].szLotID);
		}
		else
		{
			strcpy(m_ProcessLog.szLotID, m_PMInfo[nPMIdx].szLotID);
		}
		*/

		strcpy(m_ProcessLog.szLotID, m_PMInfo[nPMIdx].szLotID);
		strcpy(m_ProcessLog.szRecipeID, m_PMInfo[nPMIdx].szRecipeID);

		*bIsRunLog = IsPMRunning(nPMIdx);
		//if(RT_IdleRcp == m_PMInfo[nPMIdx].nRcpType) *bIsRunLog = FALSE;		// 2013.07.29

		bRet = TRUE;
	} while(0);

	sprintf(szLog, "'%s'%s'%s'%s'%s'%s'%s'%s%s%s%s%s'%s'%s'%s'%s%d%s%s",
				m_ProcessLog.szDeviceID,	m_szDelimiter,
				m_ProcessLog.szLogType,		m_szDelimiter,
				m_ProcessLog.szEventID,		m_szDelimiter,
				m_ProcessLog.szStatus,		m_szDelimiter,
				m_ProcessLog.szMaterialID,	m_szDelimiter,
				m_ProcessLog.szSlotNo,		m_szDelimiter,
				m_ProcessLog.szLotID,		m_szDelimiter,
				m_ProcessLog.szRecipeID,	m_szDelimiter,
				m_ProcessLog.nStepNum,		m_szDelimiter,
				m_ProcessLog.szData);
	return bRet;
}


void CEventLogServer::SetLLWaferInfo(char *szModule, int nSlot, BOOL bDualArm)
{
	int i;
	int anLPMIdx[2] = { -1, -1 };
	int anWaferNo[2] = { 0, 0 };
	int nLPMIdx = -1;
	char szTempWaferInfo[40] = "$";			

	if(strcmp(szModule, "LL1") == 0 || strcmp(szModule, "LL2") == 0)
	{
		anLPMIdx[0] = GetWaferSrcIdx(szModule, nSlot, &anWaferNo[0]);
		if(bDualArm) anLPMIdx[1] = GetWaferSrcIdx(szModule, nSlot+1, &anWaferNo[1]);
	}

	for(i=0; i < MAX_LLSLOT_INFO; i++)
	{
		if(strcmp(szModule, g_LLWaferInfo[i].szModule) == 0)
		{
			if(g_LLWaferInfo[i].nSlot == nSlot)
			{
				if(anLPMIdx[0] >= 0 && anLPMIdx[0] < MAX_LPM && anWaferNo[0] > 0)			
				{
					sprintf(szTempWaferInfo, "'C%c:%d'", 'A'+anLPMIdx[0], anWaferNo[0]);
					nLPMIdx = anLPMIdx[0];
				}				
				strcpy(g_LLWaferInfo[i].szWaferInfo, szTempWaferInfo);				
			}

			if(bDualArm && g_LLWaferInfo[i].nSlot == nSlot+1)
			{
				//g_LLWaferInfo[i].nSlot = nSlot+1;
				
				if(anLPMIdx[1] >= 0 && anLPMIdx[1] < MAX_LPM && anWaferNo[1] > 0)			
				{
					sprintf(szTempWaferInfo, "'C%c:%d'", 'A'+anLPMIdx[1], anWaferNo[1]);
					nLPMIdx = anLPMIdx[0];
				}				
				strcpy(g_LLWaferInfo[i].szWaferInfo, szTempWaferInfo);
				break;
			}
		}		
	}

	for(i=0 ; i <MAX_LLSLOT_INFO ; i++)
	{
		_LOG("SetLLWaferInfo : %s, %d, %s", g_LLWaferInfo[i].szModule, g_LLWaferInfo[i].nSlot, g_LLWaferInfo[i].szWaferInfo);
	}			
}

void CEventLogServer::GetLLWaferInfo(char *szWaferInfo, char *szModule, int nSlot, BOOL bDualArm)
{
	int i;
	int anLPMIdx[2] = { -1, -1 };
	int anWaferNo[2] = { 0, 0 };
	int nLPMIdx = -1;
	char szTempWaferInfo[40] = "\0";
	char szTempWaferInfo2[40] = "\0";
	char szTemp[80] = "\0";

	for(i=0; i < MAX_LLSLOT_INFO; i++)
	{
		if(strcmp(szModule, g_LLWaferInfo[i].szModule) == 0)
		{
			if(g_LLWaferInfo[i].nSlot == nSlot)
			{
				if(strcmp(g_LLWaferInfo[i].szWaferInfo, "") == 0 || strcmp(g_LLWaferInfo[i].szWaferInfo, "$") == 0)	
					strcpy(szTempWaferInfo, "$");
				else 					
					strcpy(szTempWaferInfo, g_LLWaferInfo[i].szWaferInfo);							
			}
			
			if(bDualArm && g_LLWaferInfo[i].nSlot == nSlot+1)
			{
				if(strcmp(g_LLWaferInfo[i].szWaferInfo, "") == 0 || strcmp(g_LLWaferInfo[i].szWaferInfo, "$") == 0)	
					strcpy(szTempWaferInfo2, "$");
				else 					
					strcpy(szTempWaferInfo2, g_LLWaferInfo[i].szWaferInfo);					

				break;
			}
		}		
	}

	if(bDualArm)
	{
		sprintf(szTemp, "[2,'%s','%s']", szTempWaferInfo, szTempWaferInfo2);
		strcpy(szWaferInfo, szTemp);
	}
	else
	{
		strcpy(szWaferInfo, szTempWaferInfo);		
	}
}


void CEventLogServer::RemoveLLWaferInfo(char *szModule, int nSlot, BOOL bDualArm)
{
	int i;
	int anLPMIdx[2] = { -1, -1 };
	int anWaferNo[2] = { 0, 0 };
	int nLPMIdx = -1;
	char szTempWaferInfo[40] = "\0";		
	
	for(i=0; i < MAX_LLSLOT_INFO; i++)
	{
		if(strcmp(szModule, g_LLWaferInfo[i].szModule) == 0)
		{
			if(g_LLWaferInfo[i].nSlot == nSlot)
			{	
				strcpy(g_LLWaferInfo[i].szWaferInfo, szTempWaferInfo);				
			}
			
			if(bDualArm && g_LLWaferInfo[i].nSlot == nSlot+1)
			{
				strcpy(g_LLWaferInfo[i].szWaferInfo, szTempWaferInfo);
				break;
			}
		}		
	}
	
	for(i=0 ; i <MAX_LLSLOT_INFO ; i++)
	{
		_LOG("RemoveLLWaferInfo : %s, %d, %s", g_LLWaferInfo[i].szModule, g_LLWaferInfo[i].nSlot, g_LLWaferInfo[i].szWaferInfo);
	}			
}

//Pick, Place했을때에 모듈 위치 및 Slot 번호 저장
//Dual Arm일 경우, Low/Upper가 각기 다르게 동작하지 않기 때문에 Pick or Place 한 시점이 Low/Upper의 이전 동작 위치임.
//이전동작하고 다음 동작의 Station 위치 정보를 알아내어 Tack Time 산출 목적
void CEventLogServer::SetPreModuleInfo(char *szModule, char *szFromDevice, char *szPreSlot, char *szEndEffect)
{
	int i;
	
	for(i=0; i < MAX_ROBOT_INFO; i++)
	{
		if(strcmp(szModule, g_PreModuleInfo[i].szModule) == 0)
		{
			strcpy(g_PreModuleInfo[i].szPreModule, szFromDevice);
			strcpy(g_PreModuleInfo[i].szPreModuleSlot, szPreSlot);
			_LOG("SetpreModuleInfo : %s, %s, %s", g_PreModuleInfo[i].szModule, g_PreModuleInfo[i].szPreModule,  g_PreModuleInfo[i].szPreModuleSlot);
			break;
		}

	}
}

//마지막에 동작했던 정보 리딩 
void CEventLogServer::GetPreModuleInfo(char *szModule, char *szPreModuleInfo, char *szEndEffect, BOOL bDualArm)
{
	int i;
	char szSlot2[10];
	
	for(i=0; i < MAX_ROBOT_INFO; i++)
	{
		if(strcmp(szModule, g_PreModuleInfo[i].szModule) == 0)
		{

			//첫 장은 해당 정보가 없으므로 Null 처리
			if ( atoi(g_PreModuleInfo[i].szPreModuleSlot) == 0 )
			{
				sprintf( g_PreModuleInfo[i].szPreModuleSlot, "%s", "''");
				sprintf( szSlot2, "%s", "''");
			}
			else
			{
				//ATM Dual Arm이 LL 2,4번에서 Pick 할 경우 이전 Slot이 2,3 or 4,5로 로깅되는 문제 수정 >> 2,2 4,4로 로깅되도록...
				if(   strncmp(szModule, "ATM", 3) == 0 && 
					( strncmp(g_PreModuleInfo[i].szPreModule, "LL", 2) == 0 || 
					( strncmp(g_PreModuleInfo[i].szPreModule, "Port", 4) == 0 &&  atoi(g_PreModuleInfo[i].szPreModuleSlot) == 25) ) ) ///마지막 Slot에 LP 25번 인 경우 25,25로 Logging 되도록..

				{
					sprintf( szSlot2, "%d", atoi(g_PreModuleInfo[i].szPreModuleSlot));
				}
				else
				{
					sprintf( szSlot2, "%d", atoi(g_PreModuleInfo[i].szPreModuleSlot)+1);
				}
			}
			
			if( bDualArm ) sprintf(szPreModuleInfo, "('WaferArmPositionValue', [3,'%s',%s,%s])", g_PreModuleInfo[i].szPreModule,
				g_PreModuleInfo[i].szPreModuleSlot, szSlot2);
			else sprintf(szPreModuleInfo, "('WaferArmPositionValue', [2,'%s',%s])", g_PreModuleInfo[i].szPreModule, g_PreModuleInfo[i].szPreModuleSlot);

			_LOG("GetPreModuleInfo %s, %s, %s ", g_PreModuleInfo[i].szModule, g_PreModuleInfo[i].szPreModule, g_PreModuleInfo[i].szPreModuleSlot);
			
			break;
		}
	}
	//_LOG("#[GET] %d, %s, %s, %s, %s, %d ", nOffset, szEndEffect, g_PreModuleInfo[i+1].szModule, g_PreModuleInfo[i+nOffset].szPreModule, g_PreModuleInfo[i+nOffset].szPreModuleSlot, atoi(g_PreModuleInfo[i+nOffset].szPreModuleSlot)+1);
}

BOOL CEventLogServer::MakeTransLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog)
{
	BOOL bRet = FALSE;
	int nActType;
	char szArm[20];
	CTextParser tp(szMessage);
	BOOL bDualArm = FALSE;
	int nSlot;

	char szPreSlot[10];
	char szAddData[20];
	char szPreModuleInfo[50];

	_LOG("-----> Trans Msg : %s", szMessage);
	do {
		if(! tp.GetWord()) break;
		if(! tp.GetWord(m_TransferLog.szDeviceID)) break;
		if(! tp.GetWord(m_TransferLog.szEventID)) break;
		if(! tp.GetWord(m_TransferLog.szStatus)) break;
		ChangeToStandardModuleName(m_TransferLog.szDeviceID);
		nActType = GetActType(m_TransferLog.szEventID);

		if(ATYPE_GET == nActType)
		{
			if(! tp.GetWord(m_TransferLog.szFromDevice)) break;
			if(! tp.GetWord(m_TransferLog.szFromSlot)) break;				// 2013.07.29
			if(! tp.GetWord(szArm)) break;

			if(strcmp(m_TransferLog.szDeviceID, "ATM1") == 0 || strcmp(m_TransferLog.szDeviceID, "ATM2") == 0)
			{
				if(! tp.GetWord(szPreSlot)) break;
				if(! tp.GetWord(szAddData)) break;
			}
			/*
			else if(strcmp(m_TransferLog.szDeviceID, "TM1") == 0 || strcmp(m_TransferLog.szDeviceID, "TM2") == 0)
			{
				strcpy(szPreSlot, m_TransferLog.szFromSlot);
			}
			*/

			ChangeToStandardModuleName(m_TransferLog.szFromDevice);
			strcpy(m_TransferLog.szToDevice, m_TransferLog.szDeviceID);

			if(strcmp(m_TransferLog.szStatus, "Start") == 0)
			{
				nSlot = GetSlotInfo(m_TransferLog.szFromSlot, &bDualArm);
				if(strncmp(m_TransferLog.szDeviceID, "ATM", 3) == 0 && strncmp(m_TransferLog.szFromDevice, "LL", 2) == 0)
				{
					GetLLWaferInfo(m_TransferLog.szMaterialID, m_TransferLog.szFromDevice, nSlot, bDualArm);
				}
				else
				{
					GetWaferInfo(m_TransferLog.szMaterialID, m_TransferLog.szFromDevice, nSlot, bDualArm);
				}
				BackupModuleWaferInfo(m_TransferLog.szFromDevice, m_TransferLog.szMaterialID);	// 2013.11.18
				GetArmInfo(m_TransferLog.szDeviceID, m_TransferLog.szToSlot, szArm, bDualArm);	// 2013.11.18
				GetPreModuleInfo(m_TransferLog.szDeviceID, szPreModuleInfo, szArm, bDualArm);   // 마지막으로 동작했던 정보 읽어옴	
			}
			// 2013.11.18
			else
			{
				nSlot = GetSlotInfo(m_TransferLog.szFromSlot, &bDualArm);
				sprintf(szPreSlot, "%d", nSlot);
				
				GetArmInfo(m_TransferLog.szDeviceID, m_TransferLog.szToSlot, szArm, bDualArm);
				if(strncmp(m_TransferLog.szDeviceID, "ATM", 3) == 0 && strncmp(m_TransferLog.szFromDevice, "LL", 2) == 0)
				{
					GetLLWaferInfo(m_TransferLog.szMaterialID, m_TransferLog.szFromDevice, nSlot, bDualArm);
				}
				else
				{
					GetWaferInfoFromModuleInfo(m_TransferLog.szFromDevice, m_TransferLog.szMaterialID);	
				}

				//GetWaferInfoFromModuleInfo(m_TransferLog.szFromDevice, m_TransferLog.szMaterialID);	      // 2013.11.18
				SetPreModuleInfo(m_TransferLog.szDeviceID, m_TransferLog.szFromDevice, szPreSlot, szArm); // 마지막 동작 읽어오고 현재 동작 저장
				GetPreModuleInfo(m_TransferLog.szDeviceID, szPreModuleInfo, szArm, bDualArm);             // 마지막으로 동작했던 정보 읽어옴
				RemoveLLWaferInfo(m_TransferLog.szFromDevice, nSlot, bDualArm);							  // 2014.07.23 ATM Robot LoadLock Pick진행시 2번 Slot부터 진행하는 이유로
																										  // Wafer Info가 맞지 않아 LoadLock Wafer Info 개별 추가				
			}

			strcpy(m_TransferLog.szMaterialType, "'Wafer'");	
			
			if (strcmp(m_TransferLog.szDeviceID, "ATM1") == 0 || strcmp(m_TransferLog.szDeviceID, "ATM2") == 0)
			{
				sprintf(m_TransferLog.szData, "%s\t('WaferSensingValue', '%s')", szPreModuleInfo, szAddData);
			}
			else
			{
				sprintf(m_TransferLog.szData, "%s", szPreModuleInfo);
			}
		}
		else if(ATYPE_PUT == nActType)
		{
			if(! tp.GetWord(m_TransferLog.szToDevice)) break;
			if(! tp.GetWord(m_TransferLog.szToSlot)) break;					// 2013.07.29
			if(! tp.GetWord(szArm)) break;
			
			if(strcmp(m_TransferLog.szDeviceID, "ATM1") == 0 || strcmp(m_TransferLog.szDeviceID, "ATM2") == 0)
			{
				if(! tp.GetWord(szPreSlot)) break;
				if(! tp.GetWord(szAddData)) break;
			}
			/*
			else if(strcmp(m_TransferLog.szDeviceID, "TM1") == 0 || strcmp(m_TransferLog.szDeviceID, "TM2") == 0)
			{
				strcpy(szPreSlot, m_TransferLog.szToSlot);
			}
			*/

			ChangeToStandardModuleName(m_TransferLog.szToDevice);
			strcpy(m_TransferLog.szFromDevice, m_TransferLog.szDeviceID);

			if(strcmp(m_TransferLog.szStatus, "Start") == 0)
			{
				GetSlotInfo(m_TransferLog.szToSlot, &bDualArm);
				nSlot = szArm[0]-'A'+1;
				GetWaferInfo(m_TransferLog.szMaterialID, m_TransferLog.szDeviceID, nSlot, bDualArm);
				BackupModuleWaferInfo(m_TransferLog.szDeviceID, m_TransferLog.szMaterialID);		// 2013.11.18
				GetArmInfo(m_TransferLog.szDeviceID, m_TransferLog.szFromSlot, szArm, bDualArm);	// 2013.11.18
				GetPreModuleInfo(m_TransferLog.szDeviceID, szPreModuleInfo, szArm, bDualArm);				
			}
			// 2013.11.18
			else
			{
				nSlot = GetSlotInfo(m_TransferLog.szToSlot, &bDualArm);
				sprintf(szPreSlot, "%d", nSlot);
				GetArmInfo(m_TransferLog.szDeviceID, m_TransferLog.szFromSlot, szArm, bDualArm);
				GetWaferInfoFromModuleInfo(m_TransferLog.szDeviceID, m_TransferLog.szMaterialID);	// 2013.11.18
				SetPreModuleInfo(m_TransferLog.szDeviceID, m_TransferLog.szToDevice, szPreSlot, szArm);
				GetPreModuleInfo(m_TransferLog.szDeviceID, szPreModuleInfo, szArm, bDualArm); 
				SetLLWaferInfo(m_TransferLog.szToDevice, nSlot, bDualArm);		// 2014.07.23 ATM Robot LoadLock Pick진행시 2번 Slot부터 진행하는 이유로
																				// Wafer Info가 맞지 않아 LoadLock Wafer Info 개별 추가
			}
			strcpy(m_TransferLog.szMaterialType, "'Wafer'");

			if (strcmp(m_TransferLog.szDeviceID, "ATM1") == 0 || strcmp(m_TransferLog.szDeviceID, "ATM2") == 0)
			{
				sprintf(m_TransferLog.szData, "%s\t('WaferSensingValue', '%s')", szPreModuleInfo, szAddData);
			}
			else
			{
				sprintf(m_TransferLog.szData, "%s", szPreModuleInfo);
			}
		}
		else if(ATYPE_MOVE == nActType)
		{
			if(! tp.GetWord(m_TransferLog.szFromDevice)) break;
			if(! tp.GetWord(m_TransferLog.szFromSlot)) break;				// 2013.07.29

			if(! tp.GetWord(m_TransferLog.szToDevice)) break;
			if(! tp.GetWord(m_TransferLog.szToSlot)) break;

			if(! tp.GetWord(szArm)) break;

			ChangeToStandardModuleName(m_TransferLog.szFromDevice);
			ChangeToStandardModuleName(m_TransferLog.szToDevice);
			if(strcmp(m_TransferLog.szStatus, "Start") == 0)
			{
				nSlot = GetSlotInfo(m_TransferLog.szFromSlot, &bDualArm);
				GetWaferInfo(m_TransferLog.szMaterialID, m_TransferLog.szFromDevice, nSlot, bDualArm);
			}
			strcpy(m_TransferLog.szMaterialType, "'Wafer'");
		}
		// 2013.07.30
		else if(ATYPE_ROT == nActType)
		{
			if(! tp.GetWord(m_TransferLog.szToDevice)) break;
			strcpy(m_TransferLog.szToSlot, "$");
			ChangeToStandardModuleName(m_TransferLog.szToDevice);

			strcpy(m_TransferLog.szFromDevice, "");
			strcpy(m_TransferLog.szFromSlot, "$");

			if(! tp.GetWord(szArm)) break;
			if(strcmp(m_TransferLog.szStatus, "Start") == 0)
			{
				GetWaferInfo(m_TransferLog.szMaterialID, m_TransferLog.szDeviceID, 1, TRUE);
				if(strlen(m_TransferLog.szMaterialID) > 1) strcpy(m_TransferLog.szMaterialType, "'Wafer'");
				else strcpy(m_TransferLog.szMaterialType, "$");
			}
		}
		else if(ATYPE_READY == nActType)
		{
			if(! tp.GetWord(m_TransferLog.szToDevice)) break;
			strcpy(m_TransferLog.szToSlot, "$");
			ChangeToStandardModuleName(m_TransferLog.szToDevice);

			strcpy(m_TransferLog.szFromDevice, "");
			strcpy(m_TransferLog.szFromSlot, "$");

			if(! tp.GetWord(szArm)) break;
			strcpy(m_TransferLog.szMaterialType, "$");
			strcpy(m_TransferLog.szMaterialID, "$");
		}
		else break;

		//첫장 정보가 짤리는 문제로 수정 (PM Run상태 체크가 아닌 LPM Run 상태 체크)
		//*bIsRunLog = IsDevRunning(m_TransferLog.szDeviceID);
		*bIsRunLog = IsLPMRunning();
		
		bRet = TRUE;
	} while(0);

	sprintf(szLog, "'%s'%s'%s'%s'%s'%s'%s'%s%s%s%s%s'%s'%s%s%s'%s'%s%s%s%s",
				m_TransferLog.szDeviceID,		m_szDelimiter,
				m_TransferLog.szLogType,		m_szDelimiter,
				m_TransferLog.szEventID,		m_szDelimiter,
				m_TransferLog.szStatus,			m_szDelimiter,
				m_TransferLog.szMaterialID,		m_szDelimiter,
				m_TransferLog.szMaterialType,	m_szDelimiter,
				m_TransferLog.szFromDevice,		m_szDelimiter,
				m_TransferLog.szFromSlot,		m_szDelimiter,
				m_TransferLog.szToDevice,		m_szDelimiter,
				m_TransferLog.szToSlot,			m_szDelimiter,
				m_TransferLog.szData);
	return bRet;
}

BOOL CEventLogServer::MakeFuncLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog)
{
	BOOL bRet = FALSE;
	int nSlotCount;
	int nPMIdx;
	char szMID[50];
	char szMAP[50];
	char szRF[20];
	char szRFPower[20];

	CTextParser tp(szMessage);

	do {

		_LOG("-----> FuncLog Msg : %s", szMessage);

		if(! tp.GetWord()) break;
		if(! tp.GetWord(m_FunctionLog.szDeviceID)) break;
		if(! tp.GetWord(m_FunctionLog.szEventID)) break;
		if(! tp.GetWord(m_FunctionLog.szStatus)) break;

		if(strcmp(m_FunctionLog.szEventID, "LPLoad") == 0 || strcmp(m_FunctionLog.szEventID, "LPUnload") == 0 || strcmp(m_FunctionLog.szEventID, "Map") == 0)
		{
			if(! tp.GetWord(szMID)) strcpy(szMID, "");		
			if(! tp.GetWord(szMAP)) strcpy(szMAP, "");		
		}
		else if(strcmp(m_FunctionLog.szEventID, "RfPowerOnOff") == 0 || strcmp(m_FunctionLog.szEventID, "Rf1PowerOnOff") == 0 || strcmp(m_FunctionLog.szEventID, "Rf2PowerOnOff") == 0 )
		{
			if(! tp.GetWord(szRF)) strcpy(szRF, "$");	
			if(! tp.GetWord(szRFPower)) strcpy(szRFPower, "$");			
		}

		if(! tp.GetString(m_FunctionLog.szData)) strcpy(m_FunctionLog.szData, "");		// 2013.07.16
		ChangeToStandardModuleName(m_FunctionLog.szDeviceID);

		strcpy(m_FunctionLog.szMaterialID, "$");
		strcpy(m_FunctionLog.szMaterialType, "$");

		*bIsRunLog = IsLPMRunning();

		// 2013.06.21
		if(strcmp(m_FunctionLog.szDeviceID, "Aligner") == 0)
		{
			GetWaferInfo(m_FunctionLog.szMaterialID, m_FunctionLog.szDeviceID, 1, FALSE);
			strcpy(m_FunctionLog.szMaterialType, "'Wafer'");
		}
		// 2013.07.16
		else if(strncmp(m_FunctionLog.szDeviceID, "LL", 2) == 0)
		{
			/* if(strcmp(m_FunctionLog.szEventID, "Pumping") == 0 
					|| strcmp(m_FunctionLog.szEventID, "Venting")==0
					) */
			{
				GetModuleInfo(m_FunctionLog.szDeviceID, &nSlotCount);
				GetWaferInfoFromBM(m_FunctionLog.szMaterialID, m_FunctionLog.szDeviceID, nSlotCount);
				if(strlen(m_FunctionLog.szMaterialID)>2) strcpy(m_FunctionLog.szMaterialType, "'Wafer'");
			}
		}
		else if(strcmp(m_FunctionLog.szEventID, "LPLoad") == 0 || strcmp(m_FunctionLog.szEventID, "LPUnload") == 0)
		{
			*bIsRunLog = TRUE;// LP Load/Unload/Map시점에 LP Running 상태가 아니라서 항상 Log 남기도록 추가.
			
			sprintf(m_FunctionLog.szMaterialID, "'%s'", szMID);
			strcpy(m_FunctionLog.szMaterialType, "'FOUP'");
			sprintf(m_FunctionLog.szData, "");
		}
		else if(strcmp(m_FunctionLog.szEventID, "Map") == 0)
		{
			*bIsRunLog = TRUE;// LP Load/Unload/Map시점에 LP Running 상태가 아니라서 항상 Log 남기도록 추가.
			
			sprintf(m_FunctionLog.szMaterialID, "'%s'", szMID);
			strcpy(m_FunctionLog.szMaterialType, "'FOUP'");
			sprintf(m_FunctionLog.szData, "('MapID', '%s')\t('MapType', 'LPType')", szMAP);
		}
		else if(strcmp(m_FunctionLog.szEventID, "RemoteStatus") == 0 || strcmp(m_FunctionLog.szEventID, "ModuleStatus")  == 0 ||
		    	strcmp(m_FunctionLog.szEventID, "FoupSensor")   == 0 )
		{
			*bIsRunLog = TRUE;//Communication,FoupSensor 상태는 항상 보고되도록.., 
		}
		else if(strncmp(m_FunctionLog.szDeviceID, "PM", 2) == 0)
		{
			*bIsRunLog = IsDevRunning(m_FunctionLog.szDeviceID); //PM Run Status Check

			nPMIdx = GetPMIdx(m_FunctionLog.szDeviceID);
			UpdatePMInfo(nPMIdx);
			strcpy(m_FunctionLog.szMaterialID, m_PMInfo[nPMIdx].szWaferInfo);
			if(strlen(m_FunctionLog.szMaterialID)>2) strcpy(m_FunctionLog.szMaterialType, "'Wafer'");

			if(strcmp(m_FunctionLog.szEventID, "RfPowerOnOff") == 0)	sprintf(m_FunctionLog.szData, "('%s',%s)", szRF,szRFPower);
			//if(strcmp(m_FunctionLog.szEventID, "Rpg1PowerOnOff") == 0 || strcmp(m_FunctionLog.szEventID, "Rpg2PowerOnOff") == 0) sprintf(m_FunctionLog.szData, "('%s',%s)", szRF,szRFPower);
		}

		if(strcmp(m_FunctionLog.szEventID, "AutoLeakCheck") == 0)
		{
			*bIsRunLog = TRUE; //2014.07.20 Auto Leak Check Status
		}
		bRet = TRUE;
	} while(0);

	sprintf(szLog, "'%s'%s'%s'%s'%s'%s'%s'%s%s%s%s%s%s",
				m_FunctionLog.szDeviceID,		m_szDelimiter,
				m_FunctionLog.szLogType,		m_szDelimiter,
				m_FunctionLog.szEventID,		m_szDelimiter,
				m_FunctionLog.szStatus,			m_szDelimiter,
				m_FunctionLog.szMaterialID,		m_szDelimiter,
				m_FunctionLog.szMaterialType,	m_szDelimiter,
				m_FunctionLog.szData);
	return bRet;
}

BOOL CEventLogServer::MakeLotLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog)
{
	BOOL bRet = FALSE;
	int nLPMIdx;
	CTextParser tp(szMessage);
	do {

		_LOG("MakeLotLogStr : %s", szMessage);

		if(! tp.GetWord()) break;
		if(! tp.GetWord(m_LotEventLog.szDeviceID)) break;
		if(! tp.GetWord(m_LotEventLog.szEventID)) break;

		ChangeToStandardModuleName(m_LotEventLog.szDeviceID);
		nLPMIdx = GetLPMIdx(m_LotEventLog.szDeviceID);
		
		_LOG("MakeLotLogStr : %s %d", m_LotEventLog.szDeviceID, nLPMIdx);

		if(nLPMIdx < 0) break;
		if(strcmp(m_LotEventLog.szEventID, "CarrierLoad") == 0)
		{
			UpdateLPMInfo(nLPMIdx);
		}
		// 2013.07.16
		else if(strcmp(m_LotEventLog.szEventID, "ProcessJobStart") == 0)
		{
			UpdateLPMInfo(nLPMIdx);
			g_nStartLot = nLPMIdx;
			tp.GetWord(m_LPMInfo[nLPMIdx].szFlowInfo);
		}

#ifdef _DEBUG	
		for(int i=0; i<MAX_LPM;i++)
		{			
			_LOG("m_LPMInfo 1 : %s %s %s %s", m_LPMInfo[i].szLotID, m_LPMInfo[i].szFlowRcpID, 
			m_LPMInfo[i].szFlowInfo, m_LPMInfo[i].szCarrierID);
		}
#endif

		strcpy(m_LotEventLog.szLotID, m_LPMInfo[nLPMIdx].szLotID);			// 2013.07.30
		strcpy(m_LotEventLog.szFlowRcpID, m_LPMInfo[nLPMIdx].szFlowRcpID);
		strcpy(m_LotEventLog.szFlowInfo, m_LPMInfo[nLPMIdx].szFlowInfo);
		strcpy(m_LotEventLog.szCarrierID, m_LPMInfo[nLPMIdx].szCarrierID);

		strcpy(m_LotEventLog.szData, "");
		// *bIsRunLog = IsLPMRunning(nLPMIdx);
		*bIsRunLog = TRUE;
		bRet = TRUE;
	} while(0);

	sprintf(szLog, "'%s'%s'%s'%s'%s'%s'%s'%s'%s'%s%s%s'%s'%s%s",
				m_LotEventLog.szDeviceID,	m_szDelimiter,
				m_LotEventLog.szLogType,	m_szDelimiter,
				m_LotEventLog.szEventID,	m_szDelimiter,
				m_LotEventLog.szLotID,		m_szDelimiter,					// 2013.07.30
				m_LotEventLog.szFlowRcpID,	m_szDelimiter,
				m_LotEventLog.szFlowInfo,	m_szDelimiter,
				m_LotEventLog.szCarrierID,	m_szDelimiter,
				m_LotEventLog.szData);

	// 2014.07.18
	if(strcmp(m_LotEventLog.szEventID, "CarrierUnload") == 0) ClearLPMInfo(nLPMIdx);

	return bRet;
}

BOOL CEventLogServer::MakeCfgLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog)
{
	char szDataKey[100];
	char szData1[100];
	char szData2[100];
	char szData3[100];
	BOOL bRet = FALSE;

	CTextParser tp(szMessage);
	do {
		if(! tp.GetWord()) break;
		if(! tp.GetWord(m_CfgEventLog.szDeviceID)) break;
		if(! tp.GetWord(m_CfgEventLog.szEventID)) break;
		if(! tp.GetWord(szDataKey)) break;
		tp.GetWord(szData1);
		tp.GetWord(szData2);
		tp.GetWord(szData3);
		
		ChangeToStandardModuleName(m_CfgEventLog.szDeviceID);


		if(strcmp(m_CfgEventLog.szEventID, "SWInfo") == 0 && strcmp(szDataKey, "Version") == 0)
		{
			sprintf(m_CfgEventLog.szData, "('%s','%s')", szDataKey, szData1);
		}
		else if(strcmp(m_CfgEventLog.szEventID, "SWInfo") == 0 && strcmp(szDataKey, "Updated") == 0)
		{
			sprintf(m_CfgEventLog.szData, "('%s','%s')", szDataKey, szData1);
		}
		else if(strcmp(m_CfgEventLog.szEventID, "ChamberInfo") == 0)
		{
			sprintf(m_CfgEventLog.szData, "('%s','%s')", szDataKey, szData1);
		}
		else if(strcmp(m_CfgEventLog.szEventID, "LL1Option") == 0)
		{
			sprintf(m_CfgEventLog.szData, "('%s','%s')", szDataKey, szData1);
		}
		else if(strcmp(m_CfgEventLog.szEventID, "LL2Option") == 0)
		{
			sprintf(m_CfgEventLog.szData, "('%s','%s')", szDataKey, szData1);
		}
		else if(strcmp(m_CfgEventLog.szEventID, "PartMaint") == 0)
		{
			sprintf(m_CfgEventLog.szData, "('%s',%s)\t('AlarmSet',%s)\t('CurrentCount',%s)", szDataKey,szData1,szData2,szData3);
		}
		
		//printf("#### %s %s %s %s %s\n",m_CfgEventLog.szDeviceID, m_CfgEventLog.szEventID, szDataKey,szData1,m_CfgEventLog.szData);

		*bIsRunLog = TRUE;
		bRet = TRUE;		
	} while(0);

	sprintf(szLog, "'%s'%s'%s'%s'%s'%s%s",
		m_CfgEventLog.szDeviceID,	m_szDelimiter,
		m_CfgEventLog.szLogType,	m_szDelimiter,
		m_CfgEventLog.szEventID,	m_szDelimiter,
		m_CfgEventLog.szData);
	return bRet;
}


BOOL CEventLogServer::MakeUserLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog)
{
	strcpy(szLog, szMessage);
	return TRUE;
}

// 2013.07.16
void CEventLogServer::ConvertToMARSStd(char *szData)
{
	char szBuf[LOG_MAX_SIZE];
	char *pSrc, *pDesc;

	// Change '' to $
	pSrc = szData;
	pDesc = szBuf;
	for(pSrc=szData; *pSrc!=NULL; pSrc++)
	{
		if(0 == strncmp(pSrc, "''", 2))
		{
			*pDesc = '$'; pSrc++;
		}
		else if(0 == strncmp(pSrc, "' '", 2))
		{
			*pDesc = '$'; pSrc+=2;
		}
		else
		{
			*pDesc = *pSrc;
		}
		pDesc++;
	}
	*pDesc = NULL;
	strcpy(szData, szBuf);
}

//////////////////////////////////////////////////////////////////////
// Member Function(Event Sub Function2)

int CEventLogServer::GetPMIdx(char *szDeviceID)
{
	int i;
	for(i=0; i<MAX_PM; i++)
	{
		if(! m_PMInfo[i].bUse) continue;
		if(strcmp(m_PMInfo[i].szDeviceID, szDeviceID) == 0) break;
	}
	if(i >= MAX_PM) return -1;
	return i;
}

int CEventLogServer::GetLPMIdx(char *szDeviceID)
{
	int i;
	for(i=0; i<MAX_LPM; i++)
	{
		if(! m_LPMInfo[i].bUse) continue;
		if(strcmp(m_LPMInfo[i].szDeviceID, szDeviceID) == 0) break;
	}
	if(i >= MAX_LPM) return -1;
	return i;
}

int CEventLogServer::GetWaferInfoFromPM(char *szWaferInfo, int nPMIdx)
{
	int i;
	int nWaferSrcIdx;
	int nRet = -1;
	int nWaferNo;
	char szTemp[40];
	char szTempWaferInfo[80] = "";
	int nCount = 0;
	int nMaxCount = 0;

	// 2013.07.16
	sprintf(szTemp, "PM%d", nPMIdx+1);
	GetModuleInfo(szTemp, &nMaxCount);

	strcpy(szWaferInfo, "");
	for(i=0; i<nMaxCount; i++)
	{
		nWaferSrcIdx = GetWaferSrcIdxFromPM(nPMIdx, i+1, &nWaferNo);
		if(nWaferSrcIdx < MAX_LPM && nWaferSrcIdx >= 0)
		{
			if(nRet < 0) nRet = nWaferSrcIdx;
			sprintf(szTemp, "'C%c:%d'", 'A'+nWaferSrcIdx, nWaferNo);
			if(*szTempWaferInfo != NULL) strcat(szTempWaferInfo, ",");
			strcat(szTempWaferInfo, szTemp);
			nCount++;
		}
	}

	if(nCount == 0) strcpy(szWaferInfo, "$");
	else if(nCount == 1) strcpy(szWaferInfo, szTempWaferInfo);
	else sprintf(szWaferInfo, "[%d,%s]", nCount, szTempWaferInfo);
	return nRet;
}

// 2013.07.16
int CEventLogServer::GetWaferInfoFromBM(char *szWaferInfo, char *szModule, int nWaferCount)
{
	int i;
	int nWaferSrcIdx;
	int nRet = -1;
	int nWaferNo;
	char szTemp[40];
	char szTempWaferInfo[80] = "";
	int nCount = 0;

	strcpy(szWaferInfo, "");
	for(i=0; i<nWaferCount; i++)
	{
		nWaferSrcIdx = GetWaferSrcIdx(szModule, i+1, &nWaferNo);
		if(nWaferSrcIdx < MAX_LPM && nWaferSrcIdx >= 0)
		{
			if(nRet < 0) nRet = nWaferSrcIdx;
			sprintf(szTemp, "'C%c:%d'", 'A'+nWaferSrcIdx, nWaferNo);
			if(*szTempWaferInfo != NULL) strcat(szTempWaferInfo, ",");
			strcat(szTempWaferInfo, szTemp);
			nCount++;
		}
	}

	if(nCount == 0) strcpy(szWaferInfo, "$");
	else if(nCount == 1) strcpy(szWaferInfo, szTempWaferInfo);
	else sprintf(szWaferInfo, "[%d,%s]", nCount, szTempWaferInfo);
	return nRet;
}

void CEventLogServer::GetArmInfo(char *szModule, char *szArmInfo, char *szArm, BOOL bDualArm)
{
	int nArmNo;
	int nArmAmount = 2;

	GetModuleInfo(szModule, &nArmAmount);		// 2013.11.18

	if(2 == nArmAmount)
	{
		nArmNo = (*szArm) - 'A' + 1;
		if(FALSE == bDualArm)
		{
			sprintf(szArmInfo, "'EndEffector%d'", nArmNo);
		}
		else
		{
			sprintf(szArmInfo, "[2,'EndEffector%d','EndEffector%d']", nArmNo, nArmNo+1);
		}
	}
	// 2013.11.18
	else if(4 == nArmAmount)
	{
		nArmNo = (*szArm) - 'A' + 1;
		if(FALSE == bDualArm)
		{
			sprintf(szArmInfo, "'EndEffector%d'", nArmNo*2-1);
		}
		else
		{
			sprintf(szArmInfo, "[2,'EndEffector%d','EndEffector%d']", nArmNo*2-1, nArmNo*2);
		}
	}
}

BOOL CEventLogServer::IsPMRunning(int nPMIdx)
{
	if(PMC_CTCInUse == m_PMInfo[nPMIdx].nCommStatus
			|| PMC_CTCDis == m_PMInfo[nPMIdx].nCommStatus
			|| PMC_CTCDis2 == m_PMInfo[nPMIdx].nCommStatus) return TRUE;

	// 2013.07.29
	if(RT_LotPreRcp == m_PMInfo[nPMIdx].nRcpType || RT_LotPostRcp == m_PMInfo[nPMIdx].nRcpType)
	{
		if(PMC_Standby == m_PMInfo[nPMIdx].nCommStatus) return TRUE;
	}
	else if(RT_IdleRcp == m_PMInfo[nPMIdx].nRcpType || RT_PurgeRcp == m_PMInfo[nPMIdx].nRcpType || RT_CleanRcp == m_PMInfo[nPMIdx].nRcpType)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CEventLogServer::IsLPMRunning()
{
	//LP중에 하나라도 Run이면 설비 Run상태로 판단 처리
	for ( int i=0; i<MAX_LPM; i++)
	{
		if(LPM_Waiting == m_LPMInfo[i].nLPMStatus
			|| LPM_Running == m_LPMInfo[i].nLPMStatus
			|| LPM_Pausing == m_LPMInfo[i].nLPMStatus
			|| LPM_Paused == m_LPMInfo[i].nLPMStatus
			|| LPM_Aborting == m_LPMInfo[i].nLPMStatus) return TRUE;
	}

	return FALSE;
}

BOOL CEventLogServer::IsDevRunning(char *szDeviceID)
{
	BOOL bRet = FALSE;

	// 2013.07.02
	BOOL bFlag = FALSE;
	int i, j;

	for(i=0; i<MAX_MOD_RUN_DEF_TABLE; i++)
	{
		if(g_ModuleRunDefTbl[i].nPMIdx < 0) break;		// 2013.07.16

		// 2013.07.02
		bFlag = FALSE;
		for(j=0; j<MAX_RUN_MODULE; j++)
		{
			if(strcmp(szDeviceID, g_ModuleRunDefTbl[i].aszRunModule[j]) == 0) {
				bFlag = TRUE; break;
			}
		}
		if(bFlag) {
			bRet = IsPMRunning(g_ModuleRunDefTbl[i].nPMIdx);
			if(bRet) break;
		}
	}
	return bRet;
}

BOOL CEventLogServer::IsProcRunning(int nPMIdx)
{
	return m_PMInfo[nPMIdx].bIsProcRunning;
}


int CEventLogServer::GetActType(char *szEventID)
{
	int nRet = ATYPE_READY;
	if(strcmp(szEventID, "Get") == 0) nRet = ATYPE_GET;
	else if(strcmp(szEventID, "Put") == 0) nRet = ATYPE_PUT;
	else if(strcmp(szEventID, "Move") == 0) nRet = ATYPE_PUT;
	else if(strcmp(szEventID, "Rotate") == 0) nRet = ATYPE_ROT;		// 2013.07.30
	return nRet;
}

void CEventLogServer::UpdateLPMInfo(int nLPMIdx)
{
	int nIOStatus;
	READ_STRING(CTC_RECIPE_FILE+nLPMIdx, m_LPMInfo[nLPMIdx].szFlowRcpID, &nIOStatus);
	READ_STRING(CTC_MID_NAME+nLPMIdx, m_LPMInfo[nLPMIdx].szCarrierID, &nIOStatus);
	READ_STRING(CTC_JOB_NAME+nLPMIdx, m_LPMInfo[nLPMIdx].szLotID, &nIOStatus);

#ifdef _DEBUG
	for(int i=0; i<MAX_LPM;i++)
	{
		_LOG("UpdateLPMInfo : %d %s %s %s", nLPMIdx, m_LPMInfo[nLPMIdx].szFlowRcpID, 
			m_LPMInfo[nLPMIdx].szCarrierID, m_LPMInfo[nLPMIdx].szLotID);
	}
#endif
}

void CEventLogServer::ClearLPMInfo(int nLPMIdx)
{
	strcpy(m_LPMInfo[nLPMIdx].szFlowRcpID, "");
	strcpy(m_LPMInfo[nLPMIdx].szCarrierID, "");
	strcpy(m_LPMInfo[nLPMIdx].szFlowInfo, "$");
	strcpy(m_LPMInfo[nLPMIdx].szLotID, "");
}

void CEventLogServer::UpdatePMInfo(int nPMIdx)
{
	int nLPMIdx;

	// 2013.11.18
	char szPM[8];		
	int nPMSlotCount;

	nLPMIdx = GetWaferInfoFromPM(m_PMInfo[nPMIdx].szWaferInfo, nPMIdx);

	// 2013.11.18
	sprintf(szPM, "PM%d", nPMIdx+1);
	GetModuleInfo(szPM, &nPMSlotCount);
	
	// 2013.07.29
	if(RT_ProcRcp == m_PMInfo[nPMIdx].nRcpType && strlen(m_PMInfo[nPMIdx].szWaferInfo) > 3)	
	{
		if(nLPMIdx >= 0) strcpy(m_PMInfo[nPMIdx].szLotID, m_LPMInfo[nLPMIdx].szLotID);

		if(6 == nPMSlotCount) strcpy(m_PMInfo[nPMIdx].szSlotInfo, "[6,1,2,3,4,5,6]");
		// 2013.11.18
		else if(2 == nPMSlotCount) strcpy(m_PMInfo[nPMIdx].szSlotInfo, "[2,1,2]");
		else if(1 == nPMSlotCount) strcpy(m_PMInfo[nPMIdx].szSlotInfo, "1");
	}
	else
	{
		strcpy(m_PMInfo[nPMIdx].szLotID, "");
		strcpy(m_PMInfo[nPMIdx].szSlotInfo, "$");
	}

}

void CEventLogServer::ClearPMInfo(int nPMIdx)
{
	strcpy(m_PMInfo[nPMIdx].szLotID, "");
	strcpy(m_PMInfo[nPMIdx].szWaferInfo, "$");
	strcpy(m_PMInfo[nPMIdx].szSlotInfo, "$");
	strcpy(m_PMInfo[nPMIdx].szRecipeID, "");
}

int CEventLogServer::GetRcpType(char *szEventID, char *szRecipeName)
{
	int nRcpType = RT_PurgeRcp;
	if(strcmp(szEventID, "Process") == 0) nRcpType = RT_ProcRcp;
	// 2013.07.29
	else if(strcmp(szEventID, "PostLotRun") == 0) nRcpType = RT_LotPostRcp;
	else if(strcmp(szEventID, "PreLotRun") == 0) nRcpType = RT_LotPreRcp;
	else if(strcmp(szEventID, "IdleRun") == 0) nRcpType = RT_IdleRcp;
	return nRcpType;
}

int CEventLogServer::GetWaferInfo(char *szWaferInfo, char *szModule, int nSlot, BOOL bDualArm)
{
	int anLPMIdx[2] = { -1, -1 };
	int anWaferNo[2] = { 0, 0 };
	int nLPMIdx = -1;
	char szTemp[40];
	char szTempWaferInfo[40];		// 2013.07.16
	int nCount;						// 2013.07.16
	int nIOStatus;
	int nArmAmount;

	_LOG("-----> GetWaferInfo called (%s %d %d)", szModule, nSlot, bDualArm);

	strcpy(szWaferInfo, "$");
	if(strncmp(szModule, "PM", 2) == 0)
	{
		anLPMIdx[0] = GetWaferSrcIdxFromPM(szModule[2]-'1', nSlot, &anWaferNo[0]);
		if(bDualArm) anLPMIdx[1] = GetWaferSrcIdxFromPM(szModule[2]-'1', nSlot+1, &anWaferNo[1]);
	}
	else if(strncmp(szModule, "Port", 4) == 0)
	{
		anLPMIdx[0] = szModule[4]-'1';
		anWaferNo[0] = nSlot;
		if(bDualArm) { anLPMIdx[1] = anLPMIdx[0]; anWaferNo[1] = nSlot+1; }
	}
	else if(strcmp(szModule, "TM1") == 0)
	{
		GetModuleInfo("TM1",&nArmAmount);

		//2013.11.21
		if(nArmAmount == 4)
		{
			anLPMIdx[0] = dREAD_DIGITAL(CTC_TA_Wafer_Source+nSlot*2-2, &nIOStatus);		//2013.11.24
			anWaferNo[0] = dREAD_DIGITAL(CTC_TA_Wafer_Status+nSlot*2-2, &nIOStatus);	//2013.11.24
			if(bDualArm)
			{
				anLPMIdx[1] = dREAD_DIGITAL(CTC_TA_Wafer_Source2+nSlot*2-2, &nIOStatus);	//2013.11.24
				anWaferNo[1] = dREAD_DIGITAL(CTC_TA_Wafer_Status2+nSlot*2-2, &nIOStatus);	//2013.11.24
			}
		}
		else
		{
			anLPMIdx[0] = dREAD_DIGITAL(CTC_TA_Wafer_Source+nSlot*2-2, &nIOStatus);
			anWaferNo[0] = dREAD_DIGITAL(CTC_TA_Wafer_Status+nSlot*2-2, &nIOStatus);
			if(bDualArm)
			{
				anLPMIdx[1] = dREAD_DIGITAL(CTC_TB_Wafer_Source, &nIOStatus);
				anWaferNo[1] = dREAD_DIGITAL(CTC_TB_Wafer_Status, &nIOStatus);
			}
		}
		//2013.11.21

	}
	else if(strcmp(szModule, "TM2") == 0)
	{
		anLPMIdx[0] = dREAD_DIGITAL(CTC_TA2_Wafer_Source+nSlot-1, &nIOStatus);
		anWaferNo[0] = dREAD_DIGITAL(CTC_TA2_Wafer_Status+nSlot-1, &nIOStatus);
		if(bDualArm)
		{
			anLPMIdx[1] = dREAD_DIGITAL(CTC_TA_Wafer_Source2+nSlot-1, &nIOStatus);
			anWaferNo[1] = dREAD_DIGITAL(CTC_TA_Wafer_Status2+nSlot-1, &nIOStatus);
		}
	}
	else if(strcmp(szModule, "ATM1") == 0)
	{
		anLPMIdx[0] = dREAD_DIGITAL(CTC_FA_Wafer_Source+nSlot-1, &nIOStatus);
		anWaferNo[0] = dREAD_DIGITAL(CTC_FA_Wafer_Status+nSlot-1, &nIOStatus);
		if(bDualArm)
		{
			anLPMIdx[1] = dREAD_DIGITAL(CTC_FB_Wafer_Source, &nIOStatus);
			anWaferNo[1] = dREAD_DIGITAL(CTC_FB_Wafer_Status, &nIOStatus);
		}
	}
	else if(strcmp(szModule, "Aligner") == 0)	// 2013.06.21
	{
		anLPMIdx[0] = dREAD_DIGITAL(CTC_FM_AL_Wafer_Source+nSlot-1, &nIOStatus);
		anWaferNo[0] = dREAD_DIGITAL(CTC_FM_AL_Wafer_Status+nSlot-1, &nIOStatus);
	}
	else
	{
		anLPMIdx[0] = GetWaferSrcIdx(szModule, nSlot, &anWaferNo[0]);
		if(bDualArm) anLPMIdx[1] = GetWaferSrcIdx(szModule, nSlot+1, &anWaferNo[1]);
	}

	nCount = 0;
	if(anLPMIdx[0] >= 0 && anLPMIdx[0] < MAX_LPM && anWaferNo[0] > 0)			// 2013.07.30
	{
		sprintf(szTempWaferInfo, "'C%c:%d'", 'A'+anLPMIdx[0], anWaferNo[0]);
		nLPMIdx = anLPMIdx[0];
		nCount++;
	}
	if(anLPMIdx[1] >= 0 && anLPMIdx[1] < MAX_LPM && anWaferNo[1] > 0)			// 2013.07.30
	{
		if(anLPMIdx[0] >= 0 && anWaferNo[0] > 0) strcat(szTempWaferInfo, ",");	// 2013.07.30
		else nLPMIdx = anLPMIdx[1];
		sprintf(szTemp, "'C%c:%d'", 'A'+anLPMIdx[1], anWaferNo[1]);
		strcat(szTempWaferInfo, szTemp);
		nCount++;
	}

	// 2013.07.16
	if(nCount == 0) strcpy(szWaferInfo, "$");
	else if(nCount == 1) strcpy(szWaferInfo, szTempWaferInfo);
	else sprintf(szWaferInfo, "[%d,%s]", nCount, szTempWaferInfo);
	return nLPMIdx;
}

int CEventLogServer::GetWaferSrcIdxFromPM(int nPMIdx, int nSlot, int *pnWaferNo)
{
	int nLPMIdx = -1;
	int nIoWaferSrc = -1;
	int nIoWaferSts = -1;
	int nIOStatus;
	int nWaferNo;
	int nPMSlotIdx;

	switch(nPMIdx)
	{
	case EN_PM1: nIoWaferSrc = CTC_PM1_Wafer_Source;
				nIoWaferSts = CTC_PM1_Wafer_Status; break;
	case EN_PM2: nIoWaferSrc = CTC_PM2_Wafer_Source;
				nIoWaferSts = CTC_PM2_Wafer_Status; break;
	case EN_PM3: nIoWaferSrc = CTC_PM3_Wafer_Source;
				nIoWaferSts = CTC_PM3_Wafer_Status; break;
	case EN_PM4: nIoWaferSrc = CTC_PM4_Wafer_Source;
				nIoWaferSts = CTC_PM4_Wafer_Status; break;
	case EN_PM5: nIoWaferSrc = CTC_PM5_Wafer_Source;
				nIoWaferSts = CTC_PM5_Wafer_Status; break;
	}

	nPMSlotIdx = nSlot-1;
	if(nIoWaferSrc >= 0)
	{
		nWaferNo = dREAD_DIGITAL(nIoWaferSts+nPMSlotIdx, &nIOStatus);
		if(nWaferNo > 0)
		{
			nLPMIdx = dREAD_DIGITAL(nIoWaferSrc+nPMSlotIdx, &nIOStatus);
			*pnWaferNo = nWaferNo;
		}
	}
	return nLPMIdx;
}

int CEventLogServer::GetWaferSrcIdx(char *szModule, int nSlot, int *pnWaferNo)
{
	int nLPMIdx = -1;
	int nIoWaferSrc = -1;
	int nIoWaferSts = -1;
	int nIOStatus;
	int nWaferNo;
	int nSlotIdx;

	do {
		if(strcmp(szModule, "BM1") == 0 || strcmp(szModule, "LL1") == 0)
		{
			nIoWaferSrc = CTC_BM1_Wafer_Source;
			nIoWaferSts = CTC_BM1_Wafer_Status;
		}
		else if(strcmp(szModule, "BM2") == 0 || strcmp(szModule, "LL2") == 0)
		{
			nIoWaferSrc = CTC_BM2_Wafer_Source;
			nIoWaferSts = CTC_BM2_Wafer_Status;
		}
		else break;

		nSlotIdx = nSlot-1;
		if(nIoWaferSrc >= 0)
		{
			nWaferNo = dREAD_DIGITAL(nIoWaferSts+nSlotIdx, &nIOStatus);
			if(nWaferNo > 0)
			{
				nLPMIdx = dREAD_DIGITAL(nIoWaferSrc+nSlotIdx, &nIOStatus);
				*pnWaferNo = nWaferNo;
			}
		}
	} while(0);
	return nLPMIdx;
}

void CEventLogServer::ChangeToStandardModuleName(char *szModuleName)
{
	int i;
	for(i=0; i<MAX_MODULE_NAME_TABLE; i++)
	{
		if(NULL == *g_StdModNameTbl[i].szPreModuleName) break;
		if(strcmp(g_StdModNameTbl[i].szPreModuleName, szModuleName) == 0)
		{
			strcpy(szModuleName, g_StdModNameTbl[i].szStdModuleName);
			break;
		}
	}
}

int CEventLogServer::GetSlotInfo(char *szSlotString, BOOL *pbDualArm)
{
	int nSlot;
	CTextParser txtP;		// 2013.07.29

	// 2013.07.29
	if(strlen(szSlotString) <= 3)
	{
		nSlot = atoi(szSlotString);
		*pbDualArm = FALSE;
	}
	else
	{
		txtP.SetText(szSlotString+1);
		txtP.GetInt(&nSlot, ',');		// 정보 갯수
		txtP.GetInt(&nSlot, ',');		// 첫번째 Slot 정보
		*pbDualArm = TRUE;
	}
	return nSlot;
}

BOOL CEventLogServer::GetModuleInfo(char *szModule, int *pnSlotCount)
{
	BOOL bRet = FALSE;
	int i;
	for(i=0; g_ModuleInfo[i].szModule[0] != NULL; i++)
	{
		if(strcmp(szModule, g_ModuleInfo[i].szModule) == 0)
		{
			*pnSlotCount = g_ModuleInfo[i].nSlotCount;
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

void CEventLogServer::BackupModuleWaferInfo(char *szModule, char *szWaferInfo)
{
	int i;
	for(i=0; g_ModuleInfo[i].szModule[0] != NULL; i++)
	{
		if(strcmp(szModule, g_ModuleInfo[i].szModule) == 0)
		{
			strcpy(g_ModuleInfo[i].szWaferInfo, szWaferInfo);
			break;
		}
	}
//2013.11.24
#ifdef _DEBUG
	for(int j=0; g_ModuleInfo[j].szModule[0] != NULL; j++)
	{
		_LOG("BackupModuleWaferInfo In : %s %s", g_ModuleInfo[j].szModule, g_ModuleInfo[j].szWaferInfo);
	}
#endif

}

void CEventLogServer::GetWaferInfoFromModuleInfo(char *szModule, char *szWaferInfo)
{
	int i;
	for(i=0; g_ModuleInfo[i].szModule[0] != NULL; i++)
	{
		if(strcmp(szModule, g_ModuleInfo[i].szModule) == 0)
		{
			strcpy(szWaferInfo, g_ModuleInfo[i].szWaferInfo);
			break;
		}

	}
//2013.11.24
#ifdef _DEBUG
	for(int j=0; g_ModuleInfo[j].szModule[0] != NULL; j++)
	{
		_LOG("GetWaferInfoFromModuleInfo In : %s %s", g_ModuleInfo[j].szModule, g_ModuleInfo[j].szWaferInfo);
	}
#endif
}

//////////////////////////////////////////////////////////////////////
// Member Function(Thread Function)

void CEventLogServer::DoMonitor()
{
	int i;
	int nIOStatus;
	int nLPMStatus = -1;
	int nPMCommStatus = -1;
	BOOL bIsProcRunning = FALSE;

	WaitSec(10);
	while(WaitSec(1))
	{
		// Check Load Port Module
		if(! WaitSec(2)) break;
		for(i=0; i<MAX_LPM; i++)
		{
			if(! m_LPMInfo[i].bUse) continue;
			nLPMStatus = READ_DIGITAL(CTC_MAIN_CONTROL+i, &nIOStatus);
			if(m_LPMInfo[i].nLPMStatus != nLPMStatus)
			{
				if(LPM_Running == nLPMStatus && LPM_Pausing != m_LPMInfo[i].nLPMStatus
							&& LPM_Paused != m_LPMInfo[i].nLPMStatus)
				{
					UpdateLPMInfo(i);
				}

				m_LPMInfo[i].nLPMStatus = nLPMStatus;
				if(LPM_Waiting == nLPMStatus)
				{
					UpdateLPMInfo(i);
				}
				//2013.11.27 Move ClearLPMInfo
				//else if(LPM_Idle == nLPMStatus || LPM_WaitHandOff == nLPMStatus) ClearLPMInfo(i);
			}
		}

		// Check Process Module
		if(! WaitSec(2)) break;
		for(i=0; i<MAX_PM; i++)
		{
			if(! m_PMInfo[i].bUse) continue;
			nPMCommStatus = READ_DIGITAL(PM1_CommStatus+i, &nIOStatus);
			if(m_PMInfo[i].nCommStatus != nPMCommStatus)
			{
				if(PMC_CTCInUse == m_PMInfo[i].nCommStatus) ClearPMInfo(i);
				m_PMInfo[i].nCommStatus = nPMCommStatus;
				if(PMC_CTCInUse == nPMCommStatus)
				{
				}
			}
		}
	}
}
