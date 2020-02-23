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
enum { UV_IDLE , UV_BUSY , UV_COMPLETE , UV_FAIL } ; // ULVAC_RUN_STS
enum { AXIS_LOAD , AXIS_CNT } ;
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
int		RB_RUN_STS = 0 ;

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

//... Wafer Status for Simulator
enum { eArmWfr_ABSENT , eArmWfr_PRESENT };
int		g_nArmA_Wafer = eArmWfr_ABSENT;
int		g_nArmB_Wafer = eArmWfr_ABSENT;
int		g_nArmC_Wafer = eArmWfr_ABSENT;
int		g_nArmD_Wafer = eArmWfr_ABSENT;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
BOOL	bDataLog = FALSE ;
int		Robot_Pos = 0;
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

	return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL Rs232_IO( char *Command, BOOL bRead,  int IO_MODE ) {

	printf("[ULVAC] Send Cmds[%s]\n", Command);

	if(IO_ACTION == IO_MODE)		Sleep(5000);


	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	double dRes = 0 , R_Data = 0 ;
	int count = 0 ;
	
	*Result = TRUE;

	switch ( ID1 ) {
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
					*Result = Rs232_IO( "RQ WORKSPACE T403", TRUE , IO_STATUS ) ;
					break ;
		case 23 :	//2008.06.10 AWS Correctable Limit Low
					*Result = Rs232_IO( "RQ WORKSPACE T412", TRUE , IO_STATUS ) ;
					break ;
		default  :	*Result = FALSE ;
					break ;
	}

	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] AI Error : [%d-%d-%d]\n" , ID1 , ID2 , ID3 ) ;

	return dRes ;
}

//////////////////////////////////////////////////////////////////////////
void Analog_Out( int ID1 , int ID2 , int ID3 , double Data , int *Result ) {
	char SendBuf[DEFAULT_LENGTH];
	double dRes = 0 ;

	switch( ID1 ) {
		case 22 :	//2008.06.10 AWS Correctable Limit High
					sprintf( SendBuf , "SET WORKSPACE T403 %d" , (int) (Data*1000) ) ;
					*Result = Rs232_IO( SendBuf, TRUE , IO_REQUEST ) ;
					break ;
		case 23 :	//2008.06.10 AWS Correctable Limit Low
					sprintf( SendBuf , "SET WORKSPACE T412 %d" , (int) (Data*1000) ) ;
					*Result = Rs232_IO( SendBuf, TRUE , IO_REQUEST ) ;
					break ;
		default :	
					*Result = FALSE ;
					break ;
	}

	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] AO Error : [%d-%d-%d]\n" , ID1 , ID2 , ID3 ) ;
}

//////////////////////////////////////////////////////////////////////////
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	int  ReadData = 0 ;
	
	*Result = TRUE ;

	switch( ID1 ) {
		case 1 :	
					ReadData = 1 ;
					break ;
		case 2 :
					*Result = Rs232_IO( "HLLO", TRUE , IO_STATUS ) ;
					ReadData = 1 ;
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
					*Result = Rs232_IO( "RQ POS STN", TRUE , IO_STATUS ) ;
					break ;
		case 4 :
					ReadData = ErrorCode ;
					break ;
		case 5 :
					ReadData = NullRsp_Chk ;
					break ;
		case 6 :	// Robot Action Status : 0 Idle - 1 Busy - 2 Complete
					// ReadData = RB_RUN_STS ; 
					ReadData = 2 ;	//2:Complete
					break ;
		case 11 : 
					*Result = Rs232_IO( "RQ IO STATE ALL", TRUE , IO_STATUS ) ;
					break ;
		case 21 :	//Wafer Load Status
					*Result = Rs232_IO( "RQ WFDTCT", TRUE , IO_STATUS );

					if     (ID2 == 1)		ReadData = g_nArmA_Wafer;
					else if(ID2 == 2)		ReadData = g_nArmB_Wafer;
					else if(ID2 == 3)		ReadData = g_nArmC_Wafer;
					else if(ID2 == 4)		ReadData = g_nArmD_Wafer;

					break;
		case 91 :	// Current Slot
					ReadData = CASSETTE_SLOT ;
					break ;
		case 92 :	// Current Arm
					ReadData = Arm ;
					break ;
		case 233 :	//2008.06.10 AWS Use
					*Result = Rs232_IO( "RQ WORKSPACE T400" , TRUE , IO_STATUS ) ;
					break ;
		default :	
					*Result = FALSE ;
					break ;
	}

	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] DI Error : [%d-%d-%d-%d]\n" , ID1 , ID2 , ID3 , ID4 ) ;

	return ReadData ;
}

//////////////////////////////////////////////////////////////////////////
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int Data , int *Result ) {
	char SendBuf[DEFAULT_LENGTH] = "";

	ErrorCode = 0 ; // Digital_Out시 Errorcode 초기화.

	switch( ID1 ) {
		case 1	 :	// Servo On/Off
					if		( Data == 0 ) sprintf( SendBuf , "SRVO OFF" ) ;
					else if ( Data == 1 ) sprintf( SendBuf , "SRVO ON" ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_REQUEST ) ;
					break ;
		case 2	 :	// Stop
					*Result = Rs232_IO( "HALT" , TRUE , IO_REQUEST ) ;
					break ;
		//[ 2012/01/19 RS232C 통신 Log ON/OFF ( BY LJH )
		case 3   :
					if ( Data == 0 ) { // ON_OFF
						bDataLog = TRUE;
					}else{
						bDataLog = FALSE;
					}
					*Result = TRUE;
					break;
		//2014/02/10 Setpoint Intlk걸림시 동작 Success 처리 수정
		case 6   :
					RB_RUN_STS = Data;
					*Result = TRUE;
					break;

		case 11	 :	// Home ==> Simply Home Action
					*Result = Rs232_IO( "HOME" , TRUE , IO_ACTION ) ;
					break ;
		case 12	 :	// Home2 ==> Wafer Check on each End Effect with the fixed station #20 after Home Action
					*Result = Rs232_IO( "HOME2" , TRUE , IO_ACTION ) ;  
					break ; 
		case 13	 :	// Home3 ==> Wafer Check on each End Effect with the user selected station after Home Action
					// EX) "HOME3 5" ==> HOME2 Action with Station #5
					sprintf( SendBuf , "HOME3 %d" , Data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 21  :	// Get Teaching Position of U(A&B) Arm
					sprintf( SendBuf , "RQ STN %d U" , Data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_STATUS ) ;
					break ;
		case 22  :	// Get Teaching Position of L(C&D) Arm
					sprintf( SendBuf , "RQ STN %d L" , Data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_STATUS ) ;
					break ;
		case 51  :	// Speed Set , 1 = Low, 2 = Med, 3 = High  
					if		( Data == 1 ) sprintf( SendBuf , "SET LOSPD" ) ;
					else if ( Data == 2 ) sprintf( SendBuf , "SET MESPD" ) ;
					else if ( Data == 3 ) sprintf( SendBuf , "SET HISPD" ) ;
					*Result = Rs232_IO( SendBuf, TRUE , IO_SETTING ) ;
					break ;
		case 111 :	// Extend EndEffector which specified just before action, //2008.06.10 AWS Function Insert
					*Result = Rs232_IO( "WCGOTO R EX" ,TRUE , IO_ACTION ) ;
					break ;
		case 112 :	// Retract EndEffector which specified just before action
					*Result = Rs232_IO( "GOTO R RE" , TRUE , IO_ACTION ) ;
					break ;
		case 113 :	// Z axis to the Up position
					*Result = Rs232_IO( "GOTO Z UP" , TRUE , IO_ACTION ) ;
					break ;
		case 114 :	// Z axis to the Mid position
					*Result = Rs232_IO( "GOTO Z MD" , TRUE , IO_ACTION ) ;
					break ;
		case 115 :	// Z axis to the Down position
					*Result = Rs232_IO( "GOTO Z DN" , TRUE , IO_ACTION ) ;
					break ;
		case 116 :	// Rotation with specified End Effector
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO N %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO N %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO N %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO N %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO N %d U" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO N %d L" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 117 :	// Rotate and Up Position
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO NZ %d A UP" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO NZ %d B UP" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO NZ %d C UP" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO NZ %d D UP" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO NZ %d U UP" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO NZ %d L UP" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 118 :	// Rotate and Middle Position
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO NZ %d A MD" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO NZ %d B MD" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO NZ %d C MD" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO NZ %d D MD" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO NZ %d U MD" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO NZ %d L MD" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 119 :	// Rotate and Down Position
					if		( ID2 == 1 ) sprintf( SendBuf , "GOTO NZ %d A DN" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "GOTO NZ %d B DN" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "GOTO NZ %d C DN" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "GOTO NZ %d D DN" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "GOTO NZ %d U DN" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "GOTO NZ %d L DN" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 121 :	// PICK from Station
					if		( ID2 == 1 ) sprintf( SendBuf , "PICK %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "PICK %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "PICK %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "PICK %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "PICK %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "PICK %d L" , Data ) ; //L : C&D
					
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					
					break ;
		case 122 :	// PLACE to Station, //2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "PLACE_C %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "PLACE_C %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "PLACE_C %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "PLACE_C %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "PLACE_C %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "PLACE_C %d L" , Data ) ; //L : C&D 	
					
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					
					break ;
		case 211 :	// (Hi-Speed) Extend EndEffector which specified just before action
					*Result = Rs232_IO( "HIGOTO R EX" , TRUE , IO_ACTION ) ;
					break ;
		case 212 :	// (Hi-Speed) Retract EndEffector which specified just before action
					*Result = Rs232_IO( "HIGOTO R RE" , TRUE , IO_ACTION ) ;
					break ;
		case 213 :	// (Hi-Speed) Z axis to the Up position
					*Result = Rs232_IO( "HIGOTO Z UP" , TRUE , IO_ACTION ) ;
					break ;
		case 214 :	// (Hi-Speed) Z axis to the Mid position
					*Result = Rs232_IO( "HIGOTO Z MD" , TRUE , IO_ACTION ) ;
					break ;
		case 215 :	// (Hi-Speed) Z axis to the Down position
					*Result = Rs232_IO( "HIGOTO Z DN" , TRUE , IO_ACTION ) ;
					break ;
		case 216 :	// (Hi-Speed) Rotation to Station
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO N %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO N %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO N %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO N %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO N %d U" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO N %d L" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 217 :	// (Hi-Speed) Rotate and Up Position
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO NZ %d A UP" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO NZ %d B UP" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO NZ %d C UP" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO NZ %d D UP" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO NZ %d U UP" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO NZ %d L UP" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 218 :	// (Hi-Speed) Rotate and Middle Position
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO NZ %d A MD" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO NZ %d B MD" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO NZ %d C MD" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO NZ %d D MD" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO NZ %d U MD" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO NZ %d L MD" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 219 :	// (Hi-Speed) Rotate and Down Position
					if		( ID2 == 1 ) sprintf( SendBuf , "HIGOTO NZ %d A DN" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIGOTO NZ %d B DN" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIGOTO NZ %d C DN" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIGOTO NZ %d D DN" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIGOTO NZ %d U DN" , Data ) ; //U : A&B
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIGOTO NZ %d L DN" , Data ) ; //L : C&D
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 221 :	// (Hi-Speed) Get from Station ==> Hi-Speed when Extend Only
					if		( ID2 == 1 ) sprintf( SendBuf , "HIPICK %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "HIPICK %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "HIPICK %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "HIPICK %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "HIPICK %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "HIPICK %d L" , Data ) ; //L : C&D 	
					
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					
					break ;
		case 222 :	// (Hi-Speed) Put to Station ==> Hi-Speed when Retract Only  //2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "PLACE_D %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "PLACE_D %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "PLACE_D %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "PLACE_D %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "PLACE_D %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "PLACE_D %d L" , Data ) ; //L : C&D 	
					
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					
					break ;
		case 231 :	//2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "AWC INIT %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "AWC INIT %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "AWC INIT %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "AWC INIT %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "AWC INIT %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "AWC INIT %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 232 :	//2008.06.10 AWS Function Insert
					if		( ID2 == 1 ) sprintf( SendBuf , "AWC INIT2 %d A" , Data ) ;
					else if	( ID2 == 2 ) sprintf( SendBuf , "AWC INIT2 %d B" , Data ) ; 	
					else if	( ID2 == 3 ) sprintf( SendBuf , "AWC INIT2 %d C" , Data ) ; 	
					else if	( ID2 == 4 ) sprintf( SendBuf , "AWC INIT2 %d D" , Data ) ; 	
					else if	( ID2 == 5 ) sprintf( SendBuf , "AWC INIT2 %d U" , Data ) ; //U : A&B 	
					else if	( ID2 == 6 ) sprintf( SendBuf , "AWC INIT2 %d L" , Data ) ; //L : C&D 	
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 233 :	//2008.06.10 AWS Use
					sprintf( SendBuf , "SET WORKSPACE T400 %d" , Data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_REQUEST ) ;
					break ;
		case 234  :	//2008.06.10 AWS Read
					if		( ID2 == 1 ) *Result =  Rs232_IO( "RQ WFOST A" , TRUE , IO_REQUEST ) ;
					else if	( ID2 == 2 ) *Result =  Rs232_IO( "RQ WFOST B" , TRUE , IO_REQUEST ) ;
					else if	( ID2 == 3 ) *Result =  Rs232_IO( "RQ WFOST C" , TRUE , IO_REQUEST ) ;
					else if	( ID2 == 4 ) *Result =  Rs232_IO( "RQ WFOST D" , TRUE , IO_REQUEST ) ;
					break ;
		case 235  :	//2008.06.10 AWS Result
					if		( ID2 == 1 ) *Result =  Rs232_IO( "RQ ASTS A 2" , TRUE , IO_REQUEST ) ;
					else if	( ID2 == 2 ) *Result =  Rs232_IO( "RQ ASTS B 2" , TRUE , IO_REQUEST ) ;
					else if	( ID2 == 3 ) *Result =  Rs232_IO( "RQ ASTS C 2" , TRUE , IO_REQUEST ) ;	
					else if	( ID2 == 4 ) *Result =  Rs232_IO( "RQ ASTS D 2" , TRUE , IO_REQUEST ) ;
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

	//////////////////////////////////////////////////////////////////////////
	//... Update Arm Wafer Status for Simulator
	if(ID1 == 121 || ID1 == 221)
	{	//... Case. Pick
		if	   (ID2 == 1)	g_nArmA_Wafer = eArmWfr_PRESENT;
		else if(ID2 == 2)	g_nArmB_Wafer = eArmWfr_PRESENT;
		else if(ID2 == 3)	g_nArmC_Wafer = eArmWfr_PRESENT;
		else if(ID2 == 4)	g_nArmD_Wafer = eArmWfr_PRESENT;
		else if(ID2 == 5)	{ g_nArmA_Wafer = eArmWfr_PRESENT; g_nArmB_Wafer = eArmWfr_PRESENT; }
		else if(ID2 == 6)	{ g_nArmC_Wafer = eArmWfr_PRESENT; g_nArmD_Wafer = eArmWfr_PRESENT; }
	}
	else if(ID1 == 122 || ID1 == 222)
	{	//... Case. Place
		if	   (ID2 == 1)	g_nArmA_Wafer = eArmWfr_ABSENT;
		else if(ID2 == 2)	g_nArmB_Wafer = eArmWfr_ABSENT;
		else if(ID2 == 3)	g_nArmC_Wafer = eArmWfr_ABSENT;
		else if(ID2 == 4)	g_nArmD_Wafer = eArmWfr_ABSENT;
		else if(ID2 == 5)	{ g_nArmA_Wafer = eArmWfr_ABSENT; g_nArmB_Wafer = eArmWfr_ABSENT; }
		else if(ID2 == 6)	{ g_nArmC_Wafer = eArmWfr_ABSENT; g_nArmD_Wafer = eArmWfr_ABSENT; }
	}


	if ( !(*Result) ) printf( "=====> [ULVACROBOT-DRIVER] DO Error : [%d-%d-%d-%d]\n" , ID1 , ID2 , ID3 , ID4 ) ;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	char RecvBuf[DEFAULT_LENGTH] ;

	*Result = FALSE;

	switch ( ID1 ) {
		case 1   :
					*Result = TRUE ;
					sprintf( data , "%s" , RQString );
					break;
		// 축별 부하율 Reading ( 2013.02.05 , LJH )
		// 예)
		// GETVCDLOG
		// GVL,5,15,7,14,15,6,51  
		case 2   :
					*Result = Rs232_IO( "GETVCDLOG" , TRUE , IO_STATUS ) ;
					sprintf( data , "%s" , RecvBuf );
					break;
		// 축별 COUNTER Reading ( 2013.10.23 , LJH )
		// 예) RQ GOTOCOUNTER
		// Nxxxx , Rxxxx , Zxxxx , TMxx
		case 3   :
					*Result = Rs232_IO( "RQ GOTOCOUNTER" , TRUE , IO_STATUS ) ;
					sprintf( data , "%s" , RecvBuf );
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
					*Result = Rs232_IO( data , FALSE , IO_SETTING );
					break ;
		case 2   :  
					sprintf( RQString , "" ) ;
					*Result = Rs232_IO( data , TRUE , IO_SETTING );
					sprintf( RQString , "%s" , RecvBuf ) ;
					break ;
		case 51 :	// Set Absolute Position of Specified Station
					// EX) "SET STN 4 A 500.115 210.001 7.993 2.004 1.005"
					// EX) "SET STN [Station] [EndEffector] [r1-Val] [t1-Val] [z-Up-Val] [z-Md-Val] [z-Dn-Val]"
					sprintf( SendBuf , "SET STN %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_SETTING ) ;
					break ;
		case 53 :	// Set Low Speed
					// EX) "SET SPEED 1 110.0 82.0 130.0 330.0 12.000 160"
					// EX) "SET SPEED [LowSpeed] [Ex-Speed] [Ex-Accel] [Rot-Speed] [Rot-Accel] [z-Speed] [z-Accel]"
					sprintf( SendBuf , "SET SPEED 1 %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_SETTING ) ;
					break ;
		case 54 :	// Set Middle Speed
					// EX) "SET SPEED 2 110.0 82.0 130.0 330.0 12.000 160"
					// EX) "SET SPEED [MidSpeed] [Ex-Speed] [Ex-Accel] [Rot-Speed] [Rot-Accel] [z-Speed] [z-Accel]"
					sprintf( SendBuf , "SET SPEED 2 %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_SETTING ) ;
					break ;
		case 55 :	// Set High Speed
					// EX) "SET SPEED 3 110.0 82.0 130.0 330.0 12.000 160"
					// EX) "SET SPEED [HighSpeed] [Ex-Speed] [Ex-Accel] [Rot-Speed] [Rot-Accel] [z-Speed] [z-Accel]"
					sprintf( SendBuf , "SET SPEED 3 %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_SETTING ) ;
					break ;
		case 61 :	// Store Absolute Position of Specified Station
					// EX) "STORE STN 4 A"
					// EX) "STORE STN [Station] [EndEffector]"
					sprintf( SendBuf , "STORE STN %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_SETTING ) ;
					break ;
		case 63 :	// Store Speed
					// EX) "STORE SPEED 1"
					// EX) "STORE STN [Speed-No]"
					sprintf( SendBuf , "STORE SPEED %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_SETTING ) ;
					break ;
		case 101 :  // Transfer from Station to Station
					// EX) "XFER 3 1 A" ==> Transfer Wafer from Station #3 to Station #1 with "A" End Effector
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "XFER_C %s A" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 102 : 
					sprintf( SendBuf , "XFER_C %s B" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 103 :  
					sprintf( SendBuf , "XFER_C %s C" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 104 : 
					sprintf( SendBuf , "XFER_C %s D" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 105 :  //U : A&B
					sprintf( SendBuf , "XFER_C %s U" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 106 :  //L : C&D	
					sprintf( SendBuf , "XFER_C %s L" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 111 :  // Pick Wafer with One End Effector and then Place Wafer with the other End Effector
					// EX) "EXCHANGE 3 A C" ==> Pick Wafer with "A" End Effector and Place Wafer with "C" End Effector at Station #3
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "EXCHANGE_C %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 201 :  // Transfer from Station to Station ==> HI-Speed with no-Wafer
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "XFER_D %s A" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 202 : 
					sprintf( SendBuf , "XFER_D %s B" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 203 : 
					sprintf( SendBuf , "XFER_D %s C" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 204 :  
					sprintf( SendBuf , "XFER_D %s D" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 205 :  //U : A&B
					sprintf( SendBuf , "XFER_D %s U" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 206 :  //L : C&D	
					sprintf( SendBuf , "XFER_D %s L" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
					break ;
		case 211 :  // Pick Wafer with One End Effector and then Place Wafer with the other End Effector ==> HI-Speed with no-Wafer
					// EX) "HIEXCHANGE 3 A C" ==> Pick Wafer with "A" End Effector and Place Wafer with "C" End Effector at Station #3
					//2008.06.10 AWS Function Insert
					sprintf( SendBuf , "EXCHANGE_D %s" , data ) ;
					*Result = Rs232_IO( SendBuf , TRUE , IO_ACTION ) ;
		
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
