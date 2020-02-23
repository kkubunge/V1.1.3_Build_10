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


//-------------------------------------------------------------------------

extern char PM_SUB_DIRECTORY[MAX_PM_CHAMBER_DEPTH][MAX_SUB_CHAMBER_DEPTH][256];
extern int  PM_SUB_CURRENT_POS[MAX_PM_CHAMBER_DEPTH];
extern int  PM_EX_STATUS[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
extern int  PM_EX_COUNT;
extern int  PM_PX_STATUS[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
extern int  PM_PX_COUNT;

extern BOOL PM_LOW_CONTROL[MAX_PM_CHAMBER_DEPTH]; // 2002.11.22
extern char PM_LOW_DIRECTORY[MAX_PM_CHAMBER_DEPTH][3][64]; // 2002.11.22
extern char PM_LOW_EXTENSION[MAX_PM_CHAMBER_DEPTH][3][32]; // 2004.03.22
//
BOOL CHECK_MODULE_STRING( char *data , int *index ) {
	int i;
	i = strlen( data );
	if ( data[i-1] == ')' ) data[i-1] = 0;
	if ( STRNCMP_L( data , "PM" , 2 ) ) {
		*index = atoi( data + 2 ) - 1;
		if ( *index >= 0 ) {
			if ( *index < MAX_PM_CHAMBER_DEPTH ) return TRUE;
		}
	}
	if ( STRNCMP_L( data , "P" , 1 ) ) {
		*index = atoi( data + 1 ) - 1;
		if ( *index >= 0 ) {
			if ( *index < MAX_PM_CHAMBER_DEPTH ) return TRUE;
		}
	}
	return FALSE;
}
//-------------------------------------------------------------------------
// Function = FILE_SYSTEM_SETUP
//-------------------------------------------------------------------------
BOOL FILE_SYSTEM_SETUP( char *Filename , char *Close ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[256];
	char chr_return2[256];
	int group , index , i , j , l;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;

	// Default Session
	Set_HANDLING_SIDE_CHECK( FALSE );
	Set_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT( FALSE );
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		sprintf( Buffer , "PM%d" , i + 1 );
		Set_Prm_PROCESS_NAME( i , Buffer );
		Set_Prm_MODULE_MODE( i , FALSE );
	}
	//
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

			if      ( STRCMP_L( chr_return , "Module_Mode(TM)" ) ) group = 1;
			else if ( STRNCMP_L( chr_return , "Module_Mode(" , 12     ) ) {
				if ( CHECK_MODULE_STRING( chr_return + 12 , &index ) ) group = 2;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Module_Name(" , 12   ) ) {
				if ( CHECK_MODULE_STRING( chr_return + 12 , &index ) ) group = 3;
				else                                                   group = 0;
			}
			else if ( STRCMP_L( chr_return , "CLUSTER_RECIPE_PARALLEL_SUPPORT"	) ) group = 4;

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			switch ( group ) {
			case 1 :
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					i = atoi(chr_return);
					if ( i <= 0 ) i = 1;
					if ( i > 1 ) Set_HANDLING_SIDE_CHECK( TRUE );
					else		 Set_HANDLING_SIDE_CHECK( FALSE );
				}
				else {
					 Set_HANDLING_SIDE_CHECK( FALSE );
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
				sprintf( chr_return2 , "P%d:%s" , index + 1 , chr_return ); 
				Set_Prm_PROCESS_NAME( index , chr_return2 );
				break;
			case 4 :
				if      ( STRCMP_L( chr_return , "On"  ) ) Set_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT( TRUE );
				else if ( STRCMP_L( chr_return , "Off" ) ) Set_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT( FALSE );
				break;
			}
		}
	}
	_lclose( hFile );
	l = strlen( Close );
	for ( i = 0 ; i < l ; i++ ) {
		j = -1;
		switch( Close[i] ) {
		case '1' : j = 0; break;
		case '2' : j = 1; break;
		case '3' : j = 2; break;
		case '4' : j = 3; break;
		case '5' : j = 4; break;
		case '6' : j = 5; break;
		case '7' : j = 6; break;
		case '8' : j = 7; break;
		case '9' : j = 8; break;
		case 'A' : j = 9; break;
		case 'B' : j = 10; break;
		case 'C' : j = 11; break;
		case 'D' : j = 12; break;
		case 'E' : j = 13; break;
		case 'F' : j = 14; break;
		case 'G' : j = 15; break;
		case 'H' : j = 16; break;
		case 'I' : j = 17; break;
		case 'J' : j = 18; break;
		case 'K' : j = 19; break;
		case 'L' : j = 20; break;
		case 'M' : j = 21; break;
		case 'N' : j = 22; break;
		case 'O' : j = 23; break;
		case 'P' : j = 24; break;
		case 'Q' : j = 25; break;
		case 'R' : j = 26; break;
		case 'S' : j = 27; break;
		case 'T' : j = 28; break;
		case 'U' : j = 29; break;
		case 'V' : j = 30; break;
		case 'W' : j = 31; break;
		case 'X' : j = 32; break;
		case 'Y' : j = 33; break;
		case 'Z' : j = 34; break;
		case 'a' : j = 35; break;
		case 'b' : j = 36; break;
		case 'c' : j = 37; break;
		case 'd' : j = 38; break;
		case 'e' : j = 39; break;
		case 'f' : j = 40; break;
		case 'g' : j = 41; break;
		case 'h' : j = 42; break;
		case 'i' : j = 43; break;
		case 'j' : j = 44; break;
		case 'k' : j = 45; break;
		case 'l' : j = 46; break;
		case 'm' : j = 47; break;
		case 'n' : j = 48; break;
		case 'o' : j = 49; break;
		case 'p' : j = 50; break;
		case 'q' : j = 51; break;
		case 'r' : j = 52; break;
		case 's' : j = 53; break;
		case 't' : j = 54; break;
		case 'u' : j = 55; break;
		case 'v' : j = 56; break;
		case 'w' : j = 57; break;
		case 'x' : j = 58; break;
		case 'y' : j = 59; break;
		case 'z' : j = 60; break;
		}
		if ( ( j >= 0 ) && ( j < MAX_PM_CHAMBER_DEPTH ) ) Set_Prm_MODULE_MODE( j , FALSE );
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ )
	{
		if ( i < 12 ) {
			if ( ( i >=  0 ) && ( i <=  3 ) && Get_Prm_MODULE_MODE( i ) ) Set_Prm_DRAW_SIZE( 4 );
			if ( ( i >=  4 ) && ( i <=  5 ) && Get_Prm_MODULE_MODE( i ) ) Set_Prm_DRAW_SIZE( 6 );
			if ( ( i >=  6 ) && ( i <=  7 ) && Get_Prm_MODULE_MODE( i ) ) Set_Prm_DRAW_SIZE( 8 );
			if ( ( i >=  8 ) && ( i <=  9 ) && Get_Prm_MODULE_MODE( i ) ) Set_Prm_DRAW_SIZE( 10 );
			if ( ( i >= 10 ) && ( i <= 11 ) && Get_Prm_MODULE_MODE( i ) ) Set_Prm_DRAW_SIZE( 12 );
		}
		else 
		{
			if ( Get_Prm_MODULE_MODE( i ) ) {
				Set_Prm_DRAW_SIZE( 4 );
				break;
			}
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( Get_Prm_MODULE_MODE( i ) ) {
			Set_Prm_CHAMBER_SIZE( i+1 );
		}
		else {
			Set_Prm_PROCESS_NAME( i , "" );
		}
	}
	return TRUE;
}

//-------------------------------------------------------------------------
// Function = FILE_SCHEDULE_SETUP
//-------------------------------------------------------------------------
BOOL FILE_SCHEDULE_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[256];
	char chr_return2[256];
	int group , index;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;

	// Default Session
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

			if ( STRNCMP_L( chr_return , "Module_Name(" , 12   ) ) {
				if ( CHECK_MODULE_STRING( chr_return + 12 , &index ) ) group = 1;
				else                                                   group = 0;
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			switch ( group ) {
			case 1 :
					if ( Get_Prm_MODULE_MODE( index ) ) 
					{
						sprintf( chr_return2 , "P%d:%s" , index + 1 , chr_return ); 
						Set_Prm_PROCESS_NAME( index , chr_return2 );
					}
					else {
						Set_Prm_PROCESS_NAME( index , "" );
					}
					break;
			}
		}
	}
	_lclose( hFile );
	return TRUE;
}

//-------------------------------------------------------------------------
// Function = FILE_MDLGRP_SETUP
//-------------------------------------------------------------------------
BOOL FILE_MDLGRP_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[256];
	int ch = -1;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;
	int i;

	// Default Session
	PM_EX_COUNT = 0;
	PM_PX_COUNT = 0;
	//
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) return TRUE;
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt );
		if ( ReadCnt > 0 ) {
			if ( ReadCnt > ( MAX_SUB_CHAMBER_DEPTH + 1 ) ) {
				_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if      ( STRCMP_L( chr_return , "SUB_CHAMBER"         ) ) {
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				ch = -1;
				if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
				if ( ch == -1 ) {
					_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
					//return FALSE;
				}
				else {
					for ( i = 2 ; i < ReadCnt ; i++ ) {
						if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
							_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						strcpy( PM_SUB_DIRECTORY[ch][i-2] , chr_return );
					}
				}
			}
			else if ( STRCMP_L( chr_return , "EX_CHAMBER"         ) ) {
				if ( ReadCnt < 2 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
//				if ( PM_EX_COUNT < USE_CLUSTERDATA_ITEM_COUNT ) { // 2005.09.12
				if ( PM_EX_COUNT < MAX_CLUSTER_DEPTH ) { // 2005.09.12
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ )
						PM_EX_STATUS[PM_EX_COUNT][i] = 0;
					for ( i = 1 ; i < ReadCnt ; i++ ) {
						if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
							_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						ch = -1;
						if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
						if ( ch == -1 ) {
							_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
							//return FALSE;
						}
						else {
							PM_EX_STATUS[PM_EX_COUNT][ch] = 1;
						}
					}
					PM_EX_COUNT++;
				}
				else { // 2005.09.12
					_IO_CIM_PRINTF( "Too Many EX_CHAMBER Parameter Define - [%s]-[%d]\n" , Filename , Line );
				}
			}
			else if ( STRCMP_L( chr_return , "EX_PARALLEL"         ) ) { // 2002.05.20
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
//				if ( PM_PX_COUNT < USE_CLUSTERDATA_ITEM_COUNT ) { // 2005.09.12
				if ( PM_PX_COUNT < MAX_CLUSTER_DEPTH ) { // 2005.09.12
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ )
						PM_PX_STATUS[PM_PX_COUNT][i] = 0;
					for ( i = 1 ; i < ReadCnt ; i++ ) {
						if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
							_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						ch = -1;
						if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
						if ( ch == -1 ) {
							_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
							//return FALSE;
						}
						else {
							PM_PX_STATUS[PM_PX_COUNT][ch] = 1;
						}
					}
					PM_PX_COUNT++;
				}
				else { // 2005.09.12
					_IO_CIM_PRINTF( "Too Many EX_PARALLEL Parameter Define - [%s]-[%d]\n" , Filename , Line );
				}
			}
			else if ( STRCMP_L( chr_return , "SUB_MAIN_PATH" ) ) { // 2002.11.21
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				ch = -1;
				if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
				if ( ch == -1 ) {
					_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
					//return FALSE;
				}
				else {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					PM_LOW_CONTROL[ch] = TRUE;
					strcpy( PM_LOW_DIRECTORY[ch][0] , chr_return );
				}
			}
			else if ( STRCMP_L( chr_return , "SUB_POST_PATH" ) ) { // 2002.11.21
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				ch = -1;
				if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
				if ( ch == -1 ) {
					_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
					//return FALSE;
				}
				else {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					PM_LOW_CONTROL[ch] = TRUE;
					strcpy( PM_LOW_DIRECTORY[ch][1] , chr_return );
				}
			}
			else if ( STRCMP_L( chr_return , "SUB_PRE_PATH" ) ) { // 2002.11.21
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				ch = -1;
				if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
				if ( ch == -1 ) {
					_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
					//return FALSE;
				}
				else {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					PM_LOW_CONTROL[ch] = TRUE;
					strcpy( PM_LOW_DIRECTORY[ch][2] , chr_return );
				}
			}
			else if ( STRCMP_L( chr_return , "SUB_MAIN_EXTENSION" ) ) { // 2004.03.22
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				ch = -1;
				if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
				if ( ch == -1 ) {
					_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
					//return FALSE;
				}
				else {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					PM_LOW_CONTROL[ch] = TRUE;
					strcpy( PM_LOW_EXTENSION[ch][0] , chr_return );
				}
			}
			else if ( STRCMP_L( chr_return , "SUB_POST_EXTENSION" ) ) { // 2004.03.22
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				ch = -1;
				if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
				if ( ch == -1 ) {
					_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
					//return FALSE;
				}
				else {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					PM_LOW_CONTROL[ch] = TRUE;
					strcpy( PM_LOW_EXTENSION[ch][1] , chr_return );
				}
			}
			else if ( STRCMP_L( chr_return , "SUB_PRE_EXTENSION" ) ) { // 2004.03.22
				if ( ReadCnt < 3 ) {
					_IO_CIM_PRINTF( "Parameter Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				ch = -1;
				if ( !CHECK_MODULE_STRING( chr_return , &ch ) ) ch = -1;
				if ( ch == -1 ) {
					_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , chr_return , Filename , Line );
					//return FALSE;
				}
				else {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					PM_LOW_CONTROL[ch] = TRUE;
					strcpy( PM_LOW_EXTENSION[ch][2] , chr_return );
				}
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

	Set_Prm_FILE_ID( "" );
	Set_Prm_CLUSTER_RECIPE_ID( "" );
	Set_Prm_MAIN_RECIPE_PATH( "" );
	Set_Prm_LOT_RECIPE_PATH( "" );
	Set_Prm_CLUSTER_RECIPE_PATH( "" );
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		Set_Prm_PROCESS_RECIPE_ID( i , "" );
		Set_Prm_PROCESS_RECIPE_PATH( i , "" );
		Set_Prm_PROCESS_RECIPE_HEAD_PATH( i , "" );
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

			if      ( STRCMP_L( chr_return , "FILE_ID"                ) ) group = 1;
			else if ( STRCMP_L( chr_return , "CLUSTER_ID"             ) ) group = 2;
			else if ( STRNCMP_L( chr_return , "PROCESS_ID(" , 11     ) ) {
				if ( CHECK_MODULE_STRING( chr_return + 11 , &index ) ) group = 3;
				else                                                   group = 0;
			}
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
				Set_Prm_CLUSTER_RECIPE_ID( chr_return );
				break;
			case 3 :
				Set_Prm_PROCESS_RECIPE_ID( index , chr_return );
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
		strncpy( Buffer2 , Get_Prm_PROCESS_RECIPE_PATH( i ) , 255 );
		l = strlen( Buffer2 );
		for ( j = 0 ; j < l ; j++ ) {
			if ( ( Buffer2[j] == '\\' ) || ( Buffer2[j] == '/' ) ) {
				Buffer2[j] = 0;
				break;
			}
		}
		Set_Prm_PROCESS_RECIPE_HEAD_PATH( i , Buffer2 );
	}
	return TRUE;
}


//==============================================================================
//==============================================================================
//==============================================================================

BOOL FILE_ALL_SETUP( char *Filename , char *Close ) {
	char Buffer[256];

	Set_Prm_GROUP_RECIPE_PATH( "." );

	sprintf( Buffer , "%s/System.cfg" , Filename );
	if ( !FILE_SYSTEM_SETUP( Buffer , Close ) ) return FALSE;

	sprintf( Buffer , "%s/Schedule.cfg" , Filename );
	if ( !FILE_SCHEDULE_SETUP( Buffer ) ) return FALSE;

	sprintf( Buffer , "%s/File.cfg" , Filename );
	if ( !FILE_FILE_SETUP( Buffer ) ) return FALSE;
	
	sprintf( Buffer , "%s/MdlGrp.cfg" , Filename );
	if ( !FILE_MDLGRP_SETUP( Buffer ) ) return FALSE;
	
	return TRUE;
}
