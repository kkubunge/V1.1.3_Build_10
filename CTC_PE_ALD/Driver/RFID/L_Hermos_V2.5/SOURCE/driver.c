#include "CimRs232.h"
#include "kutletc.h"
#include "kutlstr.h"
#include "driver.h"
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
enum	{ Ret_No , Ret_Yes };
enum	{	  DO	  ,	DI		 ,	AO		  ,	 AI		      ,	 SO		     ,	SI	};
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
char  RecvTermStr[16] ;
int   TimeOut = 100;
int  Target_Address = 0 ;
//-----------------------------------------------------------------------------------
BOOL  CommStatus = FALSE;
//-----------------------------------------------------------------------------------
char	Error_Code = '0';
//-----------------------------------------------------------------------------------
#define MAX_RFID_STR 100
char  BarData[20][MAX_RFID_STR];
char  BarDataSet[MAX_RFID_STR];
//-----------------------------------------------------------------------------------
// [ 2010.06.03    Driver Version & Driver Build date ADD By khkim
char 	DRV_VERSION[50]	=	"Hermos_DRV_Ver. 2.5";
char 	DRV_BUILDDATE[50] =	"2016.03.14";
// ]
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {

//	BOOL	CONNECT_SUCESS = FALSE;
	
	RecvTermStr[0] = 0x0D; // CR
	RecvTermStr[1] = 0;

	Target_Address = ID8;

	if ( ID10 < 100 ) TimeOut = 100;
	else              TimeOut = ID10;

	// [ 2010.06.03    Driver Version & Driver Build date ADD By khkim
	//	CONNECT_SUCESS = RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 );
    //	return CONNECT_SUCESS ;	
	if ( !RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ) ) {
	_sleep(1500);
	printf("Hermors Driver COMM OPEN FAILED....!!!!!\n" ); return FALSE;
	}
	else printf("Hermors[%d] Driver COMM OPEN SUCCESS...[Ver:%s / Rebuild_Date:%s] \n" , ID9, DRV_VERSION, DRV_BUILDDATE );  
		CommStatus = TRUE;
	return TRUE;
	// ]


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
// Make CRC
//-----------------------------------------------------------------------------------
void Get_CRC( int Mode , char *Command , char *CRC , int Data ){
	unsigned char Result[128];
	char CRC1[128] , CRC2[128];
	int DataBuf, DataBuf2, i;
	BOOL Change1 = FALSE , Change2 = FALSE ;

	memset( Result	, 0 , 128 );
	memset( CRC1	, 0 , 128 );
	memset( CRC2	, 0 , 128 );

	if      ( Mode == 1 ) {
		Result[0] = Command[0];
		for ( i = 1 ; i< (int)strlen( Command ) ; i++ ) { 
			Result[0] ^= Command[i];
			//printf( "Result XOR[%d] = %c\n" , i , Result[0] );
		}
		//printf( "Result All XOR = char[%c] , int[%d]\n" , Result[0] , Result[0] );
		Data = Result[0];
	}
	else if ( Mode == 2 ) {
		Result[0] = Command[0];
		//printf( "Command[0] = [%X]\n" , Result[0] );
		for ( i = 1 ; i< (int)strlen(Command) ; i++ ) { 
			Result[0] += Command[i];
			//printf( "Command[%d] = [%X] , ADD[%d] = [%X]\n" , i , Command[i] , i , Result[0] );
		}
		//printf( "Result All ADD = char[%c] , int[%d]\n" , Result[0] , Result[0] );
		Data = Result[0];
	}

	DataBuf  = Data/16 ;
	DataBuf2 = Data%16 ; 

	if (  DataBuf >= 10 && DataBuf  <= 15 ) {
		DataBuf  = DataBuf  + 55 ;
		Change1 = TRUE ;
	}

	if (  DataBuf2>= 10 && DataBuf2 <= 15 )  {
		DataBuf2 = DataBuf2 + 55 ;
		Change2 = TRUE ;
	}

	if	    ( Change1 == TRUE   && Change2 == TRUE  ) sprintf( CRC , "%c%c", DataBuf , DataBuf2 ); 
	else if ( Change1 == TRUE   && Change2 == FALSE ) sprintf( CRC , "%c%x", DataBuf , DataBuf2 ); 
	else if ( Change1 == FALSE  && Change2 == TRUE  ) sprintf( CRC , "%x%c", DataBuf , DataBuf2 ); 
	else	sprintf( CRC , "%x%x", DataBuf , DataBuf2 ); 

	//printf( "			[HERMOS] Mode[%d] : Command[%s] , Result[%c] , Data[%d]\n" , Mode , Command , Result[0] , Data);
	//printf( "			[HERMOS]            DataBuf[%d] , DataBuf2[%d] , CRC[%s]\n" , DataBuf , DataBuf2 , CRC);
}
//-----------------------------------------------------------------------------------
//Command를 지정된 형식으로 만든다. SR + LEN + [ COMMAND ] + ER + CRC1 + CRC2
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Get_Command( char *Command ) {
	char SR[128] , ER[128] , Buffer[128] , CRC[128] , CRC2[128];
	int i;
	
	memset( SR		, 0 , 128 );
	memset( ER		, 0 , 128 );
	memset( Buffer	, 0 , 128 );
	memset( CRC		, 0 , 128 );
	memset( CRC2	, 0 , 128 );

	SR[0] = (char)0x53;  SR[1] = (char)0x00;
	ER[0] = (char)0x0D;  ER[1] = (char)0x00;

	Get_CRC( 0 , Command , CRC , (int) strlen( Command ) );
	

	strcat ( Buffer, SR	    );
	strcat ( Buffer, CRC    );
	strcat ( Buffer, Command);
	strcat ( Buffer, ER	    );
	strcpy( Command, Buffer );

	for ( i = 0 ; i<128  ; i++ ) CRC[i] = 0x00 ;

	Get_CRC( 1 , Command , CRC  , 0 );
	Get_CRC( 2 , Command , CRC2 , 0 );
	strcat ( Command, CRC );
	strcat ( Command, CRC2 );

	//printf( "			[HERMOS] Get Command[%s]\n" , Command );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int ctoi(char str) {

	if(str >= '0' && str <= '9')
		return str - '0';
	else if(str >= 'A' && str <= 'F')
		return str + 10- 'A';
	else return -1;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Hexadecimal (str) - > ascii(str)
void itostr( char* des , char* str , int lenth ) {

	char strTemp[3];
	int i , count = 0;

	memset( strTemp , 0 , 3 );

	//printf("	des = [%s]\n	str = [%s]\n strlen = [%d] , Lenth = [%d]\n\n" , des , str , (int)strlen(str) , lenth );
	if( (int)strlen(str) != lenth ) return;
	for( i = 0 ; i < lenth ; i += 2 ) {
		strncpy( strTemp , str + i , 2 );
		//printf("	strTemp = [%s]\n" , strTemp );
		*( des + count ) = ctoi( strTemp[0] ) * 16 + ctoi( strTemp[1] );
		//printf("	des = [%s]\n" , des );
		count++;
	}
		//printf("	Final des = [%s]\n" , des );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int error_check(char* Receive) {
	char strTemp[3];
	int res = 0;

	if( Receive[6] == '6' && Receive[7] == '5' ) {
		memset(strTemp,0,3);
		itostr(strTemp, Receive+10,	2 );
		if      ( strTemp[0] == '0' )	res = 0;
		else if ( strTemp[0] == '1' )	res = 1;
		else if ( strTemp[0] == '2' )	res = 2;
		else if ( strTemp[0] == '3' )	res = 3;
		else if ( strTemp[0] == '4' )	res = 4;
		else if ( strTemp[0] == '5' )	res = 5;
		else if ( strTemp[0] == '6' )	res = 6;
		else if ( strTemp[0] == '7' )	res = 7;
		else if ( strTemp[0] == '8' )	res = 8;
		else if ( strTemp[0] == '9' )	res = 9;
		else if ( strTemp[0] == 'A' )	res = 10;
		else if ( strTemp[0] == ':' )	res = 11;
		else if ( strTemp[0] == ';' )	res = 12;
		else if ( strTemp[0] == 'B' )	res = 13;
	}
	return res;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//  alter space into 'div'
void space_check( char* str , char div ) {	
	int len = strlen(str) , i;

	for( i = 0 ; i < len ; i ++ ) {
		if( str[i] == ' ' ) str[i] = div;
	}
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// char* res(receive Current Parameters from reader)  Mode 0,1,2,3,4,5,6,7,F
int analysys_receive( char* res ) {
	char str_temp[10];

	itostr( res+12 , str_temp , 2 );
	return atoi( str_temp );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Rs232_IO_Send( char *Command ) {
	
	if( !RS232_Write_String( Command     , strlen( Command  ) ) ) {
		printf( "[HERMOS] RS232 Send Error => Sending : %s\n" , Command );
		return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Rs232_IO( char *Command , char *Response , BOOL Check , int Data ) {
	BOOL Result = TRUE;
	int ReadCnt,  ReadCrcCnt;
	char CrcString[16] , Command2[128];
	
	Get_Command( Command );	

	if ( Check ) {
		sprintf( Command2 , "%s%02d" , Command , Data );
		//sprintf( Command2 , "%s" , Command);
		
		strcpy( Command , Command2 );
	}

	//printf( "			[test] Final Command[%s]\n" , Command );
	RS232_Clear_Buffer();

	Result = Rs232_IO_Send( Command );

	if( Result ) {
		if ( RS232_Read_String( Response , RecvTermStr , 255 , TimeOut , &ReadCnt ) ) {	// Read to ER(0x0D = CR)
			// Error_Code Check
			Error_Code = error_check( Response );
			if ( RS232_Read_Count_Char( CrcString , 4 , &ReadCrcCnt , TimeOut  ) ) {	// Read CRC1/CRC2
					Result = TRUE;	
			}
			else {
				printf( "			[HERMOS] Receive2 Error => Send : [ %s ] , Recive : [ %s ] , CRC String : [%s]\n" , Command , Response , CrcString );
				Result = FALSE;
			}
		}
		else {
			printf( "			[HERMOS] Receive Error1 => Send : [ %s ] , Recive : [ %s ]\n" , Command , Response );
			Result = FALSE;
		}

		// Check Response String
		if	( !Result ) {
			strcat ( Response , CrcString ) ;
			printf( "			[HERMOS] Receive Error3 => Send : [ %s ] , Recive : [ %s ]\n" , Command , Response );
		}
	}

	if ( !Result ) CommStatus = FALSE;
	else           CommStatus = TRUE;

	return Result;
}

BOOL Rs232_IO2( char *Command , char *Response , BOOL Check , int Data ) {
	BOOL Result = TRUE;
	int ReadCnt,  ReadCrcCnt;
	char CrcString[16] , Command2[128];
	
	Get_Command( Command );	

	if ( Check ) {
		//sprintf( Command2 , "%s%02d" , Command , Data );
		sprintf( Command2 , "%s" , Command);
		
		strcpy( Command , Command2 );
	}

	printf( "			[test] Final Command[%s]\n" , Command );
	RS232_Clear_Buffer();

	Result = Rs232_IO_Send( Command );

	if( Result ) {
		if ( RS232_Read_String( Response , RecvTermStr , 255 , TimeOut , &ReadCnt ) ) {	// Read to ER(0x0D = CR)
			// Error_Code Check
			Error_Code = error_check( Response );
			if ( RS232_Read_Count_Char( CrcString , 4 , &ReadCrcCnt , TimeOut  ) ) {	// Read CRC1/CRC2
					Result = TRUE;	
			}
			else {
				printf( "			[HERMOS]2 Receive2 Error => Send : [ %s ] , Recive : [ %s ] , CRC String : [%s]\n" , Command , Response , CrcString );
				Result = FALSE;
			}
		}
		else {
			printf( "			[HERMOS]2 Receive Error1 => Send : [ %s ] , Recive : [ %s ]\n" , Command , Response );
			Result = FALSE;
		}

		// Check Response String
		if	( !Result ) {
			strcat ( Response , CrcString ) ;
			printf( "			[HERMOS]2 Receive Error3 => Send : [ %s ] , Recive : [ %s ]\n" , Command , Response );
		}
	}

	if ( !Result ) CommStatus = FALSE;
	else           CommStatus = TRUE;

	return Result;
}


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Rs232_IO_Repeat( char *Command , char *Response, int count ) {
	BOOL Result = TRUE;
	int ReadCnt,  ReadCrcCnt;
	char CrcString[16];
	int i;
	
	//-------------------------------------------
	Get_Command( Command );	
	//-------------------------------------------
	
	RS232_Clear_Buffer();
	
	Result = Rs232_IO_Send( Command );
	
	if( Result ) {
		for( i = 0 ; i < count ; i ++ )
		{
			// Send Commad & Receive Response
			if ( RS232_Read_String( Response , RecvTermStr , 255 , TimeOut , &ReadCnt )) {	// Read to ER(0x0D = CR)
				// Error_Code Check
				Error_Code = error_check(Response);
				if ( RS232_Read_Count_Char( CrcString , 8 , &ReadCrcCnt , TimeOut  ) )		// Read CRC1/CRC2
					Result = TRUE;		
				else {
					printf( "			[HERMOS] Receive2 Error => Send : [ %s ] , Recive : [ %s ] , CRC String : [%s]\n" , Command , Response , CrcString );
					Result = FALSE;
				}
			}
			else {
				printf( "			[HERMOS] Receive Error1 => Send : [ %s ] , Recive : [ %s ]\n" , Command , Response );
				Result = FALSE;
			}
			
			// Check Response String
			if	( !Result ) {
				strcat ( Response , CrcString ) ;
				printf( "			[HERMOS] Receive Error3 => Send : [ %s ] , Recive : [ %s ]\n" , Command , Response );
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
void Error_Message( int IO , int ID1 , int ID2 , int ID3 , int ID4 ) {
	switch( IO ) {
		case DO :	printf( "[HERMOS] DO Error : [%d][%d][%d][%d]\n" , ID1 , ID2 , ID3 , ID4 ); 
					break;
		
		case DI :	printf( "[HERMOS] DI Error : [%d][%d][%d][%d]\n" , ID1 , ID2 , ID3 , ID4 ); 
					break;
		
		case AO :	printf( "[HERMOS] AO Error : [%d][%d][%d]\n" , ID1 , ID2 , ID3 ); 
					break;
		
		case AI :	printf( "[HERMOS] AI Error : [%d][%d][%d]\n" , ID1 , ID2 , ID3 ); 
					break;
		
		case SO :	printf( "[HERMOS] SO Error : [%d]\n" , ID1 ); 
					break;
		
		case SI :	printf( "[HERMOS] SI Error : [%d]\n" , ID1 ); 	
					break;
		
		default :	printf( "(HERMOS)Unknown data type\n" );
					break;
	}
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	char Command[128] , Response[128] , str_Temp[10];
	double Data = 0;
	
	memset( Command		, 0 , 128 );
	memset( Response	, 0 , 128 );
	memset( str_Temp	, 0 , 10 );

	*Result = FALSE;

	if(ID1 == 1) {
		sprintf( Command , "G%X" , Target_Address);
		if		( ID2 == 1 )	*Result = Rs232_IO_Repeat(Command,Response,1);
		else if	( ID2 == 2 )	*Result = Rs232_IO_Repeat(Command,Response,4);
		else if	( ID2 == 3 )	*Result = Rs232_IO_Repeat(Command,Response,5);
		else if	( ID2 == 4 )	*Result = Rs232_IO_Repeat(Command,Response,6);
		else if	( ID2 == 5 )	*Result = Rs232_IO_Repeat(Command,Response,7);
		else *Result = FALSE;

		if(*Result) Data = (double)analysys_receive(Response);
	}
	if( !(*Result) ) Error_Message( AI , ID1 , ID2 , ID3 , 0 );  
	return Data;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) {
/*
	char Command[128],Response[128];
	
	memset(Command,0,128);
	memset(Response,0,128);
*/
	*Result = TRUE;

	/*    ================      Parameter Setting Part   ================== */
/*
	if(ID1 == 1){			
		switch(ID2){
			case 1 :	sprintf( Command , "P%X0%2d" , Target_Address , (int)Data );	break;	// sensor delay				
			case 2 :	sprintf( Command , "P%X3%2d" , Target_Address , (int)Data );	break;	// r/w repeattime
			case 3 :	sprintf( Command , "P%X4%2d" , Target_Address , (int)Data );	break;	// r/w maxrepeat
			case 4 :	sprintf( Command , "P%X5%2d" , Target_Address , (int)Data );	break;	// RS232 repeattime
			case 5 :	sprintf( Command , "P%X6%2d" , Target_Address , (int)Data );	break;	// RS232 maxrepeat
			default:	*Result = FALSE;
		}
		space_check(Command,'0');		
		if ( *Result &&!((*Result= Rs232_IO( Command , Response )) && strlen( Response ) >= 1))	CommStatus = FALSE;
	}

*/	/*    ================      Parameter Setting Part End  ================== */

	if( !(*Result) ) Error_Message( AO , ID1 , ID2 , ID3 , 0 );  
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	char Command[128] , Response[128] , str_temp[10];
	int Data = 0;
	*Result = TRUE;

	memset( Command		, 0 , 128 );
	memset( Response	, 0 , 128 );
	memset( str_temp	, 0 , 10 );

	switch( ID1 ){
		case 1 : // Communication Status
			     if ( CommStatus )	Data = 1;
				 else				Data = 0;
				 break;

		case 2 : // Error Code Status	
			     Data = Error_Code;
				 break;

/*		case 100 : sprintf( Command , "G%X" , Target_Address);
			
				if		( ID2 == 1 )	*Result = Rs232_IO_Repeat(Command,Response,2);
				else if	( ID2 == 2 )	*Result = Rs232_IO_Repeat(Command,Response,3);
				else if	( ID2 == 3 )	*Result = Rs232_IO_Repeat(Command,Response,8);
				else if	( ID2 == 4 )	*Result = Rs232_IO_Repeat(Command,Response,9);
				else *Result = FALSE;
				if(*Result) Data = analysys_receive(Response);
				
				break;
		case 200 : sprintf( Command , "J%X" , Target_Address);
				if( ( *Result = Rs232_IO(Command,Response) ) )
				{
					itostr(Response+10,str_temp,4);
					if(strcmp(str_temp,"00") == 0) Data = 0;
					else if(strcmp(str_temp,"3F") == 0) Data = 1;
					else *Result = FALSE;			
				}	
				break;				 
*/

	   default : *Result = FALSE;	   
			     break;
	}

	if( !(*Result) ) { 
		Error_Message( DI , ID1 , ID2 , ID3 , ID4 );  
		return Data; 
	}

	return Data;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) {

	char Command[128] , Response[128] , Buffer[128];
	int i , Read_Index;

	memset( Command		, 0 , 128 );
	memset( Response	, 0 , 128 );
	memset( Buffer		, 0 , 10 );

	*Result = TRUE;

	switch( ID1 ){
		case 1 :	// Start an Externally Triggered Read

			// New RFID Reader Change 2014.01.23 by hyuk 
			sprintf( Command , "X%X%2d" , Target_Address , Data+1 );
			//sprintf( Command , "X%X" , Target_Address );

			space_check( Command , '0' );
			
		    //... 2013.03.14
		    memset(BarData[Data+1], 0x00, MAX_RFID_STR);
		    //...

			if ( Rs232_IO2( Command , Response , TRUE , Data+1 ) ) {
				//printf("	Command = [%s]\n	Response [%s]\n" , Command , Response );
				if ( strlen( Response ) <= 0 ) {
					*Result = FALSE;
					CommStatus = FALSE;			
				}
				else {
					*Result = TRUE;
					CommStatus = TRUE;			

					itostr( BarData[Data+1] , Response + 7 , 16 );	
					//for(i = 0 ; i < 20 ; i ++)	printf("BarData[%d] =[%s]\n",i,BarData[i]);						
				}
			}
			else {
				*Result = FALSE;
				CommStatus = FALSE;
			}
			break;

		case 2 :		// Write Tag
			sprintf( Command , "W%X%2d" , Target_Address , Data+1 );
			space_check( Command , '0' );
			Read_Index = strlen( BarDataSet );
			for ( i = 0 ; i < Read_Index ; i++ ) {
				sprintf( Buffer , "%X" , BarDataSet[i] );
				//printf( "Buffer=========>[%s]\n" , Buffer );
				strcat ( Command , Buffer );
			}

			if ( Rs232_IO( Command , Response , FALSE , Data+1 ) ) {
				//printf("	Command = [%s]\n	Response [%s]\n" , Command , Response );
				if ( strlen( Response ) <= 0 ) {
					*Result = FALSE;
					CommStatus = FALSE;			
				}
				else {
					*Result = TRUE;
					CommStatus = TRUE;			

//					itostr( BarData[Data+1] , Response + 7 , 16 );	
//					for(i = 0 ; i < 20 ; i ++)	printf("BarData[%d] =[%s]\n",i,BarData[i]);						
				}
			}
			else {
				*Result = FALSE;
				CommStatus = FALSE;
			}
			break;

		case 3 :		// Start Software Reset
			sprintf( Command , "N%X" , Target_Address );
			space_check( Command , '0' );

			if ( Rs232_IO( Command , Response , FALSE , Data+1 ) ) {
				//printf("	Command = [%s]\n	Response [%s]\n" , Command , Response );
				if ( strlen( Response ) <= 0 ) {
					*Result = FALSE;
					CommStatus = FALSE;			
				}
				else {
					*Result = TRUE;
					CommStatus = TRUE;			
				}
			}
			else {
				*Result = FALSE;
				CommStatus = FALSE;
			}
			break;

		case 4 :	// Lock One Page
			sprintf( Command , "L%X%2d" , Target_Address , Data+1 );
			space_check( Command , '0' );

			if ( Rs232_IO( Command , Response , FALSE , Data+1 ) ) {
				//printf("	Command = [%s]\n	Response [%s]\n" , Command , Response );
				if ( strlen( Response ) <= 0 ) {
					*Result = FALSE;
					CommStatus = FALSE;			
				}
				else {
					*Result = TRUE;
					CommStatus = TRUE;			
				}
			}
			else {
				*Result = FALSE;
				CommStatus = FALSE;
			}
			break;

/*
		case 100 : 		//  Parameter Set ex> "P0000"   (command:P,TargetAddress:0,Parameter:0,value:00)			
				if(ID2 == 1){
					sprintf( Command , "P%X1" , Target_Address);
					switch(Data){
						case 0 : strcat(Command,"00");	break;
						case 1 : strcat(Command,"01");	break;
						case 2 : strcat(Command,"01");	break;
						case 3 : strcat(Command,"10");	break;
						case 4 : strcat(Command,"11");	break;
						case 5 : strcat(Command,"12");	break;
						case 6 : strcat(Command,"99");	break;
						default: *Result = FALSE; break;
					}						
				}			
				else if( ID2 == 2)	{sprintf( Command , "P%X2%2d" , Target_Address,Data+1); space_check(Command,'0');}
				else if( ID2 == 3)	{sprintf( Command , "P%X7%2d" , Target_Address,Data+1);	space_check(Command,'0');}
				else if( ID2 == 4)	{sprintf( Command , "P%XF%2d" , Target_Address,Data+1);	space_check(Command,'0');}
				else *Result = FALSE;
	
				break;
		case 200 :
				sprintf( Command ,"I%X",Target_Address);
				switch(Data){
					case 0 : strcat(Command,"00"); break;
					case 1 : strcat(Command,"3F"); break;
					case 2 : strcat(Command,"40"); break;
					default : *Result = FALSE; break;
				}
				break;
*/
		default : *Result = FALSE;		 
			      break;
	}
	
	if( !(*Result) ) Error_Message( DO , ID1 , ID2 , ID3 , ID4 ); 
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	char Command[128], Response[128], Buffer[128];
	int Data = 0, i;
	*Result = TRUE;

	for ( i= 0 ; i<128 ; i++ ) {
		 Response[i] = 0 ;
		 Buffer	[i]	 = 0 ;
		 Command[i]  = 0 ;
	}

	switch( ID1 ){	
		case 1  :	
		case 2  :	
		case 3  :	
		case 4  :	
		case 5  :	
		case 6  :	
		case 7  :	
		case 8  :	
		case 9  :	
		case 10 :	
		case 11 :	
		case 12 :	
		case 13 :	
		case 14 :	
		case 15 :	
		case 16 :	
		case 17 : // RF Code Reader
			     strcpy( data ,	BarData[ID1] ); break; // Page(1~17) Reader
				 *Result = TRUE;
				 break; 

		case 20 : // Version Read
				 sprintf( Command , "V%X" , Target_Address );
			  	 if ( Rs232_IO( Command , Response , FALSE , 0 ) ) {
					if ( strlen( Response ) <= 0 ) {
						*Result = FALSE;
						CommStatus = FALSE;
					}
					else {
						*Result = TRUE;
						CommStatus = TRUE;
						itostr(data,Response + 5,16);
						//strncpy( data , Response + 4  , 16 );
					}
				 }
				 else {
					 *Result = FALSE;
					 CommStatus = FALSE;
				 }
                 break;

		case 98 :	
					strncpy( data , DRV_VERSION , 99 );
					*Result    = TRUE;
					break;

		case 99 :	
					strncpy( data , DRV_BUILDDATE , 99 );
					*Result    = TRUE;
					break;

		default: *Result = FALSE;		
			     break;
	}

	if( !(*Result) ) { Error_Message( SI , ID1 , 0 , 0 , 0 );  return; }

}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {
	*Result = TRUE;

	switch( ID1 ) {
		case  1 : // 
			     strcpy( BarDataSet ,	data ); 
				 break;

		default :
				 *Result = FALSE;
				 break;
	}

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
