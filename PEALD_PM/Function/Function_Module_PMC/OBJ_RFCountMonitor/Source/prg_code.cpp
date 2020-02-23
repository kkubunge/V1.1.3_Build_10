#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <string.h>

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

#define APP_NAME		"RFCountMonitor"

#define MAX_RF_USE		2

//------------------------------------------------------------------------------------------
// Alarm Definition(2671 ~ 2699)
#define ALARM_RF1_COMM_ERROR		2671
#define ALARM_RF2_COMM_ERROR		2672

#define ALARM_RF1_PARAM_SET_FAIL	2673
#define ALARM_RF2_PARAM_SET_FAIL	2674

#define ALARM_RF1_LOAD_PWR_CNT_ERR	2675
#define ALARM_RF2_LOAD_PWR_CNT_ERR	2676

#define ALARM_RF1_REFL_PWR_CNT_ERR	2677
#define ALARM_RF2_REFL_PWR_CNT_ERR	2678

//------------------------------------------------------------------------------------------
// Enumeration  Definition
enum {	CS_OFFLINE,		CS_ONLINE,		CS_CTRLERR	};
enum {	OFFON_OFF,		OFFON_ON	};
enum {	RFALM_PMDIS,	RFALM_PMABORT,	RFALM_WARN	};

enum RecipeTypeEnum
{
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
// Global Variable
int	 g_nLast_LoadCnt[MAX_RF_USE];
int	 g_nLast_ReflCnt[MAX_RF_USE];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {

	{ "PRMD_AL_RFChmOpt",	_K_D_IO	,	PRMD_AL_RFChmOpt,	0 } ,
	//... 2015.11.25
	{ "PRMD_AL_RFChmOptLp",	_K_D_IO	,	PRMD_AL_RFChmOptLp,	0 } ,

	{ "SW_GbIntlckDM",		_K_D_IO	,	SW_GbIntlckDM,		0 } ,
	{ "SW_ModuleFailDM",	_K_D_IO	,	SW_ModuleFailDM,	0 } ,

	{ "RF1CommStsDI",		_K_D_IO	,	RF1CommStsDI,		0 } ,
	{ "RF2CommStsDI",		_K_D_IO	,	RF2CommStsDI,		0 } ,

	//... RF Delivery(Load) Power Items
	{ "RF1LoadCntAIO",		_K_A_IO	,	RF1LoadCntAIO,		0 } ,
	{ "RF2LoadCntAIO",		_K_A_IO	,	RF2LoadCntAIO,		0 } ,

	{ "RF1LoadTolAO",		_K_A_IO	,	RF1LoadTolAO,		0 } ,
	{ "RF2LoadTolAO",		_K_A_IO	,	RF2LoadTolAO,		0 } ,

	{ "PRMA_TL_RF1LdCnt",	_K_A_IO	,	PRMA_TL_RF1LdCnt,	0 } ,
	{ "PRMA_TL_RF2LdCnt",	_K_A_IO	,	PRMA_TL_RF2LdCnt,	0 } ,

	//... 2015.11.25	Add RF Parameter For Lot Pre & Clean Recipe 
	{ "PRMA_TL_RF1LdCntLp",	_K_A_IO	,	PRMA_TL_RF1LdCntLp,	0 } ,
	{ "PRMA_TL_RF2LdCntLp",	_K_A_IO	,	PRMA_TL_RF2LdCntLp,	0 } ,

	{ "PRMA_TL_RF1LdPw",	_K_A_IO	,	PRMA_TL_RF1LdPw,	0 } ,
	{ "PRMA_TL_RF2LdPw",	_K_A_IO	,	PRMA_TL_RF2LdPw,	0 } ,

	//... 2015.11.25
	{ "PRMA_TL_RF1LdPwLp",	_K_A_IO	,	PRMA_TL_RF1LdPwLp,	0 } ,
	{ "PRMA_TL_RF2LdPwLp",	_K_A_IO	,	PRMA_TL_RF2LdPwLp,	0 } ,
	
	//... RF Reflect Power Items
	{ "RF1RefWrnRstDO",		_K_D_IO	,	RF1RefWrnRstDO,		0 } ,
	{ "RF2RefWrnRstDO",		_K_D_IO	,	RF2RefWrnRstDO,		0 } ,

	{ "RF1ReflCntAI",		_K_A_IO	,	RF1ReflCntAI,		0 } ,
	{ "RF2ReflCntAI",		_K_A_IO	,	RF2ReflCntAI,		0 } ,

	{ "RF1RefWrnThrsAO",	_K_A_IO	,	RF1RefWrnThrsAO,	0 } ,
	{ "RF2RefWrnThrsAO",	_K_A_IO	,	RF2RefWrnThrsAO,	0 } ,

	{ "RF1RefWrnTOAO",		_K_A_IO	,	RF1RefWrnTOAO,		0 } ,
	{ "RF2RefWrnTOAO",		_K_A_IO	,	RF2RefWrnTOAO,		0 } ,

	{ "PRMA_TL_RF1RefCnt",	_K_A_IO	,	PRMA_TL_RF1RefCnt,	0 } ,
	{ "PRMA_TL_RF2RefCnt",	_K_A_IO	,	PRMA_TL_RF2RefCnt,	0 } ,

	//... 2015.11.25
	{ "PRMA_TL_RF1RefCntLp",_K_A_IO	,	PRMA_TL_RF1RefCntLp,0 } ,
	{ "PRMA_TL_RF2RefCntLp",_K_A_IO	,	PRMA_TL_RF2RefCntLp,0 } ,

	{ "PRMA_TL_RF1RefPw",	_K_A_IO	,	PRMA_TL_RF1RefPw,	0 } ,
	{ "PRMA_TL_RF2RefPw",	_K_A_IO	,	PRMA_TL_RF2RefPw,	0 } ,

	//... 2015.11.25
	{ "PRMA_TL_RF1RefPwLp",	_K_A_IO	,	PRMA_TL_RF1RefPwLp,	0 } ,
	{ "PRMA_TL_RF2RefPwLp",	_K_A_IO	,	PRMA_TL_RF2RefPwLp,	0 } ,

	{ "PRMA_TL_RF1RefTm",	_K_A_IO	,	PRMA_TL_RF1RefTm,	0 } ,
	{ "PRMA_TL_RF2RefTm",	_K_A_IO	,	PRMA_TL_RF2RefTm,	0 } ,
	
	//... 2015.11.25 
	{ "PRMA_TL_RF1RefTmLp",	_K_A_IO	,	PRMA_TL_RF1RefTmLp,	0 } ,
	{ "PRMA_TL_RF2RefTmLp",	_K_A_IO	,	PRMA_TL_RF2RefTmLp,	0 } ,
	
	{ "RF1RefFltRstDO",		_K_D_IO	,	RF1RefFltRstDO,		0 } ,	// 2014.12.02
	{ "RF2RefFltRstDO",		_K_D_IO	,	RF2RefFltRstDO,		0 } ,	// 2014.12.02

	{ "HF_END_DO_A",		_K_D_IO	,	HF_END_DO_A,		0 } ,
	{ "HF_END_DO_B",		_K_D_IO	,	HF_END_DO_B,		0 } ,
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//... Check Count Monitor
Module_Status CheckCount(char *pParam)
{
	char pszLocFtnName[] = "CheckCount";
	char pszFtnName[256];
	Module_Status msRet = SYS_SUCCESS;
	int		nIOStatus;

	int		nRFOnCount[MAX_RF_USE];
	//
	int		nParmData = 0;
	int		nReadData = 0;
	int		nCurCount = 0;
	//
	int		nRFAlmOpt = RFALM_PMDIS;

	char szAlarm[256]	 = "";
	char szExtAlarm[256] = "";
	//... 2015.11.25
	int		nRecipeType = -1;	
	int     nParmIndex  =  0;
	int     nArmOpIndex =  0;
	//...

	CTextParser tp(pParam);
	
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	//... 2015.11.25 Check Recipe Type
	if(!tp.GetInt(&nRecipeType, 0))
	{
		printf("[RF CheckCount ]Recipe Type Reading Fail\n");	
		return SYS_ABORTED;
	}

	if(nRecipeType == RT_ProcRcp) { nParmIndex = 0; nArmOpIndex = 0; }
	else 						  { nParmIndex = 2; nArmOpIndex = 1; } //... Rf parameter for Lot pre, clean recipe 
   //...

	for (int i = 0; i < MAX_RF_USE; i++)
	{
		//... Parsing RF On Count from SubParm
		nRFOnCount[i] = 0;
		if(! tp.GetInt(&nRFOnCount[i], 0))
		{
			msRet = SYS_ABORTED;
			break;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//... Case. Check Skip
		if(0 == nRFOnCount[i])		continue;

		//////////////////////////////////////////////////////////////////////////
		//... Check RF Delivery(Load) Count
		nParmData = (int) READ_ANALOG(PRMA_TL_RF1LdCnt + i + nParmIndex, &nIOStatus); //... 2015.11.25
		nReadData = (int) READ_ANALOG(RF1LoadCntAIO + i, &nIOStatus);
		nCurCount = nReadData - g_nLast_LoadCnt[i];
		g_nLast_LoadCnt[i] = nReadData;		//... accumulate Count
		
		if(0 < nParmData && (abs(nRFOnCount[i] - nCurCount) > nParmData))
		{
			//...
			memset(szAlarm,		0, sizeof(szAlarm));
			memset(szExtAlarm,	0, sizeof(szExtAlarm));
			//
			ALARM_MESSAGE_GET(ALARM_RF1_LOAD_PWR_CNT_ERR + i, szAlarm);
			sprintf(szExtAlarm, "%s [%d / %d]", szAlarm, nCurCount, nRFOnCount[i]);
			ALARM_MESSAGE_SET(ALARM_RF1_LOAD_PWR_CNT_ERR + i, szExtAlarm);
			ALARM_POST(ALARM_RF1_LOAD_PWR_CNT_ERR + i);
			//
			printf("[ERR] RF%d Load Count Rcp[%d] Cur[%d/%d] Tol[%d]\n", 
				i+1, nRFOnCount[i], nCurCount, nReadData, nParmData);
			msRet = SYS_ABORTED;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check Communication Status
		if(CS_ONLINE != READ_DIGITAL(RF1CommStsDI + i, &nIOStatus))
		{
			ALARM_POST(ALARM_RF1_COMM_ERROR + i);
			msRet = SYS_ABORTED;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Check RF Reflect Count
		nParmData = (int) READ_ANALOG(PRMA_TL_RF1RefCnt + i + nParmIndex, &nIOStatus); //... 2015.11.25
		nReadData = (int) READ_ANALOG(RF1ReflCntAI + i, &nIOStatus);
		nCurCount = nReadData - g_nLast_ReflCnt[i];
		g_nLast_ReflCnt[i] = nReadData;		//... accumulate Count
		
		if(0 < nParmData && (abs(nCurCount) > nParmData))
		{
			//...
			memset(szAlarm,		0, sizeof(szAlarm));
			memset(szExtAlarm,	0, sizeof(szExtAlarm));
			//
			ALARM_MESSAGE_GET(ALARM_RF1_REFL_PWR_CNT_ERR + i, szAlarm);
			sprintf(szExtAlarm, "%s [%d]", szAlarm, nCurCount);
			ALARM_MESSAGE_SET(ALARM_RF1_REFL_PWR_CNT_ERR + i, szExtAlarm);
			ALARM_POST(ALARM_RF1_REFL_PWR_CNT_ERR + i);
			//
			printf("[ERR] RF%d Reflect Count Cur[%d/%d] Limit[%d]\n", 
				i+1, nCurCount, nReadData, nParmData);
			msRet = SYS_ABORTED;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	//... Check Alarm Option
	if(SYS_SUCCESS != msRet)
	{
		//... Read Alarm Option
		nRFAlmOpt = READ_DIGITAL(PRMD_AL_RFChmOpt + nArmOpIndex, &nIOStatus);
		if     (RFALM_PMDIS   == nRFAlmOpt)		WRITE_DIGITAL(SW_ModuleFailDM,	OFFON_ON, &nIOStatus);
		else if(RFALM_PMABORT == nRFAlmOpt)		WRITE_DIGITAL(SW_GbIntlckDM,	OFFON_ON, &nIOStatus);
		else									msRet = SYS_SUCCESS;		//... Case. Warning
	}

	printf("Leaved %s[%d]\n", pszFtnName, msRet);
	return	msRet;
}

//////////////////////////////////////////////////////////////////////////
//... Write User Config to Driver
Module_Status ReadConfig(char* pParam)
{
	Module_Status	msRet = SYS_SUCCESS;
	int		nIOStatus;
	double	dbParmData = 0;
	//... 2015.11.25
	int		nRecipeType = -1;	
	int     nParmIndex  =  0;
	CTextParser tp(pParam);
	//...

	//... 2015.11.25 Check Recipe Type
	if(!tp.GetInt(&nRecipeType, 0))
	{
		printf("[RF ReadConfig ]Recipe Type Reading Fail\n");	
		return SYS_ABORTED;
	}
	
	if(nRecipeType == RT_ProcRcp) { nParmIndex = 0;}
	else 						  { nParmIndex = 2;} //... Rf parameter for Lot pre, clean recipe 
   //...

	for (int i = 0; i < MAX_RF_USE; i++)
	{
		g_nLast_LoadCnt[i] = 0;		//... Last Count
		g_nLast_ReflCnt[i] = 0;		//... Last Count

		//////////////////////////////////////////////////////////////////////////
		//... Set RF Delivery(Load) Parameter		
		//... Reset Count
		WRITE_ANALOG(RF1LoadCntAIO + i, 0, &nIOStatus);
		if(nIOStatus == 0)		WRITE_ANALOG(RF1LoadCntAIO + i, 0, &nIOStatus);
		
		//... Set Tolerance
		dbParmData = READ_ANALOG(PRMA_TL_RF1LdPw + i + nParmIndex, &nIOStatus);   //... 2015.11.25
		WRITE_ANALOG(RF1LoadTolAO + i, dbParmData, &nIOStatus);
		if(nIOStatus == 0)		WRITE_ANALOG(RF1LoadTolAO + i, dbParmData, &nIOStatus);
		
		//... Check Communication Status
		if(CS_ONLINE != READ_DIGITAL(RF1CommStsDI + i, &nIOStatus))
		{
			ALARM_POST(ALARM_RF1_COMM_ERROR + i);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Set RF Reflect Parameter
		//... Reset Count
		WRITE_DIGITAL(RF1RefWrnRstDO + i, OFFON_ON, &nIOStatus);
		if(nIOStatus == 0)		WRITE_DIGITAL(RF1RefWrnRstDO + i, OFFON_ON, &nIOStatus);

		//... Reset Fault Count   2014.12.02
		WRITE_DIGITAL(RF1RefFltRstDO + i, OFFON_ON, &nIOStatus);
		if(nIOStatus == 0)		WRITE_DIGITAL(RF1RefFltRstDO + i, OFFON_ON, &nIOStatus);

		//... Set Tolerance
		dbParmData = READ_ANALOG(PRMA_TL_RF1RefPw + i + nParmIndex, &nIOStatus);  //... 2015.11.25
		WRITE_ANALOG(RF1RefWrnThrsAO + i, dbParmData, &nIOStatus);
		if(nIOStatus == 0)		WRITE_ANALOG(RF1RefWrnThrsAO + i, dbParmData, &nIOStatus);
		
		//... Set Threshold Time
		dbParmData = READ_ANALOG(PRMA_TL_RF1RefTm + i + nParmIndex, &nIOStatus);  //... 2015.11.25
		WRITE_ANALOG(RF1RefWrnTOAO + i, dbParmData, &nIOStatus);
		if(nIOStatus == 0)		WRITE_ANALOG(RF1RefWrnTOAO + i, dbParmData, &nIOStatus);

		WRITE_DIGITAL(HF_END_DO_A + i, OFFON_ON, &nIOStatus);
		if(nIOStatus == 0)		WRITE_DIGITAL(HF_END_DO_A + i, OFFON_ON, &nIOStatus);
	}

	return	msRet;
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

		STR_SEPERATE(pParam, szCmd, szSubParam, 255);

 		if     (STRCMP_L(szCmd, "READ_CONFIG")) 		msRet = ReadConfig(szSubParam);
		else if(STRCMP_L(szCmd, "CHECK_COUNT")) 		msRet = CheckCount(szSubParam);
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

	//... Initialize Global Variable
	for(int i = 0; i < MAX_RF_USE; i++)
	{
		g_nLast_LoadCnt[i] = 0;
		g_nLast_ReflCnt[i] = 0;
	}

	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
