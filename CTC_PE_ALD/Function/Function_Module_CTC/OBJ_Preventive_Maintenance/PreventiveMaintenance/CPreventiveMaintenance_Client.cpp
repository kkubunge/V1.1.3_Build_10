// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CPreventiveMaintenance_Component.h"
#include "CPreventiveMaintenance_Composite.h"
#include "CCountAnalogBase_Leaf.h"
#include "CCountBase_Leaf.h"
#include "CTimeBase_Leaf.h"
#include "CPreventiveMaintenance_Client.h"
#include <time.h>
//// It was added for online dry cleaning Leak Check, Dry Post 2007.02.1 //////////////////////
#include "PrevMaintMonitor.h"
extern CPrevMaintMonitor* g_pPrevMaintMonitor;
int    nProcessWaferCount = __INITIAL_STATE;
//// It was added for online dry cleaning Leak Check, Dry Post 2007.02.1 //////////////////////

// #define _DEBUG

void CPreventiveMaintenance_Client::DoMonitor(int nThreadNo)
{
	switch(nThreadNo)
	{
	case Thrd0_OneSecondPolling:
		Thread_OneSecondPolling(nThreadNo);
		break;
	case Thrd1_RFONMonitoring:
		//Thread_RFONMonitoring(nThreadNo);
		break;
	}
}

DWORD CPreventiveMaintenance_Client::Thread_OneSecondPolling(int nThreadNo)
{
	int nCommStatus;
	int nSeconds;
	clock_t clkBegin = 0, clkEnd = 0, clkTimeElapsed = 0;
	
	int nNewProcessWaferCount = 0;
	enum{Unknown, Proc, PreProc, PostProc, Idle, Clean, Purge, LotPre, LotPost};  //... 2015.06.19
	#define WAFER_N_PER_BATCH		2		// 2009.08.13
	int nCountIncrement = 0;				// 2009.08.13
	int nProcessCompleted  = __INITIAL_STATE;
	int nProcessCompleted1 = __INITIAL_STATE;
	int nProcessCompleted2 = __INITIAL_STATE;
	int nProcessCompleted3 = __INITIAL_STATE;
	int nProcessCompleted4 = __INITIAL_STATE;
	int nProcessCompleted5 = __INITIAL_STATE;
	int nProcessCompleted6 = __INITIAL_STATE;
	int nProcessCompleted7 = __INITIAL_STATE;
	int nProcessCompleted8 = __INITIAL_STATE;
	int nProcessCompleted9 = __INITIAL_STATE;

	int nSourceFeedingDataUpdate = 0;
	int nValveTotalCount = 0;
	double doubSourceFeedingAmount = 0;
	double dbThicknessAmount = 0;
	BOOL bProcessedWafer = FALSE;

	int nLoopCount = 0;
	clkBegin = clock();
	
	while(WAIT_SEC(0.5))
	{
		clkEnd = clock();
		clkTimeElapsed += (clkEnd - clkBegin);
		if ((nSeconds = clkTimeElapsed / 1000) >= 1)
		{
			ChangeCurrentValue((void*)&nSeconds, LIFE_TIME_LEAF_TYPE);
		}
		clkTimeElapsed = clkTimeElapsed % 1000;
		clkBegin = clkEnd;

		if (__INITIAL_STATE == nProcessCompleted)
		{
			WRITE_DIGITAL(PROCESS_Completed, __PROCESS_READY, &nCommStatus);
			nProcessCompleted = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted = READ_DIGITAL(PROCESS_Completed, &nCommStatus))&&
				Clean != READ_DIGITAL(PRO_RcpTypeDM, &nCommStatus))     //... 2015.06.19
			{
				if (++nSourceFeedingDataUpdate > 3)
				{
					nValveTotalCount = (int)READ_ANALOG(VLV_Total_Count, &nCommStatus);
					doubSourceFeedingAmount = READ_ANALOG(SRC_Feeding_Amount, &nCommStatus);
					dbThicknessAmount = READ_ANALOG(Thickness_Amount, &nCommStatus);

					ChangeCurrentValue((void*)&nValveTotalCount, VALVE_CYCLE_COUNT_LEAF_TYPE);
					ChangeCurrentValue((void*)&doubSourceFeedingAmount, SOURCE_FEEDING_AMOUNT_LEAF_TYPE);
					ChangeCurrentValue((void*)&dbThicknessAmount, THICKNESS_AMOUNT_LEAF_TYPE);

					//2014.02.21 Process Wafer count by jonghyuk
					//if(doubSourceFeedingAmount > 0 || dbThicknessAmount > 0) 기존에 Source Feeding Amount 만 Check 했으니 같이 가자...
					if(doubSourceFeedingAmount > 0)

					{
						nCountIncrement = WAFER_N_PER_BATCH;
						ChangeCurrentValue((void*)&nCountIncrement, WAFER_COUNT_LEAF_TYPE);
					}

					WRITE_DIGITAL(PROCESS_Completed, __PROCESS_READY, &nCommStatus);

					if (doubSourceFeedingAmount > 0)
						bProcessedWafer = TRUE;
					else
						bProcessedWafer = FALSE;
					#ifdef _DEBUG
					printf("-----> Preventive Item Updated !\n");
					#endif
				}
			}
		}
		Sleep(500);

		
		/////  It was added for online dry cleaning 2007.01.11 /////////////////////////////////////////////
		if (__INITIAL_STATE == nProcessCompleted1)
		{
			WRITE_DIGITAL(PROCESS_Completed1, __PROCESS_READY, &nCommStatus);
			nProcessCompleted1 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted1 = READ_DIGITAL(PROCESS_Completed1, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed1, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted2)
		{
			WRITE_DIGITAL(PROCESS_Completed2, __PROCESS_READY, &nCommStatus);
			nProcessCompleted2 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted2 = READ_DIGITAL(PROCESS_Completed2, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed2, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted3)
		{
			WRITE_DIGITAL(PROCESS_Completed3, __PROCESS_READY, &nCommStatus);
			nProcessCompleted3 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted3 = READ_DIGITAL(PROCESS_Completed3, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed3, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted4)
		{
			WRITE_DIGITAL(PROCESS_Completed4, __PROCESS_READY, &nCommStatus);
			nProcessCompleted4 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted4 = READ_DIGITAL(PROCESS_Completed4, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed4, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted5)
		{
			WRITE_DIGITAL(PROCESS_Completed5, __PROCESS_READY, &nCommStatus);
			nProcessCompleted5 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted5 = READ_DIGITAL(PROCESS_Completed5, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed5, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted6)
		{
			WRITE_DIGITAL(PROCESS_Completed6, __PROCESS_READY, &nCommStatus);
			nProcessCompleted6 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted6 = READ_DIGITAL(PROCESS_Completed6, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed6, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted7)
		{
			WRITE_DIGITAL(PROCESS_Completed7, __PROCESS_READY, &nCommStatus);
			nProcessCompleted7 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted7 = READ_DIGITAL(PROCESS_Completed7, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed7, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted8)
		{
			WRITE_DIGITAL(PROCESS_Completed8, __PROCESS_READY, &nCommStatus);
			nProcessCompleted8 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted8 = READ_DIGITAL(PROCESS_Completed8, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed8, __PROCESS_READY, &nCommStatus);
			}
		}

		if (__INITIAL_STATE == nProcessCompleted9)
		{
			WRITE_DIGITAL(PROCESS_Completed9, __PROCESS_READY, &nCommStatus);
			nProcessCompleted9 = __PROCESS_READY;
		}
		else
		{
			if (__PROCESS_COMPLETED == (nProcessCompleted9 = READ_DIGITAL(PROCESS_Completed9, &nCommStatus)))
			{
				WRITE_DIGITAL(PROCESS_Completed9, __PROCESS_READY, &nCommStatus);
			}
		}
		// 2008.03.21
		// Sleep(1500);

		if (__INITIAL_STATE == nProcessWaferCount)
		{
			nProcessWaferCount = READ_DIGITAL(Process_Wafer_Count, &nCommStatus) - WAFER_N_PER_BATCH;		// 2009.09.02
		}
		else
		{
			if (nSourceFeedingDataUpdate > 3 && TRUE == bProcessedWafer)
			{
				// 2009.08.13
				nNewProcessWaferCount = READ_DIGITAL(Process_Wafer_Count, &nCommStatus);
				nCountIncrement = nNewProcessWaferCount - nProcessWaferCount;

				// 2009.09.02
				if(nCountIncrement < WAFER_N_PER_BATCH*(-1) || nCountIncrement > WAFER_N_PER_BATCH*2)
				{
					nCountIncrement = WAFER_N_PER_BATCH;
				}
				nProcessWaferCount = nNewProcessWaferCount;
			   	if (TRUE == bProcessedWafer)
			   	{
					//2014.02.21 Process Wafer count by jonghyuk
			   	    //ChangeCurrentValue((void*)&nCountIncrement, WAFER_COUNT_LEAF_TYPE);
			   	    bProcessedWafer = FALSE;
			   	}

				nSourceFeedingDataUpdate = 0;
			}
		}

		if (++nLoopCount >= (2*30))
		{
			WRITE_DIGITAL(PREVENTIVE_SAVE, 1, &nCommStatus);
			nLoopCount = 0;
		}
	}

	return 0;
}

DWORD CPreventiveMaintenance_Client::Thread_RFONMonitoring(int nThreadNo)
{
	/*int nCommStatus;
	int nSeconds;
	clock_t clkBegin = 0, clkEnd = 0, clkTimeElapsed = 0;

	int nRFONStatus = __INITIAL_STATE;
	int nLoopCount = 0;
	
	clkBegin = clock();
	while(WAIT_SEC(0.1))
	{
		clkEnd = clock();
		if (__RF_ON == (nRFONStatus = READ_DIGITAL(RFPwOnDO, &nCommStatus)))
		{
			clkTimeElapsed += (clkEnd - clkBegin);
			if ((nSeconds = clkTimeElapsed / 1000) >= 1)
			{
				ChangeCurrentValue((void*)&nSeconds, RF_GENERATOR_ON_LEAF_TYPE);
			}
			clkTimeElapsed = clkTimeElapsed % 1000;
		}
		else if (nLoopCount++ > 10)
		{
			nLoopCount = 0;
			nSeconds = 0;
			ChangeCurrentValue((void*)&nSeconds, RF_GENERATOR_ON_LEAF_TYPE);
		}
		clkBegin = clkEnd;
	}*/

	return 0;
}


void CPreventiveMaintenance_Client::ChangeCurrentValue(void* pData, int nLeafType)
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->ChangeCurrentValue(pData, nLeafType);
	}
}

void CPreventiveMaintenance_Client::ChangeMonitoringState(BOOL bEnable, int nCompositeType, int nLeafType)
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		if (nCompositeType == (*theIter)->GetType())
			(*theIter)->EnableMonitoring(bEnable, nLeafType);
	}
}

void CPreventiveMaintenance_Client::ResetCurrentValue(int nCompositeType)
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		if (nCompositeType == (*theIter)->GetType())
		{
			(*theIter)->ResetCurrentValue();
		}
	}
    //// It was added for online dry cleaning Leak Check, Dry Post 2007.02.1 //////////////////////
	g_pPrevMaintMonitor->ResetPostRun(nCompositeType);
}

void CPreventiveMaintenance_Client::RefreshWarningAlarmValue()
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->RefreshWarningAlarmValue();
	}
}

// added by ycchoi 2006.11.06 < V 1.0.0.6 >
void CPreventiveMaintenance_Client::RefreshCurrentValue()
{
	vector<CPreventiveMaintenance_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		(*theIter)->RefreshCurrentValue();
	}
}

void CPreventiveMaintenance_Client::MsgParser(string strEvent)
{
	BOOL bRet = FALSE;
	do
	{
		if ("INITIALIZE" == strEvent)
		{
			// Do initialization if it need.
		}
		//-------------------------------------
		// 2006.10.18
		else if("INIT_1000MS" == strEvent)
		{
			m_nInitMode = INIT_1000MS;
		}
		else if("INIT_500MS" == strEvent)
		{
			m_nInitMode = INIT_500MS;
		}
		//-------------------------------------
		else if ("APPLY" == strEvent)
		{
			// added by ycchoi 2006.11.06 < V 1.0.0.6 >
			RefreshCurrentValue();
			
			RefreshWarningAlarmValue();
		}
		else if ((strEvent.size() == 15) || (strEvent.size() == 14))
		{
			int nCountResult = count(strEvent.begin(), strEvent.end(), ' ');
			if (3 != nCountResult)
				break;
			BOOL bEnable = FALSE;
			int nCompositeType = INVALID_TYPE;
			int nLeafType = INVALID_TYPE;
			char szTemp[20];
			sscanf(strEvent.c_str(), "%s %d %d %d", szTemp, &bEnable, &nCompositeType, &nLeafType);
			if ((INVALID_TYPE == nCompositeType) || (INVALID_TYPE == nLeafType))
				break;
			if ((0 == strEvent.compare(0, 6, "ENABLE")) && (strEvent.size() == 15))
			{
				ChangeMonitoringState(bEnable, nCompositeType, nLeafType);
			}
			else if ((0 == strEvent.compare(0, 5, "RESET")) && (strEvent.size() == 14))
			{
				ResetCurrentValue(nCompositeType);
			}
			else
				break;
		}
		else
			break;

		bRet = TRUE;
	}while(0);
	if (FALSE == bRet)
	{
		string strMsg = "Invalid Event : ";
		strMsg += strEvent;
		::MessageBox(NULL, strMsg.c_str(), "Preventive Maintenance", MB_ICONERROR|MB_SYSTEMMODAL|MB_OK);
	}
}

// 2006.10.18
CPreventiveMaintenance_Client::CPreventiveMaintenance_Client()
{
}

CPreventiveMaintenance_Client::CPreventiveMaintenance_Client(BOOL bRFMonitoring)
{
	CPreventiveMaintenance_Composite* pComposite = NULL;
	CCountBase_Leaf* pCountLeaf = NULL;
	CCountAnalogBase_Leaf* pCountAnalogLeaf = NULL;
	CTimeBase_Leaf* pTimeLeaf = NULL;
	int nChannelID = 0;
	int nAlarmID = 0;
	m_nInitMode = INIT_1000MS;		// 2006.10.18

	// WET cleaning
	pComposite = new CPreventiveMaintenance_Composite(WET_CLEANING_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	//pComposite->InitializeChannelInformation(PRMD_WETRstCnt, PRMS_WETRstTime, PRMS_WETRstTime01, PRMD_WETAlarm);
	pComposite->InitializeChannelInformation(PRMD_WETRstCnt, PRMS_WETRstTime, PRMS_WETRstTime01, PRMS_WETRstCount01, PRMD_WETAlarm);

	nChannelID = PRMD_WETCWfrCnt;
	nAlarmID = WARNING__WET_CLEANING__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_WETEn_01, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	// WET Cleaning 2    2009.08.24
	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+12, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_WETEn_01, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);
	// end

	pCountLeaf = new CCountBase_Leaf(VALVE_CYCLE_COUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nAlarmID+2, nAlarmID+3, PRMD_WETEn_02, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountAnalogLeaf = new CCountAnalogBase_Leaf(SOURCE_FEEDING_AMOUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountAnalogLeaf->InitializeChannelInformation(nChannelID+6, nChannelID+7, nChannelID+8, nAlarmID+4, nAlarmID+5, PRMD_WETEn_03, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountAnalogLeaf);

	//... 2018.06.17 WET Thick	
	pCountAnalogLeaf = new CCountAnalogBase_Leaf(THICKNESS_AMOUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountAnalogLeaf->InitializeChannelInformation(nChannelID+9, nChannelID+10, nChannelID+11, 
		WARNING__WET_CLEANING__THICKNESS_AMOUNT__OVER, ALARM__WET_CLEANING__THICKNESS_AMOUNT__OVER, PRMD_WETEn_04, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountAnalogLeaf);
	//....

	m_vecComponent.push_back(pComposite);

	// DRY cleaning
	pComposite = new CPreventiveMaintenance_Composite(DRY_CLEANING_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	// pComposite->InitializeChannelInformation(PRMD_DRYRstCnt, PRMS_DRYRstTime, PRMS_DRYRstTime01, PRMD_DRYAlarm);
	pComposite->InitializeChannelInformation(PRMD_DRYRstCnt, PRMS_DRYRstTime, PRMS_DRYRstTime01, PRMS_DRYRstCount01, PRMD_DRYAlarm);

	nChannelID = PRMD_DRYCWfrCnt;
	nAlarmID = WARNING__DRY_CLEANING__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_DRYEn_01, PRMD_DRYAlarm, TRUE); // 2006.02.20
	pComposite->AddComponent(pCountLeaf);

	// Dry cleaning 2    2009.08.24
	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+9, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_DRYEn_01, PRMD_DRYAlarm, TRUE); 
	pComposite->AddComponent(pCountLeaf);
	// end

	pCountLeaf = new CCountBase_Leaf(VALVE_CYCLE_COUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nAlarmID+2, nAlarmID+3, PRMD_DRYEn_02, PRMD_DRYAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountAnalogLeaf = new CCountAnalogBase_Leaf(SOURCE_FEEDING_AMOUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountAnalogLeaf->InitializeChannelInformation(nChannelID+6, nChannelID+7, nChannelID+8, nAlarmID+4, nAlarmID+5, PRMD_DRYEn_03, PRMD_DRYAlarm, TRUE); // 2006.02.20
	pComposite->AddComponent(pCountAnalogLeaf);

	// 2006.02.20 Dry Cleaning For Thickness
	pCountAnalogLeaf = new CCountAnalogBase_Leaf(THICKNESS_AMOUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountAnalogLeaf->InitializeChannelInformation(PRMA_DRYCThick, PRMA_DRYCThick+1, PRMA_DRYCThick+2,
				WARNING__DRY_CLEANING__THICKNESS_AMOUNT__OVER,
				ALARM__DRY_CLEANING__THICKNESS_AMOUNT__OVER,
				PRMD_DRYEn_04, PRMD_DRYAlarm, TRUE);
	pComposite->AddComponent(pCountAnalogLeaf);

	// Dry Cleaning All Item Add to Vector
	m_vecComponent.push_back(pComposite);

	// Source Exchange
	pComposite = new CPreventiveMaintenance_Composite(SOURCE_EXCHANGE_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	// pComposite->InitializeChannelInformation(PRMD_SRCRstCnt, PRMS_SRCRstTime, PRMS_SRCRstTime01, PRMD_SRCAlarm);
	pComposite->InitializeChannelInformation(PRMD_SRCRstCnt, PRMS_SRCRstTime, PRMS_SRCRstTime01,PRMS_SRCRstCount01, PRMD_SRCAlarm);

	nChannelID = PRMD_SRCCWfrCnt;
	nAlarmID = WARNING__SOURCE_EXCHANGE__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , SOURCE_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_SRCEn_01, PRMD_SRCAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	// Source Exchange 2      2008.08.24
	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , SOURCE_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+17, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_SRCEn_01, PRMD_SRCAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);
	// end

	pCountLeaf = new CCountBase_Leaf(VALVE_CYCLE_COUNT_LEAF_TYPE , SOURCE_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nAlarmID+2, nAlarmID+3, PRMD_SRCEn_02, PRMD_SRCAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountAnalogLeaf = new CCountAnalogBase_Leaf(SOURCE_FEEDING_AMOUNT_LEAF_TYPE , SOURCE_EXCHANGE_COMPOSITE_TYPE);
	pCountAnalogLeaf->InitializeChannelInformation(nChannelID+6, nChannelID+7, nChannelID+8, nAlarmID+4, nAlarmID+5, PRMD_SRCEn_03, PRMD_SRCAlarm, TRUE);
	pComposite->AddComponent(pCountAnalogLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+9, nChannelID+10, nChannelID+11, nChannelID+12, nChannelID+13, nChannelID+14, nChannelID+15, nChannelID+16, nAlarmID+6, nAlarmID+7, PRMD_SRCEn_04, PRMD_SRCAlarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);

	// Stage Heater Exchange
	pComposite = new CPreventiveMaintenance_Composite(STAGE_HEATER_EXCHANGE_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	// pComposite->InitializeChannelInformation(PRMD_STGRstCnt, PRMS_STGRstTime, PRMS_STGRstTime01, PRMD_STGAlarm);
	pComposite->InitializeChannelInformation(PRMD_STGRstCnt, PRMS_STGRstTime, PRMS_STGRstTime01,PRMS_STGRstCount01, PRMD_STGAlarm);

	nChannelID = PRMD_STGCWfrCnt;
	nAlarmID = WARNING__STG_HEATER_EXCHANGE__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , STAGE_HEATER_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_STGEn_01, PRMD_STGAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_STGEn_02, PRMD_STGAlarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);

	// Stage Heater2 Exchange
	pComposite = new CPreventiveMaintenance_Composite(STAGE_HEATER2_EXCHANGE_COMPOSITE_TYPE);

	pComposite->InitializeChannelInformation(PRMD_STG2RstCnt, PRMS_STG2RstTime, PRMS_STG2RstTime01,PRMS_STG2RstCount01, PRMD_STG2Alarm);

	nChannelID = PRMD_STG2CWfrCnt;
	nAlarmID = WARNING__STG_HEATER2_EXCHANGE__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , STAGE_HEATER2_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_STG2En_01, PRMD_STG2Alarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_STG2En_02, PRMD_STG2Alarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);
	if (TRUE == bRFMonitoring)
	{
		// RF Generator ON Time
		pComposite = new CPreventiveMaintenance_Composite(RF_GENERATOR_COMPOSITE_TYPE);
		pComposite->InitializeChannelInformation(PRMD_RFGRstCnt, PRMS_RFGRstTime, PRMS_RFGRstTime01, PRMD_RFGAlarm);
		nChannelID = PRMD_RFGCLTDay;
		nAlarmID = WARNING__RF_GENERATOR__ON_TIME__OVER;

		pTimeLeaf = new CTimeBase_Leaf(RF_GENERATOR_ON_LEAF_TYPE, TRUE);
		pTimeLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nAlarmID, nAlarmID+1, PRMD_RFGEn_01, PRMD_RFGAlarm, TRUE);
		pComposite->AddComponent(pTimeLeaf);
		m_vecComponent.push_back(pComposite);
	}

	// Dry Pump Exchange
	pComposite = new CPreventiveMaintenance_Composite(DRY_PUMP_EXCHANGE_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	// pComposite->InitializeChannelInformation(PRMD_DPRstCnt, PRMS_DPRstTime, PRMS_DPRstTime01, PRMD_DPAlarm);
	pComposite->InitializeChannelInformation(PRMD_DPRstCnt, PRMS_DPRstTime, PRMS_DPRstTime01,PRMS_DPRstCount01, PRMD_DPAlarm);

	nChannelID = PRMD_DPCWfrCnt;
	nAlarmID = WARNING__DRY_PUMP_EXCHANGE__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , DRY_PUMP_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_DPEn_01, PRMD_DPAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_DPEn_02, PRMD_DPAlarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);

	// TRAP Exchange
	pComposite = new CPreventiveMaintenance_Composite(TRAP_EXCHANGE_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	// pComposite->InitializeChannelInformation(PRMD_TRAPRstCnt, PRMS_TRAPRstTime, PRMS_TRAPRstTime01, PRMD_TRAPAlarm);
	pComposite->InitializeChannelInformation(PRMD_TRAPRstCnt, PRMS_TRAPRstTime, PRMS_TRAPRstTime01,PRMS_TRAPRstCount01, PRMD_TRAPAlarm);

	nChannelID = PRMD_TRAPCWfrCnt;
	nAlarmID = WARNING__TRAP_EXCHANGE__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , TRAP_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_TRAPEn_01, PRMD_TRAPAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_TRAPEn_02, PRMD_TRAPAlarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);

	// Maintenance Period A
	pComposite = new CPreventiveMaintenance_Composite(MAINTENANCE_PERIOD_A_EXCHANGE_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	//pComposite->InitializeChannelInformation(PRMD_MPARstCnt, PRMS_MPARstTime, PRMS_MPARstTime01, PRMD_MPAAlarm);
	pComposite->InitializeChannelInformation(PRMD_MPARstCnt, PRMS_MPARstTime, PRMS_MPARstTime01,PRMS_MPARstCount01, PRMD_MPAAlarm);

	nChannelID = PRMD_MPACWfrCnt;
	nAlarmID = WARNING__MAINTENANCE_PERIOD_A__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , MAINTENANCE_PERIOD_A_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_MPAEn_01, PRMD_MPAAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_MPAEn_02, PRMD_MPAAlarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);

	// Maintenance Period B
	pComposite = new CPreventiveMaintenance_Composite(MAINTENANCE_PERIOD_B_EXCHANGE_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	// pComposite->InitializeChannelInformation(PRMD_MPBRstCnt, PRMS_MPBRstTime, PRMS_MPBRstTime01, PRMD_MPBAlarm);
	pComposite->InitializeChannelInformation(PRMD_MPBRstCnt, PRMS_MPBRstTime, PRMS_MPBRstTime01,PRMS_MPBRstCount01, PRMD_MPBAlarm);

	nChannelID = PRMD_MPBCWfrCnt;
	nAlarmID = WARNING__MAINTENANCE_PERIOD_B__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , MAINTENANCE_PERIOD_B_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_MPBEn_01, PRMD_MPBAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_MPBEn_02, PRMD_MPBAlarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);

	// Maintenance Period C
	pComposite = new CPreventiveMaintenance_Composite(MAINTENANCE_PERIOD_C_EXCHANGE_COMPOSITE_TYPE);

	// modified by ycchoi 2006.05.28 < 1.0.0.2 >
	// pComposite->InitializeChannelInformation(PRMD_MPCRstCnt, PRMS_MPCRstTime, PRMS_MPCRstTime01, PRMD_MPCAlarm);
	pComposite->InitializeChannelInformation(PRMD_MPCRstCnt, PRMS_MPCRstTime, PRMS_MPCRstTime01,PRMS_MPCRstCount01, PRMD_MPCAlarm);

	nChannelID = PRMD_MPCCWfrCnt;
	nAlarmID = WARNING__MAINTENANCE_PERIOD_C__WAFER_COUNT__OVER;

	pCountLeaf = new CCountBase_Leaf(WAFER_COUNT_LEAF_TYPE , MAINTENANCE_PERIOD_C_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_MPCEn_01, PRMD_MPCAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_MPCEn_02, PRMD_MPCAlarm, TRUE);
	pComposite->AddComponent(pTimeLeaf);
	m_vecComponent.push_back(pComposite);

	if(! CreateThread(Thrd0_OneSecondPolling))
		::MessageBox(NULL, "Thread Creation Failed....", "Preventive Maintenance", MB_ICONERROR|MB_SYSTEMMODAL|MB_OK);
	if (TRUE == bRFMonitoring)
	{
		if(! CreateThread(Thrd1_RFONMonitoring))
			::MessageBox(NULL, "Thread Creation Failed....", "Preventive Maintenance", MB_ICONERROR|MB_SYSTEMMODAL|MB_OK);
	}	
}
