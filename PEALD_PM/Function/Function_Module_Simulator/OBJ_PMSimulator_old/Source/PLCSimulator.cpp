// PLCSimulator.cpp: implementation of the CPLCSimulator class.
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
#include "PLCSimulator.h"

// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

#define MAX_VALVE_BYTE		6
#define VR_HDR_BASE			2
#define VRHD_MFC_SP_IDX		10
#define VRHD_MFC_VL_IDX		58
#define VRHD_RF_SP_IDX		64
#define VRHD_RF2_SP_IDX		66
#define VRHD_HTE_SP_IDX		68
#define STEP_TIME_SP_IDX    98

#define LINE_STEP_BASE		12
#define LINE_STEP_SIZE		8

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

	//... 2015.04.08 Add RF Forward/Reflect Count Up
	RF1LoadCntAIO,
	RF2LoadCntAIO,
	RF1ReflCntAI,
	RF2ReflCntAI,

} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "FastRoughVlvDO",	_K_D_IO	,	0},

	{ "RFSetPointAO",	_K_A_IO	,	0 },
	{ "RFPwOnDO",		_K_D_IO	,	0},
	{ "RFSetPoint2AO",	_K_A_IO	,	0},
	{ "RFPwOn2DO",		_K_D_IO	,	0},
	{ "RPGPwOnDO",		_K_D_IO	,	0},
	{ "RFLoadPowAI",	_K_A_IO	,	0},
	{ "RFLoadPow2AI",	_K_A_IO	,	0},

	{ "MFC01StPtAO"	, _K_A_IO	,	0 },
	{ "MFC02StPtAO"	, _K_A_IO	,	0 },
	{ "MFC03StPtAO"	, _K_A_IO	,	0 },
	{ "MFC04StPtAO"	, _K_A_IO	,	0 },
	{ "MFC05StPtAO"	, _K_A_IO	,	0 },
	{ "MFC06StPtAO"	, _K_A_IO	,	0 },
	{ "MFC07StPtAO"	, _K_A_IO	,	0 },
	{ "MFC08StPtAO"	, _K_A_IO	,	0 },
	{ "MFC09StPtAO"	, _K_A_IO	,	0 },
	{ "MFC10StPtAO"	, _K_A_IO	,	0 },
	{ "MFC11StPtAO"	, _K_A_IO	,	0 },
	{ "MFC12StPtAO"	, _K_A_IO	,	0 },
	{ "MFC13StPtAO"	, _K_A_IO	,	0 },
	{ "MFC14StPtAO"	, _K_A_IO	,	0 },
	{ "MFC15StPtAO"	, _K_A_IO	,	0 },
	{ "MFC16StPtAO"	, _K_A_IO	,	0 },
	{ "MFC17StPtAO"	, _K_A_IO	,	0 },
	{ "MFC18StPtAO"	, _K_A_IO	,	0 },
	{ "MFC19StPtAO"	, _K_A_IO	,	0 },
	{ "MFC20StPtAO"	, _K_A_IO	,	0 },

	{ "MFC01PgDO"	, _K_D_IO	,	0 },
	{ "MFC02PgDO"	, _K_D_IO	,	0 },
	{ "MFC03PgDO"	, _K_D_IO	,	0 },
	{ "MFC04PgDO"	, _K_D_IO	,	0 },
	{ "MFC05PgDO"	, _K_D_IO	,	0 },
	{ "MFC06PgDO"	, _K_D_IO	,	0 },
	{ "MFC07PgDO"	, _K_D_IO	,	0 },
	{ "MFC08PgDO"	, _K_D_IO	,	0 },
	{ "MFC09PgDO"	, _K_D_IO	,	0 },
	{ "MFC10PgDO"	, _K_D_IO	,	0 },
	{ "MFC11PgDO"	, _K_D_IO	,	0 },
	{ "MFC12PgDO"	, _K_D_IO	,	0 },
	{ "MFC13PgDO"	, _K_D_IO	,	0 },
	{ "MFC14PgDO"	, _K_D_IO	,	0 },
	{ "MFC15PgDO"	, _K_D_IO	,	0 },
	{ "MFC16PgDO"	, _K_D_IO	,	0 },
	{ "MFC17PgDO"	, _K_D_IO	,	0 },
	{ "MFC18PgDO"	, _K_D_IO	,	0 },
	{ "MFC19PgDO"	, _K_D_IO	,	0 },
	{ "MFC20PgDO"	, _K_D_IO	,	0 },

	{ "VRCP_StaStpDO"	, _K_D_IO, 0} ,
	{ "VRCP_GoNextDO"	, _K_D_IO, 0} ,
	{ "VRCP_RunningDI"	, _K_D_IO, 0} ,
	{ "VRCP_TgStpAI",	_K_A_IO,   0},
	{ "VRCP_StpCntAI",	_K_A_IO,   0},
	{ "VRCP_TgCycAI",	_K_A_IO,   0},
	{ "VRCP_LTgStpAI",	_K_A_IO,   0},
	{ "VRCP_CycCntAI",	_K_A_IO,   0},
	{ "VRCP_LStpCntAI",	_K_A_IO,   0},
	{ "VRCP_PauseDI",	_K_D_IO,   0},
	{ "VRCP_DnLdErr1DI", _K_D_IO,  0},
	{ "VRCP_DnLdErr2DI", _K_D_IO,  0},

	{ "PLC_CurStepAI",	_K_A_IO,   0},
	{ "PLC_CurCycAI",	_K_A_IO,   0},
	{ "PLC_RCPRunTMAI",	_K_A_IO,   0} ,
	{ "PLC_GbIntlckDI", _K_D_IO,  0},
	{ "PLC_RstIntLckDO", _K_D_IO,  0},

	{ "RPGGateVlvPosDI",	_K_D_IO	, 0},
	{ "RPGGateVlv2PosDI",	_K_D_IO	, 0},
	{ "V103ValveDO",		_K_D_IO	, 0},
	{ "V111ValveDO",		_K_D_IO	, 0},

	{ "HTE_SetTempAO"	, _K_A_IO	,	0 } ,
	{ "HTE_ReadTempAI"	, _K_A_IO	,	0 } ,
	{ "HTE_SrtStpDO"	, _K_D_IO	,	0 } ,

	//... 2015.04.08 Add RF Forward/Reflect Count Up
	{ "RF1LoadCntAIO"	, _K_A_IO	,	0 },
	{ "RF2LoadCntAIO"	, _K_A_IO	,	0 },
	{ "RF1ReflCntAI"	, _K_A_IO	,	0 },
	{ "RF2ReflCntAI"	, _K_A_IO	,	0 },

	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPLCSimulator::CPLCSimulator()
{
	m_nRunStep = 0;
	m_nRunPage = 0;
	m_bGoNext = FALSE;
	m_nPLC_VrcpSts = PLC_VRCP_STOP;
	m_nValveTimeRes = VTS_10MS;

	int i;
	for(i=0; i<MAX_VALVE_IO; i++)
	{
		sprintf(m_ValveList[i].szIOName, "NotDefined");	// 2006.09.12
		m_ValveList[i].nIOType = _K_D_IO;
		m_ValveList[i].nIOPoint = -1;
	}
	strcpy(m_ValveList[i].szIOName, "");

	m_nRfScale = 0;
	m_nRfPhyMax = 8000;

	m_pTimer = new CTimer();		// 2006.07.31
}

CPLCSimulator::~CPLCSimulator()
{
	delete m_pTimer;				// 2006.07.31
}

BOOL CPLCSimulator::Initialize()
{
	int i;
	for(i=0; i<MAX_MFC; i++) { m_pnMfcScale[i] = 0; m_pnMfcPhyMax[i] = 0; }
	m_VR_Header[0][VR_HDR_BASE+2] = 0x01;
	m_VR_Header[1][VR_HDR_BASE+2] = 0x01;
	ReadSimCfg();

	InitIOList(g_pIOList);
	InitValveIO();
	CreateThread_B();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CPLCSimulator::ReadSimCfg()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(SIM_CFG_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("MFC", szItem) == 0)				nTitle = 1;
				else if(strcmp("Valve List", szItem) == 0)	nTitle = 2;
				else if(strcmp("Valve Recipe Configration", szItem) == 0) nTitle = 3;
				else if(strcmp("RF", szItem) == 0)			nTitle = 4;		// 2006.03.06
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseMfcCfg(szRead); break;
					case 2: ParseValveListCfg(szRead); break;
					case 3: ParseVrcpConfig(szRead); break;
					case 4: ParseRFCfg(szRead);break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else
			{
				if(NULL == fgets(szRead, 255, fp)) break;
			}
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	return bRet;
}

void CPLCSimulator::ParseVrcpConfig(char *szData)
{
	char szItem[40];
	char szValue[40];

	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "ValveTimeResolution") == 0)
		{
			tp.GetWord(szValue);
			if(strcmp(szValue, "10ms") == 0) m_nValveTimeRes = VTS_10MS;
			else if(strcmp(szValue, "100ms") == 0) m_nValveTimeRes = VTS_100MS;
		}
	} while(0);
}

void CPLCSimulator::ParseMfcCfg(char *szParam)
{
	char szItem[40] = "";
	CTextParser tp(szParam);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "Scale") == 0 || strcmp(szItem, "MfcConfig") == 0)
		{
			int nMfcN, nMfcIdx; 
			tp.GetInt(& nMfcN);
			nMfcIdx = nMfcN - 1;
			tp.GetInt(& m_pnMfcScale[nMfcIdx]);
			tp.GetInt(& m_pnMfcPhyMax[nMfcIdx]);

			char szLog[200];
			sprintf(szLog, "SimCfg => MFC[%d] Max = %d sccm, PhyMax = %d",
				nMfcN, m_pnMfcScale[nMfcIdx], m_pnMfcPhyMax[nMfcIdx]);
			_LOG(szLog);
		}
	} while(0);
}

void CPLCSimulator::ParseValveListCfg(char *szParam)
{
	CTextParser tp(szParam);
	int nBit;
	do {
		if(! tp.GetInt(&nBit)) break;
		if(nBit>=0 && nBit<MAX_VALVE_IO) tp.GetWord(m_ValveList[nBit].szIOName);
	} while(0);
}

void CPLCSimulator::ParseRFCfg(char *szParam)		// 2006.03.06
{
	char szItem[40] = "";
	CTextParser tp(szParam);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "Scale") == 0)
		{
			tp.GetInt(& m_nRfScale);
			tp.GetInt(& m_nRfPhyMax);
		}
	} while(0);
}

Module_Status CPLCSimulator::SetValveData(char *pParam)
{
	Module_Status msRet = SYS_SUCCESS;
	unsigned int unData;
	int i;
	unsigned char pData[8];
	CTextParser tp;

	do {
		// Read Parameter
		tp.SetText(pParam);
		for(i=0; i<8; i++)
		{
			tp.GetHexa(&unData, 0);
			pData[i] = unData;
		}
		ExcuteVRCPLine(pData);
	} while(0);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CPLCSimulator::Download(char *pParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nPage;
	int nStep;
	int nDataIdx;
	int nSize;
	int nIdx;
	unsigned int unData;
	int i;
	char szLog[1000];
	char szBuf[40];
	CTextParser tp;

//	_LOG(pParam);
	do {
		// Read Parameter
		tp.SetText(pParam);
		tp.GetInt(&nPage, 0);
		tp.GetInt(&nStep, 0);
		tp.GetInt(&nDataIdx, 0);
		tp.GetInt(&nSize, 0);
		tp.Token(':');		// Read Data Start Tag

		if(nStep > 0)
		{
			nIdx = nStep-1;
			sprintf(szLog, "Data[Page:%d Step:%d Idx:%d Size:%d] : ", nPage, nStep, nDataIdx, nSize);
			for(i = 0; i < nSize; i++)
			{
				unData = 0;
				if(FALSE == tp.GetHexa(&unData, 0)) break;
				if(nDataIdx+i < VR_STEP_SIZE)
				{
					m_VR_Step[nPage][nIdx][nDataIdx+i] = unData;
					sprintf(szBuf, " %02X", unData);
					strcat(szLog, szBuf);
				}
			}
			_LOG(szLog);
		}
		else
		{
			sprintf(szLog, "Data[Page:%d Step:%d(Header) Idx:%d Size:%d] : ", nPage, nStep, nDataIdx, nSize);
			for(i=0; i<nSize; i++)
			{
				unData = 0;
				if(FALSE == tp.GetHexa(&unData, 0)) break;
				if(nDataIdx+i < VR_STEP_SIZE)
				{
					m_VR_Header[nPage][nDataIdx+i] = unData;
					sprintf(szBuf, " %02X", unData);
					strcat(szLog, szBuf);
				}
			}
			_LOG(szLog);
		}
	} while(0);

	return msRet;
}

Module_Status CPLCSimulator::CloseAllValve()
{
	Module_Status msRet = SYS_SUCCESS;
	int i;
	for(i=0; i<MAX_VALVE_IO; i++) SetValveStatus(i+1, 0);	// 0:Close 1:Open
	return msRet;
}

Module_Status CPLCSimulator::StartValveRecipe()
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;

	_LOG("StartValveRecipe Called");

	dWRITE_DIGITAL(VRCP_DnLdErr1DI, 0, &nIOStatus);
	dWRITE_DIGITAL(VRCP_DnLdErr2DI, 0, &nIOStatus);
	dWRITE_ANALOG(PLC_CurStepAI, 0, &nIOStatus);

	// VRCP_PauseDI => 0:Normal 1:Paused
	dWRITE_DIGITAL(VRCP_PauseDI, 0, &nIOStatus);

	DestroyThread();
	if(m_nPLC_VrcpSts != PLC_VRCP_RUN)
	{
		m_nPLC_VrcpSts = PLC_VRCP_RUN;
		CreateThread();
	}
	return msRet;
}

Module_Status CPLCSimulator::ResumeValveRecipe()
{
	Module_Status msRet = SYS_SUCCESS;
	_LOG("ResumeValveRecipe Called");
	if(m_nPLC_VrcpSts == PLC_VRCP_PAUSED) m_nPLC_VrcpSts = PLC_VRCP_RUN;
	return msRet;
}

Module_Status CPLCSimulator::StopValveRecipe()
{
	Module_Status msRet = SYS_SUCCESS;
	_LOG("StopValveRecipe Called");
	m_nPLC_VrcpSts = PLC_VRCP_STOP;
	return msRet;
}

Module_Status CPLCSimulator::GoNextStep()
{
	Module_Status msRet = SYS_SUCCESS;
	_LOG("GoNextStep Called");
	m_bGoNext = TRUE;
	return msRet;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// Sub Function

int CPLCSimulator::CheckAlmOpt(int nAlarm, int nAlmOptChannel)
{
	int nRet = AOPT_IGNORE;
	int nIOStatus;

	nRet = dREAD_DIGITAL(nAlmOptChannel, &nIOStatus);
	switch(nRet)
	{
	case AOPT_IGNORE: break;
	case AOPT_WARNING:
	case AOPT_ABORT:
		ALARM_POST(nAlarm);
		break;
	}
	return nRet;
}

BOOL CPLCSimulator::ExcuteVRCPLine(unsigned char *pData)
{
	BOOL bRet = TRUE;
	double dbDelayTime;
	int nValveNum;
	int i, j;
	int nIdx;
	static int pnValveByteIdx[MAX_VALVE_BYTE] = { 3, 2, 5, 4, 7, 6 };

	do {
		// Read Parameter
		if(m_nValveTimeRes == VTS_10MS)		dbDelayTime = ((double) (pData[0]*256 + pData[1])) /100;
		else								dbDelayTime = ((double) (pData[0]*256 + pData[1])) /10;

		printf("-------> Time:%5.2fsec VRCPData: %02X %02X %02X %02X %02X %02X %02X %02X in PLCSimulator\n",
			dbDelayTime,
			pData[0], pData[1], pData[2], pData[3],
			pData[4], pData[5], pData[6], pData[7]);
	
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
void CPLCSimulator::SetMFC()
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

void CPLCSimulator::SetRF()		// 2006.03.06
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

void CPLCSimulator::SetRF2()	// 2014.04.09
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

void CPLCSimulator::SetHTE()	// 2014.04.09
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
Module_Status CPLCSimulator::RunValveRecipe()
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
	SetRF();
	SetRF2();
	SetMFC();// 2014.04.09
	SetHTE();

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
			sprintf(szLog, "-----===> [ProStep:%d] This is Time Unlimited Step in PLCSimulator", nPLC_CurStepAI);
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

//////////////////////////////////////////////////////////////////////////
//... PLC Line Step Simulator
BOOL CPLCSimulator::WaitPLCTm(double dbTime)		// 2006.07.31
{
	BOOL bRet = TRUE;
	double dbPrgTm;
	int i = 0, nIOStatus;

	dWRITE_ANALOG(PLC_RCPRunTMAI, 0, &nIOStatus);
	
	do {
		Sleep(30);
		if(m_nThreadSts != THRD_STS_RUN)		{ bRet = FALSE; break; }
		dbPrgTm = m_pTimer->GetProgressTime();
		dbPrgTm /= 1000;

		dWRITE_ANALOG(PLC_RCPRunTMAI, dbPrgTm, &nIOStatus);
		
		if(m_nPLC_VrcpSts != PLC_VRCP_RUN)		{ bRet = FALSE; break; }

		if(((++i) % 15) == 0)
		{
			if(dREAD_DIGITAL(VRCP_StaStpDO, &nIOStatus) == 0) { bRet = FALSE; break; }
		}

		//... 2015.04.03 Add Next Step
		if(m_bGoNext)	break;

	} while(dbTime > dbPrgTm - m_dbVrcpProgress);	//... Waiting Step Time

	m_dbVrcpProgress += dbTime;

	return bRet;
}

//--------------------------------------------------------------------
void CPLCSimulator::DoMonitor()
{
	int nIOStatus;
	char szLog[200];
	int nPause = 0;

	m_nRunPage = 0;
	sprintf(szLog, "< Start Valve Recipe(Thread) >");
	_LOG(szLog); printf("%s\n", szLog);

	//... 2015.04.08 Add RF Forward/Reflect Count Up
	m_nRF1LoadCnt	= 0;
	m_nRF2LoadCnt	= 0;
	m_nRF1ReflCnt	= 0;
	m_nRF2ReflCnt	= 0;

	dWRITE_ANALOG(RF1ReflCntAI, m_nRF1ReflCnt, &nIOStatus);
	dWRITE_ANALOG(RF2ReflCntAI, m_nRF2ReflCnt, &nIOStatus);

	// VRCP_RunningDI => 0:Stop 1:Running
	dWRITE_DIGITAL(VRCP_RunningDI, 1, &nIOStatus);

	while(m_nPLC_VrcpSts == PLC_VRCP_RUN || m_nPLC_VrcpSts == PLC_VRCP_PAUSED)
	{
		if(m_VR_Header[m_nRunPage][VR_HDR_BASE+2] == 0x00)
		{
			if(SYS_SUCCESS != RunValveRecipe())
			{
				m_nPLC_VrcpSts = PLC_VRCP_FAILED;
				break;
			}
			m_VR_Header[m_nRunPage][VR_HDR_BASE+2] = 0x01;
		}
		else WaitSec(0.1);

		// Exchange Page
//		m_nRunPage ^= 0x01; 
		if(m_nRunPage != 4)
		{
			m_nRunPage++;
		}
		else
			m_nRunPage = 0;
		sprintf(szLog, "-----=====> [Page:%d -> %d] Running Process Step Page Exchanged in PLCSimulator",
			m_nRunPage-1, m_nRunPage);
		_LOG(szLog); printf("%s\n", szLog);

		//... Valve Recipe Pause
		do {
			while(m_nPLC_VrcpSts == PLC_VRCP_PAUSED)
			{
				if(dREAD_DIGITAL(VRCP_StaStpDO, &nIOStatus) == 0)
				{
					m_nPLC_VrcpSts = PLC_VRCP_STOP;
					break;
				}
				if(nPause == 0)
				{
					// VRCP_PauseDI => 0:Normal 1:Paused
					nPause = 1;
					dWRITE_DIGITAL(VRCP_PauseDI, nPause, &nIOStatus);
				}
				WaitSec(0.1);
			}
			if(m_nPLC_VrcpSts == PLC_VRCP_STOP || m_nPLC_VrcpSts == PLC_VRCP_FAILED) break;

			// Check Download Completed
			if(m_nPLC_VrcpSts != PLC_VRCP_PAUSED && (m_VR_Header[m_nRunPage][VR_HDR_BASE+2] & BIT_EXEC_FLAG) == BIT_EXEC_FLAG)
			{
				if(m_nRunPage == 0) dWRITE_DIGITAL(VRCP_DnLdErr1DI, 1, &nIOStatus);
				else dWRITE_DIGITAL(VRCP_DnLdErr2DI, 1, &nIOStatus);
				m_nPLC_VrcpSts = PLC_VRCP_PAUSED;
				sprintf(szLog, "-----=====> [Page:%d] Valve Recipe is not Downloaded yet in PLCSimulator !",	m_nRunPage);
				_LOG(szLog); printf("%s\n", szLog);
			}
		} while(m_nPLC_VrcpSts == PLC_VRCP_PAUSED);

		// VRCP_PauseDI => 0:Normal 1:Paused
		if(nPause == 1)
		{
			nPause = 0;
			dWRITE_DIGITAL(VRCP_PauseDI, nPause, &nIOStatus);
			dWRITE_DIGITAL(VRCP_DnLdErr1DI, 0, &nIOStatus);
			dWRITE_DIGITAL(VRCP_DnLdErr2DI, 0, &nIOStatus);
		}

	} // end while
	
//	CloseAllValve();

	// VRCP_RunningDI => 0:Stop 1:Running
	dWRITE_DIGITAL(VRCP_RunningDI, 0, &nIOStatus);
	_LOG("-----> VRCP_RunningDI Channel Dropped in PLCSimulator");

	// VRCP_PauseDI => 0:Normal 1:Paused
	dWRITE_DIGITAL(VRCP_PauseDI, 0, &nIOStatus);

	if(m_nPLC_VrcpSts != PLC_VRCP_FAILED)
	{
		sprintf(szLog, "< Stop Valve Recipe (Thread Success) >");
		_LOG(szLog); printf("%s\n", szLog);
		m_nPLC_VrcpSts = PLC_VRCP_STOP;
	}
	else
	{
		sprintf(szLog, "< Stop Valve Recipe (Thread Failed) >");
		_LOG(szLog); printf("%s\n", szLog);
	}
}

//--------------------------------------------------------------------
void CPLCSimulator::DoMonitor_B()
{
	int nIOStatus;
	int nSetTemp;
	while(WaitSec(0.3))
	{
		// 0:Normal 1:Reset
		if(dREAD_DIGITAL(PLC_RstIntLckDO, &nIOStatus) == 1)
		{
			while(dREAD_DIGITAL(PLC_RstIntLckDO, &nIOStatus) == 1)
			{
				WaitSec(0.3);
			}
			// 0:Normal 1:Interlock
			dWRITE_DIGITAL(PLC_GbIntlckDI, 0, &nIOStatus);
			CloseAllValve();
		}
		if(dREAD_DIGITAL(RPGGateVv1DO, &nIOStatus) == 1)
		{
			dWRITE_DIGITAL(RPGGateVlvPosDI, 1, &nIOStatus);
		}
		else if(dREAD_DIGITAL(RPGGateVv1DO, &nIOStatus) == 0)
		{
			dWRITE_DIGITAL(RPGGateVlvPosDI, 2, &nIOStatus);
		}

		if(dREAD_DIGITAL(RPGGateVv2DO, &nIOStatus) == 1)
		{
			dWRITE_DIGITAL(RPGGateVlv2PosDI, 1, &nIOStatus);
		}
		else if(dREAD_DIGITAL(RPGGateVv2DO, &nIOStatus) == 0)
		{
			dWRITE_DIGITAL(RPGGateVlv2PosDI, 2, &nIOStatus);
		}

        if(1 == dREAD_DIGITAL(HTE_StaStpDIO,&nIOStatus)){
			nSetTemp = dREAD_DIGITAL(HTE_SetTempAIO,&nIOStatus);	
			dWRITE_DIGITAL(HTE_ReadTempAI,nSetTemp, &nIOStatus);
		}
	}
}

//--------------------------------------------------------------------
// for Valve I/O Control
void CPLCSimulator::InitValveIO()
{
	m_DynValveCtrl.InitIOList(m_ValveList, FALSE);
}

void CPLCSimulator::SetValveStatus(int nValveNum, int nValue)
{
	int nIdx;
	BOOL bIOStatus;
	nIdx = nValveNum - 1;

	m_DynValveCtrl.dWRITE_DIGITAL(nIdx, nValue, &bIOStatus);

	//... 2015.04.08 Add RF Forward/Reflect Count Up
	if(VOpen == nValue)
	{
		//... RF1
		if(94 == nIdx)
		{
			m_nRF1LoadCnt++;
			dWRITE_ANALOG(RF1LoadCntAIO, m_nRF1LoadCnt, &bIOStatus);

			if(m_nRF1ReflCnt++ < 1)		dWRITE_ANALOG(RF1ReflCntAI, m_nRF1ReflCnt, &bIOStatus);
		}
		else if(95 == nIdx)
		{
			m_nRF2LoadCnt++;
			dWRITE_ANALOG(RF2LoadCntAIO, m_nRF2LoadCnt, &bIOStatus);

			if(m_nRF2ReflCnt++ < 2)		dWRITE_ANALOG(RF2ReflCntAI, m_nRF2ReflCnt, &bIOStatus);
		}
	}

}

int CPLCSimulator::GetValveStatus(int nValveNum)
{
	int nIdx;
	BOOL bIOStatus;
	nIdx = nValveNum - 1;
	return m_DynValveCtrl.dREAD_DIGITAL(nIdx, &bIOStatus);
}
