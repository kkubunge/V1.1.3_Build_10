// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CPreventiveMaintenance_Component.h"



void CPreventiveMaintenance_Component::RemoveComponent(CPreventiveMaintenance_Component* pComponent)
{
	// TODO: Add your specialized code here.
}

int CPreventiveMaintenance_Component::GetType()
{
	// TODO: Add your specialized code here.
	return (int)0;
}

void CPreventiveMaintenance_Component::EnableMonitoring(BOOL bEnable, int nLeafType)
{
	// TODO: Add your specialized code here.
}

void CPreventiveMaintenance_Component::ResetCurrentValue()
{
	// TODO: Add your specialized code here.
}

int CPreventiveMaintenance_Component::ChangeCurrentValue(void* pData, int nLeafType)
{
	// TODO: Add your specialized code here.
	return (int)0;
}

void CPreventiveMaintenance_Component::RefreshWarningAlarmValue()
{
	// TODO: Add your specialized code here.
}

// added by ycchoi 2006.11.06 < V 1.0.0.6 >
void CPreventiveMaintenance_Component::RefreshCurrentValue()
{
}

CPreventiveMaintenance_Component::CPreventiveMaintenance_Component()
{
	// ToDo: Add your specialized code here and/or call the base class
}



void CPreventiveMaintenance_Component::AddComponent(CPreventiveMaintenance_Component* pComponent)
{
	// TODO: Add your specialized code here.
}

int CPreventiveMaintenance_Component::GetCurrentWaferCount()
{
	return 0;
}

// 2006.10.18
bool CPreventiveMaintenance_Component::IsPostRunTime(int nType)
{
	return false;
}

