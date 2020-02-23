//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

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
#include <Kutlanal.h>

#include "iodefine.h"
#include "appprm.h"

#include "cimseqnc.h"
//----------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int  LOG_ENCRIPTION; // 2010.04.14
//------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
char SYS_DIRECTORY[ 128 + 1 ];
char LOG_DIRECTORY[ 128 + 1 ];
char METHOD_STRING[ 128 + 1 ];
char JID_STRING[ 128 + 1 ];
char LID_STRING[ 128 + 1 ];
char MID_STRING[ 128 + 1 ];
char WID_STRING[ 128 + 1 ]; // 2005.02.15
//----------------------------------------------------------------------------------
char PRE_STRING[ 128 + 1 ] = { 0 , }; // 2018.09.04
char POST_STRING[ 128 + 1 ] = { 0 , }; // 2018.09.04
//----------------------------------------------------------------------------------
char	FILE_ID[256];
char	LOT_RECIPE_ID[256];

char	MAIN_RECIPE_PATH[256];
char	GROUP_RECIPE_PATH[256];
char	LOT_RECIPE_PATH[256];
char	CLUSTER_RECIPE_PATH[256];
char	PROCESS_RECIPE_PATH[MAX_CHAMBER][256];
char	PROCESS_RECIPE_HEAD_PATH[MAX_CHAMBER][256];

BOOL	MODULE_MODE[MAX_CHAMBER];

int		MAX_CASSETTE_SLOT				= 25;
int		MAX_CASSETTE_COUNT				= 2;

//
//
BOOL	MINISCREEN_MODE		= FALSE;
//
BOOL	FIND_MODE			= FALSE;
//
BOOL	NOTANAL_MODE		= FALSE;
//
BOOL	SAVEFILE_MODE		= FALSE;
//
BOOL	SCROLLBUTTON_MODE	= FALSE;
//
BOOL	GROUP_RUN_CHECK		= FALSE;
//
BOOL	TITLE_MODE			= FALSE;
//
BOOL	WINDOW_RETURN_MODE	= FALSE;
//
BOOL	NETMONITOR_MODE		= FALSE;
char	NETMONITOR_FOLDER[256]; // 2003.12.18
//
int		SIMPLE_DISPLAY = 0;
//
BOOL	LOT_FILE_CHANGE	= FALSE;

int		EXTENDVIEW_MODE = 0; // 2005.02.15

BOOL	DELETE_HIDE = FALSE; // 2006.08.30

BOOL	SORT_MODE = FALSE; // 2009.09.04

BOOL	DISPLAY_MODE = FALSE; // 2010.11.30

int     TREE_ITEM_DISPLAY = 0; // 2011.07.15

BOOL	STATUS_VIEW = FALSE; // 2014.07.17
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int CONTROL_RECIPE_TYPE = 0;
int CONTROL_FIXED_CASSETTE_IN	= -1;
int CONTROL_FIXED_CASSETTE_OUT	= -1;
int CONTROL_FIXED_SLOT_START	= -1;
int CONTROL_FIXED_SLOT_END		= -1;
//----------------------------------------------------------------------------
BOOL EXCLUSIVE_JOB_CHECK[MAX_SIDE];
//----------------------------------------------------------------------------
int READONLY_LEVEL = 0;
int READONLY_LEVEL_IO = -2;
char READONLY_LEVEL_IO_NAME[256];
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//=======================================================================================
//=======================================================================================
char _chamber_String[MAX_CHAMBER][8+1];
BOOL _chamber_Change[MAX_CHAMBER];

void _SET_CHAMBER_STRING( int ch , char *data ) {
//	strncpy( _chamber_String[ch] , data , 31 ); // 2007.05.25
//	_chamber_String[ch][30] = 0; // 2007.05.25
	strncpy( _chamber_String[ch] , data , 8 ); // 2007.05.25
	_chamber_String[ch][8] = 0; // 2007.05.25
	if ( strlen( data ) > 0 ) _chamber_Change[ch] = TRUE;
	else                      _chamber_Change[ch] = FALSE;
}
//
char *_GET_CHAMBER_STRING( int ch ) {
	return _chamber_String[ch];
}
//
BOOL _GET_CHAMBER_STRING_POSSIBLE( int ch ) {
	return _chamber_Change[ch];
}
//
//
void CHAMBER_STRING_CHANGE_FOR_CM( char *datas , char *datat ) { // 2006.10.26
	int s , c1 , c2;

	if (
		( toupper( datas[0] ) == 'C' ) &&
		( toupper( datas[1] ) == 'M' ) &&
		( datas[2] >= '1' ) &&
		( datas[2] <= '9' ) &&
		( datas[3] == '-' ) &&
		( datas[4] == '>' ) &&
		( toupper( datas[5] ) == 'C' ) &&
		( toupper( datas[6] ) == 'M' ) &&
		( datas[7] >= '1' ) &&
		( datas[7] <= '9' ) &&
		( toupper( datas[8] ) == ':' ) ) {
		c1 = datas[2] - '0';
		c2 = datas[7] - '0';
		s = atoi( datas + 9 );

		if      ( ( !_chamber_Change[c1] ) && ( !_chamber_Change[c2] ) ) {
			strcpy( datat , datas );
		}
		else if ( ( _chamber_Change[c1] ) && ( !_chamber_Change[c2] ) ) {
//			sprintf( datat , "%s->CM%d:%d" , _chamber_String[c1] , c2 , 2 ); // 2009.07.20
			sprintf( datat , "%s->CM%d:%d" , _chamber_String[c1] , c2 , s ); // 2009.07.20
		}
		else if ( ( !_chamber_Change[c1] ) && ( _chamber_Change[c2] ) ) {
//			sprintf( datat , "CM%d->%s:%d" , c1 , _chamber_String[c2] , 2 ); // 2009.07.20
			sprintf( datat , "CM%d->%s:%d" , c1 , _chamber_String[c2] , s ); // 2009.07.20
		}
		else {
//			sprintf( datat , "%s->%s:%d" , _chamber_String[c1] , _chamber_String[c2] , 2 ); // 2009.07.20
			sprintf( datat , "%s->%s:%d" , _chamber_String[c1] , _chamber_String[c2] , s ); // 2009.07.20
		}
	}
	else {
		strcpy( datat , datas );
	}
}
//

void CHAMBER_STRING_CHANGE( char *datas , char *datat ) {
	int i;
	if ( STRNCMP_L( datas , "PM" , 2 ) ) {
		i = atoi( datas + 2 ) - 1 + PM1;
		if ( i >= PM1 ) {
			if ( ( i - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) {
				if ( _chamber_Change[i] ) {
					strcpy( datat , _chamber_String[i] );
				}
				else {
					strcpy( datat , datas );
				}
				return;
			}
		}
	}
	if ( STRNCMP_L( datas , "P" , 1 ) ) {
		i = atoi( datas + 1 ) - 1 + PM1;
		if ( i >= PM1 ) {
			if ( ( i - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) {
				if ( _chamber_Change[i] ) {
					strcpy( datat , _chamber_String[i] );
				}
				else {
					strcpy( datat , datas );
				}
				return;
			}
		}
	}
	//-----------------------------------------------------------------
	// 2008.04.16
	//-----------------------------------------------------------------
	if ( STRNCMP_L( datas , "BM" , 2 ) ) {
		i = atoi( datas + 2 ) - 1 + BM1;
		if ( i >= BM1 ) {
			if ( ( i - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) {
				if ( _chamber_Change[i] ) {
					strcpy( datat , _chamber_String[i] );
				}
				else {
					strcpy( datat , datas );
				}
				return;
			}
		}
	}
	if ( STRNCMP_L( datas , "B" , 1 ) ) {
		i = atoi( datas + 1 ) - 1 + BM1;
		if ( i >= BM1 ) {
			if ( ( i - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) {
				if ( _chamber_Change[i] ) {
					strcpy( datat , _chamber_String[i] );
				}
				else {
					strcpy( datat , datas );
				}
				return;
			}
		}
	}
	//-----------------------------------------------------------------
//	strcpy( datat , datas ); // 2015.12.03
	CHAMBER_STRING_CHANGE_FOR_CM( datas , datat ); // 2015.12.03
}
//
int CHAMBER_STRING_FIND( char *data ) {
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( _chamber_Change[i] ) {
			if ( STRCMP_L( _chamber_String[i] , data ) ) {
				return i;
			}
		}
	}
	return -1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_CONTROL_RECIPE_TYPE( int data ) {	CONTROL_RECIPE_TYPE = data; }
int  Get_Prm_CONTROL_RECIPE_TYPE() { return CONTROL_RECIPE_TYPE; }
void Set_Prm_CONTROL_FIXED_CASSETTE_IN( int data ) { CONTROL_FIXED_CASSETTE_IN = data; }
int  Get_Prm_CONTROL_FIXED_CASSETTE_IN() { return CONTROL_FIXED_CASSETTE_IN; }
void Set_Prm_CONTROL_FIXED_CASSETTE_OUT( int data ) { CONTROL_FIXED_CASSETTE_OUT = data; }
int  Get_Prm_CONTROL_FIXED_CASSETTE_OUT() { return CONTROL_FIXED_CASSETTE_OUT; }
void Set_Prm_CONTROL_FIXED_SLOT_START( int data ) {	CONTROL_FIXED_SLOT_START = data; }
int  Get_Prm_CONTROL_FIXED_SLOT_START() { return CONTROL_FIXED_SLOT_START; }
void Set_Prm_CONTROL_FIXED_SLOT_END( int data ) { CONTROL_FIXED_SLOT_END = data; }
int  Get_Prm_CONTROL_FIXED_SLOT_END() { return CONTROL_FIXED_SLOT_END; }
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_SYSTEM_DIRECTORY( char *data )	{ strncpy( SYS_DIRECTORY , data , 128 ); }
char *Get_Prm_SYSTEM_DIRECTORY()				{ return SYS_DIRECTORY; }
//----------------------------------------------------------------------------
void Set_Prm_LOG_DIRECTORY( char *data )	{ strncpy( LOG_DIRECTORY , data , 128 ); }
char *Get_Prm_LOG_DIRECTORY()				{ return LOG_DIRECTORY; }
//----------------------------------------------------------------------------
void Set_Prm_METHOD_STRING( char *data )	{ strncpy( METHOD_STRING , data , 128 ); }
char *Get_Prm_METHOD_STRING()				{ return METHOD_STRING; }
//----------------------------------------------------------------------------
void Set_Prm_JID_STRING( char *data )	{ strncpy( JID_STRING , data , 128 ); }
char *Get_Prm_JID_STRING()				{ return JID_STRING; }
//----------------------------------------------------------------------------
void Set_Prm_LID_STRING( char *data )	{ strncpy( LID_STRING , data , 128 ); }
char *Get_Prm_LID_STRING()				{ return LID_STRING; }
//----------------------------------------------------------------------------
void Set_Prm_MID_STRING( char *data )	{ strncpy( MID_STRING , data , 128 ); }
char *Get_Prm_MID_STRING()				{ return MID_STRING; }
//----------------------------------------------------------------------------
void Set_Prm_WID_STRING( char *data )	{ strncpy( WID_STRING , data , 128 ); } // 2005.02.15
char *Get_Prm_WID_STRING()				{ return WID_STRING; } // 2005.02.15
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
void Set_Prm_PRE_STRING( char *data )	{ strncpy( PRE_STRING , data , 128 ); } // 2018.09.04
char *Get_Prm_PRE_STRING()				{ return PRE_STRING; } // 2018.09.04
//
void Set_Prm_POST_STRING( char *data )	{ strncpy( POST_STRING , data , 128 ); } // 2018.09.04
char *Get_Prm_POST_STRING()				{ return POST_STRING; } // 2018.09.04
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_FILE_ID( char *data )			{ strncpy( FILE_ID , data , 255 ); }
char *Get_Prm_FILE_ID()						{ return FILE_ID; }

void Set_Prm_LOT_RECIPE_ID( char *data )	{ strncpy( LOT_RECIPE_ID , data , 255 ); }
char *Get_Prm_LOT_RECIPE_ID()				{ return LOT_RECIPE_ID; }

//
void Set_Prm_MAIN_RECIPE_PATH( char *data )	{ strcpy( MAIN_RECIPE_PATH , data ); }
char *Get_Prm_MAIN_RECIPE_PATH()				{ return MAIN_RECIPE_PATH; }

void Set_Prm_GROUP_RECIPE_PATH( char *data )	{ strcpy( GROUP_RECIPE_PATH , data ); }
char *Get_Prm_GROUP_RECIPE_PATH()				{ return GROUP_RECIPE_PATH; }

void Set_Prm_LOT_RECIPE_PATH( char *data )	{ strcpy( LOT_RECIPE_PATH , data ); }
char *Get_Prm_LOT_RECIPE_PATH()				{ return LOT_RECIPE_PATH; }

void Set_Prm_CLUSTER_RECIPE_PATH( char *data )	{ strcpy( CLUSTER_RECIPE_PATH , data ); }
char *Get_Prm_CLUSTER_RECIPE_PATH()				{ return CLUSTER_RECIPE_PATH; }

void Set_Prm_PROCESS_RECIPE_PATH( int i , char *data )	{ strcpy( PROCESS_RECIPE_PATH[i] , data ); }
char *Get_Prm_PROCESS_RECIPE_PATH( int i )				{ return PROCESS_RECIPE_PATH[i]; }

void Set_Prm_PROCESS_RECIPE_HEAD_PATH( int i , char *data )	{ strcpy( PROCESS_RECIPE_HEAD_PATH[i] , data ); }
char *Get_Prm_PROCESS_RECIPE_HEAD_PATH( int i )				{ return PROCESS_RECIPE_HEAD_PATH[i]; }


void Set_Prm_MAX_CASSETTE_SLOT( int data )		{ MAX_CASSETTE_SLOT = data; }
int  Get_Prm_MAX_CASSETTE_SLOT()				{ return MAX_CASSETTE_SLOT; }

void Set_Prm_MAX_CASSETTE_COUNT( int data )		{ MAX_CASSETTE_COUNT = data; }
int  Get_Prm_MAX_CASSETTE_COUNT()				{ return MAX_CASSETTE_COUNT; }
//
//
//
void Set_Prm_SIMPLE_DISPLAY( int data )		{ SIMPLE_DISPLAY = data; }
int  Get_Prm_SIMPLE_DISPLAY()				{ return SIMPLE_DISPLAY; }
//
void Set_Prm_TREE_ITEM_DISPLAY( int data )		{ TREE_ITEM_DISPLAY = data; }
int  Get_Prm_TREE_ITEM_DISPLAY()				{ return TREE_ITEM_DISPLAY; }
//
//
void Set_LOT_FILE_CHANGE( BOOL data ) { LOT_FILE_CHANGE = data; }
BOOL Get_LOT_FILE_CHANGE() { return LOT_FILE_CHANGE; }

void Set_GROUP_RUN_CHECK( BOOL data ) { GROUP_RUN_CHECK = data; }
BOOL Get_GROUP_RUN_CHECK() { return GROUP_RUN_CHECK; }

void Set_Prm_MINISCREEN_MODE( int data )		{ MINISCREEN_MODE = data; }
BOOL Get_Prm_MINISCREEN_MODE()				{ return MINISCREEN_MODE; }
//----------------------------------------------------------------------------
void Set_Prm_FIND_MODE( int data )		{ FIND_MODE = data; }
BOOL Get_Prm_FIND_MODE()				{ return FIND_MODE; }
//----------------------------------------------------------------------------
void Set_Prm_NOTANAL_MODE( int data )		{ NOTANAL_MODE = data; }
BOOL Get_Prm_NOTANAL_MODE()				{ return NOTANAL_MODE; }
//----------------------------------------------------------------------------
void Set_Prm_SAVEFILE_MODE( int data )		{ SAVEFILE_MODE = data; }
BOOL Get_Prm_SAVEFILE_MODE()				{ return SAVEFILE_MODE; }
//----------------------------------------------------------------------------
void Set_Prm_SCROLLBUTTON_MODE( int data )		{ SCROLLBUTTON_MODE = data; }
BOOL Get_Prm_SCROLLBUTTON_MODE()				{ return SCROLLBUTTON_MODE; }
//----------------------------------------------------------------------------
void Set_Prm_EXCLUSIVE_JOB_CHECK( int side , BOOL data )	{ EXCLUSIVE_JOB_CHECK[side] = data; }
BOOL Get_Prm_EXCLUSIVE_JOB_CHECK( int side )				{ return EXCLUSIVE_JOB_CHECK[side]; }
//----------------------------------------------------------------------------
void Set_Prm_TITLE_MODE( int data )		{ TITLE_MODE = data; }
BOOL Get_Prm_TITLE_MODE()				{ return TITLE_MODE; }
//----------------------------------------------------------------------------
void Set_Prm_WINDOW_RETURN_MODE( int data )		{ WINDOW_RETURN_MODE = data; }
BOOL Get_Prm_WINDOW_RETURN_MODE()				{ return WINDOW_RETURN_MODE; }
//----------------------------------------------------------------------------
void Set_Prm_NETMONITOR_MODE( int data )		{ NETMONITOR_MODE = data; } // 2003.12.18
BOOL Get_Prm_NETMONITOR_MODE()				{ return NETMONITOR_MODE; } // 2003.12.18
//----------------------------------------------------------------------------
void Set_Prm_NETMONITOR_FOLDER( char *data )		{ strncpy( NETMONITOR_FOLDER , data , 255 ); } // 2003.12.18
char *Get_Prm_NETMONITOR_FOLDER()				{ return NETMONITOR_FOLDER; } // 2003.12.18
//----------------------------------------------------------------------------
void Set_Prm_EXTENDVIEW_MODE( int data )		{ EXTENDVIEW_MODE = data; } // 2005.02.15
int  Get_Prm_EXTENDVIEW_MODE()					{ return EXTENDVIEW_MODE; } // 2005.02.15
//----------------------------------------------------------------------------
void Set_Prm_DELETE_HIDE( int data )			{ DELETE_HIDE = data; } // 2006.08.30
BOOL Get_Prm_DELETE_HIDE()						{ return DELETE_HIDE; } // 2006.08.30
//----------------------------------------------------------------------------
void Set_Prm_SORT_MODE( int data )				{ SORT_MODE = data; } // 2009.09.04
BOOL Get_Prm_SORT_MODE()						{ return SORT_MODE; } // 2009.09.04
//----------------------------------------------------------------------------
void Set_Prm_DISPLAY_MODE( int data )			{ DISPLAY_MODE = data; } // 2010.11.30
BOOL Get_Prm_DISPLAY_MODE()						{ return DISPLAY_MODE; } // 2010.11.30
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_STATUS_VIEW( int data )			{ STATUS_VIEW = data; } // 2014.07.17
BOOL Get_Prm_STATUS_VIEW()						{ return STATUS_VIEW; } // 2014.07.17
//----------------------------------------------------------------------------
void Set_Prm_MODULE_MODE( int i , BOOL data )	{ MODULE_MODE[i] = data; }
BOOL Get_Prm_MODULE_MODE( int i )				{ return MODULE_MODE[i]; }
//
//----------------------------------------------------------------------------
int Check_Single_Chamber_Use() {
	int i , j = -1;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( Get_Prm_MODULE_MODE( i + PM1 ) ) {
			if ( j == -1 ) j = i;
		}
	}
	return j;
}

int GET_CURRENT_USERLEVEL();
void Set_Prm_READONLY_LEVEL( int data )		{ READONLY_LEVEL = data; } // 2002.11.08
BOOL Get_Prm_READONLY_MODE() { // 2002.11.08
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


DWORD Process_Log_File_Get_Size( char *filename ) { // 2014.02.06
	DWORD dwFilesize; 
	HANDLE hFile;

	hFile = CreateFileA( filename , GENERIC_READ | GENERIC_WRITE , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL ); 
	if ( hFile != INVALID_HANDLE_VALUE ) {
		dwFilesize = GetFileSize( hFile , NULL ); 
		CloseHandle( hFile ); 
	}
	else {
		dwFilesize = 0;
	}
	return dwFilesize;
}
BOOL Process_Log_File_Is_Empty( char *filename ) { // 2007.06.25
	FILE *fpt;
	BOOL bres;
	char buffer[1025];
	int i;
	int encflag;

	if ( ( fpt = fopen( filename , "r" ) ) == NULL ) return FALSE;

	encflag = 0; // 2011.12.07
	//
	i = 0;
	do {
		bres = Read_Line_From_File4( fpt , buffer , 1024 );
		//
//		Make_Simple_Encryption( LOG_ENCRIPTION , buffer ); // 2010.04.15
		Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , buffer ); // 2011.12.07
		//
		if ( ( *buffer != 0 ) && ( *buffer != '$' ) ) {
			// Title
			i++;
			if ( i >= 2 ) {
				fclose( fpt );
				return FALSE;
			}
		}
	}
	while ( bres );
	fclose( fpt );
	return TRUE;
}