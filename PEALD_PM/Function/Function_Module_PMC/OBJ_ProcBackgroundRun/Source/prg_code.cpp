/////////////////////////////////////////////////////////////////////////
// Version : 1.0.1.0
// Modify : 2004.08.24

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextParser.h"
#include "DynamicIO.h"

#define APP_NAME	"ProcessBackgroundRun"

//------------------------------------------------------------------------------------------
// Alarm Definition(1500 ~ 1599)

#define ALARM_CHM_PRSS_HIGH			1502
#define ALARM_CHM_PRSS_LOW			1503
#define ALARM_BACKSIDE_AR_PRS_HIGH	1504
#define ALARM_BACKSIDE_AR_PRS_LOW	1505

//------------------------------------------------------------------------------------------
// Enumeration  Definition

//--------------------------------------------------------------------
// 2006.06.28
enum { ALMOPT_IGNORE, ALMOPT_WARNING, ALMOPT_CHMDIS, ALMOPT_ABORT, ALMOPT_RETRY };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_ModuleFailDM"				, _K_D_IO	,	SW_ModuleFailDM		,	0	} ,
	{ "M651_Pressure"				, _K_A_IO	,	M651_Pressure		,	0	} ,
	// 2006.01.26
	{ "ChmPrsAI"					, _K_A_IO	,	ChmPrsAI			,	0	} ,
	{ "PRMA_EC_ChmPrss"				, _K_A_IO	,	PRMA_EC_ChmPrss		,	0	} ,
	""
};

//------------------------------------------------------------------------------------------

// 2006.01.26
enum GUAGE_TYPE
{
	LLT_GUAGE,				// ChmPrsAI Guage
	MKS_GUAGE,				// M651_Pressure Guage
	// 2006.02.08
	MIX1_TYPE,				// Lower than 1 Torr : ChmPrsAI, Higher than 1 Torr : M651_Pressure
	MIX2_TYPE				// Lower than 1 Torr : ChmPrsAI, Higher than 1 Torr : M651_Pressure
 };

int g_nGuageType = LLT_GUAGE;

double g_dbProcPrsHighLmt = 1;
double g_dbProcPrsLowLmt = 0;
int g_nProcPrs_Option = 0;
int g_nProcPrs_ErrCnt = 0;
int	g_nProcPrs_ErrLmt = 5;

// 2005.06.02 by cskim for Backside Ar Check
BOOL g_bBacksideArChk = FALSE;
double	g_dbBsArPrsHighLmt = 5;
double	g_dbBsArPrsLowLmt = 0;
int g_nBsArPrsErrLmt = 5;
int g_nBsArPrsErrCnt = 0;

// 2006.01.18
int g_nProcPrs_Timeout = 0;

//------------------------------------------------------------------------------------------
// 2006.06.28
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	MFM01FlwAI,
	PRMA_BsArHgLmt,
	PRMA_BsArLoLmt,
	PRMA_EC_BsAr,
	PRO_FlowControlDM,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{"MFM01FlwAI",		_K_A_IO, 0},
	{"PRMA_BsArHgLmt",	_K_A_IO, 0},
	{"PRMA_BsArLoLmt",	_K_A_IO, 0},
	{"PRMA_EC_BsAr",	_K_A_IO, 0},
	{"PRO_FlowControlDM", _K_D_IO, 0},
	""
};

//--------------------------------------------------------------------
// 2006.06.28
CDynamicIO g_DynIO;


//------------------------------------------------------------------------------------------
// Alarm(Hazard) Option Function
// return TRUE : Sequence Is Continuous
// return FALSE : Alarm Is Valid And Sequence Aborted

BOOL CheckAlarmOption(int nAlarm, int nOption)
{
	// 2006.06.28
	enum { PRO_ABORT, PRO_RETRY };
	enum { INTLCK_OFF, INTLCK_ON };

	BOOL bRet = TRUE;
	int nIOStatus;
	switch(nOption)
	{
	case ALMOPT_IGNORE: break;
	case ALMOPT_WARNING:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		break;
	case ALMOPT_CHMDIS:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		WRITE_DIGITAL(SW_ModuleFailDM, INTLCK_ON, &nIOStatus);
		break;
	case ALMOPT_ABORT:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		bRet = FALSE;
		break;
	case ALMOPT_RETRY:		// 2006.06.28
		g_DynIO.dWRITE_DIGITAL(PRO_FlowControlDM, PRO_RETRY, &nIOStatus);
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		bRet = FALSE;
	default: break;
	}
	return bRet;
}


//------------------------------------------------------------------------------------------
// Sub Function

void InitParam()
{
	int nIOStatus = 0;

	// Process Pressure
	g_dbProcPrsHighLmt = 10;
	g_dbProcPrsLowLmt = 0;
	g_nProcPrs_Option = 0;
	g_nProcPrs_ErrCnt = 0;
	g_nProcPrs_ErrLmt = (int) READ_ANALOG(PRMA_EC_ChmPrss, &nIOStatus);

	// 2006.01.18
	g_nProcPrs_Timeout = 0;
}

BOOL ParseParam(const char *pParam)
{
	BOOL bRet = FALSE;
	float fValue;
	char szCmd[40];
	char szValue[40];
	CTextParser tp(pParam);

	do {
		// Command
		tp.GetWord(szCmd);
		if(strcmp(szCmd, "START") == 0)
		{
			// Process Pressure
			if(false == tp.GetFloat(&fValue)) break;
			g_dbProcPrsHighLmt = fValue;
			tp.GetFloat(&fValue); g_dbProcPrsLowLmt = fValue;
			tp.GetInt(&g_nProcPrs_Option);

			// Backside Ar Check
			if(false == tp.GetWord(szValue)) break;
			if(strcmp(szValue, "BACKSIDE_AR_CHK") == 0) g_bBacksideArChk = TRUE;
			else g_bBacksideArChk = FALSE;
		}
		// 2006.01.08
		else if(strcmp(szCmd, "START2") == 0)
		{
			// Process Pressure
			if(false == tp.GetFloat(&fValue)) break;
			g_dbProcPrsHighLmt = fValue;
			tp.GetFloat(&fValue); g_dbProcPrsLowLmt = fValue;
			tp.GetInt(&g_nProcPrs_Option);
			tp.GetInt(&g_nProcPrs_Timeout);

			// Backside Ar Check
			if(false == tp.GetWord(szValue)) break;
			if(strcmp(szValue, "BACKSIDE_AR_CHK") == 0) g_bBacksideArChk = TRUE;
			else g_bBacksideArChk = FALSE;
		}

		bRet = TRUE;
	} while(0);
	return bRet;
}

//------------------------------------------------------------------------------------------
// Backside Ar Check
// 2005.06.02 by cskim for Backside Ar Check

void InitBacksideArCheckOption()
{
	BOOL bIOStatus;

	if(! g_bBacksideArChk) return;
	if(g_DynIO.IOValid(PRMA_BsArHgLmt))
		g_dbBsArPrsHighLmt = g_DynIO.dREAD_ANALOG(PRMA_BsArHgLmt, &bIOStatus);
	if(g_DynIO.IOValid(PRMA_BsArLoLmt))
		g_dbBsArPrsLowLmt = g_DynIO.dREAD_ANALOG(PRMA_BsArLoLmt, &bIOStatus);
	if(g_DynIO.IOValid(PRMA_EC_BsAr))
		g_nBsArPrsErrLmt = (int) g_DynIO.dREAD_ANALOG(PRMA_EC_BsAr, &bIOStatus);
	if(! g_DynIO.IOValid(MFM01FlwAI)) g_bBacksideArChk = FALSE;
	g_nBsArPrsErrCnt = 0;
}

BOOL CheckBacksideAr()
{
	BOOL bRet = TRUE;
	double dbBsArPrs;
	BOOL bIOStatus;
	int nAlarm = ALARM_BACKSIDE_AR_PRS_LOW;
	if(! g_bBacksideArChk) return bRet;
	// printf("-----> Backside Ar Check in ProcBgRun\n");

	dbBsArPrs = g_DynIO.dREAD_ANALOG(MFM01FlwAI, &bIOStatus);
	// printf("-----> BsAr : %7.3f (%7.3f ~ %7.3f) in ProcBgRun\n",
	//	dbBsArPrs, g_dbBsArPrsLowLmt, g_dbBsArPrsHighLmt);
	if(dbBsArPrs < g_dbBsArPrsLowLmt || dbBsArPrs > g_dbBsArPrsHighLmt)
	{
		if(dbBsArPrs > g_dbBsArPrsHighLmt)  nAlarm = ALARM_BACKSIDE_AR_PRS_HIGH;
		g_nBsArPrsErrCnt++;
		// printf("-----> Backside Ar Error[%d] in ProcBgRun\n", g_nBsArPrsErrCnt);
	}
	else g_nBsArPrsErrCnt = 0;
	if(g_nBsArPrsErrCnt > g_nBsArPrsErrLmt)
	{
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		bRet = FALSE;
	}
	return bRet;
}

//------------------------------------------------------------------------------------------
// Process Pressure

// 2006.01.26
double GetChamberPressure(double *pdbRefPrs = NULL)
{
	double dbChmPrs;
	BOOL bIOStatus;
	switch(g_nGuageType)
	{
	case LLT_GUAGE: dbChmPrs = READ_ANALOG(ChmPrsAI, &bIOStatus); break;
	case MKS_GUAGE: dbChmPrs = READ_ANALOG(M651_Pressure, &bIOStatus); break;
	case MIX1_TYPE:					// 2006.02.08
		if(pdbRefPrs != NULL)
		{
			if(*pdbRefPrs < 1) dbChmPrs = READ_ANALOG(ChmPrsAI, &bIOStatus);
			else dbChmPrs = READ_ANALOG(M651_Pressure, &bIOStatus);
			break;
		}
	case MIX2_TYPE:					// 2006.02.08
		dbChmPrs = READ_ANALOG(ChmPrsAI, &bIOStatus);
		if(dbChmPrs >= 1) READ_ANALOG(M651_Pressure, &bIOStatus);
		break;
	default: dbChmPrs = READ_ANALOG(ChmPrsAI, &bIOStatus); break;
	}
	return dbChmPrs;
}

BOOL CheckChmPrs()
{
	char pszLocFtnName[] = "CheckChmPrs";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	double dblValue;

	if(g_nProcPrs_Option == 0) return TRUE;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	do {
		dblValue = GetChamberPressure(&g_dbProcPrsHighLmt);		// 2006.02.08
		if(dblValue > g_dbProcPrsHighLmt)
		{
			g_nProcPrs_ErrCnt++;
			if(g_nProcPrs_ErrCnt >= g_nProcPrs_ErrLmt)
			{
				if(CheckAlarmOption(ALARM_CHM_PRSS_HIGH, g_nProcPrs_Option) == FALSE) break;
				g_nProcPrs_ErrCnt = 0;
			}
		}
//		else if(dblValue < g_dbProcPrsLowLmt)
		//... 2015.01.20 Check Skip Low Limit "0" for Full Pumping Minus Pressure
		else if(dblValue < g_dbProcPrsLowLmt && g_dbProcPrsLowLmt > 0)
		{
			g_nProcPrs_ErrCnt++;
			if(g_nProcPrs_ErrCnt >= g_nProcPrs_ErrLmt)
			{
				if(CheckAlarmOption(ALARM_CHM_PRSS_LOW, g_nProcPrs_Option) == FALSE) break;
				g_nProcPrs_ErrCnt = 0;
			}
		}
		else g_nProcPrs_ErrCnt = 0;
		bRet = TRUE;
	} while(0);
	return bRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 

Module_Status ProcBgRunStart(const char *pParam)
{
	char pszLocFtnName[] = "ProcBgRunStart";
	char pszFtnName[256];
	Module_Status msRet = SYS_SUCCESS;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	InitParam();
	ParseParam(pParam);

	// 2006.01.18
	if(g_nProcPrs_Timeout > 0) g_nProcPrs_ErrLmt = g_nProcPrs_Timeout;

	InitBacksideArCheckOption();
	do {
		if(! WAIT_SECONDS(0.1)) break;
		if(CheckChmPrs() == FALSE) { msRet = SYS_ABORTED; break; }
		if(! WAIT_SECONDS(0.1)) break;
		if(! WAIT_SECONDS(0.1)) break;
		if(! WAIT_SECONDS(0.1)) break;
		if(! WAIT_SECONDS(0.1)) break;
		if(CheckBacksideAr() == FALSE) { msRet = SYS_ABORTED; break; }
		if(! WAIT_SECONDS(0.1)) break;
		if(! WAIT_SECONDS(0.1)) break;
		if(! WAIT_SECONDS(0.1)) break;
		if(! WAIT_SECONDS(0.1)) break;
		if(! WAIT_SECONDS(0.1)) break;
	} while(1);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//==========================================================================================
extern "C"
{

Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int nParLen = 0;
	char *pParam;
	char szCmd[41], szSubParam[256];

	printf("Entered %s\n", APP_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );
		if(strcmp(szCmd, "START") == 0) msRet = ProcBgRunStart(pParam);
		else if(strcmp(szCmd, "START2") == 0) msRet = ProcBgRunStart(pParam);
		else
		{
			msRet = SYS_ABORTED;
		}
	} while(0);

	printf("Leave %s\n", APP_NAME);
	return msRet;
}

BOOL Program_Init_Code() {
	g_DynIO.InitIOList(g_pIOList, false);
	return TRUE;
}

//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char szParam[256];
	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	#ifdef _DEBUG
	printf("Argument = %s in Program_Enter_Code\n", szParam);
	#endif

	char szTemp[80];

	CTextParser tp(szParam);
	while(! tp.IsEnd())
	{
		tp.GetWord(szTemp, '|');
		if(tp.BeEqual("LLT", szTemp)) g_nGuageType = LLT_GUAGE;
		if(tp.BeEqual("MKS", szTemp)) g_nGuageType = MKS_GUAGE;
		if(tp.BeEqual("MIX1", szTemp)) g_nGuageType = MIX1_TYPE;
		if(tp.BeEqual("MIX2", szTemp)) g_nGuageType = MIX2_TYPE;
	}
}

} // End of [extern "C"]
//==========================================================================================
