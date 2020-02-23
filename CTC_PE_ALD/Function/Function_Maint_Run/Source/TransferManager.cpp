// TransferManager.cpp: implementation of the CTransferManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TextParser.h"
#include "FingerAction.h"
#include "TransferOrder.h"
#include "TransferManager.h"
#include "Area.h"
#include "MsgBoxDll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTransferManager::CTransferManager()
{
	m_pTransferArea = NULL;
	m_pFingerAction = NULL;
	m_nOrderCount   = 0;
	m_nAreaCount    = 0;

	for (int i = 0 ; i < MAX_BM_SLOT ; i++)
		m_nBMMode[i] = _INOUT;

	//2007.12.06
	m_nCMSelectSlot    = 0;
	m_nDummySelectSlot = 0;

	//2010.10.10
	m_bDummyMode       = TRUE;
}

CTransferManager::~CTransferManager()
{
	//Destruction TransferArea Object
	if (NULL != m_pTransferArea) {
		delete m_pTransferArea;
		m_pTransferArea = NULL;
	}

	//Destruction FingerAction Object
	if (NULL != m_pFingerAction) {
		delete m_pFingerAction;
		m_pFingerAction = NULL;
	}

	//Destruction TransferOrder Objects
	for (int i = 0 ; i < m_nOrderCount; i++) {
		if (NULL != m_pTransferOrder[i]) delete m_pTransferOrder[i];
	}
}

//2010.10.10
void CTransferManager::SetDummyMode(BOOL bMode)
{
	m_bDummyMode = bMode;
}

BOOL CTransferManager::GetDummyMode()
{
	return m_bDummyMode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : AddTransferOrder()
//Date         : 2007.06.25
//Description  : Save Step Order
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CTransferManager::AddTransferOrder(CTransferOrder* pTransferOrder)
{
	if (m_nOrderCount >= MAX_ORDER_COUNT) return;
	if (NULL == pTransferOrder)           return;
	
	m_pTransferOrder[m_nOrderCount] = pTransferOrder;
	m_nOrderCount++;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitConfigFile()
//Date         : 2007.06.25
//Description  : Configuration file access
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::InitConfigFile(const char* szFilePath)
{
	BOOL bRet = TRUE;
	FILE *fp;
	char szRead        [256];
	char szItem        [256];

	do {	
		fp = fopen(szFilePath, "rt");
		if (fp == NULL) {
			printf("file %s open fail \n" , szFilePath);
			bRet = FALSE; break;
		}		
		if (NULL == fgets(szRead, 255, fp)) {
			printf("file %s fgets fail \n" , szFilePath);
			bRet = FALSE; break;
		}
		
		do {
			CTextParser tptemp(szRead);
			tptemp.GetString(szItem);
		
			if (0 == strcmp("$$BEGIN_TRANSFER_AREA" , szItem)) {
				//Create Transfer Area;
				m_pTransferArea = new CTransferArea();
				char szAreaName[32];
				int  nMaxSlot = 0;
				int  nCapacity= 0;
				int  nSWEnum  = 0;
				BOOL bDummy   = TRUE;
				do {
					if (NULL == fgets(szRead, 255, fp)) {bRet = FALSE;   break;}
					CTextParser tptemp2(szRead); tptemp2.GetString(szItem);
					if (0 == strcmp("$$END_TRANSFER_AREA" , szItem))     break;
					if (szRead[0] == ';' || szRead[0] == '#') {/*do nothing*/}
					else {
						CTextParser tp(szRead);
						if (FALSE == tp.GetWord(szAreaName)) {bRet = FALSE; break;}
						if (0 != strcmp("Dummy" , szAreaName)) {
							tp.GetInt(&nMaxSlot);
							tp.GetInt(&nCapacity);	
							tp.GetInt(&nSWEnum);
							if (FALSE == m_pTransferArea->AddArea(szAreaName , nMaxSlot , nCapacity , nSWEnum)) {
								printf("Error - AddArea got same Area again or Same SWEnum check config file... \n");
								bRet = FALSE; break;
							}
							#ifdef _DEBUG
								printf(">>>>>AddArea Name[%s] , MaxSlot[%d] , Capacity[%d] , SWEnum[%d] \n" , szAreaName , nMaxSlot , nCapacity , nSWEnum);
							#endif
						}
						else {
							//2010.10.10 by mgsong
							//Bluetain Single Process Enable or Disable
							tp.GetBOOL(&bDummy);
							SetDummyMode(bDummy);							
						}
					}					
				} while (feof(fp) == 0);
			} else if (0 == strcmp("$$BEGIN_BM_MODE" , szItem)) {
				char szBMMode[32];
				int  nSlotNum  = 0;
				do {
					if (NULL == fgets(szRead, 255, fp)) {bRet = FALSE;   break;}
					CTextParser tptemp2(szRead); tptemp2.GetString(szItem);
					if (0 == strcmp("$$END_BM_MODE" , szItem))     break;
					if (szRead[0] == ';' || szRead[0] == '#') {/*do nothing*/}
					else {
						CTextParser tp(szRead);
						if (FALSE == tp.GetWord(szBMMode)) {bRet = FALSE; break;}
						tp.GetInt(&nSlotNum);
						if (nSlotNum > 0 && nSlotNum <= MAX_BM_SLOT) {
							if     (0 == strcmp("IN"  , szBMMode)) m_nBMMode[nSlotNum-1] = _IN;
							else if(0 == strcmp("OUT" , szBMMode)) m_nBMMode[nSlotNum-1] = _OUT;
						}
						
						#ifdef _DEBUG
							printf(">>>>>BMMode[%s] , Slot Num[%d]\n" , szBMMode , nSlotNum);
						#endif
					}					
				} while (feof(fp) == 0);
			}else if (0 == strcmp("$$BEGIN_FINGER_DEFINE" , szItem)) {
				m_pFingerAction = new CFingerAction();
				char szFingerName[32];
				char szActionType[32];
				int  nSWEnum  = 0;
				do {
					if (NULL == fgets(szRead, 255, fp)) {bRet = FALSE;   break;}
					CTextParser tptemp2(szRead); tptemp2.GetString(szItem);
					if (0 == strcmp("$$END_FINGER_DEFINE" , szItem))     break;
					if (szRead[0] == ';' || szRead[0] == '#') {/*do nothing*/}
					else {
						CTextParser tp(szRead);
						if (FALSE == tp.GetWord(szFingerName)) {bRet = FALSE; break;}
						if (FALSE == tp.GetWord(szActionType)) {bRet = FALSE; break;}
						tp.GetInt(&nSWEnum);
						if (FALSE == m_pFingerAction->AddFingerAction(szFingerName , szActionType , nSWEnum)) {
							printf("Error - AddFingerAction got same sw Enum check config file... \n");
							bRet = FALSE; break;
						}
						#ifdef _DEBUG
							printf(">>>>>Finger Name[%s] , ActionType[%s] , SWEnum[%d] \n" , szFingerName , szActionType , nSWEnum);
						#endif
					}					
				} while (feof(fp) == 0);
			} else if (0 == strcmp("$$BEGIN_TRANSFER_ORDER" , szItem)) {
				//Create Transfer Order (1 or n object)
				CTransferOrder* pTransferOrder = new CTransferOrder();
				char szAreaName[32] = {0};
				int  nStep = 0;
				do {
					if (NULL == fgets(szRead, 255, fp)) {bRet = FALSE;    break;}
					CTextParser tptemp2(szRead); tptemp2.GetString(szItem);
					if (0 == strcmp("$$END_TRANSFER_ORDER" , szItem)) {
						if (NULL != pTransferOrder) AddTransferOrder(pTransferOrder);
						break;
					}
					if (szRead[0] == ';' || szRead[0] == '#') {/*do nothing*/}
					else {
						CTextParser tp(szRead);
						if (FALSE == tp.GetWord(szAreaName)) break;
						if (0 == strcmp("STEP" , szAreaName)) {
							tp.GetInt(&nStep);
						} else {
							if (FALSE == pTransferOrder->AddOrderStepItem(nStep , szAreaName)) {
								printf("Eror - AddOrderStepItem got same area Name check config file... \n");
								bRet = FALSE; break;
							}
							#ifdef _DEBUG
								printf(">>>>>AddStep Item Step[%d] , Name[%s]\n" , nStep , szAreaName);
							#endif
							do {
								if (FALSE == tp.GetWord(szAreaName)) break;
								else {
									if (FALSE == pTransferOrder->AddOrderStepItem(nStep , szAreaName)) {
										printf ("Transfer Config.ini Defined Same Area Name In Same Step Order \n");
										bRet = FALSE;
										break;
									}										
								}								
								#ifdef _DEBUG
									printf(">>>>>AddStep Item Step[%d] , Name[%s]\n" , nStep , szAreaName);
								#endif
							} while (1);
						}					
					}
				} while (feof(fp) == 0);

			} else if (0 == strcmp("$$BEGIN_TRANSFER_PRIORITY" , szItem)) {
				//Create Transfer Priority
			}
		
			if (FALSE == bRet) break;
			if (NULL == fgets(szRead, 255, fp)) //end of file
				break;
		} while(feof(fp) == 0);

		fclose(fp);
	} while (0);

	return bRet;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitManager()
//Date         : 2007.06.25
//Description  : Create each area object based on configuration file
//               With basic area information
//				 if you want add new area then modify configuration file & this function
//				 with proper IO Point 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::InitManager()
{
	BOOL bRet = TRUE;
	do {
		if (NULL == m_pTransferArea) {bRet = FALSE; break;}		
		if (0    == m_pTransferArea->GetAreaCount()) {bRet = FALSE; break;}
		
		sArea* psArea = NULL;
		psArea = m_pTransferArea->GetFirstArea();
		while (NULL != psArea) {			
			CArea* pArea = NULL;
			if (0 == strcmp("CM1" , psArea->szAreaName) || 0 == strcmp("CM2" , psArea->szAreaName) || 0 == strcmp("CM3" , psArea->szAreaName) || 0 == strcmp("BM3" , psArea->szAreaName))        {
				//CM Area
				if      (0 == strcmp("CM1" , psArea->szAreaName)) 
					pArea = new CCMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , CM1_C01_Wafer , GetDummyMode());
				else if (0 == strcmp("CM2" , psArea->szAreaName))
					pArea = new CCMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , CM2_C01_Wafer , GetDummyMode());
				else if (0 == strcmp("CM3" , psArea->szAreaName))
					pArea = new CCMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , CM3_C01_Wafer , GetDummyMode());
				else if (0 == strcmp("BM3" , psArea->szAreaName))
					pArea = new CCMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , BM3_Wafer_Status , GetDummyMode());
				
				if (NULL != pArea) psArea->pArea = pArea;	
			} else if (0 == strcmp("FM" , psArea->szAreaName)) {
				//FM Area
				pArea = new CATMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , FA_Wafer_Status , GetDummyMode());

				if (NULL != pArea) psArea->pArea = pArea;
			} else if (0 == strcmp("BM1" , psArea->szAreaName) || 0 == strcmp("BM2" , psArea->szAreaName)) {
				//BM Area
				if      (0 == strcmp("BM1" , psArea->szAreaName))
					pArea = new CLLArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , BM1_Wafer_Status , GetDummyMode());
				else if (0 == strcmp("BM2" , psArea->szAreaName))
					pArea = new CLLArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , BM2_Wafer_Status , GetDummyMode());				

				if (NULL != pArea) {
					for (int i = 0 ; i < MAX_BM_SLOT ; i++) pArea->SetBMMode(i , m_nBMMode[i]);
					psArea->pArea = pArea;
				}
			} else if (0 == strcmp("TM" , psArea->szAreaName) || 0 == strcmp("TM2" , psArea->szAreaName)) {
				//TM Area
				if      (0 == strcmp("TM" , psArea->szAreaName)) 
					pArea = new CVTMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , TA_Wafer_Status , GetDummyMode());
				/*else if (0 == strcmp("TM2" , psArea->szAreaName)) {
					pArea = new CVTMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , TA_Wafer_Status);*/
										
				if (NULL != pArea) psArea->pArea = pArea;
			} else if (0 == strcmp("PM1" , psArea->szAreaName) || 0 == strcmp("PM2" , psArea->szAreaName) || 0 == strcmp("PM3" , psArea->szAreaName)
				     ||0 == strcmp("PM4" , psArea->szAreaName) || 0 == strcmp("PM5" , psArea->szAreaName) || 0 == strcmp("PM6" , psArea->szAreaName))  {
				//PM Area
				if      (0 == strcmp("PM1" , psArea->szAreaName))
					pArea = new CPMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , PM1_Wafer_Status , GetDummyMode());
				else if (0 == strcmp("PM2" , psArea->szAreaName))
					pArea = new CPMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , PM2_Wafer_Status , GetDummyMode());
				else if (0 == strcmp("PM3" , psArea->szAreaName))
					pArea = new CPMArea(psArea->szAreaName , psArea->nMaxSlot , psArea->nCapacity , psArea->nSWEnum , PM3_Wafer_Status , GetDummyMode());

				if (NULL != pArea) psArea->pArea = pArea;
			} else {
				bRet = FALSE; break;
			}

			psArea = m_pTransferArea->GetNextArea();			
		}
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : FindMoveStepInfo()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::FindMoveStepInfo (char* szStartArea , char* szLastArea , int& nStartStep , int& nLastStep , CTransferOrder* pCurrentOrder)
{
	BOOL bRet = FALSE;

	for (int i = 0 ; i < MAX_ORDER_COUNT ; i++) {
		if (NULL == m_pTransferOrder[i]) continue;
		if (TRUE == m_pTransferOrder[i]->GetStepOrder(szStartArea , szLastArea , nStartStep , nLastStep)) {
			m_pCurrentOrder = m_pTransferOrder[i]; return TRUE;
		}
	}	

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetWaferSourceStation()
//Date         : 2007.06.25
//Description  : Find CM1/2/3 BM3 SW Enum by given Wafer Source
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CTransferManager::GetWaferSourceStation(int nWaferSource)
{
	printf ("GetWaferSourceStation %d \n" , nWaferSource);
	switch (nWaferSource) {
	case 0: return  0; break;
	case 1: return  1; break;
	case 2: return  2; break;
	case 3: return -1; break;
	case 4: return -1; break;
	case 5: return 20; break;
	default:return -1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoClearList()
//Date         : 2007.06.25
//Description  : Delete using TransferJob Memory after complete movement
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CTransferManager::DoClearList()
{
	for (int i = 0 ; i < MAX_MOVEJOB_COUNT; i++) {
		if (NULL != m_sMovementList[i].SendJob) {
			delete m_sMovementList[i].SendJob;
			m_sMovementList[i].SendJob = NULL;
		}
		if (NULL != m_sMovementList[i].ReceiveJob) {
			if (m_sMovementList[i].ReceiveJob == m_sMovementList[i+1].SendJob) {
				delete m_sMovementList[i].ReceiveJob;
				m_sMovementList[i].ReceiveJob = NULL;
				m_sMovementList[i+1].SendJob  = NULL;
			} else {
				delete m_sMovementList[i].ReceiveJob;
				m_sMovementList[i].ReceiveJob = NULL;
			}
		}
	}	
}

void CTransferManager::PopupMsgBox(const char* szAreaName , const char* szMsg1 , const char* szMsg2)
{
	char szMsg[256] = {0};
	sprintf(szMsg , "[%s] : %s Action is %s" , szAreaName , szMsg1 , szMsg2);
	_gOutMsgBox("Manual Transfer", szMsg, FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : LPMErrorCheck()
//Date         : 2016.03.03
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::LPMErrorCheck(int i)
{
	int nIOStatus;

	if (0 == strcmp("CM1" , m_sMovementList[i].ReceiveJob->pArea->GetAreaName()) 
		||0 == strcmp("CM1" , m_sMovementList[i].SendJob->pArea->GetAreaName()))
	{
		if(eIn != READ_DIGITAL(LPMA_ShuttleInOutXI, &nIOStatus)) return FALSE;
		if(!nIOStatus) return FALSE;
		
		if(eDown != READ_DIGITAL(LPMA_DoorUpDownXI, &nIOStatus)) return FALSE;
		if(!nIOStatus) return FALSE;
	}
	else if (0 == strcmp("CM2" , m_sMovementList[i].ReceiveJob->pArea->GetAreaName()) 
		||0 == strcmp("CM2" , m_sMovementList[i].SendJob->pArea->GetAreaName()))
	{
		if(eIn != READ_DIGITAL(LPMB_ShuttleInOutXI, &nIOStatus)) return FALSE;
		if(!nIOStatus) return FALSE;
		
		if(eDown != READ_DIGITAL(LPMB_DoorUpDownXI, &nIOStatus)) return FALSE;
		if(!nIOStatus) return FALSE;	
	}
	else if (0 == strcmp("CM3" , m_sMovementList[i].ReceiveJob->pArea->GetAreaName()) 
		||0 == strcmp("CM3" , m_sMovementList[i].SendJob->pArea->GetAreaName()))
	{
		if(eIn != READ_DIGITAL(LPMC_ShuttleInOutXI, &nIOStatus)) return FALSE;
		if(!nIOStatus) return FALSE;
		
		if(eDown != READ_DIGITAL(LPMC_DoorUpDownXI, &nIOStatus)) return FALSE;
		if(!nIOStatus) return FALSE;	
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoMovement()
//Date         : 2007.06.25
//Description  : All Constructed MovementList Execute here
//Arguments    :
//Return Value : Module_Status
//////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CTransferManager::DoMovement(int nMoveCount)
{
	Module_Status nStatus  = SYS_SUCCESS;
	int           nIOStatus;

    for (int i = 0 ; i < nMoveCount; i++) {

		//... Check LPM Error Status
		if(TRUE !=LPMErrorCheck(i))
		{
			PopupMsgBox("Line 350" , "LPM Status check" , "LPM Status unknown");
			return SYS_ABORTED;
		}

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		//Step 1 Ready Check
		//Wafer Status Validation
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (0 == strcmp("FM" , m_sMovementList[i].ReceiveJob->pArea->GetAreaName()))     {
			if (m_sMovementList[i].SendJob->nSlot1 > 0 && m_sMovementList[i].SendJob->nSlot2 > 0) 
			     m_sMovementList[i].ReceiveJob->enFingerType = ALL;
			else if (m_sMovementList[i].SendJob->nSlot1 > 0 && m_sMovementList[i].SendJob->nSlot2 <= 0)
				 m_sMovementList[i].ReceiveJob->enFingerType = SLOT1;
			else m_sMovementList[i].ReceiveJob->enFingerType = SLOT2;
		} else if (0 == strcmp("FM" , m_sMovementList[i].SendJob->pArea->GetAreaName())) {
			if (m_sMovementList[i].ReceiveJob->nSlot1 > 0 && m_sMovementList[i].ReceiveJob->nSlot2 > 0) 
			     m_sMovementList[i].SendJob->enFingerType    = ALL;
			else if (m_sMovementList[i].ReceiveJob->nSlot1 > 0 && m_sMovementList[i].ReceiveJob->nSlot2 <= 0)
				 m_sMovementList[i].SendJob->enFingerType    = SLOT1;
			else m_sMovementList[i].SendJob->enFingerType    = SLOT2;
		} else if (0 == strcmp("TM" , m_sMovementList[i].ReceiveJob->pArea->GetAreaName())) {
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only
			//           Move or Clear Case Suitable Arm will be selected autimatically 
			//           AUTO Arm Mode with Pick/Place action should be changed with arm interlock option
			if (AUTO == m_sMovementList[i].ReceiveJob->enFingerType) {
				if      (1/*ARM A Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) 
					m_sMovementList[i].ReceiveJob->enFingerType = SLOT1;
				else if (2/*ARM B Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus))
					m_sMovementList[i].ReceiveJob->enFingerType = SLOT2;
			}
		} else if (0 == strcmp("TM" , m_sMovementList[i].SendJob->pArea->GetAreaName()))    {
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only
			//           Move or Clear Case Suitable Arm will be selected autimatically 
			//           AUTO Arm Mode with Pick/Place action should be changed with arm interlock option
			if (AUTO == m_sMovementList[i].SendJob->enFingerType) {
				if      (1/*ARM A Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) 
					m_sMovementList[i].SendJob->enFingerType = SLOT1;
				else if (2/*ARM B Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus))
					m_sMovementList[i].SendJob->enFingerType = SLOT2;
			}
		}
		
		if (FALSE == m_sMovementList[i].ReceiveJob->pArea->IsReadyToReceive(m_sMovementList[i].ReceiveJob->enFingerType 
			                                                              , m_sMovementList[i].ReceiveJob->nSlot1 
																		  , m_sMovementList[i].ReceiveJob->nSlot2)) {
			PopupMsgBox("Line 409" , "Receive" , "Not Valid Condition");			
			return SYS_ABORTED;
		}
		if (FALSE == m_sMovementList[i].SendJob->pArea->   IsReadyToSend   (m_sMovementList[i].SendJob->enFingerType 
			                                                              , m_sMovementList[i].SendJob->nSlot1 
																		  , m_sMovementList[i].SendJob->nSlot2))    {
			PopupMsgBox("Line 415" , "Send" , "Not Valid Condition");
			return SYS_ABORTED;
		}

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		//Step 2 Prepare Check
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		char szTrAreaName[32] = {0};
		if      (TRANSFER_TYPE == m_sMovementList[i].SendJob->pArea->   IsTypeOfArea())
			strcpy(szTrAreaName , m_sMovementList[i].SendJob->pArea->   GetAreaName());
		else if (TRANSFER_TYPE == m_sMovementList[i].ReceiveJob->pArea->IsTypeOfArea())
			strcpy(szTrAreaName , m_sMovementList[i].ReceiveJob->pArea->GetAreaName());
		else return SYS_ABORTED;

		if (FALSE == m_sMovementList[i].ReceiveJob->pArea->PrepareForTransfer(szTrAreaName)) {
			PopupMsgBox("Line 430" , "Prepare Receive" , "Fail");
			return SYS_ABORTED;
		}
		if (FALSE == m_sMovementList[i].SendJob->pArea->   PrepareForTransfer(szTrAreaName)) {
			PopupMsgBox("Line 434" , "Prepare Send" , "Fail");
			return SYS_ABORTED;
		}

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		//Step 3 Pick/Place Details
		//Movement List has Area Slot & Arm selection so just give that data to Transfer Area
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if       (TRANSFER_TYPE == m_sMovementList[i].SendJob->pArea->IsTypeOfArea()) {//Then Put Transfer			
            if (FALSE == m_sMovementList[i].SendJob->pArea->DoPutTransfer(m_sMovementList[i].ReceiveJob->pArea->GetAreaName() 
				                                                         ,m_sMovementList[i].ReceiveJob->nSlot1
																		 ,m_sMovementList[i].ReceiveJob->nSlot2
																		 ,m_sMovementList[i].ReceiveJob->pArea->IsTypeOfArea()
																		 ,m_sMovementList[i].SendJob->enFingerType)) {
				PopupMsgBox("Line 448" , "Send" , "Not Complete Successfully Check Aligner wafer");
				return SYS_ABORTED;
			}
		} else if (TRANSFER_TYPE == m_sMovementList[i].ReceiveJob->pArea->IsTypeOfArea()) {//Then Get Transfer			
			if (FALSE == m_sMovementList[i].ReceiveJob->pArea->DoGetTransfer(m_sMovementList[i].SendJob->pArea->GetAreaName() 
				                                                         ,m_sMovementList[i].SendJob->nSlot1
																		 ,m_sMovementList[i].SendJob->nSlot2
																		 ,m_sMovementList[i].SendJob->pArea->IsTypeOfArea()
																		 ,m_sMovementList[i].ReceiveJob->enFingerType)) {
				PopupMsgBox("Line 457" , "Receive" , "Not Complete Successfully");
				return SYS_ABORTED;
			}
		} else return SYS_ABORTED;
	}
	return nStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetWafer()
//Date         : 2007.06.25
//Description  : Finger + Source Station & Slot
//Arguments    :
//Return Value : Module_Status
//////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CTransferManager::GetWafer()
{
	Module_Status nStatus  = SYS_SUCCESS;
	int nIOStatus          = 0;
	char szAreaName[32]   = {0};
	int nSelectedArm       = 0;	
	int nSourceStation     = 0;	
	FingerACTType enType   = SLOT1;
	CArea* SendArea        = NULL;
	CArea* ReceiveArea     = NULL;
	
	nSourceStation = READ_DIGITAL(TR_SRC_STATION, &nIOStatus);
	SendArea       = m_pTransferArea->GetArea(nSourceStation);
	//---------------------------------------------------------------------------------
	//1.Put/Get Wafer case Transfer must be Started from Robot Arm so Check Arm
	//---------------------------------------------------------------------------------
	nSelectedArm = READ_DIGITAL(TR_FINGER, &nIOStatus);
	//2008.02.04 Hard Cording fo temporary aligner transfer
	if (16 == nSourceStation) {
		if (FALSE == GetFromAligner(nSelectedArm)) return SYS_ABORTED;
		else return SYS_SUCCESS;
	}
	if (FALSE == m_pFingerAction->GetFingerInfo(nSelectedArm , szAreaName , enType)) {
		printf ("Error... CTransferManager::GetWafer::GetFingerInfo() fail... \n");
		PopupMsgBox("Line 489" , "Select Finger For Pick" , "Fail...");
		return SYS_ABORTED;
	}

	//---------------------------------------------------------------------------------
	//2.Find Area By Finger Name
	//---------------------------------------------------------------------------------
	ReceiveArea = m_pTransferArea->GetArea(szAreaName);
	if (NULL == ReceiveArea) {
		printf ("Error... CTransferManager::GetWafer::GetRcvArea() fail... \n");
		PopupMsgBox("Line 499", "Get Area For Pick" , "Fail");
		return SYS_ABORTED;
	} 

	//---------------------------------------------------------------------------------
	//3. Create Receive Area
	//---------------------------------------------------------------------------------
	sTransferJob* sRcv = new sTransferJob;
	sRcv->enFingerType = enType;
	sRcv->pArea        = ReceiveArea;

	
	//---------------------------------------------------------------------------------
	//4. Create Send Area
	//---------------------------------------------------------------------------------
	if (NULL == SendArea) {
		printf ("Error... CTransferManager::GetWafer::GetSndArea() fail... \n");
		PopupMsgBox("Line 518", "Get Area For Pick" , "Fail");
		delete sRcv; return SYS_ABORTED;
	}
	sTransferJob* sSnd = new sTransferJob;
	sSnd->pArea  = SendArea;
	if (PROCESS_TYPE == sSnd->pArea->IsTypeOfArea() && sSnd->pArea->GetMaxWafer() <= 2) {
		//Process module has no slot setting method so
		//this should be change with revolver version
		if (TRUE == GetDummyMode()) {
			sSnd->nSlot1 = 1;
			sSnd->nSlot2 = 2;
		}
		else {
			//2008.10.06 Only BM Slot Range IO Added
			sSnd->nSlot1 = READ_DIGITAL(TR_SRC_PMSLOT, &nIOStatus);
			sSnd->nSlot2 = READ_DIGITAL(TR_SRC_PMSLOT2, &nIOStatus);
			//2009.08.28 interlock
			if (sSnd->nSlot1 == sSnd->nSlot2) {
				PopupMsgBox("Line 593", "Slot range set is not valid" , "Fail");
	 			delete sSnd;
	 			return SYS_ABORTED;
			}
			//2009.08.28

			//2014.05.19
			if ((sSnd->nSlot1 % 2) == 0 && sSnd->nSlot1 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sRcv;
	 			return SYS_ABORTED;
			}

			if ((sSnd->nSlot2 % 2) == 1 && sSnd->nSlot2 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sRcv;
	 			return SYS_ABORTED;
			}
			//2014.05.19
		}
	} else if (BUFFER_TYPE == sSnd->pArea->IsTypeOfArea()) {
		//2008.10.06 Only BM Slot Range IO Added
		sSnd->nSlot1 = READ_DIGITAL(TR_SRC_BMSLOT, &nIOStatus);
		sSnd->nSlot2 = READ_DIGITAL(TR_SRC_BMSLOT2, &nIOStatus);
		//2009.08.28 interlock
		if (sSnd->nSlot1 == sSnd->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sSnd;
	 	    return SYS_ABORTED;
		}
		//2009.08.28

		//2014.05.19
		if ((sSnd->nSlot1 % 2) == 0 && sSnd->nSlot1 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sRcv;
	 		return SYS_ABORTED;
		}

		if ((sSnd->nSlot2 % 2) == 1 && sSnd->nSlot2 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sRcv;
	 		return SYS_ABORTED;
		}
		//2014.05.19
	} else {
		sSnd->nSlot1 = READ_DIGITAL(TR_SRC_SLOT, &nIOStatus);
		sSnd->nSlot2 = READ_DIGITAL(TR_SRC_SLOT2, &nIOStatus);
		//2009.08.28 interlock
		if (sSnd->nSlot1 == sSnd->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sSnd;
	 	    return SYS_ABORTED;
		}
		//2009.08.28
	}

	m_sMovementList[0].SendJob    = sSnd;
	m_sMovementList[0].ReceiveJob = sRcv;    
	
	//---------------------------------------------------------------------------------
	//5.Execute
	//---------------------------------------------------------------------------------
	nStatus = DoMovement();
	DoClearList();
	
	return nStatus;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PutWafer()
//Date         : 2007.06.25
//Description  : Finger + Destination Sation & Slot
//Arguments    :
//Return Value : Module_Status
//////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CTransferManager::PutWafer()
{
	Module_Status nStatus  = SYS_SUCCESS;
	int nIOStatus          = 0;
	char szAreaName[32]    = {0};
	int nSelectedArm       = 0;	
	int nTargetStation     = 0;	
	FingerACTType enType   = SLOT1;
	CArea* SendArea        = NULL;
	CArea* ReceiveArea     = NULL;

	nTargetStation = READ_DIGITAL(TR_TRG_STATION, &nIOStatus);
	ReceiveArea = m_pTransferArea->GetArea(nTargetStation);
	
	//---------------------------------------------------------------------------------
	//1.Put/Get Wafer case Transfer must be Started from Robot Arm so Check Arm
	//---------------------------------------------------------------------------------
	nSelectedArm = READ_DIGITAL(TR_FINGER, &nIOStatus);
	//2008.02.04 Hard Cording fo temporary aligner transfer
	if (16 == nTargetStation) {
		if (FALSE == PutToAligner(nSelectedArm)) return SYS_ABORTED;
		else return SYS_SUCCESS;
	}
	//2008.02.04 End
	if (FALSE == m_pFingerAction->GetFingerInfo(nSelectedArm , szAreaName , enType)) {
		printf ("Error... CTransferManager::GetWafer::GetFingerInfo() fail... \n");
		PopupMsgBox("Line 568" , "Finger Select" , "Fail");
		return SYS_ABORTED;
	}

	//---------------------------------------------------------------------------------
	//2.Find Area By Finger Name
	//---------------------------------------------------------------------------------
	SendArea = m_pTransferArea->GetArea(szAreaName);
	if (NULL == SendArea) {
		printf ("Error... CTransferManager::PutWafer::GetSndArea() fail... \n");
		PopupMsgBox("Line 578", "Get Area For Place" , "Fail");
		return SYS_ABORTED;
	} 

	//---------------------------------------------------------------------------------
	//3. Create Send Area
	//---------------------------------------------------------------------------------
	sTransferJob* sSnd = new sTransferJob;
	sSnd->enFingerType = enType;
	sSnd->pArea        = SendArea;
	
	//---------------------------------------------------------------------------------
	//4. Create Receive Area
	//---------------------------------------------------------------------------------
	if (NULL == ReceiveArea) {
		printf ("Error... CTransferManager::PutWafer::GetRcvArea() fail... \n");
		delete sSnd;
		PopupMsgBox("Line 597", "Get Area For Place" , "Fail");
		return SYS_ABORTED;
	}
	sTransferJob* sRcv = new sTransferJob;
	sRcv->pArea = ReceiveArea;
	if (PROCESS_TYPE == sRcv->pArea->IsTypeOfArea() && sRcv->pArea->GetMaxWafer() <= 2) {
		//Process module has no slot setting method so
		// this should be change with revolver version
		if (TRUE == GetDummyMode()) {
			sRcv->nSlot1 = 1;
			sRcv->nSlot2 = 2;
		}
		else
		{
			//2010.10.10
			sRcv->nSlot1 = READ_DIGITAL(TR_TRG_PMSLOT, &nIOStatus);
			sRcv->nSlot2 = READ_DIGITAL(TR_TRG_PMSLOT2, &nIOStatus);
			//2009.08.28 interlock
			if (sRcv->nSlot1 == sRcv->nSlot2) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sRcv;
	 			return SYS_ABORTED;
			}
			
			//2014.05.19
			if ((sRcv->nSlot1 % 2) == 0 && sRcv->nSlot1 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sRcv;
	 			return SYS_ABORTED;
			}

			if ((sRcv->nSlot2 % 2) == 1 && sRcv->nSlot2 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sRcv;
	 			return SYS_ABORTED;
			}
			//2014.05.19
		}
	} else if (BUFFER_TYPE == sRcv->pArea->IsTypeOfArea()) {
		//2008.10.06 Only BM Slot Range IO Added
		sRcv->nSlot1 = READ_DIGITAL(TR_TRG_BMSLOT, &nIOStatus);
		sRcv->nSlot2 = READ_DIGITAL(TR_TRG_BMSLOT2, &nIOStatus);
		//2009.08.28 interlock
		if (sRcv->nSlot1 == sRcv->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sRcv;
	 	    return SYS_ABORTED;
		}
		//2009.08.28

		//2014.05.19
		if ((sRcv->nSlot1 % 2) == 0 && sRcv->nSlot1 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sRcv;
	 		return SYS_ABORTED;
		}

		if ((sRcv->nSlot2 % 2) == 1 && sRcv->nSlot2 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sRcv;
	 		return SYS_ABORTED;
		}
		//2014.05.19
	} else {
		sRcv->nSlot1 = READ_DIGITAL(TR_TRG_SLOT, &nIOStatus);
		sRcv->nSlot2 = READ_DIGITAL(TR_TRG_SLOT2, &nIOStatus);
		//2009.08.28 interlock
		if (sRcv->nSlot1 == sRcv->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sRcv;
	 	    return SYS_ABORTED;
		}
		//2009.08.28
	}
	m_sMovementList[0].SendJob    = sSnd;
	m_sMovementList[0].ReceiveJob = sRcv;    
	
	//---------------------------------------------------------------------------------
	//5.Execute
	//---------------------------------------------------------------------------------
	nStatus = DoMovement();
	DoClearList();
	
	return nStatus;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : MoveWafer()
//Date         : 2007.06.25
//Description  : Source Staton & Slot + Destionation station & Slot
//Arguments    :
//Return Value : Module_Status
//////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CTransferManager::MoveWafer()
{
	Module_Status nStatus  = SYS_SUCCESS;
	int nIOStatus          = 0;
	int nSelectedArm       = 0;	
	int nSourceStation     = 0;
	int nTargetStation     = 0;	
	CArea* SendArea        = NULL;
	CArea* NextArea        = NULL;
	CArea* LastArea        = NULL;
	
	//---------------------------------------------------------------------------------
	//1. Check Start Area
	//---------------------------------------------------------------------------------
	nSourceStation = READ_DIGITAL(TR_SRC_STATION, &nIOStatus);
	SendArea = m_pTransferArea->GetArea(nSourceStation);
	if (NULL == SendArea) {
		printf ("Error... CTransferManager::MoveWafer::GetSndArea() fail... \n");
		PopupMsgBox("Line 648", "Get Area For Move" , "Fail");
		return SYS_ABORTED;
	}
	sTransferJob* sSnd = new sTransferJob;
	sSnd->pArea  = SendArea;
	if (PROCESS_TYPE == sSnd->pArea->IsTypeOfArea() && sSnd->pArea->GetMaxWafer() <= 2) {
		// Process module has no slot setting method so
		// this should be change with revolver version
		if (TRUE == GetDummyMode()) {
			sSnd->nSlot1 = 1;
			sSnd->nSlot2 = 2;
		}
		else {
			sSnd->nSlot1 = READ_DIGITAL(TR_SRC_PMSLOT, &nIOStatus);
			sSnd->nSlot2 = READ_DIGITAL(TR_SRC_PMSLOT2, &nIOStatus);

			if (sSnd->nSlot1 == sSnd->nSlot2) {
				PopupMsgBox("Line 795", "Slot range set is not valid" , "Fail");
	 			delete sSnd;
	 			return SYS_ABORTED;
			}

			//2014.05.19
			if ((sSnd->nSlot1 % 2) == 0 && sSnd->nSlot1 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sSnd;
	 			return SYS_ABORTED;
			}

			if ((sSnd->nSlot2 % 2) == 1 && sSnd->nSlot2 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sSnd;
	 			return SYS_ABORTED;
			}
			//2014.05.19
		}
	} else if (BUFFER_TYPE == sSnd->pArea->IsTypeOfArea()) {
		//2008.10.06 Only BM Slot Range IO Added
		sSnd->nSlot1 = READ_DIGITAL(TR_SRC_BMSLOT, &nIOStatus);
		sSnd->nSlot2 = READ_DIGITAL(TR_SRC_BMSLOT2, &nIOStatus);

		//2009.08.28 interlock
		if (sSnd->nSlot1 == sSnd->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sSnd;
	 	    return SYS_ABORTED;
		}
		//2009.08.28

		//2014.05.19
		if ((sSnd->nSlot1 % 2) == 0 && sSnd->nSlot1 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sSnd;
	 		return SYS_ABORTED;
		}

		if ((sSnd->nSlot2 % 2) == 1 && sSnd->nSlot2 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sSnd;
	 		return SYS_ABORTED;
		}
		//2014.05.19
	} else {
		sSnd->nSlot1 = READ_DIGITAL(TR_SRC_SLOT, &nIOStatus);
		sSnd->nSlot2 = READ_DIGITAL(TR_SRC_SLOT2, &nIOStatus);
		//2009.08.28 interlock
		if (sSnd->nSlot1 == sSnd->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sSnd;
	 	    return SYS_ABORTED;
		}
		//2009.08.28
	}

	//---------------------------------------------------------------------------------
	//2. Check Last Area
	//---------------------------------------------------------------------------------
	nTargetStation = READ_DIGITAL(TR_TRG_STATION, &nIOStatus);
	LastArea = m_pTransferArea->GetArea(nTargetStation);
	if (NULL == LastArea) {
		printf ("Error... CTransferManager::MoveWafer::GetLastArea() fail... \n");
		PopupMsgBox("Line 671", "Get Area For Move" , "Fail");
		delete sSnd;
		return SYS_ABORTED;
	}    
	sTransferJob* sLast = new sTransferJob;	
	sLast->pArea  = LastArea;
	if (PROCESS_TYPE == sLast->pArea->IsTypeOfArea() && sLast->pArea->GetMaxWafer() <= 2) {
		//Process module has no slot setting method so
		// this should be change with revolver version
		if (TRUE == GetDummyMode()) {
			sLast->nSlot1 = 1;
			sLast->nSlot2 = 2;
		}
		else {
			sLast->nSlot1 = READ_DIGITAL(TR_TRG_PMSLOT, &nIOStatus);
			sLast->nSlot2 = READ_DIGITAL(TR_TRG_PMSLOT2, &nIOStatus);

			if (sLast->nSlot1 == sLast->nSlot2) {
				PopupMsgBox("Line 849", "Slot range set is not valid" , "Fail");
	 			delete sLast;
	 			return SYS_ABORTED;
			}

			//2014.05.19
			if ((sLast->nSlot1 % 2) == 0 && sLast->nSlot1 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sLast;
	 			return SYS_ABORTED;
			}

			if ((sLast->nSlot2 % 2) == 1 && sLast->nSlot2 > 0) {
				PopupMsgBox("Line 698", "PM Slot range set is not valid" , "Fail");
	 			delete sLast;
	 			return SYS_ABORTED;
			}
			//2014.05.19
		}
	} else if (BUFFER_TYPE == sLast->pArea->IsTypeOfArea()) {
		//2008.10.06 Only BM Slot Range IO Added
		sLast->nSlot1 = READ_DIGITAL(TR_TRG_BMSLOT, &nIOStatus);
		sLast->nSlot2 = READ_DIGITAL(TR_TRG_BMSLOT2, &nIOStatus);
		//2009.08.28 interlock
		if (sLast->nSlot1 == sLast->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sLast;
	 	    return SYS_ABORTED;
		}
		//2009.08.28

		//2014.05.19
		if ((sLast->nSlot1 % 2) == 0 && sLast->nSlot1 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sLast;
	 		return SYS_ABORTED;
		}

		if ((sLast->nSlot2 % 2) == 1 && sLast->nSlot2 > 0) {
			PopupMsgBox("Line 698", "BM Slot range set is not valid" , "Fail");
	 		delete sLast;
	 		return SYS_ABORTED;
		}
		//2014.05.19
	} else {
		sLast->nSlot1 = READ_DIGITAL(TR_TRG_SLOT, &nIOStatus);
		sLast->nSlot2 = READ_DIGITAL(TR_TRG_SLOT2, &nIOStatus);
		//2009.08.28 interlock
		if (sLast->nSlot1 == sLast->nSlot2) {
		    PopupMsgBox("Line 671", "Slot range set is not valid" , "Fail");
	 	    delete sLast;
	 	    return SYS_ABORTED;
		}
		//2009.08.28
	}

	//---------------------------------------------------------------------------------
	//3. Find Next Move Area by Transfer Order utility object
	//---------------------------------------------------------------------------------
	int nStartStep   = 0;
	int nLastStep    = 0;
	int nCurrentStep = 0;;
	int nMoveCount   = 0;
	CTransferOrder* pCurrentOrder = NULL;
	sOrderStep      sNextOrder;
	sTransferJob*   sNext  = NULL;
	if (FALSE == FindMoveStepInfo(SendArea->GetAreaName() , LastArea->GetAreaName() , nStartStep , nLastStep , pCurrentOrder)) {
		printf ("Error... CTransferManager::MoveWafer::FindMoveStepInfo() fail... \n");
		PopupMsgBox("Line 699", "Get Next Move Step" , "Fail");
		delete sSnd; delete sLast; return SYS_ABORTED;
	}

	//mgsong temp 2007.12.06
	/*if (nStartStep == nLastStep) {
		delete sSnd; delete sLast; 
		PopupMsgBox("Line 704", "Move Same Step Level" , "Try Pick Arm & Place");
		return SYS_ABORTED;
	}*/

	nCurrentStep = nStartStep;
	m_sMovementList[nMoveCount].SendJob    = sSnd;
	while (1) {
		//IN Direction
		if     (nStartStep < nLastStep) {nCurrentStep++;}
		//OUT Direction
		else if(nStartStep > nLastStep) {nCurrentStep--;}

		if (nCurrentStep == nLastStep) {
			m_sMovementList[nMoveCount].ReceiveJob = sLast; break;
		}

		memset(&sNextOrder , 0 , sizeof(sOrderStep));
		m_pCurrentOrder->GetNextStepOrder(nCurrentStep , sNextOrder);
		for (int i = 0 ; i < sNextOrder.nStepAreaCount; i++) {
			char szTrAreaName[32] = {0};
			NextArea = NULL;
			NextArea = m_pTransferArea->GetArea(sNextOrder.sOrderStepArea[i].szAreaName);
			if      (TRANSFER_TYPE == NextArea->IsTypeOfArea()) strcpy(szTrAreaName , NextArea->GetAreaName());
			else if (TRANSFER_TYPE == SendArea->IsTypeOfArea()) strcpy(szTrAreaName , SendArea->GetAreaName());
			else    {delete sLast; DoClearList(); return SYS_ABORTED;}
            
			FingerACTType enFingerType;
			int           nSlot1 , nSlot2;
			if (TRUE == NextArea->GetValidReceive(szTrAreaName,enFingerType , nSlot1 , nSlot2)) {
				sNext = new sTransferJob;
				if (TRUE == GetDummyMode()) {
					sNext->nSlot1       = nSlot1;
					sNext->nSlot2       = nSlot2;
				}
				else {
					if (0 == sSnd->nSlot2) {
						sNext->nSlot1   = nSlot1;
						sNext->nSlot2   = 0;
					}
					else {
						sNext->nSlot1   = nSlot1;
						sNext->nSlot2   = nSlot2;
					}
				}
				sNext->pArea        = NextArea;
				sNext->enFingerType = enFingerType;
				m_sMovementList[nMoveCount].ReceiveJob = sNext;

				nMoveCount++;

				m_sMovementList[nMoveCount].SendJob    = sNext;
				SendArea = NextArea;
				break;
			} else NextArea = NULL;				
		}
		if (NULL == NextArea) {delete sLast; DoClearList(); PopupMsgBox("Line 742", "Get Area For Pick" , "Fail"); return SYS_ABORTED;}
	}	
	
	//---------------------------------------------------------------------------------
	//4. Execute
	//---------------------------------------------------------------------------------
	nStatus = DoMovement(nMoveCount+1);
	DoClearList();	
	
	return nStatus;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : ClearWafer()
//Date         : 2007.06.25
//Description  : Just scan area list & execute each step movement function
//Arguments    :
//Return Value : Module_Status
//////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CTransferManager::ClearWafer()
{
	Module_Status nStatus = SYS_SUCCESS;
	int nIOStatus = 0;
	if(!WAIT_SECONDS(4)) return SYS_ABORTED; //...2015.04.17
	sArea* psArea = m_pTransferArea->GetFirstClear();
	while (NULL != psArea) {
		if (0 == strcmp("CM1" , psArea->szAreaName) || 0 == strcmp("CM2" , psArea->szAreaName) || 0 == strcmp("CM3" , psArea->szAreaName) || 0 == strcmp("BM3" , psArea->szAreaName)) {
			psArea = m_pTransferArea->GetNextClear(); continue;
		} else if (0 == strcmp("FM" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromFM(psArea , bEnd)) return SYS_ABORTED;}
		} else if (0 == strcmp("BM1", psArea->szAreaName) || 0 == strcmp("BM2" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromBM(psArea , bEnd)) return SYS_ABORTED;}
		} else if (0 == strcmp("TM" , psArea->szAreaName) || 0 == strcmp("TM2" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromTM(psArea , bEnd)) return SYS_ABORTED;}
		} else if (0 == strcmp("PM1", psArea->szAreaName) || 0 == strcmp("PM2" , psArea->szAreaName) || 0 == strcmp("PM3" , psArea->szAreaName)
		     	|| 0 == strcmp("PM4", psArea->szAreaName) || 0 == strcmp("PM5" , psArea->szAreaName) || 0 == strcmp("PM6" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromPM(psArea , bEnd)) return SYS_ABORTED;}
		}

		psArea = m_pTransferArea->GetNextClear();
	}

	//Try one more time
	//may be after pm-->bm transfer some case still remain wafter in bm slot so try again
	psArea = m_pTransferArea->GetFirstClear();
	while (NULL != psArea) {
		if (0 == strcmp("CM1" , psArea->szAreaName) || 0 == strcmp("CM2" , psArea->szAreaName) || 0 == strcmp("CM3" , psArea->szAreaName) || 0 == strcmp("BM3" , psArea->szAreaName)) {
			psArea = m_pTransferArea->GetNextClear(); continue;
		} else if (0 == strcmp("FM" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromFM(psArea , bEnd)) return SYS_ABORTED;}
		} else if (0 == strcmp("BM1", psArea->szAreaName) || 0 == strcmp("BM2" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromBM(psArea , bEnd)) return SYS_ABORTED;}
		} else if (0 == strcmp("TM" , psArea->szAreaName) || 0 == strcmp("TM2" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromTM(psArea , bEnd)) return SYS_ABORTED;}
		} else if (0 == strcmp("PM1", psArea->szAreaName) || 0 == strcmp("PM2" , psArea->szAreaName) || 0 == strcmp("PM3" , psArea->szAreaName)
		     	|| 0 == strcmp("PM4", psArea->szAreaName) || 0 == strcmp("PM5" , psArea->szAreaName) || 0 == strcmp("PM6" , psArea->szAreaName)) {
			BOOL bEnd = FALSE;
			while (FALSE == bEnd) {if (FALSE == DoClearFromPM(psArea , bEnd)) return SYS_ABORTED;}
		}

		psArea = m_pTransferArea->GetNextClear();
	}

	return nStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : FixedAlignerClear()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::FixedAlignerClear()
{
	BOOL bRet = TRUE;
	char szEvent[256] = {0};
	int  nIOStatus;

	//Option Aligner Wafer Check here
	int nAlignerWafer , nAlignerSrc;
	int nFinger;
	nAlignerWafer = READ_DIGITAL(FM_AL_Wafer_Status , &nIOStatus);
	nAlignerSrc   = READ_DIGITAL(FM_AL_Wafer_Source , &nIOStatus);
	do {
		if   (0 >= nAlignerWafer) break;
		if   (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 , "FM" , nFinger)) return FALSE;
		else {
			bRet = FALSE;
				sprintf(szEvent , "MAINT_INTERFACE PICKAL_FALIC|1|AL|%d|0|0" , nAlignerWafer );

			WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
			WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
			while (TRUE) {
				if (!WAIT_SECONDS(1)) return FALSE;
				switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
				case SYS_ABORTED : break;
				case SYS_ERROR   : break;
				case SYS_SUCCESS : bRet = TRUE; break;
				}
				if (TRUE == bRet) break;
			}
			WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
			WRITE_DIGITAL(TR_TRG_STATION , GetWaferSourceStation(nAlignerSrc)  , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_SLOT    , nAlignerWafer , &nIOStatus );
			WRITE_DIGITAL(TR_TRG_SLOT2   , 0, &nIOStatus );
		
			if (SYS_ABORTED == PutWafer())  {
				PopupMsgBox("Line 858", "Put Wafer to CM" , "Fail");
				return FALSE;
			}
		}
	} while (0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoClearFromFM()
//Date         : 2007.06.25
//Description  : Movement ATM Arm To CM , Aligner Wafer Clear is fixed
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::DoClearFromFM(sArea* psArea , BOOL& bEnd , BOOL bSkipAligner)
{
	BOOL bRet = TRUE;
	int  nIOStatus;	
	
	if (NULL == psArea) return FALSE;

	//Just Put Wafer to CM
	FingerACTType enFingerType = SLOT1;
	int nDesStation1 = 0;
	int nDesStation2 = 0;
	int nDesSlot1    = 0;
	int nDesSlot2    = 0;
	int nFinger      = 0;	
	BOOL bStepMode   = FALSE;
	bEnd             = FALSE;

	if (FALSE == psArea->pArea->GetCurWaferInfo(enFingerType , nDesStation1 , nDesStation2 , nDesSlot1 , nDesSlot2 , bStepMode)) {
		//There is no clear wafer in fm arm	
		bEnd = TRUE;
	} else {
		if (FALSE == m_pFingerAction->GetFingerEnum(enFingerType ,psArea->szAreaName , nFinger)) return FALSE;
		if (TRUE  == bStepMode) {
			//Arm A First
			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 , psArea->szAreaName , nFinger))   return FALSE;
			WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
			WRITE_DIGITAL(TR_TRG_STATION , GetWaferSourceStation(nDesStation1)  , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_SLOT    , nDesSlot1 , &nIOStatus );
			WRITE_DIGITAL(TR_TRG_SLOT2   , 0 , &nIOStatus );

			if (SYS_ABORTED == PutWafer())  {
				PopupMsgBox("Line 904", "Put Wafer to CM From FM Arm A" , "Fail");
				return FALSE;
			}

			//Arm B Later
			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , psArea->szAreaName , nFinger))   return FALSE;
			WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
			WRITE_DIGITAL(TR_TRG_STATION , GetWaferSourceStation(nDesStation2)  , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_SLOT    , 0         , &nIOStatus );
			WRITE_DIGITAL(TR_TRG_SLOT2   , nDesSlot2 , &nIOStatus );

			if (SYS_ABORTED == PutWafer()) {
				PopupMsgBox("Line 916", "Put Wafer to CM From FM Arm B" , "Fail");
				return FALSE;
			}
		} else {
			if (SLOT1 == enFingerType) {
				if (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 , psArea->szAreaName , nFinger))   return FALSE;
				WRITE_DIGITAL(TR_FINGER , nFinger, &nIOStatus);
				WRITE_DIGITAL(TR_TRG_STATION , GetWaferSourceStation(nDesStation1)  , &nIOStatus);				
			} else if (SLOT2 == enFingerType) {
				//2007.07.30 bug fix
				//           Just 1 Arm has wafer to transfer so correct Arm should be checked before set target station
				if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , psArea->szAreaName , nFinger))   return FALSE;
				WRITE_DIGITAL(TR_FINGER , nFinger, &nIOStatus);
				WRITE_DIGITAL(TR_TRG_STATION , GetWaferSourceStation(nDesStation2)  , &nIOStatus);		
			} else if (ALL == enFingerType) {
				if (FALSE == m_pFingerAction->GetFingerEnum(ALL , psArea->szAreaName , nFinger))   return FALSE;
				WRITE_DIGITAL(TR_FINGER , nFinger, &nIOStatus);
				WRITE_DIGITAL(TR_TRG_STATION , GetWaferSourceStation(nDesStation1)  , &nIOStatus);		
			} else return FALSE;

			WRITE_DIGITAL(TR_TRG_SLOT    , nDesSlot1 , &nIOStatus );
			WRITE_DIGITAL(TR_TRG_SLOT2   , nDesSlot2 , &nIOStatus );
			
			if (SYS_ABORTED == PutWafer())  {
				PopupMsgBox("Line 926", "Put Wafer to CM From FM Arm AB" , "Fail");
				return FALSE;
			}
		}
	}

	if (FALSE == bSkipAligner) {bRet = FixedAlignerClear();}
	return bRet;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoClearFromBM()
//Date         : 2007.06.25
//Description  : BM--> ATM Arm --> Execute ClearFromFM Function
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::DoClearFromBM(sArea* psArea , BOOL& bEnd)
{
	BOOL bRet = TRUE;
	BOOL bTemp= FALSE;
	int  nIOStatus;
	
	if (NULL == psArea) return FALSE;

	FingerACTType enFingerType = SLOT1;
	int  nDesStation1 = 0;
	int  nDesStation2 = 0;
	int  nDesSlot1    = 0;
	int  nDesSlot2    = 0;
	int  nFinger      = 0;	
	BOOL bStepMode    = FALSE;

	if (FALSE == psArea->pArea->GetCurWaferInfo(enFingerType , nDesStation1 , nDesStation2 , nDesSlot1 , nDesSlot2 , bStepMode)) {
		//There is no clear wafer in fm arm	
		bEnd = TRUE;
	} else {
		//1.Pick Wafer to FM Arm
		if (nDesSlot1 > 0 && nDesSlot2 > 0) {
			if (FALSE == m_pFingerAction->GetFingerEnum(ALL   ,"FM" , nFinger)) return FALSE;
		} else if (nDesSlot1 > 0 && nDesSlot2 <= 0) {
			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 ,"FM" , nFinger)) return FALSE;
		} else if (nDesSlot1 <= 0 && nDesSlot2 > 0) {
			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 ,"FM" , nFinger)) return FALSE;
		} else {
			PopupMsgBox("Line 972", "Get FM Finger for Pick From BM" , "Fail");
			return FALSE;
		}

		WRITE_DIGITAL(TR_FINGER      , nFinger          , &nIOStatus);
		WRITE_DIGITAL(TR_SRC_STATION , psArea->nSWEnum  , &nIOStatus);
		/*
		WRITE_DIGITAL(TR_SRC_SLOT    , nDesSlot1        , &nIOStatus);
		WRITE_DIGITAL(TR_SRC_SLOT2   , nDesSlot2        , &nIOStatus);
		*/
		//2008.10.01 Give Slot Range Limitation each station
		WRITE_DIGITAL(TR_SRC_BMSLOT    , nDesSlot1      , &nIOStatus);
		WRITE_DIGITAL(TR_SRC_BMSLOT2   , nDesSlot2      , &nIOStatus);
		if (SYS_ABORTED == GetWafer()) return FALSE;	
		
		//2.Place to CM
		sArea* psNextArea = NULL;
		psNextArea = m_pTransferArea->GetSArea("FM");
		if (NULL == psNextArea) return FALSE;
		if (FALSE == DoClearFromFM(psNextArea , bTemp , TRUE)) return FALSE;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoClearFromTM()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::DoClearFromTM(sArea* psArea , BOOL& bEnd)
{
	BOOL bRet = FALSE;
	int  nIOStatus;
		
	if (NULL == psArea) return FALSE;

	//Just Put Wafer to Next Step Area
	CTransferOrder* pCurrentOrder = NULL;
	sOrderStep      sNextOrder;
	CArea*          pNextArea     = NULL;
	FingerACTType   enFingerType  = SLOT1;

	int             nStartStep    = 0;
	int             nLastStep     = 0;
	int             nCurrentStep  = 0;
	int             nDesStation1  = 0;
	int             nDesStation2  = 0;
	int             nDesSlot1     = 0;
	int             nDesSlot2     = 0;
	int             nSlot1        = 0;
	int             nSlot2        = 0;
	int             nFinger       = 0; 
	
	BOOL            bStepMode     = FALSE;

	if (FALSE == psArea->pArea->GetCurWaferInfo(enFingerType , nDesStation1 , nDesStation2 , nDesSlot1 , nDesSlot2 , bStepMode)) {
		//There is no clear wafer in TM arm	
		bEnd = TRUE; bRet = TRUE;
	} else {
		//1. Find Next Step & Check Transfer Possibility
		sArea* psLast = m_pTransferArea->GetSArea(GetWaferSourceStation(nDesStation1));
		if (NULL  == psLast) return FALSE;
		if (FALSE == FindMoveStepInfo(psArea->szAreaName , psLast->szAreaName , nStartStep , nLastStep , pCurrentOrder)) {
			PopupMsgBox("Line 1033", "Get Move Step List" , "Fail");
			return FALSE;
		}

		nCurrentStep = nStartStep;
		//IN Direction
		if     (nStartStep < nLastStep) {nCurrentStep++;}
		//OUT Direction
		else if(nStartStep > nLastStep) {nCurrentStep--;}

		//2. Find Next Area Slot
		memset(&sNextOrder , 0 , sizeof(sOrderStep));
		m_pCurrentOrder->GetNextStepOrder(nCurrentStep , sNextOrder);
		for (int i = 0 ; i < sNextOrder.nStepAreaCount; i++) {
			char szTrAreaName[32] = {0};
			pNextArea = m_pTransferArea->GetArea(sNextOrder.sOrderStepArea[i].szAreaName);
			strcpy(szTrAreaName , psArea->pArea->GetAreaName()); //current tm name            
			FingerACTType tmpType = SLOT1;
			if (TRUE == pNextArea->GetValidReceive(szTrAreaName , tmpType , nSlot1 , nSlot2)) {
				//3. Execute Transfer TM --> Next Out Step
				//There Exist Valid Receive so TM Wafer possible to Move
				//TM Arm has no Step Mode just execute
				if (nSlot1 <= 0 && nSlot2 <= 0) return FALSE;
				if (FALSE == m_pFingerAction->GetFingerEnum(enFingerType ,psArea->szAreaName , nFinger)) return FALSE;				
				break;
			} else pNextArea = NULL;
		}
		
		if (NULL == pNextArea) {
			for (i = 0 ; i < sNextOrder.nStepAreaCount; i++) {
				if (0 == strcmp("BM1" , sNextOrder.sOrderStepArea[i].szAreaName) || 0 == strcmp("BM2" , sNextOrder.sOrderStepArea[i].szAreaName)) {
					pNextArea = m_pTransferArea->GetArea(sNextOrder.sOrderStepArea[i].szAreaName);
					bEnd = FALSE;
					while (bEnd == FALSE) {if (FALSE == DoClearFromBM(m_pTransferArea->GetSArea(pNextArea->GetAreaName()) , bEnd)) return FALSE;}					
				} /*else if (...) {
					// other area code describe here if you need
				}*/
			}
			bEnd = FALSE; return TRUE;			
		} else {
			WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
			WRITE_DIGITAL(TR_TRG_STATION , pNextArea->GetSWEnum() , &nIOStatus);
			/*
			WRITE_DIGITAL(TR_TRG_SLOT    , nSlot1 , &nIOStatus );
			WRITE_DIGITAL(TR_TRG_SLOT2   , nSlot2 , &nIOStatus );
			*/
			//2008.10.01 Give Slot Range Limitation each station

			if (0 == nDesSlot2 && FALSE == GetDummyMode()) {
				WRITE_DIGITAL(TR_TRG_BMSLOT    , nSlot1 , &nIOStatus );
				WRITE_DIGITAL(TR_TRG_BMSLOT2   , 0      , &nIOStatus );
			}
			else {
				WRITE_DIGITAL(TR_TRG_BMSLOT    , nSlot1 , &nIOStatus );
				WRITE_DIGITAL(TR_TRG_BMSLOT2   , nSlot2 , &nIOStatus );
			}
			if (SYS_ABORTED == PutWafer()) return FALSE;
			else bRet = TRUE;
		}		
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoClearFromPM()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::DoClearFromPM(sArea* psArea , BOOL& bEnd)
{
	BOOL bRet = FALSE;
	int  nIOStatus;
		
	if (NULL == psArea) return FALSE;

	//Just Put Wafer to Next Step Area
	CTransferOrder* pCurrentOrder    = NULL;
	sOrderStep      sNextOrder;
	CArea*          pNextArea        = NULL;
	CArea*          pNextNextArea    = NULL;
	FingerACTType   enFingerType     = SLOT1;

	char            szTrAreaName[32] = {0};
	int             nStartStep       = 0;
	int             nLastStep        = 0;
	int             nCurrentStep     = 0;
	int             nDesStation1     = 0;
	int             nDesStation2     = 0;
	int             nDesSlot1        = 0;
	int             nDesSlot2        = 0;
	int             nFinger          = 0; 
	int             nSlot1           = 0;
	int             nSlot2           = 0;	
	BOOL            bStepMode        = FALSE;

	//---------------------------------------------------------------------------------
	//1.Check Valid Send Wafer Exist
	//---------------------------------------------------------------------------------
	if (FALSE == psArea->pArea->GetCurWaferInfo(enFingerType , nDesStation1 , nDesStation2 , nDesSlot1 , nDesSlot2 , bStepMode)) {
		//There is no clear wafer in PM#	
		bEnd = TRUE; bRet = TRUE;
	} else {
		//---------------------------------------------------------------------------------
		//2. Find Next Step & Check Transfer Possibility
		//---------------------------------------------------------------------------------
		sArea* psLast = m_pTransferArea->GetSArea(GetWaferSourceStation(nDesStation1));
		if (NULL  == psLast) return FALSE;
		if (FALSE == FindMoveStepInfo(psArea->szAreaName , psLast->szAreaName , nStartStep , nLastStep , pCurrentOrder)) {
			PopupMsgBox("Line 1131", "Get Move Step List" , "Fail");
			return FALSE;
		}
		nCurrentStep = nStartStep;
		if     (nStartStep < nLastStep) {nCurrentStep++;}
		else if(nStartStep > nLastStep) {nCurrentStep--;}

		//---------------------------------------------------------------------------------
		//3. Find Next Area Slot (may be tm , tm2)
		//---------------------------------------------------------------------------------
		memset(&sNextOrder , 0 , sizeof(sOrderStep));
		m_pCurrentOrder->GetNextStepOrder(nCurrentStep , sNextOrder);
		for (int i = 0 ; i < sNextOrder.nStepAreaCount; i++) {
			if (0 == strcmp("TM" , sNextOrder.sOrderStepArea[i].szAreaName) || 0 == strcmp("TM2" , sNextOrder.sOrderStepArea[i].szAreaName)) {
				pNextArea = m_pTransferArea->GetArea(sNextOrder.sOrderStepArea[i].szAreaName);
				strcpy(szTrAreaName , pNextArea->GetAreaName()); //current tm name   				
				if (TRUE == pNextArea->GetValidReceive(szTrAreaName , enFingerType , nSlot1 , nSlot2)) {
					if (FALSE == m_pFingerAction->GetFingerEnum(enFingerType ,pNextArea->GetAreaName() , nFinger)) return FALSE;	
					break;
			   } else pNextArea = NULL;
			} /*else if (...) {
				// other area code describe here if you need
			}*/
		}
		
		if (NULL == pNextArea) {
			//Transfer Area should be clare before called this func
			return FALSE;
		}

		//---------------------------------------------------------------------------------
		//4.Check Next Next area (may be bm)
		//---------------------------------------------------------------------------------
		//Now you Get Next Next area and execute something if that area is BM
		if     (nStartStep < nLastStep) {nCurrentStep++;}
		else if(nStartStep > nLastStep) {nCurrentStep--;}
		memset(&sNextOrder , 0 , sizeof(sOrderStep));
		m_pCurrentOrder->GetNextStepOrder(nCurrentStep , sNextOrder);
		for (i = 0 ; i < sNextOrder.nStepAreaCount; i++) {
			if (0 == strcmp("BM1" , sNextOrder.sOrderStepArea[i].szAreaName) || 0 == strcmp("BM2" , sNextOrder.sOrderStepArea[i].szAreaName)) {
				FingerACTType tmpType = SLOT1;
				pNextNextArea = m_pTransferArea->GetArea(sNextOrder.sOrderStepArea[i].szAreaName);				
				if (TRUE == pNextNextArea->GetValidReceive(szTrAreaName , tmpType , nSlot1 , nSlot2)) {
					break;
				} else pNextNextArea = NULL;			
			} /*else if (...) {
				// other area code describe here if you need
			}*/
		}

        //---------------------------------------------------------------------------------
		//5.Just BM Slot Full Not Abort Status may be so... Try again after BM Clear
		//---------------------------------------------------------------------------------
		if (NULL == pNextNextArea) {
			for (i = 0 ; i < sNextOrder.nStepAreaCount; i++) {
				if (0 == strcmp("BM1" , sNextOrder.sOrderStepArea[i].szAreaName) || 0 == strcmp("BM2" , sNextOrder.sOrderStepArea[i].szAreaName)) {
					pNextNextArea = m_pTransferArea->GetArea(sNextOrder.sOrderStepArea[i].szAreaName);
					bEnd = FALSE;
					while (bEnd == FALSE) {if (FALSE == DoClearFromBM(m_pTransferArea->GetSArea(pNextNextArea->GetAreaName()) , bEnd)) return FALSE;}					
				} /*else if (...) {
					// other area code describe here if you need
				}*/
			}

			bEnd = FALSE; return TRUE;
			//return to parent so called again this function after bm clear
		} else {
			//---------------------------------------------------------------------------------
			//6.Transfer & This function will called one more time
			//---------------------------------------------------------------------------------
			WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
			WRITE_DIGITAL(TR_TRG_STATION , pNextNextArea->GetSWEnum() , &nIOStatus);
			/*
			WRITE_DIGITAL(TR_TRG_SLOT    , nSlot1 , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_SLOT2   , nSlot2 , &nIOStatus);
			*/
			//2008.10.06 Only BM Slot Range IO Added
			if (0 == nDesSlot2 && FALSE == GetDummyMode()) {
				WRITE_DIGITAL(TR_TRG_BMSLOT    , nSlot1 , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_BMSLOT2   , 0      , &nIOStatus);
			}
			else {
				WRITE_DIGITAL(TR_TRG_BMSLOT    , nSlot1 , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_BMSLOT2   , nSlot2 , &nIOStatus);
			}

			WRITE_DIGITAL(TR_SRC_STATION , psArea->nSWEnum , &nIOStatus);
			WRITE_DIGITAL(TR_SRC_PMSLOT  , nDesSlot1 , &nIOStatus);
			WRITE_DIGITAL(TR_SRC_PMSLOT2 , nDesSlot2 , &nIOStatus);
		
			if (SYS_ABORTED == MoveWafer()) return FALSE;
		}
		
		bRet = TRUE;
	}
	return bRet;
}

//^^^^^^^^^
//2007.12.26
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetFromAligner()
//Date         : 2008.02.04
//Description  : Added for wafer pick / place to aligner
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::GetFromAligner(int nSelectedArm)
{
	BOOL bRet = FALSE;
	CArea* ReceiveArea   = NULL;
	char   szFinger[128] = {0};
	char   szEvent[256]  = {0};
	int    nIOStatus     = 0;
	int    nWaferStatus  = 0;
	int    nWaferStatus2 = 0;

	FingerACTType enType   = SLOT1;
	do {
		if (FALSE == m_pFingerAction->GetFingerInfo(nSelectedArm  , szFinger , enType)) {
			PopupMsgBox("Line 1298", "Finer Info is not valid" , "Fail");
			break;
		}

		if (0 != strcmp(szFinger , "FM")) {
			PopupMsgBox("Line 1303", "Select ATM Arm Please" , "Fail");
			break;
		}

		nWaferStatus = READ_DIGITAL(FM_AL_Wafer_Status, &nIOStatus);
		if (SLOT1 == enType) {
		    if (nWaferStatus <= 0)  {
				PopupMsgBox("Line 1307", "Aligner has no Wafer" , "Fail");
				break;
		    }
		} else if (SLOT2 == enType) {
		    if (nWaferStatus <= 0)  {
				PopupMsgBox("Line 1307", "Aligner has no Wafer" , "Fail");
				break;
		    }
		} else {
		    PopupMsgBox("Line 1307", "Not supported arm select" , "Fail");
			break;
		}


		ReceiveArea = m_pTransferArea->GetArea("FM");
		if (NULL == ReceiveArea) break;

		if (FALSE == ReceiveArea->IsReadyToReceive(enType , 0 , 0)) {
			PopupMsgBox("Line 1317", "ATM Arm not ready to get wafer" , "Fail");
			break;
		}

		if (SLOT1 == enType) {
			sprintf(szEvent , "MAINT_INTERFACE PICKAL_FALIC|1|AL|%d|0|0" , nWaferStatus);				
		} else if (SLOT2 == enType) {
			sprintf(szEvent , "MAINT_INTERFACE PICKAL_FALIC|1|AL|0|%d|0" , nWaferStatus);
		} else {PopupMsgBox("Line 1325", "Select ATM Arm is not valid" , "Fail");break;}

		WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
		WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
		while (TRUE) {
			if (!WAIT_SECONDS(1)) return SYS_ABORTED;
			switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
			case SYS_ABORTED : break;
			case SYS_ERROR   : break;
			case SYS_SUCCESS : bRet = TRUE; break;
			}
			if (TRUE == bRet) break;
		}

		bRet = TRUE;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PutToAligner()
//Date         : 2008.02.04
//Description  : Added for wafer pick / place to aligner
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::PutToAligner  (int nSelectedArm)
{
	BOOL bRet = FALSE;
	CArea* SendArea   = NULL;
	char   szFinger[128] = {0};
	char   szEvent[256]  = {0};
	int    nIOStatus     = 0;
	int    nWaferStatus  = 0;
	int    nWaferStatus2 = 0;

	FingerACTType enType   = SLOT1;
	do {
		if (FALSE == m_pFingerAction->GetFingerInfo(nSelectedArm , szFinger , enType)) {
			PopupMsgBox("Line 1357", "Finer Info is not valid" , "Fail");
			break;
		}

		if (0 != strcmp(szFinger , "FM")) {
			PopupMsgBox("Line 1362", "Select ATM Arm Please" , "Fail");
			break;
		}

		nWaferStatus = READ_DIGITAL(FM_AL_Wafer_Status, &nIOStatus);
		if (nWaferStatus > 0)  {
			PopupMsgBox("Line 1368", "Aligner has Wafer aleady" , "Fail");
			break;
		}

		SendArea = m_pTransferArea->GetArea("FM");
		if (NULL == SendArea) {
			PopupMsgBox("Line 1374", "GetArea Fail" , "Fail");
			break;
		}

		if (SLOT1 == enType) {
			nWaferStatus = READ_DIGITAL(FA_Wafer_Status, &nIOStatus);
		} else if (SLOT2 == enType) {
			nWaferStatus = READ_DIGITAL(FB_Wafer_Status, &nIOStatus);
		} else {
			PopupMsgBox("Line 1307", "Not supported arm select" , "Fail");
			break;
		}

		if (FALSE == SendArea->IsReadyToSend(enType , 0 , 0)) {
			PopupMsgBox("Line 1385", "ATM Arm not ready to send wafer" , "Fail");
			break;
		}

		if (SLOT1 == enType) {
			sprintf(szEvent , "MAINT_INTERFACE PLACE_FALIC|1|AL|%d|0|0" , nWaferStatus );				
		} else if (SLOT2 == enType) {
			sprintf(szEvent , "MAINT_INTERFACE PLACE_FALIC|1|AL|0|%d|0" , nWaferStatus);
		} else {PopupMsgBox("Line 1470", "Select ATM Arm is not valid" , "Fail");break;}

		WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
		WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
		while (TRUE) {
			if (!WAIT_SECONDS(1)) return SYS_ABORTED;
			switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
			case SYS_ABORTED : break;
			case SYS_ERROR   : break;
			case SYS_SUCCESS : bRet = TRUE; break;
			}
			if (TRUE == bRet) break;
		}

		bRet = TRUE;
	} while (0);

	return bRet;
}
//2008.02.04 End
Module_Status CTransferManager::FixedAlign(int nSlotA , int nSlotB)
{
	Module_Status nStatus  = SYS_SUCCESS;
	BOOL bRet         = FALSE;
	int  nIOStatus    = 0;
	char szEvent[256] = {0};
	do {
		if (0 != nSlotA) {
			sprintf(szEvent , "MAINT_INTERFACE PLACE_FALIC|1|AL|%d|0|0" , nSlotA );
			WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
			WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
			while (TRUE) {
				if (!WAIT_SECONDS(1)) return SYS_ABORTED;
				switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
				case SYS_ABORTED : break;
				case SYS_ERROR   : break;
				case SYS_SUCCESS : bRet = TRUE; break;
				}
				if (TRUE == bRet) break;
			}

			bRet = FALSE;
			sprintf(szEvent , "MAINT_INTERFACE PICKAL_FALIC|1|AL|%d|0|0" , nSlotA );
			WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
			WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
			while (TRUE) {
				if (!WAIT_SECONDS(1)) return SYS_ABORTED;
				switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
				case SYS_ABORTED : break;
				case SYS_ERROR   : break;
				case SYS_SUCCESS : bRet = TRUE; break;
				}
				if (TRUE == bRet) break;
			}
		}

		bRet = FALSE;
		if (0 != nSlotB) {
			sprintf(szEvent , "MAINT_INTERFACE PLACE_FALIC|1|AL|0|%d|0" ,nSlotB);
			WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
			WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
			while (TRUE) {
				if (!WAIT_SECONDS(1)) return SYS_ABORTED;
				switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
				case SYS_ABORTED : break;
				case SYS_ERROR   : break;
				case SYS_SUCCESS : bRet = TRUE; break;
				}
				if (TRUE == bRet) break;
			}

			bRet = FALSE;
			sprintf(szEvent , "MAINT_INTERFACE PICKAL_FALIC|1|AL|0|%d|0" , nSlotB);
			WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
			WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
			while (TRUE) {
				if (!WAIT_SECONDS(1)) return SYS_ABORTED;
				switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
				case SYS_ABORTED : break;
				case SYS_ERROR   : break;
				case SYS_SUCCESS : bRet = TRUE; break;
				}
				if (TRUE == bRet) break;
			}
		}

		nStatus = SYS_SUCCESS;
	} while (0);

	return nStatus;
}
//2007.12.06
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DummyAutoExchange()
//Date         : 2007.12.06
//Description  : Select Cassette $ BM3 Dummy Stacker Slot 1:1 Exchange
//               Execute CM Pick --> BM3 Pick --> BM3 Place (From CM) --> CM Place (From BM3)
//Arguments    :
//Return Value : Module_Status
//////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CTransferManager::DummyAutoExchange()
{
	Module_Status nStatus  = SYS_SUCCESS;
	int nIOStatus          = 0;
	int nFinger            = 0;	
	int nCurDummySlot      = 1;
	int nSourceStation     = 0;
	int nDummyAlignOpt     = 0;
	CArea* SrcCMArea       = NULL;
	CArea* DummyArea       = NULL;
	CArea* TrArea          = NULL;
	char   szDummySet[128] = "";
	
	//---------------------------------------------------------------------------------
	//1. Check Start Area
	//---------------------------------------------------------------------------------
	//2009.02.25
	nDummyAlignOpt = READ_DIGITAL(FC_FM_DUMMY_ALIGN, &nIOStatus);
	//Get Cassette Module		
	nSourceStation = READ_DIGITAL(Dummy_Sel_CM, &nIOStatus);
	WRITE_DIGITAL(TR_SRC_STATION , nSourceStation , &nIOStatus);
	SrcCMArea = m_pTransferArea->GetArea(nSourceStation);

	//Get Dummy Module fixed bm3
	DummyArea = m_pTransferArea->GetArea("BM3");

	//Get TrArea Module fixed FM
	TrArea     = m_pTransferArea->GetArea("FM");

	if (NULL == SrcCMArea) {
		printf ("Error... CTransferManager::DummyAutoExchange::GetCMArea() fail... \n");
		PopupMsgBox("Line 1263", "Get Area For DummyAutoExchange" , "Fail");
		return SYS_ABORTED;
	} else if (0 != strcmp(SrcCMArea->GetAreaName() , "CM1") && 0 != strcmp(SrcCMArea->GetAreaName() , "CM2") && 0 != strcmp(SrcCMArea->GetAreaName() , "CM3")) {
		printf ("Error... CTransferManager::DummyAutoExchange() fail... \n");
		PopupMsgBox("Line 1266", "Dummy Exchange Source must be CM1/2/3 select again" , "Fail");
		return SYS_ABORTED;
	}

	if (NULL == DummyArea) {
		printf ("Error... CTransferManager::DummyAutoExchange::GetDummyArea() fail... \n");
		PopupMsgBox("Line 1277", "Get Area For DummyAutoExchange" , "Fail");
		return SYS_ABORTED;
	}

	if (NULL == TrArea) {
		printf ("Error... CTransferManager::DummyAutoExchange::GetDummyArea() fail... \n");
		PopupMsgBox("Line 1287", "Get Area For DummyAutoExchange" , "Fail");
		return SYS_ABORTED;
	}

	//Start Slot 1 ~ Slot 25
	for (int i = 1; i <= SrcCMArea->GetMaxWafer(); i++) {
		//Check Selected Slot has valid wafer
		if (nCurDummySlot > DummyArea->GetMaxWafer()) break;
		if (SrcCMArea->IsReadyToSend(SLOT1 , i , 0)) {
			if (FALSE == TrArea->IsReadyToReceive(SLOT1 , 0 , 0)) {
				printf ("Error... CTransferManager::DummyAutoExchange::IsReadyToReceive() fail... \n");
				PopupMsgBox("Line 1297", "ATM Arm Aleady Wafer befor Get from cm" , "Fail");
				return SYS_ABORTED;
			}
			//======================================
			//Step1 Get from CM with ATM Arm A first
			//======================================
			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 , "FM" , nFinger))   return SYS_ABORTED;
			WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
			WRITE_DIGITAL(TR_SRC_STATION , SrcCMArea->GetSWEnum() , &nIOStatus);
			WRITE_DIGITAL(TR_SRC_SLOT    , i                      , &nIOStatus);
			WRITE_DIGITAL(TR_SRC_SLOT2   , 0                      , &nIOStatus);
			if (SYS_ABORTED == GetWafer()) return SYS_ABORTED;	
			//^^^^^^ Optional
			//2007.12.26
			if (1 == nDummyAlignOpt)
				if (SYS_ABORTED == FixedAlign(i , 0)) return SYS_ABORTED;
			////////////////////////////////////////////////////////

			//=======================================
			//Step 2 Check BM3 Slot Wafer
			//=======================================			
			if (DummyArea->IsReadyToSend(SLOT1 , 0 , nCurDummySlot)) {
				if (FALSE == TrArea->IsReadyToReceive(SLOT2 , 0 , 0)) {
					printf ("Error... CTransferManager::DummyAutoExchange::IsReadyToReceive() fail... \n");
					PopupMsgBox("Line 1315", "ATM Arm Aleady Wafer befor Get from dummy" , "Fail");
					return SYS_ABORTED;
				}
				//Get Wafer first if there exist send wafer
				if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , "FM" , nFinger))   return SYS_ABORTED;
				WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
				WRITE_DIGITAL(TR_SRC_STATION , DummyArea->GetSWEnum() , &nIOStatus);
				WRITE_DIGITAL(TR_SRC_SLOT    , 0                      , &nIOStatus);
				WRITE_DIGITAL(TR_SRC_SLOT2   , nCurDummySlot          , &nIOStatus);
				if (SYS_ABORTED == GetWafer()) return SYS_ABORTED;	

			//Dummy Reset [][][][]
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			memset (szDummySet , 0                , sizeof(szDummySet));
			sprintf(szDummySet , "RESET_COUNT %d" , nCurDummySlot);
			RUN_FUNCTION(DUMMY_MANAGEMENT         , szDummySet);
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			} 

			//========================================
			//Step 3 Put New Wafer
			//========================================
			if (FALSE == TrArea->IsReadyToSend(SLOT1 , 0 , 0)) {
				printf ("Error... CTransferManager::DummyAutoExchange::IsReadyToSend() fail... \n");
				PopupMsgBox("Line 1337", "ATM Arm A no wafer before place to dummy" , "Fail");
				return SYS_ABORTED;

			}
			
			if (FALSE == DummyArea->IsReadyToReceive(SLOT1 , nCurDummySlot , 0)) {
				printf ("Error... CTransferManager::DummyAutoExchange::IsReadyToSend() fail... \n");
				PopupMsgBox("Line 1344", "Dummy slot aleady has wafer before place new wafer" , "Fail");
				return SYS_ABORTED;
			}

			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 , "FM" , nFinger))   return SYS_ABORTED;
			WRITE_DIGITAL(TR_FINGER      , nFinger                 , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_STATION , DummyArea->GetSWEnum()  , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_SLOT    , nCurDummySlot           , &nIOStatus );
			WRITE_DIGITAL(TR_TRG_SLOT2   , 0                       , &nIOStatus );
			if (SYS_ABORTED == PutWafer()) return SYS_ABORTED;	

			//Setting New Dummy Slot Information about new wafer
			WRITE_DIGITAL(BM3_Wafer_Status + (nCurDummySlot-1) , nCurDummySlot , &nIOStatus);
			WRITE_DIGITAL(BM3_Wafer_Source + (nCurDummySlot-1) , 5/*dummy*/    , &nIOStatus);

			//Step 4 Current Dummy Slot Increase
			nCurDummySlot++;
			
			//Step 5 Old wafer return to CM if exist in atm arm
			if (TRUE == TrArea->IsReadyToSend(SLOT2 , 0 , 0)) {				
				if (FALSE == SrcCMArea->IsReadyToReceive(SLOT1 , 0 , i)) {
					printf ("Error... CTransferManager::DummyAutoExchange::IsReadyToReceive() fail... \n");
					PopupMsgBox("Line 1361", "cm slot aleady has wafer before place new wafer" , "Fail");
					return SYS_ABORTED;
				}
				//^^^^^^ Optional
				//2009.02.25
				if (1 == nDummyAlignOpt)
					if (SYS_ABORTED == FixedAlign(0 , i)) return SYS_ABORTED;
				////////////////////////////////////////////////////////
                if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , "FM" , nFinger))   return SYS_ABORTED;
				WRITE_DIGITAL(TR_FINGER      , nFinger                 , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_STATION , SrcCMArea->GetSWEnum()  , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_SLOT    , 0                       , &nIOStatus );
				WRITE_DIGITAL(TR_TRG_SLOT2   , i                       , &nIOStatus );
				if (SYS_ABORTED == PutWafer()) return SYS_ABORTED;	

				//2008.10.14 Make Exchanged CM Wafer State to Processed
				ChangeWaferState(nSourceStation , i);
			}			
		} else continue;
	}	
	
	return nStatus;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : CheckSelectExchangeSlot()
//Date         : 2007.12.06
//Description  : 
//               
//Arguments    :
//Return Value : 
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CTransferManager::CheckSelectExchangeSlot()
{
	int nIOStatus;

	//reset member
	m_nCMSelectSlot   = 0;
	m_nDummySelectSlot = 0;

	//initialize data
	for (int i = 0 ; i < 25; i++) {
		if (0 == READ_DIGITAL(CM_Select_Slot1 + i , &nIOStatus))
			m_bCMSelect[i]  = FALSE;
		else m_bCMSelect[i] = TRUE;

		if (0 == READ_DIGITAL(Dummy_Select_Slot1 + i , &nIOStatus))
			m_bDummySelect[i]  = FALSE;
		else m_bDummySelect[i] = TRUE;		
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetFirstCMSelectSlot()
//Date         : 2007.12.06
//Description  : 
//               
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CTransferManager::GetFirstCMSelectSlot()
{
	int nSlotCount = 0;
	for (int i = 0 ; i < 25; i++) {
		if (TRUE == m_bCMSelect[i]) {
			nSlotCount = i+1;
			break;
		}
	}

	if (0 != nSlotCount) {
		m_nCMSelectSlot = nSlotCount;
	}

	return nSlotCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetNextCMSelectSlot()
//Date         : 2007.12.06
//Description  : 
//               
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CTransferManager::GetNextCMSelectSlot()
{
	int nSlotCount = 0;
	for (int i = m_nCMSelectSlot ; i < 25; i++) {
		if (TRUE == m_bCMSelect[i]) {
			nSlotCount = i+1;
			break;
		}
	}

	if (0 != nSlotCount) {
		m_nCMSelectSlot = nSlotCount;
	}

	return nSlotCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetFirstDummySelectSlot()
//Date         : 2007.12.06
//Description  : 
//               
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CTransferManager::GetFirstDummySelectSlot()
{
	int nSlotCount = 0;
	for (int i = 0 ; i < 25; i++) {
		if (TRUE == m_bDummySelect[i]) {
			nSlotCount = i+1;
			break;
		}
	}

	if (0 != nSlotCount) {
		m_nDummySelectSlot = nSlotCount;
	}

	return nSlotCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetNextDummySelectSlot()
//Date         : 2007.12.06
//Description  : 
//               
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CTransferManager::GetNextDummySelectSlot()
{
	int nSlotCount = 0;
	for (int i = m_nDummySelectSlot ; i < 25; i++) {
		if (TRUE == m_bDummySelect[i]) {
			nSlotCount = i+1;
			break;
		}
	}

	if (0 != nSlotCount) {
		m_nDummySelectSlot = nSlotCount;
	}

	return nSlotCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DummyCMSelectSlotReset()   DummySelectSlotReset()
//Date         : 2008.11.21
//Description  : 
//               
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CTransferManager::DummyCMSelectSlotReset()
{
	int nIOStatus  = 0;
	for(int i = 0 ; i < 25; i++)
	{
		WRITE_DIGITAL(CM_Select_Slot1 + i, 0, &nIOStatus);
	}
	return SYS_SUCCESS;
}

int CTransferManager::DummySelectSlotReset()
{
	int nIOStatus  = 0;
	for(int i = 0 ; i < 25; i++)
	{
		WRITE_DIGITAL(Dummy_Select_Slot1 + i, 0, &nIOStatus);
	}
	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DummySelectExchange()
//Date         : 2007.12.06
//Description  : Select Cassette $ BM3 Dummy Stacker Slot 1:1 Exchange
//               Execute CM Pick --> BM3 Pick --> BM3 Place (From CM) --> CM Place (From BM3)
//Arguments    :
//Return Value : Module_Status
//////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CTransferManager::DummySelectExchange()
{
	Module_Status nStatus  = SYS_SUCCESS;
	int nIOStatus          = 0;
	int nFinger            = 0;	
	int nSelectSlot        = 0;
	int nDummySlot         = 0;
	int nSourceStation     = 0;
	int nDummyAlignOpt     = 0;
	CArea* SrcCMArea       = NULL;
	CArea* DummyArea       = NULL;
	CArea* TrArea          = NULL;
	char   szDummySet[128] = "";
	
	
	CheckSelectExchangeSlot();
	//---------------------------------------------------------------------------------
	//1. Check Start Area
	//---------------------------------------------------------------------------------
	//2009.02.25
	nDummyAlignOpt = READ_DIGITAL(FC_FM_DUMMY_ALIGN, &nIOStatus);

	//Get Cassette Module
	nSourceStation = READ_DIGITAL(Dummy_Sel_CM, &nIOStatus);
	WRITE_DIGITAL(TR_SRC_STATION , nSourceStation , &nIOStatus);
	SrcCMArea = m_pTransferArea->GetArea(nSourceStation);

	//Get Dummy Module fixed bm3
	DummyArea = m_pTransferArea->GetArea("BM3");

	//Get TrArea Module fixed FM
	TrArea     = m_pTransferArea->GetArea("FM");

	if (NULL == SrcCMArea) {
		printf ("Error... CTransferManager::DummySelectExchange::GetCMArea() fail... \n");
		PopupMsgBox("Line 1508", "Get Area For DummySelectExchange" , "Fail");
		return SYS_ABORTED;
	} else if (0 != strcmp(SrcCMArea->GetAreaName() , "CM1") && 0 != strcmp(SrcCMArea->GetAreaName() , "CM2") && 0 != strcmp(SrcCMArea->GetAreaName() , "CM3")) {
		printf ("Error... CTransferManager::DummySelectExchange() fail... \n");
		PopupMsgBox("Line 1512", "DummySelectExchange Source must be CM1/2/3 select again" , "Fail");
		return SYS_ABORTED;
	}

	if (NULL == DummyArea) {
		printf ("Error... CTransferManager::DummySelectExchange::GetDummyArea() fail... \n");
		PopupMsgBox("Line 1518", "Get Area For DummySelectExchange" , "Fail");
		return SYS_ABORTED;
	}

	if (NULL == TrArea) {
		printf ("Error... CTransferManager::DummySelectExchange::GetDummyArea() fail... \n");
		PopupMsgBox("Line 1524", "Get Area For DummySelectExchange" , "Fail");
		return SYS_ABORTED;
	}

	//Start Slot 1 ~ Slot 25
	nSelectSlot = GetFirstCMSelectSlot();
	if (0 == nSelectSlot) {
		printf ("Error... CTransferManager::DummySelectExchange::GetDummyArea() fail... \n");
		PopupMsgBox("Line 1532", "No CM Slot Selected for exchange" , "Fail");
		return SYS_ABORTED;
	}
	nDummySlot = GetFirstDummySelectSlot();
	if (0 == nDummySlot || nDummySlot > DummyArea->GetMaxWafer()) {
		printf ("Error... CTransferManager::DummySelectExchange::GetDummyArea() fail... \n");
		PopupMsgBox("Line 1538", "No Dummy Slot selected for exchange" , "Fail");
		return SYS_ABORTED;
	}
	while (0 != nSelectSlot) {
		if (0 == nSelectSlot || 0 == nDummySlot || nDummySlot > DummyArea->GetMaxWafer()) return SYS_SUCCESS; //no more exchange wafer

		//CM Wafer Exist case
		if (SrcCMArea->IsReadyToSend(SLOT1 , nSelectSlot , 0)) {
			//get from cm
			if (FALSE == TrArea->IsReadyToReceive(SLOT1 , 0 , 0)) {
				printf ("Error... CTransferManager::DummySelectExchange::IsReadyToReceive() fail... \n");
				PopupMsgBox("Line 1549", "ATM Arm Aleady Wafer befor Get from cm" , "Fail");
				return SYS_ABORTED;
			}
			
			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 , "FM" , nFinger))   return SYS_ABORTED;
			WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
			WRITE_DIGITAL(TR_SRC_STATION , SrcCMArea->GetSWEnum() , &nIOStatus);
			WRITE_DIGITAL(TR_SRC_SLOT    , nSelectSlot            , &nIOStatus);
			WRITE_DIGITAL(TR_SRC_SLOT2   , 0                      , &nIOStatus);
			if (SYS_ABORTED == GetWafer()) return SYS_ABORTED;	

			//^^^^^^ Optional
			//2009.02.25
			if (1 == nDummyAlignOpt)
				if (SYS_ABORTED == FixedAlign(nSelectSlot , 0)) return SYS_ABORTED;

			////////////////////////////////////////////////////////
			//get from dummy
			if (DummyArea->IsReadyToSend(SLOT1 , 0 , nDummySlot)) {
				if (FALSE == TrArea->IsReadyToReceive(SLOT2 , 0 , 0)) {
					printf ("Error... CTransferManager::DummySelectExchange::IsReadyToReceive() fail... \n");
					PopupMsgBox("Line 1565", "ATM Arm Aleady Wafer befor Get from dummy" , "Fail");
					return SYS_ABORTED;
				}
				//Get Wafer first if there exist send wafer
				if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , "FM" , nFinger))   return SYS_ABORTED;
				WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
				WRITE_DIGITAL(TR_SRC_STATION , DummyArea->GetSWEnum() , &nIOStatus);
				WRITE_DIGITAL(TR_SRC_SLOT    , 0                      , &nIOStatus);
				WRITE_DIGITAL(TR_SRC_SLOT2   , nDummySlot             , &nIOStatus);
				if (SYS_ABORTED == GetWafer()) return SYS_ABORTED;	

			//Reset Dummy Count [][][][]
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			memset (szDummySet , 0                , sizeof(szDummySet));
			sprintf(szDummySet , "RESET_COUNT %d" , nDummySlot);
			RUN_FUNCTION(DUMMY_MANAGEMENT         , szDummySet);
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			} 			

			//put to dummy
			if (FALSE == TrArea->IsReadyToSend(SLOT1 , 0 , 0)) {
				printf ("Error... CTransferManager::DummySelectExchange::IsReadyToSend() fail... \n");
				PopupMsgBox("Line 1582", "ATM Arm A no wafer before place to dummy" , "Fail");
				return SYS_ABORTED;

			}
			if (FALSE == DummyArea->IsReadyToReceive(SLOT1 , nDummySlot , 0)) {
				printf ("Error... CTransferManager::DummySelectExchange::IsReadyToSend() fail... \n");
				PopupMsgBox("Line 1588", "Dummy slot aleady has wafer before place new wafer" , "Fail");
				return SYS_ABORTED;
			}

			if (FALSE == m_pFingerAction->GetFingerEnum(SLOT1 , "FM" , nFinger))   return SYS_ABORTED;
			WRITE_DIGITAL(TR_FINGER      , nFinger                 , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_STATION , DummyArea->GetSWEnum()  , &nIOStatus);
			WRITE_DIGITAL(TR_TRG_SLOT    , nDummySlot              , &nIOStatus );
			WRITE_DIGITAL(TR_TRG_SLOT2   , 0                       , &nIOStatus );
			if (SYS_ABORTED == PutWafer()) return SYS_ABORTED;	
			
			//Setting New Dummy Slot Information about new wafer
			WRITE_DIGITAL(BM3_Wafer_Status + (nDummySlot-1) , nDummySlot , &nIOStatus);
			WRITE_DIGITAL(BM3_Wafer_Source + (nDummySlot-1) , 5/*dummy*/    , &nIOStatus);

			//put to cm
			if (TRUE == TrArea->IsReadyToSend(SLOT2 , 0 , 0)) {				
				if (FALSE == SrcCMArea->IsReadyToReceive(SLOT1 , 0 , nSelectSlot)) {
					printf ("Error... CTransferManager::DummySelectExchange::IsReadyToReceive() fail... \n");
					PopupMsgBox("Line 1603", "cm slot aleady has wafer before place new wafer" , "Fail");
					return SYS_ABORTED;
				}

				//^^^^^^ Optional
				//2009.02.25
				if (1 == nDummyAlignOpt)
					if (SYS_ABORTED == FixedAlign(0 , nSelectSlot)) return SYS_ABORTED;
				////////////////////////////////////////////////////////

                if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , "FM" , nFinger))   return SYS_ABORTED;
				WRITE_DIGITAL(TR_FINGER      , nFinger                 , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_STATION , SrcCMArea->GetSWEnum()  , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_SLOT    , 0                       , &nIOStatus );
				WRITE_DIGITAL(TR_TRG_SLOT2   , nSelectSlot             , &nIOStatus );
				if (SYS_ABORTED == PutWafer()) return SYS_ABORTED;	

				//2008.10.14 Make Exchanged CM Wafer State to Processed
				ChangeWaferState(nSourceStation , nSelectSlot);
			}			
		} else {
			//CM Wafer Not Exist then just Dummy --> CM Transfer
			//get from dummy
			if (DummyArea->IsReadyToSend(SLOT1 , 0 , nDummySlot)) {
				if (FALSE == TrArea->IsReadyToReceive(SLOT2 , 0 , 0)) {
					printf ("Error... CTransferManager::DummySelectExchange::IsReadyToReceive() fail... \n");
					PopupMsgBox("Line 1619", "ATM Arm Aleady Wafer befor Get from dummy" , "Fail");
					return SYS_ABORTED;
				}
				//Get Wafer first if there exist send wafer
				if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , "FM" , nFinger))   return SYS_ABORTED;
				WRITE_DIGITAL(TR_FINGER      , nFinger, &nIOStatus);
				WRITE_DIGITAL(TR_SRC_STATION , DummyArea->GetSWEnum() , &nIOStatus);
				WRITE_DIGITAL(TR_SRC_SLOT    , 0                      , &nIOStatus);
				WRITE_DIGITAL(TR_SRC_SLOT2   , nDummySlot             , &nIOStatus);
				if (SYS_ABORTED == GetWafer()) return SYS_ABORTED;	

				//Reset Dummy Count [][][][]
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				memset (szDummySet , 0                , sizeof(szDummySet));
				sprintf(szDummySet , "RESET_COUNT %d" , nDummySlot);
				RUN_FUNCTION(DUMMY_MANAGEMENT         , szDummySet);
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			} 

			//put to cm
			if (TRUE == TrArea->IsReadyToSend(SLOT2 , 0 , 0)) {				
				if (FALSE == SrcCMArea->IsReadyToReceive(SLOT1 , 0 , nSelectSlot)) {
					printf ("Error... CTransferManager::DummySelectExchange::IsReadyToReceive() fail... \n");
					PopupMsgBox("Line 1637", "cm slot aleady has wafer before place new wafer" , "Fail");
					return SYS_ABORTED;
				}
				//^^^^^^ Optional
				//2009.02.25
				if (1 == nDummyAlignOpt)
					if (SYS_ABORTED == FixedAlign(0 , nSelectSlot)) return SYS_ABORTED;
				////////////////////////////////////////////////////////
                if (FALSE == m_pFingerAction->GetFingerEnum(SLOT2 , "FM" , nFinger))   return SYS_ABORTED;
				WRITE_DIGITAL(TR_FINGER      , nFinger                 , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_STATION , SrcCMArea->GetSWEnum()  , &nIOStatus);
				WRITE_DIGITAL(TR_TRG_SLOT    , 0                       , &nIOStatus );
				WRITE_DIGITAL(TR_TRG_SLOT2   , nSelectSlot             , &nIOStatus );
				if (SYS_ABORTED == PutWafer()) return SYS_ABORTED;	

				//2008.10.14 Make Exchanged CM Wafer State to Processed
				ChangeWaferState(nSourceStation , nSelectSlot);
				
			}			
		}
		nSelectSlot = GetNextCMSelectSlot();
		nDummySlot  = GetNextDummySelectSlot();
	}
	
	return nStatus;	
}//Function     : ChangeWaferState()
//Date         : 2008.10.14
//Description  : Change Wafer State after exchange dummy wafer
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CTransferManager::ChangeWaferState(int nSourceCM , int nSlot)
{
	int nIOStatus;
	switch (nSourceCM) {
	case 0:
		//CM1
		WRITE_DIGITAL(CM1_C01_Wafer + nSlot - 1  , 3 /*Process*/ , &nIOStatus );
		break;
	case 1:
		WRITE_DIGITAL(CM2_C01_Wafer + nSlot - 1  , 3 /*Process*/ , &nIOStatus );
		break;
	case 2:
		WRITE_DIGITAL(CM3_C01_Wafer + nSlot - 1  , 3 /*Process*/ , &nIOStatus );
		break;
	default: break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Auto Particle Check Function Start
// 2015.04.17
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferManager::Read_Parameter()
{
	BOOL bRst = TRUE;
	int nCycleCount,nIOComStatus; 
	char szPopUpString[256] = {"",};
		
	for(nCycleCount = 0; nCycleCount < 5; nCycleCount++)
	{
		m_pParticleChkJob[nCycleCount].nUPCCheck     = READ_DIGITAL(UPC_CHECK1DO + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nJobReserv     = READ_DIGITAL(PC_CheckJob1DO + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nLPMSlotSts1   = READ_DIGITAL(Sel1LPMSlt1DO + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nLPMSlotSts2   = READ_DIGITAL(Sel1LPMSlt2DO + nCycleCount,&nIOComStatus);
		
		m_pParticleChkJob[nCycleCount].nLLSts		    = READ_DIGITAL(Sel1LLDO		 + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nLLSlot1	    = READ_DIGITAL(Sel1LLSlt1DO  + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nLLSlot2	    = READ_DIGITAL(Sel1LLSlt2DO  + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nLLDoorCycNum  = READ_DIGITAL(LLDoorCyc1DO  + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nLLPumpCycNum  = READ_DIGITAL(LLPmVnCyc1DO  + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nLLSlitCycNum  = READ_DIGITAL(LLSlitCyc1DO  + nCycleCount,&nIOComStatus);
		
		m_pParticleChkJob[nCycleCount].nTMRobotSelSts = READ_DIGITAL(TM_Rb1SelDO   + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nTMRobotArmSts = READ_DIGITAL(TM_ArmSel1DO  + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nTMSlitCycNum  = READ_DIGITAL(TMSlitCyc1DO + nCycleCount,&nIOComStatus);
		
		m_pParticleChkJob[nCycleCount].nPMSelSts     = READ_DIGITAL(PM_Sel1DO    + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nPMSlitCycNum = READ_DIGITAL(PM_Slt1CycDO + nCycleCount,&nIOComStatus);
		m_pParticleChkJob[nCycleCount].nChuckCycNum  = READ_DIGITAL(Chuck1CycDO + nCycleCount,&nIOComStatus);

		m_pParticleChkJob[nCycleCount].nLoopCycNum  = READ_DIGITAL(Loop1CycDO + nCycleCount,&nIOComStatus);
	
		m_pParticleChkJob[nCycleCount].nChuckSel  = READ_DIGITAL(ChuckSel1DO + nCycleCount,&nIOComStatus);							
	    m_pParticleChkJob[nCycleCount].dbChuckUpPos  = READ_ANALOG(ChuckUpPos1 + nCycleCount,&nIOComStatus);       		 
		m_pParticleChkJob[nCycleCount].dbChuckDnPos  = READ_ANALOG(ChuckDnPos1 + nCycleCount,&nIOComStatus);

		if(m_pParticleChkJob[nCycleCount].nJobReserv != OFF && 
			m_pParticleChkJob[nCycleCount].nPMSelSts != NotUse3 &&
			(m_pParticleChkJob[nCycleCount].dbChuckUpPos >= m_pParticleChkJob[nCycleCount].dbChuckDnPos))
		{
			sprintf(szPopUpString,"Parameter Error Case%d Chuck Up Pos %0.2f > Down Pos %0.2f",nCycleCount+1,
				m_pParticleChkJob[nCycleCount].dbChuckUpPos,
				m_pParticleChkJob[nCycleCount].dbChuckDnPos);
			bRst = FALSE; 
			break;
		}
	}

	if(bRst == FALSE)
	{
		_gOutMsgBox("AutoParticle Test", szPopUpString, FALSE);
	}

	return bRst;
}

Module_Status CTransferManager::ParticleTest_CM_BM(int nCycleCount, int nStartModule, int nEndModule)
{
	Module_Status msRet = SYS_SUCCESS;
	int nMaxCount = 0;
	int nIOComStatus;
	char szPopUpString[256] = {"",};

	do 
	{
		// Source
		switch(nStartModule)
		{
		case T_CM1: WRITE_DIGITAL(TR_SRC_STATION, T_CM1, &nIOComStatus); break;
		case T_CM2: WRITE_DIGITAL(TR_SRC_STATION, T_CM2, &nIOComStatus); break;
		case T_CM3: WRITE_DIGITAL(TR_SRC_STATION, T_CM3, &nIOComStatus); break;
		case T_BM1: WRITE_DIGITAL(TR_SRC_STATION, T_BM1, &nIOComStatus); break;
		case T_BM2: WRITE_DIGITAL(TR_SRC_STATION, T_BM2, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;


		// Target
		switch(nEndModule)
		{
		case T_CM1: WRITE_DIGITAL(TR_TRG_STATION, T_CM1, &nIOComStatus); break;
		case T_CM2: WRITE_DIGITAL(TR_TRG_STATION, T_CM2, &nIOComStatus); break;
		case T_CM3: WRITE_DIGITAL(TR_TRG_STATION, T_CM3, &nIOComStatus); break;
		case T_BM1: WRITE_DIGITAL(TR_TRG_STATION, T_BM1, &nIOComStatus); break;
		case T_BM2: WRITE_DIGITAL(TR_TRG_STATION, T_BM2, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;

		if( (nStartModule == T_CM1 || nStartModule == T_CM2 || nStartModule == T_CM3) && 
			(nEndModule == T_BM1 ||nEndModule == T_BM2) )
		{
			WRITE_DIGITAL(TR_SRC_SLOT, m_pParticleChkJob[nCycleCount].nLPMSlotSts1, &nIOComStatus);
			WRITE_DIGITAL(TR_SRC_SLOT2, m_pParticleChkJob[nCycleCount].nLPMSlotSts2, &nIOComStatus);

			WRITE_DIGITAL(TR_TRG_BMSLOT, m_pParticleChkJob[nCycleCount].nLLSlot1, &nIOComStatus);
			WRITE_DIGITAL(TR_TRG_BMSLOT2, m_pParticleChkJob[nCycleCount].nLLSlot2, &nIOComStatus);
		}
		else
		{
			WRITE_DIGITAL(TR_SRC_BMSLOT, m_pParticleChkJob[nCycleCount].nLLSlot1, &nIOComStatus);
			WRITE_DIGITAL(TR_SRC_BMSLOT2, m_pParticleChkJob[nCycleCount].nLLSlot2, &nIOComStatus);

			WRITE_DIGITAL(TR_TRG_SLOT, m_pParticleChkJob[nCycleCount].nLPMSlotSts1, &nIOComStatus);
			WRITE_DIGITAL(TR_TRG_SLOT2, m_pParticleChkJob[nCycleCount].nLPMSlotSts2, &nIOComStatus);
		}
		msRet = MoveWafer();

	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_CM_BM Fail", FALSE);
	}
	return msRet;
}

Module_Status CTransferManager::ParticleTest_LL_Door_Control(int nCycleCount, int nModule)
{
	int nCycNum;
	Module_Status msRet = SYS_SUCCESS;
	int nIOComStatus;
	do 
	{
		if(nModule == T_BM1)
		{
			for(nCycNum = 0; nCycNum < m_pParticleChkJob[nCycleCount].nLLDoorCycNum; nCycNum++)
			{
				RUN_FUNCTION(SCHEDULER_MAINT_BM1,"DOOR_CLOSE");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM1))
				{
					msRet = SYS_ABORTED; 
					break;
				}
				RUN_FUNCTION(SCHEDULER_MAINT_BM1,"DOOR_OPEN");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM1))
				{
					msRet = SYS_ABORTED; 
					break;				
				}
				WRITE_DIGITAL(LLDoorCyc1DI+nCycleCount,nCycNum+1,&nIOComStatus);
			}
			if(msRet != SYS_SUCCESS) break;
		}
		else if(nModule == T_BM2)
		{
			for(nCycNum = 0; nCycNum < m_pParticleChkJob[nCycleCount].nLLDoorCycNum; nCycNum++)
			{
				RUN_FUNCTION(SCHEDULER_MAINT_BM2,"DOOR_CLOSE");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM2))
				{
					msRet = SYS_ABORTED; 
					break;
				}
				RUN_FUNCTION(SCHEDULER_MAINT_BM2,"DOOR_OPEN");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM2))
				{
					msRet = SYS_ABORTED; 
					break;				
				}
				WRITE_DIGITAL(LLDoorCyc1DI+nCycleCount,nCycNum+1,&nIOComStatus);

			}
			if(msRet != SYS_SUCCESS) break;
		}
	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_LL_Door_Control Fail", FALSE);
	}

	return msRet;
}

Module_Status CTransferManager::ParticleTest_LL_Pump_Vent_Control(int nCycleCount, int nModule)
{
	int nCycNum;
	Module_Status msRet = SYS_SUCCESS;
	int nIOComStatus;

	do 
	{
		if(nModule == T_BM1)
		{
			for(nCycNum = 0; nCycNum < m_pParticleChkJob[nCycleCount].nLLDoorCycNum; nCycNum++)
			{
				RUN_FUNCTION(SCHEDULER_MAINT_BM1,"PUMP");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM1))
				{
					msRet = SYS_ABORTED; 
					break;
				}
				RUN_FUNCTION(SCHEDULER_MAINT_BM1,"VENT");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM1))
				{
					msRet = SYS_ABORTED; 
					break;				
				}
				WRITE_DIGITAL(LLPmVnCyc1DI+nCycleCount,nCycNum+1,&nIOComStatus);
			}
			RUN_FUNCTION(SCHEDULER_MAINT_BM1,"PUMP");
			if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED;}
			if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM1))
			{
				msRet = SYS_ABORTED; 				
			}

			if(msRet != SYS_SUCCESS) break;
		}
		else if(nModule == T_BM2)
		{
			for(nCycNum = 0; nCycNum < m_pParticleChkJob[nCycleCount].nLLDoorCycNum; nCycNum++)
			{
				RUN_FUNCTION(SCHEDULER_MAINT_BM2,"PUMP");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM2))
				{
					msRet = SYS_ABORTED; 
					break;
				}
				RUN_FUNCTION(SCHEDULER_MAINT_BM2,"VENT");
				if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
				if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM2))
				{
					msRet = SYS_ABORTED; 
					break;				
				}
				WRITE_DIGITAL(LLPmVnCyc1DI+nCycleCount,nCycNum+1,&nIOComStatus);
			}
			RUN_FUNCTION(SCHEDULER_MAINT_BM2,"PUMP");
			if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED;}
			if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_BM2))
			{
				msRet = SYS_ABORTED; 				
			}

			if(msRet != SYS_SUCCESS) break;
		}
	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_LL_Pump_Vent_Control Fail", FALSE);
	}

	return msRet;
}

Module_Status CTransferManager::ParticleTest_TMPick(int nCycleCount, int nModule)
{
	Module_Status msRet = SYS_SUCCESS;
	int nMaxCount = 0;
	int nIOComStatus;
	int nPMPortketSts1, nPMPortketSts2;

	do 
	{
		// Source
		switch(nModule)
		{
		case T_PMA: WRITE_DIGITAL(TR_SRC_STATION, T_PMA, &nIOComStatus); break;
		case T_PMB: WRITE_DIGITAL(TR_SRC_STATION, T_PMB, &nIOComStatus); break;
		case T_PMC: WRITE_DIGITAL(TR_SRC_STATION, T_PMC, &nIOComStatus); break;
		case T_BM1: WRITE_DIGITAL(TR_SRC_STATION, T_BM1, &nIOComStatus); break;
		case T_BM2: WRITE_DIGITAL(TR_SRC_STATION, T_BM2, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;


		// Target
		switch(nModule)
		{
		case T_PMA: WRITE_DIGITAL(TR_TRG_STATION, T_PMA, &nIOComStatus); break;
		case T_PMB: WRITE_DIGITAL(TR_TRG_STATION, T_PMB, &nIOComStatus); break;
		case T_PMC: WRITE_DIGITAL(TR_TRG_STATION, T_PMC, &nIOComStatus); break;
		case T_BM1: WRITE_DIGITAL(TR_TRG_STATION, T_BM1, &nIOComStatus); break;
		case T_BM2: WRITE_DIGITAL(TR_TRG_STATION, T_BM2, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;
		
		// Arm Selection
		switch(m_pParticleChkJob[nCycleCount].nTMRobotArmSts)
		{
		case Para_UpArm: WRITE_DIGITAL(TR_FINGER, IO_UpArm, &nIOComStatus); break;
		case Para_LwArm: WRITE_DIGITAL(TR_FINGER, IO_LwArm, &nIOComStatus); break;
		case Para_AutoArm: WRITE_DIGITAL(TR_FINGER, IO_AutoArm, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;

		if(m_pParticleChkJob[nCycleCount].nLLSlot1 > 0) nPMPortketSts1 = 1;
		else nPMPortketSts1 = 0;
		if(m_pParticleChkJob[nCycleCount].nLLSlot2 > 0) nPMPortketSts2 = 2;
		else nPMPortketSts2 = 0;

		if(nModule == T_PMA || nModule == T_PMB || nModule == T_PMC)
		{
			WRITE_DIGITAL(TR_SRC_PMSLOT, nPMPortketSts1, &nIOComStatus);
			WRITE_DIGITAL(TR_SRC_PMSLOT2, nPMPortketSts2, &nIOComStatus);

			WRITE_DIGITAL(TR_TRG_PMSLOT, nPMPortketSts1, &nIOComStatus);
			WRITE_DIGITAL(TR_TRG_PMSLOT2, nPMPortketSts2, &nIOComStatus);
		}
		else if(nModule == T_BM1 || nModule == T_BM2)
		{
			WRITE_DIGITAL(TR_SRC_BMSLOT, m_pParticleChkJob[nCycleCount].nLLSlot1, &nIOComStatus);
			WRITE_DIGITAL(TR_SRC_BMSLOT2, m_pParticleChkJob[nCycleCount].nLLSlot2, &nIOComStatus);

			WRITE_DIGITAL(TR_TRG_BMSLOT, m_pParticleChkJob[nCycleCount].nLLSlot1, &nIOComStatus);
			WRITE_DIGITAL(TR_TRG_BMSLOT2, m_pParticleChkJob[nCycleCount].nLLSlot2, &nIOComStatus);
		}
		msRet= GetWafer();

	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_TMPick Fail", FALSE);
	}

	return msRet;	
}

Module_Status CTransferManager::ParticleTest_TMPlace(int nCycleCount, int nModule)
{
	Module_Status msRet = SYS_SUCCESS;

	int nMaxCount = 0;
	int nIOComStatus;
	int nPMPortketSts1, nPMPortketSts2;

	do 
	{
		// Source
		switch(nModule)
		{
		case T_PMA: WRITE_DIGITAL(TR_SRC_STATION, T_PMA, &nIOComStatus); break;
		case T_PMB: WRITE_DIGITAL(TR_SRC_STATION, T_PMB, &nIOComStatus); break;
		case T_PMC: WRITE_DIGITAL(TR_SRC_STATION, T_PMC, &nIOComStatus); break;
		case T_BM1: WRITE_DIGITAL(TR_SRC_STATION, T_BM1, &nIOComStatus); break;
		case T_BM2: WRITE_DIGITAL(TR_SRC_STATION, T_BM2, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;


		// Target
		switch(nModule)
		{
		case T_PMA: WRITE_DIGITAL(TR_TRG_STATION, T_PMA, &nIOComStatus); break;
		case T_PMB: WRITE_DIGITAL(TR_TRG_STATION, T_PMB, &nIOComStatus); break;
		case T_PMC: WRITE_DIGITAL(TR_TRG_STATION, T_PMC, &nIOComStatus); break;
		case T_BM1: WRITE_DIGITAL(TR_TRG_STATION, T_BM1, &nIOComStatus); break;
		case T_BM2: WRITE_DIGITAL(TR_TRG_STATION, T_BM2, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;
		
		// Arm Selection
		switch(m_pParticleChkJob[nCycleCount].nTMRobotArmSts)
		{
		case Para_UpArm: WRITE_DIGITAL(TR_FINGER, IO_UpArm, &nIOComStatus); break;
		case Para_LwArm: WRITE_DIGITAL(TR_FINGER, IO_LwArm, &nIOComStatus); break;
		case Para_AutoArm: WRITE_DIGITAL(TR_FINGER, IO_AutoArm, &nIOComStatus); break;
		default: msRet = SYS_ABORTED;
		}
		if(SYS_SUCCESS != msRet) break;

		if(m_pParticleChkJob[nCycleCount].nLLSlot1 > 0) nPMPortketSts1 = 1;
		else nPMPortketSts1 = 0;
		if(m_pParticleChkJob[nCycleCount].nLLSlot2 > 0) nPMPortketSts2 = 2;
		else nPMPortketSts2 = 0;

		if(nModule == T_PMA || nModule == T_PMB || nModule == T_PMC)
		{
			WRITE_DIGITAL(TR_SRC_PMSLOT, nPMPortketSts1, &nIOComStatus);
			WRITE_DIGITAL(TR_SRC_PMSLOT2, nPMPortketSts2, &nIOComStatus);

			WRITE_DIGITAL(TR_TRG_PMSLOT, nPMPortketSts1, &nIOComStatus);
			WRITE_DIGITAL(TR_TRG_PMSLOT2, nPMPortketSts2, &nIOComStatus);
		}
		else if(nModule == T_BM1 || nModule == T_BM2)
		{
			WRITE_DIGITAL(TR_SRC_BMSLOT, m_pParticleChkJob[nCycleCount].nLLSlot1, &nIOComStatus);
			WRITE_DIGITAL(TR_SRC_BMSLOT2, m_pParticleChkJob[nCycleCount].nLLSlot2, &nIOComStatus);

			WRITE_DIGITAL(TR_TRG_BMSLOT, m_pParticleChkJob[nCycleCount].nLLSlot1, &nIOComStatus);
			WRITE_DIGITAL(TR_TRG_BMSLOT2, m_pParticleChkJob[nCycleCount].nLLSlot2, &nIOComStatus);
		}
		msRet= PutWafer();

	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_TMPlace Fail", FALSE);
	}

	return msRet;	
}

Module_Status CTransferManager::ParticleTest_SlitVlv_Control(int nCycleCount, int nModule, BOOL bPMSide)
{
	int nCycNum;
	int nIOCommStatus;
	Module_Status msRet = SYS_SUCCESS;
	int nSlitIO;
	int nCycleCnt;
	int nStationIO;
	

	if(nModule == T_BM1)
	{
		nCycleCnt = m_pParticleChkJob[nCycleCount].nLLSlitCycNum;
		nSlitIO = LLSlitCyc1DI;
		nStationIO = IO_BM1;
	}
	else if(nModule == T_BM2)
	{
		nCycleCnt = m_pParticleChkJob[nCycleCount].nLLSlitCycNum;
		nSlitIO = LLSlitCyc1DI;
		nStationIO = IO_BM2;

	}
	else if(nModule == T_PMA)
	{
		if(bPMSide == FALSE)
		{ 
			nCycleCnt = m_pParticleChkJob[nCycleCount].nTMSlitCycNum;
			nSlitIO = TMSlitCyc1DI;
		}
		else
		{
			nCycleCnt = m_pParticleChkJob[nCycleCount].nPMSlitCycNum;
			nSlitIO = PM_Slt1CycDI;
		}
		nStationIO = IO_PMA;
	}
	else if(nModule == T_PMB)
	{
		if(bPMSide == FALSE)
		{ 
			nCycleCnt = m_pParticleChkJob[nCycleCount].nTMSlitCycNum;
			nSlitIO = TMSlitCyc1DI;
		}
		else
		{
			nCycleCnt = m_pParticleChkJob[nCycleCount].nPMSlitCycNum;
			nSlitIO = PM_Slt1CycDI;
		}
		nStationIO = IO_PMB;
	}
	else if(nModule == T_PMC)
	{
		if(bPMSide == FALSE)
		{ 
			nCycleCnt = m_pParticleChkJob[nCycleCount].nTMSlitCycNum;
			nSlitIO = TMSlitCyc1DI;
		}
		else
		{
			nCycleCnt = m_pParticleChkJob[nCycleCount].nPMSlitCycNum;
			nSlitIO = PM_Slt1CycDI;
		}
		nStationIO = IO_PMC;
	}

	do 
	{
		for(nCycNum = 0; nCycNum < nCycleCnt; nCycNum++)
		{
			WRITE_DIGITAL(TM_MAINT_STATION3,nStationIO,&nIOCommStatus);
			RUN_FUNCTION(SCHEDULER_MAINT_TM,"GATE_OPEN");
			if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
			if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_TM))
			{
				msRet = SYS_ABORTED; 
				break;
			}
			WRITE_DIGITAL(TM_MAINT_STATION3,nStationIO,&nIOCommStatus);
			RUN_FUNCTION(SCHEDULER_MAINT_TM,"GATE_CLOSE");
			if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
			if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_TM))
			{
				msRet = SYS_ABORTED; 
				break;				
			}
			WRITE_DIGITAL(nSlitIO+nCycleCount,nCycNum+1,&nIOCommStatus);
		}
		if(msRet != SYS_SUCCESS) break;

	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_SlitVlv_Control Fail", FALSE);
	}

	return msRet;
}

Module_Status CTransferManager::ParticleTest_PM_Chuck_Control(int nCycleCount, int nModule)
{
	int nCycNum;
	int nIOCommStatus;
	Module_Status msRet = SYS_SUCCESS;
	int nStationIO;
	int nChuckFuc;
	int nChuckSelIO;
    int nChuckPosIO;

	if(nModule == T_PMA)
	{
		nStationIO = IO_PMA;
		nChuckFuc = FUNC_CALL_PM1;
		nChuckSelIO = PM1_ZMT_StageSelectDM;
		nChuckPosIO = PM1_ZMT_MaintSetPosAM;
	}
	else if(nModule == T_PMB)
	{
		nStationIO = IO_PMB;
		nChuckFuc = FUNC_CALL_PM2;
		nChuckSelIO = PM2_ZMT_StageSelectDM;
		nChuckPosIO = PM2_ZMT_MaintSetPosAM;
	}
	else if(nModule == T_PMC)
	{
		nStationIO = IO_PMC;
		nChuckFuc = FUNC_CALL_PM3;
		nChuckSelIO = PM3_ZMT_StageSelectDM;
		nChuckPosIO = PM3_ZMT_MaintSetPosAM;
	}

	do 
	{
		for(nCycNum = 0; nCycNum < m_pParticleChkJob[nCycleCount].nChuckCycNum; nCycNum++)
		{       		 
			WRITE_DIGITAL(nChuckSelIO,m_pParticleChkJob[nCycleCount].nChuckSel,&nIOCommStatus);
			WRITE_ANALOG(nChuckPosIO,m_pParticleChkJob[nCycleCount].dbChuckUpPos,&nIOCommStatus);
			WRITE_DIGITAL(TM_MAINT_STATION3,nStationIO,&nIOCommStatus);
			RUN_FUNCTION(nChuckFuc,"CHUCKRUN");
			if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
			if(SYS_SUCCESS != READ_FUNCTION(nChuckFuc))
			{
				msRet = SYS_ABORTED; 
				break;
			}

			WRITE_DIGITAL(nChuckSelIO,m_pParticleChkJob[nCycleCount].nChuckSel,&nIOCommStatus);
			WRITE_ANALOG(nChuckPosIO,m_pParticleChkJob[nCycleCount].dbChuckDnPos,&nIOCommStatus);
			WRITE_DIGITAL(TM_MAINT_STATION3,nStationIO,&nIOCommStatus);
			RUN_FUNCTION(nChuckFuc,"CHUCKRUN");
			if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
			if(SYS_SUCCESS != READ_FUNCTION(nChuckFuc))
			{
				msRet = SYS_ABORTED; 
				break;				
			}
			WRITE_DIGITAL(Chuck1CycDI+nCycleCount,nCycNum+1,&nIOCommStatus);
		}
		if(msRet != SYS_SUCCESS) break;
	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_PM_Chuck_Control Fail", FALSE);
	}

	return msRet;
}

Module_Status CTransferManager::ParticleTest_TMRotate(int nCycleCount,int nModule)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOCommStatus;
	int nStationIO;


	switch(m_pParticleChkJob[nCycleCount].nTMRobotArmSts)	
	{
	case Para_UpArm:
		WRITE_DIGITAL(TM_MAINT_FINGER,IO_UpArm,&nIOCommStatus);
		break;
	case Para_LwArm:
		WRITE_DIGITAL(TM_MAINT_FINGER,IO_LwArm,&nIOCommStatus);
		break;
	case Para_AutoArm:
		WRITE_DIGITAL(TM_MAINT_FINGER,IO_UpArm,&nIOCommStatus);
		break;
	default:
		PopupMsgBox("AutoParticle Test" , "TM Robot" , "Arm Selection Fail");
		msRet = SYS_ABORTED;
		break;
	}
	
	if(msRet != SYS_SUCCESS) return msRet;

	if(nModule == T_PMA)
	{
		nStationIO = IO_PMA;
	}
	else if(nModule == T_PMB)
	{
		nStationIO = IO_PMB;
	}
	else if(nModule == T_PMC)
	{
		nStationIO = IO_PMC;
	}


	do 
	{    
		WRITE_DIGITAL(TM_MAINT_STATION4,nStationIO,&nIOCommStatus);
		RUN_FUNCTION(SCHEDULER_MAINT_TM,"RB_ROTATE");
		if(!WAIT_SECONDS(3)){msRet = SYS_ABORTED; break;}
		if(SYS_SUCCESS != READ_FUNCTION(SCHEDULER_MAINT_TM))
		{
			msRet = SYS_ABORTED; 
			break;
		}

	} while(0);

	if(msRet != SYS_SUCCESS)
	{
		_gOutMsgBox("AutoParticle Test", "ParticleTest_TMRotate Fail", FALSE);
	}

	return msRet;
}

BOOL CTransferManager::Check_PMStatus()
{
	int i;
	int nIOCommStatus;
	BOOL bRet = TRUE;

	for(i = 0; i < 5; i++)
	{
		if(m_pParticleChkJob[i].nPMSelSts == Para_PMA)
		{
			if( 4 != READ_DIGITAL(PM1_CommStatus,&nIOCommStatus)) //2 = Discon 3 = Standby 4 = Maint 
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMA] Change to Maint", FALSE);
				break;
			}
			if(0 != READ_DIGITAL(PM1_PRO_RunStsDM,&nIOCommStatus))   // 0 = Stop 1 = Running
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMA] Need to Stop Process", FALSE);
				break;
			}
			//... 2015.07.01
			if(SYS_SUCCESS != RUN_FUNCTION(FUNC_CALL_PM1 , "ABORT_GAS"))
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMA] Need to Stop Manual Purge Process", FALSE);
				break;
			}	
		}	
		if(m_pParticleChkJob[i].nPMSelSts == Para_PMB)
		{
			if( 4 != READ_DIGITAL(PM2_CommStatus,&nIOCommStatus)) //2 = Discon 3 = Standby 4 = Maint 
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMB] Change to Maint", FALSE);
				break;
			}
			if(0 != READ_DIGITAL(PM2_PRO_RunStsDM,&nIOCommStatus))   // 0 = Stop 1 = Running
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMB] Need to Stop Process", FALSE);
				break;
			}	
			//... 2015.07.01
			if(SYS_SUCCESS != RUN_FUNCTION(FUNC_CALL_PM2 , "ABORT_GAS"))
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMB] Need to Stop Manual Purge Process", FALSE);
				break;
			}	
		}	
		if(m_pParticleChkJob[i].nPMSelSts == Para_PMC)
		{
			if( 4 != READ_DIGITAL(PM3_CommStatus,&nIOCommStatus)) //2 = Discon 3 = Standby 4 = Maint 
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMC] Change to Maint", FALSE);
				break;
			}
			if(0 != READ_DIGITAL(PM3_PRO_RunStsDM,&nIOCommStatus))   // 0 = Stop 1 = Running
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMC] Need to Stop Process", FALSE);
				break;
			}	
			//... 2015.07.01
			if(SYS_SUCCESS != RUN_FUNCTION(FUNC_CALL_PM3 , "ABORT_GAS"))
			{
				bRet = FALSE;
				_gOutMsgBox("AutoParticle Test", "[PMC] Need to Stop Manual Purge Process", FALSE);
				break;
			}		
		}	
	}
	return bRet;
}

BOOL CTransferManager::Check_WaferStatus()
{
	BOOL bRet = TRUE;
	int nIOCommStatus;
	do 
	{
		if(0 < READ_DIGITAL(FA_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(FB_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(TA_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(TA_Wafer_Status2,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(TB_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(TB_Wafer_Status2,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(PM1_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(PM1_Wafer_Status2,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(PM2_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(PM2_Wafer_Status2,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(PM3_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(PM3_Wafer_Status2,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM1_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM1_Wafer_Status2,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM1_Wafer_Status3,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM1_Wafer_Status4,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM2_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM2_Wafer_Status2,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM2_Wafer_Status3,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(BM2_Wafer_Status4,&nIOCommStatus)){bRet = FALSE; break;}
		if(0 < READ_DIGITAL(FM_AL_Wafer_Status,&nIOCommStatus)){bRet = FALSE; break;}
	} while(0);

	if(bRet == FALSE)
	{
		_gOutMsgBox("AutoParticle Test", "Wafer is Present On EQ", FALSE);
	}
	return bRet;
}

void CTransferManager::IO_Reset()
{
	int nIOCommStatus;
	int i;
	for(i = 0; i < 5; i++)
	{
		WRITE_DIGITAL(LLDoorCyc1DI+i,0,&nIOCommStatus);
		WRITE_DIGITAL(LLSlitCyc1DI+i,0,&nIOCommStatus);
		WRITE_DIGITAL(LLPmVnCyc1DI+i,0,&nIOCommStatus);
		WRITE_DIGITAL(TMSlitCyc1DI+i,0,&nIOCommStatus);
		WRITE_DIGITAL(Chuck1CycDI+i, 0,&nIOCommStatus);
		WRITE_DIGITAL(PM_Slt1CycDI+i,0,&nIOCommStatus);
		WRITE_DIGITAL(Loop1CycDI+i,  0,&nIOCommStatus);
	}
}

BOOL CTransferManager::LPM_Mode_Check(int nCheckStart, int nPCCheckLPM)
{
	BOOL bRet = TRUE;
	int nIOComStatus;
	BOOL bLPM1 = TRUE,bLPM2 = TRUE,bLPM3 = TRUE;
	char schdBuffer[128];
	do 
	{
		// 0 = idle, 3 = Paused 
		if(nCheckStart == ON)
		{	  
			if(0 != READ_DIGITAL(CTC_MAIN_CONTROL,&nIOComStatus) &&    
				0 != READ_DIGITAL(CTC_MAIN_CONTROL2,&nIOComStatus)&& 
				0 != READ_DIGITAL(CTC_MAIN_CONTROL3,&nIOComStatus))
			{
				_gOutMsgBox("AutoParticle Test", "Can Not Start PC_Check Because other side LPM Run", FALSE);
				bRet = FALSE;
				break;
			}

			switch(nPCCheckLPM)
			{
			case T_CM1:
				{bLPM1 = FALSE,bLPM2 = TRUE,bLPM3 = TRUE;}
				break;
			case T_CM2:
				{bLPM1 = TRUE,bLPM2 = FALSE,bLPM3 = TRUE;}
				break;
			case T_CM3:
				{bLPM1 = TRUE,bLPM2 = TRUE,bLPM3 = FALSE;}
				break;
			default:
				_gOutMsgBox("AutoParticle Test", "LPM Selection Fail", FALSE);
				bRet = FALSE;
				break;
			}
			if(bRet == FALSE) break;
			RunBGChecker(bLPM1,bLPM1,bLPM1);
		}
		else //OFF 
		{
			KillBGChecker();

			if(3 == READ_DIGITAL(CTC_MAIN_CONTROL,&nIOComStatus))
			{
				sprintf(schdBuffer , "CONTINUE_F PORT1");
				WRITE_FUNCTION_EVENT(SCHEDULER , schdBuffer);
			}

			if(3 == READ_DIGITAL(CTC_MAIN_CONTROL2,&nIOComStatus))
			{
				sprintf(schdBuffer , "CONTINUE_F PORT2");
				WRITE_FUNCTION_EVENT(SCHEDULER , schdBuffer);
			}

			if(3 == READ_DIGITAL(CTC_MAIN_CONTROL3,&nIOComStatus))
			{
				sprintf(schdBuffer , "CONTINUE_F PORT3");
				WRITE_FUNCTION_EVENT(SCHEDULER , schdBuffer);
			}
		}
	} while(0);
	return bRet;
}

Module_Status CTransferManager::Over_Pressure_Control(int nCycleCount,int nIsControl)
{
	Module_Status msRet = SYS_SUCCESS;

	if (m_pParticleChkJob[nCycleCount].nUPCCheck == ON)
	{
		if(nIsControl == ON)
		{
			msRet = RUN_FUNCTION(TMC_TM_OVERPRESSURE,"OPEN_OPRES_VALVE");
		}
		else
		{
			msRet = RUN_FUNCTION(TMC_TM_OVERPRESSURE,"CLOSE_OPRES_VALVE");
		}
		
		if(msRet != SYS_SUCCESS)
		{
			_gOutMsgBox("AutoParticle Test", "TM UPC OverPressure Control Fail", FALSE);
		}
	}
	return msRet;
	
}
Module_Status CTransferManager::Auto_Particle_Check()
{
	Module_Status msRet = SYS_SUCCESS;
	int nCycleCount = 0;
	int nMaxCount = 0;
	int nIOComStatus, i, nLoopCnt = 0; 
	CTextParser txtP;
	int nLPMSts, nLLSts, nPMSts;
	int nStartModule = 0, nEndModule = 0; 

	printf("Auto Particle Test Start \n");
	
	nLPMSts = READ_DIGITAL(Sel_LPMDO,&nIOComStatus);
	
	WRITE_DIGITAL(PCJobSts,0,&nIOComStatus);
	
	if(nLPMSts <=0 )
	{
		_gOutMsgBox("AutoParticle Test", "[LPM] Do Not Select LPM", FALSE);
	}

	nLPMSts -=1; 

	WRITE_DIGITAL(AutoPC_ParmSaveDM,ON,&nIOComStatus);	

	if(FALSE == Read_Parameter()) return SYS_ABORTED;

	if(FALSE == Check_PMStatus()) return SYS_ABORTED;
	
	if(FALSE == Check_WaferStatus()) return SYS_ABORTED;

	if(FALSE == LPM_Mode_Check(ON,nLPMSts)) return SYS_ABORTED;

	IO_Reset();

	do {
		if(m_pParticleChkJob[nCycleCount].nJobReserv == OFF ) continue;

		printf("---> Auto  Particle Job[%d] Started\n",nCycleCount + 1);
 
		if(m_pParticleChkJob[nCycleCount].nLLSts == Para_LL1) nLLSts = T_BM1;
		if(m_pParticleChkJob[nCycleCount].nLLSts == Para_LL2) nLLSts = T_BM2;
		if(m_pParticleChkJob[nCycleCount].nPMSelSts == Para_PMA) nPMSts = T_PMA;
		if(m_pParticleChkJob[nCycleCount].nPMSelSts == Para_PMB) nPMSts = T_PMB;
		if(m_pParticleChkJob[nCycleCount].nPMSelSts == Para_PMC) nPMSts = T_PMC;
		
		if(FALSE == Check_PMStatus()){msRet = SYS_ABORTED; break;}
		
		WRITE_DIGITAL(PCJobSts,nCycleCount + 1,&nIOComStatus);

		msRet = ParticleTest_CM_BM(nCycleCount, nLPMSts, nLLSts);					// CM -> BM	
		if(SYS_SUCCESS != msRet) break;

		msRet = ParticleTest_LL_Door_Control(nCycleCount,nLLSts);
		if(SYS_SUCCESS != msRet) break;

		msRet = ParticleTest_LL_Pump_Vent_Control(nCycleCount,nLLSts);
		if(SYS_SUCCESS != msRet) break;

		msRet = Over_Pressure_Control(nCycleCount,ON);								// TM OverPresure ON
		if(SYS_SUCCESS != msRet) break;

		msRet = ParticleTest_SlitVlv_Control(nCycleCount,nLLSts,FALSE);				// BM Slit
		if(SYS_SUCCESS != msRet) break;
		
		if(m_pParticleChkJob[nCycleCount].nTMRobotArmSts != NotUse1 )
		{
			msRet = ParticleTest_TMPick(nCycleCount,nLLSts);						// From BM Pick
			if(SYS_SUCCESS != msRet) break;

			msRet = ParticleTest_TMRotate(nCycleCount,nPMSts);						// TM Robot Rotate
			if(SYS_SUCCESS != msRet) break;

			msRet = ParticleTest_SlitVlv_Control(nCycleCount,nPMSts,FALSE);			// PM Slit Control before PM Place
			if(SYS_SUCCESS != msRet) break;

			if(m_pParticleChkJob[nCycleCount].nPMSelSts != NotUse3 )
			{
				for(i = 0 ; i <= m_pParticleChkJob[nCycleCount].nLoopCycNum; i++)
				{
					msRet = ParticleTest_TMPlace(nCycleCount,nPMSts);				// To PM Place	
					if(SYS_SUCCESS != msRet) break;

					msRet = ParticleTest_PM_Chuck_Control(nCycleCount,nPMSts);		
					if(SYS_SUCCESS != msRet) break;

					msRet = ParticleTest_SlitVlv_Control(nCycleCount,nPMSts,TRUE);	// PM Slit Control after PM Place
					if(SYS_SUCCESS != msRet) break;

					msRet = ParticleTest_TMPick(nCycleCount,nPMSts);				// From PM Pick
					if(SYS_SUCCESS != msRet) break;

					WRITE_DIGITAL(Loop1CycDI + nCycleCount, nLoopCnt++,&nIOComStatus);		
				}
				msRet = ParticleTest_TMPlace(nCycleCount,nLLSts);					// to BM Place
				if(SYS_SUCCESS != msRet) break;

				msRet = ParticleTest_CM_BM(nCycleCount, nLLSts, nLPMSts);			// BM -> CM
				if(SYS_SUCCESS != msRet) break;

			}
			else
			{
				msRet = ParticleTest_TMPlace(nCycleCount,nLLSts);					// to BM Place
				if(SYS_SUCCESS != msRet) break;

				msRet = ParticleTest_CM_BM(nCycleCount, nLLSts, nLPMSts);			// BM -> CM
				if(SYS_SUCCESS != msRet) break;
			}
		}
		else
		{
			msRet = ParticleTest_CM_BM(nCycleCount, nLLSts, nLPMSts);				// BM -> CM
			if(SYS_SUCCESS != msRet) break;
		}
		if(! WAIT_SECONDS(8)) break;
		nLoopCnt = 0;

		msRet = Over_Pressure_Control(nCycleCount,OFF);								// TM OverPresure ON
		if(SYS_SUCCESS != msRet) break;

	} while(++nCycleCount < 5);

	WRITE_DIGITAL(PCJobSts,0,&nIOComStatus);

	WRITE_DIGITAL(AutoPC_ParmSaveDM,ON,&nIOComStatus);

	LPM_Mode_Check(OFF,nLPMSts);

	return msRet;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Auto Particle Check Function End
//////////////////////////////////////////////////////////////////////////////////////////////////////


