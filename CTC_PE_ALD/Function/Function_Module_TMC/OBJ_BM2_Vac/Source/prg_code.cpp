#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//Version History//////////////////////////////////////////////////////////////////////////
//Date : 2008.03.18
//       Modified from Wuxi Bluetain
//       Motion Control and Position Check부 제거 
//       BM Motor Calibration용으로 추가된 Function제거




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
#include "TextParser.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		ALARM_S_VENT_TIMEOUT	2031
#define		ALARM_F_VENT_TIMEOUT	2032
#define		ALARM_S_PUMP_TIMEOUT	2033
#define		ALARM_F_PUMP_TIMEOUT	2034
#define		ALARM_RPUMP_TIMEOUT		2037
#define     ALARM_LEAK_RATE_ERR		2046
#define     ALARM_LEAK_AVG_ERR		2047
//------------------------------------------------------------------------------------------
typedef enum
{ 
	ULL_UNLOCK, 
	ULL_LOCK
}EUNLOCK_LOCK;

typedef enum
{ 
	OFFON_OFF, 
	OFFON_ON 
}EOFFON;

typedef enum
{ 
	ONOFF_ON,
	ONOFF_OFF 
}EONOFF;

typedef enum
{ 
	VS_ATM, 
	VS_VACUUM, 
	VS_GOATM, 
	VS_GOVAC, 
	VS_ABORT
}EVAC_STATUS;

typedef enum
{ 
	CO_CLOSE,
	CO_OPEN 
}ECLOSEOPEN;

typedef enum
{ 
	YN_YES,
	YN_NO
}EYESNO;

typedef enum
{ 
	NY_NO,
	NY_YES
}ENOYES;

typedef enum
{ 
	LS_IDLE,
	LS_RUNNING,
	LS_PAUSING,
	LS_PAUSED,
	LS_ABORTING,
	LS_DISABLE,
	LS_WAITING,
	LS_WAIT_HANDOFF
}ELOTSTATUS;

typedef enum
{
	UNKNOWN_SENSOR,
	ATM_SENSOR,
	VAC_SENSOR,
	ERROR_SENSOR
}ATMVAC_SENSOR_STATUS;

typedef enum
{
	VC_SVENT,
	VC_FVENT
}EVENT_CMD;

typedef enum
{
	UNKNOWN_DOOR,
	OPEN_DOOR,
	CLOSE_DOOR,
	ERROR_DOOR
}DOOR_STATUS;

typedef enum
{
	UNKNOWN_MT,
	LOAD_MT,
	UNLOAD_MT,
	ERROR_MT
}MT_STATUS;

// 2008.11.17 by jonghyuk (Cycle Purge)
typedef enum
{
	CYCLE_PURGE_STATE_STOP,
	CYCLE_PURGE_STATE_PUMP,
	CYCLE_PURGE_STATE_VENT,
}CYCLE_PURGE_STATE;

typedef enum
{ 
	BM_CLOSE,
	BM_OPENSLOW,
	BM_OPENFAST
}BMVALVE;

//------------------------------------------------------------------------------------------
char NAME_CHAMBER[5] = "BM2";
//------------------------------------------------------------------------------------------
struct tm		Pres_Time;
//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE = FALSE;
//------------------------------------------------------------------------------------------
//2008.04.04 Auto Leak Check History Save
FILE*   g_fLog    = NULL;
#define FILE_PATH ".\\Parameter\\LeakCheckLog_BM2.cfg"
#define MAX_HISTORY_COUNT	5
typedef struct sLog{
	char    szTime[256];
	int     nLastLeakRate;
	int     nAvgLeakRate;
}S_LOG;

S_LOG g_LeakLog[MAX_HISTORY_COUNT];
//2008.04.04

double g_dbSlowVent = 0;
double g_dbFastVent = 0;

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		    , _K_D_IO	,	IO_Driver_Loding	    ,	0	} ,
	{ "CTC_EVENT_LOG_SVR",	_K_F_IO,	CTC_EVENT_LOG_SVR,		0 } ,			// 2013.11.15
	{ "FC_BM2_VAC_CTRL"				, _K_D_IO	, FC_Vacuum_BM2				,   0   } ,

	{ "BM2.LockPumpDM"				, _K_D_IO	, LockPumpDM_BM2			,	0	} ,
	{ "BM2.VacuumStatusDM"			, _K_D_IO	, VacuumStatusDM_BM2		,	0	} ,
	{ "BM2.RoughVvXO"				, _K_D_IO	, RoughVvXO_BM2				,	0	} ,
	{ "BM2.VentVvXO"				, _K_D_IO	, VentVvXO_BM2				,	0	} ,
	{ "BM2.ATMVACSensorXI"			, _K_D_IO	, ATMVACSensorXI			,	0	} ,
	{ "BM2.CGChmPrsAI"				, _K_A_IO	, CGChmPrsAI				,	0	} ,
	{ "BM2.CGPumpPrsAI"				, _K_A_IO	, CGPumpPrsAI				,	0	} ,
	{ "BM2.DoorLXI"					, _K_D_IO	, BM_DoorLXI				,	0	} ,
	{ "BM2.DoorRXI"					, _K_D_IO	, BM_DoorRXI				,	0	} ,

//-------------------------------------------------------------------------------------
	{ "PRMA_BM2_SVENT_PRES"			, _K_A_IO	, PRMA_BM2_SVENT_PRES		,	0	} ,
	{ "PRMA_BM2_FVENT_PRES"			, _K_A_IO	, PRMA_BM2_FVENT_PRES		,	0	} ,
	{ "PRMA_BM2_SPUMP_PRES"			, _K_A_IO	, PRMA_BM2_SPUMP_PRES		,	0	} ,
	{ "PRMA_BM2_FPUMP_PRES"			, _K_A_IO	, PRMA_BM2_FPUMP_PRES		,	0	} ,
	{ "PRMA_BM2_RPUMP_PRES"			, _K_A_IO	, PRMA_BM2_RPUMP_PRES		,	0	} ,
	{ "PRMD_BM2_SVENT_TMO"			, _K_D_IO	, PRMD_BM2_SVENT_TMO		,	0	} ,
	{ "PRMD_BM2_FVENT_TMO"			, _K_D_IO	, PRMD_BM2_FVENT_TMO		,	0	} ,
	{ "PRMD_BM2_SPUMP_TMO"			, _K_D_IO	, PRMD_BM2_SPUMP_TMO		,	0	} ,
	{ "PRMD_BM2_FPUMP_TMO"			, _K_D_IO	, PRMD_BM2_FPUMP_TMO		,	0	} ,
	{ "PRMD_BM2_RPUMP_TMO"			, _K_D_IO	, PRMD_BM2_RPUMP_TMO		,	0	} ,
	{ "PRMD_BM2_OVENT_TIME"			, _K_D_IO	, PRMD_BM2_OVENT_TIME		,	0	} ,
	{ "PRMA_BM2_VENT_TD" 			, _K_A_IO	, LL_VENT_DELAYTIME		 	,	0	} ,
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
	{ "BM2.Leak_Test_Number"		, _K_D_IO	, BM2_Leak_Test_Number		,	0	} ,
	{ "BM2.Leak_Remain_Time"		, _K_D_IO	, BM2_Leak_Test_Time		,	0	} ,
	{ "BM2.Leak_End"				, _K_D_IO	, BM2_Leak_End				,	0	} ,

	{ "BM2.Sys.Year"				, _K_D_IO	, BM2_Year 					,	0	} ,
	{ "BM2.Sys.Month"				, _K_D_IO	, BM2_Month 				,	0	} ,
	{ "BM2.Sys.Day"					, _K_D_IO	, BM2_Day 					,	0	} ,
	{ "BM2.Sys.Hour"				, _K_D_IO	, BM2_Hour 					,	0	} ,
	{ "BM2.Sys.Min"					, _K_D_IO	, BM2_Min 					,	0	} ,
	{ "BM2.Sys.Sec"					, _K_D_IO	, BM2_Sec 					,	0	} ,
	
	{ "BM2.Leak_Rate1"				, _K_A_IO	, BM2_Leak_Rate1			,	0	} ,
	{ "BM2.Leak_Rate2"				, _K_A_IO	, BM2_Leak_Rate2			,	0	} ,
	{ "BM2.Leak_Rate3"				, _K_A_IO	, BM2_Leak_Rate3			,	0	} ,
	{ "BM2.Leak_Rate4"				, _K_A_IO	, BM2_Leak_Rate4			,	0	} ,
	{ "BM2.Leak_Rate5"				, _K_A_IO	, BM2_Leak_Rate5			,	0	} ,
	{ "BM2.Leak_Rate6"				, _K_A_IO	, BM2_Leak_Rate6			,	0	} ,
	{ "BM2.Leak_Rate7"				, _K_A_IO	, BM2_Leak_Rate7			,	0	} ,
	{ "BM2.Leak_Rate8"				, _K_A_IO	, BM2_Leak_Rate8			,	0	} ,
	{ "BM2.Leak_Rate9"				, _K_A_IO	, BM2_Leak_Rate9			,	0	} ,
	{ "BM2.Leak_Rate10"				, _K_A_IO	, BM2_Leak_Rate10			,	0	} ,

	{ "BM2.Leak_LastRate"			, _K_A_IO	, BM2_Leak_LastRate			,	0	} ,

	{ "BM2.Leak_PumpingTime_H"		, _K_D_IO	, BM2_Leak_PumpingTime_H	,	0	} ,
	{ "BM2.Leak_PumpingTime_M"		, _K_D_IO	, BM2_Leak_PumpingTime_M	,	0	} ,
	{ "BM2.Leak_WaitingTime_H"		, _K_D_IO	, BM2_Leak_WaitingTime_H	,	0	} ,
	{ "BM2.Leak_WaitingTime_M"		, _K_D_IO	, BM2_Leak_WaitingTime_M	,	0	} ,
	{ "FC_BM_VENT_TYPE"				, _K_D_IO	, BM_VentType				,	0	} ,

	{ "BM2.LeakChk_Message"			, _K_S_IO	, BM2_LeakChk_Message		,	0	} ,
	{ "BM2.Leak_PumpRemain_Time"	, _K_S_IO	, BM2_Leak_PumpRemain_Time	,	0	} ,
	{ "BM2.Leak_WaitRemain_Time"	, _K_S_IO	, BM2_Leak_WaitRemain_Time	,	0	} ,

    { "BM2.pcCHM.LeakAve"           , _K_A_IO   , BM2_PC_LEAKAVE			,	0   } ,	
    { "BM2.psLKCHK.LastTIme"        , _K_S_IO   , BM2_PC_LKCHK_LASTTIME		,	0   } ,
    { "vBM2.Logging"                , _K_D_IO   , BM2_LOGGING				,	0   } ,
	{ "CTC.MAIN_CONTROL"			, _K_D_IO	, LOT_CONTROL			    ,	0	} ,
	{ "CTC.MAIN_CONTROL2"			, _K_D_IO	, LOT2_CONTROL		     	,	0	} ,
	{ "CTC.MAIN_CONTROL3"			, _K_D_IO	, LOT3_CONTROL		        ,	0	} ,
	{ "TMC_BM2_RPUMP_CONTROL"		, _K_F_IO	, BM2_PUMP_CONTROL			,	0	} ,
	{ "TMC_BM2_DOOR_CONTROL"		, _K_F_IO	, DOOR_CONTROL		        ,	0	} ,
	{ "TMC_BM2_OVERPRESSURE"		, _K_F_IO	, TMC_BM2_OVERPRESSURE		,   0   } ,

	//2009.06.09
	{ "TMC_ISOGATE_CONTROL"		    , _K_F_IO	, TMC_ISOGATE_CONTROL		,   0   } ,
	
	//Auto Leak Check Associated 2008.04.04
	{ "CTC.AutoLeakCheckSts"        , _K_D_IO	,	LeakCheckSts		    ,	0	} ,
	{ "PRMD_BM2_ALC_RATE_LIMIT"      , _K_D_IO	,	LeakRateLimit		    ,	0	} ,
	{ "PRMD_BM2_ALC_AVG_LIMIT"       , _K_D_IO	,	LeakAvgLimit		    ,	0	} ,

	{ "BM2.LeakChk_Log_Rate1"		, _K_D_IO	, Log_Rate1					,	0	} ,
	{ "BM2.LeakChk_Log_Rate2"		, _K_D_IO	, Log_Rate2					,	0	} ,
	{ "BM2.LeakChk_Log_Rate3"		, _K_D_IO	, Log_Rate3					,	0	} ,
	{ "BM2.LeakChk_Log_Rate4"		, _K_D_IO	, Log_Rate4					,	0	} ,
	{ "BM2.LeakChk_Log_Rate5"		, _K_D_IO	, Log_Rate5					,	0	} ,
	{ "BM2.LeakChk_Log_Avg1"		, _K_D_IO	, Log_Avg1					,	0	} ,
	{ "BM2.LeakChk_Log_Avg2"		, _K_D_IO	, Log_Avg2					,	0	} ,
	{ "BM2.LeakChk_Log_Avg3"		, _K_D_IO	, Log_Avg3					,	0	} ,
	{ "BM2.LeakChk_Log_Avg4"		, _K_D_IO	, Log_Avg4					,	0	} ,
	{ "BM2.LeakChk_Log_Avg5"		, _K_D_IO	, Log_Avg5					,	0	} ,
	{ "BM2.LeakChk_Log_Time1"       , _K_S_IO   , Log_Time1					,	0   } ,
	{ "BM2.LeakChk_Log_Time2"       , _K_S_IO   , Log_Time2					,	0   } ,
	{ "BM2.LeakChk_Log_Time3"       , _K_S_IO   , Log_Time3					,	0   } ,
	{ "BM2.LeakChk_Log_Time4"       , _K_S_IO   , Log_Time4					,	0   } ,
	{ "BM2.LeakChk_Log_Time5"       , _K_S_IO   , Log_Time5					,	0   } ,

//-------------------------------------------------------------------------------------
	// 2008.11.17 by jonghyuk (Cycle Purge)
	{ "FC_BM2_CYCPURGE_CTRL"		, _K_D_IO	, FC_CYCLE_PURGE_BM2		,	0	} ,
	{ "PRMA_BM2_CP_SVENT_PRES"		, _K_A_IO	, PRMA_BM2_CP_SVENT_PRES	,	0	} ,
	{ "PRMA_BM2_CP_FVENT_PRES"		, _K_A_IO	, PRMA_BM2_CP_FVENT_PRES	,	0	} ,
	{ "PRMA_BM2_CP_SPUMP_PRES"		, _K_A_IO	, PRMA_BM2_CP_SPUMP_PRES	,	0	} ,
	{ "PRMA_BM2_CP_FPUMP_PRES"		, _K_A_IO	, PRMA_BM2_CP_FPUMP_PRES	,	0	} ,

	{ "PRMA_BM2_CP_RPUMP_PRES"		, _K_A_IO	, PRMA_BM2_CP_RPUMP_PRES	,	0	} ,

	{ "PRMD_BM2_CP_SVENT_TMO"		, _K_D_IO	, PRMD_BM2_CP_SVENT_TMO		,	0	} ,
	{ "PRMD_BM2_CP_FVENT_TMO"		, _K_D_IO	, PRMD_BM2_CP_FVENT_TMO		,	0	} ,

	{ "PRMD_BM2_CP_SPUMP_TMO"		, _K_D_IO	, PRMD_BM2_CP_SPUMP_TMO		,	0	} ,
	{ "PRMD_BM2_CP_FPUMP_TMO"		, _K_D_IO	, PRMD_BM2_CP_FPUMP_TMO		,	0	} ,

	{ "PRMD_BM2_CP_RPUMP_TMO"		, _K_D_IO	, PRMD_BM2_CP_RPUMP_TMO		,	0	} ,
	{ "PRMD_BM2_CP_OVENT_TIME"		, _K_D_IO	, PRMD_BM2_CP_OVENT_TIME	,	0	} ,

	{ "PRMA_BM2_CP_VENT_TD" 			, _K_A_IO	, LL_CP_VENT_DELAYTIME	,	0	} ,

	{ "PRMD_BM2_CYCLE_PURGE_CNT"		, _K_D_IO	, PRMD_BM2_CYCLE_PURGE_CNT	,	0	} ,
	{ "BM2.CYCLE_PURGE_RCNT_DM"			, _K_D_IO	, BM2_CYCLE_PURGE_RCNT_DM	,	0	} ,
	{ "BM2.CYCLE_PURGE_STS_DM"			, _K_D_IO	, BM2_CYCLE_PURGE_STS_DM	,	0	} ,
	//... 2017.03.06
	{ "PRMA_BM2_TOTAL_VENT_TIME"	, _K_A_IO	, PRMA_BM2_TOTAL_VENT_TIME	,	0	} ,
	{ "PRMD_BM2_STABLE_OVERVENT"	, _K_D_IO	, PRMD_BM2_STABLE_OVERVENT	,	0	} ,
	//... 2017.03.07
	{ "PRMD_BM2_FINAL_PUMP"			, _K_D_IO	, PRMD_BM2_FINAL_PUMP		,	0	} ,
	{ "CTC.MANUAL_TR_CONTROL"		, _K_D_IO	, MANUAL_TR_CONTROL		,	0	} ,
//------------------------------------------------------------------------------------------


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

//--------------------------------------------------------------------
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
		if(STRCMP_L(szRunStr , "PUMP")) strcpy(szEventID, "Pumping");
		else if(STRCMP_L(szRunStr , "SOFTPUMP")) strcpy(szEventID, "SoftPumping");
		else if(STRCMP_L(szRunStr , "FASTPUMP")) strcpy(szEventID, "FastPumping");
		else if(STRCMP_L(szRunStr , "VENT"))     strcpy(szEventID, "Venting");
		else if(STRCMP_L(szRunStr , "SOFTVENT")) strcpy(szEventID, "SoftVenting");
		else if(STRCMP_L(szRunStr , "FASTVENT")) strcpy(szEventID, "FastVenting");
		else if(STRCMP_L(szRunStr , "READY"))    strcpy(szEventID, "Ready");
		else
		{
			_MLOG("-----> %s %s %s %s", szModule, szRunStr, szStatus, szData);
			break;
		}
		_MLOG("FNC %s %s %s %s", szModule, szEventID, szStatus, szData);
	} while(0);
}


//------------------------------------------------------------------------------------------
Module_Status Vacuum_Pump_Sub();
Module_Status Vacuum_Vent_Sub();
Module_Status Vent_Wait( int P_P , int Vlv , int Vlv_Set , int T_P , int TMO , int ALARM , int ATMVACSensor, EVENT_CMD eVentCmd);

// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Pump_Sub();
Module_Status CyclePurge_Vent_Sub();
Module_Status CyclePurge_Vent_Wait( int P_P , int Vlv , int Vlv_Set , int T_P , int TMO , int ALARM , int ATMVACSensor, EVENT_CMD eVentCmd);

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status SetOverPres() {
	int CommStatus;

	// Check Pre_Vacuum
	if (READ_DIGITAL( RoughVvXO_BM2 , &CommStatus ) == BM_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		     READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		     READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
			 RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");
		}
	}
	
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Pump(){
	Module_Status res;
	int CommStatus;

	WRITE_DIGITAL( BM2_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_PUMP, &CommStatus);
	WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_GOVAC , &CommStatus );        		

	// Lock BM2
	WRITE_DIGITAL( LockPumpDM_BM2 , ULL_LOCK , &CommStatus );

	// Check Pre_Vacuum
	if( READ_DIGITAL( RoughVvXO_BM2 , &CommStatus ) == BM_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		     READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		     READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
			RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");
		}
		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_VACUUM , &CommStatus );
		return SYS_SUCCESS;
	}

	// Do Pumping
	res = CyclePurge_Pump_Sub();

	// Check Post_Vacuum
	if( READ_DIGITAL( RoughVvXO_BM2 , &CommStatus ) == BM_OPENFAST &&
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		     READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		     READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
			RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");
		}
	}

	// Check Return Value of Vacuum_Pump_Sub Function
	if ( SYS_ABORTED == res ) 
	{

		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_ABORT , &CommStatus );
	}
	else
	{
		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_VACUUM , &CommStatus );
	}

	// Unlock BM2
	WRITE_DIGITAL( LockPumpDM_BM2 , ULL_UNLOCK , &CommStatus );

	return res;
}

Module_Status Vacuum_Pump() {
	Module_Status res;
	int CommStatus;

	WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_GOVAC , &CommStatus );        		

	// Lock BM2
	WRITE_DIGITAL( LockPumpDM_BM2 , ULL_LOCK , &CommStatus );

	//Check Pre_Vacuum
	if (READ_DIGITAL( RoughVvXO_BM2 , &CommStatus ) == BM_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if (READ_DIGITAL(LOT_CONTROL   , &CommStatus ) == LS_RUNNING ||
		    READ_DIGITAL(LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
			READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ||
			READ_DIGITAL( MANUAL_TR_CONTROL, &CommStatus) == 1 ) {
			RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");
		}

		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_VACUUM , &CommStatus );
		return SYS_SUCCESS;
	}

	// Do Pumping
	res = Vacuum_Pump_Sub();

	// Check Post_Vacuum
	if (READ_DIGITAL( RoughVvXO_BM2 , &CommStatus ) == BM_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if (READ_DIGITAL(LOT_CONTROL  , &CommStatus) == LS_RUNNING ||
		    READ_DIGITAL(LOT2_CONTROL , &CommStatus) == LS_RUNNING || 
			READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ||
			READ_DIGITAL( MANUAL_TR_CONTROL, &CommStatus) == 1 ) {
			RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");
		}
	}

	// Check Return Value of Vacuum_Pump_Sub Function
	if (SYS_ABORTED == res) {
		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_ABORT , &CommStatus );
	} else {
		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_VACUUM , &CommStatus );
	}

	// Unlock BM2
	WRITE_DIGITAL( LockPumpDM_BM2 , ULL_UNLOCK , &CommStatus );
	return res;
}


//------------------------------------------------------------------------------------------
Module_Status Wait_Pump_Pressure( int PUMP , int P_P , int T_P , int TMO , int ALARM ) {
	int Timer = 0;
	int CommStatus , AlmResult;

	// Do Pump ON
	if (RUN_FUNCTION( PUMP , "ON" ) == SYS_ABORTED)
		return SYS_ABORTED;

	// Check Pressure Between P_P(Physical Pressure Channel) and T_P(Target Pressure Channel)
	if (READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus))
		return SYS_SUCCESS;

	// Wait Until Pressure is Satisfied Between P_P(Physical Pressure) and T_P(Target Pressure)
	while (TRUE) {
		if (!WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
		if (READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus)) break;
		if (( Timer / 2 ) > READ_DIGITAL(TMO , &CommStatus)) {
			AlmResult = ALARM_MANAGE( ALARM );
			Timer = 0;
			if      (AlmResult == ALM_IGNORE) break;
			else if (AlmResult != ALM_RETRY ) return SYS_ABORTED;
		}
		Timer++;
	}
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
// Pump_Wait( BackPressure_Fore , 
//					-1 ,					// For pressure monitoring
//					-1,						// For pressure monitoring
//					T_PUMP_PRESSURE_TM , 
//					T_PUMP_TIMEOUT_TM , 
//					ALARM_PUMP_CHECK_TM 
//					) == SYS_ABORTED ) return SYS_ABORTED;
//------------------------------------------------------------------------------------------
Module_Status Pump_Wait(int P_P , int Vlv , int Vlv_Set, int T_P , int TMO , int ALARM) {
	int Timer = 0;
	int CommStatus , AlmResult;

	// Do Vavle Operation
	WRITE_DIGITAL( Vlv , Vlv_Set , &CommStatus );

	// Check Pressure Between Pr(Physical Pressure Channel) and TP(Target Pressure Channel)
	if (READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus ) ) {
		if (!WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
		if (READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus ) ) return SYS_SUCCESS;
		return SYS_SUCCESS;
	}

	// Wait Until Pressure is Satisfied Between P_P(Physical Pressure) and T_P(Target Pressure)
	while (1) {
		if (!WAIT_SECONDS( 0.5 ) ) 
			return SYS_ABORTED;
		
		if (READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus)) {
			if (!WAIT_SECONDS( 0.5)) return SYS_ABORTED;
			if (READ_ANALOG( P_P , &CommStatus) <= READ_ANALOG( T_P , &CommStatus))	break;
		}

		if ((Timer / 2) > READ_DIGITAL( TMO , &CommStatus)) {
			AlmResult = ALARM_MANAGE(ALARM);
			Timer = 0;
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		Timer++;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Pump_Sub() {
	int CommStatus;
	
	do
	{
		if ( READ_ANALOG( CGChmPrsAI , &CommStatus ) > READ_ANALOG( CGPumpPrsAI , &CommStatus ) )	break;			
//		if ( ( READ_DIGITAL( ATMSensorDI , &CommStatus ) == AS_ATM ) &&
//			 ( READ_DIGITAL( VACSensorDI , &CommStatus ) == VS_NOTVAC ) ) break;
		if ( READ_DIGITAL( ATMVACSensorXI , &CommStatus ) == ATM_SENSOR ) break;

//		if ( READ_DIGITAL( SlowRoughVvDO_BM2 , &CommStatus ) != CO_CLOSE ) break;
//		if ( READ_DIGITAL( FastRoughVvDO_BM2  , &CommStatus) != CO_OPEN ) break;
		if ( READ_DIGITAL( RoughVvXO_BM2  , &CommStatus) != BM_OPENFAST ) break;		
		if ( READ_DIGITAL( VacuumStatusDM_BM2 , &CommStatus) != VS_VACUUM ) break;
		return SYS_SUCCESS;
	}while(0);
	if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// CloseGate & Cassette Door
	do {
		int nBM_DoorLXI = UNKNOWN_DOOR;
		int nBM_DoorRXI = UNKNOWN_DOOR;

		nBM_DoorLXI = READ_DIGITAL(BM_DoorLXI, &CommStatus);
		nBM_DoorRXI = READ_DIGITAL(BM_DoorRXI, &CommStatus);

		if( (CLOSE_DOOR == nBM_DoorLXI) && (CLOSE_DOOR == nBM_DoorRXI) )
		{
			// no need to close door, becase already done.
			break;
		}
		else
		{
			if (RUN_FUNCTION(DOOR_CONTROL   , "CLOSE")  == SYS_ABORTED ) return SYS_ABORTED;
		}

		break;
	}while(0);

	if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// Slow & Fast Rough Valve Close
	RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "CLOSE_OPRES_VALVE");
	// WRITE_DIGITAL( BM2_OverPresVvDO , CO_CLOSE , &CommStatus );	
//	WRITE_DIGITAL( SlowVentVvDO_BM2 , CO_CLOSE , &CommStatus );
//	WRITE_DIGITAL( FastVentVvDO_BM2 , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );

//	WRITE_DIGITAL( SlowRoughVvDO_BM2 , CO_CLOSE , &CommStatus );
//	WRITE_DIGITAL( FastRoughVvDO_BM2 , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( RoughVvXO_BM2 , BM_CLOSE , &CommStatus );

	if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;

	// Do Roughing Line Pump
	if ( Wait_Pump_Pressure ( BM2_PUMP_CONTROL, 
		                      CGPumpPrsAI , 
							  PRMA_BM2_CP_RPUMP_PRES , 
							  PRMD_BM2_CP_RPUMP_TMO, 
							  ALARM_RPUMP_TIMEOUT 
							  ) == SYS_ABORTED ) return SYS_ABORTED;

	// Do Chamber Pump by Slow Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
//					SlowRoughVvDO_BM2 , 
					RoughVvXO_BM2 ,
//					CO_OPEN ,
					BM_OPENSLOW ,
					PRMA_BM2_CP_SPUMP_PRES , 
					PRMD_BM2_CP_SPUMP_TMO , 
					ALARM_S_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;

	// Close Slow Rough Valve
//	WRITE_DIGITAL( SlowRoughVvDO_BM2 , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( RoughVvXO_BM2 , BM_CLOSE , &CommStatus );

	// mcshin : fast pump전에 over pressure valve open
	if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		 READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		 READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
		RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");
		// WRITE_DIGITAL( BM2_OverPresVvDO , CO_OPEN , &CommStatus );
	}

	// Do Chamber Pump by Fast Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
//					FastRoughVvDO_BM2 , 
					RoughVvXO_BM2 ,
//					CO_OPEN , 
					BM_OPENFAST ,
					PRMA_BM2_CP_FPUMP_PRES , 
					PRMD_BM2_CP_FPUMP_TMO , 
					ALARM_F_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;

	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Vacuum_Pump_Sub() {
	int CommStatus;
	
	do {
		if (READ_ANALOG (CGChmPrsAI , &CommStatus ) > READ_ANALOG( CGPumpPrsAI , &CommStatus ))	break;			
		if (READ_DIGITAL(ATMVACSensorXI    , &CommStatus) == ATM_SENSOR) break;
		if (READ_DIGITAL(RoughVvXO_BM2 , &CommStatus) != BM_OPENFAST)    break;	
		if (READ_DIGITAL(VacuumStatusDM_BM2, &CommStatus) != VS_VACUUM)  break;
		return SYS_SUCCESS;
	}while(0);

	//if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
	if( TRUE == MANAGER_ABORT() ) return SYS_ABORTED;

	//LogEvent("BM2", "PUMP", "Start", "");				// 2013.11.15

	// CloseGate & Cassette Door
	do {
		int nBM_DoorLXI = UNKNOWN_DOOR;
		int nBM_DoorRXI = UNKNOWN_DOOR;

		nBM_DoorLXI = READ_DIGITAL(BM_DoorLXI, &CommStatus);
		nBM_DoorRXI = READ_DIGITAL(BM_DoorRXI, &CommStatus);

		if( (CLOSE_DOOR == nBM_DoorLXI) && (CLOSE_DOOR == nBM_DoorRXI) )
		{
			// no need to close door, becase already done.
			break;
		}
		else
		{
			if (RUN_FUNCTION(DOOR_CONTROL   , "CLOSE")  == SYS_ABORTED ) return SYS_ABORTED;
		}


		break;
	}while(0);

	if( TRUE == MANAGER_ABORT() ) return SYS_ABORTED;

	LogEvent("BM2", "PUMP", "Start", "");					// 2014.7.25

	// Slow & Fast Rough Valve Close
	RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "CLOSE_OPRES_VALVE");
	WRITE_DIGITAL(VentVvXO_BM2 , BM_CLOSE , &CommStatus);

	WRITE_DIGITAL(RoughVvXO_BM2 , BM_CLOSE , &CommStatus);

	if ( !WAIT_SECONDS( 0.1 ) ) return SYS_ABORTED;

	// Do Roughing Line Pump
	if ( Wait_Pump_Pressure ( BM2_PUMP_CONTROL, 
		                      CGPumpPrsAI , 
							  PRMA_BM2_RPUMP_PRES , 
							  PRMD_BM2_RPUMP_TMO, 
							  ALARM_RPUMP_TIMEOUT 
							  ) == SYS_ABORTED ) return SYS_ABORTED;


	LogEvent("BM2", "SOFTPUMP", "Start", "");				// 2014.7.25
	// Do Chamber Pump by Slow Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
					RoughVvXO_BM2 ,
					BM_OPENSLOW ,
					PRMA_BM2_SPUMP_PRES , 
					PRMD_BM2_SPUMP_TMO , 
					ALARM_S_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;

	LogEvent("BM2", "SOFTPUMP", "End", "");		    		// 2014.7.25

	// Close Slow Rough Valve
	WRITE_DIGITAL( RoughVvXO_BM2 , BM_CLOSE , &CommStatus );

	LogEvent("BM2", "FASTPUMP", "Start", "");				// 2014.7.25
	// Do Chamber Pump by Fast Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
					RoughVvXO_BM2 ,
					BM_OPENFAST ,
					PRMA_BM2_FPUMP_PRES , 
					PRMD_BM2_FPUMP_TMO , 
					ALARM_F_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;

	LogEvent("BM2", "FASTPUMP", "End", "");					// 2014.7.25
	
	if ( READ_DIGITAL(LOT_CONTROL  , &CommStatus ) == LS_RUNNING ||
		 READ_DIGITAL(LOT2_CONTROL , &CommStatus ) == LS_RUNNING || 
		 READ_DIGITAL(LOT3_CONTROL , &CommStatus ) == LS_RUNNING ) {
		 RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");
	}
	
	LogEvent("BM2", "PUMP", "End", "");				// 2013.11.15
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Vent() {
	Module_Status res;
	int CommStatus;

	// Close overpressure valve
	RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "CLOSE_OPRES_VALVE");
    // WRITE_DIGITAL( BM2_OverPresVvDO , CO_CLOSE , &CommStatus );	

	// Set BM2 Vent Status
	WRITE_DIGITAL( BM2_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_VENT, &CommStatus);
    WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_GOATM , &CommStatus );	

	// Do Venting
	res = CyclePurge_Vent_Sub();

	// Close Slow & Fast Vent Valve
//	WRITE_DIGITAL( SlowVentVvDO_BM2 , CO_CLOSE , &CommStatus );
//	WRITE_DIGITAL( FastVentVvDO_BM2 , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );

	// Check Return Value of Vacuum_Vent_Sub Function
	if ( SYS_ABORTED == res ) 
	{
		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_ABORT , &CommStatus );
	}
	else
	{
	    WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_ATM , &CommStatus );	
	}

	return res;
}


//------------------------------------------------------------------------------------------
Module_Status Vacuum_Vent() {
	Module_Status res;
	int CommStatus;

	// Close overpressure valve
	RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "CLOSE_OPRES_VALVE");

	// Set BM2 Vent Status
    WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_GOATM , &CommStatus );	

	// Do Venting
	res = Vacuum_Vent_Sub();

	// Close Slow & Fast Vent Valve
	WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );

	// Check Return Value of Vacuum_Vent_Sub Function
	if (SYS_ABORTED == res) {
		WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_ABORT , &CommStatus );
	} else {
	    WRITE_DIGITAL( VacuumStatusDM_BM2 , VS_ATM , &CommStatus );	
	}

	return res;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Vent_Sub() {
	int CommStatus;
//	int OverVent_Time;
	double Pressure , Target;
	int CountTime = 0;    // LoadLock Cooling Delay Value CountTime, DelayTime 
	int DelayTime = 0;
	// 2005.03.31 by cskim, Modified
	Module_Status msRet = SYS_SUCCESS;

	// Check whether Chamber is Vent or not
	Pressure  = READ_ANALOG( CGChmPrsAI		, &CommStatus );
	Target = READ_ANALOG( PRMA_BM2_CP_FVENT_PRES	, &CommStatus );
	if ( (READ_DIGITAL( ATMVACSensorXI , &CommStatus ) == ATM_SENSOR) && (Pressure >= Target) ) return SYS_SUCCESS;

	// CloseGate & Cassette Door
	do {
		int nBM_DoorLXI = UNKNOWN_DOOR;
		int nBM_DoorRXI = UNKNOWN_DOOR;

		nBM_DoorLXI = READ_DIGITAL(BM_DoorLXI, &CommStatus);
		nBM_DoorRXI = READ_DIGITAL(BM_DoorRXI, &CommStatus);

		if( (CLOSE_DOOR == nBM_DoorLXI) && (CLOSE_DOOR == nBM_DoorRXI) )
		{
			// no need to close door, becase already done.
			break;
		}
		else
		{
			if (RUN_FUNCTION(DOOR_CONTROL   , "CLOSE")  == SYS_ABORTED ) return SYS_ABORTED;
		}

		break;
	}while(0);

	// 2005.09.07 Load Lock Cooling Delay (by wjkim)
        DelayTime = (int) READ_ANALOG( LL_CP_VENT_DELAYTIME , &CommStatus );

	do{
		WAIT_SECONDS(1);
		CountTime++;
		printf( "[Debug] BM2 -- VENT Delay CountTime : %d, VENT Delay SetTime : %d\n", CountTime, DelayTime);
	}while (CountTime < DelayTime);

	// Close Slow & Fast Rough Valve
//	WRITE_DIGITAL( SlowRoughVvDO_BM2 , CO_CLOSE , &CommStatus );
//	WRITE_DIGITAL( FastRoughVvDO_BM2 , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( RoughVvXO_BM2 , BM_CLOSE , &CommStatus );

//	WRITE_DIGITAL( SlowVentVvDO_BM2  , CO_CLOSE , &CommStatus );
//	WRITE_DIGITAL( FastVentVvDO_BM2  , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_BM2  , BM_CLOSE , &CommStatus );

	if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;

	// Do Slow Vent
	if ( CyclePurge_Vent_Wait( CGChmPrsAI , 
//					SlowVentVvDO_BM2 , 
					VentVvXO_BM2 ,
//					CO_OPEN , 
					BM_OPENSLOW ,
					PRMA_BM2_CP_SVENT_PRES , 
					PRMD_BM2_CP_SVENT_TMO , 
					ALARM_S_VENT_TIMEOUT , 
//					ATMSensorDI,
//					VACSensorDI
					ATMVACSensorXI,
					VC_SVENT
					) == SYS_ABORTED ) return SYS_ABORTED;
//	WRITE_DIGITAL( SlowVentVvDO_BM2 , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );

	// Do Fast Vent
	if ( CyclePurge_Vent_Wait( CGChmPrsAI , 
//					FastVentVvDO_BM2 , 
					VentVvXO_BM2 ,
//					CO_OPEN , 
					BM_OPENFAST ,
					PRMA_BM2_CP_FVENT_PRES , 
					PRMD_BM2_CP_FVENT_TMO , 
					ALARM_F_VENT_TIMEOUT , 
//					ATMSensorDI, 
//					VACSensorDI
					ATMVACSensorXI,
					VC_FVENT
					) == SYS_ABORTED ) return SYS_ABORTED;

	// Do Over Vent
	// 2005.03.31 by cskim, Modified
	if ( !WAIT_SECONDS( READ_DIGITAL( PRMD_BM2_CP_OVENT_TIME , &CommStatus ) ) ) {
		msRet = SYS_ABORTED;
	}
//	WRITE_DIGITAL( FastVentVvDO_BM2 , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );

	return msRet;
}

Module_Status Vacuum_Vent_Sub() {
	int CommStatus;
	double Pressure , Target;
	int CountTime = 0;    // LoadLock Cooling Delay Value CountTime, DelayTime 
	int DelayTime = 0;
	// 2005.03.31 by cskim, Modified
	Module_Status msRet = SYS_SUCCESS;
	double dbTotalTime = 0;
	int nStableOverVent = 0;

	// Check whether Chamber is Vent or not
	Pressure  = READ_ANALOG( CGChmPrsAI		        , &CommStatus );
	Target    = READ_ANALOG( PRMA_BM2_FVENT_PRES	, &CommStatus );
	if ((READ_DIGITAL( ATMVACSensorXI , &CommStatus ) == ATM_SENSOR) && (Pressure >= Target)) 
		return SYS_SUCCESS;

	//LogEvent("BM2", "VENT", "Start", "");				// 2013.11.15

	//2009.06.09
	if (RUN_FUNCTION(TMC_ISOGATE_CONTROL , "CLOSE BM2" ) == SYS_ABORTED ) return SYS_ABORTED;

	do {
		int nBM_DoorLXI = UNKNOWN_DOOR;
		int nBM_DoorRXI = UNKNOWN_DOOR;

		nBM_DoorLXI = READ_DIGITAL(BM_DoorLXI, &CommStatus);
		nBM_DoorRXI = READ_DIGITAL(BM_DoorRXI, &CommStatus);

		if( (CLOSE_DOOR == nBM_DoorLXI) && (CLOSE_DOOR == nBM_DoorRXI) )
		{
			// no need to close door, becase already done.
			break;
		}
		else
		{
			if (RUN_FUNCTION(DOOR_CONTROL   , "CLOSE")  == SYS_ABORTED ) return SYS_ABORTED;
		}


		break;
	}while(0);

	// 2005.09.07 Load Lock Cooling Delay (by wjkim)
    DelayTime = (int) READ_ANALOG(LL_VENT_DELAYTIME , &CommStatus);
	do
	{
		if( CountTime >= (DelayTime * 1000) )
		{
			break;
		}

		WAIT_SECONDS(0.1);
		//CountTime++;
		CountTime += 100;

	}while (1);

	//... Start Time
	S_TIMER_READY();

	LogEvent("BM2", "VENT", "Start", "");				// 2014.7.25
	//... 2017.02.24 timer start... for stable vent

	// Close Slow & Fast Rough Valve
	WRITE_DIGITAL( RoughVvXO_BM2 , BM_CLOSE , &CommStatus );

	WRITE_DIGITAL( VentVvXO_BM2  , BM_CLOSE , &CommStatus );

	if (!WAIT_SECONDS(0.1)) return SYS_ABORTED;

	LogEvent("BM2", "SOFTVENT", "Start", "");			// 2014.7.25
	// Do Slow Vent
	if (Vent_Wait(CGChmPrsAI , 
				  VentVvXO_BM2 ,
				  BM_OPENSLOW ,
				  PRMA_BM2_SVENT_PRES , 
				  PRMD_BM2_SVENT_TMO , 
				  ALARM_S_VENT_TIMEOUT , 
				  ATMVACSensorXI,
				  VC_SVENT
				  ) == SYS_ABORTED ) return SYS_ABORTED;
	WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );

	//Modified from here for exhaust heat vent 20071115
	// Close Slow & Fast Rough Valve
	if (1 == READ_DIGITAL(BM_VentType , &CommStatus)) {
		WRITE_DIGITAL(RoughVvXO_BM2 , BM_CLOSE , &CommStatus);
		WRITE_DIGITAL(VentVvXO_BM2  , BM_CLOSE , &CommStatus);

		//delay after slow vent
		if (!WAIT_SECONDS(20)) return SYS_ABORTED;

		// pump
		Vacuum_Pump_Sub();

		// Close Slow & Fast Rough Valve
		WRITE_DIGITAL(RoughVvXO_BM2 , BM_CLOSE , &CommStatus);
		WRITE_DIGITAL(VentVvXO_BM2  , BM_CLOSE , &CommStatus);

		if (!WAIT_SECONDS(1)) return SYS_ABORTED;

		// Do Slow Vent
		if (Vent_Wait(CGChmPrsAI , 
						VentVvXO_BM2 ,
						BM_OPENSLOW ,
						PRMA_BM2_SVENT_PRES , 
						PRMD_BM2_SVENT_TMO , 
						ALARM_S_VENT_TIMEOUT , 
						ATMVACSensorXI,
						VC_SVENT
						) == SYS_ABORTED ) return SYS_ABORTED;
		WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );
	}
	//Modify End 2007.11.15
	LogEvent("BM2", "SOFTVENT", "End", "");			// 2014.7.25

	LogEvent("BM2", "FASTVENT", "Start", "");		// 2014.7.25
	// Do Fast Vent
	if (Vent_Wait(CGChmPrsAI , 
				  VentVvXO_BM2 ,
				  BM_OPENFAST ,
				  PRMA_BM2_FVENT_PRES , 
				  PRMD_BM2_FVENT_TMO , 
				  ALARM_F_VENT_TIMEOUT , 
				  ATMVACSensorXI,
				  VC_FVENT
				  ) == SYS_ABORTED ) return SYS_ABORTED;

	LogEvent("BM2", "FASTVENT", "End", "");			// 2014.7.25

	// Do Over Vent
	// 2005.03.31 by cskim, Modified
	if (!WAIT_SECONDS( READ_DIGITAL( PRMD_BM2_OVENT_TIME , &CommStatus))) {
		msRet = SYS_ABORTED;
	}

	nStableOverVent = READ_DIGITAL(PRMD_BM2_STABLE_OVERVENT,&CommStatus);
	if(nStableOverVent == OFFON_OFF)
	{
		WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );
	}

	//... 2017.03.06 wait left time ,, timer stop
	do
	{

		if(900 <= READ_ANALOG(CGChmPrsAI,&CommStatus))
		{
			_LOG("----->Pressure [%0.3f] OverVent Occur in BM1 Vacuum Seq", READ_ANALOG(CGChmPrsAI,&CommStatus));
			break;
		}

		dbTotalTime =  S_TIMER_ELAPSED();
		if(READ_ANALOG(PRMA_BM2_TOTAL_VENT_TIME,&CommStatus) <= dbTotalTime)
		{
			_LOG("----->Total[%0.1f] Slow[%0.1f] Fast[%0.1f] Stable vent in BM1 Vacuum Seq", dbTotalTime,g_dbSlowVent,g_dbFastVent);
			break;
		}

		if (!WAIT_SECONDS(0.1))
		{
			WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );
			_LOG("----->All Abort in BM1 Vacuum Seq");
			return SYS_ABORTED;
		}
	}
	while(1);

	WRITE_DIGITAL( VentVvXO_BM2 , BM_CLOSE , &CommStatus );

	LogEvent("BM2", "VENT", "End", "");				// 2013.11.15
	return msRet;
}
//------------------------------------------------------------------------------------------
Module_Status Vent_Wait( int P_P , int Vlv , int Vlv_Set , int T_P , int TMO , int ALARM , int ATMVACSensor, EVENT_CMD eVentCmd) {
	int Timer = 0;
	int CommStatus , AlmResult;

	// Do Vent Valve Operation
	WRITE_DIGITAL( Vlv , Vlv_Set , &CommStatus );

	// Check Chamber Pressure for Vent
	while (1) {
		if (VC_SVENT == eVentCmd) {
			if (READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG(T_P , &CommStatus)) {
				if (!WAIT_SECONDS(0.5)) {
					WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus);
					return SYS_ABORTED;
				}

				g_dbSlowVent = 0;
				g_dbSlowVent = Timer/2;
				if ( READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) break;
			}
		} else {
			if ((READ_DIGITAL( ATMVACSensor , &CommStatus ) == ATM_SENSOR ) &&
				(READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) ) {
				if (!WAIT_SECONDS(0.5)) {
					WRITE_DIGITAL(Vlv , CO_CLOSE , &CommStatus);
					return SYS_ABORTED;
				}
				g_dbFastVent = 0;
				g_dbFastVent = Timer/2;
				if (READ_ANALOG(P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) break;
			}
		}

		if ( !WAIT_SECONDS( 0.5 ) ) {
			WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
			return SYS_ABORTED;
		}

		if ( ( Timer / 2 ) > READ_DIGITAL( TMO , &CommStatus ) ) {
			WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
			AlmResult = ALARM_MANAGE( ALARM );
			Timer = 0;
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			WRITE_DIGITAL( Vlv , CO_OPEN , &CommStatus );
		}
		Timer++;
	}
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
Module_Status CyclePurge_Vent_Wait( int P_P , int Vlv , int Vlv_Set , int T_P , int TMO , int ALARM , int ATMVACSensor, EVENT_CMD eVentCmd) {
	int Timer = 0;
	int CommStatus , AlmResult;

	// Do Vent Valve Operation
	WRITE_DIGITAL( Vlv , Vlv_Set , &CommStatus );

	// Check Chamber Pressure for Vent
	while (1) {
//		if ( ( READ_DIGITAL( ATMS , &CommStatus ) == AS_ATM ) &&
//			 ( READ_DIGITAL( VAC , &CommStatus )  == VS_NOTVAC ) ) break;

		if ( VC_SVENT == eVentCmd ) {
			if ( READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) {
				if ( !WAIT_SECONDS( 0.5 ) ) {
					WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
					return SYS_ABORTED;
				}
				if ( READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) break;
			}
		}
		else {
			if ( /*( READ_DIGITAL( ATMVACSensor , &CommStatus ) == ATM_SENSOR ) && */
				 ( READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) ) {
				if ( !WAIT_SECONDS( 0.5 ) ) {
					WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
					return SYS_ABORTED;
				}
				if ( READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) break;
			}
		}

		if ( !WAIT_SECONDS( 0.5 ) ) {
			WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
			return SYS_ABORTED;
		}

		if ( ( Timer / 2 ) > READ_DIGITAL( TMO , &CommStatus ) ) {
			WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
			AlmResult = ALARM_MANAGE( ALARM );
			Timer = 0;
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			WRITE_DIGITAL( Vlv , CO_OPEN , &CommStatus );
		}
		Timer++;
	}
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void saveLkAvePrs(const double dData)
{
	int nComSts = 0;
    WRITE_DIGITAL(BM2_LOGGING, NY_NO, &nComSts);
	WRITE_ANALOG(BM2_PC_LEAKAVE, dData, &nComSts);
	WRITE_DIGITAL(BM2_LOGGING, NY_YES, &nComSts);
}

//------------------------------------------------------------------------------------------
void saveLkChkTime(void)
{
	char buf[80];
	SYSTEMTIME time;
	int nComSts = 0;

	GetLocalTime(&time);
	sprintf(buf, "%d/%02d/%02d  %d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
    WRITE_DIGITAL(BM2_LOGGING, NY_NO, &nComSts);
	WRITE_STRING(BM2_PC_LKCHK_LASTTIME, buf, &nComSts);
	WRITE_DIGITAL(BM2_LOGGING, NY_YES, &nComSts);
}

//2008.04.04 Leak Check History Log added
BOOL InitHistory()
{
	int CommStatus , i;
	char szRead[256];
	char szItem[256];

	//initialize structure
	for (i = 0 ; i < MAX_HISTORY_COUNT ; i++) {
		memset(g_LeakLog[i].szTime, 0 , 256);
		g_LeakLog[i].nLastLeakRate = 0;
		g_LeakLog[i].nAvgLeakRate  = 0;
	}

	//Get Log from file
	FILE *fpLog;
	fpLog = fopen(FILE_PATH , "rt");
	if (NULL == fpLog) return FALSE;
	else {
		i = 0;
		if (NULL == fgets(szRead, 256, fpLog)) return TRUE;
		do {
			CTextParser tp(szRead);
			do {
				if (FALSE == tp.GetWord(szItem , 0))			               break;
				if (strlen(szItem) > 255) {
					printf ("Read Time over limited count \n");
					break;
				}
				else strcpy(g_LeakLog[i].szTime , szItem);
				if (FALSE == tp.GetInt((int*)&g_LeakLog[i].nLastLeakRate , 0)) break;			
				if (FALSE == tp.GetInt((int*)&g_LeakLog[i].nAvgLeakRate  , 0)) break;;

				i++;
				if (i >= MAX_HISTORY_COUNT) break;
			} while (0);
			if (NULL == fgets(szRead, 256, fpLog)) break;
		} while (feof(fpLog) == 0);
	}

	if (NULL != fpLog) fclose(fpLog);

	//Set to IO
	for (i = 0 ; i < MAX_HISTORY_COUNT ; i++ ) {
		WRITE_DIGITAL(Log_Rate1 + i , g_LeakLog[i].nLastLeakRate , &CommStatus);
		WRITE_DIGITAL(Log_Avg1  + i , g_LeakLog[i].nAvgLeakRate  , &CommStatus);
		WRITE_STRING (Log_Time1 + i , g_LeakLog[i].szTime        , &CommStatus);
	}    

	return TRUE;
}

void SaveLeakCheckHistory(int nLeakRate , int nLeakAvg)
{
	SYSTEMTIME time;
	int        i , CommStatus;	
	char       buf[80];

	//2008.04.04 Save History
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GetLocalTime(&time);		
	sprintf(buf, "%d/%02d/%02d:%d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	for (i = 1 ; i < MAX_HISTORY_COUNT ; i++) {
		memset (g_LeakLog[MAX_HISTORY_COUNT-i].szTime , 0 , 256);
		if   (NULL == g_LeakLog[MAX_HISTORY_COUNT-i-1].szTime) continue;
		else {
			//Log Leak Check Time
			strcpy(g_LeakLog[MAX_HISTORY_COUNT-i].szTime    , g_LeakLog[MAX_HISTORY_COUNT-i-1].szTime);
			WRITE_STRING (Log_Time1 + MAX_HISTORY_COUNT-i , g_LeakLog[MAX_HISTORY_COUNT-i].szTime , &CommStatus);
			//Log Leak Check Rate
			g_LeakLog[MAX_HISTORY_COUNT-i].nLastLeakRate = g_LeakLog[MAX_HISTORY_COUNT-i-1].nLastLeakRate;
			WRITE_DIGITAL(Log_Rate1 + MAX_HISTORY_COUNT-i , g_LeakLog[MAX_HISTORY_COUNT-i].nLastLeakRate , &CommStatus);
			//Log Leak Check Average
			g_LeakLog[MAX_HISTORY_COUNT-i].nAvgLeakRate  = g_LeakLog[MAX_HISTORY_COUNT-i-1].nAvgLeakRate;
			WRITE_DIGITAL(Log_Avg1  + MAX_HISTORY_COUNT-i , g_LeakLog[MAX_HISTORY_COUNT-i].nAvgLeakRate  , &CommStatus);
		}
	}
	
	//Current History
	g_LeakLog[0].nLastLeakRate = nLeakRate;
	g_LeakLog[0].nAvgLeakRate  = nLeakAvg;
	memset(g_LeakLog[0].szTime , 0 , 256);
	strcpy(g_LeakLog[0].szTime , buf);
	WRITE_STRING (Log_Time1 , g_LeakLog[0].szTime        , &CommStatus);
	WRITE_DIGITAL(Log_Rate1 , g_LeakLog[0].nLastLeakRate , &CommStatus);
	WRITE_DIGITAL(Log_Avg1  , g_LeakLog[0].nAvgLeakRate  , &CommStatus);

	//Save to file
	FILE *fpLog;
	fpLog = fopen(FILE_PATH , "w+");
	if (NULL != fpLog) {
		for (i = 0 ; i < MAX_HISTORY_COUNT ; i++) {
            if (NULL == g_LeakLog[i].szTime) continue;
			fprintf(fpLog , "%s\t%d\t%d\n" , g_LeakLog[i].szTime , g_LeakLog[i].nLastLeakRate , g_LeakLog[i].nAvgLeakRate);
		}			
	}

	if (NULL != fpLog) fclose(fpLog);
	//2008.04.04 Save History
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

//2013.01.09 Save Leak Rate & Average 
//---------------------------------------------------------------------------------------
void saveIO()
{
	int nComSts = 0;
    WRITE_DIGITAL(BM2_LOGGING, NY_NO , &nComSts);
	WRITE_DIGITAL(BM2_LOGGING, NY_YES, &nComSts);
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Vacuum_Leak() {
	int CommStatus , CurrCount = 1 , i , Time , AlmResult;
	char Buffer_Str[1025];
	char BM2_Time[1024];
	double Start_Press , Start_Press1 , Current_Press , Result;
	int nHour = 0, nMin = 0, nSec = 0, nTemp = 0;
	BOOL bSuccess;
	int  nLastRate , nAvgRate;

	//2008.04.04
	while (1) {

		bSuccess = TRUE;

	// Close overpressure valve
	RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "CLOSE_OPRES_VALVE");

	//////////////////////////////////////////////////////////////////
	// Begin Initialize
	WRITE_DIGITAL( BM2_Leak_End , OFFON_OFF , &CommStatus );
	if ( READ_DIGITAL( BM2_Leak_Test_Number , &CommStatus ) == 0 ) {
		sprintf( Buffer_Str , "%s Leak Test count check...", NAME_CHAMBER);
		WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );
		return SYS_SUCCESS;
	}

	// Set zero to each Leak Rate Channel
	for ( i = 0 ; i < 11 ; i++ ) 
		WRITE_ANALOG ( BM2_Leak_Rate1 + i , 0 , &CommStatus );
	saveIO();

	// Set Time zero to Leak Wait Remain Time Channel
	WRITE_STRING( BM2_Leak_WaitRemain_Time , "00:00:00" , &CommStatus );

	// Set Time zero to Leak Pump Remain Time Channel
	WRITE_STRING( BM2_Leak_PumpRemain_Time , "00:00:00" , &CommStatus );
	// End Initialize
	//////////////////////////////////////////////////////////////////

	// Start Pumping
	sprintf( Buffer_Str , "%s Pumping Start...", NAME_CHAMBER);
	WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );
	if ( Vacuum_Pump() == SYS_ABORTED ) return SYS_ABORTED;

	//////////////////////////////////////////////////////////////////
	// Begin Continued Pump
	Time = READ_DIGITAL( BM2_Leak_PumpingTime_H , &CommStatus )*3600 + READ_DIGITAL( BM2_Leak_PumpingTime_M , &CommStatus )*60;
	sprintf( Buffer_Str , "%s Pumping continue time..." , NAME_CHAMBER);
	WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );
	S_TIMER_READY();
	while ( TRUE ) {
		nHour = (int)( Time - S_TIMER_ELAPSED() ) / 3600;
		nTemp = (int)( Time - S_TIMER_ELAPSED() ) % 3600;
		nMin  = nTemp / 60;
		nSec  = nTemp % 60;
		sprintf( BM2_Time , "%02d:%02d:%02d" , nHour , nMin , nSec );
		WRITE_STRING( BM2_Leak_PumpRemain_Time , BM2_Time , &CommStatus );
		if		( S_TIMER_REACH( Time ) ) break;
		else if ( !WAIT_SECONDS ( 0.1 ) ) return SYS_ABORTED;
	}
	WRITE_DIGITAL( RoughVvXO_BM2 , BM_CLOSE , &CommStatus );
	// End Continued Pump
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// Begin Wait Time
	Time = READ_DIGITAL( BM2_Leak_WaitingTime_H , &CommStatus )*3600 + READ_DIGITAL( BM2_Leak_WaitingTime_M , &CommStatus )*60;
	sprintf( Buffer_Str , "%s Leak Test Wait time..." , NAME_CHAMBER);
	WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );
	S_TIMER_READY();
	while ( TRUE ) {
		nHour = (int)( Time - S_TIMER_ELAPSED() ) / 3600;
		nTemp = (int)( Time - S_TIMER_ELAPSED() ) % 3600;
		nMin  = nTemp / 60;
		nSec  = nTemp % 60;
		sprintf( BM2_Time , "%02d:%02d:%02d" , nHour , nMin , nSec );
		WRITE_STRING( BM2_Leak_WaitRemain_Time , BM2_Time , &CommStatus );
		if		( S_TIMER_REACH( Time ) ) break;
		else if ( !WAIT_SECONDS ( 0.1 ) ) return SYS_ABORTED;
	}
	// End Wait Time
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// Start Calibration

	// Set Leak Check Date & Time
	sprintf( Buffer_Str , "%s Calibration Start..." , NAME_CHAMBER);
	WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );
	_getsystime( &Pres_Time );
	WRITE_DIGITAL( BM2_Year  , Pres_Time.tm_year+1900 , &CommStatus );
	WRITE_DIGITAL( BM2_Month , Pres_Time.tm_mon + 1   , &CommStatus );
	WRITE_DIGITAL( BM2_Day   , Pres_Time.tm_mday      , &CommStatus );
	WRITE_DIGITAL( BM2_Hour  , Pres_Time.tm_hour      , &CommStatus );
	WRITE_DIGITAL( BM2_Min   , Pres_Time.tm_min       , &CommStatus );
	WRITE_DIGITAL( BM2_Sec   , Pres_Time.tm_sec       , &CommStatus );

	// Check Leak
	for ( CurrCount = 0 ; CurrCount < 10 ; CurrCount++ ) {
		S_TIMER_READY();
		Start_Press1 = READ_ANALOG( CGChmPrsAI , &CommStatus );
		if( CurrCount == 0 ) Start_Press = Start_Press1;

		sprintf( Buffer_Str , "%s %d Step Leak Test..." , NAME_CHAMBER, CurrCount+1 );
		WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );
		while ( TRUE ) {
			// If User click Stop Leak Check on screen, BM2_Leak_End is changed OFFON_ON !!!
			if ( READ_DIGITAL( BM2_Leak_End , &CommStatus ) == OFFON_ON ) {
				Time = (int)( 60 - ( (int)S_TIMER_ELAPSED() % 60 ) );
				WRITE_DIGITAL( BM2_Leak_Test_Time , Time , &CommStatus );
			}
			else {
				Time = (int)( ( 60 * READ_DIGITAL( BM2_Leak_Test_Number , &CommStatus ) ) -
					          ( S_TIMER_ELAPSED() + (CurrCount*60 ) ) );
				WRITE_DIGITAL( BM2_Leak_Test_Time , Time , &CommStatus );
			}
			if		( S_TIMER_REACH( 60 ) ) break;
			else if ( !WAIT_SECONDS ( 0.1 ) ) return SYS_ABORTED;
		}

		Current_Press = READ_ANALOG( CGChmPrsAI , &CommStatus );

		Result = Current_Press - Start_Press1 ;
        if ( Result < 0.0 ) Result = 0.0;
		WRITE_ANALOG( BM2_Leak_Rate1 + CurrCount , Result*1000 , &CommStatus );
		// Leak Rate 1~10 Data Save 2013.01.09
		saveIO();

			//2008.04.04 Auto Leak Check & Leak check alarm post
			nLastRate = (int)(Result * 1000);
			if (nLastRate < 0) nLastRate = 0;
			if ((Result * 1000) > READ_DIGITAL(LeakRateLimit , &CommStatus)) { bSuccess = FALSE; break;}			

			if ( READ_DIGITAL( BM2_Leak_Test_Number , &CommStatus ) == CurrCount+1 ) break;
			if ( READ_DIGITAL( BM2_Leak_End , &CommStatus ) == OFFON_ON ) {
				WRITE_DIGITAL( BM2_Leak_End , OFFON_OFF , &CommStatus );
				break;
			}
		}
		//2008.04.04 Auto Leak Check & Leak check alarm post
		if (FALSE == bSuccess) {
			//Leak Rate Alarm Post
			AlmResult = ALARM_MANAGE(ALARM_LEAK_RATE_ERR);
			if (AlmResult != ALM_RETRY)
			{
				
				Result = ((Current_Press - Start_Press) / (CurrCount+1));
				WRITE_ANALOG( BM2_Leak_LastRate , Result*1000 , &CommStatus );
				//2013.01.09 
				saveIO();
				nAvgRate = (int)(Result * 1000);
				if (nAvgRate < 0) nAvgRate = 0;
				SaveLeakCheckHistory(nLastRate , nAvgRate);
				saveLkAvePrs(Result*1000);
				saveLkChkTime();
				sprintf( Buffer_Str , "%s Leak Test Abnormal Abort..." , NAME_CHAMBER );
				WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );

				return SYS_ABORTED;
			}

			else continue;
		}
		//2008.04.04

		Result = ( ( Current_Press - Start_Press ) / (CurrCount+1) );
		WRITE_ANALOG( BM2_Leak_LastRate , Result*1000 , &CommStatus );
	//2013.01.09 
	saveIO();
		nAvgRate = (int)(Result * 1000);
		if (nAvgRate < 0) nAvgRate = 0;

		//2008.04.04 Auto Leak Check & Leak check alarm post
		if ((Result * 1000) > READ_DIGITAL(LeakAvgLimit , &CommStatus)) {
			//Leak Rate Alarm Post
			AlmResult = ALARM_MANAGE(ALARM_LEAK_AVG_ERR);
			if (AlmResult != ALM_RETRY)
			{
				// 2012.05.08 History Save When Abnormal Abort
				SaveLeakCheckHistory(nLastRate , nAvgRate);
				saveLkAvePrs(Result*1000);
				saveLkChkTime();
				sprintf( Buffer_Str , "%s Leak Test Abnormal Abort..." , NAME_CHAMBER );
				WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );

				return SYS_ABORTED;
			}
			else continue;
		}

		SaveLeakCheckHistory(nLastRate , nAvgRate);
		//2008.04.04

		saveLkAvePrs(Result*1000);
		saveLkChkTime();
		sprintf( Buffer_Str , "%s Leak Test Success End..." , NAME_CHAMBER );
		WRITE_STRING( BM2_LeakChk_Message , Buffer_Str , &CommStatus );
		WRITE_DIGITAL( RoughVvXO_BM2 , BM_OPENFAST , &CommStatus );
		break;
	}
	//... 2016.07.14
	RUN_FUNCTION(TMC_BM2_OVERPRESSURE, "OPEN_OPRES_VALVE");

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status ExecuteCyclePurge()
{
	Module_Status msRet = SYS_SUCCESS;
	int nTargetCycleCount;
	int	CommStatus, step;

	nTargetCycleCount = READ_DIGITAL(PRMD_BM2_CYCLE_PURGE_CNT, &CommStatus);
	if(nTargetCycleCount < 0 ) nTargetCycleCount = 0;

	// Cycle Purge 횟수만큼 Purge실시
	for(step=0; step<nTargetCycleCount; step++)
	{
		WRITE_DIGITAL(BM2_CYCLE_PURGE_RCNT_DM, step+1, &CommStatus);

		if(CyclePurge_Pump() == SYS_ABORTED)	{ msRet = SYS_ABORTED; break;	}
		if(CyclePurge_Vent() == SYS_ABORTED)	{ msRet = SYS_ABORTED; break;	}
	}

	// 정상적으로 cycle purge가 완료 되면, Vacuum상태로 변화시킨다.
	if(msRet == SYS_SUCCESS)	{			
		WRITE_DIGITAL( BM2_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_PUMP, &CommStatus);
		msRet = Vacuum_Pump();	
	}
	
	WRITE_DIGITAL( BM2_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_STOP, &CommStatus);

	return msRet;
}

//==========================================================================================
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int	Dl , CommStatus;
	char *Ed;
	char RunStr[21] , TempStr[21];
	//char szCmd[80];

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;
	STR_SEPERATE( Ed , RunStr , TempStr , 20 );

	// 2005.12.09
	/*
	if ( SIMULATION_MODE )
	{
		_LOG("-----> '%s' Started in BM2 Vac(Sim)", Ed);

		sprintf(szCmd,"BM2 %s", RunStr);
		if(! g_DynIO.dRUN_FUNCTION(TMSIM_BM2, szCmd)) msRet = SYS_ABORTED;

		_LOG("-----> '%s' Finished in BM2 Vac(Sim)", Ed);
		return msRet;
	}
	*/

	if ( READ_DIGITAL( FC_Vacuum_BM2 , &CommStatus ) == ONOFF_OFF ) return SYS_SUCCESS;

	if ( Dl < 1 ) {
		printf( "---> BM2 Vacuum Control Parameter Error\n" );
		return SYS_ABORTED;
	}

	// 2008.11.17 by jonghyuk (Cycle Purge)
	if	( STRCMP_L( RunStr , "CYCLE_PURGE" ) ){
		if ( READ_DIGITAL( FC_CYCLE_PURGE_BM2 , &CommStatus ) == ONOFF_OFF ) return SYS_SUCCESS;

		msRet = ExecuteCyclePurge();
		_LOG("-----> Finished (%s) in BM2 CyclePurgeFunc", Ed);
		return msRet;
	}	

	if ( READ_DIGITAL( FC_Vacuum_BM2 , &CommStatus ) == ONOFF_OFF ) return SYS_SUCCESS;	
	_LOG("-----> Started (%s) in BM2VacFunc", Ed);

	if		( STRCMP_L( RunStr , "VENT"        ) ) msRet = ( Vacuum_Vent() );
	else if ( STRCMP_L( RunStr , "VENT_ALC"    ) ) {
		//2008.04.04 Vent Skip When Auto Leak Check Running...
		if (2/*AutoLeak Running*/ == READ_DIGITAL(LeakCheckSts , &CommStatus)) 
			msRet = SYS_SUCCESS;
		else
			msRet = ( Vacuum_Vent() );
	}
	else if ( STRCMP_L( RunStr , "PUMP"        ) ) msRet = ( Vacuum_Pump() );
	else if ( STRCMP_L( RunStr , "FINAL_PUMP"  ) )
	{
		if(READ_DIGITAL(PRMD_BM2_FINAL_PUMP,&CommStatus) == OFFON_ON)
		{
			msRet = Vacuum_Pump();
		}
	}
	else if ( STRCMP_L( RunStr , "LEAKCHECK"   ) ) msRet = ( Vacuum_Leak() );	
	else if ( STRCMP_L( RunStr , "READY"   ) )	   msRet = ( SetOverPres() );	
	else {
		printf( "---> BM2 Vacuum Check Error - Not Support Code %s\n" , Ed );
		msRet =  SYS_ABORTED;
	}
	_LOG("-----> Finished (%s) in BM2VacFunc", Ed);
	return msRet;
}

//==========================================================================================
extern "C"
{
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	g_DynIO.InitIOList(g_pIOList, FALSE);

	//2008.04.04 leak check history
	if (FALSE == InitHistory()) return FALSE;

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
		if (NULL != g_fLog) fclose(g_fLog);
	}
	//--------------------------------------
}

} // End of [extern "C"]
//==========================================================================================
