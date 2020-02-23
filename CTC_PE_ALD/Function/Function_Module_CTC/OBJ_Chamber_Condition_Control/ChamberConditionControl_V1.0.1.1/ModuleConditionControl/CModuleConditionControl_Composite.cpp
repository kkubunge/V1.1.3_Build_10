// // Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CModuleConditionControl_Composite.h"

// 2007.03.28 by ycchoi < 1.0.0.8 >
extern BOOL g_bEnableMaintMode;
extern BOOL g_bAutoRun;

BOOL CModuleConditionControl_Composite::Run(BOOL bAutoControl)
{
	BOOL bRet = FALSE;
	do
	{
		vector<CModuleConditionControl_Component*>::iterator theIter;
		for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
		{
			(*theIter)->ResetRunningStatus();
		}
		for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
		{
			if (TRUE == (*theIter)->GetAutoControl())
				break;
		}
		if (theIter == m_vecComponent.end())
			break;

		if (TRUE == m_bModuleModeToDisableBeforeRun)
		{
			string strEvent = "SET_MODULE_INFO_LOCAL ";
			strEvent += g_strModuleName;
			strEvent += "|DISABLE";
			WRITE_FUNCTION_EVENT(SCHEDULER , (char*)strEvent.c_str());
		} 

		WRITE_DIGITAL(m_nBaseChannelID, __SEQUENCE_RUNNING, &g_nCommStatus);
		for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
		{
			if (TRUE == (*theIter)->GetAutoControl())
			{
				if (FALSE == (*theIter)->Run(bAutoControl))
					break;
			}
		}
		WRITE_DIGITAL(m_nBaseChannelID, __SEQUENCE_IDLE, &g_nCommStatus);
		if (theIter != m_vecComponent.end())
			break;
		if (TRUE == m_bModuleModeToEnableAfterRun)
		{
			string strEvent = "SET_MODULE_INFO_LOCAL ";
			strEvent += g_strModuleName;
			// 2007.03.28 by ycchoi < 1.0.0.8 >
			// strEvent += "|ENABLE";
			if(g_bEnableMaintMode)	strEvent += "|ENABLEN";
			else					strEvent += "|ENABLE";
			WRITE_FUNCTION_EVENT(SCHEDULER , (char*)strEvent.c_str());
		}
		bRet = TRUE;
	} while(0);

	return bRet;
}

void CModuleConditionControl_Composite::Abort()
{
	WRITE_DIGITAL(m_nBaseChannelID, __SEQUENCE_ABORTED, &g_nCommStatus);
	vector<CModuleConditionControl_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->Abort();
	}
}

void CModuleConditionControl_Composite::AddComponent(CModuleConditionControl_Component* pComponent)
{
	if (m_vecComponent.end() == find(m_vecComponent.begin(), m_vecComponent.end(), pComponent))
		m_vecComponent.push_back(pComponent);
}

void CModuleConditionControl_Composite::RemoveComponent(CModuleConditionControl_Component* pComponent)
{
	if (m_vecComponent.end() != find(m_vecComponent.begin(), m_vecComponent.end(), pComponent))
	{
		vector<CModuleConditionControl_Component*>::iterator theIterRemove = remove(m_vecComponent.begin(), m_vecComponent.end(), pComponent);
		m_vecComponent.erase(theIterRemove, m_vecComponent.end());
	}
}

BOOL CModuleConditionControl_Composite::GetRunningStatus()
{
	vector<CModuleConditionControl_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		if (TRUE == (*theIter)->GetRunningStatus())
		{
			if (__SEQUENCE_ABORTED != READ_DIGITAL(m_nBaseChannelID, &g_nCommStatus))
				WRITE_DIGITAL(m_nBaseChannelID, __SEQUENCE_RUNNING, &g_nCommStatus);
			return TRUE;
		}
	}
	WRITE_DIGITAL(m_nBaseChannelID, __SEQUENCE_IDLE, &g_nCommStatus);
	return FALSE;
}

void CModuleConditionControl_Composite::SetModuleModeToEnableAfterRun(BOOL bEnable)
{
	m_bModuleModeToEnableAfterRun = bEnable;
	WRITE_DIGITAL(m_nBaseChannelID+1, (int)bEnable, &g_nCommStatus);
}

void CModuleConditionControl_Composite::SetAutoControl(BOOL bEnable, int nIndex)
{
	vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin()+nIndex;
	(*theIter)->SetAutoControl(bEnable, nIndex);
}

void CModuleConditionControl_Composite::ClearRecipe(int nComposite , int nLeaf)
{
	vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin()+nLeaf;
	(*theIter)->ClearRecipe(nComposite, nLeaf);
}

CModuleConditionControl_Composite::~CModuleConditionControl_Composite()
{
}

CModuleConditionControl_Composite::CModuleConditionControl_Composite(BOOL bModuleModeToDisableBeforeRun) : m_doubWaitingTime(-1)
{
	m_bModuleModeToDisableBeforeRun = bModuleModeToDisableBeforeRun;
}

double CModuleConditionControl_Composite::GetWaitingTime()
{
	//2007.07.04 as of today member updated not by function call but io value read directly
	if ((IDLE_CONTROL_COMPOSITE_TYPE == m_nType) || (PRE_TREATMENT_COMPOSITE_TYPE == m_nType))
	{
		int nDay  = READ_DIGITAL(m_nBaseChannelID+2, &g_nCommStatus);
		int nHour = READ_DIGITAL(m_nBaseChannelID+3, &g_nCommStatus);
		int nMin  = READ_DIGITAL(m_nBaseChannelID+4, &g_nCommStatus);
		int nSec  = READ_DIGITAL(m_nBaseChannelID+5, &g_nCommStatus);
		m_doubWaitingTime = (nDay*24*60*60) + (nHour*60*60) + (nMin*60) + nSec;
	}
	
	return m_doubWaitingTime;
}

void CModuleConditionControl_Composite::Initialize(int nType, int nBaseChannelID, int nBaseAlarmID)
{
	CModuleConditionControl_Component::Initialize(nType, nBaseChannelID, nBaseAlarmID);

	m_bModuleModeToEnableAfterRun = READ_DIGITAL(m_nBaseChannelID+1, &g_nCommStatus);
	if ((IDLE_CONTROL_COMPOSITE_TYPE == m_nType) || (PRE_TREATMENT_COMPOSITE_TYPE == m_nType))
	{
		int nDay  = READ_DIGITAL(m_nBaseChannelID+2, &g_nCommStatus);
		int nHour = READ_DIGITAL(m_nBaseChannelID+3, &g_nCommStatus);
		int nMin  = READ_DIGITAL(m_nBaseChannelID+4, &g_nCommStatus);
		int nSec  = READ_DIGITAL(m_nBaseChannelID+5, &g_nCommStatus);
		m_doubWaitingTime = (nDay*24*60*60) + (nHour*60*60) + (nMin*60) + nSec;
	}
}

//2007.07.04 may be not used from now
//////////////////////////////////////////////////////////////////////////////////////////
void CModuleConditionControl_Composite::ChangeCurrentValue()
{
	vector<CModuleConditionControl_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->ChangeCurrentValue();
	}
	if ((IDLE_CONTROL_COMPOSITE_TYPE == m_nType) || (PRE_TREATMENT_COMPOSITE_TYPE == m_nType))
	{
		int nDay = READ_DIGITAL(m_nBaseChannelID+2, &g_nCommStatus);
		int nHour = READ_DIGITAL(m_nBaseChannelID+3, &g_nCommStatus);
		int nMin = READ_DIGITAL(m_nBaseChannelID+4, &g_nCommStatus);
		int nSec = READ_DIGITAL(m_nBaseChannelID+5, &g_nCommStatus);
		m_doubWaitingTime = (nDay*24*60*60) + (nHour*60*60) + (nMin*60) + nSec;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CModuleConditionControl_Composite::GetAutoControl()
{
	BOOL bRet = FALSE;
	do
	{
		vector<CModuleConditionControl_Component*>::iterator theIter;
		for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
		{
			if (TRUE == (*theIter)->GetAutoControl())
				break;
		}
		if (theIter == m_vecComponent.end())
			break;

		bRet = TRUE;
	}while(0);

	return bRet;
}
