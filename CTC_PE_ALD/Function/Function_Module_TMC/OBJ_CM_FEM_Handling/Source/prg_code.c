 #include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "Selfcure.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE	= FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int ALARM_START_POINTER = 101;
int CASSETTE_INDEX = 1;
//------------------------------------------------------------------------------------------
#define	ALARM_NOTCARRIER		0
#define	ALARM_CARRIER			1
#define	ALARM_EXTEND			2
#define	ALARM_RETRACT			3
#define	ALARM_CLOSE				4
#define	ALARM_OPEN				5
#define	ALARM_DOORDOWN			6
#define	ALARM_DOORUP			7
#define	ALARM_DOCKING			8
#define	ALARM_UNDOCKING			9
#define	ALARM_CLAMP				10
#define	ALARM_UNCLAMP			11
#define	ALARM_SLIDE				12
#define	ALARM_TMO				13
#define ALARM_CROSS				200
#define ALARM_DOUBLE			230
#define ALARM_N2FRONTABORTTOL	131
#define ALARM_N2REARABORTTOL	132
#define ALARM_N2PURGERATEFAIL	142
#define ALARM_N2POINTER			300		

enum {NO, YES}; // 2004.08.27 swhan for DCOP Initialize 

//------------------------------------------------------------------------------------------
#define	ALARM_COMM_ERROR		19
#define	ALARM_MAP_WAFER			20
//------------------------------------------------------------------------------------------
char *EVENT_STRING_MOVINGIN		= "EVENT_ON FOUP_MOVING PORT%d|1";
char *EVENT_STRING_MOVINGOUT	= "EVENT_ON FOUP_MOVING PORT%d|2";
//
char *EVENT_STRING_DOCKING		= "EVENT_ON LL_DOCKING_COMPLETE PORT%d";
char *EVENT_STRING_UNDOCKING	= "EVENT_ON LL_UNDOCKING_COMPLETE PORT%d";
//
char *EVENT_STRING_DOOR_OPEN	= "EVENT_ON LL_OPEN_COMPLETE PORT%d";
char *EVENT_STRING_DOOR_CLOSE	= "EVENT_ON LL_CLOSE_COMPLETE PORT%d";
//
char *EVENT_UNLOAD_REQUEST		= "EVENT_ON UNLOAD_REQUEST_F PORT%d";

//------------------------------------------------------------------------------------------
char *EVENT_STRING_MAPREQ_IN		= "EVENT_ON MAPPING_F PORT%d|0";
char *EVENT_STRING_MAPCOMPLETE_IN	= "EVENT_ON MAPPING_COMPLETE_F PORT%d|0";
char *EVENT_STRING_AT_SOURCE_IN		= "EVENT_ON STS_ATSOURCE_F PORT%d|0";
char *EVENT_STRING_AT_OCCUPY_IN		= "EVENT_ON STS_OCCUPIED_F PORT%d|0";
//
char *EVENT_STRING_MAPREQ_MAN		= "EVENT_ON MAPPING_F PORT%d|1";
char *EVENT_STRING_MAPCOMPLETE_MAN	= "EVENT_ON MAPPING_COMPLETE_F PORT%d|1";
char *EVENT_STRING_AT_SOURCE_MAN	= "EVENT_ON STS_ATSOURCE_F PORT%d|1";
char *EVENT_STRING_AT_OCCUPY_MAN	= "EVENT_ON STS_OCCUPIED_F PORT%d|1";
//
char *EVENT_STRING_MAPREQ_OUT		= "EVENT_ON MAPPING_F PORT%d|2";
char *EVENT_STRING_MAPCOMPLETE_OUT	= "EVENT_ON MAPPING_COMPLETE_F PORT%d|2";
//------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,		IO_Driver_Loding		,	0	} ,
	{ "CTC_EVENT_LOG_SVR"		, _K_F_IO	,		CTC_EVENT_LOG_SVR	,	0	} ,		// 2013.11.15
	{ "e:SCHEDULER"				, _K_S_IO	,		SCHEDULER				,	0	} ,
	{ "FC_FM$1_CONTROL"			, _K_D_IO	,		CONTROL					,	0	} ,
	{ "TMC_$1_MID_CONTROL"		, _K_F_IO	,		MID_FUNCTION			,	0	} ,
	{ "PRM_FM$1_CLAMP_MODE"		, _K_D_IO	,		CLAMP_MODE				,	0	} , // Clamp , Unclamp
	{ "PRM_FM$1_TIMEOUT"		, _K_D_IO	,		TIMEOUT					,	0	} ,
	{ "$0.PaddleExtRetXI"		, _K_D_IO	,		LPM_PaddleExtRetXI		,	0	} ,
	{ "$0.LatchOpenCloseXI"		, _K_D_IO	,		LPM_LatchOpenCloseXI	,	0	} ,
	{ "$0.FOUPDoor"				, _K_D_IO	,		LPM_FOUPDoor			,	0	} ,
	{ "$0.DoorUpDownXI"			, _K_D_IO	,		LPM_DoorUpDownXI		,	0	} ,
	{ "$0.ShuttleInOutXI"		, _K_D_IO	,		LPM_ShuttleInOutXI		,	0	} ,
	{ "$0.ClampUnclampXI"		, _K_D_IO	,		LPM_ClampUnclampXI		,	0	} ,
	{ "$0.VacuumOKDI"			, _K_D_IO	,		LPM_VacuumOK			,	0	} ,
	{ "$0.ClampUnclampXO"		, _K_D_IO	,		LPM_ClampUnclampXO		,	0	},
	{ "$0.DockDO"				, _K_D_IO	,		LPM_DockDO				,	0	},
	{ "$0.UndockDO"				, _K_D_IO	,		LPM_UndockDO			,	0	},
	{ "$0.WaferSlideOutDI"		, _K_D_IO	,		LPM_WaferSlideOut		,	0	} ,
	{ "$0.F_ProperlyPlaceDI"	, _K_D_IO	,		LPM_FOUP_ProperlyPlaced	,	0	} ,
	{ "$0.LOADING_SIGNAL"		, _K_D_IO	,		LPM_LOADING_SIGNAL		,	0	} ,
	{ "$0.UNLOADING_SIGNAL"		, _K_D_IO	,		LPM_UNLOADING_SIGNAL	,	0	} ,
	{ "$0.InterlockActiveDI"	, _K_D_IO	,		LPM_INTERLOCK_ACTIVE	,	0	} ,
    //{ "$0.Load_Flash"			, _K_D_IO	,		LPM_LOAD_LAMP			,	0	} ,
    //{ "$0.UnLoad_Flash"			, _K_D_IO	,		LPM_UNLOAD_LAMP			,	0	} ,
    { "$0.ErrorResetDO"			, _K_D_IO	,		LPM_ErrorResetDO		,	0	} ,
    { "$0.Fix6MInit"			, _K_D_IO	,		LPM_Init				,	0	} ,

	// Begin 2004.11.30 Mapping Added swhan
	{ "$0.Mapping"				, _K_D_IO	,		LPM_MAPPING				,	0	} ,
	{ "$0.Read_Map_Info"		, _K_D_IO	,		LPM_MAP_UPDATE			,	0	} ,
	{ "$0.WaferMap"				, _K_S_IO	,		LPM_Wafer_Map			,	0	} ,
	{ "$0.MappingStatusDI"		, _K_D_IO	,		LPM_Mapping_Status		,	0	} ,
	{ "$0.MAPPING_SIGNAL"		, _K_D_IO	,		LPM_MAPL_STATUS			,	0	} ,
	{ "$1.Mapping"				, _K_D_IO	,		CM_MAP_STATUS			,	0	} ,
	{ "$1.C01_Wafer"			, _K_D_IO	,		CM_WAFER01				,	0	} ,
	{ "$1.C02_Wafer"			, _K_D_IO	,		CM_WAFER02				,	0	} ,
	{ "$1.C03_Wafer"			, _K_D_IO	,		CM_WAFER03				,	0	} ,
	{ "$1.C04_Wafer"			, _K_D_IO	,		CM_WAFER04				,	0	} ,
	{ "$1.C05_Wafer"			, _K_D_IO	,		CM_WAFER05				,	0	} ,
	{ "$1.C06_Wafer"			, _K_D_IO	,		CM_WAFER06				,	0	} ,
	{ "$1.C07_Wafer"			, _K_D_IO	,		CM_WAFER07				,	0	} ,
	{ "$1.C08_Wafer"			, _K_D_IO	,		CM_WAFER08				,	0	} ,
	{ "$1.C09_Wafer"			, _K_D_IO	,		CM_WAFER09				,	0	} ,
	{ "$1.C10_Wafer"			, _K_D_IO	,		CM_WAFER10				,	0	} ,
	{ "$1.C11_Wafer"			, _K_D_IO	,		CM_WAFER11				,	0	} ,
	{ "$1.C12_Wafer"			, _K_D_IO	,		CM_WAFER12				,	0	} ,
	{ "$1.C13_Wafer"			, _K_D_IO	,		CM_WAFER13				,	0	} ,
	{ "$1.C14_Wafer"			, _K_D_IO	,		CM_WAFER14				,	0	} ,
	{ "$1.C15_Wafer"			, _K_D_IO	,		CM_WAFER15				,	0	} ,
	{ "$1.C16_Wafer"			, _K_D_IO	,		CM_WAFER16				,	0	} ,
	{ "$1.C17_Wafer"			, _K_D_IO	,		CM_WAFER17				,	0	} ,
	{ "$1.C18_Wafer"			, _K_D_IO	,		CM_WAFER18				,	0	} ,
	{ "$1.C19_Wafer"			, _K_D_IO	,		CM_WAFER19				,	0	} ,
	{ "$1.C20_Wafer"			, _K_D_IO	,		CM_WAFER20				,	0	} ,
	{ "$1.C21_Wafer"			, _K_D_IO	,		CM_WAFER21				,	0	} ,
	{ "$1.C22_Wafer"			, _K_D_IO	,		CM_WAFER22				,	0	} ,
	{ "$1.C23_Wafer"			, _K_D_IO	,		CM_WAFER23				,	0	} ,
	{ "$1.C24_Wafer"			, _K_D_IO	,		CM_WAFER24				,	0	} ,
	{ "$1.C25_Wafer"			, _K_D_IO	,		CM_WAFER25				,	0	} ,	
	{ "$1.MAPWAFER_COUNT"		, _K_D_IO	,		CM_MAPWAFER_COUNT		,	0	} ,	
	{ "$1.MAPWAFER_INFO"		, _K_S_IO	,		CM_MAPWAFER_INFO		,	0	} ,
	{ "PRMD_FM_RB_MOVE_TMO"		, _K_D_IO	,		MOVE_WAFER_TIMEOUT		,	0	} ,

	// Begin 2004.08.27 swhan for DCOP Initialize
    { "$0.PM1_Use"			, _K_D_IO	,		LPM_PM1_Use			,	0	} ,
    { "$0.PM2_Use"			, _K_D_IO	,		LPM_PM2_Use			,	0	} ,
	{ "$0.PM3_Use"			, _K_D_IO	,		LPM_PM3_Use			,	0	} ,
	{ "$0.PM4_Use"			, _K_D_IO	,		LPM_PM4_Use			,	0	} ,
	{ "$0.PM5_Use"			, _K_D_IO	,		LPM_PM5_Use			,	0	} ,
	// End 2004.08.27 swhan for DCOP Initialize

	// End 2004.11.30 Mapping Added swhan
//	{"TMSIM_CM1"			, _K_F_IO	,	TMSIM_CM1	, 0},
//	{"TMSIM_CM2"			, _K_F_IO	,	TMSIM_CM2	, 0},
//	{"TMSIM_CM3"			, _K_F_IO	,	TMSIM_CM3	, 0},
	// 2013.04.30 Error Code Remapping by hyuk
	{ "$0.EVTErrorCode"		, _K_S_IO	,		LPM_EVTErrorCode	,	0	} ,
	{ "CTC.MID_NAME$2"			,   _K_S_IO	,	CTC_MID_NAME	,	0	} ,

	// 2017.11.28 N2 Purge Monitor Flag Added
	{ "$1_N2_PURGE"			, _K_F_IO	,		CM_N2_PURGE		,	0	} ,
	{ "PRMD_LP$3_N2MODE"		, _K_D_IO	,		PRMD_N2MODE		,	0	} ,
	{ "CTC.N2PURGE_CONTROL"	, _K_D_IO	,	N2Purge_Control		,	0	} ,
	""
};

enum {FullDock,			ShuttleIn,		Clamp};
enum {FullUndock,		CloseDoor,		ShuttleOut};
enum {SH_Unknown,		SH_In,   		SH_Out,			SH_Error};
enum {CLAMP_Unknown,	CLAMP_Clamp,	CLAMP_Unclamp,	CLAMP_Error};
enum {DOOR_Unknown,		DOOR_Up,		DOOR_Down,		DOOR_Error};
enum {LATCH_Unknown,	LATCH_Open,		LATCH_Close, 	LATCH_Error};
enum {PAD_Unknown,  	PAD_Extend, 	PAD_Retract,	PAD_Error};

// Begin 2004.11.30 Mapping Added swhan
enum {MCMD_None, MCMD_Map};
enum {RMCMD_None,		RMCMD_ReadMap};
enum {MAP_NONE, MAP_STARTED, MAP_COMPLETE, MAP_FAIL};
// End 2004.11.30 Mapping Added swhan



static char g_szModuleName[20] = "CM1";			// 2013.11.15
static char g_szMID[256] = "";	//2014.06.26
//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.11.15
// LOG_TYPE
enum { LTYPE_PRC, LTYPE_XFR, LTYPE_FNC, LTYPE_LEH, LTYPE_USER };

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
void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szData, char *szData2)
{
	char szEventID[20];
	int nLogType = LTYPE_LEH;
	
	do {
		if(STRCMP_L(szRunStr , "LOAD"))			  strcpy(szEventID, "CarrierLoad");
		else if(STRCMP_L(szRunStr , "UNLOAD"))	  strcpy(szEventID, "CarrierUnload");
		else if(STRCMP_L(szRunStr , "JOB_START")) strcpy(szEventID, "ProcessJobStart");
		else if(STRCMP_L(szRunStr , "JOB_END"))   strcpy(szEventID, "ProcessJobEnd");
		else if(STRCMP_L(szRunStr , "MAP"))     { strcpy(szEventID, "Map"); nLogType = LTYPE_FNC; }
		else if(STRCMP_L(szRunStr , "LPLOAD"))  { strcpy(szEventID, "LPLoad"); nLogType = LTYPE_FNC; }
		else if(STRCMP_L(szRunStr , "LPUNLOAD")){ strcpy(szEventID, "LPUnload"); nLogType = LTYPE_FNC; }

		else break;
		
		switch(nLogType)
		{
		case LTYPE_LEH: _MLOG("LEH %s %s %s %s", szModule, szEventID, szStatus, szData); break;		// 2013.07.23
		case LTYPE_FNC: _MLOG("FNC %s %s %s %s %s", szModule, szEventID, szStatus, szData, szData2); break;
		}
		
	} while(0);
}


//------------------------------------------------------------------------------------------
int READ_DIGITAL_TRUE( int io , int *CommStatus ) {
	int Data;
	Data = READ_DIGITAL( io , CommStatus );
	if ( !(*CommStatus) ) {
		_sleep(250);
		Data = READ_DIGITAL( io , CommStatus );
		if ( !(*CommStatus) ) {
			_sleep(500);
			Data = READ_DIGITAL( io , CommStatus );
			if ( !(*CommStatus) ) {
				_sleep(1000);
				Data = READ_DIGITAL( io , CommStatus );
				if ( !(*CommStatus) ) {
					_sleep(1000);
					return( READ_DIGITAL( io , CommStatus ) );
				}
				else return Data;
			}
			else return Data;
		}
		else return Data;
	}
	else return Data;
}

// 2013.04.26 Error Code Remapping
void ALARM_MESSAGE_REMAPPING( int id ) {
	char Buffer[256] , Buffer2[256], szErrorCode[256];
	int  err = 0;
	int  errlevel = 0;
	int CommStatus;

	memset(Buffer,		0, sizeof(Buffer));
	memset(Buffer2,		0, sizeof(Buffer2));
	memset(szErrorCode, 0, sizeof(szErrorCode));
	
	READ_STRING( LPM_EVTErrorCode, szErrorCode, &CommStatus );
	if (strlen(szErrorCode) > 0) {
		sprintf( Buffer2 , "(%s)" , szErrorCode);
		ALARM_MESSAGE_GET( id , Buffer );
		strcat( Buffer , Buffer2 );
		ALARM_MESSAGE_SET( id , Buffer );
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//			  Run_Part(			-1 ,               -1 ,		  -1 ,              -1 ,           -1 ,
//					  LPM_UndockDO ,   nUndockCommand ,       -1 ,              -1 ,          TRUE, 
//			       LPM_DoorUpDownXI,          DOOR_Up ,       -1 ,              -1 , ALARM_DOORUP ,     TRUE ) == SYS_ABORTED )

Module_Status Run_Part( int PR_IO1 , int PR_IO1_Data , int PR_IO2 , int PR_IO2_Data , int PR_Alarm ,
					   int AC_IO1 , int AC_IO1_Data , int AC_IO2 , int AC_IO2_Data , BOOL ACFIRST , 
					   int CV_IO1 , int CV_IO1_Data , int CV_IO2 , int CV_IO2_Data , int RUN_Alarm , BOOL TMO ) {
	int CommStatus;// , AlmResult;
	BOOL SUC;
	if ( PR_IO1 > 0 ) {
		if ( READ_DIGITAL( PR_IO1 , &CommStatus ) != PR_IO1_Data ) {
			ALARM_MESSAGE_REMAPPING(PR_Alarm + ALARM_START_POINTER);
			ALARM_POST( PR_Alarm + ALARM_START_POINTER);
			return SYS_ABORTED;
		}
	}
	if ( PR_IO2 > 0 ) {
		if ( READ_DIGITAL( PR_IO2 , &CommStatus ) != PR_IO2_Data ) {
			ALARM_MESSAGE_REMAPPING(PR_Alarm + ALARM_START_POINTER);
			ALARM_POST( PR_Alarm + ALARM_START_POINTER);
			return SYS_ABORTED;
		}
	}
	if ( ( AC_IO1 < 0 ) && ( AC_IO2 < 0 ) && ( CV_IO1 < 0 ) && ( CV_IO2 < 0 ) ) return SYS_SUCCESS;
	while (1) {
		if ( ( CV_IO1 >= 0 ) || ( CV_IO2 >= 0 ) ) {
			if ( !ACFIRST ) {
				SUC = TRUE;
				if ( CV_IO1 > 0 ) {
					if ( READ_DIGITAL( CV_IO1 , &CommStatus ) != CV_IO1_Data ) SUC = FALSE;
				}
				if ( CV_IO2 > 0 ) {
					if ( READ_DIGITAL( CV_IO2 , &CommStatus ) != CV_IO2_Data ) SUC = FALSE;
				}
				if ( SUC ) return SYS_SUCCESS;
			}
		}
		if ( AC_IO1 > 0 ) {
			WRITE_DIGITAL( AC_IO1 , AC_IO1_Data , &CommStatus );
		}
		if ( AC_IO2 > 0 ) {
			WRITE_DIGITAL( AC_IO2 , AC_IO2_Data , &CommStatus );
		}
		if ( ( CV_IO1 < 0 ) && ( CV_IO2 < 0 ) ) return SYS_SUCCESS;
		S_TIMER_READY();
		while(1) {
			SUC = TRUE;
			if ( CV_IO1 > 0 ) {
				if ( READ_DIGITAL( CV_IO1 , &CommStatus ) != CV_IO1_Data ) SUC = FALSE;
			}
			if ( CV_IO2 > 0 ) {
				if ( READ_DIGITAL( CV_IO2 , &CommStatus ) != CV_IO2_Data ) SUC = FALSE;
			}
			if ( SUC ) return SYS_SUCCESS;
			else {
				if ( !TMO ) {
					if ( RUN_Alarm > 0 ) {
						ALARM_MESSAGE_REMAPPING(RUN_Alarm + ALARM_START_POINTER);
						ALARM_POST( RUN_Alarm + ALARM_START_POINTER );
					}
					return SYS_ABORTED;
				}
			}
			if ( !WAIT_SECONDS( 0.1 ) ) return SYS_ABORTED;
			if ( S_TIMER_CHECK( 1 , (double) READ_DIGITAL( TIMEOUT , &CommStatus ) ) ) {
				if ( READ_DIGITAL( LPM_INTERLOCK_ACTIVE , &CommStatus ) ) { // Active Error
					if ( (( LPM_DockDO == AC_IO1 ) && ( AC_IO1_Data == FullDock || AC_IO1_Data == ShuttleIn)) ||
						 (( LPM_DockDO == AC_IO2 ) && ( AC_IO2_Data == FullDock || AC_IO2_Data == ShuttleIn)) ) {
						WRITE_DIGITAL( LPM_UndockDO , FullUndock , &CommStatus );
					}
				}
				ALARM_MESSAGE_REMAPPING(RUN_Alarm + ALARM_START_POINTER);
				ALARM_POST( RUN_Alarm + ALARM_START_POINTER );
				return SYS_ABORTED;
//				AlmResult = ALARM_MANAGE( RUN_Alarm + ALARM_START_POINTER );
//				S_TIMER_READY();
//				break;
				// For Only Interlock Active..
/*				if ((( LPM_DockDO == AC_IO1 ) && ( AC_IO1_Data == FullDock || AC_IO1_Data == ShuttleIn)) ||
					(( LPM_DockDO == AC_IO2 ) && ( AC_IO2_Data == FullDock || AC_IO2_Data == ShuttleIn)) ) {
						if ( AlmResult == ALM_ABORT  ) return SYS_ABORTED;
						else                           break;
					}
				//==
				else{
					if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
					else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
					else                                break;
				}
*/			}
		}
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Ready_Control() {
	// Begin 2004.08.27 swhan for DCOP Initialize
	int nCommStatus = 0;
	WRITE_DIGITAL(LPM_PM1_Use, NO, &nCommStatus);
	WRITE_DIGITAL(LPM_PM2_Use, NO, &nCommStatus);
	WRITE_DIGITAL(LPM_PM3_Use, NO, &nCommStatus);
	WRITE_DIGITAL(LPM_PM4_Use, NO, &nCommStatus);
	WRITE_DIGITAL(LPM_PM5_Use, NO, &nCommStatus);
	// End 2004.08.27 swhan for DCOP Initialize
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status Home_Control() {
	return SYS_SUCCESS;
}

Module_Status Init_Control() {
	int CommStatus , AlmResult;
	BOOL Check = TRUE;
	int nLPM_ShuttleInOutXI = 0;	// 0 Unknown, 1 In, 2 Out, 3 Error
	int nLPM_ClampUnclampXI = 0;	// 0 Unknown ,1 Clamp, 2 Unclamp, 3 Error

	while ( TRUE ) {
		S_TIMER_READY();

		WRITE_DIGITAL(LPM_ErrorResetDO, 1, &CommStatus);
		WRITE_DIGITAL(LPM_Init, 1, &CommStatus);

		while ( TRUE ) {

			nLPM_ShuttleInOutXI = READ_DIGITAL(LPM_ShuttleInOutXI, &CommStatus);
			nLPM_ClampUnclampXI = READ_DIGITAL(LPM_ClampUnclampXI, &CommStatus);

			if( (2 == nLPM_ShuttleInOutXI) && (2 == nLPM_ClampUnclampXI) )
			{
				//break;
				return SYS_SUCCESS;
			}

			if (!WAIT_SECONDS(0.1)) return SYS_ABORTED;

			if ( S_TIMER_CHECK( 1 , (double) READ_DIGITAL( TIMEOUT , &CommStatus ) ) ) 
			{
				ALARM_MESSAGE_REMAPPING(ALARM_TMO + ALARM_START_POINTER);
				AlmResult = ALARM_MANAGE( ALARM_TMO + ALARM_START_POINTER );
					if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
					else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
					else                                { Check = FALSE; break; }
			}
		}

	}
	return SYS_SUCCESS;
}

Module_Status Door_Control( int mode ) { // 0 : CLOSE
	int nCommStatus = 0;
	int nAlarmResult = -1;
	switch( mode ) {
	case 0 : // Door Close
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
					-1 , -1 , -1 , -1 , FALSE , 
					LPM_DoorUpDownXI , 1 , -1 , -1 , -1 , FALSE ) == SYS_SUCCESS ) {
			if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
						-1 , -1 , -1 , -1 , FALSE , 
						LPM_PaddleExtRetXI , 2 , -1 , -1 , -1 , FALSE ) == SYS_SUCCESS ) {
				if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
							-1 , -1 , -1 , -1 , FALSE , 
							LPM_LatchOpenCloseXI , 2 , -1 , -1 , -1 , FALSE ) == SYS_SUCCESS ) {
					return SYS_SUCCESS;
				}
			}
		}

		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
				-1 , -1 , -1 , -1 , FALSE , 
				-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Run_Part( LPM_ShuttleInOutXI , 1 , -1 , -1 , ALARM_NOTCARRIER ,
					-1 , -1 , -1 , -1 , FALSE , 
					-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;

		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
					LPM_UndockDO, CloseDoor , -1 , -1 , FALSE , 
					LPM_DoorUpDownXI, DOOR_Up , -1 , -1 , ALARM_DOORUP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
					-1 , -1 , -1 , -1 , FALSE , 
					LPM_PaddleExtRetXI , PAD_Retract , -1 , -1 , ALARM_RETRACT , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
					-1, -1 , -1 , -1 , FALSE , 
					LPM_LatchOpenCloseXI , LATCH_Close , -1 , -1 , ALARM_CLOSE , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		break;
	case 1 : // Door Open
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
					-1 , -1 , -1 , -1 , FALSE , 
					LPM_DoorUpDownXI , 2 , -1 , -1 , -1 , FALSE ) == SYS_SUCCESS ) {
			if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
						-1 , -1 , -1 , -1 , FALSE , 
						LPM_PaddleExtRetXI , 1 , -1 , -1 , -1 , FALSE ) == SYS_SUCCESS ) {
				if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
							-1 , -1 , -1 , -1 , FALSE , 
							LPM_LatchOpenCloseXI , 1 , -1 , -1 , -1 , FALSE ) == SYS_SUCCESS ) {
					return SYS_SUCCESS;
				}
			}
		}

		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
			-1 , -1 , -1 , -1 , FALSE , 
			-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Run_Part( LPM_ShuttleInOutXI , 1 , -1 , -1 , ALARM_NOTCARRIER ,
			-1 , -1 , -1 , -1 , FALSE , 
			-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;
		
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			LPM_DockDO, FullDock , -1 , -1 , FALSE , 
			LPM_LatchOpenCloseXI, LATCH_Open , -1 , -1 , ALARM_DOORUP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
					-1 , -1 , -1 , -1 , FALSE , 
					LPM_PaddleExtRetXI , PAD_Extend , -1 , -1 , ALARM_EXTEND , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
					-1 , -1 , -1 , -1 , FALSE , 
					LPM_DoorUpDownXI , DOOR_Down , -1 , -1 , ALARM_DOORDOWN , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		if ( Run_Part( LPM_WaferSlideOut , 1 , -1 , -1 , ALARM_SLIDE ,
					-1 , -1 , -1 , -1 , FALSE , 
					-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;

		break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status Cass_Control( int mode ) { // 0 : Present
	switch( mode ) {
	case 0 :
		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
					-1 , -1 , -1 , -1 , FALSE , 
					-1 , -1 , -1 , -1 , -1 , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		break;
	case 1 :
		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 0 , -1 , -1 , ALARM_CARRIER ,
					-1 , -1 , -1 , -1 , FALSE , 
					-1 , -1 , -1 , -1 , -1 , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status Dock_Control( int mode ) { // 0 : UNDOCK
	switch( mode ) {
	case 0 :// Shuttle Out
		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
					LPM_UndockDO , ShuttleOut , -1 , -1 , FALSE , 
					LPM_ShuttleInOutXI , SH_Out , -1 , -1 , ALARM_UNDOCKING , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		break;
	case 1 :// Shuttle In
		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
					LPM_DockDO , ShuttleIn , -1 , -1 , FALSE , 
					LPM_ShuttleInOutXI , SH_In , -1 , -1 , ALARM_DOCKING , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status Clamp_Control( int mode , BOOL Option ) { // 0 : UNCLAMP
	int CommStatus;
	switch( mode ) {
	case 0 :// Unclamp
		if ( Option ) {
			if ( READ_DIGITAL( CLAMP_MODE , &CommStatus ) == 0 ) return SYS_SUCCESS;
		}
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
				LPM_ClampUnclampXO , 2 , -1 , -1 , FALSE , 
				LPM_ClampUnclampXI , 2 , -1 , -1 , ALARM_UNCLAMP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		break;
	case 1 :// Clamp
		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
					LPM_ClampUnclampXO , 1 , -1 , -1 , FALSE , 
					LPM_ClampUnclampXI , 1 , -1 , -1 , ALARM_CLAMP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
// Load_Control( 0 , FALSE , AUTO_MODE )
Module_Status Load_Control( int mode , BOOL Option , BOOL AutoMode ) { // 0 : Unload
	int CommStatus;
	int nUndockCommand;
	printf( "Load control[%d] Start\n", mode);
	//char Buffer[256];
	switch( mode ) {
	case 0 :// Unload
		/*
		if ( Door_Control( 0 ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( AutoMode ) {
//			sprintf( Buffer , EVENT_STRING_DOOR_CLOSE , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
//			sprintf( Buffer , EVENT_STRING_UNDOCKING , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		if ( Dock_Control( 0 ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( AutoMode ) {
//			sprintf( Buffer , EVENT_STRING_MOVINGOUT , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		if ( Clamp_Control( 0 , Option ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( AutoMode ) {
//			sprintf( Buffer , EVENT_UNLOAD_REQUEST , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}*/		
		
		if( READ_DIGITAL( LPM_DoorUpDownXI, &CommStatus) == DOOR_Up && 
			// delete 2008.09.19
			//READ_DIGITAL( LPM_PaddleExtRetXI, &CommStatus) == PAD_Retract && 
			//READ_DIGITAL( LPM_LatchOpenCloseXI, &CommStatus) == LATCH_Close &&
			READ_DIGITAL( LPM_ShuttleInOutXI, &CommStatus) == SH_Out &&
			READ_DIGITAL( LPM_ClampUnclampXI, &CommStatus) == CLAMP_Unclamp &&
			(READ_DIGITAL( LPM_ClampUnclampXI, &CommStatus) == CLAMP_Clamp && TRUE == Option  && READ_DIGITAL( CLAMP_MODE , &CommStatus ) == 0) )
			return SYS_SUCCESS;
		

		/*
		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
			-1 , -1 , -1 , -1 , FALSE , 
			-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;

		if ( Run_Part( LPM_ShuttleInOutXI , SH_In , -1 , -1 , ALARM_NOTCARRIER ,
			-1 , -1 , -1 , -1 , FALSE , 
			-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;
			*/


		if ( TRUE == Option  && (READ_DIGITAL( CLAMP_MODE , &CommStatus ) == 0) )  // Shuttle Out Only
			nUndockCommand = ShuttleOut;
		else
			nUndockCommand = FullUndock;
		
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			LPM_UndockDO, nUndockCommand , -1 , -1 , TRUE, 
			LPM_DoorUpDownXI, DOOR_Up , -1 , -1 , ALARM_DOORUP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;		

		// Deleted 2008.09.19 by jonghyuk
		/*
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			-1 , -1 , -1 , -1 , FALSE , 
			LPM_PaddleExtRetXI , PAD_Retract , -1 , -1 , ALARM_RETRACT , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		*/
		
		// deleted 2006.05.11 by ycchoi < 1.0.1.0 >
		/*
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			-1, -1 , -1 , -1 , FALSE , 
			LPM_LatchOpenCloseXI , LATCH_Close , -1 , -1 , ALARM_CLOSE , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		*/
		
		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
			-1 , -1, -1 , -1 , FALSE , 
			LPM_ShuttleInOutXI , SH_Out , -1 , -1 , ALARM_UNDOCKING , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		if( ShuttleOut == nUndockCommand  ) break;

		// delete 2007.09.12 ijbae.
		// it only use for duraport LPM.

		/*
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			-1 , -1 , -1 , -1 , FALSE , 
			LPM_ClampUnclampXI , CLAMP_Unclamp , -1 , -1 , ALARM_UNCLAMP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;		
		*/
		// end of delete

		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			LPM_ClampUnclampXO , 2 , -1 , -1 , FALSE , 
			LPM_ClampUnclampXI , CLAMP_Unclamp , -1 , -1 , ALARM_UNCLAMP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;		

		break;
	case 1 : //Load
		if ( READ_DIGITAL( LPM_FOUP_ProperlyPlaced , &CommStatus )	== 1			&&
			 READ_DIGITAL( LPM_ClampUnclampXI , &CommStatus )		== CLAMP_Clamp	&&
			 READ_DIGITAL( LPM_ShuttleInOutXI , &CommStatus )		== SH_In		&&
			 // Modify Motor Type Change 2008.09.19  by jonghyuk
			 //READ_DIGITAL( LPM_VacuumOK , &CommStatus )				== 1			&&
			 //READ_DIGITAL( LPM_LatchOpenCloseXI , &CommStatus )		== LATCH_Open	&&
			 //READ_DIGITAL( LPM_PaddleExtRetXI , &CommStatus )		== PAD_Extend	&&
			 READ_DIGITAL( LPM_DoorUpDownXI , &CommStatus )			== DOOR_Down )
			return SYS_SUCCESS;
		/*
		if ( Clamp_Control( 1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( AutoMode ) {
//			sprintf( Buffer , EVENT_STRING_MOVINGIN , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		if ( Dock_Control( 1 ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( AutoMode ) {
//			sprintf( Buffer , EVENT_STRING_DOCKING , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		if ( Door_Control( 1 ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( AutoMode ) {
//			sprintf( Buffer , EVENT_STRING_DOOR_OPEN , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}*/
		
		printf("+++FullDock!\n");

		// add - 2007.09.12 ijbae
		// it only use for duraport LPM.

		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
					LPM_ClampUnclampXO , 1 , -1 , -1 , FALSE , 
					LPM_ClampUnclampXI , 1 , -1 , -1 , ALARM_CLAMP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		// end of add.

		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
			LPM_DockDO , FullDock , -1 , -1 , TRUE , 
			LPM_ClampUnclampXI , CLAMP_Clamp , -1 , -1 , ALARM_CLAMP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		printf("+++Shuttle In Check\n");

		if ( Run_Part( LPM_FOUP_ProperlyPlaced , 1 , -1 , -1 , ALARM_NOTCARRIER ,
			-1, -1, -1 , -1 , FALSE , 
			LPM_ShuttleInOutXI , SH_In , -1 , -1 , ALARM_DOCKING , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		printf("+++Latch Check\n");

		// deleted 2006.05.11 by ycchoi < 1.0.1.0 >
		/*
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			-1, -1, -1 , -1 , FALSE , 
			LPM_LatchOpenCloseXI, LATCH_Open , -1 , -1 , ALARM_DOORUP , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		*/

		// Delete 2008.09.19 by jonghyuk
		/*
		printf("+++Paddle Check\n");

		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			-1 , -1 , -1 , -1 , FALSE , 
			LPM_PaddleExtRetXI , PAD_Extend , -1 , -1 , ALARM_EXTEND , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;
		*/
		printf("+++Door Check\n");	
		
		if ( Run_Part( -1 , -1 , -1 , -1 , -1 ,
			-1 , -1 , -1 , -1 , FALSE , 
			LPM_DoorUpDownXI , DOOR_Down , -1 , -1 , ALARM_DOORDOWN , TRUE ) == SYS_ABORTED ) return SYS_ABORTED;

		printf("+++Wafer Slide Check\n");
		
		if ( Run_Part( LPM_WaferSlideOut , 1 , -1 , -1 , ALARM_SLIDE ,
			-1 , -1 , -1 , -1 , FALSE , 
			-1 , -1 , -1 , -1 , -1 , FALSE ) == SYS_ABORTED ) return SYS_ABORTED;

		break;
		
	}
	printf( "Load control[%d] End\n", mode);
	return SYS_SUCCESS;
}


Module_Status MAPPING_SUB( int mode ) {
	char Buffer[256] , Buffer2[256];	
	int i, Data , Count = 0;
	int CommStatus;// , AlmResult;	
	int nIndex = 0;
	int j = 0;
	BOOL bMapError = FALSE;
	
	// For MARS
	LogEvent(g_szModuleName, "MAP", "Start", g_szMID, "(2,");

	printf("---> CM%d TBM mapping: call sub func\n", CASSETTE_INDEX);
	while (1) 
	{
		printf("---> CM%d TBM mapping sub: Clear\n", CASSETTE_INDEX);		
		for ( i = CM_WAFER01 ; i <= CM_WAFER25 ; i++ ) 
			WRITE_DIGITAL( i , 0 , &CommStatus );
		//
		//printf("---> FEM Robot mapping: write event message to scheduler in sub\n");
		switch( mode ) {
		case 1 :
			sprintf( Buffer , EVENT_STRING_MAPREQ_IN , CASSETTE_INDEX);
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			break;
		case 2 :
			sprintf( Buffer , EVENT_STRING_MAPREQ_MAN , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			break;
		case 3 :
			sprintf( Buffer , EVENT_STRING_MAPREQ_OUT , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			break;
		}
		//
		//WRITE_DIGITAL( CM_MAPPING , Station , &CommStatus );
		printf("===@~~ map Start!! \n");
		WRITE_DIGITAL( LPM_MAPPING, MCMD_Map, &CommStatus);
		
		_sleep(3000);

		S_TIMER_READY();
		while(1) 
		{
			if ( FALSE == WAIT_SECONDS( 0.1 ) ) {
				return SYS_ABORTED;
			}
			Data = 99;
			Data = READ_DIGITAL( LPM_Mapping_Status, &CommStatus);
			if( FALSE == CommStatus)
			{
/*				AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_COMM_ERROR );								
				if ( AlmResult != ALM_RETRY && AlmResult != ALM_IGNORE ) return SYS_ABORTED;
*/				
				ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + ALARM_COMM_ERROR);
				ALARM_POST( ALARM_START_POINTER + ALARM_COMM_ERROR );
				return SYS_ABORTED;
			}
			
			if( MAP_COMPLETE == Data) // if Idle...
				break;
			else if( MAP_FAIL == Data )
			{
/*				AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_MAP_WAFER );				
				if      ( AlmResult == ALM_IGNORE ) break;				
				else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;	
*/			
				ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + ALARM_MAP_WAFER );
				ALARM_POST( ALARM_START_POINTER + ALARM_MAP_WAFER );
				return SYS_ABORTED;
			}
			
			//printf( "++##^^' LPRunningNum = [%d: %d]\n",nIndex++, Data);
			
			if ( S_TIMER_CHECK( 1 , (double) READ_DIGITAL( MOVE_WAFER_TIMEOUT , &CommStatus ) ) ) 
			{
/*				AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_MAP_WAFER );
				S_TIMER_READY();
				if      ( AlmResult == ALM_IGNORE ) break;				
				else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
*/				
				ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + ALARM_MAP_WAFER);
				ALARM_POST( ALARM_START_POINTER + ALARM_MAP_WAFER );
				return SYS_ABORTED;
			}
		}
//		if( MAP_COMPLETE == Data || AlmResult == ALM_IGNORE) break;
		printf("===@~~ map End!! \n");
		if( MAP_COMPLETE == Data ) break;	
	}

	
	WRITE_DIGITAL( LPM_MAP_UPDATE , RMCMD_ReadMap , &CommStatus );
	READ_STRING( LPM_Wafer_Map , Buffer , &CommStatus );
	
	sprintf( Buffer2 , "" );
	for ( i = CM_WAFER01 ; i <= CM_WAFER25 ; i++ ) {
		j = i - CM_WAFER01;
		switch( Buffer[i-CM_WAFER01] ) {
			case '1' : 
				strcat( Buffer2 , "1" );
				WRITE_DIGITAL( i , 1 , &CommStatus ); 
				break;
			case '2' : 
				Count++;
				strcat( Buffer2 , "2" );
				WRITE_DIGITAL( i , 2 , &CommStatus ); 
				break;
			case '3' :
				strcat( Buffer2 , "0" );
				WRITE_DIGITAL( i , 0 , &CommStatus ); 
				ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + ALARM_CROSS + j);
				ALARM_POST(ALARM_START_POINTER + ALARM_CROSS + j);
				bMapError = TRUE;				
				break;
			case '4' :
				strcat( Buffer2 , "0" );
				WRITE_DIGITAL( i , 0 , &CommStatus ); 
				ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + ALARM_DOUBLE + j);
				ALARM_POST(ALARM_START_POINTER + ALARM_DOUBLE + j);
				bMapError = TRUE;				
				break;
			default  : 
				strcat( Buffer2 , "0" );
				WRITE_DIGITAL( i , 0 , &CommStatus ); 
				break;
		}
	}
	WRITE_DIGITAL( CM_MAPWAFER_COUNT, Count , &CommStatus );
	WRITE_STRING( CM_MAPWAFER_INFO, Buffer2 , &CommStatus );

	//printf("---> FEM Robot mapping: EVENT_STRING_MAPCOMPLETE_IN in sub\n");
	switch( mode ) {
	case 1 :
		sprintf( Buffer , EVENT_STRING_MAPCOMPLETE_IN , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		//
		sprintf( Buffer , EVENT_STRING_AT_SOURCE_IN , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		//
		sprintf( Buffer , EVENT_STRING_AT_OCCUPY_IN , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		break;
	case 2 :
		sprintf( Buffer , EVENT_STRING_MAPCOMPLETE_MAN , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		//
		sprintf( Buffer , EVENT_STRING_AT_SOURCE_MAN , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		//
		sprintf( Buffer , EVENT_STRING_AT_OCCUPY_MAN , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		break;
	case 3 :
		sprintf( Buffer , EVENT_STRING_MAPCOMPLETE_OUT , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		break;
	}

	LogEvent(g_szModuleName, "MAP", "End", g_szMID, Buffer2);

	if (TRUE == bMapError) return SYS_ABORTED;
	else                   return SYS_SUCCESS;
}

Module_Status MAPPING( int mode ) 
{
	int Res , CommStatus;	
	
	while( TRUE ) 
	{
		if ( READ_DIGITAL_TRUE( LPM_WaferSlideOut , &CommStatus ) != 1 ) 
		{
/*			Res = ALARM_MANAGE( ALARM_START_POINTER + ALARM_SLIDE );
			if ( Res == ALM_IGNORE ) break;
			if ( Res == ALM_ABORT  ) return SYS_ABORTED;
*/		
			ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + ALARM_SLIDE);
			ALARM_POST( ALARM_START_POINTER + ALARM_SLIDE );
			return SYS_ABORTED;
		}
		else 
		{
			break;
		}
	}
	
	
	if ( READ_DIGITAL( LPM_DoorUpDownXI , &CommStatus ) != 2 ) 
	{
		ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + ALARM_MAP_WAFER);
		ALARM_POST( ALARM_START_POINTER + ALARM_MAP_WAFER );
		return SYS_ABORTED;
	}
		
	WRITE_DIGITAL( CM_MAP_STATUS , 1 , &CommStatus );
	WRITE_DIGITAL( LPM_MAPL_STATUS , 2 , &CommStatus );

	Res = MAPPING_SUB( mode );

	WRITE_DIGITAL( CM_MAP_STATUS , 0 , &CommStatus );
	WRITE_DIGITAL( LPM_MAPL_STATUS , 0 , &CommStatus );
	return Res;
}
//////////////////////////////////////////////////////////////////////////
//...2016.04.19 Check Door Status
BOOL IsDoorStatusOk(int nMode)
{
	int nCommStatus;
	BOOL bRet = FALSE;
	enum { eUnKnown, eCLOSE, eOPEN };
	
	S_TIMER_READY();
	do 
	{	
		if(nMode == eCLOSE)
		{
			if(DOOR_Up == READ_DIGITAL(LPM_DoorUpDownXI,&nCommStatus)) {bRet = TRUE; break;}
		}
		else if(nMode == eOPEN)
		{
			if(DOOR_Down == READ_DIGITAL(LPM_DoorUpDownXI,&nCommStatus)) {bRet = TRUE; break;}
		}
		
		if ( !WAIT_SECONDS( 0.1 ) ) return FALSE;
		
		if ( S_TIMER_CHECK( 1 , (double) READ_DIGITAL( TIMEOUT , &nCommStatus ) ) ) 
		{
			break;
		}
	} while (1);

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2010.03.22 Added by jhlee
//... 2016.04.19 Add Retry Function 
Module_Status FOUP_Door_Control(int nMode) // 1:close, 2:open
{
	int nCommStatus;
	int nAlmSts;
	int nAlmIndex;

	enum { eUnKnown, eCLOSE, eOPEN };
	
	//... initialize alarm Index
	if	   (nMode == eCLOSE) 	nAlmIndex = ALARM_DOORUP;
	else if(nMode == eOPEN)		nAlmIndex = ALARM_DOORDOWN;
	else 
	{
		printf("[FOUP_Door_Control]-------> UnKnown Command\n");
		return SYS_SUCCESS;
	}

	//... PreCheck
	if(READ_DIGITAL(LPM_DoorUpDownXI, &nCommStatus) == nMode )
		return SYS_SUCCESS;

	while(1)
	{
		//... Set Command
		WRITE_DIGITAL(LPM_FOUPDoor, nMode, &nCommStatus);
		/*
		//... Check Communication
		if(!nCommStatus) 
		{
			ALARM_POST( ALARM_START_POINTER + ALARM_COMM_ERROR);
			return SYS_ABORTED;	
		}
		*/
		//... Check Door Status
		if(!IsDoorStatusOk(nMode))
		{ 
			ALARM_MESSAGE_REMAPPING(ALARM_START_POINTER + nAlmIndex);
			nAlmSts = ALARM_MANAGE( ALARM_START_POINTER + nAlmIndex);
			if ( nAlmSts == ALM_RETRY ) 
			{
				WRITE_DIGITAL(LPM_ErrorResetDO, 1, &nCommStatus);
				if(!WAIT_SECONDS(1)) return SYS_ABORTED;
				continue;
			}
			if ( nAlmSts == ALM_ABORT ) return SYS_ABORTED;
			
			return SYS_ABORTED;
		}
		else
		{
			break;
		}
	}
	return SYS_SUCCESS;
}


//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int CommStatus , m , Res , AUTO_MODE;
	int AlmResult = 0;
	char RunStr[65] , RunStr1[65];
	char TempStr[41] , TempStr2[41] , StationStr[41] , SlotStr[41] , ElseStr[41];
	char Buffer[256], Buffer2[256];
	char szCmd[80] = "";
	BOOL N2Use = TRUE;

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , RunStr1 , 64 );
	STR_SEPERATE( RunStr1	, StationStr , TempStr2	, 40 );
	STR_SEPERATE( TempStr2	, SlotStr	 , ElseStr	, 40 );

	printf( "---> Check CM-FEM-Handling  RunStr:[%s] , RunStr1:[%s]  StationStr:[%s] \n" , RunStr,RunStr1, StationStr);

	READ_STRING( CTC_MID_NAME, g_szMID, &CommStatus);

	if ( SIMULATION_MODE ) 
	{		
		if ( STRCMP_L( RunStr , "MAPPING"   ) ) 
		{
			//STR_SEPERATE( PROGRAM_PARAMETER_READ()	, RunStr     , TempStr	, 40 );
			//STR_SEPERATE( TempStr	, StationStr , TempStr2	, 40 );
			//STR_SEPERATE( TempStr2	, SlotStr	 , ElseStr	, 40 );
			
			LogEvent(g_szModuleName, "MAP", "Start", g_szMID, "" );	//For MARS
			
			WRITE_DIGITAL( CM_MAPWAFER_COUNT, 25 , &CommStatus );
			WRITE_STRING( CM_MAPWAFER_INFO, "2222222222222222222222222" , &CommStatus );
			strcpy(Buffer2, "2222222222222222222222222");

			if ( STRCMP( SlotStr , "IN"  ) ) {
				if ( strlen( ElseStr ) > 0 ) {
					sprintf( Buffer , EVENT_STRING_MAPREQ_MAN , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					
					if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;
					
					sprintf( Buffer , EVENT_STRING_MAPCOMPLETE_MAN ,CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					//
					sprintf( Buffer , EVENT_STRING_AT_SOURCE_MAN , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					//
					sprintf( Buffer , EVENT_STRING_AT_OCCUPY_MAN , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
				else {
					sprintf( Buffer , EVENT_STRING_MAPREQ_IN , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					
					if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;
					
					sprintf( Buffer , EVENT_STRING_MAPCOMPLETE_IN , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					//
					sprintf( Buffer , EVENT_STRING_AT_SOURCE_IN , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					//
					sprintf( Buffer , EVENT_STRING_AT_OCCUPY_IN , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
			}
			else if ( STRCMP( SlotStr , "OUT"  ) ) {
				sprintf( Buffer , EVENT_STRING_MAPREQ_OUT , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				
				if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;
				
				sprintf( Buffer , EVENT_STRING_MAPCOMPLETE_OUT , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			}
			
			printf("FEM Handling : %s\n", Buffer);
		LogEvent(g_szModuleName, "MAP", "End", g_szMID, Buffer2);	// For MARS

		}else if ( STRCMP_L( RunStr , "LOAD"   ) ) {
			LogEvent(g_szModuleName, "LPLOAD"  , "Start", g_szMID, "");	// For MARS 2014.06.26
			LogEvent(g_szModuleName, "LPLOAD"  , "End"  , g_szMID, "");	// For MARS 2014.06.26
			LogEvent(g_szModuleName, "LOAD"    , ""     , ""     , "");	// For MARS 2013.06.13
		}else if ( STRCMP_L( RunStr , "UNLOAD" ) ) {
			LogEvent(g_szModuleName, "LPUNLOAD", "Start", g_szMID, "");	// For MARS 2014.06.26
			LogEvent(g_szModuleName, "LPUNLOAD", "End"  , g_szMID, "");	// For MARS 2014.06.26			
			LogEvent(g_szModuleName, "UNLOAD"  , ""     , ""     , "");	// For MARS 2013.06.13
		}
		return SYS_SUCCESS;
	}

	if ( READ_DIGITAL( CONTROL , &CommStatus ) == 1 ) return SYS_SUCCESS;

	if ( strlen( RunStr1 ) > 0 ) AUTO_MODE = 1;
	else                         AUTO_MODE = 0;

	if      ( STRCMP_L( RunStr , "READY"  ) ) {
		if (READ_DIGITAL( N2Purge_Control , &CommStatus ) == 1)
			RUN_SET_FUNCTION( CM_N2_PURGE , "N2_MONITOR_STOP");
		return( Ready_Control() );
	}
	else if ( STRCMP_L( RunStr , "HOME"   ) ) {
		return( Home_Control() );
	}
	else if ( STRCMP_L( RunStr , "INIT"   ) ) {
		if (READ_DIGITAL( N2Purge_Control , &CommStatus ) == 1)
			RUN_SET_FUNCTION( CM_N2_PURGE , "N2_MONITOR_STOP");
		return( Init_Control() );
	}
	else if ( STRCMP_L( RunStr , "OPEN"  ) ) {
		return( Door_Control( 1 ) );
	}
	else if ( STRCMP_L( RunStr , "CLOSE" ) ) {
		return( Door_Control( 0 ) );
	}
	else if ( STRCMP_L( RunStr , "LOAD"  ) ) {
//haeyong 2003. 07. 01
//        WRITE_DIGITAL( LPM_LOAD_LAMP,       1 , &CommStatus );

		// 2017.11.28 added by mj
		do 
		{
			// N2 Purge Kit 사용하지 않으면 N2 Purge Skip
			if (( 0 == READ_DIGITAL( PRMD_N2MODE, &CommStatus ))	||
				( 1 != READ_DIGITAL( N2Purge_Control , &CommStatus )))	{N2Use = FALSE;	break;}
			
			// Alarm 상태일 경우 대기하며, Action이 이뤄 질 경우 Ignore를 제외한 나머지는 Abort 
			if ((ALM_PAUSED == ALARM_STATUS( ALARM_N2FRONTABORTTOL + ( (CASSETTE_INDEX - 1) * ALARM_N2POINTER))) ||
			    (ALM_PAUSED == ALARM_STATUS( ALARM_N2REARABORTTOL + ( (CASSETTE_INDEX - 1) * ALARM_N2POINTER))))
			{
				AlmResult = ALARM_MANAGE( ALARM_N2PURGERATEFAIL + ( (CASSETTE_INDEX - 1) * ALARM_N2POINTER));
				if (AlmResult == ALM_IGNORE)	break;
				else return SYS_ABORTED;		
			}

		} while (0);

		LogEvent(g_szModuleName, "LOAD", "", "", "");			// For MARS 2013.06.13
		//...

		m = READ_DIGITAL( LPM_LOADING_SIGNAL , &CommStatus );
		if ( m == 0 ) {
			WRITE_DIGITAL( LPM_LOADING_SIGNAL , 2 , &CommStatus );            
		}

		LogEvent(g_szModuleName, "LPLOAD", "Start", g_szMID, "");	// For MARS 2014.06.26

		// 2017.11.28 added by mj
		if (N2Use == TRUE )	RUN_SET_FUNCTION( CM_N2_PURGE , "N2_MONITOR_START");

		Res = Load_Control( 1 , FALSE , AUTO_MODE );
		
		LogEvent(g_szModuleName, "LPLOAD", "End", g_szMID, "");		// For MARS 2014.06.26

		if ( m == 0 ) {
			WRITE_DIGITAL( LPM_LOADING_SIGNAL , 0 , &CommStatus );
		}

//haeyong 2003. 07. 01
//        if ( Res == SYS_SUCCESS) WRITE_DIGITAL( LPM_LOAD_LAMP,       0 , &CommStatus );
		return Res;
	}	
	else if ( STRCMP_L( RunStr , "UNLOAD" ) ) {
		LogEvent(g_szModuleName, "UNLOAD", "", "", "");		// For MARS 2013.06.13

		// 2017.11.28 added by mj
		if (READ_DIGITAL( N2Purge_Control , &CommStatus ) == 1)
			RUN_SET_FUNCTION( CM_N2_PURGE , "N2_MONITOR_STOP");

//haeyong 2003. 07. 01
//        WRITE_DIGITAL( LPM_UNLOAD_LAMP,       1 , &CommStatus );

		m = READ_DIGITAL( LPM_UNLOADING_SIGNAL , &CommStatus );
		if ( m == 0 ) {
			WRITE_DIGITAL( LPM_UNLOADING_SIGNAL , 2 , &CommStatus );
		}
	
		LogEvent(g_szModuleName, "LPUNLOAD", "Start", g_szMID, "");	// For MARS 2014.06.26

		Res = Load_Control( 0 , FALSE , AUTO_MODE );

		LogEvent(g_szModuleName, "LPUNLOAD", "End", g_szMID, "");	// For MARS 2014.06.26

		if ( m == 0 ) {
			WRITE_DIGITAL( LPM_UNLOADING_SIGNAL , 0 , &CommStatus );
		}

//haeyong 2003. 07. 01
//        if ( Res == SYS_SUCCESS) WRITE_DIGITAL( LPM_UNLOAD_LAMP,       0 , &CommStatus );
		return Res;
	}
	else if ( STRCMP_L( RunStr , "UNLOAD_OPTION" ) ) {
//haeyong 2003. 07. 01
//        WRITE_DIGITAL( LPM_UNLOAD_LAMP,       1 , &CommStatus );

		m = READ_DIGITAL( LPM_UNLOADING_SIGNAL , &CommStatus );
		if ( m == 0 ) {
			WRITE_DIGITAL( LPM_UNLOADING_SIGNAL , 2 , &CommStatus );
		}
		Res = Load_Control( 0 , TRUE , AUTO_MODE );
		if ( m == 0 ) {
			WRITE_DIGITAL( LPM_UNLOADING_SIGNAL , 0 , &CommStatus );
		}

//haeyong 2003. 07. 01
//        if ( Res == SYS_SUCCESS) WRITE_DIGITAL( LPM_UNLOAD_LAMP,       0 , &CommStatus );
		return Res;
	}
	else if ( STRCMP_L( RunStr , "DOCKING"  ) ) {
		return( Dock_Control( 1 ) );
	}
	else if ( STRCMP_L( RunStr , "UNDOCKING" ) ) {
		return( Dock_Control( 0 ) );
	}
	else if ( STRCMP_L( RunStr , "DOCK"  ) ) {
		return( Dock_Control( 1 ) );
	}
	else if ( STRCMP_L( RunStr , "UNDOCK" ) ) {
		return( Dock_Control( 0 ) );
	}
	else if ( STRCMP_L( RunStr , "CLAMP"  ) ) {
		return( Clamp_Control( 1 , FALSE ) );
	}
	else if ( STRCMP_L( RunStr , "UNCLAMP") ) {
		if (READ_DIGITAL( N2Purge_Control , &CommStatus ) == 1)
			RUN_SET_FUNCTION( CM_N2_PURGE , "N2_MONITOR_STOP");
		return( Clamp_Control( 0 , FALSE ) );
	}
	else if ( STRCMP_L( RunStr , "UNCLAMP_OPTION" ) ) {
		return( Clamp_Control( 0 , TRUE ) );
	}
	else if ( STRCMP_L( RunStr , "PRESENT" ) ) {
		return( Cass_Control( 0 ) );
	}
	else if ( STRCMP_L( RunStr , "ABSENT") ) {
		return( Cass_Control( 1 ) );
	}
	else if ( STRCMP_L( RunStr , "MAPPING"   ) ) 
	{
		STR_SEPERATE( PROGRAM_PARAMETER_READ()	, RunStr     , TempStr	, 40 );
		STR_SEPERATE( TempStr	, StationStr , TempStr2	, 40 );
		STR_SEPERATE( TempStr2	, SlotStr	 , ElseStr	, 40 );

		if      ( STRCMP( SlotStr , "OUT" ) ) 
		{
			return( MAPPING( 3 ) );
		}
		else if ( STRCMP( SlotStr , "IN"  ) ) 
		{
			if ( strlen( ElseStr ) > 0 ) 
			{
				return( MAPPING( 2 ) );
			}
			else 
			{
				return( MAPPING( 1 ) );
			}
		}
		else 
		{
			return( MAPPING( 0 ) );
		}
	}
	// Added by jhlee. 2010.03.19
	else if ( STRCMP_L( RunStr , "DOOR_CTRL" ) ) 
	{
		printf(">>>>>>>>>>>>>>>>>>>>> FOUP Door Control Command %s \n <<<<<<<<<<<<<<<<<<<<",PROGRAM_PARAMETER_READ());
		STR_SEPERATE( PROGRAM_PARAMETER_READ()	, RunStr     , TempStr	, 40 );
		STR_SEPERATE( TempStr	, RunStr1 , TempStr2	, 40 );

		if( STRCMP( RunStr1 , "OPEN" ) ) 
			return FOUP_Door_Control(2);
		else if(STRCMP( RunStr1 , "CLOSE" ))
			return FOUP_Door_Control(1);
	}
	//----------------------------------------------------------------------------------------------

	else {
		printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}
	return SYS_SUCCESS;
}
//
//
//
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char Str1[21] , Str2[21] , Str3[21] , ElseStr[21];
	// 2013.07.03
	char szStr4[21], szTemp[] = "1";

	char szStr5[21] = {"0",};

	memset(szStr4, 0, sizeof(szStr4));

	STR_SEPERATE( PROGRAM_ARGUMENT_READ() , Str1 , ElseStr , 20 );
	REPLACE_CHAR_SET( 0 , Str1 );
	STR_SEPERATE( ElseStr , Str2 , Str3 , 20 );
	REPLACE_CHAR_SET( 1 , Str2 );
	CASSETTE_INDEX = Str2[2] - '0';
	ALARM_START_POINTER = atoi( Str3 );

	strcpy(g_szModuleName, Str2);	// 2013.11.15

	if(Str2[2] == '1') strcpy(szStr4,"");
	else strncpy(szStr4,Str2+2,1);
	REPLACE_CHAR_SET( 2 , szStr4 );

	if(Str1[0] == 'L') strncpy(szStr5,Str1+3,1);
	REPLACE_CHAR_SET( 3 , szStr5 );
	
}
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 ){
		SIMULATION_MODE = TRUE;
		//printf("FEM Handling Simulation mode : %d\n",SIMULATION_MODE);
	}
	else SIMULATION_MODE = FALSE;
	return TRUE;
}
