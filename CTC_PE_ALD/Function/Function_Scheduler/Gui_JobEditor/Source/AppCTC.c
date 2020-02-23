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
#include <Kutlgui.h>


#include "appctc.h"
#include "appprm.h"

#include "iodefine.h"
#include "cimseqnc.h"
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
int IO_Adress_MAIN_CONTROL[MAX_SIDE];
int IO_Adress_MAIN_BM_CONTROL[MAX_SIDE]; // 2010.03.10
int IO_Adress_END_CONTROL[MAX_SIDE];
int IO_Adress_IN_PATH[MAX_SIDE];
int IO_Adress_OUT_PATH[MAX_SIDE];
int IO_Adress_START_SLOT[MAX_SIDE];
int IO_Adress_END_SLOT[MAX_SIDE];
int IO_Adress_LOOP_COUNT[MAX_SIDE];

int IO_Adress_DONE_WFR_STATUS[MAX_SIDE];
int IO_Adress_CMH_CONTROL[MAX_SIDE];
int IO_Adress_JOB_NAME[MAX_SIDE];
int IO_Adress_MID_NAME[MAX_SIDE];
int IO_Adress_RECIPE_FILE[MAX_SIDE];
int IO_Adress_RECIPE_BM_FILE[MAX_SIDE]; // 2010.03.10
int IO_Adress_START_TIME[MAX_SIDE];
int IO_Adress_END_TIME[MAX_SIDE];
int IO_Adress_ELAPSED_TIME[MAX_SIDE];
int IO_Adress_FIRSTIN_TIME[MAX_SIDE];
int IO_Adress_LASTIN_TIME[MAX_SIDE];
int IO_Adress_DIRECTORY_PATH[MAX_SIDE];
int IO_Adress_HANDOFF_CM[MAX_SIDE];
int IO_Adress_SCHEDULER;

//===================================================================
void IO_READ_PART() {
	int i;
	//
	IO_Adress_SCHEDULER = -99; // for Control
	//
	for ( i = 0 ; i < MAX_SIDE ; i++ ) {
		// for Control
		IO_Adress_HANDOFF_CM[i] = -99;
		IO_Adress_END_CONTROL[i] = -99;
		IO_Adress_LOOP_COUNT[i] = -99;
		IO_Adress_CMH_CONTROL[i] = -99;

		// for Monitoring
		IO_Adress_MAIN_CONTROL[i] = -99;
		IO_Adress_MAIN_BM_CONTROL[i] = -99; // 2010.03.10
		IO_Adress_IN_PATH[i] = -99;
		IO_Adress_OUT_PATH[i] = -99;
		IO_Adress_START_SLOT[i] = -99;
		IO_Adress_END_SLOT[i] = -99;

		IO_Adress_RECIPE_FILE[i] = -99;
		IO_Adress_RECIPE_BM_FILE[i] = -99; // 2010.03.10
		IO_Adress_DONE_WFR_STATUS[i] = -99;

		IO_Adress_JOB_NAME[i] = -99;
		IO_Adress_MID_NAME[i] = -99;
		IO_Adress_START_TIME[i] = -99;
		IO_Adress_END_TIME[i] = -99;
		IO_Adress_ELAPSED_TIME[i] = -99;
		IO_Adress_FIRSTIN_TIME[i] = -99;
		IO_Adress_LASTIN_TIME[i] = -99;
		IO_Adress_DIRECTORY_PATH[i] = -99;
	}

	if ( Get_Prm_NETMONITOR_MODE() ) return;

//	IO_Adress_SCHEDULER			= _FIND_FROM_STRING( _K_F_IO , "SCHEDULER" );

//	IO_Adress_HANDOFF_CM[0]		= _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM1" );
//	IO_Adress_HANDOFF_CM[1]		= _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM2" );
//	IO_Adress_HANDOFF_CM[2]		= _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM3" );
//	IO_Adress_HANDOFF_CM[3]		= _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM4" );

//	IO_Adress_MAIN_CONTROL[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL" );
//	IO_Adress_MAIN_CONTROL[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL2" );
//	IO_Adress_MAIN_CONTROL[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL3" );
//	IO_Adress_MAIN_CONTROL[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL4" );

//	IO_Adress_END_CONTROL[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL" );
//	IO_Adress_END_CONTROL[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL2" );
//	IO_Adress_END_CONTROL[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL3" );
//	IO_Adress_END_CONTROL[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL4" );
	
//	IO_Adress_IN_PATH[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH" );
//	IO_Adress_IN_PATH[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH2" );
//	IO_Adress_IN_PATH[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH3" );
//	IO_Adress_IN_PATH[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH4" );
	
//	IO_Adress_OUT_PATH[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH" );
//	IO_Adress_OUT_PATH[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH2" );
//	IO_Adress_OUT_PATH[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH3" );
//	IO_Adress_OUT_PATH[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH4" );
	
//	IO_Adress_START_SLOT[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT" );
//	IO_Adress_START_SLOT[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT2" );
//	IO_Adress_START_SLOT[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT3" );
//	IO_Adress_START_SLOT[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT4" );
	
//	IO_Adress_END_SLOT[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT" );
//	IO_Adress_END_SLOT[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT2" );
//	IO_Adress_END_SLOT[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT3" );
//	IO_Adress_END_SLOT[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT4" );
	
//	IO_Adress_LOOP_COUNT[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT" );
//	IO_Adress_LOOP_COUNT[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT2" );
//	IO_Adress_LOOP_COUNT[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT3" );
//	IO_Adress_LOOP_COUNT[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT4" );
	
//	IO_Adress_DONE_WFR_STATUS[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS" );
//	IO_Adress_DONE_WFR_STATUS[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS2" );
//	IO_Adress_DONE_WFR_STATUS[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS3" );
//	IO_Adress_DONE_WFR_STATUS[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS4" );
	
//	IO_Adress_CMH_CONTROL[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.CM1H_CONTROL" );
//	IO_Adress_CMH_CONTROL[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.CM2H_CONTROL" );
//	IO_Adress_CMH_CONTROL[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.CM3H_CONTROL" );
//	IO_Adress_CMH_CONTROL[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.CM4H_CONTROL" );
	
//	IO_Adress_JOB_NAME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME" );
//	IO_Adress_JOB_NAME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME2" );
//	IO_Adress_JOB_NAME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME3" );
//	IO_Adress_JOB_NAME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME4" );
	
//	IO_Adress_MID_NAME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME" );
//	IO_Adress_MID_NAME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME2" );
//	IO_Adress_MID_NAME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME3" );
//	IO_Adress_MID_NAME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME4" );
	
//	IO_Adress_RECIPE_FILE[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE" );
//	IO_Adress_RECIPE_FILE[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE2" );
//	IO_Adress_RECIPE_FILE[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE3" );
//	IO_Adress_RECIPE_FILE[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE4" );
	
//	IO_Adress_START_TIME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME" );
//	IO_Adress_START_TIME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME2" );
//	IO_Adress_START_TIME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME3" );
//	IO_Adress_START_TIME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME4" );
	
//	IO_Adress_END_TIME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME" );
//	IO_Adress_END_TIME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME2" );
//	IO_Adress_END_TIME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME3" );
//	IO_Adress_END_TIME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME4" );
	
//	IO_Adress_ELAPSED_TIME[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME" );
//	IO_Adress_ELAPSED_TIME[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME2" );
//	IO_Adress_ELAPSED_TIME[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME3" );
//	IO_Adress_ELAPSED_TIME[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME4" );
	
//	IO_Adress_DIRECTORY_PATH[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY" );
//	IO_Adress_DIRECTORY_PATH[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY2" );
//	IO_Adress_DIRECTORY_PATH[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY3" );
//	IO_Adress_DIRECTORY_PATH[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY4" );
	
//	IO_Adress_FIRSTIN_TIME[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME" );
//	IO_Adress_FIRSTIN_TIME[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME2" );
//	IO_Adress_FIRSTIN_TIME[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME3" );
//	IO_Adress_FIRSTIN_TIME[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME4" );
	
//	IO_Adress_LASTIN_TIME[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME" );
//	IO_Adress_LASTIN_TIME[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME2" );
//	IO_Adress_LASTIN_TIME[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME3" );
//	IO_Adress_LASTIN_TIME[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME4" );
}

int IO_READ_for_Adress_SCHEDULER() { // 2003.08.01
	if ( IO_Adress_SCHEDULER == -99 ) IO_Adress_SCHEDULER = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER" );
	return IO_Adress_SCHEDULER;
}

int IO_READ_for_Adress_HANDOFF_CM( int id ) { // 2003.08.01
	//
	if ( ( id < 0 ) || ( id >= MAX_SIDE ) ) return -1; // 2011.01.17
	//
	if ( IO_Adress_HANDOFF_CM[id] == -99 ) IO_Adress_HANDOFF_CM[id] = _FIND_FROM_STRINGF( _K_F_IO , "SCHEDULER_HANDOFF_CM%d" , id + 1 ); // 2011.01.17
	return IO_Adress_HANDOFF_CM[id];
/*
	switch( id ) {
	case 0 :
		if ( IO_Adress_HANDOFF_CM[0] == -99 ) IO_Adress_HANDOFF_CM[0] = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM1" );
		return IO_Adress_HANDOFF_CM[0];
		break;
	case 1 :
		if ( IO_Adress_HANDOFF_CM[1] == -99 ) IO_Adress_HANDOFF_CM[1] = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM2" );
		return IO_Adress_HANDOFF_CM[1];
		break;
	case 2 :
		if ( IO_Adress_HANDOFF_CM[2] == -99 ) IO_Adress_HANDOFF_CM[2] = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM3" );
		return IO_Adress_HANDOFF_CM[2];
		break;
	case 3 :
		if ( IO_Adress_HANDOFF_CM[3] == -99 ) IO_Adress_HANDOFF_CM[3] = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_HANDOFF_CM4" );
		return IO_Adress_HANDOFF_CM[3];
		break;
	}
	return -1;
*/
}

int IO_READ_for_Adress_END_CONTROL( int id ) { // 2003.08.01
	//
	if ( ( id < 0 ) || ( id >= MAX_SIDE ) ) return -1; // 2011.01.17
	//
	if ( IO_Adress_END_CONTROL[id] == -99 ) { // 2011.01.26
		if ( id == 0 ) {
			IO_Adress_END_CONTROL[id] = _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL" );
		}
		else {
			IO_Adress_END_CONTROL[id] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.END_CONTROL%d" , id + 1 );
		}
	}
	return IO_Adress_END_CONTROL[id];

/*
	switch( id ) {
	case 0 :
		if ( IO_Adress_END_CONTROL[0] == -99 ) IO_Adress_END_CONTROL[0] = _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL" );
		return IO_Adress_END_CONTROL[0];
		break;
	case 1 :
		if ( IO_Adress_END_CONTROL[1] == -99 ) IO_Adress_END_CONTROL[1] = _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL2" );
		return IO_Adress_END_CONTROL[1];
		break;
	case 2 :
		if ( IO_Adress_END_CONTROL[2] == -99 ) IO_Adress_END_CONTROL[2] = _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL3" );
		return IO_Adress_END_CONTROL[2];
		break;
	case 3 :
		if ( IO_Adress_END_CONTROL[3] == -99 ) IO_Adress_END_CONTROL[3] = _FIND_FROM_STRING( _K_D_IO , "CTC.END_CONTROL4" );
		return IO_Adress_END_CONTROL[3];
		break;
	}
	return -1;
*/
}

int IO_READ_for_Adress_LOOP_COUNT( int id ) { // 2003.08.01
	//
	if ( ( id < 0 ) || ( id >= MAX_SIDE ) ) return -1; // 2011.01.17
	//
	if ( IO_Adress_LOOP_COUNT[id] == -99 ) { // 2011.01.26
		if ( id == 0 ) {
			IO_Adress_LOOP_COUNT[id] = _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT" );
		}
		else {
			IO_Adress_LOOP_COUNT[id] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.LOOP_COUNT%d" , id + 1 );
		}
	}
	return IO_Adress_LOOP_COUNT[id];
/*
	switch( id ) {
	case 0 :
		if ( IO_Adress_LOOP_COUNT[0] == -99 ) IO_Adress_LOOP_COUNT[0] = _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT" );
		return IO_Adress_LOOP_COUNT[0];
		break;
	case 1 :
		if ( IO_Adress_LOOP_COUNT[1] == -99 ) IO_Adress_LOOP_COUNT[1] = _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT2" );
		return IO_Adress_LOOP_COUNT[1];
		break;
	case 2 :
		if ( IO_Adress_LOOP_COUNT[2] == -99 ) IO_Adress_LOOP_COUNT[2] = _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT3" );
		return IO_Adress_LOOP_COUNT[2];
		break;
	case 3 :
		if ( IO_Adress_LOOP_COUNT[3] == -99 ) IO_Adress_LOOP_COUNT[3] = _FIND_FROM_STRING( _K_D_IO , "CTC.LOOP_COUNT4" );
		return IO_Adress_LOOP_COUNT[3];
		break;
	}
	return -1;
*/
}

int IO_READ_for_Adress_CMH_CONTROL( int id ) { // 2003.08.01
	//
	if ( ( id < 0 ) || ( id >= MAX_SIDE ) ) return -1; // 2011.01.17
	//
	if ( IO_Adress_CMH_CONTROL[id] == -99 ) IO_Adress_CMH_CONTROL[id] = _FIND_FROM_STRINGF( _K_F_IO , "CTC.CM%dH_CONTROL" , id + 1 ); // 2011.01.17
	return IO_Adress_CMH_CONTROL[id];
/*
	switch( id ) {
	case 0 :
		if ( IO_Adress_CMH_CONTROL[0] == -99 ) IO_Adress_CMH_CONTROL[0] = _FIND_FROM_STRING( _K_D_IO , "CTC.CM1H_CONTROL" );
		return IO_Adress_CMH_CONTROL[0];
		break;
	case 1 :
		if ( IO_Adress_CMH_CONTROL[1] == -99 ) IO_Adress_CMH_CONTROL[1] = _FIND_FROM_STRING( _K_D_IO , "CTC.CM2H_CONTROL" );
		return IO_Adress_CMH_CONTROL[1];
		break;
	case 2 :
		if ( IO_Adress_CMH_CONTROL[2] == -99 ) IO_Adress_CMH_CONTROL[2] = _FIND_FROM_STRING( _K_D_IO , "CTC.CM3H_CONTROL" );
		return IO_Adress_CMH_CONTROL[2];
		break;
	case 3 :
		if ( IO_Adress_CMH_CONTROL[3] == -99 ) IO_Adress_CMH_CONTROL[3] = _FIND_FROM_STRING( _K_D_IO , "CTC.CM4H_CONTROL" );
		return IO_Adress_CMH_CONTROL[3];
		break;
	}
	return -1;
*/
}

int IO_READ_for_Adress_MAIN_CONTROL( int id ) { // 2003.08.01
	//
	if ( ( id < 0 ) || ( id >= MAX_SIDE ) ) return -1; // 2011.01.17
	//
	if ( IO_Adress_MAIN_CONTROL[id] == -99 ) { // 2011.01.26
		if ( id == 0 ) {
			IO_Adress_MAIN_CONTROL[id] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL" );
		}
		else {
			IO_Adress_MAIN_CONTROL[id] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.MAIN_CONTROL%d" , id + 1 );
		}
	}
	return IO_Adress_MAIN_CONTROL[id];
/*
	switch( id ) {
	case 0 :
		if ( IO_Adress_MAIN_CONTROL[0] == -99 ) IO_Adress_MAIN_CONTROL[0] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL" );
		return IO_Adress_MAIN_CONTROL[0];
		break;
	case 1 :
		if ( IO_Adress_MAIN_CONTROL[1] == -99 ) IO_Adress_MAIN_CONTROL[1] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL2" );
		return IO_Adress_MAIN_CONTROL[1];
		break;
	case 2 :
		if ( IO_Adress_MAIN_CONTROL[2] == -99 ) IO_Adress_MAIN_CONTROL[2] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL3" );
		return IO_Adress_MAIN_CONTROL[2];
		break;
	case 3 :
		if ( IO_Adress_MAIN_CONTROL[3] == -99 ) IO_Adress_MAIN_CONTROL[3] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_CONTROL4" );
		return IO_Adress_MAIN_CONTROL[3];
		break;
	}
	return -1;
*/
}

int IO_READ_for_Adress_MAIN_BM_CONTROL( int id ) { // 2010.03.10
	//
	if ( ( id < 0 ) || ( id >= MAX_SIDE ) ) return -1; // 2011.01.17
	//
	if ( IO_Adress_MAIN_BM_CONTROL[id] == -99 ) { // 2011.01.26
		if ( id == 0 ) {
			IO_Adress_MAIN_BM_CONTROL[id] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_BM_CONTROL" );
		}
		else {
			IO_Adress_MAIN_BM_CONTROL[id] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.MAIN_BM_CONTROL%d" , id + 1 );
		}
	}
	return IO_Adress_MAIN_BM_CONTROL[id];
/*
	switch( id ) {
	case 0 :
		if ( IO_Adress_MAIN_BM_CONTROL[0] == -99 ) IO_Adress_MAIN_BM_CONTROL[0] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_BM_CONTROL" );
		return IO_Adress_MAIN_BM_CONTROL[0];
		break;
	case 1 :
		if ( IO_Adress_MAIN_BM_CONTROL[1] == -99 ) IO_Adress_MAIN_BM_CONTROL[1] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_BM_CONTROL2" );
		return IO_Adress_MAIN_BM_CONTROL[1];
		break;
	case 2 :
		if ( IO_Adress_MAIN_BM_CONTROL[2] == -99 ) IO_Adress_MAIN_BM_CONTROL[2] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_BM_CONTROL3" );
		return IO_Adress_MAIN_BM_CONTROL[2];
		break;
	case 3 :
		if ( IO_Adress_MAIN_BM_CONTROL[3] == -99 ) IO_Adress_MAIN_BM_CONTROL[3] = _FIND_FROM_STRING( _K_D_IO , "CTC.MAIN_BM_CONTROL4" );
		return IO_Adress_MAIN_BM_CONTROL[3];
		break;
	}
	return -1;
*/
}
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//===================================================================
//
//----------+-------------------------------------------------------------------------
//          |   0       1       2        3        4        5        6          7
//----------+--------------------------------------------------------------------------
//	Ending  | Idle                                      Disable            Wait-HandOff
//	Waiting |                                                     Waiting
//	Running |        Running  Pausing  Paused  Aborting
//	Ready   | Idle
//----------+-------------------------------------------------------------
//
//
BOOL RUN_FUNCTION_For_ENDING_CHECK( int CHECKING_SIDE ) {
	int CommStatus , i , j , ib , jb;
	//
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j < 0 ) {
		i = 0;
	}
	else {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	//
	jb = IO_READ_for_Adress_MAIN_BM_CONTROL( CHECKING_SIDE ); // 2010.03.10
	if ( jb < 0 ) {
		ib = 0;
	}
	else {
		ib = _dREAD_DIGITAL( jb , &CommStatus );
	}
	//
//	if ( i == 0 ) return TRUE; // 2010.03.10
//	if ( i == 5 ) return TRUE; // 2010.03.10
//	if ( i >= 7 ) return TRUE; // 2010.03.10
	if ( ( i == 0 ) || ( i == 5 ) || ( i >= 7 ) ) { // 2010.03.10
		if ( ( ib == 0 ) || ( ib == 5 ) || ( ib >= 7 ) ) { // 2010.03.10
			return TRUE;
		}
	}
	return FALSE;
}

int RUN_FUNCTION_For_RUNWAIT_CHECK( int CHECKING_SIDE ) {
	int CommStatus , i , j;
	//
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j < 0 ) {
		i = 0;
	}
	else {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	if ( i == 1 ) return 1;
	if ( i == 2 ) return 1;
	if ( i == 3 ) return 1;
	if ( i == 4 ) return 1;
	if ( i == 6 ) return 2;
	//
	j = IO_READ_for_Adress_MAIN_BM_CONTROL( CHECKING_SIDE ); // 2010.03.10
	if ( j < 0 ) {
		i = 0;
	}
	else {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	if ( i == 1 ) return 1;
	if ( i == 2 ) return 1;
	if ( i == 3 ) return 1;
	if ( i == 4 ) return 1;
	if ( i == 6 ) return 2;
	//
	return 0;
}

BOOL RUN_FUNCTION_For_READY_CHECK( int CHECKING_SIDE ) {
	int CommStatus , j , i , jb , ib;
	//
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		//if ( _dREAD_DIGITAL( j , &CommStatus ) == 0 ) return TRUE; // 2010.03.10
		i = _dREAD_DIGITAL( j , &CommStatus ); // 2010.03.10
	}
	else { // 2010.03.01
		i = 0;
	}
	//
	jb = IO_READ_for_Adress_MAIN_BM_CONTROL( CHECKING_SIDE ); // 2010.03.10
	if ( jb >= 0 ) {
		ib = _dREAD_DIGITAL( j , &CommStatus );
	}
	else {
		ib = 0;
	}
	//
	if ( ( i == 0 ) && ( ib == 0 ) ) return TRUE; // 2010.03.10
	//
	return FALSE;
}


int RUN_FUNCTION_For_Recipe_Select( int CHECKING_SIDE , char *RcpFile ) { // Just Use for GUI Start
	int CommStatus , j;
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == 0 ) {
			//
			if ( IO_Adress_RECIPE_FILE[CHECKING_SIDE] == -99 ) { // 2011.01.26
				if ( CHECKING_SIDE == 0 ) {
					IO_Adress_RECIPE_FILE[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE" );
				}
				else {
					IO_Adress_RECIPE_FILE[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.RECIPE_FILE%d" , CHECKING_SIDE + 1 );
				}
			}
/*
			if ( IO_Adress_RECIPE_FILE[CHECKING_SIDE] == -99 ) { // 2003.08.01
				switch( CHECKING_SIDE ) {
				case 0 :	IO_Adress_RECIPE_FILE[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE" );		break;
				case 1 :	IO_Adress_RECIPE_FILE[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE2" );	break;
				case 2 :	IO_Adress_RECIPE_FILE[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE3" );	break;
				case 3 :	IO_Adress_RECIPE_FILE[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE4" );	break;
				}
			}
*/

			if ( IO_Adress_RECIPE_FILE[CHECKING_SIDE] >= 0 ) {
				_dWRITE_STRING( IO_Adress_RECIPE_FILE[CHECKING_SIDE] , RcpFile , &CommStatus );
			}
			return Success;
		}
	}
	return Not_Enable_Status;
}
//
int RUN_FUNCTION_For_PATH_Select( int CHECKING_SIDE , long Path , long Path2 ) { // Just Use for GUI Start
	int CommStatus , j;
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == 0 ) {
			//
			if ( IO_Adress_IN_PATH[CHECKING_SIDE] == -99 ) { // 2011.01.26
				if ( CHECKING_SIDE == 0 ) {
					IO_Adress_IN_PATH[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH" );
				}
				else {
					IO_Adress_IN_PATH[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IN_PATH%d" , CHECKING_SIDE + 1 );
				}
			}
/*
			if ( IO_Adress_IN_PATH[CHECKING_SIDE] == -99 ) { // 2003.08.01
				switch( CHECKING_SIDE ) {
				case 0 :	IO_Adress_IN_PATH[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH" );		break;
				case 1 :	IO_Adress_IN_PATH[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH2" );	break;
				case 2 :	IO_Adress_IN_PATH[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH3" );	break;
				case 3 :	IO_Adress_IN_PATH[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH4" );	break;
				}
			}
*/

			if ( IO_Adress_IN_PATH[CHECKING_SIDE] >= 0 ) {
				_dWRITE_DIGITAL( IO_Adress_IN_PATH[CHECKING_SIDE] , Path + 1 , &CommStatus );
			}

			//
			if ( IO_Adress_OUT_PATH[CHECKING_SIDE] == -99 ) { // 2011.01.26
				if ( CHECKING_SIDE == 0 ) {
					IO_Adress_OUT_PATH[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH" );
				}
				else {
					IO_Adress_OUT_PATH[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.OUT_PATH%d" , CHECKING_SIDE + 1 );
				}
			}
/*
			if ( IO_Adress_OUT_PATH[CHECKING_SIDE] == -99 ) { // 2003.08.01
				switch( CHECKING_SIDE ) {
				case 0 :	IO_Adress_OUT_PATH[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH" );	break;
				case 1 :	IO_Adress_OUT_PATH[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH2" );	break;
				case 2 :	IO_Adress_OUT_PATH[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH3" );	break;
				case 3 :	IO_Adress_OUT_PATH[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH4" );	break;
				}
			}
*/
			if ( IO_Adress_OUT_PATH[CHECKING_SIDE] >= 0 ) {
				_dWRITE_DIGITAL( IO_Adress_OUT_PATH[CHECKING_SIDE] , Path2 + 1 , &CommStatus );
			}
			return Success;
		}
	}
	return Not_Enable_Status;
}

int RUN_FUNCTION_For_START_SLOT_Select( int CHECKING_SIDE , long StartSlot ) { // Just Use for GUI Start
	int CommStatus , j;
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == 0 ) {

			//
			if ( IO_Adress_START_SLOT[CHECKING_SIDE] == -99 ) { // 2011.01.26
				if ( CHECKING_SIDE == 0 ) {
					IO_Adress_START_SLOT[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT" );
				}
				else {
					IO_Adress_START_SLOT[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.START_SLOT%d" , CHECKING_SIDE + 1 );
				}
			}
/*
			if ( IO_Adress_START_SLOT[CHECKING_SIDE] == -99 ) { // 2003.08.01
				switch( CHECKING_SIDE ) {
				case 0 :	IO_Adress_START_SLOT[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT" );	break;
				case 1 :	IO_Adress_START_SLOT[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT2" );	break;
				case 2 :	IO_Adress_START_SLOT[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT3" );	break;
				case 3 :	IO_Adress_START_SLOT[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT4" );	break;
				}
			}
*/

			if ( IO_Adress_START_SLOT[CHECKING_SIDE] >= 0 ) {
				_dWRITE_DIGITAL( IO_Adress_START_SLOT[CHECKING_SIDE] , StartSlot , &CommStatus );
				return Success;
			}
		}
	}
	return Not_Enable_Status;
}

int RUN_FUNCTION_For_END_SLOT_Select( int CHECKING_SIDE , long EndSlot ) { // Just Use for GUI Start
	int CommStatus , j;
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == 0 ) {

			//
			if ( IO_Adress_END_SLOT[CHECKING_SIDE] == -99 ) { // 2011.01.26
				if ( CHECKING_SIDE == 0 ) {
					IO_Adress_END_SLOT[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT" );
				}
				else {
					IO_Adress_END_SLOT[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.END_SLOT%d" , CHECKING_SIDE + 1 );
				}
			}

/*
			if ( IO_Adress_END_SLOT[CHECKING_SIDE] == -99 ) { // 2003.08.01
				switch( CHECKING_SIDE ) {
				case 0 :	IO_Adress_END_SLOT[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT" );	break;
				case 1 :	IO_Adress_END_SLOT[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT2" );	break;
				case 2 :	IO_Adress_END_SLOT[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT3" );	break;
				case 3 :	IO_Adress_END_SLOT[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT4" );	break;
				}
			}
*/
			if ( IO_Adress_END_SLOT[CHECKING_SIDE] >= 0 ) {
				_dWRITE_DIGITAL( IO_Adress_END_SLOT[CHECKING_SIDE] , EndSlot , &CommStatus );
				return Success;
			}
		}
	}
	return Not_Enable_Status;
}

int RUN_FUNCTION_For_LOOP_COUNT_Select( int CHECKING_SIDE , long LoopCount ) { // Just Use for GUI Start
	int CommStatus , j;
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == 0 ) {
			j = IO_READ_for_Adress_LOOP_COUNT( CHECKING_SIDE ); // 2003.08.01
			if ( j >= 0 ) {
				_dWRITE_DIGITAL( j , LoopCount , &CommStatus );
				return Success;
			}
		}
	}
	return Not_Enable_Status;
}



//
//
//
//
//
int RUN_FUNCTION_Event_START( int CHECKING_SIDE , int mode , char *data ) {
	int CommStatus , j;
	char Buffer[256];
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == CTC_IDLE ) {
			j = IO_READ_for_Adress_SCHEDULER(); // 2003.08.01
			if ( j >= 0 ) {
				if      ( mode == 0 )	sprintf( Buffer , "START%d %s"  , CHECKING_SIDE + 1 , data );
				else if ( mode == 1 )	sprintf( Buffer , "STARTP%d %s" , CHECKING_SIDE + 1 , data );
				else                    sprintf( Buffer , "STARTS%d %s" , CHECKING_SIDE + 1 , data );
				_dWRITE_FUNCTION_EVENT( j , Buffer );
			}
			return Success;
		}
	}
	return Not_Enable_Status;
}
//
int RUN_FUNCTION_Event_STOP( int CHECKING_SIDE ) {
	int CommStatus , j;
	char Buffer[16];
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == CTC_RUNNING ) {
			j = IO_READ_for_Adress_SCHEDULER(); // 2003.08.01
			if ( j >= 0 ) {
				sprintf( Buffer , "PAUSE%d" , CHECKING_SIDE + 1 );
				_dWRITE_FUNCTION_EVENT( j , Buffer );
			}
			return Success;
		}
	}
	return Not_Enable_Status;
}
//
int RUN_FUNCTION_Event_CONTINUE( int CHECKING_SIDE ) {
	int CommStatus , j;
	char Buffer[16];
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == CTC_PAUSED ) {
			j = IO_READ_for_Adress_SCHEDULER(); // 2003.08.01
			if ( j >= 0 ) {
				sprintf( Buffer , "CONTINUE%d" , CHECKING_SIDE + 1 );
				_dWRITE_FUNCTION_EVENT( j , Buffer );
			}
			return Success;
		}
	}
	return Not_Enable_Status;
}
//
int RUN_FUNCTION_Event_ABORT( int CHECKING_SIDE ) {
	int CommStatus , i , j;
	char Buffer[16];
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	else {
		i = CTC_IDLE;
	}
	if ( ( i == CTC_RUNNING ) || ( i == CTC_PAUSING ) || ( i == CTC_PAUSED ) ) {
		j = IO_READ_for_Adress_SCHEDULER(); // 2003.08.01
		if ( j >= 0 ) {
			sprintf( Buffer , "ABORT%d" , CHECKING_SIDE + 1 );
			_dWRITE_FUNCTION_EVENT( j , Buffer );
		}
		return Success;
	}
	if ( i == CTC_ABORTING ) {
		return Success;
	}
	return Not_Enable_Status;
}
//
int RUN_FUNCTION_Event_END( int CHECKING_SIDE ) {
	int CommStatus , i , j;
	char Buffer[16];
	j = IO_READ_for_Adress_MAIN_CONTROL( CHECKING_SIDE ); // 2003.08.01
	if ( j >= 0 ) {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	else {
		i = CTC_IDLE;
	}
	if ( ( i == CTC_IDLE ) || ( i == CTC_DISABLE ) || ( i == CTC_ABORTING ) ) {
		return Not_Enable_Status;
	}
	j = IO_READ_for_Adress_END_CONTROL( CHECKING_SIDE );
	if ( j >= 0 ) {
		if ( _dREAD_DIGITAL( j , &CommStatus ) == 1 ) {
			j = IO_READ_for_Adress_SCHEDULER(); // 2003.08.01
			if ( j >= 0 ) {
				sprintf( Buffer , "END%d" , CHECKING_SIDE + 1 );
				_dWRITE_FUNCTION_EVENT( j , Buffer );
			}
		}
	}
	return Success;
}
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
BOOL HANDOFF_For_RUNNING_CHECK( int ID ) {
	int CommStatus , i , j;
	j = IO_READ_for_Adress_CMH_CONTROL( ID ); // 2003.08.01
	if ( j >= 0 ) {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	else {
		i = CTC_IDLE;
	}
	if ( ( i != CTC_IDLE ) && ( i != CTC_DISABLE )	)	return TRUE;
	else												return FALSE;
}
//
//
int HANDOFF_Event_OUT( int ID ) {
	int CommStatus , i , j;
	j = IO_READ_for_Adress_CMH_CONTROL( ID ); // 2003.08.01
	if ( j >= 0 ) {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	else {
		i = CTC_IDLE;
	}
	j = IO_READ_for_Adress_HANDOFF_CM( ID ); // 2003.08.01
	//
	if ( i != CTC_IDLE ) return Not_Enable_Status;
	//	
	if ( j >= 0 ) {
		_dRUN_SET_FUNCTION( j , "OUT" );
	}
	return Success;
}
//
int HANDOFF_Event_IN( int ID ) {
	int CommStatus , i , j;
	j = IO_READ_for_Adress_CMH_CONTROL( ID ); // 2003.08.01
	if ( j >= 0 ) {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	else {
		i = CTC_IDLE;
	}
	j = IO_READ_for_Adress_HANDOFF_CM( ID ); // 2003.08.01
	//
	if ( i != CTC_IDLE ) return Not_Enable_Status;
	//	
	if ( j >= 0 ) {
		_dRUN_SET_FUNCTION( j , "IN" );
	}
	return Success;
}
//
//
int HANDOFF_Event_ABORT( int ID ) {
	int CommStatus , i , j;
	j = IO_READ_for_Adress_CMH_CONTROL( ID ); // 2003.08.01
	if ( j >= 0 ) {
		i = _dREAD_DIGITAL( j , &CommStatus );
	}
	else {
		i = CTC_IDLE;
	}
	j = IO_READ_for_Adress_HANDOFF_CM( ID ); // 2003.08.01
	//
	if ( ( i == CTC_IDLE ) || ( i == CTC_DISABLE ) ) return Not_Enable_Status;
	//	
	if ( j >= 0 ) {
		_dRUN_FUNCTION_ABORT( j );
	}
	return Success;
}
//
void Get_Job_Name( int CHECKING_SIDE , char *Data ) {
	int CommStatus;
	char buffer[100];

	//
	if ( IO_Adress_JOB_NAME[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_JOB_NAME[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME" );
		}
		else {
			IO_Adress_JOB_NAME[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.JOB_NAME%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_JOB_NAME[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_JOB_NAME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME" );	break;
		case 1 :	IO_Adress_JOB_NAME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME2" );	break;
		case 2 :	IO_Adress_JOB_NAME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME3" );	break;
		case 3 :	IO_Adress_JOB_NAME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.JOB_NAME4" );	break;
		}
	}
*/

	if ( IO_Adress_JOB_NAME[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_JOB_NAME[CHECKING_SIDE] , buffer , &CommStatus );
//		strncpy( Data , buffer , MAX_STRING_SIZE_50 ); // 2009.11.20
		strncpy( Data , buffer , MAX_STRING_SIZE_64 ); // 2009.11.20
	}
	else {
		strcpy( Data , "" );
	}
}
//
void Get_MID_Name( int CHECKING_SIDE , char *Data ) {
	int CommStatus;
	char buffer[100];

	//
	if ( IO_Adress_MID_NAME[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_MID_NAME[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME" );
		}
		else {
			IO_Adress_MID_NAME[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.MID_NAME%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_MID_NAME[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_MID_NAME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME" );	break;
		case 1 :	IO_Adress_MID_NAME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME2" );	break;
		case 2 :	IO_Adress_MID_NAME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME3" );	break;
		case 3 :	IO_Adress_MID_NAME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.MID_NAME4" );	break;
		}
	}
*/

	if ( IO_Adress_MID_NAME[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_MID_NAME[CHECKING_SIDE] , buffer , &CommStatus );
		strncpy( Data , buffer , MAX_STRING_SIZE_50 );
	}
	else {
		strcpy( Data , "" );
	}
}
//
void Get_Start_Time( int CHECKING_SIDE , BOOL remap , char *Data , int *mode , int *orgside ) {
	int CommStatus;
	char buffer[100];
	char timestr[100];
	char elseStr[100];

	*orgside = -1;
	*mode = -1;
	//
	if ( IO_Adress_START_TIME[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_START_TIME[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME" );
		}
		else {
			IO_Adress_START_TIME[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.START_TIME%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_START_TIME[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_START_TIME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME" );	break;
		case 1 :	IO_Adress_START_TIME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME2" );	break;
		case 2 :	IO_Adress_START_TIME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME3" );	break;
		case 3 :	IO_Adress_START_TIME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.START_TIME4" );	break;
		}
	}
*/
	if ( IO_Adress_START_TIME[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_START_TIME[CHECKING_SIDE] , buffer , &CommStatus );
		//
		STR_SEPERATE_CHAR( buffer , '|' , timestr , elseStr , 99 );
		//
		if ( remap ) _dWRITE_STRING( IO_Adress_START_TIME[CHECKING_SIDE] , timestr , &CommStatus );
		//
		strncpy( Data , timestr , MAX_STRING_SIZE_22 );
		//
		if ( elseStr[0] == 'S' ) {
			//
			*orgside = atoi( elseStr + 1 ) - 1;
			//
			*mode = 0;
		}
		else if ( elseStr[0] == 'L' ) {
			//
			*orgside = atoi( elseStr + 1 );
			//
			*mode = 1;
		}
		//
	}
	else {
		strcpy( Data , "" );
	}
}
//
void Get_End_Time( int CHECKING_SIDE , BOOL remap , char *Data , int *mode , int *res ) {
	int CommStatus;
	char buffer[100];
	char timestr[100];
	char elseStr[100];

	*mode = 0;
	//
	if ( IO_Adress_END_TIME[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_END_TIME[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME" );
		}
		else {
			IO_Adress_END_TIME[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.END_TIME%d" , CHECKING_SIDE + 1 );
		}
	}

/*
	if ( IO_Adress_END_TIME[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_END_TIME[0]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME" );	break;
		case 1 :	IO_Adress_END_TIME[1]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME2" );	break;
		case 2 :	IO_Adress_END_TIME[2]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME3" );	break;
		case 3 :	IO_Adress_END_TIME[3]		= _FIND_FROM_STRING( _K_S_IO , "CTC.END_TIME4" );	break;
		}
	}
*/
	if ( IO_Adress_END_TIME[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_END_TIME[CHECKING_SIDE] , buffer , &CommStatus );
		//
		STR_SEPERATE_CHAR( buffer , '|' , timestr , elseStr , 99 );
		//
		// 2012.03.20
		//
		if ( remap ) _dWRITE_STRING( IO_Adress_END_TIME[CHECKING_SIDE] , timestr , &CommStatus );
		//
		strncpy( Data , timestr , MAX_STRING_SIZE_22 );
		//
		if      ( elseStr[0] == 'U' ) {
			*mode = 1;
			*res = atoi( elseStr + 1 );
		}
		else if ( elseStr[0] == 'S' ) {
			*mode = 2;
			*res = atoi( elseStr + 1 );
		}
		//
	}
	else {
		strcpy( Data , "" );
	}
}
//
void Get_Elapsed_Time( int CHECKING_SIDE , char *Data ) {
	int CommStatus;
	char buffer[100];

	//
	if ( IO_Adress_ELAPSED_TIME[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_ELAPSED_TIME[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME" );
		}
		else {
			IO_Adress_ELAPSED_TIME[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.ELAPSED_TIME%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_ELAPSED_TIME[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_ELAPSED_TIME[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME" );	break;
		case 1 :	IO_Adress_ELAPSED_TIME[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME2" );	break;
		case 2 :	IO_Adress_ELAPSED_TIME[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME3" );	break;
		case 3 :	IO_Adress_ELAPSED_TIME[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.ELAPSED_TIME4" );	break;
		}
	}
*/
	if ( IO_Adress_ELAPSED_TIME[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_ELAPSED_TIME[CHECKING_SIDE] , buffer , &CommStatus );
		strncpy( Data , buffer , MAX_STRING_SIZE_22 );
	}
	else {
		strcpy( Data , "" );
	}
}
//
BOOL Get_FirstIn_Supply( int CHECKING_SIDE , char *Data ) {
	int CommStatus;
	char buffer[100];

	//
	if ( IO_Adress_FIRSTIN_TIME[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_FIRSTIN_TIME[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME" );
		}
		else {
			IO_Adress_FIRSTIN_TIME[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.FIRSTIN_TIME%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_FIRSTIN_TIME[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_FIRSTIN_TIME[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME" );	break;
		case 1 :	IO_Adress_FIRSTIN_TIME[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME2" );	break;
		case 2 :	IO_Adress_FIRSTIN_TIME[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME3" );	break;
		case 3 :	IO_Adress_FIRSTIN_TIME[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.FIRSTIN_TIME4" );	break;
		}
	}
*/
	if ( IO_Adress_FIRSTIN_TIME[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_FIRSTIN_TIME[CHECKING_SIDE] , buffer , &CommStatus );
		strncpy( Data , buffer , MAX_STRING_SIZE_22 );
	}
	else {
		strcpy( Data , "" );
	}
	if ( strlen( Data ) > 0 ) return TRUE;
	return FALSE;
}
//
BOOL Get_LastIn_Supply( int CHECKING_SIDE ) {
	int CommStatus;
	char Data[256];

	//
	if ( IO_Adress_LASTIN_TIME[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_LASTIN_TIME[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME" );
		}
		else {
			IO_Adress_LASTIN_TIME[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.LASTIN_TIME%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_LASTIN_TIME[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_LASTIN_TIME[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME" );		break;
		case 1 :	IO_Adress_LASTIN_TIME[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME2" );	break;
		case 2 :	IO_Adress_LASTIN_TIME[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME3" );	break;
		case 3 :	IO_Adress_LASTIN_TIME[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LASTIN_TIME4" );	break;
		}
	}
*/

	if ( IO_Adress_LASTIN_TIME[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_LASTIN_TIME[CHECKING_SIDE] , Data , &CommStatus );
	}
	else {
		strcpy( Data , "" );
	}
	if ( strlen( Data ) > 0 ) return TRUE;
	return FALSE;
}
//
void Get_Directory( int CHECKING_SIDE , char *Data ) {
	int CommStatus;
	char buffer[100];

	//
	if ( IO_Adress_DIRECTORY_PATH[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_DIRECTORY_PATH[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY" );
		}
		else {
			IO_Adress_DIRECTORY_PATH[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.LOG_DIRECTORY%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_DIRECTORY_PATH[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_DIRECTORY_PATH[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY" );	break;
		case 1 :	IO_Adress_DIRECTORY_PATH[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY2" );	break;
		case 2 :	IO_Adress_DIRECTORY_PATH[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY3" );	break;
		case 3 :	IO_Adress_DIRECTORY_PATH[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.LOG_DIRECTORY4" );	break;
		}
	}
*/
	if ( IO_Adress_DIRECTORY_PATH[CHECKING_SIDE] >= 0 ) {
		_dREAD_STRING( IO_Adress_DIRECTORY_PATH[CHECKING_SIDE] , buffer , &CommStatus );
		strncpy( Data , buffer , MAX_STRING_SIZE_100 );
	}
	else {
		strcpy( Data , "" );
	}
}
//
int Get_Wfr_Count( int CHECKING_SIDE ) {
	int CommStatus;
	//
	if ( IO_Adress_DONE_WFR_STATUS[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_DONE_WFR_STATUS[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS" );
		}
		else {
			IO_Adress_DONE_WFR_STATUS[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.DONE_WFR_STATUS%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_DONE_WFR_STATUS[CHECKING_SIDE] == -99 ) {
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_DONE_WFR_STATUS[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS" );		break;
		case 1 :	IO_Adress_DONE_WFR_STATUS[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS2" );	break;
		case 2 :	IO_Adress_DONE_WFR_STATUS[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS3" );	break;
		case 3 :	IO_Adress_DONE_WFR_STATUS[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.DONE_WFR_STATUS4" );	break;
		}
	}
*/
	if ( IO_Adress_DONE_WFR_STATUS[CHECKING_SIDE] >= 0 ) {
		return _dREAD_DIGITAL( IO_Adress_DONE_WFR_STATUS[CHECKING_SIDE] , &CommStatus );
	}
	return 0;
}
//
void Get_Lot_Recipe( int CHECKING_SIDE , int cmin , char *Data ) {
	int CommStatus;
	char buffer[100];
	if ( cmin < PM1 ) {

		//
		if ( IO_Adress_RECIPE_FILE[CHECKING_SIDE] == -99 ) { // 2011.01.26
			if ( CHECKING_SIDE == 0 ) {
				IO_Adress_RECIPE_FILE[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE" );
			}
			else {
				IO_Adress_RECIPE_FILE[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.RECIPE_FILE%d" , CHECKING_SIDE + 1 );
			}
		}
/*
		if ( IO_Adress_RECIPE_FILE[CHECKING_SIDE] == -99 ) { // 2003.08.01
			switch( CHECKING_SIDE ) {
			case 0 :	IO_Adress_RECIPE_FILE[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE" );		break;
			case 1 :	IO_Adress_RECIPE_FILE[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE2" );	break;
			case 2 :	IO_Adress_RECIPE_FILE[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE3" );	break;
			case 3 :	IO_Adress_RECIPE_FILE[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_FILE4" );	break;
			}
		}
*/
		if ( IO_Adress_RECIPE_FILE[CHECKING_SIDE] >= 0 ) {
			_dREAD_STRING( IO_Adress_RECIPE_FILE[CHECKING_SIDE] , buffer , &CommStatus );
			strncpy( Data , buffer , MAX_STRING_SIZE_50 );
		}
		else {
			strcpy( Data , "" );
		}
	}
	else { // 2010.03.10

		//
		if ( IO_Adress_RECIPE_BM_FILE[CHECKING_SIDE] == -99 ) { // 2011.01.26
			if ( CHECKING_SIDE == 0 ) {
				IO_Adress_RECIPE_BM_FILE[CHECKING_SIDE] = _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_BM_FILE" );
			}
			else {
				IO_Adress_RECIPE_BM_FILE[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.RECIPE_BM_FILE%d" , CHECKING_SIDE + 1 );
			}
		}
/*
		if ( IO_Adress_RECIPE_BM_FILE[CHECKING_SIDE] == -99 ) { // 2003.08.01
			switch( CHECKING_SIDE ) {
			case 0 :	IO_Adress_RECIPE_BM_FILE[0]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_BM_FILE" );		break;
			case 1 :	IO_Adress_RECIPE_BM_FILE[1]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_BM_FILE2" );	break;
			case 2 :	IO_Adress_RECIPE_BM_FILE[2]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_BM_FILE3" );	break;
			case 3 :	IO_Adress_RECIPE_BM_FILE[3]	= _FIND_FROM_STRING( _K_S_IO , "CTC.RECIPE_BM_FILE4" );	break;
			}
		}
*/
		if ( IO_Adress_RECIPE_BM_FILE[CHECKING_SIDE] >= 0 ) {
			_dREAD_STRING( IO_Adress_RECIPE_BM_FILE[CHECKING_SIDE] , buffer , &CommStatus );
			strncpy( Data , buffer , MAX_STRING_SIZE_50 );
		}
		else {
			strcpy( Data , "" );
		}
	}
}
//
int  Get_In_Cassette( int CHECKING_SIDE ) {
	int CommStatus;
	int Res; // 2010.03.10

	//
	if ( IO_Adress_IN_PATH[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_IN_PATH[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH" );
		}
		else {
			IO_Adress_IN_PATH[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IN_PATH%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_IN_PATH[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_IN_PATH[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH" );		break;
		case 1 :	IO_Adress_IN_PATH[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH2" );	break;
		case 2 :	IO_Adress_IN_PATH[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH3" );	break;
		case 3 :	IO_Adress_IN_PATH[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.IN_PATH4" );	break;
		}
	}
*/
	if ( IO_Adress_IN_PATH[CHECKING_SIDE] >= 0 ) {
//		return ( _dREAD_DIGITAL( IO_Adress_IN_PATH[CHECKING_SIDE] , &CommStatus ) - 1 ); // 2010.03.10
		//
		Res = _dREAD_DIGITAL( IO_Adress_IN_PATH[CHECKING_SIDE] , &CommStatus ); // 2010.03.10
		if ( Res >= PM1 ) {
			return Res - PM1 + BM1 - 1;
		}
		else {
			return Res             - 1;
		}
		//
	}
	return 0;
}
//
int  Get_Out_Cassette( int CHECKING_SIDE ) {
	int CommStatus;
	int Res; // 2010.03.10

	//
	if ( IO_Adress_OUT_PATH[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_OUT_PATH[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH" );
		}
		else {
			IO_Adress_OUT_PATH[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.OUT_PATH%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_OUT_PATH[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_OUT_PATH[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH" );	break;
		case 1 :	IO_Adress_OUT_PATH[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH2" );	break;
		case 2 :	IO_Adress_OUT_PATH[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH3" );	break;
		case 3 :	IO_Adress_OUT_PATH[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.OUT_PATH4" );	break;
		}
	}
*/
	if ( IO_Adress_OUT_PATH[CHECKING_SIDE] >= 0 ) {
//		return ( _dREAD_DIGITAL( IO_Adress_OUT_PATH[CHECKING_SIDE] , &CommStatus ) - 1 ); // 2010.03.10
		//
		Res = _dREAD_DIGITAL( IO_Adress_OUT_PATH[CHECKING_SIDE] , &CommStatus ); // 2010.03.10
		if ( Res >= PM1 ) {
			return Res - PM1 + BM1 - 1;
		}
		else {
			return Res             - 1;
		}
		//
	}
	return 0;
}
//
int	 Get_Start_Slot( int CHECKING_SIDE ) {
	int CommStatus;

	//
	if ( IO_Adress_START_SLOT[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_START_SLOT[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT" );
		}
		else {
			IO_Adress_START_SLOT[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.START_SLOT%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_START_SLOT[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_START_SLOT[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT" );	break;
		case 1 :	IO_Adress_START_SLOT[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT2" );	break;
		case 2 :	IO_Adress_START_SLOT[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT3" );	break;
		case 3 :	IO_Adress_START_SLOT[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.START_SLOT4" );	break;
		}
	}
*/
	if ( IO_Adress_START_SLOT[CHECKING_SIDE] >= 0 ) {
		return _dREAD_DIGITAL( IO_Adress_START_SLOT[CHECKING_SIDE] , &CommStatus );
	}
	return 0;
}
//
int	 Get_End_Slot( int CHECKING_SIDE ) {
	int CommStatus;

	//
	if ( IO_Adress_END_SLOT[CHECKING_SIDE] == -99 ) { // 2011.01.26
		if ( CHECKING_SIDE == 0 ) {
			IO_Adress_END_SLOT[CHECKING_SIDE] = _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT" );
		}
		else {
			IO_Adress_END_SLOT[CHECKING_SIDE] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.END_SLOT%d" , CHECKING_SIDE + 1 );
		}
	}
/*
	if ( IO_Adress_END_SLOT[CHECKING_SIDE] == -99 ) { // 2003.08.01
		switch( CHECKING_SIDE ) {
		case 0 :	IO_Adress_END_SLOT[0]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT" );	break;
		case 1 :	IO_Adress_END_SLOT[1]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT2" );	break;
		case 2 :	IO_Adress_END_SLOT[2]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT3" );	break;
		case 3 :	IO_Adress_END_SLOT[3]	= _FIND_FROM_STRING( _K_D_IO , "CTC.END_SLOT4" );	break;
		}
	}
*/
	if ( IO_Adress_END_SLOT[CHECKING_SIDE] >= 0 ) {
		return _dREAD_DIGITAL( IO_Adress_END_SLOT[CHECKING_SIDE] , &CommStatus );
	}
	return 0;
}
//==================================================================================
int Get_Enable_Side_Lot( int in , int out ) {
	if ( in == 0 ) return 0;
	if ( in == 1 ) return 1;
	if ( in == 2 ) return 2;
	if ( in == 3 ) return 3;
	return 0;
}
//==================================================================================
/*
BOOL Check_File_is_Open_Possible_Check( char *file ) {
	HFILE hFile;
	int CommStatus , j , s;
	DWORD dwSize;
	//
	if ( *file == 0 ) return TRUE;
	hFile = _lopen( file , OF_READ );
	if ( hFile == -1 ) return TRUE;
	dwSize = GetFileSize( (HANDLE) hFile , NULL );
	_lclose( hFile );
	if ( dwSize <= 3000000 ) return TRUE;
	for ( s = 0 ; s < MAX_SIDE ; s++ ) {
		j = IO_READ_for_Adress_MAIN_CONTROL( s );
		if ( j >= 0 ) {
			switch( _dREAD_DIGITAL( j , &CommStatus ) ) {
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 6 :
				return FALSE;
			}
		}
	}
	return TRUE;
}
*/