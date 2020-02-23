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

#define DRIVER_NAME	"EIP_MATCH"
char g_szDrvName[40] = {0,};

#define TIMEOUT_T	1000
#define PollingTime	10

enum { DO, DI, AO, AI, SO, SI };
enum { CS_OFFLINE, CS_ONLINE, CS_CONTROLLER_FAILED };
enum { NONE, MANUAL_LoadSet, MANUAL_TuneSet };
enum { RF_OFF, RF_ON};    //2015.06.11

int g_nPortNum		= 502;
int g_nCommStatus	= CS_ONLINE;
int g_nTimeout		= TIMEOUT_T;
int g_nMatchNumber	= 1;

//////////////////////////////////////////////////////////////////////////
double g_dbLoadPostion = 0;
double g_dbTunePostion = 0;
double g_dbSensorR = 0;
double g_dbSensorX = 0;
double g_dbZ_ScanR = 0, g_dbZ_ScanX = 0, g_dbZ_ScanVoltage = 0, g_dbZ_ScanCurr = 0;
double g_dbZ_ScanPhase = 0, g_dbZ_ScanPower = 0;
unsigned short g_usLoadValue = 0;

double g_dbPresetLoad = 0;
double g_dbPresetTune = 0;

double	g_dbLoadLow = 0;
double	g_dbTuneLow = 0;
double	g_dbLoadUp  = 0;
double	g_dbTuneUp  = 0;

//////////////////////////////////////////////////////////////////////////
int g_nHF_Sel_Preset = 0;
int g_nHF_Ctrl_Mode = 0;
int g_nHF_Preset_Mode = 0;
int g_nHF_IntExternal_Mode = 0;

//////////////////////////////////////////////////////////////////////////
int g_nRetryCnt = 10;
int	g_nRF_DetectSts = 0;
//////////////////////////////////////////////////////////////////////////
#define IO_DATA_FILE_NAME	".\\Parameter\\RFConfig\\EIP_MATCHER.dat"

unsigned char g_ucMatcherType = 1;

char g_pDevice_Address[24] = {0,};

CRITICAL_SECTION	g_csLock;

///////////// Parameter Monitoring ////////////////////////////////////////
Parameters g_Param;
ParameterUpdateStatus g_UpdateSts;
///////////////////////////////////////////////////////////////////////////

#define		TERM_STR		"\n"
//////////////////////////////////////////////////////////////////////////
//...2015.06.11
DWORD  g_dwIOUpdateTime		= 0;
DWORD  g_dwPollIntervalTmAI	= 0;
double g_dbMaxPollTime		= 0;
double g_dbRFGetTime		= 0;
double g_dbRFGetTimeCfg		= 0;
int	   g_nMatcherInProcess	= 0;

double g_dbLoadPostionMax   = 0;
double g_dbLoadPostionMin   = 0;		
double g_dbTunePostionMax   = 0;
double g_dbTunePostionMin   = 0;		
double g_dbZ_ScanRMax       = 0;
double g_dbZ_ScanRMin       = 0;	
double g_dbZ_ScanXMax       = 0;
double g_dbZ_ScanXMin       = 0;	
double g_dbZ_ScanVoltageMax = 0;
double g_dbZ_ScanVoltageMin = 0;		
double g_dbZ_ScanCurrMax    = 0;
double g_dbZ_ScanCurrMin    = 0;	
double g_dbZ_ScanPhaseMax   = 0;
double g_dbZ_ScanPhaseMin   = 0;		
double g_dbZ_ScanPowerMax   = 0;
double g_dbZ_ScanPowerMin   = 0;
double g_dbLoadPostionAvr   = 0;
double g_dbTunePostionAvr   = 0;
double g_dbZ_ScanRAvr	    = 0;
double g_dbZ_ScanXAvr	    = 0;
double g_dbZ_ScanVoltageAvr = 0;
double g_dbZ_ScanCurrAvr	= 0;
double g_dbZ_ScanPhaseAvr   = 0;
double g_dbZ_ScanPowerAvr   = 0;
	
double g_dbLoadPostionSum   = 0;
double g_dbTunePostionSum   = 0;
double g_dbZ_ScanRSum       = 0;
double g_dbZ_ScanXSum       = 0;
double g_dbZ_ScanVoltageSum = 0;
double g_dbZ_ScanCurrSum    = 0; 	
double g_dbZ_ScanPhaseSum   = 0;
double g_dbZ_ScanPowerSum   = 0;
int g_nGetTotalCount = 0;
//////////////////////////////////////////////////////////////////////////
//... 2015.10.11
int		g_nRemoveCycCntCfg = 0;
int		g_nRFCycleCnt	   = 0;
BOOL	g_bIsOkRFOnCycle   = FALSE;
int		g_nGetDataCntDM    = 0;
double  g_dbLoadMaxMinAM   = 0;	
double  g_dbTuneMaxMinAM   = 0;
double  g_dbZRMaxMinAM   = 0;
double  g_dbZXMaxMinAM   = 0;
double  g_dbZVtMaxMinAM  = 0;
double  g_dbZCrMaxMinAM  = 0;
double  g_dbZPhMaxMinAM  = 0;
double  g_dbZPwMaxMinAM  = 0;
BOOL	g_bIsOkRFCntRst	 = FALSE; 

///////////// DCOP Buffer for average calculation /////////////////////////
int		g_nGetFilterCount = 0;
vector<RF_DCOP> g_dcopBuffer;
///////////////////////////////////////////////////////////////////////////
BOOL LoadIOData()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256]  = {0,};
	char szItem[40]  = {0,};
	char szIPAddress[24]  = {0,};
	int nMatcherType;
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
			if     (strcmp(szItem, "EthernetIP1") == 0 && g_nMatchNumber == 1)		{ tpGetWord(szIPAddress, 0);	strcpy(g_pDevice_Address, szIPAddress);	}
			else if(strcmp(szItem, "EthernetIP2") == 0 && g_nMatchNumber == 2)		{ tpGetWord(szIPAddress, 0);	strcpy(g_pDevice_Address, szIPAddress);	}
			else if(strcmp(szItem, "PortNum") == 0)									{ tpGetInt(&nPortNum,0);		g_nPortNum = nPortNum;					}
			else if(strcmp(szItem, "MatcherType") == 0)								{ tpGetInt(&nMatcherType,0);	g_ucMatcherType = (unsigned char)nMatcherType; }
			//...2015.06.11
			else if(strcmp(szItem, "RFGetTime") == 0)								{ tpGetInt(&nRFGetTmTemp,0); g_dbRFGetTimeCfg = (double)nRFGetTmTemp;	}
			else if(strcmp(szItem, "RemoveCycCnt") == 0)							{ tpGetInt(&nRemoveCycCnt,0);	g_nRemoveCycCntCfg = nRemoveCycCnt;}		 //... 2015.10.11
			//...
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
	unsigned char pSndBuf[256]  = {0,};
	unsigned short usByteLenth = 0;
	unsigned long ulData = 0;
	unsigned short usData = 0;
	unsigned short usLoadData = 0;
	unsigned short usTuneData = 0;
	int nByteCount = 0;
	BOOL bRet=TRUE;
	usData = (unsigned short) dbData;
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

	// Data Value
	if(ucCmd == 154)
	{
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType);  // Matcher Type
		pSndBuf[ nByteCount++ ] = 0x00;
	}
	else if(ucCmd == 160)
	{
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType);  // Matcher Type
		pSndBuf[ nByteCount++ ] = 0x00;
		pSndBuf[ nByteCount++ ] = 0x01;
	}
	else if(ucCmd == 92 && ucSubCmd == NONE) //...Preset Load/Tune Position
	{
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType);
		pSndBuf[ nByteCount++ ] = 0x00;
		pSndBuf[ nByteCount++ ] = 0x01;
		pSndBuf[ nByteCount++ ] = 0x00;

		pSndBuf[ nByteCount++ ] = g_usLoadValue & 0x00ff;    
		pSndBuf[ nByteCount++ ] = g_usLoadValue >> 8;

		pSndBuf[ nByteCount++ ] = usData & 0x00ff; 
		pSndBuf[ nByteCount++ ] = usData >> 8;
		g_dbPresetTune = (double)usData;
		g_dbPresetLoad = (double)g_usLoadValue;
	}
	else if(ucCmd == 92 && ucSubCmd == MANUAL_TuneSet)
	{
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType);
		pSndBuf[ nByteCount++ ] = 0x00;
		pSndBuf[ nByteCount++ ] = 0x01;
		pSndBuf[ nByteCount++ ] = 0x00;
		usLoadData = (unsigned short)g_dbPresetLoad;
		pSndBuf[ nByteCount++ ] = usLoadData & 0x00ff;   
		pSndBuf[ nByteCount++ ] = usLoadData >> 8;
		
		pSndBuf[ nByteCount++ ] = usData & 0x00ff; 
		pSndBuf[ nByteCount++ ] = usData >> 8;
		g_dbPresetTune = (double)usData;
		printf("[%s] Manual Tune Set Load[%d] Tune[%d]\n", g_szDrvName, usLoadData, usData);
	}
	else if(ucCmd == 92 && ucSubCmd == MANUAL_LoadSet)
	{
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType); 
		pSndBuf[ nByteCount++ ] = 0x00;
		pSndBuf[ nByteCount++ ] = 0x01;
		pSndBuf[ nByteCount++ ] = 0x00;

		pSndBuf[ nByteCount++ ] = usData & 0x00ff;  
		pSndBuf[ nByteCount++ ] = usData >> 8;
		usTuneData = (unsigned short)g_dbPresetTune;
		pSndBuf[ nByteCount++ ] = usTuneData & 0x00ff;       
		pSndBuf[ nByteCount++ ] = usTuneData >> 8;
		g_dbPresetLoad = (double)usData;

		printf("[%s] Manual Load Set Load[%d] Tune[%d]\n", g_szDrvName, usData, usTuneData);
	}
	else if(ucCmd == 112 || ucCmd == 122)     //...Manual Load / Tune Load
	{
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType); 
		pSndBuf[ nByteCount++ ] = 0x00;

		pSndBuf[ nByteCount++ ] = usData & 0x00ff;
		pSndBuf[ nByteCount++ ] = usData >> 8;
	}

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
	unsigned char pSndBuf[256]  = {0,};
	unsigned short usByteLenth = 0;
	unsigned short usDataTemp = 0;

	int nByteCount = 0;
	BOOL bRet=TRUE;

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

	if(ucCmd == 91 || ucCmd == 93 || ucCmd == 94 || ucCmd == 97)
	{	
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType); 
		pSndBuf[ nByteCount++ ] = 0x00;
		//Value
		pSndBuf[ nByteCount++ ] = usData & 0x00ff;
		pSndBuf[ nByteCount++ ] = usData >> 8; 
	}
	else if(ucCmd == 161 || ucCmd == 163 || ucCmd == 164 || ucCmd == 167)
	{	
		pSndBuf[ nByteCount++ ] = (BYTE)(g_ucMatcherType); 
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
//...Digital Input Data
void Get_Sel_PresetData(unsigned char *pRcvBuf)
{
	unsigned short usSelectData = 0;
	usSelectData = pRcvBuf[15];
	usSelectData = usSelectData << 8;
	usSelectData = usSelectData + (pRcvBuf[14] & 0xff);	
	g_nHF_Sel_Preset = usSelectData;
}
void Get_Ctrl_ModeData(unsigned char *pRcvBuf)
{
	unsigned short usCtrlModeData = 0;
	usCtrlModeData = pRcvBuf[15];
	usCtrlModeData = usCtrlModeData << 8;
	usCtrlModeData = usCtrlModeData + (pRcvBuf[14] & 0xff);	
	g_nHF_Ctrl_Mode = usCtrlModeData;
}
void Get_Preset_ModeData(unsigned char *pRcvBuf)
{
	unsigned short usPresetModeData = 0;
	usPresetModeData = pRcvBuf[15];
	usPresetModeData = usPresetModeData << 8;
	usPresetModeData = usPresetModeData + (pRcvBuf[14] & 0xff);	
	g_nHF_Preset_Mode = usPresetModeData;
}
void Get_IntExternal_ModeData(unsigned char *pRcvBuf)
{
	unsigned short usIntExterModeData = 0;
	usIntExterModeData = pRcvBuf[15];
	usIntExterModeData = usIntExterModeData << 8;
	usIntExterModeData = usIntExterModeData + (pRcvBuf[14] & 0xff);
	g_nHF_IntExternal_Mode = usIntExterModeData;
}
//////////////////////////////////////////////////////////////////////////
void HEX_DI_Conversion( double *result , unsigned char HEX1 , unsigned char HEX2 , unsigned char HEX3 , unsigned char HEX4 )
{  // HEX->Digital Conversion.........................................................
	
	unsigned int  c[8],d[8],e[8],j,HEX01;
	double		  both,HEX10,HEX20,HEX30,HEX40;
	int			  i;
	  
	unsigned char HEX02 = 0,HEX03 = 0,HEX04 = 0,tmpHex1 = 0;
	double        sum,sum1,sum2,sum3;
	double        k[26];


	tmpHex1	=   HEX1 	<<	1;
	HEX01 = (unsigned int) tmpHex1;


	HEX02	=	HEX2;
	HEX03	=	HEX3;
	HEX04	=	HEX4;
	

	HEX20	=	( double )HEX02;
	for(i=7;i>-1;i--)
	{
		both = pow(2,i);
		if( HEX20 > both || HEX20 == both )
		{
			HEX20 = HEX20 - both;
			c[i] = 1;
		}
		else
		{
			c[i] = 0;			
		}
	}

	HEX30	=	( double )HEX03;
	for(i=7;i>-1;i--)
	{
		both = pow(2,i);
		if( HEX30 > both || HEX30 == both )
		{
			HEX30 = HEX30 - both;
			d[i] = 1;		
		}
		else
		{
			d[i] = 0;
		}
	}

	HEX40	=	( double )HEX04;
	for(i=7;i>-1;i--)
	{
		both = pow(2,i);
		if( HEX40 > both || HEX40 == both )
		{
			HEX40 = HEX40 - both;
			e[i] = 1;
		}
		else
		{
			e[i] = 0;			
		}
	}


	for(j=1;j<25;j++)
	{	
		k[j] = ( 1 / pow( 2 , j ));	
	}

	sum1=( c[6]*k[1] )+( c[5]*k[2] )+( c[4]*k[3] )+( c[3]*k[4] )+( c[2]*k[5] )+( c[1]*k[6] )+( c[0]*k[7] );
	sum2=( d[7]*k[8] )+( d[6]*k[9])+( d[5]*k[10])+( d[4]*k[11])+( d[3]*k[12])+( d[2]*k[13])+( d[1]*k[14])+( d[0]*k[15]);
	sum3=( e[7]*k[16])+( e[6]*k[17])+( e[5]*k[18])+( e[4]*k[19])+( e[3]*k[20])+( e[2]*k[21])+( e[1]*k[22])+( e[0]*k[23]);	

	sum = sum1 + sum2 + sum3 ;


	if( HEX01 > 127 || HEX01 == 127)
	{
		HEX01 = HEX01 - 127;

		HEX10 = pow( 2 , HEX01 );

		if( HEX2 > 128 || HEX2 == 128 )
		{
			sum = ( sum + 1 ) * 2 ;
		}
		else
		{
			sum =  sum + 1  ;
		}
	}
	else
	{
		HEX01 =  127 - HEX01  ;

		HEX10 = 1 / ( pow( 2 , HEX01 ));

		if( HEX2 > 128 || HEX2 == 128 )
		{
			sum = ( sum + 1 ) * 2 ;
		}
		else
		{
			sum =  sum + 1  ;
		}
	}

	*result = ( HEX10 ) * ( sum );

	if ( HEX1 >= 128 )
	{
		*result = *result * -1;	
	}

}
//////////////////////////////////////////////////////////////////////////

void Get_Capacitor_LimitData(unsigned char *pRcvBuf)
{
	unsigned short usLoadLow = 0;
	unsigned short usTuneLow = 0;
	unsigned short usLoadUp = 0;
	unsigned short usTuneUp = 0;

	//... Min load
	usLoadLow = pRcvBuf[15];
	usLoadLow = usLoadLow << 8;
	usLoadLow = usLoadLow + (pRcvBuf[14] & 0xff);
	//... Min Tune
	usTuneLow = pRcvBuf[17];
	usTuneLow = usTuneLow << 8;
	usTuneLow = usTuneLow + (pRcvBuf[16] & 0xff);
	//... Max Load
	usLoadUp = pRcvBuf[19];
	usLoadUp = usLoadUp << 8;
	usLoadUp = usLoadUp + (pRcvBuf[18] & 0xff);
	//... Max Tune
	usTuneUp = pRcvBuf[21];
	usTuneUp = usTuneUp << 8;
	usTuneUp = usTuneUp + (pRcvBuf[20] & 0xff);

	g_dbLoadLow = (double)usLoadLow/100;
	g_dbTuneLow = (double)usTuneLow/100;
	g_dbLoadUp  = (double)usLoadUp/100;
	g_dbTuneUp  = (double)usTuneUp/100;
}
//////////////////////////////////////////////////////////////////////////
//...2015.06.10
void Analyze_RFData()
{
	if(g_dbLoadPostion > g_dbLoadPostionMax)									g_dbLoadPostionMax = g_dbLoadPostion;
	if(g_dbLoadPostion < g_dbLoadPostionMin || g_dbLoadPostionMin == 0)			g_dbLoadPostionMin = g_dbLoadPostion;
	
	if(g_dbTunePostion > g_dbTunePostionMax)									g_dbTunePostionMax = g_dbTunePostion;
	if(g_dbTunePostion < g_dbTunePostionMin || g_dbTunePostionMin == 0)			g_dbTunePostionMin = g_dbTunePostion;	
	
	if(g_dbZ_ScanR > g_dbZ_ScanRMax)											g_dbZ_ScanRMax = g_dbZ_ScanR;
	if(g_dbZ_ScanR < g_dbZ_ScanRMin || g_dbZ_ScanRMin == 0)						g_dbZ_ScanRMin = g_dbZ_ScanR;	

	if(g_dbZ_ScanX > g_dbZ_ScanXMax)											g_dbZ_ScanXMax = g_dbZ_ScanX;
	if(g_dbZ_ScanX < g_dbZ_ScanXMin || g_dbZ_ScanXMin == 0)						g_dbZ_ScanXMin = g_dbZ_ScanX;
	
	if(g_dbZ_ScanVoltage > g_dbZ_ScanVoltageMax)								g_dbZ_ScanVoltageMax = g_dbZ_ScanVoltage;
	if(g_dbZ_ScanVoltage < g_dbZ_ScanVoltageMin || g_dbZ_ScanVoltageMin == 0)   g_dbZ_ScanVoltageMin = g_dbZ_ScanVoltage;	
	
	if(g_dbZ_ScanCurr > g_dbZ_ScanCurrMax)										g_dbZ_ScanCurrMax = g_dbZ_ScanCurr;
	if(g_dbZ_ScanCurr < g_dbZ_ScanCurrMin || g_dbZ_ScanCurrMin == 0)			g_dbZ_ScanCurrMin = g_dbZ_ScanCurr;	

	if(g_dbZ_ScanPhase > g_dbZ_ScanPhaseMax)									g_dbZ_ScanPhaseMax = g_dbZ_ScanPhase;
	if(g_dbZ_ScanPhase < g_dbZ_ScanPhaseMin || g_dbZ_ScanPhaseMin == 0)			g_dbZ_ScanPhaseMin = g_dbZ_ScanPhase;	
	
	if(g_dbZ_ScanPower > g_dbZ_ScanPowerMax)									g_dbZ_ScanPowerMax = g_dbZ_ScanPower;
	if(g_dbZ_ScanPower < g_dbZ_ScanPowerMin || g_dbZ_ScanPowerMin == 0)			g_dbZ_ScanPowerMin = g_dbZ_ScanPower;	

	if(g_dbZ_ScanVoltageSum < 99999999)  
	{
		RF_DCOP dcopData;
		dcopData.dbLoadPostion		= g_dbLoadPostion;
		dcopData.dbTunePostion		= g_dbTunePostion; 
		dcopData.dbZ_ScanR			= g_dbZ_ScanR;
		dcopData.dbZ_ScanX			= g_dbZ_ScanX;	
		dcopData.dbZ_ScanVoltage	= g_dbZ_ScanVoltage;
		dcopData.dbZ_ScanCurr	    = g_dbZ_ScanCurr; 	
		dcopData.dbZ_ScanPhase		= g_dbZ_ScanPhase;
		dcopData.dbZ_ScanPower		= g_dbZ_ScanPower;

		g_dcopBuffer.push_back(dcopData);
	
	}
	//... 2015.10.11
	if( 0 < (g_dbLoadPostionMax - g_dbLoadPostionMin)
		&& g_dbLoadPostionMax > 0
		&& g_dbLoadPostionMin > 0)
	{
		g_dbLoadMaxMinAM = (g_dbLoadPostionMax - g_dbLoadPostionMin);
	}
	
	if( 0 < (g_dbTunePostionMax - g_dbTunePostionMin)
		&& g_dbTunePostionMax > 0
		&& g_dbTunePostionMin > 0) 
	{
		g_dbTuneMaxMinAM = (g_dbTunePostionMax - g_dbTunePostionMin);
	}
	
	if( 0 < (g_dbZ_ScanRMax - g_dbZ_ScanRMin)
		&& g_dbZ_ScanRMax > 0
		&& g_dbZ_ScanRMin > 0) 
	{
		g_dbZRMaxMinAM = (g_dbZ_ScanRMax - g_dbZ_ScanRMin);
	}


	if( 0 < (g_dbZ_ScanXMax - g_dbZ_ScanXMin)
		&& g_dbZ_ScanXMax > 0
		&& g_dbZ_ScanXMin > 0)
	{
		g_dbZXMaxMinAM = (g_dbZ_ScanXMax - g_dbZ_ScanXMin);
	}
	
	if( 0 < (g_dbZ_ScanVoltageMax - g_dbZ_ScanVoltageMin)
		&& g_dbZ_ScanVoltageMax > 0
		&& g_dbZ_ScanVoltageMin > 0) 
	{
		g_dbZVtMaxMinAM = (g_dbZ_ScanVoltageMax - g_dbZ_ScanVoltageMin);
	}
	
	if( 0 < (g_dbZ_ScanCurrMax - g_dbZ_ScanCurrMin)
		&& g_dbZ_ScanCurrMax > 0
		&& g_dbZ_ScanCurrMin > 0) 
	{
		g_dbZCrMaxMinAM = (g_dbZ_ScanCurrMax - g_dbZ_ScanCurrMin);
	}

	if( 0 < (g_dbZ_ScanPhaseMax - g_dbZ_ScanPhaseMin)
		&& g_dbZ_ScanPhaseMax > 0
		&& g_dbZ_ScanPhaseMin > 0) 
	{
		g_dbZPhMaxMinAM = (g_dbZ_ScanPhaseMax - g_dbZ_ScanPhaseMin);
	}
	
	if( 0 < (g_dbZ_ScanPowerMax - g_dbZ_ScanPowerMin)
		&& g_dbZ_ScanPowerMax > 0
		&& g_dbZ_ScanPowerMin > 0) 
	{
		g_dbZPwMaxMinAM = (g_dbZ_ScanPowerMax - g_dbZ_ScanPowerMin);
	}
	//...

}
void InitSummaryData()
{
	g_dbLoadPostionAvr   = 0;
	g_dbTunePostionAvr   = 0;
	g_dbZ_ScanRAvr	     = 0;
	g_dbZ_ScanXAvr	     = 0;
	g_dbZ_ScanVoltageAvr = 0;
	g_dbZ_ScanCurrAvr	 = 0;
	g_dbZ_ScanPhaseAvr   = 0;
	g_dbZ_ScanPowerAvr   = 0;

	g_dbLoadPostionMin   = 0;
	g_dbTunePostionMin   = 0;
	g_dbZ_ScanRMin	     = 0;
	g_dbZ_ScanXMin	     = 0;
	g_dbZ_ScanVoltageMin = 0;
	g_dbZ_ScanCurrMin	 = 0;
	g_dbZ_ScanPhaseMin   = 0;
	g_dbZ_ScanPowerMin   = 0;

	g_dbLoadPostionMax   = 0;
	g_dbTunePostionMax   = 0;
	g_dbZ_ScanRMax	     = 0;
	g_dbZ_ScanXMax	     = 0;
	g_dbZ_ScanVoltageMax = 0;
	g_dbZ_ScanCurrMax	 = 0;
	g_dbZ_ScanPhaseMax   = 0;
	g_dbZ_ScanPowerMax   = 0;

	g_dbLoadPostionSum   = 0;
	g_dbTunePostionSum   = 0; 
	g_dbZ_ScanRSum		 = 0;
	g_dbZ_ScanXSum		 = 0;	
	g_dbZ_ScanVoltageSum = 0;
	g_dbZ_ScanCurrSum	 = 0; 	
	g_dbZ_ScanPhaseSum	 = 0;
	g_dbZ_ScanPowerSum	 = 0;

	g_nGetTotalCount	 = 0;
	//... 2015.10.11
	g_dbLoadMaxMinAM	 = 0;	
	g_dbTuneMaxMinAM     = 0;
	g_dbZRMaxMinAM		 = 0;
	g_dbZXMaxMinAM		 = 0;
	g_dbZVtMaxMinAM		 = 0;
	g_dbZCrMaxMinAM		 = 0;
	g_dbZPhMaxMinAM		 = 0;
	g_dbZPwMaxMinAM		 = 0;
	//...
}
//////////////////////////////////////////////////////////////////////////
void Get_SNAP_DATA(unsigned char *pRcvBuf)
{
	unsigned short usLoadPostion = 0;
	unsigned short usTunePostion = 0;
	unsigned short usSensorR = 0;
	unsigned short usSensorX = 0;
	unsigned long ulZ_ScanR = 0,ulZ_ScanX = 0,ulZ_ScanVoltage = 0,ulZ_ScanCurr = 0,ulZ_ScanPhase = 0,ulZ_ScanPower = 0;

	unsigned long ulZ_ScanRTemp = 0,		ulZ_ScanRTemp2 = 0;
	unsigned long ulZ_ScanXTemp = 0,		ulZ_ScanXTemp2 = 0;
	unsigned long ulZ_ScanVoltageTemp = 0,	ulZ_ScanVoltageTemp2 = 0;
	unsigned long ulZ_ScanCurrTemp = 0,		ulZ_ScanCurrTemp2 = 0;
	unsigned long ulZ_ScanPhaseTemp = 0,	ulZ_ScanPhaseTemp2 = 0;
	unsigned long ulZ_ScanPowerTemp = 0,	ulZ_ScanPowerTemp2 = 0;
	//... 2015.06.11
	unsigned short usRFOnSts = 0; 
	usRFOnSts = (BYTE)pRcvBuf[12] & 0x01;
	if(usRFOnSts == 0x01) {g_nRF_DetectSts = RF_ON;}
	else				  {g_nRF_DetectSts = RF_OFF; g_dbRFGetTime = 0;}
	//...
	//... Load position
	usLoadPostion = pRcvBuf[17];
	usLoadPostion = usLoadPostion << 8;
	usLoadPostion = usLoadPostion + (pRcvBuf[16] & 0xff);
	//... Tune position
	usTunePostion = pRcvBuf[19];
	usTunePostion = usTunePostion << 8;
	usTunePostion = usTunePostion + (pRcvBuf[18] & 0xff);
	//... Sensor R
	usSensorR = pRcvBuf[21];
	usSensorR = usSensorR << 8;
	usSensorR = usSensorR + (pRcvBuf[20] & 0xff);
	//... Sensor X
	usSensorX = pRcvBuf[23];
	usSensorX = usSensorX << 8;
	usSensorX = usSensorX + (pRcvBuf[22] & 0xff);
	/*
	//... Z Scan R
	ulZ_ScanR = pRcvBuf[56];
	ulZ_ScanR = ulZ_ScanR << 24;
	ulZ_ScanRTemp = pRcvBuf[55];
	ulZ_ScanRTemp = ulZ_ScanRTemp << 16;
	ulZ_ScanRTemp2 = pRcvBuf[54];
	ulZ_ScanRTemp2 = ulZ_ScanRTemp2 << 8;
	ulZ_ScanR = ulZ_ScanR + ulZ_ScanRTemp + ulZ_ScanRTemp2 + (pRcvBuf[53] & 0xff);
	//... Z Scan X
	ulZ_ScanX = pRcvBuf[60];
	ulZ_ScanX = ulZ_ScanX << 24;
	ulZ_ScanXTemp = pRcvBuf[59];
	ulZ_ScanXTemp = ulZ_ScanXTemp << 16;
	ulZ_ScanXTemp2 = pRcvBuf[58];
	ulZ_ScanXTemp2 = ulZ_ScanXTemp2 << 8;
	ulZ_ScanX = ulZ_ScanX + ulZ_ScanXTemp + ulZ_ScanXTemp2 + (pRcvBuf[57] & 0xff);
	//... Z Scan Voltage
	ulZ_ScanVoltage = pRcvBuf[64];
	ulZ_ScanVoltage = ulZ_ScanVoltage << 24;
	ulZ_ScanVoltageTemp = pRcvBuf[63];
	ulZ_ScanVoltageTemp = ulZ_ScanVoltageTemp << 16;
	ulZ_ScanVoltageTemp2 = pRcvBuf[62];
	ulZ_ScanVoltageTemp2 = ulZ_ScanVoltageTemp2 << 8;
	ulZ_ScanVoltage = ulZ_ScanVoltage + ulZ_ScanVoltageTemp + ulZ_ScanVoltageTemp2 + (pRcvBuf[61] & 0xff);
	//... Z Scan Current
	ulZ_ScanCurr = pRcvBuf[68];
	ulZ_ScanCurr = ulZ_ScanCurr << 24;
	ulZ_ScanCurrTemp = pRcvBuf[67];
	ulZ_ScanCurrTemp = ulZ_ScanCurrTemp << 16;
	ulZ_ScanCurrTemp2 = pRcvBuf[66];
	ulZ_ScanCurrTemp2 = ulZ_ScanCurrTemp2 << 8;
	ulZ_ScanCurr = ulZ_ScanCurr + ulZ_ScanVoltageTemp + ulZ_ScanVoltageTemp2 + (pRcvBuf[65] & 0xff);
	//... Z Scan Phase
	ulZ_ScanPhase = pRcvBuf[72];
	ulZ_ScanPhase = ulZ_ScanPhase << 24;
	ulZ_ScanPhaseTemp = pRcvBuf[71];
	ulZ_ScanPhaseTemp = ulZ_ScanPhaseTemp << 16;
	ulZ_ScanPhaseTemp2 = pRcvBuf[70];
	ulZ_ScanPhaseTemp2 = ulZ_ScanPhaseTemp2 << 8;
	ulZ_ScanPhase = ulZ_ScanPhase + ulZ_ScanPhaseTemp + ulZ_ScanPhaseTemp2 + (pRcvBuf[69] & 0xff);
	//... Z Scan Power
	ulZ_ScanPower = pRcvBuf[76];
	ulZ_ScanPower = ulZ_ScanPower << 24;
	ulZ_ScanPowerTemp = pRcvBuf[75];
	ulZ_ScanPowerTemp = ulZ_ScanPowerTemp << 16;
	ulZ_ScanPowerTemp2 = pRcvBuf[74];
	ulZ_ScanPowerTemp2 = ulZ_ScanPowerTemp2 << 8;
	ulZ_ScanPower = ulZ_ScanPower + ulZ_ScanPowerTemp + ulZ_ScanPowerTemp2 + (pRcvBuf[73] & 0xff);
	*/
	//... Data Conversion
	g_dbLoadPostion   = (double)usLoadPostion/100;
	g_dbTunePostion   = (double)usTunePostion/100;
	g_dbSensorR		  = (double)(usSensorR*50)/1024;
	g_dbSensorX		  = (double)(usSensorX*50)/1024;
	HEX_DI_Conversion( &g_dbZ_ScanR,		pRcvBuf[55], pRcvBuf[54] , pRcvBuf[53], pRcvBuf[52]);
	HEX_DI_Conversion( &g_dbZ_ScanX,		pRcvBuf[59], pRcvBuf[58] , pRcvBuf[57], pRcvBuf[56]);
	HEX_DI_Conversion( &g_dbZ_ScanVoltage,  pRcvBuf[63], pRcvBuf[62] , pRcvBuf[61], pRcvBuf[60]);
	HEX_DI_Conversion( &g_dbZ_ScanCurr,		pRcvBuf[67], pRcvBuf[66] , pRcvBuf[65], pRcvBuf[64]);
	HEX_DI_Conversion( &g_dbZ_ScanPhase,	pRcvBuf[71], pRcvBuf[70] , pRcvBuf[69], pRcvBuf[68]);
	HEX_DI_Conversion( &g_dbZ_ScanPower,	pRcvBuf[75], pRcvBuf[74] , pRcvBuf[73], pRcvBuf[72]);
	//printf("[Load/Tune] %d %f %d %f \n",usLoadPostion,g_dbLoadPostion,usTunePostion,g_dbTunePostion);
	/*
	g_dbZ_ScanR		  = (double)ulZ_ScanR; 
	g_dbZ_ScanX		  = (double)ulZ_ScanX; 
	g_dbZ_ScanVoltage = (double)ulZ_ScanVoltage;
	g_dbZ_ScanCurr	  = (double)ulZ_ScanCurr;
	g_dbZ_ScanPhase	  = (double)ulZ_ScanPhase;
	g_dbZ_ScanPower   = (double)ulZ_ScanPower;
	*/

	//... 2015.10.11
	if(g_nRF_DetectSts == RF_ON)
	{
		g_bIsOkRFOnCycle = TRUE;
	}
	else
	{
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
		&& g_nRFCycleCnt >= g_nRemoveCycCntCfg		//... 2015.10.11 Compare Real RF On Cycle & Cfg Data 
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
	else
	{
		//... When RF Off, Calculate average of dcop data.
		CalcDCOPAverage();
	}
	//////////////////////////////////////////////////////////////////////////

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
		g_dbLoadPostionSum	+= g_dcopBuffer[i].dbLoadPostion;
		g_dbTunePostionSum	+= g_dcopBuffer[i].dbTunePostion;
		g_dbZ_ScanRSum		+= g_dcopBuffer[i].dbZ_ScanR;
		g_dbZ_ScanXSum		+= g_dcopBuffer[i].dbZ_ScanX;	
		g_dbZ_ScanVoltageSum+= g_dcopBuffer[i].dbZ_ScanVoltage;
		g_dbZ_ScanCurrSum	+= g_dcopBuffer[i].dbZ_ScanCurr;	
		g_dbZ_ScanPhaseSum	+= g_dcopBuffer[i].dbZ_ScanPhase;
		g_dbZ_ScanPowerSum	+= g_dcopBuffer[i].dbZ_ScanPower;

	}
	g_nGetTotalCount += stackedSize;
	
	// 평균 산출
	if( g_nGetTotalCount > 0 )
	{ 
		if(0 < g_dbLoadPostionSum )   g_dbLoadPostionAvr   = g_dbLoadPostionSum  /g_nGetTotalCount;
		if(0 < g_dbTunePostionSum )   g_dbTunePostionAvr   = g_dbTunePostionSum  /g_nGetTotalCount;
		if(0 < g_dbZ_ScanRSum )       g_dbZ_ScanRAvr	   = g_dbZ_ScanRSum      /g_nGetTotalCount;
		if(0 < g_dbZ_ScanXSum )       g_dbZ_ScanXAvr	   = g_dbZ_ScanXSum      /g_nGetTotalCount;
		if(0 < g_dbZ_ScanVoltageSum ) g_dbZ_ScanVoltageAvr = g_dbZ_ScanVoltageSum/g_nGetTotalCount;
		if(0 < g_dbZ_ScanCurrSum )    g_dbZ_ScanCurrAvr	   = g_dbZ_ScanCurrSum   /g_nGetTotalCount;
		if(0 < g_dbZ_ScanPhaseSum )   g_dbZ_ScanPhaseAvr   = g_dbZ_ScanPhaseSum  /g_nGetTotalCount;
		if(0 < g_dbZ_ScanPowerSum )   g_dbZ_ScanPowerAvr   = g_dbZ_ScanPowerSum  /g_nGetTotalCount;
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
	pSndBufTemp[ nByteCountTemp++ ] = (usByteLength >> 8) & 0xff;
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


void UpdateParameters() 
{
	BYTE sendData[80];
	BYTE recvData[256];
	int nDataLength;

	int tempValue;

	g_UpdateSts = Param_Updaing;

	memset(sendData, 0, sizeof(sendData));
	memset(recvData, 0, sizeof(recvData));
	
	sendData[0] = g_ucMatcherType;  // Matcher type
	sendData[1] = 0x00;				// Current Values	
	sendData[3] = 0x03;				// For NavII
	sendData[4] = 0x00;
	if( RequestCommand(recvData, &nDataLength, 152, sendData, 4) && nDataLength >= 20)
	{		
		tempValue = recvData[4] | (recvData[5] << 8) | (recvData[6] << 16) | (recvData[7] << 24);
		memcpy(&g_Param.stopVSWR, &tempValue, 4);

		tempValue = recvData[8] | (recvData[9] << 8) | (recvData[10] << 16) | (recvData[11] << 24);
		memcpy(&g_Param.startVSWR, &tempValue, 4);

		g_Param.maxStepLoad = (recvData[12] | (recvData[13] << 8)) / 100;
		g_Param.maxStepTune = (recvData[14] | (recvData[15] << 8)) / 100;
		g_Param.targetPosLoad = (recvData[16] | (recvData[17] << 8)) / 100;
		g_Param.targetPosTune = (recvData[18] | (recvData[19] << 8)) / 100;
	}

	sendData[0] = g_ucMatcherType;  // Matcher type
	sendData[1] = 0x00;				// Current Values
	if( RequestCommand(recvData, &nDataLength, 148, sendData, 2) && nDataLength >= 6)
	{
		g_Param.targetZR = recvData[2] | (recvData[3] << 8);
		g_Param.targetZX = recvData[4] | (recvData[5] << 8);
	}

	sendData[0] = 209;
	sendData[1] = 0;
	sendData[2] = 1;
	sendData[3] = 0;
	if( RequestCommand(recvData, &nDataLength, 248, sendData, 4) && nDataLength >= 8)
	{
		g_Param.V_ThreadoldCount = recvData[0] | (recvData[1] << 8);
		g_Param.I_ThreadoldCount = recvData[2] | (recvData[3] << 8);
		g_Param.priorTime = recvData[4] | (recvData[5] << 8);
		g_Param.future = recvData[6] | (recvData[7] << 8);
	}

	g_UpdateSts = Param_Ready;
}

BOOL Net_Port_Write(unsigned short usDataSize, unsigned char ucCmd, unsigned char ucSubCmd)
{
	unsigned char pSndBuf[256] = {0,};
	unsigned short usByteLenth = 0;

	int nByteCount = 0;
	BOOL bRet=TRUE;
	
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
	pSndBuf[ nByteCount++ ] = 0x00;
	pSndBuf[ nByteCount++ ] = 0x00;

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
 		if(! Net_Port_Write(2,(unsigned char)219,(unsigned char)0))		break;
		if(! Net_Port_Rread((unsigned char)219, pRcvBuf, 100))			break;
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
	//...2015.06.11	
	DWORD dwCurrentTime = 0;
	DWORD dwElapsedTime = 0;
	double dbRFGetTimeTemp = 0;
	//...	
	BOOL  bNetConOK	= FALSE;
	BOOL  bUpdateOK	= FALSE;
	g_bThreadRun = TRUE;
	g_bThreadFuncRun = TRUE;

	Sleep(1000);

	while(g_bThreadRun)
	{
		//////////////////////////////////////////////////////////////////////////
		//...2015.06.11
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
	unsigned char pRcvBuf[256] = {0,};
	unsigned char ucSubCmd = 0;
	unsigned char ucCmd = 0;
	unsigned short usDataSize;
	int nReadDataSize;
	*Result = TRUE;
	ucCmd = ID1;

	switch(ID1)
	{
	case 20:	return g_Param.maxStepLoad;
	case 21:	return g_Param.maxStepTune;
	case 22:	return g_Param.targetPosLoad;
	case 23:	return g_Param.targetPosTune;
	case 24:	return g_Param.targetZR;
	case 25:	return g_Param.targetZX;
	case 26:	return g_Param.V_ThreadoldCount;
	case 27:	return g_Param.I_ThreadoldCount;
	case 28:	return g_Param.priorTime;
	case 29:	return g_Param.future;	
	case 40:	return g_UpdateSts;

	case 10:    return g_nGetDataCntDM; //... 2015.10.11
	case 11:    return g_nGetTotalCount;	
	case 99:	return g_nCommStatus;
	case 161:
		usDataSize = 2;
		ucSubCmd = NONE;
		nReadDataSize = 16;
		break;
	case 163:
		usDataSize = 2;
		ucSubCmd = NONE;
		nReadDataSize = 16;
		break;
	case 164:
		usDataSize = 2;
		ucSubCmd = NONE;
		nReadDataSize = 16;
		break;
	case 167:
		usDataSize = 2;
		ucSubCmd = NONE;
		nReadDataSize = 16;
		break;
	default:
		PrintError(DI, ID1, ID2, ID3, ID4);
		*Result = FALSE;
		return 0;
	}

	EnterCriticalSection(&g_csLock);
	if(! Digital_Set_CMD(usDataSize, ucCmd, ucSubCmd, (unsigned short)0))	*Result = FALSE;
	if(! IO_Read_CMD(ucCmd, pRcvBuf, nReadDataSize))						*Result = FALSE;
	LeaveCriticalSection(&g_csLock);
	
	switch(ID1)
	{
	case 161:
		Get_Sel_PresetData(pRcvBuf);
		return g_nHF_Sel_Preset;
		break;
	case 163:
		Get_Ctrl_ModeData(pRcvBuf);
		return g_nHF_Ctrl_Mode;
		break;
	case 164:
		Get_Preset_ModeData(pRcvBuf);
		return g_nHF_Preset_Mode;
		break;
	case 167:
		Get_IntExternal_ModeData(pRcvBuf);
		return g_nHF_IntExternal_Mode;
		break;
	default:
		PrintError(DI, ID1, ID2, ID3, ID4);
		*Result = FALSE;
		return 0;
	}


	if(FALSE == *Result)	PrintError(DO, ID1, ID2, ID3, ID4);
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
	ucCmd = ID1;

	switch(ID1)
	{
	case 4:
		if( Data == 1 && Net_Connect_Status() ) 
		{
			UpdateParameters();			
		}
		return;
		break;
	case 10:
		InitSummaryData();
		g_nRFCycleCnt = 0;     //... 2015.10.11 Reset RF Cycle Count
		g_nGetDataCntDM = 0;   //... Reset RF On Count
		return;
	case 12:
		g_nGetFilterCount = Data;
		return; 
	case 91:
		ucSubCmd = NONE;
		usDataSize = 4;
		break;
	case 93:
		ucSubCmd = NONE;
		usDataSize = 4;
		break;
	case 94:
		ucSubCmd = NONE;
		usDataSize = 4;
		break;
	case 97:
		ucSubCmd = NONE;
		usDataSize = 4;
		break;
	default:
		PrintError(DO, ID1, ID2, ID3, ID4);
		*Result = FALSE;
		return;
	}

	EnterCriticalSection(&g_csLock);
	if(! Digital_Set_CMD(usDataSize, ucCmd, ucSubCmd, (unsigned short)Data))	*Result = FALSE;
	if(!IO_Read_CMD((unsigned char)ucCmd, pRcvBuf, 13))							*Result = FALSE;
	LeaveCriticalSection(&g_csLock);
	
	if(FALSE == *Result)	PrintError(DO, ID1, ID2, ID3, ID4);
}

//////////////////////////////////////////////////////////////////////////
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result )
{
	unsigned char pRcvBuf[256] = {0,};
	*Result = FALSE;
	switch(ID1)
	{
	case 1:	*Result = TRUE;		return g_dbLoadPostion;		
	case 2:	*Result = TRUE;		return g_dbTunePostion;		
	case 3:	*Result = TRUE;		return g_dbSensorR;		
	case 4:	*Result = TRUE;		return g_dbSensorX;
	case 5:	*Result = TRUE;		return g_dbZ_ScanPhase;	
	case 6: *Result = TRUE;		return g_dbZ_ScanPower;		
	case 7:	*Result = TRUE;		return g_dbZ_ScanR;		
	case 8:	*Result = TRUE;		return g_dbZ_ScanX;	
	case 9:	*Result = TRUE;		return g_dbZ_ScanVoltage;
	case 10:*Result = TRUE;		return g_dbZ_ScanCurr;	
    //...2015.06.12
	case 11:*Result = TRUE;		return g_dbLoadPostionMax;
	case 12:*Result = TRUE;		return g_dbLoadPostionMin;		
	case 13:*Result = TRUE;		return g_dbTunePostionMax;
	case 14:*Result = TRUE;		return g_dbTunePostionMin;		
	case 15:*Result = TRUE;		return g_dbZ_ScanRMax;
	case 16:*Result = TRUE;		return g_dbZ_ScanRMin;	
	case 17:*Result = TRUE;		return g_dbZ_ScanXMax;
	case 18:*Result = TRUE;		return g_dbZ_ScanXMin;	
	case 19:*Result = TRUE;		return g_dbZ_ScanVoltageMax;
	case 20:*Result = TRUE;		return g_dbZ_ScanVoltageMin;		
	case 21:*Result = TRUE;		return g_dbZ_ScanCurrMax;
	case 22:*Result = TRUE;		return g_dbZ_ScanCurrMin;	
	case 23:*Result = TRUE;		return g_dbZ_ScanPhaseMax;
	case 24:*Result = TRUE;		return g_dbZ_ScanPhaseMin;		
	case 25:*Result = TRUE;		return g_dbZ_ScanPowerMax;
	case 26:*Result = TRUE;		return g_dbZ_ScanPowerMin;
	case 27:*Result = TRUE;		return g_dbLoadPostionAvr;
	case 28:*Result = TRUE;		return g_dbTunePostionAvr;
	case 29:*Result = TRUE;		return g_dbZ_ScanRAvr;
	case 30:*Result = TRUE;		return g_dbZ_ScanXAvr;
	case 31:*Result = TRUE;		return g_dbZ_ScanVoltageAvr;
	case 32:*Result = TRUE;		return g_dbZ_ScanCurrAvr;
	case 33:*Result = TRUE;		return g_dbZ_ScanPhaseAvr;
	case 34:*Result = TRUE;		return g_dbZ_ScanPowerAvr;
	//...
    //...2015.10.11
	case 35:*Result = TRUE;		return g_dbLoadMaxMinAM;
	case 36:*Result = TRUE;		return g_dbTuneMaxMinAM;
	case 37:*Result = TRUE;		return g_dbZRMaxMinAM;
	case 38:*Result = TRUE;		return g_dbZXMaxMinAM;
	case 39:*Result = TRUE;		return g_dbZVtMaxMinAM;
	case 40:*Result = TRUE;		return g_dbZCrMaxMinAM;
	case 41:*Result = TRUE;		return g_dbZPhMaxMinAM;
	case 42:*Result = TRUE;		return g_dbZPwMaxMinAM;

	case 43:*Result = TRUE;		return g_Param.stopVSWR;
	case 44:*Result = TRUE;		return g_Param.startVSWR;
    //...2015.10.11 
	case 154:
			{	//.. 
				EnterCriticalSection(&g_csLock);

				if(Analog_Set_CMD(2, (unsigned char)154, 0, 0))
				{
					if(IO_Read_CMD((unsigned char)154, pRcvBuf, 22))
					{
						Get_Capacitor_LimitData(pRcvBuf);
						*Result = TRUE;
					}
				}
				LeaveCriticalSection(&g_csLock);
				switch(ID2)
				{
				case 1:
					return g_dbLoadLow;
				case 2:
					return g_dbTuneLow;
				case 3:
					return g_dbLoadUp;
				case 4:
					return g_dbTuneUp;
				}
			}		
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
	unsigned char usSubCmd = 0;
	unsigned char ucCmd = 0;
	unsigned short usDataSize;
	*Result = TRUE;
	switch(ID1)
	{
	// Load Tune set On Recipe control    
	case 92:
		if(ID2 == 1)
		{
			g_usLoadValue = (unsigned short)Data;
			return;
		}
		else if(ID2 == 2)
		{
			ucCmd = ID1;
			usSubCmd = NONE;
			usDataSize = 8;
		}
		else
		{
			*Result = FALSE;		
			return;
		}
		break;

	//Load Tune Set Manual
	case 192:    
		//...Load Position Set
		if(ID2 == 1)
		{
			ucCmd = 92;
			usSubCmd = MANUAL_LoadSet;
			usDataSize = 8;
		}
		//...Tune Position Set
		else if(ID2 == 2)
		{
			ucCmd = 92;
			usSubCmd = MANUAL_TuneSet;
			usDataSize = 8;
		}
		else
		{
			*Result = FALSE;		
			return;
		}
		break;
	case 112:
		ucCmd = ID1;
		usSubCmd = NONE;
		usDataSize = 4;
		break;
	case 122:
		ucCmd = ID1;
		usSubCmd = NONE;
		usDataSize = 4;
		break;
	default:	//.. Case. Invalid ID
		PrintError(AO, ID1, ID2, ID3, 0);
		*Result = FALSE;
		return;
	}

	EnterCriticalSection(&g_csLock);
	if(! Analog_Set_CMD(usDataSize, ucCmd, usSubCmd, Data))		*Result = FALSE;
	if(!IO_Read_CMD((unsigned char)ucCmd, pRcvBuf, 13))			*Result = FALSE;
	LeaveCriticalSection(&g_csLock);

	if(!(*Result))	PrintError(AO, ID1, ID2, ID3, 0);
}
//-----------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
void Get_PresetReadBack_data(unsigned char *pRcvBuf)
{
	unsigned short usPresetLoad = 0, usPresetTune = 0;

	//...
	usPresetLoad = pRcvBuf[17];
	usPresetLoad = usPresetLoad << 8;
	usPresetLoad = usPresetLoad + (pRcvBuf[16] & 0xff);
	//...
	usPresetTune = pRcvBuf[19];
	usPresetTune = usPresetTune << 8;
	usPresetTune = usPresetTune + (pRcvBuf[18] & 0xff);
	//...
	g_dbPresetLoad = (double)usPresetLoad;
	g_dbPresetTune = (double)usPresetTune;
}
//////////////////////////////////////////////////////////////////////////
//... 2014.10.10 Add Matcher1,2 Separate (ID9: RF Number)
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 )
{
	BOOL bRet = FALSE;
	int nCnt = 0;
	unsigned char pRcvBuf[256];

	//... Read Matcher Number (ID9) ... Range Fixed Matcher 1 or 2
	if(ID9 == 2)		g_nMatchNumber = 2;
	else				g_nMatchNumber = 1;

	//... Driver Name
	sprintf(g_szDrvName, "%s%d", DRIVER_NAME, g_nMatchNumber);
	
	//... DCOP Buffer reservation.
	g_dcopBuffer.reserve(50);

	/*
		Parameter monitoring
	*/
	memset(&g_Param, 0, sizeof(Parameters));
	g_UpdateSts = Param_Ready;

	do {
		if(ID10 <= 0)		g_nTimeout = TIMEOUT_T;
		else				g_nTimeout = ID10;
		
		if(! LoadIOData())
		{
			printf("[%s] Parameter Read Fail \n]", g_szDrvName);
			break;
		}

		if(! Net_Connect_Port(g_pDevice_Address, g_nPortNum, TRUE))
		{ 
			printf("[%s] Connect failed IP[%s] Port[%d]\n", g_szDrvName, g_pDevice_Address, g_nPortNum);
			break;
		}

		printf("[%s] Driver Connect Success  IP[%s] Port[%d] Type[%d]\n", g_szDrvName, g_pDevice_Address, g_nPortNum, g_ucMatcherType);

		g_nCommStatus = CS_ONLINE;
		InitializeCriticalSection(&g_csLock);
		Sleep(4000);

		////////////////////////////////////////////////////////////////////////// 
		//... Preset Read
		for(nCnt = 0; nCnt < g_nRetryCnt; nCnt++)
		{
			if(Analog_Set_CMD(3, 160, 0, 0))	break;
			else	printf("[%s] Preset Readback Set Failed Retry Count[%d]\n", g_szDrvName, nCnt);

			Sleep(500);
		}

		for(nCnt = 0; nCnt < g_nRetryCnt; nCnt++)
		{
			if(IO_Read_CMD(160, pRcvBuf, 20))
			{
				bRet = TRUE;
				break;
			}
			else	printf("[%s] Preset Readback Read Failed Retry Count[%d]\n", g_szDrvName, nCnt);

			Sleep(500);
		}
		
		PrintBuffer("Preset", pRcvBuf, 20);
		Get_PresetReadBack_data(pRcvBuf);
		printf("[%s] Preset Readback Read success Load[%f] Tune[%f]\n", g_szDrvName, g_dbPresetLoad, g_dbPresetTune);
	    ////////////////////////////////////////////////////////////////////////// 

		if(! bRet)		break;
		
		_beginthread(Polling_Thread, 0, NULL);
		
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
void String_In( int ID1 , char *data , int *Result ) {

	*Result = FALSE;

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
