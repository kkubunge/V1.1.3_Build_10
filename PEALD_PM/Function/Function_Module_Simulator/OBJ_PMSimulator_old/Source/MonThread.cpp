// MonThread.cpp: implementation of the CMonThread class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "MonThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonThread::CMonThread()
{
	m_hThread = NULL;
	m_dwThreadId = NULL;
	m_nThreadSts = THRD_STS_IDLE;

	m_hThread_B = NULL;
	m_dwThreadId_B = NULL;
	m_nThreadSts_B = THRD_STS_IDLE;
}

CMonThread::~CMonThread()
{
	if(m_hThread != NULL) DestroyThread();
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CMonThread::CreateThread()
{
	if(m_hThread == NULL)
	{
		m_nThreadSts = THRD_STS_RUN;
		m_hThread = ::CreateThread(0, 0, ThreadProc, this, 0, &m_dwThreadId);
	}
}

void CMonThread::CreateThread_B()
{
	if(m_hThread_B == NULL)
	{
		m_nThreadSts_B = THRD_STS_RUN;
		m_hThread_B = ::CreateThread(0, 0, ThreadProc_B, this, 0, &m_dwThreadId_B);
	}
}

void CMonThread::DestroyThread(int nWaitTime)
{
	if(m_nThreadSts == THRD_STS_RUN) m_nThreadSts = THRD_STS_STOP;
	if(m_nThreadSts_B == THRD_STS_RUN) m_nThreadSts_B = THRD_STS_STOP;

	if(m_hThread != NULL)
	{
		if(m_nThreadSts != THRD_STS_IDLE && nWaitTime > 0)
						WaitForSingleObject(m_hThread, nWaitTime);
		CloseHandle(m_hThread);
		m_hThread = NULL;
		m_dwThreadId = NULL;
	}
	if(m_hThread_B != NULL)
	{
		if(m_nThreadSts_B != THRD_STS_IDLE && nWaitTime > 0)
						WaitForSingleObject(m_hThread_B, nWaitTime);
		CloseHandle(m_hThread_B);
		m_hThread_B = NULL;
		m_dwThreadId_B = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Member Function (Thread)

DWORD WINAPI CMonThread::ThreadProc(LPVOID lParam)
{
	CMonThread *pBaseApp = (CMonThread *)lParam;
	pBaseApp->DoMonitor();
	pBaseApp->m_nThreadSts = THRD_STS_IDLE;
	return 0;
}

void CMonThread::DoMonitor()
{
	while(WaitSec(1))
	{
		// Monitor Code
	}
}

bool CMonThread::WaitSec(int nSec)
{
	int i;
	for(i=0; i<nSec*2 && m_nThreadSts == THRD_STS_RUN; i++) Sleep(500);
	if(m_nThreadSts != THRD_STS_RUN) return false;
	return true;
}

bool CMonThread::WaitSec(double dbSec)
{
	return WaitMillSec((int) (dbSec*1000));
}

bool CMonThread::WaitMillSec(int nMillSec)
{
	int i;
	for(i=0; i<nMillSec && m_nThreadSts == THRD_STS_RUN; i+= 100) Sleep(100);
	if(m_nThreadSts != THRD_STS_RUN) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Member Function (Thread B)

DWORD WINAPI CMonThread::ThreadProc_B(LPVOID lParam)
{
	CMonThread *pBaseApp = (CMonThread *)lParam;
	pBaseApp->DoMonitor_B();
	pBaseApp->m_nThreadSts_B = THRD_STS_IDLE;
	return 0;
}

void CMonThread::DoMonitor_B()
{
	while(WaitSec_B(1))
	{
		// Monitor Code
	}
}

bool CMonThread::WaitSec_B(int nSec)
{
	int i;
	for(i=0; i<nSec*2 && m_nThreadSts_B == THRD_STS_RUN; i++) Sleep(500);
	if(m_nThreadSts_B != THRD_STS_RUN) return false;
	return true;
}

bool CMonThread::WaitSec_B(double dbSec)
{
	return WaitMillSec_B((int) (dbSec*1000));
}

bool CMonThread::WaitMillSec_B(int nMillSec)
{
	int i;
	for(i=0; i<nMillSec && m_nThreadSts_B == THRD_STS_RUN; i+= 100) Sleep(100);
	if(m_nThreadSts_B != THRD_STS_RUN) return false;
	return true;
}
