//################################################################################
//  2008/08/27 기능추가.
//	  1) HeaderItem 추가 
//        수정전 : String IO 1개, Digital IO 1개
//        수정후 : String IO 2개, Digital IO 3개,  Analog IO 1개
//                 < Recommend IO >
//					String_IO  : Recipe_Name      , 	CURRENT_LOT_ID
//					Digital_IO : PM_WAFER_STATUS_A,     PM_WAFER_STATUS_B,    ACC_WAFERCOUNT
//					Analog_IO  : ACC_THICKNESS   
//
//    2) Graph All Delete 추가.(Popup Menu로 구현)
//		 : 이미 Select된 Graph Item들을 한번에 모두 UnSelect 하는 기능 추가.
//
//  검색 키워드 : "2008/08/27"
//################################################################################

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <SYS/STAT.H>
#include <direct.h>

#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>
#include <Kutlanal.h>

#include "resource.h"

#include "cimseqnc.h"
#include "iodefine.h"
#include "ctl_code.h"
#include "plt_code.h"
#include "plt_parm.h"
#include "dat_code.h"

//========================================================================================
BOOL REMOTE_MONITOR_MODE = FALSE; // 2005.03.15
char REMOTE_MONITOR_FOLDER[256]; // 2005.03.15
//========================================================================================
long DISPLAY_START_TIME;
long DATALOG_START_TIME;
//========================================================================================
int  DISPLAY_CONTROL;
int	 DISPLAY_TIME_MODE;
//========================================================================================
int  DATALOG_CONTROL;
int  DATALOG_FILE;
int  DATALOG_TIME;
int  DATALOG_TIME_OLD;
int  DATALOG_AUTO_DELETE_TIME;
int  DATALOG_AUTO_DELETE_COUNT; // 2004.01.14
char DATALOG_DIRECTORY[256];
BOOL DATALOG_DIRECTORY_SEP; // 2004.01.09

//---> Modify 2008/08/27  HeaderItem 추가 by KSM
//int  DATALOG_STRING_IO; 
int  DATALOG_STRING_IO1; 
int  DATALOG_STRING_IO2; 
//---> End    2008/08/27  HeaderItem 추가 by KSM

//---> Modify 2008/08/27  HeaderItem 추가 by KSM
//int  DATALOG_DIGITAL_IO; // 2005.08.01 JJY
int  DATALOG_DIGITAL_IO1;
int  DATALOG_DIGITAL_IO2;
int  DATALOG_DIGITAL_IO3;
//---> End    2008/08/27  HeaderItem 추가 by KSM

//---> Add    2008/08/27  HeaderItem 추가 by KSM
int  DATALOG_ANALOG_IO1;
//---> End    2008/08/27  HeaderItem 추가 by KSM


char INFORMATION_FILE[256];
char WINDOW_TITLE[256];
//========================================================================================
int  HEADER_MESSAGE_IO = -1;
int  HEADER_MESSAGE_DISPLAY_START_X = 0;
int  HEADER_MESSAGE_DISPLAY_START_Y = 0;
char HEADER_MESSAGE_STRING[256];
int  HEADER_MESSAGE_SIZE = 10;
//========================================================================================
int	 AUTO_SCALE_POLL_TIME = 1;
//========================================================================================
BOOL A_IO_MONITOR_REFORM = FALSE;
//========================================================================================
void	*A_IOP[ MAX_ANALOG_TABLE ];
int		ANALOG_CNT	= 0;
//========================================================================================
void	*E_IOP[ MAX_EVENT_TABLE ];
int		EVENT_CNT	= 0;
//========================================================================================
//========================================================================================
int		DISP_LAST_POINTER[2];
int		DISP_LAST_OLD_POINTER[2] = { -1 , -1 };
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
#define		DISPLAY_COUNT_FOR_ANALOG			5

#define		DISPLAY_START_Y_FOR_ANALOG_COUNT	398
#define		DISPLAY_START_Y_FOR_ANALOG			426
#define		DISPLAY_SIZE_Y_FOR_ANALOG			28

#define		DISPLAY_START_X_FOR_ANALOG_COUNT	100
#define		DISPLAY_START_X_FOR_ANALOG_NO		47
#define		DISPLAY_START_X_FOR_ANALOG_NAME		86

#define		DISPLAY_START_X_FOR_ANALOG_DATA		220

#define		DISPLAY_START_X_FOR_ANALOG_DISPLAY	319

#define		DISPLAY_START_X_FOR_ANALOG_TIME		377
#define		DISPLAY_START_X_FOR_ANALOG_MIN		427
#define		DISPLAY_START_X_FOR_ANALOG_MAX		511

#define		DISPLAY_START_X_FOR_ANALOG_INTERVAL	593
#define		DISPLAY_START_X_FOR_ANALOG_HELP		640

#define		DISPLAY_SIZE_X_FOR_ANALOG_NO		3
#define		DISPLAY_SIZE_X_FOR_ANALOG_NAME		16
#define		DISPLAY_SIZE_X_FOR_ANALOG_DATA		10

#define		DISPLAY_SIZE_X_FOR_ANALOG_DISPLAY	5
#define		DISPLAY_SIZE_X_FOR_ANALOG_TIME		4
#define		DISPLAY_SIZE_X_FOR_ANALOG_MIN		8
#define		DISPLAY_SIZE_X_FOR_ANALOG_MAX		8
#define		DISPLAY_SIZE_X_FOR_ANALOG_INTERVAL	4
#define		DISPLAY_SIZE_X_FOR_ANALOG_HELP		27

int DISPLAY_ANALOG_START_POINTER	= 0;


//================================================================================================================
BOOL Util_Prepare_Folder( char *FolderName ) { // 2005.03.17
	int i , len;
	char Buffer[512];
	char imsi[512];

	len = strlen( FolderName );
	if ( len <= 0 ) return TRUE;

	if ( _getcwd( Buffer, 511 ) != NULL ) {
		for ( i = 0 ; i < len ; i++ ) {
			if ( ( ( FolderName[i] == '\\' ) || ( FolderName[i] == '/' ) ) && ( i != 0 ) ) {
				if ( _chdir( imsi ) ) {
					if ( _mkdir( imsi ) != 0 ) return FALSE;
				}
				else {
					_chdir( Buffer );
				}
			}
			imsi[i] = FolderName[i];
			imsi[i+1] = 0;
		}
		if ( _chdir( imsi ) ) {
			if ( _mkdir( imsi ) != 0 ) return FALSE;
		}
		else {
			_chdir( Buffer );
		}
		return TRUE;
	}
	return FALSE;
}
//========================================================================================
void Console_Time_Display( int mode ) {
	if ( mode >= 0 && mode <= 4 ) DISPLAY_TIME_MODE = mode;
}
//
void Get_Long_Time_To_String( long time_data , char *buffer ) {
	long hour , min , sec;
	double d;
	struct tm		*Pres_Time;
	
	if ( time_data < 0 ) {
		strcpy( buffer , "" );
		return;
	}

	switch( DISPLAY_TIME_MODE ) {
		case 1 :
				d = (double) time_data / 3600;
				sprintf( buffer , "%8.5lf" , d );
				break;
		case 2 :
				d = (double) time_data / 60;
				sprintf( buffer , "%8.4lf" , d );
				break;
		case 3 :
				sprintf( buffer , "%8ld" , time_data );
				break;
		case 4 :
				sec = DISPLAY_START_TIME + time_data;
				Pres_Time = localtime( &sec );
				sprintf( buffer , "%02d:%02d:%02d" , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
				break;
		default:
				hour =   time_data / 3600;
				min  = ( time_data % 3600 ) / 60;
				sec  = ( time_data % 60 );
				sprintf( buffer , "%d:%d:%d" , hour , min , sec );
				break;
	}
}
//
//
long Get_Time_Data_From_String( char *b0 ) {
	char b1[32],b2[32],b3[32],bx[32];
	STR_SEPERATE_CHAR( b0 , ':' , b1 , bx , 31 );
	if ( strlen( bx ) == 0 ) return atol(b1);
	STR_SEPERATE_CHAR( bx , ':' , b2 , b3 , 31 );
	return( (atol(b1) * 3600) + (atol(b2) * 60) + atol(b3) );
}
//

long Elapsed_Time( char *buffer ) {
	time_t finish;
	long elapsed_time , hour , min , sec;
	time( &finish );
	elapsed_time = (long) difftime( finish , DISPLAY_START_TIME );
	hour =   elapsed_time / 3600;
	min  = ( elapsed_time % 3600 ) / 60;
	sec  = ( elapsed_time % 60 );
	sprintf( buffer , "%d:%d:%d" , hour , min , sec );
	return elapsed_time;
}

void Elapsed_Time_with_Datalog( int mode , char *buffer ) {
	time_t finish;
	long elapsed_time , hour , min , sec;
	time( &finish );
	elapsed_time = (long) difftime( finish , DATALOG_START_TIME );
	switch ( mode ) {
	case 0 :
	case 1 :
		hour =   elapsed_time / 3600;
		min  = ( elapsed_time % 3600 ) / 60;
		sec  = ( elapsed_time % 60 );
		sprintf( buffer , "%d:%d:%d" , hour , min , sec );
		break;
	case 2 :
	case 3 :
		sprintf( buffer , "%d" , elapsed_time );
		break;
	}
}

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
char *DATALOG_DIRECTORY_NAME() {
	return DATALOG_DIRECTORY;
}

BOOL DATALOG_DIRECTORY_SEPARATE() {
	return DATALOG_DIRECTORY_SEP;
}

//========================================================================================
//========================================================================================
void Console_Gui_Parameter_Save() {
	FILE *fpt;
	int i;
	if ( *INFORMATION_FILE == 0 ) return;

	if ( SYSTEM_LOGSKIP_STATUS() ) return; // 2004.05.21

	if ( ( fpt = fopen( INFORMATION_FILE , "w" ) ) == NULL ) return;
	fprintf( fpt , "$_PLOT_BIG      %4d %4d\n" ,
				V_PLOTTER_BIG( SCREEN_1 ) , V_PLOTTER_BIG( SCREEN_2 ) );
	fprintf( fpt , "$_PLOT_BACK     %4d %4d\n" ,
				V_PLOTTER_BACK_DRAW( SCREEN_1 ) , V_PLOTTER_BACK_DRAW( SCREEN_2 ) );
	fprintf( fpt , "$_PLOT_BACK_COLOR %4d %4d\n" ,
				V_PLOTTER_BACK_COLOR( SCREEN_1 ) , V_PLOTTER_BACK_COLOR( SCREEN_2 ) );
	fprintf( fpt , "$_PLOT_POINT    %4d %4d\n" ,
				V_PLOTTER_POINT( SCREEN_1 ) , V_PLOTTER_POINT( SCREEN_2 ) );
	fprintf( fpt , "$_PLOT_LINE     %4d %4d\n" ,
				V_LINE_DRAW_TYPE( SCREEN_1 ) , V_LINE_DRAW_TYPE( SCREEN_2 ) );
	fprintf( fpt , "$_PLOT_COLOR_1  %4d %4d %4d %4d\n" ,
				V_PLOTTER_COLOR( SCREEN_1 , 0 ) , V_PLOTTER_COLOR( SCREEN_1 , 1 ) , V_PLOTTER_COLOR( SCREEN_1 , 2 ) , V_PLOTTER_COLOR( SCREEN_1 , 3 ) );
	fprintf( fpt , "$_PLOT_COLOR_2  %4d %4d %4d %4d\n" ,
				V_PLOTTER_COLOR( SCREEN_2 , 0 ) , V_PLOTTER_COLOR( SCREEN_2 , 1 ) , V_PLOTTER_COLOR( SCREEN_2 , 2 ) , V_PLOTTER_COLOR( SCREEN_2 , 3 ) );
	fprintf( fpt , "$_DATALOG_FILE_SAVE_TYPE  %4d\n" , V_PLOTTER_SAVE_TAB_TYPE() );
	fprintf( fpt , "$_DATALOG_TIME  %4d\n" , DATALOG_TIME );

	for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
			fprintf( fpt , "%20s %4d %20.3lf %20.3lf %4d %4d %4d\n" ,
					A_IO_NAME(i) ,	A_IO_SAVE(i) ,	A_IO_MIN(i) ,	A_IO_MAX(i) ,
					A_IO_PLT_DISPLAY(i) ,	A_IO_PLT_POLL(i) ,	A_IO_PLT_DEV(i) );
	}
	fclose(fpt);
}

//========================================================================================
//========================================================================================
void ConGui_Datalog_Display( HDC hDC , int mode ) {
	switch ( mode ) {
		case 1 :
			if ( DATA_LOGGING_STATUS() ) {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , "DataLog-Running" , 15 , 0 , COLOR_White , COLOR_DarkGray , COLOR_LightGray , COLOR_Black );
			}
			else {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , "DataLog-Stopped" , 15 , 0 , COLOR_DarkGray , COLOR_White , COLOR_Black , COLOR_LightGray );
			}
			break;

		case 2 :
			if ( DATA_LOGGING_STATUS() ) {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , DATA_LOGGING_FILENAME() , 15 , 0 , COLOR_DarkGray , COLOR_White , COLOR_Black , COLOR_LightGray );
			}
			else {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , "DataLog-Stopped" , 15 , 0 , COLOR_DarkGray , COLOR_White , COLOR_Black , COLOR_LightGray );
			}
			break;

		case 3 :
			if ( DATA_LOGGING_STATUS() ) {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , DATA_LOGGING_FILENAME() , 15 , 0 , COLOR_White , COLOR_DarkGray , COLOR_LightGray , COLOR_Black );
			}
			else {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , "DataLog-Stopped" , 15 , 0 , COLOR_DarkGray , COLOR_White , COLOR_Black , COLOR_LightGray );
			}
			break;

		default :
			if ( DATA_LOGGING_STATUS() ) {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , "DataLog-Running" , 15 , 0 , COLOR_DarkGray , COLOR_White , COLOR_Black , COLOR_LightGray );
			}
			else {
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT + 80 , DISPLAY_START_Y_FOR_ANALOG_COUNT , "DataLog-Stopped" , 15 , 0 , COLOR_DarkGray , COLOR_White , COLOR_Black , COLOR_LightGray );
			}
			break;
	}
}
//========================================================================================
void ConGui_Display_HeadTime( HDC hDC , int Mode ) {
	char buffer[256];
	char buffer2[256];
	struct tm		*Pres_Time;
	int c;
	Pres_Time = localtime( &DISPLAY_START_TIME );
	sprintf( buffer2 , "%04d/%02d/%02d %02d:%02d:%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
	wsprintf( buffer , "Start Time = %s" , buffer2 );
	if ( Mode < 0 ) {
		KGUI_DRAW_StrBox2( hDC , 18 , 12 , buffer , strlen(buffer) - 6 , 1 , COLOR_White , COLOR_DarkGray , COLOR_Black , COLOR_LightGray );
	}
	else {
		if      ( Mode == 0 ) c = COLOR_White;
		else if ( Mode == 1 ) c = COLOR_LightGray;
		else if ( Mode == 2 ) c = COLOR_MediumGray;
		else if ( Mode == 3 ) c = COLOR_DarkGray;
		else                  c = COLOR_Black;
		KGUI_DRAW_StrBox2( hDC , 18 , 12 , buffer , strlen(buffer) - 6 , 1 , COLOR_White , COLOR_Black , c , COLOR_PaleGreen );
	}
}
//========================================================================================
void ConGui_Display_Title( HDC hDC ) {
	if ( strlen(WINDOW_TITLE) <= 0 ) return;
	KGUI_DRAW_StrBox23( hDC , 282 , 5 , WINDOW_TITLE , 40 , 0 , 13 , 13 , COLOR_Black , 13 );
}
//========================================================================================
void ConGui_Display( HDC hDC ) {
	int i , j;
	char buffer[256];

	ConGui_Display_HeadTime( hDC , -1 );

	wsprintf( buffer , "%3d" , ANALOG_CNT );
	KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_COUNT , DISPLAY_START_Y_FOR_ANALOG_COUNT , buffer , 6 , 1 , COLOR_White , COLOR_DarkGray , COLOR_Black , COLOR_LightGray );

	for ( i = DISPLAY_ANALOG_START_POINTER , j = 0 ; ( i < ANALOG_CNT ) && ( j < DISPLAY_COUNT_FOR_ANALOG ) ; i++ , j++ ) {
		wsprintf( buffer , "%3d" , i+1 );
		//=================================================================================================
		if ( !REMOTE_MONITOR_MODE ) { // 2005.03.17
			if ( A_IO_SAVE(i) ) KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NO   , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer       , DISPLAY_SIZE_X_FOR_ANALOG_NO   , 1 , COLOR_LightGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
			else                KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NO   , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer       , DISPLAY_SIZE_X_FOR_ANALOG_NO   , 1 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_Black , COLOR_PaleBlue );
		}
		else {
			KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NO   , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer       , DISPLAY_SIZE_X_FOR_ANALOG_NO   , 1 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_Black , COLOR_PaleBlue );
		}
		//=================================================================================================
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NAME , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_SHOW(i) , DISPLAY_SIZE_X_FOR_ANALOG_NAME , 1 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_Black , COLOR_PaleBlue );
		if      ( A_IO_PLT_DISPLAY(i) == DRAW_NONE    ) strcpy( buffer , "x" );
		else if ( A_IO_PLT_DISPLAY(i) == DRAW_1_LEFT  ) strcpy( buffer , "1-L" );
		else if ( A_IO_PLT_DISPLAY(i) == DRAW_1_RIGHT ) strcpy( buffer , "1-R" );
		else if ( A_IO_PLT_DISPLAY(i) == DRAW_2_LEFT  ) strcpy( buffer , "2-L" );
		else if ( A_IO_PLT_DISPLAY(i) == DRAW_2_RIGHT ) strcpy( buffer , "2-R" );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_DISPLAY  , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_DISPLAY  , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		sprintf( buffer , "%d" , A_IO_PLT_POLL( i ) );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_TIME     , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_TIME     , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		STRING_CONV_With_DEV( A_IO_MIN( i ) , A_IO_IN_DEVIATION( i ) , buffer );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_MIN      , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_MIN      , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		STRING_CONV_With_DEV( A_IO_MAX( i ) , A_IO_IN_DEVIATION( i ) , buffer );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_MAX      , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_MAX      , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		if ( !V_AUTO_SCALE_DRAW() ) {
			switch( A_IO_PLT_DEV( i ) ) {
				case -12 :	sprintf( buffer , "1/14" ); break;
				case -11 :	sprintf( buffer , "1/13" ); break;
				case -10 :	sprintf( buffer , "1/12" ); break;
				case -9 :	sprintf( buffer , "1/11" ); break;
				case -8 :	sprintf( buffer , "1/10" ); break;
				case -7 :	sprintf( buffer , "1/9" ); break;
				case -6 :	sprintf( buffer , "1/8" ); break;
				case -5 :	sprintf( buffer , "1/7" ); break;
				case -4 :	sprintf( buffer , "1/6" ); break;
				case -3 :	sprintf( buffer , "1/5" ); break;
				case -2 :	sprintf( buffer , "1/4" ); break;
				case -1 :	sprintf( buffer , "1/3" ); break;
				case 0 :	sprintf( buffer , "1/2" ); break;
				case 1 :	sprintf( buffer , "1" ); break;
				case 2 :	sprintf( buffer , "2" ); break;
				case 3 :	sprintf( buffer , "3" ); break;
				case 4 :	sprintf( buffer , "4" ); break;
				case 5 :	sprintf( buffer , "5" ); break;
				case 6 :	sprintf( buffer , "6" ); break;
				case 7 :	sprintf( buffer , "7" ); break;
				case 8 :	sprintf( buffer , "8" ); break;
				case 9 :	sprintf( buffer , "9" ); break;
				case 10 :	sprintf( buffer , "10" ); break;
				case 11 :	sprintf( buffer , "11" ); break;
				case 12 :	sprintf( buffer , "12" ); break;
			}
			KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_INTERVAL , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_INTERVAL , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		}
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_HELP     , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_HELP(i) , DISPLAY_SIZE_X_FOR_ANALOG_HELP , 1 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
	}
	for ( ; ( j < DISPLAY_COUNT_FOR_ANALOG ) ; j++ ) {
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NO   , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_NO   , 1 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_Black , COLOR_PaleBlue );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NAME , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_NAME , 1 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_Black , COLOR_PaleBlue );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_DATA , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_DATA , 1 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_Black , COLOR_PaleBlue );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_DISPLAY  , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_DISPLAY  , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_TIME     , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_TIME     , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_MIN      , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_MIN      , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_MAX      , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_MAX      , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		if ( !V_AUTO_SCALE_DRAW() ) {
			KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_INTERVAL , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_INTERVAL , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
		}
		KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_HELP     , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_HELP     , 1 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ConGui_Display_Data( HDC hDC ) {
	int i , j;
	for ( i = DISPLAY_ANALOG_START_POINTER , j = 0 ; ( i < ANALOG_CNT ) && ( j < DISPLAY_COUNT_FOR_ANALOG ) ; i++ , j++ ) {
		//=================================================================================================
		if ( !REMOTE_MONITOR_MODE ) { // 2005.03.17
			if ( A_IO_SAVE(i) ) 
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_DATA , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , A_IO_VALUE_STR( i ) , DISPLAY_SIZE_X_FOR_ANALOG_DATA , 2 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_DarkBlue , COLOR_PaleBlue );
			else
				KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_DATA , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_DATA , 2 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_DarkBlue , COLOR_PaleBlue );
		}
		else {
			KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_DATA , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , "" , DISPLAY_SIZE_X_FOR_ANALOG_DATA , 2 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_DarkBlue , COLOR_PaleBlue );
		}
		//=================================================================================================
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Console_Gui_Move( HWND hdlg , int Move ) {
	HDC hDC;
	int NewPointer;
	if ( ANALOG_CNT <= 0 ) return;
	switch ( Move ) {
		case IDB_REWIND2	:	NewPointer = DISPLAY_ANALOG_START_POINTER - (DISPLAY_COUNT_FOR_ANALOG-1);	break;
		case IDB_REWIND		:	NewPointer = DISPLAY_ANALOG_START_POINTER - 1;	break;
		case IDB_FORWARD	:	NewPointer = DISPLAY_ANALOG_START_POINTER + 1;	break;
		case IDB_FORWARD2	:	NewPointer = DISPLAY_ANALOG_START_POINTER + (DISPLAY_COUNT_FOR_ANALOG-1);	break;
	}
	if ( NewPointer < 0            ) NewPointer = 0;
	if ( NewPointer > ANALOG_CNT-1 ) NewPointer = ANALOG_CNT-1;
	if ( NewPointer != DISPLAY_ANALOG_START_POINTER ) {
		DISPLAY_ANALOG_START_POINTER = NewPointer;
		hDC = GetDC( hdlg );
		ConGui_Display( hDC );
		ConGui_Display_Data( hDC );
		ReleaseDC( hdlg , hDC );
	}
}
//========================================================================================
//========================================================================================
//
void ConGui_Display_Y_num_Get( int point , double pos , char *buffer ) {
	STRING_CONV_With_DEV( A_IO_MIN( point ) + ( ( A_IO_MAX( point ) - A_IO_MIN( point ) ) * pos ) , A_IO_IN_DEVIATION( point ) , buffer );
}
//
void ConGui_Display_Y_num_Draw( HDC hDC , int no1 , int no2 , int xpos , int co1 , int co2 ) {
	int i , j , k , c;
	char Buffer[32];
	for ( i = 0 ; i < 5 ; i ++ ) {
		KGUI_DRAW_StrBox( hDC , xpos , DISPLAY_START_Y_FOR_MINMAX_UP + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
		"" , DISPLAY_SIZE_X_FOR_MINMAX ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , xpos , DISPLAY_START_Y_FOR_MINMAX_DN + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
		"" , DISPLAY_SIZE_X_FOR_MINMAX ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	}
	if ( no1 != -1 && no2 != -1 ) {
		for ( j = 0 ; j < 5 ; j++ ) {
			if ( A_IO_TOGGLE( no1 ) < -1 ) {
				ConGui_Display_Y_num_Get( no1 , 1 - ( (double) j * 0.25 ) , Buffer );
				KGUI_DRAW_StrBox( hDC , xpos , DISPLAY_START_Y_FOR_MINMAX_UP + ( j * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
					Buffer , DISPLAY_SIZE_X_FOR_MINMAX ,
					PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , co1 , PLOTTER_COLOR_FOR_INFO );
			}
			if ( A_IO_TOGGLE( no2 ) < -1 ) {
				ConGui_Display_Y_num_Get( no2 , 1 - ( (double) j * 0.25 ) , Buffer );
				KGUI_DRAW_StrBox( hDC , xpos , DISPLAY_START_Y_FOR_MINMAX_DN + ( j * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
					Buffer , DISPLAY_SIZE_X_FOR_MINMAX ,
					PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , co2 , PLOTTER_COLOR_FOR_INFO );
			}
		}
	}
	else if ( no1 != -1 || no2 != -1 ) {
		for ( i = 0 ; i < 2 ; i++ ) {
			if      ( i == 0 ) { k = no1; c = co1; }
			else if ( i == 1 ) { k = no2; c = co2; }
			if ( k != -1 ) {
				for ( j = 0 ; j < 5 ; j++ ) {
					if ( A_IO_TOGGLE( k ) < -1 ) {
						ConGui_Display_Y_num_Get( k , 1 - ( (double) j * 0.25 ) , Buffer );
						KGUI_DRAW_StrBox( hDC , xpos , DISPLAY_START_Y_FOR_MINMAX_CE + ( j * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
							Buffer , DISPLAY_SIZE_X_FOR_MINMAX ,
							PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , c , PLOTTER_COLOR_FOR_INFO );
					}
				}
			}
		}
	}
}
//========================================================================================
//========================================================================================
void ConGui_Display_Plot_Item_Clear( HDC hDC ) {
	int i;
	for ( i = 0 ; i < 2 ; i ++ ) {
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
			"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
			DISPLAY_START_X_FOR_PLOTTER_0L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
			PLOTTER_COLOR_FOR_INFO , FALSE );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
			"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
			DISPLAY_START_X_FOR_PLOTTER_0R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
			PLOTTER_COLOR_FOR_INFO , FALSE );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_1L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
			"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_1L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
			DISPLAY_START_X_FOR_PLOTTER_1L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
			PLOTTER_COLOR_FOR_INFO , FALSE );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_1R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
			"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_1R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
			DISPLAY_START_X_FOR_PLOTTER_1R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
			PLOTTER_COLOR_FOR_INFO , FALSE );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_2L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
			"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_2L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
			DISPLAY_START_X_FOR_PLOTTER_2L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
			PLOTTER_COLOR_FOR_INFO , FALSE );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_2R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
			"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_2R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
			DISPLAY_START_X_FOR_PLOTTER_2R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
			PLOTTER_COLOR_FOR_INFO , FALSE );
	}
	for ( i = 0 ; i < 5 ; i ++ ) {
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_0L , DISPLAY_START_Y_FOR_MINMAX_UP + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_0L , DISPLAY_START_Y_FOR_MINMAX_DN + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_0R , DISPLAY_START_Y_FOR_MINMAX_UP + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_0R , DISPLAY_START_Y_FOR_MINMAX_DN + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_1L , DISPLAY_START_Y_FOR_MINMAX_UP + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_1L , DISPLAY_START_Y_FOR_MINMAX_DN + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_1R , DISPLAY_START_Y_FOR_MINMAX_UP + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_1R , DISPLAY_START_Y_FOR_MINMAX_DN + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_2L , DISPLAY_START_Y_FOR_MINMAX_UP + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_2L , DISPLAY_START_Y_FOR_MINMAX_DN + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_2R , DISPLAY_START_Y_FOR_MINMAX_UP + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_MINMAX_2R , DISPLAY_START_Y_FOR_MINMAX_DN + ( i * DISPLAY_SIZE_Y_FOR_MINMAX ) ,
			"" , DISPLAY_SIZE_X_FOR_MINMAX ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	}
}
//========================================================================================
void ConGui_Display_Plot_Item( HDC hDC , int mode ) {
	int i , s1 , s2;
	s1 = V_PLOTTER_DRAW_MODE(SCREEN_1);
	s2 = V_PLOTTER_DRAW_MODE(SCREEN_2);
	if ( mode == 0 || mode == 1 ) {
		for ( i = 0 ; i < 2 ; i ++ ) {
			if ( s1 == SCR_SMALL_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_1L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_1,i) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_1L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_1L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_1,i) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_1L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_1L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_1L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_1,0) , V_PLOTTER_TABLE(SCREEN_1,1) , DISPLAY_START_X_FOR_MINMAX_1L , V_PLOTTER_COLOR(SCREEN_1,0) , V_PLOTTER_COLOR(SCREEN_1,1) );
			}
			else if ( s1 == SCR_BIG_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_1,i) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_1,i) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
					"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
					PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_1,0) , V_PLOTTER_TABLE(SCREEN_1,1) , DISPLAY_START_X_FOR_MINMAX_0L , V_PLOTTER_COLOR(SCREEN_1,0) , V_PLOTTER_COLOR(SCREEN_1,1) );
			}
		}
	}
	if ( mode == 0 || mode == 2 ) {
		for ( i = 0 ; i < 2 ; i ++ ) {
			if ( s1 == SCR_SMALL_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i+2) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_1R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_1,i+2) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_1R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_1R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_1,i+2) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_1R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_1R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_1R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_1,2) , V_PLOTTER_TABLE(SCREEN_1,3) , DISPLAY_START_X_FOR_MINMAX_1R , V_PLOTTER_COLOR(SCREEN_1,2) , V_PLOTTER_COLOR(SCREEN_1,3) );
			}
			else if ( s1 == SCR_BIG_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i+2) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_1,i+2) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_1,i+2) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_1,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_1,2) , V_PLOTTER_TABLE(SCREEN_1,3) , DISPLAY_START_X_FOR_MINMAX_0R , V_PLOTTER_COLOR(SCREEN_1,2) , V_PLOTTER_COLOR(SCREEN_1,3) );
			}
		}
	}
	if ( mode == 0 || mode == 3 ) {
		for ( i = 0 ; i < 2 ; i ++ ) {
			if ( s2 == SCR_SMALL_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_2L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_2,i) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_2L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_2L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_2,i) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_2L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_2L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_2L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_2,0) , V_PLOTTER_TABLE(SCREEN_2,1) , DISPLAY_START_X_FOR_MINMAX_2L , V_PLOTTER_COLOR(SCREEN_2,0) , V_PLOTTER_COLOR(SCREEN_2,1) );
			}
			else if ( s2 == SCR_BIG_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_2,i) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_2,i) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0L , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0L - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0L - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_2,0) , V_PLOTTER_TABLE(SCREEN_2,1) , DISPLAY_START_X_FOR_MINMAX_0L , V_PLOTTER_COLOR(SCREEN_2,0) , V_PLOTTER_COLOR(SCREEN_2,1) );
			}
		}
	}
	if ( mode == 0 || mode == 4 ) {
		for ( i = 0 ; i < 2 ; i ++ ) {
			if ( s2 == SCR_SMALL_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i+2) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_2R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_2,i+2) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_2R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_2R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_2,i+2) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_2R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_2R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_2R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_2,2) , V_PLOTTER_TABLE(SCREEN_2,3) , DISPLAY_START_X_FOR_MINMAX_2R , V_PLOTTER_COLOR(SCREEN_2,2) , V_PLOTTER_COLOR(SCREEN_2,3) );
			}
			else if ( s2 == SCR_BIG_DRAW ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i+2) != -1 ) {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						A_IO_SHOW( V_PLOTTER_TABLE(SCREEN_2,i+2) ) , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						V_PLOTTER_COLOR(SCREEN_2,i+2) , FALSE );
				}
				else {
					KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER_0R , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
						"" , DISPLAY_SIZE_X_FOR_PLOTTER ,
						PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , V_PLOTTER_COLOR(SCREEN_2,i+2) , PLOTTER_COLOR_FOR_INFO );
					KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER_0R - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
						DISPLAY_START_X_FOR_PLOTTER_0R - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( i * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
						PLOTTER_COLOR_FOR_INFO , FALSE );
				}
				ConGui_Display_Y_num_Draw( hDC , V_PLOTTER_TABLE(SCREEN_2,2) , V_PLOTTER_TABLE(SCREEN_2,3) , DISPLAY_START_X_FOR_MINMAX_0R , V_PLOTTER_COLOR(SCREEN_2,2) , V_PLOTTER_COLOR(SCREEN_2,3) );
			}
		}
	}
}
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
BOOL Console_Gui_Display_Setup( HWND hdlg , int Find , int Old , int New ) {
	HDC hDC;
	int i , j;
	BOOL c1L = FALSE , c1R = FALSE;
	BOOL c2L = FALSE , c2R = FALSE;

	if ( Old == New ) return FALSE;

	if ( New != DRAW_NONE ) {
		for ( i = 0 , j = 0 ; i < ANALOG_CNT ; i++ ) {
			if ( A_IO_PLT_DISPLAY(i) == New ) j++;
		}
		if ( j >= 2 ) return FALSE;
	}

	switch ( Old ) {
		case DRAW_NONE		:
							break;
		case DRAW_1_LEFT	:
							if ( V_PLOTTER_TABLE(SCREEN_1,0) == Find ) S_PLOTTER_TABLE(SCREEN_1,0,-1);
							if ( V_PLOTTER_TABLE(SCREEN_1,1) == Find ) S_PLOTTER_TABLE(SCREEN_1,1,-1);
							c1L = TRUE;
							break;
		case DRAW_1_RIGHT	:
							if ( V_PLOTTER_TABLE(SCREEN_1,2) == Find ) S_PLOTTER_TABLE(SCREEN_1,2,-1);
							if ( V_PLOTTER_TABLE(SCREEN_1,3) == Find ) S_PLOTTER_TABLE(SCREEN_1,3,-1);
							c1R = TRUE;
							break;
		case DRAW_2_LEFT	:
							if ( V_PLOTTER_TABLE(SCREEN_2,0) == Find ) S_PLOTTER_TABLE(SCREEN_2,0,-1);
							if ( V_PLOTTER_TABLE(SCREEN_2,1) == Find ) S_PLOTTER_TABLE(SCREEN_2,1,-1);
							c2L = TRUE;
							break;
		case DRAW_2_RIGHT	:
							if ( V_PLOTTER_TABLE(SCREEN_2,2) == Find ) S_PLOTTER_TABLE(SCREEN_2,2,-1);
							if ( V_PLOTTER_TABLE(SCREEN_2,3) == Find ) S_PLOTTER_TABLE(SCREEN_2,3,-1);
							c2R = TRUE;
							break;
	}

	A_IO_PLT_DISPLAY( Find ) = New;

	switch ( New ) {
		case DRAW_NONE		:
							break;
		case DRAW_1_LEFT	:
							if      ( V_PLOTTER_TABLE(SCREEN_1,0) == -1 ) S_PLOTTER_TABLE(SCREEN_1,0,Find);
							else if ( V_PLOTTER_TABLE(SCREEN_1,1) == -1 ) S_PLOTTER_TABLE(SCREEN_1,1,Find);
							c1L = TRUE;
							break;
		case DRAW_1_RIGHT	:
							if      ( V_PLOTTER_TABLE(SCREEN_1,2) == -1 ) S_PLOTTER_TABLE(SCREEN_1,2,Find);
							else if ( V_PLOTTER_TABLE(SCREEN_1,3) == -1 ) S_PLOTTER_TABLE(SCREEN_1,3,Find);
							c1R = TRUE;
							break;
		case DRAW_2_LEFT	:
							if      ( V_PLOTTER_TABLE(SCREEN_2,0) == -1 ) S_PLOTTER_TABLE(SCREEN_2,0,Find);
							else if ( V_PLOTTER_TABLE(SCREEN_2,1) == -1 ) S_PLOTTER_TABLE(SCREEN_2,1,Find);
							c2L = TRUE;
							break;
		case DRAW_2_RIGHT	:
							if      ( V_PLOTTER_TABLE(SCREEN_2,2) == -1 ) S_PLOTTER_TABLE(SCREEN_2,2,Find);
							else if ( V_PLOTTER_TABLE(SCREEN_2,3) == -1 ) S_PLOTTER_TABLE(SCREEN_2,3,Find);
							c2R = TRUE;
							break;
	}

	if ( c1L ) {
		hDC = GetDC( hdlg );	ConGui_Display_Plot_Item( hDC , 1 );	ReleaseDC( hdlg , hDC );
	}
	if ( c1R ) {
		hDC = GetDC( hdlg );	ConGui_Display_Plot_Item( hDC , 2 );	ReleaseDC( hdlg , hDC );
	}
	if ( c2L ) {
		hDC = GetDC( hdlg );	ConGui_Display_Plot_Item( hDC , 3 );	ReleaseDC( hdlg , hDC );
	}
	if ( c2R ) {
		hDC = GetDC( hdlg );	ConGui_Display_Plot_Item( hDC , 4 );	ReleaseDC( hdlg , hDC );
	}
	return TRUE;
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//========================================================================================
//========================================================================================
BOOL Console_Gui_Left_Button( HWND hdlg , POINT ptcursor ) {
	int item , j , Find , x1 , x2 , iRes;
	double dRes;
	HDC hDC;
	char buffer[32];
	BOOL B1 , B2;

	for ( item = 0 ; item < 6 ; item++ ) {
		switch( item ) {
			case 0 :	x1 = DISPLAY_START_X_FOR_ANALOG_NO;
						x2 = DISPLAY_START_X_FOR_ANALOG_NO + ( DISPLAY_SIZE_X_FOR_ANALOG_NO * 8 ) + 4;
						break;
			case 1 :	x1 = DISPLAY_START_X_FOR_ANALOG_DISPLAY;
						x2 = DISPLAY_START_X_FOR_ANALOG_DISPLAY + ( DISPLAY_SIZE_X_FOR_ANALOG_DISPLAY * 8 ) + 4;
						break;
			case 2 :	x1 = DISPLAY_START_X_FOR_ANALOG_TIME;
						x2 = DISPLAY_START_X_FOR_ANALOG_TIME + ( DISPLAY_SIZE_X_FOR_ANALOG_TIME * 8 ) + 4;
						break;
			case 3 :	x1 = DISPLAY_START_X_FOR_ANALOG_MIN;
						x2 = DISPLAY_START_X_FOR_ANALOG_MIN + ( DISPLAY_SIZE_X_FOR_ANALOG_MIN * 8 ) + 4;
						break;
			case 4 :	x1 = DISPLAY_START_X_FOR_ANALOG_MAX;
						x2 = DISPLAY_START_X_FOR_ANALOG_MAX + ( DISPLAY_SIZE_X_FOR_ANALOG_MAX * 8 ) + 4;
						break;
			case 5 :	x1 = DISPLAY_START_X_FOR_ANALOG_INTERVAL;
						x2 = DISPLAY_START_X_FOR_ANALOG_INTERVAL + ( DISPLAY_SIZE_X_FOR_ANALOG_INTERVAL * 8 ) + 4;
						break;
		}
		Find = -1;
		for ( j = 0 ; j < DISPLAY_COUNT_FOR_ANALOG ; j++ ) {
			if ( (	ptcursor.x >= x1 ) &&
				 (	ptcursor.x <= x2 ) &&
			     (	ptcursor.y >= DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) ) &&
				 (	ptcursor.y <= DISPLAY_START_Y_FOR_ANALOG + ( (j+1) * DISPLAY_SIZE_Y_FOR_ANALOG ) ) ) {
				Find = DISPLAY_ANALOG_START_POINTER + j;
				break;
			}
		}
		if ( Find >= 0 && Find < ANALOG_CNT ) {
			switch( item ) {
				case 0 :
							if ( !REMOTE_MONITOR_MODE ) { // 2005.03.17
								if ( DATA_LOGGING_STATUS() ) {
									MessageBox( hdlg , "Data Buffering Cannot Change(Log Running..)" , "Data Logging...." , MB_ICONQUESTION );
								}
								else {
									iRes = A_IO_SAVE( Find );
									if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Data Buffering" , "No|Yes" , &iRes ) ) {
										if ( iRes != A_IO_SAVE( Find ) ) {
											A_IO_SAVE( Find ) = iRes;
											A_IO_PLT_OLD( Find ) = 9999;
											A_IO_PLT_OLD2( Find ) = 9999;
											hDC = GetDC( hdlg );
											wsprintf( buffer , "%3d" , Find+1 );
											if ( A_IO_SAVE(Find) ) KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NO   , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_NO   , 1 , COLOR_LightGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
											else                   KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_NO   , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_NO   , 1 , COLOR_PaleBlue , COLOR_PaleBlue , COLOR_Black , COLOR_PaleBlue );
											ReleaseDC( hdlg , hDC );
											A_IO_MONITOR_REFORM = TRUE;
										}
									}
								}
							}
							break;
				case 1 :
							iRes = A_IO_PLT_DISPLAY( Find );
							if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Where Display" , "None|1-Left|1-Right|2-Left|2-Right" , &iRes ) ) {
								if ( iRes != A_IO_PLT_DISPLAY( Find ) ) {
									B1 = V_PLOTTER_TABLE_BLANK( SCREEN_1 );
									B2 = V_PLOTTER_TABLE_BLANK( SCREEN_2 );
									if ( Console_Gui_Display_Setup( hdlg , Find , A_IO_PLT_DISPLAY( Find ) , iRes ) ) {
										hDC = GetDC( hdlg );
										if      ( A_IO_PLT_DISPLAY(Find) == DRAW_NONE    ) strcpy( buffer , "x" );
										else if ( A_IO_PLT_DISPLAY(Find) == DRAW_1_LEFT  ) strcpy( buffer , "1-L" );
										else if ( A_IO_PLT_DISPLAY(Find) == DRAW_1_RIGHT ) strcpy( buffer , "1-R" );
										else if ( A_IO_PLT_DISPLAY(Find) == DRAW_2_LEFT  ) strcpy( buffer , "2-L" );
										else if ( A_IO_PLT_DISPLAY(Find) == DRAW_2_RIGHT ) strcpy( buffer , "2-R" );
										KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_DISPLAY  , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_DISPLAY  , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );

										if ( B1 != V_PLOTTER_TABLE_BLANK( SCREEN_1 ) || B2 != V_PLOTTER_TABLE_BLANK( SCREEN_2 ) ) {
											ConGui_Display_Plot_Item_Clear( hDC );
											Draw_Time_Data_Clear( hDC );
											ConGui_Display_Plot_Item( hDC , 0 );
										}
										Console_Gui_For_Drawing( hDC , -1 );
										ReleaseDC( hdlg , hDC );
									}
									else {
										MessageBox( hdlg , "Selected Display Position is Busy.." , "No Use" , MB_ICONQUESTION );
									}
								}
							}
							break;
				case 2 :
							if ( V_AUTO_SCALE_DRAW() ) return TRUE;
							iRes = A_IO_PLT_POLL( Find );
							if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Polling Time" , 0 , 50 , &iRes ) ) {
								if ( iRes != A_IO_PLT_POLL( Find ) ) {
									A_IO_PLT_POLL( Find ) = iRes;
									sprintf( buffer , "%d" , A_IO_PLT_POLL(Find) );
									hDC = GetDC( hdlg );
									KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_TIME , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_TIME , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
									ReleaseDC( hdlg , hDC );
								}
							}
							break;
				case 3 :
							dRes = A_IO_MIN( Find );
							if ( MODAL_ANALOG_BOX1( hdlg , "Select" , "Minimum" , A_IO_MIN( Find ) - 1000 , A_IO_MAX( Find ) , A_IO_IN_DEVIATION( Find ) , &dRes ) ) {
								if ( dRes < A_IO_MAX( Find ) ) {
									A_IO_MIN( Find ) = dRes;
									hDC = GetDC( hdlg );
									if      ( A_IO_PLT_DISPLAY(Find) == DRAW_1_LEFT  ) ConGui_Display_Plot_Item( hDC , 1 );
									else if ( A_IO_PLT_DISPLAY(Find) == DRAW_1_RIGHT ) ConGui_Display_Plot_Item( hDC , 2 );
									else if ( A_IO_PLT_DISPLAY(Find) == DRAW_2_LEFT  ) ConGui_Display_Plot_Item( hDC , 3 );
									else if ( A_IO_PLT_DISPLAY(Find) == DRAW_2_RIGHT ) ConGui_Display_Plot_Item( hDC , 4 );
									STRING_CONV_With_DEV( A_IO_MIN(Find) , A_IO_IN_DEVIATION(Find) , buffer );
									KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_MIN      , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_MIN      , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
									//
									ConGui_Display_Plot_Item_Clear( hDC );
									Draw_Time_Data_Clear( hDC );
									ConGui_Display_Plot_Item( hDC , 0 );
									Console_Gui_For_Drawing( hDC , -1 );
									//
									ReleaseDC( hdlg , hDC );
								}
								else {
									MessageBox( hdlg , "Data is too big than MAX Value.." , "No Use" , MB_ICONQUESTION );
								}
							}
							break;

				case 4 :
							dRes = A_IO_MAX( Find );
							if ( MODAL_ANALOG_BOX1( hdlg , "Select" , "Maximum" , A_IO_MIN( Find ) , A_IO_MAX( Find ) + 1000 , A_IO_IN_DEVIATION( Find ) , &dRes ) ) {
								if ( dRes > A_IO_MIN( Find ) ) {
									A_IO_MAX( Find ) = dRes;
									hDC = GetDC( hdlg );
									if      ( A_IO_PLT_DISPLAY(Find) == DRAW_1_LEFT  ) ConGui_Display_Plot_Item( hDC , 1 );
									else if ( A_IO_PLT_DISPLAY(Find) == DRAW_1_RIGHT ) ConGui_Display_Plot_Item( hDC , 2 );
									else if ( A_IO_PLT_DISPLAY(Find) == DRAW_2_LEFT  ) ConGui_Display_Plot_Item( hDC , 3 );
									else if ( A_IO_PLT_DISPLAY(Find) == DRAW_2_RIGHT ) ConGui_Display_Plot_Item( hDC , 4 );
									ReleaseDC( hdlg , hDC );
									hDC = GetDC( hdlg );
									STRING_CONV_With_DEV( A_IO_MAX(Find) , A_IO_IN_DEVIATION(Find) , buffer );
									KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_MAX      , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_MAX      , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
									//
									ConGui_Display_Plot_Item_Clear( hDC );
									Draw_Time_Data_Clear( hDC );
									ConGui_Display_Plot_Item( hDC , 0 );
									Console_Gui_For_Drawing( hDC , -1 );
									//
									ReleaseDC( hdlg , hDC );
								}
								else {
									MessageBox( hdlg , "Data is too small than MIN Value.." , "No Use" , MB_ICONQUESTION );
								}
							}
							break;

				case 5 :
							if ( V_AUTO_SCALE_DRAW() ) return TRUE;
							iRes = A_IO_PLT_DEV( Find ) + 3;
							if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "X-Interval" , "1/5|1/4|1/3|1/2|1|2|3|4|5|6|7|8|9|10|11|12" , &iRes ) ) {
								if ( (iRes-3) != A_IO_PLT_DEV( Find ) ) {
									A_IO_PLT_DEV( Find ) = (iRes-3);
									hDC = GetDC( hdlg );
									switch( A_IO_PLT_DEV(Find) ) {
										case -12 :	sprintf( buffer , "1/14" ); break;
										case -11 :	sprintf( buffer , "1/13" ); break;
										case -10 :	sprintf( buffer , "1/12" ); break;
										case -9 :	sprintf( buffer , "1/11" ); break;
										case -8 :	sprintf( buffer , "1/10" ); break;
										case -7 :	sprintf( buffer , "1/9" ); break;
										case -6 :	sprintf( buffer , "1/8" ); break;
										case -5 :	sprintf( buffer , "1/7" ); break;
										case -4 :	sprintf( buffer , "1/6" ); break;
										case -3 :	sprintf( buffer , "1/5" ); break;
										case -2 :	sprintf( buffer , "1/4" ); break;
										case -1 :	sprintf( buffer , "1/3" ); break;
										case 0 :	sprintf( buffer , "1/2" ); break;
										case 1 :	sprintf( buffer , "1" ); break;
										case 2 :	sprintf( buffer , "2" ); break;
										case 3 :	sprintf( buffer , "3" ); break;
										case 4 :	sprintf( buffer , "4" ); break;
										case 5 :	sprintf( buffer , "5" ); break;
										case 6 :	sprintf( buffer , "6" ); break;
										case 7 :	sprintf( buffer , "7" ); break;
										case 8 :	sprintf( buffer , "8" ); break;
										case 9 :	sprintf( buffer , "9" ); break;
										case 10 :	sprintf( buffer , "10" ); break;
										case 11 :	sprintf( buffer , "11" ); break;
										case 12 :	sprintf( buffer , "12" ); break;
									}
									KGUI_DRAW_StrBox2( hDC , DISPLAY_START_X_FOR_ANALOG_INTERVAL , DISPLAY_START_Y_FOR_ANALOG + ( j * DISPLAY_SIZE_Y_FOR_ANALOG ) , buffer , DISPLAY_SIZE_X_FOR_ANALOG_INTERVAL , 0 , COLOR_PaleGreen , COLOR_PaleGreen , COLOR_Black , COLOR_PaleGreen );
									//
									ConGui_Display_Plot_Item_Clear( hDC );
									Draw_Time_Data_Clear( hDC );
									ConGui_Display_Plot_Item( hDC , 0 );
									Console_Gui_For_Drawing( hDC , -1 );
									//
									ReleaseDC( hdlg , hDC );
								}
							}
							break;
			}
			return TRUE;
		}
	}
	return FALSE;
}
//========================================================================================
//========================================================================================
BOOL Console_Gui_Right_Button( HWND hdlg , POINT ptcursor , BOOL LikeLeft ) {
	int i , iRes , x1 , x2 , y1 , y2;
	BOOL Find = FALSE;
	if ( LikeLeft ) {
		x1 = 282;
		x2 = 282 + 365;
		y1 = 5;
		y2 = 5 + 37;
	}
	else {
		x1 = DISPLAY_START_X_FOR_ANALOG_NO;
		x2 = DISPLAY_START_X_FOR_ANALOG_INTERVAL + ( DISPLAY_SIZE_X_FOR_ANALOG_INTERVAL * 8 ) + 4;
		y1 = DISPLAY_START_Y_FOR_ANALOG;
		y2 = DISPLAY_START_Y_FOR_ANALOG + ( DISPLAY_COUNT_FOR_ANALOG * DISPLAY_SIZE_Y_FOR_ANALOG );
	}
	if ( ( ptcursor.x >= x1 ) &&
		 ( ptcursor.x <= x2 ) &&
	     ( ptcursor.y >= y1 ) &&
		 ( ptcursor.y <= y2 ) ) {
		//==============================================================================================================
		// 2005.03.17
		//==============================================================================================================
		if ( REMOTE_MONITOR_MODE ) {
			iRes = 0;
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Utility" , "Read from DataLogged File|Save Parameter" , &iRes ) ) {
				if      ( iRes == 1 ) Console_Gui_Parameter_Save();
				else if ( iRes == 0 ) {
					if ( Console_Datalog_Read( hdlg , FALSE ) ) {
						Plotting_Zoom_Recover( -1 ); // 2002.04.11
						InvalidateRect( hdlg , NULL , TRUE );
					}
				}
			}
		}
		//==============================================================================================================
		else {
			for ( i = 0 ; i < ANALOG_CNT ; i++ ) if ( A_IO_SAVE( i ) ) Find = TRUE;
			iRes = 0;

			if ( Find ) {
				if ( DATA_LOGGING_STATUS() ) {
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Utility" , "Save Parameter|DataLog Control" , &iRes ) ) {
						if      ( iRes == 0 ) Console_Gui_Parameter_Save();
						else if ( iRes == 1 ) Console_Datalog_Control( hdlg );
					}
				}
				else {
					if ( DISP_GO_READ_MODE_GET() ) {
						if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Utility" , "Save Parameter|DataLog Control|DataLog Time|DataLog Saving Mode|Read from DataLogged File|Return Last Data Logged Staus" , &iRes ) ) {
							if      ( iRes == 0 ) Console_Gui_Parameter_Save();
							else if ( iRes == 1 ) Console_Datalog_Control( hdlg );
							else if ( iRes == 2 ) Console_Datalog_Time( hdlg );
							else if ( iRes == 3 ) Console_Datalog_Save_Mode( hdlg );
							else if ( iRes == 4 ) {
								if ( Console_Datalog_Read( hdlg , TRUE ) ) {
									Plotting_Zoom_Recover( -1 ); // 2002.04.11
									InvalidateRect( hdlg , NULL , TRUE );
								}
							}
							else if ( iRes == 5 ) {
								Plotting_Zoom_Recover( -1 ); // 2002.04.11
								DISP_GO_READ_MODE_SET( FALSE );
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
					}
					else {
						//---> Modify  2008/08/27  All Delete Graph Item  by KSM
						//if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Utility" , "Save Parameter|DataLog Control|DataLog Time|DataLog Saving Mode|Read from DataLogged File" , &iRes ) ) {
						if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Utility" , "Save Parameter|DataLog Control|DataLog Time|DataLog Saving Mode|Read from DataLogged File|AllClearDisplay" , &iRes ) ) {
						//---> END     2008/08/27  All Delete Graph Item  by KSM

							if      ( iRes == 0 ) Console_Gui_Parameter_Save();
							else if ( iRes == 1 ) Console_Datalog_Control( hdlg );
							else if ( iRes == 2 ) Console_Datalog_Time( hdlg );
							else if ( iRes == 3 ) Console_Datalog_Save_Mode( hdlg );
							else if ( iRes == 4 ) {
								if ( Console_Datalog_Read( hdlg , TRUE ) ) {
									Plotting_Zoom_Recover( -1 ); // 2002.04.11
									InvalidateRect( hdlg , NULL , TRUE );
								}
							}
							//---> Add 2008/08/27 All Delete Graph Item   by KSM
							else if ( iRes == 5 )
							{
								
								for ( i = 0 ; i < ANALOG_CNT ; i++ )
								{
									Console_Gui_Display_Setup( hdlg , i , A_IO_PLT_DISPLAY( i ) , 0 ) ;
									printf("all delete [%d]\n", i);	
									InvalidateRect( hdlg , NULL , TRUE );
								}

							}
							//---> End 2008/08/27 All Delete Graph Item   by KSM
						}
					}
				}
			}
			else {
				if ( DISP_GO_READ_MODE_GET() ) {
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Utility" , "Save Parameter|Read from DataLogged File|Return Last Data Logged Staus" , &iRes ) ) {
						if      ( iRes == 0 ) Console_Gui_Parameter_Save();
						else if ( iRes == 1 ) {
							if ( Console_Datalog_Read( hdlg , FALSE ) ) {
								Plotting_Zoom_Recover( -1 ); // 2002.04.11
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
						else if ( iRes == 2 ) {
							Plotting_Zoom_Recover( -1 ); // 2002.04.11
							DISP_GO_READ_MODE_SET( FALSE );
							InvalidateRect( hdlg , NULL , TRUE );
						}
					}
				}
				else {
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Utility" , "Save Parameter|Read from DataLogged File" , &iRes ) ) {
						if      ( iRes == 0 ) Console_Gui_Parameter_Save();
						else if ( iRes == 1 ) {
							if ( Console_Datalog_Read( hdlg , FALSE ) ) {
								Plotting_Zoom_Recover( -1 ); // 2002.04.11
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
					}
				}
			}
			return TRUE;
		}
	}
	if ( LikeLeft ) return TRUE;
	return FALSE;
}
//========================================================================================
//========================================================================================
BOOL READ_EIO_INFORMATION( char *Name , char *Show , int Type , int Point ) {
	if ( EVENT_CNT >= MAX_EVENT_TABLE ) {
		_IO_CIM_PRINTF( "Too Many EVENT Table Error\n" );
		return FALSE;
	}
	E_IOP[ EVENT_CNT ] = malloc( EIO_TOTAL );
	if ( E_IOP[ EVENT_CNT ] == NULL ) {
		_IO_CIM_PRINTF( "Allocate memory for Event Table Error\n" );
		return FALSE;
	}
	strncpy( E_IO_NAME( EVENT_CNT )	, Name , MAX_CHANNEL_NAME_SIZE );
	if ( strlen( Show ) <= 0 )
		strncpy( E_IO_SHOW( EVENT_CNT )	, Name , MAX_CHANNEL_NAME_SIZE );
	else
		strncpy( E_IO_SHOW( EVENT_CNT )	, Show , MAX_CHANNEL_NAME_SIZE );
	E_IO_TYPE( EVENT_CNT )	= Type;
	E_IO_POINT( EVENT_CNT )	= Point;

	EVENT_CNT++;
	return TRUE;
}
//
//
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//========================================================================================
//
BOOL READ_AIO_INFORMATION( char *Name , char *Show , int Point , char *Help , BOOL Ana , int Tgl , BOOL remotemode ) {
	int Type , min , max;
	int Out_Dev;
	char EnumList[1024];

	if ( ANALOG_CNT >= MAX_ANALOG_TABLE ) {
		_IO_CIM_PRINTF( "Too Many Analog Table Error\n" );
		return FALSE;
	}
	A_IOP[ ANALOG_CNT ] = malloc( AIO_TOTAL );
	if ( A_IOP[ ANALOG_CNT ] == NULL ) {
		_IO_CIM_PRINTF( "Allocate memory for Analog Table Error\n" );
		return FALSE;
	}
	strncpy( A_IO_NAME( ANALOG_CNT )	, Name , MAX_CHANNEL_NAME_SIZE );
	if ( strlen( Show ) <= 0 )
		strncpy( A_IO_SHOW( ANALOG_CNT )	, Name , MAX_CHANNEL_NAME_SIZE );
	else
		strncpy( A_IO_SHOW( ANALOG_CNT )	, Show , MAX_CHANNEL_NAME_SIZE );
	strcpy( A_IO_HELP( ANALOG_CNT )	, Help );

	A_IO_TYPE( ANALOG_CNT )		= Ana;
	A_IO_SAVE( ANALOG_CNT )		= FALSE;
	A_IO_SAVE_OLD( ANALOG_CNT )	= FALSE;
	//=================================================================================
	// 2005.03.17
	//=================================================================================
	if ( !remotemode ) {
		A_IO_POINT( ANALOG_CNT )	= Point;
	}
	else {
		A_IO_POINT( ANALOG_CNT )	= ANALOG_CNT;
	}
	//=================================================================================
	A_IO_TOGGLE( ANALOG_CNT )	= Tgl;

	//=================================================================================
	// 2005.03.17
	//=================================================================================
	if ( !remotemode ) {
		if ( Ana ) {
			_dANALOG_UNIT( Point , A_IO_UNIT( ANALOG_CNT ) );
			_dANALOG_SHAPE( Point , &Type , &(A_IO_IN_DEVIATION( ANALOG_CNT )) , &Out_Dev , &(A_IO_MIN( ANALOG_CNT )) , &(A_IO_MAX( ANALOG_CNT )) );
		}
		else {
			_dDIGITAL_ENUM( Point , A_IO_UNIT( ANALOG_CNT ) );
			_dDIGITAL_SHAPE( Point , &Type , &min , &max , &Out_Dev , EnumList );
			A_IO_IN_DEVIATION( ANALOG_CNT ) = 0;
			A_IO_MIN( ANALOG_CNT ) = (double) min;
			A_IO_MAX( ANALOG_CNT ) = (double) max;
		}
	}
	else {
		strcpy( A_IO_UNIT( ANALOG_CNT ) , "" );
		A_IO_IN_DEVIATION( ANALOG_CNT ) = 0;
		A_IO_MIN( ANALOG_CNT ) = -999999999;
		A_IO_MAX( ANALOG_CNT ) =  999999999;
	}
	//=================================================================================

	A_IO_PLT_DISPLAY( ANALOG_CNT )	= DRAW_NONE;
	A_IO_PLT_START( ANALOG_CNT )	= 0;
	A_IO_PLT_END( ANALOG_CNT )		= 0;
	A_IO_PLT_COUNT( ANALOG_CNT )	= 0;
	A_IO_PLT_HERE( ANALOG_CNT )		= 0;
	A_IO_PLT_POLL( ANALOG_CNT )		= 1;
	A_IO_PLT_OLD( ANALOG_CNT )		= 9999;
	A_IO_PLT_OLD2( ANALOG_CNT )		= 9999;
	A_IO_PLT_DEV( ANALOG_CNT )		= 1;
	A_IO_ZOOM( ANALOG_CNT )			= 0;

	strcpy( A_IO_VALUE_STR( ANALOG_CNT ) , "" );

	ANALOG_CNT++;
	return TRUE;
}
//
//
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Read_Information_Saved_File( char *FileName ) {
	HFILE hFile;
	char Buffer[ 256 ];
	char chr_return1[ 256 ];
	int  ReadCnt , Line , Point , iRes;
	BOOL FileEnd = TRUE;

	if ( *FileName == 0 ) return TRUE;
	hFile = _lopen( FileName , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "Warning : [%s] Information Saved File Not Found\n" , FileName );
		return TRUE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt );
		if ( ReadCnt > 0 ) {
			if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 1 - [%s]-[%d]\n" , FileName , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( STRCMP_L( "$_PLOT_BIG" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Big 1 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_BIG( SCREEN_1 , (BOOL) iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Big 2 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_BIG( SCREEN_2 , (BOOL) iRes );
			}
			else if ( STRCMP_L( "$_PLOT_BACK" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Back 1 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_BACK_DRAW( SCREEN_1 , (BOOL) iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Back 2 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_BACK_DRAW( SCREEN_2 , (BOOL) iRes );
			}
			else if ( STRCMP_L( "$_PLOT_BACK_COLOR" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Back Color 1 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_BACK_COLOR( SCREEN_1 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Back Color 2 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_BACK_COLOR( SCREEN_2 , iRes );
			}
			else if ( STRCMP_L( "$_PLOT_POINT" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Point 1 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_POINT( SCREEN_1 , (BOOL) iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Point 2 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_POINT( SCREEN_2 , (BOOL) iRes );
			}
			else if ( STRCMP_L( "$_PLOT_LINE" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Point 1 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_LINE_DRAW_TYPE( SCREEN_1 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Point 2 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_LINE_DRAW_TYPE( SCREEN_2 , iRes );
			}
			else if ( STRCMP_L( "$_PLOT_COLOR_1" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 1 - 1 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_1 , 0 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 1 - 2 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_1 , 1 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 3 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 1 - 3 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_1 , 2 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 4 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 1 - 4 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_1 , 3 , iRes );
			}
			else if ( STRCMP_L( "$_PLOT_COLOR_2" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 2 - 1 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_2 , 0 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 2 - 2 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_2 , 1 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 3 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 2 - 3 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_2 , 2 , iRes );
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 4 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Color 2 - 4 Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_COLOR( SCREEN_2 , 3 , iRes );
			}
			else if ( STRCMP_L( "$_DATALOG_TIME" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 999 , &iRes ) ) {
					_IO_CIM_PRINTF( "Data log Time - Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				DATALOG_TIME = iRes;
			}
			else if ( STRCMP_L( "$_DATALOG_FILE_SAVE_TYPE" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 3 , &iRes ) ) {
					_IO_CIM_PRINTF( "Data log File Save Type - Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				S_PLOTTER_SAVE_TAB_TYPE( iRes );
			}
			else {
				for ( Point = 0 ; Point < ANALOG_CNT ; Point++ ) {
					if ( STRCMP_L( A_IO_NAME(Point) , chr_return1 ) ) break;
				}
				if ( Point == ANALOG_CNT ) {
					for ( Point = 0 ; Point < ANALOG_CNT ; Point++ ) {
						if ( STRCMP_L( A_IO_SHOW(Point) , chr_return1 ) ) break;
					}
					if ( Point == ANALOG_CNT ) continue;
				}
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &(A_IO_SAVE(Point)) ) ) {
					_IO_CIM_PRINTF( "Buffering type Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				//
				if ( !Get_Data_From_String_for_DOUBLE( Buffer , 2 , chr_return1 , 0 , 0 , &( A_IO_MIN(Point) ) ) ) {
					_IO_CIM_PRINTF( "MIN Value Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				//
				if ( !Get_Data_From_String_for_DOUBLE( Buffer , 3 , chr_return1 , 0 , 0 , &( A_IO_MAX(Point) ) ) ) {
					_IO_CIM_PRINTF( "MAX Value Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				//
				if ( A_IO_MIN(Point) > A_IO_MAX(Point) ) {
					_IO_CIM_PRINTF( "MIN > MAX Value Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 4 , chr_return1 , 0 , 4 , &(A_IO_PLT_DISPLAY(Point)) ) ) {
					_IO_CIM_PRINTF( "Display Position Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 5 , chr_return1 , 0 , 50 , &(A_IO_PLT_POLL(Point)) ) ) {
					_IO_CIM_PRINTF( "Polling Time Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				//
//				if ( !Get_Data_From_String_for_INT( Buffer , 6 , chr_return1 , 1 , 12 , &(A_IO_PLT_DEV(Point)) ) ) { // 2005.04.28
				if ( !Get_Data_From_String_for_INT( Buffer , 6 , chr_return1 , -12 , 12 , &(A_IO_PLT_DEV(Point)) ) ) { // 2005.04.28
					_IO_CIM_PRINTF( "X Deviation Data Error- [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
			}
		}
	}
	_lclose( hFile );
//	fclose(fpt);
	return TRUE;
}
//========================================================================================
//========================================================================================
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Read_Information_File( char *FileName , BOOL GuiDraw ) {
	HFILE hFile;
	char Buffer[ 256 ];
	char chr_return1[ 256 ];
	char chr_return2[ 256 ];
	char chr_return3[ 256 ];
	int  ReadCnt , Line , Point , Type , Tp , Tgl;
	BOOL FileEnd = TRUE;

	DISPLAY_CONTROL				=	-1;

	DISPLAY_TIME_MODE			=	0;

	DATALOG_CONTROL				=	-1;
	DATALOG_FILE				=	-1;
	DATALOG_TIME				=	2;
	DATALOG_TIME_OLD			=	9999;
	DATALOG_AUTO_DELETE_TIME	=	3;
	DATALOG_AUTO_DELETE_COUNT	=	50; // 2004.01.14
	
	//---> Modify 2008/08/27 HeaderItem 추가 by KSM
	//DATALOG_STRING_IO			= -1;
	DATALOG_STRING_IO1			= -1;
	DATALOG_STRING_IO2			= -1;
	//---> END    2008/08/27 HeaderItem 추가 by KSM

 	
	//---> Modify 2008/08/27 HeaderItem 추가 by KSM
	//DATALOG_DIGITAL_IO			= -1; // 2005.08.01 JJY
	DATALOG_DIGITAL_IO1			= -1; 
	DATALOG_DIGITAL_IO2			= -1; 
	DATALOG_DIGITAL_IO3			= -1; 
	//---> END	  2008/08/27 HeaderItem 추가 by KSM

	//---> Add    2008/08/27 HeaderItem 추가 by KSM
	DATALOG_ANALOG_IO1			= -1;
	//---> End    2008/08/27 HeaderItem 추가 by KSM


	strcpy( INFORMATION_FILE , "" );
	strcpy( DATALOG_DIRECTORY , "" );
	DATALOG_DIRECTORY_SEP = FALSE; // 2004.01.09
	strcpy( WINDOW_TITLE , "" );

	HEADER_MESSAGE_IO = -1;
	HEADER_MESSAGE_DISPLAY_START_X = 0;
	HEADER_MESSAGE_DISPLAY_START_Y = 0;
	strcpy( HEADER_MESSAGE_STRING , "" );
	HEADER_MESSAGE_SIZE = 10;

	if ( *FileName == 0 ) return TRUE;
	hFile = _lopen( FileName , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "[%s] File Not Found\n" , FileName );
		return FALSE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt );
		if ( ReadCnt > 0 ) {
			if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 1 - [%s]-[%d]\n" , FileName , Line );
				_lclose( hFile );
				return FALSE;
			}
			if      ( STRCMP_L( "$HEADER_MESSAGE" , chr_return1 ) ) {
				if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				HEADER_MESSAGE_IO = _FIND_FROM_STRING( _KG_S_IO , chr_return2 );
				if ( HEADER_MESSAGE_IO == -1 ) {
					_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $HEADER_MESSAGE %s\n" , chr_return2 );
					_lclose( hFile );
					return FALSE;
				}
				else {
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return2 , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 2 $HEADER_MESSAGE - [%s]-[%d]\n" , FileName , Line );
							_lclose( hFile );
							return FALSE;
						}
						HEADER_MESSAGE_DISPLAY_START_X = atoi(chr_return2);
						if ( ReadCnt > 3 ) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , 3 ) ) {
								_IO_CIM_PRINTF( "Unknown Error 3 $HEADER_MESSAGE - [%s]-[%d]\n" , FileName , Line );
								_lclose( hFile );
								return FALSE;
							}
							HEADER_MESSAGE_DISPLAY_START_Y = atoi(chr_return2);
							if ( ReadCnt > 4 ) {
								if ( !Get_Data_From_String( Buffer , chr_return2 , 4 ) ) {
									_IO_CIM_PRINTF( "Unknown Error 3 $HEADER_MESSAGE - [%s]-[%d]\n" , FileName , Line );
									_lclose( hFile );
									return FALSE;
								}
								HEADER_MESSAGE_SIZE = atoi(chr_return2);
								if ( HEADER_MESSAGE_SIZE < 5 ) HEADER_MESSAGE_SIZE = 5;
							}
						}
					}
				}
			}
			else if ( STRCMP_L( "$DATALOG_CONTROL" , chr_return1 ) ) {
				if ( !REMOTE_MONITOR_MODE ) { // 2005.03.15
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_CONTROL - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DATALOG_CONTROL = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
					if ( DATALOG_CONTROL == -1 ) { 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_CONTROL %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}

			//---> DEL 2008/08/27 HeaderItem 추가 by KSM
			/*
			else if ( STRCMP_L( "$DATALOG_STRING_IO" , chr_return1 ) ) { // 2005.07.19
				if ( !REMOTE_MONITOR_MODE ) {
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_STRING_IO - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DATALOG_STRING_IO = _FIND_FROM_STRING( _KG_S_IO , chr_return2 );
					if ( DATALOG_STRING_IO == -1 ) { 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_STRING_IO %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			*/
			//---> END 2008/08/27 HeaderItem 추가 by KSM

			//---> ADD 2008/08/27 HeaderItem 추가 by KSM
			else if ( STRCMP_L( "$DATALOG_STRING_IO1" , chr_return1 ) ) { 
				if ( !REMOTE_MONITOR_MODE ) {
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_STRING_IO1 - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DATALOG_STRING_IO1 = _FIND_FROM_STRING( _KG_S_IO , chr_return2 );
					if ( DATALOG_STRING_IO1 == -1 ) { 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_STRING_IO1 %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}

			else if ( STRCMP_L( "$DATALOG_STRING_IO2" , chr_return1 ) ) { 
				if ( !REMOTE_MONITOR_MODE ) {
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_STRING_IO2 - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DATALOG_STRING_IO2 = _FIND_FROM_STRING( _KG_S_IO , chr_return2 );
					if ( DATALOG_STRING_IO2 == -1 ) { 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_STRING_IO2 %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			//---> End 2008/08/27 HeaderItem 추가  by KSM


			//---> Modify 2008/08/27  HeaderItem 추가 by KSM
			//else if ( STRCMP_L( "$DATALOG_DIGITAL_IO" , chr_return1 ) ) { // 2005.08.01 JJY
			else if ( STRCMP_L( "$DATALOG_DIGITAL_IO1" , chr_return1 ) )
			//---> End    2008/08/27  HeaderItem 추가 by KSM
			{ 
				if ( !REMOTE_MONITOR_MODE )
				{
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) )
					{
						//---> Modify 2008/08/27  HeaderItem 추가 by KSM
						//_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_DIGITAL_IO - [%s]-[%d]\n" , FileName , Line );
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_DIGITAL_IO1 - [%s]-[%d]\n" , FileName , Line );
						//---> End    2008/08/27  HeaderItem 추가 by KSM

						_lclose( hFile );
						return FALSE;
					}

					//---> Modify 2008/08/27 HeaderItem 추가 by KSM
					//DATALOG_DIGITAL_IO = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
					//if ( DATALOG_DIGITAL_IO == -1 ) { 
					DATALOG_DIGITAL_IO1 = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
					if ( DATALOG_DIGITAL_IO1 == -1 )
					{ 
					//---> End    2008/08/27 HeaderItem 추가 by KSM

						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_DIGITAL_IO1 %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}

			//---> ADD 2008/08/27 HeaderItem 추가 by KSM
			else if ( STRCMP_L( "$DATALOG_DIGITAL_IO2" , chr_return1 ) )
			{
				if ( !REMOTE_MONITOR_MODE )
				{
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) )
					{
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_DIGITAL_IO2 - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DATALOG_DIGITAL_IO2 = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
					if ( DATALOG_DIGITAL_IO2 == -1 )
					{ 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_DIGITAL_IO2 %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}

			else if ( STRCMP_L( "$DATALOG_DIGITAL_IO3" , chr_return1 ) )
			{
				if ( !REMOTE_MONITOR_MODE )
				{
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) )
					{
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_DIGITAL_IO3 - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DATALOG_DIGITAL_IO3 = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
					if ( DATALOG_DIGITAL_IO3 == -1 )
					{ 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_DIGITAL_IO3 %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			//---> End 2008/08/27 HeaderItem 추가 by KSM

			//---> ADD 2008/08/27 HeaderItem 추가 by KSM
			else if ( STRCMP_L( "$DATALOG_ANALOG_IO1" , chr_return1 ) )
			{
				if ( !REMOTE_MONITOR_MODE )
				{
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) )
					{
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_ANALOG_IO1 - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DATALOG_ANALOG_IO1 = _FIND_FROM_STRING( _KG_A_IO , chr_return2 );  // _KG_A_IO 에 주의 
					if ( DATALOG_ANALOG_IO1 == -1 )
					{ 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_ANALOG_IO1 %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			//---> End 2008/08/27 HeaderItem 추가 by KSM


			else if ( STRCMP_L( "$DATALOG_FILE_SAVE_TYPE" , chr_return1 ) ) {
				if ( !REMOTE_MONITOR_MODE ) { // 2005.03.15
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_FILE_SAVE_TYPE - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( STRCMP_L( chr_return2 , "SPACE" ) ) {
						S_PLOTTER_SAVE_TAB_TYPE( 0 );
					}
					else if ( STRCMP_L( chr_return2 , "TAB" ) ) {
						S_PLOTTER_SAVE_TAB_TYPE( 1 );
					}
					else if ( STRCMP_L( chr_return2 , "SPACE2" ) ) {
						S_PLOTTER_SAVE_TAB_TYPE( 2 );
					}
					else if ( STRCMP_L( chr_return2 , "TAB2" ) ) {
						S_PLOTTER_SAVE_TAB_TYPE( 3 );
					}
					else {
						_IO_CIM_PRINTF( "$DATALOG_FILE_SAVE_TYPE Error 1 - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			else if ( STRCMP_L( "$DATALOG_FILE" , chr_return1 ) ) {
				if ( !REMOTE_MONITOR_MODE ) { // 2005.03.15
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_FILE - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( STRCMP_L( chr_return2 , "$DATE_TIME_BASED" ) ) {
						DATALOG_FILE           = -1;
						DATALOG_DIRECTORY_SEP = FALSE; // 2004.01.09
					}
					else if ( STRCMP_L( chr_return2 , "$DATE_TIME_BASED_SEPARATE" ) ) {
						DATALOG_FILE           = -1;
						DATALOG_DIRECTORY_SEP = TRUE; // 2004.01.09
					}
					else {
						DATALOG_DIRECTORY_SEP = FALSE; // 2004.01.09
						DATALOG_FILE = _FIND_FROM_STRING( _KG_S_IO , chr_return2 );
						if ( DATALOG_FILE == -1 ) { 
							_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DATALOG_FILE %s\n" , chr_return2 );
							_lclose( hFile );
							return FALSE;
						}
					}
				}
			}
			else if ( STRCMP_L( "$DATALOG_TIME" , chr_return1 ) ) {
				if ( !REMOTE_MONITOR_MODE ) { // 2005.03.15
					if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 999 , &DATALOG_TIME ) ) {
						_IO_CIM_PRINTF( "$DATALOG_TIME : Datalog time Data Error(1-999)- [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			else if ( STRCMP_L( "$DATALOG_AUTO_DELETE_TIME" , chr_return1 ) ) {
				if ( !REMOTE_MONITOR_MODE ) { // 2005.03.15
					if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 1 , 999 , &DATALOG_AUTO_DELETE_TIME ) ) {
							_IO_CIM_PRINTF( "$DATALOG_AUTO_DELETE_TIME : Datalog auto delete time Data Error(1-999)- [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ReadCnt > 2 ) { // 2004.01.14
						if ( !Get_Data_From_String_for_INT( Buffer , 2 , chr_return1 , 100 , 9999 , &DATALOG_AUTO_DELETE_COUNT ) ) {
							_IO_CIM_PRINTF( "$DATALOG_AUTO_DELETE_TIME : Datalog auto delete count Data Error(100-9999)- [%s]-[%d]\n" , FileName , Line );
							_lclose( hFile );
							return FALSE;
						}
					}
				}
			}
			else if ( STRCMP_L( "$DATALOG_DIRECTORY" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_STRING2( Buffer , 1 , chr_return1 , 255 , DATALOG_DIRECTORY ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 $DATALOG_DIRECTORY - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
			}
			else if ( STRCMP_L( "$DISPLAY_CONTROL" , chr_return1 ) ) {
				if ( !REMOTE_MONITOR_MODE ) { // 2005.03.15
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $DISPLAY_CONTROL - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					DISPLAY_CONTROL = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
					if ( DISPLAY_CONTROL == -1 ) { 
						_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $DISPLAY_CONTROL %s\n" , chr_return2 );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			else if ( STRCMP_L( "$INFORMATION_FILE" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_STRING2( Buffer , 1 , chr_return1 , 255 , INFORMATION_FILE ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 $INFORMATION_FILE - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
			}
			else if ( STRCMP_L( chr_return1 , "$COLOR_TABLE_FILE" ) ) {
				if ( !GuiDraw ) {
					if ( !Get_Data_From_String_for_STRING2( Buffer , 1 , chr_return1 , 255 , chr_return2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $COLOR_TABLE_FILE - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( !KGUI_PALETTE_Read_PalletteTable_File( NULL , chr_return2 ) ) {
						_IO_CIM_PRINTF( "Open File Error $COLOR_TABLE_FILE - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
			}
			else if ( STRCMP_L( chr_return1 , "$TIME_DISPLAY_MODE" ) ) {
				if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 $TIME_DISPLAY_MODE - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				if      ( STRCMP_L( chr_return2 , "All"      ) ) Console_Time_Display( 0 );
				else if ( STRCMP_L( chr_return2 , "Hour"     ) ) Console_Time_Display( 1 );
				else if ( STRCMP_L( chr_return2 , "Min"      ) ) Console_Time_Display( 2 );
				else if ( STRCMP_L( chr_return2 , "Sec"      ) ) Console_Time_Display( 3 );
				else if ( STRCMP_L( chr_return2 , "Current"  ) ) Console_Time_Display( 4 );
			}
			else if ( STRCMP_L( chr_return1 , "$START_DRAW_MODE" ) ) {
				if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 $START_DRAW_MODE - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				if      ( STRCMP_L( chr_return2 , "RIGHT"    ) ) S_START_LEFT_DRAW( FALSE );
				else if ( STRCMP_L( chr_return2 , "LEFT"     ) ) S_START_LEFT_DRAW( TRUE );
			}
			else if ( STRCMP_L( chr_return1 , "$AUTO_SCALE_MODE" ) ) {
				if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 $AUTO_SCALE_MODE - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				if      ( STRCMP_L( chr_return2 , "OFF"     ) ) {
					S_AUTO_SCALE_DRAW( FALSE );
				}
				else if ( STRCMP_L( chr_return2 , "FALSE"   ) ) {
					S_AUTO_SCALE_DRAW( FALSE );
				}
				else if ( STRCMP_L( chr_return2 , "ON"      ) ) {
					S_AUTO_SCALE_DRAW( TRUE );
					S_AUTO_SCALE_TIME( FALSE );
				}
				else if ( STRCMP_L( chr_return2 , "TRUE"    ) ) {
					S_AUTO_SCALE_DRAW( TRUE );
					S_AUTO_SCALE_TIME( FALSE );
				}
				else if ( STRCMP_L( chr_return2 , "ON(T)"   ) ) {
					S_AUTO_SCALE_DRAW( TRUE );
					S_AUTO_SCALE_TIME( TRUE );
				}
				else if ( STRCMP_L( chr_return2 , "TRUE(T)" ) ) {
					S_AUTO_SCALE_DRAW( TRUE );
					S_AUTO_SCALE_TIME( TRUE );
				}

				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return2 , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 2 $AUTO_SCALE_MODE - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					AUTO_SCALE_POLL_TIME = atoi( chr_return2 );
					if ( AUTO_SCALE_POLL_TIME < 0    ) AUTO_SCALE_POLL_TIME = 1;
					if ( AUTO_SCALE_POLL_TIME > 9999 ) AUTO_SCALE_POLL_TIME = 9999;
				}
				if ( ReadCnt > 3 ) { // 2005.05.24
					if ( !Get_Data_From_String( Buffer , chr_return2 , 3 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 2 $AUTO_SCALE_MODE - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					S_AUTO_SCALE_DEFAULT_DEV( atoi( chr_return2 ) );
					if ( V_AUTO_SCALE_DEFAULT_DEV() < -12 ) S_AUTO_SCALE_DEFAULT_DEV( -12 );
					if ( V_AUTO_SCALE_DEFAULT_DEV() >  12 ) S_AUTO_SCALE_DEFAULT_DEV( 12 );
				}
			}
			else if ( STRCMP_L( chr_return1 , "$AUTO_RETURN_TO_MONITOR_MODE" ) ) { // 2003.02.11
				if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 $AUTO_RETURN_TO_MONITOR_MODE - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
				if      ( STRCMP_L( chr_return2 , "OFF"     ) ) {
					S_AUTO_RETURN_TO_MONITOR( FALSE );
				}
				else if ( STRCMP_L( chr_return2 , "FALSE"   ) ) {
					S_AUTO_RETURN_TO_MONITOR( FALSE );
				}
				else if ( STRCMP_L( chr_return2 , "ON"      ) ) {
					S_AUTO_RETURN_TO_MONITOR( TRUE );
				}
				else if ( STRCMP_L( chr_return2 , "TRUE"    ) ) {
					S_AUTO_RETURN_TO_MONITOR( TRUE );
				}
			}
			else if ( STRCMP_L( chr_return1 , "$WINDOW_TITLE" ) ) {
				if ( !Get_Data_From_String_for_STRING2( Buffer , 1 , chr_return1 , 255 , WINDOW_TITLE ) ) {
					_IO_CIM_PRINTF( "Unknown Error 1 $WINDOW_TITLE - [%s]-[%d]\n" , FileName , Line );
					_lclose( hFile );
					return FALSE;
				}
			}
			else if ( STRCMP_L( chr_return1 , "$EVENT" ) ) {
				if ( !REMOTE_MONITOR_MODE ) { // 2005.03.15
					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 1 $EVENT - [%s]-[%d]\n" , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( STRCMP_L( chr_return2 , "$ALARM" ) ) {
						Type = _KG_F_IO;
						Point = -1;
						strcpy( chr_return3 , "" );
					}
					else {
						Point = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
						if ( Point == -1 ) { 
							Point = _FIND_FROM_STRING( _KG_A_IO , chr_return2 );
							if ( Point == -1 ) { 
								Point = _FIND_FROM_STRING( _KG_S_IO , chr_return2 );
								if ( Point == -1 ) { 
									_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - $EVENT %s\n" , chr_return2 );
									_lclose( hFile );
									return FALSE;
								}
								else {
									Type = _KG_S_IO;
								}
							}
							else {
								Type = _KG_A_IO;
							}
						}
						else {
							Type = _KG_D_IO;
						}
						if ( ReadCnt >= 3 ) {
							if ( !Get_Data_From_String( Buffer , chr_return3 , 2 ) ) {
								_IO_CIM_PRINTF( "$EVENT : Data(%s) Conversion Error [%s] - [%d]\n" , chr_return2 , FileName , Line );
								_lclose( hFile );
								return FALSE;
							}
						}
						else strcpy( chr_return3 , "" );
					}
					if ( !READ_EIO_INFORMATION( chr_return2 , chr_return3 , Type , Point ) ) return FALSE;
				}
			}
			else {
				Tp = TRUE;
				Tgl = 0;
				STR_SEPERATE_CHAR( chr_return1 , '|' , chr_return2 , chr_return3 , 255 );
				if ( REMOTE_MONITOR_MODE ) { // 2005.03.17
					Point = -1;
				}
				else {
					Point = _FIND_FROM_STRING( _KG_A_IO , chr_return2 );
					if ( Point == -1 ) { 
						Tp = FALSE;
						Point = _FIND_FROM_STRING( _KG_D_IO , chr_return2 );
						if ( Point == -1 ) { 
							_IO_CIM_PRINTF( "Unknown Object name in Data IO Table - %s\n" , chr_return1 );
							_lclose( hFile );
							return FALSE;
						}
					}
				}
				//
				if      ( strlen( chr_return3 )     == 0 )   Tgl = -2;
				else if ( tolower( chr_return3[0] ) == 't' ) Tgl = -1;
				else                                         Tgl = atoi( chr_return3 );
				if ( ReadCnt >= 2 ) {
					if ( !Get_Data_From_String_for_STRING( Buffer , 1 , chr_return3 , MAX_STRING_SIZE , chr_return2 ) ) {
						_IO_CIM_PRINTF( "Data(%s) Conversion Error[%s] - [%d]\n" , chr_return1 , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
				else strcpy( chr_return2 , "" );
				if ( ReadCnt >= 3 ) {
					if ( !Get_Data_From_String( Buffer , chr_return3 , 2 ) ) {
						_IO_CIM_PRINTF( "Data(%s) Conversion Error[%s] - [%d]\n" , chr_return1 , FileName , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
				else strcpy( chr_return3 , "" );
				if ( !READ_AIO_INFORMATION( chr_return1 , chr_return3 , Point , chr_return2 , Tp , Tgl , REMOTE_MONITOR_MODE ) ) return FALSE;
			}
		}
	}
	_lclose( hFile );

	time( &DISPLAY_START_TIME );

	if ( !Read_Information_Saved_File( INFORMATION_FILE ) ) return FALSE;

	for ( ReadCnt = 0 ; ReadCnt < ANALOG_CNT ; ReadCnt++ ) {
		if      ( A_IO_PLT_DISPLAY(ReadCnt) == DRAW_1_LEFT  ) {
			if      ( V_PLOTTER_TABLE( SCREEN_1 , 0 ) == -1 ) S_PLOTTER_TABLE( SCREEN_1 , 0 , ReadCnt );
			else if ( V_PLOTTER_TABLE( SCREEN_1 , 1 ) == -1 ) S_PLOTTER_TABLE( SCREEN_1 , 1 , ReadCnt );
		}
		else if ( A_IO_PLT_DISPLAY(ReadCnt) == DRAW_1_RIGHT ) {
			if      ( V_PLOTTER_TABLE( SCREEN_1 , 2 ) == -1 ) S_PLOTTER_TABLE( SCREEN_1 , 2 , ReadCnt );
			else if ( V_PLOTTER_TABLE( SCREEN_1 , 3 ) == -1 ) S_PLOTTER_TABLE( SCREEN_1 , 3 , ReadCnt );
		}
		else if ( A_IO_PLT_DISPLAY(ReadCnt) == DRAW_2_LEFT  ) {
			if      ( V_PLOTTER_TABLE( SCREEN_2 , 0 ) == -1 ) S_PLOTTER_TABLE( SCREEN_2 , 0 , ReadCnt );
			else if ( V_PLOTTER_TABLE( SCREEN_2 , 1 ) == -1 ) S_PLOTTER_TABLE( SCREEN_2 , 1 , ReadCnt );
		}
		else if ( A_IO_PLT_DISPLAY(ReadCnt) == DRAW_2_RIGHT ) {
			if      ( V_PLOTTER_TABLE( SCREEN_2 , 2 ) == -1 ) S_PLOTTER_TABLE( SCREEN_2 , 2 , ReadCnt );
			else if ( V_PLOTTER_TABLE( SCREEN_2 , 3 ) == -1 ) S_PLOTTER_TABLE( SCREEN_2 , 3 , ReadCnt );
		}
	}
	return TRUE;
}
//========================================================================================
//========================================================================================
