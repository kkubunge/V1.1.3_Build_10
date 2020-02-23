// StgHtrControl.cpp: implementation of the CStgHtrControl class.
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

#include "TextParser.h"
#include "Definition.h"
#include "StgHtrControl.h"

//--------------------------------------------------------------------
// #include "DynamicIO.h"
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
	{ "SHT_StbStsDM"		, _K_D_IO	,	0	} ,
	// 2006.01.18
	{ "SHT_CalOffsAIO"		, _K_A_IO	,	0	} ,
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStgHtrControl::CStgHtrControl()
{
	m_nSHTNum = 1;	// default value '1' - 2007.04.24 ijbae.
}

CStgHtrControl::~CStgHtrControl()
{

}

BOOL CStgHtrControl::Initialize()
{
	BOOL bRet = TRUE;
	if(! InitIOList(g_pIOList)) printf("-----> InitIOList Failed in %s\n", FUNC_NAME);
	return bRet;
}

BOOL CStgHtrControl::Initialize(char *szSHTNum)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Member Function

Module_Status CStgHtrControl::CheckSetPoint(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nSetPoint;
	int nOffset;
	int nCurTemp;
	CTextParser tp(szParam);
	tp.GetInt(&nSetPoint);
	nCurTemp = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	if(nCurTemp != nSetPoint)
	{
		//ALARM_POST(ALARM_STG_HTR_TEMP_SET_INVALID);
		ALARM_POST(ALARM_STG_HTR_1_TEMP_SET_INVALID + m_nSHTNum - 1 );
		msRet = SYS_ERROR; 
	}
	if(SHSS_Stable != dREAD_DIGITAL(SHT_StbStsDM, &nIOStatus))
	{
		//ALARM_POST(ALARM_STG_HTR_TEMP_NOT_STABLE);
		ALARM_POST(ALARM_STG_HTR_1_TEMP_NOT_STABLE + m_nSHTNum - 1 );
		msRet = SYS_ERROR;
	}

	// 2006.02.27
	if(tp.GetInt(&nOffset))
	{
		if(nOffset != (int) dREAD_ANALOG(SHT_CalOffsAIO, &nIOStatus))
		{
			//ALARM_POST(ALARM_STG_HTR_OFFSET_INVALID);
			ALARM_POST(ALARM_STG_HTR_1_OFFSET_INVALID + m_nSHTNum - 1 );
			msRet = SYS_ERROR;
		}
	}
	return msRet;
}

Module_Status CStgHtrControl::CheckSetPoint2(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nSetPoint;
	int nOffset;
	int nCurTemp;
	CTextParser tp(szParam);
	tp.GetInt(&nSetPoint);
	nCurTemp = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	if(nCurTemp != nSetPoint)
	{
		// ALARM_POST(ALARM_STG_HTR_TEMP_SET_INVALID);
		msRet = SYS_ERROR; 
	}
	if(SHSS_Stable != dREAD_DIGITAL(SHT_StbStsDM, &nIOStatus))
	{
		// ALARM_POST(ALARM_STG_HTR_TEMP_NOT_STABLE);
		msRet = SYS_ERROR;
	}

	// 2006.02.27
	if(tp.GetInt(&nOffset))
	{
		if(nOffset != (int) dREAD_ANALOG(SHT_CalOffsAIO, &nIOStatus))
		{
			// ALARM_POST(ALARM_STG_HTR_OFFSET_INVALID);
			msRet = SYS_ERROR;
		}
	}
	return msRet;
}

Module_Status CStgHtrControl::CheckTemp(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nSetPoint;
	int nCurTemp;
	int nTol = 5;
	int nDiff;

	nTol = (int) READ_ANALOG(PRMA_TL_SHtrTmp, &nIOStatus);
	nSetPoint = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	nCurTemp = (int) dREAD_ANALOG(SHT_TmpRdAI, &nIOStatus);
	nDiff = abs(nCurTemp - nSetPoint);
	if(nDiff > nTol) msRet = SYS_ERROR;
	
	if(msRet == SYS_SUCCESS)
	{
		char szBuf[40];
		CTextParser tp(szParam);
		tp.GetWord(szBuf);
		tp.MakeLower(szBuf);
		if(strcmp(szBuf, "stable") == 0)
		{
			if(SHSS_Stable != dREAD_DIGITAL(SHT_StbStsDM, &nIOStatus)) msRet = SYS_ERROR;
		}
	}
	return msRet;
}

Module_Status CStgHtrControl::CheckUpper(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nSetPoint;
	int nCurTemp;
	int nTol = 5;
	CTextParser tp(szParam);
	nTol = (int) READ_ANALOG(PRMA_TL_SHtrTmp, &nIOStatus);
	nSetPoint = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	nCurTemp = (int) dREAD_ANALOG(SHT_TmpRdAI, &nIOStatus);
	if(nCurTemp < nSetPoint - nTol) msRet = SYS_ERROR;
	return msRet;
}

Module_Status CStgHtrControl::CheckLower(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nSetPoint;
	int nCurTemp;
	int nTol = 5;
	CTextParser tp(szParam);
	nTol = (int) READ_ANALOG(PRMA_TL_SHtrTmp, &nIOStatus);
	nSetPoint = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	nCurTemp = (int) dREAD_ANALOG(SHT_TmpRdAI, &nIOStatus);
	if(nCurTemp > nSetPoint + nTol) msRet = SYS_ERROR;
	return msRet;
}


Module_Status CStgHtrControl::SetTemp(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nNewSP;
	int nSetPoint;
	int nOffset;
	int nNewOffs;
	bool bOffset = false;
	int nRamp = 0;
	int nNewRamp = 0;

	CTextParser tp(szParam);
	tp.GetInt(&nNewSP);
	tp.GetInt(&nNewRamp);
	bOffset = tp.GetInt(&nNewOffs);

	//-------------------------------------------------------------------
	// 2006.11.18
	// Set Temp & Offset
	nSetPoint = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	if(nSetPoint != nNewSP) dWRITE_ANALOG(SHT_TmpSpAIO, (double) nNewSP, &nIOStatus);

	if(bOffset)
	{
		nOffset = (int) dREAD_ANALOG(SHT_CalOffsAIO, &nIOStatus);
		if(nNewOffs != nOffset) dWRITE_ANALOG(SHT_CalOffsAIO, (double) nNewOffs, &nIOStatus);
	}

	if(nSetPoint != nNewSP || (nOffset != nNewOffs && bOffset))
				dWRITE_DIGITAL(SHT_StbStsDM, SHSS_Moving, &nIOStatus);
	//-------------------------------------------------------------------

	// Set RampRate
	if(nNewRamp >= 0)
	{
		nRamp = (int) dREAD_ANALOG(SHT_RampRateRAIO, &nIOStatus);
		if(nRamp != nNewRamp) dWRITE_ANALOG(SHT_RampRateRAIO, (double) nNewRamp, &nIOStatus);

		nRamp = (int) dREAD_ANALOG(SHT_RampRateFAIO, &nIOStatus);
		if(nRamp != nNewRamp) dWRITE_ANALOG(SHT_RampRateFAIO, (double) nNewRamp, &nIOStatus);
	}
	return msRet;
}

Module_Status CStgHtrControl::SetIfUpper(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nSetPoint;
	int nNewSP;
	CTextParser tp(szParam);
	tp.GetInt(&nNewSP);
	nSetPoint = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	if(nSetPoint > nNewSP)
	{
		dWRITE_DIGITAL(SHT_StbStsDM, SHSS_Moving, &nIOStatus);		// 0:Stable 1:Unstable 2:Moving
		dWRITE_ANALOG(SHT_TmpSpAIO, (double) nNewSP, &nIOStatus);
	}

	// Set RampRate
	int nRamp = 0;
	int nNewRamp = 0;
	tp.GetInt(&nNewRamp);
	if(nNewRamp >= 0)
	{
		nRamp = (int) dREAD_ANALOG(SHT_RampRateRAIO, &nIOStatus);
		if(nRamp != nNewRamp) dWRITE_ANALOG(SHT_RampRateRAIO, (double) nNewRamp, &nIOStatus);

		nRamp = (int) dREAD_ANALOG(SHT_RampRateFAIO, &nIOStatus);
		if(nRamp != nNewRamp) dWRITE_ANALOG(SHT_RampRateFAIO, (double) nNewRamp, &nIOStatus);
	}
	return msRet;
}

Module_Status CStgHtrControl::SetIfLower(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nSetPoint;
	int nNewSP;
	CTextParser tp(szParam);
	tp.GetInt(&nNewSP);
	nSetPoint = (int) dREAD_ANALOG(SHT_TmpSpAIO, &nIOStatus);
	if(nSetPoint < nNewSP)
	{
		dWRITE_DIGITAL(SHT_StbStsDM, SHSS_Moving, &nIOStatus);		// 0:Stable 1:Unstable 2:Moving
		dWRITE_ANALOG(SHT_TmpSpAIO, (double) nNewSP, &nIOStatus);
	}

	// Set RampRate
	int nRamp = 0;
	int nNewRamp = 0;
	tp.GetInt(&nNewRamp);
	if(nNewRamp >= 0)
	{
		nRamp = (int) dREAD_ANALOG(SHT_RampRateRAIO, &nIOStatus);
		if(nRamp != nNewRamp) dWRITE_ANALOG(SHT_RampRateRAIO, (double) nNewRamp, &nIOStatus);

		nRamp = (int) dREAD_ANALOG(SHT_RampRateFAIO, &nIOStatus);
		if(nRamp != nNewRamp) dWRITE_ANALOG(SHT_RampRateFAIO, (double) nNewRamp, &nIOStatus);
	}
	return msRet;
}

Module_Status CStgHtrControl::IsUpper(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nTemp;
	nTemp = atoi(szParam);
	if(dREAD_ANALOG(SHT_TmpRdAI, &nIOStatus) < nTemp) msRet = SYS_ERROR;
	return msRet;
}

Module_Status CStgHtrControl::IsLower(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nTemp;
	nTemp = atoi(szParam);
	if(dREAD_ANALOG(SHT_TmpRdAI, &nIOStatus) > nTemp) msRet = SYS_ERROR;
	return msRet;
}