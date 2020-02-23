// DummyMonitor.cpp: implementation of the CDummyMonitor class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "DummyMonitor.h"
#include "DummySlotInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDummyMonitor::CDummyMonitor()
{
	m_pDummySlotInfo  = new CDummySlotInfo();	
	m_nAlarmStartID   = 0;
	m_nWarningStartID = 0;

	m_hMutex          = NULL;
}

CDummyMonitor::~CDummyMonitor()
{
	if (NULL != m_pDummySlotInfo)
		delete m_pDummySlotInfo;
	
	if (NULL != m_hMutex)
		CloseHandle(m_hMutex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitializeMonitor()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDummyMonitor::InitializeMonitor(int nAlarm , int nWarning)
{
	BOOL bRet = FALSE;
	do {
		//Initialize Dummy Slot Member
		if (NULL != m_pDummySlotInfo)
			m_pDummySlotInfo->InitializeMember();
		else break;

		//Initial Save Setted Dummy Mode
		InitialSaveDummyMode();

		//Save Alarm ID by given parameter
		m_nAlarmStartID   = nAlarm;
		m_nWarningStartID = nWarning;

		//CreateMutex
		if (FALSE == InitLock())
			break;		
		
		//Create Monitoring Thread
        if (NULL == ::CreateThread(NULL, 0, Thread_DummyMonitoring, (LPVOID)this, 0, NULL))
			break;		

		bRet = TRUE;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitLock()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDummyMonitor::InitLock()
{
	BOOL bRet = TRUE;

	m_hMutex = CreateMutex(NULL, FALSE, NULL);
	if (NULL == m_hMutex)
		return FALSE;

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : LockSharedData()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummyMonitor::LockSharedData  (HANDLE hMutex)
{
	if (NULL != hMutex) {
		WaitForSingleObject(hMutex, INFINITE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : UnlockSharedData()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummyMonitor::UnlockSharedData(HANDLE hMutex)
{
	if (NULL != hMutex)
		ReleaseMutex(hMutex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitialSaveDummyMode()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummyMonitor::InitialSaveDummyMode()
{
	int  nCommStatus;
	long enMode = -1;
	for (int i = 0 ; i < MAX_SLOT ; i++) {
		enMode = READ_DIGITAL(Dummy_Used_Mode1+i, &nCommStatus);
		m_pDummySlotInfo->SetDummyMode(i , enMode);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : ChangeSlotDummyMode()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDummyMonitor::ChangeSlotDummyMode(int nSlot , int nMode)
{
	BOOL bRet = FALSE;
	int nCommStatus;

	do {
		char szEvent[64] = {0};
		if (nSlot < 1 || nSlot > 25) break;
		if (nMode != 0 && nMode != 5 && nMode != 6 && nMode != 7) break;
		
		sprintf(szEvent , "INEXP_DUMMY %d|MODE|%d" , nSlot , nMode);
		WRITE_STRING(SCHEDULER , szEvent , &nCommStatus);

		LockSharedData(m_hMutex);
		////////////////////////////////////////////////////////////////
		m_pDummySlotInfo->SetDummyMode(nSlot-1 , nMode);
		////////////////////////////////////////////////////////////////
		UnlockSharedData(m_hMutex);

		bRet = TRUE;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : RemoveDummy()
//Date         : 2007.05.08
//Description  : Reset Current Dummy Used Count when Cassette for dummy is removed
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummyMonitor::RemoveDummy()
{
	int nCommStatus;

	//Send Reset Event to Scheduler about all slot
	for (int i = 0 ; i < MAX_SLOT ; i++) {
		char szEvent[64] = {0};
		sprintf(szEvent , "INEXP_DUMMY %d|RESET" , i+1);
		WRITE_STRING(SCHEDULER , szEvent , &nCommStatus);	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : Thread_DummyMonitoring()
//Date         : 2007.05.08
//Description  : Monitoring current value by io & Alarm Post & Clear
//Arguments    :
//Return Value : DWORD
//////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CDummyMonitor::Thread_DummyMonitoring(LPVOID lpParam)
{
	int nCommStatus , i , j;
	int nCurrent , nWarning , nAlarm;
	int nCM1RunStatus , nCM2RunStatus , nCM3RunStatus;
	CDummyMonitor* pThis = (CDummyMonitor*)lpParam;

	while (1) {
		/*if (0 == READ_DIGITAL(DUMMY_USE_MODE, &nCommStatus)) {
			Sleep(1000);
			continue;
		}*/

		/*if (0 == READ_DIGITAL(DUMMY_CASSETTE_STATUS, &nCommStatus)) {
			Sleep(1000);
			continue;
		}*/

		//1.Count Compare
		for (i = 0; i < MAX_SLOT ; i++) {
			nCurrent = READ_DIGITAL(Current_Dummy_Prcs_Count1 + i, &nCommStatus);
			nWarning = READ_DIGITAL(Dummy_Used_Warning_Count1 + i, &nCommStatus);
			nAlarm   = READ_DIGITAL(Dummy_Used_Alarm_Count1   + i, &nCommStatus);

			if (nCurrent >= nWarning) {
				if (nCurrent >= nAlarm) {
					//Clear Warning Count Alarm
					if (W_POSTED == pThis->m_pDummySlotInfo->GetAlarmStatus(i)) {
						ALARM_CLEAR(pThis->m_nWarningStartID + i);
					} else if (A_POSTED == pThis->m_pDummySlotInfo->GetAlarmStatus(i) && ALM_PAUSED == ALARM_STATUS(pThis->m_nAlarmStartID + i)) {
						//2008.08.27 Added for Prevent Alarm RePost
						continue;
					}

					//Post Alarm
					ALARM_POST(pThis->m_nAlarmStartID + i);
					if (NULL != pThis->m_pDummySlotInfo) {
						pThis->m_pDummySlotInfo->SetAlarmStatus(i , A_POSTED);
					}
				} else {
					//Clear Alarm Count Alarm
					if (A_POSTED == pThis->m_pDummySlotInfo->GetAlarmStatus(i)) {
						ALARM_CLEAR(pThis->m_nAlarmStartID + i);

						//This item got alarm count but now changed to normal so this flag set
						pThis->m_pDummySlotInfo->SetChangeFlag(i);
					} else if (W_POSTED == pThis->m_pDummySlotInfo->GetAlarmStatus(i) && ALM_PAUSED == ALARM_STATUS(pThis->m_nWarningStartID + i)) {
						//2008.08.27 Added for Prevent Warning RePost
						continue;
					}

					//Post Warning
					ALARM_POST(pThis->m_nWarningStartID + i);
					if (NULL != pThis->m_pDummySlotInfo) {
						pThis->m_pDummySlotInfo->SetAlarmStatus(i , W_POSTED);
					}
				}
			} else {
				//Check Alarm & Clear Alarm
				if (W_POSTED == pThis->m_pDummySlotInfo->GetAlarmStatus(i)) {
						ALARM_CLEAR(pThis->m_nWarningStartID + i);
						pThis->m_pDummySlotInfo->SetAlarmStatus(i , NO_ALARM);
				} else if (A_POSTED == pThis->m_pDummySlotInfo->GetAlarmStatus(i)) {
						ALARM_CLEAR(pThis->m_nAlarmStartID + i);				
						pThis->m_pDummySlotInfo->SetAlarmStatus(i , NO_ALARM);
				}
			}

			Sleep(100);
		}

		//2. Running Lot Check
		nCM1RunStatus =  READ_DIGITAL(MAIN_CONTROL , &nCommStatus);
		nCM2RunStatus =  READ_DIGITAL(MAIN_CONTROL2, &nCommStatus);
		nCM3RunStatus =  READ_DIGITAL(MAIN_CONTROL3, &nCommStatus);
		if ((0 == nCM1RunStatus || 7 == nCM1RunStatus) && (0 == nCM2RunStatus || 7 == nCM2RunStatus) && (0 == nCM3RunStatus || 7 == nCM3RunStatus)) {
			//Locking
			pThis->LockSharedData(pThis->m_hMutex);
			///////////////////////////////////////////////////////////////////////////////////////////

			//Check Dummy Slot Info & Dummy Mode Change about Reached Alarm Count Item
			for (j = 0 ; j < MAX_SLOT ; j++) {
				char szEvent[64] = {0};
				if (A_POSTED == pThis->m_pDummySlotInfo->GetAlarmStatus(j)) {
					sprintf(szEvent , "INEXP_DUMMY %d|MODE|0" , j+1);
					WRITE_STRING( SCHEDULER , szEvent , &nCommStatus);					
				} else {
					if (NA != pThis->m_pDummySlotInfo->GetDummyMode(j) && TRUE == pThis->m_pDummySlotInfo->GetChangeFlag(j)) {
						sprintf(szEvent , "INEXP_DUMMY %d|MODE|%d" , j+1 , pThis->m_pDummySlotInfo->GetDummyMode(j));
						WRITE_STRING( SCHEDULER , szEvent , &nCommStatus);	
						pThis->m_pDummySlotInfo->ResetChangeFlag(j);
					}
				}
			}

			//Unlocking
			pThis->UnlockSharedData(pThis->m_hMutex);
			///////////////////////////////////////////////////////////////////////////////////////////
		}

		Sleep(1000);	
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : ResetCount()
//Date         : 2007.12.06
//Description  : 
//Arguments    :
//Return Value : 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummyMonitor::ResetCount(char* szSlot)
{
	int  nCommStatus;
	int  nSlot = 0;
	char szEvent[64] = {0};

	if (NULL == szSlot) return;

	nSlot = atoi(szSlot);
	if (nSlot < 0 || nSlot > MAX_SLOT) return;

	//reset slot	
	sprintf(szEvent , "INEXP_DUMMY %d|RESET" , nSlot);
	WRITE_STRING(SCHEDULER , szEvent , &nCommStatus);	
}