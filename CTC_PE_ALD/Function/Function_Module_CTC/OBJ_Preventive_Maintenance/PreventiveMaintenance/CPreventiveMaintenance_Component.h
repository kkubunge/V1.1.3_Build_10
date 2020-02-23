// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPREVENTIVEMAINTENANCE_COMPONENT_40C07083006D_INCLUDED
#define _INC_CPREVENTIVEMAINTENANCE_COMPONENT_40C07083006D_INCLUDED

class CPreventiveMaintenance_Component 
{
public:
	virtual void AddComponent(CPreventiveMaintenance_Component* pComponent);

	virtual void RemoveComponent(CPreventiveMaintenance_Component* pComponent);

	virtual int GetType();

	virtual void EnableMonitoring(BOOL bEnable, int nLeafType);

	virtual void ResetCurrentValue();

	virtual int ChangeCurrentValue(void* pData, int nLeafType);

	virtual void RefreshWarningAlarmValue();

	// added by ycchoi 2006.11.06 < V 1.0.0.6 >
	virtual void RefreshCurrentValue();

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	virtual int GetCurrentWaferCount();

	// 2006.10.18
	virtual bool IsPostRunTime(int nType);

protected:
	CPreventiveMaintenance_Component();

};

#endif /* _INC_CPREVENTIVEMAINTENANCE_COMPONENT_40C07083006D_INCLUDED */
