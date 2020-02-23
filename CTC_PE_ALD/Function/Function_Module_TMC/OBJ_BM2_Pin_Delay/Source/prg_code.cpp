#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]

//Version History//////////////////////////////////////////////////////////////////////////
//Date : 2008.03.18
//       Modified from Wuxi Bluetain
//       Motion Control 에서 Fix Type으로 변경으로 인하여 Cool Plate의 Pin추가됨
//       아래와 같이 Pin Control Function추가 후 Bluetain Type으로 Left / Right Control Concept으로 수정됨




//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int ALARM_START_POINTER = 2040; 
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE = FALSE;
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"      , _K_D_IO	,	IO_Driver_Loding	        ,	0	} ,
	{ "FC_BM2_PIN_CTRL"		  , _K_D_IO	,	CONTROL				    ,	0	} ,

	{ "PRMD_BM2_PIN_TMO"	  , _K_D_IO	,	CASSETTE_TIMEOUT	    ,	0	} ,
	// PRMD_BM2_PIN_TD, Pin Down Delay Time
	{ "PRMA_BM2_PIN_TD" 	  , _K_A_IO	,	LL_PINDOWN_DELAYTIME 	,	0	} ,

	{ "BM2.PinUpDownLXO"	  , _K_D_IO	,	LL_PinUpDownLXO			,	0	} ,
	{ "BM2.PinUpDownRXO"	  , _K_D_IO	,	LL_PinUpDownRXO			,	0	} ,
	{ "BM2.PinUpDownLXI"	  , _K_D_IO	,	LL_PinUpDownLXI			,	0	} ,
	{ "BM2.PinUpDownRXI"	  , _K_D_IO	,	LL_PinUpDownRXI			,	0	} ,

	{ "CTC.BM2_Wafer_Status"  , _K_D_IO	,	CTC_BM2_Wafer_Status    ,	0	} ,
	{ "CTC.BM2_Wafer_Status2" , _K_D_IO ,	CTC_BM2_Wafer_Status2   ,	0	} ,
	""
};

//------------------------------------------------------------------------------------------
// 2005.12.20
#include "DynamicIO.h"
CDynamicIO g_DynIO;
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	TMSIM_BM2,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"TMSIM_BM2",				_K_F_IO, 0},
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PIN_LL( int Data ) {
	int CommStatus , AlmResult;
	int CountTime = 0;    // LoadLock Cooling Delay Value CountTime, DelayTime 
	int DelayTime = 0;

	while (1) {
		switch (Data) {
		case 0 : // Up
			if ((READ_DIGITAL(LL_PinUpDownLXI , &CommStatus) == 1) && (READ_DIGITAL(LL_PinUpDownRXI , &CommStatus) == 1)) {
				return SYS_SUCCESS;
			}
			WRITE_DIGITAL(LL_PinUpDownLXO   , 1 , &CommStatus );
			WRITE_DIGITAL(LL_PinUpDownRXO   , 1 , &CommStatus );

			break;
		case 1 : // Down
			if ((READ_DIGITAL(LL_PinUpDownLXI , &CommStatus) == 2) && (READ_DIGITAL(LL_PinUpDownRXI , &CommStatus) == 2)) {
				return SYS_SUCCESS;
			}

			// 2005.09.07 Load Lock Cooling Delay (by wjkim)
			DelayTime = (int) READ_ANALOG(LL_PINDOWN_DELAYTIME , &CommStatus);
			do{
				WAIT_SECONDS(1);
				CountTime++;
				printf( "[Debug] BM1 -- Delay CountTime : %d, Delay SetTime : %d\n", CountTime, DelayTime);
			}while (CountTime < DelayTime);
			
			WRITE_DIGITAL(LL_PinUpDownLXO   , 2 , &CommStatus);
			WRITE_DIGITAL(LL_PinUpDownRXO   , 2 , &CommStatus);

			break;
		}

		S_TIMER_READY();
		while(1) {
			if ( !WAIT_SECONDS( 0.1 ) ) return SYS_ABORTED;
			switch (Data) {
			case 0 : // Up
				if ((READ_DIGITAL(LL_PinUpDownLXI , &CommStatus) == 1) && (READ_DIGITAL( LL_PinUpDownRXI , &CommStatus) == 1)) {
					return SYS_SUCCESS;
				}
				break;
			case 1 : // Down
				if ((READ_DIGITAL(LL_PinUpDownLXI , &CommStatus) == 2) && (READ_DIGITAL(LL_PinUpDownRXI , &CommStatus) == 2)) {
					return SYS_SUCCESS;
				}

				break;
			}

			if (S_TIMER_CHECK( 1 , (double) READ_DIGITAL(CASSETTE_TIMEOUT , &CommStatus))) {
				AlmResult = ALARM_MANAGE( ALARM_START_POINTER );
				S_TIMER_READY();
				if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
				else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
				else                                break;
			}
		}
	}
	return SYS_SUCCESS;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	CommStatus;
	char SlotStr[21] , SlotStr2[21] , temp[21] , temp1[21] , temp2[21] , RunStr[21];
	char szCmd[80];
 
//	printf("[Debug] Entered %s\n", APP_NAME);
	
	STR_SEPERATE( PROGRAM_PARAMETER_READ()	, RunStr	, temp	, 20 );
	STR_SEPERATE( temp						, SlotStr	, temp1	, 20 );
	STR_SEPERATE( temp1						, SlotStr2	, temp2	, 20 );
	if ( SIMULATION_MODE )
	{
		sprintf(szCmd,"BM2 %s", RunStr);
		if(! g_DynIO.dRUN_FUNCTION(TMSIM_BM2, szCmd)) return SYS_ABORTED;
		return SYS_SUCCESS;
	}

	if (READ_DIGITAL(CONTROL , &CommStatus ) == 1) return SYS_SUCCESS;

	if      (STRCMP_L( RunStr , "A_UP"     ) ) {
		//if (STRCMP_L(SlotStr , "1" ) && STRCMP_L(SlotStr2 , "2" )) return ( PIN_LL( 0 ) );
		//else return SYS_SUCCESS;
		return ( PIN_LL( 0 ) );
	}
	else if (STRCMP_L( RunStr , "A_DOWN"   ) ) {
		//if (STRCMP_L(SlotStr , "1" ) && STRCMP_L(SlotStr2 , "2" )) return( PIN_LL( 1 ) );
		//else return SYS_SUCCESS;
		return( PIN_LL( 1 ) );
	}
	else if (STRCMP_L( RunStr , "CHK_WFR_BEFORE_DOWN")) {
		if ((READ_DIGITAL( CTC_BM2_Wafer_Status  , &CommStatus) >= 1 ) && 
			(READ_DIGITAL( CTC_BM2_Wafer_Status  , &CommStatus) <= 25 ) ||
			(READ_DIGITAL( CTC_BM2_Wafer_Status2 , &CommStatus) >= 1 ) && 
			(READ_DIGITAL( CTC_BM2_Wafer_Status2 , &CommStatus) <= 25 )) {
			return (PIN_LL(1));
		}
		else return SYS_SUCCESS;
	}
	else if (STRCMP_L( RunStr , "UP"     )) return (PIN_LL( 0 ));
	else if (STRCMP_L( RunStr , "DOWN"   )) return (PIN_LL( 1 ));
	else {
		printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}
//	printf("[Debug] Leaved %s\n", APP_NAME);
}
//
//
//
//==========================================================================================
extern "C"
{

//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	g_DynIO.InitIOList(g_pIOList, FALSE);
	return TRUE;
}
void Screen_Nav_Info( int no ) {

	//--------------------------------------
	// 2005.10.07 for Checking System Exit
	if(no == -1)
	{
		// Deinitialize Function
		//_gCLOSE_LOG();
	}
	//--------------------------------------
}
} // End of [extern "C"]
