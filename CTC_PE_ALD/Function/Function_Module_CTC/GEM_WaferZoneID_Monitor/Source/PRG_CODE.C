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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum	{ IDLE, RUN };
enum	{ A,	B,	C,	X,	Y,	D };

enum	{ EN_LPM_A, EN_LPM_B, EN_LPM_C };

#define MAX_PM_ZONE		2		// for MAHA-DPT
#define MAX_LPM_SLOT	25		// 300 mm


typedef struct{
//	int nWaferID;
	char szWaferZoneID[32];
}SPRJobInfo1;

typedef struct {
	int nProcessState;
	int nWaferID;
	char szModuleID[255];
	char szPJID[255];	
}SPRJobInfo2;

typedef struct {
	char szCJID[255];
}SCTRJobInfo;

typedef struct {
	BOOL bUse;
	BOOL bPMUse[6];
}SPRJobPMUseInfo;
SPRJobInfo1 g_sLPMA_PRJobInfo[25], g_sLPMB_PRJobInfo[25], g_sLPMC_PRJobInfo[25];
SCTRJobInfo g_sLPMA_CTRJobInfo[25], g_sLPMB_CTRJobInfo[25], g_sLPMC_CTRJobInfo[25];
SPRJobPMUseInfo g_sLPMA_PMUse, g_sLPMB_PMUse, g_sLPMC_PMUse;

int g_nPM1_EventCount, g_nPM2_EventCount, g_nPM3_EventCount;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {

	{ "CTC.PM1_Wafer_Status"		, _K_D_IO	,   PM1_Wafer_Status				,	0   } ,
	{ "CTC.PM1_Wafer_Status2"		, _K_D_IO	,   PM1_Wafer_Status2				,	0   } ,

	{ "CTC.PM2_Wafer_Status"		, _K_D_IO	,   PM2_Wafer_Status				,	0   } ,
	{ "CTC.PM2_Wafer_Status2"		, _K_D_IO	,   PM2_Wafer_Status2				,	0   } ,

	{ "CTC.PM3_Wafer_Status"		, _K_D_IO	,   PM3_Wafer_Status				,	0   } ,
	{ "CTC.PM3_Wafer_Status2"		, _K_D_IO	,   PM3_Wafer_Status2				,	0   } ,

	{ "CTC.PM1_Wafer_Source"		, _K_D_IO	,   PM1_Wafer_Source				,	0   } ,
	{ "CTC.PM1_Wafer_Source2"		, _K_D_IO	,   PM1_Wafer_Source2				,	0   } ,
	
	{ "CTC.PM2_Wafer_Source"		, _K_D_IO	,   PM2_Wafer_Source				,	0   } ,
	{ "CTC.PM2_Wafer_Source2"		, _K_D_IO	,   PM2_Wafer_Source2				,	0   } ,

	{ "CTC.PM3_Wafer_Source"		, _K_D_IO	,   PM3_Wafer_Source				,	0   } ,
	{ "CTC.PM3_Wafer_Source2"		, _K_D_IO	,   PM3_Wafer_Source2				,	0   } ,

	{ "GEM.Port1_Subst1_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst01_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst2_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst02_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst3_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst03_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst4_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst04_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst5_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst05_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst6_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst06_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst7_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst07_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst8_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst08_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst9_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst09_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst10_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst10_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst11_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst11_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst12_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst12_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst13_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst13_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst14_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst14_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst15_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst15_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst16_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst16_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst17_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst17_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst18_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst18_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst19_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst19_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst20_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst20_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst21_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst21_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst22_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst22_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst23_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst23_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst24_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst24_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst25_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst25_PMZoneHistory		,	0   } ,

	{ "GEM.Port2_Subst1_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst01_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst2_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst02_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst3_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst03_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst4_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst04_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst5_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst05_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst6_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst06_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst7_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst07_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst8_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst08_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst9_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst09_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst10_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst10_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst11_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst11_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst12_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst12_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst13_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst13_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst14_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst14_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst15_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst15_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst16_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst16_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst17_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst17_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst18_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst18_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst19_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst19_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst20_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst20_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst21_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst21_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst22_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst22_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst23_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst23_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst24_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst24_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst25_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst25_PMZoneHistory		,	0   } ,

	{ "GEM.Port3_Subst1_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst01_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst2_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst02_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst3_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst03_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst4_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst04_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst5_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst05_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst6_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst06_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst7_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst07_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst8_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst08_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst9_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst09_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst10_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst10_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst11_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst11_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst12_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst12_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst13_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst13_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst14_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst14_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst15_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst15_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst16_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst16_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst17_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst17_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst18_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst18_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst19_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst19_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst20_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst20_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst21_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst21_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst22_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst22_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst23_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst23_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst24_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst24_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst25_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst25_PMZoneHistory		,	0   } ,
	""
};

//////////////////////////////////////////////////////////////////////////
// PM# WaferZoneID Monitoring Function Add start by jelee 2008.10.15
void PM1_Wafer_ZoneID_Monitor() {
	int		nIOStatus;
	int		nPM_PortID[MAX_PM_ZONE], nPM_WaferID[MAX_PM_ZONE];
	int		i;
	int		nSlotIdx = 0;
	int		cnt = 0;
	char	szBuffer[10] = "";
	
	// 1. Read LPM Port & Slot Number by PM Zone
	for(i = 0; i < MAX_PM_ZONE; i++)
	{
		do 
		{
			nPM_PortID[i]	= READ_DIGITAL(PM1_Wafer_Source + i, &nIOStatus);
			nPM_WaferID[i]	= READ_DIGITAL(PM1_Wafer_Status + i, &nIOStatus);

			if (nPM_WaferID[i] != 0)	break;

			Sleep(10);
			cnt++;
		}while(cnt < 200);		
	}
					  
	if(200 <= cnt)printf("===> PM1 Wafer Zone ID Update fail\n");
					  
	for(i = 0; i < MAX_PM_ZONE; i++)
	{
		//... Check Load Port
		if(nPM_WaferID[i] > 0 && nPM_WaferID[i] <= MAX_LPM_SLOT)
		{
			nSlotIdx = nPM_WaferID[i] - 1;
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "PM1|Z%d", i+1);
			
			switch(nPM_PortID[i])
			{
			case EN_LPM_A :
				strcpy(g_sLPMA_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port1_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			case EN_LPM_B :
				strcpy(g_sLPMB_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port2_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			case EN_LPM_C :
				strcpy(g_sLPMC_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port3_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			}
		}
	}
}

void PM2_Wafer_ZoneID_Monitor() {
	int		nIOStatus;
	int		nPM_PortID[MAX_PM_ZONE], nPM_WaferID[MAX_PM_ZONE];
	int		i;
	int		nSlotIdx = 0;
	int		cnt = 0;
	char	szBuffer[10] = "";
	
	// 1. Read LPM Port & Slot Number by PM Zone
	for(i = 0; i < MAX_PM_ZONE; i++)
	{
		do 
		{
			nPM_PortID[i]	= READ_DIGITAL(PM2_Wafer_Source + i, &nIOStatus);
			nPM_WaferID[i]	= READ_DIGITAL(PM2_Wafer_Status + i, &nIOStatus);
			
			if (nPM_WaferID[i] != 0)	break;
			
			Sleep(10);
			cnt++;
		}while(cnt < 200);		
	}
			 
	if(200 <= cnt)printf("===> PM2 Wafer Zone ID Update fail\n");		   
			   
	for(i = 0; i < MAX_PM_ZONE; i++)
	{
		//... Check Load Port
		if(nPM_WaferID[i] > 0 && nPM_WaferID[i] <= MAX_LPM_SLOT)
		{
			nSlotIdx = nPM_WaferID[i] - 1;
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "PM2|Z%d", i+1);
			
			switch(nPM_PortID[i])
			{
			case EN_LPM_A :
				strcpy(g_sLPMA_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port1_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			case EN_LPM_B :
				strcpy(g_sLPMB_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port2_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			case EN_LPM_C :
				strcpy(g_sLPMC_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port3_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			}
		}
	}
}

void PM3_Wafer_ZoneID_Monitor() {
	int		nIOStatus;
	int		nPM_PortID[MAX_PM_ZONE], nPM_WaferID[MAX_PM_ZONE];
	int		i;
	int		nSlotIdx = 0;
	int		cnt = 0;
	char	szBuffer[10] = "";
	
	// 1. Read LPM Port & Slot Number by PM Zone
	for(i = 0; i < MAX_PM_ZONE; i++)
	{
		do 
		{
			nPM_PortID[i]	= READ_DIGITAL(PM3_Wafer_Source + i, &nIOStatus);
			nPM_WaferID[i]	= READ_DIGITAL(PM3_Wafer_Status + i, &nIOStatus);
			
			if (nPM_WaferID[i] != 0)	break;
			
			Sleep(10);
			cnt++;
		}while(cnt < 200);		
	}                 
	
	if(200 <= cnt)printf("===> PM3 Wafer Zone ID Update fail\n");
	
	for(i = 0; i < MAX_PM_ZONE; i++)
	{
		//... Check Load Port
		if(nPM_WaferID[i] > 0 && nPM_WaferID[i] <= MAX_LPM_SLOT)
		{
			nSlotIdx = nPM_WaferID[i] - 1;
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "PM3|Z%d", i+1);
			
			switch(nPM_PortID[i])
			{
			case EN_LPM_A :
				strcpy(g_sLPMA_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port1_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			case EN_LPM_B :
				strcpy(g_sLPMB_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port2_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			case EN_LPM_C :
				strcpy(g_sLPMC_PRJobInfo[nSlotIdx].szWaferZoneID, szBuffer);
				WRITE_STRING(GEM_Port3_Subst01_PMZoneHistory + nSlotIdx, szBuffer, &nIOStatus);
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {

	char *pszEd;
	char szEvent[256], szBuffer1[256], szBuffer2[256], szBuffer3[256], szBuffer4[256], szBuffer5[256], szBuffer6[256];
	char szPortNum[256], szTimeInfo[256], szModuleName[256], szWaferNum[256], szRecipeName[256], szCJName[256], szPJName[256];

	memset(szEvent,			0,  sizeof(szEvent));
	memset(szBuffer1,		0,  sizeof(szBuffer1));
	memset(szBuffer2,		0,	sizeof(szBuffer2));
	memset(szBuffer3,		0,  sizeof(szBuffer3));
	memset(szBuffer4,		0,	sizeof(szBuffer4));
	memset(szBuffer5,		0,  sizeof(szBuffer5));
	memset(szBuffer6,		0,	sizeof(szBuffer6));
	memset(szPortNum,		0,	sizeof(szPortNum));
	memset(szTimeInfo,		0,	sizeof(szTimeInfo));
	memset(szModuleName,	0,	sizeof(szModuleName));
	memset(szWaferNum,		0,	sizeof(szWaferNum));
	memset(szRecipeName,	0,	sizeof(szRecipeName));
	memset(szCJName,		0,	sizeof(szCJName));
    memset(szPJName,		0,	sizeof(szPJName));

	//.....
	pszEd = PROGRAM_PARAMETER_READ();
	STR_SEPERATE( pszEd , szEvent , szBuffer1	, 255 );

	//.....
	if(STRCMP_L(szEvent, "TIME_PROCESS_START")) 
	{
		STR_SEPERATE_CHAR(szBuffer1, '|', szPortNum,	szBuffer2,	255);	
		STR_SEPERATE_CHAR(szBuffer2, '|', szTimeInfo,	szBuffer3,	255);	
		STR_SEPERATE_CHAR(szBuffer3, '|', szModuleName,	szBuffer4,	255);	
		STR_SEPERATE_CHAR(szBuffer4, '|', szWaferNum,	szBuffer5,	255);	
		STR_SEPERATE_CHAR(szBuffer5, '|', szRecipeName,	szBuffer6,	255);	
		STR_SEPERATE_CHAR(szBuffer6, '|', szCJName,		szPJName,	255);

		//...
		if(STRCMP_L(szPortNum, "PORT1")) 
		{
			if(STRCMP_L(szModuleName, "PM1")) {
				if(g_nPM1_EventCount < 1)	{ g_nPM1_EventCount++;	PM1_Wafer_ZoneID_Monitor();	}
			}
			else if( STRCMP_L( szModuleName, "PM2" )) {
				if(g_nPM2_EventCount < 1)	{ g_nPM2_EventCount++;	PM2_Wafer_ZoneID_Monitor(); }
			}
			else if( STRCMP_L( szModuleName, "PM3" )) {
				if(g_nPM3_EventCount < 1)	{ g_nPM3_EventCount++;	PM3_Wafer_ZoneID_Monitor(); }
			}
			else	printf("===> Error : TIME_PROCESS_START Process Module Number is not Valid.\n");
		}
		else if( STRCMP_L( szPortNum, "PORT2" )) 
		{
			if( STRCMP_L( szModuleName, "PM1" )) {
				if(g_nPM1_EventCount < 1)	{ g_nPM1_EventCount++;	PM1_Wafer_ZoneID_Monitor(); }
			}
			else if( STRCMP_L( szModuleName, "PM2" )) {
				if(g_nPM2_EventCount < 1)	{ g_nPM2_EventCount++;	PM2_Wafer_ZoneID_Monitor(); }
			}
			else if( STRCMP_L( szModuleName, "PM3" )) {
				if(g_nPM3_EventCount < 1)	{ g_nPM3_EventCount++;	PM3_Wafer_ZoneID_Monitor(); }
			}
			else	printf("===> Error : TIME_PROCESS_START Process Module Number is not Valid.\n");
		}
		else if( STRCMP_L( szPortNum, "PORT3" )) 
		{
			if( STRCMP_L( szModuleName, "PM1" )) {
				if(g_nPM1_EventCount < 1)	{ g_nPM1_EventCount++;	PM1_Wafer_ZoneID_Monitor(); }
			}
			else if( STRCMP_L( szModuleName, "PM2" )) {
				if(g_nPM2_EventCount < 1)	{ g_nPM2_EventCount++;	PM2_Wafer_ZoneID_Monitor(); }
			}
			else if( STRCMP_L( szModuleName, "PM3" )) {
				if(g_nPM3_EventCount < 1)	{ g_nPM3_EventCount++;	PM3_Wafer_ZoneID_Monitor(); }
			}
			else	printf("===> Error : TIME_PROCESS_START Process Module Number is not Valid.\n");
		}
		else	printf("===> Error : TIME_PROCESS_START Load Port Number is not Valid.\n");

	}
	else if( STRCMP_L( szEvent, "TIME_PROCESS_END" )) 
	{
		STR_SEPERATE_CHAR( szBuffer1, '|', szPortNum, szBuffer2, 255 );	
		STR_SEPERATE_CHAR( szBuffer2, '|', szTimeInfo, szBuffer3, 255 );	
		STR_SEPERATE_CHAR( szBuffer3, '|', szModuleName, szBuffer4, 255 );	
		STR_SEPERATE_CHAR( szBuffer4, '|', szWaferNum, szBuffer5, 255 );	
		STR_SEPERATE_CHAR( szBuffer5, '|', szRecipeName, szBuffer6, 255 );	
		STR_SEPERATE_CHAR( szBuffer6, '|', szCJName, szPJName, 255 );	
		
		if( STRCMP_L( szPortNum, "PORT1" )) {
			if     ( STRCMP_L( szModuleName, "PM1" )) 		g_nPM1_EventCount = 0;
			else if( STRCMP_L( szModuleName, "PM2" )) 		g_nPM2_EventCount = 0;
			else if( STRCMP_L( szModuleName, "PM3" )) 		g_nPM3_EventCount = 0;
			else	printf("===> Error : TIME_PROCESS_END Process Module Number is not Valid.\n");
		}
		else if( STRCMP_L( szPortNum, "PORT2" )) {
			if     ( STRCMP_L( szModuleName, "PM1" ))		g_nPM1_EventCount = 0;
			else if( STRCMP_L( szModuleName, "PM2" ))		g_nPM2_EventCount = 0;
			else if( STRCMP_L( szModuleName, "PM3" ))		g_nPM3_EventCount = 0;
			else	printf("===> Error : TIME_PROCESS_END Process Module Number is not Valid.\n");
		}
		else if( STRCMP_L( szPortNum, "PORT3" )) {
			if     ( STRCMP_L( szModuleName, "PM1" ))		g_nPM1_EventCount = 0;
			else if( STRCMP_L( szModuleName, "PM2" ))		g_nPM2_EventCount = 0;
			else if( STRCMP_L( szModuleName, "PM3" ))		g_nPM3_EventCount = 0;
			else	printf("===> Error : TIME_PROCESS_END Process Module Number is not Valid.\n");
		}
		else	printf("===> Error : TIME_PROCESS_END Load Port Number is not Valid.\n");
	}
	else if( STRCMP_L( szEvent, "START_F" )) 
	{
		int i;
		STR_SEPERATE_CHAR( szBuffer1, '|', szPortNum, szBuffer2, 255 );

		if( STRCMP_L(szPortNum, "PORT1")) {
			for( i=0; i<25; i++ )	{ strcpy(g_sLPMA_PRJobInfo[i].szWaferZoneID,""); }
		}
		else if( STRCMP_L(szPortNum, "PORT2")) {
			for( i=0; i<25; i++ )	{ strcpy(g_sLPMB_PRJobInfo[i].szWaferZoneID,""); }
		}
		else if( STRCMP_L(szPortNum, "PORT3")) {
			for( i=0; i<25; i++ )	{ strcpy(g_sLPMC_PRJobInfo[i].szWaferZoneID,""); }
		}
		else	printf("===> Error : START_F PortID is not Valid.\n");
	}
	else if	( STRCMP_L( szEvent  , "CARR_DELETED_F" ) ) 
	{
		int i, nCommStatus;
		STR_SEPERATE_CHAR( szBuffer1, '|', szPortNum, szBuffer2, 255 );

		if( STRCMP_L(szPortNum, "PORT1")) {
			for( i=0; i<25; i++ )	{ WRITE_STRING(GEM_Port1_Subst01_PMZoneHistory+i, "", &nCommStatus); }
			return SYS_SUCCESS;
		}
		else if( STRCMP_L(szPortNum, "PORT2")) {
			for( i=0; i<25; i++ )	{ WRITE_STRING(GEM_Port2_Subst01_PMZoneHistory+i, "", &nCommStatus); }
			return SYS_SUCCESS;
		}
		else if( STRCMP_L(szPortNum, "PORT3")) {
			for( i=0; i<25; i++ )	{ WRITE_STRING(GEM_Port3_Subst01_PMZoneHistory+i, "", &nCommStatus); }
			return SYS_SUCCESS;
		}
		else	printf("===> Error : CARR_DELETED_F PortID is not Valid.\n");
	}
	else	printf("===> Error : GEM_WaferZoneID_Monitor is not Valid.\n");
	
	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
BOOL Program_Init_Code() {
	int i;
	
	g_nPM1_EventCount=0;
	g_nPM2_EventCount=0;
	g_nPM3_EventCount=0;

	for( i=0; i<25; i++ ) 
	{
		strcpy(g_sLPMA_PRJobInfo[i].szWaferZoneID,"");
		strcpy(g_sLPMB_PRJobInfo[i].szWaferZoneID,"");
		strcpy(g_sLPMC_PRJobInfo[i].szWaferZoneID,"");
	}

	return TRUE;
}
