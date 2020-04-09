#include "CimRs232.h"
#include "kutletc.h"
#include "kutlstr.h"
#include "kutlanal.h"
#include "driver.h"

//-----------------------------------------------------------------------------------
enum	{	NOTLOG	,	LOG		};
enum	{	TEXT	,	BINARY	};

enum	{ Ret_No		,Ret_Yes };
enum    { LOCAL			,REMOTE  };
enum    { Packet_To_Read,Packet_To_Write };
enum	{ DO			,DI			,AO				    ,AI			            ,SO		      ,SI		};
enum    { DISCONNECT	,NORMAL		,OVERCURRENTALARM	,AUTO_OVERCURRENTALARM	,SETTINGALARM ,RPMALARM };
//-----------------------------------------------------------------------------------

BOOL  CommStatus;
char  RecvTermStr[4];

int		g_nMode1;
int		g_nSts1;
double	g_nRPM1;
int		g_nLowLimit1;
int		g_nHighLimit1;
int		g_nMode2;
int		g_nSts2;
double	g_nRPM2;
double	g_nLowLimit2;
double	g_nHighLimit2;
char*	LogDrive;//Add ybkim
// [ 2010.05.20    Driver Version & Driver Build date ADD By JTM
char 	DRV_VERSION[50]	=	"FFU_DRV_Ver. 1.8";
char 	DRV_BUILDDATE[50] =	"2018.11.09";
// ]

// 2010.06.04 Log ON_OFF ADD by JTM
int	LOG_ONOFF	=	1;
// 2010.09.12 Read Mode ADD by JTM
unsigned char R_MODE1;

//-----------------------------------------------------------------------------------
const int RECIEVE_TIMEOUT					= 1000;

//MCUL Controller Protocol Related Definition
const unsigned char DLE						= 0x10;	//Data link escape - 전송제어 문자 앞에 삽입하여 전송제어 문자를 알림
const unsigned char SOH						= 0x01;	//start of heading
const unsigned char STX						= 0x02;	//start of text
const unsigned char ETX						= 0x03;	//end of text
const unsigned char EOT						= 0x04;	//end of transmission - 전송 종료 및 데이터 링크의 해제
const unsigned char ENQ						= 0x05;	//enquiry - 상대편에 데이터 링크 설정 및 응답 요구
const unsigned char ACK						= 0x06;	//ACKnowledge - 수신된 메시지에 대한 긍정의 대답
const unsigned char NAK						= 0x15;	//Negative acknowledge - 수신된 메시지에 대한 부정 응답
const unsigned char RETURN_FLAG_OK			= 0xb9;
const unsigned char DUPID					= 0x9f;
const unsigned char INDIVIDUAL_CONTROL		= 0x85;
const unsigned char INDIVIDUAL_READ			= 0x86;
const unsigned char BLOCK_CONTROL			= 0x89;
const unsigned char BLOCK_READ				= 0x8a;
const unsigned char READ_OPTION_ALL_DATA	= 0x9f;
const unsigned char GROUP_READ				= 0x8e;
const unsigned char cID0					= 0x80;
const unsigned char LOCAL_CONTROL_MODE		= 0x81;
const unsigned char REMOTE_CONTROL_MODE		= 0x80;
const unsigned char RPM_ALARM   			= 0x90;
const unsigned char RPM_ALARM_LIMIT			= 0x9c;
const unsigned char SETTING_ALARM   		= 0x88;
const unsigned char AUTO_OVER_CURRENT_ALARM = 0xA0;
const unsigned char OVER_CURRENT_ALARM		= 0xC0;
const unsigned char NO_ALARM				= 0x80;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//[2010.06.04 Time Log ADD by JTM
void Time_Log( char *Command, int Mode ) {
	FILE *fp = NULL;
	char FilePath[MAX_PATH] , FileName[64];
	SYSTEMTIME	st;

	HANDLE hSrch;
	WIN32_FIND_DATA wfd;

	if( LOG_ONOFF != 1) return ;

	GetLocalTime( &st );

	if ( strlen(LogDrive) >= 2) {//Driver/MCUL32.dll 드라이버 문자열을 선언하지 않은 경우에는 dll파일의 경로와 파일명이 넘어온다.
		sprintf( FileName , "f:\\Datalog\\FFU\\COM_%04d%02d%02d%02d.log" , st.wYear , st.wMonth , st.wDay , st.wHour );
		sprintf( FilePath, "f:\\Datalog\\FFU");
	}
	else if ( strlen(LogDrive) == 1 ) {//드라이버 문자열을 선언한 경우
		sprintf( FileName , "%s:\\Datalog\\FFU\\COM_%04d%02d%02d%02d.log" , LogDrive, st.wYear , st.wMonth , st.wDay , st.wHour );
		sprintf( FilePath, "%s:\\Datalog\\FFU",LogDrive);
	}

	//Logging 폴더 생성
	hSrch=FindFirstFile(FilePath,&wfd);
	if (hSrch == INVALID_HANDLE_VALUE)
	{
		if ( !CreateDirectory ( FilePath , NULL ))
		{
			printf( "[FFU_DRIVER] : Can't Create Log File\n"  );
			FindClose(hSrch);
			return;
		}
	}
	FindClose(hSrch);
	//

	fp = fopen( FileName , "a+" );
	if( !fp ) { printf( "[ FFU_DRIVER ] : Can't Create Log File\n" ); return; }
	if ( Mode == 0 )	fprintf( fp , "%04d/%02d/%02d %02d:%02d:%02d:%04d\t[SND]\t%s\n" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond , st.wMilliseconds, Command );
	if ( Mode == 1 )	fprintf( fp , "%04d/%02d/%02d %02d:%02d:%02d:%04d\t[RCV]\t\t%s\n" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond , st.wMilliseconds, Command );

	fclose( fp );
}
//]


//-----------------------------------------------------------------------------------
//Module Loading이 될 때 Initialize
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 )
{
	g_nMode1      =0;
	g_nSts1       =0;
	g_nRPM1       =0;
	g_nLowLimit1  =0;
	g_nHighLimit1 =0;
	g_nMode2      =0;
	g_nSts2       =0;
	g_nRPM2       =0;
	g_nLowLimit2  =0;
	g_nHighLimit2 =0;

	CommStatus     = FALSE;
	RecvTermStr[0] = 0x03;

	//printf("DeviceAttach Name : %s, File : %s\n", Name, File );

	if(RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ))
	{
		//printf("==== MCUL Port Configuration Port:%d Baud:%d DataBit:%d StopBit:%d Parity:%d\n",ID1,ID2,ID3,ID4,ID5);
		if ( File != NULL ) {
			LogDrive = File;//I/O Driver선언시 ID1(Port)앞에 선언된 문자열을 전역변수로 넣어줌...20100113, ybkim
		}
		//2010.05.25 FFU COMM Print by JTM
		printf("FFU COMM OPEN SUCCESS...[Ver:%s / Rebuild_Date:%s] \n" , DRV_VERSION, DRV_BUILDDATE );

		return TRUE;
	}
	else
	{
		//2010.05.25 FFU COMM Print by JTM
		printf("FFU COMM OPEN Fail...[Ver:%s / Rebuild_Date:%s] \n" , DRV_VERSION, DRV_BUILDDATE );

		return FALSE;
	}
}
//-----------------------------------------------------------------------------------
//Module이 Unloading 될때 Deinitialize
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 )
{
	RS232_Disconnect_Port();
	return TRUE;
}


unsigned char GetBCCErrCheck(char *pPacket, int nSize)
{
    int i = 0;
    unsigned char   cCheckSum = 0;

    for (i = 1; i <= nSize; ++i) //Except Starting STX
	{
        if (pPacket[0] == 0x00)
		{
			break;
		}
        else
		{
			cCheckSum += pPacket[i] ;
		}
	}
    return cCheckSum;
}


int CreatePacketToRead(unsigned char* szDataToSend,int ID1/*MCULID*/,int ID2/*LCUID*/,int ID3/*CHID*/,int ID4)
{
	int i=0;
	int nIndex = 0;

	//unsigned char MODE1; 	Read Mode 전역변수 선언 by JTM
	unsigned char MODE2;
	unsigned char MCULID;
    unsigned char LCUID;
	unsigned char CS;
    unsigned char cID1;
	unsigned char cID2;

	do
	{
		//Block Read Packet Creation
		//STX|MODE1|MODE2|MCULID(ControllerID)|DUPID|StartLCUID(FanID)|EndLCUID(FanID)|ChekSum|ETX
		//MODE1:GROUP/BLOCK/INDIVIDUAL/READ/CONTROL/ALARMHOLDRESET/FLAGCHECK
		//MODE2:ProcessValue/AlarmData/SettingValue/LowSettingValue/AllData

          //MP#1호기(차압 분리형) - Block Read
		  //EFEM 표준화(차압 일체형) - Group Read
		  //R_MODE1  = BLOCK_READ;
		  R_MODE1  = GROUP_READ;
          MODE2  = READ_OPTION_ALL_DATA;

		  cID1   = (unsigned char)ID1; //MCULID
		  MCULID = (cID0 | cID1); //0X(cID0)(cID1)

		  cID2  = (unsigned char)ID2; //LCUID
		  LCUID = (cID0 | cID2); //0x(cID0)(cID2)

		  szDataToSend[nIndex++] = STX;
		  szDataToSend[nIndex++] = R_MODE1; //0x8A
		  szDataToSend[nIndex++] = MODE2; //0x9F
		  szDataToSend[nIndex++] = MCULID; //0X81
		  szDataToSend[nIndex++] = DUPID; //0X9f
		  //Group Read에는 Start/End LCUID가 없음.(Block Read를 사용할경우 아래 두줄의 주석을 제거하고 사용)
		  //szDataToSend[nIndex++] = LCUID; //0X81/0x82
		  //szDataToSend[nIndex++] = LCUID; //0X81/0x82

		  CS = GetBCCErrCheck(szDataToSend, nIndex); //CHECKSUM
		  szDataToSend[nIndex++] = CS;
		  szDataToSend[nIndex++] = ETX;


		  //Write Packet to console for the trace /////////////////////
		  //printf("Packet_To_Read => ");
		  for(i=0; i <= nIndex ; i++)
		  {
			//printf(" 0x%x",szDataToSend[i]);
			//if(i == nIndex) printf(" \n",szDataToSend[i]);
		  }
		  //Write Packet to console for the trace /////////////////////


		  return nIndex;

	}while(0); //위의 명령을 한번 실행하고 멈춘다.
	return nIndex;
}

BOOL ReadIOCValue(int ID1/*MCULID*/,int ID2/*LCUID*/,int ID3/*CHID*/,int *nMode,int *nSts,int *nRPM,int *nLowLimit,int *nHighLimit)
{
	int nSize         = 0;
	int nIDLoc        = 0;
	int nRPMLoc       = 0;
	int nSetRPM       = 0;
	int nHLimitLoc    = 0;
	int nLLimitLoc    = 0;
	int nStatusLoc    = 0;
	int nIncrease     = 0;
	int nReadCount    = 0;
	int ID4           = 0;
	int nGetRPM       = 0;
	int nGetLowLimit  = 0;
	int nGetHighLimit = 0;
    int  nIdx		  = 0;
	int  nSTX         = 0;
	int  nETX         = 0;
	int  nRetry       = 0;
	int  RecvDataCnt  = 199; //Total Recv Data Count
	unsigned char szSendData[256];
	unsigned char szRawData[256];

	char buffer[256];


	memset(szSendData,0,256);
	memset(szRawData ,0,256);
	memset(buffer ,0,256);
	nSize = CreatePacketToRead(szSendData,ID1,ID2,ID3,ID4);

	RS232_Clear_Buffer();

	if(RS232_Write_String(szSendData,nSize))
	{
		sprintf(buffer, "%02X %02X %02X %02X %02X %02X %02X",
				szSendData[0], szSendData[1], szSendData[2], szSendData[3],szSendData[4], szSendData[5],  szSendData[6] );
		Time_Log( buffer, FALSE );
	}
	else
	{
		printf("[FFU_DRIVER] => Read Request Packet to Controller Failed !! \n");
		sprintf(buffer, "Read Request Error => %02X %02X %02X %02X %02X %02X %02X",
				szSendData[0], szSendData[1], szSendData[2], szSendData[3],szSendData[4], szSendData[5],  szSendData[6] );
		Time_Log( buffer, FALSE );
		return FALSE;
	}

	//_sleep(500); Remove Delay Time

	if ( RS232_Read_Count_Char(szRawData,RecvDataCnt,&nReadCount,RECIEVE_TIMEOUT) )
	{
		sprintf(buffer, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",szRawData[0], szRawData[1], szRawData[2], szRawData[3],szRawData[4], szRawData[5],  szRawData[6],
			szRawData[7], szRawData[8], szRawData[9], szRawData[10],szRawData[11],szRawData[12], szRawData[13], szRawData[14], szRawData[15], szRawData[16]);
		Time_Log( buffer, TRUE );
	}
	else
	{
		printf("[FFU_DRIVER] => Recv Timeout [%d-%d-%d] \n" , ID1 , ID2 , ID3);
		sprintf(buffer, "Recv Timeout %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",szRawData[0], szRawData[1], szRawData[2], szRawData[3],szRawData[4], szRawData[5],  szRawData[6],
			szRawData[7], szRawData[8], szRawData[9], szRawData[10],szRawData[11],szRawData[12], szRawData[13], szRawData[14], szRawData[15], szRawData[16]);
		Time_Log( buffer, TRUE );
		//return FALSE;
	}

	if( GetBCCErrCheck(szRawData,RecvDataCnt-3) != szRawData[RecvDataCnt-2] )
	{
		printf("[FFU_DRIVER] => Check Sum Error [%d-%d-%d] \n" , ID1 , ID2 , ID3);
		sprintf( buffer, "[FFU_DRIVER] => Check Sum Error [%d-%d-%d]", ID1 , ID2 , ID3);
		Time_Log( buffer, TRUE );
		return FALSE;
	}

	if ( ID2 == 1 )
	{
		nRPMLoc       = 6;
		nStatusLoc    = 7;
		nSetRPM       = 8;
		nLLimitLoc    = 9;
		nHLimitLoc    = 10;
	}
	else if ( ID2 == 2 )
	{
		nRPMLoc       = 12;
		nStatusLoc    = 13;
		nSetRPM       = 14;
		nLLimitLoc    = 15;
		nHLimitLoc    = 16;
	}

	*nRPM	   = szRawData[nRPMLoc]*10;    // Set RPM
	//*nLowLimit = szRcvData[nIncrease+nLLimitLoc]*10; // Set LowLimit
	//*nHighLimit= szRcvData[nIncrease+nHLimitLoc]*10; // Set HighLimit
	*nLowLimit = szRawData[nLLimitLoc]; // Read LSV
	*nHighLimit= szRawData[nHLimitLoc]; // Read HSV
	//printf("\n Current   RPM of LCU%d : %d ",ID2,szRcvData[nIncrease+nRPMLoc]*10);
	//printf("\n Set       RPM of LCU%d : %d ",ID2,szRcvData[nIncrease+nSetRPM]*10);
	//printf("\n Set LowLimit  of LCU%d : %d\n",ID2, *nLowLimit);
	//printf("\n Set HighLimit of LCU%d : %d\n",ID2, *nHighLimit);
	//printf("\n Alarm Status  of LCU%d : %x \n",ID2,szRcvData[nIncrease+nStatusLoc]);

	// Status Check Starts Here !!
	if(szRawData[nStatusLoc] == 0x00)
	{ //No Connection
		*nSts = DISCONNECT;
	}
	else if(szRawData[nStatusLoc]  == NO_ALARM)
	{ //No Alarm
		*nSts = NORMAL;
	}

	if((szRawData[nStatusLoc] & LOCAL_CONTROL_MODE) == LOCAL_CONTROL_MODE)
	{ // Local Control Mode
	    *nMode = LOCAL;
	}
	else
	{ // Remote Control Mode
	    *nMode = REMOTE;
	}
	if((szRawData[nStatusLoc] & RPM_ALARM) == RPM_ALARM)
	{// RPM Alarm
		*nSts = RPMALARM;
	}
	else if((szRawData[nStatusLoc] & SETTING_ALARM) == SETTING_ALARM)
	{// Setting Alarm
		*nSts = SETTINGALARM;
	}
	else if((szRawData[nStatusLoc] & AUTO_OVER_CURRENT_ALARM) == AUTO_OVER_CURRENT_ALARM)
	{// Auto Over Current Alarm
		*nSts = AUTO_OVERCURRENTALARM;
	}
	else if((szRawData[nStatusLoc] & OVER_CURRENT_ALARM) == OVER_CURRENT_ALARM)
	{// Over Current Alarm
		*nSts = OVERCURRENTALARM;
	}

	return TRUE;
}

int CreatePacketToWrite(unsigned char* szDataToSend,int ID1/*MCULID*/,int ID2/*MCUID*/,int ID3/*CHID*/,int ID4)
{
	int i=0;
	int nIndex = 0;
	unsigned char MODE1;
	unsigned char MODE2;
	unsigned char MCULID;
    unsigned char LCUID;
	unsigned char CS;
    unsigned char cID1;
	unsigned char cID2;
	unsigned char SETRPM;

	do
	{

		//Individual Write Packet Creation
		//STX|MODE1|MODE2|MCULID(FanID)|DUPID|LCUID(ControllerID)|DATA SV|DATA LSV|DATA HSV|ChekSum|ETX
		//MODE1:GROUP/BLOCK/INDIVIDUAL/READ/CONTROL/ALARMHOLDRESET/FLAGCHECK
		//MODE2:ProcessValue/AlarmData/SettingValue/LowSettingValue/AllData

		  MODE1  = BLOCK_CONTROL;
		  MODE2  = RPM_ALARM_LIMIT;


		  cID1   = (unsigned char)ID1;
		  MCULID = (cID0 | cID1);

		  cID2  = (unsigned char)ID2;
		  LCUID = (cID0 | cID2);

		  SETRPM = (int)ID4/10;

		  szDataToSend[nIndex++] = STX;
		  szDataToSend[nIndex++] = MODE1; //0X89
		  szDataToSend[nIndex++] = MODE2; //0X9C
		  szDataToSend[nIndex++] = MCULID; //0X81
		  szDataToSend[nIndex++] = DUPID; //0X9F
		  szDataToSend[nIndex++] = LCUID; //0X81/0x82
		  szDataToSend[nIndex++] = LCUID; //0X81/0x82
		  szDataToSend[nIndex++] = SETRPM;// 0X64
		  szDataToSend[nIndex++] = SETRPM-(SETRPM/2);// Low Limit //0x32
		  szDataToSend[nIndex++] = SETRPM+(SETRPM/2);// High Limit //0x96
		  CS = GetBCCErrCheck(szDataToSend, nIndex);
		  szDataToSend[nIndex++] = CS;
		  szDataToSend[nIndex++] = ETX;

		  return nIndex;

	}while(0);
	return nIndex;
}

BOOL WriteIOCValue(int ID1,int ID2,int ID3,int nRPM)
{
	BOOL bRet = FALSE;
    int nIdx =0;
	int nSize = 0;
	int nRetry = 0;
	int nReadCount = 0;
	unsigned char szSendData[256];
	unsigned char szRcvData[256];

	char buffer[256];


	memset(szSendData,0,256);
	memset(szRcvData,0,256);
	memset(buffer,0,256);
	nSize = CreatePacketToWrite(szSendData,ID1,ID2,ID3,nRPM);
	RS232_Clear_Buffer();


	if( RS232_Write_String(szSendData,nSize) )
	{
		sprintf(buffer, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",szSendData[0], szSendData[1], szSendData[2], szSendData[3],szSendData[4], szSendData[5],  szSendData[6],
					   szSendData[7], szSendData[8], szSendData[9], szSendData[10],szSendData[11] );
		Time_Log( buffer, FALSE );

	}
	else
	{
		printf("[FFU_DRIVER] => Write Packet Failed !! \n");

		sprintf(buffer, "Write Request Error => %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",szSendData[0], szSendData[1], szSendData[2], szSendData[3],szSendData[4], szSendData[5],  szSendData[6],
												   szSendData[7], szSendData[8], szSendData[9], szSendData[10],szSendData[11] );
		Time_Log( buffer, FALSE );
		return FALSE;

	}

	if ( RS232_Read_Count_Char(szRcvData,10,&nReadCount,RECIEVE_TIMEOUT) )
	{
		sprintf(buffer, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",szRcvData[0], szRcvData[1], szRcvData[2], szRcvData[3],szRcvData[4], szRcvData[5],  szRcvData[6],
															szRcvData[7], szRcvData[8], szRcvData[9]);
		Time_Log( buffer, TRUE );

	}
	else
	{
		sprintf(buffer, "Recv Error %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",szRcvData[0], szRcvData[1], szRcvData[2], szRcvData[3],szRcvData[4], szRcvData[5],  szRcvData[6],
			szRcvData[7], szRcvData[8], szRcvData[9]);
		Time_Log( buffer, TRUE );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result )
{

	int nMode=0;
	int nSts=0;
	int nRPM=0;
	int nLowLimit=0;
	int nHighLimit=0;
    double dwData=0;
	double CalTemp = 0;
//	char buffer[20], buffer2[20];

	*Result = FALSE;


	switch(ID2)
	{
		case 1: // LCU1
		{
			switch(ID3)
			{
				case 1: //FFU1_RpmAI
				{
					if(ReadIOCValue(ID1,ID2,ID3,&nMode,&nSts,&nRPM,&nLowLimit,&nHighLimit))
					{
						g_nLowLimit1 = nLowLimit;
						g_nHighLimit1= nHighLimit;
						dwData = (double)nRPM;
						*Result = TRUE;
					}
					break;
				}
 				case 2: //FFU1_RpmLowLimitAI
				{
					dwData = (double)g_nLowLimit1;
					*Result = TRUE;
					break;
				}
 				case 3: //FFU1_RpmHighLimitAI
				{
					dwData = (double)g_nHighLimit1;
					*Result = TRUE;
					break;
				}
				case 4: //FFU1_DifferentPress (SINSUNG)
				{
					if ( g_nHighLimit1 == 255 ) {//마이너스처리
					//printf("##################################################\n");
						//[ 2010/01/08 MCUL32 Unit Change : mmAq -> Pa ( BY LJH )
					//	dwData = -1 * (((double)g_nHighLimit1 - (double)g_nLowLimit1) / 100) - 0.01;
						dwData = ( -1 * (((double)g_nHighLimit1 - (double)g_nLowLimit1) / 100) - 0.01 ) * 9.8 ;
						//]
						//dwData = (((double)g_nLowLimit1 + (double)g_nHighLimit1) / 100) * 9.8;//mmAq를 Pa단위값에 맞게 변환
					}
					else {
					//printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
						//[ 2010/01/08 MCUL32 Unit Change : mmAq -> Pa ( BY LJH )
					//	dwData = (((double)g_nLowLimit1 + (double)g_nHighLimit1) / 100);
						dwData = (((double)g_nLowLimit1 + (double)g_nHighLimit1) / 100 ) * 9.8 ;
						//]
					}
					//dwData = (((double)g_nLowLimit1 + (double)g_nHighLimit1) / 100) * 9.8;//mmAq를 Pa단위값에 맞게 변환
					*Result = TRUE;
					break;
				}
				case 5: //FFU1_DifferentPress (MARS)
				{
					if ( g_nHighLimit1 == 255 ) {//마이너스처리
						dwData = ( -1 * (((double)g_nHighLimit1 - (double)g_nLowLimit1) / 100) - 0.01 );
					}
					else {
						dwData = (((double)g_nLowLimit1 + (double)g_nHighLimit1) / 100 );
					}
					*Result = TRUE;
					break;
				}
				// case 5: //FFU1_DifferentPress : mmAq		//P1-3L 요청사항 : FFU Controller 지시계와 일치된 값으로 표현.
				// 	{
				// 		if ( g_nHighLimit1 == 255 ) {//마이너스처리
				// 			dwData = ( -1 * (((double)g_nHighLimit1 - (double)g_nLowLimit1) / 100) - 0.01 );
				// 		}
				// 		else {
				// 			dwData = (((double)g_nLowLimit1 + (double)g_nHighLimit1) / 100 );
				// 		}
				// 		*Result = TRUE;
				// 		break;
				// }
				// case 6: //FFU1_DifferentPress : inchAq		//P1-3L 요청사항 : FFU Controller 지시계와 일치된 값으로 표현.
				// {
				// 	if ( g_nHighLimit1 == 255 ) {//마이너스처리
				// 		dwData = ( -1 * (((double)g_nHighLimit1 - (double)g_nLowLimit1) / 100) - 0.01 );
				// 		dwData = dwData/1000;
				// 	}
				// 	else {
				// 		dwData = (((double)g_nLowLimit1 + (double)g_nHighLimit1) / 100 );
				// 		dwData = dwData/1000;
				// 	}
				// 	*Result = TRUE;
				// 	break;
				// }
				default:
					break;
			}
			break;
		}
		case 2: // LCU2
		{
			switch(ID3)			{
				case 1: //FFU2_RpmAI
				{
					if(ReadIOCValue(ID1,ID2,ID3,&nMode,&nSts,&nRPM,&nLowLimit,&nHighLimit))	{
						g_nLowLimit2 = nLowLimit;
						g_nHighLimit2= nHighLimit;
						dwData = (double)nRPM;
						*Result = TRUE;
					}break;

				}
 				case 2: //FFU2_RpmLowLimitAI
				{
					dwData = (double)g_nLowLimit2;
					*Result = TRUE;
					break;
				}
 				case 3: //FFU2_RpmHighLimitAI
				{
					dwData = (double)g_nHighLimit2;
					*Result = TRUE;
					break;
				}
				case 4: //FFU2_DifferentPress
					{
						if ( g_nHighLimit2 == 255 ) {//마이너스처리
						//printf("##################################################\n");
							dwData = -1 * (((double)g_nHighLimit2 - (double)g_nLowLimit2) / 100) - 0.01;
							//dwData = (((double)g_nLowLimit2 + (double)g_nHighLimit2) / 100) * 9.8;//mmAq를 Pa단위값에 맞게 변환
						}
						else {
						//printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
							dwData = (((double)g_nLowLimit2 + (double)g_nHighLimit2) / 100);
						}
						//dwData = (((double)g_nLowLimit2 + (double)g_nHighLimit2) / 100) * 9.8;//mmAq를 Pa단위값에 맞게 변환
						*Result = TRUE;
						break;
				}
				default:
					break;
			}
			break;
		}
		default:
			break;
	}
	return dwData;
}
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3/*CHID 1*/ , double Data , int *Result )
{
	int nRPM;

	*Result = FALSE;
	nRPM = (int)Data;


	if(WriteIOCValue(ID1,ID2,ID3,nRPM))
	{
		*Result = TRUE;
	}
}

//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	int nData = 0;
	int nMode = 0;
	int nSts  = 0;
	int nRPM  = 0;
	int nLowLimit = 0;
	int nHighLimit= 0;


	*Result = FALSE;

	switch(ID2)
	{
		case 1: // LCU1
		{
			switch(ID3)
			{
				case 1: //FFU1_StsDI
				{
					if(ReadIOCValue(ID1,ID2,ID3,&nMode,&nSts,&nRPM,&nLowLimit,&nHighLimit))
					{
						CommStatus = TRUE;
						nData = nSts;
						g_nMode1 = nMode;
						*Result = TRUE;
					}
					else
					{
						CommStatus = FALSE;
					}
					break;
				}
 				case 2: //FFU1_ModeDI
				{
					nData = g_nMode1;
					*Result = TRUE;
					break;
				}
 				case 99: //FFU1_CommStsDI
				{
					//if(CommStatus) *Result = TRUE;
					//else *Result = FALSE;
					*Result = TRUE;
					nData = CommStatus;
					break;
				}
				default:
					break;
			}
			break;
		}
		case 2: // LCU2
		{
			switch(ID3)
			{
				case 1: //FFU2_StsDI
				{
					if(ReadIOCValue(ID1,ID2,ID3,&nMode,&nSts,&nRPM,&nLowLimit,&nHighLimit))
					{
						CommStatus = TRUE;
						nData = nSts;
						g_nMode2 = nMode;
						*Result = TRUE;
					}
					else
					{
						CommStatus = FALSE;
					}
					break;
				}
 				case 2: //FFU2_ModeDI
				{
					nData = g_nMode2;
					*Result = TRUE;
					break;
					break;
				}
 				case 99: //FFU2_CommStsDI
				{
					//if(CommStatus) *Result = TRUE;
					//else *Result = FALSE;
					*Result = TRUE;
					nData = CommStatus;
					break;
				}
				default:
					break;
			}
			break;
		}
		default:
			break;
	}


	return nData;
}
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) {

	switch(ID1) {
		case 9999:
			if ( Data == 0 ) // 0 : On , 1 : Off
			{
				LOG_ONOFF = 1 ;
			}else {
				LOG_ONOFF = 0 ;
			}
			//Protocol_Logging( Data , BINARY);	//BINARY, TEXT
			break;
	}

	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	// [ 2010.05.20    Driver Version & Driver Build date ADD By JTM
	if( ID1 == 98 )
	{
		strncpy( data , DRV_VERSION , 98 );
		*Result = TRUE;
	}
	else if( ID1 == 99 )
	{
		strncpy( data , DRV_BUILDDATE , 99 );
		*Result = TRUE;
	}

	else{
		*Result = FALSE;
	}
	// ]
}

//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {
	*Result = TRUE;
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
int Digital_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}
//-----------------------------------------------------------------------------------
void Digital_Name_Out( char *Name , int data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
void String_Name_In( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
void String_Name_Out( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
