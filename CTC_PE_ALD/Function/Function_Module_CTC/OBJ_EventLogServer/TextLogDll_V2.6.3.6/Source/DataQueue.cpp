// DataQueue.cpp: implementation of the CDataQueue class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "DataQueue.h"
#include <stdio.h>

#include "MsgBoxDll.h"
#define _ERRMSG(_SZ_MSG)	_OUTMSG(_SZ_MSG)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataQueue::CDataQueue()
{
	_gINIT_MSGBOX("CDataQueue");
	m_bErrMsg = false;
	m_nQueueFirst = 0;
	m_nQueueLast = 0;
	m_nElementSize = 0;
	m_nQueueSize = 0;
	m_pBuffer = NULL;
	InitializeCriticalSection(&m_csLock);
}

CDataQueue::CDataQueue(int nElementSize, int nQueueSize, bool bErrMsg)
{
	_gINIT_MSGBOX("CDataQueue");
	InitializeCriticalSection(&m_csLock);
	InitQueue(nElementSize, nQueueSize, bErrMsg);
}

CDataQueue::~CDataQueue()
{
	DeinitQueue();
	DeleteCriticalSection(&m_csLock);
	_gDEINIT_MSGBOX();
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CDataQueue::InitQueue(int nElementSize, int nQueueSize, bool bErrMsg)
{
	int nBufferSize;
	EnterCriticalSection(&m_csLock);
	m_nQueueFirst = 0;
	m_nQueueLast = 0;
	m_nElementSize = nElementSize;
	m_nQueueSize = nQueueSize;
	nBufferSize = nElementSize * nQueueSize;
	m_pBuffer = new char[nBufferSize];
	m_bErrMsg = bErrMsg;
	LeaveCriticalSection(&m_csLock);
}

void CDataQueue::DeinitQueue()
{
	EnterCriticalSection(&m_csLock);
	if(m_pBuffer != NULL)
	{
		m_nQueueFirst = 0;
		m_nQueueLast = 0;
		m_nElementSize = 0;
		m_nQueueSize = 0;
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	LeaveCriticalSection(&m_csLock);
}

void CDataQueue::ClearBuffer()
{
	EnterCriticalSection(&m_csLock);
	m_nQueueFirst = 0;
	m_nQueueLast = 0;
	LeaveCriticalSection(&m_csLock);
}

bool CDataQueue::Push(void *pData, int nSize)
{
	bool bRet = false;
	int nLimit;
	int nIdx;
	if(m_pBuffer == NULL)
	{
		if(m_bErrMsg) _ERRMSG("Not Initialized");
		return false;
	}
	EnterCriticalSection(&m_csLock);
	do {
		if(m_pBuffer == NULL) break;
		nLimit = m_nQueueLast - 1;
		if(nLimit < 0) nLimit = m_nQueueSize - 1;
		if(m_nQueueFirst == nLimit)
		{
			if(m_bErrMsg) _ERRMSG("Buffer Is Overflow");
			break;
		}

		nIdx = m_nElementSize * m_nQueueFirst;
		if(nSize <= 0 || nSize > m_nElementSize) nSize = m_nElementSize;	// 2006.08.07
		memcpy(m_pBuffer+nIdx, pData, nSize);
		m_nQueueFirst++;
		if(m_nQueueFirst >= m_nQueueSize) m_nQueueFirst = 0;
		bRet = true;
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

bool CDataQueue::Pop(void *pData)
{
	bool bRet = false;
	int nIdx;
	if(m_pBuffer == NULL)
	{
		if(m_bErrMsg) _ERRMSG("Not Initialized");
		return false;
	}
	EnterCriticalSection(&m_csLock);
	do {
		if(m_pBuffer == NULL) break;
		if(m_nQueueFirst == m_nQueueLast) break;
		nIdx = m_nElementSize * m_nQueueLast;
		memcpy(pData, m_pBuffer+nIdx, m_nElementSize);
		m_nQueueLast++;
		if(m_nQueueLast >= m_nQueueSize) m_nQueueLast = 0;
		bRet = true;
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

bool CDataQueue::Read(void *pData, int nIndex)
{
	bool bRet = false;
	int nIdx;
	if(m_pBuffer == NULL)
	{
		if(m_bErrMsg) _ERRMSG("Not Initialized");
		return false;
	}
	EnterCriticalSection(&m_csLock);
	do {
		if(m_pBuffer == NULL) break;
		if(nIndex < 0 || nIndex >= GetStoredDataSize()) break;

		nIdx = m_nQueueLast+nIndex;
		if(nIdx >= m_nQueueSize) nIdx -= m_nQueueSize;
		nIdx *= m_nElementSize;
		memcpy(pData, m_pBuffer+nIdx, m_nElementSize);
		bRet = true;
	} while(0);
	LeaveCriticalSection(&m_csLock);
	return bRet;
}

bool CDataQueue::IsEmpty()
{
	if(m_nQueueFirst == m_nQueueLast) return true;
	return false;
}

bool CDataQueue::IsFull()
{
	int nLimit;
	nLimit = m_nQueueLast - 1;
	if(nLimit < 0) nLimit = m_nQueueSize - 1;
	if(m_nQueueFirst == nLimit) return true;
	return false;
}

bool CDataQueue::IsValid()
{
	if(m_pBuffer == NULL) return false;
	return true;
}

int CDataQueue::GetStoredDataSize()
{
	int nSize;
	if(m_pBuffer == NULL) return 0;
	nSize = m_nQueueFirst - m_nQueueLast;
	if(nSize < 0) nSize += m_nQueueSize;
	return nSize;
}