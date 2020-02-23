#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "IPS_Log.h"

BOOL	SIMULATION_MODE		= FALSE;
#define MAX_HISTORY_COUNT 5

#define	IN_SECOND_TIME	5

#define TARGET_HOUR		23
#define TARGET_MIN		59
#define TOTAL_PM		3

double m_dPM1OperRate;
double m_dPM2OperRate;
double m_dPM3OperRate;
double m_dTotalOperRate;


char	pszFtnName[256] = "";

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {

	// PM1
	{ "PM1.PRO_RcpTypeDM"			, _K_D_IO  ,	PM1_RcpTypeDM			,	0	} ,
	{ "PM1.PRO_RunStsDM"			, _K_D_IO  ,	PM1_RUN_STATUS			,	0	} ,
	{ "CTC.PM1_CommStatus"			, _K_D_IO  ,	PM1_CommStatus			,	0	} ,

	{ "CTC.PM1_DayIDLE_TimeD"		, _K_D_IO  ,	PM1_DayIDLE_TimeD		,	0	} ,
	{ "CTC.PM1_DayIDLE_TimeH"		, _K_D_IO  ,	PM1_DayIDLE_TimeH		,	0	} ,
	{ "CTC.PM1_DayIDLE_TimeM"		, _K_D_IO  ,	PM1_DayIDLE_TimeM		,	0	} ,
	{ "CTC.PM1_DayIDLE_TimeS"		, _K_D_IO  ,	PM1_DayIDLE_TimeS		,	0	} ,
	{ "CTC.PM1_DayIDLE_Time"		, _K_D_IO  ,	PM1_DayIDLE_Time		,	0	} ,

	{ "CTC.PM1_DayClean_TimeD"		, _K_D_IO  ,	PM1_DayClean_TimeD		,	0	} ,
	{ "CTC.PM1_DayClean_TimeH"		, _K_D_IO  ,	PM1_DayClean_TimeH		,	0	} ,
	{ "CTC.PM1_DayClean_TimeM"		, _K_D_IO  ,	PM1_DayClean_TimeM		,	0	} ,
	{ "CTC.PM1_DayClean_TimeS"		, _K_D_IO  ,	PM1_DayClean_TimeS		,	0	} ,
	{ "CTC.PM1_DayClean_Time"		, _K_D_IO  ,	PM1_DayClean_Time		,	0	} ,
	
	{ "CTC.PM1_DayRun_TimeD"		, _K_D_IO  ,	PM1_DayRun_TimeD		,	0	} ,
	{ "CTC.PM1_DayRun_TimeH"		, _K_D_IO  ,	PM1_DayRun_TimeH		,	0	} ,
	{ "CTC.PM1_DayRun_TimeM"		, _K_D_IO  ,	PM1_DayRun_TimeM		,	0	} ,
	{ "CTC.PM1_DayRun_TimeS"		, _K_D_IO  ,	PM1_DayRun_TimeS		,	0	} ,
	{ "CTC.PM1_DayRun_Time"			, _K_D_IO  ,	PM1_DayRun_Time			,	0	} ,	

	{ "CTC.PM1_Idle_Log1"			, _K_S_IO  ,	PM1_Idle_Log1			,	0	} ,
	{ "CTC.PM1_Idle_Log2"			, _K_S_IO  ,	PM1_Idle_Log2			,	0	} ,
	{ "CTC.PM1_Idle_Log3"			, _K_S_IO  ,	PM1_Idle_Log3			,	0	} ,
	{ "CTC.PM1_Idle_Log4"			, _K_S_IO  ,	PM1_Idle_Log4			,	0	} ,
	{ "CTC.PM1_Idle_Log5"			, _K_S_IO  ,	PM1_Idle_Log5			,	0	} ,

	{ "CTC.PM1_Clean_Log1"			, _K_S_IO  ,	PM1_Clean_Log1			,	0	} ,
	{ "CTC.PM1_Clean_Log2"			, _K_S_IO  ,	PM1_Clean_Log2			,	0	} ,
	{ "CTC.PM1_Clean_Log3"			, _K_S_IO  ,	PM1_Clean_Log3			,	0	} ,
	{ "CTC.PM1_Clean_Log4"			, _K_S_IO  ,	PM1_Clean_Log4			,	0	} ,
	{ "CTC.PM1_Clean_Log5"			, _K_S_IO  ,	PM1_Clean_Log5			,	0	} ,
	                                                                                           
	{ "CTC.PM1_Run_Log1"			, _K_S_IO  ,	PM1_Run_Log1			,	0	} ,
	{ "CTC.PM1_Run_Log2"			, _K_S_IO  ,	PM1_Run_Log2			,	0	} ,
	{ "CTC.PM1_Run_Log3"			, _K_S_IO  ,	PM1_Run_Log3			,	0	} ,
	{ "CTC.PM1_Run_Log4"			, _K_S_IO  ,	PM1_Run_Log4			,	0	} ,
	{ "CTC.PM1_Run_Log5"			, _K_S_IO  ,	PM1_Run_Log5			,	0	} ,

	{ "CTC.PM1_OperRate_Log1"		, _K_S_IO  ,	PM1_OperRate_Log1		,	0	} ,
	{ "CTC.PM1_OperRate_Log2"		, _K_S_IO  ,	PM1_OperRate_Log2		,	0	} ,
	{ "CTC.PM1_OperRate_Log3"		, _K_S_IO  ,	PM1_OperRate_Log3		,	0	} ,
	{ "CTC.PM1_OperRate_Log4"		, _K_S_IO  ,	PM1_OperRate_Log4		,	0	} ,
	{ "CTC.PM1_OperRate_Log5"		, _K_S_IO  ,	PM1_OperRate_Log5		,	0	} ,


	// PM2
	{ "PM2.PRO_RcpTypeDM"			, _K_D_IO  ,	PM2_RcpTypeDM			,	0	} ,
	{ "PM2.PRO_RunStsDM"			, _K_D_IO  ,	PM2_RUN_STATUS			,	0	} ,
	{ "CTC.PM2_CommStatus"			, _K_D_IO  ,	PM2_CommStatus			,	0	} ,

	{ "CTC.PM2_DayIDLE_TimeD"		, _K_D_IO  ,	PM2_DayIDLE_TimeD		,	0	} ,
	{ "CTC.PM2_DayIDLE_TimeH"		, _K_D_IO  ,	PM2_DayIDLE_TimeH		,	0	} ,
	{ "CTC.PM2_DayIDLE_TimeM"		, _K_D_IO  ,	PM2_DayIDLE_TimeM		,	0	} ,
	{ "CTC.PM2_DayIDLE_TimeS"		, _K_D_IO  ,	PM2_DayIDLE_TimeS		,	0	} ,
	{ "CTC.PM2_DayIDLE_Time"		, _K_D_IO  ,	PM2_DayIDLE_Time		,	0	} ,

	{ "CTC.PM2_DayClean_TimeD"		, _K_D_IO  ,	PM2_DayClean_TimeD		,	0	} ,
	{ "CTC.PM2_DayClean_TimeH"		, _K_D_IO  ,	PM2_DayClean_TimeH		,	0	} ,
	{ "CTC.PM2_DayClean_TimeM"		, _K_D_IO  ,	PM2_DayClean_TimeM		,	0	} ,
	{ "CTC.PM2_DayClean_TimeS"		, _K_D_IO  ,	PM2_DayClean_TimeS		,	0	} ,
	{ "CTC.PM2_DayClean_Time"		, _K_D_IO  ,	PM2_DayClean_Time		,	0	} ,
	
	{ "CTC.PM2_DayRun_TimeD"		, _K_D_IO  ,	PM2_DayRun_TimeD		,	0	} ,
	{ "CTC.PM2_DayRun_TimeH"		, _K_D_IO  ,	PM2_DayRun_TimeH		,	0	} ,
	{ "CTC.PM2_DayRun_TimeM"		, _K_D_IO  ,	PM2_DayRun_TimeM		,	0	} ,
	{ "CTC.PM2_DayRun_TimeS"		, _K_D_IO  ,	PM2_DayRun_TimeS		,	0	} ,
	{ "CTC.PM2_DayRun_Time"			, _K_D_IO  ,	PM2_DayRun_Time			,	0	} ,	

	{ "CTC.PM2_Idle_Log1"			, _K_S_IO  ,	PM2_Idle_Log1			,	0	} ,
	{ "CTC.PM2_Idle_Log2"			, _K_S_IO  ,	PM2_Idle_Log2			,	0	} ,
	{ "CTC.PM2_Idle_Log3"			, _K_S_IO  ,	PM2_Idle_Log3			,	0	} ,
	{ "CTC.PM2_Idle_Log4"			, _K_S_IO  ,	PM2_Idle_Log4			,	0	} ,
	{ "CTC.PM2_Idle_Log5"			, _K_S_IO  ,	PM2_Idle_Log5			,	0	} ,

	{ "CTC.PM2_Clean_Log1"			, _K_S_IO  ,	PM2_Clean_Log1			,	0	} ,
	{ "CTC.PM2_Clean_Log2"			, _K_S_IO  ,	PM2_Clean_Log2			,	0	} ,
	{ "CTC.PM2_Clean_Log3"			, _K_S_IO  ,	PM2_Clean_Log3			,	0	} ,
	{ "CTC.PM2_Clean_Log4"			, _K_S_IO  ,	PM2_Clean_Log4			,	0	} ,
	{ "CTC.PM2_Clean_Log5"			, _K_S_IO  ,	PM2_Clean_Log5			,	0	} ,
	                                                                                           
	{ "CTC.PM2_Run_Log1"			, _K_S_IO  ,	PM2_Run_Log1			,	0	} ,
	{ "CTC.PM2_Run_Log2"			, _K_S_IO  ,	PM2_Run_Log2			,	0	} ,
	{ "CTC.PM2_Run_Log3"			, _K_S_IO  ,	PM2_Run_Log3			,	0	} ,
	{ "CTC.PM2_Run_Log4"			, _K_S_IO  ,	PM2_Run_Log4			,	0	} ,
	{ "CTC.PM2_Run_Log5"			, _K_S_IO  ,	PM2_Run_Log5			,	0	} ,

	{ "CTC.PM2_OperRate_Log1"		, _K_S_IO  ,	PM2_OperRate_Log1		,	0	} ,
	{ "CTC.PM2_OperRate_Log2"		, _K_S_IO  ,	PM2_OperRate_Log2		,	0	} ,
	{ "CTC.PM2_OperRate_Log3"		, _K_S_IO  ,	PM2_OperRate_Log3		,	0	} ,
	{ "CTC.PM2_OperRate_Log4"		, _K_S_IO  ,	PM2_OperRate_Log4		,	0	} ,
	{ "CTC.PM2_OperRate_Log5"		, _K_S_IO  ,	PM2_OperRate_Log5		,	0	} ,

	// PM3
	{ "PM3.PRO_RcpTypeDM"			, _K_D_IO  ,	PM3_RcpTypeDM			,	0	} ,
	{ "PM3.PRO_RunStsDM"			, _K_D_IO  ,	PM3_RUN_STATUS			,	0	} ,
	{ "CTC.PM3_CommStatus"			, _K_D_IO  ,	PM3_CommStatus			,	0	} ,

	{ "CTC.PM3_DayIDLE_TimeD"		, _K_D_IO  ,	PM3_DayIDLE_TimeD		,	0	} ,
	{ "CTC.PM3_DayIDLE_TimeH"		, _K_D_IO  ,	PM3_DayIDLE_TimeH		,	0	} ,
	{ "CTC.PM3_DayIDLE_TimeM"		, _K_D_IO  ,	PM3_DayIDLE_TimeM		,	0	} ,
	{ "CTC.PM3_DayIDLE_TimeS"		, _K_D_IO  ,	PM3_DayIDLE_TimeS		,	0	} ,
	{ "CTC.PM3_DayIDLE_Time"		, _K_D_IO  ,	PM3_DayIDLE_Time		,	0	} ,

	{ "CTC.PM3_DayClean_TimeD"		, _K_D_IO  ,	PM3_DayClean_TimeD		,	0	} ,
	{ "CTC.PM3_DayClean_TimeH"		, _K_D_IO  ,	PM3_DayClean_TimeH		,	0	} ,
	{ "CTC.PM3_DayClean_TimeM"		, _K_D_IO  ,	PM3_DayClean_TimeM		,	0	} ,
	{ "CTC.PM3_DayClean_TimeS"		, _K_D_IO  ,	PM3_DayClean_TimeS		,	0	} ,
	{ "CTC.PM3_DayClean_Time"		, _K_D_IO  ,	PM3_DayClean_Time		,	0	} ,
	
	{ "CTC.PM3_DayRun_TimeD"		, _K_D_IO  ,	PM3_DayRun_TimeD		,	0	} ,
	{ "CTC.PM3_DayRun_TimeH"		, _K_D_IO  ,	PM3_DayRun_TimeH		,	0	} ,
	{ "CTC.PM3_DayRun_TimeM"		, _K_D_IO  ,	PM3_DayRun_TimeM		,	0	} ,
	{ "CTC.PM3_DayRun_TimeS"		, _K_D_IO  ,	PM3_DayRun_TimeS		,	0	} ,
	{ "CTC.PM3_DayRun_Time"			, _K_D_IO  ,	PM3_DayRun_Time			,	0	} ,	

	{ "CTC.PM3_Idle_Log1"			, _K_S_IO  ,	PM3_Idle_Log1			,	0	} ,
	{ "CTC.PM3_Idle_Log2"			, _K_S_IO  ,	PM3_Idle_Log2			,	0	} ,
	{ "CTC.PM3_Idle_Log3"			, _K_S_IO  ,	PM3_Idle_Log3			,	0	} ,
	{ "CTC.PM3_Idle_Log4"			, _K_S_IO  ,	PM3_Idle_Log4			,	0	} ,
	{ "CTC.PM3_Idle_Log5"			, _K_S_IO  ,	PM3_Idle_Log5			,	0	} ,

	{ "CTC.PM3_Clean_Log1"			, _K_S_IO  ,	PM3_Clean_Log1			,	0	} ,
	{ "CTC.PM3_Clean_Log2"			, _K_S_IO  ,	PM3_Clean_Log2			,	0	} ,
	{ "CTC.PM3_Clean_Log3"			, _K_S_IO  ,	PM3_Clean_Log3			,	0	} ,
	{ "CTC.PM3_Clean_Log4"			, _K_S_IO  ,	PM3_Clean_Log4			,	0	} ,
	{ "CTC.PM3_Clean_Log5"			, _K_S_IO  ,	PM3_Clean_Log5			,	0	} ,
	                                                                                           
	{ "CTC.PM3_Run_Log1"			, _K_S_IO  ,	PM3_Run_Log1			,	0	} ,
	{ "CTC.PM3_Run_Log2"			, _K_S_IO  ,	PM3_Run_Log2			,	0	} ,
	{ "CTC.PM3_Run_Log3"			, _K_S_IO  ,	PM3_Run_Log3			,	0	} ,
	{ "CTC.PM3_Run_Log4"			, _K_S_IO  ,	PM3_Run_Log4			,	0	} ,
	{ "CTC.PM3_Run_Log5"			, _K_S_IO  ,	PM3_Run_Log5			,	0	} ,

	{ "CTC.PM3_OperRate_Log1"		, _K_S_IO  ,	PM3_OperRate_Log1		,	0	} ,
	{ "CTC.PM3_OperRate_Log2"		, _K_S_IO  ,	PM3_OperRate_Log2		,	0	} ,
	{ "CTC.PM3_OperRate_Log3"		, _K_S_IO  ,	PM3_OperRate_Log3		,	0	} ,
	{ "CTC.PM3_OperRate_Log4"		, _K_S_IO  ,	PM3_OperRate_Log4		,	0	} ,
	{ "CTC.PM3_OperRate_Log5"		, _K_S_IO  ,	PM3_OperRate_Log5		,	0	} ,

	{ "CTC.Total_OperRate_Log1"		, _K_S_IO  ,	Total_OperRate_Log1		,	0	} ,
	{ "CTC.Total_OperRate_Log2"		, _K_S_IO  ,	Total_OperRate_Log2		,	0	} ,
	{ "CTC.Total_OperRate_Log3"		, _K_S_IO  ,	Total_OperRate_Log3		,	0	} ,
	{ "CTC.Total_OperRate_Log4"		, _K_S_IO  ,	Total_OperRate_Log4		,	0	} ,
	{ "CTC.Total_OperRate_Log5"		, _K_S_IO  ,	Total_OperRate_Log5		,	0	} ,

	{ "CTC.OPERRATE_SAVE"			, _K_D_IO  ,	Log_Save				,	0	} ,


	
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PM1_SaveIO(int nHourIdle, int nMinIdle, int nSecIdle, int nHourClean, int nMinClean, int nSecClean, int nHourRun, int nMinRun, int nSecRun)
{

	SYSTEMTIME time;
	int        i , nCommStatus;	
	char       bufDate[80];

	char       bufIdle[80];
	char       bufBeforeIdle[80];

	char	   bufClean[80];
	char	   bufBeforeClean[80];
	
	char	   bufRun[80];
	char	   bufBeforeRun[80];

	char	bufOperRate[10];

	int		nRunTime;


	GetLocalTime(&time);		
	sprintf(bufDate, "%d/%02d/%02d", time.wYear, time.wMonth, time.wDay);

	sprintf(bufIdle		,"%s  %d:%d:%d", bufDate, nHourIdle, nMinIdle, nSecIdle);
	sprintf(bufClean	,"%s  %d:%d:%d", bufDate, nHourClean, nMinClean, nSecClean);
	sprintf(bufRun		,"%s  %d:%d:%d", bufDate, nHourRun, nMinRun, nSecRun);

	nRunTime = READ_DIGITAL(PM1_DayRun_Time, &nCommStatus);	
	m_dPM1OperRate = ((double) nRunTime / 86400) *100;	// 3600 * 24 =86400

	sprintf(bufOperRate	,"%.2f", m_dPM1OperRate);

	// Last History
	for (i = 1 ; i < MAX_HISTORY_COUNT ; i++) {

		READ_STRING(PM1_Idle_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeIdle, &nCommStatus);
		WRITE_STRING(PM1_Idle_Log1 + MAX_HISTORY_COUNT - i, bufBeforeIdle, &nCommStatus);

		READ_STRING(PM1_Clean_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeClean, &nCommStatus);
		WRITE_STRING(PM1_Clean_Log1 + MAX_HISTORY_COUNT - i, bufBeforeClean, &nCommStatus);

		READ_STRING(PM1_Run_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeRun, &nCommStatus);
		WRITE_STRING(PM1_Run_Log1 + MAX_HISTORY_COUNT - i, bufBeforeRun, &nCommStatus);
		
		READ_STRING(PM1_OperRate_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeRun, &nCommStatus);
		WRITE_STRING(PM1_OperRate_Log1 + MAX_HISTORY_COUNT - i, bufBeforeRun, &nCommStatus);
	}

	// Current History
	WRITE_STRING(PM1_Idle_Log1, bufIdle, &nCommStatus);
	WRITE_STRING(PM1_Clean_Log1, bufClean, &nCommStatus);
	WRITE_STRING(PM1_Run_Log1, bufRun, &nCommStatus);
	WRITE_STRING(PM1_OperRate_Log1, bufOperRate, &nCommStatus);

	_Rate_LOG(pszFtnName, ELT_File, "PM1 Idle Time	: %s", bufIdle);
	_Rate_LOG(pszFtnName, ELT_File, "PM1 Clean Time	: %s", bufClean);	
	_Rate_LOG(pszFtnName, ELT_File, "PM1 Run Time	: %s", bufRun);
	_Rate_LOG(pszFtnName, ELT_File, "PM1 Operation Rate	: %.2f\n", m_dPM1OperRate);

	WRITE_DIGITAL(Log_Save, 1, &nCommStatus);

}

void PM2_SaveIO(int nHourIdle, int nMinIdle, int nSecIdle, int nHourClean, int nMinClean, int nSecClean, int nHourRun, int nMinRun, int nSecRun)
{

	SYSTEMTIME time;
	int        i , nCommStatus;	
	char       bufDate[80];

	char       bufIdle[80];
	char       bufBeforeIdle[80];

	char	   bufClean[80];
	char	   bufBeforeClean[80];
	
	char	   bufRun[80];
	char	   bufBeforeRun[80];

	char	bufOperRate[10];
	int		nRunTime;


	GetLocalTime(&time);		
	sprintf(bufDate, "%d/%02d/%02d", time.wYear, time.wMonth, time.wDay);

	sprintf(bufIdle		,"%s  %d:%d:%d", bufDate, nHourIdle, nMinIdle, nSecIdle);
	sprintf(bufClean	,"%s  %d:%d:%d", bufDate, nHourClean, nMinClean, nSecClean);
	sprintf(bufRun		,"%s  %d:%d:%d", bufDate, nHourRun, nMinRun, nSecRun);

	nRunTime = READ_DIGITAL(PM2_DayRun_Time, &nCommStatus);
	m_dPM2OperRate = ((double) nRunTime / 86400) *100;
	sprintf(bufOperRate	,"%.2f", m_dPM2OperRate);

	// Last History
	for (i = 1 ; i < MAX_HISTORY_COUNT ; i++) {

		READ_STRING(PM2_Idle_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeIdle, &nCommStatus);
		WRITE_STRING(PM2_Idle_Log1 + MAX_HISTORY_COUNT - i, bufBeforeIdle, &nCommStatus);

		READ_STRING(PM2_Clean_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeClean, &nCommStatus);
		WRITE_STRING(PM2_Clean_Log1 + MAX_HISTORY_COUNT - i, bufBeforeClean, &nCommStatus);

		READ_STRING(PM2_Run_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeRun, &nCommStatus);
		WRITE_STRING(PM2_Run_Log1 + MAX_HISTORY_COUNT - i, bufBeforeRun, &nCommStatus);
		
		READ_STRING(PM2_OperRate_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeRun, &nCommStatus);
		WRITE_STRING(PM2_OperRate_Log1 + MAX_HISTORY_COUNT - i, bufBeforeRun, &nCommStatus);
	}

	// Current History
	WRITE_STRING(PM2_Idle_Log1, bufIdle, &nCommStatus);
	WRITE_STRING(PM2_Clean_Log1, bufClean, &nCommStatus);
	WRITE_STRING(PM2_Run_Log1, bufRun, &nCommStatus);
	WRITE_STRING(PM2_OperRate_Log1, bufOperRate, &nCommStatus);

	_Rate_LOG(pszFtnName, ELT_File, "PM2 Idle Time	: %s", bufIdle);
	_Rate_LOG(pszFtnName, ELT_File, "PM2 Clean Time	: %s", bufClean);	
	_Rate_LOG(pszFtnName, ELT_File, "PM2 Run Time	: %s", bufRun);
	_Rate_LOG(pszFtnName, ELT_File, "PM2 Operation Rate	: %.2f\n", m_dPM2OperRate);

	WRITE_DIGITAL(Log_Save, 1, &nCommStatus);

}

void PM3_SaveIO(int nHourIdle, int nMinIdle, int nSecIdle, int nHourClean, int nMinClean, int nSecClean, int nHourRun, int nMinRun, int nSecRun)
{

	SYSTEMTIME time;
	int        i , nCommStatus;	
	char       bufDate[80];

	char       bufIdle[80];
	char       bufBeforeIdle[80];

	char	   bufClean[80];
	char	   bufBeforeClean[80];
	
	char	   bufRun[80];
	char	   bufBeforeRun[80];

	char	bufOperRate[10];
	char	buftotalOperRate[10];

	int		nRunTime;

	GetLocalTime(&time);		
	sprintf(bufDate, "%d/%02d/%02d", time.wYear, time.wMonth, time.wDay);

	sprintf(bufIdle		,"%s  %d:%d:%d", bufDate, nHourIdle, nMinIdle, nSecIdle);
	sprintf(bufClean	,"%s  %d:%d:%d", bufDate, nHourClean, nMinClean, nSecClean);
	sprintf(bufRun		,"%s  %d:%d:%d", bufDate, nHourRun, nMinRun, nSecRun);

	nRunTime = READ_DIGITAL(PM3_DayRun_Time, &nCommStatus);
	m_dPM3OperRate = ((double) nRunTime / 86400) *100;
	sprintf(bufOperRate	,"%.2f", m_dPM3OperRate);

	m_dTotalOperRate = (m_dPM1OperRate + m_dPM2OperRate + m_dPM3OperRate) / TOTAL_PM;
	sprintf(buftotalOperRate	,"%.2f", m_dTotalOperRate);


	// Last History
	for (i = 1 ; i < MAX_HISTORY_COUNT ; i++) {

		READ_STRING(PM3_Idle_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeIdle, &nCommStatus);
		WRITE_STRING(PM3_Idle_Log1 + MAX_HISTORY_COUNT - i, bufBeforeIdle, &nCommStatus);

		READ_STRING(PM3_Clean_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeClean, &nCommStatus);
		WRITE_STRING(PM3_Clean_Log1 + MAX_HISTORY_COUNT - i, bufBeforeClean, &nCommStatus);

		READ_STRING(PM3_Run_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeRun, &nCommStatus);
		WRITE_STRING(PM3_Run_Log1 + MAX_HISTORY_COUNT - i, bufBeforeRun, &nCommStatus);
		
		READ_STRING(PM3_OperRate_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeRun, &nCommStatus);
		WRITE_STRING(PM3_OperRate_Log1 + MAX_HISTORY_COUNT - i, bufBeforeRun, &nCommStatus);

		READ_STRING(Total_OperRate_Log1 + MAX_HISTORY_COUNT - i-1, bufBeforeRun, &nCommStatus);
		WRITE_STRING(Total_OperRate_Log1 + MAX_HISTORY_COUNT - i, bufBeforeRun, &nCommStatus);
	}

	// Current History
	WRITE_STRING(PM3_Idle_Log1, bufIdle, &nCommStatus);
	WRITE_STRING(PM3_Clean_Log1, bufClean, &nCommStatus);
	WRITE_STRING(PM3_Run_Log1, bufRun, &nCommStatus);
	WRITE_STRING(PM3_OperRate_Log1, bufOperRate, &nCommStatus);
	WRITE_STRING(Total_OperRate_Log1, buftotalOperRate, &nCommStatus);
	
	_Rate_LOG(pszFtnName, ELT_File, "PM3 Idle Time	: %s", bufIdle);
	_Rate_LOG(pszFtnName, ELT_File, "PM3 Clean Time	: %s", bufClean);	
	_Rate_LOG(pszFtnName, ELT_File, "PM3 Run Time	: %s", bufRun);
	_Rate_LOG(pszFtnName, ELT_File, "PM3 Operation Rate	: %.2f\n", m_dPM3OperRate);
	_Rate_LOG(pszFtnName, ELT_File, "TOTAL Operation Rate	: %.2f\n", m_dTotalOperRate);

	WRITE_DIGITAL(Log_Save, 1, &nCommStatus);

}

void PM1_Operation_Thread()
{

	// Daily Idle, Cleaning Time
	SYSTEMTIME systime;
	int    nIOStatus      = 0;
	DWORD  dwTargetSec    = 0;
	DWORD  dwCurSec       = 0;

	double dDayIdleTime, dDayCleanTime, dDayRunTime;
	int nDayIdle, nHourIdle, nMinIdle, nSecIdle;
	int nDayClean, nHourClean, nMinClean, nSecClean;
	int nDayRun, nHourRun, nMinRun, nSecRun;

	int nSeconds;
	clock_t clkBegin = 0, clkEnd = 0, clkTimeElapsed = 0;
	clkBegin = clock();


	dDayIdleTime  = READ_DIGITAL(PM1_DayIDLE_Time, &nIOStatus);
	dDayCleanTime = READ_DIGITAL(PM1_DayClean_Time, &nIOStatus);
	dDayRunTime   = READ_DIGITAL(PM1_DayRun_Time, &nIOStatus);

	while(1)
	{
		// Daily Idle, Cleaning Time Reset
		GetLocalTime(&systime);
		dwCurSec    = (systime.wHour * 3600) + (systime.wMinute * 60) + systime.wSecond;
		dwTargetSec = (TARGET_HOUR * 3600) + (TARGET_MIN * 60) + 30;

		if (abs(dwCurSec - dwTargetSec) > IN_SECOND_TIME)
		{
			;
		}
		else 
		{
			if (dDayIdleTime > 60 || dDayCleanTime > 60 || dDayRunTime >60 )
			{
				printf("PM1 Daily Time Reset\n");
				PM1_SaveIO(nHourIdle, nMinIdle, nSecIdle, nHourClean, nMinClean, nSecClean, nHourRun, nMinRun, nSecRun);
				dDayIdleTime = 0;
				dDayCleanTime	= 0;
				dDayRunTime = 0;
			}
		}

		Sleep(1000);
		clkEnd = clock();
		clkTimeElapsed += (clkEnd - clkBegin);
		nSeconds = clkTimeElapsed / 1000;
		clkTimeElapsed = clkTimeElapsed % 1000;
		clkBegin = clkEnd;

		// Daily Idle Time
		if ((4 == READ_DIGITAL(PM1_RcpTypeDM, &nIOStatus)) && (1 == READ_DIGITAL(PM1_RUN_STATUS, &nIOStatus)) && (3 == READ_DIGITAL(PM1_CommStatus, &nIOStatus)))	// Idle(4) Running(1) Enable(3)
		{
			dDayIdleTime += nSeconds;
		}	
		
		nDayIdle = (int) ( dDayIdleTime / (24*60*60));
		nHourIdle =(int) ( dDayIdleTime - (nDayIdle*24*60*60)) / (60*60);
		nMinIdle = (int) ( dDayIdleTime - (nDayIdle*24*60*60)-(nHourIdle*60*60)) / 60;
		nSecIdle = (int) ( dDayIdleTime - (nDayIdle*24*60*60)-(nHourIdle*60*60)-(nMinIdle*60));
		WRITE_DIGITAL(PM1_DayIDLE_TimeH, nHourIdle, &nIOStatus);
		WRITE_DIGITAL(PM1_DayIDLE_TimeM, nMinIdle, &nIOStatus);
		WRITE_DIGITAL(PM1_DayIDLE_TimeS, nSecIdle, &nIOStatus);
		WRITE_DIGITAL(PM1_DayIDLE_Time,  (int) dDayIdleTime, &nIOStatus);	
		//printf("Idle Hour : %d, Min : %d, Sec : %d\n", nHourIdle, nMinIdle, nSecIdle);
		// Daily Idle Time

		// Daily Auto Dry Cleaning Time
		if ((5 == READ_DIGITAL(PM1_RcpTypeDM, &nIOStatus)) && (1 == READ_DIGITAL(PM1_RUN_STATUS, &nIOStatus)) && (7 == READ_DIGITAL(PM1_CommStatus, &nIOStatus)))	// Clean(5) Running(1) CTCInUse(7)
		{
			dDayCleanTime += nSeconds;
		}		
	
		nDayClean = (int) ( dDayCleanTime / (24*60*60));
		nHourClean =(int) ( dDayCleanTime - (nDayClean*24*60*60)) / (60*60);
		nMinClean = (int) ( dDayCleanTime - (nDayClean*24*60*60)-(nHourClean*60*60)) / 60;
		nSecClean = (int) ( dDayCleanTime - (nDayClean*24*60*60)-(nHourClean*60*60)-(nMinClean*60));
		WRITE_DIGITAL(PM1_DayClean_TimeH, nHourClean, &nIOStatus);
		WRITE_DIGITAL(PM1_DayClean_TimeM, nMinClean, &nIOStatus);
		WRITE_DIGITAL(PM1_DayClean_TimeS, nSecClean, &nIOStatus);
		WRITE_DIGITAL(PM1_DayClean_Time,  (int) dDayCleanTime, &nIOStatus);	
		//printf("Clean Hour : %d, Min : %d, Sec : %d\n", nHourClean, nMinClean, nSecClean);
		// Daily Cleaning Time

		// Daily Run Time
		if (7 == READ_DIGITAL(PM1_CommStatus, &nIOStatus))	// CTCInUse(7) by CS Requirements
		{
			dDayRunTime += nSeconds;
		}		

		nDayRun = (int) ( dDayRunTime / (24*60*60));
		nHourRun =(int) ( dDayRunTime - (nDayRun*24*60*60)) / (60*60);
		nMinRun = (int) ( dDayRunTime - (nDayRun*24*60*60)-(nHourRun*60*60)) / 60;
		nSecRun = (int) ( dDayRunTime - (nDayRun*24*60*60)-(nHourRun*60*60)-(nMinRun*60));
		WRITE_DIGITAL(PM1_DayRun_TimeH, nHourRun, &nIOStatus);
		WRITE_DIGITAL(PM1_DayRun_TimeM, nMinRun, &nIOStatus);
		WRITE_DIGITAL(PM1_DayRun_TimeS, nSecRun, &nIOStatus);
		WRITE_DIGITAL(PM1_DayRun_Time,  (int) dDayRunTime, &nIOStatus);		
		//printf("Run Hour : %d, Min : %d, Sec : %d\n", nHourRun, nMinRun, nSecRun);
		// Daily Run Time
		
	}
	_endthread();
}

void PM2_Operation_Thread()
{

	// Daily Idle, Cleaning Time
	SYSTEMTIME systime;
	int    nIOStatus      = 0;
	DWORD  dwTargetSec    = 0;
	DWORD  dwCurSec       = 0;

	double dDayIdleTime, dDayCleanTime, dDayRunTime;
	int nDayIdle, nHourIdle, nMinIdle, nSecIdle;
	int nDayClean, nHourClean, nMinClean, nSecClean;
	int nDayRun, nHourRun, nMinRun, nSecRun;

	int nSeconds;
	clock_t clkBegin = 0, clkEnd = 0, clkTimeElapsed = 0;
	clkBegin = clock();


	dDayIdleTime  = READ_DIGITAL(PM2_DayIDLE_Time, &nIOStatus);
	dDayCleanTime = READ_DIGITAL(PM2_DayClean_Time, &nIOStatus);
	dDayRunTime   = READ_DIGITAL(PM2_DayRun_Time, &nIOStatus);

	while(1)
	{
		// Daily Idle, Cleaning Time Reset
		GetLocalTime(&systime);
		dwCurSec    = (systime.wHour * 3600) + (systime.wMinute * 60) + systime.wSecond;
		dwTargetSec = (TARGET_HOUR * 3600) + ( TARGET_MIN * 60) + 35;

		if (abs(dwCurSec - dwTargetSec) > IN_SECOND_TIME)
		{
			;
		}
		else 
		{
			if (dDayIdleTime > 60 || dDayCleanTime > 60 || dDayRunTime > 60 )
			{
				printf("PM2 Daily Time Reset\n");
				PM2_SaveIO(nHourIdle, nMinIdle, nSecIdle, nHourClean, nMinClean, nSecClean, nHourRun, nMinRun, nSecRun);
				dDayIdleTime = 0;
				dDayCleanTime	= 0;
				dDayRunTime = 0;
			}
		}

		Sleep(1000);
		clkEnd = clock();
		clkTimeElapsed += (clkEnd - clkBegin);
		nSeconds = clkTimeElapsed / 1000;
		clkTimeElapsed = clkTimeElapsed % 1000;
		clkBegin = clkEnd;

		// Daily Idle Time
		if ((4 == READ_DIGITAL(PM2_RcpTypeDM, &nIOStatus)) && (1 == READ_DIGITAL(PM2_RUN_STATUS, &nIOStatus)) && (3 == READ_DIGITAL(PM1_CommStatus, &nIOStatus)))	// Idle(4) Running(1)
		{
			dDayIdleTime += nSeconds;
		}	
		
		nDayIdle = (int) ( dDayIdleTime / (24*60*60));
		nHourIdle =(int) ( dDayIdleTime - (nDayIdle*24*60*60)) / (60*60);
		nMinIdle = (int) ( dDayIdleTime - (nDayIdle*24*60*60)-(nHourIdle*60*60)) / 60;
		nSecIdle = (int) ( dDayIdleTime - (nDayIdle*24*60*60)-(nHourIdle*60*60)-(nMinIdle*60));
		WRITE_DIGITAL(PM2_DayIDLE_TimeH, nHourIdle, &nIOStatus);
		WRITE_DIGITAL(PM2_DayIDLE_TimeM, nMinIdle, &nIOStatus);
		WRITE_DIGITAL(PM2_DayIDLE_TimeS, nSecIdle, &nIOStatus);
		WRITE_DIGITAL(PM2_DayIDLE_Time,  (int) dDayIdleTime, &nIOStatus);	
		//printf("Idle Hour : %d, Min : %d, Sec : %d\n", nHourIdle, nMinIdle, nSecIdle);
		// Daily Idle Time

		// Daily Cleaning Time
		if ((5 == READ_DIGITAL(PM2_RcpTypeDM, &nIOStatus)) && (1 == READ_DIGITAL(PM2_RUN_STATUS, &nIOStatus)) && (7 == READ_DIGITAL(PM1_CommStatus, &nIOStatus)))	// Clean(5) Running(1)
		{
			dDayCleanTime += nSeconds;
		}		
	
		nDayClean = (int) ( dDayCleanTime / (24*60*60));
		nHourClean =(int) ( dDayCleanTime - (nDayClean*24*60*60)) / (60*60);
		nMinClean = (int) ( dDayCleanTime - (nDayClean*24*60*60)-(nHourClean*60*60)) / 60;
		nSecClean = (int) ( dDayCleanTime - (nDayClean*24*60*60)-(nHourClean*60*60)-(nMinClean*60));
		WRITE_DIGITAL(PM2_DayClean_TimeH, nHourClean, &nIOStatus);
		WRITE_DIGITAL(PM2_DayClean_TimeM, nMinClean, &nIOStatus);
		WRITE_DIGITAL(PM2_DayClean_TimeS, nSecClean, &nIOStatus);
		WRITE_DIGITAL(PM2_DayClean_Time,  (int) dDayCleanTime, &nIOStatus);	
		//printf("Clean Hour : %d, Min : %d, Sec : %d\n", nHourClean, nMinClean, nSecClean);
		// Daily Cleaning Time

		// Daily Run Time
		if (7 == READ_DIGITAL(PM2_CommStatus, &nIOStatus))	// CTCInUse(7) by CS Requirements
		{
			dDayRunTime += nSeconds;
		}		

		nDayRun = (int) ( dDayRunTime / (24*60*60));
		nHourRun =(int) ( dDayRunTime - (nDayRun*24*60*60)) / (60*60);
		nMinRun = (int) ( dDayRunTime - (nDayRun*24*60*60)-(nHourRun*60*60)) / 60;
		nSecRun = (int) ( dDayRunTime - (nDayRun*24*60*60)-(nHourRun*60*60)-(nMinRun*60));
		WRITE_DIGITAL(PM2_DayRun_TimeH, nHourRun, &nIOStatus);
		WRITE_DIGITAL(PM2_DayRun_TimeM, nMinRun, &nIOStatus);
		WRITE_DIGITAL(PM2_DayRun_TimeS, nSecRun, &nIOStatus);
		WRITE_DIGITAL(PM2_DayRun_Time,  (int) dDayRunTime, &nIOStatus);		
		//printf("Run Hour : %d, Min : %d, Sec : %d\n", nHourRun, nMinRun, nSecRun);
		// Daily Run Time
		
	}
	_endthread();
}

void PM3_Operation_Thread()
{

	// Daily Idle, Cleaning Time
	SYSTEMTIME systime;
	int    nIOStatus      = 0;
	DWORD  dwTargetSec    = 0;
	DWORD  dwCurSec       = 0;

	double dDayIdleTime, dDayCleanTime, dDayRunTime;
	int nDayIdle, nHourIdle, nMinIdle, nSecIdle;
	int nDayClean, nHourClean, nMinClean, nSecClean;
	int nDayRun, nHourRun, nMinRun, nSecRun;

	int nSeconds;
	clock_t clkBegin = 0, clkEnd = 0, clkTimeElapsed = 0;
	clkBegin = clock();


	dDayIdleTime  = READ_DIGITAL(PM3_DayIDLE_Time, &nIOStatus);
	dDayCleanTime = READ_DIGITAL(PM3_DayClean_Time, &nIOStatus);
	dDayRunTime   = READ_DIGITAL(PM3_DayRun_Time, &nIOStatus);

	while(1)
	{
		// Daily Idle, Cleaning Time Reset
		GetLocalTime(&systime);
		dwCurSec    = (systime.wHour * 3600) + (systime.wMinute * 60) + systime.wSecond;
		dwTargetSec = (TARGET_HOUR * 3600) + ( TARGET_MIN * 60) + 40;

		if (abs(dwCurSec - dwTargetSec) > IN_SECOND_TIME)
		{
			;
		}
		else 
		{
			if (dDayIdleTime > 60 || dDayCleanTime > 60 || dDayRunTime > 60 )
			{
				printf("PM3 Daily Time Reset\n");
				PM3_SaveIO(nHourIdle, nMinIdle, nSecIdle, nHourClean, nMinClean, nSecClean, nHourRun, nMinRun, nSecRun);
				dDayIdleTime = 0;
				dDayCleanTime	= 0;
				dDayRunTime = 0;
			}
		}

		Sleep(1000);
		clkEnd = clock();
		clkTimeElapsed += (clkEnd - clkBegin);
		nSeconds = clkTimeElapsed / 1000;
		clkTimeElapsed = clkTimeElapsed % 1000;
		clkBegin = clkEnd;

		// Daily Idle Time
		if ((4 == READ_DIGITAL(PM3_RcpTypeDM, &nIOStatus)) && (1 == READ_DIGITAL(PM3_RUN_STATUS, &nIOStatus)) && (3 == READ_DIGITAL(PM1_CommStatus, &nIOStatus)))	// Idle(4) Running(1)
		{
			dDayIdleTime += nSeconds;
		}	
		
		nDayIdle = (int) ( dDayIdleTime / (24*60*60));
		nHourIdle =(int) ( dDayIdleTime - (nDayIdle*24*60*60)) / (60*60);
		nMinIdle = (int) ( dDayIdleTime - (nDayIdle*24*60*60)-(nHourIdle*60*60)) / 60;
		nSecIdle = (int) ( dDayIdleTime - (nDayIdle*24*60*60)-(nHourIdle*60*60)-(nMinIdle*60));
		WRITE_DIGITAL(PM3_DayIDLE_TimeH, nHourIdle, &nIOStatus);
		WRITE_DIGITAL(PM3_DayIDLE_TimeM, nMinIdle, &nIOStatus);
		WRITE_DIGITAL(PM3_DayIDLE_TimeS, nSecIdle, &nIOStatus);
		WRITE_DIGITAL(PM3_DayIDLE_Time,  (int) dDayIdleTime, &nIOStatus);	
		//printf("Idle Hour : %d, Min : %d, Sec : %d\n", nHourIdle, nMinIdle, nSecIdle);
		// Daily Idle Time

		// Daily Cleaning Time
		if ((5 == READ_DIGITAL(PM3_RcpTypeDM, &nIOStatus)) && (1 == READ_DIGITAL(PM3_RUN_STATUS, &nIOStatus)) && (7 == READ_DIGITAL(PM1_CommStatus, &nIOStatus)))	// Clean(5) Running(1)
		{
			dDayCleanTime += nSeconds;
		}		
	
		nDayClean = (int) ( dDayCleanTime / (24*60*60));
		nHourClean =(int) ( dDayCleanTime - (nDayClean*24*60*60)) / (60*60);
		nMinClean = (int) ( dDayCleanTime - (nDayClean*24*60*60)-(nHourClean*60*60)) / 60;
		nSecClean = (int) ( dDayCleanTime - (nDayClean*24*60*60)-(nHourClean*60*60)-(nMinClean*60));
		WRITE_DIGITAL(PM3_DayClean_TimeH, nHourClean, &nIOStatus);
		WRITE_DIGITAL(PM3_DayClean_TimeM, nMinClean, &nIOStatus);
		WRITE_DIGITAL(PM3_DayClean_TimeS, nSecClean, &nIOStatus);
		WRITE_DIGITAL(PM3_DayClean_Time,  (int) dDayCleanTime, &nIOStatus);	
		//printf("Clean Hour : %d, Min : %d, Sec : %d\n", nHourClean, nMinClean, nSecClean);
		// Daily Cleaning Time

		// Daily Run Time
		if (7 == READ_DIGITAL(PM3_CommStatus, &nIOStatus))	// CTCInUse(7) by CS Requirements
		{
			dDayRunTime += nSeconds;
		}		

		nDayRun = (int) ( dDayRunTime / (24*60*60));
		nHourRun =(int) ( dDayRunTime - (nDayRun*24*60*60)) / (60*60);
		nMinRun = (int) ( dDayRunTime - (nDayRun*24*60*60)-(nHourRun*60*60)) / 60;
		nSecRun = (int) ( dDayRunTime - (nDayRun*24*60*60)-(nHourRun*60*60)-(nMinRun*60));
		WRITE_DIGITAL(PM3_DayRun_TimeH, nHourRun, &nIOStatus);
		WRITE_DIGITAL(PM3_DayRun_TimeM, nMinRun, &nIOStatus);
		WRITE_DIGITAL(PM3_DayRun_TimeS, nSecRun, &nIOStatus);
		WRITE_DIGITAL(PM3_DayRun_Time,  (int) dDayRunTime, &nIOStatus);		
		//printf("Run Hour : %d, Min : %d, Sec : %d\n", nHourRun, nMinRun, nSecRun);
		//Daily Run Time
		
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {

	Sleep(1 * 10000);

	_Rate_LOG(pszFtnName, ELT_File, "Operation Rate Monitoring Start...\n");

	_beginthread((void*)PM1_Operation_Thread, 0, NULL);
	Sleep(200);
	_beginthread((void*)PM2_Operation_Thread, 0, NULL);
	Sleep(200);
	_beginthread((void*)PM3_Operation_Thread, 0, NULL);
	return SYS_SUCCESS;
//
//
}