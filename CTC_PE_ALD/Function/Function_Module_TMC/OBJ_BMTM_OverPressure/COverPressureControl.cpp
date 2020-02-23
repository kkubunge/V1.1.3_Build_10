// Copyright (C) IPS Ltd.

#include "COverPressureControl.h"



const string& COverPressureControl::get_ModuleName() const
{
	return m_strModuleName;
}

void COverPressureControl::set_ModuleName(string value)
{
	m_strModuleName = value;
	return;
}



COverPressureControl::COverPressureControl()
{
	// ToDo: Add your specialized code here and/or call the base class
}

