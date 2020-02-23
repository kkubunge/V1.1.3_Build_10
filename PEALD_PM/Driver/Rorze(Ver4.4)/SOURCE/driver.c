#include "cimrs232.h"
#include "driver.h"
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
/*----------------------------------------------------------------------------------------------------------------------
#  REVISION HISTORY
#------------+------------+------------+--------------------------------------------------------------------------------
#    DATE    |   WRITER   | VERSION    |  ETC
#------------+------------+------------+--------------------------------------------------------------------------------
# 2001.07.06 |            |   v1.0     | Initial Version.
#------------+------------+------------+--------------------------------------------------------------------------------
# 2005.11.24 |   joyeom   |   v2.0     | 1 st Revision.
#------------+------------+------------+--------------------------------------------------------------------------------
# 2006.04.24 |   jyjang   |   v3.0     | 2 nd Revision. ==> 동시 Set Command("MA" , "MB") 추가
#------------+------------+------------+--------------------------------------------------------------------------------
# 2009.02.03 |   jyjang   |   v4.0     | 1. Add IO : AI/AO 1-3-1(High Speed) 
#			 |            |			   |				   1-3-2(Low Speed) 
#			 |            |			   |				   1-3-3(Frequency Rate) 
#			 |            |			   |				   1-3-4(Accelation Time) 
#			 |            |			   |  				   1-3-5(S-curve Rate)    
#------------+------------+------------+------------------------------------------------------------------------------*/
#include "kutletc.h"
#include "kutlanal.h"
#include "kutlstr.h"
//-----------------------------------------------------------------------------------
enum { Ret_No , Ret_Yes };
//-----------------------------------------------------------------------------------
char  SendTermStr[16];
char  RecvTermStr[16];
int   TimeOut = 100;
char  Header[256];
//---> Add 2006/04/24 for Simultaneous Motor Set Action(Ver3.0)
int	  SetData1 , SetData2;
//------------------------> End ( 1 of 4 ) 2006/04/24
//-----------------------------------------------------------------------------------
BOOL  CommStatus = FALSE;
//BOOL  ErrStatus =  FALSE; NotUse Delete
//-->ADD 2012/03/21 Communication Logging Option by JTM
int	LOG_ONOFF	=	0; 
char*	LogDrive;
//-->END 2012/03/21 Communication Logging Option by JTM
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
__declspec(dllexport) void DEVICE_INFO ( char *data , int len ) {
	//strncpy( data , "Ver 1.0 (2004.09.16)" , len );
	//strncpy( data , "Ver 2.0 (2005.11.24)" , len );
	strncpy( data , "Ver 4.4 (2014.03.10)" , len );
}

char 	DRV_VERSION[50]	=	"RORZE_MOTOR_DRV_VER. 4.4";
char 	DRV_BUILDDATE[50] =	"2014.03.11"; //2014/03/11 에외처리 보강

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	SendTermStr[0] = 13;
	SendTermStr[1] = 0 ;
	RecvTermStr[0] = 13;
	RecvTermStr[1] = 0 ;

	if ( ID10 < 100 ) TimeOut = 100;
	else              TimeOut = ID10;

	//--> Modify 2012/03/21 Driver Version 표기 by JTM
	//return( RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ) );
	if ( !RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ) )
	{
		_sleep(1500);

		printf("RORZE MOTOR DRIVER COMM OPEN FAILED....!!!!!\n" ); 

		MessageBox( GetTopWindow( NULL ) , "RORZE MOTOR DRIVER COMM OPEN FAILED!", "RORZE MOTOR", MB_ICONHAND );
		
		return FALSE;
	}
	else
	{
		if ( File != NULL )
		{
			LogDrive = File;//I/O Driver선언시 ID1(Port)앞에 선언된 문자열을 전역변수로 넣어줌...20100113, ybkim
		}	
	
		
		printf("RORZE MOTOR DRIVER COMM OPEN SUCCESS.[Ver:%s / Rebuild_Date:%s] \n" , DRV_VERSION, DRV_BUILDDATE );  
		CommStatus = TRUE;
	}
	return TRUE;
	//--> Modify 2012/03/21 Driver Version 표기 by JTM

}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	RS232_Disconnect_Port();
	return TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

void FormatData( int Add , char *UnFormat , char *Format ) {
	int i;
	for ( i = 0 ; i < 256 ; i++ ) Format[i] = 0;

	sprintf( Format , "%s%s" , Header , UnFormat );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Rs232_IO_Send( char *Cmnds ) 
{
	char Buffer[256];
	memset( Buffer, 0, sizeof(Buffer) );
	strcpy( Buffer, Cmnds);
	strcat( Buffer, SendTermStr );

	return ( RS232_Write_String( Buffer , strlen( Buffer ) ) );
    //printf( "Rorze Motor Command Send ==> %s\n", Cmnds );
	//return( RS232_Write_String( SendTermStr , strlen( SendTermStr ) ) );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

BOOL Rs232_IO( char *Command , char *Response ) {
	int Result;
	int ReadCnt;
	Result = Rs232_IO_Send( Command );
	Result = RS232_Read_String( Response , RecvTermStr , 255 , TimeOut , &ReadCnt );
	if ( !Result ) CommStatus = FALSE;
	else           CommStatus = TRUE;
//	printf("Rorze Motor Command ==> %s\n", Command );
//	printf("Rorze Motor Respons ==> %s\n", Response );
	return Result;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-->2012/03/21 Communication Logging Option by JTM
void TimeLog( int Log_Option )
{
	FILE *fp = NULL;
	char FilePath[MAX_PATH] , FileName[64];
	SYSTEMTIME	st;	
	
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	
	memset(FileName,0,sizeof(FileName));
	memset(FilePath,0,sizeof(FilePath));
	
	if( Log_Option != 1) return ;
	
	GetLocalTime( &st );
	
	if ( strlen(LogDrive) >= 2) { //드라이버 문자열을 선언하지 않은 경우에는 dll파일의 경로와 파일명이 넘어온다.
		sprintf( FilePath , "DATALOG\\Rorze");
		sprintf( FileName , "DATALOG\\Rorze\\COM_%04d%02d%02d%02d.log" , st.wYear , st.wMonth , st.wDay , st.wHour );
		
		
	}
	else if ( strlen(LogDrive) == 1 ) {//드라이버 문자열을 선언한 경우
		sprintf( FilePath , "%s:\\DATALOG\\Rorze" , LogDrive );
		sprintf( FileName , "%s:\\DATALOG\\Rorze\\COM_%04d%02d%02d%02d.log" , LogDrive, st.wYear , st.wMonth , st.wDay , st.wHour );		
		
	}
	
	//Logging 폴더 생성
	hSrch=FindFirstFile(FilePath,&wfd);
	if (hSrch == INVALID_HANDLE_VALUE) 	
	{
		if ( !CreateDirectory ( FilePath , NULL )) 
		{
			printf( "[Matcher_DRIVER] : Can't Create Log File\n"  );
			FindClose(hSrch);
			return; 
		}
	}
	FindClose(hSrch);
	//
	
	RS232_Logging_Style( 0 , FileName );
	RS232_Logging_Control( Log_Option );
}
//-----------------------------------------------------------------------------------
BOOL Send_Command( char *Command , char *Response , int Return ) {

	//-->2012/03/21 Communication Logging Option by JTM
	TimeLog( LOG_ONOFF );

	RS232_Clear_Buffer();
	if( Return ) {
		if( !Rs232_IO( Command , Response ) ) return FALSE;
	}
	else {
		if( !Rs232_IO_Send( Command ) ) return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	char Command[256] , Buffer[256] , Response[256], Temp1[10] ;
	double Data = 0;
	int i;
	for ( i = 0 ; i < 256 ; i++ ) Buffer[i] = 0;

	//printf("Analog_In Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] \n",ID1,ID2,ID3); 

	if      ( ID1 == 1 ) sprintf( Header , "$1" );
	else if ( ID1 == 2 ) sprintf( Header , "$2" );

    switch( ID2 ) {
	    case 1 :
			switch( ID3 ) {

				case 0 :	// Move Pulse Set Read			
				   sprintf( Buffer , "2D" );
			       FormatData( ID2 , Buffer , Command );
		       	   *Result = Send_Command( Command , Response , Ret_Yes );
                   if ( *Result ) {
				       strncpy( Temp1 , Response+3, 8 );
				       sprintf( Temp1+8,"\0");
					   Data = atof( Temp1 );
				   }
		           break;
				case 2 :	// ROR1.Motor1_Position /1축 Current Pulse Reading / >$1+0054955<CR>
				   sprintf( Buffer , "61" );
			       FormatData( ID2 , Buffer , Command );
		       	   *Result = Send_Command( Command , Response , Ret_Yes );
                   if ( *Result )
				   {
				       strncpy( Temp1 , Response+3, 8 );
				       sprintf( Temp1+8,"\0" );

					   //printf( ">>> %c %c %c %c ,,%d \n",*Response, *(Response+1), *(Response+2), *(Response+3), strlen( Response ) );

					   if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && ( *(Response+3) == '+' || *(Response+3) == '-' ) && strlen( Response ) == 11 )
					  		Data = atof( Temp1 ); 				   
					   else
						   *Result = FALSE;	
				   }
				   break;
				   
				case 3 :	// ROR1.Motor2_Position / 2축 Current Pulse Reading / >$1+0054955<CR>
					sprintf( Buffer , "62" );
			       FormatData( ID2 , Buffer , Command );
		       	   *Result = Send_Command( Command , Response , Ret_Yes );
                   if ( *Result ) 
				   {
				       strncpy( Temp1 , Response+3, 8 );
				       sprintf( Temp1+8,"\0" );

					   if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && ( *(Response+3) == '+' || *(Response+3) == '-' ) && strlen( Response ) == 11 )
					  		Data = atof( Temp1 ); 				   
					   else
						   *Result = FALSE;	
				   }
				   break;
				   
				default :
					printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not AI\n",ID1,ID2,ID3); 
					*Result = FALSE;  
					break;
			} 
            break;

		case 2 :	// ROR1.Position8_Pulse, ROR1.Position9_Pulse / Home Sensor 감시 시점의 Pulse Reading / >$1+0003933<CR>
			if      ( ID3 < 10 ) 
				sprintf( Buffer , "A00%dD", ID3 );
			else if ( ID3 < 100 )
				sprintf( Buffer , "A0%dD", ID3 );
			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

            if ( *Result )
			{
				strncpy( Temp1 , Response+3, 8 );
			    sprintf( Temp1+8,"\0" );

				if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && ( *(Response+3) == '+' || *(Response+3) == '-' ) && strlen( Response ) == 11 )
					Data = atof( Temp1 ); 				   
				else
				   *Result = FALSE;	
			}
			break;
			
		case 3 : //ROR1.Motor1_High_Speed, 	ROR1.Motor2_High_Speed / 고속이동 Speed Set Reading / >$105000<CR>
			switch( ID3 ) {
				case 1 : sprintf( Buffer , "OHM1" ); break; // Motor1 High Speed Set Read
				case 2 : sprintf( Buffer , "OHM2" ); break; // Motor2 High Speed Set Read
			}
			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result )
			{
				  strncpy( Temp1 , Response+3, 5 );
				  sprintf( Temp1+5,"\0" );

				  if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && strlen( Response ) == 8 )
					Data = atof( Temp1 ); 				   
				  else
					*Result = FALSE;	
			}
			break;
		
		case 4 : //ROR1.Motor1_Low_Speed, ROR1.Motor2_Low_Speed	 / 저속이동 Speed Set Reading / >$100500<CR>
			switch( ID3 ) {
				case 1 : sprintf( Buffer , "OLM1" ); break; // Motor1 Low Speed Set Read
				case 2 : sprintf( Buffer , "OLM2" ); break; // Motor2 Low Speed Set Read
			}
			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result )
			{
				  strncpy( Temp1 , Response+3, 5 );
				  sprintf( Temp1+5,"\0" );

				  if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && strlen( Response ) == 8 )
					Data = atof( Temp1 ); 				   
				  else
					  *Result = FALSE;
			}
			break;
			
		case 5 : //	ROR1.Motor1_Frequency_Rate, ROR1.Motor2_Frequency_Rate / >$100020<CR>
			switch( ID3 ) {
				case 1 : sprintf( Buffer , "OXM1" ); break; // Motor1 Speed Set Read(OX)
				case 2 : sprintf( Buffer , "OXM2" ); break; // Motor2 Speed Set Read(OX)
			}
			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result )
			{
				  strncpy( Temp1 , Response+3, 5 );
				  sprintf( Temp1+5,"\0" );

				  if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && strlen( Response ) == 8 )
					Data = atof( Temp1 ); 				   
				  else
					  *Result = FALSE;
			}
			break;
			
		case 6 :	// ROR1.Motor1_Accel_Time, ROR1.Motor2_Accel_Time / >$100300<CR>
			switch( ID3 ) {
				case 1 : sprintf( Buffer , "OSM1" ); break; // Motor1 Accelation Time(OS)
				case 2 : sprintf( Buffer , "OSM2" ); break; // Motor2 Accelation Time(OS)
			}
			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result )
			{
				  strncpy( Temp1 , Response+3, 5 );
				  sprintf( Temp1+5,"\0" );

				  if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && strlen( Response ) == 8 )
					Data = atof( Temp1 ); 				   
				  else
					  *Result = FALSE;
			}
			break;
			
		case 7 : //	ROR1.Motor1_S_Curve_Rate, ROR1.Motor2_S_Curve_Rate / >$1000<CR>
			switch( ID3 ) {
				case 1 : sprintf( Buffer , "OCM1" ); break; // Motor1 S-curve Rate(OC)
				case 2 : sprintf( Buffer , "OCM2" ); break; // Motor2 S-curve Rate(OC)
			}
			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result ) {
				  strncpy( Temp1 , Response+3, 5 );
				  sprintf( Temp1+5,"\0" );

				  if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && strlen( Response ) == 6 )
					Data = atof( Temp1 ); 				   
				  else
					*Result = FALSE;
			}
			break;
			
			default :
				printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not AI\n",ID1,ID2,ID3); 
			*Result = FALSE; 
			break;
	} 

	if( !( *Result ) ) { 
		printf("Rorze Motor = AI [ID1]-[ID2]-[ID3] => [%d]-[%d]-[%d] Error\n",ID1,ID2,ID3); 
		return 0;
	}

	return Data;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) {
	char Buffer[256] , Command[256] , Response[256] ;
	int i;
	for ( i = 0 ; i < 256 ; i++ ) Buffer[i] = 0;

	//printf("Analog_Out Rorze Motor = [ID1]-[ID2]-[ID3]-[Data] : [%d]-[%d]-[%d]-[%f] \n",ID1,ID2,ID3,Data); 

	if      ( ID1 == 1 ) sprintf( Header , "$1" );
	else if ( ID1 == 2 ) sprintf( Header , "$2" );

    switch( ID2 ) {
	    case 1 :
			switch( ID3 ) { 

				case 0 :	// // ROR1.Move_Pulse_Set /1축 이동 Pulse Set / ><CR>
				   sprintf( Buffer , "2%d", ( int ) Data );
			       FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_Yes );

				   if ( *Result )
				   {					   
					   if ( *Response != '>' || strlen( Response ) != 1 )
						   *Result = FALSE;
				   }

				   break;
			
				case 4 :	//ROR1.All_Move_Pulse_Set / ><CR>
				   sprintf( Buffer , "MA001%d,%d", SetData1 , SetData2 );
			       FormatData( ID2 , Buffer , Command );

				   *Result = Send_Command( Command , Response , Ret_Yes );

				   //printf( ">>> %c %c %c %c ,,%d \n",*Response, *(Response+1), *(Response+2), *(Response+3), strlen( Response ) );


				   if ( *Result )
				   {					   
					   if ( *Response != '>' || strlen( Response ) != 1 )
						   *Result = FALSE;
				   }

				   break;


 			    default :
					printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not AO\n",ID1,ID2,ID3); 
					*Result = FALSE; 
					break;
			} 
		    break;

		case 2 :	// Position Pulse Set
			if      ( ID3 < 10 )
				sprintf( Buffer , "A00%d%d", ID3, ( int ) Data );
			else if ( ID3 < 100 )
				sprintf( Buffer , "A0%d%d", ID3, ( int ) Data );

			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_No );

		    break;

		case 3 :
			switch( ID3 ) { 
				case 1 : sprintf( Buffer , "OHM1,%05d", ( int ) Data ); break; // Motor1 High Speed Set
 				case 2 : sprintf( Buffer , "OHM2,%05d", ( int ) Data ); break; // Motor2 High Speed Set
			}

			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

		    if ( *Result )
			{					   
			   if ( *Response != '>' || strlen( Response ) != 1 )
				   *Result = FALSE;
			}

			break;

		case 4 :  //ROR1.Motor1_Low_Speed, ROR1.Motor2_Low_Speed	 / 저속이동 Speed Set / ><CR>
			switch( ID3 ) { 
				case 1 : sprintf( Buffer , "OLM1,%05d", ( int ) Data ); break; // Motor1 Low Speed Set
 				case 2 : sprintf( Buffer , "OLM2,%05d", ( int ) Data ); break; // Motor2 Low Speed Set
			}

			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result )
			{					   
				if ( *Response != '>' || strlen( Response ) != 1 )
					*Result = FALSE;
			}

			break;

		case 5 :  //	ROR1.Motor1_Frequency_Rate, ROR1.Motor2_Frequency_Rate / ><CR>
			switch( ID3 ) { 
				case 1 : sprintf( Buffer , "OXM1,%05d", ( int ) Data ); break; // Motor1 Speed Set(OX)
 				case 2 : sprintf( Buffer , "OXM2,%05d", ( int ) Data ); break; // Motor2 Speed Set(OX)
			}

			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );
			if ( *Result )
			{					   
				if ( *Response != '>' || strlen( Response ) != 1 )
					*Result = FALSE;
			}
			break;

		case 6 : // ROR1.Motor1_Accel_Time, ROR1.Motor2_Accel_Time / ><CR>
			switch( ID3 ) { 
				case 1 : sprintf( Buffer , "OSM1,%05d", ( int ) Data ); break; // Motor1 Accelation Time(OS)
 				case 2 : sprintf( Buffer , "OSM2,%05d", ( int ) Data ); break; // Motor2 Accelation Time(OS)
			}

			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result )
			{					   
				if ( *Response != '>' || strlen( Response ) != 1 )
					*Result = FALSE;
			}

			break;

		case 7 :  //	ROR1.Motor1_S_Curve_Rate, ROR1.Motor2_S_Curve_Rate / ><CR>
			switch( ID3 ) { 
				case 1 : sprintf( Buffer , "OCM1,%05d", ( int ) Data ); break; // Motor1 S-curve Rate(OC)
 				case 2 : sprintf( Buffer , "OCM2,%05d", ( int ) Data ); break; // Motor2 S-curve Rate(OC)
			}

			FormatData( ID3 , Buffer , Command );
			*Result = Send_Command( Command , Response , Ret_Yes );

			if ( *Result )
			{					   
				if ( *Response != '>' || strlen( Response ) != 1 )
					*Result = FALSE;
			}
 
			break;
			
		default :
			printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not AO\n",ID1,ID2,ID3); 
			*Result = FALSE; 
			break;
	} 

	if( !(*Result) ) {
		printf("Rorze Motor = AO [ID1]-[ID2]-[ID3] => [%d]-[%d]-[%d] Error\n",ID1,ID2,ID3); 		
	}
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	char Buffer[256] , Command[256] , Response[256], Temp2[10];
	int  i, j, Data = 0, Data1 = 0;
	for ( i = 0 ; i < 256 ; i++ ) Buffer[i] = 0;

	//printf("Digital_In Rorze Motor = [ID1]-[ID2]-[ID3]-[ID4] : [%d]-[%d]-[%d]-[%d] \n",ID1,ID2,ID3,ID4); 

	if      ( ID1 == 1 ) sprintf( Header , "$1" );
	else if ( ID1 == 2 ) sprintf( Header , "$2" );

	switch( ID2 ) {

		case  1 :  if( CommStatus ) Data = 1; // comm status
				   else 		    Data = 0;
				   *Result = TRUE;
	               break;	


		case  5 : 	//	ROR1.Motor_Status_DI / Error Check / >$10<CR>
				*Result = Send_Command( "$1" , Response , Ret_Yes );
				
				if ( *Result )
				{
					if ( *Response == '>' && *(Response+1) == '$' && *(Response+2) == '1' && strlen( Response ) == 4 )
					{
						if( *(Response+3) == 'A' )
							Data = 10;
						else if( *(Response+3) == 'B' )
							Data = 11;
						else if( *(Response+3) == 'C' )
							Data = 12;
						else if( *(Response+3) == 'D' )
							Data = 13;
						else if( *(Response+3) == 'E' )
							Data = 14;
						else if( *(Response+3) == 'F' )
							Data = 15;
						else							
							Data = atoi( Response+3 );

						if( Data < 0 || Data > 15 )
							*Result = FALSE;
					}
					else					
						*Result = FALSE;
					
				}
				break;

		case  10 :   
			Data = LOG_ONOFF;
			*Result = TRUE;
					break;

	   default :
		    printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not DI\n",ID1,ID2,ID3); 
		    *Result = FALSE;  
		    break;

	}

    if( !(*Result) ) { 	
		printf("Rorze Motor = DI [ID1]-[ID2]-[ID3]-[ID4] => [%d]-[%d]-[%d]-[%d] Error\n", ID1,ID2,ID3,ID4);
		return 0; 
	}

	return Data;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) {
	char Buffer[256] , Command[256] , Response[256], Temp1[10], Temp2[10];
	int  i , Err=0;

	//printf("Digital_Out Rorze Motor = [ID1]-[ID2]-[ID3]-[ID4]-[Data] : [%d]-[%d]-[%d]-[%d]-[%f] \n",ID1,ID2,ID3,ID4,Data); 

	for ( i = 0 ; i < 256 ; i++ ) Buffer[i] = 0;

	if      ( ID1 == 1 ) sprintf( Header , "$1" );
	else if ( ID1 == 2 ) sprintf( Header , "$2" );

	switch ( ID2 ) {
		case 2 :   
			switch ( ID3 ) {
			   case 1 :		// Motor Stop
				   if      ( Data == 0 )
		              sprintf( Buffer , "S1" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "SS1" );
				   else if ( Data == 2 )
		              sprintf( Buffer , "S2" );
				   else if ( Data == 3 )
		              sprintf( Buffer , "SS2" );
				   else if ( Data == 4 )
		              sprintf( Buffer , "S3" );
				   else if ( Data == 5 )
		              sprintf( Buffer , "SS3" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 2 :		// Motor Slow1 Home
				   if      ( Data == 0 )
		              sprintf( Buffer , "01" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "02" );
				   else 
		              sprintf( Buffer , "03" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 3 :		// Motor Slow2 Home
				   if      ( Data == 0 )
		              sprintf( Buffer , "0R1" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "0R2" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   //	ROR1.Motor_Fast1_Home / 고속 원점 찾기 / ><CR>
			   case 4 :		// Motor Fast1 Home
				   if      ( Data == 0 )
		              sprintf( Buffer , "0H1" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "0H2" );
				   else 
		              sprintf( Buffer , "0H3" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_Yes );

				   if ( *Result )
				   {					   
					   if ( *Response != '>' || strlen( Response ) != 1 )
						   *Result = FALSE;
				   }

			   break;

			   case 5 :		// Motor Fast2 Home
				   if      ( Data == 0 )
		              sprintf( Buffer , "0A1" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "0A2" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   default :
				   printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not DO\n",ID1,ID2,ID3); 
				   *Result = FALSE; 
				   break;

			} 
            break; 

		case 4 :   
			switch ( ID3 ) {
				case 1 :  //	ROR1.Motor_Move_Home / 현재 Pulse만큼 Down 시킴 / ><CR>

				   if      ( Data == 0 )
		              sprintf( Buffer , "11" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "12" );
				   else 
		              sprintf( Buffer , "13" );
				   FormatData( ID2 , Buffer , Command );

				   *Result = Send_Command( Command , Response , Ret_Yes );
				   if ( *Result )
				   {					   
					   if ( *Response != '>' || strlen( Response ) != 1 )
						   *Result = FALSE;
				   }

			   break;

			   case 2 :		//	ROR1.Motor_Move_ABS / 한축 이동 시킴 / ><CR>
				   if      ( Data == 0 )
		              sprintf( Buffer , "31" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "32" );
				   else if ( Data == 2 )
		              sprintf( Buffer , "33" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_Yes );
				   if ( *Result )
				   {					   
					   if ( *Response != '>' || strlen( Response ) != 1 )
						   *Result = FALSE;
				   }
			   break;

			   case 3 :		// Motor Move CW Set 
				   if      ( Data == 0 )
		              sprintf( Buffer , "41" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "42" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 4 :		// Motor Move CCW Set 
				   if      ( Data == 0 )
		              sprintf( Buffer , "51" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "52" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 5 :		// Motor Move Slow Rotate CW Set 
				   if      ( Data == 0 )
		              sprintf( Buffer , "71" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "72" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 6 :		// Motor Move Slow Rotate CCW Set 
				   if      ( Data == 0 )
		              sprintf( Buffer , "81" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "82" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 7 :		// Motor Move Pulse Rotate CW Set 
				   if      ( Data == 0 )
		              sprintf( Buffer , "7*1" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "7*2" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 8 :		// Motor Move Pulse Rotate CCW Set 
				   if      ( Data == 0 )
		              sprintf( Buffer , "8*1" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "8*2" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 9 :		// Motor Slot Position Copy to RAM from EEPROM 
				   if      ( Data == 0 )
		              *Result = TRUE;
				   else if ( Data == 1 ) {
		                sprintf( Buffer , "AL" );
					    FormatData( ID2 , Buffer , Command );
						*Result = Send_Command( Command , Response , Ret_No );
				   }
			   break;

			   case 10 :		// Motor Slot Position Write EEPROM 
				   if      ( Data == 0 )
		              *Result = TRUE;
				   else if ( Data == 1 ) {
		                sprintf( Buffer , "AW" );
					    FormatData( ID2 , Buffer , Command );
						*Result = Send_Command( Command , Response , Ret_No );
				   }
			   break;

			   case 11 :		// Motor Speed etc Copy to RAM from EEPROM 
				   if      ( Data == 0 )
		              *Result = TRUE;
				   else if ( Data == 1 ) {
		                sprintf( Buffer , "DL" );
					    FormatData( ID2 , Buffer , Command );
						*Result = Send_Command( Command , Response , Ret_No );
				   }
			   break;

			   case 12 :		// Motor Speed etc Write EEPROM 
				   if      ( Data == 0 )
		              *Result = TRUE;
				   else if ( Data == 1 ) {
		                sprintf( Buffer , "DW" );
					    FormatData( ID2 , Buffer , Command );
						*Result = Send_Command( Command , Response , Ret_No );
				   }
			   break;

			   case 13 :		// Motor EEPROM Data Delete
				   if      ( Data == 0 )
		              *Result = TRUE;
				   else if ( Data == 1 ) {
		                sprintf( Buffer , "EE///" );
					    FormatData( ID2 , Buffer , Command );
						*Result = Send_Command( Command , Response , Ret_No );
				   }
			   break;

			   case 14 :		// Motor Communication CR( Terminal String ) Set 
				   if      ( Data == 0 )
		              sprintf( Buffer , "EL0" );
				   else if ( Data == 1 )
		              sprintf( Buffer , "EL1" );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   case 15 :	//	ROR1.Motor_Move_All / 두축 이동 시킴 / ><CR>
				   if      ( Data == 0 )
		              *Result = TRUE;
				   else if ( Data == 1 ) {
		                sprintf( Buffer , "MB001" );
					    FormatData( ID2 , Buffer , Command );

						*Result = Send_Command( Command , Response , Ret_Yes );
						if ( *Result )
						{					   
							if ( *Response != '>' || strlen( Response ) != 1 )
								*Result = FALSE;
					  }
				   }
			   break;
			   //------------------------> End ( 3 of 4 ) 2006/04/24

			   default :
				   printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not DO\n",ID1,ID2,ID3); 
				   *Result = FALSE; 
				   break;

			} 
            break; 

		case 5 :		// Motor Slot Position Moving    
			if ( Data == 0 ) {
				 if (ID3 < 10 ) sprintf( Buffer , "B00%d1", ID3  );
				 else if (ID3 < 100 ) sprintf( Buffer , "B0%d1", ID3  );
				 else sprintf( Buffer , "B%d1", ID3  );
			}
			else if ( Data == 1 ) {
				 if (ID3 < 10 ) sprintf( Buffer , "B00%d2", ID3  );
				 else if (ID3 < 100 ) sprintf( Buffer , "B0%d2", ID3  );
				 else sprintf( Buffer , "B%d2", ID3  );

			}
			 FormatData( ID2 , Buffer , Command );
			 *Result = Send_Command( Command , Response , Ret_No );
		break;

		case 6 :		// Motor Echo Back Set    
			switch ( ID3 ) {
			   case 1 :
				   sprintf( Buffer , "EE%d", Data );
				   FormatData( ID2 , Buffer , Command );
				   *Result = Send_Command( Command , Response , Ret_No );
			   break;

			   default :
				   *Result = FALSE;
					break;
			} 
            break; 

		//-->2012/03/21 Communication Logging Option by JTM
		case  10 :   
			LOG_ONOFF = Data;
			*Result = TRUE;
			break;
			
		default :
		    printf("Rorze Motor = [ID1]-[ID2]-[ID3] : [%d]-[%d]-[%d] is not DO\n",ID1,ID2,ID3); 
		    *Result = FALSE; 
		    break;

	} 

	if( !( *Result ) ) {
		printf("Rorze Motor = DO [ID1]-[ID2]-[ID3]-[ID4] => [%d]-[%d]-[%d]-[%d] Error\n", ID1,ID2,ID3,ID4);
		
	}
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	
	switch ( ID1 ) {
		
	case 98 :	
		strncpy( data , DRV_VERSION , sizeof(DRV_VERSION) );
		*Result    = TRUE;
		break;
		
	case 99 :	
		strncpy( data , DRV_BUILDDATE , sizeof(DRV_BUILDDATE) );
		*Result    = TRUE;
		break;
	}
	
}
//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {
	//---> Add 2006/04/24 for Simultaneous Motor Set Action(Ver3.0)
	char StrData1[256] , StrData2[256];
	STR_SEPERATE_CHAR( data	, ',' , StrData1 , StrData2 , 255 );
	SetData1 = atoi ( StrData1 );
	SetData2 = atoi ( StrData2 );
	//------------------------> End ( 4 of 4 ) 2006/04/24
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
