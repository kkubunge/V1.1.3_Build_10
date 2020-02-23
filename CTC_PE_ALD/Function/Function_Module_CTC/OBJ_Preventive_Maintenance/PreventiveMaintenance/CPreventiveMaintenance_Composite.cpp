// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CPreventiveMaintenance_Composite.h"
#include "CCountBase_Leaf.h"

// 2006.10.18
#include "PrevMaintMonitor.h"
extern CPrevMaintMonitor* g_pPrevMaintMonitor;
extern int g_nPMNumber;

void CPreventiveMaintenance_Composite::RemoveComponent(CPreventiveMaintenance_Component* pComponent)
{
	if (m_vecComponent.end() != find(m_vecComponent.begin(), m_vecComponent.end(), pComponent))
	{
		vector<CPreventiveMaintenance_Component*>::iterator theIterRemove = remove(m_vecComponent.begin(), m_vecComponent.end(), pComponent);
		m_vecComponent.erase(theIterRemove, m_vecComponent.end());
	}
}

int CPreventiveMaintenance_Composite::GetType()
{
	return m_nType;
}

void CPreventiveMaintenance_Composite::EnableMonitoring(BOOL bEnable, int nLeafType)
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->EnableMonitoring(bEnable, nLeafType);
	}
}


//////////////////////////////////////////////////////////////////////////
// added by ycchoi 2006.05.28 < V 1.0.0.2 >
int CPreventiveMaintenance_Composite::GetCurrentWaferCount()
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		if((*theIter)->GetType() == WAFER_COUNT_LEAF_TYPE)
		{
			return (*theIter)->GetCurrentWaferCount();
		}
	}

	return 0;
}
//
//////////////////////////////////////////////////////////////////////////

void CPreventiveMaintenance_Composite::ResetCurrentValue()
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;

	int nWarningID;
	int nAlarmID;

	// acced by ycchoi 2006.05.28 < 1.0.0.2 >
	int nCurrWfrCnt = GetCurrentWaferCount();

	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->ResetCurrentValue();
		// 2009.08.21 by mgsong
		if ((*theIter)->GetType() == WAFER_COUNT_LEAF_TYPE) {
		    CCountBase_Leaf* pLeaf = (CCountBase_Leaf*)(*theIter);
		    nWarningID = pLeaf->GetWaringID();
		    nAlarmID   = pLeaf->GetAlarmID();
		}		
	}


	//2009.08.21 Clear Dry Wafer Count Warning & Alarm here automatically
	// if Compoisite Type == DRY_CLEANING_COMPOSITE_TYPE then clear alarm (DRY Count Over Alarm / Warning
	if (m_nType == DRY_CLEANING_COMPOSITE_TYPE) {
	    if (ALM_PAUSED == ALARM_STATUS(nWarningID)) {
		 ALARM_CLEAR(nWarningID);
	    }

	    if (ALM_PAUSED == ALARM_STATUS(nAlarmID)) {
		 ALARM_CLEAR(nAlarmID);
	    }
	}


	char szStringTemp[255] = "";	
	int nCommStatus;

	// added by ycchoi 2006.05.28 < 1.0.0.2 >
	int nResetWfrCount;

	for (int nCount = 8; nCount >= 0; nCount--)
	{
		READ_STRING(m_nResetHistoryBaseChannelID+nCount, (char*)szStringTemp, &nCommStatus);
		WRITE_STRING(m_nResetHistoryBaseChannelID+nCount+1, (char*)szStringTemp, &nCommStatus);

		// added by ycchoi 2006.05.28 < 1.0.0.2 >
		nResetWfrCount = READ_DIGITAL(m_nResetWfrCntHistoryBaseChannelID+nCount, &nCommStatus);
		WRITE_DIGITAL(m_nResetWfrCntHistoryBaseChannelID+nCount+1, nResetWfrCount, &nCommStatus);
	}
	int nResetCount = READ_DIGITAL(m_nResetCountChannelID, &nCommStatus);
	WRITE_DIGITAL(m_nResetCountChannelID, (int)(nResetCount+1), &nCommStatus);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(szStringTemp, "%02d / %02d / %02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	WRITE_STRING(m_nResetTimeChannelID, (char*)szStringTemp, &nCommStatus);
	sprintf(szStringTemp, "%04d / %02d / %02d   %02d : %02d : %02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	WRITE_STRING(m_nResetHistoryBaseChannelID, (char*)szStringTemp, &nCommStatus);

	// added by ycchoi 2006.05.28 < 1.0.0.2 >
	WRITE_DIGITAL(m_nResetWfrCntHistoryBaseChannelID, nCurrWfrCnt, &nCommStatus);

	WRITE_DIGITAL(m_nAlarmChannelID, NORWARALA_NORMAL, &nCommStatus);
}

//////////////////////////////////////////////////////////////////////////
//... Reserved Post Run Flag
int CPreventiveMaintenance_Composite::ChangeCurrentValue(void* pData, int nLeafType)
{
	int nAlarmStatus = NORWARALA_NORMAL;
	int nCommStatus;
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		int nCurrentAlarmStatus = (*theIter)->ChangeCurrentValue(pData, nLeafType);
		if (NORWARALA_ALARM == nCurrentAlarmStatus)
		{
			nAlarmStatus = NORWARALA_ALARM;
		}
		else if ((NORWARALA_WARNING == nCurrentAlarmStatus) && (NORWARALA_NORMAL == nAlarmStatus))
		{
			nAlarmStatus = NORWARALA_WARNING;
		}

		//////////////////////////////////////////////////////////////////////////
		//... 2014.12.22 Only Dry Clean Post Run
		if(m_nType == DRY_CLEANING_COMPOSITE_TYPE &&
			(nLeafType == WAFER_COUNT_LEAF_TYPE || nLeafType == THICKNESS_AMOUNT_LEAF_TYPE))
		{
			if((*theIter)->IsPostRunTime(m_nType))
			{
				if(! g_pPrevMaintMonitor->IsPostRunReserved(m_nType))
				{	//... Reserve Post Run
					g_pPrevMaintMonitor->ReservePostRun(PR_RESERVE, m_nType);
					printf("[INFO] Composite::ChangeCurrentValue << RESERVED POST RUN BY RUNTIME >> Comp[%d] Leaf[%d]\n", 
						m_nType, nLeafType);
				}
				
				//mgsong
				//2007.12.06 At this Time Dry Post Enabled but we need control timing
				switch (g_nPMNumber) {
				
				case 1: // 2008.08.12 1PM Addition
					if((!g_pPrevMaintMonitor->IsPostRunReserved(m_nType)) && 
						(DRY_RUNNING != READ_DIGITAL(PM1_ChamberStatus, &nCommStatus)))
					{
						WRITE_DIGITAL(PM1_ChamberStatus, 1 , &nCommStatus);
						printf("PM1 ChamberStatus Set\n");
					}					
					break; 
				case 2:
					if((!g_pPrevMaintMonitor->IsPostRunReserved(m_nType)) && 
						(DRY_RUNNING != READ_DIGITAL(PM2_ChamberStatus, &nCommStatus)))
					{
						WRITE_DIGITAL(PM2_ChamberStatus, 1 , &nCommStatus);
						printf("PM2 ChamberStatus Set\n");
					}
					break;
				case 3:
					if((!g_pPrevMaintMonitor->IsPostRunReserved(m_nType)) && 
						(DRY_RUNNING != READ_DIGITAL(PM3_ChamberStatus, &nCommStatus)))
					{
						WRITE_DIGITAL(PM3_ChamberStatus, 1 , &nCommStatus);
						printf("PM3 ChamberStatus Set\n");
					}
					break;
				default:
					break;
				}				
			}
		}
	}

	//mgsong
	//2007.12.06 Sometimes over count but PostRun flag Not Reserved due to this compare routine so remarks....
	if (/*m_nAlarmStatus != nAlarmStatus &&*/ nAlarmStatus == NORWARALA_ALARM)
	{
		int nCommStatus;

		m_nAlarmStatus = nAlarmStatus;
		WRITE_DIGITAL(m_nAlarmChannelID, m_nAlarmStatus, &nCommStatus);

		//... 2014.12.22 Only Dry Clean Post Run
		if(m_nType == DRY_CLEANING_COMPOSITE_TYPE &&
			(nLeafType == WAFER_COUNT_LEAF_TYPE || nLeafType == THICKNESS_AMOUNT_LEAF_TYPE))
		{
			// 2006.10.18
			if(m_nAlarmStatus != NORWARALA_NORMAL)
			{
				if(! g_pPrevMaintMonitor->IsPostRunReserved(m_nType))
				{	//... Reserve Post Run
					g_pPrevMaintMonitor->ReservePostRun(PR_RESERVE, m_nType);
					printf("[INFO] Composite::ChangeCurrentValue << RESERVED POST RUN BY ALARM[%d] >> Comp[%d] Leaf[%d]\n", 
						m_nAlarmStatus, m_nType, nLeafType);
				}
			}
		}
	}

	return m_nAlarmStatus;
}

void CPreventiveMaintenance_Composite::RefreshWarningAlarmValue()
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->RefreshWarningAlarmValue();
	}
}

// added by ycchoi 2006.11.06 < V 1.0.0.6 >
void CPreventiveMaintenance_Composite::RefreshCurrentValue()
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->RefreshCurrentValue();
	}
}

CPreventiveMaintenance_Composite::CPreventiveMaintenance_Composite(int nCompositeType)
{
	m_nType = nCompositeType;
}

void CPreventiveMaintenance_Composite::AddComponent(CPreventiveMaintenance_Component* pComponent)
{
	if (m_vecComponent.end() == find(m_vecComponent.begin(), m_vecComponent.end(), pComponent))
	{
		m_vecComponent.push_back(pComponent);
	}
}

void CPreventiveMaintenance_Composite::InitializeChannelInformation(int nResetCountID, int nResetTimeID, int nResetHistoryBaseID, int nAlarmChannelID)
{
	m_nResetCountChannelID = nResetCountID;
	m_nResetTimeChannelID = nResetTimeID;
	m_nResetHistoryBaseChannelID = nResetHistoryBaseID;
	m_nAlarmChannelID = nAlarmChannelID;
	int nCommStatus;
	m_nAlarmStatus = READ_DIGITAL(m_nAlarmChannelID, &nCommStatus);
}

void CPreventiveMaintenance_Composite::InitializeChannelInformation(int nResetCountID, int nResetTimeID, int nResetHistoryBaseID, int nResetWfrCntHistoryBaseID, int nAlarmChannelID)
{
	m_nResetCountChannelID = nResetCountID;
	m_nResetTimeChannelID = nResetTimeID;
	m_nResetHistoryBaseChannelID = nResetHistoryBaseID;
	m_nResetWfrCntHistoryBaseChannelID = nResetWfrCntHistoryBaseID;
	m_nAlarmChannelID = nAlarmChannelID;
	int nCommStatus;
	m_nAlarmStatus = READ_DIGITAL(m_nAlarmChannelID, &nCommStatus);
}
