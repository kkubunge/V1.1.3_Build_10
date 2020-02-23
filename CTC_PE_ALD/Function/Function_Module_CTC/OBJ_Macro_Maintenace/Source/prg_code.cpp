#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
#include "TextParser.h"
} // End of [extern "C"]
//==========================================================================================

#include "SeqInterface.h"
#include "AutoPM.h"

#define FUNC_NAME		"AutoPM"
CAutoPM g_CAutoPM;
char g_szModuleName[8] = "PM1";

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding", 		_K_D_IO,	IO_Driver_Loding,	 0 },
	{ "e:SCHEDULER",			_K_S_IO,	SCHEDULER,			 0 },
	{ "Scheduler",				_K_F_IO, 	SCHEDULER2,			 0 },
	{ "AutoPMEvent",			_K_S_IO,	AutoPMEventName,	 0 },
	{ "AutoPMMacroName_$0",		_K_S_IO,	AutoPMMacroName,	 0 },
	{ "CTC.$0_CommStatus",		_K_D_IO,	PM_CommStatus,		 0 },
	{ "CTC.MAIN_CONTROL",		_K_D_IO,	CTC_MAIN_CONTROL,	 0 },
	{ "CTC.MAIN_CONTROL2",		_K_D_IO,	CTC_MAIN_CONTROL2,	 0 },
	{ "CTC.MAIN_CONTROL3",		_K_D_IO,	CTC_MAIN_CONTROL3,	 0 },
	{ "CTC.MAIN_CONTROL4",		_K_D_IO,	CTC_MAIN_CONTROL4,	 0 },

	{ "SCHEDULER_MAINT_$0",		_K_F_IO,	SCHEDULER_MAINT_PM,	 0 },

	{ "CHAMBER_CON_CTL_$0",		_K_F_IO,	CHAMBER_CON_CTL_PM,	 0 },
	{ "CHAMBER_CON_ABORT_$0",	_K_F_IO,	CHAMBER_CON_ABORT_PM,0 },

	{ "PRMD_$0_PreMEn",			_K_D_IO, 	PRMD_PreMEn,		 0 },
                                                                     
	{ "PRMS_$0_PreMRcp_01",     _K_S_IO, 	PRMS_PreMRcp_01, 	 0 },
	{ "PRMS_$0_PreMRcp_02",     _K_S_IO, 	PRMS_PreMRcp_02, 	 0 },

	{ "PRMS_$0_PreMRcp2_01",     _K_S_IO, 	PRMS_PreMRcp2_01, 	 0 },
	{ "PRMS_$0_PreMRcp2_02",     _K_S_IO, 	PRMS_PreMRcp2_02, 	 0 },

	{ "PRMD_$0_PreMRcp_01",     _K_D_IO, 	PRMD_PreMRcp_01, 	 0 },
	{ "PRMD_$0_PreMRcp_02",     _K_D_IO, 	PRMD_PreMRcp_02, 	 0 },

	{ "PRMD_$0_PreMRcp2_01",     _K_D_IO, 	PRMD_PreMRcp2_01, 	 0 },
	{ "PRMD_$0_PreMRcp2_02",     _K_D_IO, 	PRMD_PreMRcp2_02, 	 0 },

	
	{ "PRMD_$0_PostMEn",		_K_D_IO, 	PRMD_PostMEn,		 0 },
                                                                     
	{ "PRMS_$0_PostMRcp_01",    _K_S_IO, 	PRMS_PostMRcp_01, 	 0 },
	{ "PRMS_$0_PostMRcp_02",    _K_S_IO, 	PRMS_PostMRcp_02, 	 0 },

	{ "PRMS_$0_PostMRcp2_01",    _K_S_IO, 	PRMS_PostMRcp2_01, 	 0 },
	{ "PRMS_$0_PostMRcp2_02",    _K_S_IO, 	PRMS_PostMRcp2_02, 	 0 },

	{ "PRMD_$0_PostMRcp_01",    _K_D_IO, 	PRMD_PostMRcp_01, 	 0 },
	{ "PRMD_$0_PostMRcp_02",    _K_D_IO, 	PRMD_PostMRcp_02, 	 0 },

	{ "PRMD_$0_PostMRcp2_01",    _K_D_IO, 	PRMD_PostMRcp2_01, 	 0 },
	{ "PRMD_$0_PostMRcp2_02",    _K_D_IO, 	PRMD_PostMRcp2_02, 	 0 },

	{ "PRMD_$0_ALC_Use_After_PreM",		_K_D_IO, 	PRMD_ALC_Use_After_PreM, 	 0 },		//2018.12.04 by kimhc
	{ "PRMD_$0_ALC_Use_PostM",			_K_D_IO, 	PRMD_ALC_Use_PostM, 	 0 },
	{ "PRMD_$0_MFCveriUse",				_K_D_IO, 	PRMD_MFCveriUse, 	 0 },	
	
	{ "$0.PLCInkUnlockStsDO",   _K_D_IO,    PLCInkUnlockStsDO,   0 },
	{ "$0.PLCIntRemDO",			_K_D_IO,    PLCInkRvrStsDO,      0 },
	{ "$0.PLCInkUnlockStsDI",   _K_D_IO,    PLCInkUnlockStsDI,   0 },
	{ "$0.PLCInkRvrStsDI",      _K_D_IO,    PLCInkRvrStsDI,      0 },
	
	{ "$0.ChmPrsAI",			_K_A_IO,	ChmPrsAI,			 0 }, // 1000Torr Gague
	{ "$0.M651_Pressure",		_K_A_IO,	M651_Pressure,		 0 }, // 10 Torr Gague
	{ "$0.RoughValveDI",		_K_D_IO,	RoughValveDI,		 0 },
//	{ "$0.RoughValve2DI",		_K_D_IO,	RoughValve2DI,		 0 },
	{ "$0.PRMA_VS_PrsBaRough",	_K_A_IO,	PRMA_VS_PrsBaRough,	 0 },
	{ "ABORT_PROCESS_$0",		_K_F_IO,	ABORT_PROCESS,		 0 },
	{ "PRMD_AutoPM_$0Flw",		_K_D_IO, 	PRMD_AutoPMFlw, 	 0 },

	{ "$0.ChmPrssDI1",			_K_D_IO,	ChmPrssDI,			0 },
	{ "$0.PRMA_VS_PrsBaVent",	_K_A_IO,	PRMA_VS_PrsBaVent,	0 },

	{ "PRMD_PreMacroFlw_$0",	_K_D_IO,	PRMD_PreMacroFlw,	0 },
	{ "PRMD_PostMacroFlw_$0",	_K_D_IO,	PRMD_PostMacroFlw,	0 },

	{ "$0_PRO_ClnStsDM",		_K_D_IO,	PRO_DryClnStsDM,	0 },

	{ "$0_Pre_PM_Resumed",		_K_D_IO,	Pre_PM_Resumed,		0 },
	{ "$0_Post_PM_Resumed",		_K_D_IO,	Post_PM_Resumed,	0 },
	{ "PREVENTIVE_$0",			_K_F_IO,	PREVENTIVE,			0 },
	{ "$0.DryCleaningReserveDI", _K_D_IO,	ReserveDI,			0 },
	{ "$0.PMJobStart",			_K_D_IO,	PMJobStart,			0 },

	{ "PRMS_$0_MacroName_01",	_K_S_IO,	MacroName_01,		0 },
	{ "PRMS_$0_MacroName_02",	_K_S_IO,	MacroName_02,		0 },
	{ "PRMS_$0_MacroName_03",	_K_S_IO,	MacroName_03,		0 },
	{ "PRMS_$0_MacroName_04",	_K_S_IO,	MacroName_04,		0 },
	{ "PRMS_$0_MacroName_05",	_K_S_IO,	MacroName_05,		0 },
	{ "PRMS_$0_MacroName_06",	_K_S_IO,	MacroName_06,		0 },

	{ "AutoPMRcpName_$0",		_K_S_IO,	AutoPMRcpName,		0 },
	{ "AutoPMOwner_$0",			_K_D_IO,	AutoPMOwner,		0 },
	{ "$0.WaferDI",				_K_D_IO,	WaferDI,			0 },
	{ "$0.TopLidSwitchDI",		_K_D_IO,	TopLidSwitchDI,		0 },
	{ "PRMD_AUTO$0_POST_LEAK",	_K_D_IO,	PRMD_AUTOPM_POST_LEAK,		0 },
	{ "MANUAL_$0_SSP",			_K_D_IO,	MANUAL_SSP,	0 },
	""
};

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	if ( g_CAutoPM.m_bSimulMode ) return g_CAutoPM.Sim_Main();
	else return g_CAutoPM.Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	g_CAutoPM.m_bSimulMode = TRUE;
	else														g_CAutoPM.m_bSimulMode = FALSE;
	g_CAutoPM.Initialize(g_szModuleName);
	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char szTemp[80];
	CTextParser txtP;
	txtP.SetText(PROGRAM_ARGUMENT_READ());

	txtP.GetWord(szTemp);
	if(strlen(szTemp) > 1) strcpy(g_szModuleName, szTemp);
	REPLACE_CHAR_SET(0, g_szModuleName);
}


//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		g_CAutoPM.Deinitialize();
	}
}

void Event_Message_Received() {
	char szCmd[80];
	char szModule[80];
	char szMacroType[80];
	CTextParser txtP;
	txtP.SetText(PROGRAM_EVENT_READ());

	txtP.GetWord(szCmd);
	txtP.GetWord(szModule);
	txtP.GetWord(szMacroType);

	if(strcmp(szCmd,"PMJobCreate") == 0){
		g_CAutoPM.AutoPMEventMon(szCmd,szMacroType);
	}
	if(strcmp(szCmd,"PMJobStop") == 0){
		g_CAutoPM.AutoPMEventMon(szCmd,szMacroType);
	}
	if(strcmp(szCmd,"PMJobAbort") == 0){
		g_CAutoPM.AutoPMEventMon(szCmd,szMacroType);
	}
}

} // End of [extern "C"]
//==========================================================================================
