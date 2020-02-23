//////////////////////////////////////////////////////////////////////////////
// Version : 1.0.1.0
// Modify : 2004.08.24

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

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
#include "TextLogDll_M.h"

#define APP_NAME	"MFCControl"
#define MAX_MFC		25
#define MAX_CHK_VALVE 2

//------------------------------------------------------------------------------------------
// Alarm Definition (2000 ~ 2199)

#define ALARM_MFC01_FLOW_OUT_OF_TOL	2001		// MFC01 ~ MFC20 => 2001 ~ 2025
#define ALARM_MFC02_FLOW_OUT_OF_TOL	2002

#define ALARM_MFC01_SET_TIMEOUT		2041		// MFC01 ~ MFC20 => 2041 ~ 2065
#define ALARM_MFC02_SET_TIMEOUT		2042
#define ALARM_SOURCE_PRESSURE_ERROR 1090
//------------------------------------------------------------------------------------------
// Enumeration  Definition

enum {
	VLV_CLOSE, VLV_OPEN
};

enum {
	MFCV_SP, MFCV_OPEN, MFCV_CLOSE
};

// 2006.07.04
enum {
	AL_IGNORE, AL_WARNING, AL_CHMDIS, AL_ABORT, AL_RETRY
};
//... 2019.01.16
enum {
	Unknown_0, Proc_1,	PreProc_2,	PostProc_3,	Idle_4	,Clean_5, Purge_6,	LotPre_7, LotPos
};

//... 2019.01.16
enum {
	ONLY_PRC_0,	ALL_RECIPE_1
};


char g_szValveIO[MAX_CHK_VALVE][64] = {0,};
double g_dbParmLow   = 0;
double g_dbParmHigh  = 0;
double g_dbParmChkTO = 0;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_ModuleFailDM",	 _K_D_IO	,	SW_ModuleFailDM,	0	} ,
	{ "PRO_FlowControlDM",	 _K_D_IO	,	PRO_FlowControlDM,	0	} ,

	{ "MFC01StPtAO"	, _K_A_IO	,	MFC01StPt,	0 },
	{ "MFC02StPtAO"	, _K_A_IO	,	MFC02StPt,	0 },
	{ "MFC03StPtAO"	, _K_A_IO	,	MFC03StPt,	0 },
	{ "MFC04StPtAO"	, _K_A_IO	,	MFC04StPt,	0 },
	{ "MFC05StPtAO"	, _K_A_IO	,	MFC05StPt,	0 },
	{ "MFC06StPtAO"	, _K_A_IO	,	MFC06StPt,	0 },
	{ "MFC07StPtAO"	, _K_A_IO	,	MFC07StPt,	0 },
	{ "MFC08StPtAO"	, _K_A_IO	,	MFC08StPt,	0 },
	{ "MFC09StPtAO"	, _K_A_IO	,	MFC09StPt,	0 },
	{ "MFC10StPtAO"	, _K_A_IO	,	MFC10StPt,	0 },
	{ "MFC11StPtAO"	, _K_A_IO	,	MFC11StPt,	0 },
	{ "MFC12StPtAO"	, _K_A_IO	,	MFC12StPt,	0 },
	{ "MFC13StPtAO"	, _K_A_IO	,	MFC13StPt,	0 },
	{ "MFC14StPtAO"	, _K_A_IO	,	MFC14StPt,	0 },
	{ "MFC15StPtAO"	, _K_A_IO	,	MFC15StPt,	0 },
	{ "MFC16StPtAO"	, _K_A_IO	,	MFC16StPt,	0 },
	{ "MFC17StPtAO"	, _K_A_IO	,	MFC17StPt,	0 },
	{ "MFC18StPtAO"	, _K_A_IO	,	MFC18StPt,	0 },
	{ "MFC19StPtAO"	, _K_A_IO	,	MFC19StPt,	0 },
	{ "MFC20StPtAO"	, _K_A_IO	,	MFC20StPt,	0 },
	{ "MFC21StPtAO"	, _K_A_IO	,	MFC21StPt,	0 },
	{ "MFC22StPtAO"	, _K_A_IO	,	MFC22StPt,	0 },
	{ "MFC23StPtAO"	, _K_A_IO	,	MFC23StPt,	0 },
	{ "MFC24StPtAO"	, _K_A_IO	,	MFC24StPt,	0 },
	{ "MFC25StPtAO"	, _K_A_IO	,	MFC25StPt,	0 },

	{ "MFC01FlwAI"	, _K_A_IO	,	MFC01Flw,	0 },
	{ "MFC02FlwAI"	, _K_A_IO	,	MFC02Flw,	0 },
	{ "MFC03FlwAI"	, _K_A_IO	,	MFC03Flw,	0 },
	{ "MFC04FlwAI"	, _K_A_IO	,	MFC04Flw,	0 },
	{ "MFC05FlwAI"	, _K_A_IO	,	MFC05Flw,	0 },
	{ "MFC06FlwAI"	, _K_A_IO	,	MFC06Flw,	0 },
	{ "MFC07FlwAI"	, _K_A_IO	,	MFC07Flw,	0 },
	{ "MFC08FlwAI"	, _K_A_IO	,	MFC08Flw,	0 },
	{ "MFC09FlwAI"	, _K_A_IO	,	MFC09Flw,	0 },
	{ "MFC10FlwAI"	, _K_A_IO	,	MFC10Flw,	0 },
	{ "MFC11FlwAI"	, _K_A_IO	,	MFC11Flw,	0 },
	{ "MFC12FlwAI"	, _K_A_IO	,	MFC12Flw,	0 },
	{ "MFC13FlwAI"	, _K_A_IO	,	MFC13Flw,	0 },
	{ "MFC14FlwAI"	, _K_A_IO	,	MFC14Flw,	0 },
	{ "MFC15FlwAI"	, _K_A_IO	,	MFC15Flw,	0 },
	{ "MFC16FlwAI"	, _K_A_IO	,	MFC16Flw,	0 },
	{ "MFC17FlwAI"	, _K_A_IO	,	MFC17Flw,	0 },
	{ "MFC18FlwAI"	, _K_A_IO	,	MFC18Flw,	0 },
	{ "MFC19FlwAI"	, _K_A_IO	,	MFC19Flw,	0 },
	{ "MFC20FlwAI"	, _K_A_IO	,	MFC20Flw,	0 },
	{ "MFC21FlwAI"	, _K_A_IO	,	MFC21Flw,	0 },
	{ "MFC22FlwAI"	, _K_A_IO	,	MFC22Flw,	0 },
	{ "MFC23FlwAI"	, _K_A_IO	,	MFC23Flw,	0 },
	{ "MFC24FlwAI"	, _K_A_IO	,	MFC24Flw,	0 },
	{ "MFC25FlwAI"	, _K_A_IO	,	MFC25Flw,	0 },

	{ "PRMA_TL_MFC01Set"	, _K_A_IO	,	PRMA_TL_MFC01Set,	0 },
	{ "PRMA_TL_MFC02Set"	, _K_A_IO	,	PRMA_TL_MFC02Set,	0 },
	{ "PRMA_TL_MFC03Set"	, _K_A_IO	,	PRMA_TL_MFC03Set,	0 },
	{ "PRMA_TL_MFC04Set"	, _K_A_IO	,	PRMA_TL_MFC04Set,	0 },
	{ "PRMA_TL_MFC05Set"	, _K_A_IO	,	PRMA_TL_MFC05Set,	0 },
	{ "PRMA_TL_MFC06Set"	, _K_A_IO	,	PRMA_TL_MFC06Set,	0 },
	{ "PRMA_TL_MFC07Set"	, _K_A_IO	,	PRMA_TL_MFC07Set,	0 },
	{ "PRMA_TL_MFC08Set"	, _K_A_IO	,	PRMA_TL_MFC08Set,	0 },
	{ "PRMA_TL_MFC09Set"	, _K_A_IO	,	PRMA_TL_MFC09Set,	0 },
	{ "PRMA_TL_MFC10Set"	, _K_A_IO	,	PRMA_TL_MFC10Set,	0 },
	{ "PRMA_TL_MFC11Set"	, _K_A_IO	,	PRMA_TL_MFC11Set,	0 },
	{ "PRMA_TL_MFC12Set"	, _K_A_IO	,	PRMA_TL_MFC12Set,	0 },
	{ "PRMA_TL_MFC13Set"	, _K_A_IO	,	PRMA_TL_MFC13Set,	0 },
	{ "PRMA_TL_MFC14Set"	, _K_A_IO	,	PRMA_TL_MFC14Set,	0 },
	{ "PRMA_TL_MFC15Set"	, _K_A_IO	,	PRMA_TL_MFC15Set,	0 },
	{ "PRMA_TL_MFC16Set"	, _K_A_IO	,	PRMA_TL_MFC16Set,	0 },
	{ "PRMA_TL_MFC17Set"	, _K_A_IO	,	PRMA_TL_MFC17Set,	0 },
	{ "PRMA_TL_MFC18Set"	, _K_A_IO	,	PRMA_TL_MFC18Set,	0 },
	{ "PRMA_TL_MFC19Set"	, _K_A_IO	,	PRMA_TL_MFC19Set,	0 },
	{ "PRMA_TL_MFC20Set"	, _K_A_IO	,	PRMA_TL_MFC20Set,	0 },
	{ "PRMA_TL_MFC21Set"	, _K_A_IO	,	PRMA_TL_MFC21Set,	0 },
	{ "PRMA_TL_MFC22Set"	, _K_A_IO	,	PRMA_TL_MFC22Set,	0 },
	{ "PRMA_TL_MFC23Set"	, _K_A_IO	,	PRMA_TL_MFC23Set,	0 },
	{ "PRMA_TL_MFC24Set"	, _K_A_IO	,	PRMA_TL_MFC24Set,	0 },
	{ "PRMA_TL_MFC25Set"	, _K_A_IO	,	PRMA_TL_MFC25Set,	0 },

	{ "PRMA_TO_MFC01"		, _K_A_IO	,	PRMA_TO_MFC01,	0	} ,
	{ "PRMA_TO_MFC02"		, _K_A_IO	,	PRMA_TO_MFC02,	0	} ,
	{ "PRMA_TO_MFC03"		, _K_A_IO	,	PRMA_TO_MFC03,	0	} ,
	{ "PRMA_TO_MFC04"		, _K_A_IO	,	PRMA_TO_MFC04,	0	} ,
	{ "PRMA_TO_MFC05"		, _K_A_IO	,	PRMA_TO_MFC05,	0	} ,		// 2006.03.23
	{ "PRMA_TO_MFC06"		, _K_A_IO	,	PRMA_TO_MFC06,	0	} ,
	{ "PRMA_TO_MFC07"		, _K_A_IO	,	PRMA_TO_MFC07,	0	} ,
	{ "PRMA_TO_MFC08"		, _K_A_IO	,	PRMA_TO_MFC08,	0	} ,
	{ "PRMA_TO_MFC09"		, _K_A_IO	,	PRMA_TO_MFC09,	0	} ,
	{ "PRMA_TO_MFC10"		, _K_A_IO	,	PRMA_TO_MFC10,	0	} ,
	{ "PRMA_TO_MFC11"		, _K_A_IO	,	PRMA_TO_MFC11,	0	} ,
	{ "PRMA_TO_MFC12"		, _K_A_IO	,	PRMA_TO_MFC12,	0	} ,
	{ "PRMA_TO_MFC13"		, _K_A_IO	,	PRMA_TO_MFC13,	0	} ,
	{ "PRMA_TO_MFC14"		, _K_A_IO	,	PRMA_TO_MFC14,	0	} ,
	{ "PRMA_TO_MFC15"		, _K_A_IO	,	PRMA_TO_MFC15,	0	} ,
	{ "PRMA_TO_MFC16"		, _K_A_IO	,	PRMA_TO_MFC16,	0	} ,
	{ "PRMA_TO_MFC17"		, _K_A_IO	,	PRMA_TO_MFC17,	0	} ,
	{ "PRMA_TO_MFC18"		, _K_A_IO	,	PRMA_TO_MFC18,	0	} ,
	{ "PRMA_TO_MFC19"		, _K_A_IO	,	PRMA_TO_MFC19,	0	} ,
	{ "PRMA_TO_MFC20"		, _K_A_IO	,	PRMA_TO_MFC20,	0	} ,
	{ "PRMA_TO_MFC21"		, _K_A_IO	,	PRMA_TO_MFC21,	0	} ,
	{ "PRMA_TO_MFC22"		, _K_A_IO	,	PRMA_TO_MFC22,	0	} ,
	{ "PRMA_TO_MFC23"		, _K_A_IO	,	PRMA_TO_MFC23,	0	} ,
	{ "PRMA_TO_MFC24"		, _K_A_IO	,	PRMA_TO_MFC24,	0	} ,
	{ "PRMA_TO_MFC25"		, _K_A_IO	,	PRMA_TO_MFC25,	0	} ,

	{ "PRMA_EC_MFC01"		, _K_A_IO	,	PRMA_EC_MFC01,	0 },
	{ "PRMA_EC_MFC02"		, _K_A_IO	,	PRMA_EC_MFC02,	0 },
	{ "PRMA_EC_MFC03"		, _K_A_IO	,	PRMA_EC_MFC03,	0 },
	{ "PRMA_EC_MFC04"		, _K_A_IO	,	PRMA_EC_MFC04,	0 },
	{ "PRMA_EC_MFC05"		, _K_A_IO	,	PRMA_EC_MFC05,	0 },		// 2006.03.23
	{ "PRMA_EC_MFC06"		, _K_A_IO	,	PRMA_EC_MFC06,	0 },
	{ "PRMA_EC_MFC07"		, _K_A_IO	,	PRMA_EC_MFC07,	0 },
	{ "PRMA_EC_MFC08"		, _K_A_IO	,	PRMA_EC_MFC08,	0 },
	{ "PRMA_EC_MFC09"		, _K_A_IO	,	PRMA_EC_MFC09,	0 },
	{ "PRMA_EC_MFC10"		, _K_A_IO	,	PRMA_EC_MFC10,	0 },
	{ "PRMA_EC_MFC11"		, _K_A_IO	,	PRMA_EC_MFC11,	0 },
	{ "PRMA_EC_MFC12"		, _K_A_IO	,	PRMA_EC_MFC12,	0 },
	{ "PRMA_EC_MFC13"		, _K_A_IO	,	PRMA_EC_MFC13,	0 },
	{ "PRMA_EC_MFC14"		, _K_A_IO	,	PRMA_EC_MFC14,	0 },
	{ "PRMA_EC_MFC15"		, _K_A_IO	,	PRMA_EC_MFC15,	0 },
	{ "PRMA_EC_MFC16"		, _K_A_IO	,	PRMA_EC_MFC16,	0 },
	{ "PRMA_EC_MFC17"		, _K_A_IO	,	PRMA_EC_MFC17,	0 },
	{ "PRMA_EC_MFC18"		, _K_A_IO	,	PRMA_EC_MFC18,	0 },
	{ "PRMA_EC_MFC19"		, _K_A_IO	,	PRMA_EC_MFC19,	0 },
	{ "PRMA_EC_MFC20"		, _K_A_IO	,	PRMA_EC_MFC20,	0 },
	{ "PRMA_EC_MFC21"		, _K_A_IO	,	PRMA_EC_MFC21,	0 },
	{ "PRMA_EC_MFC22"		, _K_A_IO	,	PRMA_EC_MFC22,	0 },
	{ "PRMA_EC_MFC23"		, _K_A_IO	,	PRMA_EC_MFC23,	0 },
	{ "PRMA_EC_MFC24"		, _K_A_IO	,	PRMA_EC_MFC24,	0 },
	{ "PRMA_EC_MFC25"		, _K_A_IO	,	PRMA_EC_MFC25,	0 },
	//... 2018.06.02
	{ "PRMD_CARRIER_MFC"	, _K_D_IO	,	PRMD_CARRIER_MFC,	 0 },
	{ "PRMD_CANISTER_VALVE1", _K_D_IO	,	PRMD_CANISTER_VALVE1,0 },
	{ "PRMD_CANISTER_VALVE2", _K_D_IO	,	PRMD_CANISTER_VALVE2,0 },
	{ "PRMD_SOUREPRS_OPTION", _K_D_IO	,	PRMD_SOUREPRS_OPTION,0 },
	{ "PRMA_SRCPRESSURE_LOW"	, _K_A_IO	,	PRMA_SRCPRESSURE_LOW,		0 },
	{ "PRMA_SRCPRESSURE_HIGH"	, _K_A_IO	,	PRMA_SRCPRESSURE_HIGH,		0 },
	{ "PRMA_SRCPRESSURE_TIMEOUT", _K_A_IO	,	PRMA_SRCPRESSURE_TIMEOUT,	0 },
	{ "PrsGauge1AI"				, _K_A_IO	,	PrsGauge1AI				,	0 },
	//... 2019.01.16 Process Recipe 진행 시에만 Source Pressure Interlock 감지 하도록 수정
	{ "PRO_RcpTypeDM"			, _K_D_IO	,	PRO_RcpTypeDM			,	0 },
	{ "PRMD_INT_RCP"			, _K_D_IO	,	PRMD_INT_RCP			,	0 },
	""
};

//------------------------------------------------------------------------------------------
// 2007.08.10
#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\PMCConfig.ini"
BOOL g_abCloseAllList[MAX_MFC] = 
{
	TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE, TRUE
};

//------------------------------------------------------------------------------------------
typedef struct MFC_PARAM_STRUCTURE
{
	int nMfcNum;
	double dbSetVal;
	int nSetTimeout;
	int nMonErrCnt;
	BOOL bMon;
	double dbTol;
	
	BOOL bErr;
	int nOption;
	BOOL bSetCompt;
} MFC_PARAM_STRUCT;

//--------------------------------------------------------------------
// 2006.06.28
enum { ALMOPT_IGNORE, ALMOPT_WARNING, ALMOPT_CHMDIS, ALMOPT_ABORT, ALMOPT_RETRY };

//------------------------------------------------------------------------------------------
// Alarm(Hazard) Option Function
// return TRUE : Sequence Is Continuous
// return FALSE : Alarm Is Valid And Sequence Aborted

BOOL CheckAlarmOption(int nAlarm, int nOption)
{
	// 2006.06.28
	enum { PRO_ABORT, PRO_RETRY };
	enum { INTLCK_OFF, INTLCK_ON };

	BOOL bRet = TRUE;
	int nIOStatus;
	switch(nOption)
	{
	case ALMOPT_IGNORE: break;
	case ALMOPT_WARNING:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		break;
	case ALMOPT_CHMDIS:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		WRITE_DIGITAL(SW_ModuleFailDM, INTLCK_ON, &nIOStatus);
		break;
	case ALMOPT_ABORT:
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		bRet = FALSE;
		break;
	case ALMOPT_RETRY:		// 2006.06.28
		WRITE_DIGITAL(PRO_FlowControlDM, PRO_RETRY, &nIOStatus);
		if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
		bRet = FALSE;
	default: break;
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2015.01.23 Add Common Spec Notuse MFC DO Channel
//... Parameter Format : %d %d ... (Open MFC Number)
Module_Status FullOpen(char *pParam)
{
	char pszLocFtnName[] = "FullOpen";
	Module_Status msRet = SYS_SUCCESS;
	int nChannel;
	int nIOStatus;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	int nTimeoutT = 0;

	//... IO Shape Value
	int		nType	= 0;	// 0:Unknown, 1:Input_Only, 2:InOut Virtual, 3: InOut Local, 4:InOut Remote
	int		nInDev	= 0;
	int		nOutDev = 0;
	double	dbMin	= 0;
	double	dbMax	= 0;

	do {

		CTextParser tp(pParam);
		for(i = 0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0))
			{
				// End of MFC Parameter
				break;
			}
		}
		if(i <= 0)
		{
			printf("-----> 'Parsing MFC Parameter' failed in %s\n", pszLocFtnName);
			msRet = SYS_ERROR;
			break;
		}
		nMfcCount = i;

		//////////////////////////////////////////////////////////////////////////
		//... 2015.01.24 Change (MFC OPEN -> MAX FLOW) ... <2015 SEC Common Spec>
		//... Max Flow Set
		_LOG("-----> MFC Full Flow Set Started");
		for(i = 0; i < nMfcCount; i++)
		{
			nChannel = MFC01StPt + MfcPrm[i].nMfcNum - 1;

			//... Read Max Set Flow Value
			ANALOG_SHAPE(nChannel, &nType, &nInDev, &nOutDev, &dbMin, &dbMax);
			WRITE_ANALOG(nChannel, dbMax, &nIOStatus);	//... Set Max Flow

			printf("[DEBUG] [%d]Read MFC[%d] Setpoint ANALOG_SHAPE IoIdx[%d] Type[%d] Min[%.0f] Max[%.0f]\n",
				i+1, MfcPrm[i].nMfcNum, nChannel, nType, nInDev, dbMax);
		}
		_LOG("-----> MFC Full Flow Set Finished");

	} while(0);

	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status SetValue(char *pParam)
{
	char pszLocFtnName[] = "SetValue";
	Module_Status msRet = SYS_SUCCESS;
	int nChannel;
	int nIOStatus;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	int nTimeoutT = 0;
	double dbDiff, dbTol;
	double dbValue;
	BOOL bMon = FALSE;
	int nMntrErrCnt = 3;
	int nErrorCount = 0;
	int nTmCount = 0;
	int nMfcCheck;

	//... IO Shape Value
	int		nType	= 0;	// 0:Unknown, 1:Input_Only, 2:InOut Virtual, 3: InOut Local, 4:InOut Remote
	int		nInDev	= 0;
	int		nOutDev = 0;
	double	dbMin	= 0;
	double	dbMax	= 0;

	_LOG("---> MFC Set Function Entered(%s)", pParam);
	do {
		//////////////////////////////////////////////////////////////////////////
		// Read Param
		// printf("Call CTextParser tp( %s )\n", pParam);
		CTextParser tp(pParam);
		if(! tp.GetInt(&nTimeoutT, 0))
		{
			printf("-----> 'Parsing Timeout' failed in %s\n", pszLocFtnName);
			msRet = SYS_ERROR;
			break;
		}

		for(i = 0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0))
			{
				// End of MFC Parameter
				break;
			}
			if(! tp.GetDbFloat(& MfcPrm[i].dbSetVal, 0))
			{
				printf("-----> MFC[%d] 'Parsing SetVal' failed in %s\n", i+1, pszLocFtnName);
				break;
			}
			if(! tp.GetBOOL(& MfcPrm[i].bMon, 0))
			{
				printf("-----> MFC[%d] 'Parsing bMon' failed in %s\n", i+1, pszLocFtnName);
				break;
			}
			if(! tp.GetDbFloat(& MfcPrm[i].dbTol, 0))
			{
				printf("-----> MFC[%d] 'Parsing Tol' failed %s\n", i+1, pszLocFtnName);
				break;
			}

			#ifdef _DEBUG
			printf("MFC[%d]  Sp:%8.2f  bMon:%d  Tol:%5.1f\n", MfcPrm[i].nMfcNum, MfcPrm[i].dbSetVal, MfcPrm[i].bMon, MfcPrm[i].dbTol);
			#endif
			
			if(MfcPrm[i].bMon) bMon = TRUE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}

		if(i <= 0)
		{
			printf("-----> 'Parsing MFC Parameter' failed in %s\n", pszLocFtnName);
			msRet = SYS_ERROR;
			break;
		}
		nMfcCount = i;

		//////////////////////////////////////////////////////////////////////////
		//... Set MFC Flow
		_LOG("-----> MFC Set Started");
		for(i=0; i<nMfcCount; i++)
		{
			if(MfcPrm[i].dbSetVal >= 0)
			{
				nChannel = MFC01StPt + MfcPrm[i].nMfcNum - 1;
				dbValue = (double) MfcPrm[i].dbSetVal;
				WRITE_ANALOG(nChannel, dbValue, &nIOStatus);
			}
			else if(MfcPrm[i].dbSetVal == -1)
			{	//... Set Max Flow
				nChannel = MFC01StPt + MfcPrm[i].nMfcNum - 1;

				//... Read Max Set Flow Value
				ANALOG_SHAPE(nChannel, &nType, &nInDev, &nOutDev, &dbMin, &dbMax);
				WRITE_ANALOG(nChannel, dbMax, &nIOStatus);	//... Set Max Flow
			}
		}
		_LOG("-----> MFC Set Finished");
		
		//////////////////////////////////////////////////////////////////////////
		//
		if(bMon)
		{
			// Timeout Check
			_LOG("-----> MFC Monitoring Started");
			nTmCount = 0;
			while(nTmCount < nTimeoutT)
			{
				if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
				nMfcCheck = 0;
				for(i=0; i < nMfcCount; i++)
				{
					if(TRUE == MfcPrm[i].bMon && MfcPrm[i].dbSetVal > 0 && (! MfcPrm[i].bSetCompt))
					{
						nChannel = MFC01Flw + MfcPrm[i].nMfcNum - 1;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
						if(dbDiff > dbTol)
						{
							MfcPrm[i].bErr = TRUE;
							nMfcCheck++;
						}
						else
						{
							MfcPrm[i].bErr = FALSE;
							MfcPrm[i].bSetCompt = TRUE;
						}
					}
				}
				if(nMfcCheck == 0) break;
				nTmCount++;
			}
			if(msRet != SYS_SUCCESS) break;

			int nAlarm;
			for(i=0; i < nMfcCount; i++)
			{
				if(MfcPrm[i].bErr == TRUE)
				{
					nAlarm = ALARM_MFC01_SET_TIMEOUT + MfcPrm[i].nMfcNum-1;
					if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
					// msRet = SYS_ABORTED;
					// break;
				}
			}
			if(msRet != SYS_SUCCESS) break;

			// Monitoring
			while(1)
			{
				if(nMntrErrCnt < 0) nMntrErrCnt = 3;
				for(i=0; i < nMfcCount; i++)
				{
					if(TRUE == MfcPrm[i].bMon && MfcPrm[i].dbSetVal > 0)
					{
						nErrorCount = 0;
						do {
							nChannel = PRMA_EC_MFC01 + MfcPrm[i].nMfcNum - 1;
							nMntrErrCnt = (int) READ_ANALOG(nChannel, &nIOStatus);
							if(nMntrErrCnt <= 0) break;

							nChannel = MFC01Flw + MfcPrm[i].nMfcNum - 1;
							dbValue = READ_ANALOG(nChannel, &nIOStatus);
							dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
							dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
							if(dbDiff <= dbTol) break;
							if(++nErrorCount >= nMntrErrCnt) break;
							if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
						} while(1);
						if(msRet != SYS_SUCCESS) break;
						if(nErrorCount > nMntrErrCnt)
						{
							nAlarm = ALARM_MFC01_FLOW_OUT_OF_TOL + MfcPrm[i].nMfcNum-1;
							if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
							// msRet = SYS_ABORTED;
							// break;
						}
					}
				}
				if(msRet != SYS_SUCCESS) break;
				if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
			}
			_LOG("-----> MFC Monitoring Finished");
		}
	} while(0);
	if(msRet == SYS_SUCCESS) _LOG("---> MFC Set Function Leaved(Success)");
	else  _LOG("---> MFC Set Function Leaved(Fail)");
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status SetValueP(char *pParam)
{
	char pszLocFtnName[] = "SetValueP";
	Module_Status msRet = SYS_SUCCESS;
	int nChannel;
	int nIOStatus;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	int nTimeoutT = 0;
	double dbDiff, dbTol;
	double dbValue;
	BOOL bMon = FALSE;
	int nErrorCount = 0;
	int nTmCount = 0;
	int nMfcCheck;
	int nTemp;
	int nIdx;

	//... IO Shape Value
	int		nType	= 0;	// 0:Unknown, 1:Input_Only, 2:InOut Virtual, 3: InOut Local, 4:InOut Remote
	int		nInDev	= 0;
	int		nOutDev = 0;
	double	dbMin	= 0;
	double	dbMax	= 0;

	_LOG("---> MFC Set Function Enterd(%s)", pParam);
	do {
		// Read Param
		// printf("Call CTextParser tp( %s )\n", pParam);
		CTextParser tp(pParam);
		for(i=0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0))
			{
				// End of MFC Parameter
				break;
			}
			if(! tp.GetDbFloat(& MfcPrm[i].dbSetVal, 0))
			{
				printf("-----> MFC[%d] 'Parsing SetVal' failed in %s\n", i+1, pszLocFtnName);
				break;
			}
			if(! tp.GetBOOL(& MfcPrm[i].bMon, 0))
			{
				printf("-----> MFC[%d] 'Parsing bMon' failed in %s\n", i+1, pszLocFtnName);
				break;
			}
			nIdx = MfcPrm[i].nMfcNum -1;
			nChannel = PRMA_TO_MFC01 + nIdx;
			nTemp = (int) READ_ANALOG(nChannel, &nIOStatus);
			if(nTimeoutT < nTemp) nTimeoutT = nTemp;

			MfcPrm[i].dbTol = READ_ANALOG(PRMA_TL_MFC01Set+nIdx, &nIOStatus);
			MfcPrm[i].nSetTimeout = (int) READ_ANALOG(PRMA_TO_MFC01+nIdx, &nIOStatus);
			MfcPrm[i].nMonErrCnt = (int) READ_ANALOG(PRMA_EC_MFC01+nIdx, &nIOStatus);

			#ifdef _DEBUG
			printf("MFC[%d]  Sp:%8.2f  bMon:%d  Tol:%5.1f\n", MfcPrm[i].nMfcNum, MfcPrm[i].dbSetVal, MfcPrm[i].bMon, MfcPrm[i].dbTol);
			#endif
			if(MfcPrm[i].bMon) bMon = TRUE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}
		if(i <= 0)
		{
			printf("-----> 'Parsing MFC Paramter' failed in %s\n", pszLocFtnName);
			msRet = SYS_ERROR;
			break;
		}
		nMfcCount = i;

		//////////////////////////////////////////////////////////////////////////
		//... Set MFC Flow
		_LOG("-----> MFC Set Started");
		for(i=0; i<nMfcCount; i++)
		{
			if(MfcPrm[i].dbSetVal >= 0)
			{
				nChannel = MFC01StPt + MfcPrm[i].nMfcNum - 1;
				dbValue = MfcPrm[i].dbSetVal;
				WRITE_ANALOG(nChannel, dbValue, &nIOStatus);
			}
			else if(MfcPrm[i].dbSetVal == -1)
			{	//... Set Max Flow
				nChannel = MFC01StPt + MfcPrm[i].nMfcNum - 1;

				//... Read Max Set Flow Value
				ANALOG_SHAPE(nChannel, &nType, &nInDev, &nOutDev, &dbMin, &dbMax);
				WRITE_ANALOG(nChannel, dbMax, &nIOStatus);	//... Set Max Flow
			}
		}
		_LOG("-----> MFC Set Finished");

		if(bMon)
		{
			// Timeout Check
			_LOG("-----> MFC Monitoring Started");
			nTmCount = 0;
			if(nTimeoutT > 0) while(nTmCount < nTimeoutT)
			{
				if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
				nMfcCheck = 0;
				for(i=0; i < nMfcCount; i++)
				{
					if(TRUE == MfcPrm[i].bMon && MfcPrm[i].dbSetVal > 0 && (! MfcPrm[i].bSetCompt))
					{
						nChannel = MFC01Flw + MfcPrm[i].nMfcNum - 1;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
						if(dbDiff > dbTol)
						{
							MfcPrm[i].bErr = TRUE;
							nMfcCheck++;
						}
						else
						{
							MfcPrm[i].bErr = FALSE;
							MfcPrm[i].bSetCompt = TRUE;
						}
					}
				}
				if(nMfcCheck == 0) break;
				nTmCount++;
			}
			if(msRet != SYS_SUCCESS) break;

			int nAlarm;
			for(i=0; i < nMfcCount; i++)
			{
				if(MfcPrm[i].bErr == TRUE)
				{
					nAlarm = ALARM_MFC01_SET_TIMEOUT + MfcPrm[i].nMfcNum-1;
					if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
					// msRet = SYS_ABORTED;
					// break;
				}
			}
			if(msRet != SYS_SUCCESS) break;

			// Monitoring
			while(1)
			{
				for(i=0; i < nMfcCount; i++)
				{
					if(TRUE == MfcPrm[i].bMon && MfcPrm[i].dbSetVal > 0)
					{
						nErrorCount = 0;
						do {
							if(MfcPrm[i].nMonErrCnt <= 0) break;
							nIdx = MfcPrm[i].nMfcNum - 1;
							nChannel = MFC01Flw + nIdx;
							dbValue = READ_ANALOG(nChannel, &nIOStatus);
							dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
							dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
							if(dbDiff <= dbTol) break;
							nErrorCount++;
							if(nErrorCount > MfcPrm[i].nMonErrCnt) break;
							if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
						} while(1);
						if(msRet != SYS_SUCCESS) break;
						if(nErrorCount > MfcPrm[i].nMonErrCnt)
						{
							nAlarm = ALARM_MFC01_FLOW_OUT_OF_TOL + MfcPrm[i].nMfcNum-1;
							if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
							// msRet = SYS_ABORTED;
							// break;
						}
					}
				}
				if(msRet != SYS_SUCCESS) break;
				if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
			}
			_LOG("-----> MFC Monitoring Finished");
		}
	} while(0);
	if(msRet == SYS_SUCCESS) _LOG("---> MFC Set Function Leaved(Success)");
	else  _LOG("---> MFC Set Function Leaved(Fail)");
	return msRet;
}

Module_Status StartMonitor(char *pParam)
{
	Module_Status msRet = SYS_ERROR;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	double dbDiff, dbTol;
	int nChannel;
	double dbValue;
	int nIOStatus;
	int nErrorCount = 0;
	int nIdx;

	do {
		// Read Param
		CTextParser tp(pParam);
		for(i=0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0)) break;
			if(! tp.GetDbFloat(& MfcPrm[i].dbTol, 0)) break;
			nIdx = MfcPrm[i].nMfcNum - 1;
			nChannel = MFC01StPt + nIdx;
			MfcPrm[i].dbSetVal = READ_ANALOG(nChannel, &nIOStatus);
			MfcPrm[i].nSetTimeout = (int) READ_ANALOG(PRMA_TO_MFC01+nIdx, &nIOStatus);
			MfcPrm[i].nMonErrCnt = (int) READ_ANALOG(PRMA_EC_MFC01+nIdx, &nIOStatus);
			MfcPrm[i].bMon = TRUE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}
		if(0 == i) break;
		nMfcCount = i;
		msRet = SYS_SUCCESS;

		// Start Monitoring
		int nAlarm;
		while(1)
		{
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon)
				{
					nErrorCount = 0;
					do {
						if(MfcPrm[i].nMonErrCnt <= 0) break;
						nChannel = MFC01Flw + MfcPrm[i].nMfcNum - 1;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
						if(dbDiff <= dbTol) break;
						nErrorCount++;
						if(nErrorCount > MfcPrm[i].nMonErrCnt) break;
						if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
					} while(1);
					if(msRet != SYS_SUCCESS) break;
					if(nErrorCount > MfcPrm[i].nMonErrCnt)
					{
						nAlarm = ALARM_MFC01_FLOW_OUT_OF_TOL + MfcPrm[i].nMfcNum-1;
						if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
						// msRet = SYS_ABORTED;
						// break;
					}
				}
			}
			if(msRet != SYS_SUCCESS) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
		}
	} while(0);
	return msRet;
}

Module_Status StartMonitorP(char *pParam)
{
	Module_Status msRet = SYS_ERROR;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	double dbDiff, dbTol;
	int nChannel;
	double dbValue;
	int nIOStatus;
	int nErrorCount = 0;
	int nIdx;

	do {
		// Read Param
		CTextParser tp(pParam);
		for(i=0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0)) break;
			nIdx = MfcPrm[i].nMfcNum - 1;
			MfcPrm[i].dbTol = READ_ANALOG(PRMA_TL_MFC01Set+nIdx, &nIOStatus);
			MfcPrm[i].nSetTimeout = (int) READ_ANALOG(PRMA_TO_MFC01+nIdx, &nIOStatus);
			MfcPrm[i].nMonErrCnt = (int) READ_ANALOG(PRMA_EC_MFC01+nIdx, &nIOStatus);
			if(MfcPrm[i].dbTol <= 0) MfcPrm[i].dbTol = 30;
			nChannel = MFC01StPt + nIdx;
			MfcPrm[i].dbSetVal = READ_ANALOG(nChannel, &nIOStatus);
			MfcPrm[i].bMon = TRUE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}
		if(0 == i) break;
		nMfcCount = i;
		msRet = SYS_SUCCESS;

		// Start Monitoring
		int nAlarm;
		while(1)
		{
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon)
				{
					nErrorCount = 0;
					do {
						if(MfcPrm[i].nMonErrCnt <= 0) break;
						nIdx = MfcPrm[i].nMfcNum - 1;
						nChannel = MFC01Flw + nIdx;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
						if(dbDiff <= dbTol) break;
						nErrorCount++;
						if(nErrorCount > MfcPrm[i].nMonErrCnt) break;
						if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
					} while(1);
					if(msRet != SYS_SUCCESS) break;
					if(nErrorCount > MfcPrm[i].nMonErrCnt)
					{
						nAlarm = ALARM_MFC01_FLOW_OUT_OF_TOL + nIdx;
						if(ALM_PAUSED != ALARM_STATUS(nAlarm)) ALARM_POST(nAlarm);
						// msRet = SYS_ABORTED;
						// break;
					}
				}
			}
			if(msRet != SYS_SUCCESS) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
		}
	} while(0);
	return msRet;
}

Module_Status StartMonitorP_O(char *pParam)
{
	Module_Status msRet = SYS_SUCCESS;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	double dbDiff, dbTol;
	int nChannel;
	double dbValue;
	int nIOStatus;
	int nMntrErrCnt = 3;
	int nErrorCount = 0;
	int nIdx;
	int nMfcErrCnt;
	int nTimeCnt;

	do {
		// Read Param
		CTextParser tp(pParam);
		for(i=0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0)) break;
			if(! tp.GetInt(& MfcPrm[i].nOption, 0)) break;
			nIdx = MfcPrm[i].nMfcNum - 1;
			MfcPrm[i].dbTol = READ_ANALOG(PRMA_TL_MFC01Set+nIdx, &nIOStatus);
			MfcPrm[i].nSetTimeout = (int) READ_ANALOG(PRMA_TO_MFC01+nIdx, &nIOStatus);
			MfcPrm[i].nMonErrCnt = (int) READ_ANALOG(PRMA_EC_MFC01+nIdx, &nIOStatus);
			if(MfcPrm[i].dbTol <= 0) MfcPrm[i].dbTol = 30;
			nChannel = MFC01StPt + nIdx;
			MfcPrm[i].dbSetVal = READ_ANALOG(nChannel, &nIOStatus);
			if(MfcPrm[i].nOption == AL_WARNING || MfcPrm[i].nOption == AL_CHMDIS	// 2006.07.04
				|| MfcPrm[i].nOption == AL_ABORT || MfcPrm[i].nOption == AL_RETRY) MfcPrm[i].bMon = TRUE;
			else MfcPrm[i].bMon = FALSE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}
		if(0 == i) break;
		nMfcCount = i;
		msRet = SYS_SUCCESS;

		// Set Time Monitoring
		nTimeCnt = 0;
		do {
			nMfcErrCnt = 0;
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon && (! MfcPrm[i].bSetCompt))
				{
					if(MfcPrm[i].nSetTimeout <= 0) continue;
					nIdx = MfcPrm[i].nMfcNum - 1;
					nChannel = MFC01Flw + nIdx;
					dbValue = READ_ANALOG(nChannel, &nIOStatus);
					dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
					dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
					if(dbDiff > dbTol)
					{
						nMfcErrCnt++;
						if(nTimeCnt >= MfcPrm[i].nSetTimeout)			// 2010.08.24
						{
							if(FALSE == CheckAlarmOption(ALARM_MFC01_SET_TIMEOUT + nIdx, MfcPrm[i].nOption))
							{
								_LOG("-----> Set Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
									MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
								msRet = SYS_ABORTED;
								break;
							}
						}
					}
					else MfcPrm[i].bSetCompt = TRUE;
				}
			}
			if(nMfcErrCnt <= 0) break;
			if(msRet == SYS_ABORTED) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
			nTimeCnt ++;
		} while(nMfcErrCnt > 0);
		if(msRet == SYS_ABORTED) break;

		// Start Monitoring
		while(1)
		{
			if(nMntrErrCnt <= 0) nMntrErrCnt = 3;
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon)
				{
					nErrorCount = 0;
					do {
						if(MfcPrm[i].nMonErrCnt <= 0) break;
						nIdx = MfcPrm[i].nMfcNum - 1;
						nChannel = MFC01Flw + nIdx;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
						if(dbDiff <= dbTol) break;
						nErrorCount++;
						if(nErrorCount > nMntrErrCnt) break;
						if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
					} while(1);
					if(msRet != SYS_SUCCESS) break;
					if(nErrorCount > nMntrErrCnt)
					{
						if(FALSE == CheckAlarmOption(ALARM_MFC01_FLOW_OUT_OF_TOL + nIdx, MfcPrm[i].nOption))
						{
							_LOG("-----> Mon Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
								MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
							msRet = SYS_ABORTED;
							break;
						}
					}
				}
			}
			if(msRet != SYS_SUCCESS) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
		}
	} while(0);
	return msRet;
}

Module_Status StartMonitorP_O2(char *pParam)
{
	Module_Status msRet = SYS_ERROR;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	double dbDiff, dbTol;
	int nChannel;
	double dbValue;
	int nIOStatus;
	int nTimeCnt = 3;
	int nErrorCount = 0;
	int nIdx;
	int nMfcErrCnt = 0;

	do {
		// Read Param
		CTextParser tp(pParam);
		for(i=0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0)) break;
			if(! tp.GetDbFloat(& MfcPrm[i].dbSetVal, 0)) break;
			if(! tp.GetInt(& MfcPrm[i].nOption, 0)) break;
			nIdx = MfcPrm[i].nMfcNum - 1;
			MfcPrm[i].dbTol = READ_ANALOG(PRMA_TL_MFC01Set+nIdx, &nIOStatus);
			MfcPrm[i].nSetTimeout = (int) READ_ANALOG(PRMA_TO_MFC01+nIdx, &nIOStatus);
			MfcPrm[i].nMonErrCnt = (int) READ_ANALOG(PRMA_EC_MFC01+nIdx, &nIOStatus);
			if(MfcPrm[i].dbTol <= 0) MfcPrm[i].dbTol = 30;
			nChannel = MFC01StPt + nIdx;
			if(MfcPrm[i].nOption == AL_WARNING || MfcPrm[i].nOption == AL_CHMDIS		// 2006.07.04
				|| MfcPrm[i].nOption == AL_ABORT || MfcPrm[i].nOption == AL_RETRY) MfcPrm[i].bMon = TRUE;
			else MfcPrm[i].bMon = FALSE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}
		if(0 == i) break;
		nMfcCount = i;
		msRet = SYS_SUCCESS;

		// Set Time Monitoring
		nTimeCnt = 0;
		do {
			nMfcErrCnt = 0;
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon && (! MfcPrm[i].bSetCompt))
				{
					if(MfcPrm[i].nSetTimeout <= 0) continue;
					nIdx = MfcPrm[i].nMfcNum - 1;
					nChannel = MFC01Flw + nIdx;
					dbValue = READ_ANALOG(nChannel, &nIOStatus);
					dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
					dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
					if(dbDiff > dbTol)
					{
						nMfcErrCnt++;
						if(nTimeCnt >= MfcPrm[i].nSetTimeout)			// 2010.08.24
						{
							if(FALSE == CheckAlarmOption(ALARM_MFC01_SET_TIMEOUT + nIdx, MfcPrm[i].nOption))
							{
								_LOG("-----> Set Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
									MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
								msRet = SYS_ABORTED;
								break;
							}
						}
					}
					else MfcPrm[i].bSetCompt = TRUE;
				}
			}
			if(nMfcErrCnt <= 0) break;
			if(msRet == SYS_ABORTED) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
			nTimeCnt ++;
		} while(nMfcErrCnt > 0);
		if(msRet == SYS_ABORTED) break;

		// Start Monitoring
		while(1)
		{
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon)
				{
					nErrorCount = 0;
					do {
						if(MfcPrm[i].nMonErrCnt <= 0) break;
						nIdx = MfcPrm[i].nMfcNum - 1;
						nChannel = MFC01Flw + nIdx;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal -dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
						if(dbDiff <= dbTol) break;
						nErrorCount ++;
						if(nErrorCount > MfcPrm[i].nMonErrCnt) break;
						if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
					} while(1);
					if(msRet == SYS_ABORTED) break;
					if(nErrorCount > MfcPrm[i].nMonErrCnt)
					{
						if(FALSE == CheckAlarmOption(ALARM_MFC01_FLOW_OUT_OF_TOL + nIdx, MfcPrm[i].nOption))
						{
							_LOG("-----> Mon Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
								MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
							msRet = SYS_ABORTED;
							break;
						}
					}
				}
			}
			if(msRet != SYS_SUCCESS) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
		}
	} while(0);
	return msRet;
}

//... 2018.06.02
//... Add Source feed Line Pressure gauge Sensor Check Interlock
BOOL IsSourceFeedLineFlowOk(int nIntOption)
{
	static int nErrCnt = 0;
	int nVlvIndex = 0;
	int nSrcVlvOpen = 0;
	int nDIOIndex = 0;
	int nIOStatus;
	char szAlarm[256] = {0,};
	char szDesMsg[512] = {0,};
	double dbSrcPrsValue = 0;
	enum {ALL_VLV_CL, ONE_VLV_OP_CL , ALL_VLV_OP};
	//printf("[debug default Check]===> count %d \n",nErrCnt);
	//... Pre Heat/flow Step Mode Check (Source Feed Line Valve Close ?)
	for(nVlvIndex = 0; nVlvIndex < MAX_CHK_VALVE; nVlvIndex++)
	{
		nDIOIndex = _FIND_FROM_STRING(_K_D_IO,g_szValveIO[nVlvIndex]);
		nSrcVlvOpen += _dREAD_DIGITAL(nDIOIndex,&nIOStatus);	
	}
	
	dbSrcPrsValue = READ_ANALOG(PrsGauge1AI,&nIOStatus);
	
	if(nSrcVlvOpen == ONE_VLV_OP_CL) return TRUE;
	else
	{
		if(g_dbParmLow >= dbSrcPrsValue || g_dbParmHigh <= dbSrcPrsValue) nErrCnt++;
		else nErrCnt = 0;
	}
	//printf("[debug]===> Low Limit%0.3f  High Limit %0.3f  cur %0.3f count %d \n",g_dbParmLow,g_dbParmHigh,dbSrcPrsValue,nErrCnt);
	if(nErrCnt > (int)g_dbParmChkTO)
	{
		ALARM_MESSAGE_GET(ALARM_SOURCE_PRESSURE_ERROR,szAlarm);
		sprintf(szDesMsg,"%s[Cur Prs: %0.3f HighLimit: %0.3f LowLimit: %0.3f]",szAlarm,dbSrcPrsValue,g_dbParmHigh,g_dbParmLow);
		ALARM_MESSAGE_SET(ALARM_SOURCE_PRESSURE_ERROR,szDesMsg);
		if(FALSE == CheckAlarmOption(ALARM_SOURCE_PRESSURE_ERROR, nIntOption))
		{
			nErrCnt = 0;
			return FALSE;
		}
		nErrCnt = 0;
	}
	else return TRUE;
	
	return TRUE;
}

// 2006.09.05
Module_Status StartMonitorP_O3(char *pParam)
{
	Module_Status msRet = SYS_ERROR;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	double dbDiff, dbTol;
	int nChannel;
	double dbValue;
	int nIOStatus;
	int nTimeCnt = 3;
	int nErrorCount = 0;
	int nIdx;
	int nMfcErrCnt = 0;
	int nSourcePrsOption = 0;
	int nCanisterVlv = 0;
	int nCarrierMFC = 0;
	BOOL bIntRcp = FALSE;

	nSourcePrsOption = READ_DIGITAL(PRMD_SOUREPRS_OPTION,&nIOStatus); 
	if(0 < nSourcePrsOption) 
	{
	
		g_dbParmLow   = READ_ANALOG(PRMA_SRCPRESSURE_LOW,&nIOStatus);
		if(g_dbParmLow <= 0) g_dbParmLow = 30; //... default 30 torr
		g_dbParmHigh  = READ_ANALOG(PRMA_SRCPRESSURE_HIGH,&nIOStatus);
		if(g_dbParmLow <= 0) g_dbParmLow = 70; //... default 70 torr
		g_dbParmChkTO = READ_ANALOG(PRMA_SRCPRESSURE_TIMEOUT,&nIOStatus);
		if(g_dbParmLow <= 0) g_dbParmChkTO = 1; //... default 1 sec
		
		nCarrierMFC = READ_DIGITAL(PRMD_CARRIER_MFC,&nIOStatus);
		if(nCarrierMFC <= 0) nCarrierMFC = 4;//... default MFC4
		
		for(i = 0; i < MAX_CHK_VALVE; i++)
		{
			nCanisterVlv = READ_DIGITAL(PRMD_CANISTER_VALVE1+i,&nIOStatus);
			if(nCanisterVlv <= 0)
			{
				if(i ==0 )nCanisterVlv = 13; //.. default V13
				if(i ==1 )nCanisterVlv = 15; //.. default V15
			}
			sprintf(g_szValveIO[i],"V%d",READ_DIGITAL(PRMD_CANISTER_VALVE1+i,&nIOStatus));
			strcat(g_szValveIO[i],"ValveDO");
			printf("[debug] Valve name==> %s \n",g_szValveIO[i]);
		}
	}

	do {
		// Read Param
		CTextParser tp(pParam);
		for(i=0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0)) break;
			if(! tp.GetDbFloat(& MfcPrm[i].dbSetVal, 0)) break;
			if(! tp.GetDbFloat(& MfcPrm[i].dbTol, 0)) break;
			if(! tp.GetInt(& MfcPrm[i].nOption, 0)) break;
			nIdx = MfcPrm[i].nMfcNum - 1;

			if(MfcPrm[i].dbTol <= 0) MfcPrm[i].dbTol = READ_ANALOG(PRMA_TL_MFC01Set+nIdx, &nIOStatus);
			else if(MfcPrm[i].dbSetVal > 0) MfcPrm[i].dbTol = MfcPrm[i].dbTol/MfcPrm[i].dbSetVal*100;

			#ifdef _DEBUG
			printf("-----> MFC Func Param : %d %.1f %.1f %d\n", MfcPrm[i].nMfcNum, MfcPrm[i].dbSetVal,
									MfcPrm[i].dbTol, MfcPrm[i].nOption);
			#endif

			MfcPrm[i].nSetTimeout = (int) READ_ANALOG(PRMA_TO_MFC01+nIdx, &nIOStatus);
			MfcPrm[i].nMonErrCnt = (int) READ_ANALOG(PRMA_EC_MFC01+nIdx, &nIOStatus);
			if(MfcPrm[i].dbTol <= 0) MfcPrm[i].dbTol = 30;
			nChannel = MFC01StPt + nIdx;
			if(MfcPrm[i].nOption == AL_WARNING || MfcPrm[i].nOption == AL_CHMDIS		// 2006.07.04
				|| MfcPrm[i].nOption == AL_ABORT || MfcPrm[i].nOption == AL_RETRY) MfcPrm[i].bMon = TRUE;
			else MfcPrm[i].bMon = FALSE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}
		if(0 == i) break;
		nMfcCount = i;
		msRet = SYS_SUCCESS;

		// Set Time Monitoring
		nTimeCnt = 0;
		do {
			nMfcErrCnt = 0;
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon && (! MfcPrm[i].bSetCompt))
				{
					if(MfcPrm[i].nSetTimeout <= 0) continue;
					nIdx = MfcPrm[i].nMfcNum - 1;
					nChannel = MFC01Flw + nIdx;
					dbValue = READ_ANALOG(nChannel, &nIOStatus);
					dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
					dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
					if(dbDiff > dbTol)
					{
						nMfcErrCnt++;
						if(nTimeCnt >= MfcPrm[i].nSetTimeout)			// 2010.08.24
						{
							if(FALSE == CheckAlarmOption(ALARM_MFC01_SET_TIMEOUT + nIdx, MfcPrm[i].nOption))
							{
								_LOG("-----> Set Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
									MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
								msRet = SYS_ABORTED;
								break;
							}
						}
					}
					else MfcPrm[i].bSetCompt =TRUE;
				}
			}
			if(nMfcErrCnt <= 0) break;
			if(msRet == SYS_ABORTED) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
			nTimeCnt ++;
		} while(nMfcErrCnt > 0);
		if(msRet == SYS_ABORTED) break;

		// Start Monitoring
		while(1)
		{
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon)
				{
					nErrorCount = 0;
					do {
						if(MfcPrm[i].nMonErrCnt <= 0) break;
						nIdx = MfcPrm[i].nMfcNum - 1;
						nChannel = MFC01Flw + nIdx;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal -dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;

						//... 2018.06.02
						//... Add Source feed Line Pressure gauge Sensor Check Interlock	
						//printf("[debug] ==> Int Op:%d  PRM:%d MFC No:%d \n",nSourcePrsOption,MfcPrm[i].nMfcNum,nCarrierMFC);
						if(nSourcePrsOption > 0)
						{
							if(ONLY_PRC_0 == READ_DIGITAL(PRMD_INT_RCP,&nIOStatus)
								&& Proc_1 == READ_DIGITAL(PRO_RcpTypeDM,&nIOStatus))
							{
								bIntRcp = TRUE;
							}
							else if(ONLY_PRC_0 != READ_DIGITAL(PRMD_INT_RCP,&nIOStatus))
							{
								bIntRcp = TRUE;
							}
							else 
							{
								bIntRcp = FALSE;
							}

							if(nCarrierMFC == MfcPrm[i].nMfcNum && bIntRcp == TRUE)
							{
								if(!IsSourceFeedLineFlowOk(nSourcePrsOption))
								{
									msRet = SYS_ABORTED;
									break;
								}
							}
						}

						if(dbDiff <= dbTol) break;
						nErrorCount ++;
						if(nErrorCount > MfcPrm[i].nMonErrCnt) break;
						if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
					} while(1);
					if(msRet == SYS_ABORTED) break;
					if(nErrorCount > MfcPrm[i].nMonErrCnt)
					{
						if(FALSE == CheckAlarmOption(ALARM_MFC01_FLOW_OUT_OF_TOL + nIdx, MfcPrm[i].nOption))
						{
							_LOG("-----> Mon Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
								MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
							msRet = SYS_ABORTED;
							break;
						}
					}
				}
			}
			if(msRet != SYS_SUCCESS) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
		}
	} while(0);
	return msRet;
}

// 2007.10.24
Module_Status StartMonitorP_O4(char *pParam)
{
	Module_Status msRet = SYS_ERROR;
	MFC_PARAM_STRUCT MfcPrm[MAX_MFC];
	int i;
	int nMfcCount;
	double dbDiff, dbTol;
	int nChannel;
	double dbValue;
	int nIOStatus;
	int nTimeCnt = 3;
	int nErrorCount = 0;
	int nIdx;
	int nMfcErrCnt = 0;

	do {
		// Read Param
		CTextParser tp(pParam);
		for(i=0; i < MAX_MFC; i++)
		{
			if(! tp.GetInt(& MfcPrm[i].nMfcNum, 0)) break;
			if(! tp.GetDbFloat(& MfcPrm[i].dbSetVal, 0)) break;
			if(! tp.GetDbFloat(& MfcPrm[i].dbTol, 0)) break;
			if(! tp.GetInt(& MfcPrm[i].nOption, 0)) break;

			if(! tp.GetInt(& MfcPrm[i].nSetTimeout, 0)) break;
			if(! tp.GetInt(& MfcPrm[i].nMonErrCnt, 0)) break;
			
			nIdx = MfcPrm[i].nMfcNum - 1;

			if(MfcPrm[i].dbTol <= 0)
			{
				MfcPrm[i].dbTol = READ_ANALOG(PRMA_TL_MFC01Set+nIdx, &nIOStatus);
				MfcPrm[i].nSetTimeout = (int) READ_ANALOG(PRMA_TO_MFC01+nIdx, &nIOStatus);
				MfcPrm[i].nMonErrCnt = (int) READ_ANALOG(PRMA_EC_MFC01+nIdx, &nIOStatus);
			}
			else
			{
				if(MfcPrm[i].dbSetVal > 0)
				{
					MfcPrm[i].dbTol = MfcPrm[i].dbTol/MfcPrm[i].dbSetVal*100;
				}
			}

			#ifdef _DEBUG
			printf("-----> MFC Func Param : %d %.1f %.1f %d\n", MfcPrm[i].nMfcNum, MfcPrm[i].dbSetVal,
									MfcPrm[i].dbTol, MfcPrm[i].nOption);
			#endif

			if(MfcPrm[i].dbTol <= 0) MfcPrm[i].dbTol = 30;
			nChannel = MFC01StPt + nIdx;
			if(MfcPrm[i].nOption == AL_WARNING || MfcPrm[i].nOption == AL_CHMDIS		// 2006.07.04
				|| MfcPrm[i].nOption == AL_ABORT || MfcPrm[i].nOption == AL_RETRY) MfcPrm[i].bMon = TRUE;
			else MfcPrm[i].bMon = FALSE;
			MfcPrm[i].bErr = FALSE;
			MfcPrm[i].bSetCompt = FALSE;
		}
		if(0 == i) break;
		nMfcCount = i;
		msRet = SYS_SUCCESS;

		// Set Time Monitoring
		nTimeCnt = 0;
		do {
			nMfcErrCnt = 0;
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon && (! MfcPrm[i].bSetCompt))
				{
					// if(MfcPrm[i].nSetTimeout <= 0) continue;
					nIdx = MfcPrm[i].nMfcNum - 1;
					nChannel = MFC01Flw + nIdx;
					dbValue = READ_ANALOG(nChannel, &nIOStatus);
					dbDiff = fabs(MfcPrm[i].dbSetVal - dbValue);
					dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
					if(dbDiff > dbTol)
					{
						nMfcErrCnt++;
						if(nTimeCnt >= MfcPrm[i].nSetTimeout)			// 2010.08.24
						{
							if(FALSE == CheckAlarmOption(ALARM_MFC01_SET_TIMEOUT + nIdx, MfcPrm[i].nOption))
							{
								_LOG("-----> Set Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
									MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
								msRet = SYS_ABORTED;
								break;
							}
						}
					}
					else MfcPrm[i].bSetCompt =TRUE;
				}
			}
			if(nMfcErrCnt <= 0) break;
			if(msRet == SYS_ABORTED) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
			nTimeCnt ++;
		} while(nMfcErrCnt > 0);
		if(msRet == SYS_ABORTED) break;

		// Start Monitoring
		while(1)
		{
			for(i=0; i < nMfcCount; i++)
			{
				if(TRUE == MfcPrm[i].bMon)
				{
					nErrorCount = 0;
					do {
						// if(MfcPrm[i].nMonErrCnt <= 0) break;
						nIdx = MfcPrm[i].nMfcNum - 1;
						nChannel = MFC01Flw + nIdx;
						dbValue = READ_ANALOG(nChannel, &nIOStatus);
						dbDiff = fabs(MfcPrm[i].dbSetVal -dbValue);
						dbTol = MfcPrm[i].dbSetVal * MfcPrm[i].dbTol / 100;
						if(dbDiff <= dbTol) break;
						nErrorCount ++;
						if(nErrorCount > MfcPrm[i].nMonErrCnt) break;
						if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
					} while(1);
					if(msRet == SYS_ABORTED) break;
					if(nErrorCount > MfcPrm[i].nMonErrCnt)
					{
						if(FALSE == CheckAlarmOption(ALARM_MFC01_FLOW_OUT_OF_TOL + nIdx, MfcPrm[i].nOption))
						{
							_LOG("-----> Mon Timeout MFC[%d] Set:%.1f, Read:%.1f, Diff:%.1f, Tol:%.1f", i+1,
								MfcPrm[i].dbSetVal, dbValue, dbDiff, dbTol);
							msRet = SYS_ABORTED;
							break;
						}
					}
				}
			}
			if(msRet != SYS_SUCCESS) break;
			if(! WAIT_SECONDS(1)) {	msRet = SYS_ABORTED; break;	}
		}
	} while(0);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CloseAll(char *pParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nChannel;
	int nIOStatus;
	int i;

	for(i=0; i<MAX_MFC; i++)
	{
		if(g_abCloseAllList[i])
		{
			nChannel = MFC01StPt + i;
			WRITE_ANALOG(nChannel, 0, &nIOStatus);
		}
	}
	return msRet;
}

void ParseMFCCloseAllList(char *szParam)
{
	char szMFC[8];
	int nIdx;
	CTextParser tp(szParam);

	if(! tp.GetWord(szMFC)) return;
	nIdx = szMFC[3] - '0' - 1;
	tp.GetBOOL(&g_abCloseAllList[nIdx]);
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
		// printf("-----> Read PMC Configuration Data\n");

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("MFC Close All List", szItem) == 0)			nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseMFCCloseAllList(szRead); break;
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
		printf("-----> Config File Reading Failed : %s", PMC_CONFIG_FILE);
	}
	return bRet;
}

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int nParLen = 0;
	char *pParam;
	char szCmd[41], szSubParam[1024];		// 2010.04.07

	printf("Entered %s\n", APP_NAME);

	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if ( nParLen < 1 )	{
			printf( "---> Parameter Error, Len = %d in %s !\n", nParLen, APP_NAME);
			msRet = SYS_ERROR;
			break;
		}

		STR_SEPERATE(pParam , szCmd, szSubParam, 399 );

		//.....
		if     (STRCMP_L(szCmd, "SET_VALUE")) 		msRet = SetValue(szSubParam);
		else if(STRCMP_L(szCmd, "SET_VALUE_P")) 	msRet = SetValueP(szSubParam);
		else if(STRCMP_L(szCmd, "START_MON")) 		msRet = StartMonitor(szSubParam);
		else if(STRCMP_L(szCmd, "START_MON_P")) 	msRet = StartMonitorP(szSubParam);
		else if(STRCMP_L(szCmd, "START_MON_P_O"))	msRet = StartMonitorP_O(szSubParam);
		else if(STRCMP_L(szCmd, "START_MON_P_O2")) 	msRet = StartMonitorP_O2(szSubParam);
		else if(STRCMP_L(szCmd, "START_MON_P_O3")) 	msRet = StartMonitorP_O3(szSubParam);		// 2006.09.05
		else if(STRCMP_L(szCmd, "START_MON_P_O4")) 	msRet = StartMonitorP_O4(szSubParam);		// 2007.10.24
		else if(STRCMP_L(szCmd, "FULL_OPEN")) 		msRet = FullOpen(szSubParam);
		else if(STRCMP_L(szCmd, "CLOSE_ALL")) 		msRet = CloseAll(szSubParam);
		else 
		{
			printf("---> Invalid Command in %s !\n", APP_NAME);
			msRet = SYS_ERROR;
		}
	} while(0);
	
	printf("Leave %s\n", APP_NAME);

	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	ReadPmcConfig();

	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
