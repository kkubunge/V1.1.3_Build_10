// DataProcessor.cpp: implementation of the CDataProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextParser.h"
#include "EthernetManager.h"
#include "IOInfo.h"
#include "DataProcessor.h"
#include "WindowInterface.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDataProcessor::CDataProcessor(CEthernetManager* pEthernetManager)
{
	if (NULL != pEthernetManager)
		m_pEthernetManager = pEthernetManager;

	for (int i = 0 ; i < MAX_IO; i++)
		m_pIOInfo[i] = NULL;

	memset(m_szFilePath , 0 , sizeof(m_szFilePath));

	m_enIOCount = 0;
	m_enLogCount= 0;
	m_fp        = NULL;

	m_nIOCM1JobID   = -1;
	m_nIOCM2JobID   = -1;
	m_nIOCM3JobID   = -1;
	m_nIOPMWaferSrc = -1;
	m_nIOPMWaferSts = -1;
}

CDataProcessor::~CDataProcessor()
{
	for (int i = 0 ; i < m_enIOCount; i++) {
		if (NULL != m_pIOInfo[i]) delete m_pIOInfo[i];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetWindowInterface()
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDataProcessor::SetWindowInterface(CWindowInterface* pWindowInterface)
{
	m_pWindowInterface = pWindowInterface;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetModuleName
//Date         : 2007.03.13
//Description  : 
//Arguments    :
//Return Value : char*
//////////////////////////////////////////////////////////////////////////////////////////////////////
char* CDataProcessor::GetModuleName()
{
	return m_pEthernetManager->GetModuleName();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitIOConfig
//Date         : 2007.03.13
//Description  : IO Get from IOList
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDataProcessor::InitIOConfig()
{
	bool bRet = true;
	long enCurMode = OTHERS;
	FILE *fp;
	char szRead        [256];
	char szItem		   [256];
	char szFile        [256];
	char szType        [32];
	memset(szFile , 0 , sizeof(szFile));

	if (NULL == m_pEthernetManager) return false;

	do {
		strcpy(szFile , IO_INFO_FILE);
		strcat(szFile , m_pEthernetManager->GetModuleName());
		strcat(szFile , "\\IO.cfg");
		
		fp = fopen(szFile, "rt");
		if (fp == NULL) {
			printf("file %s open fail \n" , szFile);
			bRet = FALSE; break;
		}

		if (NULL == fgets(szRead, 255, fp)) {
			printf("file %s fgets fail \n" , szFile);
			bRet = FALSE; break;
		}

		do {
			CTextParser tp(szRead);
			tp.GetWord(szItem, 0);
			if (szRead[0] == ';' || szRead[0] == '#' || szRead[0] == '\n' || szRead[0] == '\t') {
				//do nothing
			} else if(szItem[0] == '$') {
				//IO Type
				tp.GetWord(szType,0);
				if (0 == strcmp(szType , "Digital"))     enCurMode = DIGITAL;
				else if (0 == strcmp(szType , "Analog")) enCurMode = ANALOG;
				else enCurMode = OTHERS;
			} else {
				if (enCurMode == OTHERS) {
					//Do Nothing
				} else {
					//IO Type is digital or analog then create io
					if (m_enIOCount > MAX_IO) {
						printf ("Exceed Max IO Count Not Created any more...\n");
					} else {
						CIOInfo* pIOConfig = new CIOInfo(this);
						if (false == pIOConfig->CreateIO(enCurMode , szRead))
							delete pIOConfig;
						else {
							m_pIOInfo[m_enIOCount] = pIOConfig;
							m_enIOCount++;
						}
					}
				}
			}
			
			if (NULL == fgets(szRead, 255, fp)) //end of file
				break;
			
		} while(feof(fp) == 0);

		fclose(fp);
	} while (0);

	if (bRet == FALSE) {
		if (NULL != fp) fclose(fp);
	}
	return bRet;
}

bool CDataProcessor::InitIOPoints()
{
	bool bRet = false;
	do {
		if (NULL == m_pEthernetManager) return false;

		//Crete Base Folder
		sprintf(m_szFilePath , "D:\\$$IOMonitorLog");
		CreateDirectory(m_szFilePath , NULL);
		memset(m_szFilePath , 0 , sizeof(m_szFilePath));
		sprintf(m_szFilePath , "D:\\$$IOMonitorLog\\%s" , m_pEthernetManager->GetModuleName());
		CreateDirectory(m_szFilePath , NULL);
		
		//Initialize Using IO Point
		char szTempName[32] = {0};
		m_nIOCM1JobID = _FIND_FROM_STRING(_K_S_IO ,"CTC.JOB_NAME");
		if (-1 == m_nIOCM1JobID) break;
		m_nIOCM2JobID = _FIND_FROM_STRING(_K_S_IO ,"CTC.JOB_NAME2");
		if (-1 == m_nIOCM2JobID) break;
		m_nIOCM3JobID = _FIND_FROM_STRING(_K_S_IO ,"CTC.JOB_NAME3");
		if (-1 == m_nIOCM3JobID) break;

		sprintf(szTempName , "CTC.%s_Wafer_Source" , m_pEthernetManager->GetModuleName());
	    m_nIOPMWaferSrc = _FIND_FROM_STRING(_K_D_IO ,szTempName);
		if (-1 == m_nIOPMWaferSrc) break;

		memset (szTempName , 0 , sizeof(szTempName));
		sprintf(szTempName , "CTC.%s_Wafer_Status" , m_pEthernetManager->GetModuleName());
		m_nIOPMWaferSts = _FIND_FROM_STRING(_K_D_IO ,szTempName);
		if (-1 == m_nIOPMWaferSts) break;

		bRet = true;
	} while (0);

	return bRet;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetConnectionMessage()
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CDataProcessor::GetConnectionMessage()
{
	bool bRet = false;
	do {
		MSG_INFO sMsgInfo;
		strcpy(sMsgInfo.szModuleName , GetModuleName());
		sMsgInfo.enNetStatus  = CONNECTED;
		m_pWindowInterface->SendIOMessage(&sMsgInfo);
		bRet = true;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetDisconnectionMessage()
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CDataProcessor::GetDisconnectionMessage()
{
	bool bRet = false;
	do {
		MSG_INFO sMsgInfo;
		strcpy(sMsgInfo.szModuleName , GetModuleName());
		sMsgInfo.enNetStatus  = DISCONNECTED;
		m_pWindowInterface->SendIOMessage(&sMsgInfo);
		bRet = true;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetLogStartMessage()
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CDataProcessor::GetLogStartMessage(IO_DATA_STRUCT* sIOData)
{
	bool bRet = false;
	do {
		//Get Recipe Name & Create File Based on ?
		//Check if not close before file
		if (NULL != m_fp) {
			fclose(m_fp); m_fp = NULL;
		}

		m_enLogCount = 0;

		//open new file for log
		///////////////////////////////////////////////////////////////////////////////////////////////
		char szFilePath [256] = {0};
		char szJobID    [256] = {0};
		char szTimeStamp[256] = {0};
		SYSTEMTIME sysTime;
		int        nWaferSrc , nWaferSts , nIOStatus;
		GetLocalTime(&sysTime);
		nWaferSrc = _dREAD_DIGITAL(m_nIOPMWaferSrc , &nIOStatus);
		nWaferSts = _dREAD_DIGITAL(m_nIOPMWaferSts , &nIOStatus);

		//Get JobID 
		switch (nWaferSrc) {
		case 0: //CM1
			_dREAD_STRING(m_nIOCM1JobID , szJobID , &nIOStatus);			
			break;
		case 1: //CM2
			_dREAD_STRING(m_nIOCM2JobID , szJobID , &nIOStatus);			
			break;
		case 2: //CM3
			_dREAD_STRING(m_nIOCM3JobID , szJobID , &nIOStatus);			
			break;
		default:
			break;
		}	

		//Create Log Folder Details
		sprintf(szFilePath , "%s\\%d" , m_szFilePath , sysTime.wYear);
		CreateDirectory(szFilePath , NULL);
		memset(szFilePath , 0 , sizeof(szFilePath));
		sprintf(szFilePath , "%s\\%d\\%d" , m_szFilePath , sysTime.wYear , sysTime.wMonth);
		CreateDirectory(szFilePath , NULL);
		memset(szFilePath , 0 , sizeof(szFilePath));
		sprintf(szFilePath , "%s\\%d\\%d\\%d" , m_szFilePath , sysTime.wYear , sysTime.wMonth , sysTime.wDay);
		CreateDirectory(szFilePath , NULL);
		memset(szFilePath , 0 , sizeof(szFilePath));
		sprintf(szFilePath , "%s\\%d\\%d\\%d\\%s" , m_szFilePath , sysTime.wYear , sysTime.wMonth , sysTime.wDay , szJobID);
		
		if ("" != szJobID) {
			CreateDirectory(szFilePath , NULL);
			memset(szFilePath , 0 , sizeof(szFilePath));
			sprintf(szFilePath , "%s\\%d\\%d\\%d\\%s\\W%d_%2d%2d%2d.log" , m_szFilePath , sysTime.wYear
				                                                                         , sysTime.wMonth
																						 , sysTime.wDay
																						 , szJobID
																						 , nWaferSts
																						 , sysTime.wHour
																						 , sysTime.wMinute
																						 , sysTime.wSecond);
		} else {
			memset(szFilePath , 0 , sizeof(szFilePath));
			sprintf(szFilePath , "%s\\%d\\%d\\%d\\W%d_%2d%2d%2d.log" , m_szFilePath , sysTime.wYear
				                                                                         , sysTime.wMonth
																						 , sysTime.wDay
																						 , nWaferSts
																						 , sysTime.wHour
																						 , sysTime.wMinute
																						 , sysTime.wSecond);
		}

		m_fp  = fopen(szFilePath , "a+");
		sprintf(szTimeStamp , "%.2d:%.2d:%.2d:%.2d:%.2d:%.3d " , sysTime.wMonth , sysTime.wDay , sysTime.wHour , sysTime.wMinute , sysTime.wSecond , sysTime.wMilliseconds);
		if (NULL != m_fp) {
			fprintf(m_fp , "#%s --------------------------------------------------------------------------------------------\n" , szTimeStamp);
			fprintf(m_fp , "#%s LOG START : RECIPE NAME : %s\n" , szTimeStamp , sIOData->szRecipeName);
			fprintf(m_fp , "#%s --------------------------------------------------------------------------------------------\n" , szTimeStamp);
			fprintf(m_fp , "$%s\t" , szTimeStamp);
			CIOInfo* pInfo = NULL;
			for (int i = 0 ; i < m_enIOCount; i++) {
				pInfo = m_pIOInfo[i];
				if (NULL != pInfo) {
					fprintf(m_fp , "%s\t" , pInfo->GetIOName());
				}				
			}
			fprintf(m_fp , "\n");
		} else printf ("file open error %s \n" , szFilePath);
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		
		MSG_INFO sMsgInfo;
		strcpy(sMsgInfo.szModuleName , GetModuleName());
		sMsgInfo.enNetStatus  = POLL_START;
		m_pWindowInterface->SendIOMessage(&sMsgInfo);

		bRet = true;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetLogStopMessage()
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CDataProcessor::GetLogStopMessage (IO_DATA_STRUCT* sIOData)
{
	bool bRet = false;
	do {
		//File Close
		if (NULL != m_fp) {
			fclose(m_fp); m_fp = NULL;
		}

		m_enLogCount = 0;

		MSG_INFO sMsgInfo;
		strcpy(sMsgInfo.szModuleName , GetModuleName());
		sMsgInfo.enNetStatus  = POLL_STOP;
		m_pWindowInterface->SendIOMessage(&sMsgInfo);

		bRet = true;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetDNetFailMessage()
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CDataProcessor::GetDNetFailMessage(IO_DATA_STRUCT* sIOData)
{
	bool bRet = false;
	do {
		MSG_INFO sMsgInfo;
		strcpy(sMsgInfo.szModuleName , GetModuleName());
		sMsgInfo.enNetStatus  = DNET_FAIL;
		m_pWindowInterface->SendIOMessage(&sMsgInfo);

		bRet = true;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetNormalIOMessage
//Date         : 2007.03.14
//Description  : 
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CDataProcessor::GetNormalIOMessage(IO_DATA_STRUCT* sIOData)
{
	bool bRet = false;
	do {
		if (m_enLogCount >= MAX_LOG_COUNT) {bRet = true; break;}

		CIOInfo* pInfo = NULL;
		MSG_INFO sMsgInfo;
		strcpy(sMsgInfo.szModuleName , GetModuleName());
		sMsgInfo.enNetStatus  = IO_NORMAL;

		//File Log
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		char       szTimeStamp[256] = {0};
		sprintf(szTimeStamp , "%.2d:%.2d:%.2d:%.2d:%.2d:%.3d " , sysTime.wMonth , sysTime.wDay , sysTime.wHour , sysTime.wMinute , sysTime.wSecond , sysTime.wMilliseconds);
		if (NULL != m_fp) fprintf(m_fp , "%s\t" , szTimeStamp);
		
		//IO Process
		for (int i = 0 ; i < m_enIOCount; i++) {
			pInfo = m_pIOInfo[i];
			if (NULL != pInfo) {
				pInfo->CalCurrentIOValue(sIOData->ucIOMemData);
				strcpy(sMsgInfo.sMsgIO[i].szIOName , pInfo->GetIOName());
				sMsgInfo.sMsgIO[i].enIOType  = pInfo->GetIOType();
				sMsgInfo.sMsgIO[i].nIOValue  = pInfo->GetIntValue();
				sMsgInfo.sMsgIO[i].dbIOValue = pInfo->GetDBValue();
				sMsgInfo.nIOCount++;

				//File Log IO Details
				if (NULL != m_fp) {
					if (DIGITAL == pInfo->GetIOType())
						fprintf(m_fp , "%6d\t" , pInfo->GetIntValue());
					else if (ANALOG == pInfo->GetIOType())
						fprintf(m_fp , "%3.2f\t" , pInfo->GetDBValue());			
				}
				//End file Log
			}
		}

		//LF
		if (NULL != m_fp) fprintf(m_fp , "\n");

		m_pWindowInterface->SendIOMessage(&sMsgInfo);
		m_enLogCount++;
		bRet = true;
	} while (0);

	return bRet;
}