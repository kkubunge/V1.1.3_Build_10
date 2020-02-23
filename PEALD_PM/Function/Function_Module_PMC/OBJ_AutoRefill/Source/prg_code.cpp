///////////////////////////////////////////////////////////////////////////////
// AbortProcess

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include "Timer.h"

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextParser.h"
#include "TextLogDll.h"

//------------------------------------------------------------------------------------------

#define APP_NAME		"AutoRefill"

enum { VLV_CLOSE, VLV_OPEN } ;
enum { RVLV_UNKNOWN, RVLV_OPEN, RVLV_CLOSE, RVLV_ERROR} ;
enum { DISABLE, ENABLE } ;
enum { MANUAL, AUTO, LI_MAN } ;
enum { LEVEL50, LEVEL75, PERCENT, PERCENT75 } ;	// 2011.07.14
enum { NORMAL, ALARM};
enum { CRS_ALARM, CRS_NORMAL};
enum { HOver, HLow};
enum { LLow, LOver};
enum { OFFLINE, ONLINE};
enum { OFF, ON};
enum { Idle, Running };																					// VRCP_RunningDI

// RefillStatusDM
// 2010.06.30
enum { RS_NORMAL, RS_CHECK, RS_READY, RS_RUN, RS_STABLE, RS_COMPLETE, RS_ABORT, RS_SKIP, RS_PUMP, RS_HE_CHARGE };

enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };
enum { REFILL_RUN, REFILL_STOP };

//------------------------------------------------------------------------------------------
// Alarm Definition

#define ALARM_DEVICE_NET_COMM_ERROR			6000
#define ALARM_REFILL_CONDITION_NOT_OK		6001
//#define ALARM_PREPARE_REFIL_NOT_OK	    6002  // Not Use
#define ALARM_DO_REFILL_FAILED  			6003
#define ALARM_SOURCE_LEVEL_HIGH  			6004
#define ALARM_PM_INTERLOCK_DETECTED			6005
#define ALARM_REFILL_SYSTEM_DETECTED		6006
//#define WARNING_REFILL_SYSTEM_DETECTED	6007  // Not Use (Add Valve Interlock Alarm)
//#define ALARM_REFILL_PURGE_TIMEOUT  		6008  // Not Use
#define ALARM_REFILL_TIMEOUT  				6009
#define MSG_BOX_REFILL_TIMEOUT				4	  // 2012.04.10 	
//#define ALARM_RELAY_OFF_DETECTED			6010  // Not Use
//#define ALARM_PM_SYSTEM_NOT_OK			6011  // Not Use
#define ALARM_REFILL_ABORTED				6012
#define ALARM_REFILL_FAILED 				6013
#define MSG_BOX_REFILL_FAILED				3     // 2012.04.10	
//#define ALARM_REFILL_NOTSTABLE 				6014
#define ALARM_RECIPE_RUNNING				6016
#define ALARM_REFILL_PUMP_TIMEOUT			6021  // Revolver Add Alarm
#define ALARM_REFILL_NOTREADY				6014
#define ALARM_REFILL_RUNNING				6018
#define ALARM_RVALVE_CHECK					6019
#define ALARM_PUMP_FAILED					6020

// 2011.07.14
#define ALARM_HEREFILL_FAILED			    6021
#define ALARM_SOURCE_LEVEL_LOW				8026

//------------------------------------------------------------------------------------------
#include "DynamicIO.h"						     // 2012.04.10
//------------------------------------------------------------------------------------------
// 2009.12.21
#include "ValveSeq.h"
CValveSeq	g_RefillValveSeq;
CDynamicIO  g_DynamicIO;
//------------------------------------------------------------------------------------------
static enum {
	PRO_MsgBoxDM,								// 2012.04.10
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"NotDefined",		_K_D_IO, 0},		// 2012.04.10
	""
};
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_GbIntlckDM",			_K_D_IO	,	SW_GbIntlckDM,		 0} ,
	{ "SW_ModuleFailDM",		_K_D_IO	,	SW_ModuleFailDM,	 0} ,
	{ "PLC_GbIntlckDI",			_K_D_IO	,	PLC_GbIntlckDI,		 0} ,
	{ "DN_CommStsDI",			_K_D_IO	,	DN_CommStsDI,		 0} ,	
	{ "VRCP_RunningDI",			_K_D_IO	,	VRCP_RunningDI,	     0} ,
	{ "DPRunDI",				_K_D_IO	,	DPRunDI,		     0} ,

	{ "CRSAlarm",				_K_D_IO	,	RefillSysAlmDI,		 0} ,	
	{ "CRS2Alarm",				_K_D_IO	,	RefillSysAlm2DI,	 0} ,					
	{ "CRS3Alarm",				_K_D_IO	,	RefillSysAlm3DI,	 0} ,				
	{ "RefillStatusDM",			_K_D_IO	,	RefillStatusDM,      0} ,
	{ "RefillSrcNoDM",			_K_D_IO	,	RefillSrcNoDM,		 0} ,			

	{ "SrcLvHighLmtDI",			_K_D_IO	,	SrcLvHighLmtDI,      0} ,
	{ "Src2LvHighLmtDI",		_K_D_IO	,	Src2LvHighLmtDI,      0} ,
	{ "Src3LvHighLmtDI",		_K_D_IO	,	Src3LvHighLmtDI,      0} ,

	{ "SrcLvHighDI",			_K_D_IO	,	SrcLvHighDI,         0} ,
	{ "Src2LvHighDI",			_K_D_IO	,	Src2LvHighDI,         0} ,
	{ "Src3LvHighDI",			_K_D_IO	,	Src3LvHighDI,         0} ,

	{ "SrcLvLowDI",				_K_D_IO	,	SrcLvLowDI,          0} ,
	{ "Src2LvLowDI",			_K_D_IO	,	Src2LvLowDI,          0} ,
	{ "Src3LvLowDI",			_K_D_IO	,	Src3LvLowDI,          0} ,

	{ "SrcLvLowLmtDI",			_K_D_IO	,	SrcLvLowLmtDI,       0} ,
	{ "Src2LvLowLmtDI",			_K_D_IO	,	Src2LvLowLmtDI,       0} ,
	{ "Src3LvLowLmtDI",			_K_D_IO	,	Src3LvLowLmtDI,       0} ,

	{ "CRSReady",		    	_K_D_IO	,	RefillReadyDI,       0} ,
	{ "CRS2Ready",			    _K_D_IO	,	RefillReady2DI,       0} ,
	{ "CRS3Ready",			    _K_D_IO	,	RefillReady3DI,       0} ,

	{ "RefillReqDO",			_K_D_IO	,	RefillReqDO,         0} ,
	{ "RefillReq2DO",			_K_D_IO	,	RefillReq2DO,         0} ,
	{ "RefillReq3DO",			_K_D_IO	,	RefillReq3DO,         0} ,

	{ "SrcCanisterAI",			_K_A_IO	,	SrcCanisterAI,       0} ,
	{ "Src2CanisterAI",			_K_A_IO	,	Src2CanisterAI,       0} ,
	{ "Src3CanisterAI",			_K_A_IO	,	Src3CanisterAI,       0} ,

	{ "PRMD_Refill_Opt",		_K_D_IO	,	PRMD_Refill_Opt,     0} ,
	{ "PRMD_Refill_Level",		_K_D_IO	,	PRMD_Refill_Level,   0} ,

	{ "PRMA_TO_Refill_Pump",	_K_A_IO	,	PRMA_TO_RefillPump,  0} ,
	{ "PRMA_TO_Refill",			_K_A_IO	,	PRMA_TO_Refill,      0} ,
	{ "PRMA_SP_RefillPer",		_K_A_IO	,	PRMA_SP_RefillPer,   0} ,

	{ "PRMA_VS_Ref_SrcLow",		_K_A_IO	,	PRMA_VS_Ref_SrcLow,   0} ,			// 2012.07.14

	{ "ABORT_GAS",				_K_F_IO	,	ABORT_GAS,			 0} ,

	{ "V14ValveDO",				_K_D_IO	,	V14ValveDO,				0} ,	//... 2014.07.22 for Refill Fast Close

	""
};

//------------------------------------------------------------------------------------------
typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int g_nRefill_Thread = THREAD_STOP;
int g_nRefill_STOP = REFILL_RUN;

BOOL AbortGas()
{
	BOOL bRet = FALSE;

	do{
		if(SYS_SUCCESS != RUN_FUNCTION(ABORT_GAS, "")) 
		{
			_LOG("[AbortGas] ABORT GAS Failed");
			bRet = FALSE;
			break;
		}
		bRet = TRUE;
	}while(0);

	_LOG("[AbortGas] ABORT GAS Finished");
	return bRet;
}

BOOL CloseRefillValve(int nSrcNo)
{
	BOOL bRet = FALSE;

	switch(nSrcNo)
	{
	case 1: bRet = g_RefillValveSeq.StartValveSeq("CLOSE_REFILL_VALVE", TRUE); break;
	case 2: bRet = g_RefillValveSeq.StartValveSeq("CLOSE_REFILL_VALVE_2", TRUE); break;
	case 3: bRet = g_RefillValveSeq.StartValveSeq("CLOSE_REFILL_VALVE_3", TRUE); break;
	}

	_LOG("[CloseRefillValve] CloseRefillValve Finished");
	return bRet;
}

BOOL OpenRefillValve(int nSrcNo)
{
	BOOL bRet = TRUE;
 
	switch(nSrcNo)
	{
	case 1: bRet = g_RefillValveSeq.StartValveSeq("OPEN_REFILL_VALVE", TRUE); break;
	case 2: bRet = g_RefillValveSeq.StartValveSeq("OPEN_REFILL_VALVE_2", TRUE); break;
	case 3: bRet = g_RefillValveSeq.StartValveSeq("OPEN_REFILL_VALVE_3", TRUE); break;
	}

	_LOG("[OpenRefillValve] OpenRefillValve Finished");
	return bRet;
}

BOOL CloseAllValve()
{
	BOOL bRet = FALSE;
	bRet = g_RefillValveSeq.ExecValveStep("CLOSE_ALL", TRUE);			// 2010.06.30

	_LOG("[CloseAllValve] CloseAllValve Finished");
	return bRet;
}

BOOL IsSystemOK()
{
	BOOL bRet =	FALSE;
    int nIOStatus;

	do{
		if( OFFLINE == READ_DIGITAL(DN_CommStsDI, &nIOStatus))	// 0:OffLine 1:OnLine
		{
			ALARM_POST(ALARM_DEVICE_NET_COMM_ERROR);
			_LOG("[IsSystemOK] ALARM_DEVICE_NET_COMM_ERROR");
			break;
		}
		if( ALARM == READ_DIGITAL(SW_ModuleFailDM, &nIOStatus)  ||
			ALARM == READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus)   )
		{
			ALARM_POST(ALARM_PM_INTERLOCK_DETECTED);
			_LOG("[IsSystemOK] ALARM_PM_INTERLOCK_DETECTED");
			break;
		}
		if( Running == READ_DIGITAL(VRCP_RunningDI, &nIOStatus)   )
		{
			ALARM_POST(ALARM_RECIPE_RUNNING);
			_LOG("[IsSystemOK] ALARM_RECIPE_RUNNING");
			break;
		}		

		if( OFF == READ_DIGITAL(DPRunDI, &nIOStatus)   )
		{
			_LOG("[IsSystemOK] ALARM_REFILL_PUMP_NOT_RUNNING");
			break;
		}
		bRet = TRUE;
	}while(0);
	return bRet;
}

BOOL IsRefillReady(int nSrcNo)
{
	BOOL bRet =	FALSE;
    int nIOStatus;
	int nRefillSysAlmDI;
	int nRefillReadyDI;

	nRefillSysAlmDI = RefillSysAlmDI + nSrcNo -1;
	nRefillReadyDI = RefillReadyDI + nSrcNo -1;

	do {

		if( CRS_ALARM == READ_DIGITAL(nRefillSysAlmDI, &nIOStatus)   )
		{
			ALARM_POST(ALARM_REFILL_SYSTEM_DETECTED);
			_LOG("[IsSystemOK] ALARM_REFILL_SYSTEM_DETECTED");
			break;
		}
		if( DISABLE == READ_DIGITAL(nRefillReadyDI, &nIOStatus)   )
		{
			ALARM_POST(ALARM_REFILL_NOTREADY);
			_LOG("[IsSystemOK] ALARM_REFILL_NOTREADY");
			break;
		}
		bRet = TRUE;
	} while(0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... Using DPT & HT_DB
BOOL SensorRefill(int nSrcNo)
{
	BOOL bRet = FALSE;
	int nTimeout = 0;	// 0.1 sec
	int nTCount  = 0;	// 0.1 sec
	int nLevOpt  = 0;
    int nIOStatus;
	int nSrcLvHighDI;
	int nSrcLvHighLmtDI;

	nSrcLvHighDI	= SrcLvHighDI + nSrcNo -1;
	nSrcLvHighLmtDI = SrcLvHighLmtDI + nSrcNo -1;

	do{
		nTimeout = (int) READ_ANALOG(PRMA_TO_Refill, &nIOStatus); 
		if(nTimeout <= 0)		nTimeout = 5;
		//... Change 1 sec -> 100 ms
		nTimeout = nTimeout * 10;

		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[SensorRefill] REFILL_STOP Checked");
			bRet = TRUE;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_RUN, &nIOStatus); // RefillStatusDM is Running

		if(!OpenRefillValve(nSrcNo))
		{
			_LOG("[SensorRefill] OpenRefillValve is Failed");
			CloseAllValve();
			bRet = FALSE;
			break;
		}

		do{
			if(g_nRefill_STOP == REFILL_STOP)
			{
				_LOG("[SensorRefill] REFILL_STOP Checked");
				bRet = TRUE;
				break;
			}
			
			//////////////////////////////////////////////////////////////////////////
			//... Check Refill Target Sensor Level ... Completed
			if(HOver == READ_DIGITAL(nSrcLvHighDI, &nIOStatus))
			{
				WRITE_DIGITAL(V14ValveDO, VLV_CLOSE, &nIOStatus);
				_LOG("[SensorRefill] Refill Complete - Level High Sensor Over");
				bRet = TRUE;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			if(HOver == READ_DIGITAL(nSrcLvHighLmtDI, &nIOStatus))
			{
				ALARM_POST(ALARM_SOURCE_LEVEL_HIGH);
				_LOG("[SensorRefill] Source-Level Sensor is High Limit");
				bRet = FALSE;
				break;
			}
			
			if(!IsSystemOK())
			{
				_LOG("[SensorRefill] System Check Fail");
				bRet = FALSE;
				break;
			}
			
			if(!WAIT_SECONDS(0.1))
			{
				_LOG("[SensorRefill] WAIT_SECONDS is Failed");
				bRet = FALSE;
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Check Refill Target Sensor Level ... Completed
			if(HOver == READ_DIGITAL(nSrcLvHighDI, &nIOStatus))
			{
				WRITE_DIGITAL(V14ValveDO, VLV_CLOSE, &nIOStatus);
				_LOG("[SensorRefill] Refill Complete - Level High Sensor Over");
				bRet = TRUE;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			nTCount ++;		// Refill TimeOut Counter
			if(nTCount > nTimeout)
			{      
				if(g_DynamicIO.IOValid(PRO_MsgBoxDM)){    // 2012.04.10	
					g_DynamicIO.dWRITE_DIGITAL(PRO_MsgBoxDM,MSG_BOX_REFILL_TIMEOUT, &nIOStatus);
				}else{
					ALARM_POST(ALARM_REFILL_TIMEOUT);
				}
				_LOG("[SensorRefill] ALARM_REFILL_TIMEOUT");
				bRet = FALSE;
				break;
			}
		}while(1);

		if(!CloseRefillValve(nSrcNo))
		{
			_LOG("[SensorRefill] CloseRefillValve is Failed");
			CloseAllValve();
			bRet = FALSE;
			break;
		}		
	}while(0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2014.07.22
//		1) Add Fast Refill Valve Close
//		2) Timeout Counter 1 sec -> 0.1 sec (Wait & Timeout)
BOOL PercentRefill(int nSrcNo)
{
	BOOL bRet = FALSE;
	int nTimeout = 0;	// 0.1 sec
	int nTCount  = 0;	// 0.1 sec
	int nLevOpt  = 0;
	int nSrcPercent = 0;
    int nIOStatus;
	int nSrcCanisterAI;
	int nSrcLvHighLmtDI;

	double	dbCurSrcLevel = 0;

	nSrcCanisterAI	= SrcCanisterAI + nSrcNo -1;
	nSrcLvHighLmtDI = SrcLvHighLmtDI + nSrcNo -1;

	do{
		printf("[DEBUG] PercentRefill Source[%d]\n", nSrcNo);

		nTimeout = (int) READ_ANALOG(PRMA_TO_Refill, &nIOStatus); 
		if(nTimeout <= 0)		nTimeout = 5;
		//... Change 1 sec -> 100 ms
		nTimeout = nTimeout * 10;

		nSrcPercent = (int) READ_ANALOG(PRMA_SP_RefillPer, &nIOStatus);
		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[PercentRefill] REFILL_STOP Checked");
			bRet = TRUE;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_RUN, &nIOStatus); // RefillStatusDM is Running

		//////////////////////////////////////////////////////////////////////////
		//... 2018.06.17 Check Refill Target Percent ... Completed
		dbCurSrcLevel = READ_ANALOG(nSrcCanisterAI, &nIOStatus);
		if(nSrcPercent <= dbCurSrcLevel)
		{
			WRITE_DIGITAL(V14ValveDO, VLV_CLOSE, &nIOStatus);
			_LOG("[PercentRefill] Pre Check Refill Complete - Cur[%.2f] Tgt[%d]", dbCurSrcLevel, nSrcPercent);
			bRet = TRUE;
			break;
		}
		//...

		if(!OpenRefillValve(nSrcNo))
		{
			_LOG("[PercentRefill] OpenRefillValve is Failed");
			CloseAllValve();
			bRet = FALSE;
			break;
		}

		do{
			if(g_nRefill_STOP == REFILL_STOP)
			{
				_LOG("[PercentRefill] REFILL_STOP Checked");
				bRet = TRUE;
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Check Refill Target Percent ... Completed
			dbCurSrcLevel = READ_ANALOG(nSrcCanisterAI, &nIOStatus);
			if(nSrcPercent <= dbCurSrcLevel)
			{
				WRITE_DIGITAL(V14ValveDO, VLV_CLOSE, &nIOStatus);
				_LOG("[PercentRefill] Refill Complete - Cur[%.2f] Tgt[%d]", dbCurSrcLevel, nSrcPercent);
				bRet = TRUE;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			if( HOver == READ_DIGITAL(nSrcLvHighLmtDI, &nIOStatus))
			{
				ALARM_POST(ALARM_SOURCE_LEVEL_HIGH);
				_LOG("[PercentRefill] Sensor is High Limit - Cur[%.2f] Tgt[%d]", dbCurSrcLevel, nSrcPercent);
				bRet = FALSE;
				break;
			}

			if(!IsSystemOK())
			{
				_LOG("[PercentRefill] System Check Fail - Cur[%.2f] Tgt[%d]", dbCurSrcLevel, nSrcPercent);
				bRet = FALSE;
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Check Refill Target Percent ... Completed
			dbCurSrcLevel = READ_ANALOG(nSrcCanisterAI, &nIOStatus);
			if(nSrcPercent <= dbCurSrcLevel)
			{
				WRITE_DIGITAL(V14ValveDO, VLV_CLOSE, &nIOStatus);
				_LOG("[PercentRefill] Refill Complete - Cur[%.2f] Tgt[%d]", dbCurSrcLevel, nSrcPercent);
				bRet = TRUE;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			if(! WAIT_SECONDS(0.1))
			{
				_LOG("[PercentRefill] WAIT_SECONDS is Failed");
				bRet = FALSE;
				break;
			}

			nTCount ++;		// Refill TimeOut Counter
			if(nTCount > nTimeout)
			{
				if(g_DynamicIO.IOValid(PRO_MsgBoxDM)){     // 2012.04.10
					g_DynamicIO.dWRITE_DIGITAL(PRO_MsgBoxDM,MSG_BOX_REFILL_TIMEOUT,&nIOStatus);
				}else{
					ALARM_POST(ALARM_REFILL_TIMEOUT);
				}
				_LOG("[PercentRefill] ALARM_REFILL_TIMEOUT - Cur[%.2f] Tgt[%d]", dbCurSrcLevel, nSrcPercent);
				bRet = FALSE;
				break;
			}

		}while(1);

		if(!CloseRefillValve(nSrcNo))
		{
			_LOG("[PercentRefill] CloseRefillValve is Failed");
			CloseAllValve();
			bRet = FALSE;
			break;
		}		
	}while(0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2014.07.22
//		1) Add Fast Refill Valve Close
//		2) Timeout Counter 1 sec -> 0.1 sec (Wait & Timeout)
BOOL SensorRefill_Li(int nSrcNo)
{
	BOOL bRet = FALSE;
	int nTimeout = 0;	// 0.1 sec
	int nTCount  = 0;	// 0.1 sec
	int nLevOpt  = 0;
    int nIOStatus;
	int nSrcCanisterAI;
	int nSrcLvHighLmtDI;

	double	dbCurSrcLevel = 0;

	nSrcCanisterAI = SrcCanisterAI + nSrcNo - 1;
	nSrcLvHighLmtDI = SrcLvHighLmtDI + nSrcNo -1;

	do{
		printf("[DEBUG] SensorRefill_Li Source[%d]\n", nSrcNo);

		nTimeout = (int) READ_ANALOG(PRMA_TO_Refill, &nIOStatus); 
		if(nTimeout <= 0)		nTimeout = 5;
		//... Change 1 sec -> 100 ms
		nTimeout = nTimeout * 10;

		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[SensorRefill_Li] REFILL_STOP Checked");
			bRet = TRUE;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_RUN, &nIOStatus); // RefillStatusDM is Running

		if(!OpenRefillValve(nSrcNo))
		{
			_LOG("[SensorRefill_Li] OpenRefillValve is Failed");
			CloseAllValve();
			bRet = FALSE;
			break;
		}

		do{
			if(g_nRefill_STOP == REFILL_STOP)
			{
				_LOG("[SensorRefill_Li] REFILL_STOP Checked");
				bRet = TRUE;
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Check Refill Target Percent ... Completed
			dbCurSrcLevel = READ_ANALOG(nSrcCanisterAI, &nIOStatus);
			if(dbCurSrcLevel >= 50)
			{
				WRITE_DIGITAL(V14ValveDO, VLV_CLOSE, &nIOStatus);
				_LOG("[SensorRefill_Li] Refill Complete - Cur[%.2f]", dbCurSrcLevel);
				bRet = TRUE;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			if( HOver == READ_DIGITAL(nSrcLvHighLmtDI, &nIOStatus))
			{
				ALARM_POST(ALARM_SOURCE_LEVEL_HIGH);
				_LOG("[SensorRefill_Li] Sensor is High Limit - Cur[%.2f]", dbCurSrcLevel);
				bRet = FALSE;
				break;
			}

			if(!IsSystemOK())
			{
				_LOG("[SensorRefill_Li] System Check Fail - Cur[%.2f]", dbCurSrcLevel);
				bRet = FALSE;
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Check Refill Target Percent ... Completed
			dbCurSrcLevel = READ_ANALOG(nSrcCanisterAI, &nIOStatus);
			if(dbCurSrcLevel >= 50)
			{
				WRITE_DIGITAL(V14ValveDO, VLV_CLOSE, &nIOStatus);
				_LOG("[SensorRefill_Li] Refill Complete - Cur[%.2f]", dbCurSrcLevel);
				bRet = TRUE;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			if(!WAIT_SECONDS(1))
			{
				_LOG("[SensorRefill_Li] WAIT_SECONDS is Failed");
				bRet = FALSE;
				break;
			}

			nTCount ++;		// Refill TimeOut Counter
			if(nTCount > nTimeout)
			{
				if(g_DynamicIO.IOValid(PRO_MsgBoxDM)){    // 2012.04.10	
					g_DynamicIO.dWRITE_DIGITAL(PRO_MsgBoxDM,MSG_BOX_REFILL_TIMEOUT, &nIOStatus);
				}else{
					ALARM_POST(ALARM_REFILL_TIMEOUT);
				}
				_LOG("[SensorRefill_Li] ALARM_REFILL_TIMEOUT - Cur[%.2f]", dbCurSrcLevel);
				bRet = FALSE;
				break;
			}
		}while(1);

		if(!CloseRefillValve(nSrcNo))
		{
			_LOG("[SensorRefill] CloseRefillValve is Failed");
			CloseAllValve();
			bRet = FALSE;
			break;
		}		
	}while(0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL DoRefill(int nSrcNo)
{
	BOOL bRet = FALSE;
    int nIOStatus;
	int nRefillReqDO;

	nRefillReqDO = RefillReqDO + nSrcNo -1;

	_LOG("Do Refill Started (Src: %d)",nSrcNo);
	
	do{
		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[DoRefill] REFILL_STOP Checked");
			bRet = TRUE;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_READY, &nIOStatus);	// RefillStatusDM is Check (Ready Not Used)
		WRITE_DIGITAL(nRefillReqDO, VLV_OPEN, &nIOStatus);		// RefillStatusDM is Check (Ready Not Used)

		if(!CloseAllValve())
		{
			_LOG("[DoRefill] CloseAllValve Failed");
			bRet = FALSE;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		if(LI_MAN != READ_DIGITAL(PRMD_Refill_Opt, &nIOStatus))
		{	// Use Point Sensor			
			if(LEVEL50 == READ_DIGITAL(PRMD_Refill_Level, &nIOStatus))
			{	// Sensor Base Refill
				if(! SensorRefill(nSrcNo))
				{
					WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Abort
					if(g_DynamicIO.IOValid(PRO_MsgBoxDM)){    // 2012.04.10
						g_DynamicIO.dWRITE_DIGITAL(PRO_MsgBoxDM,MSG_BOX_REFILL_FAILED, &nIOStatus);
					}else	ALARM_POST(ALARM_REFILL_FAILED);
					
					_LOG("[DoRefill] Sensor Base ALARM_REFILL_FAILED");
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
				break;
			}			
			else if(PERCENT == READ_DIGITAL(PRMD_Refill_Level, &nIOStatus))
			{	// Percent Base Refill
				if(! PercentRefill(nSrcNo))
				{
					WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Abort
					if(g_DynamicIO.IOValid(PRO_MsgBoxDM)){    // 2012.04.10
						g_DynamicIO.dWRITE_DIGITAL(PRO_MsgBoxDM,MSG_BOX_REFILL_FAILED, &nIOStatus);
					}else	ALARM_POST(ALARM_REFILL_FAILED);
					
					_LOG("[DoRefill] Percent Base ALARM_REFILL_FAILED");
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
				break;
			}

		}
		else if(LI_MAN == READ_DIGITAL(PRMD_Refill_Opt, &nIOStatus))
		{	// 2009.09.10 Use Continuous Sensor			
			if(LEVEL50 == READ_DIGITAL(PRMD_Refill_Level, &nIOStatus))
			{	// Sensor Base Refill
				if(! SensorRefill_Li(nSrcNo))
				{
					WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Abort
					if(g_DynamicIO.IOValid(PRO_MsgBoxDM)){    // 2012.04.10
						g_DynamicIO.dWRITE_DIGITAL(PRO_MsgBoxDM,MSG_BOX_REFILL_FAILED, &nIOStatus);
					}else	ALARM_POST(ALARM_REFILL_FAILED);
					
					_LOG("[DoRefill] Sensor Base ALARM_REFILL_FAILED");
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
				break;
			}
		}

		ALARM_POST(ALARM_DO_REFILL_FAILED);
		bRet = FALSE;
		break;		
	}while(0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
void Refill_Thread()
{
    int nIOStatus;
	int nSrcNo = 1;

	do{
		g_nRefill_Thread = THREAD_RUN;
		g_nRefill_STOP = REFILL_RUN;

		if(!AbortGas())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Thread] AbortGas Failed");
			break;
		}

		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[Refill_Thread] REFILL_STOP Checked");
			break;
		}

		if(!DoRefill(nSrcNo))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Thread] DoRefill Failed");
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_COMPLETE, &nIOStatus); // RefillStatusDM is Complete

	}while(0);

	WRITE_DIGITAL(RefillReqDO, VLV_CLOSE, &nIOStatus);
	CloseAllValve();

	printf("[Refill_Thread] THREAD_STOP\n");
	_LOG("[##########] FINISH Auto Refill Thread");
	g_nRefill_Thread = THREAD_STOP;
	
	_endthread();
}

//////////////////////////////////////////////////////////////////////////
//... HAHA-DPT Refill Only Sequence
Module_Status Refill_Manual()
{
	Module_Status msRet = SYS_SUCCESS;
	int	nRefill_Method;
	int nIOStatus;
	int nSrcNo = 1;

	double	dbSrcLevel		= 0;
	double	dbCfgFillLevel	= 0;
	double	dbCfgFillTarget = 0;

	do {
		printf("[DEBUG] Refill_Manual by HAHA-DPT\n");

		_LOG("Src%d Refill Started", nSrcNo);

		if(g_nRefill_Thread == THREAD_RUN)		// 2009.12.29
		{
			_LOG("[Program_Main] Refill Thread already Running");
			msRet = SYS_SUCCESS;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check Refill Skip Case

		// Canister Source Check
		if(HOver == READ_DIGITAL(SrcLvHighLmtDI, &nIOStatus))  // Canister Source 70% 
		{
			WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus); // RefillStatusDM is Skipped
			_LOG("[Refill_Manual] HighLimit REFILL_SKIPPED");
			msRet = SYS_SUCCESS;
			break;
		}

		//... Special MAHA-DPT for Low Level
		nRefill_Method	= READ_DIGITAL(PRMD_Refill_Level, &nIOStatus);
		if(PERCENT == nRefill_Method)
		{
			dbSrcLevel		= READ_ANALOG(SrcCanisterAI, &nIOStatus);
			dbCfgFillLevel	= READ_ANALOG(PRMA_VS_Ref_SrcLow, &nIOStatus);
			if(dbSrcLevel >= dbCfgFillLevel)
			{	//... Case Percent Refill Low Skip 
				WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus);
				_LOG("[Refill_Manual] Fill Level Skip Low Cur[%.0f] Low[%.0f]", dbSrcLevel, dbCfgFillLevel);
				msRet = SYS_SUCCESS;
				break;
			}

			dbCfgFillTarget	= READ_ANALOG(PRMA_SP_RefillPer, &nIOStatus);
			if(dbSrcLevel >= dbCfgFillTarget)
			{	//... Case Percent Refill High Skip 
				WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus);
				_LOG("[Refill_Manual] Fill Level Skip High Cur[%.0f] High[%.0f]", dbSrcLevel, dbCfgFillTarget);
				msRet = SYS_SUCCESS;
				break;
			}
		}
		else if(LEVEL50 == nRefill_Method)
		{	// Canister Source Check
			if(HOver == READ_DIGITAL(SrcLvHighDI, &nIOStatus)) // Canister Source 50% 
			{
				WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus); // RefillStatusDM is Skipped
				_LOG("[Refill_Manual] High REFILL_SKIPPED");
				msRet = SYS_SUCCESS;
				break;
			}
		}
		else
		{	//... Not Define Method Skipped
			WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus); // RefillStatusDM is Skipped
			_LOG("[Refill_Manual] Undefined Method[%d]", nRefill_Method);
			msRet = SYS_SUCCESS;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Refill Main Routine
		WRITE_DIGITAL(RefillStatusDM, RS_CHECK, &nIOStatus); // RefillStatusDM is Check

		if(!IsSystemOK())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Manual] ALARM_REFILL_CONDITION_NOT_OK");
			msRet = SYS_ERROR;
			break;
		}

		if(!IsRefillReady(nSrcNo))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("ALARM_REFILL_CONDITION_NOT_OK");
			msRet = SYS_ERROR;
			break;
		}
	
		g_nRefill_Thread = THREAD_RUN;
		g_nRefill_STOP = REFILL_RUN;

		if(!AbortGas())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Manual] AbortGas Failed");
			break;
		}

		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[Refill_Manual] REFILL_STOP Checked");
			msRet = SYS_ERROR;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		//... 
		if(!DoRefill(nSrcNo))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Manual] DoRefill Failed");
			msRet = SYS_ERROR;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_COMPLETE, &nIOStatus); // RefillStatusDM is Complete

	}while(0);

	WRITE_DIGITAL(RefillReqDO, VLV_CLOSE, &nIOStatus);	
	CloseAllValve();

	g_nRefill_Thread = THREAD_STOP;
	
	if(msRet != SYS_SUCCESS)		WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus);		// RefillStatusDM is Aborted
	
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status Refill_Auto()
{
	Module_Status msRet = SYS_SUCCESS;
	int nPRMD_Refill_Opt;
	int nIOStatus;
	int nSrcNo = 1;

	do {
		// 2009.12.29
		nPRMD_Refill_Opt = READ_DIGITAL(PRMD_Refill_Opt, &nIOStatus);	// 2009.09.10
		if(AUTO != nPRMD_Refill_Opt)
		{
			msRet = SYS_SUCCESS;
			break;  
		}

		// 2012.07.04
		if(LOver==READ_DIGITAL(SrcLvLowDI, &nIOStatus)
			&& READ_ANALOG(SrcCanisterAI, &nIOStatus) >= READ_ANALOG(PRMA_VS_Ref_SrcLow, &nIOStatus))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus);
			_LOG("[Program_Main] Auto Sorce Low Level Over REFILL_SKIPPED");
			msRet = SYS_SUCCESS;
			break;  
		}

		// Refill Thread Running, Skip Refill
		if(g_nRefill_Thread == THREAD_RUN)
		{
			_LOG("[Program_Main] Thread Already Running. Refill Skip");
			printf("Refill Thread Already Running - Refill Skip\n");
			msRet = SYS_SUCCESS;
			break;  
		}

		// Refill Main Routine
		if(!IsSystemOK())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Program_Main] ALARM_REFILL_CONDITION_NOT_OK");
			msRet = SYS_ERROR;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_CHECK, &nIOStatus); // RefillStatusDM is Check

		//////////////////////////////////////////////////////////////////////////
		if(g_nRefill_Thread != THREAD_RUN)
		{
			_beginthread((_THREAD_FUNC) Refill_Thread, 0, NULL);
			printf("[Refill_Thread] THREAD_RUN\n");
			_LOG("[##########] START Auto Refill Thread");
			msRet = SYS_SUCCESS;
			break;
		}
		else
		{
			_LOG("[Program_Main] Refill Thread already Running");
			msRet = SYS_SUCCESS;
			break;
		}
	} while(0);

	return msRet;
}

Module_Status Refill_Manual_2()
{
	Module_Status msRet = SYS_SUCCESS;
	int nPRMD_Refill_Opt;
	int nIOStatus;
	int nSrcNo = 2;

	do {
		_LOG("Src%d Refill Started", nSrcNo);

		// 2009.12.29
		nPRMD_Refill_Opt = READ_DIGITAL(PRMD_Refill_Opt, &nIOStatus);	// 2009.09.10
		
		if(g_nRefill_Thread == THREAD_RUN)		// 2009.12.29
		{
			_LOG("Refill Thread already Running");
			msRet = SYS_SUCCESS;
			break;
		}

		// Refill Main Routine
		if(!IsSystemOK())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("ALARM_REFILL_CONDITION_NOT_OK");
			msRet = SYS_ERROR;
			break;
		}

		if(!IsRefillReady(nSrcNo))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("ALARM_REFILL_CONDITION_NOT_OK");
			msRet = SYS_ERROR;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_CHECK, &nIOStatus); // RefillStatusDM is Check
		if(LI_MAN != nPRMD_Refill_Opt)		// 2009.09.10
		{
			// Canister Source Check
			if((HOver == READ_DIGITAL(Src2LvHighLmtDI, &nIOStatus))  // Canister Source 70% 
				||(HOver == READ_DIGITAL(Src2LvHighDI, &nIOStatus))) // Canister Source 50% 
			{
				WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus); // RefillStatusDM is Skipped
				_LOG("[Program_Main] HighLimit or High REFILL_SKIPPED");
				msRet = SYS_SUCCESS;
				break;
			}
		}
		else								// 2009.09.10
		{
			if(READ_ANALOG(Src2CanisterAI, &nIOStatus) >= 50)
			{
				WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus); // RefillStatusDM is Skipped
				_LOG("[Program_Main] Level Is Over than 50 %, REFILL_SKIPPED");
				msRet = SYS_SUCCESS;
				break;
			}
		}

		g_nRefill_Thread = THREAD_RUN;
		g_nRefill_STOP = REFILL_RUN;

		if(!AbortGas())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Manual] AbortGas Failed");
			break;
		}

		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[Refill_Manual] REFILL_STOP Checked");
			msRet = SYS_ERROR;
			break;
		}

		if(!DoRefill(nSrcNo))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Manual] DoRefill Failed");
			msRet = SYS_ERROR;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_COMPLETE, &nIOStatus); // RefillStatusDM is Complete

	}while(0);

	WRITE_DIGITAL(RefillReq2DO, VLV_CLOSE, &nIOStatus);
	CloseAllValve();
	g_nRefill_Thread = THREAD_STOP;
	if(msRet != SYS_SUCCESS)		// 2011.07.14
	{
		WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus);		// RefillStatusDM is Aborted
	}
	return msRet;
}

Module_Status Refill_Manual_3()
{
	Module_Status msRet = SYS_SUCCESS;
	int nPRMD_Refill_Opt;
	int nIOStatus;
	int nSrcNo = 3;

	do {
		_LOG("Src%d Refill Started", nSrcNo);

		// 2009.12.29
		nPRMD_Refill_Opt = READ_DIGITAL(PRMD_Refill_Opt, &nIOStatus);	// 2009.09.10
		
		if(g_nRefill_Thread == THREAD_RUN)		// 2009.12.29
		{
			_LOG("Refill Thread already Running");
			msRet = SYS_SUCCESS;
			break;
		}

		// Refill Main Routine
		if(!IsSystemOK())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("ALARM_REFILL_CONDITION_NOT_OK");
			msRet = SYS_ERROR;
			break;
		}

		if(!IsRefillReady(nSrcNo))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("ALARM_REFILL_CONDITION_NOT_OK");
			msRet = SYS_ERROR;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_CHECK, &nIOStatus); // RefillStatusDM is Check
		if(LI_MAN != nPRMD_Refill_Opt)		// 2009.09.10
		{
			// Canister Source Check
			if((HOver == READ_DIGITAL(Src3LvHighLmtDI, &nIOStatus))  // Canister Source 70% 
				||(HOver == READ_DIGITAL(Src3LvHighDI, &nIOStatus))) // Canister Source 50% 
			{
				WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus); // RefillStatusDM is Skipped
				_LOG("[Program_Main] HighLimit or High REFILL_SKIPPED");
				msRet = SYS_SUCCESS;
				break;
			}
		}
		else								// 2009.09.10
		{
			if(READ_ANALOG(Src3CanisterAI, &nIOStatus) >= 50)
			{
				WRITE_DIGITAL(RefillStatusDM, RS_SKIP, &nIOStatus); // RefillStatusDM is Skipped
				_LOG("[Program_Main] Level Is Over than 50 %, REFILL_SKIPPED");
				msRet = SYS_SUCCESS;
				break;
			}
		}

		g_nRefill_Thread = THREAD_RUN;
		g_nRefill_STOP = REFILL_RUN;

		if(!AbortGas())
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Manual] AbortGas Failed");
			break;
		}

		if(g_nRefill_STOP == REFILL_STOP)
		{
			_LOG("[Refill_Manual] REFILL_STOP Checked");
			msRet = SYS_ERROR;
			break;
		}

		if(!DoRefill(nSrcNo))
		{
			WRITE_DIGITAL(RefillStatusDM, RS_ABORT, &nIOStatus); // RefillStatusDM is Aborted
			_LOG("[Refill_Manual] DoRefill Failed");
			msRet = SYS_ERROR;
			break;
		}

		WRITE_DIGITAL(RefillStatusDM, RS_COMPLETE, &nIOStatus); // RefillStatusDM is Complete
	}while(0);

	WRITE_DIGITAL(RefillReq3DO, VLV_CLOSE, &nIOStatus);
	CloseAllValve();
	g_nRefill_Thread = THREAD_STOP;
	return msRet;
}

Module_Status Refill_Manual_SN()
{
	enum { SRC_1, SRC_2, SRC_3 };

	int nSrcNo;
	BOOL bIOStatus;

	nSrcNo = READ_DIGITAL(RefillSrcNoDM, &bIOStatus);
	switch(nSrcNo)
	{
	case SRC_1: return Refill_Manual();
	case SRC_2: return Refill_Manual_2();
	case SRC_3: return Refill_Manual_3();
	}

	return SYS_ERROR;
}

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int nParLen = 0;
	// int nIOStatus;
	char *pParam;
	char szCmd[41], szSubParam[256];

	printf("Entered %s\n", APP_NAME);
	_LOG("---------------------------------------------");
	_LOG("Entered %s", APP_NAME);
	do {

		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if ( nParLen < 1 )	{
			printf( "---> Parameter Error, Len = %d in %s !\n", nParLen, APP_NAME);
			msRet = SYS_ERROR;
			break;
		}
		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );

		// 2009.12.29
		if     (STRCMP_L(szCmd, "AUTO"))		msRet = Refill_Auto();		// NotUsed
		else if(STRCMP_L(szCmd, "MANUAL"))		msRet = Refill_Manual();	// Refill MANUAL
		else if(STRCMP_L(szCmd, "STOP"))									// Refill STOP
		{
			g_nRefill_STOP = REFILL_STOP;
			_LOG("[Program_Main] Auto Refill STOPPED");
			msRet = SYS_SUCCESS;
			break;  
		}
		// 2009.12.29
		else if(STRCMP_L(szCmd, "MANUAL_2"))	msRet = Refill_Manual_2();
		else if(STRCMP_L(szCmd, "MANUAL_3"))	msRet = Refill_Manual_3();
		else if(STRCMP_L(szCmd, "MANUAL_SN"))	msRet = Refill_Manual_SN();
		// 2011.07.14
		else if(STRCMP_L(szCmd, "AUTO_SRC1"))	msRet = Refill_Manual();	// None Thread Used by PROCESS_CONTROL
		else if(STRCMP_L(szCmd, "AUTO_SRC2"))	msRet = Refill_Manual();	// None Thread Used by PROCESS_CONTROL
		else if(STRCMP_L(szCmd, "AUTO_ALL"))	msRet = Refill_Manual();	// None Thread Used by PROCESS_CONTROL
	} while(0);
	
	_LOG("Leaved %s", APP_NAME);
	printf("Leaved %s\n", APP_NAME);

	return msRet;												// 2011.07.14
}

//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	g_RefillValveSeq.Initialize("AutoRefill");
	g_DynamicIO.InitIOList(g_pIOList);
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
