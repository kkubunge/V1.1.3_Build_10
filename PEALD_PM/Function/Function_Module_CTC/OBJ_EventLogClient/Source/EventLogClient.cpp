// EventLogClient.cpp: implementation of the CEventLogClient class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

//--------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
#include <CimSeqncKNFS.h>
//--------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "EventLogClient.h"
#include "TextLogDll_M.h"
#include "TextParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventLogClient::CEventLogClient()
{
	strcpy(m_szModuleName, "PM1");
}

CEventLogClient::~CEventLogClient()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

void CEventLogClient::Event_Message_Received()
{
	CTextParser tp;
	char szMessage[256];
	char szLogType[40];
	char szTemp[256];
	char szLog[256];

	strcpy(szMessage, PROGRAM_EVENT_READ());
	tp.SetText(szMessage);
	tp.GetWord(szLogType);
	tp.GetString(szTemp);
	if(tp.BeEqual(szLogType, "PRC"))
	{
		sprintf(szLog, "PRC %s %s", m_szModuleName, szTemp);
		_nINF_KNFS_dWRITE_FUNCTION_EVENT("CTC_EVENT_LOG_SVR", szLog);
	}
	else if(tp.BeEqual(szLogType, "FNC"))
	{
		sprintf(szLog, "FNC %s %s", m_szModuleName, szTemp);
		_nINF_KNFS_dWRITE_FUNCTION_EVENT("CTC_EVENT_LOG_SVR", szLog);
	}
	__LOG(szMessage);
	PROGRAM_EVENT_STATUS_SET(SYS_SUCCESS);
}

void CEventLogClient::Enter_Code()
{
	char szParam[256];
	char szTemp[80];
	CTextParser txtP;

	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	txtP.SetText(szParam);						// 2012.07.05

	do {
		if(! txtP.GetWord(szTemp)) break;
		strcpy(m_szModuleName, szTemp);
	} while(0);
}

BOOL CEventLogClient::Initialize()
{
	char szHostName[256];
	int nServerControlMode;
	int nPort1;
	int nPort2;
	int nExtendPort;
	int nIOStatus;

	CSeqInterface::Initialize();
	READ_STRING(MARS_PMNameSM,m_szModuleName,&nIOStatus);
	GET_MODULE_CONNECTION_INFO(szHostName, &nServerControlMode, &nPort1, &nPort2, &nExtendPort);
	if( (! nServerControlMode) && (nExtendPort > 0) )
	{
		_nINF_KNFS_SET_FILESAVE_SKIP_STATUS( SYSTEM_LOGSKIP_STATUS() );
		if(_nINF_KNFS_CONNECTION_FOR_CLIENT(szHostName, nExtendPort) != 0)
		{
			printf_LOG("_nINF_KNFS_CONNECTION_FOR_CLIENT ExtendPort Initialize Error\n");
			return FALSE;
		}
	}

	return TRUE;
}