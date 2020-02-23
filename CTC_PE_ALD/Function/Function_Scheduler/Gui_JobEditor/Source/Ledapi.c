//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

#include <stdio.h>
#include <windows.h>

#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutlanal.h>
#include <KutlFile.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>


#include "iodefine.h"
#include "Lottype.h"
#include "Ledapi.h"
#include "appprm.h"


//=========================================================================================
//=========================================================================================
#define		JOB30_READ_FORMAT( Name )	_lread( hFile , &(JOB_DATA[JOBCOUNT].##Name##_size) , sizeof(char) ); \
										_lread( hFile , JOB_DATA[JOBCOUNT].##Name## , (JOB_DATA[JOBCOUNT].##Name##_size) ); \
										JOB_DATA[JOBCOUNT].##Name##[(JOB_DATA[JOBCOUNT].##Name##_size)] = 0;
//=========================================================================================
#define		JOB30_WRITE_FORMAT( Name )	_lwrite( hFile , &(JOB_DATA[i].##Name##_size) , sizeof(char) ); \
										_lwrite( hFile , JOB_DATA[i].##Name## , JOB_DATA[i].##Name##_size );
//=========================================================================================
//=========================================================================================
InfoTemplate				INFO_DATA;
InfoTemplate_Old			INFO_DATA_OLD;
//
StepViewTemplate			STEPVIEW_DATA; // 2016.10.11
BOOL						STEPVIEW_SAVE = FALSE; // 2016.10.11
//
//=========================================================================================
int	JOBCOUNT = 0;
JOBStepTemplate_Old			JOB_DATA_OLD;
JOBStepTemplate_Old2		JOB_DATA_OLD2;
JOBStepTemplate				JOB_DATA[MAX_JOB_COUNT];
//
int							JOB_RUN[MAX_JOB_COUNT];
int							JOB_SIDE[MAX_JOB_COUNT];
BOOL						JOB_HANDOFF[MAX_JOB_COUNT];
int							JOB_FIRSTIN[MAX_JOB_COUNT];
int							JOB_LASTIN[MAX_JOB_COUNT];
int							JOB_SEL_LOOPCOUNT[ MAX_JOB_COUNT ];
//
//run queue
//=========================================================================================
int	 FIND_COUNT = 0;
int	 FIND_INDEX[MAX_JOB_COUNT];
char FIND_JID[256];		BOOL FIND_JID_RUN;	int FIND_OPTION1;
char FIND_LID[256];		BOOL FIND_LID_RUN;	int FIND_OPTION2;
char FIND_MID[256];		BOOL FIND_MID_RUN;	int FIND_OPTION3;
char FIND_WID[256];		BOOL FIND_WID_RUN;	int FIND_OPTION4;
BOOL FIND_START_RUN;
int FIND_START_DATE_Y;
int FIND_START_DATE_M;
int FIND_START_DATE_D;
BOOL FIND_END_RUN;
int FIND_END_DATE_Y;
int FIND_END_DATE_M;
int FIND_END_DATE_D;
int FIND_OPTION5;
//=========================================================================================

//void KPLT_Local_Time( long time_data , char *buffer ) {
//	struct tm *time_data2;
//	time_data2 = localtime( &time_data );
//	sprintf( buffer , "%04d/%02d/%02d %02d:%02d:%02d" , time_data2->tm_year+1900 , time_data2->tm_mon + 1 , time_data2->tm_mday , time_data2->tm_hour , time_data2->tm_min , time_data2->tm_sec );
//}

//void KPLT_Local_Time2( long time_data , char *buffer ) {
//	struct tm *time_data2;
//	time_data2 = localtime( &time_data );
//	sprintf( buffer , "%02d:%02d:%02d" , time_data2->tm_hour , time_data2->tm_min , time_data2->tm_sec );
//}

//void KPLT_Local_Time_for_File( long time_data , char *buffer ) {
//	struct tm *time_data2;
//	time_data2 = localtime( &time_data );
//	sprintf( buffer , "%04d%02d%02d_%02d%02d%02d" , time_data2->tm_year+1900 , time_data2->tm_mon + 1 , time_data2->tm_mday , time_data2->tm_hour , time_data2->tm_min , time_data2->tm_sec );
//}

//void KPLT_Elapsed_Time( long time_data , char *buffer ) {
//	time_t finish;
//	long elapsed_time , day , hour , min , sec;
//	time( &finish );
//	elapsed_time = (long) difftime( finish , time_data );
//	day  = ( elapsed_time         ) / 86400;
//	hour = ( elapsed_time % 86400 ) / 3600;
//	min  = ( elapsed_time % 3600  ) / 60;
//	sec  = ( elapsed_time % 60    );
//	if ( day > 0 ) sprintf( buffer , "%d:%d:%d:%d" , day , hour , min , sec );
//	else           sprintf( buffer , "%d:%d:%d" , hour , min , sec );
//}
//void All2FileName( char *Target , const char *Source , int MaxCnt ) {
//	int pnt , i , j;
//	j = (int) strlen( Source );
//	for ( pnt = j ; pnt > 0 ; pnt-- ) if ( Source[ pnt-1 ] == '/' || Source[ pnt-1 ] == '\\' ) break;
//	for ( i = pnt ; ( i < j ) && ( i-pnt < MaxCnt ); i++ ) Target[ i-pnt ] = Source[ i ];
//	Target[ i-pnt ] = 0x00;
//}
//
//
//
void Kor_JOB_Initial() {
	int i;
	memset( JOB_DATA , 0 , sizeof( JOBStepTemplate ) * MAX_JOB_COUNT );
	for ( i = 0 ; i < MAX_JOB_COUNT ; i++ ) JOB_SEL_LOOPCOUNT[i] = 0; // 2012.04.13
}
//
//
BOOL Kor_JOB_Open( char *filename ) {
	char Buffer[256];
	HFILE hFile;
	int i , k = 0 , r;
	JOBCOUNT = 0;
	Kor_JOB_Initial();
	hFile = _lopen( filename , OF_READ );
	if ( hFile == -1 ) return FALSE;
	_lread( hFile , Buffer , 14 ); Buffer[14] = 0;
	if ( strcmp( "JOBDATAFILE1.0" , Buffer ) == 0 ) {
		_lread( hFile , &k , sizeof(int) );
		for ( i = 0 ; i < k ; i++ ) {
			r = _lread( hFile , &JOB_DATA_OLD , sizeof( JOBStepTemplate_Old ) );
			if ( r <= 0 ) {
				_lclose( hFile );
				return TRUE;
			}
			if ( ( strlen( JOB_DATA_OLD.LOT_RECIPE ) > 0 ) && ( JOB_DATA_OLD.IN_CASS >= 0 ) && ( JOB_DATA_OLD.IN_CASS <= 10 ) ) {
				strncpy( JOB_DATA[JOBCOUNT].JOB_NAME , JOB_DATA_OLD.JOB_NAME , MAX_STRING_SIZE_50 );		JOB_DATA[ JOBCOUNT ].JOB_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].JOB_NAME );
				strncpy( JOB_DATA[JOBCOUNT].LOT_RECIPE , JOB_DATA_OLD.LOT_RECIPE , MAX_STRING_SIZE_50 );	JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = strlen( JOB_DATA[ JOBCOUNT ].LOT_RECIPE );
				strcpy( JOB_DATA[JOBCOUNT].MID_NAME , "" );													JOB_DATA[ JOBCOUNT ].MID_NAME_size = 0;
				strncpy( JOB_DATA[JOBCOUNT].RUN_TIME , JOB_DATA_OLD.START_TIME , MAX_STRING_SIZE_22 );		JOB_DATA[ JOBCOUNT ].RUN_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].RUN_TIME );
				strncpy( JOB_DATA[JOBCOUNT].END_TIME , JOB_DATA_OLD.END_TIME , MAX_STRING_SIZE_22 );		JOB_DATA[ JOBCOUNT ].END_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].END_TIME );
				strncpy( JOB_DATA[JOBCOUNT].START_TIME , JOB_DATA_OLD.START_TIME , MAX_STRING_SIZE_22 );	JOB_DATA[ JOBCOUNT ].START_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].START_TIME );
				strcpy( JOB_DATA[JOBCOUNT].ELAPSED_TIME , "" );												JOB_DATA[ JOBCOUNT ].ELAPSED_TIME_size = 0;
				strncpy( JOB_DATA[JOBCOUNT].USER , JOB_DATA_OLD.USER , MAX_STRING_SIZE_22 );				JOB_DATA[ JOBCOUNT ].USER_size = strlen( JOB_DATA[ JOBCOUNT ].USER );
				strncpy( JOB_DATA[JOBCOUNT].DIRECTORY , JOB_DATA_OLD.DIRECTORY , MAX_STRING_SIZE_100 );		JOB_DATA[ JOBCOUNT ].DIRECTORY_size = strlen( JOB_DATA[ JOBCOUNT ].DIRECTORY );
				//
				JOB_DATA[JOBCOUNT].IN_CASS = (char) JOB_DATA_OLD.IN_CASS;
				JOB_DATA[JOBCOUNT].OUT_CASS = (char) JOB_DATA_OLD.OUT_CASS;
				JOB_DATA[JOBCOUNT].START_SLOT = (char) JOB_DATA_OLD.START_SLOT;
				JOB_DATA[JOBCOUNT].END_SLOT = (char) JOB_DATA_OLD.END_SLOT;
				JOB_DATA[JOBCOUNT].METHOD_COUNT = JOB_DATA_OLD.METHOD_COUNT;
				JOB_DATA[JOBCOUNT].EDIT_TYPE = (char) JOB_DATA_OLD.EDIT_TYPE;
				JOB_DATA[JOBCOUNT].PROGRESS = (char) JOB_DATA_OLD.PROGRESS;
				JOB_DATA[JOBCOUNT].DIRECTORY_TYPE = (char) JOB_DATA_OLD.DIRECTORY_TYPE;
				JOB_DATA[JOBCOUNT].DIRECTORY_DATE = JOB_DATA_OLD.DIRECTORY_DATE;
				JOB_DATA[JOBCOUNT].STATUS_SAVE = 0;
				JOBCOUNT++;
				if ( JOBCOUNT >= MAX_JOB_COUNT ) { // 2003.09.18
					_lclose( hFile );
					return TRUE;
				}
			}
//			if ( ( i % 25 ) == 0 ) _sleep(2); // 2015.04.10
			if ( ( i % 25 ) == 0 ) Sleep(2); // 2015.04.10
		}
		_lclose( hFile );
		return TRUE;
	}
	else if ( strcmp( "JOBDATAFILE2.0" , Buffer ) == 0 ) {// 2003.07.30
		_lread( hFile , &k , sizeof(int) );
		for ( i = 0 ; i < k ; i++ ) {
			r = _lread( hFile , &JOB_DATA_OLD2 , sizeof( JOBStepTemplate_Old2 ) );
			if ( r <= 0 ) {
				_lclose( hFile );
				return TRUE;
			}
			if ( ( strlen( JOB_DATA_OLD2.LOT_RECIPE ) > 0 ) && ( JOB_DATA_OLD2.IN_CASS >= 0 ) && ( JOB_DATA_OLD2.IN_CASS <= 10 ) ) {
				strncpy( JOB_DATA[JOBCOUNT].JOB_NAME , JOB_DATA_OLD2.JOB_NAME , MAX_STRING_SIZE_50 );			JOB_DATA[ JOBCOUNT ].JOB_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].JOB_NAME );
				strncpy( JOB_DATA[JOBCOUNT].LOT_RECIPE , JOB_DATA_OLD2.LOT_RECIPE , MAX_STRING_SIZE_50 );		JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = strlen( JOB_DATA[ JOBCOUNT ].LOT_RECIPE );
				strncpy( JOB_DATA[JOBCOUNT].MID_NAME , JOB_DATA_OLD2.MID_NAME , MAX_STRING_SIZE_50 );			JOB_DATA[ JOBCOUNT ].MID_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].MID_NAME );
				strncpy( JOB_DATA[JOBCOUNT].RUN_TIME , JOB_DATA_OLD2.RUN_TIME , MAX_STRING_SIZE_22 );			JOB_DATA[ JOBCOUNT ].RUN_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].RUN_TIME );
				strncpy( JOB_DATA[JOBCOUNT].END_TIME , JOB_DATA_OLD2.END_TIME , MAX_STRING_SIZE_22 );			JOB_DATA[ JOBCOUNT ].END_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].END_TIME );
				strncpy( JOB_DATA[JOBCOUNT].START_TIME , JOB_DATA_OLD2.START_TIME , MAX_STRING_SIZE_22 );		JOB_DATA[ JOBCOUNT ].START_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].START_TIME );
				strncpy( JOB_DATA[JOBCOUNT].ELAPSED_TIME , JOB_DATA_OLD2.ELAPSED_TIME , MAX_STRING_SIZE_22 );	JOB_DATA[ JOBCOUNT ].ELAPSED_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].ELAPSED_TIME );
				strncpy( JOB_DATA[JOBCOUNT].USER , JOB_DATA_OLD2.USER , MAX_STRING_SIZE_22 );					JOB_DATA[ JOBCOUNT ].USER_size = strlen( JOB_DATA[ JOBCOUNT ].USER );
				strncpy( JOB_DATA[JOBCOUNT].DIRECTORY , JOB_DATA_OLD2.DIRECTORY , MAX_STRING_SIZE_100 );		JOB_DATA[ JOBCOUNT ].DIRECTORY_size = strlen( JOB_DATA[ JOBCOUNT ].DIRECTORY );
				//
				JOB_DATA[JOBCOUNT].IN_CASS = (char) JOB_DATA_OLD2.IN_CASS;
				JOB_DATA[JOBCOUNT].OUT_CASS = (char) JOB_DATA_OLD2.OUT_CASS;
				JOB_DATA[JOBCOUNT].START_SLOT = (char) JOB_DATA_OLD2.START_SLOT;
				JOB_DATA[JOBCOUNT].END_SLOT = (char) JOB_DATA_OLD2.END_SLOT;
				JOB_DATA[JOBCOUNT].METHOD_COUNT = JOB_DATA_OLD2.METHOD_COUNT;
				JOB_DATA[JOBCOUNT].EDIT_TYPE = (char) JOB_DATA_OLD2.EDIT_TYPE;
				JOB_DATA[JOBCOUNT].PROGRESS = (char) JOB_DATA_OLD2.PROGRESS;
				JOB_DATA[JOBCOUNT].DIRECTORY_TYPE = (char) JOB_DATA_OLD2.DIRECTORY_TYPE;
				JOB_DATA[JOBCOUNT].DIRECTORY_DATE = JOB_DATA_OLD2.DIRECTORY_DATE;
				JOB_DATA[JOBCOUNT].STATUS_SAVE = JOB_DATA_OLD2.STATUS_SAVE;
				if ( ( JOB_DATA[JOBCOUNT].STATUS_SAVE != 0 ) && ( JOB_DATA[JOBCOUNT].STATUS_SAVE != 1 ) ) {
					JOB_DATA[JOBCOUNT].STATUS_SAVE = 0;
				}
				//
				JOBCOUNT++;
				if ( JOBCOUNT >= MAX_JOB_COUNT ) { // 2003.09.18
					_lclose( hFile );
					return TRUE;
				}
			}
//			if ( ( i % 25 ) == 0 ) _sleep(2); // 2015.04.10
			if ( ( i % 25 ) == 0 ) Sleep(2); // 2015.04.10
		}
		_lclose( hFile );
		return TRUE;
	}
	else if ( strcmp( "JOBDATAFILE3.0" , Buffer ) == 0 ) { // 2003.07.30
		_lread( hFile , &k , sizeof(int) );

		for ( i = 0 ; i < k ; i++ ) {
			r = _lread( hFile , &JOB_DATA[JOBCOUNT] , sizeof( JOBStepTemplate_Head ) );
			if ( r <= 0 ) {
				_lclose( hFile );
				return TRUE;
			}
			//
			JOB30_READ_FORMAT( JOB_NAME )
			JOB30_READ_FORMAT( LOT_RECIPE )
			JOB30_READ_FORMAT( MID_NAME )
			JOB30_READ_FORMAT( RUN_TIME )
			JOB30_READ_FORMAT( END_TIME )
			JOB30_READ_FORMAT( START_TIME )
			JOB30_READ_FORMAT( ELAPSED_TIME )
			JOB30_READ_FORMAT( USER )
			JOB30_READ_FORMAT( DIRECTORY )
			//
			if ( ( JOB_DATA[JOBCOUNT].LOT_RECIPE_size > 0 ) && ( JOB_DATA[JOBCOUNT].IN_CASS >= 0 ) && ( JOB_DATA[JOBCOUNT].IN_CASS <= 10 ) ) {
				if ( ( JOB_DATA[JOBCOUNT].STATUS_SAVE != 0 ) && ( JOB_DATA[JOBCOUNT].STATUS_SAVE != 1 ) ) {
					JOB_DATA[JOBCOUNT].STATUS_SAVE = 0;
				}
				//
				JOBCOUNT++;
				if ( JOBCOUNT >= MAX_JOB_COUNT ) { // 2003.09.18
					_lclose( hFile );
					return TRUE;
				}
			}
//			if ( ( i % 25 ) == 0 ) _sleep(2); // 2004.01.14
//			if ( ( i % 250 ) == 0 ) _sleep(2); // 2004.01.14 // 2015.04.10
			if ( ( i % 250 ) == 0 ) Sleep(2); // 2004.01.14 // 2015.04.10
		}
		_lclose( hFile );
		return TRUE;
	}
	else {
		_lclose( hFile );
		return FALSE;
	}
	return TRUE;
}

//
//
//
//
//
//

BOOL Kor_JOB_Save( char *filename ) {
	int i;
	HFILE hFile;
	hFile = _lcreat( filename , 0 );
	if ( hFile == -1 ) return FALSE;
/*
	// 2003.07.30
	_lwrite( hFile , "JOBDATAFILE2.0" , 14 );
	_lwrite( hFile , &JOBCOUNT , sizeof(int) );
	_lwrite( hFile , JOB_DATA , sizeof( JOBStepTemplate ) * JOBCOUNT );
*/
	_lwrite( hFile , "JOBDATAFILE3.0" , 14 );
	_lwrite( hFile , (char * ) &JOBCOUNT , sizeof(int) );
	for ( i = 0 ; i < JOBCOUNT ; i++ ) {
		_lwrite( hFile , (char * ) &JOB_DATA[i] , sizeof( JOBStepTemplate_Head ) );
		//
		JOB30_WRITE_FORMAT( JOB_NAME )
		JOB30_WRITE_FORMAT( LOT_RECIPE )
		JOB30_WRITE_FORMAT( MID_NAME )
		JOB30_WRITE_FORMAT( RUN_TIME )
		JOB30_WRITE_FORMAT( END_TIME )
		JOB30_WRITE_FORMAT( START_TIME )
		JOB30_WRITE_FORMAT( ELAPSED_TIME )
		JOB30_WRITE_FORMAT( USER )
		JOB30_WRITE_FORMAT( DIRECTORY )
		//
	}
	_lclose( hFile );
	return TRUE;
}

//
//
//
//
//
//
//
//

int Get_Info_LOG_SAVE_MONTH() {	return INFO_DATA.LOG_SAVE_MONTH; }
void Set_Info_LOG_SAVE_MONTH( int data ) {	INFO_DATA.LOG_SAVE_MONTH = data; }

BOOL Get_Info_DUAL_LOT_MODE() {	return INFO_DATA.DUAL_LOT_MODE; }
void Set_Info_DUAL_LOT_MODE( BOOL data ) {	INFO_DATA.DUAL_LOT_MODE = data; }

BOOL Get_Info_DISPLAY_MODE() {	return INFO_DATA.DISPLAY_MODE; }
void Set_Info_DISPLAY_MODE( BOOL data ) {	INFO_DATA.DISPLAY_MODE = data; }

int  Get_Info_AUTO_DELETE_MODE() {	return INFO_DATA.AUTO_DELETE_MODE; }
void Set_Info_AUTO_DELETE_MODE( int data ) {	INFO_DATA.AUTO_DELETE_MODE = data; }

int  Get_Info_END_TIME_SHOW_MODE() {	return INFO_DATA.END_TIME_SHOW_MODE; }
void Set_Info_END_TIME_SHOW_MODE( int data ) {	INFO_DATA.END_TIME_SHOW_MODE = data; }

int  Get_Info_PRE_POST_SHOW_MODE() {	return INFO_DATA.PRE_POST_SHOW_MODE; }
void Set_Info_PRE_POST_SHOW_MODE( int data ) {	INFO_DATA.PRE_POST_SHOW_MODE = data; }

int  Get_Info_DELETE_COUNT() {	return INFO_DATA.DELETE_COUNT; } // 2014.10.15
void Set_Info_DELETE_COUNT( int data ) {	INFO_DATA.DELETE_COUNT = data; } // 2014.10.15

void Kor_Info_Initial() {
	//
	memset( &INFO_DATA_OLD , 0 , sizeof( InfoTemplate_Old ) );
	memset( &INFO_DATA , 0 , sizeof( InfoTemplate ) );
	//
	INFO_DATA.LOG_SAVE_MONTH = 12;
	INFO_DATA.DUAL_LOT_MODE  = 0;
	INFO_DATA.DISPLAY_MODE  = 0;
	INFO_DATA.AUTO_DELETE_MODE = 0;
	INFO_DATA.END_TIME_SHOW_MODE  = 0;
	//
//	INFO_DATA.USER_MODE_03  = 0; // 2006.02.23
	INFO_DATA.PRE_POST_SHOW_MODE  = 0; // 2006.02.23
	//
//	INFO_DATA.USER_MODE_04  = 0; // 2014.10.15
	INFO_DATA.DELETE_COUNT  = 0; // 2014.10.15
	//
	INFO_DATA.USER_MODE_05  = 0;
	INFO_DATA.USER_MODE_06  = 0;
	INFO_DATA.USER_MODE_07  = 0;
	INFO_DATA.USER_MODE_08  = 0;
	INFO_DATA.USER_MODE_09  = 0;
	INFO_DATA.USER_MODE_10  = 0;
	//
}
//
void Kor_StepView_Initial( StepViewTemplate *STD ) {
	//
	// 2016.10.11
	//
	memset( STD , 0 , sizeof( StepViewTemplate ) );
	//
	STD->SV_Opt_Button_Char1[0]					= 0;
	STD->SV_Opt_Button_Char2[0]					= 0;
	//
	STD->SV_Opt_Summary_CharSize				= 0;
	//
	STD->SV_Opt_Summary_BackColor				= COLOR_White;
	STD->SV_Opt_Summary_LineColor				= COLOR_BackGray;
	STD->SV_Opt_Summary_BackColor_Job			= COLOR_Yellow;
	STD->SV_Opt_Summary_BackColor_TimeJob		= COLOR_Yellow;
	STD->SV_Opt_Summary_BackColor_Prcs			= COLOR_PaleGreen;
	STD->SV_Opt_Summary_BackColor_TimePrcs		= COLOR_PaleBlue;
	//
	STD->SV_Opt_Summary_ColumnSize_NameJob		= 100;
	STD->SV_Opt_Summary_ColumnSize_DataJob		= 200;
	STD->SV_Opt_Summary_ColumnSize_NamePrcs		= 100;
	STD->SV_Opt_Summary_ColumnSize_DataPrcs		= 200;
	//
	STD->SV_Opt_Summary_Align_NameJob			= 0;
	STD->SV_Opt_Summary_Align_DataJob			= 0;
	STD->SV_Opt_Summary_Align_NamePrcs			= 0;
	STD->SV_Opt_Summary_Align_DataPrcs			= 0;
	//
	//--------
	//
	//
	STD->SV_Opt_StepItem_CharSize				= 0;
	//
	//
	STD->SV_Opt_Step_BackColor					= COLOR_White;
	STD->SV_Opt_Step_LineColor					= COLOR_BackGray;
	STD->SV_Opt_Step_BackColor_TimeTitle		= COLOR_PaleBlue;
	STD->SV_Opt_Step_BackColor_TimeData			= COLOR_BackGray;
	STD->SV_Opt_Step_BackColor_StepTitle		= COLOR_BrightGreen;
	STD->SV_Opt_Step_BackColor_StepData			= COLOR_BackGray;
	STD->SV_Opt_Step_BackColor_StepPrePost		= COLOR_LightGray;
	//
	STD->SV_Opt_Step_ColumnSize_Step			= 120;
	STD->SV_Opt_Step_ColumnSize_StartTime		= 140;
	STD->SV_Opt_Step_ColumnSize_EndTime			= 140;
	STD->SV_Opt_Step_ColumnSize_ActualTime		= 90;
	STD->SV_Opt_Step_ColumnSize_StableTime		= 90;
	STD->SV_Opt_Step_ColumnSize_StepTime		= 90;
	STD->SV_Opt_Step_ColumnSize_SetTime			= 90;
	STD->SV_Opt_Step_ColumnSize_Result			= 70;
	//
	STD->SV_Opt_Step_View_HidePrePost			= 0;	// Off/On
	STD->SV_Opt_Step_View_HideStartTime			= 0;
	STD->SV_Opt_Step_View_HideEndTime			= 0;
	STD->SV_Opt_Step_View_HideActualTime		= 0;
	STD->SV_Opt_Step_View_HideStableTime		= 0;
	STD->SV_Opt_Step_View_HideStepTime			= 0;
	STD->SV_Opt_Step_View_HideSetTime			= 0;
	STD->SV_Opt_Step_View_HideResult			= 0;
	//
	STD->SV_Opt_Step_Align_TimeTitle			= 0;
	STD->SV_Opt_Step_Align_TimeData				= 0;
	//
	STD->SV_Opt_Step_Align_Step					= 0;
	STD->SV_Opt_Step_Align_StartTime			= 0;
	STD->SV_Opt_Step_Align_EndTime				= 0;
	STD->SV_Opt_Step_Align_ActualTime			= 0;
	STD->SV_Opt_Step_Align_StableTime			= 0;
	STD->SV_Opt_Step_Align_StepTime				= 0;
	STD->SV_Opt_Step_Align_SetTime				= 0;
	STD->SV_Opt_Step_Align_Result				= 0;
	//--------
	//
	STD->SV_Opt_Item_BackColor					= COLOR_White;
	STD->SV_Opt_Item_LineColor					= COLOR_BackGray;
	//
	STD->SV_Opt_Item_ColorMode					= 0; // Column/Row
	//
	STD->SV_Opt_Item_BackColor_Title			= COLOR_BrightGreen;
	STD->SV_Opt_Item_BackColor_No				= COLOR_LightGray;
	STD->SV_Opt_Item_BackColor_Name				= COLOR_BackGray;
	STD->SV_Opt_Item_BackColor_Count			= COLOR_LightGray;
	STD->SV_Opt_Item_BackColor_Target			= COLOR_PaleBlue;
	STD->SV_Opt_Item_BackColor_Start			= COLOR_BackGray;
	STD->SV_Opt_Item_BackColor_End				= COLOR_BackGray;
	STD->SV_Opt_Item_BackColor_Min				= COLOR_LightGray;
	STD->SV_Opt_Item_BackColor_Max				= COLOR_LightGray;
	STD->SV_Opt_Item_BackColor_Avr				= COLOR_BackGray;
	STD->SV_Opt_Item_BackColor_StdDev			= COLOR_BackGray;
	STD->SV_Opt_Item_BackColor_StableTime		= COLOR_PaleGreen;
	STD->SV_Opt_Item_BackColor_StepTime			= COLOR_PaleGreen;
	STD->SV_Opt_Item_BackColor_Result			= COLOR_PaleBlue;
	//
	STD->SV_Opt_Item_BackColor_Row1				= COLOR_BackGray;
	STD->SV_Opt_Item_BackColor_Row2				= COLOR_LightGray;
	//
	STD->SV_Opt_Item_ColumnSize_No				= 35;
	STD->SV_Opt_Item_ColumnSize_Name			= 140;
	STD->SV_Opt_Item_ColumnSize_Target			= 50;
	STD->SV_Opt_Item_ColumnSize_Count			= 50;
	STD->SV_Opt_Item_ColumnSize_Start			= 50;
	STD->SV_Opt_Item_ColumnSize_End				= 50;
	STD->SV_Opt_Item_ColumnSize_Min				= 50;
	STD->SV_Opt_Item_ColumnSize_Max				= 50;
	STD->SV_Opt_Item_ColumnSize_Avr				= 50;
	STD->SV_Opt_Item_ColumnSize_StdDev			= 50;
	STD->SV_Opt_Item_ColumnSize_StblTime		= 70;
	STD->SV_Opt_Item_ColumnSize_StepTime		= 70;
	STD->SV_Opt_Item_ColumnSize_Result			= 80;
	//
	STD->SV_Opt_Item_RowSize_Row1				= 0;
	STD->SV_Opt_Item_RowSize_Row2				= 0;
	//
	STD->SV_Opt_Item_View_HideCount				= 0;
	STD->SV_Opt_Item_View_HideTarget			= 1;
	STD->SV_Opt_Item_View_HideStart				= 0;
	STD->SV_Opt_Item_View_HideEnd				= 0;
	STD->SV_Opt_Item_View_HideMin				= 0;
	STD->SV_Opt_Item_View_HideMax				= 0;
	STD->SV_Opt_Item_View_HideAvr				= 0;
	STD->SV_Opt_Item_View_HideStdDev			= 0;
	STD->SV_Opt_Item_View_HideStblTime			= 0;
	STD->SV_Opt_Item_View_HideStepTime			= 0;
	STD->SV_Opt_Item_View_HideResult			= 0;
	//
	STD->SV_Opt_Item_Align_No					= 0;
	STD->SV_Opt_Item_Align_Name					= 0;
	STD->SV_Opt_Item_Align_Target				= 0;
	STD->SV_Opt_Item_Align_Count				= 0;
	STD->SV_Opt_Item_Align_Start				= 0;
	STD->SV_Opt_Item_Align_End					= 0;
	STD->SV_Opt_Item_Align_Min					= 0;
	STD->SV_Opt_Item_Align_Max					= 0;
	STD->SV_Opt_Item_Align_Avr					= 0;
	STD->SV_Opt_Item_Align_StdDev				= 0;
	STD->SV_Opt_Item_Align_StblTime				= 0;
	STD->SV_Opt_Item_Align_StepTime				= 0;
	STD->SV_Opt_Item_Align_Result				= 0;

	STD->SV_Opt_Item_Cal_ExceptStable			= 0;
	//
	STD->SV_Opt_Item_Time_Calculation			= 0;
	//
	STD->SV_Opt_TimeDisplay_Run					= 0;
	STD->SV_Opt_TimeDisplay_Cal					= 0;
	STD->SV_Opt_TimeDisplay_Unknown				= 0;
	STD->SV_Opt_DecimalPoint					= 0;
	//
}

//

BOOL Kor_Info_Open( char *filename ) {
	char Buffer[20];
	HFILE hFile;
	//
	Kor_Info_Initial();
	//
	Kor_StepView_Initial( &STEPVIEW_DATA ); // 2016.10.11
	//
	STEPVIEW_SAVE = FALSE;
	//
	hFile = _lopen( filename , OF_READ );
	if ( hFile == -1 ) return TRUE;
	_lread( hFile , Buffer , 14 ); Buffer[14] = 0;
	if ( strcmp( "JOBINFOFILE1.0" , Buffer ) == 0 ) {
		_lread( hFile , &INFO_DATA_OLD , sizeof( InfoTemplate_Old ) );
		_lclose( hFile );
		INFO_DATA.LOG_SAVE_MONTH = INFO_DATA_OLD.LOG_SAVE_MONTH;
		INFO_DATA.DUAL_LOT_MODE  = INFO_DATA_OLD.DUAL_LOT_MODE;
	}
	else if ( strcmp( "JOBINFOFILE2.0" , Buffer ) == 0 ) {
		_lread( hFile , &INFO_DATA , sizeof( InfoTemplate ) );
		_lclose( hFile );
	}
	else if ( strcmp( "JOBINFOFILE3.0" , Buffer ) == 0 ) { // 2016.10.11
		//
		_lread( hFile , &INFO_DATA , sizeof( InfoTemplate ) );
		_lread( hFile , &STEPVIEW_DATA , sizeof( StepViewTemplate ) );
		_lclose( hFile );
		//
		STEPVIEW_SAVE = TRUE;
		//
	}
	else {
		_lclose( hFile );
		return FALSE;
	}
	return TRUE;
}

//
//
//
//
//
//

BOOL Kor_Info_Save( char *filename ) {
	HFILE hFile;
	//
	if ( STEPVIEW_SAVE ) { // 2016.10.11
		//
		hFile = _lcreat( filename , 0 );
		if ( hFile == -1 ) return FALSE;
		_lwrite( hFile , "JOBINFOFILE3.0" , 14 );
		_lwrite( hFile , (char * ) &INFO_DATA , sizeof( InfoTemplate ) );
		_lwrite( hFile , (char * ) &STEPVIEW_DATA , sizeof( StepViewTemplate ) );
		_lclose( hFile );
		//
	}
	else {
		hFile = _lcreat( filename , 0 );
		if ( hFile == -1 ) return FALSE;
		_lwrite( hFile , "JOBINFOFILE2.0" , 14 );
		_lwrite( hFile , (char * ) &INFO_DATA , sizeof( InfoTemplate ) );
		_lclose( hFile );
	}
	return TRUE;
}














