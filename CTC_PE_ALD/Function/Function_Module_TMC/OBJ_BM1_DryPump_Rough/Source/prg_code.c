#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		ALARM_COMM_ERROR			1341
#define		ALARM_PUMP_CTRL_TMO			1342
#define		ALARM_PUMP_WARN				1343
#define		ALARM_PUMP_ERROR			1344

//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE			= FALSE;

enum	{ Stopped		,	Running	};
enum	{ WARNOK_OK		,	WARNOK_WARN	};
enum	{ ERROK_OK		,	ERROK_ERR };
enum	{ RUN = 1		,	STOP	};
enum	{ OFF			,   ON  };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"	, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	
	{ "FC_BM1_RPUMP_CTRL"	, _K_D_IO	,	FC_RPUMP_CTL		,	0	} ,
	{ "PRMD_BM1_RPUMP_TMO"	, _K_D_IO	,	PRM_PUMP_CTL_TMO	,	0	} ,
	
	{ "BM1.DPSetXO"			, _K_D_IO	,	Rough_DP_Set		,	0	} ,

	{ "BM1.DPStsDI"			, _K_D_IO	,	Rough_DP_Status		,	0	} ,
	{ "BM1.DPAlarmStsDI"	, _K_D_IO	,	Rough_DP_Alarm		,	0	} ,
	{ "BM1.DPWarnStsDI"		, _K_D_IO	,	Rough_DP_Warn		,	0	} ,
	
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
double READ_ANALOG_TRUE( int io , int *CommStatus ) {
	double Data;
	
	Data = READ_ANALOG( io , CommStatus );
	if ( *CommStatus ) return Data;
	_sleep( 500 );
	Data = READ_ANALOG( io , CommStatus );
	if ( *CommStatus ) return Data;
	_sleep(500);
	Data = READ_ANALOG( io , CommStatus );
	if ( *CommStatus ) return Data;
	
	return Data;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PumpReady() {
	int CommStatus , AlmResult;
	
	while ( TRUE ) { 
		//Check DP Alarm Status
		if ( READ_DIGITAL ( Rough_DP_Alarm, &CommStatus ) == ERROK_OK ) break;
		else {
			AlmResult = ALARM_MANAGE( ALARM_PUMP_ERROR );
			if		( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult == ALM_ABORT  ) return SYS_ABORTED;
			else if ( AlmResult == ALM_RETRY  ) continue;
			else {
				printf("Wrong AlmResult[%d].\n", AlmResult);
				return SYS_ABORTED;
			}
		}
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RunCheck(int *Status ) {
	int CommStatus , PumpSNS;

	PumpSNS	= READ_DIGITAL( Rough_DP_Status , &CommStatus );
    if ( PumpSNS == Running ) *Status = ON;
	else if ( PumpSNS == Stopped ) *Status = OFF;
	else {
		printf("Wrong Pump Status[%d].\n", PumpSNS);
		return SYS_ABORTED;
	}
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PumpControl( int Control , BOOL Check ) {
	int Status , CommStatus , AlmResult , Timer;
	BOOL bAlmRetry = FALSE;

	printf( "===>Roughline Dry Pump Off/On Control : %d\n" , Control );

	while ( TRUE ) {
		if ( RunCheck( &Status ) == SYS_ABORTED ) return SYS_ABORTED;
		if( Control == ON ) { //Check Pump Running Status.
			if( Status != Control ) WRITE_DIGITAL( Rough_DP_Set , RUN , &CommStatus );
			else printf("Rough Dry Pump is Operating.\n");
		}
		else { //Check Pump Stopped Status.
			if( Status != Control ) WRITE_DIGITAL( Rough_DP_Set , STOP , &CommStatus );
			else printf("Rough Dry Pump is Stopped.\n");
		}
		
		if ( !Check ) break;
		Timer = 0;
		while ( TRUE ) {
			if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
			if ( RunCheck( &Status ) == SYS_ABORTED ) return SYS_ABORTED;

			//Check Pump ON or OFF
			if ( Status == Control ) { 
				if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
				return SYS_SUCCESS;
			}
			else if ( Timer > READ_DIGITAL( PRM_PUMP_CTL_TMO , &CommStatus ) ){
				printf("Command Control - %d\n", Control);
				AlmResult = ALARM_MANAGE( ALARM_PUMP_CTRL_TMO );
				if		( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
				else if ( AlmResult == ALM_ABORT  ) return SYS_ABORTED;
				else if ( AlmResult == ALM_RETRY  ) {
					bAlmRetry = TRUE;
					break;
				}
				else {
					printf("Wrong AlmResult[%d].\n", AlmResult);
					return SYS_ABORTED;
				}
			}
			else Timer++;
		}

		//case of alarm retry
		if (bAlmRetry) {
			bAlmRetry = FALSE;
			continue;
		}
	}
    return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	Dl;
	unsigned char *Ed;

	if ( READ_DIGITAL( FC_RPUMP_CTL , &Dl ) == 1 ) return SYS_SUCCESS;

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if		( STRCMP_L( Ed , "ON"        ) ) return( PumpControl( ON  , FALSE ) );
	else if ( STRCMP_L( Ed , "OFF"       ) ) return( PumpControl( OFF , FALSE ) );
	else if	( STRCMP_L( Ed , "ON_CHECK"  ) ) return( PumpControl( ON  , TRUE  ) );
	else if ( STRCMP_L( Ed , "OFF_CHECK" ) ) return( PumpControl( OFF , TRUE  ) );
	else if ( STRCMP_L( Ed , "READY"     ) ) return( PumpReady() );
	else {
		printf( "===> Roughline Pump Control Error - Not Support Code %s\n" , Ed );
		return SYS_ABORTED;
	}

	return SYS_ABORTED;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
