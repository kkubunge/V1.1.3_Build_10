#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "TextParser_C.h"
//------------------------------------------------------------------------------------------
#define ALARM_CHAMBER_PRESSURE_HIGH		8070
#define ALARM_SHCOVER_OPEN				8085
//... 2015.09.04
#define CHAMBER_VACCUM_SENSOR_FAIL		8094
#define CHAMBER_PRESSURE_HI				8095
#define CHAMBER_PRESSURE_LOW			8096


#define CONFIG_FILE_NAME	".\\Function\\Function_INT_Monitor\\PMTypeCfg.ini"	
						
const double MAX_PRESSURE	   =  10.0;
const double MAX_PRESSURE_VENT = 790.0;

BOOL	SIMULATION_MODE = FALSE;

enum	{	Transition,	ATM,	Vacuum,	Error	};
enum	{	eNOTUSE,		eUSE			};
enum	{	OFFON_OFF,	OFFON_ON	};
int g_nPMType = 0;

//...2015.09.05
int    g_nSensorType  = 0;
int    g_nRcpStepNo   = 0;
char   g_pRcpName[16] = {"",};
float  g_dbHiLimitPrs = 0.0;
float  g_dbLwLimitPrs = 0.0;
int    g_nPrsTimeOut = 0;
int	   g_nPrsBandInt = 0;
int	   g_nPrsIntUse	 = 0;

enum {eGague10Torr, eGague1000Torr};
enum {eNormal, eHIGHLIMIT, eLOWLIMIT};
enum {eUnKnown, eProc, ePreProc, ePostProc, eIdle, eClean, ePurge};
enum {eWarning, eChamDis, eAbort};
enum { INTLK_OFF, INTLK_ON };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "ABORT_GAS"			, _K_F_IO	,	ABORT_GAS,			0	} ,

	{ "M651_Pressure"		, _K_A_IO	,	ChmPres10T,			0	} ,
	{ "ChmPrsAI"			, _K_A_IO	,	ChmPres1000T,		0	} ,
	{ "ChmPrssDI1"			, _K_D_IO	,	ChmPresSwitch,		0	} ,
	{ "PRO_RunStsDM"		, _K_D_IO	,	RunStsDM,			0	} ,
	{ "SW_GbIntlckDM"		, _K_D_IO	,	GbIntlckDM,			0	} ,
	{ "VRCP_RunningDI"		, _K_D_IO	,	VRCP_RunningDI,		0	} ,
	{ "INTLKS_CONTROL"		, _K_D_IO	,	INTLKS_CONTROL,		0	} ,
	{ "RFSHCovIntDI"		, _K_D_IO	,	SHCover,			0	} ,
	{ "RFSHCovInt2DI"		, _K_D_IO	,	SHCover2,			0	} ,
	{ "RFPwOnDO"			, _K_D_IO	,	RFPwOnDO,			0	} ,
	{ "RFPwOn2DO"			, _K_D_IO	,	RFPwOn2DO,			0	} ,
	//2015.09.04
	{ "PRO_RcpTypeDM"		, _K_D_IO	,	PRO_RcpTypeDM,		0	} ,
	{ "PRO_StepNameSM"		, _K_S_IO	,	PRO_StepNameSM,		0	} ,
	{ "PRO_CurStepAM"		, _K_A_IO	,	PRO_CurStepAI,		0	} ,
	{ "SW_ModuleFailDM"	    , _K_D_IO	,	SW_ModuleFailDM,	0	} ,
	""
};


//------------------------------------------------------------------------------------------
void ParsePMConfig(char *szData)
{
	char szItem[64];
	if(sizeof(szData) < 1) return;
	tpSetText(szData);
	if(!tpGetWord(szItem, 0)) return;

	if(strcmp(szItem, "UseOption") == 0) tpGetInt(&g_nPMType, 0);
}

void ParsePrsConfig(char *szData)
{
	char szItem[64];
	if(sizeof(szData) < 1) return;
	tpSetText(szData);
	if(!tpGetWord(szItem, 0)) return;
	
	if(strcmp(szItem, "UseOption") == 0)		tpGetInt(&g_nPrsIntUse, 	0);
	else if(strcmp(szItem, "gaugeType") == 0)	tpGetInt(&g_nSensorType,	0);
	else if(strcmp(szItem, "StepNo") == 0)		tpGetInt(&g_nRcpStepNo,		0);
	//else if(strcmp(szItem, "StepName") == 0)	tpGetWord(g_pRcpName,	    0);
	else if(strcmp(szItem, "HighLimitPrs") == 0)tpGetFloat(&g_dbHiLimitPrs, 0);
	else if(strcmp(szItem, "LowLimiPrs") == 0)  tpGetFloat(&g_dbLwLimitPrs, 0);
	else if(strcmp(szItem, "TimeOut") == 0)		tpGetInt(&g_nPrsTimeOut,    0);
	else if(strcmp(szItem, "IntOption") == 0)	tpGetInt(&g_nPrsBandInt,    0);
}

BOOL ReadConfigration()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(CONFIG_FILE_NAME, "rt");
		if(fp == NULL) break;
		// printf("-----> Read PMC Configration Data\n");

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				tpSetText(szRead);
				tpGetChar();
				tpGetString(szItem, ']');

				if(strcmp("HeaterCoverMonitor", szItem) == 0)			nTitle = 1;
				if(strcmp("PressureConfiguration", szItem) == 0)		nTitle = 2;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
						case 1: ParsePMConfig(szRead); break;
						case 2: ParsePrsConfig(szRead); break;
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
		printf("-----> Config File Reading Failed : %s", CONFIG_FILE_NAME);
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsShowerHeadCoverOpen()
{
	BOOL	bRet = FALSE;
	int		CommStatus = 0;

	if( READ_DIGITAL(SHCover,  &CommStatus) == 0 || 
		READ_DIGITAL(SHCover2, &CommStatus) == 0)
	{
		bRet = TRUE;
	}
	
	return bRet;
}

BOOL IsProcessRunning()
{
	BOOL bRet = FALSE;
	int CommStatus=0;
	if(READ_DIGITAL(RunStsDM,&CommStatus) ==1 || READ_DIGITAL(VRCP_RunningDI,&CommStatus) ==1) bRet = TRUE;
	return bRet;
}

BOOL SetInterlock()
{
	BOOL bRet = TRUE;
	int CommStatus=0;
	if(IsProcessRunning()) WRITE_DIGITAL( INTLKS_CONTROL , 0 , &CommStatus );
	return bRet;
}

BOOL IsPressureSwitchNotVacuum()
{
	BOOL bRet = FALSE;
	int CommStatus=0;
	int nValve=0;
	if(READ_DIGITAL(ChmPresSwitch,&CommStatus) != Vacuum) bRet = TRUE;
	return bRet;
}

BOOL IsOverPressure()
{
	BOOL bRet = FALSE;
	int CommStatus=0;
	int nMaxCount=2;
	int nCount=0;
	int i=0;
	double nValue=0;

	for(i=0;i<5;i++)
	{
		if(READ_ANALOG(ChmPres1000T, &CommStatus) >=  MAX_PRESSURE_VENT) nCount++;
		Sleep(800);
	}

	if(nCount > nMaxCount) bRet=TRUE;

	return bRet;
}

BOOL IsPressureLimit()
{
	BOOL bRet = FALSE;
	int CommStatus=0;
	int nMaxCount=2;
	int nCount=0;
	int i=0;
	double nValue=0;

	for(i=0;i<5;i++)
	{
		if(READ_ANALOG(ChmPres10T, &CommStatus) >=  MAX_PRESSURE) nCount++;
		Sleep(800);
	}

	if(nCount > nMaxCount) bRet=TRUE;

	return bRet;
}

BOOL IsPressureLimit2()
{
	BOOL bRet = FALSE;
	int CommStatus=0;
	int nMaxCount=2;
	int nCount=0;
	int i=0;
	double nValue=0;

	for(i=0;i<5;i++)
	{
		if(READ_ANALOG(ChmPres1000T, &CommStatus) >=  100) nCount++;
		Sleep(800);
	}

	if(nCount > nMaxCount) bRet=TRUE;

	return bRet;
}
//------------------------------------------------------------------------------------------
//... 2015.09.04
int IsOkPressureLimit()
{
	int nGagueType = eGague10Torr;
	int nCount = 0;
	int nRet   = eNormal;
	int i;
	int nHiCount = 0,nLwCount = 0;
	int nCommStatus;
	
	if(g_nSensorType == eGague1000Torr)		nGagueType =  ChmPres1000T;	
	else if(g_nSensorType == eGague10Torr)	nGagueType =  ChmPres10T;
	
	for(i=0;i<(g_nPrsTimeOut*2);i++)
	{
		if(READ_ANALOG(nGagueType, &nCommStatus) >= g_dbHiLimitPrs) nHiCount++;
		Sleep(500);
		if(READ_ANALOG(nGagueType, &nCommStatus) <= g_dbLwLimitPrs) nLwCount++;
		Sleep(500);
		if(!IsProcessRunning())
		{
			nHiCount = 0;
			nLwCount = 0;
			break;
		}
		if(nHiCount >= g_nPrsTimeOut) {nRet=eHIGHLIMIT;break;}
		if(nLwCount >= g_nPrsTimeOut) {nRet=eLOWLIMIT;break;}
	}	
	return nRet;
}

BOOL IsOKMonRcpStep()
{
	BOOL bRet = FALSE;
	char szStepName[16] = {"",};
	int nCommStaus;
	do
	{
		if(READ_DIGITAL(PRO_RcpTypeDM,&nCommStaus) != eProc) break;
		if((int)READ_ANALOG(PRO_CurStepAI,&nCommStaus) != g_nRcpStepNo) break;
		READ_STRING(PRO_StepNameSM,szStepName,&nCommStaus);
		//if(!STRCMP(szStepName,g_pRcpName)) break;
		bRet = TRUE;
	}
	while(0);	
	return bRet;
}

void InitMonOption()
{
	int nIOStatus;

	switch(g_nPrsBandInt)
	{
	case eWarning:
		break;
	case eChamDis:
		WRITE_DIGITAL(SW_ModuleFailDM, INTLK_ON, &nIOStatus);
		break;
	case eAbort:
		WRITE_DIGITAL(SW_GbIntlckDM, INTLK_ON, &nIOStatus);
		break;		
	}	
}




void PressureBandInterlock_Monitor() {
	int CommStatus =0;
	
	printf(" ==>Pressure Band Interlock_Monitor Started! \n");
	while ( TRUE ) {
		Sleep(1000);
		if(IsProcessRunning())
		{   // IsOver 10 Torr       IsOver 1000 Torr       
			if(IsOKMonRcpStep())
			{
				switch(IsOkPressureLimit())
				{
				case eNormal:
					break;
				case eHIGHLIMIT:
					InitMonOption();
					printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning GbIntlckDM Write \n");
					if(ALARM_STATUS(CHAMBER_PRESSURE_HI) == 0)
					{
						ALARM_POST(CHAMBER_PRESSURE_HI);	
						printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning ALARM_CHAMBER_PRESSURE_HIGH Write \n");
					}
					break;
				case eLOWLIMIT:
					InitMonOption();
					printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning GbIntlckDM Write \n");					
					if(ALARM_STATUS(CHAMBER_PRESSURE_LOW) == 0)
					{
						ALARM_POST(CHAMBER_PRESSURE_LOW);	
						printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning ALARM_CHAMBER_PRESSURE_LOW Write \n");
					}
					break;
				}
			}		
		}
		
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
void PressureInterlock_Monitor() {
	int CommStatus =0;

	printf(" ==>PressureInterlock_Monitor Started! \n");
	while ( TRUE ) {

		SetInterlock(); // If Process Running Interlock Set
		Sleep(1000);

		// Check if Chambher Pressure > 790 torr
		if(IsOverPressure())  
		{
			if(ALARM_STATUS(ALARM_CHAMBER_PRESSURE_HIGH) == 0)
			{
				if(IsProcessRunning())
				{
					WRITE_DIGITAL( GbIntlckDM , 1 , &CommStatus );
					printf(" ==>PressureInterlock_Monitor  GbIntlckDM Write \n");
				}
				else
				{
					RUN_FUNCTION(ABORT_GAS, "");
					printf(" ==>PressureInterlock_Monitor  ABORT_GAS Write \n");
				}
			}

			if(ALARM_STATUS(ALARM_CHAMBER_PRESSURE_HIGH) == 0)
			{
				ALARM_POST(ALARM_CHAMBER_PRESSURE_HIGH);	
				printf(" ==>PressureInterlock_Monitor IsOverPressure  ABORT_GAS Write \n");
			}
		}

		if(IsProcessRunning())
		{   // IsOver 10 Torr       IsOver 1000 Torr       
			if(IsPressureLimit() || IsPressureLimit2() || IsPressureSwitchNotVacuum())
			{
				WRITE_DIGITAL( GbIntlckDM , 1 , &CommStatus );
				printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning GbIntlckDM Write \n");
				if(ALARM_STATUS(CHAMBER_VACCUM_SENSOR_FAIL) == 0)
				{
					ALARM_POST(CHAMBER_VACCUM_SENSOR_FAIL);	
					printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning ALARM_CHAMBER_PRESSURE_HIGH Write \n");
				}
			}		
		}
	
		if(IsProcessRunning())
		{   // IsOver 10 Torr       IsOver 1000 Torr       
			if(IsPressureLimit() || IsPressureLimit2() || IsPressureSwitchNotVacuum())
			{
				WRITE_DIGITAL( GbIntlckDM , 1 , &CommStatus );
				printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning GbIntlckDM Write \n");
				if(ALARM_STATUS(ALARM_CHAMBER_PRESSURE_HIGH) == 0)
				{
					ALARM_POST(ALARM_CHAMBER_PRESSURE_HIGH);	
					printf(" ==>PressureInterlock_Monitor MainCheck IsProcessRunning ALARM_CHAMBER_PRESSURE_HIGH Write \n");
				}
			}		
		}

	}
	_endthread();
}

//////////////////////////////////////////////////////////////////////////
void ShowerHeadCover_Monitor() {
	int CommStatus = 0;

	printf("[INFO] ShowerHeadCover_Monitor Started!\n");

	while(TRUE)
	{
		Sleep(1000);

		if(IsShowerHeadCoverOpen())
		{	//... Opened RF Cover
			if(IsProcessRunning())
			{
				WRITE_DIGITAL(GbIntlckDM,	1, &CommStatus);
				printf("[ABORT] ShowerHeadCover_Monitor Set GbIntlckDM\n");
			}

			if( OFFON_OFF != READ_DIGITAL(RFPwOnDO,  &CommStatus) || 
				OFFON_OFF != READ_DIGITAL(RFPwOn2DO, &CommStatus))
			{
				WRITE_DIGITAL(RFPwOnDO,	OFFON_OFF, &CommStatus);
				WRITE_DIGITAL(RFPwOn2DO, OFFON_OFF, &CommStatus);
				printf("[INFO] ShowerHeadCover_Monitor RF Power Off\n");
			}

			if(ALARM_STATUS(ALARM_SHCOVER_OPEN) == 0)	ALARM_POST(ALARM_SHCOVER_OPEN);
		}
	}

	_endthread();
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {

	ReadConfigration();
	printf("==>g_nPMType:%d\n",g_nPMType);

	_beginthread( (void*)PressureInterlock_Monitor	, 0 , NULL );

	if(g_nPMType == eUSE)		_beginthread( (void*)ShowerHeadCover_Monitor,0,NULL);
	//...2015.09.03
	if(g_nPrsIntUse  == eUSE)	_beginthread( (void*)PressureBandInterlock_Monitor,0,NULL);

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {	
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
