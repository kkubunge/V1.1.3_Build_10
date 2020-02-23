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
char  ADD[16];
//-----------------------------------------------------------------------------------
BOOL  CommStatus = FALSE;
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
char  BarData[20][100];
char  BarDataSet[100];
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {

	char LogFileName[128] ;
	
	RecvTermStr[0] = 0x0D; // CR
	RecvTermStr[1] = 0;

	// ----- RS232 Data Logging ( ASC : FALSE ) ------------>
	sprintf( LogFileName , "Hermos_Logging_%d", ID1 ) ;
	RS232_Logging_Style( FALSE , LogFileName );
	// ----------------------------------------------------->

	sprintf( ADD , "%01d" , ID9 );

	if ( ID10 < 100 ) TimeOut = 100;
	else              TimeOut = ID10;
	return( RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ) );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	RS232_Disconnect_Port();
	return TRUE;
}
//-----------------------------------------------------------------------------------
// Make CRC
//-----------------------------------------------------------------------------------
void Get_CRC ( int Mode , char *Command , char *CRC , int Data ){
	char Result[128], CRC1[128] , CRC2[128];
	int DataBuf, DataBuf2, i;
	BOOL Change1 = FALSE , Change2 = FALSE ;

	for ( i = 0 ; i<128  ; i++ ) Result[i] = 0x00 ;
	for ( i = 0 ; i<128  ; i++ ) CRC1[i]   = 0x00 ;
	for ( i = 0 ; i<128  ; i++ ) CRC2[i]   = 0x00 ;

	if      ( Mode == 1 ) {
		Result[0] = Command[0];
		for ( i = 1 ; i< strlen( Command ) ; i++ ) { 
			Result[0] ^= Command[i];
			printf( "Result XOR[%d] = %c\n" , i , Result[0] );
		}
			printf( "Result All XOR = char[%c] , int[%d]\n" , Result[0] , Result[0] );
//		Data = 256 + ( Result[0] );
		Data = Result[0];
	}
	else if ( Mode == 2 ) {
		Result[0] = Command[0];
		for ( i = 1 ; i< strlen(Command) ; i++ ) { 
			Result[0] += Command[i];
			printf( "Result ADD[%d] = %c\n" , i , Result[0] );
		}
			printf( "Result All ADD = char[%c] , int[%d]\n" , Result[0] , Result[0] );
//		Data = 256 + ( Result[0] );
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

	printf( "			[HERMOS] Mode[%d] : Command[%s] , Result[%c] , Data[%d]\n" , Mode , Command , Result[0] , Data);
	printf( "			[HERMOS]            DataBuf[%d] , DataBuf2[%d] , CRC[%s]\n" , DataBuf , DataBuf2 , CRC);
}
//-----------------------------------------------------------------------------------
//Command를 지정된 형식으로 만든다. SR + LEN + [ COMMAND ] + ER + CRC1 + CRC2
//-----------------------------------------------------------------------------------
void Get_Command( char *Command ) {
	char SR[128], ER[128];
	char Buffer[128], CRC[128], CRC2[128];
	int Data, i;
	
	for ( i = 0 ; i<128  ; i++ ) {
		SR[i]     = 0x00 ;
		ER[i]     = 0x00 ;
		Buffer[i] = 0x00 ;
		CRC[i]    = 0x00 ;
		CRC2[i]   = 0x00 ;
	}

	SR[0] = (char)0x53;  SR[1] = (char)0x00;

	Data = strlen( Command );
	Get_CRC( 0 , Command , CRC , Data );

	ER[0] = (char)0x0D;  ER[1] = (char)0x00;

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
	printf( "			[HERMOS] Get Command[%s]\n" , Command );
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
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
BOOL Rs232_IO( char *Command , char *Response ) {
	BOOL Result = TRUE;
	int ReadCnt, len, ReadCrcCnt;
	char CrcString[16] = {0,};
	
	//-------------------------------------------
	Get_Command( Command );	
	//-------------------------------------------

	RS232_Clear_Buffer();

	// ----- Start Data Logging Start ----------->
	RS232_Logging_Control ( TRUE ) ;
	//------------------------------------------->

	Result = Rs232_IO_Send( Command );

	if( Result ) {
		// Send Commad & Receive Response
		if ( RS232_Read_String( Response , RecvTermStr , 255 , TimeOut , &ReadCnt )) {	// Read to ER(0x0D = CR)
			if ( RS232_Read_Count_Char( CrcString , 4 , &ReadCrcCnt , TimeOut  ) )		// Read CRC1/CRC2
					Result = TRUE;		
			else {
				printf( "			[HERMOS] Receive2 Error => Send : [ %s ] , Receive : [ %s ] , CRC String : [%s]\n" , Command , Response , CrcString );
				Result = FALSE;
			}
		}
		else {
			printf( "			[HERMOS] Receive Error1 => Send : [ %s ] , Receive : [ %s ]\n" , Command , Response );
			Result = FALSE;
		}

		// Check Response String
		if	( !Result ) {
			strcat ( Response , CrcString ) ;
			printf( "			[HERMOS] Receive Error3 => Send : [ %s ] , Receive : [ %s ]\n" , Command , Response );
		}
	}

	if ( !Result ) CommStatus = FALSE;
	else           CommStatus = TRUE;

	// ----- Start Data Logging End ------------->
	RS232_Logging_Control ( FALSE ) ;
	//------------------------------------------->
	
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
//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	return TRUE;
}
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) {
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	char Command[128] , Response[128], Buffer[128];
	int Data = 0, i;
	*Result = TRUE;

	for ( i= 0 ; i<128 ; i++ ) {
		 Response[i] = 0 ;
		 Buffer	[i]	 = 0 ;
		 Command[i]  = 0 ;
	}

	switch( ID1 ){
		case 1 : // Communication Status
			     if ( CommStatus )	Data = 1;
				 else				Data = 0;
				 return Data;
				 break;

		case 2 : // Error Code Status	
			     sprintf(Command, "e"); 
				 break;  

	   default : *Result = FALSE;	   
			     break;
	}

	if( !(*Result) ) { 
		Error_Message( DI , ID1 , ID2 , ID3 , ID4 );  
		return Data; 
	}

	strcat(Command , ADD );
	*Result = Rs232_IO(Command, Response);

	if( !(*Result) ) { 
		Error_Message( DI , ID1 , ID2 , ID3 , ID4 );  
		return Data; 
	}

	//-----------------------------------------------------------
	//---------------- Response Analysis = Data  ----------------
	//-----------------------------------------------------------
	switch( ID1 ){
		case 2 : //Error_Code
				 if      ( Response[3] == '0' )	Data = 0;
				 else if ( Response[3] == '1' )	Data = 1;
				 else if ( Response[3] == '2' )	Data = 2;
				 else if ( Response[3] == '3' )	Data = 3;
				 else if ( Response[3] == '4' )	Data = 4;
				 else if ( Response[3] == '5' )	Data = 5;
				 else if ( Response[3] == '6' )	Data = 6;
				 else if ( Response[3] == '7' )	Data = 7;
				 else if ( Response[3] == '8' )	Data = 8;
				 else if ( Response[3] == '9' )	Data = 9;
				 else if ( Response[3] == 'A' )	Data = 10;
				 else if ( Response[3] == ':' )	Data = 11;
				 else if ( Response[3] == ';' )	Data = 12;
				 else if ( Response[3] == 'B' )	Data = 13;
				 break; 
	}

	return Data;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) {
	char Command[128] = {0,} , Response[128] = {0,}, Buffer[128] = {0,};
	int i;

	*Result = TRUE;

	for ( i = 0 ; i < 19 ; i++ ) strcpy( BarData[i] , ""  );

	switch( ID1 ){
		case  1 : // Start an Externally Triggered Read
				  sprintf( Command , "X%x%x" , ADD , Data );
				  printf("RFID Command : %s\n", Command);
			  	  if ( Rs232_IO( Command , Response ) ) {
					  if ( strlen( Response ) <= 0 ) {
						  *Result = FALSE;
						  CommStatus = FALSE;

					  }
					  else {
						  *Result = TRUE;
						  CommStatus = TRUE;
						  strncpy( BarData[Data] , Response + 4  , 16 );
					  }
				  }
				  else {
					  *Result = FALSE;
					  CommStatus = FALSE;
				  }
                  break;
		
		case  2 : // Write Tag
				  sprintf( Command , "W%x%x" , ADD , Data );
				  strcat ( Command , BarDataSet ) ;
			  	  if ( Rs232_IO( Command , Response ) ) {
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

		case 3 : // Start Software Reset
				 sprintf( Command , "N%x" , ADD );
			  	 if ( Rs232_IO( Command , Response ) ) {
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

		case 4 : // Lock One Page
				 sprintf( Command , "N%x%x" , ADD , Data );
			  	 if ( Rs232_IO( Command , Response ) ) {
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

		default : *Result = FALSE;		 
			      break;
	}
	if( !(*Result) ) Error_Message( DO , ID1 , ID2 , ID3 , ID4 ); 
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	char Command[128] = {0,}, Response[128] = {0,}, Buffer[128] = {0,};
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
				 sprintf( Command , "V%x" , ADD );
			  	 if ( Rs232_IO( Command , Response ) ) {
					if ( strlen( Response ) <= 0 ) {
						  *Result = FALSE;
						  CommStatus = FALSE;
					  }
					  else {
						  *Result = TRUE;
						  CommStatus = TRUE;
						  strncpy( data , Response + 4  , 16 );

					  }
				 }
				 else {
				  *Result = FALSE;
				  CommStatus = FALSE;
				 }
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
void String_Name_Out( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
