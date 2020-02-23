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
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "iodefine.h"
#include "lottype.h"
#include "ledapi.h"
#include "appinit.h"
#include "appprm.h"
#include "interface.h"

#include "cimseqnc.h"
#include "resource.h"

#include <CimSeqncKNFS.h>

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};
//--------------------------------------------------------------------------------
int  MAX_FILENAME_SIZE = 50; // 2008.02.14

BOOL SAVE_PRESET_MODE = FALSE; // 2008.02.14

//--------------------------------------------------------------------------------
// 2007.09.17
//--------------------------------------------------------------------------------
BOOL SEARCH_CONDITION_USE = FALSE;
char SEARCH_CONDITION1_EXT[256] = { 0 , };
int  SEARCH_CONDITION2_BEGIN_YEAR = 0;
int  SEARCH_CONDITION2_BEGIN_MONTH = 0;
int  SEARCH_CONDITION2_BEGIN_DAY = 0;
int  SEARCH_CONDITION2_END_YEAR = 0;
int  SEARCH_CONDITION2_END_MONTH = 0;
int  SEARCH_CONDITION2_END_DAY = 0;
//--------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	MAIN_WINDOW_REFRESH = FALSE;
//------------------------------------------------------------------------------------------
HINSTANCE	BackBmpLib = NULL;
void		KGUI_Set_BACKGROUND( char *file ) { BackBmpLib = LoadLibrary( file ); }
HINSTANCE	KGUI_Get_BACKGROUND() { return BackBmpLib; }
//------------------------------------------------------------------------------------------
int oldcommsts = 0;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int	 USE_CLUSTERDATA_ITEM_COUNT = 6;
//------------------------------------------------------------------------------------------
char PM_SUB_DIRECTORY[MAX_PM_CHAMBER_DEPTH][MAX_SUB_CHAMBER_DEPTH][256];
int  PM_SUB_CURRENT_POS[MAX_PM_CHAMBER_DEPTH];
int  PM_EX_STATUS[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
int  PM_EX_COUNT = 0;
int  PM_PX_STATUS[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH]; // 2002.05.20
int  PM_PX_COUNT = 0; // 2002.05.20
//------------------------------------------------------------------------------------------
int	 USE_SELECT_CURRENT_PM_TYPE = 0;
//------------------------------------------------------------------------------------------
BOOL PM_LOW_CONTROL[MAX_PM_CHAMBER_DEPTH]; // 2002.11.22
char PM_LOW_DIRECTORY[MAX_PM_CHAMBER_DEPTH][3][64]; // 2002.11.22
char PM_LOW_EXTENSION[MAX_PM_CHAMBER_DEPTH][3][32]; // 2004.03.22
//------------------------------------------------------------------------------------------
int	 MAX_VIEW_CHAMBER = 0;
int	 MAX_VIEW_TYPE = 0;
int  MAX_VIEW_START_POINTER = -1;
int  MAX_VIEW_LAST_POINTER = 0;
int  MAX_VIEW_SELECT_POINTER = 0;
//------------------------------------------------------------------------------------------
extern CLUSTERStepTemplate			CLUSTER_RECIPE;
//------------------------------------------------------------------------------------------
BOOL FileData_ListView_Init( HWND hWnd , char *data , char *data2 , char *data3 , char *data4 ); // 2004.03.22
//------------------------------------------------------------------------------------------
int  PROGRAM_GUI_SPAWNING = 0; // 2003.07.12
//------------------------------------------------------------------------------------------
HWND h_button[256];
int h_button_count = 0;
//------------------------------------------------------------------------------------------
int         LV_Size_4[5]   = { 32 , 163 , 163 , 163 , 155 };
int         LV_Size_4_M[5] = { 32 , 165 , 165 , 165 , 162 };
int         LV_Size_6[7]   = { 30 , 110 , 110 , 110 , 110 , 110 , 108 };
int         LV_Size_6_M[7] = { 30 , 110 , 110 , 110 , 110 , 110 , 108 };
int         LV_Size_8[9]   = { 19 ,  99 ,  99 ,  99 ,  99 ,  99 ,  99 ,  99 ,  98 }; //
int         LV_Size_10[11] = { 19 ,  79 ,  79 ,  79 ,  79 ,  79 ,  79 ,  79 ,  79 ,  79 ,  78 }; //
int         LV_Size_12[13] = { 19 ,  66 ,  66 ,  66 ,  66 ,  66 ,  66 ,  66 ,  66 ,  66 ,  66 ,  66 ,  65 }; //
//------------------------------------------------------------------------------------------
int         F_X[5][5]  = {	16 ,  86 , 156 ,  16 ,  86 , //
							16 ,  86 , 156 ,  16 ,  86 , //
							16 ,  86 , 156 , 226 , 296 , //
							16 ,  86 , 156 , 226 , 296 , //
							16 ,  86 , 156 , 226 , 296 }; //
int         F_Y[5][5]  = { 420 , 420 , 420 , 490 , 490 , //
						   420 , 420 , 420 , 490 , 490 , //
						   490 , 490 , 490 , 490 , 490 , //
						   490 , 490 , 490 , 490 , 490 , //
						   490 , 490 , 490 , 490 , 490 }; //

int         P_XA[5][12] = { 228 , 393 , 558 , 723 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							228 , 338 , 448 , 558 , 668 , 778 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							124 , 224 , 324 , 424 , 524 , 624 , 724 , 824 ,  -1 ,  -1 ,  -1 ,  -1 , //
							124 , 205 , 284 , 364 , 444 , 524 , 604 , 682 , 760 , 838 ,  -1 ,  -1 , //
							124 , 190 , 256 , 322 , 388 , 454 , 520 , 586 , 652 , 718 , 784 , 850 }; //
int         P_YA[5][12] = { 290 , 290 , 290 , 290 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							290 , 290 , 290 , 290 , 290 , 290 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 ,  -1 ,  -1 ,  -1 ,  -1 , //
							264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 ,  -1 ,  -1 , //
							264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 }; //

int         P_XC[5][12] = { 310 , 475 , 640 , 805 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							268 , 378 , 488 , 598 , 708 , 818 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							162 , 262 , 362 , 462 , 562 , 662 , 762 , 862 ,  -1 ,  -1 ,  -1 ,  -1 , //
							162 , 243 , 322 , 402 , 482 , 562 , 642 , 720 , 798 , 876 ,  -1 ,  -1 , //
							153 , 219 , 285 , 351 , 417 , 483 , 549 , 615 , 681 , 747 , 813 , 879 }; //
int         P_YC[5][12] = { 290 , 290 , 290 , 290 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							290 , 290 , 290 , 290 , 290 , 290 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 ,  -1 , //
							264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 ,  -1 ,  -1 ,  -1 ,  -1 , //
							264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 ,  -1 ,  -1 , //
							264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 , 264 }; //

int         P_ALL[5][3] = { 32 , 55 , 4 , // 2005.07.19
							30 , 50 , 3 , //
							15 , 50 , 3 , //
							15 , 50 , 3 , //
							15 , 50 , 3 }; //

//------------------------------------------------------------------------------------------
#define BUTTON_X_SELECT_START		700
#define BUTTON_Y_SELECT_START		80
//------------------------------------------------------------------------------------------
//
//
int		DISPLAY_MAX_VIEW_COUNT = 7;
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  Address_StringIO_for_Scheduler = -2; // 2004.11.08
//
void Set_Event_String_For_Scheduler( char *data ) { // 2004.11.08
	int CommStatus;
	if ( Address_StringIO_for_Scheduler == -2 ) {
		Address_StringIO_for_Scheduler = _FIND_FROM_STRING( _K_S_IO , "e:SCHEDULER" );
	}
	if ( Address_StringIO_for_Scheduler != -1 ) {
		_dWRITE_STRING( Address_StringIO_for_Scheduler , data , &CommStatus );
	}
}
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	int i;
	if ( no == -1 ) {
		KGUI_STANDARD_Window_Kill_Signal();
		KWIN_DIALOG_DRIVER_UNLOADING();
	}
	if ( ( no > 0 ) && ( PROGRAM_GUI_SPAWNING == 0 ) ) { // 2003.07.12
		PROGRAM_GUI_SPAWNING = 1;
		for ( i = 0 ; i < 200 ; i++ ) {
			_sleep(100);
			if ( PROGRAM_GUI_SPAWNING == 2 ) break;
		}
	}
	KGUI_STANDARD_Screen_Nav_Info( no );
}

//------------------------------------------------------------------------------------------
void Change_Name_Type_With_Option( char *string , int mode ) { // 2004.06.15
	int i , l;
	switch( mode ) {
	case 1 :
		l = strlen( string );
		for ( i = 0 ; i < l ; i++ ) {
			string[i] = toupper( string[i] );
		}
		break;
	case 2 :
		l = strlen( string );
		for ( i = 0 ; i < l ; i++ ) {
			string[i] = tolower( string[i] );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//... 2014.10.16 Fixed Align Option Button Position
void FILE_HANDLING_BUTTON_MAKE( HWND hdlg , int Init ) {
	int			F_I, i, ae, ad, ap, ce, cd, cp, xs, ys;

	//------------------------------------------------------------------------------------------------------------------
	if ( Init == 1 ) {
		;
	}
	else 
	{
		for ( i = 0 ; i < h_button_count ; i++ ) DestroyWindow( h_button[i] );
		if ( Init == 2 ) return; // 2004.04.06
	}
	//------------------------------------------------------------------------------------------------------------------
	switch( Get_Prm_DRAW_SIZE() ) {
	case 4 :	F_I = 0;	break;
	case 6 :	F_I = 1;	break;
	case 8 :	F_I = 2;	break;
	case 10 :	F_I = 3;	break;
	case 12 :	F_I = 4;	break;
	}
	//------------------------------------------------------------------------------------------------------------------
	h_button_count = 0;
	//------------------------------------------------------------------------------------------------------------------
	//== File Handling
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][0] , F_Y[F_I][0] , 58 , 58 , hdlg , (HMENU) IDB_NEW_ENABLE , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][0] , F_Y[F_I][0] , 58 , 58 , hdlg , (HMENU) IDB_NEW_DISABLE , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][0] , F_Y[F_I][0] , 58 , 58 , hdlg , (HMENU) IDB_NEW_PRESSED , GETHINST( hdlg ) , NULL );	h_button_count++;
	//
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][1] , F_Y[F_I][1] , 58 , 58 , hdlg , (HMENU) IDB_SAVE_ENABLE   , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][1] , F_Y[F_I][1] , 58 , 58 , hdlg , (HMENU) IDB_SAVE_DISABLE   , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][1] , F_Y[F_I][1] , 58 , 58 , hdlg , (HMENU) IDB_SAVE_PRESSED   , GETHINST( hdlg ) , NULL );	h_button_count++;
	//
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][2] , F_Y[F_I][2] , 58 , 58 , hdlg , (HMENU) IDB_SAVEAS_ENABLE  , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][2] , F_Y[F_I][2] , 58 , 58 , hdlg , (HMENU) IDB_SAVEAS_DISABLE  , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][2] , F_Y[F_I][2] , 58 , 58 , hdlg , (HMENU) IDB_SAVEAS_PRESSED  , GETHINST( hdlg ) , NULL );	h_button_count++;
	//
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][3] , F_Y[F_I][3] , 58 , 58 , hdlg , (HMENU) IDB_CANCEL_ENABLE , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][3] , F_Y[F_I][3] , 58 , 58 , hdlg , (HMENU) IDB_CANCEL_DISABLE , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][3] , F_Y[F_I][3] , 58 , 58 , hdlg , (HMENU) IDB_CANCEL_PRESSED , GETHINST( hdlg ) , NULL );	h_button_count++;
	//
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][4] , F_Y[F_I][4] , 58 , 58 , hdlg , (HMENU) IDB_FILE_ENABLE , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][4] , F_Y[F_I][4] , 58 , 58 , hdlg , (HMENU) IDB_FILE_DISABLE , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , F_X[F_I][4] , F_Y[F_I][4] , 58 , 58 , hdlg , (HMENU) IDB_FILE_PRESSED , GETHINST( hdlg ) , NULL );	h_button_count++;

	//== Contents Insert / Remove
	i = 0;
	//== Process Select Recipe Pr / Main / Po
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , BUTTON_X_SELECT_START , BUTTON_Y_SELECT_START + i , 154 , 40 , hdlg , (HMENU) IDB_SELECT_RCP_PR , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , BUTTON_X_SELECT_START , BUTTON_Y_SELECT_START + i , 154 , 40 , hdlg , (HMENU) IDB_SELECT_RCP_MAIN , GETHINST( hdlg ) , NULL );	h_button_count++;
	h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , BUTTON_X_SELECT_START , BUTTON_Y_SELECT_START + i , 154 , 40 , hdlg , (HMENU) IDB_SELECT_RCP_PO , GETHINST( hdlg ) , NULL );	h_button_count++;

	//////////////////////////////////////////////////////////////////////////
	for ( i = 0 ; i < Get_Prm_DRAW_SIZE() ; i++ ) 
	{
		if ( F_I == 0 ) 
		{
			if ( i == 0 ) {
				ae = IDB_ADD_ENABLE_P1;		ad = IDB_ADD_DISABLE_P1;		ap = IDB_ADD_PRESSED_P1;
				ce = IDB_CLEAR_ENABLE_P1;	cd = IDB_CLEAR_DISABLE_P1;		cp = IDB_CLEAR_PRESSED_P1;
			}
			else if ( i == 1 ) {
				ae = IDB_ADD_ENABLE_P2;		ad = IDB_ADD_DISABLE_P2;		ap = IDB_ADD_PRESSED_P2;
				ce = IDB_CLEAR_ENABLE_P2;	cd = IDB_CLEAR_DISABLE_P2;		cp = IDB_CLEAR_PRESSED_P2;
			}
			else if ( i == 2 ) {
				ae = IDB_ADD_ENABLE_P3;		ad = IDB_ADD_DISABLE_P3;		ap = IDB_ADD_PRESSED_P3;
				ce = IDB_CLEAR_ENABLE_P3;	cd = IDB_CLEAR_DISABLE_P3;		cp = IDB_CLEAR_PRESSED_P3;
			}
			else if ( i == 3 ) {
				ae = IDB_ADD_ENABLE_P4;		ad = IDB_ADD_DISABLE_P4;		ap = IDB_ADD_PRESSED_P4;
				ce = IDB_CLEAR_ENABLE_P4;	cd = IDB_CLEAR_DISABLE_P4;		cp = IDB_CLEAR_PRESSED_P4;
			}
			xs = 80;	ys = 33;
		}
		else 
		{
			if ( i == 0 ) {
				ae = IDB_ADD_ENABLE2_P1;	ad = IDB_ADD_DISABLE2_P1;		ap = IDB_ADD_PRESSED2_P1;
				ce = IDB_CLEAR_ENABLE2_P1;	cd = IDB_CLEAR_DISABLE2_P1;		cp = IDB_CLEAR_PRESSED2_P1;
			}
			else if ( i == 1 ) {
				ae = IDB_ADD_ENABLE2_P2;	ad = IDB_ADD_DISABLE2_P2;		ap = IDB_ADD_PRESSED2_P2;
				ce = IDB_CLEAR_ENABLE2_P2;	cd = IDB_CLEAR_DISABLE2_P2;		cp = IDB_CLEAR_PRESSED2_P2;
			}
			else if ( i == 2 ) {
				ae = IDB_ADD_ENABLE2_P3;	ad = IDB_ADD_DISABLE2_P3;		ap = IDB_ADD_PRESSED2_P3;
				ce = IDB_CLEAR_ENABLE2_P3;	cd = IDB_CLEAR_DISABLE2_P3;		cp = IDB_CLEAR_PRESSED2_P3;
			}
			else if ( i == 3 ) {
				ae = IDB_ADD_ENABLE2_P4;	ad = IDB_ADD_DISABLE2_P4;		ap = IDB_ADD_PRESSED2_P4;
				ce = IDB_CLEAR_ENABLE2_P4;	cd = IDB_CLEAR_DISABLE2_P4;		cp = IDB_CLEAR_PRESSED2_P4;
			}
			else if ( i == 4 ) {
				ae = IDB_ADD_ENABLE2_P5;	ad = IDB_ADD_DISABLE2_P5;		ap = IDB_ADD_PRESSED2_P5;
				ce = IDB_CLEAR_ENABLE2_P5;	cd = IDB_CLEAR_DISABLE2_P5;		cp = IDB_CLEAR_PRESSED2_P5;
			}
			else if ( i == 5 ) {
				ae = IDB_ADD_ENABLE2_P6;	ad = IDB_ADD_DISABLE2_P6;		ap = IDB_ADD_PRESSED2_P6;
				ce = IDB_CLEAR_ENABLE2_P6;	cd = IDB_CLEAR_DISABLE2_P6;		cp = IDB_CLEAR_PRESSED2_P6;
			}
			else if ( i == 6 ) {
				ae = IDB_ADD_ENABLE2_P7;	ad = IDB_ADD_DISABLE2_P7;		ap = IDB_ADD_PRESSED2_P7;
				ce = IDB_CLEAR_ENABLE2_P7;	cd = IDB_CLEAR_DISABLE2_P7;		cp = IDB_CLEAR_PRESSED2_P7;
			}
			else if ( i == 7 ) {
				ae = IDB_ADD_ENABLE2_P8;	ad = IDB_ADD_DISABLE2_P8;		ap = IDB_ADD_PRESSED2_P8;
				ce = IDB_CLEAR_ENABLE2_P8;	cd = IDB_CLEAR_DISABLE2_P8;		cp = IDB_CLEAR_PRESSED2_P8;
			}
			else if ( i == 8 ) {
				ae = IDB_ADD_ENABLE2_P9;	ad = IDB_ADD_DISABLE2_P9;		ap = IDB_ADD_PRESSED2_P9;
				ce = IDB_CLEAR_ENABLE2_P9;	cd = IDB_CLEAR_DISABLE2_P9;		cp = IDB_CLEAR_PRESSED2_P9;
			}
			else if ( i == 9 ) {
				ae = IDB_ADD_ENABLE2_P10;	ad = IDB_ADD_DISABLE2_P10;		ap = IDB_ADD_PRESSED2_P10;
				ce = IDB_CLEAR_ENABLE2_P10;	cd = IDB_CLEAR_DISABLE2_P10;	cp = IDB_CLEAR_PRESSED2_P10;
			}
			else if ( i == 10 ) {
				ae = IDB_ADD_ENABLE2_P11;	ad = IDB_ADD_DISABLE2_P11;		ap = IDB_ADD_PRESSED2_P11;
				ce = IDB_CLEAR_ENABLE2_P11;	cd = IDB_CLEAR_DISABLE2_P11;	cp = IDB_CLEAR_PRESSED2_P11;
			}
			else if ( i == 11 ) {
				ae = IDB_ADD_ENABLE2_P12;	ad = IDB_ADD_DISABLE2_P12;		ap = IDB_ADD_PRESSED2_P12;
				ce = IDB_CLEAR_ENABLE2_P12;	cd = IDB_CLEAR_DISABLE2_P12;	cp = IDB_CLEAR_PRESSED2_P12;
			}
			xs = 38;	ys = 33;
		}
		//
		h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , P_XA[F_I][i] , P_YA[F_I][i] , xs , ys , hdlg , (HMENU) ae , GETHINST( hdlg ) , NULL );	h_button_count++;
		h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , P_XA[F_I][i] , P_YA[F_I][i] , xs , ys , hdlg , (HMENU) ad , GETHINST( hdlg ) , NULL );	h_button_count++;
		h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , P_XA[F_I][i] , P_YA[F_I][i] , xs , ys , hdlg , (HMENU) ap , GETHINST( hdlg ) , NULL );	h_button_count++;
		//
		h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , P_XC[F_I][i] , P_YC[F_I][i] , xs , ys , hdlg , (HMENU) ce , GETHINST( hdlg ) , NULL );	h_button_count++;
		h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , P_XC[F_I][i] , P_YC[F_I][i] , xs , ys , hdlg , (HMENU) cd , GETHINST( hdlg ) , NULL );	h_button_count++;
		h_button[h_button_count] = CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , P_XC[F_I][i] , P_YC[F_I][i] , xs , ys , hdlg , (HMENU) cp , GETHINST( hdlg ) , NULL );	h_button_count++;
	}
}

//
void FILE_HANDLING_BUTTON_MAPPING( HWND hdlg , int id , int Enable ) {
	long index1 , index2 , index3;
	switch ( id ) {
		case BUTTON_NEW :		index1 = IDB_NEW_ENABLE;	index2 = IDB_NEW_DISABLE;		index3 = IDB_NEW_PRESSED;		break;
		case BUTTON_SAVE :		index1 = IDB_SAVE_ENABLE;	index2 = IDB_SAVE_DISABLE;		index3 = IDB_SAVE_PRESSED;		break;
		case BUTTON_SAVEAS :	index1 = IDB_SAVEAS_ENABLE;	index2 = IDB_SAVEAS_DISABLE;	index3 = IDB_SAVEAS_PRESSED;		break;
		case BUTTON_CANCEL :	index1 = IDB_CANCEL_ENABLE;	index2 = IDB_CANCEL_DISABLE;	index3 = IDB_CANCEL_PRESSED;		break;
		case BUTTON_FILE :		index1 = IDB_FILE_ENABLE;	index2 = IDB_FILE_DISABLE;		index3 = IDB_FILE_PRESSED;		break;
	}
	if ( Enable == 1 ) {
		KWIN_Item_Enable( hdlg , index1 );
		KWIN_Item_Hide( hdlg , index2 );
		KWIN_Item_Hide( hdlg , index3 );
	}
	else if ( Enable == 0 ) {
		KWIN_Item_Disable( hdlg , index2 );
		KWIN_Item_Hide( hdlg , index1 );
		KWIN_Item_Hide( hdlg , index3 );
	}
	else if ( Enable == 2 ) {
		KWIN_Item_Disable( hdlg , index3 );
		KWIN_Item_Hide( hdlg , index1 );
		KWIN_Item_Hide( hdlg , index2 );
	}
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL CHAMBER_EXCLUSIVE_ENABLE( int insertch , int selcl ) {
	int i , j , k , r , r1 , r2 , imsi;
	if ( ( PM_EX_COUNT <= 0 ) && ( PM_PX_COUNT <= 0 ) ) return TRUE;
	r1 = 0;
	r2 = 0;
	for ( i = 0 ; i < PM_EX_COUNT ; i++ ) {
		r = 0;
		if ( PM_EX_STATUS[i][insertch] != 0 ) {
			r = 1;
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				if ( ( insertch != j ) && ( PM_EX_STATUS[i][j] != 0 ) ) {
					r = 2;
					break;
				}
			}
		}
		if      ( r == 1 ) r1 = 1;
		else if ( r == 2 ) r2 = 1;

	}
	if ( r1 ) {
		imsi = 0;
		if ( selcl >= 0 ) {
			if ( CLUSTER_RECIPE.MODULE[selcl][insertch] ) {
				imsi = CLUSTER_RECIPE.MODULE[selcl][insertch];
				CLUSTER_RECIPE.MODULE[selcl][insertch] = 0;
			}
		}
		for ( k = 0 ; k < USE_CLUSTERDATA_ITEM_COUNT ; k++ ) {
			if ( CLUSTER_RECIPE.MODULE[k][insertch] > 0 ) {
				if ( selcl >= 0 ) {
					CLUSTER_RECIPE.MODULE[selcl][insertch] = imsi;
				}
				return FALSE;
			}
		}
		if ( selcl >= 0 ) {
			CLUSTER_RECIPE.MODULE[selcl][insertch] = imsi;
		}
	}
	if ( r2 ) {
		for ( i = 0 ; i < PM_EX_COUNT ; i++ ) {
			if ( PM_EX_STATUS[i][insertch] != 0 ) {
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					if ( ( insertch != j ) && ( PM_EX_STATUS[i][j] != 0 ) ) {
						for ( k = 0 ; k < USE_CLUSTERDATA_ITEM_COUNT ; k++ ) {
							if ( CLUSTER_RECIPE.MODULE[k][j] > 0 ) return FALSE;
						}
					}
				}
			}
		}
	}
	if ( !Get_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT() ) return TRUE;
	if ( selcl < 0 ) return TRUE;

	for ( i = 0 ; i < PM_PX_COUNT ; i++ ) {
		if ( PM_PX_STATUS[i][insertch] != 0 ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				if ( ( insertch != j ) && ( PM_PX_STATUS[i][j] != 0 ) ) {
					if ( CLUSTER_RECIPE.MODULE[selcl][j] > 0 ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Check_Select_CLUSTERFILE( HWND hdlg ) {
	char Buffer[32];
	GetWindowText( GetDlgItem( hdlg , IDC_CURRENT_CLUSTERFILE ) , Buffer , 31 ); 
	if ( strlen( Buffer ) > 0 ) return TRUE;
	return FALSE;
}
//
BOOL Check_Select_PMFILE( HWND hdlg , int ch ) {
	if ( strlen( Get_Prm_PROCESS_RECIPE( ch ) ) > 0 ) return TRUE;
	return FALSE;
}
//
void Get_Select_CLUSTERFILE( HWND hdlg , char *Buffer , int Count ) {
	GetWindowText( GetDlgItem( hdlg , IDC_CURRENT_CLUSTERFILE ) , Buffer , Count ); 
}
//
void Set_Select_CLUSTERFILE( HWND hdlg , char *Buffer ) {
	KWIN_Item_String_Display( hdlg , IDC_CURRENT_CLUSTERFILE , Buffer );
}
//
void Set_Select_PMFILE( HWND hdlg , int ch , char *Buffer ) {
	Set_Prm_PROCESS_RECIPE( ch , Buffer );
}
//
char *Get_Select_PMFILE( HWND hdlg , int ch ) {
	return( Get_Prm_PROCESS_RECIPE( ch ) );
}
//
//
//
//

void Set_Enable_CLUSTERFILE( HWND hdlg , BOOL data ) {
	if ( data ) {
		KWIN_Item_Enable( hdlg , IDC_CTRL_RELOAD_CLUSTER );
		//===============================================
		if ( Get_GROUP_RUN_CHECK() ) {
			KWIN_Item_Enable( hdlg , IDC_LIST_RECIPEDIR );
		}
		//===============================================
		KWIN_Item_Enable( hdlg , IDC_LIST_CLUSTERFILE );
	}
	else {
		KWIN_Item_Disable( hdlg , IDC_CTRL_RELOAD_CLUSTER );
		//===============================================
		if ( Get_GROUP_RUN_CHECK() ) {
			KWIN_Item_Disable( hdlg , IDC_LIST_RECIPEDIR );
		}
		//===============================================
		KWIN_Item_Disable( hdlg , IDC_LIST_CLUSTERFILE );
	}
}
//
void Set_Enable2_PROCESSFILE( HWND hdlg , BOOL data ) {

	if ( !Get_Prm_MODULE_MODE( 0 ) ) {
		KWIN_Item_Hide( hdlg , IDC_CTRL_RELOAD_PM1 );
		KWIN_Item_Hide( hdlg , IDC_LIST_PM1FILE );
		//
		KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM1 );
		//
	}
	else if ( data ) {
		KWIN_Item_Enable( hdlg , IDC_CTRL_RELOAD_PM1 );
		KWIN_Item_Enable( hdlg , IDC_LIST_PM1FILE );
		//
		if ( strlen( PM_SUB_DIRECTORY[0][PM_SUB_CURRENT_POS[0]] ) > 0 )
			KWIN_Item_Enable( hdlg , IDC_CTRL_CHANGE_PM1 );
		else
			KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM1 );
		//
	}
	else {
		KWIN_Item_Disable( hdlg , IDC_CTRL_RELOAD_PM1 );
		KWIN_Item_Disable( hdlg , IDC_LIST_PM1FILE );
		//
		if ( strlen( PM_SUB_DIRECTORY[0][PM_SUB_CURRENT_POS[0]] ) > 0 )
			KWIN_Item_Enable( hdlg , IDC_CTRL_CHANGE_PM1 );
		else
			KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM1 );
		//
	}
	if ( !Get_Prm_MODULE_MODE( 1 ) ) {
		KWIN_Item_Hide( hdlg , IDC_CTRL_RELOAD_PM2 );
		KWIN_Item_Hide( hdlg , IDC_LIST_PM2FILE );
		//
		KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM2 );
		//
	}
	else if ( data ) {
		KWIN_Item_Enable( hdlg , IDC_CTRL_RELOAD_PM2 );
		KWIN_Item_Enable( hdlg , IDC_LIST_PM2FILE );
		//
		if ( strlen( PM_SUB_DIRECTORY[1][PM_SUB_CURRENT_POS[1]] ) > 0 )
			KWIN_Item_Enable( hdlg , IDC_CTRL_CHANGE_PM2 );
		else
			KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM2 );
		//
	}
	else {
		KWIN_Item_Disable( hdlg , IDC_CTRL_RELOAD_PM2 );
		KWIN_Item_Disable( hdlg , IDC_LIST_PM2FILE );
		//
		if ( strlen( PM_SUB_DIRECTORY[1][PM_SUB_CURRENT_POS[1]] ) > 0 )
			KWIN_Item_Enable( hdlg , IDC_CTRL_CHANGE_PM2 );
		else
			KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM2 );
		//
	}
	if ( !Get_Prm_MODULE_MODE( 2 ) ) {
		KWIN_Item_Hide( hdlg , IDC_CTRL_RELOAD_PM3 );
		KWIN_Item_Hide( hdlg , IDC_LIST_PM3FILE );
		//
		KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM3 );
		//
	}
	else if ( data ) {
		KWIN_Item_Enable( hdlg , IDC_CTRL_RELOAD_PM3 );
		KWIN_Item_Enable( hdlg , IDC_LIST_PM3FILE );
		//
		if ( strlen( PM_SUB_DIRECTORY[2][PM_SUB_CURRENT_POS[2]] ) > 0 )
			KWIN_Item_Enable( hdlg , IDC_CTRL_CHANGE_PM3 );
		else
			KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM3 );
		//
	}
	else {
		KWIN_Item_Disable( hdlg , IDC_CTRL_RELOAD_PM3 );
		KWIN_Item_Disable( hdlg , IDC_LIST_PM3FILE );
		//
		if ( strlen( PM_SUB_DIRECTORY[2][PM_SUB_CURRENT_POS[2]] ) > 0 )
			KWIN_Item_Enable( hdlg , IDC_CTRL_CHANGE_PM3 );
		else
			KWIN_Item_Hide( hdlg , IDC_CTRL_CHANGE_PM3 );
		//
	}
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void ClusterData_Insert( int index ) {
	int i , j;
	for ( i = USE_CLUSTERDATA_ITEM_COUNT - 1 ; i >= (index+1) ; i-- ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE.MODULE[i][j] = CLUSTER_RECIPE.MODULE[i-1][j];
			strncpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] , CLUSTER_RECIPE.MODULE_PR_RECIPE[i-1][j] , MAX_STRING_SIZE );
			strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] , CLUSTER_RECIPE.MODULE_IN_RECIPE[i-1][j] , MAX_STRING_SIZE );
			strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] ,CLUSTER_RECIPE.MODULE_OUT_RECIPE[i-1][j] , MAX_STRING_SIZE );
		}
	}
	for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		CLUSTER_RECIPE.MODULE[index][j] = 0;
		strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[index][j] , "" );
		strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[index][j] , "" );
		strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[index][j] , "" );
	}
}
//-------------------------------------------------------------------------
void ClusterData_Insert_for_MaxView( HWND hdlg , int index , int type , BOOL InsertOrder ) {
	int i , j;
	if ( InsertOrder ) {
		for ( i = USE_CLUSTERDATA_ITEM_COUNT - 1 ; i >= (index+1) ; i-- ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				CLUSTER_RECIPE.MODULE[i][j] = CLUSTER_RECIPE.MODULE[i-1][j];
				strncpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] , CLUSTER_RECIPE.MODULE_PR_RECIPE[i-1][j] , MAX_STRING_SIZE );
				strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] , CLUSTER_RECIPE.MODULE_IN_RECIPE[i-1][j] , MAX_STRING_SIZE );
				strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] ,CLUSTER_RECIPE.MODULE_OUT_RECIPE[i-1][j] , MAX_STRING_SIZE );
			}
		}
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE.MODULE[index][j] = 0;
			strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[index][j] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[index][j] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[index][j] , "" );
		}
		CLUSTER_RECIPE.MODULE[index][MAX_VIEW_CHAMBER] = 1;
		switch( type ) {
		case 0 :
			strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[index][MAX_VIEW_CHAMBER] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[index][MAX_VIEW_CHAMBER] , Get_Select_PMFILE( hdlg , MAX_VIEW_CHAMBER ) );
			strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[index][MAX_VIEW_CHAMBER] , "" );
			break;
		case 1 :
			strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[index][MAX_VIEW_CHAMBER] , Get_Select_PMFILE( hdlg , MAX_VIEW_CHAMBER ) );
			strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[index][MAX_VIEW_CHAMBER] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[index][MAX_VIEW_CHAMBER] , "" );
			break;
		case 2 :
			strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[index][MAX_VIEW_CHAMBER] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[index][MAX_VIEW_CHAMBER] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[index][MAX_VIEW_CHAMBER] , Get_Select_PMFILE( hdlg , MAX_VIEW_CHAMBER ) );
			break;
		}
	}
	else {
		CLUSTER_RECIPE.MODULE[index][MAX_VIEW_CHAMBER] = 1;
		switch( type ) {
		case 0 :
			strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[index][MAX_VIEW_CHAMBER] , Get_Select_PMFILE( hdlg , MAX_VIEW_CHAMBER ) );
			break;
		case 1 :
			strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[index][MAX_VIEW_CHAMBER] , Get_Select_PMFILE( hdlg , MAX_VIEW_CHAMBER ) );
			break;
		case 2 :
			strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[index][MAX_VIEW_CHAMBER] , Get_Select_PMFILE( hdlg , MAX_VIEW_CHAMBER ) );
			break;
		}
	}
}
//-------------------------------------------------------------------------
void ClusterData_Remapping() {
	int i , j , k;

	for ( i = 0 ; i < USE_CLUSTERDATA_ITEM_COUNT ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( CLUSTER_RECIPE.MODULE[i][j] > 0 ) {
				if ( strlen( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] ) <= 0 && strlen( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] ) <= 0 && strlen( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] ) <= 0 ) {
					CLUSTER_RECIPE.MODULE[i][j] = 0;
				}
			}
			else {
				strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] , "" );
				strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] , "" );
				strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] , "" );
			}
		}
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( CLUSTER_RECIPE.MODULE[i][j] > 0 ) break;
		}
		if ( j == MAX_PM_CHAMBER_DEPTH ) {
			if ( (i+1) < USE_CLUSTERDATA_ITEM_COUNT ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					CLUSTER_RECIPE.MODULE[i][k] = CLUSTER_RECIPE.MODULE[i+1][k];
					strncpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][k] , CLUSTER_RECIPE.MODULE_PR_RECIPE[i+1][k] , MAX_STRING_SIZE );
					strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][k] , CLUSTER_RECIPE.MODULE_IN_RECIPE[i+1][k] , MAX_STRING_SIZE );
					strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][k] ,CLUSTER_RECIPE.MODULE_OUT_RECIPE[i+1][k] , MAX_STRING_SIZE );
				}
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					CLUSTER_RECIPE.MODULE[i+1][k] = 0;
					strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i+1][k] , "" );
					strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i+1][k] , "" );
					strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i+1][k] , "" );
				}
			}
		}
	}
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int ClusterData_ListView_SelectCount( HWND hWnd ) {
	return( ListView_GetSelectedCount( hWnd ) );
}
//
int ClusterData_ListView_SelectData( HWND hWnd , int no ) {
	return( ListView_GetNextItem( hWnd , no , LVNI_SELECTED ) );
}
//
BOOL ClusterData_ListView_Enable( HWND hWnd ) {
	int i , j , k;
	k = ClusterData_ListView_SelectData( hWnd , -1 );
	if ( k < 0 ) return FALSE;
	for ( i = USE_CLUSTERDATA_ITEM_COUNT - 1 ; i >= 0 ; i-- ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( CLUSTER_RECIPE.MODULE[i][j] > 0 ) break;
		}
		if ( j != MAX_PM_CHAMBER_DEPTH ) {
			if ( i == USE_CLUSTERDATA_ITEM_COUNT - 1 ) return FALSE;
			break;
		}
	}
	i++;
	if ( ClusterData_ListView_SelectCount( hWnd ) >  1 ) return FALSE;
	else if ( ( k / Get_CLUSTER_USE_TYPE_ITEM() ) >= i ) return FALSE;
	while ( k >= 0 ) {
		if ( ( k / Get_CLUSTER_USE_TYPE_ITEM() ) > i ) return FALSE;
		k = ClusterData_ListView_SelectData( hWnd , k );
	}
	return TRUE;
}
//
BOOL ClusterData_ListView_Enable_Mdl( HWND hWnd ) {
	int i , j , k;
	k = ClusterData_ListView_SelectData( hWnd , -1 );
	if ( k < 0 ) return FALSE;
	for ( i = USE_CLUSTERDATA_ITEM_COUNT - 1 ; i >= 0 ; i-- ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( CLUSTER_RECIPE.MODULE[i][j] > 0 ) break;
		}
		if ( j != MAX_PM_CHAMBER_DEPTH ) break;
	}
	i++;
	if ( ClusterData_ListView_SelectCount( hWnd ) > 1 ) return FALSE;
	else if ( ( k / Get_CLUSTER_USE_TYPE_ITEM() ) > i ) return FALSE;
	return TRUE;
}
//
BOOL ClusterData_ListView_Remove( HWND hWnd ) {
	int j , k;
	k = ClusterData_ListView_SelectData( hWnd , -1 );
	if ( k < 0 ) return FALSE;
	for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		if ( CLUSTER_RECIPE.MODULE[k][j] > 0 ) break;
	}
	if ( j == MAX_PM_CHAMBER_DEPTH ) return FALSE;
	return TRUE;
}
//
//
BOOL ClusterData_ListView_SetData( HWND hWnd ) {
	LV_ITEM lvi;
	int i;
	while( TRUE ) {
		if ( !ListView_DeleteItem( hWnd , 0 ) ) break;
	}
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	for ( i = 0 ; i < ( USE_CLUSTERDATA_ITEM_COUNT * Get_CLUSTER_USE_TYPE_ITEM() ); i++ ) {
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.pszText		= LPSTR_TEXTCALLBACK;
		lvi.cchTextMax	= 0;
		lvi.iImage		= i;
		if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	}
	return TRUE;
}
//

//////////////////////////////////////////////////////////////////////////
//... 2014.10.15 Add Aligner Use Option & PostPostion 
void EXTRA_STATION_MAPPING( HWND hdlg ) {

	printf("[DEBUG] EXTRA_STATION_MAPPING ... EXTRA_STATION[%d]\n", CLUSTER_RECIPE.EXTRA_STATION);

	switch(CLUSTER_RECIPE.EXTRA_STATION) 
	{
	case 10 : KWIN_Item_String_Display( hdlg , IDC_CTRL_AL_MODE , "Wafer Align");		break;
	case 50 : KWIN_Item_String_Display( hdlg , IDC_CTRL_AL_MODE , "Align Skip");		break;
	default:
		CLUSTER_RECIPE.EXTRA_STATION = 10;
		KWIN_Item_String_Display( hdlg , IDC_CTRL_AL_MODE , "Wafer Align" );
		break;
	}
}

//
void Process_Recipe_Directory_Refresh( HWND hdlg , int mode ) {
	if ( mode >= 0 ) 
	{
		USE_SELECT_CURRENT_PM_TYPE = mode;

		if ( PM_LOW_CONTROL[0] ) {
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM1FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 0 ) , PM_SUB_DIRECTORY[0][ PM_SUB_CURRENT_POS[0] ] , PM_LOW_DIRECTORY[0][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[0][USE_SELECT_CURRENT_PM_TYPE] );
			Set_Select_PMFILE( hdlg , 0 , "" );
		}
		if ( PM_LOW_CONTROL[1] ) {
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM2FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 1 ) , PM_SUB_DIRECTORY[1][ PM_SUB_CURRENT_POS[1] ] , PM_LOW_DIRECTORY[1][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[1][USE_SELECT_CURRENT_PM_TYPE] );
			Set_Select_PMFILE( hdlg , 1 , "" );
		}
		if ( PM_LOW_CONTROL[2] ) {
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM3FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 2 ) , PM_SUB_DIRECTORY[2][ PM_SUB_CURRENT_POS[2] ] , PM_LOW_DIRECTORY[2][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[2][USE_SELECT_CURRENT_PM_TYPE] );
			Set_Select_PMFILE( hdlg , 2 , "" );
		}
	}
}
//
void PROCESS_SEND_BUTTON_MAPPING( HWND hdlg , int ch , int id , int Enable ) {
	int i;
	BOOL Find;
	long index1 , index2 , index3;

	if ( Get_Prm_DRAW_SIZE() <= 4 ) 
	{
		switch ( ch ) {
		case 102 :
		case 103 :
			index1 = IDB_SELECT_RCP_PR;	index2 = IDB_SELECT_RCP_MAIN;	index3 = IDB_SELECT_RCP_PO;
			break;
			//
		case 0 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P1;		index2 = IDB_ADD_DISABLE_P1;	index3 = IDB_ADD_PRESSED_P1;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P1;	index2 = IDB_CLEAR_DISABLE_P1;	index3 = IDB_CLEAR_PRESSED_P1;
			}
			break;
		case 1 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P2;		index2 = IDB_ADD_DISABLE_P2;	index3 = IDB_ADD_PRESSED_P2;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P2;	index2 = IDB_CLEAR_DISABLE_P2;	index3 = IDB_CLEAR_PRESSED_P2;
			}
			break;
		case 2 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P3;		index2 = IDB_ADD_DISABLE_P3;	index3 = IDB_ADD_PRESSED_P3;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P3;	index2 = IDB_CLEAR_DISABLE_P3;	index3 = IDB_CLEAR_PRESSED_P3;
			}
			break;
		case 3 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P4;		index2 = IDB_ADD_DISABLE_P4;	index3 = IDB_ADD_PRESSED_P4;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P4;	index2 = IDB_CLEAR_DISABLE_P4;	index3 = IDB_CLEAR_PRESSED_P4;
			}
			break;
		case 4 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P5;		index2 = IDB_ADD_DISABLE_P5;	index3 = IDB_ADD_PRESSED_P5;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P5;	index2 = IDB_CLEAR_DISABLE_P5;	index3 = IDB_CLEAR_PRESSED_P5;
			}
			break;
		case 5 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P6;		index2 = IDB_ADD_DISABLE_P6;	index3 = IDB_ADD_PRESSED_P6;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P6;	index2 = IDB_CLEAR_DISABLE_P6;	index3 = IDB_CLEAR_PRESSED_P6;
			}
			break;
		case 6 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P7;		index2 = IDB_ADD_DISABLE_P7;	index3 = IDB_ADD_PRESSED_P7;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P7;	index2 = IDB_CLEAR_DISABLE_P7;	index3 = IDB_CLEAR_PRESSED_P7;
			}
			break;
		case 7 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P8;		index2 = IDB_ADD_DISABLE_P8;	index3 = IDB_ADD_PRESSED_P8;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P8;	index2 = IDB_CLEAR_DISABLE_P8;	index3 = IDB_CLEAR_PRESSED_P8;
			}
			break;
		case 8 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P9;		index2 = IDB_ADD_DISABLE_P9;	index3 = IDB_ADD_PRESSED_P9;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P9;	index2 = IDB_CLEAR_DISABLE_P9;	index3 = IDB_CLEAR_PRESSED_P9;
			}
			break;
		case 9 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P10;	index2 = IDB_ADD_DISABLE_P10;	index3 = IDB_ADD_PRESSED_P10;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P10;	index2 = IDB_CLEAR_DISABLE_P10;	index3 = IDB_CLEAR_PRESSED_P10;
			}
			break;
		case 10 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P11;	index2 = IDB_ADD_DISABLE_P11;	index3 = IDB_ADD_PRESSED_P11;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P11;	index2 = IDB_CLEAR_DISABLE_P11;	index3 = IDB_CLEAR_PRESSED_P11;
			}
			break;
		case 11 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE_P12;	index2 = IDB_ADD_DISABLE_P12;	index3 = IDB_ADD_PRESSED_P12;
			}
			else {
				index1 = IDB_CLEAR_ENABLE_P12;	index2 = IDB_CLEAR_DISABLE_P12;	index3 = IDB_CLEAR_PRESSED_P12;
			}
			break;
		}
	}
	else 
	{
		switch ( ch ) {
		case 102 :
		case 103 :
			index1 = IDB_SELECT_RCP_PR;	index2 = IDB_SELECT_RCP_MAIN;	index3 = IDB_SELECT_RCP_PO;
			break;
			//
		case 0 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P1;	index2 = IDB_ADD_DISABLE2_P1;		index3 = IDB_ADD_PRESSED2_P1;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P1;	index2 = IDB_CLEAR_DISABLE2_P1;	index3 = IDB_CLEAR_PRESSED2_P1;
			}
			break;
		case 1 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P2;	index2 = IDB_ADD_DISABLE2_P2;		index3 = IDB_ADD_PRESSED2_P2;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P2;	index2 = IDB_CLEAR_DISABLE2_P2;	index3 = IDB_CLEAR_PRESSED2_P2;
			}
			break;
		case 2 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P3;	index2 = IDB_ADD_DISABLE2_P3;		index3 = IDB_ADD_PRESSED2_P3;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P3;	index2 = IDB_CLEAR_DISABLE2_P3;	index3 = IDB_CLEAR_PRESSED2_P3;
			}
			break;
		case 3 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P4;	index2 = IDB_ADD_DISABLE2_P4;		index3 = IDB_ADD_PRESSED2_P4;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P4;	index2 = IDB_CLEAR_DISABLE2_P4;	index3 = IDB_CLEAR_PRESSED2_P4;
			}
			break;
		case 4 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P5;	index2 = IDB_ADD_DISABLE2_P5;		index3 = IDB_ADD_PRESSED2_P5;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P5;	index2 = IDB_CLEAR_DISABLE2_P5;	index3 = IDB_CLEAR_PRESSED2_P5;
			}
			break;
		case 5 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P6;	index2 = IDB_ADD_DISABLE2_P6;		index3 = IDB_ADD_PRESSED2_P6;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P6;	index2 = IDB_CLEAR_DISABLE2_P6;	index3 = IDB_CLEAR_PRESSED2_P6;
			}
			break;
		case 6 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P7;	index2 = IDB_ADD_DISABLE2_P7;		index3 = IDB_ADD_PRESSED2_P7;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P7;	index2 = IDB_CLEAR_DISABLE2_P7;	index3 = IDB_CLEAR_PRESSED2_P7;
			}
			break;
		case 7 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P8;	index2 = IDB_ADD_DISABLE2_P8;		index3 = IDB_ADD_PRESSED2_P8;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P8;	index2 = IDB_CLEAR_DISABLE2_P8;	index3 = IDB_CLEAR_PRESSED2_P8;
			}
			break;
		case 8 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P9;	index2 = IDB_ADD_DISABLE2_P9;		index3 = IDB_ADD_PRESSED2_P9;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P9;	index2 = IDB_CLEAR_DISABLE2_P9;	index3 = IDB_CLEAR_PRESSED2_P9;
			}
			break;
		case 9 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P10;	index2 = IDB_ADD_DISABLE2_P10;		index3 = IDB_ADD_PRESSED2_P10;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P10;	index2 = IDB_CLEAR_DISABLE2_P10;	index3 = IDB_CLEAR_PRESSED2_P10;
			}
			break;
		case 10 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P11;	index2 = IDB_ADD_DISABLE2_P11;		index3 = IDB_ADD_PRESSED2_P11;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P11;	index2 = IDB_CLEAR_DISABLE2_P11;	index3 = IDB_CLEAR_PRESSED2_P11;
			}
			break;
		case 11 :
			if ( id == 0 ) {
				index1 = IDB_ADD_ENABLE2_P12;	index2 = IDB_ADD_DISABLE2_P12;		index3 = IDB_ADD_PRESSED2_P12;
			}
			else {
				index1 = IDB_CLEAR_ENABLE2_P12;	index2 = IDB_CLEAR_DISABLE2_P12;	index3 = IDB_CLEAR_PRESSED2_P12;
			}
			break;
		}
	}

	if ( ( ch == 102 ) || ( ch == 103 ) ) 
	{
		if ( Enable == 1 ) {
			if ( ClusterData_ListView_SelectCount( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) ) != 1 ) {
				KWIN_Item_Hide( hdlg , index1 );
				KWIN_Item_Hide( hdlg , index2 );
				KWIN_Item_Hide( hdlg , index3 );
				if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , -1 );
				return;
			}
			i = ClusterData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , -1 );
			switch ( Get_CLUSTER_USE_TYPE_ITEM() ) {
			case 3 :
				if ( ( i % 3 ) == 0 ) {
					KWIN_Item_Disable( hdlg , index1 );
					KWIN_Item_Hide( hdlg , index2 );
					KWIN_Item_Hide( hdlg , index3 );
					if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , 2 );
				}
				else if ( ( i % 3 ) == 1 ) {
					KWIN_Item_Hide( hdlg , index1 );
					KWIN_Item_Disable( hdlg , index2 );
					KWIN_Item_Hide( hdlg , index3 );
					if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , 0 );
				}
				else {
					KWIN_Item_Hide( hdlg , index1 );
					KWIN_Item_Hide( hdlg , index2 );
					KWIN_Item_Disable( hdlg , index3 );
					if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , 1 );
				}
				break;
			case 2 :
				if ( ( i % 2 ) == 0 ) {
					KWIN_Item_Hide( hdlg , index1 );
					KWIN_Item_Disable( hdlg , index2 );
					KWIN_Item_Hide( hdlg , index3 );
					if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , 0 );
				}
				else {
					KWIN_Item_Hide( hdlg , index1 );
					KWIN_Item_Hide( hdlg , index2 );
					KWIN_Item_Disable( hdlg , index3 );
					if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , 1 );
				}
				break;
			default :
				KWIN_Item_Hide( hdlg , index1 );
				KWIN_Item_Disable( hdlg , index2 );
				KWIN_Item_Hide( hdlg , index3 );
				if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , 0 );
				break;
			}
		}
		else {
			KWIN_Item_Hide( hdlg , index1 );
			KWIN_Item_Hide( hdlg , index2 );
			KWIN_Item_Hide( hdlg , index3 );
			if ( ch == 103 ) Process_Recipe_Directory_Refresh( hdlg , -1 );
		}
	}
	else {
		if ( !Get_Prm_MODULE_MODE( ch ) ) {
			KWIN_Item_Hide( hdlg , index1 );
			KWIN_Item_Hide( hdlg , index2 );
			KWIN_Item_Hide( hdlg , index3 );
			return;
		}
		if ( Enable == 1 ) {
			if ( ClusterData_ListView_SelectCount( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) ) <= 0 ) {
				KWIN_Item_Hide( hdlg , index1 );
				KWIN_Item_Hide( hdlg , index2 );
				KWIN_Item_Hide( hdlg , index3 );
				return;
			}
			if ( id == 0 ) {
				if ( !ClusterData_ListView_Enable_Mdl( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) ) ) {
					KWIN_Item_Hide( hdlg , index1 );
					KWIN_Item_Hide( hdlg , index2 );
					KWIN_Item_Hide( hdlg , index3 );
					return;
				}
				if ( !Check_Select_PMFILE( hdlg , ch ) ) {
					KWIN_Item_Hide( hdlg , index1 );
					KWIN_Item_Hide( hdlg , index2 );
					KWIN_Item_Hide( hdlg , index3 );
					return;
				}
			}
			else {
				Find = FALSE;
				i = ClusterData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , -1 );
				while ( ( i >= 0 ) && (!Find) ) {
					switch ( Get_CLUSTER_USE_TYPE_ITEM() ) {
					case 3 :
						if ( ( i % 3 ) == 0 ) {
							if ( strlen( CLUSTER_RECIPE.MODULE_PR_RECIPE[i/3][ch] ) > 0 ) Find = TRUE;
						}
						else if ( ( i % 3 ) == 1 ) {
							if ( strlen( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/3][ch] ) > 0 ) Find = TRUE;
						}
						else {
							if ( strlen( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/3][ch] ) > 0 ) Find = TRUE;
						}
						break;
					case 2 :
						if ( ( i % 2 ) == 0 ) {
							if ( strlen( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/2][ch] ) > 0 ) Find = TRUE;
						}
						else {
							if ( strlen( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/2][ch] ) > 0 ) Find = TRUE;
						}
						break;
					default :
						if ( strlen( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/1][ch] ) > 0 ) Find = TRUE;
						break;
					}
					i = ClusterData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , i );
				}
				if ( !Find ) {
					KWIN_Item_Hide( hdlg , index1 );
					KWIN_Item_Hide( hdlg , index2 );
					KWIN_Item_Hide( hdlg , index3 );
					return;
				}
			}
			KWIN_Item_Enable( hdlg , index1 );
			KWIN_Item_Hide( hdlg , index2 );
			KWIN_Item_Hide( hdlg , index3 );
		}
		else if ( Enable == 0 ) {
			KWIN_Item_Hide( hdlg , index1 );
			KWIN_Item_Hide( hdlg , index2 );
			KWIN_Item_Hide( hdlg , index3 );
		}
		else if ( Enable == 2 ) {
			KWIN_Item_Hide( hdlg , index1 );
			KWIN_Item_Hide( hdlg , index2 );
			KWIN_Item_Disable( hdlg , index3 );
		}
	}
}
//
void ClusterData_Button_Init( HWND hWnd , int mode , BOOL pmtypemode ) {
	if ( mode == 0 ) {

		if ( pmtypemode )	PROCESS_SEND_BUTTON_MAPPING( hWnd , 103 , 0 , 0 );
		else				PROCESS_SEND_BUTTON_MAPPING( hWnd , 102 , 0 , 0 );
		//
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 0 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 0 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 1 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 1 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 2 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 2 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 3 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 3 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 4 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 4 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 5 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 5 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 6 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 6 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 7 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 7 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 8 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 8 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 9 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 9 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 10 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 10 , 1 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 11 , 0 , 0 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 11 , 1 , 0 );
	}
	else if ( mode == 1 ) {
		if ( pmtypemode )	PROCESS_SEND_BUTTON_MAPPING( hWnd , 103 , 0 , 1 );
		else				PROCESS_SEND_BUTTON_MAPPING( hWnd , 102 , 0 , 1 );
		//
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 0 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 0 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 1 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 1 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 2 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 2 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 3 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 3 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 4 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 4 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 5 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 5 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 6 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 6 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 7 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 7 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 8 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 8 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 9 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 9 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 10 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 10 , 1 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 11 , 0 , 1 );
		PROCESS_SEND_BUTTON_MAPPING( hWnd , 11 , 1 , 1 );
	}
}
//
void ClusterData_Button_Set( HWND hWnd , BOOL PMTypeChange ) {
	if ( !Check_Select_CLUSTERFILE( hWnd ) ) {
		ClusterData_Button_Init( hWnd , 0 , PMTypeChange );
	}
	else {
		if ( ClusterData_ListView_SelectData( GetDlgItem( hWnd , IDC_LIST_CLUSTERDATA ) , -1 ) < 0 ) {
			ClusterData_Button_Init( hWnd , 0 , PMTypeChange );
		}
		else {
			ClusterData_Button_Init( hWnd , 1 , PMTypeChange );
		}
	}
}
//---------------------------------------------------------------------------------------
//
BOOL ClusterData_ListView_Init( HWND hwndListView ) {
	int         i;
	LV_COLUMN   lvColumn;
	TCHAR       *szString     = "ID";
	//
	Kor_CLUSTER_Initial();
	//
	ListView_DeleteAllItems( hwndListView );
	//---------------------------------------------------
	for ( i = 0 ; i < 16 ; i++ ) ListView_DeleteColumn( hwndListView , 0 ); // 2004.04.06
	//---------------------------------------------------
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;

	printf("[DEBUG] Get_Prm_DRAW_SIZE[%d]\n", Get_Prm_DRAW_SIZE());

	for ( i = 0 ; i < ( Get_Prm_DRAW_SIZE() + 1 ) ; i++ ) {
		switch( Get_Prm_DRAW_SIZE() ) {
		case 4 :
			lvColumn.cx = LV_Size_4_M[i];
			break;
		case 6 :
			lvColumn.cx = LV_Size_6_M[i];
			break;
		case 8 :	lvColumn.cx = LV_Size_8[i];		break;
		case 10 :	lvColumn.cx = LV_Size_10[i];	break;
		case 12 :	lvColumn.cx = LV_Size_12[i];	break;
		}
		if ( i == 0 ) lvColumn.pszText = szString;
		else          lvColumn.pszText = Get_Prm_PROCESS_NAME( i - 1 );
		ListView_InsertColumn( hwndListView , i , &lvColumn );
	}	
	ListView_DeleteAllItems( hwndListView );
	ClusterData_ListView_SetData( hwndListView );
	return TRUE;
}
//
//
LRESULT ClusterData_ListView_Notify( HWND hWnd , LPARAM lParam ) {
	LPNMHDR  lpnmh = (LPNMHDR) lParam;
	int sel;
	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: {
				LV_DISPINFO *lpdi = (LV_DISPINFO *)lParam;
				TCHAR szString[ 256 ];
				if ( lpdi->item.iSubItem ) {
					if ( lpdi->item.mask & LVIF_TEXT ) {
						if ( lpdi->item.iSubItem >= 1 && lpdi->item.iSubItem <= MAX_PM_CHAMBER_DEPTH ) {
							switch ( Get_CLUSTER_USE_TYPE_ITEM() ) {
							case 3 :
								sel = CLUSTER_RECIPE.MODULE[lpdi->item.iItem/3][lpdi->item.iSubItem-1];
								if ( sel ) {
									if ( (lpdi->item.iItem % 3 ) == 0 ) {
										if ( strlen( CLUSTER_RECIPE.MODULE_PR_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] ) > 0 ) {
											if ( strlen( PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] ) > 0 )
												wsprintf( szString , "<%s>[%s]" , PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] , CLUSTER_RECIPE.MODULE_PR_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] );
											else
												wsprintf( szString , "[%s]" , CLUSTER_RECIPE.MODULE_PR_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] );
										}
										else
											wsprintf( szString, "--------" );
									}
									else if ( (lpdi->item.iItem % 3 ) == 1 ) {
										if ( strlen( CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] ) > 0 ) {
											if ( strlen( PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] ) > 0 )
												wsprintf( szString , "<%s>%s" , PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] , CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] );
											else
												wsprintf( szString , "%s" , CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] );
										}
										else
											wsprintf( szString, "--------" );
									}
									else {
										if ( strlen( CLUSTER_RECIPE.MODULE_OUT_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] ) > 0 ) {
											if ( strlen( PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] ) > 0 )
												wsprintf( szString , "<%s>(%s)" , PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] , CLUSTER_RECIPE.MODULE_OUT_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] );
											else
												wsprintf( szString , "(%s)" , CLUSTER_RECIPE.MODULE_OUT_RECIPE[lpdi->item.iItem/3][lpdi->item.iSubItem-1] );
										}
										else
											wsprintf( szString, "" );
									}
								}
								else {
									wsprintf( szString, "" );
								}
								break;
							case 2 :
								sel = CLUSTER_RECIPE.MODULE[lpdi->item.iItem/2][lpdi->item.iSubItem-1];
								if ( sel ) {
									if ( (lpdi->item.iItem % 2 ) == 0 ) {
										if ( strlen( CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/2][lpdi->item.iSubItem-1] ) > 0 ) {
											if ( strlen( PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] ) > 0 )
												wsprintf( szString , "<%s>%s" , PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] , CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/2][lpdi->item.iSubItem-1] );
											else
												wsprintf( szString , "%s" , CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/2][lpdi->item.iSubItem-1] );
										}
										else
											wsprintf( szString, "--------" );
									}
									else {
										if ( strlen( CLUSTER_RECIPE.MODULE_OUT_RECIPE[lpdi->item.iItem/2][lpdi->item.iSubItem-1] ) > 0 ) {
											if ( strlen( PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] ) > 0 )
												wsprintf( szString , "<%s>(%s)" , PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] , CLUSTER_RECIPE.MODULE_OUT_RECIPE[lpdi->item.iItem/2][lpdi->item.iSubItem-1] );
											else
												wsprintf( szString , "(%s)" , CLUSTER_RECIPE.MODULE_OUT_RECIPE[lpdi->item.iItem/2][lpdi->item.iSubItem-1] );
										}
										else
											wsprintf( szString, "" );
									}
								}
								else {
									wsprintf( szString, "" );
								}
								break;
							default :
								sel = CLUSTER_RECIPE.MODULE[lpdi->item.iItem/1][lpdi->item.iSubItem-1];
								if ( sel ) {
									if ( strlen( CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/1][lpdi->item.iSubItem-1] ) > 0 ) {
										if ( strlen( PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] ) > 0 )
											wsprintf( szString , "<%s>%s" , PM_SUB_DIRECTORY[lpdi->item.iSubItem-1][sel-1] , CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/1][lpdi->item.iSubItem-1] );
										else
											wsprintf( szString , "%s" , CLUSTER_RECIPE.MODULE_IN_RECIPE[lpdi->item.iItem/1][lpdi->item.iSubItem-1] );
									}
									else
										wsprintf( szString, "--------" );
								}
								else {
									wsprintf( szString, "" );
								}
								break;
							}
						}
						else {
							wsprintf( szString, "" );
						}
						lstrcpy( lpdi->item.pszText , szString );
					}
				}
				else {
					if ( lpdi->item.mask & LVIF_TEXT ) {
						if ( (lpdi->item.iItem % Get_CLUSTER_USE_TYPE_ITEM() ) == 0 )
							wsprintf( szString, "%2d", (lpdi->item.iItem/Get_CLUSTER_USE_TYPE_ITEM()) + 1 );
						else
							wsprintf( szString, "" );
						lstrcpy( lpdi->item.pszText , szString );
					}
					if ( lpdi->item.mask & LVIF_IMAGE ) {
						lpdi->item.iImage = 0;
					}
				}
		}
		return 0;

		case NM_DBLCLK :
			return 0;

		case LVN_ITEMCHANGED :
			ClusterData_Button_Set( GetParent(hWnd) , TRUE );
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
//
//
//
#define	DISPLAY_MAX_VIEW_UP2_X				850
#define	DISPLAY_MAX_VIEW_UP_X				850
#define	DISPLAY_MAX_VIEW_DN_X				850
#define	DISPLAY_MAX_VIEW_DN2_X				850
#define	DISPLAY_MAX_VIEW_UD_SIZE_X			33
#define	DISPLAY_MAX_VIEW_UD_SIZE_Y			33
//
int		DISPLAY_MAX_VIEW_UP2_Y	=	85;
int		DISPLAY_MAX_VIEW_UP_Y	=	135;
int		DISPLAY_MAX_VIEW_DN_Y	=	185;
int		DISPLAY_MAX_VIEW_DN2_Y	=	235;
//
int		DISPLAY_MAX_VIEW_START_Y	= 70;
//
#define	DISPLAY_MAX_VIEW_INTERVAL_Y			30
//
#define	DISPLAY_MAX_VIEW_SELECTOR_START_X	200
//
#define	DISPLAY_MAX_VIEW_ID_START_X			224
#define	DISPLAY_MAX_VIEW_ID_STRING_SIZE		4
//
#define	DISPLAY_MAX_VIEW_MDL_START_X		262
#define	DISPLAY_MAX_VIEW_MDL_STRING_SIZE	15
//
#define	DISPLAY_MAX_VIEW_MODE_START_X		388
#define	DISPLAY_MAX_VIEW_MODE_STRING_SIZE	6
//
#define	DISPLAY_MAX_VIEW_RECIPE_START_X		442
#define	DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE	50
//
void ClusterData_MaxView_Draw_Select_Pointer( HWND hdlg , HDC hDC ) {
	int i;
	for ( i = 0 ; i < DISPLAY_MAX_VIEW_COUNT ; i++ ) {
		if      ( MAX_VIEW_SELECT_POINTER == i+MAX_VIEW_START_POINTER ) {
			KWIN_DRAW_BITMAP_PAINT_WITH_TRANSPARENT( GetModuleHandle(NULL) , hDC , MAKEINTRESOURCE( IDB_ARROW_ON ) , MAKEINTRESOURCE( IDB_ARROW_ON_X ) , DISPLAY_MAX_VIEW_SELECTOR_START_X , DISPLAY_MAX_VIEW_START_Y + ( i * (DISPLAY_MAX_VIEW_INTERVAL_Y) ) );
		}
		else {
			KWIN_DRAW_BITMAP_PAINT_WITH_TRANSPARENT( GetModuleHandle(NULL) , hDC , MAKEINTRESOURCE( IDB_ARROW_OFF ) , MAKEINTRESOURCE( IDB_ARROW_OFF_X ) , DISPLAY_MAX_VIEW_SELECTOR_START_X , DISPLAY_MAX_VIEW_START_Y + ( i * (DISPLAY_MAX_VIEW_INTERVAL_Y) ) );
		}
	}
	KWIN_DRAW_BITMAP_PAINT_WITH_TRANSPARENT( GetModuleHandle(NULL) , hDC , MAKEINTRESOURCE( IDB_ARROW_UP2 )   , MAKEINTRESOURCE( IDB_ARROW_UP2_X )   , DISPLAY_MAX_VIEW_UP2_X , DISPLAY_MAX_VIEW_UP2_Y );
	KWIN_DRAW_BITMAP_PAINT_WITH_TRANSPARENT( GetModuleHandle(NULL) , hDC , MAKEINTRESOURCE( IDB_ARROW_UP )    , MAKEINTRESOURCE( IDB_ARROW_UP_X )    , DISPLAY_MAX_VIEW_UP_X , DISPLAY_MAX_VIEW_UP_Y );
	KWIN_DRAW_BITMAP_PAINT_WITH_TRANSPARENT( GetModuleHandle(NULL) , hDC , MAKEINTRESOURCE( IDB_ARROW_DOWN )  , MAKEINTRESOURCE( IDB_ARROW_DOWN_X )  , DISPLAY_MAX_VIEW_DN_X , DISPLAY_MAX_VIEW_DN_Y );
	KWIN_DRAW_BITMAP_PAINT_WITH_TRANSPARENT( GetModuleHandle(NULL) , hDC , MAKEINTRESOURCE( IDB_ARROW_DOWN2 ) , MAKEINTRESOURCE( IDB_ARROW_DOWN2_X ) , DISPLAY_MAX_VIEW_DN2_X , DISPLAY_MAX_VIEW_DN2_Y );
}
//
void ClusterData_MaxView_LeftDown( HWND hdlg ) {
	int i;
	POINT ptcursor;
	GetCursorPos( &ptcursor );
	ScreenToClient( hdlg , &ptcursor );
	for ( i = 0 ; i < DISPLAY_MAX_VIEW_COUNT ; i++ ) {
		if ( ( ptcursor.x >= DISPLAY_MAX_VIEW_SELECTOR_START_X ) && ( ptcursor.x <= DISPLAY_MAX_VIEW_SELECTOR_START_X + 23 ) &&
			( ptcursor.y >= DISPLAY_MAX_VIEW_START_Y + ( i * (DISPLAY_MAX_VIEW_INTERVAL_Y) ) ) && ( ptcursor.y <= DISPLAY_MAX_VIEW_START_Y + ( i * (DISPLAY_MAX_VIEW_INTERVAL_Y) ) + 29 ) ) {
			if ( MAX_VIEW_SELECT_POINTER != (i+MAX_VIEW_START_POINTER) ) {
				if ( MAX_VIEW_LAST_POINTER >= (i+MAX_VIEW_START_POINTER) ) {
					MAX_VIEW_SELECT_POINTER = i+MAX_VIEW_START_POINTER;
					InvalidateRect( hdlg , NULL , FALSE );
					ClusterData_Button_Set( hdlg , FALSE );
					return;
				}
			}
		}
	}

	if ( ( ptcursor.x >= DISPLAY_MAX_VIEW_UP_X ) && ( ptcursor.x <= DISPLAY_MAX_VIEW_UP_X + DISPLAY_MAX_VIEW_UD_SIZE_X ) &&
		( ptcursor.y >= DISPLAY_MAX_VIEW_UP_Y ) && ( ptcursor.y <= DISPLAY_MAX_VIEW_UP_Y + DISPLAY_MAX_VIEW_UD_SIZE_Y ) ) {
		if ( MAX_VIEW_START_POINTER > 0 ) {
			i = MAX_VIEW_START_POINTER - 1;
			if ( ( i >= 0 ) && ( i < MAX_VIEW_LAST_POINTER ) ) {
				MAX_VIEW_START_POINTER = i;
				InvalidateRect( hdlg , NULL , FALSE );
			}
		}
	}
	if ( ( ptcursor.x >= DISPLAY_MAX_VIEW_UP2_X ) && ( ptcursor.x <= DISPLAY_MAX_VIEW_UP2_X + DISPLAY_MAX_VIEW_UD_SIZE_X ) &&
		( ptcursor.y >= DISPLAY_MAX_VIEW_UP2_Y ) && ( ptcursor.y <= DISPLAY_MAX_VIEW_UP2_Y + DISPLAY_MAX_VIEW_UD_SIZE_Y ) ) {
		if ( MAX_VIEW_START_POINTER > 0 ) {
			i = MAX_VIEW_START_POINTER - DISPLAY_MAX_VIEW_COUNT;
			if ( i < 0 ) i = 0;
			if ( ( i >= 0 ) && ( i < MAX_VIEW_LAST_POINTER ) ) {
				MAX_VIEW_START_POINTER = i;
				InvalidateRect( hdlg , NULL , FALSE );
			}
		}
	}
	if ( ( ptcursor.x >= DISPLAY_MAX_VIEW_DN_X ) && ( ptcursor.x <= DISPLAY_MAX_VIEW_DN_X + DISPLAY_MAX_VIEW_UD_SIZE_X ) &&
		( ptcursor.y >= DISPLAY_MAX_VIEW_DN_Y ) && ( ptcursor.y <= DISPLAY_MAX_VIEW_DN_Y + DISPLAY_MAX_VIEW_UD_SIZE_Y ) ) {
		if ( MAX_VIEW_START_POINTER < MAX_VIEW_LAST_POINTER ) {
			i = MAX_VIEW_START_POINTER + 1;
			if ( ( i >= 0 ) && ( i < MAX_VIEW_LAST_POINTER ) ) {
				MAX_VIEW_START_POINTER = i;
				InvalidateRect( hdlg , NULL , FALSE );
			}
		}
	}
	if ( ( ptcursor.x >= DISPLAY_MAX_VIEW_DN2_X ) && ( ptcursor.x <= DISPLAY_MAX_VIEW_DN2_X + DISPLAY_MAX_VIEW_UD_SIZE_X ) &&
		( ptcursor.y >= DISPLAY_MAX_VIEW_DN2_Y ) && ( ptcursor.y <= DISPLAY_MAX_VIEW_DN2_Y + DISPLAY_MAX_VIEW_UD_SIZE_Y ) ) {
		if ( MAX_VIEW_START_POINTER < MAX_VIEW_LAST_POINTER ) {
			i = MAX_VIEW_START_POINTER + DISPLAY_MAX_VIEW_COUNT;
			if ( MAX_VIEW_START_POINTER >= MAX_VIEW_LAST_POINTER ) i = MAX_VIEW_LAST_POINTER - 1;
			if ( ( i >= 0 ) && ( i < MAX_VIEW_LAST_POINTER ) ) {
				MAX_VIEW_START_POINTER = i;
				InvalidateRect( hdlg , NULL , FALSE );
			}
		}
	}
	ClusterData_Button_Set( hdlg , FALSE );
}
//
void ClusterData_MaxView_Get_NewDrawPoint( int *i , int *c , int *m ) {
	int si , sc , sm , z;
	z = 0;
	sm = 0;
	for ( si = 0 ; si < USE_CLUSTERDATA_ITEM_COUNT ; si++ ) {
		for ( sc = 0 ; sc < MAX_PM_CHAMBER_DEPTH ; sc++ ) {
			if ( CLUSTER_RECIPE.MODULE[si][sc] ) {
				switch( Get_CLUSTER_USE_TYPE_ITEM() ) {
				case 3 :
					if ( z == MAX_VIEW_START_POINTER ) { *i = si; *c = sc; *m = sm; return; }
					z++; sm = 1;
					if ( z == MAX_VIEW_START_POINTER ) { *i = si; *c = sc; *m = sm; return; }
					z++; sm = 2;
					if ( z == MAX_VIEW_START_POINTER ) { *i = si; *c = sc; *m = sm; return; }
					z++; sm = 0;
					break;
				case 2 :
					if ( z == MAX_VIEW_START_POINTER ) { *i = si; *c = sc; *m = sm; return; }
					z++; sm = 1;
					if ( z == MAX_VIEW_START_POINTER ) { *i = si; *c = sc; *m = sm; return; }
					z++; sm = 0;
					break;
				default :
					if ( z == MAX_VIEW_START_POINTER ) { *i = si; *c = sc; *m = sm; return; }
					z++; sm = 0;
					break;
				}
			}
		}
	}
	*i = si;
	*c = sc;
	*m = sm;
}

void ClusterData_MaxView_Draw( HWND hdlg , HDC hDC ) {
	int x , si , sc , sm , z;
//	char Buffer[32]; // 2007.12.28
	char Buffer[256]; // 2007.12.28

	if ( MAX_VIEW_START_POINTER < 0 ) return;

	ClusterData_MaxView_Get_NewDrawPoint( &si , &sc , &sm );
	z = 0;
	//
	for ( ; si < USE_CLUSTERDATA_ITEM_COUNT ; si++ ) {
		x = 0;
		for ( ; sc < MAX_PM_CHAMBER_DEPTH ; sc++ ) {
			if ( CLUSTER_RECIPE.MODULE[si][sc] ) {
				if      ( x == 0 ) x = 1;
				else if ( x == 1 ) x = 2;
				switch( Get_CLUSTER_USE_TYPE_ITEM() ) {
				case 3 :
					if ( sm == 0 && x == 1 ) {
						wsprintf( Buffer , "%4d" , si+1 );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_ID_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_ID_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
					}
					if ( sm == 0 ) {
						if ( strlen( Get_Prm_PROCESS_NAME( sc ) ) < 0 )
							wsprintf( Buffer , "PM%d" , sc+1 );
						else
							wsprintf( Buffer , "%s" , Get_Prm_PROCESS_NAME( sc ) );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MDL_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MDL_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						wsprintf( Buffer , "MAIN" );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MODE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MODE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//
						wsprintf( Buffer , "%s" , CLUSTER_RECIPE.MODULE_IN_RECIPE[si][sc] );
						if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_START_POINTER + z )
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
						else
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						z++; sm = 1;
						if ( z >= DISPLAY_MAX_VIEW_COUNT ) { si = sc = 99999; break; }
					}
					if ( sm == 1 ) {
						//---------------------
						wsprintf( Buffer , "PRE" );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MODE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MODE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//
						wsprintf( Buffer , "%s" , CLUSTER_RECIPE.MODULE_PR_RECIPE[si][sc] );
						if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_START_POINTER + z )
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
						else
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						z++; sm = 2;
						if ( z >= DISPLAY_MAX_VIEW_COUNT ) { si = sc = 99999; break; }
					}
					if ( sm == 2 ) {
						//---------------------
						wsprintf( Buffer , "OUT" );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MODE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MODE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//
						wsprintf( Buffer , "%s" , CLUSTER_RECIPE.MODULE_OUT_RECIPE[si][sc] );
						if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_START_POINTER + z )
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
						else
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						z++; sm = 0;
						if ( z >= DISPLAY_MAX_VIEW_COUNT ) { si = sc = 99999; break; }
					}
					break;
				case 2 :
					if ( sm == 0 && x == 1 ) {
						wsprintf( Buffer , "%4d" , si+1 );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_ID_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_ID_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
					}
					if ( sm == 0 ) {
						if ( strlen( Get_Prm_PROCESS_NAME( sc ) ) < 0 )
							wsprintf( Buffer , "PM%d" , sc+1 );
						else
							wsprintf( Buffer , "%s" , Get_Prm_PROCESS_NAME( sc ) );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MDL_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MDL_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						wsprintf( Buffer , "MAIN" );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MODE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MODE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//
						wsprintf( Buffer , "%s" , CLUSTER_RECIPE.MODULE_IN_RECIPE[si][sc] );
						if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_START_POINTER + z )
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
						else
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						z++; sm = 1;
						if ( z >= DISPLAY_MAX_VIEW_COUNT ) { si = sc = 99999; break; }
					}
					if ( sm == 1 ) {
						//---------------------
						wsprintf( Buffer , "OUT" );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MODE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MODE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//
						wsprintf( Buffer , "%s" , CLUSTER_RECIPE.MODULE_OUT_RECIPE[si][sc] );
						if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_START_POINTER + z )
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
						else
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_RECIPE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						z++; sm = 0;
						if ( z >= DISPLAY_MAX_VIEW_COUNT ) { si = sc = 99999; break; }
					}
					break;
				default :
					if ( sm == 0 && x == 1 ) {
						wsprintf( Buffer , "%4d" , si+1 );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_ID_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_ID_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
					}
					if ( sm == 0 ) {
						if ( strlen( Get_Prm_PROCESS_NAME( sc ) ) < 0 )
							wsprintf( Buffer , "PM%d" , sc+1 );
						else
							wsprintf( Buffer , "%s" , Get_Prm_PROCESS_NAME( sc ) );
						KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MDL_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_MDL_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						wsprintf( Buffer , "%s" , CLUSTER_RECIPE.MODULE_IN_RECIPE[si][sc] );
						if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_START_POINTER + z )
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MODE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE + DISPLAY_MAX_VIEW_MODE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_White , COLOR_DarkBlue );
						else
							KGUI_DRAW_StrBox22( hDC , DISPLAY_MAX_VIEW_MODE_START_X , DISPLAY_MAX_VIEW_START_Y + ( z * DISPLAY_MAX_VIEW_INTERVAL_Y ) , Buffer , DISPLAY_MAX_VIEW_RECIPE_STRING_SIZE + DISPLAY_MAX_VIEW_MODE_STRING_SIZE , 1 , COLOR_DarkGray , COLOR_Black , COLOR_Black , COLOR_White );
						//---------------------
						z++; sm = 0;
						if ( z >= DISPLAY_MAX_VIEW_COUNT ) { si = sc = 99999; break; }
					}
					break;
				}
			}
		}
		sc = 0;
	}
	ClusterData_MaxView_Draw_Select_Pointer( hdlg , hDC );
}
//
void ClusterData_Get_Last_View( BOOL Init ) {
	int si,sc;
	if ( Init ) {
		MAX_VIEW_START_POINTER  = 0;
		MAX_VIEW_SELECT_POINTER = 0;
		MAX_VIEW_LAST_POINTER = 0;
	}
	if ( MAX_VIEW_START_POINTER < 0 ) return;
	MAX_VIEW_LAST_POINTER = 0;
	//
	for ( si = 0 ; si < USE_CLUSTERDATA_ITEM_COUNT ; si++ ) {
		for ( sc = 0 ; sc < MAX_PM_CHAMBER_DEPTH ; sc++ ) {
			if ( CLUSTER_RECIPE.MODULE[si][sc] ) {
				switch( Get_CLUSTER_USE_TYPE_ITEM() ) {
				case 3 :	MAX_VIEW_LAST_POINTER += 3;		break;
				case 2 :	MAX_VIEW_LAST_POINTER += 2;		break;
				default :	MAX_VIEW_LAST_POINTER++;		break;
				}
			}
		}
	}
	if ( MAX_VIEW_START_POINTER  > MAX_VIEW_LAST_POINTER ) MAX_VIEW_START_POINTER = 0;
	if ( MAX_VIEW_SELECT_POINTER > MAX_VIEW_LAST_POINTER ) MAX_VIEW_SELECT_POINTER = 0;
	if ( MAX_VIEW_START_POINTER == MAX_VIEW_LAST_POINTER ) {
		MAX_VIEW_START_POINTER -= DISPLAY_MAX_VIEW_COUNT;
		if ( MAX_VIEW_START_POINTER < 0 ) MAX_VIEW_START_POINTER = 0;
	}
}
//
BOOL ClusterData_MaxView_Delete( HWND hdlg ) {
	int si , sc , z;
	z = 0;
	for ( si = 0 ; si < USE_CLUSTERDATA_ITEM_COUNT ; si++ ) {
		for ( sc = 0 ; sc < MAX_PM_CHAMBER_DEPTH ; sc++ ) {
			if ( CLUSTER_RECIPE.MODULE[si][sc] ) {
				switch( Get_CLUSTER_USE_TYPE_ITEM() ) {
				case 3 :
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[si][sc] , "" );
						return TRUE;
					}
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[si][sc] , "" );
						return TRUE;
					}
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[si][sc] , "" );
						return TRUE;
					}
					z++;
					break;
				case 2 :
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[si][sc] , "" );
						return TRUE;
					}
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[si][sc] , "" );
						return TRUE;
					}
					z++;
					break;
				default :
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[si][sc] , "" );
						return TRUE;
					}
					z++;
					break;
				}
			}
		}
	}
	return FALSE;
}
//
int ClusterData_MaxView_Match_Sel_Chamber( HWND hdlg , int *selcl ) {
	int si , sc , z , m , lp = 0;
	z = 0;
	for ( si = 0 ; si < USE_CLUSTERDATA_ITEM_COUNT ; si++ ) {
		m = 0;
		for ( sc = 0 ; sc < MAX_PM_CHAMBER_DEPTH ; sc++ ) {
			if ( CLUSTER_RECIPE.MODULE[si][sc] ) {
				m = 1;
				switch( Get_CLUSTER_USE_TYPE_ITEM() ) {
				case 3 :
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						*selcl = si;
						if ( CLUSTER_RECIPE.MODULE[si][MAX_VIEW_CHAMBER] ) return 1;
						return 0; //2002.05.20
					}					
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						*selcl = si;
						if ( CLUSTER_RECIPE.MODULE[si][MAX_VIEW_CHAMBER] ) return 1;
						return 0; //2002.05.20
					}					
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						*selcl = si;
						if ( CLUSTER_RECIPE.MODULE[si][MAX_VIEW_CHAMBER] ) return 1;
						return 0; //2002.05.20
					}					
					z++;
					break;
				case 2 :
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						*selcl = si;
						if ( CLUSTER_RECIPE.MODULE[si][MAX_VIEW_CHAMBER] ) return 1;
						return 0; //2002.05.20
					}					
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						*selcl = si;
						if ( CLUSTER_RECIPE.MODULE[si][MAX_VIEW_CHAMBER] ) return 1;
						return 0; //2002.05.20
					}					
					z++;
					break;
				default :
					if ( z == MAX_VIEW_SELECT_POINTER ) {
						*selcl = si;
						if ( CLUSTER_RECIPE.MODULE[si][MAX_VIEW_CHAMBER] ) return 1;
						return 0; //2002.05.20
					}					
					z++;
					break;
				}
			}
		}
		if ( m == 0 ) lp = 1;
	}
	if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_LAST_POINTER ) {
		if ( lp == 0 ) return 3;
		return 2;
	}
	return 0;
}
//
BOOL ClusterData_MaxView_Match_Sel_Order( HWND hdlg , int *k , int *l ) {
	int si , sc , z , m , lp = -1;
	z = 0;
	for ( si = 0 ; si < USE_CLUSTERDATA_ITEM_COUNT ; si++ ) {
		m = 0;
		for ( sc = 0 ; sc < MAX_PM_CHAMBER_DEPTH ; sc++ ) {
			if ( CLUSTER_RECIPE.MODULE[si][sc] ) {
				m = 1;
				switch( Get_CLUSTER_USE_TYPE_ITEM() ) {
				case 3 :
					if ( z == MAX_VIEW_SELECT_POINTER ) { *k = si; *l = 0; return TRUE; }
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) { *k = si; *l = 2; return TRUE; }
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) { *k = si; *l = 1; return TRUE; }
					z++;
					break;
				case 2 :
					if ( z == MAX_VIEW_SELECT_POINTER ) { *k = si; *l = 0; return TRUE; }
					z++;
					if ( z == MAX_VIEW_SELECT_POINTER ) { *k = si; *l = 1; return TRUE; }
					z++;
					break;
				default :
					if ( z == MAX_VIEW_SELECT_POINTER ) { *k = si; *l = 0; return TRUE; }
					z++;
					break;
				}
			}
		}
		if ( m == 0 ) {
			if ( lp < 0 ) {
				lp = si;
			}
		}
	}
	if ( MAX_VIEW_SELECT_POINTER == MAX_VIEW_LAST_POINTER ) {
		if ( lp >= 0 ) {
			*k = lp;
		}
		else {
			*k = MAX_VIEW_LAST_POINTER;
		}
		*l = 0;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Set_CLUSTERDATA_DISPLAY( HWND hdlg , int mode ) {
	if ( !mode ) {
		KWIN_Item_Hide( hdlg , IDC_LIST_CLUSTERDATA );
		//
		KWIN_Item_Hide( hdlg , IDC_CTRL_AL_MODE );
		//
	}
	else {
		KWIN_Item_Enable( hdlg , IDC_LIST_CLUSTERDATA );
		//
		EXTRA_STATION_MAPPING( hdlg );
		//
	}
}
//
//
//
void FileData_Fresh_View( HWND hdlg ) {
	Set_CLUSTER_FILE_CHANGE( FALSE );
	//
	FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 1 );
	FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 0 );
	FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 1 );
	FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 0 );
	FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 1 );
	//
	Set_Enable_CLUSTERFILE( hdlg , TRUE );
	Set_Enable2_PROCESSFILE( hdlg , TRUE );
	//
	Set_CLUSTERDATA_DISPLAY( hdlg , TRUE );
	//
	ClusterData_Button_Set( hdlg , FALSE );
	InvalidateRect( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , NULL , FALSE );
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL DirData_ListView_Init( HWND hWnd ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	BOOL Find;
	char Buffer[256];
	int i;
	sprintf( Buffer , "%s/*.*" , Get_Prm_MAIN_RECIPE_PATH() );
	ListBox_ResetContent( hWnd );
	if ( !Get_Prm_MONITOR_MODE() ) {
		hFind = FindFirstFile( Buffer , &fd );
	}
	else { // 2003.12.15
		hFind = _nINF_KNFS_FindFirstFile( Buffer , &fd );
	}
	while ( (hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) {
			if ( ( strcmp( fd.cFileName , "." ) != 0 ) &&
				( strcmp( fd.cFileName , ".." ) != 0 ) ) {
				Find = FALSE;
				if ( STRCMP_L( fd.cFileName , Get_Prm_LOT_RECIPE_PATH() ) ) Find = TRUE;
				if ( !Find ) {
					if ( STRCMP_L( fd.cFileName , Get_Prm_CLUSTER_RECIPE_PATH() ) ) Find = TRUE;
				}
				if ( !Find ) {
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						if ( STRCMP_L( fd.cFileName , Get_Prm_PROCESS_RECIPE_HEAD_PATH( i ) ) ) {
							Find = TRUE;
							break;
						}
					}
				}
				if ( !Find ) ListBox_AddString( hWnd , fd.cFileName );
			}
		}
		if ( !Get_Prm_MONITOR_MODE() ) {
			bRet = FindNextFile( hFind , &fd );
		}
		else { // 2003.12.15
			bRet = _nINF_KNFS_FindNextFile( hFind , &fd );
		}
	}
	if ( !Get_Prm_MONITOR_MODE() ) {
		FindClose( hFind );
	}
	else { // 2003.12.15
		_nINF_KNFS_FindClose( hFind );
	}
	ListBox_SetCurSel( hWnd , -1 );
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int ModuleData_ListView_Index( int data ) { // 2006.04.27
	int i , j = 0;
	for ( i = 0 ; i < Get_Prm_CHAMBER_SIZE() ; i++ ) {
		if ( Get_Prm_MODULE_MODE( i ) ) {
			if ( j == data ) return i;
			j++;
		}
	}
	return 0;
}

void ModuleData_ListView_Init( HWND hWnd ) {
	int i;
	char Buffer[32];
	ListBox_ResetContent( hWnd );
	for ( i = 0 ; i < Get_Prm_CHAMBER_SIZE() ; i++ ) {
		if ( Get_Prm_MODULE_MODE( i ) ) { // 2006.04.27
			if ( strlen( Get_Prm_PROCESS_NAME( i ) ) > 0 ) {
				ListBox_AddString( hWnd , Get_Prm_PROCESS_NAME( i ) );
			}
			else {
				sprintf( Buffer , "PM%d" , i + 1 );
				ListBox_AddString( hWnd , Buffer );
			}
		}
	}
	ListBox_SetCurSel( hWnd , -1 );
}

BOOL FileData_ListView_Init( HWND hWnd , char *data , char *data2 , char *data3 , char *data4 ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	char Buffer[256];
	if ( strlen( data2 ) > 0 ) {
		if ( strlen( data3 ) > 0 ) {
			sprintf( Buffer , "%s/%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , data3 , data4 ); // 2004.03.22
		}
		else {
			sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , data4 ); // 2004.03.22
		}
	}
	else {
		if ( strlen( data3 ) > 0 ) {
			sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data3 , data4 ); // 2004.03.22
		}
		else {
			sprintf( Buffer , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data4 ); // 2004.03.22
		}
	}
	ListBox_ResetContent( hWnd );
	if ( !Get_Prm_MONITOR_MODE() ) {
		hFind = FindFirstFile( Buffer , &fd );
	}
	else { // 2003.12.15
		hFind = _nINF_KNFS_FindFirstFile( Buffer , &fd );
	}
	while ( (hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			ListBox_AddString( hWnd , fd.cFileName );
		}
		if ( !Get_Prm_MONITOR_MODE() ) {
			bRet = FindNextFile( hFind , &fd );
		}
		else { // 2003.12.15
			bRet = _nINF_KNFS_FindNextFile( hFind , &fd );
		}
	}
	if ( !Get_Prm_MONITOR_MODE() ) {
		FindClose( hFind );
	}
	else { // 2003.12.15
		_nINF_KNFS_FindClose( hFind );
	}
	ListBox_SetCurSel( hWnd , -1 );
	return TRUE;
}

BOOL FileData_ListView_Init2( HWND hWnd , char *data , char *data2 , char *data3 , char *data4 ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	SYSTEMTIME sysTime; // 2007.09.17
	FILETIME fTime; // 2007.09.17
	BOOL bRet = TRUE , add , bc , lc , cc;
	char Buffer[256];
	if ( SEARCH_CONDITION_USE ) {
		if ( strlen( SEARCH_CONDITION1_EXT ) <= 0 ) {
			if ( strlen( data2 ) > 0 ) {
				if ( strlen( data3 ) > 0 ) {
					sprintf( Buffer , "%s/%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , data3 , data4 ); // 2004.03.22
				}
				else {
					sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , data4 ); // 2004.03.22
				}
			}
			else {
				if ( strlen( data3 ) > 0 ) {
					sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data3 , data4 ); // 2004.03.22
				}
				else {
					sprintf( Buffer , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data4 ); // 2004.03.22
				}
			}
		}
		else {
			if ( strlen( data2 ) > 0 ) {
				if ( strlen( data3 ) > 0 ) {
					sprintf( Buffer , "%s/%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , data3 , SEARCH_CONDITION1_EXT ); // 2004.03.22
				}
				else {
					sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , SEARCH_CONDITION1_EXT ); // 2004.03.22
				}
			}
			else {
				if ( strlen( data3 ) > 0 ) {
					sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data3 , SEARCH_CONDITION1_EXT ); // 2004.03.22
				}
				else {
					sprintf( Buffer , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , SEARCH_CONDITION1_EXT ); // 2004.03.22
				}
			}
		}
	}
	else {
		if ( strlen( data2 ) > 0 ) {
			if ( strlen( data3 ) > 0 ) {
				sprintf( Buffer , "%s/%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , data3 , data4 ); // 2004.03.22
			}
			else {
				sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data2 , data4 ); // 2004.03.22
			}
		}
		else {
			if ( strlen( data3 ) > 0 ) {
				sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data3 , data4 ); // 2004.03.22
			}
			else {
				sprintf( Buffer , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data , data4 ); // 2004.03.22
			}
		}
	}
	ListBox_ResetContent( hWnd );
	if ( !Get_Prm_MONITOR_MODE() ) {
		hFind = FindFirstFile( Buffer , &fd );
	}
	else { // 2003.12.15
		hFind = _nINF_KNFS_FindFirstFile( Buffer , &fd );
	}
	while ( (hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			add = TRUE;
			//
			if ( SEARCH_CONDITION_USE ) {
				//
				FileTimeToLocalFileTime( &(fd.ftLastWriteTime) , &fTime );
				FileTimeToSystemTime( &fTime , &sysTime );
				//
				bc = ( SEARCH_CONDITION2_BEGIN_YEAR  > 0 ) ?      ( SEARCH_CONDITION2_BEGIN_YEAR  * 10000 ) :      ( sysTime.wYear * 10000 );
				bc = ( SEARCH_CONDITION2_BEGIN_MONTH > 0 ) ? bc + ( SEARCH_CONDITION2_BEGIN_MONTH * 100   ) : bc + ( sysTime.wMonth * 100 );
				bc = ( SEARCH_CONDITION2_BEGIN_DAY   > 0 ) ? bc + ( SEARCH_CONDITION2_BEGIN_DAY           ) : bc + ( sysTime.wDay );
				lc = ( SEARCH_CONDITION2_END_YEAR  > 0 )   ?      ( SEARCH_CONDITION2_END_YEAR  * 10000 ) :      ( sysTime.wYear * 10000 );
				lc = ( SEARCH_CONDITION2_END_MONTH > 0 )   ? lc + ( SEARCH_CONDITION2_END_MONTH * 100   ) : lc + ( sysTime.wMonth * 100 );
				lc = ( SEARCH_CONDITION2_END_DAY   > 0 )   ? lc + ( SEARCH_CONDITION2_END_DAY           ) : lc + ( sysTime.wDay );
				//
				cc = ( sysTime.wYear * 10000 ) + ( sysTime.wMonth * 100 ) + ( sysTime.wDay );
				//
				if ( ( cc < bc ) || ( cc > lc ) ) add = FALSE;
			}
			if ( add ) ListBox_AddString( hWnd , fd.cFileName );
		}
		if ( !Get_Prm_MONITOR_MODE() ) {
			bRet = FindNextFile( hFind , &fd );
		}
		else { // 2003.12.15
			bRet = _nINF_KNFS_FindNextFile( hFind , &fd );
		}
	}
	if ( !Get_Prm_MONITOR_MODE() ) {
		FindClose( hFind );
	}
	else { // 2003.12.15
		_nINF_KNFS_FindClose( hFind );
	}
	ListBox_SetCurSel( hWnd , -1 );
	return TRUE;
}

//
BOOL Console_Cluster_File_Open( HWND hdlg , char *Buffer ) {
	char Filename[512];
	char localFilename[512];
	sprintf( Filename , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_CLUSTER_RECIPE_PATH() , Buffer );
	if ( !Get_Prm_MONITOR_MODE() ) {
		sprintf( localFilename , "%s" , Filename );
	}
	else {
		sprintf( localFilename , "%s/%s" , Get_Prm_MONITOR_FOLDER() , Filename );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( localFilename );
		if ( !_nINF_KNFS_GetFile( Filename , localFilename , FALSE ) ) {
			return FALSE;
		}
	}
	if ( Kor_CLUSTER_Open( localFilename , Get_Prm_GROUP_RECIPE_PATH() ) ) {
		//
		Set_Select_CLUSTERFILE( hdlg , Buffer );
		//
		ClusterData_Get_Last_View( TRUE );
		//
		FileData_Fresh_View( hdlg );
		return TRUE;
	}
	return FALSE;
}
//
BOOL Console_Cluster_File_Save( HWND hdlg , char *Buffer , int newmode ) {
	char Filename[512];
	char localFilename[512];
	sprintf( Filename , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_CLUSTER_RECIPE_PATH() , Buffer );
	if ( !Get_Prm_MONITOR_MODE() ) {
		sprintf( localFilename , "%s" , Filename );
	}
	else {
		sprintf( localFilename , "%s/%s" , Get_Prm_MONITOR_FOLDER() , Filename );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( localFilename );
	}
	if ( Kor_CLUSTER_Save( localFilename , newmode , Get_Prm_GROUP_RECIPE_PATH() ) ) {
		//
		if ( Get_Prm_MONITOR_MODE() ) {
			if ( _nINF_KNFS_SetFile( localFilename , Filename , FALSE ) ) {
				FileData_Fresh_View( hdlg );
				return TRUE;
			}
		}
		else {
			FileData_Fresh_View( hdlg );
			return TRUE;
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Console_L_View_Open_Full_Init( HWND hwndListView , int count , char *String[] , int *Size ) {
	int         i;
	LV_COLUMN   lvColumn;
	//
	ListView_DeleteAllItems( hwndListView );
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for ( i = 0 ; i < count ; i++ ) {
		lvColumn.cx = Size[i];
		lvColumn.pszText = String[i];
		ListView_InsertColumn( hwndListView , i , &lvColumn );
	}	
	ListView_DeleteAllItems( hwndListView );
}
//
void Console_L_View_Open_Full_Init_for_CHAMBER( HWND hwndListView , char *head , int count , int size ) {
	int         i;
	LV_COLUMN   lvColumn;
	char Buffer[32];
	//
	ListView_DeleteAllItems( hwndListView );
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	if ( count == 0 ) {
		lvColumn.cx = size + 100;
		sprintf( Buffer , "Not Used" );
	}
	else {
		lvColumn.cx = size + 20;
		sprintf( Buffer , "%s" , head );
	}
	lvColumn.pszText = Buffer;
	ListView_InsertColumn( hwndListView , 0 , &lvColumn );
	for ( i = 0 ; i < count ; i++ ) {
		lvColumn.cx = size;
		sprintf( Buffer , "%d" , i + 1 );
		lvColumn.pszText = Buffer;
		ListView_InsertColumn( hwndListView , i + 1 , &lvColumn );
	}	
	ListView_DeleteAllItems( hwndListView );
}
//
void Console_L_View_Open_CallBack_Insert( HWND hWnd , int count ) {
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
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_Cluster_Parameter_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	TCHAR *szString[] = { "PM" , "Use" , "RecipePath" , "Recipe ID" };
	int    szSize[4]  = {  52  ,    53 ,          180 ,        143  };

	switch ( msg ) {
	case WM_INITDIALOG:
		KWIN_Item_String_Display( hdlg , IDC_STATIC_FILEID , Get_Prm_FILE_ID() );
		KWIN_Item_String_Display( hdlg , IDC_STATIC_CLUSTERID , Get_Prm_CLUSTER_RECIPE_ID() );
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MAINPATH , Get_Prm_MAIN_RECIPE_PATH() );
		KWIN_Item_String_Display( hdlg , IDC_STATIC_CLUSTERPATH , Get_Prm_CLUSTER_RECIPE_PATH() );
		//
		if ( Get_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT() )
			KWIN_Item_String_Display( hdlg , IDC_STATIC_PARALLEL , "On" );
		else
			KWIN_Item_String_Display( hdlg , IDC_STATIC_PARALLEL , "Off" );

		if ( Get_GROUP_RUN_CHECK() )
			KWIN_Item_String_Display( hdlg , IDC_STATIC_GROUP , "Group" );
		else
			KWIN_Item_String_Display( hdlg , IDC_STATIC_GROUP , "Single" );

		KWIN_Item_Int_Display( hdlg , IDC_STATIC_TYPE  , Get_CLUSTER_USE_TYPE_ITEM() );
		KWIN_Item_Int_Display( hdlg , IDC_STATIC_DEPTH , USE_CLUSTERDATA_ITEM_COUNT );
		if ( Get_HANDLING_SIDE_CHECK() )
			KWIN_Item_String_Display( hdlg , IDC_STATIC_HANDLINGSIDE , "Use" );
		else
			KWIN_Item_String_Display( hdlg , IDC_STATIC_HANDLINGSIDE , "N/A" );
		//
		Console_L_View_Open_Full_Init( GetDlgItem( hdlg , IDC_LIST_BOX ) , 4 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		Console_L_View_Open_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_BOX ) , MAX_PM_CHAMBER_DEPTH );
		//
		Console_L_View_Open_Full_Init_for_CHAMBER( GetDlgItem( hdlg , IDC_LIST_BOX2 ) , "ALL" , PM_EX_COUNT , 22 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		Console_L_View_Open_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_BOX2 ) , MAX_PM_CHAMBER_DEPTH );
		//
		Console_L_View_Open_Full_Init_for_CHAMBER( GetDlgItem( hdlg , IDC_LIST_BOX3 ) , "PAR" , PM_PX_COUNT , 22 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_BOX3 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		Console_L_View_Open_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_BOX3 ) , MAX_PM_CHAMBER_DEPTH );
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i;
			switch( lpnmh->code ) {
			case LVN_GETDISPINFO:	{
					LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
					TCHAR szString[ 256 ];
					if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_BOX ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if  ( Get_Prm_MODULE_MODE(i) ) {
										sprintf( szString , "On" );
									}
									else {
										sprintf( szString , "" );
									}
									break;
								case 2 :
									if  ( Get_Prm_MODULE_MODE(i) )	{
										sprintf( szString , "%s" , Get_Prm_PROCESS_RECIPE_PATH(i) );
									}
									else {
										sprintf( szString , "" );
									}
									break;
								case 3 :
									if  ( Get_Prm_MODULE_MODE(i) )	{
										sprintf( szString , "%s" , Get_Prm_PROCESS_RECIPE_ID(i) );
									}
									else {
										sprintf( szString , "" );
									}
									break;
								}
								lstrcpy( lpdi->item.pszText , szString );
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
								lstrcpy( lpdi->item.pszText , szString );
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_BOX2 ) ) {
						if ( lpdi->item.iSubItem ) {
							if  ( PM_EX_STATUS[lpdi->item.iSubItem-1][lpdi->item.iItem] ) {
								sprintf( szString , "O" );
							}
							else {
								sprintf( szString , "" );
							}
						}
						else {
							wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
						}
						lstrcpy( lpdi->item.pszText , szString );
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_BOX3 ) ) {
						if ( lpdi->item.iSubItem ) {
							if  ( PM_PX_STATUS[lpdi->item.iSubItem-1][lpdi->item.iItem] ) {
								sprintf( szString , "O" );
							}
							else {
								sprintf( szString , "" );
							}
						}
						else {
							wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
						}
						lstrcpy( lpdi->item.pszText , szString );
					}
				}
				return 0;

			case NM_DBLCLK :		return 0;
			case LVN_ITEMCHANGED :	return 0;
			case LVN_KEYDOWN :		return 0;
			}
		}
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
			case IDCANCEL :
				EndDialog( hdlg , 0 );
				return TRUE;
			}
		return TRUE;

	case WM_DESTROY:
		return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
BOOL APIENTRY SEARCH_PANEL_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int idx;
	char buffer[31+1];

	switch ( msg ) {
		case WM_INITDIALOG:
			SendMessage( GetDlgItem( hdlg , IDC_EDIT1 ) , WM_SETTEXT , 0 , (LPARAM) SEARCH_CONDITION1_EXT );
			//
			if ( SEARCH_CONDITION2_BEGIN_YEAR > 0 ) {
				sprintf( buffer , "%d" , SEARCH_CONDITION2_BEGIN_YEAR );
				SendMessage( GetDlgItem( hdlg , IDC_EDIT2 ) , WM_SETTEXT , 0 , (LPARAM) buffer );
			}
			if ( SEARCH_CONDITION2_BEGIN_MONTH > 0 ) {
				sprintf( buffer , "%d" , SEARCH_CONDITION2_BEGIN_MONTH );
				SendMessage( GetDlgItem( hdlg , IDC_EDIT3 ) , WM_SETTEXT , 0 , (LPARAM) buffer );
			}
			if ( SEARCH_CONDITION2_BEGIN_DAY > 0 ) {
				sprintf( buffer , "%d" , SEARCH_CONDITION2_BEGIN_DAY );
				SendMessage( GetDlgItem( hdlg , IDC_EDIT4 ) , WM_SETTEXT , 0 , (LPARAM) buffer );
			}
			if ( SEARCH_CONDITION2_END_YEAR > 0 ) {
				sprintf( buffer , "%d" , SEARCH_CONDITION2_END_YEAR );
				SendMessage( GetDlgItem( hdlg , IDC_EDIT5 ) , WM_SETTEXT , 0 , (LPARAM) buffer );
			}
			if ( SEARCH_CONDITION2_END_MONTH > 0 ) {
				sprintf( buffer , "%d" , SEARCH_CONDITION2_END_MONTH );
				SendMessage( GetDlgItem( hdlg , IDC_EDIT6 ) , WM_SETTEXT , 0 , (LPARAM) buffer );
			}
			if ( SEARCH_CONDITION2_END_DAY > 0 ) {
				sprintf( buffer , "%d" , SEARCH_CONDITION2_END_DAY );
				SendMessage( GetDlgItem( hdlg , IDC_EDIT7 ) , WM_SETTEXT , 0 , (LPARAM) buffer );
			}
			return TRUE;
			
		case WM_COMMAND :
			switch (wParam) {
				case IDOK : // Reload All
				case IDYES : // Change Skip
					//
					SEARCH_CONDITION_USE = ( wParam == IDYES );
					//
					SendMessage( GetDlgItem( hdlg , IDC_EDIT1 ) , WM_GETTEXT , 255 , (LPARAM) SEARCH_CONDITION1_EXT );
					//
					SendMessage( GetDlgItem( hdlg , IDC_EDIT2 ) , WM_GETTEXT , 31 , (LPARAM) buffer );
					idx = atoi( buffer );	if ( idx > 0 ) SEARCH_CONDITION2_BEGIN_YEAR = idx;
					//
					SendMessage( GetDlgItem( hdlg , IDC_EDIT3 ) , WM_GETTEXT , 31 , (LPARAM) buffer );
					idx = atoi( buffer );	if ( idx > 0 ) SEARCH_CONDITION2_BEGIN_MONTH = idx;
					//
					SendMessage( GetDlgItem( hdlg , IDC_EDIT4 ) , WM_GETTEXT , 31 , (LPARAM) buffer );
					idx = atoi( buffer );	if ( idx > 0 ) SEARCH_CONDITION2_BEGIN_DAY = idx;
					//
					SendMessage( GetDlgItem( hdlg , IDC_EDIT5 ) , WM_GETTEXT , 31 , (LPARAM) buffer );
					idx = atoi( buffer );	if ( idx > 0 ) SEARCH_CONDITION2_END_YEAR = idx;
					//
					SendMessage( GetDlgItem( hdlg , IDC_EDIT6 ) , WM_GETTEXT , 31 , (LPARAM) buffer );
					idx = atoi( buffer );	if ( idx > 0 ) SEARCH_CONDITION2_END_MONTH = idx;
					//
					SendMessage( GetDlgItem( hdlg , IDC_EDIT7 ) , WM_GETTEXT , 31 , (LPARAM) buffer );
					idx = atoi( buffer );	if ( idx > 0 ) SEARCH_CONDITION2_END_DAY = idx;

					EndDialog( hdlg , 1 );
					return TRUE;

				case IDCANCEL :
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			break;
	}
	return FALSE;
}


BOOL APIENTRY Console_Gui_For_Main_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT	ps;
	int i , j , k , f;
	char Buffer[1024+1];
//	char Buffer2[256]; // 2008.02.15
	char Buffer2[1024+1]; // 2008.02.15
//	char Buffer3[256]; // 2008.02.15
	char Buffer3[1024+1]; // 2008.02.15
	static UINT uTimer;
	HBITMAP hBitmap;
	static int tcount;
	int od1;

	int Get_Resource , Get_Index; // 2004.05.13

	WIN32_FIND_DATA	fd; // 2004.11.08
	HANDLE hFind; // 2004.11.08
	int EditStatus; // 2004.11.08

	//////////////////////////////////////////////////////////////////////////
	enum { OFFON_OFF, OFFON_ON };
	int	nAlignSkipOpt = OFFON_OFF;

	switch ( msg ) {
		case WM_TIMER :
			tcount++;
			if ( tcount >= 15 ) 
			{
				if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
					SendMessage( hdlg , WM_COMMAND , IDC_CTRL_RELOAD_PM1 , (LPARAM) NULL );
					SendMessage( hdlg , WM_COMMAND , IDC_CTRL_RELOAD_PM2 , (LPARAM) NULL );
					SendMessage( hdlg , WM_COMMAND , IDC_CTRL_RELOAD_PM3 , (LPARAM) NULL );
					tcount = 0;
				}
				else {
					tcount--;
				}
			}

			if ( Get_Prm_MONITOR_MODE() ) {
				i = _nINF_KNFS_CONNECTION_STATUS();
				if ( ( i == 3 ) && ( oldcommsts != 3 ) ) {
					//------------------------------------------------------------------------------------------
					USE_SELECT_CURRENT_PM_TYPE = 0;
					//------------------------------------------------------------------------------------------
					MAX_VIEW_CHAMBER = 0;
					MAX_VIEW_TYPE = 0;
					MAX_VIEW_START_POINTER = -1;
					MAX_VIEW_LAST_POINTER = 0;
					MAX_VIEW_SELECT_POINTER = 0;
					//------------------------------------------------------------------------------------------
					Set_Select_CLUSTERFILE( hdlg , "" );
					//
					//------------------------------------------------------------------------------------------
					sprintf( Buffer3 , "%s/%s" , Get_Prm_MONITOR_FOLDER() , Get_Prm_Scheduler_Path() );
					//
					sprintf( Buffer , "%s/System.cfg" , Get_Prm_Scheduler_Path() );
					sprintf( Buffer2 , "%s/%s/System.cfg" , Get_Prm_MONITOR_FOLDER() , Get_Prm_Scheduler_Path() );
					_nINF_KNFS_PrepareLocalFolder_for_Filename( Buffer2 );
					_nINF_KNFS_GetFile( Buffer , Buffer2 , FALSE );
					//
					sprintf( Buffer , "%s/Schedule.cfg" , Get_Prm_Scheduler_Path() );
					sprintf( Buffer2 , "%s/%s/Schedule.cfg" , Get_Prm_MONITOR_FOLDER() , Get_Prm_Scheduler_Path() );
					_nINF_KNFS_PrepareLocalFolder_for_Filename( Buffer2 );
					_nINF_KNFS_GetFile( Buffer , Buffer2 , FALSE );
					//
					sprintf( Buffer , "%s/File.cfg" , Get_Prm_Scheduler_Path() );
					sprintf( Buffer2 , "%s/%s/File.cfg" , Get_Prm_MONITOR_FOLDER() , Get_Prm_Scheduler_Path() );
					_nINF_KNFS_PrepareLocalFolder_for_Filename( Buffer2 );
					_nINF_KNFS_GetFile( Buffer , Buffer2 , FALSE );
					//
					sprintf( Buffer , "%s/MdlGrp.cfg" , Get_Prm_Scheduler_Path() );
					sprintf( Buffer2 , "%s/%s/MdlGrp.cfg" , Get_Prm_MONITOR_FOLDER() , Get_Prm_Scheduler_Path() );
					_nINF_KNFS_PrepareLocalFolder_for_Filename( Buffer2 );
					_nINF_KNFS_GetFile( Buffer , Buffer2 , FALSE );
					//
					od1 = Get_Prm_DRAW_SIZE();
					//===============================================
					FILE_ALL_SETUP( Buffer3 , Get_Prm_Scheduler_CloseString() );
					//===============================================
					oldcommsts = i; // 2005.05.03

					MAIN_WINDOW_REFRESH = TRUE; // 2005.05.03
					//===============================================
					FILE_HANDLING_BUTTON_MAKE( hdlg , 2 ); // 2005.05.03
					//===============================================
					EndDialog( hdlg , 0 ); // 2005.05.03
					return TRUE; // 2005.05.03
					
					
					if ( od1 != Get_Prm_DRAW_SIZE() ) { // 2004.04.06
						MAIN_WINDOW_REFRESH = TRUE;
						//===============================================
						FILE_HANDLING_BUTTON_MAKE( hdlg , 2 );
						//===============================================
						EndDialog( hdlg , 0 );
						return TRUE;
					}

					//===============================================
					FILE_HANDLING_BUTTON_MAKE( hdlg , 0 );
					//===============================================
					if ( Get_GROUP_RUN_CHECK() ) {
						DirData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_RECIPEDIR ) );
					}

					KWIN_Item_String_Display( hdlg , IDC_CTRL_CHANGE_PM1 , PM_SUB_DIRECTORY[0][PM_SUB_CURRENT_POS[0]] );
					KWIN_Item_String_Display( hdlg , IDC_CTRL_CHANGE_PM2 , PM_SUB_DIRECTORY[1][PM_SUB_CURRENT_POS[1]] );
					KWIN_Item_String_Display( hdlg , IDC_CTRL_CHANGE_PM3 , PM_SUB_DIRECTORY[2][PM_SUB_CURRENT_POS[2]] );
					//===============================================
					FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() , "" , "" , "*.*" );
					FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM1FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 0 ) , PM_SUB_DIRECTORY[0][ PM_SUB_CURRENT_POS[0] ] , PM_LOW_DIRECTORY[0][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[0][USE_SELECT_CURRENT_PM_TYPE] );
					FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM2FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 1 ) , PM_SUB_DIRECTORY[1][ PM_SUB_CURRENT_POS[1] ] , PM_LOW_DIRECTORY[1][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[1][USE_SELECT_CURRENT_PM_TYPE] );
					FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM3FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 2 ) , PM_SUB_DIRECTORY[2][ PM_SUB_CURRENT_POS[2] ] , PM_LOW_DIRECTORY[2][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[2][USE_SELECT_CURRENT_PM_TYPE] );
					//
					ClusterData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) );
					//
					Set_CLUSTERDATA_DISPLAY( hdlg , FALSE );
					//
					Set_Enable_CLUSTERFILE( hdlg , TRUE );
					Set_Enable2_PROCESSFILE( hdlg , FALSE );
					//
					FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 1 );
					FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 0 );
					FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 0 );
					FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 0 );
					FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 0 );
					//
					ClusterData_Button_Init( hdlg , 0 , FALSE );
					//===============================================
					InvalidateRect( hdlg , NULL , FALSE );
				}
				//oldcommsts = i; // 2005.05.03
			}
			return TRUE;

		case WM_USER:
            switch ( wParam ) {
				case WM_USER:
					//===============================================
					if ( Get_GROUP_RUN_CHECK() ) {
						if ( !Get_Prm_MONITOR_MODE() ) { // 2004.04.06
							DirData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_RECIPEDIR ) );
						}
					}
					//===============================================
					return TRUE;
			
				case WM_PAINT:
					return TRUE;

				case WM_CLOSE:
						//<<-----------------------------------------------------------
						//>>-----------------------------------------------------------
						return TRUE;

				case WM_ACTIVATE:
						//>>-----------------------------------------------------------
						//<<-----------------------------------------------------------
						return TRUE;
			}
			break;

		case WM_INITDIALOG:
//			oldcommsts = 0; // 2005.05.03
			tcount = 0;
			USE_SELECT_CURRENT_PM_TYPE = 0; // 2002.11.21
			if ( !Get_GROUP_RUN_CHECK() ) {
				KWIN_Item_Hide( hdlg , IDC_LIST_RECIPEDIR );
				KWIN_Item_Hide( hdlg , IDC_BUTTON1 );
				//
				//KWIN_Item_Enable( hdlg , IDC_LIST_CLUSTERFILE );
			}

			SetWindowText( hdlg , "GUI_ClusterEditor" );
			//<<-----------------------------------------------------------
			KGUI_STANDARD_Set_Function_Size( hdlg );
			//>>-----------------------------------------------------------
			//===============================================
			FILE_HANDLING_BUTTON_MAKE( hdlg , 1 );
			//===============================================
			if ( Get_GROUP_RUN_CHECK() ) {
//				if ( !Get_Prm_MONITOR_MODE() ) { // 2004.04.06 // 2005.05.03
					DirData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_RECIPEDIR ) );
//				} // 2005.05.03
			}
			//
			KWIN_Item_String_Display( hdlg , IDC_CTRL_CHANGE_PM1 , PM_SUB_DIRECTORY[0][PM_SUB_CURRENT_POS[0]] );
			KWIN_Item_String_Display( hdlg , IDC_CTRL_CHANGE_PM2 , PM_SUB_DIRECTORY[1][PM_SUB_CURRENT_POS[1]] );
			KWIN_Item_String_Display( hdlg , IDC_CTRL_CHANGE_PM3 , PM_SUB_DIRECTORY[2][PM_SUB_CURRENT_POS[2]] );
			//===============================================
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() , "" , "" , "*.*" );
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM1FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 0 ) , PM_SUB_DIRECTORY[0][ PM_SUB_CURRENT_POS[0] ] , PM_LOW_DIRECTORY[0][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[0][USE_SELECT_CURRENT_PM_TYPE] );
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM2FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 1 ) , PM_SUB_DIRECTORY[1][ PM_SUB_CURRENT_POS[1] ] , PM_LOW_DIRECTORY[1][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[1][USE_SELECT_CURRENT_PM_TYPE] );
			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM3FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 2 ) , PM_SUB_DIRECTORY[2][ PM_SUB_CURRENT_POS[2] ] , PM_LOW_DIRECTORY[2][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[2][USE_SELECT_CURRENT_PM_TYPE] );
			//
			ClusterData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) );
			//
			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
			Set_CLUSTERDATA_DISPLAY( hdlg , FALSE );
			//
			Set_Enable_CLUSTERFILE( hdlg , TRUE );
			Set_Enable2_PROCESSFILE( hdlg , FALSE );
			//
			FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 1 );
			FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 0 );
			FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 0 );
			FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 0 );
			FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 0 );
			//
			ClusterData_Button_Init( hdlg , 0 , FALSE );
			//
//			uTimer = SetTimer( hdlg , 1000 , 15000 , NULL ); // 2003.12.18
			uTimer = SetTimer( hdlg , 1000 , 1000 , NULL ); // 2003.12.18
			//
			PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
			return TRUE;
			
		case WM_NOTIFY:
			ClusterData_ListView_Notify( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , lParam );
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

			switch( Get_Prm_DRAW_SIZE() ) {
			case 4 :
				if ( KGUI_Get_BACKGROUND() == NULL ) {
					KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
				}
				else {
					hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND" );
					if ( hBitmap ) {
						KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
						DeleteObject( hBitmap );
					}
					else {
						KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
					}
				}
				break;
			case 6 :
				if ( KGUI_Get_BACKGROUND() == NULL ) {
					KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_6 ) );
				}
				else {
					hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND_6" );
					if ( hBitmap ) {
						KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
						DeleteObject( hBitmap );
					}
					else {
						KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_6 ) );
					}
				}
				break;
			case 8 :
				if ( KGUI_Get_BACKGROUND() == NULL ) {
					KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_8 ) );
				}
				else {
					hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND_8" );
					if ( hBitmap ) {
						KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
						DeleteObject( hBitmap );
					}
					else {
						KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_8 ) );
					}
				}
				break;
			case 10 :
				if ( KGUI_Get_BACKGROUND() == NULL ) {
					KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_10 ) );
				}
				else {
					hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND_10" );
					if ( hBitmap ) {
						KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
						DeleteObject( hBitmap );
					}
					else {
						KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_10 ) );
					}
				}
				break;
			case 12 :
				if ( KGUI_Get_BACKGROUND() == NULL ) {
					KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_12 ) );
				}
				else {
					hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND_12" );
					if ( hBitmap ) {
						KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
						DeleteObject( hBitmap );
					}
					else {
						KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND_12 ) );
					}
				}
				break;
			}

			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_LBUTTONDBLCLK :
			{
				POINT ptcursor;
				GetCursorPos( &ptcursor );
				ScreenToClient( hdlg , &ptcursor );
				if ( ( ptcursor.x < 280 ) || ( ptcursor.x > 650 ) || ( ptcursor.y < 5 ) || ( ptcursor.y > 40 ) ) return TRUE;
			}
		case WM_RBUTTONDOWN:
			if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2002.11.08
			GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_CLUSTER_PARAMETER ) , hdlg , Console_Gui_For_Cluster_Parameter_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
			return TRUE;

		case WM_LBUTTONDOWN:
			return TRUE;

		case WM_DRAWITEM:
			KWIN_DRAW_BUTTON_ITEM( GetModuleHandle( NULL ) , (LPDRAWITEMSTRUCT) lParam );
			return TRUE;

		case WM_COMMAND :
			if ( GET_WM_COMMAND_CMD( wParam , lParam ) == LBN_DBLCLK ) {
				//==================================================================================================================================================
				// 2004.05.13
				//==================================================================================================================================================
				if ( Get_Index_from_Resource_LIST_PMFILE( GET_WM_COMMAND_ID( wParam , lParam ) , &Get_Index ) ) 
				{
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , GET_WM_COMMAND_ID( wParam , lParam ) ) );
					if ( i >= 0 ) {
						ListBox_GetText( GetDlgItem( hdlg , GET_WM_COMMAND_ID( wParam , lParam ) ) , i , Buffer2 );
						if ( strlen( PM_SUB_DIRECTORY[Get_Index][ PM_SUB_CURRENT_POS[Get_Index] ] ) > 0 )
							sprintf( Buffer , "%s/%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_PROCESS_RECIPE_PATH( Get_Index ) , PM_SUB_DIRECTORY[Get_Index][ PM_SUB_CURRENT_POS[Get_Index] ] , Buffer2 );
						else
							sprintf( Buffer , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_PROCESS_RECIPE_PATH( Get_Index ) , Buffer2 );
						LogView_Open_Recipe( hdlg , Get_Index , Buffer );
					}
					else {
						MessageBeep(MB_ICONHAND);
					}
					return TRUE;
				}
				//==================================================================================================================================================
				return TRUE;
			}
			else if ( GET_WM_COMMAND_CMD( wParam , lParam ) == LBN_SELCHANGE ) {
				//===============================================
				if ( Get_GROUP_RUN_CHECK() ) {
					if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_LIST_RECIPEDIR ) {
						i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_RECIPEDIR ) );
						if ( i >= 0 ) {
							ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_RECIPEDIR ) , i , Buffer );
							Set_Prm_GROUP_RECIPE_PATH( Buffer );
							Set_CLUSTER_FILE_CHANGE( FALSE );
							//
							Set_Select_CLUSTERFILE( hdlg , "" );
							//
							Kor_CLUSTER_Initial();
							//
							FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() , "" , "" , "*.*" );
							FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM1FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 0 ) , PM_SUB_DIRECTORY[0][ PM_SUB_CURRENT_POS[0] ] , PM_LOW_DIRECTORY[0][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[0][USE_SELECT_CURRENT_PM_TYPE] );
							FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM2FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 1 ) , PM_SUB_DIRECTORY[1][ PM_SUB_CURRENT_POS[1] ] , PM_LOW_DIRECTORY[1][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[1][USE_SELECT_CURRENT_PM_TYPE] );
							FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM3FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 2 ) , PM_SUB_DIRECTORY[2][ PM_SUB_CURRENT_POS[2] ] , PM_LOW_DIRECTORY[2][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[2][USE_SELECT_CURRENT_PM_TYPE] );
							//
							Set_CLUSTERDATA_DISPLAY( hdlg , FALSE );
							//
							Set_Enable_CLUSTERFILE( hdlg , TRUE );
							//
							ClusterData_Button_Init( hdlg , 0 , FALSE );
							//
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 1 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 0 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 0 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 0 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 0 );
							//--------------------------
						}
					}
				}
				//==================================================================================================================================================
				// 2004.05.13
				//==================================================================================================================================================
				if ( Get_Index_from_Resource_LIST_PMFILE( GET_WM_COMMAND_ID( wParam , lParam ) , &Get_Index ) ) {
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , GET_WM_COMMAND_ID( wParam , lParam ) ) );
					if ( i >= 0 ) {
						ListBox_GetText( GetDlgItem( hdlg , GET_WM_COMMAND_ID( wParam , lParam ) ) , i , Buffer );
						Set_Select_PMFILE( hdlg , Get_Index , Buffer );
						ClusterData_Button_Set( hdlg , FALSE );
					}
					return TRUE;
				}
				//==================================================================================================================================================
				switch ( GET_WM_COMMAND_ID( wParam , lParam ) ) {
				case IDC_LIST_CLUSTERFILE :
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) );
					if ( i >= 0 ) {
						ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , i , Buffer );
						//-------------------------------------------------------------------------------------------------------
						RECIPE_LOG_HANDLING( "OPEN" , "Open with (%s)\n" , Buffer ); // 2004.01.08
						//-------------------------------------------------------------------------------------------------------
						Console_Cluster_File_Open( hdlg , Buffer );
					}
					break;
				}
				return TRUE;
			}
			//==================================================================================================================================================
			// 2004.05.13
			//==================================================================================================================================================
			if ( Get_Resource_LIST_PMFILE_Index_from_Resource_RELOAD_PM( wParam , &Get_Resource , &Get_Index ) ) {
				if ( !IsWindowEnabled( GetDlgItem( hdlg , wParam ) ) ) return TRUE;
				//
				i = ListBox_GetCurSel( GetDlgItem( hdlg , Get_Resource ) );
				//
				FileData_ListView_Init( GetDlgItem( hdlg , Get_Resource ) , Get_Prm_PROCESS_RECIPE_PATH( Get_Index ) , PM_SUB_DIRECTORY[Get_Index][ PM_SUB_CURRENT_POS[Get_Index] ] , PM_LOW_DIRECTORY[Get_Index][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[Get_Index][USE_SELECT_CURRENT_PM_TYPE] );
				//
				ListBox_SetCurSel( GetDlgItem( hdlg , Get_Resource ) , i );
				//
				i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , Get_Resource ) );
				if ( i >= 0 ) {
					ListBox_GetText( GetDlgItem( hdlg , Get_Resource ) , i , Buffer );
					Set_Select_PMFILE( hdlg , Get_Index , Buffer );
				}
				else {
					Set_Select_PMFILE( hdlg , Get_Index , "" );
				}
				ClusterData_Button_Set( hdlg , FALSE );
				return TRUE;
			}
			//==================================================================================================================================================
			switch( wParam ) {
				case IDC_CTRL_RELOAD_CLUSTER :
					FileData_ListView_Init2( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() , "" , "" , "*.*" );
					return TRUE;

				case IDC_CTRL_AL_MODE :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MessageBox( hdlg , "Operation Impossible(Read Only Mode)" , "Operation Impossible" , MB_ICONHAND );
						return TRUE;
					}

					//////////////////////////////////////////////////////////////////////////
					if(MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Wafer Align Mode" , "Wafer Align|Align Skip" , &nAlignSkipOpt)) 
					{
						if( (OFFON_OFF == nAlignSkipOpt && 50 == CLUSTER_RECIPE.EXTRA_STATION) ||
							(OFFON_ON  == nAlignSkipOpt && 10 == CLUSTER_RECIPE.EXTRA_STATION))
						{
							if(OFFON_ON == nAlignSkipOpt)		CLUSTER_RECIPE.EXTRA_STATION = 50;
							else								CLUSTER_RECIPE.EXTRA_STATION = 10;
							//
							EXTRA_STATION_MAPPING( hdlg );
							//
							Set_CLUSTER_FILE_CHANGE( TRUE );
							//
							Set_Enable_CLUSTERFILE( hdlg , FALSE );
							//
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 0 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 1 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 1 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 1 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 0 );
							//
							ClusterData_Button_Set( hdlg , FALSE );
						}
					}
					return TRUE;

				case IDB_SELECT_RCP_MAIN :
					return TRUE;
				case IDB_SELECT_RCP_PR :
					return TRUE;
				case IDB_SELECT_RCP_PO :
					return TRUE;
				//==============================================================================
				//==============================================================================
				//==============================================================================
				//==============================================================================
				//==============================================================================
				//==============================================================================
				case IDB_ADD_ENABLE_P1 :
				case IDB_ADD_ENABLE_P2 :
				case IDB_ADD_ENABLE_P3 :
				case IDB_ADD_ENABLE_P4 :
				case IDB_ADD_ENABLE_P5 :
				case IDB_ADD_ENABLE_P6 :
				case IDB_ADD_ENABLE_P7 :
				case IDB_ADD_ENABLE_P8 :
				case IDB_ADD_ENABLE_P9 :
				case IDB_ADD_ENABLE_P10 :
				case IDB_ADD_ENABLE_P11 :
				case IDB_ADD_ENABLE_P12 :
				case IDB_ADD_ENABLE2_P1 :
				case IDB_ADD_ENABLE2_P2 :
				case IDB_ADD_ENABLE2_P3 :
				case IDB_ADD_ENABLE2_P4 :
				case IDB_ADD_ENABLE2_P5 :
				case IDB_ADD_ENABLE2_P6 :
				case IDB_ADD_ENABLE2_P7 :
				case IDB_ADD_ENABLE2_P8 :
				case IDB_ADD_ENABLE2_P9 :
				case IDB_ADD_ENABLE2_P10 :
				case IDB_ADD_ENABLE2_P11 :
				case IDB_ADD_ENABLE2_P12 :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MessageBox( hdlg , "Operation Impossible(Read Only Mode)" , "Operation Impossible" , MB_ICONHAND );
						return TRUE;
					}

					if      ( wParam == IDB_ADD_ENABLE_P1 ) j = 0;
					else if ( wParam == IDB_ADD_ENABLE_P2 ) j = 1;
					else if ( wParam == IDB_ADD_ENABLE_P3 ) j = 2;
					else if ( wParam == IDB_ADD_ENABLE_P4 ) j = 3;
					else if ( wParam == IDB_ADD_ENABLE_P5 ) j = 4;
					else if ( wParam == IDB_ADD_ENABLE_P6 ) j = 5;
					else if ( wParam == IDB_ADD_ENABLE_P7 ) j = 6;
					else if ( wParam == IDB_ADD_ENABLE_P8 ) j = 7;
					else if ( wParam == IDB_ADD_ENABLE_P9 ) j = 8;
					else if ( wParam == IDB_ADD_ENABLE_P10 ) j = 9;
					else if ( wParam == IDB_ADD_ENABLE_P11 ) j = 10;
					else if ( wParam == IDB_ADD_ENABLE_P12 ) j = 11;
					else if ( wParam == IDB_ADD_ENABLE2_P1 ) j = 0;
					else if ( wParam == IDB_ADD_ENABLE2_P2 ) j = 1;
					else if ( wParam == IDB_ADD_ENABLE2_P3 ) j = 2;
					else if ( wParam == IDB_ADD_ENABLE2_P4 ) j = 3;
					else if ( wParam == IDB_ADD_ENABLE2_P5 ) j = 4;
					else if ( wParam == IDB_ADD_ENABLE2_P6 ) j = 5;
					else if ( wParam == IDB_ADD_ENABLE2_P7 ) j = 6;
					else if ( wParam == IDB_ADD_ENABLE2_P8 ) j = 7;
					else if ( wParam == IDB_ADD_ENABLE2_P9 ) j = 8;
					else if ( wParam == IDB_ADD_ENABLE2_P10 ) j = 9;
					else if ( wParam == IDB_ADD_ENABLE2_P11 ) j = 10;
					else if ( wParam == IDB_ADD_ENABLE2_P12 ) j = 11;

					f = 0;
					PROCESS_SEND_BUTTON_MAPPING( hdlg , j , 0 , 2 );
					i = ClusterData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , -1 );
					while ( i >= 0 ) {
						if ( !CHAMBER_EXCLUSIVE_ENABLE( j , i / Get_CLUSTER_USE_TYPE_ITEM() ) ) {
							sprintf( Buffer, "CHAMBER [PM%d] SELECT IMPOSSIBLE(Interlock)" , j + 1 );
							MessageBox( hdlg , Buffer , "ERROR2" , MB_ICONHAND | MB_OK );
							break;
						}
						f = 1;
						if ( !Get_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT() ) {
							switch ( Get_CLUSTER_USE_TYPE_ITEM() ) {
							case 3 :
								if ( ( i % 3 ) == 0 ) {
									for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i/3][k] , "" );
								}
								else if ( ( i % 3 ) == 1 ) {
									for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/3][k] , "" );
								}
								else {
									for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/3][k] , "" );
								}
								break;
							case 2 :
								if ( ( i % 2 ) == 0 ) {
									for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/2][k] , "" );
								}
								else {
									for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/2][k] , "" );
								}
								break;
							default :
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/1][k] , "" );
								break;
							}
						}
						switch ( Get_CLUSTER_USE_TYPE_ITEM() ) {
						case 3 :
							CLUSTER_RECIPE.MODULE[i/3][j] = PM_SUB_CURRENT_POS[j] + 1;
							if ( ( i % 3 ) == 0 ) {
								if ( PM_LOW_CONTROL[j] ) {
									if ( strlen( PM_LOW_DIRECTORY[j][2] ) > 0 ) {
										sprintf( CLUSTER_RECIPE.MODULE_PR_RECIPE[i/3][j] , "%s/%s" , PM_LOW_DIRECTORY[j][2] , Get_Select_PMFILE( hdlg , j ) );
									}
									else {
										strncpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i/3][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
									}
								}
								else {
									strncpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i/3][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
								}
							}
							else if ( ( i % 3 ) == 1 ) {
								if ( PM_LOW_CONTROL[j] ) {
									if ( strlen( PM_LOW_DIRECTORY[j][0] ) > 0 ) {
										sprintf( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/3][j] , "%s/%s" , PM_LOW_DIRECTORY[j][0] , Get_Select_PMFILE( hdlg , j ) );
									}
									else {
										strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/3][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
									}
								}
								else {
									strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/3][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
								}
							}
							else {
								if ( PM_LOW_CONTROL[j] ) {
									if ( strlen( PM_LOW_DIRECTORY[j][1] ) > 0 ) {
										sprintf( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/3][j] , "%s/%s" , PM_LOW_DIRECTORY[j][1] , Get_Select_PMFILE( hdlg , j ) );
									}
									else {
										strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/3][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
									}
								}
								else {
									strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/3][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
								}
							}
							break;
						case 2 :
							CLUSTER_RECIPE.MODULE[i/2][j] = PM_SUB_CURRENT_POS[j] + 1;
							if ( ( i % 2 ) == 0 ) {
								if ( PM_LOW_CONTROL[j] ) {
									if ( strlen( PM_LOW_DIRECTORY[j][0] ) > 0 ) {
										sprintf( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/2][j] , "%s/%s" , PM_LOW_DIRECTORY[j][0] , Get_Select_PMFILE( hdlg , j ) );
									}
									else {
										strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/2][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
									}
								}
								else {
									strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/2][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
								}
							}
							else {
								if ( PM_LOW_CONTROL[j] ) {
									if ( strlen( PM_LOW_DIRECTORY[j][1] ) > 0 ) {
										sprintf( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/2][j] , "%s/%s" , PM_LOW_DIRECTORY[j][1] , Get_Select_PMFILE( hdlg , j ) );
									}
									else {
										strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/2][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
									}
								}
								else {
									strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/2][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
								}
							}
							break;
						default :
							CLUSTER_RECIPE.MODULE[i/1][j] = PM_SUB_CURRENT_POS[j] + 1;
							if ( PM_LOW_CONTROL[j] ) {
								if ( strlen( PM_LOW_DIRECTORY[j][0] ) > 0 ) {
									sprintf( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/1][j] , "%s/%s" , PM_LOW_DIRECTORY[j][0] , Get_Select_PMFILE( hdlg , j ) );
								}
								else {
									strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/1][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
								}
							}
							else {
								strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/1][j] , Get_Select_PMFILE( hdlg , j ) , MAX_STRING_SIZE );
							}
							break;
						}
						i = ClusterData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , i );
					}
					if ( f != 0 ) {
						Set_CLUSTER_FILE_CHANGE( TRUE );
						//
						Set_Enable_CLUSTERFILE( hdlg , FALSE );
						//
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 0 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 1 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 1 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 1 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 0 );
						//
						ClusterData_Remapping();
						InvalidateRect( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , NULL , FALSE );
						ClusterData_Button_Set( hdlg , FALSE );
					}
					else {
						PROCESS_SEND_BUTTON_MAPPING( hdlg , j , 0 , 1 );
					}

					return TRUE;
				//==============================================================================
				case IDB_CLEAR_ENABLE_P1 :
				case IDB_CLEAR_ENABLE_P2 :
				case IDB_CLEAR_ENABLE_P3 :
				case IDB_CLEAR_ENABLE_P4 :
				case IDB_CLEAR_ENABLE_P5 :
				case IDB_CLEAR_ENABLE_P6 :
				case IDB_CLEAR_ENABLE_P7 :
				case IDB_CLEAR_ENABLE_P8 :
				case IDB_CLEAR_ENABLE_P9 :
				case IDB_CLEAR_ENABLE_P10 :
				case IDB_CLEAR_ENABLE_P11 :
				case IDB_CLEAR_ENABLE_P12 :
				case IDB_CLEAR_ENABLE2_P1 :
				case IDB_CLEAR_ENABLE2_P2 :
				case IDB_CLEAR_ENABLE2_P3 :
				case IDB_CLEAR_ENABLE2_P4 :
				case IDB_CLEAR_ENABLE2_P5 :
				case IDB_CLEAR_ENABLE2_P6 :
				case IDB_CLEAR_ENABLE2_P7 :
				case IDB_CLEAR_ENABLE2_P8 :
				case IDB_CLEAR_ENABLE2_P9 :
				case IDB_CLEAR_ENABLE2_P10 :
				case IDB_CLEAR_ENABLE2_P11 :
				case IDB_CLEAR_ENABLE2_P12 :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MessageBox( hdlg , "Operation Impossible(Read Only Mode)" , "Operation Impossible" , MB_ICONHAND );
						return TRUE;
					}

					if      ( wParam == IDB_CLEAR_ENABLE_P1 ) j = 0;
					else if ( wParam == IDB_CLEAR_ENABLE_P2 ) j = 1;
					else if ( wParam == IDB_CLEAR_ENABLE_P3 ) j = 2;
					else if ( wParam == IDB_CLEAR_ENABLE_P4 ) j = 3;
					else if ( wParam == IDB_CLEAR_ENABLE_P5 ) j = 4;
					else if ( wParam == IDB_CLEAR_ENABLE_P6 ) j = 5;
					else if ( wParam == IDB_CLEAR_ENABLE_P7 ) j = 6;
					else if ( wParam == IDB_CLEAR_ENABLE_P8 ) j = 7;
					else if ( wParam == IDB_CLEAR_ENABLE_P9 ) j = 8;
					else if ( wParam == IDB_CLEAR_ENABLE_P10 ) j = 9;
					else if ( wParam == IDB_CLEAR_ENABLE_P11 ) j = 10;
					else if ( wParam == IDB_CLEAR_ENABLE_P12 ) j = 11;
					else if ( wParam == IDB_CLEAR_ENABLE2_P1 ) j = 0;
					else if ( wParam == IDB_CLEAR_ENABLE2_P2 ) j = 1;
					else if ( wParam == IDB_CLEAR_ENABLE2_P3 ) j = 2;
					else if ( wParam == IDB_CLEAR_ENABLE2_P4 ) j = 3;
					else if ( wParam == IDB_CLEAR_ENABLE2_P5 ) j = 4;
					else if ( wParam == IDB_CLEAR_ENABLE2_P6 ) j = 5;
					else if ( wParam == IDB_CLEAR_ENABLE2_P7 ) j = 6;
					else if ( wParam == IDB_CLEAR_ENABLE2_P8 ) j = 7;
					else if ( wParam == IDB_CLEAR_ENABLE2_P9 ) j = 8;
					else if ( wParam == IDB_CLEAR_ENABLE2_P10 ) j = 9;
					else if ( wParam == IDB_CLEAR_ENABLE2_P11 ) j = 10;
					else if ( wParam == IDB_CLEAR_ENABLE2_P12 ) j = 11;
					PROCESS_SEND_BUTTON_MAPPING( hdlg , j , 1 , 2 );
					i = ClusterData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , -1 );
					if ( i >= 0 ) {
						Set_CLUSTER_FILE_CHANGE( TRUE );
						//
						Set_Enable_CLUSTERFILE( hdlg , FALSE );
						//
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 0 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 1 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 1 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 1 );
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 0 );
						//
						while ( i >= 0 ) {
							switch ( Get_CLUSTER_USE_TYPE_ITEM() ) {
							case 3 :
								if ( ( i % 3 ) == 0 ) {
									strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i/3][j] , "" );
								}
								else if ( ( i % 3 ) == 1 ) {
									strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/3][j] , "" );
								}
								else {
									strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/3][j] , "" );
								}
								break;
							case 2 :
								if ( ( i % 2 ) == 0 ) {
									strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/2][j] , "" );
								}
								else {
									strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i/2][j] , "" );
								}
								break;
							default :
								strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i/1][j] , "" );
								break;
							}
							i = ClusterData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , i );
						}
						ClusterData_Remapping();
						InvalidateRect( GetDlgItem( hdlg , IDC_LIST_CLUSTERDATA ) , NULL , FALSE );
						ClusterData_Button_Set( hdlg , FALSE );
					}
					else {
						PROCESS_SEND_BUTTON_MAPPING( hdlg , j , 1 , 1 );
					}

					return TRUE;
				//==============================================================================
				//==============================================================================
				//==============================================================================
				//==============================================================================
				case IDC_CTRL_CHANGE_PM1 :
				case IDC_CTRL_CHANGE_PM2 :
				case IDC_CTRL_CHANGE_PM3 :
					if      ( wParam == IDC_CTRL_CHANGE_PM1 ) j = 0;
					else if ( wParam == IDC_CTRL_CHANGE_PM2 ) j = 1;
					else if ( wParam == IDC_CTRL_CHANGE_PM3 ) j = 2;
					if ( strlen( PM_SUB_DIRECTORY[j][0] ) <= 0 ) return TRUE;
					strcpy( Buffer , "" );
					for ( i = 0 ; i < MAX_SUB_CHAMBER_DEPTH ; i++ ) {
						if ( strlen( PM_SUB_DIRECTORY[j][i] ) <= 0 ) break;
						if ( i != 0 ) strcat( Buffer , "|" );
						strcat( Buffer , PM_SUB_DIRECTORY[j][i] );
					}
					i = PM_SUB_CURRENT_POS[j];
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Mode" , Buffer , &i ) ) {
						PM_SUB_CURRENT_POS[j] = i;
						KWIN_Item_String_Display( hdlg , wParam , PM_SUB_DIRECTORY[j][PM_SUB_CURRENT_POS[j]] );
						if ( j ==  0 ) FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM1FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 0 ) , PM_SUB_DIRECTORY[0][ PM_SUB_CURRENT_POS[0] ] , PM_LOW_DIRECTORY[0][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[0][USE_SELECT_CURRENT_PM_TYPE] );
						if ( j ==  1 ) FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM2FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 1 ) , PM_SUB_DIRECTORY[1][ PM_SUB_CURRENT_POS[1] ] , PM_LOW_DIRECTORY[1][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[1][USE_SELECT_CURRENT_PM_TYPE] );
						if ( j ==  2 ) FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_PM3FILE ) , Get_Prm_PROCESS_RECIPE_PATH( 2 ) , PM_SUB_DIRECTORY[2][ PM_SUB_CURRENT_POS[2] ] , PM_LOW_DIRECTORY[2][USE_SELECT_CURRENT_PM_TYPE] , PM_LOW_EXTENSION[2][USE_SELECT_CURRENT_PM_TYPE] );
						Set_Select_PMFILE( hdlg , j , "" );
						ClusterData_Button_Set( hdlg , FALSE );
					}
					return TRUE;

				//==============================================================================
				//==============================================================================
				//==============================================================================
				//==============================================================================
				case IDB_NEW_ENABLE :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MessageBox( hdlg , "New Impossible(Read Only Mode)" , "New Impossible" , MB_ICONHAND );
					}
					else {
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 2 );
						if ( !Get_Prm_MONITOR_MODE() ) {
							switch( Get_Prm_NAME_STYLE() ) { // 2004.06.15
							case 1 :
								strcpy( Buffer3 , "upper" );
								break;
							case 2 :
								strcpy( Buffer3 , "lower" );
								break;
							default :
								strcpy( Buffer3 , "" );
								break;
							}
						}
						else {
							switch( Get_Prm_NAME_STYLE() ) { // 2004.06.15
							case 1 :
								strcpy( Buffer3 , "Remote|upper" );
								break;
							case 2 :
								strcpy( Buffer3 , "Remote|lower" );
								break;
							default :
								strcpy( Buffer3 , "Remote" );
								break;
							}
							MODAL_SET_REMOTE_ACCESS_API_FOR_FINDFILE( _nINF_KNFS_FindFirstFile , _nINF_KNFS_FindNextFile , _nINF_KNFS_FindClose );
						}
						sprintf( Buffer , "%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_CLUSTER_RECIPE_PATH() );
						if ( MODAL_FILE_SAVE_BOX( hdlg , Buffer , "Type New File Name?" , "*.*" , Buffer3 , Buffer2 ) ) {
							//
							Change_Name_Type_With_Option( Buffer2 , Get_Prm_NAME_STYLE() );// 2004.06.15
							//
							All2FileName( Buffer , Buffer2 , 1024 );
							//
							if ( strlen( Buffer ) > (unsigned) MAX_FILENAME_SIZE ) { // 2007.03.13
								MessageBox( hdlg , "File Name Size too long" , "SAVE Impossible" , MB_ICONHAND ); // 2007.03.13
								FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 1 ); // 2007.03.13
							} // 2007.03.13
							else {
								USE_SELECT_CURRENT_PM_TYPE = 0; // 2002.11.21
								//
								//-------------------------------------------------------------------------------------------------------
								RECIPE_LOG_HANDLING( "NEW" , "New with (%s)\n" , Buffer ); // 2004.01.08
								//-------------------------------------------------------------------------------------------------------
								Kor_CLUSTER_Initial();
								//
								Console_Cluster_File_Save( hdlg , Buffer , 0 );
								FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() , "" , "" , "*.*" );
								Console_Cluster_File_Open( hdlg , Buffer );
								//=======================================================================================
								//=======================================================================================
								// 2004.11.08 (CREATE EVENT)
								//=======================================================================================
								//=======================================================================================
								if ( STRCMP( Get_Prm_GROUP_RECIPE_PATH() , "." ) ) { // No Group
									sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s|%d|%d" , Buffer /*FileName*/ , 0 /*Step for Process*/ , 0 /*Create*/ );
									Set_Event_String_For_Scheduler( Buffer3 );
								}
								else { // Use Group
									sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s:(%s)|%d|%d" , Buffer /*FileName*/ , Get_Prm_GROUP_RECIPE_PATH() , 0 /*Step for Process*/ , 0 /*Create*/ );
									Set_Event_String_For_Scheduler( Buffer3 );
								}
								//=======================================================================================
							}
						}
						else {
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 1 );
						}
					}
					return TRUE;

				case IDB_SAVE_ENABLE :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MessageBox( hdlg , "Save Impossible(Read Only Mode)" , "Save Impossible" , MB_ICONHAND );
					}
					else {
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 2 );
						ClusterData_Remapping();
						Get_Select_CLUSTERFILE( hdlg , Buffer , 255 );
						//-------------------------------------------------------------------------------------------------------
						RECIPE_LOG_HANDLING( "SAVE" , "Save to (%s)\n" , Buffer ); // 2004.01.08
						//-------------------------------------------------------------------------------------------------------
						Console_Cluster_File_Save( hdlg , Buffer , 1 );
						//=======================================================================================
						//=======================================================================================
						// 2004.11.08 (UPDATE EVENT)
						//=======================================================================================
						//=======================================================================================
						if ( STRCMP( Get_Prm_GROUP_RECIPE_PATH() , "." ) ) { // No Group
							sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s|%d|%d" , Buffer /*FileName*/ , 0 /*Step for Process*/ , 1 /*Update*/ );
							Set_Event_String_For_Scheduler( Buffer3 );
						}
						else { // Use Group
							sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s:(%s)|%d|%d" , Buffer /*FileName*/ , Get_Prm_GROUP_RECIPE_PATH() , 0 /*Step for Process*/ , 1 /*Update*/ );
							Set_Event_String_For_Scheduler( Buffer3 );
						}
						//=======================================================================================
						//=======================================================================================
					}
					return TRUE;

				case IDB_SAVEAS_ENABLE :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MessageBox( hdlg , "Save Impossible(Read Only Mode)" , "Save Impossible" , MB_ICONHAND );
					}
					else {
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 2 );
						if ( !Get_Prm_MONITOR_MODE() ) {
							switch( Get_Prm_NAME_STYLE() ) { // 2004.06.15
							case 1 :
								strcpy( Buffer3 , "upper" );
								if ( SAVE_PRESET_MODE ) strcat( Buffer3 , "|Preset" );
								break;
							case 2 :
								strcpy( Buffer3 , "lower" );
								if ( SAVE_PRESET_MODE ) strcat( Buffer3 , "|Preset" );
								break;
							default :
								strcpy( Buffer3 , "" );
								if ( SAVE_PRESET_MODE ) strcat( Buffer3 , "Preset" );
								break;
							}
						}
						else {
							switch( Get_Prm_NAME_STYLE() ) { // 2004.06.15
							case 1 :
								strcpy( Buffer3 , "Remote|upper" );
								if ( SAVE_PRESET_MODE ) strcat( Buffer3 , "|Preset" );
								break;
							case 2 :
								strcpy( Buffer3 , "Remote|lower" );
								if ( SAVE_PRESET_MODE ) strcat( Buffer3 , "|Preset" );
								break;
							default :
								strcpy( Buffer3 , "Remote" );
								if ( SAVE_PRESET_MODE ) strcat( Buffer3 , "|Preset" );
								break;
							}
							MODAL_SET_REMOTE_ACCESS_API_FOR_FINDFILE( _nINF_KNFS_FindFirstFile , _nINF_KNFS_FindNextFile , _nINF_KNFS_FindClose );
						}
						sprintf( Buffer , "%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_CLUSTER_RECIPE_PATH() );
						if ( MODAL_FILE_SAVE_BOX( hdlg , Buffer , "Type Save as File Name?" , "*.*" , Buffer3 , Buffer2 ) ) {
							Change_Name_Type_With_Option( Buffer2 , Get_Prm_NAME_STYLE() );// 2004.06.15
							//-------------------------------------------------------------------------------------------------------
							All2FileName( Buffer , Buffer2 , 1024 );
							//
							if ( strlen( Buffer ) > (unsigned) MAX_FILENAME_SIZE ) { // 2007.03.13
								MessageBox( hdlg , "File Name Size too long" , "Save as Impossible" , MB_ICONHAND ); // 2007.03.13
								FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 1 ); // 2007.03.13
							} // 2007.03.13
							else {
								//-------------------------------------------------------------------------------------------------------
								RECIPE_LOG_HANDLING( "SAVEAS" , "Save as to (%s)\n" , Buffer ); // 2004.01.08
								//-------------------------------------------------------------------------------------------------------
								ClusterData_Remapping();
								//=======================================================================================
								//=======================================================================================
								// 2004.11.08 (CREATE or UPDATE EVENT)
								//=======================================================================================
								//=======================================================================================
								sprintf( Buffer3 , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_CLUSTER_RECIPE_PATH() , Buffer );
								hFind = FindFirstFile( Buffer3 , &fd );
								if ( hFind != INVALID_HANDLE_VALUE ) {
									FindClose( hFind );
									EditStatus = 1 /*Update*/;
								}
								else EditStatus = 0; /*Create*/
								//=======================================================================================
								//=======================================================================================
								Console_Cluster_File_Save( hdlg , Buffer , ( EditStatus == 1 ) ? 3 : 2 );
								FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() , "" , "" , "*.*" );
								Console_Cluster_File_Open( hdlg , Buffer );
								//=======================================================================================
								//=======================================================================================
								// 2004.11.08 (CREATE or UPDATE EVENT)
								//=======================================================================================
								//=======================================================================================
								if ( STRCMP( Get_Prm_GROUP_RECIPE_PATH() , "." ) ) { // No Group
									sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s|%d|%d" , Buffer /*FileName*/ , 0 /*Step for Process*/ , EditStatus );
									Set_Event_String_For_Scheduler( Buffer3 );
								}
								else { // Use Group
									sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s:(%s)|%d|%d" , Buffer /*FileName*/ , Get_Prm_GROUP_RECIPE_PATH() , 0 /*Step for Process*/ , EditStatus );
									Set_Event_String_For_Scheduler( Buffer3 );
								}
								//=======================================================================================
							}
						}
						else {
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 1 );
						}
					}
					return TRUE;

				case IDB_CANCEL_ENABLE :
					FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 2 );
					if ( IDYES == MessageBox( hdlg , "Really Cancel edited data and reload last saved data ???" , "CANCEL Check" , MB_ICONQUESTION | MB_YESNO ) ) {
						Get_Select_CLUSTERFILE( hdlg , Buffer , 255 );
						//-------------------------------------------------------------------------------------------------------
						RECIPE_LOG_HANDLING( "CANCEL" , "Cancel (%s)\n" , Buffer ); // 2004.01.08
						//-------------------------------------------------------------------------------------------------------
						Console_Cluster_File_Open( hdlg , Buffer );
					}
					else {
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 1 );
					}
					return TRUE;

				case IDB_FILE_ENABLE :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MessageBox( hdlg , "Delete Impossible(Read Only Mode)" , "Delete Impossible" , MB_ICONHAND );
					}
					else {
						FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 2 );
						if ( IDYES == MessageBox( hdlg , "Really Delete Select File" , "DELETE Check" , MB_ICONQUESTION | MB_YESNO ) ) {
							Get_Select_CLUSTERFILE( hdlg , Buffer , 255 );
							//-------------------------------------------------------------------------------------------------------
							RECIPE_LOG_HANDLING( "DELETE" , "Delete (%s)\n" , Buffer ); // 2004.01.08
							//-------------------------------------------------------------------------------------------------------
							sprintf( Buffer2 , "%s/%s/%s/%s" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_CLUSTER_RECIPE_PATH() , Buffer );
							if ( !Get_Prm_MONITOR_MODE() ) {
								//DeleteFile( Buffer2 );
								Kor_CLUSTER_Delete( Buffer2 , 0 , Get_Prm_GROUP_RECIPE_PATH() );
							}
							else { // 2003.12.15
								_nINF_KNFS_DeleteFile( Buffer2 );
								sprintf( Buffer2 , "%s/%s/%s/%s/%s" , Get_Prm_MONITOR_FOLDER() , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , Get_Prm_CLUSTER_RECIPE_PATH() , Buffer );
								//DeleteFile( Buffer2 );
								Kor_CLUSTER_Delete( Buffer2 , 0 , Get_Prm_GROUP_RECIPE_PATH() );
							}
							//
							Set_CLUSTER_FILE_CHANGE( FALSE );
							//
							Set_Select_CLUSTERFILE( hdlg , "" );
							//
							Kor_CLUSTER_Initial();
							//
							FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_CLUSTERFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() , "" , "" , "*.*" );
							//
							Set_CLUSTERDATA_DISPLAY( hdlg , FALSE );
							//
							Set_Enable_CLUSTERFILE( hdlg , TRUE );
							//
							ClusterData_Button_Init( hdlg , 0 , FALSE );
							//
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_NEW , 1 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVE , 0 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_SAVEAS , 0 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_CANCEL , 0 );
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 0 );
							//
							//=======================================================================================
							//=======================================================================================
							// 2004.11.08 (DELETE EVENT)
							//=======================================================================================
							//=======================================================================================
							if ( STRCMP( Get_Prm_GROUP_RECIPE_PATH() , "." ) ) { // No Group
								sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s|%d|%d" , Buffer /*FileName*/ , 0 /*Step for Process*/ , 2 /*Delete*/ );
								Set_Event_String_For_Scheduler( Buffer3 );
							}
							else { // Use Group
								sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT CLT|%s:(%s)|%d|%d" , Buffer /*FileName*/ , Get_Prm_GROUP_RECIPE_PATH() , 0 /*Step for Process*/ , 2 /*Delete*/ );
								Set_Event_String_For_Scheduler( Buffer3 );
							}
							//=======================================================================================
							//=======================================================================================
						}
						else {
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_FILE , 1 );
						}
					}
					return TRUE;

			}
			return TRUE;

		case WM_DESTROY:
			if ( uTimer ) KillTimer( hdlg , uTimer );
			return TRUE;

	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	HWND hWnd;
	//----------------------------------
//	REGIST_FILE_INFO( "Gui Cluster Editor 2.2(2001.08.21)" );
//	REGIST_FILE_INFO( "Gui Cluster Editor 2.3(2001.11.20)" ); // File Read upto 3 times
//	REGIST_FILE_INFO( "Gui Cluster Editor 2.4(2001.12.04)" ); // File Read
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.0(2002.01.01)" ); // CimSqCon Change Rebuild
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.1(2002.05.20)" ); // Append EX_Parallel , Update EX_Chamber
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.2(2002.05.31)" ); // MaxView Type Display/Data Insert Bug Update
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.3(2002.06.17)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.4(2002.10.03)" ); // Append Title Show
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.5(2002.11.08)" ); // Append Readonly Mode
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.6(2002.11.14)" ); // Append Group button Hide Mode
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.7(2002.11.22)" ); // Append Sub path per type
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.8(2003.07.12)" ); // Append Wait Receive Spawn Message
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.9(2003.12.15)" ); // Append MonitorMode
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.91(2004.01.08)" ); // Append Log System & Update Access Control Part
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.92(2004.01.19)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.93(2004.03.22)" ); // Append Sub extension per type 
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.94(2004.04.06)" ); // Monitor Mode Bug Update
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.95(2004.06.15)" ); // Upper,Lower Case Append
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.96(2004.08.10)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.97(2004.11.08)" ); // Append FA Log Message for File Operation
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.98(2005.02.28)" ); // CPU OverHead Bug Update
//	REGIST_FILE_INFO( "Gui Cluster Editor 3.99(2005.07.19)" ); // Append All Move Button
//	REGIST_FILE_INFO( "Gui Cluster Editor 4.0(2005.09.12)" ); // EX_PARALLEL , EX_EXCLUSIVE Max Count Bug Update
//	REGIST_FILE_INFO( "Gui Cluster Editor 4.01(2006.04.27)" ); // Max View Mode Unuse Module Display Bug Update
//	REGIST_FILE_INFO( "Gui Cluster Editor 4.02(2006.08.23)" ); // File Save Bug Update
//	REGIST_FILE_INFO( "Gui Cluster Editor 4.03(2007.09.18)" ); // File Search Part Append
//	REGIST_FILE_INFO( "Gui Cluster Editor 4.04(2007.12.28)" ); // Max View File name Size bug update
	REGIST_FILE_INFO( "Gui Cluster Editor 4.05(2008.02.15)" ); // File Name Size 50/64/128
	//----------------------------------
	while( PROGRAM_GUI_SPAWNING == 0 ) { _sleep(100); } // 2003.07.12
	//----------------------------------
	InitCommonControls();
	//----------------------------------
	KGUI_Set_BACKGROUND( PROGRAM_PARAMETER_READ() );
	KWIN_DIALOG_DRIVER_LOADING();

	//--------------------------------------------------------------------------------------------------------------
	MAIN_WINDOW_REFRESH = FALSE; // 2004.04.06
	//--------------------------------------------------------------------------------------------------------------
	do {
		hWnd = GET_MAIN_WINDOW();
		GoModalDialogBoxParam(GETHINST(hWnd), KWIN_MAKEINTRESOURCE(IDD_MAIN_CONTROL_M, IDD_E_MAIN_CONTROL_M), hWnd, Console_Gui_For_Main_Proc, (LPARAM) NULL);
	}
	while ( MAIN_WINDOW_REFRESH );
	//--------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------
	KWIN_DIALOG_DRIVER_UNLOADING();
	if ( KGUI_Get_BACKGROUND() ) FreeLibrary( KGUI_Get_BACKGROUND() );
	PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
	return( SYS_SUCCESS );
}

//////////////////////////////////////////////////////////////////////////
void Program_Enter_Code() {
	char    NAME1[ 128 + 1 ];
	char	NAME2[ 128 + 1 ];
	char	NAME3[ 128 + 1 ];
	char	TYPE[ 128 + 1 ];
	char	LENGTH[ 128 + 1 ];
	char	CLOSE[ 128 + 1 ];
	char	MONS[ 128 + 1 ];
	char	ELSE[ 128 + 1 ];
	char	ELSE2[ 128 + 1 ];
	int i , j;

	//-----------------------------------------------------------
	// When Simulation Test Use
	//-----------------------------------------------------------
	//PROGRAM_PARAMETER_SET( "" );
	//PROGRAM_ARGUMENT_SET( "scheduler|GM|2|6|" );
	//PROGRAM_SCREENDATA_SET( -1 );
	//-----------------------------------------------------------

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		PM_SUB_CURRENT_POS[i] = 0;
		for ( j = 0 ; j < MAX_SUB_CHAMBER_DEPTH ; j++ )
			strcpy( PM_SUB_DIRECTORY[i][j] , "" );
		PM_LOW_CONTROL[i] = FALSE;
		for ( j = 0 ; j < 3 ; j++ ) {
			//strcpy( PM_LOW_DIRECTORY[i][3] , "" ); // 2004.03.22
			strcpy( PM_LOW_DIRECTORY[i][j] , "" ); // 2004.03.22
			strcpy( PM_LOW_EXTENSION[i][j] , "*.*" ); // 2004.03.22
		}
	}

	STR_SEPERATE_CHAR( PROGRAM_ARGUMENT_READ() , '|' , NAME1 , LENGTH , 128 );
	STR_SEPERATE_CHAR( LENGTH , '|' , MONS , ELSE2 , 128 );
	STR_SEPERATE_CHAR( MONS , ':' , NAME2 , NAME3 , 128 ); // 2003.12.28
	STR_SEPERATE_CHAR( ELSE2 , '|' , TYPE , ELSE , 128 );
	STR_SEPERATE_CHAR( ELSE , '|' , LENGTH , ELSE2 , 128 );
	STR_SEPERATE_CHAR( ELSE2 , '|' , CLOSE , MONS , 128 );

	switch ( atoi( TYPE ) ) {
	case 2 : Set_CLUSTER_USE_TYPE_ITEM( 2 ); break;
	case 3 : Set_CLUSTER_USE_TYPE_ITEM( 3 ); break;
	default: Set_CLUSTER_USE_TYPE_ITEM( 1 ); break;
	}
	//
	Set_Prm_READONLY_LEVEL_IO_NAME( NAME3 ); // 2003.12.28
	//
	Set_GROUP_RUN_CHECK( FALSE );
	Set_Prm_READONLY_LEVEL( 0 );
	Set_Prm_NAME_STYLE( 0 ); // 2004.06.15
	SAVE_PRESET_MODE = FALSE; // 2008.02.14
	
	//////////////////////////////////////////////////////////////////////////
	//.. "TBMG"
	for ( i = 0 ; i < (signed) strlen( NAME2 ) ; i++ ) 
	{
		if ( tolower( NAME2[i] ) == 'g' ) Set_GROUP_RUN_CHECK( TRUE );
		if ( tolower( NAME2[i] ) == 'u' ) Set_Prm_NAME_STYLE( 1 ); // 2004.06.15
		if ( tolower( NAME2[i] ) == 'l' ) Set_Prm_NAME_STYLE( 2 ); // 2004.06.15
		if ( tolower( NAME2[i] ) == 'p' ) SAVE_PRESET_MODE = TRUE; // 2008.02.14
		if ( tolower( NAME2[i] ) == '0' ) Set_Prm_READONLY_LEVEL( 0 );
		if ( tolower( NAME2[i] ) == '1' ) Set_Prm_READONLY_LEVEL( 1 );
		if ( tolower( NAME2[i] ) == '2' ) Set_Prm_READONLY_LEVEL( 2 );
		if ( tolower( NAME2[i] ) == '3' ) Set_Prm_READONLY_LEVEL( 3 );
		if ( tolower( NAME2[i] ) == '4' ) Set_Prm_READONLY_LEVEL( 4 );
		if ( tolower( NAME2[i] ) == '5' ) Set_Prm_READONLY_LEVEL( 5 );
		if ( tolower( NAME2[i] ) == '6' ) Set_Prm_READONLY_LEVEL( 6 );
		if ( tolower( NAME2[i] ) == '7' ) Set_Prm_READONLY_LEVEL( 7 );
		if ( tolower( NAME2[i] ) == '8' ) Set_Prm_READONLY_LEVEL( 8 );
		if ( tolower( NAME2[i] ) == '9' ) Set_Prm_READONLY_LEVEL( 9 );
		if ( tolower( NAME2[i] ) == 'w' ) MAX_FILENAME_SIZE = 64; // 2008.02.14
		if ( tolower( NAME2[i] ) == 'y' ) MAX_FILENAME_SIZE = 128; // 2008.02.14
	}
	//
	j = atoi( LENGTH );
	if ( ( j > 0 ) && ( j <= MAX_CLUSTER_DEPTH ) )
		USE_CLUSTERDATA_ITEM_COUNT = j;
	//
	Set_Prm_Scheduler_Path( NAME1 );
	Set_Prm_Scheduler_CloseString( CLOSE );
	//
	DISPLAY_MAX_VIEW_COUNT = 7;
	DISPLAY_MAX_VIEW_START_Y = 70;
	//
	DISPLAY_MAX_VIEW_UP2_Y	=	85;
	DISPLAY_MAX_VIEW_UP_Y	=	135;
	DISPLAY_MAX_VIEW_DN_Y	=	185;
	DISPLAY_MAX_VIEW_DN2_Y	=	235;
	//
	if ( strlen( MONS ) > 0 ) { // 2003.12.17
		Set_Prm_MONITOR_MODE( TRUE );
		STR_SEPERATE_CHAR( MONS , ':' , NAME1 , NAME2 , 128 );
		sprintf( ELSE , "$Remote$\\[%s][%s]" , NAME1 , NAME2 );
		Set_Prm_MONITOR_FOLDER( ELSE );
		_nINF_KNFS_Prepare_for_ConnectionName( PROGRAM_FUNCTION_READ() );
		if ( _nINF_KNFS_CONNECTION_FOR_CLIENT( NAME1 , atoi(NAME2) ) != 0 ) {
			_IO_CIM_PRINTF( "Network KNFS Initialization Error\n" );
			return;
		}
	}
	else {
		Set_Prm_MONITOR_MODE( FALSE );
	}
	if ( !Get_Prm_MONITOR_MODE() ) {
		if ( !FILE_ALL_SETUP( Get_Prm_Scheduler_Path() , Get_Prm_Scheduler_CloseString() ) ) {
			MessageBeep(MB_ICONHAND);
		}
	}
}
//-------------------------------------------------------------------------
