#include "cimrs232.h"
#include "driver.h"
//-----------------------------------------------------------------------------------
#include "kutletc.h"
#include "kutlanal.h"
#include "kutlstr.h"


#include "cimseqnc.h"
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
#define		CR				13
#define		LF				10	
#define		DEFAULT_LENGTH	256
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
/*-----------------------------------------------------------------------------------
#  REVISION HISTORY
#------------+------------+------------+---------------------------------------------
#    DATE    |   WRITER   | VERSION    |  ETC
#------------+------------+------------+---------------------------------------------
# 2005.12.07 | skpark     | v1.2       |
#------------+------------+------------+---------------------------------------------
# 2007.12.01 | Jangjy     | v2.0       | Ulvac Covot6 Initial Version
#            |            |            | 7-axis Robot, 4 independent end effets
#------------+------------+------------+---------------------------------------------
# 2007.12.31 | KSM	      | v2.1       | Insert HIPICK/HIPLACE etc.
#            |            |            | 
#------------+------------+------------+-------------------------------------------*/
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
#define STATUS_UPDATE_INTERVAL	( 50 * 10000 ) //300msec
//-----------------------------------------------------------------------------------
enum { IO_STATUS , IO_REQUEST , IO_ACTION , IO_SETTING };
//-----------------------------------------------------------------------------------
char	SendTermStr[6] ;
char	RecvTermStr[6] ;

int		TimeOut			= 100 ;
int		Arm				= 1 ;		// 1 : A Arm , 2 : B Arm
int		CASSETTE_SLOT	= 1 ;
int		MAX_SLOT_NUMBER = 26 ;
int		MAX_ARM_NUMBER	= 2 ;

BOOL	CommStatus		= FALSE ;

char	RQString[256] ;

int		ErrorCode = 0 ;
int		NullRsp_Chk = 0; 

int		RealTimeout = 100 ;
char*	LogDrive;//Add ybkim
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
double	R_VALUE_A_UP = 0;
double	T_VALUE_A_UP = 0;
double	R_VALUE_B_UP = 0;
double	T_VALUE_B_UP = 0;
double	Z_UP_VALUE_UP = 0;
double	Z_MD_VALUE_UP = 0;
double	Z_DN_VALUE_UP = 0;

double	R_VALUE_C_LO = 0;
double	T_VALUE_C_LO = 0;
double	R_VALUE_D_LO = 0;
double	T_VALUE_D_LO = 0;
double	Z_UP_VALUE_LO = 0;
double	Z_MD_VALUE_LO = 0;
double	Z_DN_VALUE_LO = 0;

double	X_VALUE_READ_A = 0;
double	Y_VALUE_READ_A = 0;
double	X_VALUE_READ_B = 0;
double	Y_VALUE_READ_B = 0;
double	X_VALUE_READ_C = 0;
double	Y_VALUE_READ_C = 0;
double	X_VALUE_READ_D = 0;
double	Y_VALUE_READ_D = 0;

double	X_VALUE_RESULT_A = 0;
double	Y_VALUE_RESULT_A = 0;
double	X_VALUE_RESULT_B = 0;
double	Y_VALUE_RESULT_B = 0;
double	X_VALUE_RESULT_C = 0;
double	Y_VALUE_RESULT_C = 0;
double	X_VALUE_RESULT_D = 0;
double	Y_VALUE_RESULT_D = 0;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
BOOL	bDataLog = FALSE ;
int		Robot_Pos = 0;
INT64	ULVAC_LastPollingTime = 0;
//-----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
INT64 SystemTimeToAbsTime( SYSTEMTIME st ) {
	INT64 i64;
	FILETIME fst;

	SystemTimeToFileTime( &st , &fst );
	i64 = ( ( (INT64)fst.dwHighDateTime) << 32 ) + fst.dwLowDateTime ;
	return i64;
}
//-----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void TimeLog( char* str ) {
	FILE *fp = NULL;
	char Path[MAX_PATH], DirectoryName[MAX_PATH] , FileName[64];
	SYSTEMTIME st;

	if( !bDataLog ) return;

	GetLocalTime( &st );
	sprintf( FileName , "%04d%02d%02d%02d.log" , st.wYear , st.wMonth , st.wDay , st.wHour );
	if ( strlen(LogDrive) >= 2 ) {//Log드라이브 선택에 따라 다른 경로로 분기되도록 수정(2010.01.25, ybkim)
		sprintf( Path , "DATALOG\\VAC%d\\%s" , Robot_Pos, FileName );
	}
	else if ( strlen(LogDrive) == 1 ) {
		sprintf( Path , "%s:\\DATALOG\\VAC%d\\%s" , LogDrive, Robot_Pos, FileName );
	}
	
	fp = fopen( Path , "a+" );
// Start : 2008.08.17 LOG Folder 미존재 시 자동 Folder 생성 및 예외 처리. ( BY LJH )
	if( !fp ) { 
		if ( strlen(LogDrive) >= 2 ) {
			sprintf( DirectoryName , "DATALOG/VAC1" );//ybkim
		}
		else if ( strlen(LogDrive) == 1 ) {
			sprintf( DirectoryName , "%s:/DATALOG/VAC1", LogDrive );//ybkim
		}
		//CreateDirectory ( "F:/DATALOG/VAC1" , NULL );
		CreateDirectory ( DirectoryName , NULL );//ybkim
		fp = fopen( Path , "a+" );
	}
	if( !fp ) { 
		printf( "[ULVAC_VAC%d-DRIVER] : Can't Create Log File\n" , Robot_Pos ); 
		return; 
	}else {
		fprintf( fp , "%04d/%02d/%02d %02d:%02d:%02d:%03d\t" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond , st.wMilliseconds );
		fprintf( fp , "[%s]\n" , str );
		
		fclose( fp );
	}
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL NeedUpdate ( int Mode ) {
 	SYSTEMTIME st;

	if		( Mode == IO_ACTION ) { 
 		return TRUE; 
	}
	else if ( Mode == IO_STATUS ) {
		GetLocalTime( &st );
		if ( ( SystemTimeToAbsTime( st ) - ULVAC_LastPollingTime ) > STATUS_UPDATE_INTERVAL ) {
			//printf ( "=================== 2-1\n" ) ;
			return TRUE;
		}
		else {
			//printf ( "=================== 2-2\n" ) ;
			return FALSE;
		}
	}
	else return TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void STR_SEPERATE( char *SourceString , char *ResultString1 , char *ResultString2 , int ReadMaxCount ) {
	int Mode = 0 , count = 0;

	while( *SourceString ) {
		if      ( Mode == 0 ) {
			if ( *SourceString == ' ' ) {
				*ResultString1 = 0;
				Mode  = 1;
				count = 0;
			}
			else {
				if ( count < ReadMaxCount ) {
					*ResultString1 = *SourceString;
					*ResultString1++;
					count++;
				}
			}
		}
		else if ( Mode == 1 ) {
			if ( count != 0 || *SourceString != ' ' ) {
				*ResultString2 = *SourceString;
				*ResultString2++;
				count++;
				if ( count >= ReadMaxCount ) break;
			}
		}
		*SourceString++;
	}
	if      ( Mode == 0 ) {
		*ResultString1 = 0;
		*ResultString2 = 0;
	}
	else if ( Mode == 1 ) {
		*ResultString2 = 0;
	}
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	SendTermStr[0] = CR ;	
	SendTermStr[1] = LF ;	
	SendTermStr[2] = 0 ;

	RecvTermStr[0] = CR ;	
	RecvTermStr[1] = LF ;	
	RecvTermStr[2] = 0 ;

 	if ( ID10 < 100 ) RealTimeout = 100 ;
	else			  RealTimeout = ID10 ;

	if ( ID9 == 1 ) Robot_Pos = 1 , bDataLog = TRUE; // Robot 1 // 변수는 당연히 선언되어 있어야 하고,
	else			Robot_Pos = 2 , bDataLog = TRUE; // Robot 2

	printf("\n---------------------------------------------------------------\n");
   	printf("          ULVAC COVOT6 DRIVER VERSION : 2.0 ( 2007/12/01 ) \n");
   	printf("#---------------------------------------------------------------\n");
	if ( File != NULL ) {
		LogDrive = File;//I/O Driver선언시 ID1(Port)앞에 선언된 문자열을 전역변수로 넣어줌...20100113, ybkim
	}
	return( RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8) ) ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	RS232_Disconnect_Port() ;
	return TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Rs232_IO_Send( char *Cmnds ) {
	char SendBuf[DEFAULT_LENGTH*2] ;
	sprintf( SendBuf , "%s%c%c" , Cmnds , CR , LF ) ;

//	printf ( "\tUlvac Send MSG : [%s]\n" , Cmnds ) ;

	if	( !strcmp( Cmnds , "HOME" ) || !strcmp( Cmnds , "HOME2" ) || !strcmp( Cmnds , "HOME3" ) ) TimeOut = 90000;	
	else  TimeOut = RealTimeout;

	if ( !RS232_Write_String( SendBuf , strlen( SendBuf ) ) ) {
		printf( "=====> [ULVACROBOT-DRIVER] RS232 Send Error : %s\n" , SendBuf ) ;
		return FALSE ;
	}
	return TRUE ;
}
//-----------------------------------------------------------------------------------
BOOL Rs232_IO_Recv( char *Response ) {
	int ReadCnt ;
	char RecvBuf[DEFAULT_LENGTH] ;

	if ( !RS232_Read_String( Response , RecvTermStr , DEFAULT_LENGTH*2-1 , TimeOut , &ReadCnt ) ) {
		printf ( "\tUlvac Recv MSG : [%s]\n" , Response ) ;
		printf( "=====> [ULVACROBOT-DRIVER] RS232 Terminal String Recv. Error\n" ) ;
		if ( strlen( Response ) > 0 ) NullRsp_Chk = 0;
		else NullRsp_Chk = 1;				// Response 없는 상태.
		return FALSE ;
	}
//	printf ( "\tUlvac Recv MSG : [%s]\n" , Response ) ;
	if ( strncmp ( Response , "ERR" , 3 ) == 0 ) {
		strcpy( RecvBuf , Response + 3 ) ;
		ErrorCode = atoi( RecvBuf ) ;
		printf ( "===>> ULVAC Robot Control Error : %d \n " , ErrorCode ) ;
		return FALSE ;
	}
//	else	ErrorCode = 0 ;  // Input IO Reading시 초기화 Bug 수정(2012/11/07,LJH)
	if ( strlen( Response ) > 0 ) NullRsp_Chk = 0;
	return TRUE ;
}
//-----------------------------------------------------------------------------------
BOOL Rs232_IO( char *Command , char *Response , BOOL bRead,  int IO_MODE ) {
	BOOL Result ;
	SYSTEMTIME st;

	if ( !NeedUpdate( IO_MODE ) ) {
		printf( "Update Skip ==> IO_MODE [%d]\n" , IO_MODE  ) ;
	//	return TRUE; //del 2006/09/21
	}

	TimeLog( Command );

	sprintf( Response , "" ) ; 
	RS232_Clear_Buffer() ;
	Result = Rs232_IO_Send( Command ) ;

	if ( Result && bRead ) Result = Rs232_IO_Recv( Response ) ;

	if ( !Result ) CommStatus = FALSE;
	else {
		CommStatus = TRUE;
		GetLocalTime( &st );
		ULVAC_LastPollingTime = SystemTimeToAbsTime( st );
		if ( IO_MODE == IO_STATUS );	 
		else if( IO_MODE == IO_ACTION ) NeedUpdate( IO_ACTION );
	}
	TimeLog( Response );
	//

	return Result ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	char RecvBuf[DEFAULT_LENGTH] , *token ; //, Str1[DEFAULT_LENGTH] ;
//	char Str1[DEFAULT_LENGTH] ; //, No ;//, Str2[DEFAULT_LENGTH]  ;
	double dRes = 0 , R_Data = 0 ;
	int count = 0 ;
	*Result = FALSE ;
	switch ( ID1 ) {
		//---> AI #11&12 will be obtained after DO #21 or #22 Command(RQ STN - -) Send
		case 11 : //U(A&B) Arm Teaching Value
					switch ( ID2 ) {
						case 1 : dRes = R_VALUE_A_UP ;  break ;
						case 2 : dRes = T_VALUE_A_UP ;  break ;
						case 3 : dRes = R_VALUE_B_UP ;  break ;
						case 4 : dRes = T_VALUE_B_UP ;  break ;
						case 5 : dRes = Z_UP_VALUE_UP ;	break ;
						case 6 : dRes = Z_MD_VALUE_UP ; break ;
						case 7 : dRes = Z_DN_VALUE_UP ; break ;
					}
					*Result = TRUE ;
					break ;
		case 12 : //L(C&D) Arm Teaching Value
					switch ( ID2 ) {
						case 1 : dRes = R_VALUE_C_LO ;  break ;
						case 2 : dRes = T_VALUE_C_LO ;  break ;
						case 3 : dRes = R_VALUE_D_LO ;  break ;
						case 4 : dRes = T_VALUE_D_LO ;  break ;
						case 5 : dRes = Z_UP_VALUE_LO ;	break ;
						case 6 : dRes = Z_MD_VALUE_LO ; break ;
						case 7 : dRes = Z_DN_VALUE_LO ; break ;
					}
					*Result = TRUE ;
					break ;

		case 14 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_READ_A ;  break ;
						case 2 : dRes = Y_VALUE_READ_A ;  break ;
					}
					*Result = TRUE ;
					break ;
		case 15 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_READ_B ;  break ;
						case 2 : dRes = Y_VALUE_READ_B ;  break ;
					}
					*Result = TRUE ;
					break ;		
		case 16 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_READ_C ;  break ;
						case 2 : dRes = Y_VALUE_READ_C ;  break ;
					}
					*Result = TRUE ;
					break ;
					
		case 17 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_READ_D ;  break ;
						case 2 : dRes = Y_VALUE_READ_D ;  break ;
					}
					*Result = TRUE ;
					break ;		
		case 18 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_RESULT_A ;  break ;
						case 2 : dRes = Y_VALUE_RESULT_A ;  break ;
					}
					*Result = TRUE ;
					break ;
		case 19 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_RESULT_B ;  break ;
						case 2 : dRes = Y_VALUE_RESULT_B ;  break ;
					}
					*Result = TRUE ;
					break ;		
		case 20 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_RESULT_C ;  break ;
						case 2 : dRes = Y_VALUE_RESULT_C ;  break ;
					}
					*Result = TRUE ;
					break ;
					
		case 21 : //2008.06.10 AWS Read A
					switch ( ID2 ) {
						case 1 : dRes = X_VALUE_RESULT_D ;  break ;
						case 2 : dRes = Y_VALUE_RESULT_D ;  break ;
					}
					*Result = TRUE ;
					break ;	
		case 22 :	//2008.06.10 AWS Correctable Limit High
					*Result = Rs232_IO( "RQ WORKSPACE T403" , RecvBuf , TRUE , IO_STATUS ) ;

					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) { //ReadData = 1 ;
						}
						else {   
						//	printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;
						
						//	STR_SEPERATE_CHAR ( RecvBuf , ',' , Str1 , Str2 , 255) ;
						//	Ext_Str_From_Str ( RecvBuf , 6 , 4 , Str1) ;
						//	Get_Data_From_String_For_CHAR ( RecvBuf , 6 , Str1 , 0 , 255 , &No) ;

							token = strtok( RecvBuf , ",") ;
							while( token != NULL ) {
							//	printf("token  =  %s\n", token);
								if ( count > 0 ) {
									R_Data = atof ( token ) ;
								//	printf("R_Data  =  %f\n", R_Data);
									
								}
								else count++;

								token = strtok( NULL , "") ;
							}
						//	R_Data = atoi ( token ) ;

							dRes = R_Data / 1000 ;

							/*

						//	if ( strcmp( RecvBuf , "T403,0" )  == 0 )	dRes = 0;
							if ( strcmp( RecvBuf , "T403,1000" ) == 0 ) dRes = 1;
							if ( strcmp( RecvBuf , "T403,2000" ) == 0 ) dRes = 2;
							if ( strcmp( RecvBuf , "T403,3000" ) == 0 ) dRes = 3;
							if ( strcmp( RecvBuf , "T403,4000" ) == 0 ) dRes = 4;
							if ( strcmp( RecvBuf , "T403,5000" ) == 0 ) dRes = 5; */
						}
					}
					break ;
		case 23 :	//2008.06.10 AWS Correctable Limit Low
					*Result = Rs232_IO( "RQ WORKSPACE T412" , RecvBuf , TRUE , IO_STATUS ) ;

					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) { //ReadData = 1 ;
						}
						else {   
						//	printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;

							token = strtok( RecvBuf , ",") ;
							while( token != NULL ) {
							//	printf("token  =  %s\n", token);
								if ( count > 0 ) {
									R_Data = atof ( token ) ;
								//	printf("R_Data  =  %f\n", R_Data);

								}
								else count++;

								token = strtok( NULL , "") ;
							}
						//	R_Data = atoi ( token ) ;

							dRes = R_Data / 1000 ;

							/*
							if ( strcmp( RecvBuf , "T412,0" ) == 0 )	 dRes = 0;
							if ( strcmp( RecvBuf , "T412,1000" ) == 0 ) dRes = 1;
							if ( strcmp( RecvBuf , "T412,2000" ) == 0 ) dRes = 2;
							if ( strcmp( RecvBuf , "T412,3000" ) == 0 ) dRes = 3;
							if ( strcmp( RecvBuf , "T412,4000" ) == 0 ) dRes = 4;
							if ( strcmp( RecvBuf , "T412,5000" ) == 0 ) dRes = 5;
							*/
						}
					}
					break ;
		default  :	*Result = FALSE ;
					break ;
	}
	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] AI Error : [%d-%d-%d]\n" , ID1 , ID2 , ID3 ) ;
	return dRes ;
}
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) {
	char SendBuf[DEFAULT_LENGTH] , RecvBuf[DEFAULT_LENGTH] ;
	double dRes = 0 ;


	switch( ID1 ) {
		case 22 :	//2008.06.10 AWS Correctable Limit High
					
					sprintf( SendBuf , "SET WORKSPACE T403 %d" , (int) (Data*1000) ) ;
			//		printf ( "\n SendBuf[%s] \n" , SendBuf ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					if ( *Result ) {
			//			printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;						
					}
			//		sprintf( SendBuf , "STORE WORKSPACE T403" ) ;
			//		*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
			//		if ( *Result ) {
			//			printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;						
			//		}
					break ;
		case 23 :	//2008.06.10 AWS Correctable Limit Low
					sprintf( SendBuf , "SET WORKSPACE T412 %d" , (int) (Data*1000) ) ;
			//		printf ( "\n SendBuf[%s] \n" , SendBuf ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					if ( *Result ) {
			//			printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;						
					}
			//		sprintf( SendBuf , "STORE WORKSPACE T412" ) ;
			//		*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
			//		if ( *Result ) {
			//			printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;						
			//		}
					break ;
		default :	
					*Result = FALSE ;
					break ;
	}
	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] AO Error : [%d-%d-%d]\n" , ID1 , ID2 , ID3 ) ;
//	return dRes ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	char SendBuf[DEFAULT_LENGTH] , RecvBuf[DEFAULT_LENGTH] ;
	char Str1[256] , Str2[256] , Str3[256] ;
	char RetractA[256] , RetractB[256] , RetractC[256] , RetractD[256] ; 
	char WaferA[256] , WaferB[256] , WaferC[256] , WaferD[256] ;
	char Interlock[256] , Status[256] , ZStatus[256] , Alarm[256] , Error[256] ; 
	int  i , ReadData = 0 ;

	for ( i = 0 ; i < DEFAULT_LENGTH ; i++ ) { SendBuf[i] = 0 ; RecvBuf[i] = 0 ; }
	for ( i = 0 ; i < 256 ; i++ ) { 
		Str1[i] = 0 ; Str2[i] = 0 ; Str3[i] = 0 ; 
		Interlock[i] = 0 ; Status[i] = 0 ; ZStatus[i] = 0 ; Alarm[i] = 0 ; Error[i] = 0 ; 
		RetractA[i] = 0 ; RetractB[i] = 0 ; RetractC[i] = 0 ; RetractD[i] = 0 ; 
		WaferA[i] = 0 ; WaferB[i] = 0 ; WaferC[i] = 0 ; WaferD[i] = 0 ; 
	}
	
	switch( ID1 ) {
		case 1 :	
					*Result = TRUE ;
					if ( CommStatus ) ReadData = 1 ;
					else ReadData = 0 ;
					break ;
		case 2 :
					*Result = Rs232_IO( "HLLO" , RecvBuf , TRUE , IO_STATUS ) ;

					if ( *Result ) {
						if ( !strncmp( RecvBuf , "RDY" , 3 ) ) ReadData = 1 ;
						else {   
					//		printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;
							ReadData = 0 ;
						}
					}
					break ;
		//#-------------------------------------------------------------------------------------
		//#- Arm Status
		//#  XXXXX
		//#  TTTTT
		//#  ||||+--- Station          (0:Home Position , 1-99)
		//#  |||+---- Up/Down location (0:not_at_a_station , 1:Up , 2:Middle , 3:Down)
		//#  ||+----- EX/RE Status     (1:Extend , 2:Retract) ==> Left Arm
		//#  |+------ EX/RE Status     (1:Extend , 2:Retract) ==> Right Arm
		//#  +------- Rotbot Arm       (1:U(A&B) Arm , 2:L(C&D) Arm)
		//#-------------------------------------------------------------------------------------
		case 3 :
					*Result = Rs232_IO( "RQ POS STN" , RecvBuf , TRUE , IO_STATUS ) ;
					if ( *Result ) {
						printf ( "DI #3 [RQ POS STN] ===>> Result Str : %s \n" , RecvBuf ) ;
						if		( !strncmp( RecvBuf + 8 , "U" , 1 ) )	ReadData = 1 * 100000 ;
						else if ( !strncmp( RecvBuf + 8 , "L" , 1 ) )	ReadData = 2 * 100000 ;
						
						if		( !strncmp( RecvBuf + 10 , "EX" , 2 ) )	ReadData = ReadData + 1 * 10000 ;
						else if	( !strncmp( RecvBuf + 10 , "RE" , 2 ) )	ReadData = ReadData + 2 * 10000 ;

						if		( !strncmp( RecvBuf + 13 , "EX" , 2 ) )	ReadData = ReadData + 1 * 1000 ;
						else if	( !strncmp( RecvBuf + 13 , "RE" , 2 ) )	ReadData = ReadData + 2 * 1000 ;

						if		( !strncmp( RecvBuf + 16 , "UP" , 2 ) )	ReadData = ReadData + 1 * 100 ;
						else if ( !strncmp( RecvBuf + 16 , "MD" , 2 ) )	ReadData = ReadData + 2 * 100 ;
						else if	( !strncmp( RecvBuf + 16 , "DN" , 2 ) )	ReadData = ReadData + 3 * 100 ;
						else if	( !strncmp( RecvBuf + 16 , "--" , 2 ) )	ReadData = ReadData + 0 * 100 ;

						strcpy( SendBuf , RecvBuf + 19 ) ;
						ReadData = ReadData + atoi( SendBuf ) ;
					}
					break ;
		case 4 :
					*Result = TRUE ;
					ReadData = ErrorCode ;
					break ;
		case 5 :
					*Result = TRUE ;
					ReadData = NullRsp_Chk ;
					break ;
		case 11 : 
					*Result = Rs232_IO( "RQ IO STATE ALL" , RecvBuf , TRUE , IO_STATUS ) ;
					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) {

						}
						else {
							printf ( "DI #11 [RQ IO STATE ALL] ===>> Result Str : %s \n" , RecvBuf ) ;
							STR_SEPERATE( RecvBuf , Str1 , Str2 , 255 ) ;	//Str1 : IO
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : STATE
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Interlock Status
							strcpy( Interlock , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Retract Status A Arm
							strcpy( RetractA , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Retract Status B Arm
							strcpy( RetractB , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Retract Status C Arm
							strcpy( RetractC , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Retract Status D Arm
							strcpy( RetractD , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Status
							strcpy( Status , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Z Position Status
							strcpy( ZStatus , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Alarm Status
							strcpy( Alarm , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Error Code
							strcpy( Error , Str1 ) ;
							/* ---> Wafer Status will be Used DI #21
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Wafer Status A Arm
							strcpy( WaferA , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Wafer Status B Arm
							strcpy( WaferB , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Wafer Status C Arm
							strcpy( WaferC , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Wafer Status D Arm
							strcpy( WaferD , Str1 ) ;
							*/
							 
							switch( ID2 ) {
								case 1 :	//Interlock Status
									ReadData = atoi( Interlock ) ;	
									break ;
								case 2 :	//Retract Status A Arm
									ReadData = atoi( RetractA ) ;	
									break ;
								case 3 :	//Retract Status B Arm
									ReadData = atoi( RetractB ) ;	
									break ;
								case 4 :	//Retract Status C Arm
									ReadData = atoi( RetractC ) ;	
									break ;
								case 5 :	//Retract Status D Arm
									ReadData = atoi( RetractD ) ;	
									break ;
								case 6 :	//Status
									ReadData = atoi( Status ) ;	
									break ;
								case 7 :	//Z-Status
									ReadData = atoi( ZStatus ) ;	
									break ;
								case 8 :	//Alarm Status
									ReadData = atoi( Alarm ) ;	
									break ;
								case 9 :	//Error Code
									ReadData = atoi( Error ) ;	
									break ;
								//---> ADD 2007/12/31 SlitValve Close Interlock Delay 개선    by KSM
								case 10 :	//Error Code
									if ( atoi( RetractA ) >= 0 && atoi( RetractA ) <= 2 &&
										 atoi( RetractB ) >= 0 && atoi( RetractB ) <= 2 &&
										 atoi( RetractC ) >= 0 && atoi( RetractC ) <= 2 &&
										 atoi( RetractD ) >= 0 && atoi( RetractD ) <= 2  )
									{

										ReadData =  atoi( RetractA ) * 1000 +
												    atoi( RetractB ) *  100 + 
												    atoi( RetractC ) *   10 +
													atoi( RetractD ) ;
									}
									else
									{
										ReadData = -100;
									}
									break ;
								//---> End 2007/12/31 SlitValve Close Interlock Delay 개선    ( 1 of )

								/* ---> Wafer Status will be Used DI #21
								case 10 :	//Wafer Status A Arm
									ReadData = atoi( WaferA ) ;	
									break ;
								case 11 :	//Wafer Status B Arm
									ReadData = atoi( WaferB ) ;	
									break ;
								case 12 :	//Wafer Status C Arm
									ReadData = atoi( WaferC ) ;	
									break ;
								case 13 :	//Wafer Status D Arm
									ReadData = atoi( WaferD ) ;	
									break ;
								*/
							}
						}
					}
					break ;
		case 21 :	//Wafer Load Status
					*Result = Rs232_IO( "RQ WFDTCT" , RecvBuf , TRUE , IO_STATUS ) ;
					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) {

						}
						else {
							printf ( "DI #21 [RQ WFDTCT] ===>> Result Str : %s \n" , RecvBuf ) ;
							STR_SEPERATE( RecvBuf , Str1 , Str2 , 255 ) ;	//Str1 : WF
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Wafer Status A Arm
							strcpy( WaferA , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Wafer Status B Arm
							strcpy( WaferB , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Wafer Status C Arm
							strcpy( WaferC , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Wafer Status D Arm
							strcpy( WaferD , Str1 ) ;

							switch ( ID2 ) {
								case 1 :	//Wafer Status A Arm
									ReadData = atoi( WaferA ) ;
									//---> ADD 2010/10/04 WaferStatus Logging 보완 by CNL
									printf ( "Wafer Status A Arm [%d]\n" , ReadData ) ;
									//---> END 2010/10/04 WaferStatus Logging 보완 by CNL
									break ;
								case 2 :	//Wafer Status B Arm
									ReadData = atoi( WaferB ) ;
									//---> ADD 2010/10/04 WaferStatus Logging 보완 by CNL
									printf ( "Wafer Status B Arm [%d]\n" , ReadData ) ;
									//---> END 2010/10/04 WaferStatus Logging 보완 by CNL
									break ;
								case 3 :	//Wafer Status C Arm
									ReadData = atoi( WaferC ) ;	
									//---> ADD 2010/10/04 WaferStatus Logging 보완 by CNL
									printf ( "Wafer Status C Arm [%d]\n" , ReadData ) ;
									//---> END 2010/10/04 WaferStatus Logging 보완 by CNL
									break ;
								case 4 :	//Wafer Status D Arm
									ReadData = atoi( WaferD ) ;	
									//---> ADD 2010/10/04 WaferStatus Logging 보완 by CNL
									printf ( "Wafer Status D Arm [%d]\n" , ReadData ) ;
									//---> END 2010/10/04 WaferStatus Logging 보완 by CNL
									break ;
							}
						}
					}
					break;
		case 91 :	// Current Slot
					*Result = TRUE ;
					ReadData = CASSETTE_SLOT ;
					break ;
		case 92 :	// Current Arm
					*Result = TRUE ;
					ReadData = Arm ;
					break ;
		case 233 :	//2008.06.10 AWS Use
					*Result = Rs232_IO( "RQ WORKSPACE T400" , RecvBuf , TRUE , IO_STATUS ) ;

					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) { //ReadData = 1 ;
						}
						else {   
					//		printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;
							if ( strcmp( RecvBuf , "T400,0" ) == 0 ) ReadData = 0;
							if ( strcmp( RecvBuf , "T400,1" ) == 0 ) ReadData = 1;							
						}
					}
					break ;
		

		default :	
					*Result = FALSE ;
					break ;
	}

	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] DI Error : [%d-%d-%d-%d]\n" , ID1 , ID2 , ID3 , ID4 ) ;

	return ReadData ;
}
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) {
	char SendBuf[DEFAULT_LENGTH] , RecvBuf[DEFAULT_LENGTH] ;
	char Str1[256] , Str2[256] , Str3[256] ;
	char RvalueA[256] , RvalueB[256] , TvalueA[256] , TvalueB[256] ;
	char Xvalue[256] , Yvalue[256] , XvalueR[256] , YvalueR[256] ; 
	char ZvalueUp[256] , ZvalueMd[256] , ZvalueDn[256] ; 
	int i ;

	ErrorCode = 0 ; // Digital_Out시 Errorcode 초기화.

	for ( i = 0 ; i < DEFAULT_LENGTH ; i++ ) { SendBuf[i] = 0 ; RecvBuf[i] = 0 ; }

	switch( ID1 ) {
		case 1	 :	// Servo On/Off
					if		( Data == 0 ) sprintf( SendBuf , "SRVO OFF" ) ;
					else if ( Data == 1 ) sprintf( SendBuf , "SRVO ON" ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 2	 :	// Stop
					*Result = Rs232_IO( "HALT" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		//[ 2012/01/19 RS232C 통신 Log ON/OFF ( BY LJH )
		case 3   :
					if ( Data == 0 ) { // ON_OFF
						bDataLog = TRUE;
					}else{
						bDataLog = FALSE;
					}
					break;
		//]
		case 11	 :	// Home ==> Simply Home Action
					*Result = Rs232_IO( "HOME" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 12	 :	// Home2 ==> Wafer Check on each End Effect with the fixed station #20 after Home Action
					*Result = Rs232_IO( "HOME2" , RecvBuf , TRUE , IO_ACTION ) ;  
					break ; 
		case 13	 :	// Home3 ==> Wafer Check on each End Effect with the user selected station after Home Action
					// EX) "HOME3 5" ==> HOME2 Action with Station #5
					sprintf( SendBuf , "HOME3 %d" , Data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 21  :	// Get Teaching Position of U(A&B) Arm
					sprintf( SendBuf , "RQ STN %d U" , Data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_STATUS ) ;
					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) {

						}
						else {
							STR_SEPERATE( RecvBuf , Str1 , Str2 , 255 ) ;	//Str1 : STN
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : STATION
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : END EFFECTOR
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Right Arm(B-Arm) R-Value
							strcpy( RvalueB , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Right Arm(B-Arm) T-Value
							strcpy( TvalueB , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Left Arm(A-Arm) R-Value
							strcpy( RvalueA , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Left Arm(A-Arm) T-Value
							strcpy( TvalueA , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Z-Up Value
							strcpy( ZvalueUp , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Z-Middle Value
							strcpy( ZvalueMd , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Z-Down Value
							strcpy( ZvalueDn , Str1 ) ;

							R_VALUE_A_UP  = atof( RvalueA ) ;
							T_VALUE_A_UP  = atof( TvalueA ) ;
							R_VALUE_B_UP  = atof( RvalueB ) ;
							T_VALUE_B_UP  = atof( TvalueB ) ;
							Z_UP_VALUE_UP = atof( ZvalueUp ) ;
							Z_MD_VALUE_UP = atof( ZvalueMd ) ;
							Z_DN_VALUE_UP = atof( ZvalueDn ) ;
						}
					}
					break ;
		case 22  :	// Get Teaching Position of L(C&D) Arm
					sprintf( SendBuf , "RQ STN %d L" , Data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_STATUS ) ;
					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) {

						}
						else {
							STR_SEPERATE( RecvBuf , Str1 , Str2 , 255 ) ;	//Str1 : STN
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : STATION
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : END EFFECTOR
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Right Arm(B-Arm) R-Value
							strcpy( RvalueB , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Right Arm(B-Arm) T-Value
							strcpy( TvalueB , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Left Arm(A-Arm) R-Value
							strcpy( RvalueA , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Left Arm(A-Arm) T-Value
							strcpy( TvalueA , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Z-LO Value
							strcpy( ZvalueUp , Str1 ) ;
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : Z-Middle Value
							strcpy( ZvalueMd , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Z-Down Value
							strcpy( ZvalueDn , Str1 ) ;

							R_VALUE_C_LO  = atof( RvalueA ) ;
							T_VALUE_C_LO  = atof( TvalueA ) ;
							R_VALUE_D_LO  = atof( RvalueB ) ;
							T_VALUE_D_LO  = atof( TvalueB ) ;
							Z_UP_VALUE_LO = atof( ZvalueUp ) ;
							Z_MD_VALUE_LO = atof( ZvalueMd ) ;
							Z_DN_VALUE_LO = atof( ZvalueDn ) ;
						}
					}
					break ;
		case 51  :	// Speed Set , 1 = Low, 2 = Med, 3 = High  
					if		( Data == 1 ) sprintf( SendBuf , "SET LOSPD" ) ;
					else if ( Data == 2 ) sprintf( SendBuf , "SET MESPD" ) ;
					else if ( Data == 3 ) sprintf( SendBuf , "SET HISPD" ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_SETTING ) ;
					break ;
		case 111 :	// Extend EndEffector which specified just before action, //2008.06.10 AWS Function Insert
					*Result = Rs232_IO( "WCGOTO R EX" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 112 :	// Retract EndEffector which specified just before action
					*Result = Rs232_IO( "GOTO R RE" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 113 :	// Z axis to the Up position
					*Result = Rs232_IO( "GOTO Z UP" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 114 :	// Z axis to the Mid position
					*Result = Rs232_IO( "GOTO Z MD" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 115 :	// Z axis to the Down position
					*Result = Rs232_IO( "GOTO Z DN" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 116 :	// Rotation with specified End Effector
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO N %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO N %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO N %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO N %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO N %d U" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO N %d L" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 117 :	// Rotate and Up Position
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO NZ %d A UP" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO NZ %d B UP" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO NZ %d C UP" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO NZ %d D UP" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO NZ %d U UP" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO NZ %d L UP" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 118 :	// Rotate and Middle Position
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO NZ %d A MD" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO NZ %d B MD" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO NZ %d C MD" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO NZ %d D MD" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO NZ %d U MD" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO NZ %d L MD" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 119 :	// Rotate and Down Position
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO NZ %d A DN" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO NZ %d B DN" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO NZ %d C DN" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO NZ %d D DN" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO NZ %d U DN" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO NZ %d L DN" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 121 :	// PICK from Station
					if		( ID2 == 1 ) sprintf( SendBuf , "PICK %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "PICK %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "PICK %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "PICK %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "PICK %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "PICK %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 122 :	// PLACE to Station, //2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "PLACE_C %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "PLACE_C %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "PLACE_C %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "PLACE_C %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "PLACE_C %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "PLACE_C %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 211 :	// (Hi-Speed) Extend EndEffector which specified just before action
					*Result = Rs232_IO( "HIGOTO R EX" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 212 :	// (Hi-Speed) Retract EndEffector which specified just before action
					*Result = Rs232_IO( "HIGOTO R RE" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 213 :	// (Hi-Speed) Z axis to the Up position
					*Result = Rs232_IO( "HIGOTO Z UP" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 214 :	// (Hi-Speed) Z axis to the Mid position
					*Result = Rs232_IO( "HIGOTO Z MD" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 215 :	// (Hi-Speed) Z axis to the Down position
					*Result = Rs232_IO( "HIGOTO Z DN" , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 216 :	// (Hi-Speed) Rotation to Station
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO N %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO N %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO N %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO N %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO N %d U" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO N %d L" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 217 :	// (Hi-Speed) Rotate and Up Position
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO NZ %d A UP" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO NZ %d B UP" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO NZ %d C UP" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO NZ %d D UP" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO NZ %d U UP" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO NZ %d L UP" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 218 :	// (Hi-Speed) Rotate and Middle Position
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO NZ %d A MD" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO NZ %d B MD" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO NZ %d C MD" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO NZ %d D MD" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO NZ %d U MD" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO NZ %d L MD" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 219 :	// (Hi-Speed) Rotate and Down Position
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO NZ %d A DN" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO NZ %d B DN" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO NZ %d C DN" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO NZ %d D DN" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO NZ %d U DN" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO NZ %d L DN" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 221 :	// (Hi-Speed) Get from Station ==> Hi-Speed when Extend Only
					if		( ID2 == 1 ) sprintf( SendBuf , "HIPICK %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIPICK %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIPICK %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIPICK %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIPICK %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIPICK %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 222 :	// (Hi-Speed) Put to Station ==> Hi-Speed when Retract Only  //2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "PLACE_D %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "PLACE_D %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "PLACE_D %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "PLACE_D %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "PLACE_D %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "PLACE_D %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 231 :	//2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "AWC INIT %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "AWC INIT %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "AWC INIT %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "AWC INIT %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "AWC INIT %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "AWC INIT %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 232 :	//2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "AWC INIT2 %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "AWC INIT2 %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "AWC INIT2 %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "AWC INIT2 %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "AWC INIT2 %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "AWC INIT2 %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 233 :	//2008.06.10 AWS Use
					sprintf( SendBuf , "SET WORKSPACE T400 %d" , Data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
		//[ 2012/07/23 AWS User EPROM Wrtie 안도록 처리 
				//	sprintf( SendBuf , "STORE WORKSPACE T400" , Data ) ;
				//	*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 234  :	//2008.06.10 AWS Read
					if		( ID2 == 1 ) *Result =  Rs232_IO( "RQ WFOST A" , RecvBuf , TRUE , IO_ACTION ) ;
					else if	( ID2 == 2 ) *Result =  Rs232_IO( "RQ WFOST B" , RecvBuf , TRUE , IO_ACTION ) ;
					else if	( ID2 == 3 ) *Result =  Rs232_IO( "RQ WFOST C" , RecvBuf , TRUE , IO_ACTION ) ;
					else if	( ID2 == 4 ) *Result =  Rs232_IO( "RQ WFOST D" , RecvBuf , TRUE , IO_ACTION ) ;
					
				//	*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_STATUS ) ;
					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) {

						}
						else {
						//	printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;
							STR_SEPERATE( RecvBuf , Str1 , Str2 , 255 ) ;	//Str1 : 'WFOST'
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : EndEffector
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : X-Value
							strcpy( Xvalue , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Y-Value
							strcpy( Yvalue , Str1 ) ;							

							if		( ID2 == 1 ) {
								X_VALUE_READ_A  = atof( Xvalue ) ;
								Y_VALUE_READ_A  = atof( Yvalue ) ;
							}
							else if	( ID2 == 2 ) {
								X_VALUE_READ_B  = atof( Xvalue ) ;
								Y_VALUE_READ_B  = atof( Yvalue ) ;
							}
							else if	( ID2 == 3 ) {
								X_VALUE_READ_C  = atof( Xvalue ) ;
								Y_VALUE_READ_C  = atof( Yvalue ) ;
							}
							else if	( ID2 == 4 ) {
								X_VALUE_READ_D  = atof( Xvalue ) ;
								Y_VALUE_READ_D  = atof( Yvalue ) ;
							}							
						}
					}
					break ;
		case 235  :	//2008.06.10 AWS Result
					if		( ID2 == 1 ) *Result =  Rs232_IO( "RQ ASTS A 2" , RecvBuf , TRUE , IO_ACTION ) ;
					else if	( ID2 == 2 ) *Result =  Rs232_IO( "RQ ASTS B 2" , RecvBuf , TRUE , IO_ACTION ) ;
					else if	( ID2 == 3 ) *Result =  Rs232_IO( "RQ ASTS C 2" , RecvBuf , TRUE , IO_ACTION ) ;	
					else if	( ID2 == 4 ) *Result =  Rs232_IO( "RQ ASTS D 2" , RecvBuf , TRUE , IO_ACTION ) ;
					
				//	*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_STATUS ) ;
					if ( *Result ) {
						if ( !strncmp( RecvBuf , "ERR" , 3 ) ) {

						}
						else {
						//	printf ( "\n RecvBuf[%s] \n" , RecvBuf ) ;
							STR_SEPERATE( RecvBuf , Str1 , Str2 , 255 ) ;	//Str1 : 'ASTS'
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : EndEffector
							STR_SEPERATE( Str3    , Str1 , Str2 , 255 ) ;	//Str1 : X-Value
							strcpy( XvalueR , Str1 ) ;
							STR_SEPERATE( Str2    , Str1 , Str3 , 255 ) ;	//Str1 : Y-Value
							strcpy( YvalueR , Str1 ) ;							

							if		( ID2 == 1 ) {
								X_VALUE_RESULT_A  = atof( XvalueR ) ;
								Y_VALUE_RESULT_A  = atof( YvalueR ) ;
							}
							else if	( ID2 == 2 ) {
								X_VALUE_RESULT_B  = atof( XvalueR ) ;
								Y_VALUE_RESULT_B  = atof( YvalueR ) ;
							}
							else if	( ID2 == 3 ) {
								X_VALUE_RESULT_C  = atof( XvalueR ) ;
								Y_VALUE_RESULT_C  = atof( YvalueR ) ;
							}
							else if	( ID2 == 4 ) {
								X_VALUE_RESULT_D  = atof( XvalueR ) ;
								Y_VALUE_RESULT_D  = atof( YvalueR ) ;
							}							
						}
					}
					break ;

		case 302 :	// Slot Number
					if ( Data < 1 && Data > MAX_SLOT_NUMBER ) *Result = FALSE ;
					else {
						*Result = TRUE ;
						CASSETTE_SLOT = Data ;
					}
					break ;
		case 303 :	// Arm Number
					if ( Data < 1 && Data > MAX_ARM_NUMBER ) *Result = FALSE ;
					else {
						*Result = TRUE ;
						Arm = Data ;
					}
					break ;
		default :	
					*Result = FALSE ;
					break ;
	}

	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] DO Error : [%d-%d-%d-%d]\n" , ID1 , ID2 , ID3 , ID4 ) ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
//	char RecvBuf[DEFAULT_LENGTH] ;

	*Result = FALSE;

	switch ( ID1 ) {
		case 1   :
					*Result = TRUE ;
					sprintf( data , "%s" , RQString );
					break;
		
		default  :	
					*Result = FALSE ;
					break ;
	}
	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] SI Error : [%d]\n" , ID1 ) ;
}
//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {
	char SendBuf[DEFAULT_LENGTH] , RecvBuf[DEFAULT_LENGTH] ; //, Str1[256] , Str2[256] ;
	*Result = FALSE ;
	switch ( ID1 ) {
		case 1	 :  
					*Result = Rs232_IO( data , RecvBuf , FALSE , IO_SETTING );
					break ;
		case 2   :  
					sprintf( RQString , "" ) ;
					*Result = Rs232_IO( data , RecvBuf , TRUE , IO_SETTING );
					sprintf( RQString , "%s" , RecvBuf ) ;
					break ;
		case 51 :	// Set Absolute Position of Specified Station
					// EX) "SET STN 4 A 500.115 210.001 7.993 2.004 1.005"
					// EX) "SET STN [Station] [EndEffector] [r1-Val] [t1-Val] [z-Up-Val] [z-Md-Val] [z-Dn-Val]"
					sprintf( SendBuf , "SET STN %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_SETTING ) ;
					break ;
		case 53 :	// Set Low Speed
					// EX) "SET SPEED 1 110.0 82.0 130.0 330.0 12.000 160"
					// EX) "SET SPEED [LowSpeed] [Ex-Speed] [Ex-Accel] [Rot-Speed] [Rot-Accel] [z-Speed] [z-Accel]"
					sprintf( SendBuf , "SET SPEED 1 %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_SETTING ) ;
					break ;
		case 54 :	// Set Middle Speed
					// EX) "SET SPEED 2 110.0 82.0 130.0 330.0 12.000 160"
					// EX) "SET SPEED [MidSpeed] [Ex-Speed] [Ex-Accel] [Rot-Speed] [Rot-Accel] [z-Speed] [z-Accel]"
					sprintf( SendBuf , "SET SPEED 2 %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_SETTING ) ;
					break ;
		case 55 :	// Set High Speed
					// EX) "SET SPEED 3 110.0 82.0 130.0 330.0 12.000 160"
					// EX) "SET SPEED [HighSpeed] [Ex-Speed] [Ex-Accel] [Rot-Speed] [Rot-Accel] [z-Speed] [z-Accel]"
					sprintf( SendBuf , "SET SPEED 3 %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_SETTING ) ;
					break ;
		case 61 :	// Store Absolute Position of Specified Station
					// EX) "STORE STN 4 A"
					// EX) "STORE STN [Station] [EndEffector]"
					sprintf( SendBuf , "STORE STN %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_SETTING ) ;
					break ;
		case 63 :	// Store Speed
					// EX) "STORE SPEED 1"
					// EX) "STORE STN [Speed-No]"
					sprintf( SendBuf , "STORE SPEED %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_SETTING ) ;
					break ;
		case 101 :  // Transfer from Station to Station
					// EX) "XFER 3 1 A" ==> Transfer Wafer from Station #3 to Station #1 with "A" End Effector
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "XFER_C %s A" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 102 : 
					sprintf( SendBuf , "XFER_C %s B" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 103 :  
					sprintf( SendBuf , "XFER_C %s C" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 104 : 
					sprintf( SendBuf , "XFER_C %s D" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 105 :  //U : A&B
					sprintf( SendBuf , "XFER_C %s U" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 106 :  //L : C&D	
					sprintf( SendBuf , "XFER_C %s L" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 111 :  // Pick Wafer with One End Effector and then Place Wafer with the other End Effector
					// EX) "EXCHANGE 3 A C" ==> Pick Wafer with "A" End Effector and Place Wafer with "C" End Effector at Station #3
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "EXCHANGE_C %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 201 :  // Transfer from Station to Station ==> HI-Speed with no-Wafer
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "XFER_D %s A" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 202 : 
					sprintf( SendBuf , "XFER_D %s B" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 203 : 
					sprintf( SendBuf , "XFER_D %s C" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 204 :  
					sprintf( SendBuf , "XFER_D %s D" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 205 :  //U : A&B
					sprintf( SendBuf , "XFER_D %s U" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 206 :  //L : C&D	
					sprintf( SendBuf , "XFER_D %s L" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
					break ;
		case 211 :  // Pick Wafer with One End Effector and then Place Wafer with the other End Effector ==> HI-Speed with no-Wafer
					// EX) "HIEXCHANGE 3 A C" ==> Pick Wafer with "A" End Effector and Place Wafer with "C" End Effector at Station #3
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "EXCHANGE_D %s" , data ) ;
					*Result = Rs232_IO( SendBuf , RecvBuf , TRUE , IO_ACTION ) ;
		
		default  :	
					*Result = FALSE ;
					break ;
	}
	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] SO Error : [%d]\n" , ID1 ) ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_Name_In( char *Name , int *Result ) {
	*Result = TRUE ;
	return 0 ;
}
//-----------------------------------------------------------------------------------
void Analog_Name_Out( char *Name , double data , int *Result ) {
	*Result = TRUE ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_Name_In( char *Name , int *Result ) {
	*Result = TRUE ;
	return 0 ;
}
//-----------------------------------------------------------------------------------
void Digital_Name_Out( char *Name , int data , int *Result ) {
	*Result = TRUE ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Name_In( char *Name , char *data , int *Result ) {
	*Result = TRUE ;
}
//-----------------------------------------------------------------------------------
void String_Name_Out( char *Name , char *data , int *Result ) {
	*Result = TRUE ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
