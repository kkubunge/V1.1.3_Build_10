// // Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CMODULECONDITIONCONTROL_COMPOSITE_4146958F03C8_INCLUDED
#define _INC_CMODULECONDITIONCONTROL_COMPOSITE_4146958F03C8_INCLUDED

#include "CModuleConditionControl_Component.h"

class CModuleConditionControl_Composite 
: public CModuleConditionControl_Component
{
public:

	virtual BOOL GetAutoControl();
	
	virtual void ChangeCurrentValue();

	virtual BOOL Run(BOOL bAutoControl);

	virtual void Initialize(int nType, int nBaseChannelID, int nBaseAlarmID);

	virtual double GetWaitingTime();

	CModuleConditionControl_Composite(BOOL bModuleModeToDisableBeforeRun);

	virtual ~CModuleConditionControl_Composite();

	virtual void Abort();

	virtual void AddComponent(CModuleConditionControl_Component* pComponent);

	virtual void RemoveComponent(CModuleConditionControl_Component* pComponent);

	virtual BOOL GetRunningStatus();

	virtual void SetModuleModeToEnableAfterRun(BOOL bEnable);

	virtual void SetAutoControl(BOOL bEnable, int nIndex);

	virtual void ClearRecipe(int nComposite , int nLeaf);

private:
	double m_doubWaitingTime;

	BOOL m_bModuleModeToDisableBeforeRun;

	BOOL m_bModuleModeToEnableAfterRun;

	vector<CModuleConditionControl_Component*> m_vecComponent;

};

#endif /* _INC_CMODULECONDITIONCONTROL_COMPOSITE_4146958F03C8_INCLUDED */
