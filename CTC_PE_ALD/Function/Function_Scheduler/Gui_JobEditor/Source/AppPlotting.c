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
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>
#include <Kutlanal.h>

#include "appPlotting.h"

#include "iodefine.h"
#include "lottype.h"
#include "ledapi.h"
#include "appinit.h"
#include "appprm.h"
#include "jobdata.h"

#include "cimseqnc.h"

#include "resource.h"

#include <CimSeqncKNFS.h>


void CURRENT_WAFER_INIT( int init );
BOOL LogView_Open_Method_History_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName0 , int jobindex , int infoinsert , int findwid , int fileorder , int mindex , int option6 );
BOOL TreeData_Get_Select_Slot_PM_Info( int index , int subid , char *waferstr , char *pmname , char *pmnameorg );
void Get_Log_File_Name( char *Buffer , char *catstr , int mode , char *logsubfolder , BOOL slotisdummy , int slot , int slot2 , char *slotstr0 , int ch , char *chstr0 , char *appstr );

extern JOBStepTemplate				JOB_DATA[MAX_JOB_COUNT];
extern int  CURRENT_WAFER_VIEW_COUNT;
extern char *CURRENT_WAFER_NAME[MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_WAFER_MODULE_DEPTH[MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_WAFER_MODULE_MODE[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//========================================================================================
//---------------------------------------------------------------------------------------
//========================================================================================
//------------------------------------------------------------------------
#define		PLOTTING_PRM_DRAW_START_X	87
#define		PLOTTING_PRM_DRAW_START_Y	25

#define		PLOTTING_PRM_DRAW_SIZE_X	300
#define		PLOTTING_PRM_DRAW_SIZE_Y	240

#define		PLT_X1_START_TIME			85
#define		PLT_X2_START_TIME			PLT_X1_START_TIME + PLOTTING_PRM_DRAW_START_X- 60
#define		PLT_Y_START_TIME			270
#define		PLT_X_SIZE_TIME				8
//=============================================

//========================================================================================
//========================================================================================
#define	DISPLAY_START_X_FOR_PLOTTER		30
#define	DISPLAY_START_Y_FOR_PLOTTER		300
#define	DISPLAY_SIZE_X_FOR_PLOTTER		280
#define	DISPLAY_SIZE_Y_FOR_PLOTTER		17
//========================================================================================
#define	DISPLAY_START_X_FOR_MINMAX		4
#define	DISPLAY_SIZE_X_FOR_MINMAX		387
#define	DISPLAY_START_Y_FOR_MINMAX		13
#define	DISPLAY_SIZE_Y_FOR_MINMAX		11
#define	DISPLAY_SIZE_Y2_FOR_MINMAX		60
//========================================================================================
#define	PLOTTER_COLOR_FOR_INFO	COLOR_Blank
//========================================================================================
//========================================================================================
BOOL		PLOTTING_PRM_POINT_RECT		= TRUE;
BOOL		PLOTTING_PRM_BACK_DRAW		= TRUE;
BOOL		PLOTTING_PRM_BACK_COLOR		= 0;
BOOL		PLOTTING_PRM_LIN_TYPE		= TRUE;
BOOL		PLOTTING_PRM_LEFT_START		= FALSE;
//=============================================
int			PLOTTING_PRM_LAST_POINTER =	0;
int			PLOTTING_PRM_X_INTERVAL	=	1;
//------------------------------------------------------------------------------------------
char Plt_ITEM[ MAX_X_TABLE ][MAX_ITEM_SIZE+1];
//
char	Plt_Title[ 256 ];
void	*P_IOP[ MAX_Y_TABLE ];
int		PLOTTING_CNT	= 0;
int		PLOTTING_MAX	= 0;
//
double	Plt_Min[ MAX_X_TABLE ];
double	Plt_Max[ MAX_X_TABLE ];
BOOL	Plt_Draw[ MAX_X_TABLE ];
int		Plt_Color[ MAX_X_TABLE ];


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
#define MAX_COMPARE_COUNT	128

int   COMP_COUNT = 0;

int   COMP_INDEX[MAX_COMPARE_COUNT];
char *COMP_LOG[MAX_COMPARE_COUNT];
char *COMP_SLOT[MAX_COMPARE_COUNT];
char *COMP_DESP[MAX_COMPARE_COUNT];
char *COMP_NAME[MAX_COMPARE_COUNT];
char *COMP_FILE[MAX_COMPARE_COUNT];
char *COMP_STARTTIME[MAX_COMPARE_COUNT];
char *COMP_GROUP[MAX_COMPARE_COUNT];
int   COMP_POLL[MAX_COMPARE_COUNT];

int   COMP_PLOT_FUNCTION = -2;


BOOL COMP_HAS_PLOT_FUNC() {
	//
	if ( COMP_PLOT_FUNCTION == -2 ) {
		COMP_PLOT_FUNCTION = _FIND_FROM_STRING( _K_F_IO + 1 , "PROCESS_PLOT_VIEWER" );
	}
	if ( COMP_PLOT_FUNCTION == -1 ) return FALSE;
	return TRUE;
}

BOOL COMP_CALL_PLOT_FUNC( HWND hWnd , char *msg ) {
	//
	if ( !COMP_HAS_PLOT_FUNC() ) return FALSE;
	//
	_dWRITE_FUNCTION_EVENT( COMP_PLOT_FUNCTION , msg );
	//
	KGUI_STANDARD_Set_Enable_Monitoring_Function( hWnd , COMP_PLOT_FUNCTION , 0 ); // 2018.01.10
	//
	return TRUE;
}


void COMP_INIT() {
	int i;
	//
	COMP_COUNT = 0;
	//
	for ( i = 0 ; i < MAX_COMPARE_COUNT ; i++ ) {
		COMP_LOG[i] = NULL;
		COMP_SLOT[i] = NULL;
		COMP_DESP[i] = NULL;
		COMP_NAME[i] = NULL;
		COMP_FILE[i] = NULL;
		COMP_STARTTIME[i] = NULL;
		COMP_GROUP[i] = NULL;
	}
}

//
BOOL COMP_SEARCH( int index , char *file ) {
	int i;
	for ( i = 0 ; i < COMP_COUNT ; i++ ) {
		if ( COMP_INDEX[i] == index ) {
			if ( STRCMP_L( STR_MEM_GET_STR( COMP_FILE[i] ) , file ) ) return TRUE;
		}
	}
	return FALSE;
}


int COMP_APPEND( int index , char *slotstr , char *desp , char *name , char *file , char *starttime , char *group , char *logstr , int polltime ) {
	//
	if ( COMP_COUNT >= MAX_COMPARE_COUNT ) return 1;
	//
	if ( desp[0] == 0 ) return 2;
	if ( name[0] == 0 ) return 3;
	if ( file[0] == 0 ) return 4;
	if ( starttime[0] == 0 ) return 5;
	//
	if ( COMP_SEARCH( index , file ) ) return 6;
	//
	if ( !STR_MEM_MAKE_COPY2( &(COMP_DESP[COMP_COUNT]) , desp ) ) return 11;
	if ( !STR_MEM_MAKE_COPY2( &(COMP_NAME[COMP_COUNT]) , name ) ) return 12;
	if ( !STR_MEM_MAKE_COPY2( &(COMP_FILE[COMP_COUNT]) , file ) ) return 13;
	if ( !STR_MEM_MAKE_COPY2( &(COMP_STARTTIME[COMP_COUNT]) , starttime ) ) return 14;
	if ( !STR_MEM_MAKE_COPY2( &(COMP_GROUP[COMP_COUNT]) , group ) ) return 15;
	if ( !STR_MEM_MAKE_COPY2( &(COMP_SLOT[COMP_COUNT]) , slotstr ) ) return 16;
	if ( !STR_MEM_MAKE_COPY2( &(COMP_LOG[COMP_COUNT]) , logstr ) ) return 17;
	//
	COMP_INDEX[COMP_COUNT] = index;
	COMP_POLL[COMP_COUNT] = polltime;
	//
	COMP_COUNT++;
	//
	return 0;
	//
}

int COMP_CHANGE( int index1 , int index2 ) {
	int x;
	char *cx;
	//
	x = COMP_INDEX[index1];	COMP_INDEX[index1] = COMP_INDEX[index2];	COMP_INDEX[index2] = x;
	//
	cx = COMP_SLOT[index1];	COMP_SLOT[index1] = COMP_SLOT[index2];	COMP_SLOT[index2] = cx;
	cx = COMP_LOG[index1];	COMP_LOG[index1] = COMP_LOG[index2];	COMP_LOG[index2] = cx;
	cx = COMP_DESP[index1];	COMP_DESP[index1] = COMP_DESP[index2];	COMP_DESP[index2] = cx;
	cx = COMP_NAME[index1];	COMP_NAME[index1] = COMP_NAME[index2];	COMP_NAME[index2] = cx;
	cx = COMP_FILE[index1];	COMP_FILE[index1] = COMP_FILE[index2];	COMP_FILE[index2] = cx;
	cx = COMP_STARTTIME[index1];	COMP_STARTTIME[index1] = COMP_STARTTIME[index2];	COMP_STARTTIME[index2] = cx;
	cx = COMP_GROUP[index1];	COMP_GROUP[index1] = COMP_GROUP[index2];	COMP_GROUP[index2] = cx;
	//
	x = COMP_POLL[index1];	COMP_POLL[index1] = COMP_POLL[index2];	COMP_POLL[index2] = x;
	//
	return 0;
}

int COMP_DELETE( int index ) {
	int i , f , c;
	//
	if ( index >= 0 ) {
		//
		COMP_INDEX[index] = -1;
		//
		return 0;
	}
	//
	c = 0;
	//
	while ( TRUE ) {
		//
		f = -1;
		//
		for ( i = 0 ; i < COMP_COUNT ; i++ ) {
			//
			if ( COMP_INDEX[i] < 0 ) {
				f = i;
				break;
			}
			//
		}
		//
		if ( f < 0 ) return c;
		//
		STR_MEM_MAKE_COPY2( &(COMP_LOG[f]) , NULL );
		STR_MEM_MAKE_COPY2( &(COMP_SLOT[f]) , NULL );
		STR_MEM_MAKE_COPY2( &(COMP_DESP[f]) , NULL );
		STR_MEM_MAKE_COPY2( &(COMP_NAME[f]) , NULL );
		STR_MEM_MAKE_COPY2( &(COMP_FILE[f]) , NULL );
		STR_MEM_MAKE_COPY2( &(COMP_STARTTIME[f]) , NULL );
		STR_MEM_MAKE_COPY2( &(COMP_GROUP[f]) , NULL );
		//
		c++;
		//
		for ( i = f ; i < (COMP_COUNT-1) ; i++ ) {
			//
			COMP_INDEX[i] = COMP_INDEX[i + 1];
			COMP_SLOT[i] = COMP_SLOT[i + 1];
			COMP_LOG[i] = COMP_LOG[i + 1];
			COMP_DESP[i] = COMP_DESP[i + 1];
			COMP_NAME[i] = COMP_NAME[i + 1];
			COMP_FILE[i] = COMP_FILE[i + 1];
			COMP_STARTTIME[i] = COMP_STARTTIME[i + 1];
			COMP_GROUP[i] = COMP_GROUP[i + 1];
			COMP_POLL[i] = COMP_POLL[i + 1];
			//
		}
		//
		COMP_SLOT[i] = NULL;
		COMP_LOG[i] = NULL;
		COMP_DESP[i] = NULL;
		COMP_NAME[i] = NULL;
		COMP_FILE[i] = NULL;
		COMP_STARTTIME[i] = NULL;
		COMP_GROUP[i] = NULL;
		//
		COMP_COUNT--;
		//
	}
	//
	return c;
	//
}

extern int JOBCOUNT;

int COMP_CHECK() {
	int i , f;
	char folderstr[256]; // 2017.03.13
	//
	f = 0;
	//
	for ( i = 0 ; i < COMP_COUNT ; i++ ) {
		//
		if      ( COMP_INDEX[i] >= JOBCOUNT ) {
			COMP_INDEX[i] = -1;
			f++;
		}
		else {
			//
			JOB_Get_Directory( COMP_INDEX[i] , folderstr ); // 2017.03.13
			//
//			if ( !STRCMP_L( STR_MEM_GET_STR( COMP_LOG[i] ) , JOB_Get_Directory( COMP_INDEX[i] ) ) ) { // 2017.03.13
			if ( !STRCMP_L( STR_MEM_GET_STR( COMP_LOG[i] ) , folderstr ) ) { // 2017.03.13
				COMP_INDEX[i] = -1;
				f++;
			}
		}
		//
	}
	//
	if ( f > 0 ) COMP_DELETE( -1 );
	//
	return f;
	//
}




int COMP_MAKEFILE( char *filename ) {
	FILE *fpt;
	int i;
	//
	sprintf( filename , "%s/PlotViewInf.cfg" , Get_Prm_SYSTEM_DIRECTORY() );
	//
	if ( COMP_COUNT <= 0 ) return 1;
	//
	fpt = fopen( filename , "w" );
	//
	if ( fpt == NULL ) return 2;
	//
	for ( i = 0 ; i < COMP_COUNT ; i++ ) {
		//
		fprintf( fpt , "%c%s%c	%c%s%c	%c%s%c	%c%s%c	%d	%c%s%c\n" ,
			'"' , STR_MEM_GET_STR( COMP_DESP[i] ) , '"' , 
			'"' , STR_MEM_GET_STR( COMP_NAME[i] ) , '"' , 
			'"' , STR_MEM_GET_STR( COMP_FILE[i] ) , '"' , 
			'"' , STR_MEM_GET_STR( COMP_STARTTIME[i] ) , '"' , 
			COMP_POLL[i] ,
			'"' , STR_MEM_GET_STR( COMP_GROUP[i] ) , '"' );
	}
	fclose( fpt );
	//
	return 0;
	//
}





//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

HWND COMP_TEMP_HWNDPR;
int  COMP_TEMP_INDEX;
char COMP_TEMP_SLOT[256];
char COMP_TEMP_GROUP[256];
char COMP_TEMP_DESP[256];
char COMP_TEMP_NAME[256];
char COMP_TEMP_FILE[256];
char COMP_TEMP_TIME[256];
int  COMP_TEMP_POLLTIME;

void ListView_Draw_User_with_WM_DRAWITEM_COMP( HWND hwnd , LPARAM lParam , char *buffer , int buffercount ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , b , a;
	int align = 1;
	int color_text , color_back , color_line;
	BOOL sel;
	LVCOLUMN lvcol;
	//
	color_line = COLOR_Blank;
	//
	if ( dis->itemState & ODS_SELECTED ) { // select
		//
		sel = TRUE;
		//
//		color_text = COLOR_White;		color_back = COLOR_DarkBlue;
		color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
		//
	}
	else {
		//
		sel = FALSE;
		//
		color_text = COLOR_Black;				color_back = COLOR_White;
		//
	}
	//
	i = 0;
	z = 0;
	while( TRUE ) {
		//
		lvcol.mask = LVCF_WIDTH;
		lvcol.cx = 0;
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break;
		j = lvcol.cx;
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		a = align;
		b = color_back;
		c = color_text;
		//
		KGUI_DRAW_StrBoxLV2( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , a , color_line , 0 /*lb*/ , c , b , "" , 0 ); // lbline 0:lb 1:l 2:b
		//
		i = i + j;
		z++;
	}
}


LRESULT Comp_EDIT_Notify( HWND hWnd , HWND hWndPr , LPARAM lParam ) {
	int ItemData;
	char RpszText[256];

	LPNMHDR  lpnmh = (LPNMHDR) lParam;
//	LPNMLISTVIEW lpnmv = (LPNMLISTVIEW) lParam;
	LV_DISPINFO *lpdi = (LV_DISPINFO *)lParam;

	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: 

			if      ( lpdi->item.iSubItem == 0 ) {
				sprintf( RpszText , "%d" , lpdi->item.iItem + 1 );
			}
			else if ( lpdi->item.iSubItem == 1 ) {
				sprintf( RpszText , "%s" , STR_MEM_GET_STR( COMP_SLOT[ lpdi->item.iItem ] ) );
			}
			else if ( lpdi->item.iSubItem == 2 ) {
				sprintf( RpszText , "%s" , STR_MEM_GET_STR( COMP_GROUP[ lpdi->item.iItem ] ) );
			}
			else if ( lpdi->item.iSubItem == 3 ) {
				sprintf( RpszText , "%s" , STR_MEM_GET_STR( COMP_NAME[ lpdi->item.iItem ] ) );
			}
			else if ( lpdi->item.iSubItem == 4 ) {
				sprintf( RpszText , "%s" , STR_MEM_GET_STR( COMP_DESP[ lpdi->item.iItem ] ) );
			}
			else if ( lpdi->item.iSubItem == 5 ) {
				sprintf( RpszText , "%s" , STR_MEM_GET_STR( COMP_STARTTIME[ lpdi->item.iItem ] ) );
			}
			else {
				strcpy( RpszText , "" );
			}
			//
			strcpy( lpdi->item.pszText , RpszText );
			return 0;

		case NM_DBLCLK :
			return 0;

		case LVN_COLUMNCLICK:
			return 0;

		case LVN_ITEMCHANGED :
			//
			ItemData = ListView_GetSelectedCount( hWnd );
			//
			if ( ItemData <= 0 ) {
				KWIN_Item_Disable( hWndPr , IDABORT );
				KWIN_Item_Disable( hWndPr , IDNO );
				KWIN_Item_Disable( hWndPr , IDIGNORE );
			}
			else if ( ItemData == 1 ) {
				//
				KWIN_Item_Enable( hWndPr , IDABORT );
				//
				if ( COMP_COUNT <= 1 ) {
					KWIN_Item_Disable( hWndPr , IDNO );
					KWIN_Item_Disable( hWndPr , IDIGNORE );
				}
				else {
					//
					ItemData = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED );
					//
					if ( ItemData == 0 ) {
						KWIN_Item_Disable( hWndPr , IDNO );
					}
					else {
						KWIN_Item_Enable( hWndPr , IDNO );
					}
					//
					if ( ItemData == (COMP_COUNT-1) ) {
						KWIN_Item_Disable( hWndPr , IDIGNORE );
					}
					else {
						KWIN_Item_Enable( hWndPr , IDIGNORE );
					}
				}
			}
			else {
				KWIN_Item_Enable( hWndPr , IDABORT );
				KWIN_Item_Disable( hWndPr , IDNO );
				KWIN_Item_Disable( hWndPr , IDIGNORE );
			}
			//
			return 0;

		case LVN_KEYDOWN :
			return 0;

		case LVN_ODCACHEHINT: {
			LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT) lParam;
		}
		return 0;

		case LVN_ODFINDITEM: {
			LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
		}
		return 0;
	}
	return 0;
}
//

BOOL APIENTRY Console_Gui_For_Comp_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	LPDRAWITEMSTRUCT dis;
	LPNMHDR  lpnmh;
	LV_COLUMN   lvColumn;
	char Temp_Buffer[512];
	int ItemData , i , j , k;
	//
	char B_SlotStr[256];
	char B_NameStr[256];
	char B_FileStr[256];
	char B_GrpStr[256];
	char B_DespStr[256];
	char B_TimeStr[256];
	char B_PMOrg[256];

	char folderstr[256]; // 2017.03.13

	switch ( msg ) {

		case WM_INITDIALOG:
			//
			//------------------------------------
			COMP_CHECK();
			//
			if ( COMP_TEMP_INDEX >= 0 ) {
				//
				sprintf( Temp_Buffer , "%s , %s , %s" , COMP_TEMP_GROUP , COMP_TEMP_SLOT , COMP_TEMP_TIME );
				KWIN_Item_String_Display( hdlg , IDC_CTRL_ADD , Temp_Buffer );
				KWIN_Item_String_Display( hdlg , IDC_CTRL_ADD2 , COMP_TEMP_DESP );
				KWIN_Item_String_Display( hdlg , IDC_CTRL_ADD3 , COMP_TEMP_NAME );
				//
				if ( COMP_SEARCH( COMP_TEMP_INDEX , COMP_TEMP_FILE ) ) {
					KWIN_Item_Disable( hdlg , IDYES );
				}
				//
				if ( COMP_COUNT >= MAX_COMPARE_COUNT ) {
					KWIN_Item_Disable( hdlg , IDYES );
					KWIN_Item_Disable( hdlg , IDYES2 );
					KWIN_Item_Disable( hdlg , IDYES3 );
				}
				//
			}
			else {
				KWIN_Item_Disable( hdlg , IDC_CTRL_ADD );
				KWIN_Item_Disable( hdlg , IDC_CTRL_ADD2 );
				KWIN_Item_Disable( hdlg , IDC_CTRL_ADD3 );
				KWIN_Item_Disable( hdlg , IDYES );
				KWIN_Item_Disable( hdlg , IDYES2 );
				KWIN_Item_Disable( hdlg , IDYES3 );
			}
			//
			KWIN_Item_Disable( hdlg , IDABORT );
			KWIN_Item_Disable( hdlg , IDNO );
			KWIN_Item_Disable( hdlg , IDIGNORE );

			if ( COMP_COUNT <= 1 ) {
				KWIN_Item_Disable( hdlg , IDRETRY );
				KWIN_Item_Disable( hdlg , IDOK );
			}
			else {
				KWIN_Item_Enable( hdlg , IDRETRY );
				KWIN_Item_Enable( hdlg , IDOK );
			}
			//
			ListView_DeleteAllItems( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			while ( TRUE ) if ( !ListView_DeleteColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW )  , 0 ) ) break;
			//
			lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvColumn.fmt = LVCFMT_LEFT;
			//
			lvColumn.cx = 30;				lvColumn.pszText = "No";			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &lvColumn );
			lvColumn.cx = 60;				lvColumn.pszText = "Slot";			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 1 , &lvColumn );
			lvColumn.cx = 60;				lvColumn.pszText = "Group";			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 2 , &lvColumn );
			lvColumn.cx = 240;				lvColumn.pszText = "File";			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 3 , &lvColumn );
			lvColumn.cx = 240;				lvColumn.pszText = "Desp";			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 4 , &lvColumn );
			lvColumn.cx = 120;				lvColumn.pszText = "Time";			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 5 , &lvColumn );
			//
			ListView_SetItemCount( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , COMP_COUNT );
			//
			return TRUE;
			
		case WM_NOTIFY:
			//
			lpnmh = (LPNMHDR) lParam;
			//
			if      ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) ) {
				//
				Comp_EDIT_Notify( lpnmh->hwndFrom , hdlg , lParam );
				//
			}
			return TRUE;


		case WM_DRAWITEM:
			//
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
			if      ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) ) {
				ListView_Draw_User_with_WM_DRAWITEM_COMP( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , Temp_Buffer , 511 );
			}
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
				case IDYES  : // ADD to COMPARE LIST
					//
					if ( COMP_TEMP_INDEX < 0 ) return TRUE;
					//
					JOB_Get_Directory( COMP_TEMP_INDEX , folderstr ); // 2017.03.13
					//
					if ( 0 == COMP_APPEND( COMP_TEMP_INDEX , COMP_TEMP_SLOT , COMP_TEMP_DESP , COMP_TEMP_NAME , COMP_TEMP_FILE , COMP_TEMP_TIME , COMP_TEMP_GROUP , folderstr , COMP_TEMP_POLLTIME ) ) {
						//
						ListView_SetItemCount( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , COMP_COUNT );
						//
						KWIN_Item_Disable( hdlg , IDYES );
						//
						if ( COMP_COUNT <= 1 ) {
							KWIN_Item_Disable( hdlg , IDRETRY );
							KWIN_Item_Disable( hdlg , IDOK );
						}
						else {
							KWIN_Item_Enable( hdlg , IDRETRY );
							KWIN_Item_Enable( hdlg , IDOK );
						}
						//
					}
					else {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "ADD to Compare List Error" , "Error" , MB_ICONHAND );
					}
					//
					return TRUE;

				case IDYES2  : // ADD to COMPARE LIST(All Method)
				case IDYES3  : // ADD to COMPARE LIST(All Lot)
					//
					if ( COMP_TEMP_INDEX < 0 ) return TRUE;
					//
					CURRENT_WAFER_INIT( 0 );
					//----------------------------------------------------------------------------------------------
					//
					LogView_Open_Method_History_Data_Set_Callback( NULL , NULL , 0 , NULL , COMP_TEMP_INDEX , 1 , -1 , -1 , 0 , 0 );
					//
					k = 0;
					//
					for ( i = 0 ; i < CURRENT_WAFER_VIEW_COUNT ; i++ ) {
						//
						for ( j = 0 ; j < CURRENT_WAFER_MODULE_DEPTH[i] ; j++ ) {
							//
							if ( CURRENT_WAFER_MODULE_MODE[i][j] > TCH_PROCESS_X ) continue;
							//
							if ( TreeData_Get_Select_Slot_PM_Info( COMP_TEMP_INDEX , k , B_SlotStr , B_GrpStr , B_PMOrg ) ) {
								//
								if ( wParam == IDYES2 ) {
									if ( !STRCMP_L( COMP_TEMP_SLOT , B_SlotStr ) ) {
										k++;
										continue;
									}
								}
								//
								JOB_Get_Directory( COMP_TEMP_INDEX , folderstr ); // 2017.03.13
								//
								Get_Log_File_Name( B_FileStr , "PROCESS" ,  0 , folderstr , TRUE , 0 , 0 , B_SlotStr , 0 , B_PMOrg , "" );
								Get_Log_File_Name( B_NameStr , "PROCESS" , 10 , folderstr , TRUE , 0 , 0 , B_SlotStr , 0 , B_PMOrg , "" );
								//----------------------------------------------------------------------------------------------
								//
								JOB_Get_Display_String2( COMP_TEMP_INDEX , "PROCESS_LOG" , B_DespStr );
								//
								strcpy( B_TimeStr , JOB_DATA[ COMP_TEMP_INDEX ].RUN_TIME );
								//
								COMP_APPEND( COMP_TEMP_INDEX , B_SlotStr , B_DespStr , B_NameStr , B_FileStr , B_TimeStr , B_GrpStr , folderstr , 0 );
								//
							}
							//
							k++;
							//
						}
						//
					}
					//
					ListView_SetItemCount( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , COMP_COUNT );
					//
					if ( COMP_COUNT <= 1 ) {
						KWIN_Item_Disable( hdlg , IDRETRY );
						KWIN_Item_Disable( hdlg , IDOK );
					}
					else {
						KWIN_Item_Enable( hdlg , IDRETRY );
						KWIN_Item_Enable( hdlg , IDOK );
					}
					//
					KWIN_Item_Disable( hdlg , wParam );
					//
					return TRUE;

				case IDABORT  : // Delete
					//
					ItemData = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , -1 , LVNI_SELECTED );
					//
					while ( ItemData >= 0 ) {
						//
						COMP_DELETE( ItemData );
						//
						ItemData = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , ItemData , LVNI_SELECTED );
						//
					}
					//
					COMP_DELETE( -1 );
					//
					ListView_SetItemCount( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , COMP_COUNT );
					//
					ItemData = ListView_GetSelectedCount( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
					//
					if ( ItemData <= 0 ) {
						KWIN_Item_Disable( hdlg , IDABORT );
						KWIN_Item_Disable( hdlg , IDNO );
						KWIN_Item_Disable( hdlg , IDIGNORE );
					}
					else if ( ItemData == 1 ) {
						//
						KWIN_Item_Enable( hdlg , IDABORT );
						//
						if ( COMP_COUNT <= 1 ) {
							KWIN_Item_Disable( hdlg , IDNO );
							KWIN_Item_Disable( hdlg , IDIGNORE );
						}
						else {
							//
							ItemData = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , -1 , LVNI_SELECTED );
							//
							if ( ItemData == 0 ) {
								KWIN_Item_Disable( hdlg , IDNO );
							}
							else {
								KWIN_Item_Enable( hdlg , IDNO );
							}
							//
							if ( ItemData == (COMP_COUNT-1) ) {
								KWIN_Item_Disable( hdlg , IDIGNORE );
							}
							else {
								KWIN_Item_Enable( hdlg , IDIGNORE );
							}
						}
					}
					else {
						KWIN_Item_Enable( hdlg , IDABORT );
						KWIN_Item_Disable( hdlg , IDNO );
						KWIN_Item_Disable( hdlg , IDIGNORE );
					}
					//
					if ( COMP_COUNT <= 1 ) {
						KWIN_Item_Disable( hdlg , IDRETRY );
						KWIN_Item_Disable( hdlg , IDOK );
					}
					else {
						KWIN_Item_Enable( hdlg , IDRETRY );
						KWIN_Item_Enable( hdlg , IDOK );
					}
					//
					return TRUE;

				case IDNO  : // up
					//
					ItemData = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , -1 , LVNI_SELECTED );
					//
					if ( ( ItemData >= 1 ) && ( ItemData <= (COMP_COUNT-1) ) ) {
						//
						COMP_CHANGE( ItemData , ItemData - 1 );
						//
						ListView_SetItemState( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , -1 , 0 , LVIS_SELECTED );
						ListView_SetItemState( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , ItemData - 1 , LVIS_SELECTED , LVIS_SELECTED );
						//
					}
					//
					return TRUE;

				case IDIGNORE  : // down
					//
					ItemData = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , -1 , LVNI_SELECTED );
					//
					if ( ( ItemData >= 0 ) && ( ItemData < (COMP_COUNT-1) ) ) {
						//
						COMP_CHANGE( ItemData , ItemData + 1 );
						//
						ListView_SetItemState( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , -1 , 0 , LVIS_SELECTED );
						ListView_SetItemState( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , ItemData + 1 , LVIS_SELECTED , LVIS_SELECTED );
						//
					}
					//
					return TRUE;

				case IDRETRY  : // merge
					//
					if ( 0 == COMP_MAKEFILE( B_FileStr ) ) {
						EndDialog( hdlg , 1 );
						//


//						sprintf( B_DespStr , "TABLE_USER_COMPARE %d|%d|%s" , (int) GetParent(hdlg) , 3 , B_FileStr );
						//
//						COMP_CALL_PLOT_FUNC( GetParent(hdlg) , B_DespStr );
						//
						sprintf( B_DespStr , "TABLE_USER_COMPARE %d|%d|%s" , (int) COMP_TEMP_HWNDPR , 3 , B_FileStr );
						//
						COMP_CALL_PLOT_FUNC( COMP_TEMP_HWNDPR , B_DespStr );
					}
					//
					return TRUE;

				case IDOK : // compare
					//
					if ( 0 == COMP_MAKEFILE( B_FileStr ) ) {
						EndDialog( hdlg , 1 );
						//
//						sprintf( B_DespStr , "TABLE_USER_COMPARE %d|%d|%s" , (int) GetParent(hdlg) , 2 , B_FileStr );
						//
//						COMP_CALL_PLOT_FUNC( GetParent(hdlg) , B_DespStr );
						//
						sprintf( B_DespStr , "TABLE_USER_COMPARE %d|%d|%s" , (int) COMP_TEMP_HWNDPR , 2 , B_FileStr );
						//
						COMP_CALL_PLOT_FUNC( COMP_TEMP_HWNDPR , B_DespStr );
						//
					}
					//
					return TRUE;

				case IDCANCEL :
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}



BOOL COMP_EDITING( HWND hdlg , HWND hWndPr , int index , char *slotstr , char *name , char *filename , char *modulestr , int polltime ) {
	//
	COMP_TEMP_HWNDPR = hWndPr;
	//
	COMP_TEMP_INDEX = index;
	//
	if ( index >= 0 ) {
		//
		JOB_Get_Display_String2( index , "PROCESS_LOG" , COMP_TEMP_DESP );
		//
		strcpy( COMP_TEMP_SLOT , slotstr );
		strcpy( COMP_TEMP_NAME , name );
		strcpy( COMP_TEMP_FILE , filename );
		strcpy( COMP_TEMP_GROUP , modulestr );
		strcpy( COMP_TEMP_TIME , JOB_DATA[ index ].RUN_TIME );
		//
		COMP_TEMP_POLLTIME = polltime;
		//
	}
	//
	if ( GoModalDialogBoxParam( GETHINST( hdlg ) , MAKEINTRESOURCE( IDD_COMP_VIEW ) , hdlg , Console_Gui_For_Comp_Proc , (LPARAM) NULL ) ) {
		return TRUE;
	}
	//
	return FALSE;
	//
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void L_View_Open_Full_Init( HWND hwndListView , int count , char *String[] , int *Size ) {
	int         i;
	LV_COLUMN   lvColumn;
	//
	ListView_DeleteAllItems( hwndListView );
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for ( i = 0 ; i < count ; i++ ) {
		lvColumn.cx = Size[i];
		lvColumn.pszText = String[i];
		//
		if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
		//
		ListView_InsertColumn( hwndListView , i , &lvColumn );
	}	
	ListView_DeleteAllItems( hwndListView );
}
//
void L_View_Open_CallBack_Insert( HWND hWnd , int count ) {
	LV_ITEM lvi;
	int i;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	for ( i = 0 ; i < count ; i++ ) {
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.pszText		= LPSTR_TEXTCALLBACK;
		lvi.cchTextMax	= 0;
		lvi.iImage		= i;
		if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return;
	}
}
//
//========================================================================================
//========================================================================================
void PLOTTING_DATA_PARAMETER_SAVE() {
	FILE *fpt;
	int i;
	char FileName[256];
	//
	if ( Get_Prm_NETMONITOR_MODE() ) return; // 2003.12.18
	//
	sprintf( FileName , "%s/Job_Plot.cfg" , Get_Prm_SYSTEM_DIRECTORY() );
	if ( ( fpt = fopen( FileName , "w" ) ) == NULL ) return;
	fprintf( fpt , "$_PLOT_BACK     %4d\n" ,	PLOTTING_PRM_BACK_DRAW );
	fprintf( fpt , "$_PLOT_BACK_COLOR %4d\n" ,	PLOTTING_PRM_BACK_COLOR );
	fprintf( fpt , "$_PLOT_POINT    %4d\n" ,	PLOTTING_PRM_POINT_RECT );
	fprintf( fpt , "$_PLOT_LINE     %4d\n" ,	PLOTTING_PRM_LIN_TYPE );
	fprintf( fpt , "$_PLOT_XINT     %4d\n" ,	PLOTTING_PRM_X_INTERVAL );
	fprintf( fpt , "$_PLOT_LEFT     %4d\n" ,	PLOTTING_PRM_LEFT_START );
	for ( i = 0 ; i < PLOTTING_MAX ; i++ ) {
		fprintf( fpt , "%20s %20.3lf %20.3lf %4d %4d\n" , Plt_ITEM[i] , Plt_Min[i] , Plt_Max[i] , Plt_Draw[i] , Plt_Color[i] );
	}
	fclose(fpt);
}
//========================================================================================
//========================================================================================
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL PLOTTING_DATA_PARAMETER_LOAD() {
	FILE *fpt;
	char Buffer[ 256 ];
	char chr_return1[ 256 ];
	int  ReadCnt , Line , Point , iRes;
	BOOL FileEnd = TRUE;
	char FileName[256];
	char FileName_org[256];
	double d;

	//-----------------------------------------------------------------
	sprintf( FileName_org , "%s/Job_Plot.cfg" , Get_Prm_SYSTEM_DIRECTORY() ); // 2003.12.18
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else { // 2003.12.18
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		if ( !_nINF_KNFS_GetFile( FileName_org , FileName , FALSE ) ) {
			_IO_CIM_PRINTF( "Warning : [%s] Information Saved File Not Found[2]\n" , FileName );
			return TRUE;
		}
	}
	//-----------------------------------------------------------------
//	if ( ( fpt = fopen( FileName , "r+t" ) ) == NULL ) { // 2005.03.22
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) { // 2005.03.22
		_IO_CIM_PRINTF( "Warning : [%s] Information Saved File Not Found\n" , FileName );
		return TRUE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
		FileEnd = Get_Line_String_From_File( fpt , Buffer , &ReadCnt );
		if ( ReadCnt > 0 ) {
			if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 1 - [%s]-[%d]\n" , FileName , Line );
				return FALSE;
			}

			if ( STRCMP_L( "$_PLOT_BACK" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Back Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				PLOTTING_PRM_BACK_DRAW = (BOOL) iRes;
			}
			else if ( STRCMP_L( "$_PLOT_BACK_COLOR" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Back Color Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				PLOTTING_PRM_BACK_COLOR = iRes;
			}
			else if ( STRCMP_L( "$_PLOT_POINT" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Point Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				PLOTTING_PRM_POINT_RECT = (BOOL) iRes;
			}
			else if ( STRCMP_L( "$_PLOT_LINE" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 1 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Point Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				PLOTTING_PRM_LIN_TYPE = iRes;
			}
			else if ( STRCMP_L( "$_PLOT_XINT" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot X int Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				PLOTTING_PRM_X_INTERVAL = iRes;
			}
			else if ( STRCMP_L( "$_PLOT_LEFT" , chr_return1 ) ) {
				if ( !Get_Data_From_String_for_INT( Buffer , 1 , chr_return1 , 0 , 275 , &iRes ) ) {
					_IO_CIM_PRINTF( "Plot Left int Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				PLOTTING_PRM_LEFT_START = iRes;
			}
			else {
				for ( Point = 0 ; Point < PLOTTING_MAX ; Point++ ) {
					if ( STRCMP_L( Plt_ITEM[Point] , chr_return1 ) ) break;
				}
				if ( Point == PLOTTING_MAX ) continue;
				if ( !Get_Data_From_String_for_DOUBLE( Buffer , 1 , chr_return1 , 0 , 0 , &( Plt_Min[Point] ) ) ) {
					_IO_CIM_PRINTF( "MINIMUM Value Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				//
				if ( !Get_Data_From_String_for_DOUBLE( Buffer , 2 , chr_return1 , 0 , 0 , &( Plt_Max[Point] ) ) ) {
					_IO_CIM_PRINTF( "MAXIMUM Value Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				//
				if ( Plt_Min[Point] > Plt_Max[Point] ) {
					d = Plt_Min[Point];
					Plt_Min[Point] = Plt_Max[Point];
					Plt_Max[Point] = d;
				}
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 3 , chr_return1 , 0 , 1 , &(Plt_Draw[Point]) ) ) {
					_IO_CIM_PRINTF( "Display Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
				//
				if ( !Get_Data_From_String_for_INT( Buffer , 4 , chr_return1 , 0 , 275 , &(Plt_Color[Point]) ) ) {
					_IO_CIM_PRINTF( "Polling Time Data Error- [%s]-[%d]\n" , FileName , Line );
					return FALSE;
				}
			}
		}
	}
	fclose(fpt);
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void PLOTTING_DATA_CLEAR() {
	int i;
	for ( i = 0 ; i < PLOTTING_CNT ; i++ ) free( P_IOP[ i ] );
	PLOTTING_CNT = 0;
	PLOTTING_MAX = 0;
	PLOTTING_PRM_LAST_POINTER = 0;
}

void PLOTTING_TITLE_SET( char *Data ) {
	strncpy( Plt_Title , Data , 255 );
}

void PLOTTING_MAX_ITEM( int Item ) {
	if ( Item < MAX_X_TABLE )
		PLOTTING_MAX = Item;
	else
		PLOTTING_MAX = MAX_X_TABLE;
}

void PLOTTING_ADD_ITEM( int Index , char *Item ) {
	if ( Index < MAX_X_TABLE ) {
		strncpy( Plt_ITEM[ Index ] , Item , MAX_ITEM_SIZE );
	}
}

char *PLOTTING_GET_ITEM( int Index ) {
	return Plt_ITEM[ Index ];
}

void PLOTTING_ADD_TIME( char *Time ) {
	if ( PLOTTING_CNT < MAX_Y_TABLE ) { // 2002.10.07
		P_IOP[ PLOTTING_CNT ] = malloc( PIO_TOTAL );
		if ( P_IOP[ PLOTTING_CNT ] == NULL ) {
			_IO_CIM_PRINTF( "Allocate memory for Plotting Table Error\n" );
			return;
		}
		strncpy( P_IO_TIME( PLOTTING_CNT ) , Time , MAX_TIME_SIZE );
		PLOTTING_CNT++;
	}
}

void PLOTTING_ADD_DATA( int Index , double Data ) {
	if ( Index < MAX_X_TABLE )
		P_IO_DATA( PLOTTING_CNT - 1 , Index ) = Data;
}


//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
void ConGui_Display_Y_num_Get( int point , double pos , char *buffer ) {
	STRING_CONV_With_DEV( Plt_Min[ point ] + ( ( Plt_Max[ point ] - Plt_Min[ point ] ) * pos ) , 1 , buffer );
}
//
//========================================================================================
//========================================================================================
//========================================================================================
void Drawing_Plot_Item( HDC hDC ) {
	int i , j , Index;
	char Buffer[64];

	for ( i = 0 , Index = 0 ; i < PLOTTING_MAX ; i++ ) {
		if ( !Plt_Draw[ i ] ) continue;
		KGUI_DRAW_StrBox( hDC , DISPLAY_START_X_FOR_PLOTTER + ( ( Index / 2 ) * DISPLAY_SIZE_X_FOR_PLOTTER ) , DISPLAY_START_Y_FOR_PLOTTER + ( ( Index % 2 ) * DISPLAY_SIZE_Y_FOR_PLOTTER ) ,
			Plt_ITEM[i] , 21 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , Plt_Color[i] , PLOTTER_COLOR_FOR_INFO );

		KGUI_DRAW_Line( hDC , DISPLAY_START_X_FOR_PLOTTER + ( ( Index / 2 ) * DISPLAY_SIZE_X_FOR_PLOTTER ) - 25 , DISPLAY_START_Y_FOR_PLOTTER + ( ( Index % 2 ) * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7,
			DISPLAY_START_X_FOR_PLOTTER + ( ( Index / 2 ) * DISPLAY_SIZE_X_FOR_PLOTTER ) - 2 , DISPLAY_START_Y_FOR_PLOTTER + ( ( Index % 2 ) * DISPLAY_SIZE_Y_FOR_PLOTTER ) + 7 ,
			Plt_Color[i] , FALSE );

		for ( j = 0 ; j < 5 ; j++ ) {
			ConGui_Display_Y_num_Get( i , 1 - ( (double) j * 0.25 ) , Buffer );
			KGUI_DRAW_StrBox( hDC ,
				DISPLAY_START_X_FOR_MINMAX + ( ( Index / 2 ) * DISPLAY_SIZE_X_FOR_MINMAX ) ,
				DISPLAY_START_Y_FOR_MINMAX + ( ( Index % 2 ) * DISPLAY_SIZE_Y_FOR_MINMAX ) + ( j * DISPLAY_SIZE_Y2_FOR_MINMAX ) ,
				Buffer , 10 ,
				PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , Plt_Color[i] , PLOTTER_COLOR_FOR_INFO );
		}
		Index++;
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Get_Graph_Plotting_Value( double A , double B , double data ) {
	if ( A == 0 ) return 120;
	return ( (int) ( 240 + ( (-240) * ( data - B ) / A ) ) );
}
//------------------------------------------------------------------------
int Get_Graph_Plotting_Count( int p ) {
	if ( PLOTTING_PRM_X_INTERVAL <= 0 ) 
		return( ( PLOTTING_PRM_DRAW_SIZE_X * ( 2 - PLOTTING_PRM_X_INTERVAL ) ) + 1 );
	else
		return( PLOTTING_PRM_DRAW_SIZE_X / PLOTTING_PRM_X_INTERVAL + 1 );
}
//------------------------------------------------------------------------
int Get_Graph_Plotting_Draw_Count( int p , int l ) {
	int x;
	if ( PLOTTING_PRM_X_INTERVAL <= 0 ) {
		return( PLOTTING_PRM_DRAW_SIZE_X + 1 - l / (2-PLOTTING_PRM_X_INTERVAL) );
	}
	else {
		x = PLOTTING_PRM_DRAW_SIZE_X / PLOTTING_PRM_X_INTERVAL + 1;
		return( x - l );
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------
void Drawing_Graph( HDC hDC ) {
	int  j , xc , pt , xs , ys , ye , Cs , Ce , xq , xz , dc;
	double A , B;
	int DataBuf = -1;

	if ( PLOTTING_CNT <= 0 ) return;

	for ( pt = 0 , dc = 0 ; pt < PLOTTING_MAX ; pt++ ) {
		if ( !Plt_Draw[ pt ] ) continue;
		A = Plt_Max[pt] - Plt_Min[pt];
		B = Plt_Min[pt];
		if ( PLOTTING_CNT == 1 ) {
			ys = Get_Graph_Plotting_Value( A , B , P_IO_DATA( 0 , pt ) );
			if ( PLOTTING_PRM_LEFT_START )
				KGUI_DRAW_Line( hDC , 0 , ys , 0 , ys , Plt_Color[pt] , PLOTTING_PRM_POINT_RECT );
			else
				KGUI_DRAW_Line( hDC , PLOTTING_PRM_DRAW_SIZE_X , ys , PLOTTING_PRM_DRAW_SIZE_Y , ys , Plt_Color[pt] , PLOTTING_PRM_POINT_RECT );
		}
		else {
			xc = Get_Graph_Plotting_Count( pt );
			//
			if ( PLOTTING_PRM_LAST_POINTER < 1 ) {
				Ce = PLOTTING_CNT;
				Cs = Ce - xc + 1;
				if ( Cs < 1 ) Cs = 1;
				if ( PLOTTING_PRM_LEFT_START ) {
					xs = 0;
				}
				else {
					xs = Get_Graph_Plotting_Draw_Count( pt , Ce );
					if ( xs < 0 ) xs = 0;
				}
			}
			else {
				if ( PLOTTING_PRM_LAST_POINTER > PLOTTING_CNT ) {
					Ce = PLOTTING_CNT;
					Cs = Ce - xc + 1;
					if ( Cs < 1 ) Cs = 1;
					if ( PLOTTING_PRM_LEFT_START ) {
						xs = 0;
					}
					else {
						xs = Get_Graph_Plotting_Draw_Count( pt , Ce );
						if ( xs < 0 ) xs = 0;
					}
				}
				else {
					if ( PLOTTING_PRM_LAST_POINTER < xc ) {
						if ( xc > PLOTTING_CNT ) {
							Ce = PLOTTING_CNT;
							Cs = 1;
						}
						else {
							Ce = xc;
							Cs = 1;
						}
					}
					else {
						Ce = PLOTTING_PRM_LAST_POINTER;
						Cs = Ce - xc + 1;
						if ( Cs < 1 ) Cs = 1;
					}
					if ( PLOTTING_PRM_LEFT_START ) {
						xs = 0;
					}
					else {
						xs = Get_Graph_Plotting_Draw_Count( pt , Ce );
						if ( xs < 0 ) xs = 0;
					}
				}
			}
			if ( PLOTTING_PRM_X_INTERVAL <= 0 ) xq = 1; 
			else                                xq = PLOTTING_PRM_X_INTERVAL;
			ys = Get_Graph_Plotting_Value( A , B , P_IO_DATA( Cs - 1 , pt ) );
			for ( j = Cs+1 , xz = 0 ; j <= Ce ; xs++ , j++ ) {
				ye = Get_Graph_Plotting_Value( A , B , P_IO_DATA( j - 1 , pt ) );
				if ( PLOTTING_PRM_LIN_TYPE ) {
					KGUI_DRAW_Line( hDC , xs*xq , ys , (xs+1)*xq , ye , Plt_Color[pt] , PLOTTING_PRM_POINT_RECT );
				}
				else {
					KGUI_DRAW_Line( hDC , xs*xq , ys , xs*xq , ye , Plt_Color[pt] , PLOTTING_PRM_POINT_RECT );
					KGUI_DRAW_Line( hDC , xs*xq , ye , (xs+1)*xq , ye , Plt_Color[pt] , PLOTTING_PRM_POINT_RECT );
				}
				if ( PLOTTING_PRM_X_INTERVAL <= 0 ) {
					if ( xz < PLOTTING_PRM_X_INTERVAL ) {
						xz = 0;
					}
					else {
						xz--;
						xs--;
					}
				}
				ys = ye;
			}
		}
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Console_Gui_For_Drawing( HWND hdlg , HDC hDC ) {
	HBITMAP hBitmap;
	HDC hMemDC;
	hBitmap = CreateCompatibleBitmap( hDC , PLOTTING_PRM_DRAW_SIZE_X , PLOTTING_PRM_DRAW_SIZE_Y );
	hMemDC = CreateCompatibleDC( hDC );
	SelectObject( hMemDC , hBitmap );
	KGUI_DRAW_Box( hMemDC ,   0 ,   0 ,  PLOTTING_PRM_DRAW_SIZE_X , PLOTTING_PRM_DRAW_SIZE_Y , PLOTTING_PRM_BACK_COLOR , PLOTTING_PRM_BACK_COLOR );
	if ( PLOTTING_PRM_BACK_DRAW ) {
		KGUI_DRAW_Line( hMemDC ,   0 ,  30 ,  300 ,  30 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,   0 ,  90 ,  300 ,  90 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,   0 , 150 ,  300 , 150 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,   0 , 210 ,  300 , 210 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,  30 ,   0 ,   30 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,  90 ,   0 ,   90 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC , 150 ,   0 ,  150 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC , 210 ,   0 ,  210 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC , 270 ,   0 ,  270 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,   0 ,  60 ,  300 ,  60 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,   0 , 120 ,  300 , 120 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,   0 , 180 ,  300 , 180 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hMemDC ,  60 ,   0 ,   60 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hMemDC , 120 ,   0 ,  120 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hMemDC , 180 ,   0 ,  180 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hMemDC , 240 ,   0 ,  240 , 240 , COLOR_MediumGray , FALSE );
	}
	Drawing_Graph( hMemDC );
	BitBlt( hDC , PLOTTING_PRM_DRAW_START_X , PLOTTING_PRM_DRAW_START_Y , PLOTTING_PRM_DRAW_SIZE_X , PLOTTING_PRM_DRAW_SIZE_Y , hMemDC , 0 , 0 , SRCCOPY );
	DeleteDC( hMemDC );
	DeleteObject( hBitmap );
	//
	KWIN_Item_Show( hdlg , IDC_PLOT_SCROLL );
	MoveWindow( GetDlgItem( hdlg , IDC_PLOT_SCROLL ) , 
			PLT_X1_START_TIME + 70 , PLT_Y_START_TIME - 2 , PLOTTING_PRM_DRAW_SIZE_X - 134 , 20 , TRUE );
	if ( PLOTTING_PRM_LAST_POINTER < 1 ) {
		SetScrollRange( GetDlgItem( hdlg , IDC_PLOT_SCROLL ) , SB_CTL , 1 , PLOTTING_CNT , TRUE );
		SetScrollPos( GetDlgItem( hdlg , IDC_PLOT_SCROLL ) , SB_CTL , PLOTTING_CNT , TRUE );
	}
	else {
		SetScrollRange( GetDlgItem( hdlg , IDC_PLOT_SCROLL ) , SB_CTL , 1 , PLOTTING_CNT , TRUE );
		SetScrollPos( GetDlgItem( hdlg , IDC_PLOT_SCROLL ) , SB_CTL , PLOTTING_PRM_LAST_POINTER , TRUE );
	}
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
//================================================================================================
//================================================================================================
//================================================================================================
//================================================================================================
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY Plot_Item_Change_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	static int Point;
	int i , j , iRes;
	static int PLT_DRAW;
	static int PLT_COLOR;
	static double PLT_MIN;
	static double PLT_MAX;


	switch ( msg ) {

		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			Point = (int) lParam;
			MoveCenterWindow( hdlg );

			PLT_DRAW = Plt_Draw[ Point ];
			PLT_COLOR = Plt_Color[Point];
			PLT_MIN = Plt_Min[Point];
			PLT_MAX = Plt_Max[Point];

			KWIN_Item_String_Display( hdlg , IDC_PLOT_NAME , Plt_ITEM[Point] );
			if ( PLT_DRAW ) 
				KWIN_Item_String_Display( hdlg , IDC_PLOT_SHOW , "Yes" );
			else
				KWIN_Item_String_Display( hdlg , IDC_PLOT_SHOW , "No" );
			KWIN_Item_Double_Display( hdlg , IDC_PLOT_MIN , PLT_MIN , 1 );
			KWIN_Item_Double_Display( hdlg , IDC_PLOT_MAX , PLT_MAX , 1 );
			KWIN_Item_Int_Display( hdlg , IDC_PLOT_COLOR , PLT_COLOR );
			//
			for ( i = 0 , j = 0 ; i < PLOTTING_MAX ; i++ ) {
				if ( ( i != Point ) && ( Plt_Draw[ i ] ) ) j++;
			}
			if ( j >= 4 ) {
				KWIN_Item_Disable( hdlg , IDC_PLOT_SHOW );
			}
			//
			if ( PLOTTING_PRM_BACK_DRAW ) 
				KWIN_Item_String_Display( hdlg , IDC_PLOT_BACKLINE , "Draw" );
			else
				KWIN_Item_String_Display( hdlg , IDC_PLOT_BACKLINE , "" );
			KWIN_Item_Int_Display( hdlg , IDC_PLOT_BACKCOLOR , PLOTTING_PRM_BACK_COLOR );
			if ( PLOTTING_PRM_POINT_RECT ) 
				KWIN_Item_String_Display( hdlg , IDC_PLOT_BOLDPOINT , "Draw" );
			else
				KWIN_Item_String_Display( hdlg , IDC_PLOT_BOLDPOINT , "" );
			if ( PLOTTING_PRM_LIN_TYPE ) 
				KWIN_Item_String_Display( hdlg , IDC_PLOT_LINEDRAW , "Linear" );
			else
				KWIN_Item_String_Display( hdlg , IDC_PLOT_LINEDRAW , "Rect" );
			if ( PLOTTING_PRM_LEFT_START ) 
				KWIN_Item_String_Display( hdlg , IDC_PLOT_LEFTSTART , "True" );
			else
				KWIN_Item_String_Display( hdlg , IDC_PLOT_LEFTSTART , "False" );
			switch( PLOTTING_PRM_X_INTERVAL ) {
				case -12 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/14" ); break;
				case -11 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/13" ); break;
				case -10 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/12" ); break;
				case -9 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/11" ); break;
				case -8 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/10" ); break;
				case -7 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/9" ); break;
				case -6 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/8" ); break;
				case -5 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/7" ); break;
				case -4 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/6" ); break;
				case -3 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/5" ); break;
				case -2 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/4" ); break;
				case -1 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/3" ); break;
				case 0 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/2" ); break;
				case 1 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1" ); break;
				case 2 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "2" ); break;
				case 3 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "3" ); break;
				case 4 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "4" ); break;
				case 5 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "5" ); break;
				case 6 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "6" ); break;
				case 7 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "7" ); break;
				case 8 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "8" ); break;
				case 9 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "9" ); break;
				case 10 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "10" ); break;
				case 11 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "11" ); break;
				case 12 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "12" ); break;
			}
			return TRUE;

		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch ( LOWORD( wParam ) ) {
			case IDC_PLOT_SHOW :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "NO|YES" , &PLT_DRAW ) ) {
						if ( PLT_DRAW ) 
							KWIN_Item_String_Display( hdlg , IDC_PLOT_SHOW , "Yes" );
						else
							KWIN_Item_String_Display( hdlg , IDC_PLOT_SHOW , "No" );
					}
					return TRUE;

			case IDC_PLOT_COLOR :
					if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Select" , 0 , 64 , &PLT_COLOR ) ) {
						KWIN_Item_Int_Display( hdlg , IDC_PLOT_COLOR , PLT_COLOR );
					}
					return TRUE;

			case IDC_PLOT_MIN :
					if ( MODAL_ANALOG_BOX1( hdlg , "Select" , "Minimum" , -999999 , 999999 , 1 , &PLT_MIN ) ) {
						KWIN_Item_Double_Display( hdlg , IDC_PLOT_MIN , PLT_MIN , 1 );
					}
					return TRUE;

			case IDC_PLOT_MAX :
					if ( MODAL_ANALOG_BOX1( hdlg , "Select" , "Maximum" , -999999 , 999999 , 1 , &PLT_MAX ) ) {
						KWIN_Item_Double_Display( hdlg , IDC_PLOT_MAX , PLT_MAX , 1 );
					}
					return TRUE;

			//----------

			case IDC_PLOT_BACKLINE :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "NO|Draw" , &PLOTTING_PRM_BACK_DRAW ) ) {
						if ( PLOTTING_PRM_BACK_DRAW ) 
							KWIN_Item_String_Display( hdlg , IDC_PLOT_BACKLINE , "Draw" );
						else
							KWIN_Item_String_Display( hdlg , IDC_PLOT_BACKLINE , "" );
					}
					return TRUE;
			case IDC_PLOT_BACKCOLOR :
					if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Select" , 0 , 64 , &PLOTTING_PRM_BACK_COLOR ) ) {
						KWIN_Item_Int_Display( hdlg , IDC_PLOT_BACKCOLOR , PLOTTING_PRM_BACK_COLOR );
					}
					return TRUE;
			case IDC_PLOT_BOLDPOINT :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "NO|Draw" , &PLOTTING_PRM_POINT_RECT ) ) {
						if ( PLOTTING_PRM_POINT_RECT ) 
							KWIN_Item_String_Display( hdlg , IDC_PLOT_BOLDPOINT , "Draw" );
						else
							KWIN_Item_String_Display( hdlg , IDC_PLOT_BOLDPOINT , "" );
					}
					return TRUE;
			case IDC_PLOT_LINEDRAW :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Rect|Linear" , &PLOTTING_PRM_LIN_TYPE ) ) {
						if ( PLOTTING_PRM_LIN_TYPE ) 
							KWIN_Item_String_Display( hdlg , IDC_PLOT_LINEDRAW , "Linear" );
						else
							KWIN_Item_String_Display( hdlg , IDC_PLOT_LINEDRAW , "Rect" );
					}
					return TRUE;
			case IDC_PLOT_LEFTSTART :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "False|True" , &PLOTTING_PRM_LEFT_START ) ) {
						if ( PLOTTING_PRM_LEFT_START ) 
							KWIN_Item_String_Display( hdlg , IDC_PLOT_LEFTSTART , "True" );
						else
							KWIN_Item_String_Display( hdlg , IDC_PLOT_LEFTSTART , "False" );
					}
					return TRUE;
			case IDC_PLOT_XINT :
					iRes = PLOTTING_PRM_X_INTERVAL + 3;
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "X-Interval" , "1/5|1/4|1/3|1/2|1|2|3|4|5|6|7|8|9|10|11|12" , &iRes ) ) {
						PLOTTING_PRM_X_INTERVAL = (iRes-3);
						switch( PLOTTING_PRM_X_INTERVAL ) {
							case -12 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/14" ); break;
							case -11 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/13" ); break;
							case -10 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/12" ); break;
							case -9 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/11" ); break;
							case -8 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/10" ); break;
							case -7 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/9" ); break;
							case -6 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/8" ); break;
							case -5 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/7" ); break;
							case -4 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/6" ); break;
							case -3 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/5" ); break;
							case -2 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/4" ); break;
							case -1 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/3" ); break;
							case 0 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1/2" ); break;
							case 1 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "1" ); break;
							case 2 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "2" ); break;
							case 3 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "3" ); break;
							case 4 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "4" ); break;
							case 5 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "5" ); break;
							case 6 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "6" ); break;
							case 7 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "7" ); break;
							case 8 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "8" ); break;
							case 9 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "9" ); break;
							case 10 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "10" ); break;
							case 11 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "11" ); break;
							case 12 :	KWIN_Item_String_Display( hdlg , IDC_PLOT_XINT , "12" ); break;
						}
					}
					return TRUE;

			case IDOK:
					Plt_Draw[ Point ] = PLT_DRAW;
					Plt_Color[Point] = PLT_COLOR;
					Plt_Min[Point] = PLT_MIN;
					Plt_Max[Point] = PLT_MAX;
					PLOTTING_DATA_PARAMETER_SAVE();
					EndDialog( hdlg , 0 );
					return TRUE;

				case IDCANCEL:
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
LRESULT Plotting_List_Data_ListView_Notify( HWND hWnd , LPARAM lParam ) {
	LPNMHDR  lpnmh = (LPNMHDR) lParam;
	int i, ItemData;
	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: {
				LV_DISPINFO *lpdi = (LV_DISPINFO *)lParam;
				TCHAR szString[ 256 ];
				if ( lpdi->item.iSubItem ) {
					if ( lpdi->item.mask & LVIF_TEXT ) {
						i = lpdi->item.iItem;
						switch( lpdi->item.iSubItem ) {
						case 1 :
							sprintf( szString , "%s" , Plt_ITEM[i] );
							break;
						case 2 :
							if ( Plt_Draw[ i ] )
								sprintf( szString , "Yes" );
							else
								sprintf( szString , "" );
							break;
						case 3 :
							sprintf( szString , "%.1f" , Plt_Min[ i ] );
							break;
						case 4 :
							sprintf( szString , "%.1f" , Plt_Max[ i ] );
							break;
						}
						lstrcpy( lpdi->item.pszText , szString );
					}
				}
				else {
					if ( lpdi->item.mask & LVIF_TEXT ) {
						wsprintf( szString, "%d", lpdi->item.iItem + 1 );
						lstrcpy( lpdi->item.pszText , szString );
					}
					if ( lpdi->item.mask & LVIF_IMAGE ) {
						lpdi->item.iImage = 0;
					}
				}
		}
		return 0;

		case NM_DBLCLK :
			if ( lpnmh->hwndFrom == hWnd ) {
				ItemData = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED );
				if ( ItemData >= 0 ) {
//					GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_PLOTTING_ITEM , IDD_E_PLOTTING_ITEM ) , hWnd , Plot_Item_Change_Proc , (LPARAM) ItemData ); // 2002.06.17
//					GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_PLOTTING_ITEM , IDD_E_PLOTTING_ITEM ) , hWnd , Plot_Item_Change_Proc , (LPARAM) ItemData ); // 2004.01.19
//					GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_PLOTTING_ITEM , IDD_E_PLOTTING_ITEM ) , hWnd , Plot_Item_Change_Proc , (LPARAM) ItemData ); // 2004.01.19 // 2004.08.10
					GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_PLOTTING_ITEM , IDD_E_PLOTTING_ITEM ) , hWnd , Plot_Item_Change_Proc , (LPARAM) ItemData ); // 2004.01.19 // 2004.08.10
					InvalidateRect( GetParent( hWnd ) , NULL , FALSE );
				}
			}
			return 0;

		case LVN_ITEMCHANGED :
			return 0;

		case LVN_KEYDOWN :
			return 0;

		case LVN_ODCACHEHINT: {
			LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT) lParam;
		}
		return 0;

		case LVN_ODFINDITEM: {
			LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
		}
		return 0;
	}
	return 0;
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY LogView_Open_Plotting_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	TCHAR       *szString[] = { "No" , "Name" , "Show" , "Min" , "Max" };
	int         szSize[5]  = {  30  ,   150 ,   47 , 100 , 100 };
	int i , j;

	switch ( msg ) {

		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			//<<-----------------------------------------------------------
			//<<-----------------------------------------------------------
			PLOTTING_DATA_PARAMETER_LOAD();
			//<<-----------------------------------------------------------
			//<<-----------------------------------------------------------
			//<<-----------------------------------------------------------
			SetWindowText( hdlg , Plt_Title );
			MoveCenterWindow( hdlg );
			L_View_Open_Full_Init( GetDlgItem( hdlg , IDC_PLOT_LIST ) , 5 , szString , szSize );
			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_PLOT_LIST ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			L_View_Open_CallBack_Insert( GetDlgItem( hdlg , IDC_PLOT_LIST ) , PLOTTING_MAX );
			return TRUE;

		case WM_NOTIFY:
			Plotting_List_Data_ListView_Notify( GetDlgItem( hdlg , IDC_PLOT_LIST ) , lParam );
			return TRUE;
			
		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			KGUI_DRAW_Box( hDC ,   0 ,   0 ,  600 , 600 , 13 , 13 );
			Console_Gui_For_Drawing( hdlg , hDC );
			Drawing_Plot_Item( hDC );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_HSCROLL:
			if ( PLOTTING_PRM_LAST_POINTER < 1 ) {
				PLOTTING_PRM_LAST_POINTER = PLOTTING_CNT;
			}
			i = PLOTTING_PRM_LAST_POINTER;
			j = PLOTTING_CNT / 10;

			switch( LOWORD( wParam ) ) {
				case SB_PAGEUP   : i -= j;	break;
				case SB_PAGEDOWN : i += j;	break;
				case SB_LINEUP   : i--;		break;
				case SB_LINEDOWN : i++;		break;
				case SB_THUMBTRACK    :
				case SB_THUMBPOSITION :	
					i = HIWORD( wParam );
					break;
			}
			if      ( i <= 0           ) PLOTTING_PRM_LAST_POINTER = 1; 
			else if ( i > PLOTTING_CNT ) PLOTTING_PRM_LAST_POINTER = PLOTTING_CNT;
			else                         PLOTTING_PRM_LAST_POINTER = i;
//			SetScrollRange( GetDlgItem( hdlg , ID_PLOT_SCROLL ) , SB_CTL , 1 , PLOTTING_CNT , TRUE );
//			SetScrollPos( GetDlgItem( hdlg , ID_PLOT_SCROLL ) , SB_CTL , PLOTTING_PRM_LAST_POINTER , TRUE );
			InvalidateRect( hdlg , NULL , FALSE );
			return TRUE;

		case WM_COMMAND :
			switch ( LOWORD( wParam ) ) {
				case IDCANCEL:
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}
