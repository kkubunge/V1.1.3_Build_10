// GblMsgMngr.h: interface for the CGblMsgMngr class.
// Create : 2006.03.16
// Last Update : 2006.03.16
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GBLMSGMNGR_H__E7F17907_9E8D_42C8_91BE_C06DE5506C41__INCLUDED_)
#define AFX_GBLMSGMNGR_H__E7F17907_9E8D_42C8_91BE_C06DE5506C41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Interface Status Definition
#ifndef INTF_DISCONNECTED
#define INTF_DISCONNECTED	0
#define INTF_VALID			1
#define INTF_OVERFLOW		2
#define INTF_EMPTY			3
#define INTF_TIMEOUT		4
#endif

class CGblMsgMngr  
{
public:
	CGblMsgMngr();
	virtual ~CGblMsgMngr();

// Member Function
public:
	bool InitMsgMngr(const char *szServerName = NULL, const char *szClientName = NULL);
	void DeinitMsgMngr(bool bFreeDll = false);

	bool CreateInterface(const char *szServerName, const char *szClientName = NULL);
	bool SendMsg(const char *szMsg, int nMsgSize = 0);
	bool RecvMsg(char *szMsg, int *pnMsgSize = NULL);
	bool WaitForMessage();
	bool InvalidateWaitFunc();
	void ValidateWaitFunc();
	bool IsSvrQueueEmpty();
	bool IsCltQueueFull();
	const char *GetLastError();
	int GetClientStatus();
	bool Command(const char *szCmd, char *szReply = NULL);

// Member Variable
public:
	int m_nInstanceID;		// Valid:0 ~, Invalid: -1
private:
};

#endif // !defined(AFX_GBLMSGMNGR_H__E7F17907_9E8D_42C8_91BE_C06DE5506C41__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* _gGblMsgMngr_Command Parameter(char *szCmd, char *szReply)
	"NAME_AUTO_CREATION YES/NO" :  Name이 중복될 때 자동 Creation 기능.
	"GET_SERVER_NAME" : 자동 Creation 된 Name을 'szReply'로 얻는다.
*/

//////////////////////////////////////////////////////////////////////
/* Update History
2006.03.16	Created by cskim.
			이 class를 사용하기 위해서는 'GblMsgMngr.dll' Version 1.0.2.0 이상 필요
*/