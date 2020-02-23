// // Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CRECIPECONTROL_LEAF_414695800203_INCLUDED
#define _INC_CRECIPECONTROL_LEAF_414695800203_INCLUDED

#include "CModuleConditionControl_Component.h"

class CRecipeControl_Leaf 
: public CModuleConditionControl_Component
{
public:
	virtual BOOL GetAutoControl();

	virtual BOOL Run(BOOL bAutoControl);

	virtual void Initialize(int nType, int nBaseChannelID, int nBaseAlarmID);

	virtual void Abort();

	virtual void ChangeCurrentValue();

	virtual BOOL GetRunningStatus();

	virtual void SetAutoControl(BOOL bEnable, int nIndex);

	virtual void ClearRecipe(int nComposite , int nLeaf);

private:
	BOOL m_bAutoControl;

	string m_strRecipeName;

	int m_nRunningStatus;

};

#endif /* _INC_CRECIPECONTROL_LEAF_414695800203_INCLUDED */
