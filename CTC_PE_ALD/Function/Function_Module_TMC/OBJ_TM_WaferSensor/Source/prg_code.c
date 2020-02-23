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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		ALARM_ABS_PIC			6401
#define		ALARM_PRS_PIC			6402
#define		ALARM_ABS_PLC			6403
#define		ALARM_PRS_PLC			6404
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { ABSENT , PRESENT };
enum { PICK , PLACE };
enum { None_Sns = -1, A_WaferSns, B_WaferSns};

enum { eArm_DUAL, eArm_LEFT, eArm_RIGHT };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	//============================================================================
	{ "FC_TM_WFRSNS_CONTROL"	, _K_D_IO	,	CONTROL				,	0	} ,

	{ "TM.Wafer_Result"			, _K_D_IO	,	TM_Wafer_Result		,	0	} ,

//	{ "MTR7.Load_Status"		, _K_D_IO	,	A_WaferSns		,	0	} ,
//	{ "MTR7.Load_StatusB"		, _K_D_IO	,	B_WaferSns		,	0	} ,

	{ "ULVAC_Load_Status_A"		, _K_D_IO	,	AL_WaferSns		,	0	} ,
	{ "ULVAC_Load_Status_B"		, _K_D_IO	,	AR_WaferSns		,	0	} ,
	{ "ULVAC_Load_Status_C"		, _K_D_IO	,	BL_WaferSns		,	0	} ,
	{ "ULVAC_Load_Status_D"		, _K_D_IO	,	BR_WaferSns		,	0	} ,	
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_TRUE(int io , int *CommStatus) {
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

//////////////////////////////////////////////////////////////////////////
Module_Status SensorCheck( BOOL Check , int LSensor , int RSensor, int Target , int Mode, int ArmMode) {
	int Alarm , ck , os;
	int CommStatus , AlmResult;
	int Sense_Read , LoadSts, LoadSts2;

	printf("[DEBUG] SensorCheck Chk[%d] LSns[%d] RSns[%d] Tgt[%d] Mode[%d] ArmMode[%d]\n",
		Check, LSensor, RSensor, Target, Mode, ArmMode);

	ck = 0;
	while (1) 
	{
		//////////////////////////////////////////////////////////////////////////
		//... Read Sensor IO
		LoadSts  = READ_DIGITAL_TRUE(LSensor, &CommStatus);
		LoadSts2 = READ_DIGITAL_TRUE(RSensor, &CommStatus);

		// A&B, C&D Arm Status Merge by hyuk 2013.03.13
		if(eArm_LEFT == ArmMode)
		{
			if(PRESENT == LoadSts)								Sense_Read = PRESENT;
			else												Sense_Read = ABSENT;
		}
		else if(eArm_RIGHT == ArmMode)
		{
			if(PRESENT == LoadSts2)								Sense_Read = PRESENT;
			else												Sense_Read = ABSENT;
		}
		else
		{	//... eArm_DUAL == ArmMode
			if(PRESENT == LoadSts && PRESENT == LoadSts2)		Sense_Read = PRESENT;
			else												Sense_Read = ABSENT;
		}

		// A&B, C&D Arm Status Merge by hyuk 2013.03.13

		//////////////////////////////////////////////////////////////////////////
		//...
		if ( !Check ) {
			if ( ck == 0 ) {
				os  = Sense_Read;
				ck++;
			}
			else {
				if ( ( ck == 3 ) && ( os == Sense_Read ) ) {
					if   ( Sense_Read == ABSENT  )	WRITE_DIGITAL( TM_Wafer_Result , 0 , &CommStatus );
					else							WRITE_DIGITAL( TM_Wafer_Result , 1 , &CommStatus );
					return SYS_SUCCESS;
				}
				if ( os != Sense_Read ) {
					ck = 0;
					_sleep(100);
				}
				else {
					ck++;
				}
			}
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check Target & Sensor Status
		if      ( Target == ABSENT  && Sense_Read == ABSENT  )		return SYS_SUCCESS;
		else if ( Target == PRESENT && Sense_Read == PRESENT )		return SYS_SUCCESS;

		//...
		switch ( Mode ) {
			case PICK :
						if(Target == ABSENT)	Alarm = ALARM_ABS_PIC;
						else                    Alarm = ALARM_PRS_PIC;
						break;
			case PLACE :
						if(Target == ABSENT)	Alarm = ALARM_ABS_PLC;
						else                    Alarm = ALARM_PRS_PLC;
						break;
		}

		//...
		if(Sense_Read == ABSENT)	WRITE_DIGITAL(TM_Wafer_Result, 0, &CommStatus);
		else						WRITE_DIGITAL(TM_Wafer_Result, 1, &CommStatus);
		
		AlmResult = ALARM_MANAGE( Alarm );
		
		if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
		else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
	}

	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	int	i , Sensor, LeftSensor, RightSensor;
	BOOL Check;
	char SensorStr[64] , RunStr[64] , /*ElseStr[64] ,*/ ElseStr2[64] , ElseStr3[64] , ElseStr4[64] , ElseStr5[64] , TrStr[64] , SINGLE[64];

	Module_Status msRet = SYS_SUCCESS;
	int	 nArmMode = eArm_DUAL;

	//.....
	if(READ_DIGITAL(CONTROL, &i) == 1)		return SYS_SUCCESS;

	printf("[INFO] TM Wafer Sensor check [%s]\n", PROGRAM_PARAMETER_READ());

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , TrStr , 63 ); // Mode
	STR_SEPERATE( TrStr    , SINGLE    , ElseStr2 , 63 ); // Single Transfer
	STR_SEPERATE( ElseStr2 , SensorStr , ElseStr3 , 63 ); // Chamber
	STR_SEPERATE( ElseStr3 , TrStr     , ElseStr4 , 63 ); // Slot (Not Used)
	STR_SEPERATE( ElseStr4 , TrStr     , ElseStr5 , 63 ); // TRANSFER

	LeftSensor = RightSensor = None_Sns;

	//.....
	if     (STRCMP_L(SINGLE, "SINGLE"))		nArmMode = eArm_LEFT;
	else if(STRCMP_L(SINGLE, "SINGLE2"))	nArmMode = eArm_RIGHT;
	
	//
	_LOG("-----> %s %s %s WaferSense Started in TM WaferSense Func", RunStr, SensorStr, TrStr);
	
	//.....
	if ( STRCMP_L( TrStr , "TRANSFER2" ) )	Check = FALSE;
	else									Check = TRUE;
	
	//.....
	if      ( STRCMP_L( SensorStr , "A_CM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_CM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM3" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM4" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM5" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM6" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM7" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM8" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_PM9" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_CM1" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_CM2" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM1" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM2" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM3" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM4" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM5" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM6" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM7" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM8" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_PM9" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_CM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_CM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM3" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM4" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM5" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM6" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM7" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM8" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_PM9" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_CM1" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_CM2" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM1" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM2" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM3" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM4" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM5" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM6" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM7" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM8" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_PM9" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "CM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "CM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM3" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM4" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM5" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM6" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM7" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM8" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "PM9" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "C1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "C2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P3" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P4" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P5" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P6" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P7" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P8" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "P9" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_BM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "A_BM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_BM1" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "B_BM2" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_BM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "S_BM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_BM1" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "T_BM2" ) ) Sensor = B_WaferSns;
	else if ( STRCMP_L( SensorStr , "BM1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "BM2" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "B1" ) ) Sensor = A_WaferSns;
	else if ( STRCMP_L( SensorStr , "B2" ) ) Sensor = A_WaferSns;
	else {
		printf( "---> Sensor Error - Not Support Sensor %s\n" , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}

	// 2014.03.13 All Arm Status Check
	if(Sensor == A_WaferSns) 
	{	//... Upper Arm
		LeftSensor  = AL_WaferSns;
		RightSensor = AR_WaferSns;
	}
	else if(Sensor == B_WaferSns) 
	{	//... Lower Arm
		LeftSensor  = BL_WaferSns;
		RightSensor = BR_WaferSns;
	}

	if ( SIMULATION_MODE ) return SYS_SUCCESS;

	//.....
	if     (STRCMP_L(RunStr, "ABSENT_PICK"))		msRet = SensorCheck(Check, LeftSensor, RightSensor, ABSENT,  PICK,	nArmMode);
	else if(STRCMP_L(RunStr, "ABSENT_PLACE"))		msRet = SensorCheck(Check, LeftSensor, RightSensor, ABSENT,  PLACE, nArmMode);
	else if(STRCMP_L(RunStr, "PRESENT_PICK"))		msRet = SensorCheck(Check, LeftSensor, RightSensor, PRESENT, PICK,	nArmMode);
	else if(STRCMP_L(RunStr, "PRESENT_PLACE"))		msRet = SensorCheck(Check, LeftSensor, RightSensor, PRESENT, PLACE,	nArmMode);
	else 
	{
		printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}

	_LOG("-----> %s %s %s WaferSense Finished in TM WaferSense Func", RunStr, SensorStr, TrStr);
	
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	return TRUE;
}
