// SharedMemory.h: interface for the CSharedMemory class.
// Create : 2005.08.31
// Last Update : 2005.11.22
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREDMEMORY_H__AC01ED11_787E_4DC6_B4A8_64AA86C2DFBE__INCLUDED_)
#define AFX_SHAREDMEMORY_H__AC01ED11_787E_4DC6_B4A8_64AA86C2DFBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSharedMemory  
{
public:
	CSharedMemory();
	virtual ~CSharedMemory();

// Member Function
public:
	bool CreateSM(char *szFileName, int nSM_Size, bool bErrMsgBox = true);
	bool OpenSM(char *szFileName, int nSM_Size, bool bErrMsgBox = true);
	bool CloseSM(bool bForced = true);
	bool FreeSM(bool bForced = true);
	bool CreateSMBuffer();
	void FreeSMBuffer();
	bool ReadFromSM();
	bool WriteToSM();

	bool ReadFromSM(int nIndex, char *pData, int nSize);
	bool WriteToSM(int nIndex, char *pData, int nSize);

// Member Variable
public:
	char m_szFileName[256];
	char m_szErrDesc[256];
	int m_nSM_Size;
	char *m_pSM_Data;

private:
	HANDLE m_hFileHandle;
	LPVOID m_pViewOfFile;
	bool m_bErrMsgBox;
};

#endif // !defined(AFX_SHAREDMEMORY_H__AC01ED11_787E_4DC6_B4A8_64AA86C2DFBE__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2005.04.20	Created
2005.08.31	C로 구성된 Code를 Class로 구조로 변경
2005.10.13	Update History 기록 시작
			Shared Memory 해제 중 문제 발생시 강제 해제를 Default Option
			으로 동작 하도록 수정
2005.11.22	MsgBoxDll.h 를 Include 하지 못할 경우 Message Box 기능
			Compile 오류 발생하지 않도록 수정
--------------------------------------------------------------------*/
