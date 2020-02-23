// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CCountAnalogBase_Leaf.h"
#include "PrevMaintMonitor.h"					// 2006.02.20

void CCountAnalogBase_Leaf::EnableMonitoring(BOOL bEnable, int nLeafType)
{
	if (nLeafType == m_nType)
	{
		m_bMonitoring = bEnable;
		int nCommStatus;
		WRITE_DIGITAL(m_nEnableChannelID, (int)m_bMonitoring, &nCommStatus);
	}
}

void CCountAnalogBase_Leaf::ResetCurrentValue()
{
	extern CPrevMaintMonitor* g_pPrevMaintMonitor;		// 2006.02.20
	m_doubCurrentValue = 0;
	int nCommStatus;
	WRITE_ANALOG(m_nCurrentValueChannelID, 0, &nCommStatus);
	g_pPrevMaintMonitor->UpdatePM();	// 2006.02.20
}

//////////////////////////////////////////////////////////////////////
// Member Function

int CCountAnalogBase_Leaf::ChangeCurrentValue(void* pData, int nLeafType)
{
	double doubIncrement = *((double*)pData);
	extern CPrevMaintMonitor* g_pPrevMaintMonitor;		// 2006.02.20
	if ((nLeafType == m_nType) && (doubIncrement > 0))
	{
		m_doubCurrentValue += doubIncrement;
		// For protecting minus calculation..
		if (m_doubCurrentValue < 0)
			m_doubCurrentValue = 0;

		int nCommStatus;
		WRITE_ANALOG(m_nCurrentValueChannelID, m_doubCurrentValue, &nCommStatus);
		g_pPrevMaintMonitor->UpdatePM();	// 2006.02.20

		//... 2014.12.22 Add Increase Value for Thickness Period DryClean
		//... Keep Last Thickness Increase
		//... nLeafType : Leaf
		if(THICKNESS_AMOUNT_LEAF_TYPE == nLeafType)
		{
// 			printf("[DEBUG] ALeafType2::ChangeCurrentValue Leaf[%d] Cur[%.2f] Add[%.2f]\n", 
// 				nLeafType, m_doubCurrentValue, doubIncrement);

			//... Only Thickness Increase Value
			m_doubIncreaseValue = doubIncrement;
		}
	}

	if (FALSE == m_bMonitoring)
		return NORWARALA_NORMAL;
	if ((m_doubCurrentValue >= m_doubAlarmValue) && (0 < m_doubAlarmValue))
	{
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nAlarmID)))
		{
			//... 2014.07.22 Alarm Post Skip for Thickness Dry Clean
			if((DRY_CLEANING_COMPOSITE_TYPE != m_nCompositeType) ||
				(THICKNESS_AMOUNT_LEAF_TYPE != m_nType))
			{
				ALARM_POST(m_nAlarmID);
			}
		}
		return NORWARALA_ALARM;
	}
	else if ((m_doubCurrentValue >= m_doubWarningValue) && (0 < m_doubWarningValue))
	{
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nWarningID)))
		{
			//... 2014.07.22 Alarm Post Skip for Thickness Dry Clean
			if((DRY_CLEANING_COMPOSITE_TYPE != m_nCompositeType) ||
				(THICKNESS_AMOUNT_LEAF_TYPE != m_nType))
			{
				ALARM_POST(m_nWarningID);
			}
		}
		return NORWARALA_WARNING;
	}

	return NORWARALA_NORMAL;
}

void CCountAnalogBase_Leaf::RefreshWarningAlarmValue()
{
	m_doubWarningValue = 0;
	m_doubAlarmValue = 0;
	int nCommStatus;
	m_doubWarningValue = READ_ANALOG(m_nWarningValueChannelID, &nCommStatus);
	m_doubAlarmValue = READ_ANALOG(m_nAlarmValueChannelID, &nCommStatus);
}


// added by ycchoi 2006.11.06 < V 1.0.0.6 >
void CCountAnalogBase_Leaf::RefreshCurrentValue()
{
	m_doubCurrentValue = 0;
	int nCommStatus;
	m_doubCurrentValue = READ_ANALOG(m_nCurrentValueChannelID, &nCommStatus);
}

CCountAnalogBase_Leaf::CCountAnalogBase_Leaf(int nLeafType , int nCompositeType)
{
	m_nType = nLeafType;
	m_nCompositeType = nCompositeType;
}

void CCountAnalogBase_Leaf::InitializeChannelInformation(int nCurrentValueChannelID, int nWarningValueChannelID, int nAlarmValueChannelID, int nWarningID, int nAlarmID, int nEnableChannelID, int nAlarmChannelID, BOOL bAlarmPost)
{
	m_nCurrentValueChannelID = nCurrentValueChannelID;
	m_nWarningValueChannelID = nWarningValueChannelID;
	m_nAlarmValueChannelID = nAlarmValueChannelID;
	m_nWarningID = nWarningID;
	m_nAlarmID = nAlarmID;
	m_nEnableChannelID = nEnableChannelID;

	m_doubCurrentValue	= 0;
	m_doubWarningValue	= 0;
	m_doubAlarmValue	= 0;
	m_doubIncreaseValue = 0;	//... 2014.12.22 Add Increase Value for Thickness Period DryClean

	int nCommStatus;
	m_doubCurrentValue = READ_ANALOG(m_nCurrentValueChannelID, &nCommStatus);
	m_doubWarningValue = READ_ANALOG(m_nWarningValueChannelID, &nCommStatus);
	m_doubAlarmValue = READ_ANALOG(m_nAlarmValueChannelID, &nCommStatus);
	m_bMonitoring = READ_DIGITAL(m_nEnableChannelID, &nCommStatus);

	m_bAlarmPost = bAlarmPost;
}

//////////////////////////////////////////////////////////////////////////
//... 2014.12.22 Add Increase Value for Thickness Period DryClean
bool CCountAnalogBase_Leaf::IsPostRunTime(int nType)
{
	bool bRet = false;
	double	dbIncreaseValue = 0;
	double	dbReservedValue = 0;

	if(m_doubIncreaseValue > 0)		dbIncreaseValue = m_doubIncreaseValue;

	dbReservedValue = m_doubWarningValue - dbIncreaseValue;
	if(dbReservedValue < 0)		dbReservedValue = 0;

	if((m_doubCurrentValue >= dbReservedValue) && (0 < m_doubWarningValue) && m_bMonitoring == TRUE)
	{
		bRet = true;
	}

	if(bRet)
	{
		// nType : Composite
		printf("[INFO] ALeafType::IsPostRunTime[%d] Comp[%d] Cur[%.0f] Resv[%.0f] Warn[%.0f]\n", 
			bRet, nType, m_doubCurrentValue, dbReservedValue, m_doubWarningValue);
	}

	/*
	if((m_doubCurrentValue >= m_doubWarningValue - dbIncreaseData) && (0 < m_doubWarningValue) && m_bMonitoring == TRUE)
	{
		bRet = true;
	}
	*/

	return bRet;
}
