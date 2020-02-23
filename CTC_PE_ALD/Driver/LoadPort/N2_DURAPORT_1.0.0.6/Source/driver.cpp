
#pragma warning(disable: 4786)

#include <vector>
#include <string>

using namespace std;

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
int g_nMotorStatusDI		=	0;

int g_nSwitch1UnloadDI		= 	0;			
int g_nSwitch2LoadDI		=	0;	

int g_nMappingStatus		=	0;
int g_nErrorCode			=	0;
int g_nErrorDI				=	0;
int g_nFoupHoleSts			=	0;

typedef enum 
{
	PRE_PURGE
	, PROC_PURGE
	, POST_PURGE
	, TEMP_PRE_PURGE
	, POSTPOST_PURGE
	, NOZZLE_BLOW
	, IDLE_STATUS
	, MANUAL_PURGE
	, ERROR_STATUS
} PURGE_STATUS;

typedef struct  
{
	int TP_CONNECT;			
	int TP_DISCONNECT;			
	int OP_ACCESS_BTN_PUSH;	
	
	int FOUP_IN_STS;			
	int FOUP_OUT_STS;
	
	int RESET_START;
	int RESET_END;
	
	int N2_FLOW_ALARM;			
	int N2_PRES_ALARM;			
	int N2_COMM_ALARM;
	
	int FOUP_START_READY;
	int FOUP_INCORRECT_POS;
	
	int N2_UNCLAMP_ALARM;
	
	int EMG_STOP_PUSH;
	int O2SENSOR_FAIL;
	int FFU_SIGNAL_FAIL;
	int O2SENSOR_WARNING;
} LP_E_STS;
LP_E_STS	LP_EVT;

typedef struct 
{
	int PRE_START		  ;	
	int PRE_END			  ;
	int PRE_STOP	      ;       
	int PRE_WARNING       ;      
	
	int PROC_START		  ;	
	int PROC_END		  ;
	int PROC_STOP	      ;       
	int PROC_WARNING      ;      
	
	int POST_START		  ;	
	int POST_END		  ;
	int POST_STOP	      ;       
	int POST_WARNING      ;      
	
	int TPRE_START		  ;	
	int TPRE_END		  ;
	int TPRE_STOP	      ;       
	int TPRE_WARNING      ;      
	
	int PPOST_START		  ;	
	int PPOST_END		  ;
	int PPOST_STOP	      ;       
	int PPOST_WARNING     ;      
	
	int BLOW_START		  ;	
	int BLOW_END		  ;
	int BLOW_STOP	      ;       
	int BLOW_WARNING      ;      
	
	int PURGE_START		  ;	
	int PURGE_END		  ;
	int PURGE_STOP	      ;       
	int PURGE_WARNING     ;
} N2LP_STATUS;
N2LP_STATUS	N2_STS;

// N2 Status
typedef struct 
{
	PURGE_STATUS time;
	double N2Flow1;
	double N2Flow2;
	double N2MainPrs;
	double N2ExhaustPrs;
	double OxygenDensity;
} N2_STATUS;

N2_STATUS g_N2Status;

// state
BOOL    g_bCommStatus = FALSE;
BOOL	g_bBusy = FALSE;

int g_nEQC_Wafer_Info[MAX_BUF];
const int MAX_SLOT = 25;

char g_szErrorCode[MAX_BUF];
char g_szEventCode[MAX_BUF];
char g_szPurgeEventCode[MAX_BUF];

char g_szCustomRequest[MAX_BUF];
char g_szCustomResponse[MAX_BUF];

BOOL g_bStatusResponse[32];
int  g_nLampArray[10];
BOOL g_bThreadRun = FALSE;

//2013.04.29
int g_nLogSts ;

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

typedef enum _LOG_ENUM
{
	Log_Off,
	Log_On,
}LOG_ENUM;

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

const int POLLING_DELAY = 100;

BOOL Rs232_IO(char *szDataToSend , char *szDataToReceive);
BOOL Rs232_IO2(char *szDataToSend , char *szDataToReceive);
void PollingIOChannels();
BOOL ParseReceiveStateData(char* szDataToReceive);
BOOL ParseReceiveMapData(char* szDataToReceive);
BOOL ParseReceiveLampData(char* szDataToReceive);
BOOL ParseReceiveN2PurgeData(char* szDataToReceive);
BOOL UpdatePurgeEvent(char *szEventCode);
BOOL DetectError(char *szDataToReceive);
BOOL DetectEvent(char *szDataToReceive);
BOOL LogProtocolData(char *szSendData, char *szReceiveData);
BOOL LogProtocolSendData(char *szSendData);
BOOL LogProtocolRcvData(char *szReceiveData);
BOOL LogProtocolRcvDataEx(char* szReceiveData);
int  GetBitStatus(char source, int pos);

unsigned long __stdcall AutoMapRetry(void *pVoid)
{
	g_bThreadRun = TRUE;

	char szDataToSend[MAX_BUF];
	char szDataToReceive[MAX_BUF];

	WaitForSingleObject(g_hMutex, INFINITE);

	do {

		// Command Change 2008.08.25
		// sprintf(szDataToSend, "DN_MAPPING");	
		sprintf(szDataToSend, "SCAN DN");						
		if(FALSE == Rs232_IO2(szDataToSend, szDataToReceive))
		{
			g_nMappingStatus = 3;
			//_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
			break;
		}

		g_nMappingStatus = 2;
	} while (0);

	ReleaseMutex(g_hMutex);
	printf("===@~~ map Driver End!! \n"); 

	g_bThreadRun = FALSE;
	return 0L;
}

unsigned long __stdcall ThreadProcDevice(void *pVoid)
{
	g_bThreadRunning = TRUE;

	PollingIOChannels();

	g_bThreadRunning = FALSE;
	return 0L;
}

void PollingIOChannels()
{
	char szDataToSend[MAX_BUF];
	char szDataToReceive[MAX_BUF];

	while(!g_bThreadTerminate)
	{
		do
		{
			// Status Update
			memset(szDataToSend, 0, sizeof(szDataToSend));
			memset(szDataToReceive, 0, sizeof(szDataToReceive));

			strcpy(szDataToSend, "STATUS");
			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
			{

				break;
			}
			// parsing receive data
			if(FALSE == ParseReceiveStateData(szDataToReceive))
			{
				break;
			}
		} while(0);			
		
		
		do
		{	// Indicator Status Update
			memset(szDataToSend, 0, sizeof(szDataToSend));
			memset(szDataToReceive, 0, sizeof(szDataToReceive));

			strcpy(szDataToSend, "STATUS_INDICATOR");
			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
			{			
				break;
			}
			
			// parsing receive data
			if(FALSE == ParseReceiveLampData(szDataToReceive))
			{			
				break;
			}
		} while(0);

		do
		{	// N2 Purge Status Update
			memset(szDataToSend, 0, sizeof(szDataToSend));
			memset(szDataToReceive, 0, sizeof(szDataToReceive));

			strcpy(szDataToSend, "STATUS_N2");
			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
			{			
				break;
			}
			
			// parsing receive data
			if(FALSE == ParseReceiveN2PurgeData(szDataToReceive))
			{			
				break;
			}
		} while(0);

		do
		{	// N2 Purge Status Update
			memset(szDataToSend, 0, sizeof(szDataToSend));
			memset(szDataToReceive, 0, sizeof(szDataToReceive));
			
			strcpy(szDataToSend, "CTYPE");
			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
			{			
				break;
			}
			
			// parsing receive data
			if(FALSE == ParseReceiveN2PurgeData(szDataToReceive))
			{			
				break;
			}
		} while(0);
	
		Sleep(POLLING_DELAY);
	} 
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

BOOL ParseReceiveStateData(char* szDataToReceive)
{
	BOOL BRet = FALSE;

	do
	{
		// response data
		//
		//      12345678
		// ex) S12345678\n
		//

		char *szResponse = NULL;
		char szToken[] = "S";

		szResponse = strtok(szDataToReceive, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to find 'S' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}

		// get real response
		szResponse = strtok(NULL, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to get real response in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}

		// convert hex data from ascii
		uHexValue uHexBuffer;
		uHexValue uHexRev;



		AtoH(szResponse, (char *)&uHexBuffer.szHexValue, 4);
		//nHexData = uHexBuffer.nHexValue;

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
		int nEndIndex = sizeof(uHexRev.szHexValue) - 1;

		for(nIndex = 0; nIndex < 4; nIndex ++)
		{
			uHexRev.szHexValue[nEndIndex - nIndex] = uHexBuffer.szHexValue[nIndex];
		}

		//  Data of uHexRev, after for(...) loop.
		//
		//	uHexRev.szHexValue    :   0  1  2  3
		//						      78 56 34 12 
		//
		//  uHexRev.nHexValue     :   0x12345678


		unsigned int nHexData = 0;
		nHexData = uHexRev.nHexValue;


		// convert "bit status" from "total response hex data".
		for(nIndex = 0; nIndex < 32; nIndex ++)
		{
			nShiftData = ( nHexData >> nIndex);
			BData = nShiftData & 0x1;
			g_bStatusResponse[nIndex] = BData;
		}


		BRet = TRUE;
	}while(0);

	return BRet;
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
		char szWaferInfopre[MAX_BUF];
		char szTotalWaferInfopre[MAX_BUF];
		/*char szRevBuffer[MAX_BUF];
		memset(szRevBuffer, 0, sizeof(szRevBuffer));
		strcpy(szRevBuffer, szDataToReceive);*/

		szResponse = strtok(szDataToReceive, szToken);
		if(NULL == szResponse)
		{
			_LOG("Failed to find 'M' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
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
			_LOG("Failed to find ',' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
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
			_LOG("Failed to find ',' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
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
			_LOG("Failed to find ',' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
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
		
		// 2013.04.29 Map Data Logging 
		for(int nSlot = 0; nSlot < MAX_SLOT; nSlot ++)
		{
			sprintf(szWaferInfopre, "%1.0d", g_nEQC_Wafer_Info[nSlot]);
			strcat(szTotalWaferInfopre, szWaferInfopre);
		}
		_LOG("MapData : %s",szTotalWaferInfopre );

		BRet = TRUE;
	}while(0);

	if(FALSE == BRet)
	{
		_LOG("Failed to function ParseReceiveMapData(). (%d)", __LINE__);
	}

	return BRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL ParseReceiveLampData(char* szDataToReceive)
{
	BOOL BRet = FALSE;

	do
	{
		// response data
		//
		//      12345678
		// ex) S12345678\n
		//

		char *szResponse = NULL;
		char szToken[] = "S";


		szResponse = strtok(szDataToReceive, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to find 'S' character in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}

		// get real response
		szResponse = strtok(NULL, szToken);
		if(NULL == szResponse)
		{
//			_LOG("Failed to get real response in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}


		int nTotalLampSize = 8;

		if(nTotalLampSize <= (int) strlen(szResponse))
		{
			char szLamp[40];
			int nOneLampSize = 1;

			memset(szLamp, 0, sizeof(szLamp));
			memset(g_nLampArray, 0, sizeof(g_nLampArray));

			for(int nLampIndex = 0; nLampIndex < nTotalLampSize; nLampIndex ++)
			{
				memset(szLamp, 0, sizeof(szLamp));

				// get one lamp data
				strncpy(szLamp, szResponse + nLampIndex, nOneLampSize);

				g_nLampArray[nLampIndex] = atoi(szLamp);
			}

			/*
			_LOG("Lamp Data : '%d' '%d' '%d' '%d' '%d' '%d' '%d' '%d'", g_nLampArray[0],
																		g_nLampArray[1],
																		g_nLampArray[2],
																		g_nLampArray[3],
																		g_nLampArray[4],
																		g_nLampArray[5],
																		g_nLampArray[6],
																		g_nLampArray[7] );
			*/
		}
		else
		{
//			_LOG("Failed to get all lamp data in received data. szDataToReceive : '%s'. (%d)", szDataToReceive, __LINE__);
			break;
		}


		BRet = TRUE;
	}while(0);

	return BRet;
}

int SplitString( string text, vector<string> &words )
{
	int i=0;
	char ch;
	string word;

	words.clear();
	while(ch=text[i++])
	{
		if (isspace(ch))
		{
			if (!word.empty())
			{
				words.push_back(word);
			}
			word = "";
		}
		else
		{
			word += ch;
		}
	}
	if (!word.empty())
	{
		words.push_back(word);
	}

	return words.size();
}

BOOL ParseReceiveN2PurgeData(char* szDataToReceive)
{
	// N2 6 -0.05 -0.11 N 493.81 N N 0.00 N
	char *pStr = strstr(szDataToReceive, "N2");
	vector<string> words;	
	
	if( pStr == NULL )
	{
		return FALSE;
	}

	if( SplitString(pStr, words) >= 10 )
	{
		g_N2Status.time = (PURGE_STATUS)atoi(words[1].data());
		g_N2Status.N2Flow1 = (words[2] != "N") ? atof(words[2].data()) : 0;
		g_N2Status.N2Flow2 = (words[3] != "N") ? atof(words[3].data()) : 0;
		g_N2Status.N2MainPrs = (words[5] != "N") ? atof(words[5].data()) : 0;
		g_N2Status.N2ExhaustPrs = (words[8] != "N") ? atof(words[8].data()) : 0;
		g_N2Status.OxygenDensity = (words[9] != "N") ? atof(words[9].data()) : 0;
	}
	
	return TRUE;
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
	memset(g_szPurgeEventCode, 0, sizeof(g_szPurgeEventCode));
	memset(szDataToSend, 0, sizeof(szDataToSend));
	memset(szDataToReceive, 0, sizeof(szDataToReceive));
	memset(g_bStatusResponse, 0, sizeof(g_bStatusResponse));
	memset(g_nLampArray, 0, sizeof(g_nLampArray));

	memset(&LP_EVT, 0, sizeof(LP_E_STS));
	memset(&N2_STS, 0, sizeof(N2LP_STATUS));

	strcpy(g_strSendTerm, "\n");
	strcpy(g_strRecvTerm, "\n");
	strcpy(g_strRecvTerm2, "");

	g_nLogSts = Log_Off;


/*	do
	{
		Sleep(100);
	}while(1);*/

	do
	{
		// set time out

		if ( ID10 < 100 ) 
		{
			g_nTimeOut = 100;
		}
		else
		{
			g_nTimeOut = ID10;
		}


		// create mutex

		char szMutexName[MAX_BUF];
		memset(szMutexName, 0, sizeof(szMutexName));

		// Mutex string name : (drivername)_(portnumber)
		//       ex) "duraport_12"

		sprintf(szMutexName, "%s_%d", APP_NAME, ID1);

		g_hMutex = CreateMutex(NULL, FALSE, szMutexName);
		if(NULL == g_hMutex)
		{
			_LOG("Failed to create mutex. (%d)", __LINE__);
			break;
		}


		// init log file
		char szLogFileName[MAX_BUF];
		char szCfgFileName[MAX_BUF];

		memset(szLogFileName, 0, sizeof(szLogFileName));

		// log file name : _Log\\(drivername)_(portname).log
		//				   ex) "_Log\\duraport_12.log"

		sprintf(szLogFileName, "f:\\Datalog\\_Log\\%s.log", szMutexName);

		sprintf(szCfgFileName, "Driver\\DURAPORT.cfg");		
		
		_gInitLogDll(szLogFileName, szCfgFileName, TRUE);

		
		// com port open

		if ( RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ) ) 
		{
			g_bCommStatus = TRUE;

			RS232_Clear_Buffer();

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
			// 0. Motor Servo On
			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "AMPON");
			Rs232_IO(szDataToSend, szDataToReceive);

			// 1. Foup Place Sensor On
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

			//... 2015.04.27 Add 2015 Common Spec Config Setting
			//... FAN ALARM OFF Reference MAHA-MP
			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "CFG_FAN_ERR OFF");
			Rs232_IO(szDataToSend, szDataToReceive);

			//... FAN ALARM OFF Reference MAHA-MP
			// 12. Fan Alarm (LPM)
			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "CFG_FAN_BIT ON");
			Rs232_IO(szDataToSend, szDataToReceive);
			
			// 13. Auto Clamp Disable 
			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "CFG_AUTO_UNCLAMP OFF");
			Rs232_IO(szDataToSend, szDataToReceive);
			
			// 14. Parameter save to EEPROM in DuraPort
			memset(szDataToSend, 0, sizeof(szDataToSend));
			strcpy(szDataToSend, "SAVE_ALL");
			Rs232_IO(szDataToSend, szDataToReceive);
			
			RS232_Clear_Buffer();
		}
		else
		{
			_LOG("Dura Init failure!!!. (%d)", __LINE__);
			break;
		}


		//////////////////////////////////////////////////////////////////////////
		// create polling thread
		g_hThread = CreateThread(NULL, 0, ThreadProcDevice, NULL, 0, &g_dwThreadID);
		if(NULL != g_hThread)
		{
			int nElapsedTime = 0; // 0 ms
			int nWaitTime = 5000; // 5000 ms

			do
			{
				if(TRUE == g_bThreadRunning)
				{
					break;
				}
				else
				{
					nElapsedTime += 100;
					Sleep(100);
				}

			}while(nWaitTime > nElapsedTime);

			if(TRUE != g_bThreadRunning)
			{
				_LOG("Failed to wait thread initialize. (%d)", __LINE__);
				break;
			}
		}
		else
		{
			_LOG("Failed to create thread. (%d)", __LINE__);
			break;
		}

		if(TRUE != g_bThreadRunning)
		{
			_LOG("Failed to run thread. (%d)", __LINE__);
			break;
		}

		printf("Dura Init success!!! (%d) \n", __LINE__);
		_LOG("Dura Init success!!!. (%d)", __LINE__);

		BRet = TRUE;
	}while(0);

	return TRUE; //... 2018.01.04 연결 안되도 그냥 로딩시킨다.
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

	RS232_Disconnect_Port();

	_LOG("Dura Is Detached successfully!!!. (%d)", __LINE__);

	_gCLOSE_LOG();

	return TRUE;
}

BOOL CheckReceiveDataAndClearBuffer()
{
	BOOL bRet = FALSE;

	char szDataToReceive[MAX_BUF];
	char szReceiveBuffer[MAX_BUF];

	//int  nFoupSts = 0;
	//enum {Unknown, Absent, Nozzle2, Nozzle4};

	memset(szDataToReceive, 0, sizeof(szDataToReceive));
	memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));

	do
	{
		int nReadCount = 0;

		// get data, if receive event from LPM device

		/*if(FALSE == RS232_Read_Char(szDataToReceive))
		{
			break;
		}*/
		//RS232_Read_Char(szDataToReceive);


		int nRevMax = 10;
		int nRevCnt = 0;

		do
		{
			// read com port data, if exit.
			if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 20, &nReadCount) )
			{
				// check whether com port buffer is empty

				if(0 != strlen(szReceiveBuffer))
				{
					strcat(szDataToReceive, szReceiveBuffer);
					//strcat(szDataToReceive, "\n");
					//strcat(szDataToReceive, g_strRecvTerm2);
				}

				// check whether total receive data is empty.
				if(0 == strlen(szDataToReceive))
				{
					// if (total receive) data length is zero, then get out of loop.
					break;
				}
			}
			else
			{
				strcat(szDataToReceive, szReceiveBuffer);
				//strcat(szDataToReceive, "\n");
				strcat(szDataToReceive, g_strRecvTerm);
			}

			// check whether all data is received
			char *strResponse = NULL;
			strResponse = strstr(szDataToReceive, g_strRecvTerm);	// check first 'terminal string'
			if(NULL != strResponse)
			{
				//strResponse = NULL;
				//strResponse = strstr(szDataToReceive, g_strRecvTerm);	// check second 'terminal string'

				//if(NULL != strResponse)
				//{
					break;
				//}
			}
			else
			{
				; // continue to read
			}

			if(nRevCnt >= nRevMax)
			{
				//_LOG("It is over the allowable receive count limit. (%d)", __LINE__);
				break;
			}
			else
			{
				nRevCnt ++;
			}

		}while(1);


		
		/*// get 2nd response data
		if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 100, &nReadCount) )
		{
			break;
		}

		strcat(szDataToReceive, szReceiveBuffer);
		strcat(szDataToReceive, "\n");

		//strcpy(Response, szDataToReceive);*/


		// check data size
		int nDataLen = strlen(szDataToReceive);

		// log receive data
		if(0 != nDataLen)
		{
			//LogProtocolRcvData(szDataToReceive);
			if(g_nLogSts) LogProtocolRcvDataEx(szDataToReceive);
		}


		// check error code or event code
		if(0 == nDataLen)												// no error code and event data
		{
			//_LOG("Data length is zero. (%d)", __LINE__);
			break;
		}
		else if( (0 < nDataLen) && ((int)strlen("C00000001\r") > nDataLen) ) // no error code and no event data
		{
			break;
		}
		else if(MAX_BUF <= nDataLen)											// has error or event, but make terminal string
		{
//			_LOG("Data length is big. Data length : '%d'. (%d)", nDataLen, __LINE__);
			*(szDataToReceive + MAX_BUF - 1) = NULL;
		}
		else															// has error or event
		{
			;
		}
/*
		if(TRUE == DetectError(szDataToReceive))
		{
			//bRet = TRUE;
			//break; // no break; check event also.
		}
*/		
		if(TRUE == DetectEvent(szDataToReceive))
		{
			//bRet = TRUE;
			//break; // no break;
		}

		//bRet = TRUE;
	}while(0);

	// clear buffer

	//RS232_Clear_Buffer();		//<- never use this fn(RS232_Clear_Buffer) in Duraport driver.
								//   if use, get lost data(event or error code) from duraport

	bRet = TRUE;

	return bRet;
}

void UpdateCassetEvent(char* szEventCode)
{
	if(NULL == szEventCode)
	{
		return;
	}
	// event sample response data
	//          1         2         3
	// 123456789012345678901234567890
	// C00000001

	*(szEventCode + strlen("C00000001\r") - 1) = NULL; // set '\n' to be null.

	memset(g_szEventCode, 0, sizeof(g_szEventCode));
	memset(&LP_EVT, 0, sizeof(LP_E_STS));

	if(0 == strncmp(szEventCode, "C00000001", strlen("C00000001")))
	{
		sprintf(g_szEventCode, "%s - TP Connected", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00000002", strlen("C00000002")))
	{
		sprintf(g_szEventCode, "%s - TP Disconnected", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00000004", strlen("C00000004")))
	{
		sprintf(g_szEventCode, "%s - Load Button Pushed", szEventCode);

		g_nSwitch1UnloadDI = 0;
		g_nSwitch2LoadDI = 1;
	}
	// 메뉴얼 상에는 bit3 미존재?
	else if(0 == strncmp(szEventCode, "C00000008", strlen("C00000008")))
	{
		sprintf(g_szEventCode, "%s - Unload Button Pushed", szEventCode);
		
		g_nSwitch1UnloadDI = 1;
		g_nSwitch2LoadDI = 0;
	}
	else if(0 == strncmp(szEventCode, "C00000010", strlen("C00000010")))
	{
		sprintf(g_szEventCode, "%s - Foup load", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00000020", strlen("C00000020")))
	{
		sprintf(g_szEventCode, "%s - Foup unload", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00000100", strlen("C00000100")))
	{
		sprintf(g_szEventCode, "%s - Reset start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00000200", strlen("C00000200")))
	{
		sprintf(g_szEventCode, "%s - Reset end", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00002000", strlen("C00002000")))
	{
		sprintf(g_szEventCode, "%s - N2 Purge Flow Alarm", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00004000", strlen("C00004000")))
	{
		sprintf(g_szEventCode, "%s - N2 Purge Press Alarm", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00008000", strlen("C00008000")))
	{
		sprintf(g_szEventCode, "%s - N2 Purge COM Alarm", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00010000", strlen("C00010000")))
	{
		sprintf(g_szEventCode, "%s - Foup started", szEventCode);
	}
	// Event Message Addition 2008.08.25
	else if(0 == strncmp(szEventCode, "C00020000", strlen("C00020000")))
	{
		sprintf(g_szEventCode, "%s - Foup Incorrect Postion", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C00080000", strlen("C00080000")))
	{
		sprintf(g_szEventCode, "%s - N2 Purge Unclamp Alarm", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C01000000", strlen("C01000000")))
	{
		sprintf(g_szEventCode, "%s - ESTOP Pushed", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C02000000", strlen("C02000000")))
	{
		sprintf(g_szEventCode, "%s - O2 Sensor Failed", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C04000000", strlen("C04000000")))
	{
		sprintf(g_szEventCode, "%s - FFU SIGNAL FAILED", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "C08000000", strlen("C08000000")))
	{
		sprintf(g_szEventCode, "%s - O2 SENSOR WARNING", szEventCode);
	}
	else
	{
		sprintf(g_szEventCode, "%s - Not defined", szEventCode);
	}

	LP_EVT.TP_CONNECT			= GetBitStatus(szEventCode[8], 0);	// bit 0 
	LP_EVT.TP_DISCONNECT		= GetBitStatus(szEventCode[8], 1);  // bit 1
	LP_EVT.OP_ACCESS_BTN_PUSH	= GetBitStatus(szEventCode[8], 2);  // bit 2

	LP_EVT.FOUP_IN_STS			= GetBitStatus(szEventCode[7], 0);	// bit 4
	LP_EVT.FOUP_OUT_STS			= GetBitStatus(szEventCode[7], 1);  // bit 5

	LP_EVT.RESET_START			= GetBitStatus(szEventCode[6], 0);	// bit 8
	LP_EVT.RESET_END			= GetBitStatus(szEventCode[6], 1);	// bit 9

	LP_EVT.N2_FLOW_ALARM		= GetBitStatus(szEventCode[5], 1);	// bit 13
	LP_EVT.N2_PRES_ALARM		= GetBitStatus(szEventCode[5], 2);  // bit 14
	LP_EVT.N2_COMM_ALARM		= GetBitStatus(szEventCode[5], 3);  // bit 15

	LP_EVT.FOUP_START_READY		= GetBitStatus(szEventCode[4], 0);	// bit 16
	LP_EVT.FOUP_INCORRECT_POS	= GetBitStatus(szEventCode[4], 1);	// bit 17
	LP_EVT.N2_UNCLAMP_ALARM		= GetBitStatus(szEventCode[4], 3);	// bit 19

	LP_EVT.EMG_STOP_PUSH		= GetBitStatus(szEventCode[2], 0);	// bit 24
	LP_EVT.O2SENSOR_FAIL		= GetBitStatus(szEventCode[2], 1);	// bit 25
	LP_EVT.FFU_SIGNAL_FAIL		= GetBitStatus(szEventCode[2], 2);	// bit 26
	LP_EVT.O2SENSOR_WARNING		= GetBitStatus(szEventCode[2], 3);	// bit 27
}

BOOL UpdatePurgeEvent(char *szEventCode)
{
	BOOL bRet = TRUE;
	if (NULL == szEventCode)	
	{
		bRet = FALSE;
		return bRet;
	}

	*(szEventCode + strlen("P00000001\r") - 1) = NULL; // set '\n' to be null.

	memset(g_szPurgeEventCode, 0, sizeof(g_szPurgeEventCode));
	memset(&N2_STS, 0, sizeof(N2LP_STATUS));

	if(0 == strncmp(szEventCode, "P00000001", strlen("P00000001")))
	{
		sprintf(g_szPurgeEventCode, "%s - Pre Purge Start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000002", strlen("P00000002")))
	{
		sprintf(g_szPurgeEventCode, "%s - Pre Purge End", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000004", strlen("P00000004")))
	{
		sprintf(g_szPurgeEventCode, "%s - Pre Purge Stop", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000008", strlen("P00000008")))
	{
		sprintf(g_szPurgeEventCode, "%s - Pre Purge Warning", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000010", strlen("P00000010")))
	{
		sprintf(g_szPurgeEventCode, "%s - Process Purge Start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000020", strlen("P00000020")))
	{
		sprintf(g_szPurgeEventCode, "%s - Process Purge End", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000040", strlen("P00000040")))
	{
		sprintf(g_szPurgeEventCode, "%s - Process Purge Stop", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000080", strlen("P00000080")))
	{
		sprintf(g_szPurgeEventCode, "%s - Process Purge Warning", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000100", strlen("P00000100")))
	{
		sprintf(g_szPurgeEventCode, "%s - Post Purge Start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000200", strlen("P00000200")))
	{
		sprintf(g_szPurgeEventCode, "%s - Post Purge End", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000400", strlen("P00000400")))
	{
		sprintf(g_szPurgeEventCode, "%s - Post Purge Stop", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00000800", strlen("P00000800")))
	{
		sprintf(g_szPurgeEventCode, "%s - Post Purge Warning", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00001000", strlen("P00001000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Temporary Pre Purge Start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00002000", strlen("P00002000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Temporary Pre Purge End", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00004000", strlen("P00004000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Temporary Pre Purge Stop", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00008000", strlen("P00008000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Temporary Pre Purge Warning", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00010000", strlen("P00010000")))
	{
		sprintf(g_szPurgeEventCode, "%s - PostPost Purge Start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00020000", strlen("P00020000")))
	{
		sprintf(g_szPurgeEventCode, "%s - PostPost Purge End", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00040000", strlen("P00040000")))
	{
		sprintf(g_szPurgeEventCode, "%s - PostPost Purge Stop", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00080000", strlen("P00080000")))
	{
		sprintf(g_szPurgeEventCode, "%s - PostPost Purge Warning", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00100000", strlen("P00100000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Blow Purge Start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00200000", strlen("P00200000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Blow Purge End", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00400000", strlen("P00400000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Blow Purge Stop", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P00800000", strlen("P00800000")))
	{
		sprintf(g_szPurgeEventCode, "%s - Blow Purge Warning", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P01000000", strlen("P01000000")))
	{
		sprintf(g_szPurgeEventCode, "%s -  Purge Start", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P02000000", strlen("P02000000")))
	{
		sprintf(g_szPurgeEventCode, "%s -  Purge End", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P04000000", strlen("P04000000")))
	{
		sprintf(g_szPurgeEventCode, "%s -  Purge Stop", szEventCode);
	}
	else if(0 == strncmp(szEventCode, "P08000000", strlen("P08000000")))
	{
		sprintf(g_szPurgeEventCode, "%s -  Purge Warning", szEventCode);
	}
	else
	{
		sprintf(g_szPurgeEventCode, "%s - Not defined", szEventCode);
		bRet = FALSE;
	}

	N2_STS.PRE_START		=	GetBitStatus(szEventCode[8], 0);
	N2_STS.PRE_END			=	GetBitStatus(szEventCode[8], 1);
	N2_STS.PRE_STOP			=	GetBitStatus(szEventCode[8], 2);
	N2_STS.PRE_WARNING		=	GetBitStatus(szEventCode[8], 3);

	N2_STS.PROC_START		=	GetBitStatus(szEventCode[7], 0);
	N2_STS.PROC_END			=	GetBitStatus(szEventCode[7], 1);
	N2_STS.PROC_STOP		=	GetBitStatus(szEventCode[7], 2);
	N2_STS.PROC_WARNING		=	GetBitStatus(szEventCode[7], 3);

	N2_STS.POST_START		=	GetBitStatus(szEventCode[6], 0);
	N2_STS.POST_END			=	GetBitStatus(szEventCode[6], 1);
	N2_STS.POST_STOP		=	GetBitStatus(szEventCode[6], 2);
	N2_STS.POST_WARNING		=	GetBitStatus(szEventCode[6], 3);

	N2_STS.TPRE_START		=	GetBitStatus(szEventCode[5], 0);
	N2_STS.TPRE_END			=	GetBitStatus(szEventCode[5], 1);
	N2_STS.TPRE_STOP		=	GetBitStatus(szEventCode[5], 2);
	N2_STS.TPRE_WARNING		=	GetBitStatus(szEventCode[5], 3);

	N2_STS.PPOST_START		=	GetBitStatus(szEventCode[4], 0);
	N2_STS.PPOST_END		=	GetBitStatus(szEventCode[4], 1);
	N2_STS.PPOST_STOP		=	GetBitStatus(szEventCode[4], 2);
	N2_STS.PPOST_WARNING	=	GetBitStatus(szEventCode[4], 3);

	N2_STS.BLOW_START		=	GetBitStatus(szEventCode[3], 0);
	N2_STS.BLOW_END			=	GetBitStatus(szEventCode[3], 1);
	N2_STS.BLOW_STOP		=	GetBitStatus(szEventCode[3], 2);
	N2_STS.BLOW_WARNING		=	GetBitStatus(szEventCode[3], 3);

	N2_STS.PURGE_START		=	GetBitStatus(szEventCode[2], 0);
	N2_STS.PURGE_END		=	GetBitStatus(szEventCode[2], 1);
	N2_STS.PURGE_STOP		=	GetBitStatus(szEventCode[2], 2);
	N2_STS.PURGE_WARNING	=	GetBitStatus(szEventCode[2], 3);

	return bRet;
}


BOOL DetectEvent(char *szDataToReceive)
{
	// Declaration.
	BOOL bRet = FALSE;
	char szEventCodeBuffer[MAX_BUF];
	int nDataLen = 0;

	// Initialize
	memset(szEventCodeBuffer, 0, sizeof(szEventCodeBuffer));	

	// ...
	do
	{		
		strcpy(szEventCodeBuffer, szDataToReceive);		
		nDataLen = strlen(szEventCodeBuffer);	

		if(0 == nDataLen)												// no event code
		{
			//_LOG("Data length is zero. (%d)", __LINE__);
			break;
		}
		else if( (0 < nDataLen) && (((int)strlen("C00000001\r") > nDataLen) || (int)strlen("P00000001\r") > nDataLen) ) // no event code
		{
			break;
		}
		else if(MAX_BUF <= nDataLen)											// has event, but make terminal string
		{
			//_LOG("Data length is big. Data length : '%d'. (%d)", nDataLen, __LINE__);
			*(szEventCodeBuffer + MAX_BUF - 1) = NULL;
		}		

		// parsing data(event data) and update event code
		char* szEventCode = NULL;
		szEventCode = strstr(szEventCodeBuffer, "C0");
		if( szEventCode != NULL )
		{
			UpdateCassetEvent(szEventCode);		
		}

		szEventCode = strstr(szEventCodeBuffer, "P0");
		if( szEventCode != NULL )
		{
			UpdatePurgeEvent(szEventCode);		
		}

	
	} while(0);

	return bRet;
}

//2013.04.26 ECODE Cmd Response -> Error Code by hyuk
BOOL DetectError(char *szDataToReceive)
{
	BOOL bRet = FALSE;

	do
	{
		char szErrorCodeBuffer[MAX_BUF];
		memset(szErrorCodeBuffer, 0, sizeof(szErrorCodeBuffer));
		strcpy(szErrorCodeBuffer, szDataToReceive);

		// check data size
		int nDataLen = strlen(szErrorCodeBuffer);
 
		if(0 == nDataLen)													// no error code
		{
			_LOG("Data length is zero. (%d)", __LINE__);
			break;
		}
		else if( (0 < nDataLen) && ((int)strlen("E9 ERR_\r") > nDataLen) )  // no error code. check minimun error code string length - (int)strlen("E9 ERR_\r")
		{
			break;
		}
		else if(MAX_BUF <= nDataLen)										// has error, but make terminal string
		{
			_LOG("Data length is big. Data length : '%d'. (%d)", nDataLen, __LINE__);
			*(szErrorCodeBuffer + MAX_BUF - 1) = NULL;
		}
		else if(!STRNCMP_L(szErrorCodeBuffer,"E",1))						// prohibit Posting Exception Case "Alarm Letter"  
		{
			break;
		}
		else																// has error
		{
			;
		}

		char* szErrorCode = NULL;
		szErrorCode = strstr(szErrorCodeBuffer, "E");						// find sting pointer (Exxx)

		if(NULL != szErrorCode)
		{			
			char* szTerminal = NULL;										// set '\n' to be null.	
			szTerminal = strstr(szErrorCode, "\n");
			if(NULL != szTerminal)
			{
				*szTerminal = NULL;
			}
			else
			{
				_LOG("Failed to find '\n' in sub string. szErrorCodeBuffer : '%s', (%d)", szErrorCodeBuffer, __LINE__);
				break;
			}
			
			g_nErrorCode = atoi(szErrorCode + 1);
			memset(g_szErrorCode, 0, sizeof(g_szErrorCode));
			strcpy(g_szErrorCode, szErrorCode);
			bRet = TRUE;
			break;
		}

	} while(0);

	return bRet;
}
//2013.04.26 ECODE Cmd Response -> Error Code by hyuk


BOOL Rs232_IO( char *Command , char *Response )
{
	BOOL bRetCode = FALSE;
	BOOL bInsideLoop = FALSE;
	int  nFoupSts = 0;
	enum {Unknown, Absent, Nozzle2, Nozzle4};
	
	WaitForSingleObject(g_hMutex, INFINITE);

	g_bBusy = TRUE;

	do
	{
		int nReadCount = 0;
		char szDataToSend[MAX_BUF];
		char szDataToReceive[MAX_BUF];
		char szReceiveBuffer[MAX_BUF];
		BOOL bIsN2Event = FALSE;
		char szN2Event[MAX_BUF] = {0,};
	
		memset(szDataToSend, 0, sizeof(szDataToSend));
		memset(szDataToReceive, 0, sizeof(szDataToReceive));
		memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));

		do
		{
			// check receive data and clear buffer

			//RS232_Clear_Buffer();			//<- never use this fn(RS232_Clear_Buffer) in Duraport driver.
											//   if use, get lost data(event or error code) from duraport


			CheckReceiveDataAndClearBuffer();

			sprintf(szDataToSend, "%s%s", Command, g_strSendTerm);

			/*
			if (0 == strcmp(Command, "CTYPE") && 0 <= atoi(&Response[1]))
			{
				printf("[debug] ==> cmd %s res %s\n",Command,Response);
				nFoupSts = atoi(&Response[1]);
				switch(nFoupSts)
				{
				case 0:	g_nFoupHoleSts = Absent;	break;
				case 2: g_nFoupHoleSts = Nozzle2;	break;
				case 4: g_nFoupHoleSts = Nozzle4;	break;
				default: break;
				}
			}
			*/
			if(strlen(g_strSendTerm) == strlen(szDataToSend))
			{
				_LOG("Command string is null. (%d)", __LINE__);
				//int i = 1;
			}

			if(g_nLogSts) LogProtocolSendData(szDataToSend);

			if( FALSE == RS232_Write_String(szDataToSend, strlen(szDataToSend)) )
			{
//				_LOG("Send Error - Send Data : '%s'. (%d)", szDataToSend, __LINE__);
				break;
			}

			//Sleep(200);

			// receive data third times.

			do
			{
				/*
				// get 1st response data
				if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 1000, &nReadCount) )
				{
					break;
				}

				strcat(szDataToReceive, szReceiveBuffer);
				//strcat(szDataToReceive, "\n");
				strcat(szDataToReceive, g_strRecvTerm);
				
				// get 2nd response data
				memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));
				
				if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 5000, &nReadCount) )
				{
					break;
				}

				strcat(szDataToReceive, szReceiveBuffer);
				//strcat(szDataToReceive, "\n");
				strcat(szDataToReceive, g_strRecvTerm);
				*/


				// get 3rd response data
				int nRevMax = 7;   //... 2015.12.16 [100 -> 7]  
				int nRevCnt = 0;

				// if Open Case, wait for 100 secs while receiving message return
				if(0 == strcmp(Command, "OPEN"))
				{
					 nRevMax = 100;
				}
				else
				{
					 nRevMax = 7;	
				}

				do
				{
					// read com port data, if exit.
					memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));

					if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 1000, &nReadCount) )
					{
						// check whether com port buffer is empty

						if(0 != strlen(szReceiveBuffer))
						{
							strcat(szDataToReceive, szReceiveBuffer);
							//strcat(szDataToReceive, "\n");
							//strcat(szDataToReceive, g_strRecvTerm2);
							//Exception case : "P0" data receive [N2 Purge Use]
							if(STRNCMP_L(szReceiveBuffer,"P0",2))
							{
								bIsN2Event = TRUE;
								strcpy(szN2Event,szReceiveBuffer);
							}
							else
							{
								bIsN2Event = FALSE;
								*szN2Event = NULL;
							}
						}
						//break;
					}
					else
					{
						strcat(szDataToReceive, szReceiveBuffer);
						//strcat(szDataToReceive, "\n");
						strcat(szDataToReceive, g_strRecvTerm);

						//Exception case : "P0" data receive [N2 Purge Use]
						if(STRNCMP_L(szReceiveBuffer,"P0",2))
						{
							bIsN2Event = TRUE;
							strcpy(szN2Event,szReceiveBuffer);
						}
						else
						{
							bIsN2Event = FALSE;
							*szN2Event = NULL;
						}
					}


					// check whether all data is received
					char *strResponse = NULL;
					strResponse = strstr(szDataToReceive, g_strRecvTerm);	// check first 'terminal string'
					if(NULL != strResponse && bIsN2Event == FALSE)
					{
						char* str2ndResponse = NULL;
						str2ndResponse = strstr(strResponse + 1, g_strRecvTerm);	// check second 'terminal string'

						if(NULL != str2ndResponse)
						{
							break;
						}
					}
					else
					{
						; // continue to read
					}

					if(nRevCnt >= nRevMax)
					{
//						_LOG("It is over the allowable receive count limit. (%d)", __LINE__);
						break;
					}
					else
					{
						nRevCnt ++;
					}

				}while(1);

				//strcpy(Response, szDataToReceive);
				strncpy(Response, szDataToReceive, MAX_BUF - 1);

				// szDataToReceive = "A\n%d"
				if (0 == strcmp(Command, "CTYPE") && 0 <= atoi(&szDataToReceive[2]))
				{
					
					nFoupSts = atoi(&szDataToReceive[1]);
					//printf("[debug] ==> cmd %s %d \n",Command,nFoupSts);
					switch(nFoupSts)
					{
					case 0:	g_nFoupHoleSts = Absent;	break;
					case 2: g_nFoupHoleSts = Nozzle2;	break;
					case 4: g_nFoupHoleSts = Nozzle4;	break;
					default: break;
					}
				}

				//LogProtocolRcvData(szDataToReceive);
				if(g_nLogSts) LogProtocolRcvDataEx(szDataToReceive);

				// check error data in response data.
				
				/*if(TRUE == DetectError(szReceiveBuffer))
				{
					break;
				}*/

				//DetectError(szDataToReceive);	// if detect error, update error code.
				DetectError(szDataToReceive);
				if(bIsN2Event == FALSE)DetectEvent(szDataToReceive);
				else DetectEvent(szN2Event);
				//... 2015.12.16
				if(nRevCnt >= nRevMax)
				{	
					break;
				}
				//... 
				bInsideLoop = TRUE;

			}while(0);

			if(TRUE == bInsideLoop)
			{
				bRetCode = TRUE;
			}

		}while(0);	

		//LogProtocolData(szDataToSend, szDataToReceive);
		//LogProtocolRcvData(szDataToReceive);

		if(TRUE == bRetCode)
		{
			g_bCommStatus = TRUE;
			break;
		}
		else
		{
//			_LOG("Failed to send and receive from device. (%d)", __LINE__);
			g_bCommStatus = FALSE;
		}

	}while(0);

	g_bBusy = FALSE;

	ReleaseMutex(g_hMutex);

	return bRetCode;
}

BOOL Rs232_IO2( char *Command , char *Response )
{
	BOOL bRetCode = FALSE;
	BOOL bInsideLoop = FALSE;
	BOOL bIsN2Event = FALSE;

	g_bBusy = TRUE;

	do
	{
		int nReadCount = 0;
		char szDataToSend[MAX_BUF];
		char szDataToReceive[MAX_BUF];
		char szReceiveBuffer[MAX_BUF];
		char szTempBuffer[MAX_BUF];
		char szN2Event[MAX_BUF] = {0,};
	
		memset(szDataToSend, 0, sizeof(szDataToSend));
		memset(szDataToReceive, 0, sizeof(szDataToReceive));
		memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));
		memset(szTempBuffer, 0, sizeof(szTempBuffer));

		do
		{
			// check receive data and clear buffer

			//RS232_Clear_Buffer();			//<- never use this fn(RS232_Clear_Buffer) in Duraport driver.
											//   if use, get lost data(event or error code) from duraport


			CheckReceiveDataAndClearBuffer();

			sprintf(szDataToSend, "%s%s", Command, g_strSendTerm);

			if(strlen(g_strSendTerm) == strlen(szDataToSend))
			{
//				_LOG("Command string is null. (%d)", __LINE__);
				//int i = 1;
			}

			if(g_nLogSts) LogProtocolSendData(szDataToSend);

			if( FALSE == RS232_Write_String(szDataToSend, strlen(szDataToSend)) )
			{
//				_LOG("Send Error - Send Data : '%s'. (%d)", szDataToSend, __LINE__);
				break;
			}

			//Sleep(200);

			// receive data third times.

			do
			{
				/*
				// get 1st response data
				if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 1000, &nReadCount) )
				{
					break;
				}

				strcat(szDataToReceive, szReceiveBuffer);
				//strcat(szDataToReceive, "\n");
				strcat(szDataToReceive, g_strRecvTerm);
				
				// get 2nd response data
				memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));
				
				if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 5000, &nReadCount) )
				{
					break;
				}

				strcat(szDataToReceive, szReceiveBuffer);
				//strcat(szDataToReceive, "\n");
				strcat(szDataToReceive, g_strRecvTerm);
				*/


				// get 3rd response data
				int nRevMax = 100;
				int nRevCnt = 0;

				do
				{
					// read com port data, if exit.
					memset(szReceiveBuffer, 0, sizeof(szReceiveBuffer));

					if( FALSE == RS232_Read_String(szReceiveBuffer, g_strRecvTerm, MAX_BUF, 1000, &nReadCount) )
					{
						// check whether com port buffer is empty

						if(0 != strlen(szReceiveBuffer))
						{
							strcat(szDataToReceive, szReceiveBuffer);
							//strcat(szDataToReceive, "\n");
							//strcat(szDataToReceive, g_strRecvTerm2);
							if(STRNCMP_L(szReceiveBuffer,"P0",2))
							{
								bIsN2Event = TRUE;
								strcpy(szN2Event,szReceiveBuffer);
							}
							else
							{
								bIsN2Event = FALSE;
								*szN2Event = NULL;
							}
						}
						//break;
					}
					else
					{
						strcat(szDataToReceive, szReceiveBuffer);
						//strcat(szDataToReceive, "\n");
						strcat(szDataToReceive, g_strRecvTerm);

						if(STRNCMP_L(szReceiveBuffer,"P0",2))
						{
							bIsN2Event = TRUE;
							strcpy(szN2Event,szReceiveBuffer);
						}
						else
						{
							bIsN2Event = FALSE;
							*szN2Event = NULL;
						}
					}


					// check whether all data is received
					char *strResponse = NULL;
					strResponse = strstr(szDataToReceive, g_strRecvTerm);	// check first 'terminal string'
					if(NULL != strResponse && bIsN2Event == FALSE)   //... 2018.06.15 Recive값이 Mapping Data일때 만 Check,   
					{                                                        
						char* str2ndResponse = NULL;
						str2ndResponse = strstr(strResponse + 1, g_strRecvTerm);	// check second 'terminal string'
							
						if(NULL != str2ndResponse)
						{
							break;
						}
					}
					else
					{
						; // continue to read
					}

					if(nRevCnt >= nRevMax)
					{
//						_LOG("It is over the allowable receive count limit. (%d)", __LINE__);
						break;
					}
					else
					{
						nRevCnt ++;
					}

				}while(1);

				//strcpy(Response, szDataToReceive);
				strncpy(Response, szDataToReceive, MAX_BUF - 1);

				//LogProtocolRcvData(szDataToReceive);
				if(g_nLogSts) LogProtocolRcvDataEx(szDataToReceive);

				// check error data in response data.
				
				/*if(TRUE == DetectError(szReceiveBuffer))
				{
					break;
				}*/

				//DetectError(szDataToReceive);	// if detect error, update error code.
				DetectError(szDataToReceive);
				if(bIsN2Event == FALSE)DetectEvent(szDataToReceive);
				else DetectEvent(szN2Event);
				bInsideLoop = TRUE;

			}while(0);

			if(TRUE == bInsideLoop)
			{
				bRetCode = TRUE;
			}

		}while(0);	

		//LogProtocolData(szDataToSend, szDataToReceive);
		//LogProtocolRcvData(szDataToReceive);

		if(TRUE == bRetCode)
		{
			g_bCommStatus = TRUE;
			break;
		}
		else
		{
//			_LOG("Failed to send and receive from device. (%d)", __LINE__);
			g_bCommStatus = FALSE;
		}

	}while(0);

	g_bBusy = FALSE;

	return bRetCode;
}

double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) 
{
	*Result = TRUE;
	if( ID1 == 10 )
	{
		switch(ID2) {		
		case 1: return g_N2Status.N2Flow1;			
		case 2: return g_N2Status.N2Flow2;			
		case 3: return g_N2Status.N2MainPrs;			
		case 4: return g_N2Status.N2ExhaustPrs;			
		case 5: return g_N2Status.OxygenDensity;
		default:
			*Result = FALSE;
			break;
		}
	}

	return 0;
}

void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) 
{
	*Result = TRUE;
}

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
		{
			switch(ID2)
			{
			case 1 :
				{
					// Motor Type Modify : PaddleExtRetXI  2008.09.18 
					/*
					if(1 == g_bStatusResponse[7])
					{
						g_nPaddleExtRetXI = 1; // 0 unknown, 1 extend, 2 retract, 3 error
					}
					else if(1 == g_bStatusResponse[8])
					{
						g_nPaddleExtRetXI = 2;
					}
					else
					{
						g_nPaddleExtRetXI = 0;
					}
					*/

					if(1 == g_bStatusResponse[2])
					{
						g_nPaddleExtRetXI = 1; // 0 unknown, 1 extend, 2 retract, 3 error
					}
					else if(1 == g_bStatusResponse[3])
					{
						g_nPaddleExtRetXI = 2;
					}
					else
					{
						g_nPaddleExtRetXI = 0;
					}


					nRet = g_nPaddleExtRetXI;
					
					*Result = TRUE;
					break;
				}
			case 2 :
				{
					// Motor Type Modify : LatchOpenCloseXI 2008.09.18 
					/*
					if(1 == g_bStatusResponse[14])
					{
						g_nLatchOpenCloseXI = 2; // 0 Unknown, 1 Open, 2 Close, 3 Error	
					}
					else if(1 == g_bStatusResponse[15])
					{
						g_nLatchOpenCloseXI = 1;
					}
					else
					{
						g_nLatchOpenCloseXI = 0;
					}
					*/

					if(1 == g_bStatusResponse[3])
					{
						g_nLatchOpenCloseXI = 2; // 0 Unknown, 1 Open, 2 Close, 3 Error	
					}
					else if(1 == g_bStatusResponse[2])
					{
						g_nLatchOpenCloseXI = 1;
					}
					else
					{
						g_nLatchOpenCloseXI = 0;
					}

					nRet = g_nLatchOpenCloseXI;
					
					
					*Result = TRUE;
					break;
				}
			case 3 :
				{
					// Motor Type Modify : DoorUpDownXI 2008.09.18 
					/*
					if(1 == g_bStatusResponse[7])
					{
						g_nDoorUpDownXI = 2;
					}
					else if(1 == g_bStatusResponse[8])
					{
						g_nDoorUpDownXI = 1;
					}
					else
					{
						g_nDoorUpDownXI = 0;
					}
					*/
					if(1 == g_bStatusResponse[2])
					{
						g_nDoorUpDownXI = 2;	//0: Unknown	1: Up	2: Down	  3: Error
					}
					else if(1 == g_bStatusResponse[3])
					{
						g_nDoorUpDownXI = 1;
					}
					else
					{
						g_nDoorUpDownXI = 0;
					}

					nRet = g_nDoorUpDownXI;		// 0 Unknown, 1 up, 2 down ,3 Error
					
					//... 2016.03.03
					if(g_bCommStatus == TRUE)
					{
						*Result = TRUE;
					}
					else
					{
						*Result = FALSE;	
					}
					//...
					break;
				}
			case 4 :
				{
					// Motor Type Modify : VacuumOKD  Deleted 2008.09.18 
					/*
					if(0 == g_bStatusResponse[13])
					{
						g_nVacuumOKDI = 0;
					}
					else if(1 == g_bStatusResponse[13])
					{
						g_nVacuumOKDI = 1;
					}
					else
					{
						g_nVacuumOKDI = 0;
					}
					*/
					nRet = g_nVacuumOKDI;
					
					
					*Result = TRUE;
					break;
				}
			case 5 :
				{
					g_nWaferSlideOutDI = 1;	// 0 on, 1 off
					nRet = g_nWaferSlideOutDI;
					*Result = TRUE;
					break;
				}
			// Motor Driver Status DI Added 2008.09.08
			case 6 :
				{
					if(0 == g_bStatusResponse[1])
					{
						g_nMotorStatusDI = 0;
					}
					else if(1 == g_bStatusResponse[1])
					{
						g_nMotorStatusDI = 1;
					}
					else
					{
						g_nMotorStatusDI = 0;
					}
					nRet = g_nMotorStatusDI;		// Motor  0 off, 1 on
					*Result = TRUE;
					break;
				}
			default :
				{
//					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}
		
	case 2 :
		{
			switch(ID2)
			{
			case 1 :
				{
					if(1 == g_bStatusResponse[11])
					{
						g_nShuttleInOutXI = 1; // 0 Unknown, 1 In, 2 Out, 3 Error
					}
					else if(1 == g_bStatusResponse[12])
					{
						g_nShuttleInOutXI = 2;
					}
					else
					{
						g_nShuttleInOutXI = 0;
					}

					nRet = g_nShuttleInOutXI;
					//... 2016.03.03
					if(g_bCommStatus == TRUE)
					{
						*Result = TRUE;
					}
					else
					{
						*Result = FALSE;	
					}
					//...
					break;
				}
			case 2 :
				{
					if(0 == g_bStatusResponse[28])
					{
						g_nF_ProperlyPlaceDI = 0;	// 0 Off, 1 On
					}
					else if(1 == g_bStatusResponse[28])
					{
						g_nF_ProperlyPlaceDI = 1;
					}
					else
					{
						g_nF_ProperlyPlaceDI = 0;
					}

					nRet = g_nF_ProperlyPlaceDI;
					//... 2015.12.16
					if(g_bCommStatus == TRUE)
					{
						*Result = TRUE;
					}
					else
					{
						*Result = FALSE;	
					}
					//...
					break;
				}
			case 3 :
				{
					nRet = g_nInterlockActiveDI;	// 0 Off, 1 On
					*Result = TRUE;
					break;
				}
			case 4 :
				{
					// duraport clamp/unclamp firmware bug fixed. - 2007.05.28 ijbae.

					if(1 == g_bStatusResponse[9])
					{
						g_nClampUnclampXI = 1; // 0 Unknown, 1 Clamp, 2 Unclamp, 3 Error
					}
					else if(1 == g_bStatusResponse[10])
					{
						g_nClampUnclampXI = 2;
					}
					else
					{
						g_nClampUnclampXI = 0;
					}

					nRet = g_nClampUnclampXI;
					//... 2015.12.16
					if(g_bCommStatus == TRUE)
					{
						*Result = TRUE;
					}
					else
					{
						*Result = FALSE;	
					}
					//...
					break;
				}
			case 5 :
				{
					if(0 == g_bStatusResponse[29])
					{
						g_nFOUP_PresentDI = 0;	// 0 Off, 1 On
					}
					else if(1 == g_bStatusResponse[29])
					{
						g_nFOUP_PresentDI = 1;
					}
					else
					{
						g_nFOUP_PresentDI = 0;
					}

					nRet = g_nFOUP_PresentDI;
					//... 2015.12.16
					if(g_bCommStatus == TRUE)
					{
						*Result = TRUE;
					}
					else
					{
						*Result = FALSE;	
					}
					//...
					break;
				}
			default :
				{
//					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}

	case 3 :
		{
			switch(ID2)
			{
			case 1 :
				{
					nRet = g_nSwitch1UnloadDI;
					*Result = TRUE;
					break;
				}
			case 2 :
				{
					nRet = g_nSwitch2LoadDI;
					*Result = TRUE;
					break;
				}
			default :
				{
//					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}

	case 4 :
		{
			switch(ID2)
			{
			case 1 :
				{
					nRet = g_bBusy;
					*Result = TRUE;
					break;
				}
			case 2 :
				{
					nRet = g_nMappingStatus;	// 0 None, 1 Mapping, 2 MapComplete, 3 MapFail
					//... 2015.12.16
					if(g_bCommStatus == TRUE)
					{
						*Result = TRUE;
					}
					else
					{
						*Result = FALSE;	
					}
					//...
					break;
				}
			case 3 :
				{
					nRet = g_nErrorCode;
					*Result = TRUE;
					break;
				}
			case 4 :
				{
					// delete get error digital input.
					// only use error code (analog data)

					/*if(0 == g_bStatusResponse[16])
					{
						g_nErrorDI = 1;
					}
					else if(1 == g_bStatusResponse[16])
					{
						g_nErrorDI = 0;
					}
					else
					{
						g_nErrorDI = 0;
					}*/

					nRet = g_nErrorDI;
					*Result = TRUE;
					break;
				}
////////////////////////////////////////////////////////////////////////////////////
			case 5 :
				{
					nRet = g_bStatusResponse[31];
					*Result = TRUE;
					break;
				}
////////////////////////////////////////////////////////////////////////////////////////
			default :
				{
//					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}

	case 5 :
		{
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
				{
					nRet = g_nLampArray[ID2 - 1];	// 0 off, 1 on, 2 blink
					*Result = TRUE;
					break;
				}
			default :
				{
//					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}

	case 10:
		{
			switch(ID2)
			{
			case 1:
				nRet = g_N2Status.time;
				*Result = TRUE;
				break;
			}
			break;
		}

	case 11:	// CTYPE
		{
			switch(ID2)
			{
			case 1:
				nRet = g_nFoupHoleSts;
				*Result = TRUE;
				break;
			}
			break;
		}

	case 20:	// Cassette Event Message ("C")
		{
			switch(ID2)
			{
			case 1:	nRet = LP_EVT.TP_CONNECT;			*Result = TRUE;	break;
			case 2: nRet = LP_EVT.TP_DISCONNECT;		*Result = TRUE;	break;
			case 3: nRet = LP_EVT.OP_ACCESS_BTN_PUSH;	*Result = TRUE;	break;
			case 4: nRet = LP_EVT.FOUP_IN_STS;			*Result = TRUE;	break;
			case 5: nRet = LP_EVT.FOUP_OUT_STS;			*Result = TRUE;	break;
			case 6: nRet = LP_EVT.RESET_START;			*Result = TRUE;	break;
			case 7: nRet = LP_EVT.RESET_END;			*Result = TRUE;	break;
			case 8: nRet = LP_EVT.FOUP_START_READY;		*Result = TRUE;	break;
			case 9: nRet = LP_EVT.FOUP_INCORRECT_POS;	*Result = TRUE;	break;
			case 10: nRet = LP_EVT.EMG_STOP_PUSH;		*Result = TRUE;	break;
			case 11: nRet = LP_EVT.N2_FLOW_ALARM;		*Result = TRUE;	break;
			case 12: nRet = LP_EVT.N2_PRES_ALARM;		*Result = TRUE;	break;
			case 13: nRet = LP_EVT.N2_COMM_ALARM;		*Result = TRUE;	break;
			case 14: nRet = LP_EVT.N2_UNCLAMP_ALARM;	*Result = TRUE;	break;
			case 15: nRet = LP_EVT.O2SENSOR_FAIL;		*Result = TRUE;	break;
			case 16: nRet = LP_EVT.FFU_SIGNAL_FAIL;		*Result = TRUE;	break;
			case 17: nRet = LP_EVT.O2SENSOR_WARNING;	*Result = TRUE;	break;

			}
		}

	case 30:	// Purge Event Message ("P")
		{
			switch(ID2)
			{
			case 1: nRet = N2_STS.PRE_START;		*Result = TRUE;	break;
			case 2: nRet = N2_STS.PRE_END;			*Result = TRUE;	break;
			case 3: nRet = N2_STS.PRE_STOP;			*Result = TRUE;	break;
			case 4: nRet = N2_STS.PRE_WARNING;		*Result = TRUE;	break;
			case 5: nRet = N2_STS.PROC_START;		*Result = TRUE;	break;
			case 6: nRet = N2_STS.PROC_END;			*Result = TRUE;	break;
			case 7: nRet = N2_STS.PROC_STOP;		*Result = TRUE;	break;
			case 8: nRet = N2_STS.PROC_WARNING;		*Result = TRUE;	break;
			case 9: nRet = N2_STS.POST_START;		*Result = TRUE;	break;
			case 10: nRet = N2_STS.POST_END;		*Result = TRUE;	break;
			case 11: nRet = N2_STS.POST_STOP;		*Result = TRUE;	break;
			case 12: nRet = N2_STS.POST_WARNING;	*Result = TRUE;	break;
			case 13: nRet = N2_STS.TPRE_START;		*Result = TRUE;	break;
			case 14: nRet = N2_STS.TPRE_END;		*Result = TRUE;	break;
			case 15: nRet = N2_STS.TPRE_STOP;		*Result = TRUE;	break;
			case 16: nRet = N2_STS.TPRE_WARNING;	*Result = TRUE;	break;
			case 17: nRet = N2_STS.PPOST_START;		*Result = TRUE;	break;
			case 18: nRet = N2_STS.PPOST_END;		*Result = TRUE;	break;
			case 19: nRet = N2_STS.PPOST_STOP;		*Result = TRUE;	break;
			case 20: nRet = N2_STS.PPOST_WARNING;	*Result = TRUE;	break;
			case 21: nRet = N2_STS.BLOW_START;		*Result = TRUE;	break;
			case 22: nRet = N2_STS.BLOW_END;		*Result = TRUE;	break;
			case 23: nRet = N2_STS.BLOW_STOP;		*Result = TRUE;	break;
			case 24: nRet = N2_STS.BLOW_WARNING;	*Result = TRUE;	break;
			case 25: nRet = N2_STS.PURGE_START;		*Result = TRUE;	break;
			case 26: nRet = N2_STS.PURGE_END;		*Result = TRUE;	break;
			case 27: nRet = N2_STS.PURGE_STOP;		*Result = TRUE;	break;
			case 28: nRet = N2_STS.PURGE_WARNING;	*Result = TRUE;	break;

			}
		}

	//2013.04.26 Error Code 
	case 99 :
		{
			sprintf(szDataToSend, "ECODE");
			
			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
			{
				_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
				break;
			}

			if(FALSE == DetectError(szDataToReceive))
			{
				_LOG("Failed to DetectError : '%s'. (%d)", szDataToReceive, __LINE__);
				break;
			}

			*Result = TRUE;
			break;
		}	
	default :
		{
//			_LOG("Failed to find match case. ID1 : '%d', (%d)", ID1, __LINE__);
			break;
		}
	}
	 
	return nRet;
}

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
	case 1 :
		{
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
				{
					*Result = TRUE;
					break;
				}
			default :
				{
					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}
			
			break;
		}

	case 2 :
		{
			switch(ID2)
			{
			case 1 :
				{
					if(1 == Data)
					{
						if (TRUE == g_bThreadRun) {
							*Result = TRUE;
							break;
						}

						printf("===@~~ map Driver Start!! \n");
						g_nMappingStatus = 1;	// 0 None, 1 Mapping, 2 MapComplete, 3 MapFail
						CreateThread(NULL, 0, AutoMapRetry, NULL, 0, &dwTemp);						
					}
					else
					{
						g_nMappingStatus = 0;
					}

					*Result = TRUE;
					break;
				}
			case 2 :
				{
					sprintf(szDataToSend, "GETMAP");
					
					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					if(FALSE == ParseReceiveMapData(szDataToReceive))
					{
						_LOG("Failed to parse receive map data : '%s'. (%d)", szDataToReceive, __LINE__);
						break;
					}

					*Result = TRUE;
					break;
				}
			default :
				{
					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}
	case 3 :
		{
			switch(ID2)
			{
			case 1 :
				{
					if(1 == Data)
					{
						//g_nClampUnclampXI  = 1;	// temporary 2006.12.17
						// Command Change 2008.08.25
						// sprintf(szDataToSend, "CLAMP");
						sprintf(szDataToSend, "POD_LOCK ON");
					}
					else if(2 == Data)
					{
						//g_nClampUnclampXI  = 2;	// temporary 2006.12.17
						// Command Change 2008.08.25
						// sprintf(szDataToSend, "UNCLAMP");
						sprintf(szDataToSend, "POD_LOCK OFF");
					}
					else
					{
						_LOG("Failed to find match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}
					
					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}

					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}

			case 2 :
				{
					//g_nClampUnclampXI = 1;	// temporary 2006.12.17

					if(FULLDOCK == Data)
					{
						g_nMappingStatus = 1;
						// Command Change 2008.08.25
						// sprintf(szDataToSend, "LOAD");
						sprintf(szDataToSend, "OPEN");
					}
					else if(SHUTTLEIN == Data)
					{
						sprintf(szDataToSend, "DOCK");
					}
					else if(CLAMP == Data)
					{
						// Command Change 2008.08.25
						// sprintf(szDataToSend, "CLAMP");
						sprintf(szDataToSend, "POD_LOCK ON");
					}
					else if(FULLDOCKMAP == Data)
					{
						// Command Change 2008.08.25
						// sprintf(szDataToSend, "LOAD");
						sprintf(szDataToSend, "OPEN");
					}
					else
					{
						_LOG("Failed to find match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					//int nTimeOut = 20000;
					//if(FALSE == Rs232_IOEx(szDataToSend, szDataToReceive, nTimeOut))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					if((FULLDOCK == Data) || (FULLDOCKMAP == Data))
					{
						if(TRUE == ParseReceiveMapData(szDataToReceive))
						{
							g_nMappingStatus = 2;
						}
						else
						{
							g_nMappingStatus = 3;
						}
					}

					*Result = TRUE;
					break;
				}

			case 3 :
				{
					//g_nClampUnclampXI = 2;	// temporary 2006.12.17

					if(FULLUNDOCK == Data)
					{
						// Command Change 2008.08.25
						// sprintf(szDataToSend, "UNLOAD");
						sprintf(szDataToSend, "CLOSE");
					}
					else if(CLOSEDOOR == Data)
					{
						sprintf(szDataToSend, "DR_CLOSE");
					}
					else if(SHUTTLEOUT == Data)
					{
						sprintf(szDataToSend, "UNDOCK");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					//int nTimeOut = 20000;
					//if(FALSE == Rs232_IOEx(szDataToSend, szDataToReceive, nTimeOut))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}

			default :
				{
					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}
	case 4 :
		{
			switch(ID2)
			{
			case 1 :
				{
					/*if(1 == Data)
					{
						sprintf(szDataToSend, "HOM");
					}
					else if(2 == Data)
					{
						sprintf(szDataToSend, "HOM");
					}
					else
					{
						break;
					}*/

					sprintf(szDataToSend, "HOM");

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					//int nTimeOut = 20000;
					//if(FALSE == Rs232_IOEx(szDataToSend, szDataToReceive, nTimeOut))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			case 2 :
				{
					if(Log_Off == Data)	g_nLogSts = Log_Off;
					else g_nLogSts = Log_On;
					*Result = TRUE;
					break;
				}

			case 3 :
				{
					g_nErrorCode = Data;
					*Result = TRUE;
					break;
				}
			case 4 :
				{
					sprintf(szDataToSend, "RESET");

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					// Initialize Information
					memset(&LP_EVT, 0, sizeof(LP_E_STS));
					memset(&N2_STS, 0, sizeof(N2LP_STATUS));

					// reset error code
					g_nErrorCode = 0;
					memset(g_szErrorCode, 0, sizeof(g_szErrorCode));

					*Result = TRUE;
					break;
				}
			case 5 :
				{
					if(0 == Data)
					{
						sprintf(szDataToSend, "AUTO_MODE OFF");
					}
					else if(1 == Data)
					{
						sprintf(szDataToSend, "AUTO_MODE ON");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			case 6 :
				{
					if(0 == Data)
					{
						sprintf(szDataToSend, "MAINT_MODE OFF");
					}
					else if(1 == Data)
					{
						sprintf(szDataToSend, "MAINT_MODE ON");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			case 7 :
				{
					*Result = TRUE;
					break;
				}
			case 8 :
				{
					g_nMappingStatus = Data;
					*Result = TRUE;
					break;
				}
			// Motor Driver On/Off IO Addition 2008.08.25
			case 9 :
				{
					if(0 == Data)
					{
						sprintf(szDataToSend, "AMPOFF");
					}
					else if(1 == Data)
					{
						sprintf(szDataToSend, "AMPON");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			default :
				{
					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}

			break;
		}
	case 5 :
		{
			if( 0 == Data ) //Unknown
			{
				*Result = TRUE;
			}
			else if(Data == 1) //Close
			{
				sprintf( szDataToSend, "TCLOSE");			
			}
			else if(Data == 2) // Open
			{
				sprintf( szDataToSend, "TOPEN");			
			}
			else {*Result = FALSE; return;}

			if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
			{
				_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
				break;
			}
			_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

			*Result = TRUE;
			break;
		}
// INDICATOR LED
	case 6 :
		{
			switch(ID2)
			{
			case 1 :
				{
					if(0 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_LOAD OFF");
					}
					else if(1 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_LOAD ON");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			case 2 :
				{
					if(0 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_UNLOAD OFF");
					}
					else if(1 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_UNLOAD ON");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			case 3 :
				{
					if(0 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_RESERVE OFF");
					}
					else if(1 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_RESERVE ON");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			case 4 :
				{
					if(0 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_AUTO OFF");
					}
					else if(1 == Data)
					{
						sprintf(szDataToSend, "INDICATOR_AUTO ON");
					}
					else
					{
						_LOG("Failed to match data. Data : '%d'. (%d)", Data, __LINE__);
						break;
					}

					if(FALSE == Rs232_IO(szDataToSend, szDataToReceive))
					{
						_LOG("Failed to send data : '%s'. (%d)", szDataToSend, __LINE__);
						break;
					}
					_LOG("Send data : '%s'. (%d)", szDataToSend, __LINE__);  //Added by jhlee. 2010 10 17

					*Result = TRUE;
					break;
				}
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
			case 10 :
			case 11 :
				{
					*Result = TRUE;
					break;
				}
			default :
				{
					_LOG("Failed to find match case. ID1 : '%d', ID2 : '%d', (%d)", ID1, ID2, __LINE__);
					break;
				}
			}
			
			break;
		}

	default :
		{
			_LOG("Failed to find match case. ID1 : '%d', (%d)", ID1, __LINE__);
			break;
		}
	}
}

void String_In( int ID1 , char *data , int *Result ) 
{
	*Result = FALSE;

	switch(ID1)
	{
	case 1 :
		{
			char szWaferInfo[MAX_BUF];
			char szTotalWaferInfo[MAX_BUF];

			memset(szWaferInfo, 0, sizeof(szWaferInfo));
			memset(szTotalWaferInfo, 0, sizeof(szTotalWaferInfo));

			for(int nSlot = 0; nSlot < MAX_SLOT; nSlot ++)
			{
				sprintf(szWaferInfo, "%1.0d", g_nEQC_Wafer_Info[nSlot]);
				strcat(szTotalWaferInfo, szWaferInfo);
			}

			if(99 > strlen(szTotalWaferInfo))
			{
				strcpy(data, szTotalWaferInfo);
			}
			else
			{
				strncpy(data, szTotalWaferInfo, 99); // only copy 99 char
				strcpy(data + 100, "\0");
			}

			*Result = TRUE;
			break;
		}
	case 2 :
	case 3 :
	case 4 :
		{
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
		}
	case 5 : // Cassette Event Message "C"
		{
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
		}
	case 6:	// Purge Event Message "P"
		{
			if(99 > strlen(g_szPurgeEventCode))
			{
				strcpy(data, g_szPurgeEventCode);
			}
			else
			{
				strncpy(data, g_szPurgeEventCode, 99);
				strcpy(data + 100, "\0");
			}

			*Result = TRUE;
			break;
		}
	case 99:	// custom response
		strncpy(data, g_szCustomResponse, 99);
		*Result = TRUE;
		break;

	default :
		{
//			_LOG("Failed to find match case. ID1 : '%d', (%d)", ID1, __LINE__);
			break;
		}
	}
}

void String_Out( int ID1 , char *data , int *Result ) 
{
	switch(ID1)
	{
	case 99:	// custom request
		strcpy(g_szCustomRequest, data);

		Rs232_IO(g_szCustomRequest, g_szCustomResponse);
		break;

	default:
		break;
	}
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

BOOL LogProtocolData(char *szSendData, char *szReceiveData)
{
	BOOL bRet = FALSE;
	do
	{
		// check send, receive data

		if(NULL == szSendData)
		{
//			_LOG("Failed to get SendData. (%d)", __LINE__);
			break;
		}

		if(NULL == szReceiveData)
		{
//			_LOG("Failed to get receive data. (%d)", __LINE__);
			break;
		}


		// init buffer data

		char szTempSendData[MAX_BUF] = "";
		char szTempReceiveData[MAX_BUF] = "";
		memset(szTempSendData, 0, sizeof(szTempSendData));
		memset(szTempReceiveData, 0, sizeof(szTempReceiveData));

		int nSendLength = 0;
		int nReceiveLength = 0;

		nSendLength = strlen(szSendData);
		nReceiveLength = strlen(szReceiveData);


		// set data to buffer

		/*for(int i = 0; i < MAX_BUF; i++)
		{
			szTempSendData[i] = (unsigned long)*(szSendData++);
			szTempReceiveData[i] = (unsigned long)*(szReceiveData++);
		}*/

		strcpy(szTempSendData, szSendData);
		strcpy(szTempReceiveData, szReceiveData);


		// init protocol buffer

		int nArrayCnt = 0;
		char szSendDataProtocol[MAX_BUF * 3 + 50] = "";
		char szReceiveDataProtocol[MAX_BUF * 3 + 50] = "";

		memset(szSendDataProtocol, 0, sizeof(szSendDataProtocol));
		memset(szReceiveDataProtocol, 0, sizeof(szReceiveDataProtocol));

		int nSendDataIndex = 0;
		int nReceiveDataIndex = 0;


		// make hex data and log

		//for(nArrayCnt = 0; nArrayCnt < MAX_BUF; nArrayCnt)
		for(nArrayCnt = 0; nArrayCnt < nSendLength; nArrayCnt)
		{
			BYTE byteTemp = szTempSendData[nArrayCnt++];
			nSendDataIndex += sprintf(szSendDataProtocol + nSendDataIndex, " %0.2x", byteTemp );
		}

		/*char szProtocolLog[MAX_BUF * 4] = "";
		memset(szProtocolLog, 0, sizeof(szProtocol));
		sprintf(szProtocolLog, "SendData \t- %s", szSendDataProtocol);
		LOG_ERROR(szProtocolLog, MEDIUM);*/

//		_LOG("SendData \t- %s", szSendDataProtocol);
//		_LOG("         \t- %s", szSendData);
		
		//for(nArrayCnt = 0; nArrayCnt < MAX_BUF; nArrayCnt)
		for(nArrayCnt = 0; nArrayCnt < nReceiveLength; nArrayCnt)
		{
			BYTE byteTemp = szTempReceiveData[nArrayCnt++];
			nReceiveDataIndex += sprintf(szReceiveDataProtocol + nReceiveDataIndex, " %0.2x", byteTemp);
		}

		if(0 == nReceiveLength)
		{
			sprintf(szReceiveDataProtocol, " (No response data)");
		}

		/*memset(szProtocolLog, 0, sizeof(szProtocolLog));
		sprintf(szProtocolLog, "ReceiveData \t- %s", szReceiveDataProtocol);*/
		//LOG_ERROR(szProtocolLog, MEDIUM);

		//_LOG("ReceiveData \t- %s", szReceiveDataProtocol);
		//_LOG("            \t- %s", szReceiveData);

		if( 200 < strlen(szReceiveDataProtocol) )
		{
			char szSliceReceiveData[MAX_BUF];
			memset(szSliceReceiveData, 0, sizeof(szSliceReceiveData));

			int nSlice = 0;
			int nTotalSlice = strlen(szSliceReceiveData) / 200;

			for(nSlice = 0; nSlice < nTotalSlice; nSlice ++)
			{
				memset(szSliceReceiveData, 0, sizeof(szSliceReceiveData));
				strncpy(szSliceReceiveData, szReceiveDataProtocol + 200 * nSlice, 200);	// copy to szSliceReceiveData as 200 bytes

				if(0 == nSlice)
				{
//					_LOG("ReceiveData \t- %s", szSliceReceiveData);
				}
				else
				{
//					_LOG("            \t- %s", szSliceReceiveData);
				}
			}

			strcpy(szSliceReceiveData, szReceiveDataProtocol + 200 * nTotalSlice);	// copy 'remain data of szReceiveDataProtocol' to szSliceReceiveData

			if(0 == nTotalSlice)
			{
//				_LOG("ReceiveData \t- %s", szSliceReceiveData);
			}
			else
			{
//				_LOG("            \t- %s", szSliceReceiveData);
			}
		}

		bRet = TRUE;
	}while(0);

	return bRet;
}

BOOL LogProtocolSendData(char *szSendData)
{
	BOOL bRet = FALSE;
	do
	{
		// check send, receive data

		if(NULL == szSendData)
		{
			_LOG("Failed to get SendData. (%d)", __LINE__);
			break;
		}


		// init buffer data

		char szTempSendData[MAX_BUF] = "";
		memset(szTempSendData, 0, sizeof(szTempSendData));

		int nSendLength = 0;

		nSendLength = strlen(szSendData);


		// set data to buffer

		/*for(int i = 0; i < MAX_BUF; i++)
		{
			szTempSendData[i] = (unsigned long)*(szSendData++);
			szTempReceiveData[i] = (unsigned long)*(szReceiveData++);
		}*/

		strcpy(szTempSendData, szSendData);


		// init protocol buffer

		int nArrayCnt = 0;
		char szSendDataProtocol[MAX_BUF * 3 + 50] = "";

		memset(szSendDataProtocol, 0, sizeof(szSendDataProtocol));

		int nSendDataIndex = 0;


		// make hex data and log

		//for(nArrayCnt = 0; nArrayCnt < MAX_BUF; nArrayCnt)
		for(nArrayCnt = 0; nArrayCnt < nSendLength; nArrayCnt ++)
		{
			BYTE byteTemp = szTempSendData[nArrayCnt];
			nSendDataIndex += sprintf(szSendDataProtocol + nSendDataIndex, "%0.2x ", byteTemp );
		}

		/*char szProtocolLog[MAX_BUF * 4] = "";
		memset(szProtocolLog, 0, sizeof(szProtocolLog));
		sprintf(szProtocolLog, "SendData \t- %s", szSendDataProtocol);
		LOG_ERROR(szProtocolLog, MEDIUM);*/

//		_LOG("SendData \t- %s", szSendDataProtocol);
//		_LOG("         \t- %s", szSendData);
		_LOG("SendData \t- %s", szSendData);	// temp log. 2007.04.26
												// "_LOG" is not enough to get 'szSendDataProtocol' string.
	
		bRet = TRUE;
	}while(0);

	return bRet;
}

BOOL LogProtocolRcvData(char *szReceiveData)
{
	BOOL bRet = FALSE;
	do
	{
		// check send, receive data

		if(NULL == szReceiveData)
		{
//			_LOG("Failed to get receive data. (%d)", __LINE__);
			break;
		}


		// init buffer data

		char szTempReceiveData[MAX_BUF] = "";
		memset(szTempReceiveData, 0, sizeof(szTempReceiveData));

		int nReceiveLength = 0;

		nReceiveLength = strlen(szReceiveData);


		// set data to buffer

		/*for(int i = 0; i < MAX_BUF; i++)
		{
			szTempSendData[i] = (unsigned long)*(szSendData++);
			szTempReceiveData[i] = (unsigned long)*(szReceiveData++);
		}*/

		strcpy(szTempReceiveData, szReceiveData);


		// init protocol buffer

		int nArrayCnt = 0;
		char szReceiveDataProtocol[MAX_BUF * 3 + 50] = "";

		memset(szReceiveDataProtocol, 0, sizeof(szReceiveDataProtocol));

		int nReceiveDataIndex = 0;


		// make hex data and log

		//for(nArrayCnt = 0; nArrayCnt < MAX_BUF; nArrayCnt)
		for(nArrayCnt = 0; nArrayCnt < nReceiveLength; nArrayCnt ++)
		{
			BYTE byteTemp = szTempReceiveData[nArrayCnt];
			nReceiveDataIndex += sprintf(szReceiveDataProtocol + nReceiveDataIndex, "%0.2x ", byteTemp);
		}

		if(0 == nReceiveLength)
		{
			sprintf(szReceiveDataProtocol, " (No response data)");
		}

		/*memset(szProtocolLog, 0, sizeof(szProtocolLog));
		sprintf(szProtocolLog, "ReceiveData \t- %s", szReceiveDataProtocol);*/
		//LOG_ERROR(szProtocolLog, MEDIUM);

		//_LOG("ReceiveData \t- %s", szReceiveDataProtocol);
		//_LOG("            \t- %s", szReceiveData);

		int nSliceSize = 100;

		if( nSliceSize <= (int) strlen(szReceiveDataProtocol) )
		{
			char szSliceReceiveData[MAX_BUF];
			memset(szSliceReceiveData, 0, sizeof(szSliceReceiveData));

			int nSlice = 0;
			//int nTotalSlice = strlen(szSliceReceiveData) / 200;
			int nTotalSlice = strlen(szReceiveDataProtocol) / nSliceSize;

			for(nSlice = 0; nSlice < nTotalSlice; nSlice ++)
			{
				memset(szSliceReceiveData, 0, sizeof(szSliceReceiveData));
				strncpy(szSliceReceiveData, szReceiveDataProtocol + nSliceSize * nSlice, nSliceSize);	// copy to szSliceReceiveData as 200 bytes
				strncpy(szSliceReceiveData + nSliceSize * ( nSlice + 1 ), "\0", 1);

				if(0 == nSlice)
				{
					//_LOG("ReceiveData \t- %s", szSliceReceiveData);
				}
				else
				{
					//_LOG("            \t- %s", szSliceReceiveData);
				}
			}

			strcpy(szSliceReceiveData, szReceiveDataProtocol + nSliceSize * nTotalSlice);	// copy 'remain data of szReceiveDataProtocol' to szSliceReceiveData

			if(0 == nTotalSlice)
			{
				//_LOG("ReceiveData \t- %s", szSliceReceiveData);
			}
			else
			{
				//_LOG("            \t- %s", szSliceReceiveData);
			}

			//_LOG("            \t- %s", szReceiveData);
//			_LOG("ReceiveData \t- %s", szReceiveData);

		}
		else
		{
			//_LOG("ReceiveData \t- %s", szReceiveDataProtocol);
			//_LOG("            \t- %s", szReceiveData);
//			_LOG("ReceiveData \t- %s", szReceiveData);
		}

		bRet = TRUE;
	}while(0);

	return bRet;
}

BOOL LogProtocolRcvDataEx(char* szReceiveData)
{
	BOOL bRet = FALSE;
	do
	{
		// check send, receive data

		if(NULL == szReceiveData)
		{
			_LOG("Failed to get receive data. (%d)", __LINE__);
			break;
		}


		// init buffer data

		char szTempReceiveData[MAX_BUF] = "";
		memset(szTempReceiveData, 0, sizeof(szTempReceiveData));

		int nReceiveLength = 0;

		nReceiveLength = strlen(szReceiveData);
		if(0 == nReceiveLength)
		{
			_LOG("Receive Data length is zero. (%d)", __LINE__);
			break;
		}


		// set data to buffer

		/*for(int i = 0; i < MAX_BUF; i++)
		{
			szTempSendData[i] = (unsigned long)*(szSendData++);
			szTempReceiveData[i] = (unsigned long)*(szReceiveData++);
		}*/

		strcpy(szTempReceiveData, szReceiveData);


		// init protocol buffer

		int nArrayCnt = 0;
		char szReceiveDataProtocol[MAX_BUF * 3 + 50] = "";

		memset(szReceiveDataProtocol, 0, sizeof(szReceiveDataProtocol));

		int nReceiveDataIndex = 0;


		// make hex data and log

		//for(nArrayCnt = 0; nArrayCnt < MAX_BUF; nArrayCnt)
		for(nArrayCnt = 0; nArrayCnt < nReceiveLength; nArrayCnt ++)
		{
			BYTE byteTemp = szTempReceiveData[nArrayCnt];
			nReceiveDataIndex += sprintf(szReceiveDataProtocol + nReceiveDataIndex, "%0.2x ", byteTemp);
		}

		if(0 == nReceiveLength)
		{
			sprintf(szReceiveDataProtocol, " (No response data)");
		}

		/*memset(szProtocolLog, 0, sizeof(szProtocolLog));
		sprintf(szProtocolLog, "ReceiveData \t- %s", szReceiveDataProtocol);*/
		//LOG_ERROR(szProtocolLog, MEDIUM);

		_LOG("ReceiveData \t- %s", szReceiveDataProtocol);
		_LOG("            \t- %s", szReceiveData);

		int nSliceSize = 100;

		//if( nSliceSize <= (int) strlen(szReceiveDataProtocol) )
		if( nSliceSize <= (int) strlen(szTempReceiveData) )
		{
			char szSliceReceiveData[MAX_BUF];
			memset(szSliceReceiveData, 0, sizeof(szSliceReceiveData));

			int nSlice = 0;
			//int nTotalSlice = strlen(szSliceReceiveData) / 200;
			int nTotalSlice = strlen(szTempReceiveData) / nSliceSize;

			for(nSlice = 0; nSlice < nTotalSlice; nSlice ++)
			{
				memset(szSliceReceiveData, 0, sizeof(szSliceReceiveData));
				strncpy(szSliceReceiveData, szTempReceiveData + nSliceSize * nSlice, nSliceSize);	// copy to szSliceReceiveData as 200 bytes
				strncpy(szSliceReceiveData + nSliceSize * ( nSlice + 1 ), "\0", 1);

				if(0 == nSlice)
				{
					_LOG("ReceiveData \t- %s", szSliceReceiveData);
				}
				else
				{
					_LOG("            \t- %s", szSliceReceiveData);
				}
			}

			strcpy(szSliceReceiveData, szTempReceiveData + nSliceSize * nTotalSlice);	// copy 'remain data of szReceiveDataProtocol' to szSliceReceiveData

			if(0 == nTotalSlice)
			{
				_LOG("ReceiveData \t- %s", szSliceReceiveData);
			}
			else
			{
				_LOG("            \t- %s", szSliceReceiveData);
			}

		}
		else
		{
			_LOG("ReceiveData \t- %s", szTempReceiveData);
		}

		bRet = TRUE;
	}while(0);

	return bRet;
}

int GetBitStatus(char source, int pos)
{
	short int res;
	short int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128} , temp;
	char src1;
	
	if     ( source == '0' ) src1=0;
	else if( source == '1' ) src1=1;
	else if( source == '2' ) src1=2;
	else if( source == '3' ) src1=3;
	else if( source == '4' ) src1=4;
	else if( source == '5' ) src1=5;
	else if( source == '6' ) src1=6;
	else if( source == '7' ) src1=7;
	else if( source == '8' ) src1=8;
	else if( source == '9' ) src1=9;
	else if( source == 'A' || source == 'a' ) src1=10;
	else if( source == 'B' || source == 'b' ) src1=11;
	else if( source == 'C' || source == 'c' ) src1=12;
	else if( source == 'D' || source == 'd' ) src1=13;
	else if( source == 'E' || source == 'e' ) src1=14;
	else if( source == 'F' || source == 'f' ) src1=15;
	
	temp = src1 & mask[pos];
	temp = temp >> pos;
	res = temp;
	
	return res;
}