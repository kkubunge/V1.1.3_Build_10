// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPREVENTIVEMAINTENANCE_CLIENT_40C16FA901C5_INCLUDED
#define _INC_CPREVENTIVEMAINTENANCE_CLIENT_40C16FA901C5_INCLUDED

#include "MonThread.h"

class CPreventiveMaintenance_Component;
class CPreventiveMaintenance_Client : public CMonThread
{
public:
	CPreventiveMaintenance_Client();
	CPreventiveMaintenance_Client(BOOL bRFMonitoring);

	void MsgParser(string strEvent);

protected:

	void ChangeMonitoringState(BOOL bEnable, int nCompositeType, int nLeafType);

	vector<CPreventiveMaintenance_Component*> m_vecComponent;

	virtual void DoMonitor(int nThreadNo);
	DWORD Thread_OneSecondPolling(int nThreadNo);
	DWORD Thread_RFONMonitoring(int nThreadNo);
	enum {
		Thrd0_OneSecondPolling,
		Thrd1_RFONMonitoring
	};

	void ChangeCurrentValue(void* pData, int nLeafType);

	void ResetCurrentValue(int nCompositeType);

	void RefreshWarningAlarmValue();

	// added by ycchoi 2006.11.06 < V 1.0.0.6 >
	void RefreshCurrentValue();

// Member Variable
protected:
	// 2006.10.18
	enum { INIT_1000MS, INIT_500MS };
	int m_nInitMode;
};

#endif /* _INC_CPREVENTIVEMAINTENANCE_CLIENT_40C16FA901C5_INCLUDED */
