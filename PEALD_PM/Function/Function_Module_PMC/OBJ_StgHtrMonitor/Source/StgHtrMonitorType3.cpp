// StgHtrMonitorType3.cpp: implementation of the CStgHtrMonitorType3 class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

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
#include "StgHtrMonitorType3.h"

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "SHT$0_TmpRdAI"		, _K_A_IO	,	0	} ,
	{ "SHT$0_TmpSpAIO"		, _K_A_IO	,	0	} ,
	{ "SHT$0_TmpHgLmtAIO"	, _K_A_IO	,	0	} ,
	{ "SHT$0_TmpLwLmtAIO"	, _K_A_IO	,	0	} ,
	{ "SHT$0_RampRateAIO"	, _K_A_IO	,	0	} ,
	{ "NotDefined"			, _K_A_IO	,	0	} ,
	{ "SHT$0_CalOffsAIO"	, _K_A_IO	,	0	} ,

	{ "SHT$0_StbStsDM"		, _K_D_IO	,	0	} ,
	{ "SHT$0_MonOptDM"		, _K_D_IO	,	0	} ,

	//{ "SHT$0_OutEnDO"		, _K_D_IO	,	0	} ,
	{ "SHT_OutEnDO"			, _K_D_IO	,	0	} ,
	{ "NotDefined"			, _K_D_IO	,	0	} ,
	{ "NotDefined"			, _K_D_IO	,	0	} ,
	{ "SHT$0_AlrmSts1DI"	, _K_D_IO	,	0	} ,
	//---------------------------------------------
	{ "SHT$0_RatioAIO"		, _K_A_IO	,	0	} ,
	{ "SHT$0_TmpSpAI"		, _K_A_IO	,	0	} ,
	{ "StgHtr$0RelayDO"		, _K_D_IO	,	0	} ,
	{ "StgHtr$0RelayDI"		, _K_D_IO	,	0	} ,
	//... 2016.02.28
	{ "StgHtr$0CurrAI"		, _K_A_IO	,	0	} ,
	{ "StgHtr$0VoltAI"		, _K_A_IO	,	0	} ,

	{ "STH$0_CurHiLimitAM"	, _K_A_IO	,	0	} ,
	{ "STH$0_CurLwLimitAM"	, _K_A_IO	,	0	} ,
	{ "STH$0_VtgHiLimitAM"	, _K_A_IO	,	0	} ,
	{ "STH$0_VtgLwLimitAM"	, _K_A_IO	,	0	} ,
	//...
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStgHtrMonitorType3::CStgHtrMonitorType3()
{
//	m_nSHTNum = 0;
}

CStgHtrMonitorType3::~CStgHtrMonitorType3()
{

}

BOOL CStgHtrMonitorType3::Initialize()
{
	BOOL bRet = TRUE;
	int nIOStatus;
	double dbValue;

	if(! InitIOList(g_pIOList)) printf("-----> InitIOList Failed in %s\n", FUNC_NAME);

	dbValue = dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_TmpSpAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_TmpHgLmtAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_TmpHgLmtAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_TmpLwLmtAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_TmpLwLmtAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_RampRateRAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_RampRateRAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_CalOffsAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_CalOffsAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_RatioAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_RatioAIO, dbValue, &nIOStatus);

	Sleep(1000);
	dWRITE_DIGITAL(SHT_OutEnDO, 1, &nIOStatus);

	return bRet;
}

BOOL CStgHtrMonitorType3::Initialize(char * szSHTNum)
{
	BOOL bRet = TRUE;
	int nIOStatus;
	double dbValue;

	ReplaceIONameCharSet(g_pIOList, "$0",szSHTNum);

	m_nSHTNum = atoi(szSHTNum);

	if(! InitIOList(g_pIOList)) printf("-----> InitIOList Failed in %s\n", FUNC_NAME);
	
	dbValue = dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_TmpSpAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_TmpHgLmtAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_TmpHgLmtAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_TmpLwLmtAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_TmpLwLmtAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_RampRateRAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_RampRateRAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_CalOffsAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_CalOffsAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_RatioAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_RatioAIO, dbValue, &nIOStatus);

	Sleep(1000);
	dWRITE_DIGITAL(SHT_OutEnDO, 1, &nIOStatus);

	return bRet;
	
}


//////////////////////////////////////////////////////////////////////
// Member Function

void CStgHtrMonitorType3::ReadHeaterTemp()
{
	BOOL bIOStatus;
	CStgHtrMonitor::ReadHeaterTemp();
	m_dbTmpSpAI = dREAD_ANALOG(SHT_TmpSpAI, &bIOStatus);
	//... 2016.02.28
	m_dbCurrentAI = dREAD_ANALOG(StgHtr_CurrAI, &bIOStatus);
	m_dbVoltageAI = dREAD_ANALOG(StgHtr_VoltAI, &bIOStatus);
	m_dbCurrHiLimit = dREAD_ANALOG(STH_CurHiLimitAM, &bIOStatus);
	m_dbCurrLwLimit = dREAD_ANALOG(STH_CurLwLimitAM, &bIOStatus);
	m_dbVtgHiLimit  = dREAD_ANALOG(STH_VtgHiLimitAM, &bIOStatus); 
	m_dbVtgLwLimit  = dREAD_ANALOG(STH_VtgLwLimitAM, &bIOStatus);
	//...
}

void CStgHtrMonitorType3::CheckHtrError()
{
	enum { RCMD_OFF, RCMD_ON };
	int		nIOStatus;
	int		nAlarmSts;
	int		nMonOpt;
	BOOL	bError;
	int		nAlarmID = 0;

	nAlarmSts = dREAD_DIGITAL(SHT_AlrmSts1DI, &nIOStatus);
	// Monitoring Option => 0:Ignore 1:Warning 2:ChmDis 3:Abort
	nMonOpt = dREAD_DIGITAL(SHT_MonOptDM, &nIOStatus);

	if(nAlarmSts != 0)
	{
		if(nMonOpt != MO_Ignore)
		{
			nAlarmID = ALARM_STAGE_HTR_1_CONTROLL_FAIL + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
	}

	bError = FALSE;
	if(m_dbTmpRd > m_dbHighLmt)
	{
		if(nMonOpt != MO_Ignore)
		{
			nAlarmID = ALARM_STAGE_HTR_1_TEMP_HIGH_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}
	else if(m_dbTmpRd < m_dbLowLmt)
	{
		if(nMonOpt != MO_Ignore)
		{
			nAlarmID = ALARM_STAGE_HTR_1_TEMP_LOW_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}

	if(fabs(m_dbTmpRd-m_dbTmpSpAI) > 50)
	{
		nAlarmID = ALARM_STAGE_HTR_1_WORK_TMP_ERROR + m_nSHTNum - 1;
		if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);

		dWRITE_DIGITAL(StgHtrRelayDO, RCMD_OFF, &nIOStatus);
		ApplyAlarmEffect(MO_Abort);
		bError = TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	//... 2016.02.28 Chuck Heater Electric Current Check
	if(m_dbCurrentAI > m_dbCurrHiLimit)
	{
		if(nMonOpt != MO_Ignore)
		{
			nAlarmID = ALARM_STAGE_HTR_1_CURR_HIGH_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}
	else if(m_dbCurrentAI < m_dbCurrLwLimit)
	{
		if(nMonOpt != MO_Ignore)
		{
			nAlarmID = ALARM_STAGE_HTR_1_CURR_LOW_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	//... 2016.02.28 Chuck Heater Voltage Check
	if(m_dbVoltageAI > m_dbVtgHiLimit)
	{
		if(nMonOpt != MO_Ignore)
		{
			nAlarmID = ALARM_STAGE_HTR_1_VOLT_HIGH_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}
	else if(m_dbVoltageAI < m_dbVtgLwLimit)
	{
		if(nMonOpt != MO_Ignore)
		{
			nAlarmID = ALARM_STAGE_HTR_1_VOLT_LOW_LIMIT + m_nSHTNum - 1;
			if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);
		}
		ApplyAlarmEffect(nMonOpt);
		bError = TRUE;
	}
}
