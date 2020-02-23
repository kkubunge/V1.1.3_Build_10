#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE = FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding			,	0	} ,

	{ "TM.PM1_Gate_Status"		, _K_D_IO	,	PM1_Gate_Status				,	0	} ,
	{ "TM.PM2_Gate_Status"		, _K_D_IO	,	PM2_Gate_Status				,	0	} ,
	{ "TM.PM3_Gate_Status"		, _K_D_IO	,	PM3_Gate_Status				,	0	} ,
	{ "TM.PM4_Gate_Status"		, _K_D_IO	,	PM4_Gate_Status				,	0	} ,
	{ "TM.PM5_Gate_Status"		, _K_D_IO	,	PM5_Gate_Status				,	0	} ,
//	{ "TM.PM6_Gate_Status"		, _K_D_IO	,	PM6_Gate_Status				,	0	} ,

	{ "CTC.PM1_Wafer_Status"	, _K_D_IO	,	PM1_Wafer_Status			,	0	} ,
	{ "CTC.PM2_Wafer_Status"	, _K_D_IO	,	PM2_Wafer_Status			,	0	} ,
	{ "CTC.PM3_Wafer_Status"	, _K_D_IO	,	PM3_Wafer_Status			,	0	} ,
	{ "CTC.PM4_Wafer_Status"	, _K_D_IO	,	PM4_Wafer_Status			,	0	} ,
	{ "CTC.PM5_Wafer_Status"	, _K_D_IO	,	PM5_Wafer_Status			,	0	} ,
//	{ "CTC.PM6_Wafer_Status"	, _K_D_IO	,	PM6_Wafer_Status			,	0	} ,

	{ "TM.ATM_Status_ForeDry"	, _K_D_IO	,	ATM_Status_ForeDry			,	0	} ,
	{ "TM.ATM_Status_RoughDry"	, _K_D_IO	,	ATM_Status_RoughDry			,	0	} ,

	{ "VG.TM_ForeCONPressure"	, _K_A_IO	,	TM_ForeCONPressure			,	0	} ,
	{ "VG.TM_RoughCONPressure"	, _K_A_IO	,	TM_RoughCONPressure			,	0	} ,

	{ "TM.ForeDRP_Status"		, _K_D_IO	,	ForeDRP_Status				,	0	} ,
	{ "TM.RoughDRP_Status"		, _K_D_IO	,	RoughDRP_Status				,	0	} ,

	{ "PM1.Gate_Status"			, _K_D_IO	,	MPM1_Gate_Status			,	0	} ,
	{ "PM2.Gate_Status"			, _K_D_IO	,	MPM2_Gate_Status			,	0	} ,
	{ "PM3.Gate_Status"			, _K_D_IO	,	MPM3_Gate_Status			,	0	} ,
	{ "PM4.Gate_Status"			, _K_D_IO	,	MPM4_Gate_Status			,	0	} ,
	{ "PM5.Gate_Status"			, _K_D_IO	,	MPM5_Gate_Status			,	0	} ,
//	{ "PM6.Gate_Status"			, _K_D_IO	,	MPM6_Gate_Status			,	0	} ,

	{ "PM1.Wafer_Status"		, _K_D_IO	,	MPM1_Wafer_Status			,	0	} ,
	{ "PM2.Wafer_Status"		, _K_D_IO	,	MPM2_Wafer_Status			,	0	} ,
	{ "PM3.Wafer_Status"		, _K_D_IO	,	MPM3_Wafer_Status			,	0	} ,
	{ "PM4.Wafer_Status"		, _K_D_IO	,	MPM4_Wafer_Status			,	0	} ,
	{ "PM5.Wafer_Status"		, _K_D_IO	,	MPM5_Wafer_Status			,	0	} ,
//	{ "PM6.Wafer_Status"		, _K_D_IO	,	MPM6_Wafer_Status			,	0	} ,

	{ "PM1.ForeDRY1_ATM_Status"	, _K_D_IO	,	MPM1_ForeDRY1_ATM_Status	,	0	} ,
	{ "PM2.ForeDRY1_ATM_Status"	, _K_D_IO	,	MPM2_ForeDRY1_ATM_Status	,	0	} ,
	{ "PM3.ForeDRY1_ATM_Status"	, _K_D_IO	,	MPM3_ForeDRY1_ATM_Status	,	0	} ,
	{ "PM4.ForeDRY1_ATM_Status"	, _K_D_IO	,	MPM4_ForeDRY1_ATM_Status	,	0	} ,
	{ "PM5.ForeDRY1_ATM_Status"	, _K_D_IO	,	MPM5_ForeDRY1_ATM_Status	,	0	} ,
//	{ "PM6.ForeDRY1_ATM_Status"	, _K_D_IO	,	MPM6_ForeDRY1_ATM_Status	,	0	} ,

	{ "PM1.RoughDRY2_ATM_Status", _K_D_IO	,	MPM1_RoughDRY2_ATM_Status	,	0	} ,
	{ "PM2.RoughDRY2_ATM_Status", _K_D_IO	,	MPM2_RoughDRY2_ATM_Status	,	0	} ,
	{ "PM3.RoughDRY2_ATM_Status", _K_D_IO	,	MPM3_RoughDRY2_ATM_Status	,	0	} ,
	{ "PM4.RoughDRY2_ATM_Status", _K_D_IO	,	MPM4_RoughDRY2_ATM_Status	,	0	} ,
	{ "PM5.RoughDRY2_ATM_Status", _K_D_IO	,	MPM5_RoughDRY2_ATM_Status	,	0	} ,
//	{ "PM6.RoughDRY2_ATM_Status", _K_D_IO	,	MPM6_RoughDRY2_ATM_Status	,	0	} ,

	{ "PM1.ForeDRY1_Pressure"	, _K_A_IO	,	MPM1_ForeDRY1_Pressure		,	0	} ,
	{ "PM2.ForeDRY1_Pressure"	, _K_A_IO	,	MPM2_ForeDRY1_Pressure		,	0	} ,
	{ "PM3.ForeDRY1_Pressure"	, _K_A_IO	,	MPM3_ForeDRY1_Pressure		,	0	} ,
	{ "PM4.ForeDRY1_Pressure"	, _K_A_IO	,	MPM4_ForeDRY1_Pressure		,	0	} ,
	{ "PM5.ForeDRY1_Pressure"	, _K_A_IO	,	MPM5_ForeDRY1_Pressure		,	0	} ,
//	{ "PM6.ForeDRY1_Pressure"	, _K_A_IO	,	MPM6_ForeDRY1_Pressure		,	0	} ,

	{ "PM1.RoughDRY2_Pressure"	, _K_A_IO	,	MPM1_RoughDRY2_Pressure		,	0	} ,
	{ "PM2.RoughDRY2_Pressure"	, _K_A_IO	,	MPM2_RoughDRY2_Pressure		,	0	} ,
	{ "PM3.RoughDRY2_Pressure"	, _K_A_IO	,	MPM3_RoughDRY2_Pressure		,	0	} ,
	{ "PM4.RoughDRY2_Pressure"	, _K_A_IO	,	MPM4_RoughDRY2_Pressure		,	0	} ,
	{ "PM5.RoughDRY2_Pressure"	, _K_A_IO	,	MPM5_RoughDRY2_Pressure		,	0	} ,
//	{ "PM6.RoughDRY2_Pressure"	, _K_A_IO	,	MPM6_RoughDRY2_Pressure		,	0	} ,

	{ "PM1.ForeDRY1Pump_Status"	, _K_D_IO	,	MPM1_ForeDRY1Pump_Status	,	0	} ,
	{ "PM2.ForeDRY1Pump_Status"	, _K_D_IO	,	MPM2_ForeDRY1Pump_Status	,	0	} ,
	{ "PM3.ForeDRY1Pump_Status"	, _K_D_IO	,	MPM3_ForeDRY1Pump_Status	,	0	} ,
	{ "PM4.ForeDRY1Pump_Status"	, _K_D_IO	,	MPM4_ForeDRY1Pump_Status	,	0	} ,
	{ "PM5.ForeDRY1Pump_Status"	, _K_D_IO	,	MPM5_ForeDRY1Pump_Status	,	0	} ,
//	{ "PM6.ForeDRY1Pump_Status"	, _K_D_IO	,	MPM6_ForeDRY1Pump_Status	,	0	} ,

	{ "PM1.RoughDRY2Pump_Status", _K_D_IO	,	MPM1_RoughDRY2Pump_Status	,	0	} ,
	{ "PM2.RoughDRY2Pump_Status", _K_D_IO	,	MPM2_RoughDRY2Pump_Status	,	0	} ,
	{ "PM3.RoughDRY2Pump_Status", _K_D_IO	,	MPM3_RoughDRY2Pump_Status	,	0	} ,
	{ "PM4.RoughDRY2Pump_Status", _K_D_IO	,	MPM4_RoughDRY2Pump_Status	,	0	} ,
	{ "PM5.RoughDRY2Pump_Status", _K_D_IO	,	MPM5_RoughDRY2Pump_Status	,	0	} ,
//	{ "PM6.RoughDRY2Pump_Status", _K_D_IO	,	MPM6_RoughDRY2Pump_Status	,	0	} ,

	{ "CTC.RUN_CONTROL"			, _K_D_IO	,	RUN_CONTROL					,	0	} ,

	{ "CTC.MAIN_CONTROL"		, _K_D_IO	,	MAIN_CONTROL				,	0	} ,
	{ "CTC.MAIN_CONTROL2"		, _K_D_IO	,	MAIN_CONTROL2				,	0	} ,

	{ "PM1.Rotaion_Speed"		, _K_A_IO	,	PM1_Rotaion_Speed			,	0	} ,
	{ "PM2.Rotaion_Speed"		, _K_A_IO	,	PM2_Rotaion_Speed			,	0	} ,
	{ "PM3.Rotaion_Speed"		, _K_A_IO	,	PM3_Rotaion_Speed			,	0	} ,
	{ "PM4.Rotaion_Speed"		, _K_A_IO	,	PM4_Rotaion_Speed			,	0	} ,
	{ "PM5.Rotaion_Speed"		, _K_A_IO	,	PM5_Rotaion_Speed			,	0	} ,
//	{ "PM6.Rotaion_Speed"		, _K_A_IO	,	PM6_Rotaion_Speed			,	0	} ,

	{ "SCHEDULER_MAINT_PM1"		, _K_F_IO	,	SCHEDULER_MAINT_PM1			,	0	} ,
	{ "SCHEDULER_MAINT_PM2"		, _K_F_IO	,	SCHEDULER_MAINT_PM2			,	0	} ,
	{ "SCHEDULER_MAINT_PM3"		, _K_F_IO	,	SCHEDULER_MAINT_PM3			,	0	} ,
	{ "SCHEDULER_MAINT_PM4"		, _K_F_IO	,	SCHEDULER_MAINT_PM4			,	0	} ,
	{ "SCHEDULER_MAINT_PM5"		, _K_F_IO	,	SCHEDULER_MAINT_PM5			,	0	} ,
//	{ "SCHEDULER_MAINT_PM6"		, _K_F_IO	,	SCHEDULER_MAINT_PM6			,	0	} ,

	{ "PRM_HEATEROFF_TIME"		, _K_D_IO	,	PRM_HEATEROFF_TIME			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int CommStatus , i , Port1, Port2 , Run;
	BOOL ROTSTOP = FALSE;
	unsigned long TimeCount = 0 , Time = 0;
	while ( TRUE ) {
		_sleep( 100 );
		for ( i = 0 ; i < 5 ; i++ ) {
			WRITE_DIGITAL( MPM1_Gate_Status			+ i , READ_DIGITAL( PM1_Gate_Status  + i, &CommStatus ) , &CommStatus );
			WRITE_DIGITAL( MPM1_Wafer_Status		+ i , READ_DIGITAL( PM1_Wafer_Status + i, &CommStatus ) , &CommStatus );
			WRITE_DIGITAL( MPM1_ForeDRY1_ATM_Status + i , READ_DIGITAL( ATM_Status_ForeDry  , &CommStatus ) , &CommStatus );
			WRITE_DIGITAL( MPM1_RoughDRY2_ATM_Status+ i , READ_DIGITAL( ATM_Status_RoughDry , &CommStatus ) , &CommStatus );
			WRITE_ANALOG ( MPM1_ForeDRY1_Pressure	+ i , READ_ANALOG ( TM_ForeCONPressure  , &CommStatus ) , &CommStatus );
			WRITE_ANALOG ( MPM1_RoughDRY2_Pressure	+ i , READ_ANALOG ( TM_RoughCONPressure , &CommStatus ) , &CommStatus );
			WRITE_DIGITAL( MPM1_ForeDRY1Pump_Status	+ i , READ_DIGITAL( ForeDRP_Status		, &CommStatus ) , &CommStatus );
			WRITE_DIGITAL( MPM1_RoughDRY2Pump_Status+ i , READ_DIGITAL( RoughDRP_Status		, &CommStatus ) , &CommStatus );
		}

		// when ready part have a error, rotation stop
		Port1 = READ_DIGITAL( MAIN_CONTROL  , &CommStatus );
		Port2 = READ_DIGITAL( MAIN_CONTROL2 , &CommStatus );
		if ( ROTSTOP && ( Port1 == 0 || Port1 == 7 ) && ( Port2 == 0 || Port2 == 7 ) ) { // 0 = idle, 7 = Wait_HandOff 
			ROTSTOP = FALSE;
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( READ_ANALOG( PM1_Rotaion_Speed+i , &CommStatus ) > 0 ) RUN_SET_FUNCTION_FREE( SCHEDULER_MAINT_PM1+i , "ROTSTOP_CHK" );
			}
		}
		if ( ROTSTOP == FALSE && ( Port1 == 1 || Port1 == 6 || Port2 == 1 || Port2 == 6 ) ) ROTSTOP = TRUE; // 1 = running, 6 = waiting
		else ROTSTOP = FALSE;

		// Heater Off
		Run = READ_DIGITAL( RUN_CONTROL , &CommStatus );
//		if ( ( Port1 == 0 || Port1 == 7 ) && ( Port2 == 0 || Port2 == 7 ) ) { // 0 = idle, 7 = Wait_HandOff 
//		if ( Run == 0 ) { // 0 = idle
//			Time = READ_DIGITAL( PRM_HEATEROFF_TIME , &CommStatus )*3600;
//			if ( (int)(TimeCount/10) > Time ) {
//				TimeCount = 0;
//				for ( i = 0 ; i < 6 ; i++ ) {
//					RUN_SET_FUNCTION_FREE( SCHEDULER_MAINT_PM1+i , "HEATERALLOFF" );
//				}
//			}
//			else TimeCount++;
//		}
//		else TimeCount = 0;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
