// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CModuleConditionControl_Client.h"
#include "ModuleConditionControl_Client_Type2.h"
#include "TextParser.h"

CModuleConditionControl_Client* g_pCModuleConditionControl_Client = NULL;
string g_strModuleName = "";
int g_nALARM_BASE = 9999;
int g_nCommStatus = 0;

// 2006.10.18
enum { CType1_MAINT_CLEAN, CType2_ONLINE_CLEAN };
int g_nCleanType = CType1_MAINT_CLEAN;

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "CTC.$0_CCondition_SAVE"		, _K_D_IO  ,	CCONDITION_SAVE			,	0	} ,
	{ "PRMD_$0_IDLE_TimeD"			, _K_D_IO  ,	PRMD_IDLE_TimeD			,	0	} ,
	{ "PRMD_$0_IDLE_TimeH"			, _K_D_IO  ,	PRMD_IDLE_TimeH			,	0	} ,
	{ "PRMD_$0_IDLE_TimeM"			, _K_D_IO  ,	PRMD_IDLE_TimeM			,	0	} ,
	{ "PRMD_$0_IDLE_TimeS"			, _K_D_IO  ,	PRMD_IDLE_TimeS			,	0	} ,

	{ "$0.PRO_ChamberStatus"	    , _K_D_IO  ,	PMChamberStatus			,	0	} ,

	//
	// IO list for Composite
	//
	// DRY Cleaning
	{ "PRMD_$0_Comp_RunSt_01"		, _K_D_IO  ,	PRMD_Comp_RunSt_01		,	0	} ,
	{ "PRMD_$0_Comp_ModeEn_01"		, _K_D_IO  ,	PRMD_Comp_ModeEn_01		,	0	} ,
	{ "PRMD_$0_Comp_TimeD_01"		, _K_D_IO  ,	PRMD_Comp_TimeD_01		,	0	} ,
	{ "PRMD_$0_Comp_TimeH_01"		, _K_D_IO  ,	PRMD_Comp_TimeH_01		,	0	} ,
	{ "PRMD_$0_Comp_TimeM_01"		, _K_D_IO  ,	PRMD_Comp_TimeM_01		,	0	} ,
	{ "PRMD_$0_Comp_TimeS_01"		, _K_D_IO  ,	PRMD_Comp_TimeS_01		,	0	} ,
	
	// WET Cleaning
	{ "PRMD_$0_Comp_RunSt_02"		, _K_D_IO  ,	PRMD_Comp_RunSt_02		,	0	} ,
	{ "PRMD_$0_Comp_ModeEn_02"		, _K_D_IO  ,	PRMD_Comp_ModeEn_02		,	0	} ,
	{ "PRMD_$0_Comp_TimeD_02"		, _K_D_IO  ,	PRMD_Comp_TimeD_02		,	0	} ,
	{ "PRMD_$0_Comp_TimeH_02"		, _K_D_IO  ,	PRMD_Comp_TimeH_02		,	0	} ,
	{ "PRMD_$0_Comp_TimeM_02"		, _K_D_IO  ,	PRMD_Comp_TimeM_02		,	0	} ,
	{ "PRMD_$0_Comp_TimeS_02"		, _K_D_IO  ,	PRMD_Comp_TimeS_02		,	0	} ,

	// IDLE Control
	{ "PRMD_$0_Comp_RunSt_03"		, _K_D_IO  ,	PRMD_Comp_RunSt_03		,	0	} ,
	{ "PRMD_$0_Comp_ModeEn_03"		, _K_D_IO  ,	PRMD_Comp_ModeEn_03		,	0	} ,
	{ "PRMD_$0_Comp_TimeD_03"		, _K_D_IO  ,	PRMD_Comp_TimeD_03		,	0	} ,
	{ "PRMD_$0_Comp_TimeH_03"		, _K_D_IO  ,	PRMD_Comp_TimeH_03		,	0	} ,
	{ "PRMD_$0_Comp_TimeM_03"		, _K_D_IO  ,	PRMD_Comp_TimeM_03		,	0	} ,
	{ "PRMD_$0_Comp_TimeS_03"		, _K_D_IO  ,	PRMD_Comp_TimeS_03		,	0	} ,

	// Pre-Treatment
	{ "PRMD_$0_Comp_RunSt_04"		, _K_D_IO  ,	PRMD_Comp_RunSt_04		,	0	} ,
	{ "PRMD_$0_Comp_ModeEn_04"		, _K_D_IO  ,	PRMD_Comp_ModeEn_04		,	0	} ,
	{ "PRMD_$0_Comp_TimeD_04"		, _K_D_IO  ,	PRMD_Comp_TimeD_04		,	0	} ,
	{ "PRMD_$0_Comp_TimeH_04"		, _K_D_IO  ,	PRMD_Comp_TimeH_04		,	0	} ,
	{ "PRMD_$0_Comp_TimeM_04"		, _K_D_IO  ,	PRMD_Comp_TimeM_04		,	0	} ,
	{ "PRMD_$0_Comp_TimeS_04"		, _K_D_IO  ,	PRMD_Comp_TimeS_04		,	0	} ,

	//2007.7.4 Added
	//WET Backup Component + Error Backup Component
	// WET Backup
	{ "PRMD_$0_Comp_RunSt_05"		, _K_D_IO  ,	PRMD_Comp_RunSt_05		,	0	} ,
	{ "PRMD_$0_Comp_ModeEn_05"		, _K_D_IO  ,	PRMD_Comp_ModeEn_05		,	0	} ,
	{ "PRMD_$0_Comp_TimeD_05"		, _K_D_IO  ,	PRMD_Comp_TimeD_05		,	0	} ,
	{ "PRMD_$0_Comp_TimeH_05"		, _K_D_IO  ,	PRMD_Comp_TimeH_05		,	0	} ,
	{ "PRMD_$0_Comp_TimeM_05"		, _K_D_IO  ,	PRMD_Comp_TimeM_05		,	0	} ,
	{ "PRMD_$0_Comp_TimeS_05"		, _K_D_IO  ,	PRMD_Comp_TimeS_05		,	0	} ,

	// Error Backup
	{ "PRMD_$0_Comp_RunSt_06"		, _K_D_IO  ,	PRMD_Comp_RunSt_06		,	0	} ,
	{ "PRMD_$0_Comp_ModeEn_06"		, _K_D_IO  ,	PRMD_Comp_ModeEn_06		,	0	} ,
	{ "PRMD_$0_Comp_TimeD_06"		, _K_D_IO  ,	PRMD_Comp_TimeD_06		,	0	} ,
	{ "PRMD_$0_Comp_TimeH_06"		, _K_D_IO  ,	PRMD_Comp_TimeH_06		,	0	} ,
	{ "PRMD_$0_Comp_TimeM_06"		, _K_D_IO  ,	PRMD_Comp_TimeM_06		,	0	} ,
	{ "PRMD_$0_Comp_TimeS_06"		, _K_D_IO  ,	PRMD_Comp_TimeS_06		,	0	} ,

	// Error Backup2
	{ "PRMD_$0_Comp_RunSt_07"		, _K_D_IO  ,	PRMD_Comp_RunSt_07		,	0	} ,
	{ "PRMD_$0_Comp_ModeEn_07"		, _K_D_IO  ,	PRMD_Comp_ModeEn_07		,	0	} ,
	{ "PRMD_$0_Comp_TimeD_07"		, _K_D_IO  ,	PRMD_Comp_TimeD_07		,	0	} ,
	{ "PRMD_$0_Comp_TimeH_07"		, _K_D_IO  ,	PRMD_Comp_TimeH_07		,	0	} ,
	{ "PRMD_$0_Comp_TimeM_07"		, _K_D_IO  ,	PRMD_Comp_TimeM_07		,	0	} ,
	{ "PRMD_$0_Comp_TimeS_07"		, _K_D_IO  ,	PRMD_Comp_TimeS_07		,	0	} ,


	//
	// IO list for Leaf
	//
	// DRY Cleaning (Cleaning)
	{ "PRMD_$0_Leaf_AuEn_01"		, _K_D_IO  ,	PRMD_Leaf_AuEn_01		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_01"			, _K_S_IO  ,	PRMS_Leaf_Rcp_01		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_01"		, _K_D_IO  ,	PRMD_Leaf_RunSt_01		,	0	} ,

	// DRY Cleaning (Leak-Check)
	{ "PRMD_$0_Leaf_AuEn_02"		, _K_D_IO  ,	PRMD_Leaf_AuEn_02		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_02"			, _K_S_IO  ,	PRMS_Leaf_Rcp_02		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_02"		, _K_D_IO  ,	PRMD_Leaf_RunSt_02		,	0	} ,

	// DRY Cleaning (DRY-Post)
	{ "PRMD_$0_Leaf_AuEn_03"		, _K_D_IO  ,	PRMD_Leaf_AuEn_03		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_03"			, _K_S_IO  ,	PRMS_Leaf_Rcp_03		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_03"		, _K_D_IO  ,	PRMD_Leaf_RunSt_03		,	0	} ,

	// WET Cleaning
	//2007.07.04 Added Leaf 
	{ "PRMD_$0_Leaf_AuEn_04"		, _K_D_IO  ,	PRMD_Leaf_AuEn_04		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_04"			, _K_S_IO  ,	PRMS_Leaf_Rcp_04		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_04"		, _K_D_IO  ,	PRMD_Leaf_RunSt_04		,	0	} ,
	//WET Post
	{ "PRMD_$0_Leaf_AuEn_07"		, _K_D_IO  ,	PRMD_Leaf_AuEn_07		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_07"			, _K_S_IO  ,	PRMS_Leaf_Rcp_07		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_07"		, _K_D_IO  ,	PRMD_Leaf_RunSt_07		,	0	} ,

	// IDLE Control
	{ "PRMD_$0_Leaf_AuEn_05"		, _K_D_IO  ,	PRMD_Leaf_AuEn_05		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_05"			, _K_S_IO  ,	PRMS_Leaf_Rcp_05		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_05"		, _K_D_IO  ,	PRMD_Leaf_RunSt_05		,	0	} ,

	// Pre-Treatment
	{ "PRMD_$0_Leaf_AuEn_06"		, _K_D_IO  ,	PRMD_Leaf_AuEn_06		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_06"			, _K_S_IO  ,	PRMS_Leaf_Rcp_06		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_06"		, _K_D_IO  ,	PRMD_Leaf_RunSt_06		,	0	} ,

	//2007.07.04 Added Leaf (WET Backup Leaf + Error Backup Leaf
	//WET Backup
	{ "PRMD_$0_Leaf_AuEn_08"		, _K_D_IO  ,	PRMD_Leaf_AuEn_08		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_08"			, _K_S_IO  ,	PRMS_Leaf_Rcp_08		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_08"		, _K_D_IO  ,	PRMD_Leaf_RunSt_08		,	0	} ,
	//WET Backup Post
	{ "PRMD_$0_Leaf_AuEn_09"		, _K_D_IO  ,	PRMD_Leaf_AuEn_09		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_09"			, _K_S_IO  ,	PRMS_Leaf_Rcp_09		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_09"		, _K_D_IO  ,	PRMD_Leaf_RunSt_09		,	0	} ,

	//Error Backup
	{ "PRMD_$0_Leaf_AuEn_10"		, _K_D_IO  ,	PRMD_Leaf_AuEn_10		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_10"			, _K_S_IO  ,	PRMS_Leaf_Rcp_10		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_10"		, _K_D_IO  ,	PRMD_Leaf_RunSt_10		,	0	} ,
	//Error Backup Post
	{ "PRMD_$0_Leaf_AuEn_11"		, _K_D_IO  ,	PRMD_Leaf_AuEn_11		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_11"			, _K_S_IO  ,	PRMS_Leaf_Rcp_11		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_11"		, _K_D_IO  ,	PRMD_Leaf_RunSt_11		,	0	} ,

	//Error Backup2
	{ "PRMD_$0_Leaf_AuEn_12"		, _K_D_IO  ,	PRMD_Leaf_AuEn_12		,	0	} ,
	{ "PRMS_$0_Leaf_Rcp_12"			, _K_S_IO  ,	PRMS_Leaf_Rcp_12		,	0	} ,
	{ "PRMD_$0_Leaf_RunSt_12"		, _K_D_IO  ,	PRMD_Leaf_RunSt_12		,	0	} ,


	{ "Scheduler"					, _K_F_IO  ,	SCHEDULER				,	0	} ,
	{ "e:SCHEDULER"					, _K_S_IO  ,	SCHEDULER2				,	0	} ,
	{ "SCHEDULER_MAINT_$0"			, _K_F_IO  ,	SCHEDULER_MAINT			,	0	} ,
	{ "CTC.$0_RECIPE_FILE"			, _K_S_IO  ,	RECIPE_FILE				,	0	} ,

	{ "$0.MAINT_CONTROL"			, _K_D_IO  ,	MAINT_CONTROL			,	0	} ,
	{ "CTC.$0_CommStatus"			, _K_D_IO  ,	COMM_STATUS				,	0	} ,
	{ "CTC.FA_STATUS_MDL_$0"		, _K_D_IO  ,	FA_STATUS				,	0	} ,
	{ "$0.PRO_RunStsDM"				, _K_D_IO  ,	PROCESS_RUN_STATUS		,	0	} ,
	{ "ABORT_PROCESS_$0"			, _K_F_IO  ,	ABORT_PROCESS			,	0	} ,
	{ "$0.PRO_IdleRcpStsDM"			, _K_D_IO  ,	PRO_IdleRcpStsDM		,	0	} ,

	{ "PRMD_$0_DRYAlarm"			, _K_D_IO  ,	DRY_CLEANING_ALARM		,	0	} ,
	{ "PRMD_$0_WETAlarm"			, _K_D_IO  ,	WET_CLEANING_ALARM		,	0	} ,
	{ "$0.PRE_TREATMENT_NEED"		, _K_D_IO  ,	PRE_TREATMENT_NEED		,	0	} ,
	{ "TM.$0.SlotVv1XI"				, _K_D_IO  ,	SLOT_VALVE1_XI			,	0	} ,
	{ "TM.$0.SlotVv2XI"				, _K_D_IO  ,	SLOT_VALVE2_XI			,	0	} ,
	{ "CTC.$0_Wafer_Status"			, _K_D_IO  ,	WAFER_STATUS			,	0	} ,

	{ "PREVENTIVE_$0"				, _K_F_IO  ,	PREVENTIVE				,	0	} ,

	{ "$0.PRO_PreTreatDM"			, _K_D_IO  ,	PRO_PreTreatDM			,	0	} ,

	{ "TM.SigTwrBuzzerDM"			, _K_D_IO  ,	SigTwrBuzzerDM			,	0	} ,	
	//20081021
	{ "DRY_CONTROL_OPTION"			, _K_D_IO  ,	DRY_CONTROL_OPTION  	,	0	} ,	

	// 2006.12.16 by ycchoi < 1.0.0.9 >
	{ "MANUAL_LOG"					, _K_F_IO  ,	MANUAL_LOG				,	0	} ,

	// 2012.05.xx Idle Dry maint option Added
	{ "IDLE_MAINT_DRY_OPTION"		, _K_D_IO  ,	IDLE_MAINT_DRY_OPTION	,	0	} ,

	{ "CTC.AutoLeakCheckSts"		, _K_D_IO	,	LeakCheckSts			,	0	} ,

	""
};

extern "C" void Screen_Nav_Info( int no )
{
}

extern "C" BOOL Program_Init_Code()
{
	if (NULL == g_pCModuleConditionControl_Client)
	{
		switch(g_nCleanType)
		{
		case CType1_MAINT_CLEAN:
			g_pCModuleConditionControl_Client = new CModuleConditionControl_Client(); break;
		case CType2_ONLINE_CLEAN:
			g_pCModuleConditionControl_Client = new CModuleConditionControl_Client_Type2(); break;
		}
	}
	return TRUE;
}

extern "C" void Event_Message_Received()
{
}

#define CONFIG_FILE_NAME ".\\Function\\Function_Module_CTC\\OBJ_Chamber_Condition_Control\\ChamberConditonControl.ini"
// 2007.02.23 by ycchoi < 1.0.0.7 >
BOOL g_bDryCleanBuzzerOn = FALSE;
BOOL g_bDryCleanMsgBoxOn = FALSE;
BOOL g_bAutoDryClean = TRUE;		// Dry Cleaning 조건이 충족 될때(Dry cleaning wafer Count Over alarm Post상태)
									// 해당 Chamber를 Maint상태로 전환 후, Chamber Contiion control Screen에 설정된
									// Dry Cleaning Recipe를 자동 진행 할 것인지를 구분하기 위한 Flag
									// (** Default : TRUE)
// 2007.03.28 by ycchoi < 1.0.0.8 >
// Dry, Wet Cleaning 진행 후 Chamber Enable Mode로 자동 전환시에 ENABLE MAINT MODE로 전환 할 것인지 확인 Flag
BOOL g_bEnableMaintMode = FALSE;
//2007.12.6 by mgsong Dry Clean Composite call by Manual or not

//------------------------------------------------------------------------------------------
// Dry Cleaning Configuration Reading
void ParseDryCleaningConfig(char *szData)
{
	char szItem[40];
	CTextParser tp(szData);

	tp.GetWord(szItem);
	if(strcmp(szItem, "BuzzerEnable") == 0)					tp.GetBOOL(&g_bDryCleanBuzzerOn);
	else if(strcmp(szItem, "MessageBoxEnable") == 0)		tp.GetBOOL(&g_bDryCleanMsgBoxOn);
	else if(strcmp(szItem, "AutoDryCleanEnable") == 0)		tp.GetBOOL(&g_bAutoDryClean);
	else if(strcmp(szItem, "EnableMaintMode") == 0)			tp.GetBOOL(&g_bEnableMaintMode);
}


//------------------------------------------------------------------------------------------
// Configuration File Parsing
BOOL ReadConfiguration()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;
	CTextParser tp;

	do {
		fp = fopen(CONFIG_FILE_NAME, "rt");
		if(fp == NULL) break;

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				tp.SetText(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("Dry Cleaning Configuration", szItem) == 0)	nTitle = 1;
				else																nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseDryCleaningConfig(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);

	#ifdef _DEBUG
	printf("-----> BUZZER:%d, MSGBOX:%d, AUTODRY:%d, EnableMaint:%d\n",
			g_bDryCleanBuzzerOn, g_bDryCleanMsgBoxOn, g_bAutoDryClean, g_bEnableMaintMode);
	#endif
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s", CONFIG_FILE_NAME);
	}
	return bRet;
}


extern "C" void Program_Enter_Code()
{
	char szTemp[40];
	CTextParser txtP;

	// 2006.10.18
	txtP.SetText(PROGRAM_ARGUMENT_READ());
	txtP.GetWord(szTemp);
	if(strncmp(szTemp, "TYPE", 4) == 0)
	{
		if(szTemp[4] == '1') g_nCleanType = CType1_MAINT_CLEAN;
		else if(szTemp[4] == '2') g_nCleanType = CType2_ONLINE_CLEAN;
		txtP.GetWord(szTemp);
	}

	// module name
	REPLACE_CHAR_SET(0, szTemp);
	g_strModuleName = szTemp;

	// alarm base code
	txtP.GetInt(&g_nALARM_BASE);

	// Reading Config File
	if(!ReadConfiguration())
	{
		printf("-----> Chamber Condition Control  Config File Read Fail !\n");
	}

	printf("-----> CCControl Config [Buzzer:%d][MsgBox:%d][AutoDryClean:%d][EnableMaint:%d]\n", 
		g_bDryCleanBuzzerOn, g_bDryCleanMsgBoxOn, g_bAutoDryClean, g_bEnableMaintMode);
}

Module_Status Program_Main()
{
	g_pCModuleConditionControl_Client->MsgParser(PROGRAM_PARAMETER_READ());

	return SYS_SUCCESS;
}
