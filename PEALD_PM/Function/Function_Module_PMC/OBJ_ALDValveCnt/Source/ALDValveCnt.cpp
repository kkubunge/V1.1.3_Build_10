// LotFlowControl.cpp: implementation of the CLotFlowControl class.
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

#include "Timer.h"
#include "TextParser.h"
#include "ALDValveCnt.h"
#include "TextLogDll_M.h"


//--------------------------------------------------------------------
enum { EN_Alarm, EN_Normal };

enum {
	PMC_Unuse, PMC_Local, PMC_DisCon,  PMC_Standby, PMC_Maint,
	PMC_Uninit, PMC_EnablePM, PMC_CTCInUse, PMC_CTCDis, PMC_CTCDis2,
	PMC_CTCEnaPM, PMC_CTCDisHW, PMC_CTCDisHW2
};

enum { LOTSM_Conc, LOTSM_Cont, LOTSM_Seq, LOTSM_None };
enum { LOTC_None, LOTC_AutoLotPause };
enum { MANUAL, AUTO, LI_MAN } ;
enum { PM_PATH_NONE, PM_PATH_PM1, PM_PATH_PM2, PM_PATH_PM1_2 };

enum { EN_PM1, EN_PM2 };
enum { EN_TM1, EN_TM2 };
enum { PROSTS_Idle, PROSTS_Running, PROSTS_Aborted, PROSTS_SeqErr, PROSTS_LotPreEnd, PROSTS_LotPstEnd };
enum { PMFLC_None, PMFLC_Break };

enum { EN_OFF, EN_ON };

enum { LLVAC_ATM, LLVAC_Vac, LLVAC_GoATM, LLVAC_GoVac, LLVAC_Abort };

enum { RCP_STOP, RCP_RUNNING};

#define ALARM_ALDVALVE_USECOUNTOVER 1060

#define CONFIG_FILE ".\\Function\\Function_Module_PMC\\OBJ_ALDValveCnt\\ALDMonConfig.ini"

//--------------------------------------------------------------------			
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CALDVlvMon::CALDVlvMon()
{
	InitializeCriticalSection(&m_csLock);
}

CALDVlvMon::~CALDVlvMon()
{
	DeleteCriticalSection(&m_csLock);
}

//////////////////////////////////////////////////////////////////////
//

Module_Status CALDVlvMon::Main()
{
	Module_Status msRet = SYS_SUCCESS;
	CTextParser txtP;
	char szCmd[16] = {0,};
	int nListNumber = 0;
	char *pParam;

	pParam = PROGRAM_PARAMETER_READ();
	txtP.SetText(pParam);

	do {
		if(!txtP.GetWord(szCmd))	break;

		if(STRNCMP_L("RESET", szCmd, 5))
		{
			nListNumber = atoi(szCmd+5);
			if(nListNumber < 1 || nListNumber > MAX_VALVE)
			{
				printf("[ERROR] Invalid ALD Valve RESET Number[%d]\n", nListNumber);
				break;
			}
			//
			ResetVlvCount(nListNumber);
		}

		if(STRCMP_L("ALD_VLV_USESUM",szCmd)) SUMVlvCount();
	} while(0);

	return msRet;
}

void CALDVlvMon::Enter_Code()
{
	char szParam[256] = "";

	strcpy(szParam, PROGRAM_ARGUMENT_READ());
}

//////////////////////////////////////////////////////////////////////////
BOOL CALDVlvMon::Initialize()
{
	char szBuf[256] = "";
	int nVlvNum = 0;
	int i;

	BOOL bRet;
	bRet = CSeqInterface::Initialize();

	_gINIT_LOG();

	m_nALDVlvUseCnt = 0;

	_LOG("ALD Valve Read Last Count !!");
	//...
	for(i = 0; i < MAX_VALVE; i++)
	{
		m_nOpenCnt[i]		= 0;
		m_nOpenCnt_old[i]	= 0;
		m_nOpenDif[i]		= 0;
		m_nCloseCnt[i]		= 0;
		m_nCloseCnt_old[i]	= 0;
		m_nCloseDif[i]		= 0;

		//... Read Last Count
		m_nALDValveOpenCnt[i]	= (int) READ_ANALOG(ALD97OpenCnAM  + i, &nIOStatus);    //2014.11.28
		m_nALDValveCloseCnt[i]	= (int) READ_ANALOG(ALD97CloseCnAM + i, &nIOStatus);   //2014.11.28

		nVlvNum = Change_IndexToVlvNum(i);
		sprintf(szBuf, "ALD Valve[%d]\tOpen = %d\tClose = %d", nVlvNum, m_nALDValveOpenCnt[i], m_nALDValveCloseCnt[i]);

		_LOG(szBuf);
	}

	_LOG("==================================================");
	//...
	bRet = ReadConfiguration();

	CreateThread();

	return bRet;
}


//////////////////////////////////////////////////////////////////////
// Member Function
//////////////////////////////////////////////////////////////////////
// Monitor Thread Function
BOOL CALDVlvMon::ReadConfiguration()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(CONFIG_FILE, "rt");
		if(fp == NULL) break;

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("ALD Valve", szItem) == 0)			nTitle = 1;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseALDValveConfig(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s", CONFIG_FILE);
	}
	return bRet;
}

void CALDVlvMon::ParseALDValveConfig(char *szData)
{
	char szItem[40];
	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "ALDValveUseCount") == 0) tp.GetInt(&m_nALDVlvUseCnt);
	} while(0);
}

//////////////////////////////////////////////////////////////////////////
int CALDVlvMon::Change_IndexToVlvNum(int nIndex)
{
	int nVlvNum = -1;	//.. Check for Invalid Number

	//... 2015.02.13 Index Value Bug Fix (Index = IO Index 0:97)
	switch(nIndex)
	{
	case 0 : nVlvNum = 97;		break;
	case 1 : nVlvNum = 98;		break;
	case 2 : nVlvNum = 100;		break;
	case 3 : nVlvNum = 101;		break;
	case 4 : nVlvNum = 102;		break;
	case 5 : nVlvNum = 105;		break;
	case 6 : nVlvNum = 106;		break;
	case 7 : nVlvNum = 108;		break;
	case 8 : nVlvNum = 109;		break;
	case 9 : nVlvNum = 110;		break;

	//... 2015.04.28 Add ALD Valve 3 EA (V11, V12, V17)
	case 10 : nVlvNum = 11;		break;
	case 11 : nVlvNum = 12;		break;
	case 12 : nVlvNum = 17;		break;
	}
	
	return nVlvNum;
}

//////////////////////////////////////////////////////////////////////////
void CALDVlvMon::ALARM_POST_WithMsg(int nAlarmID, int nValve)
{
	char szAlarmDesc[256];
	char szTemp[256];

	sprintf(szTemp, "(Valve=%d)", nValve);
	ALARM_MESSAGE_GET(nAlarmID, szAlarmDesc );
	strcat( szAlarmDesc , szTemp );
	ALARM_MESSAGE_SET(nAlarmID, szAlarmDesc);
	ALARM_POST(nAlarmID);
}

//////////////////////////////////////////////////////////////////////////
void CALDVlvMon::ResetVlvCount(int nValveIndex)
{
	int nIOStatus;
	char szStringTemp[255] = "";
	int nResetCount= 0;
	int nACCCount= 0;
	int nVlvNum = 0;
	int nIOIndex= 0;
	int nBackUpAccCount= 0;

	EnterCriticalSection(&m_csLock);

	//...
	nIOIndex = nValveIndex - 1;

	//... Init Valve Count
	m_nALDValveOpenCnt[nIOIndex]	= 0;
	m_nALDValveCloseCnt[nIOIndex]   = 0;

	nBackUpAccCount = (int) READ_ANALOG(ALD97OpenCnAM + nIOIndex, &nIOStatus);
	WRITE_ANALOG(ALD97OpenCnAM	+ nIOIndex, m_nALDValveOpenCnt[nIOIndex],	&nIOStatus);
	WRITE_ANALOG(ALD97CloseCnAM	+ nIOIndex, m_nALDValveCloseCnt[nIOIndex],	&nIOStatus);

	//... Update Reset History
	for(int nCount = 8; nCount >= 0; nCount--)
	{
		READ_STRING (ALDResetTime01 + nCount,		(char*)szStringTemp, &nIOStatus);
		WRITE_STRING(ALDResetTime01 + nCount + 1,	(char*)szStringTemp, &nIOStatus);

		nACCCount = READ_DIGITAL(ALDVlvRstCnt01 + nCount, &nIOStatus);
		WRITE_DIGITAL(ALDVlvRstCnt01 + nCount + 1, nACCCount, &nIOStatus);
	}
	
	nResetCount = READ_DIGITAL(ALDResetCount, &nIOStatus);
	WRITE_DIGITAL(ALDResetCount, (int)(nResetCount+1), &nIOStatus);

	//... 
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(szStringTemp, "%02d / %02d / %02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	WRITE_STRING(ALD97VlvRstTime + nIOIndex, (char*)szStringTemp, &nIOStatus);

	nVlvNum = Change_IndexToVlvNum(nIOIndex);
	_LOG("Reset Vlv Count");
	if(nVlvNum > 0)
	{	//... Update Last Reset History
		sprintf(szStringTemp, "%04d / %02d / %02d   %02d : %02d : %02d (V%d)", 
			sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,nVlvNum);
		WRITE_STRING(ALDResetTime01, (char*)szStringTemp, &nIOStatus);		
		WRITE_DIGITAL(ALDVlvRstCnt01, nBackUpAccCount, &nIOStatus);
		_LOG(szStringTemp);
	}
	_LOG("==================================================");
	LeaveCriticalSection(&m_csLock);

}

//////////////////////////////////////////////////////////////////////////
void CALDVlvMon::SUMVlvCount()
{
	char szBuf[256] = "";
	int nIOStatus;
	int i = 0;
	int nVlvNum = 0;

	EnterCriticalSection(&m_csLock);

	for(i = 0; i < MAX_VALVE; i++)
	{	
		m_nOpenCnt[i]	= (int) READ_ANALOG(ALD97OpenCnAI  + i, &nIOStatus);
		m_nCloseCnt[i]	= (int) READ_ANALOG(ALD97CloseCnAI + i, &nIOStatus);

		m_nALDValveOpenCnt[i]  += m_nOpenCnt[i];
		m_nALDValveCloseCnt[i] += m_nCloseCnt[i];

		nVlvNum = Change_IndexToVlvNum(i);
		sprintf(szBuf, "ALD Valve[%d]\tOpen = %d\tClose = %d", nVlvNum, m_nALDValveOpenCnt[i], m_nALDValveCloseCnt[i]);

		_LOG(szBuf);

		if(m_nALDValveOpenCnt[i] > m_nALDVlvUseCnt || m_nALDValveCloseCnt[i] > m_nALDVlvUseCnt)
		{			
			if(ALM_PAUSED != ALARM_STATUS(ALARM_ALDVALVE_USECOUNTOVER))
			{
				// nVlvNum = Change_IndexToVlvNum(i);
				_LOG("ALARM_ALDVALVE_USECOUNTOVER => ALD Valve Use Count Over = %d", m_nALDVlvUseCnt);
				ALARM_POST_WithMsg(ALARM_ALDVALVE_USECOUNTOVER, nVlvNum);
			}
		}
		else
		{
			if(m_nOpenCnt[i]  != 0)		WRITE_ANALOG(ALD97OpenCnAM  + i, m_nALDValveOpenCnt[i],  &nIOStatus);
			if(m_nCloseCnt[i] != 0)		WRITE_ANALOG(ALD97CloseCnAM + i, m_nALDValveCloseCnt[i], &nIOStatus);
		}
	}
	_LOG("==================================================");
	LeaveCriticalSection(&m_csLock);
}

//////////////////////////////////////////////////////////////////////////
void CALDVlvMon::DoMonitor()
{
// 	int nIOStatus;
// 	int i = 0, k = 0;
	
	while(1)
	{
		_sleep(1000);
		
		/*
		if(m_bSimulMode)
		{
			if(RCP_RUNNING == READ_DIGITAL(PRO_RunStsDM,&nIOStatus))
			{
				for(i = 0; i < MAX_VALVE; i++)
				{
					if(i == 0)		k++;
					WRITE_ANALOG(ALD97OpenCnAI  + i, k, &nIOStatus);
					WRITE_ANALOG(ALD97CloseCnAI + i, k, &nIOStatus);
				}
			}
			else
			{
				for(i = 0; i < MAX_VALVE; i++)
				{
					k = 0;
					WRITE_ANALOG(ALD97OpenCnAI	+ i, 0, &nIOStatus);
					WRITE_ANALOG(ALD97CloseCnAI	+ i, 0, &nIOStatus);
				}
			}
		}
		*/
	}
}


