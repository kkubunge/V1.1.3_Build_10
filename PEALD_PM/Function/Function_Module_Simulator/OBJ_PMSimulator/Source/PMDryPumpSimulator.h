// PMDryPumpSimulator.h: interface for the CPMDryPumpSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMDRYPUMPSIMULATOR_H__A6D1284F_ECEC_400C_9F4C_9CDE6ABD3043__INCLUDED_)
#define AFX_PMDRYPUMPSIMULATOR_H__A6D1284F_ECEC_400C_9F4C_9CDE6ABD3043__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CPMDryPumpSimulator  : protected CDynamicIO, protected CMonThread
{
public:
	CPMDryPumpSimulator();
	virtual ~CPMDryPumpSimulator();

// Member Function
public:
	virtual BOOL Initialize();
protected:
	virtual void DoMonitor();
};

#endif // !defined(AFX_PMDRYPUMPSIMULATOR_H__A6D1284F_ECEC_400C_9F4C_9CDE6ABD3043__INCLUDED_)
