#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>
#include <Kutlanal.h>
//------------------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "ctl_code.h"
#include "plt_code.h"
#include "plt_parm.h"
#include "dat_code.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int  HEADER_MESSAGE_IO;
extern int  HEADER_MESSAGE_DISPLAY_START_X;
extern int  HEADER_MESSAGE_DISPLAY_START_Y;
extern char HEADER_MESSAGE_STRING[256];
extern int  HEADER_MESSAGE_SIZE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int LINE_DRAW_TYPE[2] = { LIN_TYPE , LIN_TYPE }; //LIN_TYPE , REC_TYPE
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int V_LINE_DRAW_TYPE( int scr ) {
	return( LINE_DRAW_TYPE[scr] );
}
//------------------------------------------------------------------------------------------
void S_LINE_DRAW_TYPE( int scr , int mode ) {
	LINE_DRAW_TYPE[scr] = mode;
}
//------------------------------------------------------------------------------------------
int Get_Graph_Plotting_Value( double A , double B , double data ) {
	if ( A == 0 ) return 120;
	return ( (int) ( 240 + ( (-240) * ( data - B ) / A ) ) );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Get_Graph_Plotting_Dev( int p ) {
	return( A_IO_PLT_DEV( p ) );
}
//------------------------------------------------------------------------------------------
int Get_Graph_Plotting_Count_Just_Single( int dev ) {
	if ( dev <= 0 ) 
		return( 30 * ( 2 - dev ) );
	else
		return( 30 / dev );
}
//------------------------------------------------------------------------------------------
int Get_Graph_Plotting_Count( int p , int dm ) {
	if ( dm == SCR_BIG_DRAW ) {
		if ( A_IO_PLT_DEV( p ) <= 0 ) 
			return( ( DRAW_SIZE_X_BIG * ( 2 - A_IO_PLT_DEV(p) ) ) + 1 );
		else
			return( DRAW_SIZE_X_BIG / A_IO_PLT_DEV(p) + 1 );
	}
	else {
		if ( A_IO_PLT_DEV(p) <= 0 ) 
			return( ( DRAW_SIZE_X_SMALL * ( 2 - A_IO_PLT_DEV(p) ) ) + 1 );
		else
			return( DRAW_SIZE_X_SMALL / A_IO_PLT_DEV(p) + 1 );
	}
}
//------------------------------------------------------------------------------------------
int Get_Graph_Plotting_Draw_Count( int p , int dm , int l ) {
	int x;
	if ( dm == SCR_BIG_DRAW ) {
		if ( A_IO_PLT_DEV(p) <= 0 ) {
			return( DRAW_SIZE_X_BIG + 1 - l / (2-A_IO_PLT_DEV(p)) );
		}
		else {
			x = DRAW_SIZE_X_BIG / A_IO_PLT_DEV(p) + 1;
			return( x - l );
		}
	}
	else {
		if ( A_IO_PLT_DEV(p) <= 0 ) {
			return( DRAW_SIZE_X_SMALL + 1 - l / (2-A_IO_PLT_DEV(p)) );
		}
		else {
			x = DRAW_SIZE_X_SMALL / A_IO_PLT_DEV(p) + 1;
			return( x - l );
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int		Drawing_Item_S1_Draw = 0;
int		Drawing_Item_S1 = 0;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Drawing_ItemDraw_S1( HDC hDC , int DrawMode ) {
	int item , xc , pt , Cs , Ce , T1 , T2;
	int DataBuf = -1;
	BOOL Atset;

	if ( V_AUTO_SCALE_DRAW() && V_AUTO_SCALE_TIME() ) Atset = TRUE; else Atset = FALSE;

	for ( item = 0 ; item < 4 ; item++ ) if ( V_PLOTTER_TABLE( SCREEN_1 , item ) != -1 ) DataBuf = item;
	if ( DataBuf == -1 ) {
		Draw_Time_Data( hDC , DrawMode , SCREEN_1 , -1 , -1 , -1 , COLOR_LightGray , Atset );
		return;
	}
	if ( Drawing_Item_S1 == 0 ) Drawing_Item_S1 = 1;
	else                        Drawing_Item_S1 = 0;
	if ( Drawing_Item_S1 == 0 ) return;

	if ( V_AUTO_SCALE_DRAW() ) Drawing_Item_S1_Draw = 0;

	do {
		item = Drawing_Item_S1_Draw;
		pt = V_PLOTTER_TABLE( SCREEN_1 , Drawing_Item_S1_Draw );
		Drawing_Item_S1_Draw++;	if ( Drawing_Item_S1_Draw == 4 ) Drawing_Item_S1_Draw = 0;
	}
	while ( pt == -1 );
	
	if ( A_IO_PLT_COUNT( pt ) >= 0 ) {
		if ( A_IO_PLT_COUNT( pt ) == 1 ) {
			if ( FindPosFromDataBuffer( pt , 1 , &T1 ) ) {
				Draw_Time_Data( hDC , DrawMode , SCREEN_1 , pt , T1 , T1 , V_PLOTTER_COLOR( SCREEN_1 , item ) , Atset );
			}
		}
		else {
			xc = Get_Graph_Plotting_Count( pt , DrawMode );
			if ( DISP_LAST_POINTER[SCREEN_1] < 1 ) {
				Ce = A_IO_PLT_COUNT( pt );
				Cs = Ce - xc + 1;
				if ( Cs < 1 ) Cs = 1;
			}
			else {
				if ( DISP_LAST_POINTER[SCREEN_1] > A_IO_PLT_COUNT( pt ) ) {
					Ce = A_IO_PLT_COUNT( pt );
					Cs = Ce - xc + 1;
					if ( Cs < 1 ) Cs = 1;
				}
				else {
					if ( DISP_LAST_POINTER[SCREEN_1] < xc ) {
						if ( xc > A_IO_PLT_COUNT( pt ) ) {
							Ce = A_IO_PLT_COUNT( pt );
							Cs = 1;
						}
						else {
							Ce = xc;
							Cs = 1;
						}
					}
					else {
						Ce = DISP_LAST_POINTER[SCREEN_1];
						Cs = Ce - xc + 1;
						if ( Cs < 1 ) Cs = 1;
					}
				}
			}
			if ( FindPosFromDataBuffer( pt , Cs , &T1 ) ) {
				if ( FindPosFromDataBuffer( pt , Ce , &T2 ) ) {
					Draw_Time_Data( hDC , DrawMode , SCREEN_1 , pt , T1 , T2 , V_PLOTTER_COLOR( SCREEN_1 , item ) , Atset );
				}
			}
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Drawing_Graph_S1( HDC hDC , int DrawMode ) {
	int i , j , xc , pt , co , yp , xd , xs , ys , ye , Cs , Ce , xq , xz;
	double A , B;
	int DataBuf = -1;

	for ( i = 0 ; i < 4 ; i++ ) if ( V_PLOTTER_TABLE(SCREEN_1,i) != -1 ) DataBuf = i;
	if ( DataBuf == -1 ) return;

	for ( i = 0 ; i < 4 ; i++ ) {
		pt = V_PLOTTER_TABLE(SCREEN_1,i);
		co = V_PLOTTER_COLOR(SCREEN_1,i);
		if ( pt != -1 ) {
			if ( A_IO_PLT_COUNT( pt ) >= 0 ) {
				A = A_IO_MAX( pt ) - A_IO_MIN( pt );
				B = A_IO_MIN( pt );
				if ( A_IO_PLT_COUNT( pt ) == 1 ) {
					if ( FindPosFromDataBuffer( pt , 1 , &yp ) ) {
						ys = Get_Graph_Plotting_Value( A , B , A_IO_PLT_DATA( pt, yp ) );
					}
					else {
						ys = 0;
					}
					if ( A_IO_TOGGLE( pt ) < -1 ) {
						if ( V_START_LEFT_DRAW() )
							KGUI_DRAW_Line( hDC , 0 , ys , 0 , ys , co , V_PLOTTER_POINT(SCREEN_1) );
						else
							KGUI_DRAW_Line( hDC , DRAW_SIZE_X_SMALL , ys , DRAW_SIZE_X_SMALL , ys , co , V_PLOTTER_POINT(SCREEN_1) );
					}
				}
				else {
					xc = Get_Graph_Plotting_Count( pt , DrawMode );
					if ( DISP_LAST_POINTER[SCREEN_1] < 1 ) {
						Ce = A_IO_PLT_COUNT( pt );
						Cs = Ce - xc + 1;
						if ( Cs < 1 ) Cs = 1;
						if ( V_START_LEFT_DRAW() ) {
							xs = 0;
						}
						else {
							xs = Get_Graph_Plotting_Draw_Count( pt , DrawMode , Ce );
							if ( xs < 0 ) xs = 0;
						}
					}
					else {
						if ( DISP_LAST_POINTER[SCREEN_1] > A_IO_PLT_COUNT( pt ) ) {
							Ce = A_IO_PLT_COUNT( pt );
							Cs = Ce - xc + 1;
							if ( Cs < 1 ) Cs = 1;
							if ( V_START_LEFT_DRAW() ) {
								xs = 0;
							}
							else {
								xs = Get_Graph_Plotting_Draw_Count( pt , DrawMode , Ce );
								if ( xs < 0 ) xs = 0;
							}
						}
						else {
							if ( DISP_LAST_POINTER[SCREEN_1] < xc ) {
								if ( xc > A_IO_PLT_COUNT( pt ) ) {
									Ce = A_IO_PLT_COUNT( pt );
									Cs = 1;
								}
								else {
									Ce = xc;
									Cs = 1;
								}
							}
							else {
								Ce = DISP_LAST_POINTER[SCREEN_1];
								Cs = Ce - xc + 1;
								if ( Cs < 1 ) Cs = 1;
							}
							if ( V_START_LEFT_DRAW() ) {
								xs = 0;
							}
							else {
								xs = Get_Graph_Plotting_Draw_Count( pt , DrawMode , Ce );
								if ( xs < 0 ) xs = 0;
							}
						}
					}
					xd = Get_Graph_Plotting_Dev( pt );
					if ( xd <= 0 ) xq = 1; 
					else           xq = xd;
					if ( FindPosFromDataBuffer( pt , Cs , &yp ) ) {
						ys = Get_Graph_Plotting_Value( A , B , A_IO_PLT_DATA( pt, yp ) );
					}
					else {
						ys = 0;
					}
					for ( j = Cs+1 , xz = 0 ; j <= Ce ; xs++ , j++ ) {
						if ( FindPosFromDataBuffer( pt , j , &yp ) ) ye = Get_Graph_Plotting_Value( A , B , A_IO_PLT_DATA( pt, yp ) );
						else break;
						if ( A_IO_TOGGLE( pt ) < -1 ) {
							if ( V_LINE_DRAW_TYPE( SCREEN_1 ) == LIN_TYPE ) {
								KGUI_DRAW_Line( hDC , xs*xq , ys , (xs+1)*xq , ye , co , V_PLOTTER_POINT(SCREEN_1) );
							}
							else {
								KGUI_DRAW_Line( hDC , xs*xq , ys , xs*xq , ye , co , V_PLOTTER_POINT(SCREEN_1) );
								KGUI_DRAW_Line( hDC , xs*xq , ye , (xs+1)*xq , ye , co , V_PLOTTER_POINT(SCREEN_1) );
							}
						}
						else {
							if ( A_IO_TOGGLE( pt ) == -1 ) {
								if ( ys != ye ) {
									KGUI_DRAW_Line( hDC , (xs+1)*xq , 0 , (xs+1)*xq , 240 , co , V_PLOTTER_POINT(SCREEN_1) );
								}
							}
							else {
								if ( ( ys != ye ) && ( A_IO_PLT_DATA( pt, yp ) == A_IO_TOGGLE( pt ) ) ) {
									KGUI_DRAW_Line( hDC , (xs+1)*xq , 0 , (xs+1)*xq , 240 , co , V_PLOTTER_POINT(SCREEN_1) );
								}
							}
						}
						if ( xd <= 0 ) {
							if ( xz < xd ) {
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
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int		Drawing_Item_S2_Draw = 0;
int		Drawing_Item_S2 = 0;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Drawing_ItemDraw_S2( HDC hDC , int DrawMode ) {
	int item , xc , pt , Cs , Ce , T1 , T2;
	int DataBuf = -1;
	BOOL Atset;

	if ( V_AUTO_SCALE_DRAW() && V_AUTO_SCALE_TIME() ) Atset = TRUE; else Atset = FALSE;

	for ( item = 0 ; item < 4 ; item++ ) if ( V_PLOTTER_TABLE( SCREEN_2 , item ) != -1 ) DataBuf = item;
	if ( DataBuf == -1 ) {
		Draw_Time_Data( hDC , DrawMode , SCREEN_2 , -1 , -1 , -1 , COLOR_LightGray , Atset );
		return;
	}
	if ( Drawing_Item_S2 == 0 ) Drawing_Item_S2 = 1;
	else                        Drawing_Item_S2 = 0;
	if ( Drawing_Item_S2 == 0 ) return;

	if ( V_AUTO_SCALE_DRAW() ) Drawing_Item_S1_Draw = 0;

	do {
		item = Drawing_Item_S2_Draw;
		pt = V_PLOTTER_TABLE( SCREEN_2 , Drawing_Item_S2_Draw );
		Drawing_Item_S2_Draw++;	if ( Drawing_Item_S2_Draw == 4 ) Drawing_Item_S2_Draw = 0;
	}
	while ( pt == -1 );

	if ( A_IO_PLT_COUNT( pt ) >= 0 ) {
		if ( A_IO_PLT_COUNT( pt ) == 1 ) {
			if ( FindPosFromDataBuffer( pt , 1 , &T1 ) ) {
				Draw_Time_Data( hDC , DrawMode , SCREEN_2 , pt , T1 , T1 , V_PLOTTER_COLOR( SCREEN_2 , item ) , Atset );
			}
		}
		else {
			xc = Get_Graph_Plotting_Count( pt , DrawMode );
			if ( DISP_LAST_POINTER[SCREEN_2] < 1 ) {
				Ce = A_IO_PLT_COUNT( pt );
				Cs = Ce - xc + 1;
				if ( Cs < 1 ) Cs = 1;
			}
			else {
				if ( DISP_LAST_POINTER[SCREEN_2] > A_IO_PLT_COUNT( pt ) ) {
					Ce = A_IO_PLT_COUNT( pt );
					Cs = Ce - xc + 1;
					if ( Cs < 1 ) Cs = 1;
				}
				else {
					if ( DISP_LAST_POINTER[SCREEN_2] < xc ) {
						if ( xc > A_IO_PLT_COUNT( pt ) ) {
							Ce = A_IO_PLT_COUNT( pt );
							Cs = 1;
						}
						else {
							Ce = xc;
							Cs = 1;
						}
					}
					else {
						Ce = DISP_LAST_POINTER[SCREEN_2];
						Cs = Ce - xc + 1;
						if ( Cs < 1 ) Cs = 1;
					}
				}
			}
			if ( FindPosFromDataBuffer( pt , Cs , &T1 ) ) {
				if ( FindPosFromDataBuffer( pt , Ce , &T2 ) ) {
					Draw_Time_Data( hDC , DrawMode , SCREEN_2 , pt , T1 , T2 , V_PLOTTER_COLOR( SCREEN_2 , item ) , Atset );
				}
			}
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Drawing_Graph_S2( HDC hDC , int DrawMode ) {
	int i , j , xc , pt , co , yp , xd , xs , ys , ye , Cs , Ce , xq , xz;
	double A , B;
	int DataBuf = -1;

	for ( i = 0 ; i < 4 ; i++ ) if ( V_PLOTTER_TABLE(SCREEN_2,i) != -1 ) DataBuf = i;
	if ( DataBuf == -1 ) return;

	for ( i = 0 ; i < 4 ; i++ ) {
		pt = V_PLOTTER_TABLE(SCREEN_2,i);
		co = V_PLOTTER_COLOR(SCREEN_2,i);
		if ( pt != -1 ) {
			if ( A_IO_PLT_COUNT( pt ) >= 0 ) {
				A = A_IO_MAX( pt ) - A_IO_MIN( pt );
				B = A_IO_MIN( pt );
				if ( A_IO_PLT_COUNT( pt ) == 1 ) {
					if ( FindPosFromDataBuffer( pt , 1 , &yp ) ) {
						ys = Get_Graph_Plotting_Value( A , B , A_IO_PLT_DATA( pt, yp ) );
					}
					else {
						ys = 0;
					}
					if ( A_IO_TOGGLE( pt ) < -1 ) {
						if ( V_START_LEFT_DRAW() )
							KGUI_DRAW_Line( hDC , 0 , ys , 0 , ys , co , V_PLOTTER_POINT(SCREEN_2) );
						else
							KGUI_DRAW_Line( hDC , DRAW_SIZE_X_SMALL , ys , DRAW_SIZE_X_SMALL , ys , co , V_PLOTTER_POINT(SCREEN_2) );
					}
				}
				else {
					xc = Get_Graph_Plotting_Count( pt , DrawMode );
					if ( DISP_LAST_POINTER[SCREEN_2] < 1 ) {
						Ce = A_IO_PLT_COUNT( pt );
						Cs = Ce - xc + 1;
						if ( Cs < 1 ) Cs = 1;
						if ( V_START_LEFT_DRAW() ) {
							xs = 0;
						}
						else {
							xs = Get_Graph_Plotting_Draw_Count( pt , DrawMode , Ce );
							if ( xs < 0 ) xs = 0;
						}
					}
					else {
						if ( DISP_LAST_POINTER[SCREEN_2] > A_IO_PLT_COUNT( pt ) ) {
							Ce = A_IO_PLT_COUNT( pt );
							Cs = Ce - xc + 1;
							if ( Cs < 1 ) Cs = 1;
							if ( V_START_LEFT_DRAW() ) {
								xs = 0;
							}
							else {
								xs = Get_Graph_Plotting_Draw_Count( pt , DrawMode , Ce );
								if ( xs < 0 ) xs = 0;
							}
						}
						else {
							if ( DISP_LAST_POINTER[SCREEN_2] < xc ) {
								if ( xc > A_IO_PLT_COUNT( pt ) ) {
									Ce = A_IO_PLT_COUNT( pt );
									Cs = 1;
								}
								else {
									Ce = xc;
									Cs = 1;
								}
							}
							else {
								Ce = DISP_LAST_POINTER[SCREEN_2];
								Cs = Ce - xc + 1;
								if ( Cs < 1 ) Cs = 1;
							}
							if ( V_START_LEFT_DRAW() ) {
								xs = 0;
							}
							else {
								xs = Get_Graph_Plotting_Draw_Count( pt , DrawMode , Ce );
								if ( xs < 0 ) xs = 0;
							}
						}
					}
					xd = Get_Graph_Plotting_Dev( pt );
					if ( xd <= 0 ) xq = 1; 
					else           xq = xd;
					if ( FindPosFromDataBuffer( pt , Cs , &yp ) ) {
						ys = Get_Graph_Plotting_Value( A , B , A_IO_PLT_DATA( pt, yp ) );
					}
					else {
						ys = 0;
					}
					for ( j = Cs+1 , xz = 0 ; j <= Ce ; xs++ , j++ ) {
						if ( FindPosFromDataBuffer( pt , j , &yp ) ) ye = Get_Graph_Plotting_Value( A , B , A_IO_PLT_DATA( pt, yp ) );
						else break;
						if ( A_IO_TOGGLE( pt ) < -1 ) {
							if ( V_LINE_DRAW_TYPE( SCREEN_2 ) == LIN_TYPE ) {
								KGUI_DRAW_Line( hDC , xs*xq , ys , (xs+1)*xq , ye , co , V_PLOTTER_POINT(SCREEN_2) );
							}
							else {
								KGUI_DRAW_Line( hDC , xs*xq , ys , xs*xq , ye , co , V_PLOTTER_POINT(SCREEN_2) );
								KGUI_DRAW_Line( hDC , xs*xq , ye , (xs+1)*xq , ye , co , V_PLOTTER_POINT(SCREEN_2) );
							}
						}
						else {
							if ( A_IO_TOGGLE( pt ) == -1 ) {
								if ( ys != ye ) {
									KGUI_DRAW_Line( hDC , (xs+1)*xq , 0 , (xs+1)*xq , 240 , co , V_PLOTTER_POINT(SCREEN_2) );
								}
							}
							else {
								if ( ( ys != ye ) && ( A_IO_PLT_DATA( pt, yp ) == A_IO_TOGGLE( pt ) ) ) {
									KGUI_DRAW_Line( hDC , (xs+1)*xq , 0 , (xs+1)*xq , 240 , co , V_PLOTTER_POINT(SCREEN_2) );
								}
							}
						}
						if ( xd <= 0 ) {
							if ( xz < xd ) {
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
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Drawing_Graph_BackGround_S1( HDC hDC , int dm ) {
	if      ( dm == SCR_SMALL_DRAW ) KGUI_DRAW_Box( hDC ,   0 ,   0 ,  DRAW_SIZE_X_SMALL , DRAW_SIZE_Y_SMALL , V_PLOTTER_BACK_COLOR( SCREEN_1 ) , V_PLOTTER_BACK_COLOR( SCREEN_1 ) );
	else if ( dm == SCR_BIG_DRAW   ) KGUI_DRAW_Box( hDC ,   0 ,   0 ,  DRAW_SIZE_X_BIG   , DRAW_SIZE_Y_BIG   , V_PLOTTER_BACK_COLOR( SCREEN_1 ) , V_PLOTTER_BACK_COLOR( SCREEN_1 ) );
}
//------------------------------------------------------------------------------------------
void Drawing_Graph_BackGround_S2( HDC hDC , int dm ) {
	if      ( dm == SCR_SMALL_DRAW ) KGUI_DRAW_Box( hDC ,   0 ,   0 ,  DRAW_SIZE_X_SMALL , DRAW_SIZE_Y_SMALL , V_PLOTTER_BACK_COLOR( SCREEN_2 ) , V_PLOTTER_BACK_COLOR( SCREEN_2 ) );
	else if ( dm == SCR_BIG_DRAW   ) KGUI_DRAW_Box( hDC ,   0 ,   0 ,  DRAW_SIZE_X_BIG   , DRAW_SIZE_Y_BIG   , V_PLOTTER_BACK_COLOR( SCREEN_2 ) , V_PLOTTER_BACK_COLOR( SCREEN_2 ) );
}
//------------------------------------------------------------------------------------------
void Drawing_Graph_Back( HDC hDC , int dm ) {
	if ( dm == SCR_SMALL_DRAW ) {
		KGUI_DRAW_Line( hDC ,   0 ,  30 ,  300 ,  30 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 ,  90 ,  300 ,  90 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 150 ,  300 , 150 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 210 ,  300 , 210 , COLOR_DarkGray , FALSE );

		KGUI_DRAW_Line( hDC ,  30 ,   0 ,   30 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,  90 ,   0 ,   90 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 150 ,   0 ,  150 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 210 ,   0 ,  210 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 270 ,   0 ,  270 , 240 , COLOR_DarkGray , FALSE );

		KGUI_DRAW_Line( hDC ,   0 ,  60 ,  300 ,  60 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 120 ,  300 , 120 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 180 ,  300 , 180 , COLOR_MediumGray , FALSE );

		KGUI_DRAW_Line( hDC ,  60 ,   0 ,   60 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 120 ,   0 ,  120 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 180 ,   0 ,  180 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 240 ,   0 ,  240 , 240 , COLOR_MediumGray , FALSE );
	}
	else if ( dm == SCR_BIG_DRAW ) {
		KGUI_DRAW_Line( hDC ,   0 ,  30 ,  720 ,  30 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 ,  90 ,  720 ,  90 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 150 ,  720 , 150 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 210 ,  720 , 210 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 270 ,  720 , 270 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 330 ,  720 , 330 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 390 ,  720 , 390 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 450 ,  720 , 450 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 510 ,  720 , 510 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 570 ,  720 , 570 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 630 ,  720 , 630 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 690 ,  720 , 690 , COLOR_DarkGray , FALSE );

		KGUI_DRAW_Line( hDC ,  30 ,   0 ,   30 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC ,  90 ,   0 ,   90 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 150 ,   0 ,  150 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 210 ,   0 ,  210 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 270 ,   0 ,  270 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 330 ,   0 ,  330 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 390 ,   0 ,  390 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 450 ,   0 ,  450 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 510 ,   0 ,  510 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 570 ,   0 ,  570 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 630 ,   0 ,  630 , 240 , COLOR_DarkGray , FALSE );
		KGUI_DRAW_Line( hDC , 690 ,   0 ,  690 , 240 , COLOR_DarkGray , FALSE );

		KGUI_DRAW_Line( hDC ,   0 ,  60 ,  720 ,  60 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 120 ,  720 , 120 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 180 ,  720 , 180 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 240 ,  720 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 300 ,  720 , 300 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 360 ,  720 , 360 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 420 ,  720 , 420 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 480 ,  720 , 480 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 540 ,  720 , 540 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 600 ,  720 , 600 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC ,   0 , 660 ,  720 , 660 , COLOR_MediumGray , FALSE );

		KGUI_DRAW_Line( hDC ,  60 ,   0 ,   60 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 120 ,   0 ,  120 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 180 ,   0 ,  180 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 240 ,   0 ,  240 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 300 ,   0 ,  300 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 360 ,   0 ,  360 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 420 ,   0 ,  420 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 480 ,   0 ,  480 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 540 ,   0 ,  540 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 600 ,   0 ,  600 , 240 , COLOR_MediumGray , FALSE );
		KGUI_DRAW_Line( hDC , 660 ,   0 ,  660 , 240 , COLOR_MediumGray , FALSE );
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Console_Gui_For_Drawing_S1( HDC hDC , BOOL BackDraw , int dm ) {
	HBITMAP hBitmap;
	HDC hMemDC;
	int x , y;
	if ( dm == SCR_SMALL_DRAW ) {
		hBitmap = CreateCompatibleBitmap( hDC , DRAW_SIZE_X_SMALL , DRAW_SIZE_Y_SMALL );
		hMemDC = CreateCompatibleDC( hDC );
		SelectObject( hMemDC , hBitmap );
		Drawing_Graph_BackGround_S1( hMemDC , dm );
		if ( BackDraw ) Drawing_Graph_Back( hMemDC , dm );
		Drawing_Graph_S1( hMemDC , dm );
		if ( Console_Drawing_Zoom_Ready( SCREEN_1 , &x , &y ) ) {
			KGUI_DRAW_StrBox( hMemDC , x - PLT1_X_START , y - PLT1_Y_START , "zoom" , 4 , COLOR_RightRed , COLOR_RightRed , COLOR_Black , COLOR_White );           // System Font
			KGUI_DRAW_Line( hMemDC , x - PLT1_X_START , y - PLT1_Y_START , x - PLT1_X_START + 60 , y - PLT1_Y_START , COLOR_RightRed , FALSE );
			KGUI_DRAW_Line( hMemDC , x - PLT1_X_START , y - PLT1_Y_START , x - PLT1_X_START , y - PLT1_Y_START + 55 , COLOR_RightRed , FALSE );
		}
		BitBlt( hDC , PLT1_X_START , PLT1_Y_START , DRAW_SIZE_X_SMALL , DRAW_SIZE_Y_SMALL , hMemDC , 0 , 0 , SRCCOPY );
		DeleteDC( hMemDC );
		DeleteObject( hBitmap );
		KWIN_Item_Show( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 );
		MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , 
			PLT1_X1_START_TIME + 70 , PLT_Y_START_TIME - 2 , DRAW_SIZE_X_SMALL - 134 , 20 , TRUE );
		if ( DISP_LAST_POINTER[SCREEN_1] < 1 ) {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , A_IO_PLT_COUNT( 0 ) , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 ) , PLT1_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 );
		}
		else {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , DISP_LAST_POINTER[SCREEN_1] , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 ) , PLT1_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Enable( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 );
		}
	}
	else if ( dm == SCR_BIG_DRAW ) {
		hBitmap = CreateCompatibleBitmap( hDC , DRAW_SIZE_X_BIG , DRAW_SIZE_Y_BIG );
		hMemDC = CreateCompatibleDC( hDC );
		SelectObject( hMemDC , hBitmap );
		Drawing_Graph_BackGround_S1( hMemDC , dm );
		if ( BackDraw ) Drawing_Graph_Back( hMemDC , dm );
		Drawing_Graph_S1( hMemDC , dm );
		if ( Console_Drawing_Zoom_Ready( SCREEN_1 , &x , &y ) ) {
			KGUI_DRAW_StrBox ( hMemDC , x - PLT0_X_START , y - PLT0_Y_START , "zoom" , 4 , COLOR_RightRed , COLOR_RightRed , COLOR_Black , COLOR_White );           // System Font
			KGUI_DRAW_Line( hMemDC , x - PLT0_X_START , y - PLT0_Y_START , x - PLT0_X_START + 60 , y - PLT0_Y_START , COLOR_RightRed , FALSE );
			KGUI_DRAW_Line( hMemDC , x - PLT0_X_START , y - PLT0_Y_START , x - PLT0_X_START , y - PLT0_Y_START + 55 , COLOR_RightRed , FALSE );
		}
		BitBlt( hDC , PLT0_X_START , PLT0_Y_START , DRAW_SIZE_X_BIG , DRAW_SIZE_Y_BIG , hMemDC , 0 , 0 , SRCCOPY );
		DeleteDC( hMemDC );
		DeleteObject( hBitmap );
		KWIN_Item_Show( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 );
		MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , 
			PLT0_X1_START_TIME + 70 , PLT_Y_START_TIME - 2 , DRAW_SIZE_X_BIG - 134 , 20 , TRUE );
		if ( DISP_LAST_POINTER[SCREEN_1] < 1 ) {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , A_IO_PLT_COUNT( 0 ) , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 ) , PLT0_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 );
		}
		else {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 ) , SB_CTL , DISP_LAST_POINTER[SCREEN_1] , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 ) , PLT0_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Enable( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 );
		}
	}
	else {
		KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_1 );
		//
		KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_1 );
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Console_Gui_For_Drawing_S2( HDC hDC , BOOL BackDraw , int dm ) {
	HBITMAP hBitmap;
	HDC hMemDC;
	int x , y;
	if ( dm == SCR_SMALL_DRAW ) {
		hBitmap = CreateCompatibleBitmap( hDC , DRAW_SIZE_X_SMALL , DRAW_SIZE_Y_SMALL );
		hMemDC = CreateCompatibleDC( hDC );
		SelectObject( hMemDC , hBitmap );
		Drawing_Graph_BackGround_S2( hMemDC , dm );
		if ( BackDraw ) Drawing_Graph_Back( hMemDC , dm );
		Drawing_Graph_S2( hMemDC , dm );
		if ( Console_Drawing_Zoom_Ready( SCREEN_2 , &x , &y ) ) {
			KGUI_DRAW_StrBox( hMemDC , x - PLT2_X_START , y - PLT2_Y_START , "zoom" , 4 , COLOR_RightRed , COLOR_RightRed , COLOR_Black , COLOR_White );           // System Font
			KGUI_DRAW_Line( hMemDC , x - PLT2_X_START , y - PLT2_Y_START , x - PLT2_X_START + 60 , y - PLT2_Y_START , COLOR_RightRed , FALSE );
			KGUI_DRAW_Line( hMemDC , x - PLT2_X_START , y - PLT2_Y_START , x - PLT2_X_START , y - PLT2_Y_START + 55 , COLOR_RightRed , FALSE );
		}
		BitBlt( hDC , PLT2_X_START , PLT2_Y_START , DRAW_SIZE_X_SMALL , DRAW_SIZE_Y_SMALL , hMemDC , 0 , 0 , SRCCOPY );
		DeleteDC( hMemDC );
		DeleteObject( hBitmap );
		KWIN_Item_Show( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 );
		MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , 
			PLT2_X1_START_TIME + 70 , PLT_Y_START_TIME - 2 , DRAW_SIZE_X_SMALL - 134 , 20 , TRUE );
		if ( DISP_LAST_POINTER[SCREEN_2] < 1 ) {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , A_IO_PLT_COUNT( 0 ) , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 ) , PLT2_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 );
		}
		else {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , DISP_LAST_POINTER[SCREEN_2] , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 ) , PLT2_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Enable( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 );
		}
	}
	else if ( dm == SCR_BIG_DRAW ) {
		hBitmap = CreateCompatibleBitmap( hDC , DRAW_SIZE_X_BIG , DRAW_SIZE_Y_BIG );
		hMemDC = CreateCompatibleDC( hDC );
		SelectObject( hMemDC , hBitmap );
		Drawing_Graph_BackGround_S2( hMemDC , dm );
		if ( BackDraw ) Drawing_Graph_Back( hMemDC , dm );
		Drawing_Graph_S2( hMemDC , dm );
		if ( Console_Drawing_Zoom_Ready( SCREEN_2 , &x , &y ) ) {
			KGUI_DRAW_StrBox( hMemDC , x - PLT0_X_START , y - PLT0_Y_START , "zoom" , 4 , COLOR_RightRed , COLOR_RightRed , COLOR_Black , COLOR_White );           // System Font
			KGUI_DRAW_Line( hMemDC , x - PLT0_X_START , y - PLT0_Y_START , x - PLT0_X_START + 60 , y - PLT0_Y_START , COLOR_RightRed , FALSE );
			KGUI_DRAW_Line( hMemDC , x - PLT0_X_START , y - PLT0_Y_START , x - PLT0_X_START , y - PLT0_Y_START + 55 , COLOR_RightRed , FALSE );
		}
		BitBlt( hDC , PLT0_X_START , PLT0_Y_START , DRAW_SIZE_X_BIG , DRAW_SIZE_Y_BIG , hMemDC , 0 , 0 , SRCCOPY );
		DeleteDC( hMemDC );
		DeleteObject( hBitmap );
		KWIN_Item_Show( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 );
		MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , 
			PLT0_X1_START_TIME + 70 , PLT_Y_START_TIME - 2 , DRAW_SIZE_X_BIG - 134 , 20 , TRUE );
		if ( DISP_LAST_POINTER[SCREEN_2] < 1 ) {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , A_IO_PLT_COUNT( 0 ) , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 ) , PLT0_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 );
		}
		else {
			SetScrollRange( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , 1 , A_IO_PLT_COUNT( 0 ) , TRUE );
			SetScrollPos( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 ) , SB_CTL , DISP_LAST_POINTER[SCREEN_2] , TRUE );
			//
			MoveWindow( GetDlgItem( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 ) , PLT0_X2_START_TIME + 80 , PLT_Y_START_TIME - 2 , 20 , 20 , TRUE );
			KWIN_Item_Enable( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 );
		}
	}
	else {
		KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_SCROLL_2 );
		//
		KWIN_Item_Hide( PLOT_MAIN_WINDOW , ID_PLOT_AUTO_2 );
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Console_Gui_For_Item( HDC hDC ) {
	if ( HEADER_MESSAGE_IO >= 0 ) {
		KGUI_DRAW_StrBox2( hDC , HEADER_MESSAGE_DISPLAY_START_X , HEADER_MESSAGE_DISPLAY_START_Y , HEADER_MESSAGE_STRING , HEADER_MESSAGE_SIZE , 0 , COLOR_White , COLOR_DarkGray , COLOR_Black , COLOR_LightGray );
	}
	Drawing_ItemDraw_S1( hDC , V_PLOTTER_DRAW_MODE( SCREEN_1 ) );
	Drawing_ItemDraw_S2( hDC , V_PLOTTER_DRAW_MODE( SCREEN_2 ) );
}
//------------------------------------------------------------------------------------------
void Console_Gui_For_Drawing( HDC hDC , int mode ) {
	if ( mode == -1 ) {
		Console_Gui_For_Drawing_S1( hDC , V_PLOTTER_BACK_DRAW( SCREEN_1 ) , V_PLOTTER_DRAW_MODE( SCREEN_1 ) );
		Console_Gui_For_Drawing_S2( hDC , V_PLOTTER_BACK_DRAW( SCREEN_2 ) , V_PLOTTER_DRAW_MODE( SCREEN_2 ) );
	}
	else {
		if ( mode == SCREEN_1 ) {
			Console_Gui_For_Drawing_S1( hDC , V_PLOTTER_BACK_DRAW(mode) , V_PLOTTER_DRAW_MODE(mode) );
		}
		else {
			Console_Gui_For_Drawing_S2( hDC , V_PLOTTER_BACK_DRAW(mode) , V_PLOTTER_DRAW_MODE(mode) );
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
