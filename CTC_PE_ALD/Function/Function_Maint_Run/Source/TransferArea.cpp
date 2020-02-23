// TransferArea.cpp: implementation of the CTransferArea class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TransferArea.h"
#include "Area.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTransferArea::CTransferArea()
{
	m_psAreaList    = NULL;
	m_psCurrentList = NULL;
	m_psStartList   = NULL;
	m_psCurrentClear= NULL;
}

CTransferArea::~CTransferArea()
{
	sArea* psArea = NULL;
	sArea* psNext = NULL;
	sArea* psTemp = NULL;
	psArea = m_psStartList;
	psNext = m_psStartList->pNext;
	do {		
		if (NULL == psNext) {
			//delete current & break;
			if (NULL != psArea) {
				if (NULL != psArea->pArea) delete psArea->pArea;
				delete psArea;				
			}
			break;
		} else {
			psTemp = psNext;
			if (NULL != psArea) {
				if (NULL != psArea->pArea) delete psArea->pArea;
				delete psArea;
			}
			psArea = psTemp;
			psNext = psTemp->pNext;
		}
	} while (1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsValidArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferArea::IsValidArea (char* szAreaName , int nSWEnum)
{
	BOOL bRet = TRUE;
	sArea* psArea = NULL;

	psArea = m_psStartList;
	//Not Yet created any area
	if (NULL == psArea) return TRUE;
	while (NULL != psArea) {
		if (0 == strcmp(psArea->szAreaName , szAreaName)) {bRet = FALSE; break;}
		if (nSWEnum == psArea->nSWEnum)                   {bRet = FALSE; break;}
		psArea = psArea->pNext;
	}

	return bRet;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : AddArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferArea::AddArea(char* szAreaName , int nMaxSlot , int nCapacity , int nSWEnum)
{
	BOOL bRet = TRUE;
	do {
		if (FALSE == IsValidArea(szAreaName , nSWEnum)) {bRet = FALSE; break;}
		if (NULL == m_psCurrentList) {
			//Initial Create
			m_psAreaList = new sArea;
			strcpy(m_psAreaList->szAreaName , szAreaName);
			m_psAreaList->nMaxSlot  = nMaxSlot;
			m_psAreaList->nCapacity = nCapacity;
			m_psAreaList->nSWEnum   = nSWEnum;

			m_psCurrentList = m_psAreaList;
			m_psStartList   = m_psAreaList;
		} else {
			m_psCurrentList->pNext = new sArea;
			strcpy(m_psCurrentList->pNext->szAreaName , szAreaName);
			m_psCurrentList->pNext->nMaxSlot  = nMaxSlot;
			m_psCurrentList->pNext->nCapacity = nCapacity;
			m_psCurrentList->pNext->nSWEnum   = nSWEnum;

			m_psCurrentList->pPrev = m_psCurrentList;
			m_psCurrentList = m_psCurrentList->pNext;
		}
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetFirstArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : sArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
sArea* CTransferArea::GetFirstArea()
{
	m_psCurrentList = m_psStartList;
	return m_psCurrentList;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetNextArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : sArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
sArea* CTransferArea::GetNextArea ()
{
	m_psCurrentList = m_psCurrentList->pNext;
	return m_psCurrentList;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetFirstClear()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : sArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
sArea* CTransferArea::GetFirstClear()
{
	m_psCurrentClear = m_psStartList;
	return m_psCurrentClear;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetNextClear()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : sArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
sArea* CTransferArea::GetNextClear ()
{
	m_psCurrentClear = m_psCurrentClear->pNext;
	return m_psCurrentClear;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetAreaCount()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CTransferArea::GetAreaCount()
{
	int nCount = 0;
	if (NULL != m_psStartList) nCount++;
	else return 0;

	sArea* psArea = NULL;
	psArea = m_psStartList;
	while (NULL != psArea) {
		if (NULL == psArea->pNext) break;
		else nCount++;

		psArea = psArea->pNext;
	}

	return nCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : CArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
CArea* CTransferArea::GetArea(char* szAreaName)
{
	CArea* pArea = NULL;
	m_psCurrentList = m_psStartList;
	while (NULL != m_psCurrentList) {
		if (0 == strcmp(m_psCurrentList->szAreaName , szAreaName)) {
			pArea = m_psCurrentList->pArea;
			break;
		}
		m_psCurrentList = m_psCurrentList->pNext;
	}

	return pArea;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : CArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
CArea* CTransferArea::GetArea(int nSWEnum)
{
	CArea* pArea = NULL;
	m_psCurrentList = m_psStartList;
	while (NULL != m_psCurrentList) {
		if (nSWEnum == m_psCurrentList->nSWEnum) {
			pArea = m_psCurrentList->pArea;
			break;
		}
		m_psCurrentList = m_psCurrentList->pNext;
	}

	return pArea;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetSArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : sArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
sArea* CTransferArea::GetSArea(char* szAreaName)
{
	sArea* psArea = NULL;
	m_psCurrentList = m_psStartList;
	while (NULL != m_psCurrentList) {
		if (0 == strcmp(m_psCurrentList->szAreaName , szAreaName)) {
			return m_psCurrentList;
			break;
		}
		m_psCurrentList = m_psCurrentList->pNext;
	}

	return psArea;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetSArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : sArea*
//////////////////////////////////////////////////////////////////////////////////////////////////////
sArea* CTransferArea::GetSArea(int nSWEnum)
{
	sArea* psArea = NULL;
	m_psCurrentList = m_psStartList;
	while (NULL != m_psCurrentList) {
		if (nSWEnum == m_psCurrentList->nSWEnum) {
			return m_psCurrentList;
			break;
		}
		m_psCurrentList = m_psCurrentList->pNext;
	}

	return psArea;
}