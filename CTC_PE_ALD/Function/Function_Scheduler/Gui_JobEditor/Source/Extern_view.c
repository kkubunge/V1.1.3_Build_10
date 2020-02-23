//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
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
#include <KutlFile.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "cimseqnc.h"
#include "iodefine.h"

void GET_SYSTEM_FILE( char *retfilename , char *name1 , char *name2 ); // 2017.03.12

//=========================================================================================
BOOL _Info_Display_Function_Read = FALSE;
char _Info_Display_Function_Name_PM[MAX_PM_CHAMBER_DEPTH][64];
int  _Info_Display_Function_Addr_PM[MAX_PM_CHAMBER_DEPTH];
char _Info_Display_Function_Name_BM[MAX_BM_CHAMBER_DEPTH][64];
int  _Info_Display_Function_Addr_BM[MAX_BM_CHAMBER_DEPTH];
//=========================================================================================
void _Info_Display_Function_Get( char *schpath ) {
	FILE *fpt;
	char Filename[ 256+1 ];
	char Buffer[ 1024+1 ];
	char chr_return[ 1024+1 ];
	int  i , ReadCnt , Line;
	BOOL FileEnd = TRUE;

	if ( _Info_Display_Function_Read ) return;
	_Info_Display_Function_Read = TRUE;

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		_Info_Display_Function_Addr_PM[i] = -2;
		strcpy( _Info_Display_Function_Name_PM[i] , "" );
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		_Info_Display_Function_Addr_BM[i] = -2;
		strcpy( _Info_Display_Function_Name_BM[i] , "" );
	}

	sprintf( Filename , "%s/InfoFunction.cfg" , schpath );
	//
	if ( ( fpt = fopen( Filename , "r" ) ) == NULL ) {
		//
		// 2017.02.22
		//
		GET_SYSTEM_FILE( Filename , "InfoFunction.cfg" , NULL ); // 2017.03.12
//		sprintf( Filename , "%s/InfoFunction.cfg" , "system" ); // 2017.03.12
		//
		if ( ( fpt = fopen( Filename , "r" ) ) == NULL ) {
			return;
		}
		//
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = Get_Line_String_From_File( fpt , Buffer , &ReadCnt );
		if ( ReadCnt > 1 ) {
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				fclose( fpt );
				return;
			}
			if ( tolower( chr_return[0] ) == 'p' ) {
				if ( tolower( chr_return[1] ) == 'm' ) {
					i = atoi( chr_return + 2 ) - 1;
					if ( ( i >= 0 ) && ( i < MAX_PM_CHAMBER_DEPTH ) ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
							fclose( fpt );
							return;
						}
						strncpy( _Info_Display_Function_Name_PM[ i ] , chr_return , 63 );
					}
				}
			}
			else if ( tolower( chr_return[0] ) == 'b' ) {
				if ( tolower( chr_return[1] ) == 'm' ) {
					i = atoi( chr_return + 2 ) - 1;
					if ( ( i >= 0 ) && ( i < MAX_BM_CHAMBER_DEPTH ) ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
							fclose( fpt );
							return;
						}
						strncpy( _Info_Display_Function_Name_BM[ i ] , chr_return , 63 );
					}
				}
			}
		}
	}
	fclose( fpt );
}

BOOL _Info_Display_Function_Run( HWND hWnd , char *schpath , char *rcppath , int ch , BOOL bmmode , char *filename ) {
	//
	if ( ch < 0 ) return FALSE;
	//
	_Info_Display_Function_Get( schpath );
	//
	if ( bmmode ) {
		//
		if ( ch >= MAX_BM_CHAMBER_DEPTH ) return FALSE;
		//
		if ( _Info_Display_Function_Addr_BM[ch] == -2 ) {
			if ( strlen( _Info_Display_Function_Name_BM[ch] ) <= 0 ) {
				_Info_Display_Function_Addr_BM[ch] = -1;
			}
			else {
				_Info_Display_Function_Addr_BM[ch] = _FIND_FROM_STRING( _K_F_IO , _Info_Display_Function_Name_BM[ch] );
			}
		}
		//
		if ( _Info_Display_Function_Addr_BM[ch] == -1 ) return FALSE;
		//
		_dWRITE_FUNCTIONF_EVENT( _Info_Display_Function_Addr_BM[ch] , "FILE_DISPLAY %d|%s|%s" , hWnd , filename , rcppath );
		//
		KGUI_STANDARD_Set_Enable_Monitoring_Function( hWnd , _Info_Display_Function_Addr_BM[ch] , 0 ); // 2018.01.10
		//
	}
	else {
		//
		if ( ch >= MAX_PM_CHAMBER_DEPTH ) return FALSE;
		//
		if ( _Info_Display_Function_Addr_PM[ch] == -2 ) {
			if ( strlen( _Info_Display_Function_Name_PM[ch] ) <= 0 ) {
				_Info_Display_Function_Addr_PM[ch] = -1;
			}
			else {
				_Info_Display_Function_Addr_PM[ch] = _FIND_FROM_STRING( _K_F_IO , _Info_Display_Function_Name_PM[ch] );
			}
		}
		//
		if ( _Info_Display_Function_Addr_PM[ch] == -1 ) return FALSE;
		//
		_dWRITE_FUNCTIONF_EVENT( _Info_Display_Function_Addr_PM[ch] , "FILE_DISPLAY %d|%s|%s" , hWnd , filename , rcppath );
		//
		KGUI_STANDARD_Set_Enable_Monitoring_Function( hWnd , _Info_Display_Function_Addr_PM[ch] , 0 ); // 2018.01.10
		//
	}
	return TRUE;
}
