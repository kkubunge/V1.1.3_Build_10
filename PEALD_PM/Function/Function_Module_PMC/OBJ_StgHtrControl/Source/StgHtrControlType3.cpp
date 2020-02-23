// StgHtrControlType3.cpp: implementation of the CStgHtrControlType3 class.
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

#include "LookupTblFunc.h"
#include "TextParser.h"
#include "Definition.h"
#include "StgHtrControlType3.h"

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "SHT$0_TmpRdAI"			, _K_A_IO	,	0	} ,
	{ "SHT$0_TmpSpAIO"			, _K_A_IO	,	0	} ,
	{ "SHT$0_TmpHgLmtAIO"		, _K_A_IO	,	0	} ,
	{ "SHT$0_TmpLwLmtAIO"		, _K_A_IO	,	0	} ,
	{ "SHT$0_RampRateAIO"		, _K_A_IO	,	0	} ,
	{ "NotDefined"				, _K_A_IO	,	0	} ,
	{ "SHT$0_StbStsDM"			, _K_D_IO	,	0	} ,
	// 2006.01.18
	{ "SHT$0_CalOffsAIO"		, _K_A_IO	,	0	} ,
	//-----------------------------------------------
	{ "SHT$0_TmpSpAI"			, _K_A_IO	,	0	} ,
	{ "SHT$0_RatioAIO"			, _K_A_IO	,	0	} ,

	{ "StgHtr$0RelayDO"			, _K_D_IO	,	0	} ,
	{ "StgHtr$0RelayDI"			, _K_D_IO	,	0	} ,

	{ "PRMA_TO_SHT$0CoDn"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHT$0CoDnTemp"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Temp1"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Temp2"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Temp3"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Temp4"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Temp5"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Temp6"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Ratio1"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Ratio2"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Ratio3"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Ratio4"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Ratio5"		, _K_A_IO	,	0	} ,
	{ "PRMA_SHtr$0Ratio6"		, _K_A_IO	,	0	} ,

	{ "PRMD_SHT$0AutoRat"		, _K_D_IO	,	0	} ,
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStgHtrControlType3::CStgHtrControlType3()
{
//	m_nSHTNum = 0;
}

CStgHtrControlType3::~CStgHtrControlType3()
{

}

BOOL CStgHtrControlType3::Initialize()
{
	BOOL bRet = TRUE;	
	if(! InitIOList(g_pIOList)) printf("-----> InitIOList Failed in %s\n", FUNC_NAME);
	CreateThread();
	return bRet;
}
	
BOOL CStgHtrControlType3::Initialize(char* szSHTNum)
{
	BOOL bRet = TRUE;	

	ReplaceIONameCharSet(g_pIOList, "$0",szSHTNum);

	m_nSHTNum = atoi(szSHTNum);

	if(! InitIOList(g_pIOList)) printf("-----> InitIOList Failed in %s\n", FUNC_NAME);
	CreateThread();
	return bRet;
}


//////////////////////////////////////////////////////////////////////
// Thread Function

void CStgHtrControlType3::DoMonitor()
{
	enum { AutoRatioCtrl_Disable, AutoRatioCtrl_Enable };
	#define LOOKUP_SIZE			6
	LOOKUP_TBL	lookupTbl[LOOKUP_SIZE];
	CLookupTblFunc ltFunc;

	BOOL bIOStatus;
	double dbRdTemp;
	double dbPrevRdTemp = 0;
	double dbRatio;
	int i;

	WaitSec(5);
	while(WaitSec(1))
	{
		// 2007.03.13
		if(IOValid(PRMD_SHTAutoRat) == FALSE
			|| dREAD_DIGITAL(PRMD_SHTAutoRat, &bIOStatus) == AutoRatioCtrl_Disable)
		{
			if(! WaitSec(5)) break;
			continue;
		}

		//--------------------------------------------------------
		// Ratio Auto Control As Lookup Table
		for(i=0; i<LOOKUP_SIZE; i++)
		{
			lookupTbl[i].dbValueA = dREAD_ANALOG(PRMA_SHtrTemp1+i, &bIOStatus);
			lookupTbl[i].dbValueB = dREAD_ANALOG(PRMA_SHtrRatio1+i, &bIOStatus);
			WaitSec(3.0/LOOKUP_SIZE);
		}
		ltFunc.SetTable(lookupTbl, LOOKUP_SIZE);

		if(! WaitSec(1)) break;
		dbRdTemp = dREAD_ANALOG(SHT_TmpRdAI, &bIOStatus);
		if(dbRdTemp != dbPrevRdTemp)
		{
			dbRatio = ltFunc.GetScaleValue(dbRdTemp);
			dWRITE_ANALOG(SHT_RatioAIO, dbRatio, &bIOStatus);
			dbPrevRdTemp = dbRdTemp;
		}
		//--------------------------------------------------------
	}
}

//////////////////////////////////////////////////////////////////////
// Member Function

Module_Status CStgHtrControlType3::CoolDown(char *szParam)
{
	enum { RCMD_OFF, RCMD_ON };
	Module_Status msRet = SYS_ABORTED;
	BOOL bIOStatus;
	double dbTargetTemp;
	double dbRdTemp;
	int nTimeout, nTimeCount;
	CTextParser tp(szParam);

	if(! tp.GetDbFloat(&dbTargetTemp)) dbTargetTemp = dREAD_ANALOG(PRMA_SHTCoDnTemp, &bIOStatus);
	do {
		dWRITE_DIGITAL(SHT_StbStsDM, SHSS_CoolDown, &bIOStatus);
		dWRITE_DIGITAL(StgHtrRelayDO, RCMD_OFF, &bIOStatus);
		if(! WAIT_SECONDS(1)) break;

		nTimeout = (int) dREAD_ANALOG(PRMA_TO_SHTCoDn, &bIOStatus) * 60;
		nTimeCount = 0;
		do {
			dbRdTemp = dREAD_ANALOG(SHT_TmpRdAI, &bIOStatus);
			if(dbRdTemp <= dbTargetTemp) break;
			if(! WAIT_SECONDS(10)) break;
			nTimeCount += 10;
		} while(nTimeCount < nTimeout);
		if(nTimeCount >= nTimeout)
		{
			//ALARM_POST(ALARM_STG_HTR_COOL_DOWN_TIMEOUT);
			ALARM_POST(ALARM_STG_HTR_1_COOL_DOWN_TIMEOUT + m_nSHTNum - 1 );
			break;
		}

		if(SYS_SUCCESS != ResetSetPoint("")) break;
		msRet = SYS_SUCCESS;
	} while(0);
	dWRITE_DIGITAL(SHT_StbStsDM, SHSS_Unstable, &bIOStatus);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CStgHtrControlType3::ResetSetPoint(char *szParam)
{
	enum { RSTS_OFF, RSTS_ON };
	Module_Status msRet = SYS_ABORTED;
	BOOL bIOStatus;
	double dbRdTemp;
	double dbRampRate;

	do {
		if(! WAIT_SECONDS(1)) break;
		if(RSTS_ON == dREAD_DIGITAL(StgHtrRelayDI, &bIOStatus))
		{
			ALARM_POST(ALARM_STG_HTR_1_RELAY_NOT_OFF + m_nSHTNum - 1);
			break;
		}
		dbRdTemp	= dREAD_ANALOG(SHT_TmpRdAI, &bIOStatus);
		dbRampRate	= dREAD_ANALOG(SHT_RampRateRAIO, &bIOStatus);
		dWRITE_ANALOG(SHT_TmpSpAIO, dbRdTemp, &bIOStatus);
		if(! WAIT_SECONDS(1)) break;
		dWRITE_ANALOG(SHT_RampRateRAIO, 0, &bIOStatus);
		if(! WAIT_SECONDS(1)) break;
		dWRITE_ANALOG(SHT_RampRateRAIO, dbRampRate, &bIOStatus);
		msRet = SYS_SUCCESS;
	} while(0);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CStgHtrControlType3::SetRelay(char *szParam)
{
	enum { RCMD_OFF, RCMD_ON	};
	Module_Status msRet = SYS_ABORTED;
	BOOL bIOStatus;
	double dbRdTemp, dbWorkTemp;
	CTextParser tp(szParam);
	tp.GetWord();
	do {
		dbRdTemp = dREAD_ANALOG(SHT_TmpRdAI, &bIOStatus);
		dbWorkTemp = dREAD_ANALOG(SHT_TmpSpAI, &bIOStatus);
		if(tp.BeEqual("ON"))
		{
			if(fabs(dbRdTemp-dbWorkTemp) > 50)
			{
				ALARM_POST(ALARM_STAGE_HTR_1_WORK_TMP_ERROR + m_nSHTNum - 1);
				break;
			}
			dWRITE_DIGITAL(StgHtrRelayDO, RCMD_ON, &bIOStatus);
			if(! bIOStatus) break;
		}
		else if(tp.BeEqual("OFF"))
		{
			dWRITE_DIGITAL(StgHtrRelayDO, RCMD_OFF, &bIOStatus);
			if(! bIOStatus) break;
		}
		msRet = SYS_SUCCESS;
	} while(0);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CStgHtrControlType3::SetTemp(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	
	int nCurrentTempSet = 0;
	int nCurrentOffset	= 0;
	int nCurrentRamp	= 0;

	int nNewTempSet		= 0;
	int nNewRamp		= -1;
	int nNewOffset		= 0;

	BOOL bOffset		= FALSE;

	//////////////////////////////////////////////////////////////////////////
	//... Parsing Command Parameter
	CTextParser tp(szParam);

	tp.GetInt(&nNewTempSet);
	tp.GetInt(&nNewRamp);
	bOffset = tp.GetInt(&nNewOffset);

	do
	{
		//////////////////////////////////////////////////////////////////////////
		//... Set RampRate
		if(nNewRamp >= 0)	dWRITE_ANALOG(SHT_RampRateRAIO, (double) nNewRamp, &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		//... Set Offset
		nCurrentTempSet = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
		
		//... Check Offset Used
		if(bOffset)
		{
			nCurrentOffset = (int) dREAD_ANALOG(SHT_CalOffsAIO, &nIOStatus);
			if(nNewOffset != nCurrentOffset)	dWRITE_ANALOG(SHT_CalOffsAIO, (double) nNewOffset, &nIOStatus);
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check Heater Temp Status
		if((nCurrentTempSet != nNewTempSet) || ((nCurrentOffset != nNewOffset) && bOffset))
		{
			dWRITE_DIGITAL(SHT_StbStsDM, SHSS_Moving, &nIOStatus);
		}

		//////////////////////////////////////////////////////////////////////////
		//... Set temp
		dWRITE_ANALOG(SHT_TmpSpAIO, (double) nNewTempSet, &nIOStatus);

	}while(0);

	return msRet;
}