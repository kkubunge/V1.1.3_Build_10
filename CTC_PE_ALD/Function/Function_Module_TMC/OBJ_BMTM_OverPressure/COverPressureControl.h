// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_COVERPRESSURECONTROL_406D43260119_INCLUDED
#define _INC_COVERPRESSURECONTROL_406D43260119_INCLUDED

#include "stdafx.h"

class COverPressureControl 
{
public:
	virtual BOOL Operate(string strParameter) = 0;
	void set_ModuleName(string value);

private:
	string m_strModuleName;

protected:
	const string& get_ModuleName() const;
	COverPressureControl();
};

#endif /* _INC_COVERPRESSURECONTROL_406D43260119_INCLUDED */
