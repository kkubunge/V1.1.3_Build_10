// // Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CRecipeControl_Leaf.h"

void CRecipeControl_Leaf::Abort()
{
	if (TRUE ==  GetRunningStatus())
	{
		if (IDLE_CONTROL_LEAF_TYPE == m_nType)
			WRITE_DIGITAL(PRO_IdleRcpStsDM, 0, &g_nCommStatus);
		else
			WRITE_DIGITAL(PROCESS_RUN_STATUS, 0, &g_nCommStatus);
//		RUN_FUNCTION(ABORT_PROCESS, "");
		string strEvent = "SET_MODULE_INFO_LOCAL ";
		strEvent += g_strModuleName;
		strEvent += "|DISABLE";
		WRITE_FUNCTION_EVENT(SCHEDULER , (char*)strEvent.c_str());
	}
}

//2007.07.04 may be not used from now
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecipeControl_Leaf::ChangeCurrentValue()
{
	char szTemp[255] = "";
	READ_STRING(m_nBaseChannelID+1, (char*)szTemp, &g_nCommStatus);
	m_strRecipeName = szTemp;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CRecipeControl_Leaf::GetRunningStatus()
{
	if (__SEQUENCE_RUNNING == m_nRunningStatus)
		return TRUE;
	else
		return FALSE;
}

BOOL CRecipeControl_Leaf::Run(BOOL bAutoControl)
{
	if ((TRUE == bAutoControl) && (FALSE == m_bAutoControl))
		return TRUE;

	//2007.07.04 as of today channel updated not by function call but io read directly
	char szTemp[255] = "";
	READ_STRING(m_nBaseChannelID+1, (char*)szTemp, &g_nCommStatus);
	m_strRecipeName = szTemp;
	if ("" == m_strRecipeName)
		return TRUE;

	m_nRunningStatus = __SEQUENCE_RUNNING;
	WRITE_DIGITAL(m_nBaseChannelID+2, m_nRunningStatus, &g_nCommStatus);
	WRITE_STRING(RECIPE_FILE, (char*)m_strRecipeName.c_str(), &g_nCommStatus);

	char szEvent[256]     = {0};
	char szFuncParam[256] = {0};
    //-------------- Cleaning Start Event Add Start 2006.03.38 by Jelee. --------------//
	if(m_nType == DRY_CLEANING_CLEANING_LEAF_TYPE)         {
		sprintf(szEvent,"EVENT_ON %s_DryCleaning_Start"    , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);
	} else if(m_nType == DRY_CLEANING_LEAKCHECK_LEAF_TYPE) {
		sprintf(szEvent,"EVENT_ON %s_Leakcheck_Start"       , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);		
	} else if(m_nType == DRY_CLEANING_DRYPOST_LEAF_TYPE)   {
		sprintf(szEvent,"EVENT_ON %s_DryPost_Start"         , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);			
	} else if(m_nType == WET_CLEANING_LEAF_TYPE)           {
		sprintf(szEvent,"EVENT_ON %s_WetCleaning_Start"     , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);		
	} else if(m_nType == WET_CLEANING_POST_LEAF_TYPE)      {
		//2007.07.04 Added New Leaf Type
		sprintf(szEvent,"EVENT_ON %s_WetCleaning_Post_Start" , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);		
	} else if(m_nType == WET_BACKUP_LEAF_TYPE)             {
		sprintf(szEvent,"EVENT_ON %s_Wet_Backup_Start"       , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);		
	} else if(m_nType == WET_BACKUP_POST_LEAF_TYPE)        {
		sprintf(szEvent,"EVENT_ON %s_Wet_Backup_Post_Start"  , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);		
	} else if(m_nType == ERROR_BACKUP_LEAF_TYPE)           {
		sprintf(szEvent,"EVENT_ON %s_Error_Backup_Start"     , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);		
	} else if(m_nType == ERROR_BACKUP_POST_LEAF_TYPE)      {
		sprintf(szEvent,"EVENT_ON %s_Error_Backup_Post_Start" , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);		
	} else if(m_nType == ERROR_BACKUP2_LEAF_TYPE)          {
		sprintf(szEvent,"EVENT_ON %s_Error_Backup2_Start"     , g_strModuleName.c_str());
		WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);	
		//End 2007.07.04
	} else {;}
	//-------------- Cleaning Start Event Add End 2006.03.38 by Jelee. --------------//

	Module_Status enumModuleStatus = RUN_FUNCTION(SCHEDULER_MAINT, "PROCESS");
	if (m_nType != DRY_CLEANING_LEAKCHECK_LEAF_TYPE) Sleep(2000);
	if (SYS_SUCCESS == enumModuleStatus) {
		m_nRunningStatus = __SEQUENCE_IDLE;
		WRITE_DIGITAL(m_nBaseChannelID+2, m_nRunningStatus, &g_nCommStatus);
        //-------------- Cleaning Completed Event Add Start 2006.03.38 by Jelee. --------------//
		memset(szEvent , 0 , sizeof(szEvent));
		if (m_nType == DRY_CLEANING_CLEANING_LEAF_TYPE)        {
			sprintf(szEvent     , "EVENT_ON %s_DryCleaning_Completed"    , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_DryCleaning_Completed"             , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);			
		} else if(m_nType == DRY_CLEANING_LEAKCHECK_LEAF_TYPE) {
			sprintf(szEvent     , "EVENT_ON %s_Leakcheck_Completed"       , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_Leakcheck_Completed"                , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);			
		} else if(m_nType == DRY_CLEANING_DRYPOST_LEAF_TYPE)   {
			sprintf(szEvent     , "EVENT_ON %s_DryPost_Completed"         , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_DryPost_Completed"                  , g_strModuleName.c_str());			
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);
		} else if(m_nType == WET_CLEANING_LEAF_TYPE)           {
			sprintf(szEvent     , "EVENT_ON %s_WetCleaning_Completed"     , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_WetCleaning_Completed"              , g_strModuleName.c_str());			
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);			
		} else if(m_nType == WET_CLEANING_POST_LEAF_TYPE)      {
			//2007.07.04 Added New Leaf Type
			sprintf(szEvent     , "EVENT_ON %s_WetCleaning_Post_Completed" , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_WetCleaning_Post_Completed"          , g_strModuleName.c_str());				
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);			
		} else if(m_nType == WET_BACKUP_LEAF_TYPE)             {
			sprintf(szEvent     , "EVENT_ON %s_Wet_Backup_Completed"       , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_Wet_Backup_Completed"                , g_strModuleName.c_str());			
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);	
		} else if(m_nType == WET_BACKUP_POST_LEAF_TYPE)        {
			sprintf(szEvent     , "EVENT_ON %s_Wet_Backup_Post_Completed"  , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_Wet_Backup_Post_Completed"           , g_strModuleName.c_str());			
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);				
		} else if(m_nType == ERROR_BACKUP_LEAF_TYPE)           {
			sprintf(szEvent     , "EVENT_ON %s_Error_Backup_Completed"      , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_Error_Backup_Completed"               , g_strModuleName.c_str());				
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);			
		} else if(m_nType == ERROR_BACKUP_POST_LEAF_TYPE)      {
			sprintf(szEvent     , "EVENT_ON %s_Error_Backup_Post_Completed" , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_Error_Backup_Post_Completed"          , g_strModuleName.c_str());				
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);			
		} else if(m_nType == ERROR_BACKUP2_LEAF_TYPE)          {
			sprintf(szEvent     , "EVENT_ON %s_Error_Backup2_Completed"    , g_strModuleName.c_str());
			sprintf(szFuncParam , "%s_Error_Backup2_Completed"             , g_strModuleName.c_str());				
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			WRITE_FUNCTION_EVENT(MANUAL_LOG, szFuncParam);		
			//End 2007.07.04
		} else {;}
		//-------------- Cleaning Completed Event Add End 2006.03.38 by Jelee. --------------//
		return TRUE;
	} else {
		Abort();	// For Chamber mode change
		m_nRunningStatus = __SEQUENCE_ABORTED;
		WRITE_DIGITAL(m_nBaseChannelID+2, m_nRunningStatus, &g_nCommStatus);
		if ((FALSE == bAutoControl) || (IDLE_CONTROL_LEAF_TYPE != enumModuleStatus)) {
			if (ALM_PAUSED != ALARM_STATUS(ALARM__RECIPE_RUN_FAILED))
				ALARM_POST(ALARM__RECIPE_RUN_FAILED);
		}
		
		//-------------- Cleaning Aborted Event Add Start 2006.03.38 by Jelee. --------------//
		if (m_nType == DRY_CLEANING_CLEANING_LEAF_TYPE) {
			sprintf(szEvent , "EVENT_ON %s_DryCleaning_Aborted"    , g_strModuleName.c_str());
			WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);			
		} else if(m_nType == DRY_CLEANING_LEAKCHECK_LEAF_TYPE) {
			sprintf(szEvent , "EVENT_ON %s_Leakcheck_Aborted"       , g_strModuleName.c_str());
			WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);			
		} else if(m_nType == DRY_CLEANING_DRYPOST_LEAF_TYPE)   {
			sprintf(szEvent , "EVENT_ON %s_DryPost_Aborted"          , g_strModuleName.c_str());
			WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);				
		} else if(m_nType == WET_CLEANING_LEAF_TYPE)           {
			sprintf(szEvent , "EVENT_ON %s_WetCleaning_Aborted"      , g_strModuleName.c_str());
			WRITE_STRING(SCHEDULER2, szEvent , &g_nCommStatus);			
		} else if(m_nType == WET_CLEANING_POST_LEAF_TYPE)      {
			//2007.07.04 Added New Leaf Type
			sprintf(szEvent , "EVENT_ON %s_WetCleaning_Post_Aborted" , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
		} else if(m_nType == WET_BACKUP_LEAF_TYPE)             {
			sprintf(szEvent , "EVENT_ON %s_Wet_Backup_Aborted"       , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
		} else if(m_nType == WET_BACKUP_POST_LEAF_TYPE)        {
			sprintf(szEvent , "EVENT_ON %s_Wet_Backup_Post_Aborted"  , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
		} else if(m_nType == ERROR_BACKUP_LEAF_TYPE)           {
			sprintf(szEvent , "EVENT_ON %s_Error_Backup_Aborted"      , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
		} else if(m_nType == ERROR_BACKUP_POST_LEAF_TYPE)      {
			sprintf(szEvent , "EVENT_ON %s_Error_Backup_Post_Aborted" , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
		} else if(m_nType == ERROR_BACKUP2_LEAF_TYPE)          {
			sprintf(szEvent , "EVENT_ON %s_Error_Backup2_Aborted"    , g_strModuleName.c_str());
			WRITE_STRING        (SCHEDULER2, szEvent , &g_nCommStatus);
			//End 2007.07.04
		} else {;}
		//-------------- Cleaning Aborted Event Add End 2006.03.38 by Jelee. --------------//
		return FALSE;
	}
}

void CRecipeControl_Leaf::SetAutoControl(BOOL bEnable, int nIndex)
{
	m_bAutoControl = bEnable;
	WRITE_DIGITAL(m_nBaseChannelID, (int)bEnable, &g_nCommStatus);
}

void CRecipeControl_Leaf::ClearRecipe(int nComposite , int nLeaf)
{	
	m_strRecipeName = "";
	WRITE_STRING(m_nBaseChannelID+1, "" , &g_nCommStatus);
}

void CRecipeControl_Leaf::Initialize(int nType, int nBaseChannelID, int nBaseAlarmID)
{
	CModuleConditionControl_Component::Initialize(nType, nBaseChannelID, nBaseAlarmID);
	m_strRecipeName = "";
	m_nRunningStatus = __SEQUENCE_IDLE;

	m_bAutoControl = READ_DIGITAL(m_nBaseChannelID+0, &g_nCommStatus);
	char szTemp[255] = "";
	READ_STRING(m_nBaseChannelID+1, (char*)szTemp, &g_nCommStatus);
	m_strRecipeName = szTemp;
}

BOOL CRecipeControl_Leaf::GetAutoControl()
{
	return m_bAutoControl;
}

