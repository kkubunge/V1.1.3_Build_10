// MutexLock.h: interface for the CMutexLock class.
// Create : 2005.10.18
// Last Update : 2005.10.18
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTEXLOCK_H__40231B14_217A_4ABB_BD0D_213CB692162D__INCLUDED_)
#define AFX_MUTEXLOCK_H__40231B14_217A_4ABB_BD0D_213CB692162D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMutexLock  
{
public:
	CMutexLock();
	CMutexLock(const char *szMutexName /*= NULL*/, DWORD dwTimeout = INFINITE /*ms*/, bool bLock = true);
	virtual ~CMutexLock();

// Member Function
public:
	bool InitMutex(const char *szMutexName /*= NULL*/, DWORD dwTimeout = INFINITE /*ms*/, bool bLock = false);
	bool LockMutex();
	bool UnlockMutex();
	bool IsMutexValid();

// Member Variable
private:
	HANDLE m_hMutex;
	bool m_bMutexLock;
	DWORD m_dwTimeout;
};

#endif // !defined(AFX_MUTEXLOCK_H__40231B14_217A_4ABB_BD0D_213CB692162D__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2005.10.18	Created, Mutex의 생성, 초기화, 소멸, 활용을 용이하게
			하기위한 class
--------------------------------------------------------------------*/