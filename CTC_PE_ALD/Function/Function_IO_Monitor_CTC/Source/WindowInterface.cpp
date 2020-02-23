// WindowInterface.cpp: implementation of the CWindowInterface class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WindowInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWindowInterface::CWindowInterface()
{
	memset(m_szWindowName , 0 , sizeof(m_szWindowName));
	strcpy(m_szWindowName , "IOMonitorApp");

	m_hMsgLock = CreateMutex(NULL, FALSE, NULL);
}

CWindowInterface::~CWindowInterface()
{
	if (NULL != m_hMsgLock)
		CloseHandle(m_hMsgLock);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SendIOMessage()
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CWindowInterface::SendIOMessage(MSG_INFO* sMsgInfo)
{
	bool bRet = true;

	do {
		WaitForSingleObject(m_hMsgLock, INFINITE);
        m_hWnd = ::FindWindow(NULL , m_szWindowName);
		COPYDATASTRUCT CopyStruct;
		if (NULL != m_hWnd) {			
			CopyStruct.dwData = 0;
			CopyStruct.cbData = 2560;
			CopyStruct.lpData = (sMsgInfo);
			::SendMessage(m_hWnd , WM_COPYDATA , NULL , (LPARAM)&CopyStruct);			
		}

		ReleaseMutex(m_hMsgLock);
	} while (0);

	return bRet;
}