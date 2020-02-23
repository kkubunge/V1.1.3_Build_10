// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CTMOVERPRESSURECONTROL_406D44E40070_INCLUDED
#define _INC_CTMOVERPRESSURECONTROL_406D44E40070_INCLUDED

#include "stdafx.h"
#include "COverPressureControl.h"

class CTMOverPressureControl 
: public COverPressureControl
{
private:
	string ExtractRunString(string strParameter);
	BOOL IsVacuum();
	void CloseOverPressureValve();
	void CloseRoughingValve();
	void OpenOverPressureValve();
	void OpenRoughingValve();

protected:
	virtual BOOL Operate(string strParameter);
};

#endif /* _INC_CTMOVERPRESSURECONTROL_406D44E40070_INCLUDED */
