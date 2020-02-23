// PMMfcSimulator.h: interface for the CPMMfcSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMMFCSIMULATOR_H__6793C261_8559_4796_A2BD_24AA17610F7C__INCLUDED_)
#define AFX_PMMFCSIMULATOR_H__6793C261_8559_4796_A2BD_24AA17610F7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CPMMfcSimulator  : protected CDynamicIO, protected CMonThread
{
public:
	CPMMfcSimulator();
	virtual ~CPMMfcSimulator();

// Member Function
public:
	virtual BOOL Initialize(double *pdbTargetPress);
	void SetMFCError(char *szParam);
	void ClearMFCError();
	void SetMfcFeedValve(int nMfcNo, int nValveIdx, const char *szFeedValveName);		// 2006.09.07
protected:
	virtual void DoMonitor();
private:
	BOOL GetAllUsedMFCHandle(int * pnUsedMFCHandle, int * pnSize);
	bool IsFeedValveOpened(int nMfcNo);		// 2006.09.07

// Member Variable
public:
	int m_pnMFCErr[_MFC_MAX_NO];
	bool m_bMFC_SimEnable;
private:
	double *m_pdbTargetPress;
};

#endif // !defined(AFX_PMMFCSIMULATOR_H__6793C261_8559_4796_A2BD_24AA17610F7C__INCLUDED_)


