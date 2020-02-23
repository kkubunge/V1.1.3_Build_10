// PrevMaintMonitor.h: interface for the CPrevMaintMonitor class.
// 2006.02.20
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREVMAINTMONITOR_H__652E08F0_99C8_4F38_91F9_9FA4757026A6__INCLUDED_)
#define AFX_PREVMAINTMONITOR_H__652E08F0_99C8_4F38_91F9_9FA4757026A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
#include "MonThread.h"

enum { PR_IDLE, PR_RESERVE };

class CPrevMaintMonitor  : protected CDynamicIO, CMonThread
{
public:
	CPrevMaintMonitor();
	virtual ~CPrevMaintMonitor();

public:
	void Initialize(int nPMNumber);
	void UpdatePM();
	void ReservePostRun(int nPRState /*= PR_IDLE or PR_RESERVE*/,int nType );
	void ResetPostRun(int nType);
	BOOL IsPostRunReserved(int nType);

protected:
	virtual void DoMonitor();

private:
	int m_nPMNumber;
	CRITICAL_SECTION m_csLock;
};

#endif // !defined(AFX_PREVMAINTMONITOR_H__652E08F0_99C8_4F38_91F9_9FA4757026A6__INCLUDED_)
