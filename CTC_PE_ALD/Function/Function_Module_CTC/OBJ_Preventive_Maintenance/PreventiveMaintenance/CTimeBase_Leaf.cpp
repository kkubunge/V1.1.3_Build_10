// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CTimeBase_Leaf.h"



void CTimeBase_Leaf::EnableMonitoring(BOOL bEnable, int nLeafType)
{
	if (nLeafType == m_nType)
	{
		m_bMonitoring = bEnable;
		int nCommStatus;
		WRITE_DIGITAL(m_nEnableChannelID, (int)m_bMonitoring, &nCommStatus);
	}
}

void CTimeBase_Leaf::ResetCurrentValue()
{
	m_nCurrentValueSec = 0;
	int nCommStatus;
	WRITE_DIGITAL(m_nCurrentValueDayChannelID, 0, &nCommStatus);
	WRITE_DIGITAL(m_nCurrentValueHourChannelID, 0, &nCommStatus);
	WRITE_DIGITAL(m_nCurrentValueMinChannelID, 0, &nCommStatus);
	WRITE_DIGITAL(m_nCurrentValueSecondChannelID, 0, &nCommStatus);
}

int CTimeBase_Leaf::ChangeCurrentValue(void* pData, int nLeafType)
{
	int nSeconds = *((int*)pData);
	if (nLeafType != m_nType)return NORWARALA_NORMAL;
	if (nSeconds >= 1)
	{
		m_nCurrentValueSec += nSeconds;

		// For protecting minus calculation..
		if (m_nCurrentValueSec < 0)
			m_nCurrentValueSec = 0;

		int nDay, nHour, nMin, nSecond;
		nDay = m_nCurrentValueSec / (24*60*60);
		nHour = (m_nCurrentValueSec-(nDay*24*60*60)) / (60*60);
		nMin = (m_nCurrentValueSec-(nDay*24*60*60)-(nHour*60*60)) / 60;
		nSecond = (m_nCurrentValueSec-(nDay*24*60*60)-(nHour*60*60)-(nMin*60));
		if (TRUE == m_bHourMode)
		{
			nHour += (nDay*24);
			nDay = 0;
		}
		int nCommStatus;
		WRITE_DIGITAL(m_nCurrentValueDayChannelID, nDay, &nCommStatus);
		WRITE_DIGITAL(m_nCurrentValueHourChannelID, nHour, &nCommStatus);
		WRITE_DIGITAL(m_nCurrentValueMinChannelID, nMin, &nCommStatus);
		WRITE_DIGITAL(m_nCurrentValueSecondChannelID, nSecond, &nCommStatus);
	}

	if (FALSE == m_bMonitoring)
		return NORWARALA_NORMAL;
	if ((m_nCurrentValueSec > m_nAlarmValueSec) && (0 < m_nAlarmValueSec))
	{
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nAlarmID)))
			ALARM_POST(m_nAlarmID);
		return NORWARALA_ALARM;
	}
	else if ((m_nCurrentValueSec > m_nWarningValueSec) && (0 < m_nWarningValueSec))
	{
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nWarningID)))
			ALARM_POST(m_nWarningID);
		return NORWARALA_WARNING;
	}
	return NORWARALA_NORMAL;
}

void CTimeBase_Leaf::RefreshWarningAlarmValue()
{
	m_nWarningValueSec = 0;
	m_nAlarmValueSec = 0;
	int nCommStatus;
	m_nWarningValueSec += (READ_DIGITAL(m_nWarningValueDayChannelID, &nCommStatus)*24*60*60);
	m_nWarningValueSec += (READ_DIGITAL(m_nWarningValueHourChannelID, &nCommStatus)*60*60);
	m_nAlarmValueSec += (READ_DIGITAL(m_nAlarmValueDayChannelID, &nCommStatus)*24*60*60);
	m_nAlarmValueSec += (READ_DIGITAL(m_nAlarmValueHourChannelID, &nCommStatus)*60*60);
}

void CTimeBase_Leaf::InitializeChannelInformation(int nCurrentDayID, int nCurrentHourID, int nCurrentMinID, int nCurrentSecondID, int nWarningDayID, int nWarningHourID, int nAlarmDayID, int nAlarmHourID, int nWarningID, int nAlarmID, int nEnableChannelID, int nAlarmChannelID, BOOL bAlarmPost)
{
	m_nCurrentValueDayChannelID = nCurrentDayID;
	m_nCurrentValueHourChannelID = nCurrentHourID;
	m_nCurrentValueMinChannelID = nCurrentMinID;
	m_nCurrentValueSecondChannelID = nCurrentSecondID;
	m_nWarningValueDayChannelID = nWarningDayID;
	m_nWarningValueHourChannelID = nWarningHourID;
	m_nAlarmValueDayChannelID = nAlarmDayID;
	m_nAlarmValueHourChannelID = nAlarmHourID;
	m_nWarningID = nWarningID;
	m_nAlarmID = nAlarmID;
	m_nEnableChannelID = nEnableChannelID;

	m_nCurrentValueSec = 0;
	m_nWarningValueSec = 0;
	m_nAlarmValueSec = 0;
	int nCommStatus;
	m_nCurrentValueSec += (READ_DIGITAL(m_nCurrentValueDayChannelID, &nCommStatus)*24*60*60);
	m_nCurrentValueSec += (READ_DIGITAL(m_nCurrentValueHourChannelID, &nCommStatus)*60*60);
	m_nCurrentValueSec += (READ_DIGITAL(m_nCurrentValueMinChannelID, &nCommStatus)*60);
	m_nCurrentValueSec += (READ_DIGITAL(m_nCurrentValueSecondChannelID, &nCommStatus));

	m_nWarningValueSec += (READ_DIGITAL(m_nWarningValueDayChannelID, &nCommStatus)*24*60*60);
	m_nWarningValueSec += (READ_DIGITAL(m_nWarningValueHourChannelID, &nCommStatus)*60*60);

	m_nAlarmValueSec += (READ_DIGITAL(m_nAlarmValueDayChannelID, &nCommStatus)*24*60*60);
	m_nAlarmValueSec += (READ_DIGITAL(m_nAlarmValueHourChannelID, &nCommStatus)*60*60);

	m_bMonitoring = READ_DIGITAL(m_nEnableChannelID, &nCommStatus);

	m_bAlarmPost = bAlarmPost;
}

CTimeBase_Leaf::CTimeBase_Leaf(int nLeafType, BOOL bHourMode)
{
	m_nType = nLeafType;
	m_bHourMode = bHourMode;
}
