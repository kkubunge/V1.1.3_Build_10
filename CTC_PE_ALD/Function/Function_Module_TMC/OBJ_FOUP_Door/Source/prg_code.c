#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"

#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"	, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	//============================================================================
    { "PRMD_FOUP_WAIT_OPT"	, _K_D_IO	,	PRMD_FOUP_WAIT_OPT	,	0	} ,
	{ "TMC_FMCM1_CONTROL"	, _K_F_IO	,	TMC_FMCM1_CONTROL	,	0	} ,
	{ "TMC_FMCM2_CONTROL"	, _K_F_IO	,	TMC_FMCM2_CONTROL	,	0	} ,
	{ "TMC_FMCM3_CONTROL"	, _K_F_IO	,	TMC_FMCM3_CONTROL	,	0	} ,

	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {

	char StationStr[64] , RunStr[64] , ElseStr[64], TrStr[64];
	int  nIOStatus = -1;
	Module_Status msRet = SYS_SUCCESS;

//	if ( SIMULATION_MODE ) return SYS_SUCCESS;

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , TrStr , 63 );
	STR_SEPERATE( TrStr    , StationStr , ElseStr , 63 );

	if (0 == READ_DIGITAL(PRMD_FOUP_WAIT_OPT , &nIOStatus)) 
		return SYS_SUCCESS;

	printf("-----> Receive Data FOUP Door Command: %s, Station: %s FOUP Door Function\n", RunStr,StationStr);

	if(STRCMP_L(StationStr , "CM1"))
	{
		if(STRCMP_L(RunStr , "OPEN"))
			return RUN_FUNCTION(TMC_FMCM1_CONTROL,"DOOR_CTRL OPEN");
		else if(STRCMP_L(RunStr , "CLOSE"))
			return RUN_FUNCTION(TMC_FMCM1_CONTROL,"DOOR_CTRL CLOSE");
	}
	else if(STRCMP_L(StationStr , "CM2"))
	{
		if(STRCMP_L(RunStr , "OPEN"))
			return RUN_FUNCTION(TMC_FMCM2_CONTROL,"DOOR_CTRL OPEN");
		else if(STRCMP_L(RunStr , "CLOSE"))
			return RUN_FUNCTION(TMC_FMCM2_CONTROL,"DOOR_CTRL CLOSE");
	}
	else if(STRCMP_L(StationStr , "CM3"))
	{
		if(STRCMP_L(RunStr , "OPEN"))
			return RUN_FUNCTION(TMC_FMCM3_CONTROL,"DOOR_CTRL OPEN");
		else if(STRCMP_L(RunStr , "CLOSE"))
			return RUN_FUNCTION(TMC_FMCM3_CONTROL,"DOOR_CTRL CLOSE");
	}
	else
		return SYS_SUCCESS;

	_LOG("-----> %s %s FOUP Door Control Finished in FM FOUP Door Func", RunStr, StationStr);
	return msRet;
}
//

BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	return TRUE;
}
