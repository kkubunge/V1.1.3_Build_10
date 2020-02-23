// MutexLock.cpp: implementation of the CMutexLock class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "MutexLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMutexLock::CMutexLock()
{
	m_bMutexLock = false;
	m_dwTimeout = INFINITE;
	m_hMutex = NULL;
}

CMutexLock::CMutexLock(const char *szMutexName, DWORD dwTimeout, bool bLock)
{
	m_bMutexLock = false;
	m_dwTimeout = dwTimeout;
	m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
	if(bLock) LockMutex();
}

CMutexLock::~CMutexLock()
{
	UnlockMutex();
	if(m_hMutex != NULL) CloseHandle(m_hMutex);
}

//////////////////////////////////////////////////////////////////////
// Member Variable

bool CMutexLock::InitMutex(const char *szMutexName, DWORD dwTimeout, bool bLock)
{
	bool bRet = false;
	do {
		if(m_hMutex != NULL) break;
		m_dwTimeout = dwTimeout;
		m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
		if(m_hMutex == NULL) break;
		if(bLock) LockMutex();
		bRet = true;
	} while(0);
	return bRet;
}

bool CMutexLock::LockMutex()
{
	if(m_hMutex == NULL) return true;
	if(! m_bMutexLock)
	{
		if(WAIT_OBJECT_0 == WaitForSingleObject(m_hMutex, m_dwTimeout)) m_bMutexLock = true;
	}
	return m_bMutexLock;
}

bool CMutexLock::UnlockMutex()
{
	if(m_hMutex == NULL) return true;
	if(m_bMutexLock)
	{
		if(ReleaseMutex(m_hMutex)) m_bMutexLock = false;
	}
	return (! m_bMutexLock);
}

bool CMutexLock::IsMutexValid()
{
	if(m_hMutex == NULL) return false;
	return true;
}
