///////////////////////////////////////////////////////////////////////////////
// Heater MoniECr
// Create : 2004.08.18

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

//////////////////////////////////////////////////////////////////////////
#define FUNC_NAME	"MFCPressureMonitor"
#define MAX_MFC		9
#define PARAMETER_READ_INTERVAL		30		// Loop Count (1 sec in MFC 9 EA)

//////////////////////////////////////////////////////////////////////////
#define ALARM_MFC01_PRS_HIGH_LIMIT			2070	
#define ALARM_MFC01_PRS_LOW_LIMIT			2100

//////////////////////////////////////////////////////////////////////////
enum {eStop,	eRunning};
enum {eOFF_0,	eON_1};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {

	//...
	{ "EasyCluster_Connect"	, _K_D_IO	,	EasyCluster_Connect,	0	} ,

	{ "SW_GbIntlckDM"		, _K_D_IO	,	SW_GbIntlckDM,			0	} ,
	{ "SW_ModuleFailDM"		, _K_D_IO	,	SW_ModuleFailDM,		0	} ,
	{ "PRO_FlowControlDM"	, _K_D_IO	,	PRO_FlowControlDM,		0	} ,
	//... 2016.08.20
	{ "SysLeakChkDM"		, _K_D_IO	,	SysLeakChkDM,			0	} ,
	{ "PRO_RunStsDM"		, _K_D_IO	,	PRO_RunStsDM,			0	} ,
	//...
	{ "MFC01_UseDM"			, _K_D_IO	,	MFC01_UseDM,			0	} ,
	{ "MFC02_UseDM"			, _K_D_IO	,	MFC02_UseDM,			0	} ,
	{ "MFC03_UseDM"			, _K_D_IO	,	MFC03_UseDM,			0	} ,
	{ "MFC04_UseDM"			, _K_D_IO	,	MFC04_UseDM,			0	} ,
	{ "MFC05_UseDM"			, _K_D_IO	,	MFC05_UseDM,			0	} ,
	{ "MFC06_UseDM"			, _K_D_IO	,	MFC06_UseDM,			0	} ,
	{ "MFC07_UseDM"			, _K_D_IO	,	MFC07_UseDM,			0	} ,
	{ "MFC08_UseDM"			, _K_D_IO	,	MFC08_UseDM,			0	} ,
	{ "MFC09_UseDM"			, _K_D_IO	,	MFC09_UseDM,			0	} ,

	//...
	{ "MFC01_MonOptDM"		, _K_D_IO	,	MFC01_MonOptDM,			0	} ,
	{ "MFC02_MonOptDM"		, _K_D_IO	,	MFC02_MonOptDM,			0	} ,
	{ "MFC03_MonOptDM"		, _K_D_IO	,	MFC03_MonOptDM,			0	} ,
	{ "MFC04_MonOptDM"		, _K_D_IO	,	MFC04_MonOptDM,			0	} ,
	{ "MFC05_MonOptDM"		, _K_D_IO	,	MFC05_MonOptDM,			0	} ,
	{ "MFC06_MonOptDM"		, _K_D_IO	,	MFC06_MonOptDM,			0	} ,
	{ "MFC07_MonOptDM"		, _K_D_IO	,	MFC07_MonOptDM,			0	} ,
	{ "MFC08_MonOptDM"		, _K_D_IO	,	MFC08_MonOptDM,			0	} ,
	{ "MFC09_MonOptDM"		, _K_D_IO	,	MFC09_MonOptDM,			0	} ,

	//...
	{ "PRMA_TL_MFC01PrsHi"	, _K_A_IO	,	PRMA_TL_MFC01PrsHi,		0	} ,
	{ "PRMA_TL_MFC02PrsHi"	, _K_A_IO	,	PRMA_TL_MFC02PrsHi,		0	} ,
	{ "PRMA_TL_MFC03PrsHi"	, _K_A_IO	,	PRMA_TL_MFC03PrsHi,		0	} ,
	{ "PRMA_TL_MFC04PrsHi"	, _K_A_IO	,	PRMA_TL_MFC04PrsHi,		0	} ,
	{ "PRMA_TL_MFC05PrsHi"	, _K_A_IO	,	PRMA_TL_MFC05PrsHi,		0	} ,
	{ "PRMA_TL_MFC06PrsHi"	, _K_A_IO	,	PRMA_TL_MFC06PrsHi,		0	} ,
	{ "PRMA_TL_MFC07PrsHi"	, _K_A_IO	,	PRMA_TL_MFC07PrsHi,		0	} ,
	{ "PRMA_TL_MFC08PrsHi"	, _K_A_IO	,	PRMA_TL_MFC08PrsHi,		0	} ,
	{ "PRMA_TL_MFC09PrsHi"	, _K_A_IO	,	PRMA_TL_MFC09PrsHi,		0	} ,

	//...
	{ "PRMA_TL_MFC01PrsLw"	, _K_A_IO	,	PRMA_TL_MFC01PrsLw,		0	} ,
	{ "PRMA_TL_MFC02PrsLw"	, _K_A_IO	,	PRMA_TL_MFC02PrsLw,		0	} ,
	{ "PRMA_TL_MFC03PrsLw"	, _K_A_IO	,	PRMA_TL_MFC03PrsLw,		0	} ,
	{ "PRMA_TL_MFC04PrsLw"	, _K_A_IO	,	PRMA_TL_MFC04PrsLw,		0	} ,
	{ "PRMA_TL_MFC05PrsLw"	, _K_A_IO	,	PRMA_TL_MFC05PrsLw,		0	} ,
	{ "PRMA_TL_MFC06PrsLw"	, _K_A_IO	,	PRMA_TL_MFC06PrsLw,		0	} ,
	{ "PRMA_TL_MFC07PrsLw"	, _K_A_IO	,	PRMA_TL_MFC07PrsLw,		0	} ,
	{ "PRMA_TL_MFC08PrsLw"	, _K_A_IO	,	PRMA_TL_MFC08PrsLw,		0	} ,
	{ "PRMA_TL_MFC09PrsLw"	, _K_A_IO	,	PRMA_TL_MFC09PrsLw,		0	} ,

	//...
	{ "MFC01PressAI" 		, _K_A_IO	,	MFC01PressAI,			0	} ,
	{ "MFC02PressAI" 		, _K_A_IO	,	MFC02PressAI,			0	} ,
	{ "MFC03PressAI" 		, _K_A_IO	,	MFC03PressAI,			0	} ,
	{ "MFC04PressAI" 		, _K_A_IO	,	MFC04PressAI,			0	} ,
	{ "MFC05PressAI" 		, _K_A_IO	,	MFC05PressAI,			0	} ,
	{ "MFC06PressAI" 		, _K_A_IO	,	MFC06PressAI,			0	} ,
	{ "MFC07PressAI"		, _K_A_IO	,	MFC07PressAI,			0	} ,
	{ "MFC08PressAI"		, _K_A_IO	,	MFC08PressAI,			0	} ,
	{ "MFC09PressAI"		, _K_A_IO	,	MFC09PressAI,			0	} ,

	//...
	{ "V1ValveDO"			, _K_D_IO	,	MFC01_FontVlv,			0	} ,
	{ "V3ValveDO"			, _K_D_IO	,	MFC02_FontVlv,			0	} ,
	{ "V5ValveDO"			, _K_D_IO	,	MFC03_FontVlv,			0	} ,
	{ "V49ValveDO"			, _K_D_IO	,	MFC04_FontVlv,			0	} ,
	{ "V52ValveDO"			, _K_D_IO	,	MFC05_FontVlv,			0	} ,
	{ "V54ValveDO"			, _K_D_IO	,	MFC06_FontVlv,			0	} ,
	{ "V56ValveDO"			, _K_D_IO	,	MFC07_FontVlv,			0	} ,
	{ "V58ValveDO"			, _K_D_IO	,	MFC08_FontVlv,			0	} ,
	{ "V60ValveDO"			, _K_D_IO	,	MFC09_FontVlv,			0	} ,

	{ "PRMA_EC_MFC01" 		, _K_A_IO	,	PRMA_EC_MFC01,			0	} ,
	{ "PRMA_EC_MFC02" 		, _K_A_IO	,	PRMA_EC_MFC02,			0	} ,
	{ "PRMA_EC_MFC03" 		, _K_A_IO	,	PRMA_EC_MFC03,			0	} ,
	{ "PRMA_EC_MFC04" 		, _K_A_IO	,	PRMA_EC_MFC04,			0	} ,
	{ "PRMA_EC_MFC05" 		, _K_A_IO	,	PRMA_EC_MFC05,			0	} ,
	{ "PRMA_EC_MFC06" 		, _K_A_IO	,	PRMA_EC_MFC06,			0	} ,
	{ "PRMA_EC_MFC07"		, _K_A_IO	,	PRMA_EC_MFC07,			0	} ,
	{ "PRMA_EC_MFC08"		, _K_A_IO	,	PRMA_EC_MFC08,			0	} ,
	{ "PRMA_EC_MFC09"		, _K_A_IO	,	PRMA_EC_MFC09,			0	} ,
	{ "PRMA_EC_MFC10"		, _K_A_IO	,	PRMA_EC_MFC10,			0	} ,

	""
};

//////////////////////////////////////////////////////////////////////////
void ApplyAlarmEffect(int nMonOpt)
{
	enum { ALM_IGNORE, ALM_WARING, ALM_CHMDIS, ALM_ABORT, ALM_RETRY };
	enum { PRO_ABORT, PRO_RETRY };
	enum { INTLK_OFF, INTLK_ON };

	int nIOStatus;
	switch(nMonOpt)
	{
	case ALM_IGNORE:
	case ALM_WARING:
		break;
	case ALM_CHMDIS:
		WRITE_DIGITAL(SW_ModuleFailDM, INTLK_ON, &nIOStatus);
		break;
	case ALM_ABORT:
		WRITE_DIGITAL(SW_GbIntlckDM, INTLK_ON, &nIOStatus);
	case ALM_RETRY:
		WRITE_DIGITAL(PRO_FlowControlDM, PRO_RETRY, &nIOStatus);
		WRITE_DIGITAL(SW_GbIntlckDM, INTLK_ON, &nIOStatus);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
Module_Status StartMonitor(char *pParam)
{
	enum { MFC_NotUSE, MFC_USE };
	enum { Vlv_CLOSE, Vlv_OPEN };
	enum { MONOPT_IGNORE, MONOPT_WARING, MONOPT_CHMDIS, MONOPT_ABORT, MONOPT_RETRY };
	enum { PM_DISCON, PM_CONNECT, PM_ENABLE, PM_DISABLE, PM_DISABLE_HW };

	int nIOStatus;
	int i;

	int	nPMStatus = PM_ENABLE;
	int	nLoopCount = 0;

	int	anUseDM[MAX_MFC];
	int	anMonOpt[MAX_MFC];
	int anTimeOut[MAX_MFC];		// TimeOut
	int anLowOutCnt[MAX_MFC];	// Low Prs OutCheck
	int anHighOutCnt[MAX_MFC];	// High Prs OutCheck
	int	nFronVlvSts;

	double dbMFCPrs;	
	double dbCfgHigh[MAX_MFC];
	double dbCfgLow[MAX_MFC];

	//...
	while(1)
	{
		Sleep(100);

		//////////////////////////////////////////////////////////////////////////
		//... Check PM Mode Status for PM Enable
		nPMStatus = READ_DIGITAL(EasyCluster_Connect, &nIOStatus);
		if(PM_CONNECT != nPMStatus && PM_ENABLE != nPMStatus)
		{
			nLoopCount = 0;
			Sleep(1000);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//...2016.08.20 Skip System Leak Check 
		if(eON_1 == READ_DIGITAL(SysLeakChkDM, &nIOStatus)
			&& eStop == READ_DIGITAL(PRO_RunStsDM, &nIOStatus))
		{
			nLoopCount = 0;
			Sleep(1000);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Interval Parameter Read
		if(nLoopCount == 0)
		{	
			//... Update MFC Config Channel
			for(i = 0; i < MAX_MFC; i++)
			{
				//... All Init
				anUseDM[i]		= MFC_NotUSE;		//... Init
				anMonOpt[i]		= MONOPT_WARING;	//... Init
				dbCfgLow[i]		= -20;
				dbCfgHigh[i]	= 100;
				anLowOutCnt[i]  = 0;
				anHighOutCnt[i] = 0;
				
				//... Check Parameter Read Skip Case
				anUseDM[i]  = READ_DIGITAL(MFC01_UseDM + i, &nIOStatus);
				if(MFC_NotUSE == anUseDM[i])		continue;
				
				anMonOpt[i] = READ_DIGITAL(MFC01_MonOptDM + i, &nIOStatus);
				if(MONOPT_IGNORE == anMonOpt[i])	continue;

				anTimeOut[i] = (int)READ_ANALOG(PRMA_EC_MFC01 + i, &nIOStatus);
				
				//... Read Parameter
				dbCfgLow[i]	 = READ_ANALOG(PRMA_TL_MFC01PrsLw + i, &nIOStatus);
				dbCfgHigh[i] = READ_ANALOG(PRMA_TL_MFC01PrsHi + i, &nIOStatus);
			}
		}

		//.... Interval Count Up
		if(nLoopCount < 0 || nLoopCount >= PARAMETER_READ_INTERVAL)		nLoopCount = 0;
		else															nLoopCount++;

		//////////////////////////////////////////////////////////////////////////
		//... Check MFCx_Pressure
		for(i = 0; i < MAX_MFC; i++)
		{
			//... Check Skip Case
			if(MFC_NotUSE == anUseDM[i])		continue;
			if(MONOPT_IGNORE == anMonOpt[i])	continue;

			//... Check Valve Status
			nFronVlvSts = READ_DIGITAL(MFC01_FontVlv + i, &nIOStatus);
			if(Vlv_OPEN == nFronVlvSts)
			{
				dbMFCPrs = READ_ANALOG(MFC01PressAI + i, &nIOStatus);
				
				//... Abnormal Case ReCheck Valve Status
				if(dbMFCPrs < dbCfgLow[i] || dbMFCPrs > dbCfgHigh[i])
				{
					nFronVlvSts = READ_DIGITAL(MFC01_FontVlv + i, &nIOStatus);
				}

				if(dbMFCPrs < dbCfgLow[i] && Vlv_OPEN == nFronVlvSts)
				{
					if (anLowOutCnt[i] >= anTimeOut[i])
					{
						nFronVlvSts = READ_DIGITAL(MFC01_FontVlv + i, &nIOStatus);
						if(ALM_PAUSED != ALARM_STATUS(ALARM_MFC01_PRS_LOW_LIMIT + i))
							ALARM_POST(ALARM_MFC01_PRS_LOW_LIMIT + i);
								 
						anLowOutCnt[i] = 0;		 
						ApplyAlarmEffect(anMonOpt[i]);
					}
					anLowOutCnt[i]++;
				}
				else	anLowOutCnt[i] = 0;

				if(dbMFCPrs > dbCfgHigh[i] && Vlv_OPEN == nFronVlvSts)
				{
					if (anHighOutCnt[i] >= anTimeOut[i])
					{
						if(ALM_PAUSED != ALARM_STATUS(ALARM_MFC01_PRS_HIGH_LIMIT + i))
							ALARM_POST(ALARM_MFC01_PRS_HIGH_LIMIT + i);
						   
						anHighOutCnt[i] = 0;   
						ApplyAlarmEffect(anMonOpt[i]);
					}
				}
				else	anHighOutCnt[i] = 0;
			}
			//...
			Sleep(100);
		}
	}

	return SYS_SUCCESS;
}

//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int nParLen = 0;
	char *pParam;

	printf("Entered %s\n", FUNC_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		msRet = StartMonitor(pParam);

	} while(0);

	printf("Leave %s\n", FUNC_NAME);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================


