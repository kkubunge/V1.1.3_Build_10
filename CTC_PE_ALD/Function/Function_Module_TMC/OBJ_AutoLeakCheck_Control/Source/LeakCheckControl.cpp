// LeakCheckControl.cpp: implementation of the CLeakCheckControl class.
// 2008.04.02 by mgsong
// 
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "LeakCheckControl.h"
#include "TextLogDll.h"
#include "cimseqnc.h"
#include <math.h>

extern	void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szData);

#define		ALARM_AUTOLEAKCHECK_ABORT	6156
#define		INIT_WAIT_TIME	15
#define		IDLE_WAIT_HOUR	0
#define		IDLE_WAIT_TIME	1
#define		DRY_WAIT_TIMEOVER	120
#define		MAX_LK_PARAMETERDO	12
#define		MAX_LK_PARAMETERAO	45
#define		MAX_PM_VERTUAL		3


enum {__SEQUENCE_IDLE = 0, __SEQUENCE_RUNNING};
enum {__IDLE_STOP = 0, __IDLE_RUNNING = 1};
enum {__PURGE_STOP = 0, __PURGE_RUNNING = 1};
enum {__ABSENT = 0, __PRESENT};

//Function Table Initialize
#define TM_FUNC_TABLE     {TM_VAC_CONTROL,     \
                           TM_OVERPRESSURE,    \
                          "TM",                \
                          "LEAKCHECK",         \
                          "PUMP",              \
                          "OPEN_OPRES_VALVE",  \
                          "CLOSE_OPRES_VALVE"}

#define BM1_FUNC_TABLE    {BM1_VAC_CONTROL,    \
                           BM1_OVERPRESSURE,   \
                          "BM1",               \
                          "LEAKCHECK",         \
                          "PUMP",              \
                          "OPEN_OPRES_VALVE",  \
                          "CLOSE_OPRES_VALVE"}


#define BM2_FUNC_TABLE    {BM2_VAC_CONTROL,   \
                           BM2_OVERPRESSURE,  \
                          "BM2",              \
                          "LEAKCHECK",        \
                          "PUMP",             \
                          "OPEN_OPRES_VALVE", \
                          "CLOSE_OPRES_VALVE"}


SFUNCTABLE g_sFuncTable[] = {BM1_FUNC_TABLE , BM2_FUNC_TABLE , TM_FUNC_TABLE};

//2011.12.09 by mgsong
int g_nMdays[12]={31,28,31,30,31,30,31,31,30,31,30,31};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLeakCheckControl::CLeakCheckControl()
{
	m_bAbort     = FALSE;
	m_bRunning   = FALSE;
	m_dwThreadId = 0;

	m_nPM1Hour   = 0;
	m_nPM1Min    = 0;
	m_nPM2Hour   = 0;
	m_nPM2Min    = 0;
	m_nPM3Hour   = 0;
	m_nPM3Min    = 0;

	m_nOverPresVlvSts = 0;

	m_nAccIdleTime = 0;
	m_nAccLotCount = 0;
	m_nAccWaferCount = 0;

	_gINIT_LOG();
}

CLeakCheckControl::~CLeakCheckControl()
{
	if(0 != m_dwThreadId)
		PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);

	_gCLOSE_LOG();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : 
//Date         : 2007.11.21
//Description  : 
//Arguments    :
//Return Value : 
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::Initialize()
{
	BOOL bRet = FALSE;
	int  nIOStatus = 0;

	HANDLE hThread = CreateThread(NULL, 0, Thread_LeakCheckCtrl, (void*)this, 0, &m_dwThreadId);
	if (NULL == hThread) {
		bRet = FALSE;
		printf("Leak Check Control Thread Create Fail.. \n");
		_LOG("Leak Check Control Thread Create Fail.. ");
	} 
	else 
	{
		m_nPM1Hour = READ_DIGITAL(PM1IdleHour , &nIOStatus);
		m_nPM1Min  = READ_DIGITAL(PM1IdleMin  , &nIOStatus);
		m_nPM2Hour = READ_DIGITAL(PM2IdleHour , &nIOStatus);
		m_nPM2Min  = READ_DIGITAL(PM2IdleMin  , &nIOStatus);
		m_nPM3Hour = READ_DIGITAL(PM3IdleHour , &nIOStatus);
		m_nPM3Min  = READ_DIGITAL(PM3IdleMin  , &nIOStatus);

		//Write Leak Check Status to READY
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);

		//... 2016.01.06 Add PM LeakCheck & Lot Pre Flag
		for (int i = 0; i < PM_COUNT; i++)		m_nPM_LeakChk[i] = enPM_LC_NONE;

		bRet =TRUE;
	}
	CloseHandle(hThread);
	
	//2012.03.22
	DWORD dwTemp;
	hThread = CreateThread(NULL , 0 , Thread_CountAccumlate , (void*)this , 0 , &dwTemp);
	if (NULL == hThread) {
		_LOG("Leak Check Count Acc Thread Create Fail.. ");
		bRet = FALSE;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetIdleRunTime
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::SetIdleRunTime(int nHour , int nMin)
{
	int  nIOStatus = 0;
	do {
		if (nHour < 0 || nMin < 0) break;

		//Save Current Set Time
	    m_nPM1Hour = READ_DIGITAL(PM1IdleHour , &nIOStatus);
	    m_nPM1Min  = READ_DIGITAL(PM1IdleMin  , &nIOStatus);
		m_nPM2Hour = READ_DIGITAL(PM2IdleHour , &nIOStatus);
		m_nPM2Min  = READ_DIGITAL(PM2IdleMin  , &nIOStatus);
		m_nPM3Hour = READ_DIGITAL(PM3IdleHour , &nIOStatus);
		m_nPM3Min  = READ_DIGITAL(PM3IdleMin  , &nIOStatus);

		_LOG("Save Current Idle Time PM1 %d , %d" , m_nPM1Hour , m_nPM1Min);
		_LOG("Save Current Idle Time PM2 %d , %d" , m_nPM2Hour , m_nPM2Min);
		_LOG("Save Current Idle Time PM3 %d , %d" , m_nPM3Hour , m_nPM3Min);

		//Change Set Time
	    WRITE_DIGITAL(PM1IdleHour , nHour , &nIOStatus);
	    WRITE_DIGITAL(PM1IdleMin  , nMin  , &nIOStatus);
		WRITE_DIGITAL(PM2IdleHour , nHour , &nIOStatus);
		WRITE_DIGITAL(PM2IdleMin  , nMin  , &nIOStatus);
		WRITE_DIGITAL(PM3IdleHour , nHour , &nIOStatus);
		WRITE_DIGITAL(PM3IdleMin  , nMin  , &nIOStatus);

		_LOG("Change Idle Time PM %d , %d" , nHour , nMin);
	} while (0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : RestoreIdleRunTime
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::RestoreIdleRunTime()
{
	int  nIOStatus = 0;
	do {

		_LOG("Restore m_nPm1Hour : %d , m_nPM1Min : %d" , m_nPM1Hour , m_nPM1Min);
		_LOG("Restore m_nPm2Hour : %d , m_nPM2Min : %d" , m_nPM2Hour , m_nPM2Min);
		_LOG("Restore m_nPm3Hour : %d , m_nPM3Min : %d" , m_nPM3Hour , m_nPM3Min);

		//Restore PM1
		if (m_nPM1Hour >= 0 && m_nPM1Min >= 0) {
			if (m_nPM1Hour > 1 ) m_nPM1Hour = IDLE_WAIT_HOUR;
			if (m_nPM1Min  > 10) m_nPM1Min  = IDLE_WAIT_TIME;

			WRITE_DIGITAL(PM1IdleHour , m_nPM1Hour , &nIOStatus);
			WRITE_DIGITAL(PM1IdleMin  , m_nPM1Min  , &nIOStatus);
			_LOG("Restore Idle Time PM1 %d , %d" , m_nPM1Hour , m_nPM1Min);
		} else {
			WRITE_DIGITAL(PM1IdleHour , 0  , &nIOStatus);
			WRITE_DIGITAL(PM1IdleMin  , 1  , &nIOStatus);
			_LOG("Abnormally Restore Idle Time PM1 0 , 1");
		}

		//Restore PM2
		if (m_nPM2Hour >= 0 && m_nPM2Min >= 0) {
			if (m_nPM2Hour > 1 ) m_nPM2Hour = IDLE_WAIT_HOUR;
			if (m_nPM2Min  > 10) m_nPM2Min  = IDLE_WAIT_TIME;

			WRITE_DIGITAL(PM2IdleHour , m_nPM2Hour , &nIOStatus);
			WRITE_DIGITAL(PM2IdleMin  , m_nPM2Min  , &nIOStatus);
			_LOG("Restore Idle Time PM2 %d , %d" , m_nPM2Hour , m_nPM2Min);
		} else {
			WRITE_DIGITAL(PM2IdleHour , 0  , &nIOStatus);
			WRITE_DIGITAL(PM2IdleMin  , 1  , &nIOStatus);
			_LOG("Abnormally Restore Idle Time PM2 0 , 1");
		}

		//Restore PM3
		if (m_nPM3Hour >= 0 && m_nPM3Min >= 0) {
			if (m_nPM3Hour > 1 ) m_nPM3Hour = IDLE_WAIT_HOUR;
			if (m_nPM3Min  > 10) m_nPM3Min  = IDLE_WAIT_TIME;

			WRITE_DIGITAL(PM3IdleHour , m_nPM3Hour , &nIOStatus);
			WRITE_DIGITAL(PM3IdleMin  , m_nPM3Min  , &nIOStatus);
			_LOG("Restore Idle Time PM3 %d , %d" , m_nPM3Hour , m_nPM3Min);
		} else {
			WRITE_DIGITAL(PM3IdleHour , 0   , &nIOStatus);
			WRITE_DIGITAL(PM3IdleMin  , 1   , &nIOStatus);
			_LOG("Abnormally Restore Idle Time PM3 0 , 1");
		}
	} while (0);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetPMSidePrameter
//Date         : 2016.01.19
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::SetPMPrameter()
{
	int  nIOStatus = 0;
	int  i = 0;
	int  nParaValue = 0;
	int  nPMCnt = 0;
	int	 nALCVlvCnt = 0;
	int  nLKMacro1 = 0;
	int  nLKMacro2 = 0;
	double dbParaUpValue = 0;
	double dbParaDnValue = 0;
	double dbParaValue = 0;

	//...Set PM Leak Check Parameter
	for(i = 0; i < MAX_LK_PARAMETERDO; i++)
	{
		nParaValue = READ_DIGITAL(PRMD_PM1_ALECHK_NORM + i, &nIOStatus);
		WRITE_DIGITAL(PM1_ALECHK_NormDM + i, nParaValue, &nIOStatus);
	}

	for(i = 0; i < MAX_LK_PARAMETERAO; i++)
	{
		dbParaValue = READ_ANALOG(PRMA_PM1_BASEPRES + i, &nIOStatus);
		WRITE_ANALOG(PM1_LeakChkBasePressAM + i, dbParaValue, &nIOStatus);
	}


    //...Set Chuck & ALD Valve & Leak Check Macro Type Parameter
	nLKMacro1	  = READ_DIGITAL(PRMD_PMLK_MACRO1, &nIOStatus);
	nLKMacro2	  = READ_DIGITAL(PRMD_PMLK_MACRO2, &nIOStatus);
	nPMCnt		  = READ_DIGITAL(PRMD_CHUCK_CYCNT, &nIOStatus);	
	dbParaUpValue = READ_ANALOG(PRMA_CHUCK_UPPOS, &nIOStatus);
	dbParaDnValue = READ_ANALOG(PRMA_CHUCK_DNPOS, &nIOStatus);
	nALCVlvCnt	  = READ_DIGITAL(PRMD_ALDVLV_CYCNT, &nIOStatus);

	for(i = 0; i < MAX_PM_VERTUAL; i++)
	{
		WRITE_DIGITAL(PM1_LK_MACRO1 + i,		nLKMacro1,	&nIOStatus);
		WRITE_DIGITAL(PM1_LK_MACRO2 + i,		nLKMacro2,	&nIOStatus);
		WRITE_DIGITAL(PM1_ALECHK_DynCntDM + i,   nPMCnt,    &nIOStatus);
		WRITE_ANALOG(PM1_ALDVLV_CntAM + i,	 nALCVlvCnt,	&nIOStatus);
		WRITE_ANALOG(PM1_CHUCK_UPPOS_AM + i, dbParaUpValue, &nIOStatus);
		WRITE_ANALOG(PM1_CHUCK_DNPOS_AM + i, dbParaDnValue, &nIOStatus);

		WRITE_DIGITAL(PM1_PurgeLineLK_DM + i,			 1, &nIOStatus); /*On*/
		WRITE_DIGITAL(PM1_CleanLineLK_DM + i,			 1, &nIOStatus); /*On*/
	}		

}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetLeakCheckParam
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::SetLeakCheckParam()
{
	int  nIOStatus = 0;
	int  nSetItem , nSetItem2;
	do {
		// Pumping Time Setting
		nSetItem  = READ_DIGITAL(PUMP_TIME_HOUR   , &nIOStatus);
		nSetItem2 = READ_DIGITAL(PUMP_TIME_MINUTE , &nIOStatus);
		//TM
		WRITE_DIGITAL(TM_PUMP_TIME_HOUR    , nSetItem  , &nIOStatus);
		WRITE_DIGITAL(TM_PUMP_TIME_MINUTE  , nSetItem2 , &nIOStatus);
		//BM1
		WRITE_DIGITAL(BM1_PUMP_TIME_HOUR   , nSetItem  , &nIOStatus);
		WRITE_DIGITAL(BM1_PUMP_TIME_MINUTE , nSetItem2 , &nIOStatus);
		//BM2
		WRITE_DIGITAL(BM2_PUMP_TIME_HOUR   , nSetItem  , &nIOStatus);
		WRITE_DIGITAL(BM2_PUMP_TIME_MINUTE , nSetItem2 , &nIOStatus);
		_LOG("Set Pump Time %d , %d" , nSetItem , nSetItem2);

		// Pump Wait Time Setting
		nSetItem  = READ_DIGITAL(PUMP_WAIT_HOUR   , &nIOStatus);
		nSetItem2 = READ_DIGITAL(PUMP_WAIT_MINUTE , &nIOStatus);
		//TM
		WRITE_DIGITAL(TM_PUMP_WAIT_HOUR    , nSetItem  , &nIOStatus);
		WRITE_DIGITAL(TM_PUMP_WAIT_MINUTE  , nSetItem2 , &nIOStatus);
		//BM1
		WRITE_DIGITAL(BM1_PUMP_WAIT_HOUR   , nSetItem  , &nIOStatus);
		WRITE_DIGITAL(BM1_PUMP_WAIT_MINUTE , nSetItem2 , &nIOStatus);
		//BM2
		WRITE_DIGITAL(BM2_PUMP_WAIT_HOUR   , nSetItem  , &nIOStatus);
		WRITE_DIGITAL(BM2_PUMP_WAIT_MINUTE , nSetItem2 , &nIOStatus);
		_LOG("Set Pump Wait Time %d , %d" , nSetItem , nSetItem2);

		// Leak Check Count Setting
		nSetItem  = READ_DIGITAL(LEAK_CHECK_COUNT , &nIOStatus);
		//TM
		WRITE_DIGITAL(TM_LEAK_CHECK_COUNT  , nSetItem , &nIOStatus);
		//BM1
		WRITE_DIGITAL(BM1_LEAK_CHECK_COUNT , nSetItem , &nIOStatus);
		//BM2
		WRITE_DIGITAL(BM2_LEAK_CHECK_COUNT , nSetItem , &nIOStatus);
		_LOG("Set Leak Check Count %d" , nSetItem);

		//... 2015.01.19 Parameter Reading & Writing (PM Side IO)
		SetPMPrameter();
		
	} while (0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : ChangeControlState
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::ChangeControlState(int nState)
{
	int nIOStatus = 0;
	//TM
	WRITE_DIGITAL(TM_CONTROL       , nState  , &nIOStatus);
	//BM1
	WRITE_DIGITAL(BM1_CONTROL      , nState  , &nIOStatus);
	//BM2
	WRITE_DIGITAL(BM2_CONTROL      , nState  , &nIOStatus);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoLotAbort
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::DoLotAbort()
{
	char schdBuffer[128];
	memset (schdBuffer , 0, sizeof(schdBuffer));
	sprintf(schdBuffer , "ABORT_F PORT1");
	WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);

	memset (schdBuffer , 0, sizeof(schdBuffer));
	sprintf(schdBuffer , "ABORT_F PORT2");
	WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);

	memset (schdBuffer , 0, sizeof(schdBuffer));
	sprintf(schdBuffer , "ABORT_F PORT3");
	WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoFunctionAbort
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::DoFunctionAbort()
{
	int nIOStatus;

	//Close All Over Pressure Valve & Vac function abort		
	for (int i = 0 ; i < FUNCTION_COUNT ; i++) {
		RUN_FUNCTION(g_sFuncTable[i].nOverPresFuncIndex , g_sFuncTable[i].szClOverPresCmd);
		RUN_FUNCTION_ABORT(g_sFuncTable[i].nVacFuncIndex);
	}

	//PM ABort
	for(i=0 ; i<PM_COUNT ; i++){

		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM1+i, &nIOStatus)){
			if(__SEQUENCE_RUNNING == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nIOStatus)){
				if( 2 /* Dry Cleaning */ != READ_DIGITAL(PM1_PRO_ChamberStatus+i, &nIOStatus)){
					if( __PURGE_RUNNING == READ_DIGITAL(PM1_PRO_PurgeRcpStsDM+i, &nIOStatus)){
						RUN_FUNCTION_ABORT(SCHEDULER_MAINT_PM1+i);
						_LOG("PM%d Function Abort", i+1);				
					}	
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : 
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::EXIT_BY_EXCEPTION(char* szFuncName)
{
	CLeakCheckCtrlAbort AbortInfo;

	if (NULL != szFuncName) {
		sprintf(AbortInfo.m_szAbortInfo , "Auto Leak Check Control::Function Escape from %s due to exception" , szFuncName);
		_LOG("Auto Leak Check EXIT from %s due to Exception" , szFuncName);
	}

	throw AbortInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : EXIT_IF_ABORT 
//Date         : 2008.04.03
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::EXIT_IF_ABORT(char* szFuncName)
{
	//Check Abort Flag
	int nIOStatus;
	int nAbortFlag = NORMAL;
	nAbortFlag = READ_DIGITAL(LeakCheckAbortSts , &nIOStatus);
	if (ABORTED == nAbortFlag) {
		CLeakCheckCtrlAbort AbortInfo;
		if (NULL != szFuncName) {
			sprintf(AbortInfo.m_szAbortInfo , "Auto Leak Check Control::Function Escape from %s due to Abort by User" , szFuncName);
			_LOG("Auto Leak Check EXIT from %s due to Abort by User" , szFuncName);
		}
		throw AbortInfo;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DOPMLeakCheckStart 
//Date         : 2012.11.13
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::DoPMLeakCheckStart(char* szFuncName)
{
	int  nIOStatus;
	//char szPMRecipe[256];

	//memset(szPMRecipe, 0, sizeof(szPMRecipe));

	for(int i=0 ; i<PM_COUNT ; i++)
	{
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM1+i, &nIOStatus))
		{
			if(__SEQUENCE_IDLE == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nIOStatus))
			{
				if( 2 /* Dry Cleaning */ != READ_DIGITAL(PM1_PRO_ChamberStatus+i, &nIOStatus))
				{
					if(__ABSENT == READ_DIGITAL(PM1_WaferDI+i, &nIOStatus))
					{
						RUN_SET_FUNCTION(SCHEDULER_MAINT_PM1 + i, "SYSTEM_LEAK_CHECK");
						_LOG("PM%d Leak Check Start : %s by %s", i+1, "SYSTEM_LEAK_CHECK", szFuncName);
						//memset(szPMRecipe, 0, sizeof(szPMRecipe));
						WRITE_DIGITAL(PM1_SysLeakChkDM + i,1,&nIOStatus);
					}
					else _LOG("PM%d Wafer Present so cannot Auto Leak Check", i+1);	
				}else _LOG("PM%d Dry Cleaning Recipe Running", i+1);		
			}else _LOG("PM%d Already Recipe Running", i+1);
		}else _LOG("PM%d status is Not Enable", i+1);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::DoPMLotPreStart(char* szFuncName)
{
	int  nIOStatus;
	char szPMRecipe[256];
	
	memset(szPMRecipe, 0, sizeof(szPMRecipe));
	
	for(int i=0 ; i<PM_COUNT ; i++)
	{
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM1+i, &nIOStatus))
		{
			if(__SEQUENCE_IDLE == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nIOStatus))
			{
				if( 2 /* Dry Cleaning */ != READ_DIGITAL(PM1_PRO_ChamberStatus+i, &nIOStatus))
				{
					if(__ABSENT == READ_DIGITAL(PM1_WaferDI+i, &nIOStatus))
					{
						READ_STRING(PM1_LC_RECIPE_FILE	+ i, szPMRecipe, &nIOStatus);
						WRITE_STRING(PM1_RECIPE_FILE	+ i, szPMRecipe, &nIOStatus);
						RUN_SET_FUNCTION(SCHEDULER_MAINT_PM1 + i, "PROCESS");
						_LOG("PM%d Leak Check Recipe Start : %s by %s", i+1, szPMRecipe, szFuncName);
						memset(szPMRecipe, 0, sizeof(szPMRecipe));
						
						//... Lot Pre Skip Timer Flag On
						WRITE_DIGITAL(PM1_SysLeakChkDM + i,1,&nIOStatus);

						m_nPM_LeakChk[i] = enPM_LC_RUN;
					}
					else _LOG("PM%d Wafer Present so cannot Auto Leak Check", i+1);	
				}else _LOG("PM%d Dry Cleaning Recipe Running", i+1);		
			}else _LOG("PM%d Already Recipe Running", i+1);
		}else _LOG("PM%d status is Not Enable", i+1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPMLeakCheckEnd
//Date         : 2012.11.13
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CLeakCheckControl::DoPMLeakCheckEnd(char* szFuncName)
{
	int nCommStatus;
	/*
	for(int i=0 ; i<PM_COUNT ; i++){
		if(1 == READ_DIGITAL(FA_STATUS_MDL_PM1+i, &nCommStatus)){
			if (__SEQUENCE_RUNNING == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nCommStatus)){
				if( 2  != READ_DIGITAL(PM1_PRO_ChamberStatus+i, &nCommStatus)){
					if( __PURGE_RUNNING == READ_DIGITAL(PM1_PRO_PurgeRcpStsDM+i, &nCommStatus)){
						WRITE_DIGITAL(PM1_PRO_PurgeRcpStsDM+i, __PURGE_STOP, &nCommStatus);
					}
				}
			}		
		}
	}
	*/
	
	for(int i = 0 ; i<PM_COUNT ; i++){
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM1+i, &nCommStatus)){
			if (__SEQUENCE_RUNNING == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nCommStatus)){
				if( 2 /* Dry Cleaning */ != READ_DIGITAL(PM1_PRO_ChamberStatus+i, &nCommStatus)){
					//WRITE_DIGITAL(PM1_PRO_PurgeRcpStsDM+i, __PURGE_STOP, &nCommStatus);
					do
					{
						if (FALSE == WAIT_SECONDS(WAIT_TILL_PERIOD))
						{
							_LOG("%s : Waiting module idle aborted by user", szFuncName);
							break;
						}
						EXIT_IF_ABORT("DoPMLeakCheckEnd");
					} while(__SEQUENCE_RUNNING == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nCommStatus));
					_LOG("PM%d Post Recipe Check Success : %s", i+1, szFuncName);
					if (i == 2) Sleep(5000);		//Manual Log 
				}
			}  else _LOG("PM%d Post Recipe Check Process End : %s", i+1, szFuncName);		
		} else _LOG("PM%d Post Recipe Check PM disable : %s", i+1, szFuncName);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPMIdleStop
//Date         : 2012.11.13
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CLeakCheckControl::DoPMIdleStop(char* szFuncName)
{
	int nCommStatus;

	for(int i=0 ; i<PM_COUNT ; i++)
	{
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM1+i, &nCommStatus)){
			if (__SEQUENCE_RUNNING == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nCommStatus)){
				if( 2 /* Dry Cleaning */ != READ_DIGITAL(PM1_PRO_ChamberStatus+i, &nCommStatus)){
					if( __IDLE_RUNNING == READ_DIGITAL(PM1_PRO_IdleRcpStsDM+i, &nCommStatus))
					{
						WRITE_DIGITAL(PM1_PRO_IdleRcpStsDM+i, __IDLE_STOP, &nCommStatus);
						do
						{
							if (FALSE == WAIT_SECONDS(WAIT_TILL_PERIOD))
							{
								_LOG("%s : Waiting module idle aborted by user", szFuncName);
								break;
							}
							EXIT_IF_ABORT("DoPMIdleStop");
						} while(__SEQUENCE_RUNNING == READ_DIGITAL(PM1_PRO_RunStsDM+i, &nCommStatus));
						_LOG("PM%d idle Stop Success : %s", i+1, szFuncName);
						if (i == 2) Sleep(5000);
					}
				}
			}		
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::WaitLeakCheck(char* szFuncName)
{
	int nCommStatus;
	int nPM1DryPass = 0, nPM2DryPass = 0, nPM3DryPass = 0; 
	int nFailCount = 0;
	
	_LOG("PM Leak Check Wait Start..");
	
	do{
		// PM1 Dry Pass Status
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM1, &nCommStatus)){
			if(SYS_RUNNING == READ_FUNCTION(SCHEDULER_MAINT_PM1)){
				nPM1DryPass = 0;
			}else nPM1DryPass = 1;
		}else nPM1DryPass = 1;
		// PM1 Dry Pass Status
		
		Sleep(100);
		
		// PM2 Dry Pass Status
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM2, &nCommStatus)){
			if(SYS_RUNNING == READ_FUNCTION(SCHEDULER_MAINT_PM2)){
				nPM2DryPass = 0;
			}else nPM2DryPass = 1;
		}else nPM2DryPass = 1;
		// PM2 Dry Pass Status
		
		Sleep(100);
		
		// PM3 Dry Pass Status
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM3, &nCommStatus)){
			if(SYS_RUNNING == READ_FUNCTION(SCHEDULER_MAINT_PM3)){
				nPM3DryPass = 0;
			}else nPM3DryPass = 1;
		}else nPM3DryPass = 1;
		// PM3 Dry Pass Status
		
		if(nPM1DryPass == 1 && nPM2DryPass == 1 && nPM3DryPass ==1){
			_LOG("All PM Leak Check Pass : %s", szFuncName);
			nFailCount = 0;
			break;
		}
		
		if (!WAIT_SECONDS(1)) EXIT_BY_EXCEPTION("DoLeakCheckPre");
		
		nFailCount += 1;
		printf("Auto PM Leak Check Wait..%d\n",nFailCount);
		if (nFailCount > DRY_WAIT_TIMEOVER*60) {
			_LOG("Auto PM Leak Check Wait Timeout Over..");
			ResetAccIdleTime  ();
			ResetAccLotCount  ();
			ResetAccWaferCount();
			EXIT_BY_EXCEPTION("DoLeakCheckPre");
		}		
	}while(1);
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPMDryWait
//Date         : 2013.03.04
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CLeakCheckControl::DoPMDryWait(char* szFuncName)
{
	int nCommStatus;
	int nPM1DryPass = 0, nPM2DryPass = 0, nPM3DryPass = 0; 
	int nFailCount = 0;

	_LOG("Dry Cleaning Wait Start..");

	do{
		// PM1 Dry Pass Status
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM1, &nCommStatus)){
			if(__SEQUENCE_RUNNING == READ_DIGITAL(PM1_PRO_RunStsDM, &nCommStatus)){
				if( 2 /* Dry Cleaning */ == READ_DIGITAL(PM1_PRO_ChamberStatus, &nCommStatus)){
					nPM1DryPass = 0;
				}else nPM1DryPass = 1;
			}else nPM1DryPass = 1;
		}else nPM1DryPass = 1;
		// PM1 Dry Pass Status

		Sleep(100);

		// PM2 Dry Pass Status
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM2, &nCommStatus)){
			if(__SEQUENCE_RUNNING == READ_DIGITAL(PM2_PRO_RunStsDM, &nCommStatus)){
				if( 2 /* Dry Cleaning */ == READ_DIGITAL(PM2_PRO_ChamberStatus, &nCommStatus)){
					nPM2DryPass = 0;
				}else nPM2DryPass = 1;
			}else nPM2DryPass = 1;
		}else nPM2DryPass = 1;
		// PM2 Dry Pass Status

		Sleep(100);

		// PM3 Dry Pass Status
		if(1 /* Enable */ == READ_DIGITAL(FA_STATUS_MDL_PM3, &nCommStatus)){
			if(__SEQUENCE_RUNNING == READ_DIGITAL(PM3_PRO_RunStsDM, &nCommStatus)){
				if( 2 /* Dry Cleaning */ == READ_DIGITAL(PM3_PRO_ChamberStatus, &nCommStatus)){
					nPM3DryPass = 0;
				}else nPM3DryPass = 1;
			}else nPM3DryPass = 1;
		}else nPM3DryPass = 1;
		// PM3 Dry Pass Status

		if(nPM1DryPass == 1 && nPM2DryPass == 1 && nPM3DryPass ==1){
			_LOG("All PM Dry Cleaning Pass : %s", szFuncName);
			nFailCount = 0;
			break;
		}
		
		if (!WAIT_SECONDS(1)) EXIT_BY_EXCEPTION("DoLeakCheckPre");
		
		nFailCount += 1;
		printf("Auto Leak Check Dry Cleaning Wait..%d\n",nFailCount);
		if (nFailCount > DRY_WAIT_TIMEOVER*60) {
			_LOG("Dry Cleaning Wait Timeout Over..");
			ResetAccIdleTime  ();
			ResetAccLotCount  ();
			ResetAccWaferCount();
			EXIT_BY_EXCEPTION("DoLeakCheckPre");
		}		
	}while(1);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetForceAgain 
//Date         : 2012.10.29
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CLeakCheckControl::SetForceAgainPM()
{
	int nCommStatus;
	char szSetForce[128];

	for(int i = 0; i < PM_COUNT; i++)
	{
		//... Check Include Run PM
		if( 7 /* CTC In Use */ == READ_DIGITAL(PM1_CommStatus+i, &nCommStatus))
		{
			//... Check Leak Check PM
			if(enPM_LC_RUN == m_nPM_LeakChk[i])
			{
				//... Check Already Lot Pre
				if(7 /* Lot Pre */ != READ_DIGITAL(PM1_PRO_RcpTypeDM+i, &nCommStatus) )
				{
					memset(szSetForce, 0, sizeof(szSetForce));
					sprintf(szSetForce, "SET_FORCE_PRE_AGAIN PM%d", i+1);
					WRITE_STRING(SCHEDULER, szSetForce, &nCommStatus);

					if(0 == nCommStatus)	_LOG("PM%d Set Force Again Failed : %s", i+1, szSetForce);
					else					_LOG("PM%d Set Force Again : %s", i+1, szSetForce);
					
					Sleep(1000);	//... For Scheduler Interval
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : Equipment Wafer Check 
//Date         : 2014.11.20
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CLeakCheckControl::EquipWaferCheck()
{
	int nCommStatus;
			
	for(int i=0 ; i<21 ; i++)
	{
		if(READ_DIGITAL(CTC_FM_AL_Wafer_Status+i, &nCommStatus) > 0)
		{
			_LOG("EquipWaferCheck::Equipment Wafer Check Fail");
			return FALSE;		
		}
	}
	
	return TRUE;
}

int 
CLeakCheckControl::GetAccLotCount()
{
	return m_nAccLotCount;
}

int 
CLeakCheckControl::GetAccWaferCount()
{
	return m_nAccWaferCount;
}

int 
CLeakCheckControl::GetAccIdleTime()
{
	return m_nAccIdleTime;
}

void 
CLeakCheckControl::SetAccLotCount(int nLotCount)
{
	int nIOStatus;
	m_nAccLotCount = nLotCount;

	//누적 Lot Count 변동으로 File Save필요
	WRITE_DIGITAL(ALC_LOGGING  , 0              , &nIOStatus);
	WRITE_DIGITAL(ACC_ALC_LOT  , m_nAccLotCount , &nIOStatus);	
	WRITE_DIGITAL(ALC_LOGGING  , 1              , &nIOStatus);
}

void 
CLeakCheckControl::SetAccWaferCount(int nWaferCount)
{
	int nIOStatus;
	m_nAccWaferCount = nWaferCount;

	//누적 Wafer Count 변동으로 File Save필요
	WRITE_DIGITAL(ALC_LOGGING    , 0                , &nIOStatus);
	WRITE_DIGITAL(ACC_ALC_WAFER  , m_nAccWaferCount , &nIOStatus);	
	WRITE_DIGITAL(ALC_LOGGING    , 1                , &nIOStatus);
}

void 
CLeakCheckControl::SetAccIdleTime(int nIdleTime)
{
	int nIOStatus;

	m_nAccIdleTime = nIdleTime;
	WRITE_DIGITAL(ACC_ALC_IDLETIME    , m_nAccIdleTime    , &nIOStatus);
}

void 
CLeakCheckControl::ResetAccLotCount()
{
	int nIOStatus;
	m_nAccLotCount = 0;

	//누적 Lot Count 변동으로 File Save필요
	WRITE_DIGITAL(ALC_LOGGING  , 0              , &nIOStatus);
	WRITE_DIGITAL(ACC_ALC_LOT  , m_nAccLotCount , &nIOStatus);	
	WRITE_DIGITAL(ALC_LOGGING  , 1              , &nIOStatus);
}

void 
CLeakCheckControl::ResetAccWaferCount()
{
	int nIOStatus;
	m_nAccWaferCount = 0;

	//누적 Wafer Count 변동으로 File Save필요
	WRITE_DIGITAL(ALC_LOGGING    , 0                , &nIOStatus);
	WRITE_DIGITAL(ACC_ALC_WAFER  , m_nAccWaferCount , &nIOStatus);	
	WRITE_DIGITAL(ALC_LOGGING    , 1                , &nIOStatus);
}

void 
CLeakCheckControl::ResetAccIdleTime()
{
	int nIOStatus;
	m_nAccIdleTime = 0;
	WRITE_DIGITAL(ACC_ALC_IDLETIME    , m_nAccIdleTime    , &nIOStatus);
}

BOOL 
CLeakCheckControl::IsAnyLotRunning()
{
	BOOL bRet = TRUE;
	int  nCMStatus , nIOStatus;
	do {
		nCMStatus = READ_DIGITAL(MAIN_CONTROL  , &nIOStatus );			
		if (nCMStatus == Running || nCMStatus == Waiting || nCMStatus ==  Aborting ) break; 

		nCMStatus = READ_DIGITAL(MAIN_CONTROL2  , &nIOStatus );			
		if (nCMStatus == Running || nCMStatus == Waiting || nCMStatus ==  Aborting) break; 

		nCMStatus = READ_DIGITAL(MAIN_CONTROL3  , &nIOStatus );			
		if (nCMStatus == Running || nCMStatus == Waiting || nCMStatus ==  Aborting) break;

		bRet = FALSE;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : Thread_CountAccumlate
//Date         : 2012.03.22
//Description  : Leak Check에 필요한 Count를 관리하는 쓰레드
//Arguments    :
//Return Value : DWORD
//////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CLeakCheckControl::Thread_CountAccumlate(LPVOID pVoid)
{
	BOOL bRet        = FALSE;
	BOOL bModeChange = FALSE;
	CLeakCheckControl* pLeakCheckCtrl = (CLeakCheckControl*)pVoid;	
	//Initial Sleep
	Sleep(INIT_WAIT_TIME);

	//Count관리 변수 선언
	int nMode       , nControl , nIOStatus;
	int nLotCount   , nLotCountTemp;
	int nWaferCount , nWaferCountTemp;
	int nIdleTime   , nIdleTimeTemp;

	int nGap , nNewCount;

	//초기 값 Load
	nLotCount  = READ_DIGITAL(ACC_ALC_LOT   , &nIOStatus);
	nWaferCount= READ_DIGITAL(ACC_ALC_WAFER , &nIOStatus);
	pLeakCheckCtrl->SetAccLotCount  (nLotCount);
	pLeakCheckCtrl->SetAccWaferCount(nWaferCount);
	pLeakCheckCtrl->SetAccIdleTime  (0); //<-- Idle Time은 저장해 둘 필요가 없다.


	//초기 Count값 
	nLotCount = READ_DIGITAL(CM1_LOT_COUNT , &nIOStatus);
	nLotCount = nLotCount + READ_DIGITAL(CM2_LOT_COUNT , &nIOStatus);
	nLotCount = nLotCount + READ_DIGITAL(CM3_LOT_COUNT , &nIOStatus);

	nWaferCount = READ_DIGITAL(CM1_WAFER_COUNT , &nIOStatus);
	nWaferCount = nWaferCount + READ_DIGITAL(CM2_WAFER_COUNT , &nIOStatus);
	nWaferCount = nWaferCount + READ_DIGITAL(CM3_WAFER_COUNT , &nIOStatus);

	nIdleTime = nIdleTimeTemp = 0;
	
	nMode = READ_DIGITAL(START_MODE , &nIOStatus);
	while(TRUE) {
		//infinite state check routine
		Sleep(CHECK_TILL_PERIOD);	
		
		nControl = READ_DIGITAL(AUTO_LEAKCHECK_USE , &nIOStatus);
		if (nControl != USE) {
		
			Sleep(5000);
			continue;
		}    

		nMode = READ_DIGITAL(START_MODE , &nIOStatus);
		
		if      (TRUE == pLeakCheckCtrl->IsAnyLotRunning()) {	
			//Idle 누적은 Reset한다.
			pLeakCheckCtrl->ResetAccIdleTime();

			nLotCountTemp = READ_DIGITAL(CM1_LOT_COUNT , &nIOStatus);
			nLotCountTemp = nLotCountTemp + READ_DIGITAL(CM2_LOT_COUNT , &nIOStatus);
			nLotCountTemp = nLotCountTemp + READ_DIGITAL(CM3_LOT_COUNT , &nIOStatus);

			if ((nLotCount != nLotCountTemp) && (nLotCountTemp > nLotCount)) {
				nGap      = nLotCountTemp - nLotCount;
				nNewCount = pLeakCheckCtrl->GetAccLotCount();
				nNewCount = nNewCount + nGap;
				pLeakCheckCtrl->SetAccLotCount(nNewCount);

				nLotCount = nLotCountTemp;
			}
			
			nWaferCountTemp = READ_DIGITAL(CM1_WAFER_COUNT , &nIOStatus);
			nWaferCountTemp = nWaferCountTemp + READ_DIGITAL(CM2_WAFER_COUNT , &nIOStatus);
			nWaferCountTemp = nWaferCountTemp + READ_DIGITAL(CM3_WAFER_COUNT , &nIOStatus);

			if ((nWaferCount != nWaferCountTemp) && (nWaferCountTemp > nWaferCount)) {
				nGap = nWaferCountTemp - nWaferCount;
				nNewCount = pLeakCheckCtrl->GetAccWaferCount();
				nNewCount = nNewCount + nGap;
				pLeakCheckCtrl->SetAccWaferCount(nNewCount);

				nWaferCount = nWaferCountTemp;
			}
		}
		else if (IDLETIME   == nMode && FALSE == pLeakCheckCtrl->IsAnyLotRunning()) {
			//Sleep이 1초 니깐 60번 돌면 Count를 하나씩 증가시킨다.
			nIdleTimeTemp++;
			if (nIdleTimeTemp >= 60) {
				nIdleTime = pLeakCheckCtrl->GetAccIdleTime();
				nIdleTime++;
				pLeakCheckCtrl->SetAccIdleTime(nIdleTime);

				nIdleTimeTemp = 0;
			}
		}		
	};

	return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//... 2016.04.04
//... Do Clean Recipe
//... If source Thickness is Zero, clean Recipe Skip 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLeakCheckControl::DoCleanBefLeakCheck()
{
	int		nIOStatus;
	int		i = 0;
	char	szPMRecipe[256] = {"0",};

	Sleep(1000);

	_LOG("Auto Leak Check Activated.. : DoCleanBefLeakCheck...");
	printf("Auto Leak Check Activated.. : DoCleanBefLeakCheck...\n");

	for(i = 0 ; i<PM_COUNT ; i++)
	{		
		if(0 != (int)READ_ANALOG(PRMA_PM1_DRYCThick+i,&nIOStatus)){									//... Source Thickness Check
			if(PMCOMM_STANDBY == READ_DIGITAL(PM1_CommStatus+i,&nIOStatus)){						//... Check PM CommStatus
				if(enSTS_RUNNING != READ_DIGITAL(PRMD_PM1_AutoCycRun+i,&nIOStatus)					//... Check Auto Cycle Clean Recipe Running
					||enSTS_RESERVE != READ_DIGITAL(PRMD_PM1_AutoCycRun+i,&nIOStatus)){	
					if( 2 /* Dry Cleaning */ != READ_DIGITAL(PM1_PRO_ChamberStatus+i, &nIOStatus)){ //... Check Dry Clean Recipe Running
						if(__ABSENT == READ_DIGITAL(PM1_WaferDI+i, &nIOStatus))
						{
							READ_STRING(PM1_CLN_RECIPE_FILE	+ i, szPMRecipe, &nIOStatus);
							WRITE_STRING(PM1_RECIPE_FILE	+ i, szPMRecipe, &nIOStatus);
							RUN_SET_FUNCTION(SCHEDULER_MAINT_PM1 + i, "PROCESS");
							_LOG("PM%d Clean Recipe Start : %s by %s", i+1, szPMRecipe, "DoCleanBefLeakCheck");
							memset(szPMRecipe, 0, sizeof(szPMRecipe));
						}
						else _LOG("PM%d Wafer Present so cannot Run Clean Recipe", i+1);	
					}else _LOG("PM%d Dry Cleaning Recipe Running", i+1);
				}else _LOG("PM%d AutoCycle Clean Recipe Running", i+1);
			}else  _LOG("PM%d status is Not Enable", i+1);
		}else   _LOG("PM%d Source Thickness Is Zero", i+1);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : 
//Date         : 2008.04.03
//Description  : State Polling & State Transition
//Arguments    :
//Return Value : DWORD
//////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CLeakCheckControl::Thread_LeakCheckCtrl(LPVOID pVoid)
{
	BOOL bRet = FALSE;
	CLeakCheckControl* pLeakCheckCtrl = (CLeakCheckControl*)pVoid;	
	//Initial Sleep
	Sleep(INIT_WAIT_TIME);

	while(TRUE) 
	{
		//infinite state check routine
		Sleep(CHECK_TILL_PERIOD);

		if (FALSE == pLeakCheckCtrl->IsReadyLeakCheck())    continue;
		if (FALSE == pLeakCheckCtrl->PrepareForLeakCheck()) continue;
		if (FALSE == pLeakCheckCtrl->DoLeakCheckPre())      continue;
		if (FALSE == pLeakCheckCtrl->DoLeakCheck())         continue;
		if (FALSE == pLeakCheckCtrl->IsCompleteLeakCheck()) continue;
		if (FALSE == pLeakCheckCtrl->DoLeakCheckPost())     continue;
		if (FALSE == pLeakCheckCtrl->IsVerify())            continue;
	};

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsReadyLeakCheck
//Date         : 2008.04.02
//Description  : 
//Arguments    :
//Return Value : Boolean
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::IsReadyLeakCheck()
{
	BOOL bRet = FALSE;
	SYSTEMTIME systime;
	int    nIOStatus      = 0;
	int    nControl       = NOTUSE;
	int    nMode          = DAILY;
	int    nYear          = 0;
	int    nDay           = 0;
	int    nWeek		  = 0;
	DWORD  dwTargetSec    = 0;
	DWORD  dwCurSec       = 0;

	double dbTotalDay , dbCurTotalDay;
	int    i = 0;

	int    nStartCount;
	int    nGEMSts;

	try {		
		while (1) {
			//printf("-----> IsReadyLeakCheck... \n");
			//1. Use , Nout Use Check
			nControl = READ_DIGITAL(AUTO_LEAKCHECK_USE , &nIOStatus);
            nGEMSts  = READ_DIGITAL(GEM_Control_State  , &nIOStatus);

			if (nControl != USE || nGEMSts != 4) {	// 4 = REMOTE
				//Auto Leak Check is not activated
				//printf ("Leak Check State Transition \n");
				Sleep(1000);
				break;
			} 
			else {
				//printf ("Leak Check State Transition On \n");
			}

			//2. Auto Leak Check Start Time Check
			GetLocalTime(&systime);

			nMode = READ_DIGITAL(START_MODE , &nIOStatus);
			if (WEEKLY == nMode) {
				//Skip Time check when mismatched week day
				nWeek = READ_DIGITAL(START_TIME_SWEEK , &nIOStatus);
				if ((nWeek+1) != systime.wDayOfWeek) {
					Sleep(1000);
					break;
				}
			} else if (MONTHLY == nMode) {
				//Skip time check when mismatched date
				nDay = READ_DIGITAL(START_TIME_SDAY , &nIOStatus);
				if (nDay != systime.wDay) {
					Sleep(1000);
					break;
				}
			} else if (DAYPERIOD == nMode) {
				//현재 시간 총 날짜 계산
				dbCurTotalDay = systime.wYear * 365;
				for (i = 0 ; i < systime.wMonth ; i++)
					dbCurTotalDay += g_nMdays[i];

				dbCurTotalDay += systime.wDay;
				
				//마지막 저장 시간 
				dbTotalDay = READ_DIGITAL(LAST_ALC_YEAR , &nIOStatus) * 365;
				for (i = 0 ; i < READ_DIGITAL(LAST_ALC_MONTH , &nIOStatus) ; i++)
					dbTotalDay += g_nMdays[i];

				dbTotalDay += READ_DIGITAL(LAST_ALC_DAY , &nIOStatus);

				if (fabs(dbCurTotalDay - dbTotalDay) < READ_DIGITAL(START_DAY_PERIOD , &nIOStatus))
				{
					//printf ("Day Period Not yet Reached %f \n" , dbCurTotalDay - dbTotalDay);
					Sleep(1000);
					break;
				}
				else
				{
					//printf ("Now Day Period Leak Check Started \n");

				}

			} else if (LOTCOUNT == nMode) {
				nStartCount = READ_DIGITAL(START_LOT_COUNT , &nIOStatus);
				if (GetAccLotCount() < nStartCount) break;

			} else if (WAFERCOUNT == nMode) {
				nStartCount = READ_DIGITAL(START_WAFER_COUNT , &nIOStatus);
				if (GetAccWaferCount() < nStartCount) break;

			} else if (IDLETIME == nMode) {
				nStartCount = READ_DIGITAL(START_IDLE_TIME , &nIOStatus);
				if (GetAccIdleTime() < nStartCount) break;

			}
			
			else {
				// Go Next Step
			}

			if (nMode != LOTCOUNT && nMode != WAFERCOUNT && nMode != IDLETIME) {
				dwCurSec    = (systime.wHour * 3600) + (systime.wMinute * 60);
				dwTargetSec = (READ_DIGITAL(START_TIME_HOUR , &nIOStatus) * 3600 + READ_DIGITAL(START_TIME_MINUTE , &nIOStatus) * 60);
				if (abs(dwCurSec - dwTargetSec) > IN_TIME_SECOND) break;
				else {
					printf ("Auto Leak Check Activated.. \n");
					_LOG("...");
					_LOG("Auto Leak Check Activated.. : IsReadyLeakCheck");
				}
			}
			else{
				printf ("Auto Leak Check Activated.. \n");
				_LOG("...");
				_LOG("Auto Leak Check Activated.. : IsReadyLeakCheck");
			}

			//3. Exception Check
			if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("IsReadyLeakCheck");

			//4. Set Auto Leak Check Status to Reserved
			WRITE_DIGITAL(LeakCheckSts , RESERVED , &nIOStatus);

			//5. Aborted Check
			EXIT_IF_ABORT("IsReadyLeakCheck");

			bRet = TRUE; break;
		}
	} catch (CLeakCheckCtrlAbort AbortInfo) {
		printf ("%s \n" , AbortInfo.m_szAbortInfo);
		//Write Leak Check Status to READY
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);
		//ALARM_POST(ALARM_AUTOLEAKCHECK_ABORT);
		bRet = FALSE;	
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PrepareForLeakCheck
//Date         : 2008.04.02
//Description  : 
//Arguments    :
//Return Value : Boolean
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::PrepareForLeakCheck()
{
	BOOL bRet = FALSE;
	int  nIOStatus = 0;
	int  nCM1Status , nCM2Status , nCM3Status;
	char schdBuffer[128];
	BOOL bCM1Run , bCM2Run , bCM3Run;
	bCM1Run = bCM2Run = bCM3Run = FALSE;

	try {
			//... 2016.01.06 Add PM LeakCheck & Lot Pre Flag
			for (int i = 0; i < PM_COUNT; i++)		m_nPM_LeakChk[i] = enPM_LC_NONE;

			memset(schdBuffer , 0, sizeof(schdBuffer));
			nCM1Status = READ_DIGITAL(MAIN_CONTROL  , &nIOStatus );
			nCM2Status = READ_DIGITAL(MAIN_CONTROL2 , &nIOStatus );
			nCM3Status = READ_DIGITAL(MAIN_CONTROL3 , &nIOStatus );

			// Aborted Check
			EXIT_IF_ABORT("PrepareForLeakCheck");

			//CM1 Lot Pause
			if (nCM1Status == Waiting) {
				sprintf(schdBuffer , "PAUSE_F PORT1");
				WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
				
			} else if (nCM1Status == Running) {
				bCM1Run = TRUE;
			}

			//CM2 Lot Pause
			if (nCM2Status == Waiting) {
				memset(schdBuffer , 0, sizeof(schdBuffer));
				sprintf(schdBuffer , "PAUSE_F PORT2");
				WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
			} else if (nCM2Status == Running) {
				bCM2Run = TRUE;
			}

			//CM3 Lot Pause
			if (nCM3Status == Waiting) {
				memset(schdBuffer , 0, sizeof(schdBuffer));
				sprintf(schdBuffer , "PAUSE_F PORT3");
				WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
			} else if (nCM3Status == Running) {
				bCM3Run = TRUE;
		}
	
		//BGChecker Activate
		RunBGChecker(bCM1Run , bCM2Run , bCM3Run);

		//Check & Wait Till All Lot Paused or Idle
		while (1) {
//			printf("-----> PrepareForLeakCheck... \n");
			nCM1Status = READ_DIGITAL(MAIN_CONTROL  , &nIOStatus );
			nCM2Status = READ_DIGITAL(MAIN_CONTROL2 , &nIOStatus );
			nCM3Status = READ_DIGITAL(MAIN_CONTROL3 , &nIOStatus );
			//CM1 Check
			if ((nCM1Status != Running) && (nCM1Status != Pausing) && (nCM1Status != Aborting) && (nCM1Status != Waiting)) {
				if ((nCM2Status != Running) && (nCM2Status != Pausing) && (nCM2Status != Aborting) && (nCM2Status != Waiting)) {
					if ((nCM3Status != Running) && (nCM3Status != Pausing) && (nCM3Status != Aborting) && (nCM3Status != Waiting)) {
						bRet = TRUE; break;
					}
				}
			}

			if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("PrepareForLeakCheck");

			// Aborted Check
			EXIT_IF_ABORT("PrepareForLeakCheck");
		}
	} catch (CLeakCheckCtrlAbort AbortInfo) {
		printf ("%s \n" , AbortInfo.m_szAbortInfo);
		//Lot Abort
		DoLotAbort();
		//Kill BGChecker
		KillBGChecker();
		//Write Leak Check Status to READY
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);
		ALARM_POST(ALARM_AUTOLEAKCHECK_ABORT);
		bRet = FALSE;	
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoLeakCheckPre
//Date         : 2008.04.02
//Description  : 
//Arguments    :
//Return Value : Boolean
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::DoLeakCheckPre()
{
	BOOL bRet = FALSE;
	int  nIOStatus = 0;
	int  nPMPumpWaitTime = 0;

	try {
		//Status Update
//		printf("-----> DoLeakCheckPre... \n");

		if (FALSE == EquipWaferCheck())
		{
			printf("Auto Leak Check:: Wafer Check Failed\n");
			Sleep(INIT_WAIT_TIME*1000);
			EXIT_BY_EXCEPTION("DoLeakCheckPre");
		}

		//Control Disable
		ChangeControlState(DISABLE);

		// Aborted Check
		EXIT_IF_ABORT("DoLeakCheckPre");

		//TM , BM1 , BM2 Leak Check IO Setting
		SetLeakCheckParam();

		// Aborted Check
		EXIT_IF_ABORT("DoLeakCheckPre");

		WRITE_DIGITAL(LeakCheckSts , RUNNING , &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		//... 2016.04.04 
		//... Clean Recipe Running Before System Leak Check Run

		SetIdleRunTime(IDLE_RUNTIME_HOUR , IDLE_RUNTIME_MIN);
		Sleep(1000);	//... 2016.05.09
		DoPMIdleStop("DoLeakCheckPre");
		Sleep(1000);
		DoCleanBefLeakCheck();

		//////////////////////////////////////////////////////////////////////////

		//1. TM/BM Site Pumping Step by Step
		_LOG("DoLeakCheckPre : TM_BM Pumping Start.");
		for (int i = 0 ; i < FUNCTION_COUNT ; i++) {
			if (SYS_SUCCESS != RUN_FUNCTION(g_sFuncTable[i].nVacFuncIndex , g_sFuncTable[i].szPumpCmd))
				EXIT_BY_EXCEPTION("DoLeakCheckPre");
		}
		_LOG("DoLeakCheckPre : TM_BM Pumping End.");
		/* Dry Clean & Idle Time Reserve Delete */
		// 2. PM Dry Cleaning Wait Part
		//DoPMDryWait("DoLeakCheckPre");
 
		//WRITE_DIGITAL(LeakCheckSts , RUNNING , &nIOStatus);
		
		//Idle Run Time Change
		//SetIdleRunTime(IDLE_RUNTIME_HOUR , IDLE_RUNTIME_MIN);

		//3. PM Idle Recipe Stop
		DoPMIdleStop("DoLeakCheckPre");

		if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("DoLeakCheckPre");
		// Aborted Check
		EXIT_IF_ABORT("DoLeakCheckPre");

		//3. All Site Over Pressure Control Abort Start Simultaneously

		//2012.009.14 TM Over Pressure Option
		//m_nOverPresVlvSts = READ_DIGITAL(TM_OverPresVlv , &nIOStatus);

		for (i = 0 ; i < FUNCTION_COUNT ; i++) 
		{
			RUN_SET_FUNCTION(g_sFuncTable[i].nOverPresFuncIndex , g_sFuncTable[i].szClOverPresCmd);
		}

		if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("DoLeakCheckPre");
		// Aborted Check
		EXIT_IF_ABORT("DoLeakCheckPre");

		//3. Wait Till All Over Pressure Control Function Complete
		for (i = 0 ; i < FUNCTION_COUNT ; i++) 
		{
			if (SYS_SUCCESS != RUN_WAIT_FUNCTION(g_sFuncTable[i].nOverPresFuncIndex))
				EXIT_BY_EXCEPTION("DoLeakCheckPre");
		}

		// Aborted Check
		EXIT_IF_ABORT("DoLeakCheckPre");

		bRet = TRUE;

	} catch (CLeakCheckCtrlAbort AbortInfo) {
		printf ("%s \n" , AbortInfo.m_szAbortInfo);
		//Lot Abort
		DoLotAbort();
		//Function Abort
		DoFunctionAbort();
		//Kill BGChecker
		KillBGChecker();
		//Control Enable
		ChangeControlState(ENABLE);
		//Write Leak Check Status to READY
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);		
		//Restore Idle Time
		RestoreIdleRunTime();
		ALARM_POST(ALARM_AUTOLEAKCHECK_ABORT);
		bRet = FALSE;	
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoLeakCheck
//Date         : 2008.04.02
//Description  : 
//Arguments    :
//Return Value : Boolean
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::DoLeakCheck()
{
	BOOL bRet = FALSE;
	int  nIOStatus = 0;
	SYSTEMTIME systime;

	try {
		//Status Update
//		printf("-----> DoLeakCheck... \n");
		// Aborted Check
		EXIT_IF_ABORT("DoLeakCheck");

		//2011.12.09 by mgsong
		//우선 이곳까지 수행 되었다면 Leak Check Last Time으로 기록한다.
		//Last Leak Check Time Update
		GetLocalTime(&systime);
		WRITE_DIGITAL(ALC_LOGGING    , 0             , &nIOStatus);
		WRITE_DIGITAL(LAST_ALC_YEAR  , systime.wYear , &nIOStatus);
		WRITE_DIGITAL(LAST_ALC_MONTH , systime.wMonth, &nIOStatus);
		WRITE_DIGITAL(LAST_ALC_DAY   , systime.wDay  , &nIOStatus);
		WRITE_DIGITAL(ALC_LOGGING    , 1             , &nIOStatus);
		//2011.12.09 by mgsong	

		//2014.07.20 For MARS
		LogEvent("LL1", "LeakCheck", "Start", "");				
		LogEvent("LL2", "LeakCheck", "Start", "");
		LogEvent("TM1", "LeakCheck", "Start", "");

		//2. TM/BM Site Leak Check Start Simultaneously except Main BM
		_LOG("DoLeakCheck : TM/BM Leak Check Start.");
		for (int i = 0 ; i < FUNCTION_COUNT ; i++)
		{
			RUN_SET_FUNCTION(g_sFuncTable[i].nVacFuncIndex , g_sFuncTable[i].szLeakCheckCmd);			
		}

		if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("DoLeakCheck");

		//..Wait PM Dry Clean
		DoPMDryWait("DoLeakCheckPre");

		Sleep(30000);	//...2016.05.09 Dry Cleaning Check Timing Issue   7 -> 30
		
		//... Add Manual Leak Check & Add MFC Purge Set
		//1. PM Leak Check Start
		DoPMLeakCheckStart("DoLeakCheck");
		
		WaitLeakCheck("DoLeakCheck");

		Sleep(10000);

		//3. PM Lot Pre Recipe Start   //... Clean Recipe Start
		DoPMLotPreStart("DoLeakCheck");
		
		// Aborted Check
		EXIT_IF_ABORT("DoLeakCheck");
		bRet = TRUE;
	} catch (CLeakCheckCtrlAbort AbortInfo) {
		printf ("%s \n" , AbortInfo.m_szAbortInfo);
		//Lot Abort
		DoLotAbort();
		//Function Abort
		DoFunctionAbort();
		//Kill BGChecker
		KillBGChecker();
		//Control Enable
		ChangeControlState(ENABLE);
		//Write Leak Check Status to READY		
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);
		WRITE_DIGITAL(PM1_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM2_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM3_SysLeakChkDM	,	0,&nIOStatus);
		//Restore Idle Time
		RestoreIdleRunTime();
		ALARM_POST(ALARM_AUTOLEAKCHECK_ABORT);
		bRet = FALSE;	
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsCompleteLeakCheck
//Date         : 2008.04.02
//Description  : 
//Arguments    :
//Return Value : Boolean
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::IsCompleteLeakCheck()
{
	BOOL bRet    = FALSE;
	BOOL bEscape = FALSE;
	int  nIOStatus = 0;

	try {
		//1. Wait Till TM/BM Leak Check Control Function Complete
		while (1) {
//			printf("-----> IsCompleteLeakCheck... \n");	
			// Aborted Check
			EXIT_IF_ABORT("IsCompleteLeakCheck");

			bEscape = TRUE;			
			for (int i = 0 ; i < FUNCTION_COUNT ; i++) 
			{
				if (SYS_ABORTED == READ_FUNCTION(g_sFuncTable[i].nVacFuncIndex) || 
					SYS_ERROR == READ_FUNCTION(g_sFuncTable[i].nVacFuncIndex))
				{
					EXIT_BY_EXCEPTION("IsCompleteLeakCheck");
				}
				else if (SYS_RUNNING == READ_FUNCTION(g_sFuncTable[i].nVacFuncIndex))
				{
					bEscape = FALSE;
				}
			}

			if (TRUE == bEscape) {bRet = TRUE;	break;}
			if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("IsCompleteLeakCheck");			
		}

		_LOG("IsCompleteLeakCheck : TM/BM Leak Check Complete.");

		//2. PM Leak Check End
		//DoPMLeakCheckEnd("IsCompleteLeakCheck");
        //... 2016.08.16 Wait Clean Recipe
		Sleep(10000);
		DoPMDryWait("IsCompleteLeakCheck");
		Sleep(10000);
		//관련된 Count를 모두 Reset한다.
		//어차피 Auto Leak Check은 진행이 되었으니...
		//그쪽에서 저장은 저절로 된다.
		ResetAccIdleTime  ();
		ResetAccLotCount  ();
		ResetAccWaferCount();

	} catch (CLeakCheckCtrlAbort AbortInfo) {
		printf ("%s \n" , AbortInfo.m_szAbortInfo);

		ResetAccIdleTime  ();
		ResetAccLotCount  ();
		ResetAccWaferCount();

		DoLotAbort();
		//Function Abort
		DoFunctionAbort();
		//Kill BGChecker
		KillBGChecker();
		//Control Enable
		ChangeControlState(ENABLE);
		//Write Leak Check Status to READY
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);
		WRITE_DIGITAL(PM1_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM2_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM3_SysLeakChkDM	,	0,&nIOStatus);		
		//Restore Idle Time
		RestoreIdleRunTime();

		ALARM_POST(ALARM_AUTOLEAKCHECK_ABORT);
		bRet = FALSE;	
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoLeakCheckPost
//Date         : 2008.04.02
//Description  : 
//Arguments    :
//Return Value : Boolean
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::DoLeakCheckPost()
{
	BOOL bRet = FALSE;
	int  nIOStatus = 0;
	try {

		while (1) 
		{
			// Aborted Check
			EXIT_IF_ABORT("DoLeakCheckPost");

			bRet = TRUE; break;
			if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("DoLeakCheckPost");
		}
	}
	catch (CLeakCheckCtrlAbort AbortInfo) {
		printf ("%s \n" , AbortInfo.m_szAbortInfo);
		DoLotAbort();
		//Function Abort
		DoFunctionAbort();
		//Kill BGChecker
		KillBGChecker();
		//Control Enable
		ChangeControlState(ENABLE);
		//Write Leak Check Status to READY
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);
		WRITE_DIGITAL(PM1_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM2_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM3_SysLeakChkDM	,	0,&nIOStatus);		
		//Restore Idle Time
		RestoreIdleRunTime();

		ALARM_POST(ALARM_AUTOLEAKCHECK_ABORT);
		bRet = FALSE;	
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsVerify
//Date         : 2008.04.02
//Description  : 
//Arguments    :
//Return Value : Boolean
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLeakCheckControl::IsVerify()
{
	BOOL bRet = FALSE;
	int  nIOStatus = 0;
	int  nCM1Status , nCM2Status , nCM3Status;
	char schdBuffer[128];
	int  nPRMD_OVER_PRES_OPT;

	try {
		memset(schdBuffer , 0, sizeof(schdBuffer));	
		//Wait Until All Lot Paused
		while (1) 
		{
			// Aborted Check
			EXIT_IF_ABORT("IsVerify");
			nCM1Status = READ_DIGITAL(MAIN_CONTROL  , &nIOStatus );
			nCM2Status = READ_DIGITAL(MAIN_CONTROL2 , &nIOStatus );
			nCM3Status = READ_DIGITAL(MAIN_CONTROL3 , &nIOStatus );

			if (   (nCM1Status != Pausing) && (nCM2Status != Pausing) && (nCM3Status != Pausing) 
				&& (nCM1Status != Waiting) && (nCM2Status != Waiting) && (nCM3Status != Waiting)) break;

			if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("IsVerify");
		}

		//Kill BGChecker
		KillBGChecker();

		// Aborted Check
		EXIT_IF_ABORT("IsVerify");

		//Control Enable
		ChangeControlState(ENABLE);

		//////////////////////////////////////////////////////////////////////////
		//CM1 Lot Continue
		if (nCM1Status == Paused)
		{
			sprintf(schdBuffer , "CONTINUE_F PORT1");
			WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
		}
		//CM2 Lot Continue
		if (nCM2Status == Paused) 
		{
			memset(schdBuffer , 0, sizeof(schdBuffer));
			sprintf(schdBuffer , "CONTINUE_F PORT2");
			WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
		}
		//CM3 Lot Continue
		if (nCM3Status == Paused) 
		{
			memset(schdBuffer , 0, sizeof(schdBuffer));
			sprintf(schdBuffer , "CONTINUE_F PORT3");
			WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
		}
		//////////////////////////////////////////////////////////////////////////

		//Confirm before idle time restore
		while (1) 
		{
			printf("-----> IsVerify... \n");
			// Aborted Check
			EXIT_IF_ABORT("IsVerify");
			nCM1Status = READ_DIGITAL(MAIN_CONTROL  , &nIOStatus );
			nCM2Status = READ_DIGITAL(MAIN_CONTROL2 , &nIOStatus );
			nCM3Status = READ_DIGITAL(MAIN_CONTROL3 , &nIOStatus );

			if ((nCM1Status != Paused)  && (nCM2Status != Paused) && (nCM3Status != Paused)) break;

			if (!WAIT_SECONDS(WAIT_TILL_PERIOD)) EXIT_BY_EXCEPTION("IsVerify");
		}

 		//2014.08.11 PM Idle Stop Added
 		DoPMIdleStop("IsVerify");

		//2012.09.14 TM Over Pressure Option
		nCM1Status = READ_DIGITAL(MAIN_CONTROL  , &nIOStatus );
		nCM2Status = READ_DIGITAL(MAIN_CONTROL2 , &nIOStatus );
		nCM3Status = READ_DIGITAL(MAIN_CONTROL3 , &nIOStatus );

		nPRMD_OVER_PRES_OPT = READ_DIGITAL(PRMD_OVER_PRES_OPT , &nIOStatus );

		if( (nPRMD_OVER_PRES_OPT == 1) && 
			(nCM1Status == Pausing || nCM1Status == Paused || nCM1Status == Running || nCM1Status == Waiting ||
			 nCM2Status == Pausing || nCM2Status == Paused || nCM2Status == Running || nCM2Status == Waiting ||
			 nCM3Status == Pausing || nCM3Status == Paused || nCM3Status == Running || nCM3Status == Waiting) )
		{
			WRITE_DIGITAL(TM_OverPresVlv , OPEN , &nIOStatus);
			_LOG("IsVerify : Over Pressure Valve Open.");
		}
		else
		{
			//WRITE_DIGITAL(TM_OverPresVlv , CLOSE , &nIOStatus);
			//_LOG("IsVerify : Over Pressure Valve Close.");
		}


		// Sequencial Lot 인 경우에 Lot Pre Recipe가 Lot Continue 와 동시에 Start 된다
		// 이런경우에 SetForceAgain 생략을 위해 15초 대기 시간 가짐 (Scheduler에서 Lotpre 동작 시키면 SetForceAgain Pass..) by hyuk
		if(! WAIT_SECONDS(15))
		{
			EXIT_BY_EXCEPTION("IsVerify");
		}

		//2012.10.29 Lot Pre Issue by hyuk
		//SetForceAgainPM();

		//Restore Idle Run Time
		_LOG("IsVerify : RestroreIdleRunTime Start");
		RestoreIdleRunTime();
		_LOG("IsVerify : RestroreIdleRunTime End");
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);

		_LOG("Auto Leak Check End...");

		//2014.07.20
		LogEvent("LL1", "LeakCheck", "End", "");				
		LogEvent("LL2", "LeakCheck", "End", "");
		LogEvent("TM1", "LeakCheck", "End", "");

		bRet = TRUE;		
	}
	catch (CLeakCheckCtrlAbort AbortInfo) {
		printf ("%s \n" , AbortInfo.m_szAbortInfo);
		DoLotAbort();
		//Function Abort
		DoFunctionAbort();
		//Kill BGChecker
		KillBGChecker();
		//Control Enable
		ChangeControlState(ENABLE);
		//Write Leak Check Status to READY
		WRITE_DIGITAL(LeakCheckSts , READY , &nIOStatus);
		//Flag Reset
		WRITE_DIGITAL(LeakCheckAbortSts , NORMAL , &nIOStatus);
		WRITE_DIGITAL(PM1_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM2_SysLeakChkDM	,	0,&nIOStatus);
		WRITE_DIGITAL(PM3_SysLeakChkDM	,	0,&nIOStatus);		
		//Restore Idle Time
		RestoreIdleRunTime();
		
		ALARM_POST(ALARM_AUTOLEAKCHECK_ABORT);
		bRet = FALSE;	
	}

	return bRet;
}


