//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#define _WIN32_WINNT 0x0400		// 2014.02.11

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
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
#include <Kutlfile.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "appCTC.h"

#include "iodefine.h"
#include "lottype.h"
#include "ledapi.h"
#include "appinit.h"
#include "appprm.h"
#include "logview.h"
#include "Jobdata.h"
#include "appPlotting.h"

#include "cimseqnc.h"
#include "resource.h"


#include <CimSeqncKNFS.h>


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
CRITICAL_SECTION _LOG_OPEN_i_CR_SC;

CALLBACK_LOG_OPEN_CONTROL	_LOG_OPEN_CALLBACK_API;

BOOL _LOG_OPEN_i_THREAD_TAG = FALSE;
int  _LOG_OPEN_i_THREAD_HANDLE = -1;
//
HWND _LOG_OPEN_i_HWND_Main;
HWND _LOG_OPEN_i_HWND_Sub;


BOOL _LOG_OPEN_i_Interrupt = FALSE;
int  _LOG_OPEN_i_OPTION1;
int  _LOG_OPEN_i_OPTION2;
int  _LOG_OPEN_i_OPTION3;
int  _LOG_OPEN_i_OPTION4;
int  _LOG_OPEN_i_OPTION5;
int  _LOG_OPEN_i_OPTION6;
char _LOG_OPEN_i_FILE[256];
//
//int  _LOG_OPEN_i_xs; // 2014.07.17
//int  _LOG_OPEN_i_ys; // 2014.07.17
//
BOOL _LOG_OPEN_i_Wait_INITDIALOG = FALSE; // 2015.11.11

BOOL _LOG_OPEN_i_Wait_THREAD_TAG = FALSE;
int  _LOG_OPEN_i_Wait_THREAD_HANDLE = -1;

HWND _LOG_OPEN_i_Wait_hWnd;
HWND _LOG_OPEN_i_Wait_hWnds;
BOOL _LOG_OPEN_i_Wait_sepload;
char _LOG_OPEN_i_Wait_FileName[256];
int _LOG_OPEN_i_Wait_option1;
int _LOG_OPEN_i_Wait_option2;
int _LOG_OPEN_i_Wait_option3;
int _LOG_OPEN_i_Wait_option4;
int _LOG_OPEN_i_Wait_option5;
int _LOG_OPEN_i_Wait_option6;
CALLBACK_LOG_OPEN_CONTROL _LOG_OPEN_i_Wait_call_api;

void LOG_OPEN_INIT() {
	InitializeCriticalSection( &_LOG_OPEN_i_CR_SC );
}


void LOG_OPEN_CALL_THREADING() { // 2014.02.05
	//
//	RECT rect; // 2014.07.17
	//
//	GetWindowRect( _LOG_OPEN_i_HWND_Sub , &rect ); // 2014.07.17
	//
//	_LOG_OPEN_i_xs = rect.right - rect.left; // 2014.07.17
//	_LOG_OPEN_i_ys = rect.bottom - rect.top; // 2014.07.17
	//
//	SetWindowPos( _LOG_OPEN_i_HWND_Sub , NULL , 0 , 0 , _LOG_OPEN_i_xs , _LOG_OPEN_i_ys - 22 , SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW ); // 2014.07.17
//	KWIN_Item_Show( _LOG_OPEN_i_HWND_Main , IDC_LIST_JOBDATA_SPS ); // 2014.07.17
	//
	if ( (* _LOG_OPEN_CALLBACK_API)( _LOG_OPEN_i_HWND_Main , _LOG_OPEN_i_HWND_Sub , 1 , _LOG_OPEN_i_FILE , _LOG_OPEN_i_OPTION1 , _LOG_OPEN_i_OPTION2 , _LOG_OPEN_i_OPTION3 , _LOG_OPEN_i_OPTION4 , _LOG_OPEN_i_OPTION5 , _LOG_OPEN_i_OPTION6 ) ) {
		//
//		SetWindowPos( _LOG_OPEN_i_HWND_Sub , NULL , 0 , 0 , _LOG_OPEN_i_xs , _LOG_OPEN_i_ys , SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW ); // 2014.07.17
//		KWIN_Item_Hide( _LOG_OPEN_i_HWND_Main , IDC_LIST_JOBDATA_SPS ); // 2014.07.17
		//
		LOG_OPEN_MESSAGE( "" ); // 2014.07.17
	}
	else {
	}
	//
	_LOG_OPEN_i_THREAD_TAG = FALSE;
	//
	_endthread();
}


BOOL LOG_OPEN_CALL_THREAD() {
	//
	_LOG_OPEN_i_THREAD_TAG = TRUE; // 2014.07.17
	//
	_LOG_OPEN_i_THREAD_HANDLE = _beginthread( (void *) LOG_OPEN_CALL_THREADING , 0 , 0 );
	//
	if ( _LOG_OPEN_i_THREAD_HANDLE == -1 ) {
		//
		_LOG_OPEN_i_THREAD_TAG = FALSE; // 2014.07.17
		//
		return FALSE;
	}
	//
//	_LOG_OPEN_i_THREAD_TAG = TRUE; // 2014.07.17
	//
	return TRUE;
}

BOOL LOG_OPEN_CANCELED() {
	return _LOG_OPEN_i_Interrupt;
}

BOOL LOG_OPEN_MESSAGE( char *msg ) {
	//
	if ( !Get_Prm_STATUS_VIEW() ) return TRUE; // 2014.07.17
	if ( Get_Prm_DISPLAY_MODE() < 2 ) return TRUE; // 2015.10.28
	//
	if ( _LOG_OPEN_i_Interrupt ) return FALSE;
	if ( !TryEnterCriticalSection( &_LOG_OPEN_i_CR_SC ) ) return FALSE;
	if ( _LOG_OPEN_i_Interrupt ) {
		LeaveCriticalSection( &_LOG_OPEN_i_CR_SC );
		return FALSE;
	}
	//
	KWIN_Item_String_Display( _LOG_OPEN_i_HWND_Main , IDC_LIST_JOBDATA_SPS , msg );
	//
	LeaveCriticalSection( &_LOG_OPEN_i_CR_SC );
	//
	return TRUE;
}

BOOL LOG_OPEN_TERMINATE() {
	//
	if ( _LOG_OPEN_i_Wait_THREAD_TAG ) return FALSE;
	//
	_LOG_OPEN_i_Interrupt = TRUE;
	//
	if ( !TryEnterCriticalSection( &_LOG_OPEN_i_CR_SC ) ) return FALSE;
	//
	while ( TRUE ) {
		//
		if ( !_LOG_OPEN_i_THREAD_TAG ) break;
		//
		Sleep(1);
		//
	}
	//
	LeaveCriticalSection( &_LOG_OPEN_i_CR_SC );
	//
	return TRUE;
}


BOOL LOG_OPEN_CONTROL_S( HWND hWnd , HWND hWnds , BOOL sepload , char *FileName , int option1 , int option2 , int option3 , int option4 , int option5 , int option6 , CALLBACK_LOG_OPEN_CONTROL call_api ) {
	BOOL Res;
	DWORD FileSize;
	//
	_LOG_OPEN_i_Interrupt = FALSE;
	//
	_LOG_OPEN_CALLBACK_API = call_api;
	_LOG_OPEN_i_HWND_Main = hWnd;
	_LOG_OPEN_i_HWND_Sub = hWnds;
	_LOG_OPEN_i_OPTION1 = option1;
	_LOG_OPEN_i_OPTION2 = option2;
	_LOG_OPEN_i_OPTION3 = option3;
	_LOG_OPEN_i_OPTION4 = option4;
	_LOG_OPEN_i_OPTION5 = option5;
	_LOG_OPEN_i_OPTION6 = option6;
	//
	//----------------------------------------------------------------------------------------------
	if ( FileName == NULL ) {
		//
		strcpy( _LOG_OPEN_i_FILE , "" );
		//
		FileSize = 0; // 2015.11.11
		//
	}
	else {
		strcpy( _LOG_OPEN_i_FILE , FileName );
		//
		FileSize = Process_Log_File_Get_Size( FileName ); // 2015.11.11
		//
	}
	//
	if ( sepload ) {
		//
		if ( FileSize >= 1000000 ) { // 2015.11.11
			//
			if ( LOG_OPEN_CALL_THREAD() ) {
				//
				Res = TRUE;
				//
			}
			else {
				//
				Res = (* call_api )( hWnd , hWnds , 2 , _LOG_OPEN_i_FILE , option1 , option2 , option3 , option4 , option5 , option6 );
				//
			}
			//
		}
		else { // 2015.11.11
			//
			Res = (* call_api )( hWnd , hWnds , 2 , _LOG_OPEN_i_FILE , option1 , option2 , option3 , option4 , option5 , option6 );
			//
		}
		//
	}
	else {
		//
		Res = (* call_api )( hWnd , hWnds , 0 , _LOG_OPEN_i_FILE , option1 , option2 , option3 , option4 , option5 , option6 );
		//
	}
	//
	return Res;
}


void LOG_OPEN_CALL_WAIT_THREADING() { // 2014.02.05
	//
	while ( TRUE ) {
		if ( TryEnterCriticalSection( &_LOG_OPEN_i_CR_SC ) ) break;
	}
	//
	while ( TRUE ) {
		//
		if ( !_LOG_OPEN_i_THREAD_TAG ) break;
		//
		Sleep(1);
		//
	}
	//
	LeaveCriticalSection( &_LOG_OPEN_i_CR_SC );
	//
	LOG_OPEN_CONTROL_S( _LOG_OPEN_i_Wait_hWnd , _LOG_OPEN_i_Wait_hWnds , _LOG_OPEN_i_Wait_sepload , _LOG_OPEN_i_Wait_FileName , _LOG_OPEN_i_Wait_option1 , _LOG_OPEN_i_Wait_option2 , _LOG_OPEN_i_Wait_option3 , _LOG_OPEN_i_Wait_option4 , _LOG_OPEN_i_Wait_option5 , _LOG_OPEN_i_Wait_option6 , _LOG_OPEN_i_Wait_call_api );
	//
	_LOG_OPEN_i_Wait_THREAD_TAG = FALSE;
	//
	_endthread();
}



BOOL LOG_OPEN_CONTROL_WAIT( HWND hWnd , HWND hWnds , BOOL sepload , char *FileName , int option1 , int option2 , int option3 , int option4 , int option5 , int option6 , CALLBACK_LOG_OPEN_CONTROL call_api ) {
	//
	_LOG_OPEN_i_Wait_hWnd = hWnd;
	_LOG_OPEN_i_Wait_hWnds = hWnds;
	_LOG_OPEN_i_Wait_sepload = sepload;

	if ( FileName == NULL ) {
		strcpy( _LOG_OPEN_i_Wait_FileName , "" );
	}
	else {
		strcpy( _LOG_OPEN_i_Wait_FileName , FileName );
	}
	_LOG_OPEN_i_Wait_option1 = option1;
	_LOG_OPEN_i_Wait_option2 = option2;
	_LOG_OPEN_i_Wait_option3 = option3;
	_LOG_OPEN_i_Wait_option4 = option4;
	_LOG_OPEN_i_Wait_option5 = option5;
	_LOG_OPEN_i_Wait_option6 = option6;
	_LOG_OPEN_i_Wait_call_api = call_api;
	//
	_LOG_OPEN_i_Wait_INITDIALOG = TRUE; // 2015.11.11
	//
	_LOG_OPEN_i_Wait_THREAD_HANDLE = _beginthread( (void *) LOG_OPEN_CALL_WAIT_THREADING , 0 , 0 );
	//
	if ( _LOG_OPEN_i_Wait_THREAD_HANDLE == -1 ) return FALSE;
	//
	_LOG_OPEN_i_Wait_THREAD_TAG = TRUE;
	//
	return TRUE;
}


BOOL LOG_OPEN_CONTROL( HWND hWnd , HWND hWnds , BOOL sepload , char *FileName , int option1 , int option2 , int option3 , int option4 , int option5 , int option6 , CALLBACK_LOG_OPEN_CONTROL call_api ) {
	//
	if ( !LOG_OPEN_TERMINATE() ) {
		//
		if ( _LOG_OPEN_i_Wait_THREAD_TAG ) return FALSE;
		//
		return LOG_OPEN_CONTROL_WAIT( hWnd , hWnds , sepload , FileName , option1 , option2 , option3 , option4 , option5 , option6 , call_api );
		//
	}
	//
	return LOG_OPEN_CONTROL_S( hWnd , hWnds , sepload , FileName , option1 , option2 , option3 , option4 , option5 , option6 , call_api );
	//
}



BOOL xListView_Enter() {
	//
	if ( _LOG_OPEN_i_Interrupt ) return FALSE;
	if ( !TryEnterCriticalSection( &_LOG_OPEN_i_CR_SC ) ) return FALSE;
	if ( _LOG_OPEN_i_Interrupt ) {
		LeaveCriticalSection( &_LOG_OPEN_i_CR_SC );
		return FALSE;
	}
	//
	return TRUE;
	//
}
//
BOOL xListView_Leave() {
	//
	LeaveCriticalSection( &_LOG_OPEN_i_CR_SC );
	return !_LOG_OPEN_i_Interrupt;
	//
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void xListView_Clear( HWND hWnd ) {
	//
	ListView_DeleteAllItems( hWnd );
	while ( TRUE ) if ( !ListView_DeleteColumn( hWnd , 0 ) ) break;
	//
	ListView_SetExtendedListViewStyleEx( hWnd , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	//
}


