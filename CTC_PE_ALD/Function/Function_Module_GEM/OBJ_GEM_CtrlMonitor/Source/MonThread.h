// MonThread.h: interface for the CMonThread class.
// Create : 2004.04.09
// Last Update : 2006.03.21
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MonThread_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_)
#define AFX_MonThread_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef THRD_STS_IDLE
#define THRD_STS_IDLE			0
#define THRD_STS_RUN			1
#define THRD_STS_STOP			2
#define THRD_STS_FAILED			3
#endif

#define MAX_THREAD	30

//////////////////////////////////////////////////////////////////////
// Class Definition

class CMonThread  
{
public:
	CMonThread();
	virtual ~CMonThread();

	// Member Function
public:
	bool CreateThread(int nThreadNo = 0);
	bool WaitSec(int nThreadNo, int nSec);
	bool WaitSec(int nThreadNo, double dbSec);								// dbSec >= 0.1
	void DestroyThread(int nThreadNo = 0, int nWaitTime = 3000 /*(ms)*/ );
	void DestroyAllThread(int nWaitTime = 3000 /*(ms)*/ );
	int GetThreadStatus(int nThreadNo = 0);

	bool WaitSec(int nSec = 1) { return WaitSec(0, nSec); }					// Default ThreadNo = 0
	bool WaitSec(double dbSec) { return WaitSec(0, dbSec); }				// Default ThreadNo = 0
	bool WaitMillSec(int nMillSec = 100) { return WaitMillSec(0, nMillSec); } // Default ThreadNo = 0
	#define WAIT_SEC(_SEC)	WaitSec(nThreadNo, _SEC)						// for Multi Thread

	bool IsThreadStopping(int nThreadNo = 0);	// 2006.03.21

private:
	static DWORD WINAPI ThreadProc(LPVOID lParam);
	virtual bool WaitMillSec(int nThreadNo, int nMillSec);					// nMillSec >= 100

	// Virtual Function
protected:
	virtual void DoMonitor();
	virtual void DoMonitor(int nThreadNo);

	// Member Variable
public:
	char m_szLastErr[256];
private:
	HANDLE	m_phThread[MAX_THREAD];
	DWORD	m_pdwThreadId[MAX_THREAD];
	int		m_nThreadNo;
	int		m_pnThreadSts[MAX_THREAD];
};

#endif // !defined(AFX_MonThread_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2004.04.09	Created
2005.09.24	Update History ��� ����.
			ProcessThreadMessage() ��� �߰� Function�� ���ְ� ������ ����ȭ
			PostMessage() ��ɵ� ���ְ� ����ȭ.
2005.11.15	Wait ��� Function�� double type�� �ð����� ����� �� �ִ� Function
			�߰� : WaitSec(double dbSec)
2005.11.16	CreateThread() Function�� Thread�� �������� �ƴϸ� �ٽ� ȣ���ص�
			����ǵ��� ����.
2005.11.22	30������ Multi Thread ��� ����.
2005.12.16	Thread No�� ���� Monitor Function No ���Ǹ� Enumeration���� ����
			�ϵ��� �ϴ� Enum ���� �߰�.
2006.03.21	IsThreadStopping(...) Function �߰�
--------------------------------------------------------------------*/