// CountBase_Leaf_Type2.cpp: implementation of the CCountBase_Leaf_Type2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CountBase_Leaf_Type2.h"
#include "PrevMaintMonitor.h"					// 2006.02.20

extern int g_nPMNumber;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCountBase_Leaf_Type2::CCountBase_Leaf_Type2(int nLeafType , int nCompositeType)
	:CCountBase_Leaf(nLeafType , nCompositeType)
{
	
}

CCountBase_Leaf_Type2::~CCountBase_Leaf_Type2()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

int CCountBase_Leaf_Type2::ChangeCurrentValue(void* pData, int nLeafType)
{
	int nIncrement = *((int*)pData);
	extern CPrevMaintMonitor* g_pPrevMaintMonitor;		// 2006.02.20
	if ((nLeafType == m_nType) && (nIncrement >= 1))
	{
		m_nCurrentValue += nIncrement;
		// For protecting minus calculation..
		if (m_nCurrentValue < 0)
			m_nCurrentValue = 0;

		int nCommStatus;
		WRITE_DIGITAL(m_nCurrentValueChannelID, m_nCurrentValue, &nCommStatus);
		g_pPrevMaintMonitor->UpdatePM(); // 2006.02.20
	}

	if (FALSE == m_bMonitoring)
		return NORWARALA_NORMAL;
	if ((m_nCurrentValue >= m_nAlarmValue) && (0 < m_nAlarmValue))
	{
		if ((TRUE == m_bAlarmPost) && (ALM_PAUSED != ALARM_STATUS(m_nAlarmID))) {
			//2011.01.04 Dry Clean Alarm 발생 시키지 않고 dry cleaning을 진행 하도록 수정
			//현재 count over시 online dry를 위하여 maint mode로 변환하지 않으므로 추후 Alarm 발생이 없어 문제 발생되지 않도록 수정 필요
			if (DRY_CLEANING_COMPOSITE_TYPE != m_nCompositeType)
				ALARM_POST(m_nAlarmID);
		}
		//2008.01.04 Over Alarm Count then Post alarm by mgsong
		/*if ((m_nCurrentValue >= m_nAlarmValue + 50) && (ALM_PAUSED != ALARM_STATUS(m_nAlarmID)) && (DRY_CLEANING_COMPOSITE_TYPE == m_nCompositeType)) {
			//Dry cleaning composite only
			ALARM_POST(m_nAlarmID);
			int nComStatus;
			string strEvent = "SET_MODULE_INFO_LOCAL ";
			switch (g_nPMNumber) {
				case 1: break; //spare
				case 2: strEvent += "PM2"; break;
				case 3: strEvent += "PM3"; break;
				default: break;
			}			
			strEvent += "|DISABLE";
			WRITE_STRING(SCHEDULER , (char*)strEvent.c_str() , &nComStatus);
		}*/
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

bool CCountBase_Leaf_Type2::IsPostRunTime(int nType)
{
	bool bRet = false;
	if ((m_nCurrentValue >= m_nWarningValue-1)
		&& (0 < m_nWarningValue)
		&& m_bMonitoring == TRUE)
	{
		bRet = true;
	}
	return bRet;
}

