// PreventiveMaintenance_Client_Type2.cpp: implementation of the CPreventiveMaintenance_Client_Type2 class.
// 2006.10.18
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CPreventiveMaintenance_Component.h"
#include "CPreventiveMaintenance_Composite.h"
#include "CountAnalogBase_Leaf_Type2.h"
#include "CountBase_Leaf_Type2.h"
#include "CTimeBase_Leaf.h"
#include "PreventiveMaintenance_Client_Type2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPreventiveMaintenance_Client_Type2::CPreventiveMaintenance_Client_Type2(BOOL bRFMonitoring)
	: CPreventiveMaintenance_Client()
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_WETEn_01, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountLeaf = new CCountBase_Leaf_Type2(VALVE_CYCLE_COUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nAlarmID+2, nAlarmID+3, PRMD_WETEn_02, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountAnalogLeaf = new CCountAnalogBase_Leaf_Type2(SOURCE_FEEDING_AMOUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
	pCountAnalogLeaf->InitializeChannelInformation(nChannelID+6, nChannelID+7, nChannelID+8, nAlarmID+4, nAlarmID+5, PRMD_WETEn_03, PRMD_WETAlarm, TRUE);
	pComposite->AddComponent(pCountAnalogLeaf);
	//... 2018.06.17 WET Thick	
	pCountAnalogLeaf = new CCountAnalogBase_Leaf_Type2(THICKNESS_AMOUNT_LEAF_TYPE , WET_CLEANING_COMPOSITE_TYPE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_DRYEn_01, PRMD_DRYAlarm, TRUE); // 2006.02.20
	pComposite->AddComponent(pCountLeaf);

	pCountLeaf = new CCountBase_Leaf_Type2(VALVE_CYCLE_COUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nAlarmID+2, nAlarmID+3, PRMD_DRYEn_02, PRMD_DRYAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountAnalogLeaf = new CCountAnalogBase_Leaf_Type2(SOURCE_FEEDING_AMOUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
	pCountAnalogLeaf->InitializeChannelInformation(nChannelID+6, nChannelID+7, nChannelID+8, nAlarmID+4, nAlarmID+5, PRMD_DRYEn_03, PRMD_DRYAlarm, TRUE); // 2006.02.20
	pComposite->AddComponent(pCountAnalogLeaf);

	// 2006.02.20 Dry Cleaning For Thickness
	pCountAnalogLeaf = new CCountAnalogBase_Leaf_Type2(THICKNESS_AMOUNT_LEAF_TYPE , DRY_CLEANING_COMPOSITE_TYPE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , SOURCE_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_SRCEn_01, PRMD_SRCAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountLeaf = new CCountBase_Leaf_Type2(VALVE_CYCLE_COUNT_LEAF_TYPE , SOURCE_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nAlarmID+2, nAlarmID+3, PRMD_SRCEn_02, PRMD_SRCAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pCountAnalogLeaf = new CCountAnalogBase_Leaf_Type2(SOURCE_FEEDING_AMOUNT_LEAF_TYPE , SOURCE_EXCHANGE_COMPOSITE_TYPE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , STAGE_HEATER_EXCHANGE_COMPOSITE_TYPE);
	pCountLeaf->InitializeChannelInformation(nChannelID, nChannelID+1, nChannelID+2, nAlarmID, nAlarmID+1, PRMD_STGEn_01, PRMD_STGAlarm, TRUE);
	pComposite->AddComponent(pCountLeaf);

	pTimeLeaf = new CTimeBase_Leaf(LIFE_TIME_LEAF_TYPE, FALSE);
	pTimeLeaf->InitializeChannelInformation(nChannelID+3, nChannelID+4, nChannelID+5, nChannelID+6, nChannelID+7, nChannelID+8, nChannelID+9, nChannelID+10, nAlarmID+2, nAlarmID+3, PRMD_STGEn_02, PRMD_STGAlarm, TRUE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , DRY_PUMP_EXCHANGE_COMPOSITE_TYPE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , TRAP_EXCHANGE_COMPOSITE_TYPE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , MAINTENANCE_PERIOD_A_EXCHANGE_COMPOSITE_TYPE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , MAINTENANCE_PERIOD_B_EXCHANGE_COMPOSITE_TYPE);
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

	pCountLeaf = new CCountBase_Leaf_Type2(WAFER_COUNT_LEAF_TYPE , MAINTENANCE_PERIOD_C_EXCHANGE_COMPOSITE_TYPE);
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

	/*
	if (NULL == ::CreateThread(NULL, 0, Thread_OneSecondPolling, (LPVOID)this, 0, NULL))
		::MessageBox(NULL, "Thread Creation Failed....", "Preventive Maintenance", MB_ICONERROR|MB_SYSTEMMODAL|MB_OK);
	if (TRUE == bRFMonitoring)
	{
		if (NULL == ::CreateThread(NULL, 0, Thread_RFONMonitoring, (LPVOID)this, 0, NULL))
			::MessageBox(NULL, "Thread Creation Failed....", "Preventive Maintenance", MB_ICONERROR|MB_SYSTEMMODAL|MB_OK);
	}
	*/
}

CPreventiveMaintenance_Client_Type2::~CPreventiveMaintenance_Client_Type2()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function
