#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"

//------------------------------------------------------------------------------------------
#define	MAX_CASSETTE				3
#define MAX_SLOT					25

// 2006.08.11 by ycchoi
// #define MAX_PAGE					18 // 17 page + mid page = 18
#define MAX_PAGE					3 // 2 page + mid page = 3

#define MAX_ALL_PM_WAFER			6
//------------------------------------------------------------------------------------------
#define MAX_WAFERZONE				2
typedef struct{
	int nWaferSource;
	int nWaferStatus;
	char szWaferMID;
}SPMInfo;
SPMInfo g_PM1WaferZone[2], g_PM2WaferZone[2], g_PM3WaferZone[2];

extern void _LOG(const char *pFormat, ...);
#define	 ALARM_FOUP_ABSENT_RUNNING_CM1	388

int     m_nAlarmPointer = 300;

BOOL	SIMULATION_MODE		= FALSE;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	//
	{ "AlarmStatus"					, _K_D_IO	,	CTC_Alarm_Status		,	0	} ,
	{ "AlarmHappenSts"				, _K_D_IO	,	CTC_Alarm_Happen		,	0	} ,
	{ "PRMD_SIGNALTOWER_OPT"		, _K_D_IO	,	PRMD_SIGNALTOWER_OPT	,	0	} ,

	{ "PM1.SW_GbIntlckDM"			, _K_D_IO	,	PM1_SW_GbIntlckDM		,	0	} ,
	{ "PM2.SW_GbIntlckDM"			, _K_D_IO	,	PM2_SW_GbIntlckDM		,	0	} ,
	{ "PM3.SW_GbIntlckDM"			, _K_D_IO	,	PM3_SW_GbIntlckDM		,	0	} ,

	{ "CTC.PM1_CommStatus"			, _K_D_IO	,	PM1_CommStatus			,	0	} ,
	{ "CTC.PM2_CommStatus"			, _K_D_IO	,	PM2_CommStatus			,	0	} ,
	{ "CTC.PM3_CommStatus"			, _K_D_IO	,	PM3_CommStatus			,	0	} ,

	{ "GEM.Port1_LotID"				, _K_S_IO	,	GEM_Port1_LotID			,	0	} ,
	{ "GEM.Port2_LotID"				, _K_S_IO	,	GEM_Port2_LotID			,	0	} ,
	{ "GEM.Port3_LotID"				, _K_S_IO	,	GEM_Port3_LotID			,	0	} ,

	{ "TM.SigTwrRDO"			    , _K_D_IO	,	RED_SIGNAL_ONOFF		,	0	} ,
    { "TM.SigTwrGDO"		        , _K_D_IO	,	GREEN_SIGNAL_ONOFF		,	0	} ,
    { "TM.SigTwrYDO"		        , _K_D_IO	,	YELLOW_SIGNAL_ONOFF		,	0	} ,
    { "TM.SigTwrBDO"		        , _K_D_IO	,	BLUE_SIGNAL_ONOFF		,	0	} ,
    { "TM.SigTwrBuzzerDO"			, _K_D_IO	,	ALARM_BUZZER_ONOFF		,	0	} ,

	{ "TM.SigTwrRFlashDO"			, _K_D_IO	,	RED_SIGNAL_Flash_ONOFF		,	0	} ,
    { "TM.SigTwrGFlashDO"		    , _K_D_IO	,	GREEN_SIGNAL_Flash_ONOFF		,	0	} ,
    { "TM.SigTwrYFlashDO"		    , _K_D_IO	,	YELLOW_SIGNAL_Flash_ONOFF		,	0	} ,
    { "TM.SigTwrBFlashDO"		    , _K_D_IO	,	BLUE_SIGNAL_Flash_ONOFF		,	0	} ,

	{ "GEM.Control_State"			, _K_D_IO	,	CONTROL_STATE			,	0	} ,

	{ "BM1.ATMVACSensorXI"			, _K_D_IO	,	BM1_ATMVACSensorXI		,	0	} ,
	{ "BM1.VacuumStatusDM"			, _K_D_IO	,	BM1_Vacuum_Status		,	0	} ,
	{ "BM2.ATMVACSensorXI"			, _K_D_IO	,	BM2_ATMVACSensorXI		,	0	} ,
	{ "BM2.VacuumStatusDM"			, _K_D_IO	,	BM2_Vacuum_Status		,	0	} ,
	{ "TM.ATMVACSensorXI"			, _K_D_IO	,	TM_ATMVACSensorXI		,	0	} ,
	{ "TM.VacuumStatusDM"			, _K_D_IO	,	TM_Vacuum_Status		,	0	} ,

	{ "BM1.CGChmPrsAI"				, _K_A_IO	,	Pressure_CG_BM1			,	0	} ,
	{ "PRMA_BM1_FPUMP_PRES"			, _K_A_IO	,	F_PUMP_PRESSURE_BM1		,	0	} ,
	{ "BM2.CGChmPrsAI"				, _K_A_IO	,	Pressure_CG_BM2			,	0	} ,
	{ "PRMA_BM2_FPUMP_PRES"			, _K_A_IO	,	F_PUMP_PRESSURE_BM2		,	0	} ,
	{ "TM.CGChmPrsAI"				, _K_A_IO	,	Pressure_CG_TM			,	0	} ,
	{ "PRMA_TM_FPUMP_PRES"			, _K_A_IO	,	F_PUMP_PRESSURE_TM		,	0	} ,

	{ "FC_CONFIG_LOADING"			, _K_D_IO	,	CONFIG_LOADING			,	0	} ,

	{ "CTC.MAIN_CONTROL"			, _K_D_IO	,	LOT_CONTROL			    ,	0	} ,
	{ "CTC.MAIN_CONTROL2"			, _K_D_IO	,	LOT2_CONTROL			,	0	} ,
	{ "CTC.MAIN_CONTROL3"			, _K_D_IO	,	LOT3_CONTROL		    ,	0	} ,
//2014.10.14
	{ "PM1_LOTID"					, _K_S_IO	,	PM1_LOTID			    ,	0	} ,
	{ "PM1_LOTID2"					, _K_S_IO	,	PM1_LOTID2				,	0	} ,
	{ "PM2_LOTID"					, _K_S_IO	,	PM2_LOTID			    ,	0	} ,
	{ "PM2_LOTID2"					, _K_S_IO	,	PM2_LOTID2				,	0	} ,
	{ "PM3_LOTID"					, _K_S_IO	,	PM3_LOTID			    ,	0	} ,
	{ "PM3_LOTID2"					, _K_S_IO	,	PM3_LOTID2				,	0	} ,
//
	{ "CTC.TR_Clear"				, _K_D_IO	,	CTC_TR_Clear		    ,	0	} ,

	{ "FM.SLOT_NUM"					, _K_A_IO	,	SLOT_NUM			    ,	0	} ,
	{ "FM.SLOT_NUM2"				, _K_A_IO	,	SLOT_NUM2			    ,	0	} ,
	{ "FM.MAINT_STATION"			, _K_D_IO	,	MAINT_STATION		    ,	0	} ,

	{ "CM1.C01_Wafer"				, _K_D_IO	,	CM1_C01_Wafer			,	0	} ,
	{ "CM1.C02_Wafer"				, _K_D_IO	,	CM1_C02_Wafer			,	0	} ,
	{ "CM1.C03_Wafer"				, _K_D_IO	,	CM1_C03_Wafer			,	0	} ,
	{ "CM1.C04_Wafer"				, _K_D_IO	,	CM1_C04_Wafer			,	0	} ,
	{ "CM1.C05_Wafer"				, _K_D_IO	,	CM1_C05_Wafer			,	0	} ,
	{ "CM1.C06_Wafer"				, _K_D_IO	,	CM1_C06_Wafer			,	0	} ,
	{ "CM1.C07_Wafer"				, _K_D_IO	,	CM1_C07_Wafer			,	0	} ,
	{ "CM1.C08_Wafer"				, _K_D_IO	,	CM1_C08_Wafer			,	0	} ,
	{ "CM1.C09_Wafer"				, _K_D_IO	,	CM1_C09_Wafer			,	0	} ,
	{ "CM1.C10_Wafer"				, _K_D_IO	,	CM1_C10_Wafer			,	0	} ,
	{ "CM1.C11_Wafer"				, _K_D_IO	,	CM1_C11_Wafer			,	0	} ,
	{ "CM1.C12_Wafer"				, _K_D_IO	,	CM1_C12_Wafer			,	0	} ,
	{ "CM1.C13_Wafer"				, _K_D_IO	,	CM1_C13_Wafer			,	0	} ,
	{ "CM1.C14_Wafer"				, _K_D_IO	,	CM1_C14_Wafer			,	0	} ,
	{ "CM1.C15_Wafer"				, _K_D_IO	,	CM1_C15_Wafer			,	0	} ,
	{ "CM1.C16_Wafer"				, _K_D_IO	,	CM1_C16_Wafer			,	0	} ,
	{ "CM1.C17_Wafer"				, _K_D_IO	,	CM1_C17_Wafer			,	0	} ,
	{ "CM1.C18_Wafer"				, _K_D_IO	,	CM1_C18_Wafer			,	0	} ,
	{ "CM1.C19_Wafer"				, _K_D_IO	,	CM1_C19_Wafer			,	0	} ,
	{ "CM1.C20_Wafer"				, _K_D_IO	,	CM1_C20_Wafer			,	0	} ,
	{ "CM1.C21_Wafer"				, _K_D_IO	,	CM1_C21_Wafer			,	0	} ,
	{ "CM1.C22_Wafer"				, _K_D_IO	,	CM1_C22_Wafer			,	0	} ,
	{ "CM1.C23_Wafer"				, _K_D_IO	,	CM1_C23_Wafer			,	0	} ,
	{ "CM1.C24_Wafer"				, _K_D_IO	,	CM1_C24_Wafer			,	0	} ,
	{ "CM1.C25_Wafer"				, _K_D_IO	,	CM1_C25_Wafer			,	0	} ,

	{ "CM2.C01_Wafer"				, _K_D_IO	,	CM2_C01_Wafer			,	0	} ,
	{ "CM2.C02_Wafer"				, _K_D_IO	,	CM2_C02_Wafer			,	0	} ,
	{ "CM2.C03_Wafer"				, _K_D_IO	,	CM2_C03_Wafer			,	0	} ,
	{ "CM2.C04_Wafer"				, _K_D_IO	,	CM2_C04_Wafer			,	0	} ,
	{ "CM2.C05_Wafer"				, _K_D_IO	,	CM2_C05_Wafer			,	0	} ,
	{ "CM2.C06_Wafer"				, _K_D_IO	,	CM2_C06_Wafer			,	0	} ,
	{ "CM2.C07_Wafer"				, _K_D_IO	,	CM2_C07_Wafer			,	0	} ,
	{ "CM2.C08_Wafer"				, _K_D_IO	,	CM2_C08_Wafer			,	0	} ,
	{ "CM2.C09_Wafer"				, _K_D_IO	,	CM2_C09_Wafer			,	0	} ,
	{ "CM2.C10_Wafer"				, _K_D_IO	,	CM2_C10_Wafer			,	0	} ,
	{ "CM2.C11_Wafer"				, _K_D_IO	,	CM2_C11_Wafer			,	0	} ,
	{ "CM2.C12_Wafer"				, _K_D_IO	,	CM2_C12_Wafer			,	0	} ,
	{ "CM2.C13_Wafer"				, _K_D_IO	,	CM2_C13_Wafer			,	0	} ,
	{ "CM2.C14_Wafer"				, _K_D_IO	,	CM2_C14_Wafer			,	0	} ,
	{ "CM2.C15_Wafer"				, _K_D_IO	,	CM2_C15_Wafer			,	0	} ,
	{ "CM2.C16_Wafer"				, _K_D_IO	,	CM2_C16_Wafer			,	0	} ,
	{ "CM2.C17_Wafer"				, _K_D_IO	,	CM2_C17_Wafer			,	0	} ,
	{ "CM2.C18_Wafer"				, _K_D_IO	,	CM2_C18_Wafer			,	0	} ,
	{ "CM2.C19_Wafer"				, _K_D_IO	,	CM2_C19_Wafer			,	0	} ,
	{ "CM2.C20_Wafer"				, _K_D_IO	,	CM2_C20_Wafer			,	0	} ,
	{ "CM2.C21_Wafer"				, _K_D_IO	,	CM2_C21_Wafer			,	0	} ,
	{ "CM2.C22_Wafer"				, _K_D_IO	,	CM2_C22_Wafer			,	0	} ,
	{ "CM2.C23_Wafer"				, _K_D_IO	,	CM2_C23_Wafer			,	0	} ,
	{ "CM2.C24_Wafer"				, _K_D_IO	,	CM2_C24_Wafer			,	0	} ,
	{ "CM2.C25_Wafer"				, _K_D_IO	,	CM2_C25_Wafer			,	0	} ,

	{ "CM3.C01_Wafer"				, _K_D_IO	,	CM3_C01_Wafer			,	0	} ,
	{ "CM3.C02_Wafer"				, _K_D_IO	,	CM3_C02_Wafer			,	0	} ,
	{ "CM3.C03_Wafer"				, _K_D_IO	,	CM3_C03_Wafer			,	0	} ,
	{ "CM3.C04_Wafer"				, _K_D_IO	,	CM3_C04_Wafer			,	0	} ,
	{ "CM3.C05_Wafer"				, _K_D_IO	,	CM3_C05_Wafer			,	0	} ,
	{ "CM3.C06_Wafer"				, _K_D_IO	,	CM3_C06_Wafer			,	0	} ,
	{ "CM3.C07_Wafer"				, _K_D_IO	,	CM3_C07_Wafer			,	0	} ,
	{ "CM3.C08_Wafer"				, _K_D_IO	,	CM3_C08_Wafer			,	0	} ,
	{ "CM3.C09_Wafer"				, _K_D_IO	,	CM3_C09_Wafer			,	0	} ,
	{ "CM3.C10_Wafer"				, _K_D_IO	,	CM3_C10_Wafer			,	0	} ,
	{ "CM3.C11_Wafer"				, _K_D_IO	,	CM3_C11_Wafer			,	0	} ,
	{ "CM3.C12_Wafer"				, _K_D_IO	,	CM3_C12_Wafer			,	0	} ,
	{ "CM3.C13_Wafer"				, _K_D_IO	,	CM3_C13_Wafer			,	0	} ,
	{ "CM3.C14_Wafer"				, _K_D_IO	,	CM3_C14_Wafer			,	0	} ,
	{ "CM3.C15_Wafer"				, _K_D_IO	,	CM3_C15_Wafer			,	0	} ,
	{ "CM3.C16_Wafer"				, _K_D_IO	,	CM3_C16_Wafer			,	0	} ,
	{ "CM3.C17_Wafer"				, _K_D_IO	,	CM3_C17_Wafer			,	0	} ,
	{ "CM3.C18_Wafer"				, _K_D_IO	,	CM3_C18_Wafer			,	0	} ,
	{ "CM3.C19_Wafer"				, _K_D_IO	,	CM3_C19_Wafer			,	0	} ,
	{ "CM3.C20_Wafer"				, _K_D_IO	,	CM3_C20_Wafer			,	0	} ,
	{ "CM3.C21_Wafer"				, _K_D_IO	,	CM3_C21_Wafer			,	0	} ,
	{ "CM3.C22_Wafer"				, _K_D_IO	,	CM3_C22_Wafer			,	0	} ,
	{ "CM3.C23_Wafer"				, _K_D_IO	,	CM3_C23_Wafer			,	0	} ,
	{ "CM3.C24_Wafer"				, _K_D_IO	,	CM3_C24_Wafer			,	0	} ,
	{ "CM3.C25_Wafer"				, _K_D_IO	,	CM3_C25_Wafer			,	0	} ,

	{ "CTC.MID_NAME"				, _K_S_IO	,	MID_NAME				,	0	} ,
	{ "CTC.MID_NAME2"				, _K_S_IO	,	MID_NAME2				,	0	} ,
	{ "CTC.MID_NAME3"				, _K_S_IO	,	MID_NAME3				,	0	} ,
	{ "CTC.MID_READ"				, _K_S_IO	,	MID_READ				,	0	} ,
	{ "CTC.MID_READ2"				, _K_S_IO	,	MID_READ2				,	0	} ,
	{ "CTC.MID_READ3"				, _K_S_IO	,	MID_READ3				,	0	} ,

	{ "CTC.JOB_NAME"				, _K_S_IO	,	JID_NAME				,	0	} ,
	{ "CTC.JOB_NAME2"				, _K_S_IO	,	JID_NAME2				,	0	} ,
	{ "CTC.JOB_NAME3"				, _K_S_IO	,	JID_NAME3				,	0	} ,
	{ "CTC.JID_READ"				, _K_S_IO	,	JID_READ				,	0	} ,
	{ "CTC.JID_READ2"				, _K_S_IO	,	JID_READ2				,	0	} ,
	{ "CTC.JID_READ3"				, _K_S_IO	,	JID_READ3				,	0	} ,

	{ "CTC.RECIPE_FILE"				, _K_S_IO	,	RECIPE_NAME				,	0	} ,
	{ "CTC.RECIPE_FILE2"			, _K_S_IO	,	RECIPE_NAME2			,	0	} ,
	{ "CTC.RECIPE_FILE3"			, _K_S_IO	,	RECIPE_NAME3			,	0	} ,

	{ "CTC.START_SLOT"				, _K_D_IO	,	START_SLOT				,	0	} ,
	{ "CTC.START_SLOT2"				, _K_D_IO	,	START_SLOT2				,	0	} ,
	{ "CTC.START_SLOT3"				, _K_D_IO	,	START_SLOT3				,	0	} ,

	{ "CTC.END_SLOT"				, _K_D_IO	,	END_SLOT				,	0	} ,
	{ "CTC.END_SLOT2"				, _K_D_IO	,	END_SLOT2				,	0	} ,
	{ "CTC.END_SLOT3"				, _K_D_IO	,	END_SLOT3				,	0	} ,

	{ "CTC.LOOP_COUNT"				, _K_D_IO	,	LOOP_COUNT				,	0	} ,
	{ "CTC.LOOP_COUNT2"				, _K_D_IO	,	LOOP_COUNT2				,	0	} ,
	{ "CTC.LOOP_COUNT3"				, _K_D_IO	,	LOOP_COUNT3				,	0	} ,

	{ "LPMA.F_ProperlyPlaceDI"		, _K_D_IO	,	CM1_CASS_STATUS			,	0	} ,
	{ "LPMB.F_ProperlyPlaceDI"		, _K_D_IO	,	CM2_CASS_STATUS			,	0	} ,
	{ "LPMC.F_ProperlyPlaceDI"		, _K_D_IO	,	CM3_CASS_STATUS			,	0	} ,

	{ "LPMA.FOUP_PresentDI"			, _K_D_IO	,	CM1_CASS_SenSor_STATUS	,	0	} ,
	{ "LPMB.FOUP_PresentDI"			, _K_D_IO	,	CM2_CASS_SenSor_STATUS	,	0	} ,
	{ "LPMC.FOUP_PresentDI"			, _K_D_IO	,	CM3_CASS_SenSor_STATUS	,	0	} ,

	{ "LPMA.AMHS_VALID_DI"			, _K_D_IO	,	CM1_AGV_INPUT_VALID		,	0	} ,
	{ "LPMB.AMHS_VALID_DI"			, _K_D_IO	,	CM2_AGV_INPUT_VALID		,	0	} ,
	{ "LPMC.AMHS_VALID_DI"			, _K_D_IO	,	CM3_AGV_INPUT_VALID		,	0	} ,

	{ "LPMA.AMHS_L_REQ_DO"			, _K_D_IO	,	CM1_AGV_OUTPUT_LREQ		,	0	} ,
	{ "LPMA.AMHS_U_REQ_DO"			, _K_D_IO	,	CM1_AGV_OUTPUT_UREQ		,	0	} ,
	{ "LPMA.AMHS_READY_DO"			, _K_D_IO	,	CM1_AGV_OUTPUT_READY	,	0	} ,

	{ "LPMB.AMHS_L_REQ_DO"			, _K_D_IO	,	CM2_AGV_OUTPUT_LREQ		,	0	} ,
	{ "LPMB.AMHS_U_REQ_DO"			, _K_D_IO	,	CM2_AGV_OUTPUT_UREQ		,	0	} ,
	{ "LPMB.AMHS_READY_DO"			, _K_D_IO	,	CM2_AGV_OUTPUT_READY	,	0	} ,

	{ "LPMC.AMHS_L_REQ_DO"			, _K_D_IO	,	CM3_AGV_OUTPUT_LREQ		,	0	} ,
	{ "LPMC.AMHS_U_REQ_DO"			, _K_D_IO	,	CM3_AGV_OUTPUT_UREQ		,	0	} ,
	{ "LPMC.AMHS_READY_DO"			, _K_D_IO	,	CM3_AGV_OUTPUT_READY	,	0	} ,

	{ "LPMA.DoorUpDownXI"			, _K_D_IO	,	CM1_DoorUpDownXI		,	0	} ,
	{ "LPMB.DoorUpDownXI"			, _K_D_IO	,	CM2_DoorUpDownXI		,	0	} ,
	{ "LPMC.DoorUpDownXI"			, _K_D_IO	,	CM3_DoorUpDownXI		,	0	} ,

	{ "LPMA.ClampUnclampXI"			, _K_D_IO	,	CM1_ClampUnclampXI		,	0	} ,
	{ "LPMB.ClampUnclampXI"			, _K_D_IO	,	CM2_ClampUnclampXI		,	0	} ,
	{ "LPMC.ClampUnclampXI"			, _K_D_IO	,	CM3_ClampUnclampXI		,	0	} ,

	{ "CTC.TR_CONTROL"				, _K_D_IO	,	Transfer_Status			,	0	} ,
	{ "CTC.TR_FINGER"				, _K_D_IO	,	CTC_TR_FINGER			,	0	} ,	//TA/TB/Auto/FA
	{ "TM.OverPresVvDO"				, _K_D_IO	,	TM_OverPresVvDO			,	0	} ,	//Close/Open
	{ "TM.RoughVvXO"				, _K_D_IO	,	TM_RoughVvXO			,	0	} ,	//Close/OpenSlow/OpenFast/Unknown

	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER				,	0	} ,

	{ "CM1.OPERATOR_CANCEL"			, _K_D_IO	,	CM1_OPERATOR_CANCEL		,	0	} ,
	{ "CM2.OPERATOR_CANCEL"			, _K_D_IO	,	CM2_OPERATOR_CANCEL		,	0	} ,
	{ "CM3.OPERATOR_CANCEL"			, _K_D_IO	,	CM3_OPERATOR_CANCEL		,	0	} ,

	{ "CM1.PROCESS_END"				, _K_D_IO	,	CM1_PROCESS_END			,	0	} ,
	{ "CM2.PROCESS_END"				, _K_D_IO	,	CM2_PROCESS_END			,	0	} ,
	{ "CM3.PROCESS_END"				, _K_D_IO	,	CM3_PROCESS_END			,	0	} ,

	{ "CM1.MAPWAFER_COUNT"			, _K_D_IO	,	CM1_MAPWAFER_COUNT		,	0	} ,
	{ "CM2.MAPWAFER_COUNT"			, _K_D_IO	,	CM2_MAPWAFER_COUNT		,	0	} ,
	{ "CM3.MAPWAFER_COUNT"			, _K_D_IO	,	CM3_MAPWAFER_COUNT		,	0	} ,

	{ "CM1.MAPWAFER_INFO"			, _K_S_IO	,	CM1_MAPWAFER_INFO		,	0	} ,
	{ "CM2.MAPWAFER_INFO"			, _K_S_IO	,	CM2_MAPWAFER_INFO		,	0	} ,
	{ "CM3.MAPWAFER_INFO"			, _K_S_IO	,	CM3_MAPWAFER_INFO		,	0	} ,

	{ "CTC.PM1_Wafer_Source"		, _K_D_IO	,	PM1_Wafer_Source		,	0	} ,
	{ "CTC.PM1_Wafer_Source2"		, _K_D_IO	,	PM1_Wafer_Source2		,	0	} ,
	{ "CTC.PM2_Wafer_Source"		, _K_D_IO	,	PM2_Wafer_Source		,	0	} ,
	{ "CTC.PM2_Wafer_Source2"		, _K_D_IO	,	PM2_Wafer_Source2		,	0	} ,
	{ "CTC.PM3_Wafer_Source"		, _K_D_IO	,	PM3_Wafer_Source		,	0	} ,
	{ "CTC.PM3_Wafer_Source2"		, _K_D_IO	,	PM3_Wafer_Source2		,	0	} ,

	{ "CTC.PM1_Wafer_Status"		, _K_D_IO	,	PM1_Wafer_Status		,	0	} ,
	{ "CTC.PM1_Wafer_Status2"		, _K_D_IO	,	PM1_Wafer_Status2		,	0	} ,
	{ "CTC.PM2_Wafer_Status"		, _K_D_IO	,	PM2_Wafer_Status		,	0	} ,
	{ "CTC.PM2_Wafer_Status2"		, _K_D_IO	,	PM2_Wafer_Status2		,	0	} ,
	{ "CTC.PM3_Wafer_Status"		, _K_D_IO	,	PM3_Wafer_Status		,	0	} ,
	{ "CTC.PM3_Wafer_Status2"		, _K_D_IO	,	PM3_Wafer_Status2		,	0	} ,

	{ "CTC.PM1_MIDName"				, _K_S_IO	,	PM1_MIDName				,	0	} ,
	{ "CTC.PM1_MIDName2"			, _K_S_IO	,	PM1_MIDName2			,	0	} ,
	{ "CTC.PM2_MIDName"				, _K_S_IO	,	PM2_MIDName				,	0	} ,
	{ "CTC.PM2_MIDName2"			, _K_S_IO	,	PM2_MIDName2			,	0	} ,
	{ "CTC.PM3_MIDName"				, _K_S_IO	,	PM3_MIDName				,	0	} ,
	{ "CTC.PM3_MIDName2"			, _K_S_IO	,	PM3_MIDName2			,	0	} ,

	{ "CTC.PM1_PROCESS_WAFER_NUMBER"	, _K_D_IO ,	PM1_PRCS_WFR_NUMBER		,	0	} ,
	{ "CTC.PM1_PROCESS_WAFER_NUMBER2"	, _K_D_IO ,	PM1_PRCS_WFR_NUMBER2	,	0	} ,
	{ "CTC.PM2_PROCESS_WAFER_NUMBER"	, _K_D_IO ,	PM2_PRCS_WFR_NUMBER		,	0	} ,
	{ "CTC.PM2_PROCESS_WAFER_NUMBER2"	, _K_D_IO ,	PM2_PRCS_WFR_NUMBER2	,	0	} ,
	{ "CTC.PM3_PROCESS_WAFER_NUMBER"	, _K_D_IO ,	PM3_PRCS_WFR_NUMBER		,	0	} ,
	{ "CTC.PM3_PROCESS_WAFER_NUMBER2"	, _K_D_IO ,	PM3_PRCS_WFR_NUMBER2	,	0	} ,

	{ "CM1.RFID_MID_PAGE"			, _K_S_IO	,	CM1_RFID_MID_PAGE	,	0	} ,
	{ "CM1.RFID_PAGE1"				, _K_S_IO	,	CM1_RFID_PAGE1		,	0	} ,
	{ "CM1.RFID_PAGE2"				, _K_S_IO	,	CM1_RFID_PAGE2		,	0	} ,
	{ "CM1.RFID_PAGE3"				, _K_S_IO	,	CM1_RFID_PAGE3		,	0	} ,
	{ "CM1.RFID_PAGE4"				, _K_S_IO	,	CM1_RFID_PAGE4		,	0	} ,
	{ "CM1.RFID_PAGE5"				, _K_S_IO	,	CM1_RFID_PAGE5		,	0	} ,
	{ "CM1.RFID_PAGE6"				, _K_S_IO	,	CM1_RFID_PAGE6		,	0	} ,
	{ "CM1.RFID_PAGE7"				, _K_S_IO	,	CM1_RFID_PAGE7		,	0	} ,
	{ "CM1.RFID_PAGE8"				, _K_S_IO	,	CM1_RFID_PAGE8		,	0	} ,
	{ "CM1.RFID_PAGE9"				, _K_S_IO	,	CM1_RFID_PAGE9		,	0	} ,
	{ "CM1.RFID_PAGE10"				, _K_S_IO	,	CM1_RFID_PAGE10		,	0	} ,
	{ "CM1.RFID_PAGE11"				, _K_S_IO	,	CM1_RFID_PAGE11		,	0	} ,
	{ "CM1.RFID_PAGE12"				, _K_S_IO	,	CM1_RFID_PAGE12		,	0	} ,
	{ "CM1.RFID_PAGE13"				, _K_S_IO	,	CM1_RFID_PAGE13		,	0	} ,
	{ "CM1.RFID_PAGE14"				, _K_S_IO	,	CM1_RFID_PAGE14		,	0	} ,
	{ "CM1.RFID_PAGE15"				, _K_S_IO	,	CM1_RFID_PAGE15		,	0	} ,
	{ "CM1.RFID_PAGE16"				, _K_S_IO	,	CM1_RFID_PAGE16		,	0	} ,
	{ "CM1.RFID_PAGE17"				, _K_S_IO	,	CM1_RFID_PAGE17		,	0	} ,

	{ "CM2.RFID_MID_PAGE"			, _K_S_IO	,	CM2_RFID_MID_PAGE	,	0	} ,
	{ "CM2.RFID_PAGE1"				, _K_S_IO	,	CM2_RFID_PAGE1		,	0	} ,
	{ "CM2.RFID_PAGE2"				, _K_S_IO	,	CM2_RFID_PAGE2		,	0	} ,
	{ "CM2.RFID_PAGE3"				, _K_S_IO	,	CM2_RFID_PAGE3		,	0	} ,
	{ "CM2.RFID_PAGE4"				, _K_S_IO	,	CM2_RFID_PAGE4		,	0	} ,
	{ "CM2.RFID_PAGE5"				, _K_S_IO	,	CM2_RFID_PAGE5		,	0	} ,
	{ "CM2.RFID_PAGE6"				, _K_S_IO	,	CM2_RFID_PAGE6		,	0	} ,
	{ "CM2.RFID_PAGE7"				, _K_S_IO	,	CM2_RFID_PAGE7		,	0	} ,
	{ "CM2.RFID_PAGE8"				, _K_S_IO	,	CM2_RFID_PAGE8		,	0	} ,
	{ "CM2.RFID_PAGE9"				, _K_S_IO	,	CM2_RFID_PAGE9		,	0	} ,
	{ "CM2.RFID_PAGE10"				, _K_S_IO	,	CM2_RFID_PAGE10		,	0	} ,
	{ "CM2.RFID_PAGE11"				, _K_S_IO	,	CM2_RFID_PAGE11		,	0	} ,
	{ "CM2.RFID_PAGE12"				, _K_S_IO	,	CM2_RFID_PAGE12		,	0	} ,
	{ "CM2.RFID_PAGE13"				, _K_S_IO	,	CM2_RFID_PAGE13		,	0	} ,
	{ "CM2.RFID_PAGE14"				, _K_S_IO	,	CM2_RFID_PAGE14		,	0	} ,
	{ "CM2.RFID_PAGE15"				, _K_S_IO	,	CM2_RFID_PAGE15		,	0	} ,
	{ "CM2.RFID_PAGE16"				, _K_S_IO	,	CM2_RFID_PAGE16		,	0	} ,
	{ "CM2.RFID_PAGE17"				, _K_S_IO	,	CM2_RFID_PAGE17		,	0	} ,

	{ "CM3.RFID_MID_PAGE"			, _K_S_IO	,	CM3_RFID_MID_PAGE	,	0	} ,
	{ "CM3.RFID_PAGE1"				, _K_S_IO	,	CM3_RFID_PAGE1		,	0	} ,
	{ "CM3.RFID_PAGE2"				, _K_S_IO	,	CM3_RFID_PAGE2		,	0	} ,
	{ "CM3.RFID_PAGE3"				, _K_S_IO	,	CM3_RFID_PAGE3		,	0	} ,
	{ "CM3.RFID_PAGE4"				, _K_S_IO	,	CM3_RFID_PAGE4		,	0	} ,
	{ "CM3.RFID_PAGE5"				, _K_S_IO	,	CM3_RFID_PAGE5		,	0	} ,
	{ "CM3.RFID_PAGE6"				, _K_S_IO	,	CM3_RFID_PAGE6		,	0	} ,
	{ "CM3.RFID_PAGE7"				, _K_S_IO	,	CM3_RFID_PAGE7		,	0	} ,
	{ "CM3.RFID_PAGE8"				, _K_S_IO	,	CM3_RFID_PAGE8		,	0	} ,
	{ "CM3.RFID_PAGE9"				, _K_S_IO	,	CM3_RFID_PAGE9		,	0	} ,
	{ "CM3.RFID_PAGE10"				, _K_S_IO	,	CM3_RFID_PAGE10		,	0	} ,
	{ "CM3.RFID_PAGE11"				, _K_S_IO	,	CM3_RFID_PAGE11		,	0	} ,
	{ "CM3.RFID_PAGE12"				, _K_S_IO	,	CM3_RFID_PAGE12		,	0	} ,
	{ "CM3.RFID_PAGE13"				, _K_S_IO	,	CM3_RFID_PAGE13		,	0	} ,
	{ "CM3.RFID_PAGE14"				, _K_S_IO	,	CM3_RFID_PAGE14		,	0	} ,
	{ "CM3.RFID_PAGE15"				, _K_S_IO	,	CM3_RFID_PAGE15		,	0	} ,
	{ "CM3.RFID_PAGE16"				, _K_S_IO	,	CM3_RFID_PAGE16		,	0	} ,
	{ "CM3.RFID_PAGE17"				, _K_S_IO	,	CM3_RFID_PAGE17		,	0	} ,

	{ "CTC.FA_STATUS_MDL_PM1"		, _K_D_IO	,	PM1_MODULE			,	0	} ,
	{ "CTC.FA_STATUS_MDL_PM2"		, _K_D_IO	,	PM2_MODULE			,	0	} ,
	{ "CTC.FA_STATUS_MDL_PM3"		, _K_D_IO	,	PM3_MODULE			,	0	} ,

	{ "PM1.MAINT_CONTROL"			, _K_D_IO	,	PM1_CONTROL			,	0	} ,
	{ "PM2.MAINT_CONTROL"			, _K_D_IO	,	PM2_CONTROL			,	0	} ,
	{ "PM3.MAINT_CONTROL"			, _K_D_IO	,	PM3_CONTROL			,	0	} ,
	{ "PM4.MAINT_CONTROL"			, _K_D_IO	,	PM4_CONTROL			,	0	} ,
	{ "PM5.MAINT_CONTROL"			, _K_D_IO	,	PM5_CONTROL			,	0	} ,

	{ "CTC.FA_STATUS_AGV_CM1"		, _K_D_IO	,	LPMA_AMHS_MODE		,	0	} ,
	{ "CTC.FA_STATUS_AGV_CM2"		, _K_D_IO	,	LPMB_AMHS_MODE		,	0	} ,
	{ "CTC.FA_STATUS_AGV_CM3"		, _K_D_IO	,	LPMC_AMHS_MODE		,	0	} ,

	// add 2006.05.08 by yccoi
	{ "LPMA.ErrorStatusDI"			, _K_D_IO	,	LPMA_ErrorStatusDI	,	0	} ,
	{ "LPMB.ErrorStatusDI"			, _K_D_IO	,	LPMB_ErrorStatusDI	,	0	} ,
	{ "LPMC.ErrorStatusDI"			, _K_D_IO	,	LPMC_ErrorStatusDI	,	0	} ,

    { "CTC.OPERATOR_START"			, _K_D_IO	,	OPERATOR_START           , 0   } ,
    { "CTC.OPERATOR_START2"			, _K_D_IO	,	OPERATOR_START2          , 0   } ,
    { "CTC.OPERATOR_START3"			, _K_D_IO	,	OPERATOR_START3          , 0   } ,
	{ "LPMA.AMHS_HO_AVBL_DO"		, _K_D_IO	,	LPMA_AGV_OUTPUT_HOAVBL,	0	} ,
	{ "LPMB.AMHS_HO_AVBL_DO"		, _K_D_IO	,	LPMB_AGV_OUTPUT_HOAVBL,	0	} ,
	{ "LPMC.AMHS_HO_AVBL_DO"		, _K_D_IO	,	LPMC_AGV_OUTPUT_HOAVBL,	0	} ,
	{ "PRMD_BUZZER_OPT"				, _K_D_IO	,	PRMD_BUZZER_OPT,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	ATM		,
	VACUUM	,
	GOATM	,
	GOVAC	,
	ABORT	,
} VacuumStatus;
//------------------------------------------------------------------------------------------
typedef enum {
	Idle	,
	Running	,
	Pausing	,
	Paused  ,
	Aborting,
	Disable ,
	Waiting ,
	Wait_HO ,
} StbMainCtrl;
//------------------------------------------------------------------------------------------
typedef enum {
	SOUND_ON		,
	SOUND_OFF		,
} Buzzer_Sound;
//------------------------------------------------------------------------------------------
typedef enum {
	OFF		,
	ON		,
	BLINK	,
} OffOnBlink;
//------------------------------------------------------------------------------------------
typedef enum
{
	UNKNOWN_SENSOR,
	ATM_SENSOR,
	VAC_SENSOR,
	ERROR_SENSOR
}ATMVAC_SENSOR_STATUS;
//------------------------------------------------------------------------------------------
enum { BM1 , BM2 , CM1 , CM2 , CM3 , AL };
enum { PORT1 , PORT2 , PORT3 };
enum { Manual, Auto };
enum { Off, On };
enum { EQ_OFFL  , ATTEMP_ONL , HOST_OFFL , LOCAL , REMOTE };
enum { Enalbe	, Dry_Reserved	, Dry_Running	}; // jelee add 2007.12.06
enum { Unuse, Local, DisConn, Standby, Maint, Uninit, EnablePM, CTCinUSE, CTCDis, CTCDis2, CTCEnaPM, CTCDisHW, CTCDIsHW2}; // jelee add 2007.12.06
enum { ON2, OFF2 };
enum { INTLK_OFF, INTLK_ON };
enum { Default, Red, Yellow, Green, AllOff };
//------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
BOOL Attatch6MDrvErrMesaage(char *errMsg, int nErrCode)
{
	char szDetailMessage[512];
	BOOL bRet = TRUE;

	switch(nErrCode)
	{
	case 0xc018:	sprintf(szDetailMessage, "(ErrCode:0x%04x)No compressed air available", nErrCode);	break;
	case 0xc019:	sprintf(szDetailMessage, "(ErrCode:0x%04x)Poor vacuum", nErrCode); break;
	case 0xc01b:	sprintf(szDetailMessage, "(ErrCode:0x%04x)Ligh beam of wafer protrusion sensors is broken", nErrCode); break;
	case 0xc01c:	sprintf(szDetailMessage, "(ErrCode:0x%04x)No system initialization", nErrCode);	break;
	case 0xc01d:	sprintf(szDetailMessage, "(ErrCode:0x%04x)Initialization incomplete",nErrCode);	break;
	case 0xc01e:	sprintf(szDetailMessage, "(ErrCode:0x%04x)Wafer protrusion sensor is not triggered", nErrCode);	break;
	case 0xc01f:	sprintf(szDetailMessage, "(ErrCode:0x%04x)24 V not available", nErrCode);	break;
	case 0xc070:	sprintf(szDetailMessage, "(ErrCode:0x%04x)Higher-level timeout", nErrCode);	break;
	case 0xc040:	sprintf(szDetailMessage, "(ErrCode:0x%04x)Mapper communication disconnected", nErrCode);	break;
	default:		sprintf(szDetailMessage, "(ErrCode:0x%04x)", nErrCode);	bRet = FALSE;	break;
	}
	strcat(errMsg, szDetailMessage);

	return bRet;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define CM1_FOUP_PLACE_ERROR 197
#define CM2_FOUP_PLACE_ERROR 497
#define CM3_FOUP_PLACE_ERROR 797

//... 2015.12.07
#define CM1_COMM_ERROR		159
#define CM2_COMM_ERROR		459
#define CM3_COMM_ERROR		759

int READ_DIGITAL_TRUE( int io , int *CommStatus ) {
	int Data;
	Data = READ_DIGITAL( io , CommStatus );
	if ( !(*CommStatus) ) {
		_sleep(250);
		Data = READ_DIGITAL( io , CommStatus );
		if ( !(*CommStatus) ) {
			_sleep(500);
			Data = READ_DIGITAL( io , CommStatus );
			if ( !(*CommStatus) ) {
				_sleep(1000);
				Data = READ_DIGITAL( io , CommStatus );
				if ( !(*CommStatus) ) {
					_sleep(1000);
					return( READ_DIGITAL( io , CommStatus ) );
				}
				else return Data;
			}
			else return Data;
		}
		else return Data;
	}
	else return Data;
}


//... 2015.11.23 Add OHT Loading Interlock
void CM1_FOUP_Check_Thread()
{
	int nCommStatus, nAlmResult, nCommStatus1, nCommStatus2;
	int nPresentSensor_Curr, nPlacementSenosr_Curr, nValidSignal, nAMHS_Mode, nHOAVBL;
	while(1)
	{
		nPresentSensor_Curr = READ_DIGITAL(CM1_CASS_SenSor_STATUS, &nCommStatus1);
		nPlacementSenosr_Curr = READ_DIGITAL(CM1_CASS_STATUS, &nCommStatus2);
		nAMHS_Mode = READ_DIGITAL(LPMA_AMHS_MODE, &nCommStatus);
		if ( nPresentSensor_Curr != nPlacementSenosr_Curr || nCommStatus1 == FALSE || nCommStatus2 == FALSE)
		{
			nValidSignal = READ_DIGITAL(CM1_AGV_INPUT_VALID, &nCommStatus);
			if ((nAMHS_Mode == Auto) && (nValidSignal == On))
				Sleep(6000);
			else if ((nAMHS_Mode == Auto) && (nValidSignal == Off))
				Sleep(3000);
			else
				Sleep(10000);

			nPresentSensor_Curr = READ_DIGITAL_TRUE(CM1_CASS_SenSor_STATUS, &nCommStatus1);
			nPlacementSenosr_Curr = READ_DIGITAL_TRUE(CM1_CASS_STATUS, &nCommStatus2);

			if(nCommStatus1 == FALSE || nCommStatus2 == FALSE)
			{
				ALARM_POST(CM1_COMM_ERROR);
				nHOAVBL = READ_DIGITAL(LPMA_AGV_OUTPUT_HOAVBL, &nCommStatus);
				if(nHOAVBL != 0)WRITE_DIGITAL(LPMA_AGV_OUTPUT_HOAVBL, 0, &nCommStatus);

				while ( TRUE )
				{
					_sleep(2);
					nAlmResult = ALARM_STATUS( CM1_COMM_ERROR );
					if ( nAlmResult != ALM_PAUSED ) break;
				}
			}
			else if ( nPresentSensor_Curr != nPlacementSenosr_Curr)
			{
				ALARM_POST(CM1_FOUP_PLACE_ERROR);
				nHOAVBL = READ_DIGITAL(LPMA_AGV_OUTPUT_HOAVBL, &nCommStatus);
				if(nHOAVBL != 0)WRITE_DIGITAL(LPMA_AGV_OUTPUT_HOAVBL, 0, &nCommStatus);

				while ( TRUE )
				{
					_sleep(2);
					nAlmResult = ALARM_STATUS( CM1_FOUP_PLACE_ERROR );
					if ( nAlmResult != ALM_PAUSED ) break;
				}
			}
		}
		Sleep(1000);
	}
	_endthread();
}

//... 2015.11.23 Add OHT Loading Interlock
void CM2_FOUP_Check_Thread()
{
	int nCommStatus, nAlmResult, nCommStatus1, nCommStatus2;
	int nPresentSensor_Curr, nPlacementSenosr_Curr, nValidSignal, nAMHS_Mode, nHOAVBL;
	while(1)
	{
		nPresentSensor_Curr = READ_DIGITAL(CM2_CASS_SenSor_STATUS, &nCommStatus1);
		nPlacementSenosr_Curr = READ_DIGITAL(CM2_CASS_STATUS, &nCommStatus2);
		nAMHS_Mode = READ_DIGITAL(LPMB_AMHS_MODE, &nCommStatus);
		if ( nPresentSensor_Curr != nPlacementSenosr_Curr || nCommStatus1 == FALSE || nCommStatus2 == FALSE)
		{
			nValidSignal = READ_DIGITAL(CM2_AGV_INPUT_VALID, &nCommStatus);
			if ((nAMHS_Mode == Auto) && (nValidSignal == On))
				Sleep(6000);
			else if ((nAMHS_Mode == Auto) && (nValidSignal == Off))
				Sleep(3000);
			else
				Sleep(10000);

			nPresentSensor_Curr = READ_DIGITAL_TRUE(CM2_CASS_SenSor_STATUS, &nCommStatus1);
			nPlacementSenosr_Curr = READ_DIGITAL_TRUE(CM2_CASS_STATUS, &nCommStatus2);

			if(nCommStatus1 == FALSE || nCommStatus2 == FALSE)
			{
				ALARM_POST(CM2_COMM_ERROR);
				nHOAVBL = READ_DIGITAL(LPMB_AGV_OUTPUT_HOAVBL, &nCommStatus);
				if(nHOAVBL != 0)WRITE_DIGITAL(LPMB_AGV_OUTPUT_HOAVBL, 0, &nCommStatus);

				while ( TRUE )
				{
					_sleep(2);
					nAlmResult = ALARM_STATUS( CM2_COMM_ERROR );
					if ( nAlmResult != ALM_PAUSED ) break;
				}
			}
			else if ( nPresentSensor_Curr != nPlacementSenosr_Curr)
			{
				ALARM_POST(CM2_FOUP_PLACE_ERROR);
				nHOAVBL = READ_DIGITAL(LPMB_AGV_OUTPUT_HOAVBL, &nCommStatus);
				if(nHOAVBL != 0)WRITE_DIGITAL(LPMB_AGV_OUTPUT_HOAVBL, 0, &nCommStatus);

				while ( TRUE )
				{
					_sleep(2);
					nAlmResult = ALARM_STATUS( CM2_FOUP_PLACE_ERROR );
					if ( nAlmResult != ALM_PAUSED ) break;
				}
			}
		}
		Sleep(1000);
	}
	_endthread();
}

//... 2015.11.23 Add OHT Loading Interlock
void CM3_FOUP_Check_Thread()
{
	int nCommStatus, nAlmResult, nCommStatus1, nCommStatus2;
	int nPresentSensor_Curr, nPlacementSenosr_Curr, nValidSignal, nAMHS_Mode, nHOAVBL;
	while(1)
	{
		nPresentSensor_Curr = READ_DIGITAL(CM3_CASS_SenSor_STATUS, &nCommStatus1);
		nPlacementSenosr_Curr = READ_DIGITAL(CM3_CASS_STATUS, &nCommStatus2);
		nAMHS_Mode = READ_DIGITAL(LPMC_AMHS_MODE, &nCommStatus);
		if ( nPresentSensor_Curr != nPlacementSenosr_Curr || nCommStatus1 == FALSE || nCommStatus2 == FALSE)
		{
			nValidSignal = READ_DIGITAL(CM3_AGV_INPUT_VALID, &nCommStatus);
			if ((nAMHS_Mode == Auto) && (nValidSignal == On))
				Sleep(6000);
			else if ((nAMHS_Mode == Auto) && (nValidSignal == Off))
				Sleep(3000);
			else
				Sleep(10000);
			nPresentSensor_Curr = READ_DIGITAL_TRUE(CM3_CASS_SenSor_STATUS, &nCommStatus1);
			nPlacementSenosr_Curr = READ_DIGITAL_TRUE(CM3_CASS_STATUS, &nCommStatus2);

			if(nCommStatus1 == FALSE || nCommStatus2 == FALSE)
			{
				ALARM_POST(CM3_COMM_ERROR);
				nHOAVBL = READ_DIGITAL(LPMC_AGV_OUTPUT_HOAVBL, &nCommStatus);
				if(nHOAVBL != 0)WRITE_DIGITAL(LPMC_AGV_OUTPUT_HOAVBL, 0, &nCommStatus);

				while ( TRUE )
				{
					_sleep(2);
					nAlmResult = ALARM_STATUS( CM3_COMM_ERROR );
					if ( nAlmResult != ALM_PAUSED ) break;
				}
			}
			else if ( nPresentSensor_Curr != nPlacementSenosr_Curr)
			{
				ALARM_POST(CM3_FOUP_PLACE_ERROR);
				nHOAVBL = READ_DIGITAL(LPMC_AGV_OUTPUT_HOAVBL, &nCommStatus);
				if(nHOAVBL != 0)WRITE_DIGITAL(LPMC_AGV_OUTPUT_HOAVBL, 0, &nCommStatus);

				while ( TRUE )
				{
					_sleep(2);
					nAlmResult = ALARM_STATUS( CM3_FOUP_PLACE_ERROR );
					if ( nAlmResult != ALM_PAUSED ) break;
				}
			}
		}
		Sleep(1000);
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// PM# MID Monitoring Function Add start by jelee 2007.12.10
void PM_MID_Check_Thread() {
	int nCommStatus, i;
	int nPM1_Wafer_Status[MAX_WAFERZONE]	= { PM1_Wafer_Status, PM1_Wafer_Status2 };
	int nPM1_Wafer_Source[MAX_WAFERZONE]	= { PM1_Wafer_Source, PM1_Wafer_Source2 };
	int nPM1_Wafer_MID[MAX_WAFERZONE]		= { PM1_MIDName, PM1_MIDName2 };
	int nPM1_Wafer_LOTID[MAX_WAFERZONE]		= { PM1_LOTID, PM1_LOTID2 };     //2014.10.14
	//
	int nPM2_Wafer_Status[MAX_WAFERZONE]	= { PM2_Wafer_Status, PM2_Wafer_Status2 };
	int nPM2_Wafer_Source[MAX_WAFERZONE]	= { PM2_Wafer_Source, PM2_Wafer_Source2 };
	int nPM2_Wafer_MID[MAX_WAFERZONE]		= { PM2_MIDName, PM2_MIDName2	};
	int nPM2_Wafer_LOTID[MAX_WAFERZONE]		= { PM2_LOTID, PM2_LOTID2	};   //2014.10.14
	//
	int nPM3_Wafer_Status[MAX_WAFERZONE]	= { PM3_Wafer_Status, PM3_Wafer_Status2 };
	int nPM3_Wafer_Source[MAX_WAFERZONE]	= { PM3_Wafer_Source, PM3_Wafer_Source2 };
	int nPM3_Wafer_MID[MAX_WAFERZONE]		= { PM3_MIDName, PM3_MIDName2	};
	int nPM3_Wafer_LOTID[MAX_WAFERZONE]		= { PM3_LOTID, PM3_LOTID2	};   //2014.10.14

	char szLPMA_MID[255]="", szLPMB_MID[255]="", szLPMC_MID[255]="", szDummy_MID[255]="Dummy";
	char szLPMA_LOTID[255]="", szLPMB_LOTID[255]="", szLPMC_LOTID[255]="", szDummy_LOTID[255]="Dummy";  //2014.10.14

	for( i = 0; i < MAX_WAFERZONE; i++ )
	{
		memset(&g_PM1WaferZone[i], 0, sizeof(g_PM1WaferZone[i]));
		memset(&g_PM2WaferZone[i], 0, sizeof(g_PM2WaferZone[i]));
		memset(&g_PM3WaferZone[i], 0, sizeof(g_PM3WaferZone[i]));
	}

	while(1){
		_LOG("###===>PM_MID_Check_Thread Start\n");
		while(1) {
			READ_STRING(MID_NAME, szLPMA_MID, &nCommStatus);
			READ_STRING(MID_NAME2, szLPMB_MID, &nCommStatus);
			READ_STRING(MID_NAME3, szLPMC_MID, &nCommStatus);
			//2014.10.14
			READ_STRING(JID_NAME, szLPMA_LOTID, &nCommStatus);
			READ_STRING(JID_NAME2, szLPMB_LOTID, &nCommStatus);
			READ_STRING(JID_NAME3, szLPMC_LOTID, &nCommStatus);
			//
			for(i = 0; i < MAX_WAFERZONE; i++)
			{
				g_PM1WaferZone[i].nWaferStatus = READ_DIGITAL(nPM1_Wafer_Status[i], &nCommStatus);
				g_PM1WaferZone[i].nWaferSource = READ_DIGITAL(nPM1_Wafer_Source[i], &nCommStatus);
				g_PM2WaferZone[i].nWaferStatus = READ_DIGITAL(nPM2_Wafer_Status[i], &nCommStatus);
				g_PM2WaferZone[i].nWaferSource = READ_DIGITAL(nPM2_Wafer_Source[i], &nCommStatus);
				g_PM3WaferZone[i].nWaferStatus = READ_DIGITAL(nPM3_Wafer_Status[i], &nCommStatus);
				g_PM3WaferZone[i].nWaferSource = READ_DIGITAL(nPM3_Wafer_Source[i], &nCommStatus);
				if ( 0 != g_PM1WaferZone[i].nWaferStatus && READ_DIGITAL(PM1_MODULE, &nCommStatus) == 1 )
				{
					if ( 0 == g_PM1WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM1_Wafer_MID[i], szLPMA_MID, &nCommStatus);
						WRITE_STRING( nPM1_Wafer_LOTID[i], szLPMA_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 1 == g_PM1WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM1_Wafer_MID[i], szLPMB_MID, &nCommStatus);
						WRITE_STRING( nPM1_Wafer_LOTID[i], szLPMB_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 2 == g_PM1WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM1_Wafer_MID[i], szLPMC_MID, &nCommStatus);
						WRITE_STRING( nPM1_Wafer_LOTID[i], szLPMC_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 5 == g_PM1WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM1_Wafer_MID[i], szDummy_MID, &nCommStatus);
						WRITE_STRING( nPM1_Wafer_LOTID[i], szDummy_LOTID, &nCommStatus);  //2014.10.14
					}
					else {
						printf("PM1 Wafer_Zone1 MID Error =>[%d]\n", g_PM1WaferZone[i].nWaferSource );
					}
				}
				else {
					WRITE_STRING(nPM1_Wafer_MID[i], "", &nCommStatus);
					WRITE_STRING( nPM1_Wafer_LOTID[i], "", &nCommStatus);  //2014.10.14
				}

				if ( 0 != g_PM2WaferZone[i].nWaferStatus && READ_DIGITAL(PM2_MODULE, &nCommStatus) == 1) {
					if ( 0 == g_PM2WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM2_Wafer_MID[i], szLPMA_MID, &nCommStatus);
						WRITE_STRING( nPM2_Wafer_LOTID[i], szLPMA_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 1 == g_PM2WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM2_Wafer_MID[i], szLPMB_MID, &nCommStatus);
						WRITE_STRING( nPM2_Wafer_LOTID[i], szLPMB_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 2 == g_PM2WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM2_Wafer_MID[i], szLPMC_MID, &nCommStatus);
						WRITE_STRING( nPM2_Wafer_LOTID[i], szLPMC_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 5 == g_PM2WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM2_Wafer_MID[i], szDummy_MID, &nCommStatus);
						WRITE_STRING( nPM2_Wafer_LOTID[i], szDummy_LOTID, &nCommStatus);  //2014.10.14
					}
					else {
						printf("PM2 Wafer_Zone1 MID Error =>[%d]\n", g_PM2WaferZone[i].nWaferSource );
					}
				}
				else{
					WRITE_STRING(nPM2_Wafer_MID[i], "", &nCommStatus);
					WRITE_STRING( nPM2_Wafer_LOTID[i], "", &nCommStatus);  //2014.10.14
				}

				if ( 0 != g_PM3WaferZone[i].nWaferStatus && READ_DIGITAL(PM3_MODULE, &nCommStatus) == 1) {
					if ( 0 == g_PM3WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM3_Wafer_MID[i], szLPMA_MID, &nCommStatus);
						WRITE_STRING( nPM3_Wafer_LOTID[i], szLPMA_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 1 == g_PM3WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM3_Wafer_MID[i], szLPMB_MID, &nCommStatus);
						WRITE_STRING( nPM3_Wafer_LOTID[i], szLPMB_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 2 == g_PM3WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM3_Wafer_MID[i], szLPMC_MID, &nCommStatus);
						WRITE_STRING( nPM3_Wafer_LOTID[i], szLPMC_LOTID, &nCommStatus);  //2014.10.14
					}
					else if ( 5 == g_PM3WaferZone[i].nWaferSource ) {
						WRITE_STRING( nPM3_Wafer_MID[i], szDummy_MID, &nCommStatus);
						WRITE_STRING( nPM3_Wafer_LOTID[i], szDummy_LOTID, &nCommStatus);  //2014.10.14
					}
					else {
						printf("PM3 Wafer_Zone1 MID Error =>[%d]\n", g_PM3WaferZone[i].nWaferSource );
					}
				}
				else {
					WRITE_STRING(nPM3_Wafer_MID[i], "", &nCommStatus);
					WRITE_STRING( nPM3_Wafer_LOTID[i], "", &nCommStatus);  //2014.10.14
				}
			}
			Sleep(1000);
		}
		_LOG("###===>PM_MID_Check_Thread Kill\n");
	}

	_endthread();
}
// PM# MID Monitoring Function Add End by jelee 2007.12.10
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define CM1_6MDRV_ERR_ALARM	130
#define CM2_6MDRV_ERR_ALARM	430
#define CM3_6MDRV_ERR_ALARM	730

void CM1_ErrorCode_Check_Thread()
{
	int CommStatus;
	int nErrCode;
	char errMsg[512];

	while(TRUE)
	{
		_sleep(1000); //... 2016.03.03
		nErrCode = READ_DIGITAL(LPMA_ErrorStatusDI, &CommStatus);

		if(nErrCode > 2 && ALARM_STATUS(CM1_6MDRV_ERR_ALARM) != ALM_PAUSED)
		{
			ALARM_MESSAGE_GET(CM1_6MDRV_ERR_ALARM, errMsg);

			// Alarm Post가 필요한 경우에만 처리
			if(Attatch6MDrvErrMesaage(errMsg, nErrCode))
			{
				ALARM_MESSAGE_SET(CM1_6MDRV_ERR_ALARM, errMsg);
				ALARM_MANAGE(CM1_6MDRV_ERR_ALARM);
				WRITE_DIGITAL(LPMA_ErrorStatusDI, 1, &CommStatus);
			}

			_LOG("CM1 :: %s", errMsg);
		}
	}
	_endthread();
}

void CM2_ErrorCode_Check_Thread()
{
	int CommStatus;
	int nErrCode;
	char errMsg[512];

	while(TRUE)
	{
		_sleep(1000); //... 2016.03.03
		nErrCode = READ_DIGITAL(LPMB_ErrorStatusDI, &CommStatus);
		if(nErrCode > 2 && ALARM_STATUS(CM2_6MDRV_ERR_ALARM) != ALM_PAUSED)
		{
			ALARM_MESSAGE_GET(CM2_6MDRV_ERR_ALARM, errMsg);

			// Alarm Post가 필요한 경우에만 처리
			if(Attatch6MDrvErrMesaage(errMsg, nErrCode))
			{
				ALARM_MESSAGE_SET(CM2_6MDRV_ERR_ALARM, errMsg);
				ALARM_MANAGE(CM2_6MDRV_ERR_ALARM);
				WRITE_DIGITAL(LPMB_ErrorStatusDI, 1, &CommStatus);
			}

			_LOG("CM2 :: %s", errMsg);
		}
	}
	_endthread();
}

void CM3_ErrorCode_Check_Thread()
{
	int CommStatus;
	int nErrCode;
	char errMsg[512];

	while(TRUE)
	{
		_sleep(1000); //... 2016.03.03
		nErrCode = READ_DIGITAL(LPMC_ErrorStatusDI, &CommStatus);
		if(nErrCode > 2 && ALARM_STATUS(CM3_6MDRV_ERR_ALARM) != ALM_PAUSED)
		{
			ALARM_MESSAGE_GET(CM3_6MDRV_ERR_ALARM, errMsg);

			// Alarm Post가 필요한 경우에만 처리
			if(Attatch6MDrvErrMesaage(errMsg, nErrCode))
			{
				ALARM_MESSAGE_SET(CM3_6MDRV_ERR_ALARM, errMsg);
				ALARM_MANAGE(CM3_6MDRV_ERR_ALARM);
				WRITE_DIGITAL(LPMC_ErrorStatusDI, 1, &CommStatus);
			}

			_LOG("CM3 :: %s", errMsg);
		}
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Clear_PortData( int Port ) {
	int Index , CommStatus , i;
	if      ( Port == PORT1 ) Index = CM1_C01_Wafer;
	else if ( Port == PORT2 ) Index = CM2_C01_Wafer;
	else if ( Port == PORT3 ) Index = CM3_C01_Wafer;

	for ( i = 0 ; i < MAX_SLOT ; i++ )
	{
		WRITE_DIGITAL( Index + i , 0 , &CommStatus ); // unknown
	}

	if      ( Port == PORT1 ) Index = CM1_RFID_MID_PAGE;
	else if ( Port == PORT2 ) Index = CM2_RFID_MID_PAGE;
	else if ( Port == PORT3 ) Index = CM3_RFID_MID_PAGE;
	else return;

	for ( i = 0 ; i < MAX_PAGE ; i++ ) WRITE_STRING( Index + i , "" , &CommStatus );
	WRITE_STRING ( MID_READ		+ Port , "" , &CommStatus );
	WRITE_STRING ( MID_NAME		+ Port , "" , &CommStatus );
	WRITE_STRING ( JID_READ		+ Port , "" , &CommStatus );
	WRITE_STRING ( JID_NAME		+ Port , "" , &CommStatus );
	WRITE_STRING ( RECIPE_NAME	+ Port , "" , &CommStatus );
	WRITE_DIGITAL( CM1_PROCESS_END	  + Port , OFF , &CommStatus );
	WRITE_DIGITAL( CM1_MAPWAFER_COUNT + Port , 0   , &CommStatus );
	WRITE_STRING ( CM1_MAPWAFER_INFO  + Port , ""  , &CommStatus );

	// add Default Wafer & Loop Conunt setting by jelee 2006.06.01
	WRITE_DIGITAL ( START_SLOT  + Port , 1  , &CommStatus );
	WRITE_DIGITAL ( END_SLOT    + Port , 25  , &CommStatus );
	WRITE_DIGITAL ( LOOP_COUNT  + Port , 1  , &CommStatus );

	// 2004.07.26 by Kornic Inform, for FA
	WRITE_STRING ( GEM_Port1_LotID	  + Port , ""  , &CommStatus );
	WRITE_DIGITAL( OPERATOR_START + Port , OFF2, &CommStatus);
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Port_Foup_Check() {
	int CommStatus ,i ,Cass ,nPresent;

	int nErrCnt[MAX_CASSETTE];

	BOOL Check[MAX_CASSETTE]  , Check_Old[MAX_CASSETTE];
	//BOOL Check2[MAX_CASSETTE] , Check_Old2[MAX_CASSETTE];
	//BOOL Check3[MAX_CASSETTE] , Check_Old3[MAX_CASSETTE];

	for ( i = 0 ; i < MAX_CASSETTE ; i++  ) {
		Cass     = READ_DIGITAL( CM1_CASS_STATUS + i , &CommStatus );
		nPresent = READ_DIGITAL( CM1_CASS_SenSor_STATUS + i , &CommStatus );	// [2/1/2016 jonghyuk] Foup Status Reset 문제 보완

		if ( Cass != 1 && nPresent != 1) Check[i] = Check_Old[i] = TRUE;
		else Check[i] = Check_Old[i] = FALSE;

		nErrCnt[i] = 0;

		//Door_Status  = READ_DIGITAL( CM1_DoorUpDownXI+i , &CommStatus );
		//if ( Door_Status == 2 ) Check2[i] = Check_Old2[i] = FALSE;
		//else Check2[i] = Check_Old2[i] = TRUE;
		//Clamp_Status  = READ_DIGITAL( CM1_ClampUnclampXI+i   , &CommStatus );
		//if ( Clamp_Status == 1 ) Check3[i] = Check_Old3[i] = FALSE;
		//else Check3[i] = Check_Old3[i] = TRUE;
	}

	// when program init, port data clear if foup is absent.
	for ( i = 0 ; i < MAX_CASSETTE ; i++  )
	{
		if ( READ_DIGITAL( CM1_CASS_STATUS + i , &CommStatus ) != 1 && READ_DIGITAL( CM1_CASS_SenSor_STATUS + i , &CommStatus ) != 1)
		{
			Clear_PortData( i );
		}
	}

	while ( TRUE ) {
		_sleep( 100 );
		for ( i = 0 ; i < MAX_CASSETTE ; i++ )
		{
			// [2/1/2016 jonghyuk] Foup Status Reset 문제 보완
			if ( READ_DIGITAL( CM1_CASS_STATUS + i , &CommStatus ) != 1 && READ_DIGITAL( CM1_CASS_SenSor_STATUS + i , &CommStatus ) != 1 ) {

				nErrCnt[i]++;
				//printf("FoupStatusCheck : nErrCnt[%d]:%d\n", i, nErrCnt[i]);

				if( nErrCnt[i] > 10 ) {
					if ( READ_DIGITAL(LOT_CONTROL + i, &CommStatus) != Running ) {
						Check[i] = TRUE;
						if ( Check[i] != Check_Old[i] ) {
							Check_Old[i] = Check[i];
							Clear_PortData( i ); // Delete 2006.09.08  by hwasaeng
							Check[i] = TRUE;
						}
					}
					else {
						Check[i] = TRUE;
						if ( Check[i] != Check_Old[i] ) {
							Check_Old[i] = Check[i];
							if( ALARM_STATUS( ALARM_FOUP_ABSENT_RUNNING_CM1 + i*m_nAlarmPointer ) != ALM_PAUSED )	ALARM_POST( ALARM_FOUP_ABSENT_RUNNING_CM1 + i*m_nAlarmPointer );
							Check[i] = TRUE;
						}
					}

					nErrCnt[i] = 0;
				}
			}
			else {
				nErrCnt[i] = 0;
				Check[i] = FALSE;
				if ( Check[i] != Check_Old[i] ) {
					Check_Old[i] = Check[i];
				}
			}
		}
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Module_Control() {
	int i , Module , Control , CommStatus;
	while ( TRUE ) {
		_sleep( 100 );
		for ( i = 0 ; i < 4 ; i++ ) {
			Module = READ_DIGITAL( PM1_MODULE + i , &CommStatus );
			Control = READ_DIGITAL( PM1_CONTROL + i , &CommStatus );
			if ( Module != 2 ) { // 2 = DISABLE
				WRITE_DIGITAL( PM1_CONTROL + i , 2 , &CommStatus );
			}
			else { // DISABLE
				Control = READ_DIGITAL( PM1_CONTROL + i  , &CommStatus );
				if ( Control == 2 ) WRITE_DIGITAL( PM1_CONTROL + i , 0 , &CommStatus );
			}
		}
	}
	_endthread();
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	int CommStatus , LotSts , Lot2Sts , Lot3Sts , i;//, CommStatusA, CommStatusB, CommStatusC ;
	int WfrNum[MAX_ALL_PM_WAFER] , WfrNumOld[MAX_ALL_PM_WAFER] , WfrCount;
	int SrcNum[MAX_ALL_PM_WAFER] , SrcNumOld[MAX_ALL_PM_WAFER];
	int BUF, ALARM_LAMP, m_lampOp;
	int PM1_Status, PM2_Status, PM3_Status;
	int AlarmCnt;

	ALARM_LAMP   = -1;

	_sleep( 3000 );

	// jelee add for Foup Sensor Monitoring 2007.03.07
	_beginthread( (void*)CM1_FOUP_Check_Thread , 0 , NULL );
	_beginthread( (void*)CM2_FOUP_Check_Thread , 0 , NULL );
	_beginthread( (void*)CM3_FOUP_Check_Thread , 0 , NULL );

	_beginthread( (void*)PM_MID_Check_Thread , 0 , NULL );

	printf( "Configuration file Load starting(Monitoring function)!!!.............\n" );
	S_TIMER_READY();

	while ( TRUE )
	{
		WAIT_SECONDS(0.1);
		if ( READ_DIGITAL( CONFIG_LOADING , &CommStatus ) == ON ) break;
	}
	printf( "Configuration file Loading end((Monitoring function - %d sec)!!!.............\n" , (int)S_TIMER_ELAPSED() );

	// add 2006.05.08 by ycchoi
	_beginthread((void*)CM1_ErrorCode_Check_Thread, 0, NULL);
	_beginthread((void*)CM2_ErrorCode_Check_Thread, 0, NULL);
	_beginthread((void*)CM3_ErrorCode_Check_Thread, 0, NULL);

	//
	if( READ_ANALOG( Pressure_CG_BM1 , &CommStatus ) <= READ_ANALOG( F_PUMP_PRESSURE_BM1 , &CommStatus ) ) {
		WRITE_DIGITAL( BM1_Vacuum_Status , VACUUM , &CommStatus );
	}
	else if ( READ_DIGITAL( BM1_ATMVACSensorXI , &CommStatus ) == ATM_SENSOR ) {
		WRITE_DIGITAL( BM1_Vacuum_Status , ATM , &CommStatus );
	}
	else WRITE_DIGITAL( BM1_Vacuum_Status , ABORT , &CommStatus );

	if( READ_ANALOG( Pressure_CG_BM2 , &CommStatus ) <= READ_ANALOG( F_PUMP_PRESSURE_BM2 , &CommStatus ) ) {
		WRITE_DIGITAL( BM2_Vacuum_Status , VACUUM , &CommStatus );
	}
	else if ( READ_DIGITAL( BM2_ATMVACSensorXI , &CommStatus ) == ATM_SENSOR ) {
		WRITE_DIGITAL( BM2_Vacuum_Status , ATM , &CommStatus );
	}
	else WRITE_DIGITAL( BM2_Vacuum_Status , ABORT , &CommStatus );

	if( READ_ANALOG( Pressure_CG_TM , &CommStatus ) <= READ_ANALOG( F_PUMP_PRESSURE_TM , &CommStatus ) ) {
		WRITE_DIGITAL( TM_Vacuum_Status , VACUUM , &CommStatus );
	}
	else if ( READ_DIGITAL( TM_ATMVACSensorXI , &CommStatus ) == ATM_SENSOR ) {
		WRITE_DIGITAL( TM_Vacuum_Status , ATM , &CommStatus );
	}
	else WRITE_DIGITAL( TM_Vacuum_Status , ABORT , &CommStatus );
//==
	_beginthread( (void*)Port_Foup_Check					, 0 , NULL );
	_beginthread( (void*)Module_Control						, 0 , NULL );

	for ( i = 0 ; i < MAX_ALL_PM_WAFER ; i++ )
	{
		WfrNumOld[i] = -1;
		SrcNumOld[i] = -1;
	}

	while ( TRUE )
	{
		_sleep( 250 );

		//////////////////////////////////////////////////////////////////////////
		//... 2015.01.02 PM Wafer Processed Count 오류 수정
		//=============================== Chamber Process Wafer Count ============================//
		LotSts  = READ_DIGITAL( LOT_CONTROL   , &CommStatus );
		Lot2Sts = READ_DIGITAL( LOT2_CONTROL  , &CommStatus );
		Lot3Sts = READ_DIGITAL( LOT3_CONTROL  , &CommStatus );
		// CTC.MAIN_CONTROL	LOT_CONTROL	CTCControl   Idle  Running  Pausing  Paused  Aborting  Disable  Waiting  Wait-HandOff
		// enum { Idle, Running, Pausing, Paused, Aborting, Disable, Waiting, Wait_HO} StbMainCtrl;
		if(LotSts == Running || Lot2Sts == Running || Lot3Sts == Running)
		{	// 1 = Running
			for(i = 0; i < MAX_ALL_PM_WAFER; i++)
			{
				WfrNum[i] = READ_DIGITAL(PM1_Wafer_Status + i, &CommStatus);
				SrcNum[i] = READ_DIGITAL(PM1_Wafer_Source + i, &CommStatus);

				if(SrcNum[i] != SrcNumOld[i])
				{
					SrcNumOld[i] = SrcNum[i];
					WfrNumOld[i] = -1;
					WRITE_DIGITAL(PM1_PRCS_WFR_NUMBER + i, 0, &CommStatus);
				}

				if(WfrNum[i] != 0 && SrcNum[i] < 4 && WfrNum[i] != WfrNumOld[i])
				{
					WfrNumOld[i] = WfrNum[i];
					WfrCount = READ_DIGITAL(PM1_PRCS_WFR_NUMBER + i, &CommStatus);
					WRITE_DIGITAL(PM1_PRCS_WFR_NUMBER + i, WfrCount + 1, &CommStatus);
				}
			}
		}
		else if((LotSts  == Idle || LotSts  == Waiting || LotSts  == Wait_HO) &&
			    (Lot2Sts == Idle || Lot2Sts == Waiting || Lot2Sts == Wait_HO) &&
				(Lot3Sts == Idle || Lot3Sts == Waiting || Lot3Sts == Wait_HO))
		{	//... Not Present Foup
			for(i = 0; i < MAX_ALL_PM_WAFER; i++)
			{
				WRITE_DIGITAL(PM1_PRCS_WFR_NUMBER + i, 0, &CommStatus);
				SrcNumOld[i] = -1;
				WfrNumOld[i] = -1;
			}
		}

		//=============================== Operator Cancel Signal Clear ============================//
		switch( LotSts ) {
			case Running:
			case Pausing:
			case Paused:
			case Aborting:
				WRITE_DIGITAL( CM1_OPERATOR_CANCEL , OFF , &CommStatus );
				break;
			default:
				break;
		}
		switch( Lot2Sts ) {
			case Running:
			case Pausing:
			case Paused:
			case Aborting:
				WRITE_DIGITAL( CM2_OPERATOR_CANCEL , OFF , &CommStatus );
				break;
			default:
				break;
		}
		switch( Lot3Sts ) {
			case Running:
			case Pausing:
			case Paused:
			case Aborting:
				WRITE_DIGITAL( CM3_OPERATOR_CANCEL , OFF , &CommStatus );
				break;
			default:
				break;
		}

		// 2020.01.15 Signal Tower Comm SPEC
		// 적(고장) / 황(대기) / 녹(가동)
		// 조건
		// 	- 점멸(깜빡임) 없으며, 두개 동시에 켜지는것도 없음.
       	// 	- 멀티 Chamber 중 한 Chamber 라도 Alarm(PM,BM 포함) 이면 적색.
       	// 	- All Chamber 가동중일때 녹색.
    	// 	- All Chamber Idle 일때 황색.
		// 오직 하나의 Lamp만 점등.
		// LAMP 점멸 없음.
		// Run, Alarm 공존 시, Alarm 우선.
		// PM(Preventive, Maintenance ) 1 Chamber 라도 해당 되면 적색 점등. ( Chamber Maint시 )
		//	Red      Yellow   Green
		//	On       OFF      OFF       Alarm ( Abort성, PM,BM으로 인한 Chamber Maint시에도 )
		//	OFF		 ON		  OFF       Idle  ( All PM Enable && No Alarm && Idle )
		//	OFF      OFF      ON        Run   ( 1 PM 이라도 Run 상황이면 && No Alarm && Chamber 모두 Enable. )

		//=============================== Signal Tower Red Buzzer Alarm ============================//
		m_lampOp = READ_DIGITAL(PRMD_SIGNALTOWER_OPT, &CommStatus);
		PM1_Status  = READ_DIGITAL(PM1_CommStatus, &CommStatus);
		PM2_Status = READ_DIGITAL(PM2_CommStatus, &CommStatus);
		PM3_Status = READ_DIGITAL(PM3_CommStatus, &CommStatus);

		if (m_lampOp == Red)
		{
			WRITE_DIGITAL(RED_SIGNAL_ONOFF, ON, &CommStatus);
			WRITE_DIGITAL(YELLOW_SIGNAL_ONOFF, OFF, &CommStatus);
			WRITE_DIGITAL(GREEN_SIGNAL_ONOFF , OFF, &CommStatus);
		}
		else if (m_lampOp == Yellow)
		{
			WRITE_DIGITAL(RED_SIGNAL_ONOFF, OFF, &CommStatus);
			WRITE_DIGITAL(YELLOW_SIGNAL_ONOFF, ON, &CommStatus);
			WRITE_DIGITAL(GREEN_SIGNAL_ONOFF , OFF, &CommStatus);
		}
		else if (m_lampOp == Green)
		{
			WRITE_DIGITAL(RED_SIGNAL_ONOFF, OFF, &CommStatus);
			WRITE_DIGITAL(YELLOW_SIGNAL_ONOFF, OFF, &CommStatus);
			WRITE_DIGITAL(GREEN_SIGNAL_ONOFF , ON, &CommStatus);
		}
		else if (m_lampOp == AllOff)
		{
			WRITE_DIGITAL(RED_SIGNAL_ONOFF, OFF, &CommStatus);
			WRITE_DIGITAL(YELLOW_SIGNAL_ONOFF, OFF, &CommStatus);
			WRITE_DIGITAL(GREEN_SIGNAL_ONOFF , OFF, &CommStatus);
		}
		else	// Default
		{
			//BUF = READ_DIGITAL(CTC_Alarm_Status, &CommStatus);
			if ((PM1_SW_GbIntlckDM == INTLK_ON || PM2_SW_GbIntlckDM == INTLK_ON || PM3_SW_GbIntlckDM == INTLK_ON))
			{
				BUF = 1;
			}
			else if (((PM1_Status == DisConn) || (PM1_Status == Maint)) ||
			            ((PM2_Status == DisConn) || (PM2_Status == Maint)) ||
			                ((PM3_Status == DisConn) || (PM3_Status == Maint)))
			{
				BUF = 1;
			}
			else
			{
				BUF = 0;
			}

			if (BUF == 0)
			{ // Alarm Absent
				WRITE_DIGITAL(RED_SIGNAL_ONOFF, OFF, &CommStatus);
				WRITE_DIGITAL(ALARM_BUZZER_ONOFF, OFF, &CommStatus);
			}
			else
			{ // Alarm Present

				AlarmCnt = ALARMQUEUECOUNT();
				// printf("ALARMQUEUECOUNT [%d]\n", AlarmCnt);

				if (READ_DIGITAL(RED_SIGNAL_ONOFF, &CommStatus) == OFF)
				{
				    WRITE_DIGITAL(RED_SIGNAL_ONOFF, ON, &CommStatus);
				}

				if (READ_DIGITAL(CTC_Alarm_Happen, &CommStatus))
				{
				    WRITE_DIGITAL(CTC_Alarm_Happen, OFF, &CommStatus);
				}

				//... 2017.12.23 Add Buuzer Use On/off parameter
				if (READ_DIGITAL(PRMD_BUZZER_OPT, &CommStatus ) == ON)
			    {
					if (READ_DIGITAL(ALARM_BUZZER_ONOFF, &CommStatus ) == OFF)
					{
						WRITE_DIGITAL(ALARM_BUZZER_ONOFF, ON, &CommStatus);
					}
			    }
			    else
			    {
			        if (READ_DIGITAL(ALARM_BUZZER_ONOFF, &CommStatus ) == ON)
			        {
			            WRITE_DIGITAL(ALARM_BUZZER_ONOFF, OFF, &CommStatus);
			        }
			    }

			}

			//=============================== Signal Tower Green Yellow ============================//
			if (LotSts != Idle && LotSts != Disable && LotSts != Wait_HO) LotSts = Running;
			if (Lot2Sts != Idle && Lot2Sts != Disable && Lot2Sts != Wait_HO) Lot2Sts = Running;
			if (Lot3Sts != Idle && Lot3Sts != Disable && Lot3Sts != Wait_HO) Lot3Sts = Running;

			if (LotSts == Running || Lot2Sts == Running || Lot3Sts == Running)
			{ // System is Running
				if (BUF == 0)
				{
					if (READ_DIGITAL(GREEN_SIGNAL_ONOFF , &CommStatus) != ON)
					{
						WRITE_DIGITAL(GREEN_SIGNAL_ONOFF , ON, &CommStatus);
					}
					if (READ_DIGITAL(YELLOW_SIGNAL_ONOFF, &CommStatus) != OFF)
					{
						WRITE_DIGITAL( YELLOW_SIGNAL_ONOFF , OFF , &CommStatus );
					}
					WRITE_DIGITAL(RED_SIGNAL_ONOFF, OFF, &CommStatus);
				}
				else
				{
					if (READ_DIGITAL(GREEN_SIGNAL_ONOFF, &CommStatus) != OFF)
					{
						WRITE_DIGITAL(GREEN_SIGNAL_ONOFF, OFF, &CommStatus);
					}
					if (READ_DIGITAL(YELLOW_SIGNAL_ONOFF, &CommStatus) != OFF)
					{
						WRITE_DIGITAL(YELLOW_SIGNAL_ONOFF, OFF, &CommStatus);
					}
				}
			}

			if ((LotSts != Running && Lot2Sts != Running && Lot3Sts != Running))
			{ // System is Idle
				if (BUF == 0)
				{
					if (READ_DIGITAL(GREEN_SIGNAL_ONOFF, &CommStatus) != OFF)
					{
						WRITE_DIGITAL(GREEN_SIGNAL_ONOFF, OFF, &CommStatus);
					}
					if (READ_DIGITAL(YELLOW_SIGNAL_ONOFF, &CommStatus) != ON)
					{
						WRITE_DIGITAL(YELLOW_SIGNAL_ONOFF, ON, &CommStatus);
					}
					WRITE_DIGITAL(RED_SIGNAL_ONOFF, OFF, &CommStatus);
				}
				else
				{
					if (READ_DIGITAL(GREEN_SIGNAL_ONOFF, &CommStatus) != OFF)
					{
						WRITE_DIGITAL(GREEN_SIGNAL_ONOFF, OFF, &CommStatus);
					}
					if (READ_DIGITAL(YELLOW_SIGNAL_ONOFF, &CommStatus) != OFF)
					{
						WRITE_DIGITAL(YELLOW_SIGNAL_ONOFF, OFF, &CommStatus);
					}
				}
			}
		}

	}	//... Unlimited Cycle

	return SYS_SUCCESS;
}

//
//
