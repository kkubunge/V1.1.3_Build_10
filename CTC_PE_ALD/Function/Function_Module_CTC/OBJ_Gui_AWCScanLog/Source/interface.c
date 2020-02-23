#include <stdio.h>

#include <windows.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutlgui.h>

#include "cimseqnc.h"
#include "resource.h"
#include "interface.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Control-PRO Interface
//-------------------------------------------------------------------------
//char _ArgvParam[256];
//char *PROGRAM_ARGUMENT_READ()  { return( _ArgvParam ); }
//
//HWND MainHDL = NULL;
//void KGUI_STANDARD_Screen_Nav_Info( int no ) {}
//int _KGUI_WIN_SIZE_HEIGHT() { int i; i = GetSystemMetrics( SM_CYSCREEN ); if ( i >= 768 ) return 90; else return ( i / 9 ); }
//int _KGUI_WIN_SIZE_WIDTH() { int i;	i = GetSystemMetrics( SM_CXSCREEN ); if ( i >= 1024 ) return 94; else return ( i / 11 ); }
//BOOL KGUI_STANDARD_Window_Signal_Mode() { return 0; }
//void KGUI_STANDARD_Window_Kill_Signal() {}
//void KGUI_STANDARD_Window_Close_Signal() {}
//void KGUI_STANDARD_Window_Open_Signal() {}
//void KGUI_STANDARD_Set_Function_Size( HWND hWnd ) {
//	RECT    rect;
//	MainHDL = hWnd;
//	MoveWindow( hWnd , 0 , 0 , 1 , 1 , TRUE );
//	GetWindowRect( GetDesktopWindow() , &rect );
//	MoveWindow( hWnd , 0 , _KGUI_WIN_SIZE_HEIGHT() + 1 , rect.right - _KGUI_WIN_SIZE_WIDTH() - 1 , rect.bottom - (_KGUI_WIN_SIZE_HEIGHT()+_KGUI_WIN_SIZE_HEIGHT()) - 2 , TRUE );
//}
//HWND KGUI_STANDARD_Get_Screen_Handle() {
//	return MainHDL;
//}
//void SYSTEM_KILL_MYSELF() {	KGUI_STANDARD_Window_Kill_Signal();	KWIN_DIALOG_DRIVER_UNLOADING(); }
//
//void main( int argc , char *argv[] ) {
//	if ( argc >= 2 ) strncpy( _ArgvParam , argv[1] , 255 );
//	else             strcpy( _ArgvParam , "" );
//	Program_Enter_Code();
//	Program_Main();
//}
//-------------------------------------------------------------------------
