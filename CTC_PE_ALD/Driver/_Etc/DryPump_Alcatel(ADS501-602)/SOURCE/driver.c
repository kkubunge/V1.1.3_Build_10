#include "cimrs232.h"
#include "driver.h"
//------------------------------------------------------------------------------------
#include "kutletc.h"
#include "kutlanal.h"
#include "kutlstr.h"
#include "CIMseqcdll.h"
//-----------------------------------------------------------------------------------
#define	MAXSHAREMEMORY	100
#define DATA_POS		5
#define POLLING_PERIOD	800		// Polling주기(단위: msec)
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
enum	{ Ret_No , Ret_Yes };
enum	{ DO, DI, AO, AI, SO, SI, PS, ADDR };
//-----------------------------------------------------------------------------------
char OffOnParam[2][5] = { "OFF" , "ON" }; 
//-----------------------------------------------------------------------------------
char  SendTermStr[16];
char  RecvTermStr[16];
int   TimeOut = 100;		// COM 통신 TimeOut(단위: msec)
int	  PumpAddress;			// Alcatel Pump의 Address[0 ~ 32]
//-----------------------------------------------------------------------------------
BOOL  CommStatus = FALSE;
BOOL  ErrStatus  = FALSE;
//-----------------------------------------------------------------------------------
char	gpSTA[MAXSHAREMEMORY];
//-----------------------------------------------------------------------------------
UINT	gnOutCount = 0;
//-----------------------------------------------------------------------------------
HANDLE	hMutex = NULL;
//-----------------------------------------------------------------------------------
void POLLING_THREAD();
void Error_Message( int IO , int ID1 , int ID2 , int ID3 , int ID4 );
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Get_Bit_Status (char src , short int pos)
{
	short int res;
	short int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128} , temp;
	temp = src & mask[pos];
	temp = temp >> pos;
	res = temp;
	return res;
}

//-----------------------------------------------------------------------------------
/*
 *	ID9 : Pump Address[0~32]
 *	IP10: TimeOut
 */
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 )
{
	char buf[256];
	SendTermStr[0] = 13; //CR
	SendTermStr[1] = 0;
	RecvTermStr[0] = 13;
	RecvTermStr[1] = 0;

	// Pump Addr
	if( ID9 < 0 )
	{
		Error_Message(ADDR, ID9, 0, 0, 0);
		return FALSE;
	}
	else
	{
		PumpAddress = ID9;
	}

	// TimeOut
	if ( ID10 < 100 ) TimeOut = 50;
	else              TimeOut = ID10;

	sprintf(buf, "ALCATEL_DRIVER_MUTEX_%s", Name);
	if((hMutex =CreateMutex(NULL, FALSE, buf)) == NULL)
	{
#ifdef _DEBUG
	printf("[ALCATEL DRIVER]Mutex Create Fail!\n");
#endif
		return FALSE;
	}

#ifdef _DEBUG
	printf("[ALCATEL DRIVER]Mutex Create Success!\n");
#endif

	if(!RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ))
	{
#ifdef _DEBUG
	printf("[ALCATEL DRIVER]COM Port[%d] Connect Fail!\n", ID1);
#endif
		return FALSE;
	}

#ifdef _DEBUG
	printf("[ALCATEL DRIVER]COM Port[%d] Connect Success!\n", ID1);
#endif

	if(_beginthread( (void *) POLLING_THREAD , 0 , 0) == -1)	return FALSE;
	else														return TRUE;
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) 
{
	CloseHandle(hMutex);
	RS232_Disconnect_Port();
	return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Rs232_IO_Send( char *Command )
{
	if(!RS232_Write_String( Command, strlen(Command)))
	{
		printf( "(DryPump)Rs232 Send Error => Sending : %s\n" , Command );
		return FALSE;
	}

#ifdef _DEBUG
	printf("[ALCATEL DRIVER]TX >> %s\n", Command);
#endif

	if(!RS232_Write_String(SendTermStr, strlen(SendTermStr)))
	{
		printf( "(DryPump)Rs232 Send Error => Sending : SendTermStr(CR)\n" );
		return FALSE;
	}

	return TRUE;
}



//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Rs232_IO( char *Command , char *Response ) {
	BOOL Result = TRUE;
	int ReadCnt/* , i*/;
	char Buffer[256];

	Result = Rs232_IO_Send( Command );
#ifdef _DEBUG
	printf("[%d]TX >> %s\n", GetTickCount(), Command);
#endif

	if( Result ) 
	{
		Result = RS232_Read_String( Buffer , RecvTermStr , 255 , TimeOut , &ReadCnt );
#ifdef _DEBUG
	printf("[%d]RX << %s\n", GetTickCount(), Buffer);
#endif

		if(strncmp( Command , Buffer , strlen(Command) ) == 0)
		{
			Result = RS232_Read_String( Buffer , RecvTermStr , 255 , TimeOut , &ReadCnt );
#ifdef _DEBUG
	printf("[%d]RX << %s\n", GetTickCount(), Buffer);
#endif
		}
		if( !Result ) printf( "(DryPump)Rs232 Receive Error => Sending : %s\n" , Command ); 
		else 
		{
			if (strncmp(Command, Buffer, 4) != 0)
			{
				printf( "(DryPump)Response Address Fail\n" );
				Result = FALSE;
			}
			else 
			{
				memcpy(Response, Buffer, ReadCnt);
				Response[ReadCnt] = 0x00;
			}
		}
	}

	if ( !Result ) CommStatus = FALSE;
	else           CommStatus = TRUE;
	
	return Result;
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Send_Command( char *Command , char *Response , int Return ) 
{

	RS232_Clear_Buffer();

	if( Return ) { if( !Rs232_IO( Command , Response ) ) return FALSE; }
	else { if( !Rs232_IO_Send( Command ) ) return FALSE; }

	return TRUE;
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Error_Message( int IO , int ID1 , int ID2 , int ID3 , int ID4 )
{
	switch( IO ) {
		case DO :	printf( "(DryPump)DO Error : [%d][%d][%d][%d]\n" , ID1 , ID2 , ID3 , ID4 ); 
					break;
		
		case DI :	printf( "(DryPump)DI Error : [%d][%d][%d][%d]\n" , ID1 , ID2 , ID3 , ID4 ); 
					break;
		
		case AO :	printf( "(DryPump)AO Error : [%d][%d][%d]\n" , ID1 , ID2 , ID3 ); 
					break;
		
		case AI :	printf( "(DryPump)AI Error : [%d][%d][%d]\n" , ID1 , ID2 , ID3 ); 
					break;
		
		case SO :	printf( "(DryPump)SO Error : [%d]\n" , ID1 ); 
					break;
		
		case SI :	printf( "(DryPump)SI Error : [%d]\n" , ID1 ); 	
					break;

		case PS :	printf( "(DryPump)Polling Error\n"); 	
					break;

		case ADDR :	printf( "(DryPump)Pump Address Error : [%d]\n", ID1); 	
					break;

		default :	printf( "(DryPump)Unknown data type\n" );
					break;
	}
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void FormatData( int Add , char *UnFormat , char *Format ) {
	sprintf( Format , "#%03d%s" , Add , UnFormat );
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double GetAnalogValue(int spos, int epos)
{
	char buffer[128];
	int len = epos - spos + 1;
	if((len <= 0) || (len > sizeof(buffer)-1)) return 0;

	memcpy(buffer, &gpSTA[spos], len);
	buffer[len] = 0;

	return atol(buffer);
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result )
{
	double Data = 0;
	*Result = TRUE;

	switch( ID2 ) 
	{
		// TM.DP_PRESSURE
		case 1 :	
					Data = GetAnalogValue(17,20);
					break;

		// TM.DP_POWER
		case 2 :	
					Data = GetAnalogValue(22,25);
					break;

		// TM.DP_ADPTEMP
		case 3 :	
					Data = GetAnalogValue(27,29);
					break;

		// TM.DP_AUXTEMP
		case 4 :	
					Data = GetAnalogValue(35,37);
					break;

		default :	*Result = FALSE;
					break;
	}	


	if( !(*Result) ) { Error_Message( AI , ID1 , ID2 , ID3 , 0 ); return 0; }

	return Data;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) {
	*Result = FALSE;
	if( !(*Result) ) Error_Message( AO , ID1 , ID2 , ID3 , 0 ); 
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) 
{
	int Data = 0;

	switch( ID2 ) 
	{
		// TM.DP_COMMSTAT
		case 1 :	if(CommStatus)	Data = 1;
					else			Data = 0;
					*Result = TRUE;
					break;

		// TM.DP_ERRSTAT
		case 2 :	if(ErrStatus)	Data = 1;
					else			Data = 0;
					*Result = TRUE;
					break;

		// TM.DP_PUMPSTAT
		case 3 :	if(gpSTA[5] == '0' )			Data = 0;
					else if ( gpSTA[5] = '1' )		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_CONTROLSTAT
		case 4 :	Data = Get_Bit_Status( gpSTA[15] , 0);
					break;

		// TM.DP_PRESS_FAULT
		case 5 :	if (gpSTA[47] == '0')			Data = 0;
					else if(gpSTA[47] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_AUXTEMP_FAULT
		case 6 :	if(gpSTA[48] == '0')			Data = 0;
					else if(gpSTA[48] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_CONSUMPTION_FAULT
		case 7 :	if(gpSTA[49] == '0')			Data = 0;
					else if(gpSTA[49] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_EANALOG_FAULT
		case 8 :	if(gpSTA[50] == '0')			Data = 0;
					else if(gpSTA[50] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_E1LOGIN_FAULT
		case 9 :	if(gpSTA[51] == '0')			Data = 0;
					else if(gpSTA[51] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_E2LOGIN_FAULT
		case 10 :	if(gpSTA[52] == '0')			Data = 0;
					else if(gpSTA[52] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_VARIATOR_FAULT
		case 11 :	if(gpSTA[53] == '0')			Data = 0;
					else if(gpSTA[53] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_WATERFLOW_FAULT
		case 12 :	if(gpSTA[54] == '0')			Data = 0;
					else if (gpSTA[54] = '1')	Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_MAINPOWER_FAULT
		case 13 :	if(gpSTA[55] == '0')			Data = 0;
					else if(gpSTA[55] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_MOTORTEMP_FAULT
		case 14 :	if(gpSTA[56] == '0')			Data = 0;
					else if(gpSTA[56] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_VALVE_FAULT
		case 15 :	if(gpSTA[57] == '0')			Data = 0;
					else if(gpSTA[57] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_BREAKER_FAULT
		case 16 :	if(gpSTA[58] == '0')			Data = 0;
					else if(gpSTA[58] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_N2PURGE_FAULT
		case 17 :	if(gpSTA[59] == '0')			Data = 0;
					else if(gpSTA[59] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_MAINTENANCE_FAULT
		case 18 :	if(gpSTA[60] == '0')			Data = 0;
					else if(gpSTA[60] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_ADPHIGHTEMP_FAULT
		case 19 :	if(gpSTA[61] == '0')			Data = 0;
					else if(gpSTA[61] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_ADPLOWTEMP_FAULT
		case 20 :	if(gpSTA[62] == '0')			Data = 0;
					else if(gpSTA[62] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_LLPUMP_FAULT
		case 21 :	if(gpSTA[64] == '0')			Data = 0;
					else if(gpSTA[64] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		// TM.DP_E3LOGIG_FAULT
		case 22 :	if(gpSTA[65] == '0')			Data = 0;
					else if(gpSTA[65] = '1')		Data = 1;
					else							*Result = FALSE;
					break;

		default :	*Result = FALSE;
					break;
	}

	if( !(*Result) ) { Error_Message( DI , ID1 , ID2 , ID3 , ID4 ); return 0; }

	return Data;
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) 
{
	char Command[256] , Response[256] , Buffer[256];
	DWORD rMutex;

	gnOutCount++;
	rMutex = WaitForSingleObject(hMutex, TimeOut);
	if(rMutex == WAIT_OBJECT_0)
	{
		switch( ID2 )
		{
			// TM.DP_DLRCTRL
			case 1 :	sprintf( Buffer , "DLR" );
						*Result = TRUE;
						break;

			// TM.DP_ECHOCTRL
			case 2 :	sprintf( Buffer , "ECH%s" , OffOnParam[Data] ); 
						*Result = TRUE;
						break;

			// TM.DP_PUMPCTRL
			case 3 :	sprintf( Buffer , "SYS%s" , OffOnParam[Data] ); 
						*Result = TRUE;
						break;

			// TM.DP_TPSCTRL
			case 4 :	sprintf( Buffer , "TPS%02d" , Data ); 
						FormatData( ID1 , Buffer , Command );
						*Result = Send_Command( Command , Response , Ret_No );
						break;

			// TM.DP_ROOTSCTRL
			case 5 :	sprintf( Buffer , "ROO%s" , OffOnParam[Data] ); 
						*Result = TRUE;
						break;

			// TM.DP_PURGECTRL
			case 6 :	sprintf( Buffer , "PUR%s" , OffOnParam[Data] ); 
						*Result = TRUE;
						break;

			default :	*Result = FALSE;
						break;
		}
	
		if((*Result) && (ID2 != 4))
		{
			FormatData( ID1 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );
			if ( *Result )
			{
				if ( strncmp( Response+DATA_POS , "OK" , 2 ) != 0 ) { *Result = FALSE; ErrStatus = TRUE; }
				else ErrStatus = FALSE;
			}
		}

		ReleaseMutex(hMutex);
	}
	gnOutCount--;

	if( !(*Result) ) Error_Message( DO , ID1 , ID2 , ID3 , ID4 );
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result )
{
	*Result = FALSE;
	if( !(*Result) ) Error_Message( SI , ID1 , 0 , 0 , 0 );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {
	*Result = FALSE;
	if( !(*Result) ) Error_Message( SO , ID1 , 0 , 0 , 0 );
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

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void POLLING_THREAD()
{
	DWORD rMutex;
	char Command[256];

	FormatData( PumpAddress, "STA", Command );

	do
	{
#ifdef _DEBUG
		printf("[ALCATEL DRIVER]Polling Thread[%d]\n", gnOutCount);
#endif
		if(gnOutCount == 0)
		{
			rMutex = WaitForSingleObject(hMutex, 0);
			if(rMutex == WAIT_OBJECT_0)
			{			
				if(!Send_Command( Command , gpSTA, Ret_Yes ))
				{
					Error_Message( PS , 0, 0, 0, 0 );
				}
				ReleaseMutex(hMutex);
			}
#ifdef _DEBUG
			printf("[ALCATEL DRIVER]Polling Thread[%s]\n", gpSTA);
#endif
		}
		Sleep(POLLING_PERIOD);
	}while(1);
}

