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
#include <Kutlfile.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "cimseqnc.h"
#include "iodefine.h"

#include "resource.h"
 
#include <CimSeqncKNFS.h>
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};
//------------------------------------------------------------------------------------------
HINSTANCE	BackBmpLib = NULL;
void		KGUI_Set_BACKGROUND( char *file ) { BackBmpLib = LoadLibrary( file ); }
HINSTANCE	KGUI_Get_BACKGROUND() { return BackBmpLib; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	MINI_SCREEN_MODE = FALSE;
BOOL	TITLE_MODE = FALSE;
//----------------------------------------------------------------------------
int READONLY_LEVEL = 0;
int READONLY_LEVEL_IO = -2;
char READONLY_LEVEL_IO_NAME[256];
//----------------------------------------------------------------------------
int		AUTO_DELETE_MONTH	= 0;
//---------------------------------------------------------------------------------------
char	HISTORY_DIRECTORY[128+1];
char	IDSTR_NAME[128+1]; // 2004.01.07
char	SELECT_FILENAME[256];
//--------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL    MONITOR_MODE = FALSE; // 2003.12.16
char    MONITOR_MODE_STRING[256]; // 2003.12.16
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  PROGRAM_GUI_SPAWNING = 0; // 2003.07.12
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void Set_Prm_MINISCREEN_MODE( BOOL data ) { MINI_SCREEN_MODE = data; }
BOOL Get_Prm_MINISCREEN_MODE() { return MINI_SCREEN_MODE; }
//--------------------------------------------------------------------------------------------------
void Set_Prm_TITLE_MODE( BOOL data ) { TITLE_MODE = data; }
BOOL Get_Prm_TITLE_MODE() { return TITLE_MODE; }
//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------
void Set_Prm_AUTO_DELETE_MONTH( int data ) { AUTO_DELETE_MONTH = data; }
int  Get_Prm_AUTO_DELETE_MONTH() { return AUTO_DELETE_MONTH; }
//--------------------------------------------------------------------------------------------------
void Set_Prm_MONITOR_MODE( int data ) { // 2003.12.16
	MONITOR_MODE = data;
}
BOOL Get_Prm_MONITOR_MODE() { // 2003.12.16
	return MONITOR_MODE;
}

void Set_Prm_MONITOR_FOLDER( char *data ) { // 2003.12.16
	strncpy( MONITOR_MODE_STRING , data , 255 );
}
char *Get_Prm_MONITOR_FOLDER() { // 2003.12.16
	return MONITOR_MODE_STRING;
}
//--------------------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	int i;
	if ( no == -1 ) {
		KGUI_STANDARD_Window_Kill_Signal();
	}
	if ( ( no > 0 ) && ( PROGRAM_GUI_SPAWNING == 0 ) ) { // 2003.07.12
		PROGRAM_GUI_SPAWNING = 1;
		for ( i = 0 ; i < 200 ; i++ ) {
			_sleep(100);
			if ( PROGRAM_GUI_SPAWNING == 2 ) break;
		}
	}
	KGUI_STANDARD_Screen_Nav_Info( no );
	if ( no > 0 ) {
		SendMessage( KGUI_STANDARD_Get_Screen_Handle() , WM_COMMAND , IDIGNORE , (LPARAM) NULL );
	}
}
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL FileData_ListView_Init( HWND hWnd , char *data ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	char Buffer[256];
	int l , c;
	//
	c = strlen( IDSTR_NAME ); // 2004.01.07
	sprintf( Buffer , "%s/%s*.log" , data , IDSTR_NAME );
	ListBox_ResetContent( hWnd );
	if ( !Get_Prm_MONITOR_MODE() ) {
		hFind = FindFirstFile( Buffer , &fd );
	}
	else { // 2003.12.16
		hFind = _nINF_KNFS_FindFirstFile( Buffer , &fd );
	}
	while ( (hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
//			strcpy( Buffer , fd.cFileName + 2 ); // 2004.01.07
			strcpy( Buffer , fd.cFileName + c ); // 2004.01.07
			l = strlen( Buffer );
			Buffer[l-4] = 0;
			ListBox_AddString( hWnd , Buffer );
		}
		if ( !Get_Prm_MONITOR_MODE() ) {
			bRet = FindNextFile( hFind , &fd );
		}
		else { // 2003.12.16
			bRet = _nINF_KNFS_FindNextFile( hFind , &fd );
		}
	}
	if ( !Get_Prm_MONITOR_MODE() ) {
		FindClose( hFind );
	}
	else { // 2003.12.16
		_nINF_KNFS_FindClose( hFind );
	}
	ListBox_SetCurSel( hWnd , -1 );
	return TRUE;
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void LView_Insert_Column( HWND hWnd , int k , char *Msg , char *Msg2 , char *Msg3 , char *Msg4 , char *Msg5 , char *Msg6 , char *Msg7 , char *Msg8 , char *Msg9) {
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= k;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
	lvi.cchTextMax	= 300;
	lvi.iImage		= 0;
	lvi.lParam		= 0;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return;
	ListView_SetItemText( hWnd , k , 1 , Msg2 );
	ListView_SetItemText( hWnd , k , 2 , Msg3 );
	ListView_SetItemText( hWnd , k , 3 , Msg4 );
	ListView_SetItemText( hWnd , k , 4 , Msg5 );
	ListView_SetItemText( hWnd , k , 5 , Msg6 );
	ListView_SetItemText( hWnd , k , 6 , Msg7 );
	ListView_SetItemText( hWnd , k , 7 , Msg8 );
	ListView_SetItemText( hWnd , k , 8 , Msg9 );
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void Operate_Data_Read( HWND hdlg ) {
	HFILE hFile;
	BOOL bres;
	int i , z1 , z2;
	char Buffer[256];
	char Buffer1[256];
	char Buffer2[256];
	char Buffer3[256];
	char Buffer4[256];
	char Buffer5[256];
	char Buffer6[256];
	char Buffer7[256];
	char Buffer8[256];
	char Buffer1_2[32];
	char localfilename[256];

	KWIN_Item_Disable( hdlg , IDRETRY );
	KWIN_Item_Disable( hdlg , IDABORT );
	KWIN_Item_Hide( hdlg , IDC_DATA_LIST );

	ListView_DeleteAllItems( GetDlgItem( hdlg , IDC_DATA_LIST ) );
	if ( strlen( SELECT_FILENAME ) <= 0 ) return;
	if ( !Get_Prm_MONITOR_MODE() ) {
		strcpy( localfilename , SELECT_FILENAME );
	}
	else { // 2003.12.16
		sprintf( localfilename , "%s/%s" , Get_Prm_MONITOR_FOLDER() , SELECT_FILENAME );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( localfilename );
		if ( !_nINF_KNFS_GetFile( SELECT_FILENAME , localfilename , FALSE ) ) {
			KWIN_Item_String_Display( hdlg , IDC_DATA_TOTAL , "File Error" );
			return;
		}
	}
	if ( ( hFile = _lopen( localfilename , OF_READ ) ) == -1 ) {
		KWIN_Item_String_Display( hdlg , IDC_DATA_TOTAL , "File Error" );
		return;
	}
	KWIN_Item_Disable( hdlg , IDC_LIST_FILE );
	KWIN_Item_Disable( hdlg , IDYES );
	KWIN_Item_String_Display( hdlg , IDC_DATA_TOTAL , "Reading...." );
	//
	i = 0;
	do {
		bres = H_Read_Line_From_File4( hFile , Buffer , 255 );
		if ( *Buffer != 0 ) {
			strcpy( Buffer1 , "" );
			strcpy( Buffer2 , "" );
			strcpy( Buffer3 , "" );
			strcpy( Buffer4 , "" );
			strcpy( Buffer5 , "" );
			strcpy( Buffer6 , "" );
			strcpy( Buffer7 , "" );
			strcpy( Buffer8 , "" );
			
			z1 = 0;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , z1 , 255 );
			if ( z1 != z2 ) {
				z1 = z2;
				z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
				if ( z1 != z2 ) {
					z1 = z2;
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer3 , z1 , 255 );
					if ( z1 != z2 ) {
						z1 = z2;
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer4 , z1 , 255 );
						if ( z1 != z2 ) {
							z1 = z2;
							z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer5 , z1 , 255 );
							if ( z1 != z2 ) {
								z1 = z2;
								z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer6 , z1 , 255 );
								if ( z1 != z2 ) {
									z1 = z2;
									z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer7 , z1 , 255 );
									if ( z1 != z2 ) {
										z1 = z2;
										z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer8 , z1 , 255 );
									}
								}
							}
						}
					}
				}
			}
			sprintf( Buffer1_2 , "%d" , i + 1 );
			LView_Insert_Column( GetDlgItem( hdlg , IDC_DATA_LIST ) , i , Buffer1_2 , Buffer1 , Buffer2 , Buffer3, Buffer4 , Buffer5, Buffer6, Buffer7, Buffer8 );
			i++;
		}
	}
	while ( bres );
	_lclose( hFile );
	wsprintf( Buffer1_2 , "%d item(s)" , i );
	KWIN_Item_String_Display( hdlg , IDC_DATA_TOTAL , Buffer1_2 );
	KWIN_Item_Show( hdlg , IDC_DATA_LIST );
	KWIN_Item_Enable( hdlg , IDC_LIST_FILE );
	KWIN_Item_Enable( hdlg , IDYES );
	KWIN_Item_Enable( hdlg , IDRETRY );
	KWIN_Item_Enable( hdlg , IDABORT );
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_RECIPELog_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT	ps;
	int i;
//	TCHAR       *szString[] = { "No" , "Time" , "Type" , "Contents" };
//	int         szSize[4]   = {  55  ,    155 ,    140 ,  2000 };
	TCHAR       *szString[] = { "No", "Date", "Edit Time", "RecipeType", "GroupID", "Recipe Name", "Control", "UserID"};
	int         szSize[8]   = { 30, 45, 95, 90, 100, 250, 75, 70 };
	int         szSize_M[8] = { 30, 45, 95, 90, 100, 250, 75, 70 };
	char Buffer[256];
	HBITMAP hBitmap;

	switch ( msg ) {
		case WM_INITDIALOG:
			SetWindowText( hdlg , "GUI_OperateLog" );
			//<<-----------------------------------------------------------
			KGUI_STANDARD_Set_Function_Size( hdlg );
			//>>-----------------------------------------------------------
			if ( Get_Prm_TITLE_MODE() ) i = 40;
			else                        i = 0;
			KWIN_Item_Resize( hdlg , IDYES              ,   5 ,  8 + i , 174 , 32 ); // reload file list
			KWIN_Item_Resize( hdlg , IDRETRY            , 588 ,  8 + i , 166 , 32 ); // read again
			KWIN_Item_Resize( hdlg , IDABORT            , 755 ,  8 + i , 166 , 32 ); // delete file
			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS2 , 180 ,  2 + i , 303 , 39 ); // box
			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS  , 189 , 14 + i ,  56 , 23 ); // string
			KWIN_Item_Resize( hdlg , IDC_DATA_TOTAL     , 246 , 13 + i , 233 , 24 ); // data count
			KWIN_Item_Resize( hdlg , IDC_LIST_FILE      ,   5 , 45 + i , 174 , 535 - i ); // file list
			KWIN_Item_Resize( hdlg , IDC_DATA_LIST      , 179 , 45 + i , 743 , 535 - i ); // data list(bin)
			//
			if ( Get_Prm_MINISCREEN_MODE() )
				KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_DATA_LIST ) , 8 , szString , szSize_M );
			else
				KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_DATA_LIST ) , 8 , szString , szSize );

			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_DATA_LIST ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_FILE ) , HISTORY_DIRECTORY );
			//>>-----------------------------------------------------------
//			ShowWindow( hdlg , SW_HIDE ); // 2003.07.12
			//----------------------------------
			PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
			//----------------------------------
			return TRUE;
			
		case WM_PAINT:
			//=============================================================================================
			// 2004.05.21
			//=============================================================================================
//			if ( !KGUI_STANDARD_Window_Signal_Mode() ) return TRUE; // 2005.02.28
			if ( !KGUI_STANDARD_Window_Signal_Mode() ) { // 2005.02.28
				BeginPaint( hdlg, &ps );
				EndPaint( hdlg , &ps );
				return TRUE;
			}
			//=============================================================================================
			BeginPaint( hdlg, &ps );
			if ( KGUI_Get_BACKGROUND() != NULL ) {
				hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND" );
				if ( hBitmap ) {
					KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
					DeleteObject( hBitmap );
				}
			}
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			if ( GET_WM_COMMAND_CMD( wParam , lParam ) == LBN_SELCHANGE ) {
				if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_LIST_FILE ) {
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_FILE ) );
					if ( i >= 0 ) {
						ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_FILE ) , i , Buffer );
//						sprintf( SELECT_FILENAME , "%s/OP%s.log" , HISTORY_DIRECTORY , Buffer ); // 2004.01.07
						sprintf( SELECT_FILENAME , "%s/%s%s.log" , HISTORY_DIRECTORY , IDSTR_NAME , Buffer ); // 2004.01.07
						Operate_Data_Read( hdlg );
					}
				}
				return TRUE;
			}
			switch (wParam) {
				case IDYES :
					FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_FILE ) , HISTORY_DIRECTORY );
// 2002.03.07
//					strcpy( SELECT_FILENAME , "" );
//					Operate_Data_Read( hdlg );
					return TRUE;

				case IDRETRY :
					Operate_Data_Read( hdlg );
					return TRUE;

				case IDIGNORE :
					if ( !Get_Prm_MONITOR_MODE() ) {
						if ( Get_Prm_AUTO_DELETE_MONTH() > 0 ) {
//							i = File_Delete_for_Time_Reached( HISTORY_DIRECTORY , "OP*.log" , Get_Prm_AUTO_DELETE_MONTH() ); // 2004.01.07
							sprintf( Buffer , "%s*.log" , IDSTR_NAME ); // 2004.01.07
							i = File_Delete_for_Time_Reached( HISTORY_DIRECTORY , Buffer , Get_Prm_AUTO_DELETE_MONTH() ); // 2004.01.07
							if ( i > 0 ) {
								FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_FILE ) , HISTORY_DIRECTORY );
							}
						}
					}
					return TRUE;

				case IDABORT :
					if ( strlen( SELECT_FILENAME ) > 0 ) {
						if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
							MessageBox( hdlg , "Delete Impossible(Ready only mode)", "Delete Impossible", MB_ICONHAND );
						}
						else {
							if ( IDOK == MessageBox( hdlg , "Do you want to delete Operate Log Data File ?", "Delete", MB_ICONQUESTION | MB_OKCANCEL ) ) {
								if ( !Get_Prm_MONITOR_MODE() ) {
									DeleteFile( SELECT_FILENAME );
								}
								else { // 2003.12.16
									sprintf( Buffer , "%s/%s" , Get_Prm_MONITOR_FOLDER() , SELECT_FILENAME );
									DeleteFile( Buffer );
									_nINF_KNFS_DeleteFile( SELECT_FILENAME );
								}
								FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_FILE ) , HISTORY_DIRECTORY );
								strcpy( SELECT_FILENAME , "" );
								Operate_Data_Read( hdlg );
							}
						}
					}
					return TRUE;

				case IDCANCEL :
					//EndDialog( hdlg , wParam );
					return TRUE;
			}
			break;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {
	HWND hWnd;
	//----------------------------------
	//REGIST_FILE_INFO( "Gui Operatelog 1.0" );	// Initial
	//REGIST_FILE_INFO( "Gui Operatelog 1.1(2001.09.06)" );	// Append Auto Delete
	//REGIST_FILE_INFO( "Gui Operate Log View 3.0(2002.01.01)" ); // CimSqCon Change Rebuild
	//REGIST_FILE_INFO( "Gui Operate Log View 3.1(2002.02.15)" ); // Button position/size fixed
	//REGIST_FILE_INFO( "Gui Operate Log View 3.2(2002.03.07)" ); // File Name Clear Bug Update when Operation change
	//REGIST_FILE_INFO( "Gui Operate Log View 3.3(2002.06.17)" ); // Window Instance Change
	//REGIST_FILE_INFO( "Gui Operate Log View 3.4(2002.10.03)" ); // Append Title Show
	//REGIST_FILE_INFO( "Gui Operate Log View 3.5(2002.11.08)" ); // Append Readonly mode
	//REGIST_FILE_INFO( "Gui Operate Log View 3.6(2003.07.12)" ); // Append Wait Receive Spawn Message
	//REGIST_FILE_INFO( "Gui Operate Log View 3.7(2003.12.15)" ); // Append MonitorMode
//	REGIST_FILE_INFO( "Gui Operate Log View 3.8(2004.01.07)" ); // Append Multi File format Check
//	REGIST_FILE_INFO( "Gui Operate Log View 3.81(2004.01.19)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Operate Log View 3.82(2004.08.10)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Operate Log View 3.83(2005.02.28)" ); // CPU OverHead Bug Update
	REGIST_FILE_INFO( "Gui Recipe History Log View 3.84(2006.02.16)" ); // Log Folder Auto Part Append
	//----------------------------------
	//----------------------------------
	while( PROGRAM_GUI_SPAWNING == 0 ) { _sleep(100); } // 2003.07.12
	//----------------------------------
	//----------------------------------
	//----------------------------------
	//----------------------------------
	InitCommonControls();
	//----------------------------------
	KGUI_Set_BACKGROUND( PROGRAM_PARAMETER_READ() );

	strcpy( SELECT_FILENAME , "" );
	hWnd = GET_MAIN_WINDOW();
	if ( Get_Prm_MINISCREEN_MODE() ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY_M , IDD_E_RECIPE_HISTORY_M ) , hWnd , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY_M , IDD_E_RECIPE_HISTORY_M ) , hWnd , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY_M , IDD_E_RECIPE_HISTORY_M ) , hWnd , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY_M , IDD_E_RECIPE_HISTORY_M ) , GET_MAIN_WINDOW() , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2002.06.17
	}
	else {
		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY , IDD_E_RECIPE_HISTORY ) , hWnd , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY , IDD_E_RECIPE_HISTORY ) , hWnd , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY , IDD_E_RECIPE_HISTORY ) , hWnd , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_RECIPE_HISTORY , IDD_E_RECIPE_HISTORY ) , GET_MAIN_WINDOW() , Console_Gui_For_RECIPELog_Proc , (LPARAM) NULL ); // 2002.06.17
	}
	SYSTEM_KILL_MYSELF();
	//----------------------------------
	PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
	//----------------------------------
	return( SYS_SUCCESS );
}

//
//
//
void Program_Enter_Code() {
	int	i;
	char	ELSE_STR[ 128 + 1 ];
	char	ELSE2_STR[ 128 + 1 ];
	char	MODE_STR[ 128 + 1 ];
	char	AUTO_STR[ 128 + 1 ];
	char	MON_STR[ 128 + 1 ];

	STR_SEPERATE_CHAR( PROGRAM_ARGUMENT_READ() , '|' , ELSE2_STR , ELSE_STR , 128 );
	STR_SEPERATE_CHAR( ELSE2_STR , ':' , HISTORY_DIRECTORY , IDSTR_NAME , 128 ); // 2004.01.07
	//============================================================================
	// 2006.02.16
	//============================================================================
	if ( strcmp( HISTORY_DIRECTORY , "$" ) == 0 ) { // 2006.02.16
		_IO_GET_OPER_FOLDER( HISTORY_DIRECTORY , 128 ); // 2006.02.16
	}
	//==========================================================
	if ( strlen( IDSTR_NAME ) <= 0 ) strcpy( IDSTR_NAME , "RH" ); // 2004.01.07
	//
	STR_SEPERATE_CHAR( ELSE_STR , '|' , MON_STR , ELSE2_STR , 128 );
	STR_SEPERATE_CHAR( MON_STR , ':' , MODE_STR , ELSE_STR , 128 ); // 2003.12.28
	//
	Set_Prm_READONLY_LEVEL_IO_NAME( ELSE_STR ); // 2003.12.28
	//
	STR_SEPERATE_CHAR( ELSE2_STR , '|' , AUTO_STR , MON_STR , 128 );

	Set_Prm_MINISCREEN_MODE( FALSE );
	Set_Prm_TITLE_MODE( FALSE );
	Set_Prm_READONLY_LEVEL( 0 );

	for ( i = 0 ; i < (signed) strlen( MODE_STR ) ; i++ ) {
		if ( tolower( MODE_STR[i] ) == 'm' ) Set_Prm_MINISCREEN_MODE( TRUE );
		if ( tolower( MODE_STR[i] ) == 't' ) Set_Prm_TITLE_MODE( TRUE );
		if ( tolower( MODE_STR[i] ) == '0' ) Set_Prm_READONLY_LEVEL( 0 );
		if ( tolower( MODE_STR[i] ) == '1' ) Set_Prm_READONLY_LEVEL( 1 );
		if ( tolower( MODE_STR[i] ) == '2' ) Set_Prm_READONLY_LEVEL( 2 );
		if ( tolower( MODE_STR[i] ) == '3' ) Set_Prm_READONLY_LEVEL( 3 );
		if ( tolower( MODE_STR[i] ) == '4' ) Set_Prm_READONLY_LEVEL( 4 );
		if ( tolower( MODE_STR[i] ) == '5' ) Set_Prm_READONLY_LEVEL( 5 );
		if ( tolower( MODE_STR[i] ) == '6' ) Set_Prm_READONLY_LEVEL( 6 );
		if ( tolower( MODE_STR[i] ) == '7' ) Set_Prm_READONLY_LEVEL( 7 );
		if ( tolower( MODE_STR[i] ) == '8' ) Set_Prm_READONLY_LEVEL( 8 );
		if ( tolower( MODE_STR[i] ) == '9' ) Set_Prm_READONLY_LEVEL( 9 );
	}
	//
	Set_Prm_AUTO_DELETE_MONTH( atoi( AUTO_STR ) );
	//
	if ( strlen( MON_STR ) > 0 ) { // 2003.12.16
		Set_Prm_MONITOR_MODE( TRUE );
		STR_SEPERATE_CHAR( MON_STR , ':' , AUTO_STR , MODE_STR , 128 );
		sprintf( ELSE_STR , "$Remote$\\[%s][%s]" , AUTO_STR , MODE_STR );
		Set_Prm_MONITOR_FOLDER( ELSE_STR );
		_nINF_KNFS_Prepare_for_ConnectionName( PROGRAM_FUNCTION_READ() );
		if ( _nINF_KNFS_CONNECTION_FOR_CLIENT( AUTO_STR , atoi(MODE_STR) ) != 0 ) {
			_IO_CIM_PRINTF( "Network KNFS Initialization Error\n" );
			return;
		}
	}
	else {
		Set_Prm_MONITOR_MODE( FALSE );
	}
	//
}
//-------------------------------------------------------------------------
