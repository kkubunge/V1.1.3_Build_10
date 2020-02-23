// For Sysmac Gateway
#include "EvtMem.h"					//Exchange IO

#include "CIPCoreAPI.h"				//LanPort Open
#include "CIPCorePortAPI.h"			//LanPort Open
#include "CIPCoreApiRess.h"			//LanPort Open
#include "CIPCorePortRess.h"		//LanPort Open
#include <math.h>
//==========================================================================================
extern "C"
{
#include <stdio.h>
#include "driver.h"
//------------------------------------------------------------------------------------

} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll_M.h"

//Definition----------------------------------------------------------------------
#define MAX_ADDRESS						 2000

//--------------------------------------------------------------------------------
//2011/06/29
#define READ_IO_POLL_TIME	20				// 2012.03.08
DWORD	g_dwIOUpdateTime = 0;				// 2012.03.09
BOOL    g_bIOUpdate = FALSE;				// 2012.03.08
int		g_nUpdatePassCount = 0;
int		g_nUpdatePassMax = 0;

//EIP---------------------------------------------------------------------
HANDLE		g_hIN_Mem = NULL;
HANDLE		g_hOUT_Mem = NULL;				// 2012.08.02
HANDLE		g_hVrcpMem = NULL;
DWORD		g_nErrMsg;

//--------------------------------------------------------------------------------

//Buffer Member-------------------------------------------------------------------
#define		IN_DATA_ADDR_wOFFS			0		//word		// 설비별 실제 MemoryIndex 확인 필요..
#define		OUT_DATA_ADDR_wOFFS			500		//word		// 설비별 실제 MemoryIndex 확인 필요..

//---> Modify 2014.04.02 by KSM
#define		IN_DATA_btSIZE				1000	//byte		// 설비별 실제 MemoryIndex 확인 필요.. (Base Address 0, 500)
#define		OUT_DATA_btSIZE				600		//byte		//(Output Tag Size와 동일해야 한다.)


//For Valve Recipe Download
#define		VRCP_PAGE1_wOFFS			0			// EM0
#define		VRCP_PAGE2_wOFFS			200			// EM0
#define		VRCP_PAGE3_wOFFS			400			// EM0
#define		VRCP_PAGE4_wOFFS			600			// EM0
#define		VRCP_PAGE5_wOFFS			800			// EM0

#define		VRCP_HEADER_wOFFS			100			//word

#define		VRCP_PAGE_btSIZE			200			//byte

#define		MAX_SUB_STEP				1			// 10 -> 1     PLC EIP have one VlvStep       by KSM
#define		MAX_MEM_RCP_BUFFER			5

#define		STEP_HEADER_btSIZE			100
//... 2017.03.23
//... MFC Info size
#define     MFC_INFO_bytSIZE			4000		//word
#define		MAX_MFC_NUM					9			// Total 9 MFC update to EMO area from PLC if S/W Control Flag I/O 
#define		MAX_MFC_INFO				9			// This MFC Support 9 information
#define		MAX_ITEM_DATASIZE			30			// 15word (30byte)
#define		MAX_MFC_DATASIZE			400
//... MFC Off set
#define     MFC01_INFO_wOFFS			12000
#define     MFC02_INFO_wOFFS			12200
#define     MFC03_INFO_wOFFS			12400
#define     MFC04_INFO_wOFFS			12600
#define     MFC05_INFO_wOFFS			12800
#define     MFC06_INFO_wOFFS			13000
#define     MFC07_INFO_wOFFS			13200
#define     MFC08_INFO_wOFFS			13400
#define     MFC09_INFO_wOFFS			13600
#define     MFC10_INFO_wOFFS			13800

char	g_szMFCManufacSI[10][400] = {0,};
char	g_szMFCModelNumSI[10][400] = {0,};
char	g_szMFCSWVersionSI[10][400] = {0,};
char	g_szMFCHWVersionSI[10][400] = {0,};
char	g_szMFCSerialNumSI[10][400] = {0,};
char	g_szMFCGasNumSI[10][400] = {0,};
char	g_szMFCGasSymbolSI[10][400] = {0,};
char	g_szMFCFullScaleSI[10][400] = {0,};
char	g_szMFCOffesetSI[10][400] = {0,};

enum	{
	eMFC_MANUFAC,
	eMFC_MODELNUM,
	eMFC_SWVER,
	eMFC_HWVER,
	eMFC_SERNUM,
	eMFC_GASNUM,
	eMFC_GASSYM,
	eMFC_FULLSCAL,
	eMFC_OFFSET_B,
};
//... 		

int		g_nVRCPBufPage = 0;


unsigned char  g_abInData[IN_DATA_btSIZE]	= {0,};
unsigned char  g_abOutData[OUT_DATA_btSIZE] = {0,};
WORD           g_abBUFFER[MAX_ADDRESS];
//... 2017.03.23
unsigned char  g_abMFCInfoData[MFC_INFO_bytSIZE] = {0,};
//...
unsigned char g_pSubStepBuf[VRCP_PAGE_btSIZE];
unsigned char g_pStepHeaderBuf[STEP_HEADER_btSIZE];
BOOL g_bIsOkMFCInfo	= FALSE;
//--------------------------------------------------------------------------------


//Variable------------------------------------------------------------------------
enum { EIP_OFFLINE, EIP_ONLINE };
int            g_nCommSts		= EIP_OFFLINE;

//--------------------------------------------------------------------------------
// 2012.03.08
double g_adbOffset[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


//-----------------------------------------------------------------------------------------------------------
void printf_LOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);
	
	printf(_szString);
	_LOG(_szString);
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
// Shared Memory
#define	S_MEMORY_SIZE	2100
#define VRCP_BUF_SIZE	2100

#define SM_FILE_NAME	"VRCP_DATA_FILE"

HANDLE g_hFileHandle = NULL;
LPVOID g_pViewOfFile = NULL;
char g_szErrDesc[256];
unsigned char g_pVRCPData[VRCP_BUF_SIZE] = {0,};
BOOL g_bSM_Result = FALSE;

BOOL CreateSharedMemory()
{
	char szLog[256];

	g_bSM_Result = FALSE;
	if(g_hFileHandle != NULL)
	{
		sprintf(g_szErrDesc, "FileMapping is already opened");
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		printf_LOG("%s\n", szLog);
		return FALSE;
	}

	g_hFileHandle = CreateFileMapping(	INVALID_HANDLE_VALUE,               // handle to file
										NULL,								// security
										PAGE_READWRITE,                     // protection
										0,						            // high-order DWORD of size
										S_MEMORY_SIZE,				            // low-order DWORD of size
										SM_FILE_NAME	  	                    // object name
									 );

	if (NULL == g_hFileHandle)
	{
		sprintf(g_szErrDesc, "Unable to create file mapping. Error is %d", ((BYTE)((WORD)GetLastError())));		//Get low byte
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		printf_LOG("%s\n", szLog);
		return FALSE;
	}

	g_pViewOfFile = MapViewOfFile(	g_hFileHandle,							// handle to file-mapping object
									FILE_MAP_ALL_ACCESS,					// access mode
									0,										// high-order DWORD of offset
									0,										// low-order DWORD of offset
									0										// number of bytes to map
								);
	if (NULL == g_pViewOfFile)
	{
		sprintf(g_szErrDesc, "Unable to map to the current address space. Error is %d", ((BYTE)((WORD)GetLastError())));		//Get low byte
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		printf_LOG("%s\n", szLog);
		return FALSE;
	}

	sprintf(szLog, "-----> Create Shared Memory Successfully");
	printf_LOG("%s\n", szLog);
	g_bSM_Result = TRUE;
	return TRUE;
}

BOOL FreeSharedMemory()
{
	BOOL bRet = TRUE;
	if (g_pViewOfFile != NULL)
	{
		if(FALSE == UnmapViewOfFile(g_pViewOfFile))
		{
			sprintf(g_szErrDesc, "Unable to unmap view of file. Error is %d", ((BYTE)((WORD)GetLastError())));		//Get low byte
			bRet = FALSE;
		}
	}
	if (NULL != g_hFileHandle)
		CloseHandle(g_hFileHandle);
	g_hFileHandle = NULL;

	g_bSM_Result = FALSE;
	return bRet;
}

BOOL ReadFromSharedMemory()
{
	char szLog[256];
	char szBuf[256];
	int i;

	if(g_hFileHandle == NULL)
	{
		sprintf(g_szErrDesc, "FileMapping is not opened");
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		printf_LOG("%s\n", szLog);
		g_bSM_Result = FALSE;
		return FALSE;
	}

	memcpy(g_pVRCPData, (char*)g_pViewOfFile, S_MEMORY_SIZE);
	g_bSM_Result = TRUE;
	_LOG("-----> Read From Shared Memory Successfully");

	// Packet Data Log
	strcpy(szLog, "-----> Valve Packet Data :");
	_LOG(szLog);
	strcpy(szLog, "");
	for(i=0; i<S_MEMORY_SIZE; i++)
	{
		if((i % 24) == 0 && i != 0)
		{
			_LOG(szLog);
			strcpy(szLog, "");
		}
		sprintf(szBuf, " %02X", *(g_pVRCPData+i));
		strcat(szLog, szBuf);
	}
	_LOG(szLog);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//... 2017.03.30
void MFCInfoIOUpdate(int nIdexMFCInfo, int nIdexMFCNum, char *pMFCData, int nSize)
{
	switch(nIdexMFCInfo)
	{
	case eMFC_MANUFAC:
		strncpy(g_szMFCManufacSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_MODELNUM:
		strncpy(g_szMFCModelNumSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_SWVER:
		strncpy(g_szMFCSWVersionSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_HWVER:
		strncpy(g_szMFCHWVersionSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_SERNUM:
		strncpy(g_szMFCSerialNumSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_GASNUM:
		strncpy(g_szMFCGasNumSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_GASSYM:
		strncpy(g_szMFCGasSymbolSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_FULLSCAL:
		strncpy(g_szMFCFullScaleSI[nIdexMFCNum],pMFCData,nSize);
		break;
	case eMFC_OFFSET_B:
		strncpy(g_szMFCOffesetSI[nIdexMFCNum],pMFCData,nSize);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//... 2017.03.30
BOOL IsOkHeaderData(int nIdexDataSize,unsigned char ucHexTemp)
{
	BOOL bRet = TRUE;
	//... start code check
	if(nIdexDataSize == 0)
	{
		//printf("[cHexTemp %d] %02x \n",nIdexDataSize,ucHexTemp);
		if(ucHexTemp != 0x28) return FALSE;
	}
	else if(nIdexDataSize == 1)
	{
		//printf("[cHexTemp %d] %02x \n",nIdexDataSize,ucHexTemp);
		if(ucHexTemp != 0x01) return FALSE;
	}
	else if(nIdexDataSize == 2)
	{
		//printf("[cHexTemp %d] %02x \n",nIdexDataSize,ucHexTemp);
		if(ucHexTemp != 0x00) return FALSE;
	}
	else if(nIdexDataSize == 3)
	{
		//printf("[cHexTemp %d] %02x \n",nIdexDataSize,ucHexTemp);
		if(ucHexTemp != 0x00) return FALSE;
	}
	else
	{
		//... Heater Data Ok
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL AnalyzeMFCHexCode()
{
	BOOL bRet = TRUE;
	int nReadDataSize = 0;
	char szTemp[400] = {0,};
	unsigned char ucHexTemp = 0;
	char szMFCInfo[400] = {0,};
	int nIdexMFCNum = 0,nIdexMFCInfo = 0,nIdexDataSize = 0;
	int nCnt = 0;
	int nMFCGasStNum = 0;
	int nMFCOffSet = 0;
	int nMFCFullScal = 0;
	char szMFCOffSet[8] = {0,};
	char szMFCFullScal[256] = {0,};
	BYTE* pbyTemp = NULL;
	DWORD dwDataReading = 0;
	int nIndex = 0;
	float fData = 0;
	double dbDdta = 0;

	memset(g_szMFCManufacSI,0,sizeof(g_szMFCManufacSI));
	memset(g_szMFCModelNumSI,0,sizeof(g_szMFCModelNumSI));
	memset(g_szMFCSWVersionSI,0,sizeof(g_szMFCSWVersionSI));
	memset(g_szMFCHWVersionSI,0,sizeof(g_szMFCHWVersionSI));
	memset(g_szMFCSerialNumSI,0,sizeof(g_szMFCSerialNumSI));
	memset(g_szMFCGasNumSI,0,sizeof(g_szMFCGasNumSI));
	memset(g_szMFCGasSymbolSI,0,sizeof(g_szMFCGasSymbolSI));
	memset(g_szMFCFullScaleSI,0,sizeof(g_szMFCFullScaleSI));
	memset(g_szMFCOffesetSI,0,sizeof(g_szMFCOffesetSI));

	for(nIdexMFCNum = 0; nIdexMFCNum < MAX_MFC_NUM; nIdexMFCNum++)
	{
		for(nIdexMFCInfo = 0; nIdexMFCInfo < MAX_MFC_INFO; nIdexMFCInfo++)
		{
			memset(szMFCInfo,0,sizeof(szMFCInfo));
			nCnt = 0;
			for(nIdexDataSize = 0; nIdexDataSize <MAX_ITEM_DATASIZE; nIdexDataSize++)
			{
				sprintf(szTemp,"%c",g_abMFCInfoData[nIdexDataSize + (MAX_ITEM_DATASIZE*nIdexMFCInfo) + (nIdexMFCNum*MAX_MFC_DATASIZE)]);
				ucHexTemp = (BYTE)g_abMFCInfoData[nIdexDataSize + (MAX_ITEM_DATASIZE*nIdexMFCInfo) + (nIdexMFCNum*MAX_MFC_DATASIZE)];

				if(!IsOkHeaderData(nIdexDataSize,ucHexTemp))
				{
					printf("[MFC Diagnosis Header error] MFC:%d MFCInfoNum:%d DataIndex:%d Header:%02x \n",nIdexMFCNum,nIdexMFCInfo,nIdexDataSize,ucHexTemp);
					return FALSE;
				}

				//////////////////////////////////////////////////////////////////////////
				//... Data size Check
				if(nIdexDataSize == 5)
				{
					nReadDataSize = g_abMFCInfoData[nIdexDataSize + (MAX_ITEM_DATASIZE*nIdexMFCInfo) + (nIdexMFCNum*MAX_MFC_DATASIZE)];
					if(nReadDataSize <= 0)
					{
						printf("[MFC Diagnosis] == Read Data Size Error MFC%d MFC Info%d\n",nIdexMFCNum,nIdexMFCInfo);
						break;
					}
				}
				//////////////////////////////////////////////////////////////////////////
				//... Data Gathering
				if(nIdexMFCInfo == eMFC_GASNUM && nIdexDataSize == 8) 
				{
					nMFCGasStNum = g_abMFCInfoData[nIdexDataSize + (MAX_ITEM_DATASIZE*nIdexMFCInfo) + (nIdexMFCNum*MAX_MFC_DATASIZE)];
					sprintf(szMFCInfo,"%d",nMFCGasStNum);
				}
				else if(nIdexMFCInfo == eMFC_OFFSET_B && nIdexDataSize == 8) 
				{
					/*
					for(int i = (nReadDataSize - 3); i >= 0; i--)
					{
						memset(szMFCOffSet,0,sizeof(szMFCOffSet));
						nMFCOffSet = g_abMFCInfoData[(nIdexDataSize + i) + (MAX_ITEM_DATASIZE*nIdexMFCInfo) + (nIdexMFCNum*MAX_MFC_DATASIZE)];
						sprintf(szMFCOffSet,"%d",nMFCOffSet);
						strcat(szMFCInfo,szMFCOffSet);
					}
					*/
					//... plc에서 받는게 없다 임시로 "0"
					strcpy(szMFCInfo,"0.000");
				}
				else if(nIdexMFCInfo == eMFC_FULLSCAL && nIdexDataSize == 8)
				{
					nIndex = 3;
					pbyTemp = (BYTE*)&dwDataReading;

					for(int j = (nReadDataSize - 5) ; j >= 0; j--)
					{
						memset(szMFCFullScal,0,sizeof(szMFCFullScal));
						nMFCFullScal = g_abMFCInfoData[(nIdexDataSize + j) + (MAX_ITEM_DATASIZE*nIdexMFCInfo) + (nIdexMFCNum*MAX_MFC_DATASIZE)];
						pbyTemp[nIndex--] = (BYTE)nMFCFullScal;
					}
				
					memcpy(&fData, &dwDataReading, sizeof(dwDataReading));

					sprintf(szMFCFullScal,"%.03f",fData);
					strcpy(szMFCInfo,szMFCFullScal);
				}
				else if(nIdexDataSize > 8 && (nIdexMFCInfo != eMFC_FULLSCAL && nIdexMFCInfo != eMFC_OFFSET_B && nIdexMFCInfo != eMFC_GASNUM))
				{
					if((nReadDataSize-3) >= nCnt++) strcat(szMFCInfo,szTemp);
				}
				else
				{
					//... Unknown data
				}
			}
			//////////////////////////////////////////////////////////////////////////
			//... I/O Update
			
			MFCInfoIOUpdate(nIdexMFCInfo,nIdexMFCNum,szMFCInfo,sizeof(szMFCInfo));
			//////////////////////////////////////////////////////////////////////////
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2017.03.23
BOOL READ_MFC_Info()
{
    BOOL bRet = TRUE;
	EM_ADDRESS	eMFCAddress;
	int nErrMsg;
	int nIdex = 0;

	eMFCAddress.byTypeOfFactor = EM_BYTE_TYPE; //EM_BIT_TYPE, EM_BYTE_TYPE, EM_WORD_TYPE, EM_DWORD_TYPE, EM_NO_DATA 중 선택
	eMFCAddress.byLocateOnWord = EM_BYTE_HIGH; // 비트일때(0~15) 바이트일때(EM_BYTE_HIGH 또는 EM_BYTE_LOW)
	eMFCAddress.dwNumberOfFactors = MFC_INFO_bytSIZE; // 가져올 갯수
	eMFCAddress.dwWordOffset = MFC01_INFO_wOFFS;   // This is Start_Point for Reading Memory about MFC information

	bRet = Em_readMemory(g_hVrcpMem, &eMFCAddress, g_abMFCInfoData, sizeof(g_abMFCInfoData));
	if(bRet == FALSE) 
	{ 
		nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
		printf_LOG("[PLC EIP] MFC Info readMemory Failed(Error = %d)\n", nErrMsg);
		return FALSE;
	}
	if(!AnalyzeMFCHexCode())
	{
		printf("[MFC diagnosis DataRead Error]___________________\n");
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL SetSMStepData()
{
    BOOL bRet = TRUE;
	EM_ADDRESS	omAddress;
	int nErrMsg;

	// Set Sub Step Data (Valve Data)
	memcpy(g_pSubStepBuf, g_pVRCPData, VRCP_PAGE_btSIZE);

	// Set Step Header Data
	memcpy(g_pStepHeaderBuf, g_pVRCPData + MAX_SUB_STEP * VRCP_PAGE_btSIZE, STEP_HEADER_btSIZE);

	//------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------
	g_nVRCPBufPage = *g_pStepHeaderBuf;
	printf("[PLC EIP] writeMemory SetSMStepData(g_nVRCPBufPage = %d)\n", g_nVRCPBufPage);
	//------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------
	
	// Download
	omAddress.byTypeOfFactor = EM_BYTE_TYPE; //EM_BIT_TYPE, EM_BYTE_TYPE, EM_WORD_TYPE, EM_DWORD_TYPE, EM_NO_DATA 중 선택
	omAddress.byLocateOnWord = EM_BYTE_HIGH; // 비트일때(0~15) 바이트일때(EM_BYTE_HIGH 또는 EM_BYTE_LOW)
	omAddress.dwNumberOfFactors = VRCP_PAGE_btSIZE; // 가져올 갯수


	switch(g_nVRCPBufPage)
	{
	case 0x00:
				omAddress.dwWordOffset = VRCP_PAGE1_wOFFS; // 읽을 주소 지정
				break;
	case 0x01:
				omAddress.dwWordOffset = VRCP_PAGE2_wOFFS; // 읽을 주소 지정
				break;
	case 0x02:
				omAddress.dwWordOffset = VRCP_PAGE3_wOFFS; // 읽을 주소 지정
				break;
	case 0x03:
				omAddress.dwWordOffset = VRCP_PAGE4_wOFFS; // 읽을 주소 지정
				break;
	case 0x04:
				omAddress.dwWordOffset = VRCP_PAGE5_wOFFS; // 읽을 주소 지정
				break;

	default:
				printf_LOG("[PLC EIP] writeMemory SetSMStepData Fail(BufPage is error[%d])\n", g_nVRCPBufPage);
				return FALSE;
	}
	
	//2014.01.16  ksm comment : Write SubStep  200byte        주소 제외 :  g_pSubStepBuf -> g_pSubStepBuf+2
	
	bRet = Em_writeMemory(g_hVrcpMem, &omAddress, g_pSubStepBuf+2, sizeof(g_pSubStepBuf));	// must  dwNumberOfFactors(byte) <= sizeof(byte)
	if(bRet == FALSE) 
	{ 
		nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
		printf_LOG("[PLC EIP] writeMemory Failed(g_hVrcpMem,g_pSubStepBuf)(Error = %d)\n", nErrMsg);
		return FALSE;
	}


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------


	//2014.01.16  ksm comment : Write StepHeader  100byte        주소 제외 :  g_pStepHeaderBuf -> g_pStepHeaderBuf+2 

	omAddress.dwWordOffset += (VRCP_HEADER_wOFFS);		//Unit : word			//필수
	omAddress.dwNumberOfFactors = STEP_HEADER_btSIZE;	//Unit : byte			//필수
	bRet = Em_writeMemory(g_hVrcpMem, &omAddress, g_pStepHeaderBuf+2, sizeof(g_pStepHeaderBuf));	// must  dwNumberOfFactors(byte) <= sizeof(byte)
	if(bRet == FALSE) 
	{ 
		nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
		printf_LOG("[PLC EIP] writeMemory Failed(g_hVrcpMem,g_pStepHeaderBuf)(Error = %d)\n", nErrMsg);
		return FALSE;
	}

	return TRUE;
}


BOOL ClearSMStepData()
{
    BOOL bRet = TRUE;
	EM_ADDRESS	omAddress;
	int nErrMsg;
	char szBuf[VRCP_PAGE_btSIZE*2*MAX_MEM_RCP_BUFFER] = {0,};			//2000 byte
	
	printf("[PLC EIP] writeMemory Clear(RecipeMem)\n");
	
	// Download
	omAddress.byTypeOfFactor = EM_BYTE_TYPE; //EM_BIT_TYPE, EM_BYTE_TYPE, EM_WORD_TYPE, EM_DWORD_TYPE, EM_NO_DATA 중 선택
	omAddress.byLocateOnWord = EM_BYTE_HIGH; // 비트일때(0~15) 바이트일때(EM_BYTE_HIGH 또는 EM_BYTE_LOW)
	omAddress.dwNumberOfFactors = VRCP_PAGE_btSIZE*2*MAX_MEM_RCP_BUFFER; // 가져올 갯수	
	omAddress.dwWordOffset = VRCP_PAGE1_wOFFS; // 읽을 주소 지정

	bRet = Em_writeMemory(g_hVrcpMem, &omAddress, szBuf, sizeof(szBuf));	// must  dwNumberOfFactors(byte) <= sizeof(byte)
	if(bRet == FALSE) 
	{ 
		nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
		printf_LOG("[PLC EIP] writeMemory Clear Fail(RecipeMem)(Error = %d)\n", nErrMsg);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------------------------------------
//Name        : ReadIO
//Date        :
//Return      : BOOL
//Description : Read datum from device net card
//-----------------------------------------------------------------------------------------------------------
BOOL ReadIO(void)
{
    BOOL bRet = TRUE;
	EM_ADDRESS	omAddress;
	int nErrMsg;

	//2011/06/29
	// 2012.03.09
	DWORD dwCurrentTime = 0;
	DWORD dwElapsedTime = 0;

	//////////////////////////////////////////////////////////////////////////
	//... TickCount Period Update Check
	//2011/06/29
	dwCurrentTime = GetTickCount();
	dwElapsedTime = labs(dwCurrentTime - g_dwIOUpdateTime);

	// 2012.03.08
	// 2012.03.09
	if(dwElapsedTime < READ_IO_POLL_TIME && FALSE == g_bIOUpdate)
	{
		g_nUpdatePassCount++;
		if(g_nUpdatePassCount > g_nUpdatePassMax) g_nUpdatePassMax = g_nUpdatePassCount;
		return TRUE;
	}

	g_nUpdatePassCount = 0;
	g_dwIOUpdateTime = dwCurrentTime;
	g_bIOUpdate = FALSE;	// 2012.03.08


	omAddress.byTypeOfFactor = EM_BYTE_TYPE; //EM_BIT_TYPE, EM_BYTE_TYPE, EM_WORD_TYPE, EM_DWORD_TYPE, EM_NO_DATA 중 선택
	omAddress.byLocateOnWord = EM_BYTE_HIGH; // 비트일때(0~15) 바이트일때(EM_BYTE_HIGH 또는 EM_BYTE_LOW)
	omAddress.dwWordOffset = IN_DATA_ADDR_wOFFS; // 읽을 주소 지정
	omAddress.dwNumberOfFactors = IN_DATA_btSIZE; // 가져올 갯수

	bRet = Em_readMemory(g_hIN_Mem, &omAddress, g_abInData, sizeof(g_abInData));
	if(bRet == FALSE) 
	{ 
		nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
		printf_LOG("[PLC EIP] readMemory Failed(Error = %d)\n", nErrMsg);
		return FALSE;
	}


    g_nCommSts = EIP_ONLINE;
    return TRUE;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : WriteIO
//Date        :
//Return      : BOOL
//Description : Write data to device net card
//-----------------------------------------------------------------------------------------------------------
BOOL WriteIO(void)
{
    BOOL bRet = TRUE;
	EM_ADDRESS	omAddress;
	int nErrMsg;

	omAddress.byTypeOfFactor = EM_BYTE_TYPE; //EM_BIT_TYPE, EM_BYTE_TYPE, EM_WORD_TYPE, EM_DWORD_TYPE, EM_NO_DATA 중 선택
	omAddress.byLocateOnWord = EM_BYTE_HIGH; // 비트일때(0~15) 바이트일때(EM_BYTE_HIGH 또는 EM_BYTE_LOW)
	omAddress.dwWordOffset = OUT_DATA_ADDR_wOFFS; // 읽을 주소 지정
	omAddress.dwNumberOfFactors = OUT_DATA_btSIZE; // 가져올 갯수

	bRet = Em_writeMemory(g_hOUT_Mem, &omAddress, g_abOutData, sizeof(g_abOutData));	// must  dwNumberOfFactors(byte) <= sizeof(byte)
	if(bRet == FALSE) 
	{ 
		nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
		printf_LOG("[PLC EIP] writeMemory Failed.(WriteIO)(Error = %d)\n", nErrMsg);

		if ( nErrMsg == 27 ) {
			printf_LOG("[PLC EIP](Err=%d)(The size of the buffer for reading data is insufficient)\n", nErrMsg);
		}
		else if ( nErrMsg == 29 ) {
			printf_LOG("[PLC EIP](Err=%d)(Write data are insufficient)\n", nErrMsg);
		}

		return FALSE;
	}

    g_nCommSts = EIP_ONLINE;
	g_bIOUpdate = TRUE;
    return TRUE;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : ReadBackWriteData
//Date        : 2004.04.06
//Return      : BOOL
//Description : Initially readback & update write buffer
//-----------------------------------------------------------------------------------------------------------
BOOL ReadBackWriteData(void)
{
    BOOL bRet = TRUE;
	EM_ADDRESS	omAddress;
	int nErrMsg;

	omAddress.byTypeOfFactor = EM_BYTE_TYPE; //EM_BIT_TYPE, EM_BYTE_TYPE, EM_WORD_TYPE, EM_DWORD_TYPE, EM_NO_DATA 중 선택
	omAddress.byLocateOnWord = EM_BYTE_HIGH; // 비트일때(0~15) 바이트일때(EM_BYTE_HIGH 또는 EM_BYTE_LOW)
	omAddress.dwWordOffset = OUT_DATA_ADDR_wOFFS; // 읽을 주소 지정
	omAddress.dwNumberOfFactors = OUT_DATA_btSIZE; // 가져올 갯수

	bRet = Em_readMemory(g_hOUT_Mem, &omAddress, g_abOutData, sizeof(g_abOutData));
	if(bRet == FALSE) 
	{ 
		nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
		printf_LOG("[PLC EIP] readmemory Failed(ReadBackWriteData)(Error = %d)\n", nErrMsg);

		if ( nErrMsg == 27 ) {
			printf_LOG("[PLC EIP](Err=%d)(The size of the buffer for reading data is insufficient)\n", nErrMsg);
		}
		else if ( nErrMsg == 29 ) {
			printf_LOG("[PLC EIP](Err=%d)(Write data are insufficient)\n", nErrMsg);
		}

		return FALSE;
	}

	g_nCommSts = EIP_ONLINE;
	return TRUE;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : ReadWordData
//Date        :
//Return      : WORD
//Description : Read word data from input buffer
//-----------------------------------------------------------------------------------------------------------
WORD ReadWordData(int nOffset)			//big endian
{
    WORD nRet = 0;

    // read upper byte from read buffer
    nRet = *(g_abInData + nOffset + 0);

    // read lower byte from read buffer
    nRet = (nRet << 8) + *(g_abInData + nOffset + 1);

    return nRet;
}
//-----------------------------------------------------------------------------------------------------------
//Name        : ReadDbWordData
//Date        : 2004.12.23 by cskim
//Return      : unsigned long
//Description : Read double word data from input buffer
//-----------------------------------------------------------------------------------------------------------
unsigned long ReadDbWordData(int nOffset)  	//big endian
{
    unsigned long ulRet = 0;

    ulRet = *(g_abInData + nOffset + 2);
    ulRet = (ulRet << 8) + *(g_abInData + nOffset + 3);
    ulRet = (ulRet << 8) + *(g_abInData + nOffset + 0);
    ulRet = (ulRet << 8) + *(g_abInData + nOffset + 1);

    return ulRet;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : ReadDbFloatData
//Date        :
//Return      : WORD
//Description : Read word data from input buffer
//-----------------------------------------------------------------------------------------------------------
float ReadDbFloatData(int nOffset)  	//big endian 2014.10.15
{
	int   i    = 0;
    float fRet = 0;
	unsigned char ucData[4];

//     //insert data to buffer;
// 	for(i = 0; i < 4; i++)
// 	{
// 		ucData[i] = g_abInData[nOffset + i];
// 	}

    //insert data to buffer;
	ucData[0] = g_abInData[nOffset + 1];
	ucData[1] = g_abInData[nOffset + 0];
	ucData[2] = g_abInData[nOffset + 3];
	ucData[3] = g_abInData[nOffset + 2];

	//fRet = (float)atof(ucData);
	memcpy(&fRet, ucData, 4);	

    return fRet;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : ReadByteData
//Date        : 2004.04.10
//Return      : BYTE
//Description : Read byte data from input buffer
//-----------------------------------------------------------------------------------------------------------
BYTE ReadByteData(int nOffset)
{
    BYTE nRet = 0;

    // read upper byte from read buffer
    nRet = *(g_abInData + nOffset);    

    return nRet;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : WriteByteData
//Date        :
//Return      : void
//Description : Write byte data to output buffer
//-----------------------------------------------------------------------------------------------------------
void WriteByteData(int nOffset, BYTE nData)
{
    *(g_abOutData + nOffset) = nData;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : WriteWordData
//Date        :
//Return      : void
//Description : Write word data to output buffer
//-----------------------------------------------------------------------------------------------------------
void WriteWordData(int nOffset, WORD nData)   	//big endian
{
    *(g_abOutData + nOffset + 1) = (BYTE)(nData & 0xff);

    *(g_abOutData + nOffset ) = (BYTE)(nData >> 8);
}

//-----------------------------------------------------------------------------------------------------------
//Name        : GetWordDataFromWRBuf
//Date        :
//Return      : WORD
//Description : Get word data from write buffer for do func
//-----------------------------------------------------------------------------------------------------------
WORD GetWordDataFromWRBuf(int nOffset)   	//big endian
{
    WORD nRet = 0;

    nRet = *(g_abOutData + nOffset + 0);
    nRet = (nRet << 8) + *(g_abOutData + nOffset + 1);

    return nRet;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : RemoveNotNeedData
//Date        :
//Return      : int
//Description : 
//-----------------------------------------------------------------------------------------------------------
int RemoveNotNeedData(WORD wData, int nLow, int nHigh)
{
    // remove upper bit's data
    wData <<= 15 - nHigh;

    // remove lower bit's data
    wData >>= 15 - nHigh + nLow;

    return (int)wData;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : GetMaskedData
//Date        :
//Return      : WORD
//Description : 
//-----------------------------------------------------------------------------------------------------------
WORD GetMaskedData(WORD wOut, WORD wSet, int nLow, int nHigh)
{
    int i = 0;
    WORD wMask = 0;

    for(i=0; i<=nHigh-nLow; i++)
    {
        // make mask
        wMask = 1 << (nLow + i);
        
		// if i-th bit is set(1), OR bitwise with mask
        // if i-th bit is clear(0), AND bitwise with ~(mask)
        if (wSet & (1 << i)) wOut |= wMask;
        else wOut &= ~(wMask);
    }
    return wOut;
}


//==========================================================================================
extern "C"
{

BOOL Device_Attach( char *Name , char *File , int id1 , int id2 , int id3 , int id4 , int id5 , int id6 , int id7 , int id8 , int id9 , int id10 )
{
	BOOL bRet = FALSE;
	
	CIPAppHandle hApp;
	CIPAppVersion CIPVersion;
	WORD		nPortID = 2;
	BOOL		bResult = 0;
	int			nPortCloseOpen = 0, nErrMsg = 0;
	BOOL		bClearWriteOutput = FALSE;

    // id1 -> set read/write timeout
    // id2 -> logging no(0)/yes(not 0)?
	// id3 -> Selected Board Number


	printf_LOG("[PLC EIP] Try attach!\n");

	if ( id1 > 0 && id1 < 6 ) {
		nPortID = id1;
	}
	else {
		nPortID = 2;
	}
	
	// Check Shared Memory Option
	if(id4 == 1)
	{
		CreateSharedMemory();
	}
	//------------------------------------------------------------------------------------------------------

	bRet = FALSE;
	do {
		///////////////////////////////////////////////////
		// STARTUP
		///////////////////////////////////////////////////
		CIPAPP_STARTUP();
		
		//-------------------------------
		// Register application
		hApp = CIPApp_new("SGW_Sample");
		if (hApp == NULL)
		{
			printf_LOG("[PLC EIP Open] Error: CIPApp_new failed.\n");
			break;
		}
		else {
			
			CIPVersion = CIPApp_getVersion();
			printf_LOG("[PLC EIP Open] CIPApp_new Success. Version[%d.%d]\n", CIPVersion.major, CIPVersion.minor);
		
			nPortCloseOpen = CIPPort_getStatus(hApp, nPortID);
			
			if ( nPortCloseOpen < 0 ) {		//Error  ( -1 )

				nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte

				printf_LOG("[PLC EIP Open] PortID[%d] CIPPort_getStatus[%d], GetLastError[%d]\n", nPortID, nPortCloseOpen, nErrMsg);
				if(nErrMsg == 53) {
					printf_LOG("[PLC EIP Open] GetLastError[%d] (The target entry does not exist)\n", nErrMsg);
					printf_LOG("[PLC EIP Open] Please set the communication service for the SYSMAC Gateway!!!\n");
				}

				break;
			}
			else if ( nPortCloseOpen == CIPPortStatus_Closed ) {//Close 				
				bClearWriteOutput = TRUE;
			}
			else if ( nPortCloseOpen == CIPPortStatus_Open ) {//Open
				bClearWriteOutput = FALSE;
			}
			else {							//Exception
				break;
			}
		}
		
		bRet = TRUE;
	} while(0);

	if (hApp != NULL) {
		///////////////////////////////////////////////////
		// CLEAN UP
		///////////////////////////////////////////////////
		CIPApp_delete(hApp);
	}

	if(bRet != TRUE){
		return bRet;
	}
	
	    
	//------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------

	bRet = FALSE;
	do {


		// initialize EIP
		EM_STARTUP();   // START UP으로 시작

		// 읽을 메모리 영역 지정
		//hMem = Em_openMemory("E1", NULL);
		//hMem = Em_openMemory("EM1", NULL); 
		//g_hIN_Mem = Em_openMemory("DM",NULL);		
		g_hIN_Mem = Em_openMemory("CIO",NULL);			// 설비별 실제 MemoryName 확인 필요..
		if(g_hIN_Mem== NULL) 
		{ 
			g_nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
			printf_LOG("[PLC EIP] Open Memory Failed (Error = %d)\n", g_nErrMsg);
			break;
		}

		// 2012.08.02
		//g_hOUT_Mem = Em_openMemory("EM0",NULL);	
		g_hOUT_Mem = Em_openMemory("CIO",NULL);			// 설비별 실제 MemoryName 확인 필요..
		if(g_hIN_Mem== NULL) 
		{ 
			g_nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
			printf_LOG("[PLC EIP] Open Memory Failed (Error = %d)\n", g_nErrMsg);
			break;
		}

		g_hVrcpMem = Em_openMemory("EM0",NULL);			// 설비별 실제 MemoryName 확인 필요..
		if(g_hVrcpMem== NULL) 
		{ 
			g_nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
			printf_LOG("[PLC EIP] Open Memory Failed (Error = %d)\n", g_nErrMsg);
			break;
		}

		if(!ClearSMStepData())
		{
			printf_LOG("[PLC EIP] writeMemory Clear Fail(RecipeMem)\n");		
		}

		//ReadSendData function , read current write data & update write buffer
		if ( bClearWriteOutput == TRUE ) {			
			printf_LOG("[PLC EIP] Clear WriteData\n");
			memset(g_abOutData, 0, sizeof(g_abOutData));
			WriteIO();
		}
		else {
			printf_LOG("[PLC EIP] Get ReadBackWriteData\n");
			if(!ReadBackWriteData())
			{
				printf_LOG("[PLC EIP] ReadBackWriteData Failed\n");
				break;
			}
		}

		// add delay
		_sleep(1000);

		bRet = TRUE;
	} while(0);

	if(bRet != TRUE)
	{
		if(g_hIN_Mem != NULL) Em_closeMemory(g_hIN_Mem);
		if(g_hOUT_Mem != NULL) Em_closeMemory(g_hOUT_Mem);				// 2012.08.02
		if(g_hVrcpMem != NULL) Em_closeMemory(g_hVrcpMem);

		return bRet;
	}

	//--------------------------------------------------------------------------------------------------------------

	bRet = FALSE;
	do {
		///////////////////////////////////////////////////
		// STARTUP
		///////////////////////////////////////////////////
		CIPAPP_STARTUP();
		
		//-------------------------------
		// Register application
		hApp = CIPApp_new("SGW_Sample");
		if (hApp == NULL)
		{
			printf_LOG("[PLC EIP Open] Error: CIPApp_new failed.\n");
			break;
		}
		else {
			
			nPortCloseOpen = CIPPort_getStatus(hApp, nPortID);
			
			if ( nPortCloseOpen < 0 ) {		//Error 
				nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
				printf_LOG("[PLC EIP Open] PortID[%d] CIPPort_getStatus[%d], GetLastError[%d]\n", nPortID, nPortCloseOpen, nErrMsg);
				break;
			}
			else if ( nPortCloseOpen == CIPPortStatus_Closed ) {//Close 
				
				bResult = CIPPort_open(hApp, nPortID);
				printf_LOG("[PLC EIP Open] CIPPort_open Start : Result[%d]\n", bResult);
				
				nPortCloseOpen = CIPPort_getStatus(hApp, nPortID);
				if ( nPortCloseOpen == CIPPortStatus_Open ) {
					printf_LOG("[PLC EIP Open] CIPPort_open Success[%d]\n", nPortCloseOpen);	

					//Success
				}
				else {
					printf_LOG("[PLC EIP Open] CIPPort_open Fail[%d]\n", nPortCloseOpen);

					nErrMsg = ((BYTE)((WORD)GetLastError()));		//Get low byte
					printf_LOG("[PLC EIP Open] PortID[%d] GetLastError[%d]\n", nPortID, nErrMsg);
					if(nErrMsg == 2) {
						printf_LOG("[PLC EIP Open] GetLastError[%d] (An Exception was generated during processing)\n", nErrMsg);
						printf_LOG("[PLC EIP Open] Please check connection LAN cable or LAN Port ID !!! \n");
					}

					break;
				}
				
			}
			else if ( nPortCloseOpen == CIPPortStatus_Open ) {//Open
				//Success
			}
			else {							//Exception
				break;
			}
			
			//bResult = CIPPort_close(hApp, nPortID);
			//printf("[PLC EIP Open] CIPPort_close [%d]\n", bResult);
			
			
		}
		
		bRet = TRUE;

	} while(0);
	
	
	
	if (hApp != NULL) {
		///////////////////////////////////////////////////
		// CLEAN UP
		///////////////////////////////////////////////////
		CIPApp_delete(hApp);
	}
	
	
	if(bRet != TRUE){
		return bRet;
	}

	printf_LOG("[PLC EIP] attached !\n");
	g_nCommSts = EIP_ONLINE;
	

    return bRet;
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

BOOL Device_Detach( char *Name , char *File , int id1 , int id2 , int id3 , int id4 , int id5 , int id6 , int id7 , int id8 , int id9 , int id10 )
{
    _LOG("Device detached!");
	if(g_hFileHandle != NULL) FreeSharedMemory();
	if(g_hOUT_Mem != NULL) Em_closeMemory(g_hOUT_Mem);					// 2012.08.02
	if(g_hIN_Mem != NULL) Em_closeMemory(g_hIN_Mem);
	if(g_hVrcpMem != NULL) Em_closeMemory(g_hVrcpMem);

    return TRUE;

}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// id1 : IO Identification
// id2 : axis(1~4)
int Digital_In( int id1 , int id2 , int id3 , int id4 , int *result )
{
	//id1 = MACID is member variable
	//id2 = Offset address when R/W IO , somtimes id2 which is greater than 1000 is reserved for special channel
	//id3 = high byte start bit
	//id4 = low  byte end   bit


	if(id2 >= 0 && id2 <= 1000) //Common Write address area
	{
		switch(id1)
		{
		case 100: //Memory read
			if ( (id2 >= 0) && (id2 < MAX_ADDRESS) )
			{
				if ( (id3 <= id4) && (id3 >= 0) && (id3 <= 15) && (id4 >= 0) && (id4 <= 15))
				{
					*result = TRUE;
					return RemoveNotNeedData( g_abBUFFER[id2], id3, id4);
				}
			}
			printf( "ERROR: [DNet] DI: IO word/bit Range Error - [%d]-[%d]-[%d]\n", id2, id3, id4 ); 
			break;

		case 201:
			return g_bSM_Result;

		default: //Read from device
			if ( (id3 <= id4) && (id3 >= 0) && (id3 <= 15) && (id4 >= 0) && (id4 <= 15) )
			{
				if (ReadIO()) {
					*result = TRUE;
					return RemoveNotNeedData( ReadWordData(id2), id3, id4);
				}
			}
			else
				printf( "ERROR: [DNet] DI: IO bit Range Error - [%d]-[%d]\n", id3, id4 ); 
			
			break;
		}


	}
	else //Special channel read
	{
		switch(id2)
		{
		case 1099: //[PLC EIP] Net status
			ReadIO();
			*result = TRUE;
			return g_nCommSts;
			
		case 3000:		// 2012.03.19
			*result = TRUE;
			return g_nUpdatePassCount;
		case 3001:		// 2012.03.19
			*result = TRUE;
			return g_nUpdatePassMax;
		default:
			printf( "ERROR : [DNet] DI : Invalid id2 for special channel[%d] comes \n" , id2); 
			break;
		}
	}    
    
    *result = FALSE;
    return 0;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int id1 , int id2 , int id3 , int id4 , int data , int *result )
{
    WORD nTmp = 0;

    //id1 = MACID is member variable
	//id2 = Offset address when R/W IO , somtimes id2 which is greater than 1000 is reserved for special channel
	//id3 = high byte start bit , downloading data when explicitmessage
	//id4 = low  byte end   bit

	if(id1 < 0 || id1 > 63)
	{
		*result = FALSE;
		switch(id1)
		{
		case 201 : *result = ReadFromSharedMemory(); break;
		case 202 : *result = SetSMStepData();  break;
		case 203 : *result = ClearSMStepData();  break;
		case 400 : 
			if(data == 1)  //... On 할때만 값을 MFC 정보를 Update한다.
			{
				*result = READ_MFC_Info();
			}
			else{*result = TRUE;}
			break; 
		}
		return;
	}
	//Common Write address area
	if(id2 >= 0 && id2 <= 1000)
	{
		if ((id3 > id4) || (id3 < 0 || id3 > 15 || id4 < 0 || id4 > 15))
		{
			printf( "ERROR: [DNet] DO: IO bit Range Error - low[%d] <-> high[%d]\n" , id3 , id4 ); 
			*result = FALSE;
			return;
		}


		nTmp = GetWordDataFromWRBuf(id2);
		WriteWordData(id2, GetMaskedData(nTmp, (WORD)data, id3, id4) );

		if (!WriteIO())
		{
			printf("Error : [DNet] Digital_Out->WriteIO got invalid result\n");
			*result = FALSE;
			return;
		}

		// write setpoint to buffer for valid check,
		// therefore we have to have another buffer (abBUFFER)
		if ( id2 >= 0 && id2 < MAX_ADDRESS )
		{
			nTmp          = g_abBUFFER[id2];
			g_abBUFFER[id2] = GetMaskedData(nTmp, (WORD)data, id3, id4);
		}
	}
    //Special channels
	else 
	{
		printf( "ERROR : [DNet] DO : Invalid id2 for special channel[%d] comes \n" , id2); 
		*result = FALSE; 

		return;
	}
    *result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

double Analog_In( int id1 , int id2 , int id3 , int *result )
{
    WORD          nData  = 0;
	unsigned char ucData = 0;
	float         fData  = 0.0f;
	unsigned long ulData = 0;
	signed short  ssData = 0;
	unsigned short usData = 0;
	int nIdx;						 // 2012.03.08

	//Address valid check
	if ( (id2 < 0) || (id2 >= MAX_ADDRESS))    //2014.12.29
	{
		printf("ERROR: [DNet] AI - Out of Address Range[id2 : %d] \n", id2);
		*result = FALSE;
		return 0;
	}

	//id3 used for byte write
    switch(id3)
	{
	case 0:
		if (ReadIO())
		{
			nData = ReadWordData(id2);
			*result = TRUE;
			return (double)nData;
		}
		else
			printf("Error : [DNet] Alanog_In->ReadIO got invalid result!! \n");
		break;
	case 1:
		//Just read & return high byte
		if (ReadIO())
		{
			ucData = ReadByteData(id2);
			*result = TRUE;
			return (double)ucData;
		}
		else
			printf("Error : [DNet] Alanog_In->ReadIO(HighByte) got invalid result!! \n");
		break;
	case 2:
		//Just read & return low byte
		if (ReadIO())
		{
			ucData = ReadByteData(id2+1);
			*result = TRUE;
			return (double)ucData;
		}
		else
			printf("Error : [DNet] Alanog_In->ReadIO(LowByte) got invalid result!! \n");
		break;
	case 3:
		//Read 2 Word float data & return
		if(ReadIO())
		{
			fData = ReadDbFloatData(id2);
			*result = TRUE;
			return (double)fData;
		}
		else
			printf("Error : [DNet] Analog_In->ReadIO(double float) got invalid result!! \n");
		break;
	// 2004.12.23 by cskim
	case 4:
		//Read double word data & return
		if(ReadIO())
		{
			ulData = ReadDbWordData(id2);
			*result = TRUE;
			return (double) ulData;
		}
		else
			printf("Error : [DNet] Analog_In->ReadIO(double word) got invalid result!! \n");
		break;
	case 5:
		// signed short(16bit signed integer)
		if (ReadIO())
		{
			usData = ReadWordData(id2);
			ssData = (signed short) usData;
			*result = TRUE;
			return (double)ssData;
		}
		else
			printf("Error : [DNet] Alanog_In->ReadIO got invalid result!! \n");
		break;
	default:
		// 2012.03.08
		if(id3>=10 && id3<20)
		{
			nIdx = id3 - 10;
			if (ReadIO())
			{
				nData = ReadWordData(id2);
				*result = TRUE;
				return (double) (nData+(int)g_adbOffset[nIdx]);
			}
			else
				printf("Error : [DNet] Alanog_In->ReadIO got invalid result!! \n");
			break;
		}
		printf("Error : [DNet] Alanog_In->Invalid id3 comes \n");
		break;
	}

    *result = FALSE;
    return 0;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Out( int id1 , int id2 , int id3 , double data , int *result )
{
	//id1 Device MacID
	//id2 Offset address
	//id3 Special function code
    unsigned char ucData;
	signed short ssData;
	unsigned short usData;
	int nIdx;

	/*
	if(id1 < 0 || id1 > 63)
	{
		printf("ERROR: [DNet] AO - Map number(id1) %d is invalid \n", id1);
		*result = FALSE;
		return;
	}
	else usSetDeviceMacID = id1;
	*/

	//Address valid check
	if ( (id2 < 0) && (id2 >= MAX_ADDRESS))
	{
		printf("ERROR: [DNet] AO - Out of Address Range[id2 : %d] \n", id2);
		*result = FALSE;
		return;
	}

    //id3 used for byte write
	switch(id3)
	{
	case 0:
		WriteWordData(id2, (WORD)data);
		if (WriteIO())
		{
			*result = TRUE;
			//printf("[PLC_EIP] Alanog_Out Success ->WriteIO[%f] ID[%d][%d][%d]\n",data, id1, id2, id3);
			return;
		}
		else
			printf("Error : [DNet] Alanog_Out->WriteIO got invalid result!! \n");

		break;
	case 1:
		//Just write incoming high byte
		ucData  = ((BYTE)(((WORD) (data) >> 8) & 0xff));
		WriteByteData(id2 + 1 , ucData);
		if(WriteIO())
		{
			*result = TRUE;
			return;
		}
		else
			printf("Error : [DNet] Alanog_Out->WriteIO(HighByte) got invalid result!! \n");

		break;
	case 2:
		//Just write incoming low byte
		ucData = ((BYTE)((WORD)data));
		WriteByteData(id2 , ucData);
		if(WriteIO())
		{
			*result = TRUE;
			return;
		}
		else
			printf("Error : [DNet] Alanog_Out->WriteIO(LowByte) got invalid result!! \n");
		break;
	case 5:
		ssData = (signed short) data;
		usData = (unsigned short) ssData;
		WriteWordData(id2, (WORD)usData);
		if (WriteIO())
		{
			*result = TRUE;
			return;
		}
		else
			printf("Error : [DNet] Alanog_Out->WriteIO(signed short) got invalid result!! \n");
		break;
	default:
		// 2012.03.08
		if(id3>=10 && id3<20)
		{
			nIdx = id3 - 10;
			g_adbOffset[nIdx] = data;
			*result = TRUE;
			return;
		}
		printf("Error : [DNet] Alanog_Out->Invalid id3 comes \n");
		break;
	}

    *result = FALSE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int id1 , char *data , int *result ) {

	int nMFCIdex = 0;
	int nItemIdex = 0;

	nMFCIdex = (int)id1/100;
	nMFCIdex -= 1;
	nItemIdex = (int)id1%100;
	nItemIdex -= 1;

	switch(nItemIdex)
	{
	case eMFC_MANUFAC:
		strcpy(data,g_szMFCManufacSI[nMFCIdex]);
		break;
	case eMFC_MODELNUM:
		strcpy(data,g_szMFCModelNumSI[nMFCIdex]);
		break;
	case eMFC_SWVER:
		strcpy(data,g_szMFCSWVersionSI[nMFCIdex]);
		break;
	case eMFC_HWVER:
		strcpy(data,g_szMFCHWVersionSI[nMFCIdex]);
		break;
	case eMFC_SERNUM:
		strcpy(data,g_szMFCSerialNumSI[nMFCIdex]);
		break;
	case eMFC_GASNUM:
		strcpy(data,g_szMFCGasNumSI[nMFCIdex]);
		break;
	case eMFC_GASSYM:
		strcpy(data,g_szMFCGasSymbolSI[nMFCIdex]);
		break;
	case eMFC_FULLSCAL:
		strcpy(data,g_szMFCFullScaleSI[nMFCIdex]);
		break;
	case eMFC_OFFSET_B:
		strcpy(data,g_szMFCOffesetSI[nMFCIdex]);
		break;
	}
	*result = TRUE;		
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Out( int id1, char *data , int *result ) {
	*result = FALSE;

}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_Name_In( char *Name , int *result ) {
	*result = TRUE;
	return 0;
}
//-----------------------------------------------------------------------------------
void Analog_Name_Out( char *Name , double data , int *result ) {
	*result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_Name_In( char *Name , int *result ) {
	*result = TRUE;
	return 0;
}
//-----------------------------------------------------------------------------------
void Digital_Name_Out( char *Name , int data , int *result ) {
	*result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Name_In( char *Name , char *data , int *result ) {
	*result = TRUE;
}
//-----------------------------------------------------------------------------------
void String_Name_Out( char *Name , char *data , int *result ) {
	*result = TRUE;
}

} // End of [extern "C"]
//==========================================================================================
