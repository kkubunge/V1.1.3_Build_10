#ifndef __ALARMDFEINE_H__
#define __ALARMDFEINE_H__


const int _ALM_CLEAR		= ALM_CLEAR;
const int _ALM_RETRY		= ALM_RETRY;
const int _ALM_WAIT			= ALM_WAIT;
const int _ALM_IGNORE		= ALM_IGNORE;
const int _ALM_DISABLE		= ALM_DISABLE;
const int _ALM_ENABLE		= ALM_ENABLE;
const int _ALM_STOP			= ALM_STOP;
const int _ALM_ABORT		= ALM_ABORT;
const int _ALM_PAUSED		= ALM_PAUSED;

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

const int THROTTLE_VALVE_POSITION_INCORRECT		= 1106;
const int SLOW_ROUGHING_TIMED_OUT				= 1107;
const int FAST_ROUGHING_TIMED_OUT				= 1108;
const int STAGE_HEATER_TEMP_ABOVE_LIMIT			= 1109;
const int VENTING_TIMED_OUT						= 1110;
const int VENT_NOT_OK							= 1111;
const int LEAK_CHECK_LIMIT_PRESS_OVER			= 1112;
const int ALARM_TRANS_PRESS_HIGH				= 1113;
const int ALARM_ROUGH_VALVE_T_OUT				= 1114;

//... 2015.03.23 Add Check Teach Mode for Safety PLC
const int ALARM_SAFETY_PLC_NOT_RUN_MODE			= 1120;
const int ALARM_SAFETY_PLC_NOT_TEACH_MODE		= 1121;
const int ALARM_START_PRESSURE_LIMIT			= 1123; //... 2018.06.17

const int ALARM_AR_CHARGING_FAIL				= 1122;
const int ALARM_CANNOT_VENT						= 1116;
const int ALARM_DYNAMICLK_FAILL					= 1117;
const int ALARM_TOXICGAS_PURGE_FAILL			= 1118;

const int LEAK_CHECK_LIMIT_ROR_OVER				= 1119;

const int ALARM_RPG_Gate_Valve1_FAILED			= 2611;
const int ALARM_RPG_Gate_Valve2_FAILED			= 2612;

#endif
