// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPREVENTIVEMAINTENANCE_COMPOSITE_40C070A901F4_INCLUDED
#define _INC_CPREVENTIVEMAINTENANCE_COMPOSITE_40C070A901F4_INCLUDED

#include "CPreventiveMaintenance_Component.h"

class CPreventiveMaintenance_Composite 
: public CPreventiveMaintenance_Component
{
	enum {PROCESS_EMABLE , DRY_RESERVED , DRY_RUNNING};
public:
	void InitializeChannelInformation(int nResetCountID, int nResetTimeID, int nResetHistoryBaseID, int nAlarmChannelID);

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	void InitializeChannelInformation(int nResetCountID, int nResetTimeID, int nResetHistoryBaseID, int nResetWfrCntHistoryBaseID, int nAlarmChannelID);

	virtual void AddComponent(CPreventiveMaintenance_Component* pComponent);

	virtual void RemoveComponent(CPreventiveMaintenance_Component* pComponent);

	virtual int GetType();

	virtual void EnableMonitoring(BOOL bEnable, int nLeafType);

	virtual void ResetCurrentValue();

	virtual int ChangeCurrentValue(void* pData, int nLeafType);

	virtual void RefreshWarningAlarmValue();

	CPreventiveMaintenance_Composite(int nCompositeType);

	// added by ycchoi 2006.05.28 < V 1,0.0.2 >
	int GetCurrentWaferCount();

	// added by ycchoi 2006.11.06 < V 1.0.0.6 >
	virtual void RefreshCurrentValue();

private:
	int m_nAlarmChannelID;

	int m_nAlarmStatus;

	int m_nResetCountChannelID;

	int m_nResetTimeChannelID;

	int m_nResetHistoryBaseChannelID;

	int m_nType;

	vector<CPreventiveMaintenance_Component*> m_vecComponent;

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	int m_nResetWfrCntHistoryBaseChannelID;

};

#endif /* _INC_CPREVENTIVEMAINTENANCE_COMPOSITE_40C070A901F4_INCLUDED */
