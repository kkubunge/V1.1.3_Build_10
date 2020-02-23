// MonThread.h: interface for the CMonThread class.
// Create : 2004.04.09
// Last Update : 2005.11.15
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONTHREAD_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_)
#define AFX_MONTHREAD_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef THRD_STS_IDLE
#define THRD_STS_IDLE			0
#define THRD_STS_RUN			1
#define THRD_STS_STOP			2
#define THRD_STS_FAILED			3
#endif

//////////////////////////////////////////////////////////////////////
// Class Definition

class CMonThread  
{
public:
	CMonThread();
	virtual ~CMonThread();

	// Member Function
public:
	void CreateThread();
	bool WaitSec(int nSec);
	bool WaitSec(double dbSec);							// dbSec >= 0.1

	void CreateThread_B();
	bool WaitSec_B(int nSec);
	bool WaitSec_B(double dbSec);						// dbSec >= 0.1

	void DestroyThread(int nWaitTime = 3000 /*(ms)*/ );

private:
	static DWORD WINAPI ThreadProc(LPVOID lParam);
	static DWORD WINAPI ThreadProc_B(LPVOID lParam);

	// Virtual Function
protected:
	virtual void DoMonitor();
	virtual bool WaitMillSec(int nMillSec = 100);		// nMillSec >= 100
	virtual void DoMonitor_B();
	virtual bool WaitMillSec_B(int nMillSec = 100);		// nMillSec >= 100

	// Member Variable
private:
	HANDLE	m_hThread;
	DWORD	m_dwThreadId;
	HANDLE	m_hThread_B;
	DWORD	m_dwThreadId_B;
protected:
	int		m_nThreadSts;
	int		m_nThreadSts_B;
};

#endif // !defined(AFX_MONTHREAD_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_)
