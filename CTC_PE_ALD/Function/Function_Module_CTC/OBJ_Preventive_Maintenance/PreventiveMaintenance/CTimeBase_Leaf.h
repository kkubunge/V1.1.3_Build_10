// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CTIMEBASE_LEAF_40C070A0029F_INCLUDED
#define _INC_CTIMEBASE_LEAF_40C070A0029F_INCLUDED

#include "CPreventiveMaintenance_Component.h"

class CTimeBase_Leaf 
: public CPreventiveMaintenance_Component
{
public:
	CTimeBase_Leaf(int nLeafType, BOOL bHourMode);

	void InitializeChannelInformation(int nCurrentDayID, int nCurrentHourID, int nCurrentMinID, int nCurrentSecondID, int nWarningDayID, int nWarningHourID, int nAlarmDayID, int nAlarmHourID, int nWarningID, int nAlarmID, int nEnableChannelID, int nAlarmChannelID, BOOL bAlarmPost);

	virtual void EnableMonitoring(BOOL bEnable, int nLeafType);

	virtual void ResetCurrentValue();

	virtual int ChangeCurrentValue(void* pData, int nLeafType);

	virtual void RefreshWarningAlarmValue();

private:
	BOOL m_bAlarmPost;

	BOOL m_bHourMode;

	BOOL m_nHourMode;

	int m_nEnableChannelID;

	int m_nCurrentValueSec;

	int m_nWarningValueSec;

	int m_nAlarmValueSec;

	int m_nWarningID;

	int m_nAlarmID;

	int m_nCurrentValueDayChannelID;

	int m_nCurrentValueHourChannelID;

	int m_nCurrentValueMinChannelID;

	int m_nCurrentValueSecondChannelID;

	int m_nWarningValueDayChannelID;

	int m_nWarningValueHourChannelID;

	int m_nAlarmValueDayChannelID;

	int m_nAlarmValueHourChannelID;

	int m_nType;

	BOOL m_bMonitoring;

};

#endif /* _INC_CTIMEBASE_LEAF_40C070A0029F_INCLUDED */
