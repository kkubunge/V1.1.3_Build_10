// TMSchStopCtrl.cpp: implementation of the CTMSchStopCtrl class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

//--------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//--------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextParser.h"
#include "AutoPM.h"

//--------------------------------------------------------------------
enum {
	PMC_Unuse, PMC_Local, PMC_DisCon,  PMC_Standby, PMC_Maint,
	PMC_Uninit, PMC_EnablePM, PMC_CTCInUse, PMC_CTCDis, PMC_CTCDis2,
	PMC_CTCEnaPM, PMC_CTCDisHW, PMC_CTCDisHW2
};

enum {C_HOST,C_OPERATOR};

enum {TRAN, ATM, VACCUM, S_ERROR};
enum { UNKNOWN_STS, R_OPEN, R_CLOSE, R_ERROR};	
enum {_IDLE = 0 , _RUNNING , _PAUSING , _PAUSED , _ABORTING , _DISABLE , _WAITING , _WAIT_HAND_OFF};
enum { IN_UNLOCK, IN_LOCK };
enum { NOT_CHK, CHECK };
enum { P_NOTSAVE, P_SAVE };
enum { PHYNONE,	PHY_START, PHY_END };
enum { PMNONE,	PMRESV,	 PMRUN,	PMEND };
enum { TOPLID_OPEN, TOPLID_CLOSE};
enum { RESUMED_OFF, RESUMED_ON};
char *EVENT_STRING_PMJOB_CREATED ="EVENT_ON %s_PMJOB_CREATED";
char *EVENT_STRING_PMJOB_START ="EVENT_ON %s_PMJOB_STARTED";

char *EVENT_STRING_PRE_MACRO_START ="EVENT_ON %s_PRE_MACRO_STARTED";
char *EVENT_STRING_PRE_MACRO_END ="EVENT_ON %s_PRE_MACRO_ENDED";
char *EVENT_STRING_PRE_MACRO_STOP ="EVENT_ON %s_PRE_MACRO_STOPED";

char *EVENT_STRING_PRE_RECIPE_START ="EVENT_ON %s_PREPM_RECIPE_STARTED";
char *EVENT_STRING_PRE_RECIPE_END ="EVENT_ON %s_PREPM_RECIPE_ENDED";

char *EVENT_STRING_PHYSICAL_PM_START ="EVENT_ON %s_PHYSICAL_PM_STARTED";
char *EVENT_STRING_PHYSICAL_PM_END ="EVENT_ON %s_PHYSICAL_PM_ENDED";

char *EVENT_STRING_POST_MACRO_START ="EVENT_ON %s_POST_MACRO_STARTED";
char *EVENT_STRING_POST_MACRO_END ="EVENT_ON %s_POST_MACRO_ENDED";
char *EVENT_STRING_POST_MACRO_STOP ="EVENT_ON %s_POST_MACRO_STOPED";

char *EVENT_STRING_POST_RECIPE_START ="EVENT_ON %s_POSTPM_RECIPE_STARTED";
char *EVENT_STRING_POST_RECIPE_END ="EVENT_ON %s_POSTPM_RECIPE_ENDED";

char *EVENT_STRING_HI_TEACHING_START ="EVENT_ON %s_HI_TEACHING_STARTED";
char *EVENT_STRING_HI_TEACHING_END ="EVENT_ON %s_HI_TEACHING_ENDED";

char *EVENT_STRING_PMJOB_END ="EVENT_ON %s_PMJOB_COMPLETED";

char *EVENT_STRING_PRE_PM_RESUME ="EVENT_ON %s_PRE_PM_RESUMED";
char *EVENT_STRING_POST_PM_RESUME ="EVENT_ON %s_POST_PM_RESUMED";

char *EVENT_STRING_PRE_PM_ABORT ="EVENT_ON %s_PRE_PM_ABORTED";
char *EVENT_STRING_POST_PM_ABORT ="EVENT_ON %s_POST_PM_ABORTED";

char *EVENT_STRING_SOMEJOB_START ="EVENT_ON %s_SOMEJOB_START";
char *EVENT_STRING_SOMEJOB_END ="EVENT_ON %s_SOMEJOB_END";

#define PM1_PreMacro_Stop 21
#define PM2_PreMacro_Stop 22
#define PM3_PreMacro_Stop 23
#define PM4_PreMacro_Stop 24
#define PM5_PreMacro_Stop 25

#define PM1_PostMacro_Stop 31
#define PM2_PostMacro_Stop 32
#define PM3_PostMacro_Stop 33
#define PM4_PostMacro_Stop 34
#define PM5_PostMacro_Stop 35

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static enum {
	Fixed_IO_End,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	""
};

CAutoPM::CAutoPM()
{
	strcpy(m_szFilePath,CONFIG_FILE_NAME_PM1);
	m_nIOCount = 0;
	for(int i = 0 ; i < MAX_IO_LIST ; i++)
	{
		m_IOList[i] = new PROFILE_LIST;
	}
	m_nAlarmIdx = 0;
	m_nMacroID = 0;
	m_nTimeCount = 0;
	memset(m_szCmd,0,sizeof(m_szCmd));
}

CAutoPM::~CAutoPM()
{
	for(int i = 0 ; i < MAX_IO_LIST ; i++)
	{
		PROFILE_LIST* pIOListPM1 = m_IOList[i];

		if(NULL != pIOListPM1)
		{
			delete pIOListPM1;
			pIOListPM1 = NULL;		
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Member Function
void CAutoPM::AutoPMEventMon(char *szCmd,char *szMacroType)
{
	int nCommStatus;
	CTextParser tp(szCmd);

	if(strcmp(szCmd,"PMJobCreate") == 0){
		m_nMacroID = Get_MacroType(szMacroType);
		m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD = FALSE;
		m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD = FALSE;
		Report_Event(PMJOB_CREAT);
	}
	if(strcmp(szCmd,"PMJobStop") == 0){
		m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD = TRUE;
		WRITE_DIGITAL(AutoPMOwner,C_HOST,&nCommStatus);
	}
	if(strcmp(szCmd,"PMJobAbort") == 0){
		m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD = TRUE;
		WRITE_DIGITAL(AutoPMOwner,C_HOST,&nCommStatus);		
	}
}

int CAutoPM::Get_MacroType(char *pMacroType)
{
	char szMacroType[256] = {0,};
	char szMacroTemp[256] = {0,};
	int nMacroType = 0;
	int nCnt = 0,nCnt2 = 0;
	int nIOSts;

	memset(szMacroType			,		0	,	sizeof(szMacroType)	);
	memset(szMacroTemp			,		0	,	sizeof(szMacroTemp)	);

	strcpy(szMacroTemp,pMacroType);
/*
	while(szMacroTemp[nCnt] !='\0')
	{
		if(szMacroTemp[nCnt] >='0' && szMacroTemp[nCnt] <='9'){
			szMacroType[nCnt2++] = szMacroTemp[nCnt];
		}
		nCnt++;
	}
	szMacroType[nCnt2] = '\0';
	nMacroType = atoi(szMacroType);
*/
	WRITE_STRING(AutoPMMacroName,pMacroType,&nIOSts);
	for(nCnt = 0; nCnt < 2; nCnt++){
		READ_STRING(MacroName_01+nCnt,szMacroType,&nIOSts);
		if(STRCMP_L(pMacroType,szMacroType)) nMacroType = nCnt + 1;
	}
	return nMacroType;
}
void CAutoPM::ParseAutoPMConfig2(char *szData)
{
	CTextParser txtP;

	txtP.SetText(szData);
	do {
		if(! txtP.GetWord()) break;
		if(txtP.BeEqual("AutoPMUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bAutoPMUse); break; }
		if(txtP.BeEqual("PreMacroUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPreMacroUse); break; }
		if(txtP.BeEqual("PostMacroUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPostMacroUse); break; }
		if(txtP.BeEqual("PostMacroAutoStart")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPostMacroAutoStart); break; }
		if(txtP.BeEqual("PreRecipeFirst")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPreRecFstUse); break; }
		if(txtP.BeEqual("PostRecipeFirst")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPostRecFstUse); break; }
		if(txtP.BeEqual("PreRecipeSecond")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPreRecSndUse); break; }
		if(txtP.BeEqual("PostRecipeSecond")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPostRecSndUse); break; }
		if(txtP.BeEqual("PhysicalPMUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPhysicalPMUse); break; }
		if(txtP.BeEqual("PLCIntUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPLCIntUse); break; }
		if(txtP.BeEqual("AutoVentUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bAutoVentUse); break; }
		if(txtP.BeEqual("PrePMSucWhenAbort")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPrePMSuccess); break; }
		if(txtP.BeEqual("PostPMSucWhenAbort")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bPostPMSuccess); break; }
		if(txtP.BeEqual("DryCntResetUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bDryCntResetUse); break; }
		if(txtP.BeEqual("WetCntResetUse")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bWetCntResetUse); break; }
		if(txtP.BeEqual("AutoEnableAfrPMJob")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].bAutoEnable); break; }
		if(txtP.BeEqual("AutoPMStartDelayTime")) { txtP.GetInt('=', &m_AutoPMCfgFile2[m_nModuleIndex].nStartDelayTime); break; }
		_LOG("-----> [PM%d] Invalid DryClean Config [%s]", m_nModuleIndex, szData);
	} while(0);
}

void CAutoPM::ParseAutoPMConfig(char *szData)
{
	CTextParser txtP;

	txtP.SetText(szData);
	do {
		if(! txtP.GetWord()) break;
		if(txtP.BeEqual("AutoPMUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bAutoPMUse); break; }
		if(txtP.BeEqual("PreMacroUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPreMacroUse); break; }
		if(txtP.BeEqual("PostMacroUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPostMacroUse); break; }
		if(txtP.BeEqual("PostMacroAutoStart")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPostMacroAutoStart); break; }
		if(txtP.BeEqual("PreRecipeFirst")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPreRecFstUse); break; }
		if(txtP.BeEqual("PostRecipeFirst")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPostRecFstUse); break; }
		if(txtP.BeEqual("PreRecipeSecond")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPreRecSndUse); break; }
		if(txtP.BeEqual("PostRecipeSecond")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPostRecSndUse); break; }
		if(txtP.BeEqual("PhysicalPMUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPhysicalPMUse); break; }
		if(txtP.BeEqual("PLCIntUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPLCIntUse); break; }
		if(txtP.BeEqual("AutoVentUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bAutoVentUse); break; }
		if(txtP.BeEqual("PrePMSucWhenAbort")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPrePMSuccess); break; }
		if(txtP.BeEqual("PostPMSucWhenAbort")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bPostPMSuccess); break; }
		if(txtP.BeEqual("DryCntResetUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bDryCntResetUse); break; }
		if(txtP.BeEqual("WetCntResetUse")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bWetCntResetUse); break; }
		if(txtP.BeEqual("AutoEnableAfrPMJob")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].bAutoEnable); break; }
		if(txtP.BeEqual("AutoPMStartDelayTime")) { txtP.GetInt('=', &m_AutoPMCfgFile[m_nModuleIndex].nStartDelayTime); break; }
		_LOG("-----> [PM%d] Invalid WetPM Config [%s]", m_nModuleIndex, szData);
	} while(0);
}

BOOL CAutoPM::Initialize(const char *szModuleName)
{	
	BOOL bRet = TRUE;
	FILE *fp;
	char szRead    [256] = {0,};
	char szItem    [256] = {0,};
	int nTitle = 0;

	if(szModuleName != NULL)
	{
		strcpy(m_szModuleName, szModuleName);
	}
	ReplaceIONameChar(g_pIOList, '#', m_szModuleName[2]);

	if(strcmp(m_szModuleName,"PM1")==0){
		strcpy(m_szFilePath,CONFIG_FILE_NAME_PM1);
		m_nModuleIndex = 0;
		m_nAlarmIdx = 0;
	}else if(strcmp(m_szModuleName,"PM2")==0){
		strcpy(m_szFilePath,CONFIG_FILE_NAME_PM2);
		m_nModuleIndex = 1;
		m_nAlarmIdx = 1;
	}else if(strcmp(m_szModuleName,"PM3")==0){
		strcpy(m_szFilePath,CONFIG_FILE_NAME_PM3);
		m_nModuleIndex = 2;
		m_nAlarmIdx = 2;
	}else if(strcmp(m_szModuleName,"PM4")==0){
		strcpy(m_szFilePath,CONFIG_FILE_NAME_PM4);
		m_nModuleIndex = 3;
		m_nAlarmIdx = 3;
	}else if(strcmp(m_szModuleName,"PM5")==0){
		strcpy(m_szFilePath,CONFIG_FILE_NAME_PM5);
		m_nModuleIndex = 4;
		m_nAlarmIdx = 4;
	}
		
	InitIOList(g_pIOList, true, Fixed_IO_End);
	do{
		fp = fopen(m_szFilePath,"rt");
		if(fp == NULL)
		{
			printf("file %s open fail \n" , m_szFilePath);
			break;
		}
		if(NULL == fgets(szRead,255,fp))
		{
			printf("file %s fgets fail \n" , m_szFilePath);
			break;
		}

		do{
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');
				
				if(strcmp("WetPM",szItem) == 0) nTitle = 1;
				else if(strcmp("DryClean",szItem) == 0) nTitle = 2;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
						case 1: ParseAutoPMConfig(szRead); break;
						case 2: ParseAutoPMConfig2(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;

		}while(feof(fp) == 0);

	}while(0);
	CreateThread();
	return bRet;

}

Module_Status CAutoPM::Main()
{
	Module_Status msRet = SYS_SUCCESS;
	char szParam[80] = {0,}, szCmd[40] = {0,};
	int nValue;
	CTextParser tp;

	strcpy(szParam, PROGRAM_PARAMETER_READ());
	tp.SetText(szParam);
	strcpy(m_szCmd, szParam);
	_LOG("-----> Manual Control Started[%s]", szParam);
	do {
		tp.GetWord(szCmd);
		tp.GetInt(&nValue);
		
		if(tp.BeEqual(szCmd, "PREMACRO_START")){
			//AutoPM_FlwSts(PREMACRO_S);
			Manual_START(PREMACRO_S);
		}else if(tp.BeEqual(szCmd, "PREMACRO_END")){
			//msRet = Manual_END(PREMACRO_ABORT);
			Manual_END(PREMACRO_ABORT);
		}else if(tp.BeEqual(szCmd, "POSTMACRO_START")){
			//AutoPM_FlwSts(POSTMACRO_S);
			Manual_START(POSTMACRO_S);
		}else if(tp.BeEqual(szCmd, "POSTMACRO_END")){
			//msRet = Manual_END(POSTMACRO_ABORT);
			Manual_END(POSTMACRO_ABORT);
		}else if(tp.BeEqual(szCmd, "PHYSICAL_START")){
			msRet = PhysicalPM(PHYSICAL_S);
		}else if(tp.BeEqual(szCmd, "PHYSICAL_END")){
			msRet = PhysicalPM(PHYSICAL_E);
		}else if(tp.BeEqual(szCmd, "Manaul_Abort")){
			msRet = Manaul_Abort();
		}
	}while(0);

	if(SYS_SUCCESS == msRet) _LOG("-----> Manual Control Success[%s]", szParam);
	else _LOG("-----> Manual Control Failed ![%s]", szParam);
	return msRet;
}
void CAutoPM::AutoPM_FlwSts(int nFlwSts)
{
	int nCommStatus;

	WRITE_DIGITAL(PRMD_AutoPMFlw,nFlwSts,&nCommStatus);

}
void CAutoPM::Report_Event(int nEventID)
{
	Module_Status msRet = SYS_SUCCESS;	
	char szCmd[256] = {0,};
	int nCommStatus;
	int nAutoPMSts;
	int nPreMacroSts;
	int nPostMacroSts;
	int nMacroID;
	char szPrePMRecipe[256] = {0,};
	char szPostPMRecipe[256] = {0,};
	char szEventTemp[256] = {0,};
	char szEventBfr[256] = {0,};

	nAutoPMSts = READ_DIGITAL(PRMD_AutoPMFlw,&nCommStatus);
	nPreMacroSts = READ_DIGITAL(PRMD_PreMacroFlw,&nCommStatus);
	nPostMacroSts = READ_DIGITAL(PRMD_PostMacroFlw,&nCommStatus);

	/*PRE MACRO*/
	switch(nEventID)
	{
	case PMJOB_CREAT:
		WRITE_DIGITAL(AutoPMOwner,C_HOST,&nCommStatus);
		sprintf(szCmd , EVENT_STRING_PMJOB_CREATED , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(PREMACRO_S); 
		WRITE_DIGITAL(PRO_DryClnStsDM,PMRESV,&nCommStatus);
		WRITE_DIGITAL(PRMD_PreMacroFlw,POSTNONE,&nCommStatus);
		WRITE_DIGITAL(PRMD_PostMacroFlw,POSTNONE,&nCommStatus);
		WRITE_DIGITAL(ReserveDI,PMJOB_REV,&nCommStatus);
		break;
	case PREMACRO_ABORT:
		sprintf(szCmd , EVENT_STRING_PRE_PM_ABORT , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(PREMACRO_E);
		RUN_SET_FUNCTION(ABORT_PROCESS,"");
		if(m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD == TRUE
			||m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD == TRUE){
			AutoPM_FlwSts(UNKNOWN_TYPE);
			WRITE_DIGITAL(ReserveDI,NONE_REV,&nCommStatus);
			WRITE_DIGITAL(PMJobStart,NONE_REV,&nCommStatus);
			WRITE_DIGITAL(PRO_DryClnStsDM,PMNONE,&nCommStatus);
		}
		WRITE_DIGITAL(PRMD_PreMacroFlw,PRENONE,&nCommStatus);
		break;
	case POSTMACRO_ABORT:
		sprintf(szCmd , EVENT_STRING_POST_PM_ABORT , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(UNKNOWN_TYPE);
		RUN_SET_FUNCTION(ABORT_PROCESS,"");
		WRITE_DIGITAL(ReserveDI,NONE_REV,&nCommStatus);
		WRITE_DIGITAL(PMJobStart,NONE_REV,&nCommStatus);
		WRITE_DIGITAL(PRO_DryClnStsDM,PMNONE,&nCommStatus);
		WRITE_DIGITAL(PRMD_PostMacroFlw,POSTNONE,&nCommStatus);
		break;
	case PMJOB_START:
		sprintf(szCmd , EVENT_STRING_PMJOB_START , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		break;
	case PREMACRO_S:
		sprintf(szCmd , EVENT_STRING_PRE_MACRO_START , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(PREMACRO_S);
		WRITE_DIGITAL(PRO_DryClnStsDM,PMRUN,&nCommStatus);
		break;
	case PREMACRO_REC_S:
		nMacroID = m_nMacroID - 1;
		READ_STRING(PRMS_PreMRcp_01+nMacroID,szPrePMRecipe,&nCommStatus);
		WRITE_STRING (AutoPMRcpName, szPrePMRecipe, &nCommStatus);
		sprintf(szCmd , EVENT_STRING_PRE_RECIPE_START , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		WRITE_DIGITAL(PRO_DryClnStsDM,PMRUN,&nCommStatus);
		break;
	case PREMACRO_REC_E:
		sprintf(szCmd , EVENT_STRING_PRE_RECIPE_END , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		break;
	case PREMACRO_E:
		sprintf(szCmd , EVENT_STRING_PRE_MACRO_END , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(PREMACRO_E);
		WRITE_DIGITAL(PRMD_PreMacroFlw,PRENONE,&nCommStatus);
		break;
	/*PHYSICAL MACRO*/
	case PHYSICAL_S:
		WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nCommStatus);
		sprintf(szCmd , EVENT_STRING_PHYSICAL_PM_START , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(PHYSICAL_S);
		break;
	case PHYSICAL_E:
		WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nCommStatus);
		sprintf(szCmd , EVENT_STRING_PHYSICAL_PM_END , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(PHYSICAL_E);
		break;
	/*POST MACRO*/
	case POSTMACRO_S:
		//WRITE_DIGITAL(AutoPMOwner,C_HOST,&nCommStatus);
		sprintf(szCmd , EVENT_STRING_POST_MACRO_START , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		WRITE_DIGITAL(PRO_DryClnStsDM,PMRUN,&nCommStatus);
		AutoPM_FlwSts(POSTMACRO_S);
		break;
	case POSTMACRO_REC_S:
		nMacroID = m_nMacroID - 1;
		READ_STRING(PRMS_PostMRcp_01+nMacroID,szPostPMRecipe,&nCommStatus);
		WRITE_STRING (AutoPMRcpName, szPostPMRecipe, &nCommStatus);
		sprintf(szCmd , EVENT_STRING_POST_RECIPE_START , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		WRITE_DIGITAL(PRO_DryClnStsDM,PMRUN,&nCommStatus);
		break;
	case POSTMACRO_REC_E:
		sprintf(szCmd , EVENT_STRING_POST_RECIPE_END , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		break;
	case POSTMACRO_E:
		sprintf(szCmd , EVENT_STRING_POST_MACRO_END , m_szModuleName);
		WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		WRITE_STRING (AutoPMEventName, szCmd+9 , &nCommStatus);
		AutoPM_FlwSts(POSTMACRO_E);
		WRITE_DIGITAL(PRO_DryClnStsDM,PMEND,&nCommStatus);
		WRITE_DIGITAL(PRMD_PostMacroFlw,POSTNONE,&nCommStatus);
		break;
	case PMJOB_E:
		sprintf(szCmd , EVENT_STRING_PMJOB_END , m_szModuleName);
		READ_STRING(AutoPMEventName,szEventBfr,&nCommStatus);
		sprintf(szEventTemp,"%s_PRE_PM_ABORTED",m_szModuleName);
		if(!STRCMP_L(szEventBfr,szEventTemp)){
			WRITE_STRING (SCHEDULER , szCmd , &nCommStatus);
		}
		//... SSP 끝나도 메인화면에 Message 남아 있어서 Clear함
		WRITE_STRING (AutoPMEventName, "" , &nCommStatus);
		AutoPM_FlwSts(UNKNOWN_TYPE);
		WRITE_DIGITAL(PRO_DryClnStsDM,PMNONE,&nCommStatus);
		WRITE_DIGITAL(ReserveDI,NONE_REV,&nCommStatus);
		WRITE_DIGITAL(PMJobStart,NONE_REV,&nCommStatus);
		m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD = FALSE;
		m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD = FALSE;
		break;
	default:
		break;
	}
}

Module_Status CAutoPM::PLCIntOperation(int nRecID)
{
	Module_Status msRet = SYS_ABORTED;
	int nIOStatus;
	int nPLCInkUnlockSts;  

    if(m_bSimulMode) return SYS_SUCCESS;
	do{ 
		if(nRecID == IN_UNLOCK){
			WRITE_DIGITAL(PLCInkUnlockStsDO,1,&nIOStatus);
			if(!nIOStatus) break;
			if(!WAIT_SECONDS(2))break;
			nPLCInkUnlockSts = READ_DIGITAL(PLCInkUnlockStsDI,&nIOStatus);
			if(!nIOStatus) break;
			if(nPLCInkUnlockSts == 1) msRet = SYS_SUCCESS;
			if(!WAIT_SECONDS(1)){
				msRet = SYS_ABORTED;
				break;
			}
			WRITE_DIGITAL(PLCInkUnlockStsDO,0,&nIOStatus);
		}else if(nRecID == IN_LOCK){
			WRITE_DIGITAL(PLCInkRvrStsDO,1,&nIOStatus);
			if(!nIOStatus) break;
			if(!WAIT_SECONDS(2))break;
			msRet = SYS_SUCCESS;
			if(!WAIT_SECONDS(1)){
				msRet = SYS_ABORTED;
				break;
			}
			WRITE_DIGITAL(PLCInkRvrStsDO,0,&nIOStatus);
		}
	}while(0);
	return	msRet;
}

Module_Status CAutoPM::PhysicalPM(int nEventID)
{
	Module_Status msRet = SYS_ABORTED;
	int nChmStsDI;
	double dbChmPrsAI;
	double dbChmVPra;
	double dbChmAPra;
	int nIOSts;

	_LOG("[%s]Physical PM Start ",m_szModuleName);
	nChmStsDI = READ_DIGITAL(ChmPrssDI,&nIOSts);
	dbChmPrsAI = GetChamberPressure();
	dbChmVPra = READ_ANALOG(PRMA_VS_PrsBaRough,&nIOSts);
	dbChmAPra = READ_ANALOG(PRMA_VS_PrsBaVent,&nIOSts);
	if(nEventID == PHYSICAL_S){
		if(nChmStsDI != ATM || dbChmPrsAI < dbChmAPra){
			PopupMessage("Physical PM Start Fail","Check Chamber Status[Change to ATM]");
			_LOG("Physical PM Start Fail ChmSts[%d] ChmPrs[%f]",nChmStsDI,dbChmPrsAI);
		}else{
			Report_Event(PHYSICAL_S);
			_LOG("Physical PM Start Success");
			msRet = SYS_SUCCESS;
		}
	}else if(nEventID == PHYSICAL_E){
		if(nChmStsDI != VACCUM || dbChmPrsAI > dbChmVPra){
			PopupMessage("Physical PM End Fail","Check Chamber Status[Change to VTM]");
			_LOG("Physical PM End Fail ChmSts[%d] ChmPrs[%f]",nChmStsDI,dbChmPrsAI);
		}else{
			Report_Event(PHYSICAL_E);
			_LOG("Physical PM End Success");
			msRet = SYS_SUCCESS;
		}
	}
	return msRet;
}

Module_Status CAutoPM::PMMonitor(int nAutoPMType)
{
	Module_Status msRet = SYS_ABORTED;
	int nIOSts;
	int nPMComSts;
	int nWaferSts;
	int nStartDelayTime;
	int nAutoPMOwner;
	
	if(nAutoPMType == WETPM){
		nStartDelayTime = m_AutoPMCfgFile[m_nModuleIndex].nStartDelayTime * 12;
	}else if(nAutoPMType == DRYCLEAN){
		nStartDelayTime = m_AutoPMCfgFile2[m_nModuleIndex].nStartDelayTime * 12;
	}
	nPMComSts = READ_DIGITAL(PM_CommStatus,&nIOSts);
	nWaferSts = READ_DIGITAL(WaferDI,&nIOSts);
	nAutoPMOwner = READ_DIGITAL(AutoPMOwner,&nIOSts);
	do{
		if((nPMComSts == PMC_Standby && ABSENT == nWaferSts)
			||(nAutoPMOwner == C_OPERATOR && ABSENT == nWaferSts)){
			m_nTimeCount++;
			_LOG("[%s] PMMonitor Wait for standby Time[%d]",m_szModuleName, m_nTimeCount);
		}else{
			m_nTimeCount = 0;
			_LOG("[%s] PMMonitor Wait for standby & No Wafer Status",m_szModuleName);
			break;
		}
		
		if(nStartDelayTime < m_nTimeCount){
			_LOG("[%s] PMMonitor Wait Success",m_szModuleName);
			WRITE_DIGITAL(PMJobStart,PMJOB_REV,&nIOSts);
			WRITE_DIGITAL(PRMD_PreMEn,m_nMacroID,&nIOSts);
			WRITE_DIGITAL(PRMD_PostMEn,m_nMacroID,&nIOSts);
			m_nTimeCount = 0;
			msRet = SYS_SUCCESS;
		}

		if(m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD == TRUE
			||m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD == TRUE){
			_LOG("[%s] PMMonitor Abort Start",m_szModuleName);
			AutoPM_FlwSts(UNKNOWN_TYPE);
			WRITE_DIGITAL(ReserveDI,NONE_REV,&nIOSts);
			WRITE_DIGITAL(PMJobStart,NONE_REV,&nIOSts);
			WRITE_DIGITAL(PRO_DryClnStsDM,PMNONE,&nIOSts);
			m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD = FALSE;
			m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD = FALSE;
		}
	}while(0);
	return msRet;
}

Module_Status CAutoPM::PreMacro(int nValue)
{
	Module_Status msRet = SYS_SUCCESS;
	char szCmd[256] = {0,};
	int nIOStatus;
	BOOL bPreMacroEnd = FALSE;
	int nMacroID = 0;
	int nPLCIntOpSts = 0;
	int nIOIndex = 0;
	int nPM_CommSts;
	char szEvent[256] = {0,};
	char szPreMacroRecipe[256] = {0,};
	char szPreMacroRecipe2[256] = {0,};
	int nPreMacroSts;
	int nPreRecFstUse;
	int nPreRecSndUse;
	int nDryCntResetUse;
	int nPLCIntUse;
	int nAutoVentUse;
	int nAutoLeakCheckUse;

	memset(szPreMacroRecipe,0,sizeof(szPreMacroRecipe));
	memset(szPreMacroRecipe2,0,sizeof(szPreMacroRecipe2));
	/*
	if(nValue == WETPM){
		bPreRecFstUse = m_AutoPMCfgFile[m_nModuleIndex].bPreRecFstUse;
		bPreRecSndUse = m_AutoPMCfgFile[m_nModuleIndex].bPreRecSndUse;
		bDryCntResetUse = m_AutoPMCfgFile[m_nModuleIndex].bDryCntResetUse;
		bPLCIntUse = m_AutoPMCfgFile[m_nModuleIndex].bPLCIntUse;
		bAutoVentUse = m_AutoPMCfgFile[m_nModuleIndex].bAutoVentUse;
	}
	else if(nValue == DRYCLEAN){
		bPreRecFstUse = m_AutoPMCfgFile2[m_nModuleIndex].bPreRecFstUse;
		bPreRecSndUse = m_AutoPMCfgFile2[m_nModuleIndex].bPreRecSndUse;
		bDryCntResetUse = m_AutoPMCfgFile2[m_nModuleIndex].bDryCntResetUse;
		bPLCIntUse = m_AutoPMCfgFile2[m_nModuleIndex].bPLCIntUse;
		bAutoVentUse = m_AutoPMCfgFile2[m_nModuleIndex].bAutoVentUse;
	}	
	*/
	nPreMacroSts = READ_DIGITAL(PRMD_PreMacroFlw,&nIOStatus);
	switch(nPreMacroSts)
	{
	case PRENONE:
		_LOG("[%s] PreMacro Process Start", m_szModuleName);
		WRITE_DIGITAL(PRMD_PreMacroFlw,PREIDLESTOP,&nIOStatus);
		Report_Event(PMJOB_START);
		break;
	case PREIDLESTOP:
		_LOG("[%s] Idle Recipe Stop Start", m_szModuleName);
		RUN_SET_FUNCTION(CHAMBER_CON_CTL_PM,"ABORT 00 03 00");
		WRITE_DIGITAL(PRMD_PreMacroFlw,PREIDLESTOP_CHK,&nIOStatus);
		Report_Event(PREMACRO_S);
		break;
	case PREIDLESTOP_CHK:
		msRet = (Module_Status)READ_FUNCTION(CHAMBER_CON_CTL_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] Idle Process Stop Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_RECCHK,&nIOStatus);
		}
		break;
	case PRE_RECCHK:
		_LOG("[%s] PreMacro PRE Recipe Check Start", m_szModuleName);

		nPM_CommSts = READ_DIGITAL(PM_CommStatus,&nIOStatus);
		if(nPM_CommSts != PMC_Maint){
			sprintf(szEvent, "SET_MODULE_INFO_LOCAL %s|DISABLE",m_szModuleName);
			WRITE_FUNCTION_EVENT(SCHEDULER2, szEvent);
		}
		
		nMacroID = READ_DIGITAL(PRMD_PreMEn,&nIOStatus);
		if(nMacroID == 0){
			PopupMessage("Pre Macro Bottom Not Select","Check Pre Macro Enable");
			_LOG("[%s] Pre Macro Bottom Not Select", m_szModuleName);
			msRet = SYS_ABORTED;
			break;
		}		
		nIOIndex = nMacroID - 1;
		nPreRecFstUse = READ_DIGITAL(PRMD_PreMRcp_01+nIOIndex, &nIOStatus);
		if(nPreRecFstUse == USE){
			READ_STRING(PRMS_PreMRcp_01+nIOIndex,szPreMacroRecipe, &nIOStatus);
			if(STRCMP_L(szPreMacroRecipe, "") == TRUE){
				PopupMessage("Pre Macro No Recipe","Select Pre PM Recipe");
				_LOG("[%s] Pre Macro No Recipe", m_szModuleName);
				msRet = SYS_ABORTED;
				break;
			}
		}
		nPreRecSndUse = READ_DIGITAL(PRMD_PreMRcp2_01+nIOIndex, &nIOStatus);
		if(nPreRecSndUse == USE){
			READ_STRING(PRMS_PreMRcp2_01+nIOIndex,szPreMacroRecipe2, &nIOStatus);
			if(STRCMP_L(szPreMacroRecipe2, "") == TRUE){
				PopupMessage("Pre Macro No Recipe2","Select Pre PM Recipe2");
				_LOG("[%s] Pre Macro No Recipe2", m_szModuleName);
				msRet = SYS_ABORTED;
				break;
			}
		}

		Report_Event(PREMACRO_REC_S);
		WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_RECRUN,&nIOStatus);
		break;
	case PRE_RECRUN:
		nMacroID = READ_DIGITAL(PRMD_PreMEn,&nIOStatus);
		nIOIndex = nMacroID - 1;
		nPreRecFstUse = READ_DIGITAL(PRMD_PreMRcp_01+nIOIndex, &nIOStatus);	
		if(nPreRecFstUse != USE){
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_REC2RUN,&nIOStatus);
			break;
		}
		_LOG("[%s] PreMacro Recipe Process Start",m_szModuleName);
		nMacroID = READ_DIGITAL(PRMD_PreMEn,&nIOStatus);
		sprintf(szCmd,"PRE_MACRO%d",nMacroID);
		RUN_SET_FUNCTION(SCHEDULER_MAINT_PM,szCmd);
		WRITE_DIGITAL(PRMD_PreMacroFlw,PRERCP_STOPCHK,&nIOStatus);
		break;
	case PRERCP_STOPCHK:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] PreMacro Recipe Process Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_REC2RUN,&nIOStatus);
			break;
		}else if(msRet != SYS_RUNNING){
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nIOStatus);
			_LOG("[%s] PreMacro Recipe Process Failed",m_szModuleName);
			msRet = SYS_ABORTED;
			//WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_END,&nIOStatus);
			break;
		}
		break;
	case PRE_REC2RUN:
		nMacroID = READ_DIGITAL(PRMD_PreMEn,&nIOStatus);
		nIOIndex = nMacroID - 1;
		nPreRecSndUse = READ_DIGITAL(PRMD_PreMRcp2_01+nIOIndex, &nIOStatus);
		if(nPreRecSndUse != USE){
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_LKFUNC,&nIOStatus);
			break;
		}
		Sleep(10000); // 2018.06.07 jhkim - Wait for Wafer count reset in the Preventive function.
		_LOG("[%s] PreMacro Recipe2 Process Start",m_szModuleName);
		nMacroID = READ_DIGITAL(PRMD_PreMEn,&nIOStatus);
		sprintf(szCmd,"PRE2_MACRO%d",nMacroID);
		RUN_SET_FUNCTION(SCHEDULER_MAINT_PM,szCmd);
		WRITE_DIGITAL(PRMD_PreMacroFlw,PRERCP2_STOPCHK,&nIOStatus);
		break;
	case PRERCP2_STOPCHK:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] PreMacro Recipe2 Process Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_LKFUNC,&nIOStatus);
			Report_Event(PREMACRO_REC_E);
			break;
		}else if(msRet != SYS_RUNNING){
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nIOStatus);
			_LOG("[%s] PreMacro Recipe2 Process Failed",m_szModuleName);
			msRet = SYS_ABORTED;
			//WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_END,&nIOStatus);
			break;
		}
		break;
	case PRE_LKFUNC:
	    //... Pre Recipe 끝나고 5초 쉬고 Leak Check한다.
	    Sleep(5000);
		nAutoLeakCheckUse = READ_DIGITAL(PRMD_ALC_Use_After_PreM , &nIOStatus);		
		if(nAutoLeakCheckUse == USE)												
		{
			_LOG("[%s] PREMACRO Leak Check Run",m_szModuleName);					
			msRet = RUN_FUNCTION(SCHEDULER_MAINT_PM,"LEAK_CHECK");
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_LKEND,&nIOStatus);
			break;
		}
		WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_INTCHK,&nIOStatus);
		break;
	case PRE_LKEND:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] PREMACRO Leak Check Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_INTCHK,&nIOStatus);
			break;
		}else if(msRet != SYS_RUNNING){
			_LOG("[%s] PREMACRO Leak Check Failed",m_szModuleName);
			//WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_INTCHK,&nIOStatus);
			break;
		}
		break;		
	case PRE_INTCHK:          
	    //... 성공적으로 Recipe완료 했으면 무조건 Dry Clean 누적 Reset한다.
		nDryCntResetUse = USE;	
		if(nDryCntResetUse == USE){
			RUN_SET_FUNCTION(PREVENTIVE,"RESET 00 02 01");
			_LOG("[%s] Dry Clean Count Reset",m_szModuleName);
		}
		//... PLC Interlock Off할 수 없음
		nPLCIntUse = NOTUSE;
		if(nPLCIntUse != USE){
			_LOG("[%s] PLC Interlock Auto UnLock Skip",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,VENT,&nIOStatus);
			break;
		}
		msRet = PLCIntOperation(IN_UNLOCK);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] PLC Interlock UnLock Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,VENT,&nIOStatus);
			break;
		}else{
			_LOG("[%s] PLC Interlock UnLock Failed",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_END,&nIOStatus);
			break;
		}
	case VENT:
		//... Auto Vent 못한다. PLC Interlock 때문에
		nAutoVentUse = NOTUSE;
		if(nAutoVentUse == USE){
			_LOG("[%s] Vent Process Start",m_szModuleName);
			RUN_SET_FUNCTION(SCHEDULER_MAINT_PM,"VENT");
			WRITE_DIGITAL(PRMD_PreMacroFlw,VENT_CHK,&nIOStatus);
			break;
		}
		WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_END,&nIOStatus);
		break;
	case VENT_CHK:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] Vent Process Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_END,&nIOStatus);
			break;
		}else if(msRet != SYS_RUNNING){
			_LOG("[%s] Vent Process Failed",m_szModuleName);
			WRITE_DIGITAL(PRMD_PreMacroFlw,PRE_END,&nIOStatus);
			break;
		}
		break;
	case PRE_END:
		_LOG("[%s] PreMacro End",m_szModuleName);
		Report_Event(PREMACRO_E);
		break;
	default:
		break;
	}

	if(nPreMacroSts == PRE_END){
		_LOG("[%s] Pre Macro Process Success",m_szModuleName);
	}else if(msRet == SYS_ABORTED
				||m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD == TRUE
				||m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD == TRUE)
	{
		AlarmMessagePost(PM1_PreMacro_Stop+m_nModuleIndex,nPreMacroSts);
		Report_Event(PREMACRO_ABORT);
		_LOG("[%s] Pre Macro Process Fail",m_szModuleName);
	}
	return msRet;
}
void CAutoPM::AlarmMessagePost(int nAlarmID, int nMacroSts)
{	
	int nIOStatus;
	char szMessage[256] = {0,};
	char szAlarmDesc[256] = {0,};
	char szTemp[256] = {0,};


	if(READ_DIGITAL(PRMD_AutoPMFlw,&nIOStatus) == PREMACRO_S){
		switch(nMacroSts)
		{
			case PRENONE: strcpy(szMessage,"Ready");break;
			case PREIDLESTOP: strcpy(szMessage,"IdleStop");break;
			case PREIDLESTOP_CHK: strcpy(szMessage,"IdleStop Check");break;
			case PRE_RECCHK: strcpy(szMessage,"Recipe Check");break;
			case PRE_RECRUN: strcpy(szMessage,"Recipe Run");break;
			case PRERCP_STOPCHK: strcpy(szMessage,"Recipe Stop");break;
			case PRE_REC2RUN: strcpy(szMessage,"Recipe2 Run");break;
			case PRERCP2_STOPCHK: strcpy(szMessage,"Recipe2 Stop");break;
			case PRE_INTCHK: strcpy(szMessage,"Interlcok Check");break;
			case VENT: strcpy(szMessage,"Vent");break;
			case VENT_CHK: strcpy(szMessage,"Vent Check");break;
			case PRE_LKFUNC: strcpy(szMessage,"Leak Check Fail");break;
			case PRE_LKEND: strcpy(szMessage,"Leak Check Fail");break;
			case PRE_END: strcpy(szMessage,"Macro End");break;
		}
	}else if(READ_DIGITAL(PRMD_AutoPMFlw,&nIOStatus) == POSTMACRO_S){
		switch(nMacroSts)
		{
			case POSTNONE: strcpy(szMessage,"Ready");break;
			case VACCHKSTEP: strcpy(szMessage,"Vaccum Check");break;
			case POSTIDLESTOP: strcpy(szMessage,"Idle Stop");break;
			case POSTIDLESTOP_CHK: strcpy(szMessage,"Idle Stop Check");break;
			case POST_RECCHK: strcpy(szMessage,"Recipe Check");break;
			case POST_RECRUN: strcpy(szMessage,"Recipe Run");break;
			case POSTRCP_STOPCHK: strcpy(szMessage,"Recipe Stop");break;
			case POST_REC2RUN: strcpy(szMessage,"Recipe2 Run");break;
			case POSTRCP2_STOPCHK: strcpy(szMessage,"Recipe2 Stop");break;
			case POST_INTCHK: strcpy(szMessage,"Interlock Check");break;
			case VP_LEAKCHK: strcpy(szMessage,"VP Leak Check");break;
			case POST_MFCVERFI: strcpy(szMessage,"MFC Verfi Fail");break;
			case POSTMFCVERCHK: strcpy(szMessage,"MFC Verfi Fail");break;
			case POST_LKFUNC: strcpy(szMessage,"Leak Check Fail");break;
			case POST_LKEND: strcpy(szMessage,"Leak Check Fail");break;
			case POST_END: strcpy(szMessage,"Macro End");break;
		}
	}
	sprintf(szTemp, "(%s)", szMessage);
	ALARM_MESSAGE_GET(nAlarmID, szAlarmDesc );
	strcat( szAlarmDesc , szTemp );
	ALARM_MESSAGE_SET(nAlarmID, szAlarmDesc);
	ALARM_POST(nAlarmID);
}

Module_Status CAutoPM::PostMacro(int nValue)
{
	Module_Status msRet = SYS_SUCCESS;
	char szPostMacroRecipe[256] = {0,};
	char szPostMacroRecipe2[256] = {0,};
	char szCmd[256] = {0,};
	int nCommStatus;
	int nMacroID = 0;
	int nIOIndex = 0;
	int nIOStatus;
	int nRoughStsDI;
	double dbChmPrsSts;
	double dbChmPraPrsSts;
	BOOL bPostMacroEnd = FALSE;
	int nPLCIntOpSts = 0;
	int nPostMacroSts;
	int nPostRecFstUse;
	int nPostRecSndUse;
	int nWetCntResetUse;
	int nPLCIntUse;
	int nMFCverification_Use;
	int nAutoLeakCheckUse;

	memset(szPostMacroRecipe,0,sizeof(szPostMacroRecipe));
	memset(szPostMacroRecipe2,0,sizeof(szPostMacroRecipe2));
	/*
	if(nValue == WETPM){
		bPostRecFstUse = m_AutoPMCfgFile[m_nModuleIndex].bPostRecFstUse;
		bPostRecSndUse = m_AutoPMCfgFile[m_nModuleIndex].bPostRecSndUse;
		bWetCntResetUse = m_AutoPMCfgFile[m_nModuleIndex].bWetCntResetUse;
		bPLCIntUse = m_AutoPMCfgFile[m_nModuleIndex].bPLCIntUse;
	}
	else if(nValue == DRYCLEAN){
		bPostRecFstUse = m_AutoPMCfgFile2[m_nModuleIndex].bPostRecFstUse;
		bPostRecSndUse = m_AutoPMCfgFile2[m_nModuleIndex].bPostRecSndUse;
		bWetCntResetUse = m_AutoPMCfgFile2[m_nModuleIndex].bWetCntResetUse;
		bPLCIntUse = m_AutoPMCfgFile2[m_nModuleIndex].bPLCIntUse;
	} 
	*/
	nPostMacroSts = READ_DIGITAL(PRMD_PostMacroFlw,&nIOStatus);
	switch(nPostMacroSts)
	{
	case POSTNONE:
		_LOG("[%s] Post Macro Process Start",m_szModuleName);
		WRITE_DIGITAL(PRMD_PostMacroFlw,VACCHKSTEP,&nIOStatus);
		Report_Event(POSTMACRO_S);
		break;
	case VACCHKSTEP:
		_LOG("[%s] Chamber Vaccum Status Check Start",m_szModuleName);
		nRoughStsDI = READ_DIGITAL(RoughValveDI,&nCommStatus);
		dbChmPrsSts = GetChamberPressure();
		dbChmPraPrsSts = READ_ANALOG(PRMA_VS_PrsBaRough,&nCommStatus);
		if(nRoughStsDI == R_OPEN && dbChmPrsSts <= dbChmPraPrsSts){
			_LOG("[%s] Chamber Vaccum Status Check Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POSTIDLESTOP,&nIOStatus);
			break;
		}else{
			msRet = SYS_ABORTED;
			_LOG("[%s] Chamber Vaccum Status Check Fail",m_szModuleName);
			PopupMessage("Not Vaccum","Check Chamber Status And Press Post Macro Start Bottom");
			break;
		}		
	case POSTIDLESTOP:
		_LOG("[%s] Idle Recipe Stop Start", m_szModuleName);
		RUN_SET_FUNCTION(CHAMBER_CON_CTL_PM,"ABORT 00 03 00");
		WRITE_DIGITAL(PRMD_PostMacroFlw,POSTIDLESTOP_CHK,&nIOStatus);
		break;
	case POSTIDLESTOP_CHK:
		msRet = (Module_Status)READ_FUNCTION(CHAMBER_CON_CTL_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] Idle Process Stop Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_LKFUNC,&nIOStatus);
		}
		break;
	case POST_LKFUNC:
	    //... Idle Stop후 5초 쉬고 leak Check한다.
	    Sleep(5000);
		nAutoLeakCheckUse = READ_DIGITAL(PRMD_ALC_Use_PostM, &nIOStatus);		
		if(nAutoLeakCheckUse == USE)												
		{
			_LOG("[%s] POSTMACRO Leak Check Run",m_szModuleName);					
			msRet = RUN_FUNCTION(SCHEDULER_MAINT_PM,"LEAK_CHECK");
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_LKEND,&nIOStatus);
			break;
		}
		WRITE_DIGITAL(PRMD_PostMacroFlw,POST_MFCVERFI,&nIOStatus);
		break;
	case POST_LKEND:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] POSTMACRO Leak Check Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_MFCVERFI,&nIOStatus);
			break;
		}else if(msRet != SYS_RUNNING){
			_LOG("[%s] POSTMACRO Leak Check Failed",m_szModuleName);
			//WRITE_DIGITAL(PRMD_PostMacroFlw,POST_MFCVERFI,&nIOStatus);
			break;
		}
		break;		
	case POST_MFCVERFI: 
		//... Leak check완료 후 5초 쉬고 MFC Verification 한다.
		Sleep(5000);
		//nMFCverification_Use = READ_DIGITAL(PRMD_ALC_Use_PostM, &nIOStatus);		
		nMFCverification_Use = READ_DIGITAL(PRMD_MFCveriUse, &nIOStatus);		
		if(nMFCverification_Use == USE)												
		{
			_LOG("[%s] POSTMACRO MFC Verification Run",m_szModuleName);					
			//msRet = RUN_FUNCTION(SCHEDULER_MAINT_PM,"LEAK_CHECK");
			msRet = RUN_FUNCTION(SCHEDULER_MAINT_PM,"CALIBRATION");
			WRITE_DIGITAL(PRMD_PostMacroFlw,POSTMFCVERCHK,&nIOStatus);
			break;
		}
		WRITE_DIGITAL(PRMD_PostMacroFlw,POST_RECCHK,&nIOStatus);
		break;
	case POSTMFCVERCHK:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] POSTMACRO MFC Verification Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_RECCHK,&nIOStatus);
			break;
		}else if(msRet != SYS_RUNNING){
			_LOG("[%s] POSTMACRO MFC Verification Failed",m_szModuleName);
			//WRITE_DIGITAL(PRMD_PostMacroFlw,POST_RECCHK,&nIOStatus);
			break;
		}
		break;		
	case POST_RECCHK:
		_LOG("[%s] PostMacro Post Recipe Check Start", m_szModuleName);

		nMacroID = READ_DIGITAL(PRMD_PostMEn,&nCommStatus);
		if(nMacroID == 0){
			PopupMessage("Post Macro Bottom Not Select","Check Post Macro Enable");
			_LOG("[%s] Post Macro Bottom Not Select", m_szModuleName);
			msRet = SYS_ABORTED;
			break;
		}		
		nIOIndex = nMacroID - 1;
		nPostRecFstUse = READ_DIGITAL(PRMD_PostMRcp_01+nIOIndex, &nIOStatus);
		if(nPostRecFstUse == USE){
			READ_STRING(PRMS_PostMRcp_01+nIOIndex,szPostMacroRecipe, &nCommStatus);
			if(STRCMP_L(szPostMacroRecipe, "") == TRUE){
				PopupMessage("Post Macro Recipe","Select Post Recipe");
				_LOG("[%s] Post Macro No Recipe", m_szModuleName);
				msRet = SYS_ABORTED;
				break;
			}
		}
		nPostRecSndUse = READ_DIGITAL(PRMD_PostMRcp2_01+nIOIndex, &nIOStatus); 
		if(nPostRecSndUse == USE){
			READ_STRING(PRMS_PostMRcp2_01+nIOIndex,szPostMacroRecipe2, &nCommStatus);
			if(STRCMP_L(szPostMacroRecipe2, "") == TRUE){
				PopupMessage("Post Macro Recipe2","Select Post Recipe2");
				_LOG("[%s] Post Macro No Recipe2", m_szModuleName);
				msRet = SYS_ABORTED;
				break;
			}
		}
		Report_Event(POSTMACRO_REC_S);
		WRITE_DIGITAL(PRMD_PostMacroFlw,POST_RECRUN,&nIOStatus);
		break;
	case POST_RECRUN:
		nMacroID = READ_DIGITAL(PRMD_PostMEn,&nCommStatus);
		nIOIndex = nMacroID - 1;
		nPostRecFstUse = READ_DIGITAL(PRMD_PostMRcp_01+nIOIndex, &nIOStatus);	
		if(nPostRecFstUse != USE){
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_REC2RUN,&nIOStatus);
			break;
		}
		_LOG("[%s] Post Macro Recipe Process Start",m_szModuleName);
		nMacroID = READ_DIGITAL(PRMD_PostMEn,&nCommStatus);
		sprintf(szCmd,"POST_MACRO%d",nMacroID);
		RUN_SET_FUNCTION(SCHEDULER_MAINT_PM,szCmd);
		WRITE_DIGITAL(PRMD_PostMacroFlw,POSTRCP_STOPCHK,&nIOStatus);
		break;
	case POSTRCP_STOPCHK:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] Post Macro Recipe Process Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_REC2RUN,&nIOStatus);
			break;
		}else if(msRet != SYS_RUNNING){
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nIOStatus);
			_LOG("[%s] Post Macro Recipe Process Failed",m_szModuleName);
			msRet = SYS_ABORTED;
			//WRITE_DIGITAL(PRMD_PostMacroFlw,POST_END,&nIOStatus);
			break;
		}
		break;
	case POST_REC2RUN:
		nMacroID = READ_DIGITAL(PRMD_PostMEn,&nCommStatus);
		nIOIndex = nMacroID - 1;
		nPostRecSndUse = READ_DIGITAL(PRMD_PostMRcp2_01+nIOIndex, &nIOStatus);	
		if(nPostRecSndUse != USE){
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_INTCHK,&nIOStatus);
			break;
		}
		Sleep(10000); // 2018.06.07 jhkim - Wait for Wafer count reset in the Preventive function.
		_LOG("[%s] Post Macro Recipe2 Process Start",m_szModuleName);
		nMacroID = READ_DIGITAL(PRMD_PostMEn,&nCommStatus);
		sprintf(szCmd,"POST2_MACRO%d",nMacroID);
		RUN_SET_FUNCTION(SCHEDULER_MAINT_PM,szCmd);
		WRITE_DIGITAL(PRMD_PostMacroFlw,POSTRCP2_STOPCHK,&nIOStatus);
		break;
	case POSTRCP2_STOPCHK:
		msRet = (Module_Status)READ_FUNCTION(SCHEDULER_MAINT_PM);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] Post Macro Recipe2 Process Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_INTCHK,&nIOStatus);
			Report_Event(POSTMACRO_REC_E);
			break;
		}else if(msRet != SYS_RUNNING){
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nIOStatus);
			_LOG("[%s] Post Macro Recipe2 Process Failed",m_szModuleName);
			msRet = SYS_ABORTED;
			//WRITE_DIGITAL(PRMD_PostMacroFlw,POST_END,&nIOStatus);
			break;
		}
		break;
	case POST_INTCHK:
	    //... Post  Recipe정상 완료 했으면 Wet PM Count Reset한다.
		nWetCntResetUse = USE;		
		if(nWetCntResetUse == USE){
			RUN_SET_FUNCTION(PREVENTIVE,"RESET 00 01 01");
			_LOG("[%s] Wet Clean Count Reset",m_szModuleName);
		}
		//... PLC Interlock Auto Unlock/Lock 안됨 
		nPLCIntUse = NOTUSE;
		if(nPLCIntUse != USE){
			_LOG("[%s] PLC Interlock Auto Lock Skip",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_END,&nIOStatus);
			break;
		}
		msRet = PLCIntOperation(IN_LOCK);
		if(msRet == SYS_SUCCESS){
			_LOG("[%s] PLC Interlock Lock Success",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,VP_LEAKCHK,&nIOStatus);
			break;
		}else{
			_LOG("[%s] PLC Interlock Lock Failed",m_szModuleName);
			WRITE_DIGITAL(PRMD_PostMacroFlw,POST_END,&nIOStatus);
			msRet = SYS_ABORTED;
			break;
		}
	case VP_LEAKCHK:
		_LOG("[%s] VP_LEAKCHK Leak Check Start",m_szModuleName);
		WRITE_DIGITAL(PRMD_PostMacroFlw,POST_END,&nIOStatus);
		break;
	case POST_END:
		_LOG("[%s] PostMacro End",m_szModuleName);
		Report_Event(POSTMACRO_E);
		break;
	default:
		break;
	}

	if(nPostMacroSts == POST_END){
		_LOG("[%s] Post Macro Process Success",m_szModuleName);
	}else if(msRet == SYS_ABORTED
				||m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD == TRUE
				||m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD == TRUE)
	{
		AlarmMessagePost(PM1_PostMacro_Stop+m_nModuleIndex,nPostMacroSts);
		_LOG("[%s] Post Macro[Temp Up] Process Fail",m_szModuleName);
		Report_Event(POSTMACRO_ABORT);
		_LOG("[%s] Post Macro Process Fail",m_szModuleName);
	}	
	
	return msRet;
}
Module_Status CAutoPM::Manual_START(int nEventID)
{
	Module_Status msRet = SYS_SUCCESS;
	int nCommStatus;

	_LOG("[%s] Manual SSP Process Start",m_szModuleName);

	switch(nEventID)
	{
		case PREMACRO_S:
			PopupMessage("Pre Macro Start","Manual Pre Macro Process Start");			
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nCommStatus);
			WRITE_DIGITAL(ReserveDI,PMJOB_REV,&nCommStatus);
			if(WetPM_1 == READ_DIGITAL(MANUAL_SSP,&nCommStatus))
			{
				m_nMacroID = 2;
				WRITE_STRING(AutoPMMacroName,"WetPM",&nCommStatus);
			}
			else
			{
				m_nMacroID = 1;
				WRITE_STRING(AutoPMMacroName,"DryClean",&nCommStatus); 
			}
			Report_Event(PREMACRO_S);
			break;
		case POSTMACRO_S:
			PopupMessage("Post Macro Start","Manual Post Macro Process Start");
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nCommStatus);
			WRITE_DIGITAL(ReserveDI,PMJOB_REV,&nCommStatus);
			if(WetPM_1 == READ_DIGITAL(MANUAL_SSP,&nCommStatus))
			{
				m_nMacroID = 2;
				WRITE_STRING(AutoPMMacroName,"WetPM",&nCommStatus);
			}
			else
			{
				m_nMacroID = 1;
				WRITE_STRING(AutoPMMacroName,"DryClean",&nCommStatus); 
			}	
			Report_Event(POSTMACRO_S);
			break;
	}
	if(msRet == SYS_SUCCESS)_LOG("[%s] Manual Abort Process Finish",m_szModuleName);
	else _LOG("[%s] Manual Abort Process Fail",m_szModuleName);

	return msRet; 
}

Module_Status CAutoPM::Manual_END(int nEventID)
{
	Module_Status msRet = SYS_SUCCESS;
	int nCommStatus;

	_LOG("[%s] Manual END Process Start",m_szModuleName);

	switch(nEventID)
	{
		case PREMACRO_ABORT:
			PopupMessage("Pre Macro End","Pre Macro Process End");
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nCommStatus);
			Report_Event(PREMACRO_ABORT);
			break;
		case POSTMACRO_ABORT:
			PopupMessage("Post Macro End","Post Macro Process End");
			WRITE_DIGITAL(AutoPMOwner,C_OPERATOR,&nCommStatus);
			Report_Event(POSTMACRO_ABORT);
			break;
	}
	if(msRet == SYS_SUCCESS)_LOG("[%s] Manual Abort Process Finish",m_szModuleName);
	else _LOG("[%s] Manual Abort Process Fail",m_szModuleName);

	return msRet; 
}
Module_Status CAutoPM::AutoPMMacro(int nValue)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	int nMacroSts;
	char szEvent[256] = {0,};
	BOOL bPreMacroUse;
	BOOL bPostMacroUse;
	BOOL bAutoPMUse;
	BOOL bPostMacroAutoStart;
	BOOL bPhysicalPMUse;
	BOOL bAutoEnable;
	int nTopLIdSts;
	
	memset(szEvent,0,sizeof(szEvent));

	if(nValue == WETPM){
		bAutoPMUse = m_AutoPMCfgFile[m_nModuleIndex].bAutoPMUse;
		bPreMacroUse = m_AutoPMCfgFile[m_nModuleIndex].bPreMacroUse;
		bPostMacroUse = m_AutoPMCfgFile[m_nModuleIndex].bPostMacroUse;
		bPostMacroAutoStart = m_AutoPMCfgFile[m_nModuleIndex].bPostMacroAutoStart;
		bPhysicalPMUse = m_AutoPMCfgFile[m_nModuleIndex].bPhysicalPMUse;
		bAutoEnable = m_AutoPMCfgFile[m_nModuleIndex].bAutoEnable;
	}
	else if(nValue == DRYCLEAN){
		bAutoPMUse = m_AutoPMCfgFile2[m_nModuleIndex].bAutoPMUse;
		bPreMacroUse = m_AutoPMCfgFile2[m_nModuleIndex].bPreMacroUse;
		bPostMacroUse = m_AutoPMCfgFile2[m_nModuleIndex].bPostMacroUse;
		bPostMacroAutoStart = m_AutoPMCfgFile[m_nModuleIndex].bPostMacroAutoStart;
		bPhysicalPMUse = m_AutoPMCfgFile2[m_nModuleIndex].bPhysicalPMUse;
		bAutoEnable = m_AutoPMCfgFile2[m_nModuleIndex].bAutoEnable;
	}

	if(bAutoPMUse != TRUE){
			WRITE_DIGITAL(ReserveDI,NONE_REV,&nIOStatus);
			return msRet;
	}
	nTopLIdSts = READ_DIGITAL(TopLidSwitchDI,&nIOStatus);
	nMacroSts = READ_DIGITAL(PRMD_AutoPMFlw,&nIOStatus);
	switch(nMacroSts)
	{
		case PREMACRO_S:
			if(bPreMacroUse == TRUE){
				PreMacro(nValue);
				break;
			}
		case PREMACRO_E:
			if(bPhysicalPMUse != TRUE){
				AutoPM_FlwSts(POSTMACRO_S); 
			}
			if(nTopLIdSts == TOPLID_OPEN){
				PopupMessage("Physical Button Check","Press Physical Start Button");
			}
			break;
		case PHYSICAL_S:break;
		case PHYSICAL_E:
			if(bPostMacroAutoStart == TRUE){
				AutoPM_FlwSts(POSTMACRO_S); 
				PopupMessage("Post Macro Start","Post Macro Auto Start");
			}
			break;
		case POSTMACRO_S:
		   if(bPostMacroUse == TRUE){
				PostMacro(nValue);
				break;
		   }else{
				AutoPM_FlwSts(PMJOB_E);
				break;
		   }
		case POSTMACRO_E:
		case PMJOB_E:
			Report_Event(PMJOB_E);
			if(bAutoEnable == TRUE){
				sprintf(szEvent, "SET_MODULE_INFO_LOCAL %s|ENABLE",m_szModuleName);
				WRITE_FUNCTION_EVENT(SCHEDULER2, szEvent);
			}
			break;
	}
	return msRet;
}

int CAutoPM::SeletPMMacro()
{
	int nCommStatus;
	char szMacroName[20] = {0,};
	memset(szMacroName,0,sizeof(szMacroName));
	READ_STRING(AutoPMMacroName,szMacroName,&nCommStatus);
	if(STRCMP_L(szMacroName,"WetPM")){
		return WETPM;
	}else if(STRCMP_L(szMacroName,"DryClean")){
		return DRYCLEAN;
	}
	return NotPM;
}

double CAutoPM::GetChamberPressure()
{
	double fChmPres;
	int nIOStatus;

	fChmPres = READ_ANALOG(M651_Pressure, &nIOStatus);
	if(fChmPres >= 10)
	{
		fChmPres  = READ_ANALOG(ChmPrsAI, &nIOStatus);
	}

	return fChmPres;
}

void CAutoPM::DoMonitor()
{
	Module_Status msRet = SYS_ABORTED;
	int nCommStatus;
	int nAutoPMType;

	while(WaitSec(5))
	{
		if(PMJOB_REV != READ_DIGITAL(ReserveDI,&nCommStatus)) 
		{
			m_nReserveStatus = FALSE;
			continue;
		}
		else
		{
			m_nReserveStatus = TRUE;
		}
		 nAutoPMType = SeletPMMacro();

		if(READ_DIGITAL(PMJobStart,&nCommStatus)){
			switch(nAutoPMType){
				case WETPM:
					AutoPMMacro(WETPM);
					break;
				case DRYCLEAN:
					AutoPMMacro(DRYCLEAN);
					break;
                case NotPM:
					WRITE_DIGITAL(ReserveDI,NONE_REV,&nCommStatus);
					break;
			}
		}else{
			msRet = PMMonitor(nAutoPMType);
			if(msRet != SYS_SUCCESS) continue;
		}
	}
}

Module_Status CAutoPM::Manaul_Abort()
{
	Module_Status msRet = SYS_SUCCESS;	
	int nCommStatus;
	int nCount=0;

	_LOG("Manual Abort Start");
	
	RUN_SET_FUNCTION(ABORT_PROCESS,"");
	WRITE_DIGITAL(ReserveDI,NONE_REV,&nCommStatus);

	while(m_nReserveStatus)
	{
		_LOG("[Manaul_Abort] nCount : %d, PRMD_AutoPMFlw : %d, ReserveDI : %d, PMJobStart : %d, PRO_DryClnStsDM : %d, bPMJobStopRCMD : %d, bPMJobAbortRCMD: %d\n", nCount, 
			READ_DIGITAL(PRMD_AutoPMFlw, &nCommStatus), READ_DIGITAL(ReserveDI, &nCommStatus), READ_DIGITAL(PMJobStart, &nCommStatus), READ_DIGITAL(PRO_DryClnStsDM, &nCommStatus),
			m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD,	m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD);

		Sleep(1000);
		
		if(nCount++ > 30)	break;	
	}

	AutoPM_FlwSts(UNKNOWN_TYPE);
	WRITE_DIGITAL(PMJobStart,NONE_REV,&nCommStatus);	
	WRITE_DIGITAL(PRO_DryClnStsDM,PMNONE,&nCommStatus);
	WRITE_DIGITAL(AutoPMOwner,C_HOST,&nCommStatus);
	WRITE_STRING (AutoPMEventName, "" , &nCommStatus);	
	WRITE_DIGITAL(PRMD_PreMacroFlw,NONE_REV,&nCommStatus);
	WRITE_DIGITAL(PRMD_PostMacroFlw,NONE_REV,&nCommStatus);


	m_AutoPMCfgFile[m_nModuleIndex].bPMJobStopRCMD = FALSE;
	m_AutoPMCfgFile[m_nModuleIndex].bPMJobAbortRCMD = FALSE;

	_LOG("Manual Abort End");
	
	return SYS_SUCCESS;
}