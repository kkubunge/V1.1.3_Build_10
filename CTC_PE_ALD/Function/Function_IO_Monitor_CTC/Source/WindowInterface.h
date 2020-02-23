// WindowInterface.h: interface for the CWindowInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWINTERFACE_H__B404ACE2_2FD9_455C_82D7_9EDB973AA131__INCLUDED_)
#define AFX_WINDOWINTERFACE_H__B404ACE2_2FD9_455C_82D7_9EDB973AA131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Utility.h"
class CWindowInterface  
{
public:
	CWindowInterface();
	virtual ~CWindowInterface();

	bool SendIOMessage(MSG_INFO* sMsgInfo);

private:
	HWND    m_hWnd;
	char    m_szWindowName[32];
	HANDLE	m_hMsgLock;

};

#endif // !defined(AFX_WINDOWINTERFACE_H__B404ACE2_2FD9_455C_82D7_9EDB973AA131__INCLUDED_)
