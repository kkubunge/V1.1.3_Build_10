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
	{ "IO_Driver_Loding"				, _K_D_IO	,	IO_Driver_Loding			,	0	} ,

    { "Bottom_System"					, _K_D_IO	,	Bottom_Operate				,	0	} ,

	{ "Bottom_Config"					, _K_D_IO	,	Bottom_Config				,	0	} ,

	{ "Bottom_Service1"					, _K_D_IO	,	Bottom_Maint1				,	0	} ,
	{ "Bottom_Service2"					, _K_D_IO	,	Bottom_Maint2				,	0	} ,

	{ "Right_Config_CMC"				, _K_D_IO	,	Config_CMC					,	0	} ,
	{ "Right_Config_TMC"				, _K_D_IO	,	Config_TMC					,	0	} ,
	{ "Right_Config_TMC_Nav"			, _K_D_IO	,	Config_TMC_Nav				,	0	} ,
	{ "Right_Config_PM1"				, _K_D_IO	,	Config_PM1					,	0	} ,
	{ "Right_Config_PM2"				, _K_D_IO	,	Config_PM2					,	0	} ,
	{ "Right_Config_PM3"				, _K_D_IO	,	Config_PM3					,	0	} ,
	{ "Right_Config_PM4"				, _K_D_IO	,	Config_PM4					,	0	} ,
	{ "Right_Config_PM5"				, _K_D_IO	,	Config_PM5					,	0	} ,

	{ "Right_Utility_Prev"				, _K_D_IO	,	Right_Utility_Prev			,	0	} ,
	{ "Right_Utility_Prev_Nav"			, _K_D_IO	,	Right_Utility_Prev_Nav		,	0	} ,
	{ "Right_Utility_Prev2"				, _K_D_IO	,	Right_Utility_Prev2			,	0	} ,
	{ "Right_Utility_Prev2_Nav"			, _K_D_IO	,	Right_Utility_Prev2_Nav		,	0	} ,
	{ "Right_Utility_Prev3"				, _K_D_IO	,	Right_Utility_Prev3			,	0	} ,
	{ "Right_Utility_Prev3_Nav"			, _K_D_IO	,	Right_Utility_Prev3_Nav		,	0	} ,

	{ "CTC.PM1_PREVENTIVE_NEXT"			, _K_D_IO	,	PM1_PREVENTIVE_NEXT			,	0	} ,
	{ "CTC.PM2_PREVENTIVE_NEXT"			, _K_D_IO	,	PM2_PREVENTIVE_NEXT			,	0	} ,
	{ "CTC.PM3_PREVENTIVE_NEXT"			, _K_D_IO	,	PM3_PREVENTIVE_NEXT			,	0	} ,
	{ "CTC.PM4_PREVENTIVE_NEXT"			, _K_D_IO	,	PM4_PREVENTIVE_NEXT			,	0	} ,
	{ "CTC.PM5_PREVENTIVE_NEXT"			, _K_D_IO	,	PM5_PREVENTIVE_NEXT			,	0	} ,
	
	{ "CTC.PM1_PREVENTIVE_PREV"			, _K_D_IO	,	PM1_PREVENTIVE_PREV			,	0	} ,
	{ "CTC.PM2_PREVENTIVE_PREV"			, _K_D_IO	,	PM2_PREVENTIVE_PREV			,	0	} ,
	{ "CTC.PM3_PREVENTIVE_PREV"			, _K_D_IO	,	PM3_PREVENTIVE_PREV			,	0	} ,
	{ "CTC.PM4_PREVENTIVE_PREV"			, _K_D_IO	,	PM4_PREVENTIVE_PREV			,	0	} ,
	{ "CTC.PM5_PREVENTIVE_PREV"			, _K_D_IO	,	PM5_PREVENTIVE_PREV			,	0	} ,

	{ "TM_Config_Next"					, _K_D_IO	,	TM_Config_Next				,	0	} ,
	{ "TM_Config_Prev"					, _K_D_IO	,	TM_Config_Prev				,	0	} ,

	{ "LotPre_Config_Next"				, _K_D_IO	,	LotPre_Config_Next			,	0	} ,
	{ "LotPre_Config_Prev"				, _K_D_IO	,	LotPre_Config_Prev			,	0	} ,
	{ "Right_System_LotPre"				, _K_D_IO	,	Config_LotPre				,	0	} ,
	{ "Right_System_LotPre_Nav"			, _K_D_IO	,	Config_LotPre_Nav			,	0	} ,

	{ "Right_Service2_PM_Heater1"		, _K_D_IO	,	Right_Service2_PM_Heater1	,	0	} ,
	{ "Right_Service2_PM_Ht1_Nav"	, _K_D_IO	,	Right_Service2_PM_Heater1_Nav	,	0	} ,
	{ "Right_Service2_PM_Heater2"		, _K_D_IO	,	Right_Service2_PM_Heater2	,	0	} ,
	{ "Right_Service2_PM_Ht2_Nav"	, _K_D_IO	,	Right_Service2_PM_Heater2_Nav	,	0	} ,
	{ "Right_Service2_PM_Heater3"		, _K_D_IO	,	Right_Service2_PM_Heater3	,	0	} ,
	{ "Right_Service2_PM_Ht3_Nav"	, _K_D_IO	,	Right_Service2_PM_Heater3_Nav	,	0	} ,
	{ "Right_Service2_PM_Heater4"		, _K_D_IO	,	Right_Service2_PM_Heater4	,	0	} ,
	{ "Right_Service2_PM_Ht4_Nav"	, _K_D_IO	,	Right_Service2_PM_Heater4_Nav	,	0	} ,
	{ "Right_Service2_PM_Heater5"		, _K_D_IO	,	Right_Service2_PM_Heater5	,	0	} ,
	{ "Right_Service2_PM_Ht5_Nav"	, _K_D_IO	,	Right_Service2_PM_Heater5_Nav	,	0	} ,

	{ "PM1.Heater_Next"					, _K_D_IO	,	PM1_Heater_Next				,	0	} ,
	{ "PM2.Heater_Next"					, _K_D_IO	,	PM2_Heater_Next				,	0	} ,
	{ "PM3.Heater_Next"					, _K_D_IO	,	PM3_Heater_Next				,	0	} ,
	{ "PM4.Heater_Next"					, _K_D_IO	,	PM4_Heater_Next				,	0	} ,
	{ "PM5.Heater_Next"					, _K_D_IO	,	PM5_Heater_Next				,	0	} ,

	{ "PM1.Heater_Prev"					, _K_D_IO	,	PM1_Heater_Prev				,	0	} ,
	{ "PM2.Heater_Prev"					, _K_D_IO	,	PM2_Heater_Prev				,	0	} ,
	{ "PM3.Heater_Prev"					, _K_D_IO	,	PM3_Heater_Prev				,	0	} ,
	{ "PM4.Heater_Prev"					, _K_D_IO	,	PM4_Heater_Prev				,	0	} ,
	{ "PM5.Heater_Prev"					, _K_D_IO	,	PM5_Heater_Prev				,	0	} ,

	{ "Right_Config_PM1_Cfg1_Nav"    , _K_D_IO	,	PM1_GasFlow_Config_Nav		,	0	} ,
	{ "PM1.Param_Next"					, _K_D_IO	,	PM1_GasFlow_Config_Next		,	0	} ,
	{ "PM1.Param_Prev"					, _K_D_IO	,	PM1_GasFlow_Config_Prev		,	0	} ,

    { "Right_Config_PM1_Cfg2_Nav"    , _K_D_IO	,	PM1_Moving_Config_Nav		,	0	} ,
	{ "PM1.Config2_Next"				, _K_D_IO	,	PM1_Moving_Config_Next		,	0	} ,
	{ "PM1.Config2_Prev"				, _K_D_IO	,	PM1_Moving_Config_Prev		,	0	} ,

	{ "Right_Config_PM2_Cfg1_Nav"    , _K_D_IO	,	PM2_GasFlow_Config_Nav		,	0	} ,
	{ "PM2.Param_Next"					, _K_D_IO	,	PM2_GasFlow_Config_Next		,	0	} ,
	{ "PM2.Param_Prev"					, _K_D_IO	,	PM2_GasFlow_Config_Prev		,	0	} ,

    { "Right_Config_PM2_Cfg2_Nav"    , _K_D_IO	,	PM2_Moving_Config_Nav		,	0	} ,
	{ "PM2.Config2_Next"				, _K_D_IO	,	PM2_Moving_Config_Next		,	0	} ,
	{ "PM2.Config2_Prev"				, _K_D_IO	,	PM2_Moving_Config_Prev		,	0	} ,
	
    { "Right_Config_PM3_Cfg1_Nav"    , _K_D_IO	,	PM3_GasFlow_Config_Nav		,	0	} ,
	{ "PM3.Param_Next"					, _K_D_IO	,	PM3_GasFlow_Config_Next		,	0	} ,
	{ "PM3.Param_Prev"					, _K_D_IO	,	PM3_GasFlow_Config_Prev		,	0	} ,

    { "Right_Config_PM3_Cfg2_Nav"    , _K_D_IO	,	PM3_Moving_Config_Nav		,	0	} ,
	{ "PM3.Config2_Next"				, _K_D_IO	,	PM3_Moving_Config_Next		,	0	} ,
	{ "PM3.Config2_Prev"				, _K_D_IO	,	PM3_Moving_Config_Prev		,	0	} ,

    { "Right_Config_PM4_Cfg1_Nav"    , _K_D_IO	,	PM4_GasFlow_Config_Nav		,	0	} ,
	{ "PM4.Param_Next"					, _K_D_IO	,	PM4_GasFlow_Config_Next		,	0	} ,
	{ "PM4.Param_Prev"					, _K_D_IO	,	PM4_GasFlow_Config_Prev		,	0	} ,

    { "Right_Config_PM4_Cfg2_Nav"    , _K_D_IO	,	PM4_Moving_Config_Nav		,	0	} ,
	{ "PM4.Config2_Next"				, _K_D_IO	,	PM4_Moving_Config_Next		,	0	} ,
	{ "PM4.Config2_Prev"				, _K_D_IO	,	PM4_Moving_Config_Prev		,	0	} ,

    { "Right_Config_PM5_Cfg1_Nav"    , _K_D_IO	,	PM5_GasFlow_Config_Nav		,	0	} ,
	{ "PM5.Param_Next"					, _K_D_IO	,	PM5_GasFlow_Config_Next		,	0	} ,
	{ "PM5.Param_Prev"					, _K_D_IO	,	PM5_GasFlow_Config_Prev		,	0	} ,

    { "Right_Config_PM5_Cfg2_Nav"    , _K_D_IO	,	PM5_Moving_Config_Nav		,	0	} ,
	{ "PM5.Config2_Next"				, _K_D_IO	,	PM5_Moving_Config_Next		,	0	} ,
	{ "PM5.Config2_Prev"				, _K_D_IO	,	PM5_Moving_Config_Prev		,	0	} ,

    { "Maint_TMC_Status"				, _K_D_IO	,	TMC_Status					,	0	} ,
    { "Maint_PM1_Status"				, _K_D_IO	,	PM1_Status					,	0	} ,
	{ "Maint_PM2_Status"				, _K_D_IO	,	PM2_Status					,	0	} ,
    { "Maint_PM3_Status"				, _K_D_IO	,	PM3_Status					,	0	} ,
    { "Maint_PM4_Status"				, _K_D_IO	,	PM4_Status					,	0	} ,
    { "Maint_PM5_Status"				, _K_D_IO	,	PM5_Status					,	0	} ,
    { "Maint_EFEM_Status"				, _K_D_IO	,	EFEM_Status					,	0	} ,

    { "Right_Operate_Status"	    	, _K_D_IO	,	Operate_Status   			,	0	} ,
    { "Right_Operate_Status_Nav"	   	, _K_D_IO	,	Operate_Status_Nav   		,	0	} ,

    { "Move_To_Maint_TMC"				, _K_D_IO	,	Move_To_Maint_TMC		 	,	0	} ,
    { "Move_To_Status_TMC"				, _K_D_IO	,	Move_To_Status_TMC			,	0	} ,
    { "Move_To_Maint_PM1"				, _K_D_IO	,	Move_To_Maint_PM1			,	0	} ,
    { "Move_To_Status_PM1"				, _K_D_IO	,	Move_To_Status_PM1			,	0	} ,
	{ "Move_To_Maint_PM2"				, _K_D_IO	,	Move_To_Maint_PM2			,	0	} ,
    { "Move_To_Status_PM2"				, _K_D_IO	,	Move_To_Status_PM2			,	0	} ,
    { "Move_To_Maint_PM3"				, _K_D_IO	,	Move_To_Maint_PM3			,	0	} ,
    { "Move_To_Status_PM3"				, _K_D_IO	,	Move_To_Status_PM3			,	0	} ,
    { "Move_To_Maint_PM4"				, _K_D_IO	,	Move_To_Maint_PM4			,	0	} ,
    { "Move_To_Status_PM4"				, _K_D_IO	,	Move_To_Status_PM4			,	0	} ,
    { "Move_To_Maint_PM5"				, _K_D_IO	,	Move_To_Maint_PM5			,	0	} ,
    { "Move_To_Status_PM5"				, _K_D_IO	,	Move_To_Status_PM5			,	0	} ,

	//kcr
	{ "Right_Service1_TMC_Vacuum"		, _K_D_IO	,	Right_Service1_TMC_Vacuum	,	0	} ,	//TMC Module
	{ "Right_Service1_TMC_FEM"			, _K_D_IO	,	Right_Service1_TMC_FEM		,	0	} ,	//CM/FM Module
	{ "Right_Service2_PM_Main"			, _K_D_IO	,	Right_Service2_PM_Main		,	0	} ,	//PM Module
	{ "Right_Service2_PM_M_Nav"			, _K_D_IO	,	Right_Service2_PM_Main_Nav		,	0	} ,	//PM Nav
	

    { "Navi_TM"							, _K_D_IO	,	Navi_TM						,	0	} ,
    { "Navi_PM1"						, _K_D_IO	,	Navi_PM1					,	0	} ,
    { "Navi_PM2"						, _K_D_IO	,	Navi_PM2					,	0	} ,
    { "Navi_PM3"						, _K_D_IO	,	Navi_PM3					,	0	} ,
	{ "Navi_PM4"						, _K_D_IO	,	Navi_PM4					,	0	} ,
	{ "Navi_PM5"						, _K_D_IO	,	Navi_PM5					,	0	} ,
    { "Navi_CM1"						, _K_D_IO	,	Navi_CM1					,	0	} ,
    { "Navi_CM2"						, _K_D_IO	,	Navi_CM2					,	0	} ,
    { "Navi_CM3"						, _K_D_IO	,	Navi_CM3					,	0	} ,
    { "Navi_FEM"						, _K_D_IO	,	Navi_FEM					,	0	} ,
	{ "Right_Direct_Navigation"			, _K_D_IO	,	Right_Direct_Navigation		,	0	} ,	

    { "Service_Old"						, _K_D_IO	,	Service_Old					,	0	} ,	//0:Service1, 1:Service2
	
	""
};


//------------------------------------------------------------------------------------------
void LotPreConfig_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 100 );

		if( READ_DIGITAL( LotPre_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( Config_LotPre_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 7 ) Para = 1;
			WRITE_DIGITAL( LotPre_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( Config_LotPre_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( LotPre_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( Config_LotPre_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 6;
			WRITE_DIGITAL( LotPre_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( Config_LotPre_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void TMConfig_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;


	while ( TRUE ) {
		_sleep( 50 );

		if( READ_DIGITAL( TM_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( Config_TMC_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 7 ) Para = 1;
			WRITE_DIGITAL( TM_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( Config_TMC_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( TM_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( Config_TMC_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 7;
			WRITE_DIGITAL( TM_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( Config_TMC_Nav , Para , &CommStatus );
		}
		// KCR Service1/Service2 의 마지막 사용버튼 기억
		if( READ_DIGITAL( Bottom_Maint1 , &CommStatus ) == 1 ) WRITE_DIGITAL( Service_Old , 0 , &CommStatus );
		if( READ_DIGITAL( Bottom_Maint2 , &CommStatus ) == 1 ) WRITE_DIGITAL( Service_Old , 1 , &CommStatus );

	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
void PM_Heater_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( (READ_DIGITAL(PM2_Heater_Next,&CommStatus)==1) || (READ_DIGITAL(PM3_Heater_Next,&CommStatus)==1)
			 || (READ_DIGITAL(PM4_Heater_Next,&CommStatus)==1) || (READ_DIGITAL(PM5_Heater_Next,&CommStatus)==1) ){
			WRITE_DIGITAL( Right_Service2_PM_Heater2 , 1 , &CommStatus );
			WRITE_DIGITAL( PM2_Heater_Next , 0    , &CommStatus );
		}
		if( (READ_DIGITAL(PM2_Heater_Prev,&CommStatus)==1) || (READ_DIGITAL(PM3_Heater_Prev,&CommStatus)==1)
			 || (READ_DIGITAL(PM4_Heater_Prev,&CommStatus)==1) || (READ_DIGITAL(PM5_Heater_Prev,&CommStatus)==1) ){
			WRITE_DIGITAL( Right_Service2_PM_Heater1 , 1 , &CommStatus );
			WRITE_DIGITAL( PM2_Heater_Prev , 0    , &CommStatus );
		}
	}
	_endthread();
}
*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM_Heater_Control() {
	int CommStatus , i;

	while ( TRUE ) {
		_sleep( 50 );
		for ( i=0 ; i<5 ; i++ ) {
			if ( READ_DIGITAL( Right_Service2_PM_Heater1_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_Heater_Next+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater2 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater2_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_Heater_Next+i , 0 , &CommStatus );
				}
			}
			if ( READ_DIGITAL( Right_Service2_PM_Heater2_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_Heater_Prev+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater1 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater1_Nav , i+1 , &CommStatus );	//kcr
					WRITE_DIGITAL( PM1_Heater_Prev+i , 0 , &CommStatus );			
				}
				
				if( READ_DIGITAL( PM1_Heater_Next+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater3 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater3_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_Heater_Next+i , 0 , &CommStatus );
				}
			}
			if ( READ_DIGITAL( Right_Service2_PM_Heater3_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_Heater_Prev+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater2 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater2_Nav , i+1 , &CommStatus );	//kcr
					WRITE_DIGITAL( PM1_Heater_Prev+i , 0 , &CommStatus );			
				}

				if( READ_DIGITAL( PM1_Heater_Next+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater4 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater4_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_Heater_Next+i , 0 , &CommStatus );
				}
			}
			if ( READ_DIGITAL( Right_Service2_PM_Heater4_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_Heater_Prev+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater3 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater3_Nav , i+1 , &CommStatus );	//kcr
					WRITE_DIGITAL( PM1_Heater_Prev+i , 0 , &CommStatus );			
				}

				if( READ_DIGITAL( PM1_Heater_Next+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater5 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater5_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_Heater_Next+i , 0 , &CommStatus );
				}
			}

			if ( READ_DIGITAL( Right_Service2_PM_Heater5_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_Heater_Prev+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Service2_PM_Heater4 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Service2_PM_Heater4_Nav , i+1 , &CommStatus );	//kcr
					WRITE_DIGITAL( PM1_Heater_Prev+i , 0 , &CommStatus );			
				}
			}
			
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Preventive_Maintenance_Control() {
	int CommStatus , i;

	while ( TRUE ) {
		_sleep( 50 );
		for ( i=0 ; i<=4 ; i++ ) {
			if ( READ_DIGITAL( Right_Utility_Prev_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_PREVENTIVE_NEXT+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Utility_Prev2 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Utility_Prev2_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_PREVENTIVE_NEXT+i , 0 , &CommStatus );
				}
				
				if( READ_DIGITAL( PM1_PREVENTIVE_PREV+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Utility_Prev3 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Utility_Prev3_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_PREVENTIVE_PREV+i , 0 , &CommStatus );
				}
			}

			if ( READ_DIGITAL( Right_Utility_Prev2_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_PREVENTIVE_PREV+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Utility_Prev , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Utility_Prev_Nav , i+1 , &CommStatus );	//kcr
					WRITE_DIGITAL( PM1_PREVENTIVE_PREV+i , 0 , &CommStatus );
				}
				
				if( READ_DIGITAL( PM1_PREVENTIVE_NEXT+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Utility_Prev3 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Utility_Prev3_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_PREVENTIVE_NEXT+i , 0 , &CommStatus );
				}
			}
			
			if ( READ_DIGITAL( Right_Utility_Prev3_Nav , &CommStatus ) == i+1 ) {
				if( READ_DIGITAL( PM1_PREVENTIVE_PREV+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Utility_Prev2 , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Utility_Prev2_Nav , i+1 , &CommStatus );	//kcr
					WRITE_DIGITAL( PM1_PREVENTIVE_PREV+i , 0 , &CommStatus );
				}
				
				if( READ_DIGITAL( PM1_PREVENTIVE_NEXT+i , &CommStatus ) == 1 ) {
					WRITE_DIGITAL( Right_Utility_Prev , 1 , &CommStatus );
					WRITE_DIGITAL( Right_Utility_Prev_Nav , i+1 , &CommStatus );  //kcr
					WRITE_DIGITAL( PM1_PREVENTIVE_NEXT+i , 0 , &CommStatus );
				}
			}
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM1_Heater_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM1_Heater_Next , &CommStatus ) == 1 ) {
			printf("F\n");
			WRITE_DIGITAL( Right_Service2_PM_Heater2 , 1 , &CommStatus );
			WRITE_DIGITAL( PM1_Heater_Next , 0    , &CommStatus );
//			WRITE_DIGITAL( PM2_Heater_Prev , 1    , &CommStatus );
		}
		if( READ_DIGITAL( PM1_Heater_Prev , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Right_Service2_PM_Heater1 , 1 , &CommStatus );
			WRITE_DIGITAL( PM1_Heater_Prev , 0    , &CommStatus );
//			WRITE_DIGITAL( PM2_Heater_Next , 1    , &CommStatus );
		}
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM1_Gas_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM1_GasFlow_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM1_GasFlow_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 6 ) Para = 1;
			WRITE_DIGITAL( PM1_GasFlow_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM1_GasFlow_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM1_GasFlow_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM1_GasFlow_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 5;
			WRITE_DIGITAL( PM1_GasFlow_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM1_GasFlow_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM1_Moving_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM1_Moving_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM1_Moving_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 5 ) Para = 1;
			WRITE_DIGITAL( PM1_Moving_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM1_Moving_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM1_Moving_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM1_Moving_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 2;
			WRITE_DIGITAL( PM1_Moving_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM1_Moving_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void PM2_Heater_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM2_Heater_Next , &CommStatus ) == 1 ) {
			printf("F\n");
			WRITE_DIGITAL( Right_Service2_PM_Heater2 , 1 , &CommStatus );
			WRITE_DIGITAL( PM2_Heater_Next , 0    , &CommStatus );
//			WRITE_DIGITAL( PM2_Heater_Prev , 1    , &CommStatus );
		}
		if( READ_DIGITAL( PM2_Heater_Prev , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Right_Service2_PM_Heater1 , 1 , &CommStatus );
			WRITE_DIGITAL( PM2_Heater_Prev , 0    , &CommStatus );
//			WRITE_DIGITAL( PM2_Heater_Next , 1    , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM2_Gas_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM2_GasFlow_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM2_GasFlow_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 6 ) Para = 1;
			WRITE_DIGITAL( PM2_GasFlow_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM2_GasFlow_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM2_GasFlow_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM2_GasFlow_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 6;
			WRITE_DIGITAL( PM2_GasFlow_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM2_GasFlow_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM2_Moving_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM2_Moving_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM2_Moving_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 6 ) Para = 1;
			WRITE_DIGITAL( PM2_Moving_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM2_Moving_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM2_Moving_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM2_Moving_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 6;
			WRITE_DIGITAL( PM2_Moving_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM2_Moving_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}




//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM3_Gas_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM3_GasFlow_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM3_GasFlow_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 6 ) Para = 1;
			WRITE_DIGITAL( PM3_GasFlow_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM3_GasFlow_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM3_GasFlow_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM3_GasFlow_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 6;
			WRITE_DIGITAL( PM3_GasFlow_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM3_GasFlow_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM3_Moving_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM3_Moving_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM3_Moving_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 6 ) Para = 1;
			WRITE_DIGITAL( PM3_Moving_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM3_Moving_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM3_Moving_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM3_Moving_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 6;
			WRITE_DIGITAL( PM3_Moving_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM3_Moving_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM4_Gas_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM4_GasFlow_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM4_GasFlow_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 4 ) Para = 1;
			WRITE_DIGITAL( PM4_GasFlow_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM4_GasFlow_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM4_GasFlow_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM4_GasFlow_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 3;
			WRITE_DIGITAL( PM4_GasFlow_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM4_GasFlow_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM4_Moving_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM4_Moving_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM4_Moving_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 3 ) Para = 1;
			WRITE_DIGITAL( PM4_Moving_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM4_Moving_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM4_Moving_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM4_Moving_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 2;
			WRITE_DIGITAL( PM4_Moving_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM4_Moving_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM5_Gas_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM5_GasFlow_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM5_GasFlow_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 4 ) Para = 1;
			WRITE_DIGITAL( PM5_GasFlow_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM5_GasFlow_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM5_GasFlow_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM5_GasFlow_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 3;
			WRITE_DIGITAL( PM5_GasFlow_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM5_GasFlow_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM5_Moving_Config_Select_Control() {
	int CommStatus , Next=0 , Prev=0 , Para=0;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( PM5_Moving_Config_Next , &CommStatus ) == 1 ) {
			Next = READ_DIGITAL( PM5_Moving_Config_Nav , &CommStatus );
			Para = Next+1;
			if( Para == 3 ) Para = 1;
			WRITE_DIGITAL( PM5_Moving_Config_Next , 0    , &CommStatus );
			WRITE_DIGITAL( PM5_Moving_Config_Nav , Para , &CommStatus );
		}
		if( READ_DIGITAL( PM5_Moving_Config_Prev , &CommStatus ) == 1 ) {
			Prev = READ_DIGITAL( PM5_Moving_Config_Nav , &CommStatus );
			Para = Prev-1;
			if( Para == 0 ) Para = 2;
			WRITE_DIGITAL( PM5_Moving_Config_Prev , 0    , &CommStatus );
			WRITE_DIGITAL( PM5_Moving_Config_Nav , Para , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Module_Select_Navigation_Control() {
	int CommStatus;

	while ( TRUE ) {
		_sleep( 50 );
		if( READ_DIGITAL( Navi_TM , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Bottom_Maint1 , 1    , &CommStatus );
			WRITE_DIGITAL( Right_Service1_TMC_Vacuum , 1 , &CommStatus );
			WRITE_DIGITAL( Navi_TM , 0    , &CommStatus );
		}
		if( READ_DIGITAL( Navi_FEM , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Bottom_Maint1 , 1    , &CommStatus );
			WRITE_DIGITAL( Right_Service1_TMC_FEM , 1 , &CommStatus );
			WRITE_DIGITAL( Navi_FEM , 0    , &CommStatus );
		}

		if( READ_DIGITAL( Navi_PM1 , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Bottom_Maint2 , 1    , &CommStatus );
			WRITE_DIGITAL( Right_Service2_PM_Main , 1 , &CommStatus );
			WRITE_DIGITAL( Right_Direct_Navigation , 1 , &CommStatus );
			WRITE_DIGITAL( Navi_PM1 , 0    , &CommStatus );
		}

		if( READ_DIGITAL( Navi_PM2 , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Bottom_Maint2 , 1    , &CommStatus );
			WRITE_DIGITAL( Right_Service2_PM_Main , 1 , &CommStatus );
			WRITE_DIGITAL( Right_Direct_Navigation , 2 , &CommStatus );
			WRITE_DIGITAL( Navi_PM2 , 0    , &CommStatus );
		}
		if( READ_DIGITAL( Navi_PM3 , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Bottom_Maint2 , 1    , &CommStatus );
			WRITE_DIGITAL( Right_Service2_PM_Main , 1 , &CommStatus );
			WRITE_DIGITAL( Right_Direct_Navigation , 3 , &CommStatus );
			WRITE_DIGITAL( Navi_PM3 , 0    , &CommStatus );
		}
		if( READ_DIGITAL( Navi_PM4 , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Bottom_Maint2 , 1    , &CommStatus );
			WRITE_DIGITAL( Right_Service2_PM_Main , 1 , &CommStatus );
			WRITE_DIGITAL( Right_Direct_Navigation , 4 , &CommStatus );
			WRITE_DIGITAL( Navi_PM4 , 0    , &CommStatus );
		}
		if( READ_DIGITAL( Navi_PM5 , &CommStatus ) == 1 ) {
			WRITE_DIGITAL( Bottom_Maint2 , 1    , &CommStatus );
			WRITE_DIGITAL( Right_Service2_PM_Main , 1 , &CommStatus );
			WRITE_DIGITAL( Right_Direct_Navigation , 5 , &CommStatus );
			WRITE_DIGITAL( Navi_PM5 , 0    , &CommStatus );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {

	_beginthread( (void*)Preventive_Maintenance_Control  , 0 , NULL );

	_beginthread( (void*)TMConfig_Select_Control	, 0 , NULL );

	_beginthread( (void*)LotPreConfig_Select_Control	, 0 , NULL );

	_beginthread( (void*)PM_Heater_Control  , 0 , NULL );

	_beginthread( (void*)PM1_Gas_Config_Select_Control  , 0 , NULL );
    _beginthread( (void*)PM1_Moving_Config_Select_Control  , 0 , NULL );

	_beginthread( (void*)PM2_Gas_Config_Select_Control  , 0 , NULL );
    _beginthread( (void*)PM2_Moving_Config_Select_Control  , 0 , NULL );

    _beginthread( (void*)PM3_Gas_Config_Select_Control  , 0 , NULL );
    _beginthread( (void*)PM3_Moving_Config_Select_Control  , 0 , NULL );

    _beginthread( (void*)PM4_Gas_Config_Select_Control  , 0 , NULL );
    _beginthread( (void*)PM4_Moving_Config_Select_Control  , 0 , NULL );

    _beginthread( (void*)PM5_Gas_Config_Select_Control  , 0 , NULL );
    _beginthread( (void*)PM5_Moving_Config_Select_Control  , 0 , NULL );

	//kcr
	_beginthread( (void*)Module_Select_Navigation_Control  , 0 , NULL );

	return SYS_SUCCESS;
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
