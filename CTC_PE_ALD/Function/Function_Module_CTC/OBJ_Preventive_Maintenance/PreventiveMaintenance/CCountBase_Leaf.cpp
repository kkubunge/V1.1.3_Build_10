// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CCountBase_Leaf.h"
#include "PrevMaintMonitor.h"					// 2006.02.20

extern int g_nPMNumber;
void CCountBase_Leaf::EnableMonitoring(BOOL bEnable, int nLeafType)
{
	if (nLeafType == m_nType)
	{
		m_bMonitoring = bEnable;
		int nCommStatus;
		WRITE_DIGITAL(m_nEnableChannelID, (int)m_bMonitoring, &nCommStatus);
	}
}

void CCountBase_Leaf::ResetCurrentValue()
{
	extern CPrevMaintMonitor* g_pPrevMaintMonitor;		// 2006.02.20
	m_nCurrentValue = 0;
	int nCommStatus;
	WRITE_DIGITAL(m_nCurrentValueChannelID, 0, &nCommStatus);
	g_pPrevMaintMonitor->UpdatePM(); // 2006.02.20
}

//////////////////////////////////////////////////////////////////////////
int CCountBase_Leaf::ChangeCurrentValue(void* pData, int nLeafType)
{
	int nIncrement = *((int*)pData);
	extern CPrevMaintMonitor* g_pPrevMaintMonitor;		// 2006.02.20

	if ((nLeafType == m_nType) && (nIncrement >= 1))
	{
		m_nCurrentValue += nIncrement;
		// For protecting minus calculation..
		if (m_nCurrentValue < 0)	m_nCurrentValue = 0;

		int nCommStatus;
		WRITE_DIGITAL(m_nCurrentValueChannelID, m_nCurrentValue, &nCommStatus);
		g_pPrevMaintMonitor->UpdatePM(); // 2006.02.20
	}

	if (FALSE == m_bMonitoring)		return NORWARALA_NORMAL;

	if ((m_nCurrentValue >= m_nAlarmValue) && (0 < m_nAlarmValue))
	{
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nAlarmID))) 
		{
			//2011.01.04 Dry Clean Alarm 발생 시키지 않고 dry cleaning을 진행 하도록 수정
			//현재 count over시 online dry를 위하여 maint mode로 변환하지 않으므로 추후 Alarm 발생이 없어 문제 발생되지 않도록 수정 필요
			if (DRY_CLEANING_COMPOSITE_TYPE != m_nCompositeType)	ALARM_POST(m_nAlarmID);
		}

		//2008.01.04 Over Alarm Count then Post alarm by mgsong
		if ((m_nCurrentValue >= m_nAlarmValue + 50) && (ALM_PAUSED != ALARM_STATUS(m_nAlarmID)) && (DRY_CLEANING_COMPOSITE_TYPE == m_nCompositeType)) {
			//Dry cleaning composite only
			ALARM_POST(m_nAlarmID);
			int nComStatus;
			string strEvent = "SET_MODULE_INFO_LOCAL ";
			switch (g_nPMNumber) {
				case 1: strEvent += "PM1"; break;
				case 2: strEvent += "PM2"; break;
				case 3: strEvent += "PM3"; break;
				default: break;
			}			
			strEvent += "|DISABLE";
			WRITE_STRING(SCHEDULER , (char*)strEvent.c_str() , &nComStatus);
		}
		//end 2008.01.04
		return NORWARALA_ALARM;
	}
	else if ((m_nCurrentValue >= m_nWarningValue) && (0 < m_nWarningValue))
	{
		/*
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nWarningID)))
			ALARM_POST(m_nWarningID);
		*/
		
		return NORWARALA_WARNING;
	}
	return NORWARALA_NORMAL;
}

void CCountBase_Leaf::RefreshWarningAlarmValue()
{
	m_nWarningValue = 0;
	m_nAlarmValue = 0;
	int nCommStatus;
	m_nWarningValue = READ_DIGITAL(m_nWarningValueChannelID, &nCommStatus);
	m_nAlarmValue = READ_DIGITAL(m_nAlarmValueChannelID, &nCommStatus);
}

// added by yccoi 2006.11.06 < V 1.0.0.6 >
void CCountBase_Leaf::RefreshCurrentValue()
{
	m_nCurrentValue = 0;
	int nCommStatus;
	m_nCurrentValue = READ_DIGITAL(m_nCurrentValueChannelID, &nCommStatus);
}

CCountBase_Leaf::CCountBase_Leaf(int nLeafType , int nCompositeType)
{
	m_nType          = nLeafType;
	m_nCompositeType = nCompositeType;
}

void CCountBase_Leaf::InitializeChannelInformation(int nCurrentValueChannelID, int nWarningValueChannelID, int nAlarmValueChannelID, int nWarningID, int nAlarmID, int nEnableChannelID, int nAlarmChannelID, BOOL bAlarmPost)
{
	m_nCurrentValueChannelID = nCurrentValueChannelID;
	m_nWarningValueChannelID = nWarningValueChannelID;
	m_nAlarmValueChannelID = nAlarmValueChannelID;
	m_nWarningID = nWarningID;
	m_nAlarmID = nAlarmID;
	m_nEnableChannelID = nEnableChannelID;

	m_nCurrentValue = 0;
	m_nWarningValue = 0;
	m_nAlarmValue = 0;
	int nCommStatus;
	m_nCurrentValue = READ_DIGITAL(m_nCurrentValueChannelID, &nCommStatus);
	m_nWarningValue = READ_DIGITAL(m_nWarningValueChannelID, &nCommStatus);
	m_nAlarmValue = READ_DIGITAL(m_nAlarmValueChannelID, &nCommStatus);
	m_bMonitoring = READ_DIGITAL(m_nEnableChannelID, &nCommStatus);

	m_bAlarmPost = bAlarmPost;
}

// added by ycchoi 2006.05.28 < V 1.0.0.2 >
int CCountBase_Leaf::GetCurrentWaferCount()
{
	int nCommStatus;
	return READ_DIGITAL(m_nCurrentValueChannelID, &nCommStatus);;
}

// added by ycchoi 2006.05.28 < V 1.0.0.2 >
int CCountBase_Leaf::GetType()
{
	return m_nType;
}

//2009.08.21 by mgsong
int CCountBase_Leaf::GetWaringID()
{
    return m_nWarningID;
}

int CCountBase_Leaf::GetAlarmID()
{
    return m_nAlarmID;

}

//////////////////////////////////////////////////////////////////////////
bool CCountBase_Leaf::IsPostRunTime(int nType)
{
	bool bRet = false;
	//... 2015.01.05 Twin Chamber Wafer PreCount 2 EA
	if ((m_nCurrentValue >= m_nWarningValue-2) && (0 < m_nWarningValue) && m_bMonitoring == TRUE)
	{
		bRet = true;
	}

	return bRet;
}


