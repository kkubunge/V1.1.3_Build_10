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
#include <Kutltime.h>
#include <Kutlgui.h>

#include "iodefine.h"
#include "appprm.h"
#include "appinit.h"
#include "cimseqnc.h"


BOOL CHECK_MODULE_STRING( char *data , int *index ) {
	int i;
	i = strlen( data );
	if ( data[i-1] == ')' ) data[i-1] = 0;
	if ( STRNCMP_L( data , "PM" , 2 ) ) {
		*index = atoi( data + 2 ) - 1 + PM1;
		if ( *index >= PM1 ) {
			if ( ( *index - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) return TRUE;
		}
	}
	if ( STRNCMP_L( data , "P" , 1 ) ) {
		*index = atoi( data + 1 ) - 1 + PM1;
		if ( *index >= PM1 ) {
			if ( ( *index - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) return TRUE;
		}
	}
	//========================================================================
	// 2006.10.26
	//========================================================================
	if ( STRNCMP_L( data , "CM" , 2 ) ) {
		*index = atoi( data + 2 ) - 1 + CM1;
		if ( ( *index >= CM1 ) && ( *index < PM1 ) ) return TRUE;
	}
	//========================================================================
	if ( STRNCMP_L( data , "C" , 1 ) ) {
		*index = atoi( data + 1 ) - 1 + CM1;
		if ( ( *index >= CM1 ) && ( *index < PM1 ) ) return TRUE;
	}
	//========================================================================
	// 2007.01.03
	//========================================================================
	if ( STRNCMP_L( data , "BM" , 2 ) ) {
		*index = atoi( data + 2 ) - 1 + BM1;
		if ( *index >= BM1 ) {
			if ( ( *index - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) return TRUE;
		}
	}
	if ( STRNCMP_L( data , "B" , 1 ) ) {
		*index = atoi( data + 1 ) - 1 + BM1;
		if ( *index >= BM1 ) {
			if ( ( *index - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) return TRUE;
		}
	}
	//========================================================================
	return FALSE;
}
//-------------------------------------------------------------------------
// Function = FILE_SYSTEM_SETUP
//-------------------------------------------------------------------------
BOOL FILE_SYSTEM_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[256];
	int group , index;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;

	// Default Session

	for ( Line = 0 ; Line < MAX_CHAMBER ; Line++ ) _SET_CHAMBER_STRING( Line , "" );

	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "File Open Error - [%s]\n" , Filename );
		return FALSE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt );
		if ( ReadCnt > 0 ) {
			if ( ReadCnt < 2 ) {
				_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			group = -1;
			index = -1;

			if      ( STRCMP_L( chr_return , "Module_Mode(C1)" ) ) { group = 1; index = CM1; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C2)" ) ) { group = 1; index = CM1+1; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C3)" ) ) { group = 1; index = CM1+2; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C4)" ) ) { group = 1; index = CM1+3; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C5)" ) ) { group = 1; index = CM1+4; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C6)" ) ) { group = 1; index = CM1+5; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C7)" ) ) { group = 1; index = CM1+6; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C8)" ) ) { group = 1; index = CM1+7; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C9)" ) ) { group = 1; index = CM1+8; }
			else if ( STRCMP_L( chr_return , "Module_Mode(C10)") ) { group = 1; index = CM1+9; }
			else if ( STRNCMP_L( chr_return , "Module_Mode(" , 12     ) ) {
				if ( CHECK_MODULE_STRING( chr_return + 12 , &index ) ) group = 2;
				else                                                   group = 0;
			}
			else if ( STRCMP_L( chr_return , "MAX_CASSETTE_SLOT"		  ) ) group = 3;
			else if ( STRCMP_L( chr_return , "CONTROL_RECIPE_TYPE"        ) ) group = 4;
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_CASSETTE_IN"  ) ) group = 5;
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_CASSETTE_OUT" ) ) group = 6;
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_SLOT_START"   ) ) group = 7;
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_SLOT_END"     ) ) group = 8;
			else if ( STRNCMP_L( chr_return , "Module_Log_Name(" , 16     ) ) {
				if ( CHECK_MODULE_STRING( chr_return + 16 , &index ) ) group = 11;
				else                                                   group = 0;
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			switch ( group ) {
			case 1 :
				if      ( STRCMP_L( chr_return , "On"  ) ) {
					Set_Prm_MODULE_MODE( index , TRUE );
					if ( Get_Prm_MAX_CASSETTE_COUNT() < index ) Set_Prm_MAX_CASSETTE_COUNT( index );
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					Set_Prm_MODULE_MODE( index , FALSE );
				}
				break;
			case 2 :
				if      ( STRCMP_L( chr_return , "On"  ) ) {
					Set_Prm_MODULE_MODE( index , TRUE );
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					Set_Prm_MODULE_MODE( index , FALSE );
				}
				break;
			case 3 :
				Set_Prm_MAX_CASSETTE_SLOT( atoi( chr_return ) );
				break;
			case 4 :
				if      ( STRCMP_L( chr_return , "ALL_RECIPE"         ) ) Set_Prm_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_ALL );
				else if ( STRCMP_L( chr_return , "LOT_PROCESS_RECIPE" ) ) Set_Prm_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_LOT_PROCESS );
				else if ( STRCMP_L( chr_return , "CLUSTER_RECIPE"     ) ) Set_Prm_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_CLUSTER );
				else if ( STRCMP_L( chr_return , "PROCESS_RECIPE"     ) ) Set_Prm_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_PROCESS );
				break;
			case 5 :
				if      ( STRCMP_L( chr_return , "CM1"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 );
				else if ( STRCMP_L( chr_return , "CM2"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 1 );
				else if ( STRCMP_L( chr_return , "CM3"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 2 );
				else if ( STRCMP_L( chr_return , "CM4"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 3 );
				else if ( STRCMP_L( chr_return , "CM5"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 4 );
				else if ( STRCMP_L( chr_return , "CM6"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 5 );
				else if ( STRCMP_L( chr_return , "CM7"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 6 );
				else if ( STRCMP_L( chr_return , "CM8"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 7 );
				else if ( STRCMP_L( chr_return , "CM9"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 8 );
				else if ( STRCMP_L( chr_return , "CM10"    ) ) Set_Prm_CONTROL_FIXED_CASSETTE_IN( CM1 + 9 );
				break;
			case 6 :
				if      ( STRCMP_L( chr_return , "CM1"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 );
				else if ( STRCMP_L( chr_return , "CM2"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 1 );
				else if ( STRCMP_L( chr_return , "CM3"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 2 );
				else if ( STRCMP_L( chr_return , "CM4"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 3 );
				else if ( STRCMP_L( chr_return , "CM5"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 4 );
				else if ( STRCMP_L( chr_return , "CM6"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 5 );
				else if ( STRCMP_L( chr_return , "CM7"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 6 );
				else if ( STRCMP_L( chr_return , "CM8"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 7 );
				else if ( STRCMP_L( chr_return , "CM9"     ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 8 );
				else if ( STRCMP_L( chr_return , "CM10"    ) ) Set_Prm_CONTROL_FIXED_CASSETTE_OUT( CM1 + 9 );
				break;
			case 7 :
				Set_Prm_CONTROL_FIXED_SLOT_START( atoi( chr_return ) );
				break;
			case 8 :
				Set_Prm_CONTROL_FIXED_SLOT_END( atoi( chr_return ) );
				break;
			case 11 :
				_SET_CHAMBER_STRING( index , chr_return );
				break;
			}
		}
	}
	_lclose( hFile );
	return TRUE;
}

//-------------------------------------------------------------------------
// Function = FILE_FILE_SETUP
//-------------------------------------------------------------------------
BOOL FILE_FILE_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char Buffer2[256];
	char chr_return[64];
	int group , index , i , j , l;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		Set_Prm_PROCESS_RECIPE_PATH( i + PM1 , "" );
		Set_Prm_PROCESS_RECIPE_HEAD_PATH( i + PM1 , "" );
	}
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "File Open Error - [%s]\n" , Filename );
		return FALSE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt );
		if ( ReadCnt > 0 ) {
			if ( ReadCnt < 2 ) {
				_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			group = -1;
			index = -1;

			if      ( STRCMP_L( chr_return , "FILE_ID"               ) ) group = 1;
			else if ( STRCMP_L( chr_return , "LOT_ID"				 ) ) group = 2;

			else if ( STRCMP_L( chr_return , "MAIN_RECIPE_PATH"       ) ) group = 4;
			else if ( STRCMP_L( chr_return , "LOT_RECIPE_PATH"        ) ) group = 5;
			else if ( STRCMP_L( chr_return , "CLUSTER_RECIPE_PATH"    ) ) group = 6;
			else if ( STRNCMP_L( chr_return , "PROCESS_RECIPE_PATH(" , 20     ) ) {
				if ( CHECK_MODULE_STRING( chr_return + 20 , &index ) ) group = 7;
				else                                                   group = 0;
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			switch ( group ) {
			case 1 :
				Set_Prm_FILE_ID( chr_return );
				break;
			case 2 :
				Set_Prm_LOT_RECIPE_ID( chr_return );
				break;
			case 4 :
				Set_Prm_MAIN_RECIPE_PATH( chr_return );
				break;
			case 5 :
				Set_Prm_LOT_RECIPE_PATH( chr_return );
				break;
			case 6 :
				Set_Prm_CLUSTER_RECIPE_PATH( chr_return );
				break;
			case 7 :
				Set_Prm_PROCESS_RECIPE_PATH( index , chr_return );
				break;
			}
		}
	}
	_lclose( hFile );
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		strncpy( Buffer2 , Get_Prm_PROCESS_RECIPE_PATH( i + PM1 ) , 255 );
		l = strlen( Buffer2 );
		for ( j = 0 ; j < l ; j++ ) {
			if ( ( Buffer2[j] == '\\' ) || ( Buffer2[j] == '/' ) ) {
				Buffer2[j] = 0;
				break;
			}
		}
		Set_Prm_PROCESS_RECIPE_HEAD_PATH( i + PM1 , Buffer2 );
	}
	return TRUE;
}

//==============================================================================
//==============================================================================
//==============================================================================

BOOL FILE_ALL_SETUP( char *Filename ) {
	char Buffer[256];

	Set_Prm_GROUP_RECIPE_PATH( "." );

	sprintf( Buffer , "%s/System.cfg" , Filename );
	if ( !FILE_SYSTEM_SETUP( Buffer ) ) return FALSE;

	sprintf( Buffer , "%s/File.cfg" , Filename );
	if ( !FILE_FILE_SETUP( Buffer ) ) return FALSE;
	return TRUE;
}
