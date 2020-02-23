//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>


#include <windows.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>


#include <Kutlstr.h>
#include <Kutlanal.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "logview.h"

extern BOOL Extern_Appen_Log( char * );

//---------------------------------------------------------------------------------------
extern CRITICAL_SECTION CR_SCREG;
#define REGIST_MAX_SIZE	30
//---------------------------------------------------------------------------------------
int  Reg_Size = 0;
int  Reg_Side[REGIST_MAX_SIZE];
char Reg_EndStr[REGIST_MAX_SIZE][128];
char Reg_ElapsedStr[REGIST_MAX_SIZE][128];
int  Reg_Methodcnt[REGIST_MAX_SIZE];
//int  Reg_Endtype[REGIST_MAX_SIZE]; // 2003.07.30
//---------------------------------------------------------------------------------------
void READ_FUNCTION_For_RESET() {
	EnterCriticalSection( &CR_SCREG );
	Reg_Size = 0;
	LeaveCriticalSection( &CR_SCREG );
}
//---------------------------------------------------------------------------------------
BOOL READ_FUNCTION_For_STOP_CHECK( int Side , int *Res ) {
	int i;
	EnterCriticalSection( &CR_SCREG );
	if ( Reg_Size <= 0 ) {
		LeaveCriticalSection( &CR_SCREG );
		return FALSE;
	}
	for ( i = 0 ; i < Reg_Size ; i++ ) {
		if ( Reg_Side[i] == Side ) {
			*Res = i;
			LeaveCriticalSection( &CR_SCREG );
			return TRUE;
		}
	}
	LeaveCriticalSection( &CR_SCREG );
	return FALSE;
}
//---------------------------------------------------------------------------------------
void READ_FUNCTION_For_STOP_CHECK_DATA( int Res , char *END_TIME , char *ELAPSED_TIME , int *METHOD_COUNT ) {
	int i;
	EnterCriticalSection( &CR_SCREG );
	strcpy( END_TIME , Reg_EndStr[Res] );
	strcpy( ELAPSED_TIME , Reg_ElapsedStr[Res] );
	*METHOD_COUNT = Reg_Methodcnt[Res];
//	*END_TYPE = Reg_Endtype[Res]; // 2003.07.30
	for ( i = Res ; i < ( Reg_Size - 1 ); i++ ) {
		strcpy( Reg_EndStr[i] , Reg_EndStr[i+1] );
		strcpy( Reg_ElapsedStr[i] , Reg_ElapsedStr[i+1] );
		Reg_Methodcnt[i] = Reg_Methodcnt[i+1];
//		Reg_Endtype[i] = Reg_Endtype[i+1]; // 2003.07.30
	}
	Reg_Size--;
	LeaveCriticalSection( &CR_SCREG );
}
//---------------------------------------------------------------------------------------
void READ_FUNCTION_For_REGIST_DATA( int Side , char *END_TIME , char *ELAPSED_TIME , int METHOD_COUNT ) {
	EnterCriticalSection( &CR_SCREG );
	if ( Reg_Size < REGIST_MAX_SIZE ) {
		Reg_Side[Reg_Size] = Side;
		strncpy( Reg_EndStr[Reg_Size] , END_TIME , 127 );
		strncpy( Reg_ElapsedStr[Reg_Size] , ELAPSED_TIME , 127 );
		Reg_Methodcnt[Reg_Size] = METHOD_COUNT;
//		Reg_Endtype[Reg_Size] = END_TYPE; // 2003.07.30
		Reg_Size++;
	}
	LeaveCriticalSection( &CR_SCREG );
}
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
	int	side , wfrcnt , endtype;
	char RunStr[1025] , ElseStr[1025] , Else1Str[256] , Else2Str[256];
	char endStr[256] , elapsedStr[256];

	STR_SEPERATE( PROGRAM_EVENT_READ() , RunStr , ElseStr , 1024 );
	if      ( STRCMP_L( RunStr , "STOP" ) ) {
		STR_SEPERATE_CHAR( ElseStr , '|' , Else2Str , Else1Str , 255 );
		if      ( STRCMP_L( Else2Str , "1" ) ) side = 0;
		else if ( STRCMP_L( Else2Str , "2" ) ) side = 1;
		else if ( STRCMP_L( Else2Str , "3" ) ) side = 2;
		else if ( STRCMP_L( Else2Str , "4" ) ) side = 3;
		else return;
		STR_SEPERATE_CHAR( Else1Str , '|' , endStr , ElseStr , 255 );
		STR_SEPERATE_CHAR( ElseStr , '|' , elapsedStr , Else1Str , 255 );
		STR_SEPERATE_CHAR( Else1Str , '|' , Else2Str , ElseStr , 255 );
		wfrcnt = atoi( Else2Str );
		STR_SEPERATE_CHAR( ElseStr , '|' , Else2Str , Else1Str , 255 );
//		endtype = atoi( Else2Str ); // 2003.07.30
//		READ_FUNCTION_For_REGIST_DATA( side , endStr , elapsedStr , wfrcnt , endtype ); // 2003.07.30
		READ_FUNCTION_For_REGIST_DATA( side , endStr , elapsedStr , wfrcnt );
	}
	else if ( STRCMP_L( RunStr , "MAKELOG" ) ) {
		STR_SEPERATE_CHAR( ElseStr , '|' , Else2Str , Else1Str , 255 );
		if      ( STRCMP_L( Else2Str , "PROCESS" ) ) side = 0;
		else if ( STRCMP_L( Else2Str , "MODULE"  ) ) side = 1;
		else if ( STRCMP_L( Else2Str , "LOT"     ) ) side = 2;
		else {
			PROGRAM_EVENT_STATUS_SET( SYS_ERROR );
			return;
		}
		STR_SEPERATE_CHAR( Else1Str , '|' , endStr , ElseStr , 255 );
		if ( side != 2 ) {
			if ( STRNCMP_L( endStr , "PM" , 2 ) ) {
				wfrcnt = atoi( endStr + 2 );
				if ( ( wfrcnt <= 0 ) || ( wfrcnt >= 99 ) ) {
					PROGRAM_EVENT_STATUS_SET( SYS_ERROR );
					return;
				}
			}
			else {
				PROGRAM_EVENT_STATUS_SET( SYS_ERROR );
				return;
			}
		}
		else {
			wfrcnt = 0;
		}
		STR_SEPERATE_CHAR( ElseStr , '|' , elapsedStr , Else1Str , 255 );
		STR_SEPERATE_CHAR( Else1Str , '|' , Else2Str , ElseStr , 255 );
		endtype = atoi( ElseStr );
		if ( ( endtype < 0 ) || ( endtype > 3 ) ) {
			PROGRAM_EVENT_STATUS_SET( SYS_ERROR );
			return;
		}
		if ( Make_Estimate_LogData_for_Min_Max_Avr( TRUE , side , wfrcnt , -1 , -1 , elapsedStr , Else2Str , endtype , TRUE , 0 , FALSE ) ) {
			PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		}
		else {
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
		}
	}
	else if ( STRCMP_L( RunStr , "APPENDLOG" ) ) { // 2002.10.02
		if ( Extern_Appen_Log( ElseStr ) ) {
			PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		}
		else {
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
		}
	}
	else if ( STRCMP_L( RunStr , "REPORT_BUTTON_BITMAP" ) ) { // 2017.02.22
		KWIN_REPORT_USER_BUTTON_BITMAP();
	}
}