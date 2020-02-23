// EventLogClient.h: interface for the CEventLogClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTLOGCLIENT_H__869BEEAA_5FE9_4FE6_83F0_08DD4C8F1CEE__INCLUDED_)
#define AFX_EVENTLOGCLIENT_H__869BEEAA_5FE9_4FE6_83F0_08DD4C8F1CEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SeqInterface.h"
class CEventLogClient  : public CSeqInterface
{
public:
	CEventLogClient();
	virtual ~CEventLogClient();

// Member Function
	virtual void Event_Message_Received();
	virtual void Enter_Code();
	virtual BOOL Initialize();

// Member Variable
public:
	char m_szModuleName[20];
};

#endif // !defined(AFX_EVENTLOGCLIENT_H__869BEEAA_5FE9_4FE6_83F0_08DD4C8F1CEE__INCLUDED_)
