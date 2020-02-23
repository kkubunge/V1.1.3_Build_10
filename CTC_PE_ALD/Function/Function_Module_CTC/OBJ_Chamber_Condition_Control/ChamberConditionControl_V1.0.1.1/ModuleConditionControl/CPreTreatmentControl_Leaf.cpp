// // Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CPreTreatmentControl_Leaf.h"



void CPreTreatmentControl_Leaf::SetAutoControl(BOOL bEnable, int nIndex)
{
	m_bAutoControl = bEnable;
	WRITE_DIGITAL(m_nBaseChannelID, (int)bEnable, &g_nCommStatus);
	if (FALSE == m_bAutoControl)
		WRITE_DIGITAL(PRO_PreTreatDM, __OFF, &g_nCommStatus);
}

void CPreTreatmentControl_Leaf::ClearRecipe(int nComposite , int nLeaf)
{
	return;
}

void CPreTreatmentControl_Leaf::Initialize(int nType, int nBaseChannelID, int nBaseAlarmID)
{
	CModuleConditionControl_Component::Initialize(nType, nBaseChannelID, nBaseAlarmID);

	m_bAutoControl = READ_DIGITAL(m_nBaseChannelID+0, &g_nCommStatus);
}

BOOL CPreTreatmentControl_Leaf::GetAutoControl()
{
	return m_bAutoControl;
}

