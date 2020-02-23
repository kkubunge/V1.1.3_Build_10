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
#include "TextLogDll.h"

#define APP_NAME	"PressureControl"

//------------------------------------------------------------------------------------------
// Alarm Definition(2500 ~ 2599)


//------------------------------------------------------------------------------------------
// Enumeration  Definition


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_ModuleFailDM",	 _K_D_IO	,	SW_ModuleFailDM,	0	} ,
	{ "FC_PRESSURE_CONTROL"	, _K_D_IO	,	CONTROL_CHECK		    ,	0	} ,
	{ "M651_CommStatus"		, _K_D_IO	,	MKS_CommStatus		    ,	0	} ,
	{ "M651_Valve"			, _K_D_IO	,	MKS_Control			    ,	0	} , //  Stop/Run   Open   Close
	{ "M651_Pressure"		, _K_A_IO	,	MKS_Pressure		    ,	0	} ,
	{ "M651_Position"		, _K_A_IO	,	MKS_Position		    ,	0	} ,
	{ "M651_Set_Position"	, _K_A_IO	,	MKS_Set_Position        ,	0	} ,
	{ "M651_Set_Pressure"	, _K_A_IO	,	MKS_Set_Pressure    	,	0	} ,
	{ "PRMA_TL_ThrtlPos"	, _K_A_IO	,	PRMA_TL_ThrtlPos		,   0	} ,
	{ "PRMA_TL_ThrtlPrss"	, _K_A_IO	,	PRMA_TL_ThrtlPrss		,   0	} ,
	{ "PRMA_TO_ThrtlPos"	, _K_A_IO	,	PRMA_TO_ThrtlPos		,   0	} ,
	{ "PRMA_TO_ThrtlPrss"	, _K_A_IO	,	PRMA_TO_ThrtlPrss		,   0	} ,
	{ "M651_ModeDM"			, _K_D_IO	,	M651_ModeDM		,   0	} ,
	""
};

//------------------------------------------------------------------------------------------

Module_Status OpenTV()
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	_LOG("-----> Mks Open in %s", APP_NAME);
	WRITE_DIGITAL( M651_ModeDM, 0, &nIOStatus); // 0:Position 1:Pressure
	WRITE_DIGITAL( MKS_Control , 1 , &nIOStatus );
	return msRet;
}

Module_Status CloseTV()
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	_LOG("-----> Mks Close in %s", APP_NAME);
	WRITE_DIGITAL( M651_ModeDM, 0, &nIOStatus); // 0:Position 1:Pressure
	WRITE_DIGITAL( MKS_Control , 2 , &nIOStatus );
	return msRet;
}

Module_Status SetPosition(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	CTextParser tp(szParam);
	double dbSetData = 0;

	_LOG("-----> Mks Pos Setting Started in %s", APP_NAME);
	tp.GetDbFloat(&dbSetData);
	WRITE_DIGITAL(M651_ModeDM, 0, &nIOStatus); // 0:Position 1:Pressure
	WRITE_ANALOG( MKS_Set_Position , dbSetData , &nIOStatus );
	_LOG("-----> Mks Pos Setting Finished in %s", APP_NAME);
	return msRet;
}

Module_Status SetPressure(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	CTextParser tp(szParam);
	double dbSetData = 0;

	tp.GetDbFloat(&dbSetData);
	if ( dbSetData <= 0 ) return ( OpenTV() );

	_LOG("-----> Mks Pres Setting Started in %s", APP_NAME);
	WRITE_DIGITAL(M651_ModeDM, 1, &nIOStatus); // 0:Position 1:Pressure
	WRITE_ANALOG( MKS_Set_Pressure , dbSetData , &nIOStatus ); //*10 from t0 5
	_LOG("-----> Mks Pres Setting Finished in %s", APP_NAME);
	return msRet;
}

//------------------------------------------------------------------------------------------
// Initialize Function
#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\PMCConfig.ini"
BOOL g_bInit = FALSE;

void ParseCfgData(char *szData)
{
	/*
	char szItem[40] = "";
	char szValue[40] = "";
	BOOL bEnable = TRUE;

	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(! tp.GetWord(szValue)) break;
		tp.MakeLower(szValue);

		if(strcmp(szItem, "HomeSensorCheckPosition") == 0) g_dbMT_HmChkPosAM = atof(szValue);
		else if(strcmp(szItem, "SetPointUnit") == 0)
		{
			if(strcmp(szValue, "mil") == 0)
			{
				g_nSetPointUnit = 0; // 0:mil 1:mm
				g_nZPosIn = ZMStgPosIn;
				g_nZPosOut = ZMStgPosOut;
			}
			else if(strcmp(szValue, "mm") == 0)
			{
				g_nSetPointUnit = 1;
				g_nZPosIn = ZMStgPosIn2;
				g_nZPosOut = ZMStgPosOut2;
			}
		}
		else if(strcmp(szItem, "LoadPosCheckOption") == 0) g_nLdPosChkOpt = atoi(szValue);
		else if(strcmp(szItem, "AlarmOption") == 0) g_nAlarmOption = atoi(szValue);
	} while(0);
	*/
}

BOOL ReadPmcConfig()
{
	BOOL bRet = FALSE;
	/*
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

				if(strcmp("Z-Motion Control", szItem) == 0)			nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseCfgData(szRead); break;
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
	*/
	return bRet;
}

BOOL Initialize()
{
	BOOL bRet = FALSE;
	do {
		ReadPmcConfig();

		bRet = TRUE;
	} while(0);
	return bRet;
}

//------------------------------------------------------------------------------------------


//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int nParLen = 0;
	char *pParam;
	char szCmd[41], szSubParam[256];

	if(g_bInit == FALSE) g_bInit = Initialize();

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

		if( STRCMP_L(szCmd, "POSITION") ) {
			msRet = SetPosition(szSubParam);
		}
		else if( STRCMP_L(szCmd, "PRESSURE") ) {
			msRet = SetPressure(szSubParam);
		}
		else if( STRCMP_L(szCmd, "OPEN") ) {
			msRet = OpenTV();
		}
		else if( STRCMP_L(szCmd, "CLOSE") ) {
			msRet = CloseTV();
		}
		else {
			printf("---> Invalid Command in %s !\n", APP_NAME);
			msRet = SYS_ERROR;
		}
	} while(0);

	// 2005.01.25 by cskim
	printf("Leaved %s\n", APP_NAME);
	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
