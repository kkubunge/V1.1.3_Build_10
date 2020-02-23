#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include "cimseqnc.h"
#include "iodefine.h"
#include "resource.h"
#include "IPS_Log.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define	ALARM_PORT_COMM_ERROR			60
#define	ALARM_VALID_TIMEOUT_TP6			61 // T6
#define	ALARM_TRREQ_TIMEOUT_TP1			62 // T1
#define	ALARM_BUSY_TIMEOUT_TP2			63 // T2
#define	ALARM_FOUPRECV_TIMEOUT_TP3		64 // T3
#define	ALARM_FOUPREMOVE_TIMEOUT_TP3	65 // T3
#define	ALARM_COMPLETE_TIMEOUT_TP4		66 // T4
#define ALARM_COMPLOFF_TIMEOUT_TP5		67 // T5
#define	ALARM_CSOFF_TIMEOUT_TP5			68 // T5
#define	ALARM_CONTINUE_TIMEOUT_TP6		69 // T6
#define	ALARM_VALIDOFF_ERROR			70
#define	ALARM_FOUP_ABSENT				71 // Unloading, Foup is Absent
#define	ALARM_FOUP_PRESENT				72 // Loading, Foup is Present
#define	ALARM_ABNORMAL_FOUPCHANGE		73
#define	ALARM_HOAVBLOFF_ERROR			74
#define	ALARM_CSOFF_ERROR				75
#define	ALARM_TRREQOFF_ERROR			76
#define	ALARM_BUSYOFF_ERROR				77
#define ALARM_COMPLOFF_ERROR			78
#define	ALARM_CONTOFF_ERROR				79
#define	ALARM_PORT_AGV_ERROR			80
#define	ALARM_AGV_COMM_ERROR			81
#define ALARM_LREQON_ERROR				85
#define	ALARM_SENARIO_ERROR				86
#define	ALARM_PORT_CLAMP_TIMEOUT		91
#define	ALARM_PORT_UNCLAMP_TIMEOUT		92
#define ALARM_LIGHT_CURTAIN_OFF			93
#define	ALARM_FOUP_ABSENT2				95 // Loading, Foup is Absent
#define	ALARM_FOUP_PRESENT2				96 // UnLoading, Foup is Present
#define	ALARM_FOUP_PRESENT_WHEN_LOAD	98
#define	ALARM_FOUP_ABSENT_WHEN_UNLOAD	99
#define	DEFAULT_TIME_OUT				25
#define	DEFAULT_TIME_OUT1				131072  // TimeOut Do Not Check.
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define	PORT_TIME					3
#define	WAIT_TIME					0.1
#define	WAIT_TIME2					0.8
#define	WAIT_TIME3					0.3
#define	WAIT_TIME4					0.5
#define	WAIT_TIME5					1
#define	WAIT_TIME6					0.2
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char *EVENT_STRING_LOAD_COMPLETE		= "EVENT_ON LOAD_COMPLETE_F PORT%d";
char *EVENT_STRING_CLAMP_COMPLETE		= "EVENT_ON LL_CLAMP_COMPLETE PORT%d";
char *EVENT_STRING_UNLOAD_COMPLETE		= "EVENT_ON UNLOAD_COMPLETE_F PORT%d";
char *EVENT_STRING_UNCLAMP_COMPLETE		= "EVENT_ON LL_UNCLAMP_COMPLETE PORT%d";
char *EVENT_STRING_STS_UNOCCUPIED		= "EVENT_ON STS_UNOCCUPIED_F PORT%d";
char *EVENT_STRING_CARR_DELETED			= "EVENT_ON CARR_DELETED_F PORT%d";
char *EVENT_STRING_STS_DELETED			= "EVENT_ON STS_DELETED_F PORT%d";
char *EVENT_STRING_CARR_NOTASSOCIATED	= "EVENT_ON CARR_NOTASSOCIATED_F PORT%d";
char *EVENT_STRING_TR_LOAD_RECV			= "EVENT_ON LOAD_TB_F PORT%d";
char *EVENT_STRING_TR_LOAD_RECV2		= "EVENT_ON LOAD_TB2_F CM";
char *EVENT_STRING_TR_UNLOAD_RECV		= "EVENT_ON UNLOAD_TB_F PORT%d";
char *EVENT_LOAD_REQUEST				= "EVENT_ON LOAD_REQUEST_F PORT%d";
char *EVENT_UNLOAD_REQUEST				= "EVENT_ON UNLOAD_REQUEST_F PORT%d";
char *EVENT_STRING_UNDOCKING			= "EVENT_ON LL_UNDOCKING_COMPLETE CM";
char *EVENT_PORT_CANCEL					= "EVENT_ON CANCEL_F PORT%d"; // jelee add 2007.12.07
char StrBuffer[256];
char Caption[3][10]						= { "Disable" , "LOAD" , "UNLOAD" };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum {
	MON_LOAD_WAIT_CARRIER , 
	MON_LOAD_WAIT_CLAMP ,
	MON_UNLOAD_WAIT_UNCLAMP ,
	MON_UNLOAD_WAIT_CARRIER ,
	MON_LOAD_AGV_WAIT_VALID ,
	MON_LOAD_AGV_WAIT_TRREQ ,
	MON_LOAD_AGV_WAIT_BUSY ,
	MON_LOAD_AGV_WAIT_CARRIER ,
	MON_LOAD_AGV_WAIT_COMPLETE ,
	MON_LOAD_AGV_WAIT_COMPLETE2 ,
	MON_LOAD_AGV_WAIT_CSOFF ,
	MON_LOAD_AGV_WAIT_FINISH , 
	MON_UNLOAD_AGV_WAIT_UNCLAMPED ,
	MON_UNLOAD_AGV_WAIT_VALID , 
	MON_UNLOAD_AGV_WAIT_TRREQ ,
	MON_UNLOAD_AGV_WAIT_BUSY ,
	MON_UNLOAD_AGV_WAIT_CARRIER ,
	MON_UNLOAD_AGV_WAIT_COMPLETE , 
	MON_UNLOAD_AGV_WAIT_COMPLETE2 ,
	MON_UNLOAD_AGV_WAIT_CSOFF
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { RUN		};
enum { SIM		, REAL		};
enum { OFF		, ON		};
enum { DISABLE  , ENABLE	};
enum { UNLOAD	, LOAD		};
enum { MANUAL	, AMHS		};
enum { ABSENT	, PRESENT	};
enum { NOTEXIST , EXIST		};
enum { UNKNOWN	, CLAMP		, UNCLAMP		};
enum { READY	, BUSY		, ERROR1		};
enum { NONE		, RETRY		, COMPLETE2		};
enum { NONE2	, ABSENT2	, PRESENT2		}; // jelee Added.
enum { NONE3	, LOADING	, UNLOADING     , BLINK		};
enum { sUNKNOWN	, sCLAMP	, sUNCLAMP	}; // jelee Added.
enum { RETRY1	, COMPLETE1	};
enum { Enalbe	, Reserved	, Running	}; // jelee add 2007.12.06
enum { FULLUNDOCK = 0, DOORCLOSE = 1, SHUTTLEOUT = 2};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//int		CASSETTE_INDEX		= 1;
int		ALARM_START_POINT	= 100;
int		CONTROL_REFRESH		= FALSE;
int		MONITOR_VIEW		= FALSE;
int		MONITOR_COUNT		= 0;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	CONTROL_AUTO_MODE		= FALSE;
BOOL	SIMULATION_AGV_MODE		= FALSE;
BOOL	SIMULATION_CASS_MODE	= FALSE;
BOOL	SIMULATION_LSWITCH_MODE = FALSE;
BOOL	SIMULATION_USWITCH_MODE = FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
HWND	hBtn;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int		  CASSETTE_INDEX		= 1;
int		  ALARM_START_POINTER	= 100;
int		  Current				= NONE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"			, _K_D_IO	,	IO_Driver_Loading	,	0	} ,
	//
	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER			,	0	} ,
	//
	{ "TMC_$0_MID_CONTROL"			, _K_F_IO	,	MID_CONTROL			,	0	} ,
	//
	{ "$0.LOADING_SIGNAL"			, _K_D_IO	,	LP_LOADING_SIGNAL	,	0	} ,
	{ "$0.UNLOADING_SIGNAL"			, _K_D_IO	,	LP_UNLOADING_SIGNAL	,	0	} ,
	{ "$0.PROCESS_END"				, _K_D_IO	,	LP_PROCESS_END		,	0	} ,
	{ "$0.OPERATOR_CANCEL"			, _K_D_IO	,	LP_OPERATOR_CANCEL	,	0	} ,
	{ "$0.AMHS_MOVING"				, _K_D_IO	,	LP_AMHS_MOVING		,	0	} ,
	{ "CTC.FA_STATUS_MDL_$0"		, _K_D_IO	,	LP_MDL_STATUS		,	0	} ,
	//
	{ "$1.AMHS_HO_AVBL_DO"			, _K_D_IO	,	LP_AGV_OUTPUT_HOAVBL,	0	} ,
	{ "$1.AMHS_ES_DO"				, _K_D_IO	,	LP_AGV_OUTPUT_ESTOP	,	0	} ,
	{ "$1.AMHS_L_REQ_DO"			, _K_D_IO	,	LP_AGV_OUTPUT_LREQ	,	0	} ,
	{ "$1.AMHS_U_REQ_DO"			, _K_D_IO	,	LP_AGV_OUTPUT_UREQ	,	0	} ,
	{ "$1.AMHS_READY_DO"			, _K_D_IO	,	LP_AGV_OUTPUT_READY	,	0	} ,
	
	{ "$1.AMHS_CS_0_DI"				, _K_D_IO	,	LP_AGV_INPUT_CS		,	0	} ,
	{ "$1.AMHS_CS_1_DI"				, _K_D_IO	,	LP_AGV_INPUT_CS1	,	0	} ,
	{ "$1.AMHS_VALID_DI"			, _K_D_IO	,	LP_AGV_INPUT_VALID	,	0	} ,
	{ "$1.AMHS_TR_REQ_DI"			, _K_D_IO	,	LP_AGV_INPUT_TRREQ	,	0	} ,
	{ "$1.AMHS_BUSY_DI"				, _K_D_IO	,	LP_AGV_INPUT_BUSY	,	0	} ,
	{ "$1.AMHS_COMPT_DI"			, _K_D_IO	,	LP_AGV_INPUT_COMPLETE ,	0	} ,
	{ "$1.AMHS_CONT_DI"				, _K_D_IO	,	LP_AGV_INPUT_CONT	,	0	} ,

	{ "EFEM.AMHS_TP1"				, _K_D_IO	,	AMHS_TP1TIMEOUT		,	0	} ,
	{ "EFEM.AMHS_TP2"				, _K_D_IO	,	AMHS_TP2TIMEOUT		,	0	} ,
	{ "EFEM.AMHS_TP3"				, _K_D_IO	,	AMHS_TP3TIMEOUT		,	0	} ,
	{ "EFEM.AMHS_TP4"				, _K_D_IO	,	AMHS_TP4TIMEOUT		,	0	} ,
	{ "EFEM.AMHS_TP5"				, _K_D_IO	,	AMHS_TP5TIMEOUT		,	0	} ,
	{ "EFEM.AMHS_TP6"				, _K_D_IO	,	AMHS_TP6TIMEOUT		,	0	} ,

	{ "$1.AGV_MODE_SET"				, _K_D_IO	,	LP_MODE_AGV_SET		,	0	} ,
	{ "CTC.FA_STATUS_AGV_$0"		, _K_D_IO	,	LP_MODE_AGV_STS		,	0	} ,

	{ "$1.AGV_STATUS"				, _K_D_IO	,	LP_AGV_STS			,	0	} ,
	{ "$1.AGV_ERROR"				, _K_D_IO	,	LP_AGV_ERROR		,	0	} ,

	{ "$1.AGV_ERR_CLEAR"			, _K_D_IO	,	PIO_ERR_CLEAR		,	0	} ,

	{ "$1.F_ProperlyPlaceDI"		, _K_D_IO	,	LP_FOUP_STS			,	0	} ,
	{ "$1.FOUP_PresentDI"			, _K_D_IO	,	PRESENT_STS			,	0	} ,

	{ "$1.AlarmStatus"				, _K_D_IO	,	LP_ALARM_STS		,	0	} ,

	{ "LPA.AlarmStatus"				, _K_D_IO	,	LP1_ALARM_STS		,	0	} ,
	{ "LPB.AlarmStatus"				, _K_D_IO	,	LP2_ALARM_STS		,	0	} ,
	{ "LPC.AlarmStatus"				, _K_D_IO	,	LP3_ALARM_STS		,	0	} ,

	{ "$1.ClampUnclampXO"			, _K_D_IO	,	LP_CLAMP			,	0	} ,
	{ "$1.ClampUnclampXI"			, _K_D_IO	,	LP_CLAMP_STS		,	0	} ,

	{ "$1.Switch2LoadDI"			, _K_D_IO	,	LP_LOAD_BUTTON		,	0	} ,
	{ "$1.Switch1UnloadDI"			, _K_D_IO	,	LP_UNLOAD_BUTTON	,	0	} ,

	{ "$1.Lamp2OnDIO"				, _K_D_IO	,	LP_UNLOAD_LAMP		,	0	} ,
	{ "$1.Lamp1OnDIO"				, _K_D_IO	,	LP_LOAD_LAMP		,	0	} ,

	{ "EFEM.LightCurtainDI"			, _K_D_IO	,   Light_Curtain		,	0   } ,

	{ "LPMA.UndockDO"				, _K_D_IO	,	LPMAUnDockCmd		,	0	} , //2010.08.10
	{ "LPMB.UndockDO"				, _K_D_IO	,	LPMBUnDockCmd		,	0	} ,
	{ "LPMC.UndockDO"				, _K_D_IO	,	LPMCUnDockCmd		,	0	} ,
	{ "FC_CM1_MID_CONTROL"			, _K_D_IO	,	FC_CM1_MID_CONTROL	,	0	} ,
	{ "FC_CM2_MID_CONTROL"			, _K_D_IO	,	FC_CM2_MID_CONTROL	,	0	} ,
	{ "FC_CM3_MID_CONTROL"			, _K_D_IO	,	FC_CM3_MID_CONTROL	,	0	} ,

	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void MONITOR( int mode ) {
	if ( !MONITOR_VIEW ) {
		MONITOR_COUNT = 0;
		return;
	}
	MONITOR_COUNT++;
	if ( MONITOR_COUNT > 3 ) MONITOR_VIEW = FALSE;
	switch( mode ) {
	case MON_LOAD_WAIT_CARRIER :
		printf( "=====> CHECKING CM%d LOAD_WAIT_CARRIER\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_WAIT_CLAMP :
		printf( "=====> CHECKING CM%d LOAD_WAIT_CLAMP\n" , CASSETTE_INDEX );
		break;
	//====
	case MON_UNLOAD_WAIT_UNCLAMP :
		printf( "=====> CHECKING CM%d UNLOAD_WAIT_UNCLAMP\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_WAIT_CARRIER :
		printf( "=====> CHECKING CM%d UNLOAD_WAIT_CARRIER\n" , CASSETTE_INDEX );
		break;
	//====
	case MON_LOAD_AGV_WAIT_VALID :
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_VALID\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_AGV_WAIT_TRREQ :
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_TRREQ\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_AGV_WAIT_BUSY :
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_BUSY\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_AGV_WAIT_CARRIER :
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_CARRIER\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_AGV_WAIT_COMPLETE :
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_COMPLETE\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_AGV_WAIT_COMPLETE2 :
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_COMPLETE2\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_AGV_WAIT_CSOFF : 
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_CSOFF\n" , CASSETTE_INDEX );
		break;
	case MON_LOAD_AGV_WAIT_FINISH : 
		printf( "=====> CHECKING CM%d LOAD_AGV_WAIT_VALID\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_UNCLAMPED :
		printf( "=====> CHECKING CM%d UNLOAD_AGV_WAIT_UNCLAMPED\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_VALID : 
		printf( "=====> CHECKING CM%d UNLOAD_AGV_WAIT_VALID\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_TRREQ :
		printf( "=====> CHECKING CM%d UNLOAD_AGV_WAIT_TRREQ\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_BUSY :
		printf( "=====> CHECKING CM%d UNLOAD_AGV_WAIT_BUSY\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_CARRIER : 
		printf( "=====> CHECKING CM%d UNLOAD_AGV_WAIT_CARRIER\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_COMPLETE : 
		printf( "=====> CHECKING CM%d UNLOAD_AGV_WAIT_COMPLETE\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_COMPLETE2 : 
		printf( "=====> CHECKING CM%d UNLOAD_AGV_WAIT_COMPLETE2\n" , CASSETTE_INDEX );
		break;
	case MON_UNLOAD_AGV_WAIT_CSOFF : 
		printf( "=====> CHECKING CM%d MON_UNLOAD_AGV_WAIT_CSOFF\n" , CASSETTE_INDEX );
		break;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_TRUE2( int a , int *CommStatus ) {
	char pszFtnName[256] = ""; // jelee added 2005.11.11
	int data;
	data = READ_DIGITAL( a , CommStatus );
//	LOG(pszFtnName, ELT_File, "[LPM%d] READ DISGITAL_TRUE2 = 1", CASSETTE_INDEX);
	if ( *CommStatus ) {
		return data;
	}

//	_sleep(500);
	_sleep(250);
	
//	LOG(pszFtnName, ELT_File, "[LPM%d] READ DISGITAL_TRUE2 = 2 after sleep 250", CASSETTE_INDEX);
	data = READ_DIGITAL( a , CommStatus );
	if ( *CommStatus ) {
		return data;
	}

//	_sleep(1000);
	_sleep(500);
//	LOG(pszFtnName, ELT_File, "[LPM%d] READ DISGITAL_TRUE2 = 3 after sleep 500", CASSETTE_INDEX);
	data = READ_DIGITAL( a , CommStatus );
	if ( *CommStatus ) {
		return data;
	}
	
	return data;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_DOUBLE( int a , int b , int *CommStatus ) {
	int i , data = 0;
	for ( i = 0 ; i < 2 ; i++ ) {
		data = ( READ_DIGITAL( a , CommStatus ) * 2 );
		data = data + READ_DIGITAL( b , CommStatus );
		if ( *CommStatus ) return data;
	}
	return data;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void WRITE_DIGITAL_DOUBLE( int a , int b , int SetValue , int *CommStatus ) {
	WRITE_DIGITAL( a , (int)(SetValue / 2) , CommStatus );
	WRITE_DIGITAL( b , (int)(SetValue % 2) , CommStatus );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int ALARM_MANAGE_for_CarrHandling( int id ) {
	int AlmResult;
	if ( CONTROL_REFRESH ) return SYS_ABORTED;
	ALARM_POST( id );
	while ( TRUE ) {
		_sleep(2);
		AlmResult = ALARM_STATUS( id );
		if ( AlmResult != ALM_PAUSED ) break;
		if ( CONTROL_REFRESH ) {
			ALARM_CLEAR( id );
			return SYS_ABORTED;
		}
	}
	return AlmResult;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void AGV_INIT_SET_IO( int mode ) {
	int CommStatus;

	switch( mode ) {
	case -1 : // AGV Possible
		WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , ON  , &CommStatus ); // avaliable
		WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP  , ON  , &CommStatus ); // not Emergency stop
		WRITE_DIGITAL( LP_AGV_OUTPUT_LREQ   , OFF , &CommStatus ); 
		WRITE_DIGITAL( LP_AGV_OUTPUT_UREQ   , OFF , &CommStatus ); 
		WRITE_DIGITAL( LP_AGV_OUTPUT_READY  , OFF , &CommStatus ); 
		break;
	case -2 : // Manual Posiblet
		WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP  , ON  , &CommStatus ); // not Emergency stop
		WRITE_DIGITAL( LP_AGV_OUTPUT_LREQ   , OFF , &CommStatus ); 
		WRITE_DIGITAL( LP_AGV_OUTPUT_UREQ   , OFF , &CommStatus ); 
		WRITE_DIGITAL( LP_AGV_OUTPUT_READY  , OFF , &CommStatus ); 
		break;
	case -3 : // Emergency
		WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP  , OFF , &CommStatus ); // Emergency stop
		WRITE_DIGITAL( LP_AGV_OUTPUT_LREQ   , OFF , &CommStatus ); 
		WRITE_DIGITAL( LP_AGV_OUTPUT_UREQ   , OFF , &CommStatus ); 
		WRITE_DIGITAL( LP_AGV_OUTPUT_READY  , OFF , &CommStatus ); 
		break;

	case MON_LOAD_AGV_WAIT_VALID :
		break;
	case MON_LOAD_AGV_WAIT_TRREQ :
		break;
	case MON_LOAD_AGV_WAIT_BUSY :
		break;
	case MON_LOAD_AGV_WAIT_CARRIER :
		break;
	case MON_LOAD_AGV_WAIT_COMPLETE :
		break;
	case MON_LOAD_AGV_WAIT_COMPLETE2 :
		break;
	case MON_LOAD_AGV_WAIT_FINISH : 
		break;
	case MON_UNLOAD_AGV_WAIT_VALID : 
		break;
	case MON_UNLOAD_AGV_WAIT_TRREQ :
		break;
	case MON_UNLOAD_AGV_WAIT_BUSY :
		break;
	case MON_UNLOAD_AGV_WAIT_CARRIER : 
		break;
	case MON_UNLOAD_AGV_WAIT_COMPLETE : 
		break;
	case MON_UNLOAD_AGV_WAIT_COMPLETE2 : 
		break;
	case MON_UNLOAD_AGV_WAIT_CSOFF : 
		break;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int FOUP_SENSOR_CHECK( int *CommStatus ) { // return 0 = None, 1 = Absent, 2 = Present
//	int Data1 , Data2 , CommStatus1 , CommStatus2;
	int Data1 ,CommStatus1;
	char pszFtnName[256] = ""; // jelee added 2005.11.11
	Data1 = READ_DIGITAL( LP_FOUP_STS , &CommStatus1 ); // 0 = Absent, 1 = Present
	if(Data1 == 1) {
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_CHECK FOUP Placement Sensor Data1 = Present", CASSETTE_INDEX);
	}
	else {
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_CHECK FOUP Placement Sensor Data1 = Absent", CASSETTE_INDEX);
	}
/*	Data2 = READ_DIGITAL( PRESENT_STS , &CommStatus2 ); // 0 = Absent, 1 = Present
	if(Data2 == 1)
	{
		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Presence Sensor ON", CASSETTE_INDEX);
	}
	if ( CommStatus1 && CommStatus2 ) { 
		*CommStatus = 1;
		if		( Data1 == PRESENT && Data2 == PRESENT ) return PRESENT2;
		else if ( Data1 == ABSENT  && Data2 == ABSENT  ) return ABSENT2; 
		else return NONE2;
*/
	if ( CommStatus1 ) { 
		*CommStatus = 1;
		if( Data1 == PRESENT ) {
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_CHECK FOUP Placement Sensor ON [Data1 = PRESENT]", CASSETTE_INDEX);
			return PRESENT2;
		}
		else {//if( Data1 == ABSENT ) 
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_CHECK FOUP Placement Sensor OFF [Data1 = ABSENT]", CASSETTE_INDEX);
			return ABSENT2;
		}
//		else 
//			return NONE2;
//		if		( Data1 == PRESENT ) return PRESENT2;
//		else if ( Data1 == ABSENT  ) return ABSENT2; 
//		else return NONE2;
	}
	else { 
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_CHECK FOUP Placement Sensor OFF [CommStatus = 0]", CASSETTE_INDEX);
		*CommStatus = 0;
		return NONE2;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*Module_Status FOUP_SENSOR_CHECK2( BOOL Present , BOOL Agv ) {
	int Check_Value , Alarm , Data , AlmResult;
	if ( Present ) { Check_Value = PRESENT; Alarm = ALARM_FOUP_PRESENT; } // 3 = present
	else { Check_Value = ABSENT; Alarm = ALARM_FOUP_ABSENT; }
	while( TRUE ) {
		if ( READ_DIGITAL_TRUE2( LP_FOUP_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data == Check_Value ) {
			AlmResult = ALARM_MANAGE_FREE( ALARM_START_POINTER + Alarm );
			if      ( AlmResult == ALM_IGNORE ) return SYS_ERROR;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	return SYS_SUCCESS;
}
*/ //------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status FOUP_SENSOR_ERROR( BOOL Present , BOOL HO_AVAL ) {
	int AlmResult , CommStatus , Alarm , Check_Value;
	char pszFtnName[256] = "";
	BOOL Check = FALSE;
	if ( Present ) { 
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR 1 [Absent2]", CASSETTE_INDEX);
		Check_Value = ABSENT2; 
		Alarm = ALARM_FOUP_PRESENT;
	} 
	else { 
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR 2 [Present2]", CASSETTE_INDEX);
		Check_Value = PRESENT2; 
		Alarm = ALARM_FOUP_ABSENT; 
	}
	
	while ( TRUE ) {
		if ( FOUP_SENSOR_CHECK( &CommStatus ) != Check_Value ) {
			_sleep(1000); // jelee add 2006.04.20
			if( FOUP_SENSOR_CHECK( &CommStatus ) != Check_Value ) {
				_sleep(1000); // jelee add 2006.04.20
				if( FOUP_SENSOR_CHECK( &CommStatus ) != Check_Value ) {
//					LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR 2 FOUP_SENSOR_CHECK_ERROR ===========> Different FOUP State", CASSETTE_INDEX);
					if ( HO_AVAL ) { 
						Check = TRUE; 
						AGV_INIT_SET_IO( -2 );
					}
					ALARM_POST( ALARM_START_POINTER + Alarm );
					WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable //jelee added 2005.10.14
					while( TRUE ) {
						_sleep( 100 );
//						LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR 2", CASSETTE_INDEX);
						if ( ALARM_STATUS( ALARM_START_POINTER + Alarm ) != ALM_PAUSED ) {// Get finally Alarm Recovery Status
//							LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR 3", CASSETTE_INDEX);
							AlmResult = ALARM_STATUS( ALARM_START_POINTER + Alarm ); // Get finally Alarm Recovery Status
							break;
						}
					} // recovery is only ignore and abbort
					
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] Alarm Recovery = Complete Option", CASSETTE_INDEX);
						return SYS_ERROR;
					}
					else { // Retry Option
// 						LOG(pszFtnName, ELT_File, "[LPM%d] Alarm Recovery = Retry Option", CASSETTE_INDEX);
						return SYS_ABORTED;
					}
				}
			}
		}
		else {
			break;
		}
	}
	if ( HO_AVAL && Check ) {
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR 4", CASSETTE_INDEX);		
		WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , ON , &CommStatus );
	}
	//	LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR 5", CASSETTE_INDEX);
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status FOUP_SENSOR_ERROR2( BOOL Present , BOOL HO_AVAL ) {
	int AlmResult , CommStatus , Alarm , Check_Value;
	char pszFtnName[256] = "";
	BOOL Check = FALSE;
	if ( Present ) { 
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  1[Absent2]", CASSETTE_INDEX);
		Check_Value = ABSENT2; 
		Alarm = ALARM_FOUP_PRESENT_WHEN_LOAD;
	} 
	else { 
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  2[Present2]", CASSETTE_INDEX);
		Check_Value = PRESENT2; 
		Alarm = ALARM_FOUP_ABSENT_WHEN_UNLOAD; 
	}
	
	while ( TRUE ) {
		if ( FOUP_SENSOR_CHECK( &CommStatus ) != Check_Value ) {
			_sleep(1000); // jelee add 2006.04.20
			if( FOUP_SENSOR_CHECK( &CommStatus ) != Check_Value ) {
				_sleep(1000); // jelee add 2006.04.20
				if( FOUP_SENSOR_CHECK( &CommStatus ) != Check_Value ) {
//					LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  3===========> Different FOUP State", CASSETTE_INDEX);
					if ( HO_AVAL ) { 
						Check = TRUE; 
						AGV_INIT_SET_IO( -2 );
					}
					ALARM_POST( ALARM_START_POINTER + Alarm );
					WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable //jelee added 2005.10.14
					while( TRUE ) {
						_sleep( 100 );
//						LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  4", CASSETTE_INDEX);
						if ( ALARM_STATUS( ALARM_START_POINTER + Alarm ) != ALM_PAUSED ) {
//							LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  5", CASSETTE_INDEX);
							AlmResult = ALARM_STATUS( ALARM_START_POINTER + Alarm );
							break;
						}
					} // recovery is only ignore and abbort
					
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  6Alarm Recovery = Complete Option", CASSETTE_INDEX);
						return SYS_ERROR;
					}
					else {// Retry Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  7Alarm Recovery = Retry Option", CASSETTE_INDEX);
						return SYS_ABORTED;
					}
				}
			}
		}
		else {
			break;
		}
	}
	if ( HO_AVAL && Check ) {
//		LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  8", CASSETTE_INDEX);		
		WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , ON , &CommStatus );
	}
//	LOG(pszFtnName, ELT_File, "[LPM%d] FOUP_SENSOR_ERROR2  9", CASSETTE_INDEX);
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status AGV_RECOVERY_SCENARIO( int LoadUnLoad ) {
	char pszFtnName[256] = "";
	int CarrierSts = 0 , CommStatus , Data = 0 , Count = 0;
	int AlmResult, Status;
	
//	LOG(pszFtnName, ELT_File, "[LPM%d] AGV_RECOVERY_SCENARIO 1", CASSETTE_INDEX);
	while( TRUE ) {
		Data = READ_DIGITAL( LP_FOUP_STS , &CommStatus ) ;
		if( CommStatus ) { 
			if ( Data == EXIST ) {
//				LOG(pszFtnName, ELT_File, "[LPM%d] AGV_RECOVERY_SCENARIO 1 [CarrierSts = PRESENT]", CASSETTE_INDEX);
				CarrierSts = PRESENT;
				break;
			}
			else {
//				LOG(pszFtnName, ELT_File, "[LPM%d] AGV_RECOVERY_SCENARIO 2 [CarrierSts = ABSENT]", CASSETTE_INDEX);
				CarrierSts = ABSENT;
				break; 
			}
		}
		else {
			Count++;
			if ( Count > 5 ) {
				Count = 0;
//				LOG(pszFtnName, ELT_File, "[LPM%d] AGV_RECOVERY_SCENARIO 3 [CommStatus = 0]", CASSETTE_INDEX);
				ALARM_POST( ALARM_START_POINTER + ALARM_PORT_COMM_ERROR );
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable //jelee added 2005.10.14
				return SYS_ABORTED;
			}
		} 
		_sleep(250);
	}
	switch( LoadUnLoad ) {
		case LOAD :
			if ( CarrierSts == PRESENT ) {
//				LOG(pszFtnName, ELT_File, "[LPM%d] AGV_RECOVERY_SCENARIO 4 LOAD case [CarrierSta = PRESENT]", CASSETTE_INDEX);
				{
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
//						LOG(pszFtnName, ELT_File, "======> [LPM%d] AGV_RECOVERY_SCENARIO 5 LOAD case [CarrierSta = PRESENT]", CASSETTE_INDEX);
						return SYS_SUCCESS;
					}
					else {// Retry Option
//						LOG(pszFtnName, ELT_File, "======> [LPM%d] AGV_RECOVERY_SCENARIO 6 LOAD case [CarrierSta = PRESENT]", CASSETTE_INDEX);
						Status = AGV_RECOVERY_SCENARIO( LOAD );
						if ( Status != SYS_SUCCESS ) {
//							LOG(pszFtnName, ELT_File, "======> [LPM%d] AGV_RECOVERY_SCENARIO 7 LOAD case [CarrierSta = PRESENT]", CASSETTE_INDEX);
							return SYS_ERROR; // No Foup
						}
						else { // SYS_SUCCESS, Foup Present
//							LOG(pszFtnName, ELT_File, "======> [LPM%d] AGV_RECOVERY_SCENARIO 8 LOAD case [CarrierSta = PRESENT]", CASSETTE_INDEX);
							//-------------------------------------------------------------------------------
							sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
							WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
							//-------------------------------------------------------------------------------
							return SYS_SUCCESS;
						}
					}
				}
//				LOG(pszFtnName, ELT_File, "======> [LPM%d] AGV_RECOVERY_SCENARIO 9 LOAD case [CarrierSta = PRESENT]", CASSETTE_INDEX);
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable //jelee added 2005.10.14
				return SYS_ABORTED;// Send Load Request;	
			}
			else {	
//				LOG(pszFtnName, ELT_File, "======> 6 [LPM%d] AGV_RECOVERY_SCENARIO LOAD case [CarrierSta = ABSENT]", CASSETTE_INDEX);
				{
//					LOG(pszFtnName, ELT_File, "======> 7 [LPM%d] AGV_RECOVERY_SCENARIO LOAD case [CarrierSta = ABSENT]", CASSETTE_INDEX);
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_ABSENT2 );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
//						LOG(pszFtnName, ELT_File, "======> 8 [LPM%d] AGV_RECOVERY_SCENARIO LOAD case [CarrierSta = ABSENT]", CASSETTE_INDEX);
						Status = AGV_RECOVERY_SCENARIO( LOAD );
						if ( Status != SYS_SUCCESS ) {
//							LOG(pszFtnName, ELT_File, "======> 9 [LPM%d] AGV_RECOVERY_SCENARIO LOAD case [CarrierSta = ABSENT]", CASSETTE_INDEX);
							return SYS_ERROR; // No Foup
						}
						else { // SYS_SUCCESS, Foup Present
//							LOG(pszFtnName, ELT_File, "======> 10 [LPM%d] AGV_RECOVERY_SCENARIO LOAD case [CarrierSta = ABSENT]", CASSETTE_INDEX);
							//-------------------------------------------------------------------------------
							sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
							WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
							//-------------------------------------------------------------------------------
							return SYS_SUCCESS;
						}
					}
					else {// Retry Option
//						LOG(pszFtnName, ELT_File, "======> 11 [LPM%d] AGV_RECOVERY_SCENARIO LOAD case [CarrierSta = ABSENT]", CASSETTE_INDEX);
						return SYS_ABORTED;
					}
				}
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable //jelee added 2005.10.14
				return SYS_ABORTED;// Send Load Request;	
			}
		case UNLOAD :
			if ( CarrierSts == PRESENT ) {
//				LOG(pszFtnName, ELT_File, "======> 12 [LPM%d] AGV_RECOVERY_SCENARIO UNLOAD case [CarrierSta = PRESENT]", CASSETTE_INDEX);
				{
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT2 );
					if ( AlmResult == ALM_IGNORE ) {
						Status = AGV_RECOVERY_SCENARIO( UNLOAD );
						if ( Status != SYS_SUCCESS ) {
							return SYS_ERROR; // No Foup
						}
						else { // SYS_SUCCESS, Foup Present
							//-------------------------------------------------------------------------------
							sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
							WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
							//-------------------------------------------------------------------------------
							return SYS_SUCCESS;
						}
					}
					else {
						return SYS_ABORTED;
					}
				}
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable //jelee added 2005.10.14
				return SYS_ABORTED;// Send UnLoad Request;
			}
			else {
//				LOG(pszFtnName, ELT_File, "=====> 13 [LPM%d] AGV_RECOVERY_SCENARIO UNLOAD case [CarrierSta = ABSENT]", CASSETTE_INDEX);
				AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_ABSENT );
				if ( AlmResult == ALM_IGNORE ) {
					return SYS_SUCCESS; // Send UnLoad Complete;
				}
				else {
					Status = AGV_RECOVERY_SCENARIO( UNLOAD );
					if ( Status != SYS_SUCCESS ) {
						return SYS_ERROR; // No Foup
					}
					else { // SYS_SUCCESS, Foup Present
						//-------------------------------------------------------------------------------
						sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
						WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
						//-------------------------------------------------------------------------------
						return SYS_SUCCESS; // Send UnLoad Complete;
					}
				}
			}
	}
	return SYS_ABORTED;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status AGV_RECOVERY_SCENARIO2( int LoadUnLoad, int RetryComplete ) {
	char pszFtnName[256] = "";
	int CarrierSts = 0 , CommStatus , Data = 0 , Count = 0;
	int AlmResult, Status;
	
	while( TRUE ) {
		Data = READ_DIGITAL( LP_FOUP_STS , &CommStatus ) ;
		if( CommStatus ) { 
			if ( Data == EXIST ) {
				CarrierSts = PRESENT;
				break;
			}
			else {
				CarrierSts = ABSENT;
				break; 
			}
		}
		else {
			Count++;
			if ( Count > 5 ) {
				Count = 0;
				ALARM_POST( ALARM_START_POINTER + ALARM_PORT_COMM_ERROR );
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus );
				return SYS_ABORTED;
			}
		} 
		_sleep(250);
	}
	switch( LoadUnLoad ) {
		case LOAD :
			if ( CarrierSts == PRESENT ) {
				if ( RetryComplete == RETRY1 ) {
//					LOG(pszFtnName, ELT_File, "======>#1 [LPM%d] AGV_RECOVERY_SCENARIO2 LOAD case RETRY [CarrierSta = PRESENT]", CASSETTE_INDEX);
					return SYS_ERROR; // No POUP
				}
				else {
//					LOG(pszFtnName, ELT_File, "======>#2 [LPM%d] AGV_RECOVERY_SCENARIO2 LOAD case COMPLETE [CarrierSta = PRESENT]", CASSETTE_INDEX);
					return SYS_SUCCESS; // FOUP Present
				}
			}
			else {
				if ( RetryComplete == RETRY1 ) {
//					LOG(pszFtnName, ELT_File, "======>#3 [LPM%d] AGV_RECOVERY_SCENARIO2 LOAD case RETRY [CarrierSta = ABSENT]", CASSETTE_INDEX);
					return SYS_ERROR; // No POUP
				}
				else {
//					LOG(pszFtnName, ELT_File, "======>#4 [LPM%d] AGV_RECOVERY_SCENARIO2 LOAD case COMPLETE [CarrierSta = ABSENT]", CASSETTE_INDEX);
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_ABSENT2 );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
						Status = AGV_RECOVERY_SCENARIO( LOAD );
						return Status;
					}
					else {// Retry Option
//						LOG(pszFtnName, ELT_File, "======>#5 [LPM%d] AGV_RECOVERY_SCENARIO2 LOAD case COMPLETE [CarrierSta = ABSENT]", CASSETTE_INDEX);
						return SYS_ERROR;
					}
				}
			}
		case UNLOAD :
			if ( CarrierSts == PRESENT ) {
				if ( RetryComplete == RETRY1 ) { // Retry
//					LOG(pszFtnName, ELT_File, "======>#6 [LPM%d] AGV_RECOVERY_SCENARIO2 UNLOAD case RETRY [CarrierSta = PRESENT]", CASSETTE_INDEX);
					return SYS_SUCCESS; // FOUP Present
				}
				else { // Complete
//					LOG(pszFtnName, ELT_File, "======>#7 [LPM%d] AGV_RECOVERY_SCENARIO2 UNLOAD case COMPLETE [CarrierSta = PRESENT]", CASSETTE_INDEX);
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT2 );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
						Status = AGV_RECOVERY_SCENARIO( UNLOAD );
						return Status;
					}
					else { // Retry Option
						return SYS_ERROR;
					}
				}
			}
			else {
				if ( RetryComplete == RETRY1 ) { // No FOUP
//					LOG(pszFtnName, ELT_File, "======>#8 [LPM%d] AGV_RECOVERY_SCENARIO2 UNLOAD case RETRY [CarrierSta = ABSENT]", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else { // Complete Option
//					LOG(pszFtnName, ELT_File, "======>#9 [LPM%d] AGV_RECOVERY_SCENARIO2 UNLOAD case COMPLETE [CarrierSta = ABSENT]", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
	}
	return SYS_ABORTED;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status AGV_RECOVERY_SCENARIO3( int LoadUnLoad, int RetryComplete ) {
	char pszFtnName[256] = "";
	int CarrierSts = 0 , CommStatus , Data = 0 , Count = 0;
	int AlmResult, Status;
	
	while( TRUE ) {
		Data = READ_DIGITAL( LP_FOUP_STS , &CommStatus ) ;
		if( CommStatus ) { 
			if ( Data == EXIST ) {
				CarrierSts = PRESENT;
				break;
			}
			else {
				CarrierSts = ABSENT;
				break; 
			}
		}
		else {
			Count++;
			if ( Count > 5 ) {
				Count = 0;
				ALARM_POST( ALARM_START_POINTER + ALARM_PORT_COMM_ERROR );
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus );
				return SYS_ABORTED;
			}
		} 
		_sleep(250);
	}
	switch( LoadUnLoad ) {
		case LOAD :
			if ( CarrierSts == PRESENT ) {
				if ( RetryComplete == RETRY1 ) {
//					LOG(pszFtnName, ELT_File, "======>#1 [LPM%d] AGV_RECOVERY_SCENARIO3 LOAD case RETRY [CarrierSta = PRESENT]", CASSETTE_INDEX);
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
						Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
						return Status;
					}
					else {// Retry Option
//						LOG(pszFtnName, ELT_File, "======>#2 [LPM%d] AGV_RECOVERY_SCENARIO3 LOAD case COMPLETE [CarrierSta = ABSENT]", CASSETTE_INDEX);
						Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
						return Status;
//						return SYS_ERROR;
					}
				}
				else {
//					LOG(pszFtnName, ELT_File, "======>#3 [LPM%d] AGV_RECOVERY_SCENARIO3 LOAD case COMPLETE [CarrierSta = PRESENT]", CASSETTE_INDEX);
					return SYS_SUCCESS; // FOUP Present
				}
			}
			else {
				if ( RetryComplete == RETRY1 ) {
//					LOG(pszFtnName, ELT_File, "======>#4 [LPM%d] AGV_RECOVERY_SCENARIO3 LOAD case RETRY [CarrierSta = ABSENT]", CASSETTE_INDEX);
					return SYS_ERROR; // No POUP
				}
				else {
//					LOG(pszFtnName, ELT_File, "======>#5 [LPM%d] AGV_RECOVERY_SCENARIO3 LOAD case COMPLETE [CarrierSta = ABSENT]", CASSETTE_INDEX);
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_ABSENT2 );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
						Status = AGV_RECOVERY_SCENARIO( LOAD );
						return Status;
					}
					else {// Retry Option
//						LOG(pszFtnName, ELT_File, "======>#6 [LPM%d] AGV_RECOVERY_SCENARIO3 LOAD case COMPLETE [CarrierSta = ABSENT]", CASSETTE_INDEX);
						return SYS_ERROR;
					}
				}
			}
		case UNLOAD :
			if ( CarrierSts == PRESENT ) {
				if ( RetryComplete == RETRY1 ) { // Retry
//					LOG(pszFtnName, ELT_File, "======>#7 [LPM%d] AGV_RECOVERY_SCENARIO3 UNLOAD case RETRY [CarrierSta = PRESENT]", CASSETTE_INDEX);
//					return SYS_SUCCESS; // FOUP Present
					return SYS_ERROR;
				}
				else { // Complete
//					LOG(pszFtnName, ELT_File, "======>#8 [LPM%d] AGV_RECOVERY_SCENARIO3 UNLOAD case COMPLETE [CarrierSta = PRESENT]", CASSETTE_INDEX);
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT2 );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
						Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
						return Status;
					}
					else { // Retry Option
						return SYS_ERROR;
					}
				}
			}
			else {
				if ( RetryComplete == RETRY1 ) { // No FOUP
//					LOG(pszFtnName, ELT_File, "======>#9 [LPM%d] AGV_RECOVERY_SCENARIO2 UNLOAD case RETRY [CarrierSta = ABSENT]", CASSETTE_INDEX);
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_ABSENT );
					if ( AlmResult == ALM_IGNORE ) { // Complete Option
						Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
						return Status;
					}
					else { // Retry Option
						Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
						return Status;
//						return SYS_ERROR;
					}
				}
				else { // Complete Option
//					LOG(pszFtnName, ELT_File, "======>#10 [LPM%d] AGV_RECOVERY_SCENARIO2 UNLOAD case COMPLETE [CarrierSta = ABSENT]", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
	}
	return SYS_ABORTED;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL ALARM_MESSAGE_REMAPPING( int id , int *Error ) {
	char Buffer[256] , Buffer2[32];
	int CommStatus;
	*Error = READ_DIGITAL( LP_AGV_ERROR , &CommStatus );
	if ( *Error > 0 ) {
		sprintf( Buffer2 , "(Err:%d)" , *Error );
		ALARM_MESSAGE_GET( id , Buffer );
		strcat( Buffer , Buffer2 );
		ALARM_MESSAGE_SET( id , Buffer );
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status READ_DIGITAL_TRUE( int io , int *Data ) {
	char pszFtnName[256] = "";
	int i , AlmResult , CommStatus;
	
	while ( TRUE ) {
		for( i = 0 ; i < 3 ; i++ ) {
//			LOG(pszFtnName, ELT_File, "[LPM%d] READ DIGITAL TRUE [Retry = %d]", CASSETTE_INDEX, i);
			*Data = READ_DIGITAL( io , &CommStatus );
			if ( !CommStatus ) {
				if ( !WAIT_SECONDS( (double)WAIT_TIME2 ) ) return SYS_ABORTED;
			}
			else 
				break;
		}

		if ( !CommStatus ) {
			*Data = -1;
//			LOG(pszFtnName, ELT_File, "=====> 1 [LPM%d] READ DIGITAL TRUE [Retry = Fail After 3 Times Retry]", CASSETTE_INDEX);
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_PORT_COMM_ERROR );
			if ( AlmResult == ALM_IGNORE ) {
				return SYS_SUCCESS;
			}
			else if ( AlmResult != ALM_RETRY ) {
				return SYS_ABORTED;
			}
		}
		else {
			break;
		}
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status WRITE_DIGITAL_TRUE( int io , int Data ) {
	char pszFtnName[256] = "";
	int i , AlmResult , CommStatus;
	
	while ( TRUE ) {
//		for( i = 0 ; i < 10 ; i++ ) {
		for( i = 0 ; i < 3 ; i++ ) {
			WRITE_DIGITAL( io , Data , &CommStatus );
			if ( !CommStatus ) {
//				LOG(pszFtnName, ELT_File, "=====> 1 [LPM%d] WRITE DIGITAL TRUE [Retry = %d]", CASSETTE_INDEX, i);
				if ( !WAIT_SECONDS( (double)WAIT_TIME6 ) ) return SYS_ABORTED;
			}
			else {
				break;
			}
		}

		if ( !CommStatus )  {
//			LOG(pszFtnName, ELT_File, "=====> 2 [LPM%d] WRITE DIGITAL TRUE [Retry = Fail After 3 Times Retry]", CASSETTE_INDEX);
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_PORT_COMM_ERROR );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else {
			break;
		}
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void AGV_IO_CURR_SET( HWND hdlg , int iop , int on , int off , int *old ) {
	int CommStatus , i;
	i = READ_DIGITAL( iop , &CommStatus );
	if ( i == *old ) return;
	*old = i;
	if ( i ) {
		KWIN_Item_Disable( hdlg , on  );
		KWIN_Item_Enable( hdlg  , off );
	}
	else {
		KWIN_Item_Disable( hdlg , off );
		KWIN_Item_Enable( hdlg  , on  );
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void AGV_IO_DATA_SET( HWND hdlg , int iop , int io , int *old ) {
	int CommStatus , i;
	i = READ_DIGITAL( iop	, &CommStatus );
	if ( i == *old ) return;
	*old = i;
	if ( i ) KWIN_Item_String_Display( hdlg , io , "On"  );
	else     KWIN_Item_String_Display( hdlg , io , "Off" );
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Blink() {
	BOOL flag = FALSE;
	if( Current != NONE ) EnableWindow( hBtn , TRUE );
	while ( Current != NONE ) {
		if ( flag )	SetWindowText( hBtn , Caption[Current] );
		else SetWindowText( hBtn , "" );
		flag = ~flag;
		_sleep(500);
	}
	if( Current == NONE ) EnableWindow( hBtn , FALSE );
	SetWindowText( hBtn , Caption[Current] );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Indicator( int Mode ) {
	if( Mode != NONE ) EnableWindow( hBtn , TRUE );
	switch( Mode ) {
		case LOADING :
			SetWindowText( hBtn , "LOAD"	);
			break;
		case UNLOADING :
			SetWindowText( hBtn , "UNLOAD"	);
			break;
		case NONE :
			SetWindowText( hBtn , "Disable" );
			EnableWindow( hBtn , FALSE );
			break;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_EXCHANGE_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int Ibuf[9] , Obuf[9];
	static int uc , lc , cc;
	int CommStatus;
	char buf[11] = {0,};

	switch ( msg ) {
	case WM_INITDIALOG:
		//
		hBtn = GetDlgItem( hdlg , ID_10_ON );
		//
		uc = 0;
		lc = 0;
		cc = 0;
		sprintf( StrBuffer , "Handler for Port%d" , CASSETTE_INDEX );
		SetWindowText( hdlg , StrBuffer );
		for ( CommStatus = 0 ; CommStatus < 9 ; CommStatus++ ) {
			Ibuf[CommStatus] = -1;
			Obuf[CommStatus] = -1;
		}
		if ( !SIMULATION_LSWITCH_MODE ) KWIN_Item_Hide( hdlg , ID_10_ON );
		if ( !SIMULATION_USWITCH_MODE ) KWIN_Item_Hide( hdlg , ID_11_ON );
		if ( !SIMULATION_CASS_MODE ) {
			KWIN_Item_Hide( hdlg , ID_9_ON );
			KWIN_Item_Hide( hdlg , ID_9_OFF );
			KWIN_Item_Hide( hdlg , ID_12_ON );
		}
		if ( !SIMULATION_AGV_MODE ) {
			KWIN_Item_Hide( hdlg , ID_1 );
			KWIN_Item_Hide( hdlg , ID_2 );
			KWIN_Item_Hide( hdlg , ID_3 );
			KWIN_Item_Hide( hdlg , ID_4 );
			KWIN_Item_Hide( hdlg , ID_5 );
			KWIN_Item_Hide( hdlg , ID_1_ON  );
			KWIN_Item_Hide( hdlg , ID_1_OFF );
			KWIN_Item_Hide( hdlg , ID_2_ON  );
			KWIN_Item_Hide( hdlg , ID_2_OFF );
			KWIN_Item_Hide( hdlg , ID_4_ON  );
			KWIN_Item_Hide( hdlg , ID_4_OFF );
			KWIN_Item_Hide( hdlg , ID_5_ON  );
			KWIN_Item_Hide( hdlg , ID_5_OFF );
			KWIN_Item_Hide( hdlg , ID_6_ON  );
			KWIN_Item_Hide( hdlg , ID_6_OFF );
			KWIN_Item_Hide( hdlg , ID_7_ON  );
			KWIN_Item_Hide( hdlg , ID_7_OFF );
			KWIN_Item_Hide( hdlg , ID_8_ON  );
			KWIN_Item_Hide( hdlg , ID_8_OFF );
		}

		SetTimer( hdlg , 1000 , 100 , NULL );
		return TRUE;
			
	case WM_TIMER :
		if ( SIMULATION_LSWITCH_MODE ) {
			if ( lc > 0 ) {
				lc--;
				if ( lc == 0 ) {
					GetWindowText( GetDlgItem(hdlg,ID_10_ON) , buf , 10 );
					if( !STRCMP_L( buf , "Disable" ) ) {
						KWIN_Item_Enable( hdlg , ID_10_ON );
					}
					if ( READ_DIGITAL( LP_LOAD_BUTTON , &CommStatus ) )
						WRITE_DIGITAL( LP_LOAD_BUTTON , OFF , &CommStatus );
					if ( READ_DIGITAL( LP_UNLOAD_BUTTON , &CommStatus ) )
						WRITE_DIGITAL( LP_UNLOAD_BUTTON , OFF , &CommStatus );
				}
			}
		}
//		if ( SIMULATION_USWITCH_MODE ) {
//			if ( uc > 0 ) {
//				uc--;
//				if ( uc == 0 ) {
//					KWIN_Item_Enable( hdlg , ID_11_ON );
//					WRITE_DIGITAL( LP_UNLOAD_BUTTON , OFF , &CommStatus );
//				}
//			}
//		}
		if ( SIMULATION_CASS_MODE ) {
			if ( cc > 0 ) {
				cc--;
				if ( cc == 0 ) {
					KWIN_Item_Enable( hdlg , ID_12_ON );
				}
			}
			AGV_IO_CURR_SET( hdlg , LP_FOUP_STS , ID_9_ON , ID_9_OFF , &Ibuf[8] );
		}

		if ( SIMULATION_AGV_MODE ) {
			AGV_IO_DATA_SET( hdlg , LP_AGV_OUTPUT_HOAVBL	, ID_1 , &Obuf[0] );
			AGV_IO_DATA_SET( hdlg , LP_AGV_OUTPUT_ESTOP		, ID_2 , &Obuf[1] );
			AGV_IO_DATA_SET( hdlg , LP_AGV_OUTPUT_LREQ		, ID_3 , &Obuf[2] );
			AGV_IO_DATA_SET( hdlg , LP_AGV_OUTPUT_UREQ		, ID_4 , &Obuf[3] );
			AGV_IO_DATA_SET( hdlg , LP_AGV_OUTPUT_READY		, ID_5 , &Obuf[4] );
			//
			AGV_IO_CURR_SET( hdlg , LP_AGV_INPUT_CS			, ID_1_ON , ID_1_OFF , &Ibuf[0] );
			AGV_IO_CURR_SET( hdlg , LP_AGV_INPUT_CS1		, ID_2_ON , ID_2_OFF , &Ibuf[1] );
			AGV_IO_CURR_SET( hdlg , LP_AGV_INPUT_VALID		, ID_4_ON , ID_4_OFF , &Ibuf[3] );
			AGV_IO_CURR_SET( hdlg , LP_AGV_INPUT_TRREQ		, ID_5_ON , ID_5_OFF , &Ibuf[4] );
			AGV_IO_CURR_SET( hdlg , LP_AGV_INPUT_BUSY		, ID_6_ON , ID_6_OFF , &Ibuf[5] );
			AGV_IO_CURR_SET( hdlg , LP_AGV_INPUT_COMPLETE	, ID_7_ON , ID_7_OFF , &Ibuf[6] );
			AGV_IO_CURR_SET( hdlg , LP_AGV_INPUT_CONT		, ID_8_ON , ID_8_OFF , &Ibuf[7] );
		}
		return TRUE;

	case WM_COMMAND :
		switch (wParam) {
		case ID_1_ON :	WRITE_DIGITAL( LP_AGV_INPUT_CS		, ON  , &CommStatus );	return TRUE;
		case ID_1_OFF :	WRITE_DIGITAL( LP_AGV_INPUT_CS		, OFF , &CommStatus );	return TRUE;
		case ID_2_ON :	WRITE_DIGITAL( LP_AGV_INPUT_CS1		, ON  , &CommStatus );	return TRUE;
		case ID_2_OFF :	WRITE_DIGITAL( LP_AGV_INPUT_CS1		, OFF , &CommStatus );	return TRUE;
		case ID_4_ON :	WRITE_DIGITAL( LP_AGV_INPUT_VALID	, ON  , &CommStatus );	return TRUE;
		case ID_4_OFF :	WRITE_DIGITAL( LP_AGV_INPUT_VALID	, OFF , &CommStatus );	return TRUE;
		case ID_5_ON :	WRITE_DIGITAL( LP_AGV_INPUT_TRREQ	, ON  , &CommStatus );	return TRUE;
		case ID_5_OFF :	WRITE_DIGITAL( LP_AGV_INPUT_TRREQ	, OFF , &CommStatus );	return TRUE;
		case ID_6_ON :	WRITE_DIGITAL( LP_AGV_INPUT_BUSY	, ON  , &CommStatus );	return TRUE;
		case ID_6_OFF :	WRITE_DIGITAL( LP_AGV_INPUT_BUSY	, OFF , &CommStatus );	return TRUE;
		case ID_7_ON :	WRITE_DIGITAL( LP_AGV_INPUT_COMPLETE, ON  , &CommStatus );	return TRUE;
		case ID_7_OFF :	WRITE_DIGITAL( LP_AGV_INPUT_COMPLETE, OFF , &CommStatus );	return TRUE;
		case ID_8_ON :	WRITE_DIGITAL( LP_AGV_INPUT_CONT	, ON  , &CommStatus );	return TRUE;
		case ID_8_OFF :	WRITE_DIGITAL( LP_AGV_INPUT_CONT	, OFF , &CommStatus );	return TRUE;
		case ID_9_ON :	WRITE_DIGITAL( LP_FOUP_STS			, ON  , &CommStatus );	return TRUE;
		case ID_9_OFF :	WRITE_DIGITAL( LP_FOUP_STS			, OFF , &CommStatus );	return TRUE;
		case ID_10_ON :
			KWIN_Item_Disable( hdlg , ID_10_ON );
			lc = 5;
			if		( READ_DIGITAL( LP_LOADING_SIGNAL , &CommStatus ) )
				WRITE_DIGITAL( LP_LOAD_BUTTON , ON , &CommStatus );
			else if ( READ_DIGITAL( LP_UNLOADING_SIGNAL , &CommStatus ) )
				WRITE_DIGITAL( LP_UNLOAD_BUTTON , ON , &CommStatus );
			return TRUE;
//		case ID_11_ON :
//			KWIN_Item_Disable( hdlg , ID_11_ON );
//			WRITE_DIGITAL( LP_UNLOAD_BUTTON , ON , &CommStatus );
//			uc = 5;
//			return TRUE;
		case ID_12_ON :
			KWIN_Item_Disable( hdlg , ID_12_ON );
			printf( "===###===> CASSETTE CM%d|SET" , CASSETTE_INDEX );
			sprintf( StrBuffer , "CASSETTE CM%d|SET" , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
			cc = 5;
			return TRUE;
		}
		break;

	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void EXCHANGE_SIM_RUN() {
	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_DIALOG ) , NULL , Console_Gui_For_EXCHANGE_Proc , (LPARAM) NULL );
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void AGV_INIT_SET_IO_FOR_SIM() {
	int CommStatus;
	WRITE_DIGITAL( LP_AGV_INPUT_CS			, OFF , &CommStatus );
	WRITE_DIGITAL( LP_AGV_INPUT_CS1			, OFF , &CommStatus );
	WRITE_DIGITAL( LP_AGV_INPUT_VALID		, OFF , &CommStatus );
	WRITE_DIGITAL( LP_AGV_INPUT_TRREQ		, OFF , &CommStatus );
	WRITE_DIGITAL( LP_AGV_INPUT_BUSY		, OFF , &CommStatus );
	WRITE_DIGITAL( LP_AGV_INPUT_COMPLETE	, OFF , &CommStatus );
	WRITE_DIGITAL( LP_AGV_INPUT_CONT		, OFF , &CommStatus );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status AGV_LOAD_CONTROL() {
	int AlmResult , CommStatus , Status;
	char pszFtnName[256] = ""; // jelee added 2005.11.11
	double Time;

	//EFEM Alarm Check Function Start by jelee
	if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT)) {
		WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
	}
	//EFEM Alarm Check Function End

	// if Only IGNORE Recovery is used , that is selete scenario
	//---------------------------------------------------
	// Checking Receive Carrier with PIO
	//---------------------------------------------------
	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);

		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_LOAD_AGV_WAIT_VALID );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}

		if ( READ_DIGITAL( LP_AGV_INPUT_TRREQ , &CommStatus ) == ON ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load LP_AGV_INPUT_TRREQ IS ON But, Valid Signal Off Sequence Error", CASSETTE_INDEX);
			//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_SENARIO_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
				//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
					//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
		}
	
		Status = FOUP_SENSOR_ERROR( TRUE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>1", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>2", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}

		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
//			LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>1", CASSETTE_INDEX);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>2", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>2-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>2-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}
		
		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_VALID , &CommStatus ) ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP1TIMEOUT , &CommStatus );
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALID_TIMEOUT_TP6 );
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
						return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( LOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ERROR; // No Foup
				else { // SYS_SUCCESS, Foup Present
					//-------------------------------------------------------------------------------
					sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
					//-------------------------------------------------------------------------------
					return SYS_SUCCESS;
				}
			}
			else return SYS_ABORTED;
*/		}
	}
//	if((READ_DIGITAL(LP1_ALARM_STS , &CommStatus) != PRESENT) && (READ_DIGITAL(LP2_ALARM_STS , &CommStatus) != PRESENT) && (READ_DIGITAL(LP3_ALARM_STS , &CommStatus) != PRESENT))
//	{
	WRITE_DIGITAL( LP_AGV_OUTPUT_LREQ , ON , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Load LREQ SIGNAL IS ON", CASSETTE_INDEX);
//	}

	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);

		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_LOAD_AGV_WAIT_TRREQ );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS OFF After LREQ ON", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>2", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>2-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>2-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}
		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>3", CASSETTE_INDEX);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>3-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>3-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>4", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>4-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>4-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		Status = FOUP_SENSOR_ERROR( TRUE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>3", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>4", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}
		
		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_TRREQ , &CommStatus ) ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Load TRREQ SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP1TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT;
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_TRREQ_TIMEOUT_TP1 );
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
						return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
	}

	// Befere TB, return SYS_ERROR, After return SYS_ABORTED
	//-------------------------------------------------------------------------------
	sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
	//-------------------------------------------------------------------------------
	WRITE_DIGITAL( LP_AGV_OUTPUT_READY , ON , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Load READY SIGNAL IS ON", CASSETTE_INDEX);

	S_TIMER_READY();
	while( TRUE ) {
		
		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_LOAD_AGV_WAIT_BUSY );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS OFF After READY ON", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>3-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>3-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>4", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>4-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>4-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>5", CASSETTE_INDEX);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>5-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>5-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>6", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>6-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>6-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		Status = FOUP_SENSOR_ERROR( TRUE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>5", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>6", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}

		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_BUSY , &CommStatus ) ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Load BUSY SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP2TIMEOUT , &CommStatus );
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_BUSY_TIMEOUT_TP2 );
						if ( AlmResult == ALM_IGNORE ) { // Complete Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
						return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
		_sleep(100);
	}

	S_TIMER_READY();
	while ( TRUE ) {
		
		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_LOAD_AGV_WAIT_CARRIER );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS OFF After BUSY ON", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>5-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>5-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>6", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>6-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>6-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}			
		}

		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>7", CASSETTE_INDEX);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>7-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>7-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>8", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>8-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>8-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		if ( FOUP_SENSOR_CHECK( &CommStatus ) == PRESENT2 ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load FOUP PRESENT CONPLETED", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP3TIMEOUT , &CommStatus );
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUPRECV_TIMEOUT_TP3 );
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
						return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
		_sleep(100);
	}
	WRITE_DIGITAL( LP_AGV_OUTPUT_LREQ , OFF , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Load LREQ SIGNAL IS OFF", CASSETTE_INDEX);

	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);
		
		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_LOAD_AGV_WAIT_COMPLETE );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS OFF After LREQ OFF", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>7-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>7-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>8", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>8-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>8-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

// jelee added start 2005.10.20 - LREQ Signal check
		if ( READ_DIGITAL( LP_AGV_OUTPUT_LREQ , &CommStatus ) != OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load LREQ SIGNAL IS STILL ON", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LREQON_ERROR );
			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( LOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // No Foup
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
			else return SYS_ABORTED;
		}
// jelee added end 2005.10.20 - LREQ Signal check

		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>9", CASSETTE_INDEX);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>9-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>9-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>10", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>10-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>10-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		Status = FOUP_SENSOR_ERROR( FALSE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>7", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>8", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}
	
		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Load COMLETE SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP4TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT / 2.;		
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_COMPLETE_TIMEOUT_TP4 );
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
						return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
	}
	WRITE_DIGITAL( LP_AGV_OUTPUT_READY , OFF , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Load READY SIGNAL IS OFF", CASSETTE_INDEX);
	_sleep(250);
	
	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);
		
		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_LOAD_AGV_WAIT_CSOFF );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
/*		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS OFF After LREQ OFF", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( LOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // No Foup
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
			else return SYS_ABORTED;
		}
*/		
/*		if ( READ_DIGITAL( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) {
			if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
				LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS OFF After COMPLETE ON", CASSETTE_INDEX);
//				AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
				if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>9-1", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>9-2", CASSETTE_INDEX);
						return SYS_ERROR;
					}
					else {
						return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
					}
				}
				else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>10", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>10-1", CASSETTE_INDEX);
						return SYS_ERROR; // No Foup
					}
					else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>10-2", CASSETTE_INDEX);
						return SYS_SUCCESS;
					}
				}
			}
		}
*/
		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>11", CASSETTE_INDEX);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>11-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>11-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>12", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>12-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When Load Sequence------>12-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		Status = FOUP_SENSOR_ERROR( FALSE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>9", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>10", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}

		if ( READ_DIGITAL( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) {
			if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
				WAIT_SECONDS(1);
				if ( READ_DIGITAL( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) {
					if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
						LOG(pszFtnName, ELT_File, "[LPM%d] Load VALID SIGNAL IS OFF After COMPLETE ON", CASSETTE_INDEX);
//						AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
						WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
						if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//							LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>9-1", CASSETTE_INDEX);
							Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
							if ( Status != SYS_SUCCESS ) { // Retry Option
//								LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>9-2", CASSETTE_INDEX);
								return SYS_ERROR;
							}
							else {
								return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
							}
						}
						else if ( Status == SYS_SUCCESS ) { // Complete Option
//							LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>10", CASSETTE_INDEX);
							Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
							if ( Status != SYS_SUCCESS ) { // Retry Option
//								LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>10-1", CASSETTE_INDEX);
								return SYS_ERROR; // No Foup
							}
							else { // SYS_SUCCESS, Foup Present, Complete Option
//								LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>10-2", CASSETTE_INDEX);
								return SYS_SUCCESS;
							}
						}
					}
				}
			}
		}

		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_CS , &CommStatus ) == 0 ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Load CS SIGNAL IS OFF", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP5TIMEOUT , &CommStatus );
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_COMPLOFF_TIMEOUT_TP5 );
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
					return SYS_ERROR;
//					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT );
//					Status = AGV_RECOVERY_SCENARIO( LOAD );
//					if ( Status != SYS_SUCCESS ) {
//						return SYS_ERROR; // No Foup
//					}
//					else { // SYS_SUCCESS, Foup Present
//						//-------------------------------------------------------------------------------
//						sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
//						WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
//						//-------------------------------------------------------------------------------
//						return SYS_SUCCESS;
//					}
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
				Status = AGV_RECOVERY_SCENARIO3( LOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
//					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT2 );
//					Status = AGV_RECOVERY_SCENARIO2( LOAD, RETRY1 );
//					if ( Status != SYS_SUCCESS ){
						return SYS_ERROR; // No Foup
//					}
//					else { // SYS_SUCCESS, Foup Present
//						//-------------------------------------------------------------------------------
//						sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
//						WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
//						//-------------------------------------------------------------------------------
//						return SYS_SUCCESS;
//					}
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
	}
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status AGV_UNLOAD_CONTROL( BOOL unclamp) {
	int AlmResult , CommStatus , Status;
	char pszFtnName[256] = ""; // jelee added 2005.11.11
	double Time;

	//EFEM Alarm Check Function Start by jelee
	if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
	{
		WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
	}
	//EFEM Alarm Check Function End
	
	// if Only IGNORE Recovery is used , that is selete scenario
	//---------------------------------------------------
	// Checking Send Carrier with PIO
	//---------------------------------------------------
	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);

		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_UNLOAD_AGV_WAIT_VALID );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}

		if ( READ_DIGITAL( LP_AGV_INPUT_TRREQ , &CommStatus ) == ON ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] UNLoad LP_AGV_INPUT_TRREQ IS ON But, Valid Signal Off Sequence Error", CASSETTE_INDEX);
			//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_SENARIO_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
				//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
					//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When Load Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
		}

		
		Status = FOUP_SENSOR_ERROR( FALSE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>1", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>2", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}
		
/*		if		( Status == SYS_ABORTED )
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>1", CASSETTE_INDEX);
			return SYS_ABORTED;
		}
		else if ( Status == SYS_ERROR   ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>2", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO( UNLOAD );
			if ( Status != SYS_SUCCESS ) return SYS_ERROR; // Foup Present
			else { // SYS_SUCCESS, Foup Absent
				//-------------------------------------------------------------------------------
				sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
				//-------------------------------------------------------------------------------
				return SYS_SUCCESS;
			}
		}
*/		
		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
 			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>2", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>2-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>2-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
			}
			else return SYS_ABORTED;
*/		}

		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_VALID , &CommStatus ) )
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP1TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT;
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALID_TIMEOUT_TP6 );
			LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALID_TIMEOUT_TP6 Error When UnLoad Sequence------>1", CASSETTE_INDEX);
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALID_TIMEOUT_TP6 Error When UnLoad Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALID_TIMEOUT_TP6 Error When UnLoad Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALID_TIMEOUT_TP6 Error When UnLoad Sequence------>2-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALID_TIMEOUT_TP6 Error When UnLoad Sequence------>2-2", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ERROR; // Foup Present
				else { // SYS_SUCCESS, Foup Absent
					//-------------------------------------------------------------------------------
					sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
					//-------------------------------------------------------------------------------
					return SYS_SUCCESS;
				}
			}
			else return SYS_ABORTED;
		}
*/	}
//	if((READ_DIGITAL(LP1_ALARM_STS , &CommStatus) != PRESENT) && (READ_DIGITAL(LP2_ALARM_STS , &CommStatus) != PRESENT) && (READ_DIGITAL(LP3_ALARM_STS , &CommStatus) != PRESENT))
//	{
	WRITE_DIGITAL( LP_AGV_OUTPUT_UREQ , ON , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Unload UREQ SIGNAL IS ON", CASSETTE_INDEX);
//	}

	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);
		
		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_UNLOAD_AGV_WAIT_TRREQ );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After UREQ ON", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>2", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>2-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>2-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ERROR; // Foup Present
				else { // SYS_SUCCESS, Foup Absent
					//-------------------------------------------------------------------------------
					sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
					//-------------------------------------------------------------------------------
					return SYS_SUCCESS;
				}
			}
			else return SYS_ABORTED;
*/		}
		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
 			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>3-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>3-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>4", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>4-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>4-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
			}
			else return SYS_ABORTED;
*/		}

		Status = FOUP_SENSOR_ERROR( FALSE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>3", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>4", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}

		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_TRREQ , &CommStatus ) ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload TRREQ SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP1TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT;
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_TRREQ_TIMEOUT_TP1 );
			LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1", CASSETTE_INDEX);
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-2", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
	}

	// Befere TB, return SYS_ERROR, After return SYS_ABORTED
	//-------------------------------------------------------------------------------
	sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
	//-------------------------------------------------------------------------------

	if ( unclamp == 0 ) {
		//---------------------------------------------------
		// Make UnClamp
		//---------------------------------------------------
		WRITE_DIGITAL( LP_CLAMP   , UNCLAMP , &CommStatus );
//		WRITE_DIGITAL( LP_CLAMP_STS   , UNCLAMP , &CommStatus );
//		WRITE_DIGITAL( UNCLAMP_SET , ON  , &CommStatus );

		if ( SIMULATION_AGV_MODE ) {
			if ( READ_DIGITAL( LP_CLAMP_STS , &CommStatus ) != UNCLAMP ) {
				if ( !WAIT_SECONDS( PORT_TIME ) ) return SYS_ABORTED;
				WRITE_DIGITAL( LP_CLAMP_STS , UNCLAMP , &CommStatus );
			}
		}

		if ( READ_DIGITAL( LP_CLAMP_STS , &CommStatus ) != UNCLAMP ) {
			if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
			WRITE_DIGITAL( LP_CLAMP , UNCLAMP , &CommStatus );
		}

		//---------------------------------------------------
		// Send Unclamped Message
		//---------------------------------------------------
		S_TIMER_READY();
		while ( TRUE ) {
			_sleep(250);

			//EFEM Alarm Check Function Start by jelee
			if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
			{
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			}
			//EFEM Alarm Check Function End

			//======================================================================
			if ( CONTROL_REFRESH ) return SYS_ABORTED;
			//======================================================================
			MONITOR( MON_UNLOAD_AGV_WAIT_UNCLAMPED );
			
			// MDL_STATUS check of under part may be skipped in unload sequence auto
			switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
			case 1 : case 4 : break;
			default:
				AGV_INIT_SET_IO( -2 );
				return SYS_ABORTED;
				break;
			}
			
			if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
				LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After TRREQ ON", CASSETTE_INDEX);
//				AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
				AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
				if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>3-1", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>3-2", CASSETTE_INDEX);
						return SYS_ERROR;
					}
					else {
						return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
					}
				}
				else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>4", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>4-1", CASSETTE_INDEX);
						return SYS_ERROR; // No Foup
					}
					else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>4-2", CASSETTE_INDEX);
						return SYS_SUCCESS;
					}
				}
			}

			if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//				AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
				WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
				AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
				if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>5-1", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>5-2", CASSETTE_INDEX);
						return SYS_ERROR;
					}
					else {
						return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
					}
				}
				else if ( Status == SYS_SUCCESS ) { // Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>6", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>6-1", CASSETTE_INDEX);
						return SYS_ERROR; // No Foup
					}
					else { // SYS_SUCCESS, Foup Present, Complete Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>6-2", CASSETTE_INDEX);
						return SYS_SUCCESS;
					}
				}
/*				if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
				}
				else return SYS_ABORTED;
*/			}
				
			Status = FOUP_SENSOR_ERROR( FALSE , TRUE );
			if ( Status == SYS_ABORTED ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>5", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else if ( Status == SYS_ERROR ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>6", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
					return SYS_SUCCESS;
				}
			}
			
			if ( READ_DIGITAL( LP_CLAMP_STS , &CommStatus ) == sUNCLAMP ) {
				// Send Unclamp Complete
				sprintf( StrBuffer , "%s%d" , EVENT_STRING_UNCLAMP_COMPLETE , CASSETTE_INDEX );
				LOG(pszFtnName, ELT_File, "[LPM%d] Unclamped", CASSETTE_INDEX);
				WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
				break;
			}
			else {
				if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
				WRITE_DIGITAL( LP_CLAMP , UNCLAMP , &CommStatus );
			}

			if ( S_TIMER_CHECK( 1 , DEFAULT_TIME_OUT ) ) {
//				AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
				AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_PORT_UNCLAMP_TIMEOUT );
				if      ( AlmResult == ALM_IGNORE ) { 
					Status = AGV_RECOVERY_SCENARIO( UNLOAD );
					if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
					else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
//					WRITE_DIGITAL( AMHS_OUTPUT_UREQ   , ON , &CommStatus );
//					WRITE_DIGITAL( AMHS_OUTPUT_HOAVBL , ON , &CommStatus );
//					break; 
				}
//				else if ( AlmResult == ALM_RETRY  ) {
//					WRITE_DIGITAL( AMHS_OUTPUT_UREQ   , ON , &CommStatus );
//					WRITE_DIGITAL( AMHS_OUTPUT_HOAVBL , ON , &CommStatus );

//					WRITE_DIGITAL( CLAMP_SET   , OFF , &CommStatus );
//					WRITE_DIGITAL( UNCLAMP_SET , ON  , &CommStatus );

//					S_TIMER_READY();
//				}
				else return SYS_ABORTED;
			}
		}
	}
	WRITE_DIGITAL( LP_AGV_OUTPUT_READY , ON , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Unload READY SIGNAL IS ON", CASSETTE_INDEX);

	S_TIMER_READY();
	while( TRUE ) {

		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_UNLOAD_AGV_WAIT_BUSY );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_CLAMP_STS , &CommStatus ) != UNCLAMP ) {
			if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
			WRITE_DIGITAL( LP_CLAMP , UNCLAMP , &CommStatus );
		}

		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After READY ON", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>5-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>5-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>6", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>6-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>6-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
 			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>7-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>7-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>8", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>8-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>8-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
			}
			else return SYS_ABORTED;
*/		}

		Status = FOUP_SENSOR_ERROR( FALSE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>7", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>8", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}
		
		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_BUSY , &CommStatus ) ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload BUSY SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP2TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT;
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_BUSY_TIMEOUT_TP2 );
			LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1", CASSETTE_INDEX);
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-2", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
		_sleep(100);
	}

	S_TIMER_READY();
	while ( TRUE ) {

		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_UNLOAD_AGV_WAIT_CARRIER );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After BUSY ON", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>7-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>7-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>8", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>8-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>8-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
 			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>9-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>9-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>10", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>10-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>10-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
			}
			else return SYS_ABORTED;
*/		}

		if ( FOUP_SENSOR_CHECK( &CommStatus ) == ABSENT2 ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload FOUP IS ABSENT", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP3TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT;
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUPREMOVE_TIMEOUT_TP3 );
			LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1", CASSETTE_INDEX);
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-2", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
		_sleep(100);
	}
	WRITE_DIGITAL( LP_AGV_OUTPUT_UREQ , OFF , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Unload UREQ SIGNAL IS OFF", CASSETTE_INDEX);

	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);

		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_UNLOAD_AGV_WAIT_COMPLETE );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After UREQ OFF", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>9-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>9-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>10", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>10-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>10-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
		}

		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
 			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>11-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>11-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>12", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>12-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>12-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
			}
			else return SYS_ABORTED;
*/		}

		Status = FOUP_SENSOR_ERROR( TRUE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>9", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>10", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}
		
		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload COMPLETE SIGNAL IS ON", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP4TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT;
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_COMPLETE_TIMEOUT_TP4 );
			LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1", CASSETTE_INDEX);
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>1-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_TRREQ_TIMEOUT_TP1 Error When UnLoad Sequence------>2-2", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
		}
	}
	WRITE_DIGITAL( LP_AGV_OUTPUT_READY , OFF , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Unload READY SIGNAL IS OFF", CASSETTE_INDEX);
	
	S_TIMER_READY();
	while( TRUE ) {
		_sleep(250);

		//EFEM Alarm Check Function Start by jelee
		if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
		{
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
		}
		//EFEM Alarm Check Function End

		//======================================================================
		if ( CONTROL_REFRESH ) return SYS_ABORTED;
		//======================================================================
		MONITOR( MON_UNLOAD_AGV_WAIT_CSOFF );
		
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			AGV_INIT_SET_IO( -2 );
			return SYS_ABORTED;
			break;
		}
		
/*		if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After UREQ OFF", CASSETTE_INDEX);
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR);
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>11-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>11-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>12", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>12-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>12-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}

		}
*/		
/*		if ( READ_DIGITAL( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) {
			if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
				LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After COMPLETE ON", CASSETTE_INDEX);
//				AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
				AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
				if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>13-1", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>13-2", CASSETTE_INDEX);
						return SYS_ERROR;
					}
					else {
						return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
					}
				}
				else if ( Status == SYS_SUCCESS ) { // Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>14", CASSETTE_INDEX);
					Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
					if ( Status != SYS_SUCCESS ) { // Retry Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>14-1", CASSETTE_INDEX);
						return SYS_ERROR; // No Foup
					}
					else { // SYS_SUCCESS, Foup Present, Complete Option
//						LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>14-2", CASSETTE_INDEX);
						return SYS_SUCCESS;
					}
				}				
			}
		}
*/
		if ( READ_DIGITAL( Light_Curtain , &CommStatus ) != ON ) {
//			AGV_INIT_SET_IO( -3 ); // recovery is only ignore and abbort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			WRITE_DIGITAL( LP_AGV_OUTPUT_ESTOP , OFF , &CommStatus ); // not avaliable
 			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_LIGHT_CURTAIN_OFF );
			if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>13-1", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>13-2", CASSETTE_INDEX);
					return SYS_ERROR;
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
				}
			}
			else if ( Status == SYS_SUCCESS ) { // Complete Option
//				LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>14", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) { // Retry Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>14-1", CASSETTE_INDEX);
					return SYS_ERROR; // No Foup
				}
				else { // SYS_SUCCESS, Foup Present, Complete Option
//					LOG(pszFtnName, ELT_File, "[LPM%d] Light_Curtain ERROR When UNLOAD Sequence------>14-2", CASSETTE_INDEX);
					return SYS_SUCCESS;
				}
			}
/*			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) return SYS_ABORTED; // Foup Present
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Absent
			}
			else return SYS_ABORTED;
*/		}

		Status = FOUP_SENSOR_ERROR( TRUE , TRUE );
		if ( Status == SYS_ABORTED ) { // Retry Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>11", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR;
			}
			else {
				return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
		}
		else if ( Status == SYS_ERROR ) { // Complete Option
//			LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>12", CASSETTE_INDEX);
			Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
			if ( Status != SYS_SUCCESS ) { // Retry Option
				return SYS_ERROR; // No Foup
			}
			else { // SYS_SUCCESS, Foup Present, Complete Option
				return SYS_SUCCESS;
			}
		}

		if ( READ_DIGITAL( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) {
			if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
				WAIT_SECONDS(1);
				if ( READ_DIGITAL( LP_AGV_INPUT_COMPLETE , &CommStatus ) ) {
					if ( READ_DIGITAL( LP_AGV_INPUT_VALID , &CommStatus ) == OFF ) {
						LOG(pszFtnName, ELT_File, "[LPM%d] Unload VALID SIGNAL IS OFF After COMPLETE ON", CASSETTE_INDEX);
//						AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abbort
						WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
						AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_VALIDOFF_ERROR );
						if ( AlmResult != SYS_SUCCESS ) { // Retry Option
//							LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>13-1", CASSETTE_INDEX);
							Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
							if ( Status != SYS_SUCCESS ) { // Retry Option
//								LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>13-2", CASSETTE_INDEX);
								return SYS_ERROR;
							}
							else {
								return SYS_SUCCESS; // SYS_SUCCESS, Foup Present, Complete Option
							}
						}
						else if ( Status == SYS_SUCCESS ) { // Complete Option
//							LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>14", CASSETTE_INDEX);
							Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
							if ( Status != SYS_SUCCESS ) { // Retry Option
//								LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>14-1", CASSETTE_INDEX);
								return SYS_ERROR; // No Foup
							}
							else { // SYS_SUCCESS, Foup Present, Complete Option
//								LOG(pszFtnName, ELT_File, "[LPM%d] ALARM_VALIDOFF_ERROR When UNLoad Sequence------>14-2", CASSETTE_INDEX);
								return SYS_SUCCESS;
							}
						}				
					}
				}
			}
		}

		if ( READ_DIGITAL_TRUE2( LP_AGV_INPUT_CS , &CommStatus ) == 0 ) 
		{
			LOG(pszFtnName, ELT_File, "[LPM%d] Unload CS SIGNAL IS OFF", CASSETTE_INDEX);
			break;
		}
		Time = (double)READ_DIGITAL( AMHS_TP5TIMEOUT , &CommStatus );
//		if ( Time <= 0 ) Time = DEFAULT_TIME_OUT;
		if ( Time == 999 ) Time = DEFAULT_TIME_OUT1;
		
		if ( S_TIMER_CHECK( 1 , Time ) ) {
//			AGV_INIT_SET_IO( -2 ); // recovery is only ignore and abort
			WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_COMPLOFF_TIMEOUT_TP5 );
			if ( AlmResult == ALM_IGNORE ) { // Complete Option
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, COMPLETE1 );
				if ( Status != SYS_SUCCESS ) {
					return SYS_ERROR;
//					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT );
//					Status = AGV_RECOVERY_SCENARIO( LOAD );
//					if ( Status != SYS_SUCCESS ) {
//						return SYS_ERROR; // No Foup
//					}
//					else { // SYS_SUCCESS, Foup Present
//						//-------------------------------------------------------------------------------
//						sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
//						WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
//						//-------------------------------------------------------------------------------
//						return SYS_SUCCESS;
//					}
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
			else { // Retry Option
				Status = AGV_RECOVERY_SCENARIO3( UNLOAD, RETRY1 );
				if ( Status != SYS_SUCCESS ){ // Retry Option
//					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT2 );
//					Status = AGV_RECOVERY_SCENARIO2( LOAD, RETRY1 );
//					if ( Status != SYS_SUCCESS ){
						return SYS_ERROR; // No Foup
//					}
//					else { // SYS_SUCCESS, Foup Present
//						//-------------------------------------------------------------------------------
//						sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_LOAD_RECV2 , CASSETTE_INDEX );
//						WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
//						//-------------------------------------------------------------------------------
//						return SYS_SUCCESS;
//					}
				}
				else {
					return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
				}
			}
/*			AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_COMPLOFF_TIMEOUT_TP5 );
			if ( AlmResult == ALM_IGNORE ) {
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ) {
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_ABSENT );
					Status = AGV_RECOVERY_SCENARIO( UNLOAD );
					if ( Status != SYS_SUCCESS ) {
						return SYS_ERROR; // No Foup
					}
					else { // SYS_SUCCESS, Foup Present
						//-------------------------------------------------------------------------------
						sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
						WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
						//-------------------------------------------------------------------------------
						return SYS_SUCCESS;
					}
				}
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
			else
			{
				Status = AGV_RECOVERY_SCENARIO( UNLOAD );
				if ( Status != SYS_SUCCESS ){
					AlmResult = ALARM_MANAGE_for_CarrHandling( ALARM_START_POINTER + ALARM_FOUP_PRESENT2 );
					Status = AGV_RECOVERY_SCENARIO( UNLOAD );
					if ( Status != SYS_SUCCESS ){
						return SYS_ERROR; // No Foup
					}
					else { // SYS_SUCCESS, Foup Present
						//-------------------------------------------------------------------------------
						sprintf( StrBuffer , "%s%d" , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
						WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
						//-------------------------------------------------------------------------------
						return SYS_SUCCESS;
					}
				}
				else return SYS_SUCCESS; // SYS_SUCCESS, Foup Present
			}
*/		}
	}

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Load_Control() {
//	Module_Status Status;
	int CommStatus , AlmResult , OLD_AUTO_MODE , Data , Data2 , Error;
	int nSkip;
	char pszFtnName[256] = ""; // jelee added 2005.11.11
	BOOL Check = FALSE;
	BOOL bRunStatus = TRUE; // jelee add 2007.12.06

	printf( "=====> Start CM%d Load Handling : %s\n" , CASSETTE_INDEX , PROGRAM_PARAMETER_READ() );

	sprintf( StrBuffer , EVENT_LOAD_REQUEST , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );

	if ( READ_DIGITAL_TRUE( LP_MODE_AGV_STS , &Data ) == SYS_ABORTED )
	{
		return SYS_ABORTED;
	}

	OLD_AUTO_MODE = Data;

	if ( Data == MANUAL ) 
	{
		Indicator( LOADING );
	}
	else 
	{
		Indicator( NONE );
	}
	
//	printf( "=====> CM%d Load Handling : 1\n" , CASSETTE_INDEX);//jelee

	WRITE_DIGITAL( LP_OPERATOR_CANCEL , OFF , &CommStatus );
	WRITE_DIGITAL( LP_PROCESS_END	  , OFF , &CommStatus );
	WRITE_DIGITAL_DOUBLE( LP_UNLOADING_SIGNAL , LP_LOADING_SIGNAL , LOADING , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Loading SIGNAL IS ON", CASSETTE_INDEX);
	while ( TRUE ) { // 1st while() : Carrier Load
		_sleep(250);
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			Indicator( NONE );
			return SYS_ABORTED;
		}

//		printf( "=====> CM%d Load Handling : 2\n" , CASSETTE_INDEX);//jelee
		if ( READ_DIGITAL_TRUE( LP_MODE_AGV_STS , &Data ) == SYS_ABORTED )
		{
			return SYS_ABORTED;
		}
		if ( OLD_AUTO_MODE != Data )
		{
			OLD_AUTO_MODE = Data;
			if ( Data == AMHS ) AGV_INIT_SET_IO( -1 );
			else AGV_INIT_SET_IO( -2 );
		}
//		printf( "=====> CM%d Load Handling : 3\n" , CASSETTE_INDEX);
		if ( Data == AMHS ) {
			AGV_INIT_SET_IO( -1 );
//			LOG(pszFtnName, ELT_File, "[LPM%d] Load Sequence is AMHS", CASSETTE_INDEX);

			//EFEM Alarm Check Function Start by jelee
			if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
			{
//				LOG(pszFtnName, ELT_File, "[LPM%d] Load Sequence ALARM Detected", CASSETTE_INDEX);
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			}
			//EFEM Alarm Check Function End

//			printf( "=====> CM%d Load Handling : 3-1\n" , CASSETTE_INDEX);//jelee
			Indicator( NONE );
//			Status = FOUP_SENSOR_CHECK2( TRUE , FALSE );

// 2008.05.10 jelee Delete Sequence Release start
/*			Status = FOUP_SENSOR_ERROR2( TRUE , FALSE );
			if( Status == SYS_ABORTED )  // Retry Option
			{
//				LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>100", CASSETTE_INDEX);
//				LOG(pszFtnName, ELT_File, "[LPM%d] LOADING FOUP_SENSOR_ERROR is SYS_ABORTED", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO2( LOAD, RETRY1 ); // jelee Added.
				if( Status != SYS_SUCCESS) // Retry Option
				{
//					LOG(pszFtnName, ELT_File, "[LPM%d] LOADING AGV_RECOVERY_SCENARIO is Not SYS_SUCCESS [ FOUP ABSENT ]", CASSETTE_INDEX);
					continue; // No Foup // jelee Added.
//					return SYS_ERROR;
				}
				else // Complete Option
				{ // SYS_SUCCESS, Foup Present
//					LOG(pszFtnName, ELT_File, "[LPM%d] LOADING AGV_RECOVERY_SCENARIO is SYS_SUCCESS [ FOUP PRESENT ]", CASSETTE_INDEX);
					sprintf( StrBuffer , EVENT_STRING_TR_LOAD_RECV , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
					AGV_INIT_SET_IO( -1 );
					break; // 1st while out, Complete recovery
				}
				return SYS_ABORTED;
			}
			else if ( Status == SYS_ERROR   ) // Complete Option
			{
//				LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When Load Sequence------>101", CASSETTE_INDEX);
//				LOG(pszFtnName, ELT_File, "[LPM%d] LOADING FOUP_SENSOR_ERROR is SYS_ERROR", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO2( LOAD, COMPLETE1 ); // jelee Added.
				if( Status != SYS_SUCCESS)
				{
//					LOG(pszFtnName, ELT_File, "[LPM%d] LOADING AGV_RECOVERY_SCENARIO is Not SYS_SUCCESS [ FOUP ABSENT ]", CASSETTE_INDEX);
					continue; // No Foup // jelee Added.
//					return SYS_ERROR;
				}
				else
				{ // SYS_SUCCESS, Foup Present
//					LOG(pszFtnName, ELT_File, "[LPM%d] LOADING AGV_RECOVERY_SCENARIO is SYS_SUCCESS [ FOUP PRESENT ]", CASSETTE_INDEX);
					sprintf( StrBuffer , EVENT_STRING_TR_LOAD_RECV , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
					AGV_INIT_SET_IO( -1 );
					break; // 1st while out, Complete recovery
				}
			}
*/// 2008.05.10 jelee Delete Sequence Release End.

//			printf( "=====> CM%d Load Handling : 3-2\n" , CASSETTE_INDEX);//jelee
			if ( READ_DIGITAL_TRUE( LP_AGV_STS , &Data2 ) == SYS_ABORTED )
			{
				LOG(pszFtnName, ELT_File, "[LPM%d] LP_AGV_STS = [SYS_ABORTED]", CASSETTE_INDEX);
				return SYS_ABORTED;
			}
			if ( Data2 == ERROR1 )
			{ 
				LOG(pszFtnName, ELT_File, "[LPM%d] LP_AGV_STS = [ERROR1]", CASSETTE_INDEX);
				ALARM_MESSAGE_REMAPPING( ALARM_START_POINTER + ALARM_PORT_AGV_ERROR , &Error );
				AlmResult = ALARM_MANAGE_FREE( ALARM_START_POINTER + ALARM_PORT_AGV_ERROR );
				WRITE_DIGITAL( PIO_ERR_CLEAR , RUN , &CommStatus );
				if ( AlmResult == ALM_ABORT ) return SYS_ABORTED;
			}
//			printf( "=====> CM%d Load Handling : 3-3\n" , CASSETTE_INDEX);//jelee
			if ( READ_DIGITAL_TRUE( LP_AGV_INPUT_CS , &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( Data == ON || Data2 == BUSY ) {
				printf( "######> Start CM%d Load AGV\n" , CASSETTE_INDEX );
				LOG(pszFtnName, ELT_File, "[LPM%d] AGV Load Start", CASSETTE_INDEX);
				WRITE_DIGITAL( LP_AMHS_MOVING , ON , &CommStatus ); // load moving
				if ( AGV_LOAD_CONTROL() != SYS_SUCCESS ) {
					printf( "#####> Abort CM%d Load AGV\n" , CASSETTE_INDEX );
					LOG(pszFtnName, ELT_File, "[LPM%d] AGV Load Abort", CASSETTE_INDEX);
					AGV_INIT_SET_IO( -3 );
					WRITE_DIGITAL( LP_AMHS_MOVING , OFF , &CommStatus ); // idle
					WRITE_DIGITAL( PIO_ERR_CLEAR , RUN , &CommStatus );
					return SYS_ABORTED;
				}
				else {
					printf( "#####> End CM%d Load AGV\n" , CASSETTE_INDEX );
					LOG(pszFtnName, ELT_File, "[LPM%d] AGV Load End", CASSETTE_INDEX);
					AGV_INIT_SET_IO( -1 );
					WRITE_DIGITAL( LP_AMHS_MOVING , OFF , &CommStatus ); // idle
					while ( TRUE ) { // wait foup present
						_sleep(250);
						
						switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
						case 1 : case 4 : break;
						default:
							return SYS_ABORTED;
							break;
						}

						if ( READ_DIGITAL_TRUE( LP_FOUP_STS , &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
						if ( Data == EXIST ) break;
					}
					WRITE_DIGITAL_DOUBLE( LP_UNLOADING_SIGNAL , LP_LOADING_SIGNAL , NONE3 , &CommStatus );
					break; // 1st while out
				}
			}
		}
		else { // manual mode
//			printf( "=====> CM%d Load Handling : 4\n" , CASSETTE_INDEX);//jelee
			Indicator( LOADING );
//			printf( "=====> CM%d Load Handling : 5\n" , CASSETTE_INDEX);//jelee
			if ( READ_DIGITAL_TRUE( LP_FOUP_STS , &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
//			printf( "=====> CM%d Load Handling : 6\n" , CASSETTE_INDEX);//jelee

			if ( Data == EXIST ) {
				sprintf( StrBuffer , EVENT_STRING_TR_LOAD_RECV , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
				WRITE_DIGITAL( LP_LOAD_LAMP , ON , &CommStatus );
				Current = LOADING;
				_beginthread( (void *)Blink , 0 , NULL );
				break; // 1st while out
			}
		}
	} // End of 1st while()
	while( TRUE ) { // 2nd while() : Clamp Action
		_sleep(250);
//		printf( "=====> CM%d Load Handling : 7\n" , CASSETTE_INDEX);//jelee
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
		case 1 : case 4 : break;
		default:
			return SYS_ABORTED;
			break;
		}
//		printf( "=====> CM%d Load Handling : 8\n" , CASSETTE_INDEX);//jelee

		if ( READ_DIGITAL_TRUE( LP_FOUP_STS		, &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
//		printf( "=====> CM%d Load Handling : 9\n" , CASSETTE_INDEX);//jelee
		if ( Data == NOTEXIST ) return SYS_ABORTED;
//		printf( "=====> CM%d Load Handling : 10\n" , CASSETTE_INDEX);//jelee
		
		if ( READ_DIGITAL_TRUE( LP_MODE_AGV_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
//		printf( "=====> CM%d Load Handling : 11\n" , CASSETTE_INDEX);//jelee

		if ( Data == AMHS ) {
			Indicator( NONE );
			WRITE_DIGITAL_DOUBLE( LP_UNLOADING_SIGNAL , LP_LOADING_SIGNAL , NONE3 , &CommStatus );
			if ( WRITE_DIGITAL_TRUE( LP_CLAMP , CLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( SIMULATION_CASS_MODE == TRUE )
			{
				if ( WRITE_DIGITAL_TRUE( LP_CLAMP_STS , CLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
			}
			break;
		}
		else { // Dynamic Load/Unload (Auto Clamp)
			Indicator( NONE );
			WRITE_DIGITAL_DOUBLE( LP_UNLOADING_SIGNAL , LP_LOADING_SIGNAL , NONE3 , &CommStatus );
			if ( WRITE_DIGITAL_TRUE( LP_CLAMP , CLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( SIMULATION_CASS_MODE == TRUE )
			{
				if ( WRITE_DIGITAL_TRUE( LP_CLAMP_STS , CLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
			}		
			break;
		}
		
//		printf( "=====> CM%d Load Handling : 17\n" , CASSETTE_INDEX);//jelee
		if ( READ_DIGITAL_TRUE( LP_CLAMP_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
//		printf( "=====> CM%d Load Handling : 18\n" , CASSETTE_INDEX);
		if ( Data == CLAMP ) {
//			printf( "=====> CM%d Load Handling : 19\n" , CASSETTE_INDEX);
			Indicator( NONE );
//			printf( "=====> CM%d Load Handling : 20\n" , CASSETTE_INDEX);
			break;
		}
	}
	WRITE_DIGITAL( LP_LOAD_LAMP , OFF , &CommStatus );
	WRITE_DIGITAL_DOUBLE( LP_UNLOADING_SIGNAL , LP_LOADING_SIGNAL , NONE3 , &CommStatus );

	if ( SIMULATION_AGV_MODE ) {
		if ( READ_DIGITAL( LP_CLAMP_STS , &CommStatus ) != CLAMP ) {
			if ( !WAIT_SECONDS( PORT_TIME ) ) return SYS_ABORTED;
			WRITE_DIGITAL( LP_CLAMP_STS , CLAMP , &CommStatus );
		}
	}

//	printf( "=====> CM%d Load Handling : 21\n" , CASSETTE_INDEX);
	Indicator( NONE );
//	printf( "=====> CM%d Load Handling : 22\n" , CASSETTE_INDEX);
	S_TIMER_READY();
	while ( TRUE ) { // 3rd while() : Wait & Check Clamped Status
		_sleep(250);

//		printf( "=====> CM%d Load Handling : 23\n" , CASSETTE_INDEX);
		switch( READ_DIGITAL( LP_MDL_STATUS , &CommStatus ) ) {
//		printf( "=====> CM%d Load Handling : 24\n" , CASSETTE_INDEX);
		case 1 : case 4 : break;
		default:
			return SYS_ABORTED;
			break;
		}
		
//		printf( "=====> CM%d Load Handling : 25\n" , CASSETTE_INDEX);
		if ( READ_DIGITAL_TRUE( LP_FOUP_STS , &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
//		printf( "=====> CM%d Load Handling : 26\n" , CASSETTE_INDEX);
		if ( Data == NOTEXIST ) return SYS_ABORTED;

//		printf( "=====> CM%d Load Handling : 27\n" , CASSETTE_INDEX);
		if ( READ_DIGITAL_TRUE( LP_CLAMP_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
//		printf( "=====> CM%d Load Handling : 28\n" , CASSETTE_INDEX);
		if ( Data == CLAMP ) { 
			_sleep( 1000 ); 
			break; 
		} // event report waiting = 200msec

		if ( S_TIMER_CHECK( 1 , DEFAULT_TIME_OUT ) ) {
			AlmResult = ALARM_MANAGE_FREE( ALARM_START_POINTER + ALARM_PORT_CLAMP_TIMEOUT );
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			else {
				if ( WRITE_DIGITAL_TRUE( LP_CLAMP , CLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
				S_TIMER_READY();
			}
		}
	}
	sprintf( StrBuffer , EVENT_STRING_LOAD_COMPLETE , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );

	// HO signal Off Test
	//WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable



// Jelee Add 2007.12.06 => Chamber DryCleaning Reserve Status Check. If Reserved, Then Lot Cancel.
	//bRunStatus = PM_Run_Status_Check();
	//if( bRunStatus == TRUE )
	{
		if (1 == CASSETTE_INDEX) {
			nSkip = READ_DIGITAL(FC_CM1_MID_CONTROL , &CommStatus);
		}
		else if (2 == CASSETTE_INDEX) {
			nSkip = READ_DIGITAL(FC_CM2_MID_CONTROL , &CommStatus);
		}
		else if (3 == CASSETTE_INDEX) {
			nSkip = READ_DIGITAL(FC_CM3_MID_CONTROL , &CommStatus);
		}

		//2010.08.10 mgsong
		//Foup Place ~ MID Read Delay due to Shuttle Sensor Hunting when Operator Manual Loading
		if ( READ_DIGITAL_TRUE( LP_MODE_AGV_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if (Data == MANUAL && 0 == nSkip) {
			printf("*_____* Manual Load Delay, Shuttle Out Confirm Start!! \n");

			_sleep(3000);

			if (1 == CASSETTE_INDEX) {
				WRITE_DIGITAL(LPMAUnDockCmd , SHUTTLEOUT , &CommStatus);
			}
			else if (2 == CASSETTE_INDEX) {
				WRITE_DIGITAL(LPMBUnDockCmd , SHUTTLEOUT , &CommStatus);
			}
			else if (3 == CASSETTE_INDEX) {
				WRITE_DIGITAL(LPMCUnDockCmd , SHUTTLEOUT , &CommStatus);
			}
			
			_sleep(3000);

			printf("*_____* Manual Load Delay, Shuttle Out Confirm End!! \n");

		}
		//2010.08.10 mgsong

		if ( RUN_FUNCTION_FREE( MID_CONTROL , "MID_READ" ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	//else
	//{
		//sprintf( StrBuffer , EVENT_PORT_CANCEL , CASSETTE_INDEX );
		//WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
		//return SYS_ABORTED;
	//}
// Jelee End 2007.12.06 => Chamber DryCleaning Reserve Status Check. If Reserved, Then Lot Cancel.

//	printf( "=====> CM%d Load Handling : 30\n" , CASSETTE_INDEX);

	return SYS_SUCCESS;
}
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
Module_Status Unload_Control() {
//	Module_Status Status;
	int CommStatus , AlmResult , OLD_AUTO_MODE , Data , Data2 , Data3 , Cancel , Error;
	char pszFtnName[256] = ""; // jelee added 2005.11.11
	BOOL Check = FALSE;

	printf( "=====> Start CM%d Unload Handling : %s\n" , CASSETTE_INDEX , PROGRAM_PARAMETER_READ() );

	if ( READ_DIGITAL_TRUE( LP_MODE_AGV_STS , &Data ) == SYS_ABORTED )
	{
		return SYS_ABORTED;
	}

	OLD_AUTO_MODE = Data;
	if( OLD_AUTO_MODE == MANUAL )
	{
		Indicator( UNLOADING );
	}
	else 
	{
		Indicator( NONE );
	}
	
	_sleep( 100 ); // engine unload req. event sent wait time
	Cancel = READ_DIGITAL( LP_OPERATOR_CANCEL , &CommStatus );
	if ( Cancel == OFF )
	{	// HOST CANCEL
		sprintf( StrBuffer , EVENT_UNLOAD_REQUEST , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
	}
	else
	{
		sprintf( StrBuffer , EVENT_UNLOAD_REQUEST , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
		WRITE_DIGITAL( LP_OPERATOR_CANCEL , OFF	, &CommStatus );	// GUI CANCEL
	}

	WRITE_DIGITAL( LP_PROCESS_END , ON , &CommStatus );
	WRITE_DIGITAL_DOUBLE( LP_UNLOADING_SIGNAL , LP_LOADING_SIGNAL , UNLOADING , &CommStatus );
	LOG(pszFtnName, ELT_File, "[LPM%d] Unloading SIGNAL IS ON", CASSETTE_INDEX);

	while ( TRUE ) { // 1st while() : Wait Unload
		_sleep(250);

		if ( READ_DIGITAL_TRUE( LP_MODE_AGV_STS , &Data ) == SYS_ABORTED )
		{
			return SYS_ABORTED;
		}
		if ( OLD_AUTO_MODE != Data )
		{
			OLD_AUTO_MODE = Data;
			if ( Data == AMHS )
			{
				AGV_INIT_SET_IO( -1 );
			}
			else 
			{
				AGV_INIT_SET_IO( -2 );
			}
		}
		
		if ( Data == AMHS )
		{
			AGV_INIT_SET_IO( -1 ); // jelee added 2005.10.18
//			LOG(pszFtnName, ELT_File, "[LPM%d] Unload Sequence is AMHS", CASSETTE_INDEX);
			//EFEM Alarm Check Function Start by jelee
			if((READ_DIGITAL( LP_ALARM_STS , &CommStatus ) == PRESENT))
			{
//				LOG(pszFtnName, ELT_File, "[LPM%d] Unoad Sequence ALARM Detected", CASSETTE_INDEX);
				WRITE_DIGITAL( LP_AGV_OUTPUT_HOAVBL , OFF , &CommStatus ); // not avaliable
			}
			//EFEM Alarm Check Function End

			Indicator( NONE );
//			Status = FOUP_SENSOR_CHECK2( FALSE , FALSE );	// jelee Delete
//			printf( "=====> CM%d UNLoad Handling : 3-1\n" , CASSETTE_INDEX);//jelee

// 2008.05.10 jelee Delete Sequence Release start
/*			Status = FOUP_SENSOR_ERROR2( FALSE , FALSE ); // Foup check before unload.
			if( Status == SYS_ABORTED ) // Retry Option
			{
//				LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>100", CASSETTE_INDEX);
//				LOG(pszFtnName, ELT_File, "[LPM%d] Unload FOUP SENDOR ERROR = [SYS_ABORTED]", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO2( UNLOAD, RETRY1 ); // jelee Added.
				if( Status != SYS_SUCCESS)
				{
//					LOG(pszFtnName, ELT_File, "[LPM%d] UNLOADING AGV_RECOVERY_SCENARIO is Not SYS_SUCCESS [ FOUP PRESENT ]", CASSETTE_INDEX);
					continue; // Foup Present // jelee Added.
				}
				else
				{ // SYS_SUCCESS, Foup Absent
//					LOG(pszFtnName, ELT_File, "[LPM%d] UNLOADING AGV_RECOVERY_SCENARIO is SYS_SUCCESS [ FOUP ABSENT ]", CASSETTE_INDEX);
					sprintf( StrBuffer , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
					AGV_INIT_SET_IO( -1 );
					break; // 1st while out, Complete recovery
				}
			}
			else if ( Status == SYS_ERROR  ) // Complete Option
			{
//				LOG(pszFtnName, ELT_File, "[LPM%d] FOUP Sensor Error When UnLoad Sequence------>101", CASSETTE_INDEX);
//				LOG(pszFtnName, ELT_File, "[LPM%d] Unload FOUP SENDOR ERROR = [SYS_ERROR]", CASSETTE_INDEX);
				Status = AGV_RECOVERY_SCENARIO2( UNLOAD, COMPLETE1 ); // jelee Added.
				if( Status != SYS_SUCCESS)
				{
//					LOG(pszFtnName, ELT_File, "[LPM%d] UNLOADING AGV_RECOVERY_SCENARIO is Not SYS_SUCCESS [ FOUP PRESENT ]", CASSETTE_INDEX);
					continue; // Foup Present // jelee Added.
				}
				else
				{ // SYS_SUCCESS, Foup Absent
//					LOG(pszFtnName, ELT_File, "[LPM%d] UNLOADING AGV_RECOVERY_SCENARIO is SYS_SUCCESS [ FOUP ABSENT ]", CASSETTE_INDEX);
					sprintf( StrBuffer , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
					WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
					AGV_INIT_SET_IO( -1 );
					break; // 1st while out, Complete recovery
				}
			}
*/// 2008.05.10 jelee Delete Release End.

			if ( READ_DIGITAL_TRUE( LP_CLAMP_STS , &Data ) == SYS_ABORTED )
			{
				LOG(pszFtnName, ELT_File, "[LPM%d] Unload LP_CLAMP_STS = [CLAMP]", CASSETTE_INDEX);
				return SYS_ABORTED;
			}
			if ( Data != UNCLAMP )
			{
				LOG(pszFtnName, ELT_File, "[LPM%d] Unload LP_UNCLAMP WRITE [UNCLAMP], But ERROR => 1", CASSETTE_INDEX);
				WRITE_DIGITAL( LP_CLAMP , UNCLAMP, &CommStatus ); //jelee add 2006.04.20
				_sleep(2000); //jelee add 2006.04.26
				READ_DIGITAL_TRUE( LP_CLAMP_STS , &Data );
				if (Data != UNCLAMP)
				{
					LOG(pszFtnName, ELT_File, "[LPM%d] = [STILL CLAMP]", CASSETTE_INDEX);
					if ( WRITE_DIGITAL_TRUE( LP_CLAMP , UNCLAMP ) == SYS_ABORTED )
					{
						LOG(pszFtnName, ELT_File, "[LPM%d] Unload LP_CLAMP WRITE [UNCLAMP], But ERROR => 2", CASSETTE_INDEX);
						return SYS_ABORTED;
					}
				}
			}

			if ( READ_DIGITAL_TRUE( LP_AGV_STS , &Data2 ) == SYS_ABORTED ) 
			{
				LOG(pszFtnName, ELT_File, "[LPM%d] Unload LP_AGV_STS = [SYS_ABORTED]", CASSETTE_INDEX);
				return SYS_ABORTED;
			}
			if ( Data2 == ERROR1 )
			{ 
				LOG(pszFtnName, ELT_File, "[LPM%d] Unload LP_CLAMP_STS = [ERROR1]", CASSETTE_INDEX);
				ALARM_MESSAGE_REMAPPING( ALARM_START_POINTER + ALARM_PORT_AGV_ERROR , &Error );
				AlmResult = ALARM_MANAGE_FREE( ALARM_START_POINTER + ALARM_PORT_AGV_ERROR );
				WRITE_DIGITAL( PIO_ERR_CLEAR , RUN , &CommStatus );
				if ( AlmResult == ALM_ABORT ) return SYS_ABORTED;
			}

			if ( READ_DIGITAL_TRUE( LP_AGV_INPUT_CS , &Data  ) == SYS_ABORTED )
			{
				return SYS_ABORTED;
			}
			if ( Data == ON || Data2 == BUSY )
			{
				printf( "#####> Start CM%d Unload AGV\n" , CASSETTE_INDEX );
				LOG(pszFtnName, ELT_File, "[LPM%d] AGV Unload Start", CASSETTE_INDEX);
				WRITE_DIGITAL( LP_AMHS_MOVING , ON , &CommStatus ); // load moving
				if ( AGV_UNLOAD_CONTROL(0) != SYS_SUCCESS )
				{
					printf( "=====> Abort CM%d Unload AGV\n" , CASSETTE_INDEX );
					LOG(pszFtnName, ELT_File, "[LPM%d] AGV Unload Abort", CASSETTE_INDEX);
					AGV_INIT_SET_IO( -3 );
					WRITE_DIGITAL( LP_AMHS_MOVING , OFF , &CommStatus ); // idle
					WRITE_DIGITAL( PIO_ERR_CLEAR , RUN , &CommStatus );
					if ( Cancel )
					{
						WRITE_DIGITAL( LP_OPERATOR_CANCEL , ON , &CommStatus );
					}
					return SYS_ABORTED;
				}
				else 
				{
					printf( "=====> End CM%d Unload AGV\n" , CASSETTE_INDEX );
					LOG(pszFtnName, ELT_File, "[LPM%d] AGV Unload End", CASSETTE_INDEX);
					AGV_INIT_SET_IO( -1 );
					WRITE_DIGITAL( LP_AMHS_MOVING , OFF , &CommStatus ); // idle
					break; // 1st while out
				}
			}
		}
		else { // manual mode
			Indicator( UNLOADING );
			
			if ( READ_DIGITAL_TRUE( LP_UNLOAD_LAMP , &Data ))
			{
				if ( Data == OFF )
					WRITE_DIGITAL( LP_UNLOAD_LAMP , ON , &CommStatus );	
			}
			if ( READ_DIGITAL_TRUE( LP_UNLOAD_BUTTON , &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( Data == ON  ) {
				sprintf( StrBuffer , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
				Current = UNLOADING;
				_beginthread( (void *)Blink , 0 , NULL );
				if ( WRITE_DIGITAL_TRUE( LP_CLAMP , UNCLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
				break; // 1st while out
			}
			if ( READ_DIGITAL( LP_CLAMP_STS , &CommStatus ) != UNCLAMP )
			{
				if ( !WAIT_SECONDS( PORT_TIME ) ) return SYS_ABORTED;
					WRITE_DIGITAL( LP_CLAMP , UNCLAMP , &CommStatus );
					if ( SIMULATION_CASS_MODE == TRUE )
					{
						if ( WRITE_DIGITAL_TRUE( LP_CLAMP_STS , UNCLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
					}
			}
			
			if ( READ_DIGITAL_TRUE( LP_CLAMP_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( Data == UNCLAMP ) {
				sprintf( StrBuffer , EVENT_STRING_TR_UNLOAD_RECV , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
				Current = UNLOADING;
				_beginthread( (void *)Blink , 0 , NULL );
				break; 
			}
		}
	} // End of 1st while()
	WRITE_DIGITAL( LP_UNLOAD_LAMP , OFF , &CommStatus );

	if ( SIMULATION_AGV_MODE ) {
		if ( READ_DIGITAL( LP_CLAMP_STS , &CommStatus ) != UNCLAMP ) {
			if ( !WAIT_SECONDS( PORT_TIME ) ) return SYS_ABORTED;
			WRITE_DIGITAL( LP_CLAMP_STS , UNCLAMP , &CommStatus );
		}
	}

	S_TIMER_READY();
	while ( TRUE ) { // 2rd while() : Wait & Check Unclamped Status
		_sleep(250);

		if ( READ_DIGITAL_TRUE( LP_CLAMP_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data == UNCLAMP ) { 
			_sleep( 1000 ); 
			break; 
		} // event report waiting = 200msec

		if ( S_TIMER_CHECK( 1 , DEFAULT_TIME_OUT ) ) {
			AlmResult = ALARM_MANAGE_FREE( ALARM_START_POINTER + ALARM_PORT_UNCLAMP_TIMEOUT );
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			else {
				if ( WRITE_DIGITAL_TRUE( LP_CLAMP , UNCLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
				S_TIMER_READY();
			}
		}
	}
	
	while ( TRUE ) { // 3rd while() : wait foup unload
		_sleep(250);
		
		// manual -> auto change case
		if ( READ_DIGITAL_TRUE( LP_MODE_AGV_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( OLD_AUTO_MODE != Data ) {
			OLD_AUTO_MODE = Data;
			if ( Data == AMHS ) AGV_INIT_SET_IO( -1 );
			else AGV_INIT_SET_IO( -2 );
		}

		if ( Data == AMHS ) {
			Indicator( NONE );
			
			if ( READ_DIGITAL_TRUE( LP_CLAMP_STS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( Data != UNCLAMP ) { 
				if ( WRITE_DIGITAL_TRUE( LP_CLAMP , UNCLAMP ) == SYS_ABORTED ) return SYS_ABORTED;
			}

			if ( READ_DIGITAL_TRUE( LP_AGV_STS , &Data2 ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( Data2 == ERROR1 ) { 
				ALARM_MESSAGE_REMAPPING( ALARM_START_POINTER + ALARM_PORT_AGV_ERROR , &Error );
				AlmResult = ALARM_MANAGE_FREE( ALARM_START_POINTER + ALARM_PORT_AGV_ERROR );
				WRITE_DIGITAL( PIO_ERR_CLEAR , RUN , &CommStatus );
				if ( AlmResult == ALM_ABORT ) return SYS_ABORTED;
			}

			if ( READ_DIGITAL_TRUE( LP_AGV_INPUT_CS , &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( READ_DIGITAL_TRUE( LP_FOUP_STS		, &Data3 ) == SYS_ABORTED ) return SYS_ABORTED;
			if ( ( Data == ON || Data2 == BUSY ) && Data3 == EXIST ) {
				WRITE_DIGITAL( LP_AMHS_MOVING , ON , &CommStatus ); // load moving
				if ( AGV_UNLOAD_CONTROL(0) != SYS_SUCCESS ) {
					AGV_INIT_SET_IO( -3 );
					WRITE_DIGITAL( LP_AMHS_MOVING , OFF , &CommStatus ); // idle
					WRITE_DIGITAL( PIO_ERR_CLEAR , RUN , &CommStatus );
					return SYS_ABORTED;
				}
				else {
					AGV_INIT_SET_IO( -1 );
					WRITE_DIGITAL( LP_AMHS_MOVING , OFF , &CommStatus ); // idle
				}
			}
		}

		if ( READ_DIGITAL_TRUE( LP_FOUP_STS , &Data  ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data == NOTEXIST ) break;
	}

	sprintf( StrBuffer , EVENT_STRING_UNLOAD_COMPLETE	, CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
	//
	sprintf( StrBuffer , EVENT_STRING_STS_UNOCCUPIED	, CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
	//
	sprintf( StrBuffer , EVENT_STRING_CARR_DELETED		, CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
	//
	sprintf( StrBuffer , EVENT_STRING_STS_DELETED		, CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );
	//
	sprintf( StrBuffer , EVENT_STRING_CARR_NOTASSOCIATED, CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , StrBuffer , &CommStatus );

	_sleep(500);

	Indicator( NONE );
	Current = NONE;

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int id , Res , CommStatus;
	char RunStr[31] , RunStr1[31] , IdStr[31] , BufStr[31];
	char pszFtnName[256] = ""; // jelee added 2007.12.05

	CONTROL_REFRESH = FALSE; // jelee Added.

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , BufStr , 30 );
	STR_SEPERATE( BufStr , RunStr1 , IdStr , 30 );

	LOG(pszFtnName, ELT_File, "[LPM%d] PROGRAM_PARAMETER_READ=>[%s]", CASSETTE_INDEX, PROGRAM_PARAMETER_READ()); // jelee added 2007.12.05

	if      ( STRCMP_L( IdStr , "DISABLE" ) ) id = 0;
	else if ( STRCMP_L( IdStr , "ENABLE"  ) ) id = 1;
	else if ( STRCMP_L( IdStr , "ENABLE2" ) ) id = 2;
	else if ( STRCMP_L( IdStr , "ENABLE3" ) ) id = 3;
	else if ( STRCMP_L( IdStr , "ENABLE4" ) ) id = 4;
	else {
		printf( "=====> CM%d Foup Exchange Control Check Error - Not Support Code %s\n" , CASSETTE_INDEX , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}

	if ( id == 0 ) 
	{ // Manual
		CONTROL_AUTO_MODE = FALSE;
		AGV_INIT_SET_IO( -2 );
		WRITE_DIGITAL( LP_MODE_AGV_SET , MANUAL , &CommStatus );
//		if ( SIMULATION_AGV_MODE ) WRITE_DIGITAL( LP_MODE_AGV_STS , MANUAL , &CommStatus );
		if ( !CommStatus ) {
			_sleep( 1 );
			WRITE_DIGITAL( LP_MODE_AGV_SET , MANUAL , &CommStatus );
		}
	}
	else 
	{ // auto = amhs
		CONTROL_AUTO_MODE = TRUE;
		AGV_INIT_SET_IO( -1 );
		WRITE_DIGITAL( LP_MODE_AGV_SET , AMHS , &CommStatus );
//		if ( SIMULATION_AGV_MODE ) WRITE_DIGITAL( LP_MODE_AGV_STS , AMHS , &CommStatus );
		if ( !CommStatus ) {
			_sleep( 1 );
			WRITE_DIGITAL( LP_MODE_AGV_SET , AMHS , &CommStatus );
		}
	}
	//--
 // jelee Added Start 2005.08.08
	WRITE_DIGITAL( LP_AMHS_MOVING , OFF , &CommStatus );
	if      ( STRCMP_L( RunStr , "LOAD"   ) ) {
		WRITE_DIGITAL( LP_OPERATOR_CANCEL	, OFF , &CommStatus );
		WRITE_DIGITAL( LP_PROCESS_END		, OFF , &CommStatus );
		WRITE_DIGITAL( LP_UNLOADING_SIGNAL	, OFF , &CommStatus );
		WRITE_DIGITAL( LP_LOADING_SIGNAL	, ON  , &CommStatus );
//		WRITE_DIGITAL( LP_LOAD_OPSW		, OFF , &CommStatus );
//		WRITE_DIGITAL( LP_UNLOAD_OPSW		, OFF , &CommStatus );
		Res = Load_Control();
		WRITE_DIGITAL( LP_UNLOADING_SIGNAL	, OFF , &CommStatus );
		WRITE_DIGITAL( LP_LOADING_SIGNAL	, OFF , &CommStatus );
		WRITE_DIGITAL( LP_LOAD_LAMP		, OFF , &CommStatus );
		WRITE_DIGITAL( LP_UNLOAD_LAMP		, OFF , &CommStatus );
//		WRITE_DIGITAL( LP_LOAD_OPSW		, OFF , &CommStatus );
//		WRITE_DIGITAL( LP_UNLOAD_OPSW		, OFF , &CommStatus );
		WRITE_DIGITAL( LP_AMHS_MOVING		, OFF , &CommStatus );
		return Res;
	}
	else if ( STRCMP_L( RunStr , "UNLOAD" ) ) {
		WRITE_DIGITAL( LP_PROCESS_END		, ON  , &CommStatus );
		WRITE_DIGITAL( LP_LOADING_SIGNAL	, OFF , &CommStatus );
		WRITE_DIGITAL( LP_UNLOADING_SIGNAL	, ON  , &CommStatus );
//		WRITE_DIGITAL( LP_LOAD_OPSW		, OFF , &CommStatus );
//		WRITE_DIGITAL( LP_UNLOAD_OPSW		, OFF , &CommStatus );
		Res = Unload_Control();
		WRITE_DIGITAL( LP_LOADING_SIGNAL	, OFF , &CommStatus );
		WRITE_DIGITAL( LP_UNLOADING_SIGNAL , OFF , &CommStatus );
		WRITE_DIGITAL( LP_LOAD_LAMP		, OFF , &CommStatus );
		WRITE_DIGITAL( LP_UNLOAD_LAMP		, OFF , &CommStatus );
//		WRITE_DIGITAL( LP_LOAD_OPSW		, OFF , &CommStatus );
//		WRITE_DIGITAL( LP_UNLOAD_OPSW		, OFF , &CommStatus );
		WRITE_DIGITAL( LP_AMHS_MOVING		, OFF , &CommStatus );
		WRITE_DIGITAL( LP_PROCESS_END		, OFF , &CommStatus );

/*		if ( Res == SYS_ABORTED ) {
			WRITE_STRING( CARRIER_ID + CASSETTE_INDEX - 1 , StrCarrierID , &CommStatus );
			WRITE_STRING( CARREAD_ID + CASSETTE_INDEX - 1 , StrCarrierID , &CommStatus );
			WRITE_STRING( JOBNAME_ID + CASSETTE_INDEX - 1 , StrJobID     , &CommStatus );
			WRITE_STRING( JOBREAD_ID + CASSETTE_INDEX - 1 , StrJobID     , &CommStatus );
			WRITE_STRING( RECIPE_ID  + CASSETTE_INDEX - 1 , StrPPID      , &CommStatus );
			WRITE_STRING( LOTNAME_ID + CASSETTE_INDEX - 1 , StrLotID     , &CommStatus );
		}*/
		return Res;
	}
// jelee Added End.
	printf( "=====> CM%d Foup Exchange Control Parameter Error - Not Support Code %s\n" , CASSETTE_INDEX , PROGRAM_PARAMETER_READ() );
	return SYS_ABORTED;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
// jelee Delete Start

	char Str1[31] , Str2[31] , Str3[31] , ElseStr[31];
	STR_SEPERATE( PROGRAM_ARGUMENT_READ() , Str1 , ElseStr , 30 ); // "CM1 PORT1 100"
	REPLACE_CHAR_SET( 0 , Str1 );
	CASSETTE_INDEX = Str1[2] - '0';
	STR_SEPERATE( ElseStr , Str2 , Str3 , 30 );
	REPLACE_CHAR_SET( 1 , Str2 );
	STR_SEPERATE( Str3    , Str1 , Str2 , 30 );
	ALARM_START_POINTER = atoi( Str1 );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
	int CommStatus;
	char RunStr[31] , BufStr[31];

	STR_SEPERATE( PROGRAM_EVENT_READ() , RunStr , BufStr , 30 );
	if ( READ_DIGITAL( LP_AMHS_MOVING , &CommStatus ) != OFF ) return;

	if      ( STRCMP_L( RunStr , "DISABLE" ) ) {
		CONTROL_AUTO_MODE = FALSE;
		AGV_INIT_SET_IO( -2 );
	}
	else if ( STRCMP_L( RunStr , "ENABLE" ) ) {
		CONTROL_AUTO_MODE = TRUE;
		AGV_INIT_SET_IO( -1 );
	}
	else if ( STRCMP_L( RunStr , "ENABLE2" ) ) {
		CONTROL_AUTO_MODE = TRUE;
		AGV_INIT_SET_IO( -1 );
	}
	else if ( STRCMP_L( RunStr , "ENABLE3" ) ) {
		CONTROL_AUTO_MODE = TRUE;
		AGV_INIT_SET_IO( -1 );
	}
	else if ( STRCMP_L( RunStr , "ENABLE4" ) ) {
		CONTROL_AUTO_MODE = TRUE;
		AGV_INIT_SET_IO( -1 );
	}
	// jelee Added Start
	else if (STRCMP_L(RunStr   , "REFRESH" ) )  {
		CONTROL_REFRESH = TRUE;
	}
	else{
		MONITOR_VIEW = TRUE;
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
//	int CommStatus;
	// jelee Delete Start 2005.08.16
	// char BufferI[256] , BufferO[256];
	
// 	SIMULATION_AGV_MODE		= FALSE;
// 	SIMULATION_CASS_MODE	= FALSE;
// 	SIMULATION_LSWITCH_MODE = FALSE;
// 	SIMULATION_USWITCH_MODE = FALSE;
// 	
// 	if ( READ_DIGITAL( IO_Driver_Loading , &CommStatus ) == 0 )	{
		SIMULATION_AGV_MODE		= TRUE;
		SIMULATION_CASS_MODE	= TRUE;
		SIMULATION_LSWITCH_MODE = TRUE;
		SIMULATION_USWITCH_MODE = TRUE;
// 	}

//	if(READ_DIGITAL(IO_Driver_Loading, &CommStatus) == 0 )
	{
		_beginthread( (void *)EXCHANGE_SIM_RUN , 0 , 0 );
	}
	
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
