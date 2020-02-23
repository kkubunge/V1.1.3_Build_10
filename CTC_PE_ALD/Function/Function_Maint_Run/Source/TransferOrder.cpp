// TransferOrder.cpp: implementation of the CTransferOrder class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TransferOrder.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransferOrder::CTransferOrder()
{
	m_nStepCount = 0;
}

CTransferOrder::~CTransferOrder()
{

}

CTransferOrder* CTransferOrder::operator =(CTransferOrder* TransferOrder)
{
	return (this);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsValidStepArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferOrder::IsValidStepArea(char* szArea)
{
	BOOL bRet = TRUE;
	for (int i = 0 ; i < m_nStepCount ; i++) {
		for (int j = 0 ; j < m_sOrderStep[i].nStepAreaCount ; j++) {
			if (0 == strcmp(szArea , m_sOrderStep[i].sOrderStepArea[j].szAreaName)) {
				bRet = FALSE;
				break;
			}
		}

		if (FALSE == bRet) break;
	}	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : AddOrderStepItem()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferOrder::AddOrderStepItem(int nStepNum , char* szAreaName)
{
	BOOL bRet = TRUE;
	if (NULL == szAreaName) return FALSE;
	if (TRUE == IsValidStepArea(szAreaName)) {
		strcpy(m_sOrderStep[nStepNum-1].sOrderStepArea[m_sOrderStep[nStepNum-1].nStepAreaCount].szAreaName , szAreaName);
		m_sOrderStep[nStepNum-1].nStepAreaCount++;
		if (nStepNum > m_nStepCount) m_nStepCount = nStepNum;
	} else {
		bRet = FALSE;
	}
	return bRet;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetStepOrder()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTransferOrder::GetStepOrder(char* szStart   , char* szLast , int& StartStep , int& LastStep)
{
	BOOL bRet = FALSE;
	int nStartStep = -1;
	int nLastStep  = -1;

	for (int i = 0 ; i < m_nStepCount; i++) {
		for (int j = 0 ; j < m_sOrderStep[i].nStepAreaCount ; j++) {
			if (0 == strcmp(szStart , m_sOrderStep[i].sOrderStepArea[j].szAreaName )) 
				nStartStep  = i;

			if (0 == strcmp(szLast   , m_sOrderStep[i].sOrderStepArea[j].szAreaName ))
				nLastStep   = i;
		}

		if (nStartStep >= 0  && nLastStep >= 0) {bRet = TRUE; break;}
	}

	if (TRUE == bRet) {
		StartStep = nStartStep+1;
		LastStep  = nLastStep+1;		
	}
	
	return bRet;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetNextStepOrder()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CTransferOrder::GetNextStepOrder(int   nCurrent  , sOrderStep& sNextOrder)
{
	sNextOrder.nStepAreaCount = m_sOrderStep[nCurrent-1].nStepAreaCount;	
	for (int i = 0 ; i < m_sOrderStep[nCurrent-1].nStepAreaCount; i++) {
		strcpy(sNextOrder.sOrderStepArea[i].szAreaName , m_sOrderStep[nCurrent-1].sOrderStepArea[i].szAreaName);
	}
}