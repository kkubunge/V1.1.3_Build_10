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

#include "iodefine.h"
#include "appprm.h"

#include "resource.h"

#include <CimSeqncKNFS.h>
//=========================================================================================
//=========================================================================================
#define		MAX_ENUM_DATA	100
#define		MAX_ENUM_DEPTH	25
//=========================================================================================
int	 RUN_CHAMBER;
//---------------------------------------------------------------------------------------
int  Enum_Count = 0;
char Enum_Index[ MAX_ENUM_DATA ][32];
char Enum_Disp[ MAX_ENUM_DATA ][32];
int  Enum_Type[ MAX_ENUM_DATA ];
int  Enum_Num[ MAX_ENUM_DATA ];
char Enum_Name[ MAX_ENUM_DATA ][ MAX_ENUM_DEPTH ][32];
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void GET_ENUM_DATA( char *name_return , double data_return , int mode , char *imsi , char *imsi2 ) {
	int i , k;
	if ( mode == 3 ) {
		for ( i = 0 ; i < Enum_Count ; i++ ) {
			if ( STRCMP_L( name_return , Enum_Index[ i ] ) ) {
				if ( strlen( Enum_Disp[ i ] ) <= 0 )
					strcpy( imsi , name_return );
				else
					strcpy( imsi , Enum_Disp[ i ] );
				return;
			}
		}
	}
	else {
		for ( i = 0 ; i < Enum_Count ; i++ ) {
			if ( STRCMP_L( name_return , Enum_Index[ i ] ) ) {
				if ( Enum_Type[ i ] == 0 ) {
					k = (int) data_return;
					if ( k >= Enum_Num[ i ] ) {
						break;
					}
					else {
						strcpy( imsi2 , Enum_Name[ i ][ k ] );
					}
				}
				else {
					STRING_CONV_With_DEV( data_return , Enum_Num[ i ] , imsi2 );
				}
				if ( strlen( Enum_Disp[ i ] ) <= 0 )
					strcpy( imsi , name_return );
				else
					strcpy( imsi , Enum_Disp[ i ] );
				return;
			}
		}
	}
	strcpy( imsi , name_return );
	switch ( mode ) {
	case 0 :	sprintf( imsi2 , "%d"  , (int) data_return );	break;
	case 1 :	sprintf( imsi2 , "%d"  , (int) data_return );	break;
	case 2 :	sprintf( imsi2 , "%lf" ,       data_return );	break;
	}
}
//---------------------------------------------------------------------------------------
void READ_ENUM_DATA( int ch ) {
	FILE *fpt;
	char Filename[ 256+1 ];
	char Buffer[ 1024+1 ];
	char chr_return[ 1024+1 ];
	int  i , ReadCnt , Line;
	BOOL FileEnd = TRUE;

	Enum_Count = 0;
	sprintf( Filename , "%s/RcpEnum_Pm%d.cfg" , Get_Prm_Scheduler_Path() , ch + 1 );
//	if ( ( fpt = fopen( Filename , "r+t" ) ) == NULL ) return; // 2005.03.22
	if ( ( fpt = fopen( Filename , "r" ) ) == NULL ) return; // 2005.03.22
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = Get_Line_String_From_File( fpt , Buffer , &ReadCnt );
		if ( ( ReadCnt > 0 ) && ( Enum_Count < MAX_ENUM_DATA ) ) {
			Enum_Num[ Enum_Count ] = 0;
			strcpy( Enum_Index[ Enum_Count ] , "" );
			strcpy( Enum_Disp[ Enum_Count ] , "" );
			for ( i = 0 ; i < ReadCnt ; i++ ) {
				if ( !Get_Data_From_String( Buffer , chr_return , i ) ) return;
				if ( i == 0 ) {
					strncpy( Enum_Index[ Enum_Count ] , chr_return , 31 );
				}
				else if ( i == 1 ) {
					STRING_CHECK( chr_return , 30 , Enum_Disp[ Enum_Count ] );
					//strncpy( Enum_Disp[ Enum_Count ] , chr_return , 31 );
				}
				else if ( i == 2 ) {
					if ( tolower( chr_return[0] ) == 'd' ) {
						Enum_Type[ Enum_Count ] = 0;
					}
					else {
						Enum_Type[ Enum_Count ] = 1;
					}
				}
				else {
					if ( Enum_Type[ Enum_Count ] == 1 ) {
						if ( i == 3 ) {
							Enum_Num[ Enum_Count ] = atoi( chr_return );
						}
					}
					else {
						strncpy( Enum_Name[ Enum_Count ][ i - 3 ] , chr_return , 31 );
						Enum_Num[ Enum_Count ] = i - 2;
					}
				}
			}
			Enum_Count++;
		}
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int log_pointer = 0;
//------------------------------------------------------------------------------------------
BOOL LogView_Open_Recipe_Insert( HWND hWnd , char *Msg , char *Msg2 , char *Msg3 ) {
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= log_pointer;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
	lvi.cchTextMax	= 100;
	lvi.iImage		= log_pointer;
	lvi.lParam		= 0;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	ListView_SetItemText( hWnd , log_pointer , 1 , Msg2 );
	ListView_SetItemText( hWnd , log_pointer , 2 , Msg3 );
	log_pointer++;
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//
BOOL LogView_Open_Recipe_Init( HWND hwndListView ) {
	int         i;
	LV_COLUMN   lvColumn;
	TCHAR       *szString[] = { ""   ,  "" , ""  };
	int         szSize[3]  = {  130  , 200 ,  200 };
	//
	log_pointer = 0;
	ListView_DeleteAllItems( hwndListView );
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for ( i = 0 ; i < 3 ; i++ ) {
		lvColumn.cx = szSize[i];
		lvColumn.pszText = szString[i];
		ListView_InsertColumn( hwndListView , i , &lvColumn );
	}	
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
//=========================================================================================
//=========================================================================================
BOOL ProcessOpen_and_Display( char *Filename , HWND hdlg ) {
	HFILE hFile;
	BOOL StepEndFind;
	UINT32 i,kill,Step_Count;
	char Imsi[256];
	char Imsi2[256];

	char  cc_return;
	int   ii_return;
	float ff_return;
	char  str_return[1025];
	char  name_return[1025];
	char  type_return;

	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) return FALSE;

	LogView_Open_Recipe_Insert( hdlg , "RECIPE" , Filename , "" );
	LogView_Open_Recipe_Insert( hdlg , "HEADER_BEGIN" , "" , "" );

	// INDICATOR
	if ( !read_form_name_check( hFile , "" ) ) {
		_lclose( hFile );
		LogView_Open_Recipe_Insert( hdlg , "ERROR" , "File ID Error" , "" );
		return TRUE;
	}
	if ( !read_form_name_check( hFile , "" ) ) {
		_lclose( hFile );
		LogView_Open_Recipe_Insert( hdlg , "ERROR" , "Recipe ID Error" , "" );
		return TRUE;
	}
	// HEADER
	if ( !read_form_name_check( hFile , "__&&HEADER_BEGIN&&__" ) ) {
		_lclose( hFile );
		LogView_Open_Recipe_Insert( hdlg , "ERROR" , "Header Begin Error" , "" );
		return TRUE;
	}
	if ( !read_form( hFile , "NumberOfSteps"        , INT_FORMAT    , &cc_return , &Step_Count , &ff_return , str_return ) ) {
		_lclose( hFile );
		LogView_Open_Recipe_Insert( hdlg , "ERROR" , "Header Data Error" , "" );
		return TRUE;
	}
	if ( !read_form_name_check( hFile , "__&&HEADER_END&&__" ) ) {
		_lclose( hFile );
		LogView_Open_Recipe_Insert( hdlg , "ERROR" , "Header End Error" , "" );
		return TRUE;
	}
	//
	sprintf( Imsi , "%d" , Step_Count );
	LogView_Open_Recipe_Insert( hdlg , "" , "STEP_COUNT" , Imsi );
	LogView_Open_Recipe_Insert( hdlg , "HEADER_END" , "" , "" );
	LogView_Open_Recipe_Insert( hdlg , "-------------" , "-------------" , "-------------" );
	LogView_Open_Recipe_Insert( hdlg , "STEP_BEGIN" , "0" , "" );

	if ( !read_form( hFile , "__&&STEP0_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		_lclose( hFile );
		LogView_Open_Recipe_Insert( hdlg , "ERROR" , "Step 0 Header Error" , "" );
		return TRUE;
	}

	StepEndFind = FALSE;
	kill = 0;
	while( !StepEndFind ) {
		kill++;
		if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) {
			_lclose( hFile );
			LogView_Open_Recipe_Insert( hdlg , "ERROR" , "Step 0 Data Read Error" , "" );
			return TRUE;
		}
		switch ( type_return ) {
			case NAME_FORMAT :	if ( strcmp( name_return , "__&&STEP0_END&&__" ) == 0 ) StepEndFind = TRUE;
								break;
			case CHAR_FORMAT :
								GET_ENUM_DATA( name_return , (double) cc_return , 0 , Imsi , Imsi2 );
								LogView_Open_Recipe_Insert( hdlg , "" , Imsi , Imsi2 );
								break;
			case INT_FORMAT	 :
								GET_ENUM_DATA( name_return , (double) ii_return , 1 , Imsi , Imsi2 );
								LogView_Open_Recipe_Insert( hdlg , "" , Imsi , Imsi2 );
								break;
			case FLOAT_FORMAT:
								GET_ENUM_DATA( name_return , (double) ff_return , 2 , Imsi , Imsi2 );
								LogView_Open_Recipe_Insert( hdlg , "" , Imsi , Imsi2 );
								break;
			case STRING_FORMAT:
								GET_ENUM_DATA( name_return , 0 , 3 , Imsi , Imsi2 );
								LogView_Open_Recipe_Insert( hdlg , "" , Imsi , str_return );
								break;
		}
		if ( kill >= 1000 )	{
			_lclose( hFile );
			LogView_Open_Recipe_Insert( hdlg , "ERROR" , "Step 0 Data Error" , "" );
			return TRUE;
		}
	}
	LogView_Open_Recipe_Insert( hdlg , "STEP_END" , "0" , "" );
	LogView_Open_Recipe_Insert( hdlg , "-------------" , "-------------" , "-------------" );
	// STEP
	for ( i = 0 ; ( i < Step_Count ) && ( i < 199 ) ; i++ ) {
		if ( !read_form( hFile , "__&&STEP_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
			_lclose( hFile );
			sprintf( Imsi , "Step %d Header Error" , i + 1 );
			LogView_Open_Recipe_Insert( hdlg , "ERROR" , Imsi , "" );
			return TRUE;
		}
		sprintf( Imsi , "%d" , i + 1 );
		LogView_Open_Recipe_Insert( hdlg , "STEP_BEGIN" , Imsi , "" );
		// StepRead
		StepEndFind = FALSE;
		kill = 0;
		while( !StepEndFind ) {
			if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) {
				_lclose( hFile );
				sprintf( Imsi , "Step %d Data Read Error" , i + 1 );
				LogView_Open_Recipe_Insert( hdlg , "ERROR" , Imsi , "" );
				return FALSE;
			}
			switch (type_return) {
				case NAME_FORMAT :	if ( strcmp( name_return , "__&&STEP_END&&__" ) == 0 ) StepEndFind = TRUE;
									break;
				case CHAR_FORMAT :
									GET_ENUM_DATA( name_return , (double) cc_return , 0 , Imsi , Imsi2 );
									LogView_Open_Recipe_Insert( hdlg , "" , Imsi , Imsi2 );
									break;
				case INT_FORMAT	 :
									GET_ENUM_DATA( name_return , (double) ii_return , 1 , Imsi , Imsi2 );
									LogView_Open_Recipe_Insert( hdlg , "" , Imsi , Imsi2 );
									break;
				case FLOAT_FORMAT:
									GET_ENUM_DATA( name_return , (double) ff_return , 2 , Imsi , Imsi2 );
									LogView_Open_Recipe_Insert( hdlg , "" , Imsi , Imsi2 );
									break;
				case STRING_FORMAT:
									GET_ENUM_DATA( name_return , 0 , 3 , Imsi , Imsi2 );
									LogView_Open_Recipe_Insert( hdlg , "" , Imsi , str_return );
									break;
			}
			if ( kill >= 1000 )	{
				_lclose( hFile );
				sprintf( Imsi , "Step %d Data Error" , i + 1 );
				LogView_Open_Recipe_Insert( hdlg , "ERROR" , Imsi , "" );
				return TRUE;
			}
		}
		sprintf( Imsi , "%d" , i + 1 );
		LogView_Open_Recipe_Insert( hdlg , "STEP_END" , Imsi , "" );
	}
	_lclose( hFile );
	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

BOOL APIENTRY LogView_Open_Recipe_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	char Buffer[256];
	char localFilename[256];

	switch ( msg ) {
		case WM_INITDIALOG:
			MoveCenterWindow( hdlg );
			LogView_Open_Recipe_Init( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
			if ( !Get_Prm_MONITOR_MODE() ) {
				sprintf( localFilename , "%s" , (char *) lParam );
			}
			else {
				sprintf( localFilename , "%s/%s" , Get_Prm_MONITOR_FOLDER() , (char *) lParam );
				_nINF_KNFS_PrepareLocalFolder_for_Filename( localFilename );
				if ( !_nINF_KNFS_GetFile( (char *) lParam , localFilename , FALSE ) ) {
					sprintf( Buffer , "[%s] File Not Found" , (char *) lParam );
					MessageBox( hdlg , Buffer , "Open Error", MB_ICONHAND | MB_OK );
					EndDialog( hdlg , wParam );
					return FALSE;
				}
			}
			if ( !ProcessOpen_and_Display( localFilename , GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) ) ) {
				sprintf( Buffer , "[%s] File Not Found" , (char *) lParam );
				MessageBox( hdlg , Buffer , "Open Error", MB_ICONHAND | MB_OK );
				EndDialog( hdlg , wParam );
				return FALSE;
			}
			return TRUE;
			
		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch (wParam) {
				case IDCANCEL :
					EndDialog( hdlg , wParam );
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
//
//
void LogView_Open_Recipe( HWND hWnd , int ch , char *filename ) {
	//-------------------------------------------------------------------------------------------------------
	RECIPE_LOG_HANDLING( "VIEW" , "%s RECIPE (%s) DATA VIEW\n" , Get_Prm_PROCESS_NAME( ch ) , filename ); // 2004.01.08
	//-------------------------------------------------------------------------------------------------------
	READ_ENUM_DATA( ch );
	GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Recipe_Proc , (LPARAM) (char *) filename ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Recipe_Proc , (LPARAM) (char *) filename ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Recipe_Proc , (LPARAM) (char *) filename ); // 2004.01.19
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Recipe_Proc , (LPARAM) (char *) filename ); // 2002.06.17
}
//
