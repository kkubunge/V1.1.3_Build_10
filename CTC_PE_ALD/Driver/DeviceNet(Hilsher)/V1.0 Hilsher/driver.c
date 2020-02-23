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

/* enumerations */
enum { DNET_DEAD, DNET_ALIVE };

/* global variables */
unsigned short usBoardNumber = 0;
unsigned char  abOutputData [BUFFER_SIZE];
unsigned char  abInputData  [BUFFER_SIZE];
WORD abBUFFER[MAX_ADDRESS];
int nAlive = DNET_DEAD;
int nLogging = 0;
long nTimeOut = 0;
BOOL INITFLAG = FALSE ;


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
/* read datum from device net card */
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

/* write data to device net card */
BOOL WriteIO(void)
{
    short sRet = 0;

    // sndoffset = 0, sndbufsize = sizeof(abOutputData)
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

    return TRUE;
}

/* device detach function */
BOOL Device_Detach( char *name, char *file, int id1, int id2,
    int id3, int id4, int id5, int id6, int id7, int id8, int id9, int id10)
{
    DNet_Logging("Device detached!");
    return Discon_DNetCard();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Gauge Data convert crkim add */
/*
double BitToDecimal( WORD nData ){ 
	int i,j, zisu=0;
	unsigned char temp1[5], temp2[5];
	short int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128};
	double data=0;

	for(i=0; i<5; i++) temp1[i]=temp2[i]=0;
*/
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
/*
	*temp1 = (BYTE)(nData >> 7);	//지수승,
	temp1[1]=0;
	zisu   = (int)temp1[0] - 127;
	*temp2 = (BYTE)(nData);		//value(0-6bit),
	temp2[1]=0;

	printf("\n--->zisu,temp1    ::[%d][%d]\n",zisu, (int)temp1[0] );
	printf("--->nData   ::[%x][%x]\n",nData, nData+1 );
	printf("--->temp1-2 ::[%x][%x]\n",temp1[0],temp2[0]   );

	j=1;
	data = pow(2,zisu);

	printf("--->j-data ::[%d][%f]\n",0,data   );

	for(i=6; i>=0; i--) {
		if ( temp2[0] & mask[i] ) {
			data = data + pow(2,zisu-j);
			printf("--->j-data ::[%d][%f] 2*[%d] \n",j,data , zisu-j  );
		}
		j++;
	}

	printf("--->return data ::[%f]\n",data   );
	return data;
}
*/
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
        if (ReadIO()) {
            nData = (int)ReadWordData2(id2);
            *result = TRUE;
			printf ( "HS_DNET Analog IN : Address [%d] , Data [%d]\n" , id2 , nData );
            return (double)nData;
        }
    }
	/* Gauge Data convert crkim add(id1==10) */
    else if (id1 == 10) {
        if (ReadIO()) {
            nData = (int)ReadWordData(id2+2);
			//Press = BitToDecimal(nData);

			Press = (double)BitToDecimal_1((WORD)nData, id2);	//상위 2바이트
			
            *result = TRUE;
            return Press;
        }
		else printf("KCR ERROR: [DNet] AI - Map number(ID1) is invalid\n");
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
            if (ReadIO()) {
                *result = TRUE;
                return RemoveNotNeedData( ReadWordData(id2), id3, id4);
            }
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
                ReadIO();
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

/* digital out function */
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

void String_In(int Id1, char *data, int *result)
{
    *result = TRUE;
}

void String_Name_In(char *Name, char *data, int *result)
{
    *result = TRUE;
}

void String_Out(int Id1, char *data, int *result)
{
    *result = TRUE;
}

void String_Name_Out(char *Name, char *data, int *result)
{
    *result = TRUE;
}





