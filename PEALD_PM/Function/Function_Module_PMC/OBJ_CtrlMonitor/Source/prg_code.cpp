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

#include "TextParser.h"
//--------------------------------------------------------------------
#ifndef _DEBUG
// #define _DEBUG
#endif

//------------------------------------------------------------------------------------------
#define APP_NAME	"ControlMonitor"

//------------------------------------------------------------------------------------------
// Alarm Definition
#define ALARM_MKS651_COMM_ERROR				9100
#define ALARM_VIX250_COMM_ERROR				9101

#define ALARM_DEVICE_NET_COMM_ERROR			9104

#define ALARM_STG_HTR_RELAY_ON_FAILED		1301
#define ALARM_GAS_HTR_RELAY_ON_FAILED		1302
#define ALARM_CHM_HTR_RELAY_ON_FAILED		1303
#define ALARM_STG_HTR_RELAY_OFF_FAILED		1304
#define ALARM_GAS_HTR_RELAY_OFF_FAILED		1305
#define ALARM_CHM_HTR_RELAY_OFF_FAILED		1306
#define ALARM_SW_INTERLOCK_ACT				1307

#define ALARM_SW_MODULE_FAILED				1309

#define ALARM_STG_HTR2_RELAY_ON_FAILED		1311
#define ALARM_GAS_HTR2_RELAY_ON_FAILED		1312
#define ALARM_CHM_HTR2_RELAY_ON_FAILED		1313
#define ALARM_STG_HTR2_RELAY_OFF_FAILED		1314
#define ALARM_GAS_HTR2_RELAY_OFF_FAILED		1315
#define ALARM_CHM_HTR2_RELAY_OFF_FAILED		1316

#define ALARM_ROUGH_VALVE_CTRL_FAILED		1330

#define ALARM_RPG_PLASMA_NOT_DETECTED		2605
#define ALARM_RPG_REFLECT_IS_HIGH			2606
#define ALARM_RPG_Gate_Valve1_FAILED		2607
#define ALARM_RPG_Gate_Valve2_FAILED		2608
#define ALARM_RPG_PLASMA_NOT_DETECTED2		2609
#define ALARM_RPG_REFLECT_IS_HIGH2			2610

#define ALARM_AIR_COOLING1_PRESSURE			8902
#define ALARM_AIR_COOLING2_PRESSURE			8903

#define MAX_RECHECK_CNT	5
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_GbIntlckDM",		_K_D_IO	,	SW_GbIntlckDM		, 0},
	{ "SW_ModuleFailDM",	_K_D_IO	,	SW_ModuleFailDM		, 0},

	{ "PRO_RunStsDM",		_K_D_IO	,	PRO_RunStsDM		, 0},
	{ "PRO_FlowControlDM",	_K_D_IO	,	PRO_FlowControlDM	, 0},	//... 2015.02.07 Add Retry for RPG Alarm

	{ "ChmPrsAI",			_K_A_IO,	ChmPrsAI			, 0},
	{ "VacuumStateDM",		_K_D_IO,	VacuumStateDM		, 0},

	{ "StgHtr1RelayDI",		_K_D_IO	,	StgHtr1RelayDI		, 0},
	{ "StgHtr2RelayDI",		_K_D_IO	,	StgHtr2RelayDI		, 0},
	{ "GLHtrRelayDI",		_K_D_IO	,	GLHtrRelayDI		, 0},
	{ "GLHtrRelay2DI",		_K_D_IO	,	GLHtrRelay2DI		, 0},

	{ "StgHtr1RelayDO",		_K_D_IO	,	StgHtr1RelayDO		, 0},
	{ "StgHtr2RelayDO",		_K_D_IO	,	StgHtr2RelayDO		, 0},
	{ "GLHtrRelayDO",		_K_D_IO	,	GLHtrRelayDO		, 0},
	{ "GLHtrRelay2DO",		_K_D_IO	,	GLHtrRelay2DO		, 0},

	{ "VRCP_StaStpDO",		_K_D_IO	,	VRCP_StaStpDO		, 0},

	{ "M651_CommStatus",	_K_D_IO	,	M651_CommStatus		, 0},
	{ "DN_CommStsDI",		_K_D_IO	,	DN_CommStsDI		, 0},

	// 2006.03.02
	{ "FastRoughVlvDO",		_K_D_IO	,	FastRoughVlvDO		, 0},
	{ "RoughValveDI",		_K_D_IO	,	RoughValveDI		, 0},

	{ "RPGPlasmaDetectDI",	_K_D_IO	,	RPGPlasmaDetectDI	, 0},
	{ "RPGPwOnDO",			_K_D_IO	,	RPGPwOnDO			, 0},
	{ "PRMA_EC_RPGPlsmDtc",	_K_A_IO	,	PRMA_EC_RPGPlsmDtc	, 0},

	{ "RPGRefPowAI",		_K_A_IO	,	RPGRefPowAI			, 0},
	{ "PRMA_TL_RPGRefl",	_K_A_IO	,	PRMA_TL_RPGRefl		, 0},
	{ "PRMA_EC_RPGFwPw",	_K_A_IO	,	PRMA_EC_RPGFwPw		, 0},
	{ "RPG_UseDM",			_K_D_IO	,	RPG_UseDM			, 0},

	{ "RPGGateVlvPosDI",	_K_D_IO	,	RPGGateVlvPosDI		, 0},
	{ "RPGGateVlv2PosDI",	_K_D_IO	,	RPGGateVlv2PosDI	, 0},
	{ "V103ValveDO",		_K_D_IO	,	RPGGateVv1DO		, 0},
	{ "V111ValveDO",		_K_D_IO	,	RPGGateVv2DO		, 0},
	{ "PRMA_TO_RPGGateVv",	_K_A_IO	,	PRMA_TO_RPGGateVv	, 0},

	{ "ABORT_GAS"  ,		_K_F_IO	,	ABORT_GAS			, 0} ,
	{ "AUTO_MFC_VERIFY_CTR"			, _K_D_IO	,	AUTO_MFC_VERIFY_CTR				,	0	} ,		//2014.04.24
	//... 2019.01.17 Air Cooling Interlock Function
	{ "AirCoolingVlv",			_K_D_IO	,	AirCoolingVlv		, 0},
	{ "PRMD_HO_ChuckCoolPrs",	_K_D_IO	,	PRMD_HO_ChuckCoolPrs, 0},
	{ "ChuckCoolingOut1Prs",	_K_A_IO	,	ChuckCoolingOut1Prs	, 0},
	{ "ChuckCoolingOut2Prs",	_K_A_IO	,	ChuckCoolingOut2Prs	, 0},
	{ "PRMA_COOLPRS_HIGH",		_K_A_IO	,	PRMA_COOLPRS_HIGH	, 0},
	{ "PRMA_COOLPRS_LOW",		_K_A_IO	,	PRMA_COOLPRS_LOW	, 0},
	//...
	""
};

//------------------------------------------------------------------------------------------
// Thread Type Definition for Using thread

typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

enum { EN_OFF,   EN_ON };
enum { VALVE_CLOSE, VALVE_OPEN };
enum { GATESTS_UNK, GATESTS_OPEN, GATESTS_CLOSE, GATESTS_ERR };

//... 2015.02.07 Add Retry for RPG Alarm
enum { ePROALM_ABORT,	ePROALM_RETRY	};
enum { ePROSTS_STOP,	ePROSTS_RUN		};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//... 2015.02.07 Add Process Retry for RPG Alarm
void SetProcessRetry()
{
	int		nIOStatus;

	if( ePROSTS_RUN == READ_DIGITAL(PRO_RunStsDM, &nIOStatus) &&
		ePROALM_RETRY != READ_DIGITAL(PRO_FlowControlDM, &nIOStatus))
	{
		WRITE_DIGITAL(PRO_FlowControlDM, ePROALM_RETRY, &nIOStatus);
	}
}

//////////////////////////////////////////////////////////////////////////
//... Check RPG Status
void RPGStsMonThread()
{
	int		nIOStatus;
	int		nRPGGateVvDO = 0;
	int		nRPGGateVvXI = 0;

	int		nRPGRefErrCnt		= 0;
	int		nRPGPlsmDtcErrCnt	= 0;
	int		nRPGVlvErrCnt		= 0;
	int		nRPGVlv2ErrCnt		= 0;

	double	dbCfgReflectTol		= 0;
	double	dbReflectPower		= 0;
	int		nCfgReflectTOut		= 0;
	int		nCfgPlasmaDtcTOut	= 0;
	int		nCfgGateVvSyncTOut	= 0;

	do
	{
		if(EN_ON == READ_DIGITAL(RPGPwOnDO, &nIOStatus))
		{
			//////////////////////////////////////////////////////////////////////////
			//... Check RPG Reflect Power
			dbCfgReflectTol = READ_ANALOG(PRMA_TL_RPGRefl, &nIOStatus);
			if(dbCfgReflectTol > 0)
			{
				dbReflectPower = READ_ANALOG(RPGRefPowAI, &nIOStatus);
				if(dbReflectPower > dbCfgReflectTol)
				{
					nRPGRefErrCnt++;
					nCfgReflectTOut = (int) READ_ANALOG(PRMA_EC_RPGFwPw, &nIOStatus);
					if(nRPGRefErrCnt > nCfgReflectTOut)
					{
						//... 2015.02.07 Add Process Retry for RPG Alarm
						SetProcessRetry();

						WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
						if(ALM_PAUSED != ALARM_STATUS(ALARM_RPG_REFLECT_IS_HIGH))
							ALARM_POST(ALARM_RPG_REFLECT_IS_HIGH);
						nRPGRefErrCnt = 0;
					}
				}
				else nRPGRefErrCnt = 0;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Check RPG Plasma Detect
			if(EN_ON != READ_DIGITAL(RPGPlasmaDetectDI, &nIOStatus))
			{
				nCfgPlasmaDtcTOut = (int) READ_ANALOG(PRMA_EC_RPGPlsmDtc, &nIOStatus);
				if(nCfgPlasmaDtcTOut > 0)
				{
					nRPGPlsmDtcErrCnt++;
					if(nRPGPlsmDtcErrCnt > nCfgPlasmaDtcTOut)
					{
						//... 2015.02.07 Add Process Retry for RPG Alarm
						SetProcessRetry();

						WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
						if(ALM_PAUSED != ALARM_STATUS(ALARM_RPG_PLASMA_NOT_DETECTED))
							ALARM_POST(ALARM_RPG_PLASMA_NOT_DETECTED);
						nRPGPlsmDtcErrCnt = 0;
					}
				}
				else nRPGPlsmDtcErrCnt = 0;
			}
			else nRPGPlsmDtcErrCnt = 0;
		}
		else
		{
			nRPGRefErrCnt = 0;
			nRPGPlsmDtcErrCnt = 0;
		}

		//2014.04.24
		if( EN_ON == READ_DIGITAL(AUTO_MFC_VERIFY_CTR, &nIOStatus)){
			//if(! WAIT_SECONDS(1))	break;
			//... 2017.03.07
			Sleep(1000);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check RPG Gate Valve 1, 2
		nCfgGateVvSyncTOut = (int) READ_ANALOG(PRMA_TO_RPGGateVv, &nIOStatus);

		//... RPM GATE VALVE 1
		nRPGGateVvDO = READ_DIGITAL(RPGGateVv1DO, &nIOStatus);
		nRPGGateVvXI = READ_DIGITAL(RPGGateVlvPosDI, &nIOStatus);
		if(nRPGGateVvDO == VALVE_OPEN)
		{
			if(nRPGGateVvXI != GATESTS_OPEN)
			{
				nRPGVlvErrCnt++;
				if(nRPGVlvErrCnt > nCfgGateVvSyncTOut)
				{
					//... 2015.02.07 Add Process Retry for RPG Alarm
					SetProcessRetry();

					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_RPG_Gate_Valve1_FAILED))
						ALARM_POST(ALARM_RPG_Gate_Valve1_FAILED);
					nRPGVlvErrCnt = 0;
				}
			}
			else nRPGVlvErrCnt = 0;
		}

		if(nRPGGateVvDO == VALVE_CLOSE)
		{
			if(nRPGGateVvXI != GATESTS_CLOSE)
			{
				nRPGVlvErrCnt++;
				if(nRPGVlvErrCnt > nCfgGateVvSyncTOut)
				{
					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_RPG_Gate_Valve1_FAILED))
						ALARM_POST(ALARM_RPG_Gate_Valve1_FAILED);
					nRPGVlvErrCnt = 0;
				}
			}
			else nRPGVlvErrCnt = 0;
		}

		//... RPM GATE VALVE 2
		nRPGGateVvDO = READ_DIGITAL(RPGGateVv2DO, &nIOStatus);
		nRPGGateVvXI = READ_DIGITAL(RPGGateVlv2PosDI, &nIOStatus);
		if(nRPGGateVvDO == VALVE_OPEN)
		{
			if(nRPGGateVvXI != GATESTS_OPEN)
			{
				nRPGVlv2ErrCnt++;
				if(nRPGVlv2ErrCnt > nCfgGateVvSyncTOut)
				{
					//... 2015.02.07 Add Process Retry for RPG Alarm
					SetProcessRetry();

					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_RPG_Gate_Valve2_FAILED))
						ALARM_POST(ALARM_RPG_Gate_Valve2_FAILED);
					nRPGVlv2ErrCnt = 0;
				}
			}
			else nRPGVlv2ErrCnt = 0;
		}

		if(nRPGGateVvDO == VALVE_CLOSE)
		{
			if(nRPGGateVvXI != GATESTS_CLOSE)
			{
				nRPGVlv2ErrCnt++;
				if(nRPGVlv2ErrCnt > nCfgGateVvSyncTOut)
				{
					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_RPG_Gate_Valve2_FAILED))
						ALARM_POST(ALARM_RPG_Gate_Valve2_FAILED);
					nRPGVlv2ErrCnt = 0;
				}
			}
			else nRPGVlv2ErrCnt = 0;
		}

		//...
		//if(! WAIT_SECONDS(1))	break;
		//... 2017.03.07
		Sleep(1000);
	} while(1);

	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Interlock Monitor Thread and Function
void SWModuleFailThread()
{
	int nIOStatus;
	int nValue;
	int nAlarmRecovery;

	do {
		nValue = READ_DIGITAL(SW_ModuleFailDM, &nIOStatus);	// 0:Off 1:On
		if(nValue == 1)
		{
			if(READ_DIGITAL(PRO_RunStsDM, &nIOStatus) == 0)
							WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
			nAlarmRecovery = ALARM_MANAGE(ALARM_SW_MODULE_FAILED);
			if(nAlarmRecovery == ALM_CLEAR) WRITE_DIGITAL(SW_ModuleFailDM, 0, &nIOStatus);
		}
		//if(! WAIT_SECONDS(1)) break;
		//... 2017.03.07
		Sleep(1000);
	} while(1);
	_endthread();
}

void SWInterlockThread()
{
	int nIOStatus;
	int nValue;
	int nAlarmRecovery;

	do {
		nValue = READ_DIGITAL(SW_GbIntlckDM, &nIOStatus);	// 0:Normal 1:Interlock
		if(nValue == 1)
		{
			RUN_FUNCTION_FREE(ABORT_GAS, "");
			nAlarmRecovery = ALARM_MANAGE(ALARM_SW_INTERLOCK_ACT);
			if(nAlarmRecovery == ALM_CLEAR) WRITE_DIGITAL(SW_GbIntlckDM, 0, &nIOStatus);
		}
		//if(! WAIT_SECONDS(1)) break;
		//... 2017.03.07
		Sleep(1000);
	} while(1);
	_endthread();
}

//////////////////////////////////////////////////////////////////////////
// Heater Relay Monitoring
#define RELAY_ERR_TIMEOUT_TICK	5
int g_nStgHtrErrCnt		= 0;
int g_nStgHtrErrCnt2	= 0;
int g_nGLHtrErrCnt		= 0;
int g_nGLHtrErrCnt2		= 0;
int g_nChmHtrErrCnt		= 0;
int g_nChmHtrErrCnt2	= 0;

void CheckHeaterRelay()
{
	int nIOStatus;

	enum { RELAY_OFF, RELAY_ON };

	int	nRelayDO = RELAY_OFF;
	int	nRelayDI = RELAY_OFF;

	//////////////////////////////////////////////////////////////////////////
	// Check Stage Heater (1) Relay
	nRelayDO = READ_DIGITAL(StgHtr1RelayDO, &nIOStatus);
	nRelayDI = READ_DIGITAL(StgHtr1RelayDI, &nIOStatus);

	if(nRelayDO == RELAY_ON)
	{
		if(nRelayDI != RELAY_ON)
		{
			if(++g_nStgHtrErrCnt > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_STG_HTR_RELAY_ON_FAILED))
					ALARM_POST(ALARM_STG_HTR_RELAY_ON_FAILED);
				WRITE_DIGITAL(StgHtr1RelayDO, RELAY_OFF, &nIOStatus);
				g_nStgHtrErrCnt = 0;
			}
		}
		else g_nStgHtrErrCnt = 0;
	}
	else
	{
		if(nRelayDI != RELAY_OFF)
		{
			if(++g_nStgHtrErrCnt > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_STG_HTR_RELAY_OFF_FAILED))
					ALARM_POST(ALARM_STG_HTR_RELAY_OFF_FAILED);
				g_nStgHtrErrCnt = 0;
			}
		}
		else g_nStgHtrErrCnt = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// Check Stage Heater (2) Relay
	nRelayDO = READ_DIGITAL(StgHtr2RelayDO, &nIOStatus);
	nRelayDI = READ_DIGITAL(StgHtr2RelayDI, &nIOStatus);

	if(nRelayDO == RELAY_ON)
	{
		if(nRelayDI != RELAY_ON)
		{
			if(++g_nStgHtrErrCnt2 > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_STG_HTR2_RELAY_ON_FAILED))
					ALARM_POST(ALARM_STG_HTR_RELAY_ON_FAILED);
				WRITE_DIGITAL(StgHtr2RelayDO, RELAY_OFF, &nIOStatus);
				g_nStgHtrErrCnt2 = 0;
			}
		}
		else g_nStgHtrErrCnt2 = 0;
	}
	else
	{
		if(nRelayDI != RELAY_OFF)
		{
			if(++g_nStgHtrErrCnt2 > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_STG_HTR2_RELAY_OFF_FAILED))
					ALARM_POST(ALARM_STG_HTR_RELAY_OFF_FAILED);
				g_nStgHtrErrCnt2 = 0;
			}
		}
		else g_nStgHtrErrCnt2 = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// Check GasLine Heater (1) Relay
	nRelayDO = READ_DIGITAL(GLHtrRelayDO, &nIOStatus);
	nRelayDI = READ_DIGITAL(GLHtrRelayDI, &nIOStatus);

	if(nRelayDO == RELAY_ON)
	{
		if(nRelayDI != RELAY_ON)
		{
			if(++g_nGLHtrErrCnt > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_GAS_HTR_RELAY_ON_FAILED))
					ALARM_POST(ALARM_GAS_HTR_RELAY_ON_FAILED);
				WRITE_DIGITAL(GLHtrRelayDO, RELAY_OFF, &nIOStatus);
				g_nGLHtrErrCnt = 0;
			}
		}
		else g_nGLHtrErrCnt = 0;
	}
	else
	{
		if(nRelayDI != RELAY_OFF)
		{
			if(++g_nGLHtrErrCnt > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_GAS_HTR_RELAY_OFF_FAILED))
					ALARM_POST(ALARM_GAS_HTR_RELAY_OFF_FAILED);
				g_nGLHtrErrCnt = 0;
			}
		}
		else g_nGLHtrErrCnt = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// Check GasLine Heater (2) Relay
	nRelayDO = READ_DIGITAL(GLHtrRelay2DO, &nIOStatus);
	nRelayDI = READ_DIGITAL(GLHtrRelay2DI, &nIOStatus);

	if(nRelayDO == RELAY_ON)
	{
		if(nRelayDI != RELAY_ON)
		{
			if(++g_nGLHtrErrCnt2 > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_GAS_HTR2_RELAY_ON_FAILED))
					ALARM_POST(ALARM_GAS_HTR2_RELAY_ON_FAILED);
				WRITE_DIGITAL(GLHtrRelay2DO, RELAY_OFF, &nIOStatus);
				g_nGLHtrErrCnt2 = 0;
			}
		}
		else g_nGLHtrErrCnt2 = 0;
	}
	else
	{
		if(nRelayDI != RELAY_OFF)
		{
			if(++g_nGLHtrErrCnt2 > RELAY_ERR_TIMEOUT_TICK)
			{
				if(ALM_PAUSED != ALARM_STATUS(ALARM_GAS_HTR2_RELAY_OFF_FAILED))
					ALARM_POST(ALARM_GAS_HTR2_RELAY_OFF_FAILED);
				g_nGLHtrErrCnt2 = 0;
			}
		}
		else g_nGLHtrErrCnt2 = 0;
	}
}

//------------------------------------------------------------------------------------------
// Monitoring Rough Valve Sensor
// 2006.03.02
void CheckRoughValveSensor()
{
	int nIOStatus;
	int nRoughValveDI;		// 0:Unknown 1:Open 2:Close 3:Error
	int nFastRoughVlvDO;	// 0:Close 1:Open
	static int nErrCount = 0;

	nFastRoughVlvDO = READ_DIGITAL(FastRoughVlvDO, &nIOStatus);
	nRoughValveDI = READ_DIGITAL(RoughValveDI, &nIOStatus);

	if (nFastRoughVlvDO == 0) {
		// If Open
		if(nRoughValveDI != 2) nErrCount++;
		else nErrCount = 0;
	} else	{
		// If Close
		if(nRoughValveDI != 1) nErrCount++;
		else nErrCount = 0;
	}

	if (nErrCount >= 5) {
		//2009.01.21 Valve Abort when rough valve control case also
		WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
		if(ALM_PAUSED != ALARM_STATUS(ALARM_ROUGH_VALVE_CTRL_FAILED))
			ALARM_POST(ALARM_ROUGH_VALVE_CTRL_FAILED);
		nErrCount = 0;
	}
}

//------------------------------------------------------------------------------------------
// Common Monitor Thread
void CommonMonitor_Thread()
{
	WAIT_SECONDS(5);	// 2006.03.02

	while(1)
	{
		// 1. Stage Position
		CheckRoughValveSensor();
		Sleep(200);

		// 2. Heater Relay
		CheckHeaterRelay();
		Sleep(200);

		Sleep(200);

		Sleep(200);

		Sleep(200);
	}
	_endthread();
}

//////////////////////////////////////////////////////////////
int g_nMksCommErrRetry	= 5;
int g_nMKSCommErrCnt = 0;
void MKSCommStsMonThread()
{
	int nValue = 0;
	int nIOStatus;
	int i;

	for (i=0; i<60; i++) {
		//WAIT_SECONDS(1);
		Sleep(1000);
		nValue = READ_DIGITAL(M651_CommStatus, &nIOStatus);
		if(nValue == 1) break;
	}

	g_nMKSCommErrCnt = 0;
	while (1) {
		do {
			nValue = READ_DIGITAL(M651_CommStatus, &nIOStatus);		// 0:OffLine 1:OnLine
			if (nValue == 1) {
				g_nMKSCommErrCnt = 0;
				break;
			} else {
				g_nMKSCommErrCnt++;
				if (g_nMKSCommErrCnt > g_nMksCommErrRetry) {
					// Set Process Disable Interlock
					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_MKS651_COMM_ERROR))
						ALARM_POST(ALARM_MKS651_COMM_ERROR);
					g_nMKSCommErrCnt = 0;
				}
			}
			//WAIT_SECONDS(1);
			Sleep(1000);
		}while(1);
		//WAIT_SECONDS(2);
		Sleep(2000);
	}
	_endthread();
}

//////////////////////////////////////////////////////////////
int g_nDN_CommErrRetry		= 5;
int g_nDN_CommErrCnt		= 0;
void DN_CommStsMonThread()
{
	int nValue = 0;
	int nIOStatus;

	WAIT_SECONDS(15);

	while (1) {
		do {
			nValue = READ_DIGITAL(DN_CommStsDI, &nIOStatus);		// 0:OffLine 1:OnLine
			if (nValue == 1) {
				g_nDN_CommErrCnt = 0;
				break;
			} else {
				g_nDN_CommErrCnt++;
				if(g_nDN_CommErrCnt > g_nDN_CommErrRetry)
				{
					// Set Process Disable Interlock
					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_DEVICE_NET_COMM_ERROR))
						ALARM_POST(ALARM_DEVICE_NET_COMM_ERROR);
					g_nDN_CommErrCnt = 0;
				}
			}
			//WAIT_SECONDS(1);
			Sleep(1000);

		}while(1);
		//WAIT_SECONDS(2);
		Sleep(2000);
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
enum{Ignore_0, Warning_1, Abort_2};
enum{CLOSE_0, OPEN_1};
void ApplyAlarmEffect(int nMonOpt)
{
	int nIOStatus;
	switch(nMonOpt)
	{
	case Ignore_0:
	case Warning_1:
		break;
	case Abort_2:
		WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);	// 0:Off 1:On
		break;
	}
}
//... 2019.01.17 Air Cooling Pressure Control Function
void AirCoolingConThread()
{
	double dbValuePrs1 = 0;
	double dbValuePrs2 = 0;
	double dbPrsHigh = 0;
	double dbPrsLow = 0;
	double dbPrsTimeOut = 0;
	int nIOStatus;
	int nPrs1CntChk = 0;
	int nPrs2CntChk = 0;
	char szDesMsg[256] = {0,};
	char szAlarm[256] = {0,};

	WAIT_SECONDS(10);

	while (1) {
		do {
			dbValuePrs1 = READ_ANALOG(ChuckCoolingOut1Prs, &nIOStatus);
			dbValuePrs2 = READ_ANALOG(ChuckCoolingOut2Prs, &nIOStatus);
			dbPrsHigh	= READ_ANALOG(PRMA_COOLPRS_HIGH, &nIOStatus);
			dbPrsLow	= READ_ANALOG(PRMA_COOLPRS_LOW, &nIOStatus);

			if(dbValuePrs1 > dbPrsHigh || dbValuePrs1 < dbPrsLow ) nPrs1CntChk++;
			else nPrs1CntChk = 0;

			if(dbValuePrs2 > dbPrsHigh || dbValuePrs2 < dbPrsLow ) nPrs2CntChk++;
			else nPrs2CntChk = 0;

			if(nPrs1CntChk > MAX_RECHECK_CNT)
			{
				nPrs1CntChk = 0;
				if(Ignore_0 != READ_DIGITAL(PRMD_HO_ChuckCoolPrs,&nIOStatus))
				{
					if(OPEN_1 == READ_DIGITAL(AirCoolingVlv, &nIOStatus))WRITE_DIGITAL(AirCoolingVlv,CLOSE_0,&nIOStatus);
					ALARM_MESSAGE_GET(ALARM_AIR_COOLING1_PRESSURE,szAlarm);
					sprintf(szDesMsg,"%s[Cur Prs: %0.3f HighLimit: %0.3f LowLimit: %0.3f]",szAlarm,dbValuePrs1,dbPrsHigh,dbPrsLow);
					ALARM_MESSAGE_SET(ALARM_AIR_COOLING1_PRESSURE,szDesMsg);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_AIR_COOLING1_PRESSURE))ALARM_POST(ALARM_AIR_COOLING1_PRESSURE);
				}
				ApplyAlarmEffect(READ_DIGITAL(PRMD_HO_ChuckCoolPrs,&nIOStatus));
			}
			else if(nPrs2CntChk > MAX_RECHECK_CNT)
			{
				nPrs2CntChk = 0;
				if(Ignore_0 != READ_DIGITAL(PRMD_HO_ChuckCoolPrs,&nIOStatus))
				{
					if(OPEN_1 == READ_DIGITAL(AirCoolingVlv, &nIOStatus))WRITE_DIGITAL(AirCoolingVlv,CLOSE_0,&nIOStatus);
					ALARM_MESSAGE_GET(ALARM_AIR_COOLING2_PRESSURE,szAlarm);
					sprintf(szDesMsg,"%s[Cur Prs: %0.3f HighLimit: %0.3f LowLimit: %0.3f]",szAlarm,dbValuePrs2,dbPrsHigh,dbPrsLow);
					ALARM_MESSAGE_SET(ALARM_AIR_COOLING2_PRESSURE,szDesMsg);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_AIR_COOLING2_PRESSURE))ALARM_POST(ALARM_AIR_COOLING2_PRESSURE);
				}
				ApplyAlarmEffect(READ_DIGITAL(PRMD_HO_ChuckCoolPrs,&nIOStatus));
			}
			else
			{
				if(nPrs1CntChk == 0 && nPrs2CntChk == 0 )
				{
					if(Ignore_0 != READ_DIGITAL(PRMD_HO_ChuckCoolPrs,&nIOStatus))
					{
						if(CLOSE_0 == READ_DIGITAL(AirCoolingVlv, &nIOStatus))WRITE_DIGITAL(AirCoolingVlv,OPEN_1,&nIOStatus);
					}
				}
			}
			Sleep(1000);
		}while(1);
		Sleep(2000);
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Initialize Function
#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\PMCConfig.ini"

//------------------------------------------------------------------------------------------
// Driver Monitoring Option
BOOL g_bMks651DriverMon			= TRUE;
BOOL g_bDeviceNetDriverMon		= TRUE;

void ParseDriverMonOption(char *szData)
{
	char szItem[40] = "";
	char szValue[40] = "";
	BOOL bEnable = TRUE;

	CTextParser tp(szData);
	do {
		if(!tp.GetWord(szItem)) break;
		if(!tp.GetWord(szValue)) break;
		tp.MakeLower(szValue);

		if     (strcmp(szValue, "enable") == 0)  bEnable = TRUE;
		else if(strcmp(szValue, "disable") == 0) bEnable = FALSE;

		if     (strcmp(szItem, "Mks651DriverMonitor") == 0)			g_bMks651DriverMon    = bEnable;
		else if(strcmp(szItem, "DeviceNetDriverMonitor") == 0)		g_bDeviceNetDriverMon = bEnable;
		//
		else if(strcmp(szItem, "Mks651DriverComm_Retry") == 0)		g_nMksCommErrRetry    = atoi(szValue);
		else if(strcmp(szItem, "DeviceNetDriverComm_Retry") == 0)	g_nDN_CommErrRetry    = atoi(szValue);
		else
		{
			printf("-----> Driver Status Monitoring Configuration is Invalid[%s]\n", szData);
		}
	} while(0);
}

//------------------------------------------------------------------------------------------
void InitPmcConfigData()
{

}

BOOL ReadPmcConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(PMC_CONFIG_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("Driver CommStatus Monitoring", szItem) == 0)			nTitle = 2;
				else															nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 2: ParseDriverMonOption(szRead);   break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s\n", PMC_CONFIG_FILE);
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Initialize()
{
	BOOL bRet = TRUE;
	printf("Start Initializing in %s\n", APP_NAME);

	InitPmcConfigData();
	ReadPmcConfig();

	// IO-Init
	int nIOStatus;
	WRITE_DIGITAL(VRCP_StaStpDO, 0, &nIOStatus);

	// VacuumStateDM => 0:ATM 1:Vacuum 2:GoATM 3:GoVacuum 4:Abort
	int    nVacSts  = 1;
	double dbChmPrs = READ_ANALOG(ChmPrsAI, &nIOStatus);
	if     (dbChmPrs < 50)  nVacSts = 1;
	else if(dbChmPrs > 600) nVacSts = 0;
	else                    nVacSts = 4;
	WRITE_DIGITAL(VacuumStateDM, nVacSts, &nIOStatus);

	_beginthread((_THREAD_FUNC) CommonMonitor_Thread, 0, NULL);

	// Interlock Monitoring
	_beginthread((_THREAD_FUNC)SWInterlockThread, 0, NULL);
	_beginthread((_THREAD_FUNC)SWModuleFailThread, 0, NULL);

	if(g_bMks651DriverMon)    _beginthread((_THREAD_FUNC) MKSCommStsMonThread   , 0, NULL);
	if(g_bDeviceNetDriverMon) _beginthread((_THREAD_FUNC) DN_CommStsMonThread   , 0, NULL);

	//...
	if(EN_ON == READ_DIGITAL(RPG_UseDM, &nIOStatus))
	{
		_beginthread((_THREAD_FUNC)RPGStsMonThread, 0, NULL);
	}

	//... Air Cooling Pressure/Valve Monitoring 기능 추가
	_beginthread((_THREAD_FUNC)AirCoolingConThread, 0, NULL);

	return bRet;
}

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int nParLen = 0;
	char *pParam;
//	char szCmd[41], szSubParam[256];

	// printf("Entered %s\n", APP_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);
	} while(0);
	// printf("Leaved %s\n", APP_NAME);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		// Deinitialize
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	/*
	char szParam[256];
	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	#ifdef _DEBUG
	printf("Argument = %s in Program_Enter_Code\n", szParam);
	#endif
	*/

	//------------------------------------------------------
	// 2006.03.20
	SYSTEM_KILL_WITH_MESSAGE();
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	Initialize();

	return TRUE;
}

} // End of [extern "C"]
//==========================================================================================
