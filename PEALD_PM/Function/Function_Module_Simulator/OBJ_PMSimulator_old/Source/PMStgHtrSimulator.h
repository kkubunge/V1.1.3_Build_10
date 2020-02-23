// PMStgHtrSimulator.h: interface for the CPMStgHtrSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMStgHtrSimulator_H__5792A902_80B2_43FC_AEB9_CFAE8DE5AAF7__INCLUDED_)
#define AFX_PMStgHtrSimulator_H__5792A902_80B2_43FC_AEB9_CFAE8DE5AAF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CPMStgHtrSimulator  : protected CDynamicIO, protected CMonThread
{
public:
	CPMStgHtrSimulator();
	virtual ~CPMStgHtrSimulator();

// Member Function
public:
	virtual BOOL Initialize();
protected:
	virtual void DoMonitor();

// Member Variable
public:
	bool m_bSimEnable;
	float m_fStgHtrRamp;
	float m_fStgHtrError;
};

#endif // !defined(AFX_PMStgHtrSimulator_H__5792A902_80B2_43FC_AEB9_CFAE8DE5AAF7__INCLUDED_)
