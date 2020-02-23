#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlfile.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>
#include <Kutlanal.h>

#include "resource.h"

#include "cimseqnc.h"
#include "iodefine.h"
#include "ctl_code.h"
#include "plt_code.h"
#include "plt_parm.h"
#include "dat_code.h"

#include <CimSeqncKNFS.h>

//------------------------------------------------------------------------------------------
BOOL Util_Prepare_Folder( char * );
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
#define	GUI_REGIN_Y_SIZE	207
//------------------------------------------------------------------------------------------
int PLOTTER_POS_X;
int PLOTTER_POS_Y;
int PLOTTER_POS_XS;
int PLOTTER_POS_YS;
//
HWND PLOT_MAIN_WINDOW = NULL;
HWND PLOT_SCREEN_CONTROL;
FARPROC PLOT_PROC_CONTROL;
BOOL GUI_DRAW_USE = FALSE;
int  GUI_DRAW_POLL_TIME = 0;
//------------------------------------------------------------------------------------------
extern	int	 AUTO_SCALE_POLL_TIME;
extern	int  HEADER_MESSAGE_IO;
extern	char HEADER_MESSAGE_STRING[256];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
HINSTANCE	BackBmpLib = NULL;
void		KGUI_Set_BACKGROUND( char *file ) { BackBmpLib = LoadLibrary( file ); }
HINSTANCE	KGUI_Get_BACKGROUND() { return BackBmpLib; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  PROGRAM_GUI_SPAWNING = 0; // 2003.07.12
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	int i;
	if ( no == -1 ) {
		KGUI_STANDARD_Window_Kill_Signal();
		KGUI_STANDARD_Display_Information_DeInitial();
	}
	if ( ( no > 0 ) && ( PROGRAM_GUI_SPAWNING == 0 ) ) { // 2003.07.12
		PROGRAM_GUI_SPAWNING = 1;
		for ( i = 0 ; i < 200 ; i++ ) {
			_sleep(100);
			if ( PROGRAM_GUI_SPAWNING == 2 ) break;
		}
	}
	KGUI_STANDARD_Screen_Nav_Info( no );
	if ( GUI_DRAW_USE ) { // 2004.09.13
		if ( PLOT_MAIN_WINDOW != NULL ) {
			if ( no == 0 ) {
				MoveWindow( PLOT_MAIN_WINDOW , 0 , 0 , 0 , 0 , TRUE );
			}
			else {
				MoveWindow( PLOT_MAIN_WINDOW , PLOTTER_POS_X , PLOTTER_POS_Y , PLOTTER_POS_XS , PLOTTER_POS_YS , TRUE );
			}
		}
	}
	if ( no == 0 ) { // 2003.02.11
		if ( V_AUTO_RETURN_TO_MONITOR() ) {
			if ( !DATA_LOGGING_STATUS() ) {
				if ( DISP_GO_READ_MODE_GET() ) {
					Plotting_Zoom_Recover( -1 );
					DISP_GO_READ_MODE_SET( FALSE );
				}
			}
		}
	}
}
//
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//char COLOR_SELECT[2048];
char COLOR_SELECT[4096]; // 2002.09.17
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
#define		SCR_BUTTON_XSTART		869
#define		SCR_BUTTON_YSTART		420
#define		SCR_BUTTON_YINTERVAL	38
#define		SCR_BUTTON_XSIZE		30
#define		SCR_BUTTON_YSIZE		30

//------------------------------------------------------------------------
int  Console_Zoom_Ready[2] = { 0 , 0 };
int  Console_x[2] , Console_y[2];
int  Console_x2[2] , Console_y2[2];
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Console_Drawing_Zoom_Reset() {
	if ( ( Console_Zoom_Ready[0] == 1 ) || ( Console_Zoom_Ready[0] == 2 ) ) {
		Console_Zoom_Ready[0] = 3;
	}
	if ( ( Console_Zoom_Ready[1] == 1 ) || ( Console_Zoom_Ready[1] == 2 ) ) {
		Console_Zoom_Ready[1] = 3;
	}
}

BOOL Console_Drawing_Zoom_Draw_Check( int SCR ) {
	if ( Console_Zoom_Ready[SCR] == 1 ) {
		Console_Zoom_Ready[SCR] = 2;
		return TRUE;
	}
	if ( Console_Zoom_Ready[SCR] == 3 ) {
		Console_Zoom_Ready[SCR] = 0;
		return TRUE;
	}
	return FALSE;
}

void Console_Drawing_Zoom_Set( int SCR , int X , int Y ) {
	Console_Zoom_Ready[SCR] = 1;
	Console_x[SCR] = X;
	Console_y[SCR] = Y;
}

void Console_Drawing_Zoom_Run( HWND hdlg , int SCR , int X , int Y ) {
	Console_x2[SCR] = X;
	Console_y2[SCR] = Y;
	if ( Plotting_Data_Set_For_Zoom_Scale( SCR , Console_x[SCR] , Console_x2[SCR] , Console_y[SCR] , Console_y2[SCR] ) ) {
		InvalidateRect( hdlg , NULL , TRUE );
	}
}

BOOL Console_Drawing_Zoom_Ready( int SCR , int *x , int *y ) {
	*x = Console_x[SCR];
	*y = Console_y[SCR];
	if ( ( Console_Zoom_Ready[SCR] == 1 ) || ( Console_Zoom_Ready[SCR] == 2 ) ) return TRUE;
	else return FALSE;
}

void Console_Drawing_Left_Button2( HWND hdlg , int SCR , int x , int y ) {
	if ( V_AUTO_SCALE_DRAW() ) return;
	if ( ( Console_Zoom_Ready[SCR] == 1 ) || ( Console_Zoom_Ready[SCR] == 2 ) ) {
		Console_Zoom_Ready[SCR] = 3;
		Console_Drawing_Zoom_Run( hdlg , SCR , x , y );
	}
	else {
		Console_Drawing_Zoom_Set( SCR , x , y );
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Console_Drawing_Right_Button2_2( HWND hdlg , int SCR ) {
	HDC hDC;
	int iRes , iBuf;
	iRes = 0;
	if ( MODAL_DIGITAL_BOX1( hdlg ,
		"Select" ,
		"Where Color" ,
		"L-No1|L-No2|R-No1|R-No2|BackGround" ,
		&iRes ) ) {
		if ( iRes == 4 ) {
			if ( V_PLOTTER_BACK_COLOR( SCR ) >= 256 ) iBuf = V_PLOTTER_BACK_COLOR( SCR ) - 256;
			else                                      iBuf = V_PLOTTER_BACK_COLOR( SCR ) + 20;
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select BackGround" , "Color" , COLOR_SELECT , &iBuf ) ) {
				if ( iBuf < 20 ) iBuf = iBuf + 256;
				else             iBuf = iBuf - 20;
				if ( V_PLOTTER_BACK_COLOR( SCR ) != iBuf ) {
					S_PLOTTER_BACK_COLOR( SCR , iBuf );
					hDC = GetDC( hdlg );
					Console_Gui_For_Drawing( hDC , SCR );
					ReleaseDC( hdlg , hDC );
				}
			}
		}
		else {
			if ( V_PLOTTER_COLOR( SCR , iRes ) >= 256 ) iBuf = V_PLOTTER_COLOR( SCR , iRes ) - 256;
			else                                        iBuf = V_PLOTTER_COLOR( SCR , iRes ) + 20;
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Color" , COLOR_SELECT , &iBuf ) ) {
				if ( iBuf < 20 ) iBuf = iBuf + 256;
				else             iBuf = iBuf - 20;
				if ( V_PLOTTER_COLOR( SCR , iRes ) != iBuf ) {
					S_PLOTTER_COLOR( SCR , iRes , iBuf );
					hDC = GetDC( hdlg );
					if ( iRes == 0 || iRes == 1 ) ConGui_Display_Plot_Item( hDC , SCR*2 + 1 );
					else                          ConGui_Display_Plot_Item( hDC , SCR*2 + 2 );
					Console_Gui_For_Drawing( hDC , SCR );
					ReleaseDC( hdlg , hDC );
				}
			}
		}
	}
}
//------------------------------------------------------------------------
void Console_Drawing_Right_Button2( HWND hdlg , int SCR ) {
	HDC hDC;
	char Buffer[256];
	int iBuf , iRes , no = 0;
	BOOL b;
	if      ( !(V_PLOTTER_BACK_DRAW(SCR)) && !(V_PLOTTER_POINT(SCR)) ) iRes = 0;
	else if ( !(V_PLOTTER_BACK_DRAW(SCR)) &&  (V_PLOTTER_POINT(SCR)) ) iRes = 1;
	else if (  (V_PLOTTER_BACK_DRAW(SCR)) && !(V_PLOTTER_POINT(SCR)) ) iRes = 2;
	else if (  (V_PLOTTER_BACK_DRAW(SCR)) &&  (V_PLOTTER_POINT(SCR)) ) iRes = 3;
	iBuf = iRes;
	b = V_PLOTTER_BIG(SCR);
	strcpy( Buffer ,  "Back(x),Point(x)    " );	if ( iRes == 0 ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Back(x),Point(o)    " );	if ( iRes == 1 ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Back(o),Point(x)    " );	if ( iRes == 2 ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Back(o),Point(o)    " );	if ( iRes == 3 ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Linear Line         " );	if ( V_LINE_DRAW_TYPE(SCR) == LIN_TYPE ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Rectangle Line      " );	if ( V_LINE_DRAW_TYPE(SCR) == REC_TYPE ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Standalone SmallSize" );	if ( !b ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Standalone FullSize " );	if ( b  ) strcat( Buffer , " <======" );
	strcat( Buffer , "|Color Define" );
	if ( MODAL_DIGITAL_BOX1( hdlg ,
		"Select" ,
		"BGline/PntGuide/LineType/StdScale/.." ,
		Buffer ,
		&iRes ) ) {
		if ( iRes != iBuf ) {
			if      ( iRes == 0 ) {	S_PLOTTER_BACK_DRAW(SCR,FALSE); S_PLOTTER_POINT(SCR,FALSE); }
			else if ( iRes == 1 ) {	S_PLOTTER_BACK_DRAW(SCR,FALSE); S_PLOTTER_POINT(SCR,TRUE);  }
			else if ( iRes == 2 ) {	S_PLOTTER_BACK_DRAW(SCR,TRUE);  S_PLOTTER_POINT(SCR,FALSE); }
			else if ( iRes == 3 ) {	S_PLOTTER_BACK_DRAW(SCR,TRUE);  S_PLOTTER_POINT(SCR,TRUE);  }
			else if ( iRes == 4 ) {	S_LINE_DRAW_TYPE(SCR,LIN_TYPE); }
			else if ( iRes == 5 ) {	S_LINE_DRAW_TYPE(SCR,REC_TYPE); }

			else if ( iRes == 6 ) {
				S_PLOTTER_BIG(SCR,FALSE);
				if ( b != V_PLOTTER_BIG(SCR) ) no = -1;
			}
			else if ( iRes == 7 ) {
				S_PLOTTER_BIG(SCR,TRUE);
				if ( b != V_PLOTTER_BIG(SCR) ) no = -1;
			}
			else if ( iRes == 8 ) {
				Console_Drawing_Right_Button2_2( hdlg , SCR );
			}
			if ( iRes != 8 ) {
				if ( no == -1 ) {
					InvalidateRect( hdlg , NULL , TRUE );
				}
				else {
					hDC = GetDC( hdlg );
					Console_Gui_For_Drawing( hDC , SCR );
					ReleaseDC( hdlg , hDC );
				}
			}
		}
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
	
#define REFPLT_INTERVAL 60
#define REFPLT_POSITION 15
#define REFPLT2_POSITION 30

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Draw_Time_Data_Clear( HDC hDC ) {
	int i;
	KGUI_DRAW_StrBox( hDC , PLT0_X1_START_TIME , PLT_Y_START_TIME ,	"" , PLT_X_SIZE_TIME ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	KGUI_DRAW_StrBox( hDC , PLT0_X2_START_TIME , PLT_Y_START_TIME ,	"" , PLT_X_SIZE_TIME ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	KGUI_DRAW_StrBox( hDC , PLT1_X1_START_TIME , PLT_Y_START_TIME ,	"" , PLT_X_SIZE_TIME ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	KGUI_DRAW_StrBox( hDC , PLT1_X2_START_TIME , PLT_Y_START_TIME ,	"" , PLT_X_SIZE_TIME ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	KGUI_DRAW_StrBox( hDC , PLT2_X1_START_TIME , PLT_Y_START_TIME ,	"" , PLT_X_SIZE_TIME ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	KGUI_DRAW_StrBox( hDC , PLT2_X2_START_TIME , PLT_Y_START_TIME ,	"" , PLT_X_SIZE_TIME ,
		PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	//
	for ( i = 0 ; i < 13 ; i++ ) {
		KGUI_DRAW_StrBox( hDC , PLT1_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , PLT0_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , PLT1_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , PLT0_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , PLT2_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , PLT0_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , PLT2_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
		KGUI_DRAW_StrBox( hDC , PLT0_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME , "" , 4 ,
			PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO , PLOTTER_COLOR_FOR_INFO );
	}
	//
}
//------------------------------------------------------------------------
void Draw_Time_Data( HDC hDC , int dm , int mode , int PT , int T1 , int T2 , int color2 , BOOL Atset ) {
	char Buffer1[64], Buffer2[64];
	long time_data1 , time_data2;
	int i , k , x , z , color;
	memset(Buffer1			,0x00, sizeof(Buffer1));
	memset(Buffer2 			,0x00, sizeof(Buffer2));

	if ( PT < 0 ) {
		time_data1 = -1;
		time_data2 = -1;
	}
	else {
		time_data1 = A_IO_PLT_TIME( PT , T1 );
		time_data2 = A_IO_PLT_TIME( PT , T2 );
	}

	if ( Atset ) {
		color = PLOTTER_COLOR_FOR_INFO;
		if ( dm == SCR_SMALL_DRAW ) k = 6; else k = 13;
		x = Get_Graph_Plotting_Count_Just_Single( A_IO_PLT_DEV( 0 ) );
		for ( i = 0 ; i < k ; i++ ) {
			if ( ( PT < 0 ) || ( T1 < 0 ) || ( T2 < 0 ) ) {
				strcpy( Buffer1 , "" );
			}
			else {
				z = ( x * 2 * i ) + T1;
				if ( z > T2 ) {
					strcpy( Buffer1 , "" );
				}
				else {
					sprintf( Buffer1 , "%ld" , A_IO_PLT_TIME( PT , z ) );
				}
			}
			switch ( mode ) {
			case SCREEN_1 :
				if ( i == (k-1) ) {
					if ( dm == SCR_SMALL_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT1_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
					else if ( dm == SCR_BIG_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT0_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
				}
				else {
					if ( dm == SCR_SMALL_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT1_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
					else if ( dm == SCR_BIG_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT0_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
				}
				break;
			case SCREEN_2 :
				if ( i == (k-1) ) {
					if ( dm == SCR_SMALL_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT2_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
					else if ( dm == SCR_BIG_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT0_X2_START_TIME + REFPLT2_POSITION , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
				}
				else {
					if ( dm == SCR_SMALL_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT2_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
					else if ( dm == SCR_BIG_DRAW ) {
						KGUI_DRAW_StrBox( hDC , PLT0_X1_START_TIME - REFPLT_POSITION + (i * REFPLT_INTERVAL) , PLT_Y_START_TIME ,	Buffer1 , 4 ,
							color , color , COLOR_Black , color );
					}
				}
				break;
			}
		}
	}
	else {
		color = color2;
		Get_Long_Time_To_String( time_data1 , Buffer1 );
		Get_Long_Time_To_String( time_data2 , Buffer2 );
		switch ( mode ) {
			case SCREEN_1 :
				if ( dm == SCR_SMALL_DRAW ) {
					KGUI_DRAW_StrBox( hDC , PLT1_X1_START_TIME , PLT_Y_START_TIME ,	Buffer1 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
					KGUI_DRAW_StrBox( hDC , PLT1_X2_START_TIME , PLT_Y_START_TIME ,	Buffer2 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
				}
				else if ( dm == SCR_BIG_DRAW ) {
					KGUI_DRAW_StrBox( hDC , PLT0_X1_START_TIME , PLT_Y_START_TIME ,	Buffer1 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
					KGUI_DRAW_StrBox( hDC , PLT0_X2_START_TIME , PLT_Y_START_TIME ,	Buffer2 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
				}
				break;
			case SCREEN_2 :
				if ( dm == SCR_SMALL_DRAW ) {
					KGUI_DRAW_StrBox( hDC , PLT2_X1_START_TIME , PLT_Y_START_TIME ,	Buffer1 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
					KGUI_DRAW_StrBox( hDC , PLT2_X2_START_TIME , PLT_Y_START_TIME ,	Buffer2 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
				}
				else if ( dm == SCR_BIG_DRAW ) {
					KGUI_DRAW_StrBox( hDC , PLT0_X1_START_TIME , PLT_Y_START_TIME ,	Buffer1 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
					KGUI_DRAW_StrBox( hDC , PLT0_X2_START_TIME , PLT_Y_START_TIME ,	Buffer2 , PLT_X_SIZE_TIME ,
						color , color , COLOR_Black , color );
				}
				break;
		}
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Console_Drawing_Left_Button( HWND hdlg , POINT ptcursor ) {
	if ( V_PLOTTER_DRAW_MODE( SCREEN_1 ) == SCR_BIG_DRAW ) {
		if ( (	ptcursor.x >= PLT0_X_START ) &&
			 (	ptcursor.x <= PLT0_X_START + DRAW_SIZE_X_BIG ) &&
			 (	ptcursor.y >= PLT0_Y_START ) &&
			 (	ptcursor.y <= PLT0_Y_START + DRAW_SIZE_Y_BIG ) ) {
			Console_Drawing_Left_Button2( hdlg , SCREEN_1 , ptcursor.x , ptcursor.y );
		}
		else {
			Console_Drawing_Zoom_Reset();
		}
	}
	else if ( V_PLOTTER_DRAW_MODE( SCREEN_2 ) == SCR_BIG_DRAW ) {
		if ( (	ptcursor.x >= PLT0_X_START ) &&
			 (	ptcursor.x <= PLT0_X_START + DRAW_SIZE_X_BIG ) &&
			 (	ptcursor.y >= PLT0_Y_START ) &&
			 (	ptcursor.y <= PLT0_Y_START + DRAW_SIZE_Y_BIG ) ) {
			Console_Drawing_Left_Button2( hdlg , SCREEN_2 , ptcursor.x , ptcursor.y );
		}
		else {
			Console_Drawing_Zoom_Reset();
		}
	}
	else {
		if ( (	ptcursor.x >= PLT1_X_START ) &&
			 (	ptcursor.x <= PLT1_X_START + DRAW_SIZE_X_SMALL ) &&
			 (	ptcursor.y >= PLT1_Y_START ) &&
			 (	ptcursor.y <= PLT1_Y_START + DRAW_SIZE_Y_SMALL ) ) {
			Console_Drawing_Left_Button2( hdlg , SCREEN_1 , ptcursor.x , ptcursor.y );
		}
		else if ( (	ptcursor.x >= PLT2_X_START ) &&
			 (	ptcursor.x <= PLT2_X_START + DRAW_SIZE_X_SMALL ) &&
			 (	ptcursor.y >= PLT2_Y_START ) &&
			 (	ptcursor.y <= PLT2_Y_START + DRAW_SIZE_Y_SMALL ) ) {
			Console_Drawing_Left_Button2( hdlg , SCREEN_2 , ptcursor.x , ptcursor.y );
		}
		else {
			Console_Drawing_Zoom_Reset();
		}
	}
}
//------------------------------------------------------------------------
void Console_Drawing_Right_Button( HWND hdlg , POINT ptcursor ) {
	if ( (	ptcursor.x >= PLT1_X_START ) &&
		 (	ptcursor.x <= PLT1_X_START + DRAW_SIZE_X_SMALL ) &&
	     (	ptcursor.y >= PLT1_Y_START ) &&
		 (	ptcursor.y <= PLT1_Y_START + DRAW_SIZE_Y_SMALL ) ) {
		Console_Drawing_Right_Button2( hdlg , SCREEN_1 );
	}
	else if ( (	ptcursor.x >= PLT2_X_START ) &&
		 (	ptcursor.x <= PLT2_X_START + DRAW_SIZE_X_SMALL ) &&
	     (	ptcursor.y >= PLT2_Y_START ) &&
		 (	ptcursor.y <= PLT2_Y_START + DRAW_SIZE_Y_SMALL ) ) {
		Console_Drawing_Right_Button2( hdlg , SCREEN_2 );
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void KGUI_Plot_Button( HWND hdlg , BOOL Display ) {
	if ( Display ) {
		KWIN_Item_Enable( hdlg , IDB_REWIND2 );
		KWIN_Item_Enable( hdlg , IDB_REWIND );
		KWIN_Item_Enable( hdlg , IDB_FORWARD );
		KWIN_Item_Enable( hdlg , IDB_FORWARD2 );
		//
		KWIN_Item_Enable( hdlg , ID_PLOT_SCROLL_1 );
		KWIN_Item_Enable( hdlg , ID_PLOT_SCROLL_2 );
		//
		if ( DISP_LAST_POINTER[SCREEN_1] > 0 ) {
			KWIN_Item_Enable( hdlg , ID_PLOT_AUTO_1 );
			MoveWindow( GetDlgItem( hdlg , ID_PLOT_AUTO_1 ) , PLT1_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
		}
		else {
			KWIN_Item_Hide( hdlg , ID_PLOT_AUTO_1 );
		}
		if ( DISP_LAST_POINTER[SCREEN_2] > 0 ) {
			KWIN_Item_Enable( hdlg , ID_PLOT_AUTO_2 );
			MoveWindow( GetDlgItem( hdlg , ID_PLOT_AUTO_2 ) , PLT2_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
		}
		else {
			KWIN_Item_Hide( hdlg , ID_PLOT_AUTO_2 );
		}
	}
	else {
		KWIN_Item_Hide( hdlg , IDB_REWIND2 );
		KWIN_Item_Hide( hdlg , IDB_REWIND );
		KWIN_Item_Hide( hdlg , IDB_FORWARD );
		KWIN_Item_Hide( hdlg , IDB_FORWARD2 );
		//
		KWIN_Item_Hide( hdlg , ID_PLOT_SCROLL_1 );
		KWIN_Item_Hide( hdlg , ID_PLOT_SCROLL_2 );
		//
		KWIN_Item_Hide( hdlg , ID_PLOT_AUTO_1 );
		KWIN_Item_Hide( hdlg , ID_PLOT_AUTO_2 );
	}
}


void KGUI_Plot_Button_Make( HWND hdlg ) {
	CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , SCR_BUTTON_XSTART , SCR_BUTTON_YSTART + ( SCR_BUTTON_YINTERVAL * 0 ) , SCR_BUTTON_XSIZE , SCR_BUTTON_YSIZE , hdlg , (HMENU) IDB_REWIND2  , GETHINST( hdlg ) , NULL );
	CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , SCR_BUTTON_XSTART , SCR_BUTTON_YSTART + ( SCR_BUTTON_YINTERVAL * 1 ) , SCR_BUTTON_XSIZE , SCR_BUTTON_YSIZE , hdlg , (HMENU) IDB_REWIND   , GETHINST( hdlg ) , NULL );
	CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , SCR_BUTTON_XSTART , SCR_BUTTON_YSTART + ( SCR_BUTTON_YINTERVAL * 2 ) , SCR_BUTTON_XSIZE , SCR_BUTTON_YSIZE , hdlg , (HMENU) IDB_FORWARD  , GETHINST( hdlg ) , NULL );
	CreateWindowEx( 0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , SCR_BUTTON_XSTART , SCR_BUTTON_YSTART + ( SCR_BUTTON_YINTERVAL * 3 ) , SCR_BUTTON_XSIZE , SCR_BUTTON_YSIZE , hdlg , (HMENU) IDB_FORWARD2 , GETHINST( hdlg ) , NULL );
	if ( !V_AUTO_SCALE_DRAW() ) {
		CreateWindowEx( 0L , "Scrollbar" , "" ,	WS_CHILD | WS_VISIBLE | SBS_HORZ     ,
			155 , 308 , 164 , 20 , hdlg , (HMENU) ID_PLOT_SCROLL_1  , GETHINST( hdlg ) , NULL );
		CreateWindowEx( 0L , "Scrollbar" , "" ,	WS_CHILD | WS_VISIBLE | SBS_HORZ     ,
			610 , 308 , 164 , 20 , hdlg , (HMENU) ID_PLOT_SCROLL_2  , GETHINST( hdlg ) , NULL );
		CreateWindowEx( 0L , "button" , "A" , WS_CHILD | WS_VISIBLE ,
			395 , 308 , 20 , 20 , hdlg , (HMENU) ID_PLOT_AUTO_1  , GETHINST( hdlg ) , NULL );
		CreateWindowEx( 0L , "button" , "A" , WS_CHILD | WS_VISIBLE ,
			850 , 308 , 20 , 20 , hdlg , (HMENU) ID_PLOT_AUTO_2  , GETHINST( hdlg ) , NULL );
	}
	KGUI_Plot_Button( hdlg , TRUE );
}
//
//
//
//
//
//
//
void Console_Gui_HSCROLL( HWND hWnd , WPARAM wParam , LPARAM lParam ) {
	int i , id , sc , sc2 , sss , j;
	id = GetWindowLong( (HWND) lParam , GWL_ID );
	if ( id == ID_PLOT_SCROLL_1 ) {
		sss = PLT1_X2_START_TIME;
		sc = SCREEN_1;
		sc2 = ID_PLOT_AUTO_1;
	}
	else {
		sss = PLT2_X2_START_TIME;
		sc = SCREEN_2;
		sc2 = ID_PLOT_AUTO_2;
	}
	if ( DISP_LAST_POINTER[sc] < 1 ) {
//		MoveWindow( GetDlgItem( hWnd , sc2 ) , sss + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
		KWIN_Item_Enable( hWnd , sc2 );
		DISP_LAST_POINTER[sc] = A_IO_PLT_COUNT( 0 );
	}

	i = DISP_LAST_POINTER[sc];
	
	j = A_IO_PLT_COUNT( 0 ) / 10;

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
	if      ( i <= 0                  ) DISP_LAST_POINTER[sc] = 1; 
	else if ( i > A_IO_PLT_COUNT( 0 ) ) DISP_LAST_POINTER[sc] = A_IO_PLT_COUNT( 0 );
	else                                DISP_LAST_POINTER[sc] = i;
	SetScrollRange( GetDlgItem( hWnd , id ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
	SetScrollPos( GetDlgItem( hWnd , id ) , SB_CTL , DISP_LAST_POINTER[sc] , TRUE );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_GuiMain_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	HDC hDC;
	RECT rect;
	
	switch ( msg ) {
		case WM_USER:
			switch ( LOWORD( wParam ) ) {
			case WM_PAINT:
				//<<-----------------------------------------------------------
				hDC = GetDC( hdlg );
				KGUI_STANDARD_Display_for_Redraw( 0 , hDC );
				ReleaseDC( hdlg , hDC );
				//>>-----------------------------------------------------------
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
			return TRUE;

		case WM_INITDIALOG:
			//<<-----------------------------------------------------------
			//GetWindowRect( GetDesktopWindow() , &rect ); // 2004.11.22
			KGUI_STANDARD_Get_Window_Size( &rect ); // 2004.11.22
			//-----------------------------------------------------------------------
			//KGUI_s_Set_WIN_SIZE_X( 0 );
			//KGUI_s_Set_WIN_SIZE_Y( KGUI_s_WIN_SIZE_HEIGHT() + 1 );
			//KGUI_s_Set_WIN_SIZE_XS( rect.right - KGUI_s_WIN_SIZE_WIDTH() - 1 );
			//KGUI_s_Set_WIN_SIZE_YS( rect.bottom - (KGUI_s_WIN_SIZE_HEIGHT()+KGUI_s_WIN_SIZE_HEIGHT()) - 2 );
			//KGUI_IO_SIZE_FILE( "Screen_Size_Function.cfg" );
			//
		//	PLOTTER_POS_X = KGUI_s_Get_WIN_SIZE_X();
		//	PLOTTER_POS_Y = KGUI_s_Get_WIN_SIZE_Y();
		//	PLOTTER_POS_XS = KGUI_s_Get_WIN_SIZE_XS();
		//	PLOTTER_POS_YS = KGUI_s_Get_WIN_SIZE_YS();
			//
		//	KGUI_s_Set_WIN_SIZE_X( -1 );
			//
			if ( GUI_DRAW_USE ) {
				KGUI_STANDARD_Set_User_Size( hdlg , PLOTTER_POS_X , PLOTTER_POS_Y + PLOTTER_POS_YS - GUI_REGIN_Y_SIZE , PLOTTER_POS_XS , GUI_REGIN_Y_SIZE );
				//=====================================================================================================
				// 2004.11.20 (Remapping for chagne reference position)
				//=====================================================================================================
				//PLOTTER_POS_X = KGUI_s_Get_WIN_SIZE_X();
				//=====================================================================================================
			}
			else {
				KGUI_STANDARD_Set_User_Size( hdlg , 0 , 0 , 0 , 0  );
			}
			KGUI_STANDARD_Data_Monitoring( hdlg , -1 , GUI_DRAW_POLL_TIME );
			//>>-----------------------------------------------------------
			return TRUE;

		case WM_MOUSEMOVE:
			KGUI_STANDARD_Find_Action_Position( hdlg , wParam , lParam );
			//<<-------------------------------------------------------------------------
			return TRUE;

		case WM_HSCROLL:
			//>>-------------------------------------------------------------------------
			if ( KGUI_STANDARD_Redraw_with_HSCROLL( hdlg , wParam , lParam ) ) return TRUE;
			//<<-------------------------------------------------------------------------
			return TRUE;

		case WM_VSCROLL:
			//>>-------------------------------------------------------------------------
			if ( KGUI_STANDARD_Redraw_with_VSCROLL( hdlg , wParam , lParam ) ) return TRUE;
			//<<-------------------------------------------------------------------------
			return TRUE;

		case WM_PAINT:
			BeginPaint( hdlg, &ps );
			//>>-------------------------------------------------------------------------
			KGUI_STANDARD_Display_for_Redraw( 1 , ps.hdc );
			//<<-------------------------------------------------------------------------
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_LBUTTONDOWN:
			return TRUE;

		case WM_LBUTTONUP :
			//>>-------------------------------------------------------------------------
			KGUI_STANDARD_Select_Control_Point( hdlg );
			//<<-------------------------------------------------------------------------
			return TRUE;
		
		case WM_RBUTTONDOWN:
			//>>-------------------------------------------------------------------------
			KGUI_STANDARD_Select_Image_Point( hdlg , IDD_POINT_CONTROL , IDD_IMAGE_CONTROL );
			//<<-------------------------------------------------------------------------
			return TRUE;

		case WM_COMMAND:
			switch ( LOWORD( wParam ) ) {
				case IDCANCEL:
					//EndDialog( hdlg , 0 );
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
BOOL APIENTRY Console_Gui_For_PlotMain_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT	ps;
	POINT		ptcursor;
	HBITMAP hBitmap;
	//-------------------------------
	switch ( msg ) {
        case WM_USER:
            switch ( wParam ) {
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
			if ( GUI_DRAW_USE ) { // 2004.09.13
				PLOT_MAIN_WINDOW = hdlg;
				MoveWindow( hdlg , 0 , 0 , 0 , 0 , TRUE );
				KGUI_Plot_Button_Make( hdlg );
				GoModlessDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hdlg , Console_Gui_For_GuiMain_Proc , &PLOT_SCREEN_CONTROL , &PLOT_PROC_CONTROL );
			}
			else { // 2004.09.13
				PLOT_MAIN_WINDOW = hdlg;
				KGUI_Plot_Button_Make( hdlg );
				KGUI_STANDARD_Set_Function_Size( hdlg );
			}
			//----------------------------------
			PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
			//----------------------------------
			return TRUE;

		case WM_DRAWITEM:
			KWIN_DRAW_BUTTON_ITEM( GetModuleHandle( NULL ) , (LPDRAWITEMSTRUCT) lParam );
			return TRUE;

		case WM_PAINT:
			//=============================================================================================
			// 2004.05.21
			//=============================================================================================
//			if ( !KGUI_STANDARD_Window_Signal_Mode() ) return TRUE; // 2004.09.24
			//=============================================================================================
			BeginPaint( hdlg, &ps );
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
			ConGui_Display_Title( ps.hdc );
			ConGui_Display( ps.hdc );
			ConGui_Display_Data( ps.hdc );
			ConGui_Datalog_Display( ps.hdc , -1 );
			ConGui_Display_Plot_Item( ps.hdc , 0 );
			Console_Gui_For_Drawing( ps.hdc , -1 );
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_LBUTTONDOWN:
			GetCursorPos( &ptcursor );
			ScreenToClient( hdlg , &ptcursor );
			if ( !Console_Gui_Left_Button( hdlg , ptcursor ) ) {
				Console_Drawing_Left_Button( hdlg , ptcursor );
			}
			else {
				Console_Drawing_Zoom_Reset();
			}
			return TRUE;

		case WM_RBUTTONDOWN:
			Console_Drawing_Zoom_Reset();
			GetCursorPos( &ptcursor );
			ScreenToClient( hdlg , &ptcursor );
			if ( !Console_Gui_Right_Button( hdlg , ptcursor , FALSE ) ) {
				Console_Drawing_Right_Button( hdlg , ptcursor );
			}
			return TRUE;

		case WM_LBUTTONDBLCLK :
			Console_Drawing_Zoom_Reset();
			GetCursorPos( &ptcursor );
			ScreenToClient( hdlg , &ptcursor );
			if ( !Console_Gui_Right_Button( hdlg , ptcursor , TRUE ) ) {
			}
			return TRUE;

		case WM_HSCROLL:
			Console_Gui_HSCROLL( hdlg , wParam , lParam );
			return TRUE;

		case WM_COMMAND:
			switch ( LOWORD( wParam ) ) {
				case IDB_REWIND2	:
					Console_Gui_Move( hdlg , IDB_REWIND2 );
					return TRUE;

				case IDB_REWIND		:
					Console_Gui_Move( hdlg , IDB_REWIND );
					return TRUE;

				case IDB_FORWARD	:
					Console_Gui_Move( hdlg , IDB_FORWARD );	
					return TRUE;

				case IDB_FORWARD2	:
					Console_Gui_Move( hdlg , IDB_FORWARD2 );
					return TRUE;

				case ID_PLOT_AUTO_1 :
					KWIN_Item_Hide( hdlg , ID_PLOT_AUTO_1 );
					if ( Plotting_Zoom_Recover( SCREEN_1 ) ) { // 2002.04.11
						InvalidateRect( hdlg , NULL , TRUE );
					}
					return TRUE;

				case ID_PLOT_AUTO_2 :
					KWIN_Item_Hide( hdlg , ID_PLOT_AUTO_2 );
					if ( Plotting_Zoom_Recover( SCREEN_2 ) ) { // 2002.04.11
						InvalidateRect( hdlg , NULL , TRUE );
					}
					return TRUE;
					//
				case IDCANCEL:
					//EndDialog( hdlg , 0 );
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
BOOL MAIN_RUNTIME = FALSE;
int  MAIN_READCOUNT = 0;
//------------------------------------------------------------------------
BOOL Directory_Delete_at_Sub_for_Plotter( char *DirectoryName ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet;
	char Buffer[512];

	if ( !RemoveDirectory( DirectoryName ) ) {
		bRet = TRUE;
		sprintf( Buffer , "%s\\*.*" , DirectoryName );
		hFind = FindFirstFile( Buffer , &fd );
		while ( ( hFind != INVALID_HANDLE_VALUE) && bRet ) {
			if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
				sprintf( Buffer , "%s\\%s" , DirectoryName , fd.cFileName );
				if ( !DeleteFile( Buffer ) ) return FALSE;
			}
			else {
				if ( ( strcmp( fd.cFileName , "."  ) != 0 ) && ( strcmp( fd.cFileName , ".." ) != 0 ) ) {
					sprintf( Buffer , "%s\\%s" , DirectoryName , fd.cFileName );
					if ( !Directory_Delete_at_Sub_for_Plotter( Buffer ) ) return FALSE;
				}
			}
			bRet = FindNextFile( hFind , &fd );
		}
		FindClose( hFind );
		if ( !RemoveDirectory( DirectoryName ) ) return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
int File_Delete_for_Time_Reached_for_Plotter( BOOL dirmode , char *DirectoryName , char *FileType , int month , int maxdeletecount ) {
	struct tm Pres_Time;
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	SYSTEMTIME sysTime;
	FILETIME fTime;
	int ddate , tdate , cnt = 0;
	char FileName[256];

	if ( month >= 0 ) { // 2003.05.10
		_getsystime( &Pres_Time );
		tdate = ( Pres_Time.tm_year+1900 ) * 12 + (Pres_Time.tm_mon+1);
	}
	//
	sprintf( FileName , "%s/%s" , DirectoryName , FileType );
	hFind = FindFirstFile( FileName , &fd );
	while ( ( hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( dirmode ) {
			if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) { // folder
				if ( ( fd.cFileName[0] >= '0' ) || ( fd.cFileName[0] <= '9' ) ) {
					if ( ( fd.cFileName[1] >= '0' ) || ( fd.cFileName[1] <= '9' ) ) {
						if ( ( fd.cFileName[2] >= '0' ) || ( fd.cFileName[2] <= '9' ) ) {
							if ( ( fd.cFileName[3] >= '0' ) || ( fd.cFileName[3] <= '9' ) ) {
								if ( ( fd.cFileName[4] >= '0' ) || ( fd.cFileName[4] <= '9' ) ) {
									if ( ( fd.cFileName[5] >= '0' ) || ( fd.cFileName[5] <= '9' ) ) {
										if ( ( fd.cFileName[6] >= '0' ) || ( fd.cFileName[6] <= '9' ) ) {
											if ( ( fd.cFileName[7] >= '0' ) || ( fd.cFileName[7] <= '9' ) ) {
												sysTime.wYear  = ( ( fd.cFileName[0] - '0' ) * 1000 ) + ( ( fd.cFileName[1] - '0' ) * 100 ) + ( ( fd.cFileName[2] - '0' ) * 10 ) + ( fd.cFileName[3] - '0' );
												sysTime.wMonth = ( ( fd.cFileName[4] - '0' ) * 10 ) + ( fd.cFileName[5] - '0' );
												sysTime.wDay   = ( ( fd.cFileName[6] - '0' ) * 10 ) + ( fd.cFileName[7] - '0' );
												//
												if ( month >= 0 ) {
													ddate = tdate - ( ( sysTime.wYear * 12 ) + sysTime.wMonth );
													if ( Pres_Time.tm_mday < sysTime.wDay ) ddate--;
													if ( ddate >= month ) {
														sprintf( FileName , "%s/%s" , DirectoryName , fd.cFileName );
														Directory_Delete_at_Sub_for_Plotter( FileName );
														cnt++;
														if ( cnt >= maxdeletecount ) { // 2004.01.14
															FindClose( hFind );
															return cnt;
														}
													}
												}
												else {
													sprintf( FileName , "%s/%s" , DirectoryName , fd.cFileName );
													Directory_Delete_at_Sub_for_Plotter( FileName );
													cnt++;
													if ( cnt >= maxdeletecount ) { // 2004.01.14
														FindClose( hFind );
														return cnt;
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
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			FileTimeToLocalFileTime( &(fd.ftLastWriteTime) , &fTime );
			FileTimeToSystemTime( &fTime , &sysTime );
			if ( month >= 0 ) {
				ddate = tdate - ( ( sysTime.wYear * 12 ) + sysTime.wMonth );
				if ( Pres_Time.tm_mday < sysTime.wDay ) ddate--;
				if ( ddate >= month ) {
					sprintf( FileName , "%s/%s" , DirectoryName , fd.cFileName );
					DeleteFile( FileName );
					cnt++;
					if ( cnt >= maxdeletecount ) { // 2004.01.14
						FindClose( hFind );
						return cnt;
					}
				}
			}
			else {
				sprintf( FileName , "%s/%s" , DirectoryName , fd.cFileName );
				DeleteFile( FileName );
				cnt++;
				if ( cnt >= maxdeletecount ) { // 2004.01.14
					FindClose( hFind );
					return cnt;
				}
			}
		}
		bRet = FindNextFile( hFind , &fd );
	}
	FindClose( hFind );
	return cnt;
}
//------------------------------------------------------------------------
void Plotter_Pre_Polling() {
	long Mon_Goal;
	long Mon_Goal2;
	int i , r = 0 , d;
	int CommStatus;
	HDC hDC;
	int t = 0;

	//===========================================================================================
	// 2005.03.17
	//===========================================================================================
	if ( REMOTE_MONITOR_MODE ) {
		_endthread();
		return;
	}
	//===========================================================================================

	if ( DISPLAY_CONTROL >= 0 ) MAIN_RUNTIME = FALSE;
	else                        MAIN_RUNTIME = TRUE;

	while ( TRUE ) 
	{
		Mon_Goal = Clock_Goal_Get_Start_Time();
		if ( DISPLAY_CONTROL >= 0 ) {
			d = _dREAD_DIGITAL( DISPLAY_CONTROL , &CommStatus );
			if ( !MAIN_RUNTIME ) {
				if ( ( d >= 1 ) && ( d <= 6 ) ) { // 2002.04.02
					MAIN_READCOUNT = 0; // 2003.01.29
					MAIN_RUNTIME = TRUE;
					r = 0;
					if ( ( d == 1 ) || ( d == 3 ) || ( d == 5 ) ) { // 2002.04.02
						//-----------------------------------------------------------------------
						Mon_Goal2 = Clock_Goal_Get_Start_Time(); // 2003.01.29
						//-----------------------------------------------------------------------
						//============================================================
						//	Reset Item
						//============================================================
						time( &DISPLAY_START_TIME );
						for ( i = 0 ; i < ANALOG_CNT ; i++ )  {
							A_IO_VALUE( i )		= 0; // 2003.01.29
							A_IO_PLT_START( i )	= 0;
							A_IO_PLT_END( i )	= 0;
							A_IO_PLT_COUNT( i ) = 0;
						}
						if ( V_AUTO_SCALE_DRAW() ) Plotting_Data_Set_For_Auto_Scale( 0 );
						//-----------------------------------------------------------------------
						do { // 2003.01.29
							if ( MAIN_READCOUNT	> 2 ) break; // 2003.01.29
							_sleep(2); // 2003.01.29
						} while ( !Clock_Goal_Data_Check( &Mon_Goal2 , 2000 ) ); // 2003.01.29
						//-----------------------------------------------------------------------
					}
					if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.09.16
						if ( PLOT_MAIN_WINDOW != NULL ) InvalidateRect( PLOT_MAIN_WINDOW , NULL , TRUE );
					}
				}
			}
			else {
				if ( ( d < 1 ) || ( d > 6 ) ) { // 2002.04.02
					MAIN_RUNTIME = FALSE;
					if ( PLOT_MAIN_WINDOW != NULL ) {
						if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.09.16
							hDC = GetDC( PLOT_MAIN_WINDOW );
							ConGui_Display_HeadTime( hDC , -1 );
							ReleaseDC( PLOT_MAIN_WINDOW , hDC );
						}
					}
				}
				else {
					if ( PLOT_MAIN_WINDOW != NULL ) {
						if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.09.16
							hDC = GetDC( PLOT_MAIN_WINDOW );
							ConGui_Display_HeadTime( hDC , r );
							ReleaseDC( PLOT_MAIN_WINDOW , hDC );
						}
					}
					r++;
					if ( r > 4 ) r = 0;
				}
			}
		}
		Plotting_Data_Polling( PLOT_MAIN_WINDOW , KGUI_STANDARD_Window_Signal_Mode() , MAIN_RUNTIME );
		do 
		{
			_sleep(2);
		} 
		while ( !Clock_Goal_Data_Check( &Mon_Goal , 498 ) );

		if ( t >= 7200 ) { // d / 2 = sec
			t = 0;
			if ( !SYSTEM_LOGSKIP_STATUS() ) { // 2005.03.17
				if ( DATALOG_CONTROL != -1 ) {
	//				i = File_Delete_for_Time_Reached( DATALOG_DIRECTORY_NAME() , "*.log" , DATALOG_AUTO_DELETE_TIME ); // 2004.01.09
					i = File_Delete_for_Time_Reached_for_Plotter( DATALOG_DIRECTORY_SEPARATE() , DATALOG_DIRECTORY_NAME() , "*.log" , DATALOG_AUTO_DELETE_TIME , DATALOG_AUTO_DELETE_COUNT ); // 2004.01.09
					if ( i != 0 ) {
						_IO_CIM_PRINTF( "Plotter auto delete system delete %d file(s)\n" , i );
					}
				}
			}
		}
		t++;
	}
	_endthread();
}

extern CRITICAL_SECTION CR_READ_DATA; // 2016.09.06

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Plotter_Data_Polling() {
	clock_t  Mon_Goal;
	char *Set_IO_Status;
	char *Return_IO_Status;
	BOOL OLD_MAIN_RUNTIME = FALSE;

	char Buffer1[128] = {0,} , Buffer2[128] = {0,} , Type;
	int i , j , k , Point , IOP , sj;
	memset(Buffer1			,0x00, sizeof(Buffer1));
	memset(Buffer2 			,0x00, sizeof(Buffer2));

	//===========================================================================================
	// 2005.03.17
	//===========================================================================================
	if ( REMOTE_MONITOR_MODE ) {
		_endthread();
		return;
	}
	//===========================================================================================
	
	Set_IO_Status = calloc( 5120 + 1 , sizeof( char ) );
	if ( Set_IO_Status == NULL ) {
		_IO_CIM_PRINTF( "PLOTTER Monitoring System Failed\n" );
		_endthread(); // 2004.09.24
		return;
	}
	Return_IO_Status = calloc( 5120 + 1 , sizeof( char ) );
	if ( Return_IO_Status == NULL ) {
		free( Set_IO_Status );
		_IO_CIM_PRINTF( "PLOTTER Monitoring System Failed\n" );
		_endthread(); // 2004.09.24
		return;
	}
	
	strcpy( Set_IO_Status , "" );
	strcpy( Return_IO_Status , "" );
	A_IO_MONITOR_REFORM = TRUE;
	while ( TRUE ) {
		Mon_Goal = Clock_Goal_Get_Start_Time();
		if ( A_IO_MONITOR_REFORM ) {
			_dGROUP_IO_MONITORING_OFF();
			strcpy( Set_IO_Status , "T250" );
			if ( HEADER_MESSAGE_IO >= 0 ) {
				sprintf( Buffer1 , "S%d" , HEADER_MESSAGE_IO );
				strcat( Set_IO_Status , Buffer1 );
			}
			for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
				if ( A_IO_SAVE( i ) ) {
					if ( A_IO_TYPE( i ) )
						sprintf( Buffer1 , "A%d" , A_IO_POINT( i ) );
					else
						sprintf( Buffer1 , "D%d" , A_IO_POINT( i ) );
					strcat( Set_IO_Status , Buffer1 );
				}
			}
			A_IO_MONITOR_REFORM = FALSE;
			OLD_MAIN_RUNTIME    = FALSE;
		}
		if ( OLD_MAIN_RUNTIME != MAIN_RUNTIME ) {
			OLD_MAIN_RUNTIME = MAIN_RUNTIME;
			switch( OLD_MAIN_RUNTIME ) {
			case TRUE :
				MAIN_READCOUNT = 0; // 2003.01.29
				_dGROUP_IO_MONITORING( Set_IO_Status );
				break;
			case FALSE :
				MAIN_READCOUNT = 0; // 2003.01.29
				_dGROUP_IO_MONITORING_OFF();
				break;
			}
		}
		//----------------------------------------------------
		if ( OLD_MAIN_RUNTIME ) {
			MAIN_READCOUNT++; // 2003.01.29
			if ( _dGROUP_IO_READ_from_MEM( Set_IO_Status , Return_IO_Status , 5120 ) ) {
				i = 0;
				sj = 0; // 2004.09.13
				while ( _dGROUP_IO_TRANSLATE_from_BUFFER( Return_IO_Status , i , &Type , &Point , Buffer1 , Buffer2 , &k ) ) {
					IOP = -1;
					switch( Type ) {
					case 'S' :
						if ( Point == HEADER_MESSAGE_IO ) {
							IOP = HEADER_MESSAGE_IO;
						}
						else {
							IOP = -1;
						}
						break;
					default :
//-------------------------------------------------------------------------------
//						for ( j = 0 ; j < ANALOG_CNT ; j++ ) { // 2004.09.13
//							if ( A_IO_POINT( j ) == Point ) break; // 2004.09.13
//						} // 2004.09.13
//						if ( j == ANALOG_CNT ) IOP = -1; // 2004.09.13
//						else                   IOP = j; // 2004.09.13
//-------------------------------------------------------------------------------
// 2004.09.13
//-------------------------------------------------------------------------------
						for ( j = sj ; j < ANALOG_CNT ; j++ ) {
							if ( A_IO_POINT( j ) == Point ) {
								IOP = j;
								sj++;
								break;
							}
						}
						if ( IOP == -1 ) {
							for ( j = 0 ; j < sj ; j++ ) {
								if ( A_IO_POINT( j ) == Point ) {
									IOP = j;
									sj++;
									break;
								}
							}
						}
//-------------------------------------------------------------------------------
						break;
					}
					if ( IOP != -1 ) {
						if ( Type == 'S' ) {
							strcpy( HEADER_MESSAGE_STRING , Buffer1 );
						}
						else {
							//
							EnterCriticalSection( &CR_READ_DATA ); // 2016.09.06
							//
							A_IO_VALUE( IOP ) = atof( Buffer1 );
							strcpy( A_IO_VALUE_STR( IOP ), "");
							strcpy( A_IO_VALUE_STR( IOP ) , Buffer2 );
							memset(Buffer1			,0x00, sizeof(Buffer1));
							memset(Buffer2 			,0x00, sizeof(Buffer2));
							//
							LeaveCriticalSection( &CR_READ_DATA ); // 2016.09.06
							//
						}
					}
					i = k;
				}
			}
		}
		do {
			_sleep(2);
		} while ( !Clock_Goal_Data_Check( &Mon_Goal , 250 ) );
	}
	free( Set_IO_Status );
	free( Return_IO_Status );
	_endthread();
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {
	HWND hWnd;
	int i;
	char Buffer[16];
	char Else1Str[256];
	char Else2Str[256];
	char Else3Str[256];
	char Else4Str[256];
	char Else5Str[256];
	
	
	memset(Buffer			,0x00, sizeof(Buffer));
	memset(Else1Str			,0x00, sizeof(Else1Str));
	memset(Else2Str			,0x00, sizeof(Else2Str));
	memset(Else3Str			,0x00, sizeof(Else3Str));
	memset(Else4Str			,0x00, sizeof(Else4Str));
	memset(Else5Str			,0x00, sizeof(Else5Str));

	//----------------------------------
//	REGIST_FILE_INFO( "Gui Plotter 2.1(2001.08.16)" );
//	REGIST_FILE_INFO( "Gui Plotter 2.2(2001.11.20)" ); // File Read upto 3 times
//	REGIST_FILE_INFO( "Gui Plotter 3.0(2002.01.01)" ); // CimSqCon Change Rebuild
//	REGIST_FILE_INFO( "Gui Plotter 3.1(2002.03.05)" ); // Log File Write / Read Part Modified
//	REGIST_FILE_INFO( "Gui Plotter 3.11(2002.04.02)" ); // Display Control Expand(case(0~2)-> case(0~12))
//	REGIST_FILE_INFO( "Gui Plotter 3.2(2002.04.10)" ); // Bug Fixed(Redraw when File Read and scroll)
//	REGIST_FILE_INFO( "Gui Plotter 3.3(2002.04.11)" ); // Zoom Support
//	REGIST_FILE_INFO( "Gui Plotter 3.4(2002.06.17)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Plotter 3.5(2002.08.08)" ); // Datalog IO Expand
//	REGIST_FILE_INFO( "Gui Plotter 3.6(2002.09.17)" ); // Pallette Size Bug Update
//	REGIST_FILE_INFO( "Gui Plotter 3.7(2002.10.18)" ); // Logged File Read from New File
//	REGIST_FILE_INFO( "Gui Plotter 3.8(2003.01.29)" ); // Read Data Reset when Reset Plotting
//	REGIST_FILE_INFO( "Gui Plotter 3.9(2003.02.11)" ); // Append Auto Return Monitor mode when window close
//	REGIST_FILE_INFO( "Gui Plotter 3.95(2003.07.12)" ); // Append Wait Receive Spawn Message
//	REGIST_FILE_INFO( "Gui Plotter 4.0(2004.01.09)" ); // Append Log Folder Separate Option
//	REGIST_FILE_INFO( "Gui Plotter 4.01(2004.01.14)" ); // Auto Delete Part Update
//	REGIST_FILE_INFO( "Gui Plotter 4.02(2004.01.19)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Plotter 4.03(2004.08.10)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Plotter 4.04(2004.08.18)" ); // Event Start Data Log Possible
//	REGIST_FILE_INFO( "Gui Plotter 4.05(2004.09.13)" ); // Group IO Read part Update
//	REGIST_FILE_INFO( "Gui Plotter 4.06(2004.09.24)" ); // CPU Overhead Bug Update
//	REGIST_FILE_INFO( "Gui Plotter 4.1(2005.03.17)" ); // Append Remote Mode(Read Only)
//	REGIST_FILE_INFO( "Gui Plotter 4.11(2005.04.29)" ); // Append Remote Mode(Read Only)
//	REGIST_FILE_INFO( "Gui Plotter 4.12(2005.05.24)" ); // Append Auto Scal Default Dev Value Set Part
//	REGIST_FILE_INFO( "Gui Plotter 4.13(2005.06.15)" ); // Append Go to Last Status when Datalog start
	REGIST_FILE_INFO( "Gui Plotter 4.14(2005.07.19)" ); // Append Datalog String IO Data Logging Part
	//----------------------------------
	//----------------------------------
	//----------------------------------
	InitCommonControls();
	//----------------------------------
	STR_SEPERATE_CHAR( PROGRAM_PARAMETER_READ() , '|' , Else1Str , Else2Str , 255 );
	//>>-------------------------------------------------------------------------
	GUI_DRAW_POLL_TIME = atoi( Else2Str );
	KGUI_Set_BACKGROUND( Else1Str );
	//----------------------------------
	STR_SEPERATE_CHAR( PROGRAM_ARGUMENT_READ() , '|' , Else1Str , Else4Str , 255 );
//	STR_SEPERATE_CHAR( Else4Str , '|' , Else2Str , Else3Str , 255 ); // 2005.04.19
	STR_SEPERATE_CHAR( Else4Str , '|' , Else2Str , Else5Str , 255 ); // 2005.04.19
	STR_SEPERATE_CHAR( Else5Str , '|' , Else3Str , Else4Str , 255 ); // 2005.04.19 (Gabage)
	//>>-------------------------------------------------------------------------
	if ( strlen( Else2Str ) <= 0 ) GUI_DRAW_USE = FALSE;
	else                           GUI_DRAW_USE = TRUE;
	//========================================================================================
	// 2005.03.17
	//========================================================================================
	if ( strlen( Else3Str ) > 0 ) {
		REMOTE_MONITOR_MODE = TRUE;
		STR_SEPERATE_CHAR( Else3Str , ':' , Else4Str , Else5Str , 255 );
		sprintf( REMOTE_MONITOR_FOLDER , "$Remote$\\[%s][%s]" , Else4Str , Else5Str );
		_nINF_KNFS_Prepare_for_ConnectionName( PROGRAM_FUNCTION_READ() );
		if ( _nINF_KNFS_CONNECTION_FOR_CLIENT( Else4Str , atoi(Else5Str) ) != 0 ) {
			_IO_CIM_PRINTF( "Network KNFS Initialization Error\n" );
			return SYS_ABORTED;
		}
	}
	else {
		REMOTE_MONITOR_MODE = FALSE;
	}
	//========================================================================================
	if ( !Read_Information_File( Else1Str , GUI_DRAW_USE ) ) {
		SYSTEM_KILL_MYSELF();
		return SYS_ABORTED;
	}
	if ( GUI_DRAW_USE ) { // 2004.09.13
		if ( !KGUI_STANDARD_Display_Information_Initial( Else2Str ) ) {
			SYSTEM_KILL_MYSELF();
			return SYS_ABORTED;
		}
	}
	else {
		if ( !KWIN_DIALOG_DRIVER_LOADING() ) {
			//_IO_CIM_PRINTF( "{%s} - Dialog Driver Loading Error\n" , PROGRAM_FUNCTION_READ() );
		}
		//================================================================================
		KGUI_PALETTE_LOADING( NULL ); // 2004.11.20
		//================================================================================
	}
	if ( ANALOG_CNT <= 0 ) {
		_IO_CIM_PRINTF( "Can not Find Regist Item\n" );
		SYSTEM_KILL_MYSELF();
		return SYS_ABORTED;
	}
	//<<-------------------------------------------------------------------------

	strcpy( COLOR_SELECT , "Black|DarkRed|DarkGreen|PeaGreen|DarkBlue|Lavender|Slate|LightGray|PaleGreen|PaleBlue|OffWhite|MediumGray|DarkGray|RightRed|BrightGreen|Yellow|BrightBlue|Magenta|Cyan|White" );
	for ( i = 0 ; i < KGUI_PALETTE_Get_SIZE() ; i++ ) {
		sprintf( Buffer , "|USER %d" , i );
		strcat( COLOR_SELECT , Buffer );
	}
	
	if ( V_AUTO_SCALE_DRAW() ) {
		Plotting_Data_Set_For_Auto_Scale( 0 );
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) A_IO_PLT_POLL( i ) = AUTO_SCALE_POLL_TIME;
	}

	_beginthread( (void *) Plotter_Pre_Polling , 0 , 0 );
	_beginthread( (void *) Plotter_Data_Polling , 0 , 0 );


	//----------------------------------
	while( PROGRAM_GUI_SPAWNING == 0 ) { _sleep(100); } // 2003.07.12
	//----------------------------------
	hWnd = GET_MAIN_WINDOW();
	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Console_Gui_For_PlotMain_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Console_Gui_For_PlotMain_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Console_Gui_For_PlotMain_Proc , (LPARAM) NULL ); // 2004.01.19
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , GET_MAIN_WINDOW() , Console_Gui_For_PlotMain_Proc , (LPARAM) NULL ); // 2002.06.l17

	//<<-------------------------------------------------------------------------
	SYSTEM_KILL_MYSELF();
	//----------------------------------
	PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
	//----------------------------------
	return( SYS_SUCCESS );
}

