// EthernetManager.cpp: implementation of the CEthernetManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EthernetManager.h"
#include "PortInfo.h"
#include "DataProcessor.h"
#include "WindowInterface.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEthernetManager::CEthernetManager(CPortInfo* pPortInfo)
{
	m_dwThreadId      = 0;
	m_hThreadHandle   = NULL;
	m_bSoketConnected = false;
	m_pPortInfo       = pPortInfo;
	m_pDataProcessor  = NULL;
}

CEthernetManager::~CEthernetManager()
{
	DeInitSocket();
	if (NULL != m_pDataProcessor)
		delete m_pDataProcessor;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetModuleName
//Date         : 2007.03.13
//Description  : 
//Arguments    :
//Return Value : char*
//////////////////////////////////////////////////////////////////////////////////////////////////////
char* CEthernetManager::GetModuleName()
{
	return m_pPortInfo->GetModuleName();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetPortNum
//Date         : 2007.03.13
//Description  : 
//Arguments    :
//Return Value : unsigned short
//////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short CEthernetManager::GetPortNum()
{
	return m_pPortInfo->GetPortNum();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : Initialize()
//Date         : 2007.03.13
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CEthernetManager::Initialize(CWindowInterface* pWindowInterface)
{
	bool bRet = true;

	do {
		//Data Processor Init
		m_pDataProcessor = new CDataProcessor(this);
		if (false == m_pDataProcessor->InitIOConfig()) {
			bRet = false; break;
		}

		if (false == m_pDataProcessor->InitIOPoints()) {
			bRet = false; break;
		}

		m_pDataProcessor->SetWindowInterface(pWindowInterface);

		//Socket Item Init
		if (false == InitSocket()) {
			DeInitSocket();
			bRet = false; break;
		}
	
	} while(0);

	return bRet;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitSocket()
//Date         : 2007.03.14
//Description  : Open Socket & Initialize Listener thread
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CEthernetManager::InitSocket()
{
	bool bRet = true;

	do {
		m_hThreadHandle = CreateThread(NULL , 0 , ListenerThread , (void*)this , 0 ,  &m_dwThreadId);
		if (NULL == m_hThreadHandle) {
			bRet = false; break;
		}
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DeInitSocket()
//Date         : 2007.03.13
//Description  : Close Listener thread and close socket
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CEthernetManager::DeInitSocket()
{
	do {
		//1. Terminate Thread first
		if (NULL == m_hThreadHandle) break;

		TerminateThread(m_hThreadHandle,0);
		if (m_hThreadHandle) {
			DWORD dwExitCode = STILL_ACTIVE;
			BOOL BExitReturn = GetExitCodeThread(m_hThreadHandle,&dwExitCode);
			if (STILL_ACTIVE == dwExitCode) {	
				while (STILL_ACTIVE == dwExitCode) {
					Sleep(100);
					BExitReturn = GetExitCodeThread(m_hThreadHandle,&dwExitCode);	
					if (FALSE == BExitReturn)
						break;
				}

				CloseHandle(m_hThreadHandle);
				m_hThreadHandle = NULL;
			}
		}
	} while (0);

	//2.Socket Close
	do {
		if (false == IsSocketConnected()) break;

		ResetSocketConnectFlag();
		closesocket (m_ReceiveSocket);
		WSACleanup  ();
	} while (0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : ListenerThread()
//Date         : 2007.03.13
//Description  : 
//Arguments    :
//Return Value : DWORD
//////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CEthernetManager::ListenerThread(LPVOID lPvoid)
{
	IO_DATA_STRUCT sIOStruct;
	CEthernetManager* pManager = (CEthernetManager*)lPvoid;

	while (1) {
		//Socket Initialize
		WORD    version = MAKEWORD( 1, 1 );
		WSADATA wsa;
		if (0 != WSAStartup(version, &wsa)) {
			Sleep(1000);
			continue;
		}

		pManager->m_ReceiveSocket = socket(PF_INET , SOCK_STREAM , 0);
		if (pManager->m_ReceiveSocket == INVALID_SOCKET) {
			Sleep(1000);
			continue;
		}
		
		struct sockaddr_in Address;
		Address.sin_family      = AF_INET;
		Address.sin_addr.s_addr = INADDR_ANY;
		Address.sin_port        = htons(pManager->m_pPortInfo->GetPortNum());
		if (0 != bind (pManager->m_ReceiveSocket, reinterpret_cast<sockaddr*>(&Address), sizeof(Address))) {
			Sleep(1000);
			continue;
		}

		listen (pManager->m_ReceiveSocket, 50);

		while (TRUE) {	
			int AddressSize = sizeof(Address);
			pManager->m_SenderSocket = accept(pManager->m_ReceiveSocket, reinterpret_cast<sockaddr*>(&Address), &AddressSize);
			break;
		}

		pManager->SetSocketConnectFlag();
		pManager->m_pDataProcessor->GetConnectionMessage();

		while (recv(pManager->m_SenderSocket , reinterpret_cast<char*>(&sIOStruct), sizeof(sIOStruct), 0) != SOCKET_ERROR)
		{	
			//Data Processor Working start from here
			switch (sIOStruct.enNetStatus) {
			case POLL_START:
				//Log Start File Create 
				pManager->m_pDataProcessor->GetLogStartMessage(&sIOStruct);
				break;
			case POLL_STOP:
				//Log Stop File Close
				pManager->m_pDataProcessor->GetLogStopMessage(&sIOStruct);
				break;
			case IO_NORMAL:
				//File Write & Send to Window
				pManager->m_pDataProcessor->GetNormalIOMessage(&sIOStruct);
				break;
			case DNET_FAIL:
				break;
			}			
		}	

		//Disconnected with Server so De Initialize Socket and Retry Connect again
		pManager->ResetSocketConnectFlag();
		pManager->m_pDataProcessor->GetDisconnectionMessage();
		closesocket (pManager->m_ReceiveSocket);
		WSACleanup  ();
        printf("Disconnected %s Port %d \n" , pManager->m_pPortInfo->GetModuleName() , pManager->m_pPortInfo->GetPortNum());		
		Sleep(1000);
	}

	return 1;
}

