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

#include "iodefine.h"
#include "appprm.h"

#include "cimseqnc.h"
#include <CimSeqncKNFS.h>
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
int		CHAMBER_SIZE = 4;
int		DRAW_SIZE = 4;
//----------------------------------------------------------------------------------
char	FILE_ID[MAX_STRING_SIZE+1];

char	CLUSTER_RECIPE_ID[MAX_STRING_SIZE+1];

char	PROCESS_NAME[MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];
char	PROCESS_RECIPE_ID[MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];

char	SCHEDULER_PATH[MAX_STRING_SIZE+1];
char	SCHEDULER_CloseString[MAX_STRING_SIZE+1];

char	MAIN_RECIPE_PATH[MAX_STRING_SIZE+1];
char	GROUP_RECIPE_PATH[MAX_STRING_SIZE+1];
char	LOT_RECIPE_PATH[MAX_STRING_SIZE+1];
char	CLUSTER_RECIPE_PATH[MAX_STRING_SIZE+1];

char	PROCESS_RECIPE_PATH[MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];
char	PROCESS_RECIPE_HEAD_PATH[MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];

BOOL	MODULE_MODE[MAX_PM_CHAMBER_DEPTH];

BOOL	CLUSTER_RECIPE_PARALLEL_SUPPORT	= FALSE;
//
char	PROCESS_RECIPE[MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];
//
//
//
//
BOOL	CLUSTER_FILE_CHANGE	= FALSE;
BOOL	GROUP_RUN_CHECK		= FALSE;
BOOL	HANDLING_SIDE_CHECK = FALSE;
int		CLUSTER_USE_TYPE_ITEM = 1;
int		READONLY_LEVEL		= 0;
int		READONLY_LEVEL_IO	= -2;
char	READONLY_LEVEL_IO_NAME[256];
int		NAME_STYLE			= 0; // 2004.06.15
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BOOL	MONITOR_MODE		= FALSE;
char 	MONITOR_FOLDER[64];
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_CHAMBER_SIZE( int size ) { CHAMBER_SIZE = size; }
int  Get_Prm_CHAMBER_SIZE() { return CHAMBER_SIZE; }
//----------------------------------------------------------------------------
void Set_Prm_DRAW_SIZE( int size ) { DRAW_SIZE = size; }
int  Get_Prm_DRAW_SIZE() { return DRAW_SIZE; }
//----------------------------------------------------------------------------
void Set_Prm_FILE_ID( char *data )			{ strncpy( FILE_ID , data , MAX_STRING_SIZE ); }
char *Get_Prm_FILE_ID()						{ return FILE_ID; }
//
void Set_Prm_CLUSTER_RECIPE_ID( char *data )	{ strncpy( CLUSTER_RECIPE_ID , data , MAX_STRING_SIZE ); }
char *Get_Prm_CLUSTER_RECIPE_ID()				{ return CLUSTER_RECIPE_ID; }
//
void Set_Prm_PROCESS_RECIPE_ID( int i , char *data ){ strncpy( PROCESS_RECIPE_ID[i] , data , MAX_STRING_SIZE ); }
char *Get_Prm_PROCESS_RECIPE_ID( int i )			{ return PROCESS_RECIPE_ID[i]; }
//
void Set_Prm_Scheduler_Path( char *data )	{ strncpy( SCHEDULER_PATH , data , MAX_STRING_SIZE ); }
char *Get_Prm_Scheduler_Path()				{ return SCHEDULER_PATH; }
//
void Set_Prm_Scheduler_CloseString( char *data )	{ strncpy( SCHEDULER_CloseString , data , MAX_STRING_SIZE ); }
char *Get_Prm_Scheduler_CloseString()				{ return SCHEDULER_CloseString; }
//
void Set_Prm_MAIN_RECIPE_PATH( char *data )	{ strncpy( MAIN_RECIPE_PATH , data , MAX_STRING_SIZE ); }
char *Get_Prm_MAIN_RECIPE_PATH()				{ return MAIN_RECIPE_PATH; }
//
void Set_Prm_GROUP_RECIPE_PATH( char *data )	{ strncpy( GROUP_RECIPE_PATH , data , MAX_STRING_SIZE ); }
char *Get_Prm_GROUP_RECIPE_PATH()				{ return GROUP_RECIPE_PATH; }
//
void Set_Prm_LOT_RECIPE_PATH( char *data )	{ strcpy( LOT_RECIPE_PATH , data ); }
char *Get_Prm_LOT_RECIPE_PATH()				{ return LOT_RECIPE_PATH; }
//
void Set_Prm_CLUSTER_RECIPE_PATH( char *data )	{ strncpy( CLUSTER_RECIPE_PATH , data , MAX_STRING_SIZE ); }
char *Get_Prm_CLUSTER_RECIPE_PATH()				{ return CLUSTER_RECIPE_PATH; }
//
void Set_Prm_PROCESS_RECIPE_PATH( int i , char *data )	{ strncpy( PROCESS_RECIPE_PATH[i] , data , MAX_STRING_SIZE ); }
char *Get_Prm_PROCESS_RECIPE_PATH( int i )				{ return PROCESS_RECIPE_PATH[i]; }
//
void Set_Prm_PROCESS_RECIPE_HEAD_PATH( int i , char *data )	{ strncpy( PROCESS_RECIPE_HEAD_PATH[i] , data , MAX_STRING_SIZE ); }
char *Get_Prm_PROCESS_RECIPE_HEAD_PATH( int i )				{ return PROCESS_RECIPE_HEAD_PATH[i]; }

void Set_Prm_MODULE_MODE( int i , BOOL data )	{ MODULE_MODE[i] = data; }
BOOL Get_Prm_MODULE_MODE( int i )				{ return MODULE_MODE[i]; }
//
//
void Set_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT( BOOL data )	{ CLUSTER_RECIPE_PARALLEL_SUPPORT = data; }
BOOL Get_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT()				{ return CLUSTER_RECIPE_PARALLEL_SUPPORT; }
//
//
void Set_Prm_PROCESS_RECIPE( int i , char *data ) { strncpy( PROCESS_RECIPE[i] , data , MAX_STRING_SIZE ); }
char *Get_Prm_PROCESS_RECIPE( int i )			{ return PROCESS_RECIPE[i]; }
//
//
//
//
void Set_CLUSTER_FILE_CHANGE( BOOL data ) { CLUSTER_FILE_CHANGE = data; }
BOOL Get_CLUSTER_FILE_CHANGE() { return CLUSTER_FILE_CHANGE; }

void Set_Prm_PROCESS_NAME( int i , char *data ){ strncpy( PROCESS_NAME[i] , data , MAX_STRING_SIZE ); }
char *Get_Prm_PROCESS_NAME( int i )			{ return PROCESS_NAME[i]; }
//
//
//
void Set_GROUP_RUN_CHECK( BOOL data ) { GROUP_RUN_CHECK = data; }
BOOL Get_GROUP_RUN_CHECK() { return GROUP_RUN_CHECK; }

int  Get_CLUSTER_USE_TYPE_ITEM() { return CLUSTER_USE_TYPE_ITEM; }
void Set_CLUSTER_USE_TYPE_ITEM( int data ) { CLUSTER_USE_TYPE_ITEM = data; }

void Set_HANDLING_SIDE_CHECK( BOOL data ) { HANDLING_SIDE_CHECK = data; }
BOOL Get_HANDLING_SIDE_CHECK() { return HANDLING_SIDE_CHECK; }

void Set_Prm_NAME_STYLE( int data ) { NAME_STYLE = data; } // 2004.06.15
int  Get_Prm_NAME_STYLE() { return NAME_STYLE; } // 2004.06.15


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



void Set_Prm_MONITOR_MODE( int data )		{ MONITOR_MODE = data; }
BOOL Get_Prm_MONITOR_MODE()				{ return MONITOR_MODE; }

void Set_Prm_MONITOR_FOLDER( char *data )		{ strncpy( MONITOR_FOLDER , data , 63 ); }
char *Get_Prm_MONITOR_FOLDER()				{ return MONITOR_FOLDER; }

//------------------------------------------------------------------------------------------
void RECIPE_LOG_HANDLING( char *data , LPSTR list , ... ) { // 2004.01.08
	va_list va;
	char Buffer[2048];
	int i;
	//
	if ( !Get_Prm_MONITOR_MODE() ) {
		sprintf( Buffer , "[%s] " , data );
	}
	else {
		sprintf( Buffer , "[%s_R] " , data );
	}
	i = strlen( Buffer );
	va_start( va , list );
	vsprintf( Buffer + i , list , (LPSTR) va );
    va_end( va );
	//
	if ( !Get_Prm_MONITOR_MODE() ) {
//		_IO_COMMON_PRINTF( 0 , "RECIPE" , "CLUSTER" , Buffer ); // 2006.08.23
		_IO_COMMON_PRINTF( 0 , "RECIPE" , "CLUSTER" , "%s" , Buffer ); // 2006.08.23
	}
	else {
//		_nINF_KNFS_COMMON_PRINTF( 0 , "RECIPE" , "CLUSTER" , Buffer ); // 2006.08.23
		_nINF_KNFS_COMMON_PRINTF( 0 , "RECIPE" , "CLUSTER" , "%s" , Buffer ); // 2006.08.23
	}
}
//------------------------------------------------------------------------------------------
