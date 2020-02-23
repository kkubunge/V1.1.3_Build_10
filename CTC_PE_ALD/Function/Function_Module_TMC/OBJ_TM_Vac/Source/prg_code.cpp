#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------
#define		ALARM_S_VENT_TIMEOUT	6101
#define		ALARM_F_VENT_TIMEOUT	6102
#define		ALARM_S_PUMP_TIMEOUT	6103
#define		ALARM_F_PUMP_TIMEOUT	6104
#define		ALARM_RPUMP_TIMEOUT		6107
#define		ALARM_TM_TRPRS_OVER		6120		// 2006.01.19
#define     ALARM_LEAK_RATE_ERR		6154
#define     ALARM_LEAK_AVG_ERR		6155
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
	VENT_CLOSE,
	VENT_OPENSLOW,
	VENT_OPENFAST
}VENTVALVE;

typedef enum
{ 
	ROUGH_CLOSE,
	ROUGH_OPENSLOW,
	ROUGH_OPENFAST
}ROUGHVALVE;

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

// 2008.11.17 by jonghyuk (Cycle Purge)
typedef enum
{
	CYCLE_PURGE_STATE_STOP,
	CYCLE_PURGE_STATE_PUMP,
	CYCLE_PURGE_STATE_VENT,
}CYCLE_PURGE_STATE;

//------------------------------------------------------------------------------------------
char g_szAlarmMessage[256] = {"",};
char NAME_CHAMBER[5] = "TM";
//------------------------------------------------------------------------------------------
struct tm		Pres_Time;
//------------------------------------------------------------------------------------------
//2008.04.04 Auto Leak Check History Save
FILE*   g_fLog    = NULL;
#define FILE_PATH ".\\Parameter\\LeakCheckLog_TM.cfg"
#define MAX_HISTORY_COUNT	5
typedef struct sLog{
	char    szTime[256];
	int     nLastLeakRate;
	int     nAvgLeakRate;
}S_LOG;

S_LOG g_LeakLog[MAX_HISTORY_COUNT];
//2008.04.04

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "FC_TM_VAC_CTRL"				, _K_D_IO	, FC_Vacuum_TM				,   0   } ,

	{ "TM.LockPumpDM"				, _K_D_IO	, LockPumpDM_TM				,	0	} ,
	{ "TM.VacuumStatusDM"			, _K_D_IO	, VacuumStatusDM_TM			,	0	} ,
	{ "TM.RoughVvXO"				, _K_D_IO	, RoughVvXO_TM				,	0	} ,
	{ "TM.VentVvXO"					, _K_D_IO	, VentVvXO_TM				,	0	} ,
	{ "TM.ATMVACSensorXI"			, _K_D_IO	, ATMVACSensorXI			,	0	} ,

	{ "TM.CGChmPrsAI"				, _K_A_IO	, CGChmPrsAI				,	0	} ,
	{ "TM.CGPumpPrsAI"				, _K_A_IO	, CGPumpPrsAI				,	0	} ,

	{ "TMC_TM_RPUMP_CONTROL"		, _K_F_IO	, TM_PUMP_CONTROL			,	0	} ,
	{ "TMC_ISOGATE_CONTROL"			, _K_F_IO	, GATE_CONTROL				,	0	} ,
//-------------------------------------------------------------------------------------
	{ "PRMA_TM_SVENT_PRES"			, _K_A_IO	, PRMA_TM_SVENT_PRES		,	0	} ,
	{ "PRMA_TM_FVENT_PRES"			, _K_A_IO	, PRMA_TM_FVENT_PRES		,	0	} ,
	{ "PRMA_TM_SPUMP_PRES"			, _K_A_IO	, PRMA_TM_SPUMP_PRES		,	0	} ,
	{ "PRMA_TM_FPUMP_PRES"			, _K_A_IO	, PRMA_TM_FPUMP_PRES		,	0	} ,
	{ "PRMA_TM_RPUMP_PRES"			, _K_A_IO	, PRMA_TM_RPUMP_PRES		,	0	} ,

	{ "PRMD_TM_SVENT_TMO"			, _K_D_IO	, PRMD_TM_SVENT_TMO			,	0	} ,
	{ "PRMD_TM_FVENT_TMO"			, _K_D_IO	, PRMD_TM_FVENT_TMO			,	0	} ,
	{ "PRMD_TM_SPUMP_TMO"			, _K_D_IO	, PRMD_TM_SPUMP_TMO			,	0	} ,
	{ "PRMD_TM_FPUMP_TMO"			, _K_D_IO	, PRMD_TM_FPUMP_TMO			,	0	} ,
	{ "PRMD_TM_RPUMP_TMO"			, _K_D_IO	, PRMD_TM_RPUMP_TMO			,	0	} ,
	{ "PRMD_TM_OVENT_TIME"			, _K_D_IO	, PRMD_TM_OVENT_TIME		,	0	} ,
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
	{ "TM.Leak_Test_Number"			, _K_D_IO	, TM_Leak_Test_Number		,	0	} ,
	{ "TM.Leak_Remain_Time"			, _K_D_IO	, TM_Leak_Test_Time			,	0	} ,
	{ "TM.Leak_End"					, _K_D_IO	, TM_Leak_End				,	0	} ,

	{ "TM.Sys.Year"					, _K_D_IO	, TM_Year 					,	0	} ,
	{ "TM.Sys.Month"				, _K_D_IO	, TM_Month 					,	0	} ,
	{ "TM.Sys.Day"					, _K_D_IO	, TM_Day 					,	0	} ,
	{ "TM.Sys.Hour"					, _K_D_IO	, TM_Hour 					,	0	} ,
	{ "TM.Sys.Min"					, _K_D_IO	, TM_Min 					,	0	} ,
	{ "TM.Sys.Sec"					, _K_D_IO	, TM_Sec 					,	0	} ,
	
	{ "TM.Leak_Rate1"				, _K_A_IO	, TM_Leak_Rate1				,	0	} ,
	{ "TM.Leak_Rate2"				, _K_A_IO	, TM_Leak_Rate2				,	0	} ,
	{ "TM.Leak_Rate3"				, _K_A_IO	, TM_Leak_Rate3				,	0	} ,
	{ "TM.Leak_Rate4"				, _K_A_IO	, TM_Leak_Rate4				,	0	} ,
	{ "TM.Leak_Rate5"				, _K_A_IO	, TM_Leak_Rate5				,	0	} ,
	{ "TM.Leak_Rate6"				, _K_A_IO	, TM_Leak_Rate6				,	0	} ,
	{ "TM.Leak_Rate7"				, _K_A_IO	, TM_Leak_Rate7				,	0	} ,
	{ "TM.Leak_Rate8"				, _K_A_IO	, TM_Leak_Rate8				,	0	} ,
	{ "TM.Leak_Rate9"				, _K_A_IO	, TM_Leak_Rate9				,	0	} ,
	{ "TM.Leak_Rate10"				, _K_A_IO	, TM_Leak_Rate10			,	0	} ,

	{ "TM.Leak_LastRate"			, _K_A_IO	, TM_Leak_LastRate			,	0	} ,

	{ "TM.Leak_PumpingTime_H"		, _K_D_IO	, TM_Leak_PumpingTime_H		,	0	} ,
	{ "TM.Leak_PumpingTime_M"		, _K_D_IO	, TM_Leak_PumpingTime_M		,	0	} ,
	{ "TM.Leak_WaitingTime_H"		, _K_D_IO	, TM_Leak_WaitingTime_H		,	0	} ,
	{ "TM.Leak_WaitingTime_M"		, _K_D_IO	, TM_Leak_WaitingTime_M		,	0	} ,

	{ "TM.LeakChk_Message"			, _K_S_IO	, TM_LeakChk_Message		,	0	} ,
	{ "TM.Leak_PumpRemain_Time"		, _K_S_IO	, TM_Leak_PumpRemain_Time	,	0	} ,
	{ "TM.Leak_WaitRemain_Time"		, _K_S_IO	, TM_Leak_WaitRemain_Time	,	0	} ,

    { "TM.pcCHM.LeakAve"            , _K_A_IO   , TM_PC_LEAKAVE				,	0   } ,	
    { "TM.psLKCHK.LastTIme"         , _K_S_IO   , TM_PC_LKCHK_LASTTIME		,	0   } ,
    { "vTM.Logging"                 , _K_D_IO   , TM_LOGGING				,	0   } ,

	{ "TM.OverPresVvDO"				, _K_D_IO	, TM_OverPresVvDO			,	0	} ,
	// 2006.01.19
	{ "PRMD_OVER_PRES_OPT"			, _K_D_IO	, PRMD_OVER_PRES_OPT		,	0	} ,
	{ "PRMA_TM_TransPrs"				, _K_A_IO	, PRMA_TM_TransPrs		,	0	} ,
	
	{ "CTC.MAIN_CONTROL"			, _K_D_IO	,	LOT_CONTROL			    ,	0	} ,
	{ "CTC.MAIN_CONTROL2"			, _K_D_IO	,	LOT2_CONTROL			,	0	} ,
	{ "CTC.MAIN_CONTROL3"			, _K_D_IO	,	LOT3_CONTROL		    ,	0	} ,

	//Auto Leak Check Associated 2008.04.04
	{ "CTC.AutoLeakCheckSts"        , _K_D_IO	,	LeakCheckSts		    ,	0	} ,
	{ "PRMD_TM_ALC_RATE_LIMIT"      , _K_D_IO	,	LeakRateLimit		    ,	0	} ,
	{ "PRMD_TM_ALC_AVG_LIMIT"       , _K_D_IO	,	LeakAvgLimit		    ,	0	} ,

	{ "TM.LeakChk_Log_Rate1"		, _K_D_IO	, Log_Rate1					,	0	} ,
	{ "TM.LeakChk_Log_Rate2"		, _K_D_IO	, Log_Rate2					,	0	} ,
	{ "TM.LeakChk_Log_Rate3"		, _K_D_IO	, Log_Rate3					,	0	} ,
	{ "TM.LeakChk_Log_Rate4"		, _K_D_IO	, Log_Rate4					,	0	} ,
	{ "TM.LeakChk_Log_Rate5"		, _K_D_IO	, Log_Rate5					,	0	} ,
	{ "TM.LeakChk_Log_Avg1"			, _K_D_IO	, Log_Avg1					,	0	} ,
	{ "TM.LeakChk_Log_Avg2"			, _K_D_IO	, Log_Avg2					,	0	} ,
	{ "TM.LeakChk_Log_Avg3"			, _K_D_IO	, Log_Avg3					,	0	} ,
	{ "TM.LeakChk_Log_Avg4"			, _K_D_IO	, Log_Avg4					,	0	} ,
	{ "TM.LeakChk_Log_Avg5"			, _K_D_IO	, Log_Avg5					,	0	} ,
	{ "TM.LeakChk_Log_Time1"        , _K_S_IO   , Log_Time1					,	0   } ,
	{ "TM.LeakChk_Log_Time2"        , _K_S_IO   , Log_Time2					,	0   } ,
	{ "TM.LeakChk_Log_Time3"        , _K_S_IO   , Log_Time3					,	0   } ,
	{ "TM.LeakChk_Log_Time4"        , _K_S_IO   , Log_Time4					,	0   } ,
	{ "TM.LeakChk_Log_Time5"        , _K_S_IO   , Log_Time5					,	0   } ,

//-------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
	{ "FC_TM_CYCPURGE_CTRL"			, _K_D_IO	, FC_CYCLE_PURGE_TM			,   0	} ,

//	{ "PRMA_TM_CP_SVENT_PRES"		, _K_A_IO	, PRMA_TM_CP_SVENT_PRES		,	0	} ,
	{ "PRMA_TM_CP_FVENT_PRES"		, _K_A_IO	, PRMA_TM_CP_FVENT_PRES		,	0	} ,

	{ "PRMA_TM_CP_SPUMP_PRES"		, _K_A_IO	, PRMA_TM_CP_SPUMP_PRES		,	0	} ,
	{ "PRMA_TM_CP_FPUMP_PRES"		, _K_A_IO	, PRMA_TM_CP_FPUMP_PRES		,	0	} ,

	{ "PRMA_TM_CP_RPUMP_PRES"		, _K_A_IO	, PRMA_TM_CP_RPUMP_PRES		,	0	} ,

//	{ "PRMD_TM_CP_SVENT_TMO"		, _K_D_IO	, PRMD_TM_CP_SVENT_TMO		,	0	} ,
	{ "PRMD_TM_CP_FVENT_TMO"		, _K_D_IO	, PRMD_TM_CP_FVENT_TMO		,	0	} ,

	{ "PRMD_TM_CP_SPUMP_TMO"		, _K_D_IO	, PRMD_TM_CP_SPUMP_TMO		,	0	} ,
	{ "PRMD_TM_CP_FPUMP_TMO"		, _K_D_IO	, PRMD_TM_CP_FPUMP_TMO		,	0	} ,

	{ "PRMD_TM_CP_RPUMP_TMO"		, _K_D_IO	, PRMD_TM_CP_RPUMP_TMO		,	0	} ,

	{ "PRMD_TM_CP_OVENT_TIME"		, _K_D_IO	, PRMD_TM_CP_OVENT_TIME		,	0	} ,

	{ "PRMD_TM_CYCLE_PURGE_CNT"		, _K_D_IO	, PRMD_TM_CYCLE_PURGE_CNT	,	0	} ,
	{ "TM.CYCLE_PURGE_RCNT_DM"		, _K_D_IO	, TM_CYCLE_PURGE_RCNT_DM	,	0	} ,
	{ "TM.CYCLE_PURGE_STS_DM"		, _K_D_IO	, TM_CYCLE_PURGE_STS_DM		,	0	} ,
	{ "TMC_RBLOW_CONTROL"			, _K_F_IO	, TMC_RBLOW_CONTROL			,	0	} ,
	{ "PRMD_TM_ALECHK_DYN"			, _K_D_IO	, PRMD_TM_ALECHK_DYN		,	0	} ,
	//... 2016.07.14
	{ "TMC_TM_OVERPRESSURE"			, _K_F_IO	, TMC_TM_OVERPRESSURE		,	0	} ,

//------------------------------------------------------------------------------------------


	""
};

//------------------------------------------------------------------------------------------
Module_Status Wait_Pump_Pressure( int PUMP , int P_P , int T_P , int TMO , int ALARM ) {
	int Timer = 0;
	int CommStatus , AlmResult;

	// Do Pump ON
	if (RUN_FUNCTION( PUMP , "ON" ) == SYS_ABORTED ) return SYS_ABORTED;

	// Check Pressure Between P_P(Physical Pressure Channel) and T_P(Target Pressure Channel)
	if (READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus ) ) return SYS_SUCCESS;

	// Wait Until Pressure is Satisfied Between P_P(Physical Pressure) and T_P(Target Pressure)
	while (TRUE) {
		if (!WAIT_SECONDS( 0.5 )) return SYS_ABORTED;
		
		if (READ_ANALOG( P_P , &CommStatus) <= READ_ANALOG(T_P , &CommStatus ))	break;

		if (( Timer / 2 ) > READ_DIGITAL( TMO , &CommStatus )) {
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
Module_Status Pump_Wait( int P_P , int Vlv , int Vlv_Set, int T_P , int TMO , int ALARM ) {
	int Timer = 0;
	int CommStatus , AlmResult;

	// Do Vavle Operation
	WRITE_DIGITAL( Vlv , Vlv_Set , &CommStatus );

	// Check Pressure Between Pr(Physical Pressure Channel) and TP(Target Pressure Channel)
	if ( READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus ) ) {
		if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
		if ( READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus ) ) return SYS_SUCCESS;
		return SYS_SUCCESS;
	}

	// Wait Until Pressure is Satisfied Between P_P(Physical Pressure) and T_P(Target Pressure)
	while(1) {
		if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

		if ( READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus ) ) {
			if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
			if ( READ_ANALOG( P_P , &CommStatus ) <= READ_ANALOG( T_P , &CommStatus ) )	break;
		}

		if ( ( Timer / 2 ) > READ_DIGITAL( TMO , &CommStatus ) ) {
			AlmResult = ALARM_MANAGE( ALARM );
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
	
	// Check Pre_Vacuum
	if( //READ_DIGITAL( TM_OverPresVvDO  , &CommStatus ) == CO_OPEN && 
//		READ_DIGITAL( FastRoughVvDO_TM , &CommStatus ) == CO_OPEN &&
		READ_DIGITAL( RoughVvXO_TM , &CommStatus ) == ROUGH_OPENFAST &&		
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20  ) return SYS_SUCCESS;


	do
	{
		if ( READ_ANALOG( CGChmPrsAI , &CommStatus ) > READ_ANALOG( CGPumpPrsAI , &CommStatus ) )	break;			
//		if ( ( READ_DIGITAL( ATMSensorDI , &CommStatus ) == AS_ATM ) &&
//			 ( READ_DIGITAL( VACSensorDI , &CommStatus ) == VS_NOTVAC ) ) break;
		if ( READ_DIGITAL( ATMVACSensorXI , &CommStatus ) == ATM_SENSOR ) break;
		
//		if ( READ_DIGITAL( SlowRoughVvDO_TM , &CommStatus ) != CO_CLOSE ) break;
//		if ( READ_DIGITAL( FastRoughVvDO_TM , &CommStatus ) != CO_OPEN  ) break;
		if ( READ_DIGITAL( RoughVvXO_TM , &CommStatus ) != ROUGH_OPENFAST  ) break;	
		if ( READ_DIGITAL( VacuumStatusDM_TM , &CommStatus) != VS_VACUUM) break;
		return SYS_SUCCESS;
	}while(0);
	if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// Close All Gates
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE BM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE BM2" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE PM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE PM2" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE PM3" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// Slow & Fast Rough Valve Close
	WRITE_DIGITAL( TM_OverPresVvDO  , CO_CLOSE , &CommStatus );	
	WRITE_DIGITAL( VentVvXO_TM  , VENT_CLOSE , &CommStatus );

//	WRITE_DIGITAL( SlowRoughVvDO_TM , CO_CLOSE , &CommStatus );
//	WRITE_DIGITAL( FastRoughVvDO_TM , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( RoughVvXO_TM	, ROUGH_CLOSE , &CommStatus );
	if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;

	// Do Roughing Line Pump
	if ( Wait_Pump_Pressure ( TM_PUMP_CONTROL ,
		                      CGPumpPrsAI ,
							  PRMA_TM_CP_RPUMP_PRES , 
							  PRMD_TM_CP_RPUMP_TMO, 
							  ALARM_RPUMP_TIMEOUT 
							  ) == SYS_ABORTED ) return SYS_ABORTED;

	// Do Chamber Pump by Slow Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
//					SlowRoughVvDO_TM , 
					RoughVvXO_TM,
//					CO_OPEN , 
					ROUGH_OPENSLOW,
					PRMA_TM_CP_SPUMP_PRES , 
					PRMD_TM_CP_SPUMP_TMO , 
					ALARM_S_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;

	// Close Slow Rough Valve
//	WRITE_DIGITAL( SlowRoughVvDO_TM , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( RoughVvXO_TM , ROUGH_CLOSE , &CommStatus );

	// Do Chamber Pump by Fast Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
//					FastRoughVvDO_TM , 
					RoughVvXO_TM,
//					CO_OPEN , 
					ROUGH_OPENFAST,
					PRMA_TM_CP_FPUMP_PRES , 
					PRMD_TM_CP_FPUMP_TMO , 
					ALARM_F_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;
	
	return SYS_SUCCESS;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Vacuum_Pump_Sub() {
	int CommStatus;
	
	// Check Pre_Vacuum
	if (READ_DIGITAL( RoughVvXO_TM , &CommStatus ) == ROUGH_OPENFAST && 
	READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20  ) return SYS_SUCCESS;


	do {
		if (READ_ANALOG( CGChmPrsAI , &CommStatus ) > READ_ANALOG( CGPumpPrsAI , &CommStatus ) )	break;			
		if (READ_DIGITAL( ATMVACSensorXI , &CommStatus ) == ATM_SENSOR ) break;
		
		if (READ_DIGITAL( RoughVvXO_TM , &CommStatus ) != ROUGH_OPENFAST  ) break;
		if (READ_DIGITAL( VacuumStatusDM_TM , &CommStatus) != VS_VACUUM) break;
		return SYS_SUCCESS;
	}while(0);
	if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// Close All Gates
	if (RUN_FUNCTION( GATE_CONTROL  , "CLOSE BM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (RUN_FUNCTION( GATE_CONTROL  , "CLOSE BM2" ) == SYS_ABORTED ) return SYS_ABORTED;
    //2006.11.30 using PM Iso Control Function
	if (RUN_FUNCTION( GATE_CONTROL  , "CLOSE PM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (RUN_FUNCTION( GATE_CONTROL  , "CLOSE PM2" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (RUN_FUNCTION( GATE_CONTROL  , "CLOSE PM3" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (!WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// Slow & Fast Rough Valve Close
	WRITE_DIGITAL( TM_OverPresVvDO  , CO_CLOSE , &CommStatus );	
	WRITE_DIGITAL( VentVvXO_TM  , VENT_CLOSE , &CommStatus );

	WRITE_DIGITAL( RoughVvXO_TM , ROUGH_CLOSE , &CommStatus );
	if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;

	// Do Roughing Line Pump
	if ( Wait_Pump_Pressure ( TM_PUMP_CONTROL, 
		                      CGPumpPrsAI , 
							  PRMA_TM_RPUMP_PRES , 
							  PRMD_TM_RPUMP_TMO, 
							  ALARM_RPUMP_TIMEOUT 
							  ) == SYS_ABORTED ) return SYS_ABORTED;

	// Do Chamber Pump by Slow Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
					RoughVvXO_TM,
					ROUGH_OPENSLOW,
					PRMA_TM_SPUMP_PRES , 
					PRMD_TM_SPUMP_TMO , 
					ALARM_S_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;

	// Close Slow Rough Valve
	WRITE_DIGITAL( RoughVvXO_TM , ROUGH_CLOSE , &CommStatus );

	// Do Chamber Pump by Fast Rough Valve
	if ( Pump_Wait( CGChmPrsAI , 
					RoughVvXO_TM , 
					ROUGH_OPENFAST , 
					PRMA_TM_FPUMP_PRES , 
					PRMD_TM_FPUMP_TMO , 
					ALARM_F_PUMP_TIMEOUT 
					) == SYS_ABORTED ) return SYS_ABORTED;
	
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Pump() {
	int CommStatus;
	Module_Status res;

    char szLog[256];
	// 2006.01.19
	BOOL bOverPrs = FALSE;
	
	// Log Added For Check MCC Time
	sprintf(szLog, "TM Pump Started");
	_LOG(szLog);
	// Log Added For Check MCC Time

	// 2006.01.19
	if(READ_DIGITAL(PRMD_OVER_PRES_OPT, &CommStatus) == 1) bOverPrs = TRUE;

	WRITE_DIGITAL( TM_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_PUMP, &CommStatus);
	WRITE_DIGITAL( VacuumStatusDM_TM , VS_GOVAC , &CommStatus );        		

	// Lock TM
	WRITE_DIGITAL( LockPumpDM_TM , ULL_LOCK , &CommStatus );

	// Check Pre_Vacuum
//	if( READ_DIGITAL( FastRoughVvDO_TM , &CommStatus ) == CO_OPEN && 
	if( READ_DIGITAL( RoughVvXO_TM , &CommStatus ) == ROUGH_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		     READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		     READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
			if(bOverPrs) WRITE_DIGITAL( TM_OverPresVvDO , CO_OPEN , &CommStatus );
		}
		WRITE_DIGITAL( VacuumStatusDM_TM , VS_VACUUM , &CommStatus );
		return SYS_SUCCESS;
	}

	// Do Pumping
	res = CyclePurge_Pump_Sub();

	// Check Post_Vacuum
//	if( READ_DIGITAL( FastRoughVvDO_TM , &CommStatus ) == CO_OPEN && 
	if( READ_DIGITAL( RoughVvXO_TM , &CommStatus ) == ROUGH_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		     READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		     READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
			if(bOverPrs) WRITE_DIGITAL( TM_OverPresVvDO , CO_OPEN , &CommStatus );
		}
	}

	// Check Return Value of Vacuum_Pump_Sub Function
	if ( SYS_ABORTED == res ) 
	{
		//WRITE_DIGITAL( SlowRoughVvDO_TM , CO_CLOSE , &CommStatus );
		//WRITE_DIGITAL( FastRoughVvDO_TM , CO_CLOSE , &CommStatus );

		WRITE_DIGITAL( VacuumStatusDM_TM , VS_ABORT , &CommStatus );
	}
	else
	{
		WRITE_DIGITAL( VacuumStatusDM_TM , VS_VACUUM , &CommStatus );
	}

	// Unlock TM
	WRITE_DIGITAL( LockPumpDM_TM , ULL_UNLOCK , &CommStatus );

	// Log Added For Check MCC Time
	sprintf(szLog, "TM Pump Completed");
	_LOG(szLog);
	// Log Added For Check MCC Time
	return res;
}


//------------------------------------------------------------------------------------------
Module_Status Vacuum_Pump() {
	int CommStatus;
	Module_Status res;
	// EVAC_STATUS eVacMode;
	// int old_scv , old_fcv , old_scp , old_fcp;
    char szLog[256];
	// 2006.01.19
	BOOL bOverPrs = FALSE;
	
	// Log Added For Check MCC Time
	sprintf(szLog, "TM Pump Started");
	_LOG(szLog);
	// Log Added For Check MCC Time

	// 2006.01.19
	if(READ_DIGITAL(PRMD_OVER_PRES_OPT, &CommStatus) == 1) bOverPrs = TRUE;

	WRITE_DIGITAL( VacuumStatusDM_TM , VS_GOVAC , &CommStatus );        		

	// Lock TM
	WRITE_DIGITAL( LockPumpDM_TM , ULL_LOCK , &CommStatus );

	// Check Pre_Vacuum
	if( READ_DIGITAL( RoughVvXO_TM , &CommStatus ) == ROUGH_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		     READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		     READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
			if(bOverPrs) WRITE_DIGITAL( TM_OverPresVvDO , CO_OPEN , &CommStatus );
		}
		WRITE_DIGITAL( VacuumStatusDM_TM , VS_VACUUM , &CommStatus );
		return SYS_SUCCESS;
	}

	// Do Pumping
	res = Vacuum_Pump_Sub();

	// Check Post_Vacuum
	if( READ_DIGITAL( RoughVvXO_TM , &CommStatus ) == ROUGH_OPENFAST && 
		READ_ANALOG( CGChmPrsAI , &CommStatus ) < 20 ) {
		if ( READ_DIGITAL( LOT_CONTROL  , &CommStatus ) ==  LS_RUNNING ||
		     READ_DIGITAL( LOT2_CONTROL  , &CommStatus ) == LS_RUNNING || 
		     READ_DIGITAL( LOT3_CONTROL  , &CommStatus ) == LS_RUNNING ) {
			if(bOverPrs) WRITE_DIGITAL( TM_OverPresVvDO , CO_OPEN , &CommStatus );
		}
	}

	// Check Return Value of Vacuum_Pump_Sub Function
	if (SYS_ABORTED == res) {
		WRITE_DIGITAL( VacuumStatusDM_TM , VS_ABORT , &CommStatus );
	} else {
		WRITE_DIGITAL( VacuumStatusDM_TM , VS_VACUUM , &CommStatus );
	}

	// Unlock TM
	WRITE_DIGITAL( LockPumpDM_TM , ULL_UNLOCK , &CommStatus );

	// Log Added For Check MCC Time
	sprintf(szLog, "TM Pump Cpmpleted");
	_LOG(szLog);
	// Log Added For Check MCC Time
	return res;
}

//------------------------------------------------------------------------------------------
//Module_Status Vent_Wait( int P_P , int Vlv , int Vlv_Set , int T_P , int TMO , int ALARM , int ATMS , int VAC ) {
Module_Status Vent_Wait( int P_P , int Vlv , int Vlv_Set , int T_P , int TMO , int ALARM , int ATMVACSensor, EVENT_CMD eVentCmd) {
	int Timer = 0;
	int CommStatus , AlmResult;

	// Do Vent Valve Operation
	WRITE_DIGITAL( Vlv , Vlv_Set , &CommStatus );

	// Check Chamber Pressure for Vent
	while (1) {
		if (VC_SVENT == eVentCmd ) {
			if ( READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) {
				if (!WAIT_SECONDS( 0.5 ) ) {
					WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
					return SYS_ABORTED;
				}
				if (READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) break;
			}
		} else {
			if (( READ_DIGITAL( ATMVACSensor , &CommStatus ) == ATM_SENSOR ) &&
				 ( READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) ) {
				if (!WAIT_SECONDS( 0.5 ) ) {
					WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
					return SYS_ABORTED;
				}
				if (READ_ANALOG( P_P , &CommStatus ) >= READ_ANALOG( T_P , &CommStatus ) ) break;
			}
		}

		if (!WAIT_SECONDS( 0.5 ) ) {
			WRITE_DIGITAL( Vlv , CO_CLOSE , &CommStatus );
			return SYS_ABORTED;
		}

		if (( Timer / 2 ) > READ_DIGITAL( TMO , &CommStatus ) ) {
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
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Vent_Sub() {
	int CommStatus;
	// int OverVent_Time;
	double Pressure , Target;

	// Check whether Chamber is Vent or not
	Pressure  = READ_ANALOG( CGChmPrsAI		 , &CommStatus );
	Target = READ_ANALOG( PRMA_TM_CP_FVENT_PRES , &CommStatus );
	if ( (READ_DIGITAL( ATMVACSensorXI , &CommStatus ) == ATM_SENSOR) && (Pressure >= Target) ) return SYS_SUCCESS;

	// Close All Gates
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE BM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE BM2" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE PM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE PM2" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( RUN_FUNCTION( GATE_CONTROL , "CLOSE PM3" ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// Close Slow & Fast Rough Valve
//	WRITE_DIGITAL( SlowRoughVvDO_TM , CO_CLOSE , &CommStatus );
//	WRITE_DIGITAL( FastRoughVvDO_TM , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( RoughVvXO_TM , ROUGH_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_TM  , VENT_CLOSE , &CommStatus );
	if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;

	// Do Fast Vent
	if ( CyclePurge_Vent_Wait( CGChmPrsAI , 
								VentVvXO_TM , 
								VENT_OPENFAST , 
								PRMA_TM_CP_FVENT_PRES , 
								PRMD_TM_CP_FVENT_TMO , 
								ALARM_F_VENT_TIMEOUT , 
			//					ATMSensorDI,
			//					VACSensorDI
								ATMVACSensorXI,
								VC_FVENT
								) == SYS_ABORTED ) return SYS_ABORTED;

	WRITE_DIGITAL( VentVvXO_TM , VENT_CLOSE , &CommStatus );

	// Do Over Vent
	if ( !WAIT_SECONDS( READ_DIGITAL( PRMD_TM_CP_OVENT_TIME , &CommStatus ) ) ) {
		return SYS_ABORTED;
	}

	return SYS_SUCCESS;
}


//------------------------------------------------------------------------------------------
Module_Status Vacuum_Vent_Sub() {
	int CommStatus;
	// int OverVent_Time;
	double Pressure , Target;

	// Check whether Chamber is Vent or not
	Pressure  = READ_ANALOG( CGChmPrsAI		 , &CommStatus );
	Target = READ_ANALOG( PRMA_TM_FVENT_PRES , &CommStatus );
	if ((READ_DIGITAL( ATMVACSensorXI , &CommStatus ) == ATM_SENSOR) && (Pressure >= Target) ) return SYS_SUCCESS;

	// Close All Gates
	if (RUN_FUNCTION( GATE_CONTROL , "CLOSE BM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (RUN_FUNCTION( GATE_CONTROL , "CLOSE BM2" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (RUN_FUNCTION( GATE_CONTROL , "CLOSE PM1" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (RUN_FUNCTION( GATE_CONTROL , "CLOSE PM2" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (RUN_FUNCTION( GATE_CONTROL , "CLOSE PM3" ) == SYS_ABORTED ) return SYS_ABORTED;
	if (!WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;

	// Close Slow & Fast Rough Valve
	WRITE_DIGITAL( RoughVvXO_TM , ROUGH_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_TM  , VENT_CLOSE , &CommStatus );
	if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;

	// MAHA BB Modify 2014.03.14 by Hyuk
	// Do Slow Vent
	if ( Vent_Wait( CGChmPrsAI , 
					VentVvXO_TM , 
					VENT_OPENSLOW , 
					PRMA_TM_SVENT_PRES , 
					PRMD_TM_SVENT_TMO , 
					ALARM_S_VENT_TIMEOUT , 
					ATMVACSensorXI,
					VC_SVENT
					) == SYS_ABORTED ) return SYS_ABORTED;
	
	WRITE_DIGITAL( VentVvXO_TM  , VENT_CLOSE , &CommStatus );


	// Do Fast Vent
	if ( Vent_Wait( CGChmPrsAI , 
					VentVvXO_TM , 
					VENT_OPENFAST , 
					PRMA_TM_FVENT_PRES , 
					PRMD_TM_FVENT_TMO , 
					ALARM_F_VENT_TIMEOUT , 
					ATMVACSensorXI,
					VC_FVENT
					) == SYS_ABORTED ) return SYS_ABORTED;

	WRITE_DIGITAL( VentVvXO_TM , VENT_CLOSE , &CommStatus );
	// MAHA BB Modify 2014.03.14 by Hyuk

	// Do Over Vent
	if ( !WAIT_SECONDS( READ_DIGITAL( PRMD_TM_OVENT_TIME , &CommStatus ) ) ) {
		return SYS_ABORTED;
	}

	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status CyclePurge_Vent() {
	int CommStatus;
	Module_Status res;
	char szLog[256];
	// Log Added For Check MCC Time
	sprintf(szLog, "CyclePurge Vent Started");
	_LOG(szLog);
	// Log Added For Check MCC Time

	// Close overpressure valve
    WRITE_DIGITAL( TM_OverPresVvDO , CO_CLOSE , &CommStatus );	

	// Set TM Vent Status
	WRITE_DIGITAL( TM_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_VENT, &CommStatus);
    WRITE_DIGITAL( VacuumStatusDM_TM , VS_GOATM , &CommStatus );	

	// Do Venting
	res = CyclePurge_Vent_Sub();

	// Close Slow & Fast Vent Valve
//	WRITE_DIGITAL( SlowVentVvDO_TM , CO_CLOSE , &CommStatus );
	WRITE_DIGITAL( VentVvXO_TM , VENT_CLOSE , &CommStatus );

	// Check Return Value of Vacuum_Vent_Sub Function
	if ( SYS_ABORTED == res ) 
	{
		WRITE_DIGITAL( VacuumStatusDM_TM , VS_ABORT , &CommStatus );
	}
	else
	{
	    WRITE_DIGITAL( VacuumStatusDM_TM , VS_ATM , &CommStatus );	
	}

	// Log Added For Check MCC Time
	sprintf(szLog, "CyclePurge Vent Completed");
	_LOG(szLog);
	// Log Added For Check MCC Time

	return res;
}



//------------------------------------------------------------------------------------------
Module_Status Vacuum_Vent() {
	int CommStatus;
	Module_Status res;
	char szLog[256];
	// Log Added For Check MCC Time
	sprintf(szLog, "TM Vent Started");
	_LOG(szLog);
	// Log Added For Check MCC Time

	// Close overpressure valve
    WRITE_DIGITAL( TM_OverPresVvDO , CO_CLOSE , &CommStatus );	

	// Set TM Vent Status
    WRITE_DIGITAL( VacuumStatusDM_TM , VS_GOATM , &CommStatus );	

	// Do Venting
	res = Vacuum_Vent_Sub();

	// Close Slow & Fast Vent Valve
	WRITE_DIGITAL( VentVvXO_TM , VENT_CLOSE , &CommStatus );

	// Check Return Value of Vacuum_Vent_Sub Function
	if (SYS_ABORTED == res) {
		WRITE_DIGITAL( VacuumStatusDM_TM , VS_ABORT , &CommStatus );
	} else {
	    WRITE_DIGITAL( VacuumStatusDM_TM , VS_ATM , &CommStatus );	
	}

	// Log Added For Check MCC Time
	sprintf(szLog, "TM Vent Completed");
	_LOG(szLog);
	// Log Added For Check MCC Time

	return res;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void saveLkAvePrs(const double dData)
{
	int nComSts = 0;
    WRITE_DIGITAL(TM_LOGGING, NY_NO, &nComSts);
	WRITE_ANALOG(TM_PC_LEAKAVE, dData, &nComSts);
	WRITE_DIGITAL(TM_LOGGING, NY_YES, &nComSts);
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void saveIO()
{
	int nComSts = 0;
    WRITE_DIGITAL(TM_LOGGING, NY_NO, &nComSts);
	WRITE_DIGITAL(TM_LOGGING, NY_YES, &nComSts);
}
//------------------------------------------------------------------------------------------
void saveLkChkTime(void)
{
	char buf[80];
	SYSTEMTIME time;
	int nComSts = 0;

	GetLocalTime(&time);
	sprintf(buf, "%d/%02d/%02d  %d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
    WRITE_DIGITAL(TM_LOGGING, NY_NO, &nComSts);
	WRITE_STRING(TM_PC_LKCHK_LASTTIME, buf, &nComSts);
	WRITE_DIGITAL(TM_LOGGING, NY_YES, &nComSts);
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

//---------------------------------------------------------------------------------------
Module_Status Vacuum_Leak() {
	int CommStatus , CurrCount = 1 , i , Time , AlmResult;
	char Buffer_Str[1025];
	char TM_Time[1024];
	double Start_Press , Start_Press1 , Current_Press , Result;
	int nHour = 0, nMin = 0, nSec = 0, nTemp = 0;
	BOOL bSuccess;
	int  nLastRate , nAvgRate;

	//2008.04.04
	while (1) {

		bSuccess = TRUE;

	// Close overpressure valve
    WRITE_DIGITAL( TM_OverPresVvDO , CO_CLOSE , &CommStatus );	

	//////////////////////////////////////////////////////////////////
	// Begin Initialize
	WRITE_DIGITAL( TM_Leak_End , OFFON_OFF , &CommStatus );
	if ( READ_DIGITAL( TM_Leak_Test_Number , &CommStatus ) == 0 ) {
		sprintf( Buffer_Str , "%s Leak Test count check...", NAME_CHAMBER);
		WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
		return SYS_SUCCESS;
	}

	// Set zero to each Leak Rate Channel
	for ( i = 0 ; i < 11 ; i++ ) WRITE_ANALOG ( TM_Leak_Rate1 + i , 0 , &CommStatus );
	saveIO();

	// Set Time zero to Leak Wait Remain Time Channel
	WRITE_STRING( TM_Leak_WaitRemain_Time , "00:00:00" , &CommStatus );

	// Set Time zero to Leak Pump Remain Time Channel
	WRITE_STRING( TM_Leak_PumpRemain_Time , "00:00:00" , &CommStatus );
	// End Initialize
	//////////////////////////////////////////////////////////////////

	// Start Pumping
	sprintf( Buffer_Str , "%s Pumping Start...", NAME_CHAMBER);
	WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
	if ( Vacuum_Pump() == SYS_ABORTED ) return SYS_ABORTED;

	//////////////////////////////////////////////////////////////////
	// Begin Continued Pump
	Time = READ_DIGITAL( TM_Leak_PumpingTime_H , &CommStatus )*3600 + READ_DIGITAL( TM_Leak_PumpingTime_M , &CommStatus )*60;
	sprintf( Buffer_Str , "%s Pumping continue time..." , NAME_CHAMBER);
	WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
	S_TIMER_READY();
	while ( TRUE ) {
		nHour = (int)( Time - S_TIMER_ELAPSED() ) / 3600;
		nTemp = (int)( Time - S_TIMER_ELAPSED() ) % 3600;
		nMin  = nTemp / 60;
		nSec  = nTemp % 60;
		sprintf( TM_Time , "%02d:%02d:%02d" , nHour , nMin , nSec );
		WRITE_STRING( TM_Leak_PumpRemain_Time , TM_Time , &CommStatus );
		if		( S_TIMER_REACH( Time ) ) break;
		else if ( !WAIT_SECONDS ( 0.1 ) ) return SYS_ABORTED;
	}
	WRITE_DIGITAL( RoughVvXO_TM , ROUGH_CLOSE , &CommStatus );
	// End Continued Pump
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// Begin Wait Time
	Time = READ_DIGITAL( TM_Leak_WaitingTime_H , &CommStatus )*3600 + READ_DIGITAL( TM_Leak_WaitingTime_M , &CommStatus )*60;
	sprintf( Buffer_Str , "%s Leak Test Wait time..." , NAME_CHAMBER);
	WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
	S_TIMER_READY();
	while ( TRUE ) {
		nHour = (int)( Time - S_TIMER_ELAPSED() ) / 3600;
		nTemp = (int)( Time - S_TIMER_ELAPSED() ) % 3600;
		nMin  = nTemp / 60;
		nSec  = nTemp % 60;
		sprintf( TM_Time , "%02d:%02d:%02d" , nHour , nMin , nSec );
		WRITE_STRING( TM_Leak_WaitRemain_Time , TM_Time , &CommStatus );
		if		( S_TIMER_REACH( Time ) ) break;
		else if ( !WAIT_SECONDS ( 0.1 ) ) return SYS_ABORTED;
	}
	// End Wait Time
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// Start Calibration

	// Set Leak Check Date & Time
	sprintf( Buffer_Str , "%s Calibration Start..." , NAME_CHAMBER);
	WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
	_getsystime( &Pres_Time );
	WRITE_DIGITAL( TM_Year  , Pres_Time.tm_year+1900 , &CommStatus );
	WRITE_DIGITAL( TM_Month , Pres_Time.tm_mon + 1   , &CommStatus );
	WRITE_DIGITAL( TM_Day   , Pres_Time.tm_mday      , &CommStatus );
	WRITE_DIGITAL( TM_Hour  , Pres_Time.tm_hour      , &CommStatus );
	WRITE_DIGITAL( TM_Min   , Pres_Time.tm_min       , &CommStatus );
	WRITE_DIGITAL( TM_Sec   , Pres_Time.tm_sec       , &CommStatus );
	
	//...2016.04.12 Dynamic Leak Check Start
	if(1/*ON*/ == READ_DIGITAL(PRMD_TM_ALECHK_DYN,&CommStatus))
	{
		if (!WAIT_SECONDS(1)) return SYS_ABORTED;
		printf("LeakCheck : TM Robot Rotate Start.\n");
		RUN_SET_FUNCTION(TMC_RBLOW_CONTROL,"CYLROTATE");
	}

	// Check Leak
	for ( CurrCount = 0 ; CurrCount < 10 ; CurrCount++ ) {
		S_TIMER_READY();
		Start_Press1 = READ_ANALOG( CGChmPrsAI , &CommStatus );
		if( CurrCount == 0 ) Start_Press = Start_Press1;

		sprintf( Buffer_Str , "%s %d Step Leak Test..." , NAME_CHAMBER, CurrCount+1 );
		WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
		while ( TRUE ) {
			// If User click Stop Leak Check on screen, TM_Leak_End is changed OFFON_ON !!!
			if ( READ_DIGITAL( TM_Leak_End , &CommStatus ) == OFFON_ON ) {
				Time = (int)( 60 - ( (int)S_TIMER_ELAPSED() % 60 ) );
				WRITE_DIGITAL( TM_Leak_Test_Time , Time , &CommStatus );
			}
			else {
				Time = (int)( ( 60 * READ_DIGITAL( TM_Leak_Test_Number , &CommStatus ) ) -
					          ( S_TIMER_ELAPSED() + (CurrCount*60 ) ) );
				WRITE_DIGITAL( TM_Leak_Test_Time , Time , &CommStatus );
			}
			if		( S_TIMER_REACH( 60 ) ) break;
			else if ( !WAIT_SECONDS ( 0.1 ) ) return SYS_ABORTED;
		}

		Current_Press = READ_ANALOG( CGChmPrsAI , &CommStatus );

		Result = Current_Press - Start_Press1 ;
        if ( Result < 0.0 ) Result = 0.0;
		WRITE_ANALOG( TM_Leak_Rate1 + CurrCount , Result*1000 , &CommStatus );
		saveIO();

			//2008.04.04 Auto Leak Check & Leak check alarm post
			nLastRate = (int)(Result * 1000);
			if (nLastRate < 0) nLastRate = 0;
			if ((Result * 1000) > READ_DIGITAL(LeakRateLimit , &CommStatus)) { bSuccess = FALSE; break;}		


			if ( READ_DIGITAL( TM_Leak_Test_Number , &CommStatus ) == CurrCount+1 ) break;
			if ( READ_DIGITAL( TM_Leak_End , &CommStatus ) == OFFON_ON ) {
				WRITE_DIGITAL( TM_Leak_End , OFFON_OFF , &CommStatus );
				break;
			}
		}

		//... 2016.04.12. Wait TM Dynamic Leak Check (TM Robot Rotate) 
		if(1/*ON*/ == READ_DIGITAL(PRMD_TM_ALECHK_DYN,&CommStatus))
		{
			if (SYS_RUNNING == READ_FUNCTION(TMC_RBLOW_CONTROL))
			{
				RUN_FUNCTION_ABORT(TMC_RBLOW_CONTROL);
				Sleep(6000);
				RUN_FUNCTION_FREE(TMC_RBLOW_CONTROL,"HOME");
			}
		}

		//2008.04.04 Auto Leak Check & Leak check alarm post
		if (FALSE == bSuccess) {
			//Leak Rate Alarm Post
			AlmResult = ALARM_MANAGE(ALARM_LEAK_RATE_ERR);
			if (AlmResult != ALM_RETRY)
			{
				// 2012.05.08 History Save When Abnormal Abort...
				Result = ( ( Current_Press - Start_Press ) / (CurrCount+1) );
				WRITE_ANALOG( TM_Leak_LastRate , Result*1000 , &CommStatus );
				saveIO();
				nAvgRate = (int)(Result * 1000);
				if (nAvgRate < 0) nAvgRate = 0;
				SaveLeakCheckHistory(nLastRate , nAvgRate);
				saveLkAvePrs(Result*1000);
				saveLkChkTime();
				sprintf( Buffer_Str , "%s Leak Test Abnormal Abort..." , NAME_CHAMBER );
				WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );

				return SYS_ABORTED;
			}
			else continue;
		}
		//2008.04.04

		Result = ( ( Current_Press - Start_Press ) / (CurrCount+1) );
		WRITE_ANALOG( TM_Leak_LastRate , Result*1000 , &CommStatus );
		saveIO();
		nAvgRate = (int)(Result * 1000);
		if (nAvgRate < 0) nAvgRate = 0;

		//2008.04.04 Auto Leak Check & Leak check alarm post
		if ((Result * 1000) > READ_DIGITAL(LeakAvgLimit , &CommStatus)) {
			//Leak Rate Alarm Post
			AlmResult = ALARM_MANAGE(ALARM_LEAK_AVG_ERR);
			if (AlmResult != ALM_RETRY)
			{
				// 2012.05.08 History Save When Abnormal Abort...
				SaveLeakCheckHistory(nLastRate , nAvgRate);
				saveLkAvePrs(Result*1000);
				saveLkChkTime();
				sprintf( Buffer_Str , "%s Leak Test Abnormal Abort..." , NAME_CHAMBER );
				WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
				return SYS_ABORTED;
			}
			else continue;
		}

		SaveLeakCheckHistory(nLastRate , nAvgRate);
		//2008.04.04

		saveLkAvePrs(Result*1000);
		saveLkChkTime();
		sprintf( Buffer_Str , "%s Leak Test Success End..." , NAME_CHAMBER );
		WRITE_STRING( TM_LeakChk_Message , Buffer_Str , &CommStatus );
		WRITE_DIGITAL( RoughVvXO_TM , ROUGH_OPENFAST , &CommStatus );
		break;
	}
	//...2016.07.14
	RUN_FUNCTION_FREE(TMC_TM_OVERPRESSURE,"OPEN_OPRES_VALVE");
	return SYS_SUCCESS;
}

// 2006.01.19
Module_Status CheckTransPres()
{
back:
	Module_Status msRet = SYS_SUCCESS;
	BOOL bIOStatus;
	double dbChmPrs;
	double dbTrPrs;
	int i;
	int nAlarmRecovery;

	char szAlarmDesc[256] = {"",};	
	
	memset(g_szAlarmMessage,0,sizeof(g_szAlarmMessage));
	dbTrPrs = READ_ANALOG(PRMA_TM_TransPrs, &bIOStatus);
	for(i=0; i<5; i++)
	{
		dbChmPrs = READ_ANALOG(CGChmPrsAI, &bIOStatus);
		if(	dbChmPrs <= dbTrPrs ) break;
		if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
	}
	if(i >= 5)
	{
		_LOG(" CGChmPrsAI : %f , PRMA_TM_TransPrs : %f ", dbChmPrs, dbTrPrs);
		sprintf(g_szAlarmMessage,"[TM Prs:%0.3f Limit:%0.3f]",dbChmPrs,dbTrPrs); //... 2015.09.11
		msRet = SYS_ABORTED;
	}
	if(msRet == SYS_ABORTED)
	{
			
		ALARM_MESSAGE_GET(ALARM_TM_TRPRS_OVER, szAlarmDesc );
		strcat( szAlarmDesc , g_szAlarmMessage );
		ALARM_MESSAGE_SET(ALARM_TM_TRPRS_OVER, szAlarmDesc);
				
		nAlarmRecovery = ALARM_MANAGE(ALARM_TM_TRPRS_OVER);
		
		if(nAlarmRecovery == ALM_RETRY)
		{
			if(! WAIT_SECONDS(2))		return SYS_ABORTED;
			goto back;	//... RETRY
		}
	}
	
	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2008.11.17 by jonghyuk (Cycle Purge)
Module_Status ExecuteCyclePurge()
{
	Module_Status msRet = SYS_SUCCESS;
	int nTargetCycleCount;
	int	CommStatus, step;

	nTargetCycleCount = READ_DIGITAL(PRMD_TM_CYCLE_PURGE_CNT, &CommStatus);
	if(nTargetCycleCount < 0 ) nTargetCycleCount = 0;

	// Cycle Purge 횟수만큼 Purge실시
	for(step=0; step<nTargetCycleCount; step++)
	{
printf("----->TM Cycle Purge Run Count : %d\n", step+1);
		WRITE_DIGITAL(TM_CYCLE_PURGE_RCNT_DM, step+1, &CommStatus);

		if(CyclePurge_Pump() == SYS_ABORTED)	{ msRet = SYS_ABORTED; break;	}
		if(CyclePurge_Vent() == SYS_ABORTED)	{ msRet = SYS_ABORTED; break;	}
	}

	// 정상적으로 cycle purge가 완료 되면, Vacuum상태로 변화시킨다.
	if(msRet == SYS_SUCCESS)	{			
		WRITE_DIGITAL( TM_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_PUMP, &CommStatus);
printf("----->TM Cycle Purge Success :: Last Pump\n");
		msRet = Vacuum_Pump();	
	}
	
	WRITE_DIGITAL( TM_CYCLE_PURGE_STS_DM, CYCLE_PURGE_STATE_STOP, &CommStatus);

	return msRet;
}


//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	Dl , CommStatus;
	char *Ed;
	char RunStr[21] , TempStr[21];
	Module_Status msRet = SYS_SUCCESS;

	if ( READ_DIGITAL( FC_Vacuum_TM , &CommStatus ) == ONOFF_OFF ) return SYS_SUCCESS;

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( Dl < 1 ) {
		printf( "---> TMC Vacuum Control Parameter Error\n" );
		return SYS_ABORTED;
	}

	STR_SEPERATE( Ed , RunStr , TempStr , 20 );

	// 2008.11.17 by jonghyuk (Cycle Purge)
	if	( STRCMP_L( RunStr , "CYCLE_PURGE" ) ){
		if ( READ_DIGITAL( FC_CYCLE_PURGE_TM , &CommStatus ) == ONOFF_OFF ) return SYS_SUCCESS;

		msRet = ExecuteCyclePurge();
		_LOG("-----> Finished (%s) in TM CyclePurgeFunc", Ed);
		return msRet;
	}

	if ( READ_DIGITAL( FC_Vacuum_TM , &CommStatus ) == ONOFF_OFF ) return SYS_SUCCESS;	

	if		( STRCMP_L( RunStr , "VENT"        ) ) return( Vacuum_Vent() );
	else if ( STRCMP_L( RunStr , "PUMP"        ) ) return( Vacuum_Pump() );
	else if ( STRCMP_L( RunStr , "PUMP_ALC"    ) ) {
		//2008.04.04 Vent Skip When Auto Leak Check Running...
		if (2/*AutoLeak Running*/ == READ_DIGITAL(LeakCheckSts , &CommStatus)) 
			return SYS_SUCCESS;
		else
			return ( Vacuum_Pump() );
	}
	else if ( STRCMP_L( RunStr , "LEAKCHECK"   ) ) return( Vacuum_Leak() );	
	else if ( STRCMP_L( RunStr , "READY"   ) ) return( Vacuum_Pump() );	
	// 2006.01.19
	else if ( STRCMP_L( RunStr , "CHECK_TR_PRS"   ) ) return( CheckTransPres() );	
	else {
		printf( "---> TMC Vacuum Check Error - Not Support Code %s\n" , Ed );
		return SYS_ABORTED;
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	
	//2008.04.04 leak check history
	if (FALSE == InitHistory()) return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		_gCLOSE_LOG();
	}
}


} // End of [extern "C"]
//==========================================================================================
