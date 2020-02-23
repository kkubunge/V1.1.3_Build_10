#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <KutlAnal.h>
#include <kutlgui.h>
#include <kutlStr.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "..\resource.h"
//------------------------------------------------------------------------------------------
#define	MAX_IO_TABLE	1000
//------------------------------------------------------------------------------------------
char *Error_Data[MAX_IO_TABLE+1];
BOOL BOX_RUN		= FALSE;
HWND hwnd			= NULL;
BOOL MESSAGE_THREAD = FALSE;
BOOL MESSAGE_POPUP  = FALSE;
char MESSAGE_DATA[256];
char ARGUMENT_DATA[256];
enum { OFF , ON };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
//	{	"e:GEM.CONTROL"			,	_K_S_IO	,	CONTROL		,	0		} , 
//	{	"GEM.Terminal_Msg"		,	_K_S_IO	,	TERMMSG		,	0		} ,
//	{	"GEM.Terminal_SendMsg"	,	_K_S_IO	,	SENDMSG		,	0		} ,
	""
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
BOOL APIENTRY Gui_For_MessageBox_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
//	int CommStatus;
//	SYSTEMTIME SysTime;
//	char Buffer[32];
	switch ( msg ) {
		case WM_INITDIALOG:
			hwnd = hdlg;
			MoveCenterWindow( hdlg );
			if ( strlen(ARGUMENT_DATA) > 0 ) SetWindowText( hdlg , ARGUMENT_DATA );
			else SetWindowText( hdlg , "GEM Terminal MessageBox" );
			SendDlgItemMessage( hdlg , IDC_EDIT_MESSAGE , WM_SETTEXT , 0 , (LPARAM) MESSAGE_DATA );
			WRITE_UPLOAD_MESSAGE( MESSAGE_DATA );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
				case IDOK :
					WRITE_UPLOAD_MESSAGE( "" );
//					GetLocalTime( &SysTime );
//					sprintf( Buffer , "%04d%02d%02d%02d%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
//						SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
//					WRITE_STRING( TERMMSG , Buffer , &CommStatus );
//					WRITE_STRING( SENDMSG , Buffer , &CommStatus );
//					WRITE_STRING( CONTROL , "MESSAGE" , &CommStatus );
					EndDialog( hdlg , 0 );
					return TRUE;
				case IDCANCEL :
					WRITE_UPLOAD_MESSAGE( "" );
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			return TRUE;
		
		case WM_DESTROY :
			return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void k_Message_Box() {
// In window2000(OS), dialog popup case GDI leak in thread, so used while loop
// Also GetTopWindow case Memory leak, so used GET_MAIN_WINDOW having system modal attribut
	MESSAGE_THREAD = TRUE;
	while ( TRUE ) {
		if ( !MESSAGE_POPUP ) {
			_sleep( 100 );
			continue;
		}
		BOX_RUN = TRUE;
		GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_MESSAGE_BOX ) , GET_MAIN_WINDOW()/*GetTopWindow(NULL)*/ , Gui_For_MessageBox_Proc , (LPARAM) NULL );
		hwnd = NULL; WRITE_UPLOAD_MESSAGE( "" );
		MESSAGE_POPUP = FALSE;
		BOX_RUN = FALSE;
	}
	MESSAGE_THREAD = FALSE;
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
// Original source is no while loop.
// This is modified because dialop popup in thread cause GDI leak
	while ( TRUE ) {
		_sleep( 1000 );
	}
	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	unsigned char *Ed;
	int  Dl;

	REGIST_FILE_INFO( "Gem Terminal Interface Program" );
	if ( !KWIN_DIALOG_DRIVER_LOADING() ) MessageBeep(100);

	sprintf( ARGUMENT_DATA , "" );
	sprintf( MESSAGE_DATA  , "" );

	Ed = PROGRAM_ARGUMENT_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( strlen( Ed ) > 0 ) strncpy( ARGUMENT_DATA , Ed , 255 );
}
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
	if ( strcmp( PROGRAM_EVENT_READ() , "KILL" ) == 0 || strlen( PROGRAM_EVENT_READ() ) <= 0 ) { 
		WRITE_UPLOAD_MESSAGE( "" );
		if ( hwnd != NULL && BOX_RUN ) {
			SendMessage( hwnd , WM_COMMAND , IDCANCEL , (LPARAM)NULL ); 
			_sleep( 100 );
		}
		return; 
	}
	if ( !BOX_RUN )	{
		strcpy( MESSAGE_DATA , PROGRAM_EVENT_READ() );
		MESSAGE_POPUP = TRUE;
		if ( !MESSAGE_THREAD ) _beginthread( (void*)k_Message_Box , 0 , (void*)NULL );
	}
	else {
		if ( strcmp( MESSAGE_DATA , PROGRAM_EVENT_READ() ) != 0 ) {
			strcpy( MESSAGE_DATA , PROGRAM_EVENT_READ() );
			WRITE_UPLOAD_MESSAGE( MESSAGE_DATA );
			SendDlgItemMessage( hwnd , IDC_EDIT_MESSAGE , WM_SETTEXT , 0 , (LPARAM) MESSAGE_DATA );
			InvalidateRect( hwnd , NULL , TRUE );
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
