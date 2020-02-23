// ModuleControl.cpp: implementation of the CModuleControl class.
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

//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextParser.h"
#include "ModuleControl.h"

//------------------------------------------------------------------------------------------
#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\PMCConfig.ini"


//------------------------------------------------------------------------------------------
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	//  Carrier Ar Flow Valve List
	CAF_VALVE_01,
	CAF_VALVE_02,
	CAF_VALVE_03,
	CAF_VALVE_04,
	CAF_VALVE_05,
	CAF_VALVE_06,
	CAF_VALVE_07,
	CAF_VALVE_08,
	CAF_VALVE_09,
	CAF_VALVE_10,

	// Monitoring Guage
	PressGuageAI,
	VRCP_RunningDI,

} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	//  Carrier Ar Flow Valve List
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},
	{"NotDefined",			_K_D_IO, 0},

	// Monitoring Guage
	{"NotDefined",			_K_A_IO, 0},
	{"VRCP_RunningDI",		_K_D_IO, 0},
	""
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModuleControl::CModuleControl()
{
	int i;
	m_CAF_bEnable = FALSE;
	m_nCarrierArFlowSts = CAF_IDLE;
	m_CAF_nMfcCount = 0;
	for(i=0; i<CAF_MAX_MFC; i++) { m_CAF_anMfcNo[i] = 0; m_CAF_adbMfcSp[i] = 0; }
	m_CAF_nValveCount = 0;
	m_CAF_dbChmPrsLowLimit = 100;
}

CModuleControl::~CModuleControl()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CModuleControl::Initialize()
{
	ReadPmcConfig();
	InitIOList(g_pIOList, false);
	CreateThread();
	return TRUE;
}

void CModuleControl::Deinitialize()
{
}

BOOL CModuleControl::ReadPmcConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(PMC_CONFIG_FILE, "rt");
		if(fp == NULL) break;
		// printf("-----> Read PMC Configration Data\n");

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("Carrier Ar Flow", szItem) == 0)			nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseCarrierArFlow(szRead); break;
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
		printf("-----> Config File Reading Failed : %s", PMC_CONFIG_FILE);
	}
	return bRet;
}

void CModuleControl::ParseCarrierArFlow(char *szData)
{
	char szItem[40];
	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "MFC") == 0)
		{
			if(m_CAF_nMfcCount < CAF_MAX_MFC)
			{
				tp.GetInt(&m_CAF_anMfcNo[m_CAF_nMfcCount]);
				tp.GetDbFloat(&m_CAF_adbMfcSp[m_CAF_nMfcCount]);
				m_CAF_nMfcCount++;
			}
		}
		else if(strcmp(szItem, "VALVE_NAME") == 0)
		{
			if(m_CAF_nValveCount < CAF_MAX_VALVE)
			{
				tp.GetWord(g_pIOList[CAF_VALVE_01+m_CAF_nValveCount].szIOName);
				m_CAF_nValveCount++;
			}
		}
		else if(strcmp(szItem, "PRES_GUAGE") == 0)
		{
			tp.GetWord(g_pIOList[PressGuageAI].szIOName);
			tp.GetDbFloat(&m_CAF_dbChmPrsLowLimit);
		}
		else if(strcmp(szItem, "CarrierArFlowEnable") == 0)
		{
			tp.GetBOOL(&m_CAF_bEnable);
		}
	} while(0);
}

void CModuleControl::StartCarrierArFlow()
{
	m_nCarrierArFlowSts = CAF_RUN;
}

void CModuleControl::StopCarrierArFlow()
{
	m_nCarrierArFlowSts = CAF_STOP;
}

BOOL CModuleControl::RunCarrierArFlow()
{
	BOOL bRet = FALSE;
	int i;
	BOOL bIOStatus;
	char szCmd[256];
	char szParam[40];
	BOOL bValveAlreadyOpened = TRUE;

	enum {VLV_CLOSE, VLV_OPEN};

	do {
		// Is Valve Already Opend ?
		for(i=0; i<m_CAF_nValveCount; i++)
		{
			if(dREAD_DIGITAL(CAF_VALVE_01+i, &bIOStatus) != VLV_OPEN) bValveAlreadyOpened = FALSE;
		}

		// FastRough Vale Close
		if(! bValveAlreadyOpened)
		{
			WRITE_DIGITAL(FastRoughVlvDO, VLV_CLOSE, &bIOStatus);
			WaitSec(0.5);
		}

		// Valve Open
		for(i=0; i<m_CAF_nValveCount; i++) dWRITE_DIGITAL(CAF_VALVE_01+i, VLV_OPEN, &bIOStatus);
	
		// MFC Set
		sprintf(szCmd, "SET_VALUE 1");
		for(i=0; i<m_CAF_nMfcCount; i++)
		{
			sprintf(szParam, " %d %.1f F 1", m_CAF_anMfcNo[i], m_CAF_adbMfcSp[i]);
			strcat( szCmd, szParam);
		}
		RUN_FUNCTION( MFC_CONTROL, szCmd );

		// FastRough Vale Open
		if(! bValveAlreadyOpened)
		{
			WaitSec(3);
			WRITE_DIGITAL(FastRoughVlvDO, VLV_OPEN, &bIOStatus);
		}

		// Finished
		bRet = TRUE;
	} while(0);
	return bRet;
}

void CModuleControl::FinishCarrierArFlow()
{
	BOOL bRet = FALSE;
	int i;
	BOOL bIOStatus;
	char szCmd[256];
	char szParam[40];
	enum {VLV_CLOSE, VLV_OPEN};

	// Valve Close
	for(i=0; i<m_CAF_nValveCount; i++) dWRITE_DIGITAL(CAF_VALVE_01+i, VLV_CLOSE, &bIOStatus);

	// MFC Set 0
	sprintf(szCmd, "SET_VALUE 1");
	for(i=0; i<m_CAF_nMfcCount; i++)
	{
		sprintf(szParam, " %d 0 F 1", m_CAF_anMfcNo[i]);
		strcat( szCmd, szParam);
	}
	RUN_FUNCTION( MFC_CONTROL, szCmd );
}

BOOL CModuleControl::CheckChamberCondition()
{
	BOOL bRet = TRUE;
	BOOL bIOStatus;
	enum { CP_TRANS, CP_ATM, CP_VACUUM, CP_ERROR };
	enum { VRCP_IDLE, VRCP_RUNNING };
	enum { DP_STOP, DP_RUN };

	if(IOValid(PressGuageAI))
	{
		if(dREAD_ANALOG(PressGuageAI, &bIOStatus) > m_CAF_dbChmPrsLowLimit) bRet = FALSE;
	}
	if(READ_DIGITAL(ChmPrssDI1, &bIOStatus) != CP_VACUUM) bRet = FALSE;
	if(dREAD_DIGITAL(VRCP_RunningDI, &bIOStatus) != VRCP_IDLE) bRet = FALSE;
	if(READ_DIGITAL(DPRunDI, &bIOStatus) != DP_RUN) bRet = FALSE;

	return bRet;
}

//////////////////////////////////////////////////////////////////////
// Virtual Function

void CModuleControl::DoMonitor()
{
	if(m_CAF_bEnable)
		while(WaitSec(1))
	{
		while(m_nCarrierArFlowSts != CAF_RUN)
		{
			WaitSec(1);
			if(m_nCarrierArFlowSts == CAF_STOP)
			{
				FinishCarrierArFlow();
				m_nCarrierArFlowSts = CAF_IDLE;
			}
		}
		if(IsThreadStopping()) break;
		
		do {
			if(! CheckChamberCondition())
			{
				m_nCarrierArFlowSts = CAF_IDLE;
				break;
			}
	
			RunCarrierArFlow();
			m_nCarrierArFlowSts = CAF_MONITORING;

			while(m_nCarrierArFlowSts == CAF_MONITORING)
			{
				if(! WaitSec(1)) break;
				if(! CheckChamberCondition()) break;
			}
			if(m_nCarrierArFlowSts != CAF_RUN)
			{
				FinishCarrierArFlow();
				m_nCarrierArFlowSts = CAF_IDLE;
			}
		} while(0);
	}
}
