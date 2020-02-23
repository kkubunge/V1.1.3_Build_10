#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Winsock2.h"
#include "Process.h"

extern "C" 
{
	#include "driver.h"
	#include "CimWsock.h"
	#include "kutletc.h"
	#include "kutlanal.h"
	#include "kutlstr.h"
	#include "TextParser_C.h"
}

#include <vector>

using namespace std;


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

#define DRIVER_NAME	"EIP_RF"
char g_szDrvName[40] = {0,};

#define TIMEOUT_T	1000
#define PollingTime	10

enum { DO, DI, AO, AI, SO, SI };
enum { CS_OFFLINE, CS_ONLINE, CS_CONTROLLER_FAILED };
enum { RF_OFF, RF_ON};
enum { NONE, SET_REF_FLT_TIMEOUT, SET_REF_WRN_THRES, SET_REF_FLT_TIMEOUT2, SET_REF_WRN_TIMEOUT, SET_REF_FLT_THRES, RSET_REF_WRNCNT,RSET_REF_FLTCNT };

int g_nPortNum		= 502;
int g_nCommStatus	= CS_ONLINE;
int g_nTimeout		= TIMEOUT_T;
int g_nRFNumber		= 1;
int	g_nRetry		= 0;

//////////////////////////////////////////////////////////////////////////
//... Real IO for Polling
double g_dbForward = 0, g_dbReflect = 0, g_dbSetPoint = 0, g_dbDeliver = 0, g_dbSetPower = 0;
double g_dbReflectDM = 0, g_dbDeliverDM = 0;
double g_dbRefWrnTimeOut = 0, g_dbRefWrnThreshold = 0;
		
double g_dbFrequency = 0;
double g_dbRecflectWrnCnt = 0;

//////////////////////////////////////////////////////////////////////////
//... 2014.09.27 Virtual IO for Delivery Count
double	g_dbDeliveryCnt		= 0;
double	g_dbDeliveryTol		= 0;	// Unit: %
double	g_dbDeliveryDiff	= 0;

BOOL	g_bDeliveryChecked	= FALSE;

DWORD g_dwIOUpdateTime = 0;				
DWORD g_dwPollIntervalTmAI = 0;

DWORD g_dwAnalogSetTimeAI = 0;
DWORD g_dwAnalogSetTime2AI = 0;

DWORD g_dwAnalogSetTimeAO = 0;

double	g_dbMaxPollTime	= 0;
//////////////////////////////////////////////////////////////////////////
//...2015.06.10
double	g_dbRFGetTime = 0;  
double	g_dbRFGetTimeCfg = 0;

double	g_nGetTotalCount = 0;
double	g_dbFrequencyMax = 0;
double	g_dbFrequencyMin = 0;
double	g_dbFrequencyAvr = 0;
double	g_dbFrequencySum = 0;

double	g_dbReflectMax = 0;
double	g_dbReflectMin = 0;
double	g_dbReflectAvr = 0;
double	g_dbReflectSum = 0;

double	g_dbDeliverMax = 0;
double	g_dbDeliverMin = 0;
double	g_dbDeliverAvr = 0;
double	g_dbDeliverSum = 0;

int		g_nGetFilterCount = 0;
//////////////////////////////////////////////////////////////////////////
int		g_nRF_OnCount	= 0;

enum { eOFFON_OFF, eOFFON_ON};
int		g_nPrintLogOn = eOFFON_OFF;

//////////////////////////////////////////////////////////////////////////
//... 2015.10.11
int		g_nRemoveCycCntCfg = 0;
int		g_nRFCycleCnt	   = 0;
BOOL	g_bIsOkRFOnCycle   = FALSE;
int		g_nGetDataCntDM    = 0;
double  g_dbFreMaxMinAM	   = 0;	
double  g_dbRefMaxMinAM    = 0;
double  g_dbLoadMaxMinAM   = 0;
BOOL	g_bIsOkRFCntRst	   = FALSE; 	
//////////////////////////////////////////////////////////////////////////
#define IO_DATA_FILE_NAME	".\\Parameter\\RFConfig\\EIP_RFGen.dat"

int		g_nRF_DetectSts	  = RF_OFF;
int		g_nRF_OnRdBackSts = RF_OFF;

char g_pDevice_Address[24] = {0,};

CRITICAL_SECTION	g_csLock;

///////////// Parameter Monitoring ////////////////////////////////////////
Parameters g_Param;
ParameterUpdateStatus g_UpdateSts;
///////////////////////////////////////////////////////////////////////////

///////////// DCOP Buffer for average calculation /////////////////////////
vector<RF_DCOP> g_dcopBuffer;
///////////////////////////////////////////////////////////////////////////

#define		TERM_STR		"\n"

//////////////////////////////////////////////////////////////////////////
BOOL LoadIOData()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256] = {0,};
	char szItem[40] = {0,};
	char szIPAddress[24] = {0,};
	int nPortNum;
	int nRFGetTmTemp = 0;
	int nRemoveCycCnt = 0;

	do {
		fp = fopen(IO_DATA_FILE_NAME, "rt");
		if(fp == NULL) break;

		if(NULL == fgets(szRead, 255, fp))		break;
		
		do {
			tpSetText(szRead);
			tpGetWord(szItem, 0);
			//
			if     (strcmp(szItem, "EthernetIP1") == 0 && g_nRFNumber == 1)			{ tpGetWord(szIPAddress, 0);	strcpy(g_pDevice_Address, szIPAddress);	}
			else if(strcmp(szItem, "EthernetIP2") == 0 && g_nRFNumber == 2)			{ tpGetWord(szIPAddress, 0);	strcpy(g_pDevice_Address, szIPAddress);	}
			else if(strcmp(szItem, "PortNum") == 0)									{ tpGetInt(&nPortNum,0);		g_nPortNum = nPortNum;					}
			else if(strcmp(szItem, "RFGetTime") == 0)								{ tpGetInt(&nRFGetTmTemp,0);	g_dbRFGetTimeCfg = (double)nRFGetTmTemp;}
			else if(strcmp(szItem, "RemoveCycCnt") == 0)							{ tpGetInt(&nRemoveCycCnt,0);	g_nRemoveCycCntCfg = nRemoveCycCnt;}		 //... 2015.10.11
			//
			if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);

		fclose(fp);
		bRet = TRUE;
	} while(0);

	if(bRet == FALSE)	printf("[%s] Config File Loading Failed [%s]\n", g_szDrvName, IO_DATA_FILE_NAME);
	
	if(strlen(g_pDevice_Address) < 8)
	{
		printf("[%s] Config File Invalid IP[%s]\n", g_szDrvName, g_pDevice_Address);
		bRet = FALSE;
	}

	return bRet;
}

void PrintBuffer(char *szBufName, unsigned char *pBuf, int nSize)
{
	int i;
	printf("%s :", szBufName);
	for(i=0; i<nSize; i++)
	{
		printf(" %02X", pBuf[i]);
	}
	printf(" in %s\n", g_szDrvName);
}

void Get_ReflectWarning_ThresHold(unsigned char *pRcvBuf)
{
	unsigned short usRefWrnThresHold = 0;
	
	usRefWrnThresHold = pRcvBuf[13];
	usRefWrnThresHold = usRefWrnThresHold << 8;
	usRefWrnThresHold = usRefWrnThresHold + (pRcvBuf[12] & 0xff);
	

	g_dbRefWrnThreshold = (double)usRefWrnThresHold;
}

void Get_ReflectWarning_TimeOut(unsigned char *pRcvBuf)
{
	unsigned long unlRefWrnTimeOut = 0, unlRefWrnTimeOutTemp = 0, unlRefWrnTimeOutTemp2 = 0;
	
	unlRefWrnTimeOut = pRcvBuf[15];
	unlRefWrnTimeOut = unlRefWrnTimeOut << 24;
	unlRefWrnTimeOutTemp = pRcvBuf[14];
	unlRefWrnTimeOutTemp = unlRefWrnTimeOutTemp << 16;
	unlRefWrnTimeOutTemp2 = pRcvBuf[13];
	unlRefWrnTimeOutTemp2 = unlRefWrnTimeOutTemp2 << 8;
	unlRefWrnTimeOut = unlRefWrnTimeOut + unlRefWrnTimeOutTemp + unlRefWrnTimeOutTemp2 + (pRcvBuf[12] & 0xff);

	g_dbRefWrnTimeOut = unlRefWrnTimeOut;
}

//////////////////////////////////////////////////////////////////////////
BOOL IO_Read_CMD(unsigned char nCmd, unsigned char *pRcvBuf, int nReadDataSize)
{
	int nReadCount;
	BOOL bRet=TRUE;

	if(! Net_Read_Count_Char( (char*)pRcvBuf ,  nReadDataSize , &nReadCount, g_nTimeout ))
	{
 		printf("Read failed in %s\n", g_szDrvName);
		PrintBuffer("RcvData", pRcvBuf, nReadCount);
 		g_nCommStatus = CS_OFFLINE;
 		bRet = FALSE;
	}

	if(nReadCount < 1)
	{
		printf("[%s] Analog_Read Invalid ReadCount[%d]\n", g_szDrvName, nReadCount);
		return FALSE;
	}
	
	if(pRcvBuf[9] != 0x00)
	{ 
		printf("[%s] Analog_Read CRS CHECK ERROR [%d]\n", g_szDrvName, pRcvBuf[9]);
		bRet = FALSE;
	}
	
	if(nCmd != pRcvBuf[8]) 
	{
		printf("[%s] Analog_Read COMMAD CHECK ERROR [%d]\n", g_szDrvName, pRcvBuf[8]);
		bRet = FALSE;
	} 
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Analog_Set_CMD(unsigned short usDataSize, unsigned char ucCmd, unsigned char ucSubCmd, double dbData)
{
	unsigned char pSndBuf[256] = {0,};
	unsigned short usByteLenth = 0;
	unsigned long ulData = 0;
	unsigned short usData = 0;
	int nByteCount = 0;
	BOOL bRet=TRUE;
	
//MBAP
	//transactionID
	pSndBuf[ nByteCount++ ] = 0x00;  
	pSndBuf[ nByteCount++ ] = 0x00;

    //ProtocalID
	pSndBuf[ nByteCount++ ] = 0x00;
	pSndBuf[ nByteCount++ ] = 0x00;	
	
	//Number of bytes following = Data Value + Unit ID ~ Data Length
	usByteLenth = usDataSize + 6;
	pSndBuf[ nByteCount++ ] = usByteLenth >> 8;
	pSndBuf[ nByteCount++ ] = usByteLenth & 0x00ff;
	 
	// Unit ID
	pSndBuf[ nByteCount++ ] = 0x01;
	
	
//PDU
	// Function Code;
	pSndBuf[ nByteCount++ ] = 0x64; 
	//AE Host command number
	pSndBuf[ nByteCount++ ] = (BYTE)(ucCmd);  
	
	//CSR
	pSndBuf[ nByteCount++ ] = 0x00;           
	// Data Length
	pSndBuf[ nByteCount++ ] = (BYTE)(usDataSize);			
	pSndBuf[ nByteCount++ ] = 0x00;


	if(ucCmd == 118 && ucSubCmd == SET_REF_WRN_THRES)
	{	
	
		usData = (unsigned short)dbData;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x42;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x05;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Value Data
		pSndBuf[ nByteCount++ ] = usData & 0x00ff;
		pSndBuf[ nByteCount++ ] = usData >> 8;
	}
	else if(ucCmd == 118 && ucSubCmd == SET_REF_WRN_TIMEOUT)
	{	
		ulData = (unsigned long)dbData;
		//pSndBuf[ nByteCount++ ] = 0x08;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x42;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x04;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Value Data
		pSndBuf[ nByteCount++ ] = (unsigned char)(ulData & 0x000000ff);
		pSndBuf[ nByteCount++ ] = (unsigned char)(ulData >> 8);
		pSndBuf[ nByteCount++ ] = (unsigned char)(ulData >> 16);
		pSndBuf[ nByteCount++ ] = (unsigned char)(ulData >> 24);
	}
	else if(ucCmd == 248 && ucSubCmd == SET_REF_WRN_THRES)
	{	
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x42;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x05;
		pSndBuf[ nByteCount++ ] = 0x00;
	}
	else if(ucCmd == 248 && ucSubCmd == SET_REF_WRN_TIMEOUT)
	{	
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x42;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x04;
		pSndBuf[ nByteCount++ ] = 0x00;
	}
	else
	{
		pSndBuf[ nByteCount++ ] = 0x00;
	}
	//PrintBuffer("Set Analog", pSndBuf, nByteCount);

	if(Net_Connect_Status())
	{
		Net_Clear_Buffer();	
		if(! Net_Write_String((char*)pSndBuf, nByteCount))
		{
			printf("Write failed in %s\n", g_szDrvName);
			PrintBuffer("SndData", pSndBuf, nByteCount);
			g_nCommStatus = CS_OFFLINE;
			bRet = FALSE;
		}
		//PrintBuffer("Time Set", pSndBuf, nByteCount);
	}
	else
	{
		printf("[%s] Disconnected in Analog Set\n", g_szDrvName);
		g_nCommStatus = CS_OFFLINE;
		bRet = FALSE;
	}

	return bRet;
}

BOOL Digital_Set_CMD(unsigned short usDataSize, unsigned char ucCmd, unsigned char ucSubCmd,unsigned short usData)
{
	unsigned char pSndBuf[256] = {0,};
	unsigned short usByteLenth = 0;
	unsigned int ulData = 0;

	int nByteCount = 0;
	BOOL bRet=TRUE;
	if(usData == 0) return TRUE; 
//MBAP
	// transactionID
	pSndBuf[ nByteCount++ ] = 0x00;  
	pSndBuf[ nByteCount++ ] = 0x00;

    //ProtocalID
	pSndBuf[ nByteCount++ ] = 0x00;
	pSndBuf[ nByteCount++ ] = 0x00;	
	
	//Number of bytes following = Data Value + Unit ID ~ Data Length
	usByteLenth = usDataSize + 6;
	pSndBuf[ nByteCount++ ] = usByteLenth >> 8;
	pSndBuf[ nByteCount++ ] = usByteLenth & 0x00ff;
	 
	// Unit ID
	pSndBuf[ nByteCount++ ] = 0x01;
	
	
//PDU
	// Function Code;
	pSndBuf[ nByteCount++ ] = 0x64; 
	//AE Host command number
	pSndBuf[ nByteCount++ ] = (BYTE)(ucCmd);  
	
	//CSR
	pSndBuf[ nByteCount++ ] = 0x00;           
	// Data Length
	pSndBuf[ nByteCount++ ] = (BYTE)(usDataSize);			
	pSndBuf[ nByteCount++ ] = 0x00;

	if(ucCmd == 118 && ucSubCmd == RSET_REF_WRNCNT)
	{	
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x42;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x06;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Value
		pSndBuf[ nByteCount++ ] = 0x00;
		pSndBuf[ nByteCount++ ] = 0x00;
	}	
	else if(ucCmd == 118 && ucSubCmd == RSET_REF_FLTCNT)  //2014.12.02
	{	
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x42;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x07;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Value
		pSndBuf[ nByteCount++ ] = 0x00;
		pSndBuf[ nByteCount++ ] = 0x00;
	}
	else
	{
		pSndBuf[ nByteCount++ ] = 0x00;
	}

	if(Net_Connect_Status())
	{
		Net_Clear_Buffer();
		if(!Net_Write_String((char*)pSndBuf, nByteCount))
		{
			printf("Write failed in %s\n", g_szDrvName);
			PrintBuffer("SndData", pSndBuf, nByteCount);
			g_nCommStatus = CS_OFFLINE;
			bRet = FALSE;
		}
	}
	else
	{
		printf("[%s] Disconnected in Digital Set\n", g_szDrvName);
		g_nCommStatus = CS_OFFLINE;
		bRet = FALSE;
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//...2015.06.10
void Analyze_RFData()
{	
	if(g_dbFrequency > g_dbFrequencyMax)						  g_dbFrequencyMax = g_dbFrequency;
	if(g_dbFrequency < g_dbFrequencyMin || g_dbFrequencyMin == 0) g_dbFrequencyMin = g_dbFrequency;
	
	if(g_dbReflect > g_dbReflectMax)								g_dbReflectMax = g_dbReflect;
	if(g_dbReflect < g_dbReflectMin || g_dbReflectMin == 0)			g_dbReflectMin = g_dbReflect;	
	
	if(g_dbDeliver > g_dbDeliverMax)								g_dbDeliverMax = g_dbDeliver;
	if(g_dbDeliver < g_dbDeliverMin || g_dbDeliverMin == 0)			g_dbDeliverMin = g_dbDeliver;	
		
	if(g_dbFrequencySum < 99999999)  
	{
		// When the RF On, push dcop data.
		RF_DCOP dcopData;
		dcopData.deliver = g_dbDeliver;
		dcopData.reflect = g_dbReflect;
		dcopData.freq = g_dbFrequency;

		g_dcopBuffer.push_back(dcopData);
	}

	//... 2015.10.11
	if( 0 < (g_dbFrequencyMax - g_dbFrequencyMin)
		&& g_dbFrequencyMax > 0
		&& g_dbFrequencyMin > 0)
	{
		g_dbFreMaxMinAM = (g_dbFrequencyMax - g_dbFrequencyMin);
	}

	if( 0 < (g_dbReflectMax - g_dbReflectMin)
		&& g_dbReflectMax > 0
		&& g_dbReflectMin > 0) 
	{
		g_dbRefMaxMinAM = (g_dbReflectMax - g_dbReflectMin);
	}
	
	if( 0 < (g_dbDeliverMax - g_dbDeliverMin)
		&& g_dbDeliverMax > 0
		&& g_dbDeliverMin > 0) 
	{
		g_dbLoadMaxMinAM = (g_dbDeliverMax - g_dbDeliverMin);
	}	
	//... TODO. AVERAGE DATA LOSS

	//////////////////////////////////////////////////////////////////////////
	if(eOFFON_ON == g_nPrintLogOn)
	{
		printf("Get Cnt[%d] Fwd[%.0f %.0f %.0f] Ref[%.0f %.0f %.0f] Frq[%.0f %.0f %.0f]\n",
			g_nGetTotalCount, 
			g_dbDeliverAvr, g_dbDeliverMin, g_dbDeliverMax,
			g_dbReflectAvr, g_dbReflectMin, g_dbReflectMax,
			g_dbFrequencyAvr, g_dbFrequencyMin, g_dbFrequencyMax);
	}
}

void InitSummaryData()
{
	//... TODO. Init Summary Data
	//... Init Count
	g_nGetTotalCount = 0;

	//... Init Min/Max/Avr/Sum
	g_dbDeliverMin = 0;
	g_dbDeliverMax = 0;
	g_dbDeliverAvr = 0;
	g_dbDeliverSum = 0;

	g_dbReflectMin = 0;
	g_dbReflectMax = 0;
	g_dbReflectAvr = 0;
	g_dbReflectSum = 0;

	g_dbFrequencyMin = 0;
	g_dbFrequencyMax = 0;
	g_dbFrequencyAvr = 0;
	g_dbFrequencySum = 0;

	//... 2015.10.11	
	g_dbFreMaxMinAM  = 0;
	g_dbRefMaxMinAM  = 0;
	g_dbLoadMaxMinAM = 0;
}

//////////////////////////////////////////////////////////////////////////
void Get_SNAP_DATA(unsigned char *pRcvBuf)
{
	unsigned short usForward = 0, usReflect = 0, usDeliver = 0, usSetPower = 0;
	unsigned short usRF_OnRealSts = 0, usRF_OnReadBack = 0;
	unsigned long ulFrequency = 0, ulFrequencyTemp = 0, ulFrequencyTemp2 = 0;
	unsigned long unlReflectWrnCnt = 0, unlReflectWrnCntTemp = 0, unlReflectWrnCntTemp2 = 0;  //2014.12.08
	unsigned char usRF_OnSts = 0x00;
	unsigned char usRF_OnReadback = 0x00;

	//...
	usForward = pRcvBuf[17];
	usForward = usForward << 8;
	usForward = usForward + (pRcvBuf[16] & 0xff);

	//...
	usReflect = pRcvBuf[19];
	usReflect = usReflect << 8;
	usReflect = usReflect + (pRcvBuf[18] & 0xff);

	//...
	usDeliver = pRcvBuf[21];
	usDeliver = usDeliver << 8;
	usDeliver = usDeliver + (pRcvBuf[20] & 0xff);

	//...
	usSetPower = pRcvBuf[23];
	usSetPower = usSetPower << 8;
	usSetPower = usSetPower + (pRcvBuf[22] & 0xff);

	//...
	ulFrequency = pRcvBuf[35];
	ulFrequency = ulFrequency << 24;
	ulFrequencyTemp = pRcvBuf[34];
	ulFrequencyTemp = ulFrequencyTemp << 16;
	ulFrequencyTemp2 = pRcvBuf[33];
	ulFrequencyTemp2 = ulFrequencyTemp2 << 8;
	ulFrequency = ulFrequency + ulFrequencyTemp + ulFrequencyTemp2 + (pRcvBuf[32] & 0xff);

	usRF_OnSts		= (BYTE)pRcvBuf[36] & 0x20;
	usRF_OnReadback = (BYTE)pRcvBuf[36] & 0x40;
	
	//... 2014.12.08  Add Reflect Warning Count
	unlReflectWrnCnt = pRcvBuf[47];
	unlReflectWrnCnt = unlReflectWrnCnt << 24;
	unlReflectWrnCntTemp = pRcvBuf[46];
	unlReflectWrnCntTemp = unlReflectWrnCntTemp << 16;
	unlReflectWrnCntTemp2 = pRcvBuf[45];
	unlReflectWrnCntTemp2 = unlReflectWrnCntTemp2 << 8;
	unlReflectWrnCnt = unlReflectWrnCnt + unlReflectWrnCntTemp + unlReflectWrnCntTemp2 + (pRcvBuf[44] & 0xff);
	//...

	//... Get RF On Status	
	if(usRF_OnSts == 0x20)			g_nRF_DetectSts = RF_ON;
	else							g_nRF_DetectSts = RF_OFF;

	if(usRF_OnReadback == 0x40)		g_nRF_OnRdBackSts = RF_ON;
	else							g_nRF_OnRdBackSts = RF_OFF;

	//... Data Conversion
	g_dbForward		= (double) usForward / 10;
	g_dbReflect		= (double) usReflect / 10;
	g_dbDeliver		= (double) usDeliver / 10;
	g_dbSetPower	= (double) usSetPower/ 10;
	g_dbFrequency	= (double) ulFrequency/10;
	g_dbRecflectWrnCnt = (double) unlReflectWrnCnt;			//2014.12.08

	//... RF Virtual Data in RF ON Status
	if(g_nRF_DetectSts == RF_ON)
	{
		g_dbReflectDM = g_dbReflect;
		g_dbDeliverDM = g_dbDeliver;
		g_bIsOkRFOnCycle = TRUE;
 	}
 	else
 	{
 		g_dbReflectDM = 0;
 		g_dbDeliverDM = 0;
		g_dbRFGetTime = 0;					//...2015.06.10
		
		if( g_bIsOkRFOnCycle == TRUE )					//...2015.10.11 Counting RF On Cycle 
		{
			if(g_nRFCycleCnt < 99999) g_nRFCycleCnt ++; //... Protect Overflow
			g_bIsOkRFOnCycle = FALSE;
		}
		g_bIsOkRFCntRst  = TRUE;
	}

	//////////////////////////////////////////////////////////////////////////	
	//... 2015.06.10
	if(g_dbRFGetTime > g_dbRFGetTimeCfg 
		&& g_nRF_DetectSts == RF_ON
		&& g_nRFCycleCnt >= g_nRemoveCycCntCfg		//... 2015.10.11 Compare RF On Cycle & Cfg Data  
		)								
	{
		if(g_bIsOkRFCntRst  == TRUE)
		{
			g_nGetDataCntDM = 0;	//... Reset RF On Count
			g_bIsOkRFCntRst = FALSE;
		}
		
		g_nGetDataCntDM++;      //...2015.10.11     //... Counting RF On Step
		
		Analyze_RFData();
	}
	//////////////////////////////////////////////////////////////////////////
	//... 2014.09.27 Count Delivery Power Normal Count
	if(RF_ON == g_nRF_DetectSts)
	{
		if(FALSE == g_bDeliveryChecked)
		{
			if(g_dbDeliveryTol > 0 && g_dbForward > 0 && g_dbDeliver > 0)
			{

				//... 2014.12.11 Absolute Check
				g_dbDeliveryDiff = fabs(g_dbForward - g_dbDeliver);
				
				if(g_dbDeliveryDiff <= (g_dbForward * (g_dbDeliveryTol * 0.01)))
				{
					g_dbDeliveryCnt = g_dbDeliveryCnt + 1;
					g_bDeliveryChecked = TRUE;
					
					if(g_dbDeliveryCnt > 9999)		g_dbDeliveryCnt = 9999;
				}
			}
		}
	}
	else
	{
		g_bDeliveryChecked = FALSE;

		// When RF Off, Calculate average of dcop data.
		CalcDCOPAverage();
	}
}

void CalcDCOPAverage()
{
	// RF가 On되는 시점부터 취합된 데이터를 바탕으로 
	// RF가 Off되는 시점에 호출되어 누적 평균을 구합니다.
	// 마지막 데이터는 누적하지 않기 때문에 버퍼에 쌓인 데이터 수 - 1 개만큼 계산.
	int stackedSize = g_dcopBuffer.size() - g_nGetFilterCount;
	
	if( stackedSize < 1 ) 
	{
		// 버퍼에 쌓인 데이터가 없다.
		return;
	}

	// 버퍼에 쌓아둔 값을 누적
	for( int i=0; i<stackedSize; i++ ) 
	{
		g_dbDeliverSum += g_dcopBuffer[i].deliver;
		g_dbReflectSum += g_dcopBuffer[i].reflect;
		g_dbFrequencySum += g_dcopBuffer[i].freq;
	}
	g_nGetTotalCount += stackedSize;

	// 평균 산출
	if( g_nGetTotalCount > 0 )
	{
		if(0 < g_dbFrequencySum)	g_dbFrequencyAvr = g_dbFrequencySum / g_nGetTotalCount;
		if(0 < g_dbReflectSum)		g_dbReflectAvr   = g_dbReflectSum   / g_nGetTotalCount;
		if(0 < g_dbDeliverSum)		g_dbDeliverAvr   = g_dbDeliverSum   / g_nGetTotalCount;
	}

	g_dcopBuffer.clear();
}

int MakePacket(unsigned char *pSndBuf, BYTE cmd, BYTE *pDataBytes, int nDataBytes)
{
	const int headerSize = 6;

	unsigned char pSndBufTemp[256];
	unsigned short usByteLength = 0;
	unsigned short nByteCountTemp = 0;

	memset(pSndBufTemp, 0, sizeof(pSndBufTemp));

//MBAP
	// transactionID
	pSndBufTemp[ nByteCountTemp++ ] = 0x00;  
	pSndBufTemp[ nByteCountTemp++ ] = 0x00;

    //ProtocalID
	pSndBufTemp[ nByteCountTemp++ ] = 0x00;
	pSndBufTemp[ nByteCountTemp++ ] = 0x00;	
	
	//Number of bytes following = Data Value + Unit ID ~ Data Length
	usByteLength = headerSize + nDataBytes;
	pSndBufTemp[ nByteCountTemp++ ] = (usByteLength >> 8) &0xff;
	pSndBufTemp[ nByteCountTemp++ ] = usByteLength & 0xff;
	 
	// Unit ID
	pSndBufTemp[ nByteCountTemp++ ] = 0x01;	
	
//PDU
	// Function Code;
	pSndBufTemp[ nByteCountTemp++ ] = 0x64; 
	//AE Host command number
	pSndBufTemp[ nByteCountTemp++ ] = cmd;
	
	//CSR
	pSndBufTemp[ nByteCountTemp++ ] = 0x00;
	
	// Data Length
	pSndBufTemp[ nByteCountTemp++ ] = nDataBytes & 0xff;
	pSndBufTemp[ nByteCountTemp++ ] = (nDataBytes >> 8) & 0xff;

	if( nDataBytes > 0 ) 
	{
		memcpy(&pSndBufTemp[nByteCountTemp], pDataBytes, nDataBytes);
	}	

	memcpy(pSndBuf,pSndBufTemp, nByteCountTemp + nDataBytes);
	
	return nByteCountTemp + nDataBytes;
}

BOOL RequestCommand(BYTE *pResponseData, int *pResponseLength, BYTE cmd, BYTE *pDataBytes, int nDataBytesCnt)
{
	const int headerSize = 6;

	BYTE sendBuffer[80];
	BYTE header[32];
	BYTE body[255];

	int nByteCount;
	int nPacketCount;
	int nDataLength;

	BYTE responseCmd; 
	BYTE functionCode;
	BYTE exceptionCode;
	BYTE csrCode;

	BOOL bCommSuccess = FALSE;	

	memset(sendBuffer, 0, sizeof(sendBuffer));
	memset(header, 0, sizeof(header));
	memset(body, 0, sizeof(body));

	nByteCount = MakePacket(sendBuffer, cmd, pDataBytes, nDataBytesCnt);
	
	EnterCriticalSection(&g_csLock);
	do {
		Net_Clear_Buffer();
		if( !Net_Write_String((char*)sendBuffer, nByteCount) ) 
		{
			printf("[%s] Send Failed.\n", DRIVER_NAME);
			break;
		}
		
		if( !Net_Read_Count_Char((char*)header, headerSize, &nByteCount, g_nTimeout) ) 
		{
			PrintBuffer("Recv Header Error : ", header, nByteCount);		
			break;
		}
		
		nPacketCount = header[4] << 8 | header[5];
		if( !Net_Read_Count_Char((char*)body, nPacketCount, &nByteCount, g_nTimeout) ) {
			PrintBuffer("Recv Data Error : ", body, nByteCount);		
			break;
		}

		bCommSuccess = TRUE;
	} while(0);
	LeaveCriticalSection(&g_csLock);

	if( !bCommSuccess ) 
	{
		printf("[%s] Communication Failed!!\n", DRIVER_NAME);
		return FALSE;
	}
	
	functionCode = body[1];
	if( functionCode == 0xE4 ) {
		exceptionCode = body[2];
		printf("[%s] returned exception %d\n", DRIVER_NAME,exceptionCode);
		return FALSE;
	}

	responseCmd = body[2];	
	csrCode = body[3];
	nDataLength = (body[5] << 8) | body[4];

	if( csrCode != 0 ) 
	{
		printf("[%s] CSR Check Error(CSR Code = %d)\n", DRIVER_NAME, csrCode);
		return FALSE;
	}
	
	if( cmd != responseCmd ) {
		printf("[%s] Wrong command received SendCmd(%d) != ResponseCmd(%d)\n", DRIVER_NAME, cmd, responseCmd);
		return FALSE;
	}
	
	// Unit ID(1), Function Code(1), AE Command Number(1), CSR(1), Data length(2), Data Bytes(n)
	// Copy only data bytes
	*pResponseLength = nDataLength;
	memcpy(pResponseData, body + 6, nDataLength);	
	
	return TRUE;
}

int GetDigitalValue(BYTE cmd, BYTE subCmd)
{
	BYTE sendData[4];
	BYTE recvData[255];

	int nSendDataLength = 0;
	int nDataLength = 0;
	int val = 0;
	int i;

	memset(sendData, 0, sizeof(sendData));
	memset(recvData, 0, sizeof(recvData));
	
	if( subCmd < 255 ) {
		sendData[0] = subCmd;
		nDataLength = 1;
	}

	if( cmd == 248 ) {
		nDataLength = 2;
	}

	if( RequestCommand(recvData, &nDataLength, cmd, sendData, nDataLength ) )
	{
		for( i=0; i<nDataLength; ++i ) {
			val |= (recvData[i] << 8*i);			
		}
	}

	return val;
}

void UpdateParameters() 
{
	g_UpdateSts = Param_Updaing;

	g_Param.freqMode			= GetDigitalValue(148, 255);
	g_Param.ignitionMode		= GetDigitalValue(248, 15);
	g_Param.startFreq			= GetDigitalValue(146, 255);
	g_Param.minFreq				= GetDigitalValue(144, 255);
	g_Param.maxFreq				= GetDigitalValue(145, 255);
	g_Param.freqStepMax			= GetDigitalValue(248, 2);
	g_Param.freqStepMin			= GetDigitalValue(248, 1);
	g_Param.returningThreshold	= GetDigitalValue(158, 255);
	g_Param.highTuneThreshold	= GetDigitalValue(248, 6);
	g_Param.lowTuneThreshold	= GetDigitalValue(248, 7);
	g_Param.gammaThresholdMode	= GetDigitalValue(248, 21);
	g_Param.tuningTimeout		= GetDigitalValue(138, 255);
	g_Param.tuningDelay			= GetDigitalValue(160, 255);
	g_Param.tuningGainDelay		= GetDigitalValue(248, 23);
	g_Param.maxTuningCount		= GetDigitalValue(248, 8);
	g_Param.stepUpGain			= GetDigitalValue(248, 3);
	g_Param.stepDownGain		= GetDigitalValue(248, 4);
	g_Param.scanStepSize		= GetDigitalValue(248, 17);
	g_Param.tuningStepTime		= GetDigitalValue(248, 22);
	g_Param.ipAddr				= GetDigitalValue(204, 0);
	g_Param.subnetMask			= GetDigitalValue(204, 2);
	g_Param.gatewayAddr			= GetDigitalValue(204, 1);
	g_Param.dhcpClient			= GetDigitalValue(204, 5);


	g_UpdateSts = Param_Ready;
}

//////////////////////////////////////////////////////////////////////////
BOOL Net_Port_Write(unsigned short usDataSize, unsigned char ucCmd, unsigned char ucSubCmd)
{
	unsigned char pSndBuf[256] = {0,};
	unsigned short usByteLenth = 0;

	int nByteCount = 0;
	BOOL bRet=TRUE;

	//MakePacket(pSndBuf,ucCmd,usDataSize,&usByteLenth);
	
//MBAP
	// transactionID
	pSndBuf[ nByteCount++ ] = 0x00;  
	pSndBuf[ nByteCount++ ] = 0x00;

    //ProtocalID
	pSndBuf[ nByteCount++ ] = 0x00;
	pSndBuf[ nByteCount++ ] = 0x00;	
	
	//Number of bytes following = Data Value + Unit ID ~ Data Length
	usByteLenth = usDataSize + 6;
	pSndBuf[ nByteCount++ ] = usByteLenth >> 8;
	pSndBuf[ nByteCount++ ] = usByteLenth & 0x00ff;
	 
	// Unit ID
	pSndBuf[ nByteCount++ ] = 0x01;
	
	
//PDU
	// Function Code;
	pSndBuf[ nByteCount++ ] = 0x64; 
	//AE Host command number
	pSndBuf[ nByteCount++ ] = (BYTE)(ucCmd);  
	
	//CSR
	pSndBuf[ nByteCount++ ] = 0x00;
	
	// Data Length
	pSndBuf[ nByteCount++ ] = (BYTE)(usDataSize);			
	pSndBuf[ nByteCount++ ] = 0x00;

	//Data value
	if(ucCmd == 248 && ucSubCmd == RSET_REF_WRNCNT)
	{	
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x42;
		pSndBuf[ nByteCount++ ] = 0x00;
		//Sub Command
		pSndBuf[ nByteCount++ ] = 0x06;
		pSndBuf[ nByteCount++ ] = 0x00;
	}
	else
	{
		pSndBuf[ nByteCount++ ] = 0x06;		//... 2014.12.08
	}
	

    //printf("[AE RF TCP IP WRITE DATA] ==> %s  Count %d\n",*pSndBuf,nByteCount);

 	Net_Clear_Buffer();
 	if(! Net_Write_String((char*)pSndBuf, nByteCount))
	{
		printf("Write failed in %s\n", g_szDrvName);
		PrintBuffer("SndData", pSndBuf, nByteCount);
		g_nCommStatus = CS_OFFLINE;
		bRet = FALSE;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL Net_Port_Rread(unsigned char nCmd, unsigned char *pRcvBuf, int nReadDataSize)
{	
	int nReadCount;
	BOOL bRet=TRUE;

	if(!Net_Read_Count_Char( (char*)pRcvBuf ,  nReadDataSize , &nReadCount, g_nTimeout))
	{
 		printf("Read failed in %s\n", g_szDrvName);
		PrintBuffer("RcvData", pRcvBuf, nReadCount);
 		g_nCommStatus = CS_OFFLINE;
 		bRet = FALSE;
	}

	if(nReadCount < 1)		return FALSE;
	
	if(pRcvBuf[9] != 0x00)
	{ 
		printf("[%s] Port_Read CRS CHECK ERROR[%d]\n", g_szDrvName, pRcvBuf[9]);
		bRet = FALSE;
	}
	
	if(nCmd != pRcvBuf[8]) 
	{
		printf("[%s] Port_Read COMMAD CHECK ERROR[%d]\n", g_szDrvName, pRcvBuf[8]);
		bRet = FALSE;
	}
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL UpdateAI()
{
	BOOL	bRet = FALSE;
	unsigned char pRcvBuf[256] = {0,};
	
	do{
		//...		
 		if(! Net_Port_Write(1, (unsigned char)219, (unsigned char)0))	break;
		if(! Net_Port_Rread((unsigned char)219, pRcvBuf, 52))			break; //2014.12.08		
		Get_SNAP_DATA(pRcvBuf);

		g_nCommStatus = CS_ONLINE;
		bRet = TRUE;
	}while(0);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Thread

BOOL	g_bThreadRun = FALSE;
BOOL	g_bThreadFuncRun = FALSE;

void Polling_Thread(void *pParam)
{
	DWORD dwCurrentTime = 0;
	DWORD dwElapsedTime = 0;
	double dbRFGetTimeTemp = 0;
	BOOL  bUpdateOK	= FALSE;
	BOOL  bNetConOK	= FALSE;

	g_bThreadRun = TRUE;
	g_bThreadFuncRun = TRUE;

	Sleep(5000);

	while(g_bThreadRun)
	{
		//////////////////////////////////////////////////////////////////////////
		//... TickCount Period Update Check
		dwCurrentTime = GetTickCount();
		dwElapsedTime = labs(dwCurrentTime - g_dwIOUpdateTime);
		
		if(g_dwIOUpdateTime > 0)
		{
			g_dwPollIntervalTmAI = dwElapsedTime;
			//...2015.06.10
			dbRFGetTimeTemp = (double)dwElapsedTime;
			g_dbRFGetTime += dbRFGetTimeTemp;
			//...
			//... Max Poll Time
			if(g_dbMaxPollTime <= (double) g_dwPollIntervalTmAI)
				g_dbMaxPollTime = (double) g_dwPollIntervalTmAI;
		}

		g_dwIOUpdateTime = dwCurrentTime;
		
		//////////////////////////////////////////////////////////////////////////
		EnterCriticalSection(&g_csLock);
		Sleep(PollingTime);

		bNetConOK = Net_Connect_Status();			//... Check Network Status
		//.....
		if(bNetConOK)		bUpdateOK = UpdateAI();	//.. Main IO Update Part
		//.....
		LeaveCriticalSection(&g_csLock);
		//////////////////////////////////////////////////////////////////////////

		if(! bNetConOK)
		{	//... Network Disconnect
			printf("[%s] Disconnected...\n", g_szDrvName);
			g_nCommStatus = CS_OFFLINE;
			Sleep(3000);
		}
		else if(! bUpdateOK)
		{	//... Read Fail
			printf("[%s] IO Update Failed\n", g_szDrvName);
			Sleep(1000);
		}

		Sleep(PollingTime);
	}

	g_bThreadFuncRun = FALSE;
	g_nCommStatus = CS_OFFLINE;

	_endthread();
}


//////////////////////////////////////////////////////////////////////////
void PrintError( int IO , int ID1 , int ID2 , int ID3 , int ID4 ) {
	switch( IO )
	{
	case DO :	printf( "DO Failed : [%d][%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3 , ID4, g_szDrvName); 
				break;
	case DI :	printf( "DI Failed : [%d][%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3 , ID4, g_szDrvName); 
				break;
	case AO :	printf( "AO Failed : [%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3, g_szDrvName); 
				break;
	case AI :	printf( "AI Failed : [%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3, g_szDrvName); 
				break;
	case SO :	printf( "SO Failed : [%d] in %s Driver\n" , ID1, g_szDrvName); 
				break;
	case SI :	printf( "SI Failed : [%d] in %s Driver\n" , ID1, g_szDrvName); 	
				break;
	default :	printf( "I/O Failed in %s Driver", g_szDrvName);
				break;
	}
}

//////////////////////////////////////////////////////////////////////////
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result )
{
	*Result = TRUE;

	switch(ID1)
	{
	case 99:	return g_nCommStatus;		//.. RF1CommStsDI
	case 1:		return g_nRF_DetectSts;		//.. RF1GenStsDI
	case 2:		return g_nRF_OnRdBackSts;	//.. RF1CmdStsDI
	case 10:	return g_nGetDataCntDM;     //.. RF1 Data Count for Depo one Cycle  2015.10.11
	case 11:	return g_nGetTotalCount;	//.. RF Total Data Count in whole process time. 2016.02.15	

	case 20:	return g_Param.freqMode;
	case 21:	return g_Param.ignitionMode;
	case 22:	return g_Param.dhcpClient;
	case 23:	return g_Param.gammaThresholdMode;
	case 24:	return g_Param.startFreq;
	case 25:	return g_Param.minFreq;
	case 26:	return g_Param.maxFreq;
	case 27:	return g_Param.freqStepMax;
	case 28:	return g_Param.freqStepMin;
	case 29:	return g_Param.returningThreshold;
	case 30:	return g_Param.highTuneThreshold;
	case 31:	return g_Param.lowTuneThreshold;
	case 32:	return g_Param.tuningTimeout;
	case 33:	return g_Param.tuningDelay;
	case 34:	return g_Param.tuningGainDelay;
	case 35:	return g_Param.maxTuningCount;
	case 36:	return g_Param.stepUpGain;
	case 37:	return g_Param.stepDownGain;
	case 38:	return g_Param.scanStepSize;
	case 39:	return g_Param.tuningStepTime;	
	case 40:	return g_UpdateSts;

	default:
		PrintError(DI, ID1, ID2, ID3, ID4);
		*Result = FALSE;
		return 0;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result )
{
	unsigned char pRcvBuf[256] = {0,};
	unsigned char ucSubCmd = 0;
	unsigned char ucCmd = 0;
	unsigned short usDataSize;
	*Result = TRUE;

	switch(ID1)
	{
	case 1:
	    InitSummaryData();
		g_nRFCycleCnt = 0;     //... 2015.10.11 Reset RF Cycle Count
		g_nGetDataCntDM = 0;   //... Reset RF On Count
		ucCmd = 118;
		usDataSize = 6;
		if(ID2 == 6)		ucSubCmd = RSET_REF_WRNCNT;		//.. RF1RefWrnRstDO
		else if(ID2 == 7)	ucSubCmd = RSET_REF_FLTCNT;		//.. RF1RefFltRstDO
		else
		{
			PrintError(DO, ID1, ID2, ID3, ID4);
			*Result = FALSE;
			return;
		}
		break;

	case 3:
		g_nPrintLogOn = Data;
		break;

	case 4:
		if( Data == 1 && Net_Connect_Status() ) 
		{
			UpdateParameters();			
		}
		return;

		break;

	case 12:
		 g_nGetFilterCount = Data;
		return;

	default:
		PrintError(DO, ID1, ID2, ID3, ID4);
		*Result = FALSE;
		return;
	}

	EnterCriticalSection(&g_csLock);
	Sleep(PollingTime);		//2014.12.02
	if(! Digital_Set_CMD(usDataSize, ucCmd, ucSubCmd, (unsigned short)Data))	*Result = FALSE;	
	if(! IO_Read_CMD((unsigned char)ucCmd, pRcvBuf, 13))						*Result = FALSE;
	LeaveCriticalSection(&g_csLock);
	
	if(FALSE == *Result)	PrintError(DO, ID1, ID2, ID3, ID4);
}

//////////////////////////////////////////////////////////////////////////
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result )
{
	unsigned char pRcvBuf[256] = {0,};
	DWORD dwStartTime = 0;
	DWORD dwEndTime = 0;
	DWORD dwElapsedTime = 0;

	*Result = FALSE;

	switch(ID1)
	{
	case 10:	*Result = TRUE;		return g_dbForward;			//.. RF1ForwardAI
	case 11:	*Result = TRUE;		return g_dbDeliver;			//.. RF1LoadPowAI
	case 12:	*Result = TRUE;		return g_dbReflect;			//.. RF1RefPowAI
	case 13:	*Result = TRUE;		return g_dbSetPower;		//.. RF1SetPowAI
	case 14:	*Result = TRUE;		return g_dbFrequency;		//.. RF1FrequencyAI
	case 15:	*Result = TRUE;		return g_dbReflectDM;		//.. RF1RefPowAM for FDC
	case 16:	*Result = TRUE;		return g_dbDeliverDM;		//.. RF1LoadPowAM for FDC
	case 17:	*Result = TRUE;		return g_dbRecflectWrnCnt;	//.. RF1ReflCntAI
	case 18:
			{	//.. RF1RefWrnTOAI
				EnterCriticalSection(&g_csLock);
				dwStartTime = GetTickCount();

				if(Analog_Set_CMD(4, (unsigned char)248, SET_REF_WRN_TIMEOUT, 0))
				{
					if(IO_Read_CMD((unsigned char)248, pRcvBuf, 16))
					{
						Get_ReflectWarning_TimeOut(pRcvBuf);
						*Result = TRUE;
					}
				}
				dwEndTime  = GetTickCount();
				dwElapsedTime = labs(dwStartTime - dwEndTime);

				g_dwAnalogSetTimeAI = dwElapsedTime;
				LeaveCriticalSection(&g_csLock);
				return g_dbRefWrnTimeOut;
			}
	case 19:
			{	//.. RF1RefWrnThrsAI
				EnterCriticalSection(&g_csLock);
	        	dwStartTime = GetTickCount();
				if(Analog_Set_CMD(4, (unsigned char)248, SET_REF_WRN_THRES, 0))
				{
					if(IO_Read_CMD((unsigned char)248, pRcvBuf, 14))
					{
						Get_ReflectWarning_ThresHold(pRcvBuf);
						*Result = TRUE;
					}
				}
				dwEndTime  = GetTickCount();
				dwElapsedTime = labs(dwStartTime - dwEndTime);

				g_dwAnalogSetTime2AI = dwElapsedTime;
				LeaveCriticalSection(&g_csLock);
			}
			return g_dbRefWrnThreshold;	

	//... 2014.09.27 Virtual IO for Delivery Count
	case 20:	*Result = TRUE;		return (double) g_dwPollIntervalTmAI;	
	case 21:	*Result = TRUE;		return (double) g_dwAnalogSetTimeAI;
	case 22:	*Result = TRUE;		return (double) g_dwAnalogSetTime2AI;
	case 23:	*Result = TRUE;		return (double) g_dwAnalogSetTimeAO;
	case 24:	*Result = TRUE;		return g_dbMaxPollTime;

	case 30:	*Result = TRUE;		return g_dbDeliveryCnt;		//.. RF1LoadCntAIO

	//... 2015.06.10
	case 31:	*Result = TRUE;		return g_dbFrequencyMax;
	case 32:	*Result = TRUE;		return g_dbFrequencyMin;
	case 33:	*Result = TRUE;		return g_dbFrequencyAvr;
	case 34:	*Result = TRUE;		return g_dbReflectMax;
	case 35:	*Result = TRUE;		return g_dbReflectMin;
	case 36:	*Result = TRUE;		return g_dbReflectAvr;
	case 37:	*Result = TRUE;		return g_dbDeliverMax;
	case 38:	*Result = TRUE;		return g_dbDeliverMin;
	case 39:	*Result = TRUE;		return g_dbDeliverAvr;
	//... 2015.10.11
	case 40:	*Result = TRUE;     return g_dbLoadMaxMinAM;	 
	case 41:	*Result = TRUE;     return g_dbRefMaxMinAM;
	case 42:	*Result = TRUE;     return g_dbFreMaxMinAM;
	//... 
	default:	//.. Case. Invalid ID
		PrintError(AI, ID1, ID2, ID3, 0);
		*Result = FALSE;
		return 0;
	}
	
	*Result = FALSE;
	if(! (*Result))		PrintError(AI, ID1, ID2, ID3, 0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result )
{
	unsigned char pRcvBuf[256] = {0,};
	unsigned char ucSubCmd = 0;
	unsigned char ucCmd = 0;
	unsigned short usDataSize;
	DWORD dwStartTime = 0;
	DWORD dwEndTime = 0;
	DWORD dwElapsedTime = 0;

	*Result = TRUE;

	switch(ID1)
	{
	case 1:
		ucCmd = 118;
		usDataSize = 8;
		if(ID2 == 2)	ucSubCmd = SET_REF_WRN_TIMEOUT;		//.. RF1RefWrnTOAO
		else
		{
			*Result = FALSE;
			return;
		}
		break;
	case 2:
		ucCmd = 118;
		usDataSize = 6;
		if(ID2 == 4)	ucSubCmd = SET_REF_WRN_THRES;		//.. RF1RefWrnThrsAO
		else
		{
			*Result = FALSE;
			return;
		}
		break;
	case 30:
		g_dbDeliveryCnt = Data;		//... Load Power Limit Count
		return;		//... Not Send Command
	case 31:
		g_dbDeliveryTol = Data;		//... Load Power Limit
		return;		//... Not Send Command
	
	default:	//.. Case. Invalid ID
		PrintError(AO, ID1, ID2, ID3, 0);
		*Result = FALSE;
		return;
	}

	EnterCriticalSection(&g_csLock);
	Sleep(PollingTime);		//2014.12.02
	dwStartTime = GetTickCount();
	if(! Analog_Set_CMD(usDataSize, ucCmd, ucSubCmd, Data))		*Result = FALSE;
	if(! IO_Read_CMD((unsigned char)ucCmd, pRcvBuf, 13))    *Result = FALSE;
	dwEndTime  = GetTickCount();
	dwElapsedTime = labs(dwStartTime - dwEndTime);

	g_dwAnalogSetTimeAO = dwElapsedTime;
	LeaveCriticalSection(&g_csLock);

	if(!(*Result))	PrintError(AO, ID1, ID2, ID3, 0);
}
//-----------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//... 2014.10.10 Add RF1,2 Separate (ID9: RF Number)
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 )
{
	BOOL bRet = FALSE;

	// Initialize RF Parameter Monitoring variables.
	memset(&g_Param, 0, sizeof(Parameters));	
	g_UpdateSts = Param_Ready;

	//... Read RF Number (ID9) ... Range Fixed RF 1 or 2
	if(ID9 == 2)		g_nRFNumber = 2;
	else				g_nRFNumber = 1;

	//... Driver Name
	sprintf(g_szDrvName, "%s%d", DRIVER_NAME, g_nRFNumber);

	// DCOP Buffer reservation.
	g_dcopBuffer.reserve(50);

	do {
		if(ID10 <= 0)		g_nTimeout = TIMEOUT_T;
		else				g_nTimeout = ID10;
		
		if(! LoadIOData())
		{
			printf("[%s] Parameter Read Fail \n", g_szDrvName);
			break;
		}

		if(! Net_Connect_Port(g_pDevice_Address, g_nPortNum, TRUE))
		{ 
			printf("[%s] Connect failed IP[%s] Port[%d]\n", g_szDrvName, g_pDevice_Address, g_nPortNum);
			break;
		}

		printf("[%s] Driver Connect Success  IP[%s] Port[%d]\n", g_szDrvName, g_pDevice_Address, g_nPortNum);

		InitializeCriticalSection(&g_csLock);
		Sleep(1000);
		_beginthread(Polling_Thread, 0, NULL);

		bRet = TRUE;
	} while(0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
void DeInitialize()
{
	int nTCount = 0;

	g_bThreadRun = FALSE;
	
	while(g_bThreadFuncRun)
	{
		Sleep(1000);
		if(++nTCount > 5)		break;
	}
	DeleteCriticalSection(&g_csLock);
}

//////////////////////////////////////////////////////////////////////////
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {

	DeInitialize();
	Net_Disconnect_Port();

	return TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) 
{
	char szBuffer[20];

	memset(szBuffer, 0, sizeof(szBuffer));

	*Result = TRUE;

	switch(ID1) 
	{
	case 1:	// RF IpAddr
		sprintf(szBuffer, "%d.%d.%d.%d", (g_Param.ipAddr >> 24) & 0xff
										, (g_Param.ipAddr >> 16) & 0xff
										, (g_Param.ipAddr >> 8) & 0xff
										, (g_Param.ipAddr) &0xff );
		strcpy(data, szBuffer);
		break;

	case 2:
		sprintf(szBuffer, "%d.%d.%d.%d", (g_Param.subnetMask >> 24) & 0xff
										, (g_Param.subnetMask >> 16) & 0xff
										, (g_Param.subnetMask >> 8) & 0xff
										, (g_Param.subnetMask) &0xff );
		strcpy(data, szBuffer);
		break;

	case 3:
		sprintf(szBuffer, "%d.%d.%d.%d", (g_Param.gatewayAddr >> 24) & 0xff
										, (g_Param.gatewayAddr >> 16) & 0xff
										, (g_Param.gatewayAddr >> 8) & 0xff
										, (g_Param.gatewayAddr) &0xff );
		strcpy(data, szBuffer);
		break;

	default:
		strcpy(data, "");
		*Result = FALSE;
	}

}
//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {

	*Result = FALSE;

}
//-----------------------------------------------------------------------------------

double Analog_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}
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
void String_Name_Out( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
