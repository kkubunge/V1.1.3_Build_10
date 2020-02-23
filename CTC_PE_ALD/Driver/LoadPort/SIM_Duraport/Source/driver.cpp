
extern "C"
{

#include "cimrs232.h"
#include "driver.h"
#include "kutlstr.h"

}// End of [extern "C"]

#include "TextLogDll_M.h"

#define MAX_BUF 400

// terminal string
char  g_strSendTerm[8] = "\0";
char  g_strRecvTerm[8] = "\0";
char  g_strRecvTerm2[8] = "\0";
int   g_nTimeOut = 100;

int g_nPaddleExtRetXI		=	0;		
int g_nLatchOpenCloseXI		=	0;	
int g_nDoorUpDownXI			=	0;		
int g_nVacuumOKDI			=	0;			
int g_nWaferSlideOutDI		=	0;

int g_nShuttleInOutXI		=	0;		
int g_nF_ProperlyPlaceDI	=	0;			
int g_nInterlockActiveDI	=	0;			
int g_nClampUnclampXI		=	0;	
int g_nFOUP_PresentDI		=	0;

//Motor Driver On Added 2008.09.08		
int g_nMotorStatusDI		=	1;

int g_nSwitch1UnloadDI		= 	0;			
int g_nSwitch2LoadDI		=	0;	

int g_nMappingStatus		=	0;
int g_nErrorCode			=	0;
int g_nErrorDI				=	0;

int g_nComPort				=	0;

// state
BOOL    g_bCommStatus = FALSE;
BOOL	g_bBusy = FALSE;

int g_nEQC_Wafer_Info[MAX_BUF];
const int MAX_SLOT = 25;

char g_szErrorCode[MAX_BUF];
char g_szEventCode[MAX_BUF];


typedef enum _DOCK_ENUM
{
	FULLDOCK,
	SHUTTLEIN,
	CLAMP,
	FULLDOCKMAP,
}DOCK_ENUM;

typedef enum _UNDOCK_ENUM
{
	FULLUNDOCK,
	CLOSEDOOR,
	SHUTTLEOUT,
}UNDOCK_ENUM;

typedef union _uHexValue
{
	unsigned int nHexValue;
	unsigned char szHexValue[4];

	_uHexValue::_uHexValue()
	{
		memset(this, 0, sizeof(_uHexValue));
	}
}uHexValue;

char APP_NAME[] = "duraport\0";

DWORD g_dwThreadID = NULL;
HANDLE g_hThread = NULL;
BOOL g_bThreadRunning = FALSE;
BOOL g_bThreadTerminate = FALSE;

HANDLE g_hMutex = NULL;

const int POLLING_DELAY = 50;

BOOL g_bStatusResponse[32];
int  g_nLampArray[10];
BOOL g_bThreadRun = FALSE;

extern "C"
{

BOOL Rs232_IO(char *szDataToSend , char *szDataToReceive);
BOOL Rs232_IO2(char *szDataToSend , char *szDataToReceive);
void PollingIOChannels();
BOOL ParseReceiveMapData(char* szDataToReceive);

unsigned long __stdcall AutoMapRetry(void *pVoid)
{
	g_bThreadRun = TRUE;

	char szDataToSend[MAX_BUF];
	char szDataToReceive[MAX_BUF];

	WaitForSingleObject(g_hMutex, INFINITE);

	sprintf(szDataToSend, "SCAN DN");						
	if(FALSE == Rs232_IO2(szDataToSend, szDataToReceive))		g_nMappingStatus = 3;
	else														g_nMappingStatus = 2;

	ReleaseMutex(g_hMutex);
	printf("===@~~ map Driver End!! \n"); 

	g_bThreadRun = FALSE;
	return 0L;
}

//static DWORD WINAPI ThreadProcDevice(void *pVoid)
unsigned long __stdcall ThreadProcDevice(void *pVoid)
{
	g_bThreadRunning = TRUE;

	PollingIOChannels();

	g_bThreadRunning = FALSE;
	return 0L;
}

//////////////////////////////////////////////////////////////////////////
void PollingIOChannels()
{
	char szDataToSend[MAX_BUF];
	char szDataToReceive[MAX_BUF];

	do
	{
		memset(szDataToSend, 0, sizeof(szDataToSend));
		memset(szDataToReceive, 0, sizeof(szDataToReceive));

		strcpy(szDataToSend, "STATUS");
		do
		{
			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))	break;
		}while(0);

		memset(szDataToSend, 0, sizeof(szDataToSend));
		memset(szDataToReceive, 0, sizeof(szDataToReceive));

		strcpy(szDataToSend, "INDICATOR");
		do
		{
			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))	break;
		}while(0);
		
		if(TRUE == g_bThreadTerminate)								break;

		Sleep(POLLING_DELAY);

	}while(1);
}

void AtoH(char * src, char * dest, int destlen);
unsigned char BtoH(char ch);

//    AtoH            - Converts ascii string to network order hex 
void AtoH(char * src, char * dest, int destlen) 
{     
	char * srcptr;      
	srcptr = src;      
	
	while(destlen--)     
	{     
		*dest = BtoH(*srcptr++) << 4;    // Put 1st ascii byte in upper nibble.     
		*dest++ += BtoH(*srcptr++);      // Add 2nd ascii byte to above.     
	} 
} 

//    BtoH            - Converts ascii byte to hex
unsigned char BtoH(char ch) 
{     
	if (ch >= '0' && ch <= '9') return (ch - '0');        // Handle numerals     
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits     
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits     
	return(255); 
} 

BOOL ParseReceiveMapData(char* szDataToReceive)
{
	BOOL BRet = FALSE;

	do
	{
		// response data
		//
		//      12345678
		// ex) M00000000,00000000,00000000\n
		//

		char *szResponse = NULL;
		char szToken[] = "M,";

		/*char szRevBuffer[MAX_BUF];
		memset(szRevBuffer, 0, sizeof(szRevBuffer));
		strcpy(szRevBuffer, szDataToReceive);*/

		szResponse = strtok(szDataToReceive, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to find 'M' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}

		int nWaferPresentInfo[32];
		int nWaferCrossInfo[32];
		int nWaferDoubleInfo[32];

		memset(nWaferPresentInfo, 0, sizeof(nWaferPresentInfo));
		memset(nWaferCrossInfo, 0, sizeof(nWaferCrossInfo));
		memset(nWaferDoubleInfo, 0, sizeof(nWaferDoubleInfo));

		

		// 1. get wfaer present or absent inform

		// get real response
		szResponse = strtok(NULL, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to find ',' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}

		// convert hex data from ascii
		uHexValue uHexWaferInfoBuffer;
		uHexValue uHexWaferInfoRev;



		AtoH(szResponse, (char *)&uHexWaferInfoBuffer.szHexValue, 4);
		//nHexData = uHexWaferInfoBuffer.nHexValue;

		//  Data of uHexBuffer, after AtoH() fn.
		//
		//	uHexBuffer.szHexValue :   0  1  2  3
		//						      12 23 56 78
		//
		//  uHexBuffer.nHexValue  :   0x78563412


		// get hex data from buffer(nHexBuffer)
		int nIndex = 0;
		BOOL BData = FALSE;
		int nShiftData = 0;
		int nEndIndex = sizeof(uHexWaferInfoBuffer.szHexValue) - 1;

		for(nIndex = 0; nIndex < 4; nIndex ++)
		{
			uHexWaferInfoRev.szHexValue[nEndIndex - nIndex] = uHexWaferInfoBuffer.szHexValue[nIndex];
		}

		//  Data of uHexRev, after for(...) loop.
		//
		//	uHexRev.szHexValue    :   0  1  2  3
		//						      78 56 34 12 
		//
		//  uHexRev.nHexValue     :   0x12345678


		unsigned int nHexData = 0;
		nHexData = uHexWaferInfoRev.nHexValue;


		// convert "bit status" from "total response hex data".
		for(nIndex = 0; nIndex < 32; nIndex ++)
		{
			nShiftData = ( nHexData >> nIndex);
			BData = nShiftData & 0x1;
			nWaferPresentInfo[nIndex] = BData;
		}


		// 2. get wafer cross inform

		// get cross inform response
		szResponse = strtok(NULL, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to find ',' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}

		// convert hex data from ascii
		uHexValue uHexCrossInfoBuffer;
		uHexValue uHexCrossInfoRev;

		AtoH(szResponse, (char *)&uHexCrossInfoBuffer.szHexValue, 4);

		nIndex = 0;
		BData = FALSE;
		nShiftData = 0;
		nEndIndex = sizeof(uHexCrossInfoBuffer.szHexValue) - 1;

		for(nIndex = 0; nIndex < 4; nIndex ++)
		{
			uHexCrossInfoRev.szHexValue[nEndIndex - nIndex] = uHexCrossInfoBuffer.szHexValue[nIndex];
		}

		nHexData = 0;
		nHexData = uHexCrossInfoRev.nHexValue;

		// convert "bit status" from "total response hex data".
		for(nIndex = 0; nIndex < 32; nIndex ++)
		{
			nShiftData = ( nHexData >> nIndex);
			BData = nShiftData & 0x1;
			nWaferCrossInfo[nIndex] = BData;
		}

		// 3. get wafer double inform

		// get cross inform response
		szResponse = strtok(NULL, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to find ',' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}

		// convert hex data from ascii
		uHexValue uHexDoubleInfoBuffer;
		uHexValue uHexDoubleInfoRev;

		AtoH(szResponse, (char *)&uHexDoubleInfoBuffer.szHexValue, 4);

		nIndex = 0;
		BData = FALSE;
		nShiftData = 0;
		nEndIndex = sizeof(uHexDoubleInfoBuffer.szHexValue) - 1;

		for(nIndex = 0; nIndex < 4; nIndex ++)
		{
			uHexDoubleInfoRev.szHexValue[nEndIndex - nIndex] = uHexDoubleInfoBuffer.szHexValue[nIndex];
		}

		nHexData = 0;
		nHexData = uHexDoubleInfoRev.nHexValue;

		// convert "bit status" from "total response hex data".
		for(nIndex = 0; nIndex < 32; nIndex ++)
		{
			nShiftData = ( nHexData >> nIndex);
			BData = nShiftData & 0x1;
			nWaferDoubleInfo[nIndex] = BData;
		}

		int nWaferInfo = 0;
		memset(g_nEQC_Wafer_Info, 0, sizeof(g_nEQC_Wafer_Info));

		// sum all wafer info data
		for(nIndex = 0; nIndex < 32; nIndex ++)
		{
			nWaferInfo = 0;	// 0 unknown, 1 absent, 2 present, 3 cross 4.double

			if(1 == nWaferPresentInfo[nIndex])
			{
				nWaferInfo = 2;
			}
			else
			{
				nWaferInfo = 1;
			}

			if(1 == nWaferCrossInfo[nIndex])
			{
				nWaferInfo = 3;
			}

			if(1 == nWaferDoubleInfo[nIndex])
			{
				nWaferInfo = 4;
			}

			g_nEQC_Wafer_Info[nIndex] = nWaferInfo;
		}


		BRet = TRUE;
	}while(0);

	if(FALSE == BRet)
	{
//		_LOG("Failed to function ParseReceiveMapData(). (%d)", __LINE__);
	}

	return BRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) 
{
	BOOL BRet = FALSE;

	char szDataToSend[MAX_BUF];
	char szDataToReceive[MAX_BUF];

	char pszLocFtnName[] = "Device_Attach";
	char pszFtnName[MAX_BUF];

	memset(pszFtnName, 0, sizeof(pszFtnName));
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("%s start!!!\n", pszFtnName);

	memset(g_strSendTerm, 0, sizeof(g_strSendTerm));
	memset(g_strRecvTerm, 0, sizeof(g_strRecvTerm));
	memset(g_strRecvTerm2, 0, sizeof(g_strRecvTerm2));
	memset(g_nEQC_Wafer_Info, 0, sizeof(g_nEQC_Wafer_Info));
	memset(g_szErrorCode, 0, sizeof(g_szErrorCode));
	memset(g_szEventCode, 0, sizeof(g_szEventCode));
	memset(szDataToSend, 0, sizeof(szDataToSend));
	memset(szDataToReceive, 0, sizeof(szDataToReceive));
	memset(g_bStatusResponse, 0, sizeof(g_bStatusResponse));
	memset(g_nLampArray, 0, sizeof(g_nLampArray));

	strcpy(g_strSendTerm, "\n");
	strcpy(g_strRecvTerm, "\n");
	strcpy(g_strRecvTerm2, "");


	do
	{
		g_nComPort = ID1;

		// set time out
		if ( ID10 < 100 ) 			g_nTimeOut = 100;
		else						g_nTimeOut = ID10;

		// create mutex
		char szMutexName[MAX_BUF];
		memset(szMutexName, 0, sizeof(szMutexName));

		// Mutex string name : (drivername)_(portnumber)
		//                     ex) "duraport_12"

		sprintf(szMutexName, "%s_%d", APP_NAME, ID1);		
		g_hMutex = CreateMutex(NULL, FALSE, szMutexName);
		if(NULL == g_hMutex)	break;

		// init log file
		char szLogFileName[MAX_BUF];
		memset(szLogFileName, 0, sizeof(szLogFileName));

		// log file name : _Log\\(drivername)_(portname).log
		//				   ex) "_Log\\duraport_12.log"

		sprintf(szLogFileName, "f:\\Datalog\\_Log\\%s.log", szMutexName);
			
		_gInitLogDll(szLogFileName, "", FALSE);

		
		// com port open

//		if ( RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ) ) 
		{
			g_bCommStatus = TRUE;

//			RS232_Clear_Buffer();

			// device init.
			//		0. Motor Servo on
			//		1. foup place sensor on
			//		2. door vacuum sensor on
			//		3. down mapping option enable
			//		4. up mapping optoin disable
			//		5. maint mode disable
			//		6. auto mode enable
			//		7. load/unload button disable

			// Motor Driver On Command Addition 2008.08.25
			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "AMPON");
			Rs232_IO(szDataToSend, szDataToReceive);

			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "SEN_PLACE ON");
			Rs232_IO(szDataToSend, szDataToReceive);

			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "SEN_VACUUM ON");
			Rs232_IO(szDataToSend, szDataToReceive);

			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "CFG_DNMAP ON");
			Rs232_IO(szDataToSend, szDataToReceive);

			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "CFG_UPMAP OFF");	// 
			Rs232_IO(szDataToSend, szDataToReceive);

			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "MAINT_MODE OFF");
			Rs232_IO(szDataToSend, szDataToReceive);

			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "AUTO_MODE OFF");	// this means manual mode.
			Rs232_IO(szDataToSend, szDataToReceive);

			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "CFG_IDSW ON");	// load/unload button switch use.
			Rs232_IO(szDataToSend, szDataToReceive);
		}
// 		else
// 		{
// 			_LOG("Dura Init failure!!!. (%d)", __LINE__);
// 			break;
// 		}


// 		// create polling thread
// 		g_hThread = CreateThread(NULL, 0, ThreadProcDevice, NULL, 0, &g_dwThreadID);
// 		if(NULL != g_hThread)
// 		{
// 			int nElapsedTime = 0; // 0 ms
// 			int nWaitTime = 5000; // 5000 ms
// 
// 			do
// 			{
// 				if(TRUE == g_bThreadRunning)
// 				{
// 					break;
// 				}
// 				else
// 				{
// 					nElapsedTime += 100;
// 					Sleep(100);
// 				}
// 
// 			}while(nWaitTime > nElapsedTime);
// 
// 			if(TRUE != g_bThreadRunning)	break;
// 		}
// 		else	break;
// 
// 		if(TRUE != g_bThreadRunning)	break;
// 
// 		printf("Dura Init success!!! (%d) \n", __LINE__);
// 		_LOG("Dura Init success!!!. (%d)", __LINE__);

		BRet = TRUE;
	}while(0);

	return BRet;
}

BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) 
{
	char pszLocFtnName[] = "Device_Detach";
	char pszFtnName[256];

	memset(pszFtnName, 0, sizeof(pszFtnName));
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);


	// terminate polling thread

	g_bThreadTerminate = TRUE;


	// wait thread terminate

	int nElapsedTime = 0; // 0 ms
	int nWaitTime = 5000; // 5000 ms

	do
	{
		if(FALSE == g_bThreadRunning)
		{
			break;
		}
		else
		{
			nElapsedTime += 100;
			Sleep(100);
		}

	}while(nWaitTime > nElapsedTime);


	// close com port

//	RS232_Disconnect_Port();

	_LOG("Dura Is Detached successfully!!!. (%d)", __LINE__);

	_gCLOSE_LOG();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL Rs232_IO( char *Command , char *Response )
{
	BOOL bRetCode = FALSE;
	BOOL bInsideLoop = FALSE;

	WaitForSingleObject(g_hMutex, INFINITE);

	g_bBusy = TRUE;

	do
	{
		int nReadCount = 0;
		char szDataToSend[MAX_BUF];
		char szDataToReceive[MAX_BUF];
		char szReceiveBuffer[MAX_BUF];
	
		memset(szDataToSend, 0, sizeof(szDataToSend));
		memset(szDataToReceive, 0, sizeof(szDataToReceive));
		memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));

		sprintf(szDataToSend, "%s%s", Command, g_strSendTerm);
		printf("[DEBUG] Duraport Com[%d] Cmds[%s]\n", g_nComPort, Command);

		//... For Simulator Driver
		bRetCode = TRUE;

		if(TRUE == bRetCode)	g_bCommStatus = TRUE;
		else					g_bCommStatus = FALSE;

	}while(0);

	g_bBusy = FALSE;

	ReleaseMutex(g_hMutex);

	return bRetCode;
}

//////////////////////////////////////////////////////////////////////////
BOOL Rs232_IO2( char *Command , char *Response )
{
	BOOL bRetCode = FALSE;
	BOOL bInsideLoop = FALSE;

	g_bBusy = TRUE;

	do
	{
		int nReadCount = 0;
		char szDataToSend[MAX_BUF];
		char szDataToReceive[MAX_BUF];
		char szReceiveBuffer[MAX_BUF];
	
		memset(szDataToSend, 0, sizeof(szDataToSend));
		memset(szDataToReceive, 0, sizeof(szDataToReceive));
		memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));

		sprintf(szDataToSend, "%s%s", Command, g_strSendTerm);
		printf("[DEBUG] Duraport Com[%d] Cmds[%s]\n", g_nComPort, Command);

		//... For Simulator Driver
		bRetCode = TRUE;

		if(TRUE == bRetCode)	g_bCommStatus = TRUE;
		else					g_bCommStatus = FALSE;
	
	}while(0);

	g_bBusy = FALSE;

	return bRetCode;
}

//////////////////////////////////////////////////////////////////////////
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) 
{
	*Result = TRUE;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) 
{
	*Result = TRUE;
}

//////////////////////////////////////////////////////////////////////////
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) 
{
	int nRet = 0;
	*Result = FALSE;

	char szDataToSend[MAX_BUF];
	char szDataToReceive[MAX_BUF];

	memset(szDataToSend, 0, sizeof(szDataToSend));
	memset(szDataToReceive, 0, sizeof(szDataToReceive));

	switch(ID1)
	{
	case 1:
			switch(ID2)
			{
			case 1 :	// PaddleExtRetXI
					nRet = g_nPaddleExtRetXI;
					*Result = TRUE;
					break;
			case 2 :	// LatchOpenCloseXI
					nRet = g_nLatchOpenCloseXI;
					*Result = TRUE;
					break;
			case 3 :	// DoorUpDownXI
					nRet = g_nDoorUpDownXI;		// 0 Unknown, 1 up, 2 down ,3 Error
					*Result = TRUE;
					break;
			case 4 :	// VacuumOKDI
					nRet = g_nVacuumOKDI;
					*Result = TRUE;
					break;
			case 5 :	// WaferSlideOutDI
					g_nWaferSlideOutDI = 1;	// 0 on, 1 off
					nRet = g_nWaferSlideOutDI;
					*Result = TRUE;
					break;
			// Motor Driver Status DI Added 2008.09.08
			case 6 :	// NotDefine
					nRet = g_nMotorStatusDI;		// Motor  0 off, 1 on
					
					*Result = TRUE;
					break;
			default :
					break;
			}

			break;
		
	case 2 :
			switch(ID2)
			{
			case 1 :	// ShuttleInOutXI
// 					if     (1 == g_bStatusResponse[11])			g_nShuttleInOutXI = 1; // 0 Unknown, 1 In, 2 Out, 3 Error
// 					else if(1 == g_bStatusResponse[12])			g_nShuttleInOutXI = 2;
// 					else										g_nShuttleInOutXI = 0;

					nRet = g_nShuttleInOutXI;

					*Result = TRUE;
					break;
			case 2 :	// F_ProperlyPlaceDI
// 					if     (0 == g_bStatusResponse[28])			g_nF_ProperlyPlaceDI = 0;	// 0 Off, 1 On
// 					else if(1 == g_bStatusResponse[28])			g_nF_ProperlyPlaceDI = 1;
// 					else										g_nF_ProperlyPlaceDI = 0;

					nRet = g_nF_ProperlyPlaceDI;

					*Result = TRUE;
					break;
			case 3 :	// InterlockActiveDI
					nRet = g_nInterlockActiveDI;	// 0 Off, 1 On

					*Result = TRUE;
					break;
			case 4 :	// ClampUnclampXI
					// duraport clamp/unclamp firmware bug fixed. - 2007.05.28 ijbae.

// 					if     (1 == g_bStatusResponse[9])			g_nClampUnclampXI = 1; // 0 Unknown, 1 Clamp, 2 Unclamp, 3 Error
// 					else if(1 == g_bStatusResponse[10])			g_nClampUnclampXI = 2;
// 					else										g_nClampUnclampXI = 0;

					nRet = g_nClampUnclampXI;

					*Result = TRUE;
					break;
			case 5 :	// FOUP_PresentDI
// 					if     (0 == g_bStatusResponse[29])			g_nFOUP_PresentDI = 0;	// 0 Off, 1 On
// 					else if(1 == g_bStatusResponse[29])			g_nFOUP_PresentDI = 1;
// 					else										g_nFOUP_PresentDI = 0;

					nRet = g_nFOUP_PresentDI;

					*Result = TRUE;
					break;
			default :
					break;
			}

			break;

	case 3 :
			switch(ID2)
			{
			case 1 :	// Switch1UnloadDI
					nRet = g_nSwitch1UnloadDI;

					*Result = TRUE;
					break;
			case 2 :	// Switch2LoadDI
					nRet = g_nSwitch2LoadDI;

					*Result = TRUE;
					break;
			default :
					break;
			}

			break;

	case 4 :
			switch(ID2)
			{
			case 1 :	// 6MRunningNum
					nRet = g_bBusy;
					*Result = TRUE;
					break;
			case 2 :	// MappingStatusDI
					nRet = g_nMappingStatus;	// 0 None, 1 Mapping, 2 MapComplete, 3 MapFail
					*Result = TRUE;
					break;
			case 3 :	// ErrorStatusDI
					nRet = g_nErrorCode;
					*Result = TRUE;
					break;
			case 4 :	// ErrorDI
				nRet = g_nErrorCode;
					*Result = TRUE;
					break;
			default :
					break;
			}

			break;

	case 5 :	// Not Define
			switch(ID2)
			{
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
					nRet = g_nLampArray[ID2 - 1];	// 0 off, 1 on, 2 blink
					*Result = TRUE;
					break;
			default :
					break;
			}

			break;
	default :
			break;
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result )
{
	*Result = FALSE;
	DWORD dwTemp = 0;

	char szDataToSend[MAX_BUF];
	char szDataToReceive[MAX_BUF];

	memset(szDataToSend, 0, sizeof(szDataToSend));
	memset(szDataToReceive, 0, sizeof(szDataToReceive));

	switch(ID1)
	{
	case 1 :	// NotUse
			switch(ID2)
			{
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
			case 10 :
			case 11 :
					*Result = TRUE;
					break;
			default :
					break;
			}
			
			break;

	case 2 :
			switch(ID2)
			{
			case 1 :	// Mapping
					if(1 == Data)
					{
						if(TRUE == g_bThreadRun) { *Result = TRUE; break; }

						printf("===@~~ map Driver Start!! \n");
						g_nMappingStatus = 1;	// 0 None, 1 Mapping, 2 MapComplete, 3 MapFail
						CreateThread(NULL, 0, AutoMapRetry, NULL, 0, &dwTemp);						
					}
					else	g_nMappingStatus = 0;

					*Result = TRUE;
					break;
			case 2 :	// Read_Map_Info
					sprintf(szDataToSend, "GETMAP");
					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))	break;
					if(FALSE == ParseReceiveMapData(szDataToReceive))		break;

					*Result = TRUE;
					break;
			default :
					break;
			}
			
			break;
			
	case 3 :
			switch(ID2)
			{
			case 1 :	// ClampUnclampXO
					if     (1 == Data)			sprintf(szDataToSend, "POD_LOCK ON");
					else if(2 == Data)			sprintf(szDataToSend, "POD_LOCK OFF");
					else						break;
					
					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					g_nClampUnclampXI = Data;
					*Result = TRUE;
					break;

			case 2 :	// DockDO
					if(FULLDOCK == Data)
					{
						g_nMappingStatus = 1;
						sprintf(szDataToSend, "OPEN");
					}
					else if(SHUTTLEIN == Data)		sprintf(szDataToSend, "DOCK");
					else if(CLAMP == Data)			sprintf(szDataToSend, "POD_LOCK ON");
					else if(FULLDOCKMAP == Data)	sprintf(szDataToSend, "OPEN");
					else							break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					if((FULLDOCK == Data) || (FULLDOCKMAP == Data))
					{
						if(TRUE == ParseReceiveMapData(szDataToReceive))		g_nMappingStatus = 2;
						else													g_nMappingStatus = 3;
					}

					*Result = TRUE;

					//////////////////////////////////////////////////////////////////////////
					// Dock
					g_nF_ProperlyPlaceDI	= 1;	// 0 Off, 1 On
					g_nFOUP_PresentDI		= 1;	// 0 Off, 1 On
					g_nWaferSlideOutDI		= 1;	// 0 on, 1 off
					g_nInterlockActiveDI	= 0;	// 0 Off, 1 On
					g_nSwitch1UnloadDI		= 0;
					g_nSwitch2LoadDI		= 0;

					g_nClampUnclampXI		= 1;	// 0 Unknown, 1 Clamp, 2 Unclamp, 3 Error
					if(CLAMP == Data)		break;

					g_nShuttleInOutXI		= 1; 	// 0 Unknown, 1 In, 2 Out, 3 Error
					if(SHUTTLEIN == Data)	break;

					g_nLatchOpenCloseXI		= 1; 	// 0 Unknown, 1 Open, 2 Close, 3 Error	
					g_nVacuumOKDI			= 1;
					g_nPaddleExtRetXI		= 1;	// 0 unknown, 1 extend, 2 retract, 3 error
					g_nDoorUpDownXI			= 2;	// 0 Unknown, 1 Up, 2 Down, 3 Error
					
					g_nMappingStatus		= 2;	// 0 None, 1 Mapping, 2 MapComplete, 3 MapFail

					break;

			case 3 :	// UndockDO
					if     (FULLUNDOCK == Data)		sprintf(szDataToSend, "CLOSE");
					else if(CLOSEDOOR == Data)		sprintf(szDataToSend, "DR_CLOSE");
					else if(SHUTTLEOUT == Data)		sprintf(szDataToSend, "UNDOCK");
					else							break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					*Result = TRUE;

					//////////////////////////////////////////////////////////////////////////
					// Undock
					g_nF_ProperlyPlaceDI	= 1;	// 0 Off, 1 On
					g_nFOUP_PresentDI		= 1;	// 0 Off, 1 On
					g_nWaferSlideOutDI		= 1;	// 0 on, 1 off
					g_nInterlockActiveDI	= 0;	// 0 Off, 1 On
					g_nSwitch1UnloadDI		= 1;
					g_nSwitch2LoadDI		= 0;

					g_nDoorUpDownXI			= 1;	// 0 Unknown, 1 Up, 2 Down, 3 Error
					g_nPaddleExtRetXI		= 2;	// 0 Unknown, 1 extend, 2 retract, 3 error

					g_nVacuumOKDI			= 0;
					g_nLatchOpenCloseXI		= 2; 	// 0 Unknown, 1 Open, 2 Close, 3 Error	

					g_nShuttleInOutXI		= 2; 	// 0 Unknown, 1 In, 2 Out, 3 Error
					g_nClampUnclampXI		= 2; 	// 0 Unknown, 1 Clamp, 2 Unclamp, 3 Error
					g_nMappingStatus		= 2;	// 0 None, 1 Mapping, 2 MapComplete, 3 MapFail

					break;
			default :	
				break;
			}

			break;
	case 4 :
			switch(ID2)
			{
			case 1 :	// Fix6MInit
					sprintf(szDataToSend, "HOM");
					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					//////////////////////////////////////////////////////////////////////////
					// Undock
					g_nF_ProperlyPlaceDI	= 1;	// 0 Off, 1 On
					g_nFOUP_PresentDI		= 1;	// 0 Off, 1 On
					g_nWaferSlideOutDI		= 1;	// 0 on, 1 off
					g_nInterlockActiveDI	= 0;	// 0 Off, 1 On
					g_nSwitch1UnloadDI		= 1;
					g_nSwitch2LoadDI		= 0;
					
					g_nDoorUpDownXI			= 1;	// 0 Unknown, 1 Up, 2 Down, 3 Error
					g_nPaddleExtRetXI		= 2;	// 0 Unknown, 1 extend, 2 retract, 3 error

					g_nVacuumOKDI			= 0;
					g_nLatchOpenCloseXI		= 2; 	// 0 Unknown, 1 Open, 2 Close, 3 Error	
					
					g_nShuttleInOutXI		= 2; 	// 0 Unknown, 1 In, 2 Out, 3 Error
					g_nClampUnclampXI		= 2; 	// 0 Unknown, 1 Clamp, 2 Unclamp, 3 Error
					g_nMappingStatus		= 2;	// 0 None, 1 Mapping, 2 MapComplete, 3 MapFail

					*Result = TRUE;
					break;
			case 2 :	// Fix6MLog
					*Result = TRUE;
					break;
			case 3 :	// ErrorStatusDI
					g_nErrorCode = Data;
					*Result = TRUE;
					break;
			case 4 :	// ErrorResetDO
					sprintf(szDataToSend, "RESET");
					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					// reset error code
					g_nErrorCode = 0;
					memset(g_szErrorCode, 0, sizeof(g_szErrorCode));

					*Result = TRUE;
					break;
			case 5 :	// CTC.FA_STATUS_AGV_CM1
					if     (0 == Data)		sprintf(szDataToSend, "AUTO_MODE OFF");
					else if(1 == Data)		sprintf(szDataToSend, "AUTO_MODE ON");
					else					break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					*Result = TRUE;
					break;
			case 6 :	// MaintModeDO
					if     (0 == Data)		sprintf(szDataToSend, "MAINT_MODE OFF");
					else if(1 == Data)		sprintf(szDataToSend, "MAINT_MODE ON");
					else					break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					*Result = TRUE;
					break;
			case 7 :	// LPMSwitchUseDO
					*Result = TRUE;
					break;
			case 8 :	// MappingStatusDI
					g_nMappingStatus = Data;
					*Result = TRUE;
					break;

			// Motor Driver On/Off IO Addition 2008.08.25
			case 9 :	// MotorDriverDO
					if(0 == Data)			sprintf(szDataToSend, "AMPOFF");
					else if(1 == Data)		sprintf(szDataToSend, "AMPON");
					else					break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

					*Result = TRUE;
					break;
			default :
					break;
			}

			break;
	case 5 :	// FOUPDoor
			if     (Data == 0)		*Result = TRUE;						//Unknown
			else if(Data == 1)		sprintf( szDataToSend, "TCLOSE");	//Close
			else if(Data == 2)		sprintf( szDataToSend, "TOPEN");	// Open
			else					{ *Result = FALSE; return; }

			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))		break;

			*Result = TRUE;

			//////////////////////////////////////////////////////////////////////////
			if(Data == 1)
			{
				g_nDoorUpDownXI			= 1;	// 0 Unknown, 1 Up, 2 Down, 3 Error
				g_nPaddleExtRetXI		= 2;	// 0 Unknown, 1 extend, 2 retract, 3 error
				g_nVacuumOKDI			= 0;
				g_nLatchOpenCloseXI		= 1; 	// 0 Unknown, 1 Open, 2 Close, 3 Error	
				break;
			}
			
			if(Data == 2)
			{
				g_nLatchOpenCloseXI		= 2; 	// 0 Unknown, 1 Open, 2 Close, 3 Error	
				g_nVacuumOKDI			= 1;
				g_nPaddleExtRetXI		= 1;	// 0 Unknown, 1 extend, 2 retract, 3 error
				g_nDoorUpDownXI			= 2;	// 0 Unknown, 1 Up, 2 Down, 3 Error
				break;
			}

			break;
// INDICATOR LED
	case 6 :
			switch(ID2)
			{	// Lamp1OnDIO ~ Lamp11OnDIO
			case 1 :	// Lamp1OnDIO
					if     (0 == Data)		sprintf(szDataToSend, "INDICATOR_LOAD OFF");
					else if(1 == Data)		sprintf(szDataToSend, "INDICATOR_LOAD ON");
					else					break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))	break;

					*Result = TRUE;
					break;
			case 2 :	// Lamp2OnDIO
					if     (0 == Data)		sprintf(szDataToSend, "INDICATOR_UNLOAD OFF");
					else if(1 == Data)		sprintf(szDataToSend, "INDICATOR_UNLOAD ON");
					else					break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))	break;

					*Result = TRUE;
					break;
			case 3 :
					if     (0 == Data)		sprintf(szDataToSend, "INDICATOR_RESERVE OFF");
					else if(1 == Data)		sprintf(szDataToSend, "INDICATOR_RESERVE ON");
					else					break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))	break;

					*Result = TRUE;
					break;
			case 4 :
					if     (0 == Data)		sprintf(szDataToSend, "INDICATOR_AUTO OFF");
					else if(1 == Data)		sprintf(szDataToSend, "INDICATOR_AUTO ON");
					else					break;

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))	break;

					*Result = TRUE;
					break;
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
			case 10 :
			case 11 :
					*Result = TRUE;
					break;
			default :
					break;
			}
			
			break;
	default :
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
void String_In( int ID1 , char *data , int *Result ) 
{
	*Result = FALSE;
	int nSlot = 0;

	switch(ID1)
	{
	case 1 :	// WaferMap
			char szWaferInfo[MAX_BUF];
			char szTotalWaferInfo[MAX_BUF];

			memset(szWaferInfo, 0, sizeof(szWaferInfo));
			memset(szTotalWaferInfo, 0, sizeof(szTotalWaferInfo));

			sprintf(data, "2222222222222222222222222");

			*Result = TRUE;
			break;
	case 2 :	// STSErrorCode
	case 3 :	// CMDErrorCode
	case 4 :	// EVTErrorCode
			if(99 > strlen(g_szErrorCode))
			{
				strcpy(data, g_szErrorCode);
			}
			else
			{
				strncpy(data, g_szErrorCode, 99); // only copy 99 char
				strcpy(data + 100, "\0");
			}

			*Result = TRUE;
			break;
	case 5 :	// EventID
			if(99 > strlen(g_szEventCode))
			{
				strcpy(data, g_szEventCode);
			}
			else
			{
				strncpy(data, g_szEventCode, 99); // only copy 99 char
				strcpy(data + 100, "\0");
			}

			*Result = TRUE;
			break;
	default :
			break;
	}
}

void String_Out( int ID1 , char *data , int *Result ) 
{
	*Result = TRUE;
}

double Analog_Name_In( char *Name , int *Result ) 
{
	*Result = TRUE;
	return 0;
}

void Analog_Name_Out( char *Name , double data , int *Result ) 
{
	*Result = TRUE;
}

int Digital_Name_In( char *Name , int *Result ) 
{
	*Result = TRUE;
	return 0;
}

void Digital_Name_Out( char *Name , int data , int *Result ) 
{
	*Result = TRUE;
}

void String_Name_In( char *Name , char *data , int *Result ) 
{
	*Result = TRUE;
}

void String_Name_Out( char *Name , char *data , int *Result ) 
{
	*Result = TRUE;
}

}// End of [extern "C"]