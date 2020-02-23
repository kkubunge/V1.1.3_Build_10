// FuncThread.cpp: implementation of the CFuncThread class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "FuncThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
// Thread Function Enum
enum {
	TF_Function1,
	TF_Function2,
	TF_Function3
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuncThread::CFuncThread(bool bThreadAutoCreate)
{
	m_hFuncThread = NULL;
	m_dwFuncThreadId = NULL;
	if(bThreadAutoCreate) CreateFuncThread();
}

CFuncThread::~CFuncThread()
{
	if(m_hFuncThread != NULL) DestroyFuncThread();
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CFuncThread::CreateFuncThread()
{
	if(m_hFuncThread == NULL)
	{
		m_bThreadFuncRun = false;
		m_hFuncThread = CreateThread(0, 0, ThreadProc, this, 0, &m_dwFuncThreadId);
	}
}

void CFuncThread::DestroyFuncThread()
{
	StopThreadFunc();
	if(m_hFuncThread != NULL)
	{
		PostMessage(FT_MSG_TERMINATE, 0, 0);
		WaitForSingleObject(m_hFuncThread, 1000);
		CloseHandle(m_hFuncThread);
		m_hFuncThread = NULL;
		m_dwFuncThreadId = NULL;
	}
}

BOOL CFuncThread::CallThreadFunc(int nFuncEnum, int nParam)
{
	WPARAM wParam;
	LPARAM lParam;
	wParam = nFuncEnum;
	lParam = nParam;
	return StartThreadFunc(wParam, lParam);
}

BOOL CFuncThread::StartThreadFunc(WPARAM wParam, LPARAM lParam)
{
	return PostMessage(FT_MSG_FUNCTION, wParam, lParam);
}

void CFuncThread::StopThreadFunc()
{
	m_bThreadFuncRun = false;
}

BOOL CFuncThread::PostMessage(UINT unMsg, WPARAM wParam, LPARAM lParam)
{
	return PostThreadMessage(m_dwFuncThreadId, unMsg, wParam, lParam);
}

DWORD WINAPI CFuncThread::ThreadProc(LPVOID lParam)
{
	CFuncThread *pBaseApp = (CFuncThread *)lParam;
	pBaseApp->ProcessThreadMessage();
	return 0;
}

void CFuncThread::ProcessThreadMessage()
{
	bool bThreadRun = true;
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	while(bThreadRun)
	{
		GetMessage(&msg, NULL, 0, 0);
		switch(msg.message)
		{
		case FT_MSG_TERMINATE:
			bThreadRun = false; break;
		case FT_MSG_FUNCTION:
			DoFunction(msg.wParam, msg.lParam); break;
		}
	}
}

void CFuncThread::DoFunction(WPARAM wParam, LPARAM lParam)
{
	m_bThreadFuncRun = true;
	do {
		if(! m_bThreadFuncRun) break;
		switch(wParam)
		{
		case TF_Function1: /*Function1 Code*/ break;
		case TF_Function2: /*Function2 Code*/ break;
		case TF_Function3: /*Function3 Code*/ break;
		}
		Sleep(1000);
	} while(0);
	m_bThreadFuncRun = false;
}
