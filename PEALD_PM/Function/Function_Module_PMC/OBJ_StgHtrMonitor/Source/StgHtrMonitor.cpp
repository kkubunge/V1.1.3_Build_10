// StgHtrMonitor.cpp: implementation of the CStgHtrMonitor class.
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
#include "Definition.h"
#include "StgHtrMonitor.h"

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	SHT_TmpRdAI,
	SHT_TmpSpAIO,
	SHT_TmpHgLmtAIO,
	SHT_TmpLwLmtAIO,
	SHT_RampRateRAIO,
	SHT_RampRateFAIO,
	SHT_CalOffsAIO,

	SHT_StbStsDM,
	SHT_MonOptDM,

	SHT_OutEnDO,
	SHT_Error1DI,
	SHT_Error2DI,
	SHT_AlrmSts1DI,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "SHT_TmpRdAI"			, _K_A_IO	,	0	} ,
	{ "SHT_TmpSpAIO"		, _K_A_IO	,	0	} ,
	{ "SHT_TmpHgLmtAIO"		, _K_A_IO	,	0	} ,
	{ "SHT_TmpLwLmtAIO"		, _K_A_IO	,	0	} ,
	{ "SHT_RampRateRAIO"	, _K_A_IO	,	0	} ,
	{ "SHT_RampRateFAIO"	, _K_A_IO	,	0	} ,
	{ "SHT_CalOffsAIO"		, _K_A_IO	,	0	} ,

	{ "SHT_StbStsDM"		, _K_D_IO	,	0	} ,
	{ "SHT_MonOptDM"		, _K_D_IO	,	0	} ,

	{ "SHT_OutEnDO"			, _K_D_IO	,	0	} ,
	{ "SHT_Error1DI"		, _K_D_IO	,	0	} ,
	{ "SHT_Error2DI"		, _K_D_IO	,	0	} ,
	{ "SHT_AlrmSts1DI"		, _K_D_IO	,	0	} ,
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStgHtrMonitor::CStgHtrMonitor()
{
	m_nMonMethod = 0;
	m_nStableCount = 0;
	m_dbTmpRd = 500;
	m_dbTmpSp = 500;
	m_dbHighLmt = 1000;
	m_dbLowLmt = 0;
	m_dbTol = 5;

	m_nStableSts = SHSS_Stable;	// 0:Stable 1:Unstable 2:Moving
	m_nStableTm = 0;

	m_pTmr = new CTimer;
	m_nSHTNum = 1;	// default value '1' - 2007.04.24 ijbae.
}

CStgHtrMonitor::~CStgHtrMonitor()
{
	delete m_pTmr;
}

BOOL CStgHtrMonitor::Initialize()
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

	dbValue = dREAD_ANALOG(SHT_RampRateFAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_RampRateFAIO, dbValue, &nIOStatus);

	dbValue = dREAD_ANALOG(SHT_CalOffsAIO, &nIOStatus);
	dWRITE_ANALOG(SHT_CalOffsAIO, dbValue, &nIOStatus);

	WAIT_SECONDS(1);
	dWRITE_DIGITAL(SHT_OutEnDO, 1, &nIOStatus);
	return bRet;
}

BOOL CStgHtrMonitor::Initialize(char * szSHTNum)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Member Function

Module_Status CStgHtrMonitor::StartMonitor()
{
	Module_Status msRet = SYS_SUCCESS;
	int nTickCount = 0;
	int nIOStatus = 0;
	do {
		ReadHeaterTemp();
		CheckTempStable();
		//... 2016.02.28 Parameter Save
		if(nTickCount % 10 == 0)
		{
			WRITE_DIGITAL(SHT_PARM_Save,1,&nIOStatus);
		}
		else if(nTickCount % 10 == 2)
		{
			WRITE_DIGITAL(SHT_PARM_Save,0,&nIOStatus);
		}

		if(nTickCount % 5 == 0)		// per 5 sec
		{
			// 0:HW Monitoring 1:SW Monitoring
			if(m_nMonMethod == 0) CheckHtrError();
			else CheckHtrError_SW();
		}
		//if(! WAIT_SECONDS(1)) break;
		//... 2017.03.07
		Sleep(1000);
		if(++nTickCount >= 10000) nTickCount = 0;
	} while(1);
	return msRet;
}

void CStgHtrMonitor::ApplyAlarmEffect(int nMonOpt)
{
	int nIOStatus;
	switch(nMonOpt)
	{
	case MO_Ignore:
	case MO_Warning:
		break;
	case MO_ChmDis:
		WRITE_DIGITAL(SW_ModuleFailDM, 1, &nIOStatus);	// 0:Off 1:On
		break;
	case MO_Abort:
		WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);	// 0:Off 1:On
		break;
	}
}

void CStgHtrMonitor::CheckHtrError()
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
}

void CStgHtrMonitor::CheckHtrError_SW()
{
	CheckHtrError();
}

void CStgHtrMonitor::CheckTempStable()
{
	int nIOStatus;
	int nTol, nDef;

	nDef = abs((int)(m_dbTmpSp - m_dbTmpRd));
	nTol = (int) m_dbTol;
	if(m_nStableSts ==  SHSS_Stable)
	{
		if(nDef > nTol)
		{
			dWRITE_DIGITAL(SHT_StbStsDM, 1, &nIOStatus);
			m_pTmr->StartTimer();
		}
	}
	else if(m_nStableSts == SHSS_Unstable)
	{
		if(nDef > nTol) m_pTmr->StartTimer();
		else if(m_pTmr->GetProgressTime(UNIT_SECOND) >= m_nStableTm) 
				dWRITE_DIGITAL(SHT_StbStsDM, 0, &nIOStatus);
	}
	else
	{
		if(nDef <= nTol)
		{
			dWRITE_DIGITAL(SHT_StbStsDM, SHSS_Unstable, &nIOStatus);
			m_pTmr->StartTimer();
		}
	}
}

void CStgHtrMonitor::ReadHeaterTemp()
{
	int nIOStatus;
	m_dbHighLmt = dREAD_ANALOG(SHT_TmpHgLmtAIO, &nIOStatus);
	m_dbLowLmt = dREAD_ANALOG(SHT_TmpLwLmtAIO, &nIOStatus);
	m_dbTmpRd = dREAD_ANALOG(SHT_TmpRdAI, &nIOStatus);
	m_dbTmpSp = dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	m_nStableSts = dREAD_DIGITAL(SHT_StbStsDM, &nIOStatus);

	m_dbTol = READ_ANALOG(PRMA_TL_SHtrTmp, &nIOStatus);
	m_nStableTm = (int) READ_ANALOG(PRMA_TO_SHtrStb, &nIOStatus);
}
