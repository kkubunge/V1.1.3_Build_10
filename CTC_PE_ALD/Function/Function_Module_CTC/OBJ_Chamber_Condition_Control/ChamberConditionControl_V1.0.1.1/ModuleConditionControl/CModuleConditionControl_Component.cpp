// // Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CModuleConditionControl_Component.h"



void CModuleConditionControl_Component::AddComponent(CModuleConditionControl_Component* pComponent)
{
	// TODO: Add your specialized code here.
}

void CModuleConditionControl_Component::RemoveComponent(CModuleConditionControl_Component* pComponent)
{
	// TODO: Add your specialized code here.
}

void CModuleConditionControl_Component::SetAutoControl(BOOL bEnable, int nIndex)
{
	// TODO: Add your specialized code here.
}

void CModuleConditionControl_Component::ClearRecipe(int nComposite , int nLeaf)
{
	// TODO: Add your specialized code here.
}

void CModuleConditionControl_Component::SetModuleModeToEnableAfterRun(BOOL bEnable)
{
	// TODO: Add your specialized code here.
}

void CModuleConditionControl_Component::Abort()
{
	// TODO: Add your specialized code here.
}

BOOL CModuleConditionControl_Component::GetRunningStatus()
{
	return FALSE;
}

CModuleConditionControl_Component::CModuleConditionControl_Component() : m_nType(-1), m_nBaseChannelID(0), m_nBaseAlarmID(0)
{
	// ToDo: Add your specialized code here and/or call the base class
}

double CModuleConditionControl_Component::GetWaitingTime()
{
	return -1;
}

void CModuleConditionControl_Component::Initialize(int nType, int nBaseChannelID, int nBaseAlarmID)
{
	m_nType = nType;
	m_nBaseChannelID = nBaseChannelID;
	m_nBaseAlarmID = nBaseAlarmID;
}

int CModuleConditionControl_Component::GetType()
{
	return m_nType;
}

BOOL CModuleConditionControl_Component::Run(BOOL bAutoControl)
{
	return FALSE;
}

BOOL CModuleConditionControl_Component::GetAutoControl()
{
	return FALSE;
}

void CModuleConditionControl_Component::ChangeCurrentValue()
{
	// TODO: Add your specialized code here.
}

void CModuleConditionControl_Component::ResetRunningStatus()
{
	WRITE_DIGITAL(m_nBaseChannelID+2, __SEQUENCE_IDLE, &g_nCommStatus);
}

