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

enum {
	MT_MSG_TERMINATE = WM_USER + 2
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonThread::CMonThread()
{
	m_hThread = NULL;
	m_dwThreadId = NULL;
	m_bThreadRun = false;
}

CMonThread::~CMonThread()
{
	if(m_hThread != NULL) DestroyThrd();
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CMonThread::CreateThrd()
{
	if(m_hThread == NULL)
		m_hThread = CreateThread(0, 0, ThreadProc, this, 0, &m_dwThreadId);
}

void CMonThread::DestroyThrd()
{
	m_bThreadRun = false;
	if(m_hThread != NULL)
	{
		PostThreadMessage(m_dwThreadId, MT_MSG_TERMINATE, 0, 0);
		WaitForSingleObject(m_hThread, 3000);
		CloseHandle(m_hThread);
		m_hThread = NULL;
		m_dwThreadId = NULL;
	}
}

BOOL CMonThread::PostThrdMsg(UINT unMsg, WPARAM wParam, LPARAM lParam)
{
	return PostThreadMessage(m_dwThreadId, unMsg, wParam, lParam);
}

DWORD WINAPI CMonThread::ThreadProc(LPVOID lParam)
{
	CMonThread *pBaseApp = (CMonThread *)lParam;
	pBaseApp->ProcessThreadMessage();
	return 0;
}

void CMonThread::ProcessThreadMessage()
{
	m_bThreadRun = true;
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

	while(m_bThreadRun)
	{
		if(TRUE == PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch(msg.message)
			{
			case MT_MSG_TERMINATE:
				m_bThreadRun = false; break;
			}
		}
		else
		{
			if(m_bThreadRun == false) break;
			DoMonitor();
			if(m_bThreadRun == false) break;
			Sleep(1000);
		}
	}
}

void CMonThread::DoMonitor()
{
	// Monitor Code
}
