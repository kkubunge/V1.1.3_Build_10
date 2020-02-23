// // Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPRETREATMENTCONTROL_LEAF_414FD34D0261_INCLUDED
#define _INC_CPRETREATMENTCONTROL_LEAF_414FD34D0261_INCLUDED

#include "CModuleConditionControl_Component.h"

class CPreTreatmentControl_Leaf 
: public CModuleConditionControl_Component
{
public:
	virtual BOOL GetAutoControl();

	virtual void Initialize(int nType, int nBaseChannelID, int nBaseAlarmID);

	virtual void SetAutoControl(BOOL bEnable, int nIndex);

	virtual void ClearRecipe(int nComposite , int nLeaf);

private:
	BOOL m_bAutoControl;

};

#endif /* _INC_CPRETREATMENTCONTROL_LEAF_414FD34D0261_INCLUDED */
