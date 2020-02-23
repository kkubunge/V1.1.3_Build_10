//-----------------------------------------------------------------------------------
// the function level is higher than the scheduler level
//-----------------------------------------------------------------------------------
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
enum { Idle , Running , Pausing , Paused , Aborting , Disable , Waiting , WaitHandOff };
enum { PORT1 , PORT2 , PORT3 , PORT4 };
enum { EQ_OFFL , ATTEMP_ONL , HOST_OFFL , LOCAL , REMOTE };
enum { ON,	OFF };
enum { Check,	NotCheck };
enum { Unknown,  Absent,  Present,  Process,  Failure };
char STRATSTR[4][16] = { "START(F)" , "START2(F)" , "START3(F)" , "START4(F)" };
char PORTSTR[4][8]  = { "PORT1" , "PORT2" , "PORT3" , "PORT4" };
char MIDSTR[4][8]	= { " " , "  " , "   " , "    " };
int  PORTID = PORT2; 
BOOL REVERSE = FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SCHEDULER"				, _K_F_IO	,	F_SCHEDULER			,	0	} ,
	{ "e:SCHEDULER"				, _K_S_IO	,	S_SCHEDULER			,	0	} ,
	{ "e:GEM.CONTROL"			, _K_S_IO	,	GEM_CONTROL			,	0	} ,
	{ "CTC.MAIN_CONTROL2$0"		, _K_D_IO	,	PORT_CONTROL		,	0	} ,
	{ "CTC.MID_NAME2$0"			, _K_S_IO	,	MID_NAME			,	0	} ,
	{ "CTC.JID_READ2$0"			, _K_S_IO	,	JID_READ			,	0	} ,
	{ "CTC.RECIPE_FILE2$0"		, _K_S_IO	,	RECIPE_FILE			,	0	} ,
	{ "CTC.START_SLOT2$0"		, _K_D_IO	,	START_SLOT			,	0	} ,
	{ "CTC.END_SLOT2$0"			, _K_D_IO	,	END_SLOT			,	0	} ,
	{ "CTC.LOOP_COUNT2$0"		, _K_D_IO	,	LOOP_COUNT			,	0	} ,
	{ "CTC.IN_PATH2$0"			, _K_D_IO	,	IN_PATH				,	0	} ,
	{ "CTC.OUT_PATH2$0"			, _K_D_IO	,	OUT_PATH			,	0	} ,
	{ "CTC.MAIN_STATUS2$0"		, _K_D_IO	,	PORT_STATUS			,	0	} ,
	{ "GEM.Control_State"		, _K_D_IO	,	CONTROL_STATE		,	0	} , 
	{ "START_CONTROL_OPTION"	, _K_D_IO	,	STARTOPTION			,	0	} , // jelee Added 2005.07.25
	{ "CTC.OPERATOR_START2"		, _K_D_IO	,	OPERATOR_START2		,	0	} ,

	{ "PRM_System_WFR_Check"	, _K_D_IO	,	PRM_System_WFR_Check,	0	} ,
	{ "CM2.C01_Wafer"			, _K_D_IO	,	CM2_C01_Wafer		,	0	} ,
	{ "CM2.C02_Wafer"			, _K_D_IO	,	CM2_C02_Wafer		,	0	} ,
	{ "CM2.C03_Wafer"			, _K_D_IO	,	CM2_C03_Wafer		,	0	} ,
	{ "CM2.C04_Wafer"			, _K_D_IO	,	CM2_C04_Wafer		,	0	} ,
	{ "CM2.C05_Wafer"			, _K_D_IO	,	CM2_C05_Wafer		,	0	} ,
	{ "CM2.C06_Wafer"			, _K_D_IO	,	CM2_C06_Wafer		,	0	} ,
	{ "CM2.C07_Wafer"			, _K_D_IO	,	CM2_C07_Wafer		,	0	} ,
	{ "CM2.C08_Wafer"			, _K_D_IO	,	CM2_C08_Wafer		,	0	} ,
	{ "CM2.C09_Wafer"			, _K_D_IO	,	CM2_C09_Wafer		,	0	} ,
	{ "CM2.C10_Wafer"			, _K_D_IO	,	CM2_C10_Wafer		,	0	} ,
	{ "CM2.C11_Wafer"			, _K_D_IO	,	CM2_C11_Wafer		,	0	} ,
	{ "CM2.C12_Wafer"			, _K_D_IO	,	CM2_C12_Wafer		,	0	} ,
	{ "CM2.C13_Wafer"			, _K_D_IO	,	CM2_C13_Wafer		,	0	} ,
	{ "CM2.C14_Wafer"			, _K_D_IO	,	CM2_C14_Wafer		,	0	} ,
	{ "CM2.C15_Wafer"			, _K_D_IO	,	CM2_C15_Wafer		,	0	} ,
	{ "CM2.C16_Wafer"			, _K_D_IO	,	CM2_C16_Wafer		,	0	} ,
	{ "CM2.C17_Wafer"			, _K_D_IO	,	CM2_C17_Wafer		,	0	} ,
	{ "CM2.C18_Wafer"			, _K_D_IO	,	CM2_C18_Wafer		,	0	} ,
	{ "CM2.C19_Wafer"			, _K_D_IO	,	CM2_C19_Wafer		,	0	} ,
	{ "CM2.C20_Wafer"			, _K_D_IO	,	CM2_C20_Wafer		,	0	} ,
	{ "CM2.C21_Wafer"			, _K_D_IO	,	CM2_C21_Wafer		,	0	} ,
	{ "CM2.C22_Wafer"			, _K_D_IO	,	CM2_C22_Wafer		,	0	} ,
	{ "CM2.C23_Wafer"			, _K_D_IO	,	CM2_C23_Wafer		,	0	} ,
	{ "CM2.C24_Wafer"			, _K_D_IO	,	CM2_C24_Wafer		,	0	} ,
	{ "CM2.C25_Wafer"			, _K_D_IO	,	CM2_C25_Wafer		,	0	} ,

	//2012.06.02	Wafer Sync Confirm
	{ "CTC.WfrSyncConfirm"		, _K_D_IO	,	WfrSyncConfirm		,	0	} ,
	{ "CTC.ManualTRConfirm"		, _K_D_IO	,	ManualTRConfirm		,	0	} ,

	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// When Fixed Group is used, if you want to use a recipe of other group
// Read CTC.RECIPE_FILE, and analysis the Group of IO.
// So if the IO has Fixed Group, pleas other code
// If not, send start command to scheduler directrly
Module_Status Program_Main() {
	int Dl , CommStatus , Status , i , S_Slot , E_Slot , nOPRStartMode;
	int nWfrChkOption, nWaferStatus, j;
//	int Option; // jelee Added 2005.07.25
//	char Buffer6[256]; // jelee Added 2005.07.25
	char RunStr[32] , ElseStr[32] , Buffer[256] , Buffer2[128];
	char Buffer3[128] , Buffer4[128] , Buffer5[128];
	unsigned char *Ed;

// jelee Add Start 2005.07.25	
/*	Option = READ_DIGITAL( STARTOPTION , &CommStatus );
	if( Option == 0 )
	{
		sprintf( Buffer6 , "SET_CONFIG CTC|START_OPTION|1" ); // concurrent
		WRITE_FUNCTION_EVENT( F_SCHEDULER , Buffer6 );
		_sleep(100);
		WRITE_DIGITAL( STARTOPTION , 0 , &CommStatus ); // 0 = concurrent
	}
	else if ( Option == 1 ) 
	{
		sprintf( Buffer6 , "SET_CONFIG CTC|START_OPTION|2" ); // continue
		WRITE_FUNCTION_EVENT( F_SCHEDULER , Buffer6 );
		_sleep(100);
		WRITE_DIGITAL( STARTOPTION , 1 , &CommStatus ); // 1 = Continue
	}
	else if ( Option == 2 ) 
	{
		sprintf( Buffer6 , "SET_CONFIG CTC|START_OPTION|3" ); // sequential
		WRITE_FUNCTION_EVENT( F_SCHEDULER , Buffer6 );
		_sleep(100);
		WRITE_DIGITAL( STARTOPTION , 2 , &CommStatus ); // 2 = Sequential
	}
	_sleep(100);
*/
// jelee Add End.	

// jelee Add Start for Wafer Status Check Option at 2009.04.02
	nWfrChkOption = READ_DIGITAL(PRM_System_WFR_Check, &CommStatus);
	if( nWfrChkOption == Check ) {
		S_Slot = READ_DIGITAL( START_SLOT , &CommStatus );
		E_Slot = READ_DIGITAL( END_SLOT   , &CommStatus );
		for(j=CM2_C01_Wafer+S_Slot-1; j<=CM2_C01_Wafer+E_Slot-1; j++) {
			nWaferStatus = READ_DIGITAL(j, &CommStatus);
			if( nWaferStatus == Process ) {
				sprintf( Buffer , "[%s]Manual GUI Start Control Parameter(Wafer Slot Range) Error\n", PORTSTR[PORTID] );
				printf( "=====> %s\n" , Buffer );
				MessageBox( GetTopWindow( NULL ) , Buffer , "ERROR" , MB_ICONHAND );
				return SYS_ABORTED;
			}
		}
	}
// jelee Add End for Wafer Status Check Option at 2009.04.02

//	SYSTEMTIME SysTime;
	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;
	if ( Dl < 1 ) {
		sprintf( Buffer , "[%s] GUI Start Control Parameter Error\n" , PORTSTR[PORTID] );
		printf( "=====> %s\n" , Buffer );
		MessageBox( GetTopWindow( NULL ) , Buffer , "ERROR" , MB_ICONHAND );
		return SYS_ABORTED;
	}

	nOPRStartMode = READ_DIGITAL(OPERATOR_START2, &CommStatus);
	if ( ON != nOPRStartMode ) {
		WRITE_DIGITAL( OPERATOR_START2, ON, &CommStatus);
	}

	STR_SEPERATE( Ed , RunStr , ElseStr , 31 );
	if ( strcmp( RunStr , STRATSTR[PORTID] ) != 0 ) {
		sprintf( Buffer , "[%s] GUI Start Name Parameter Error\n" , PORTSTR[PORTID] );
		printf( "=====> %s\n" , Buffer );
		MessageBox( GetTopWindow( NULL ) , Buffer , "ERROR" , MB_ICONHAND );
		return SYS_ABORTED;
	}
	else{
		// 2012.06.02 Wafer Sync confirm 0 == Unknown, ManualTRConfirm 0 == Unknown
		WRITE_DIGITAL( WfrSyncConfirm	, 0, &CommStatus);
		WRITE_DIGITAL( ManualTRConfirm	, 0, &CommStatus);
	}
	
// if need, open
//	if ( READ_DIGITAL( CONTROL_STATE , &CommStatus ) == REMOTE ) { 
//		MessageBox( GetTopWindow( NULL )/*GET_MAIN_WINDOW()*/ , "can not start because control state is REMOTE" , "ERROR" , MB_ICONHAND );
//		return;
//	}

	if ( READ_DIGITAL( PORT_CONTROL , &CommStatus ) != Idle ) {
		sprintf( Buffer , "[%s] is not idle Status\n" , PORTSTR[PORTID] );
		printf( "=====> %s\n" , Buffer );
		MessageBox( GetTopWindow( NULL ) , Buffer , "ERROR" , MB_ICONHAND );
		return SYS_ABORTED;
	}

	READ_STRING( RECIPE_FILE , Buffer2 , &CommStatus );
	if ( strlen( Buffer2 ) <= 0 ) {
		sprintf( Buffer , "[%s] is not selected a recipe file\n" , PORTSTR[PORTID] );
		printf( "=====> %s\n" , Buffer );
		MessageBox( GetTopWindow( NULL ) , Buffer , "ERROR" , MB_ICONHAND );
		return SYS_ABORTED;
	}

// select a condition
// => Port Control Check
	// if it is maintenance( loop run ), the command must be sent directly to scheduler.
	if ( READ_DIGITAL( LOOP_COUNT , &CommStatus ) > 1 ) {
		WRITE_FUNCTION_EVENT( F_SCHEDULER , RunStr );
		return SYS_SUCCESS;
	}
	
	// even though a port is already loaded, re-load action must be done.
	// if re-load action is impossible, this part must be skipped after check the loaded.
	sprintf( Buffer , "MAPPING_F CM%c" , '1' + PORTID );
	WRITE_STRING( S_SCHEDULER , Buffer , &CommStatus );
	_sleep( 300 );

	while ( TRUE ) {
		if ( MANAGER_ABORT() ) return SYS_ABORTED;
		Status = READ_DIGITAL( PORT_CONTROL , &CommStatus );
		if		( Status == Idle ) break; // if need, use "sleep" and check three times
		else if ( Status == Aborting || Status == Disable ) return SYS_ABORTED;
		_sleep( 200 );
	}
	if ( MANAGER_ABORT() ) return SYS_ABORTED;
	if ( READ_DIGITAL( PORT_CONTROL , &CommStatus ) != Idle ) return SYS_ABORTED;
	//

	i = 0;
	while( TRUE ) {
		Status = READ_FUNCTION_EVENT( F_SCHEDULER );
		if		( Status == SYS_AVAILABLE ) { 
			i++; if ( i > 50 ) break;
		}
		else if ( Status != SYS_RUNNING	) break;
		_sleep( 100 );
	}
	sprintf( Buffer , "CONTROLJOB MAPVERIFY|CM%c" , '1' + PORTID );
	WRITE_STRING( S_SCHEDULER , Buffer , &CommStatus );

	_sleep( 300 );
	if ( READ_DIGITAL( PORT_CONTROL , &CommStatus ) != Idle ) return SYS_SUCCESS; // in the case of a por is running
	
/*
// => Port Status Check
	Status = READ_DIGITAL( PORT_STATUS , &CommStatus );
	READ_STRING( MID_NAME , Buffer , &CommStatus );
	// 2 = Loaded, 3 = LoadFail, 4 = Mapping, 5 = Mapped, 6 = MapFail, Buffer <= 0 : MID Data NULL
	if ( ( READ_DIGITAL( IN_PATH , &CommStatus ) != READ_DIGITAL( OUT_PATH , &CommStatus ) ) || 
		( Status != 2/ && Status != 3 && Status != 4 && Status != 5 && Status != 6 ) ||
		READ_DIGITAL( LOOP_COUNT , &CommStatus ) > 1 || strlen( Buffer ) <= 0 ) {
		WRITE_FUNCTION_EVENT( F_SCHEDULER , RunStr );
		return SYS_SUCCESS;
	}
	else if ( Status == 2 || Status == 3 || Status == 6 ) {
		sprintf( Buffer , "MAPPING_F CM%c" , '1' + PORTID );
		WRITE_STRING( S_SCHEDULER , Buffer , &CommStatus );
		_sleep( 1000 );
		while( TRUE ) {
			_sleep( 100 );
			Status = READ_DIGITAL( PORT_STATUS , &CommStatus );
			if		( Status == 5 ) break;
			else if ( Status != 4 ) return; // Maybe alarm will be already happened
		}

		i = 0;
		while( TRUE ) {
			Status = READ_FUNCTION_EVENT( F_SCHEDULER );
			if		( Status == SYS_AVAILABLE ) { 
				i++; if ( i > 50 ) break;
			}
			else if ( Status != SYS_RUNNING	) break;
			_sleep( 100 );
		}
		sprintf( Buffer , "CONTROLJOB MAPVERIFY|CM%c" , '1' + PORTID );
		WRITE_STRING( S_SCHEDULER , Buffer , &CommStatus );
	}
	else if ( Status == 4 ) { // Mapping
		while( TRUE ) {
			_sleep( 100 );
			Status = READ_DIGITAL( PORT_STATUS , &CommStatus );
			if		( Status == 5 ) break;
			else if ( Status != 4 ) return; // Maybe alarm will be already happened
		}

		i = 0;
		while( TRUE ) {
			Status = READ_FUNCTION_EVENT( F_SCHEDULER );
			if		( Status == SYS_AVAILABLE ) { 
				i++; if ( i > 50 ) break;
			}
			else if ( Status != SYS_RUNNING	) break;
			_sleep( 100 );
		}
		sprintf( Buffer , "CONTROLJOB MAPVERIFY|CM%c" , '1' + PORTID );
		WRITE_STRING( S_SCHEDULER , Buffer , &CommStatus );
	}
	else if ( status == 5 ) { // Mapped
		i = 0;
		while( TRUE ) {
			Status = READ_FUNCTION_EVENT( F_SCHEDULER );
			if		( Status == SYS_AVAILABLE ) { 
				i++; if ( i > 50 ) break;
			}
			else if ( Status != SYS_RUNNING	) break;
			_sleep( 100 );
		}
		sprintf( Buffer , "CONTROLJOB MAPVERIFY|CM%c" , '1' + PORTID );
		WRITE_STRING( S_SCHEDULER , Buffer , &CommStatus );
	}
	else {
		WRITE_FUNCTION_EVENT( F_SCHEDULER , RunStr );
		return SYS_SUCCESS;
	}
*/

//	GetLocalTime( &SysTime );
	READ_STRING( JID_READ	 , Buffer2 , &CommStatus ); // Control Job Name = Process Job Name
	READ_STRING( JID_READ	 , Buffer3 , &CommStatus ); // Process Job Name
	if ( strlen( Buffer2 ) <= 0 ) {
// select CJ name
//		sprintf( Buffer2 , "CJ%04d%02d%02d%02d%02d%02d01" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
//			SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // Ctrl Job Name
		sprintf( Buffer2 , "$M_CJ%c$" , '1' + PORTID ); 
	}
	if ( strlen( Buffer3 ) <= 0 ) {
// select PJ name
//		sprintf( Buffer3 , "PJ%04d%02d%02d%02d%02d%02d01" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
//			SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // Prcs Job Name
		sprintf( Buffer3 , "$M_PJ%c$" , '1' + PORTID ); 
	}
	READ_STRING( MID_NAME	 , Buffer4 , &CommStatus ); // Carrier ID
	if ( strlen( Buffer4 ) <= 0 ) {
		sprintf( Buffer4 , "%s" , MIDSTR[PORTID] );
		WRITE_STRING( MID_NAME , MIDSTR[PORTID] , &CommStatus );
	}
	READ_STRING( RECIPE_FILE , Buffer5 , &CommStatus ); // PPID, if fixed group, please cut the group name from buffer5
	S_Slot = READ_DIGITAL( START_SLOT , &CommStatus );
	E_Slot = READ_DIGITAL( END_SLOT   , &CommStatus );
	
	// CJNAME|PJNAME|CARRIERID|STARTSLOT|ENDSLOT|PPID|CJSTARTMODE|PJSTARTMODE
	// "JOB_CREATE" is used only at Fixed Group or No Group, "JOB_CREATE2" is used at All Group Control 
	if ( REVERSE ) sprintf( Buffer , "JOBCONTROL JOB_CREATE2|%s|%s|%s|%d|%d|%s|1|1" , Buffer2 , Buffer3 , Buffer4 , S_Slot , E_Slot , Buffer5 ); // 1 = Auto
	else sprintf( Buffer , "JOBCONTROL JOB_CREATE2|%s|%s|%s|%d|%d|%s|0|0" , Buffer2 , Buffer3 , Buffer4 , S_Slot , E_Slot , Buffer5 ); // 0 = Auto

	WRITE_STRING( GEM_CONTROL , Buffer , &CommStatus );

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char Str1[32] , ElseStr[32];
	STR_SEPERATE_CHAR( PROGRAM_ARGUMENT_READ()	, '|' , Str1 , ElseStr , 31 ); // NULL, 2, 3, 4
	REPLACE_CHAR_SET( 0 , Str1 );
	if ( strcmp( Str1 , "1" ) == 0 || strcmp( Str1 , "3" ) == 0 || strcmp( Str1 , "4" ) == 0 ) PORTID = atoi( Str1 ) - 1;
	else PORTID = PORT2; 
	STR_SEPERATE_CHAR( ElseStr	, '|' , Str1 , ElseStr , 31 );
	if ( strcmp( Str1 , "R" ) == 0 ) REVERSE = TRUE;
	else REVERSE = TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
