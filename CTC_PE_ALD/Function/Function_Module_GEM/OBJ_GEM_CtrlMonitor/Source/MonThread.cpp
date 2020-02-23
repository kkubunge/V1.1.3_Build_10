// MonThread.cpp: implementation of the CMonThread class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "MonThread.h"

// #include "MsgBoxDll.h"
#ifdef _MSGBOXDLL_H_
#define _ERRMSG(_SZ_MSG)	_gOutMsgBox("CSharedMemory", _SZ_MSG, FALSE)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define NOT_DEFINED_THREAD_NO		-1
#define THREAD_CREATION_TIMEOUT		3	/*sec*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonThread::CMonThread()
{
	int i;
	for(i=0; i<MAX_THREAD; i++)
	{
		m_phThread[i] = NULL;
		m_pdwThreadId[i] = NULL;
		m_pnThreadSts[i] = THRD_STS_IDLE;
	}
	m_nThreadNo = NOT_DEFINED_THREAD_NO;
	m_szLastErr[0] = 0;
}

CMonThread::~CMonThread()
{
	DestroyAllThread();
}

//////////////////////////////////////////////////////////////////////
// Member Function

bool CMonThread::CreateThread(int nThreadNo)
{
	if(nThreadNo<0 || nThreadNo>=MAX_THREAD)
	{
		sprintf(m_szLastErr, "ThreadNo[%d] Is Invalid !", nThreadNo);
		#ifdef _MSGBOXDLL_H_
		_ERRMSG(m_szLastErr);
		#endif
		return false;
	}

	int i;
	for(i=0; m_nThreadNo != NOT_DEFINED_THREAD_NO && i<THREAD_CREATION_TIMEOUT*1000; i+=10) Sleep(10);
	if(i >= THREAD_CREATION_TIMEOUT*1000)
	{
		sprintf(m_szLastErr, "Thread Creation Timeout(Creation Lock is not released) !");
		#ifdef _MSGBOXDLL_H_
		_ERRMSG(m_szLastErr);
		#endif
		return false;
	}

	if(m_phThread[nThreadNo] == NULL || m_pnThreadSts[nThreadNo] != THRD_STS_RUN)
	{
		m_nThreadNo = nThreadNo;
		m_pnThreadSts[nThreadNo] = THRD_STS_RUN;
		m_phThread[nThreadNo] = ::CreateThread(0, 0, ThreadProc, this, 0, &m_pdwThreadId[nThreadNo]);
		if(m_phThread[nThreadNo] == NULL)
		{
			m_pnThreadSts[nThreadNo] = THRD_STS_FAILED;
			m_nThreadNo = NOT_DEFINED_THREAD_NO;
			sprintf(m_szLastErr, "CreateThread Low Function Return Is NULL !");
			#ifdef _MSGBOXDLL_H_
			_ERRMSG(m_szLastErr);
			#endif
			return false;
		}
	}
	return true;
}

void CMonThread::DestroyThread(int nThreadNo, int nWaitTime)
{
	if(m_pnThreadSts[nThreadNo] == THRD_STS_RUN) m_pnThreadSts[nThreadNo] = THRD_STS_STOP;

	if(m_phThread[nThreadNo] != NULL)
	{
		if(m_pnThreadSts[nThreadNo] != THRD_STS_IDLE && nWaitTime > 0)
						WaitForSingleObject(m_phThread[nThreadNo], nWaitTime);
		CloseHandle(m_phThread[nThreadNo]);
		m_phThread[nThreadNo] = NULL;
		m_pdwThreadId[nThreadNo] = NULL;
	}
}

void CMonThread::DestroyAllThread(int nWaitTime)
{
	int i;
	for(i=0; i<MAX_THREAD; i++) DestroyThread(i, nWaitTime);
}

int CMonThread::GetThreadStatus(int nThreadNo)
{
	return m_pnThreadSts[nThreadNo];
}

//////////////////////////////////////////////////////////////////////
// Member Function (Thread)

DWORD WINAPI CMonThread::ThreadProc(LPVOID lParam)
{
	CMonThread *pBaseApp = (CMonThread *)lParam;
	int nThreadNo;
	nThreadNo = pBaseApp->m_nThreadNo;
	pBaseApp->m_nThreadNo = NOT_DEFINED_THREAD_NO;
	pBaseApp->DoMonitor(nThreadNo);
	pBaseApp->m_pnThreadSts[nThreadNo] = THRD_STS_IDLE;
	return 0;
}

void CMonThread::DoMonitor()	/* Default Monitor Function */
{
	while(WaitSec())
	{
		// Monitor Code
	}
}

void CMonThread::DoMonitor(int nThreadNo)
{
	switch(nThreadNo)
	{
	case 0: DoMonitor(); break;
	case 1: /* Function_1(nThreadNo); */ break;
	case 2: /* Function_2(nThreadNo); */ break;
	}
}

bool CMonThread::WaitSec(int nThreadNo, int nSec)
{
	int i;
	for(i=0; i<nSec*2 && m_pnThreadSts[nThreadNo] == THRD_STS_RUN; i++) Sleep(500);
	if(m_pnThreadSts[nThreadNo] != THRD_STS_RUN) return false;
	return true;
}

bool CMonThread::WaitSec(int nThreadNo, double dbSec)
{
	return WaitMillSec(nThreadNo, (int) (dbSec*1000));
}

bool CMonThread::WaitMillSec(int nThreadNo, int nMillSec)
{
	int i;
	for(i=0; i<nMillSec && m_pnThreadSts[nThreadNo] == THRD_STS_RUN; i+= 100) Sleep(100);
	if(m_pnThreadSts[nThreadNo] != THRD_STS_RUN) return false;
	return true;
}

bool CMonThread::IsThreadStopping(int nThreadNo)
{
	if(m_pnThreadSts[nThreadNo] != THRD_STS_RUN) return true;
	return false;
}
