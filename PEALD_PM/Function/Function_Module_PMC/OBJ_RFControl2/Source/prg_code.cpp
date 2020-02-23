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
#include "TextLogDll.h"

#define APP_NAME	"RFControl"

//------------------------------------------------------------------------------------------
// Alarm Definition(2600 ~ 2699)

#define ALARM_PLASMA_NOT_DETECTED	2650
#define ALARM_RF_ARC_OCCURED		2651
#define ALARM_RF_SET_TIMEOUT		2652
#define ALARM_RF_POWER_OUT_OF_TOL	2653
#define ALARM_REFLECT_IS_HIGH		2654

//------------------------------------------------------------------------------------------
// Enumeration  Definition

enum {
	RF_OFF, RF_ON
};

enum {
	eOFF_0, eON_1
};

enum {
	RT_Unknown,
	RT_ProcRcp,
	RT_PreProRcp,
	RT_PostProRcp,
	RT_IdleRcp,
	RT_CleanRcp,
	RT_PurgeRcp,
	RT_LotPreRcp,
	RT_LotPostRcp
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "RFPlasmaDetect2DI",	_K_D_IO	,	RFPlasmaDetectDI, 0},
	{ "RFPwOn2DO",			_K_D_IO	,	RFPwOnDO, 0},
	{ "RFSetPoint2AO",		_K_A_IO	,	RFSetPointAO, 0},
	{ "RFLoadPow2AI",		_K_A_IO	,	RFLoadPowAI, 0},
	{ "RFRefPow2AI",		_K_A_IO	,	RFRefPowAI, 0},

	{ "HF_LOAD_PRESET_B",	_K_A_IO	,	MNLoadPresetAO, 0},
	{ "HF_TUNE_PRESET_B",	_K_A_IO	,	MNTunePresetAO, 0},
	{ "HF_Ctrl_Mode_B",	_K_D_IO	,	MNModeSelDO, 0},
	{ "HF_Preset_Mode_B",	_K_D_IO	,	MNPresetModeDO, 0},

	{ "PRMA_EC_RF2PlsmDtc",	_K_A_IO	,	PRMA_EC_RFPlsmDtc, 0},
	{ "PRMA_EC_RF2FwPw",		_K_A_IO	,	PRMA_EC_RFFwPw, 0},
	{ "PRMA_TL_RF2Refl",		_K_A_IO	,	PRMA_TL_RFRefl, 0},
	{ "PRMA_TL_RF2FwPw",		_K_A_IO	,	PRMA_TL_RFFwPw, 0},
	{ "PRMA_Type1L",		_K_A_IO	,	PRMA_Type1L, 0},
	{ "PRMA_Type2L",		_K_A_IO	,	PRMA_Type2L, 0},
	{ "PRMA_Type3L",		_K_A_IO	,	PRMA_Type3L, 0},
	{ "PRMA_Type4L",		_K_A_IO	,	PRMA_Type4L, 0},
	{ "PRMA_Type5L",		_K_A_IO	,	PRMA_Type5L, 0},
	{ "PRMA_Type6L",		_K_A_IO	,	PRMA_Type6L, 0},
	{ "PRMA_Type7L",		_K_A_IO	,	PRMA_Type7L, 0},
	{ "PRMA_Type8L",		_K_A_IO	,	PRMA_Type8L, 0},
	{ "PRMA_Type1T",		_K_A_IO	,	PRMA_Type1T, 0},
	{ "PRMA_Type2T",		_K_A_IO	,	PRMA_Type2T, 0},
	{ "PRMA_Type3T",		_K_A_IO	,	PRMA_Type3T, 0},
	{ "PRMA_Type4T",		_K_A_IO	,	PRMA_Type4T, 0},
	{ "PRMA_Type5T",		_K_A_IO	,	PRMA_Type5T, 0},
	{ "PRMA_Type6T",		_K_A_IO	,	PRMA_Type6T, 0},
	{ "PRMA_Type7T",		_K_A_IO	,	PRMA_Type7T, 0},
	{ "PRMA_Type8T",		_K_A_IO	,	PRMA_Type8T, 0},
	//...2016.10.26
	{ "PRO_RcpTypeDM",		 _K_D_IO,	PRO_RcpTypeDM,		 0},
	{ "PRMD_AL_PresetOffOp", _K_D_IO,	PRMD_AL_PresetOffOp, 0},
	{ "PRMD_RC_SkipRcpType1",_K_D_IO,	PRMD_RC_SkipRcpType1,0},
	{ "PRMD_RC_SkipRcpType2",_K_D_IO,	PRMD_RC_SkipRcpType2,0},
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status SetPower(char *pParam)
{
	char pszLocFtnName[] = "SetPower";
	char pszFtnName[256];
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	int nSetPower;
	int nTol;
	int nTimeout;
	int nTCount = 0;
	int nReadPower;
	int nDiff;
	int nLdPwTol = 10;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		// Read Parameter
		CTextParser tp(pParam);
		if(! tp.GetInt(&nSetPower, 0)) break;
		if(! tp.GetInt(&nTol, 0)) break;
		if(! tp.GetInt(&nTimeout, 0)) break;

		nLdPwTol = nSetPower * nTol / 100;

		// Set Power
		WRITE_ANALOG(RFSetPointAO, nSetPower, &nIOStatus);

		if(nSetPower == 0)
		{
			WRITE_DIGITAL(RFPwOnDO, 0, &nIOStatus);		// 0:Off 1:On
			break;
		}

		// RF Power On
		WRITE_DIGITAL(RFPwOnDO, 1, &nIOStatus);		// 0:Off 1:On

		if(nLdPwTol > 0)
		{
			// Wait until in tolerance
			do {
				nReadPower = (int) READ_ANALOG(RFLoadPowAI, &nIOStatus);
				nDiff = abs(nReadPower - nSetPower);
				if(nDiff <= nLdPwTol) break;
				if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
			} while(++nTCount < nTimeout);
			if(msRet == SYS_ABORTED) break;
			if(nTCount >= nTimeout)
			{
				ALARM_POST(ALARM_RF_SET_TIMEOUT);
				msRet = SYS_ABORTED;
				break;
			}
		}
	} while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

Module_Status SetPwValue(char *pParam)
{
	char pszLocFtnName[] = "SetPwValue";
	char pszFtnName[256];
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	int nSetPower;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		// Read Parameter
		CTextParser tp(pParam);
		if(! tp.GetInt(&nSetPower, 0)) break;

		// Set Power Value
		WRITE_ANALOG(RFSetPointAO, nSetPower, &nIOStatus);
	} while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}
//... 2016.10.26
BOOL IsPrsetOffOk()
{
	BOOL bRet = FALSE;
	int nCtrlOp;
	int nIOStatus;
	int nRunRcpType;
	int nPramRcpeType1;
	int nPramRcpeType2;

	nCtrlOp			= READ_DIGITAL(PRMD_AL_PresetOffOp,&nIOStatus); 
	nRunRcpType		= READ_DIGITAL(PRO_RcpTypeDM,&nIOStatus);
	nPramRcpeType1	= READ_DIGITAL(PRMD_RC_SkipRcpType1,&nIOStatus);
	nPramRcpeType2	= READ_DIGITAL(PRMD_RC_SkipRcpType2,&nIOStatus);
	
	do 
	{
		if(nCtrlOp == eOFF_0) break;

		if(nRunRcpType == nPramRcpeType1
			|| nRunRcpType == nPramRcpeType2) bRet = TRUE;
	} while (0);

	return bRet;
}
// 2014.05.22
Module_Status PresetPos(char *pParam)
{
	char pszLocFtnName[] = "PresetPos";
	char pszFtnName[256];
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	char szItem[80];
	double dbData1, dbData2;
	int nMode,nSel;
	int nPresetType;
	int nType;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		// Read Parameter
		CTextParser tp(pParam);
		if(! tp.GetWord(szItem)) break;
		tp.GetInt(&nMode);
		tp.GetInt(&nSel);
		tp.GetInt(&nPresetType);

		nType = nPresetType - 1;
		dbData1 = READ_ANALOG(PRMA_Type1L + nType,&nIOStatus);
		dbData2 = READ_ANALOG(PRMA_Type1T + nType,&nIOStatus);
		
		if(nMode == 2)
		{
			WRITE_DIGITAL(MNModeSelDO, 1, &nIOStatus);		// 2:Manual 1:Auto
		}else if(nMode == 1)
		{
			WRITE_DIGITAL(MNModeSelDO, 2, &nIOStatus);		// 2:Manual 1:Auto
		}
		
		if(nSel == 1)
		{
			//... 2016.10.26
			if(!IsPrsetOffOk()) {WRITE_DIGITAL(MNPresetModeDO, 0, &nIOStatus);}	// 0:None	1:Preset
		}else if(nSel == 2)
		{
			WRITE_DIGITAL(MNPresetModeDO, 1, &nIOStatus);	// 0:None	1:Preset
		}
		
		if(nPresetType > 0)
		{
			if(strcmp(szItem, "LT") == 0) 
			{
				WRITE_ANALOG(MNLoadPresetAO, dbData1, &nIOStatus); 
				WRITE_ANALOG(MNTunePresetAO, dbData2, &nIOStatus); 
			}
		}

		if(!nIOStatus){
			msRet = SYS_ABORTED;
			break;
		}

	} while(0);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status StartMon(char *pParam)
{
	char pszLocFtnName[] = "StartMon";
	char pszFtnName[256];
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	int nRFPwOnDO = 0;
	int nSetPower = 0;
	int nReadPower = 0;
	int nPRMA_EC_RFFwPw = 0;
	int nPRMA_EC_RFPlsmDtc = 0;
	int nTolErrCount = 0;
	int nPlmErrCount = 0;
	int nRefErrCount = 0;
	int nRFPlasmaDetectDI = 0;
	int nPRMA_TL_RFRefl = 0;
	int nReflPower = 0;
	int nReflTol = 0;
	int nTol = 0, nDiff;
	int nLdPwTol = 0;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	do {
		// Read Parameter
		CTextParser tp(pParam);
		// if(! tp.GetInt(&nTol, 0)) break;

		nSetPower = (int) READ_ANALOG(RFSetPointAO, &nIOStatus);
		nLdPwTol = (int) READ_ANALOG(PRMA_TL_RFFwPw, &nIOStatus);
		nTol = nSetPower * nLdPwTol / 100;
		nPRMA_TL_RFRefl = (int) READ_ANALOG(PRMA_TL_RFRefl, &nIOStatus);
		nReflTol = nSetPower * nPRMA_TL_RFRefl / 100;

		nPRMA_EC_RFFwPw = (int) READ_ANALOG(PRMA_EC_RFFwPw, &nIOStatus);
		nPRMA_EC_RFPlsmDtc = (int) READ_ANALOG(PRMA_EC_RFPlsmDtc, &nIOStatus);
 
		// Start Monitoring
		while(1)
		{
			nRFPwOnDO = READ_DIGITAL(RFPwOnDO, &nIOStatus);		// 0:Off 1:On
			nSetPower = (int) READ_ANALOG(RFSetPointAO, &nIOStatus);
			if(nRFPwOnDO == 1 && nSetPower > 0)
			{
				// Check Plasma Detect
				nRFPlasmaDetectDI = READ_DIGITAL(RFPlasmaDetectDI, &nIOStatus);		// 0:Off 1:On
				if(nRFPlasmaDetectDI == 0)
				{
					nPlmErrCount++;
					if(nPlmErrCount > nPRMA_EC_RFPlsmDtc)
					{
						ALARM_POST(ALARM_PLASMA_NOT_DETECTED);
						msRet = SYS_ABORTED;
						break;
					}
				}
				else nPlmErrCount = 0;

				// Check Power Tolerance
				if(nTol > 0)
				{
					nReadPower = (int) READ_ANALOG(RFLoadPowAI, &nIOStatus);
					nDiff = abs(nReadPower - nSetPower);
					if(nDiff > nTol)
					{
						nTolErrCount++;
						if(nTolErrCount > PRMA_EC_RFFwPw)
						{
							ALARM_POST(ALARM_RF_POWER_OUT_OF_TOL);
							msRet = SYS_ABORTED;
							break;
						}
					}
					else nTolErrCount = 0;
				}

				// Check Reflect Power
				if(nPRMA_TL_RFRefl > 0)
				{
					nReflPower = (int) READ_ANALOG(RFRefPowAI, &nIOStatus);
					if(nReflPower > nReflTol)
					{
						nRefErrCount++;
						if(nRefErrCount > PRMA_EC_RFFwPw)
						{
							ALARM_POST(ALARM_REFLECT_IS_HIGH);
							msRet = SYS_ABORTED;
							break;
						}
					}
					else nRefErrCount = 0;
				}
			}
			else
			{
				nPlmErrCount = 0;
				nTolErrCount = 0;
			}

			if(! WAIT_SECONDS(1)) { msRet = SYS_ABORTED; break; }
		}
	} while(0);

	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int nParLen = 0;
	char *pParam;
	char szCmd[41], szSubParam[256];

	printf("Entered %s\n", APP_NAME);

	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		if ( nParLen < 1 )	{
			printf( "---> Parameter Error, Len = %d in %s !\n", nParLen, APP_NAME);
			msRet = SYS_ERROR;
			break;
		}

		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );

		//...
		if     (STRCMP_L(szCmd, "SET_POWER"))		msRet = SetPower(szSubParam);
		else if(STRCMP_L(szCmd, "START_MON"))
		{
			//... 2015.01.07 NotUse StartMon in ALD Method RF On/Off
			// msRet = StartMon(szSubParam);
			msRet = SYS_SUCCESS;
		}
		else if(STRCMP_L(szCmd, "SET_PW_VALUE"))	msRet = SetPwValue(szSubParam);
		else if(STRCMP_L(szCmd, "PRESET_POS"))		msRet = PresetPos(szSubParam);
		else 
		{
			printf("---> Invalid Command in %s !\n", APP_NAME);
			msRet = SYS_ERROR;
		}
	} while(0);

	printf("Leave %s\n", APP_NAME);

	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
