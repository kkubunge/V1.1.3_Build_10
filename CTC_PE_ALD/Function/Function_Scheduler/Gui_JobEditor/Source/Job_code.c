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
#include <Kutlfile.h>
#include <Kutlanal.h>
#include <Kutltime.h>
#include <Kutlgui.h>

#include "appctc.h"

#include "iodefine.h"
#include "lottype.h"
#include "ledapi.h"
#include "appinit.h"
#include "appprm.h"
#include "jobdata.h"

#include "cimseqnc.h"
#include "resource.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern InfoTemplate						INFO_DATA;
//------------------------------------------------------------------------------------------
extern JOBStepTemplate				JOB_DATA[MAX_JOB_COUNT];
extern int							JOB_RUN[MAX_JOB_COUNT];
extern int							JOB_SIDE[MAX_JOB_COUNT];
extern BOOL							JOB_HANDOFF[MAX_JOB_COUNT];
extern int							JOB_FIRSTIN[MAX_JOB_COUNT];
extern int							JOB_LASTIN[MAX_JOB_COUNT];
extern int							JOB_SEL_LOOPCOUNT[MAX_JOB_COUNT];
//
extern int	JOBCOUNT;
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern	int	 FIND_COUNT;
extern	int	 FIND_INDEX[MAX_JOB_COUNT];
extern	char FIND_JID[256];		extern	BOOL FIND_JID_RUN;	extern	int FIND_OPTION1;
extern	char FIND_LID[256];		extern	BOOL FIND_LID_RUN;	extern	int FIND_OPTION2;
extern	char FIND_MID[256];		extern	BOOL FIND_MID_RUN;	extern	int FIND_OPTION3;
extern	char FIND_WID[256];		extern	BOOL FIND_WID_RUN;	extern	int FIND_OPTION4;
extern	BOOL FIND_START_RUN;
extern	int FIND_START_DATE_Y;
extern	int FIND_START_DATE_M;
extern	int FIND_START_DATE_D;
extern	BOOL FIND_END_RUN;
extern	int FIND_END_DATE_Y;
extern	int FIND_END_DATE_M;
extern	int FIND_END_DATE_D;
extern	int FIND_OPTION5;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int  LOG_ENCRIPTION; // 2010.04.14
//------------------------------------------------------------------------------------------
void f_enc_printf( FILE *fpt , LPSTR list , ... ) {
	va_list va;
	char TextBuffer[1024+1];

	va_start( va , list );
	_vsnprintf( TextBuffer , 1024 , list , (LPSTR) va );
	va_end( va );

	Make_Simple_Encryption( LOG_ENCRIPTION , TextBuffer );
	//
	fprintf( fpt , "%s" , TextBuffer );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char JOB_S_DATA[256];
BOOL JOB_H_RESULT;
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY HandOff_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	static int ch;
	static UINT iTimer;
	static BOOL	First_Check;
	static BOOL	Unload_Check;
	static BOOL	Load_Check;
	static BOOL	Start_Check;

	switch ( msg ) {
		case WM_INITDIALOG:
			JOB_H_RESULT = FALSE;
			First_Check = TRUE;
			Unload_Check = FALSE;
			Load_Check = FALSE;
			Start_Check = FALSE;
			ch = (int) lParam;
			//>>-----------------------------------------------------------
			MoveCenterWindow( hdlg );
			//
			KWIN_Item_String_Display( hdlg , IDC_STATIC_JOB , JOB_S_DATA );
			if      ( ch == 0 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM1" );
			else if ( ch == 1 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM2" );
			else if ( ch == 2 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM3" );
			else if ( ch == 3 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM4" );
			else if ( ch == 4 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM5" ); // 2011.01.17
			else if ( ch == 5 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM6" );
			else if ( ch == 6 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM7" );
			else if ( ch == 7 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM8" );
			else if ( ch == 8 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM9" );
			else if ( ch == 9 ) KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM10" );
			else                KWIN_Item_String_Display( hdlg , IDC_STATIC_ID , "CM" );
			KWIN_Item_Enable( hdlg , IDC_UNLOAD );
			KWIN_Item_Disable( hdlg , IDC_LOAD );
			//
			KWIN_Item_String_Display( hdlg , IDC_SKIP , "Skip && Start" );
			KWIN_Item_Enable( hdlg , IDC_SKIP );
			//
			iTimer = SetTimer( hdlg , 1000 , 500 , NULL );
			//
			return TRUE;
			
		case WM_TIMER :
			if ( Unload_Check ) {
				if ( !HANDOFF_For_RUNNING_CHECK( ch ) ) {
					Unload_Check = FALSE;
					KWIN_Item_Enable( hdlg , IDC_LOAD );
				}
			}
			if ( Load_Check ) {
				if ( !HANDOFF_For_RUNNING_CHECK( ch ) ) {
					Load_Check = FALSE;
					Start_Check = TRUE;
					//
					KWIN_Item_String_Display( hdlg , IDC_SKIP , "Start" );
					KWIN_Item_Enable( hdlg , IDC_SKIP );
				}
			}
			return TRUE;

		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
				case IDC_UNLOAD :
					if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Unloading HandOff???" , "Unload Check" , MB_ICONQUESTION | MB_YESNO ) ) {
						First_Check = FALSE;
						if ( HANDOFF_Event_OUT( ch ) == Success ) {
							Unload_Check = TRUE;
							KWIN_Item_Disable( hdlg , IDC_UNLOAD );
							KWIN_Item_Disable( hdlg , IDC_SKIP );
						}
						else {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "HandOff Unloading Fail" , "ERROR" , MB_ICONHAND );
							JOB_H_RESULT = FALSE;
							EndDialog( hdlg , 0 );
							return TRUE;
						}
					}
					return TRUE;

				case IDC_LOAD :
					if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Loading HandOff???" , "Load Check" , MB_ICONQUESTION | MB_YESNO ) ) {
						First_Check = FALSE;
						if ( HANDOFF_Event_IN( ch ) == Success ) {
							Load_Check = TRUE;
							KWIN_Item_Disable( hdlg , IDC_LOAD );
							KWIN_Item_Disable( hdlg , IDC_SKIP );
						}
						else {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "HandOff Loading Fail" , "ERROR" , MB_ICONHAND );
							JOB_H_RESULT = FALSE;
							EndDialog( hdlg , 0 );
							return TRUE;
						}
					}
					return TRUE;

				case IDC_SKIP :
					if ( Start_Check ) {
						//if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Start JOB???" , "Skip Check" , MB_ICONQUESTION | MB_YESNO ) ) {
							JOB_H_RESULT = TRUE;
							EndDialog( hdlg , 0 );
						//}
					}
					else {
						if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Skip HandOff and Start JOB???" , "Skip Check" , MB_ICONQUESTION | MB_YESNO ) ) {
							JOB_H_RESULT = TRUE;
							EndDialog( hdlg , 0 );
						}
					}
					return TRUE;

				case IDCANCEL :
					if ( First_Check ) {
						if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Cancel HandOff & Run???" , "Abort Check" , MB_ICONQUESTION | MB_YESNO ) ) {
							JOB_H_RESULT = FALSE;
							EndDialog( hdlg , 0 );
						}
					}
					else {
						if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Abort HandOff & Run???" , "Abort Check" , MB_ICONQUESTION | MB_YESNO ) ) {
							HANDOFF_Event_ABORT( ch );
							JOB_H_RESULT = FALSE;
							EndDialog( hdlg , 0 );
						}
					}
					return TRUE;
			}
			return TRUE;

		case WM_DESTROY:
			if ( iTimer ) KillTimer( hdlg , iTimer );
			return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
BOOL JOB_Carrier_HandOff( HWND hWnd , int InCas , int OutCas , char *job ) {
	strncpy( JOB_S_DATA , job , 255 );
	if ( InCas == 0 || OutCas == 0 ) {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 0 ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 0 ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 0 ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 0 ); // 2004.01.19 // 2004.08.10
		if ( !JOB_H_RESULT ) return FALSE;
	}
	if ( InCas == 1 || OutCas == 1 ) {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 1 ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 1 ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 1 ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 1 ); // 2004.01.19 // 2004.08.10
		if ( !JOB_H_RESULT ) return FALSE;
	}
	if ( InCas == 2 || OutCas == 2 ) {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 2 ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 2 ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 2 ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 2 ); // 2004.01.19 // 2004.08.10
		if ( !JOB_H_RESULT ) return FALSE;
	}
	if ( InCas == 3 || OutCas == 3 ) {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 3 ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 3 ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 3 ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_CARR_HANDOFF , IDD_E_CARR_HANDOFF ) , hWnd , HandOff_Proc , (LPARAM) 3 ); // 2004.01.19 // 2004.08.10
		if ( !JOB_H_RESULT ) return FALSE;
	}
	return JOB_H_RESULT;
}
//------------------------------------------------------------------------------------------
BOOL Dir_Has_Data( char *folder ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	char Buffer[256];
	//
	sprintf( Buffer , "%s/*.*" , folder );
	hFind = FindFirstFile( Buffer , &fd );
	if ( hFind != INVALID_HANDLE_VALUE ) {
		FindClose( hFind );
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL JOB_Delete_Folder_Item( int index , char *optiondata ) { // 2003.09.17
	char FileName[512];
	char Folderstr[256]; // 2017.03.13
	//
	if ( ( JOB_DATA[ index ].DIRECTORY_TYPE == 1 ) || ( JOB_DATA[ index ].DIRECTORY_TYPE == 3 ) ) {
		//
		JOB_Get_Directory_0( index , Folderstr ); // 2017.03.13
//		if ( strlen( JOB_Get_Directory0( index ) ) > 0 ) { // 2017.03.13
		if ( strlen( Folderstr ) > 0 ) { // 2017.03.13
			//
//			sprintf( FileName , "%s/LotLog/%s" , Get_Prm_LOG_DIRECTORY() , JOB_Get_Directory0( index ) ); // 2017.03.13
			sprintf( FileName , "%s/LotLog/%s" , Get_Prm_LOG_DIRECTORY() , Folderstr ); // 2017.03.13
			//
//			_IO_CIM_PRINTF( "Delete %s %s\n" , optiondata , FileName ); // 2003.07.25 // 2007.05.15
//			_IO_CIM_PRINTF( "Delete %s %s [%d/%d]\n" , optiondata , FileName , index , JOB_Get_Current_Count() ); // 2003.07.25 // 2007.05.15
			//
			if ( !Dir_Has_Data( FileName ) ) return TRUE; // 2011.07.26
			//
			if ( Directory_Delete_at_Sub( FileName ) ) { // 2010.11.16
				_IO_CIM_PRINTF( "Delete %s %s [%d/%d]\n" , optiondata , FileName , index , JOB_Get_Current_Count() ); // 2003.07.25 // 2007.05.15
				return TRUE;
			}
			else {
				_IO_CIM_PRINTF( "Delete %s %s [%d/%d] Fail\n" , optiondata , FileName , index , JOB_Get_Current_Count() ); // 2003.07.25 // 2007.05.15
				return FALSE;
			}
//			Directory_Delete_at_Sub( FileName ); // 2010.11.16
//			return TRUE; // 2010.11.16
		}
	}
//	return FALSE; // 2010.11.16
	return TRUE; // 2010.11.16
}
//------------------------------------------------------------------------------------------
BOOL JOB_Make_Delete_Item( int index ) { // 2003.09.17
	if ( JOB_Delete_Folder_Item( index , "SelectLog" ) ) { // 2010.11.16
		JOB_DATA[index].LOT_RECIPE_size = 0;
		return TRUE;
	}
	else { // 2010.12.03
		//
		JOB_DATA[ index ].DIRECTORY_TYPE = 3;
		//
		return FALSE;
	}
//	JOB_Delete_Folder_Item( index , "SelectLog" ); // 2010.11.16
}
//------------------------------------------------------------------------------------------
BOOL JOB_GET_EDITTYPE( int index ) { // 2016.03.29
	//
	switch( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
	case ET_EDITOR_N :
	case ET_EDITOR_UABORT :
		return TRUE;
		break;
	}
	//
	return FALSE;
	//
}


/*
//
// 2016.03.29
//
BOOL JOB_User_Fail_Check( int index ) { // 2015.11.12
	char filename[256];
	//
	sprintf( filename , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , JOB_Get_Directory( index ) , METHOD_LOG_FAIL );
	//
	if ( !File_Check_For_Exist( TRUE , filename ) ) return FALSE;
	//
	switch( JOB_DATA[ index ].EDIT_TYPE ) {
	case ET_EDITOR_N :
		JOB_DATA[ index ].EDIT_TYPE = ET_EDITOR_UABORT;
		return TRUE;
		break;
	case ET_APPEND_N :
		JOB_DATA[ index ].EDIT_TYPE = ET_APPEND_UABORT;
		return TRUE;
		break;
	case ET_MAINT_N :
		JOB_DATA[ index ].EDIT_TYPE = ET_MAINT_UABORT;
		return TRUE;
		break;
		//
	}
	//
	return FALSE;
}

*/
//
// 2016.03.29
//

BOOL JOB_User_Fail_Check( int index ) {
	char filename[256];
	FILE *fpt;
	BOOL bres;
	char Buffer[256];
	//
	char Bufferx[64];
	//
	int encflag;
	int ft , stmain , stpost , stpre , newtype;
	//
	JOB_Get_Directory( index , Buffer ); // 2017.03.13
	//
	sprintf( filename , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , Buffer , METHOD_LOG_FAIL );
	//
	if ( ( fpt = fopen( filename , "r" ) ) == NULL ) return FALSE;
	//
	ft = FALSE;
	//
	stmain = 0;
	stpost = 0;
	stpre = 0;
	//
	encflag = 0;
	//
	do {
		//
		bres = Read_Line_From_File4( fpt , Buffer , 255 );
		//
		if ( *Buffer != 0 ) {
			//
			if ( Buffer[0] != '#' ) {
				//
				Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer );
				//
				STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , 0 , 63 );
				//
				if      ( STRCMP_L( Bufferx , "MAIN_FAIL" ) ) {
					//
					stmain = 1;
					//
					ft = TRUE;
					//
				}
				else if ( STRCMP_L( Bufferx , "POST_FAIL" ) ) {
					//
					stpost = 1;
					//
					ft = TRUE;
					//
				}
				else if ( STRCMP_L( Bufferx , "POST_SUCCESS" ) ) {
					//
					if ( stpost == 0 ) stpost = 2;
					//
					ft = TRUE;
					//
				}
				else if ( STRCMP_L( Bufferx , "PRE_FAIL" ) ) {
					//
					stpre = 1;
					//
					ft = TRUE;
					//
				}
				else if ( STRCMP_L( Bufferx , "PRE_SUCCESS" ) ) {
					//
					if ( stpre == 0 ) stpre = 2;
					//
					ft = TRUE;
					//
				}
				//
			}
		}
	}
	while ( bres );
	//
	fclose( fpt );
	//
	if ( !ft ) {
		//
		switch( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
		case ET_EDITOR_N :
			newtype = ET_EDITOR_UABORT;
			break;
		case ET_APPEND_N :
			newtype = ET_APPEND_UABORT;
			break;
		case ET_MAINT_N :
			newtype = ET_MAINT_UABORT;
			break;
			//
		case ET_EDITOR_UABORT :
		case ET_APPEND_UABORT :
		case ET_MAINT_UABORT :
			newtype = JOB_DATA[ index ].EDIT_TYPE % 10;
			break;
			//
		default :
			newtype = ET_EDITOR_UABORT;
			break;
		}
		//
		if ( newtype == JOB_DATA[ index ].EDIT_TYPE ) return FALSE;
		//
		JOB_DATA[ index ].EDIT_TYPE = newtype;
		//
		return TRUE;
		//
	}
	//
	if ( stmain == 1 ) {
		//
		switch( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
		case ET_EDITOR_N :
			newtype = ET_EDITOR_UABORT;
			break;
		case ET_APPEND_N :
			newtype = ET_APPEND_UABORT;
			break;
		case ET_MAINT_N :
			newtype = ET_MAINT_UABORT;
			break;
			//
		case ET_EDITOR_UABORT :
		case ET_APPEND_UABORT :
		case ET_MAINT_UABORT :
			newtype = JOB_DATA[ index ].EDIT_TYPE % 10;
			break;
			//
		default :
			newtype = ET_EDITOR_UABORT;
			break;
		}
		//
	}
	else {
		//
		switch( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
		case ET_EDITOR_UABORT :
			newtype = ET_EDITOR_N;
			break;
		case ET_APPEND_UABORT :
			newtype = ET_APPEND_N;
			break;
		case ET_MAINT_UABORT :
			newtype = ET_MAINT_N;
			break;
			//
		case ET_EDITOR_N :
		case ET_APPEND_N :
		case ET_MAINT_N :
			newtype = JOB_DATA[ index ].EDIT_TYPE % 10;
			break;
			//
		default :
			newtype = ET_EDITOR_N;
			break;
		}
		//
	}
	//
	switch( stpre ) {
	case 0 :
		//
		switch( stpost ) {
		case 0 :
			break;
		case 1 :
			newtype = newtype + 10;
			break;
		case 2 :
			newtype = newtype + 20;
			break;
		}
		//
		break;
	case 1 :
		//
		switch( stpost ) {
		case 0 :
			newtype = newtype + 30;
			break;
		case 1 :
			newtype = newtype + 40;
			break;
		case 2 :
			newtype = newtype + 50;
			break;
		}
		//
		break;
	case 2 :
		//
		switch( stpost ) {
		case 0 :
			newtype = newtype + 60;
			break;
		case 1 :
			newtype = newtype + 70;
			break;
		case 2 :
			newtype = newtype + 80;
			break;
		}
		//
		break;
	}
	//
	if ( newtype == JOB_DATA[ index ].EDIT_TYPE ) return FALSE;
	//
	JOB_DATA[ index ].EDIT_TYPE = newtype;
	//
	return TRUE;
	//
}


//enum { ET_LOT_ICON_NORMAL , ET_LOT_ICON_PRE_NORMAL , ET_LOT_ICON_POST_NORMAL , ET_LOT_ICON_PREPOST_NORMAL , ET_LOT_ICON_FAIL , ET_LOT_ICON_PRE_FAIL , ET_LOT_ICON_POST_FAIL , ET_LOT_ICON_PREPOST_FAIL };

int JOB_User_Get_Icon( int index ) { // 2016.03.29
	int mode;
	//
	//============================================================================
	//
	switch( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
	case ET_EDITOR_UABORT :
	case ET_APPEND_UABORT :
	case ET_MAINT_UABORT :
		mode = ET_LOT_ICON_FAIL;
		break;
	default :
		mode = ET_LOT_ICON_NORMAL;
		break;
	}
	//
	//============================================================================
	//
	switch ( JOB_DATA[ index ].EDIT_TYPE / 10 ) { // Post

	case 1	: // Fail
	case 4	: // Fail
	case 7	: // Fail
		//
		mode = ET_LOT_ICON_POST_FAIL;
		//
		break;

	case 2	: // Success
	case 5	: // Success
	case 8	: // Success
		//
		switch ( mode ) {
		case ET_LOT_ICON_NORMAL	:	mode = ET_LOT_ICON_POST_NORMAL;	break;
		case ET_LOT_ICON_FAIL	:	mode = ET_LOT_ICON_POST_FAIL;	break;
		}
		//
		break;
	}
	//
	//============================================================================
	//
	switch ( JOB_DATA[ index ].EDIT_TYPE / 10 ) { // Pre

	case 3	: // Fail
	case 4	: // Fail
	case 5	: // Fail
		//
		switch ( mode ) {
		case ET_LOT_ICON_NORMAL			:	mode = ET_LOT_ICON_PRE_FAIL;		break;
		case ET_LOT_ICON_POST_NORMAL	:	mode = ET_LOT_ICON_PREPOST_FAIL;	break;
		case ET_LOT_ICON_FAIL			:	mode = ET_LOT_ICON_PRE_FAIL;		break;
		case ET_LOT_ICON_POST_FAIL		:	mode = ET_LOT_ICON_PREPOST_FAIL;	break;
		}
		//
		break;

	case 6	: // Success
	case 7	: // Success
	case 8	: // Success
		//
		switch ( mode ) {
		case ET_LOT_ICON_NORMAL			:	mode = ET_LOT_ICON_PRE_NORMAL;		break;
		case ET_LOT_ICON_POST_NORMAL	:	mode = ET_LOT_ICON_PREPOST_NORMAL;	break;
		case ET_LOT_ICON_FAIL			:	mode = ET_LOT_ICON_PRE_FAIL;		break;
		case ET_LOT_ICON_POST_FAIL		:	mode = ET_LOT_ICON_PREPOST_FAIL;	break;
		}
		//
		break;
	}
	//
	return mode;
	//
}


//enum { ET_EDITOR_UPOST_10_N , ET_EDITOR_UPOST_10_ABORT , ET_EDITOR_UPOST_10_SUCCESS }; // 2016.03.29
//enum { ET_EDITOR_UPRE_100_N , ET_EDITOR_UPRE_100_ABORT , ET_EDITOR_UPRE_100_SUCCESS }; // 2016.03.29

int JOB_User_Get_Fail_Data( int index , int mode ) { // 2016.03.29

	if ( mode == 0 ) { // main
		//
		switch( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
		case ET_EDITOR_UABORT :
		case ET_APPEND_UABORT :
		case ET_MAINT_UABORT :
			return 1;
			break;
		default :
			return 0;
			break;
		}
		//
	}
	else if ( mode == 1 ) { // post
		//
		switch( JOB_DATA[ index ].EDIT_TYPE / 10 ) {
		case 1 :
		case 4 :
		case 7 :
			return 1;
		case 2 :
		case 5 :
		case 8 :
			return 2;
		}
		//
	}
	else if ( mode == 2 ) { // pre
		//
		switch( JOB_DATA[ index ].EDIT_TYPE / 10 ) {
		case 3 :
		case 4 :
		case 5 :
			return 1;
		case 6 :
		case 7 :
		case 8 :
			return 2;
		}
		//
	}
	else { // All 0:Nomal 1:hasFail 2:hasPrePost
		//
		switch( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
		case ET_EDITOR_UABORT :
		case ET_APPEND_UABORT :
		case ET_MAINT_UABORT :
			return 1;
			break;
		}
		//
		switch( JOB_DATA[ index ].EDIT_TYPE / 10 ) {
		case 1 :
		case 3 :
		case 4 :
		case 5 :
		case 7 :
			return 1;
		case 2 :
		case 6 :
		case 8 :
			return 2;
		}
		//
	}
	//
	return 0;
	//
}

//------------------------------------------------------------------------------------------
int JOB_DATA_REMAPPING() { // 2004.01.14
	int i , k , soffset , eoffset;

	for ( i = 0 ; i < JOBCOUNT ; i++ ) {
		if ( JOB_DATA[i].LOT_RECIPE_size <= 0 ) {
			soffset = i;
			break;
		}
	}
	if ( i >= JOBCOUNT ) return 0;

	for ( i = soffset + 1 ; i < JOBCOUNT ; i++ ) {
		if ( JOB_DATA[i].LOT_RECIPE_size > 0 ) {
			eoffset = i;
			break;
		}
	}
	if ( i >= JOBCOUNT ) {
		k =	JOBCOUNT - soffset;
		JOBCOUNT = soffset;
		return k;
	}

	k = soffset;

	while ( TRUE ) {
		JOB_RUN[soffset] = JOB_RUN[eoffset];
		JOB_SIDE[soffset] = JOB_SIDE[eoffset];
		JOB_HANDOFF[soffset] = JOB_HANDOFF[eoffset];
		JOB_FIRSTIN[soffset] = JOB_FIRSTIN[eoffset];
		JOB_LASTIN[soffset] = JOB_LASTIN[eoffset];
		JOB_SEL_LOOPCOUNT[soffset] = JOB_SEL_LOOPCOUNT[eoffset];

		memcpy( &JOB_DATA[soffset] , &JOB_DATA[eoffset] , sizeof( JOBStepTemplate ) );

		k++;
		JOB_DATA[eoffset].LOT_RECIPE_size = 0;

		for ( i = soffset + 1 ; i < JOBCOUNT ; i++ ) {
			if ( JOB_DATA[i].LOT_RECIPE_size <= 0 ) {
				soffset = i;
				break;
			}
		}
		if ( i >= JOBCOUNT ) {
			eoffset = JOBCOUNT - k;
			JOBCOUNT = k;
			return eoffset;
		}

		for ( i = soffset + 1 ; i < JOBCOUNT ; i++ ) {
			if ( JOB_DATA[i].LOT_RECIPE_size > 0 ) {
				eoffset = i;
				break;
			}
		}
		if ( i >= JOBCOUNT ) {
			eoffset = JOBCOUNT - k;
			JOBCOUNT = k;
			return eoffset;
		}
	}
	return k;
}
//int JOB_DATA_REMAPPING() {
//	int i , k = 0 , x;
//	for ( i = 0 ; i < JOBCOUNT ; i++ ) {
//		if ( JOB_DATA[i].LOT_RECIPE_size <= 0 ) {
//			for ( x = i ; x < (JOBCOUNT-1) ; x++ ) {
//				JOB_RUN[x] = JOB_RUN[x+1];
//				JOB_SIDE[x] = JOB_SIDE[x+1];
//				JOB_HANDOFF[x] = JOB_HANDOFF[x+1];
//				JOB_FIRSTIN[x] = JOB_FIRSTIN[x+1];
//				JOB_LASTIN[x] = JOB_LASTIN[x+1];
//				memcpy( &JOB_DATA[x] , &JOB_DATA[x+1] , sizeof( JOBStepTemplate ) );
//			}
//			JOBCOUNT--; if ( JOBCOUNT < 0 ) JOBCOUNT = 0;
//			k++;
//			i--;
//		}
//	}
//	return k;
//}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int JOB_TIMEOVER_DELETE_CHECK( int maxdeletecount ) {
	int i , j , tt , ft , d1 , d2 , dc;
	//
//	long td; // 2011.05.12
	time_t td; // 2011.05.12
	//
	struct tm *time_data;
	BOOL RunFind = FALSE;
	BOOL DelFind = FALSE;
	//
	time( &td );
	//
	time_data = localtime( &td );
	tt = ( time_data->tm_year * 12 ) + time_data->tm_mon;
	d1 = time_data->tm_mday;
	//
	dc = 0;
	//
	j = JOBCOUNT;
	for ( i = 0 ; i < j ; i++ ) {
		if ( JOB_RUN[i] == JOB_READY ) {
			DelFind = FALSE;
			if ( ( ( (int) JOB_DATA[ i ].DIRECTORY_TYPE ) == 1 ) || ( ( (int) JOB_DATA[ i ].DIRECTORY_TYPE ) == 3 ) ) {
				if ( Get_Info_LOG_SAVE_MONTH() > 0 ) { // 2003.09.17
					//
//					time_data = localtime( &(JOB_DATA[i].DIRECTORY_DATE) ); // 2011.05.12
					td = (time_t) JOB_DATA[i].DIRECTORY_DATE; // 2011.05.12
					time_data = localtime( &td ); // 2011.05.12
					//
					ft = tt - ( ( time_data->tm_year * 12 ) + time_data->tm_mon );
					d2 = time_data->tm_mday;
					//
					if ( d1 < d2 ) ft--;
					//
					if ( ft >= Get_Info_LOG_SAVE_MONTH() ) {
						//
						if ( JOB_Delete_Folder_Item( i , "TimeOverLog" ) ) { // 2003.09.17
							//
							if ( Get_Info_AUTO_DELETE_MODE() == 1 ) {
								//
								JOB_DATA[ i ].DIRECTORY_TYPE = 2;
								//
								JOB_Set_Directory( i , "" , 0 );
							}
							else {
								DelFind = TRUE;
								JOB_DATA[i].LOT_RECIPE_size = 0;
							}
							//
							RunFind = TRUE;
							dc++;
							if ( dc >= maxdeletecount ) { // 2004.01.14
								break;
							}
							//_sleep(1); // 2004.01.14
						}
						else { // 2010.12.03
							JOB_DATA[ i ].DIRECTORY_TYPE = 3;
						}
						//
					}
				}
			}
			else if ( ( (int) JOB_DATA[ i ].DIRECTORY_TYPE ) == 2 ) {
				if ( Get_Info_AUTO_DELETE_MODE() == 0 ) {
					JOB_DATA[i].LOT_RECIPE_size = 0;
					DelFind = TRUE;
					RunFind = TRUE;
				}
			}
		}
	}
	if ( DelFind ) {
		return JOB_DATA_REMAPPING();
	}
	else {
		if ( RunFind ) return 0;
	}
	return -1;
}
//------------------------------------------------------------------------------------------
int JOB_RUNNING_CHECK() {
	int i , j , Side , WaitFind , Res , osmode , osm , osdata;
	char St_Buffer[256];
//	BOOL Chg = FALSE; // 2010.12.02
	int Chg = 0; // 2010.12.02
	BOOL sf = FALSE;
	BOOL Check[MAX_SIDE];
	time_t tempt; // 2011.05.12
	//
	char folderstr[256]; // 2017.03.13

	WaitFind = -1;
	for ( i = 0 ; i < MAX_SIDE ; i++ ) Check[i] = FALSE;
	for ( i = 0 ; i < JOBCOUNT ; i++ ) {
		if ( ( JOB_RUN[i] != JOB_READY ) && ( JOB_RUN[i] != JOB_WAITING ) ) {
			sf = TRUE;
			Side = JOB_SIDE[i];
			if ( READ_FUNCTION_For_STOP_CHECK( Side , &Res ) ) {
//				READ_FUNCTION_For_STOP_CHECK_DATA( Res , JOB_DATA[ i ].END_TIME , JOB_DATA[ i ].ELAPSED_TIME , &(JOB_DATA[ i ].METHOD_COUNT) , &(JOB_DATA[i].END_TYPE) ); // 2003.07.30
				READ_FUNCTION_For_STOP_CHECK_DATA( Res , JOB_DATA[ i ].END_TIME , JOB_DATA[ i ].ELAPSED_TIME , &(JOB_DATA[ i ].METHOD_COUNT) ); // 2003.07.30
				//
				JOB_DATA[ i ].END_TIME_size = strlen( JOB_DATA[ i ].END_TIME );	// 2003.07.30
				JOB_DATA[ i ].ELAPSED_TIME_size = strlen( JOB_DATA[ i ].ELAPSED_TIME );	// 2003.07.30
				//
//				JOB_DATA[ i ].COUNT++; // 2003.07.30
				//
				JOB_RUN[ i ] = JOB_READY;
				//
				JOB_User_Fail_Check( i ); // 2015.11.12
				//
				JOB_HANDOFF[ i ] = FALSE;
				JOB_FIRSTIN[ i ] = 0;
				JOB_LASTIN[ i ] = 0;
				JOB_SEL_LOOPCOUNT[i] = 0;
				//
				//--------------------------------------------------------------
				JOB_Display_Data_Save_per_Each( TRUE , i ); // 2003.06.09
				//--------------------------------------------------------------
				return -1;
			}
			else if ( RUN_FUNCTION_For_ENDING_CHECK( Side ) ) {
				for ( j = 0 ; j < 50 ; j++ ) {
					//
					Get_End_Time( Side , FALSE , JOB_DATA[ i ].END_TIME , &osm , &osdata );
					//
					if ( strlen( JOB_DATA[ i ].END_TIME ) > 0 ) break;
					//
//					_sleep(100); // 2015.04.10
					Sleep(100); // 2015.04.10
					//
				}
				//
				if ( osm == 2 ) { // 2012.03.20
					//
					JOB_SIDE[ i ] = osdata - 1;
					//
					JOB_DATA[ i ].METHOD_COUNT = Get_Wfr_Count( Side );
					//
					strcpy( JOB_DATA[ i ].END_TIME , "" );
					//
					JOB_DATA[ i ].END_TIME_size = strlen( JOB_DATA[ i ].END_TIME );
				}
				else {
					JOB_DATA[ i ].END_TIME_size = strlen( JOB_DATA[ i ].END_TIME );	// 2003.07.30
					//
					Get_Elapsed_Time( Side , JOB_DATA[ i ].ELAPSED_TIME );	JOB_DATA[ i ].ELAPSED_TIME_size = strlen( JOB_DATA[ i ].ELAPSED_TIME );	// 2003.07.30
					JOB_DATA[ i ].METHOD_COUNT = Get_Wfr_Count( Side );
					//
					JOB_RUN[ i ] = JOB_READY;
					//
					JOB_User_Fail_Check( i ); // 2015.11.12
					//
					JOB_HANDOFF[ i ] = FALSE;
					JOB_FIRSTIN[ i ] = 0;
					JOB_LASTIN[ i ] = 0;
					JOB_SEL_LOOPCOUNT[i] = 0;
					//
				}
				//
				Get_End_Time( Side , TRUE , St_Buffer , &osm , &osdata );
				//
				//--------------------------------------------------------------
				JOB_Display_Data_Save_per_Each( TRUE , i ); // 2003.06.09
				//--------------------------------------------------------------
				return -1;
			}
			else {
				//
				// 2012.03.20
				//
				Get_End_Time( Side , FALSE , St_Buffer , &osm , &osdata );
				//
				if ( osm == 1 ) {
					//
					for ( j = 0 ; j < i ; j++ ) {
						if ( ( JOB_RUN[j] != JOB_READY ) && ( JOB_RUN[j] != JOB_WAITING ) ) {
							return -99;
						}
					}
					for ( j = i + 1 ; j < JOBCOUNT ; j++ ) {
						if ( ( JOB_RUN[j] != JOB_READY ) && ( JOB_RUN[j] != JOB_WAITING ) ) {
							break;
						}
					}
					//
					Get_End_Time( Side , TRUE , St_Buffer , &osm , &osdata );
					//
					if ( j == JOBCOUNT ) return -99;
					//
					strcpy( JOB_DATA[ i ].END_TIME , St_Buffer );
					//
					JOB_DATA[ i ].END_TIME_size = strlen( JOB_DATA[ i ].END_TIME );
					//
					Get_Elapsed_Time( Side , JOB_DATA[ i ].ELAPSED_TIME );	JOB_DATA[ i ].ELAPSED_TIME_size = strlen( JOB_DATA[ i ].ELAPSED_TIME );
					JOB_DATA[ i ].METHOD_COUNT = osdata;
					//
					JOB_RUN[ i ] = JOB_READY;
					//
					JOB_User_Fail_Check( i ); // 2015.11.12
					//
					JOB_HANDOFF[ i ] = FALSE;
					JOB_FIRSTIN[ i ] = 0;
					JOB_LASTIN[ i ] = 0;
					JOB_SEL_LOOPCOUNT[i] = 0;
					//
					//--------------------------------------------------------------
					JOB_Display_Data_Save_per_Each( TRUE , i ); // 2003.06.09
					//--------------------------------------------------------------
					return -1;
				}
				//
				Check[ Side ] = TRUE;
				//
				if ( JOB_FIRSTIN[ i ] == 0 ) {
					if ( Get_FirstIn_Supply( Side , JOB_DATA[ i ].RUN_TIME ) ) {
						JOB_FIRSTIN[ i ] = 1;
//						Chg = TRUE; // 2010.12.02
						Chg = 2; // 2010.12.02
					}
					JOB_DATA[ i ].RUN_TIME_size = strlen( JOB_DATA[ i ].RUN_TIME );	// 2003.07.30
				}
				if ( JOB_LASTIN[ i ] == 0 ) {
					if ( Get_LastIn_Supply( Side ) ) {
						JOB_LASTIN[ i ] = 1;
//						Chg = TRUE; // 2010.12.02
						Chg = 1; // 2010.12.02
					}
				}
				//---------------------------------------------------------------------------
				// 2012.04.12
				//---------------------------------------------------------------------------
				Get_Start_Time( Side , FALSE , St_Buffer , &osmode , &osm );
				//
				if ( osmode == 1 ) {
					//
					JOB_Set_Directory( i , NULL , osm );
					//
					Get_Start_Time( Side , TRUE , St_Buffer , &osmode , &osm );
				}
				else {
					//
					if ( ( Get_Prm_DISPLAY_MODE() == 1 ) || ( Get_Prm_DISPLAY_MODE() == 3 ) ) { // 2010.11.30
						//
						if ( strlen( St_Buffer ) > 0 ) {
							//
							if ( !STRCMP_L( JOB_DATA[ i ].START_TIME , St_Buffer ) ) {
								//
								strcpy( JOB_DATA[ i ].START_TIME , St_Buffer );
								JOB_DATA[ i ].START_TIME_size = strlen( JOB_DATA[ i ].START_TIME );
								//
								Get_Job_Name( Side , JOB_DATA[ i ].JOB_NAME );
								if ( strlen( JOB_DATA[ i ].JOB_NAME ) <= 0 ) strcpy( JOB_DATA[ i ].JOB_NAME , "-MANUAL-" );
								JOB_DATA[ i ].JOB_NAME_size = strlen( JOB_DATA[ i ].JOB_NAME );
								//
								Get_MID_Name( Side , JOB_DATA[ i ].MID_NAME );	JOB_DATA[ i ].MID_NAME_size = strlen( JOB_DATA[ i ].MID_NAME );
								//
								GET_CURRENT_USERNAME( St_Buffer );
								strncpy( JOB_DATA[ i ].USER , St_Buffer , MAX_STRING_SIZE_22 );	JOB_DATA[ i ].USER_size = strlen( JOB_DATA[ i ].USER );
								//
								Get_Lot_Recipe( Side , JOB_DATA[ i ].IN_CASS + 1 , JOB_DATA[ i ].LOT_RECIPE );	JOB_DATA[ i ].LOT_RECIPE_size = strlen( JOB_DATA[ i ].LOT_RECIPE );
								//
								if ( JOB_DATA[ i ].LOT_RECIPE_size <= 0 ) {
									strcpy( JOB_DATA[ i ].LOT_RECIPE , " " );
									JOB_DATA[ i ].LOT_RECIPE_size = 1;
								}
								//
								Get_Directory( Side , St_Buffer );
								//
								if ( strlen( St_Buffer ) > 0 ) {
									//
									JOB_Get_Directory_0( i , folderstr ); // 2017.03.13
									//
//									if ( !STRCMP_L( JOB_Get_Directory0(i) , St_Buffer ) ) { // 2017.03.13
									//
									if ( !STRCMP_L( folderstr , St_Buffer ) ) { // 2017.03.13
										//
										JOB_DATA[ i ].DIRECTORY_TYPE = 1;
										//
										JOB_Set_Directory( i , St_Buffer , 0 );
										//
	//									time( &( JOB_DATA[ i ].DIRECTORY_DATE ) ); // 2011.05.12
										//
										time( &tempt ); // 2011.05.12
										JOB_DATA[ i ].DIRECTORY_DATE = (long) tempt; // 2011.05.12
										//
									}
									//
								}
								//
								JOB_DATA[ i ].STATUS_SAVE = 0; // 2010.12.02
								//
	//							Chg = TRUE; // 2010.12.02
								Chg = 3; // 2010.12.02
								//
							}
						}
					}
					//
				}
			}
		}
		else if ( ( WaitFind < 0 ) && ( JOB_RUN[i] == JOB_WAITING ) ) {
			WaitFind = i;
		}
	}
	if ( !sf ) READ_FUNCTION_For_RESET();
	if ( WaitFind >= 0 ) return WaitFind;
//	if ( Chg ) return -2; // 2010.12.02
	//
	if ( Chg == 1 ) return -2; // 2010.12.02
	if ( Chg == 2 ) return -3; // 2010.12.02
	if ( Chg == 3 ) return -4; // 2010.12.02
	//
	for ( i = 0 ; i < MAX_SIDE ; i++ ) {
		//-----------------------------------------------------------------
		// Append Run(2001.08.17)
		//-----------------------------------------------------------------
		if ( !Get_Prm_EXCLUSIVE_JOB_CHECK( i ) && !Check[i] ) {
			switch( RUN_FUNCTION_For_RUNWAIT_CHECK( i ) ) {
			case 1 : // Running
				//
				Get_End_Time( i , FALSE , St_Buffer , &osm , &osdata );
				//
				if ( osm <= 0 ) {
					//
					if ( strcmp( "" , St_Buffer ) == 0 ) {
						//
						Get_Start_Time( i , FALSE , St_Buffer , &osmode , &osdata );
						//
						if ( strcmp( "" , St_Buffer ) != 0 ) {
							//
							if ( osmode == 0 ) {
								//
								if ( ( osdata >= 0 ) && ( osdata < MAX_SIDE ) ) {
									//
									for ( j = 0 ; j < JOBCOUNT ; j++ ) {
										//
										if ( ( JOB_RUN[j] != JOB_READY ) && ( JOB_RUN[j] != JOB_WAITING ) ) {
											//
											if ( JOB_SIDE[j] == osdata ) {
												//
												JOB_SIDE[j] = i;
												//
												JOB_DATA[ j ].METHOD_COUNT = Get_Wfr_Count( i ); // 2012.03.20
												//
												Get_Start_Time( i , TRUE , St_Buffer , &osmode , &osm );
												//
												return -99;
											}
										}
									}
									//
								}
								//
							}
							//
							return ( -11 - i );
						}
					}
				}
				break;
			case 2 : // Waiting
				break;
			default : // Idle
				break;
			}
		}
	}
	return -99;
}
//
//
//
//
BOOL JOB_RunMode_Maint_Insert_Action( HWND hwnd , int side ) {
	return JOB_Insert_New_Job_Data_for_Maint( side );
}
//
//
BOOL JOB_RunMode_Start_Action( HWND hwnd , int index ) {
	int i , Side , Result , osmode , os;
	BOOL JobRun , JH , C1_Use , C2_Use , C3_Use , C4_Use;
	char St_Buffer[256] , St_Buffer2[256];
	time_t tempt;

	JobRun = FALSE;
	JH	   = FALSE;
	C1_Use = FALSE;
	C2_Use = FALSE;
	C3_Use = FALSE;
	C4_Use = FALSE;
	for ( i = 0 ; i < JOBCOUNT ; i++ ) {
		if ( ( JOB_RUN[i] != JOB_READY ) && ( JOB_RUN[i] != JOB_WAITING ) ) {
			JobRun = TRUE;
			if ( JOB_DATA[ i ].IN_CASS  == 0 ) C1_Use = TRUE;
			if ( JOB_DATA[ i ].IN_CASS  == 1 ) C2_Use = TRUE;
			if ( JOB_DATA[ i ].IN_CASS  == 2 ) C3_Use = TRUE;
			if ( JOB_DATA[ i ].IN_CASS  == 3 ) C4_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 0 ) C1_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 1 ) C2_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 2 ) C3_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 3 ) C4_Use = TRUE;
		}
		else if ( ( JOB_RUN[i] == JOB_WAITING ) && ( JOB_HANDOFF[i] ) ) {
			if ( JOB_DATA[ i ].IN_CASS  == 0 ) C1_Use = TRUE;
			if ( JOB_DATA[ i ].IN_CASS  == 1 ) C2_Use = TRUE;
			if ( JOB_DATA[ i ].IN_CASS  == 2 ) C3_Use = TRUE;
			if ( JOB_DATA[ i ].IN_CASS  == 3 ) C4_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 0 ) C1_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 1 ) C2_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 2 ) C3_Use = TRUE;
			if ( JOB_DATA[ i ].OUT_CASS == 3 ) C4_Use = TRUE;
		}
	}
	if ( JobRun ) {
		JH = TRUE;
		if ( Get_Info_DUAL_LOT_MODE() ) {
			if ( JOB_DATA[ index ].IN_CASS == 0 && JOB_DATA[ index ].OUT_CASS == 0 ) {
				if ( RUN_FUNCTION_For_READY_CHECK( 0 ) ) {
					if ( !C1_Use ) JobRun = FALSE;
				}
			}
			if ( JOB_DATA[ index ].IN_CASS == 1 && JOB_DATA[ index ].OUT_CASS == 1 ) {
				if ( RUN_FUNCTION_For_READY_CHECK( 1 ) ) {
					if ( !C2_Use ) JobRun = FALSE;
				}
			}
			if ( JOB_DATA[ index ].IN_CASS == 2 && JOB_DATA[ index ].OUT_CASS == 2 ) {
				if ( RUN_FUNCTION_For_READY_CHECK( 2 ) ) {
					if ( !C3_Use ) JobRun = FALSE;
				}
			}
			if ( JOB_DATA[ index ].IN_CASS == 3 && JOB_DATA[ index ].OUT_CASS == 3 ) {
				if ( RUN_FUNCTION_For_READY_CHECK( 3 ) ) {
					if ( !C4_Use ) JobRun = FALSE;
				}
			}
		}
		else {
			if ( JOB_DATA[ index ].IN_CASS == 0 && JOB_DATA[ index ].OUT_CASS == 0 ) {
				if ( !C1_Use ) JH = FALSE;
			}
			if ( JOB_DATA[ index ].IN_CASS == 1 && JOB_DATA[ index ].OUT_CASS == 1 ) {
				if ( !C2_Use ) JH = FALSE;
			}
			if ( JOB_DATA[ index ].IN_CASS == 2 && JOB_DATA[ index ].OUT_CASS == 2 ) {
				if ( !C3_Use ) JH = FALSE;
			}
			if ( JOB_DATA[ index ].IN_CASS == 3 && JOB_DATA[ index ].OUT_CASS == 3 ) {
				if ( !C4_Use ) JH = FALSE;
			}
		}
	}
	if ( JobRun ) {
//		if ( ( !JH ) && ( !JOB_HANDOFF[index] ) && ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR ) ) { // 2015.11.12
//		if ( ( !JH ) && ( !JOB_HANDOFF[index] ) && ( ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR_N ) || ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR_UABORT ) ) ) { // 2015.11.12 // 2016.03.29
		if ( ( !JH ) && ( !JOB_HANDOFF[index] ) && ( JOB_GET_EDITTYPE( index ) ) ) { // 2015.11.12 // 2016.03.29
			if ( JOB_Carrier_HandOff( hwnd , (int) JOB_DATA[ index ].IN_CASS , (int) JOB_DATA[ index ].OUT_CASS , JOB_DATA[ index ].JOB_NAME ) ) {
				JOB_HANDOFF[index] = TRUE;
				JOB_RUN[index] = JOB_WAITING;
				return TRUE;
			}
			else {
				JOB_RUN[index] = JOB_READY;
				//
				JOB_User_Fail_Check( index ); // 2015.11.12
				//
				return TRUE;
			}
		}
		else {
			if ( JOB_RUN[index] != JOB_WAITING ) {
				JOB_RUN[index] = JOB_WAITING;
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
	}
	else {
		Side   = Get_Enable_Side_Lot( (int) JOB_DATA[ index ].IN_CASS , (int) JOB_DATA[ index ].OUT_CASS );
		Result = RUN_FUNCTION_For_Recipe_Select( Side , JOB_DATA[ index ].LOT_RECIPE );
		if ( Result == Success ) {
			Result = RUN_FUNCTION_For_PATH_Select( Side , (int) JOB_DATA[ index ].IN_CASS , (int) JOB_DATA[ index ].OUT_CASS );
			if ( Result == Success ) {
				Result = RUN_FUNCTION_For_START_SLOT_Select( Side , (int) JOB_DATA[ index ].START_SLOT );
				if ( Result == Success ) {
					Result = RUN_FUNCTION_For_END_SLOT_Select( Side , (int) JOB_DATA[ index ].END_SLOT );
					if ( Result == Success ) {
						Result = RUN_FUNCTION_For_LOOP_COUNT_Select( Side , 1 );
						if ( Result == Success ) {
							//
//							if ( ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR ) && ( !JOB_HANDOFF[index] ) ) { // 2015.11.12
//							if ( ( ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR_N ) || ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR_UABORT ) ) && ( !JOB_HANDOFF[index] ) ) { // 2015.11.12
							if ( ( JOB_GET_EDITTYPE( index ) && ( !JOB_HANDOFF[index] ) ) ) { // 2015.11.12 // 2016.03.29
								if ( JOB_Carrier_HandOff( hwnd , (int) JOB_DATA[ index ].IN_CASS , (int) JOB_DATA[ index ].OUT_CASS , JOB_DATA[ index ].JOB_NAME ) ) {
									JOB_HANDOFF[index] = TRUE;
									Result = Success;
								}
								else {
									Result = !Success;
								}
							}
							if ( Result == Success ) {
								//
								Get_Start_Time( Side , FALSE , St_Buffer , &osmode , &os );
								//
								Result = RUN_FUNCTION_Event_START( Side , (int) JOB_DATA[index].PROGRESS , JOB_DATA[index].JOB_NAME );
								if ( Result == Success ) {
									JOB_RUN[index] = JOB_RUNNING;
									JOB_SIDE[index] = Side;
									JOB_HANDOFF[index] = FALSE;
									JOB_FIRSTIN[index] = 0;
									JOB_LASTIN[index] = 0;
									JOB_SEL_LOOPCOUNT[index] = 0;
								}
							}
							if ( Result != Success ) {
								JOB_HANDOFF[index] = FALSE;
								//
								JOB_RUN[index] = JOB_READY;
								//
								JOB_User_Fail_Check( index ); // 2015.11.12
								//
							}
						}
					}
				}
			}
		}
		if ( Result != Success ) {
			MODAL_WINDOW_MESSAGE_BOX( NULL , "Start Error" , "Error" , MB_ICONHAND );
			return FALSE;
		}
		else {
			for ( i = 0 ; i < 100 ; i++ ) {
//				_sleep( 50 ); // 2015.04.10
				Sleep( 50 ); // 2015.04.10
				if ( !RUN_FUNCTION_For_READY_CHECK( Side ) ) break;
			}
			//
			if ( i == 100 ) {
//				JOB_DATA[index].END_TYPE = JE_FAIL; // 2003.07.30
				JOB_RUN[index] = JOB_READY;
				//
				JOB_User_Fail_Check( index ); // 2015.11.12
				//
			}
			else {
				for ( i = 0 ; i < 30 ; i++ ) {
//					_sleep( 100 ); // 2015.04.10
					Sleep( 100 ); // 2015.04.10
					Get_Start_Time( Side , FALSE , St_Buffer2 , &osmode , &os );
					if ( strcmp( St_Buffer , St_Buffer2 ) != 0 ) break;
				}
				//
				JOB_Delete_Folder_Item( index , "OldRunLog" ); // 2003.09.17
				//
				Get_Start_Time( Side , FALSE , JOB_DATA[ index ].START_TIME , &osmode , &os );	JOB_DATA[ index ].START_TIME_size = strlen( JOB_DATA[ index ].START_TIME );	// 2003.07.30
				strcpy( JOB_DATA[ index ].RUN_TIME , "" );				JOB_DATA[ index ].RUN_TIME_size = 0;	// 2003.07.30
				strcpy( JOB_DATA[ index ].END_TIME , "" );				JOB_DATA[ index ].END_TIME_size = 0;	// 2003.07.30
				strcpy( JOB_DATA[ index ].ELAPSED_TIME , "" );			JOB_DATA[ index ].ELAPSED_TIME_size = 0;	// 2003.07.30
				Get_Directory( Side , JOB_DATA[ index ].DIRECTORY );	JOB_DATA[ index ].DIRECTORY_size = strlen( JOB_DATA[ index ].DIRECTORY );	// 2003.07.30
				JOB_DATA[ index ].DIRECTORY_TYPE = 1;
				//
//				time( &( JOB_DATA[ index ].DIRECTORY_DATE ) ); // 2011.05.12
				//
				time( &tempt );
				JOB_DATA[ index ].DIRECTORY_DATE = (long) tempt;
				//
				return TRUE;
			}
		}
	}
	return TRUE;
}
//
BOOL JOB_RunMode_Abort_Action( HWND hwnd , int index ) {
	if ( RUN_FUNCTION_Event_ABORT( JOB_SIDE[index] ) == Success ) {
		JOB_RUN[index] = JOB_READY;
		//
		JOB_User_Fail_Check( index ); // 2015.11.12
		//
		JOB_HANDOFF[index] = FALSE;
		return TRUE;
	}
	return FALSE;
}
//
BOOL JOB_RunMode_End_Action( HWND hwnd , int index ) {
	if ( RUN_FUNCTION_Event_END( JOB_SIDE[index] ) == Success ) {
		JOB_RUN[index] = JOB_ENDING;
		return TRUE;
	}
	return FALSE;
}
//
BOOL JOB_RunMode_Cancel_Action( HWND hwnd , int index ) {
	JOB_RUN[index] = JOB_READY;
	//
	JOB_User_Fail_Check( index ); // 2015.11.12
	//
	JOB_HANDOFF[index] = FALSE;
	return TRUE;
}
//
//-------------------------------------------------------------------------
int  JOB_Get_Current_Count() { return JOBCOUNT; }
//
BOOL JOB_Question_Count_Empty() {
	if ( JOBCOUNT <= 0 ) return TRUE;
	else                 return FALSE;
}
//
BOOL JOB_Question_Count_Full() {
	if ( JOBCOUNT >= MAX_JOB_COUNT ) return TRUE;
	else                             return FALSE;
}
//
BOOL JOB_Question_Count_Warning( int data ) {
//	if ( JOBCOUNT + 100  >= MAX_JOB_COUNT ) return TRUE; // 2003.09.16
	if ( JOBCOUNT + data >= MAX_JOB_COUNT ) return TRUE; // 2003.09.16
	else                                    return FALSE;
}
//
BOOL JOB_Question_Data_Delete_Enable( int index ) {
	if ( index >= 0 ) {
		if ( JOB_RUN[ index ] != JOB_READY ) return FALSE;
		else                                 return TRUE;
	}
	return FALSE;
}
//
//
//
int JOB_Delete_Old_Data( int count ) {
	int i , j , k;
	j = JOBCOUNT;
	for ( i = 0 , k = 0 ; ( i < count ) && ( i < j ) ; i++ , k++ ) {
		JOB_Delete_Folder_Item( i , "OverFlowLog" ); // 2003.09.17
	}
	for ( i = k ; i < j ; i++ ) {
		JOB_RUN[i-k] = JOB_RUN[i];
		JOB_SIDE[i-k] = JOB_SIDE[i];
		JOB_HANDOFF[i-k] = JOB_HANDOFF[i];
		JOB_FIRSTIN[i-k] = JOB_FIRSTIN[i];
		JOB_LASTIN[i-k] = JOB_LASTIN[i];
		JOB_SEL_LOOPCOUNT[i-k] = JOB_SEL_LOOPCOUNT[i];
		//
		memcpy( &JOB_DATA[i-k] , &JOB_DATA[i] , sizeof( JOBStepTemplate ) );
	}
	for ( i = 0 ; i < k ; i++ ) JOBCOUNT--;
	return k;
}
//

void Get_Elapsed_String( int index , char *szString , BOOL avr ) {
	int Res;
	int year , month , day , hour , min , sec , msec;
	double st , et;
	long elapsed_time;

	if ( JOB_DATA[ index ].ELAPSED_TIME[0] == 0 ) {
		strcpy( szString , "" );
		return;
	}
	//
	Res = _UTIL_TIME_GET_DATETIME_from_StringMS( JOB_DATA[ index ].END_TIME , &year , &month , &day , &hour , &min , &sec , &msec );
	//
	if ( Res != 0 ) {
		sprintf( szString , "%s ?" , JOB_DATA[ index ].ELAPSED_TIME );
		return;
	}
	//
	_UTIL_TIME_GET_SEC_with_0BasedTimeMS( year , month , day , hour , min , sec , msec , &et );
	//
	Res = _UTIL_TIME_GET_DATETIME_from_StringMS( JOB_DATA[ index ].RUN_TIME , &year , &month , &day , &hour , &min , &sec , &msec );
	//
	if ( Res != 0 ) {
		sprintf( szString , "%s ?" , JOB_DATA[ index ].ELAPSED_TIME );
		return;
	}
	//
	_UTIL_TIME_GET_SEC_with_0BasedTimeMS( year , month , day , hour , min , sec , msec , &st );
	//
	elapsed_time = (long) ( et - st );
	//
	hour = ( elapsed_time         ) / 3600;
	min  = ( elapsed_time % 3600  ) / 60;
	sec  = ( elapsed_time % 60    );
	//
	if ( avr ) {
		if ( JOB_DATA[index].METHOD_COUNT > 0 ) {
			sprintf( szString , "%d:%d:%d (%d)" , hour , min , sec , elapsed_time / JOB_DATA[index].METHOD_COUNT );
		}
		else {
			sprintf( szString , "%d:%d:%d" , hour , min , sec );
		}
	}
	else {
		sprintf( szString , "%d:%d:%d" , hour , min , sec );
	}
	//
}
void JOB_Display_Data_Area( int index , int realitem , char *szString , int param ) {
	int item;
//	TCHAR       *sz_C1_String[]    = { "ID" , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "START TIME" , "END TIME" , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };
//	TCHAR       *sz_C1_String_L[]  = { "ID" , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "START TIME" , "ELAPSE"   , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };
//	TCHAR       *sz_C2_String[]    = { "ID" , "START TIME" , "END TIME" , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };
//	TCHAR       *sz_C2_String_L[]  = { "ID" , "START TIME" , "ELAPSE"   , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };

//	TCHAR       *sz_E_C1_String[]    = { "ID" , "SLOT" , "MID NAME" , "JOB NAME" , "RECIPE" , "START TIME" , "END TIME" };
//	TCHAR       *sz_E_C1_String_L[]  = { "ID" , "SLOT" , "MID NAME" , "JOB NAME" , "RECIPE" , "START TIME" , "ELAPSE"   };
//	TCHAR       *sz_E_C2_String[]    = { "ID" , "SLOT" , "MID NAME" , "START TIME" , "END TIME" , "JOB NAME" , "RECIPE" };
//	TCHAR       *sz_E_C2_String_L[]  = { "ID" , "SLOT" , "MID NAME" , "START TIME" , "ELAPSE"   , "JOB NAME" , "RECIPE" };

	if ( index < 0 ) return;
	//====================================================================
	if ( ( Get_Prm_DISPLAY_MODE() == 1 ) || ( Get_Prm_DISPLAY_MODE() == 3 ) ) { // 2010.12.02
		if ( ( Get_Info_DISPLAY_MODE() / 2 ) == 1 ) {
			switch( realitem ) {
			case 0 :	item = 0; break;
			case 1 :	item = 4; break;
			case 2 :	item = 6; break;
			case 3 :	item = 7; break;
			case 4 :	item = 8; break;
			case 5 :	item = 1; break;
			case 6 :	item = 2; break;
			case 7 :	item = 13; break;
			default :	item = realitem + 2; break;
			}
		}
		else {
			switch( realitem ) {
			case 0 :	item = 0; break;
			case 1 :	item = 4; break;
			case 2 :	item = 6; break;
			case 3 :	item = 1; break;
			case 4 :	item = 2; break;
			case 5 :	item = 7; break;
			case 6 :	item = 8; break;
			case 7 :	item = 13; break;
			default :	item = realitem + 2; break;
			}
		}
	}
	else {
		if ( ( Get_Info_DISPLAY_MODE() / 2 ) == 1 ) {
			switch( realitem ) {
			case 1 :	item = 7; break;
			case 2 :	item = 8; break;
			case 3 :	item = 1; break;
			case 4 :	item = 2; break;
			case 5 :	item = 3; break;
			case 6 :	item = 4; break;
			case 7 :	item = 5; break;
			case 8 :	item = 6; break;
			default :	item = realitem; break;
			}
		}
		else {
			item = realitem;
		}
	}
	//====================================================================
	switch( item ) {
		case 1 : // JOB Name
				wsprintf( szString, "%s", JOB_DATA[ index ].JOB_NAME );
//				wsprintf( szString, "[%d] %s", param , JOB_DATA[ index ].JOB_NAME );
				break;
		case 2 : // LOT RECIPE
				wsprintf( szString, "%s" , JOB_DATA[ index ].LOT_RECIPE );
				break;
		case 3 : // Path
				if ( JOB_DATA[ index ].OUT_CASS < 0 ) { // 2002.10.02
					if      ( JOB_DATA[ index ].OUT_CASS == -1 ) { // 2002.10.02
						wsprintf( szString, "%d(extern)" , JOB_DATA[ index ].IN_CASS );
					}
					else if ( JOB_DATA[ index ].OUT_CASS == -2 ) { // 2011.06.15
						wsprintf( szString, "CM%d(E)" , JOB_DATA[ index ].IN_CASS ); // 2011.06.15
					}
					else if ( JOB_DATA[ index ].OUT_CASS == -3 ) { // 2011.06.15
						wsprintf( szString, "CM%d" , JOB_DATA[ index ].IN_CASS ); // 2011.06.15
					}
					else {
						wsprintf( szString, "%d" , JOB_DATA[ index ].IN_CASS ); // 2011.06.15
					}
				}
				else {
//						wsprintf( szString, "CM%d->CM%d" , JOB_DATA[ index ].IN_CASS + 1 , JOB_DATA[ index ].OUT_CASS + 1 ); // 2006.10.24
					//====================================================================================================================
					// 2006.10.24
					//====================================================================================================================
					if      ( _GET_CHAMBER_STRING_POSSIBLE( JOB_DATA[ index ].IN_CASS + 1 ) && _GET_CHAMBER_STRING_POSSIBLE( JOB_DATA[ index ].OUT_CASS + 1 ) ) {
						wsprintf( szString, "%s->%s" , _GET_CHAMBER_STRING( JOB_DATA[ index ].IN_CASS + 1 ) , _GET_CHAMBER_STRING( JOB_DATA[ index ].OUT_CASS + 1 ) );
					}
					else if ( _GET_CHAMBER_STRING_POSSIBLE( JOB_DATA[ index ].IN_CASS + 1 ) ) {
						wsprintf( szString, "%s->CM%d" , _GET_CHAMBER_STRING( JOB_DATA[ index ].IN_CASS + 1 ) , JOB_DATA[ index ].OUT_CASS + 1 );
					}
					else if ( _GET_CHAMBER_STRING_POSSIBLE( JOB_DATA[ index ].OUT_CASS + 1 ) ) {
						wsprintf( szString, "CM%d->%s" , JOB_DATA[ index ].IN_CASS + 1 , _GET_CHAMBER_STRING( JOB_DATA[ index ].OUT_CASS + 1 ) );
					}
					else {
						wsprintf( szString, "CM%d->CM%d" , JOB_DATA[ index ].IN_CASS + 1 , JOB_DATA[ index ].OUT_CASS + 1 );
					}
					//====================================================================================================================
				}
				break;
		case 4 : // Slot
				if ( JOB_DATA[ index ].START_SLOT == JOB_DATA[ index ].END_SLOT ) { // 2011.06.15
					wsprintf( szString, "%d" , JOB_DATA[ index ].START_SLOT );
				}
				else {
					wsprintf( szString, "%d-%d" , JOB_DATA[ index ].START_SLOT , JOB_DATA[ index ].END_SLOT );
				}
				break;
		case 5 : // Run Status
				if ( JOB_DATA[ index ].OUT_CASS < 0 ) { // 2002.10.02
					if ( JOB_HANDOFF[index] )	  wsprintf( szString, "Running" );
					else                          wsprintf( szString, "Ready" );
				}
				else {
					if      ( JOB_RUN[ index ] == JOB_READY ) wsprintf( szString, "Ready" );
					else if ( JOB_RUN[ index ] == JOB_WAITING ) {
						if ( JOB_HANDOFF[index] )	  wsprintf( szString, "Wait-Run" );
						else                          wsprintf( szString, "Wait-HandOff" );
					}
					else if ( JOB_RUN[ index ] == JOB_RUNNING ) {
						if      ( JOB_FIRSTIN[index] == 0 ) wsprintf( szString, "Running(W)" );
						else if ( JOB_LASTIN[index]  == 0 ) wsprintf( szString, "Running" );
						else                                wsprintf( szString, "Running(L)" );
					}
					else if ( JOB_RUN[ index ] == JOB_ABORTING ) wsprintf( szString, "Aborting" );
					else if ( JOB_RUN[ index ] == JOB_ENDING ) wsprintf( szString, "Ending" );
					else                              wsprintf( szString, "Unknown" );
				}
				break;
		case 6 : // MID Name
				wsprintf( szString, "%s", JOB_DATA[ index ].MID_NAME );
				break;
		case 7 : // Run Time
				wsprintf( szString , "%s" , JOB_DATA[ index ].RUN_TIME );
				break;
		case 8 : // End Time
			/*
			// 2017.03.16
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					wsprintf( szString , "%s" , JOB_DATA[ index ].END_TIME );
				}
				else {
					wsprintf( szString , "%s" , JOB_DATA[ index ].ELAPSED_TIME );
				}
				
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					wsprintf( szString , "%s" , JOB_DATA[ index ].END_TIME );
				}
				else {
					wsprintf( szString , "%s" , JOB_DATA[ index ].ELAPSED_TIME );
				}
*/
				// 2017.03.16
				switch ( Get_Info_END_TIME_SHOW_MODE() ) {
				case 1 :
					wsprintf( szString , "%s" , JOB_DATA[ index ].ELAPSED_TIME );
					break;
				case 2 :
					Get_Elapsed_String( index , szString , TRUE );
					break;
				default :
					wsprintf( szString , "%s" , JOB_DATA[ index ].END_TIME );
					break;
				}
				//
				break;
		case 9 : // Method Count

//				wsprintf( szString, "%d:%d" , JOB_DATA[ index ].METHOD_COUNT , JOB_SIDE[index] );

				if ( JOB_DATA[ index ].METHOD_COUNT > 0 ) {
					wsprintf( szString, "%d" , JOB_DATA[ index ].METHOD_COUNT );
				}
				else {
					wsprintf( szString, "" );
				}
				break;
		case 10 : // Start Time
				wsprintf( szString , "%s" , JOB_DATA[ index ].START_TIME );
				break;
//			case 11 : // Dual Type // 2002.08.09
//					if      ( JOB_DATA[ index ].PROGRESS == 0 ) wsprintf( szString , "Concurrent" );
//					else if ( JOB_DATA[ index ].PROGRESS == 1 ) wsprintf( szString , "Continue" );
//					else                                        wsprintf( szString , "Sequential" );
//					break;
//			case 11 : // End Type // 2003.07.30
//					if ( JOB_DATA[ index ].END_TIME > 0 ) {
//						if      ( JOB_DATA[ index ].END_TYPE == 0 ) wsprintf( szString, "" );
//						else if ( JOB_DATA[ index ].END_TYPE == 1 ) wsprintf( szString, "A" );
//						else if ( JOB_DATA[ index ].END_TYPE == 2 ) wsprintf( szString, "I" );
//						else if ( JOB_DATA[ index ].END_TYPE == 3 ) wsprintf( szString, "F" );
//						else                                        wsprintf( szString, "X" );
//					}
//					else {
//						wsprintf( szString, "" );
//					}
//					break;
//			case 12 : // User // 2003.07.30
		case 11 : // User // 2003.07.30
				wsprintf( szString , "%s" , JOB_DATA[ index ].USER );
				break;
//			case 13 : // Edit Time // 2003.07.30
//					if ( JOB_DATA[ index ].EDIT_TIME > 0 ) {
//						KPLT_Local_Time( JOB_DATA[ index ].EDIT_TIME , szString );
//					}
//					else {
//						wsprintf( szString , "" );
//					}
//					break;
//			case 14 : // Edit Type // 2003.07.30
		case 12 : // Edit Type // 2003.07.30
			//
			// 2016.03.29
			//
			/*
				if      ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR_N ) wsprintf( szString, "E" );
				else if ( JOB_DATA[ index ].EDIT_TYPE == ET_APPEND_N ) wsprintf( szString, "A" ); // 2010.12.06
				else if ( JOB_DATA[ index ].EDIT_TYPE == ET_MAINT_N  ) wsprintf( szString, "M" );
				else if ( JOB_DATA[ index ].EDIT_TYPE == ET_EDITOR_UABORT ) wsprintf( szString, "E" ); // 2015.11.12
				else if ( JOB_DATA[ index ].EDIT_TYPE == ET_APPEND_UABORT ) wsprintf( szString, "A" ); // 2015.11.12
				else if ( JOB_DATA[ index ].EDIT_TYPE == ET_MAINT_UABORT  ) wsprintf( szString, "M" ); // 2015.11.12
				else                                                   wsprintf( szString, "X" );
				break;
				//
			*/
			//
			//
			// 2016.03.29
			//
			switch ( JOB_DATA[ index ].EDIT_TYPE % 10 ) {
			case ET_EDITOR_N		:
			case ET_EDITOR_UABORT	:	wsprintf( szString, "E" );	break;
			case ET_APPEND_N		:
			case ET_APPEND_UABORT	:	wsprintf( szString, "A" );	break;
			case ET_MAINT_N			:
			case ET_MAINT_UABORT	:	wsprintf( szString, "M" );	break;
			default					:
										wsprintf( szString, "X" );	break;
			}
			//
			//
				//
//			case 15 : // Count // 2003.07.30
//					if ( JOB_DATA[ index ].COUNT > 0 ) {
//						wsprintf( szString, "%d" , JOB_DATA[ index ].COUNT );
//					}
//					else {
//						wsprintf( szString, "" );
//					}
//					break;
//			case 16 : // Directory // 2003.07.30
		case 13 : // Directory // 2003.07.30
				if      ( JOB_DATA[ index ].DIRECTORY_TYPE == 2 ) {
					wsprintf( szString, "-Removed-" );
				}
				else if ( JOB_DATA[ index ].DIRECTORY_TYPE == 1 ) {
					wsprintf( szString, "%s" , JOB_DATA[ index ].DIRECTORY );
				}
				else if ( JOB_DATA[ index ].DIRECTORY_TYPE == 3 ) { // 2010.12.03
					wsprintf( szString, "-Remove Fail-" );
				}
				else {
					wsprintf( szString, "" );
				}
				break;
		default :
				wsprintf( szString, "" );
				break;
	}
}







// 2012.04.12
//
/*
//
// 2017.03.13
//
char JOB_Get_Directory_Temp1[256];
char JOB_Get_Directory_Temp2[256];
//
char *JOB_Get_Directory0( int index ) {
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , JOB_Get_Directory_Temp1 , JOB_Get_Directory_Temp2 , 255 );
	return JOB_Get_Directory_Temp1;
}

char *JOB_Get_Directory( int index ) {
	int c;
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , JOB_Get_Directory_Temp1 , JOB_Get_Directory_Temp2 , 255 );
	//
	if ( JOB_Get_Directory_Temp2[0] == 0 ) return JOB_Get_Directory_Temp1;
	//
	if ( JOB_SEL_LOOPCOUNT[index] <= 0 ) return JOB_Get_Directory_Temp1;
	//
	c = atoi(JOB_Get_Directory_Temp2+4);
	//
	if ( JOB_SEL_LOOPCOUNT[index] >= c ) {
		JOB_SEL_LOOPCOUNT[index] = 0;
		return JOB_Get_Directory_Temp1;
	}
	//
	sprintf( JOB_Get_Directory_Temp2 , "%s\\[LOOP%d]" , JOB_Get_Directory_Temp1 , JOB_SEL_LOOPCOUNT[index] + 1 );
	//
	return JOB_Get_Directory_Temp2;
}

char *JOB_Get_Directory2( int index , int *loop ) {
	//
	char JOB_Get_Directory_Temp1[256]; // 2017.03.13
	char JOB_Get_Directory_Temp2[256]; // 2017.03.13
	//
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , JOB_Get_Directory_Temp1 , JOB_Get_Directory_Temp2 , 255 );
	if ( JOB_Get_Directory_Temp2[0] == 0 ) {
		*loop = 0;
	}
	else {
		*loop = atoi(JOB_Get_Directory_Temp2+4);
		//
		if ( JOB_SEL_LOOPCOUNT[index] >= *loop ) JOB_SEL_LOOPCOUNT[index] = 0;
		//
	}
	return JOB_Get_Directory_Temp1;
}
*/


//
// 2017.03.13
//
void JOB_Get_Directory_0( int index , char *retstr ) {
	//
	char JOB_Get_Directory_Temp[256];
	//
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , retstr , JOB_Get_Directory_Temp , 255 );
	//
}

void JOB_Get_Directory( int index , char *retstr ) {
	int c;
	//
	char JOB_Get_Directory_Temp[256];
	//
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , retstr , JOB_Get_Directory_Temp , 255 );
	//
	if ( retstr[0] == 0 ) return;
	//
	if ( JOB_SEL_LOOPCOUNT[index] <= 0 ) return;
	//
	c = atoi(JOB_Get_Directory_Temp+4);
	//
	if ( JOB_SEL_LOOPCOUNT[index] >= c ) {
		JOB_SEL_LOOPCOUNT[index] = 0;
		return;
	}
	//
	strcpy( JOB_Get_Directory_Temp , retstr );
	sprintf( retstr , "%s\\[LOOP%d]" , JOB_Get_Directory_Temp , JOB_SEL_LOOPCOUNT[index] + 1 );
	//
}

void JOB_Get_Directory_2( int index , int *loop , char *retstr ) {
	//
	char JOB_Get_Directory_Temp[256];
	//
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , retstr , JOB_Get_Directory_Temp , 255 );
	//
	if ( JOB_Get_Directory_Temp[0] == 0 ) {
		*loop = 0;
	}
	else {
		*loop = atoi(JOB_Get_Directory_Temp+4);
		//
		if ( JOB_SEL_LOOPCOUNT[index] >= *loop ) JOB_SEL_LOOPCOUNT[index] = 0;
		//
	}
}



int JOB_Get_LoopCount( int index , int *curr ) {
	//
	char JOB_Get_Directory_Temp1[256]; // 2017.03.13
	char JOB_Get_Directory_Temp2[256]; // 2017.03.13
	//
	int max;
	*curr = 0;
	//
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , JOB_Get_Directory_Temp1 , JOB_Get_Directory_Temp2 , 255 );
	if ( JOB_Get_Directory_Temp2[0] == 0 ) return 0;
	max = atoi(JOB_Get_Directory_Temp2+4);
	if ( JOB_SEL_LOOPCOUNT[index] >= max ) JOB_SEL_LOOPCOUNT[index] = 0;
	*curr = JOB_SEL_LOOPCOUNT[index];
	return max;
}

BOOL JOB_Has_LoopCount( int index ) {
	//
	char JOB_Get_Directory_Temp1[256]; // 2017.03.13
	char JOB_Get_Directory_Temp2[256]; // 2017.03.13
	//
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , JOB_Get_Directory_Temp1 , JOB_Get_Directory_Temp2 , 255 );
	if ( JOB_Get_Directory_Temp2[0] == 0 ) return FALSE;
	return ( atoi(JOB_Get_Directory_Temp2+4) > 1 );
}

void JOB_Set_Directory( int index , char *data , int lpd ) {
	//
	char JOB_Get_Directory_Temp1[256]; // 2017.03.13
	char JOB_Get_Directory_Temp2[256]; // 2017.03.13
	//
	STR_SEPERATE_CHAR( JOB_DATA[ index ].DIRECTORY , '|' , JOB_Get_Directory_Temp1 , JOB_Get_Directory_Temp2 , 255 );
	//
	if ( lpd > 0 ) sprintf( JOB_Get_Directory_Temp2 , "LOOP%d" , lpd );
	//
	if ( data != NULL ) strcpy( JOB_Get_Directory_Temp1 , data );
	//
	if ( JOB_Get_Directory_Temp2[0] == 0 ) {
		strcpy( JOB_DATA[ index ].DIRECTORY , JOB_Get_Directory_Temp1 );
	}
	else {
		sprintf( JOB_DATA[ index ].DIRECTORY , "%s|%s" , JOB_Get_Directory_Temp1 , JOB_Get_Directory_Temp2 );
	}
	JOB_DATA[ index ].DIRECTORY_size = strlen( JOB_DATA[ index ].DIRECTORY );
}

void JOB_Set_Sel_LoopCount( int index , int data ) {
	JOB_SEL_LOOPCOUNT[index] = data;
}



/*
// 2012.04.12
char *JOB_Get_Directory( int index ) {
	return JOB_DATA[ index ].DIRECTORY;
}
*/




int JOB_Get_Directory_Type( int index ) {
	return ( (int) JOB_DATA[ index ].DIRECTORY_TYPE );
}

int JOB_Get_Run_Status( int index ) {
	return JOB_RUN[ index ];
}

//int JOB_Get_Run_Extern_Status( int index ) { // 2016.03.29
int JOB_Get_Run_Extern_Status( int index , int *multiicon ) { // 2016.03.29
	int ms;
	//
	*multiicon = -1; // 2016.03.29
	//
	if ( JOB_DATA[ index ].OUT_CASS < 0 ) {
		if ( JOB_HANDOFF[index] )	return 1;
	}
	else {
		//
		if      ( JOB_RUN[ index ] == JOB_READY ) {
		}
		else if ( JOB_RUN[ index ] == JOB_WAITING ) {
		}
		else if ( JOB_RUN[ index ] == JOB_RUNNING ) {
			//
//			switch( JOB_DATA[ index ].EDIT_TYPE ) { // 2015.11.12 // 2016.03.29
//			case ET_EDITOR_UABORT :
//			case ET_APPEND_UABORT :
//			case ET_MAINT_UABORT :
//				//
//				return 4;
//				//
//				break;
//			}
			//
			ms = JOB_User_Get_Icon( index ); // 2016.03.29
			//
			if ( ms > 0 ) { // 2016.03.29
				*multiicon = ms;
				return 4;
			}
			//
			return 1;
		}
		else if ( JOB_RUN[ index ] == JOB_ABORTING ) {
			return 2;
		}
		else if ( JOB_RUN[ index ] == JOB_ENDING ) {
			//
//			switch( JOB_DATA[ index ].EDIT_TYPE ) { // 2015.11.12 // 2016.03.29
//			case ET_EDITOR_UABORT :
//			case ET_APPEND_UABORT :
//			case ET_MAINT_UABORT :
//				//
//				return 4;
//				//
//				break;
//			}
			//
			ms = JOB_User_Get_Icon( index ); // 2016.03.29
			//
			if ( ms > 0 ) { // 2016.03.29
				*multiicon = ms;
				return 4;
			}
			//
			return 1;
		}
		else {
			return 2;
		}
		//
	}
	//
//	switch( JOB_DATA[ index ].EDIT_TYPE ) { // 2015.11.12 // 2016.03.29
//	case ET_EDITOR_UABORT :
//	case ET_APPEND_UABORT :
//	case ET_MAINT_UABORT :
//		//
//		return 4;
//		//
//		break;
//	}
	//
	//
	ms = JOB_User_Get_Icon( index ); // 2016.03.29
	//
	if ( ms > 0 ) { // 2016.03.29
		*multiicon = ms;
		return 4;
	}
	//
	if ( JOB_DATA[ index ].END_TIME_size <= 3 ) {
		if ( JOB_DATA[ index ].ELAPSED_TIME_size <= 3 ) {
			return 3;
		}
		else {
			if ( STRCMP_L( JOB_DATA[ index ].ELAPSED_TIME + ( JOB_DATA[ index ].ELAPSED_TIME_size - 3 ) , "(F)" ) ) {
				return 2;
			}
		}
	}
	else {
		if ( STRCMP_L( JOB_DATA[ index ].END_TIME + ( JOB_DATA[ index ].END_TIME_size - 3 ) , "(F)" ) ) {
			return 2;
		}
	}
	//
	return 0;
}

BOOL JOB_Get_HandOff_Status( int index ) {
	return JOB_HANDOFF[ index ];
}
//
//
void JOB_Get_Display_String( int index , char *data ) {
	int lpcnt_max , lpcnt_curr;
	char lpcnt_buffer[64];
	//
	strcpy( data , "[" );
	if ( strlen( JOB_DATA[ index ].JOB_NAME ) <= 0 ) {
		strcat( data , "???" );
	}
	else {
		strcat( data , JOB_DATA[ index ].JOB_NAME );
	}
	strcat( data , "," );
	if ( strlen( JOB_DATA[ index ].MID_NAME ) <= 0 ) {
		strcat( data , "???" );
	}
	else {
		strcat( data , JOB_DATA[ index ].MID_NAME );
	}
	strcat( data , "," );
	if ( strlen( JOB_DATA[ index ].LOT_RECIPE ) <= 0 ) {
		strcat( data , "???" );
	}
	else {
		strcat( data , JOB_DATA[ index ].LOT_RECIPE );
	}
	strcat( data , "] [" );
	strcat( data , JOB_DATA[ index ].RUN_TIME );
	strcat( data , "]" );
	//
	// 2012.04.13
	lpcnt_max = JOB_Get_LoopCount( index , &lpcnt_curr );
	//
	if ( lpcnt_max > 1 ) {
		sprintf( lpcnt_buffer , "[LOOP %d]" , lpcnt_curr + 1 );
		strcat( data , lpcnt_buffer );
	}
	//
}
//
void JOB_Get_Display_String2( int index , char *defmsg , char *data ) {
	int lpcnt_max , lpcnt_curr;
	char lpcnt_buffer[64];
	//
	int d = 0;
	//
	strcpy( data , "" );
	if ( strlen( JOB_DATA[ index ].JOB_NAME ) > 0 ) {
		d = 1;
		strcat( data , JOB_DATA[ index ].JOB_NAME );
	}
	if ( strlen( JOB_DATA[ index ].MID_NAME ) > 0 ) {
		if ( d == 1 ) strcat( data , "," );
		d = 1;
		strcat( data , JOB_DATA[ index ].MID_NAME );
	}
	if ( strlen( JOB_DATA[ index ].LOT_RECIPE ) > 0 ) {
		if ( d == 1 ) strcat( data , "," );
		d = 1;
		strcat( data , JOB_DATA[ index ].LOT_RECIPE );
	}
	//
	// 2012.04.13
	lpcnt_max = JOB_Get_LoopCount( index , &lpcnt_curr );
	//
	if ( lpcnt_max > 1 ) {
		//
		sprintf( lpcnt_buffer , "LOOP:%d" , lpcnt_curr + 1 );
		//
		if ( d == 1 ) strcat( data , "," );
		d = 1;
		strcat( data , lpcnt_buffer );
	}
	//
	if ( d == 0 ) strcpy( data , defmsg );
}
//
//
BOOL JOB_Insert_New_Job_Data( char *name , char *recipe , int ic , int oc , int ss , int es , int prg ) {
	char data[ MAX_STRING_SIZE + 1 ];
	time_t tempt; // 2011.05.12

	if ( JOBCOUNT >= MAX_JOB_COUNT ) return FALSE; // 2003.09.17

	strcpy( JOB_DATA[ JOBCOUNT ].JOB_NAME , name );		JOB_DATA[ JOBCOUNT ].JOB_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].JOB_NAME );	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].MID_NAME , "" );		JOB_DATA[ JOBCOUNT ].MID_NAME_size = 0;	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].LOT_RECIPE , recipe );	JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = strlen( JOB_DATA[ JOBCOUNT ].LOT_RECIPE );	// 2003.07.30
	JOB_DATA[ JOBCOUNT ].IN_CASS = (char) ic;
	JOB_DATA[ JOBCOUNT ].OUT_CASS = (char) oc;
	JOB_DATA[ JOBCOUNT ].START_SLOT = (char) ss;
	JOB_DATA[ JOBCOUNT ].END_SLOT = (char) es;
	JOB_DATA[ JOBCOUNT ].PROGRESS = (char) prg;
	//
	JOB_DATA[ JOBCOUNT ].METHOD_COUNT = 0; // 2010.12.02
	//
	GET_CURRENT_USERNAME( data );
	strncpy( JOB_DATA[ JOBCOUNT ].USER , data , MAX_STRING_SIZE_22 );	JOB_DATA[ JOBCOUNT ].USER_size = strlen( JOB_DATA[ JOBCOUNT ].USER );	// 2003.07.30
//	JOB_DATA[ JOBCOUNT ].EDIT_TYPE = ET_EDITOR; // 2015.11.12
	JOB_DATA[ JOBCOUNT ].EDIT_TYPE = ET_EDITOR_N; // 2015.11.12
//	time( &(JOB_DATA[ JOBCOUNT ].EDIT_TIME) ); // 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].RUN_TIME , "" );		JOB_DATA[ JOBCOUNT ].RUN_TIME_size = 0;	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].START_TIME , "" );		JOB_DATA[ JOBCOUNT ].START_TIME_size = 0;	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].END_TIME , "" );		JOB_DATA[ JOBCOUNT ].END_TIME_size = 0;	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].ELAPSED_TIME , "" );	JOB_DATA[ JOBCOUNT ].ELAPSED_TIME_size = 0;	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].DIRECTORY , "" );		JOB_DATA[ JOBCOUNT ].DIRECTORY_size = 0;	// 2003.07.30
	JOB_DATA[ JOBCOUNT ].DIRECTORY_TYPE = 0;
//	JOB_DATA[ JOBCOUNT ].DIRECTORY_DATE = 0; // 2002.10.18
	//
//	time( &( JOB_DATA[ JOBCOUNT ].DIRECTORY_DATE ) ); // 2011.05.12
	//
	time( &tempt ); // 2011.05.12
	JOB_DATA[ JOBCOUNT ].DIRECTORY_DATE = (long) tempt; // 2011.05.12
	//
//	JOB_DATA[ JOBCOUNT ].COUNT = 0; // 2003.07.30
	//
	JOB_RUN[ JOBCOUNT ] = JOB_READY;
	//
	JOB_User_Fail_Check( JOBCOUNT ); // 2015.11.12
	//
	JOB_HANDOFF[ JOBCOUNT ] = FALSE;
	JOB_FIRSTIN[ JOBCOUNT ] = 0;
	JOB_LASTIN[ JOBCOUNT ] = 0;
	JOB_SEL_LOOPCOUNT[ JOBCOUNT ] = 0;
	//
	JOB_DATA[ JOBCOUNT ].STATUS_SAVE = 1; // 2003.06.09
	//
	JOBCOUNT++;
	return TRUE;
}
//
int JOB_Insert_New_Job_Data_for_Extern(
										int runmode ,
										char *jname ,
										char *mname ,
										char *recipe ,
										int ic , int ss , int es ,
										char *runtime ,
										char *starttime ,
										char *endtime ,
										char *elapsedtime ,
										char *folder , // 2010.12.06
//										int cnt ) { // 2015.11.13
//										int cnt , int *failindex ) { // 2015.11.13 // 2016.03.29
										int cnt , int *failindex , int *failicon ) { // 2015.11.13 // 2016.03.29
	char data[ MAX_STRING_SIZE + 1 ];
	int i;
	time_t tempt; // 2011.05.12
	//
	*failindex = -1; // 2015.11.13
	//
	if ( runmode >= 0 ) {

		if ( JOBCOUNT >= MAX_JOB_COUNT ) return -1; // 2003.09.17

		strcpy( JOB_DATA[ JOBCOUNT ].JOB_NAME , jname );		JOB_DATA[ JOBCOUNT ].JOB_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].JOB_NAME );	// 2003.07.30
		strcpy( JOB_DATA[ JOBCOUNT ].MID_NAME , mname );		JOB_DATA[ JOBCOUNT ].MID_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].MID_NAME );	// 2003.07.30
		strcpy( JOB_DATA[ JOBCOUNT ].LOT_RECIPE , recipe );		JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = strlen( JOB_DATA[ JOBCOUNT ].LOT_RECIPE );	// 2003.07.30
		JOB_DATA[ JOBCOUNT ].IN_CASS = (char) ic;
		if ( ( runmode / 2 ) == 0 ) { // 2011.06.15
			JOB_DATA[ JOBCOUNT ].OUT_CASS = -1;
		}
		else if ( ( runmode / 2 ) == 1 ) { // 2011.06.15
			JOB_DATA[ JOBCOUNT ].OUT_CASS = -2;
		}
		else if ( ( runmode / 2 ) == 2 ) { // 2011.06.15
			JOB_DATA[ JOBCOUNT ].OUT_CASS = -3;
		}
		else { // 2011.06.15
			JOB_DATA[ JOBCOUNT ].OUT_CASS = -4;
		}
		JOB_DATA[ JOBCOUNT ].START_SLOT = (char) ss;
		JOB_DATA[ JOBCOUNT ].END_SLOT = (char) es;
		JOB_DATA[ JOBCOUNT ].PROGRESS = 0;
		GET_CURRENT_USERNAME( data );
		strncpy( JOB_DATA[ JOBCOUNT ].USER , data , MAX_STRING_SIZE_22 );	JOB_DATA[ JOBCOUNT ].USER_size = strlen( JOB_DATA[ JOBCOUNT ].USER );	// 2003.07.30
//		JOB_DATA[ JOBCOUNT ].EDIT_TYPE = ET_APPEND; // 2015.11.12
		JOB_DATA[ JOBCOUNT ].EDIT_TYPE = ET_APPEND_N; // 2015.11.12
//		time( &(JOB_DATA[ JOBCOUNT ].EDIT_TIME) ); // 2003.07.30
		strcpy( JOB_DATA[ JOBCOUNT ].RUN_TIME , runtime );			JOB_DATA[ JOBCOUNT ].RUN_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].RUN_TIME );	// 2003.07.30
		strcpy( JOB_DATA[ JOBCOUNT ].START_TIME , starttime );		JOB_DATA[ JOBCOUNT ].START_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].START_TIME );	// 2003.07.30
		strcpy( JOB_DATA[ JOBCOUNT ].END_TIME , endtime );			JOB_DATA[ JOBCOUNT ].END_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].END_TIME );	// 2003.07.30
		strcpy( JOB_DATA[ JOBCOUNT ].ELAPSED_TIME , elapsedtime );	JOB_DATA[ JOBCOUNT ].ELAPSED_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].ELAPSED_TIME );	// 2003.07.30
		//
		if ( folder[0] == 0 ) { // 2010.12.06
			//
			strcpy( JOB_DATA[ JOBCOUNT ].DIRECTORY , "" );				JOB_DATA[ JOBCOUNT ].DIRECTORY_size = 0;	// 2003.07.30
			//
			JOB_DATA[ JOBCOUNT ].DIRECTORY_TYPE = 0;
		}
		else { // 2010.12.06
			//
			strcpy( JOB_DATA[ JOBCOUNT ].DIRECTORY , folder );			JOB_DATA[ JOBCOUNT ].DIRECTORY_size = strlen( JOB_DATA[ JOBCOUNT ].DIRECTORY );
			//
			JOB_DATA[ JOBCOUNT ].DIRECTORY_TYPE = 1;
		}
		//
//		time( &( JOB_DATA[ JOBCOUNT ].DIRECTORY_DATE ) ); // 2011.05.12
		//
		time( &tempt ); // 2011.05.12
		JOB_DATA[ JOBCOUNT ].DIRECTORY_DATE = (long) tempt; // 2011.05.12
		//
//		JOB_DATA[ JOBCOUNT ].COUNT = 0; // 2003.07.30
		JOB_DATA[ JOBCOUNT ].METHOD_COUNT = cnt;
		//
		JOB_SIDE[ JOBCOUNT ] = 0;
		//
		JOB_RUN[ JOBCOUNT ] = JOB_READY;
		//
		JOB_User_Fail_Check( JOBCOUNT ); // 2015.11.12
		//
		if ( ( runmode % 2 ) == 0 ) {
			JOB_HANDOFF[ JOBCOUNT ] = FALSE;
		}
		else {
			JOB_HANDOFF[ JOBCOUNT ] = TRUE;
		}
		JOB_FIRSTIN[ JOBCOUNT ] = 0;
		JOB_LASTIN[ JOBCOUNT ] = 0;
		JOB_SEL_LOOPCOUNT[ JOBCOUNT ] = 0;
		//
		JOB_DATA[ JOBCOUNT ].STATUS_SAVE = 1; // 2003.06.09
		//
		JOBCOUNT++;
		//
		return 1;
	}
	else {
		for ( i = 0 ; i < JOBCOUNT ; i++ ) {
			if ( !STRCMP_L( JOB_DATA[ i ].JOB_NAME , jname ) ) continue; // 2011.06.16
			if ( !STRCMP_L( JOB_DATA[ i ].MID_NAME , mname ) ) continue; // 2011.06.16
			if ( !STRCMP_L( JOB_DATA[ i ].LOT_RECIPE , recipe ) ) continue; // 2011.06.16
			if ( JOB_DATA[ i ].IN_CASS != ic ) continue; // 2011.07.15
			if ( JOB_DATA[ i ].START_SLOT != (char) ss ) continue; // 2011.07.15
			if ( JOB_DATA[ i ].END_SLOT != (char) es ) continue; // 2011.07.15
			//
			if ( ( JOB_DATA[ i ].OUT_CASS < 0 ) && ( JOB_HANDOFF[ i ] ) ) {
				//
				if ( cnt > 0 ) { // 2010.12.06
					JOB_DATA[ i ].METHOD_COUNT = cnt;
					JOB_DATA[ i ].STATUS_SAVE = 1; // 2011.06.15
				}
				//
				if ( runtime[0] != 0 ) { // 2011.06.15
					strcpy( JOB_DATA[ i ].RUN_TIME , runtime );			JOB_DATA[ i ].RUN_TIME_size = strlen( JOB_DATA[ i ].RUN_TIME );
					JOB_DATA[ i ].STATUS_SAVE = 1; // 2011.06.15
				}
				//
				if ( starttime[0] != 0 ) { // 2011.06.15
					strcpy( JOB_DATA[ i ].START_TIME , starttime );		JOB_DATA[ i ].START_TIME_size = strlen( JOB_DATA[ i ].START_TIME );
					JOB_DATA[ i ].STATUS_SAVE = 1; // 2011.06.15
				}
				//
				if ( endtime[0] != 0 ) { // 2011.06.15
					strcpy( JOB_DATA[ i ].END_TIME , endtime );			JOB_DATA[ i ].END_TIME_size = strlen( JOB_DATA[ i ].END_TIME );
					JOB_DATA[ i ].STATUS_SAVE = 1; // 2011.06.15
				}
				//
				if ( elapsedtime[0] != 0 ) { // 2011.06.15
					strcpy( JOB_DATA[ i ].ELAPSED_TIME , elapsedtime );	JOB_DATA[ i ].ELAPSED_TIME_size = strlen( JOB_DATA[ i ].ELAPSED_TIME );
					JOB_DATA[ i ].STATUS_SAVE = 1; // 2011.06.15
				}
				//
				if ( folder[0] != 0 ) { // 2011.06.15
					strcpy( JOB_DATA[ i ].DIRECTORY , folder );			JOB_DATA[ i ].DIRECTORY_size = strlen( JOB_DATA[ i ].DIRECTORY );
					JOB_DATA[ i ].STATUS_SAVE = 1; // 2011.06.15
				}
				//
				JOB_HANDOFF[ i ] = FALSE;
				//
//				if ( JOB_User_Fail_Check( i ) ) *failindex = i; // 2015.11.13 // 2016.03.29
				//
				if ( JOB_User_Fail_Check( i ) ) { // 2015.11.13 // 2016.03.29
					*failindex = i;
					*failicon = JOB_User_Get_Icon( i );
				}
				//
				return 0;
			}
			//
		}
	}
	return -1;
}
//
//
BOOL JOB_Insert_New_Job_Data_for_Maint( int Side ) {
	int os , osmode;
	char data[ MAX_STRING_SIZE + 1 ];
	time_t tempt; // 2011.05.12

	if ( JOBCOUNT >= MAX_JOB_COUNT ) return FALSE; // 2003.09.17

	Get_Job_Name( Side , JOB_DATA[ JOBCOUNT ].JOB_NAME );
	if ( strlen( JOB_DATA[ JOBCOUNT ].JOB_NAME ) <= 0 ) strcpy( JOB_DATA[ JOBCOUNT ].JOB_NAME , "-MANUAL-" );
	//
	JOB_DATA[ JOBCOUNT ].JOB_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].JOB_NAME );	// 2003.07.30
	//
	Get_MID_Name( Side , JOB_DATA[ JOBCOUNT ].MID_NAME );	JOB_DATA[ JOBCOUNT ].MID_NAME_size = strlen( JOB_DATA[ JOBCOUNT ].MID_NAME );	// 2003.07.30
	//
	GET_CURRENT_USERNAME( data );
	strncpy( JOB_DATA[ JOBCOUNT ].USER , data , MAX_STRING_SIZE_22 );	JOB_DATA[ JOBCOUNT ].USER_size = strlen( JOB_DATA[ JOBCOUNT ].USER );	// 2003.07.30
	//
//	JOB_DATA[ JOBCOUNT ].EDIT_TIME = 0; // 2003.07.30
	//
	JOB_SIDE[ JOBCOUNT ] = Side;
	JOB_RUN[ JOBCOUNT ] = JOB_RUNNING;
	JOB_HANDOFF[ JOBCOUNT ] = FALSE;
	JOB_FIRSTIN[ JOBCOUNT ] = 0;
	JOB_LASTIN[ JOBCOUNT ] = 0;
	JOB_SEL_LOOPCOUNT[ JOBCOUNT ] = 0;
	//
//	JOB_DATA[ JOBCOUNT ].COUNT = 0; // 2003.07.30
//	JOB_DATA[ JOBCOUNT ].EDIT_TYPE = ET_MAINT; // 2003.07.30 // 2015.11.12
	JOB_DATA[ JOBCOUNT ].EDIT_TYPE = ET_MAINT_N; // 2003.07.30 // 2015.11.12
	//
	JOB_DATA[ JOBCOUNT ].METHOD_COUNT = 0; // 2010.12.02
//	Get_Lot_Recipe( Side , JOB_DATA[ JOBCOUNT ].LOT_RECIPE );	JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = strlen( JOB_DATA[ JOBCOUNT ].LOT_RECIPE );	// 2003.07.30 // 2010.03.10
	//
	//=================================================================================
	// 2007.01.05 // 2010.12.02
	//=================================================================================
//	if ( JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size <= 0 ) {
//		strcpy( JOB_DATA[ JOBCOUNT ].LOT_RECIPE , " " );
//		JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = 1;
//	}
	//=================================================================================
	JOB_DATA[ JOBCOUNT ].IN_CASS = (char) Get_In_Cassette( Side );
	JOB_DATA[ JOBCOUNT ].OUT_CASS = (char) Get_Out_Cassette( Side );
	JOB_DATA[ JOBCOUNT ].START_SLOT = (char) Get_Start_Slot( Side );
	JOB_DATA[ JOBCOUNT ].END_SLOT = (char) Get_End_Slot( Side );
	//
	Get_Lot_Recipe( Side , JOB_DATA[ JOBCOUNT ].IN_CASS + 1 , JOB_DATA[ JOBCOUNT ].LOT_RECIPE );	JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = strlen( JOB_DATA[ JOBCOUNT ].LOT_RECIPE );	// 2010.03.10
	//
	//=================================================================================
	// 2007.01.05 // 2010.12.02
	//=================================================================================
	if ( JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size <= 0 ) {
		strcpy( JOB_DATA[ JOBCOUNT ].LOT_RECIPE , " " );
		JOB_DATA[ JOBCOUNT ].LOT_RECIPE_size = 1;
	}
	//=================================================================================
	//
	Get_Start_Time( Side , FALSE , JOB_DATA[ JOBCOUNT ].START_TIME , &osmode , &os );	JOB_DATA[ JOBCOUNT ].START_TIME_size = strlen( JOB_DATA[ JOBCOUNT ].START_TIME );	// 2003.07.30
	//
	strcpy( JOB_DATA[ JOBCOUNT ].RUN_TIME , "" );				JOB_DATA[ JOBCOUNT ].RUN_TIME_size = 0;	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].END_TIME , "" );				JOB_DATA[ JOBCOUNT ].END_TIME_size = 0;	// 2003.07.30
	strcpy( JOB_DATA[ JOBCOUNT ].ELAPSED_TIME , "" );			JOB_DATA[ JOBCOUNT ].ELAPSED_TIME_size = 0;	// 2003.07.30
	Get_Directory( Side , JOB_DATA[ JOBCOUNT ].DIRECTORY );		JOB_DATA[ JOBCOUNT ].DIRECTORY_size = strlen( JOB_DATA[ JOBCOUNT ].DIRECTORY );	// 2003.07.30
	JOB_DATA[ JOBCOUNT ].DIRECTORY_TYPE = 1;

//	time( &( JOB_DATA[ JOBCOUNT ].DIRECTORY_DATE ) ); // 2011.05.12
	//
	time( &tempt ); // 2011.05.12
	JOB_DATA[ JOBCOUNT ].DIRECTORY_DATE = (long) tempt; // 2011.05.12
	//
	JOB_DATA[ JOBCOUNT ].STATUS_SAVE = 1; // 2003.06.09
	//
	JOBCOUNT++;
	return TRUE;
}



BOOL JOB_Question_Name_Exist( char *name ) {
	int i;
	for ( i = 0 ; i < JOBCOUNT ; i++ ) {
		if ( STRCMP_L( name , JOB_DATA[i].JOB_NAME ) ) break;
	}
	if ( i != JOBCOUNT ) return TRUE;
	return FALSE;
}

int JOB_Find_Search( char *jname , char *lotname , char *midname , BOOL Full ) {
	int i;
	int jl , ll , ml;
	jl = strlen( jname );
	ll = strlen( lotname );
	ml = strlen( midname );
	if ( Full ) {
		for ( i = ( JOBCOUNT - 1 ) ; i >= 0 ; i-- ) {
			if ( ( jl <= 0 ) || ( STRCMP_L( jname , JOB_DATA[i].JOB_NAME ) ) ) {
				if ( ( ll <= 0 ) || ( STRCMP_L( lotname , JOB_DATA[i].LOT_RECIPE ) ) ) {
					if ( ( ml <= 0 ) || ( STRCMP_L( midname , JOB_DATA[i].MID_NAME ) ) ) {
						return i;
					}
				}
			}
		}
	}
	else {
		for ( i = ( JOBCOUNT - 1 ) ; i >= 0 ; i-- ) {
			if ( ( jl <= 0 ) || ( STRNCMP_L( jname , JOB_DATA[i].JOB_NAME , jl ) ) ) {
				if ( ( ll <= 0 ) || ( STRNCMP_L( lotname , JOB_DATA[i].LOT_RECIPE , ll ) ) ) {
					if ( ( ml <= 0 ) || ( STRNCMP_L( midname , JOB_DATA[i].MID_NAME , ml ) ) ) {
						return i;
					}
				}
			}
		}
	}
	return -1;
}
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
int  JOB_FIND_Get_Current_Count() { return FIND_COUNT; }
//
int  JOB_FIND_Get_Real_Index( int id ) {
	return FIND_INDEX[id];
}
//
void JOB_FIND_GO_BACK( HWND hWnd ) {
	ListView_DeleteAllItems( hWnd );
	FIND_COUNT = 0;
}
//------------------------------------------------------------------------------------------
BOOL JOB_FIND_WITH_WID( char *FIND_WID , int wl , int index , int mode ) {
	FILE *fpt;
	BOOL bres;
	char Buffer[256];
	char Buffer1[65];
	int encflag;
	//
	JOB_Get_Directory( index , Buffer ); // 2017.03.13
	//
	sprintf( Buffer1 , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , Buffer , METHOD_LOG_FILE );
	//
	if ( ( fpt = fopen( Buffer1 , "r" ) ) == NULL ) return FALSE; // 2005.03.22
	//
	encflag = 0; // 2011.12.07
	//
	do {
		bres = Read_Line_From_File4( fpt , Buffer , 255 );
		if ( *Buffer != 0 ) {
			//
//			Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
			//
			STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , 0 , 64 );
			if ( mode == 0 ) {
				if ( STRNCMP_L( Buffer1 , FIND_WID , wl ) )	{
					fclose( fpt );
					return TRUE;
				}
			}
			else {
				if ( STRFIND_SUBSTRING_L( Buffer1 , FIND_WID ) ) {
					fclose( fpt );
					return TRUE;
				}
			}
		}
	}
	while ( bres );
	fclose( fpt );
	return FALSE;
}
//------------------------------------------------------------------------------------------
void JOB_FIND_SEARCH_START( HWND hWnd ) {
	int i , and , bi , ltime , cstime , cetime;
	int jl = 0 , ll = 0 , ml = 0 , wl = 0;
	int z1 , z2;
	struct tm *time_data;
	BOOL Res;
	time_t tempt;
	//
	if ( FIND_JID_RUN ) jl = strlen( FIND_JID );
	if ( FIND_LID_RUN ) ll = strlen( FIND_LID );
	if ( FIND_MID_RUN ) ml = strlen( FIND_MID );
	if ( FIND_WID_RUN ) wl = strlen( FIND_WID );
	//

	/*
	// 2011.06.20
	switch( FIND_OPTION5 ) {
	case 1  : z1 = 1; z2 = 0; break; // E
	case 2  : z1 = 0; z2 = 1; break; // N
	case 3  : z1 = 1; z2 = 1; break; // E
	default : z1 = 0; z2 = 0; break; // N
	}
	*/
	// 2011.06.20
	switch( FIND_OPTION5 ) {
	case 1  : z1 = 1; z2 = 1; break; // E
	case 2  : z1 = 0; z2 = 0; break; // N(F)
	case 3  : z1 = 1; z2 = 0; break; // E(F)
	default : z1 = 0; z2 = 1; break; // N
	}

	FIND_COUNT = 0;
	if ( FIND_START_RUN )	cstime = (FIND_START_DATE_Y*365) + (FIND_START_DATE_M*31) + FIND_START_DATE_D;
	else					cstime = -1;
	if ( FIND_END_RUN )		cetime = (FIND_END_DATE_Y*365) + (FIND_END_DATE_M*31) + FIND_END_DATE_D;
	else					cetime = 99999999;
	for ( i = 0 ; i < JOBCOUNT ; i++ ) {
		if ( JOB_RUN[i] != JOB_READY ) continue;
		bi = FALSE;
		if ( jl > 0 ) {
			bi  = TRUE;
			if ( z2 == 0 ) {
				if ( STRNCMP_L( JOB_DATA[i].JOB_NAME , FIND_JID , jl ) )	Res = TRUE;
				else														Res = FALSE;
			}
			else {
				if ( STRFIND_SUBSTRING_L( JOB_DATA[i].JOB_NAME , FIND_JID ) )	Res = TRUE;
				else															Res = FALSE;
			}
			and = FIND_OPTION1;
		}
		//
		if ( ll > 0 ) {
			if ( !bi ) {
				bi = TRUE;
				if ( z2 == 0 ) {
					if ( STRNCMP_L( JOB_DATA[i].LOT_RECIPE , FIND_LID , ll ) )	Res = TRUE;
					else														Res = FALSE;
				}
				else {
					if ( STRFIND_SUBSTRING_L( JOB_DATA[i].LOT_RECIPE , FIND_LID ) )	Res = TRUE;
					else														Res = FALSE;
				}
			}
			else {
				if ( z2 == 0 ) {
					if ( STRNCMP_L( JOB_DATA[i].LOT_RECIPE , FIND_LID , ll ) ) {
						if ( !and ) Res = TRUE;
					}
					else {
						if ( and ) Res = FALSE;
					}
				}
				else {
					if ( STRFIND_SUBSTRING_L( JOB_DATA[i].LOT_RECIPE , FIND_LID ) ) {
						if ( !and ) Res = TRUE;
					}
					else {
						if ( and ) Res = FALSE;
					}
				}
			}
			and = FIND_OPTION2;
		}
		//
		if ( ml > 0 ) {
			if ( !bi ) {
				bi = TRUE;
				if ( z2 == 0 ) {
					if ( STRNCMP_L( JOB_DATA[i].MID_NAME , FIND_MID , ml ) )	Res = TRUE;
					else														Res = FALSE;
				}
				else {
					if ( STRFIND_SUBSTRING_L( JOB_DATA[i].MID_NAME , FIND_MID ) )	Res = TRUE;
					else														Res = FALSE;
				}
			}
			else {
				if ( z2 == 0 ) {
					if ( STRNCMP_L( JOB_DATA[i].MID_NAME , FIND_MID , ml ) ) {
						if ( !and ) Res = TRUE;
					}
					else {
						if ( and ) Res = FALSE;
					}
				}
				else {
					if ( STRFIND_SUBSTRING_L( JOB_DATA[i].MID_NAME , FIND_MID ) ) {
						if ( !and ) Res = TRUE;
					}
					else {
						if ( and ) Res = FALSE;
					}
				}
			}
			and = FIND_OPTION3;
		}
		//
		if ( FIND_START_RUN || FIND_END_RUN ) {
			//
//			time_data = localtime( &(JOB_DATA[i].DIRECTORY_DATE) ); // 2011.05.12
			//
			tempt = (time_t) JOB_DATA[i].DIRECTORY_DATE; // 2011.05.12
			time_data = localtime( &tempt ); // 2011.05.12
			//
			ltime = ((time_data->tm_year)*365) + ((time_data->tm_mon)*31) + (time_data->tm_mday);
			if ( !bi ) {
				bi = TRUE;
				if ( ( cstime <= ltime ) && ( ltime <= cetime ) )	Res = TRUE;
				else												Res = FALSE;
			}
			else {
				if ( ( cstime <= ltime ) && ( ltime <= cetime ) ) {
					if ( !and ) Res = TRUE;
				}
				else {
					if ( and ) Res = FALSE;
				}
			}
			and = FIND_OPTION4;
		}
		//
		if ( wl > 0 ) {
			if ( !bi ) {
				bi = TRUE;
				if ( JOB_FIND_WITH_WID( FIND_WID , wl , i , z2 ) )	Res = TRUE;
				else												Res = FALSE;
			}
			else {
				if ( JOB_FIND_WITH_WID( FIND_WID , wl , i , z2 ) ) {
					if ( !and ) Res = TRUE;
				}
				else {
					if ( and ) Res = FALSE;
				}
			}
		}
		//
		if ( z1 == 1 ) {
			if (!( !bi || ( bi && Res ) ) ) {
				FIND_INDEX[FIND_COUNT] = i;
				FIND_COUNT++;
			}
		}
		else {
			if ( !bi || ( bi && Res ) ) {
				FIND_INDEX[FIND_COUNT] = i;
				FIND_COUNT++;
			}
		}
	}
}
//
//
//
//




void JOB_Display_Data_Save_per_Each( BOOL always , int index ) { // 2003.06.09
	FILE *fpt;
	char filename[256];
	char folder[256];
	int loop;

	if ( index < 0 ) return;

	if ( JOB_DATA[ index ].DIRECTORY_TYPE != 1 ) return;
	//
//	strcpy( folder , JOB_Get_Directory2( index , &loop ) ); // 2017.03.13
	JOB_Get_Directory_2( index , &loop , folder ); // 2017.03.13
	//
	sprintf( filename , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , folder , STATUS_LOG_FILE );

	if ( !always ) {
		if ( ( fpt = fopen( filename , "r" ) ) != NULL ) {
			//=============================================================
			JOB_DATA[ index ].STATUS_SAVE = 1;
			//=============================================================
			fclose( fpt );
			return;
		}
	}
	if ( ( fpt = fopen( filename , "w" ) ) == NULL ) return;

	f_enc_printf( fpt , "JOB_NAME\t%s\n" , JOB_DATA[ index ].JOB_NAME );
	f_enc_printf( fpt , "LOT_RECIPE\t%s\n" , JOB_DATA[ index ].LOT_RECIPE );
	f_enc_printf( fpt , "IN_CASS\t%d\n" , JOB_DATA[ index ].IN_CASS );
	f_enc_printf( fpt , "OUT_CASS\t%d\n" , JOB_DATA[ index ].OUT_CASS );
	f_enc_printf( fpt , "START_SLOT\t%d\n" , JOB_DATA[ index ].START_SLOT );
	f_enc_printf( fpt , "END_SLOT\t%d\n" , JOB_DATA[ index ].END_SLOT );
	f_enc_printf( fpt , "MID_NAME\t%s\n" , JOB_DATA[ index ].MID_NAME );
	
	f_enc_printf( fpt , "RUN_TIME\t%s\n" , JOB_DATA[ index ].RUN_TIME );
	f_enc_printf( fpt , "END_TIME\t%s\n" , JOB_DATA[ index ].END_TIME );
	f_enc_printf( fpt , "ELAPSED_TIME\t%s\n" , JOB_DATA[ index ].ELAPSED_TIME );
	f_enc_printf( fpt , "START_TIME\t%s\n" , JOB_DATA[ index ].START_TIME );
	f_enc_printf( fpt , "METHOD_COUNT\t%d\n" , JOB_DATA[ index ].METHOD_COUNT );
//	f_enc_printf( fpt , "END_TYPE\t%d\n" , JOB_DATA[ index ].END_TYPE ); // 2003.07.30
	f_enc_printf( fpt , "USER\t%s\n" , JOB_DATA[ index ].USER );

//	f_enc_printf( fpt , "EDIT_TIME\t%d\n" , JOB_DATA[ index ].EDIT_TIME ); // 2003.07.30
	f_enc_printf( fpt , "EDIT_TYPE\t%d\n" , JOB_DATA[ index ].EDIT_TYPE ); // 2003.07.30
	
//	f_enc_printf( fpt , "COUNT\t%d\n" , JOB_DATA[ index ].COUNT ); // 2003.07.30

	f_enc_printf( fpt , "DIRECTORY_DATE\t%d\n" , JOB_DATA[ index ].DIRECTORY_DATE );
	//
	if ( loop > 0 ) f_enc_printf( fpt , "LOOP_COUNT\t%d\n" , loop );
	//=============================================================
	JOB_DATA[ index ].STATUS_SAVE = 1;
	//=============================================================

	fclose( fpt );
}



BOOL JOB_Display_Data_Save_per_Each_Check( int *status_save ) { // 2003.06.09
	int i;
	i = *status_save;
	if ( *status_save < 0 ) return -1;
	for ( ; *status_save < JOBCOUNT ; (*status_save)++ ) {
		if ( JOB_DATA[ *status_save ].STATUS_SAVE == 0 ) {
			JOB_Display_Data_Save_per_Each( FALSE , *status_save );
			return 0;
		}
	}
	*status_save = -1;
	if ( i == 0 ) return -1;
	return 1;
}