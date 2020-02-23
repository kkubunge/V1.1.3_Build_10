#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//==========================================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum {PMA, PMB , PMC , BM1_1 , BM1_2 , BM2_1, BM2_2};
enum {ISO_PMA, ISO_PMB, ISO_PMC, ISO_BM1, ISO_BM2};

//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE = FALSE;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	{ "TMC_RBLOW_CONTROL"		, _K_F_IO	,	TMC_RBLOW_CONTROL	,	0	} ,	
	{ "TMC_ISOGATE_CONTROL"  	, _K_F_IO	,	TMC_ISOGATE_CONTROL	,	0	} ,	
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	char RunStr[41] , Temp[41] , StationStr[41] , Slot[21] , TempStr[41] , MsgStr[21];
	char szCmd[64], Temp1[32];
	Module_Status msRet = SYS_SUCCESS;

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , Temp , 40 );
	STR_SEPERATE( Temp , StationStr , TempStr , 40 );
	STR_SEPERATE( TempStr , Slot , MsgStr , 20 );

	do {

		// VTM robot Maint : TM.MAINT_STATION4
		if		( STRCMP_L( StationStr , "A_PMA"   ) ) sprintf(Temp1,"A_PM1 201");
		else if ( STRCMP_L( StationStr , "A_PMB"   ) ) sprintf(Temp1,"A_PM2 201"); 
		else if ( STRCMP_L( StationStr , "A_PMC"   ) ) sprintf(Temp1,"A_PM3 201"); 
		else if	( STRCMP_L( StationStr , "B_PMA"   ) ) sprintf(Temp1,"B_PM1 201"); 
		else if ( STRCMP_L( StationStr , "B_PMB"   ) ) sprintf(Temp1,"B_PM2 201"); 
		else if ( STRCMP_L( StationStr , "B_PMC"   ) ) sprintf(Temp1,"B_PM3 201"); 		

		else if ( STRCMP_L( StationStr , "A_BM1_1" ) ) sprintf(Temp1,"A_BM1 201"); 
		else if ( STRCMP_L( StationStr , "A_BM1_2" ) ) sprintf(Temp1,"A_BM1 403"); 
		else if ( STRCMP_L( StationStr , "B_BM1_1" ) ) sprintf(Temp1,"B_BM1 201"); 
		else if ( STRCMP_L( StationStr , "B_BM1_2" ) ) sprintf(Temp1,"B_BM1 403"); 
		else if ( STRCMP_L( StationStr , "A_BM2_1" ) ) sprintf(Temp1,"A_BM2 201"); 
		else if ( STRCMP_L( StationStr , "A_BM2_2" ) ) sprintf(Temp1,"A_BM2 403"); 
		else if ( STRCMP_L( StationStr , "B_BM2_1" ) ) sprintf(Temp1,"B_BM2 201");
		else if ( STRCMP_L( StationStr , "B_BM2_2" ) ) sprintf(Temp1,"B_BM2 403");


		// Slot Valve Maint : TM.MAINT_STATION3
		else if ( STRCMP_L( StationStr , "A_PMA" ) ) sprintf(Temp1,"A_PM1");
		else if ( STRCMP_L( StationStr , "A_PMB" ) ) sprintf(Temp1,"A_PM2");
		else if ( STRCMP_L( StationStr , "A_PMC" ) ) sprintf(Temp1,"A_PM3");
		else if ( STRCMP_L( StationStr , "B_PMA" ) ) sprintf(Temp1,"B_PM1");
		else if ( STRCMP_L( StationStr , "B_PMB" ) ) sprintf(Temp1,"B_PM2");
		else if ( STRCMP_L( StationStr , "B_PMC" ) ) sprintf(Temp1,"B_PM3");

		else if ( STRCMP_L( StationStr , "A_BM1" ) ) sprintf(Temp1,"A_BM1");
		else if ( STRCMP_L( StationStr , "B_BM1" ) ) sprintf(Temp1,"B_BM1");
		else if ( STRCMP_L( StationStr , "A_BM2" ) ) sprintf(Temp1,"A_BM2");
		else if ( STRCMP_L( StationStr , "B_BM2" ) ) sprintf(Temp1,"B_BM2");

		else {
			printf( "---> TM Maint Function Error - Not Support Station %s\n" , PROGRAM_PARAMETER_READ() );
			msRet = SYS_ABORTED;
			break;
		}

		if ( STRCMP_L(RunStr , "OPEN") ) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_ISOGATE_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "CLOSE" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_ISOGATE_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "PICK" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "PLACE" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "ROTATE" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "EXTEND" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "RETRACT" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "UP" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "DOWN" )) {
			sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else if (STRCMP_L( RunStr , "AWS_INIT" )) {
				sprintf(szCmd, "%s %s", RunStr, Temp1);
			if( RUN_FUNCTION(TMC_RBLOW_CONTROL, szCmd) == SYS_ABORTED ) msRet = SYS_ABORTED;
		} else {
			printf( "---> TM Maint Function Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
			msRet = SYS_ABORTED;
		}
	} while(0);

	printf("-----> %s in TM_Maint_Func\n", szCmd);
	_LOG("-----> %s Finished in TM_Maint_Func\n", szCmd);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	//g_DynIO.InitIOList(g_pIOList, false);
	return TRUE;
}

//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	//--------------------------------------
	// 2005.10.07 for Checking System Exit
	if(no == -1)
	{
		// Deinitialize Function
		_gCLOSE_LOG();
	}
	//--------------------------------------
}

} // End of [extern "C"]
//==========================================================================================
