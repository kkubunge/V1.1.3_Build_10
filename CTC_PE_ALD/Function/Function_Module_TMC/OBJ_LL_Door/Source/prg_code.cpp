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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int ALARM_START_POINTER = 1001; // BM1 : 1001 , BM2 : 2001
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		ALARM_DOOR					3
//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE = FALSE;
char ModuleName[256];

enum { enMODE_CLOSE, enMODE_OPEN };
enum { enDOOR_UNKNOWN, enDOOR_OPEN, enDOOR_CLOSE, enDOOR_ERROR };

#define	MAX_BM_UNLOAD_WAFER		2
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"	, _K_D_IO	,	IO_Driver_Loding ,	0	} ,
	{ "FC_$2_DOOR_CTRL"		, _K_D_IO	,	CONTROL			 ,	0	} ,
	{ "PRMD_$2_DOOR_TMO"	, _K_D_IO	,	CASSETTE_TIMEOUT ,	0	} ,
	{ "$2.DoorLXO"			, _K_D_IO	,	LL_DoorLXO		 ,	0	} ,
	{ "$2.DoorRXO"			, _K_D_IO	,	LL_DoorRXO		 ,	0	} ,
	{ "$2.DoorLXI"			, _K_D_IO	,	LL_DoorLXI		 ,	0	} ,
	{ "$2.DoorRXI"			, _K_D_IO	,	LL_DoorRXI		 ,	0	} ,

	//2013.03.14 MAHA BM Wafer Slide Sensor Type
	{ "BM.LInWaferSlideDI"	, _K_D_IO	,	LFrontSlideDI	 ,	0	} ,
	{ "BM.RInWaferSlideDI"	, _K_D_IO	,	RFrontSlideDI	 ,	0	} ,

	//2007.11.16
	{ "$2.ATMVACSensorXI"	, _K_D_IO	,	LL_ATMVACSensorXI,	0	} ,

	{ "CTC_EVENT_LOG_SVR"	, _K_F_IO	,	CTC_EVENT_LOG_SVR,	0	},	// 2013.11.15

	{ "CTC.$2_Wafer_Status"		, _K_D_IO	,	CTC_BM_Wafer_Status,	0	} ,		//... Unload Wafer
	{ "CTC.$2_Wafer_Status2"	, _K_D_IO	,	CTC_BM_Wafer_Status2,	0	} ,		//... Unload Wafer
	//... 2016.04.16
	{ "M_SVlv_ParmSaveDM"	, _K_D_IO	,	M_SVlv_ParmSaveDM,		0	} ,	
	{ "M_Door_StartStopDM"	, _K_D_IO	,	M_Door_StartStopDM,		0	} ,
	{ "M_SVlv_StartStopDM"	, _K_D_IO	,	M_SVlv_StartStopDM,		0	} ,
	{ "M_Door_CycTgtMdlDM"	, _K_D_IO	,	M_Door_CycTgtMdlDM,		0	} ,	      
	{ "M_Door_CycCurCntAM"	, _K_A_IO	,	M_Door_CycCurCntAM,		0	} ,      
	{ "M_Door_CycTgtCntAM"	, _K_A_IO	,	M_Door_CycTgtCntAM,		0	} ,       
	{ "$2_Door_OPTm_AM"		, _K_A_IO	,	BM_Door_OPTm_AM,		0	} ,	
	{ "M_Door$2_OP_MinTmAM" , _K_A_IO	,	M_DoorBM_OP_MinTmAM,	0	} ,	 	    
	{ "M_Door$2_OP_MaxTmAM" , _K_A_IO	,	M_DoorBM_OP_MaxTmAM,	0	} ,				
	{ "M_Door$2_OP_AvgTmAM" , _K_A_IO	,	M_DoorBM_OP_AvgTmAM,	0	} ,		    
	{ "$2_Door_CLTm_AM"		, _K_A_IO	,	BM_Door_CLTm_AM,		0	} ,	                
	{ "M_Door$2_CL_MinTmAM" , _K_A_IO	,	M_DoorBM_CL_MinTmAM,	0	} ,			
	{ "M_Door$2_CL_MaxTmAM" , _K_A_IO	,	M_DoorBM_CL_MaxTmAM,	0	} ,			
	{ "M_Door$2_CL_AvgTmAM" , _K_A_IO	,	M_DoorBM_CL_AvgTmAM,	0	} ,		
	{ "CTC.AbortControl"	, _K_D_IO	,	CTC_AbortControl,	    0	} ,
	//...
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2005.12.20
#include   "DynamicIO.h"
CDynamicIO g_DynIO;

//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	TMSIM_BM1,
	TMSIM_BM2,
} IO_INDEX_ENUM;

//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"TMSIM_BM1",				_K_F_IO, 0},
	{"TMSIM_BM2",				_K_F_IO, 0},
	""
};
int g_nTMSIM_BM = TMSIM_BM1;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.11.15
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);	
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

// 2013.11.15
void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szData)
{
	char szEventID[20];

	do {
		if(STRCMP_L(szRunStr , "OPEN")) strcpy(szEventID, "DoorOpen");
		else if(STRCMP_L(szRunStr , "CLOSE")) strcpy(szEventID, "DoorClose");
		else
		{
			printf("%s %s\n", szModule, szRunStr);
			break;
		}

		_MLOG("FNC %s %s %s %s", szModule, szEventID, szStatus, szData);
	} while(0);
}
//////////////////////////////////////////////////////////////////////////
BOOL AbsentUnloadWafer()
{
	BOOL	bRet = TRUE;
	int		nIOStatus;

	for(int i = 0; i < MAX_BM_UNLOAD_WAFER; i++)
	{
		if(0 < READ_DIGITAL(CTC_BM_Wafer_Status + i, &nIOStatus))
		{	//... Detect Unload Wafer
			bRet = FALSE;
			break;
		}
	}

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status DOOR_LL(int Data) {
	int CommStatus , AlmResult;
	int nRetryCount = 0;
	char szRunStr[40] = "";			// 2013.11.15
	//... 2016.04.16
	double dbEndTime = 0;

	//... Pre-Check Set & Status for Upgrade UPEH
	if(enMODE_CLOSE == Data)
	{
		if( (READ_DIGITAL(LL_DoorLXI, &CommStatus) == enDOOR_CLOSE) &&
			(READ_DIGITAL(LL_DoorRXI, &CommStatus) == enDOOR_CLOSE))
		{	//... Check & Completed
			return SYS_SUCCESS;
		}
	}
	else if(enMODE_OPEN == Data)
	{
		if( (READ_DIGITAL(LL_DoorLXI, &CommStatus) == enDOOR_OPEN) &&
			(READ_DIGITAL(LL_DoorRXI, &CommStatus) == enDOOR_OPEN))
		{	//... Check & Completed
			return SYS_SUCCESS;
		}
	}

	if(enMODE_CLOSE == Data) 
	{
		//2010.12.16 LL Front Slide Sensor check retry when close door	
		for (nRetryCount = 0 ; nRetryCount < 5 ; nRetryCount++) 
		{
			if (1 == READ_DIGITAL(LFrontSlideDI , &CommStatus) && 
				1 == READ_DIGITAL(RFrontSlideDI , &CommStatus)) {
				break;
			}
			else	WAIT_SECONDS(1);
		}
	}

    while (1) 
	{
		//2007.11.16 Pressure Switch Check Before Door Open
		if(enMODE_OPEN == Data && 1 != READ_DIGITAL(LL_ATMVACSensorXI , &CommStatus)) 
		{
			AlmResult = ALARM_MANAGE(ALARM_START_POINTER+3);
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			else                                continue;
		} 
		else break;
	}
	//End
	//////////////////////////////////////////////////////////////////////////
	//... 2016.04.16 Start to Calculate Door Open/Close Time 
	S_TIMER_READY();
	//////////////////////////////////////////////////////////////////////////
	while (1) 
	{
		//... Set Door IO
		switch( Data ) {
		case enMODE_CLOSE : // Close
			if( (READ_DIGITAL(LL_DoorLXI, &CommStatus) == enDOOR_CLOSE) &&
				(READ_DIGITAL(LL_DoorRXI, &CommStatus) == enDOOR_CLOSE))
			{	//... Check & Completed
				return SYS_SUCCESS;
			}
			
			// 2013.11.15
			strcpy(szRunStr, "CLOSE");
			LogEvent(ModuleName, szRunStr, "Start", "");

			//... Set Door Close
			WRITE_DIGITAL(LL_DoorLXO, enDOOR_CLOSE, &CommStatus);
			WRITE_DIGITAL(LL_DoorRXO, enDOOR_CLOSE, &CommStatus);
			break;
		case enMODE_OPEN : // Open
			if( (READ_DIGITAL(LL_DoorLXI, &CommStatus) == enDOOR_OPEN) &&
				(READ_DIGITAL(LL_DoorRXI, &CommStatus) == enDOOR_OPEN))
			{	//... Check & Completed
				return SYS_SUCCESS;
			}

			// 2013.11.15
			strcpy(szRunStr, "OPEN");
			LogEvent(ModuleName, szRunStr, "Start", "");

			WRITE_DIGITAL(LL_DoorLXO, enDOOR_OPEN, &CommStatus);
			WRITE_DIGITAL(LL_DoorRXO, enDOOR_OPEN, &CommStatus);
			break;
		}
		
		//... Check Timeout
		S_TIMER_READY();
		while(1) 
		{
			if(!WAIT_SECONDS(0.1))			return SYS_ABORTED;

			//... Check Completed
			switch( Data ) {
			case enMODE_CLOSE : // Close
				if( (READ_DIGITAL(LL_DoorLXI, &CommStatus) == enDOOR_CLOSE) &&
					(READ_DIGITAL(LL_DoorRXI, &CommStatus) == enDOOR_CLOSE)) 
				{
					LogEvent(ModuleName, szRunStr, "End", "");	// 2014.7.20
					//... 2016.04.16
					dbEndTime = S_TIMER_ELAPSED();
					WRITE_ANALOG(BM_Door_CLTm_AM,dbEndTime,&CommStatus);
					return SYS_SUCCESS;
				}
				break;
			case enMODE_OPEN : // Open
				if( (READ_DIGITAL(LL_DoorLXI, &CommStatus) == enDOOR_OPEN) &&
					(READ_DIGITAL(LL_DoorRXI, &CommStatus) == enDOOR_OPEN)) 
				{
					LogEvent(ModuleName, szRunStr, "End", "");	// 2014.7.20
					//... 2016.04.16
					dbEndTime = S_TIMER_ELAPSED();
					WRITE_ANALOG(BM_Door_OPTm_AM,dbEndTime,&CommStatus);
					return SYS_SUCCESS;
				}
				break;
			}
			
			//... Check Timeout
			if(S_TIMER_CHECK(1, (double) READ_DIGITAL(CASSETTE_TIMEOUT, &CommStatus)))
			{
				AlmResult = ALARM_MANAGE(ALARM_DOOR + ALARM_START_POINTER);				
				if     (AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
				else if(AlmResult != ALM_RETRY  ) return SYS_ABORTED;
				else
				{	//... Case. Retry
					S_TIMER_READY();
					break;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//... 2016.04.16 Stop to Calculate Door Open/Close Time 
	//dbEndTime = S_TIMER_ELAPSED();

	//if(Data == enMODE_OPEN)WRITE_ANALOG(BM_Door_OPTm_AM,dbEndTime,&CommStatus);
	//else if(Data == enMODE_CLOSE)WRITE_ANALOG(BM_Door_CLTm_AM,dbEndTime,&CommStatus);
	//else
	//{
		//WRITE_ANALOG(BM_Door_OPTm_AM,0,&CommStatus);
		//WRITE_ANALOG(BM_Door_CLTm_AM,0,&CommStatus);
	//}
	//////////////////////////////////////////////////////////////////////////

	LogEvent(ModuleName, szRunStr, "End", "");	// 2013.11.15

	return SYS_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
Module_Status DoorCycleTest()
{
	Module_Status msRet = SYS_ABORTED;

	int		nIOStatus	= 0;
	int		nTargetCnt	= 0;
	int		nCurrentCnt	= 0;
	int		i			= 0;
	int		nStation	= 5;

	double	dbOpenSumTime = 0;
	double	dbOpenMinTime = 0;
	double	dbOpenMaxTime = 0;

	double	dbCloseSumTime = 0;
	double	dbCloseMinTime = 0;
	double	dbCloseMaxTime = 0;

	double	dbCurTime	= 0;
	double	dbAvrTime	= 0;

	enum { eBM1_DOOR , eBM2_DOOR };
	enum { eOFFON_OFF, eOFFON_ON };
	enum { eRESULT_NONE, eRESULT_ERROR, eRESULT_OK };
	enum { ABC_IDLE, ABC_ABORT, ABC_ABORTING };

	//////////////////////////////////////////////////////////////////////////
	//... Check Target Cycle Count
	nTargetCnt = (int) READ_ANALOG(M_Door_CycTgtCntAM, &nIOStatus);
	if(nTargetCnt < 1)		return SYS_SUCCESS;

	//... Init Result Status
	nStation = READ_DIGITAL(M_Door_CycTgtMdlDM, &nIOStatus);

	if(nStation < eBM1_DOOR) return SYS_SUCCESS;

	if(/*ON*/ 1 == READ_DIGITAL(M_SVlv_StartStopDM, &nIOStatus))return SYS_SUCCESS;

	//////////////////////////////////////////////////////////////////////////
	//... Slit Valve CLOSE Before Test
	msRet = DOOR_LL(enMODE_CLOSE);

	//////////////////////////////////////////////////////////////////////////
	//... Run Valve Verification	
	do 
	{
		//... Check Cycle Count
		nCurrentCnt++;
		if(nCurrentCnt > nTargetCnt)
		{
			msRet = SYS_SUCCESS;
			break;		//... Completed
		}

		//... Update Current Count
		WRITE_ANALOG(M_Door_CycCurCntAM, nCurrentCnt, &nIOStatus);


		//////////////////////////////////////////////////////////////////////////
		//... Check Interlock
		if(ABC_IDLE != READ_DIGITAL(CTC_AbortControl, &nIOStatus)) break;

		if(/*ON*/ 1 == READ_DIGITAL(M_SVlv_StartStopDM, &nIOStatus)) break;

		//////////////////////////////////////////////////////////////////////////
		//... Set Valve OPEN
		msRet = DOOR_LL(enMODE_OPEN);
	
		if(msRet != SYS_SUCCESS)	break;
		
		if(! WAIT_SECONDS(1))		break;

		//////////////////////////////////////////////////////////////////////////
		//... Update OPEN Min/Max Time
		dbCurTime = READ_ANALOG(BM_Door_OPTm_AM, &nIOStatus);
		if(dbCurTime < dbOpenMinTime || nCurrentCnt == 1)
		{
			dbOpenMinTime = dbCurTime;
			WRITE_ANALOG(M_DoorBM_OP_MinTmAM, dbCurTime, &nIOStatus);
		}
		if(dbCurTime > dbOpenMaxTime || nCurrentCnt == 1)
		{
			dbOpenMaxTime = dbCurTime;
			WRITE_ANALOG(M_DoorBM_OP_MaxTmAM, dbCurTime, &nIOStatus);
		}
		
		//... Update OPEN Avg Time
		dbOpenSumTime = dbOpenSumTime + dbCurTime;
		if(dbOpenSumTime > 0)	dbAvrTime = dbOpenSumTime / nCurrentCnt;
		else						dbAvrTime = 0;
		WRITE_ANALOG(M_DoorBM_OP_AvgTmAM, dbAvrTime, &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		//... Set Valve CLOSE
		msRet = DOOR_LL(enMODE_CLOSE);
		
		if(msRet != SYS_SUCCESS)	break;
		
		if(! WAIT_SECONDS(1))		break;
	
		//////////////////////////////////////////////////////////////////////////
		//... Check Interlock
		if(ABC_IDLE != READ_DIGITAL(CTC_AbortControl, &nIOStatus)) break;
		
		//////////////////////////////////////////////////////////////////////////
		//... Update CLOSE Min/Max Time
		dbCurTime = READ_ANALOG(BM_Door_CLTm_AM, &nIOStatus);
		if(dbCurTime < dbCloseMinTime || nCurrentCnt == 1)
		{
			dbCloseMinTime = dbCurTime;
			WRITE_ANALOG(M_DoorBM_CL_MinTmAM, dbCurTime, &nIOStatus);
		}
		if(dbCurTime > dbCloseMaxTime || nCurrentCnt == 1)
		{
			dbCloseMaxTime = dbCurTime;
			WRITE_ANALOG(M_DoorBM_CL_MaxTmAM, dbCurTime, &nIOStatus);
		}
		
		//... Update CLOSE Avg Time
		dbCloseSumTime = dbCloseSumTime + dbCurTime;
		
		if(dbCloseSumTime > 0)	dbAvrTime = dbCloseSumTime / nCurrentCnt;
		else						dbAvrTime = 0;
		WRITE_ANALOG(M_DoorBM_CL_AvgTmAM, dbAvrTime, &nIOStatus);

	} while (1);	// Unlimited Loop

	//////////////////////////////////////////////////////////////////////////
	//... Door CLOSE
	msRet = DOOR_LL(enMODE_CLOSE);

	//... Toggle Save Parameter
	WRITE_DIGITAL(M_SVlv_ParmSaveDM, eOFFON_ON, &nIOStatus);

	return msRet; 
}
//------------------------------------------------------------------------------------------
//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	int	CommStatus = 0;
	char SlotStr[21] , RunStr[21];
	char *Ed;
	char szCmd[80];
	Module_Status msRet = SYS_SUCCESS;

	//... Init String Variable
	memset(SlotStr, 0, sizeof(SlotStr));
	memset(RunStr,	0, sizeof(RunStr));
	memset(szCmd,	0, sizeof(szCmd));

	//... Read Parameter
	Ed = PROGRAM_PARAMETER_READ();
	STR_SEPERATE(Ed, RunStr, SlotStr, 20);

	//... Interlock For Door Cycle Test 
	WRITE_DIGITAL(M_Door_StartStopDM,1,&CommStatus);

	// 2005.12.12
	if ( SIMULATION_MODE )
	{
		if(strcmp(RunStr , "CLOSE") == 0)
		{
			if ((READ_DIGITAL( LL_DoorLXI , &CommStatus ) == 2) && (READ_DIGITAL( LL_DoorRXI , &CommStatus ) == 2)) {
				return SYS_SUCCESS;
			}
		}
		else if(strcmp(RunStr , "OPEN") == 0)
		{
			if ((READ_DIGITAL( LL_DoorLXI , &CommStatus ) == 1) && (READ_DIGITAL( LL_DoorRXI , &CommStatus ) == 1)) {
					return SYS_SUCCESS;
			}
		}

		_LOG("-----> '%s %s' Started in LL_Door_Control(Sim)", ModuleName, Ed);
		LogEvent(ModuleName, RunStr, "Start", "");	// 2013.11.15

		sprintf(szCmd,"%s DOOR %s", ModuleName, RunStr);
		if(SYS_SUCCESS != g_DynIO.dRUN_FUNCTION(g_nTMSIM_BM, szCmd)) msRet = SYS_ABORTED;

		LogEvent(ModuleName, RunStr, "End", "");	// 2013.11.15
		_LOG("-----> '%s %s' BM1 Finished in LL_Door_Control(Sim)",ModuleName, Ed);
		return msRet;
	}

	//... Check Control Option
	if(READ_DIGITAL(CONTROL, &CommStatus) == 1)
	{
		//... Interlock For Door Cycle Test 
		WRITE_DIGITAL(M_Door_StartStopDM,0,&CommStatus);
		return SYS_SUCCESS;
	}

	//... Run Parameter
	_LOG("-----> '%s %s' Started in LL_Door_Control", ModuleName, Ed);
	
	if     (STRCMP_L(RunStr, "CLOSE"))		msRet = DOOR_LL(enMODE_CLOSE);
	else if(STRCMP_L(RunStr, "OPEN"))		msRet = DOOR_LL(enMODE_OPEN);
	else if(STRCMP_L(RunStr, "POST_SEND_CLOSE"))
	{	//... Add Command in Only FM_Post_Send for Upgrade UPEH 
		if(AbsentUnloadWafer())				msRet = DOOR_LL(enMODE_CLOSE);
		else								msRet = SYS_SUCCESS;		//... Skip Door Close
	}
	else if(STRCMP_L(RunStr, "CYCLE_TEST"))msRet = DoorCycleTest(); //...2016.04.16 Cycle Test
	else
	{
		printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
		//... Interlock For Door Cycle Test 
		WRITE_DIGITAL(M_Door_StartStopDM,0,&CommStatus);
		return SYS_ABORTED;
	}

	_LOG("-----> '%s %s' Finished in LL_Door_Control", ModuleName, Ed);

	//... Interlock For Door Cycle Test 
	WRITE_DIGITAL(M_Door_StartStopDM,0,&CommStatus);

	return msRet;
}
//
//
//  VCE4 CM1 LLA 401
//
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char Str1[21] , Str2[21] , Str3[21] , Str4[21] , Str5[21] , Str6[21];
	int	Dl;
	char *Ed;

	Ed = PROGRAM_ARGUMENT_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	//...
	STR_SEPERATE( Ed , Str1 , Str2 , 20 );
	REPLACE_CHAR_SET( 0 , Str1 );
	//...
	STR_SEPERATE( Str2 , Str3 , Str4 , 20 );
	REPLACE_CHAR_SET( 1 , Str3 );
	//...
	STR_SEPERATE( Str4 , Str5 , Str6 , 20 );
	REPLACE_CHAR_SET( 2 , Str5 );
	//...
	sprintf( ModuleName , "%s" , Str5 );
	ALARM_START_POINTER = atoi( Str6 );

	// printf("-----> LLDoorControl Module Name : %s\n", ModuleName);
	if(strcmp(ModuleName, "BM1") == 0) g_nTMSIM_BM = TMSIM_BM1;
	else g_nTMSIM_BM = TMSIM_BM2;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	g_DynIO.InitIOList(g_pIOList, false);
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
