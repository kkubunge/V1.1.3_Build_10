// TransferOrder.h: interface for the CTransferOrder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSFERORDER_H__5CE5246E_E91B_4EB1_8FD6_2F933E488038__INCLUDED_)
#define AFX_TRANSFERORDER_H__5CE5246E_E91B_4EB1_8FD6_2F933E488038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ORDER_STEP_COUNT	10
#define MAX_ORDER_STEP_AREA		10

struct sAreaName
{
	 sAreaName() {memset(szAreaName , 0 , sizeof(szAreaName));};
	~sAreaName() {};

	char szAreaName[32];
};

struct sOrderStep
{
	 sOrderStep() {
		 for (int i = 0 ; i < MAX_ORDER_STEP_AREA; i++) {memset(sOrderStepArea[i].szAreaName , 0 , 32);}
		 nStepAreaCount = 0;
	 };
	~sOrderStep() {};
	sAreaName sOrderStepArea[MAX_ORDER_STEP_AREA];
	int       nStepAreaCount;
};

class CTransferOrder  
{
public:
	CTransferOrder();
	virtual ~CTransferOrder();

	BOOL AddOrderStepItem(int nStepNum    , char* szAreaName);
	BOOL GetStepOrder    (char* szStart   , char* szLast , int& StartStep , int& LastStep);
	void GetNextStepOrder(int   nCurrent  , sOrderStep& sNextOrder);

	CTransferOrder* operator= (CTransferOrder* TransferOrder);

private:
	BOOL IsValidStepArea(char* szArea);

private:
	sOrderStep m_sOrderStep[MAX_ORDER_STEP_COUNT];
	int        m_nStepCount;

};

#endif // !defined(AFX_TRANSFERORDER_H__5CE5246E_E91B_4EB1_8FD6_2F933E488038__INCLUDED_)
