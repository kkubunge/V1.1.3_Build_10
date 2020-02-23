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
2005.08.31	C�� ������ Code�� Class�� ������ ����
2005.10.13	Update History ��� ����
			Shared Memory ���� �� ���� �߻��� ���� ������ Default Option
			���� ���� �ϵ��� ����
2005.11.22	MsgBoxDll.h �� Include ���� ���� ��� Message Box ���
			Compile ���� �߻����� �ʵ��� ����
--------------------------------------------------------------------*/
