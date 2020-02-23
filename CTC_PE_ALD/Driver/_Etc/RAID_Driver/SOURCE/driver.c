#include "cimrs232.h"
#include "driver.h"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

#define DRIVER_NAME	"RAID_Driver"
#define DRV_NAME	g_szDrvName
char g_szDrvName[40];

#define TIMEOUT_T	1000

// 2010.01.20
static char g_RAID_CMD_CTC_TYPE1[]	= { 0x1b, 0x69, 0x6e, 0x66, 0x6f, 0x30, 0x78, 0x79, 0x7a };
static char g_RAID_CMD_PMC_TYPE1[]	= "**";
static char g_RAID_CMD_PMC_TYPE2[]	= "**";	

enum { DO, DI, AO, AI, SO, SI };

int g_nTimeout = TIMEOUT_T;

enum { HD_OK, HD_FAIL, HD_BUILDING };
int g_nDiskFail1 = HD_OK, g_nDiskFail2 = HD_OK; 

enum { CS_OFFLINE, CS_ONLINE, CS_COMM_FAIL };
int g_nCommStatus = CS_ONLINE;

// 2010.01.20
enum { RT_CTC_TYPE1, RT_PMC_TYPE1, RT_PMC_TYPE2 };
int g_nRAIDType = RT_PMC_TYPE1;

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

BOOL RS232IO_Write(char *pSndBuf, int nDataCnt)
{
	// Write Data
	RS232_Clear_Buffer();
	if ( !RS232_Write_String( pSndBuf , nDataCnt ) )
	{
		printf("Write failed in %s\n", DRV_NAME);
//		PrintBuffer("SndData", pSndBuf, nDataCnt);

		g_nCommStatus = CS_COMM_FAIL;
		return FALSE;
	}

	return TRUE;
}

BOOL RS232IO_Read(char *pRcvBuf)
{
	int  nDataCnt , nRcvCnt = 0;
	BOOL bRet=TRUE;
	
	// Read Data
	nDataCnt = 256;
	RS232_Read_Count_Char( pRcvBuf , nDataCnt , &nRcvCnt , g_nTimeout );
	
//	PrintBuffer("RcvData", pRcvBuf, nRcvCnt);
		
	if(nRcvCnt <= 1)
	{
		g_nCommStatus = CS_OFFLINE;
		bRet = FALSE;
	}
	return bRet;
}


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


int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result )
{

	switch(ID1)
	{
	case 1: 
		*Result = TRUE;
		return g_nDiskFail1;
	case 2: 
		*Result = TRUE;
		return g_nDiskFail2;
	case 99: 
		*Result = TRUE;
		return g_nCommStatus;
	default:
		*Result = FALSE;
		return FALSE;
	}
}

void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result )
{
	*Result = FALSE;
}


void UpdateAI_CTC_TYPE1()			// 2010.01.20
{
	char pRcvBuf[1000];
	unsigned short usDiskFail1, usDiskFail2;

	do {
		if(!RS232IO_Write(g_RAID_CMD_CTC_TYPE1, 9))
			break;
		Sleep(500);
		if(!RS232IO_Read(pRcvBuf))
			break;

		if(pRcvBuf[18] != 0x49) { g_nCommStatus = CS_COMM_FAIL; break; }
		if(pRcvBuf[19] != 0x4e) { g_nCommStatus = CS_COMM_FAIL; break; }
		if(pRcvBuf[20] != 0x46) { g_nCommStatus = CS_COMM_FAIL; break; }
		if(pRcvBuf[21] != 0x4f) { g_nCommStatus = CS_COMM_FAIL; break; }
		if(pRcvBuf[22] != 0x30) { g_nCommStatus = CS_COMM_FAIL; break; }
	
		usDiskFail1 = pRcvBuf[30];
		if(usDiskFail1 == 0)
				g_nDiskFail1 = HD_FAIL;
		else if(usDiskFail1 == 4)
				g_nDiskFail1 = HD_OK;
		else 
				g_nDiskFail1 = HD_BUILDING;
		//	printf(">>>>>>>>> RAID Fail HDD 1 => %d\n",usDiskFail1);

		usDiskFail2 = pRcvBuf[32];
		if(usDiskFail2 == 0)
				g_nDiskFail2 = HD_FAIL;
		else if(usDiskFail2 == 4)
				g_nDiskFail2 = HD_OK;
		else 
				g_nDiskFail2 = HD_BUILDING;

		//	printf(">>>>>>>>> RAID Fail HDD 2 => %d\n",usDiskFail2);
		g_nCommStatus = CS_ONLINE;
	} while(0);
}

void UpdateAI_PMC_TYPE1()
{
	char pRcvBuf[1000];
	unsigned short usDiskFail1, usDiskFail2;

	do {
		if(!RS232IO_Write(g_RAID_CMD_PMC_TYPE1, 2))
			break;
		Sleep(500);
		if(!RS232IO_Read(pRcvBuf))
			break;

		if(pRcvBuf[0] != 0x2A) { g_nCommStatus = CS_COMM_FAIL; break; }

		usDiskFail1 = pRcvBuf[3];
		if(usDiskFail1 >= 1)
				g_nDiskFail1 = HD_FAIL;
		else if(usDiskFail1 > 4)
				{ g_nCommStatus = CS_COMM_FAIL; break; }
		else
				g_nDiskFail1 = HD_OK;

		//	printf(">>>>>>>>> RAID Fail HDD 1 => %d\n",usDiskFail1);
			
		usDiskFail2 = pRcvBuf[4];
		if(usDiskFail2 >= 1)
				g_nDiskFail2 = HD_FAIL;
		else if(usDiskFail2 > 4)
				{ g_nCommStatus = CS_COMM_FAIL; break; }
		else
				g_nDiskFail2 = HD_OK;

		//	printf(">>>>>>>>> RAID Fail HDD 2 => %d\n",usDiskFail2);
		g_nCommStatus = CS_ONLINE;
	} while(0);
}

void UpdateAI_PMC_TYPE2()			// ¹Ì¿Ï¼º
{
	char pRcvBuf[1000];
	unsigned short usDiskFail1, usDiskFail2;

	do {
		if(!RS232IO_Write(g_RAID_CMD_PMC_TYPE2, 2))
			break;
		Sleep(500);
		if(!RS232IO_Read(pRcvBuf))
			break;

		if(pRcvBuf[0] != 0x2A) { g_nCommStatus = CS_COMM_FAIL; break; }

		usDiskFail1 = pRcvBuf[3];
		if(usDiskFail1 == '%')
				g_nDiskFail1 = HD_FAIL;
		else if(usDiskFail1 == 'X' || usDiskFail1 == 'Y')
				g_nDiskFail1 = HD_OK;
		else
				{ g_nCommStatus = CS_COMM_FAIL; break; }
		//	printf(">>>>>>>>> RAID Fail HDD 1 => %d\n",usDiskFail1);
			
		usDiskFail2 = pRcvBuf[4];
		if(usDiskFail2 == '%')
				g_nDiskFail2 = HD_FAIL;
		else if(usDiskFail2 == 'X' || usDiskFail2 == 'Y')
				g_nDiskFail2 = HD_OK;
		else
				{ g_nCommStatus = CS_COMM_FAIL; break; }
		//	printf(">>>>>>>>> RAID Fail HDD 2 => %d\n",usDiskFail2);
		g_nCommStatus = CS_ONLINE;
	} while(0);
}

void UpdateAI()
{

	do{
		switch(g_nRAIDType)					// 2010.01.20
		{
		case RT_CTC_TYPE1: UpdateAI_CTC_TYPE1(); break;
		case RT_PMC_TYPE1: UpdateAI_PMC_TYPE1(); break;
		case RT_PMC_TYPE2: UpdateAI_PMC_TYPE2(); break;
		}
	}while(0);
	
	return;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Thread

BOOL	g_bThreadRun = FALSE;
BOOL	g_bThreadFuncRun = FALSE;

void Polling_Thread()
{

	g_bThreadRun = TRUE;
	g_bThreadFuncRun = TRUE;

	Sleep(5000);

	while(g_bThreadRun)
	{
		UpdateAI();
		Sleep(1000);
	}
	g_bThreadFuncRun = FALSE;
	g_bThreadRun     = FALSE;
	_endthread();
}


double Analog_In( int ID1 , int ID2 , int ID3 , int *Result )
{
	*Result = FALSE;
	return 0;
}


void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result )
{
	*Result = FALSE;
}
//-----------------------------------------------------------------------------------

BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 )
{
	BOOL bRet = TRUE;
	sprintf(g_szDrvName, "%s[%d-%d]", DRIVER_NAME, ID1, ID9);

	do {
		if(ID10 <= 0) g_nTimeout = TIMEOUT_T;
		else g_nTimeout = ID10;
		g_nRAIDType = ID9;					// 2010.01.20

		if(! RS232_Connect_Port(ID1, ID2, ID3, ID4, ID5, ID6, ID7, ID8)) break;
		Sleep(1000);
		if(bRet == FALSE) break;
		_beginthread((void*)Polling_Thread, 0, NULL);
	} while(0);
	return bRet;
}

BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {

	g_bThreadRun = FALSE;
	Sleep(5000);
	RS232_Disconnect_Port();
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
