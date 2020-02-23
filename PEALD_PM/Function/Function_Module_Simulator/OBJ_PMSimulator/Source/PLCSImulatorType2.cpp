// PLCSimulatorType2.cpp: implementation of the CPLCSimulatorType2 class.
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
#include "PLCSimulatorType2.h"

// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

#define MAX_VALVE_BYTE		14
#define VR_HDR_BASE			2
#define VRHD_MFC_SP_IDX		10
#define VRHD_MFC_VL_IDX		58
#define VRHD_RF_SP_IDX		64
#define VRHD_RF2_SP_IDX		66
#define VRHD_HTE_SP_IDX		68
#define STEP_TIME_SP_IDX    98


#define LINE_STEP_BASE		18
#define LINE_STEP_SIZE		16

#define BIT_PAUSE			0x01
#define BIT_TIME0			0x02
#define BIT_NEXT			0x04
#define BIT_EXEC_FLAG		0x01

enum { PLC_VRCP_STOP, PLC_VRCP_RUN, PLC_VRCP_FAILED, PLC_VRCP_PAUSED };
enum { VTS_10MS, VTS_100MS };

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	FastRoughVlvDO,

	RFSetPointAO,
	RFPwOnDO,
	RF2SetPointAO,
	RF2PwOnDO,
	RPGPwOnDO,
	RFLoadPowAI,
	RFLoadPow2AI,

	MFC01StPt,
	MFC02StPt,
	MFC03StPt,
	MFC04StPt,
	MFC05StPt,
	MFC06StPt,
	MFC07StPt,
	MFC08StPt,
	MFC09StPt,
	MFC10StPt,
	MFC11StPt,
	MFC12StPt,
	MFC13StPt,
	MFC14StPt,
	MFC15StPt,
	MFC16StPt,
	MFC17StPt,
	MFC18StPt,
	MFC19StPt,
	MFC20StPt,

	MFC01Pg,
	MFC02Pg,
	MFC03Pg,
	MFC04Pg,
	MFC05Pg,
	MFC06Pg,
	MFC07Pg,
	MFC08Pg,
	MFC09Pg,
	MFC10Pg,
	MFC11Pg,
	MFC12Pg,
	MFC13Pg,
	MFC14Pg,
	MFC15Pg,
	MFC16Pg,
	MFC17Pg,
	MFC18Pg,
	MFC19Pg,
	MFC20Pg,

	VRCP_StaStpDO,
	VRCP_GoNextDO,

	VRCP_RunningDI,
	VRCP_TgStpAI, 
	VRCP_StpCntAI, 
	VRCP_TgCycAI, 
	VRCP_LTgStpAI, 
	VRCP_CycCntAI, 
	VRCP_LStpCntAI,
	VRCP_PauseDI,
	VRCP_DnLdErr1DI,
	VRCP_DnLdErr2DI,

	PLC_CurStepAI,
	PLC_CurCycAI,
	PLC_RCPRunTMAI,
	PLC_GbIntlckDI,
	PLC_RstIntLckDO,
	RPGGateVlvPosDI,
	RPGGateVlv2PosDI,
	RPGGateVv1DO,
	RPGGateVv2DO,
	HTE_SetTempAIO,
	HTE_ReadTempAI,
	HTE_StaStpDIO,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPLCSimulatorType2::CPLCSimulatorType2()
{

}

CPLCSimulatorType2::~CPLCSimulatorType2()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CPLCSimulatorType2::ExcuteVRCPLine(unsigned char *pData)
{
	BOOL bRet = TRUE;
	double dbDelayTime;
	int nValveNum;
	int i, j;
	int nIdx;
	static int pnValveByteIdx[MAX_VALVE_BYTE] = { 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 };

	do {
		// Read Parameter
		if(m_nValveTimeRes == VTS_10MS)		dbDelayTime = ((double) (pData[0]*256 + pData[1])) /100;
		else								dbDelayTime = ((double) (pData[0]*256 + pData[1])) /10;

// 		printf("-------> Time:%5.2fsec VRCPData: %02X %02X %02X %02X %02X %02X %02X %02X in PLCSimulator\n",
// 			dbDelayTime,
// 			pData[0], pData[1], pData[2], pData[3],
// 			pData[4], pData[5], pData[6], pData[7]);
	
		for(j=0; j<MAX_VALVE_BYTE; j++)
		{
			for(i=0; i<8; i++)
			{
				nIdx = pnValveByteIdx[j];
				nValveNum = j*8+i+1;
				if( (pData[nIdx] & (0x01 << i)) == (0x01 << i) )	SetValveStatus(nValveNum, 1);		// 0:Close 1:Open
				else												SetValveStatus(nValveNum, 0);
			}
		}

		if(dbDelayTime == 0)
		{
			_LOG("-----> ValveRecipe Time is 0");
			bRet = FALSE;
		}

		if(! WaitPLCTm(dbDelayTime))	bRet = FALSE;		// 2008.01.11

	} while(0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
void CPLCSimulatorType2::SetMFC()
{
	int i;
	double dbMfcValue;	// 2006.01.12
	int nIOStatus;
	unsigned char byValve, byBitMask;

	for(i=0; i<MAX_MFC; i++)
	{
		if(m_pnMfcScale[i] != 0)
		{
			//////////////////////////////////////////////////////////////////////////
			dbMfcValue = m_VR_Header[m_nRunPage][VRHD_MFC_SP_IDX + i*2] ;
			dbMfcValue *= 256;
			dbMfcValue += (unsigned char) m_VR_Header[m_nRunPage][VRHD_MFC_SP_IDX + i*2 + 1];
			dbMfcValue = dbMfcValue * m_pnMfcScale[i] / m_pnMfcPhyMax[i];
			dWRITE_ANALOG(MFC01StPt + i, dbMfcValue, &nIOStatus);

			//////////////////////////////////////////////////////////////////////////
			int nBufIdx = i/4;
			int nBitIdx = i%4;
			if( (nBufIdx & 0x01) == 0 ) nBufIdx++;
			else nBufIdx--;
			byBitMask = 0x03;
			byValve = m_VR_Header[m_nRunPage][VRHD_MFC_VL_IDX + nBufIdx] & (byBitMask << (nBitIdx * 2));
			byValve >>= (nBitIdx * 2);
			dWRITE_DIGITAL(MFC01Pg + i, byValve, &nIOStatus);
			//////////////////////////////////////////////////////////////////////////
		}
	}
}

void CPLCSimulatorType2::SetRF()		// 2006.03.06
{
	BOOL bIOStatus;
	double dbRFSetPoint;
	dbRFSetPoint = m_VR_Header[m_nRunPage][VRHD_RF_SP_IDX];
	dbRFSetPoint *= 256;
	dbRFSetPoint += (unsigned char) m_VR_Header[m_nRunPage][VRHD_RF_SP_IDX+1];
	dbRFSetPoint *= ((double)m_nRfScale / m_nRfPhyMax);
	dWRITE_ANALOG(RFSetPointAO, dbRFSetPoint, &bIOStatus);
	dWRITE_ANALOG(RFLoadPowAI, dbRFSetPoint, &bIOStatus);
}

void CPLCSimulatorType2::SetRF2()	// 2014.04.09
{
	BOOL bIOStatus;
	double dbRFSetPoint;
	dbRFSetPoint = m_VR_Header[m_nRunPage][VRHD_RF2_SP_IDX];
	dbRFSetPoint *= 256;
	dbRFSetPoint += (unsigned char) m_VR_Header[m_nRunPage][VRHD_RF2_SP_IDX+1];
	dbRFSetPoint *= ((double)m_nRfScale / m_nRfPhyMax);
	dWRITE_ANALOG(RF2SetPointAO, dbRFSetPoint, &bIOStatus);
	dWRITE_ANALOG(RFLoadPow2AI, dbRFSetPoint, &bIOStatus);
}

void CPLCSimulatorType2::SetHTE()	// 2014.04.09
{
	BOOL bIOStatus;
	double dbSetPoint;
	dbSetPoint = m_VR_Header[m_nRunPage][VRHD_HTE_SP_IDX];
	dbSetPoint *= 256;
	dbSetPoint += (unsigned char) m_VR_Header[m_nRunPage][VRHD_HTE_SP_IDX+1];
	dbSetPoint *= ((double)100 / 2047);
	dWRITE_ANALOG(HTE_SetTempAIO, dbSetPoint, &bIOStatus);
	dWRITE_ANALOG(HTE_ReadTempAI, dbSetPoint, &bIOStatus);
}

//////////////////////////////////////////////////////////////////////////
//... Run Recipe Step
Module_Status CPLCSimulatorType2::RunValveRecipe()
{
	Module_Status msRet = SYS_ABORTED;
	int nIOStatus;
	BOOL bAbort = FALSE;
	int i, j, k, n;

	int nTotalCyc;
	int nTotalStep;
	int nStepCycle;
	int nTotalLineStep;
	int nPLC_CurStepAI;
	char szLog[200];

	m_pTimer->StartTimer();		// 2006.07.31
	m_dbVrcpProgress = 0;		// 2006.07.31

	m_bGoNext = FALSE;
	nTotalCyc = (int) m_VR_Header[m_nRunPage][VR_HDR_BASE+6] * 256
					+ m_VR_Header[m_nRunPage][VR_HDR_BASE+7];
	nTotalStep = m_VR_Step[m_nRunPage][0][3];
	dWRITE_ANALOG(VRCP_TgStpAI, nTotalStep, &nIOStatus);

	nPLC_CurStepAI = m_VR_Header[m_nRunPage][VR_HDR_BASE+1];
	dWRITE_ANALOG(PLC_CurStepAI, nPLC_CurStepAI, &nIOStatus);
	SetMFC();

	do {
		if( (m_VR_Header[m_nRunPage][VR_HDR_BASE+3] & BIT_TIME0) == BIT_TIME0 )
		{
			sprintf(szLog, "-----===> [ProStep:%d] This is Time 0 Step", nPLC_CurStepAI);
			_LOG(szLog); printf("%s\n", szLog);
			msRet = SYS_SUCCESS;
			break;
		}

		if( (m_VR_Header[m_nRunPage][VR_HDR_BASE+3] & BIT_NEXT) == BIT_NEXT )
		{
			nTotalCyc = 9999999;
			sprintf(szLog, "-----===> [ProStep:%d] This is Time Unlimied Step in PLCSimulator", nPLC_CurStepAI);
			_LOG(szLog); printf("%s\n", szLog);
		}

		sprintf(szLog, "-----===> [ProStep:%d] started in PLCSimulator", nPLC_CurStepAI);
		_LOG(szLog); printf("%s\n", szLog);

		for(n=0; n<nTotalCyc; n++)
		{
			dWRITE_ANALOG(PLC_CurCycAI, n+1, &nIOStatus);
			for(i=0; i<nTotalStep; i++)
			{
				dWRITE_ANALOG(VRCP_StpCntAI, i+1, &nIOStatus);

				nStepCycle = m_VR_Step[m_nRunPage][i][8]*256 + m_VR_Step[m_nRunPage][i][9];
				dWRITE_ANALOG(VRCP_TgCycAI, nStepCycle, &nIOStatus);

				nTotalLineStep = m_VR_Step[m_nRunPage][i][11];
				dWRITE_ANALOG(VRCP_LTgStpAI, nTotalLineStep, &nIOStatus);

				sprintf(szLog, "-----=====> Step:%d TotalStepCycle:%d TotalLineStep:%d", i+1, nStepCycle, nTotalLineStep);
				_LOG(szLog); printf("%s\n", szLog);
				for(j=0; j<nStepCycle; j++)
				{
					dWRITE_ANALOG(VRCP_CycCntAI, j+1, &nIOStatus);
					for(k=0; k<nTotalLineStep; k++)
					{
						dWRITE_ANALOG(VRCP_LStpCntAI, k+1, &nIOStatus);
						sprintf(szLog, "-----=====> ExecVRCP[Step:%d Cycle:%d Line:%d] in PLCSimulator", i+1, j+1, k+1);
						_LOG(szLog); printf("%s\n", szLog);
						if(FALSE == ExcuteVRCPLine(m_VR_Step[m_nRunPage][i]+LINE_STEP_BASE+k*LINE_STEP_SIZE)) bAbort = TRUE;
						if(bAbort || m_bGoNext) break;
					}
					if(bAbort || m_bGoNext) break;
				}
				if(bAbort || m_bGoNext) break;
			}
			if(bAbort || m_bGoNext) break;
		}
		if(bAbort) break;

		msRet = SYS_SUCCESS;
	} while(0);
	m_pTimer->StopTimer();		// 2008.01.15

	if(msRet != SYS_SUCCESS) {
		sprintf(szLog, "-----> Aborted Valve Recipe in PLCSimulator");
		_LOG(szLog); printf("%s\n", szLog);
		
	}
	else
	{
		if( (m_VR_Header[m_nRunPage][VR_HDR_BASE+3] & BIT_PAUSE) == BIT_PAUSE )
		{
			m_nPLC_VrcpSts = PLC_VRCP_PAUSED;
			sprintf(szLog, "---> PLC Paused in PLCSimulator");
			_LOG(szLog); printf("%s\n", szLog);
		}
	}
	return msRet;
}
