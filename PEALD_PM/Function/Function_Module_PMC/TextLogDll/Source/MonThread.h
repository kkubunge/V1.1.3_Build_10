// MonThread.h: interface for the CMonThread class.
// Version : 1.0
// Create : 2004.04.09
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONTHREAD_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_)
#define AFX_MONTHREAD_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Class Definition

class CMonThread  
{
public:
	CMonThread();
	virtual ~CMonThread();

	// Member Function
public:
	void CreateThrd();
	void DestroyThrd();
	BOOL PostThrdMsg(UINT unMsg, WPARAM wParam, LPARAM lParam);

protected:
	static DWORD WINAPI ThreadProc(LPVOID lParam);

	// Virtual Function
protected:
	virtual void ProcessThreadMessage();
	virtual void DoMonitor();

	// Member Variable
protected:
	HANDLE	m_hThread;
	DWORD	m_dwThreadId;
	bool	m_bThreadRun;
};

#endif // !defined(AFX_MONTHREAD_H__45C1DBC5_C58A_4E50_A111_AC6C1ECC7F9D__INCLUDED_)
