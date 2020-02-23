// DataQueue.h: interface for the CDataQueue class.
// Create : 2006.07.24
// Last Update : 2006.08.07
// Writter : Changu, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAQUEUE_H__046F6533_A85D_44D2_8828_68E078B6DC19__INCLUDED_)
#define AFX_DATAQUEUE_H__046F6533_A85D_44D2_8828_68E078B6DC19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDataQueue  
{
public:
	CDataQueue();
	CDataQueue(int nElementSize, int nQueueSize = 512, bool bErrMsg = true);
	virtual ~CDataQueue();

// Member Function
public:
	void InitQueue(int nElementSize, int nQueueSize = 512, bool bErrMsg = true);
	void DeinitQueue();
	void ClearBuffer();
	bool Push(void *pData, int nSize = 0);
	bool Pop(void *pData);
	bool Read(void *pData, int nIndex = 0);
	bool IsEmpty();
	bool IsFull();
	int GetStoredDataSize();
	bool IsValid();
private:

// Member Variable
private:
	int m_nElementSize;
	int m_nQueueSize;
	int m_nQueueFirst;
	int m_nQueueLast;
	char *m_pBuffer;

	bool m_bErrMsg;
	CRITICAL_SECTION m_csLock;
};

#endif // !defined(AFX_DATAQUEUE_H__046F6533_A85D_44D2_8828_68E078B6DC19__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2006.07.24	Created
2006.08.07	ClearBuffer Command 추가
			Push Function에 Data Size를 지정할 수 있는 기능 추가
--------------------------------------------------------------------*/
