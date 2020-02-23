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



const int DigitCommErr = 9000;

// Abort Gas Alarms   240~259
const int ABORT_GAS_FAIL				= 1400;
const int STOP_ALL_TIMERS_FAILED		= 1401;
const int MFC_CONTROL_FUNCTION_FAILED	= 1402;


#endif