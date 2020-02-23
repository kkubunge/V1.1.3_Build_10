// FuncThread.h: interface for the CFuncThread class.
// Create : 2004.04.09
// Last Update : 2006.11.17
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTHREAD_H__2B26673C_8F21_42A6_83BE_B06E5378BD6A__INCLUDED_)
#define AFX_FUNCTHREAD_H__2B26673C_8F21_42A6_83BE_B06E5378BD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Class Definition

class CFuncThread  
{
public:
	CFuncThread(bool bThreadAutoCreate = true);
	virtual ~CFuncThread();

	// Member Function
public:
	void CreateFuncThread();
	void DestroyFuncThread();
	BOOL CallThreadFunc(int nFuncEnum, int nParam = 0); 
	BOOL StartThreadFunc(WPARAM wParam, LPARAM lParam);
	void StopThreadFunc();
	BOOL PostMessage(UINT unMsg, WPARAM wParam, LPARAM lParam);

protected:
	static DWORD WINAPI ThreadProc(LPVOID lParam);

	// Virtual Function
protected:
	enum {
		FT_MSG_TERMINATE = WM_USER + 2,
		FT_MSG_FUNCTION
	};
	virtual void ProcessThreadMessage();
	virtual void DoFunction(WPARAM wParam, LPARAM lParam);

	// Member Variable
protected:
	HANDLE	m_hFuncThread;
	DWORD	m_dwFuncThreadId;
	bool	m_bThreadFuncRun;
};

#endif // !defined(AFX_FUNCTHREAD_H__2B26673C_8F21_42A6_83BE_B06E5378BD6A__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2004.04.09	Created
2005.10.11	Update History ��� ����
			StartThreadFunc(...) �߰�, Function����� ���� ���� �ۼ���
			�� �ֵ����ϴ� Interface
2006.11.17	�������� Function�� ���� Enum���� �����Ͽ� ����� �� �ֵ��� ��.
			CallThreadFunc(int nFuncEnum) �߰� 
--------------------------------------------------------------------*/
