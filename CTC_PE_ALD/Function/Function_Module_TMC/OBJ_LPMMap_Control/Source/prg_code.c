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
BOOL SIMULATION_MODE	= FALSE;
enum { Foup_Off = 0     , Foup_On = 1};
enum { Extend   = 0     , Retract = 1};
enum { Door_Unknown = 0 , Door_Up= 1 , Door_Down = 2, Door_Error = 3 };

//... 2015.02.16 Add Change AMHS Mode to Manual for OHT Not Access
enum { eAMHS_MANUAL,	eAMHS_AUTO		};

#define	ALARM__CHANGE_AMHS_MANUAL_CM1		270
#define	ALARM__CHANGE_AMHS_MANUAL_CM2		570
#define	ALARM__CHANGE_AMHS_MANUAL_CM3		870

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding		,	0	} ,
	//============================================================================
	{ "TMC_FMCM1_CONTROL"		, _K_F_IO	,	TMC_FMCM1_CONTROL		,	0	} ,
	{ "TMC_FMCM2_CONTROL"		, _K_F_IO	,	TMC_FMCM2_CONTROL		,	0	} ,
	{ "TMC_FMCM3_CONTROL"		, _K_F_IO	,	TMC_FMCM3_CONTROL		,	0	} ,

	{ "CTC.WfrSyncConfirm"		, _K_D_IO	,	CTC_WfrSyncConfirm		,	0	} ,

	//2012,06.04 added for Manual Control Mapping Confirm : mgsong
	{ "LPMA.FOUP_PresentDI"		, _K_D_IO	,	LPMA_FOUP_PresentDI		,	0	} ,
	{ "LPMA.F_ProperlyPlaceDI"	, _K_D_IO	,	LPMA_F_ProperlyPlaceDI	,	0	} ,
	{ "LPMA.DoorUpDownXI"		, _K_D_IO	,	LPMA_DoorUpDownXI		,	0	} ,
	
	{ "LPMB.FOUP_PresentDI"		, _K_D_IO	,	LPMB_FOUP_PresentDI		,	0	} ,
	{ "LPMB.F_ProperlyPlaceDI"	, _K_D_IO	,	LPMB_F_ProperlyPlaceDI	,	0	} ,
	{ "LPMB.DoorUpDownXI"		, _K_D_IO	,	LPMB_DoorUpDownXI		,	0	} ,

	{ "LPMC.FOUP_PresentDI"		, _K_D_IO	,	LPMC_FOUP_PresentDI		,	0	} ,
	{ "LPMC.F_ProperlyPlaceDI"	, _K_D_IO	,	LPMC_F_ProperlyPlaceDI	,	0	} ,
	{ "LPMC.DoorUpDownXI"		, _K_D_IO	,	LPMC_DoorUpDownXI		,	0	} ,

	{ "TM.AtmRbRet_LPMADI"		, _K_D_IO	,	LPMARetDI				,	0	} ,
	{ "TM.AtmRbRet_LPMBDI"		, _K_D_IO	,	LPMBRetDI				,	0	} ,
	{ "TM.AtmRbRet_LPMCDI"		, _K_D_IO	,	LPMCRetDI				,	0	} ,

	//... 2015.02.16 Add Change AMHS Mode to Manual for OHT Not Access
	{ "CTC.FA_STATUS_AGV_CM1"	, _K_D_IO	,	AGV_MODE_CM1			,	0	} ,
	{ "CTC.FA_STATUS_AGV_CM2"	, _K_D_IO	,	AGV_MODE_CM2			,	0	} ,
	{ "CTC.FA_STATUS_AGV_CM3"	, _K_D_IO	,	AGV_MODE_CM3			,	0	} ,

	""
};

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {

	char RunStr[64] , TrStr[64];
	int  nIOStatus = -1;
	int nCMFoupPresent, nCMProperlyPlace, nCMDoorSts;
	int nCommStatus;
	Module_Status msRet = SYS_SUCCESS;

	if ( SIMULATION_MODE ) {
		WRITE_DIGITAL(CTC_WfrSyncConfirm , 1 , &nIOStatus);
		return SYS_SUCCESS;
	}

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , TrStr , 63 );

	//////////////////////////////////////////////////////////////////////////
	if(STRCMP_L(RunStr , "SYNC_MAP"))
	{
		if (0 == READ_DIGITAL(CTC_WfrSyncConfirm, &nCommStatus)) 
		{
			printf("-----> Receive Foup Mapping Command [%s] for Manual Transfer Wafer Sync Confirm\n", RunStr);

			//... CM1
			nCMFoupPresent   = READ_DIGITAL(LPMA_FOUP_PresentDI   , &nCommStatus);
			nCMProperlyPlace = READ_DIGITAL(LPMA_F_ProperlyPlaceDI, &nCommStatus);
			nCMDoorSts       = READ_DIGITAL(LPMA_DoorUpDownXI     , &nCommStatus);
			//
			if ( nCMFoupPresent == Foup_On && nCMProperlyPlace == Foup_On && nCMDoorSts ==  Door_Down)
			{		
				if (Retract != READ_DIGITAL(LPMARetDI , &nCommStatus)) 		return SYS_ABORTED;
				
				//... 2015.02.16 Add Change AMHS Mode to Manual for OHT Not Access
				if(eAMHS_AUTO == READ_DIGITAL(AGV_MODE_CM1, &nCommStatus))
				{
					WRITE_DIGITAL(AGV_MODE_CM1, eAMHS_MANUAL, &nCommStatus);
					ALARM_POST(ALARM__CHANGE_AMHS_MANUAL_CM1);
				}

				//... 2015.02.16 Change Manual Map NotUse Event to HOST (Protected HOST CANCEL)
				// msRet = RUN_FUNCTION(TMC_FMCM1_CONTROL ,"MAPPING CM1 IN");
				msRet = RUN_FUNCTION(TMC_FMCM1_CONTROL ,"MAPPING CM1");
			}
			if (msRet != SYS_SUCCESS)	return SYS_ABORTED;

			//... CM2
			nCMFoupPresent   = READ_DIGITAL(LPMB_FOUP_PresentDI   , &nCommStatus);
			nCMProperlyPlace = READ_DIGITAL(LPMB_F_ProperlyPlaceDI, &nCommStatus);
			nCMDoorSts       = READ_DIGITAL(LPMB_DoorUpDownXI     , &nCommStatus);
			//
			if ( nCMFoupPresent == Foup_On && nCMProperlyPlace == Foup_On && nCMDoorSts ==  Door_Down)
			{		
				if (Retract != READ_DIGITAL(LPMBRetDI , &nCommStatus)) 		return SYS_ABORTED;

				//... 2015.02.16 Add Change AMHS Mode to Manual for OHT Not Access
				if(eAMHS_AUTO == READ_DIGITAL(AGV_MODE_CM2, &nCommStatus))
				{
					WRITE_DIGITAL(AGV_MODE_CM2, eAMHS_MANUAL, &nCommStatus);
					ALARM_POST(ALARM__CHANGE_AMHS_MANUAL_CM2);
				}

				//... 2015.02.16 Change Manual Map NotUse Event to HOST (Protected HOST CANCEL)
				// msRet = RUN_FUNCTION(TMC_FMCM2_CONTROL ,"MAPPING CM2 IN");
				msRet = RUN_FUNCTION(TMC_FMCM2_CONTROL ,"MAPPING CM2");
			}
			if (msRet != SYS_SUCCESS)	return SYS_ABORTED;

			//... CM3
			nCMFoupPresent   = READ_DIGITAL(LPMC_FOUP_PresentDI   , &nCommStatus);
			nCMProperlyPlace = READ_DIGITAL(LPMC_F_ProperlyPlaceDI, &nCommStatus);
			nCMDoorSts       = READ_DIGITAL(LPMC_DoorUpDownXI     , &nCommStatus);
			//
			if ( nCMFoupPresent == Foup_On && nCMProperlyPlace == Foup_On && nCMDoorSts ==  Door_Down)
			{		
				if (Retract != READ_DIGITAL(LPMCRetDI , &nCommStatus)) 		return SYS_ABORTED;

				//... 2015.02.16 Add Change AMHS Mode to Manual for OHT Not Access
				if(eAMHS_AUTO == READ_DIGITAL(AGV_MODE_CM3, &nCommStatus))
				{
					WRITE_DIGITAL(AGV_MODE_CM3, eAMHS_MANUAL, &nCommStatus);
					ALARM_POST(ALARM__CHANGE_AMHS_MANUAL_CM3);
				}				

				//... 2015.02.16 Change Manual Map NotUse Event to HOST (Protected HOST CANCEL)
				// msRet = RUN_FUNCTION(TMC_FMCM3_CONTROL ,"MAPPING CM3 IN");
				msRet = RUN_FUNCTION(TMC_FMCM3_CONTROL ,"MAPPING CM3");
			}
			if (msRet != SYS_SUCCESS)	return SYS_ABORTED;

			if (SYS_SUCCESS == msRet)	WRITE_DIGITAL(CTC_WfrSyncConfirm , 1 , &nIOStatus);
		}
		else	printf("-----> Receive Foup Mapping Command [%s] for Manual Transfer Skipped\n", RunStr);
	}	
	else	return SYS_SUCCESS;

	return msRet;
}
//

BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else
		SIMULATION_MODE = FALSE;
	return TRUE;
}
