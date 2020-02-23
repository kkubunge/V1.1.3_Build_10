// // Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CLEAKCHECKCONTROL_LEAF_414FD33303C8_INCLUDED
#define _INC_CLEAKCHECKCONTROL_LEAF_414FD33303C8_INCLUDED

#include "CModuleConditionControl_Component.h"

class CLeakCheckControl_Leaf 
: public CModuleConditionControl_Component
{
public:
	virtual BOOL GetAutoControl();

	virtual void SetAutoControl(BOOL bEnable, int nIndex);

	virtual BOOL Run(BOOL bAutoControl);

	virtual void Initialize(int nType, int nBaseChannelID, int nBaseAlarmID);

	virtual void Abort();

	virtual BOOL GetRunningStatus();

	virtual void ClearRecipe(int nComposite , int nLeaf);

private:
	BOOL m_bAutoControl;

	int m_nRunningStatus;

};

#endif /* _INC_CLEAKCHECKCONTROL_LEAF_414FD33303C8_INCLUDED */
