// // Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CModuleConditionControl_Client.h"

#include "CModuleConditionControl_Component.h"
#include "CModuleConditionControl_Composite.h"
#include "CRecipeControl_Leaf.h"
#include "CLeakCheckControl_Leaf.h"
#include "CPreTreatmentControl_Leaf.h"
#include "MsgBoxDll.h"

// 2007.02.23 by ycchoi < 1.0.0.7 >
extern BOOL g_bDryCleanBuzzerOn;
extern BOOL g_bDryCleanMsgBoxOn;
extern BOOL g_bAutoDryClean;

void CModuleConditionControl_Client::SetAutoControl(BOOL bEnable, int nIndexComposite, int nIndexLeaf)
{
	vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin() + nIndexComposite;
	(*theIter)->SetAutoControl(bEnable, nIndexLeaf);
}

void CModuleConditionControl_Client::ClearRecipe(int nComposite , int nLeaf)
{
	vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin() + nComposite;
	(*theIter)->ClearRecipe(nComposite, nLeaf);
}

void CModuleConditionControl_Client::SetModuleModeToEnableAfterRun(BOOL bEnable, int nIndexComposite)
{
	vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin() + nIndexComposite;
	(*theIter)->SetModuleModeToEnableAfterRun(bEnable);
}

BOOL CModuleConditionControl_Client::CheckChamberConnection(BOOL bAutoControl)
{
	if (2 == READ_DIGITAL(COMM_STATUS, &g_nCommStatus))		// DISCONNECT(2)
	{
		if (FALSE == bAutoControl)
		{
			if (ALM_PAUSED != ALARM_STATUS(ALARM__CHAMBER_DISCONNECTED))
				ALARM_POST(ALARM__CHAMBER_DISCONNECTED);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CModuleConditionControl_Client::CheckChamberEnable(BOOL bAutoControl)
{
	if ((3 != READ_DIGITAL(COMM_STATUS, &g_nCommStatus))		// Standby(3)
		// && (7 != READ_DIGITAL(COMM_STATUS, &g_nCommStatus))		// CTCInUse(7)
		)
//	if (1 != READ_DIGITAL(FA_STATUS, &g_nCommStatus))			// NOT ENABLE(1)
	{
		if (FALSE == bAutoControl)
		{
			if (ALM_PAUSED != ALARM_STATUS(ALARM__CHAMBER_NOT_ENABLED))
				ALARM_POST(ALARM__CHAMBER_NOT_ENABLED);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CModuleConditionControl_Client::CheckGateValve(BOOL bAutoControl)
{
	if (__SLOT_CLOSED != READ_DIGITAL(SLOT_VALVE1_XI, &g_nCommStatus) || __SLOT_CLOSED != READ_DIGITAL(SLOT_VALVE2_XI, &g_nCommStatus))
	{
		if (FALSE == bAutoControl)
		{
			if (ALM_PAUSED != ALARM_STATUS(ALARM__SLOT_NOT_CLOSED))
				ALARM_POST(ALARM__SLOT_NOT_CLOSED);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CModuleConditionControl_Client::CheckWaferExistence(BOOL bAutoControl)
{
	if (__WAFER_NOT_PRESENT != READ_DIGITAL(WAFER_STATUS, &g_nCommStatus))
	{
		if (FALSE == bAutoControl)
		{
			if (ALM_PAUSED != ALARM_STATUS(ALARM__WAFER_PRESENT_IN_CHAMBER))
				ALARM_POST(ALARM__WAFER_PRESENT_IN_CHAMBER);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CModuleConditionControl_Client::CheckSequenceNotRunning(BOOL bAutoControl)
{
	if (__SEQUENCE_RUNNING == READ_DIGITAL(PROCESS_RUN_STATUS, &g_nCommStatus))
	{
		if (FALSE == bAutoControl)
		{
			if (ALM_PAUSED != ALARM_STATUS(ALARM__SEQUENCE_RUNNING))
				ALARM_POST(ALARM__SEQUENCE_RUNNING);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CModuleConditionControl_Client::CheckLOTNotRunning(BOOL bAutoControl)
{
	if (7 == READ_DIGITAL(COMM_STATUS, &g_nCommStatus))		// CTCinUSE(7)
	{
		if (FALSE == bAutoControl)
		{
			if (ALM_PAUSED != ALARM_STATUS(ALARM__LOT_RUNNING))
				ALARM_POST(ALARM__LOT_RUNNING);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CModuleConditionControl_Client::CheckDRYCleaningCondition(BOOL bAutoControl)
{
	BOOL bRet = FALSE;
	do
	{
		// Running function check
		vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin();
		for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
		{
			if ((TRUE == (*theIter)->GetRunningStatus()) && (IDLE_CONTROL_COMPOSITE_TYPE != (*theIter)->GetType()))
				break;
		}
		if (theIter != m_vecComponent.end())	break;

		theIter = m_vecComponent.begin() + DRY_CLEANING_COMPOSITE_TYPE;
		if (FALSE == (*theIter)->GetAutoControl())
			break;

		if (FALSE == CheckChamberConnection(bAutoControl))	break;

		if (TRUE == m_bRunPost)
		{
			for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
			{
				if ((TRUE == (*theIter)->GetRunningStatus()) && (IDLE_CONTROL_COMPOSITE_TYPE == (*theIter)->GetType()))
					break;
			}
			if (theIter == m_vecComponent.end())	break;
		}
		if (TRUE == bAutoControl)
		{
			if (__NORWARALA_NORMAL == READ_DIGITAL(DRY_CLEANING_ALARM, &g_nCommStatus))
				break;
			if (FALSE == CheckChamberEnable(bAutoControl))	break;
		}

		if (FALSE == CheckGateValve(bAutoControl))	break;
		if (FALSE == CheckWaferExistence(bAutoControl))	break;

		if (__NORWARALA_ALARM == READ_DIGITAL(DRY_CLEANING_ALARM, &g_nCommStatus))
		{
			// For change module mode to disable
			Abort(IDLE_CONTROL_COMPOSITE_TYPE);
		}
		else
		{
			if (FALSE == CheckLOTNotRunning(bAutoControl))	break;
		}

		if (FALSE == CheckSequenceNotRunning(TRUE))
		{
			theIter = m_vecComponent.begin();
			for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
			{
				if ((TRUE == (*theIter)->GetRunningStatus()) && (IDLE_CONTROL_COMPOSITE_TYPE == (*theIter)->GetType()))
					break;
			}
			if (theIter != m_vecComponent.end())
			{
				// For change module mode to disable
				Abort(IDLE_CONTROL_COMPOSITE_TYPE);
			}
			else if (FALSE == bAutoControl)
			{
				if (ALM_PAUSED != ALARM_STATUS(ALARM__SEQUENCE_RUNNING))
					ALARM_POST(ALARM__SEQUENCE_RUNNING);
				break;
			}
			else
				break;
		}
		else
		{
			// For change module mode to disable
			Abort(IDLE_CONTROL_COMPOSITE_TYPE);
		}
		MsgParser("WAIT_IDLE_FINISHED");

		bRet = TRUE;
	} while(0);

	return bRet;
}

BOOL CModuleConditionControl_Client::CheckWETCleaningCondition(BOOL bAutoControl)
{
	BOOL bRet = FALSE;
	do
	{
		// Running function check
		vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin();
		for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
		{
			if ((TRUE == (*theIter)->GetRunningStatus()) && (IDLE_CONTROL_COMPOSITE_TYPE != (*theIter)->GetType()))
				break;
		}
		if (theIter != m_vecComponent.end())	break;

		theIter = m_vecComponent.begin() + WET_CLEANING_COMPOSITE_TYPE;
		if (FALSE == (*theIter)->GetAutoControl())
			break;

		if (FALSE == CheckChamberConnection(bAutoControl))	break;

		if (TRUE == m_bRunPost)
		{
			for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
			{
				if ((TRUE == (*theIter)->GetRunningStatus()) && (IDLE_CONTROL_COMPOSITE_TYPE == (*theIter)->GetType()))
					break;
			}
			if (theIter == m_vecComponent.end())	break;
		}
		if (TRUE == bAutoControl)
		{
			if (__NORWARALA_NORMAL == READ_DIGITAL(WET_CLEANING_ALARM, &g_nCommStatus))
				break;
			if (FALSE == CheckChamberEnable(bAutoControl))	break;
		}

		if (FALSE == CheckGateValve(bAutoControl))	break;
		if (FALSE == CheckWaferExistence(bAutoControl))	break;

		if (__NORWARALA_ALARM == READ_DIGITAL(WET_CLEANING_ALARM, &g_nCommStatus))
		{
			// For change module mode to disable
			Abort(IDLE_CONTROL_COMPOSITE_TYPE);
		}
		else
		{
			if (FALSE == CheckLOTNotRunning(bAutoControl))	break;
		}

		if (FALSE == CheckSequenceNotRunning(TRUE))
		{
			theIter = m_vecComponent.begin();
			for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
			{
				if ((TRUE == (*theIter)->GetRunningStatus()) && (IDLE_CONTROL_COMPOSITE_TYPE == (*theIter)->GetType()))
					break;
			}
			if (theIter != m_vecComponent.end())
			{
				// For change module mode to disable
				Abort(IDLE_CONTROL_COMPOSITE_TYPE);
			}
			else if (FALSE == bAutoControl)
			{
				if (ALM_PAUSED != ALARM_STATUS(ALARM__SEQUENCE_RUNNING))
					ALARM_POST(ALARM__SEQUENCE_RUNNING);
				break;
			}
			else
				break;
		}
		else
		{
			// For change module mode to disable
			Abort(IDLE_CONTROL_COMPOSITE_TYPE);
		}
		MsgParser("WAIT_IDLE_FINISHED");

		bRet = TRUE;
	} while(0);

	return bRet;
}

BOOL CModuleConditionControl_Client::CheckIDLECondition(BOOL bAutoControl)
{
	BOOL bRet = FALSE;
	do
	{
		// Running function check
		vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin();
		for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
		{
			if (TRUE == (*theIter)->GetRunningStatus())
				break;
		}
		if (theIter != m_vecComponent.end())	break;

		theIter = m_vecComponent.begin() + IDLE_CONTROL_COMPOSITE_TYPE;
		if (FALSE == (*theIter)->GetAutoControl())
			break;

		if (FALSE == CheckChamberConnection(bAutoControl))	break;

		if (TRUE == bAutoControl)
		{
			theIter = m_vecComponent.begin() + IDLE_CONTROL_COMPOSITE_TYPE;
			if (m_doubIDLETime < (*theIter)->GetWaitingTime())	break;

			if (FALSE == CheckChamberEnable(bAutoControl))	break;
		}

		if (FALSE == CheckGateValve(bAutoControl))	break;
		if (FALSE == CheckWaferExistence(bAutoControl))	break;

		if (FALSE == CheckSequenceNotRunning(bAutoControl))	break;

		bRet = TRUE;
	} while(0);

	return bRet;
}

BOOL CModuleConditionControl_Client::CheckPreTreatmentCondition(BOOL bAutoControl)
{
	BOOL bRet = FALSE;
	do
	{
		vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin() + PRE_TREATMENT_COMPOSITE_TYPE;
		if (FALSE == (*theIter)->GetAutoControl())
			break;

		if (FALSE == CheckChamberConnection(bAutoControl))	break;
		if (FALSE == CheckChamberEnable(bAutoControl))
		{
			WRITE_DIGITAL(PRO_PreTreatDM, __ON, &g_nCommStatus);
			bRet = TRUE;
			break;
		}

		if (TRUE == bAutoControl)
		{
			theIter = m_vecComponent.begin() + PRE_TREATMENT_COMPOSITE_TYPE;
			if (m_doubIDLETime < (*theIter)->GetWaitingTime())	break;
		}

		WRITE_DIGITAL(PRO_PreTreatDM, __ON, &g_nCommStatus);

		bRet = TRUE;
	} while(0);

	return bRet;
}

BOOL CModuleConditionControl_Client::Run(int nIndexComposite, BOOL bAutoControl)
{
	BOOL bRet = FALSE;
	do
	{
		if (DRY_CLEANING_COMPOSITE_TYPE == nIndexComposite)
		{
//			if (FALSE == CheckDRYCleaningCondition(bAutoControl))	break;
			if (FALSE == CheckSequenceNotRunning(bAutoControl))	break;
		}
		else if (WET_CLEANING_COMPOSITE_TYPE == nIndexComposite)
		{
//			if (FALSE == CheckWETCleaningCondition(bAutoControl))	break;
			if (FALSE == CheckSequenceNotRunning(bAutoControl))	break;
		}
		//2007.07.04 Added WetBackup + Error Backup
		else if (WET_BACKUP_COMPOSITE_TYPE == nIndexComposite)
		{
//			if (FALSE == CheckWETCleaningCondition(bAutoControl))	break;
			if (FALSE == CheckSequenceNotRunning(bAutoControl))	break;
		}
		else if (ERROR_BACKUP_COMPOSITE_TYPE == nIndexComposite)
		{
//			if (FALSE == CheckWETCleaningCondition(bAutoControl))	break;
			if (FALSE == CheckSequenceNotRunning(bAutoControl))	break;
		}
		else if (ERROR_BACKUP2_COMPOSITE_TYPE == nIndexComposite)
		{
//			if (FALSE == CheckWETCleaningCondition(bAutoControl))	break;
			if (FALSE == CheckSequenceNotRunning(bAutoControl))	break;
		}
		//End 2007.07.04
		else if (IDLE_CONTROL_COMPOSITE_TYPE == nIndexComposite)
		{
			if (FALSE == CheckIDLECondition(bAutoControl))			break;
		}
		else if (PRE_TREATMENT_COMPOSITE_TYPE == nIndexComposite)
		{
			vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin() + PRE_TREATMENT_COMPOSITE_TYPE;
			if (m_doubIDLETime < (*theIter)->GetWaitingTime())		break;
		}

		vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin() + nIndexComposite;
		if (FALSE == (*theIter)->Run(bAutoControl))
			break;

		if (DRY_CLEANING_COMPOSITE_TYPE == nIndexComposite)
		{
			RUN_FUNCTION(PREVENTIVE, "RESET 00 02 01");
		}
		else if (WET_CLEANING_COMPOSITE_TYPE == nIndexComposite)
		{
			RUN_FUNCTION(PREVENTIVE, "RESET 00 02 01");
			RUN_FUNCTION(PREVENTIVE, "RESET 00 01 01");
		}
		else if (IDLE_CONTROL_COMPOSITE_TYPE == nIndexComposite)
		{
		}

		// added by yccoi  2006.06.14 <1.0.0.3>
		bRet = TRUE;

	} while(0);

	m_bRunPost = FALSE;
	return bRet;
}

void CModuleConditionControl_Client::Abort(int nIndexComposite)
{
	vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin() + nIndexComposite;
	(*theIter)->Abort();
}

CModuleConditionControl_Client::CModuleConditionControl_Client() : m_doubIDLETime(0) , m_bRunPost(FALSE)
{
	CModuleConditionControl_Composite* pComposite = NULL;
	CRecipeControl_Leaf* pRecipeLeaf = NULL;
	CLeakCheckControl_Leaf* pLeakCheckLeaf = NULL;
	CPreTreatmentControl_Leaf* pPreTreatmentLeaf = NULL;
	int nChannelID = 0;
	int nAlarmID = 0;

	// DRY cleaning
	pComposite = new CModuleConditionControl_Composite(TRUE);
	pComposite->Initialize(DRY_CLEANING_COMPOSITE_TYPE, PRMD_Comp_RunSt_01, g_nALARM_BASE);

	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(DRY_CLEANING_CLEANING_LEAF_TYPE, PRMD_Leaf_AuEn_01, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);

	pLeakCheckLeaf = new CLeakCheckControl_Leaf();
	pLeakCheckLeaf->Initialize(DRY_CLEANING_LEAKCHECK_LEAF_TYPE, PRMD_Leaf_AuEn_02, g_nALARM_BASE);
	pComposite->AddComponent(pLeakCheckLeaf);

	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(DRY_CLEANING_DRYPOST_LEAF_TYPE, PRMD_Leaf_AuEn_03, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	m_vecComponent.push_back(pComposite);

	// WET cleaning
	pComposite = new CModuleConditionControl_Composite(TRUE);
	pComposite->Initialize(WET_CLEANING_COMPOSITE_TYPE, PRMD_Comp_RunSt_02, g_nALARM_BASE);

	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(WET_CLEANING_LEAF_TYPE, PRMD_Leaf_AuEn_04, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	//2007.07.04 Added WET Post Leaf
	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(WET_CLEANING_POST_LEAF_TYPE, PRMD_Leaf_AuEn_07, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	m_vecComponent.push_back(pComposite);

	// IDLE control
	pComposite = new CModuleConditionControl_Composite(FALSE);
	pComposite->Initialize(IDLE_CONTROL_COMPOSITE_TYPE, PRMD_Comp_RunSt_03, g_nALARM_BASE);
	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(IDLE_CONTROL_LEAF_TYPE, PRMD_Leaf_AuEn_05, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	m_vecComponent.push_back(pComposite);

	// Pre-Treatment
	pComposite = new CModuleConditionControl_Composite(FALSE);
	pComposite->Initialize(PRE_TREATMENT_COMPOSITE_TYPE, PRMD_Comp_RunSt_04, g_nALARM_BASE);

	pPreTreatmentLeaf = new CPreTreatmentControl_Leaf();
	pPreTreatmentLeaf->Initialize(PRE_TREATMENT_LEAF_TYPE, PRMD_Leaf_AuEn_06, g_nALARM_BASE);
	pComposite->AddComponent(pPreTreatmentLeaf);
	m_vecComponent.push_back(pComposite);

	//2007.07.04 Added WET Backup component + Error Backup component
	// WET Backup
	pComposite = new CModuleConditionControl_Composite(TRUE);
	pComposite->Initialize(WET_BACKUP_COMPOSITE_TYPE, PRMD_Comp_RunSt_05, g_nALARM_BASE);
	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(WET_BACKUP_LEAF_TYPE, PRMD_Leaf_AuEn_08, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(WET_BACKUP_POST_LEAF_TYPE, PRMD_Leaf_AuEn_09, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	m_vecComponent.push_back(pComposite);

	// ERROR Backup
	pComposite = new CModuleConditionControl_Composite(TRUE);
	pComposite->Initialize(ERROR_BACKUP_COMPOSITE_TYPE, PRMD_Comp_RunSt_06, g_nALARM_BASE);
	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(ERROR_BACKUP_LEAF_TYPE, PRMD_Leaf_AuEn_10, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(ERROR_BACKUP_POST_LEAF_TYPE, PRMD_Leaf_AuEn_11, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	m_vecComponent.push_back(pComposite);

	//ERROR Backup2
	pComposite = new CModuleConditionControl_Composite(TRUE);
	pComposite->Initialize(ERROR_BACKUP2_COMPOSITE_TYPE, PRMD_Comp_RunSt_07, g_nALARM_BASE);
	pRecipeLeaf = new CRecipeControl_Leaf();
	pRecipeLeaf->Initialize(ERROR_BACKUP2_LEAF_TYPE, PRMD_Leaf_AuEn_12, g_nALARM_BASE);
	pComposite->AddComponent(pRecipeLeaf);
	m_vecComponent.push_back(pComposite);

	if ((NULL == CreateThread(NULL, 0, Thread_RunFunction, this, 0, &m_dwThreadIDOfRunFunction)) ||
		(NULL == CreateThread(NULL, 0, Thread_EventHandling, this, 0, NULL)) ||
		(NULL == CreateThread(NULL, 0, Thread_CheckTime, this, 0, &m_dwThreadIDOfCheckTime)))
	{
		MessageBox(NULL, "Thread creation failed.", "Chamber Condition Control", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
	}

}

CModuleConditionControl_Client::~CModuleConditionControl_Client()
{
	vector<CModuleConditionControl_Component*>::iterator theIter;
	for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
	{
		delete *theIter;
	}
	m_vecComponent.clear();
}

DWORD WINAPI CModuleConditionControl_Client::Thread_RunFunction(LPVOID lpParam)
{
	CModuleConditionControl_Client* pThis = (CModuleConditionControl_Client*)lpParam;

	MSG msg;
	char szOutMsg[64]     ="";
	char szAppName[64] ="Chamber Condition Control";
	int nResult;

	PeekMessage(NULL, NULL, 0, 0, PM_NOREMOVE);
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if (MSG_RUN_FUNCTION == msg.message)
		{			
			//////////////////////////////////////////////////////////////////////////			
			// modified by ycchoi 2006.06.14 < 1.0.0..3>
			//////////////////////////////////////////////////////////////////////////			
			if(pThis->Run(msg.wParam, msg.lParam))
			{
				if(DRY_CLEANING_COMPOSITE_TYPE == msg.wParam){
					if(g_bDryCleanBuzzerOn)	WRITE_DIGITAL(SigTwrBuzzerDM, 1, &nResult);	// 0:Off 1:ON
					//MessageBox(NULL, "Dry Cleanning Completed !!!", "INFORM", MB_OK|MB_SYSTEMMODAL);
					if(g_bDryCleanMsgBoxOn){
						strcpy(szOutMsg," Dry Cleaning Completed ! ");
						_gOutMsgBox(szAppName, szOutMsg, FALSE);
						printf("............... dry cleaning complted ...................\n");
					}					
				}
			}
			else
			{
				if(DRY_CLEANING_COMPOSITE_TYPE == msg.wParam){
					if(g_bDryCleanBuzzerOn)	WRITE_DIGITAL(SigTwrBuzzerDM, 1, &nResult);	// 0:Off 1:ON
					//MessageBox(NULL, "Dry Cleanning Failed !!!", "INFORM", MB_OK|MB_SYSTEMMODAL);
					if(g_bDryCleanMsgBoxOn){
						strcpy(szOutMsg," Dry Cleaning Failed ! ");
						_gOutMsgBox(szAppName, szOutMsg, FALSE);
						printf("............... dry cleaning Failed ...................\n");
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			
		}
	}

	return 0;
}

DWORD WINAPI CModuleConditionControl_Client::Thread_CheckTime(LPVOID lpParam)
{
	CModuleConditionControl_Client* pThis = (CModuleConditionControl_Client*)lpParam;

	Sleep(30*1000);
	int nSequenceRunningStatus;
//	int nFA_STATUS;
	int nDay, nHour, nMin, nSec;
	int nSeconds;
	clock_t clkBegin = 0, clkEnd = 0, clkTimeElapsed = 0;
	int nReservedRun = INVALID_TYPE;
	clkBegin = clock();
	while(1)
	{
		Sleep(1000);
		clkEnd = clock();
		clkTimeElapsed += (clkEnd - clkBegin);
		nSeconds = clkTimeElapsed / 1000;
		clkTimeElapsed = clkTimeElapsed % 1000;
		clkBegin = clkEnd;

//		nFA_STATUS = READ_DIGITAL(FA_STATUS, &g_nCommStatus);		// Enable(1)
		if (FALSE == pThis->CheckChamberEnable(TRUE))
		{
			pThis->m_doubIDLETime = 0;
		}
		else
		{
			nSequenceRunningStatus = READ_DIGITAL(PROCESS_RUN_STATUS, &g_nCommStatus);
			if (__SEQUENCE_RUNNING == nSequenceRunningStatus)
			{
				vector<CModuleConditionControl_Component*>::iterator theIter = pThis->m_vecComponent.begin();
				theIter = pThis->m_vecComponent.begin() + IDLE_CONTROL_COMPOSITE_TYPE;
				if (FALSE == (*theIter)->GetRunningStatus())
					pThis->m_doubIDLETime = 0;
				else
					pThis->m_doubIDLETime += nSeconds;
			}
			else
			{
				pThis->m_doubIDLETime += nSeconds;
			}
		}
		nDay = pThis->m_doubIDLETime / (24*60*60);
		nHour = (pThis->m_doubIDLETime-(nDay*24*60*60)) / (60*60);
		nMin = (pThis->m_doubIDLETime-(nDay*24*60*60)-(nHour*60*60)) / 60;
		nSec = (pThis->m_doubIDLETime-(nDay*24*60*60)-(nHour*60*60)-(nMin*60));
		WRITE_DIGITAL(PRMD_IDLE_TimeD, nDay, &g_nCommStatus);
		WRITE_DIGITAL(PRMD_IDLE_TimeH, nHour, &g_nCommStatus);
		WRITE_DIGITAL(PRMD_IDLE_TimeM, nMin, &g_nCommStatus);
		WRITE_DIGITAL(PRMD_IDLE_TimeS, nSec, &g_nCommStatus);

		///$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4
		// Set Pre-Treatment condition
		pThis->CheckPreTreatmentCondition(TRUE);

		if (TRUE == pThis->CheckChamberConnection(TRUE))
		{
			if ((FALSE == pThis->m_bRunPost) && (INVALID_TYPE != nReservedRun))
			{
				PostThreadMessage(pThis->m_dwThreadIDOfRunFunction, MSG_RUN_FUNCTION, nReservedRun, TRUE);
				nReservedRun = INVALID_TYPE;
				pThis->m_bRunPost = TRUE;
			}
			else if ((INVALID_TYPE == nReservedRun) && (TRUE == pThis->CheckChamberEnable(TRUE)))
			{
				if (TRUE == pThis->CheckWETCleaningCondition(TRUE))
				{
					if (FALSE == pThis->m_bRunPost)
					{
						PostThreadMessage(pThis->m_dwThreadIDOfRunFunction, MSG_RUN_FUNCTION, WET_CLEANING_COMPOSITE_TYPE, TRUE);
						pThis->m_bRunPost = TRUE;
					}
					else
					{
						nReservedRun = WET_CLEANING_COMPOSITE_TYPE;
					}
				}
				else if (g_bAutoDryClean && TRUE == pThis->CheckDRYCleaningCondition(TRUE))
				{
					if (FALSE == pThis->m_bRunPost)
					{
						PostThreadMessage(pThis->m_dwThreadIDOfRunFunction, MSG_RUN_FUNCTION, DRY_CLEANING_COMPOSITE_TYPE, TRUE);
						pThis->m_bRunPost = TRUE;
					}
					else
					{
						nReservedRun = DRY_CLEANING_COMPOSITE_TYPE;
					}
				}
				else if (TRUE == pThis->CheckIDLECondition(TRUE))
				{
					//Wuxi Idle Dry Cleaning Version No need to Samsung but change to Optional
					//2007.12.06 : Dry Cleaning Run before Idle Recipe Run When Count reached
					//if (1 == READ_DIGITAL(DRY_CONTROL_OPTION , &g_nCommStatus)) {

					// 16L Bluetain Idle Maint Dry Option Added 2012.05.xx
					if (1 == READ_DIGITAL(IDLE_MAINT_DRY_OPTION , &g_nCommStatus)) {
						if (0 != READ_DIGITAL(PMChamberStatus , &g_nCommStatus )) {
							if (FALSE == pThis->m_bRunPost)
							{
								PostThreadMessage(pThis->m_dwThreadIDOfRunFunction, MSG_RUN_FUNCTION, DRY_CLEANING_COMPOSITE_TYPE, TRUE);
								pThis->m_bRunPost = TRUE;
							}
						}
					}

					if (FALSE == pThis->m_bRunPost)
					{
						PostThreadMessage(pThis->m_dwThreadIDOfRunFunction, MSG_RUN_FUNCTION, IDLE_CONTROL_COMPOSITE_TYPE, TRUE);
						pThis->m_bRunPost = TRUE;
					}
				}
			}
			else
			{
				vector<CModuleConditionControl_Component*>::iterator theIter = pThis->m_vecComponent.begin();
				for (theIter = pThis->m_vecComponent.begin(); theIter != pThis->m_vecComponent.end(); theIter++)
				{
					// For Changing running status channel
					(*theIter)->GetRunningStatus();
				}
			}
		}
		else
		{
			vector<CModuleConditionControl_Component*>::iterator theIter = pThis->m_vecComponent.begin();
			for (theIter = pThis->m_vecComponent.begin(); theIter != pThis->m_vecComponent.end(); theIter++)
			{
				// For Changing running status channel
				(*theIter)->GetRunningStatus();
			}
		}
	}

	return 0;
}

DWORD WINAPI CModuleConditionControl_Client::Thread_EventHandling(LPVOID lpParam)
{
	CModuleConditionControl_Client* pThis = (CModuleConditionControl_Client*)lpParam;

	string strServerEvent = "EVENT_FOR_CHAMBER_CONDITION_CONTROL_SERVER_" + g_strModuleName;
	string strClientEvent = "EVENT_FOR_CHAMBER_CONDITION_CONTROL_CLIENT_" + g_strModuleName;
	HANDLE hServerEvent = ::CreateEvent(NULL, TRUE, FALSE, strServerEvent.c_str());
	HANDLE hClientEvent = ::CreateEvent(NULL, TRUE, FALSE, strClientEvent.c_str());
	ResetEvent(hServerEvent);
	ResetEvent(hClientEvent);
	while(1)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(hServerEvent, INFINITE))
		{
			printf("# WAIT_IDLE_FINISHED Starting\n");
			pThis->m_doubIDLETime = 0;
			pThis->MsgParser("WAIT_IDLE_FINISHED");
			ResetEvent(hServerEvent);
			SetEvent(hClientEvent);
			pThis->m_doubIDLETime = 0;
			printf("# WAIT_IDLE_FINISHED Stopped\n");
		}
	}
	return 0;
}

BOOL CModuleConditionControl_Client::MsgParser(string strEvent)
{
	BOOL bRet = FALSE;
	do
	{
		if ("INITIALIZE" == strEvent)
		{
			//2007.10.09 by mgsong
			//Read Initial Value from file driver & Set IO Value for update io memory.
            int    nValue;
			double dbValue;
			char   szValue[128];
			for (int i = PRMD_Comp_RunSt_01 ; i <= PRMD_Leaf_RunSt_12; i++) {
				switch (IO_STR_Map_Table[i].IO_Mode) {
				case _K_D_IO:
					nValue = 0;
					nValue = READ_DIGITAL(i , &g_nCommStatus);
					if (1 == g_nCommStatus) {
						WRITE_DIGITAL(i , nValue , &g_nCommStatus);
					} else {
						printf("Error IO Fail IO Read Operation at [%d] \n" , i);
					}
					break;
				case _K_A_IO:
					dbValue = 0.0f;
					dbValue = READ_ANALOG(i , &g_nCommStatus);
					if (1 == g_nCommStatus) {
						WRITE_ANALOG(i , dbValue , &g_nCommStatus);
					} else {
						printf("Error IO Fail IO Read Operation at [%d] \n" , i);
					}
					break;
				case _K_S_IO:
					memset(szValue , 0 , sizeof(szValue));
					READ_STRING(i , szValue , &g_nCommStatus);
					if (1 == g_nCommStatus) {
						WRITE_STRING(i , szValue , &g_nCommStatus);
					} else {
						printf("Error IO Fail IO Read Operation at [%d] \n" , i);
					}
					break;
				default: break;
				}
			}
		}
		else if ("WAIT_IDLE_FINISHED" == strEvent)
		{
			int nSequenceRunningStatus = READ_DIGITAL(PROCESS_RUN_STATUS, &g_nCommStatus);
			if (__SEQUENCE_RUNNING == nSequenceRunningStatus)
			{
				vector<CModuleConditionControl_Component*>::iterator theIter = m_vecComponent.begin();
				theIter = m_vecComponent.begin() + IDLE_CONTROL_COMPOSITE_TYPE;
				if (TRUE == (*theIter)->GetRunningStatus())
				{
					WRITE_DIGITAL(PRO_IdleRcpStsDM, 0, &g_nCommStatus);
					BOOL bInsideLoop = TRUE;
					do
					{
						if (FALSE == WAIT_SECONDS(0.1))
						{
							bInsideLoop = FALSE;
							break;
						}
					} while(__SEQUENCE_RUNNING == READ_DIGITAL(PROCESS_RUN_STATUS, &g_nCommStatus));
					if (FALSE == bInsideLoop)
						break;
				}
			}
		}
		else if ("END" == strEvent)
		{
		}
		else if ("APPLY_SAVE" == strEvent)
		{
			//2007.07.04
			//Now Not Using so Disabled Temporary
			/*vector<CModuleConditionControl_Component*>::iterator theIter;
			for (theIter = m_vecComponent.begin(); theIter != m_vecComponent.end(); theIter++)
			{
				(*theIter)->ChangeCurrentValue();
			}
			WRITE_DIGITAL(CCONDITION_SAVE, __SAVE, &g_nCommStatus);*/
		}
		else if ((strEvent.size() == 12) || (strEvent.size() == 14) || (strEvent.size() == 17) || (strEvent.size() == 18))
		{
			int nCountResult = count(strEvent.begin(), strEvent.end(), ' ');
			if (3 != nCountResult)
				break;

			BOOL bEnable = FALSE;
			int nCompositeType = INVALID_TYPE;
			int nLeafType = INVALID_TYPE;
			char szTemp[20] = "";
			sscanf(strEvent.c_str(), "%s %d %d %d", szTemp, &bEnable, &nCompositeType, &nLeafType);
			if ((INVALID_TYPE == nCompositeType) || (INVALID_TYPE == nLeafType))
				break;
			string strCommand = szTemp;
			if (("ENABLE_C" == strCommand) && ( (strEvent.size() == 17) || (strEvent.size() == 18)) )
			{
				SetAutoControl(bEnable, nCompositeType-1, nLeafType-1);
			}
			else if (("ENABLE_M" == strCommand) && (strEvent.size() == 17))
			{
				SetModuleModeToEnableAfterRun(bEnable, nCompositeType-1);
			}
			else if (("RUN" == strCommand) && (strEvent.size() == 12))
			{
				PostThreadMessage(m_dwThreadIDOfRunFunction, MSG_RUN_FUNCTION, nCompositeType-1, FALSE);
			}
			else if (("ABORT" == strCommand) && (strEvent.size() == 14))
			{
				Abort(nCompositeType-1);
			}
			//2010.02.09 Rcp Clear
			else if (("CLEAR" == strCommand) && (strEvent.size() == 14))
			{
				ClearRecipe(nCompositeType-1 , nLeafType-1);				
			}
			else break;
		}
		else
			break;

		bRet = TRUE;
	}while(0);
	if (FALSE == bRet)
	{
		string strMsg = "Invalid Event : ";
		strMsg += strEvent;
		cout << "# Chamber Condition Control => " << strMsg.c_str() << endl;
//		::MessageBox(NULL, strMsg.c_str(), "Chamber Condition Control", MB_ICONERROR|MB_SYSTEMMODAL|MB_OK);
	}

	return bRet;
}
