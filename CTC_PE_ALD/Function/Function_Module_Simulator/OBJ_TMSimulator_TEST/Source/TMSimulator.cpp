// TMSimulator.cpp: implementation of the CTMSimulator class.
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

//--------------------------------------------------------------------
// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
#include "TextParser.h"
#include "TMSimulator.h"

//--------------------------------------------------------------------
#define APP_NAME	"TMSimulator"
#define MAX_ISO_VALVE		2
#define MAX_PROCESS_MODULE	3

//--------------------------------------------------------------------
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	TM_BM1_SlotVvLXI,
	TM_BM1_SlotVvRXI,
	TM_BM2_SlotVvLXI,
	TM_BM2_SlotVvRXI,

	TM_PM1_SlotVv1XI,
	TM_PM1_SlotVv2XI,
	TM_PM2_SlotVv1XI,
	TM_PM2_SlotVv2XI,
	TM_PM3_SlotVv1XI,
	TM_PM3_SlotVv2XI,

	TM_BM1_SlotVvLXO,
	TM_BM1_SlotVvRXO,
	TM_BM2_SlotVvLXO,
	TM_BM2_SlotVvRXO,

	TM_PM1_SlotVv1XO,
	TM_PM1_SlotVv2XO,
	TM_PM2_SlotVv1XO,
	TM_PM2_SlotVv2XO,
	TM_PM3_SlotVv1XO,
	TM_PM3_SlotVv2XO,

	BM1_DoorLXI,
	BM1_DoorRXI,
	BM1_DoorLXO,
	BM1_DoorRXO,

	BM2_DoorLXI,
	BM2_DoorRXI,
	BM2_DoorLXO,
	BM2_DoorRXO,

	PM1_TMIsoVlvClosedDI,
	PM1_TMIsoVlv2ClosedDI,
	PM2_TMIsoVlvClosedDI,
	PM2_TMIsoVlv2ClosedDI,
	PM3_TMIsoVlvClosedDI,
	PM3_TMIsoVlv2ClosedDI,

	BM1_DPStsDI,
	BM1_DPWarnStsDI,
	BM1_DPAlarmStsDI,
	BM1_CGPumpPrsAI, 
	BM1_CGChmPrsAI,
	BM1_RoughVvXO,
	BM1_VentVvXO,
	BM1_ATMVACSensorXI,
	BM1_OverPresVvDO,

	BM1_PinUpDownLXO,
	BM1_PinUpDownLXI,
	BM1_PinUpDownRXO,
	BM1_PinUpDownRXI,

	BM2_DPStsDI,
	BM2_DPWarnStsDI,
	BM2_DPAlarmStsDI,
	BM2_CGPumpPrsAI, 
	BM2_CGChmPrsAI,
	BM2_RoughVvXO,
	BM2_VentVvXO,
	BM2_ATMVACSensorXI,
	BM2_OverPresVvDO,

	BM2_PinUpDownLXO,
	BM2_PinUpDownLXI,
	BM2_PinUpDownRXO,
	BM2_PinUpDownRXI,

	TM_DPStsDI,
	TM_DPWarnStsDI,
	TM_DPAlarmStsDI,
	TM_CGPumpPrsAI, 
	TM_CGChmPrsAI,
	TM_RoughVvXO,
	TM_VentVvXO,
	TM_ATMVACSensorXI,
	TM_OverPresVvDO,
	
	TM_UPC_Set			,
	TM_UPC_Flow			,

	LPMA_FOUP_PresentDI,
	LPMB_FOUP_PresentDI,
	LPMC_FOUP_PresentDI,
	LPMD_FOUP_PresentDI,

	LPMA_F_ProperlyPlaceDI,
	LPMB_F_ProperlyPlaceDI,
	LPMC_F_ProperlyPlaceDI,
	LPMD_F_ProperlyPlaceDI,

	PM1_ChmPrsAI,
	PM2_ChmPrsAI,
	PM3_ChmPrsAI,

} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{ "TM.BM1.SlotVvLXI"		, _K_D_IO	,		0	} ,
	{ "TM.BM1.SlotVvRXI"		, _K_D_IO	,		0	} ,
	{ "TM.BM2.SlotVvLXI"		, _K_D_IO	,		0	} ,
	{ "TM.BM2.SlotVvRXI"		, _K_D_IO	,		0	} ,

	{ "TM.PM1.SlotVv1XI"		, _K_D_IO	,		0	} ,
	{ "TM.PM1.SlotVv2XI"		, _K_D_IO	,		0	} ,
	{ "TM.PM2.SlotVv1XI"		, _K_D_IO	,		0	} ,
	{ "TM.PM2.SlotVv2XI"		, _K_D_IO	,		0	} ,
	{ "TM.PM3.SlotVv1XI"		, _K_D_IO	,		0	} ,
	{ "TM.PM3.SlotVv2XI"		, _K_D_IO	,		0	} ,

	{ "TM.BM1.SlotVvLXO"		, _K_D_IO	,		0	} ,
	{ "TM.BM1.SlotVvRXO"		, _K_D_IO	,		0	} ,
	{ "TM.BM2.SlotVvLXO"		, _K_D_IO	,		0	} ,
	{ "TM.BM2.SlotVvRXO"		, _K_D_IO	,		0	} ,

	{ "TM.PM1.SlotVv1XO"		, _K_D_IO	,		0	} ,
	{ "TM.PM1.SlotVv2XO"		, _K_D_IO	,		0	} ,
	{ "TM.PM2.SlotVv1XO"		, _K_D_IO	,		0	} ,
	{ "TM.PM2.SlotVv2XO"		, _K_D_IO	,		0	} ,
	{ "TM.PM3.SlotVv1XO"		, _K_D_IO	,		0	} ,
	{ "TM.PM3.SlotVv2XO"		, _K_D_IO	,		0	} ,

	{ "BM1.DoorLXI"				, _K_D_IO	,		0	} ,
	{ "BM1.DoorRXI"				, _K_D_IO	,		0	} ,
	{ "BM1.DoorLXO"				, _K_D_IO	,		0	} ,
	{ "BM1.DoorRXO"				, _K_D_IO	,		0	} ,

	{ "BM2.DoorLXI"				, _K_D_IO	,		0	} ,
	{ "BM2.DoorRXI"				, _K_D_IO	,		0	} ,
	{ "BM2.DoorLXO"				, _K_D_IO	,		0	} ,
	{ "BM2.DoorRXO"				, _K_D_IO	,		0	} ,

	{ "PM1.TMIsoVlvClosedDI"	, _K_D_IO	,		0	},
	{ "PM1.TMIsoVlv2ClosedDI"	, _K_D_IO	,		0	},
	{ "PM2.TMIsoVlvClosedDI"	, _K_D_IO	,		0	},
	{ "PM2.TMIsoVlv2ClosedDI"	, _K_D_IO	,		0	},
	{ "PM3.TMIsoVlvClosedDI"	, _K_D_IO	,		0	},
	{ "PM3.TMIsoVlv2ClosedDI"	, _K_D_IO	,		0	},

	{ "BM1.DPStsDI",			_K_D_IO,		0	},
	{ "BM1.DPWarnStsDI",		_K_D_IO,		0	},
	{ "BM1.DPAlarmStsDI",		_K_D_IO,		0	},
	{ "BM1.CGPumpPrsAI",		_K_A_IO,		0	},
	{ "BM1.CGChmPrsAI",			_K_A_IO,		0	},
	{ "BM1.RoughVvXO",			_K_D_IO,		0	},
	{ "BM1.VentVvXO",			_K_D_IO,		0	},
	{ "BM1.ATMVACSensorXI",		_K_D_IO,		0	},
	{ "BM1.OverPresVvDO",		_K_D_IO,		0	},

	{ "BM1.PinUpDownLXO",		_K_D_IO,		0	},
	{ "BM1.PinUpDownLXI",		_K_D_IO,		0	},
	{ "BM1.PinUpDownRXO",		_K_D_IO,		0	},
	{ "BM1.PinUpDownRXI",		_K_D_IO,		0	},

	{ "BM2.DPStsDI",			_K_D_IO,		0	},
	{ "BM2.DPWarnStsDI",		_K_D_IO,		0	},
	{ "BM2.DPAlarmStsDI",		_K_D_IO,		0	},
	{ "BM2.CGPumpPrsAI",		_K_A_IO,		0	},
	{ "BM2.CGChmPrsAI",			_K_A_IO,		0	},
	{ "BM2.RoughVvXO",			_K_D_IO,		0	},
	{ "BM2.VentVvXO",			_K_D_IO,		0	},
	{ "BM2.ATMVACSensorXI",		_K_D_IO,		0	},
	{ "BM2.OverPresVvDO",		_K_D_IO,		0	},

	{ "BM2.PinUpDownLXO",		_K_D_IO,		0	},
	{ "BM2.PinUpDownLXI",		_K_D_IO,		0	},
	{ "BM2.PinUpDownRXO",		_K_D_IO,		0	},
	{ "BM2.PinUpDownRXI",		_K_D_IO,		0	},

	{ "TM.DPStsDI",				_K_D_IO,		0	},
	{ "TM.DPWarnStsDI",			_K_D_IO,		0	},
	{ "TM.DPAlarmStsDI",		_K_D_IO,		0	},
	{ "TM.CGPumpPrsAI",			_K_A_IO,		0	},
	{ "TM.CGChmPrsAI",			_K_A_IO,		0	},
	{ "TM.RoughVvXO",			_K_D_IO,		0	},
	{ "TM.VentVvXO",			_K_D_IO,		0	},
	{ "TM.ATMVACSensorXI",		_K_D_IO,		0	},
	{ "TM.OverPresVvDO",		_K_D_IO,		0	},
	
	{ "TM.TM_UPC_SET"				, _K_A_IO,	0	} ,
	{ "TM.TM_UPC_Flow"				, _K_A_IO,	0	} ,

	{ "LPMA.FOUP_PresentDI",		_K_D_IO,		0	},
	{ "LPMB.FOUP_PresentDI",		_K_D_IO,		0	},
	{ "LPMC.FOUP_PresentDI",		_K_D_IO,		0	},
	{ "LPMD.FOUP_PresentDI",		_K_D_IO,		0	},

	{ "LPMA.F_ProperlyPlaceDI",		_K_D_IO,		0	},
	{ "LPMB.F_ProperlyPlaceDI",		_K_D_IO,		0	},
	{ "LPMC.F_ProperlyPlaceDI",		_K_D_IO,		0	},
	{ "LPMD.F_ProperlyPlaceDI",		_K_D_IO,		0	},

	{ "PM1.ChmPrsAI",			_K_A_IO,		0	},
	{ "PM2.ChmPrsAI",			_K_A_IO,		0	},
	{ "PM3.ChmPrsAI",			_K_A_IO,		0	},

	""
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
#define SIM_CFG_FILE		".\\Function\\Function_Module_Simulator\\Simulation.cfg"

//--------------------------------------------------------------------
enum {
	TFNO_SlotValve,
	TFNO_IsoValve, 
	TFNO_TMVac, 
	TFNO_BMPin, 
	TFNO_AlarmClear, 
	TFNO_CMFoupSensor,
} THdREAD_NO_ENUM;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTMSimulator::CTMSimulator()
{
	int i;
	for(i=0; i<MAX_IO_LIST; i++)
	{
		m_IOList[i].szIOName[0] = 0;
		m_IOList[i].szValue[0] = 0;
		m_IOList[i].nIOType = 0;
		m_IOList[i].nIOPoint = -1;
	}
	m_IOList_nCount = 0;

	m_bPM1IsoVlvSync = FALSE;
	m_bPM2IsoVlvSync = FALSE;
	m_bPM3IsoVlvSync = FALSE;
	m_bPM4IsoVlvSync = FALSE;
	m_bPM5IsoVlvSync = FALSE;

	for(i=0; i<MAX_ALARM_CLEAR_LIST; i++) m_pnAlarmClearList[i] = 0;
	m_nAlarmClearCount = 0;
}

CTMSimulator::~CTMSimulator()
{

}

//////////////////////////////////////////////////////////////////////
// Thread

void CTMSimulator::DoMonitor(int nThreadNo)
{
	switch(nThreadNo)
	{
	case TFNO_SlotValve:	SlotValveSimMonitor(nThreadNo); break;
	case TFNO_IsoValve:		IsoValveSyncSimMonitor(nThreadNo); break;
	case TFNO_TMVac:		TmVacuumSimMonitor(nThreadNo); break;
	case TFNO_BMPin:		BMPinSimMonitor(nThreadNo); break;
	case TFNO_AlarmClear:	AlarmClear(nThreadNo); break;
	case TFNO_CMFoupSensor:	CMFoupSensorMonitor(nThreadNo); break;
	}
}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CTMSimulator::Initialize()
{
	InitIOList(g_pIOList, FALSE);
	ReadSimConfig();
	InitIOValue();

	CreateThread(TFNO_SlotValve);
	CreateThread(TFNO_IsoValve);
	CreateThread(TFNO_TMVac);
	CreateThread(TFNO_BMPin);
	CreateThread(TFNO_AlarmClear);
	CreateThread(TFNO_CMFoupSensor);
	return TRUE;
}

BOOL CTMSimulator::ReadSimConfig()
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

				if(strcmp("PMC Synchronization", szItem) == 0)		nTitle = 1;
				else if(strcmp("Init I/O List", szItem) == 0)		nTitle = 2;
				else if(strcmp("Alarm Clear List", szItem) == 0)	nTitle = 3;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParsePmcSyncOption(szRead); break;
					case 2: ParseInitIOCfg(szRead); break;
					case 3: ParseAlarmClearCfg(szRead); break;
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
		printf("-----> Config File Reading Failed : %s\n", SIM_CFG_FILE);
	}
	return bRet;
}

void CTMSimulator::ParseInitIOCfg(char *szRead)
{
	char szItem[40] = "";
	CTextParser tp(szRead);
	if(0 == strlen(szRead)) return;
	if(! tp.GetWord(m_IOList[m_IOList_nCount].szIOName, 0)) return;

	if(! tp.GetWord(szItem, 0)) return;
	if(szItem[0] == 'A') m_IOList[m_IOList_nCount].nIOType = _K_A_IO;
	else if(szItem[0] == 'S') m_IOList[m_IOList_nCount].nIOType = _K_S_IO;
	else m_IOList[m_IOList_nCount].nIOType = _K_D_IO;

	tp.GetWord(m_IOList[m_IOList_nCount].szValue, 0);
	m_IOList[m_IOList_nCount].nIOPoint = _FIND_FROM_STRING(
		m_IOList[m_IOList_nCount].nIOType, m_IOList[m_IOList_nCount].szIOName);
	if(m_IOList[m_IOList_nCount].nIOPoint != -1) m_IOList_nCount++;
	else
	{
		printf("-----> Invalid IO List (%s %s %s) in %s\n", m_IOList[m_IOList_nCount].szIOName,
				szItem, m_IOList[m_IOList_nCount].szValue, APP_NAME);
	}
}
//2014.03.28
void CTMSimulator::CMFoupSensorMonitor(int nThreadNo)
{
	enum { EN_OFF, EN_ON };

	int i;
	int anSensor[3] = { EN_OFF, EN_OFF, EN_OFF };
	int nSensor;
	BOOL bIOStatus;

	do {
		if(! WAIT_SEC(1)) break;

		for(i=0; i<4; i++)
		{
			nSensor = dREAD_DIGITAL(LPMA_F_ProperlyPlaceDI+i, &bIOStatus);
			if(anSensor[i] != nSensor)
			{
				dWRITE_DIGITAL(LPMA_FOUP_PresentDI+i, nSensor, &bIOStatus);
				anSensor[i] = nSensor;
				if(! WAIT_SEC(1)) break;
			}
		}
	} while(1);
}
// 2005.03.18 by cskim
void CTMSimulator::ParsePmcSyncOption(char *szRead)
{
	char szItem[40];
	CTextParser tp(szRead);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "PM1_IsoValve_SW_Sync") == 0) tp.GetBOOL(&m_bPM1IsoVlvSync);
		else if(strcmp(szItem, "PM2_IsoValve_SW_Sync") == 0) tp.GetBOOL(&m_bPM2IsoVlvSync);
		else if(strcmp(szItem, "PM3_IsoValve_SW_Sync") == 0) tp.GetBOOL(&m_bPM3IsoVlvSync);
		else if(strcmp(szItem, "PM4_IsoValve_SW_Sync") == 0) tp.GetBOOL(&m_bPM4IsoVlvSync);
		else if(strcmp(szItem, "PM5_IsoValve_SW_Sync") == 0) tp.GetBOOL(&m_bPM5IsoVlvSync);
	} while(0);
}

void CTMSimulator::ParseAlarmClearCfg(char *szRead)
{
	char szItem[40] = "";
	int nAlarmID;
	nAlarmID = atoi(szRead);
	if(nAlarmID > 0 && m_nAlarmClearCount < MAX_ALARM_CLEAR_LIST)
	{
		m_pnAlarmClearList[m_nAlarmClearCount]  = nAlarmID;
		m_nAlarmClearCount++;
	}
}

void CTMSimulator::InitIOValue()
{
	int nIOStatus;
	int i;
	int nValue;
	double dbValue;
	for(i=0; i<m_IOList_nCount; i++)
	{
		switch(m_IOList[i].nIOType)
		{
		case _K_A_IO:
			dbValue = atof(m_IOList[i].szValue);
			_dWRITE_ANALOG(m_IOList[i].nIOPoint, dbValue, &nIOStatus);
		case _K_D_IO:
			nValue = atoi(m_IOList[i].szValue);
			_dWRITE_DIGITAL(m_IOList[i].nIOPoint, nValue, &nIOStatus);
		case _K_S_IO:
			_dWRITE_STRING(m_IOList[i].nIOPoint, m_IOList[i].szValue, &nIOStatus);
		}
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void CTMSimulator::BMPinSimMonitor(int nThreadNo)
{
	int nIOStatus;
	int nValue;
	int nValue2;

	do {
		if(! WAIT_SEC(1)) break;

		// BM1 Motion
		nValue  = dREAD_DIGITAL(BM1_PinUpDownLXO, &nIOStatus);		// 0:Unknown 1:Up 2:Down
		nValue2 = dREAD_DIGITAL(BM1_PinUpDownLXI, &nIOStatus);
		if(nValue != nValue2)
		{
			WAIT_SEC(1);
			dWRITE_DIGITAL(BM1_PinUpDownLXI, nValue, &nIOStatus);
		}

		// BM1 Motion
		nValue  = dREAD_DIGITAL(BM1_PinUpDownRXO, &nIOStatus);		// 0:Unknown 1:Up 2:Down
		nValue2 = dREAD_DIGITAL(BM1_PinUpDownRXI, &nIOStatus);
		if(nValue != nValue2)
		{
			WAIT_SEC(1);
			dWRITE_DIGITAL(BM1_PinUpDownRXI, nValue, &nIOStatus);
		}

		// BM2 Motion
		nValue  = dREAD_DIGITAL(BM2_PinUpDownLXO, &nIOStatus);		// 0:Unknown 1:Up 2:Down
		nValue2 = dREAD_DIGITAL(BM2_PinUpDownLXI, &nIOStatus);
		if(nValue != nValue2)
		{
			WAIT_SEC(1);
			dWRITE_DIGITAL(BM2_PinUpDownLXI, nValue, &nIOStatus);
		}

		// BM2 Motion
		nValue  = dREAD_DIGITAL(BM2_PinUpDownRXO, &nIOStatus);		// 0:Unknown 1:Up 2:Down
		nValue2 = dREAD_DIGITAL(BM2_PinUpDownRXI, &nIOStatus);
		if(nValue != nValue2)
		{
			WAIT_SEC(1);
			dWRITE_DIGITAL(BM2_PinUpDownRXI, nValue, &nIOStatus);
		}
	} while(1);
}

//////////////////////////////////////////////////////////////////////////
//... 2015.09.08 Add BM & TM Overpress Condition
void CTMSimulator::TmVacuumSimMonitor(int nThreadNo)
{
	int		nIOStatus;
	double	dbChmPrs;
	int		nPumpValve, nVentValve, nOverPrsValve, nSlotValveL, nSlotValveR;
	int		nUPCSet = 0, nUPCFlow = 0;
	int		i = 0;

	enum { eROUGHVLV_CLOSE, eROUGHVLV_SLOW, eROUGHVLV_FAST, eROUGHVLV_ALL };
	enum { eVLV_CLOSE, eVLV_OPEN };
	enum { eSLITVLV_UNKNOWN, eSLITVLV_OPEN, eSLITVLV_CLOSE, eSLITVLV_ERROR	};

	//... Init Value
	dWRITE_DIGITAL(BM1_DPStsDI,			1,		&nIOStatus);
	dWRITE_DIGITAL(BM1_DPWarnStsDI,		1,		&nIOStatus);
	dWRITE_DIGITAL(BM1_DPAlarmStsDI,	1,		&nIOStatus);
	dWRITE_ANALOG (BM1_CGPumpPrsAI,		0.001,	&nIOStatus);
	dWRITE_ANALOG (BM1_CGChmPrsAI,		760,	&nIOStatus);

	dWRITE_DIGITAL(BM2_DPStsDI,			1,		&nIOStatus);
	dWRITE_DIGITAL(BM2_DPWarnStsDI,		1,		&nIOStatus);
	dWRITE_DIGITAL(BM2_DPAlarmStsDI,	1,		&nIOStatus);
	dWRITE_ANALOG (BM2_CGPumpPrsAI,		0.001,	&nIOStatus);
	dWRITE_ANALOG (BM2_CGChmPrsAI,		0.002,	&nIOStatus);

	dWRITE_DIGITAL(TM_DPStsDI,			1,		&nIOStatus);
	dWRITE_DIGITAL(TM_DPWarnStsDI,		1,		&nIOStatus);
	dWRITE_DIGITAL(TM_DPAlarmStsDI,		1,		&nIOStatus);
	dWRITE_ANALOG (TM_CGPumpPrsAI,		0.001,	&nIOStatus);
	dWRITE_ANALOG (TM_CGChmPrsAI,		0.002,	&nIOStatus);
	dWRITE_DIGITAL(TM_RoughVvXO,		2,		&nIOStatus);

	do {
		if(! WAIT_SEC(1))		break;

		//////////////////////////////////////////////////////////////////////////
		// BM1
		dbChmPrs		= dREAD_ANALOG(BM1_CGChmPrsAI,		&nIOStatus);
		nPumpValve		= dREAD_DIGITAL(BM1_RoughVvXO,		&nIOStatus);
		nVentValve		= dREAD_DIGITAL(BM1_VentVvXO,		&nIOStatus);
		nOverPrsValve	= dREAD_DIGITAL(BM1_OverPresVvDO,	&nIOStatus);
		nSlotValveL		= dREAD_DIGITAL(TM_BM1_SlotVvLXI,	&nIOStatus);
		nSlotValveR		= dREAD_DIGITAL(TM_BM1_SlotVvRXI,	&nIOStatus);

		if(eROUGHVLV_CLOSE != nPumpValve)
		{	//... Case Pumping (Target 15 sec)
			if     (dbChmPrs > 100)		dbChmPrs -= 123.121;
			else if(dbChmPrs > 10)		dbChmPrs -= 7.721;
			else						dbChmPrs -= 1.641;

			if(dbChmPrs < 0.002)		dbChmPrs = 0.003;	//... Fixed Base Pressure

			//... Case Over Pressure Opened
			if(eVLV_OPEN == nOverPrsValve && dbChmPrs < 0.6)	dbChmPrs = 0.6;
		}
		else if(eROUGHVLV_CLOSE != nVentValve)
		{	//... Case Venting (Target 25 sec)
			if     (dbChmPrs > 100)		dbChmPrs += 123.121;
			else if(dbChmPrs > 10)		dbChmPrs += 17.721;
			else						dbChmPrs += 3.641;

			if(dbChmPrs > 761)			dbChmPrs = 761;
		}

		//... Case Slot Valve Opened for TM Swap in VAC
		if(eSLITVLV_OPEN == nSlotValveL || eSLITVLV_OPEN == nSlotValveR)
		{	//... TM Pressure
			if(eROUGHVLV_CLOSE == nPumpValve && eVLV_CLOSE == nOverPrsValve)
				dbChmPrs = dREAD_ANALOG(TM_CGChmPrsAI, &nIOStatus);
		}

		//... Set Pressure
		dWRITE_ANALOG(BM1_CGChmPrsAI, dbChmPrs, &nIOStatus);

		if     (dbChmPrs < 100)		dWRITE_DIGITAL(BM1_ATMVACSensorXI, 2, &nIOStatus);
		else if(dbChmPrs > 600)		dWRITE_DIGITAL(BM1_ATMVACSensorXI, 1, &nIOStatus);
		else						dWRITE_DIGITAL(BM1_ATMVACSensorXI, 0, &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		// BM2
		dbChmPrs		= dREAD_ANALOG(BM2_CGChmPrsAI,		&nIOStatus);
		nPumpValve		= dREAD_DIGITAL(BM2_RoughVvXO,		&nIOStatus);
		nVentValve		= dREAD_DIGITAL(BM2_VentVvXO,		&nIOStatus);
		nOverPrsValve	= dREAD_DIGITAL(BM2_OverPresVvDO,	&nIOStatus);
		nSlotValveL		= dREAD_DIGITAL(TM_BM2_SlotVvLXI,	&nIOStatus);
		nSlotValveR		= dREAD_DIGITAL(TM_BM2_SlotVvRXI,	&nIOStatus);
		
		if(eROUGHVLV_CLOSE != nPumpValve)
		{	//... Case Pumping (Target 15 sec)
			if     (dbChmPrs > 100)		dbChmPrs -= 123.121;
			else if(dbChmPrs > 10)		dbChmPrs -= 7.721;
			else						dbChmPrs -= 1.641;

			if(dbChmPrs < 0.002)		dbChmPrs = 0.003;	//... Fixed Base Pressure

			//... Case Over Pressure Opened
			if(eVLV_OPEN == nOverPrsValve && dbChmPrs < 0.6)	dbChmPrs = 0.6;
		}
		else if(eROUGHVLV_CLOSE != nVentValve)
		{	//... Case Venting (Target 25 sec)
			if     (dbChmPrs > 100)		dbChmPrs += 123.121;
			else if(dbChmPrs > 10)		dbChmPrs += 17.721;
			else						dbChmPrs += 3.641;

			if(dbChmPrs > 761)			dbChmPrs = 761;
		}
		
		//... Case Slot Valve Opened for TM Swap in VAC
		if(eSLITVLV_OPEN == nSlotValveL || eSLITVLV_OPEN == nSlotValveR)
		{	//... TM Pressure
			if(eROUGHVLV_CLOSE == nPumpValve && eVLV_CLOSE == nOverPrsValve)
				dbChmPrs = dREAD_ANALOG(TM_CGChmPrsAI, &nIOStatus);
		}
		
		//... Set Pressure
		dWRITE_ANALOG(BM2_CGChmPrsAI, dbChmPrs, &nIOStatus);
		
		if     (dbChmPrs < 100)		dWRITE_DIGITAL(BM2_ATMVACSensorXI, 2, &nIOStatus);
		else if(dbChmPrs > 600)		dWRITE_DIGITAL(BM2_ATMVACSensorXI, 1, &nIOStatus);
		else						dWRITE_DIGITAL(BM2_ATMVACSensorXI, 0, &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		// TM
		dbChmPrs		= dREAD_ANALOG(TM_CGChmPrsAI,		&nIOStatus);
		nPumpValve		= dREAD_DIGITAL(TM_RoughVvXO,		&nIOStatus);
		nVentValve		= dREAD_DIGITAL(TM_VentVvXO,		&nIOStatus);
		nOverPrsValve	= dREAD_DIGITAL(TM_OverPresVvDO,	&nIOStatus);

		if(eROUGHVLV_CLOSE != nPumpValve)
		{	//... Case Pumping 
			if     (dbChmPrs > 100)		dbChmPrs -= 46.121;
			else if(dbChmPrs > 10)		dbChmPrs -= 4.421;
			else						dbChmPrs -= 0.341;
			
			if(dbChmPrs < 0.003)		dbChmPrs = 0.002;	//... Fixed Base Pressure
		}
		else if(eROUGHVLV_CLOSE != nVentValve)
		{	//... Case Venting
			if     (dbChmPrs > 100)		dbChmPrs += 46.135;
			else if(dbChmPrs > 10)		dbChmPrs += 4.431;
			else						dbChmPrs += 0.337;
			
			if(dbChmPrs > 761)			dbChmPrs = 761;
		}
		
		//... Case Slot Valve Opened for PM Swap in VAC
		for (i = 0; i < MAX_PROCESS_MODULE; i++)
		{
			nSlotValveL = dREAD_DIGITAL(TM_PM1_SlotVv1XI + (i * MAX_ISO_VALVE), &nIOStatus);
			nSlotValveR = dREAD_DIGITAL(TM_PM1_SlotVv2XI + (i * MAX_ISO_VALVE), &nIOStatus);
			if(eSLITVLV_OPEN == nSlotValveL || eSLITVLV_OPEN == nSlotValveR)
			{
				if(eROUGHVLV_CLOSE == nPumpValve && eVLV_CLOSE == nOverPrsValve)
				{
					if     (i == 0)		dbChmPrs = dREAD_ANALOG(PM1_ChmPrsAI, &nIOStatus);
					else if(i == 1)		dbChmPrs = dREAD_ANALOG(PM2_ChmPrsAI, &nIOStatus);
					else if(i == 2)		dbChmPrs = dREAD_ANALOG(PM3_ChmPrsAI, &nIOStatus);
				}
				break;
			}
		}
		
		//... Case Over Pressure
		if(eVLV_OPEN == nOverPrsValve)
		{
			nUPCSet = (int) dREAD_ANALOG(TM_UPC_Set, &nIOStatus);
			if(nUPCSet != nUPCFlow)
			{
				nUPCFlow = nUPCSet;
				dWRITE_ANALOG(TM_UPC_Flow, nUPCFlow, &nIOStatus);
			}
			
			//... Case Over Pressure Opened
			if(dbChmPrs < (nUPCFlow * 0.001))		dbChmPrs = nUPCFlow * 0.001;
		}
		else
		{
			 if(nUPCFlow != 0)
			 {
				 nUPCFlow = 0;
				 dWRITE_ANALOG(TM_UPC_Flow, nUPCFlow, &nIOStatus);
			 }
		}

		//... Set Pressure
		dWRITE_ANALOG(TM_CGChmPrsAI, dbChmPrs, &nIOStatus);
		
		if     (dbChmPrs < 100)		dWRITE_DIGITAL(TM_ATMVACSensorXI, 2, &nIOStatus);
		else if(dbChmPrs > 600)		dWRITE_DIGITAL(TM_ATMVACSensorXI, 1, &nIOStatus);
		else						dWRITE_DIGITAL(TM_ATMVACSensorXI, 0, &nIOStatus);

	} while(1);
}

//////////////////////////////////////////////////////////////////////////
void CTMSimulator::SlotValveSimMonitor(int nThreadNo)
{
	/* Slot Valve Simulator Move to SIM-Value.int

	int nValue;
	int nValue2;
	int nIOStatus;
	int	i = 0;


	do {
		if(! WAIT_SEC(1)) break;

		//////////////////////////////////////////////////////////////////////////
		//... Slot Valve Control
		for(i = 0; i < MAX_ISO_VALVE; i++)
		{	// PM1
			nValue  = dREAD_DIGITAL(TM_PM1_SlotVv1XO + i, &nIOStatus);		// 0:Unknown 1:Open 2:Close
			nValue2 = dREAD_DIGITAL(TM_PM1_SlotVv1XI + i, &nIOStatus);
			if(nValue != nValue2)	dWRITE_DIGITAL(TM_PM1_SlotVv1XI + i, nValue, &nIOStatus);
		}

		for(i = 0; i < MAX_ISO_VALVE; i++)
		{	// PM2
			nValue  = dREAD_DIGITAL(TM_PM2_SlotVv1XO + i, &nIOStatus);		// 0:Unknown 1:Open 2:Close
			nValue2 = dREAD_DIGITAL(TM_PM2_SlotVv1XI + i, &nIOStatus);
			if(nValue != nValue2)	dWRITE_DIGITAL(TM_PM2_SlotVv1XI + i, nValue, &nIOStatus);
		}

		for(i = 0; i < MAX_ISO_VALVE; i++)
		{	// PM3
			nValue  = dREAD_DIGITAL(TM_PM3_SlotVv1XO + i, &nIOStatus);		// 0:Unknown 1:Open 2:Close
			nValue2 = dREAD_DIGITAL(TM_PM3_SlotVv1XI + i, &nIOStatus);
			if(nValue != nValue2)	dWRITE_DIGITAL(TM_PM3_SlotVv1XI + i, nValue, &nIOStatus);
		}

		for(i = 0; i < MAX_ISO_VALVE; i++)
		{	// BM1
			nValue  = dREAD_DIGITAL(TM_BM1_SlotVvLXO + i, &nIOStatus);		// 0:Unknown 1:Open 2:Close
			nValue2 = dREAD_DIGITAL(TM_BM1_SlotVvLXI + i, &nIOStatus);
			if(nValue != nValue2)	dWRITE_DIGITAL(TM_BM1_SlotVvLXI + i, nValue, &nIOStatus);
		}

		for(i = 0; i < MAX_ISO_VALVE; i++)
		{	// BM2
			nValue  = dREAD_DIGITAL(TM_BM2_SlotVvLXO + i, &nIOStatus);		// 0:Unknown 1:Open 2:Close
			nValue2 = dREAD_DIGITAL(TM_BM2_SlotVvLXI + i, &nIOStatus);
			if(nValue != nValue2)	dWRITE_DIGITAL(TM_BM2_SlotVvLXI + i, nValue, &nIOStatus);
		}

		//////////////////////////////////////////////////////////////////////////
		//... Door Valve Control
		for(i = 0; i < MAX_ISO_VALVE; i++)
		{
			nValue  = dREAD_DIGITAL(BM1_DoorLXO + i, &nIOStatus);		// 0:Unknown 1:Open 2:Close
			nValue2 = dREAD_DIGITAL(BM1_DoorLXI + i, &nIOStatus);
			if(nValue != nValue2)	dWRITE_DIGITAL(BM1_DoorLXI + i, nValue, &nIOStatus);
		}

		for(i = 0; i < MAX_ISO_VALVE; i++)
		{
			nValue  = dREAD_DIGITAL(BM2_DoorLXO + i, &nIOStatus);		// 0:Unknown 1:Open 2:Close
			nValue2 = dREAD_DIGITAL(BM2_DoorLXI + i, &nIOStatus);
			if(nValue != nValue2)	dWRITE_DIGITAL(BM2_DoorLXI + i, nValue, &nIOStatus);
		}

	} while(1);

	*/
}

//==========================================================================================
// 2005.03.18 by cskim

void CTMSimulator::IsoValveSyncSimMonitor(int nThreadNo)
{
	int nIOStatus;
	int nValue;
	
	int nPM1_IsoVlv[MAX_ISO_VALVE];
	int nPM2_IsoVlv[MAX_ISO_VALVE];
	int nPM3_IsoVlv[MAX_ISO_VALVE];

	int	i = 0;

	do {
		if(! WAIT_SEC(1)) break;

		//////////////////////////////////////////////////////////////////////////
		// PM1
		if(m_bPM1IsoVlvSync)
		{
			for(i = 0; i < MAX_ISO_VALVE; i++)
			{
				nValue = dREAD_DIGITAL(TM_PM1_SlotVv1XI + i, &nIOStatus);	// 0:Unknown 1:Open 2:Close 3:Error
				if(nValue != nPM1_IsoVlv[i])
				{
					if(nValue == 2)	dWRITE_DIGITAL(PM1_TMIsoVlvClosedDI + i, 1, &nIOStatus);
					else			dWRITE_DIGITAL(PM1_TMIsoVlvClosedDI + i, 0, &nIOStatus);
				}
				nPM1_IsoVlv[i] = nValue;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// PM2
		if(m_bPM2IsoVlvSync)
		{
			for(i = 0; i < MAX_ISO_VALVE; i++)
			{
				nValue = dREAD_DIGITAL(TM_PM2_SlotVv1XI + i, &nIOStatus);	// 0:Unknown 1:Open 2:Close 3:Error
				if(nValue != nPM2_IsoVlv[i])
				{
					if(nValue == 2)	dWRITE_DIGITAL(PM2_TMIsoVlvClosedDI + i, 1, &nIOStatus);
					else			dWRITE_DIGITAL(PM2_TMIsoVlvClosedDI + i, 0, &nIOStatus);
				}
				nPM2_IsoVlv[i] = nValue;
			}
		}
	
		//////////////////////////////////////////////////////////////////////////
		// PM3
		if(m_bPM3IsoVlvSync)
		{
			for(i = 0; i < MAX_ISO_VALVE; i++)
			{
				nValue = dREAD_DIGITAL(TM_PM3_SlotVv1XI + i, &nIOStatus);	// 0:Unknown 1:Open 2:Close 3:Error
				if(nValue != nPM3_IsoVlv[i])
				{
					if(nValue == 2)	dWRITE_DIGITAL(PM3_TMIsoVlvClosedDI + i, 1, &nIOStatus);
					else			dWRITE_DIGITAL(PM3_TMIsoVlvClosedDI + i, 0, &nIOStatus);
				}
				nPM3_IsoVlv[i] = nValue;
			}
		}

	} while(1);
}

void CTMSimulator::AlarmClear(int nThreadNo)
{
	int i;
	WAIT_SEC(3);
	for(i=0; i<m_nAlarmClearCount; i++)
	{
		if(ALM_PAUSED == ALARM_STATUS(m_pnAlarmClearList[i])) ALARM_CLEAR(m_pnAlarmClearList[i]);
	}
}
