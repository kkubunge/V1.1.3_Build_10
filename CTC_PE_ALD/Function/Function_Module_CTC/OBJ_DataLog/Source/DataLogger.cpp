// DataLogger.cpp: implementation of the CDataLogger class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "DataLogger.h"

// #define _DEBUG


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDataLogger::CDataLogger(char* szName , int nLogPeriod , bool bLogOption ,int nDel, char* szFilePath)
{
	m_nLogPeriod    = nLogPeriod;
	m_bLogOption    = bLogOption;
	m_nDeletePeriod = nDel;

	m_nIOCount = 0;
	memset(m_szName , 0 , sizeof(m_szName));
	strcpy(m_szName , szName);
	memset(m_szFilePath , 0 , sizeof(m_szFilePath));
	strcpy(m_szFilePath , szFilePath);
	// printf("DataLogger::FilePath --> %s \n" , m_szFilePath);

	m_dwThreadId = 0;
	m_dwAutoDelThreadID = 0;
	m_bLogRunning= false;
	m_bLoggingStartReady = true;	// 2006.12.17 by ycchoi

	m_abRcpLogEnable[RT_Unknown]		= FALSE;
	m_abRcpLogEnable[RT_ProcRcp]		= TRUE;
	m_abRcpLogEnable[RT_PreProRcp]		= FALSE;
	m_abRcpLogEnable[RT_PostProRcp]		= FALSE;
	m_abRcpLogEnable[RT_IdleRcp]		= FALSE;
	m_abRcpLogEnable[RT_CleanRcp]		= TRUE;
	m_abRcpLogEnable[RT_PurgeRcp]		= FALSE;
	m_abRcpLogEnable[RT_LotPreRcp]		= TRUE;
	m_abRcpLogEnable[RT_LotPostRcp]		= FALSE;
}

CDataLogger::~CDataLogger()
{
	for(int i = 0 ; i < MAX_IO_LIST ; i++)
	{
		IO_LIST* pIOList = m_IOList[i];

		if(NULL != pIOList)
		{
			delete pIOList;
			pIOList = NULL;		
		}
	}
	
	if(0 != m_dwThreadId)
		PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);

	if(0 != m_dwAutoDelThreadID)
		PostThreadMessage(m_dwAutoDelThreadID , WM_QUIT , 0 , 0);
}

// 2006.12.17 by ycchoi < 1.0.0.9 >
bool CDataLogger::IsLoggingStartReady()
{
	return m_bLoggingStartReady;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : Initialize()
//Date         : 2005.04.21
//Description  : This function get fixed io's IOPoint & Create Log Thread
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::Initialize()
{
	char szIOName[128];

	if(0 == strcmp(m_szName, "PMC"))	// for PMC version
	{
		memset(szIOName , 0 , sizeof(szIOName));
		strcat(szIOName , "PRO_RunStsDM");
		
		m_nRusStsIOPoint = _FIND_FROM_STRING(_K_D_IO,szIOName);
		if(-1 == m_nRusStsIOPoint)
			printf("%s.PRO_RunStsDM Channel IO Point is not valid \n" , m_szName);
		
		memset(szIOName , 0 , sizeof(szIOName));
		strcat(szIOName , "PRO_RcpTypeDM");
		m_nRcpTypeIOPoint = _FIND_FROM_STRING(_K_D_IO,szIOName);
		if(-1 == m_nRcpTypeIOPoint)
			printf("%s.PRO_RcpTypeDM Channel IO Point is not valid \n" , m_szName);
		
		memset(szIOName , 0 , sizeof(szIOName));
		strcat(szIOName , "PRO_RcpNameSM");
		m_nRcpNameIOPoint = _FIND_FROM_STRING(_K_S_IO,szIOName);
		if(-1 == m_nRcpNameIOPoint)
			printf("%s.PRO_RcpNameSM Channel IO Point is not valid \n" , m_szName);
		
		memset(szIOName , 0 , sizeof(szIOName));
		strcpy(szIOName , "CTC_"  );
		strcat(szIOName , m_szName);
		strcat(szIOName , "_CommStatus");
		m_nPMInusedIOPoint = _FIND_FROM_STRING(_K_D_IO ,szIOName);
		if(-1 == m_nPMInusedIOPoint)
			printf("CTC_%s_CommStatus Channel IO Point is not valid \n" , m_szName);	
	}
	else
	{
		memset(szIOName , 0 , sizeof(szIOName));
		strcpy(szIOName , m_szName);
		strcat(szIOName , ".");
		strcat(szIOName , "PRO_RunStsDM");

		m_nRusStsIOPoint = _FIND_FROM_STRING(_K_D_IO,szIOName);
		if(-1 == m_nRusStsIOPoint)
			printf("%s.PRO_RunStsDM Channel IO Point is not valid \n" , m_szName);

		memset(szIOName , 0 , sizeof(szIOName));
		strcpy(szIOName , m_szName);
		strcat(szIOName , ".");
		strcat(szIOName , "PRO_RcpTypeDM");
		m_nRcpTypeIOPoint = _FIND_FROM_STRING(_K_D_IO,szIOName);
		if(-1 == m_nRcpTypeIOPoint)
			printf("%s.PRO_RcpTypeDM Channel IO Point is not valid \n" , m_szName);

		memset(szIOName , 0 , sizeof(szIOName));
		strcpy(szIOName , m_szName);
		strcat(szIOName , ".");
		strcat(szIOName , "PRO_RcpNameSM");
		m_nRcpNameIOPoint = _FIND_FROM_STRING(_K_S_IO,szIOName);
		if(-1 == m_nRcpNameIOPoint)
			printf("%s.PRO_RcpNameSM Channel IO Point is not valid \n" , m_szName);

		memset(szIOName , 0 , sizeof(szIOName));
		strcpy(szIOName , "CTC."  );
		strcat(szIOName , m_szName);
		strcat(szIOName , "_CommStatus");
		m_nPMInusedIOPoint = _FIND_FROM_STRING(_K_D_IO ,szIOName);
		if(-1 == m_nPMInusedIOPoint)
			printf("CTC.%s_CommStatus Channel IO Point is not valid \n" , m_szName);
	}

	
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (void*)this, 0, &m_dwThreadId);
	if (NULL == hThread)
	{
		printf("%s Logger Thread Create Fail.. \n" , m_szName);
	}
	CloseHandle(hThread);

	hThread = CreateThread(NULL, 0, AutoDelThread, (void*)this, 0, &m_dwAutoDelThreadID);
	if (NULL == hThread)
	{
		printf("%s Auto Delete Thread Create Fail.. \n" , m_szName);
	}
	CloseHandle(hThread);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetRunStsIOPoint()
//Date         : 2005.04.21
//Description  : Transfer Process Status IO's IO Point to Main function
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int  CDataLogger::GetRunStsIOPoint()
{
	return m_nRusStsIOPoint;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetItemCount()
//Date         : 2005.04.21
//Description  : Return Total Defined IO Count from configuration ini
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int  CDataLogger::GetItemCount()
{
	return m_nIOCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetLogItem()
//Date         : 2005.04.21
//Description  : After Parse config ini defined io information comming here and saved
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDataLogger::SetLogItem(char* szIOName , int nIOType)
{
	bool bRet = true;
	do
	{
		if(m_nIOCount > MAX_IO_LIST)
		{
			printf("Exceed max io count ... \n");
			break;
		}

		m_IOList[m_nIOCount] = new IO_LIST;
		//1.
		char szIOFullName[32];
		memset(szIOFullName , 0 , sizeof(szIOFullName));

		if(0 == strcmp(m_szName, "PMC"))	// for PMC version
		{
			strcat(szIOFullName , szIOName);
			strcpy(m_IOList[m_nIOCount]->szIOName , szIOFullName);
		}
		else
		{
			strcpy(szIOFullName , m_szName);
			strcat(szIOFullName , ".");
			strcat(szIOFullName , szIOName);
			strcpy(m_IOList[m_nIOCount]->szIOName , szIOFullName);
		}

		// printf("DataLogger::%s defined \n" , m_IOList[m_nIOCount]->szIOName);

		//2.
		m_IOList[m_nIOCount]->nIOType = nIOType;

		//3.	
		m_IOList[m_nIOCount]->nIOPoint = _FIND_FROM_STRING(m_IOList[m_nIOCount]->nIOType
														 ,m_IOList[m_nIOCount]->szIOName);


		if(m_IOList[m_nIOCount]->nIOPoint != -1)
			m_nIOCount++;
		else
		{
			printf("IOName Name %s , IO Type %d returns iopoint is -1 \n" , m_IOList[m_nIOCount]->szIOName ,m_IOList[m_nIOCount]->nIOType);
			bRet = false;		
		}
	}while(0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : StartLogging()
//Date         : 2005.04.21
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::StartLogging()
{
	// PM Comm Status Check ////////////////////////////////////////////////
	int nCommStatus,nIOStatus;
	nCommStatus = _dREAD_DIGITAL(m_nPMInusedIOPoint , &nIOStatus);
	if(nCommStatus == 3/*StandBy*/ || nCommStatus == 4/*Maint*/ ||  nCommStatus == 6/*Enable PM*/ ||  nCommStatus == 7/*CTC In Use*/ ||
	   (0 == strcmp(m_szName, "PMC")) )		// for PMC version
	{
		// 2006.12.17 by ychoi
		m_bLoggingStartReady = FALSE;
		m_bLogRunning = true;
		PostThreadMessage(m_dwThreadId,MSG_LOG_START,0,0);

	}
	else
	{
		printf("Log Skip because it is not Maint('%d') or Enable PM Mode \n",nCommStatus);					
	}
	// PM Comm Status Check ////////////////////////////////////////////////
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : StopLogging()
//Date         : 2005.04.21
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::StopLogging()
{
	m_bLogRunning = false;
//	PostThreadMessage(m_dwThreadId,MSG_LOG_STOP,0,0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : ThreadProc()
//Date         : 2005.04.21
//Description  : Log Thread PM_NOREMOVE , PM_REMOVE is not executed properly so co check special flag
//Arguments    :
//Return Value : DWORD
//////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CDataLogger::ThreadProc(LPVOID pVoid)
{
	MSG msg;
	BOOL bRet = FALSE;
	CDataLogger* pLogger = (CDataLogger*)pVoid;
	
//	PeekMessage(&msg,NULL, 0, 0, PM_NOREMOVE);
	
	do
	{
		bRet = GetMessage(&msg, NULL, 0, 0);
		switch(msg.message)
		{
			case WM_QUIT:
				break;
			case MSG_LOG_START:
				pLogger->DoLogging();				
				break;
		}		

	}while(1);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : AutoDelThread()
//Date         : 2005.04.21
//Description  : Log Thread PM_NOREMOVE , PM_REMOVE is not executed properly so co check special flag
//Arguments    :
//Return Value : DWORD
//////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CDataLogger::AutoDelThread(LPVOID pVoid)
{
	CDataLogger* pLogger = (CDataLogger*)pVoid;	

	do
	{
		pLogger->DoAutoLogDelete();
		Sleep(1000*60*60*24);
	}while(1);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : AutoLogDelete()
//Date         : 2005.04.21
//Description  : Each Folder's log file will deleted by this function
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::DoAutoLogDelete()
{
	CFileFind FindFile;
	//------------------------------------------------
	// 2006.04.26
	char szFilePath[512] = {0};
	char szFileName[256] = {0};

	char szYear[40] = {0};			
	char szMonth[40] = {0};
	char szDay[40] = {0};

	int  nYear  = 0;
	int  nMonth = 0;
	int  nDay   = 0;
	int  nTotal;
	//------------------------------------------------

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	#ifdef _DEBUG
	printf("GetTime:%04d.%02d.%02d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	#endif

	strcpy(szFilePath , m_szFilePath);
	strcat (szFilePath , "\\");
	strcat (szFilePath , m_szName);
	strcat (szFilePath , "\\*.*");
	// printf("Delete file path %s \n" , szFilePath);

	BOOL bRet = FALSE;
	bRet = FindFile.FindFile(szFilePath);
	while(bRet)
	{
		bRet = FindFile.FindNextFile();

		do
		{
			if (FindFile.IsDots()) continue;
			if (FindFile.IsReadOnly()) continue;
			if (FindFile.IsDirectory()) continue;

			CString str = FindFile.GetFileName();
			str.TrimLeft();
			str.TrimRight();
			strcpy(szFileName , str);

			int nByteCount = 0;
			int nCurrentByte = 0;
			int nSize = str.GetLength();
			
			//------------------------------------------------
			// 2006.04.26 by cskim
			while(1)
			{
				if(szFileName[nCurrentByte] != '_' && szFileName[nCurrentByte] != 0)
				{
					szYear[nByteCount] = szFileName[nCurrentByte];
					nByteCount++; nCurrentByte++;
				}
				else
				{
					if(szFileName[nCurrentByte] != 0) nCurrentByte++;
					break;
				}
			}
			szYear[nByteCount] = 0;
			nByteCount = 0;
			while(1)
			{
				if(szFileName[nCurrentByte] != '_' && szFileName[nCurrentByte] != 0)
				{
					szMonth[nByteCount] = szFileName[nCurrentByte];
					nByteCount++; nCurrentByte++;
				}
				else
				{
					if(szFileName[nCurrentByte] != 0) nCurrentByte++;
					break;
				}
			}
			szMonth[nByteCount] = 0;
			nByteCount = 0;
			while(1)
			{
				if(szFileName[nCurrentByte] != '_' && szFileName[nCurrentByte] != 0)
				{
					szDay[nByteCount] = szFileName[nCurrentByte];
					nByteCount++; nCurrentByte++;
				}
				else
				{
					if(szFileName[nCurrentByte] != 0) nCurrentByte++;
					break;
				}
			}
			szDay[nByteCount] = 0;
			//------------------------------------------------

			nYear = atoi(szYear);
			nMonth= atoi(szMonth);
			nDay  = atoi(szDay);

			nTotal = (sysTime.wYear - nYear)*365 + (sysTime.wMonth - nMonth)*30 + (sysTime.wDay - nDay);
			#ifdef _DEBUG
			printf("GetTime:%04d.%02d.%02d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
			printf("FileTime:%04d.%02d.%02d\n", nYear, nMonth, nDay);
			#endif
			if(nTotal > m_nDeletePeriod)
			{
				printf("File %s is passed %d days since created limit %d so deleted \n" , FindFile.GetFileName() , nTotal , m_nDeletePeriod);
				_unlink(FindFile.GetFilePath());
			}

		}while(0);
		
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoLogging()
//Date         : 2005.04.21
//Description  : Every Log period defined io value saved to file
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::DoLogging()
{

/*
 *	2006.12.17 by ycchoi < 1.0.0.9 >
 *
 *	Logging 종료 시에는 반드시 m_bIsLoggingStartReady 값을 True로 설정하기 위해서
 *	__try __finally 문을 사용한다.
 */
__try
{	
	if(false == m_bLogOption)
	{
		printf("%s Manual Log Disabled \n" , m_szName);
		return;
	}

	//--------------------------------------------------------------
	// 2006.09.14
	int nRcpType        = 0;
	int nCommStatus     = 0;
	int nIOStatus;

	nRcpType = _dREAD_DIGITAL(m_nRcpTypeIOPoint , &nIOStatus);
	nCommStatus = _dREAD_DIGITAL(m_nPMInusedIOPoint , &nIOStatus);

	do
	{
		if(0 == strcmp(m_szName, "PMC"))	// for PMC version
		{
			break;
		}

		if(! m_abRcpLogEnable[nRcpType]) return;
		switch(nRcpType)
		{
		case RT_Unknown:	break;
		case RT_ProcRcp:	if(7/*CTCINUSE*/ == nCommStatus) return; break;
		case RT_PreProRcp:	break;
		case RT_PostProRcp:	break;
		case RT_IdleRcp:	break;
		case RT_CleanRcp:	break;
		case RT_PurgeRcp:	break;
		case RT_LotPreRcp:	break;
		case RT_LotPostRcp:	break;
		default:			return;
		}
	}while(0);
	//--------------------------------------------------------------

	// Add Recipe Name To File ////////////////////////////////////////////////
	char szRCPName[64]  = {0};
	_dREAD_STRING(m_nRcpNameIOPoint , szRCPName , &nIOStatus);
	// Add Recipe Name To File ////////////////////////////////////////////////

//	MSG msg;
	// printf("%s Manual Logging Start... \n" , m_szName);

	//File Path
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	char szFilePath[512] = {0};		// 2006.04.26
	char szFileName[256] = {0};		// 2006.04.26
	//strcpy (szFilePath , m_szFilePath);
	strcpy (szFilePath , "f:\\datalog\\_manuallog");
	strcat (szFilePath , "\\");
	strcat (szFilePath , m_szName);	
	// 2006.04.26
	sprintf(szFileName , "\\%04d_%02d_%02d_%02d_%02d_%02d_%s.log" , systime.wYear , systime.wMonth , systime.wDay , systime.wHour ,systime.wMinute , systime.wSecond ,szRCPName);
	strcat (szFilePath , szFileName);
	
	FILE*  fLogFile = NULL;
	fLogFile  = fopen(szFilePath , "a+");
	if(NULL == fLogFile)
	{
		printf("Log file %s open fail.. \n" , szFilePath);
		return;
	}
	else
	{
		if(false == WriteLogStart(fLogFile))
		{
			// printf("Log Skip because it is not Maintenance Recipe \n");
			if(fLogFile != NULL) fclose(fLogFile);
			return;
		}

	}

	//-----------------------------------------------------------------------------------

	/* ORG
	DWORD dwStartTick = 0;

	do
	{
		dwStartTick = GetTickCount();

		//PeekMessage(&msg,NULL, MSG_LOG_START, MSG_LOG_STOP, PM_REMOVE);

		//if(msg.message == MSG_LOG_STOP)
		//if(m_bLogRunning == false || msg.message == MSG_LOG_STOP)
		if(m_bLogRunning == false )
		{
			WriteLogEnd(fLogFile);
			if(fLogFile != NULL) fclose(fLogFile);
			break;		// 2006.09.18
		}

		//_Log(fLogFile);
		//Sleep(m_nLogPeriod);

		DoWaitLogSpanTime(dwStartTick);

		do
		{
			if(0 == strcmp(m_szName, "PMC"))	// for PMC version
			{
				if(RT_ProcRcp == nRcpType)
				{
					_Log(fLogFile);
				}
				else
				{
					break;
				}
			}
			else
			{
				_Log(fLogFile);
			}
		}while(0);


	}while(1);
	*/


	//////////////////////////////////////////////////////////////////////////
	DWORD  dwLogCounter = 0;
	DWORD  dwLogStartTime = 0;
	DWORD  dwCurrentTime = 0;

	dwLogStartTime = GetTickCount();

	do
	{
		if(m_bLogRunning == false )
		{
			WriteLogEnd(fLogFile);

			if(fLogFile != NULL)		fclose(fLogFile);
			printf("%s Manual Logging Stop... \n" , m_szName);
			return;
		}

		dwCurrentTime = GetTickCount();

		if (((dwLogCounter * m_nLogPeriod) + dwLogStartTime) <= dwCurrentTime) 
		{
			_Log(fLogFile);
			dwLogCounter++;
		}

		Sleep(1);

	}while(1);
	//////////////////////////////////////////////////////////////////////////

	if(fLogFile != NULL) fclose(fLogFile); // File Close

	//-----------------------------------------------------------------------------------
}
__finally
{
	m_bLoggingStartReady = TRUE;
//	printf(">>>[%s] Logging Stopped \n", m_szName);
}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : WriteLogStart()
//Date         : 2005.04.21
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDataLogger::WriteLogStart(FILE* pLogFile)
{
	bool bRet = true;

	int  nIOStatus;
	char szTimeStamp[64] = {0};

	memset(m_szRcpType , 0 , sizeof(m_szRcpType));
	memset(m_szRcpName , 0 , sizeof(m_szRcpName));
	
	int  nRcpType        = 0;
	int  nCommStatus     = 0;

	SYSTEMTIME systime;
	GetLocalTime(&systime);
	sprintf(szTimeStamp , "%.2d/%.2d %.2d:%.2d:%.2d:%.3d " , systime.wMonth , systime.wDay , systime.wHour , systime.wMinute , systime.wSecond , systime.wMilliseconds);


	nRcpType = _dREAD_DIGITAL(m_nRcpTypeIOPoint , &nIOStatus);
	_dREAD_STRING(m_nRcpNameIOPoint , m_szRcpName , &nIOStatus);

	nCommStatus = _dREAD_DIGITAL(m_nPMInusedIOPoint , &nIOStatus);
	printf("CommStatus = %d\n", nCommStatus);

	// 2006.09.14
	switch(nRcpType)
	{
	case RT_Unknown:	strcpy(m_szRcpType , "UnKnown"); return false;
	case RT_ProcRcp:	strcpy(m_szRcpType , "MANUAL Process Recipe"); break;
	case RT_PreProRcp:	strcpy(m_szRcpType , "PreProcess Recipe"); break;
	case RT_PostProRcp:	strcpy(m_szRcpType , "PostProcess Recipe"); break;
	case RT_IdleRcp:	strcpy(m_szRcpType , "Idle Recipe"); break;
	case RT_CleanRcp:	strcpy(m_szRcpType , "Cleaning Recipe"); break;
	case RT_PurgeRcp:	strcpy(m_szRcpType , "Purge Recipe"); break;
	case RT_LotPreRcp:	strcpy(m_szRcpType , "LotPre Recipe"); break;
	case RT_LotPostRcp:	strcpy(m_szRcpType , "LotPost Recipe"); break;
	default:			strcpy(m_szRcpType , "UnKnown"); return false;
	}

	fprintf(pLogFile , "%s --------------------------------------------------------------------------------------------\n" , szTimeStamp);
	fprintf(pLogFile , "%s LOG START : RECIPE TYPE : %s  | RECIPE NAME : %s\n" , szTimeStamp , m_szRcpType , m_szRcpName);
	fprintf(pLogFile , "%s --------------------------------------------------------------------------------------------\n" , szTimeStamp);
	
	//Item stamp
	fprintf(pLogFile , "%s " , szTimeStamp);
	for(int i = 0 ; i < m_nIOCount ; i++)
	{
		if(0 == strcmp(m_szName, "PMC"))	// for PMC version
		{
			fprintf(pLogFile , "PMC.%s\t" , m_IOList[i]->szIOName);
		}
		else
		{
			fprintf(pLogFile , "%s\t" , m_IOList[i]->szIOName);
		}

		//printf("Item %s \n" , m_IOList[i]->szIOName);
	}
	fprintf(pLogFile , "\n");
	fflush (pLogFile);

	return bRet;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : WriteLogEnd()
//Date         : 2005.04.21
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::WriteLogEnd  (FILE* pLogFile)
{
	char szTimeStamp[64] = {0};
	
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	sprintf(szTimeStamp , "%.2d/%.2d %.2d:%.2d:%.2d:%.3d " , systime.wMonth , systime.wDay , systime.wHour , systime.wMinute , systime.wSecond , systime.wMilliseconds);


	fprintf(pLogFile , "%s --------------------------------------------------------------------------------------------\n" , szTimeStamp);
	fprintf(pLogFile , "%s LOG STOP : RECIPE TYPE : %s  | RECIPE NAME : %s\n" , szTimeStamp , m_szRcpType , m_szRcpName);
	fprintf(pLogFile , "%s --------------------------------------------------------------------------------------------\n" , szTimeStamp);
	fflush (pLogFile);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : _Log()
//Date         : 2005.04.21
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::_Log(FILE* pLogFile)
{
	int  nIOStatus;
	char szStringIO[128] = {0};

	SYSTEMTIME systime;
	GetLocalTime(&systime);
	fprintf(pLogFile , "%.2d/%.2d %.2d:%.2d:%.2d:%.3d " , systime.wMonth , systime.wDay , systime.wHour , systime.wMinute , systime.wSecond , systime.wMilliseconds);

	//Item stamp
	for(int i = 0 ; i < m_nIOCount ; i++)
	{
		GetLocalTime(&systime);
		//printf("IO Printf Start : %.2d:%.2d:%.3d\n", systime.wMinute , systime.wSecond , systime.wMilliseconds);
		//Item stamp
		switch(m_IOList[i]->nIOType)
		{
		case _K_D_IO:
			fprintf(pLogFile , "%.3d\t" , _dREAD_DIGITAL(m_IOList[i]->nIOPoint , &nIOStatus));
			break;
		case _K_S_IO:
			_dREAD_STRING(m_IOList[i]->nIOPoint ,szStringIO, &nIOStatus);
			fprintf(pLogFile , "%s\t"   , szStringIO );
			break;
		case _K_A_IO:
			fprintf(pLogFile , "%.3f\t" , _dREAD_ANALOG(m_IOList[i]->nIOPoint , &nIOStatus));
			break;
		}	
		GetLocalTime(&systime);
		//printf("IO Printf End : %.2d:%.2d:%.3d\n", systime.wMinute , systime.wSecond , systime.wMilliseconds);
	}
	fprintf(pLogFile , "\n");
	fflush (pLogFile);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : _Log()
//Date         : 2006.10.27
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataLogger::DoWaitLogSpanTime(DWORD dwStartTick)
{
	//DWORD dwStartTick = 0;
	DWORD dwCurTick = 0;
	DWORD dwElapsedTick = 0;
	DWORD dwSpanTime = (DWORD) m_nLogPeriod;

	//dwStartTick = GetTickCount();

	do
	{
		dwCurTick = GetTickCount();

		// calcurate elapsed time
		if(dwCurTick >= dwStartTick)
		{
			dwElapsedTick = dwCurTick - dwStartTick;
		}
		else
		{
			dwElapsedTick = (0xffffffff + dwCurTick) - dwStartTick;
		}

		// wait span time to log
		if(dwElapsedTick >= dwSpanTime)
		{
			break;
		}
		else
		{
			Sleep(100);
		}
	}while(1);
	
	return;
}