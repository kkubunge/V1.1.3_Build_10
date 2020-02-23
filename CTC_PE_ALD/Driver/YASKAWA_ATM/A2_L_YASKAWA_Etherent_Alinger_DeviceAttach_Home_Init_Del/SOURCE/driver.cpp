extern "C"
{
#include "Winsock2.h"
#include "CimWsock.h"
#include "stdio.h"

#include "driver.h"
#include "kutletc.h"
#include "kutlanal.h"
#include "kutlstr.h"

#include <string.h>
}

//-----------------------------------------------------------------------------------
#include "TextParser.h"
#include "TextLogDll_M.h"

#define CR			    0x0D
#define BUFFER_SIZE     256
char	SendTermStr[6];
char	RecvTermStr[6];
int     TimeOut    = 100;
int		g_nTimeOut = 1000;

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
enum   {HAND_A = 0 , HAND_B = 1 , HAND_AB = 2};
enum   {PICK   = 0 , PLACE  = 1};
enum   {UNKNOWN = 0 , ABSENT , PRESENT};
enum   {NORMAL = 0 , ALARM = 1};
enum   {NONE   = 0 , WARNING1 = 1 , WARNING2 = 2 , ALARM1 = 3 , ALARM2 = 4 , FATAL = 5};
enum   {ROBOT_IDLE  = 0 , ROBOT_BUSY = 1, ROBOT_FAULT = 2, ROBOT_INVALID_DATA = 3 , ROBOT_DISCONNECTED = 4};
enum   {SINGLE_TYPE = 0 , TWIN_TYPE  = 1};

//-----------------------------------------------------------------------------------
#define IO_DATA_FILE_NAME	".\\Parameter\\EQ_SETTING\\YASKAWA_Ethernet.dat"
#define TIMEOUT_T	1000
int		g_nPortNum = 10111;		//... Aligner Fixed
int		g_nTimeout = TIMEOUT_T;
char	g_pDevice_Address[24]= "10.0.0.2";

//-----------------------------------------------------------------------------------
BOOL CommStatus	          = TRUE;
int  g_nRobotStatus       = ROBOT_IDLE;

// System Optional
int  g_nTransferType      = TWIN_TYPE; 
//-----------------------------------------------------------------------------------

int	 INDEX_SLOT	         = 1;       //Current target slot
int  g_nRobotUnit        = 1;       //Fixed Robot Unit Number
int  g_nAlignerUnit      = 2;       //Fixed Aligner Unit Number
int  g_nErrorLevel       = NONE;    //Error Level
int  g_nErrorCode        = 0;       //Detail Error Code
//2012.05.20 by mgsong
//System Error Code IO
int  g_nSysErrorCode     = 0;

// Digital System Status
int  g_nErrorStatus      = NORMAL;
int  g_nBattery1Status   = NORMAL;
int  g_nBattery2Status   = NORMAL;
int  g_nServoStatus      = NORMAL;
// Digital System Status

int	 g_nCurrentArm       = HAND_A;
int  g_nWaferA           = UNKNOWN;
int  g_nWaferB           = UNKNOWN;
double g_dbAngle         = 0.0f;
double g_dbEccdistnace   = 0.0f;

char g_szArmName[32]     = {0};     //Current Blade name   (A, B, Dual)
char g_szStationName[32] = {0};     //Current Station name (C1, C2, C3, C4, C5... P1(Aligner))
int  g_nSlotNum          = 0;       //ReAllocated Slot Number ==> Real Slot Number

unsigned char g_szHexTable[] = {'0', '1', '2', '3',
                               	'4', '5', '6', '7',
								'8', '9', 'A', 'B',
								'C', 'D', 'E', 'F' }; 

//////////////////////////////////////////////////////////////////////////
BOOL Net_IO_Send(char *Cmnds ) {
	
	printf("[DEBUG]YASKAWA Send[%s]\n", Cmnds);

	strcat(Cmnds, SendTermStr);
	if(! Net_Write_String(Cmnds, strlen(Cmnds)))		return FALSE;
	else												return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
char* GetStationName(int nStation , int nSlot , int& nSlotNew)
{
	//AKRA Base Station Number (Use LPM3)
	//CM1 = 1, CM2 = 2, CM3 = 3, BM1 = 6, BM2 = 7, Aligner = 10

	//Bluetain Base Station Number
	//CM1 = 1, CM2 = 2, BM1 Slot 1,3 = 6, BM1 Slot 2,4 = 7, BM2 Slot 1,3 = 8, BM2 Slot 2,4 = 9, Aligner = 10

	if (nStation <= 0)              return NULL;	

	memset(g_szStationName , 0 , sizeof(g_szStationName));

	//Default New Slot will be same to Coming Slot
	nSlotNew = nSlot;

	switch (nStation) {
	case 1: //CM1
		strcpy(g_szStationName , "C1");
		break;

	case 2: //CM2
		strcpy(g_szStationName , "C2");
		break;

	case 3: //CM3
		strcpy(g_szStationName , "C3");
		break;

	case 4: //Buffer In Direction
		strcpy(g_szStationName , "C4");
		break;

	case 5: //Buffer Out Direction
		strcpy(g_szStationName , "C5");
		break;

	//////////////////////////////////////////////////////////////////////////
	case 6: //LLA Slot 1, 2 for Single Type : LLA Slot 1, 3 for Twin Type
		if(SINGLE_TYPE == g_nTransferType)
		{
			strcpy(g_szStationName , "C6");
		}
		else if (TWIN_TYPE == g_nTransferType) 
		{
			if (1 == nSlot || 3 == nSlot) 
			{
				strcpy(g_szStationName , "C7");

				//LL Left Upper's Real Slot is 2
				if      (1 == nSlot) nSlotNew = 1;
				else if (3 == nSlot) nSlotNew = 2;
			}
			else 
			{
				strcpy(g_szStationName , "C6");

				//LL Right Lower's Real Slot is 1
				//LL Right Upper's Real Slot is 2
				if      (2 == nSlot) nSlotNew = 1;
				else if (4 == nSlot) nSlotNew = 2;
			}

		}
		else return NULL;

		break;

	//////////////////////////////////////////////////////////////////////////
	case 7: //LLB Slot 1, 2 for Single Type : LLA Slot 2, 4 for Twin Type
		if      (SINGLE_TYPE == g_nTransferType)	strcpy(g_szStationName , "C7");
		else if (TWIN_TYPE == g_nTransferType) 
		{
			if (1 == nSlot || 3 == nSlot) {
				strcpy(g_szStationName , "C9");

				//LL Left Upper's Real Slot is 2
				if      (1 == nSlot) nSlotNew = 1;
				else if (3 == nSlot) nSlotNew = 2;
			}
			else {
				strcpy(g_szStationName , "C8");

				//LL Right Lower's Real Slot is 1
				//LL Right Upper's Real Slot is 2
				if      (2 == nSlot) nSlotNew = 1;
				else if (4 == nSlot) nSlotNew = 2;
			}
		}
		else return NULL;
		break;

	case 8: //LLB Slot 1, 3 for Twin Type
		return NULL;
		break;

	case 9: //LLB Slot 2, 4 for Twin Type
		return NULL;
		break;

	case 10: //Aligner
		strcpy(g_szStationName , "P1");
		break;

	case 11: //Inner Storage1 (Dummy)
		strcpy(g_szStationName , "CA");
		break;

	case 12: //Inner Storage2
		strcpy(g_szStationName , "CB");
		break;

	default:
		return NULL; break;
	}

	return g_szStationName;
}

//////////////////////////////////////////////////////////////////////////
char* GetArmName(int nArm , int nAct)
{
	//nSelected Arm A: 0 , B: 1 , Dual : 2
	//nAct      Pick : 0 , Place : 1
	if (nArm >= 3 || nArm < 0) return NULL;

	memset (g_szArmName , 0 , sizeof(g_szArmName));
	switch (nArm) {
	case 0: //Arm A
		if (nAct == PICK)
			strcpy(g_szArmName , "GA");
		else
			strcpy(g_szArmName , "PA");

		g_nCurrentArm = HAND_A;
		break;
	case 1: //Arm B
		if (nAct == PICK)
			strcpy(g_szArmName , "GB");
		else
			strcpy(g_szArmName , "PB");

		g_nCurrentArm = HAND_B;
		break;
	case 2: //Dual Arm
		if (nAct == PICK)
			strcpy(g_szArmName , "GW");
		else
			strcpy(g_szArmName , "PW");

		g_nCurrentArm = HAND_AB;
		break;
	default:
		return NULL; break;
	}

	return g_szArmName;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void AttachCheckSum(char *szSendData , int nLength)
{
	int  i    = 0;
	char cSum = 0;
	unsigned char ucUpperBit = 0;
	unsigned char ucLowerBit = 0;
	
	//Calculate Check Sum		
	for (i = 1 ; i < nLength ; i++) cSum += szSendData[i];
	cSum = cSum & 0xff;

	//Attach Check Sum Data as Hex String
	ucLowerBit = cSum & 0x0f;
	ucUpperBit = (cSum >> 4) & 0x0f;
	szSendData[nLength]   = g_szHexTable[ucUpperBit];
	szSendData[nLength+1] = g_szHexTable[ucLowerBit];
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void UpdateSystemStatus(char cSts1 , char cSts2)
{
	int nSts1 = 0;
	int nSts2 = 0;

	if (cSts1 < 'A') nSts1 = cSts1 - '0';
	else             nSts1 = cSts1 - '7';

	if (cSts2 < 'A') nSts2 = cSts2 - '0';
	else             nSts2 = cSts2 - '7';
	
	if (nSts1 & 0x01) g_nBattery1Status = ALARM;
	else              g_nBattery1Status = NORMAL;

	if (nSts1 & 0x04) g_nServoStatus    = ALARM;
	else              g_nServoStatus    = NORMAL;

	if (nSts1 & 0x08) g_nErrorStatus    = ALARM;
	else              g_nErrorStatus    = NORMAL;

	if (nSts2 & 0x01) g_nBattery2Status = ALARM;
	else              g_nBattery2Status = NORMAL;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void UpdateErrorLevel(char cLevel)
{
	if      ('0' == cLevel) g_nErrorLevel = NONE;
	else if ('1' == cLevel) g_nErrorLevel = WARNING1;
	else if ('2' == cLevel) g_nErrorLevel = WARNING2;
	else if ('3' == cLevel) g_nErrorLevel = ALARM1;
	else if ('4' == cLevel) g_nErrorLevel = ALARM2;
	else if ('5' == cLevel) g_nErrorLevel = FATAL;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL ParseErrorResponse(char *szReceiveData)
{
	BOOL bRet            = FALSE;
	char szErrorCode[10] = {0};
	int  i               = 0;
	char cSum            = 0;
	unsigned char ucUpperBit = 0;
	unsigned char ucLowerBit = 0;
	int  nLength = strlen(szReceiveData);
	
		
	do {
		//Format
		//[?][ERR LEVEL][E2][E1][E0][CSU][CSL]


	//--> DEL 2011/04/28  NotUse CheckSum  by KSM
	/*
		///////////////////////////////////////////////////////////////
		//1. Check Sum 
		///////////////////////////////////////////////////////////////
		for (i = 1; i < nLength-2 ; i++)
			cSum += szReceiveData[i];

		cSum       = cSum & 0xff;
		ucLowerBit = cSum & 0x0f;
		ucUpperBit = (cSum >> 4) & 0x0f;
		if (g_szHexTable[ucUpperBit] != szReceiveData[nLength-2]) {
			g_nRobotStatus = ROBOT_INVALID_DATA; break;
		}

		if (g_szHexTable[ucLowerBit] != szReceiveData[nLength-1]) {
			g_nRobotStatus = ROBOT_INVALID_DATA; break;
		}
	*/
	//--> END 2011/04/28  NotUse CheckSum  by KSM

		///////////////////////////////////////////////////////////////
		//2. Error Level Update
		///////////////////////////////////////////////////////////////
		UpdateErrorLevel(szReceiveData[1]);
		
		///////////////////////////////////////////////////////////////
		//3. Error Code Update
		///////////////////////////////////////////////////////////////
		strncpy(szErrorCode , szReceiveData+2 , 3);
		if (NULL != szErrorCode) g_nErrorCode = strtol(szErrorCode , NULL , 16);

		if (NONE != g_nErrorLevel || 0 != g_nErrorCode) {
			g_nRobotStatus = ROBOT_FAULT;
		}		

		bRet = TRUE;
	} while (0);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL ParseAckResponse(char *szReceiveData)
{
	BOOL bRet            = FALSE;
	char szErrorCode[10] = {0};
	int  i               = 0;
	char cSum            = 0;
	unsigned char ucUpperBit = 0;
	unsigned char ucLowerBit = 0;
	int  nLength = strlen(szReceiveData);
		
	do {
		//Format
		//[$][UnitNum][STS1][STS2][ERR LEVEL][E2][E1][E0][CSU][CSL]

	//--> DEL 2011/04/28  NotUse CheckSum  by KSM
	/*
		///////////////////////////////////////////////////////////////
		//1. Check Sum (Optional)
		///////////////////////////////////////////////////////////////
		for (i = 1; i < nLength-2 ; i++)
			cSum += szReceiveData[i];

		cSum       = cSum & 0xff;
		ucLowerBit = cSum & 0x0f;
		ucUpperBit = (cSum >> 4) & 0x0f;

		if (g_szHexTable[ucUpperBit] != szReceiveData[nLength-2]) {
			g_nRobotStatus = ROBOT_INVALID_DATA; break;
		}

		if (g_szHexTable[ucLowerBit] != szReceiveData[nLength-1]) {
			g_nRobotStatus = ROBOT_INVALID_DATA; break;
		}
	*/
	//--> DEL 2011/04/28  NotUse CheckSum  by KSM

		///////////////////////////////////////////////////////////////
		//2. System Status Update
		///////////////////////////////////////////////////////////////
		UpdateSystemStatus(szReceiveData[2] , szReceiveData[3]);		

		///////////////////////////////////////////////////////////////
		//3. Error Level Update
		///////////////////////////////////////////////////////////////
		UpdateErrorLevel(szReceiveData[4]);
		
		///////////////////////////////////////////////////////////////
		//4. Error Code Update
		///////////////////////////////////////////////////////////////
		strncpy(szErrorCode , szReceiveData+5 , 3);
		if (NULL != szErrorCode) g_nErrorCode = strtol(szErrorCode , NULL , 16);

		if (NONE != g_nErrorLevel || 0 != g_nErrorCode) {
			g_nRobotStatus = ROBOT_FAULT;
		}	

		bRet = TRUE;
	} while (0);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL ParseCompleteResponse(char *szReceiveData , char *szCmd)
{
	BOOL bRet            = FALSE;
	char szErrorCode[10] = {0};
	int  i               = 0;
	char cSum            = 0;
	unsigned char ucUpperBit = 0;
	unsigned char ucLowerBit = 0;
	int  nLength = strlen(szReceiveData);
		
	do {
		//Format
		//[$][UnitNum][STS1][STS2][ERR LEVEL][E2][E1][E0][Command, strlen(szCmd)][CSU][CSL]

	//--> DEL 2011/04/28  NotUse CheckSum  by KSM
	/*
		///////////////////////////////////////////////////////////////
		//1. Check Sum (Optional)
		///////////////////////////////////////////////////////////////
		for (i = 1; i < nLength-2 ; i++)
			cSum += szReceiveData[i];

		cSum       = cSum & 0xff;
		ucLowerBit = cSum & 0x0f;
		ucUpperBit = (cSum >> 4) & 0x0f;

		if (g_szHexTable[ucUpperBit] != szReceiveData[nLength-2]) {
			g_nRobotStatus = ROBOT_INVALID_DATA; break;
		}

		if (g_szHexTable[ucLowerBit] != szReceiveData[nLength-1]) {
			g_nRobotStatus = ROBOT_INVALID_DATA; break;
		}
	*/
	//--> END 2011/04/28  NotUse CheckSum  by KSM

		///////////////////////////////////////////////////////////////
		//2. System Status Update
		///////////////////////////////////////////////////////////////
		UpdateSystemStatus(szReceiveData[2] , szReceiveData[3]);

		///////////////////////////////////////////////////////////////
		//3. Error Level Update
		///////////////////////////////////////////////////////////////
		UpdateErrorLevel(szReceiveData[4]);

		///////////////////////////////////////////////////////////////
		//4. Error Code Update
		///////////////////////////////////////////////////////////////
		strncpy(szErrorCode , szReceiveData+5 , 3);
		if (NULL != szErrorCode) g_nErrorCode = strtol(szErrorCode , NULL , 16);

		if (NONE != g_nErrorLevel || 0 != g_nErrorCode) {
			g_nRobotStatus = ROBOT_FAULT;
		}	

		bRet = TRUE;
	} while (0);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL ParseAlignResult(char *szReceiveData)
{
	BOOL bRet              = FALSE;
	char szErrorCode[10]   = {0};
	char szEccDistance[32] = {0};
	int  i                 = 0;
	char cSum              = 0;
	unsigned char ucUpperBit = 0;
	unsigned char ucLowerBit = 0;
	int  nLength = strlen(szReceiveData);
		
	do {
		//Format
		//[$][UnitNum][STS1][STS2][ERR LEVEL][E2][E1][E0][Value1 6byte][Value2 6byte][Value3 6byte][CSU][CSL]

		///////////////////////////////////////////////////////////////
		//2. System Status Update
		///////////////////////////////////////////////////////////////
		UpdateSystemStatus(szReceiveData[2] , szReceiveData[3]);

		///////////////////////////////////////////////////////////////
		//3. Error Level Update
		///////////////////////////////////////////////////////////////
		UpdateErrorLevel(szReceiveData[4]);

		///////////////////////////////////////////////////////////////
		//4. Error Code Update
		///////////////////////////////////////////////////////////////
		strncpy(szErrorCode , szReceiveData+5 , 3);
		if (NULL != szErrorCode) g_nErrorCode = strtol(szErrorCode , NULL , 16);

		if (NONE != g_nErrorLevel || 0 != g_nErrorCode) {
			//Negative Response don't go further
			g_nRobotStatus = ROBOT_FAULT; return TRUE;
		}	
		else {
			strncpy(szEccDistance , szReceiveData+8 , 6);
			if (NULL != szEccDistance) g_dbEccdistnace = atof(szEccDistance);
		}

		bRet = TRUE;
	} while (0);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL ParseRSTSStatus(char *szReceiveData)
{
	BOOL bRet            = FALSE;
	char szErrorCode[10] = {0};
	int  i               = 0;
	char cSum            = 0;
	unsigned char ucUpperBit = 0;
	unsigned char ucLowerBit = 0;
	int  nLength = strlen(szReceiveData);
	

	//---> ADD 2011/06/01  Wafer Status  by  KSM      
	char szArmWaferStatus[2] = {0};    
	//---> END 2011/06/01  Wafer Status  by  KSM
	//  0000 (0) : 모두 존재             1010 (A) : A(Lower) arm Present
	//  0101 (5) : B(Upper) arm Present  1111 (F) : 모두 없음.

	// Example)
       // [SEND] Send msg OK [$1RSTS]
       // robot response msg : [$12000000000A2F0] 
	//                                   #

	do {		
		///////////////////////////////////////////////////////////////
		//Update Current System Error Code
		///////////////////////////////////////////////////////////////
		strncpy(szErrorCode , szReceiveData+9 , 3);
		if (NULL != szErrorCode) g_nSysErrorCode = strtol(szErrorCode , NULL , 16);	

		///////////////////////////////////////////////////////////////
		//Update Current Wafer Status
		///////////////////////////////////////////////////////////////	
		strncpy(szArmWaferStatus , szReceiveData+12 , 1);
		//[ 2011/10/10 Wafer Status만 Check하고, Solenoid Status는 Check하지 않도록 수정.
		// Grip Type에서는 Wafer Status가 Absent이어도 Solenoid Status는 Hold일 수 있음.
		if (NULL != szArmWaferStatus[0]) 
		{
			if (   0 == strcmp(szArmWaferStatus , "F") 
				|| 0 == strcmp(szArmWaferStatus , "7")
				|| 0 == strcmp(szArmWaferStatus , "B")
				|| 0 == strcmp(szArmWaferStatus , "3")) 
			{
				g_nWaferA = 1;  //absent
				g_nWaferB = 1;  //absent		
			}
			else if ( 0 == strcmp(szArmWaferStatus , "5") 
				   || 0 == strcmp(szArmWaferStatus , "D") 
				   || 0 == strcmp(szArmWaferStatus , "9")
				   || 0 == strcmp(szArmWaferStatus , "1")) 
			{
				g_nWaferA = 1;  //absent
				g_nWaferB = 2;  //present				
			}
			else if ( 0 == strcmp(szArmWaferStatus , "A") 
				   || 0 == strcmp(szArmWaferStatus , "E")
				   || 0 == strcmp(szArmWaferStatus , "6") 
				   || 0 == strcmp(szArmWaferStatus , "2")) 
			{
				g_nWaferA = 2;  //present
				g_nWaferB = 1;  //absent				
			}
			else if ( 0 == strcmp(szArmWaferStatus , "0")
				   || 0 == strcmp(szArmWaferStatus , "C")
				   || 0 == strcmp(szArmWaferStatus , "4")
				   || 0 == strcmp(szArmWaferStatus , "8")) 
			{
				g_nWaferA = 2;  //present
				g_nWaferB = 2;  //present
			}
			else
			{
				g_nWaferA = 0;  //unknown
				g_nWaferB = 0;  //unknown					
			}
		}
		//---> END 2011/06/01  Wafer Status  by  KSM

		bRet = TRUE;
	} while (0);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL CheckACK()
{
	char  szResponse [BUFFER_SIZE] = {0};
	char  szRcvBuffer[BUFFER_SIZE] = {0};
	char  szTok[]     = "\r ";
	char* szTemp      = NULL;
	int   ReadCnt;
	int   nElapseTime = 0;	
	int   nSize       = 0;
	BOOL  bRet        = FALSE;

	_LOG("Started 'CheckACK'. (%d)", __LINE__);
	
	do {
		bRet = Net_Read_String(szRcvBuffer , RecvTermStr , 255 , TimeOut , &ReadCnt);
		if (FALSE == bRet)
		{
			if (0 == strlen(szRcvBuffer)) {
				printf("Receive Null string in CheckACK %s \n" , szRcvBuffer);
				if (nElapseTime > g_nTimeOut) {
					//Can Not Received Terminal String During Time Out  
					printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
					_LOG("CheckACK reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}	

				memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
				nElapseTime++; continue;

			} else {
				//2008.09.01 added for prevent driver crash
				nSize =  strlen(szResponse);
				nSize += strlen(szRcvBuffer);
				if (nSize >= BUFFER_SIZE-1) {
					_LOG("Driver got buffer overflow in CheckACK function. (%d) [%s] + [%s]", __LINE__ , szResponse , szRcvBuffer);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}
				//2008.09.01 End

				strcat(szResponse , szRcvBuffer);
			}
		} 
		else 
		{
			//2008.09.01 added for prevent driver crash
			nSize =  strlen(szResponse);
			nSize += strlen(szRcvBuffer);
			if (nSize >= BUFFER_SIZE-1) {
				_LOG("Driver got buffer overflow in CheckACK function. (%d) [%s] + [%s]", __LINE__ , szResponse , szRcvBuffer);
				bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
			}
			//2008.09.01 End

			strcat(szResponse , szRcvBuffer);
		}

		//Check current buffered string
		szTemp = strtok(szResponse , szTok);
		if (NULL == szTemp) {
            _LOG("Driver got Null pointer after strtok in CheckACK function. (%d)", __LINE__);
			if (nElapseTime > g_nTimeOut) {
				//Can Not Received Terminal String During Time Out
				printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
				_LOG("CheckACK reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
				bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
			}	

			memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
			nElapseTime++;		
		} else {
			//buffered string contain terminal character so start parse
			if      ('?' == szResponse[0]) {
				bRet = ParseErrorResponse(szResponse);
				break;			
			}
			else if ('$' == szResponse[0]) {
				bRet = ParseAckResponse(szResponse);
				break;				
			}		
			else {			
				if (nElapseTime > g_nTimeOut) {
					//Can Not Received Terminal String During Time Out
					printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
					_LOG("CheckACK reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}	

				memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
				nElapseTime++;				
			}
		}
	} while(1);
	

	if (NULL != szResponse)
		_LOG("Finished 'CheckACK' : %s. (%d)", szResponse,__LINE__);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL CheckCompleteResponse(char *szCmnds)
{
	char  szResponse [BUFFER_SIZE] = {0,};
	char  szRcvBuffer[BUFFER_SIZE] = {0,};
	char  szTok[]     = "\r ";
	char* szTemp      = NULL;
	int   ReadCnt;
	int   nElapseTime = 0;
	int   nSize       = 0;	
	BOOL  bRet        = FALSE;

	_LOG("Started 'CheckCompleteResponse'. (%d)",  __LINE__);	

	do {
		bRet = Net_Read_String(szRcvBuffer, RecvTermStr, 255, TimeOut, &ReadCnt);
		if(FALSE == bRet) 
		{	//Time out or Communication fail			
			if (0 == strlen(szRcvBuffer)) {
				printf("Receive Null string in CheckCompleteResponse %s \n" , szRcvBuffer);
				if (nElapseTime > g_nTimeOut) {
					//Can Not Received Terminal String During Time Out
					printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
					_LOG("CheckCompleteResponse reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}
				
				memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
				nElapseTime++; continue;	
				
			} else {
				//2008.09.01 added for check driver crash
				nSize =  strlen(szResponse);
				nSize += strlen(szRcvBuffer);
				if (nSize >= BUFFER_SIZE-1) {
					_LOG("Driver got buffer overflow in CheckCompleteResponse function. (%d) [%s] + [%s]", __LINE__ , szResponse , szRcvBuffer);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}
				//2008.09.01 End

				strcat(szResponse , szRcvBuffer);
			}
		} 
		else 
		{
			//2008.09.01 added for check driver crash
			nSize =  strlen(szResponse);
			nSize += strlen(szRcvBuffer);
			if (nSize >= BUFFER_SIZE-1) {
				_LOG("Driver got buffer overflow in CheckCompleteResponse function. (%d) [%s] + [%s]", __LINE__ , szResponse , szRcvBuffer);
				bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
			}
			//2008.09.01 End

			strcat(szResponse, szRcvBuffer);
			printf("[DEBUG]YASKAWA Recv[%s]\n", szResponse);
		}

		//Check current buffered string
		szTemp = strtok(szResponse , szTok);
		if (NULL == szTemp) 
		{
            _LOG("Driver got Null pointer after strtok in CheckCompleteResponse function. (%d)" , __LINE__);
			if (nElapseTime > g_nTimeOut) {
				//Can Not Received Terminal String During Time Out
				printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
				_LOG("CheckCompleteResponse reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
				bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
			}	

			memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
			nElapseTime++;				
		} 
		else 
		{
			//buffered string contain terminal character so start parse
			if      ('?' == szResponse[0]) {
				bRet = ParseErrorResponse(szResponse);
				break;				
			}
			else if ('!' == szResponse[0]) {
				bRet = ParseCompleteResponse(szResponse , szCmnds);
				break;				
			}	
			else {			
				if (nElapseTime > g_nTimeOut) {
					//Can Not Received Terminal String During Time Out
					printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
					_LOG("CheckCompleteResponse reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}
				
				memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
				nElapseTime++;			
			}
		}
	} while(1);
	
	if (NULL != szResponse)
		_LOG("Finished 'CheckCompleteResponse' : %s. (%d)", szResponse,__LINE__);
	
	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL CheckRQResponse(char *szCmnds)
{
	char  szResponse [BUFFER_SIZE] = {0};
	char  szRcvBuffer[BUFFER_SIZE] = {0};
	char  szTok[]     = "\r ";
	char* szTemp      = NULL;
	int   ReadCnt;
	int   nElapseTime = 0;
	int   nSize       = 0;	
	BOOL  bRet        = FALSE;

	_LOG("Started 'CheckRQResponse'. (%d)",  __LINE__);	

	do {
		bRet = Net_Read_String(szRcvBuffer , RecvTermStr , 255 , TimeOut , &ReadCnt);
		if (FALSE == bRet) { //Time out or Communication fail
			if (0 == strlen(szRcvBuffer)) {
				printf("Receive Null string in CheckRQResponse %s \n" , szRcvBuffer);
				if (nElapseTime > g_nTimeOut) {
					//Can Not Received Terminal String During Time Out
					printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
					_LOG("CheckRQResponse reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}
				
				memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
				nElapseTime++; continue;	
				
			} else {
				//2008.09.01 added for check driver crash
				nSize =  strlen(szResponse);
				nSize += strlen(szRcvBuffer);
				if (nSize >= BUFFER_SIZE-1) {
					_LOG("Driver got buffer overflow in CheckRQResponse function. (%d) [%s] + [%s]", __LINE__ , szResponse , szRcvBuffer);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}
				//2008.09.01 End

				strcat(szResponse , szRcvBuffer);
			}
		} else {
			//2008.09.01 added for check driver crash
			nSize =  strlen(szResponse);
			nSize += strlen(szRcvBuffer);
			if (nSize >= BUFFER_SIZE-1) {
				_LOG("Driver got buffer overflow in CheckRQResponse function. (%d) [%s] + [%s]", __LINE__ , szResponse , szRcvBuffer);
				bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
			}
			//2008.09.01 End

			strcat(szResponse , szRcvBuffer);
		}

		//Check current buffered string
		szTemp = strtok(szResponse , szTok);
		if (NULL == szTemp) {
            _LOG("Driver got Null pointer after strtok in CheckRQResponse function. (%d)" , __LINE__);
			if (nElapseTime > g_nTimeOut) {
				//Can Not Received Terminal String During Time Out
				printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
				_LOG("CheckRQResponse reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
				bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
			}	

			memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
			nElapseTime++;				
		} else {
			//buffered string contain terminal character so start parse
			if      ('?' == szResponse[0]) {
				bRet = ParseErrorResponse(szResponse);
				break;				
			}
			else if ('$' == szResponse[0]) {
				//Align Result Request
				if (0 == strcmp(szCmnds , "RALN")) 
					bRet = ParseAlignResult(szResponse);
				else if (0 == strcmp(szCmnds , "RSTS"))
					bRet = ParseRSTSStatus(szResponse);
				
				break;
			}	
			else {			
				if (nElapseTime > g_nTimeOut) {
					//Can Not Received Terminal String During Time Out
					printf("Timeout %d , current %d \n" , g_nTimeOut , nElapseTime);
					_LOG("CheckRQResponse reached timeout time. (%d) [%s]" , __LINE__ , szResponse);
					bRet = FALSE; g_nRobotStatus = ROBOT_DISCONNECTED; break;
				}
				
				memset(szRcvBuffer , 0 , sizeof(BUFFER_SIZE));
				nElapseTime++;			
			}
		}
	} while(1);
	
	if (NULL != szResponse)
		_LOG("Finished 'CheckRQResponse' : %s. (%d)", szResponse,__LINE__);
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Send_Motion_Command(char* SendData , char* Cmnds , int nUnitNum , BOOL bACK) 
{
	BOOL bRet = FALSE;		

	//---> ADD 2011/04/28   Send  ACK   by KSM
	char SBuffer[BUFFER_SIZE] = {0,};
	char szCmd  [32]          = {0,};  
	//---> END 2011/04/28   Send  ACK   by KSM

	g_nRobotStatus = ROBOT_BUSY;
	_LOG("Started 'Send_Motion_Command': '%s'. (%d)", SendData, __LINE__);

	do {
		if(Net_Connect_Status())
		{
			//////////////////////////////////////////////////////////////////
			//1. Buffer Clear
			//////////////////////////////////////////////////////////////////
			Net_Clear_Buffer();	
			
			//////////////////////////////////////////////////////////////////
			//2.Send Command
			//////////////////////////////////////////////////////////////////
			if(FALSE == Net_IO_Send(SendData))		break;			
			
			//////////////////////////////////////////////////////////////////
			//3. Check ACK , Optional 
			//////////////////////////////////////////////////////////////////
			if (TRUE == bACK) {
				if (FALSE == CheckACK())			break;
			}

			//////////////////////////////////////////////////////////////////
			//4. Check Complete Response
			//////////////////////////////////////////////////////////////////
			if (FALSE == CheckCompleteResponse(Cmnds)) break;

			//---> ADD 2011/04/28   Send  ACK   by KSM

			//////////////////////////////////////////////////////////////////
			//5. ACK Send
			//////////////////////////////////////////////////////////////////
			strcpy (szCmd   , "ACKN");
			sprintf(SBuffer , "$%d%s" , nUnitNum , szCmd);
			_LOG("Started 'Complete ACKN': '%s'. (%d)", SendData, __LINE__);
			if (FALSE ==  Net_IO_Send(SBuffer)) 	break;
		}
		else
		{
			printf("[ERR]YASKAWA Aligner Ethernet Connect Failed\n");
			CommStatus = FALSE;
			break;
		}
	
		_sleep(50);
	//---> END 2011/04/28   Send  ACK   by KSM

		bRet = TRUE;
	} while(0);

	if (ROBOT_BUSY == g_nRobotStatus) g_nRobotStatus = ROBOT_IDLE;

	_LOG("Finished 'Send_Motion_Command': '%s'. (%d)", Cmnds, __LINE__);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Send_RQ_Command(char* SendData , char* Cmnds) 
{
	BOOL bRet     = FALSE;
	_LOG("Started 'Send_RQ_Command': '%s'. (%d)", Cmnds, __LINE__);

	g_nRobotStatus = ROBOT_BUSY;

	do {
		if(Net_Connect_Status())
		{
			//////////////////////////////////////////////////////////////////
			//1. Buffer Clear
			//////////////////////////////////////////////////////////////////
			Net_Clear_Buffer();
			
			//////////////////////////////////////////////////////////////////
			//2.Send Command
			//////////////////////////////////////////////////////////////////
			bRet = Net_IO_Send(SendData); 
			if (FALSE == bRet) break;			
			
			//////////////////////////////////////////////////////////////////
			//3.Check Response
			//////////////////////////////////////////////////////////////////
			bRet = CheckRQResponse(Cmnds);
			if (FALSE == bRet) break;
		}
		else
		{
			printf("[ERR]YASKAWA Aligner Ethernet Connect Fail\n");
			CommStatus = FALSE;
			break;
		}
	} while(0);

	if (ROBOT_BUSY == g_nRobotStatus) g_nRobotStatus = ROBOT_IDLE;

	_LOG("Finished 'Send_RQ_Command': '%s'. (%d)", Cmnds, __LINE__);

	return bRet;
}

BOOL LoadIOData()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[40];
	char szIPAddress[24];

	CTextParser tp;

	do {
		fp = fopen(IO_DATA_FILE_NAME, "rt");
		if(fp == NULL) break;

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			tp.SetText(szRead);
			tp.GetWord(szItem);
			if(strcmp(szItem, "EthernetIP") == 0) { tp.GetWord(szIPAddress); strcpy(g_pDevice_Address,szIPAddress);}
			if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Loading Failed : %s", IO_DATA_FILE_NAME);
	}

	return bRet;
}

extern "C"
{

//////////////////////////////////////////////////////////////////////////
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	char SBuffer[BUFFER_SIZE] = {0,};
	char szCmd  [32]          = {0,};  
	SendTermStr[0]    = CR;
	SendTermStr[1]    = 0;
	SendTermStr[2]    = 0;
	RecvTermStr[0]    = CR;
	RecvTermStr[1]    = 0;
	RecvTermStr[2]    = 0;

	_gINIT_LOG();
	
	//Real Communication
	if(ID10 < 0)	TimeOut = TIMEOUT_T;
	else			TimeOut = ID10;

	if(! LoadIOData())
	{
		printf("[ERR]YASKAWA Parameter Read Fail \n");
		return FALSE;
	}

	if(! Net_Connect_Port(g_pDevice_Address, g_nPortNum, TRUE))
	{
		printf("[ERR]YASKAWA Connect failed IP[%s] Port[%d]\n", g_pDevice_Address, g_nPortNum);
		return FALSE;
	}
	else
	{
		printf("[INFO]YASKAWA Connect OK IP[%s] Port[%d]\n", g_pDevice_Address, g_nPortNum);
	}

	g_nTimeOut = ID9;

	Sleep(10000);		//... Need Interval

	if(Net_Connect_Status())	printf("[INFO]YASKAWA Connect Status OK\n");
	else
	{
		printf("[ERR]YASKAWA Connect Status NG\n");
		return FALSE;
	}

	/* 2018.01.23 SW Loading 동작 실행 부분 모두 삭제	
 	//////////////////////////////////////////////////////////////////////////
 	//... Aligner Error Clear at the beginning of driver initialize
 	memset(SBuffer, 0, sizeof(SBuffer));
	strcpy(szCmd, "INIT");
 	sprintf(SBuffer, "$%d%s00", g_nAlignerUnit, szCmd);	
 	if(FALSE == Send_Motion_Command(SBuffer, szCmd, g_nAlignerUnit, TRUE))
	{
		printf("[ERR]YASKAWA Aligner Initialize Failed...\n");
		return FALSE;
	}
	else
	{
		printf("[INFO]YASKAWA Aligner Initialize Completed...\n");
	}
	*/

	return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	Net_Disconnect_Port();
	_gCLOSE_LOG();
	return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	char SBuffer[BUFFER_SIZE] = {0};
	char szCmd  [32]          = {0};

	switch( ID1 ) {	
	case 2:
		//Align Result (Wafer eccentricity)
		strcpy (szCmd , "RALN");
		sprintf(SBuffer , "$%d%s1" , g_nAlignerUnit , szCmd);
		
		//AttachCheckSum(SBuffer , strlen(SBuffer));
		*Result = Send_RQ_Command(SBuffer , szCmd);
		if (!(*Result)) {
			CommStatus = FALSE;
			printf( "[YASKAWA ATM Aligner] AI - [%d-%d-%d] - Error\n" , ID1 , ID2 , ID3);
			return 0;
		}else {
			CommStatus = TRUE;
			return g_dbEccdistnace / 1000;
		}

	case 3: 
		*Result = TRUE; return g_dbAngle;	
	default :	
		*Result = FALSE; break;
	}

	return 0;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) {
	switch( ID1 ) {		
	case 3: 
		//Notch Angle
		g_dbAngle = Data; 
		*Result = TRUE; break;
	default :	
		*Result = FALSE; break;
	}
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	char SBuffer[256] = {0};
	char szCmd  [32]  = {0};
	BOOL bACK         = FALSE;

	switch( ID1 ) {
		case 1  : //Communication Status
			        *Result = TRUE; return CommStatus;
		case 2  : //Robot Run Status
			        *Result = TRUE; return g_nRobotStatus;
		case 3  : //Error Level
			        *Result = TRUE; return g_nErrorLevel;
		case 4  : //Error Code
			*Result = TRUE; return g_nErrorCode;
		case 5:   //Sys Error Code by RSTS Command
			*Result = TRUE; return g_nSysErrorCode;			
		case 6  : //Error Status
			        *Result = TRUE; return g_nErrorStatus;
		case 7  : //Encoder Battery Status
			 	    *Result = TRUE; return g_nBattery1Status;
		case 8  : //Memory Backup Battery Status
				    *Result = TRUE; return g_nBattery2Status;
		case 9  : //Servo Status
					*Result = TRUE; return g_nServoStatus;
	    case 12 : //Wafer Status
			        //Reserve maybe later...
			        *Result = TRUE; 
					if (ID2 == 0) return g_nWaferA;
					else          return g_nWaferB;
		case 24 : //Current Slot <--- Set By Function In Advance Pick/Place/Rotate
			*Result = TRUE; return INDEX_SLOT;
		
		default :	
			*Result = FALSE; break;
	}	
	
	return 0;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) {
	char SBuffer[256] = {0};
	char szCmd  [32]  = {0};
	
	char szTest [256] = {0}; 
	char szStation[32]= {0};
	int  nAngle       = 0;
	BOOL bACK         = FALSE;

	//Memory Variable Update ==> Current Slot
	if (ID1 == 24) {
		*Result = TRUE;
		if   ( Data <= 0 )	INDEX_SLOT = 1;
		else 				INDEX_SLOT = Data;
		return;
	}	
	
	//ID1 : Command Index
	//ID2 : Arm     Index
	//Data: Station
	if (ID1 >= 1 && ID1 <= 24) {		
		switch( ID1 ) {	
		case 1:
			//Initialize : Error Clear, Servo ON
			//Change Mode from "10" to "00" then Init + Home done at the same time
			//Aligner Init			
			bACK = TRUE;
			strcpy (szCmd   , "INIT");
			sprintf(SBuffer , "$%d%s00" , g_nAlignerUnit , szCmd);
			break;
		case 2:
			//Aligner Home			
			bACK = TRUE;
			strcpy (szCmd   , "INIT");
			sprintf(SBuffer , "$%d%s00" , g_nAlignerUnit , szCmd);
			break;
		case 23:
			//////////////////////////////////////////////////////////////////////////
			//TUNo fix to 1
			nAngle = (int)(g_dbAngle * 100);
			strcpy (szCmd   , "MALN");
			sprintf(SBuffer , "$%d%s1%06d" , g_nAlignerUnit , szCmd , nAngle);
			printf("%s \n" , SBuffer);
			//
			*Result = Send_Motion_Command(SBuffer , szCmd , g_nAlignerUnit , TRUE);
			if (!(*Result)) 
			{
				CommStatus = FALSE;
				printf( "[YASKAWA ATM Aligner] DO - [%d-%d-%d-%d] - Error\n" , ID1 , ID2 , ID3 , ID4 );
				return;
			}
			else	CommStatus = TRUE;

			//////////////////////////////////////////////////////////////////////////
			//Yaskawa Aligner must vacuum off manually before pick
			memset (szCmd   , 0 , sizeof(szCmd));
			memset (SBuffer , 0 , sizeof(SBuffer));
			strcpy (szCmd   , "CSOL");
			sprintf(SBuffer , "$%d%sA0" , g_nAlignerUnit , szCmd);
			bACK = TRUE;
			break;		

		default :
			*Result = FALSE;
			return;	
		}

		//////////////////////////////////////////////////////////////////////////
		*Result = Send_Motion_Command(SBuffer , szCmd , g_nAlignerUnit , bACK);
		if (!(*Result)) 
		{
			CommStatus = FALSE;
			printf( "[YASKAWA] DO - [%d-%d-%d-%d] - Error\n" , ID1 , ID2 , ID3 , ID4 );
			return;
		} 
	} 
	else 
	{
		//It is not supported command
		printf( "[YASKAWA] DO - [%d-%d-%d-%d] comming invalid command \n" , ID1 , ID2 , ID3 , ID4 );
		*Result = FALSE;
		return;	
	}
		
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Out( int ID1 , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Name_Out( char *Name , double data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Name_Out( char *Name , int data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Name_In( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Name_Out( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
}
