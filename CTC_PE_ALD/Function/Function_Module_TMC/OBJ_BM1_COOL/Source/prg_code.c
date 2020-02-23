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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"				, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	{ "PM6.Progress"					, _K_D_IO	,	Progress			,	0	} ,
	{ "PM6.Process_Run"					, _K_D_IO	,	Process_Run			,	0	} ,
	{ "PM6.Process_Message"				, _K_S_IO	,	Process_Message		,	0	} ,
	{ "PRMD_BM1_COOL_TIME"				, _K_D_IO	,	PRM_BM1_COOL_TIME	,	0	} ,
	{ "CTC.TR_Clear"					, _K_D_IO	,	CTC_TR_Clear		,	0	} ,
    { "BM1.ATMVACSensorXI"				, _K_D_IO	,	ATMVACSensorXI		,	0	} , 
	{ "CTC.BM1_Wafer_Status"			, _K_D_IO	,	CTC_BM1_Wafer_Status,	0	} ,
	{ "CTC.BM1_Wafer_Result"			, _K_D_IO	,	CTC_BM1_Wafer_Result,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status2"			, _K_D_IO	,	CTC_BM1_Wafer_Status2,	0	} ,
	{ "CTC.BM1_Wafer_Result2"			, _K_D_IO	,	CTC_BM1_Wafer_Result2,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status3"			, _K_D_IO	,	CTC_BM1_Wafer_Status3,	0	} ,
	{ "CTC.BM1_Wafer_Result3"			, _K_D_IO	,	CTC_BM1_Wafer_Result3,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status4"			, _K_D_IO	,	CTC_BM1_Wafer_Status4,	0	} ,
	{ "CTC.BM1_Wafer_Result4"			, _K_D_IO	,	CTC_BM1_Wafer_Result4,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status5"			, _K_D_IO	,	CTC_BM1_Wafer_Status5,	0	} ,
	{ "CTC.BM1_Wafer_Result5"			, _K_D_IO	,	CTC_BM1_Wafer_Result5,	0	} , //Present, Success, Fail,Processing
	{ "CTC.BM1_Wafer_Status6"			, _K_D_IO	,	CTC_BM1_Wafer_Status6,	0	} ,
	{ "CTC.BM1_Wafer_Result6"			, _K_D_IO	,	CTC_BM1_Wafer_Result6,	0	} , //Present, Success, Fail,Processing	
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PROCESS( ) {
	int i, j , CommStatus , COOL_TIME;
	BOOL Result = FALSE;

	printf( "===> Start Cooler Process Control\n" );
	WRITE_DIGITAL( Progress , 0	, &CommStatus );

	COOL_TIME = READ_DIGITAL( PRM_BM1_COOL_TIME , &CommStatus );	
	if ( COOL_TIME == 0 ) COOL_TIME = 1;

	WRITE_STRING(  Process_Message	, "Start Process..." , &CommStatus );
	WRITE_DIGITAL( Process_Run		, 1					 , &CommStatus );

	Result = TRUE;
	/*for ( i = ( COOL_TIME ) * 10  ; i >= 0 ; i-- ) { // i : 50, 49, ... 1, 0
		if ( !WAIT_SECONDS( 0.1 ) ) { Result = FALSE; break; }

		j = ( ( ( COOL_TIME - ( i / 10 ) ) * 100 ) / ( COOL_TIME ) );
		WRITE_DIGITAL( Progress	, j , &CommStatus );
	}
	*/
	for ( i = 1  ; i <= COOL_TIME ; i++ ) { // i : 50, 49, ... 1, 0
		if ( !WAIT_SECONDS( 1 ) ) { Result = FALSE; break; }

		//j = ( ( ( COOL_TIME - ( i / 10 ) ) * 100 ) / ( COOL_TIME ) );
		WRITE_DIGITAL( Progress	, i , &CommStatus );
	}
	
	WRITE_DIGITAL( Process_Run		, 0					, &CommStatus );
	WRITE_STRING(  Process_Message  , "End Process..."  , &CommStatus );

	WRITE_DIGITAL( Progress				, 0		, &CommStatus );
	WRITE_STRING(  Process_Message		, ""	, &CommStatus );

	if ( Result ) return SYS_SUCCESS;
	else          return SYS_ABORTED;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int CommStatus;
	char RunStr[21] , TempStr[21];

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , TempStr , 20 );


	if	( STRCMP_L( RunStr , "PROCESS"		) ) {
		if (   READ_DIGITAL( CTC_BM1_Wafer_Result	, &CommStatus ) != 0 
			|| READ_DIGITAL( CTC_BM1_Wafer_Result2	, &CommStatus ) != 0
			|| READ_DIGITAL( CTC_BM1_Wafer_Result3	, &CommStatus ) != 0
			|| READ_DIGITAL( CTC_BM1_Wafer_Result4	, &CommStatus ) != 0
			|| READ_DIGITAL( CTC_BM1_Wafer_Result5	, &CommStatus ) != 0
			|| READ_DIGITAL( CTC_BM1_Wafer_Result6	, &CommStatus ) != 0) {
			// Present,Success,Fail,Processing
			if ( MANAGER_ABORT() ) return SYS_ABORTED;
			else {
				if ( (( READ_DIGITAL( CTC_BM1_Wafer_Status  , &CommStatus ) >= 1 ) && ( READ_DIGITAL( CTC_BM1_Wafer_Status  , &CommStatus ) <= 25 )) 
				  || (( READ_DIGITAL( CTC_BM1_Wafer_Status2 , &CommStatus ) >= 1 ) && ( READ_DIGITAL( CTC_BM1_Wafer_Status2 , &CommStatus ) <= 25 ))
				  || (( READ_DIGITAL( CTC_BM1_Wafer_Status3 , &CommStatus ) >= 1 ) && ( READ_DIGITAL( CTC_BM1_Wafer_Status3 , &CommStatus ) <= 25 ))
				  || (( READ_DIGITAL( CTC_BM1_Wafer_Status4 , &CommStatus ) >= 1 ) && ( READ_DIGITAL( CTC_BM1_Wafer_Status4 , &CommStatus ) <= 25 ))
				  || (( READ_DIGITAL( CTC_BM1_Wafer_Status5 , &CommStatus ) >= 1 ) && ( READ_DIGITAL( CTC_BM1_Wafer_Status5 , &CommStatus ) <= 25 ))
				  || (( READ_DIGITAL( CTC_BM1_Wafer_Status6 , &CommStatus ) >= 1 ) && ( READ_DIGITAL( CTC_BM1_Wafer_Status6 , &CommStatus ) <= 25 ))) {
					//If Aleady Vented then Skip Cooling due to changed seqence table ..mgsong
					if (READ_DIGITAL(ATMVACSensorXI , &CommStatus) == 1 /*ATM*/)
						return SYS_SUCCESS;

					if ( PROCESS( ) == SYS_ABORTED ) {
						return SYS_ABORTED;
					}
				}
			}
		}
	}	
	else if ( STRCMP_L( RunStr , "M_PROCESS"	) ) {
		if ( PROCESS( ) == SYS_ABORTED ) {
			return SYS_ABORTED;
		}
	}


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

