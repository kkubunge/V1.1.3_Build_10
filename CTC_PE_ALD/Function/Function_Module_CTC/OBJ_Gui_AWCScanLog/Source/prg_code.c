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
int Global_Pos_X[MAX_BUTTON_COUNT]; // 2005.04.20
int Global_Pos_Y[MAX_BUTTON_COUNT]; // 2005.04.20
int Global_Pos_XS[MAX_BUTTON_COUNT]; // 2005.04.20
int Global_Pos_YS[MAX_BUTTON_COUNT]; // 2005.04.20
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
char    SELECT_FILENAME_BEFORE[256]; 
//--------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL    MONITOR_MODE = FALSE; // 2003.12.16
char    MONITOR_MODE_STRING[256]; // 2003.12.16
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  PROGRAM_GUI_SPAWNING = 0; // 2003.07.12
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void Global_Pos_Set( int index , int x , int y , int xs , int ys ) {
	Global_Pos_X[index] = x;
	Global_Pos_Y[index] = y;
	Global_Pos_XS[index] = xs;
	Global_Pos_YS[index] = ys;
}
//--------------------------------------------------------------------------------------------------
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
// 2006.11.14 Lee Jai Min
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
/*
void LView_Insert_Column( HWND hWnd , int k , char *Msg , char *Msg2 , char *Msg3 , char *Msg4 ) {
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
}
*/
//void LView_Insert_Column( HWND hWnd , char *Msg , char *Msg2 , char *Msg3 , char *Msg4, char *Msg5, char *Msg6, char *Msg7, char *Msg8, char *Msg9, char *Msg10, 
//							char *Msg11, char *Msg12, char *Msg13, char *Msg14, char *Msg15, char *Msg16, char *Msg17, char *Msg18, char *Msg19, char *Msg20,
//							char *Msg21, char *Msg22, char *Msg23, int id ) {
void LView_Insert_Column( HWND hWnd , char *Msg , char *Msg2 , char *Msg3 , char *Msg4, char *Msg5, char *Msg6, char *Msg7, char *Msg8, char *Msg9, char *Msg10, 
							char *Msg11, char *Msg12, char *Msg13, int id ) {
	
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvi.iItem		= 0;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
	lvi.cchTextMax	= 100;
	lvi.iImage		= 0;
	lvi.lParam		= id;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return;
	ListView_SetItemText( hWnd , 0 , 1 , Msg2 );
	ListView_SetItemText( hWnd , 0 , 2 , Msg3 );
	ListView_SetItemText( hWnd , 0 , 3 , Msg4 );
	ListView_SetItemText( hWnd , 0 , 4 , Msg5 );
	ListView_SetItemText( hWnd , 0 , 5 , Msg6 );
	ListView_SetItemText( hWnd , 0 , 6 , Msg7 );
	ListView_SetItemText( hWnd , 0 , 7 , Msg8 );
	ListView_SetItemText( hWnd , 0 , 8 , Msg9 );
	ListView_SetItemText( hWnd , 0 , 9 , Msg10 );
	ListView_SetItemText( hWnd , 0 , 10, Msg11 );
	ListView_SetItemText( hWnd , 0 , 11 , Msg12 );
	ListView_SetItemText( hWnd , 0 , 12 , Msg13 );
	/*
	ListView_SetItemText( hWnd , 0 , 13 , Msg14 );
	ListView_SetItemText( hWnd , 0 , 14 , Msg15 );
	ListView_SetItemText( hWnd , 0 , 15 , Msg16 );
	ListView_SetItemText( hWnd , 0 , 16 , Msg17 );
	ListView_SetItemText( hWnd , 0 , 17 , Msg18 );
	ListView_SetItemText( hWnd , 0 , 18 , Msg19 );
	ListView_SetItemText( hWnd , 0 , 19 , Msg20 );
	ListView_SetItemText( hWnd , 0 , 20 , Msg21 );
	ListView_SetItemText( hWnd , 0 , 21 , Msg22 );
	ListView_SetItemText( hWnd , 0 , 22 , Msg23 );
	*/
}
// 2006.11.14 Lee Jai Min
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

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
	char Buffer9[256];
	char Buffer10[256];
	char Buffer11[256];
	char Buffer12[256];
	char Buffer13[256];
	/*
	char Buffer14[256];
	char Buffer15[256];
	char Buffer16[256];
	char Buffer17[256];
	char Buffer18[256];
	char Buffer19[256];
	char Buffer20[256];
	char Buffer21[256];
	char Buffer22[256];
	char Buffer23[256];
	*/

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
			strcpy( Buffer9 , "" );
			strcpy( Buffer10 , "" );
			strcpy( Buffer11 , "" );
			strcpy( Buffer12 , "" );
			strcpy( Buffer13 , "" );
			/*
			strcpy( Buffer14 , "" );
			strcpy( Buffer15 , "" );
			strcpy( Buffer16 , "" );
			strcpy( Buffer17 , "" );
			strcpy( Buffer18 , "" );
			strcpy( Buffer19 , "" );
			strcpy( Buffer20 , "" );
			strcpy( Buffer21 , "" );
			strcpy( Buffer22 , "" );
			strcpy( Buffer23 , "" );
			*/

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
										if ( z1 != z2 ) {
											z1 = z2;
											z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer9 , z1 , 255 );		
											if ( z1 != z2 ) {
												z1 = z2;
												z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer10 , z1 , 255 );
												if ( z1 != z2 ) {
													z1 = z2;
													z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer11 , z1 , 255 );															
													if ( z1 != z2 ) {
														z1 = z2;
														z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer12 , z1 , 255 );																	
														if ( z1 != z2 ) {
															z1 = z2;
															z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer13 , z1 , 255 );	
															/*
															if ( z1 != z2 ) {
																z1 = z2;
																z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer14 , z1 , 255 );	
																if ( z1 != z2 ) {
																	z1 = z2;
																	z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer15 , z1 , 255 );	
																	if ( z1 != z2 ) {
																		z1 = z2;
																		z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer16 , z1 , 255 );	
																		if ( z1 != z2 ) {
																			z1 = z2;
																			z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer17 , z1 , 255 );
																			if ( z1 != z2 ) {
																				z1 = z2;
																				z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer18 , z1 , 255 );
																				if ( z1 != z2 ) {
																					z1 = z2;
																					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer19 , z1 , 255 );
																					if ( z1 != z2 ) {
																						z1 = z2;
																						z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer20 , z1 , 255 );
																						if ( z1 != z2 ) {
																							z1 = z2;
																							z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer21 , z1 , 255 );
																							if ( z1 != z2 ) {
																								z1 = z2;
																								z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer22 , z1 , 255 );
																									if ( z1 != z2 ) {
																									z1 = z2;
																									z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer23 , z1 , 255 );										
																								}
																							}
																						}
																					}
																				}	
																			}
																		}
																	}
																}
															}*/
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			sprintf( Buffer1_2 , "%d" , i + 1 );
			//=====================================================================================================
			// 2006.11.14
			//=====================================================================================================
			// 2006.11.14 Lee Jai Min
//			LView_Insert_Column( GetDlgItem( hdlg , IDC_DATA_LIST ) , i , Buffer1_2 , Buffer1 , Buffer2 , Buffer3 );
//			i++;
			//=====================================================================================================
			//LView_Insert_Column( GetDlgItem( hdlg , IDC_DATA_LIST ) , Buffer1_2 , Buffer1 , Buffer2 , Buffer3 , Buffer4, Buffer5, Buffer6, Buffer7, Buffer8, Buffer9, Buffer10, 
			//	Buffer11, Buffer12, Buffer13, Buffer14, Buffer15, Buffer16, Buffer17, Buffer18, Buffer19, Buffer20, Buffer21, Buffer22, Buffer23, i - 1 );
			
			LView_Insert_Column( GetDlgItem( hdlg , IDC_DATA_LIST ) , Buffer1_2 , Buffer1 , Buffer2 , Buffer3 , Buffer4, Buffer5, Buffer6, Buffer7, Buffer8, Buffer9, Buffer10, Buffer11, Buffer12, i - 1 );

			//LView_Insert_Column( GetDlgItem( hdlg , IDC_DATA_LIST ) , i , Buffer1_2 , Buffer1 , Buffer2 , Buffer3 , Buffer4, Buffer5, Buffer6, Buffer7, Buffer8, Buffer9, Buffer10);
			i++;
			//=====================================================================================================
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
// 2006.11.14  Lee Jai Min
#define		ASC			1
#define		DESC		2
//
int nSortDir[10];
HWND hList = NULL;
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int CALLBACK pfnCompare( LPARAM lParam1 , LPARAM lParam2 , LPARAM lParamSort ) {
	static LV_FINDINFO fi;
	static char szBuf1[30] , szBuf2[30];
	static int nItem1 , nItem2 , Res;

	if ( lParamSort == 1 ) {
		//입력 라인 순서대로 정렬
		if ( nSortDir[lParamSort] == ASC )
			return ( lParam1 < lParam2 ? -1 : lParam1 > lParam2 ? 1 : 0 );
		else
			return ( lParam1 > lParam2 ? -1 : lParam1 < lParam2 ? 1 : 0 );

		//문자열 비교 정렬
		//fi.flags = LVFI_PARAM;
		//fi.lParam = lParam1;
		//nItem1 = ListView_FindItem( hList , -1 , &fi );
		//fi.lParam = lParam2;
		//nItem2 = ListView_FindItem( hList , -1 , &fi );
		//ListView_GetItemText( hList , nItem1 , lParamSort , szBuf1 , sizeof( szBuf1 ) );
		//ListView_GetItemText( hList , nItem2 , lParamSort , szBuf2 , sizeof( szBuf2 ) );

		//Res = strcmp( szBuf1 , szBuf2 );
		//if ( Res == 0 ) {
		//	ListView_GetItemText( hList , nItem1 , 0 , szBuf1 , sizeof( szBuf1 ) );
		//	ListView_GetItemText( hList , nItem2 , 0 , szBuf2 , sizeof( szBuf2 ) );
		//	if ( atoi( szBuf1 ) < atoi( szBuf2 ) ) Res = 1;
		//	else Res = 0;
		//}
		//if ( nSortDir[lParamSort] == ASC )
		//	return ( Res );
		//else
		//	return ( Res * -1 );
	}
	else {
			return 0;
	}
}
// 2006.11.14  Lee Jai Min
//--------------------------------------------------------------------------------------------------


// 2013.11.07 Compare Previous Data by Hyuk
void CompareView(HWND hdlg){
	
	HFILE hFile;
	BOOL bres;
	int i , z1 , z2, n;
	char Buffer_imsi[31][13][1024];
	char localfilename_before[256];
	FILE *fp = NULL;

	int k=0, j=0;

	SYSTEMTIME st;

	char Buffer[256];
	char Buffer1[256];
	char Buffer2[256];
	char Buffer3[256];
	char Buffer4[256];
	char Buffer5[256];
	char Buffer6[256];
	char Buffer7[256];
	char Buffer8[256];
	char Buffer9[256];
	char Buffer10[256];
	char Buffer11[256];
	char Buffer12[256];
	char Buffer13[256];

	char Buffer1_2[32];
	char localfilename[256];

	double dbLS = 0.0f;
	double dbLE = 0.0f;
	double dbRS = 0.0f;
	double dbRE = 0.0f;
	double dbLS2 = 0.0f;
	double dbLE2 = 0.0f;
	double dbRS2 = 0.0f;
	double dbRE2 = 0.0f;

	char LS[256];
	char LE[256];
	char RS[256];
	char RE[256];
	char LS2[256];
	char LE2[256];
	char RS2[256];
	char RE2[256];

	GetLocalTime( &st );

	if ( strlen( SELECT_FILENAME_BEFORE ) <= 0 ) return;
	if ( !Get_Prm_MONITOR_MODE() ) {
		strcpy( localfilename_before , SELECT_FILENAME_BEFORE );
	}
	
	hFile = _lopen( localfilename_before , OF_READ );  
	
	KWIN_Item_Disable( hdlg , IDC_LIST_FILE );
	KWIN_Item_Disable( hdlg , IDYES );
	KWIN_Item_String_Display( hdlg , IDC_DATA_TOTAL , "Reading...." );

	i = 0;
	n = 0;

	/*
	for(k = 0; k <= 60; k++)
	{
		for(j = 0; j <= 13; j++)
		{
			strcpy( Buffer_imsi[k][j] , "" );
		}
	}
*/
	
	do {
		memset(Buffer, 0, sizeof(Buffer));
		bres = H_Read_Line_From_File4( hFile , Buffer , 255 );
		
		if ( *Buffer != 0 ) {

			strcpy( Buffer_imsi[n][0] , "" );
			strcpy( Buffer_imsi[n][1] , "" );
			strcpy( Buffer_imsi[n][2] , "" );
			strcpy( Buffer_imsi[n][3] , "" );
			strcpy( Buffer_imsi[n][4] , "" );
			strcpy( Buffer_imsi[n][5] , "" );
			strcpy( Buffer_imsi[n][6] , "" );
			strcpy( Buffer_imsi[n][7] , "" );
			strcpy( Buffer_imsi[n][8] , "" );
			strcpy( Buffer_imsi[n][9] , "" );
			strcpy( Buffer_imsi[n][10] , "" );
			strcpy( Buffer_imsi[n][11] , "" );
			strcpy( Buffer_imsi[n][12] , "" );
			strcpy( Buffer_imsi[n][13] , "" );
			
			z1 = 0;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][0] , z1 , 255 );
			if ( z1 != z2 ) {
				z1 = z2;
				z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][1] , z1 , 255 );
				if ( z1 != z2 ) {
					z1 = z2;
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][2] , z1 , 255 );
					if ( z1 != z2 ) {
						z1 = z2;
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][3] , z1 , 255 );
						if ( z1 != z2 ) {
							z1 = z2;
							z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][4] , z1 , 255 );
							if ( z1 != z2 ) {
								z1 = z2;
								z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][5] , z1 , 255 );
								if ( z1 != z2 ) {
									z1 = z2;
									z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][6] , z1 , 255 );
									if ( z1 != z2 ) {
										z1 = z2;
										z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][7] , z1 , 255 );
										if(  z1 != z2 ){
											z1 = z2;
											z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][8] , z1 , 255 );
											if(  z1 != z2 ){
												z1 = z2;
												z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][9] , z1 , 255 );
												if(  z1 != z2 ){
													z1 = z2;
													z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][10] , z1 , 255 );
													if(  z1 != z2 ){
														z1 = z2;
														z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][11] , z1 , 255 );														
														if(  z1 != z2 ){
															z1 = z2;
															z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][12] , z1 , 255 );
															if(  z1 != z2 ){
																z1 = z2;
																z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer, 9 , Buffer_imsi[n][13] , z1 , 255 );
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

	n = n + 1;

	if(n == 30) break;	//Bluetain 1set All station : 30Line Max Compare

	}while ( bres );
	_lclose( hFile );

	printf("Select File Name : %s\n", SELECT_FILENAME);

	// Current File View
	ListView_DeleteAllItems( GetDlgItem( hdlg , IDC_DATA_LIST ) );
	if ( strlen( SELECT_FILENAME ) <= 0 ) return;
	if ( !Get_Prm_MONITOR_MODE() ) {
		strcpy( localfilename , SELECT_FILENAME );
	}
	else { 
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
			strcpy( Buffer9 , "" );
			strcpy( Buffer10 , "" );
			strcpy( Buffer11 , "" );
			strcpy( Buffer12 , "" );
			strcpy( Buffer13 , "" );

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
										if ( z1 != z2 ) {
											z1 = z2;
											z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer9 , z1 , 255 );		
											if ( z1 != z2 ) {
												z1 = z2;
												z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer10 , z1 , 255 );
												if ( z1 != z2 ) {
													z1 = z2;
													z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer11 , z1 , 255 );															
													if ( z1 != z2 ) {
														z1 = z2;
														z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer12 , z1 , 255 );																	
														if ( z1 != z2 ) {
															z1 = z2;
															z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer13 , z1 , 255 );										
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			sprintf( Buffer1_2 , "%d" , i + 1 );

			//Bluetain 1set All station : 30Line Max Compare
			if( i < 30 ){
				// Compare Data & Write String
				if(strlen(Buffer4) > 0 && strlen(Buffer_imsi[i][3]) > 0){
					dbLS = atof(Buffer4) - atof(Buffer_imsi[i][3]);
					sprintf(LS, "%s(%6.3f)", Buffer4, dbLS);
				}
				else sprintf(LS, "%s", Buffer4);
				
				if(strlen(Buffer5) > 0 && strlen(Buffer_imsi[i][4]) > 0){
					dbLE = atof(Buffer5) - atof(Buffer_imsi[i][4]);
					sprintf(LE, "%s(%6.3f)", Buffer5, dbLE);
				}
				else sprintf(LE, "%s", Buffer5);
				
				if(strlen(Buffer6) > 0 && strlen(Buffer_imsi[i][5]) > 0){
					dbRS = atof(Buffer6) - atof(Buffer_imsi[i][5]);
					sprintf(RS, "%s(%6.3f)", Buffer6, dbRS);
				}
				else sprintf(RS, "%s", Buffer6);
				
				if(strlen(Buffer7) > 0 && strlen(Buffer_imsi[i][6]) > 0){
					dbRE = atof(Buffer7) - atof(Buffer_imsi[i][6]);
					sprintf(RE, "%s(%6.3f)", Buffer7, dbRE);
				}
				else sprintf(RE, "%s", Buffer7);
				
				if(strlen(Buffer9) > 0 && strlen(Buffer_imsi[i][8]) > 0){
					dbLS2 = atof(Buffer9) - atof(Buffer_imsi[i][8]);
					sprintf(LS2, "%s(%6.3f)", Buffer9, dbLS2);
				}
				else sprintf(LS2, "%s", Buffer9);
				
				if(strlen(Buffer10) > 0 && strlen(Buffer_imsi[i][9]) > 0){
					dbLE2 = atof(Buffer10) - atof(Buffer_imsi[i][9]);
					sprintf(LE2, "%s(%6.3f)", Buffer10, dbLE2);
				}
				else sprintf(LE2, "%s", Buffer10);			
				
				
				if(strlen(Buffer11) > 0 && strlen(Buffer_imsi[i][10]) > 0){
					dbRS2 = atof(Buffer11) - atof(Buffer_imsi[i][10]);
					sprintf(RS2, "%s(%6.3f)", Buffer11, dbRS2);
				}
				else sprintf(RS2, "%s", Buffer11);
				
				if(strlen(Buffer12) > 0 && strlen(Buffer_imsi[i][11]) > 0){
					dbRE2 = atof(Buffer12) - atof(Buffer_imsi[i][11]);
					sprintf(RE2, "%s(%6.3f)", Buffer12, dbRE2);
				}
				else sprintf(RE2, "%s", Buffer12);
			}else{
				sprintf(LS, "%s", Buffer4);
				sprintf(LE, "%s", Buffer5);
				sprintf(RS, "%s", Buffer6);
				sprintf(RE, "%s", Buffer7);
				sprintf(LS2, "%s", Buffer9);
				sprintf(LE2, "%s", Buffer10);
				sprintf(RS2, "%s", Buffer11);
				sprintf(RE2, "%s", Buffer12);
			}

			LView_Insert_Column( GetDlgItem( hdlg , IDC_DATA_LIST ) , Buffer1_2 , Buffer1 , Buffer2 , Buffer3 , LS, LE, RS, RE, Buffer8, LS2, LE2, RS2, RE2, i - 1 );

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
// 2013.11.07 Compare Previous Data by Hyuk

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_OnlineLog_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT	ps;
	int i;
//	TCHAR       *szString[] = { "No" , "Time" , "Type" , "Contents" };
//	int         szSize[4]   = {  55  ,    155 ,    140 ,  2000 };
	/*
	TCHAR       *szString[] = { "No", "Time", "Station", "Arm", "L_Start", "L_End", "R_Start", "R_End", "Arm", "L_Start", "L_End", "R_Start", "R_End"
								, "Arm", "L_Start", "L_End", "R_Start", "R_End", "Arm", "L_Start", "L_End", "R_Start", "R_End"};
	int         szSize[23]     = { 35, 135, 55, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60 };
	int         szSize_M[23]   = { 35, 135, 55, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60 };
	*/

	TCHAR       *szString[] = { "No", "Time", "Station", "Arm", "L_Start", "L_End", "R_Start", "R_End", "Arm", "L_Start", "L_End", "R_Start", "R_End"};

	//int         szSize[23]     = { 35, 135, 55, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60};
	//int         szSize_M[23]   = { 35, 135, 55, 37, 60, 60, 60, 60, 37, 60, 60, 60, 60};

	int         szSize[23]     = { 35, 135, 55, 37, 120, 120, 120, 120, 37, 120, 120, 120, 120};
	int         szSize_M[23]   = { 35, 135, 55, 37, 120, 120, 120, 120, 37, 120, 120, 120, 120};
	
	char Buffer[256];
	HBITMAP hBitmap;

	// 2006.05.29
	HFILE hFile;
	int  ReadCnt , posindex , posdata;
	BOOL FileEnd;
	int retindexs[256];
	int retindexsz[256];
	char NameBuffer[256];

	switch ( msg ) {
		case WM_INITDIALOG:
			SetWindowText( hdlg , "AWC Scan Log" );
			//<<-----------------------------------------------------------
			KGUI_STANDARD_Set_Function_Size( hdlg );
			//>>-----------------------------------------------------------
			if ( Get_Prm_TITLE_MODE() ) i = 40;
			else                        i = 0;
			//<<-----------------------------------------------------------
			hList = GetDlgItem( hdlg , IDC_DATA_LIST );

			//====================================================================================================================================================
			// 2006.05.29
			//====================================================================================================================================================
			// BUTTON_RELOAD_FILE				
			// BUTTON_FILE_LIST			
			// BUTTON_DATA			
			// BUTTON_STATUS_TEXT			
			// BUTTON_STATUS_BOX				
			// BUTTON_STATUS_DATA			
			// BUTTON_READ		
			// BUTTON_DELETE			
			Global_Pos_Set( BUTTON_RELOAD_FILE	,   5 ,  8 + i , 174 , 32 ); // reload file list

			Global_Pos_Set( BUTTON_READ			, 588 ,  8 + i , 166 , 32 ); // read again
			Global_Pos_Set( BUTTON_DELETE		, 755 ,  8 + i , 166 , 32 ); // delete file

			Global_Pos_Set( BUTTON_STATUS_BOX	, 180 ,  2 + i , 303 , 39 ); // box
			Global_Pos_Set( BUTTON_STATUS_TEXT	, 189 , 14 + i ,  56 , 23 ); // string
			Global_Pos_Set( BUTTON_STATUS_DATA	, 246 , 13 + i , 233 , 24 ); // data count

			Global_Pos_Set( BUTTON_FILE_LIST	,   5 , 45 + i , 174 , 535 - i ); // file list
			Global_Pos_Set( BUTTON_DATA			, 179 , 45 + i , 743 , 535 - i ); // data list
			//====================================================================================================================================================
			sprintf( NameBuffer , "system/AwcScanlog(%s).cfg" , PROGRAM_FUNCTION_READ() );
			FileEnd = TRUE;
			if ( ( hFile = _lopen( NameBuffer , OF_READ ) ) != -1 ) {
				while( FileEnd ) {
					FileEnd = H_Get_Line_String_From_File2_Include_Index( hFile , Buffer , 255 , &ReadCnt , retindexs , retindexsz );
					if ( ReadCnt > 4 ) {
						if ( Get_Data_From_String_with_index( Buffer , NameBuffer , retindexs[0] , retindexsz[0] ) ) {
							posindex = -1;
							if      ( STRCMP_L( "BUTTON_RELOAD_FILE" , NameBuffer ) ) posindex = BUTTON_RELOAD_FILE;
							else if ( STRCMP_L( "BUTTON_FILE_LIST" , NameBuffer ) )  posindex = BUTTON_FILE_LIST;
							else if ( STRCMP_L( "BUTTON_DATA" , NameBuffer ) )  posindex = BUTTON_DATA;
							else if ( STRCMP_L( "BUTTON_STATUS_TEXT" , NameBuffer ) )  posindex = BUTTON_STATUS_TEXT;
							else if ( STRCMP_L( "BUTTON_STATUS_BOX" , NameBuffer ) )  posindex = BUTTON_STATUS_BOX;
							else if ( STRCMP_L( "BUTTON_STATUS_DATA" , NameBuffer ) )  posindex = BUTTON_STATUS_DATA;
							else if ( STRCMP_L( "BUTTON_READ" , NameBuffer ) )  posindex = BUTTON_READ;
							else if ( STRCMP_L( "BUTTON_DELETE" , NameBuffer ) )  posindex = BUTTON_DELETE;
							//
							if ( posindex != -1 ) {
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[1] , retindexsz[1] , NameBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_X[posindex] = posdata;
								}
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[2] , retindexsz[2] , NameBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_Y[posindex] = posdata;
								}
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[3] , retindexsz[3] , NameBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_XS[posindex] = posdata;
								}
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[4] , retindexsz[4] , NameBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_YS[posindex] = posdata;
								}
							}
						}
					}
				}
				_lclose( hFile );
			}
			//====================================================================================================================================================
			KWIN_Item_Resize( hdlg , IDYES				, Global_Pos_X[BUTTON_RELOAD_FILE] , Global_Pos_Y[BUTTON_RELOAD_FILE] , Global_Pos_XS[BUTTON_RELOAD_FILE] , Global_Pos_YS[BUTTON_RELOAD_FILE] ); // reload file list
			KWIN_Item_Resize( hdlg , IDRETRY			, Global_Pos_X[BUTTON_READ] , Global_Pos_Y[BUTTON_READ] , Global_Pos_XS[BUTTON_READ] , Global_Pos_YS[BUTTON_READ] ); // read again
			KWIN_Item_Resize( hdlg , IDABORT			, Global_Pos_X[BUTTON_DELETE] , Global_Pos_Y[BUTTON_DELETE] , Global_Pos_XS[BUTTON_DELETE] , Global_Pos_YS[BUTTON_DELETE] ); // delete file

			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS2	, Global_Pos_X[BUTTON_STATUS_BOX] , Global_Pos_Y[BUTTON_STATUS_BOX] , Global_Pos_XS[BUTTON_STATUS_BOX] , Global_Pos_YS[BUTTON_STATUS_BOX] ); // box
			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS	, Global_Pos_X[BUTTON_STATUS_TEXT] , Global_Pos_Y[BUTTON_STATUS_TEXT] , Global_Pos_XS[BUTTON_STATUS_TEXT] , Global_Pos_YS[BUTTON_STATUS_TEXT] ); // string
			KWIN_Item_Resize( hdlg , IDC_DATA_TOTAL		, Global_Pos_X[BUTTON_STATUS_DATA] , Global_Pos_Y[BUTTON_STATUS_DATA] , Global_Pos_XS[BUTTON_STATUS_DATA] , Global_Pos_YS[BUTTON_STATUS_DATA] ); // data count

			KWIN_Item_Resize( hdlg , IDC_LIST_FILE		, Global_Pos_X[BUTTON_FILE_LIST] , Global_Pos_Y[BUTTON_FILE_LIST] , Global_Pos_XS[BUTTON_FILE_LIST] , Global_Pos_YS[BUTTON_FILE_LIST] ); // file list

			KWIN_Item_Resize( hdlg , IDC_DATA_LIST		, Global_Pos_X[BUTTON_DATA] , Global_Pos_Y[BUTTON_DATA] , Global_Pos_XS[BUTTON_DATA] , Global_Pos_YS[BUTTON_DATA] ); // data list(bin)
/*
			KWIN_Item_Resize( hdlg , IDYES              ,   5 ,  8 + i , 174 , 32 ); // reload file list
			KWIN_Item_Resize( hdlg , IDRETRY            , 588 ,  8 + i , 166 , 32 ); // read again
			KWIN_Item_Resize( hdlg , IDABORT            , 755 ,  8 + i , 166 , 32 ); // delete file
			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS2 , 180 ,  2 + i , 303 , 39 ); // box
			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS  , 189 , 14 + i ,  56 , 23 ); // string
			KWIN_Item_Resize( hdlg , IDC_DATA_TOTAL     , 246 , 13 + i , 233 , 24 ); // data count

			KWIN_Item_Resize( hdlg , IDC_LIST_FILE      ,   5 , 45 + i , 174 , 535 - i ); // file list
			KWIN_Item_Resize( hdlg , IDC_DATA_LIST      , 179 , 45 + i , 743 , 535 - i ); // data list(bin)
*/
			//
			if ( Get_Prm_MINISCREEN_MODE() )
				KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_DATA_LIST ) , 13 , szString , szSize_M );
			else
				KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_DATA_LIST ) , 13 , szString , szSize );

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
		case WM_NOTIFY:
			{
				NM_LISTVIEW *pNM = (NM_LISTVIEW*) lParam;
				if ( pNM->hdr.code == LVN_COLUMNCLICK ) {
					if(pNM->iSubItem == 1){
						if ( nSortDir[ pNM->iSubItem ] == ASC )
							nSortDir[ pNM->iSubItem ] = DESC;
						else
							nSortDir[ pNM->iSubItem ] = ASC;
						ListView_SortItems( GetDlgItem( hdlg , IDC_DATA_LIST ) , pfnCompare , pNM->iSubItem );
					}
				}
			}
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
					return TRUE;

				case IDRETRY :
					// Compare Previous Data Added by hyuk 2013.11.07
					GET_WM_COMMAND_CMD( wParam , lParam );
					GET_WM_COMMAND_ID( wParam , lParam );
					
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_FILE ) );

					if ( i > 0 ) {
						ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_FILE ) , i-1 , Buffer );
						sprintf( SELECT_FILENAME_BEFORE , "%s/%s%s.log" , HISTORY_DIRECTORY , IDSTR_NAME , Buffer );
						printf("Select Filename Before : %s\n", SELECT_FILENAME_BEFORE);
						CompareView(hdlg);
						FileData_ListView_Init(GetDlgItem( hdlg , IDC_LIST_FILE ) , HISTORY_DIRECTORY );
					}
					return TRUE;
					// Compare Previous Data Added by hyuk 2013.11.07
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
							if ( IDOK == MessageBox( hdlg , "Do you want to delete AWCSCAN Log Data File ?", "Delete", MB_ICONQUESTION | MB_OKCANCEL ) ) {
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
//	REGIST_FILE_INFO( "Gui Operate Log View 3.84(2006.02.16)" ); // Log Folder Auto Part Append
//	REGIST_FILE_INFO( "Gui Operate Log View 3.9(2006.05.29)" ); // Reposition Part Append
	REGIST_FILE_INFO( "Gui Operate Log View 3.91(2006.11.14)" ); // Sort Part Append(By LJM)
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
		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY_M , IDD_E_ONLINE_HISTORY_M ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY_M , IDD_E_ONLINE_HISTORY_M ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY_M , IDD_E_ONLINE_HISTORY_M ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY_M , IDD_E_ONLINE_HISTORY_M ) , GET_MAIN_WINDOW() , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2002.06.17
	}
	else {
		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY , IDD_E_ONLINE_HISTORY ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY , IDD_E_ONLINE_HISTORY ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY , IDD_E_ONLINE_HISTORY ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY , IDD_E_ONLINE_HISTORY ) , GET_MAIN_WINDOW() , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2002.06.17
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
	STR_SEPERATE_CHAR( ELSE2_STR , '|' , HISTORY_DIRECTORY , IDSTR_NAME , 128 ); // 2004.01.07
	//============================================================================
	// 2006.02.16
	//============================================================================
	if ( strcmp( HISTORY_DIRECTORY , "$" ) == 0 ) { // 2006.02.16
		_IO_GET_OPER_FOLDER( HISTORY_DIRECTORY , 128 ); // 2006.02.16
	}
	//==========================================================
	if ( strlen( IDSTR_NAME ) <= 0 ) strcpy( IDSTR_NAME , "AWCSCAN_" ); // 2004.01.07
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
