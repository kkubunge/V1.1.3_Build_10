#include <stdio.h>

#include <windows.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutlgui.h>

#include "appinit.h"
#include "appprm.h"

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
/*char _ArgvParam[256];
char *PROGRAM_ARGUMENT_READ()  { return( _ArgvParam ); }
//
void KGUI_STANDARD_Screen_Nav_Info( int no ) {}
int _KGUI_WIN_SIZE_HEIGHT() { int i; i = GetSystemMetrics( SM_CYSCREEN ); if ( i >= 768 ) return 90; else return ( i / 9 ); }
int _KGUI_WIN_SIZE_WIDTH() { int i;	i = GetSystemMetrics( SM_CXSCREEN ); if ( i >= 1024 ) return 94; else return ( i / 11 ); }
BOOL KGUI_STANDARD_Window_Signal_Mode() { return 0; }
void KGUI_STANDARD_Window_Kill_Signal() {}
void KGUI_STANDARD_Window_Close_Signal() {}
void KGUI_STANDARD_Window_Open_Signal() {}
void KGUI_STANDARD_Set_Function_Size( HWND hWnd ) {
	RECT    rect; GetWindowRect( GetDesktopWindow() , &rect );
	MoveWindow( hWnd , 0 , _KGUI_WIN_SIZE_HEIGHT() + 1 , rect.right - _KGUI_WIN_SIZE_WIDTH() - 1 , rect.bottom - (_KGUI_WIN_SIZE_HEIGHT()+_KGUI_WIN_SIZE_HEIGHT()) - 2 , TRUE );
}
void SYSTEM_KILL_MYSELF() {	KGUI_STANDARD_Window_Kill_Signal();	KWIN_DIALOG_DRIVER_UNLOADING(); }
//
void main( int argc , char *argv[] ) {
	if ( argc >= 2 ) strncpy( _ArgvParam , argv[1] , 255 );
	else             strcpy( _ArgvParam , "" );
	Program_Enter_Code();
	Program_Main();
}*/
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
extern int	 MAX_VIEW_CHAMBER;

BOOL Get_Resource_LIST_PMFILE_Index_from_Resource_RELOAD_PM( int data , int *res , int *index ) {
	switch( data ) {
	case IDC_CTRL_RELOAD_PM1 :	*res = IDC_LIST_PM1FILE;	*index = 0;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM2 :	*res = IDC_LIST_PM2FILE;	*index = 1;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM3 :	*res = IDC_LIST_PM3FILE;	*index = 2;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM4 :	*res = IDC_LIST_PM4FILE;	*index = 3;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM5 :	*res = IDC_LIST_PM5FILE;	*index = 4;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM6 :	*res = IDC_LIST_PM6FILE;	*index = 5;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM7 :	*res = IDC_LIST_PM7FILE;	*index = 6;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM8 :	*res = IDC_LIST_PM8FILE;	*index = 7;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM9 :	*res = IDC_LIST_PM9FILE;	*index = 8;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM10 :	*res = IDC_LIST_PM10FILE;	*index = 9;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM11 :	*res = IDC_LIST_PM11FILE;	*index = 10;	return TRUE;	break;
	case IDC_CTRL_RELOAD_PM12 :	*res = IDC_LIST_PM12FILE;	*index = 11;	return TRUE;	break;
	}
	return FALSE;
}

BOOL Get_Index_from_Resource_LIST_PMFILE( int data , int *index ) {
	switch( data ) {
	case IDC_LIST_PMFILE :		*index = MAX_VIEW_CHAMBER;		return TRUE;	break;
	case IDC_LIST_PM1FILE :		*index = 0;		return TRUE;	break;
	case IDC_LIST_PM2FILE :		*index = 1;		return TRUE;	break;
	case IDC_LIST_PM3FILE :		*index = 2;		return TRUE;	break;
	case IDC_LIST_PM4FILE :		*index = 3;		return TRUE;	break;
	case IDC_LIST_PM5FILE :		*index = 4;		return TRUE;	break;
	case IDC_LIST_PM6FILE :		*index = 5;		return TRUE;	break;
	case IDC_LIST_PM7FILE :		*index = 6;		return TRUE;	break;
	case IDC_LIST_PM8FILE :		*index = 7;		return TRUE;	break;
	case IDC_LIST_PM9FILE :		*index = 8;		return TRUE;	break;
	case IDC_LIST_PM10FILE :	*index = 9;		return TRUE;	break;
	case IDC_LIST_PM11FILE :	*index = 10;	return TRUE;	break;
	case IDC_LIST_PM12FILE :	*index = 11;	return TRUE;	break;
	}
	return FALSE;
}
