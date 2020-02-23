// EthernetManager.h: interface for the CEthernetManager class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ETHERNETMANAGER_H__FF4F3C7E_86BD_4893_B1E9_6EE311859E25__INCLUDED_)
#define AFX_ETHERNETMANAGER_H__FF4F3C7E_86BD_4893_B1E9_6EE311859E25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Utility.h"

class CPortInfo;
class CDataProcessor;
class CWindowInterface;
class CEthernetManager  
{
public:
	CEthernetManager(CPortInfo* pPortInfo = NULL);
	virtual ~CEthernetManager();

	unsigned short GetPortNum  ();
	char* GetModuleName        ();

	bool Initialize            (CWindowInterface* pWindowInterface = NULL);
	bool InitSocket            ();
	void DeInitSocket          ();

	bool IsSocketConnected     () {return m_bSoketConnected;};
	void SetSocketConnectFlag  () {m_bSoketConnected = true;};
	void ResetSocketConnectFlag() {m_bSoketConnected = false;};

private:
	bool    m_bSoketConnected;
	SOCKET  m_ReceiveSocket;
	SOCKET  m_SenderSocket;
	DWORD   m_dwThreadId;
	HANDLE  m_hThreadHandle;
	static DWORD WINAPI ListenerThread(LPVOID lPvoid);

	CPortInfo*      m_pPortInfo;
	CDataProcessor* m_pDataProcessor;

};

#endif // !defined(AFX_ETHERNETMANAGER_H__FF4F3C7E_86BD_4893_B1E9_6EE311859E25__INCLUDED_)
