#include "cimrs232.h"
#include "driver.h"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

#define DRIVER_NAME	"WT300"
#define DRV_NAME	g_szDrvName
char g_szDrvName[40];

#define TIMEOUT_T	1000
#define MAX_CHANNEL	33
#define CRC16		0xA001

enum { DO, DI, AO, AI, SO, SI };
enum { CS_OFFLINE, CS_ONLINE, CS_CONTROLLER_FAILED };
enum { TYPE_UC, TYPE_SC, TYPE_UI, TYPE_SI };
//... 2016.02.25
enum { TP, DZC, ONOFF };
enum { AUTO, MANUAL };
enum { TUNE_OFF, TUNE_ON };


int g_nCtrlAddr = 0x01;
int g_nCommStatus;
int g_nTimeout;
//... 2016.02.25 Added TC Type Read
int g_pnDI[7][MAX_CHANNEL];
double g_pdblAI[10][MAX_CHANNEL];

// 2005.08.29, System Status
int g_nBatterySts = 0;		// 0:OK 1:NG
int g_nAIMCommSts = 0;		// 0:OK 1:NG
int g_nAmbientSts = 0;		// 0:OK 1:NG

//... 2016.02.25 Added String Input (Protocal Status)
char g_szSysErrSts[5]; 

typedef struct ChannelAttributeStruct
{
	unsigned short usAddr;
	int nType;						// 0:UC  1:SC  2:UI  3:SI
	int nChSize;
} CH_ATTR_STRUC;

CH_ATTR_STRUC g_DigTable[7] =
{
	0x0294, TYPE_UI, 2,
	0x20B9, TYPE_UI, 2,
	0x0108, TYPE_UC, 2,
	0x0108, TYPE_UC, 2,		// Virtual, (g_pnDI => Heater Off/On)
	0x0294, TYPE_UI, 2,		// Virtual, (g_pnDI => Alarm)
	0x00C6, TYPE_UI, 2,     //... 2016.02.25 Added InputType(J,K)
	0x0108, TYPE_UC, 2,		//... 2016.02.25 Virtual, (g_pnDI => Heater Reverse/Direct)
};

CH_ATTR_STRUC g_AnaTable[10] = 
{
	0x01CE, TYPE_UI, 2,
	0x016B, TYPE_SI, 2,
	0x014A, TYPE_SI, 2,
	0x0042, TYPE_UC, 2,
	0x0084, TYPE_UI, 2,
	0x0000, TYPE_UC, 2,
	0x1FD2, TYPE_SI, 2,
	0x0210, TYPE_SI, 2,
	0x0231, TYPE_SI, 2,
	0x0252, TYPE_UC, 2
};

unsigned char CRC_Table1[256] , CRC_Table2[256];

CRITICAL_SECTION		m_csLock;

//... 2016.02.25 TP Mode Control
#define HT_CMD_MANUAL	0x04
#define HT_CMD_AUTO		0x00
#define HT_CMD_TUNE		0x08

//... 2016.02.25  DZC Mode Control
#define HT_CMD_MANUAL_DZC	0x05
#define HT_CMD_AUTO_DZC		0x01
#define HT_CMD_TUNE_DZC		0x09

//... 2016.02.25  OnOff Mode Control 
#define HT_CMD_MANUAL_ONOFF	0x07
#define HT_CMD_AUTO_ONOFF	0x03
#define HT_CMD_TUNE_ONOFF	0x0B

#define HT_MODE_BIT		0x04
#define HT_TUNE_BIT		0x08
#define HT_ONOFF_BIT	0x10
//////////////////////////////////////////////////////////////////////////
//... 2016.02.25 Reverse/Direct Mode
//... if you want to control 
#define HT_RESV_DIR_BIT	0x80

// Alarm Bit Definition
#define ALARM_BIT_00	0x0001
#define ALARM_BIT_01	0x0002
#define ALARM_BIT_02	0x0004
#define ALARM_BIT_03	0x0008
#define ALARM_BIT_04	0x0010
#define ALARM_BIT_05	0x0020
#define ALARM_BIT_06	0x0040
#define ALARM_BIT_07	0x0080
#define ALARM_BIT_08	0x0100
#define ALARM_BIT_09	0x0200
#define ALARM_BIT_10	0x0400
#define ALARM_BIT_11	0x0800
#define ALARM_BIT_12	0x1000
#define ALARM_BIT_13	0x2000
#define ALARM_BIT_14	0x4000
#define ALARM_BIT_15	0x8000

//-----------------------------------------------------------------------------------
// Sub Function

void Initial_CRC_ETC() {
	unsigned char i;
	unsigned int mask , crc , mem;
	
	for ( mask = 0 ; mask < 256 ; mask++ ) {
		crc = mask;
		for ( i = 0 ; i < 8 ; i++ ) {
			mem = (unsigned int) ( crc & 0x0001 );
			crc /= 2;
			if ( mem ) crc ^= CRC16;
		}
		CRC_Table2[ mask ] = (unsigned char) ( crc & 0xff );
		CRC_Table1[ mask ] = (unsigned char) ( crc >>   8 );
	}
}

void Get_CRC(unsigned char *pData, int nSize, unsigned char *C0, unsigned char *C1)
{
	unsigned char Car , i , Crc0 , Crc1;

	Crc0 = 0xff;
	Crc1 = 0xff;
	for ( i = 0 ; i < nSize ; i++ ) {
		Car = pData[ i ];
		Car ^= Crc0;
		Crc0 = (unsigned char) ( Crc1 ^ CRC_Table2[ Car ] );
		Crc1 = CRC_Table1[ Car ];
	}
	*C0 = Crc0;
	*C1 = Crc1;
}

void PrintBuffer(char *szBufName, unsigned char *pBuf, int nSize)
{
	int i;
	printf("%s :", szBufName);
	for(i=0; i<nSize; i++)
	{
		printf(" %02X", pBuf[i]);
	}
	printf(" in %s\n", DRV_NAME);
}


BOOL RS232IO_Write(unsigned short usAddr, unsigned short usData)
{
	char pSndBuf[256];
	char pRcvBuf[256];
	int  nDataCnt , nRcvCnt;

	pSndBuf[ 0 ] = g_nCtrlAddr;
	pSndBuf[ 1 ] = 0x06;
	pSndBuf[ 2 ] = usAddr >> 8;
	pSndBuf[ 3 ] = usAddr & 0x00ff;
	pSndBuf[ 4 ] = usData >> 8;
	pSndBuf[ 5 ] = usData & 0x00ff;
	
	Get_CRC( pSndBuf , 6 , &pSndBuf[6] , &pSndBuf[7] );

	// Write Data
	nDataCnt = 8;
	if ( !RS232_Write_String( pSndBuf , nDataCnt ) )
	{
		printf("Write failed in %s\n", DRV_NAME);
		PrintBuffer("SndData", pSndBuf, nDataCnt);
		g_nCommStatus = CS_OFFLINE;
		return FALSE;
	}

	// Check echo
	if ( !RS232_Read_Count_Char( pRcvBuf , nDataCnt , &nRcvCnt , g_nTimeout ) ) {
		printf("EchoCheck failed in %s\n", DRV_NAME);
		PrintBuffer("RcvData", pRcvBuf, nRcvCnt);
		PrintBuffer("Sended", pSndBuf, nDataCnt);
		g_nCommStatus = CS_OFFLINE;
		return FALSE;
	}
	return TRUE;
}

BOOL RS232IO_Read(unsigned short usAddr, int nChSize, char *pRcvBuf)
{
	char pSndBuf[256];
	int  nDataCnt , nRcvCnt = 0;

	pSndBuf[ 0 ] = g_nCtrlAddr;
	pSndBuf[ 1 ] = 0x03;
	pSndBuf[ 2 ] = usAddr >> 8;
	pSndBuf[ 3 ] = usAddr & 0x00ff;
	pSndBuf[ 4 ] = 0x00;
	pSndBuf[ 5 ] = (char) MAX_CHANNEL;
	
	Get_CRC( pSndBuf , 6 , &pSndBuf[6] , &pSndBuf[7] );

	// Write Data
	if ( !RS232_Write_String( pSndBuf , 8 ) )
	{
		printf("Write failed in %s\n", DRV_NAME);
		PrintBuffer("SndData", pSndBuf, 8);
		g_nCommStatus = CS_OFFLINE;
		return FALSE;
	}
//	printf("Snd ");
//	PrintBuffer(pSndBuf, 8);

	// Read Data
	nDataCnt = 5 + ( MAX_CHANNEL * nChSize);
	if ( !RS232_Read_Count_Char( pRcvBuf , nDataCnt , &nRcvCnt , g_nTimeout ) ) {
		printf("Read failed in %s\n", DRV_NAME);
		PrintBuffer("RcvData", pRcvBuf, nRcvCnt);
		PrintBuffer("Sended", pSndBuf, 8);
		g_nCommStatus = CS_OFFLINE;
		return FALSE;
	}
//	printf("Rcv ");
//	PrintBuffer(pRcvBuf, nRcvCnt);
	return TRUE;
}

// 2005.08.29, Status Read
BOOL RS232IO_Read2(unsigned short usAddr, int nDataSize, char *pRcvBuf)
{
	char pSndBuf[256];
	int  nDataCnt , nRcvCnt = 0;

	pSndBuf[ 0 ] = g_nCtrlAddr;
	pSndBuf[ 1 ] = 0x03;
	pSndBuf[ 2 ] = usAddr >> 8;
	pSndBuf[ 3 ] = usAddr & 0x00ff;
	pSndBuf[ 4 ] = 0x00;
	pSndBuf[ 5 ] = 0x01;
	
	Get_CRC( pSndBuf , 6 , &pSndBuf[6] , &pSndBuf[7] );

	// Write Data
	if ( !RS232_Write_String( pSndBuf , 8 ) )
	{
		printf("Write failed in %s\n", DRV_NAME);
		PrintBuffer("SndData", pSndBuf, 8);
		g_nCommStatus = CS_OFFLINE;
		return FALSE;
	}
//	PrintBuffer("Snd", pSndBuf, 8);

	// Read Data
	nDataCnt = 5 + nDataSize;
	if ( !RS232_Read_Count_Char( pRcvBuf , nDataCnt , &nRcvCnt , g_nTimeout ) ) {
		printf("Read failed in %s\n", DRV_NAME);
		PrintBuffer("RcvData", pRcvBuf, nRcvCnt);
		PrintBuffer("Sended", pSndBuf, 8);
		g_nCommStatus = CS_OFFLINE;
		return FALSE;
	}
//	PrintBuffer("Rcv", pRcvBuf, nRcvCnt);
	return TRUE;
}

void UpdateDI(int nCmd, char *pBuf)
{
	int nIdx = nCmd-1;
	int nData = 0;
	unsigned char *pData;
	unsigned short usData = 0;
	int nType;
	int nChSize;
	int i;

	nType = g_DigTable[nIdx].nType;
	nChSize = g_DigTable[nIdx].nChSize;
	for(i=0; i<MAX_CHANNEL; i++)
	{
		pData = (pBuf + i*nChSize);
		usData = (*pData) * 256 + (*(pData+1));
		switch(nType)
		{
		case TYPE_UC: nData = (unsigned char) usData; break;
		case TYPE_SC: nData = (char ) usData; break;
		case TYPE_UI: nData = (unsigned short) usData; break;
		case TYPE_SI: nData = (short) usData; break;
		}
		g_pnDI[nIdx][i] = nData;
	}
}

void UpdateCtrlMd(int nCmd, char *pBuf)
{
	int nIdx = 2; // nCmd-1;
	int nData = 0;
	unsigned char *pData;
	unsigned char byData = 0;
	int nType;
	int nChSize;
	int nMode;
	int i;
    //... 2016.02.25
	BOOL Ctrl;
	BOOL AutoManMode;
	BOOL Tune;

	// for OnOff Channel
	int nOffOn = 0;
	int nOffOnIdx = 3;		 //... nIdx+1;
	int nRevsDirect = 0;
	int nRevsDirectIdx  = 6; //... Reverse & Direct Index


	nType = g_DigTable[nIdx].nType;
	nChSize = g_DigTable[nIdx].nChSize;
	for(i=0; i<MAX_CHANNEL; i++)
	{
		pData = (pBuf + i*nChSize);
		byData = *(pData+1);
        //... 2016.02.25 //... 2016.03.30 OnOff Mode Reading 오류 수정
		if((byData&0x03)==0x03) Ctrl = ONOFF;   
		else if ((byData&0x01)==0x01) Ctrl = DZC;
		else Ctrl=TP;

		if ((byData&0x04)==0x04) AutoManMode = MANUAL;
		else AutoManMode=AUTO;

		if ((byData&0x08)==0x08) Tune = TUNE_ON;
		else Tune=TUNE_OFF;
		//... TP Mode: nMode = 0:Manual 1:Auto  2:Tune 
		if ( Ctrl==TP && AutoManMode==MANUAL && Tune==TUNE_OFF) nMode=0;
		else if ( Ctrl==TP && AutoManMode==AUTO && Tune==TUNE_OFF) nMode=1;
		else if ( Ctrl==TP && AutoManMode==MANUAL && Tune==TUNE_ON) nMode=2;
		//... DZC Mode: nMode = 3:Manual 4:Auto  5:Tune
		else if ( Ctrl==DZC && AutoManMode==MANUAL && Tune==TUNE_OFF) nMode=3;
		else if ( Ctrl==DZC && AutoManMode==AUTO && Tune==TUNE_OFF) nMode=4;
		else if ( Ctrl==DZC && AutoManMode==MANUAL && Tune==TUNE_ON) nMode=5;
		//... OnOff Mode: nMode = 6:Manual 7:Auto  8:Tune
		else if ( Ctrl==ONOFF && AutoManMode==MANUAL && Tune==TUNE_OFF) nMode=6;
		else if ( Ctrl==ONOFF && AutoManMode==AUTO && Tune==TUNE_OFF) nMode=7;
		else if ( Ctrl==ONOFF && AutoManMode==MANUAL && Tune==TUNE_ON) nMode=8;
        //...

		g_pnDI[nIdx][i] = nMode;

		//... Heater Control ON/OFF Status
		if((byData & HT_ONOFF_BIT) == HT_ONOFF_BIT) nOffOn = 1;				// 0:Off  1:On
		else nOffOn = 0;
		g_pnDI[nOffOnIdx][i] = nOffOn;
		
		//... 2016.02.25
		if((byData & HT_RESV_DIR_BIT) == HT_RESV_DIR_BIT) nRevsDirect = 1;	//... 0:Reverse  1:Direct
		else nRevsDirect = 0;
		//... 2016.02.25 Update Heat Output Action
		g_pnDI[nRevsDirectIdx][i] = nRevsDirect;
	}
}

void UpdateAlarmDesc(int nHtIdx, unsigned short usValue)
{
	int nAlarm = 0;
	if(usValue == 0) nAlarm = 16;										// Normal
	else if( (usValue & ALARM_BIT_06) == ALARM_BIT_06 ) nAlarm = 6;		// TC_Reversed
	else if( (usValue & ALARM_BIT_07) == ALARM_BIT_07 ) nAlarm = 7;		// TC_Short
	else if( (usValue & ALARM_BIT_08) == ALARM_BIT_08 ) nAlarm = 8;		// TC_Break
	else if( (usValue & ALARM_BIT_09) == ALARM_BIT_09 ) nAlarm = 9;		// RTD_Open
	else if( (usValue & ALARM_BIT_10) == ALARM_BIT_10 ) nAlarm = 10;	// RTD_SHORT
	else if( (usValue & ALARM_BIT_13) == ALARM_BIT_13 ) nAlarm = 13;	// Ambient Cal Error
	else if( (usValue & ALARM_BIT_14) == ALARM_BIT_14 ) nAlarm = 14;	// Full Scale Cal Error
	else if( (usValue & ALARM_BIT_15) == ALARM_BIT_15 ) nAlarm = 15;	// Offset Cal Error
	else if( (usValue & ALARM_BIT_04) == ALARM_BIT_04 ) nAlarm = 4;		// Low process
	else if( (usValue & ALARM_BIT_05) == ALARM_BIT_05 ) nAlarm = 5;		// High process
	else if( (usValue & ALARM_BIT_02) == ALARM_BIT_02 ) nAlarm = 2;		// Low deviation
	else if( (usValue & ALARM_BIT_03) == ALARM_BIT_03 ) nAlarm = 3;		// High deviation
	else nAlarm = 16;
	g_pnDI[4][nHtIdx] = nAlarm;
}

void UpdateAlarmStatus(int nCmd, char *pBuf)
{
	int nIdx = nCmd-1;
	int nData = 0;
	unsigned char *pData;
	unsigned short usData = 0;
	int nType;
	int nChSize;
	int i;

	nType = g_DigTable[nIdx].nType;
	nChSize = g_DigTable[nIdx].nChSize;
	for(i=0; i<MAX_CHANNEL; i++)
	{
		pData = (pBuf + i*nChSize);
		usData = (*pData) * 256 + (*(pData+1));
		switch(nType)
		{
		case TYPE_UC: nData = (unsigned char) usData; break;
		case TYPE_SC: nData = (char ) usData; break;
		case TYPE_UI: nData = (unsigned short) usData; break;
		case TYPE_SI: nData = (short) usData; break;
		}
		if(nData == 0) g_pnDI[nIdx][i] = 0;
		else g_pnDI[nIdx][i] = 1;
		UpdateAlarmDesc(i, usData);
	}
}


void UpdateAI(int nCmd, char *pBuf)
{
	int nIdx = nCmd-1;
	int i;
	double dblData = 0;
	unsigned char *pData;
	unsigned short usData = 0;
	int nType;
	int nChSize;

	nType = g_AnaTable[nIdx].nType;
	nChSize = g_AnaTable[nIdx].nChSize;
	for(i=0; i<MAX_CHANNEL; i++)
	{
		pData = pBuf + i*nChSize;
		usData = (*pData) * 256 + (*(pData+1));
		switch(nType)
		{
		case TYPE_UC: dblData = (unsigned char) usData; break;
		case TYPE_SC: dblData = (char) usData; break;
		case TYPE_UI: dblData = (unsigned short) usData; break;
		case TYPE_SI: dblData = (short) usData; break;
		}
		g_pdblAI[nIdx][i] = dblData;
	}
}

// 2005.08.29, System Status
void UpdateSystemSts(char *pBuf)
{
	unsigned char *pData;
	unsigned short usData = 0;
	pData = pBuf;
	usData = (*(pData)) * 256 + (*(pData+1));
	if((usData & 0x01) == 0) g_nBatterySts = 0;		// 0:OK 1:NG
	else g_nBatterySts = 1;
	if((usData & 0x04) == 0) g_nAIMCommSts = 0;		// 0:OK 1:NG
	else g_nAIMCommSts = 1;
	if((usData & 0x08) == 0) g_nAmbientSts = 0;		// 0:OK 1:NG
	else g_nAmbientSts = 1;
    //... 2016.02.25
	memset ( g_szSysErrSts, 0x00, sizeof(g_szSysErrSts));
	g_szSysErrSts[0] = *(pData+1);
}

//-----------------------------------------------------------------------------------
// DI Function

BOOL GetCommStatus(int *pData)
{
	*pData = g_nCommStatus;
	return TRUE;
}

int GetDigitalValue(int nCmd, int nHtrNum)
{
	int nCmdIdx = nCmd-1;
	int nHtrIdx = nHtrNum-1;
	return g_pnDI[nCmdIdx][nHtrIdx];
}

//-----------------------------------------------------------------------------------
// DO Function

BOOL SetDigitalValue(int nCmd, int nHtrNum, int nData)
{
	BOOL bRet = FALSE;
	int nIdx = nCmd-1;
	int nHtrIdx = nHtrNum-1;
	unsigned short usData;
	short sData;
	unsigned short usAddr;

	RS232_Clear_Buffer();
	switch(g_DigTable[nIdx].nType)
	{
	case TYPE_UC: usData = nData; break;
	case TYPE_SC: usData = (char) nData; break;
	case TYPE_UI: usData = nData; break;
	case TYPE_SI: sData = (unsigned short) nData;
		memcpy(&usData, &sData, 2);
		break;
	}

	usAddr = g_DigTable[nIdx].usAddr+nHtrIdx;
	bRet = RS232IO_Write(usAddr, usData);
	if(TRUE == bRet) g_pnDI[nIdx][nHtrIdx] = nData;
	return bRet;
}

BOOL SetCtrlMd(int nHtrNum, int nData)
{
	BOOL bRet = FALSE;
	int nCmd = 3;
	int nIdx = nCmd-1;
	int nHtrIdx = nHtrNum-1;
	unsigned short usData;
	unsigned short usAddr;

	RS232_Clear_Buffer();
	if      ( nData == 0 ) usData = HT_CMD_MANUAL;			// Manual ( TP : Time Proportioning )
	else if ( nData == 1 ) usData = HT_CMD_AUTO;			// Auto
	else if ( nData == 2 ) usData = HT_CMD_TUNE;			// Tune
	else if ( nData == 3 ) usData = HT_CMD_MANUAL_DZC;		// Manual ( DZC )
	else if ( nData == 4 ) usData = HT_CMD_AUTO_DZC;		// Auto
	else if ( nData == 5 ) usData = HT_CMD_TUNE_DZC;		// Tune
	else if ( nData == 6 ) usData = HT_CMD_MANUAL_ONOFF;	// Manual ( OnOff )
	else if ( nData == 7 ) usData = HT_CMD_AUTO_ONOFF;		// Auto
	else if ( nData == 8 ) usData = HT_CMD_TUNE_ONOFF;		// Tune	
	else return FALSE;

	if(g_pnDI[nIdx+1][nHtrIdx] == 1) usData += HT_ONOFF_BIT;	// Check OffOn Status
	if(g_pnDI[nIdx+4][nHtrIdx] == 1) usData += HT_RESV_DIR_BIT;	//... Heater Output Action Status

	usAddr = g_DigTable[nIdx].usAddr+nHtrIdx;
	bRet = RS232IO_Write(usAddr, usData);
	if(TRUE == bRet) g_pnDI[nIdx][nHtrIdx] = nData;
	return bRet;
}

BOOL SetOnOff(int nHtrNum, int nData)
{
	BOOL bRet = FALSE;
	int nCmd = 4;
	int nIdx = nCmd-1;
	int nHtrIdx = nHtrNum-1;
	int nRevsDirectIdx  = 6; //... Reverse & Direct Index
	unsigned short usData;
	unsigned short usAddr;

	RS232_Clear_Buffer();
	switch(g_pnDI[nIdx-1][nHtrIdx])
	{
	case 0: usData = HT_CMD_MANUAL;		break;		// Manual
	case 1:	usData = HT_CMD_AUTO;		break;		// Auto
	case 2: usData = HT_CMD_TUNE;		break;		// Tune
    //... 2016.02.25
	case 3: usData = HT_CMD_MANUAL_DZC;	break; 		// Manual ( DZC )
	case 4: usData = HT_CMD_AUTO_DZC;	break;		// Auto
	case 5: usData = HT_CMD_TUNE_DZC;	break;		// Tune
	case 6: usData = HT_CMD_MANUAL_ONOFF;	break;		// Manual ( OnOff&Direct Mode )
	case 7: usData = HT_CMD_AUTO_ONOFF;	break;		// Auto
	case 8: usData = HT_CMD_TUNE_ONOFF;	break;		// Tune	
    //... 
	}

	if(nData == 1) usData += HT_ONOFF_BIT;						   // Check OffOn Status
	if(g_pnDI[nRevsDirectIdx][nHtrIdx] == 1) usData += HT_RESV_DIR_BIT; //... 0:Reverse  1:Direct

	usAddr = g_DigTable[nIdx].usAddr+nHtrIdx;
	bRet = RS232IO_Write(usAddr, usData);
	if(TRUE == bRet) g_pnDI[nIdx][nHtrIdx] = nData;
	return bRet;
}

//-----------------------------------------------------------------------------------
// AI Function

double GetAnalogValue(int nCmd, int nHtrNum)
{
	int nIdx = nCmd-1;
	int nHtrIdx = nHtrNum-1;
	return g_pdblAI[nIdx][nHtrIdx];
}

//-----------------------------------------------------------------------------------
// AO Function

BOOL SetAnalogValue(int nCmd, int nHtrNum, double dblData)
{
	BOOL bRet = FALSE;
	int nIdx = nCmd-1;
	int nHtrIdx = nHtrNum-1;
	unsigned short usData;
	short sData;
	unsigned short usAddr;

	RS232_Clear_Buffer();
	switch(g_AnaTable[nIdx].nType)
	{
	case TYPE_UC: usData = (unsigned char) dblData; break;
	case TYPE_SC: usData = (char) dblData; break;
	case TYPE_UI: usData = (unsigned int ) dblData; break;
	case TYPE_SI: sData = (unsigned short) dblData;
		memcpy(&usData, &sData, 2);
		break;
	}

	usAddr = g_AnaTable[nIdx].usAddr+nHtrIdx;
	bRet = RS232IO_Write(usAddr, usData);
	if(TRUE == bRet) g_pdblAI[nIdx][nHtrIdx] = dblData;
	return bRet;
}

//-----------------------------------------------------------------------------------
// SI Function

//-----------------------------------------------------------------------------------
// SO Function

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

void PrintError( int IO , int ID1 , int ID2 , int ID3 , int ID4 ) {
	switch( IO )
	{
	case DO :	printf( "DO Failed : [%d][%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3 , ID4, DRV_NAME ); 
				break;
	case DI :	printf( "DI Failed : [%d][%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3 , ID4, DRV_NAME ); 
				break;
	case AO :	printf( "AO Failed : [%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3, DRV_NAME ); 
				break;
	case AI :	printf( "AI Failed : [%d][%d][%d] in %s Driver\n" , ID1 , ID2 , ID3, DRV_NAME ); 
				break;
	case SO :	printf( "SO Failed : [%d] in %s Driver\n" , ID1, DRV_NAME ); 
				break;
	case SI :	printf( "SI Failed : [%d] in %s Driver\n" , ID1, DRV_NAME ); 	
				break;
	default :	printf( "I/O Failed in %s Driver", DRV_NAME );
				break;
	}
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Thread

BOOL ReadDigital(int nCmd) 
{
	BOOL bRet = FALSE;
	int nIdx = 0;
	unsigned char pBuf[256];
	unsigned short usAddr;
	int nChSize;

	EnterCriticalSection(&m_csLock);
	RS232_Clear_Buffer();
	do {
		nIdx = nCmd - 1;
		usAddr = g_DigTable[nIdx].usAddr;
		nChSize = g_DigTable[nIdx].nChSize;
		if(! RS232IO_Read(usAddr, nChSize, pBuf)) break;

		if(nCmd == 3) UpdateCtrlMd(nCmd, pBuf+3);
		else if(nCmd == 1) UpdateAlarmStatus(nCmd, pBuf+3);
		else UpdateDI(nCmd, pBuf+3);

		bRet = TRUE;
	} while(0);
	LeaveCriticalSection(&m_csLock);			
	return bRet;
}

BOOL ReadAnalog(int nCmd)
{
	BOOL bRet = FALSE;
	int nIdx = 0;
	unsigned char pBuf[256];
	unsigned short usAddr;
	int nChSize;

	EnterCriticalSection(&m_csLock);
	RS232_Clear_Buffer();
	do {
		nIdx = nCmd - 1;
		usAddr = g_AnaTable[nIdx].usAddr;
		nChSize = g_AnaTable[nIdx].nChSize;
		if(! RS232IO_Read(usAddr, nChSize, pBuf)) break;
		UpdateAI(nCmd, pBuf+3);
		bRet = TRUE;
	} while(0);
	LeaveCriticalSection(&m_csLock);			
	return bRet;
}

BOOL ReadSystemSts()
{
	BOOL bRet = FALSE;
	int nIdx = 0;
	unsigned char pBuf[256];

	EnterCriticalSection(&m_csLock);
	RS232_Clear_Buffer();
	do {
		if(! RS232IO_Read2(0x03b0, 2, pBuf)) break;
		UpdateSystemSts(pBuf+3);
		bRet = TRUE;
	} while(0);
	LeaveCriticalSection(&m_csLock);			
	return bRet;
}


BOOL g_bThreadRun = FALSE;
BOOL g_bThreadFuncRun = FALSE;

void Polling_Thread()
{
	int i;
	Sleep(10000);
	g_bThreadRun = TRUE;
	g_bThreadFuncRun = TRUE;
	while(g_bThreadRun)
	{
		for(i=1; i<=6; i++)
		{
			if(i == 2 || i == 4 || i == 5) continue;
			if(! g_bThreadRun) break;
			if(FALSE == ReadDigital(i)) //... 2016.02.25 1(AlrmStsDI),3(OutputType),6(InputType = TC Type)
			{
				g_nCommStatus = CS_OFFLINE;
				Sleep(3000);
			}
			else { g_nCommStatus = CS_ONLINE; Sleep(500); }
		}
		for(i=1; i<=6; i++)
		{
			if(! g_bThreadRun) break;
			if(FALSE == ReadAnalog(i))
			{
				g_nCommStatus = CS_OFFLINE;
				Sleep(3000);
			}
			else { g_nCommStatus = CS_ONLINE; Sleep(500); }
		}

		// 2005.08.29, System Status
		if(! g_bThreadRun) break;
		if(FALSE == ReadSystemSts())
		{
			g_nCommStatus = CS_OFFLINE;
			Sleep(3000);
		}
		else { g_nCommStatus = CS_ONLINE; Sleep(500); }
	}
	g_bThreadFuncRun = FALSE;
	_endthread();
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL ReadBackAllChannel()
{
	BOOL bRet = TRUE;
	int i;
	do {
		for(i=1; i<=6; i++)
		{
			if(i != 4 && i !=5 ) bRet = ReadDigital(i);
			else bRet = TRUE;
			if(bRet == FALSE)
			{
				printf("-----> Digital ReadBack failed[Cmd:%d] in %s\n", i, DRV_NAME);
				break;
			}
			Sleep(100);
		}
		if(bRet == FALSE) break;
		for(i=1; i<=10; i++)
		{
			bRet = ReadAnalog(i);
			if(bRet == FALSE)
			{
				printf("-----> Analog Readback failed[Cmd:%d] in %s\n", i, DRV_NAME);
				break;
			}
			Sleep(100);
		}
		if(bRet == FALSE) break;
	} while(0);
	return bRet;
}


BOOL Initialize()
{
	BOOL bRet = TRUE;
	do {
	    Initial_CRC_ETC();
		InitializeCriticalSection(&m_csLock);
		_beginthread((void*)Polling_Thread, 0, NULL);
	} while(0);
	return bRet;
}

void DeInitialize()
{
	int nTCount = 0;
	g_bThreadRun = FALSE;
	while(g_bThreadFuncRun)
	{
		Sleep(1000);
		if(++nTCount > 5) break;
	}
	DeleteCriticalSection(&m_csLock);
}

BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 )
{
	BOOL bRet = TRUE;
    //... 2016.02.25
	char FilePath[120] , FileName[64];
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
    //... 

	sprintf(g_szDrvName, "%s[%d-%d]", DRIVER_NAME, ID1, ID9);

	do {
		if(ID10 <= 0) g_nTimeout = TIMEOUT_T;
		else g_nTimeout = ID10;
		g_nCtrlAddr = ID9;

		if(! RS232_Connect_Port(ID1, ID2, ID3, ID4, ID5, ID6, ID7, ID8)) break;
		g_nCommStatus = CS_ONLINE;
		Sleep(1000);
		bRet = Initialize();
		if(bRet == FALSE) break;

		/* 2004.05.12 by cskim
		bRet = ReadBackAllChannel();
		if(bRet == FALSE) printf("-----> ReadBackAllChannel failed in %s", DRV_NAME);
		*/
	} while(0);

	//... 2016.02.25 
	if ( strlen(File) >= 2) { 
		sprintf( FilePath , "D:DATALOG\\WATLOW");
		sprintf( FileName , "D:DATALOG\\WATLOW\\COM.log" );
		
		
	}
	else if ( strlen(File) == 1 ) {
		sprintf( FilePath , "%s:\\DATALOG\\WATLOW" , File );
		sprintf( FileName , "%s:\\DATALOG\\WATLOW\\COM.log" , File );		
		
	}
	
	//Logging 폴더 생성
	hSrch=FindFirstFile(FilePath,&wfd);
	if (hSrch == INVALID_HANDLE_VALUE) 	
	{
		if ( !CreateDirectory ( FilePath , NULL )) 
		{
			printf( "\n[%s] : Can't Create Log File\n", DRV_NAME );
		}
	}
	FindClose(hSrch);
	//
	
	RS232_Logging_Style( 1 , FileName );  // 0:Ascii, 1:B
	RS232_Logging_Control( FALSE );


	bRet = ReadBackAllChannel();
	if(bRet == FALSE) printf("\n-----> ReadBackAllChannel failed in %s\n", DRV_NAME);

	//...

	return bRet;
}

BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	DeInitialize();
	RS232_Disconnect_Port();
	return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result )
{
	int nData = 0;
	int nCmd, nHtrNum;

	EnterCriticalSection(&m_csLock);
	*Result = FALSE;
	nCmd = ID1;
	nHtrNum = ID2;
	g_nCtrlAddr = ID3;
	switch(ID1)
	{
	case 99: *Result = GetCommStatus(&nData); break;
	case 3:
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:   
	case 7:   //... 2016.02.25
		nData = GetDigitalValue(nCmd, nHtrNum);
		*Result = TRUE;
		break;
	// 2005.08.29, System Status
	case 16: nData = g_nBatterySts; *Result = TRUE; break;
	case 17: nData = g_nAIMCommSts; *Result = TRUE; break;
	case 18: nData = g_nAmbientSts; *Result = TRUE; break;
	}

//  Result를 항상 True로 만들어 Comm Alarm 발생 안하는 문제 수정
// 	if( !(*Result) ) { PrintError(DI, ID1, ID2, ID3, ID4); return 0; }
// 	else g_nCommStatus = CS_ONLINE;
	LeaveCriticalSection(&m_csLock);			
	return nData;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result )
{
	int nCmd, nHtrNum;

	EnterCriticalSection(&m_csLock);
	*Result = FALSE;
	nCmd = ID1;
	nHtrNum = ID2;
	g_nCtrlAddr = ID3;
	switch(ID1)
	{
	case 98: *Result = ReadBackAllChannel(); break;
	case 3: *Result = SetCtrlMd(nHtrNum, Data); break;
	case 4: *Result = SetOnOff(nHtrNum, Data); break;
	case 2:	*Result = SetDigitalValue(nCmd, nHtrNum, Data);	break;
	//... Add Logging
	case 999: 
		if ( Data == 0 ) // Log Off
		{
			RS232_Logging_Control( FALSE );
		}else if ( Data == 1 ) // Log On
		{
			RS232_Logging_Control( TRUE );
		}
		*Result = TRUE;
	}
	if(FALSE == *Result) PrintError(DO, ID1, ID2, ID3, ID4);
	else g_nCommStatus = CS_ONLINE;
	LeaveCriticalSection(&m_csLock);		
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

double Analog_In( int ID1 , int ID2 , int ID3 , int *Result )
{
	double dblData = 0;
	int nCmd, nHtrNum;

	EnterCriticalSection(&m_csLock);
	*Result = FALSE;
	nCmd = ID1;
	nHtrNum = ID2;
	g_nCtrlAddr = ID3;
	switch(ID1)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		dblData= GetAnalogValue(nCmd, nHtrNum);
		/*
		//... 2016.02.25
		if ( ID1 == 6 )  // Gain
		{
			if ( GetDigitalValue(nCmd, nHtrNum) == 2 )  // Linear_0   J_1   K_2   T   S    R    B    Pulse RTD1   RTD2  S10   R11 R12 R13 NA14 NA15 NA16 NA17  Nickel MotorSpd  E
				dblData = (int) (( 1639 - 6 ) / dblData);		//Vaporizer : K-Type(6 to 1639 degC)
			else
				dblData = (int) ((  972 - 3 ) / dblData);	    //else : J-Type(3 to  972 degC)
		}
		//...
		*/
		*Result = TRUE;
		break;
	}

// 	if( !(*Result) ) { PrintError(AI, ID1, ID2, ID3, 0); return 0; }
// 	else g_nCommStatus = CS_ONLINE;
	LeaveCriticalSection(&m_csLock);			
	return dblData;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result )
{
	int nCmd, nHtrNum;

	EnterCriticalSection(&m_csLock);
	*Result = FALSE;
	nCmd = ID1;
	nHtrNum = ID2;
	g_nCtrlAddr = ID3;
	switch(ID1)
	{
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		/*
		//... 2016.02.25
		if ( ID1 == 6 )  // Gain
		{
			if ( GetDigitalValue(nCmd, nHtrNum) == 2 )  // Linear_0   J_1   K_2   T   S    R    B    Pulse RTD1   RTD2  S10   R11 R12 R13 NA14 NA15 NA16 NA17  Nickel MotorSpd  E
				Data = (int) (( 1639 - 6 ) / Data);		//Vaporizer : K-Type(6 to 1639 degC)
			else
				Data = (int) ((  972 - 3 ) / Data);	    //else : J-Type(3 to  972 degC)

			if ( Data > 255 ) Data = 255;
		}
		//...
		*/
		*Result = SetAnalogValue(nCmd, nHtrNum, Data);
		break;
	}

	if( !(*Result) ) PrintError(AO, ID1, ID2, ID3, 0); 
	else g_nCommStatus = CS_ONLINE;
	LeaveCriticalSection(&m_csLock);			
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	EnterCriticalSection(&m_csLock);
	*Result = FALSE;

	switch( ID1 ) {		
		case 9 :	// WTR_SystemStatus
			*Result = TRUE;
			data[0] = g_szSysErrSts[0];
			printf("( StringIn - data[0] = %02X )\n" , data[0] );
			break;
			
		default :	
			*Result = FALSE;
			break;
	}

	LeaveCriticalSection(&m_csLock);			
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {
	EnterCriticalSection(&m_csLock);
	*Result = FALSE;

	LeaveCriticalSection(&m_csLock);			
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
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
