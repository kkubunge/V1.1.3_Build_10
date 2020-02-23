// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CMAINBMOVERPRESSURECONTROL_406D44CB03C7_INCLUDED
#define _INC_CMAINBMOVERPRESSURECONTROL_406D44CB03C7_INCLUDED

#include "stdafx.h"
#include "COverPressureControl.h"

class CMainBMOverPressureControl 
: public COverPressureControl
{
private:
	BOOL IsVacuum();
	void CloseOverPressureValve();
	void OpenOverPressureValve();

protected:
	virtual BOOL Operate(string strParameter);
};

#endif /* _INC_CMAINBMOVERPRESSURECONTROL_406D44CB03C7_INCLUDED */
