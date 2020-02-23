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

} // End of [extern "C"]
//==========================================================================================

#include "SeqInterface.h"
#include "LotFlowSimul.h"

#define FUNC_NAME		"LotFlowSimul"
CLotFlowSimul g_SeqInterface;
// CSeqInterface g_SeqInterface("SeqInterf");

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding,	0	} ,
	{ "Scheduler",				_K_F_IO, 	SCHEDULER,				0	} ,
	{ "e:SCHEDULER"				, _K_S_IO	,	S_SCHEDULER		,	0	} ,
	{ "e:GEM.CONTROL"			, _K_S_IO	,	GEM_CONTROL		,	0	} ,

	{ "CTC.MAIN_CONTROL"		, _K_D_IO	,	CM1_MAIN_CONTROL	,	0	} ,
	{ "CTC.MAIN_CONTROL2"		, _K_D_IO	,	CM2_MAIN_CONTROL	,	0	} ,
	{ "CTC.MAIN_CONTROL3"		, _K_D_IO	,	CM3_MAIN_CONTROL	,	0	} ,
	{ "CTC.MAIN_CONTROL4"		, _K_D_IO	,	CM4_MAIN_CONTROL	,	0	} ,

	{ "FC_CM1_MID_CONTROL"		, _K_D_IO	,	FC_CM1_MID_CONTROL	,	0	} ,
	{ "FC_CM2_MID_CONTROL"		, _K_D_IO	,	FC_CM2_MID_CONTROL	,	0	} ,
	{ "FC_CM3_MID_CONTROL"		, _K_D_IO	,	FC_CM3_MID_CONTROL	,	0	} ,

	{ "CTC.JID_READ"			, _K_S_IO	,	CTC_JID_READ	,	0	} ,
	{ "CTC.JID_READ2"			, _K_S_IO	,	CTC_JID_READ2	,	0	} ,
	{ "CTC.JID_READ3"			, _K_S_IO	,	CTC_JID_READ3	,	0	} ,
	{ "CTC.JID_READ4"			, _K_S_IO	,	CTC_JID_READ4	,	0	} ,

	{ "CTC.RECIPE_FILE"			, _K_S_IO	,	CTC_RECIPE_FILE	,	0	} ,
	{ "CTC.RECIPE_FILE2"		, _K_S_IO	,	CTC_RECIPE_FILE2	,	0	} ,
	{ "CTC.RECIPE_FILE3"		, _K_S_IO	,	CTC_RECIPE_FILE3	,	0	} ,

	{ "CTC.MID_NAME"			, _K_S_IO	,	CTC_MID_NAME	,	0	} ,
	{ "CTC.MID_NAME2"			, _K_S_IO	,	CTC_MID_NAME2	,	0	} ,
	{ "CTC.MID_NAME3"			, _K_S_IO	,	CTC_MID_NAME3	,	0	} ,
	
	{ "LPMA.F_ProperlyPlaceDI"		, _K_D_IO	,	CM1_CASS_STATUS					,	0	} ,
	{ "LPMB.F_ProperlyPlaceDI"		, _K_D_IO	,	CM2_CASS_STATUS					,	0	} ,
	{ "LPMC.F_ProperlyPlaceDI"		, _K_D_IO	,	CM3_CASS_STATUS					,	0	} ,

	{ "LPMA.FOUP_PresentDI"			, _K_D_IO	,	CM1_CASS_SenSor_STATUS			,	0	} ,
	{ "LPMB.FOUP_PresentDI"			, _K_D_IO	,	CM2_CASS_SenSor_STATUS			,	0	} ,
	{ "LPMC.FOUP_PresentDI"			, _K_D_IO	,	CM3_CASS_SenSor_STATUS			,	0	} ,

	{ "LPMA.ClampUnclampXO"			, _K_D_IO	,	CM1_CLAMP						,	0	} ,
	{ "LPMB.ClampUnclampXO"			, _K_D_IO	,	CM2_CLAMP						,	0	} ,
	{ "LPMC.ClampUnclampXO"			, _K_D_IO	,	CM3_CLAMP						,	0	} ,

	{ "LPMA.ClampUnclampXI"			, _K_D_IO	,	CM1_CLAMP_STATUS				,	0	} ,
	{ "LPMB.ClampUnclampXI"			, _K_D_IO	,	CM2_CLAMP_STATUS				,	0	} ,
	{ "LPMC.ClampUnclampXI"			, _K_D_IO	,	CM3_CLAMP_STATUS				,	0	} ,

	{ "LPMA.ShuttleInOutXI"			, _K_D_IO	,	CM1_DOCK_STATUS					,	0	} ,
	{ "LPMB.ShuttleInOutXI"			, _K_D_IO	,	CM2_DOCK_STATUS					,	0	} ,
	{ "LPMC.ShuttleInOutXI"			, _K_D_IO	,	CM3_DOCK_STATUS					,	0	} ,

	{ "LPMA.DoorUpDownXI"			, _K_D_IO	,	CM1_OPEN_STATUS					,	0	} ,
	{ "LPMB.DoorUpDownXI"			, _K_D_IO	,	CM2_OPEN_STATUS					,	0	} ,
	{ "LPMC.DoorUpDownXI"			, _K_D_IO	,	CM3_OPEN_STATUS					,	0	} ,

	{ "CM1.C01_Wafer"				, _K_D_IO	,	CM1_C01_Wafer					,	0	} ,
	{ "CM1.C02_Wafer"				, _K_D_IO	,	CM1_C02_Wafer					,	0	} ,
	{ "CM1.C03_Wafer"				, _K_D_IO	,	CM1_C03_Wafer					,	0	} ,
	{ "CM1.C04_Wafer"				, _K_D_IO	,	CM1_C04_Wafer					,	0	} ,
	{ "CM1.C05_Wafer"				, _K_D_IO	,	CM1_C05_Wafer					,	0	} ,
	{ "CM1.C06_Wafer"				, _K_D_IO	,	CM1_C06_Wafer					,	0	} ,
	{ "CM1.C07_Wafer"				, _K_D_IO	,	CM1_C07_Wafer					,	0	} ,
	{ "CM1.C08_Wafer"				, _K_D_IO	,	CM1_C08_Wafer					,	0	} ,
	{ "CM1.C09_Wafer"				, _K_D_IO	,	CM1_C09_Wafer					,	0	} ,
	{ "CM1.C10_Wafer"				, _K_D_IO	,	CM1_C10_Wafer					,	0	} ,
	{ "CM1.C11_Wafer"				, _K_D_IO	,	CM1_C11_Wafer					,	0	} ,
	{ "CM1.C12_Wafer"				, _K_D_IO	,	CM1_C12_Wafer					,	0	} ,
	{ "CM1.C13_Wafer"				, _K_D_IO	,	CM1_C13_Wafer					,	0	} ,
	{ "CM1.C14_Wafer"				, _K_D_IO	,	CM1_C14_Wafer					,	0	} ,
	{ "CM1.C15_Wafer"				, _K_D_IO	,	CM1_C15_Wafer					,	0	} ,
	{ "CM1.C16_Wafer"				, _K_D_IO	,	CM1_C16_Wafer					,	0	} ,
	{ "CM1.C17_Wafer"				, _K_D_IO	,	CM1_C17_Wafer					,	0	} ,
	{ "CM1.C18_Wafer"				, _K_D_IO	,	CM1_C18_Wafer					,	0	} ,
	{ "CM1.C19_Wafer"				, _K_D_IO	,	CM1_C19_Wafer					,	0	} ,
	{ "CM1.C20_Wafer"				, _K_D_IO	,	CM1_C20_Wafer					,	0	} ,
	{ "CM1.C21_Wafer"				, _K_D_IO	,	CM1_C21_Wafer					,	0	} ,
	{ "CM1.C22_Wafer"				, _K_D_IO	,	CM1_C22_Wafer					,	0	} ,
	{ "CM1.C23_Wafer"				, _K_D_IO	,	CM1_C23_Wafer					,	0	} ,
	{ "CM1.C24_Wafer"				, _K_D_IO	,	CM1_C24_Wafer					,	0	} ,
	{ "CM1.C25_Wafer"				, _K_D_IO	,	CM1_C25_Wafer					,	0	} ,

	{ "CM2.C01_Wafer"				, _K_D_IO	,	CM2_C01_Wafer					,	0	} ,
	{ "CM2.C02_Wafer"				, _K_D_IO	,	CM2_C02_Wafer					,	0	} ,
	{ "CM2.C03_Wafer"				, _K_D_IO	,	CM2_C03_Wafer					,	0	} ,
	{ "CM2.C04_Wafer"				, _K_D_IO	,	CM2_C04_Wafer					,	0	} ,
	{ "CM2.C05_Wafer"				, _K_D_IO	,	CM2_C05_Wafer					,	0	} ,
	{ "CM2.C06_Wafer"				, _K_D_IO	,	CM2_C06_Wafer					,	0	} ,
	{ "CM2.C07_Wafer"				, _K_D_IO	,	CM2_C07_Wafer					,	0	} ,
	{ "CM2.C08_Wafer"				, _K_D_IO	,	CM2_C08_Wafer					,	0	} ,
	{ "CM2.C09_Wafer"				, _K_D_IO	,	CM2_C09_Wafer					,	0	} ,
	{ "CM2.C10_Wafer"				, _K_D_IO	,	CM2_C10_Wafer					,	0	} ,
	{ "CM2.C11_Wafer"				, _K_D_IO	,	CM2_C11_Wafer					,	0	} ,
	{ "CM2.C12_Wafer"				, _K_D_IO	,	CM2_C12_Wafer					,	0	} ,
	{ "CM2.C13_Wafer"				, _K_D_IO	,	CM2_C13_Wafer					,	0	} ,
	{ "CM2.C14_Wafer"				, _K_D_IO	,	CM2_C14_Wafer					,	0	} ,
	{ "CM2.C15_Wafer"				, _K_D_IO	,	CM2_C15_Wafer					,	0	} ,
	{ "CM2.C16_Wafer"				, _K_D_IO	,	CM2_C16_Wafer					,	0	} ,
	{ "CM2.C17_Wafer"				, _K_D_IO	,	CM2_C17_Wafer					,	0	} ,
	{ "CM2.C18_Wafer"				, _K_D_IO	,	CM2_C18_Wafer					,	0	} ,
	{ "CM2.C19_Wafer"				, _K_D_IO	,	CM2_C19_Wafer					,	0	} ,
	{ "CM2.C20_Wafer"				, _K_D_IO	,	CM2_C20_Wafer					,	0	} ,
	{ "CM2.C21_Wafer"				, _K_D_IO	,	CM2_C21_Wafer					,	0	} ,
	{ "CM2.C22_Wafer"				, _K_D_IO	,	CM2_C22_Wafer					,	0	} ,
	{ "CM2.C23_Wafer"				, _K_D_IO	,	CM2_C23_Wafer					,	0	} ,
	{ "CM2.C24_Wafer"				, _K_D_IO	,	CM2_C24_Wafer					,	0	} ,
	{ "CM2.C25_Wafer"				, _K_D_IO	,	CM2_C25_Wafer					,	0	} ,

	{ "CM3.C01_Wafer"				, _K_D_IO	,	CM3_C01_Wafer					,	0	} ,
	{ "CM3.C02_Wafer"				, _K_D_IO	,	CM3_C02_Wafer					,	0	} ,
	{ "CM3.C03_Wafer"				, _K_D_IO	,	CM3_C03_Wafer					,	0	} ,
	{ "CM3.C04_Wafer"				, _K_D_IO	,	CM3_C04_Wafer					,	0	} ,
	{ "CM3.C05_Wafer"				, _K_D_IO	,	CM3_C05_Wafer					,	0	} ,
	{ "CM3.C06_Wafer"				, _K_D_IO	,	CM3_C06_Wafer					,	0	} ,
	{ "CM3.C07_Wafer"				, _K_D_IO	,	CM3_C07_Wafer					,	0	} ,
	{ "CM3.C08_Wafer"				, _K_D_IO	,	CM3_C08_Wafer					,	0	} ,
	{ "CM3.C09_Wafer"				, _K_D_IO	,	CM3_C09_Wafer					,	0	} ,
	{ "CM3.C10_Wafer"				, _K_D_IO	,	CM3_C10_Wafer					,	0	} ,
	{ "CM3.C11_Wafer"				, _K_D_IO	,	CM3_C11_Wafer					,	0	} ,
	{ "CM3.C12_Wafer"				, _K_D_IO	,	CM3_C12_Wafer					,	0	} ,
	{ "CM3.C13_Wafer"				, _K_D_IO	,	CM3_C13_Wafer					,	0	} ,
	{ "CM3.C14_Wafer"				, _K_D_IO	,	CM3_C14_Wafer					,	0	} ,
	{ "CM3.C15_Wafer"				, _K_D_IO	,	CM3_C15_Wafer					,	0	} ,
	{ "CM3.C16_Wafer"				, _K_D_IO	,	CM3_C16_Wafer					,	0	} ,
	{ "CM3.C17_Wafer"				, _K_D_IO	,	CM3_C17_Wafer					,	0	} ,
	{ "CM3.C18_Wafer"				, _K_D_IO	,	CM3_C18_Wafer					,	0	} ,
	{ "CM3.C19_Wafer"				, _K_D_IO	,	CM3_C19_Wafer					,	0	} ,
	{ "CM3.C20_Wafer"				, _K_D_IO	,	CM3_C20_Wafer					,	0	} ,
	{ "CM3.C21_Wafer"				, _K_D_IO	,	CM3_C21_Wafer					,	0	} ,
	{ "CM3.C22_Wafer"				, _K_D_IO	,	CM3_C22_Wafer					,	0	} ,
	{ "CM3.C23_Wafer"				, _K_D_IO	,	CM3_C23_Wafer					,	0	} ,
	{ "CM3.C24_Wafer"				, _K_D_IO	,	CM3_C24_Wafer					,	0	} ,
	{ "CM3.C25_Wafer"				, _K_D_IO	,	CM3_C25_Wafer					,	0	} ,

	{ "CTC.BM1_Wafer_Status",	_K_D_IO,	CTC_BM1_Wafer_Status,	0},
	{ "CTC.BM1_Wafer_Status2",	_K_D_IO,	CTC_BM1_Wafer_Status2,	0},
	{ "CTC.BM1_Wafer_Status3",	_K_D_IO,	CTC_BM1_Wafer_Status3,	0},
	{ "CTC.BM1_Wafer_Status4",	_K_D_IO,	CTC_BM1_Wafer_Status4,	0},

	{ "CTC.BM2_Wafer_Status",	_K_D_IO,	CTC_BM2_Wafer_Status,	0},
	{ "CTC.BM2_Wafer_Status2",	_K_D_IO,	CTC_BM2_Wafer_Status2,	0},
	{ "CTC.BM2_Wafer_Status3",	_K_D_IO,	CTC_BM2_Wafer_Status3,	0},
	{ "CTC.BM2_Wafer_Status4",	_K_D_IO,	CTC_BM2_Wafer_Status4,	0},

	{ "CTC.TA_Wafer_Status",	_K_D_IO,	CTC_TA_Wafer_Status,	0},
	{ "CTC.TA_Wafer_Status2",	_K_D_IO,	CTC_TA2_Wafer_Status,	0},
	{ "CTC.TB_Wafer_Status",	_K_D_IO,	CTC_TB_Wafer_Status,	0},
	{ "CTC.TB_Wafer_Status2",	_K_D_IO,	CTC_TB2_Wafer_Status,	0},
		
	{ "CTC.PM1_Wafer_Status",	_K_D_IO,	CTC_PM1_Wafer_Status,	0},
	{ "CTC.PM1_Wafer_Status2",	_K_D_IO,	CTC_PM1_Wafer_Status2,	0},	

	{ "CTC.PM2_Wafer_Status",	_K_D_IO,	CTC_PM2_Wafer_Status,	0},
	{ "CTC.PM2_Wafer_Status2",	_K_D_IO,	CTC_PM2_Wafer_Status2,	0},

	{ "CTC.PM3_Wafer_Status",	_K_D_IO,	CTC_PM3_Wafer_Status,	0},
	{ "CTC.PM3_Wafer_Status2",	_K_D_IO,	CTC_PM3_Wafer_Status2,	0},

	{ "CTC.FA_Wafer_Status",	_K_D_IO,	CTC_FA_Wafer_Status,	0},
	{ "CTC.FB_Wafer_Status",	_K_D_IO,	CTC_FB_Wafer_Status,	0},
	{ "CTC.FM_AL_Wafer_Status",	_K_D_IO,	CTC_FM_AL_Wafer_Status,	0},

	{ "GEM.OPERATORSTARTINT_CONTROL", _K_F_IO,		GEM_START_CTRL,		0	} ,
	{ "GEM.OPERATORSTARTINT_CONTROL2", _K_F_IO, 	GEM_START_CTRL2,	0	} ,
	{ "GEM.OPERATORSTARTINT_CONTROL3", _K_F_IO, 	GEM_START_CTRL3,	0	} ,
	{ "CTC.START_SLOT"				 , _K_D_IO,	    CM1_START_SLOT,		0   } ,
	{ "CTC.END_SLOT"				 , _K_D_IO,	    CM1_END_SLOTT,		0   } ,
	{ "CTC.START_SLOT2"			     , _K_D_IO,	    CM2_START_SLOT,		0   } ,
	{ "CTC.END_SLOT2"				 , _K_D_IO,	    CM2_END_SLOTT,		0   } ,
	{ "CTC.START_SLOT3"				 , _K_D_IO,	    CM3_START_SLOT,		0   } ,
	{ "CTC.END_SLOT3"				 , _K_D_IO,	    CM3_END_SLOTT,		0   } ,
	""
};

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	if ( g_SeqInterface.m_bSimulMode ) return g_SeqInterface.Sim_Main();
	else return g_SeqInterface.Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	g_SeqInterface.Enter_Code();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	g_SeqInterface.m_bSimulMode = TRUE;
	else														g_SeqInterface.m_bSimulMode = FALSE;
	return g_SeqInterface.CSeqInterface::Initialize(FUNC_NAME);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		g_SeqInterface.Deinitialize();
	}
}

} // End of [extern "C"]
//==========================================================================================
