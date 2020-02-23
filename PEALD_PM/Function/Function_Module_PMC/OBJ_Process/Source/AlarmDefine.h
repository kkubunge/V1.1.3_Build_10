#ifndef __ALARMDFEINE_H__
#define __ALARMDFEINE_H__

//------------------------------------------------------------------------------------------
// Process Alarm Definition

#define ALARM_PROCESS_ABORTED				1001
// 2006.06.14
#define ALARM_PROCESS_PAUSED				1050
#define ALARM_WAFER_DETECTED_CAN_NOT_RUN	1055		// 2006.09.15

#define ALARM_SW_FUNC_ERROR_OCCURED			1002
#define ALARM_RECIPE_READING_FAILED			1003
#define ALARM_VALVE_RECIPE_DOWNLOAD_FAILED	1004
#define ALARM_VALVE_RECIPE_START_TIMEOUT	1005
#define ALARM_VALVE_RECIPE_START_ERROR		1006
#define ALARM_VALVE_RECIPE_DATA_ERROR		1007
#define ALARM_VALVE_RECIPE_UNKNOWN_ERROR	1008
#define ALARM_INVALID_RECIPE_DATA			1013

#define ALARM_TM_ISO_VALVE_NOT_CLOSE		1009

#define ALARM_PLC_GBL_INTERLOCK_CHK			1010
#define ALARM_SW_INTERLOCK_CHK				1011
#define ALARM_MODULE_ABNORMAL_FINISHED		1012

#define ALARM_STGHTR_CONTROL_TIMEOUT		1014
#define ALARM_STGHTR_TEMP_CHECK_FAILED		1015
#define ALARM_STGHTR_STABLE_TIMEOUT			1016

#define ALARM_VALVE_RECIPE_DOWNLOAD_TIMEOUT	1020
#define ALARM_SW_AND_PLC_SYNC_FAILED		1021
#define ALARM_PROGRESS_STEP_TIME_IS_INVALID	1022
#define ALARM_DOWNSTEP_RUNSTEP_DIFF			1023		/* 2006.07.07 */
#define ALARM_PLC_IS_ABNORMAL_STOPED		1024		/* 2006.07.10 */

#define ALARM_HEATER_CONTROL_TIMEOUT		1030
#define ALARM_STEP_TIME_COMPEN_OVER			1033		/* 2007.01.19 */

#define ALARM_UNKNOWN_RECIPE_TYPE			1040
#define ALARM_VALVE_TIME_IS_INVALID			1041
#define ALARM_STEP_GROUP_RANGE_IS_INVALID	1042
#define ALARM_PRE_HEAT_CAN_NOT_BE_USED		1043
#define ALARM_PM_STEP_SYNC_CAN_NOT_BE_USED	1044

#define ALARM_PM_ALREADY_PROCESS_RUNNING	1049

#define ALARM_DRY_CLEAN_COUNT_RESET_TIMEOUT	1061		//... 2014.10.13

#define ALARM_POSITIONX_TOLERANCE_WARNING   1062		//2014.07.02
#define ALARM_POSITIONX_TOLERANCE_ALARM     1063		//2014.07.02

//... 2015.02.26 RPG Alarm Auto Retry
#define ALARM_RPG_AUTO_RETRY_MAX_COUNT_OVER 1064

//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
#define ALARM_INVALID_VLVSTS_IN_CTC_RCP		 1065
#define ALARM_ALD_VALVE_COUNT_FAIL			1066

#define ALARM_RPG_PLASMA_NOT_DETECTED		2605		//... Defined Control Monitoring Alarm
#define ALARM_RPG_REFLECT_IS_HIGH			2606		//... Defined Control Monitoring Alarm

//... 2017.03.07
#define ALARM_CHUCK_MOVEING_STEP_SETTING	2531

//------------------------------------------------------------------------------------------

const int _ALM_CLEAR		= ALM_CLEAR;
const int _ALM_RETRY		= ALM_RETRY;
const int _ALM_WAIT			= ALM_WAIT;
const int _ALM_IGNORE		= ALM_IGNORE;
const int _ALM_DISABLE		= ALM_DISABLE;
const int _ALM_ENABLE		= ALM_ENABLE;
const int _ALM_STOP			= ALM_STOP;
const int _ALM_ABORT		= ALM_ABORT;
const int _ALM_PAUSED		= ALM_PAUSED;

//------------------------------------------------------------------------------------------

#define __KF_DECLARE_ALARM\
	BOOL  __KF_bAborted = FALSE;\
	int	__AlarmRecoveryCode = -1;


#define __KF_BEGIN_ALARM\
	{ __KF_bAborted = FALSE;\
	do	\
	{	\
		__AlarmRecoveryCode = -1;

#define __SET_RECOVERY_CODE(__RecoveryCode)\
	__AlarmRecoveryCode = __RecoveryCode;

#define __KF_END_ALARM\
	if( __AlarmRecoveryCode == ALM_ABORT )\
	{ __KF_bAborted = TRUE;\
	break;}\
	else if( __AlarmRecoveryCode == ALM_RETRY )\
		continue;\
	else if( __AlarmRecoveryCode == ALM_IGNORE || __AlarmRecoveryCode ==  ALM_CLEAR)\
		break;\
	else if( __AlarmRecoveryCode == -1)\
		break;\
	}while(1);\
	if( TRUE == __KF_bAborted ) break; }



const int ABORT_GAS_FAIL						= 1400;

// Common Alarm
const int DigitCommErr = 9000;

// Pump Function Alarm Range 140~159
const int VACUUM_CONTROL_FUNCTION_FAIL			= 1100;
const int VACUUM_SENSOR_OR_GAUGE_FAIL			= 1101;
const int DRY_PUMP_NOT_RUNNING					= 1102;
const int PUMPING_LINES_NOT_PUMPED_DOWN			= 1103;
const int DP_BOOSTER_NOT_RUNNING				= 1104;
const int ACV_POSITION_INCORRECT				= 1105;
const int THROTTLE_VALVE_POSITION_INCORRECT		= 1106;
const int SLOW_ROUGHING_TIMED_OUT				= 1107;
const int FAST_ROUGHING_TIMED_OUT				= 1108;
const int STAGE_HEATER_TEMP_ABOVE_LIMIT			= 1109;
const int VENTING_TIMED_OUT						= 1110;
const int VENT_NOT_OK							= 1111;


#endif