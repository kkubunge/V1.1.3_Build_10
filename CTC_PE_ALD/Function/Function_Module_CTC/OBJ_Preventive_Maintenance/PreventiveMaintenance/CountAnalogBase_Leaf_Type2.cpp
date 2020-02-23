// CountAnalogBase_Leaf_Type2.cpp: implementation of the CCountAnalogBase_Leaf_Type2 class.
// 2006.10.18
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CountAnalogBase_Leaf_Type2.h"
#include "PrevMaintMonitor.h"					// 2006.02.20

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCountAnalogBase_Leaf_Type2::CCountAnalogBase_Leaf_Type2(int nLeafType , int nCompositeType)
	:CCountAnalogBase_Leaf(nLeafType , nCompositeType)
{

}

CCountAnalogBase_Leaf_Type2::~CCountAnalogBase_Leaf_Type2()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

int CCountAnalogBase_Leaf_Type2::ChangeCurrentValue(void* pData, int nLeafType)
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
			ALARM_POST(m_nAlarmID);
		return NORWARALA_ALARM;
	}
	else if ((m_doubCurrentValue >= m_doubWarningValue) && (0 < m_doubWarningValue))
	{
		/*
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nWarningID)))
			ALARM_POST(m_nWarningID);
		*/

		return NORWARALA_WARNING;
	}
	return NORWARALA_NORMAL;
}

