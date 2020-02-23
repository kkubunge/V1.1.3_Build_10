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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;
//------------------------------------------------------------------------------------------
enum	{ SIM		, REAL		};
enum	{ OFF		, ON		};
enum	{ NO		, YES		};
enum	{ FINGER_A  , FINGER_B	};
enum	{ PREP_SEND , POST_RECV	};
enum	{ PORT1 , PORT2 , PORT3 , PORT4 , DUMMY };
enum	{ CM1 , CM2 , CM3 , CM4 , PM1 , PM2 , PM3 , PM4 , PM5 , PM6 , PM7 , PM8 , PM9 , PM10 , PM11 , PM12 , AL , IC };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_DRIVER_LOAD			,	0	} ,

	{ "CTC.TA_Wafer_Status"		, _K_D_IO	,	TA_Wafer_Status			,	0	} ,
	{ "CTC.TA_Wafer_Source"		, _K_D_IO	,	TA_Wafer_Source			,	0	} ,

	{ "CTC.TB_Wafer_Status"		, _K_D_IO	,	TB_Wafer_Status			,	0	} ,
	{ "CTC.TB_Wafer_Source"		, _K_D_IO	,	TB_Wafer_Source			,	0	} ,


	{ "LPMA.PM1_Use"			, _K_D_IO	,	LPMA_PM1_Use			,	0	} ,
	{ "LPMA.PM2_Use"			, _K_D_IO	,	LPMA_PM2_Use			,	0	} ,
	{ "LPMA.PM3_Use"			, _K_D_IO	,	LPMA_PM3_Use			,	0	} ,
	{ "LPMA.PM4_Use"			, _K_D_IO	,	LPMA_PM4_Use			,	0	} ,
	{ "LPMA.PM5_Use"			, _K_D_IO	,	LPMA_PM5_Use			,	0	} ,

	{ "LPMB.PM1_Use"			, _K_D_IO	,	LPMB_PM1_Use			,	0	} ,
	{ "LPMB.PM2_Use"			, _K_D_IO	,	LPMB_PM2_Use			,	0	} ,
	{ "LPMB.PM3_Use"			, _K_D_IO	,	LPMB_PM3_Use			,	0	} ,
	{ "LPMB.PM4_Use"			, _K_D_IO	,	LPMB_PM4_Use			,	0	} ,
	{ "LPMB.PM5_Use"			, _K_D_IO	,	LPMB_PM5_Use			,	0	} ,
	
	{ "LPMC.PM1_Use"			, _K_D_IO	,	LPMC_PM1_Use			,	0	} ,
	{ "LPMC.PM2_Use"			, _K_D_IO	,	LPMC_PM2_Use			,	0	} ,
	{ "LPMC.PM3_Use"			, _K_D_IO	,	LPMC_PM3_Use			,	0	} ,
	{ "LPMC.PM4_Use"			, _K_D_IO	,	LPMC_PM4_Use			,	0	} ,
	{ "LPMC.PM5_Use"			, _K_D_IO	,	LPMC_PM5_Use			,	0	} ,
	""
}; 
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status DCOPControl( int CheckMode , int Station , int Arm) {
	int Run_Port = -1, CommStatus;	
	switch ( CheckMode ) {
		case POST_RECV :
			if ( Station != CM1 && Station != CM2 && Station != CM3 &&
				 Station != PM1 && Station != PM2 && Station != PM3 && Station != PM4 && Station != PM5) 
			{
				printf( "=====> ***** TM DCOP Control Unknown Post Recv Station Error : %d\n" , Station );
				return SYS_SUCCESS;
			}
			break;
		default :
			printf( "=====> ***** TM DCOP Control Unknown Check Mode Error : %d\n" , CheckMode );
			return SYS_SUCCESS;
			break;
	}
	switch ( Arm ) {
		case FINGER_A :
			Run_Port = READ_DIGITAL( TA_Wafer_Source , &CommStatus );
			//Run_Slot = READ_DIGITAL( TA_Wafer_Status , &CommStatus );
			break;
		case FINGER_B :
			Run_Port = READ_DIGITAL( TB_Wafer_Source , &CommStatus );
			//Run_Slot = READ_DIGITAL( TB_Wafer_Status , &CommStatus );
			break;
		default :
			printf( "=====> ***** TM DCOP Control Unknown Arm Error : %d\n" , Arm );
			return SYS_SUCCESS;
			break;
	}
	switch ( Run_Port ) 
	{
		case PORT1 :
			if( PM1 == Station )		WRITE_DIGITAL(LPMA_PM1_Use, YES, &CommStatus);
			else if( PM2 == Station )	WRITE_DIGITAL(LPMA_PM2_Use, YES, &CommStatus);
			else if( PM3 == Station )	WRITE_DIGITAL(LPMA_PM3_Use, YES, &CommStatus);
			else if( PM4 == Station )	WRITE_DIGITAL(LPMA_PM4_Use, YES, &CommStatus);
			else if( PM5 == Station )	WRITE_DIGITAL(LPMA_PM5_Use, YES, &CommStatus);
			break;

		case PORT2 :
			if( PM1 == Station )		WRITE_DIGITAL(LPMB_PM1_Use, YES, &CommStatus);
			else if( PM2 == Station )	WRITE_DIGITAL(LPMB_PM2_Use, YES, &CommStatus);
			else if( PM3 == Station )	WRITE_DIGITAL(LPMB_PM3_Use, YES, &CommStatus);
			else if( PM4 == Station )	WRITE_DIGITAL(LPMB_PM4_Use, YES, &CommStatus);
			else if( PM5 == Station )	WRITE_DIGITAL(LPMB_PM5_Use, YES, &CommStatus);
			break;

		case PORT3 :
			if( PM1 == Station )		WRITE_DIGITAL(LPMC_PM1_Use, YES, &CommStatus);
			else if( PM2 == Station )	WRITE_DIGITAL(LPMC_PM2_Use, YES, &CommStatus);
			else if( PM3 == Station )	WRITE_DIGITAL(LPMC_PM3_Use, YES, &CommStatus);
			else if( PM4 == Station )	WRITE_DIGITAL(LPMC_PM4_Use, YES, &CommStatus);
			else if( PM5 == Station )	WRITE_DIGITAL(LPMC_PM5_Use, YES, &CommStatus);
			break;
			
			break;
		default : // case dummy return
//			printf( "=====> ***** TM DCOP Control Unknown Port Error : %d\n" , Run_Port );
			return SYS_SUCCESS;
			break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	Dl , Arm = -1 , Station = -1;
	unsigned char *Ed;
	BOOL Switch = FALSE;
	char RunStr[61] , TempStr1[61] , StationStr[41] , TempStr2[41] , SlotStr[21] , MsgStr[21];

	
	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( Dl < 1 ) return SYS_SUCCESS;

	STR_SEPERATE( Ed       , RunStr     , TempStr1 , 60 );
	STR_SEPERATE( TempStr1 , StationStr , TempStr2 , 40 );
	STR_SEPERATE( TempStr2 , SlotStr    , MsgStr   , 20 );

	if      ( STRCMP_L( StationStr , "A_CM1" ) ) { Station = CM1; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_CM2" ) ) { Station = CM2; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_CM3" ) ) { Station = CM3; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_CM4" ) ) { Station = CM4; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM1" ) ) { Station = PM1; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM2" ) ) { Station = PM2; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM3" ) ) { Station = PM3; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM4" ) ) { Station = PM4; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM5" ) ) { Station = PM5; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM6" ) ) { Station = PM6; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM7" ) ) { Station = PM7; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM8" ) ) { Station = PM8; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM9" ) ) { Station = PM9; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM10") ) { Station = PM10;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM11") ) { Station = PM11;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_PM12") ) { Station = PM12;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_AL"	 ) ) { Station = AL;  Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "A_IC"	 ) ) { Station = IC;  Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "B_CM1" ) ) { Station = CM1; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_CM2" ) ) { Station = CM2; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_CM3" ) ) { Station = CM3; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_CM4" ) ) { Station = CM4; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM1" ) ) { Station = PM1; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM2" ) ) { Station = PM2; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM3" ) ) { Station = PM3; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM4" ) ) { Station = PM4; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM5" ) ) { Station = PM5; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM6" ) ) { Station = PM6; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM7" ) ) { Station = PM7; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM8" ) ) { Station = PM8; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM9" ) ) { Station = PM9; Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM10") ) { Station = PM10;Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM11") ) { Station = PM11;Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_PM12") ) { Station = PM12;Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_AL"	 ) ) { Station = AL;  Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "B_IC"	 ) ) { Station = IC;  Arm = FINGER_B; }
	else if ( STRCMP_L( StationStr , "CM1"   ) ) { Station = CM1; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "CM2"   ) ) { Station = CM2; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "CM3"   ) ) { Station = CM3; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "CM4"   ) ) { Station = CM4; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM1"   ) ) { Station = PM1; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM2"   ) ) { Station = PM2; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM3"   ) ) { Station = PM3; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM4"   ) ) { Station = PM4; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM5"   ) ) { Station = PM5; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM6"   ) ) { Station = PM6; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM7"   ) ) { Station = PM7; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM8"	 ) ) { Station = PM8; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM9"	 ) ) { Station = PM9; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM10"	 ) ) { Station = PM10;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM11"	 ) ) { Station = PM11;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "PM12"	 ) ) { Station = PM12;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "AL"	 ) ) { Station = AL;  Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "IC"	 ) ) { Station = IC;  Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "C1"    ) ) { Station = CM1; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "C2"    ) ) { Station = CM2; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "C3"    ) ) { Station = CM3; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "C4"    ) ) { Station = CM4; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P1"    ) ) { Station = PM1; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P2"    ) ) { Station = PM2; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P3"    ) ) { Station = PM3; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P4"    ) ) { Station = PM4; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P5"    ) ) { Station = PM5; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P6"    ) ) { Station = PM6; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P7"    ) ) { Station = PM7; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P8"    ) ) { Station = PM8; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P9"    ) ) { Station = PM9; Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P10"   ) ) { Station = PM10;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P11"   ) ) { Station = PM11;Arm = FINGER_A; }
	else if ( STRCMP_L( StationStr , "P12"   ) ) { Station = PM12;Arm = FINGER_A; }
	else { 
		printf( "=====> ***** TM DCOP Control Error - Not Support Station %s\n" , Ed );
//		return SYS_ABORTED;
		return SYS_SUCCESS;
	}

	if ( STRCMP_L( RunStr , "TM_POST_RECV"		) ) {
		printf( "=====> ***** TM POST RECV WAIT CHECK : %s\n" , PROGRAM_PARAMETER_READ() );
		return DCOPControl( POST_RECV , Station , Arm);
	}
	else {
		printf( "=====> ***** TM DCOP Control Check Error - Not Support Code %s\n" , Ed );
//		return SYS_ABORTED;
	}
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
