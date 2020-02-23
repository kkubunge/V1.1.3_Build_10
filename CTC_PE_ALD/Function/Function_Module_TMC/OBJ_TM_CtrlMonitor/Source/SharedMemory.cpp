// SharedMemory.cpp: implementation of the CSharedMemory class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
// #include <afxwin.h>
#include <windows.h>
#include "SharedMemory.h"

// #include "MsgBoxDll.h"
#ifdef _MSGBOXDLL_H_
#define _ERRMSG(_SZ_MSG)	_gOutMsgBox("CSharedMemory", _SZ_MSG, FALSE)
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSharedMemory::CSharedMemory()
{
	m_bErrMsgBox = true;

	m_hFileHandle = NULL;
	m_pViewOfFile = NULL;
	strcpy(m_szErrDesc, "");
	unsigned char *m_pSM_DATA = NULL;

	strcpy(m_szFileName, "NOT_DEFINED");
	m_nSM_Size = 0;
	m_pSM_Data = NULL;
}

CSharedMemory::~CSharedMemory()
{
	if(m_hFileHandle != NULL) FreeSM(true);
	if(m_pSM_Data != NULL) FreeSMBuffer();
	// _gFreeMsgBoxDll();
}

//////////////////////////////////////////////////////////////////////
// Member Function

bool CSharedMemory::CreateSM(char *szFileName, int nSM_Size, bool bErrMsgBox)
{
	// char szLog[256];
	strcpy(m_szFileName, szFileName);
	m_nSM_Size = nSM_Size;
	m_bErrMsgBox = bErrMsgBox;

	if(m_hFileHandle != NULL)
	{
		sprintf(m_szErrDesc, "FileMapping is already opened");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	m_hFileHandle = CreateFileMapping(	INVALID_HANDLE_VALUE,               // handle to file
										NULL,								// security
										PAGE_READWRITE,                     // protection
										0,						            // high-order DWORD of size
										m_nSM_Size,				            // low-order DWORD of size
										m_szFileName	  	                    // object name
									 );

	if (NULL == m_hFileHandle)
	{
		sprintf(m_szErrDesc, "Unable to create file mapping. Error is %d", GetLastError());
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	m_pViewOfFile = MapViewOfFile(	m_hFileHandle,							// handle to file-mapping object
									FILE_MAP_ALL_ACCESS,					// access mode
									0,										// high-order DWORD of offset
									0,										// low-order DWORD of offset
									0										// number of bytes to map
								);
	if (NULL == m_pViewOfFile)
	{
		sprintf(m_szErrDesc, "Unable to map to the current address space. Error is %d", GetLastError());
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	// sprintf(szLog, "-----> Create Shared Memory Successfully");
	// _LOG(szLog);
	return true;
}

bool CSharedMemory::CreateSMBuffer()
{
	if(m_nSM_Size > 0) m_pSM_Data = new char[m_nSM_Size];
	return true;
}

void CSharedMemory::FreeSMBuffer()
{
	if(m_pSM_Data != 0)	delete m_pSM_Data;
	m_pSM_Data = NULL;
}

bool CSharedMemory::OpenSM(char *szFileName, int nSM_Size, bool bErrMsgBox)
{
	bool bRet = true;
	strcpy(m_szFileName, szFileName);
	m_nSM_Size = nSM_Size;
	m_bErrMsgBox = bErrMsgBox;

	if(m_hFileHandle != NULL)
	{
		sprintf(m_szErrDesc, "FileMapping is already opened");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		return false;
	}

	m_hFileHandle = OpenFileMapping(	FILE_MAP_WRITE,                     // protection
										(int) NULL,
										szFileName	  	                   // object name
									 );

	if (NULL == m_hFileHandle)
	{
		sprintf(m_szErrDesc, "Unable to open file mapping. Error is %d", GetLastError());
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		return false;
	}

	m_pViewOfFile = MapViewOfFile(	m_hFileHandle,							// handle to file-mapping object
									FILE_MAP_ALL_ACCESS,					// access mode
									0,										// high-order DWORD of offset
									0,										// low-order DWORD of offset
									0										// number of bytes to map
								);

	if (NULL == m_pViewOfFile)
	{
		sprintf(m_szErrDesc, "Unable to map to the current address space. Error is %d", GetLastError());
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		return false;
	}

	return bRet;
}

bool CSharedMemory::CloseSM(bool bForced)
{
	return FreeSM(bForced);
}

bool CSharedMemory::FreeSM(bool bForced)
{
	bool bRet = true;
	if (m_pViewOfFile != NULL)
	{
		if(FALSE == UnmapViewOfFile(m_pViewOfFile))
		{
			sprintf(m_szErrDesc, "Unable to unmap view of file. Error is %d", GetLastError());
			#ifdef _MSGBOXDLL_H_
			if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
			#endif
			bRet = false;
			if(! bForced) return false;
		}
		m_pViewOfFile = NULL;
	}
	if (NULL != m_hFileHandle) CloseHandle(m_hFileHandle);
	m_hFileHandle = NULL;

	return bRet;
}

bool CSharedMemory::ReadFromSM()
{
	// char szLog[256];
	if(m_hFileHandle == NULL)
	{
		sprintf(m_szErrDesc, "FileMapping is not opened");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	if(m_pSM_Data == NULL)
	{
		sprintf(m_szErrDesc, "Buffer is not created");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	memcpy(m_pSM_Data, (char*)m_pViewOfFile, m_nSM_Size);
	// _LOG("-----> Read From Shared Memory Successfully");
	return true;
}

bool CSharedMemory::WriteToSM()
{
	// char szLog[256];
	if(m_hFileHandle == NULL)
	{
		sprintf(m_szErrDesc, "FileMapping is not opened");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		// printf("%s\n", szLog);
		return false;
	}

	if(m_pSM_Data == NULL)
	{
		sprintf(m_szErrDesc, "Buffer is not created");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	memcpy((char*)m_pViewOfFile, m_pSM_Data, m_nSM_Size);
	// _LOG("-----> Write To Shared Memory Successfully");
	return true;
}

bool CSharedMemory::ReadFromSM(int nIndex, char *pData, int nSize)
{
	// char szLog[256];
	if(m_hFileHandle == NULL)
	{
		sprintf(m_szErrDesc, "FileMapping is not opened");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	memcpy(pData, (char*)m_pViewOfFile+nIndex, nSize);
	// _LOG("-----> Read From Shared Memory Successfully");
	return true;
}

bool CSharedMemory::WriteToSM(int nIndex, char *pData, int nSize)
{
	// char szLog[256];
	if(m_hFileHandle == NULL)
	{
		sprintf(m_szErrDesc, "FileMapping is not opened");
		#ifdef _MSGBOXDLL_H_
		if(m_bErrMsgBox) _ERRMSG(m_szErrDesc);
		#endif
		// sprintf(szLog, "-----> Error : %d", m_szErrDesc);
		// _LOG(szLog);
		return false;
	}

	memcpy((char*)m_pViewOfFile+nIndex, pData, nSize);
	// _LOG("-----> Write To Shared Memory Successfully");
	return true;
}
