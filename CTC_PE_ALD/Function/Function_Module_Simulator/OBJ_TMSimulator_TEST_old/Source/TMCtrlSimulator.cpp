// TMCtrlSimulator.cpp: implementation of the CTMCtrlSimulator class.
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
#include "TMCtrlSimulator.h"

//--------------------------------------------------------------------
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	SlowRoughVvDO_BM1	,
	FastRoughVvDO_BM1	,
	SlowVentVvDO_BM1	,
	FastVentVvDO_BM1	,
	BM1_CGChmPrsAI		,
	BM1_DoorLXO			,
	BM1_DoorLXI			,
	BM1_DoorRXO			,
	BM1_DoorRXI			,
	BM1_VacuumStatusDM	,
	BM1_ATMVACSensorXI	,

	BM1_PinUpDownLXO,
	BM1_PinUpDownLXI,
	BM1_PinUpDownRXO,
	BM1_PinUpDownRXI,

	SlowRoughVvDO_BM2	,
	FastRoughVvDO_BM2	,
	SlowVentVvDO_BM2	,
	FastVentVvDO_BM2	,
	BM2_CGChmPrsAI		,
	BM2_DoorLXO			,
	BM2_DoorLXI			,
	BM2_DoorRXO			,
	BM2_DoorRXI			,
	BM2_VacuumStatusDM  ,
	BM2_ATMVACSensorXI  ,

	BM2_PinUpDownLXO,
	BM2_PinUpDownLXI,
	BM2_PinUpDownRXO,
	BM2_PinUpDownRXI,

	BM1_SlotValveLXO    ,
	BM1_SlotValveRXO    ,
	BM2_SlotValveLXO    ,
	BM2_SlotValveRXO    ,
	PM1_SlotValve1XO	,
	PM1_SlotValve2XO	,
	PM2_SlotValve1XO	,
	PM2_SlotValve2XO	,
	PM3_SlotValve1XO	,
	PM3_SlotValve2XO	,

	BM1_SlotValveLXI	,
	BM1_SlotValveRXI	,
	BM2_SlotValveLXI	,
	BM2_SlotValveRXI	,
	PM1_SlotValve1XI	,
	PM1_SlotValve2XI	,
	PM2_SlotValve1XI	,
	PM2_SlotValve2XI	,
	PM3_SlotValve1XI	,
	PM3_SlotValve2XI	,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{ "BM1.SlowRoughVvDO"			, _K_D_IO,	0	} ,
	{ "BM1.FastRoughVvDO"			, _K_D_IO,	0	} ,
	{ "BM1.SlowVentVvDO"			, _K_D_IO,	0	} ,
	{ "BM1.FastVentVvDO"			, _K_D_IO,	0	} ,
	{ "BM1.CGChmPrsAI"				, _K_A_IO,	0	} ,
	{ "BM1.DoorLXO"					, _K_D_IO,	0	} ,
	{ "BM1.DoorLXI"					, _K_D_IO,	0	} ,
	{ "BM1.DoorRXO"					, _K_D_IO,	0	} ,
	{ "BM1.DoorRXI"					, _K_D_IO,	0	} ,
	{ "BM1.VacuumStatusDM"			, _K_D_IO,	0	} ,
	{ "BM1.ATMVACSensorXI"			, _K_D_IO,	0	} ,

	{ "BM1.PinUpDownLXO"			, _K_D_IO,	0	} ,
	{ "BM1.PinUpDownLXI"			, _K_D_IO,	0	} ,
	{ "BM1.PinUpDownRXO"			, _K_D_IO,	0	} ,
	{ "BM1.PinUpDownRXI"			, _K_D_IO,	0	} ,

	{ "BM2.SlowRoughVvDO"			, _K_D_IO,	0	} ,
	{ "BM2.FastRoughVvDO"			, _K_D_IO,	0	} ,
	{ "BM2.SlowVentVvDO"			, _K_D_IO,	0	} ,
	{ "BM2.FastVentVvDO"			, _K_D_IO,	0	} ,
	{ "BM2.CGChmPrsAI"				, _K_A_IO,	0	} ,
	{ "BM2.DoorLXO"					, _K_D_IO,	0	} ,
	{ "BM2.DoorLXI"					, _K_D_IO,	0	} ,
	{ "BM2.DoorRXO"					, _K_D_IO,	0	} ,
	{ "BM2.DoorRXI"					, _K_D_IO,	0	} ,
	{ "BM2.VacuumStatusDM"			, _K_D_IO,	0	} ,
	{ "BM2.ATMVACSensorXI"			, _K_D_IO,	0	} ,

	{ "BM2.PinUpDownLXO"			, _K_D_IO,	0	} ,
	{ "BM2.PinUpDownLXI"			, _K_D_IO,	0	} ,
	{ "BM2.PinUpDownRXO"			, _K_D_IO,	0	} ,
	{ "BM2.PinUpDownRXI"			, _K_D_IO,	0	} ,

	{ "TM.BM1.SlotVvLXO"			, _K_D_IO,	0	} ,
	{ "TM.BM1.SlotVvRXO"			, _K_D_IO,	0	} ,
	{ "TM.BM2.SlotVvLXO"			, _K_D_IO,	0	} ,
	{ "TM.BM2.SlotVvRXO"			, _K_D_IO,	0	} ,
	{ "TM.PM1.SlotVv1XO"			, _K_D_IO,	0	} ,
	{ "TM.PM1.SlotVv2XO"			, _K_D_IO,	0	} ,
	{ "TM.PM2.SlotVv1XO"			, _K_D_IO,	0	} ,
	{ "TM.PM2.SlotVv2XO"			, _K_D_IO,	0	} ,
	{ "TM.PM3.SlotVv1XO"			, _K_D_IO,	0	} ,
	{ "TM.PM3.SlotVv2XO"			, _K_D_IO,	0	} ,

	{ "TM.BM1.SlotVvLXI"			, _K_D_IO,	0	} ,
	{ "TM.BM1.SlotVvRXI"			, _K_D_IO,	0	} ,
	{ "TM.BM2.SlotVvLXI"			, _K_D_IO,	0	} ,
	{ "TM.BM2.SlotVvRXI"			, _K_D_IO,	0	} ,
	{ "TM.PM1.SlotVv1XI"			, _K_D_IO,	0	} ,
	{ "TM.PM1.SlotVv2XI"			, _K_D_IO,	0	} ,
	{ "TM.PM2.SlotVv1XI"			, _K_D_IO,	0	} ,
	{ "TM.PM2.SlotVv2XI"			, _K_D_IO,	0	} ,
	{ "TM.PM3.SlotVv1XI"			, _K_D_IO,	0	} ,
	{ "TM.PM3.SlotVv2XI"			, _K_D_IO,	0	} ,
	""
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
#define SIM_CFG_FILE		".\\Function\\Function_Module_Simulator\\Simulation.cfg"

//--------------------------------------------------------------------
typedef struct _TIME_TABLE
{
	char *szCmd;
	double dbTime;
} TIME_TABLE;

static TIME_TABLE g_TimeTable[] =
{
	{"CM READY",					1.0},
	{"CM MAPPING",					5.0},
	{"CM LOAD",						3.0},
	{"CM UNLOAD",					3.0},
	{"CM UNLOAD_OPTION",			3.0},

	{"ATM_ROBOT READY",					1.0},
	{"ATM_ROBOT MAPPING",				5.0},
	{"ALIGNER",							3.8},
	{"ATM_ROBOT ROTATE A_CM",			1.6},
	{"ATM_ROBOT PICK A_CM",			    3.1},
	{"ATM_ROBOT PICK B_CM",				3.5},
	{"ATM_ROBOT PLACE A_CM",			3.1},
	{"ATM_ROBOT PLACE B_CM",			3.4},
	{"ATM_ROBOT ROTATE A_BM",			0.4},
	{"ATM_ROBOT ROTATE B_BM",		    1.6},
	{"ATM_ROBOT PICK A_BM",				3.3},
	{"ATM_ROBOT PICK B_BM",				3.3},
	{"ATM_ROBOT PLACE A_BM",			3.1},
	{"ATM_ROBOT PLACE B_BM",		    3.2},
	{"ATM_ROBOT ROTATE A_AL",			1.2},
	{"ATM_ROBOT PICK A_AL",				3.2},
	{"ATM_ROBOT PICK B_AL",				3.1},
	{"ATM_ROBOT PLACE A_AL",			2.8},
	{"ATM_ROBOT PLACE B_AL",			2.9},

	{"VAC_ROBOT READY",					1.0},
	{"VAC_ROBOT ROTATE A_PM",			3.0},
	{"VAC_ROBOT ROTATE B_PM",			3.0},
	{"VAC_ROBOT ROTATE A_BM",			3.0},
	{"VAC_ROBOT ROTATE B_BM",			3.0},
	{"VAC_ROBOT ROTATE SAME_MOD",		0.5},				// Re-rotate to same module
	{"VAC_ROBOT PICK A_PM",				4.5},
	{"VAC_ROBOT PICK B_PM",				4.5},
	{"VAC_ROBOT PLACE A_PM",			4.5},
	{"VAC_ROBOT PLACE B_PM",			4.5},
	{"VAC_ROBOT PICK A_BM",				4.5},
	{"VAC_ROBOT PICK B_BM",				4.5},
	{"VAC_ROBOT PLACE A_BM",			4.5},
	{"VAC_ROBOT PLACE B_BM",			4.5},
	{"VAC_ROBOT ROTATE BM_PM1",			2.0},
	{"VAC_ROBOT ROTATE BM_PM2",			2.0},
	{"VAC_ROBOT ROTATE BM_PM3",			2.0},
	{"VAC_ROBOT ROTATE BM_PM4",			2.0},
	{"VAC_ROBOT ROTATE PM1_PM2",		2.0},
	{"VAC_ROBOT ROTATE PM1_PM3",		2.0},
	{"VAC_ROBOT ROTATE PM1_PM4",		2.0},
	{"VAC_ROBOT ROTATE PM2_PM3",		2.0},
	{"VAC_ROBOT ROTATE PM2_PM4",		2.0},
	{"VAC_ROBOT ROTATE PM3_PM4",		2.0},
	// 2006.01.05
	{"VAC_ROBOT ROTPIC A_PM",			5.5},
	{"VAC_ROBOT ROTPIC B_PM",			5.5},
	{"VAC_ROBOT ROTPLC A_PM",			5.5},
	{"VAC_ROBOT ROTPLC B_PM",			5.5},
	{"VAC_ROBOT ROTPIC A_BM",			5.5},
	{"VAC_ROBOT ROTPIC B_BM",			5.5},
	{"VAC_ROBOT ROTPLC A_BM",			5.5},
	{"VAC_ROBOT ROTPLC B_BM",			5.5},

	{"BM1 READY",						1.0},
	{"BM1 PUMP",						24.3},
	{"BM1 VENT",						45.0},
	{"BM1 DOOR OPEN",					2.0},
	{"BM1 DOOR CLOSE",					2.0},
	{"BM2 READY",						1.0},
	{"BM2 PUMP",						24.3},
	{"BM2 VENT",						45.0},
	{"BM2 DOOR OPEN",					2.0},
	{"BM2 DOOR CLOSE",					2.0},

	{"ISO_GATE OPEN	BM1",				1.5},
	{"ISO_GATE CLOSE BM1",				1.5},
	{"ISO_GATE START_CLOSE BM1",		0.05},
	{"ISO_GATE OPEN	BM2",				1.5},
	{"ISO_GATE CLOSE BM2",				1.5},
	{"ISO_GATE START_CLOSE BM2",		0.05},
	{"ISO_GATE OPEN	PM1",				1.5},
	{"ISO_GATE CLOSE PM1",				1.5},
	{"ISO_GATE START_CLOSE PM1",		0.05},
	{"ISO_GATE OPEN	PM2",				1.5},
	{"ISO_GATE CLOSE PM2",				1.5},
	{"ISO_GATE START_CLOSE PM2",		0.05},
	{"ISO_GATE OPEN	PM3",				1.5},
	{"ISO_GATE CLOSE PM3",				1.5},
	{"ISO_GATE START_CLOSE PM3",		0.05},
	{"ISO_GATE OPEN	PM4",				1.5},
	{"ISO_GATE CLOSE PM4",				1.5},
	{"ISO_GATE START_CLOSE PM4",		0.05},
	{"ISO_GATE OPEN	PM5",				1.5},
	{"ISO_GATE CLOSE PM5",				1.5},
	{"ISO_GATE START_CLOSE PM5",		0.05},
	""
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTMCtrlSimulator::CTMCtrlSimulator()
{
	m_szComponentName[0] = 0;
	m_OldDest[0] = 0;
}

CTMCtrlSimulator::CTMCtrlSimulator(char *szComponentName)
{
	strcpy(m_szComponentName, szComponentName);
	#ifdef _DEBUG
	printf("-----> Component Name = %s in TM Simulation Control\n", m_szComponentName);
	#endif // _DEBUG
}

CTMCtrlSimulator::~CTMCtrlSimulator()
{
}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CTMCtrlSimulator::Initialize()
{
	InitIOList(g_pIOList, FALSE);
	ReadSimConfig();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CTMCtrlSimulator::Main(char *szParam)
{
	BOOL bRet = TRUE;
	CTextParser tp(szParam);
	_LOG("%s Started", szParam);
	tp.GetWord();
	if(tp.BeEqual("CM", 2))				bRet = ControlCM(szParam);
	else if(tp.BeEqual("ATM_ROBOT"))	bRet = ControlAtmRobot(szParam);
	else if(tp.BeEqual("VAC_ROBOT"))	bRet = ControlVacRobot(szParam);
	else if(tp.BeEqual("ALIGNER"))		bRet = ControlAligner(szParam);
	else if(tp.BeEqual("BM", 2))		bRet = ControlBM(szParam);
	else if(tp.BeEqual("ISO_GATE"))		bRet = ControlIsoGate(szParam);
	if(! bRet) _LOG("-----> %s Failed", szParam);
	_LOG("%s Finished", szParam);
	return bRet;
}

BOOL CTMCtrlSimulator::ReadSimConfig()
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

				if(strcmp("Control Simuation Configration", szItem) == 0)	nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseCtrlConfig(szRead); break;
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

void CTMCtrlSimulator::ParseCtrlConfig(char *szRead)
{
	if(0 == strlen(szRead)) return;
	char szItem[40] = "";
	double dbValue = 0.0;
	char *pCmd;


	CTextParser tp(szRead);
	if(! tp.GetWord(szItem)) return;
	if(! tp.GetDbFloat(&dbValue)) return;

	if(tp.BeEqual(szItem, "ATM_Robot_Mapping_Time"))			pCmd = "ATM_ROBOT MAPPING";
	else if(tp.BeEqual(szItem, "ALGN_Align_Time")) 			  pCmd = "ALIGNER";
	else if(tp.BeEqual(szItem, "ATM_Robot_Rot_A_CM_Time")) 		  pCmd = "ATM_ROBOT ROTATE A_CM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPick_A_CM_Time")) 	  pCmd = "ATM_ROBOT PICK A_CM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPick_B_CM_Time")) 	  pCmd = "ATM_ROBOT PICK B_CM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPlace_A_CM_Time"))	  pCmd = "ATM_ROBOT PLACE A_CM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPlace_B_CM_Time"))	  pCmd = "ATM_ROBOT PLACE B_CM";
	else if(tp.BeEqual(szItem, "ATM_Robot_Rot_A_BM_Time")) 		  pCmd = "ATM_ROBOT ROTATE A_BM";
	else if(tp.BeEqual(szItem, "ATM_Robot_Rot_B_BM_Time")) 		  pCmd = "ATM_ROBOT ROTATE B_BM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPick_A_BM_Time"))	  pCmd = "ATM_ROBOT PICK A_BM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPick_B_BM_Time"))	  pCmd = "ATM_ROBOT PICK B_BM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPlace_A_BM_Time"))	  pCmd = "ATM_ROBOT PLACE A_BM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPlace_B_BM_Time"))   pCmd = "ATM_ROBOT PLACE B_BM";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotAligner_Time")) 	  pCmd = "ATM_ROBOT ROTATE A_AL";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPick_A_Aligner_Time"))   pCmd = "ATM_ROBOT PICK A_AL";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPick_B_Aligner_Time"))   pCmd = "ATM_ROBOT PICK B_AL";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPlace_A_Aligner_Time"))  pCmd = "ATM_ROBOT PLACE A_AL";
	else if(tp.BeEqual(szItem, "ATM_Robot_RotPlace_B_Aligner_Time"))  pCmd = "ATM_ROBOT PLACE B_AL";

	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_A_PM_Time"))	pCmd = "VAC_ROBOT ROTATE A_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_B_PM_Time"))	pCmd = "VAC_ROBOT ROTATE B_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_A_BM_Time"))	pCmd = "VAC_ROBOT ROTATE A_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_B_BM_Time"))	pCmd = "VAC_ROBOT ROTATE B_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_SameMod_Time")) pCmd = "VAC_ROBOT ROTATE SAME_MOD";
	else if(tp.BeEqual(szItem, "VAC_Robot_Pick_A_PM_Time"))		pCmd = "VAC_ROBOT PICK A_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Pick_B_PM_Time"))		pCmd = "VAC_ROBOT PICK B_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Place_A_PM_Time"))	pCmd = "VAC_ROBOT PLACE A_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Place_B_PM_Time"))	pCmd = "VAC_ROBOT PLACE B_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Pick_A_BM_Time"))		pCmd = "VAC_ROBOT PICK A_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Pick_B_BM_Time"))		pCmd = "VAC_ROBOT PICK B_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Place_A_BM_Time"))	pCmd = "VAC_ROBOT PLACE A_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_Place_B_BM_Time"))	pCmd = "VAC_ROBOT PLACE B_BM";
	// 2006.01.05
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPick_A_PM_Time"))		pCmd = "VAC_ROBOT ROTPIC A_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPick_B_PM_Time"))		pCmd = "VAC_ROBOT ROTPIC B_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPlace_A_PM_Time"))		pCmd = "VAC_ROBOT ROTPLC A_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPlace_B_PM_Time"))		pCmd = "VAC_ROBOT ROTPLC B_PM";
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPick_A_BM_Time"))		pCmd = "VAC_ROBOT ROTPIC A_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPick_B_BM_Time"))		pCmd = "VAC_ROBOT ROTPIC B_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPlace_A_BM_Time"))		pCmd = "VAC_ROBOT ROTPLC A_BM";
	else if(tp.BeEqual(szItem, "VAC_Robot_RotPlace_B_BM_Time"))		pCmd = "VAC_ROBOT ROTPLC B_BM";

	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_BM_PM1"))		pCmd = "VAC_ROBOT ROTATE BM_PM1";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_BM_PM2"))		pCmd = "VAC_ROBOT ROTATE BM_PM2";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_BM_PM3"))		pCmd = "VAC_ROBOT ROTATE BM_PM3";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_BM_PM4"))		pCmd = "VAC_ROBOT ROTATE BM_PM4";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_PM1_PM2"))		pCmd = "VAC_ROBOT ROTATE PM1_PM2";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_PM1_PM3"))		pCmd = "VAC_ROBOT ROTATE PM1_PM3";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_PM1_PM4"))		pCmd = "VAC_ROBOT ROTATE PM1_PM4";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_PM2_PM3"))		pCmd = "VAC_ROBOT ROTATE PM2_PM3";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_PM2_PM4"))		pCmd = "VAC_ROBOT ROTATE PM2_PM4";
	else if(tp.BeEqual(szItem, "VAC_Robot_Rotate_PM3_PM4"))		pCmd = "VAC_ROBOT ROTATE PM3_PM4";

	else if(tp.BeEqual(szItem, "BM1_Pump_Time"))				pCmd = "BM1 PUMP";
	else if(tp.BeEqual(szItem, "BM1_Vent_Time"))				pCmd = "BM1 VENT";
	else if(tp.BeEqual(szItem, "BM1_Door_Open_Time"))			pCmd = "BM1 DOOR OPEN";
	else if(tp.BeEqual(szItem, "BM1_Door_Close_Time"))			pCmd = "BM1 DOOR CLOSE";
	else if(tp.BeEqual(szItem, "BM2_Pump_Time"))				pCmd = "BM2 PUMP";
	else if(tp.BeEqual(szItem, "BM2_Vent_Time"))				pCmd = "BM2 VENT";
	else if(tp.BeEqual(szItem, "BM2_Door_Open_Time"))			pCmd = "BM2 DOOR OPEN";
	else if(tp.BeEqual(szItem, "BM2_Door_Close_Time"))			pCmd = "BM2 DOOR CLOSE";

	else if(tp.BeEqual(szItem, "IsoGate_Open_BM1_Time"))		pCmd = "ISO_GATE OPEN BM1";
	else if(tp.BeEqual(szItem, "IsoGate_Close_BM1_Time"))		pCmd = "ISO_GATE CLOSE BM1";
	else if(tp.BeEqual(szItem, "IsoGate_Start_Close_BM1_Time"))	pCmd = "ISO_GATE START_CLOSE BM1";
	else if(tp.BeEqual(szItem, "IsoGate_Open_BM2_Time"))		pCmd = "ISO_GATE OPEN BM2";
	else if(tp.BeEqual(szItem, "IsoGate_Close_BM2_Time"))		pCmd = "ISO_GATE CLOSE BM2";
	else if(tp.BeEqual(szItem, "IsoGate_Start_Close_BM2_Time"))	pCmd = "ISO_GATE START_CLOSE BM2";
	else if(tp.BeEqual(szItem, "IsoGate_Open_PM1_Time"))		pCmd = "ISO_GATE OPEN PM1";
	else if(tp.BeEqual(szItem, "IsoGate_Close_PM1_Time"))		pCmd = "ISO_GATE CLOSE PM1";
	else if(tp.BeEqual(szItem, "IsoGate_Start_Close_PM1_Time"))	pCmd = "ISO_GATE START_CLOSE PM1";
	else if(tp.BeEqual(szItem, "IsoGate_Open_PM2_Time"))		pCmd = "ISO_GATE OPEN PM2";
	else if(tp.BeEqual(szItem, "IsoGate_Close_PM2_Time"))		pCmd = "ISO_GATE CLOSE PM2";
	else if(tp.BeEqual(szItem, "IsoGate_Start_Close_PM2_Time"))	pCmd = "ISO_GATE START_CLOSE PM2";
	else if(tp.BeEqual(szItem, "IsoGate_Open_PM3_Time"))		pCmd = "ISO_GATE OPEN PM3";
	else if(tp.BeEqual(szItem, "IsoGate_Close_PM3_Time"))		pCmd = "ISO_GATE CLOSE PM3";
	else if(tp.BeEqual(szItem, "IsoGate_Start_Close_PM3_Time"))	pCmd = "ISO_GATE START_CLOSE PM3";
	else if(tp.BeEqual(szItem, "IsoGate_Open_PM4_Time"))		pCmd = "ISO_GATE OPEN PM4";
	else if(tp.BeEqual(szItem, "IsoGate_Close_PM4_Time"))		pCmd = "ISO_GATE CLOSE PM4";
	else if(tp.BeEqual(szItem, "IsoGate_Start_Close_PM4_Time"))	pCmd = "ISO_GATE START_CLOSE PM4";
	else if(tp.BeEqual(szItem, "IsoGate_Open_PM5_Time"))		pCmd = "ISO_GATE OPEN PM5";
	else if(tp.BeEqual(szItem, "IsoGate_Close_PM5_Time"))		pCmd = "ISO_GATE CLOSE PM5";
	else if(tp.BeEqual(szItem, "IsoGate_Start_Close_PM5_Time"))	pCmd = "ISO_GATE START_CLOSE PM5";
	else
	{
		#ifdef _DEBUG
		printf("-----> Invalid Configration[%s] in TM Control Simulation Function\n", szRead);
		#endif // _DEBUG
		return;
	}

	tp.SetText(pCmd);
	tp.GetWord();
	if(strlen(m_szComponentName) <= 0 || tp.BeEqual(m_szComponentName)) SetTimeTable(pCmd, dbValue);
}

BOOL CTMCtrlSimulator::ControlCM(char *szCmd)
{
	BOOL bRet = FALSE;
	double dbWaitTime = 0;
	do {
		_LOG("-----> Module Name : %s", m_szComponentName);
		if(! GetTimeTable(szCmd, &dbWaitTime)) break;
		if(! WAIT_SECONDS(dbWaitTime)) break;
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CTMCtrlSimulator::ControlAtmRobot(char *szCmd)
{
	BOOL bRet = FALSE;
	double dbWaitTime = 0;
	do {
		if(! GetTimeTable(szCmd, &dbWaitTime)) dbWaitTime = 5;

		if(! WAIT_SECONDS(dbWaitTime)) break;
		_LOG("************* ATM Robot Wait Time %1.1f sec", dbWaitTime);
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CTMCtrlSimulator::ControlVacRobot(char *szCmd)
{
	static char szLastDest[20] = "";
	BOOL bRet = FALSE;
	double dbWaitTime = 0;
	do {
		CTextParser tp(szCmd);
		tp.MoveNextWord();
		tp.GetWord();
		if(tp.BeEqual("ROTATE"))
		{
			tp.GetWord();
			if(tp.BeEqual(szLastDest)) 	GetTimeTable("VAC_ROBOT ROTATE SAME_MOD", &dbWaitTime);
			else if(! GetTimeTable(szCmd, &dbWaitTime)) dbWaitTime = 5.0;
			strcpy(szLastDest, tp.m_szLastPickedWord);
		}
		else if(tp.BeEqual("ROTPIC") || tp.BeEqual("ROTPLC"))
		{
			tp.GetWord();

			_LOG("************* Old Destination ==> %s ", m_OldDest);
			if(tp.BeEqual(m_OldDest))
			{
				GetTimeTable("VAC_ROBOT ROTATE SAME_MOD", &dbWaitTime);
			}
			else if(tp.BeEqual("A_BM1") || tp.BeEqual("B_BM1") || tp.BeEqual("A_BM2") || tp.BeEqual("B_BM2") )
			{
				if(STRCMP(m_OldDest,"A_PM1") || STRCMP(m_OldDest,"B_PM1"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM1", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM2") || STRCMP(m_OldDest,"B_PM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM2", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM3") || STRCMP(m_OldDest,"B_PM3"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM3", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM4") || STRCMP(m_OldDest,"B_PM4"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_BM1") || STRCMP(m_OldDest,"A_BM2") 
					 || STRCMP(m_OldDest,"B_BM1") || STRCMP(m_OldDest,"B_BM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE SAME_MOD", &dbWaitTime);
				}
			}
			else if(tp.BeEqual("A_PM1") || tp.BeEqual("B_PM1"))
			{
				if(STRCMP(m_OldDest,"A_PM2") || STRCMP(m_OldDest,"B_PM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM1_PM2", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM3") || STRCMP(m_OldDest,"B_PM3"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM1_PM3", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM4") || STRCMP(m_OldDest,"B_PM4"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM1_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_BM1") || STRCMP(m_OldDest,"A_BM2") 
					 || STRCMP(m_OldDest,"B_BM1") || STRCMP(m_OldDest,"B_BM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM1", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM1") || STRCMP(m_OldDest,"B_PM1")) 
				{
					GetTimeTable("VAC_ROBOT ROTATE SAME_MOD", &dbWaitTime);
				}
			}
			else if(tp.BeEqual("A_PM2") || tp.BeEqual("B_PM2"))
			{
				if(STRCMP(m_OldDest,"A_PM1") || STRCMP(m_OldDest,"B_PM1"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM1_PM2", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM3") || STRCMP(m_OldDest,"B_PM3"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM2_PM3", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM4") || STRCMP(m_OldDest,"B_PM4"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM2_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_BM1") || STRCMP(m_OldDest,"A_BM2") 
					 || STRCMP(m_OldDest,"B_BM1") || STRCMP(m_OldDest,"B_BM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM2", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM2") || STRCMP(m_OldDest,"B_PM2")) 
				{
					GetTimeTable("VAC_ROBOT ROTATE SAME_MOD", &dbWaitTime);
				}
			}
			else if(tp.BeEqual("A_PM3") || tp.BeEqual("B_PM3"))
			{
				if(STRCMP(m_OldDest,"A_PM1") || STRCMP(m_OldDest,"B_PM1"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM1_PM3", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM2") || STRCMP(m_OldDest,"B_PM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM2_PM3", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM4") || STRCMP(m_OldDest,"B_PM4"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM3_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_BM1") || STRCMP(m_OldDest,"A_BM2") 
					 || STRCMP(m_OldDest,"B_BM1") || STRCMP(m_OldDest,"B_BM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM3", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM3") || STRCMP(m_OldDest,"B_PM3")) 
				{
					GetTimeTable("VAC_ROBOT ROTATE SAME_MOD", &dbWaitTime);
				}
			}
			else if(tp.BeEqual("A_PM4") || tp.BeEqual("B_PM4"))
			{
				if(STRCMP(m_OldDest,"A_PM1") || STRCMP(m_OldDest,"B_PM1"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM1_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM2") || STRCMP(m_OldDest,"B_PM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM2_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM3") || STRCMP(m_OldDest,"B_PM3"))
				{
					GetTimeTable("VAC_ROBOT ROTATE PM3_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_BM1") || STRCMP(m_OldDest,"A_BM2") 
					 || STRCMP(m_OldDest,"B_BM1") || STRCMP(m_OldDest,"B_BM2"))
				{
					GetTimeTable("VAC_ROBOT ROTATE BM_PM4", &dbWaitTime);
				}
				else if(STRCMP(m_OldDest,"A_PM4") || STRCMP(m_OldDest,"B_PM4")) 
				{
					GetTimeTable("VAC_ROBOT ROTATE SAME_MOD", &dbWaitTime);
				}
			}
			strcpy(m_OldDest, tp.m_szLastPickedWord);
			_LOG("************* New Destination ==> %s ", m_OldDest);
			_LOG("************* VTM ROBOT Wait Time %1.1f sec", dbWaitTime);
		}
		else if(! GetTimeTable(szCmd, &dbWaitTime)) dbWaitTime = 5.0;
		
		if(! WAIT_SECONDS(dbWaitTime)) break;
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CTMCtrlSimulator::ControlAligner(char *szCmd)
{
	BOOL bRet = FALSE;
	double dbWaitTime = 0;
	do {
		if(! GetTimeTable(szCmd, &dbWaitTime)) break;
		if(! WAIT_SECONDS(dbWaitTime)) break;
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CTMCtrlSimulator::ControlBM(char *szCmd)
{
	BOOL bRet = FALSE;
	BOOL bIOStatus;
	double dbWaitTime = 0;
	char szModule[8];
	char szTemp[80];
	int nSlowRoughValve;
	int nFastRoughValve;
	int nSlowVentValve;
	int nFastVentValve;
	int nChmPrsAI;
	int nVacuumStatusDM;
	int nATMVACSensorXI;
	int nLiftPinLXO;
	int nLiftPinRXO;
	int nLiftPinLXI;
	int nLiftPinRXI;

	do {
		CTextParser tp(szCmd);
		tp.GetWord();
		if(tp.BeEqual("BM1"))
		{
			nSlowRoughValve = SlowRoughVvDO_BM1;
			nFastRoughValve = FastRoughVvDO_BM1;
			nSlowVentValve	= SlowVentVvDO_BM1;
			nFastVentValve	= FastVentVvDO_BM1;
			nChmPrsAI		= BM1_CGChmPrsAI;
			nATMVACSensorXI = BM1_ATMVACSensorXI;
			nVacuumStatusDM = BM1_VacuumStatusDM;
			nLiftPinLXO     = BM1_PinUpDownLXO;
			nLiftPinRXO     = BM1_PinUpDownRXO;
			nLiftPinLXI     = BM1_PinUpDownLXI;
            nLiftPinRXI     = BM1_PinUpDownRXI;
			strcpy(szModule, "BM1");
		}
		else
		{
			nSlowRoughValve = SlowRoughVvDO_BM2;
			nFastRoughValve = FastRoughVvDO_BM2;
			nSlowVentValve	= SlowVentVvDO_BM2;
			nFastVentValve	= FastVentVvDO_BM2;
			nChmPrsAI		= BM2_CGChmPrsAI;
			nATMVACSensorXI = BM2_ATMVACSensorXI;
			nVacuumStatusDM = BM2_VacuumStatusDM;
			nLiftPinLXO     = BM2_PinUpDownLXO;
			nLiftPinRXO     = BM2_PinUpDownRXO;
			nLiftPinLXI     = BM2_PinUpDownLXI;
            nLiftPinRXI     = BM2_PinUpDownRXI;
			strcpy(szModule, "BM2");
		}

		tp.GetWord();
		if(tp.BeEqual("PUMP"))
		{
			if(dREAD_ANALOG(nChmPrsAI, &bIOStatus) < 10
				&& dREAD_DIGITAL(nATMVACSensorXI, &bIOStatus) == 2)
			{
				bRet = TRUE;
				break;
			}

			sprintf(szTemp, "%s DOOR CLOSE", szModule);
			if(! ControlBMDoor(szTemp)) break;
			
			dWRITE_DIGITAL(nVacuumStatusDM, 3, &bIOStatus);		// Go To Vacuum
			sprintf(szTemp, "%s PUMP", szModule);
			if(! GetTimeTable(szCmd, &dbWaitTime)) break;
			if(dREAD_ANALOG(nChmPrsAI, &bIOStatus) < 100) dbWaitTime = 0;

			dWRITE_DIGITAL(nSlowVentValve, 0, &bIOStatus);
			dWRITE_DIGITAL(nFastVentValve, 0, &bIOStatus);

			dWRITE_DIGITAL(nSlowRoughValve, 1, &bIOStatus);
			if(dbWaitTime > 5)
			{
				if(! WAIT_SECONDS(5)) break;
				dbWaitTime -= 5;
			}
			dWRITE_DIGITAL(nSlowRoughValve, 0, &bIOStatus);
			dWRITE_DIGITAL(nFastRoughValve, 1, &bIOStatus);
			if(! WAIT_SECONDS(dbWaitTime)) break;
			dWRITE_ANALOG(nChmPrsAI, 0.001, &bIOStatus);
			dWRITE_DIGITAL(nATMVACSensorXI, 2, &bIOStatus);		// Vacuum
			dWRITE_DIGITAL(nVacuumStatusDM, 1, &bIOStatus);		// Vacuum
		}
		else if(tp.BeEqual("VENT"))
		{
			if(dREAD_ANALOG(nChmPrsAI, &bIOStatus) > 700
				&& dREAD_DIGITAL(nATMVACSensorXI, &bIOStatus) == 1)
			{
				bRet = TRUE;
				return bRet;
			}
			
			sprintf(szTemp, "%s DOOR CLOSE", szModule);
			if(! ControlBMDoor(szTemp)) break;

			dWRITE_DIGITAL(nVacuumStatusDM, 2, &bIOStatus);		// Go To ATM
			sprintf(szTemp, "%s VENT", szModule);
			if(! GetTimeTable(szCmd, &dbWaitTime)) break;
			if(dREAD_ANALOG(nChmPrsAI, &bIOStatus) > 700) dbWaitTime = 0;

			dWRITE_DIGITAL(nSlowRoughValve, 0, &bIOStatus);
			dWRITE_DIGITAL(nFastRoughValve, 0, &bIOStatus);

			dWRITE_DIGITAL(nSlowVentValve, 1, &bIOStatus);
			if(dbWaitTime > 5)
			{
				if(! WAIT_SECONDS(5)) break;
				dbWaitTime -= 5;
			}
			dWRITE_DIGITAL(nSlowVentValve, 0, &bIOStatus);
			dWRITE_DIGITAL(nFastVentValve, 1, &bIOStatus);
			if(! WAIT_SECONDS(dbWaitTime)) break;
			dWRITE_DIGITAL(nFastVentValve, 0, &bIOStatus);
			dWRITE_ANALOG(nChmPrsAI, 760, &bIOStatus);
			dWRITE_DIGITAL(nATMVACSensorXI, 1, &bIOStatus);		// ATM
			dWRITE_DIGITAL(nVacuumStatusDM, 0, &bIOStatus);		// ATM
		}
		else if(tp.BeEqual("DOOR"))
		{
			tp.GetWord();
			sprintf(szTemp, "%s DOOR %s", szModule, tp.m_szLastPickedWord);
			if(! ControlBMDoor(szTemp)) break;

		}
		else if(tp.BeEqual("A_UP") || tp.BeEqual("UP")) {
			dWRITE_DIGITAL(nLiftPinLXO, 1, &bIOStatus);
			dWRITE_DIGITAL(nLiftPinRXO, 1, &bIOStatus);	
			if(! WAIT_SECONDS(2)) break;
		}
		else if(tp.BeEqual("A_DOWN") || tp.BeEqual("DOWN") || tp.BeEqual("CHK_WFR_BEFORE_DOWN")) {
			dWRITE_DIGITAL(nLiftPinLXO, 2, &bIOStatus);
			dWRITE_DIGITAL(nLiftPinRXO, 2, &bIOStatus);		
			if(! WAIT_SECONDS(2)) break;
		}
		else
		{
			if(! GetTimeTable(szCmd, &dbWaitTime)) break;
			if(! WAIT_SECONDS(dbWaitTime)) break;
		}
		bRet = TRUE;
	} while(0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL CTMCtrlSimulator::ControlBMDoor(char *szCmd)
{
	BOOL bRet = FALSE;
	BOOL bIOStatus;
	double dbWaitTime = 0;
	int nDoorXO;
	int nDoorXI;
	int nDoorXO2;
	int nDoorXI2;
	enum { DOOR_UNKNOWN, DOOR_OPEN, DOOR_CLOSE, DOOR_ERROR };
	int nDoorCmd = DOOR_UNKNOWN;

	do {
		CTextParser tp(szCmd);
		tp.GetWord();
		if(tp.BeEqual("BM1"))
		{
			nDoorXO  = BM1_DoorLXO;
			nDoorXI  = BM1_DoorLXI;
			nDoorXO2 = BM1_DoorRXO;
			nDoorXI2 = BM1_DoorRXI;
		}
		else if(tp.BeEqual("BM2"))
		{
			nDoorXO  = BM2_DoorLXO;
			nDoorXI  = BM2_DoorLXI;
			nDoorXO2 = BM2_DoorRXO;
			nDoorXI2 = BM2_DoorRXI;
		}
		else break;

		tp.GetWord();
		if(tp.BeEqual("DOOR"))
		{
			tp.GetWord();
			if(tp.BeEqual("OPEN")) nDoorCmd = DOOR_OPEN;
			else if(tp.BeEqual("CLOSE")) nDoorCmd = DOOR_CLOSE;
			else break;
			if(dREAD_DIGITAL(nDoorXI, &bIOStatus) == nDoorCmd) { bRet = TRUE; break; }
			dWRITE_DIGITAL(nDoorXO , nDoorCmd, &bIOStatus);
			dWRITE_DIGITAL(nDoorXO2, nDoorCmd, &bIOStatus);
			dWRITE_DIGITAL(nDoorXI , DOOR_UNKNOWN, &bIOStatus);
			dWRITE_DIGITAL(nDoorXI2, DOOR_UNKNOWN, &bIOStatus);
			if(! GetTimeTable(szCmd, &dbWaitTime)) break;
			if(! WAIT_SECONDS(dbWaitTime)) break;
			dWRITE_DIGITAL(nDoorXI , nDoorCmd, &bIOStatus);
			dWRITE_DIGITAL(nDoorXI2, nDoorCmd, &bIOStatus);
		}
		else break;
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CTMCtrlSimulator::ControlIsoGate(char *szCmd)
{
	BOOL bRet = FALSE;
	BOOL bIOStatus = FALSE;
	double dbWaitTime = 0;
	enum { GATE_UNKNOWN, GATE_OPEN, GATE_CLOSE, GATE_ERROR };
	int nSlotVvXO  = BM1_SlotValveLXO;
	int nSlotVvXI  = BM1_SlotValveLXI;
	int nSlotVv2XO = PM1_SlotValve1XO;
	int nSlotVv2XI = PM1_SlotValve1XI;
	int nCmd = GATE_UNKNOWN;
	int nOption = 0;


	do {
		CTextParser tp(szCmd);
		tp.GetWord();
		tp.GetWord();
		if(tp.BeEqual("OPEN")) nCmd = GATE_OPEN;
		else if(tp.BeEqual("CLOSE")) nCmd = GATE_CLOSE;
		tp.GetWord();
		if(tp.BeEqual("BM1")) { 
			nSlotVvXO  = BM1_SlotValveLXO;
			nSlotVvXI  = BM1_SlotValveLXI; 
			nSlotVv2XO = BM1_SlotValveRXO;
			nSlotVv2XI = BM1_SlotValveRXI; 
		}
		else if(tp.BeEqual("BM2")) {
			nSlotVvXO  = BM2_SlotValveLXO;
			nSlotVvXI  = BM2_SlotValveLXI; 
			nSlotVv2XO = BM2_SlotValveRXO;
			nSlotVv2XI = BM2_SlotValveRXI; 
		}	
		else if(tp.BeEqual("PM1")) { 
			nSlotVvXO  = PM1_SlotValve1XO;
			nSlotVvXI  = PM1_SlotValve1XI; 
			nSlotVv2XO = PM1_SlotValve2XO;
			nSlotVv2XI = PM1_SlotValve2XI;
		} else if(tp.BeEqual("PM2")) { 
			nSlotVvXO  = PM2_SlotValve1XO; 
			nSlotVvXI  = PM2_SlotValve1XI;
			nSlotVv2XO = PM2_SlotValve2XO; 
			nSlotVv2XI = PM2_SlotValve2XI;
		} else if(tp.BeEqual("PM3")) { 
			nSlotVvXO  = PM3_SlotValve1XO; 
			nSlotVvXI  = PM3_SlotValve1XI; 
			nSlotVv2XO = PM3_SlotValve2XO; 
			nSlotVv2XI = PM3_SlotValve2XI; 
		}
		else break;

		if(nCmd == GATE_OPEN)
		{
			if(dREAD_DIGITAL(nSlotVvXI, &bIOStatus) == GATE_OPEN) { bRet = TRUE; break; }
			dWRITE_DIGITAL(nSlotVvXO, GATE_OPEN, &bIOStatus);
			if(dREAD_DIGITAL(nSlotVv2XI, &bIOStatus) == GATE_OPEN) { bRet = TRUE; break; }
			dWRITE_DIGITAL(nSlotVv2XO, GATE_OPEN, &bIOStatus);
		}
		else if(nCmd == GATE_CLOSE)
		{
			if(dREAD_DIGITAL(nSlotVvXI, &bIOStatus) == GATE_CLOSE) { bRet = TRUE; break; }
			dWRITE_DIGITAL(nSlotVvXO, GATE_CLOSE, &bIOStatus);
			if(dREAD_DIGITAL(nSlotVv2XI, &bIOStatus) == GATE_CLOSE) { bRet = TRUE; break; }
			dWRITE_DIGITAL(nSlotVv2XO, GATE_CLOSE, &bIOStatus);
		}

		dWRITE_DIGITAL(nSlotVvXI, GATE_UNKNOWN, &bIOStatus);
		dWRITE_DIGITAL(nSlotVv2XI, GATE_UNKNOWN, &bIOStatus);
		if(! GetTimeTable(szCmd, &dbWaitTime)) break;
		if(! WAIT_SECONDS(dbWaitTime)) break;
		dWRITE_DIGITAL(nSlotVvXI, nCmd, &bIOStatus);
		dWRITE_DIGITAL(nSlotVv2XI, nCmd, &bIOStatus);
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL CTMCtrlSimulator::SetTimeTable(char *szCmd, double dbTime)
{
	BOOL bRet = FALSE;
	int nIdx;
	nIdx = FindTimeTable(szCmd);
	if(nIdx >= 0)
	{
		g_TimeTable[nIdx].dbTime = dbTime;
		bRet = TRUE;
	}
	return bRet;
}

BOOL CTMCtrlSimulator::GetTimeTable(char *szCmd, double *pdbTime)
{
	BOOL bRet = FALSE;
	int nIdx;
	nIdx = FindTimeTable(szCmd);
	if(nIdx >= 0)
	{
		*pdbTime = g_TimeTable[nIdx].dbTime;
		bRet = TRUE;
	}
	#ifdef _DEBUG
	if(bRet == FALSE) printf("-----> GetTimeTable Func Failed [Cmd:%s] in TM Control Simulation\n", szCmd);
	#endif // _DEBUG
	return bRet;
}

int CTMCtrlSimulator::FindTimeTable(char *szCmd)
{
	int i;
	CTextParser tp1;
	CTextParser tp2;
	for(i=0; strlen(g_TimeTable[i].szCmd) > 0; i++)
	{
		tp1.SetText(g_TimeTable[i].szCmd);
		tp2.SetText(szCmd);
		if(! tp1.GetWord()) continue;
		if(! tp2.GetWord()) continue;
		if(! tp1.BeEqual(tp2.m_szLastPickedWord, strlen(tp1.m_szLastPickedWord))) continue;

		if(! tp1.GetWord()) break;
		if(! tp2.GetWord()) continue;
		if(! tp1.BeEqual(tp2.m_szLastPickedWord, strlen(tp1.m_szLastPickedWord))) continue;

		if(! tp1.GetWord()) break;
		if(! tp2.GetWord()) continue;
		if(! tp1.BeEqual(tp2.m_szLastPickedWord, strlen(tp1.m_szLastPickedWord))) continue;

		if(! tp1.GetWord()) break;
		if(! tp2.GetWord()) continue;
		if(! tp1.BeEqual(tp2.m_szLastPickedWord, strlen(tp1.m_szLastPickedWord))) continue;
		break;
	}
	if(strlen(g_TimeTable[i].szCmd) <= 0) i = -1;
	//#ifdef _DEBUG
	if(i == -1) printf("-----> SetTimeTable Func Failed [Cmd:%s] in TM Control Simulation\n", szCmd);
	//#endif // _DEBUG
	return i;
}