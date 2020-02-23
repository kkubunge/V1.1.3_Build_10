// // Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CLeakCheckControl_Leaf.h"



void CLeakCheckControl_Leaf::Abort()
{
	if (TRUE ==  GetRunningStatus())
	{
		RUN_FUNCTION_ABORT(SCHEDULER_MAINT);
	}
}

BOOL CLeakCheckControl_Leaf::GetRunningStatus()
{
	if (__SEQUENCE_RUNNING == m_nRunningStatus)
		return TRUE;
	else
		return FALSE;
}

void CLeakCheckControl_Leaf::Initialize(int nType, int nBaseChannelID, int nBaseAlarmID)
{
	CModuleConditionControl_Component::Initialize(nType, nBaseChannelID, nBaseAlarmID);
	m_nRunningStatus = __SEQUENCE_IDLE;

	m_bAutoControl = READ_DIGITAL(m_nBaseChannelID+0, &g_nCommStatus);
}

BOOL CLeakCheckControl_Leaf::Run(BOOL bAutoControl)
{
	if ((TRUE == bAutoControl) && (FALSE == m_bAutoControl))
		return TRUE;

	m_nRunningStatus = __SEQUENCE_RUNNING;
	WRITE_DIGITAL(m_nBaseChannelID+2, m_nRunningStatus, &g_nCommStatus);

	Module_Status enumModuleStatus = RUN_FUNCTION(SCHEDULER_MAINT, "LEAK_CHECK");
	if (SYS_SUCCESS == enumModuleStatus)
	{
		m_nRunningStatus = __SEQUENCE_IDLE;
		WRITE_DIGITAL(m_nBaseChannelID+2, m_nRunningStatus, &g_nCommStatus);
		return TRUE;
	}
	else
	{
		m_nRunningStatus = __SEQUENCE_ABORTED;
		WRITE_DIGITAL(m_nBaseChannelID+2, m_nRunningStatus, &g_nCommStatus);
		if (FALSE == bAutoControl)
		{
			if (ALM_PAUSED != ALARM_STATUS(ALARM__LEAK_CHECK_FAILED))
				ALARM_POST(ALARM__LEAK_CHECK_FAILED);
		}
		return FALSE;
	}
}

void CLeakCheckControl_Leaf::SetAutoControl(BOOL bEnable, int nIndex)
{
	m_bAutoControl = bEnable;
	WRITE_DIGITAL(m_nBaseChannelID, (int)bEnable, &g_nCommStatus);
}

BOOL CLeakCheckControl_Leaf::GetAutoControl()
{
	return m_bAutoControl;
}

void CLeakCheckControl_Leaf::ClearRecipe(int nComposite , int nLeaf)
{
	return;
}
