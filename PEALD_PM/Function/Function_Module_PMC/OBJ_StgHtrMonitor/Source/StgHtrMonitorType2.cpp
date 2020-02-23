// StgHtrMonitorType2.cpp: implementation of the CStgHtrMonitorType2 class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//====================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//====================================================================

#include "Timer.h"
#include "TextParser.h"
#include "Definition.h"
#include "StgHtrMonitorType2.h"

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "HTA01_TmpRdAI"		, _K_A_IO	,	0	} ,
	{ "HTA01_TmpSpAIO"		, _K_A_IO	,	0	} ,
	{ "HTA01_TmpHgLmtAIO"	, _K_A_IO	,	0	} ,
	{ "HTA01_TmpLwLmtAIO"	, _K_A_IO	,	0	} ,
	{ "HTA_RampRateRAIO"	, _K_A_IO	,	0	} ,
	{ "NotDefined"			, _K_A_IO	,	0	} ,
	{ "HTA01_CalOffsAIO"	, _K_A_IO	,	0	} ,

	{ "HTA01_StbStsDM"		, _K_D_IO	,	0	} ,
	{ "HTA_MonOptDM"		, _K_D_IO	,	0	} ,

	{ "NotDefined"			, _K_D_IO	,	0	} ,
	{ "NotDefined"		, _K_D_IO	,	0	} ,
	{ "NotDefined"		, _K_D_IO	,	0	} ,
	{ "NotDefined"			, _K_D_IO	,	0	} ,
	//---------------------------------------------
	{ "HTA_CmdDO"			, _K_D_IO	,	0	} ,
	{ "HTA_CommStsDI"		, _K_D_IO	,	0	} ,
	{ "HTA_AlrmStsDM"		, _K_D_IO	,	0	} ,
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStgHtrMonitorType2::CStgHtrMonitorType2()
{
	m_bWatlow9x8DriverMon		= TRUE;
	m_nStgHtrCommErrRetry		= 5;
	m_nStgHtrACommErrCnt		= 0;
}

CStgHtrMonitorType2::~CStgHtrMonitorType2()
{

}

BOOL CStgHtrMonitorType2::Initialize()
{
	BOOL bRet = TRUE;

	ReadPmcConfig();
	if(! InitIOList(g_pIOList)) printf("-----> InitIOList Failed in %s\n", FUNC_NAME);
	CreateThread();
	return bRet;
}

BOOL CStgHtrMonitorType2::Initialize(char * szSHTNum)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Thread Function

void CStgHtrMonitorType2::DoMonitor()
{
	int		nValue = 0;
	int		nIOStatus;
	int		nAlarmID = 0;

	WaitSec(5);
	InitW988HeaterChannel();
	WaitSec(5);
	do {
		do{
			nValue = dREAD_DIGITAL(HTA_CommStsDI, &nIOStatus);		// 0:OffLine 1:OnLine
			if(nValue == 1)
			{
				m_nStgHtrACommErrCnt = 0;
				break;
			}
			else
			{
				m_nStgHtrACommErrCnt++;
				if(m_nStgHtrACommErrCnt > m_nStgHtrCommErrRetry)
				{
					// Set Process Disable Interlock
					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					nAlarmID = ALARM_WATLOW988_1_COMM_ERROR + m_nSHTNum - 1;
					if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);

					m_nStgHtrACommErrCnt = 0;
				}
			}
		} while(WaitSec(1));
	} while(WaitSec(3));
}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CStgHtrMonitorType2::InitW988HeaterChannel()
{
	BOOL bRet = TRUE;
	int nIOStatus;
	int nAlarmRecovery;

	do {
		dWRITE_DIGITAL(HTA_CmdDO, 0, &nIOStatus);	// 0:Initialize
		if(nIOStatus == FALSE) bRet = FALSE;
		else bRet = TRUE;
		if(bRet == FALSE)
		{
			nAlarmRecovery = ALARM_MANAGE(ALARM_988_HEATER_INIT_FAILED);
			if(nAlarmRecovery == ALM_IGNORE) {bRet = TRUE; break; }
			if(nAlarmRecovery != ALM_RETRY) break;
		}
		else break;
	} while(1);
	if(bRet == FALSE)
	{
		printf("-----> Watlow 988 Heater Initialize failed in %s\n", FUNC_NAME);
	}
	return bRet;
}

void CStgHtrMonitorType2::ParseDriverMonOption(char *szData)
{
	char szItem[40] = "";
	char szValue[40] = "";
	BOOL bEnable = TRUE;

	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(! tp.GetWord(szValue)) break;
		tp.MakeLower(szValue);

		if(strcmp(szValue, "enable") == 0) bEnable = TRUE;
		else if(strcmp(szValue, "disable") == 0) bEnable = FALSE;

		if(strcmp(szItem, "Watlow9x8DriverMonitor") == 0) m_bWatlow9x8DriverMon = bEnable;
		else if(strcmp(szItem, "Watlow9x8DriverComm_Retry") == 0) m_nStgHtrCommErrRetry = atoi(szValue);
		/*
		else
		{
			printf("-----> Driver Status Monitoring Configration is Invalid[%s]\n", szData);
		}
		*/

//		printf("-----> Item : %s   Value : %s\n", szItem, szValue);
	} while(0);
}

BOOL CStgHtrMonitorType2::ReadPmcConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(PMC_CONFIG_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("PLC Interlock List", szItem) == 0)					nTitle = 1;
				else if(strcmp("Driver CommStatus Monitoring", szItem) == 0)	nTitle = 2;
				else if(strcmp("Z-Motion Control", szItem) == 0)				nTitle = 3;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					// case 1: ParsePLCInterlockList(szRead); break;
					case 2: ParseDriverMonOption(szRead); break;
					// case 3: ParseZMotionOption(szRead); break;
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
		printf("-----> Config File Reading Failed : %s\n", PMC_CONFIG_FILE);
	}
	return bRet;
}

void CStgHtrMonitorType2::CheckHtrError()
{
	int		nIOStatus;
	int		nErr1, nErr2;
	int		nMonOpt;
	int		nAlarmID = 0;

	nErr1 = dREAD_DIGITAL(SHT_Error1DI, &nIOStatus);
	nErr2 = dREAD_DIGITAL(SHT_Error2DI, &nIOStatus);

	// Monitoring Option => 0:Ignore 1:Warning 2:ChmDis 3:Abort
	nMonOpt = dREAD_DIGITAL(SHT_MonOptDM, &nIOStatus);

	if(nErr1 == 2)
	{
		if(nMonOpt > 0)
		{
			nAlarmID = ALARM_STAGE_HTR_1_TEMP_HIGH_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
	}
	else if(nErr1 == 3)
	{
		if(nMonOpt > 0)
		{
			nAlarmID = ALARM_STAGE_HTR_1_TEMP_LOW_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
	}
	else if(nErr1 != 0 || nErr2 != 0)
	{
		if(nMonOpt > 0)
		{
			nAlarmID = ALARM_STAGE_HTR_1_CONTROLL_FAIL + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
	}

	if(nErr1 != 0)	dWRITE_DIGITAL(HTA_AlrmStsDM, 1, &nIOStatus);	// 0:Off 1:On
	else			dWRITE_DIGITAL(HTA_AlrmStsDM, 0, &nIOStatus);
}

void CStgHtrMonitorType2::CheckHtrError_SW()
{
	int		nIOStatus;
	int		nErr1, nErr2;
	int		nMonOpt;
	BOOL	bError;
	int		nAlarmID = 0;

	nErr1 = dREAD_DIGITAL(SHT_Error1DI, &nIOStatus);
	nErr2 = dREAD_DIGITAL(SHT_Error2DI, &nIOStatus);

	// Monitoring Option => 0:Ignore 1:Warning 2:ChmDis 3:Abort
	nMonOpt = dREAD_DIGITAL(SHT_MonOptDM, &nIOStatus);

	if(nErr1 != 0 || nErr2 != 0)
	{
		if(nMonOpt > 0)
		{
			nAlarmID = ALARM_STAGE_HTR_1_CONTROLL_FAIL + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
	}

	bError = FALSE;
	if(m_dbTmpRd > m_dbHighLmt)
	{
		if(nMonOpt > 0)
		{
			nAlarmID = ALARM_STAGE_HTR_1_TEMP_HIGH_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}
	else if(m_dbTmpRd < m_dbLowLmt)
	{
		if(nMonOpt > 0)
		{
			nAlarmID = ALARM_STAGE_HTR_1_TEMP_LOW_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}

	if(nErr1 != 0 || bError == TRUE)	dWRITE_DIGITAL(HTA_AlrmStsDM, 1, &nIOStatus);	// 0:Off 1:On
	else								dWRITE_DIGITAL(HTA_AlrmStsDM, 0, &nIOStatus);
}
