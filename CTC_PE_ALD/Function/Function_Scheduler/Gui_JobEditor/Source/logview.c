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
#include "stepview.h"
#include "Jobdata.h"
#include "appPlotting.h"

#include "cimseqnc.h"
#include "resource.h"

#include <CimSeqncKNFS.h>

extern int GLISTVIEW_HEIGHT;


extern int FLOW_HISTORY_M_INDEX[MAX_MULTI_CHECK_JOB];

extern char   Step_Log_FileName[128]; // 2018.04.24
extern char   Step_Log_StepName[MAX_STEP_COUNT][64]; // 2018.04.24
extern int    Step_Log_StepStart[MAX_STEP_COUNT]; // 2018.04.24
extern int    Step_Log_StepEnd[MAX_STEP_COUNT]; // 2018.04.24


double CURRENT_WAFER_Get_Min_Sec( int currview );
double CURRENT_WAFER_Get_Max_Sec( int currview );

double CURRENT_WAFER_GET_TIME( int , char *Timestr );

void CURRENT_WAFER_INIT( int init );
int CURRENT_WAFER_APPEND_MODULE_F( int jobindex , int id , int sx , int mode , int tm , char *name , double st , double et );

void LOT_ANAL_DATA_APPEND( int jobindex , int mindex , int , char *timestr , char * , char *Str_Action_Message , char *Str_Module_Message , char *Str_ArmSlot_Message );

extern int    StepView_TimeView_Option; // xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int  CURRENT_WAFER_ID[MAX_CURRENT_WAFER_VIEW];
extern int  CURRENT_WAFER_DUMMYIN[MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_WAFER_MODULE_DEPTH[MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_WAFER_MODULE_MODE[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH];
extern int   CURRENT_WAFER_MODULE_TM[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH];
extern char *CURRENT_WAFER_MODULE_NAME[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH];
extern double CURRENT_WAFER_MODULE_START[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH]; // 2012.11.26
extern double CURRENT_WAFER_MODULE_END[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH]; // 2012.11.26
//
extern char *CURRENT_MODULE_VIEW_NAME[MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_MODULE_VIEW_DEPTH[MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_MODULE_VIEW_MINDEX[MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_MODULE_VIEW_1[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_VIEW];
extern int   CURRENT_MODULE_VIEW_2[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_VIEW];
//
extern int    CURRENT_WAFER_Data_Set_Min_Mode; // 2012.11.26
extern int    CURRENT_WAFER_Data_Set_Min_Index; // 2012.11.26
extern int    CURRENT_WAFER_Data_Set_Min_Dev; // 2012.11.26
extern BOOL   CURRENT_WAFER_Data_Set_MultiMode;
extern int    CURRENT_WAFER_Data_Set_View_Mode; // TimeLine / SameLine 2017.03.17

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
double CURRENT_WAFER_Get_Min_Sec( int currview );
double CURRENT_WAFER_Get_Max_Sec( int currview );


#define	MAX_READ_SLEEP_LINE	200
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int  LOG_ENCRIPTION; // 2010.04.14
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void JOBDATA_S2_DISPLAY_CONTROL( HWND hWnd , int wParam , int from , char *message , char *data , char *data2 );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int		STR_9003_MINMAX_USE[MAX_X_TABLE];
double	STR_9003_MIN[MAX_X_TABLE];
double	STR_9003_MAX[MAX_X_TABLE];

//
//================================

int    Dat_Count_Total; // 2016.08.18
int    Dat_Count_Total_Step[MAX_STEP_COUNT];

//================================

BOOL   Dat_Set_StepAll[MAX_X_TABLE]; // 2005.02.18
double Dat_Sum_StepAll[MAX_X_TABLE]; // 2005.02.18
double Dat_Sum2_StepAll[MAX_X_TABLE]; // 2016.08.18
double Dat_First_StepAll[MAX_X_TABLE]; // 2016.08.18
double Dat_Last_StepAll[MAX_X_TABLE]; // 2005.02.18
double Dat_Min_StepAll[MAX_X_TABLE]; // 2005.02.18
double Dat_Max_StepAll[MAX_X_TABLE]; // 2005.02.18

int    Dat_Count_Sum_StepAll[MAX_X_TABLE];
int    Dat_Count_MinMax_StepAll[MAX_X_TABLE];

//================================

int    Dat_Curr_Step;


BOOL   Dat_Set_StepUse[MAX_STEP_COUNT]; // 2011.08.10

BOOL   Dat_Set_Step[MAX_STEP_COUNT][MAX_X_TABLE]; // 2005.02.18
double Dat_Sum_Step[MAX_STEP_COUNT][MAX_X_TABLE]; // 2005.02.18
double Dat_Sum2_Step[MAX_STEP_COUNT][MAX_X_TABLE]; // 2016.08.18
double Dat_First_Step[MAX_STEP_COUNT][MAX_X_TABLE]; // 2016.08.18
double Dat_Last_Step[MAX_STEP_COUNT][MAX_X_TABLE]; // 2016.08.18
double Dat_Min_Step[MAX_STEP_COUNT][MAX_X_TABLE]; // 2005.02.18
double Dat_Max_Step[MAX_STEP_COUNT][MAX_X_TABLE]; // 2005.02.18

int    Dat_Count_Sum_Step[MAX_STEP_COUNT][MAX_X_TABLE];
int    Dat_Count_MinMax_Step[MAX_STEP_COUNT][MAX_X_TABLE];


void DATA_BUFFER_RESET( int StepData ) { // 2016.08.19
	int j , k;
	//
	Dat_Count_Total = 0;
	//
	for ( j = 0 ; j < MAX_X_TABLE ; j++ ) {
		Dat_Set_StepAll[j] = FALSE;
		Dat_Sum_StepAll[j] = 0;
		Dat_Sum2_StepAll[j] = 0;
		Dat_First_StepAll[j] = 0;
		Dat_Last_StepAll[j] = 0;
		Dat_Min_StepAll[j] =  999999999;
		Dat_Max_StepAll[j] = -999999999;
		//
		Dat_Count_Sum_StepAll[j] = 0;
		Dat_Count_MinMax_StepAll[j] = 0;
		//
	}
	//
	if ( StepData < 0 ) return;
	//
	Dat_Curr_Step = StepData;
	//
	for ( k = 0 ; k < MAX_STEP_COUNT ; k++ ) {
		//
		Dat_Set_StepUse[k] = FALSE;
		//
		Dat_Count_Total_Step[ k ] = 0;
		//
		for ( j = 0 ; j < MAX_X_TABLE ; j++ ) {
			Dat_Set_Step[k][j] = FALSE;
			Dat_Sum_Step[k][j] = 0;
			Dat_Sum2_Step[k][j] = 0;
			Dat_First_Step[k][j] = 0;
			Dat_Last_Step[k][j] = 0;
			Dat_Min_Step[k][j] =  999999999;
			Dat_Max_Step[k][j] = -999999999;
			//
			Dat_Count_Sum_Step[k][j] = 0;
			Dat_Count_MinMax_Step[k][j] = 0;
		}
	}
}

//------------------------------------------------------------------------------------------
void Log_Get_String_Form( HWND hWnd , int index , int start , int end , FILE *fpt );
//------------------------------------------------------------------------------------------
void Data_Read_Start( HWND hdlg ) {
	if ( hdlg == NULL ) return;
	SetCursor( LoadCursor( NULL , MAKEINTRESOURCE( IDC_WAIT ) ) );
}

void Data_Read_End( HWND hdlg ) {
	if ( hdlg == NULL ) return;
	SetCursor( LoadCursor( NULL , MAKEINTRESOURCE( IDC_ARROW ) ) );
}

void Get_Log_File_Name( char *Buffer , char *catstr , int mode0 , char *logsubfolder , BOOL slotisdummy , int slot , int slot2 , char *slotstr0 , int ch , char *chstr0 , char *appstr ) {
	char slotstr[64];
	char chstr[64];
	char buff[64];
	int mode , upm;
	//
	mode = mode0 % 10;
	upm = mode0 / 10;
	//
	if      ( ( slot > 0 ) && ( slot2 > 0 ) ) {
		if      ( ( slot >= 100 ) && ( slot2 >= 100 ) ) {
			sprintf( slotstr , "A%02d_D%02d_" , slot % 100 , slot2 % 100 );
		}
		else if ( ( slot >= 100 ) && ( slot2 < 100 ) ) {
			sprintf( slotstr , "A%02d_%02d_" , slot % 100 , slot2 % 100 );
		}
		else if ( ( slot < 100 ) && ( slot2 >= 100 ) ) {
			sprintf( slotstr , "%02d_D%02d_" , slot % 100 , slot2 % 100 );
		}
		else {
			sprintf( slotstr , "%02d_%02d_" , slot % 100 , slot2 % 100 );
		}
	}
	else if ( ( slot > 0 ) && ( slot2 <= 0 ) ) {
		if      ( slot >= 100 ) {
			if ( slotisdummy ) {
				sprintf( slotstr , "D%02d_" , slot % 100 );
			}
			else {
				sprintf( slotstr , "A%02d_" , slot % 100 );
			}
		}
		else {
			sprintf( slotstr , "%02d_" , slot % 100 );
		}
	}
	else if ( ( slot <= 0 ) && ( slot2 > 0 ) ) {
		if      ( slot2 >= 100 ) {
			sprintf( slotstr , "00_D%02d_" , slot2 % 100 );
		}
		else {
			sprintf( slotstr , "00_%02d_" , slot2 % 100 );
		}
	}
	else {
		if ( ( slotstr0 == NULL ) || ( slotstr0[0] == 0 ) ) {
			strcpy( slotstr , "" );
		}
		else {
			sprintf( slotstr , "%s_" , slotstr0 );
		}
	}
	//
	if ( ch <= 0 ) {
		if ( ( chstr0 == NULL ) || ( chstr0[0] == 0 ) ) {
			strcpy( chstr , "" );
		}
		else {
			//
			/*
			// 2013.12.05
			if ( mode == 0 ) {
				sprintf( chstr , "%s" , chstr0 );
			}
			else {
				sprintf( chstr , "%s_" , chstr0 );
			}
			*/
			//
			// 2013.12.05
			STR_SEPERATE_CHAR( chstr0 , ':' , chstr , buff , 63 );
			if ( mode != 0 ) strcat( chstr , "_" );
			//
		}
	}
	else {
		if ( mode == 0 ) {
			sprintf( chstr , "PM%d" , ch );
		}
		else {
			sprintf( chstr , "PM%d_" , ch );
		}
	}
	//
	if ( upm == 1 ) {
		if      ( mode == 5 ) {
			sprintf( Buffer , "%s/%s_%sLOTPOST%s" , logsubfolder , catstr , chstr , appstr );
		}
		else if ( mode == 4 ) {
			sprintf( Buffer , "%s/%s_%sLOTPRE%s" , logsubfolder , catstr , chstr , appstr );
		}
		else if ( mode == 3 ) {
			sprintf( Buffer , "%s/%s_%s%sPOST%s" , logsubfolder , catstr , slotstr , chstr , appstr );
		}
		else if ( mode == 2 ) {
			sprintf( Buffer , "%s/%s_%s%sPRE%s" , logsubfolder , catstr , slotstr , chstr , appstr );
		}
		else if ( mode == 1 ) {
			sprintf( Buffer , "%s/%s_%s%sALL%s" , logsubfolder , catstr , slotstr , chstr , appstr );
		}
		else {
			sprintf( Buffer , "%s/%s_%s%s%s" , logsubfolder , catstr , slotstr , chstr , appstr );
		}
	}
	else {
		if      ( mode == 5 ) {
			sprintf( Buffer , "%s/LotLog/%s/%s_%sLOTPOST%s" , Get_Prm_LOG_DIRECTORY() , logsubfolder , catstr , chstr , appstr );
		}
		else if ( mode == 4 ) {
			sprintf( Buffer , "%s/LotLog/%s/%s_%sLOTPRE%s" , Get_Prm_LOG_DIRECTORY() , logsubfolder , catstr , chstr , appstr );
		}
		else if ( mode == 3 ) {
			sprintf( Buffer , "%s/LotLog/%s/%s_%s%sPOST%s" , Get_Prm_LOG_DIRECTORY() , logsubfolder , catstr , slotstr , chstr , appstr );
		}
		else if ( mode == 2 ) {
			sprintf( Buffer , "%s/LotLog/%s/%s_%s%sPRE%s" , Get_Prm_LOG_DIRECTORY() , logsubfolder , catstr , slotstr , chstr , appstr );
		}
		else if ( mode == 1 ) {
			sprintf( Buffer , "%s/LotLog/%s/%s_%s%sALL%s" , Get_Prm_LOG_DIRECTORY() , logsubfolder , catstr , slotstr , chstr , appstr );
		}
		else {
			sprintf( Buffer , "%s/LotLog/%s/%s_%s%s%s" , Get_Prm_LOG_DIRECTORY() , logsubfolder , catstr , slotstr , chstr , appstr );
		}
	}
}

void Time_2_String( double time_data , char *buffer ) {
	long elapsed_time , hour , min , sec;
	elapsed_time = (long) time_data;
	hour = ( elapsed_time         ) / 3600;
	min  = ( elapsed_time % 3600  ) / 60;
	sec  = ( elapsed_time % 60    );
	sprintf( buffer , "%d:%d:%d" , hour , min , sec );
}

BOOL SAVE_FILE( char *srcfile , char *trgfile , BOOL option ) { // 2010.05.19
	FILE *fpt_o , *fpt_w;
//	char Buffer[256]; // 2014.10.10
	char Buffer[4096]; // 2014.10.10
	int res;
	int encflag;
//	if ( LOG_ENCRIPTION == 1 ) { // 2011.12.07
		//
		fpt_w = fopen( trgfile , "w" );
		//
		if ( fpt_w == NULL ) return FALSE;
		//
		if ( ( fpt_o = fopen( srcfile , "r" ) ) == NULL ) {
			fclose( fpt_w );
			return FALSE;
		}
		//
		encflag = 0; // 2011.12.07
		//
		do {
//			res = Read_Line_From_File4( fpt_o , Buffer , 255 ); // 2014.10.10
			res = Read_Line_From_File4( fpt_o , Buffer , 4095 ); // 2014.10.10
			//
//			Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
			//
			fprintf( fpt_w , "%s\n" , Buffer );
		}
		while ( res );
		fclose( fpt_o );
		//
		fclose( fpt_w );
		return TRUE;
//	} // 2011.12.07
//	else { // 2011.12.07
//		return CopyFile( srcfile , trgfile , option ); // 2011.12.07
//	} // 2011.12.07
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _UTIL_LV_SET_EXTEND_STYLE( HWND hWnd , BOOL userdraw , int extmode ) {
	DWORD dwStyle;
	//
	if ( userdraw ) {
		dwStyle = GetWindowLong( hWnd , GWL_STYLE );
		SetWindowLong( hWnd, GWL_STYLE, dwStyle | LVS_OWNERDRAWFIXED );
	}
	//
	switch( extmode ) {
	case 1 :
		ListView_SetExtendedListViewStyleEx( hWnd , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		break;
	case 2 :
		ListView_SetExtendedListViewStyleEx( hWnd , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP );
		break;
	}
	//
}


int ZOFFSET_5000_and_8000 = 0;

void ListView_Draw_User_with_WM_DRAWITEM_User8000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , a , b , s , zp , stroffset;
	int color_line;
	char buf[64];
	BOOL xdraw;
	LVCOLUMN lvcol; // 2016.04.08
	//
	color_line = COLOR_Blank;
	//
	i = 0;
	z = 0;
	//
	while( TRUE ) {
		//
//		j = ListView_GetColumnWidth( hwnd , z ); // 2016.04.08
//		if ( j > 2048 ) break; // 2016.04.08
		//
		lvcol.mask = LVCF_WIDTH; // 2016.04.08
		lvcol.cx = 0; // 2016.04.08
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break; // 2016.04.08
		j = lvcol.cx; // 2016.04.08
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		if ( ZOFFSET_5000_and_8000 == 0 ) {
			if ( z >= 1 )	zp = z + 1;
			else			zp = z;
		}
		else {
			zp = z;
		}
		//
		stroffset = 0;
		a = ( ( zp == 1 ) || ( zp == 3 ) ) ? 1 : 0;
		//
		if ( dis->itemState & ODS_SELECTED ) { // select
//			b = COLOR_DarkBlue;
			b = 1000+RGB(51,153,255);
			c = COLOR_White;
		}
		else {
			if ( buffer[0] == 0 ) {
				b = COLOR_White;
				c = COLOR_Black;
			}
			else {
				if      ( zp == 0 ) {
					b = COLOR_White;
					c = COLOR_Black;
				}
				else if ( zp == 1 ) { // mindex(job)
				if ( buffer[0] == '0' ) {
					b = 535;
					c = COLOR_Black;
					stroffset = 1;
				}
				else if ( buffer[0] == '1' ) {
					b = 547;
					c = COLOR_Black;
					stroffset = 1;
				}
				else {
					b = COLOR_White;
					c = COLOR_Black;
				}
				}
				else if ( zp == 2 ) { // slot
					//
					if ( buffer[0] == 'D' ) { // 2013.12.05
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						b = COLOR_PaleBlue;
						c = COLOR_Black;
					}
					//
				}
				else if ( zp == 3 ) { // wfrID
					//
					ListView_GetItemText( hwnd , dis->itemID , 1 , buf , 63 ); // 2013.12.05
					//
					if ( buf[0] == 'D' ) { // 2013.12.05
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						b = COLOR_PaleBlue;
						c = COLOR_Black;
					}
					//
				}
				else {
					if ( STRNCMP_L( buffer , "PM" , 2 ) ) {
						//
						s = atoi( buffer + 2 ) - 1;
						//
						if ( s < 0 ) s = 0;
						//
						b = s % 12;
						c = COLOR_White;
						/*
						switch ( s % 12 ) {
						default :	b = COLOR_PeaGreen;		c = COLOR_White;	break;
						case 1 :	b = COLOR_Slate;		c = COLOR_White;	break;
						case 2 :	b = COLOR_DarkGreen;	c = COLOR_White;	break;
						case 3 :	b = COLOR_BrightBlue;	c = COLOR_White;	break;
						case 4 :	b = COLOR_Magenta;		c = COLOR_Yellow;	break;
						case 5 :	b = COLOR_Lavender;		c = COLOR_White;	break;
						case 6 :	b = COLOR_LightGray;	c = COLOR_Black;	break;
						case 7 :	b = COLOR_MediumGray;	c = COLOR_Black;	break;
						case 8 :	b = COLOR_DarkGray;		c = COLOR_Yellow;	break;
						case 9 :	b = COLOR_BrightGreen;	c = COLOR_Black;	break;
						case 10 :	b = COLOR_Yellow;		c = COLOR_Black;	break;
						case 11 :	b = COLOR_Cyan;			c = COLOR_Black;	break;
						}
						*/
					}
					else {
						b = COLOR_White;
						c = COLOR_DarkGray;
					}
				}
			}
		}
		//
		xdraw = FALSE;
		//
		if ( xs >= 0 ) {
			if ( ( dis->rcItem.left + i ) > xs ) {
				xdraw = TRUE;
			}
			else if ( ( dis->rcItem.left + i + j ) < 0 ) {
				xdraw = TRUE;
			}
		}
		//
		if ( !xdraw ) KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer + stroffset , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , font );
//		KGUI_DRAW_StrBox25( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , j , dis->rcItem.bottom - dis->rcItem.top , ( a == 1 ) ? 5 : 0 , dis->rcItem.bottom - dis->rcItem.top - 1 , a , color_line , color_line , c , b , font );
		i = i + j;
		z++;
	}
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void ListView_Draw_User_with_WM_DRAWITEM_User7000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , a , b , s;
	int color_line;
	char oldstr[256];
	BOOL xdraw;
	LVCOLUMN lvcol; // 2016.04.08
	//
	color_line = COLOR_Blank;
	//
	i = 0;
	z = 0;
	//
	strcpy( oldstr , "" );
	//
	s = ( dis->itemState & ODS_SELECTED );// select
	//
	while( TRUE ) {
		//
//		j = ListView_GetColumnWidth( hwnd , z ); // 2016.04.08
//		if ( j > 2048 ) break; // 2016.04.08
		//
		lvcol.mask = LVCF_WIDTH; // 2016.04.08
		lvcol.cx = 0; // 2016.04.08
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break; // 2016.04.08
		j = lvcol.cx; // 2016.04.08
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		a = ( z == 0 ) ? 0 : 1;
		//
		if ( z == 0 ) {
			if ( s ) {
//				b = COLOR_DarkBlue;
				b = 1000+RGB(51,153,255);
				c = COLOR_White;
			}
			else {
				b = COLOR_White;
				c = COLOR_Black;
			}
		}
		else if ( z == 1 ) {
			if ( s ) {
//				b = COLOR_DarkBlue;
				b = 1000+RGB(51,153,255);
				c = COLOR_White;
			}
			else {
				b = COLOR_LightGray;
				c = COLOR_Black;
			}
		}
		else if ( z == 2 ) {
			if ( s ) {
//				b = COLOR_DarkBlue;
				b = 1000+RGB(51,153,255);
				c = COLOR_White;
			}
			else {
				b = COLOR_White;
				c = COLOR_Black;
			}
			strcpy( oldstr , buffer );
		}
		else {
			if ( STRCMP_L( oldstr , buffer ) ) {
				if ( s ) {
//				b = COLOR_DarkBlue;
				b = 1000+RGB(51,153,255);
					c = COLOR_White;
				}
				else {
					b = COLOR_White;
					c = COLOR_Black;
				}
			}
			else {
				if ( s ) {
//				b = COLOR_DarkBlue;
				b = 1000+RGB(51,153,255);
					c = ( Get_Prm_DISPLAY_MODE() > 1 ) ? 7 : COLOR_Magenta;
				}
				else {
					b = ( Get_Prm_DISPLAY_MODE() > 1 ) ? 7 : COLOR_Magenta;
					c = COLOR_Black;
				}
				strcpy( oldstr , buffer );
			}
		}
		//
		xdraw = FALSE;
		//
		if ( xs >= 0 ) {
			if ( ( dis->rcItem.left + i ) > xs ) {
				xdraw = TRUE;
			}
			else if ( ( dis->rcItem.left + i + j ) < 0 ) {
				xdraw = TRUE;
			}
		}
		//
		if ( !xdraw ) KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , font );
//		KGUI_DRAW_StrBox25( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , j , dis->rcItem.bottom - dis->rcItem.top , ( a == 1 ) ? 5 : 0 , dis->rcItem.bottom - dis->rcItem.top - 1 , a , color_line , color_line , c , b , font );
		i = i + j;
		z++;
	}
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void ListView_Draw_User_with_WM_DRAWITEM_User9000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int mode0 ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , a , b , s , err , mode , stchk;
	int color_line;
	double d;
	BOOL xdraw;
	LVCOLUMN lvcol; // 2016.04.08
	//
	//	9000 + 0 = Blank
	//	9000 + 1 = Title
	//	9000 + 2 = Data (Normal)
	//	9000 + 3 = Data (prcs)
	//	9000 + 4 = Data (?)
	//	9000 + 5 = Data (?)
	//	9000 + 6 = Data (?)
	//	9000 + 7 = Min,Max,Avr
	//	9000 + 8 = TMin,TMax,TAvr

	//	9000 + 1? = Data (PM Check % 10)
	//
	mode = mode0 % 10000; // 2015.05.27
	err = mode0 / 10000; // 2015.05.27
	//
	color_line = COLOR_Blank;
	//
	a = 1;
	//
	i = 0;
	z = 0;
	//
	while( TRUE ) {
		//
//		j = ListView_GetColumnWidth( hwnd , z ); // 2016.04.08
//		if ( j > 2048 ) break; // 2016.04.08
		//
		lvcol.mask = LVCF_WIDTH; // 2016.04.08
		lvcol.cx = 0; // 2016.04.08
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break; // 2016.04.08
		j = lvcol.cx; // 2016.04.08
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		if ( dis->itemState & ODS_SELECTED ) { // select
			if ( mode == 0 ) {
				b = COLOR_White;
				c = COLOR_White;
			}
			else {
				if ( err ) { // 2015.05.27
					b = COLOR_DarkRed;
					c = COLOR_White;
				}
				else {
//					b = COLOR_DarkBlue;
					b = 1000+RGB(51,153,255);
					c = COLOR_White;
				}
			}
		}
		else {
			switch( mode ) {
			case 0 : // blank
				b = COLOR_White;
				c = COLOR_Black;
				break;

			case 1 : // Title
				b = COLOR_Black;
				c = COLOR_White;
				break;

			case 2 : // Normal Data
				b = COLOR_White;
				c = COLOR_Black;
				break;

			case 3 : // Prcs Data
				//
				if ( ( ( z - 2 ) < 0 ) || ( ( z - 2 ) >= MAX_X_TABLE ) ) {
					b = COLOR_White;
					c = COLOR_Black;
				}
				else {
					//
					switch( STEP_CHECK_LOGDATA_GET_LV_STEP( z-2 , dis->itemID ) ) { // 2016.08.19
					case 0 : // step view (Stable)
						b = COLOR_White;
						//
						stchk = FALSE;
						break;

					case 1 : // step view (Step)
						//	COLOR_PaleGreen
						//	COLOR_PaleBlue
						b = COLOR_PaleGreen;
						//
						stchk = TRUE;
						break;

					case 2 : // step view (Over)
						b = 7;
						stchk = TRUE;
						//
						break;

					default :
						b = COLOR_White;
						stchk = TRUE;
						break;
					}
					//
					if ( stchk ) {
						if ( !STR_9003_MINMAX_USE[z-2] ) {
							c = COLOR_Black;
						}
						else {
							d = atof( buffer );
							if      ( d == STR_9003_MIN[z-2] ) {
								if ( d == STR_9003_MAX[z-2] ) {
									c = COLOR_DarkGray;
								}
								else {
									c = COLOR_Magenta;
								}
							}
							else if ( d == STR_9003_MAX[z-2] ) {
								c = COLOR_RightRed;
							}
							else {
								c = COLOR_Black;
							}
						}
					}
				}
				break;

			case 4 : // ?
			case 5 : // ?
			case 6 : // ?
				b = COLOR_White;
				c = COLOR_Black;
				break;

			case 7 : // min,max,avr
				if ( z == 0 ) {
					b = COLOR_White;
					c = COLOR_Black;
				}
				else {
					b = COLOR_OffWhite;
					c = COLOR_DarkGray;
				}
				break;

			case 8 : // min,max,avr
				b = COLOR_LightGray;
				c = COLOR_Black;
				break;

			case 9 : // ?
				b = COLOR_White;
				c = COLOR_Black;
				break;

			default :
				if ( z != ( mode - 10 ) ) {
					if ( err ) { // 2015.05.27
						b = COLOR_White;
						c = COLOR_DarkRed;
					}
					else {
						b = COLOR_White;
						c = COLOR_Black;
					}
				}
				else {
					if ( STRNCMP_L( buffer , "PM" , 2 ) ) {
						//
						s = atoi( buffer + 2 ) - 1;
						//
						if ( s < 0 ) s = 0;
						//
						b = s % 12;
						c = COLOR_White;
						/*
						switch ( s % 12 ) {
						default :	b = COLOR_PeaGreen;		c = COLOR_White;	break;
						case 1 :	b = COLOR_Slate;		c = COLOR_White;	break;
						case 2 :	b = COLOR_DarkGreen;	c = COLOR_White;	break;
						case 3 :	b = COLOR_BrightBlue;	c = COLOR_White;	break;
						case 4 :	b = COLOR_Magenta;		c = COLOR_Yellow;	break;
						case 5 :	b = COLOR_Lavender;		c = COLOR_White;	break;
						case 6 :	b = COLOR_LightGray;	c = COLOR_Black;	break;
						case 7 :	b = COLOR_MediumGray;	c = COLOR_Black;	break;
						case 8 :	b = COLOR_DarkGray;		c = COLOR_Yellow;	break;
						case 9 :	b = COLOR_BrightGreen;	c = COLOR_Black;	break;
						case 10 :	b = COLOR_Yellow;		c = COLOR_Black;	break;
						case 11 :	b = COLOR_Cyan;			c = COLOR_Black;	break;
						}
						*/
					}
					else {
						b = COLOR_White;
						c = COLOR_DarkGray;
					}
				}
				break;
			}
		}
		//
		xdraw = FALSE;
		//
		if ( xs >= 0 ) {
			if ( ( dis->rcItem.left + i ) > xs ) {
				xdraw = TRUE;
			}
			else if ( ( dis->rcItem.left + i + j ) < 0 ) {
				xdraw = TRUE;
			}
		}
		//
		if ( !xdraw ) KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , font );
//		KGUI_DRAW_StrBox25( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , j , dis->rcItem.bottom - dis->rcItem.top , ( a == 1 ) ? 5 : 0 , dis->rcItem.bottom - dis->rcItem.top - 1 , a , color_line , color_line , c , b , font );
		i = i + j;
		z++;
	}
}
//
void ListView_Draw_User_with_WM_DRAWITEM_User6000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int mode ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , a , b;
	int color_line;
	BOOL xdraw;
	LVCOLUMN lvcol; // 2016.04.08
	//
	color_line = COLOR_Blank;
	//
	a = 1;
	//
	if ( dis->itemState & ODS_SELECTED ) { // select
//		b = COLOR_DarkBlue;
		b = 1000+RGB(51,153,255);
		c = COLOR_White;
	}
	else {
		b = mode;
		c = COLOR_White;
	}
	//
	i = 0;
	z = 0;
	//
	while( TRUE ) {
		//
//		j = ListView_GetColumnWidth( hwnd , z ); // 2016.04.08
//		if ( j > 2048 ) break; // 2016.04.08
		//
		lvcol.mask = LVCF_WIDTH; // 2016.04.08
		lvcol.cx = 0; // 2016.04.08
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break; // 2016.04.08
		j = lvcol.cx; // 2016.04.08
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		xdraw = FALSE;
		//
		if ( xs >= 0 ) {
			if ( ( dis->rcItem.left + i ) > xs ) {
				xdraw = TRUE;
			}
			else if ( ( dis->rcItem.left + i + j ) < 0 ) {
				xdraw = TRUE;
			}
		}
		//
		if ( !xdraw ) KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , font );
		//
		i = i + j;
		z++;
	}
}

//

int Get_Move_Mode_Color( int mode , int *b , int *c ) {
	//
	switch( mode ) {
	case TCH_PM_PLACE :
	case TCH_BM_PLACE :
		//
		*b = COLOR_Black;
		*c = COLOR_White;
		//
		return 1;
		//
		break;
	case TCH_PM_PICK :
	case TCH_BM_PICK :
		//
		*b = 1000 + RGB( 128 , 128 , 128 );
		*c = COLOR_White;
		//
		return 1;
		//
		break;

	case TCH_TM_SWAP :
		//
		*b = 1000 + RGB( 192 , 192 , 192 );
		*c = COLOR_White;
		//
		return 1;
		//
		break;

	case TCH_TM_MOVE :
		//
		*b = 1000 + RGB( 64 , 64 , 64 );
		*c = COLOR_White;
		//
		return 2;
		//
		break;

	case TCH_FM_MOVE :
		//
		*b = 1000 + RGB( 64 , 64 , 0 );
		*c = COLOR_White;
		//
		return 2;
		//
		break;

	case TCH_FM_PICK :
		//
		*b = 1000 + RGB( 192 , 192 , 0 );
		*c = COLOR_White;
		//
		return 1;
		//
		break;

	case TCH_FM_PLACE :
		//
		*b = 1000 + RGB( 128 , 128 , 0 );
		*c = COLOR_White;
		//
		return 1;
		//
		break;

	case TCH_BM_FMSIDE :
		//
		*b = 1000+RGB(25,80,255);
		*c = COLOR_White;
		break;

	case TCH_BM_TMSIDE :
		//
		*b = COLOR_PaleBlue;
		*c = COLOR_White;
		break;

	case TCH_COOL :
		//
		*b = 1000 + RGB( 150 , 210 , 230 );
		*c = COLOR_White;
		break;

	case TCH_ALIGN :
		//
		*b = 1000 + RGB( 240 , 190 , 200 );
		*c = COLOR_White;
		break;

	case TCH_REJECT :
		//
		*b = 1000 + RGB( 255 , 0 , 0 );
		*c = COLOR_White;
		//
		return 1;
		//
		break;


	default :
		return -1;
		break;
	}
	//
	return 0;
	//
}

//
void ListView_Draw_User_with_WM_DRAWITEM_User5000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int index , int wfrid ) { // CHART Wafer / Wafer(idle)
	//
	HBITMAP hBitmap;
	HDC hMemDC;
	BOOL xdraw;
	LVCOLUMN lvcol; // 2016.04.08
	//
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , a , b , s , k , l , s2 , pos , uu , u , zp , stroffset , move;
	int color_line;
	double rate;
	char buf[64];
	char BufX[256];
	//
	color_line = COLOR_Blank;
	//
	i = 0;
	z = 0;
	//
	while( TRUE ) {
		//
		if ( ZOFFSET_5000_and_8000 == 0 ) {
			if ( z > 3 ) break;
		}
		else {
			if ( z > 4 ) break;
		}
		//
//		j = ListView_GetColumnWidth( hwnd , z ); // 2016.04.08
		//
//		if ( j > 2048 ) break;
		//
		lvcol.mask = LVCF_WIDTH; // 2016.04.08
		lvcol.cx = 0; // 2016.04.08
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break; // 2016.04.08
		j = lvcol.cx; // 2016.04.08
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		if ( ZOFFSET_5000_and_8000 == 0 ) {
			if ( z >= 1 )	zp = z + 1;
			else			zp = z;
		}
		else {
			zp = z;
		}
		//
		stroffset = 0;
		a = ( ( zp == 1 ) || ( zp == 3 ) ) ? 1 : 0;
		//
		if ( zp < 4 ) {
			if ( dis->itemState & ODS_SELECTED ) { // select
				b = 1000+RGB(51,153,255);
				c = COLOR_White;
			}
			else {
				if      ( zp == 0 ) {
					if ( index == 2 ) {
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						b = COLOR_White;
						c = COLOR_Black;
					}
				}
				else if ( zp == 1 ) { // mindex(job)
					if ( buffer[0] == '0' ) {
						b = 535;
						c = COLOR_Black;
						stroffset = 1;
					}
					else if ( buffer[0] == '1' ) {
						b = 547;
						c = COLOR_Black;
						stroffset = 1;
					}
					else {
						b = COLOR_White;
						c = COLOR_Black;
					}
				}
				else if ( zp == 2 ) { // slot
					if ( index == 2 ) {
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						//
						if ( buffer[0] == 'D' ) { // 2013.12.05
							b = COLOR_White;
							c = COLOR_Black;
						}
						else {
							b = COLOR_PaleBlue;
							c = COLOR_Black;
						}
						//
					}
				}
				else if ( zp == 3 ) { // WfrID
					if ( index == 2 ) {
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						//
						ListView_GetItemText( hwnd , dis->itemID , 1 , buf , 63 ); // 2013.12.05
						//
						if ( buf[0] == 'D' ) { // 2013.12.05
							b = COLOR_White;
							c = COLOR_Black;
						}
						else {
							b = COLOR_PaleBlue;
							c = COLOR_Black;
						}
						//
					}
				}
			}
		}
		else { // draw chart
			//
			hBitmap = CreateCompatibleBitmap( dis->hDC , j , dis->rcItem.bottom - dis->rcItem.top );
			hMemDC = CreateCompatibleDC( dis->hDC );
			SelectObject( hMemDC , hBitmap );
			//
			rate = (double) wfrid / 100.0;
			//
			if ( index == 2 ) {
				//--------------------------------------------------------------------------------------------------------------------------------------------------------------
				b = COLOR_White;
				KGUI_DRAW_StrBoxLV( hMemDC , 0 , 0 , "" , j , dis->rcItem.bottom - dis->rcItem.top , 0 , a , color_line , color_line , b , b , font );
				//--------------------------------------------------------------------------------------------------------------------------------------------------------------
				if ( rate != 0 ) {
					//
					k = 0;
					//
					while (TRUE) {
						//
						Time_2_String( (double) k / rate , BufX );
						//
						KGUI_DRAW_StrBoxLV( hMemDC , k     , 0 , ""   , k + 1  , dis->rcItem.bottom - dis->rcItem.top , 0 , 1 , color_line , color_line , COLOR_Black , COLOR_Black , font );
						KGUI_DRAW_StrBoxLV( hMemDC , k + 2 , 0 , BufX , k + 60 , dis->rcItem.bottom - dis->rcItem.top , 0 , 1 , color_line , color_line , COLOR_Black , COLOR_White , font );
						//
						k = k + 60;
						//
						if ( k >= j ) break;
					}
				}
			}
			else {
				//--------------------------------------------------------------------------------------------------------------------------------------------------------------

				if ( dis->itemState & ODS_SELECTED ) { // select
					b = 1000+RGB(51,153,255);
				}
				else {
					b = COLOR_White;
				}
				//
				KGUI_DRAW_StrBoxLV( hMemDC , 0 , 0 , "" , j , dis->rcItem.bottom - dis->rcItem.top , 0 , a , color_line , color_line , b , b , font );

				//--------------------------------------------------------------------------------------------------------------------------------------------------------------
				//
				pos = dis->itemID - 1;
				//
				for ( k = 0 ; k < 4 ; k++ ) {
					//
					strcpy( BufX , "" );
					//
					for ( l = 0 ; l < CURRENT_WAFER_MODULE_DEPTH[pos] ; l++ ) {
						//
						if ( CURRENT_WAFER_MODULE_START[pos][l] <= 0 ) continue;
						//
						uu = 0;
						//
						move = 0;
						//
						if      ( k == 0 ) { // IN
							if ( CURRENT_WAFER_MODULE_MODE[pos][l] != TCH_CM_IN ) continue;
							//
							b = COLOR_LightGray;
							c = COLOR_LightGray;
							uu = 2;
							//
							s  = 0;
							s2 = (int) ( ( CURRENT_WAFER_MODULE_START[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
							//
						}
						else if ( k == 2 ) {
							//
							move = Get_Move_Mode_Color( CURRENT_WAFER_MODULE_MODE[pos][l] , &b , &c );
							//
							if ( move == -1 ) continue;
							//
							s  = (int) ( ( CURRENT_WAFER_MODULE_START[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
							//
							if ( CURRENT_WAFER_MODULE_END[pos][l] <= 0 ) {
								s2 = (int) ( ( CURRENT_WAFER_Get_Max_Sec(pos) - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
							}
							else {
								s2 = (int) ( ( CURRENT_WAFER_MODULE_END[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
							}
							//
						}
						else if ( k == 1 ) {
							//
							switch( CURRENT_WAFER_MODULE_MODE[pos][l] ) {
							case TCH_PROCESS :
							case TCH_PROCESS_X :
								//
								strcpy( BufX , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[pos][l] ) );
								//
								if ( STRNCMP_L( BufX , "PM" , 2 ) ) {
									//
									if ( index == 0 ) {
										s = atoi( BufX + 2 ) - 1;
										//
										if ( s < 0 ) s = 0;
										//
										b = s % 12;
									}
									else {
										b = COLOR_Black;
									}
									c = COLOR_White;
									//
									s  = (int) ( ( CURRENT_WAFER_MODULE_START[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
									//
									if ( CURRENT_WAFER_MODULE_END[pos][l] <= 0 ) {
										s2 = (int) ( ( CURRENT_WAFER_Get_Max_Sec(pos) - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
										b = COLOR_RightRed;
									}
									else {
										s2 = (int) ( ( CURRENT_WAFER_MODULE_END[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
									}
									//
									if ( (int) ( strlen(BufX) * 8 ) > ( s2 - s ) ) {
										if ( (int) ( ( strlen(BufX) - 2 ) * 8 ) > ( s2 - s ) ) {
											strcpy( BufX , "" );
										}
										else {
											strcpy( BufX , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[pos][l] ) + 2 );
										}
									}
									//
								}
								else {
									//
									if ( index == 0 ) {
										b = COLOR_Cyan;
									}
									else {
										b = COLOR_Black;
									}
									c = COLOR_White;
									//
									s  = (int) ( ( CURRENT_WAFER_MODULE_START[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
									//
									if ( CURRENT_WAFER_MODULE_END[pos][l] <= 0 ) {
										s2 = (int) ( ( CURRENT_WAFER_Get_Max_Sec(pos) - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
										b = COLOR_RightRed;
									}
									else {
										s2 = (int) ( ( CURRENT_WAFER_MODULE_END[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
									}
									//
									if ( (int) ( strlen(BufX) * 8 ) > ( s2 - s ) ) strcpy( BufX , "" );
									//
								}
								//
								break;
							default :
								continue;
								break;
							}
							//
						}
						else { // OUT
							//
							if ( CURRENT_WAFER_MODULE_END[pos][l] <= 0 ) continue;
							//
							switch( CURRENT_WAFER_MODULE_MODE[pos][l] ) {
							case TCH_CM_IN :
								b = COLOR_LightGray;
								c = COLOR_LightGray;
								uu = 1;
								//
								s  = (int) ( ( CURRENT_WAFER_MODULE_END[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
								s2 = (int) ( ( CURRENT_WAFER_Get_Max_Sec(pos) - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
								//
								break;
							case TCH_CM_OUT :
								b = COLOR_Black;
								c = COLOR_White;
								//
								s  = (int) ( ( CURRENT_WAFER_MODULE_END[pos][l] - CURRENT_WAFER_Get_Min_Sec(pos) ) * rate );
								s2 = s + 1;
								//
								break;
							default :
								continue;
								break;
							}
							//
						}
						//
						if ( s > j ) continue;
						if ( s2 > j ) s2 = j;
						if ( s == s2 ) s2 = s+1;
						//
						if ( uu != 0 ) {
							if ( dis->itemState & ODS_SELECTED ) { // select
								KGUI_DRAW_StrBoxLV( hMemDC , s , 0 , buffer , s2 , dis->rcItem.bottom - dis->rcItem.top , 0 , uu , color_line , color_line , COLOR_White , 1000+RGB(51,153,255) , font );
							}
							else {
								for ( u = 0 ; u <= ( dis->rcItem.bottom - dis->rcItem.top ) ; u++ ) {
									KGUI_DRAW_Line( hMemDC , s , u , s2 , u , b , FALSE );
									u += 1;
								}
							}
						}
						else {
							//
							KGUI_DRAW_StrBoxLV( hMemDC , s , ( move == 0 ) ? 0 : ( ( move == 1 ) ? 5 : 10 )  , BufX , s2 , dis->rcItem.bottom - dis->rcItem.top , 0 , a , color_line , color_line , c , b , font );
						}
						//
					}
					//
				}
			}
			//
			BitBlt( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , j , dis->rcItem.bottom - dis->rcItem.top , hMemDC , 0 , 0 , SRCCOPY );
			DeleteDC( hMemDC );
			DeleteObject( hBitmap );
			//
			break;
			//
		}
		//
		xdraw = FALSE;
		//
		if ( xs >= 0 ) {
			if ( ( dis->rcItem.left + i ) > xs ) {
				xdraw = TRUE;
			}
			else if ( ( dis->rcItem.left + i + j ) < 0 ) {
				xdraw = TRUE;
			}
		}
		//
		if ( !xdraw ) KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer + stroffset , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , font );
		//
		i = i + j;
		z++;
	}
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void ListView_Draw_User_with_WM_DRAWITEM_User4000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int paramdata , int wfrid ) { // CHART Module / Module(idle)
	//
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , a , b , s , k , l , s2 , ix1 , ix2 , pc , pos , mode;
	int color_line;
	double rate;
	BOOL xdraw;
	LVCOLUMN lvcol; // 2016.04.08
	//
	color_line = COLOR_Blank;
	//
	mode = paramdata % 10;
	pos  = paramdata / 10;
	//
	i = 0;
	z = 0;
	//
	while( TRUE ) {
		//
		if ( z > 3 ) break;
		//
		lvcol.mask = LVCF_WIDTH; // 2016.04.08
		lvcol.cx = 0; // 2016.04.08
		//
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break; // 2016.04.08
		//
		j = lvcol.cx; // 2016.04.08
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		a = ( ( z == 1 ) || ( z == 2 ) ) ? 1 : 0;
		//
		if ( z < 3 ) { 
			if ( dis->itemState & ODS_SELECTED ) { // select
				b = 1000+RGB(51,153,255);
				c = COLOR_White;
			}
			else {
				if      ( z == 0 ) {
					if ( mode == 2 ) {
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						b = COLOR_White;
						c = COLOR_Black;
					}
				}
				else if ( z == 1 ) {
					if ( mode == 2 ) {
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						b = COLOR_PaleBlue;
						c = COLOR_Black;
					}
				}
				else if ( z == 2 ) {
					if ( mode == 2 ) {
						b = COLOR_White;
						c = COLOR_Black;
					}
					else {
						b = COLOR_PaleBlue;
						c = COLOR_Black;
					}
				}
			}
		}
		else { // chart
			//
			rate = (double) wfrid / 100.0;
			//
			//--------------------------------------------------------------------------------------------------------------------------------------------------------------
			if ( dis->itemState & ODS_SELECTED ) { // select
				b = 1000+RGB(51,153,255);
			}
			else {
				b = COLOR_White;
			}
			//
			KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , "" , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , b , b , font );
			//--------------------------------------------------------------------------------------------------------------------------------------------------------------
			if ( mode == 2 ) {
				if ( rate != 0 ) {
					//
					k = 0;
					//
					while (TRUE) {
						//
						Time_2_String( (double) k / rate , buffer );
						//
						KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i + k     , dis->rcItem.top , ""     , dis->rcItem.left + i + k + 1  , dis->rcItem.bottom , 0 , 1 , color_line , color_line , COLOR_Black , COLOR_Black , font );
						KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i + k + 2 , dis->rcItem.top , buffer , dis->rcItem.left + i + k + 60 , dis->rcItem.bottom , 0 , 1 , color_line , color_line , COLOR_Black , COLOR_White , font );
						//
						k = k + 60;
						//
						if ( k >= j ) break;
					}
				}
			}
			else {
				//
				strcpy( buffer , STR_MEM_GET_STR( CURRENT_MODULE_VIEW_NAME[pos] ) );
				//
				if ( mode == 0 ) {
					if ( STRNCMP_L( buffer , "PM" , 2 ) ) {
						s = atoi( buffer + 2 ) - 1;
						if ( s < 0 ) s = 0;
						pc = s % 12;
					}
					else {
						pc = COLOR_Cyan;
					}
				}
				else {
					pc = COLOR_Black;
				}
				//
				strcpy( buffer , "" );
				//
				for ( k = 0 ; k < 2 ; k++ ) {
					//
					for ( l = 0 ; l < CURRENT_MODULE_VIEW_DEPTH[pos] ; l++ ) {
						//
						ix1 = CURRENT_MODULE_VIEW_1[pos][l];
						ix2 = CURRENT_MODULE_VIEW_2[pos][l];
						//
						if ( CURRENT_WAFER_MODULE_START[ix1][ix2] <= 0 ) continue;
						if ( CURRENT_WAFER_MODULE_END[ix1][ix2] <= 0 ) continue;
						//
						if      ( k == 0 ) { // IN
							//
							switch( CURRENT_WAFER_MODULE_MODE[ix1][ix2] ) {
							case TCH_PROCESS :
							case TCH_PROCESS_X :
								//
								if ( CURRENT_WAFER_ID[ix1] >= 100 ) {
									sprintf( buffer , "D%d" , CURRENT_WAFER_ID[ix1] % 100 );
								}
								else {
									sprintf( buffer , "%d" , CURRENT_WAFER_ID[ix1] );
								}
								//
								s  = (int) ( ( CURRENT_WAFER_MODULE_START[ix1][ix2] - CURRENT_WAFER_Get_Min_Sec(ix1) ) * rate );
								s2 = (int) ( ( CURRENT_WAFER_MODULE_END[ix1][ix2] - CURRENT_WAFER_Get_Min_Sec(ix1) ) * rate );
								//
								if ( (int) ( strlen(buffer) * 8 ) > ( s2 - s ) ) strcpy( buffer , "" );
								//
								b = pc;
								c = COLOR_White;
								//
								break;
							default :
								continue;
								break;
							}
							//
						}
						else if ( k == 1 ) {
							//
							if ( Get_Move_Mode_Color( CURRENT_WAFER_MODULE_MODE[ix1][ix2] , &b , &c ) == -1 ) continue;
							//
							if ( CURRENT_WAFER_ID[ix1] >= 100 ) {
								sprintf( buffer , "D%d" , CURRENT_WAFER_ID[ix1] % 100 );
							}
							else {
								sprintf( buffer , "%d" , CURRENT_WAFER_ID[ix1] );
							}
							//
							s  = (int) ( ( CURRENT_WAFER_MODULE_START[ix1][ix2] - CURRENT_WAFER_Get_Min_Sec(ix1) ) * rate );
							s2 = (int) ( ( CURRENT_WAFER_MODULE_END[ix1][ix2] - CURRENT_WAFER_Get_Min_Sec(ix1) ) * rate );
							//
							if ( (int) ( strlen(buffer) * 8 ) > ( s2 - s ) ) strcpy( buffer , "" );
							//
						}
						//
						if ( s > j ) continue;
						if ( s2 > j ) s2 = j;
						if ( s == s2 ) s2 = s+1;
						//
						KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i + s , dis->rcItem.top , buffer , dis->rcItem.left + i + s2 , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , font );
						//
					}
				}
			}
			//
			break;
			//
		}
		//
		xdraw = FALSE;
		//
		if ( xs >= 0 ) {
			if ( ( dis->rcItem.left + i ) > xs ) {
				xdraw = TRUE;
			}
			else if ( ( dis->rcItem.left + i + j ) < 0 ) {
				xdraw = TRUE;
			}
		}
		//
		if ( !xdraw ) KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , font );
		//
		i = i + j;
		z++;
	}
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

// \(CColor:BColor:Align:fsize:font)
// \<Filename>

void Text_Form_Remap( BOOL sel , char *str , int cnt , int *align , int *bcolor , int* ccolor , int *fontsize , char *fontstr ) {
	char Buffer[64];
	int i , c , x , m , mc;
	//
	i = 0;
	c = 0;
	m = 0;
	//
	while ( TRUE ) {
		//
		if ( str[i] == 0 ) break;
		//
		if ( m == 0 ) {
			//
			if ( ( str[i] == '\\' ) && ( str[i+1] == '(' ) ) {
				//
				i++;
				//
				m = 1;
				mc = 0;
				//
			}
			else {
				//
				if ( c != i ) str[c] = str[i];
				//
				c++;
				//
			}
		}
		else {
			if ( ( str[i] == ')' ) || ( str[i] == ':' ) ) {
				//
				if ( mc > 0 ) {
					//
					Buffer[mc] = 0;
					//
					switch( m ) {
					case 1 : //ccolor
						switch( KGUI_FT_Get_Color_Data( Buffer , &x ) ) {
						case 1 :
						case 2 :
							if ( !sel ) {
								*ccolor = x;
							}
							break;
						}
						break;
					case 2 : //bcolor
						switch( KGUI_FT_Get_Color_Data( Buffer , &x ) ) {
						case 1 :
						case 2 :
							if ( !sel ) {
								*bcolor = x;
							}
							break;
						}
						break;
					case 3 : //align
						//
						*align = 0;
						//
						for ( x = 0 ; x < mc ; x++ ) {
							//===============================================================
							if ( Buffer[i] == 'M' ) {
								if ( ( ( *align / 10 ) / 2 ) == 1 ) {
									*align = ( *align % 10 ) + 30;
								}
								else {
									*align = ( *align % 10 ) + 10;
								}
							}
							else if ( Buffer[i] == 'E' ) {
								if ( ( ( *align / 10 ) % 2 ) == 1 ) {
									*align = ( *align % 10 ) + 30;
								}
								else {
									*align = ( *align % 10 ) + 20;
								}
							}
							//===============================================================
							else if ( Buffer[i] == 'L' ) {
								*align = ( ( *align / 10 ) * 10 ) + ( ( ( *align % 10 ) / 3 ) * 3 ) + 1;
							}
							else if ( Buffer[i] == 'R' ) {
								*align = ( ( *align / 10 ) * 10 ) + ( ( ( *align % 10 ) / 3 ) * 3 ) + 2;
							}
							else if ( Buffer[i] == 'C' ) {
								*align = ( ( *align / 10 ) * 10 ) + ( ( ( *align % 10 ) / 3 ) * 3 ) + 0;
							}
							else if ( Buffer[i] == 'T' ) {
								*align = ( ( *align / 10 ) * 10 ) + ( ( *align % 10 ) % 3 ) + ( 3 * 1 );
							}
							else if ( Buffer[i] == 'B' ) {
								*align = ( ( *align / 10 ) * 10 ) + ( ( *align % 10 ) % 3 ) + ( 3 * 2 );
							}
							else if ( Buffer[i] == 'V' ) {
								*align = ( ( *align / 10 ) * 10 ) + ( ( *align % 10 ) % 3 ) + ( 3 * 0 );
							}
							//===============================================================
						}
						break;
					case 4 : //fsize
						*fontsize = atoi( Buffer );
						break;
					case 5 : //fontstr
						strcpy( fontstr , Buffer );
						break;
					}
				}
				//
				if ( str[i] == ')' ) {
					m = 0;
				}
				else {
					m++;
					mc = 0;
				}
			}
			else {
				if ( mc < 63 ) {
					Buffer[mc] = str[i]; mc++;
				}
			}
		}
		//
		i++;
		//
	}
	//
	str[c] = 0;
	//
}


void ListView_Draw_User_with_WM_DRAWITEM( HWND hwnd , LPARAM lParam , int xs , BOOL error , int align , char *font , char *buffer , int buffercount , int colorset , int lineset , int spcolor1 , int spcolor2 , int spcolor3 , int spcolor4 ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , a;
	int color_text , color_back , color_line;
	BOOL xdraw;
	LVCOLUMN lvcol; // 2016.04.08
	//
	int b , f; // 2018.02.09
	BOOL sel; // 2018.02.09
	char fontstr[128]; // 2018.02.09
	//
	if ( dis->itemState & ODS_SELECTED ) { // select
		//
		sel = TRUE; // 2018.02.09
		//
		switch( colorset ) {
		case 1 :
			if ( error ) {
				color_text = COLOR_White;		color_back = COLOR_RightRed;
			}
			else {
				color_text = COLOR_White;		color_back = COLOR_BrightBlue;
			}
			break;
		case 2 :
			if ( error ) {
				color_text = COLOR_White;		color_back = COLOR_DarkRed;
			}
			else {
//				color_text = COLOR_White;		color_back = COLOR_DarkBlue;
				color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
			}
			break;
		case 3 :
			if ( error ) {
				color_text = COLOR_White;		color_back = COLOR_DarkRed; // ?
			}
			else {
				color_text = COLOR_Yellow;		color_back = COLOR_Magenta;
			}
			break;
		case 4 :
			if ( error ) {
				color_text = COLOR_White;		color_back = COLOR_DarkRed; // ?
			}
			else {
//				color_text = COLOR_White;		color_back = COLOR_DarkBlue;
				color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
			}
			break;
		case 8 :
			if ( error ) {
				color_text = COLOR_White;		color_back = COLOR_RightRed;
			}
			else {
//				color_text = COLOR_White;		color_back = COLOR_DarkBlue;
				color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
			}
			break;
		default :
			if ( error ) {
				color_text = COLOR_White;		color_back = COLOR_DarkRed;
			}
			else {
//				color_text = COLOR_White;		color_back = COLOR_DarkBlue;
				color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
			}
			break;
		}
		color_line = COLOR_Blank;
	}
	else {
		//
		sel = FALSE; // 2018.02.09
		//
		switch( colorset ) {
		case 1 :
			if ( error ) {
				color_text = COLOR_RightRed;	color_back = COLOR_White;
			}
			else {
				color_text = COLOR_Black;		color_back = COLOR_White;
			}
			break;
		case 2 :
			if ( error ) {
				color_text = COLOR_DarkRed;		color_back = COLOR_White;
			}
			else {
				color_text = COLOR_BrightBlue;	color_back = COLOR_White;
			}
			break;
		case 3 : // 2011.06.20
			if ( error ) {
				color_text = COLOR_DarkRed;		color_back = COLOR_White; // ?
			}
			else {
				color_text = COLOR_Magenta;		color_back = COLOR_White;
			}
			break;
		case 4 : // 2011.06.20
			if ( error ) {
				color_text = COLOR_DarkRed;		color_back = COLOR_White; // ?
			}
			else {
				color_text = COLOR_Black;		color_back = COLOR_PaleGreen;
			}
			break;
		case 5 : // 2011.07.15
			if ( error ) {
				color_text = COLOR_DarkRed;		color_back = COLOR_White; // ?
			}
			else {
				color_text = COLOR_Black;		color_back = COLOR_PaleBlue;
			}
			break;
		case 6 : // 2011.07.15
			if ( error ) {
				color_text = COLOR_DarkRed;		color_back = COLOR_White; // ?
			}
			else {
				color_text = COLOR_Black;		color_back = COLOR_LightGray;
			}
			break;
		case 7 : // 2011.07.15
			if ( error ) {
				color_text = COLOR_DarkRed;		color_back = COLOR_White; // ?
			}
			else {
				color_text = COLOR_White;		color_back = COLOR_Black;
			}
			break;
		case 8 : // 2011.07.26
			if ( error ) {
				color_text = COLOR_RightRed;	color_back = COLOR_White;
			}
			else {
				color_text = COLOR_White;		color_back = COLOR_Black;
			}
			break;
			//
		case 9 :
			if ( error ) { // 2015.05.27
				ListView_Draw_User_with_WM_DRAWITEM_User9000( hwnd , lParam , xs , font , buffer , buffercount , 10000 + 11 ); // 2015.05.27
			}
			else {
				ListView_Draw_User_with_WM_DRAWITEM_User9000( hwnd , lParam , xs , font , buffer , buffercount , 11 ); // 2015.05.27
			}
			return;

		default :
			if ( error ) {
				color_text = COLOR_DarkRed;		color_back = COLOR_White;
			}
			else {
				color_text = COLOR_Black;		color_back = COLOR_White;
			}
			break;
		}
		if ( lineset ) {
			color_line = COLOR_LightGray;
		}
		else {
			color_line = COLOR_Blank;
		}
	}
	//
	i = 0;
	z = 0;
	while( TRUE ) {
		//
//		j = ListView_GetColumnWidth( hwnd , z ); // 2016.04.08
//		if ( j > 2048 ) break; // 2016.04.08
		//
		lvcol.mask = LVCF_WIDTH; // 2016.04.08
		lvcol.cx = 0; // 2016.04.08
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break; // 2016.04.08
		j = lvcol.cx; // 2016.04.08
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		if ( color_text == COLOR_Black ) {
			if      ( ( spcolor1 >= 0 ) && ( ( spcolor1 % 100 ) == z ) ) {
				a = ( spcolor1 / 100000 );
				c = ( ( spcolor1 % 100000 ) / 100 );
			}
			else if ( ( spcolor2 >= 0 ) && ( ( spcolor2 % 100 ) == z ) ) {
				a = ( spcolor2 / 100000 );
				c = ( ( spcolor2 % 100000 ) / 100 );
			}
			else if ( ( spcolor3 >= 0 ) && ( ( spcolor3 % 100 ) == z ) ) {
				a = ( spcolor3 / 100000 );
				c = ( ( spcolor3 % 100000 ) / 100 );
			}
			else if ( ( spcolor4 >= 0 ) && ( ( spcolor4 % 100 ) == z ) ) {
				a = ( spcolor4 / 100000 );
				c = ( ( spcolor4 % 100000 ) / 100 );
			}
			else {
				a = align;
				c = color_text;
			}
		}
		else {
			if      ( ( spcolor1 >= 0 ) && ( ( spcolor1 % 100 ) == z ) ) {
				a = ( spcolor1 / 100000 );
			}
			else if ( ( spcolor2 >= 0 ) && ( ( spcolor2 % 100 ) == z ) ) {
				a = ( spcolor2 / 100000 );
			}
			else if ( ( spcolor3 >= 0 ) && ( ( spcolor3 % 100 ) == z ) ) {
				a = ( spcolor3 / 100000 );
			}
			else if ( ( spcolor4 >= 0 ) && ( ( spcolor4 % 100 ) == z ) ) {
				a = ( spcolor4 / 100000 );
			}
			else {
				a = align;
			}
			c = color_text;
		}
		//
		xdraw = FALSE;
		//
		if ( xs >= 0 ) {
			if ( ( dis->rcItem.left + i ) > xs ) {
				xdraw = TRUE;
			}
			else if ( ( dis->rcItem.left + i + j ) < 0 ) {
				xdraw = TRUE;
			}
		}
		//
		// 2018.02.09
		//
		if ( !xdraw ) {
			if ( ( buffer[0] == '\\' ) && ( buffer[1] == '(' ) ) {
				//
				f = 0;
				b = color_back;
				//
				strcpy( fontstr, font );
				//
				Text_Form_Remap( sel , buffer , buffercount , &a , &b , &c , &f , fontstr );
				//
				KGUI_DRAW_StrBoxLV2( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , a , color_line , 0 /*lb*/ , c , b , fontstr , f ); // lbline 0:lb 1:l 2:b
				//
			}
			else {
				KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , color_back , font );
			}
		}
		//
		//
//		if ( !xdraw ) KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , color_back , font ); // 2018.02.09

//		KGUI_DRAW_StrBox25( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , j , dis->rcItem.bottom - dis->rcItem.top , ( a == 1 ) ? 5 : 0 , dis->rcItem.bottom - dis->rcItem.top - 1 , a , color_line , color_line , c , color_back , font );
		i = i + j;
		z++;
	}
}
//

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  ListView_Draw_Get_TextColor( int bcolor ) { // 2016.10.10
	if ( bcolor == COLOR_White ) return COLOR_Black;
	return KGUI_PALETTE_Get_XColor( bcolor );
}

int LV_Get_Step_real_index( int z ) {
	//
	int c;
	//
	c = 1;
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStartTime == 0 ) {
		if ( z == c ) return 1;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideEndTime == 0 ) {
		if ( z == c ) return 2;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideActualTime == 0 ) {
		if ( z == c ) return 3;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStableTime == 0 ) {
		if ( z == c ) return 4;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStepTime == 0 ) {
		if ( z == c ) return 5;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideSetTime == 0 ) {
		if ( z == c ) return 6;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideResult == 0 ) {
		if ( z == c ) return 7;
		c++;
	}
	//
	return 0;
	//
}

int LV_Get_Item_real_index( int z ) {
	//
	int c;
	//
	c = 2;
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideCount == 0 ) {
		if ( z == c ) return 1;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideTarget == 0 ) {
		if ( z == c ) return 2;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStart == 0 ) {
		if ( z == c ) return 3;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideEnd == 0 ) {
		if ( z == c ) return 4;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMin == 0 ) {
		if ( z == c ) return 5;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMax == 0 ) {
		if ( z == c ) return 6;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideAvr == 0 ) {
		if ( z == c ) return 7;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStdDev == 0 ) {
		if ( z == c ) return 8;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStblTime == 0 ) {
		if ( z == c ) return 9;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStepTime == 0 ) {
		if ( z == c ) return 10;
		c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideResult == 0 ) {
		if ( z == c ) return 11;
		c++;
	}
	//
	return 0;
	//
}

void ListView_Draw_User_with_WM_DRAWITEM_SPM( HWND hwnd , LPARAM lParam , int prm , char *buffer , int buffercount ) { // 2016.08.18
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , b , a , ix , rc;
	int align = 0;
	int color_text , color_back , color_line;
	BOOL sel;
	LVCOLUMN lvcol;
	//
	if ( dis->itemState & ODS_SELECTED ) { // select
		//
		sel = TRUE;
		//
//		color_text = COLOR_White;		color_back = COLOR_DarkBlue;
		color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
		//
//		color_line = COLOR_Blank; // 2016.10.26
		//
		switch( prm ) {
		case 6 :
		case 7 :
			color_line = STEPVIEW_DATA.SV_Opt_Item_LineColor; // 2016.10.26
			break;
		default :
			color_line = STEPVIEW_DATA.SV_Opt_Step_LineColor; // 2016.10.26
			break;
		}
		//
	}
	else {
		//
		sel = FALSE;
		//
//		color_text = COLOR_Black;				color_back = COLOR_White;
		//
		switch( prm ) {
		case 6 :
		case 7 :
			color_back = STEPVIEW_DATA.SV_Opt_Item_BackColor;		color_text = ListView_Draw_Get_TextColor(color_back);
			color_line = STEPVIEW_DATA.SV_Opt_Item_LineColor; // 2016.10.26
			break;
		default :
			color_back = STEPVIEW_DATA.SV_Opt_Step_BackColor;		color_text = ListView_Draw_Get_TextColor(color_back);
			color_line = STEPVIEW_DATA.SV_Opt_Step_LineColor; // 2016.10.26
			break;
		}
		//
//		color_line = COLOR_Blank; // 2016.10.26
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
		switch( prm ) {
		//--------------------------------------------------------
		case 1 : // [1] Total Time
			if ( z == 0 ) {
				a = STEPVIEW_DATA.SV_Opt_Step_Align_TimeTitle;
			}
			else if ( z == 1 ) {
				a = STEPVIEW_DATA.SV_Opt_Step_Align_TimeData;
			}
			break;

		case 2 : // [1] TITLE
		case 3 : // [1] PRE STEP
		case 4 : // [1] STEP
		case 5 : // [1] POST STEP
			//
			ix = LV_Get_Step_real_index( z );
			//
			switch( ix ) {
			case 1 :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_StartTime;
				break;
			case 2 :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_EndTime;
				break;
			case 3 :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_ActualTime;
				break;
			case 4 :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_StableTime;
				break;
			case 5 :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_StepTime;
				break;
			case 6 :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_SetTime;
				break;
			case 7 :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_Result;
				break;
			default :
				a = STEPVIEW_DATA.SV_Opt_Step_Align_Step;
				break;
			}
			//
			break;
		//--------------------------------------------------------
		case 6 : // [2] TITLE
		case 7 :
			//
			if      ( z == 0 ) { // no
				a = STEPVIEW_DATA.SV_Opt_Item_Align_No;
			}
			else if ( z == 1 ) { // item
				a = STEPVIEW_DATA.SV_Opt_Item_Align_Name;
			}
			else {
				//
				ix = LV_Get_Item_real_index( z );
				//
				switch( ix ) {
				case 1 : // Count
					a = STEPVIEW_DATA.SV_Opt_Item_Align_Count;
					break;
				case 2 : // Target
					a = STEPVIEW_DATA.SV_Opt_Item_Align_Target;
					break;
				case 3 : // Start
					a = STEPVIEW_DATA.SV_Opt_Item_Align_Start;
					break;
				case 4 : // End
					a = STEPVIEW_DATA.SV_Opt_Item_Align_End;
					break;
				case 5 : // Min
					a = STEPVIEW_DATA.SV_Opt_Item_Align_Min;
					break;
				case 6 : // Max
					a = STEPVIEW_DATA.SV_Opt_Item_Align_Max;
					break;
				case 7 : // Avr
					a = STEPVIEW_DATA.SV_Opt_Item_Align_Avr;
					break;
				case 8 : // Std
					a = STEPVIEW_DATA.SV_Opt_Item_Align_StdDev;
					break;
				case 9 : // StableTime
					a = STEPVIEW_DATA.SV_Opt_Item_Align_StblTime;
					break;
				case 10 : // StepTime
					a = STEPVIEW_DATA.SV_Opt_Item_Align_StepTime;
					break;
				case 11 : // User
					a = STEPVIEW_DATA.SV_Opt_Item_Align_Result;
					break;
				}
			}
			//
			break;
		}
		//
		if ( sel ) {
			//
			switch( prm ) {
			//--------------------------------------------------------
			case 1 : // [1] Total Time
				if ( z >= 2 ) {
//					b = COLOR_White;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor;
					c = ListView_Draw_Get_TextColor(b);
				}
				break;
			case 2 : // [1] TITLE
				if ( z < 8 ) {
//					b = COLOR_BrightGreen;
//					c = COLOR_Black;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor_StepTitle;
					c = ListView_Draw_Get_TextColor(b);
				}
				break;
			//--------------------------------------------------------
			case 6 : // [2] TITLE
//				b = COLOR_BrightGreen;
//				c = COLOR_Black;
				b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Title;
				c = ListView_Draw_Get_TextColor(b);
				break;
			//--------------------------------------------------------
			}
		}
		else {
			//
			switch( prm ) {
			//--------------------------------------------------------
			case 1 : // [1] Total Time
				if ( z == 0 ) {
//					b = COLOR_PaleBlue;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor_TimeTitle;
					c = ListView_Draw_Get_TextColor(b);
				}
				else if ( z == 1 ) {
//					b = COLOR_BackGray;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor_TimeData;
					c = ListView_Draw_Get_TextColor(b);
				}
				break;
			case 2 : // [1] TITLE
				if ( z < 8 ) {
//					b = COLOR_BrightGreen;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor_StepTitle;
					c = ListView_Draw_Get_TextColor(b);
				}
				break;
			case 3 : // [1] PRE STEP
				if ( z < 8 ) {
//					b = COLOR_BackGray;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor_StepPrePost;
					c = ListView_Draw_Get_TextColor(b);
				}
				break;
			case 4 : // [1] STEP
				if ( z < 8 ) {
//					b = COLOR_LightGray;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor_StepData;
					c = ListView_Draw_Get_TextColor(b);
				}
				break;
			case 5 : // [1] POST STEP
				if ( z < 8 ) {
//					b = COLOR_BackGray;
					b = STEPVIEW_DATA.SV_Opt_Step_BackColor_StepPrePost;
					c = ListView_Draw_Get_TextColor(b);
				}
				break;
			//--------------------------------------------------------
			case 6 : // [2] TITLE
//				b = COLOR_BrightGreen;
//				c = COLOR_Black;
				b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Title;
				c = ListView_Draw_Get_TextColor(b);
				break;
			case 7 :
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_ColorMode == 0 ) {
					if      ( z == 0 ) { // no
	//					b = COLOR_LightGray;
						b = STEPVIEW_DATA.SV_Opt_Item_BackColor_No;
						c = ListView_Draw_Get_TextColor(b);
					}
					else if ( z == 1 ) { // item
	//					b = COLOR_BackGray;
						b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Name;
						c = ListView_Draw_Get_TextColor(b);
					}
					else {
						//
						ix = LV_Get_Item_real_index( z );
						//
						switch( ix ) {
						case 1 : // Count
	//						b = COLOR_LightGray;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Count;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 2 : // Target
							//b = COLOR_PaleBlue;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Target;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 3 : // Start
							//b = COLOR_BackGray;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Start;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 4 : // End
							//b = COLOR_BackGray;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_End;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 5 : // Min
							//b = COLOR_LightGray;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Min;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 6 : // Max
							//b = COLOR_LightGray;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Max;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 7 : // Avr
							//b = COLOR_BackGray;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Avr;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 8 : // Std
							//b = COLOR_BackGray;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_StdDev;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 9 : // StableTime
							//b = COLOR_PaleGreen;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_StableTime;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 10 : // StepTime
							//b = COLOR_PaleGreen;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_StepTime;
							c = ListView_Draw_Get_TextColor(b);
							break;
						case 11 : // User
							//b = COLOR_PaleBlue;
							b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Result;
							c = ListView_Draw_Get_TextColor(b);
							break;
						}
					}
				}
				else {
					//
					rc = STEPVIEW_DATA.SV_Opt_Item_RowSize_Row1 + STEPVIEW_DATA.SV_Opt_Item_RowSize_Row2 + 2;
					//
					if ( ( ( dis->itemID - 1 ) % rc ) < (unsigned) (STEPVIEW_DATA.SV_Opt_Item_RowSize_Row1+1) ) {
						b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Row1;
						c = ListView_Draw_Get_TextColor(b);
					}
					else {
						b = STEPVIEW_DATA.SV_Opt_Item_BackColor_Row2;
						c = ListView_Draw_Get_TextColor(b);
					}
				}
				//
				break;
			}
		}
		//
//		KGUI_DRAW_StrBoxLV(  dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , "" ); // 2016.10.26
		KGUI_DRAW_StrBoxLV2( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , a , color_line , 0 /*lb*/ , c , b , "" , 0 ); // lbline 0:lb 1:l 2:b
		//
		i = i + j;
		z++;
	}
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void ListView_Draw_User_with_WM_DRAWITEM_SPMS( HWND hwnd , LPARAM lParam , char *buffer , int buffercount ) { // 2016.08.18
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , b , a;
	int align = 0;
	int color_text , color_back , color_line;
	LVCOLUMN lvcol;
	BOOL Select;
	//
	if ( dis->itemState & ODS_SELECTED ) { // select
//		color_text = COLOR_White;		color_back = COLOR_DarkBlue;
		color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
//		color_line = COLOR_Blank; // 2016.10.26
		color_line = STEPVIEW_DATA.SV_Opt_Summary_LineColor; // 2016.10.26
		Select = TRUE;
	}
	else {
//		color_text = COLOR_Black;				color_back = COLOR_White;
		color_back = STEPVIEW_DATA.SV_Opt_Summary_BackColor;	color_text = ListView_Draw_Get_TextColor(color_back);
//		color_line = COLOR_Blank; // 2016.10.26
		color_line = STEPVIEW_DATA.SV_Opt_Summary_LineColor; // 2016.10.26
		Select = FALSE;
	}
	//
	i = 0;
	z = 0;
	//
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
		//
		switch( z ) {
		case 0 :
			a = STEPVIEW_DATA.SV_Opt_Summary_Align_NameJob;
			break;
		case 1 :
			a = STEPVIEW_DATA.SV_Opt_Summary_Align_DataJob;
			break;

		case 3 :
			a = STEPVIEW_DATA.SV_Opt_Summary_Align_NamePrcs;
			break;
		case 4 :
			a = STEPVIEW_DATA.SV_Opt_Summary_Align_DataPrcs;
			break;
		}
		//
		//-------------------------------------------
		//
		if ( Select ) {
			//
			c = color_text;
			b = color_back;
			//
		}
		else {
			switch( z ) {
			case 0 :
				//
				c = color_text;
				if ( buffer[0] == 0 ) {
					b = color_back;
					c = ListView_Draw_Get_TextColor(b);
				}
				else {
					if ( dis->itemID >= 3 ) {
//						b = COLOR_Yellow;
						b = STEPVIEW_DATA.SV_Opt_Summary_BackColor_TimeJob;
						c = ListView_Draw_Get_TextColor(b);
					}
					else {
//						b = COLOR_Yellow;
						b = STEPVIEW_DATA.SV_Opt_Summary_BackColor_Job;
						c = ListView_Draw_Get_TextColor(b);
					}
				}
				//
				break;

			case 3 :
				//
				c = color_text;
				if ( buffer[0] == 0 ) {
					b = color_back;
					c = ListView_Draw_Get_TextColor(b);
				}
				else {
					if ( dis->itemID >= 3 ) {
//						b = COLOR_PaleBlue;
						b = STEPVIEW_DATA.SV_Opt_Summary_BackColor_TimePrcs;
						c = ListView_Draw_Get_TextColor(b);
					}
					else {
//						b = COLOR_PaleGreen;
						b = STEPVIEW_DATA.SV_Opt_Summary_BackColor_Prcs;
						c = ListView_Draw_Get_TextColor(b);
					}
				}
				//
				break;

			case 2 :
				//
				c = COLOR_LightGray;
				b = COLOR_LightGray;
				//
				break;

			default :
				//
				c = color_text;
				b = color_back;
				//
				break;
			}
		}
		//
//		KGUI_DRAW_StrBoxLV(  dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , "" ); // 2016.10.26
		KGUI_DRAW_StrBoxLV2( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , a , color_line , 0 /*lb*/ , c , b , "" , 0 ); // lbline 0:lb 1:l 2:b
		//
		i = i + j;
		z++;
	}
}
//
//---------------------------------------------------------------------------------------
int LogView_GetParam( HWND hList , int r ) {
	LV_ITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iItem = r;
	ListView_GetItem( hList , &lvi );
	return lvi.lParam;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Log_Get_DevMode_from_Time( char *data , double *retdata ) {
	//
	int v;
	char T1[64] , T2[64] , T3[64] , TX[64] , TX2[64];
	//
	STR_SEPERATE_CHAR( data , ' ' , T1 , T2 , 63 );
	//
	if ( T2[0] == 0 ) strcpy( T2 , T1 );
	//
	STR_SEPERATE_CHAR( T2 , ':' , T1 , TX , 63 );	// T1:TX
	//
	if ( TX[0] == 0 ) {
		*retdata = atof( T1 );
		return TRUE;
	}
	//
	STR_SEPERATE_CHAR( TX , ':' , T2 , TX2 , 63 );	// T1:T2:TX2
	//
	STR_SEPERATE_CHAR( TX2 , '.' , T3 , TX , 63 );	// T1:T2:T3.TX
	//
	v =	( atol(T1) * 3600 ) + ( atol(T2) * 60 ) + atol(T3);
	//
	if ( TX[0] == 0 ) {
		*retdata = (double) v;
		return TRUE;
	}
	else {
		*retdata = ( (double) v ) + (double) ( atof( TX ) / (double) 1000 );
		return FALSE;
	}
}
//
BOOL Log_Get_ExtractTime_from_Time( char *srcdata , char *trgdata ) { // 2016.10.31
	//
	char ResultString1[256];
	char ResultString2[256];
	char ResultStringx[256];
	//
	switch( STEPVIEW_DATA.SV_Opt_TimeDisplay_Run ) { // def h:m:s.ms h:m:s
	case 1 :
		//
		STR_SEPERATE( srcdata , ResultString1 , ResultStringx , 255 );
		//
		if ( ResultStringx[0] == 0 ) {
			strcpy( trgdata , srcdata );
		}
		else {
			strcpy( trgdata , ResultStringx );
			return TRUE;
		}
		//
		break;

	case 2 :
		//
		STR_SEPERATE( srcdata , ResultString1 , ResultStringx , 255 );
		//
		if ( ResultStringx[0] == 0 ) {
			strcpy( trgdata , srcdata );
		}
		else {
			STR_SEPERATE_CHAR( ResultStringx , '.' , ResultString1 , ResultString2 , 255 );
			//
			strcpy( trgdata , ResultString1 );
			//
			return TRUE;
		}
		//
		break;

	default :
		//
		strcpy( trgdata , srcdata );
		//
		return TRUE;
		//
		break;
		//
	}
	//
	return FALSE;
	//
}
//
BOOL Log_Get_DiffTime_from_Time( char *dataE , char *dataS , double minoffset , char *diffdata ) { // 2016.08.23
	//
	double s , e , os , os2 , tms;
	int t , th , tm , ts;
	//
	if ( ( dataE == NULL ) && ( dataS == NULL ) ) {
		//
//		t = (int) minoffset;
		//
		tms = modf ( minoffset , &os );
		//
		t = (int) os;
		//
	}
	else {
		//
		if ( dataE[0] == 0 ) {
			//
			strcpy( diffdata , "" );
			//
			return FALSE;
			//
		}
		//
		if ( dataS[0] == 0 ) {
			//
			strcpy( diffdata , "" );
			//
			return FALSE;
			//
		}
		//
		Log_Get_DevMode_from_Time( dataE , &e );
		Log_Get_DevMode_from_Time( dataS , &s );
		//
		if ( e < s ) e = e + (24*3600);
		//
//		t = (int) ( (double) e - (double) s - (double) minoffset);
		//
		os2 = (double) ( (double) e - (double) s - (double) minoffset);
		//
		tms = modf ( os2 , &os );
		//
		t = (int) os;
		//
	}
	//
	if ( t < 0 ) {
		//
		strcpy( diffdata , "" );
		//
		return FALSE;
		//
	}
	//
	th = t / 3600;
	tm = ( t - ( th * 3600 ) ) / 60;
	ts = t - ( th * 3600 ) - ( tm * 60 );
	//
	switch( STEPVIEW_DATA.SV_Opt_TimeDisplay_Cal ) { // def xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
	case 1 :
		//
		sprintf( diffdata , "%02d:%02d:%02d" , th , tm , ts ); 
		//
		break;

	case 2 :
		//
		os = (double) ts + tms; 
		//
		sprintf( diffdata , "%02d:%02d:%02.1f" , th , tm , os ); 
		//
		break;

	case 3 :
		//
		os = (double) ts + tms; 
		//
		sprintf( diffdata , "%02d:%02d:%02.2f" , th , tm , os ); 
		//
		break;

	case 4 :
		//
		os = (double) ts + tms; 
		//
		sprintf( diffdata , "%02d:%02d:%02.3f" , th , tm , os ); 
		//
		break;

	case 5 :
		//
		sprintf( diffdata , "%02d" , t ); 
		//
		break;

	case 6 :
		//
		os = (double) t + tms; 
		//
		sprintf( diffdata , "%02.1f" , os ); 
		//
		break;

	case 7 :
		//
		os = (double) t + tms; 
		//
		sprintf( diffdata , "%02.2f" , os ); 
		//
		break;

	case 8 :
		//
		os = (double) t + tms; 
		//
		sprintf( diffdata , "%02.3f" , os ); 
		//
		break;

	default :
		//
		switch( StepView_TimeView_Option ) { // xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
		case 1 :
			//
			os = (double) ts + tms; 
			//
			sprintf( diffdata , "%02d:%02d:%02.1f" , th , tm , os ); 
			//
			break;

		case 2 :
			//
			os = (double) ts + tms; 
			//
			sprintf( diffdata , "%02d:%02d:%02.2f" , th , tm , os ); 
			//
			break;

		case 3 :
			//
			os = (double) ts + tms; 
			//
			sprintf( diffdata , "%02d:%02d:%02.3f" , th , tm , os ); 
			//
			break;

		default :
			//
			sprintf( diffdata , "%02d:%02d:%02d" , th , tm , ts ); 
			//
			break;
		}
		//
		break;
		//
	}
	//
	return TRUE;
	//
}
//
int Log_Get_String_ChkMode_from_Time( char *data ) {
//	A	M	L
//	O	O	O	0
//*	X	O	O	1
//$	O	X	O	2
//@	X	X	O	3
// 02   01
	int l;
	l = strlen( data );
	if ( l <= 0 ) return 0;
	if      ( data[l-1] == '*' ) return 1;
	else if ( data[l-1] == '$' ) return 2;
	else if ( data[l-1] == '@' ) return 3;
	return 0;
}
//------------------------------------------------------------------------------------------
BOOL Log_Get_Save_File_Name( HWND hWnd , char *FileName ) {
/*	OPENFILENAME ofn;
	char Buffer1[200],Buffer2[200]; 
	char szExtDefault[4],szExtFilter[256];
	char curdir[_MAX_PATH+1];
	strcpy( szExtDefault , "*" );
	strcpy( szExtFilter , "*.*" );
	strcpy( Buffer1 , "*.*" );
	strcpy( Buffer2 , "" );
	//
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize    = sizeof(OPENFILENAME);
	ofn.hwndOwner      = hWnd;
	ofn.lpstrFilter    = szExtFilter;
	ofn.nFilterIndex   = 1;
	ofn.lpstrFile      = Buffer1;
	ofn.nMaxFile       = MAX_PATH;
	ofn.lpstrFileTitle = Buffer2;
	ofn.nMaxFileTitle  = Buffer2 ? MAX_PATH : 0;
	ofn.lpstrDefExt    = szExtDefault;
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	_getcwd( curdir , _MAX_PATH );
	if ( GetSaveFileName( &ofn ) ) {
		strncpy( FileName , Buffer1 , 255 );
		_chdir( curdir );
		return TRUE;
	}
	_chdir( curdir );
*/
	if ( MODAL_FILE_SAVE_BOX( hWnd , "" , "Select Save File...." , "*.*" , "subdir|global" , FileName ) ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
void Log_Get_String_Form( HWND hWnd , int index , int start , int end , FILE *fpt ) {
	LV_ITEM lvi;
	int i;
	char S[256];
	if ( index < 0 ) return;
	for ( i = start ; i <= end ; i++ ) {
		lvi.mask = LVIF_TEXT;
		lvi.pszText = S;
		lvi.cchTextMax = 255;
		lvi.iItem = index;
		lvi.iSubItem = i;
		if ( !ListView_GetItem( hWnd , &lvi ) ) {
//			if ( i == 1 ) return; // 2012.06.26
			if ( i == start ) return; // 2012.06.26
			break;
		}
//		if ( i == 1 ) { // 2012.06.26
		if ( i == start ) { // 2012.06.26
			fprintf( fpt , "%s" , lvi.pszText );
		}
		else {
			fprintf( fpt , "\t%s" , lvi.pszText );
		}
	}
	fprintf( fpt , "\n" );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL LogView_Open_Lot_History_Insert( HWND hWnd , int i , char *Msg , char *Msg2 , char *Msg3 , char *Msg4 , int error ) {
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
	lvi.cchTextMax	= 100;
	lvi.iImage		= i;
	lvi.lParam		= ( i * 10000 ) + error;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	ListView_SetItemText( hWnd , i , 1 , Msg2 );
	ListView_SetItemText( hWnd , i , 2 , Msg3 );
	ListView_SetItemText( hWnd , i , 3 , Msg4 ); // 2002.07.08
	return TRUE;
}
//
BOOL LogView_Open_Lot_History_Insert2( HWND hWnd , int i , char *Msg , char *Msg2 , char *Msg3 , char *Msg4 , char *Msg5 , int error ) {
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
	lvi.cchTextMax	= 100;
	lvi.iImage		= i;
	lvi.lParam		= ( i * 10000 ) + error;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	ListView_SetItemText( hWnd , i , 1 , Msg2 );
	ListView_SetItemText( hWnd , i , 2 , Msg3 );
	ListView_SetItemText( hWnd , i , 3 , Msg4 );
	ListView_SetItemText( hWnd , i , 4 , Msg5 );
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//
BOOL LogView_Open_Lot_History_Init( HWND hwndListView ) {
	int         i;
	LV_COLUMN   lvColumn;
	TCHAR       *szString[] = { "No" , "Time" , "Context" , "ID" }; // 2002.07.08
	int         szSize[4]   = {  55  ,     210 ,      470 ,   30 }; // 2002.07.08
	int         szSize_M[4] = {  55  ,     190 ,      470 ,   30 }; // 2002.07.08
	//
	TCHAR       *szString2[] = { "No" , "Time" , "ID" , "Sub" , "Description" };
	int         szSize2[5]   = {  50  ,    160 ,   40 ,   70 ,       470 };
	int         szSize_M2[5] = {  50  ,    140 ,   40 ,   70 ,       470 };

	if ( Get_Prm_DISPLAY_MODE() > 0 ) {
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		if ( Get_Prm_MINISCREEN_MODE() ) {
			for ( i = 0 ; i < 5 ; i++ ) {
				//
				lvColumn.cx = szSize_M2[i];
				//
				if ( i == 2 ) {
					lvColumn.pszText = Get_Prm_METHOD_STRING();
				}
				else {
					lvColumn.pszText = szString2[i];
				}
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
				//
				ListView_InsertColumn( hwndListView , i , &lvColumn );
			}	
		}
		else {
			for ( i = 0 ; i < 5 ; i++ ) {
				//
				lvColumn.cx = szSize2[i];
				//
				if ( i == 2 ) {
					lvColumn.pszText = Get_Prm_METHOD_STRING();
				}
				else {
					lvColumn.pszText = szString2[i];
				}
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
				//
				ListView_InsertColumn( hwndListView , i , &lvColumn );
			}	
		}
		//
	}
	else {
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		if ( Get_Prm_MINISCREEN_MODE() ) {
			for ( i = 0 ; i < 4 ; i++ ) {
				//
				lvColumn.cx = szSize_M[i];
				//
				lvColumn.pszText = szString[i];
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
				//
				ListView_InsertColumn( hwndListView , i , &lvColumn );
			}	
		}
		else {
			for ( i = 0 ; i < 4 ; i++ ) {
				//
				lvColumn.cx = szSize[i];
				//
				lvColumn.pszText = szString[i];
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
				//
				ListView_InsertColumn( hwndListView , i , &lvColumn );
			}	
		}
		//
	}
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------



// 2014.02.13
BOOL LogView_Open_Lot_History_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName , int mode , int wfrid , int append , int jobindex , int mindex , int analstartindex ) {
	int i , a , z , s;
//	char Buffer[256]; // 2014.02.27
	char Buffer[1025]; // 2014.02.27
	char TimeStr[64];
	char ModeNoStr[64];
	char MessageStr[256];
	char CodeSIDStr[256];
	char IDStr[64];
	char ALModeStr[64];
	char ElseStr1[256];
	char ElseStr2[256];
	char ElseStr3[256];
	char ElseStr4[256];

	char TempBuffer[64];
	char Str_Action_Message[64];
	char Str_Module_Message[64];
	char Str_ArmSlot_Message[64];
	int Mode;

	BOOL bres;
	FILE *fpt;
	int encflag;
	DWORD Log_Size;
	int readline;
	//
	//-----------------------------------------------------------------
	Log_Size = Process_Log_File_Get_Size( FileName );
	//
	sprintf( Buffer , "READING [%s] : %.2f Mb" , FileName , (double) Log_Size / 1024000.0 );
	LOG_OPEN_MESSAGE( Buffer );
	//
	readline = 0;
	//-----------------------------------------------------------------
	//
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) {
		//
		if ( hWnd ) {
			sprintf( Buffer , "[%s] File Not Found" , FileName );
			MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error [1]" , MB_ICONHAND );
		}
		//
		return FALSE;
	}
	//
	if ( hlistview ) {
		//
		if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
		//
		if ( append > 0 ) {
			//
			i = ListView_GetItemCount( hlistview );
			//
		}
		else {
			//
			xListView_Clear( hlistview );
			//
			LogView_Open_Lot_History_Init( hlistview );
			//
			i = 0;
			//
		}
		//
		if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
		//
	}
	else {
		i = 0;
	}
	//
	//
	a = 0;
	s = 0;
	//
	if ( Get_Prm_DISPLAY_MODE() > 0 ) {
		//
		encflag = 0; // 2011.12.07
		//
		do {
			if ( hlistview ) {
				if ( LOG_OPEN_CANCELED() ) {
					fclose( fpt );
					return FALSE;
				}
			}
//			bres = Read_Line_From_File4( fpt , Buffer , 255 ); // 2014.02.27
			bres = Read_Line_From_File4( fpt , Buffer , 1024 ); // 2014.02.27
			if ( *Buffer != 0 ) {
				//
//				Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
				Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
				//
				z = 0;
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TimeStr , z , 63 );
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , ModeNoStr , z , 63 );
				//
				if ( STRCMP_L( ModeNoStr , "ALARM" ) ) {
					//
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , MessageStr , z , 255 );
					//
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , CodeSIDStr , z , 255 );
					//
				}
				else {
					//
					strcpy( MessageStr , "                            " );
					//
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , MessageStr + ( 7 * 4 ) , z , 255 );
					//
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , CodeSIDStr , z , 255 );
						//
						// 2017.09.08
						//
						STR_SEPERATE_CHAR( CodeSIDStr , ' ' , Str_Action_Message , TempBuffer , 63 );
						STR_SEPERATE_CHAR( TempBuffer , ':' , Str_Module_Message , Str_ArmSlot_Message , 63 );
						//
						if      ( STRCMP_L( Str_Action_Message , "WHFMSTART" ) ) {
							Mode = 0;
						}
						else if ( STRCMP_L( Str_Action_Message , "WHFMSUCCESS" ) ) {
							Mode = 0;
						}
						else if ( STRCMP_L( Str_Action_Message , "WHFMFAIL" ) ) {
							Mode = 0;
						}
						else if ( STRCMP_L( Str_Action_Message , "WHFMFWPRODUCE" ) ) {
							Mode = 0;
						}
						else if ( STRCMP_L( Str_Action_Message , "WHFMLWPRODUCE" ) ) {
							Mode = 0;
						}
						else {
							//
							if      ( STRNCMP_L( Str_Action_Message , "WHFM" , 4 ) ) {
								Mode = 1 * 4;
							}
							else if ( STRNCMP_L( Str_Action_Message , "WHTM" , 4 ) ) {
								if      ( ( Str_Action_Message[4] >= 'A' ) && ( Str_Action_Message[4] <= 'Z' ) ) {
									Mode = 2 * 4;
								}
								else if ( ( Str_Action_Message[5] >= 'A' ) && ( Str_Action_Message[5] <= 'Z' ) ) {
									Mode = ( Str_Action_Message[4] - '0' + 1 ) * 4;
								}
								else if ( ( Str_Action_Message[6] >= 'A' ) && ( Str_Action_Message[6] <= 'Z' ) ) {
									Mode = ( ( ( Str_Action_Message[4] - '0' ) * 10 ) + ( Str_Action_Message[5] - '0' ) + 1 ) * 4;
								}
							}
							else if ( STRNCMP_L( Str_Action_Message , "PROCESS_" , 8 ) ) {
								Mode = 7 * 4;
							}
							else {
								Mode = 0;
							}
							//
						}
						//
						//
				}
				//
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , IDStr , z , 63 );
				//
				if ( wfrid > 0 ) {
					if ( atoi( IDStr ) != wfrid ) continue;
				}
				//
				i++;
				//
				if ( hlistview ) {
					//
					if ( STRCMP_L( ModeNoStr , "ALARM" ) ) {
						//
	//2011/7/26 22:54:43 	ALARM	<POST>      - [PM       1001 TEST.Alarm1                    PM Alarm 1                                                                                         1 2011/7/26 22:54:42 ]	ALARM
	//2011/7/26 22:54:49 	ALARM	<R=CLEAR>   - [PM       1001 TEST.Alarm1                    Clear                                                                                              1 2011/7/26 22:54:42 ]	ALARM
	//2011/7/26 22:54:49 	ALARM	<CLEAR>     - [PM       1001 TEST.Alarm1                    PM Alarm 1                                                                                         1 2011/7/26 22:54:42 ]	ALARM
	//2011/7/26 22:54:54 	ALARM	<POST>      - [PM       1002 TEST.Alarm2                    PM Alarm 2                                                                                         2 2011/7/26 22:54:54 ]	ALARM
						//          01234567890-2--56789012-4567-901234567890123456789012345678-01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567-9-1234567890123456789-
						//          0         1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6         7         8

						if      ( STRNCMP_L( MessageStr , "<POST>" , 6 ) ) {
							strcpy( ALModeStr , "<POST>    " ); // 2012.03.23
							//
	//						if ( IDStr[0] == 0 ) {
	//							strcpy( IDStr , "<POST>" );
	//						}
	//						else {
	//							strcat( IDStr , " <POST>" );
	//						}
						}
						else if ( STRNCMP_L( MessageStr , "<R=" , 3 ) ) {
							strcpy( ALModeStr , "<" ); // 2012.03.23
							strncat( ALModeStr , MessageStr + 3 , 9 ); // 2012.03.23
							//
	//						if ( IDStr[0] == 0 ) {
	//							strcpy( IDStr , "<" );
	//							strncat( IDStr , MessageStr + 3 , 9 );
	//						}
	//						else {
	//							strcat( IDStr , " <" );
	//							strncat( IDStr , MessageStr + 3 , 9 );
	//						}
						}
						else if ( STRNCMP_L( MessageStr , "<CLEAR>" , 7 ) ) {
							i--;
							continue;
						}
						else {
							strcpy( ALModeStr , "<ALARM>   " ); // 2012.03.23
	//					
	//						if ( IDStr[0] == 0 ) {
	//							strcpy( IDStr , "<ALARM>" );
	//						}
	//						else {
	//							strcat( IDStr , " <ALARM>" );
	//						}
						}
						//
						Ext_Str_From_Str( MessageStr , 15 , 8 , ElseStr1 );	STRTRUNCATE_SPACE( ElseStr1 );
						Ext_Str_From_Str( MessageStr , 24 , 4 , ElseStr2 );	STRTRUNCATE_SPACE( ElseStr2 );
						//
						if ( ElseStr1[0] == 0 ) {
							sprintf( CodeSIDStr , "%s" , ElseStr2 );
						}
						else {
							sprintf( CodeSIDStr , "%s-%s" , ElseStr1 , ElseStr2 );
						}
						//
						Ext_Str_From_Str( MessageStr ,  29 , 30 , ElseStr1 );	STRTRUNCATE_SPACE( ElseStr1 );
						Ext_Str_From_Str( MessageStr ,  60 , 98 , ElseStr2 );	STRTRUNCATE_SPACE( ElseStr2 );
						Ext_Str_From_Str( MessageStr , 159 ,  1 , ElseStr3 );	STRTRUNCATE_SPACE( ElseStr3 );
						Ext_Str_From_Str( MessageStr , 161 , 19 , ElseStr4 );	STRTRUNCATE_SPACE( ElseStr4 );
						//
						sprintf( Buffer , "%s[NAME=%s][MESSAGE=%s][Level=%s][Time=%s]" , ALModeStr , ElseStr1 , ElseStr2 , ElseStr3 , ElseStr4 );
						//
						sprintf( ModeNoStr , "%d" , i );
						//
						if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
						//
						LogView_Open_Lot_History_Insert2( hlistview , i - 1 , ModeNoStr , TimeStr , IDStr , CodeSIDStr , Buffer , ( i * 10000 ) + 81 );
						//
						if ( mode == 1 ) { // 2011.12.06
							//
							if ( s == 0 ) {
								//
								s = 1;
								//
								LogView_Open_Lot_History_Insert2( hlistview , 0 , "" , "" , "" , "" , "" , ( 0 * 10000 ) + 81 );
								//
								i++;
								//
							}
							//
							LogView_Open_Lot_History_Insert2( hlistview , 0 , ModeNoStr , TimeStr , IDStr , CodeSIDStr , Buffer , ( 0 * 10000 ) + 81 );
							//
							i++;
							//
						}
						//
						if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
						//
						//
						a = 1;
						//
					}
					else if ( STRNCMP_L( ModeNoStr , "LOT" , 3 ) ) {
						//
						sprintf( ModeNoStr , "%d" , i );
						//
						if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
						//
						LogView_Open_Lot_History_Insert2( hlistview , i - 1 , ModeNoStr , TimeStr , IDStr , "" , MessageStr + 28 - Mode , ( i * 10000 ) + 0 );
						//
						if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
						//
					}
					else {
						sprintf( ModeNoStr , "%d" , i );
						//
						if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
						//
						LogView_Open_Lot_History_Insert2( hlistview , i - 1 , ModeNoStr , TimeStr , IDStr , "" , MessageStr + 28 - Mode , ( i * 10000 ) + 0 );
						//
						if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
						//
					}
				}
				else {
					if ( STRNCMP_L( ModeNoStr , "LOT" , 3 ) ) {
						//
						STR_SEPERATE_CHAR( CodeSIDStr , ' ' , ElseStr1 , ElseStr4 , 63 );
						STR_SEPERATE_CHAR( ElseStr4 , ':' , ElseStr2 , ElseStr3 , 63 );
						//
						LOT_ANAL_DATA_APPEND( jobindex , mindex , analstartindex , TimeStr , IDStr , ElseStr1 , ElseStr2 , ElseStr3 );
						//
					}
				}
				//=========================================================================================
				if ( sepload == 1 ) { // 2014.02.06
					//
					readline++;
					//
					if ( readline >= SEP_ONE_TIME_MAX_READ_ITEM_COUNT ) {
						//
						readline = 0;
						//
						sprintf( Buffer , "READING [%s] : %d Line(s) : %.2f Mb / %.2f Mb" , FileName , i , (double) ftell( fpt ) / 1024000.0 , (double) Log_Size / 1024000.0 );
						//
						LOG_OPEN_MESSAGE( Buffer );
						//
					}
				}
				//=========================================================================================
			}
		}
		while ( bres );
		//
		if ( hlistview ) {
			if ( mindex == -1 ) {
				if ( a == 0 ) {
					//
					if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
					//
					if ( LISTVIEW_HEIGHT_UPDATE ) { // 2015.07.29
						ListView_SetColumnWidth( hlistview , 2 , (int) ( 50.0 * LISTVIEW_HEIGHT_RATIO ) );
					}
					else {
						ListView_SetColumnWidth( hlistview , 2 , 50 );
					}
					//
					ListView_DeleteColumn( hlistview , 3 );
					//
					if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
					//
				}
			}
		}
		//
	}
	else {
		//
		if ( hlistview ) {
			//
			encflag = 0; // 2011.12.07
			//
			do {
	//			bres = Read_Line_From_File4( fpt , Buffer , 255 ); // 2014.02.27
				bres = Read_Line_From_File4( fpt , Buffer , 1024 ); // 2014.02.27
				if ( *Buffer != 0 ) {
					//
	//				Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
					Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
					//
					i++;
					z = 0;
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TimeStr , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , ModeNoStr , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , MessageStr , z , 255 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , CodeSIDStr , z , 255 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , IDStr , z , 63 );
					if ( STRCMP_L( ModeNoStr , "ALARM" ) ) {
						TimeStr[53] = 0;
						strcat( TimeStr , "(ALARM)" );
						//
						sprintf( ModeNoStr , "%d" , i );
						//
						if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
						//
						LogView_Open_Lot_History_Insert( hlistview , i - 1 , ModeNoStr , TimeStr , MessageStr , IDStr , ( i * 10000 ) + 81 ); // 2002.07.08
						//
						if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
						//
					}
					else {
						sprintf( ModeNoStr , "%d" , i );
						//
						if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
						//
						LogView_Open_Lot_History_Insert( hlistview , i - 1 , ModeNoStr , TimeStr , MessageStr , IDStr , ( i * 10000 ) + 0 ); // 2002.07.08
						//
						if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
						//
					}
				}
			}
			while ( bres );
			//
		}
	}
	fclose( fpt );
	//
	return TRUE;
	//
}

//
BOOL LogView_Open_Lot_User_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName , int option , int option2 , int option3 , int option4 , int option5 , int option6 ) {
	//
	LV_COLUMN   lvColumn;
	LV_ITEM lvi;

	BOOL bres;
	FILE *fpt;
	int encflag;
	//
//	char Buffer[512]; // 2014.02.27
	char Buffer[1025]; // 2014.02.27
	//
	int ccolcnt;
	int csize[256];
	int csizeset[256];

	char ResultString1[512];
	char ResultString2[512];
	char Str[64];
	//
	int z , z2 , i , c;
	//
	DWORD Log_Size;
	int readline;

	//-----------------------------------------------------------------
	Log_Size = Process_Log_File_Get_Size( FileName );
	//
	sprintf( Buffer , "READING [%s] : %.2f Mb" , FileName , (double) Log_Size / 1024000.0 );
	LOG_OPEN_MESSAGE( Buffer );
	//
	readline = 0;
	//-----------------------------------------------------------------
	//
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) {
		sprintf( Buffer , "[%s] File Not Found" , FileName );
		MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error [2]" , MB_ICONHAND );
		return FALSE;
	}
	//
	if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
	//
	xListView_Clear( hlistview );
	//
	if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
	//
	for ( c = 0 ; c < 256 ; c++ ) {
		csize[c] = 0;
		csizeset[c] = FALSE;
	}
	//
	ccolcnt = 0;
	//
	i = 0;
	//
	encflag = 0;
	//
	do {
		//
		if ( LOG_OPEN_CANCELED() ) {
			fclose( fpt );
			return FALSE;
		}
		//
//		bres = Read_Line_From_File4( fpt , Buffer , 511 ); // 2014.02.27
		bres = Read_Line_From_File4( fpt , Buffer , 1024 ); // 2014.02.27
		//
		if ( *Buffer != 0 ) {
			//
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer );
			//
			if ( Buffer[0] == '#' ) continue; // 2014.02.26
			//
			STR_SEPERATE_CHAR( Buffer , 9 , ResultString1 , ResultString2 , 511 );
			//
			if      ( STRCMP_L( ResultString1 , "$TITLE" ) ) {
			}
			else if ( STRCMP_L( ResultString1 , "$COLUMN" ) ) {
				//
				z = 0;
				//
				while( TRUE ) {
					//
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( ResultString2 , '|' , Str , z , 63 );
					if ( z == z2 ) break;
					//===============================================================================
					if ( ccolcnt >= 256 ) break;
					//
					//
					if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
					//
					lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvColumn.fmt = LVCFMT_LEFT;
					lvColumn.cx = 30;
					lvColumn.pszText = Str;
					//
					if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
					//
					ListView_InsertColumn( hlistview , ccolcnt , &lvColumn );
					//
					ccolcnt++;
					//===============================================================================
					//
					if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
					//
					z = z2;
				}
				//
			}
			else if ( STRCMP_L( ResultString1 , "$SIZE" ) ) {
				//
				z = 0;
				c = 0;
				//
				while( TRUE ) {
					//
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( ResultString2 , '|' , Str , z , 63 );
					if ( z == z2 ) break;
					//===============================================================================
					if ( c >= 256 ) break;
					csize[c] = atoi( Str );
					if ( csize[c] > 0 ) csizeset[c] = TRUE;
					c++;
					//===============================================================================
					z = z2;
				}
				//
			}
			else {
				//
				z = 0;
				c = 0;
				//
				while( TRUE ) {
					//
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Str , z , 63 );
					//
					if ( z == z2 ) break;
					//===============================================================================
					if ( c >= 256 ) break;
					//
					//
					if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
					//
					if ( ccolcnt <= c ) {
						//===============================================================================
						if ( !csizeset[c] ) csize[c] = 30;
						//
						lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
						lvColumn.fmt = LVCFMT_LEFT;
						lvColumn.cx = csize[c];
						lvColumn.pszText = "";
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hlistview , ccolcnt , &lvColumn );
						ccolcnt++;
						//===============================================================================
					}
					//
					if ( c == 0 ) {
						//
						lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
						lvi.state = lvi.stateMask = 0;
						lvi.iItem		= i;
						lvi.iSubItem	= 0;
						lvi.pszText		= Str;
						lvi.cchTextMax	= 100;
						lvi.iImage		= i;
						lvi.lParam		= ( i * 10000 ) + 3000;
						if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) break;
						//
						i++;
						//
					}
					else {
						ListView_SetItemText( hlistview , i - 1 , c , Str );
					}
					//
					if ( !csizeset[c] ) {
						z = ListView_GetStringWidth( hlistview , Str ) + 12;
						if ( csize[c] < z ) {
							csize[c] = z;
						}
					}
					//
					if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
					//
					//
					c++;
					//
					//===============================================================================
					z = z2;
				}
				//
			}
			//
			//=========================================================================================
			if ( sepload == 1 ) { // 2014.02.06
				//
				readline++;
				//
				if ( readline >= SEP_ONE_TIME_MAX_READ_ITEM_COUNT ) {
					//
					readline = 0;
					//
					sprintf( Buffer , "READING [%s] : %d Line(s) : %.2f Mb / %.2f Mb" , FileName , i , (double) ftell( fpt ) / 1024000.0 , (double) Log_Size / 1024000.0 );
					//
					LOG_OPEN_MESSAGE( Buffer );
					//
				}
			}
			//=========================================================================================
		}
	}
	while ( bres );
	//
	fclose( fpt );
	//
	if ( !xListView_Enter() ) {	return FALSE;	}
	//
	for ( c = 0 ; c < ccolcnt ; c++ ) {
		ListView_SetColumnWidth( hlistview , c , ( csize[c] <= 30 ) ? 30 : csize[c] );
	}
	//
	if ( !xListView_Leave() ) {	return FALSE;	}
	//
	//
	return TRUE;
	//
}


double LT_ANAL_FM_START[100];
double LT_ANAL_TM_START[100];
double LT_ANAL_BM_FMSIDE_START[100];
double LT_ANAL_BM_TMSIDE_START[100];
double LT_ANAL_PM_START[100];
double LT_ANAL_AL_START[100];
double LT_ANAL_IC_START[100];
BOOL   LT_ANAL_IC_WAIT[100];


void LOT_ANAL_DATA_APPEND( int jobindex , int mindex , int analstartindex , char *Timestr , char *IDStr , char *Str_Action_Message , char *Str_Module_Message , char *Str_ArmSlot_Message ) {
	int offset , id0 , id1 , id2;
	int mode , act , Res , tm;
	//
	double td;
	//
	id0 = atoi(IDStr);
	//
	if      ( STRCMP_L( Str_Action_Message , "WHFMSTART" ) ) {
		return;
	}
	else if ( STRCMP_L( Str_Action_Message , "WHFMSUCCESS" ) ) {
		return;
	}
	else if ( STRCMP_L( Str_Action_Message , "WHFMFAIL" ) ) {
		return;
	}
	else if ( STRCMP_L( Str_Action_Message , "WHFMFWPRODUCE" ) ) {
		return;
	}
	else if ( STRCMP_L( Str_Action_Message , "WHFMLWPRODUCE" ) ) {
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "RECIPE" , 6 ) ) {
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "HANDOFF" , 7 ) ) {
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "MAPPING" , 7 ) ) {
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "SCH" , 3 ) ) {
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "BEGIN" , 5 ) ) {
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "END" , 3 ) ) {
		return;
	}
	//
	if ( id0 == 0 ) {
		if ( IDStr[0] != 0 ) {
//printf( "A.[Timestr=%s][IDStr=%s][Action=%s][Module=%s][ArmSlot=%s]\n" , Timestr , IDStr , Str_Action_Message , Str_Module_Message , Str_ArmSlot_Message );
		}

		return;
	}
	//
	id1 = id0 / 100;
	id2 = id0 % 100;
	//
	tm = -1;
	//
	if      ( STRNCMP_L( Str_Action_Message , "WHFM" , 4 ) ) {
		act = 0;
		offset = 4;
	}
	else if ( STRNCMP_L( Str_Action_Message , "WHTM" , 4 ) ) {
		act = 1;
		offset = 4;
		//
		if ( ( Str_Action_Message[4] >= '1' ) && ( Str_Action_Message[4] <= '9' ) ) {
			//
			tm = Str_Action_Message[4] - '0';
			//
			offset++;
			//
			if ( ( Str_Action_Message[5] >= '0' ) && ( Str_Action_Message[5] <= '9' ) ) {
				//
				tm = (tm*10) + ( Str_Action_Message[5] - '0' );
				//
				offset++;
				//
				if ( ( Str_Action_Message[6] >= '0' ) && ( Str_Action_Message[6] <= '9' ) ) { // Scheduler Bug 贸府
					//
					tm = (tm*10) + ( Str_Action_Message[6] - '0' );
					//
					offset++;
					//
					if ( ( Str_Action_Message[7] >= '0' ) && ( Str_Action_Message[7] <= '9' ) ) { // Scheduler Bug 贸府
						//
						tm = (tm*10) + ( Str_Action_Message[7] - '0' );
						//
						offset++;
						//
					}
					//
					tm = tm % 100;
					//
				}
			}
		}
		//
	}
	else if ( STRNCMP_L( Str_Action_Message , "PROCESS_PRE_" , 12 ) ) { // 2018.09.10
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "PROCESS_POST_" , 13 ) ) { // 2018.09.10
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message , "PROCESS_" , 8 ) ) {
		act = -1;
		offset = 8;
	}
	else {
//printf( "B.[Timestr=%s][IDStr=%s][Action=%s][Module=%s][ArmSlot=%s]\n" , Timestr , IDStr , Str_Action_Message , Str_Module_Message , Str_ArmSlot_Message );
		return;
	}
	//
	if      ( STRNCMP_L( Str_Action_Message + offset , "PICK" , 4 ) ) {
		//
		if ( act == 1 ) return;
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		if ( STRCMP_L( Str_Module_Message , "IC" ) ) {
			//
			if ( id1 > 0 ) {
				if ( LT_ANAL_IC_WAIT[id1] ) {
					//
					Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_COOL , -1 , "IC" , LT_ANAL_IC_START[id1] , td );
					//
					LT_ANAL_IC_WAIT[id1] = 0;
					//
				}
			}
			if ( id2 > 0 ) {
				if ( LT_ANAL_IC_WAIT[id2] ) {
					//
					Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_COOL , -1 , "IC" , LT_ANAL_IC_START[id2] , td );
					//
					LT_ANAL_IC_WAIT[id2] = 0;
					//
				}
			}
			//
		}
		//
		mode = TCH_FM_PICK;
		//
		offset = offset + 4;
		//
	}
	else if ( STRNCMP_L( Str_Action_Message + offset , "PLACE" , 5 ) ) {
		//
		if ( act == 1 ) return;
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		mode = TCH_FM_PLACE;
		//
		offset = offset + 5;
		//
		if ( id1 > 0 ) LT_ANAL_IC_WAIT[id1] = 0;
		if ( id2 > 0 ) LT_ANAL_IC_WAIT[id2] = 0;
		//
	}
	else if ( STRNCMP_L( Str_Action_Message + offset , "SWAP" , 4 ) ) {
		//
		if ( act == 0 ) return;
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		mode = TCH_TM_SWAP;
		//
		offset = offset + 4;
		//
	}
	else if ( STRNCMP_L( Str_Action_Message + offset , "MOVE" , 4 ) ) {
		//
		if ( act == 0 ) {
			mode = TCH_FM_MOVE;
		}
		else {
			mode = TCH_TM_MOVE;
		}
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		offset = offset + 4;
		//
	}
	else if ( STRNCMP_L( Str_Action_Message + offset , "POST_" , 5 ) ) {
		//
		mode = TCH_PROCESS_X;
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		offset = offset + 5;
		//
	}
	else if ( STRNCMP_L( Str_Action_Message + offset , "PRE_" , 4 ) ) {
		//
		mode = TCH_PROCESS_X;
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		offset = offset + 4;
		//
	}
	else if ( STRCMP_L( Str_Action_Message + offset , "GOFMSIDE_END" ) ) {
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_BM_FMSIDE , -1 , Str_Module_Message , LT_ANAL_BM_FMSIDE_START[id1] , td );
		if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_BM_FMSIDE , -1 , Str_Module_Message , LT_ANAL_BM_FMSIDE_START[id2] , td );
		//
		offset = offset + 12;
		//
		return;
	}
	else if ( STRCMP_L( Str_Action_Message + offset , "GOTMSIDE_END" ) ) {
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_BM_TMSIDE , -1 , Str_Module_Message , LT_ANAL_BM_TMSIDE_START[id1] , td );
		if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_BM_TMSIDE , -1 , Str_Module_Message , LT_ANAL_BM_TMSIDE_START[id2] , td );
		//
		offset = offset + 12;
		//
		return;
	}
	else if ( STRCMP_L( Str_Action_Message + offset , "GOFMSIDE" ) ) {
		//
		mode = TCH_BM_FMSIDE;
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		if ( id1 > 0 ) LT_ANAL_BM_FMSIDE_START[id1] = td;
		if ( id2 > 0 ) LT_ANAL_BM_FMSIDE_START[id2] = td;
		//
		offset = offset + 8;
		//
		return;
	}
	else if ( STRCMP_L( Str_Action_Message + offset , "GOTMSIDE" ) ) {
		//
		mode = TCH_BM_TMSIDE;
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		if ( id1 > 0 ) LT_ANAL_BM_TMSIDE_START[id1] = td;
		if ( id2 > 0 ) LT_ANAL_BM_TMSIDE_START[id2] = td;
		//
		offset = offset + 8;
		//
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message + offset , "AL" , 2 ) ) {
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		offset = offset + 2;
		//
		if ( STRCMP_L( Str_Action_Message + offset , "SUCCESS" ) ) {
			//
			offset = offset + 7;
			//
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_ALIGN , -1 , "AL" , LT_ANAL_AL_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_ALIGN , -1 , "AL" , LT_ANAL_AL_START[id2] , td );
			//

		}
		else if ( STRCMP_L( Str_Action_Message + offset , "FAIL" ) ) {
			//
			offset = offset + 4;
			//
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_ALIGN , -1 , "AL" , LT_ANAL_AL_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_ALIGN , -1 , "AL" , LT_ANAL_AL_START[id2] , td );
			//
		}
		else if ( STRCMP_L( Str_Action_Message + offset , "RUN" ) ) {
			offset = offset + 3;
			//
			if ( id1 > 0 ) LT_ANAL_AL_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_AL_START[id2] = td;
			//
		}
		//
		return;
	}
	else if ( STRNCMP_L( Str_Action_Message + offset , "IC" , 2 ) ) {
		//
		td = CURRENT_WAFER_GET_TIME( mindex , Timestr );
		//
		offset = offset + 2;
		//
		if ( STRCMP_L( Str_Action_Message + offset , "SUCCESS" ) ) {
			//
			offset = offset + 7;
			//
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_COOL , -1 , "IC" , LT_ANAL_IC_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_COOL , -1 , "IC" , LT_ANAL_IC_START[id2] , td );
			//
			if ( id1 > 0 ) LT_ANAL_IC_WAIT[id1] = 0;
			if ( id2 > 0 ) LT_ANAL_IC_WAIT[id2] = 0;
		}
		else if ( STRCMP_L( Str_Action_Message + offset , "FAIL" ) ) {
			//
			offset = offset + 4;
			//
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_COOL , -1 , "IC" , LT_ANAL_IC_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_COOL , -1 , "IC" , LT_ANAL_IC_START[id2] , td );
			//
			if ( id1 > 0 ) LT_ANAL_IC_WAIT[id1] = 0;
			if ( id2 > 0 ) LT_ANAL_IC_WAIT[id2] = 0;
		}
		else if ( STRCMP_L( Str_Action_Message + offset , "RUN" ) ) {
			offset = offset + 3;
			//
			if ( id1 > 0 ) LT_ANAL_IC_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_IC_START[id2] = td;
			//
			if ( id1 > 0 ) LT_ANAL_IC_WAIT[id1] = 1;
			if ( id2 > 0 ) LT_ANAL_IC_WAIT[id2] = 1;
		}
		//
		return;
	}
	else {
		if ( act != -1 ) {
//printf( "C.[Timestr=%s][IDStr=%s][Action=%s][Module=%s][ArmSlot=%s]\n" , Timestr , IDStr , Str_Action_Message , Str_Module_Message , Str_ArmSlot_Message );
		}
		return;
	}
	//===================================================================================================
	if      ( STRCMP_L( Str_Action_Message + offset , "START" ) ) {
		switch( mode ) {
		case TCH_FM_PICK :
			if ( id1 > 0 ) LT_ANAL_FM_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_FM_START[id2] = td;
			break;							
		case TCH_FM_PLACE :					
			if ( id1 > 0 ) LT_ANAL_FM_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_FM_START[id2] = td;
			break;							
		case TCH_TM_SWAP :					
			if ( id1 > 0 ) LT_ANAL_TM_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_TM_START[id2] = td;
			break;							
		case TCH_FM_MOVE :					
			if ( id1 > 0 ) LT_ANAL_FM_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_FM_START[id2] = td;
			break;							
		case TCH_TM_MOVE :					
			if ( id1 > 0 ) LT_ANAL_TM_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_TM_START[id2] = td;
			break;							
		case TCH_PROCESS_X :				
			if ( id1 > 0 ) LT_ANAL_PM_START[id1] = td;
			if ( id2 > 0 ) LT_ANAL_PM_START[id2] = td;
			break;
		}
	}
	else if (
		( STRCMP_L( Str_Action_Message + offset , "END" ) ) ||
		( STRCMP_L( Str_Action_Message + offset , "SUCCESS" ) ) ||
		( STRCMP_L( Str_Action_Message + offset , "FAIL" ) )
		) {
		switch( mode ) {
		case TCH_FM_PICK :
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , mode , 0 , Str_Module_Message , LT_ANAL_FM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , mode , 0 , Str_Module_Message , LT_ANAL_FM_START[id2] , td );
			break;														 
		case TCH_FM_PLACE :												 
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , mode , 0 , Str_Module_Message , LT_ANAL_FM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , mode , 0 , Str_Module_Message , LT_ANAL_FM_START[id2] , td );
			break;														 
		case TCH_TM_SWAP :												 
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , mode , tm , Str_Module_Message , LT_ANAL_TM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , mode , tm , Str_Module_Message , LT_ANAL_TM_START[id2] , td );
			break;														 
		case TCH_FM_MOVE :												 
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , mode , 0 , Str_Module_Message , LT_ANAL_FM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , mode , 0 , Str_Module_Message , LT_ANAL_FM_START[id2] , td );
			break;														 
		case TCH_TM_MOVE :												 
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , mode , tm , Str_Module_Message , LT_ANAL_TM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , mode , tm , Str_Module_Message , LT_ANAL_TM_START[id2] , td );
			break;														 
		case TCH_PROCESS_X :											 
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , mode , -1 , Str_Module_Message , LT_ANAL_PM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , mode , -1 , Str_Module_Message , LT_ANAL_PM_START[id2] , td );
			break;
		}
	}
	else if ( STRCMP_L( Str_Action_Message + offset , "REJECT" ) ) {
		switch( mode ) {
		case TCH_FM_PICK :
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_REJECT , 0 , Str_Module_Message , LT_ANAL_FM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_REJECT , 0 , Str_Module_Message , LT_ANAL_FM_START[id2] , td );
			break;
		case TCH_FM_PLACE :
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_REJECT , 0 , Str_Module_Message , LT_ANAL_FM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_REJECT , 0 , Str_Module_Message , LT_ANAL_FM_START[id2] , td );
			break;
		case TCH_TM_SWAP :
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_REJECT , tm , Str_Module_Message , LT_ANAL_TM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_REJECT , tm , Str_Module_Message , LT_ANAL_TM_START[id2] , td );
			break;
		case TCH_FM_MOVE :
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_REJECT , 0 , Str_Module_Message , LT_ANAL_FM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_REJECT , 0 , Str_Module_Message , LT_ANAL_FM_START[id2] , td );
			break;
		case TCH_TM_MOVE :
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_REJECT , tm , Str_Module_Message , LT_ANAL_TM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_REJECT , tm , Str_Module_Message , LT_ANAL_TM_START[id2] , td );
			break;
		case TCH_PROCESS_X :
			if ( id1 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id1 , analstartindex , TCH_REJECT , -1 , Str_Module_Message , LT_ANAL_PM_START[id1] , td );
			if ( id2 > 0 ) Res = CURRENT_WAFER_APPEND_MODULE_F( jobindex , id2 , analstartindex , TCH_REJECT , -1 , Str_Module_Message , LT_ANAL_PM_START[id2] , td );
			break;
		}
	}
	else {
//printf( "D.[Timestr=%s][IDStr=%s][Action=%s][Module=%s][ArmSlot=%s]\n" , Timestr , IDStr , Str_Action_Message , Str_Module_Message , Str_ArmSlot_Message );
		return;
	}
	//===================================================================================================
}

int CURRENT_WAFER_GET_COUNT();
BOOL LogView_Open_Method_History_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName0 , int jobindex , int infoinsert , int findwid , int fileorder , int mindex , int option6 );

BOOL LogView_Open_Method_History_Insert_CURRENT_WAFER( HWND hwndListView , BOOL multimode , int index0 );

BOOL LogView_Open_Lot_History_Data_Set( HWND hdlg , int winpos , int mode , int wfrid , char *FileName_org , char *FileName , int index , BOOL sepload ) {
	int i , f , sx , ix;
	char folderstr[256];
	int mcheck[MAX_MULTI_CHECK_JOB];
	//-----------------------------------------------------------------
	BOOL Res;
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_Start( hdlg );
	//-----------------------------------------------------------------
	//
	if ( mode == 9999 ) {
		//
		f = 0;
		//
		CURRENT_WAFER_INIT( 0 );
		//
		for ( i = 0 ; i < MAX_MULTI_CHECK_JOB ; i++ ) mcheck[i] = 0;
		//
		while ( TRUE ) {
			//
			ix = -1;
			//
			for ( i = 0 ; i < index ; i++ ) {
				//
				if ( mcheck[i] != 0 ) continue;
				//
				if ( ix == -1 ) {
					ix = i;
				}
				else {
					if ( FLOW_HISTORY_M_INDEX[ix] > FLOW_HISTORY_M_INDEX[i] ) {
						ix = i;
					}
				}
			}
			//
			if ( ix == -1 ) break;
			//
			if ( JOB_Get_Directory_Type( FLOW_HISTORY_M_INDEX[ix] ) == 1 ) {
				//
				sx = CURRENT_WAFER_GET_COUNT();
				//
				LogView_Open_Method_History_Data_Set_Callback( NULL , NULL , 0 , NULL , FLOW_HISTORY_M_INDEX[ix] , 1 , -1 , -1 , f , 0 );
				//
				JOB_Get_Directory( FLOW_HISTORY_M_INDEX[ix] , folderstr );
				//----------------------------------------------------------------------------------------------
				sprintf( FileName_org , "%s/LotLog/%s/LotData.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
				//----------------------------------------------------------------------------------------------
				if ( !Get_Prm_NETMONITOR_MODE() ) {
					strcpy( FileName , FileName_org );
				}
				else {
					sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
					_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
					_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
				}
				//
				LogView_Open_Lot_History_Data_Set_Callback( NULL , NULL , 0 , FileName , 0 , 0 , f /*append */ , FLOW_HISTORY_M_INDEX[ix] , f , sx );
				//
				f++;
				//
			}
			//
			mcheck[ix] = 1;
			//
		}
		//
		LogView_Open_Method_History_Insert_CURRENT_WAFER( GetDlgItem( hdlg , winpos ) , TRUE , -99 );
		//
	}
	else {
		//
		if ( !Get_Prm_NETMONITOR_MODE() ) {
			strcpy( FileName , FileName_org );
		}
		else {
			sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
			_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
			_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
		}
		//
		Res = LOG_OPEN_CONTROL( hdlg , GetDlgItem( hdlg , winpos ) , sepload , FileName , mode , wfrid , 0 /*append */ , index , -1 , 0 , LogView_Open_Lot_History_Data_Set_Callback );
		//
	}
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_End( hdlg );
	//-----------------------------------------------------------------
	return Res;
	//-----------------------------------------------------------------
}


BOOL LogView_Open_Lot_User_Data_Set( HWND hdlg , int winpos , char *FileName_org , char *FileName , BOOL sepload ) {
	//-----------------------------------------------------------------
	BOOL Res;
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_Start( hdlg );
	//-----------------------------------------------------------------
	//
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else {
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
	}
	//
	Res = LOG_OPEN_CONTROL( hdlg , GetDlgItem( hdlg , winpos ) , sepload , FileName , 0 , 0 , 0 , 0 , 0 , 0 , LogView_Open_Lot_User_Data_Set_Callback );
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_End( hdlg );
	//-----------------------------------------------------------------
	return Res;
	//-----------------------------------------------------------------
}

BOOL APIENTRY LogView_Open_Lot_History_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int i , j;
//	int z;
	static char FileName[256];
//	static char FileName_org[256];
	char Buffer[256];
//	char Buffer1[64];
//	char Buffer2[64];
	char Buffer3[256];
//	char Buffer4[64]; // 2002.07.08
//	BOOL bres;
//	FILE *fpt;

	RECT rect;		// crkim // 2002.11.11

	int  color_error; // 2008.06.05
	LPDRAWITEMSTRUCT dis; // 2008.06.05
	LPMEASUREITEMSTRUCT dim; // 2015.07.29

	switch ( msg ) {

	case WM_MEASUREITEM: // 2015.07.29
		//
		if ( ( GLISTVIEW_HEIGHT > 0 ) && ( ( GLISTVIEW_HEIGHT % 1000 ) >= 10 ) ) {
			//
			dim = (LPMEASUREITEMSTRUCT) lParam;
			//
			if ( dim->CtlType == ODT_LISTVIEW ) {
				if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
					 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
				}
			}
			//
		}
		else {
			if ( LISTVIEW_HEIGHT_UPDATE ) {
				//
				dim = (LPMEASUREITEMSTRUCT) lParam;
				//
				if ( dim->CtlType == ODT_LISTVIEW ) {
					if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
						 dim->itemHeight = LISTVIEW_HEIGHT;
					}
				}
				//
			}
		}
		//
		return TRUE;


	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
		//
		KWIN_Item_Hide( hdlg , IDC_GRAPH_BUTTON );
		if ( !Get_Prm_SAVEFILE_MODE() ) KWIN_Item_Hide( hdlg , IDRETRY );
		// crkim // 2002.11.11
		if ( !Get_Prm_SCROLLBUTTON_MODE() ) {
			KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
			KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
		}
		KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
		KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
		// crkim // 2002.11.11
		//>>-----------------------------------------------------------
		MoveCenterWindow( hdlg );
		//
		//----------------------------------------------------------------------------------------------
		// 2003.09.16
		//----------------------------------------------------------------------------------------------
		if ( JOB_Get_Current_Count() <= ( (int) lParam ) ) {
			MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
			EndDialog( hdlg , FALSE );
			return FALSE;
		}
		if ( JOB_Get_Directory_Type( (int) lParam ) != 1 ) {
			MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed & Data not exist" , "Try Again" , MB_ICONHAND );
			EndDialog( hdlg , FALSE );
			return FALSE;
		}
		//
		JOB_Get_Directory( (int) lParam , Buffer3 );
		//
		//----------------------------------------------------------------------------------------------
		sprintf( Buffer , "%s/LotLog/%s/LotData.log" , Get_Prm_LOG_DIRECTORY() , Buffer3 );
		//----------------------------------------------------------------------------------------------
		if ( !LogView_Open_Lot_History_Data_Set( hdlg , IDC_LIST_LOGVIEW , 0 , 0 , Buffer , FileName , (int) lParam , FALSE ) ) {
			EndDialog( hdlg , TRUE );
			return TRUE;
		}
		//
		JOB_Get_Display_String( (int) lParam , Buffer3 );
		sprintf( Buffer , "%s [%s]" , Buffer3 , FileName );
		SetWindowText( hdlg , Buffer );
		return TRUE;
			
	case WM_DRAWITEM: // 2008.06.04
		//
		dis = (LPDRAWITEMSTRUCT) lParam;

		color_error = LogView_GetParam( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , dis->itemID );
		//
		ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , -1 , color_error % 10000 , 1 , "" , Buffer3 , 255 , 0 , 0 , -1 , -1 , -1 , -1 );
		//
		break;

	case WM_PAINT:
		hDC = BeginPaint( hdlg, &ps );
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDRETRY :
			if ( Log_Get_Save_File_Name( hdlg , Buffer ) ) {
				if ( !SAVE_FILE( FileName , Buffer , FALSE ) ) {
					MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] File Save Error!" , Buffer , MB_ICONHAND );
				}
			}
			return TRUE;
/// crkim
		case IDC_CTRL_SDOWN :
			i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
			j = ( ( rect.bottom - rect.top ) * i );
			ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , j );
			return TRUE;

		case IDC_CTRL_SUP :
			i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
			j = ( ( rect.bottom - rect.top ) * i );
			ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , -j );
			return TRUE;

		case IDC_CTRL_RIGHT :
			i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
			j = rect.right - rect.left;
			ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , 8*j , 0 );
			return TRUE;

		case IDC_CTRL_LEFT :
			i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
			j = rect.left - rect.right;
			ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , 8*j , 0 );
			return TRUE;

		case IDCANCEL :
			EndDialog( hdlg , TRUE );
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL LogView_Open_Process_Log_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName , int option_ftpStepLoad , int option2_ftpStep , int option3_ftpStart , int option4_ftpEnd , int option5_StepView , int option6 ) {
	int j , chkmode;
	int m , z1 , z2;

	char Buffer[4097];
	char Buffer2[256];
	double Dat;

	char TimeBuffer[256]; // 2016.08.19
	char TimeBufferLast[256]; // 2016.08.19
	double TimeRange; // 2016.08.19
	int TimeStepInX[MAX_X_TABLE]; // 2016.08.19
	BOOL TimeRes; // 2016.08.19
	double TimeGetDataCurr; // 2016.08.19
	double TimeGetData; // 2016.08.19

	double avr , var; // 2016.08.19

	BOOL bres;
	LV_COLUMN   lvColumn;
	LV_ITEM lvi;
	int encflag;
	int readline; // 2014.02.05
	//
	int  index;
	FILE *fpt;
	//
	DWORD Log_Size;
	//
	BOOL InsertItem; // 2016.03.25
	BOOL StepEnd; // 2016.08.19
	//

	int step_find; // 2018.04.24
	int step_f_ftellPos; // 2018.04.24
	int step_curr_ftellPos; // 2018.04.24
	//
	//-----------------------------------------------------------------
	//
	if ( !option_ftpStepLoad ) {
		strcpy( Step_Log_FileName , FileName ); // 2018.04.24
		for ( j = 0 ; j < MAX_STEP_COUNT ; j++ ) { // 2018.04.24
			Step_Log_StepStart[j] = -1;
			Step_Log_StepEnd[j] = -1;
			strcpy( Step_Log_StepName[j] , "?" );
		}
	}
	//
	//-----------------------------------------------------------------
	//
	if ( Process_Log_File_Is_Empty( FileName ) ) {
		sprintf( Buffer , "[%s] Data is Nothing" , FileName );
		MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error [3]", MB_ICONHAND );
		return FALSE;
	}
	//-----------------------------------------------------------------
	//
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) return FALSE; // 2017.05.19
	//
	//-----------------------------------------------------------------
	Log_Size = Process_Log_File_Get_Size( FileName );
	//
	sprintf( Buffer2 , "READING [%s] : %.2f Mb" , FileName , (double) Log_Size / 1024000.0 );
	LOG_OPEN_MESSAGE( Buffer2 );
	//
	/*
	// 2017.05.19
	//-----------------------------------------------------------------
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) { // 2005.03.22
//		sprintf( Buffer , "[%s] File Not Found" , FileName ); // 2017.05.18
//		MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error [4]", MB_ICONHAND ); // 2017.05.18
		return FALSE;
	}
	*/
	//
	for ( m = 0 ; m < MAX_X_TABLE ; m++ ) STR_9003_MINMAX_USE[m] = FALSE;
	//
	if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
	//
	xListView_Clear( hlistview );
	//
	if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
	//
	DATA_BUFFER_RESET( option_ftpStepLoad ? option2_ftpStep : 0 ); // 2016.08.19
	//
	PLOTTING_DATA_CLEAR();
	//
	PLOTTING_TITLE_SET( FileName );
	//
	index = 0;
	readline = 0;
	//
	m = 0;
	//
	encflag = 0; // 2011.12.07
	//
	strcpy( TimeBuffer , "" ); // 2016.08.19
	strcpy( TimeBufferLast , "" ); // 2016.08.19
	StepEnd = FALSE; // 2016.08.19
	//
	TimeRange = 0;
	//
	step_find = -1; // 2018.04.24
	step_f_ftellPos = -1; // 2018.04.24
	//
	do {
		//
		if ( LOG_OPEN_CANCELED() ) {
			fclose( fpt );
			return FALSE;
		}
		//
		step_curr_ftellPos = ftell( fpt ); // 2018.04.24
		//
		if ( option_ftpStepLoad ) { // 2018.04.24
			if ( step_curr_ftellPos >= option4_ftpEnd ) break;
		}
		//
		bres = Read_Line_From_File4( fpt , Buffer , 4096 );
		//
		if ( *Buffer != 0 ) {
			//
//			Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
			//
			if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
			//
			if ( index == 0 ) {
				//
//				ListView_DeleteAllItems( hlistview ); // 2014.02.13
				//
				lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
				lvColumn.fmt = LVCFMT_LEFT;
				//
				if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx = 45;
				else								lvColumn.cx = 55;
				//
				lvColumn.pszText = "No";
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
				//
				ListView_InsertColumn( hlistview , 0 , &lvColumn );
				//
				z1 = z2 = 0;
				//
				if ( *Buffer != 0 ) {
					//
					if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx = 140;
					else								lvColumn.cx = 200;
					lvColumn.pszText = "Time";
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 63 );
					z1 = z2;
					//
					if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
					//
					ListView_InsertColumn( hlistview , 1 , &lvColumn );
					//
					j = 2;
					//
					strcpy( TimeBufferLast , Buffer2 ); // 2016.08.19
					//
				}
				//
				STEP_CHECK_LOGDATA_MAKE_NAMEINDEX( Buffer + z1 ); // 2016.08.18
				//
				while( TRUE ) {
					//
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 63 );
					//
					if ( z1 != z2 ) {
						//
						z1 = z2;
						//
						if ( ( j - 2 ) < MAX_X_TABLE ) {
							//
							z2 = strlen( Buffer2 );
							//
							if ( Get_Prm_MINISCREEN_MODE() )	{
								if ( z2 <= 12 ) lvColumn.cx =  85;
								else            lvColumn.cx =  7 * z2;
							}
							else {
								if ( z2 <= 12 ) lvColumn.cx =  120;
								else            lvColumn.cx =  10 * z2;
							}
							//
							lvColumn.pszText = Buffer2;
							//
							if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
							//
							ListView_InsertColumn( hlistview , j , &lvColumn );
							//
							PLOTTING_ADD_ITEM( j - 2 , Buffer2 );
							//
						}
						//
						j++;
						//
					}
					else {
						break;
					}
					//
				}
				//
				PLOTTING_MAX_ITEM( j - 2 );
				m++;
				//
				//==============================================================================================================
				//
				// 2016.03.25
				//
				if ( option_ftpStepLoad ) {
					fseek( fpt , option3_ftpStart , SEEK_SET );
				}
				//
				//==============================================================================================================
				//
			}
			else {
				//
				if ( !option_ftpStepLoad ) { // 2018.04.24
					//
					if ( step_f_ftellPos == -1 ) {
						//
						step_f_ftellPos = step_curr_ftellPos;
						//
						if ( step_find != -1 ) {
							Step_Log_StepStart[step_find] = step_curr_ftellPos;
						}
					}
				}
				//
				sprintf( Buffer2 , "%d" , index );
//				lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
				lvi.mask  = LVIF_TEXT | LVIF_PARAM;
//				lvi.state = lvi.stateMask = 0;
				lvi.iItem		= index - 1;
				lvi.iSubItem	= 0;
				lvi.pszText     = Buffer2;
//				lvi.cchTextMax	= 100;
				lvi.cchTextMax	= 64;
//				lvi.iImage		= index - 1;
				//
				if ( Buffer[0] == '$' ) {
					//
					if ( index > 200000 ) { // 2014.02.12
						lvi.lParam		= ( ( index - 200001 + 10 ) * 10000 ) + 9000 + 8;
					}
					else {
						lvi.lParam		= ( ( index - 1 + 10 ) * 10000 ) + 9000 + 8;
					}
					//
					if ( option_ftpStepLoad ) { // 2016.03.25
						//
						InsertItem = FALSE;
						//
					}
					else {
						//
						if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
							fclose( fpt );
							//
							xListView_Leave();
							//
							return FALSE;
						}
						//
					}
					//
					//============================================================================================================
					// 2005.02.18
					//============================================================================================================
					strcpy( Buffer2 , "" );
					j = -1;
					//
					if ( ( Buffer[1] == 'S' ) && ( Buffer[2] == 'T' ) && ( Buffer[3] == 'E' ) && ( Buffer[4] == 'P' ) ) {
						z2 = -1;
						for ( z1 = 5 ; ; z1++ ) {
							if ( j == -1 ) {
								if      ( ( Buffer[z1] == ' ' ) || ( Buffer[z1] == 9 ) ) {
									if ( z2 != -1 ) {
										z2 = -1;
										break;
									}
								}
								else if ( Buffer[z1] == 'E' ) { // 2016.08.19
									z2 = ( Dat_Curr_Step + 2 );
									StepEnd = TRUE;
									break;
								}
								else if ( Buffer[z1] == '|' ) {
									j = 0;
								}
								else if ( Buffer[z1] == 0 ) {
									break;
								}
								else if ( ( Buffer[z1] >= '0' ) && ( Buffer[z1] <= '9' ) ) {
									if ( z2 < 0 ) z2 = 0;
									z2 = ( z2 * 10 ) + ( Buffer[z1] - '0' );
								}
								else {
									z2 = -1;
									break;
								}
							}
							else {
								if ( ( Buffer[z1] == '|' ) || ( Buffer[z1] == 0 ) ) {
									break;
								}
								else {
									Buffer2[j] = Buffer[z1];	j++;	Buffer2[j] = 0;
								}
							}
						}
						//
						if ( z2 == 1 ) { // 2016.08.19
							//
							if ( !StepEnd && !option_ftpStepLoad ) {
								//
								strcpy( TimeBuffer , "" );
								//
								DATA_BUFFER_RESET( 0 );
								//
								STEP_CHECK_CALDATA_RESET( TRUE , TimeBufferLast );
								//
							}
							//
						}
						//
						if ( z2 >= 1 ) {
							//
							if ( option_ftpStepLoad ) { // 2016.03.25
								//
								if ( StepEnd ) break; // 2016.08.19
								//
								if ( option2_ftpStep != (z2 - 1) ) break;
								//
							}
							//
							if ( Dat_Curr_Step != ( z2 - 1 ) ) strcpy( TimeBuffer , "" ); // 2016.08.19
							//
							Dat_Curr_Step = z2 - 1;
							//
							if ( StepEnd ) { // 2016.08.19
								//
								STEP_CHECK_LOGDATA_FIND_POST( TRUE , TimeBufferLast , index - 1 ); // 2016.08.19
								//
								strcpy( Buffer , "<STEP> POST" );
								//
							}
							else {
								//
								if ( Buffer2[0] == 0 ) {
									sprintf( Buffer , "<STEP> %d" , z2 );
								}
								else {
									//
									sprintf( Buffer , "<STEP> %d|%s" , z2 , Buffer2 );
									//
								}
								//
								if ( !option_ftpStepLoad ) { // 2018.04.24
									//
									if ( step_find != -1 ) {
										if ( Step_Log_StepStart[step_find] != -1 ) {
											Step_Log_StepEnd[step_find] = step_curr_ftellPos;
										}
										else {
											strcpy( Step_Log_StepName[step_find] , "?" );
										}
									}
									else {
										//
										if ( Dat_Curr_Step > 0 ) {
											//
											if ( step_f_ftellPos != -1 ) {
												if ( step_f_ftellPos < step_curr_ftellPos ) {
													Step_Log_StepStart[0] = step_f_ftellPos;
													Step_Log_StepEnd[0] = step_curr_ftellPos;
													strcpy( Step_Log_StepName[0] , "" );
												}
											}
											//
										}
										//
									}
									//
									step_find = Dat_Curr_Step;
									//
									strcpy( Step_Log_StepName[Dat_Curr_Step] , Buffer2 );
									//
								}
								//
							}
							//
							if ( option_ftpStepLoad && !InsertItem ) { // 2016.03.25
								//
								InsertItem = TRUE;
								//
								sprintf( Buffer2 , "%d" , index );
								lvi.pszText     = Buffer2;
								//
								if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
									fclose( fpt );
									//
									xListView_Leave();
									//
									return FALSE;
								}
								//
							}
							//
							ListView_SetItemText( hlistview , index - 1 , 1 , Buffer );
							//
						}
						else {
							//
							if ( option_ftpStepLoad ) { // 2016.08.19
								//
								if ( StepEnd ) break;
								//
							}
							//
							if ( option_ftpStepLoad && !InsertItem ) { // 2016.03.25
								//
								InsertItem = TRUE;
								//
								sprintf( Buffer2 , "%d" , index );
								lvi.pszText     = Buffer2;
								//
								if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
									fclose( fpt );
									//
									xListView_Leave();
									//
									return FALSE;
								}
								//
							}
							//
							ListView_SetItemText( hlistview , index - 1 , 1 , Buffer );
						}
						//
						//
						step_f_ftellPos = -1; // 2018.04.24
						//
						//
					}
					else {
						//
						if ( option_ftpStepLoad && !InsertItem ) { // 2016.03.25
							//
							InsertItem = TRUE;
							//
							sprintf( Buffer2 , "%d" , index );
							lvi.pszText     = Buffer2;
							//
							if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
								fclose( fpt );
								//
								xListView_Leave();
								//
								return FALSE;
							}
							//
						}
						//
						ListView_SetItemText( hlistview , index - 1 , 1 , Buffer );
					}
					//
					m++;
				}
				else {
					//
					if ( index > 200000 ) { // 2014.02.12
						lvi.lParam		= ( ( index - 200001 + 10 ) * 10000 ) + 9000 + 3;
					}
					else {
						lvi.lParam		= ( ( index - 1 + 10 ) * 10000 ) + 9000 + 3;
					}
					//
					if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
						fclose( fpt );
						xListView_Leave();
						return FALSE;
					}
					j = 1;
					//
					chkmode = 0; // 2008.02.28
					z1 = z2 = 0;
					//
					TimeRes = FALSE;
					//
					while ( j <= ( MAX_X_TABLE + 1 ) ) { // 2004.10.12
						//
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 63 );
						//
						if ( z1 != z2 ) {
							//
							if ( !StepEnd ) { // 2016.08.19
								if ( j == 2 ) {
									TimeRes = STEP_CHECK_LOGDATA_STEP_DATA( Dat_Curr_Step , TimeBuffer , TimeRange , Buffer + z1 , TimeStepInX , index - 1 );
								}
							}
							//
							z1 = z2;
							//
							if ( !StepEnd ) { // 2016.08.19
								//
								if ( j > 1 ) {
									//
									Dat = atof( Buffer2 );
									//
									Dat_Set_StepAll[ j - 2 ] = TRUE; // 2005.02.18
									//
									Dat_Set_Step[ Dat_Curr_Step ][ j - 2 ] = TRUE; // 2005.02.18
									//
									Dat_Set_StepUse[ Dat_Curr_Step ] = TRUE; // 2011.08.10
									//
									if ( ( chkmode == 0 ) || ( chkmode == 2 ) ) { // 2008.02.28
										//
										if ( !TimeRes || !TimeStepInX[j-2] ) {
											//
											if ( Dat_Count_Sum_StepAll[j-2] == 0 ) Dat_First_StepAll[ j - 2 ] = Dat;
											Dat_Last_StepAll[ j - 2 ] = Dat;
											//
											Dat_Sum_StepAll[ j - 2 ] = Dat_Sum_StepAll[ j - 2 ] + Dat; // 2005.02.18
											Dat_Sum2_StepAll[ j - 2 ] = Dat_Sum2_StepAll[ j - 2 ] + (Dat*Dat); // 2016.08.18
											//
											Dat_Count_Sum_StepAll[j-2]++;
											//
											//-----------------------------------------------------------
											//
											if ( Dat_Count_Sum_Step[ Dat_Curr_Step ][ j - 2 ] == 0 ) Dat_First_Step[ Dat_Curr_Step ][ j - 2 ] = Dat;
											Dat_Last_Step[ Dat_Curr_Step ][ j - 2 ] = Dat;
											//
											Dat_Sum_Step[ Dat_Curr_Step ][ j - 2 ] = Dat_Sum_Step[ Dat_Curr_Step ][ j - 2 ] + Dat; // 2005.02.18
											Dat_Sum2_Step[ Dat_Curr_Step ][ j - 2 ] = Dat_Sum2_Step[ Dat_Curr_Step ][ j - 2 ] + (Dat*Dat); // 2016.08.18
											//
											Dat_Count_Sum_Step[ Dat_Curr_Step ][ j - 2 ]++;
											//
										}
										//
									}
									if ( ( chkmode == 0 ) || ( chkmode == 1 ) ) {
										//
										if ( !TimeRes || !TimeStepInX[j-2] ) {
											//
											if ( Dat_Min_StepAll[ j - 2 ] > Dat ) Dat_Min_StepAll[ j - 2 ] = Dat;
											if ( Dat_Max_StepAll[ j - 2 ] < Dat ) Dat_Max_StepAll[ j - 2 ] = Dat;
											//
											Dat_Count_MinMax_StepAll[j-2]++;
											//
											//-----------------------------------------------------------
											//
											if ( Dat_Min_Step[ Dat_Curr_Step ][ j - 2 ] > Dat ) Dat_Min_Step[ Dat_Curr_Step ][ j - 2 ] = Dat;
											if ( Dat_Max_Step[ Dat_Curr_Step ][ j - 2 ] < Dat ) Dat_Max_Step[ Dat_Curr_Step ][ j - 2 ] = Dat;
											//
											Dat_Count_MinMax_Step[ Dat_Curr_Step ][j-2]++;
											//
										}
										//
									}
									//
									PLOTTING_ADD_DATA( j - 2 , Dat );
									//
								}
								else {
									//
									chkmode = Log_Get_String_ChkMode_from_Time( Buffer2 );
									//
									Log_Get_DevMode_from_Time( Buffer2 , &TimeGetDataCurr );
									//
									if ( TimeBuffer[0] == 0 ) {
										//
										TimeRange = 0;
										//
									}
									else {
										//
										TimeRange = TimeGetDataCurr - TimeGetData;
										//
										if ( TimeRange < 0 ) {
											TimeRange = ( TimeGetDataCurr + (double) ( 24 * 3600 ) ) - TimeGetData;
										}
										//
									}
									//
									TimeGetData = TimeGetDataCurr;
									//
									strcpy( TimeBuffer , Buffer2 );
									//
									//
									//
									PLOTTING_ADD_TIME( Buffer2 );
								}
								//
							}
							//
							if ( j == 1 ) {
								//
								strcpy( TimeBufferLast , Buffer2 ); // 2016.08.19
								//
							}
							//
							ListView_SetItemText( hlistview , index - 1 , j , Buffer2 );
							//
							j++;
						}
						else {
							break;
						}
					}
					//=========================================================================================
					if ( !StepEnd ) { // 2016.08.19
						//
						Dat_Count_Total++; // 2016.08.18
						Dat_Count_Total_Step[ Dat_Curr_Step ]++; // 2016.08.18
						//
					}
					else {
						STEP_CHECK_LOGDATA_FIND_POST( FALSE , TimeBufferLast , index - 1 ); // 2016.08.19
					}
					//=========================================================================================
				}
			}
			//
			if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
			//
			index++;
			//
			//=========================================================================================
			if ( sepload == 1 ) { // 2014.02.06
				//
				readline++;
				//
				if ( readline >= SEP_ONE_TIME_MAX_READ_ITEM_COUNT ) {
					//
					readline = 0;
					//
					sprintf( Buffer2 , "READING [%s] : %d Line(s) : %.2f Mb / %.2f Mb" , FileName , index , (double) ftell( fpt ) / 1024000.0 , (double) Log_Size / 1024000.0 );
					//
					LOG_OPEN_MESSAGE( Buffer2 );
					//
				}
			}
			//=========================================================================================
		}
	}
	while ( bres );
	//
	fclose( fpt );
	//
	STEP_CHECK_LOGDATA_END( TimeBufferLast ); // 2016.08.19
	//
	if ( !option_ftpStepLoad ) { // 2018.04.24
		//
		if ( step_find != -1 ) {
			if ( Step_Log_StepStart[step_find] != -1 ) {
				Step_Log_StepEnd[step_find] = step_curr_ftellPos;
			}
			else {
				strcpy( Step_Log_StepName[step_find] , "?" );
			}
		}
		else {
			if ( step_f_ftellPos != -1 ) {
				if ( step_f_ftellPos < step_curr_ftellPos ) {
					Step_Log_StepStart[0] = step_f_ftellPos;
					Step_Log_StepEnd[0] = step_curr_ftellPos;
					strcpy( Step_Log_StepName[0] , "" );
				}
			}
			//
		}
		//
	}
	//
	if ( Dat_Count_Total > 0 ) { // 2008.02.28
		//
		if ( !xListView_Enter() ) {	return FALSE;	}
		//
		if ( ( Get_Prm_DISPLAY_MODE() > 0 ) || ( Get_Prm_SIMPLE_DISPLAY() == 1 ) ) {
			// 2011.08.10
			//
			if ( !option_ftpStepLoad ) { // 2016.03.25
				//
				z1 = 0;
				for ( m = 0 ; m < MAX_STEP_COUNT ; m++ ) {
					if ( Dat_Set_StepUse[m] ) z1++;
				}
				//
				if ( z1 > 1 ) {
					//
					for ( m = ( MAX_STEP_COUNT - 1 ) ; m >= 0 ; m-- ) {
						//
						if ( !Dat_Set_StepUse[m] ) continue;
						//
						lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
						lvi.state = lvi.stateMask = 0;
						lvi.iSubItem	= 0;
						lvi.cchTextMax	= 100;
						lvi.lParam		= ( 4 * 10000 ) + 9000 + 0;
						lvi.iItem		= 0;
						lvi.iImage		= 0;
						lvi.pszText     = "";
						//
						if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
							xListView_Leave();
							return FALSE;
						}
						//
						STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
						//
						sprintf( Buffer2 , "S%d" , m + 1 );
						lvi.lParam		= ( 3 * 10000 ) + 9000 + 8; //  2016.08.19
						lvi.pszText     = Buffer2;
						if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
							xListView_Leave();
							return FALSE;
						}
						//
						STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
						//
						lvi.lParam		= ( 2 * 10000 ) + 9000 + 8;
						lvi.pszText     = Buffer2;
						if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
							xListView_Leave();
							return FALSE;
						}
						//
						STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
						//
						lvi.lParam		= ( 1 * 10000 ) + 9000 + 8;
						lvi.pszText     = Buffer2;
						if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
							xListView_Leave();
							return FALSE;
						}
						//
						STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
						//
						lvi.lParam		= ( 0 * 10000 ) + 9000 + 8;
						lvi.pszText     = Buffer2;
						if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
							xListView_Leave();
							return FALSE;
						}
						//
						STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
						//
						j = 1;
						ListView_SetItemText( hlistview , 0 , j , "AVERAGE" );
						ListView_SetItemText( hlistview , 1 , j , "MINIMUM" );
						ListView_SetItemText( hlistview , 2 , j , "MAXIMUM" );
						ListView_SetItemText( hlistview , 3 , j , "STD DEV" ); // 2016.08.19
						j = 2;
						//
						while( ( Dat_Set_Step[m][ j - 2 ] ) && ( j <= ( MAX_X_TABLE + 1 ) ) ) {
							if ( Dat_Count_Sum_Step[m][j-2] <= 0 ) {
								strcpy( Buffer2 , "N/A" );
								ListView_SetItemText( hlistview , 0 , j , Buffer2 );
								ListView_SetItemText( hlistview , 3 , j , Buffer2 ); // 2016.08.19
							}
							else {
								//
//								sprintf( Buffer2 , "%.3lf" , ( Dat_Sum_Step[m][ j - 2 ] ) / Dat_Count_Sum_Step[m][j-2] ); // 2016.08.19
//								ListView_SetItemText( hlistview , 0 , j , Buffer2 ); // 2016.08.19
								//
								STEP_Get_AVR_VAR_Value( Dat_Count_Sum_Step[m][j-2] , Dat_Sum_Step[m][ j - 2 ] , Dat_Sum2_Step[m][ j - 2 ] , &avr , &var ); // 2016.08.19
								//
								sprintf( Buffer2 , "%.3lf" , avr ); // 2016.08.19
								ListView_SetItemText( hlistview , 0 , j , Buffer2 ); // 2016.08.19
								//
								sprintf( Buffer2 , "%.3lf" , var ); // 2016.08.19
								ListView_SetItemText( hlistview , 3 , j , Buffer2 ); // 2016.08.19
							}
							if ( Dat_Count_MinMax_Step[m][j-2] <= 0 ) {
								strcpy( Buffer2 , "N/A" );
								ListView_SetItemText( hlistview , 1 , j , Buffer2 );
								ListView_SetItemText( hlistview , 2 , j , Buffer2 );
							}
							else {
								//
								sprintf( Buffer2 , "%.3lf" ,   Dat_Min_Step[m][ j - 2 ] );
								//
								ListView_SetItemText( hlistview , 1 , j , Buffer2 );
								//
								sprintf( Buffer2 , "%.3lf" ,   Dat_Max_Step[m][ j - 2 ] );
								//
								ListView_SetItemText( hlistview , 2 , j , Buffer2 );
								//
							}
							//
							j++;
						}
					}
					//
				}
			}
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iSubItem	= 0;
			lvi.cchTextMax	= 100;
			lvi.lParam		= ( 4 * 10000 ) + 9000 + 0;
			lvi.iItem		= 0;
			lvi.iImage		= 0;
			lvi.pszText     = "";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			lvi.lParam		= ( 3 * 10000 ) + 9000 + 1; // 2016.08.19
			if ( z1 > 1 ) lvi.pszText     = "All";	else lvi.pszText     = "STD";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			lvi.lParam		= ( 2 * 10000 ) + 9000 + 1;
			if ( z1 > 1 ) lvi.pszText     = "All";	else lvi.pszText     = "MAX";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			lvi.lParam		= ( 1 * 10000 ) + 9000 + 1;
			if ( z1 > 1 ) lvi.pszText     = "All";	else lvi.pszText     = "MIN";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			lvi.lParam		= ( 0 * 10000 ) + 9000 + 1;
			if ( z1 > 1 ) lvi.pszText     = "All";	else lvi.pszText     = "AVR";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			j = 1;
			ListView_SetItemText( hlistview , 0 , j , "AVERAGE" );
			ListView_SetItemText( hlistview , 1 , j , "MINIMUM" );
			ListView_SetItemText( hlistview , 2 , j , "MAXIMUM" );
			ListView_SetItemText( hlistview , 3 , j , "STD DEV" ); // 2016.08.19
			j = 2;
			while( ( Dat_Set_StepAll[ j - 2 ] ) && ( j <= ( MAX_X_TABLE + 1 ) ) ) { // 2004.10.12 // 2005.02.18
				//
				if ( Dat_Count_Sum_StepAll[j-2] <= 0 ) {
					strcpy( Buffer2 , "N/A" );
					ListView_SetItemText( hlistview , 0 , j , Buffer2 );
					ListView_SetItemText( hlistview , 3 , j , Buffer2 ); // 2016.08.19
				}
				else {
//					sprintf( Buffer2 , "%.3lf" , ( Dat_Sum_StepAll[ j - 2 ] ) / Dat_Count_Sum_StepAll[j-2] ); // 2005.02.18 // 2016.08.19
//					ListView_SetItemText( hlistview , 0 , j , Buffer2 ); // 2016.08.19
					//
					STEP_Get_AVR_VAR_Value( Dat_Count_Sum_StepAll[j-2] , Dat_Sum_StepAll[ j - 2 ] , Dat_Sum2_StepAll[ j - 2 ] , &avr , &var ); // 2016.08.19
					//
					sprintf( Buffer2 , "%.3lf" , avr ); // 2016.08.19
					ListView_SetItemText( hlistview , 0 , j , Buffer2 ); // 2016.08.19
					//
					sprintf( Buffer2 , "%.3lf" , var ); // 2016.08.19
					ListView_SetItemText( hlistview , 3 , j , Buffer2 ); // 2016.08.19

				}
				if ( Dat_Count_MinMax_StepAll[j-2] <= 0 ) {
					strcpy( Buffer2 , "N/A" );
					ListView_SetItemText( hlistview , 1 , j , Buffer2 );
					ListView_SetItemText( hlistview , 2 , j , Buffer2 );
				}
				else {
					//
					STR_9003_MINMAX_USE[j-2] = FALSE; // 2011.07.18
					//
					sprintf( Buffer2 , "%.3lf" ,   Dat_Min_StepAll[ j - 2 ] ); // 2005.02.18
					//
					STR_9003_MIN[j-2] = Dat_Min_StepAll[ j - 2 ]; // 2011.07.18
					//
					ListView_SetItemText( hlistview , 1 , j , Buffer2 );
					//
					sprintf( Buffer2 , "%.3lf" ,   Dat_Max_StepAll[ j - 2 ] ); // 2005.02.18
					//
					STR_9003_MAX[j-2] = Dat_Max_StepAll[ j - 2 ]; // 2011.07.18
					//
					ListView_SetItemText( hlistview , 2 , j , Buffer2 );
					//
					STR_9003_MINMAX_USE[j-2] = TRUE; // 2011.07.18
					//
				}
				//
				j++;
			}
		}
		else {
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iSubItem	= 0;
			lvi.cchTextMax	= 100;
			if ( index > 200000 ) { // 2014.02.12
				lvi.lParam		= ( ( index - 200000 + 0 + 10 ) * 10000 ) + 9000 + 0;
			}
			else {
				lvi.lParam		= ( ( index + 0 + 10 ) * 10000 ) + 9000 + 0;
			}
			lvi.iItem		= index - 1;
			lvi.iImage		= index - 1;
			lvi.pszText     = "";
			//
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			if ( index > 200000 ) { // 2014.02.12
				lvi.lParam		= ( ( index - 200000 + 1 + 10 ) * 10000 ) + 9000 + 1;
			}
			else {
				lvi.lParam		= ( ( index + 1 + 10 ) * 10000 ) + 9000 + 1;
			}
			lvi.iItem		= index;
			lvi.iImage		= index;
			lvi.pszText     = "AVR";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			if ( index > 200000 ) { // 2014.02.12
				lvi.lParam		= ( ( index - 200000 + 2 + 10 ) * 10000 ) + 9000 + 1;
			}
			else {
				lvi.lParam		= ( ( index + 2 + 10 ) * 10000 ) + 9000 + 1;
			}
			lvi.iItem		= index + 1;
			lvi.iImage		= index + 1;
			lvi.pszText     = "MIN";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			if ( index > 200000 ) { // 2014.02.12
				lvi.lParam		= ( ( index - 200000 + 3 + 10 ) * 10000 ) + 9000 + 1;
			}
			else {
				lvi.lParam		= ( ( index + 3 + 10 ) * 10000 ) + 9000 + 1;
			}
			lvi.iItem		= index + 2;
			lvi.iImage		= index + 2;
			lvi.pszText     = "MAX";
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				xListView_Leave();
				return FALSE;
			}
			//
			STEP_CHECK_LOGDATA_INC_LV_GARBAGE(); // 2016.08.19
			//
			j = 1;
			ListView_SetItemText( hlistview , index     , j , "AVERAGE" );
			ListView_SetItemText( hlistview , index + 1 , j , "MINIMUM" );
			ListView_SetItemText( hlistview , index + 2 , j , "MAXIMUM" );
			j = 2;
			while( ( Dat_Set_StepAll[ j - 2 ] ) && ( j <= ( MAX_X_TABLE + 1 ) ) ) { // 2004.10.12 // 2005.02.18
				if ( Dat_Count_Sum_StepAll[j-2] <= 0 ) {
					strcpy( Buffer2 , "N/A" );
					ListView_SetItemText( hlistview , index     , j , Buffer2 );
				}
				else {
					sprintf( Buffer2 , "%.3lf" , ( Dat_Sum_StepAll[ j - 2 ] ) / Dat_Count_Sum_StepAll[j-2] ); // 2005.02.18
					ListView_SetItemText( hlistview , index     , j , Buffer2 );
				}
				if ( Dat_Count_MinMax_StepAll[j-2] <= 0 ) {
					strcpy( Buffer2 , "N/A" );
					ListView_SetItemText( hlistview , index + 1 , j , Buffer2 );
					ListView_SetItemText( hlistview , index + 2 , j , Buffer2 );
				}
				else {
					sprintf( Buffer2 , "%.3lf" ,   Dat_Min_StepAll[ j - 2 ] ); // 2005.02.18
					ListView_SetItemText( hlistview , index + 1 , j , Buffer2 );
					sprintf( Buffer2 , "%.3lf" ,   Dat_Max_StepAll[ j - 2 ] ); // 2005.02.18
					ListView_SetItemText( hlistview , index + 2 , j , Buffer2 );
				}
				//
				j++;
			}
		}
		//
		if ( !xListView_Leave() ) {	return FALSE;	}
		//
	}
	//
	STEP_CHECK_DISPLAY();
	//
	return TRUE;
}
//
BOOL LogView_Open_Process_Log_Data_Set( HWND hdlg , int winpos , char *FileName_org , char *FileName , BOOL fptload , int fptstep , int fptdataS , int fptdataE , BOOL sepload , BOOL StepView ) {
	//-----------------------------------------------------------------
	BOOL Res;
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_Start( hdlg );
	//-----------------------------------------------------------------
	//
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else {
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
	}
	//
	Res = LOG_OPEN_CONTROL( hdlg , GetDlgItem( hdlg , winpos ) , sepload , FileName , fptload , fptstep , fptdataS , fptdataE , StepView , 0 , LogView_Open_Process_Log_Data_Set_Callback );
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_End( hdlg );
	//-----------------------------------------------------------------
	return Res;
	//-----------------------------------------------------------------
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY LogView_Open_Process_Log_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int j , k;
//	int chkmode;
//	int i , m;
//	int z1 , z2;
	static char FileName[256];
//	char Buffer[1025]; // 2007.11.21
	char Buffer[4097]; // 2007.11.21
	char Buffer2[64];  // 2009.11.20
//	char Buffer2[256]; // 2009.11.20
//	char MethodString[32];
//	double Dat;

	char folderstr[256];

	RECT rect;		// crkim

//	BOOL bres;
	FILE *fpt;
//	LV_COLUMN   lvColumn;
//	LV_ITEM lvi;
	ViewStepTemplate *readdata;

	LPDRAWITEMSTRUCT dis; // 2015.07.29
	LPMEASUREITEMSTRUCT dim; // 2015.07.29

	//
	KGUI_STANDARD_Check_Enable_Monitoring_Function( hdlg ); // 2018.01.10
	//
	switch ( msg ) {
		case WM_MEASUREITEM: // 2015.07.29
			//
			if ( ( GLISTVIEW_HEIGHT > 0 ) && ( ( GLISTVIEW_HEIGHT % 1000 ) >= 10 ) ) {
				//
				dim = (LPMEASUREITEMSTRUCT) lParam;
				//
				if ( dim->CtlType == ODT_LISTVIEW ) {
					if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
						 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
					}
				}
				//
			}
			else {
				if ( LISTVIEW_HEIGHT_UPDATE ) {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
							 dim->itemHeight = LISTVIEW_HEIGHT;
						}
					}
					//
				}
				//
			}
			return TRUE;

		case WM_DRAWITEM: // 2015.07.29
			//
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
			ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , -1 , 0 , 1 , "" , Buffer , 4096 , 0 , 0 , -1 , -1 , -1 , -1 );
			//
			break;

		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			readdata = (ViewStepTemplate *) (LPARAM) lParam;

			KWIN_Item_Show( hdlg , IDC_GRAPH_BUTTON );
			if ( !Get_Prm_SAVEFILE_MODE() ) KWIN_Item_Hide( hdlg , IDRETRY );
			// crkim // 2002.11.11
			if ( !Get_Prm_SCROLLBUTTON_MODE() ) {
				KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
			}
			// crkim // 2002.11.11
			//>>-----------------------------------------------------------
			MoveCenterWindow( hdlg );
			//----------------------------------------------------------------------------------------------
			// 2003.09.16
			//----------------------------------------------------------------------------------------------
			if ( readdata->mode > 0 ) { // 2004.01.06
				if ( JOB_Get_Current_Count() <= readdata->index ) {
					MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
					EndDialog( hdlg , FALSE );
					return FALSE;
				}
				if ( JOB_Get_Directory_Type( readdata->index ) != 1 ) {
					MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed & Data not exist" , "Try Again" , MB_ICONHAND );
					EndDialog( hdlg , FALSE );
					return FALSE;
				}
			}
			//----------------------------------------------------------------------------------------------
			//
			if ( readdata->mode <= 0 ) {
				sprintf( Buffer , "%s" , readdata->str );
			}
			else {
				//
				JOB_Get_Directory( readdata->index , folderstr );
				//
				Get_Log_File_Name( Buffer , "PROCESS" , readdata->mode - 1 , folderstr , FALSE , 0 , 0 , readdata->slot , 0 , readdata->chstr , "" );
			}
			//----------------------------------------------------------------------------------------------
			if ( !LogView_Open_Process_Log_Data_Set( hdlg , IDC_LIST_LOGVIEW , Buffer , FileName , FALSE , 0 , 0 , 0 , FALSE , FALSE ) ) {
				EndDialog( hdlg , TRUE );
				return TRUE;
			}
			//
			if ( readdata->mode <= 0 ) {
				sprintf( Buffer , "[%s]" , FileName );
				SetWindowText( hdlg , Buffer );
			}
			else {
				JOB_Get_Display_String( readdata->index , Buffer2 );
				sprintf( Buffer , "[%s] %s [%s]" , readdata->str , Buffer2 , FileName );
				SetWindowText( hdlg , Buffer );
			}
			//
			return TRUE;
			
		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
			case IDC_GRAPH_BUTTON :
				//
				if ( !COMP_HAS_PLOT_FUNC() ) {
					GoModalDialogBoxParam( GETHINST( hdlg ) , KWIN_MAKEINTRESOURCE( IDD_PLOTTING_PAD , IDD_E_PLOTTING_PAD ) , hdlg , LogView_Open_Plotting_Proc , 0 ); // 2004.01.19 // 2004.08.10
				}
				else {
					sprintf( Buffer , "TABLE_USER_APPEND1 %d|PROCESSLOG|%s||%s|||||" , (int) hdlg , FileName , FileName );
					//
					COMP_CALL_PLOT_FUNC( hdlg , Buffer );
					//
				}
				return TRUE;
/// crkim
			case IDC_CTRL_SDOWN :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * k );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , j );
				return TRUE;

			case IDC_CTRL_SUP :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * k );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , -j );
				return TRUE;

			case IDC_CTRL_RIGHT :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.right - rect.left;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;

			case IDC_CTRL_LEFT :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.left - rect.right;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;
/// crkim
			case IDRETRY :
				if ( Log_Get_Save_File_Name( hdlg , Buffer ) ) {
					if ( !SAVE_FILE( FileName , Buffer , FALSE ) ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] File Save Error!" , Buffer , MB_ICONHAND );
						return TRUE;
					}
					if ( Dat_Count_Total > 0 ) {
						fpt = fopen( Buffer , "a" );
						if ( fpt != NULL ) {
							fprintf( fpt , "\n" );
							//
							fprintf( fpt , "AVR" );
							j = 0;
							while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12 // 2005.02.18
								if ( Dat_Count_Sum_StepAll[j] <= 0 ) {
									fprintf( fpt , "\tN/A" );
								}
								else {
									fprintf( fpt , "\t%.3lf" , ( Dat_Sum_StepAll[ j ] ) / Dat_Count_Sum_StepAll[j] ); // 2005.02.18
								}
								j++;
							}
							fprintf( fpt , "\n" );
							//
							fprintf( fpt , "MIN" );
							j = 0;
							while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12 // 2005.02.18
								if ( Dat_Count_MinMax_StepAll[ j ] <= 0 ) {
									fprintf( fpt , "\tN/A" );
								}
								else {
									fprintf( fpt , "\t%.3lf" ,   Dat_Min_StepAll[ j ] ); // 2005.02.18
								}
								j++;
							}
							fprintf( fpt , "\n" );
							//
							fprintf( fpt , "MAX" );
							j = 0;
							while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12 // 2005.02.18
								if ( Dat_Count_MinMax_StepAll[ j ] <= 0 ) {
									fprintf( fpt , "\tN/A" );
								}
								else {
									fprintf( fpt , "\t%.3lf" ,   Dat_Max_StepAll[ j ] ); // 2005.02.18
								}
								j++;
							}
							fprintf( fpt , "\n" );
							fclose( fpt );
						}
					}
				}
				return TRUE;

			case IDCANCEL :
				EndDialog( hdlg , TRUE );
				return TRUE;

			}
			return TRUE;

	}
	return FALSE;
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

BOOL LogView_Open_LotPM_Log_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName0 , int mode , int index , int option3 , int option4 , int option5 , int option6 ) {
	int j , k , chkmode;
	int i , z1 , z2 , titlesetcount , titlecount , itemsetcount , sktag , titletag;
	char FileName[256];
	char FileName_org[256];
	char Buffer[4097]; // 2007.11.21
	char Buffer2[256];

	char folderstr[256];

	double Dat;

	BOOL bres;
	FILE *fpt;
	LV_COLUMN   lvColumn;
	LV_ITEM lvi;

	int  WfrchkPM[100]; // 2007.08.21
	int  WfrchkSlot1[100]; // 2007.08.21
	int  WfrchkSlot2[100]; // 2007.08.21
	char WfrchkBuf1[32]; // 2007.08.21
	char WfrchkBuf2[32]; // 2007.08.21
	int  WfrchkCount , WfrchkRead , wpm , wp1 , wp2; // 2007.08.21

	char WfrTitle[MAX_X_TABLE][64];
	int encflag;
	//
	int readline;
	DWORD Log_Size;

	//----------------------------------------------------------------------------------------------
	if ( JOB_Get_Current_Count() <= index ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
		return FALSE;
	}
	if ( JOB_Get_Directory_Type( index ) != 1 ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Item Count Changed & Data not exist" , "Try Again" , MB_ICONHAND );
		return FALSE;
	}
	//----------------------------------------------------------------------------------------------
	//
	JOB_Get_Directory( index , folderstr );
	//
	sprintf( FileName_org , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , folderstr , METHOD_LOG_FILE );
	//
	//----------------------------------------------------------------------------------------------
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else {
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
	}
	//-----------------------------------------------------------------
	if ( ( fpt = fopen( FileName, "r" ) ) == NULL ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Method Information is not exist" , "ERROR" , MB_ICONHAND );
		return FALSE;
	}
	//
	sprintf( Buffer2 , "READING [%s]" , FileName );
	LOG_OPEN_MESSAGE( Buffer2 );
	//
	//-----------------------------------------------------------------
	WfrchkCount = 0;
	WfrchkRead = 0;
	//-----------------------------------------------------------------
	encflag = 0; // 2011.12.07
	//
	do {
		//
//		bres = Read_Line_From_File4( fpt , Buffer , 255 ); // 2014.02.27
		bres = Read_Line_From_File4( fpt , Buffer , 4096 ); // 2014.02.27
		//
		if ( *Buffer != 0 ) {
			//
//			Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
			//
			z1 = 0;
			z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
			if ( strlen( Buffer2 ) <= 0 ) {
				z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
				//------------------------------------------------------------------------------------
				STR_SEPERATE_CHAR( Buffer2 , '_' , WfrchkBuf1 , WfrchkBuf2 , 32 );
				//------------------------------------------------------------------------------------
				if ( WfrchkBuf1[0] == 'A' ) { // 2007.11.07
					wp1 = atoi( WfrchkBuf1 + 1 ) + 100;
				}
				else {
					wp1 = atoi( WfrchkBuf1 );
				}
				//
				if ( WfrchkBuf2[0] == 'D' ) { // 2007.11.07
					wp2 = atoi( WfrchkBuf2 + 1 ) + 100;
				}
				else {
					wp2 = atoi( WfrchkBuf2 );
				}
				//------------------------------------------------------------------------------------
				if ( wp1 > 0 ) {
					z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
					// 2013.12.05
					if      ( ( Buffer2[0] == 'P' ) && ( Buffer2[1] == 'M' ) && ( Buffer2[2] >= '0' ) && ( Buffer2[2] <= '9' ) ) {
						wpm = atoi( Buffer2 + 2 );
					}
					else if ( ( Buffer2[0] == 'P' ) && ( Buffer2[1] >= '0' ) && ( Buffer2[1] <= '9' ) ) {
						wpm = atoi( Buffer2 + 1 );
					}
					else {
						wpm = atoi( Buffer2 );
					}
					//------------------------------------------------------------------------------------
					if ( WfrchkCount < 100 ) {
						if ( ( wpm > 0 ) && ( wpm <= MAX_PM_CHAMBER_DEPTH ) ) {
							for ( k = 0 ; k < WfrchkCount ; k++ ) {
								if ( ( WfrchkPM[k] == wpm ) && ( WfrchkSlot1[k] == wp1 ) && ( WfrchkSlot2[k] == wp2 ) ) break;
							}
							if ( k == WfrchkCount ) {
								WfrchkPM[WfrchkCount] = wpm;
								WfrchkSlot1[WfrchkCount] = wp1;
								WfrchkSlot2[WfrchkCount] = wp2;
								WfrchkCount++;
							}
						}
					}
				}
				//------------------------------------------------------------------------------------
			}
		}
	}
	while ( bres );
	//
	fclose( fpt );
	//
	if ( WfrchkCount <= 0 ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Method Information is not exist" , "ERROR" , MB_ICONHAND );
		return FALSE;
	}
	//
	if ( !xListView_Enter() ) {	return FALSE; }
	//
	xListView_Clear( hlistview );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//
	if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx = 70;
	else								lvColumn.cx = 100;
	//
	lvColumn.pszText = "Module";
	//
	if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
	//
	ListView_InsertColumn( hlistview , 0 , &lvColumn );
	//
	if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx = 77;
	else								lvColumn.cx = 110;
	//
	lvColumn.pszText = "";
	//
	if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
	//
	ListView_InsertColumn( hlistview , 1 , &lvColumn );
	//
	if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx = 77;
	else								lvColumn.cx = 110;
	//
	lvColumn.pszText = "";
	//
	if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
	//
	ListView_InsertColumn( hlistview , 2 , &lvColumn );
	//
	if ( !xListView_Leave() ) {	return FALSE; }
	//
	titlesetcount = 0;
	itemsetcount = 0;
	//
	readline = 0;
	//
	sktag = FALSE;
	//----------------------------------------------------------------------------------------------
	for ( wpm = 0 ; wpm < MAX_PM_CHAMBER_DEPTH ; wpm++ ) {
		//===========================================================================
		titletag = FALSE;
		titlecount = 0;
		//===========================================================================
		DATA_BUFFER_RESET( 0 ); // 2016.08.19
		//===========================================================================
		for ( j = 0 ; j < MAX_X_TABLE ; j++ ) strcpy( WfrTitle[j] , "" );
		//===========================================================================
		for ( wp1 = 0 ; wp1 < WfrchkCount ; wp1++ ) {
			//
			if ( WfrchkPM[wp1] == ( wpm + 1 ) ) {
				//
				WfrchkRead++;
				//
				//============================================================================================================================================
				Get_Log_File_Name( FileName_org , "PROCESS" , mode , folderstr , FALSE , WfrchkSlot1[wp1] , WfrchkSlot2[wp1] , NULL , WfrchkPM[wp1] , NULL , "" );
				//============================================================================================================================================
				if ( !Get_Prm_NETMONITOR_MODE() ) {
					strcpy( FileName , FileName_org );
				}
				else {
					sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
					_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
					_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
				}
				//-----------------------------------------------------------------
				if ( Process_Log_File_Is_Empty( FileName ) ) {
					if ( !sktag ) {
						sprintf( Buffer , "[%s] Data is Nothing. Continue without this file or Abort?\n(YES=Skip(All)/No=Skip(One File)/Cancel=Abort)" , FileName );
						switch( MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error(YES=Skip(All)/No=Skip(One File)/Cancel=Abort)" , MB_ICONQUESTION | MB_YESNOCANCEL ) ) {
						case IDYES :
							sktag = TRUE;
							break;
						case IDNO :
							break;
						default :
							return FALSE;
							break;
						}
					}
				}
				else {
					//-----------------------------------------------------------------
					Log_Size = Process_Log_File_Get_Size( FileName );
					//
					readline = 0;
					//-----------------------------------------------------------------
					if ( ( fpt = fopen( FileName, "r" ) ) == NULL ) {
						if ( !sktag ) {
							sprintf( Buffer , "[%s] File Not Found. Continue without this file or Abort?\n(YES=Skip(All)/No=Skip(One File)/Cancel=Abort)" , FileName );
							switch( MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error(YES=Skip(All)/No=Skip(One File)/Cancel=Abort)" , MB_ICONQUESTION | MB_YESNOCANCEL ) ) {
							case IDYES :
								sktag = TRUE;
								break;
							case IDNO :
								break;
							default :
								return FALSE;
								break;
							}
						}
					}
					else {
						//
						//-----------------------------------------------------------------
						Dat_Curr_Step = 0;
						//-----------------------------------------------------------------
						i = 0;
						//
						encflag = 0; // 2011.12.07
						//
						do {
							//
							if ( LOG_OPEN_CANCELED() ) {
								fclose( fpt );
								return FALSE;
							}
							//
							bres = Read_Line_From_File4( fpt , Buffer , 4096 );
							if ( *Buffer != 0 ) {
								//
//								Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
								Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
								//
								if ( i == 0 ) {
									if ( !titletag ) {
										//-----------------------------------------------------------------------------
										titletag = TRUE;
										titlecount = 0;
										//-----------------------------------------------------------------------------
										z1 = 0;
										z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 63 );
										z1 = z2;
										//-----------------------------------------------------------------------------
										while( TRUE ) {
											z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 63 );
											if ( z1 != z2 ) {
												z1 = z2;
												if ( titlecount < MAX_X_TABLE ) { // 2007.11.21
													strncpy( WfrTitle[titlecount] , Buffer2 , 63 );
													titlecount++;

												}
											}
											else {
												break;
											}
										}
									}
								}
								else {
									if ( Buffer[0] == '$' ) {
										//============================================================================================================
										if ( ( Buffer[1] == 'S' ) && ( Buffer[2] == 'T' ) && ( Buffer[3] == 'E' ) && ( Buffer[4] == 'P' ) ) {
											z2 = -1;
											for ( z1 = 5 ; ; z1++ ) {
												if      ( ( Buffer[z1] == ' ' ) || ( Buffer[z1] == 9 ) ) {
													if ( z2 != -1 ) {
														z2 = -1;
														break;
													}
												}
												else if ( ( Buffer[z1] == '|' ) || ( Buffer[z1] == 0 ) ) {
													break;
												}
												else if ( ( Buffer[z1] >= '0' ) && ( Buffer[z1] <= '9' ) ) {
													if ( z2 < 0 ) z2 = 0;
													z2 = ( z2 * 10 ) + ( Buffer[z1] - '0' );
												}
												else {
													z2 = -1;
													break;
												}
											}
											if ( z2 >= 1 ) {
												Dat_Curr_Step = z2 - 1;
											}
										}
										//============================================================================================================
									}
									else {
										z1 = z2 = j = 0;
										//
										z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 63 );
										//
										z1 = z2;
										//
										chkmode = Log_Get_String_ChkMode_from_Time( Buffer2 ); // 2008.02.28
										//
										while ( j < titlecount ) {
											z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 63 );
											if ( z1 != z2 ) {
												z1 = z2;
												Dat = atof( Buffer2 );

												Dat_Set_Step[ Dat_Curr_Step ][ j ] = TRUE;
												//
												Dat_Set_StepUse[ Dat_Curr_Step ] = TRUE; // 2011.08.10
												//
												if ( ( chkmode == 0 ) || ( chkmode == 2 ) ) { // 2008.02.28
													//
													if ( Dat_Count_Sum_Step[ Dat_Curr_Step ][j] == 0 ) Dat_First_Step[ Dat_Curr_Step ][ j ] = Dat;
													Dat_Last_Step[ Dat_Curr_Step ][ j ] = Dat;
													//
													Dat_Sum_Step[ Dat_Curr_Step ][ j ] = Dat_Sum_Step[ Dat_Curr_Step ][ j ] + Dat;
													Dat_Sum2_Step[ Dat_Curr_Step ][ j ] = Dat_Sum2_Step[ Dat_Curr_Step ][ j ] + (Dat*Dat);
													//
													Dat_Count_Sum_Step[ Dat_Curr_Step ][ j ]++;
													//
												}
												if ( ( chkmode == 0 ) || ( chkmode == 1 ) ) { // 2008.02.28
													if ( Dat_Min_Step[ Dat_Curr_Step ][ j ] > Dat ) Dat_Min_Step[ Dat_Curr_Step ][ j ] = Dat;
													if ( Dat_Max_Step[ Dat_Curr_Step ][ j ] < Dat ) Dat_Max_Step[ Dat_Curr_Step ][ j ] = Dat;
													//
													Dat_Count_MinMax_Step[ Dat_Curr_Step ][j]++;
													//
												}
												//
												j++;
											}
											else {
												break;
											}
										}
										//=========================================================================================
										Dat_Count_Total++; // 2016.08.18
										Dat_Count_Total_Step[ Dat_Curr_Step ]++; // 2016.08.18
										//=========================================================================================
									}
								}
								//
								i++;
								//
								//=========================================================================================
								if ( sepload == 1 ) { // 2014.02.06
									//
									readline++;
									//
									if ( readline >= SEP_ONE_TIME_MAX_READ_ITEM_COUNT ) {
										//
										readline = 0;
										//
										sprintf( Buffer2 , "READING [%s] : %d Line(s) : %.2f Mb / %.2f Mb : File [%d]/[%d]" , FileName , i , (double) ftell( fpt ) / 1024000.0 , (double) Log_Size / 1024000.0 , WfrchkRead , WfrchkCount );
										//
										LOG_OPEN_MESSAGE( Buffer2 );
										//
									}
								}
								//=========================================================================================
							}
						}
						while ( bres );
						//
						fclose( fpt );
						//
					}
				}
			}
		}
		//==================================================================================================================================
		if ( Dat_Count_Total > 0 ) {
			//
			i = titlecount;
			//
			if ( !xListView_Enter() ) {	return FALSE; }
			//
			if ( titlecount > titlesetcount ) {
				lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
				lvColumn.fmt = LVCFMT_LEFT;
				//
				if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx = 140;
				else								lvColumn.cx = 200;
				//
				lvColumn.pszText = "";
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
				//
				for ( k = titlesetcount ; k < titlecount ; k++ ) {
					//
					ListView_InsertColumn( hlistview , k + 3 , &lvColumn );
					//
				}
				//
				titlesetcount = titlecount;
			}
			//===============================
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iSubItem	= 0;
			lvi.cchTextMax	= 100;
//			lvi.lParam		= ( itemsetcount * 10000 ) + 9000 + 1;
			lvi.lParam		= ( itemsetcount * 10000 ) + 6000 + wpm;
			lvi.iItem		= itemsetcount;
			lvi.iImage		= 0;
			lvi.pszText     = "";
			//
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				if ( !xListView_Leave() ) {	return FALSE; }
				break;
			}
			//--------------
			sprintf( Buffer2 , "[PM%d]" , wpm + 1 );
			//
			ListView_SetItemText( hlistview , itemsetcount , 0 , Buffer2 );
			//
			for ( k = 0 ; k < i ; k++ ) {
				ListView_SetItemText( hlistview , itemsetcount , 3 + k , WfrTitle[k] );
			}
			itemsetcount++;
			//--------------
			for ( k = 0 ; k < MAX_STEP_COUNT ; k++ ) {
				if ( Dat_Count_Total_Step[ k ] > 0 ) {
					//==============================
					lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
					lvi.state = lvi.stateMask = 0;
					lvi.iSubItem	= 0;
					lvi.cchTextMax	= 100;
					lvi.lParam		= ( itemsetcount * 10000 ) + 9000 + 8;
					lvi.iItem		= itemsetcount;
					lvi.iImage		= 0;
					lvi.pszText     = "";
					//
					if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
						if ( !xListView_Leave() ) {	return FALSE; }
						break;
					}

					sprintf( Buffer2 , "[STEP%d]" , k + 1 );

					ListView_SetItemText( hlistview , itemsetcount , 1 , Buffer2 );
					//
					ListView_SetItemText( hlistview , itemsetcount , 2 , "AVR" );
					//
					j = 0;
					while( ( Dat_Set_Step[ k ][ j ] ) && ( j <= i ) ) {
						if ( Dat_Count_Sum_Step[k][j] <= 0 ) { // 2008.02.28
							strcpy( Buffer2 , "N/A" );
							ListView_SetItemText( hlistview , itemsetcount , j + 3 , Buffer2 );
						}
						else {
							sprintf( Buffer2 , "%.3lf" , ( Dat_Sum_Step[ k ][ j ] ) / Dat_Count_Sum_Step[k][j] );
							ListView_SetItemText( hlistview , itemsetcount , j + 3 , Buffer2 );
						}
						//
						j++;
					}
					itemsetcount++;
					//==============================
					lvi.lParam		= ( itemsetcount * 10000 ) + 9000 + 2;
					lvi.iItem		= itemsetcount;
					lvi.iImage		= 0;
					lvi.pszText     = "";
					//
					if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
						if ( !xListView_Leave() ) {	return FALSE; }
						break;
					}
					//
					ListView_SetItemText( hlistview , itemsetcount , 2 , "MIN" );
					//
					j = 0;
					while( ( Dat_Set_Step[ k ][ j ] ) && ( j <= i ) ) {
						if ( Dat_Count_MinMax_Step[k][j] <= 0 ) { // 2008.02.28
							strcpy( Buffer2 , "N/A" );
							ListView_SetItemText( hlistview , itemsetcount , j + 3 , Buffer2 );
						}
						else {
							sprintf( Buffer2 , "%.3lf" , Dat_Min_Step[ k ][ j ] );
							ListView_SetItemText( hlistview , itemsetcount , j + 3 , Buffer2 );
						}
						//
						j++;
					}
					itemsetcount++;
					//==============================
					lvi.lParam		= ( itemsetcount * 10000 ) + 9000 + 2;
					lvi.iItem		= itemsetcount;
					lvi.iImage		= 0;
					lvi.pszText     = "";
					//
					if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
						if ( !xListView_Leave() ) {	return FALSE; }
						break;
					}

					//
					ListView_SetItemText( hlistview , itemsetcount , 2 , "MAX" );
					//
					j = 0;
					while( ( Dat_Set_Step[ k ][ j ] ) && ( j <= i ) ) {
						if ( Dat_Count_MinMax_Step[k][j] <= 0 ) { // 2008.02.28
							strcpy( Buffer2 , "N/A" );
							ListView_SetItemText( hlistview , itemsetcount , j + 3 , Buffer2 );
						}
						else {
							sprintf( Buffer2 , "%.3lf" , Dat_Max_Step[ k ][ j ] );
							ListView_SetItemText( hlistview , itemsetcount , j + 3 , Buffer2 );
						}
						//
						j++;
					}
					itemsetcount++;
					//==============================
				}
			}
			//==========================
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iSubItem	= 0;
			lvi.cchTextMax	= 100;
			lvi.lParam		= ( itemsetcount * 10000 ) + 9000 + 0;
			lvi.iItem		= itemsetcount;
			lvi.iImage		= 0;
			lvi.pszText     = "";
			//
			if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
				if ( !xListView_Leave() ) {	return FALSE; }
				break;
			}
			itemsetcount++;
			//==========================
			if ( !xListView_Leave() ) {	return FALSE; }
			//
		}
	}
	//===============================================================================================================================
	return TRUE;
}

BOOL LogView_Open_LotPM_Log_Data_Set( HWND hdlg , int winpos , int mode , int index , BOOL sepload ) { // [LOT DATA(Module)]
	//-----------------------------------------------------------------
	BOOL Res;
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_Start( hdlg );
	//-----------------------------------------------------------------
	Res = LOG_OPEN_CONTROL( hdlg , GetDlgItem( hdlg , winpos ) , sepload , NULL , mode , index , 0 , 0 , 0 , 0 , LogView_Open_LotPM_Log_Data_Set_Callback );
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_End( hdlg );
	//-----------------------------------------------------------------
	return Res;
	//-----------------------------------------------------------------
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY LogView_Open_LotPM_Log_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int j , k;
	int i;
	char Buffer[4097]; // 2007.11.21

	RECT rect;

	FILE *fpt;
	ViewStepTemplate *readdata;

	LPDRAWITEMSTRUCT dis; // 2015.07.29
	LPMEASUREITEMSTRUCT dim; // 2015.07.29

	switch ( msg ) {
		case WM_MEASUREITEM: // 2015.07.29
			//
			if ( ( GLISTVIEW_HEIGHT > 0 ) && ( ( GLISTVIEW_HEIGHT % 1000 ) >= 10 ) ) {
				//
				dim = (LPMEASUREITEMSTRUCT) lParam;
				//
				if ( dim->CtlType == ODT_LISTVIEW ) {
					if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
						 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
					}
				}
				//
			}
			else {
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
							 dim->itemHeight = LISTVIEW_HEIGHT;
						}
					}
					//
				}
				//
			}
			return TRUE;

		case WM_DRAWITEM: // 2015.07.29
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
			ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , -1 , 0 , 1 , "" , Buffer , 4095 , 0 , 0 , -1 , -1 , -1 , -1 );
			//
			break;

		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			readdata = (ViewStepTemplate *) (LPARAM) lParam;

			KWIN_Item_Hide( hdlg , IDC_GRAPH_BUTTON );

			if ( !Get_Prm_SAVEFILE_MODE() ) KWIN_Item_Hide( hdlg , IDRETRY );

			if ( !Get_Prm_SCROLLBUTTON_MODE() ) {
				KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
			}
			//----------------------------------------------------------------------------------------------
			if ( !LogView_Open_LotPM_Log_Data_Set( hdlg , IDC_LIST_LOGVIEW , readdata->mode , readdata->index , FALSE ) ) {
				EndDialog( hdlg , FALSE );
				return FALSE;
			}
			//----------------------------------------------------------------------------------------------
			MoveCenterWindow( hdlg );
			//----------------------------------------------------------------------------------------------
			SetWindowText( hdlg , readdata->str );
			//----------------------------------------------------------------------------------------------
			//===============================================================================================================================
			return TRUE;
			
		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
			case IDRETRY :
				//============================================================================
				// 2006.09.05
				//============================================================================
				if ( !Log_Get_Save_File_Name( hdlg , Buffer ) ) return TRUE;
				//
				fpt = fopen( Buffer , "w" );
				if ( fpt == NULL ) {
					MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] File Save Error!" , Buffer , MB_ICONHAND );
					return TRUE;
				}
				//==================================================================================================================================
				j = ListView_GetItemCount( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				for ( i = 0 ; i < j ; i++ ) {
					Log_Get_String_Form( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , i , 1 , 999 , fpt );
				}
				fclose( fpt );
				return TRUE;
				//============================================================================
			case IDC_CTRL_SDOWN :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * k );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , j );
				return TRUE;

			case IDC_CTRL_SUP :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * k );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , -j );
				return TRUE;

			case IDC_CTRL_RIGHT :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.right - rect.left;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;

			case IDC_CTRL_LEFT :
				k = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.left - rect.right;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;

			case IDCANCEL :
				EndDialog( hdlg , TRUE );
				return TRUE;

			}
			return TRUE;

	}
	return FALSE;
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
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int Elpased_Error_String( BOOL lotrun , char *Time2 , char *Time3 ) { // 2015.05.27
	int l;
	//
	if ( !lotrun ) {
		if ( Time2[0] == 0 ) return 1;
	}
	//
	l = strlen( Time3 );
	//
	if ( l < 3 ) return 0;
	//
	if ( Time3[l-1] != ')' ) return 0;
	if ( Time3[l-2] != 'F' ) return 0;
	if ( Time3[l-3] != '(' ) return 0;
	//
	Time3[l-3] = 0;
	//
	return 1;
	//
}
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL LogView_Open_Method_History_Insert( HWND hWnd , int i , int mode , char *s_No , char *s_WID , char *s_Index , char *s_Module , char *s_PrTime , char *s_PoTime , char *s_ElTime , char *s_Recipe , char *s_LogInfo , char *s_Style , char *s_TMMode , int error ) {
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	//
	if      ( mode == 0 )	lvi.pszText		= s_No;
	else if ( mode == 1 )	lvi.pszText		= s_WID;
	//
	lvi.cchTextMax	= 2560; // 2008.02.28
	lvi.iImage		= i;
	lvi.lParam		= ( i * 10000 ) + error;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	//
	switch( mode ) {
	case 1 : // new
		ListView_SetItemText( hWnd , i , 1 , s_Module );
		ListView_SetItemText( hWnd , i , 2 , s_Style );
		ListView_SetItemText( hWnd , i , 3 , s_TMMode );
		ListView_SetItemText( hWnd , i , 4 , s_PrTime );
		ListView_SetItemText( hWnd , i , 5 , s_PoTime );
		ListView_SetItemText( hWnd , i , 6 , s_ElTime );
		ListView_SetItemText( hWnd , i , 7 , s_Recipe );
		ListView_SetItemText( hWnd , i , 8 , s_LogInfo );
		break;
	default : // old
		ListView_SetItemText( hWnd , i , 1 , s_WID );
		ListView_SetItemText( hWnd , i , 2 , s_Index );
		ListView_SetItemText( hWnd , i , 3 , s_Module );
		ListView_SetItemText( hWnd , i , 4 , s_PrTime );
		ListView_SetItemText( hWnd , i , 5 , s_PoTime );
		ListView_SetItemText( hWnd , i , 6 , s_ElTime );
		ListView_SetItemText( hWnd , i , 7 , s_Recipe );
		ListView_SetItemText( hWnd , i , 8 , s_LogInfo );
		break;
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
//
BOOL LogView_Open_Method_History_Init( HWND hwndListView , int mode ) {
	int         i , cc , widpos;
	LV_COLUMN   lvColumn;

	TCHAR       *szString0[] = { "No" , "MethodName" , "Index" , "Module" , "PrTime" , "PoTime" , "ElTime" , "ProcessRecipe" , "Process Data" };
	int         szSize0[9]   = {  55  ,          210 ,      70 ,      130 ,      170 ,      170 ,      100 ,           200   , 400 };
	int         szSize_M0[9] = {  40  ,          150 ,      50 ,      120 ,      140 ,      140 ,       80 ,           150   , 400 };

	TCHAR       *szString1[] = { "MethodName" , "Module" , "Mode"  ,  "TM.Arm" , "Start Time" , "End Time" , "Elapsed Time" , "ProcessRecipe" , "Process Data" };
	int         szSize1[9]   = {          175 ,       113 ,     85 ,         78 ,   170 ,      170 ,      100 ,           200   , 200 };
	int         szSize_M1[9] = {          115 ,       103 ,     75 ,         68 ,   140 ,      140 ,       80 ,           150   , 200 };

	cc     = 9;
	widpos = ( mode == 1 ) ? 0 : 1;
	//
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break; // 2011.06.16
	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	if ( Get_Prm_MINISCREEN_MODE() ) {
		for ( i = 0 ; i < cc ; i++ ) {
			switch( mode ) {
			case 1 :	lvColumn.cx = szSize_M1[i];	break;
			default :	lvColumn.cx = szSize_M0[i];	break;
			}
			if ( i != widpos ) {
				switch( mode ) {
				case 1 :	lvColumn.pszText = szString1[i];	break;
				default :	lvColumn.pszText = szString0[i];	break;
				}
			}
			else {
				lvColumn.pszText = Get_Prm_METHOD_STRING();
			}
			//
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}	
	}
	else {
		for ( i = 0 ; i < cc ; i++ ) {
			switch( mode ) {
			case 1 :	lvColumn.cx = szSize1[i];	break;
			default :	lvColumn.cx = szSize0[i];	break;
			}
			if ( i != widpos ) {
				switch( mode ) {
				case 1 :	lvColumn.pszText = szString1[i];	break;
				default :	lvColumn.pszText = szString0[i];	break;
				}
			}
			else {
				lvColumn.pszText = Get_Prm_METHOD_STRING();
			}
			//
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}	
	}
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
int  _JobData_ListView_SortDir2[256];
HWND _JobData_ListView_SortHwnd2;
int  _JobData_ListView_SortItem2_No;

int CALLBACK CallBack_JobData_ListView_Sort2( LPARAM lParam1 , LPARAM lParam2 , LPARAM lParamSort ) {
	static LV_FINDINFO fi;
	static char szBuf1[256] , szBuf2[256];
	static int nItem1 , nItem2 , Res;
	static int ci1 , ci2;
	static double di1 , di2;
	//
	fi.flags = LVFI_PARAM;
	fi.lParam = lParam1;	nItem1 = ListView_FindItem( _JobData_ListView_SortHwnd2 , -1 , &fi );
	fi.lParam = lParam2;	nItem2 = ListView_FindItem( _JobData_ListView_SortHwnd2 , -1 , &fi );
	//
	ListView_GetItemText( _JobData_ListView_SortHwnd2 , nItem1 , lParamSort , szBuf1 , sizeof( szBuf1 ) );
	ListView_GetItemText( _JobData_ListView_SortHwnd2 , nItem2 , lParamSort , szBuf2 , sizeof( szBuf2 ) );
	//
	switch( _JobData_ListView_SortItem2_No ) {
	case 1 : // int
		ci1 = atoi( szBuf1 );
		ci2 = atoi( szBuf2 );
		//
		if      ( ci1 > ci2 ) {
			Res = 1;
		}
		else if ( ci1 == ci2 ) {
			Res = 0;
		}
		else {
			Res = -1;
		}
		break;
	case 2 : // double
		di1 = atof( szBuf1 );
		di2 = atof( szBuf2 );
		//
		if      ( di1 > di2 ) {
			Res = 1;
		}
		else if ( di1 == di2 ) {
			Res = 0;
		}
		else {
			Res = -1;
		}
		break;
	default :
		Res = strcmp( szBuf1 , szBuf2 );
		break;
	}
	//
	if ( Res == 0 ) {
		if ( ( lParam1 / 10000 ) > ( lParam2 / 10000 ) ) {
			Res = 1;
		}
		else if ( ( lParam1 / 10000 ) == ( lParam2 / 10000 ) ) {
			if ( lParam1 > lParam2 ) {
				Res = 1;
			}
			else {
				Res = 0;
			}
		}
		else {
			Res = 0;
		}
	}
	//
	if ( _JobData_ListView_SortDir2[lParamSort] == 0 )
		return ( Res );
	else
		return ( Res * -1 );
	//
}
//
int  _JobData_ListView_SortDir3[256];
HWND _JobData_ListView_SortHwnd3;
int  _JobData_ListView_SortItem3_No;

int CALLBACK CallBack_JobData_ListView_Sort3( LPARAM lParam1 , LPARAM lParam2 , LPARAM lParamSort ) {
	static LV_FINDINFO fi;
	static char szBuf1[256] , szBuf2[256];
	static int nItem1 , nItem2 , Res;
	static int ci1 , ci2;
	static double di1 , di2;
	//
	fi.flags = LVFI_PARAM;
	fi.lParam = lParam1;	nItem1 = ListView_FindItem( _JobData_ListView_SortHwnd3 , -1 , &fi );
	fi.lParam = lParam2;	nItem2 = ListView_FindItem( _JobData_ListView_SortHwnd3 , -1 , &fi );
	//
	ListView_GetItemText( _JobData_ListView_SortHwnd3 , nItem1 , lParamSort , szBuf1 , sizeof( szBuf1 ) );
	ListView_GetItemText( _JobData_ListView_SortHwnd3 , nItem2 , lParamSort , szBuf2 , sizeof( szBuf2 ) );
	//
	switch( _JobData_ListView_SortItem3_No ) {
	case 1 : // int
		ci1 = atoi( szBuf1 );
		ci2 = atoi( szBuf2 );
		//
		if      ( ci1 > ci2 ) {
			Res = 1;
		}
		else if ( ci1 == ci2 ) {
			Res = 0;
		}
		else {
			Res = -1;
		}
		break;
	case 2 : // double
		di1 = atof( szBuf1 );
		di2 = atof( szBuf2 );
		//
		if      ( di1 > di2 ) {
			Res = 1;
		}
		else if ( di1 == di2 ) {
			Res = 0;
		}
		else {
			Res = -1;
		}
		break;
	default :
		Res = strcmp( szBuf1 , szBuf2 );
		break;
	}
	//
	if ( Res == 0 ) {
		if ( ( lParam1 / 10000 ) > ( lParam2 / 10000 ) ) {
			Res = 1;
		}
		else if ( ( lParam1 / 10000 ) == ( lParam2 / 10000 ) ) {
			if ( lParam1 > lParam2 ) {
				Res = 1;
			}
			else {
				Res = 0;
			}
		}
		else {
			Res = 0;
		}
	}
	//
	if ( _JobData_ListView_SortDir3[lParamSort] == 0 )
		return ( Res );
	else
		return ( Res * -1 );
	//
}
//
int ListView_Index_is_No( HWND hWnd , int No ) {
	int i , j , l , err , c;
	int double_num , num_find , minus_find , plus_find , exp_find , point_find;
	char Data[256];

	c = ListView_GetItemCount( hWnd );
	//
	if ( c > 10 ) c = 10;
	if ( c < 10 ) return FALSE;
	//
	double_num = 0;
	//
	l = 0;
	//
	for ( i = 0 ; i < c ; i++ ) {
		//
		ListView_GetItemText( hWnd , i , No , Data , 255 );
		//
		j = 0;
		err = 0;
		//
		point_find = 0;
		num_find   = 0;
		minus_find = 0;
		plus_find  = 0;
		exp_find   = 0;
		//
		while ( TRUE ) {
			//
			if ( Data[j] == 0 ) break;
			//
			if      ( Data[j] == '-' ) {
				if ( num_find && !exp_find ) { err = 1; break; }
				minus_find = 1;
			}
			else if ( Data[j] == '+' ) {
				if ( num_find && !exp_find ) { err = 1; break; }
				plus_find = 1;
			}
			else if ( Data[j] == '.' ) {
				if ( point_find && !exp_find ) { err = 1; break; }
				if ( !num_find ) { err = 1; break; }
				point_find = 1;
				double_num = 1;
			}
			else if ( ( Data[j] == 'E' ) || ( Data[j] == 'e' ) ) {
				if ( !num_find ) { err = 1; break; }
				exp_find = 1;
			}
			else if ( ( Data[j] >= '0' ) && ( Data[j] <= '9' ) ) {
				num_find = 1;
			}
			else {
				err = 1;
				break;
			}
			//
			j++;
			//
		}
		//
		if ( err == 1 ) {
			l++;
			if ( l > 3 ) return 0;
		}
		//
	}
	//
	if ( double_num ) return 2;
	return 1;
}

//---------------------------------------------------------------------------------------
BOOL LogView_Open_Method_History_Insert_CURRENT_WAFER( HWND hwndListView , BOOL multimode , int index0 );
//
BOOL LogView_Control_View_Change( HWND hWnd , BOOL multi ) {
	char Buffer2[256];
	int i;
	//
	if ( Get_Prm_DISPLAY_MODE() < 2 ) return FALSE;
	//
	i = -1;
	//
	if ( multi ) {
		//
		if ( CURRENT_WAFER_Data_Set_View_Mode == 0 ) {
			sprintf( Buffer2 , "%s|MODULE|%s(IDLE TIME)|MODULE(IDLE TIME)|INFO|VIEW SIZE (%d %c)|VIEW MODE - TIMELINE <==|VIEW MODE - SAMELINE" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , CURRENT_WAFER_Data_Set_Min_Dev , '%' );
		}
		else {
			sprintf( Buffer2 , "%s|MODULE|%s(IDLE TIME)|MODULE(IDLE TIME)|INFO|VIEW SIZE (%d %c)|VIEW MODE - TIMELINE|VIEW MODE - SAMELINE <==" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , CURRENT_WAFER_Data_Set_Min_Dev , '%' );
		}
		//
	}
	else {
		sprintf( Buffer2 , "%s|MODULE|%s(IDLE TIME)|MODULE(IDLE TIME)|INFO|VIEW SIZE (%d %c)" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , CURRENT_WAFER_Data_Set_Min_Dev , '%' );
	}
	//
	if ( !MODAL_DIGITAL_BOX1( hWnd , "Select" , "Select View Style" , Buffer2 , &i ) ) return FALSE;
	//
	if      ( i == 6 ) { // timeline
		//
		if ( CURRENT_WAFER_Data_Set_View_Mode != 0 ) {
			//
			CURRENT_WAFER_Data_Set_View_Mode = 0;
			//
			CURRENT_WAFER_Data_Set_Min_Mode = CURRENT_WAFER_Data_Set_Min_Mode % 4;
			//
			LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
			//
		}
	}
	else if ( i == 7 ) { // sameline
		//
		if ( CURRENT_WAFER_Data_Set_View_Mode == 0 ) {
			//
			CURRENT_WAFER_Data_Set_View_Mode = 1;
			//
			CURRENT_WAFER_Data_Set_Min_Mode = CURRENT_WAFER_Data_Set_Min_Mode % 4;
			//
			LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
			//
		}
	}
	else if ( i == 5 ) { // viewsize
		//
		i = CURRENT_WAFER_Data_Set_Min_Dev;
		//
		if ( !MODAL_DIGITAL_BOX2( hWnd , "Select" , "Select View Size(%)" , 1 , 999 , &i ) ) return FALSE;
		//
		CURRENT_WAFER_Data_Set_Min_Mode = CURRENT_WAFER_Data_Set_Min_Mode % 4;
		CURRENT_WAFER_Data_Set_Min_Dev = i;
		//
		LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
	}
	else if ( i == 4 ) {
		//
		LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , CURRENT_WAFER_Data_Set_Min_Index );
	}
	else {
		CURRENT_WAFER_Data_Set_Min_Mode = i;
		//
		LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
	}
	return TRUE;
}

LRESULT LogView_Open_Method_History_Notify( HWND hWnd , LPARAM lParam ) {
	LPNMHDR  lpnmh = (LPNMHDR) lParam;
	int i , prm , chk;
	char Data[256];
	char Buffer2[256];
	char GetMdlStr[64]; // 2016.12.22
	ViewStepTemplate data;

	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: 
				return 0;

		case NM_DBLCLK :
			if ( lpnmh->hwndFrom == hWnd ) {
				//
				i = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED );
				if ( i >= 0 ) {
					//
					chk = FALSE;
					//
					prm = LogView_GetParam( hWnd , i );
					//
					if      ( ( prm % 10000 ) >= 9000 ) {
					}
					else if ( ( prm % 10000 ) >= 8000 ) {
					}
					else if ( ( prm % 10000 ) >= 7000 ) {
					}
					else if ( ( prm % 10000 ) >= 6000 ) {
					}
					else if ( ( prm % 10000 ) >= 5000 ) {
						chk = TRUE;
					}
					else if ( ( prm % 10000 ) >= 4000 ) {
						chk = TRUE;
					}
					else if ( ( prm % 10000 ) >= 3000 ) {
					}
					if ( chk ) {
						//
						if ( CURRENT_WAFER_Data_Set_Min_Mode != 0 ) {
							//
							if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
								if ( i == 0 ) {
									//
									i = CURRENT_WAFER_Data_Set_Min_Mode % 4;
									//
									sprintf( Buffer2 , "%s|MODULE|%s(IDLE TIME)|MODULE(IDLE TIME)|INFO" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									//
									if ( !MODAL_DIGITAL_BOX1( hWnd , "Select" , "Select View Style" , Buffer2 , &i ) ) return FALSE;
									//
									if ( i == 4 ) {
										//
										LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , CURRENT_WAFER_Data_Set_Min_Index );
									}
									else {
										CURRENT_WAFER_Data_Set_Min_Mode = i;
										//
										LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
									}
								}
								//
								else {
									//
									i = CURRENT_WAFER_Data_Set_Min_Dev;
									//
									if ( !MODAL_DIGITAL_BOX2( hWnd , "Select" , "Select View Size(%)" , 1 , 999 , &i ) ) return FALSE;
									//
									CURRENT_WAFER_Data_Set_Min_Mode = CURRENT_WAFER_Data_Set_Min_Mode % 4;
									CURRENT_WAFER_Data_Set_Min_Dev = i;
									//
									LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
								}
								//
							}
						}
						//
						return 0;
					}
					//
					if      ( ( prm % 10000 ) >= 9000 ) {
					}
					else if ( ( prm % 10000 ) >= 8000 ) {
						//
//						if ( !LogView_Open_Process_Log_Ext( GetParent( hWnd ) , IDC_LIST_JOBDATA_S2 , ( prm / 10000 ) , 0 , ( prm % 1000 ) , Data , Buffer2 ) ) return FALSE; // 2016.12.22
						if ( !LogView_Open_Process_Log_Ext( GetParent( hWnd ) , IDC_LIST_JOBDATA_S2 , ( prm / 10000 ) , 0 , ( prm % 1000 ) , Data , Buffer2 , GetMdlStr ) ) return FALSE; // 2016.12.22
						//
//						JOBDATA_S2_DISPLAY_CONTROL( GetParent( hWnd ) , CTR_PROCESS_DATA_1_WAFER , 0 , Data , Buffer2 ); // 2016.12.22
						JOBDATA_S2_DISPLAY_CONTROL( GetParent( hWnd ) , CTR_PROCESS_DATA_1_WAFER , 0 , Data , Buffer2 , GetMdlStr ); // 2016.12.22
						//
						return TRUE;
					}
					else if ( ( prm % 10000 ) >= 3000 ) {
					}
					else {
						//
						ListView_GetItemText( hWnd , i , 8 , Data , 255 );
						//
						if ( strlen( Data ) > 0 ) {
							//
							data.mode  = 0;
							strncpy( data.str , Data , 255 );
							//
							if ( Get_Prm_DISPLAY_MODE() < 2 ) {
								//
								if ( LISTVIEW_HEIGHT_UPDATE ) { // 2015.07.29
									GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_Process_Log_Proc , (LPARAM) &data );
								}
								else {
									if ( Get_Prm_MINISCREEN_MODE() ) {
										GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Process_Log_Proc , (LPARAM) &data ); // 2004.01.19 // 2004.08.10
									}
									else {
										GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Process_Log_Proc , (LPARAM) &data ); // 2004.01.19 // 2004.08.10
									}
								}
								//
							}
							else {
								//
								if ( !LogView_Open_Process_Log_Data_Set( GetParent( hWnd ) , IDC_LIST_JOBDATA_S2 , Data , Buffer2 , FALSE , 0 , 0 , 0 , TRUE , FALSE ) ) return FALSE;
								//
								ListView_GetItemText( hWnd , i , 1 , GetMdlStr , 255 ); // 2016.12.22
								//
								sprintf( Buffer2 , "* PROCESS DATA : %s" , data.str );
								JOBDATA_S2_DISPLAY_CONTROL( GetParent( hWnd ) , CTR_PROCESS_DATA_1_WAFER , 0 , Buffer2 , Data , GetMdlStr );
								//
								return TRUE;
							}
						}
					}
				}
				else { // 2012.12.05
					//
					chk = FALSE;
					//
					prm = LogView_GetParam( hWnd , 0 );
					//
					if      ( ( prm % 10000 ) >= 9000 ) {
					}
					else if ( ( prm % 10000 ) >= 8000 ) {
						chk = TRUE;
					}
					else if ( ( prm % 10000 ) >= 7000 ) {
					}
					else if ( ( prm % 10000 ) >= 6000 ) {
					}
					else if ( ( prm % 10000 ) >= 5000 ) {
						chk = TRUE;
					}
					else if ( ( prm % 10000 ) >= 4000 ) {
						chk = TRUE;
					}
					else if ( ( prm % 10000 ) >= 3000 ) {
					}
					if ( chk ) {
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
							//
							if ( CURRENT_WAFER_Data_Set_Min_Mode == 0 ) {
								LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
							}
							else {
								LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , CURRENT_WAFER_Data_Set_Min_Index );
							}
							//
						}
					}
				}
			}
			return 0;

		case LVN_COLUMNCLICK: { // 2009.09.04
				NM_LISTVIEW *pNM = (NM_LISTVIEW*) lParam;
				//
				chk = FALSE;
				//
				prm = LogView_GetParam( hWnd , 0 );
				//
				if      ( ( prm % 10000 ) >= 9000 ) {
				}
				else if ( ( prm % 10000 ) >= 8000 ) {
				}
				else if ( ( prm % 10000 ) >= 7000 ) {
				}
				else if ( ( prm % 10000 ) >= 6000 ) {
				}
				else if ( ( prm % 10000 ) >= 5000 ) {
					chk = TRUE;
				}
				else if ( ( prm % 10000 ) >= 4000 ) {
					chk = TRUE;
				}
				else if ( ( prm % 10000 ) >= 3000 ) {
				}
				//
				if ( chk ) {
					if ( CURRENT_WAFER_Data_Set_Min_Mode != 0 ) {
						//
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
							if ( pNM->iSubItem == 2+ZOFFSET_5000_and_8000 ) {
								//
								i = CURRENT_WAFER_Data_Set_Min_Mode % 4;
								//
								sprintf( Buffer2 , "%s|MODULE|%s(IDLE TIME)|MODULE(IDLE TIME)|INFO" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
								//
								if ( !MODAL_DIGITAL_BOX1( hWnd , "Select" , "Select View Style" , Buffer2 , &i ) ) return FALSE;
								//
								if ( i == 4 ) {
									//
									LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , CURRENT_WAFER_Data_Set_Min_Index );
								}
								else {
									CURRENT_WAFER_Data_Set_Min_Mode = i;
									//
									LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
								}
							}
							//
							else if ( pNM->iSubItem == 3+ZOFFSET_5000_and_8000 ) {
								//
								i = CURRENT_WAFER_Data_Set_Min_Dev;
								//
								if ( !MODAL_DIGITAL_BOX2( hWnd , "Select" , "Select View Size(%)" , 1 , 999 , &i ) ) return FALSE;
								//
								CURRENT_WAFER_Data_Set_Min_Mode = CURRENT_WAFER_Data_Set_Min_Mode % 4;
								CURRENT_WAFER_Data_Set_Min_Dev = i;
								//
								LogView_Open_Method_History_Insert_CURRENT_WAFER( hWnd , CURRENT_WAFER_Data_Set_MultiMode , -1 );
							}
							//
						}
					}
				}
				else {
					if ( Get_Prm_SORT_MODE() ) {
						//
						if ( _JobData_ListView_SortDir2[ pNM->iSubItem ] == 0 )
							_JobData_ListView_SortDir2[ pNM->iSubItem ] = 1;
						else
							_JobData_ListView_SortDir2[ pNM->iSubItem ] = 0;
						//
						_JobData_ListView_SortHwnd2 = hWnd;
						//
						_JobData_ListView_SortItem2_No = ListView_Index_is_No( lpnmh->hwndFrom , pNM->iSubItem );
						//
						ListView_SortItems( hWnd , CallBack_JobData_ListView_Sort2 , pNM->iSubItem );
						//
					}
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
//
//

//---------------------------------------------------------------------------------------
LRESULT LogView_Open_Method_History_Notify3( HWND hWnd , LPARAM lParam ) {
	LPNMHDR  lpnmh = (LPNMHDR) lParam;

	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: 
				return 0;

		case NM_DBLCLK :
			return 0;

		case LVN_COLUMNCLICK: { // 2009.09.04
				NM_LISTVIEW *pNM = (NM_LISTVIEW*) lParam;
				//
				if ( Get_Prm_SORT_MODE() ) {
					//
					if ( _JobData_ListView_SortDir3[ pNM->iSubItem ] == 0 )
						_JobData_ListView_SortDir3[ pNM->iSubItem ] = 1;
					else
						_JobData_ListView_SortDir3[ pNM->iSubItem ] = 0;
					//
					_JobData_ListView_SortHwnd3 = hWnd;
					//
					_JobData_ListView_SortItem3_No = ListView_Index_is_No( lpnmh->hwndFrom , pNM->iSubItem );
					//
					ListView_SortItems( hWnd , CallBack_JobData_ListView_Sort3 , pNM->iSubItem );
					//
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
//
//
LRESULT LogView_Open_Method_History_NotifySPM( HWND hWnd , LPARAM lParam ) { // 2016.08.17
	LPNMHDR  lpnmh = (LPNMHDR) lParam;

	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: 
				return 0;

		case NM_DBLCLK :
			return 0;

		case LVN_COLUMNCLICK:
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

LRESULT LogView_Open_Method_History_NotifySPMS( HWND hWnd , LPARAM lParam ) { // 2016.08.17
	LPNMHDR  lpnmh = (LPNMHDR) lParam;

	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: 
				return 0;

		case NM_DBLCLK :
			return 0;

		case LVN_COLUMNCLICK:
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
//
//

BOOL APIENTRY LogView_Open_Method_History_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int i , j , z , c;
	static char FileName[256];
	static char FileName_org[256];
//	char Buffer[512]; // 2008.02.28
	char Buffer[4097]; // 2008.02.28
	char Buffer2[16];
	char MethodName[64];
	char Index[64];
	char Chamber[64];
	char Chamber2[64];
	char Time1[64];
	char Time2[64];
	char Time3[64];
//	char Recipe[256]; // 2008.02.28
	char Recipe[2560]; // 2008.02.28

	char folderstr[256]; // 2008.02.28

	BOOL bres;
	FILE *fpt;

	RECT rect;		// crkim
	//===================================================
	int  color_error;
	BOOL Temp_Use = FALSE;
	int  Temp_Color;
	char Temp_Buffer2[16];
	char Temp_MethodName[64];
	char Temp_Index[64];
	char Temp_Chamber[64];
	char Temp_Time1[64];
	char Temp_Time2[64];
	char Temp_Time3[64];
//	char Temp_Recipe[256]; // 2008.02.28
	char Temp_Recipe[2560]; // 2008.02.28
	char Temp_Buffer_z[512];

	LPDRAWITEMSTRUCT dis;
	//
	int encflag;

	int errflag; // 2015.05.27

	static int lotrun; // 2015.05.27
	//===================================================
	LPMEASUREITEMSTRUCT dim; // 2015.07.29

	switch ( msg ) {
		case WM_MEASUREITEM: // 2015.07.29
			//
			if ( ( GLISTVIEW_HEIGHT > 0 ) && ( ( GLISTVIEW_HEIGHT % 1000 ) >= 10 ) ) {
				//
				dim = (LPMEASUREITEMSTRUCT) lParam;
				//
				if ( dim->CtlType == ODT_LISTVIEW ) {
					if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
						 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
					}
				}
				//
			}
			else {
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
							 dim->itemHeight = LISTVIEW_HEIGHT;
						}
					}
					//
				}
				//
			}
			return TRUE;
			//
		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			KWIN_Item_Hide( hdlg , IDC_GRAPH_BUTTON );
			if ( !Get_Prm_SAVEFILE_MODE() ) KWIN_Item_Hide( hdlg , IDRETRY );
			// crkim // 2002.11.11
			if ( !Get_Prm_SCROLLBUTTON_MODE() ) {
				KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
			}
			// crkim // 2002.11.11

			//>>-----------------------------------------------------------
			MoveCenterWindow( hdlg );
			//
			//----------------------------------------------------------------------------------------------
			// 2003.09.16
			//----------------------------------------------------------------------------------------------
			if ( JOB_Get_Current_Count() <= ( (int) lParam ) ) {
				MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
				EndDialog( hdlg , FALSE );
				return FALSE;
			}
			if ( JOB_Get_Directory_Type( (int) lParam ) != 1 ) {
				MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed & Data not exist" , "Try Again" , MB_ICONHAND );
				EndDialog( hdlg , FALSE );
				return FALSE;
			}
			//----------------------------------------------------------------------------------------------
			JOB_Get_Directory( (int) lParam , folderstr );
			//
			sprintf( FileName_org , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , folderstr , METHOD_LOG_FILE );
			//----------------------------------------------------------------------------------------------
			if ( !Get_Prm_NETMONITOR_MODE() ) {
				//
				strcpy( FileName , FileName_org );
				//
				lotrun = ( JOB_Get_Run_Status( (int) lParam ) == JOB_RUNNING ); // 2015.05.27
				//
			}
			else { // 2003.12.18
				sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
				_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
				_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
				//
				lotrun = TRUE; // 2015.05.27
				//
			}
			//-----------------------------------------------------------------
			// 2007.07.31
			//-----------------------------------------------------------------
//			if ( !Check_File_is_Open_Possible_Check( FileName ) ) {
//				MODAL_WINDOW_MESSAGE_BOX( hdlg , "File is not properly to Open(Reject)" , FileName , MB_ICONHAND );
//				EndDialog( hdlg , TRUE );
//				return TRUE;
//			}
			//-----------------------------------------------------------------
			JOB_Get_Display_String( (int) lParam , Recipe );
			sprintf( Buffer , "%s [%s]" , Recipe , FileName );
			SetWindowText( hdlg , Buffer );
			//
			if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) { // 2005.03.22
				sprintf( Buffer , "[%s] File Not Found" , FileName );
				MODAL_WINDOW_MESSAGE_BOX( hdlg , Buffer , "Open Error [5]", MB_ICONHAND );
				EndDialog( hdlg , TRUE );
				return TRUE;
			}
			//
			LogView_Open_Method_History_Init( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 );
			//
			Temp_Use = FALSE; // 2007.06.20
			i = j = 0;
			//
			encflag = 0; // 2011.12.07
			//
			do {
				bres = Read_Line_From_File4( fpt , Buffer , 4096 ); // 2008.02.28
				if ( *Buffer != 0 ) {
					//
//					Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
					Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
					//
					z = 0;
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , MethodName , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Index , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Chamber , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time1 , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time2 , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time3 , z , 63 );
//					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Recipe , z , 255 ); // 2008.02.28
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Recipe , z , 2559 ); // 2008.02.28
					//
					if ( strlen( MethodName ) > 0 ) {
						if ( strncmp( MethodName , "{!RESTART(" , 10 ) == 0 ) { // 2002.06.17
							i = 0;
							strcpy( Buffer2 , "*" );
							//
							//======================================================================================================
							if ( Temp_Use ) {
								// error
								LogView_Open_Method_History_Insert( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , j , 0 , Temp_Buffer2 , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , "" , "" , 1 + ( Temp_Color * 10 ) );
								j++;
							}
							//
							Temp_Use = TRUE;
							//
							strcpy( Temp_Buffer2 , Buffer2 );
							strcpy( Temp_MethodName , MethodName );
							strcpy( Temp_Index , Index );
							//
//							strcpy( Temp_Chamber , Chamber ); // 2009.07.22
							//
							if ( Chamber[0] == '$' ) { // 2009.07.21
								Temp_Color = 2; // 2009.07.21
//								strcpy( Temp_Chamber , Chamber + 1 ); // 2015.12.03
								CHAMBER_STRING_CHANGE( Chamber + 1 , Temp_Chamber ); // 2015.12.03
							}
							else {
								Temp_Color = 0; // 2009.07.21
//								CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 ); // 2015.12.03
								CHAMBER_STRING_CHANGE( Chamber , Chamber2 ); // 2015.12.03
								strcpy( Temp_Chamber , Chamber2 );
							}
							//
							strcpy( Temp_Time1 , Time1 );
							strcpy( Temp_Time2 , Time2 );
							strcpy( Temp_Time3 , Time3 );
							strcpy( Temp_Recipe , Recipe );
							strcpy( Temp_Buffer_z , Buffer + z );
							//======================================================================================================
						}
						else {
							i++;
							sprintf( Buffer2 , "%d" , i );
							//
							if ( Chamber[0] == '$' ) { // 2009.07.21
								c = 2; // 2009.07.21
//								strcpy( Chamber2 , Chamber + 1 ); // 2015.12.03
								CHAMBER_STRING_CHANGE( Chamber + 1 , Chamber2 ); // 2015.12.03
							}
							else {
								c = 0; // 2009.07.21
//								CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 ); // 2006.10.26 // 2015.12.03
								CHAMBER_STRING_CHANGE( Chamber , Chamber2 ); // 2015.12.03
							}
							//
							//======================================================================================================
							if ( Temp_Use ) {
								// error
								LogView_Open_Method_History_Insert( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , j , 0 , Temp_Buffer2 , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , "" , "" , 1 + ( Temp_Color * 10 ) );
								j++;
							}
							//
							Temp_Use = TRUE;
							Temp_Color = c; // 2009.07.21
							strcpy( Temp_Buffer2 , Buffer2 );
							strcpy( Temp_MethodName , MethodName );
							strcpy( Temp_Index , Index );
//							strcpy( Temp_Chamber , Chamber ); // 2008.02.27
							strcpy( Temp_Chamber , Chamber2 ); // 2008.02.27
							strcpy( Temp_Time1 , Time1 );
							strcpy( Temp_Time2 , Time2 );
							strcpy( Temp_Time3 , Time3 );
							strcpy( Temp_Recipe , Recipe );
							strcpy( Temp_Buffer_z , Buffer + z );
							//======================================================================================================
						}
					}
					else {
						if ( Temp_Use ) {
							//
							errflag = Elpased_Error_String( lotrun , Temp_Time2 , Temp_Time3 ); // 2015.05.27
							//
							LogView_Open_Method_History_Insert( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , j , 0 , Temp_Buffer2 , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , "" , "" , errflag + ( Temp_Color * 10 ) );
							j++;
						}
						Temp_Use = FALSE;
						//===================================================================================================
						sprintf( Buffer2 , "" );
						//
						if ( Chamber[0] == '$' ) { // 2009.07.21
							c = 2; // 2009.07.21
//							strcpy( Chamber2 , Chamber + 1 ); // 2015.12.03
							CHAMBER_STRING_CHANGE( Chamber + 1 , Chamber2 ); // 2015.12.03
						}
						else {
							c = 0; // 2009.07.21
//							CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 ); // 2015.12.03
							CHAMBER_STRING_CHANGE( Chamber , Chamber2 ); // 2015.12.03
						}
						//
						errflag = Elpased_Error_String( lotrun , Time2 , Time3 ); // 2015.05.27
						//
						LogView_Open_Method_History_Insert( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , j , 0 , Buffer2 , MethodName , Index , Chamber2 , Time1 , Time2 , Time3 , Recipe , Buffer + z , "" , "" , errflag + ( c * 10 ) );
						j++; // 2007.06.20
					}
//					j++; // 2007.06.20
				}
			}
			while ( bres );
			//=========================================================================================================
			// 2007.06.20
			//=========================================================================================================
			if ( Temp_Use ) {
				// error
				LogView_Open_Method_History_Insert( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , j , 0 , Temp_Buffer2 , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , "" , "" , 1 + ( Temp_Color * 10 ) );
				j++;
			}
			//=========================================================================================================
			fclose( fpt );
			return TRUE;
			
		case WM_DRAWITEM:
			dis = (LPDRAWITEMSTRUCT) lParam;
			color_error = LogView_GetParam( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , dis->itemID );
			//
			ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , -1 , color_error % 10 , 1 , "" , Temp_Recipe , 2559 , ( color_error / 10 ) % 10 , 0 , -1 , -1 , -1 , -1 );
			//
			break;

		case WM_NOTIFY:
			LogView_Open_Method_History_Notify( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam );
			return TRUE;

		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
				case IDRETRY :
					if ( Log_Get_Save_File_Name( hdlg , Buffer ) ) {
						if ( !SAVE_FILE( FileName , Buffer , FALSE ) ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] File Save Error!" , Buffer , MB_ICONHAND );
						}
					}
					return TRUE;
/// crkim
				case IDC_CTRL_SDOWN :
					i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
					ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
					j = ( ( rect.bottom - rect.top ) * i );
					ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , j );
					return TRUE;

				case IDC_CTRL_SUP :
					i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
					ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
					j = ( ( rect.bottom - rect.top ) * i );
					ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , -j );
					return TRUE;

				case IDC_CTRL_RIGHT :
					i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
					ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
					j = rect.right - rect.left;
					ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
					return TRUE;

				case IDC_CTRL_LEFT :
					i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
					ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
					j = rect.left - rect.right;
					ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
					return TRUE;


				case IDCANCEL :
					EndDialog( hdlg , TRUE );
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
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
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL LogView_Open_Full_Method_Insert( HWND hWnd , int i , char *Msg , char *Msg2 , char *Msg3 , char *Msg4 , char *Msg5 , char *Msg6 , char *Msg7 , char *Msg8 , char *Msg9 ) {
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
//	lvi.cchTextMax	= 100; // 2008.02.28
	lvi.cchTextMax	= 2560; // 2008.02.28
	lvi.iImage		= i;
	lvi.lParam		= 0;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	ListView_SetItemText( hWnd , i , 1 , Msg2 );
	ListView_SetItemText( hWnd , i , 2 , Msg3 );
	ListView_SetItemText( hWnd , i , 3 , Msg4 );
	ListView_SetItemText( hWnd , i , 4 , Msg5 );
	ListView_SetItemText( hWnd , i , 5 , Msg6 );
	ListView_SetItemText( hWnd , i , 6 , Msg7 );
	ListView_SetItemText( hWnd , i , 7 , Msg8 );
	ListView_SetItemText( hWnd , i , 8 , Msg9 );
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//
BOOL LogView_Open_Full_Method_Init( HWND hwndListView ) {
	int         i;
	LV_COLUMN   lvColumn;
	TCHAR       *szString[] = { "No" , "Name" , "Index" , "Module" , "PrTime" , "PoTime" , "ElTime" , "ProcessRecipe" , "Process Data" };
	int         szSize[9]   = {  55  ,    210 ,      70 ,      130 ,      150 ,      150 ,      100 ,           200   , 400 };
	int         szSize_M[9] = {  40  ,    140 ,      45 ,      100 ,      140 ,      140 ,       80 ,           150   , 400 };
	//
	ListView_DeleteAllItems( hwndListView );
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	if ( Get_Prm_MINISCREEN_MODE() ) {
		for ( i = 0 ; i < 9 ; i++ ) {
			lvColumn.cx = szSize_M[i];
			lvColumn.pszText = szString[i];
			//
//			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}
	}
	else {
		for ( i = 0 ; i < 9 ; i++ ) {
			lvColumn.cx = szSize[i];
			lvColumn.pszText = szString[i];
			//
//			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}	
	}
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL APIENTRY LogView_Open_Full_Method_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int i , j , k , z , l;
	char FileName[256];
	char FileName_org[256];
	char Buffer[512];
	char Buffer2[16];
	char MethodName[64];
	char Index[64];
	char Chamber[64];
	char Time1[64];
	char Time2[64];
	char Time3[64];
//	char Recipe[256]; // 2008.02.28
	char Recipe[2560]; // 2008.02.28
	static char jd[256];
	BOOL bres;
	FILE *fpt;

	RECT rect;		// crkim
	//
	int encflag;
	//
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
			KWIN_Item_Hide( hdlg , IDC_GRAPH_BUTTON );
			KWIN_Item_Hide( hdlg , IDRETRY );
			if ( !Get_Prm_SCROLLBUTTON_MODE() ) {
				KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
			}
			//>>-----------------------------------------------------------
			MoveCenterWindow( hdlg );
			//
			LogView_Open_Full_Method_Init( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
			l = JOB_Get_Current_Count();
			//
			for ( i = 0 , j = 0 , k = 0 ; k < l ; k++ ) {
				//
//				strncpy( jd , JOB_Get_Directory( k ) , 255 ); // 2017.03.13
				JOB_Get_Directory( k , jd ); // 2017.03.13
				//
				//----------------------------------------------------------------------------------------------
				sprintf( FileName_org , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , jd , METHOD_LOG_FILE );
				//----------------------------------------------------------------------------------------------
				if ( !Get_Prm_NETMONITOR_MODE() ) {
					strcpy( FileName , FileName_org );
				}
				else { // 2003.12.18
					sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
					_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
					_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
				}
				//-----------------------------------------------------------------
				// 2007.07.31
				//-----------------------------------------------------------------
//				if ( !Check_File_is_Open_Possible_Check( FileName ) ) {
//					MODAL_WINDOW_MESSAGE_BOX( hdlg , "File is not properly to Open(Reject)" , FileName , MB_ICONHAND );
//					EndDialog( hdlg , TRUE );
//					return TRUE;
//				}
				//-----------------------------------------------------------------
//				if ( ( fpt = fopen( FileName , "r+t" ) ) == NULL ) continue; // 2005.03.22
				if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) continue; // 2005.03.22
				//
				encflag = 0; // 2011.12.07
				//
				do {
					bres = Read_Line_From_File4( fpt , Buffer , 511 );
					if ( *Buffer != 0 ) {
						//
//						Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
						Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
						//
						z = 0;
						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , MethodName , z , 63 );
						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Index , z , 63 );
						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Chamber , z , 63 );
						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time1 , z , 63 );
						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time2 , z , 63 );
						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time3 , z , 63 );
//						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Recipe , z , 255 ); // 2008.02.28
						z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Recipe , z , 2559 ); // 2008.02.28
						//
						if ( Chamber[0] == '$' ) { // 2009.07.21
							strcpy( Buffer2 , Chamber + 1 );
							strcpy( Chamber , Buffer2 );
						}
						//
						if ( strlen( MethodName ) > 0 ) {
							i++;
							sprintf( Buffer2 , "%d" , i );
						}
						else {
							sprintf( Buffer2 , "" );
						}
						LogView_Open_Full_Method_Insert( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , j , Buffer2 , MethodName , Index , Chamber , Time1 , Time2 , Time3 , Recipe , Buffer + z );
						j++;
						//
					}
				}
				while ( bres );
				fclose( fpt );
			}
			return TRUE;
			
		case WM_NOTIFY:
			LogView_Open_Method_History_Notify( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam );
			return TRUE;

		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
			case IDCANCEL :
				EndDialog( hdlg , 0 );
				return TRUE;

			case IDC_CTRL_SDOWN :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * i );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , j );
				return TRUE;

			case IDC_CTRL_SUP :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * i );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , -j );
				return TRUE;

			case IDC_CTRL_RIGHT :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.right - rect.left;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;

			case IDC_CTRL_LEFT :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.left - rect.right;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;
			}
			return TRUE;

	}
	return FALSE;
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
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL LogView_Open_Full_Insert( HWND hWnd , char *Msg , char *Msg2 , char *Msg3 , char *Msg4 ) {
	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= 0;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
	lvi.cchTextMax	= 100;
	lvi.iImage		= 0;
	lvi.lParam		= 0;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	ListView_SetItemText( hWnd , 0 , 1 , Msg2 );
	ListView_SetItemText( hWnd , 0 , 2 , Msg3 );
	ListView_SetItemText( hWnd , 0 , 3 , Msg4 );
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//
BOOL LogView_Open_Full_Error_Message_Init( HWND hwndListView ) {
	int         i;
	LV_COLUMN   lvColumn;
	TCHAR       *szString[] = { "No" , "Time" , "Function" , "Context" };
	int         szSize[4]   = {  55  ,     210 ,   250 ,  700 };
	int         szSize_M[4] = {  45  ,     140 ,   165 ,  700 };
	//
	ListView_DeleteAllItems( hwndListView );
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	if ( Get_Prm_MINISCREEN_MODE() ) {
		for ( i = 0 ; i < 4 ; i++ ) {
			lvColumn.cx = szSize_M[i];
			lvColumn.pszText = szString[i];
			//
//			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}
	}
	else {
		for ( i = 0 ; i < 4 ; i++ ) {
			lvColumn.cx = szSize[i];
			lvColumn.pszText = szString[i];
			//
//			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}
	}
	return TRUE;
}
//
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL APIENTRY LogView_Open_Full_Error_Message_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int i , j , z;
	static char FileName[256];
	static char FileName_org[256];
	char Buffer[512];
	char Buffer1[64];
	char Buffer2[64];
	char Buffer3[16];
	BOOL bres;
	FILE *fpt;

	RECT rect;		// crkim

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
			KWIN_Item_Hide( hdlg , IDC_GRAPH_BUTTON );
			KWIN_Item_Hide( hdlg , IDRETRY );
			if ( !Get_Prm_SCROLLBUTTON_MODE() ) {
				KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
			}
			//>>-----------------------------------------------------------
			MoveCenterWindow( hdlg );
			//----------------------------------------------------------------------------------------------
			sprintf( FileName_org , "History/CimSqCon.log" );
			//----------------------------------------------------------------------------------------------
			if ( !Get_Prm_NETMONITOR_MODE() ) {
				strcpy( FileName , FileName_org );
			}
			else { // 2003.12.18
				sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
				_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
				_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
			}
			//-----------------------------------------------------------------
			// 2007.07.31
			//-----------------------------------------------------------------
//			if ( !Check_File_is_Open_Possible_Check( FileName ) ) {
//				MODAL_WINDOW_MESSAGE_BOX( hdlg , "File is not properly to Open(Reject)" , FileName , MB_ICONHAND );
//				EndDialog( hdlg , wParam );
//				return TRUE;
//			}
			//-----------------------------------------------------------------
			//-----------------------------------------------------------------
//			if ( ( fpt = fopen( FileName , "r+t" ) ) == NULL ) { // 2005.03.22
			if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) { // 2005.03.22
				sprintf( Buffer , "[%s] File Not Found" , FileName );
				MODAL_WINDOW_MESSAGE_BOX( hdlg , Buffer , "Open Error [6]", MB_ICONHAND );
				EndDialog( hdlg , wParam );
				return FALSE;
			}
			//
			LogView_Open_Full_Error_Message_Init( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
			i = 0;
			do {
				bres = Read_Line_From_File4( fpt , Buffer , 511 );
				if ( *Buffer != 0 ) {
					//
					i++;
					z = 0;
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , z , 63 );
					z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z , 63 );
					sprintf( Buffer3 , "%d" , i );
					LogView_Open_Full_Insert( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , Buffer3 , Buffer2 , Buffer1 , Buffer + z );
				}
			}
			while ( bres );
			fclose( fpt );
			return TRUE;
			
		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
			case IDCANCEL :
				EndDialog( hdlg , 0 );
				return TRUE;

			case IDC_CTRL_SDOWN :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * i );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , j );
				return TRUE;

			case IDC_CTRL_SUP :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * i );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , -j );
				return TRUE;

			case IDC_CTRL_RIGHT :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.right - rect.left;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;

			case IDC_CTRL_LEFT :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.left - rect.right;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Change_9_to_Space4( char *trg , char *src ) { // 2010.04.15
	int i , l;
	for ( i = 0 , l = 0 ; ; i++ ) {
		//
		if ( src[i] == 0 ) break;
		//
		if ( src[i] == 9 ) {
			trg[l] = ' ';	l++;
			trg[l] = ' ';	l++;
			trg[l] = ' ';	l++;
			trg[l] = ' ';	l++;
		}
		else {
			trg[l] = src[i];	l++;
		}
		//
	}
	//
	trg[l] = 0;
}

/*
// 2014.02.13
BOOL LogView_Open_Recipe_Log_Data_Set_Sub( HWND hdlg , int winpos , char *FileName_org , char *FileName , int mode ) {
	int i , s , j , sb;
	int col_insert;
	int col_step;

	char Buffer[256];
	char Buffer1[256];
	char Buffer2[256];
	char Buffer3[256];
	char Buffer4[256];

	BOOL bres;
	HWND hwndListView;
	FILE *fpt;
	LV_COLUMN   lvColumn;
	LV_ITEM lvi;
	TCHAR       *szString[] = { "No" , "Item"  , "Item" , "Data"  };
	int         szSize[4]   = {  40  ,  120    ,   160  , 325     };
	//
	int encflag;
	//----------------------------------------------------------------------------------------------
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else {
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
	}
	//-----------------------------------------------------------------
	if ( Process_Log_File_Is_Empty( FileName ) ) {
		sprintf( Buffer , "[%s] Data is Nothing" , FileName );
		MODAL_WINDOW_MESSAGE_BOX( hdlg , Buffer , "Open Error", MB_ICONHAND );
		return FALSE;
	}
	//-----------------------------------------------------------------
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) {
		sprintf( Buffer , "[%s] File Not Found" , FileName );
		MODAL_WINDOW_MESSAGE_BOX( hdlg , Buffer , "Open Error", MB_ICONHAND );
		return FALSE;
	}
	//
	hwndListView = GetDlgItem( hdlg , winpos );
	//
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break; // 2011.06.16
	//
	//
	if ( mode == 0 ) {
		//
		ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT );
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		for ( i = 0 ; i < 4 ; i++ ) {
			lvColumn.cx = szSize[i];
			lvColumn.pszText = szString[i];
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}
	}
	else {
		//
//		ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT );
		//
		ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 40;
		lvColumn.pszText = "No";
		ListView_InsertColumn( hwndListView , 0 , &lvColumn );
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 160;
		lvColumn.pszText = "Item";
		ListView_InsertColumn( hwndListView , 1 , &lvColumn );
		//
	}
	//
	s = -1;
	//
	col_insert = -1;
	col_step = -1;
	//
	i = 0;
	//
	encflag = 0; // 2011.12.07
	//
	do {
		//
		bres = Read_Line_From_File4( fpt , Buffer , 255 );
		//
//		Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
		Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
		//
		if ( *Buffer != 0 ) {
			//
			STR_SEPERATE_CHAR3( Buffer , 9 , Buffer1 , Buffer2 , Buffer3 , 255 );
			//
			sb = 0;
			//
			if ( STRCMP_L( Buffer1 , "STEP_BEGIN" ) ) {
				//
				sb = 1;
				//
				j = atoi( Buffer2 );
				//
				col_step = j - 1;
				//
				if ( j == 0 ) {
					s = 0;
				}
				else if ( ( j % 2 ) == 1 ) {
					s = 1;
				}
				else {
					s = 2;
				}
				//
				if ( mode != 0 ) {
					//
					if ( col_insert < col_step ) {
						//
						col_insert++;
						//
						lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
						lvColumn.fmt = LVCFMT_LEFT;
						lvColumn.cx = 100;
						//
						sprintf( Buffer4 , "Step%d" , col_insert + 1 );
						//
						lvColumn.pszText = Buffer4;
						ListView_InsertColumn( hwndListView , col_insert + 2 , &lvColumn );
					}
					//
					continue;
				}
				//
			}
			//
			if ( mode == 0 ) {
				//
				sprintf( Buffer4 , "%d" , i + 1 );
				//
				lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
				lvi.state = lvi.stateMask = 0;
				lvi.iItem		= i;
				lvi.iSubItem	= 0;
				lvi.pszText		= Buffer4;
				lvi.cchTextMax	= 100;
				lvi.iImage		= i;
				//
				if ( sb == 1 ) {
					lvi.lParam		=  ( i * 10000 ) + 70;
				}
				else {
					switch( s ) {
					case -1 :	lvi.lParam		= ( i * 10000 ) +  0;	break;
					case  0 :	lvi.lParam		= ( i * 10000 ) + 60;	break;
					case  1 :	lvi.lParam		= ( i * 10000 ) + 40;	break;
					case  2 :	lvi.lParam		= ( i * 10000 ) + 50;	break;
					default :	lvi.lParam		= ( i * 10000 ) +  0;	break;
					}
				}
				//
				if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) break;
				ListView_SetItemText( hwndListView , i , 1 , Buffer1 );
				ListView_SetItemText( hwndListView , i , 2 , Buffer2 );
				ListView_SetItemText( hwndListView , i , 3 , Buffer3 );
				//
			}
			else {
				//
				if ( col_step < 0 ) continue;
				//
				if ( STRCMP_L( Buffer1 , "STEP_END" ) ) continue;
				//
				s = ListView_GetItemCount( hwndListView );
				//
				for ( i = 0 ; i < s ; i++ ) {
					ListView_GetItemText( hwndListView , i , 1 , Buffer4 , 255 );
					if ( STRCMP_L( Buffer2 , Buffer4 ) ) break;
				}
				//
				if ( i == s ) {
					//
					sprintf( Buffer4 , "%d" , s + 1 );
					//
					lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
					lvi.state = lvi.stateMask = 0;
					lvi.iItem		= s;
					lvi.iSubItem	= 0;
					lvi.pszText		= Buffer4;
					lvi.cchTextMax	= 100;
					lvi.iImage		= s;
					//
					lvi.lParam		= ( i * 10000 ) + 7000;
					//
					if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) break;
					//
				}
				//
				ListView_SetItemText( hwndListView , i , 1 , Buffer2 );
				ListView_SetItemText( hwndListView , i , col_step + 2 , Buffer3 );
				//
			}
			//
			i++;
		}
	}
	while ( bres );
	fclose( fpt );
	return TRUE;
}

BOOL LogView_Open_Recipe_Log_Data_Set( HWND hdlg , int winpos , char *FileName_org , char *FileName , int mode ) {
	BOOL Res;
	Data_Read_Start( hdlg );
	Res = LogView_Open_Recipe_Log_Data_Set_Sub( hdlg , winpos , FileName_org , FileName , mode );
	Data_Read_End( hdlg );
	return Res;
}
*/

// 2014.02.13
BOOL LogView_Open_Recipe_Log_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName , int mode , int option2 , int option3 , int option4 , int option5 , int option6 ) {
	int i , s , j , sb , si;
	int col_insert;
	int col_step;
	int itemsize;
	char itemstr[512][64];

	char Buffer[256];
	char Buffer1[256];
	char Buffer2[256];
	char Buffer3[256];
	char Buffer4[256];

	BOOL bres;
	FILE *fpt;
	LV_COLUMN   lvColumn;
	LV_ITEM lvi;
	TCHAR       *szString[] = { "No" , "Item"  , "Item" , "Data"  };
	int         szSize[4]   = {  40  ,  120    ,   160  , 325     };
	//
	int encflag;
	//
	DWORD Log_Size;
	int readline;
	//----------------------------------------------------------------------------------------------
	if ( Process_Log_File_Is_Empty( FileName ) ) {
		sprintf( Buffer , "[%s] Data is Nothing" , FileName );
		MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error [7]", MB_ICONHAND );
		return FALSE;
	}
	//-----------------------------------------------------------------
	Log_Size = Process_Log_File_Get_Size( FileName );
	//
	sprintf( Buffer , "READING [%s] : %.2f Mb" , FileName , (double) Log_Size / 1024000.0 );
	LOG_OPEN_MESSAGE( Buffer );
	//
	readline = 0;
	//-----------------------------------------------------------------
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) {
		sprintf( Buffer , "[%s] File Not Found" , FileName );
		MODAL_WINDOW_MESSAGE_BOX( hWnd , Buffer , "Open Error [8]", MB_ICONHAND );
		return FALSE;
	}
	//
	if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
	//
	ListView_DeleteAllItems( hlistview );
	while ( TRUE ) if ( !ListView_DeleteColumn( hlistview , 0 ) ) break; // 2011.06.16
	//
	col_insert = -1;
	//
	if ( mode == 0 ) {
		//
		ListView_SetExtendedListViewStyleEx( hlistview , LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT );
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		for ( i = 0 ; i < 4 ; i++ ) {
			lvColumn.cx = szSize[i];
			lvColumn.pszText = szString[i];
			//
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hlistview , i , &lvColumn );
		}
	}
	else {
		//
//		ListView_SetExtendedListViewStyleEx( hlistview , LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT );
		//
		ListView_SetExtendedListViewStyleEx( hlistview , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 40;
		lvColumn.pszText = "No";
		//
		if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
		//
		ListView_InsertColumn( hlistview , 0 , &lvColumn );
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 160;
		lvColumn.pszText = "Item";
		//
		if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
		//
		ListView_InsertColumn( hlistview , 1 , &lvColumn );
		//
	}
	//
	if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
	//
	s = -1;
	//
	col_step = -1;
	itemsize = 0;
	//
	i = 0;
	si = 0;
	//
	encflag = 0; // 2011.12.07
	//
	do {
		//
		bres = Read_Line_From_File4( fpt , Buffer , 255 );
		//
//		Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
		Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
		//
		if ( *Buffer != 0 ) {
			//
			STR_SEPERATE_CHAR3( Buffer , 9 , Buffer1 , Buffer2 , Buffer3 , 255 );
			//
			sb = 0;
			//
			if ( STRCMP_L( Buffer1 , "STEP_BEGIN" ) ) {
				//
				si = 1;
				//
				sb = 1;
				//
				j = atoi( Buffer2 );
				//
				col_step = j - 1;
				//
				if ( j == 0 ) {
					s = 0;
				}
				else if ( ( j % 2 ) == 1 ) {
					s = 1;
				}
				else {
					s = 2;
				}
				//
				if ( mode != 0 ) {
					//
					if ( col_insert < col_step ) {
						//
						col_insert++;
						//
						if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
						//
						lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
						lvColumn.fmt = LVCFMT_LEFT;
						lvColumn.cx = 100;
						//
						sprintf( Buffer4 , "Step%d" , col_insert + 1 );
						//
						lvColumn.pszText = Buffer4;
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hlistview , col_insert + 2 , &lvColumn );
						//
						if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
						//
					}
					//
					continue;
				}
				//
			}
			//
			if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
			//
			if ( mode == 0 ) {
				//
				sprintf( Buffer4 , "%d" , i + 1 );
				//
				lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
				lvi.state = lvi.stateMask = 0;
				lvi.iItem		= i;
				lvi.iSubItem	= 0;
				lvi.pszText		= Buffer4;
				lvi.cchTextMax	= 100;
				lvi.iImage		= i;
				//
				if ( sb == 1 ) {
					lvi.lParam		=  ( i * 10000 ) + 70;
				}
				else {
					switch( s ) {
					case -1 :	lvi.lParam		= ( i * 10000 ) +  0;	break;
					case  0 :	lvi.lParam		= ( i * 10000 ) + 60;	break;
					case  1 :	lvi.lParam		= ( i * 10000 ) + 40;	break;
					case  2 :	lvi.lParam		= ( i * 10000 ) + 50;	break;
					default :	lvi.lParam		= ( i * 10000 ) +  0;	break;
					}
				}
				//
				if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
					fclose( fpt );
					if ( !xListView_Leave() ) {	return FALSE;	}
					return FALSE;
				}
				ListView_SetItemText( hlistview , i , 1 , Buffer1 );
				ListView_SetItemText( hlistview , i , 2 , Buffer2 );
				ListView_SetItemText( hlistview , i , 3 , Buffer3 );
				//
			}
			else {
				//
				if ( col_step >= 0 ) {
					//
					if ( STRCMP_L( Buffer1 , "STEP_END" ) ) {
						si = 0;
					}
					else {
						//
						if ( si == 1 ) {
							//
							s = strlen( Buffer2 );
							//
							if ( s < 64 ) {
								for ( j = 0 ; j < itemsize ; j++ ) {
									//
									if ( j < 512 ) {
										if ( STRCMP_L( Buffer2 , itemstr[j] ) ) break;
									}
									else {
										ListView_GetItemText( hlistview , j , 1 , Buffer4 , 255 );
										if ( STRCMP_L( Buffer2 , Buffer4 ) ) break;
									}
									//
								}
							}
							else {
								for ( j = 0 ; j < itemsize ; j++ ) {
									ListView_GetItemText( hlistview , j , 1 , Buffer4 , 255 );
									if ( STRCMP_L( Buffer2 , Buffer4 ) ) break;
								}
							}
							//
							if ( j == itemsize ) {
								//
								sprintf( Buffer4 , "%d" , itemsize + 1 );
								//
								lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
								lvi.state = lvi.stateMask = 0;
								lvi.iItem		= itemsize;
								lvi.iSubItem	= 0;
								lvi.pszText		= Buffer4;
								lvi.cchTextMax	= 100;
								lvi.iImage		= itemsize;
								//
								lvi.lParam		= ( itemsize * 10000 ) + 7000;
								//
								if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
									fclose( fpt );
									if ( !xListView_Leave() ) {	return FALSE;	}
									return FALSE;
								}
								//
								ListView_SetItemText( hlistview , itemsize , 1 , Buffer2 );
								//
								if ( itemsize < 512 ) {
									strncpy( itemstr[itemsize] , Buffer2 , 63 );	itemstr[itemsize][63] = 0;
								}
								itemsize++;
								//
							}
							//
							ListView_SetItemText( hlistview , j , col_step + 2 , Buffer3 );
							//
						}
					}
				}
			}
			//
			if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
			//
			i++;
			//=========================================================================================
			if ( sepload == 1 ) { // 2014.02.06
				//
				readline++;
				//
				if ( readline >= SEP_ONE_TIME_MAX_READ_ITEM_COUNT ) {
					//
					readline = 0;
					//
					sprintf( Buffer2 , "READING [%s] : %d Line(s) : %.2f Mb / %.2f Mb" , FileName , i , (double) ftell( fpt ) / 1024000.0 , (double) Log_Size / 1024000.0 );
					//
					LOG_OPEN_MESSAGE( Buffer2 );
					//
				}
			}
			//=========================================================================================
		}
	}
	while ( bres );
	fclose( fpt );
	//=========================================================================================
	return TRUE;
}

BOOL LogView_Open_Recipe_Log_Data_Set( HWND hdlg , int winpos , char *FileName_org , char *FileName , int mode , BOOL sepload ) {
	//-----------------------------------------------------------------
	BOOL Res;
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_Start( hdlg );
	//-----------------------------------------------------------------
	//
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else {
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
	}
	//
	Res = LOG_OPEN_CONTROL( hdlg , GetDlgItem( hdlg , winpos ) , sepload , FileName , mode , 0 , 0 , 0 , 0 , 0 , LogView_Open_Recipe_Log_Data_Set_Callback );
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_End( hdlg );
	//-----------------------------------------------------------------
	return Res;
	//-----------------------------------------------------------------
}


BOOL APIENTRY LogView_Open_Recipe_Log_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int i;
	static char FileName[256];
	static char FileName_org[256];
//	char Buffer[256]; // 2015.07.29
//	char Buffer2[128]; // 2009.11.20
//	char Buffer2[256]; // 2009.11.20
	char Buffer[1025]; // 2015.07.29
	char Buffer2[1025]; // 2009.11.20 // 2015.07.29
	char Buffer3[1025]; // 2015.07.29
	char Buffer4[1025]; // 2015.07.29

	char folderstr[256];

	BOOL bres;
	FILE *fpt;
	ViewStepTemplate *readdata;
	int encflag;

	LV_COLUMN   lvColumn; // 2015.07.29
	LV_ITEM lvi; // 2015.07.29
	LPDRAWITEMSTRUCT dis; // 2015.07.29
	LPMEASUREITEMSTRUCT dim; // 2015.07.29

	switch ( msg ) {
		case WM_MEASUREITEM: // 2015.07.29
			//
			//
			if ( ( GLISTVIEW_HEIGHT > 0 ) && ( ( GLISTVIEW_HEIGHT % 1000 ) >= 10 ) ) {
				//
				dim = (LPMEASUREITEMSTRUCT) lParam;
				//
				if ( dim->CtlType == ODT_LISTVIEW ) {
					if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
						 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
					}
				}
				//
			}
			else {
				if ( LISTVIEW_HEIGHT_UPDATE ) {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
							 dim->itemHeight = LISTVIEW_HEIGHT;
						}
					}
					//
				}
			}
			//
			return TRUE;

		case WM_DRAWITEM: // 2015.07.29
			//
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
			ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , -1 , 0 , 1 , "" , Buffer2 , 1024 , 0 , 0 , -1 , -1 , -1 , -1 );
			//
			break;

		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			MoveCenterWindow( hdlg );
			readdata = (ViewStepTemplate *) (LPARAM) lParam;
			//>>-----------------------------------------------------------
			if ( !Get_Prm_SAVEFILE_MODE() ) KWIN_Item_Hide( hdlg , IDRETRY );
			//>>-----------------------------------------------------------
			//----------------------------------------------------------------------------------------------
			// 2003.09.16
			//----------------------------------------------------------------------------------------------
			if ( JOB_Get_Current_Count() <= readdata->index ) {
				MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
				EndDialog( hdlg , FALSE );
				return FALSE;
			}
			if ( JOB_Get_Directory_Type( readdata->index ) != 1 ) {
				MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed & Data not exist" , "Try Again" , MB_ICONHAND );
				EndDialog( hdlg , FALSE );
				return FALSE;
			}
			//----------------------------------------------------------------------------------------------
			JOB_Get_Directory( readdata->index , folderstr );
			//
			Get_Log_File_Name( FileName_org , "RECIPE" , readdata->mode , folderstr , FALSE , 0 , 0 , readdata->slot , 0 , readdata->chstr , "" );
			//----------------------------------------------------------------------------------------------
			if ( !Get_Prm_NETMONITOR_MODE() ) {
				strcpy( FileName , FileName_org );
			}
			else { // 2003.12.18
				sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
				_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
				_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
			}
			//-----------------------------------------------------------------
			// 2007.07.31
			//-----------------------------------------------------------------
//			if ( !Check_File_is_Open_Possible_Check( FileName ) ) {
//				MODAL_WINDOW_MESSAGE_BOX( hdlg , "File is not properly to Open(Reject)" , FileName , MB_ICONHAND );
//				EndDialog( hdlg , TRUE );
//				return TRUE;
//			}
			//-----------------------------------------------------------------
			//-----------------------------------------------------------------
//			if ( ( fpt = fopen( FileName , "r+t" ) ) == NULL ) { // 2005.03.22
			if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) { // 2005.03.22
				sprintf( Buffer , "[%s] File Not Found" , FileName );
				MODAL_WINDOW_MESSAGE_BOX( hdlg , Buffer , "Open Error [9]", MB_ICONHAND | MB_OK );
				EndDialog( hdlg , TRUE );
				return TRUE;
			}
			JOB_Get_Display_String( readdata->index , Buffer2 );
			sprintf( Buffer , "[%s] %s [%s]" , readdata->str , Buffer2 , FileName );
			SetWindowText( hdlg , Buffer );
			//
			KWIN_Item_String_Display( hdlg , IDC_LIST_METHOD_ID , readdata->slot );
			//
			//
			// 2015.07.29
			//
			lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvColumn.fmt = LVCFMT_LEFT;
			//
			lvColumn.cx = 40;
			lvColumn.pszText = "No";
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &lvColumn );
			//
			lvColumn.cx = 128;
			lvColumn.pszText = "ITEM1";
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 1 , &lvColumn );
			//
			lvColumn.cx = 200;
			lvColumn.pszText = "ITEM2";
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 2 , &lvColumn );
			//
			lvColumn.cx = 512;
			lvColumn.pszText = "DATA";
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 3 , &lvColumn );
			//
			//
			//
			i = 0;
			encflag = 0; // 2011.12.07
			//
			do {
				//
//				bres = Read_Line_From_File( fpt , Buffer ); // 2010.04.15
				bres = Read_Line_From_File4( fpt , Buffer , 255 ); // 2010.04.15
				//
//				Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
				Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
				//
				if ( *Buffer != 0 ) {
					//
					Change_9_to_Space4( Buffer2 , Buffer ); // 2010.04.15
					//
//					SendDlgItemMessage( hdlg , IDC_LIST_LOGVIEW , LB_ADDSTRING , 0 , (LPARAM) Buffer ); // 2010.04.15
//					SendDlgItemMessage( hdlg , IDC_LIST_LOGVIEW , LB_ADDSTRING , 0 , (LPARAM) Buffer2 ); // 2010.04.15 // 2015.07.29
					//
					//
					// 2015.07.29
					//
					//
					STR_SEPERATE( Buffer2 , Buffer3 , Buffer , 1024 );
					STR_SEPERATE( Buffer , Buffer4 , Buffer2 , 1024 );
					//
					//
					sprintf( Buffer , "%d" , i + 1 );
					//
					lvi.mask  = LVIF_TEXT;
					lvi.state = lvi.stateMask = 0;
					lvi.iItem		= i;
					lvi.iSubItem	= 0;
					lvi.pszText		= Buffer;
					lvi.cchTextMax	= 256;
					lvi.lParam		= 0;
					//
					if ( ListView_InsertItem( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , &lvi ) == -1 ) break;
					//
					ListView_SetItemText( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , i , 1 , Buffer3 );
					ListView_SetItemText( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , i , 2 , Buffer4 );
					ListView_SetItemText( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , i , 3 , Buffer2 );
					//
					//
					//
					i++;
				}
			}
			while ( bres );
			fclose( fpt );
			wsprintf( Buffer , "%d" , i );
			SendDlgItemMessage( hdlg , IDC_LIST_TOTAL , WM_SETTEXT , 0 , (LPARAM) Buffer );
			return TRUE;
			
		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			//
			// 2015.07.29
			//
//			if ( GET_WM_COMMAND_CMD( wParam , lParam ) == LBN_SELCHANGE ) {
//				i = (int) SendDlgItemMessage( hdlg , IDC_LIST_LOGVIEW , LB_GETCURSEL , 0 , 0L );
//				wsprintf( Buffer , "%d" , i + 1 );
//				SendDlgItemMessage( hdlg , IDC_LIST_CURRENT , WM_SETTEXT , 0 , (LPARAM) Buffer );
//				return TRUE;
//			}
			switch (wParam) {
				case IDRETRY :
					if ( Log_Get_Save_File_Name( hdlg , Buffer ) ) {
						if ( !SAVE_FILE( FileName , Buffer , FALSE ) ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] File Save Error!" , Buffer , MB_ICONHAND );
						}
					}
					return TRUE;

				case IDCANCEL :
					EndDialog( hdlg , TRUE );
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
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Make_Index_to_UseIndex( char *in , char *out , int *sl1 , int *sl2 ) {
	char Buffer1[32];
	char Buffer2[32];
	STR_SEPERATE_CHAR( in , '_' , Buffer1 , Buffer2 , 31 );
	if ( Buffer1[0] == 'A' ) { // 2007.11.07
		if ( strlen( Buffer2 ) > 0 ) {
			if ( Buffer2[0] == 'D' ) { // 2007.11.07
				sprintf( out , "A%02d_D%02d" , atoi( Buffer1 + 1 ) , atoi( Buffer2 + 1 ) );
				*sl1 = atoi( Buffer1 + 1 ) + 100;
				*sl2 = atoi( Buffer2 + 1 ) + 100;
			}
			else {
				sprintf( out , "A%02d_%02d" , atoi( Buffer1 + 1 ) , atoi( Buffer2 ) );
				*sl1 = atoi( Buffer1 + 1 ) + 100;
				*sl2 = atoi( Buffer2 );
			}
		}
		else {
			sprintf( out , "A%02d" , atoi( Buffer1 + 1 ) );
			*sl1 = atoi( Buffer1 + 1 ) + 100;
			*sl2 = 0;
		}
	}
	else {
		if ( strlen( Buffer2 ) > 0 ) {
			if ( Buffer2[0] == 'D' ) { // 2007.11.07
				sprintf( out , "%02d_D%02d" , atoi( Buffer1 ) , atoi( Buffer2 + 1 ) );
				*sl1 = atoi( Buffer1 );
				*sl2 = atoi( Buffer2 + 1 ) + 100;
			}
			else {
				sprintf( out , "%02d_%02d" , atoi( Buffer1 ) , atoi( Buffer2 ) );
				*sl1 = atoi( Buffer1 );
				*sl2 = atoi( Buffer2 );
			}
		}
		else {
			sprintf( out , "%02d" , atoi( Buffer1 ) );
			*sl1 = atoi( Buffer1 );
			*sl2 = 0;
		}
	}
}
//---------------------------------------------------------------------------------------
int Make_Chamber_to_UseChamber( char *in ) {
	int i , l = strlen( in ) , pmc;
	pmc = atoi( in + 2 ); // 2007.08.03
	for ( i = 0 ; i < l ; i++ ) {
		if ( in[i] == '(' ) { in[i] = 0; return pmc; }
	}
	return pmc;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Make_Estimate_LogData_for_Min_Max_Avr( BOOL UserMode , int ReadMode , int Module , int Slot , int SchIndex , char *LotDirectory , char *Single_Slot , int WriteMode , BOOL Upload , int extview , BOOL sepload ) {
	//---------------------------------------------------
	// ReadMode == 0 : SingleFile    => Module Use
	// ReadMode == 1 : Module(Group) => Module Use
	// ReadMode == 2 : Lot(Group)
	//---------------------------------------------------
	// SchIndex < 0 : LotDirectoryUse
	//       else   : Lot Index
	//---------------------------------------------------
	// WriteMode = 0/1/2/3(Min/Max/Avr/All)
	//---------------------------------------------------
	int i , j , k , item , z1 , z2;

	char TempFileNameL[256];
	char TempFileName1[256];
	char TempFileName2[256];
	char TempFileName3[256];
	char TempFileName4[256];
	char LotDir[256];

	char InfoFileName[256];
	char OpenFileName[256];
	char InfoFileName_org[256];
	char OpenFileName_org[256];

	char Buffer[1024];
//	char Buffer1[1024]; // 2007.11.21
	char Buffer1[4097]; // 2007.11.21
	char Buffer2[256];
	char Buffer3[256];

	char S_Index[256];
	char S_Chamber[256];

	BOOL Find , ok;
	
	double Dat;

//	BOOL readfind[MAX_PM_CHAMBER_DEPTH][50]; // 2007.08.03 // 2007.11.07
	BOOL readfind[MAX_PM_CHAMBER_DEPTH][150]; // 2007.08.03 // 2007.11.07
	int  rch , rsl1 , rsl2;

//	int    Dat_Avr_All_Count = 0 , Dat_MinMax_All_Count = 0; // 2016.08.18
//	int    Dat_Avr_Count = 0 , Dat_MinMax_Count = 0; // 2016.08.18

	int sc , chkmode;
	BOOL bres;
	BOOL bres2;

	FILE *fpt;
	FILE *fpt_o;
	FILE *fpt_l;
	//
	char *SendBuffer;
	int encflag;
	int encflag2;

	int readline;
	int readfile;
	DWORD Log_Size;
	
	//============================================================================
	// 2007.08.03
	//============================================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
//		for ( j = 0 ; j < 50 ; j++ ) { // 2007.11.07
		for ( j = 0 ; j < 150 ; j++ ) { // 2007.11.07
			readfind[i][j] = FALSE;
		}
	}
	//============================================================================
	DATA_BUFFER_RESET( 0 ); // 2016.08.19
	//============================================================================
	if ( SchIndex < 0 )	{
		STR_SEPERATE_CHAR( LotDirectory , ':' , Buffer1 , Buffer , 255 );
		STR_SEPERATE_CHAR( Buffer , ':' , Buffer2 , Buffer3 , 255 );
		j = JOB_Find_Search( Buffer1 , Buffer2 , Buffer3 , TRUE );
		if ( j < 0 ) {
			strcpy( LotDir , LotDirectory );
		}
		else {
			//strcpy( LotDir , JOB_Get_Directory( j ) );
			JOB_Get_Directory( j , LotDir );
		}
	}
	else	{
		//strcpy( LotDir , JOB_Get_Directory( SchIndex ) );
		JOB_Get_Directory( SchIndex , LotDir );
	}
	sprintf( Buffer , "%s/LotLog/%s" , Get_Prm_LOG_DIRECTORY() , LotDir );
	//----------------------------------------------------------------------------------------------
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		if ( !Directory_Name_Exist_Check( Buffer ) ) return FALSE;
	}
	else { // 2003.12.18
		if ( _nINF_KNFS_ExistFolder( Buffer ) ) return FALSE;
	}
	//-----------------------------------------------------------------
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		if ( UserMode ) {
			sprintf( TempFileNameL , "%s/LotLog/Est_Lst.txt" , Get_Prm_LOG_DIRECTORY() );	remove( TempFileNameL );
			sprintf( TempFileName1 , "%s/LotLog/Est_Min.txt" , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName1 );
			sprintf( TempFileName2 , "%s/LotLog/Est_Max.txt" , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName2 );
			sprintf( TempFileName3 , "%s/LotLog/Est_Avr.txt" , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName3 );
			sprintf( TempFileName4 , "%s/LotLog/Est_Tot.txt" , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName4 );
		}
		else {
			sprintf( TempFileNameL , "%s/LotLog/%s/Templ.txt" , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileNameL );
			sprintf( TempFileName1 , "%s/LotLog/%s/Temp1.txt" , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName1 );
			sprintf( TempFileName2 , "%s/LotLog/%s/Temp2.txt" , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName2 );
			sprintf( TempFileName3 , "%s/LotLog/%s/Temp3.txt" , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName3 );
			sprintf( TempFileName4 , "%s/LotLog/%s/TempT.txt" , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName4 );
		}
	}
	else { // 2003.12.18
		if ( UserMode ) {
			sprintf( TempFileNameL , "%s/%s/LotLog/Est_Lst.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() );
			//
			_nINF_KNFS_PrepareLocalFolder_for_Filename( TempFileNameL );
			//			
			remove( TempFileNameL );
			sprintf( TempFileName1 , "%s/%s/LotLog/Est_Min.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName1 );
			sprintf( TempFileName2 , "%s/%s/LotLog/Est_Max.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName2 );
			sprintf( TempFileName3 , "%s/%s/LotLog/Est_Avr.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName3 );
			sprintf( TempFileName4 , "%s/%s/LotLog/Est_Tot.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() );	remove( TempFileName4 );
		}
		else {
			sprintf( TempFileNameL , "%s/%s/LotLog/%s/Templ.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , LotDir );
			//
			_nINF_KNFS_PrepareLocalFolder_for_Filename( TempFileNameL );
			//			
			remove( TempFileNameL );
			sprintf( TempFileName1 , "%s/%s/LotLog/%s/Temp1.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName1 );
			sprintf( TempFileName2 , "%s/%s/LotLog/%s/Temp2.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName2 );
			sprintf( TempFileName3 , "%s/%s/LotLog/%s/Temp3.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName3 );
			sprintf( TempFileName4 , "%s/%s/LotLog/%s/TempT.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , LotDir );	remove( TempFileName4 );
		}
	}
	if ( ReadMode != 0 ) {
		sprintf( InfoFileName_org , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , LotDir , METHOD_LOG_FILE );
		//----------------------------------------------------------------------------------------------
		if ( !Get_Prm_NETMONITOR_MODE() ) {
			strcpy( InfoFileName , InfoFileName_org );
		}
		else { // 2003.12.18
			sprintf( InfoFileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , InfoFileName_org );
			_nINF_KNFS_PrepareLocalFolder_for_Filename( InfoFileName );
			_nINF_KNFS_GetFile( InfoFileName_org , InfoFileName , FALSE );
		}
		//-----------------------------------------------------------------
		//-----------------------------------------------------------------
		if ( ( fpt = fopen( InfoFileName , "r" ) ) == NULL ) { // 2005.03.22
			return FALSE;
		}
		//
	}
	//===========================================================================
	// 2007.08.10
	//===========================================================================
	sc = 0; // 2007.08.10
	//===========================================================================
	encflag = 0; // 2011.12.07
	//
	readfile = 0;
	//
	do {
		Find = FALSE;
		if ( ReadMode != 0 ) {
			//
			bres = Read_Line_From_File4( fpt , Buffer , 1023 );
			//
			//===========================================================================
			// 2007.08.10
			//===========================================================================
			sc++;
			if ( sc >= MAX_READ_SLEEP_LINE ) {
//				_sleep(1); // 2015.04.10
				Sleep(1); // 2015.04.10
				sc = 0;
			}
			//===========================================================================
			if ( *Buffer != 0 ) {
				//
//				Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
				Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
				//
				z1 = 0;
				z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , z1 , 255 );
				//
				if ( strlen( Buffer1 ) <= 0 ) {
					//
					z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , S_Chamber , z1 , 255 );
					//
					Make_Index_to_UseIndex( S_Chamber , S_Index , &rsl1 , &rsl2 );
					//
					if ( ( Slot > 0 ) && ( Slot != rsl1 ) ) continue; // 2011.07.15
					//
					z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , S_Chamber , z1 , 255 );
					//
					if ( S_Chamber[0] == '$' ) continue; // 2009.07.21
					//
					rch = Make_Chamber_to_UseChamber( S_Chamber );
					//
					if      ( ( ReadMode == 1 ) && ( Module == atoi( S_Chamber + 2 ) ) ) Find = TRUE;
					else if ( ReadMode == 2 ) Find = TRUE;
				}
			}
			//===========================================================================
		}
		else {
			//
			strcpy( Buffer , "" );
			//
			Find = TRUE;
			//
			bres = FALSE;
			//
			strcpy( S_Index , Single_Slot );
			sprintf( S_Chamber , "PM%d" , Module );
		}
		//================================================================
		// 2007.08.03
		//================================================================
		if ( Find ) {
			if ( ReadMode != 0 ) {
				if ( ( rsl1 > 0 ) && ( rsl2 > 0 ) ) {
					if ( ( readfind[rch-1][rsl1-1] ) && ( readfind[rch-1][rsl2-1] ) ) {
						Find = FALSE;
					}
					readfind[rch-1][rsl1-1] = TRUE;
					readfind[rch-1][rsl2-1] = TRUE;
				}
				else if ( rsl1 > 0 ) {
					if ( readfind[rch-1][rsl1-1] ) {
						Find = FALSE;
					}
					readfind[rch-1][rsl1-1] = TRUE;
				}
			}
		}
		//================================================================
		if ( Find ) {
			//----------------------------------------------------------------------------------------------
			Get_Log_File_Name( OpenFileName_org , "PROCESS" , extview , LotDir , FALSE , 0 , 0 , S_Index , 0 , S_Chamber , "" );
			//----------------------------------------------------------------------------------------------
			if ( !Get_Prm_NETMONITOR_MODE() ) {
				strcpy( OpenFileName , OpenFileName_org );
			}
			else { // 2003.12.18
				sprintf( OpenFileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , OpenFileName_org );
				_nINF_KNFS_PrepareLocalFolder_for_Filename( OpenFileName );
				_nINF_KNFS_GetFile( OpenFileName_org , OpenFileName , FALSE );
			}
			//-----------------------------------------------------------------
			// 2007.07.31
			//-----------------------------------------------------------------
//			if ( !Check_File_is_Open_Possible_Check( OpenFileName ) ) {
//				return FALSE;
//			}
			//-----------------------------------------------------------------
			//-----------------------------------------------------------------
			i = 0;
			item = 0;
			//-----------------------------------------------------------------
			// 2007.06.25
			//-----------------------------------------------------------------
			if ( Process_Log_File_Is_Empty( OpenFileName ) ) {
				if ( ReadMode == 0 ) return FALSE;
			}
			else {
				//-----------------------------------------------------------------
				Log_Size = Process_Log_File_Get_Size( OpenFileName );
				readline = 0;
				readfile++;
				//-----------------------------------------------------------------
				ok = FALSE;
				//
				if ( ( fpt_o = fopen( OpenFileName , "r" ) ) != NULL ) { // 2005.03.22
					ok = TRUE;
				}
				//
				if ( ok ) {
					//
					ok = FALSE;
					//
					if ( ( fpt_l = fopen( TempFileNameL , "a" ) ) != NULL ) {
						ok = TRUE;
					}
					//
					if ( !ok ) {
						fclose( fpt_o );
					}
					else {
						//============================================================================
						// 2011.12.06
						//
						DATA_BUFFER_RESET( 0 ); // 2016.08.19
						//
						//============================================================================
						//
						encflag2 = 0; // 2011.12.07
						//
						do {
							//
							if ( LOG_OPEN_CANCELED() ) {
								fclose( fpt_l );
								fclose( fpt_o );
								return FALSE;
							}
							//
							bres2 = Read_Line_From_File4( fpt_o , Buffer1 , 4096 ); // 2007.11.21
							//
							//===========================================================================
							// 2007.08.10
							//===========================================================================
							sc++;
							if ( sc >= MAX_READ_SLEEP_LINE ) {
//								_sleep(1); // 2015.04.10
								Sleep(1); // 2015.04.10
								sc = 0;
							}
							//===========================================================================
							//
//							Make_Simple_Encryption( LOG_ENCRIPTION , Buffer1 ); // 2010.04.15
							Read_Simple_Encryption( &encflag2 , LOG_ENCRIPTION , Buffer1 ); // 2011.12.07
							//
							if ( *Buffer1 != 0 ) {
								if ( Buffer1[0] == '$' ) {
									//============================================================================================================
									// 2011.12.06
									if ( ( Buffer1[1] == 'S' ) && ( Buffer1[2] == 'T' ) && ( Buffer1[3] == 'E' ) && ( Buffer1[4] == 'P' ) ) {
										z2 = -1;
										for ( z1 = 5 ; ; z1++ ) {
											if      ( ( Buffer1[z1] == ' ' ) || ( Buffer1[z1] == 9 ) ) {
												if ( z2 != -1 ) {
													z2 = -1;
													break;
												}
											}
											else if ( ( Buffer1[z1] == '|' ) || ( Buffer1[z1] == 0 ) ) {
												break;
											}
											else if ( ( Buffer1[z1] >= '0' ) && ( Buffer1[z1] <= '9' ) ) {
												if ( z2 < 0 ) z2 = 0;
												z2 = ( z2 * 10 ) + ( Buffer1[z1] - '0' );
											}
											else {
												z2 = -1;
												break;
											}
										}
										if ( z2 >= 1 ) {
											Dat_Curr_Step = z2 - 1;
										}
									}
									//============================================================================================================
								}
								else {
									//
									if ( i == 0 ) {
										z1 = z2 = 0;
										if ( *Buffer1 != 0 ) {
											z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer1 , 9 , Buffer2 , z1 , 255 );
											//
											fprintf( fpt_l , "%s" , Buffer2 );
											//
											item = 0;
										}
										while ( TRUE ) {
											z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer1 , 9 , Buffer2 , z1 , 255 );
											if ( z2 != z1 ) {
												z1 = z2;
												//
												fprintf( fpt_l , "\t%s" , Buffer2 );
												//
												item++;
											}
											else {
												break;
											}
										}
										//
										fprintf( fpt_l , "\n" );
										//
									}
									else {
										j = -1;
										z1 = z2 = 0;
										chkmode = 0; // 2008.02.28
		//								while( j <= MAX_X_TABLE ) { // 2004.10.12
										while( j < MAX_X_TABLE ) { // 2004.10.12
											z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer1 , 9 , Buffer2 , z1 , 255 );
											if ( z2 != z1 ) {
												z1 = z2;
												if ( j >= 0 ) {
													//
													Dat = atof( Buffer2 );
													//
													Dat_Set_Step[ Dat_Curr_Step ][ j ] = TRUE; // 2011.12.06
													Dat_Set_StepUse[ Dat_Curr_Step ] = TRUE; // 2011.12.06
													//
													Dat_Set_StepAll[ j ] = TRUE;
													if ( ( chkmode == 0 ) || ( chkmode == 2 ) ) {
														//
														if ( Dat_Count_Sum_StepAll[j] == 0 ) Dat_First_StepAll[ j ] = Dat;
														Dat_Last_StepAll[ j ] = Dat;
														//
														Dat_Sum_StepAll[ j ] = Dat_Sum_StepAll[ j ] + Dat;
														Dat_Sum2_StepAll[ j ] = Dat_Sum2_StepAll[ j ] + (Dat*Dat); // 2016.08.18
														//
														Dat_Count_Sum_StepAll[j]++;
														//
														//----------------------------------------------------------------------
														//
														if ( Dat_Count_Sum_Step[ Dat_Curr_Step ][j] == 0 ) Dat_First_Step[ Dat_Curr_Step ][ j ] = Dat;
														Dat_Last_Step[ Dat_Curr_Step ][ j ] = Dat;
														//
														Dat_Sum_Step[ Dat_Curr_Step ][ j ] = Dat_Sum_Step[ Dat_Curr_Step ][ j ] + Dat; // 2011.12.06
														Dat_Sum2_Step[ Dat_Curr_Step ][ j ] = Dat_Sum2_Step[ Dat_Curr_Step ][ j ] + (Dat*Dat); // 2016.08.18
														//
														Dat_Count_Sum_Step[ Dat_Curr_Step ][j]++; // 2011.12.06
														//
													}
													if ( ( chkmode == 0 ) || ( chkmode == 1 ) ) {
														//
														if ( Dat_Min_StepAll[ j ] > Dat ) Dat_Min_StepAll[ j ] = Dat;
														if ( Dat_Max_StepAll[ j ] < Dat ) Dat_Max_StepAll[ j ] = Dat;
														//
														Dat_Count_MinMax_StepAll[j]++;
														//
														//----------------------------------------------------------------------
														//
														if ( Dat_Min_Step[ Dat_Curr_Step ][ j ] > Dat ) Dat_Min_Step[ Dat_Curr_Step ][ j ] = Dat; // 2011.12.06
														if ( Dat_Max_Step[ Dat_Curr_Step ][ j ] < Dat ) Dat_Max_Step[ Dat_Curr_Step ][ j ] = Dat; // 2011.12.06
														//
														Dat_Count_MinMax_Step[ Dat_Curr_Step ][j]++; // 2011.12.06
														//
													}
												}
												else {
													chkmode = Log_Get_String_ChkMode_from_Time( Buffer2 ); // 2008.02.28
												}
												j++;
											}
											else {
												break;
											}
										}
										//=========================================================================================
										//
										Dat_Count_Total++; // 2016.08.18
										Dat_Count_Total_Step[ Dat_Curr_Step ]++; // 2016.08.18
										//
									}
									//
									i++;
									//
									//=========================================================================================
									if ( sepload ) { // 2014.02.06
										//
										readline++;
										//
										if ( readline >= SEP_ONE_TIME_MAX_READ_ITEM_COUNT ) {
											//
											readline = 0;
											//
											sprintf( Buffer2 , "READING : %d Line(s) : %.2f Mb / %.2f Mb : File [%d]" , i , (double) ftell( fpt_o ) / 1024000.0 , (double) Log_Size / 1024000.0 , readfile );
											//
											LOG_OPEN_MESSAGE( Buffer2 );
											//
										}
									}
									//=========================================================================================
									//
								}
							}
						}
						while ( bres2 );
						//
						fclose( fpt_l );
						fclose( fpt_o );
						//
					}
				}
				else {
					if ( ReadMode == 0 ) return FALSE;
				}
			}
			if ( i > 1 ) {
//				Dat_Count = Dat_Count + i - 1; // 2008.02.28
				if ( ( WriteMode == 0 ) || ( WriteMode == 3 ) ) {
					//
					ok = FALSE;
					//
					if ( ( fpt_o = fopen( TempFileName1 , "a" ) ) != NULL ) {
						ok = TRUE;
					}
					//
					if ( ok ) {
						//
						j = 0;
						fprintf( fpt_o , "%s(%s)" , S_Chamber , S_Index );
						while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12
							if ( Dat_Count_MinMax_StepAll[ j ] <= 0 ) {
								fprintf( fpt_o , "\tN/A" ); // 2008.02.28
							}
							else {
								fprintf( fpt_o , "\t%.6lf" ,  Dat_Min_StepAll[ j ] );
							}
							j++;
						}
						fprintf( fpt_o , "\n" );
						//============================================================================
						// 2011.12.06
						for ( k = 0 ; k < MAX_STEP_COUNT ; k++ ) {
							if ( !Dat_Set_StepUse[k] ) continue;
							j = 0;
							fprintf( fpt_o , "$STEP%d" , k + 1 );
							while( ( Dat_Set_Step[k][j] ) && ( j < MAX_X_TABLE ) ) {
								if ( Dat_Count_MinMax_Step[k][j] <= 0 ) {
									fprintf( fpt_o , "\tN/A" );
								}
								else {
									fprintf( fpt_o , "\t%.6lf" ,  Dat_Min_Step[k][ j ] );
								}
								j++;
							}
							fprintf( fpt_o , "\n" );
						}
						//============================================================================
						fclose( fpt_o );
						//============================================================================
					}
				}
				if ( ( WriteMode == 1 ) || ( WriteMode == 3 ) ) {
					//
					ok = FALSE;
					//
					if ( ( fpt_o = fopen( TempFileName2 , "a" ) ) != NULL ) {
						ok = TRUE;
					}
					//
					if ( ok ) {
						//
						j = 0;
						fprintf( fpt_o , "%s(%s)" , S_Chamber , S_Index );
						while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12
							if ( Dat_Count_MinMax_StepAll[ j ] <= 0 ) {
								fprintf( fpt_o , "\tN/A" ); // 2008.02.28
							}
							else {
								fprintf( fpt_o , "\t%.6lf" ,  Dat_Max_StepAll[ j ] );
							}
							j++;
						}
						fprintf( fpt_o , "\n" );
						//============================================================================
						// 2011.12.06
						for ( k = 0 ; k < MAX_STEP_COUNT ; k++ ) {
							if ( !Dat_Set_StepUse[k] ) continue;
							j = 0;
							fprintf( fpt_o , "$STEP%d" , k + 1 );
							while( ( Dat_Set_Step[k][j] ) && ( j < MAX_X_TABLE ) ) {
								if ( Dat_Count_MinMax_Step[k][j] <= 0 ) {
									fprintf( fpt_o , "\tN/A" );
								}
								else {
									fprintf( fpt_o , "\t%.6lf" ,  Dat_Max_Step[k][ j ] );
								}
								j++;
							}
							fprintf( fpt_o , "\n" );
						}
						//============================================================================
						fclose( fpt_o );
						//============================================================================
					}
				}
				if ( ( WriteMode == 2 ) || ( WriteMode == 3 ) ) {
					//
					ok = FALSE;
					//
					if ( ( fpt_o = fopen( TempFileName3 , "a" ) ) != NULL ) {
						ok = TRUE;
					}
					//
					if ( ok ) {
						//
						j = 0;
						fprintf( fpt_o , "%s(%s)" , S_Chamber , S_Index );
						while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12
							if ( Dat_Count_Sum_StepAll[j] <= 0 ) {
								fprintf( fpt_o , "\tN/A" ); // 2008.02.28
							}
							else {
								fprintf( fpt_o , "\t%.6lf" , Dat_Sum_StepAll[ j ] / Dat_Count_Sum_StepAll[j] ); // 2008.02.28
							}
							j++;
						}
						fprintf( fpt_o , "\n" );
						//============================================================================
						// 2011.12.06
						for ( k = 0 ; k < MAX_STEP_COUNT ; k++ ) {
							if ( !Dat_Set_StepUse[k] ) continue;
							j = 0;
							fprintf( fpt_o , "$STEP%d" , k + 1 );
							while( ( Dat_Set_Step[k][j] ) && ( j < MAX_X_TABLE ) ) {
								if ( Dat_Count_Sum_Step[k][j] <= 0 ) {
									fprintf( fpt_o , "\tN/A" );
								}
								else {
									fprintf( fpt_o , "\t%.6lf" ,  Dat_Sum_Step[k][ j ] / Dat_Count_Sum_Step[k][j] );
								}
								j++;
							}
							fprintf( fpt_o , "\n" );
						}
						//============================================================================
						fclose( fpt_o );
						//============================================================================
					}
				}
			}
		}
	}
	while ( bres );
	//
	if ( ReadMode != 0 ) {
		//============================================================================
		fclose( fpt );
		//============================================================================
	}
	//
	if ( WriteMode == 3 ) {
		//
		if ( ( fpt_o = fopen( TempFileName4 , "a" ) ) == NULL ) return FALSE;
		//
		if ( UserMode ) { // 2014.02.27
	//		SendBuffer  = calloc( 5000 + 1 , sizeof( char ) ); // 2014.02.27
			SendBuffer  = calloc( 20000 + 1 , sizeof( char ) ); // 2014.02.27
			if ( SendBuffer != NULL ) strcpy( SendBuffer , "" );
		}
		else { // 2014.02.27
			SendBuffer  = NULL; // 2014.02.27
		}
		//
		j = 0;
		sprintf( Buffer , "MINIMUM" );
		fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
		while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12
			if ( Dat_Count_MinMax_StepAll[ j ] <= 0 ) {
				sprintf( Buffer , "\tN/A" ); // 2008.02.28
			}
			else {
				sprintf( Buffer , "\t%.6lf" ,  Dat_Min_StepAll[ j ] );
			}
			fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
			j++;
		}
		sprintf( Buffer , "\n" );
		fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
		//
		j = 0;
		sprintf( Buffer , "MAXIMUM" );
		fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
		while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12
			if ( Dat_Count_MinMax_StepAll[ j ] <= 0 ) {
				sprintf( Buffer , "\tN/A" ); // 2008.02.28
			}
			else {
				sprintf( Buffer , "\t%.6lf" ,  Dat_Max_StepAll[ j ] );
			}
			fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
			j++;
		}
		sprintf( Buffer , "\n" );
		fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
		//
		j = 0;
		sprintf( Buffer , "AVERAGE" );
		fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
		while( ( Dat_Set_StepAll[ j ] ) && ( j < MAX_X_TABLE ) ) { // 2004.10.12
			if ( Dat_Count_Sum_StepAll[j] <= 0 ) {
				sprintf( Buffer , "\tN/A" ); // 2008.02.28
			}
			else {
				sprintf( Buffer , "\t%.6lf" , ( Dat_Sum_StepAll[ j ] ) / Dat_Count_Sum_StepAll[j] ); // 2008.02.28
			}
			fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
			j++;
		}
		sprintf( Buffer , "\n" );
		fprintf( fpt_o , "%s" , Buffer ); if ( SendBuffer != NULL ) strcat( SendBuffer , Buffer );
		//
		fclose( fpt_o );
		//
		if ( SendBuffer != NULL ) {
			WRITE_UPLOAD_MESSAGE( SendBuffer );
			free( SendBuffer );
		}
		//
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL LogView_Open_Lot_Log_Init( HWND hwndListView ) {
	int         i;
	LV_COLUMN   lvColumn;
	TCHAR       *szString[] = { "No" , "Time" , "Module" , "Context" };
	int         szSize[4]   = {  55  ,     210 ,   120 ,  500 };
	int         szSize_M[4] = {  45  ,     140 ,   100 ,  500 };
	//
	ListView_DeleteAllItems( hwndListView );
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	if ( Get_Prm_MINISCREEN_MODE() ) {
		for ( i = 0 ; i < 4 ; i++ ) {
			lvColumn.cx = szSize_M[i];
			lvColumn.pszText = szString[i];
			//
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}
	}
	else {
		for ( i = 0 ; i < 4 ; i++ ) {
			lvColumn.cx = szSize[i];
			lvColumn.pszText = szString[i];
			//
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
			//
			ListView_InsertColumn( hwndListView , i , &lvColumn );
		}
	}
	return TRUE;
}
//


BOOL LogView_Open_LotModule_Log_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName0 , int mode , int index , int ME_ReadMode , int ME_Module , int ME_Slot , int ME_extview ) {
	int i , j , k , m , z1 , z2 , stm;
	char FileName[5][256];
	char Buffer[4096];
	char Buffer2[256];
	char folderstr[256];
	double Dat;
	BOOL bres;
	FILE *fpt;
	LV_COLUMN   lvColumn;
	LV_ITEM lvi;
//	int    Dat_Avr_Count = 0 , Dat_MinMax_Count = 0 , chkmode; // 2016.08.18
	int    chkmode; // 2016.08.18

	//----------------------------------------------------------------------------------------------
	if ( JOB_Get_Current_Count() <= index ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
		return FALSE;
	}
	if ( JOB_Get_Directory_Type( index ) != 1 ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Item Count Changed & Data not exist" , "Try Again" , MB_ICONHAND );
		return FALSE;
	}
	//----------------------------------------------------------------------------------------------
	//
	if ( !xListView_Enter() ) return FALSE;
	xListView_Clear( hlistview );
	if ( !xListView_Leave() ) return FALSE;
	//
	PLOTTING_DATA_CLEAR();
	//
	if      ( mode == 0 )
		PLOTTING_TITLE_SET( "Minimum Value" );
	else if ( mode == 1 )
		PLOTTING_TITLE_SET( "Maximum Value" );
	else if ( mode == 2 )
		PLOTTING_TITLE_SET( "Average Value" );
	else
		PLOTTING_TITLE_SET( "Estimate Value" );
	//
	//----------------------------------------------------------------------------------------------
	if ( sepload != 0 ) {
		//
		if ( sepload == 1 ) {
			//
			if      ( mode == 0 )
				LOG_OPEN_MESSAGE( "MAKING Minimum Value Data" );
			else if ( mode == 1 )
				LOG_OPEN_MESSAGE( "MAKING Maximum Value Data" );
			else if ( mode == 2 )
				LOG_OPEN_MESSAGE( "MAKING Average Value Data" );
			else
				LOG_OPEN_MESSAGE( "MAKING Estimate Value Data" );
			//
		}
		Make_Estimate_LogData_for_Min_Max_Avr( FALSE , ME_ReadMode , ME_Module , ME_Slot , index , "" , "" , mode , FALSE , ME_extview , ( sepload == 1 ) );
	}
	//----------------------------------------------------------------------------------------------
	//
	JOB_Get_Directory( index , folderstr );
	//
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		sprintf( FileName[0] , "%s/LotLog/%s/Temp1.txt" , Get_Prm_LOG_DIRECTORY() , folderstr );
		sprintf( FileName[1] , "%s/LotLog/%s/Temp2.txt" , Get_Prm_LOG_DIRECTORY() , folderstr );
		sprintf( FileName[2] , "%s/LotLog/%s/Temp3.txt" , Get_Prm_LOG_DIRECTORY() , folderstr );
		//
		sprintf( FileName[3] , "%s/LotLog/%s/Templ.txt" , Get_Prm_LOG_DIRECTORY() , folderstr );
		//
		sprintf( FileName[4] , "%s/LotLog/%s/TempT.txt" , Get_Prm_LOG_DIRECTORY() , folderstr );
	}
	else { // 2003.12.18
		sprintf( FileName[0] , "%s/%s/LotLog/%s/Temp1.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , folderstr );
		sprintf( FileName[1] , "%s/%s/LotLog/%s/Temp2.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , folderstr );
		sprintf( FileName[2] , "%s/%s/LotLog/%s/Temp3.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , folderstr );
		//
		sprintf( FileName[3] , "%s/%s/LotLog/%s/Templ.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , folderstr );
		//
		sprintf( FileName[4] , "%s/%s/LotLog/%s/TempT.txt" , Get_Prm_NETMONITOR_FOLDER() , Get_Prm_LOG_DIRECTORY() , folderstr );
	}
	//-----------------------------------------------------------------
	if ( ( fpt = fopen( FileName[3], "r" ) ) != NULL ) {
		//
		do {
			bres = Read_Line_From_File4( fpt , Buffer , 4095 ); // 2008.05.13
			//
			if ( *Buffer != 0 ) {
				//
				if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
				//
				ListView_DeleteAllItems( hlistview );
				lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
				lvColumn.fmt = LVCFMT_LEFT;
				//
				if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx =  45;
				else								lvColumn.cx =  55;
				//
				lvColumn.pszText = "No";
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
				//
				ListView_InsertColumn( hlistview , 0 , &lvColumn );
				//
				z1 = z2 = 0;
				//
				if ( *Buffer != 0 ) {
					//
					if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx =  140;
					else								lvColumn.cx =  200;
					//
					lvColumn.pszText = "Mode";
					z1 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
					//
					if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
					//
					ListView_InsertColumn( hlistview , 1 , &lvColumn );
					//
					j = 2;
				}
				while ( TRUE ) {
					//
					if ( Get_Prm_MINISCREEN_MODE() )	lvColumn.cx =  100;
					else								lvColumn.cx =  120;
					//
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
					if ( z1 != z2 ) {
						z1 = z2;
						lvColumn.pszText = Buffer2;
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hlistview , j , &lvColumn );
						//
						PLOTTING_ADD_ITEM( j - 2 , Buffer2 );
						//
						j++;
					}
					else {
						break;
					}
				}
				//
				if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
				//
				PLOTTING_MAX_ITEM( j - 2 );
				break;
			}
		}
		while ( bres );
		//
		fclose( fpt );
		//
		remove( FileName[3] );
	}

	for ( m = 0 , i = 0 ; m < 3 ; m++ ) {
		if ( ( mode != m ) && ( mode != 3 ) ) continue;
		//-----------------------------------------------------------------
		if ( ( fpt = fopen( FileName[m], "r" ) ) == NULL ) {
			if      ( mode == 0 )
				MODAL_WINDOW_MESSAGE_BOX( hWnd , "Minimum Value Data Error" , "Open Error [11]", MB_ICONHAND );
			else if ( mode == 1 )
				MODAL_WINDOW_MESSAGE_BOX( hWnd , "Maximum Value Data Error" , "Open Error [12]", MB_ICONHAND );
			else if ( mode == 2 )
				MODAL_WINDOW_MESSAGE_BOX( hWnd , "Average Value Data Error" , "Open Error [13]", MB_ICONHAND );
			else
				MODAL_WINDOW_MESSAGE_BOX( hWnd , "Estimate Value Data Error" , "Open Error [14]", MB_ICONHAND );
			return TRUE;
		}
		if ( mode == 3 ) {
			if      ( m == 0 ) sprintf( Buffer2 , "[MINIMUM Part]" );
			else if ( m == 1 ) sprintf( Buffer2 , "[MAXIMUM Part]" );
			else if ( m == 2 ) sprintf( Buffer2 , "[AVERAGE Part]" );
			//
			if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= i;
			lvi.iSubItem	= 0;
			lvi.pszText     = "";
			lvi.cchTextMax	= 100;
			lvi.iImage		= i;
			lvi.lParam		= ( i * 10000 ) + 9000 + 1;
			ListView_InsertItem( hlistview , &lvi );
			ListView_SetItemText( hlistview , i , 1 , Buffer2 );
			//
			if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
			//
			i++;
		}
		//
		k = 0;
		//
		DATA_BUFFER_RESET( -1 ); // 2016.08.19
		//
		do {
			bres = Read_Line_From_File4( fpt , Buffer , 4095 ); // 2008.05.13
			//
			if ( *Buffer != 0 ) {
				//
				if ( STRNCMP_L( Buffer , "$STEP" , 5 ) ) { // 2011.12.06
					strcpy( Buffer2 , "" );
					stm = 1;
				}
				else {
					sprintf( Buffer2 , "%d" , k + 1 );
					stm = 0; // 2011.12.06
				}
				//
				if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
				//
				lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
				lvi.state = lvi.stateMask = 0;
				lvi.iItem		= i;
				lvi.iSubItem	= 0;
				lvi.pszText     = Buffer2;
				lvi.cchTextMax	= 100;
				lvi.iImage		= i;
				lvi.lParam		= ( i * 10000 ) + 9000 + 10 + 1;
				//
				if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
					//
					if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
					//
					return FALSE;
				}
				j = 1;
				//
				z1 = z2 = 0;
				//
				chkmode = 0; // 2008.02.28
				//
				while( j <= ( MAX_X_TABLE + 1 ) ) { // 2004.10.12
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
					if ( z1 != z2 ) {
						z1 = z2;
						if ( j > 1 ) {
							Dat = atof( Buffer2 );
							Dat_Set_StepAll[ j - 2 ] = TRUE;
							if ( ( chkmode == 0 ) || ( chkmode == 2 ) ) {
								Dat_Sum_StepAll[ j - 2 ] = Dat_Sum_StepAll[ j - 2 ] + Dat;
								Dat_Count_Sum_StepAll[ j - 2 ]++;
							}
							if ( ( chkmode == 0 ) || ( chkmode == 1 ) ) {
								if ( Dat_Min_StepAll[ j - 2 ] > Dat ) Dat_Min_StepAll[ j - 2 ] = Dat;
								if ( Dat_Max_StepAll[ j - 2 ] < Dat ) Dat_Max_StepAll[ j - 2 ] = Dat;
								Dat_Count_MinMax_StepAll[ j - 2 ]++;
							}
							PLOTTING_ADD_DATA( j - 2 , Dat );
						}
						else {
							if ( stm == 0 ) { // 2011.12.06
								chkmode = Log_Get_String_ChkMode_from_Time( Buffer2 ); // 2008.02.28
								PLOTTING_ADD_TIME( Buffer2 );
							}
							else { // 2011.12.06
								Buffer2[0] = '[';
								strcat( Buffer2 , "]" );
							}
						}
						ListView_SetItemText( hlistview , i , j , Buffer2 );
						//
						j++;
					}
					else {
						break;
					}
				}
				//
				if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
				//
				//
				i++;
				//
				if ( stm == 0 ) { // 2011.12.06
					k++;
				}
				//
			}
		}
		while ( bres );
		//
		fclose( fpt );
		//
		//
		if ( !xListView_Enter() ) {	return FALSE;	}
		//
		//====
		//=== Blank Oper
		//====
		lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lvi.state = lvi.stateMask = 0;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.pszText     = "";
		lvi.cchTextMax	= 100;
		lvi.iImage		= i;
		lvi.lParam		= ( i * 10000 ) + 9000 + 0;
		ListView_InsertItem( hlistview , &lvi );
		i++;
		//====
		//=== Min Oper
		//====
		lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lvi.state = lvi.stateMask = 0;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.pszText     = "";
		lvi.cchTextMax	= 100;
		lvi.iImage		= i;
		lvi.lParam		= ( i * 10000 ) + 9000 + 7;
		ListView_InsertItem( hlistview , &lvi );
		ListView_SetItemText( hlistview , i , 1 , "MIN" );
		j = 2;
		while( ( Dat_Set_StepAll[ j - 2 ] ) && ( j <= ( MAX_X_TABLE + 1 ) ) ) { // 2004.10.12
			if ( Dat_Count_MinMax_StepAll[ j - 2 ] <= 0 ) {
				strcpy( Buffer2 , "N/A" );
			}
			else {
				sprintf( Buffer2 , "%.6lf" ,   Dat_Min_StepAll[ j - 2 ] );
			}
			ListView_SetItemText( hlistview , i , j , Buffer2 );
			//
			j++;
		}
		i++;
		//====
		//=== Max Oper
		//====
		lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lvi.state = lvi.stateMask = 0;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.pszText     = "";
		lvi.cchTextMax	= 100;
		lvi.iImage		= i;
		lvi.lParam		= ( i * 10000 ) + 9000 + 7;
		ListView_InsertItem( hlistview , &lvi );
		ListView_SetItemText( hlistview , i , 1 , "MAX" );
		j = 2;
		while( ( Dat_Set_StepAll[ j - 2 ] ) && ( j <= ( MAX_X_TABLE + 1 ) ) ) { // 2004.10.12
			if ( Dat_Count_MinMax_StepAll[ j - 2 ] <= 0 ) {
				strcpy( Buffer2 , "N/A" );
			}
			else {
				sprintf( Buffer2 , "%.6lf" ,   Dat_Max_StepAll[ j - 2 ] );
			}
			ListView_SetItemText( hlistview , i , j , Buffer2 );
			//
			j++;
		}
		i++;
		//====
		//=== Avr Oper
		//====
		lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lvi.state = lvi.stateMask = 0;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.pszText     = "";
		lvi.cchTextMax	= 100;
		lvi.iImage		= i;
		lvi.lParam		= ( i * 10000 ) + 9000 + 7;
		ListView_InsertItem( hlistview , &lvi );
		ListView_SetItemText( hlistview , i , 1 , "AVR" );
		j = 2;
		while( ( Dat_Set_StepAll[ j - 2 ] ) && ( j <= ( MAX_X_TABLE + 1 ) ) ) { // 2004.10.12
			if ( Dat_Count_Sum_StepAll[j-2] <= 0 ) {
				strcpy( Buffer2 , "N/A" );
			}
			else {
				sprintf( Buffer2 , "%.6lf" , ( Dat_Sum_StepAll[ j - 2 ] ) / Dat_Count_Sum_StepAll[j-2] );
			}
			ListView_SetItemText( hlistview , i , j , Buffer2 );
			//
			j++;
		}
		i++;
		//====
		//====
		if ( mode == 3 ) {
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= i;
			lvi.iSubItem	= 0;
			lvi.pszText     = "";
			lvi.cchTextMax	= 100;
			lvi.iImage		= i;
			lvi.lParam		= ( i * 10000 ) + 9000 + 0;
			ListView_InsertItem( hlistview , &lvi );
			i++;
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= i;
			lvi.iSubItem	= 0;
			lvi.pszText     = "";
			lvi.cchTextMax	= 100;
			lvi.iImage		= i;
			lvi.lParam		= ( i * 10000 ) + 9000 + 0;
			ListView_InsertItem( hlistview , &lvi );
			i++;
		}
		//
		if ( !xListView_Leave() ) {	return FALSE;	}
		//
	}
	if ( mode == 3 ) {
		k = 0;
		//
		if ( ( fpt = fopen( FileName[4], "r" ) ) != NULL ) {
			do {
				bres = Read_Line_From_File4( fpt , Buffer , 4095 ); // 2008.05.13
				if ( *Buffer != 0 ) {
					//
					sprintf( Buffer2 , "TOTAL" );
					//
					if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
					//
					lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
					lvi.state = lvi.stateMask = 0;
					lvi.iItem		= i;
					lvi.iSubItem	= 0;
					lvi.pszText     = Buffer2;
					lvi.cchTextMax	= 100;
					lvi.iImage		= i;
					lvi.lParam		= ( i * 10000 ) + 9000 + 8;
					if ( ListView_InsertItem( hlistview , &lvi ) == -1 ) {
						//
						if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
						//
						return FALSE;
					}
					j = 1;
					//
					z1 = z2 = 0;
					while( j <= ( MAX_X_TABLE + 1 ) ) { // 2004.10.12
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer2 , z1 , 255 );
						if ( z1 != z2 ) {
							z1 = z2;
							if ( j > 1 ) {
								Dat = atof( Buffer2 );
							}
							ListView_SetItemText( hlistview , i , j , Buffer2 );
							//
							j++;
						}
						else {
							break;
						}
					}
					//
					if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
					//
					i++;
					k++;
				}
			}
			while ( bres );
			//
			fclose( fpt );
			//
		}
	}
	remove( FileName[0] );
	remove( FileName[1] );
	remove( FileName[2] );
	remove( FileName[4] );
	//
	return TRUE;
}

BOOL LogView_Open_LotModule_Log_Data_Set( HWND hdlg , int winpos , int mode , int index , BOOL sepload , int ME_ReadMode , int ME_Module , int ME_Slot , int ME_extview ) {
	BOOL Res;
	if ( !sepload ) Data_Read_Start( hdlg );
	//-----------------------------------------------------------------
	Res = LOG_OPEN_CONTROL( hdlg , GetDlgItem( hdlg , winpos ) , sepload , NULL , mode , index , ME_ReadMode , ME_Module , ME_Slot , ME_extview , LogView_Open_LotModule_Log_Data_Set_Callback );
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_End( hdlg );
	return Res;
}
//
BOOL APIENTRY LogView_Open_LotModule_Log_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	HDC			hDC;
	PAINTSTRUCT	ps;
	int i , j , k;
	char Buffer[4096]; // 2008.05.13
	FILE *fpt;
	ViewStepTemplate *readdata;

	RECT rect;		// crkim

	LPDRAWITEMSTRUCT dis; // 2015.07.29
	LPMEASUREITEMSTRUCT dim; // 2015.07.29

	switch ( msg ) {
		case WM_MEASUREITEM: // 2015.07.29
			//
			//
			if ( ( GLISTVIEW_HEIGHT > 0 ) && ( ( GLISTVIEW_HEIGHT % 1000 ) >= 10 ) ) {
				//
				dim = (LPMEASUREITEMSTRUCT) lParam;
				//
				if ( dim->CtlType == ODT_LISTVIEW ) {
					if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
						 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
					}
				}
				//
			}
			else {
				if ( LISTVIEW_HEIGHT_UPDATE ) {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_LOGVIEW ) {
							 dim->itemHeight = LISTVIEW_HEIGHT;
						}
					}
					//
				}
			}
			//
			return TRUE;

		case WM_DRAWITEM: // 2015.07.29
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
			ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , -1 , 0 , 1 , "" , Buffer , 4095 , 0 , 0 , -1 , -1 , -1 , -1 );
			//
			break;

		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			readdata = (ViewStepTemplate *) (LPARAM) lParam;
			KWIN_Item_Show( hdlg , IDC_GRAPH_BUTTON );
			if ( !Get_Prm_SAVEFILE_MODE() ) KWIN_Item_Hide( hdlg , IDRETRY );
			// crkim // 2002.11.11
			if ( !Get_Prm_SCROLLBUTTON_MODE() ) {
				KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
			}
			// crkim // 2002.11.11
			//-------------------------------------------------------------------
			MoveCenterWindow( hdlg );
			//>>-----------------------------------------------------------
			if ( !LogView_Open_LotModule_Log_Data_Set( hdlg , IDC_LIST_LOGVIEW , readdata->mode , readdata->index , FALSE , 0 , 0 , 0 , 0 ) ) {
				EndDialog( hdlg , FALSE );
				return FALSE;
			}
			//
			JOB_Get_Display_String( readdata->index , Buffer );
			SetWindowText( hdlg , Buffer );
			//
			return TRUE;
			
		case WM_PAINT:
			hDC = BeginPaint( hdlg, &ps );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
			case IDC_GRAPH_BUTTON :
				GoModalDialogBoxParam( GETHINST( hdlg ) , KWIN_MAKEINTRESOURCE( IDD_PLOTTING_PAD , IDD_E_PLOTTING_PAD ) , hdlg , LogView_Open_Plotting_Proc , 0 ); // 2004.01.19 // 2004.08.10
				return TRUE;

			case IDRETRY :
				if ( Log_Get_Save_File_Name( hdlg , Buffer ) ) {
					fpt = fopen( Buffer , "w" );
					if ( fpt != NULL ) {
						k = 0;
						for ( i = 0 ; i < MAX_X_TABLE ; i++ ) {
							if ( Dat_Set_StepAll[i] ) {
								k = i + 1;
								fprintf( fpt , "\t%s" , PLOTTING_GET_ITEM( i ) );
							}
							else {
								break;
							}
						}
						fprintf( fpt , "\n" );
						//
						j = ListView_GetItemCount( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
						for ( i = 0 ; i < j ; i++ ) {
							Log_Get_String_Form( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , i , 1 , k + 1 , fpt );
						}
						fclose( fpt );
					}
					else {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] File Save Error!" , "Buffer" , MB_ICONHAND );
					}
				}
				return TRUE;
/// crkim
			case IDC_CTRL_SDOWN :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * i );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , j );
				return TRUE;

			case IDC_CTRL_SUP :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = ( ( rect.bottom - rect.top ) * i );
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , -j );
				return TRUE;

			case IDC_CTRL_RIGHT :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.right - rect.left;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;

			case IDC_CTRL_LEFT :
				i = ListView_GetCountPerPage( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
				ListView_GetItemRect( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , &rect , LVIR_LABEL );
				j = rect.left - rect.right;
				ListView_Scroll( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 8*j , 0 );
				return TRUE;

			case IDCANCEL :
				EndDialog( hdlg , TRUE );
				return TRUE;

			}
			return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int GET_CHAMBER_INFO_BOX( int id , HWND hWnd , int slot , int pmch , char *Message , char *Message2 , int *Chamber ) {
	FILE *fpt;
	BOOL bres;
	int i , j , k , z;
	BOOL ch[MAX_CHAMBER];
	char Buffer[256] , Buffer1[256];
	char FileName[256] , FileName_org[256] , SBuffer[1025];
	int L_Count;
	char L_Str[256];
	int encflag;

	//----------------------------------------------------------------------------------------------
	// 2003.09.16
	//----------------------------------------------------------------------------------------------
	if ( JOB_Get_Current_Count() <= id ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
		return 0;
	}
	//----------------------------------------------------------------------------------------------
	JOB_Get_Directory( id , Buffer );
	//
	sprintf( FileName_org , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , Buffer , METHOD_LOG_FILE );
	//----------------------------------------------------------------------------------------------
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else { // 2003.12.18
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
	}
	//-----------------------------------------------------------------
	// 2007.07.31
	//-----------------------------------------------------------------
//	if ( !Check_File_is_Open_Possible_Check( FileName ) ) {
//		MODAL_WINDOW_MESSAGE_BOX( hWnd , "File is not properly to Open(Reject)" , FileName , MB_ICONHAND );
//		return 0;
//	}
	//-----------------------------------------------------------------
	//-----------------------------------------------------------------
//	if ( ( fpt = fopen( FileName , "r+t" ) ) == NULL ) { // 2005.03.22
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) { // 2005.03.22
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Chamber Information is not exist" , "ERROR" , MB_ICONHAND ); // 2003.09.16
		return 0;
	}
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i ++ ) ch[i] = FALSE;
	j = 0;
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
			z = 0;
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , z , 255 );
			if ( strlen( Buffer1 ) <= 0 ) {
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , z , 255 );
				//
				if ( ( slot > 0 ) && ( slot != atoi( Buffer1 ) ) ) continue;
				//
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , z , 255 );
				//
				if ( ( tolower( Buffer1[0] ) == 'p' ) && ( tolower( Buffer1[1] ) == 'm' ) ) {
					//
					i = atoi( Buffer1 + 2 );
					//
					if ( ( i > 0 ) && ( i < MAX_CHAMBER ) ) {
						//
						if ( ( pmch > 0 ) && ( pmch != i ) ) continue;
						//
						j = 1;
						//
						ch[i] = TRUE;
					}
				}
			}
		}//slot
	}
	while ( bres );
	fclose( fpt );
	if ( j == 0 ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Chamber Information is invalid" , "ERROR" , MB_ICONHAND ); // 2003.09.16
		return 0;
	}
	//
	L_Count = 0;
	//
	strcpy( SBuffer , "" );
	for ( i = 1 , j = 0 ; i < MAX_CHAMBER ; i ++ ) {
		if ( ch[i] ) {
			if ( j != 0 ) strcat( SBuffer , "|" );
			if ( _GET_CHAMBER_STRING_POSSIBLE( i - 1 + PM1 ) ) {
				sprintf( Buffer , "%s" , _GET_CHAMBER_STRING( i - 1 + PM1 ) );
			}
			else {
				sprintf( Buffer , "PM%d" , i );
			}
			strcat( SBuffer , Buffer );
			j++;
			//
			L_Count++;
			strcat( L_Str , Buffer );
			//
		}
	}
	//
	if ( ( Get_Prm_DISPLAY_MODE() != 0 ) && ( L_Count == 1 ) ) { // 2011.07.12
		j = 0;
		bres = TRUE;
	}
	else {
		j = -1;
		bres = MODAL_DIGITAL_BOX1( hWnd , Message2 , Message , SBuffer , &j );
	}
	//
	if ( bres ) {
		for ( i = 1 , k = 0 ; i < MAX_CHAMBER ; i ++ ) {
			if ( ch[i] ) {
				if ( k == j ) {
					*Chamber = i;
					return 1;
				}
				k++;
			}
		}
	}
	return 2;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL GET_METHOD_INFO_SEARCH_FILE( int id , char *SlotS , char *ChamberS , BOOL prcmode , int extview , char *mode ) { // 2006.03.09
	int i , z , z2;
	char filename[256];
	char slotstr[256];
	char Buffer[256];
	char chstr[256];
	char folderstr[256];
	HANDLE hFind;
	WIN32_FIND_DATA	fd;

	//===========================================================================================
	*mode = 0;
	//===========================================================================================
	strcpy( slotstr , "" );
	if ( extview < 4 ) { // 2007.06.25
		z = 0;
		z2 = 0;
		while ( TRUE ) {
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( SlotS , '_' , Buffer , z , 255 );
			if ( z == z2 ) break;
			if ( z == 0 ) {
				if ( Buffer[0] == 'A' ) { // 2007.11.07
					i = atoi( Buffer + 1 );
					sprintf( Buffer , "A%02d" , i );
				}
				else if ( Buffer[0] == 'D' ) { // 2009.03.02
					i = atoi( Buffer + 1 );
					sprintf( Buffer , "D%02d" , i );
				}
				else {
					i = atoi( Buffer );
					sprintf( Buffer , "%02d" , i );
				}
			}
			else {
				if ( Buffer[0] == 'D' ) { // 2007.11.07
					i = atoi( Buffer + 1 );
					sprintf( Buffer , "_D%02d" , i );
				}
				else {
					i = atoi( Buffer );
					sprintf( Buffer , "_%02d" , i );
				}
			}
			strcat( slotstr , Buffer );
			z = z2;
		}
	}
	//===========================================================================================
	i = CHAMBER_STRING_FIND( ChamberS );
	if ( i <= 0 ) {
		if      ( ( ChamberS[0] == 'P' ) && ( ChamberS[1] == 'M' ) ) {
			sprintf( chstr , "PM%d" , atoi( ChamberS + 2 ) );
		}
		else if ( ( ChamberS[0] == 'B' ) && ( ChamberS[1] == 'M' ) ) {
			sprintf( chstr , "BM%d" , atoi( ChamberS + 2 ) );
		}
		else if ( ChamberS[0] == 'P' ) {
			sprintf( chstr , "PM%d" , atoi( ChamberS + 1 ) );
		}
		else if ( ChamberS[0] == 'B' ) {
			sprintf( chstr , "BM%d" , atoi( ChamberS + 1 ) );
		}
		else if ( ChamberS[0] == 'M' ) {
			sprintf( chstr , "PM%d" , atoi( ChamberS + 1 ) );
		}
		else {
			sprintf( chstr , "PM%d" , atoi( ChamberS ) );
		}
	}
	else {
		if ( i < BM1 ) {
			sprintf( chstr , "PM%d" , i - PM1 + 1 );
		}
		else {
			sprintf( chstr , "BM%d" , i - BM1 + 1 );
		}
	}
	//===========================================================================================
	JOB_Get_Directory(id, folderstr);
	//
	if ( prcmode ) {
		Get_Log_File_Name( filename , "PROCESS" , extview , folderstr , FALSE , 0 , 0 , slotstr , 0 , chstr , "" );
	}
	else {
		Get_Log_File_Name( filename , "RECIPE" , extview , folderstr , FALSE , 0 , 0 , slotstr , 0 , chstr , "" );
		//---------------------------------------------------------------------------
		// 2008.03.14
		//---------------------------------------------------------------------------
		hFind = FindFirstFile( filename , &fd );
		if ( hFind != INVALID_HANDLE_VALUE ) {
			FindClose( hFind );
			return TRUE;
		}
		//---------------------------------------------------------------------------
		strcat( filename , "_FILE" );
		//---------------------------------------------------------------------------
		*mode = 1;
		//---------------------------------------------------------------------------
	}
	//
	hFind = FindFirstFile( filename , &fd );
	if ( hFind == INVALID_HANDLE_VALUE ) return FALSE;
	FindClose( hFind );
	return TRUE;
}
//---------------------------------------------------------------------------------------
int GET_METHOD_INFO_BOX( int id , HWND hWnd , int selslot , char *Message , char *Message2 , char *Slotstr , char *ChamberString , char *methodstring , BOOL prcmode , int extview , int *mode ) {
	FILE *fpt;
	BOOL bres;
	int i , j , z , z2 , findf;
	char m;
	char Buffer[256] , Buffer1[256] , Buffer2[256] , IndexB[256] , ChamberS[256] , ChamberS2[256];
	char FileName[256] , FileName_org[256];
//	char SBuffer[2048]; // 2008.04.16
	char SBuffer[4096]; // 2008.04.16
	//
	char folderstr[256];
	//
	char MethidMd[256]; // 2008.03.14
	char MethidStr[256][32];
	char MethidBuff[32];
	int  Methidcnt = 0;
	//
	char MethitStr[256][32];
	//
	int  L_Count;
	char L_Str[256];
	int encflag;

	//----------------------------------------------------------------------------------------------
	*mode = 0; // 2008.03.14
	//----------------------------------------------------------------------------------------------
	// 2003.09.16
	//----------------------------------------------------------------------------------------------
	if ( JOB_Get_Current_Count() <= id ) {
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
		return 0;
	}
	//----------------------------------------------------------------------------------------------
	JOB_Get_Directory( id , folderstr );

	sprintf( FileName_org , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , folderstr , METHOD_LOG_FILE );
	//----------------------------------------------------------------------------------------------
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		strcpy( FileName , FileName_org );
	}
	else { // 2003.12.18
		sprintf( FileName , "%s/%s" , Get_Prm_NETMONITOR_FOLDER() , FileName_org );
		_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName );
		_nINF_KNFS_GetFile( FileName_org , FileName , FALSE );
	}
	//-----------------------------------------------------------------
	// 2007.07.31
	//-----------------------------------------------------------------
//	if ( !Check_File_is_Open_Possible_Check( FileName ) ) {
//		MODAL_WINDOW_MESSAGE_BOX( hWnd , "File is not properly to Open(Reject)" , FileName , MB_ICONHAND );
//		return 0;
//	}
	//-----------------------------------------------------------------
	//-----------------------------------------------------------------
//	if ( ( fpt = fopen( FileName , "r+t" ) ) == NULL ) { // 2005.03.22
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) { // 2005.03.22
		MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] Method Information is not exist" , "ERROR" , MB_ICONHAND ); // 2003.09.16
		return 0;
	}
	//
	strcpy( L_Str , "" );
	L_Count = 0;
	//
	strcpy( MethidBuff , "" );
	strcpy( SBuffer , "" );
	Methidcnt = 0;
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
			z = 0;
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Buffer1 , z , 255 );
			if ( strlen( Buffer1 ) <= 0 ) {
				if ( Methidcnt < 256 ) { // 2002.10.31
					strncpy( MethidStr[Methidcnt] , MethidBuff , 31 );
				}
				else {
					break; // 2004.02.27
				}
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , IndexB , z , 255 );
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , ChamberS , z , 255 );
				//
				if ( ChamberS[0] == '$' ) continue; // 2009.07.21
				//
				CHAMBER_STRING_CHANGE( ChamberS , ChamberS2 );
				//====================================================================================
				// 2006.03.09
				//====================================================================================
				if ( !Get_Prm_NETMONITOR_MODE() ) {
					sprintf( Buffer1 , "%-8s" , IndexB );
					findf = GET_METHOD_INFO_SEARCH_FILE( id , Buffer1 , ChamberS2 , prcmode , extview , &m );
				}
				else {
					m = 0; // 2008.03.14
					findf = TRUE;
				}
				//====================================================================================
				if ( findf ) { // 2006.03.09
					//=================================================
					// 2007.01.02
					//=================================================
/*
					if ( Methidcnt == 0 )
						sprintf( Buffer1 , "%-8s,%s" , IndexB , ChamberS2 );
					else
						sprintf( Buffer1 , "|%-8s,%s" , IndexB , ChamberS2 );
					//
					for ( i = 0 ; i < Methidcnt ; i++ ) {
						if ( STRCMP_L( MethitStr[i] , Buffer1 ) ) break;
					}
					if ( i == Methidcnt ) {
						strncpy( MethitStr[Methidcnt] , Buffer1 , 31 );
						Methidcnt++;
						strcat( SBuffer , Buffer1 );
					}
*/
					//=================================================
					if ( extview < 4 ) { // 2007.06.25
						//
						//if ( ( selslot > 0 ) && ( atoi(IndexB) != selslot ) ) continue; // 2011.09.29
						if ( selslot > 0 ) {
							if ( selslot >= 100 ) {
								if ( IndexB[0] != 'D' ) continue;
								if ( atoi(IndexB+1) != ( selslot % 100 ) ) continue;
							}
							else {
								if ( atoi(IndexB) != selslot ) continue;
							}
						}
						//
						sprintf( Buffer1 , "%-8s,%s" , IndexB , ChamberS2 );
					}
					else {
						sprintf( Buffer1 , "%s" , ChamberS2 );
					}
					//=================================================
					for ( i = 0 ; i < Methidcnt ; i++ ) {
						if ( STRCMP_L( MethitStr[i] , Buffer1 ) ) break;
					}
					if ( i == Methidcnt ) {
						strncpy( MethitStr[Methidcnt] , Buffer1 , 31 );
						MethidMd[Methidcnt] = m; // 2008.03.14
						if ( Methidcnt != 0 ) strcat( SBuffer , "|" );
						Methidcnt++;
						strcat( SBuffer , Buffer1 );
						//
						strcpy( L_Str , Buffer1 ); // 2011.07.12
						L_Count++;
						//
					}
					//=================================================
				}
			}
			else { // 2002.06.17
				if ( strncmp( Buffer1 , "{!RESTART(" , 10 ) == 0 ) {
					bres = FALSE;
				}
				strncpy( MethidBuff , Buffer1 , 31 );
			}
		}
	}
	while ( bres );
	fclose( fpt );
	//
	if ( L_Count <= 0 ) return 0; // 2012.12.05
	//
	if ( ( Get_Prm_DISPLAY_MODE() != 0 ) && ( L_Count == 1 ) ) { // 2011.07.12
		i = 0;
		strcpy( Buffer1 , L_Str ); // 2011.07.12
		bres = TRUE;
	}
	else {
		i = -1;
		bres = MODAL_DIGITAL_BOX3( hWnd , Message2 , Message , SBuffer , &i , Buffer1 );
	}
	//
	if ( bres ) {
		if ( extview < 4 ) { // 2007.06.25
			STR_SEPERATE_CHAR( Buffer1 , ',' , IndexB , ChamberS , 255 );
			//
			strcpy( Slotstr , "" ); // 2004.02.27
			//
			if ( extview < 4 ) { // 2007.06.25
				z = 0;	z2 = 0;
				while ( TRUE ) {
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( IndexB , '_' , Buffer2 , z , 255 );
					if ( z == z2 ) break;
					if ( z == 0 ) {
						if ( Buffer2[0] == 'A' ) { // 2007.11.07
							j = atoi( Buffer2 + 1 );
							sprintf( Buffer2 , "A%02d" , j );
						}
						else if ( Buffer2[0] == 'D' ) { // 2009.03.03
							j = atoi( Buffer2 + 1 );
							sprintf( Buffer2 , "D%02d" , j );
						}
						else {
							j = atoi( Buffer2 );
							sprintf( Buffer2 , "%02d" , j );
						}
					}
					else {
						if ( Buffer2[0] == 'D' ) { // 2007.11.07
							j = atoi( Buffer2 + 1 );
							sprintf( Buffer2 , "_D%02d" , j );
						}
						else {
							j = atoi( Buffer2 );
							sprintf( Buffer2 , "_%02d" , j );
						}
					}
					strcat( Slotstr , Buffer2 );
					z = z2;
				} // 2004.02.27
			}
		}
		else {
			strcpy( Slotstr , "" ); // 2007.06.25
			strcpy( ChamberS , Buffer1 ); // 2007.062.25
		}
		//
		if ( i < Methidcnt ) { // 2002.10.31
			strcpy( methodstring , MethidStr[i] );
			//
			*mode = MethidMd[i]; // 2008.03.14
			//
		}
		else {
			strcpy( methodstring , "" );
		}
		i = CHAMBER_STRING_FIND( ChamberS );
		if ( i <= 0 ) {
			if ( ( ChamberS[0] == 'P' ) && ( ChamberS[1] == 'M' ) )	{
				sprintf( ChamberString , "PM%d" , atoi( ChamberS + 2 ) );
			}
			else if ( ( ChamberS[0] == 'B' ) && ( ChamberS[1] == 'M' ) ) {
				sprintf( ChamberString , "BM%d" , atoi( ChamberS + 2 ) );
			}
			else if ( ChamberS[0] == 'P' ) {
				sprintf( ChamberString , "PM%d" , atoi( ChamberS + 1 ) );
			}
			else if ( ChamberS[0] == 'B' ) {
				sprintf( ChamberString , "BM%d" , atoi( ChamberS + 1 ) );
			}
			else if ( ChamberS[0] == 'M' ) {
				sprintf( ChamberString , "PM%d" , atoi( ChamberS + 1 ) );
			}
			else {
				sprintf( ChamberString , "PM%d" , atoi( ChamberS ) );
			}
		}
		else {
			if ( i < BM1 ) {
				sprintf( ChamberString , "PM%d" , i - PM1 + 1 );
			}
			else {
				sprintf( ChamberString , "BM%d" , i - BM1 + 1 );
			}
		}
		return 1;
	}
	return 2;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void LogView_Open_Full_Error_Message( HWND hWnd ) {
	if ( Get_Prm_MINISCREEN_MODE() ) {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
	else {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Error_Message_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
}
//
void LogView_Open_Full_Method( HWND hWnd ) {
	if ( Get_Prm_MINISCREEN_MODE() ) {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
	else {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2002.06.17
//		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Full_Method_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
}
//
BOOL LogView_Open_Lot_History( HWND hWnd , int index ) {
	BOOL Res;
	//
	if ( LISTVIEW_HEIGHT_UPDATE ) { // 2015.07.29
		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index );
	}
	else {
		//
		if ( Get_Prm_MINISCREEN_MODE() ) {
	//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2002.06.17
	//		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2004.01.19
	//		Res = GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10
			Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10
		}
		else {
	//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2002.06.17
	//		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2004.01.19
	//		Res = GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10
			Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Lot_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10
		}
	}
	if ( !Res ) return FALSE;
	if ( !Get_Prm_WINDOW_RETURN_MODE() ) return FALSE;
	return TRUE;
}
//
BOOL LogView_Open_Method_History( HWND hWnd , int index ) {
	BOOL Res;
	if ( Get_Prm_MINISCREEN_MODE() ) {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2002.06.17
//		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19
//		Res = GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10
//		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10 // 2007.08.10
//		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10 // 2007.08.10 // 2008.06.05
		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10 // 2007.08.10 // 2008.06.05
	}
	else {
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2002.06.17
//		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19
//		Res = GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10
//		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10 // 2015.06.04
		Res = GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_Method_History_Proc , (LPARAM) index ); // 2004.01.19 // 2004.08.10 // 2015.06.04
	}

	if ( !Res ) return FALSE;
	if ( !Get_Prm_WINDOW_RETURN_MODE() ) return FALSE;
	return TRUE;
}
//

BOOL modeset( int lastsel , char *mthname , char *pmlogfile , int color , int *nmode ) {
	//
	*nmode = 0;
	if ( color == 2 ) *nmode = 9;
	if ( pmlogfile[0] != 0 ) *nmode = 7;
	//
	if ( lastsel == 2 ) { // mtl + prcs
		//
		if ( mthname[0] != 0 ) *nmode = 2;
		else {
			if ( *nmode != 7 ) return FALSE;
		}
		//
	}
	else if ( lastsel == 3 ) { // mtl
		//
		if ( mthname[0] != 0 ) *nmode = 3;
		else {
			return FALSE;
		}
		//
	}
	else {
		if ( mthname[0] != 0 ) *nmode = 1;
	}
	return TRUE;
}
//


void CURRENT_WAFER_APPEND_ID2( char *id , char *name , char *nameorg );
void CURRENT_WAFER_APPEND_ID( int jobindex , int mindex , int id , char *name , int dummyin );
void CURRENT_WAFER_APPEND_MODULE( int mode , int tm , char *name , double st , double et );
//


void LogView_Open_Method_String_Change( int jobindex , int mindex , int mode , char *Index , char *MethodName , char *ModeStr , char *Chamber , char *Chamber_ORG , char *Recipe , char *TMmode , double st , double et , int *colorSet , int *findex , int infoinsert , int infoindex ) {
	int uf , tm;
	char TempStr[64];
	char Buffer1[64];
	char Buffer2[64];
	//
	char tslot[64];
	char tmstr[64];
	char tmarm[64];
	//
	strcpy( TMmode , "" );
	//
	if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) *colorSet = 4;
	//
	if ( mode == 7 ) {
		//
		*colorSet = 9;
		//
		strcpy( MethodName , "            -->>" );
		//
		if ( infoinsert == 2 ) {
			if ( infoindex == *findex ) {
				if ( Index[0] == 'D' ) {
					sprintf( TempStr , "D%02d" , atoi( Index + 1 ) );
					CURRENT_WAFER_APPEND_ID2( TempStr , Chamber , Chamber_ORG );
				}
				else {
					STR_SEPERATE_CHAR( Index , '_' , Buffer1 , Buffer2 , 63 );
					//
					if ( Buffer2[0] == 0 ) {
						sprintf( TempStr , "%02d" , atoi( Buffer1 ) );
					}
					else {
						if ( Buffer2[0] == 'D' ) { // 2014.10.16
							sprintf( TempStr , "%02d_D%02d" , atoi( Buffer1 ) , atoi( Buffer2 + 1 ) );
						}
						else {
							sprintf( TempStr , "%02d_%02d" , atoi( Buffer1 ) , atoi( Buffer2 ) );
						}
					}
					//
					CURRENT_WAFER_APPEND_ID2( TempStr , Chamber , Chamber_ORG );
				}
				*findex = -1;
				return;
			}
		}
		//
		if ( infoinsert == 1 ) CURRENT_WAFER_APPEND_MODULE( TCH_PROCESS , -1 , Chamber , st , et );
		//
		(*findex)++;
	}
	//
	if      ( mode == 9 )	strcpy( ModeStr , Recipe );
	else if ( mode == 7 )	strcpy( ModeStr , "PROCESS" );
	else					strcpy( ModeStr , "" );
	//
	uf = 0;
	//
	if      ( mode == 9 )	{
		//
		tm = 1;
		//
		if      ( STRNCMP_L( ModeStr , "PUT" , 3 ) ) uf = TCH_PM_PLACE;
		else if ( STRNCMP_L( ModeStr , "GET" , 3 ) ) uf = TCH_PM_PICK;
		//
		if ( ( ModeStr[3] >= '1' ) && ( ModeStr[3] <= '9' ) ) {
			//
			tm = ModeStr[3] - '0';
			//
			if ( ( ModeStr[4] >= '0' ) && ( ModeStr[4] <= '9' ) ) {
				//
				tm = (tm*10) + ( ModeStr[4] - '0' );
				//
			}
		}
		//
		if ( uf != 0 ) {
			STR_SEPERATE_CHAR( ModeStr + 3 , ':' , tmstr , Buffer1 , 63 );
			STR_SEPERATE_CHAR( Buffer1     , ':' , tmarm , Buffer2 , 63 );
			STR_SEPERATE_CHAR( Buffer2     , ':' , tslot , Buffer1 , 63 );
		}
		//
		if ( infoinsert == 1 ) {
			//
			if ( uf != 0 ) {
				if ( tslot[0] == 0 ) {
					if ( tolower( Chamber[0] ) == 'p' ) {
						CURRENT_WAFER_APPEND_MODULE( uf , tm , Chamber , st , et );
					}
					else {
						CURRENT_WAFER_APPEND_MODULE( uf + 2 , tm , Chamber , st , et );
					}
				}
				else {
					if ( tolower( Chamber[0] ) == 'p' ) {
						if ( atoi( tslot ) <= 1 ) {
							CURRENT_WAFER_APPEND_MODULE( uf , tm , Chamber , st , et );
						}
						else {
							strcat( Chamber , ":" );
							strcat( Chamber , tslot );
							CURRENT_WAFER_APPEND_MODULE( uf , tm , Chamber , st , et );
						}
					}
					else {
						strcat( Chamber , ":" );
						strcat( Chamber , tslot );
						CURRENT_WAFER_APPEND_MODULE( uf + 2 , tm , Chamber , st , et );
					}
				}
			}
			//
		}
		//
		if ( uf == 0 ) {
			strcpy( TempStr , Chamber );
			strcpy( Chamber , "      " );
			strcat( Chamber , TempStr );
		}
		else {
			//
			if ( tslot[0] == 0 ) {
				strcpy( TempStr , Chamber );
				strcpy( Chamber , "      " );
				strcat( Chamber , TempStr );
			}
			else {
				if ( tolower( Chamber[0] ) == 'p' ) {
					if ( atoi( tslot ) <= 1 ) {
						strcpy( TempStr , Chamber );
						strcpy( Chamber , "      " );
						strcat( Chamber , TempStr );
					}
					else {
						strcpy( TempStr , Chamber );
						strcpy( Chamber , "      " );
						strcat( Chamber , TempStr );
						strcat( Chamber , ":" );
						strcat( Chamber , tslot );
					}
				}
				else {
					strcpy( TempStr , Chamber );
					strcpy( Chamber , "      " );
					strcat( Chamber , TempStr );
					strcat( Chamber , ":" );
					strcat( Chamber , tslot );
				}
			}
			//
			ModeStr[3] = 0;
			//
			if ( tmarm[0] == 0 ) {
				sprintf( TMmode , "TM%s" , tmstr );
			}
			else {
				sprintf( TMmode , "TM%s(%s)" , tmstr , tmarm );
			}
			//
		}
	}
	//
	if ( ( mode != 7 ) && ( MethodName[0] != 0 ) ) {
		if ( Index[0] != 0 ) {
			//
			if ( infoinsert == 1 ) {
				if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
					//
					// 2013.12.05
					//
					STR_SEPERATE_CHAR( Chamber , '-' , Buffer1 , Buffer2 , 63 );
					//
					if ( Index[0] == 'D' ) {
						//
						if ( ( Buffer1[0] == 'B' ) && ( Buffer1[1] == 'M' ) ) {
							CURRENT_WAFER_APPEND_ID( jobindex , mindex , 100 + atoi( Index + 1 ) , MethodName , atoi( Buffer1 + 2 ) );
						}
						else {
							CURRENT_WAFER_APPEND_ID( jobindex , mindex , 100 + atoi( Index + 1 ) , MethodName , 0 );
						}
						//
					}
					else {
						CURRENT_WAFER_APPEND_ID( jobindex , mindex , atoi( Index ) , MethodName , 0 );
					}
					//
					CURRENT_WAFER_APPEND_MODULE( TCH_CM_IN , -1 , Buffer1 , st , et );
					//
					if ( ( Buffer2[0] == 0 ) || ( Buffer2[1] == 0 ) ) {
						CURRENT_WAFER_APPEND_MODULE( TCH_CM_OUT , -1 , Buffer1 , st , et );
					}
					else {
						CURRENT_WAFER_APPEND_MODULE( TCH_CM_OUT , -1 , Buffer2 + 1 , st , et );
					}
					//
					//
				}
			}
			//
			strcat( MethodName , ":(" );
			strcat( MethodName , Index );
			strcat( MethodName , ")" );
			//
			//
		}
	}
	//
}

BOOL Wafer_ID_Same( char *data , int index ) {
	if ( index >= 100 ) {
		if ( data[0] == 'D' ) {
			if ( atoi( data + 1 ) == ( index % 100 ) ) return TRUE;
		}
	}
	else {
		if ( atoi( data ) == index ) return TRUE;
	}
	return FALSE;
}

double CURRENT_WAFER_GET_TIME( int , char *data ); // 2012.11.26


/*
// 2014.02.13
BOOL LogView_Open_Method_History_Data_Set_Sub( HWND hdlg , int index , int infoinsert , int findwid , int fileorder ) {
	//
	int i , j , f , z , c , mode , mc;
	char FileName[256];
	char Buffer[4096]; // 2008.02.28
	char Buffer2[16];
	char MethodName[64];
	char Index[64];
	char Chamber[64];
	char Chamber2[64];
	char ModeStr[256];
	char Time1[64];
	char Time2[64];
	char Time3[64];
	char Recipe[2560]; // 2008.02.28
	BOOL bres;
	FILE *fpt;
	BOOL findmode;
	int lastsel;

	//===================================================
	BOOL Temp_Use = FALSE;
	int  Temp_mc;
	int  Temp_Color;
	char Temp_Buffer2[16];
	char Temp_MethodName[64];
	char Temp_Index[64];
	char Temp_Chamber[64];
	char Temp_Mode[256];
	char Temp_Time1[64];
	char Temp_Time2[64];
	char Temp_Time3[64];
	char Temp_TM[64];
	char Temp_Recipe[2560]; // 2008.02.28
	char Temp_Buffer_z[512];
	int encflag;

	//===================================================
	double Temp_st , Temp_et , rst , ret; // 2012.11.26

	//===================================================

	if ( index < 0 ) {
		return FALSE;
	}
	if ( JOB_Get_Current_Count() <= index ) {
		return FALSE;
	}
	if ( JOB_Get_Directory_Type( index ) != 1 ) {
		return FALSE;
	}
	//
	lastsel = -1;
	//----------------------------------------------------------------------------------------------
	sprintf( FileName , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , JOB_Get_Directory( index ) , METHOD_LOG_FILE );
	//----------------------------------------------------------------------------------------------
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) return TRUE;
	//
	Temp_Use = FALSE;
	i = j = 0;
	//
	f = 0;
	//
	mc = 0;
	//
	mode = 0; // Unknown:0 Method-All:1 Method-Prcs:2 Method-Only:3 Process:7 else:8 user:9
	//
	if ( findwid == -1 ) {
		findmode = 0;
	}
	else {
		findmode = 1;
	}
	//
	encflag = 0; // 2011.12.07
	//
	do {
		bres = Read_Line_From_File4( fpt , Buffer , 4095 ); // 2008.02.28
		if ( *Buffer != 0 ) {
			//
//			Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
			//
			z = 0;
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , MethodName , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Index , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Chamber , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time1 , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time2 , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time3 , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Recipe , z , 2559 ); // 2008.02.28
			//
			rst = CURRENT_WAFER_GET_TIME( Time1 ); // 2012.11.26
			ret = CURRENT_WAFER_GET_TIME( Time2 ); // 2012.11.26
			//
			if ( strlen( MethodName ) > 0 ) {
				if ( strncmp( MethodName , "{!RESTART(" , 10 ) == 0 ) { // 2002.06.17
					i = 0;
					strcpy( Buffer2 , "*" );
					//
					if ( Temp_Use ) {
						// error
						//
						if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
							//
							LogView_Open_Method_String_Change( mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
							//
							if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
							//
							if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
								if ( findmode == 1 ) {
									if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
										if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
											findmode = 2;
										}
									}
								}
								else if ( findmode == 2 ) {
									findmode = 3;
								}
							}
							//
							if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
								if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
									if ( hdlg != NULL ) {
										LogView_Open_Method_History_Insert( hdlg , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , 1 + ( Temp_Color * 10 ) + ( mode * 100 ) );
										j++;
									}
								}
							}
							//
						}
					}
					//
					Temp_Use = TRUE;
					//
					strcpy( Temp_Buffer2 , Buffer2 );
					strcpy( Temp_MethodName , MethodName );
					strcpy( Temp_Index , Index );
					//
					if ( Chamber[0] == '$' ) { // 2009.07.21
						Temp_Color = 2; // 2009.07.21
						strcpy( Temp_Chamber , Chamber + 1 );
					}
					else {
						Temp_Color = 0; // 2009.07.21
						CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 );
						strcpy( Temp_Chamber , Chamber2 );
					}
					//
					strcpy( Temp_Time1 , Time1 );
					strcpy( Temp_Time2 , Time2 );
					strcpy( Temp_Time3 , Time3 );
					strcpy( Temp_Recipe , Recipe );
					strcpy( Temp_Buffer_z , Buffer + z );
					//
					Temp_st = rst; // 2012.11.26
					Temp_et = ret; // 2012.11.26
					//
					Temp_mc = mc - 1; // 2012.11.29
					//======================================================================================================
				}
				else {
					//
					mc++;
					//
					i++;
					sprintf( Buffer2 , "%d" , i );
					//
					if ( Chamber[0] == '$' ) { // 2009.07.21
						c = 2; // 2009.07.21
						strcpy( Chamber2 , Chamber + 1 );
					}
					else {
						c = 0; // 2009.07.21
						CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 ); // 2006.10.26
					}
					//
					//======================================================================================================
					if ( Temp_Use ) {
						// error
						if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
							//
							LogView_Open_Method_String_Change( mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
							//
							if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
							//
							if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
								if ( findmode == 1 ) {
									if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
										if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
											findmode = 2;
										}
									}
								}
								else if ( findmode == 2 ) {
									findmode = 3;
								}
							}
							//
							if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
								if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
									if ( hdlg != NULL ) {
										LogView_Open_Method_History_Insert( hdlg , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , 1 + ( Temp_Color * 10 ) + ( mode * 100 ) );
										j++;
									}
								}
							}
							//
						}
					}
					//
					Temp_Use = TRUE;
					Temp_Color = c; // 2009.07.21
					strcpy( Temp_Buffer2 , Buffer2 );
					strcpy( Temp_MethodName , MethodName );
					strcpy( Temp_Index , Index );
					strcpy( Temp_Chamber , Chamber2 ); // 2008.02.27
					strcpy( Temp_Time1 , Time1 );
					strcpy( Temp_Time2 , Time2 );
					strcpy( Temp_Time3 , Time3 );
					strcpy( Temp_Recipe , Recipe );
					strcpy( Temp_Buffer_z , Buffer + z );
					//
					Temp_st = rst; // 2012.11.26
					Temp_et = ret; // 2012.11.26
					//
					Temp_mc = mc - 1; // 2012.11.29
					//======================================================================================================
				}
			}
			else {
				if ( Temp_Use ) {
					//
					if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
						//
						LogView_Open_Method_String_Change( mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
						//
						if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
						//
						if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
							if ( findmode == 1 ) {
								if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
									if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
										findmode = 2;
									}
								}
							}
							else if ( findmode == 2 ) {
								findmode = 3;
							}
						}
						//
						if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
							if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
								if ( hdlg != NULL ) {
									LogView_Open_Method_History_Insert( hdlg , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , 0 + ( Temp_Color * 10 ) + ( mode * 100 ) );
									j++;
								}
							}
						}
						//
					}
				}
				Temp_Use = FALSE;
				//===================================================================================================
				sprintf( Buffer2 , "" );
				//
				if ( Chamber[0] == '$' ) { // 2009.07.21
					c = 2; // 2009.07.21
					strcpy( Chamber2 , Chamber + 1 );
				}
				else {
					c = 0; // 2009.07.21
					CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 );
				}
				//
				if ( modeset( lastsel , MethodName , Buffer + z , c , &mode ) ) {
					//
					LogView_Open_Method_String_Change( mode , Index , MethodName , ModeStr , Chamber2 , Recipe , Temp_TM , rst , ret , &c , &f , infoinsert , findwid );
					//
					if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
					//
					if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
						if ( findmode == 1 ) {
							if ( ( fileorder < 0 ) || ( ( mc - 1 ) == fileorder ) ) { // 2012.11.29
								if ( Wafer_ID_Same( Index , findwid ) ) {
									findmode = 2;
								}
							}
						}
						else if ( findmode == 2 ) {
							findmode = 3;
						}
					}
					//
					if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
						if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
							if ( hdlg != NULL ) {
								LogView_Open_Method_History_Insert( hdlg , j , 1 , "" , MethodName , Index , Chamber2 , Time1 , Time2 , Time3 , Recipe , Buffer + z , ModeStr , Temp_TM , 0 + ( c * 10 ) + ( mode * 100 ) );
								j++; // 2007.06.20
							}
						}
					}
				}
			}
		}
	}
	while ( bres );
	//=========================================================================================================
	// 2007.06.20
	//=========================================================================================================
	if ( Temp_Use ) {
		// error
		if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
			//
			LogView_Open_Method_String_Change( mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
			//
			if ( ( infoinsert == 2 ) && ( f == -1 ) ) {
				//
			}
			else {
				//
				if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
					if ( findmode == 1 ) {
						if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
							if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
								findmode = 2;
							}
						}
					}
					else if ( findmode == 2 ) {
						findmode = 3;
					}
				}
				//
				if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
					if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
						if ( hdlg != NULL ) {
							LogView_Open_Method_History_Insert( hdlg , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , 1 + ( Temp_Color * 10 ) + ( mode * 100 ) );
							j++;
						}
					}
				}
				//
			}
		}
	}
	//=========================================================================================================
	fclose( fpt );
	return TRUE;
}
//
BOOL LogView_Open_Method_History_Data_Set( HWND hdlg , int index , int infoinsert , int findwid , int fileorder ) {
	BOOL Res;
	Data_Read_Start( hdlg );
	Res = LogView_Open_Method_History_Data_Set_Sub( hdlg , index , infoinsert , findwid , fileorder );
	Data_Read_End( hdlg );
	return Res;
}
//
*/

// 2014.02.13
BOOL LogView_Open_Method_History_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName0 , int jobindex , int infoinsert , int findwid , int fileorder , int mindex , int option6 ) {
	//
	int i , j , f , z , c , mode , mc;
	char FileName[256];
	char folderstr[256];
	char Buffer[4096]; // 2008.02.28
	char Buffer2[16];
	char MethodName[64];
	char Index[64];
	char Chamber[64];
	char Chamber1[64]; // 2017.08.18
	char Chamber2[64];
	char ModeStr[256];
	char Time1[64];
	char Time2[64];
	char Time3[64];
	char Recipe[2560]; // 2008.02.28
	BOOL bres;
	FILE *fpt;
	BOOL findmode;
	int lastsel;

	//===================================================
	BOOL Temp_Use = FALSE;
	int  Temp_mc;
	int  Temp_Color;
	char Temp_Buffer2[16];
	char Temp_MethodName[64];
	char Temp_Index[64];
	char Temp_Chamber[64];
	char Temp_Chamber_ORG[64]; // 2017.08.18
	char Temp_Mode[256];
	char Temp_Time1[64];
	char Temp_Time2[64];
	char Temp_Time3[64];
	char Temp_TM[64];
	char Temp_Recipe[2560]; // 2008.02.28
	char Temp_Buffer_z[512];
	int encflag;

	int errflag; // 2015.05.27
	BOOL lotrun; // 2015.05.27
	//===================================================
	double Temp_st , Temp_et , rst , ret; // 2012.11.26

	//===================================================

	if ( jobindex < 0 ) {
		return FALSE;
	}
	if ( JOB_Get_Current_Count() <= jobindex ) {
		return FALSE;
	}
	if ( JOB_Get_Directory_Type( jobindex ) != 1 ) {
		return FALSE;
	}
	//
	lastsel = -1;
	//----------------------------------------------------------------------------------------------
	JOB_Get_Directory( jobindex , folderstr );
	sprintf( FileName , "%s/LotLog/%s/%s" , Get_Prm_LOG_DIRECTORY() , folderstr , METHOD_LOG_FILE );
	//----------------------------------------------------------------------------------------------
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) return TRUE;
	//
	lotrun = ( JOB_Get_Run_Status( jobindex ) == JOB_RUNNING ); // 2015.05.27
	//
	Temp_Use = FALSE;
	i = j = 0;
	//
	f = 0;
	//
	mc = 0;
	//
	mode = 0; // Unknown:0 Method-All:1 Method-Prcs:2 Method-Only:3 Process:7 else:8 user:9
	//
	if ( findwid == -1 ) {
		findmode = 0;
	}
	else {
		findmode = 1;
	}
	//
	encflag = 0; // 2011.12.07
	//
	do {
		bres = Read_Line_From_File4( fpt , Buffer , 4095 ); // 2008.02.28
		if ( *Buffer != 0 ) {
			//
//			Make_Simple_Encryption( LOG_ENCRIPTION , Buffer ); // 2010.04.15
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer ); // 2011.12.07
			//
			z = 0;
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , MethodName , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Index , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Chamber , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time1 , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time2 , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Time3 , z , 63 );
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Recipe , z , 2559 ); // 2008.02.28
			//
			rst = CURRENT_WAFER_GET_TIME( mindex , Time1 ); // 2012.11.26
			ret = CURRENT_WAFER_GET_TIME( mindex , Time2 ); // 2012.11.26
			//
			if ( strlen( MethodName ) > 0 ) {
				if ( strncmp( MethodName , "{!RESTART(" , 10 ) == 0 ) { // 2002.06.17
					i = 0;
					strcpy( Buffer2 , "*" );
					//
					if ( Temp_Use ) {
						// error
						//
						if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
							//
							LogView_Open_Method_String_Change( jobindex , mindex , mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Chamber_ORG , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
							//
							if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
							//
							if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
								if ( findmode == 1 ) {
									if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
										if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
											findmode = 2;
										}
									}
								}
								else if ( findmode == 2 ) {
									findmode = 3;
								}
							}
							//
							if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
								if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
									if ( hlistview != NULL ) {
										//
										if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
										//
										LogView_Open_Method_History_Insert( hlistview , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , 1 + ( Temp_Color * 10 ) + ( mode * 100 ) );
										//
										if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
										//
										j++;
									}
								}
							}
							//
						}
					}
					//
					Temp_Use = TRUE;
					//
					strcpy( Temp_Buffer2 , Buffer2 );
					strcpy( Temp_MethodName , MethodName );
					strcpy( Temp_Index , Index );
					//
					if ( Chamber[0] == '$' ) { // 2009.07.21
						Temp_Color = 2; // 2009.07.21
//						strcpy( Temp_Chamber , Chamber + 1 ); // 2015.12.03
						CHAMBER_STRING_CHANGE( Chamber + 1 , Temp_Chamber ); // 2015.12.03
						//
						strcpy( Chamber1 , Chamber + 1 ); // 2017.08.18
						strcpy( Temp_Chamber_ORG , Chamber1 ); // 2017.08.18
					}
					else {
						Temp_Color = 0; // 2009.07.21
//						CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 ); // 2015.12.03
						CHAMBER_STRING_CHANGE( Chamber , Chamber2 ); // 2015.12.03
						strcpy( Temp_Chamber , Chamber2 );
						//
						strcpy( Chamber1 , Chamber ); // 2017.08.18
						strcpy( Temp_Chamber_ORG , Chamber1 ); // 2017.08.18
					}
					//
					strcpy( Temp_Time1 , Time1 );
					strcpy( Temp_Time2 , Time2 );
					strcpy( Temp_Time3 , Time3 );
					strcpy( Temp_Recipe , Recipe );
					strcpy( Temp_Buffer_z , Buffer + z );
					//
					Temp_st = rst; // 2012.11.26
					Temp_et = ret; // 2012.11.26
					//
					Temp_mc = mc - 1; // 2012.11.29
					//======================================================================================================
				}
				else {
					//
					mc++;
					//
					i++;
					sprintf( Buffer2 , "%d" , i );
					//
					if ( Chamber[0] == '$' ) { // 2009.07.21
						c = 2; // 2009.07.21
//						strcpy( Chamber2 , Chamber + 1 ); // 2015.12.03
						CHAMBER_STRING_CHANGE( Chamber + 1 , Chamber2 ); // 2015.12.03
						//
						strcpy( Chamber1 , Chamber + 1 ); // 2017.08.18
					}
					else {
						c = 0; // 2009.07.21
//						CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 ); // 2006.10.26 // 2015.12.03
						CHAMBER_STRING_CHANGE( Chamber , Chamber2 ); // 2015.12.03
						//
						strcpy( Chamber1 , Chamber ); // 2017.08.18
					}
					//
					//======================================================================================================
					if ( Temp_Use ) {
						// error
						if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
							//
							LogView_Open_Method_String_Change( jobindex , mindex , mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Chamber_ORG , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
							//
							if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
							//
							if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
								if ( findmode == 1 ) {
									if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
										if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
											findmode = 2;
										}
									}
								}
								else if ( findmode == 2 ) {
									findmode = 3;
								}
							}
							//
							if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
								if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
									if ( hlistview != NULL ) {
										//
										if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
										//
										LogView_Open_Method_History_Insert( hlistview , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , 1 + ( Temp_Color * 10 ) + ( mode * 100 ) );
										//
										if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
										//
										j++;
									}
								}
							}
							//
						}
					}
					//
					Temp_Use = TRUE;
					Temp_Color = c; // 2009.07.21
					strcpy( Temp_Buffer2 , Buffer2 );
					strcpy( Temp_MethodName , MethodName );
					strcpy( Temp_Index , Index );
					strcpy( Temp_Chamber , Chamber2 ); // 2008.02.27
					strcpy( Temp_Chamber_ORG , Chamber1 ); // 2017.08.18
					strcpy( Temp_Time1 , Time1 );
					strcpy( Temp_Time2 , Time2 );
					strcpy( Temp_Time3 , Time3 );
					strcpy( Temp_Recipe , Recipe );
					strcpy( Temp_Buffer_z , Buffer + z );
					//
					Temp_st = rst; // 2012.11.26
					Temp_et = ret; // 2012.11.26
					//
					Temp_mc = mc - 1; // 2012.11.29
					//======================================================================================================
				}
			}
			else {
				if ( Temp_Use ) {
					//
					if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
						//
						LogView_Open_Method_String_Change( jobindex , mindex , mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Chamber_ORG , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
						//
						if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
						//
						if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
							if ( findmode == 1 ) {
								if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
									if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
										findmode = 2;
									}
								}
							}
							else if ( findmode == 2 ) {
								findmode = 3;
							}
						}
						//
						if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
							if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
								if ( hlistview != NULL ) {
									//
									if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
									//
									errflag = Elpased_Error_String( lotrun , Temp_Time2 , Temp_Time3 ); // 2015.05.27
									//
									LogView_Open_Method_History_Insert( hlistview , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , errflag + ( Temp_Color * 10 ) + ( mode * 100 ) );
									//
									if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
									//
									j++;
								}
							}
						}
						//
					}
				}
				Temp_Use = FALSE;
				//===================================================================================================
				sprintf( Buffer2 , "" );
				//
				if ( Chamber[0] == '$' ) { // 2009.07.21
					c = 2; // 2009.07.21
//					strcpy( Chamber2 , Chamber + 1 ); // 2015.12.03
					CHAMBER_STRING_CHANGE( Chamber + 1 , Chamber2 ); // 2015.12.03
					//
					strcpy( Chamber1 , Chamber + 1 ); // 2017.08.18
				}
				else {
					c = 0; // 2009.07.21
//					CHAMBER_STRING_CHANGE_FOR_CM( Chamber , Chamber2 ); // 2015.12.03
					CHAMBER_STRING_CHANGE( Chamber , Chamber2 ); // 2015.12.03
					//
					strcpy( Chamber1 , Chamber ); // 2017.08.18
				}
				//
				if ( modeset( lastsel , MethodName , Buffer + z , c , &mode ) ) {
					//
					LogView_Open_Method_String_Change( jobindex , mindex , mode , Index , MethodName , ModeStr , Chamber2 , Chamber1 , Recipe , Temp_TM , rst , ret , &c , &f , infoinsert , findwid );
					//
					if ( ( infoinsert == 2 ) && ( f == -1 ) ) break;
					//
					if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
						if ( findmode == 1 ) {
							if ( ( fileorder < 0 ) || ( ( mc - 1 ) == fileorder ) ) { // 2012.11.29
								if ( Wafer_ID_Same( Index , findwid ) ) {
									findmode = 2;
								}
							}
						}
						else if ( findmode == 2 ) {
							findmode = 3;
						}
					}
					//
					if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
						if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
							if ( hlistview != NULL ) {
								//
								if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
								//
								errflag = Elpased_Error_String( lotrun , Time2 , Time3 ); // 2015.05.27
								//
								LogView_Open_Method_History_Insert( hlistview , j , 1 , "" , MethodName , Index , Chamber2 , Time1 , Time2 , Time3 , Recipe , Buffer + z , ModeStr , Temp_TM , errflag + ( c * 10 ) + ( mode * 100 ) );
								//
								if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
								//
								j++; // 2007.06.20
							}
						}
					}
				}
			}
		}
	}
	while ( bres );
	//=========================================================================================================
	// 2007.06.20
	//=========================================================================================================
	if ( Temp_Use ) {
		// error
		if ( modeset( lastsel , Temp_MethodName , Temp_Buffer_z , Temp_Color , &mode ) ) {
			//
			LogView_Open_Method_String_Change( jobindex , mindex , mode , Temp_Index , Temp_MethodName , Temp_Mode , Temp_Chamber , Temp_Chamber_ORG , Temp_Recipe , Temp_TM , Temp_st , Temp_et , &Temp_Color , &f , infoinsert , findwid );
			//
			if ( ( infoinsert == 2 ) && ( f == -1 ) ) {
				//
			}
			else {
				//
				if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) {
					if ( findmode == 1 ) {
						if ( ( fileorder < 0 ) || ( Temp_mc == fileorder ) ) { // 2012.11.29
							if ( Wafer_ID_Same( Temp_Index , findwid ) ) {
								findmode = 2;
							}
						}
					}
					else if ( findmode == 2 ) {
						findmode = 3;
					}
				}
				//
				if ( ( findmode == 0 ) || ( findmode == 2 ) ) {
					if ( ( infoinsert != 2 ) && ( ( infoinsert == 0 ) || ( findwid != -1 ) ) ) {
						if ( hlistview != NULL ) {
							//
							if ( !xListView_Enter() ) {	fclose( fpt );	return FALSE;	}
							//
							LogView_Open_Method_History_Insert( hlistview , j , 1 , "" , Temp_MethodName , Temp_Index , Temp_Chamber , Temp_Time1 , Temp_Time2 , Temp_Time3 , Temp_Recipe , Temp_Buffer_z , Temp_Mode , Temp_TM , 1 + ( Temp_Color * 10 ) + ( mode * 100 ) );
							//
							if ( !xListView_Leave() ) {	fclose( fpt );	return FALSE;	}
							//
							j++;
						}
					}
				}
				//
			}
		}
	}
	//=========================================================================================================
	fclose( fpt );
	return TRUE;
}
//
BOOL LogView_Open_Method_History_Data_Set( HWND hdlg , int winpos , BOOL sepload , int index , int infoinsert , int findwid , int fileorder ) {
	//-----------------------------------------------------------------
	BOOL Res;
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_Start( hdlg );
	//-----------------------------------------------------------------
	Res = LOG_OPEN_CONTROL( hdlg , GetDlgItem( hdlg , winpos ) , sepload , NULL , index , infoinsert , findwid , fileorder , 0 , 0 , LogView_Open_Method_History_Data_Set_Callback );
	//-----------------------------------------------------------------
	if ( !sepload ) Data_Read_End( hdlg );
	//-----------------------------------------------------------------
	return Res;
	//-----------------------------------------------------------------
}

//

BOOL LogView_Open_Process_Log_Get_FileName( HWND hWnd , int index , int slot , char *filename , char *getMdlStr ) {
	char jobinfo[256];
//	char ChamberString[32]; // 2016.12.17
	char Methodstring[32];
	char Slotstring[32];
	char folderstr[256];
	int mode;

	strcpy( filename , "" );
	JOB_Get_Display_String( index , jobinfo );
	//
//	if ( GET_METHOD_INFO_BOX( index , hWnd , slot , "Select Process Data Part" , jobinfo , Slotstring , ChamberString , Methodstring , TRUE , 0 , &mode ) == 1 ) { // 2016.12.17
	if ( GET_METHOD_INFO_BOX( index , hWnd , slot , "Select Process Data Part" , jobinfo , Slotstring , getMdlStr , Methodstring , TRUE , 0 , &mode ) == 1 ) { // 2016.12.17
		//
		JOB_Get_Directory( index , folderstr );
		//
//		Get_Log_File_Name( filename , "PROCESS" , 0 , JOB_Get_Directory( index ) , FALSE , 0 , 0 , Slotstring , 0 , ChamberString , "" ); // 2016.12.17
		Get_Log_File_Name( filename , "PROCESS" , 0 , folderstr , FALSE , 0 , 0 , Slotstring , 0 , getMdlStr , "" ); // 2016.12.17
		//
		return TRUE;
	}
	return FALSE;
}
//
//
//BOOL LogView_Open_Process_Log_Ext( HWND hWnd , int wpos , int index , int extview , int slot , char *Message , char *filename ) { // 2016.12.22
BOOL LogView_Open_Process_Log_Ext( HWND hWnd , int wpos , int index , int extview , int slot , char *Message , char *filename , char *getmdlstr ) { // 2016.12.22
	int Res , mode;
	char Buffer2[256];
	char jobinfo[256];
//	char ChamberString[32]; // 2016.12.22
	char Methodstring[32];
	char Slotstring[32];
	char folderstr[256];
	ViewStepTemplate data;
	strcpy( filename , "" );
	strcpy( Message , "" );
	JOB_Get_Display_String( index , jobinfo );
	while ( TRUE ) {
//		Res = GET_METHOD_INFO_BOX( index , hWnd , slot , "Select Process Data Part" , jobinfo , Slotstring , ChamberString , Methodstring , TRUE , extview , &mode ); // 2016.12.22
		Res = GET_METHOD_INFO_BOX( index , hWnd , slot , "Select Process Data Part" , jobinfo , Slotstring , getmdlstr , Methodstring , TRUE , extview , &mode ); // 2016.12.22
		if ( Res == 1 ) {
			data.index = index;
			strncpy( data.slot , Slotstring , 63 );
			if      ( extview == 5 ) { // 2007.06.21
				data.mode  = 6;
			}
			else if ( extview == 4 ) { // 2007.06.21
				data.mode  = 5;
			}
			else if ( extview == 3 ) { // 2006.02.23
				data.mode  = 4;
			}
			else if ( extview == 2 ) { // 2006.02.23
				data.mode  = 3;
			}
			else if ( extview == 1 ) { // 2005.02.14
				data.mode  = 2; // 2005.02.14
			} // 2005.02.14
			else {
				data.mode  = 1;
			}
//			strncpy( data.chstr , ChamberString , 63 ); // 2016.12.22
			strncpy( data.chstr , getmdlstr , 63 ); // 2016.12.22
			strncpy( data.str , Methodstring , 255 );
			//
			if ( wpos <= 0 ) {
				//
				if ( LISTVIEW_HEIGHT_UPDATE ) { // 2015.07.29
					if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_Process_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
						return FALSE;
					}
				}
				else {
					if ( Get_Prm_MINISCREEN_MODE() ) {
						if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_Process_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
							return FALSE;
						}
					}
					else {
						if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_Process_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
							return FALSE;
						}
					}
				}
				if ( !Get_Prm_WINDOW_RETURN_MODE() ) return FALSE;
			}
			else {
				//
//				sprintf( Message , "* PROCESS DATA : %s %s : %s" , ChamberString , Slotstring , jobinfo ); // 2016.12.22
				sprintf( Message , "* PROCESS DATA : %s %s : %s" , getmdlstr , Slotstring , jobinfo ); // 2016.12.22
				//
				JOB_Get_Directory( index , folderstr );

//				Get_Log_File_Name( filename , "PROCESS" , extview , JOB_Get_Directory( index ) , FALSE , 0 , 0 , Slotstring , 0 , ChamberString , "" ); // 2016.12.22
				Get_Log_File_Name( filename , "PROCESS" , extview , folderstr , FALSE , 0 , 0 , Slotstring , 0 , getmdlstr , "" ); // 2016.12.22
				//
				if ( !LogView_Open_Process_Log_Data_Set( hWnd , wpos , filename , Buffer2 , FALSE , 0 , 0 , 0 , TRUE , FALSE ) ) {
					return FALSE;
				}
				//
				return TRUE;
			}
		}
		else if ( Res == 0 ) {
			return FALSE;
		}
		else {
			if ( wpos > 0 ) return FALSE;
			return Get_Prm_WINDOW_RETURN_MODE();
		}
	}
	return FALSE;
}
//
BOOL LogView_Open_Process_Log( HWND hWnd , int index , int extview ) {
	char Message[256];
	char filename[256];
	char getmdlstr[64];
//	return LogView_Open_Process_Log_Ext( hWnd , -1 , index , extview , -1 , Message , filename ); // 2016.12.22
	return LogView_Open_Process_Log_Ext( hWnd , -1 , index , extview , -1 , Message , filename , getmdlstr ); // 2016.12.22
}

//
BOOL LogView_Open_LotPM_Log_Ext( HWND hWnd , int wpos , int index , int extview , char *Message ) { // 2005.02.21
	char jobinfo[256]; // 2009.11.20
	ViewStepTemplate data;

	JOB_Get_Display_String( index , jobinfo );

	data.mode = extview;
	data.index = index;
	strncpy( data.str , jobinfo , 127 );
	//
	if ( wpos <= 0 ) {
		//
		if ( LISTVIEW_HEIGHT_UPDATE ) { // 2015.07.29
			if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_LotPM_Log_Proc , (LPARAM) &data ) ) {
				return FALSE;
			}
		}
		else {
			if ( Get_Prm_MINISCREEN_MODE() ) {
				if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_LotPM_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
					return FALSE;
				}
			}
			else {
				if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_LotPM_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
					return FALSE;
				}
			}
		}
	}
	else {
		//
		sprintf( Message , "* LOT PM DATA : %s" , jobinfo );
		//
		if ( !LogView_Open_LotPM_Log_Data_Set( hWnd , wpos , extview , index , TRUE ) ) {
			return FALSE;
		}
		//
		return TRUE;
	}
	if ( wpos > 0 ) return TRUE;
	return Get_Prm_WINDOW_RETURN_MODE();
}

BOOL LogView_Open_LotPM_Log( HWND hWnd , int index , int extview ) {
	char Message[256];
	return LogView_Open_LotPM_Log_Ext( hWnd , -1 , index , extview , Message );
}

//
BOOL _Info_Display_Function_Run( HWND hWnd , char *schpath , char *rcppath , int ch , BOOL bmmode , char *filename );
//
//BOOL LogView_Open_Recipe_Log_Ext( HWND hWnd , int wpos , int index , int extview , int slot , char *Message , char *filename , int smode ) { // 2016.12.22
BOOL LogView_Open_Recipe_Log_Ext( HWND hWnd , int wpos , int index , int extview , int slot , char *Message , char *filename , int smode , char *getmdlstr ) { // 2016.12.22
	int Res , mode , ch;
	char Buffer2[256]; // 2009.11.20
	char jobinfo[256];
//	char ChamberString[32]; // 2016.12.22
	char Methodstring[32];
	char Slotstring[32];
	char folderstr[256];

	ViewStepTemplate data;
	strcpy( filename , "" );
	strcpy( Message , "" );
	JOB_Get_Display_String( index , jobinfo );
	while ( TRUE ) {
//		Res = GET_METHOD_INFO_BOX( index , hWnd , slot , "Select Recipe Data Part" , jobinfo , Slotstring , ChamberString , Methodstring , FALSE , extview , &mode ); // 2016.12.22
		Res = GET_METHOD_INFO_BOX( index , hWnd , slot , "Select Recipe Data Part" , jobinfo , Slotstring , getmdlstr , Methodstring , FALSE , extview , &mode ); // 2016.12.22
		if ( Res == 1 ) {
			if ( mode == 0 ) {
				data.mode = extview; // 2006.02.23
				data.index = index;
				strncpy( data.slot , Slotstring , 63 );
//				strncpy( data.chstr , ChamberString , 63 ); // 2016.12.22
				strncpy( data.chstr , getmdlstr , 63 ); // 2016.12.22
				strncpy( data.str , Methodstring , 255 );
				//
				if ( wpos <= 0 ) {
					//
					if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW2 , IDD_E_LOG_VIEW2 ) , hWnd , LogView_Open_Recipe_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
						return FALSE;
					}
					//
				}
				else {
					//
					JOB_Get_Directory( index , folderstr );

//					Get_Log_File_Name( filename , "RECIPE" , extview , JOB_Get_Directory( index ) , FALSE , 0 , 0 , Slotstring , 0 , ChamberString , "" ); // 2016.12.22
					Get_Log_File_Name( filename , "RECIPE" , extview , folderstr , FALSE , 0 , 0 , Slotstring , 0 , getmdlstr , "" ); // 2016.12.22
					//
//					sprintf( Message , "* RECIPE DATA : %s %s : %s" , ChamberString , Slotstring , jobinfo ); // 2016.12.22
					sprintf( Message , "* RECIPE DATA : %s %s : %s" , getmdlstr , Slotstring , jobinfo ); // 2016.12.22
					//
					if ( !LogView_Open_Recipe_Log_Data_Set( hWnd , wpos , filename , Buffer2 , smode , TRUE ) ) {
						return FALSE;
					}
					//
					return TRUE;
				}
				//
			}
			else {
				//----------------------------------------------------------------------------------------------
				//
				JOB_Get_Directory( index , folderstr );
					//
//				Get_Log_File_Name( filename , "RECIPE" , extview , JOB_Get_Directory(index) , FALSE , 0 , 0 , Slotstring , 0 , ChamberString , "_FILE" ); // 2016.12.22
				Get_Log_File_Name( filename , "RECIPE" , extview , folderstr , FALSE , 0 , 0 , Slotstring , 0 , getmdlstr , "_FILE" ); // 2016.12.22
				//----------------------------------------------------------------------------------------------
//				ch = atoi( ChamberString + 2 ) - 1; // 2016.12.22
				ch = atoi( getmdlstr + 2 ) - 1; // 2016.12.22
//				if ( !_Info_Display_Function_Run( hWnd , Get_Prm_SYSTEM_DIRECTORY() , "." , ch , ( ChamberString[0] == 'B' ) , filename ) ) { // 2016.12.22
				if ( !_Info_Display_Function_Run( hWnd , Get_Prm_SYSTEM_DIRECTORY() , "." , ch , ( getmdlstr[0] == 'B' ) , filename ) ) { // 2016.12.22
					MODAL_WINDOW_MESSAGE_BOX( hWnd , "[ERROR] File Viewer can not find" , "Error" , MB_ICONHAND );
					return FALSE;
				}
				//
				return FALSE;
			}
			//
			if ( wpos > 0 ) return TRUE;
			//
			if ( !Get_Prm_WINDOW_RETURN_MODE() ) return FALSE;
		}
		else if ( Res == 0 ) {
			return FALSE;
		}
		else {
			if ( wpos > 0 ) return FALSE;
			//
			return Get_Prm_WINDOW_RETURN_MODE();
		}
	}
	return FALSE;
}
//
BOOL LogView_Open_Recipe_Log( HWND hWnd , int index , int extview ) {
	char Message[256];
	char filename[256];
	char getmdlstr[64]; // 2016.12.22
//	return LogView_Open_Recipe_Log_Ext( hWnd , -1 , index , extview , -1 , Message , filename , 0 ); // 2016.12.22
	return LogView_Open_Recipe_Log_Ext( hWnd , -1 , index , extview , -1 , Message , filename , 0 , getmdlstr ); // 2016.12.22
}

//
BOOL LogView_Open_Lot_Log_Ext( HWND hWnd , int wpos , int index , int extview , BOOL selmode , char *Message ) { // 2012.06.26
	int j = -1 , Res;
	char jobinfo[256];
	ViewStepTemplate data;
	JOB_Get_Display_String( index , jobinfo );
	//
	if ( selmode ) { // 2012.06.26
		Res = MODAL_DIGITAL_BOX1( hWnd , jobinfo , "Select Lot Log Type" , "Min Value|Max Value|Average Value|All Value" , &j );
	}
	else {
		if ( wpos <= 0 ) {
			Res = MODAL_DIGITAL_BOX1( hWnd , jobinfo , "Select Lot Log Type" , "Min Value|Max Value|Average Value|All Value" , &j );
		}
		else {
			Res = TRUE;
			j = 3;
		}
	}
	//
	if ( Res ) {
		//
		if ( wpos > 0 ) {
			//
			if ( !LogView_Open_LotModule_Log_Data_Set( hWnd , wpos , j , index , TRUE , 2 , 0 , -1 , extview ) ) return FALSE;
			//
			sprintf( Message , "* LOT DATA : %s" , jobinfo );
		}
		else {
			//
			Make_Estimate_LogData_for_Min_Max_Avr( FALSE , 2 , 0 , -1 , index , "" , "" , j , FALSE , extview , FALSE );
			data.mode  = j;
			data.index = index;
			//
			if ( LISTVIEW_HEIGHT_UPDATE ) { // 2015.07.29
				if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_LotModule_Log_Proc , (LPARAM) &data ) ) {
					return FALSE;
				}
			}
			else {
				if ( Get_Prm_MINISCREEN_MODE() ) {
					if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_LotModule_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
						return FALSE;
					}
				}
				else {
					if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_LotModule_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
						return FALSE;
					}
				}
			}
		}
	}
	if ( wpos > 0 ) return TRUE;
	return Get_Prm_WINDOW_RETURN_MODE();
}

//
BOOL LogView_Open_Lot_Log( HWND hWnd , int index , int extview ) {
	char Message[256];
//	return LogView_Open_Lot_Log_Ext( hWnd , -1 , index , extview , Message ); // 2012.06.26
	return LogView_Open_Lot_Log_Ext( hWnd , -1 , index , extview , TRUE , Message ); // 2012.06.26
}

//
BOOL LogView_Open_Lot_Log_S( HWND hWnd , int index , int extview , BOOL selmode ) { // 2012.06.26
	char Message[256];
	return LogView_Open_Lot_Log_Ext( hWnd , -1 , index , extview , selmode , Message );
}

//
//BOOL LogView_Open_Module_Log_Ext( HWND hWnd , int wpos , int index , int extview , int slot , int pmch , char *Message ) { // 2012.06.26
BOOL LogView_Open_Module_Log_Ext( HWND hWnd , int wpos , int index , int extview , BOOL selmode , int slot , int pmch , char *Message ) { // 2012.06.26
	int k = -1 , j = -1 , Res , mr;
	char jobinfo[256];
	ViewStepTemplate data;
	strcpy( Message , "" );
	JOB_Get_Display_String( index , jobinfo );
	while( TRUE ) {
		Res = GET_CHAMBER_INFO_BOX( index , hWnd , slot , pmch , "Select Module for Process Log Data" , jobinfo , &k );
		if ( Res == 1 ) {
//			if ( Get_Prm_DISPLAY_MODE() == 0 ) { // 2011.07.12 // 2012.06.26
			if ( selmode ) { // 2011.07.12 // 2012.06.26
				mr = MODAL_DIGITAL_BOX1( hWnd , jobinfo , "Select Lot Log Type for Module" , "Min Value|Max Value|Average Value|All Value" , &j );
			}
			else {
				j = 3;
				mr = TRUE;
			}
			//
			if ( mr ) {
				//
				if ( wpos <= 0 ) {
					//
					Make_Estimate_LogData_for_Min_Max_Avr( FALSE , 1 , k , slot , index , "" , "" , j , FALSE , extview , FALSE );
					//
					data.mode  = j;
					data.index = index;
					//
					if ( LISTVIEW_HEIGHT_UPDATE ) { // 2015.07.29
						if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M2 , IDD_E_LOG_VIEW_M2 ) , hWnd , LogView_Open_LotModule_Log_Proc , (LPARAM) &data ) ) {
							return FALSE;
						}
					}
					else {
						if ( Get_Prm_MINISCREEN_MODE() ) {
							if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW_M , IDD_E_LOG_VIEW_M ) , hWnd , LogView_Open_LotModule_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
								return FALSE;
							}
						}
						else {
							if ( !GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_LOG_VIEW , IDD_E_LOG_VIEW ) , hWnd , LogView_Open_LotModule_Log_Proc , (LPARAM) &data ) ) { // 2004.01.19 // 2004.08.10
								return FALSE;
							}
						}
					}
				}
				else {
					//
					if ( !LogView_Open_LotModule_Log_Data_Set( hWnd , wpos , j , index , TRUE , 1 , k , slot , extview ) ) return FALSE;
					//
					if ( k <= 0 ) {
						sprintf( Message , "* PROCESS MODULE ALL DATA : %s" , jobinfo );
					}
					else {
						if ( slot <= 0 ) {
							sprintf( Message , "* PROCESS MODULE DATA : PM%d : %s" , k , jobinfo );
						}
						else {
							sprintf( Message , "* PROCESS MODULE DATA : PM%d %d : %s" , k , slot , jobinfo );
						}
					}
				}
				//				
				if ( wpos > 0 ) return TRUE;
				return Get_Prm_WINDOW_RETURN_MODE();
			}
			else {
				if ( wpos > 0 ) return FALSE;
				return Get_Prm_WINDOW_RETURN_MODE();
			}
		}
		else if ( Res == 0 ) {
			return FALSE;
		}
		else {
			if ( wpos > 0 ) return FALSE;
			return Get_Prm_WINDOW_RETURN_MODE();
		}
	}
}

BOOL LogView_Open_Module_Log( HWND hWnd , int index , int extview ) {
	char Message[256];
//	return LogView_Open_Module_Log_Ext( hWnd , -1 , index , extview , -1 , -1 , Message ); // 2012.06.26
	return LogView_Open_Module_Log_Ext( hWnd , -1 , index , extview , TRUE , -1 , -1 , Message ); // 2012.06.26
}

BOOL LogView_Open_Module_Log_S( HWND hWnd , int index , int extview , BOOL selmode ) { // 2012.06.26
	char Message[256];
	return LogView_Open_Module_Log_Ext( hWnd , -1 , index , extview , selmode , -1 , -1 , Message );
}
