// FingerAction.cpp: implementation of the CFingerAction class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FingerAction.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFingerAction::CFingerAction()
{
	m_psFingerList  = NULL;
	m_psCurrentList = NULL;
	m_psStartList   = NULL;

}

CFingerAction::~CFingerAction()
{
	sFinger* psFinger = NULL;
	sFinger* psNext   = NULL;
	sFinger* psTemp   = NULL;
	psFinger = m_psStartList;
	psNext   = m_psStartList->pNext;
	do {		
		if (NULL == psNext) {
			//delete current & break;
			delete psFinger;
			break;
		} else {
			psTemp = psNext;
			if (NULL != psFinger) delete psFinger;
			psFinger = psTemp;
			psNext   = psTemp->pNext;
		}
	} while (1);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsValidFingerAction()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFingerAction::IsValidFingerAction (int nSWEnum)
{
	BOOL bRet = TRUE;
	sFinger* psFinger = NULL;

	psFinger = m_psStartList;
	//Not Yet created any Finger
	if (NULL == psFinger) return TRUE;
	while (NULL != psFinger) {
		if (nSWEnum == psFinger->nSWEnum) {bRet = FALSE; break;}
		psFinger = psFinger->pNext;
	}

	return bRet;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : AddFingerAction()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFingerAction::AddFingerAction(char* szFingerName , char* szActionType , int nSWEnum)
{
	BOOL bRet = TRUE;
	do {
		if (FALSE == IsValidFingerAction(nSWEnum)) {bRet = FALSE; break;}
		if (NULL == m_psCurrentList) {
			//Initial Create
			m_psFingerList = new sFinger;
			strcpy(m_psFingerList->szFingerName , szFingerName);
			m_psFingerList->nSWEnum   = nSWEnum;
			if      (0 == strcmp("SLOT1" , szActionType)) m_psFingerList->enFingerAction = SLOT1;
			else if (0 == strcmp("SLOT2" , szActionType)) m_psFingerList->enFingerAction = SLOT2;
			else if (0 == strcmp("SLOT3" , szActionType)) m_psFingerList->enFingerAction = SLOT3;
			else if (0 == strcmp("SLOT4" , szActionType)) m_psFingerList->enFingerAction = SLOT4;
			else if (0 == strcmp("AUTO"  , szActionType)) m_psFingerList->enFingerAction = AUTO;
			else if (0 == strcmp("ALL"   , szActionType)) m_psFingerList->enFingerAction = ALL;
			else    {bRet = FALSE;break;}

			m_psCurrentList = m_psFingerList;
			m_psStartList   = m_psFingerList;
		} else {
			m_psCurrentList->pNext = new sFinger;
			strcpy(m_psCurrentList->pNext->szFingerName , szFingerName);
			m_psCurrentList->pNext->nSWEnum   = nSWEnum;
			if      (0 == strcmp("SLOT1" , szActionType)) m_psCurrentList->pNext->enFingerAction = SLOT1;
			else if (0 == strcmp("SLOT2" , szActionType)) m_psCurrentList->pNext->enFingerAction = SLOT2;
			else if (0 == strcmp("SLOT3" , szActionType)) m_psCurrentList->pNext->enFingerAction = SLOT3;
			else if (0 == strcmp("SLOT4" , szActionType)) m_psCurrentList->pNext->enFingerAction = SLOT4;
			else if (0 == strcmp("AUTO"  , szActionType)) m_psCurrentList->pNext->enFingerAction = AUTO;
			else if (0 == strcmp("ALL"   , szActionType)) m_psCurrentList->pNext->enFingerAction = ALL;
			else    {bRet = FALSE;break;}

			m_psCurrentList->pPrev = m_psCurrentList;
			m_psCurrentList = m_psCurrentList->pNext;
		}
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetFingerInfo()
//Date         : 2007.06.25
//Description  : Find Finger type by given SW Enum
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFingerAction::GetFingerInfo(int nSWEnum , char* szFingerName , FingerACTType& enType) 
{
	BOOL bRet = FALSE;
	if (NULL == m_psStartList) return FALSE;

	m_psCurrentList = m_psStartList;
	while (NULL != m_psCurrentList) {
		if (m_psCurrentList->nSWEnum == nSWEnum) {
			strcpy(szFingerName , m_psCurrentList->szFingerName);
			enType = m_psCurrentList->enFingerAction;
			bRet = TRUE; break;
		}
		m_psCurrentList = m_psCurrentList->pNext;
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetFingerEnum()
//Date         : 2007.06.25
//Description  : Find SW Set Arm selection value by given finger type & area name
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFingerAction::GetFingerEnum(FingerACTType enType , char* szFingerName/*OUT*/ , int& nSWEnum)
{
	BOOL bRet = FALSE;

	if (NULL == m_psStartList) return FALSE;

	m_psCurrentList = m_psStartList;
	while (NULL != m_psCurrentList) {
		if (m_psCurrentList->enFingerAction == enType && 0 == strcmp(szFingerName , m_psCurrentList->szFingerName)) {
			nSWEnum = m_psCurrentList->nSWEnum;
			bRet = TRUE; break;
		}
		m_psCurrentList = m_psCurrentList->pNext;
	}

	return bRet;
}