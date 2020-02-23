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
#include "dyn_iodefine.h"

#define APP_NAME	"ValveControl"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define MAX_DYN_IO		150

typedef struct _IO_LIST
{
	char szIOName[40];
	int nIOType;
	int nIOPoint;
} IO_LIST;

extern IO_LIST	g_IOList[MAX_DYN_IO];
extern int g_nIOCount;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Alarm Definition(2500 ~ 2599)

//------------------------------------------------------------------------------------------
// Enumeration  Definition

enum {VALVE_CLOSE, VALVE_OPEN}; 

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_GbIntlckDM",			_K_D_IO	,	SW_GbIntlckDM,		0	} ,
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
// Initialize Function
#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\RecipeConfigMap.ini"

void ParseValveRecipe(char *szData)
{
	char szValveNo[40] = "";
	char szValveName[40] = "";
	int nAddr, nBitNo;
	int nIdx;
	BOOL bEnable = TRUE;

	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szValveNo))		break;
		if(! tp.GetInt(&nAddr))			break;
		if(! tp.GetInt(&nBitNo))		break;
		if(! tp.GetWord(szValveName))	break;

		nIdx = atoi(szValveNo+5) - 1;
		strcpy(g_IOList[_ValveIdx+nIdx].szIOName, szValveName);
		g_IOList[_ValveIdx+nIdx].nIOPoint = 0;
	} while(0);
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

				if(strcmp("ValveRecipe", szItem) == 0)			nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseValveRecipe(szRead); break;
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

//------------------------------------------------------------------------------------------
void InitValveIO()
{
	int i;
	for(i=_ValveIdx; i<MAX_DYN_IO; i++)
	{
		g_IOList[i].nIOPoint = -1;
		g_IOList[i].nIOType = _K_D_IO;
		if(i<MAX_DYN_IO-1) strcpy(g_IOList[i].szIOName, "_ValveIdx");
		else g_IOList[i].szIOName[0] = 0;
	}
}

BOOL Initialize()
{
	BOOL bRet = FALSE;
	do {
		InitValveIO();
		ReadPmcConfig();
		bRet = TRUE;
	} while(0);
	return bRet;
}

//------------------------------------------------------------------------------------------

Module_Status OpenValve(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	BOOL bIOStatus;
	int nValve;
	int nIO;
	char szValve[40];

	nValve = atoi(szParam);
	if(nValve == 0) return SYS_ABORTED;
	sprintf(szValve, "V%dValveDO", nValve);
	nIO = dyn_FindIO(szValve);
	if(nIO < 0)  return SYS_ABORTED;
	dyn_WRITE_DIGITAL(nIO, VALVE_OPEN, &bIOStatus);
	return msRet;
}

Module_Status CloseValve(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	BOOL bIOStatus;
	int nValve;
	int nIO;
	char szValve[40];
	
	nValve = atoi(szParam);
	if(nValve == 0) return SYS_ABORTED;
	sprintf(szValve, "V%dValveDO", nValve);
	nIO = dyn_FindIO(szValve);
	if(nIO < 0)  return SYS_ABORTED;
	dyn_WRITE_DIGITAL(nIO, VALVE_CLOSE, &bIOStatus);
	return msRet;
}

Module_Status SetValveArray(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	BOOL bIOStatus;
	int nLength;
	int i;

	nLength = strlen(szParam);
	if(nLength <= 0) return SYS_ABORTED;
	for(i=0; i<nLength; i++)
	{
		if(*(szParam+i) == '1')
			dyn_WRITE_DIGITAL(_ValveIdx+i, VALVE_OPEN, &bIOStatus);
		else if(*(szParam+i) == '0' || *(szParam+i) == '2')
			dyn_WRITE_DIGITAL(_ValveIdx+i, VALVE_CLOSE, &bIOStatus);
	}
	return msRet;
}

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_ABORTED;
	int nParLen = 0;
	char *pParam;
	char szCmd[40], szSubParam[256];

	printf("Entered %s\n", APP_NAME);

	pParam = PROGRAM_PARAMETER_READ();
	nParLen = strlen(pParam);

	STR_SEPERATE(pParam , szCmd, szSubParam, 255 );

		 if(STRCMP_L(szCmd, "OPEN_VALVE"))		msRet = OpenValve(szSubParam);
	else if(STRCMP_L(szCmd, "CLOSE_VALVE"))		msRet = CloseValve(szSubParam);
	else if(STRCMP_L(szCmd, "SET_VALVE_ARRAY"))	msRet = SetValveArray(szSubParam);
	else 
	{
		printf("---> Invalid Command in %s !\n", APP_NAME);
		msRet = SYS_ABORTED;
	}

	printf("Leaved %s[%d]\n", APP_NAME, msRet);

	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL Program_Init_Code() {
	Initialize();
	dyn_InitIOList();
	return TRUE;
}

void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
