/* <St> *******************************************************************

  FILENAME      : DRIVER.C

  -------------------------------------------------------------------------
  CREATETED     : hjeon
  DATE          : 2002.07.25
  PROJECT       : device net driver (hilscher Co.)
  Copyright     : SAMSUNG Electronics Co.
  =========================================================================

  DISCRIPTION

    Device net card driver
    out filename  : hs_dnet.dll
  =========================================================================

  CHANGES

  version name        date        Discription
  -------------------------------------------------------------------------
  v0.0    hjeon      2002.07.25   - 1st released
  v2.0	  hasong     2010.05.11   - 2nd released
  v2.1	  tmjeon     2010.05.17   - 3nd released
  v2.2	  tmjeon     2010.06.11   - 4nd released
  v2.3	  tmjeon     2011.11.02   - 5nd released /Critical Section/
  ******************************************************************** <En> */

#include "driver.h"
#include "cifuser.h"      /* Include file for device driver API */
#include "rcs_user.h"     /* Include file for RCS definition */
#include <math.h>


/* definicions */
#define DEVICE_CONFIG_FILE  "Device/DeviceNet/DeviceNet.init"
#define LOG_FILE            "Device/DeviceNet/DNet.log"
#define BUFFER_SIZE         3584
#define MAX_ADDRESS         512
#define MAX_BIT             16

//2010.03.28
#define NUMBER_OF_EXPMSG	5
#define SERIAL_NUM			0
#define MANUFACTURER_NAME	1
#define MODEL_NUM			2
#define GAS_NAME			3
#define GAS_MAX_SIZE		4
#define NUMBER_OF_MFC		18



/* enumerations */
enum { DNET_DEAD, DNET_ALIVE };
//2010.01.22 For Explicit BY SHA
enum { IDLE , DOWNLOADING , QUEUEING};
enum { MAILBOX_EMPTY, MAILBOX_FULL };
enum { Thread_DEAD, Thread_ALIVE}; //[2010.05.18 For DNET Driver Optimization by JTM
/* global variables */
unsigned short usBoardNumber = 0;
unsigned char  abOutputData [BUFFER_SIZE];
unsigned char  abInputData  [BUFFER_SIZE];
WORD abBUFFER[MAX_ADDRESS];
int nAlive = DNET_DEAD;
int nLogging = 0;
long nTimeOut = 0;
BOOL INITFLAG = FALSE ;
BOOL ThreadFlag = Thread_DEAD; //[2010.05.18 For DNET Driver Optimization by JTM

//2010.01.22 For Explicit BY SHA
unsigned short usSetDeviceMacID      = 0; //Current Selected Mac ID Member
unsigned short usGetDeviceMacID      = 0; //Getting Device MacID
unsigned short usSetServiceCode      = 0 ; //Current Service Code Member
unsigned short usSetClassID          = 0 ; //Current Class ID Member
unsigned short usSetInstanceID       = 0 ; //Current Instance ID Member
unsigned short usGetServiceCode      = 0 ; //Getting Service Code Member
unsigned short usGetClassID          = 0 ; //Getting Class ID Member
unsigned short usGetInstanceID       = 0 ; //Getting Instance ID Member
unsigned short usSetAttributeID		 = 0 ; //Current Attribute ID 
unsigned short usGetAttributeID		 = 0 ; //Getting Attribute ID 
int            g_nCurrentPaketSize     = 0;
int            g_nBufferingStatus      = IDLE;
int            g_nCurrentReadDataSize  = 0;
int            g_nTotalReadCount       = 0; //just internal member

MSG_STRUC      g_tMsgSnd;
MSG_STRUC      g_tMsgRcv;

BYTE		   bMsgId = 0;
double		   value = 0;

//2010.03.27

char Serial_No[NUMBER_OF_MFC][1024];
char Manufacturer_Name[NUMBER_OF_MFC][1024];
char Model_No[NUMBER_OF_MFC][1024];
char Gas_Name[NUMBER_OF_MFC][1024];
char Gas_Max_Size[NUMBER_OF_MFC][1024];


BOOL SUCCESS_ExpMsg_FLAG = FALSE;
int Mac_ID ;
// [ 2010.05.20    Driver Version & Driver Build date ADD By SHA
char 	DRV_VERSION[50]	=	"DNET_MFC_DRV_Ver. 2.3";
char 	DRV_BUILDDATE[50] =	"2011.11.02";
// ]

//2010.03.27

//-----------------------------------------------------------------------------------
//--- For File Logging  2008/03/28 --------------------------------------------------
//-----------------------------------------------------------------------------------

BOOL	bDataLog = FALSE ;
INT64	LastPollingTime = 0;
char    LogString[256];

#define STATUS_UPDATE_INTERVAL	( 50 * 10000 ) //300msec
enum { IO_STATUS , IO_REQUEST , IO_ACTION , IO_SETTING };


//-----------------------------------------------------------------------------------

//--> 2011/06/02 CriticalSection by JTM
CRITICAL_SECTION g_cs;
HANDLE hThread;

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
/* initialize DeviceNet card */
BOOL InitDNetCard(void)
{
    short sRet = 0;
    BOARD_INFO  tBoardInfo;

    // Open the driver
    if ( (sRet = DevOpenDriver(0)) != DRV_NO_ERROR) {
        printf( "ERROR: [DNet] DevOpenDriver       RetWord = %5d\n", sRet );
        return FALSE;
    }

    // get board information
    if ( (sRet = DevGetBoardInfo(0, sizeof(tBoardInfo), &tBoardInfo)) != DRV_NO_ERROR ) 
    {
        DevCloseDriver(0);
	    printf("ERROR: [DNet] DevGetBoardInfo(0, sizeof(BOARD_INFO), %p): %d\n", tBoardInfo, sRet);
        return FALSE;
    }

    if (tBoardInfo.tBoard[0].usAvailable == 1)
    {
        // initialize board
        usBoardNumber = tBoardInfo.tBoard[0].usBoardNumber;

        if ( (sRet = DevInitBoard (usBoardNumber, NULL)) != DRV_NO_ERROR) {
            DevCloseDriver(0);
            printf( "ERROR: [DNet] DevInitBoard        RetWord = %5d\n", sRet );
            return FALSE;
        }

        // clear in/out buffers
		// ----------------------------------------------------------------------------
		// K.H.K ( 2003.1.8 ) - For don't initialze the DeviceNet 
		// ----------------------------------------------------------------------------
		if ( !INITFLAG ) { 
			printf("======> DeiviceNet INITFLAG :: FALSE\n");
			memset(abOutputData	, 0	, BUFFER_SIZE);
			memset(abInputData	, 0	, BUFFER_SIZE);
			memset(abBUFFER		, 0	, MAX_ADDRESS);
		}
		// ----------------------------------------------------------------------------
		
		// 2006/11/07 CNLee Add
        if ( (sRet = DevReset( usBoardNumber, COLDSTART , 5000 ) ) != DRV_NO_ERROR ) {
            DevExitBoard(usBoardNumber);
            DevCloseDriver(0);
            printf( "ERROR: [DNet] DevReset (COLDSTART) RetWord = %5d\n", sRet );
            return FALSE;
        }
 		
        // Signal board, application is running
         if ( (sRet = DevSetHostState(usBoardNumber, HOST_READY, 0L)) != DRV_NO_ERROR ) {
             DevExitBoard(usBoardNumber);
             DevCloseDriver(0);
             printf( "ERROR: [DNet] DevSetHostState (HOST_READY) RetWord = %5d\n", sRet );
             return FALSE;
         }
        return TRUE;
    }

    DevCloseDriver(0);
    printf("ERROR: [DNet] DeviceNet master board is not found!\n");

	return FALSE;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
/* disconnet with device net card: close driver */
BOOL Discon_DNetCard(void)
{
    short sRet = 0;
    BOOL bRet = TRUE;


    // Signal board, application is not running
    if ( (sRet = DevSetHostState( usBoardNumber, HOST_NOT_READY, 0L )) 
        != DRV_NO_ERROR ) {
        printf( "ERROR: [DNet] DevSetHostState (HOST_NOT_READY) RetWord = %5d\n", sRet );
        bRet = FALSE;
    }


   // Close communication
    if ( (sRet = DevExitBoard( usBoardNumber )) != DRV_NO_ERROR ) {
        printf( "ERROR: [DNet] DevExitBoard  RetWord = %5d\n", sRet );
        bRet = FALSE;
    }

    // Close Driver
    if ( (sRet = DevCloseDriver(0)) != DRV_NO_ERROR ) {
        printf( "ERROR: [DNet] DevCloseDriver  RetWord = %5d\n", sRet );
        bRet = FALSE;
    }

    nAlive = DNET_DEAD;
    return bRet;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//[2010.05.18 DEL For DNET Driver Optimization by JTM
/* read datum from device net card 
BOOL ReadIO(void)
{
    short sRet = 0;
	
    // rcvoffset = 0, rcvbufsize = sizeof(abInputData)
    sRet = DevExchangeIO(usBoardNumber, 0, 0, NULL,
            0, sizeof(abInputData), &abInputData, nTimeOut);
            

    if ( sRet != DRV_NO_ERROR )
    {
        nAlive = DNET_DEAD;
        printf("ERROR: [DNet] ReadIO             RetWord = %5d\n", sRet);
        return FALSE;
    }
    nAlive = DNET_ALIVE;
    return TRUE;
}
*/
//]

//[2010.05.18 ReadIO() Modify For DNET Driver Optimization by JTM
/* read datum from device net card */
unsigned __stdcall ReadIO(void)
{
    short sRet = 0;
	
    while(ThreadFlag)
	{
		//--> 2011/06/02 CriticalSection by JTM
		EnterCriticalSection(&g_cs);
		sRet = 0;

		sRet = DevExchangeIO(usBoardNumber, 0, 0, NULL,
				0, sizeof(abInputData), &abInputData, nTimeOut);	
            

		if ( sRet != DRV_NO_ERROR )
		{
			nAlive = DNET_DEAD;
			printf("ERROR: [DNet] ReadIO             RetWord = %5d\n", sRet);		
		}
		nAlive = DNET_ALIVE;

		LeaveCriticalSection(&g_cs);

		_sleep(10);
	}

	return 0;   
}
//]


/* write data to device net card */
BOOL WriteIO(void)
{
    short sRet = 0;

    // sndoffset = 0, sndbufsize = sizeof(abOutputData)

	//--> 2011/06/02 CriticalSection by JTM
	EnterCriticalSection(&g_cs);
	sRet = 0;
    sRet = DevExchangeIO(usBoardNumber,
            0, sizeof(abOutputData), &abOutputData,
            0, 0, NULL, nTimeOut);
	

    if ( sRet != DRV_NO_ERROR )
    {
        nAlive = DNET_DEAD;
        printf("ERROR: [DNet] WriteIO             RetWord = %5d\n", sRet);
        return FALSE;
    }
    nAlive = DNET_ALIVE;
	LeaveCriticalSection(&g_cs);
    return TRUE;
}

/* setup io type for the device net module's I/O type of Allen-Bradley Co. */
BOOL SetIOType(void)
{
    BOOL bSet = FALSE;
    char sBuf[256] , sTmp[81];
	int nIdx = 0, nSet = 0, ret = 0;
	FILE *fp;

	if ( ( fp = fopen( DEVICE_CONFIG_FILE , "r" ) ) == NULL ) {
		printf( "WARN: [DNet] Cannot Find %s File\n" , DEVICE_CONFIG_FILE );
		return TRUE;
	}

    fseek( fp, 0L, SEEK_SET );
    while ( (ret = fscanf( fp, "%s", sTmp )) != 0 && ret != EOF )
    {
		fgets( sBuf , 255 , fp );
		if ( sTmp[0] == '#' ) continue;
        bSet = TRUE;
		sscanf( sBuf , "%d %X", &nIdx , &nSet );
		*(abOutputData + nIdx) = nSet;
	}

    fclose(fp);
    if (bSet) {
        if (!WriteIO()) {
            printf( "ERROR: [DNet] Module I/O type setup was failed\n");
            return FALSE;
        }
    }
    return TRUE;
}

/* device net driver logging function
        if ID2 != 0, then logging is enabled
*/
void DNet_Logging(char *msg)
{
    char buf[80];
    struct tm *newtime;
    time_t aclock;
    FILE *fp;

    if (nLogging != 0)
    {
        if ( !(fp = fopen( LOG_FILE , "a+" )) ) return;

        // get system time
        time( &aclock );
        newtime = localtime( &aclock );
        sprintf(buf, "%s: ", asctime( newtime ));
        buf[24] = ' ';

        // write message to LOG_FILE
        fputs(buf, fp);
        fputs(msg, fp);
        fputs( "\n", fp );
        fclose(fp);
    }
}

//File Logging 기능
void TimeLog( char* str ) {
	FILE *fp = NULL;
	char Path[MAX_PATH] , FileName[64] , DirPath[MAX_PATH];
	SYSTEMTIME st;
	BOOL CreateDirOK = FALSE;
	
	bDataLog = TRUE ;
	
	if( !bDataLog ) return;
	
	GetLocalTime( &st );
	sprintf( FileName , "%04d%02d%02d%02d.log" , st.wYear , st.wMonth , st.wDay , st.wHour );
	sprintf( Path , "DATALOG\\MFC_LOG\\MFC1\\%s" ,  FileName );
	fp = fopen( Path , "a+" );
	
	//---> ADD 2008/09/18  Logging폴더 자동생성  by KSM -> 보완 (Side 구별 & 예외처리) 2008/09/22
	if( !fp ) { 
		
		sprintf( DirPath , "DATALOG/MFC_LOG/MFC1" ); 
		
		CreateDirectory ( "DATALOG/MFC_LOG" , NULL );
		CreateDirOK = CreateDirectory ( DirPath , NULL );
		
		if ( !CreateDirOK ) { printf( "[DNET_DRIVER] : Can't Create Log File\n"  ); return; }
		
		fp = fopen( Path , "a+" );
	}
	//---> End 2008/09/18  Logging폴더 자동생성  ( 1 of 1 )
	
	fprintf( fp , "%04d/%02d/%02d %02d:%02d:%02d:%03d\t" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond , st.wMilliseconds );
	fprintf( fp , "[%s]\n" , str );
	
	
	fclose( fp );
}
/* read word data from input buffer */
WORD ReadWordData(int nOffset)
{
    WORD nRet = 0;

    // read upper byte from read buffer
    nRet = *(abInputData + nOffset + 1);
    // read lower byte from read buffer
    nRet = (nRet << 8) + *(abInputData + nOffset);
    return nRet;
}

short ReadWordData2(int nOffset)
{
    short nRet = 0;

    // read upper byte from read buffer
    nRet = *(abInputData + nOffset + 1);
    // read lower byte from read buffer
    nRet = (nRet << 8) + *(abInputData + nOffset);
    return nRet;
}

/* write byte data to output buffer */
void WriteByteData(int nOffset, BYTE nData)
{
    *(abOutputData + nOffset) = nData;
}

/* write word data to output buffer */
void WriteWordData(int nOffset, WORD nData)
{
    // write lower byte to buffer
    *(abOutputData + nOffset) = (BYTE)(nData & 0xff);
    // write upper byte to buffer
    *(abOutputData + nOffset + 1) = (BYTE)(nData >> 8);
}

/* get word data form write buffer for DO func. */
WORD GetWordDataFromWRBuf(int nOffset)
{
    WORD nRet = 0;

    // get upper byte form write buffer
    nRet = *(abOutputData + nOffset + 1);
    // get lower byte from write buffer
    nRet = (nRet << 8) + *(abOutputData + nOffset);
    return nRet;
}
BYTE GetMessageId(void)
{
	++bMsgId;
	if(bMsgId == 255)
		bMsgId = 0;
	
	return bMsgId;
}

//-------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------  IEEE 754 Conversion ----------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void HEX_DI_Conversion( double *result , unsigned char HEX1 , unsigned char HEX2 , unsigned char HEX3 , unsigned char HEX4 )
{  // HEX->Digital Conversion.........................................................
	
	unsigned int  c[8],d[8],e[8],j,HEX01;
	double		  both,HEX10,HEX20,HEX30,HEX40;
	int			  i;
	  
	unsigned char HEX02,HEX03,HEX04,tmpHex1;   
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


//2010.01.22 For Explicit BY SHA
//-----------------------------------------------------------------------------------------------------------
//Name        : InitializeMessageBuffer
//Date        : 2010.01.22
//Return      : void
//Description : Just before putmessage message header constructed here
//-----------------------------------------------------------------------------------------------------------
void InitializeMessageBuffer(void)
{


	memset(&g_tMsgSnd , 0 , sizeof(MSG_STRUC));

	//Explicit Message Header//////////////////////
	g_tMsgSnd.rx = 3;  
	g_tMsgSnd.tx = 16;
	g_tMsgSnd.ln = (BYTE)g_nCurrentPaketSize + 8;
	g_tMsgSnd.nr = GetMessageId();
	g_tMsgSnd.a  = 0; 
	g_tMsgSnd.f  = 0;
	g_tMsgSnd.b  = 79; 
	g_tMsgSnd.e  = 0;
	////////////////////////////////////////////////

    //Telegram Header////////////////////////////////////////////////////////////////////
	g_tMsgSnd.data[0] = (BYTE)usSetDeviceMacID;                          // mac id
	g_tMsgSnd.data[1] = (BYTE)usSetClassID;		                       // class id
	g_tMsgSnd.data[2] = (BYTE)usSetInstanceID;	                       // instance id - L
	g_tMsgSnd.data[3] = 0x00;											// instance id - H
	g_tMsgSnd.data[4] = 0x00;
	g_tMsgSnd.data[5] = (BYTE)g_nCurrentPaketSize;		                   // data length
	g_tMsgSnd.data[6] = 0x00;
	g_tMsgSnd.data[7] = (BYTE)usSetServiceCode ;		                   // service code
	g_tMsgSnd.data[8] = (BYTE)usSetAttributeID ;		                   // service code
	/////////////////////////////////////////////////////////////////////////////////////

	sprintf( LogString, "Telegram Header Data : %d %d %d %d %d %d %d %d \n Send Message : %d %d %d %d %d %d %d %d %d", g_tMsgSnd.rx, g_tMsgSnd.tx, g_tMsgSnd.ln, g_tMsgSnd.nr, g_tMsgSnd.a, g_tMsgSnd.f, g_tMsgSnd.b, g_tMsgSnd.e,
						 g_tMsgSnd.data[0], g_tMsgSnd.data[1], g_tMsgSnd.data[2], g_tMsgSnd.data[3], g_tMsgSnd.data[4], g_tMsgSnd.data[5], g_tMsgSnd.data[6], g_tMsgSnd.data[7], g_tMsgSnd.data[8]);
	TimeLog( LogString ); 
	
}

//-----------------------------------------------------------------------------------------------------------
//Name        : DownLoading
//Date        : 2010.01.22
//Return      : void
//Description : PutMessage with constructed message & data
//-----------------------------------------------------------------------------------------------------------
BOOL DownLoading(void)
{
	int            sRet;
	int            nTimeOut;
	unsigned short usDevState;
	unsigned short usHostState;	
	int i =0;

	sRet = 0;
	nTimeOut = 0;

		sRet = DevGetMBXState(0, &usDevState, &usHostState);

		if(sRet != DRV_NO_ERROR)
		{
			printf("Error : [DNet] DownLoading->DevGetMBXState got invalid result \n");
			return FALSE;
		}
		if(usDevState == MAILBOX_EMPTY);

		else{
			printf("Error : [DNet] DownLoading->DevGetMBXState got timeout \n");
			sprintf( LogString, " usHostState : %d       Error : [DNet] DownLoading->DevGetMBXState got timeout\n ", usHostState) ;
			TimeLog( LogString );
			return FALSE;			
	}
			
	sRet = DevPutMessage(0, &g_tMsgSnd, 500);
	
	if(sRet != DRV_NO_ERROR)
	{
		printf("Error : [DNet] DownLoading->DevPutMessage got invalid result!! \n");
		return FALSE;
	}
	else
		//printf("[DNet] DevPutMessage success!! \n");

	g_nBufferingStatus = IDLE;

   return TRUE;
}

//-----------------------------------------------------------------------------------------------------------
//Name        : UpLoading
//Date        : 2010.01.22
//Return      : BOOL
//Description : GetMessage & update result
//-----------------------------------------------------------------------------------------------------------
BOOL UpLoading(void)
{
	int            i=0;
	int            sRet;
	int            nTimeOut;
	unsigned short usDevState;
	unsigned short usHostState;

	memset(&g_tMsgRcv , 0 , sizeof(MSG_STRUC));
	usGetDeviceMacID     = 0;
	usGetClassID         = 0;
	usGetInstanceID      = 0;
	usGetAttributeID     = 0;
	usGetServiceCode     = 0;
	g_nCurrentReadDataSize = 0;
	g_nTotalReadCount      = 0;

	sRet = 0;
	nTimeOut = 0;
	
		sRet = DevGetMBXState(0, &usDevState, &usHostState);
		if(sRet != DRV_NO_ERROR)
		{
			sprintf( LogString, "%d    Error : [DNet] UpLoading->DevGetMBXState got invalid result ", sRet);
			TimeLog( LogString );
			return FALSE;
		}
		if(usHostState != MAILBOX_EMPTY){
			sprintf( LogString, " Normal status - usHostState : %d"  , usHostState) ;
			TimeLog( LogString );
		}

		else{
			sprintf( LogString, " fail1 - usHostState : %d       Error : [DNet] UpLoading->DevGetMBXState got timeout 1 \n ", usHostState) ;
			TimeLog( LogString );
			_sleep(30);
			sRet = DevGetMBXState(0, &usDevState, &usHostState);
			if(usHostState != MAILBOX_EMPTY){
				sprintf( LogString, " success - usHostState : %d       Error : [DNet] UpLoading->DevGetMBXState got timeout 1 \n ", usHostState) ;
				TimeLog( LogString );
			}
			else{
				sprintf( LogString, " fail2 - usHostState : %d       Error : [DNet] UpLoading->DevGetMBXState got timeout 2 \n ", usHostState) ;
				TimeLog( LogString );
				return FALSE;
			}
		}			 

	sRet = DevGetMessage(0, sizeof(MSG_STRUC), &g_tMsgRcv, 500);
	if(sRet != DRV_NO_ERROR)
	{
		sprintf( LogString, "%d      Error : [DNet] UpLoading->DevGetMessage got invalid result!! \n", sRet);
		TimeLog( LogString );
		return FALSE;
	}
	else
	{

		if( (g_tMsgRcv.a != 79) || (g_tMsgRcv.f != 0)
			|| (g_tMsgRcv.data[0] != g_tMsgSnd.data[0])	//MAC ID
			|| (g_tMsgRcv.data[1] != g_tMsgSnd.data[1])	//Class ID
			|| (g_tMsgRcv.data[2] != g_tMsgSnd.data[2])	//Instance ID low
			|| (g_tMsgRcv.data[3] != g_tMsgSnd.data[3])	//Instance ID high
			|| (g_tMsgRcv.data[7] != g_tMsgSnd.data[7]) 	// function code
			|| (g_tMsgRcv.nr != g_tMsgSnd.nr ))

		{
			//printf( " ERROR : [DNET] DI : GetMessage wrong!! \n ");
			return FALSE;
		}

		else
		{
			//printf("[DNet] UpLoading->DevGetMessage success.. \n");

			usGetDeviceMacID     = g_tMsgRcv.data[0];
			usGetClassID         = g_tMsgRcv.data[1];

			usGetInstanceID      = g_tMsgRcv.data[3];
			usGetInstanceID      = usGetInstanceID << 8; //shift high byte
			usGetInstanceID      = usGetInstanceID | g_tMsgRcv.data[2]; // add low byte
			usGetAttributeID	 = g_tMsgRcv.data[8];
			usGetServiceCode     = g_tMsgRcv.data[7];
			g_nCurrentReadDataSize = g_nTotalReadCount = g_tMsgRcv.data[5];

			sprintf( LogString, "Getting Telegram Header Data : %d %d %d %d %d %d %d %d \n Get Message : %d %d %d %d %d %d %d %d %d", g_tMsgRcv.rx, g_tMsgRcv.tx, g_tMsgRcv.ln, g_tMsgRcv.nr, g_tMsgRcv.a, g_tMsgRcv.f, g_tMsgRcv.b, g_tMsgRcv.e,
				g_tMsgRcv.data[0], g_tMsgRcv.data[1], g_tMsgRcv.data[2], g_tMsgRcv.data[3], g_tMsgRcv.data[4], g_tMsgRcv.data[5], g_tMsgRcv.data[6], g_tMsgRcv.data[7], g_tMsgRcv.data[8]);
			TimeLog( LogString ); 


			/*_strtime( Tbuffer );
			GetSystemTime( &st ) ;
			printf( " [%s.%3d]  ", Tbuffer , st.wMilliseconds );
			printf("Getting Telegram Header Data : ");

			printf("%d %d %d %d %d %d %d %d ", g_tMsgRcv.rx, g_tMsgRcv.tx, g_tMsgRcv.ln, g_tMsgRcv.nr, g_tMsgRcv.a, g_tMsgRcv.f, g_tMsgRcv.b, g_tMsgRcv.e);
 			for(i = 0 ; i < 9 ; i++)
 				printf("%d " , g_tMsgRcv.data[i]);*/
		}
	}	
	g_nBufferingStatus = IDLE;
	return TRUE;
}
BOOL ExplicitMessage (void)
{
	short ret =0;
	int i = 0;
	g_nBufferingStatus = QUEUEING;
	
	InitializeMessageBuffer();
	
	if(!DownLoading())
	{
		//printf( "ERROR : [DNet] DO : Putmessage fail! \n"); 
		
		g_nCurrentPaketSize = 0;
		g_nBufferingStatus  = IDLE;		
		return FALSE;
	}
	
	g_nCurrentPaketSize = 0;
	
	_sleep(100);

	if(!UpLoading())
	{
		sprintf( LogString, "Get Message fail : %d %d %d %d %d %d %d %d \n Get Message : %d %d %d %d %d %d %d %d %d", 
							g_tMsgRcv.rx, g_tMsgRcv.tx, g_tMsgRcv.ln, g_tMsgRcv.nr, g_tMsgRcv.a, g_tMsgRcv.f, 
							g_tMsgRcv.b, g_tMsgRcv.e,
							g_tMsgRcv.data[0], g_tMsgRcv.data[1], g_tMsgRcv.data[2], g_tMsgRcv.data[3], 
							g_tMsgRcv.data[4], g_tMsgRcv.data[5], g_tMsgRcv.data[6], g_tMsgRcv.data[7], 
							g_tMsgRcv.data[8]);
		TimeLog( LogString ); 
		printf( "ERROR : [DNet] DI : GetMessage fail! \n");
		//Get Message Fail시 다시한번 시도 하여 Get Message 
		
		
		DownLoading();
		_sleep(50);
		g_nCurrentPaketSize = 0;
		if(!UpLoading()){
			return FALSE;
		}
		
	}

	if(0 == g_nCurrentReadDataSize)
	{
		//printf( "ERROR : [DNet] DI : There is no data to read!! \n"); 
		return FALSE;
	}
	return TRUE;
}



BOOL Explicit_Msg_ALL_Read(void)
{
	char nData[1024] , nData2[1024];
	int i, j;
	char temp_data_1[1024];
	char temp_data_2[1024];
	char temp_data_3[1024];
	char temp_data_4[1024];
	char temp_data_5[1024];
	double	value = 0;
	int k; //2010.05.14 EXPMSG Check Routine Modify by JTM
	memset( temp_data_1 , 0x00 , sizeof(temp_data_1));
	memset( temp_data_2 , 0x00 , sizeof(temp_data_2));
	memset( temp_data_3 , 0x00 , sizeof(temp_data_3));
	memset( temp_data_4 , 0x00 , sizeof(temp_data_4));
	memset( temp_data_5 , 0x00 , sizeof(temp_data_5));

	
	//[2010.05.14 EXPMSG Check Routine Modify by JTM : 
	//    수정내용 : N2O MFC (8,9번 - 미장착 MFC) 를 가장 마지막에서 Check 하도록 수정함.  
	//    배  경   : N2O MFC(Hitachi) 수급 문제로 인해,   N2O MFC 미 장착 상태에서  납품 진행되고 있음.(U2L#18, MP#1 Start)
	//               차후 N2O MFC 장착시  S/W 수정이 불필요하도록 임시적으로 수정 필요하여 수정 진행.
    //    참고자료 :  Hynix Daily Report ( 2010.04.01 일자)  참고
	//                a)현 상 : Sycon으로 DNET Module Scanning하여 인식할 때 1~17개의 Module이 연속적으로 인식이 되어 잡혀있는 경우 
	//                          일부 MFC를 Disconnect하면 Disconnect된 MFC를 제외하고는 정상적으로  Explicit Message 통신을 진행
	//                          연속적이지 않게 잡혀 있는 경우 ( ex. #5, #8, #10 )에 일부 MFC를 Disconnect 하면 Disconnect된 MFC 이후의 Explicit Message 통신 불가
	//                          (Address할당으로 받아지는 Set/Flow/Pressure/Temp/Valve 값은 해당사항 없음 -> Explicit Message를 통해 받는 Data만 해당)
	//                b)해결방안 : Sycon으로 DNET Module을 Auto Scanning하여 1~17개의 Module이 연속적으로 인식되어 있는 것을 확인할 것
	
	//for(Mac_ID=4; Mac_ID<NUMBER_OF_MFC; Mac_ID++){
	for(k=4; k<NUMBER_OF_MFC+2; k++){
	
			if(k == 8 || k == 9) continue;

			Mac_ID = k;

			if(k == 18) Mac_ID = 8;
			else if(k == 19) Mac_ID = 9;
	//]      
			
	
			for(j =0; j<NUMBER_OF_EXPMSG; j++){
				usSetDeviceMacID = (int)Mac_ID;		 // MACID 
				usSetServiceCode = 0x0E;				 // Service Code
				usSetInstanceID = 0x01;					 // InstanceID 
				
					g_nCurrentPaketSize = 0x01;
					if( j == SERIAL_NUM ){ //Serial No	
						usSetAttributeID = 0x09; // Attribute ID
						usSetClassID = 0x30;     // Class ID
					}
					else if( j == MANUFACTURER_NAME ){ //Manufacturer Name	
						usSetAttributeID = 0x05; // Attribute ID
						usSetClassID = 0x30;     // Class ID
					}
					else if( j == MODEL_NUM ){ //Model No
						usSetAttributeID = 0x06; // Attribute ID
						usSetClassID = 0x30;     // Class ID
					}
					else if( j == GAS_NAME ){ //Gas Name
						usSetAttributeID = 0x05; // Attribute ID
						usSetClassID = 0x34;     // Class ID
					}
					else if ( j == GAS_MAX_SIZE ){ //Gas Max Size
						usSetAttributeID = 0x06; // Attribute ID
						usSetClassID = 0x34;     // Class ID
					}
					else break;

					if(!ExplicitMessage())
					{
						printf( "Explicit Communication Error! \n" ); 
					}
					else
					{
						memset( nData , 0x00 , sizeof(nData));
						memset( nData2 , 0x00 , sizeof(nData2));

						for(i =9; i < 9 + g_nTotalReadCount; i++)
						{
							sprintf ( nData2 , "%c" , g_tMsgRcv.data[i] ) ;
							strcat ( nData , nData2 ) ;								
						}
						if( j == SERIAL_NUM ) {
							sprintf ( temp_data_1 , nData );
							strcpy ( Serial_No[Mac_ID], temp_data_1);
							printf( " [MacID] : %d / [Serial No] : %s \n", Mac_ID, Serial_No[Mac_ID] );
						}
						else if( j == MANUFACTURER_NAME ){
							sprintf ( temp_data_2 , nData );
							strcpy ( Manufacturer_Name[Mac_ID], temp_data_2);
							printf( " [MacID] : %d / [Manufacturer Name] : %s \n", Mac_ID, Manufacturer_Name[Mac_ID] );

						} 
						else if( j == MODEL_NUM ) {
							sprintf ( temp_data_3 , nData );
							strcpy ( Model_No[Mac_ID], temp_data_3);
							printf( " [MacID] : %d / [Model No] : %s \n", Mac_ID, Model_No[Mac_ID] );

						} 
						else if( j == GAS_NAME ) {
							sprintf ( temp_data_4 , nData );
							strcpy ( Gas_Name[Mac_ID], temp_data_4);
							printf( " [MacID] : %d / [Gas Name] : %s \n", Mac_ID, Gas_Name[Mac_ID] );

						} 
						else if( j == GAS_MAX_SIZE) //GAS MAX SIZE 
						{							
							HEX_DI_Conversion(&value, (unsigned char) g_tMsgRcv.data[11], (unsigned char)g_tMsgRcv.data[10], (unsigned char)g_tMsgRcv.data[9], (unsigned char)g_tMsgRcv.data[8]);
							sprintf ( temp_data_5 , "%.0f" , value  ) ;
							strcpy ( Gas_Max_Size[Mac_ID], temp_data_5);
							printf ( " [MacID] : %d / [Gas Max Size] : %s \n", Mac_ID, Gas_Max_Size[Mac_ID] );

 						}
						else break;

					}

			}

	}

	SUCCESS_ExpMsg_FLAG = TRUE;

	return SUCCESS_ExpMsg_FLAG;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
/* device attach function */
BOOL Device_Attach( char *name, char *file, int id1, int id2,
    int id3, int id4, int id5, int id6, int id7, int id8, int id9, int id10)
{

    // ID1 -> set read/write timeout
    // ID2 -> logging no(0)/yes(not 0)?
    if (id1 <= 10) nTimeOut = 100L;
    else nTimeOut = id1;

    nLogging = id2;

    // initialize deviceNet card & make buffers
    if (!InitDNetCard()) return FALSE;
    _sleep(500);
	/*
    if (!SetIOType()) {
        Discon_DNetCard();
        return FALSE;
    }*/
    
    DNet_Logging("Device attached!");
    //printf("[DNet] Device attached!\n");
    nAlive = DNET_ALIVE;
	INITFLAG = TRUE ;
    // add delay
    _sleep(1000);

	printf(" [ATTO] DeviceNet Driver Start !!! \n ");

	//2010.03.27
	_sleep(2000);
	memset( Serial_No , 0x00 , sizeof(Serial_No));
	memset( Manufacturer_Name , 0x00 , sizeof(Manufacturer_Name));
	memset( Model_No , 0x00 , sizeof(Model_No));
	memset( Gas_Name , 0x00 , sizeof(Gas_Name));
	memset( Gas_Max_Size , 0x00 , sizeof(Gas_Max_Size));
	//2010.07.27 Del For CTC Dnet Version
	//Explicit_Msg_ALL_Read();
	//printf("DeviceNet MFC COMM OPEN SUCCESS...[Ver:%s / Rebuild_Date:%s] \n" , DRV_VERSION, DRV_BUILDDATE );
	printf("DeviceNet for CTC COMM OPEN SUCCESS...[Ver:%s / Rebuild_Date:%s] \n" , DRV_VERSION, DRV_BUILDDATE );

	//--> 2011/06/02 CriticalSection by JTM
	InitializeCriticalSection(&g_cs);

	//[2010.05.18 For DNET Driver Optimization by JTM
	ThreadFlag = Thread_ALIVE;
	hThread = (HANDLE)_beginthreadex( NULL, 0, ( unsigned int (_stdcall *)(void *) )ReadIO, 0, 0, NULL ); //쓰레드 생성
	printf("DeviceNet Data Gathering Routine Start...\n");
    //]

    return TRUE;
}

/* device detach function */
BOOL Device_Detach( char *name, char *file, int id1, int id2,
    int id3, int id4, int id5, int id6, int id7, int id8, int id9, int id10)
{
    DNet_Logging("Device detached!");
	//[2010.05.18 For DNET Driver Optimization by JTM
	ThreadFlag = Thread_DEAD;	//쓰레드 종료
	_sleep(1000);

	//--> 2011/06/02 CriticalSection by JTM
	CloseHandle(hThread);
	DeleteCriticalSection(&g_cs);

    return Discon_DNetCard();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Gauge Data convert crkim add */
double BitToDecimal_1( WORD nData, int id2 ){ 
	int i,j, zisu=0;
	unsigned char temp1[5], temp2[5];
	short int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128};
	double data=0;

	for(i=0; i<5; i++) temp1[i]=temp2[i]=0;
/*
--->zisu,temp1    ::[9][136]
--->nData   ::[443f][4440]
--->temp1-2 ::[88][3f]
--->j-data ::[0][512.000000]
--->j-data ::[2][640.000000] 2*[7]
--->j-data ::[3][704.000000] 2*[6]
--->j-data ::[4][736.000000] 2*[5]
--->j-data ::[5][752.000000] 2*[4]
--->j-data ::[6][760.000000] 2*[3]
--->j-data ::[7][764.000000] 2*[2]
--->return data ::[764.000000]
*/

	*temp1 = (BYTE)(nData >> 7);	//지수승,
	temp1[1]=0;
	zisu   = (int)temp1[0] - 127;
	*temp2 = (BYTE)(nData);		//value(0-6bit),
	temp2[1]=0;

//	printf("\n--->zisu,temp1    ::[%d][%d]\n",zisu, (int)temp1[0] );
//	printf("--->nData   ::[%x][%x]\n",nData, nData+1 );
//	printf("--->temp1-2 ::[%x][%x]\n",temp1[0],temp2[0]   );

	j=1;
	data = pow(2,zisu);

//	printf("--->j-data ::[%d][%f]\n",0,data   );

	for(i=6; i>=0; i--) {
		if ( temp2[0] & mask[i] ) {
			data = data + pow(2,zisu-j);
//			printf("--->j-data ::[%d][%f] 2*[%d] \n",j,data , zisu-j  );
		}
		j++;
	}

	//printf("--->return data ::[%f]\n",data   );

	/////// 잔여 data read
	nData = ReadWordData(id2);

	*temp1 = (BYTE)(nData >> 8);	//상위 바이트,
	temp1[1]=0;
	*temp2 = (BYTE)(nData);			//하위 바이트
	temp2[1]=0;

	for(i=7; i>=0; i--) {
		if ( temp1[0] & mask[i] ) {
			data = data + pow(2,zisu-j);
			//printf("===>j-data ::[%d][%f] 2*[%d] \n",j,data , zisu-j  );
		}
		j++;
	}

	for(i=7; i>=0; i--) {
		if ( temp2[0] & mask[i] ) {
			data = data + pow(2,zisu-j);
			//printf("===>j-data ::[%d][%f] 2*[%d] \n",j,data , zisu-j  );
		}
		j++;
	}

	return data;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* analog in function */
double Analog_In(int id1, int id2, int id3, int *result)
{
    int nData  = 0;
	double Press= 0;	

    if (id1 == 0) {
        //if (ReadIO()) { 2010.05.18 DEL For DNET Driver Optimization by JTM
		//Data Gathering Thread 에서 Data 를 주기적으로 Update 하기 때문에,
		//별도로 ReadIO() Routine 을 호출할 필요가 없음. 
            nData = (int)ReadWordData2(id2);
            *result = TRUE;
			//printf ( "HS_DNET Analog IN : Address [%d] , Data [%d]\n" , id2 , nData );
            return (double)nData;
        //} 2010.05.18 DEL For DNET Driver Optimization by JTM
    }
	/* Gauge Data convert crkim add(id1==10) */
    else if (id1 == 10) { 
        //if (ReadIO()) { 2010.05.18 DEL For DNET Driver Optimization by JTM
            nData = (int)ReadWordData(id2+2);
			//Press = BitToDecimal(nData);

			Press = (double)BitToDecimal_1((WORD)nData, id2);	//상위 2바이트
			
            *result = TRUE;
            return Press;
        //} 2010.05.18 DEL For DNET Driver Optimization by JTM
		//else printf("KCR ERROR: [DNet] AI - Map number(ID1) is invalid\n"); 2010.05.18 DEL For DNET Driver Optimization by JTM
    }  
    else printf("ERROR: [DNet] AI - Map number(ID1) is invalid\n");
    *result = FALSE;
    return 0;
}

/* analog out function */
void Analog_Out(int id1, int id2, int id3, double data, int *result)
{

    if (id1 == 1) {
        WriteWordData(id2, (WORD)data);
        if (WriteIO()) {
            *result = TRUE;		
            return;
        }
    }
    else printf("ERROR: [DNet] AO - Map number(ID1) is invalid\n");
    *result = FALSE;
}

int RemoveNotNeedData(WORD wData, int nLow, int nHigh)
{
    // remove upper bit's data
    wData <<= 15 - nHigh;
    // remove lower bit's data
    wData >>= 15 - nHigh + nLow;
    return (int)wData;
}

/* digital in function */
int Digital_In(int id1, int id2, int id3, int id4, int *result)
{


	switch (id1)
    {
    case 0:
        if ( (id3 <= id4) && (id3 >= 0) && (id3 <= 15) && (id4 >= 0) && (id4 <= 15) )
        {
            //if (ReadIO()) {  2010.05.18 DEL For DNET Driver Optimization by JTM
                *result = TRUE;				

                return RemoveNotNeedData( ReadWordData(id2), id3, id4);
            //}  2010.05.18 DEL For DNET Driver Optimization by JTM
        }
        else printf( "ERROR: [DNet] DI: IO bit Range Error - [%d]-[%d]\n", id3, id4 ); 
        break;
    case 2:
        if ( (id2 >= 0) && (id2 < MAX_ADDRESS) )
        {
            if ( (id3 <= id4) && (id3 >= 0) && (id3 <= 15) && (id4 >= 0) && (id4 <= 15))
            {
                *result = TRUE;
                return RemoveNotNeedData( abBUFFER[id2], id3, id4);
            }
        }
        printf( "ERROR: [DNet] DI: IO word/bit Range Error - [%d]-[%d]-[%d]\n", id2, id3, id4 ); 
        break;
    case 99:
        // get devicenet card status
		switch (id2)
		{
		case 0: // dnet card alive or dead
			// ReadIO();  2010.05.18 DEL For DNET Driver Optimization by JTM
			*result = TRUE;
			return nAlive;
		default:
			printf( "ERROR: [DNet] DI: [ID1=99], [ID2=%d] "
				"is not implemented! Check your config file\n", id2);
			break;
		}
        break;
		
		default:
			printf("ERROR: [DNet] DI - Map number(ID1) is invalid\n");
			break;
    }
    *result = FALSE;
    return 0;
    
}
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

///-----------------------------------------------------------------------------------------------------------
//Name        : Digital_Out
//Date        :
//Return      : void
//Description : Digital out function , Added explicitmessage associated member & sub function (2004.04.06)
//-----------------------------------------------------------------------------------------------------------
void Digital_Out(int id1, int id2, int id3, int id4, int data, int *result)
{
	WORD nTmp = 0;
	
    if (id1 != 1) {
        printf("ERROR: [DNet] DO - Map number(ID1) is invalid\n");
        *result = FALSE; return;
    }
	
    if ((id3 > id4) || (id3 < 0 || id3 > 15 || id4 < 0 || id4 > 15)) {
        printf( "ERROR: [DNet] DO: IO bit Range Error - low[%d] <-> high[%d]\n" , id3 , id4 ); 
        *result = FALSE; return;
    }
	
    nTmp = GetWordDataFromWRBuf(id2);
    WriteWordData(id2, GetMaskedData(nTmp, (WORD)data, id3, id4) );
    if (!WriteIO()) {
        *result = FALSE; return;		
    }
	
    // write setpoint to buffer for valid check,
    // therefore we have to have another buffer (abBUFFER)
    if ( id2 >= 0 && id2 < MAX_ADDRESS ) {
        nTmp = abBUFFER[id2];
        abBUFFER[id2] = GetMaskedData(nTmp, (WORD)data, id3, id4);
	}
    *result = TRUE;
}

void String_In(int id1, char *data, int *result)
{
	int temp_serial = 0;
	int temp_manufac = 0;
	int temp_model = 0;
	int temp_gas = 0;
	int temp_gassize = 0;
	Mac_ID = id1/10000;
	temp_serial = Mac_ID * 10000 + 4809;
	temp_manufac = Mac_ID * 10000 + 4805;
	temp_model = Mac_ID * 10000 + 4806;
	temp_gas = Mac_ID * 10000 + 5205;
	temp_gassize = Mac_ID * 10000 + 5206;
	
	memset( data, 0x00 , sizeof(data));


	if( id1 == temp_serial ) //Serial No
	{	
		strcpy ( data , Serial_No[Mac_ID] );
		//printf( " [Mac ID] : %d  / data: %s \n", Mac_ID, Serial_No[Mac_ID] );

		*result = TRUE; 
	}

	else if( id1 == temp_manufac ) //Manufacturer Name
	{
		strcpy ( data , Manufacturer_Name[Mac_ID] );
		//printf( " [Mac ID] : %d  / data: %s \n", Mac_ID, Manufacturer_Name[Mac_ID] );
			*result = TRUE; 
	}

	else if( id1 == temp_model ) //Model_No
	{
		strcpy ( data , Model_No[Mac_ID] );
		//printf( " [Mac ID] : %d  / data: %s \n", Mac_ID, Model_No[Mac_ID]);

		*result = TRUE; 
	}

	else if( id1 == temp_gas ) //Gas_Name
	{
		strcpy ( data , Gas_Name[Mac_ID] );
		//printf( " [Mac ID] : %d  / data: %s \n", Mac_ID, Gas_Name[Mac_ID]);

		*result = TRUE; 
	}

	else if( id1 == temp_gassize ) //Gas_Size
	{
		strcpy ( data , Gas_Max_Size[Mac_ID] );
		//printf( " [Mac ID] : %d  / data: %s \n", Mac_ID, Gas_Max_Size[Mac_ID]);
		*result = TRUE; 
	}
	// [ 2010.05.20    Driver Version & Driver Build date ADD By SHA
	else if( id1 == 98 ) 
	{
		strncpy( data , DRV_VERSION , 98 );
		*result = TRUE; 
	}
	else if( id1 == 99 ) 
	{
		strncpy( data , DRV_BUILDDATE , 99 );
		*result = TRUE; 
	}
	// ]
	else{
		*result = FALSE;
	}


}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
/* temporary functions */
int Digital_Name_In(char *Name, int *result)
{
    *result = TRUE;
    return 0;
}

void Digital_Name_Out(char *Name, int data, int *result)
{
    *result = TRUE;
}

double Analog_Name_In(char *Name, int *result)
{
    *result = TRUE;
    return 0.0;
}

void Analog_Name_Out(char *Name, double data, int *result)
{
    *result = TRUE;
}

void String_Name_In(char *Name, char *data, int *result)
{
    *result = TRUE;
}

void String_Out(int id1, char *data, int *result)
{
    *result = TRUE;
}

void String_Name_Out(char *Name, char *data, int *result)
{
    *result = TRUE;
}





