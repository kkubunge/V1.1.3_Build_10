#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "TextParser_C.h"
#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Alarm Definition(2400 ~ 2499)

//#define		ALARM_MKS_OPEN	    2401
//#define		ALARM_MKS_CLOSE     2402
#define		ALARM_MKS_COMMERROR	     2400
#define		ALARM_MKS_POS_TIMEOUT	 2404
#define		ALARM_MKS_PRS_TIMEOUT	 2405
//#define		ALARM_MKS_PRESS_WARNING	 2406
#define		ALARM_MKS_PRS_OUT_OF_TOL	2407

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#define		DELAY					4
#define		PRESSURE_STABLE_TIME	2

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
struct tm		Pres_Time;
char Date[256];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_ModuleFailDM",	 _K_D_IO	,	SW_ModuleFailDM,	0	} ,
//	{ "SIMULATION"			, _K_D_IO	,	SIMULATION			    ,	0	} ,
	{ "FC_PRESSURE_CONTROL"	, _K_D_IO	,	CONTROL_CHECK		    ,	0	} ,
	{ "M651_CommStatus"		, _K_D_IO	,	MKS_CommStatus		    ,	0	} ,
	{ "M651_Valve"			, _K_D_IO	,	MKS_Control			    ,	0	} , //  Stop/Run   Open   Close
	{ "M651_Pressure"		, _K_A_IO	,	MKS_Pressure		    ,	0	} ,
	{ "M651_Position"		, _K_A_IO	,	MKS_Position		    ,	0	} ,
	{ "M651_Set_Position"	, _K_A_IO	,	MKS_Set_Position        ,	0	} ,
	{ "M651_Set_Pressure"	, _K_A_IO	,	MKS_Set_Pressure    	,	0	} ,
//	{ "PRESS_PRESS_TIMEOUT"	, _K_D_IO	,	PRESSURE_TIMEOUT    	,	0	} ,
//	{ "PRESS_STABLE_TIME"	, _K_D_IO	,	PRESSURE_STABLE_TIME	,	0	} ,

	{ "PRMA_TL_ThrtlPos"	, _K_A_IO	,	PRMA_TL_ThrtlPos		,   0	} ,
	{ "PRMA_TL_ThrtlPrss"	, _K_A_IO	,	PRMA_TL_ThrtlPrss		,   0	} ,
	{ "PRMA_TO_ThrtlPos"	, _K_A_IO	,	PRMA_TO_ThrtlPos		,   0	} ,
	{ "PRMA_TO_ThrtlPrss"	, _K_A_IO	,	PRMA_TO_ThrtlPrss		,   0	} ,

	{ "M651_ModeDM"			, _K_D_IO	,	M651_ModeDM		,   0	} ,


//	{ "PRESSURE_ERROR_LIMIT"	, _K_D_IO	,	PRESSURE_ERROR_LIMIT	,	0	} ,
//	{ "PRESSURE_WARNING_LIMIT"	, _K_D_IO	,	PRESSURE_WARNING_LIMIT	,	0	} ,


//	{ "PRESS_STABLE_PERCENT"    , _K_D_IO	,	PRESS_STABLE_PERCENT    ,	0	} ,
//	{ "PRESS_PRESS.R"		    , _K_A_IO	,	PRESS_BUFFER		    ,	0	} ,
//	{ "PRESS_POSIT.R"		    , _K_A_IO	,	POSIT_BUFFER		    ,	0	} ,
//---20010710 constant time delay
//	{ "TIMELIMITCONST_TIME"		, _K_A_IO	,	TIMELIMITCONST_TIME	    ,	0	} ,
//	{ "TIMELIMITCONST"			, _K_D_IO	,	TIMELIMITCONST		    ,	0	} ,
//--BN2 IO
//	{ "AO0_2_BN2"		        , _K_A_IO	, 	Gas3_Set	            ,	0	} ,
//	{ "AI0_2_BN2"		        , _K_A_IO	, 	Gas3_Flow	            ,	0	} ,
//	{ "DO1A_3_BN2On"	        , _K_D_IO	, 	Gas3_In 	            ,	0	} ,

//level 4 	{ "VACUUM_CONTROL"			, _K_F_IO	,	OBJ_VACUUM_CONTROL	,	0	} ,
	""
};

//------------------------------------------------------------------------------------------
// Alarm(Hazard) Option Function
// return TRUE : Sequence Is Continuous
// return FALSE : Alarm Is Valid And Sequence Aborted

int g_nAlarmOption = 1;		// 0:Ignore 1:Warning 2:ChamberDisable 3:Abort

BOOL CheckAlarmOption(int nAlarm, int nOption)
{
	BOOL bRet = TRUE;
	int nIOStatus;
	switch(nOption)
	{
	case 0: break;
	case 1:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		break;
	case 2:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		WRITE_DIGITAL(SW_ModuleFailDM, 1, &nIOStatus);	// 0:Off 1:On
		break;
	case 3:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		bRet = FALSE;
		break;
	default: break;
	}
	return bRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MKS651Open() {
	int CommStatus;

	printf( "Start MKS Open\n" );
	WRITE_DIGITAL( M651_ModeDM, 0, &CommStatus); // 0:Position 1:Pressure
	WRITE_DIGITAL( MKS_Control , 1 , &CommStatus );
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MKS651Close() {
	int CommStatus;

	printf( "Start MKS Close\n" );
	WRITE_DIGITAL( M651_ModeDM, 0, &CommStatus); // 0:Position 1:Pressure
	WRITE_DIGITAL( MKS_Control , 2 , &CommStatus );
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2005.03.23 by cskim

Module_Status MKS651Position_P( int SetOnlyMode , double dbSetData)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	double dbTol;
	int nTimeoutT;
	double dbRead;
	int nErrCnt = 0;
	double dbLowLmt, dbHighLmt;
	int nAlarm;
	unsigned int nTimeTick;
 
	dbTol =  READ_ANALOG(PRMA_TL_ThrtlPos, &nIOStatus);
	nTimeoutT = (int) READ_ANALOG(PRMA_TO_ThrtlPos, &nIOStatus);

	_LOG("-----> Mks Pos Setting Started");
	WRITE_DIGITAL(M651_ModeDM, 0, &nIOStatus); // 0:Position 1:Pressure
	WRITE_ANALOG( MKS_Set_Position , dbSetData , &nIOStatus );
	_LOG("-----> Mks Pos Setting Finished");

	if ( SetOnlyMode ) return SYS_SUCCESS;
	if ( g_nAlarmOption == 0 ) return SYS_SUCCESS;

	_LOG("-----> Mks Pos Monitoring Started");
	dbLowLmt = dbSetData - dbTol;
	dbHighLmt = dbSetData + dbTol;

	nErrCnt = 0;
	nTimeTick = 0;
	while(1)
	{
		if ( !WAIT_SECONDS( 1 ) ) { msRet = SYS_ABORTED; break; }
		if( (nTimeTick % 2 ) == 0 )	dbRead = READ_ANALOG( MKS_Position , &nIOStatus );
		nTimeTick++;
		if(dbRead<dbLowLmt || dbRead>dbHighLmt)
		{
			nErrCnt++;
			if(nErrCnt > nTimeoutT)
			{
				nAlarm = ALARM_MKS_POS_TIMEOUT;
				if(FALSE == CheckAlarmOption(nAlarm, g_nAlarmOption))
				{
					_LOG("-----> Mks Monitoring Failed");
					msRet = SYS_ABORTED;
					break;
				}
			}
		}
		else nErrCnt = 0;
	}
	return msRet;
}


Module_Status MKS651Pressure2_P( double dbSetData )
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	double dbTol;
	int nTimeoutT;
	double dbRead;
	int nErrCnt = 0;
	double dbLowLmt, dbHighLmt;
	int nAlarm;
	unsigned int nTimeTick;

	dbTol =  READ_ANALOG(PRMA_TL_ThrtlPrss, &nIOStatus);
	nTimeoutT = (int) READ_ANALOG(PRMA_TO_ThrtlPrss, &nIOStatus);

	if ( dbSetData <= 0 ) return (MKS651Open() );
	_LOG("-----> Mks Pres Setting Started");
	WRITE_DIGITAL(M651_ModeDM, 1, &nIOStatus); // 0:Position 1:Pressure
	WRITE_ANALOG( MKS_Set_Pressure , dbSetData , &nIOStatus ); //*10 from t0 5
	_LOG("-----> Mks Pres Setting Finished");

	if ( g_nAlarmOption == 0 ) return SYS_SUCCESS;
	_LOG("-----> Mks Pres Monitoring Started");
	nErrCnt = 0;
	nTimeTick = 0;
	while(1)
	{
		if ( !WAIT_SECONDS( 1 ) ) { msRet = SYS_ABORTED; break; }
		if((nTimeTick % 2) == 0) dbRead = READ_ANALOG( MKS_Pressure , &nIOStatus );
		nTimeTick++;
		dbLowLmt = dbSetData - dbTol;
		dbHighLmt = dbSetData + dbTol;
		if(dbRead<dbLowLmt || dbRead>dbHighLmt)
		{
			nErrCnt++;
			if(nErrCnt > nTimeoutT)
			{
				nAlarm = ALARM_MKS_PRS_OUT_OF_TOL;
				if(FALSE == CheckAlarmOption(nAlarm, g_nAlarmOption))
				{
					_LOG("-----> Mks Monitoring Failed");
					msRet = SYS_ABORTED;
					break;
				}
			}
		}
		else nErrCnt = 0;
	}
	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MKS651PressureForProcess( double SetData, double dbTol, int nTimeoutT ) {
	int	Timer = 0;
	int	StableChk = 0;
	int StableTime , TimeOut;
	int CommStatus;
	// int AlmResult;
	double ReadBuffer , HighLim , LowLim;
	int i;
	int nAlarm;

	printf( "Start MKS Pressure\n" );

	if ( SetData <= 0 ) return( MKS651Close() );

	_LOG("-----> Mks Pres Setting Started");
	WRITE_DIGITAL(M651_ModeDM, 1, &CommStatus); // 0:Position 1:Pressure
	// WRITE_ANALOG( MKS_Set_Pressure , SetData * 10 , &CommStatus );//*10 from t0 5
	// WRITE_ANALOG( MKS_Set_Pressure , SetData * 10 , &CommStatus );
	WRITE_ANALOG( MKS_Set_Pressure , SetData * 10 , &CommStatus );
	_LOG("-----> Mks Pres Setting Finished");

//----SetOnly Mode-----------------------------------------------------------------------------
//	if ( SetOnlyMode ){
//		printf( "End of MKS Pressure No checking\n" );
//		return SYS_SUCCESS;}
//-------------------------------------------------------------------------------------------------------
//	ReadBuffer	= SetData * ( (double) ( READ_DIGITAL( PRESS_STABLE_PERCENT , &CommStatus ) ) ) / 100.0;
	ReadBuffer	= dbTol;
	HighLim		= SetData + ReadBuffer;
	LowLim		= SetData - ReadBuffer;
// 	TimeOut		= READ_DIGITAL( PRESSURE_TIMEOUT , &CommStatus );
	TimeOut		= nTimeoutT;
//	StableTime	= READ_DIGITAL( PRESSURE_STABLE_TIME  , &CommStatus );
	StableTime  = PRESSURE_STABLE_TIME;
	Timer		= 0;
	StableChk	= 0;

	_LOG("-----> Mks Pres Monitoring Started");
	while(1) {
		ReadBuffer = READ_ANALOG_AVR( MKS_Pressure , &CommStatus );
		if ( ReadBuffer <= HighLim &&  ReadBuffer >= LowLim ) StableChk++;
		if ( StableChk >= StableTime ) break;
		if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
		if ( Timer > TimeOut ) {
			Timer		= 0;
			StableChk	= 0;

			nAlarm = ALARM_MKS_PRS_TIMEOUT;
			if(FALSE == CheckAlarmOption(nAlarm, g_nAlarmOption))
			{
				return SYS_ABORTED;
			}
			/*
			AlmResult = ALARM_MANAGE( ALARM_MKS_PRS_TIMEOUT );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			else                                break;
			*/
		}
		else Timer++;
	}
//-----------------------------------------------------------------------------------
//----PREPARE_CCONST Waiting routine-------------------------------------------------
	if(READ_DIGITAL( TIMELIMITCONST , &CommStatus )==1)
	{
		int timer=(int)READ_ANALOG( TIMELIMITCONST_TIME , &CommStatus );
			for(i=0;i<timer;i++)
				{
					if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
					if(READ_DIGITAL( TIMELIMITCONST , &CommStatus )==0)break;
					if(i > READ_ANALOG( TIMELIMITCONST_TIME , &CommStatus ))
						{
							//pump tp base;OBJ_VACUUM_CONTROL
//							sprintf( Send_Value , "Pumping");	
//							WRITE_STRING(Process_Message,Send_Value,&CommStatus);
							//RUN_FUNCTION( OBJ_VACUUM_CONTROL ,"PUMP");
							MKS651Open();
//							WRITE_ANALOG( Gas3_Set , 0 , &CommStatus );
							if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
//							WRITE_DIGITAL( Gas3_In , 0 , &CommStatus );
						    break;
						}
					timer++;
				}
	}
//-----------------------------------------------------------------------------------
	_LOG("-----> Mks Pres Monitoring Finished");
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status MKS651Pressure2( double SetData, double dbTol, int nTimeoutT ) {
	int	Timer = 0;
	int	StableChk = 0;
	int CommStatus;
	double ReadBuffer , HighLim , LowLim;
	double HighLim2 , LowLim2;
	int nAlarm;
//	int i;

	_getsystime( &Pres_Time );
	sprintf( Date, "%04d:%02d:%02d:%02d:%02d:%02d" , Pres_Time.tm_year+1900 , Pres_Time.tm_mon + 1 , Pres_Time.tm_mday , Pres_Time.tm_hour , Pres_Time.tm_min  , Pres_Time.tm_sec );
	printf( "%s => Pressure Control Pressure(Object) => Setpoint : %f\n" , Date , SetData );

	if ( SetData <= 0 ) return (MKS651Close() );

	_LOG("-----> Mks Pres Setting Started");
	WRITE_DIGITAL(M651_ModeDM, 1, &CommStatus); // 0:Position 1:Pressure
//	WRITE_ANALOG( MKS_Set_Pressure , SetData * 10 , &CommStatus );//*10 from t0 5
	WRITE_ANALOG( MKS_Set_Pressure , SetData , &CommStatus ); //*10 from t0 5
	_LOG("-----> Mks Pres Setting Finished");

	if ( !READ_DIGITAL( CONTROL_CHECK , &CommStatus ) ) {
		if ( !WAIT_SECONDS( DELAY ) ) return SYS_ABORTED;
		return SYS_SUCCESS;
	}

	ReadBuffer = READ_ANALOG( MKS_Pressure , &CommStatus );
	if ( !CommStatus ) {
		printf("-----> CommStatus is invalid in PressureControl\n");
		ALARM_POST( ALARM_MKS_COMMERROR );
		return SYS_ABORTED;
	}

//	ReadBuffer	= SetData * ( (double) ( READ_DIGITAL( PRESSURE_ERROR_LIMIT , &CommStatus ) ) ) / 100.0;
	ReadBuffer	= dbTol;
	HighLim		= SetData + ReadBuffer;
	LowLim		= SetData - ReadBuffer;

//	ReadBuffer	= SetData * ( (double) ( READ_DIGITAL( PRESSURE_WARNING_LIMIT , &CommStatus ) ) ) / 100.0;
	HighLim2	= SetData + ReadBuffer;
	LowLim2		= SetData - ReadBuffer;

	Timer		= 0;
	StableChk	= 0;

	_LOG("-----> Mks Pres Monitoring Started");
	while(1) {
		ReadBuffer = READ_ANALOG( MKS_Pressure , &CommStatus );
		if ( ReadBuffer <= HighLim &&  ReadBuffer >= LowLim ) StableChk++;
		else {
			StableChk--;
			if ( StableChk < 0 ) StableChk = 0;
		}
//		if ( (StableChk/2) >= READ_DIGITAL( PRESSURE_STABLE_TIME  , &CommStatus ) ) break;
		if ( (StableChk/2) >= PRESSURE_STABLE_TIME) break;
		if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
//		if ( (Timer/2) > READ_DIGITAL( PRESSURE_TIMEOUT , &CommStatus ) ) {
		if ( (Timer/2) > nTimeoutT) {
			return SYS_ERROR;
		}
		Timer++;
	}

	Timer		= 0;
	StableChk	= 0;
	while(1) {
		ReadBuffer = READ_ANALOG( MKS_Pressure , &CommStatus );
		if ( ReadBuffer > HighLim || ReadBuffer < LowLim ) StableChk++;
		else {
			StableChk--;
			if ( StableChk < 0 ) StableChk = 0;
		}
		if ( ReadBuffer > HighLim2 || ReadBuffer < LowLim2 )
		{
			nAlarm = ALARM_MKS_PRS_OUT_OF_TOL;
			if(FALSE == CheckAlarmOption(nAlarm, g_nAlarmOption))
			{
				return SYS_ABORTED;
			}
		}
		if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
		if ( (StableChk/2) >= READ_DIGITAL( PRESSURE_STABLE_TIME , &CommStatus ) ) {
			return SYS_ERROR;
		}
		Timer++;
	}
	_LOG("-----> Mks Pres Monitoring Finished");
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//for constant Pressure in Procee recipe 20010710 changwoochen
Module_Status MKS651Pressure( int SetOnlyMode , double SetData, double dbTol, int nTimeoutT) {
	int	Timer = 0;
	int	StableChk = 0;
	int StableTime , TimeOut;
	int CommStatus;
//	int AlmResult;
	double ReadBuffer , HighLim , LowLim;
	int nAlarm;

	printf( "Start MKS Pressure\n" );

	if ( SetData <= 0 ) return( MKS651Close() );

	_LOG("-----> Mks Pres Setting Started");
	WRITE_DIGITAL(M651_ModeDM, 1, &CommStatus); // 0:Position 1:Pressure
//	WRITE_ANALOG( MKS_Set_Pressure , SetData * 10 , &CommStatus );//*10 from t0 5
	WRITE_ANALOG( MKS_Set_Pressure , SetData , &CommStatus );
	_LOG("-----> Mks Pres Setting Finished");

	if ( SetOnlyMode ){
		printf( "End of MKS Pressure No checking\n" );
		return SYS_SUCCESS;}

//	ReadBuffer	= SetData * ( (double) ( READ_DIGITAL( PRESS_STABLE_PERCENT , &CommStatus ) ) ) / 100.0;
	ReadBuffer	= dbTol;
	HighLim		= SetData + ReadBuffer;
	LowLim		= SetData - ReadBuffer;
//	TimeOut		= READ_DIGITAL( PRESSURE_TIMEOUT , &CommStatus );
	TimeOut		= nTimeoutT;
//	StableTime	= READ_DIGITAL( PRESSURE_STABLE_TIME  , &CommStatus );
	StableTime	= PRESSURE_STABLE_TIME;
	Timer		= 0;
	StableChk	= 0;

	_LOG("-----> Mks Pres Monitoring Started");
	while(1) {
		ReadBuffer = READ_ANALOG_AVR( MKS_Pressure , &CommStatus );
		if ( ReadBuffer <= HighLim &&  ReadBuffer >= LowLim ) StableChk++;
		// if ( StableChk >= StableTime ) break;
		if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
		if ( Timer > TimeOut ) {

			nAlarm = ALARM_MKS_PRS_TIMEOUT;
			if(FALSE == CheckAlarmOption(nAlarm, g_nAlarmOption))
			{
				return SYS_ABORTED;
			}
			Timer = 0;

			//	return SYS_ERROR;
			/*
			Timer		= 0;
			StableChk	= 0;
			AlmResult = ALARM_MANAGE( ALARM_MKS_PRS_TIMEOUT );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			else                                break;
			*/
		}
		else Timer++;
	}
	_LOG("-----> Mks Pres Monitoring Finished");
	
return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MKS651Position( int SetOnlyMode , double SetData, double dbTol, int nTimeoutT) {
	int	Timer = 0;
	int TimeOut;
	int CommStatus;
	// int AlmResult;
	double ReadBuffer , HighLim , LowLim;
	int nAlarm;

	printf( "Start MKS Position\n" );

	if ( SetData <= 0 ) return( MKS651Close() );

	_LOG("-----> Mks Pos Setting Started");
	WRITE_DIGITAL(M651_ModeDM, 0, &CommStatus); // 0:Position 1:Pressure
	WRITE_ANALOG( MKS_Set_Position , SetData , &CommStatus );
	_LOG("-----> Mks Pos Setting Finished");

	if ( SetOnlyMode ) return SYS_SUCCESS;

//	ReadBuffer	= SetData * ( (double) ( READ_DIGITAL( PRESS_STABLE_PERCENT , &CommStatus ) ) ) / 100.0;
	ReadBuffer	= dbTol;
	HighLim		= SetData + ReadBuffer;
	LowLim		= SetData - ReadBuffer;
//	TimeOut		= READ_DIGITAL( PRESSURE_TIMEOUT , &CommStatus );
	TimeOut		= nTimeoutT;
	Timer		= 0;

	_LOG("-----> Mks Pos Monitoring Started");
	while(1) {
		ReadBuffer = READ_ANALOG_AVR( MKS_Position , &CommStatus );
		if ( ReadBuffer <= HighLim &&  ReadBuffer >= LowLim ) break;
		if ( !WAIT_SECONDS( 1 ) ) return SYS_ABORTED;
		if ( Timer > TimeOut ) {
			Timer		= 0;

			nAlarm = ALARM_MKS_POS_TIMEOUT;
			if(FALSE == CheckAlarmOption(nAlarm, g_nAlarmOption))
			{
				return SYS_ABORTED;
			}

			/*
			AlmResult = ALARM_MANAGE( ALARM_MKS_POS_TIMEOUT );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			else                                break;
			*/
		}
		else Timer++;
	}
	_LOG("-----> Mks Pos Monitoring Finished");
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MKS651Init() {

	printf( "Start MKS Init\n" );
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//
//
Module_Status Program_Main() {
	int	Dl;
	unsigned char *Ed;
	char ParamStr[100] , RunStr[100];
	double ReadData, dbTol;
//	int CommStatus;
	float fValue;
	int nTimeout;
	int nAlarmOption;

/*
	if ( READ_DIGITAL( SIMULATION , &CommStatus ) == 1 ) {
		printf( "[SIM] Pressure Control\n" );
		return SYS_SUCCESS;
	}
*/
	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( Dl < 1 ) {
		printf( "--- Pressure Control Parameter Error\n" );
		return SYS_ABORTED;
	}

	STR_SEPERATE( Ed , RunStr , ParamStr , 20 );
//	ReadData = atof( ParamStr );
	tpSetText(ParamStr);

	tpGetFloat(&fValue, 0);
	ReadData = fValue;

	tpGetFloat(&fValue, 0);
	dbTol = fValue;

	tpGetInt(&nTimeout, 0);

	if(tpGetInt(&nAlarmOption, 0)) g_nAlarmOption = nAlarmOption;
	else g_nAlarmOption = 1;

	if      ( STRCMP_L( RunStr , "INIT"      ) ) return( MKS651Init() );
	else if ( STRCMP_L( RunStr , "OPEN"      ) ) return( MKS651Open() );
	else if ( STRCMP_L( RunStr , "CLOSE"     ) ) return( MKS651Close() );
	else if ( STRCMP_L( RunStr , "POSITION"  ) ) return( MKS651Position( FALSE , ReadData, dbTol, nTimeout ) );
	else if ( STRCMP_L( RunStr , "POSITION_P"  ) ) return( MKS651Position_P( FALSE , ReadData) );
	else if ( STRCMP_L( RunStr , "PRESSURE"  ) ) return( MKS651Pressure( TRUE , ReadData, dbTol, nTimeout ) );
	else if ( STRCMP_L( RunStr , "PRESSPROC"  ) ) return( MKS651PressureForProcess(ReadData, dbTol, nTimeout ) );
	else if ( STRCMP_L( RunStr , "PRESSCHK"  ) ) return( MKS651Pressure2(ReadData, dbTol, nTimeout ) );
	else if ( STRCMP_L( RunStr , "PRESSCHK_P"  ) ) return( MKS651Pressure2_P(ReadData) );
//	else if ( STRCMP_L( RunStr , "POSIT_C"   ) ) return( MKS651Position( TRUE , READ_ANALOG( POSIT_BUFFER , &CommStatus ) ) );
//	else if ( STRCMP_L( RunStr , "PRESS_C"   ) ) return( MKS651Pressure( TRUE , READ_ANALOG( PRESS_BUFFER , &CommStatus ) ) );
	else {
		printf( "---> Check Error - Not Support Code %s\n" , Ed );
		return SYS_ABORTED;
	}
}
//
//
//
//
void  Event_Message_Received() {
}
//