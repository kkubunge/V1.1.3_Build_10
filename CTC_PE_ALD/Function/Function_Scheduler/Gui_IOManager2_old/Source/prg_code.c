#define _WIN32_WINNT 0x500

#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

#include <windows.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "resource.h"

#include "cimseqnc.h"
#include "iodefine.h"



//------------------------------------------------------------------------------------------
#define ID_CONTROL_TIMER	1000
//------------------------------------------------------------------------------------------
#define MONITOR_TIME		250
//------------------------------------------------------------------------------------------
#define MAX_DISPLAY_COUNT		30
//------------------------------------------------------------------------------------------
int USE_DISPLAY_COUNT = 10; // 2006.05.29
//------------------------------------------------------------------------------------------
char GROUP_MONITOR_BUFFER[ 256 ];
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};
//------------------------------------------------------------------------------------------
int MATCH_CONTROL = 0;
//
CRITICAL_SECTION CR_RUN;
//------------------------------------------------------------------------------------------
HINSTANCE	BackBmpLib = NULL;
void		KGUI_Set_BACKGROUND( char *file ) { BackBmpLib = LoadLibrary( file ); }
HINSTANCE	KGUI_Get_BACKGROUND() { return BackBmpLib; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	IO_CONTROL_REPEAT_MODE = FALSE;
BOOL	IO_CONTROL_CHECK_MODE = FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  PROGRAM_GUI_SPAWNING = 0; // 2003.07.12
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  ConsoleGuiForLast_USE = 1;
int  ConsoleGuiForPres_USE = 0;
//------------------------------------------------------------------------------------------

void ConGui_Set_Mon( void );
void ConGui_ReSet_Mon( void );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READONLY_LEVEL = 0;
int READONLY_LEVEL_IO = -2;
char READONLY_LEVEL_IO_NAME[256];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		DISPLAY_TITLE_X			ConGui_DISPLAY_TITLE_X()
#define		DISPLAY_TITLE_Y			ConGui_DISPLAY_TITLE_Y()
#define		DISPLAY_TITLESIZE_X		15
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		DISPLAY_START_X_FOR_DIGITAL		DISPLAY_TITLE_X + 16
#define		DISPLAY_START_Y_FOR_DIGITAL		DISPLAY_TITLE_Y + 40
//------------------------------------------------------------------------------------------
#define		DISPLAY_START_X_FOR_ANALOG		DISPLAY_TITLE_X + 16
#define		DISPLAY_START_Y_FOR_ANALOG		DISPLAY_TITLE_Y + 40
//------------------------------------------------------------------------------------------
#define		DISPLAY_START_X_FOR_STRING		DISPLAY_TITLE_X + 16
#define		DISPLAY_START_Y_FOR_STRING		DISPLAY_TITLE_Y + 40
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//#define		DISPLAY_COUNT_FOR_DIGITAL		10

#define		DISPLAY_SIZE_X_FOR_DIGITAL		4
#define		DISPLAY_SIZE_X_FOR_DIGITAL1		30
#define		DISPLAY_SIZE_X_FOR_DIGITAL2		20
#define		DISPLAY_SIZE_X_FOR_DIGITAL3		20
#define		DISPLAY_SIZE_Y_FOR_DIGITAL		34

#define		DISPLAY_START_X_FOR_DIGITAL1	DISPLAY_START_X_FOR_DIGITAL+(DISPLAY_SIZE_X_FOR_DIGITAL*8)+7
#define		DISPLAY_START_X_FOR_DIGITAL2	DISPLAY_START_X_FOR_DIGITAL1+(DISPLAY_SIZE_X_FOR_DIGITAL1*8)+7+2
#define		DISPLAY_START_X_FOR_DIGITAL3	DISPLAY_START_X_FOR_DIGITAL2+(DISPLAY_SIZE_X_FOR_DIGITAL2*8)+7+2
#define		DISPLAY_SCALE_X_FOR_DIGITAL		DISPLAY_START_X_FOR_DIGITAL
#define		DISPLAY_SCALE_X_FOR_DIGITAL2	DISPLAY_SCALE_X_FOR_DIGITAL+90
//#define	DISPLAY_SCALE_Y_FOR_DIGITAL		(DISPLAY_COUNT_FOR_DIGITAL*DISPLAY_SIZE_Y_FOR_DIGITAL)+DISPLAY_START_Y_FOR_DIGITAL+10
#define		DISPLAY_SCALE_Y_FOR_DIGITAL		(USE_DISPLAY_COUNT*DISPLAY_SIZE_Y_FOR_DIGITAL)+DISPLAY_START_Y_FOR_DIGITAL+10

#define		DIGITAL_SCR_X					DISPLAY_START_X_FOR_DIGITAL3+(DISPLAY_SIZE_X_FOR_DIGITAL3*8)+7+2
#define		DIGITAL_SCR_Y					DISPLAY_START_Y_FOR_DIGITAL-1
#define		DIGITAL_SCR_XS					45
//#define		DIGITAL_SCR_YS					(DISPLAY_COUNT_FOR_DIGITAL*DISPLAY_SIZE_Y_FOR_DIGITAL)-3
#define		DIGITAL_SCR_YS					(USE_DISPLAY_COUNT*DISPLAY_SIZE_Y_FOR_DIGITAL)-3

int DISPLAY_DIGITAL_START_POINTER	= 0;
int DIGITAL_SELECT_POINTER	= -1;

//int DIGITAL_DATA_BUFFER_READ[ DISPLAY_COUNT_FOR_DIGITAL ];
int DIGITAL_DATA_BUFFER_READ[ MAX_DISPLAY_COUNT ];
//int DIGITAL_DATA_BUFFER_WRITE[ DISPLAY_COUNT_FOR_DIGITAL ];
int DIGITAL_DATA_BUFFER_WRITE[ MAX_DISPLAY_COUNT ];

//
//
//
//
//

//#define		DISPLAY_COUNT_FOR_ANALOG		10

#define		DISPLAY_SIZE_X_FOR_ANALOG		4
#define		DISPLAY_SIZE_X_FOR_ANALOG1		30
#define		DISPLAY_SIZE_X_FOR_ANALOG2		20
#define		DISPLAY_SIZE_X_FOR_ANALOG3		20
#define		DISPLAY_SIZE_X_FOR_ANALOG4		8
#define		DISPLAY_SIZE_Y_FOR_ANALOG		34

#define		DISPLAY_START_X_FOR_ANALOG1		DISPLAY_START_X_FOR_ANALOG+(DISPLAY_SIZE_X_FOR_ANALOG*8)+7
#define		DISPLAY_START_X_FOR_ANALOG2		DISPLAY_START_X_FOR_ANALOG1+(DISPLAY_SIZE_X_FOR_ANALOG1*8)+7+2
#define		DISPLAY_START_X_FOR_ANALOG3		DISPLAY_START_X_FOR_ANALOG2+(DISPLAY_SIZE_X_FOR_ANALOG2*8)+7+2
#define		DISPLAY_START_X_FOR_ANALOG4		DISPLAY_START_X_FOR_ANALOG3+(DISPLAY_SIZE_X_FOR_ANALOG3*8)+7+2

#define		DISPLAY_SCALE_X_FOR_ANALOG		DISPLAY_START_X_FOR_ANALOG
#define		DISPLAY_SCALE_X_FOR_ANALOG2		DISPLAY_SCALE_X_FOR_ANALOG+90
//#define	DISPLAY_SCALE_Y_FOR_ANALOG		(DISPLAY_COUNT_FOR_ANALOG*DISPLAY_SIZE_Y_FOR_ANALOG)+DISPLAY_START_Y_FOR_ANALOG+10
#define		DISPLAY_SCALE_Y_FOR_ANALOG		(USE_DISPLAY_COUNT*DISPLAY_SIZE_Y_FOR_ANALOG)+DISPLAY_START_Y_FOR_ANALOG+10

#define		ANALOG_SCR_X					DISPLAY_START_X_FOR_ANALOG4+(DISPLAY_SIZE_X_FOR_ANALOG4*8)+7+2
#define		ANALOG_SCR_Y					DISPLAY_START_Y_FOR_ANALOG-1
#define		ANALOG_SCR_XS					45
//#define	ANALOG_SCR_YS					(DISPLAY_COUNT_FOR_ANALOG*DISPLAY_SIZE_Y_FOR_ANALOG)-3
#define		ANALOG_SCR_YS					(USE_DISPLAY_COUNT*DISPLAY_SIZE_Y_FOR_ANALOG)-3

int DISPLAY_ANALOG_START_POINTER	= 0;
int ANALOG_SELECT_POINTER	= -1;

//double ANALOG_DATA_BUFFER_READ[ DISPLAY_COUNT_FOR_ANALOG ];
double ANALOG_DATA_BUFFER_READ[ MAX_DISPLAY_COUNT ];
//double ANALOG_DATA_BUFFER_WRITE[ DISPLAY_COUNT_FOR_ANALOG ];
double ANALOG_DATA_BUFFER_WRITE[ MAX_DISPLAY_COUNT ];

//
//
//
//
//

//#define		DISPLAY_COUNT_FOR_STRING		10

#define		DISPLAY_SIZE_X_FOR_STRING		4
#define		DISPLAY_SIZE_X_FOR_STRING1		30
#define		DISPLAY_SIZE_X_FOR_STRING3		54
#define		DISPLAY_SIZE_Y_FOR_STRING		34

#define		DISPLAY_START_X_FOR_STRING1		DISPLAY_START_X_FOR_STRING+(DISPLAY_SIZE_X_FOR_STRING*8)+7
#define		DISPLAY_START_X_FOR_STRING3		DISPLAY_START_X_FOR_STRING1+(DISPLAY_SIZE_X_FOR_STRING1*8)+7+2

#define		DISPLAY_SCALE_X_FOR_STRING		DISPLAY_START_X_FOR_STRING
#define		DISPLAY_SCALE_X_FOR_STRING2		DISPLAY_SCALE_X_FOR_STRING+90
//#define	DISPLAY_SCALE_Y_FOR_STRING		(DISPLAY_COUNT_FOR_STRING*DISPLAY_SIZE_Y_FOR_STRING)+DISPLAY_START_Y_FOR_STRING+10
#define		DISPLAY_SCALE_Y_FOR_STRING		(USE_DISPLAY_COUNT*DISPLAY_SIZE_Y_FOR_STRING)+DISPLAY_START_Y_FOR_STRING+10

#define		STRING_SCR_X					DISPLAY_START_X_FOR_STRING3+(DISPLAY_SIZE_X_FOR_STRING3*8)+7+2
#define		STRING_SCR_Y					DISPLAY_START_Y_FOR_STRING-1
#define		STRING_SCR_XS					45
//#define	STRING_SCR_YS					(DISPLAY_COUNT_FOR_STRING*DISPLAY_SIZE_Y_FOR_STRING)-3
#define		STRING_SCR_YS					(USE_DISPLAY_COUNT*DISPLAY_SIZE_Y_FOR_STRING)-3

int DISPLAY_STRING_START_POINTER	= 0;
int STRING_SELECT_POINTER	= -1;

//char STRING_DATA_BUFFER_READ[ DISPLAY_COUNT_FOR_STRING ][MAX_STRING_SIZE+1]; // 2005.05.11
//char STRING_DATA_BUFFER_WRITE[ DISPLAY_COUNT_FOR_STRING ][MAX_STRING_SIZE+1]; // 2005.05.11
//char STRING_DATA_BUFFER_READ[ DISPLAY_COUNT_FOR_STRING ][MAX_STRING_SIZE+MAX_STRING_SIZE+2]; // 2005.05.11
char STRING_DATA_BUFFER_READ[ MAX_DISPLAY_COUNT ][MAX_STRING_SIZE+MAX_STRING_SIZE+2]; // 2005.05.11
//char STRING_DATA_BUFFER_WRITE[ DISPLAY_COUNT_FOR_STRING ][MAX_STRING_SIZE+MAX_STRING_SIZE+2]; // 2005.05.11

//---------------------------------------------------------------------------------------
#define		DIGITAL_BUTTON_X		ConGui_DIGITAL_BUTTON_X()
#define		DIGITAL_BUTTON_Y		ConGui_DIGITAL_BUTTON_Y()
#define		DIGITAL_BUTTON_XS		100
#define		DIGITAL_BUTTON_YS		63
//
#define		ANALOG_BUTTON_X			ConGui_ANALOG_BUTTON_X()
#define		ANALOG_BUTTON_Y			ConGui_ANALOG_BUTTON_Y()
#define		ANALOG_BUTTON_XS		100
#define		ANALOG_BUTTON_YS		63
//
#define		STRING_BUTTON_X			ConGui_STRING_BUTTON_X()
#define		STRING_BUTTON_Y			ConGui_STRING_BUTTON_Y()
#define		STRING_BUTTON_XS		100
#define		STRING_BUTTON_YS		63
//
#define		POINT_BUTTON_X			ConGui_POINT_BUTTON_X()
#define		POINT_BUTTON_Y			ConGui_POINT_BUTTON_Y()
#define		POINT_BUTTON_XS			100
#define		POINT_BUTTON_YS			43
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void Set_Prm_READONLY_LEVEL( int data )		{ READONLY_LEVEL = data; } // 2004.01.07
BOOL Get_Prm_READONLY_MODE() { // 2004.01.07
	int i , CommStatus;
	if ( READONLY_LEVEL_IO_NAME[0] == 0 ) {
		i = GET_CURRENT_USERLEVEL();
	}
	else {
		if ( READONLY_LEVEL_IO == -2 ) {
			if ( ( READONLY_LEVEL_IO_NAME[0] == '<' ) || ( READONLY_LEVEL_IO_NAME[0] == '>' ) || ( READONLY_LEVEL_IO_NAME[0] == '=' ) || ( READONLY_LEVEL_IO_NAME[0] == '!' ) || ( READONLY_LEVEL_IO_NAME[0] == '{' ) || ( READONLY_LEVEL_IO_NAME[0] == '}' ) ) {
				READONLY_LEVEL_IO = _FIND_FROM_STRING( _K_D_IO , READONLY_LEVEL_IO_NAME + 1 );
			}
			else {
				READONLY_LEVEL_IO = _FIND_FROM_STRING( _K_D_IO , READONLY_LEVEL_IO_NAME );
			}
		}
		if ( READONLY_LEVEL_IO == -1 ) {
			i = GET_CURRENT_USERLEVEL();
		}
		else {
			i = _dREAD_DIGITAL( READONLY_LEVEL_IO , &CommStatus );
		}
	}
	if      ( READONLY_LEVEL_IO_NAME[0] == '<' ) {
		if ( i < READONLY_LEVEL ) return FALSE;
	}
	else if ( READONLY_LEVEL_IO_NAME[0] == '>' ) {
		if ( i > READONLY_LEVEL ) return FALSE;
	}
	else if ( READONLY_LEVEL_IO_NAME[0] == '=' ) {
		if ( i == READONLY_LEVEL ) return FALSE;
	}
	else if ( READONLY_LEVEL_IO_NAME[0] == '!' ) {
		if ( i != READONLY_LEVEL ) return FALSE;
	}
	else if ( READONLY_LEVEL_IO_NAME[0] == '{' ) {
		if ( i <= READONLY_LEVEL ) return FALSE;
	}
	else if ( READONLY_LEVEL_IO_NAME[0] == '}' ) {
		if ( i >= READONLY_LEVEL ) return FALSE;
	}
	else {
		if ( i >= READONLY_LEVEL ) return FALSE;
	}
	return TRUE;
}
void Set_Prm_READONLY_LEVEL_IO_NAME( char *data ) { // 2003.12.28
	strncpy( READONLY_LEVEL_IO_NAME , data , 255 );
}
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

void Screen_Nav_Info( int no ) {
	int i;
	if ( no == -1 ) {
		ConsoleGuiForPres_USE = 0;
		KGUI_STANDARD_Window_Kill_Signal();
		KWIN_DIALOG_DRIVER_UNLOADING();
		KGUI_PALETTE_UNLOADING( NULL );
	}
	if ( ( no > 0 ) && ( PROGRAM_GUI_SPAWNING == 0 ) ) { // 2003.07.12
		PROGRAM_GUI_SPAWNING = 1;
		for ( i = 0 ; i < 200 ; i++ ) {
			_sleep(100);
			if ( PROGRAM_GUI_SPAWNING == 2 ) break;
		}
	}
	KGUI_STANDARD_Screen_Nav_Info( no );
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void ConGui_START_POINTER_RESET() {
	DISPLAY_DIGITAL_START_POINTER	= 0;
	DISPLAY_ANALOG_START_POINTER	= 0;
	DISPLAY_STRING_START_POINTER	= 0;
}
//---------------------------------------------------------------------------------------
int ConGui_DISPLAY_TITLE_X() {
	if ( GetSystemMetrics( SM_CXSCREEN ) >= 1024 )	return 124;
	else											return 0;
}
int ConGui_DISPLAY_TITLE_Y() {
	if ( GetSystemMetrics( SM_CYSCREEN ) >= 768 )	return 135;
	else											return 0;
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
int ConGui_DIGITAL_BUTTON_X() {
	if ( GetSystemMetrics( SM_CXSCREEN ) >= 1024 )	return 20;
	else											return 200;
}
int ConGui_DIGITAL_BUTTON_Y() {
	if ( GetSystemMetrics( SM_CYSCREEN ) >= 768 )	return 172;
	else											return 390;
}
//
int ConGui_ANALOG_BUTTON_X() {
	if ( GetSystemMetrics( SM_CXSCREEN ) >= 1024 )	return 20;
	else											return 330;
}
//
int ConGui_ANALOG_BUTTON_Y() {
	if ( GetSystemMetrics( SM_CYSCREEN ) >= 768 )	return 255;
	else											return 390;
}
//
int ConGui_STRING_BUTTON_X() {
	if ( GetSystemMetrics( SM_CXSCREEN ) >= 1024 )	return 20;
	else											return 460;
}
//
int ConGui_STRING_BUTTON_Y() {
	if ( GetSystemMetrics( SM_CYSCREEN ) >= 768 )	return 338;
	else											return 390;
}
//
int ConGui_POINT_BUTTON_X() {
	if ( GetSystemMetrics( SM_CXSCREEN ) >= 1024 )	return 20;
	else											return 590;
}
int ConGui_POINT_BUTTON_Y() {
	if ( GetSystemMetrics( SM_CYSCREEN ) >= 768 )	return 467;
	else											return 390;
}
//
//
//
//
//
//
//
//
//
//
//
BOOL ConGui_Display_Data_Read( BOOL AllMode , int id ) {
//	char Return_IO_Status[1024] , Buffer1[256] , Buffer2[256] , Type; // 2005.05.12
	char Return_IO_Status[2048] , Buffer1[256] , Buffer2[256] , Type; // 2005.05.12
	int i , k , Point;
	
	if ( AllMode ) return TRUE;
	if ( _dGROUP_IO_READ_from_MEM( GROUP_MONITOR_BUFFER , Return_IO_Status , 2047 ) ) {
		i = 0;
		if ( ConsoleGuiForPres_USE != id ) return FALSE;
		while ( _dGROUP_IO_TRANSLATE_from_BUFFER( Return_IO_Status , i , &Type , &Point , Buffer1 , Buffer2 , &k ) ) {
			if ( ConsoleGuiForPres_USE != id ) return FALSE;
			switch( Type ) {
			case 'D' :
				D_IO_IN_VALUE( Point ) = atoi( Buffer1 );
				strcpy( D_IO_IN_VALUE_STR( Point ) , Buffer2 );
				break;
			case 'd' :
				D_IO_OUT_VALUE( Point ) = atoi( Buffer1 );
				strcpy( D_IO_OUT_VALUE_STR( Point ) , Buffer2 );
				break;
			case 'A' :
				A_IO_IN_VALUE( Point ) = atof( Buffer1 );
				strcpy( A_IO_IN_VALUE_STR( Point ) , Buffer2 );
				break;
			case 'a' :
				A_IO_OUT_VALUE( Point ) = atof( Buffer1 );
				strcpy( A_IO_OUT_VALUE_STR( Point ) , Buffer2 );
				break;
			case 'S' :
//				strcpy( S_IO_DATA( Point ) , Buffer1 ); // 2005.05.12
				strncpy( S_IO_DATA( Point ) , Buffer1 , MAX_STRING_SIZE+MAX_STRING_SIZE+1 ); // 2005.05.12
				break;
			case 's' :
//				strcpy( S_IO_DATA( Point ) , Buffer1 ); // 2005.05.12
				strncpy( S_IO_DATA( Point ) , Buffer1 , MAX_STRING_SIZE+MAX_STRING_SIZE+1 ); // 2005.05.12
				break;
			}
			i = k;
		}
	}
	return TRUE;
}
//
//

void ConGui_Display_Digital( BOOL AllMode , HDC hDC ) {
	int i , j , k;
	char buffer[256];

	if ( !ConGui_Display_Data_Read( AllMode , 1 ) ) return;

	if ( AllMode ) {
		wsprintf( buffer, "  Digital I/O" );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_TITLE_X , DISPLAY_TITLE_Y , buffer , DISPLAY_TITLESIZE_X , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkRed );
	}

	for ( i = 0 , k = 0 ; k < DIGITAL_TOTAL_COUNT() ; k++ ) {
		if ( D_IO_SELECT( k ) ) {
			if ( i == DISPLAY_DIGITAL_START_POINTER ) break;
			i++;
		}
	}
//	for ( i = DISPLAY_DIGITAL_START_POINTER , j = 0 ; ( k < DIGITAL_TOTAL_COUNT() ) && ( i < DIGITAL_SELECT_COUNT() ) && ( j < DISPLAY_COUNT_FOR_DIGITAL ) && ( ConsoleGuiForPres_USE == 1 ); k++ ) { // 2006.05.29
	for ( i = DISPLAY_DIGITAL_START_POINTER , j = 0 ; ( k < DIGITAL_TOTAL_COUNT() ) && ( i < DIGITAL_SELECT_COUNT() ) && ( j < USE_DISPLAY_COUNT ) && ( ConsoleGuiForPres_USE == 1 ); k++ ) { // 2006.05.29
		if ( D_IO_SELECT( k ) ) {
			//====================================================================
			READ_IO_INFORMATION_FOR_DISPLAY_DATA( 1 , k );
			//====================================================================
			if ( AllMode ) {
				wsprintf( buffer, "%4d" , i+1 );
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , buffer , DISPLAY_SIZE_X_FOR_DIGITAL , 1 , COLOR_White , COLOR_DarkGray , COLOR_Black , 13 );
				if ( ConsoleGuiForPres_USE != 1 ) return;
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL1 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , D_IO_NAME(k) , DISPLAY_SIZE_X_FOR_DIGITAL1 , 1 , COLOR_DarkGray , COLOR_White , COLOR_Black , 13 );
				DIGITAL_DATA_BUFFER_WRITE[ j ] = DIGITAL_DATA_BUFFER_READ[ j ] = -1;
			}
			if ( ConsoleGuiForPres_USE != 1 ) return;
//			if ( D_IO_TYPE( k ) == OUTPUT_IO_TYPE ) { // 2003.08.26
			if ( D_IO_TYPE( k ) >= OUTPUT_IO_TYPE ) { // 2003.08.26
				if ( ConsoleGuiForPres_USE != 1 ) return;
				if ( AllMode || ( DIGITAL_DATA_BUFFER_WRITE[ j ] != D_IO_OUT_VALUE(k) ) ) {
					if ( DIGITAL_SELECT_POINTER == k )	KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL2 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , D_IO_OUT_VALUE_STR(k) , DISPLAY_SIZE_X_FOR_DIGITAL2 , 1 , COLOR_DarkGray , COLOR_White , COLOR_White , COLOR_DarkBlue );
					else								KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL2 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , D_IO_OUT_VALUE_STR(k) , DISPLAY_SIZE_X_FOR_DIGITAL2 , 1 , COLOR_DarkGray , COLOR_OffWhite , COLOR_Black , COLOR_OffWhite );
					DIGITAL_DATA_BUFFER_WRITE[ j ] = D_IO_OUT_VALUE(k);
				}
			}
			else {
				if ( AllMode ) KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL2 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL2 , 1 , 13 , 13 , 13 , 13 );
			}
			if ( ConsoleGuiForPres_USE != 1 ) return;
			if ( AllMode || ( DIGITAL_DATA_BUFFER_READ[ j ] != D_IO_IN_VALUE(k) ) ) {
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL3 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , D_IO_IN_VALUE_STR(k) , DISPLAY_SIZE_X_FOR_DIGITAL3 , 1 , COLOR_DarkGray , COLOR_White , COLOR_Black , 13 );
				DIGITAL_DATA_BUFFER_READ[ j ] = D_IO_IN_VALUE(k);
			}
			i++;
			j++;
		}
	}
	if ( ConsoleGuiForPres_USE != 1 ) return;
	if ( AllMode ) {
//		for ( ; ( j < DISPLAY_COUNT_FOR_DIGITAL ) && ( ConsoleGuiForPres_USE == 1 ); j++ ) { // 2006.05.29
		for ( ; ( j < USE_DISPLAY_COUNT ) && ( ConsoleGuiForPres_USE == 1 ); j++ ) { // 2006.05.29
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL  , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL  , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL1 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL1 , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL2 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL2 , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL3 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL3 , 1 , 13 , 13 , 13 , 13 );
		}
		if ( ConsoleGuiForPres_USE != 1 ) return;
		wsprintf( buffer , "TOTAL:%4d" , DIGITAL_SELECT_COUNT() );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_SCALE_X_FOR_DIGITAL , DISPLAY_SCALE_Y_FOR_DIGITAL , buffer , 10 , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkRed );
		wsprintf( buffer , "%4d-%4d" , DISPLAY_DIGITAL_START_POINTER+1 , i  );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_SCALE_X_FOR_DIGITAL2 , DISPLAY_SCALE_Y_FOR_DIGITAL , buffer , 9 , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkBlue );
	}
}
//
//
void ConGui_Display_Clear_Digital( HDC hDC ) {
	int j;
//	for ( j = 0 ; j < DISPLAY_COUNT_FOR_DIGITAL ; j++ ) { // 2006.05.29
	for ( j = 0 ; j < USE_DISPLAY_COUNT ; j++ ) { // 2006.05.29
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL  , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL  , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL1 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL1 , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL2 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL2 , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL3 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , "" , DISPLAY_SIZE_X_FOR_DIGITAL3 , 1 , 13 , 13 , 13 , 13 );
	}
}
//
//


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//
//
//


void ConGui_Display_Analog( BOOL AllMode , HDC hDC ) {
	int i , j , k;
	char buffer[256];

	if ( !ConGui_Display_Data_Read( AllMode , 2 ) ) return;

	if ( AllMode ) {
		wsprintf( buffer, "  Analog I/O" );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_TITLE_X , DISPLAY_TITLE_Y , buffer ,  DISPLAY_TITLESIZE_X , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkRed );
	}

	for ( i = 0 , k = 0 ; k < ANALOG_TOTAL_COUNT() ; k++ ) {
		if ( A_IO_SELECT( k ) ) {
			if ( i == DISPLAY_ANALOG_START_POINTER ) break;
			i++;
		}
	}
//	for ( i = DISPLAY_ANALOG_START_POINTER , j = 0 ; ( k < ANALOG_TOTAL_COUNT() ) && ( i < ANALOG_SELECT_COUNT() ) && ( j < DISPLAY_COUNT_FOR_ANALOG ) && ( ConsoleGuiForPres_USE == 2 ); k++ ) {
	for ( i = DISPLAY_ANALOG_START_POINTER , j = 0 ; ( k < ANALOG_TOTAL_COUNT() ) && ( i < ANALOG_SELECT_COUNT() ) && ( j < USE_DISPLAY_COUNT ) && ( ConsoleGuiForPres_USE == 2 ); k++ ) {
		if ( A_IO_SELECT( k ) ) {
			//====================================================================
			READ_IO_INFORMATION_FOR_DISPLAY_DATA( 2 , k );
			//====================================================================
			if ( AllMode ) {
				wsprintf( buffer , "%4d" , i+1 );
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG  , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer       , DISPLAY_SIZE_X_FOR_ANALOG  , 1 , COLOR_White , COLOR_DarkGray , COLOR_Black , 13 );
				if ( ConsoleGuiForPres_USE != 2 ) return;
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG1 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_NAME(k) , DISPLAY_SIZE_X_FOR_ANALOG1 , 1 , COLOR_DarkGray , COLOR_White , COLOR_Black , 13 );
				ANALOG_DATA_BUFFER_WRITE[ j ] = ANALOG_DATA_BUFFER_READ[ j ] = -1;
			}
			if ( ConsoleGuiForPres_USE != 2 ) return;
//			if ( A_IO_TYPE( k ) == OUTPUT_IO_TYPE ) { // 2003.08.26
			if ( A_IO_TYPE( k ) >= OUTPUT_IO_TYPE ) { // 2003.08.26
				if ( ConsoleGuiForPres_USE != 2 ) return;
				if ( AllMode || ( ANALOG_DATA_BUFFER_WRITE[ j ] != A_IO_OUT_VALUE(k) ) ) {
					if ( ANALOG_SELECT_POINTER == k ) KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG2 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_OUT_VALUE_STR(k) ,  DISPLAY_SIZE_X_FOR_ANALOG2 , 1 , COLOR_DarkGray , COLOR_White , COLOR_White , COLOR_DarkBlue );
					else                              KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG2 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_OUT_VALUE_STR(k) ,  DISPLAY_SIZE_X_FOR_ANALOG2 , 1 , COLOR_DarkGray , COLOR_OffWhite , COLOR_Black , COLOR_OffWhite );
					ANALOG_DATA_BUFFER_WRITE[ j ] = A_IO_OUT_VALUE(k);
				}
			}
			else {
				if ( AllMode ) KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG2 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" ,  DISPLAY_SIZE_X_FOR_ANALOG2 , 1 , 13 , 13 , 13 , 13 );
			}
			if ( ConsoleGuiForPres_USE != 2 ) return;
			if ( AllMode || ( ANALOG_DATA_BUFFER_READ[ j ] != A_IO_IN_VALUE(k) ) ) {
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG3 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_IN_VALUE_STR(k) , DISPLAY_SIZE_X_FOR_ANALOG3 , 1 , COLOR_DarkGray , COLOR_White , COLOR_Black , 13 );
				ANALOG_DATA_BUFFER_READ[ j ] = A_IO_IN_VALUE(k);
			}
			if ( AllMode ) {
				if ( ConsoleGuiForPres_USE != 2 ) return;
				_dANALOG_UNIT( k , buffer );
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG4 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG4 , 1 , COLOR_DarkGray , COLOR_White , COLOR_Black , 13 );
			}
			i++;
			j++;
		}
	}
	if ( ConsoleGuiForPres_USE != 2 ) return;
	if ( AllMode ) {
//		for ( ; ( j < DISPLAY_COUNT_FOR_ANALOG ) && ( ConsoleGuiForPres_USE == 2 ) ; j++ ) {
		for ( ; ( j < USE_DISPLAY_COUNT ) && ( ConsoleGuiForPres_USE == 2 ) ; j++ ) {
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG  , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG  , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG1 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG1 , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG2 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG2 , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG3 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG3 , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG4 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG4 , 1 , 13 , 13 , 13 , 13 );
		}
		if ( ConsoleGuiForPres_USE != 2 ) return;
		wsprintf( buffer , "TOTAL:%4d" , ANALOG_SELECT_COUNT() );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_SCALE_X_FOR_ANALOG  , DISPLAY_SCALE_Y_FOR_ANALOG , buffer , 10 , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkRed );
		wsprintf( buffer , "%4d-%4d" , DISPLAY_ANALOG_START_POINTER+1 , i  );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_SCALE_X_FOR_ANALOG2 , DISPLAY_SCALE_Y_FOR_ANALOG , buffer , 9  , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkBlue );
	}
}
//
//
void ConGui_Display_Clear_Analog( HDC hDC ) {
	int j;
//	for ( j = 0 ; j < DISPLAY_COUNT_FOR_ANALOG ; j++ ) {
	for ( j = 0 ; j < USE_DISPLAY_COUNT ; j++ ) {
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG  , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG  , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG1 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG1 , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG2 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG2 , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG3 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG3 , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG4 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG4 , 1 , 13 , 13 , 13 , 13 );
	}
}
//
//
//
//

void ConGui_Display_String( BOOL AllMode , HDC hDC ) {
	int i , j , k;
	char buffer[256];

	if ( !ConGui_Display_Data_Read( AllMode , 3 ) ) return;

	if ( AllMode ) {
		wsprintf( buffer, "  String I/O" );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_TITLE_X , DISPLAY_TITLE_Y , buffer ,  DISPLAY_TITLESIZE_X , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkRed );
	}

	for ( i = 0 , k = 0 ; k < STRING_TOTAL_COUNT() ; k++ ) {
		if ( S_IO_SELECT( k ) ) {
			if ( i == DISPLAY_STRING_START_POINTER ) break;
			i++;
		}
	}
//	for ( i = DISPLAY_STRING_START_POINTER , j = 0 ; ( k < STRING_TOTAL_COUNT() ) && ( i < STRING_SELECT_COUNT() ) && ( j < DISPLAY_COUNT_FOR_STRING ) && ( ConsoleGuiForPres_USE == 3 ); k++ ) {
	for ( i = DISPLAY_STRING_START_POINTER , j = 0 ; ( k < STRING_TOTAL_COUNT() ) && ( i < STRING_SELECT_COUNT() ) && ( j < USE_DISPLAY_COUNT ) && ( ConsoleGuiForPres_USE == 3 ); k++ ) {
		if ( S_IO_SELECT( k ) ) {
			//====================================================================
			READ_IO_INFORMATION_FOR_DISPLAY_DATA( 3 , k );
			//====================================================================
			if ( AllMode ) {
				wsprintf( buffer , "%4d" , i+1 );
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING  , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , buffer       ,  DISPLAY_SIZE_X_FOR_STRING  , 1 , COLOR_White , COLOR_DarkGray , COLOR_Black , 13 );
				if ( ConsoleGuiForPres_USE != 3 ) return;
				KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING1 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , S_IO_NAME(k) ,  DISPLAY_SIZE_X_FOR_STRING1 , 1 , COLOR_DarkGray , COLOR_White , COLOR_Black , 13 );
			}
			if ( ConsoleGuiForPres_USE != 3 ) return;

			if ( AllMode || ( strcmp( STRING_DATA_BUFFER_READ[ j ] , S_IO_DATA(k) ) != 0 ) ) {
				if ( STRING_SELECT_POINTER == k ) KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING3 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , S_IO_DATA(k) ,  DISPLAY_SIZE_X_FOR_STRING3 , 1 , COLOR_DarkGray , COLOR_White , COLOR_White , COLOR_DarkBlue );
				else                              KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING3 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , S_IO_DATA(k) ,  DISPLAY_SIZE_X_FOR_STRING3 , 1 , COLOR_DarkGray , COLOR_OffWhite , COLOR_Black , COLOR_OffWhite );
//				strncpy( STRING_DATA_BUFFER_READ[ j ] , S_IO_DATA(k) , MAX_STRING_SIZE ); // 2005.05.11
				strncpy( STRING_DATA_BUFFER_READ[ j ] , S_IO_DATA(k) , MAX_STRING_SIZE+MAX_STRING_SIZE+1 ); // 2005.05.11
			}
			i++;
			j++;
		}
	}
	if ( ConsoleGuiForPres_USE != 3 ) return;
	if ( AllMode ) {
//		for ( ; ( j < DISPLAY_COUNT_FOR_STRING ) && ( ConsoleGuiForPres_USE == 3 ) ; j++ ) {
		for ( ; ( j < USE_DISPLAY_COUNT ) && ( ConsoleGuiForPres_USE == 3 ) ; j++ ) {
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING  , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , "" ,  DISPLAY_SIZE_X_FOR_STRING  , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING1 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , "" ,  DISPLAY_SIZE_X_FOR_STRING1 , 1 , 13 , 13 , 13 , 13 );
			KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING3 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , "" ,  DISPLAY_SIZE_X_FOR_STRING3 , 1 , 13 , 13 , 13 , 13 );
		}
		if ( ConsoleGuiForPres_USE != 3 ) return;
		wsprintf( buffer , "TOTAL:%4d" , STRING_SELECT_COUNT() );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_SCALE_X_FOR_STRING  , DISPLAY_SCALE_Y_FOR_STRING , buffer ,  10  , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkRed );
		wsprintf( buffer , "%4d-%4d" , DISPLAY_STRING_START_POINTER+1 , i  );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_SCALE_X_FOR_STRING2 , DISPLAY_SCALE_Y_FOR_STRING , buffer ,  9   , 1 , COLOR_White , COLOR_DarkGray , COLOR_White , COLOR_DarkBlue );
	}
}
//
//
void ConGui_Display_Clear_String( HDC hDC ) {
	int j;
//	for ( j = 0 ; j < DISPLAY_COUNT_FOR_STRING ; j++ ) {
	for ( j = 0 ; j < USE_DISPLAY_COUNT ; j++ ) {
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING  , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , "" ,  DISPLAY_SIZE_X_FOR_STRING  , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING1 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , "" ,  DISPLAY_SIZE_X_FOR_STRING1 , 1 , 13 , 13 , 13 , 13 );
		KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING3 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , "" ,  DISPLAY_SIZE_X_FOR_STRING3 , 1 , 13 , 13 , 13 , 13 );
	}
}
//
//
//
//
//
//
//
//
void ConGui_Display( BOOL AllMode , HDC hDC ) {
	EnterCriticalSection( &CR_RUN );
	if      ( ConsoleGuiForPres_USE == 1 ) ConGui_Display_Digital( AllMode , hDC );
	else if ( ConsoleGuiForPres_USE == 2 ) ConGui_Display_Analog( AllMode , hDC );
	else if ( ConsoleGuiForPres_USE == 3 ) ConGui_Display_String( AllMode , hDC );
	LeaveCriticalSection( &CR_RUN );
}	
//
//
void ConGui_Display_Clear( HWND hdlg , HDC hDC ) {
	ShowWindow( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SW_HIDE );
	if      ( ConsoleGuiForPres_USE == 1 ) ConGui_Display_Clear_Digital( hDC );
	else if ( ConsoleGuiForPres_USE == 2 ) ConGui_Display_Clear_Analog( hDC );
	else if ( ConsoleGuiForPres_USE == 3 ) ConGui_Display_Clear_String( hDC );
}	
//
//
void ConGui_Button_Click( HWND hdlg ) {
	if      ( ConsoleGuiForPres_USE == 0 ) {
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL )  , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG )   , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING )   , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL2 ) , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG2 )  , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING2 )  , SW_HIDE );
	}
	else if ( ConsoleGuiForPres_USE == 1 ) {
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL )  , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG )   , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING )   , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL2 ) , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG2 )  , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING2 )  , SW_HIDE );
	}
	else if ( ConsoleGuiForPres_USE == 2 ) {
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL )  , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG )   , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING )   , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL2 ) , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG2 )  , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING2 )  , SW_HIDE );
	}
	else if ( ConsoleGuiForPres_USE == 3 ) {
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL )  , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG )   , SW_SHOW );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING )   , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_DIGITAL2 ) , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_ANALOG2 )  , SW_HIDE );
		ShowWindow( GetDlgItem( hdlg , IDB_STRING2 )  , SW_SHOW );
	}
}
//
void ConGui_Button_Move( HWND hdlg ) {
	MoveWindow( GetDlgItem( hdlg , IDB_DIGITAL ) , DIGITAL_BUTTON_X , DIGITAL_BUTTON_Y , DIGITAL_BUTTON_XS , DIGITAL_BUTTON_YS , TRUE );
	MoveWindow( GetDlgItem( hdlg , IDB_DIGITAL2 ) , DIGITAL_BUTTON_X , DIGITAL_BUTTON_Y , DIGITAL_BUTTON_XS , DIGITAL_BUTTON_YS , TRUE );
	MoveWindow( GetDlgItem( hdlg , IDB_ANALOG  ) , ANALOG_BUTTON_X  , ANALOG_BUTTON_Y  , ANALOG_BUTTON_XS  , ANALOG_BUTTON_YS  , TRUE );
	MoveWindow( GetDlgItem( hdlg , IDB_ANALOG2  ) , ANALOG_BUTTON_X  , ANALOG_BUTTON_Y  , ANALOG_BUTTON_XS  , ANALOG_BUTTON_YS  , TRUE );
	MoveWindow( GetDlgItem( hdlg , IDB_STRING  ) , STRING_BUTTON_X  , STRING_BUTTON_Y  , STRING_BUTTON_XS  , STRING_BUTTON_YS  , TRUE );
	MoveWindow( GetDlgItem( hdlg , IDB_STRING2  ) , STRING_BUTTON_X  , STRING_BUTTON_Y  , STRING_BUTTON_XS  , STRING_BUTTON_YS  , TRUE );
	//
	MoveWindow( GetDlgItem( hdlg , IDB_POINT_ONE ) , POINT_BUTTON_X   , POINT_BUTTON_Y   , POINT_BUTTON_XS   , POINT_BUTTON_YS   , TRUE );
	MoveWindow( GetDlgItem( hdlg , IDB_POINT_REPEAT ) , POINT_BUTTON_X   , POINT_BUTTON_Y   , POINT_BUTTON_XS   , POINT_BUTTON_YS   , TRUE );
	ConGui_Button_Click( hdlg );
}
//
void ConGui_Range( HWND hdlg ) {
	if ( ConsoleGuiForPres_USE == 1 ) {
		MoveWindow( GetDlgItem( hdlg , IDC_SCROLLBAR ) , DIGITAL_SCR_X , DIGITAL_SCR_Y , DIGITAL_SCR_XS , DIGITAL_SCR_YS , TRUE );
		SetScrollRange( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , 0 , DIGITAL_SELECT_COUNT()-1 , TRUE );
		SetScrollPos(   GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , DISPLAY_DIGITAL_START_POINTER , TRUE );
	}
	else if ( ConsoleGuiForPres_USE == 2 ) {
		MoveWindow( GetDlgItem( hdlg , IDC_SCROLLBAR ) , ANALOG_SCR_X , ANALOG_SCR_Y , ANALOG_SCR_XS , ANALOG_SCR_YS , TRUE );
		SetScrollRange( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , 0 , ANALOG_SELECT_COUNT()-1 , TRUE );
		SetScrollPos(   GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , DISPLAY_ANALOG_START_POINTER , TRUE );
	}
	else if ( ConsoleGuiForPres_USE == 3 ) {
		MoveWindow( GetDlgItem( hdlg , IDC_SCROLLBAR ) , STRING_SCR_X , STRING_SCR_Y , STRING_SCR_XS , STRING_SCR_YS , TRUE );
		SetScrollRange( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , 0 , STRING_SELECT_COUNT()-1 , TRUE );
		SetScrollPos(   GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , DISPLAY_STRING_START_POINTER , TRUE );
	}
	ShowWindow( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SW_SHOW );
}
//Added by jhlee(2013.4.3)
BOOL Check_CommStatus(char cBuffer[256])
{
	int nFindPM1 = -1, nFindPM2 = -1, nFindPM3 = -1, nFindPM4 = -1, nFindPM5 = -1;
	char cTemp[256];
	char cCheckPM[256];
	double dAnalog = 0.0;
	int nDigital = 0;
	char cIOName[256];
	char cMessage[256];
	int i = 0, commstatus = 0;
	int nDIGITAL_CNT = _dDIGITAL_COUNT();

	memset(cTemp,0, sizeof(cTemp));
	memset(cCheckPM,0, sizeof(cCheckPM));

	strncpy(cTemp, cBuffer, 4);

	do 
	{
		if(strcmp(cTemp, "PM1.") == 0)
		{
			sprintf(cCheckPM,"CTC.PM1_CommStatus");
			break;
		}
		
		if(strcmp(cTemp, "PM2.") == 0)
		{
			sprintf(cCheckPM,"CTC.PM2_CommStatus");
			break;
		}
		
		if(strcmp(cTemp, "PM3.") == 0)
		{
			sprintf(cCheckPM,"CTC.PM3_CommStatus");
			break;
		}
		
		if(strcmp(cTemp, "PM4.") == 0)
		{
			sprintf(cCheckPM,"CTC.PM4_CommStatus");
			break;
		}
		
		if(strcmp(cTemp, "PM5.") == 0)
		{
			sprintf(cCheckPM,"CTC.PM5_CommStatus");
			break;
		}

		if(strcmp(cTemp, "PM6.") == 0)
		{
			sprintf(cCheckPM,"CTC.PM6_CommStatus");
			break;
		}

		return FALSE;
	} while (0);

	while(i <= nDIGITAL_CNT)
	{
		_dDIGITAL_NAME(i, cIOName);

		if(strcmp(cIOName, cCheckPM) == 0)
		{
			nDigital = _dREAD_DIGITAL(i, &commstatus);

			if(nDigital >= 7 && nDigital <= 9)
				return TRUE;
			else 
				return FALSE;
			break;
		}
		i++;
	}
	
	return FALSE;
}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

BOOL APIENTRY Console_Gui_For_MainIO_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	HBITMAP hBitmap;
	POINT		ptcursor;
	int			OldPointer;
	int			Find , i , j , sp , cnt , Type , iFind;
	int			iRes;
	double		dRes;
	char		Buffer[256];
	char		EnumBuffer[1025];
	char		SelectIOName[256];
	BOOL		bRes;
	static UINT uTimer;
	int			iMin , iMax , EnumCnt;
	int			In_Dev , Out_Dev;
	double		dMin , dMax;
	char		EnumList[1025];
	
	switch ( msg ) {
        case WM_USER:
            switch ( wParam ) {
				case WM_CLOSE:
						ConsoleGuiForLast_USE = ConsoleGuiForPres_USE;
						ConsoleGuiForPres_USE = 0;
						ConGui_ReSet_Mon();
						return TRUE;

				case WM_ACTIVATE:
						if ( ConsoleGuiForLast_USE <= 0 ) ConsoleGuiForLast_USE = 1;
						ConsoleGuiForPres_USE = ConsoleGuiForLast_USE;
						ConGui_Set_Mon();
						//
						ConGui_Button_Click( hdlg );
						// Append 2001.11.16
						InvalidateRect( hdlg , NULL , TRUE );
						return TRUE;
			}
			break;

		case WM_INITDIALOG:
			uTimer = SetTimer( hdlg , ID_CONTROL_TIMER , MONITOR_TIME , NULL );
			if ( uTimer == 0 ) MessageBox( hdlg , "Too Many Timer for Program" , "" , MB_ICONHAND | MB_OK );
			//==
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW ,  DIGITAL_BUTTON_X , DIGITAL_BUTTON_Y , DIGITAL_BUTTON_XS , DIGITAL_BUTTON_YS , hdlg , (HMENU) IDB_DIGITAL , GETHINST( hdlg ) , NULL );
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW ,  ANALOG_BUTTON_X , ANALOG_BUTTON_Y , ANALOG_BUTTON_XS , ANALOG_BUTTON_YS , hdlg , (HMENU) IDB_ANALOG , GETHINST( hdlg ) , NULL );
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW ,  STRING_BUTTON_X , STRING_BUTTON_Y , STRING_BUTTON_XS , STRING_BUTTON_YS , hdlg , (HMENU) IDB_STRING , GETHINST( hdlg ) , NULL );
			//
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW ,  DIGITAL_BUTTON_X , DIGITAL_BUTTON_Y , DIGITAL_BUTTON_XS , DIGITAL_BUTTON_YS , hdlg , (HMENU) IDB_DIGITAL2 , GETHINST( hdlg ) , NULL );
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW ,  ANALOG_BUTTON_X , ANALOG_BUTTON_Y , ANALOG_BUTTON_XS , ANALOG_BUTTON_YS , hdlg , (HMENU) IDB_ANALOG2 , GETHINST( hdlg ) , NULL );
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW ,  STRING_BUTTON_X , STRING_BUTTON_Y , STRING_BUTTON_XS , STRING_BUTTON_YS , hdlg , (HMENU) IDB_STRING2 , GETHINST( hdlg ) , NULL );
			//
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 595 ,  56 , 127 , 66 , hdlg , (HMENU) IDB_SEARCH , GETHINST( hdlg ) , NULL );
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 595 ,  56 , 127 , 66 , hdlg , (HMENU) IDB_SEARCH_N , GETHINST( hdlg ) , NULL );
			//
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , POINT_BUTTON_X ,  POINT_BUTTON_Y , POINT_BUTTON_XS , POINT_BUTTON_YS , hdlg , (HMENU) IDB_POINT_ONE , GETHINST( hdlg ) , NULL );
			CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , POINT_BUTTON_X ,  POINT_BUTTON_Y , POINT_BUTTON_XS , POINT_BUTTON_YS , hdlg , (HMENU) IDB_POINT_REPEAT , GETHINST( hdlg ) , NULL );
			//
			//<<-----------------------------------------------------------
			KGUI_STANDARD_Set_Function_Size( hdlg );
			//>>-----------------------------------------------------------
			ConGui_Button_Move( hdlg );
			if ( IO_CONTROL_REPEAT_MODE ) {
				KWIN_Item_Enable( hdlg , IDB_POINT_REPEAT );
				KWIN_Item_Hide( hdlg , IDB_POINT_ONE );
			}
			else {
				KWIN_Item_Enable( hdlg , IDB_POINT_ONE );
				KWIN_Item_Hide( hdlg , IDB_POINT_REPEAT );
			}
			ConGui_Range( hdlg );
			ConGui_Set_Mon();
			KWIN_Item_Disable( hdlg , IDB_SEARCH_N );
			KWIN_Item_Hide( hdlg , IDB_SEARCH );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_S2 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_S3 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRI2 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRI3 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRO2 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRO3 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_ANDOR1 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_ANDOR2 );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_RD );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_RA );
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_RS );

			KWIN_Item_Int_Display( hdlg , IDC_BUTTON_ALL_RD , DIGITAL_TOTAL_COUNT() );
			KWIN_Item_Int_Display( hdlg , IDC_BUTTON_ALL_RA , ANALOG_TOTAL_COUNT() );
			KWIN_Item_Int_Display( hdlg , IDC_BUTTON_ALL_RS , STRING_TOTAL_COUNT() );

			//----------------------------------
			PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
			//----------------------------------
			return TRUE;

		case WM_DRAWITEM:
			KWIN_DRAW_BUTTON_ITEM( GetModuleHandle( NULL ) , (LPDRAWITEMSTRUCT) lParam );
			return TRUE;

		case WM_TIMER :
			switch( wParam ) {
				case ID_CONTROL_TIMER :
					if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
						hDC = GetDC( hdlg );
						ConGui_Display( FALSE , hDC );
						ReleaseDC( hdlg , hDC );
					}
					break;
			}
			return TRUE;

		case WM_PAINT:
			//=============================================================================================
			// 2004.05.21
			//=============================================================================================
//			if ( !KGUI_STANDARD_Window_Signal_Mode() ) return TRUE; // 2005.02.28
			if ( !KGUI_STANDARD_Window_Signal_Mode() ) { // 2005.02.28
				BeginPaint( hdlg, &ps );
				EndPaint( hdlg , &ps );
				return TRUE;
			}
			//=============================================================================================
			BeginPaint( hdlg, &ps );
			if ( KGUI_Get_BACKGROUND() == NULL ) {
				KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
			}
			else {
				hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND" );
				if ( hBitmap ) {
					KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
					DeleteObject( hBitmap );
				}
				else {
					KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
				}
			}
			ConGui_Display( TRUE , ps.hdc );
			EndPaint( hdlg , &ps );
			ConGui_Range( hdlg );
			return TRUE;

		case WM_LBUTTONDOWN:
			GetCursorPos( &ptcursor );
			ScreenToClient( hdlg , &ptcursor );
			iFind = -1;
			if      ( ConsoleGuiForPres_USE == 1 ) {
//				for ( j = 0 ; j < DISPLAY_COUNT_FOR_DIGITAL ; j++ ) { // 2006.05.29
				for ( j = 0 ; j < USE_DISPLAY_COUNT ; j++ ) { // 2006.05.29
					if ( (	ptcursor.x >= DISPLAY_START_X_FOR_DIGITAL2 ) &&
						 (	ptcursor.x <= DISPLAY_START_X_FOR_DIGITAL2 + ( DISPLAY_SIZE_X_FOR_DIGITAL2 * 8 ) ) &&
					     (	ptcursor.y >= DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) ) &&
						 (	ptcursor.y <= DISPLAY_START_Y_FOR_DIGITAL + ( (j+1) * DISPLAY_SIZE_Y_FOR_DIGITAL ) ) ) {
						iFind = DISPLAY_DIGITAL_START_POINTER + j;
						break;
					}
				}
				if ( iFind >= 0 && iFind < DIGITAL_SELECT_COUNT() ) {
					for ( i = 0 , Find = 0 ; Find < DIGITAL_TOTAL_COUNT() ; Find++ ) {
						if ( D_IO_SELECT( Find ) ) {
							if ( i == iFind ) break;
							i++;
						}
					}
//					if ( D_IO_TYPE( Find ) == OUTPUT_IO_TYPE ) { // 2003.08.26
					if ( D_IO_TYPE( Find ) >= OUTPUT_IO_TYPE ) { // 2003.08.26
						if ( Get_Prm_READONLY_MODE() ) { // 2004.01.06
							MessageBox( hdlg , "Action Impossible(Read Only Mode)", "Impossible" , MB_ICONHAND );
							return TRUE;
						}
						DIGITAL_SELECT_POINTER = Find;
						hDC = GetDC( hdlg );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL2 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , D_IO_OUT_VALUE_STR(Find) ,  DISPLAY_SIZE_X_FOR_DIGITAL2 , 1 , COLOR_DarkGray , COLOR_White , COLOR_White , COLOR_DarkBlue );
						//
						_dDIGITAL_SHAPE( Find , &Type , &iMin , &iMax , &EnumCnt , EnumList );
						//

						//Check PM#_Commststus  Added by jhlee(2013.4.3)
						_dDIGITAL_NAME(Find, Buffer);

						bRes = Check_CommStatus(Buffer);

						if ( EnumCnt <= 0 && !bRes) {
							iRes = D_IO_IN_VALUE(Find);
							do {
								bRes = MODAL_DIGITAL_BOX2( hdlg , "Digital" , D_IO_NAME(Find) , iMin , iMax , &iRes );
								if ( bRes ) {
									if ( IO_CONTROL_CHECK_MODE ) {
										if ( IDYES != MessageBox( hdlg , "Really Adapt Data?" , "Setting Check" , MB_ICONQUESTION | MB_YESNO ) ) {
											bRes = FALSE;
										}
									}
									if ( bRes ) _dWRITE_DIGITAL_TH( Find , iRes );
								}
							} while ( IO_CONTROL_REPEAT_MODE && bRes );
						}
						else {
							strcpy( EnumBuffer , "" );
							for ( i = 0 , sp = 0 ; i < EnumCnt ; i++ ) {
								cnt = Ext_Int_From_Str( EnumList , sp , 4 );
								sp = sp + 4;
								Ext_Str_From_Str( EnumList , sp , cnt , Buffer );
								sp = sp + cnt;
								if ( i != 0 ) strcat( EnumBuffer , "|" );
								strcat( EnumBuffer , Buffer );
							}
							iRes = D_IO_IN_VALUE(Find);

							//Check PM#_Commststus  Added by jhlee(2013.4.3)
							_dDIGITAL_NAME(Find, Buffer);

							bRes = Check_CommStatus(Buffer);

							if(!bRes)
							{
								do {
									bRes = MODAL_DIGITAL_BOX1( hdlg , "Digital" , D_IO_NAME(Find) , EnumBuffer , &iRes );
									if ( bRes ) {
										if ( IO_CONTROL_CHECK_MODE ) {
											if ( IDYES != MessageBox( hdlg , "Really Adapt Data?" , "Setting Check" , MB_ICONQUESTION | MB_YESNO ) ) {
												bRes = FALSE;
											}
										}
										if ( bRes ) _dWRITE_DIGITAL_TH( Find , iRes );
									    if ( bRes ) _IO_OPERATE_PRINTF ( FALSE , "IOEDIT" , "DirectIO Edit => DIO:%s , VALUE:%d\n", D_IO_NAME(Find) , iRes );
									}
								} while ( IO_CONTROL_REPEAT_MODE && bRes );
							}
						}
						DIGITAL_SELECT_POINTER = -1;
						KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_DIGITAL2 , DISPLAY_START_Y_FOR_DIGITAL + ( j * DISPLAY_SIZE_Y_FOR_DIGITAL ) , D_IO_OUT_VALUE_STR(Find) ,  DISPLAY_SIZE_X_FOR_DIGITAL2 , 1 , COLOR_DarkGray , COLOR_OffWhite , COLOR_Black , COLOR_OffWhite );
						ReleaseDC( hdlg , hDC );
					}
				}
				else {
					return FALSE;
				}
			}
			else if ( ConsoleGuiForPres_USE == 2 ) {
//				for ( j = 0 ; j < DISPLAY_COUNT_FOR_ANALOG ; j++ ) {
				for ( j = 0 ; j < USE_DISPLAY_COUNT ; j++ ) {
					if ( (	ptcursor.x >= DISPLAY_START_X_FOR_ANALOG2 ) &&
						 (	ptcursor.x <= DISPLAY_START_X_FOR_ANALOG2 + ( DISPLAY_SIZE_X_FOR_ANALOG2 * 8 ) ) &&
					     (	ptcursor.y >= DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) ) &&
						 (	ptcursor.y <= DISPLAY_START_Y_FOR_ANALOG + ( (j+1) * DISPLAY_SIZE_Y_FOR_ANALOG ) ) ) {
						iFind = DISPLAY_ANALOG_START_POINTER + j;
						break;
					}
				}
				if ( iFind >= 0 && iFind < ANALOG_SELECT_COUNT() ) {
					for ( i = 0 , Find = 0 ; Find < ANALOG_TOTAL_COUNT() ; Find++ ) {
						if ( A_IO_SELECT( Find ) ) {
							if ( i == iFind ) break;
							i++;
						}
					}
//					if ( A_IO_TYPE( Find ) == OUTPUT_IO_TYPE ) { // 2003.08.26
					if ( A_IO_TYPE( Find ) >= OUTPUT_IO_TYPE ) { // 2003.08.26
						if ( Get_Prm_READONLY_MODE() ) { // 2004.01.06
							MessageBox( hdlg , "Action Impossible(Read Only Mode)", "Impossible" , MB_ICONHAND );
							return TRUE;
						}
						ANALOG_SELECT_POINTER = Find;
						hDC = GetDC( hdlg );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG2 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_OUT_VALUE_STR(Find) ,  DISPLAY_SIZE_X_FOR_ANALOG2 , 1 , COLOR_DarkGray , COLOR_White , COLOR_White , COLOR_DarkBlue );
						//
						_dANALOG_SHAPE( Find , &Type , &In_Dev , &Out_Dev , &dMin , &dMax );
						//
						dRes = A_IO_IN_VALUE(Find);

						//Check PM#_Commststus  Added by jhlee(2013.4.3)
						_dANALOG_NAME(Find, Buffer);

						bRes = Check_CommStatus(Buffer);

						if(!bRes)
						{
							do {
								bRes = MODAL_ANALOG_BOX1( hdlg , "Analog" , A_IO_NAME(Find) , dMin , dMax , Out_Dev , &dRes );
								if ( bRes ) {
									if ( IO_CONTROL_CHECK_MODE ) {
										if ( IDYES != MessageBox( hdlg , "Really Adapt Data?" , "Setting Check" , MB_ICONQUESTION | MB_YESNO ) ) {
											bRes = FALSE;
										}
									}
									if ( bRes ) _dWRITE_ANALOG_TH( Find , dRes );
							     	if ( bRes ) _IO_OPERATE_PRINTF ( FALSE , "IOEDIT" , "DirectIO Edit => AIO:%s , VALUE:%.3f\n", A_IO_NAME(Find) , dRes );
								}
							} while ( IO_CONTROL_REPEAT_MODE && bRes );
						}
					
						ANALOG_SELECT_POINTER = -1;
						KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_ANALOG2 , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_OUT_VALUE_STR(Find) ,  DISPLAY_SIZE_X_FOR_ANALOG2 , 1 , COLOR_DarkGray , COLOR_OffWhite , COLOR_Black , COLOR_OffWhite );
						ReleaseDC( hdlg , hDC );
					}
				}
				else {
					return FALSE;
				}
			}
			else if ( ConsoleGuiForPres_USE == 3 ) {
//				for ( j = 0 ; j < DISPLAY_COUNT_FOR_STRING ; j++ ) {
				for ( j = 0 ; j < USE_DISPLAY_COUNT ; j++ ) {
					if ( (	ptcursor.x >= DISPLAY_START_X_FOR_STRING3 ) &&
						 (	ptcursor.x <= DISPLAY_START_X_FOR_STRING3 + ( DISPLAY_SIZE_X_FOR_STRING3 * 8 ) ) &&
					     (	ptcursor.y >= DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) ) &&
						 (	ptcursor.y <= DISPLAY_START_Y_FOR_STRING + ( (j+1) * DISPLAY_SIZE_Y_FOR_STRING ) ) ) {
						iFind = DISPLAY_STRING_START_POINTER + j;
						break;
					}
				}
				if ( iFind >= 0 && iFind < STRING_SELECT_COUNT() ) {
					if ( Get_Prm_READONLY_MODE() ) { // 2004.01.06
						MessageBox( hdlg , "Action Impossible(Read Only Mode)", "Impossible" , MB_ICONHAND );
						return TRUE;
					}
					for ( i = 0 , Find = 0 ; Find < STRING_TOTAL_COUNT() ; Find++ ) {
						if ( S_IO_SELECT( Find ) ) {
							if ( i == iFind ) break;
							i++;
						}
					}
					STRING_SELECT_POINTER = Find;
					hDC = GetDC( hdlg );
					KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING3 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , S_IO_DATA(Find) ,  DISPLAY_SIZE_X_FOR_STRING3 , 1 , COLOR_DarkGray , COLOR_White , COLOR_White , COLOR_DarkBlue );
					strcpy( EnumBuffer , S_IO_DATA(Find) );

					//Check PM#_Commststus  Added by jhlee(2013.4.3)
					_dSTRING_NAME(Find, Buffer);

					bRes = Check_CommStatus(Buffer);

					if(!bRes)
					{
						do {
							bRes = MODAL_STRING_BOX1( hdlg , "String" , S_IO_NAME(Find) , EnumBuffer );
							if ( bRes ) {
								if ( IO_CONTROL_CHECK_MODE ) {
									if ( IDYES != MessageBox( hdlg , "Really Adapt Data?" , "Setting Check" , MB_ICONQUESTION | MB_YESNO ) ) {
										bRes = FALSE;
									}
								}
								if ( bRes ) _dWRITE_STRING_TH( Find , EnumBuffer );
							    if ( bRes ) _IO_OPERATE_PRINTF ( FALSE , "IOEDIT" , "DirectIO Edit => SIO:%s , VALUE:%s\n", S_IO_NAME(Find) , EnumBuffer );
							}
						} while ( IO_CONTROL_REPEAT_MODE && bRes );

					}
					STRING_SELECT_POINTER = -1;
					KGUI_DRAW_StrBox22( hDC , DISPLAY_START_X_FOR_STRING3 , DISPLAY_START_Y_FOR_STRING + ( j * DISPLAY_SIZE_Y_FOR_STRING ) , S_IO_DATA(Find) ,  DISPLAY_SIZE_X_FOR_STRING3 , 1 , COLOR_DarkGray , COLOR_OffWhite , COLOR_Black , COLOR_OffWhite );
					ReleaseDC( hdlg , hDC );
				}
				else {
					return FALSE;
				}
			}
			return TRUE;

		case WM_MOUSEWHEEL:	//123LJM
			OldPointer = (int)(short)HIWORD(wParam);
			if ( OldPointer > 0 )
				SendMessage( hdlg , WM_VSCROLL , MAKELONG(SB_PAGEUP,0) , 0 );
			else
				SendMessage( hdlg , WM_VSCROLL , MAKELONG(SB_PAGEDOWN,0) , 0 );
			return 0;

		case WM_VSCROLL:
			if      ( ConsoleGuiForPres_USE == 1 ) {
				if ( DIGITAL_SELECT_COUNT() <= 0 ) break;
				OldPointer = DISPLAY_DIGITAL_START_POINTER;
				switch( LOWORD( wParam ) ) {
//					case SB_PAGEUP   : OldPointer -= (DISPLAY_COUNT_FOR_DIGITAL-1);	break; // 2006.05.29
					case SB_PAGEUP   : OldPointer -= (USE_DISPLAY_COUNT-1);	break; // 2006.05.29
//					case SB_PAGEDOWN : OldPointer += (DISPLAY_COUNT_FOR_DIGITAL-1);	break; // 2006.05.29
					case SB_PAGEDOWN : OldPointer += (USE_DISPLAY_COUNT-1);	break; // 2006.05.29
					case SB_LINEUP   : OldPointer--;								break;
					case SB_LINEDOWN : OldPointer++;								break;
					case SB_THUMBTRACK    :
					case SB_THUMBPOSITION :	
						OldPointer = HIWORD( wParam );
						break;
				}
				if ( OldPointer < 0             ) OldPointer = 0; 
				if ( OldPointer > DIGITAL_SELECT_COUNT()-1 ) OldPointer = DIGITAL_SELECT_COUNT()-1;
				SetScrollPos( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , OldPointer , TRUE );
				if ( OldPointer != DISPLAY_DIGITAL_START_POINTER ) {
					DISPLAY_DIGITAL_START_POINTER = OldPointer;
					hDC = GetDC( hdlg );
					ConGui_Display( TRUE , hDC );
					ReleaseDC( hdlg , hDC );
					ConGui_Set_Mon();
				}
			}
			else if ( ConsoleGuiForPres_USE == 2 ) {
				if ( ANALOG_SELECT_COUNT() <= 0 ) break;
				OldPointer = DISPLAY_ANALOG_START_POINTER;
				switch( LOWORD( wParam ) ) {
//					case SB_PAGEUP   : OldPointer -= (DISPLAY_COUNT_FOR_ANALOG-1);	break;
					case SB_PAGEUP   : OldPointer -= (USE_DISPLAY_COUNT-1);	break;
//					case SB_PAGEDOWN : OldPointer += (DISPLAY_COUNT_FOR_ANALOG-1);	break;
					case SB_PAGEDOWN : OldPointer += (USE_DISPLAY_COUNT-1);	break;
					case SB_LINEUP   : OldPointer--;								break;
					case SB_LINEDOWN : OldPointer++;								break;
					case SB_THUMBTRACK    :
					case SB_THUMBPOSITION :	
						OldPointer = HIWORD( wParam );
						break;
				}
				if ( OldPointer < 0            ) OldPointer = 0; 
				if ( OldPointer > ANALOG_SELECT_COUNT()-1 ) OldPointer = ANALOG_SELECT_COUNT()-1;
				SetScrollPos( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , OldPointer , TRUE );
				if ( OldPointer != DISPLAY_ANALOG_START_POINTER ) {
					DISPLAY_ANALOG_START_POINTER = OldPointer;
					hDC = GetDC( hdlg );
					ConGui_Display( TRUE , hDC );
					ReleaseDC( hdlg , hDC );
					ConGui_Set_Mon();
				}
			}
			else if ( ConsoleGuiForPres_USE == 3 ) {
				if ( STRING_SELECT_COUNT() <= 0 ) break;
				OldPointer = DISPLAY_STRING_START_POINTER;
				switch( LOWORD( wParam ) ) {
//					case SB_PAGEUP   : OldPointer -= (DISPLAY_COUNT_FOR_STRING-1);	break;
					case SB_PAGEUP   : OldPointer -= (USE_DISPLAY_COUNT-1);	break;
//					case SB_PAGEDOWN : OldPointer += (DISPLAY_COUNT_FOR_STRING-1);	break;
					case SB_PAGEDOWN : OldPointer += (USE_DISPLAY_COUNT-1);	break;
					case SB_LINEUP   : OldPointer--;								break;
					case SB_LINEDOWN : OldPointer++;								break;
					case SB_THUMBTRACK    :
					case SB_THUMBPOSITION :	
						OldPointer = HIWORD( wParam );
						break;
				}
				if ( OldPointer < 0            ) OldPointer = 0; 
				if ( OldPointer > STRING_SELECT_COUNT()-1 ) OldPointer = STRING_SELECT_COUNT()-1;
				SetScrollPos( GetDlgItem( hdlg , IDC_SCROLLBAR ) , SB_CTL , OldPointer , TRUE );
				if ( OldPointer != DISPLAY_STRING_START_POINTER ) {
					DISPLAY_STRING_START_POINTER = OldPointer;
					hDC = GetDC( hdlg );
					ConGui_Display( TRUE , hDC );
					ReleaseDC( hdlg , hDC );
					ConGui_Set_Mon();
				}
			}
			return TRUE;

		case WM_COMMAND:
            switch ( LOWORD( wParam ) ) {
				case IDB_POINT_ONE:
					IO_CONTROL_REPEAT_MODE = TRUE;
					KWIN_Item_Enable( hdlg , IDB_POINT_REPEAT );
					KWIN_Item_Hide( hdlg , IDB_POINT_ONE );
					return TRUE;

				case IDB_POINT_REPEAT:
					IO_CONTROL_REPEAT_MODE = FALSE;
					KWIN_Item_Enable( hdlg , IDB_POINT_ONE );
					KWIN_Item_Hide( hdlg , IDB_POINT_REPEAT );
					return TRUE;

				case IDB_DIGITAL2:
				case IDB_ANALOG2:
				case IDB_STRING2:
					return FALSE;

				case IDB_DIGITAL:
					if ( ConsoleGuiForPres_USE != 1 ) {
						hDC = GetDC( hdlg );
						ConGui_Display_Clear( hdlg , hDC );
						ConsoleGuiForPres_USE = 1;
						ConGui_Display( TRUE , hDC );
						ReleaseDC( hdlg , hDC );
						ConGui_Button_Click( hdlg );
						ConGui_Range( hdlg );
						ConGui_Set_Mon();
					}
					return TRUE;

				case IDB_ANALOG:
					if ( ConsoleGuiForPres_USE != 2 ) {
						hDC = GetDC( hdlg );
						ConGui_Display_Clear( hdlg , hDC );
						ConsoleGuiForPres_USE = 2;
						ConGui_Display( TRUE , hDC );
						ReleaseDC( hdlg , hDC );
						ConGui_Button_Click( hdlg );
						ConGui_Range( hdlg );
						ConGui_Set_Mon();
					}
					return TRUE;

				case IDB_STRING:
					if ( ConsoleGuiForPres_USE != 3 ) {
						hDC = GetDC( hdlg );
						ConGui_Display_Clear( hdlg , hDC );
						ConsoleGuiForPres_USE = 3;
						ConGui_Display( TRUE , hDC );
						ReleaseDC( hdlg , hDC );
						ConGui_Button_Click( hdlg );
						ConGui_Range( hdlg );
						ConGui_Set_Mon();
					}
					return TRUE;

				case IDC_BUTTON_SEARCH_S1 :
					SEARCH_SET_STRING( hdlg , 0 , IDC_BUTTON_SEARCH_S1 );
					return TRUE;
				case IDC_BUTTON_SEARCH_S2 :
					SEARCH_SET_STRING( hdlg , 1 , IDC_BUTTON_SEARCH_S2 );
					return TRUE;
				case IDC_BUTTON_SEARCH_S3 :
					SEARCH_SET_STRING( hdlg , 2 , IDC_BUTTON_SEARCH_S3 );
					return TRUE;

				case IDC_BUTTON_SEARCH_DRI1 :
					SEARCH_SET_DRIVER_IN( hdlg , 0 , IDC_BUTTON_SEARCH_DRI1 );
					return TRUE;
				case IDC_BUTTON_SEARCH_DRI2 :
					SEARCH_SET_DRIVER_IN( hdlg , 1 , IDC_BUTTON_SEARCH_DRI2 );
					return TRUE;
				case IDC_BUTTON_SEARCH_DRI3 :
					SEARCH_SET_DRIVER_IN( hdlg , 2 , IDC_BUTTON_SEARCH_DRI3 );
					return TRUE;
				case IDC_BUTTON_SEARCH_DRO1 :
					SEARCH_SET_DRIVER_OUT( hdlg , 0 , IDC_BUTTON_SEARCH_DRO1 );
					return TRUE;
				case IDC_BUTTON_SEARCH_DRO2 :
					SEARCH_SET_DRIVER_OUT( hdlg , 1 , IDC_BUTTON_SEARCH_DRO2 );
					return TRUE;
				case IDC_BUTTON_SEARCH_DRO3 :
					SEARCH_SET_DRIVER_OUT( hdlg , 2 , IDC_BUTTON_SEARCH_DRO3 );
					return TRUE;

				case IDC_BUTTON_SEARCH_ANDOR1 :
					SEARCH_SET_ANDOR( hdlg , 0 , IDC_BUTTON_SEARCH_ANDOR1 );
					return TRUE;
				case IDC_BUTTON_SEARCH_ANDOR2 :
					SEARCH_SET_ANDOR( hdlg , 1 , IDC_BUTTON_SEARCH_ANDOR2 );
					return TRUE;

				case IDC_BUTTON_SEARCH_RD :
					return TRUE;
				case IDC_BUTTON_SEARCH_RA :
					return TRUE;
				case IDC_BUTTON_SEARCH_RS :
					return TRUE;

				case IDB_SEARCH :
					SEARCH_CONTROL( hdlg );
					//
					hDC = GetDC( hdlg );
					ConGui_Display_Clear( hdlg , hDC );
					ConGui_Display( TRUE , hDC );
					ReleaseDC( hdlg , hDC );
					ConGui_Range( hdlg );
					ConGui_Set_Mon();
					//
					return TRUE;

				case IDC_BUTTON_MATCH :
					if ( MATCH_CONTROL == 0 ) MATCH_CONTROL = 1;
					else                      MATCH_CONTROL = 0;
					switch( MATCH_CONTROL ) {
					case 0 :
						KWIN_Item_String_Display( hdlg , IDC_BUTTON_MATCH , "First Match" );
						break;
					case 1 :
						KWIN_Item_String_Display( hdlg , IDC_BUTTON_MATCH , "Keyword Match" );
						break;
					}
					KWIN_Item_Enable( hdlg , IDB_SEARCH );
					KWIN_Item_Hide( hdlg , IDB_SEARCH_N );
					return TRUE;

				case IDCANCEL:
					//EndDialog( hdlg , 0 );
					return TRUE;
			}
			break;

		case WM_DESTROY:
			ConsoleGuiForPres_USE = 0;
			if ( uTimer ) KillTimer( hdlg , uTimer );
			return TRUE;

	}
	return FALSE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void ConGui_ReSet_Mon() {
}
//---------------------------------------------------------------------------------------
void ConGui_Set_Mon() {
	int i , j , k;
	char buffer[32];

	EnterCriticalSection( &CR_RUN );
	switch ( ConsoleGuiForPres_USE ) {
		case 1 :
				sprintf( GROUP_MONITOR_BUFFER , "T%d" , MONITOR_TIME );
				for ( i = 0 , k = 0 ; k < DIGITAL_TOTAL_COUNT() ; k++ ) {
					if ( D_IO_SELECT( k ) ) {
						if ( i == DISPLAY_DIGITAL_START_POINTER ) break;
						i++;
					}
				}
//				for ( i = DISPLAY_DIGITAL_START_POINTER , j = 0 ; ( k < DIGITAL_TOTAL_COUNT() ) && ( i < DIGITAL_SELECT_COUNT() ) && ( j < DISPLAY_COUNT_FOR_DIGITAL ); k++ ) { // 2006.05.29
				for ( i = DISPLAY_DIGITAL_START_POINTER , j = 0 ; ( k < DIGITAL_TOTAL_COUNT() ) && ( i < DIGITAL_SELECT_COUNT() ) && ( j < USE_DISPLAY_COUNT ); k++ ) { // 2006.05.29
					if ( D_IO_SELECT( k ) ) {
//						if ( D_IO_TYPE( k ) == OUTPUT_IO_TYPE ) { // 2003.08.26
						if ( D_IO_TYPE( k ) >= OUTPUT_IO_TYPE ) { // 2003.08.26
							sprintf( buffer , "d%d" , k );
							strcat( GROUP_MONITOR_BUFFER , buffer );
						}
						sprintf( buffer , "D%d" , k );
						strcat( GROUP_MONITOR_BUFFER , buffer );
						i++;
						j++;
					}
				}
				_dGROUP_IO_UNIQUE_MONITORING( GROUP_MONITOR_BUFFER );
				break;

		case 2 :
				sprintf( GROUP_MONITOR_BUFFER , "T%d" , MONITOR_TIME );
				for ( i = 0 , k = 0 ; k < ANALOG_TOTAL_COUNT() ; k++ ) {
					if ( A_IO_SELECT( k ) ) {
						if ( i == DISPLAY_ANALOG_START_POINTER ) break;
						i++;
					}
				}
//				for ( i = DISPLAY_ANALOG_START_POINTER , j = 0 ; ( k < ANALOG_TOTAL_COUNT() ) && ( i < ANALOG_SELECT_COUNT() ) && ( j < DISPLAY_COUNT_FOR_ANALOG ) ; k++ ) {
				for ( i = DISPLAY_ANALOG_START_POINTER , j = 0 ; ( k < ANALOG_TOTAL_COUNT() ) && ( i < ANALOG_SELECT_COUNT() ) && ( j < USE_DISPLAY_COUNT ) ; k++ ) {
					if ( A_IO_SELECT( k ) ) {
//						if ( A_IO_TYPE( i ) == OUTPUT_IO_TYPE ) { // 2003.08.26
						if ( A_IO_TYPE( i ) >= OUTPUT_IO_TYPE ) { // 2003.08.26
							sprintf( buffer , "a%d" , k );
							strcat( GROUP_MONITOR_BUFFER , buffer );
						}
						sprintf( buffer , "A%d" , k );
						strcat( GROUP_MONITOR_BUFFER , buffer );
						i++;
						j++;
					}
				}
				_dGROUP_IO_UNIQUE_MONITORING( GROUP_MONITOR_BUFFER );
				break;

		case 3 :
				sprintf( GROUP_MONITOR_BUFFER , "T%d" , MONITOR_TIME );
				for ( i = 0 , k = 0 ; k < STRING_TOTAL_COUNT() ; k++ ) {
					if ( S_IO_SELECT( k ) ) {
						if ( i == DISPLAY_STRING_START_POINTER ) break;
						i++;
					}
				}
//				for ( i = DISPLAY_STRING_START_POINTER , j = 0 ; ( k < STRING_TOTAL_COUNT() ) && ( i < STRING_SELECT_COUNT() ) && ( j < DISPLAY_COUNT_FOR_STRING ) ; k++ ) {
				for ( i = DISPLAY_STRING_START_POINTER , j = 0 ; ( k < STRING_TOTAL_COUNT() ) && ( i < STRING_SELECT_COUNT() ) && ( j < USE_DISPLAY_COUNT ) ; k++ ) {
					if ( S_IO_SELECT( k ) ) {
						sprintf( buffer , "S%d" , k );
						strcat( GROUP_MONITOR_BUFFER , buffer );
						i++;
						j++;
					}
				}
				_dGROUP_IO_UNIQUE_MONITORING( GROUP_MONITOR_BUFFER );
				break;
	}
	LeaveCriticalSection( &CR_RUN );
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int i;
	char	NAME1[ 128 + 1 ];
	char	NAMEx[ 128 + 1 ];

	HWND hWnd;
	//----------------------------------
//	REGIST_FILE_INFO( "Gui IO Manager 2.0" );
//	REGIST_FILE_INFO( "Gui IO Manager 2.1(2001.11.16)" ); // Update Refresh Part
//	REGIST_FILE_INFO( "Gui IO Manager 2.2(2001.12.09)" ); // Update Read Way
//	REGIST_FILE_INFO( "Gui IO Manager 2.21(2001.12.19)" ); // Update Bug(Init IO Mode)
//	REGIST_FILE_INFO( "Gui IO Manager 3.0(2002.01.01)" ); // CimSqCon Change Rebuild
//	REGIST_FILE_INFO( "Gui IO Manager 3.1(2002.06.13)" ); // Click Button Append
//	REGIST_FILE_INFO( "Gui IO Manager 3.2(2002.06.17)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui IO Manager 3.3(2003.07.12)" ); // Append Wait Receive Spawn Message
//	REGIST_FILE_INFO( "Gui IO Manager 3.4(2003.08.26)" ); // IO Type Expand Bug Update
//	REGIST_FILE_INFO( "Gui IO Manager 3.5(2004.01.06)" ); // Expand for Huge Model & Update Access Control Part
//	REGIST_FILE_INFO( "Gui IO Manager 3.51(2004.01.19)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui IO Manager 3.52(2004.08.10)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui IO Manager 3.53(2005.02.28)" ); // CPU OverHead Bug Update
	REGIST_FILE_INFO( "Gui IO Manager 3.6(2005.05.11)" ); // String Size Change 99 to 199
	//----------------------------------
	//----------------------------------
	while( PROGRAM_GUI_SPAWNING == 0 ) { _sleep(100); } // 2003.07.12
	//----------------------------------
	//----------------------------------
	InitializeCriticalSection( &CR_RUN );
	//----------------------------------
	InitCommonControls();
	//----------------------------------
	KGUI_Set_BACKGROUND( PROGRAM_PARAMETER_READ() );

	IO_CONTROL_CHECK_MODE = FALSE;
	IO_CONTROL_REPEAT_MODE = FALSE;
	STR_SEPERATE_CHAR( PROGRAM_ARGUMENT_READ() , '|' , NAME1 , NAMEx , 128 );
	for ( i = 0 ; i < (signed) strlen( NAME1 ) ; i++ ) {
		if ( tolower(NAME1[i]) == 'r' ) IO_CONTROL_REPEAT_MODE = TRUE;
		if ( tolower(NAME1[i]) == 'c' ) IO_CONTROL_CHECK_MODE = TRUE;
	}
	//
	//>>-------------------------------------------------------------------------

	if ( !READ_IO_INFORMATION() ) return SYS_ABORTED;

	KGUI_PALETTE_LOADING_SYSTEM( NULL );

	if ( !KWIN_DIALOG_DRIVER_LOADING() ) {
		//_IO_CIM_PRINTF( "{%s} - Dialog Driver Loading Error\n" , PROGRAM_FUNCTION_READ() );
	}
	
	hWnd = GET_MAIN_WINDOW();
	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Console_Gui_For_MainIO_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Console_Gui_For_MainIO_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Console_Gui_For_MainIO_Proc , (LPARAM) NULL ); // 2004.01.19
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , GET_MAIN_WINDOW() , Console_Gui_For_MainIO_Proc , (LPARAM) NULL ); // 2002.06.17

	//----------------------------------
	PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
	//----------------------------------
	return( SYS_SUCCESS );
}

//
//
//
//
//
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	int	i;
	char	NAME1[ 128 + 1 ];
	char	NAMEx[ 128 + 1 ];
	char	NAMEx2[ 128 + 1 ];
	char	NAME2[ 128 + 1 ];
	char	NAME3[ 128 + 1 ];

	STR_SEPERATE_CHAR( PROGRAM_ARGUMENT_READ() , '|' , NAME1 , NAMEx2 , 128 );
	STR_SEPERATE_CHAR( NAMEx2 , '|' , NAMEx , NAME1 , 128 );
	STR_SEPERATE_CHAR( NAMEx , ':' , NAME2 , NAME3 , 128 ); // 2003.12.28
	//
	Set_Prm_READONLY_LEVEL_IO_NAME( NAME3 ); // 2003.12.28
	//
	Set_Prm_READONLY_LEVEL( 0 );

	for ( i = 0 ; i < (signed) strlen( NAME2 ) ; i++ ) {
		if ( tolower(NAME2[i]) == '0' ) Set_Prm_READONLY_LEVEL( 0 );
		if ( tolower(NAME2[i]) == '1' ) Set_Prm_READONLY_LEVEL( 1 );
		if ( tolower(NAME2[i]) == '2' ) Set_Prm_READONLY_LEVEL( 2 );
		if ( tolower(NAME2[i]) == '3' ) Set_Prm_READONLY_LEVEL( 3 );
		if ( tolower(NAME2[i]) == '4' ) Set_Prm_READONLY_LEVEL( 4 );
		if ( tolower(NAME2[i]) == '5' ) Set_Prm_READONLY_LEVEL( 5 );
		if ( tolower(NAME2[i]) == '6' ) Set_Prm_READONLY_LEVEL( 6 );
		if ( tolower(NAME2[i]) == '7' ) Set_Prm_READONLY_LEVEL( 7 );
		if ( tolower(NAME2[i]) == '8' ) Set_Prm_READONLY_LEVEL( 8 );
		if ( tolower(NAME2[i]) == '9' ) Set_Prm_READONLY_LEVEL( 9 );
	}
	//
	USE_DISPLAY_COUNT = atoi( NAME1 );
	if ( USE_DISPLAY_COUNT < 10 )                USE_DISPLAY_COUNT = 10;
	if ( USE_DISPLAY_COUNT > MAX_DISPLAY_COUNT ) USE_DISPLAY_COUNT = MAX_DISPLAY_COUNT;
}
//---------------------------------------------------------------------------------------
