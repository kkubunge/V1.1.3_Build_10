// // Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CMODULECONDITIONCONTROL_COMPONENT_4146957B0271_INCLUDED
#define _INC_CMODULECONDITIONCONTROL_COMPONENT_4146957B0271_INCLUDED

class CModuleConditionControl_Component 
{
private:

protected:
	int m_nBaseAlarmID;

	int m_nBaseChannelID;

	int m_nType;
	CModuleConditionControl_Component();

public:
	virtual void ResetRunningStatus();

	virtual void ChangeCurrentValue();

	virtual BOOL GetAutoControl();

	virtual BOOL Run(BOOL bAutoControl);

	int GetType();

	virtual void Initialize(int nType, int nBaseChannelID, int nBaseAlarmID);

	virtual double GetWaitingTime();

	virtual void AddComponent(CModuleConditionControl_Component* pComponent);

	virtual void RemoveComponent(CModuleConditionControl_Component* pComponent);

	virtual void SetAutoControl(BOOL bEnable, int nIndex);

	virtual void ClearRecipe(int nComposite , int nLeaf);

	virtual void SetModuleModeToEnableAfterRun(BOOL bEnable);

	virtual void Abort();

	virtual BOOL GetRunningStatus();

};

#endif /* _INC_CMODULECONDITIONCONTROL_COMPONENT_4146957B0271_INCLUDED */
