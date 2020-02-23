// PMHeaterSimulator.h: interface for the CPMHeaterSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMHEATERSIMULATOR_H__67263247_3744_479E_8E37_B3329D550B74__INCLUDED_)
#define AFX_PMHEATERSIMULATOR_H__67263247_3744_479E_8E37_B3329D550B74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CPMHeaterSimulator  : protected CDynamicIO, protected CMonThread
{
public:
	CPMHeaterSimulator();
	virtual ~CPMHeaterSimulator();

// Member Function
public:
	virtual BOOL Initialize();
protected:
	virtual void DoMonitor();
private:
	BOOL GetAllUsedHeaterHandle(int * pnUsedHeaterHandle, int * pnSize);

// Member Variable
public:
	// 2004.05.12 by cskim
	bool m_bHtr_SimEnable;
};

#endif // !defined(AFX_PMHEATERSIMULATOR_H__67263247_3744_479E_8E37_B3329D550B74__INCLUDED_)

