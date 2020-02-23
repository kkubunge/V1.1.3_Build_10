///////////////////////////////////////////////////////////////////////////////
// AbortProcess

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
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================
// Thread Type Definition for Using thread

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern void _LOG(const char *pFormat, ...);

BOOL	SIMULATION_MODE		= FALSE;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	//
	{ "GEM.Control_State"			, _K_D_IO	,	CONTROL_STATE			,	0	} , 

	{ "LPMA.FOUP_PresentDI"			, _K_D_IO	,	CM1_CASS_SenSor_STATUS	,	0	} ,
	{ "LPMB.FOUP_PresentDI"			, _K_D_IO	,	CM2_CASS_SenSor_STATUS	,	0	} ,
	{ "LPMC.FOUP_PresentDI"			, _K_D_IO	,	CM3_CASS_SenSor_STATUS	,	0	} ,
	
	{ "CurrentTime"					, _K_S_IO	,	CurrentTime				,	0   } ,	
	{ "IPS_VERSION"					, _K_S_IO	,	IPS_VERSION				,	0	} ,
	{ "PM1_IPS_VERSION"				, _K_S_IO	,	PM1_IPS_VERSION			,	0	} ,
	{ "PM2_IPS_VERSION"				, _K_S_IO	,	PM2_IPS_VERSION			,	0	} ,
	{ "PM3_IPS_VERSION"				, _K_S_IO	,	PM3_IPS_VERSION			,	0	} ,
	{ "Ver_Build_Date"				, _K_S_IO	,	Ver_Build_Date			,	0	} ,
		
	{ "PRMA_BM1_SVENT_PRES"			, _K_A_IO	,	PRMA_BM1_SVENT_PRES		,	0	} ,	
	{ "PRMA_BM1_FVENT_PRES"			, _K_A_IO	,	PRMA_BM1_FVENT_PRES		,	0	} ,	
	{ "PRMA_BM1_VENT_TD"			, _K_A_IO	,	PRMA_BM1_VENT_TD		,	0	} ,	
	{ "PRMA_BM1_SPUMP_PRES"			, _K_A_IO	,	PRMA_BM1_SPUMP_PRES		,	0	} ,	
	{ "PRMA_BM1_FPUMP_PRES"			, _K_A_IO	,	F_PUMP_PRESSURE_BM1		,	0	} ,	
	{ "PRMA_BM1_RPUMP_PRES"			, _K_A_IO	,	PRMA_BM1_RPUMP_PRES		,	0	} ,	
	{ "PRMD_BM1_COOL_TIME"			, _K_D_IO	,	PRMD_BM1_COOL_TIME		,	0	} ,	
	{ "PRMA_BM2_SVENT_PRES"			, _K_A_IO	,	PRMA_BM2_SVENT_PRES		,	0	} ,
	
	{ "PRMA_BM2_FVENT_PRES"			, _K_A_IO	,	PRMA_BM2_FVENT_PRES		,	0	} ,	
	{ "PRMA_BM2_VENT_TD"			, _K_A_IO	,	PRMA_BM2_VENT_TD		,	0	} ,	
	{ "PRMA_BM2_SPUMP_PRES"			, _K_A_IO	,	PRMA_BM2_SPUMP_PRES		,	0	} ,	
	{ "PRMA_BM2_FPUMP_PRES"			, _K_A_IO	,	F_PUMP_PRESSURE_BM2		,	0	} ,	
	{ "PRMA_BM2_RPUMP_PRES"			, _K_A_IO	,	PRMA_BM2_RPUMP_PRES		,	0	} ,	
	{ "PRMD_BM2_COOL_TIME"			, _K_D_IO	,	PRMD_BM2_COOL_TIME		,	0	} ,	
	{ "EQ_NAME"						, _K_S_IO	,	EQ_NAME					,	0	} ,	
	{ "CTC_EVENT_LOG_SVR"		    , _K_F_IO	,	CTC_EVENT_LOG_SVR		,	0	} ,	 
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	ATM		,
	VACUUM	,
	GOATM	,
	GOVAC	,
	ABORT	,
} VacuumStatus;
//------------------------------------------------------------------------------------------
typedef enum {
	Idle	,
	Running	,
	Pausing	,
	Paused  ,
	Aborting,  
	Disable , 
	Waiting ,
	Wait_HO ,
} StbMainCtrl;
//------------------------------------------------------------------------------------------
typedef enum {
	SOUND_ON		,    
	SOUND_OFF		,
} Buzzer_Sound;
//------------------------------------------------------------------------------------------
typedef enum {
	OFF		,
	ON		,
	BLINK	,
} OffOnBlink;
//------------------------------------------------------------------------------------------
typedef enum
{
	UNKNOWN_SENSOR,
	ATM_SENSOR,
	VAC_SENSOR,
	ERROR_SENSOR
}ATMVAC_SENSOR_STATUS;
//------------------------------------------------------------------------------------------
enum { BM1 , BM2 , CM1 , CM2 , CM3 , AL };
enum { PORT1 , PORT2 , PORT3 };
enum { MANUAL   , AUTO };
enum { Off, On };
enum { EQ_OFFL  , ATTEMP_ONL , HOST_OFFL , LOCAL , REMOTE };
enum { Unuse, Local, DisConn, Standby, Maint, Uninit, EnablePM, CTCinUSE, CTCDis, CTCDis2, CTCEnaPM, CTCDisHW, CTCDIsHW2}; // jelee add 2007.12.06
enum { ON2, OFF2 };
enum { NOTCONN	,	CONN } ;

//------------------------------------------------------------------------------------------
#define MARS_MAX_ITEM 5

typedef struct _MARS_REPORT_ITEM_LIST
{
	char szDeviceID[20];
	char szCfgID[20];
	char szDataKey[20];
	char szOldData[20];
	int nReportIO;
	
} MARS_REPORT_ITEM_LIST;

MARS_REPORT_ITEM_LIST g_MARS_REPORT_ITEM_LIST[MARS_MAX_ITEM];

extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);	
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

void CfgLogEvent(char *szModule, char *szEventID, char *DataKey, char *szData1, char *szData2)
{
	_MLOG("CFG %s %s %s %s %s", szModule, szEventID, DataKey, szData1, szData2);
}

void FncLogEvent(char *szModule, char *szEventID, char *szStatus, char *szData)
{
	_MLOG("FNC %s %s %s %s", szModule, szEventID, szStatus, szData);
}

void MARS_CFGEvent_Report( )
{
	int CS;
	char szEQID[30], szCTCVer[20], szPM1Ver[20], szPM2Ver[20], szPM3Ver[20], szLastUpdated[20]; //, szPumpOption[20], szVentOption[20];

	READ_STRING  ( EQ_NAME , szEQID , &CS ) ;
	READ_STRING  ( IPS_VERSION , szCTCVer , &CS ) ;
	READ_STRING  ( PM1_IPS_VERSION , szPM1Ver , &CS ) ;
	READ_STRING  ( PM2_IPS_VERSION , szPM2Ver , &CS ) ;
	READ_STRING  ( PM3_IPS_VERSION , szPM3Ver , &CS ) ;
	READ_STRING  ( Ver_Build_Date , szLastUpdated , &CS ) ;

	CfgLogEvent( szEQID, "SWInfo", "Version", szCTCVer,  "");
	CfgLogEvent( "PM1", "ChamberInfo", "CIP", szPM1Ver, "" );
	CfgLogEvent( "PM2", "ChamberInfo", "CIP", szPM2Ver, "" );
	CfgLogEvent( "PM3", "ChamberInfo", "CIP", szPM3Ver, "" );
	CfgLogEvent( szEQID, "SWInfo", "Updated", szLastUpdated, "" );
}

void MARS_Initialize( )
{
	int i, CS;
	char szEQID[20];

	for(i=0; i<MARS_MAX_ITEM; i++)
	{
		READ_STRING  ( IPS_VERSION+i , g_MARS_REPORT_ITEM_LIST[i].szOldData , &CS ) ;
	}

	READ_STRING  ( EQ_NAME , szEQID , &CS ) ;

	strcpy(g_MARS_REPORT_ITEM_LIST[0].szDeviceID, szEQID);
	strcpy(g_MARS_REPORT_ITEM_LIST[0].szCfgID, "SWInfo");
	strcpy(g_MARS_REPORT_ITEM_LIST[0].szDataKey, "Version");
	g_MARS_REPORT_ITEM_LIST[0].nReportIO = IPS_VERSION;

	strcpy(g_MARS_REPORT_ITEM_LIST[1].szDeviceID, "PM1");
	strcpy(g_MARS_REPORT_ITEM_LIST[1].szCfgID, "ChamberInfo");
	strcpy(g_MARS_REPORT_ITEM_LIST[1].szDataKey, "CIP");
	g_MARS_REPORT_ITEM_LIST[1].nReportIO = PM1_IPS_VERSION;

	strcpy(g_MARS_REPORT_ITEM_LIST[2].szDeviceID, "PM2");
	strcpy(g_MARS_REPORT_ITEM_LIST[2].szCfgID, "ChamberInfo");
	strcpy(g_MARS_REPORT_ITEM_LIST[2].szDataKey, "CIP");
	g_MARS_REPORT_ITEM_LIST[2].nReportIO = PM2_IPS_VERSION;

	strcpy(g_MARS_REPORT_ITEM_LIST[3].szDeviceID, "PM3");
	strcpy(g_MARS_REPORT_ITEM_LIST[3].szCfgID, "ChamberInfo");
	strcpy(g_MARS_REPORT_ITEM_LIST[3].szDataKey, "CIP");
	g_MARS_REPORT_ITEM_LIST[3].nReportIO = PM3_IPS_VERSION;

	strcpy(g_MARS_REPORT_ITEM_LIST[4].szDeviceID, szEQID);
	strcpy(g_MARS_REPORT_ITEM_LIST[4].szCfgID, "SWInfo");
	strcpy(g_MARS_REPORT_ITEM_LIST[4].szDataKey, "Updated");
	g_MARS_REPORT_ITEM_LIST[4].nReportIO = Ver_Build_Date;

}

void MARS_Value_Change_Monitor( )
{
	int i, CS;
	char szCurChangeValue[MARS_MAX_ITEM][30];

	for(i=0; i<MARS_MAX_ITEM; i++)
	{
		READ_STRING  ( IPS_VERSION+i , szCurChangeValue[i] , &CS ) ;

		if(!STRCMP_L( g_MARS_REPORT_ITEM_LIST[i].szOldData , szCurChangeValue[i]))
		{
			strcpy(g_MARS_REPORT_ITEM_LIST[i].szOldData, szCurChangeValue[i]);
			CfgLogEvent( g_MARS_REPORT_ITEM_LIST[i].szDeviceID, g_MARS_REPORT_ITEM_LIST[i].szCfgID, g_MARS_REPORT_ITEM_LIST[i].szDataKey, g_MARS_REPORT_ITEM_LIST[i].szOldData,"" );
		}
		_sleep(500);
	}	

}

void MARS_LL_Config_Info()
{
	int i, CS;
	double dbValue;
	int    nValue;
	char szName[80] = "";
	char szData[40] = "";

	for(i=0 ; i<6 ; i++)
	{
		dbValue = READ_ANALOG(PRMA_BM1_SVENT_PRES+i, &CS);
		sprintf(szData, "%7.3f", dbValue);
		ANALOG_NAME(PRMA_BM1_SVENT_PRES+i, szName);
		CfgLogEvent( "LL1", "LL1Option", szName, szData, "" );
	}
	
	nValue = READ_DIGITAL(PRMD_BM1_COOL_TIME, &CS);
	sprintf(szData, "%d", nValue);
	DIGITAL_NAME(PRMD_BM1_COOL_TIME, szName);
	CfgLogEvent( "LL1", "LL1Option", szName, szData, "" );

	for(i=0 ; i<6 ; i++)
	{
		dbValue = READ_ANALOG(PRMA_BM2_SVENT_PRES+i, &CS);
		sprintf(szData, "%7.3f", dbValue);
		ANALOG_NAME(PRMA_BM2_SVENT_PRES+i, szName);
		CfgLogEvent( "LL2", "LL2Option", szName, szData, "" );
	}

	nValue = READ_DIGITAL(PRMD_BM2_COOL_TIME, &CS);
	sprintf(szData, "%d", nValue);
	DIGITAL_NAME(PRMD_BM2_COOL_TIME, szName);
	CfgLogEvent( "LL2", "LL2Option", szName, szData, "" );

}

void MARS_Event_Monitor ()
{
	int CS, nHour, nMin, nSec;
	BOOL bEventReported , bTimeArrival;
	char HourTimeBuffer[256] , MinTimeBuffer[256],SecTimeBuffer[256],TimeBuffer[256], CTCCurrentTime[256];

	_sleep ( 60000 ); //PM Data를 안정적으로 전송하기 위한 Delay Time (60sec)
	
	bEventReported = FALSE;
	bTimeArrival = FALSE;

	MARS_Initialize();//값 변경시 Event보낼 Data들을 초기화 시킴

	MARS_CFGEvent_Report();//CTC Reset시 Event 보냄.

	MARS_LL_Config_Info();
	
	while(TRUE)
	{	
		READ_STRING  ( CurrentTime , CTCCurrentTime , &CS ) ;
		STR_SEPERATE_CHAR(  CTCCurrentTime , ':' , HourTimeBuffer , TimeBuffer , 8 );
		STR_SEPERATE_CHAR(  TimeBuffer , ':' , MinTimeBuffer , SecTimeBuffer , 5 );	
		
		nHour = atoi( HourTimeBuffer ) ;
		nMin  = atoi( MinTimeBuffer ) ;
		nSec  = atoi( SecTimeBuffer ) ;

		if( 0 == nHour && 30 == nMin && 0 <= nSec )//0H 30MIN Event Report
			bTimeArrival = TRUE;
		else
			bTimeArrival = FALSE;

		if( 0 < nHour )	bEventReported = FALSE;	

		if ( TRUE == bTimeArrival ) 
		{
			if ( FALSE == bEventReported )
			{
				MARS_CFGEvent_Report();
				bEventReported = TRUE;	
			}			
		}

		MARS_Value_Change_Monitor();	
	
		_sleep ( 10000 );	
	}
	
	_endthread ( );	
}

void MARS_Event_Monitor2 ()
{
	int CS;
	int nControlState = 99, nValue = 0;
	int nCM1Sts, nCM2Sts, nCM3Sts;
	int nPreCM1Sts = 99, nPreCM2Sts = 99, nPreCM3Sts = 99;

	char szEQID[20];
	char szSts[20];

	_sleep ( 10000 ); 
	
	READ_STRING  ( EQ_NAME , szEQID , &CS );
	
	while(TRUE)
	{	
		// Online Status Monitoring
		do 
		{
			nValue = READ_DIGITAL(CONTROL_STATE, &CS);
			if( nValue != nControlState )
			{
				if      ( nValue == EQ_OFFL ) FncLogEvent(szEQID, "RemoteStatus" ,"HostOffline" ,"" );
				else if ( nValue == LOCAL )   FncLogEvent(szEQID, "RemoteStatus" ,"HostLocal"   ,"" );
				else if ( nValue == REMOTE )  FncLogEvent(szEQID, "RemoteStatus" ,"HostRemote"  ,"" );
			}
			nControlState = nValue;
		} while (0);

		// Foup Sensor Monitoring
		do 
		{
			nCM1Sts = READ_DIGITAL(CM1_CASS_SenSor_STATUS, &CS);
			if(nCM1Sts != nPreCM1Sts)
			{
				if(nCM1Sts == 0) strcpy(szSts, "Absent");
				else		     strcpy(szSts, "Present");
				FncLogEvent("CM1", "FoupSensor" , szSts ,"" );
			}
			nPreCM1Sts = nCM1Sts;

			nCM2Sts = READ_DIGITAL(CM2_CASS_SenSor_STATUS, &CS);
			if(nCM2Sts != nPreCM2Sts)
			{
				if(nCM2Sts == 0) strcpy(szSts, "Absent");
				else		     strcpy(szSts, "Present");
				FncLogEvent("CM2", "FoupSensor" , szSts ,"" );
			}
			nPreCM2Sts = nCM2Sts;

			nCM3Sts = READ_DIGITAL(CM3_CASS_SenSor_STATUS, &CS);
			if(nCM3Sts != nPreCM3Sts)
			{
				if(nCM3Sts == 0) strcpy(szSts, "Absent");
				else		     strcpy(szSts, "Present");
				FncLogEvent("CM3", "FoupSensor" , szSts ,"" );
			}
			nPreCM3Sts = nCM3Sts;

		} while (0);

		_sleep ( 1000 );	
	}
	
	_endthread ();	
}

//------------------------------------------------------------------------------------------
//==========================================================================================
extern "C"
{
//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {

	_sleep( 3000 );
	_beginthread( (_THREAD_FUNC)MARS_Event_Monitor  , 0 , NULL );	
	_sleep( 100 );
	_beginthread( (_THREAD_FUNC)MARS_Event_Monitor2   , 0 , NULL );
	
	return SYS_SUCCESS;
}


} // End of [extern "C"]
//==========================================================================================

