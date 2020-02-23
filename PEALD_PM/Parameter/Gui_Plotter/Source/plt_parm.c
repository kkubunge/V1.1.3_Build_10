#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>
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

//========================================================================================
//========================================================================================
//========================================================================================

int	PLOTTER_TABLE_1[4] = { -1 , -1 , -1 , -1 };
int	PLOTTER_TABLE_2[4] = { -1 , -1 , -1 , -1 };

int	PLOTTER_POINT_1 = TRUE;
int	PLOTTER_POINT_2 = TRUE;

BOOL PLOTTER_BIG_1 = FALSE;
BOOL PLOTTER_BIG_2 = FALSE;

int	PLOTTER_BACK_1 = TRUE;
int	PLOTTER_BACK_2 = TRUE;

int	PLOTTER_BACK_COLOR_1 = COLOR_Black;
int	PLOTTER_BACK_COLOR_2 = COLOR_Black;

int	PLOTTER_COLOR_1[4] = { COLOR_Magenta , COLOR_White , COLOR_RightRed , COLOR_Yellow };
int	PLOTTER_COLOR_2[4] = { COLOR_Magenta , COLOR_White , COLOR_RightRed , COLOR_Yellow };

int		PLOTTER_SAVE_TAB_TYPE = 0;
BOOL	START_LEFT_DRAW = FALSE;
BOOL	AUTO_SCALE_DRAW = FALSE;
BOOL	AUTO_SCALE_TIME = FALSE;

BOOL	AUTO_RETURN_TO_MONITOR = FALSE; // 2003.02.11
//========================================================================================
int		AUTO_SCALE_DEFAULT_DEV = 2; // 2005.05.24
//========================================================================================
//========================================================================================
//========================================================================================
int V_PLOTTER_SAVE_TAB_TYPE() {
	return PLOTTER_SAVE_TAB_TYPE;
}
//
void S_PLOTTER_SAVE_TAB_TYPE( int s ) {
	PLOTTER_SAVE_TAB_TYPE = s;
}
//========================================================================================
BOOL V_PLOTTER_TABLE_BLANK( int p ) {
	int i;
	for ( i = 0 ; i < 4 ; i++ ) {
		if ( V_PLOTTER_TABLE(p,i) != -1 ) return FALSE;
	}
	return TRUE;
}
//========================================================================================
int V_PLOTTER_DRAW_MODE( int p ) {
	int i;
	BOOL b1,b2;
	b1 = V_PLOTTER_BIG( SCREEN_1 );
	b2 = V_PLOTTER_BIG( SCREEN_2 );
	if ( !b1 && !b2 ) {
		return SCR_SMALL_DRAW;
	}
	else if ( b1 && !b2 ) {
		if ( p == SCREEN_1 ) {
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i) != -1 ) return SCR_SMALL_DRAW;
			}
			return SCR_BIG_DRAW;
		}
		else {
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i) != -1 ) return SCR_SMALL_DRAW;
			}
			return SCR_HIDE_DRAW;
		}
	}
	else if ( !b1 && b2 ) {
		if ( p == SCREEN_1 ) {
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i) != -1 ) return SCR_SMALL_DRAW;
			}
			return SCR_HIDE_DRAW;
		}
		else {
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i) != -1 ) return SCR_SMALL_DRAW;
			}
			return SCR_BIG_DRAW;
		}
	}
	else if ( b1 && b2 ) {
		if ( p == SCREEN_1 ) {
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i) != -1 ) break;
			}
			if ( i == 4 ) return SCR_HIDE_DRAW;
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i) != -1 ) return SCR_SMALL_DRAW;
			}
			return SCR_BIG_DRAW;
		}
		else {
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_2,i) != -1 ) break;
			}
			if ( i == 4 ) return SCR_HIDE_DRAW;
			for ( i = 0 ; i < 4 ; i++ ) {
				if ( V_PLOTTER_TABLE(SCREEN_1,i) != -1 ) return SCR_SMALL_DRAW;
			}
			return SCR_BIG_DRAW;
		}
	}
	return SCR_SMALL_DRAW;
}
//========================================================================================
int V_PLOTTER_BACK_COLOR( int p ) {
	if      ( p == SCREEN_1 ) return PLOTTER_BACK_COLOR_1;
	else if ( p == SCREEN_2 ) return PLOTTER_BACK_COLOR_2;
	return PLOTTER_BACK_COLOR_1;
}
void S_PLOTTER_BACK_COLOR( int p , int s ) {
	if      ( p == SCREEN_1 ) PLOTTER_BACK_COLOR_1 = s;
	else if ( p == SCREEN_2 ) PLOTTER_BACK_COLOR_2 = s;
}
//========================================================================================
BOOL V_PLOTTER_BACK_DRAW( int p ) {
	if      ( p == SCREEN_1 ) return PLOTTER_BACK_1;
	else if ( p == SCREEN_2 ) return PLOTTER_BACK_2;
	return PLOTTER_BACK_1;
}
void S_PLOTTER_BACK_DRAW( int p , BOOL s ) {
	if      ( p == SCREEN_1 ) PLOTTER_BACK_1 = s;
	else if ( p == SCREEN_2 ) PLOTTER_BACK_2 = s;
}
//========================================================================================
BOOL V_PLOTTER_POINT( int p ) {
	if      ( p == SCREEN_1 ) return PLOTTER_POINT_1;
	else if ( p == SCREEN_2 ) return PLOTTER_POINT_2;
	return PLOTTER_POINT_1;
}
void S_PLOTTER_POINT( int p , BOOL v ) {
	if      ( p == SCREEN_1 ) PLOTTER_POINT_1 = v;
	else if ( p == SCREEN_2 ) PLOTTER_POINT_2 = v;
}
//========================================================================================
int  V_PLOTTER_TABLE( int p , int m ) {
	if      ( p == SCREEN_1 ) return PLOTTER_TABLE_1[m];
	else if ( p == SCREEN_2 ) return PLOTTER_TABLE_2[m];
	return 0;
}
void S_PLOTTER_TABLE( int p , int m , int v ) {
	if      ( p == SCREEN_1 ) PLOTTER_TABLE_1[m] = v;
	else if ( p == SCREEN_2 ) PLOTTER_TABLE_2[m] = v;
}
//========================================================================================
int  V_PLOTTER_COLOR( int p , int m ) {
	if      ( p == SCREEN_1 ) return PLOTTER_COLOR_1[m];
	else if ( p == SCREEN_2 ) return PLOTTER_COLOR_2[m];
	return 0;
}
void S_PLOTTER_COLOR( int p , int m , int v ) {
	if      ( p == SCREEN_1 ) PLOTTER_COLOR_1[m] = v;
	else if ( p == SCREEN_2 ) PLOTTER_COLOR_2[m] = v;
}
//========================================================================================
BOOL V_PLOTTER_BIG( int p ) {
	if      ( p == SCREEN_1 ) return PLOTTER_BIG_1;
	else if ( p == SCREEN_2 ) return PLOTTER_BIG_2;
	return PLOTTER_BIG_1;
}
void S_PLOTTER_BIG( int p , BOOL v ) {
	if      ( p == SCREEN_1 ) PLOTTER_BIG_1 = v;
	else if ( p == SCREEN_2 ) PLOTTER_BIG_2 = v;
}
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
void S_START_LEFT_DRAW( BOOL d ) {
	START_LEFT_DRAW = d;
}

BOOL V_START_LEFT_DRAW() {
	return START_LEFT_DRAW;
}
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
void S_AUTO_SCALE_DEFAULT_DEV( int d ) { // 2005.05.24
	AUTO_SCALE_DEFAULT_DEV = d;
}

int V_AUTO_SCALE_DEFAULT_DEV() { // 2005.05.24
	return AUTO_SCALE_DEFAULT_DEV;
}
//========================================================================================
void S_AUTO_SCALE_DRAW( BOOL d ) {
	AUTO_SCALE_DRAW = d;
}

BOOL V_AUTO_SCALE_DRAW() {
	return AUTO_SCALE_DRAW;
}
//========================================================================================
void S_AUTO_SCALE_TIME( BOOL d ) {
	AUTO_SCALE_TIME = d;
}

BOOL V_AUTO_SCALE_TIME() {
	return AUTO_SCALE_TIME;
}

//========================================================================================
//========================================================================================
void S_AUTO_RETURN_TO_MONITOR( BOOL d ) { // 2003.02.11
	AUTO_RETURN_TO_MONITOR = d;
}

BOOL V_AUTO_RETURN_TO_MONITOR() { // 2003.02.11
	return AUTO_RETURN_TO_MONITOR;
}


