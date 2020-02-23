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
#define		ALARM_ABS_PIC			4061
#define		ALARM_PRS_PIC			4062
#define		ALARM_ABS_PLC			4063
#define		ALARM_PRS_PLC			4064
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		SENSOR_RETRY_CNT		3

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { ABSENT , PRESENT };
enum { PICK , PLACE };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	//============================================================================
	{ "FC_FM_WFRSNS_CONTROL"	, _K_D_IO	,	CONTROL				,	0	} ,

	{ "FM.Wafer_Result"			, _K_D_IO	,	FM_Wafer_Result		,	0	} ,
	{ "FM.ATR8_Vacuum"			, _K_D_IO	,	Vacuum_Sensor		,	0	} ,
	{ "FM.ATR8_VacuumB"			, _K_D_IO	,	Vacuum_SensorB		,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_TRUE( int io , int *CommStatus ) {
	int Data;
	Data = READ_DIGITAL( io , CommStatus );
	if ( !(*CommStatus) ) {
		_sleep(500);
		Data = READ_DIGITAL( io , CommStatus );
		if ( !(*CommStatus) ) {
			_sleep(500);
			Data = READ_DIGITAL( io , CommStatus );
			if ( !(*CommStatus) ) {
				_sleep(500);
				Data = READ_DIGITAL( io , CommStatus );
				if ( !(*CommStatus) ) {
					_sleep(500);
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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status SensorCheck( BOOL Check , int nSensorDI,int Target , int Mode ) {
	int Alarm , ck , os;
	int CommStatus , AlmResult;
	int Sense_Read;
	int nRetry = 0;

	ck = 0;
	while (1) {
		Sense_Read = READ_DIGITAL_TRUE(nSensorDI , &CommStatus);
		printf( "===> ATM Robot(SensorCheck) : ABSENT(0),PRESENT(1) => [%d]\n" , Sense_Read);
		printf( "===> ATM Robot(SensorCheck) : Target[%d]\n" , Target );
		printf( "===> ATM Robot(SensorCheck) : Mode[%d], Check[%d]\n" , Mode, Check );
		if (!Check) {
			if (ck == 0) {
				os = Sense_Read;
				ck++;
			} else {
				if ((ck == 3) && (os == Sense_Read)) {
					if   (Sense_Read == ABSENT)	WRITE_DIGITAL(FM_Wafer_Result , 0 , &CommStatus);
					else						WRITE_DIGITAL(FM_Wafer_Result , 1 , &CommStatus);
					return SYS_SUCCESS;
				}

				if (os != Sense_Read) {
					ck = 0;
					_sleep(100);
				} else ck++;
			}
			continue;
		}

		if      (Target == ABSENT  && Sense_Read == ABSENT ) return SYS_SUCCESS;
		else if (Target == PRESENT && Sense_Read == PRESENT) return SYS_SUCCESS;

		nRetry ++;
		if (nRetry > SENSOR_RETRY_CNT) {
			switch ( Mode ) {
				case PICK :
							if   (Target == ABSENT) Alarm = ALARM_ABS_PIC;
							else                    Alarm = ALARM_PRS_PIC;
							break;
				case PLACE :
							if   (Target == ABSENT) Alarm = ALARM_ABS_PLC;
							else                    Alarm = ALARM_PRS_PLC;
							break;
			}
			if    (Sense_Read == ABSENT)	WRITE_DIGITAL(FM_Wafer_Result , 0 , &CommStatus );
			else							WRITE_DIGITAL(FM_Wafer_Result , 1 , &CommStatus );
			AlmResult = ALARM_MANAGE( Alarm );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
			else return SYS_AVAILABLE;
		} else {
			_LOG("-----> Wafer Sensor Retry[%d] : Check %d, Target %d, Mode %d",
						nRetry, Check, Target, Mode);
			if(! WAIT_SECONDS(1)) return SYS_ABORTED;
		}
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	i;
	int nSensorDI , nSensorDI2;
	BOOL Check;
	BOOL bDBCheck = FALSE; //2007.08.01
	char SensorStr[64] , RunStr[64] , ElseStr[64] , ElseStr2[64] , TrStr[64] , TrStr2[64];
	Module_Status msRet = SYS_SUCCESS;

	if ( SIMULATION_MODE ) return SYS_SUCCESS;

	if (READ_DIGITAL(CONTROL , &i ) == 1) return SYS_SUCCESS;

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , TrStr , 63 ); // Mode
	STR_SEPERATE( TrStr    , SensorStr , ElseStr2 , 63 ); // Chamber
	STR_SEPERATE( ElseStr2 , TrStr     , ElseStr  , 63 ); // Slot1 (Not Used)
	STR_SEPERATE( ElseStr  , TrStr     , ElseStr2 , 63 ); // Slot2 (Not Used)
	STR_SEPERATE( ElseStr2 , TrStr2    , ElseStr  , 63 ); // Double Check or Transfer2 Check
	STR_SEPERATE( ElseStr  , TrStr     , ElseStr2 , 63 ); // Transfer2 Check
	//
	_LOG("-----> %s %s %s WaferSense Started in FM WaferSense Func", RunStr, SensorStr, TrStr);
	//
	//2007.08.01 Modified for Insert Double Wafer Sensor Check Routine
	if (STRCMP_L(TrStr2 , "DB") || STRCMP_L(TrStr2 , "TRANSFER2")) {
		if (STRCMP_L(TrStr2 , "DB")) {
			bDBCheck = TRUE;
			if   (STRCMP_L(TrStr , "TRANSFER2"))  Check = FALSE;
			else                                  Check = TRUE;
		} else Check = FALSE;
	} else {
		Check = TRUE;
	}

	do {
		if (TRUE == bDBCheck) {
			printf("Start Double Arm Transfer Wafer Sensor Check...\n");
			nSensorDI  = Vacuum_Sensor;
			nSensorDI2 = Vacuum_SensorB;
		} else {
			if (0 == strncmp(SensorStr, "A", strlen("A")))       {
				nSensorDI = Vacuum_Sensor;
			} else if(0 == strncmp(SensorStr, "B", strlen("B"))) {
				nSensorDI = Vacuum_SensorB;
			} else {
				printf( "---> Failed to find ATM Hand - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
				msRet = SYS_ABORTED;
				break;
			}
		}

		//
		if      (STRCMP_L(RunStr   , "ABSENT_PICK"))   {
			if (TRUE == bDBCheck) {
				msRet = SensorCheck(Check , nSensorDI , ABSENT  , PICK);
				if (SYS_SUCCESS != msRet) return msRet;
				msRet = SensorCheck(Check , nSensorDI2, ABSENT  , PICK);
			} else msRet = SensorCheck(Check , nSensorDI , ABSENT , PICK);
		} else if (STRCMP_L(RunStr , "ABSENT_PLACE"))  {
			if (TRUE == bDBCheck) {
				msRet = SensorCheck(Check , nSensorDI , ABSENT  , PLACE);
				if (SYS_SUCCESS != msRet) return msRet;
				msRet = SensorCheck(Check , nSensorDI2, ABSENT  , PLACE);
			} else msRet = SensorCheck(Check , nSensorDI , ABSENT  , PLACE);
		} else if (STRCMP_L(RunStr , "PRESENT_PICK"))  {
			if (TRUE == bDBCheck) {
				msRet = SensorCheck(Check , nSensorDI , PRESENT , PICK);
				if (SYS_SUCCESS != msRet) return msRet;
                msRet = SensorCheck(Check , nSensorDI2, PRESENT , PICK);
			} else msRet = SensorCheck(Check , nSensorDI , PRESENT , PICK);
		} else if (STRCMP_L(RunStr , "PRESENT_PLACE")) {
			if (TRUE == bDBCheck) {
				msRet = SensorCheck(Check , nSensorDI , PRESENT , PLACE);
				if (SYS_SUCCESS != msRet) return msRet;
                msRet = SensorCheck(Check , nSensorDI2, PRESENT , PLACE);
			} else msRet = SensorCheck(Check , nSensorDI , PRESENT , PLACE);
		} else if (STRCMP_L(RunStr , "CHECK"))         {
			msRet = SensorCheck(FALSE , nSensorDI, 0 , 0 );
		} else {
			printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
			msRet = SYS_ABORTED;
		}
	} while(0);

	_LOG("-----> %s %s %s WaferSense Finished in FM WaferSense Func", RunStr, SensorStr, TrStr);
	return msRet;
}
//
//
//
//
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	return TRUE;
}
