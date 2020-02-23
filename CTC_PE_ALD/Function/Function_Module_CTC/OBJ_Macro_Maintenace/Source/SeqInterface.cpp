// SeqInterface.cpp: implementation of the CSeqInterface class.
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
//--------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextParser.h"
#include "MsgBoxDll.h"
#include "TextLogDll_M.h"
#include "SeqInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSeqInterface::CSeqInterface()
{
	m_bSimulMode = FALSE;
	strcpy(m_szFuncName, "Function");
}

CSeqInterface::CSeqInterface(const char *szFuncName)
{
	m_bSimulMode = FALSE;
	strcpy(m_szFuncName, szFuncName);
}

CSeqInterface::~CSeqInterface()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CSeqInterface::Sim_Initialize()
{
	return TRUE;
}

Module_Status CSeqInterface::Sim_Main()
{
	return Main();
}

void CSeqInterface::Enter_Code()
{
}

BOOL CSeqInterface::Initialize(const char *szFuncName)
{
	strcpy(m_szFuncName, szFuncName);
	return Initialize();
}

BOOL CSeqInterface::Initialize()
{
	char szLogFilePath[80];

	// 2010.04.28
	sprintf(szLogFilePath, "_Log\\%s.log", m_szFuncName);
	_gInitLogDll(szLogFilePath, CFG_FILE_PATH, FALSE);

	if(m_bSimulMode)
	{
		printf_LOG("%s (Simul) Loaded\n", m_szFuncName);
		Sim_Initialize();	// 2010.04.28
	}
	else  printf_LOG("%s Loaded\n", m_szFuncName);
	return TRUE;
}

BOOL CSeqInterface::Deinitialize()
{
	return TRUE;
}

Module_Status CSeqInterface::Main()
{
	Module_Status msRet = SYS_SUCCESS;
	CTextParser txtP;
	char *pParam;
	char szCmd[80];
	char szParam[256];

	// if(m_bSimulMode) return Sim_Main();

	pParam = PROGRAM_PARAMETER_READ();
	txtP.SetText(pParam);
	do {
		txtP.GetWord(szCmd);
		if(txtP.BeEqual(szCmd, "LOG"))
		{
			txtP.GetString(szParam);
			_LOG("%s\n", szParam);
		}
		else
		{
			printf_LOG("Invalid Command [%s] !\n", pParam);
		}

	} while(0);

	return msRet;
}

double CSeqInterface::READ_ANALOG_TRUE(int nIoPoint, BOOL *pbResult)
{
	double dbRet;
	int nErrCount = 0;
	do {
		dbRet = READ_ANALOG(nIoPoint, pbResult);
		if(FALSE == *pbResult)
		{
			if(++nErrCount > 5) break;
			Sleep(100 + (nErrCount-1)*50);
		}
	} while(FALSE == *pbResult);
	return dbRet;
}

int CSeqInterface::READ_DIGITAL_TRUE(int nIoPoint, BOOL *pbResult)
{
	int nRet;
	int nErrCount = 0;
	do {
		nRet = READ_DIGITAL(nIoPoint, pbResult);
		if(FALSE == *pbResult)
		{
			if(++nErrCount > 5) break;
			Sleep(100 + (nErrCount-1)*50);
		}
	} while(FALSE == *pbResult);
	return nRet;
}

void CSeqInterface::ALARM_POST_WithMsg(int nAlarmID, char *szMsg)
{
	char szAlarmDesc[256];
	ALARM_MESSAGE_GET(nAlarmID, szAlarmDesc );
	strcat( szAlarmDesc , szMsg );
	ALARM_MESSAGE_SET(nAlarmID, szAlarmDesc);
	ALARM_POST(nAlarmID);
}

void CSeqInterface::printf_LOG(const char *pFormat, ...)				// 2010.04.28
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);

	printf(_szString);
	_LOG(_szString);
}

void CSeqInterface::_LOG(const char *pFormat, ...)						// 2010.09.06
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);

	::_LOG(_szString);
}

void CSeqInterface::printf_LOG_Hex(char pData[], int nSize)
{
	#define LINE_PER_CHAR	20

	int i = 0;
	char szTemp[40] = "";
	char szTemp2[1024] = "";

	for(i=0; i<nSize; i++)
	{
		sprintf(szTemp, " %02X", (pData[i] & 0xff));
		strcat(szTemp2, szTemp);
		if(0 == (i+1)%LINE_PER_CHAR)
		{
			strcat(szTemp2, "\n");
			printf_LOG(szTemp2);
			strcpy(szTemp2, "");
		}
	}

	if(0 != i%LINE_PER_CHAR)
	{
		strcat(szTemp2, "\n");
		printf_LOG(szTemp2);
	}
}

// 2011.04.14
void CSeqInterface::PopupMessage(const char *szTitle, const char *szMsg)
{
	_gOutMsgBox(szTitle, szMsg, FALSE);
}