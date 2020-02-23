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
#define		ALARM_OCR_COMMERROR			4900
#define		ALARM_OCR_NOTREADY			4901
#define		ALARM_OCR_READ_TIMEOUT		4902
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;
enum	{ RUN };
enum	{ ON , OFF };
enum	{ MANUAL , AUTO };
enum	{ NOTREADY , READY };
enum	{ RT_DDM , RT_BC , RT_OCR };
enum	{ RV_1, RV_2, RV_3, RV_4, RV_5 };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	
	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER			,	0	} ,

	{ "CTC.FM_AL_Wafer_Status"	, _K_D_IO	,	AL_WAFER_STATUS		,	0	} ,
	{ "CTC.FM_AL_Wafer_Source"	, _K_D_IO	,	AL_WAFER_SOURCE		,	0	} ,
	
	{ "FC_OCR_CONTROL"			, _K_D_IO	,	FC_OCR_CONTROL		,	0	} ,
	{ "PRM_OCR_READ_TMO"		, _K_D_IO	,	OCR_READ_TIMEOUT	,	0	} ,
	{ "FM.OCR_DATA"				, _K_S_IO	,	OCR_DATA			,	0	} ,

	{ "WID_ReadyDI"				, _K_D_IO	,	OCR_READY			,	0	} ,
	
	{ "WID_InitDO"				, _K_D_IO	,	OCR_INIT			,	0	} ,
	{ "WID_LiveImageDO"			, _K_D_IO	,	OCR_IMAGE			,	0	} ,
	{ "WID_ResetDO"				, _K_D_IO	,	OCR_RESET			,	0	} ,

	{ "WID_RecipeTypeDO"		, _K_D_IO	,	OCR_RCPTYPE			,	0	} ,
	{ "WID_RecipeValueDO"		, _K_D_IO	,	OCR_RCPVAL			,	0	} ,

	{ "WID_LQScoreAI"			, _K_A_IO	,	OCR_SCORE			,	0	} ,
	
	{ "WID_ReadSI"				, _K_S_IO	,	OCR_READ			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status READ_DIGITAL_TRUE( int io , int *Data ) {
	int i , AlmResult , CommStatus;
	
	*Data = -1;
	while ( TRUE ) {
		for( i = 0 ; i < 3 ; i++ ) {
			*Data = READ_DIGITAL( io , &CommStatus );
			if ( !CommStatus ) {
				if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
			}
			else break;
		}

		if ( !CommStatus ) {
			*Data = -1;
			AlmResult = ALARM_MANAGE( ALARM_OCR_COMMERROR );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status READ_STRING_TRUE( int io , char *Data ) {
	int i , AlmResult , CommStatus;
	
	sprintf( Data , "" );
	while ( TRUE ) {
		for( i = 0 ; i < 3 ; i++ ) {
			READ_STRING( io , Data , &CommStatus );
			if ( !CommStatus ) {
				if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
			}
			else break;
		}

		if ( !CommStatus ) {
			sprintf( Data , "" );
			AlmResult = ALARM_MANAGE( ALARM_OCR_COMMERROR );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status OCR_Init() {
	int Data , AlmResult , CommStatus , Timer;
	if ( READ_DIGITAL( FC_OCR_CONTROL , &CommStatus ) == OFF ) return SYS_SUCCESS;
	if ( SIMULATION_MODE ) WRITE_DIGITAL( OCR_READY , READY , &CommStatus );
	while ( TRUE ) {
		Timer = 0;
		WRITE_DIGITAL( OCR_INIT , RUN , &CommStatus );
		while ( TRUE ) {
			if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
			if ( READ_DIGITAL_TRUE( OCR_READY , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( Data == READY ) break;
			if ( Timer > READ_DIGITAL( OCR_READ_TIMEOUT , &CommStatus ) ) {
				AlmResult = ALARM_MANAGE( ALARM_OCR_READ_TIMEOUT );
				if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
				else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
				break;
			}
			else Timer++;
		}
		if ( Data == READY ) break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status OCR_Read( int Mode ) {
	int Data , AlmResult , CommStatus , Port , Slot;
	char Buffer[256] , Buffer2[256];
	if ( READ_DIGITAL( FC_OCR_CONTROL , &CommStatus ) == OFF ) {
		if ( Mode == AUTO ) {
			Port = READ_DIGITAL( AL_WAFER_SOURCE , &CommStatus );
			Slot = READ_DIGITAL( AL_WAFER_STATUS , &CommStatus );
			sprintf( Buffer2 , "STS_MID_NOT_USE PORT%d|%d" , Port+1 , Slot );
			WRITE_STRING( SCHEDULER , Buffer2 , &CommStatus );
		}
		return SYS_SUCCESS;
	}
	if ( SIMULATION_MODE ) {
		WRITE_DIGITAL( OCR_READY , READY , &CommStatus );
		WRITE_STRING( OCR_READ , "OCR_DATA" , &CommStatus );
	}
	while ( TRUE ) {
		if ( READ_DIGITAL_TRUE( OCR_READY , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data != READY ) {
			AlmResult = ALARM_MANAGE( ALARM_OCR_NOTREADY );
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	WRITE_DIGITAL( OCR_RCPTYPE , RT_OCR , &CommStatus );
	WRITE_DIGITAL( OCR_RCPVAL  , RV_1   , &CommStatus );
	if ( READ_STRING_TRUE( OCR_READ , Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	WRITE_STRING( OCR_DATA , Buffer , &CommStatus );
	if ( Mode == AUTO ) {
		Port = READ_DIGITAL( AL_WAFER_SOURCE , &CommStatus );
		Slot = READ_DIGITAL( AL_WAFER_STATUS , &CommStatus );
		sprintf( Buffer2 , "STS_MID_READ_COMPLETE PORT%d|%d|%s" , Port+1 , Slot , Buffer );
		WRITE_STRING( SCHEDULER , Buffer2 , &CommStatus );
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	Dl , Status , Port , Slot , CommStatus;
	unsigned char *Ed;
	char Buffer[256];

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if		( STRCMP_L( Ed , "INIT" ) ) return( OCR_Init() );
	else if ( STRCMP_L( Ed , "READ" ) ) {
		Status = OCR_Read( AUTO );
		if ( Status == SYS_ABORTED ) {
			Port = READ_DIGITAL( AL_WAFER_SOURCE , &CommStatus );
			Slot = READ_DIGITAL( AL_WAFER_STATUS , &CommStatus );
			sprintf( Buffer , "STS_MID_READ_FAIL PORT%d|%d|%s" , Port+1 , Slot );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		return Status;
	}
	else if ( STRCMP_L( Ed , "READM") ) return( OCR_Read( MANUAL ) );
	else {
		printf( "===> OCR Read Control Error - Not Support Code %s\n" , Ed );
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
