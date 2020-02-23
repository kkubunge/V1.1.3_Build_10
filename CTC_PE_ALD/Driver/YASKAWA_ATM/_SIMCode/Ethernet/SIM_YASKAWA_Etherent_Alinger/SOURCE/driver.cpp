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
#define IO_DATA_FILE_NAME	".\\Driver\\Ethernet_YASKAWA.dat"
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

	//////////////////////////////////////////////////////////////////////////
	//... DEBUG
	printf("[YASKAWA] Aligner Send Cmnds[%s]\n", Cmnds);
	
	return TRUE;
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
		//////////////////////////////////////////////////////////////////
		//2.Send Command
		//////////////////////////////////////////////////////////////////
		if(FALSE == Net_IO_Send(SendData))		break;			
		
		//////////////////////////////////////////////////////////////////
		//5. ACK Send
		//////////////////////////////////////////////////////////////////
		strcpy (szCmd   , "ACKN");
		sprintf(SBuffer , "$%d%s" , nUnitNum , szCmd);
		_LOG("Started 'Complete ACKN': '%s'. (%d)", SendData, __LINE__);
		if (FALSE ==  Net_IO_Send(SBuffer)) 	break;
	
		_sleep(50);

		bRet = TRUE;
	} while(0);

	if (ROBOT_BUSY == g_nRobotStatus) g_nRobotStatus = ROBOT_IDLE;

	_LOG("Finished 'Send_Motion_Command': '%s'. (%d)", Cmnds, __LINE__);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Send_RQ_Command(char* SendData , char* Cmnds) 
{
	BOOL bRet     = FALSE;
	_LOG("Started 'Send_RQ_Command': '%s'. (%d)", Cmnds, __LINE__);

	g_nRobotStatus = ROBOT_BUSY;

	do {
		//////////////////////////////////////////////////////////////////
		//2.Send Command
		//////////////////////////////////////////////////////////////////
		bRet = Net_IO_Send(SendData); 
		if (FALSE == bRet) break;			
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

	printf("[INFO]YASKAWA Connect OK IP[%s] Port[%d]\n", g_pDevice_Address, g_nPortNum);

	g_nTimeOut = ID9;

	Sleep(10000);		//... Need Interval

	printf("[INFO]YASKAWA Connect Status OK\n");


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


	return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
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
		
	} else {
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
