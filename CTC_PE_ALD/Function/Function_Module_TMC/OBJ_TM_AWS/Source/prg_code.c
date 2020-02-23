/* Revision History
	1. 2008/01/07 by KSM
		(1) Lotlog 에 하위 Function 기록되도록 수정

	2. 2008/04/01 TM Robot ERROR 처리 문제 보완
*/



#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <TextLogDll.h>


#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

enum { RUN };
enum { OFF		, ON		};
enum { PMA , PMB , PMC , BM1 , BM2_1 , BM2_2 };
//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
enum	{ None_0 , PMA_1 , PMB_2	, PMC_3 , BM1_4	, BM2_5 } ;
//]
//[added 2009/05/28 Slide Data SVID 등록 
//enum { A_PM1 , B_PM1 , A_PM3 , B_PM3 , A_PM5 , B_PM5 ,  A_BM1 , B_BM1 ,A_BM2 ,B_BM2, A_BM2_1 ,B_BM2_1,A_BM2_2, B_BM2_2 };
//]
enum { IdleSts_0 , Pause_1  , Alarm_Clr_2 , Sych_Match_3 , Clear_Wfr_4 , Reset_Wfr_5 , Continue_6 }; // Run_Resume_Scenario

#define		ARM_A		0
#define		ARM_B		1

#define		A_ARM_AWS_WARN		6601
#define		B_ARM_AWS_WARN		6602

//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
#define		ALARM_COMM_ERROR	6501
//]

BOOL	SIMULATION_MODE	= FALSE;

IO_Name_String_Map IO_STR_Map_Table[] = {

	{ "IO_Driver_Loding"			, _K_D_IO	,	IO_Driver_Loding		,	0	} ,
	//===============================================================================

	// 2008.06.16 AWS Log.
	{ "ULVAC_AWS_RESULT_A"			, _K_D_IO	,	ULVAC_AWS_RESULT_A		,	0	} ,
	{ "ULVAC_AWS_RESULT_B"			, _K_D_IO	,	ULVAC_AWS_RESULT_B		,	0	} ,
	{ "ULVAC_AWS_RESULT_C"			, _K_D_IO	,	ULVAC_AWS_RESULT_C		,	0	} ,
	{ "ULVAC_AWS_RESULT_D"			, _K_D_IO	,	ULVAC_AWS_RESULT_D		,	0	} ,

	{ "ULVAC_AWS_RESULT_A_X"		, _K_A_IO	,	ULVAC_AWS_RESULT_A_X	,	0	} ,
	{ "ULVAC_AWS_RESULT_A_Y"		, _K_A_IO	,	ULVAC_AWS_RESULT_A_Y	,	0	} ,
	{ "ULVAC_AWS_RESULT_B_X"		, _K_A_IO	,	ULVAC_AWS_RESULT_B_X	,	0	} ,
	{ "ULVAC_AWS_RESULT_B_Y"		, _K_A_IO	,	ULVAC_AWS_RESULT_B_Y	,	0	} ,
	{ "ULVAC_AWS_RESULT_C_X"		, _K_A_IO	,	ULVAC_AWS_RESULT_C_X	,	0	} ,
	{ "ULVAC_AWS_RESULT_C_Y"		, _K_A_IO	,	ULVAC_AWS_RESULT_C_Y	,	0	} ,
	{ "ULVAC_AWS_RESULT_D_X"		, _K_A_IO	,	ULVAC_AWS_RESULT_D_X	,	0	} ,
	{ "ULVAC_AWS_RESULT_D_Y"		, _K_A_IO	,	ULVAC_AWS_RESULT_D_Y	,	0	} ,

	{ "ULVAC_AWS_READ_A"			, _K_D_IO	,	ULVAC_AWS_READ_A		,	0	} ,
	{ "ULVAC_AWS_READ_B"			, _K_D_IO	,	ULVAC_AWS_READ_B		,	0	} ,
	{ "ULVAC_AWS_READ_C"			, _K_D_IO	,	ULVAC_AWS_READ_C		,	0	} ,
	{ "ULVAC_AWS_READ_D"			, _K_D_IO	,	ULVAC_AWS_READ_D		,	0	} ,

	{ "ULVAC_AWS_READ_A_X"			, _K_A_IO	,	ULVAC_AWS_READ_A_X		,	0	} ,
	{ "ULVAC_AWS_READ_A_Y"			, _K_A_IO	,	ULVAC_AWS_READ_A_Y		,	0	} ,
	{ "ULVAC_AWS_READ_B_X"			, _K_A_IO	,	ULVAC_AWS_READ_B_X		,	0	} ,
	{ "ULVAC_AWS_READ_B_Y"			, _K_A_IO	,	ULVAC_AWS_READ_B_Y		,	0	} ,
	{ "ULVAC_AWS_READ_C_X"			, _K_A_IO	,	ULVAC_AWS_READ_C_X		,	0	} ,
	{ "ULVAC_AWS_READ_C_Y"			, _K_A_IO	,	ULVAC_AWS_READ_C_Y		,	0	} ,
	{ "ULVAC_AWS_READ_D_X"			, _K_A_IO	,	ULVAC_AWS_READ_D_X		,	0	} ,
	{ "ULVAC_AWS_READ_D_Y"			, _K_A_IO	,	ULVAC_AWS_READ_D_Y		,	0	} ,

	{ "ULVAC_AWS_LIMIT_WARN_LO"		, _K_A_IO	,	ULVAC_AWS_LIMIT_WARN_LO	,	0	} ,
	{ "ULVAC_AWS_LIMIT_WARN_HI"		, _K_A_IO	,	ULVAC_AWS_LIMIT_WARN_HI	,	0	} ,

	//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
	{ "TM_Pick_U_Arm_Station"			, _K_D_IO	,	TM_Pick_U_Arm_Station			,	0	} ,
	{ "TM_Pick_L_Arm_Station"			, _K_D_IO	,	TM_Pick_L_Arm_Station			,	0	} ,
	//[ added 2009/05/28 TM Robot Slide data svid 등록
	{ "PMA_SLIDE_A1_X_AXIS"			, _K_A_IO	,	PMA_SLIDE_A1_X_AXIS		,	0	} ,
	{ "PMA_SLIDE_A1_Y_AXIS"			, _K_A_IO	,	PMA_SLIDE_A1_Y_AXIS		,	0	} ,
	{ "PMA_SLIDE_A2_X_AXIS"			, _K_A_IO	,	PMA_SLIDE_A2_X_AXIS		,	0	} ,
	{ "PMA_SLIDE_A2_Y_AXIS"			, _K_A_IO	,	PMA_SLIDE_A2_Y_AXIS		,	0	} ,
	                      						                   
	{ "PMA_SLIDE_B1_X_AXIS"			, _K_A_IO	,	PMA_SLIDE_B1_X_AXIS		,	0	} ,
	{ "PMA_SLIDE_B1_Y_AXIS"			, _K_A_IO	,	PMA_SLIDE_B1_Y_AXIS		,	0	} ,
	{ "PMA_SLIDE_B2_X_AXIS"			, _K_A_IO	,	PMA_SLIDE_B2_X_AXIS		,	0	} ,
	{ "PMA_SLIDE_B2_Y_AXIS"			, _K_A_IO	,	PMA_SLIDE_B2_Y_AXIS		,	0	} ,
	                      						                   
	{ "PMB_SLIDE_A1_X_AXIS"			, _K_A_IO	,	PMB_SLIDE_A1_X_AXIS		,	0	} ,
	{ "PMB_SLIDE_A1_Y_AXIS"			, _K_A_IO	,	PMB_SLIDE_A1_Y_AXIS		,	0	} ,
	{ "PMB_SLIDE_A2_X_AXIS"			, _K_A_IO	,	PMB_SLIDE_A2_X_AXIS		,	0	} ,
	{ "PMB_SLIDE_A2_Y_AXIS"			, _K_A_IO	,	PMB_SLIDE_A2_Y_AXIS		,	0	} ,
	                      						                   
	{ "PMB_SLIDE_B1_X_AXIS"			, _K_A_IO	,	PMB_SLIDE_B1_X_AXIS		,	0	} ,
	{ "PMB_SLIDE_B1_Y_AXIS"			, _K_A_IO	,	PMB_SLIDE_B1_Y_AXIS		,	0	} ,
	{ "PMB_SLIDE_B2_X_AXIS"			, _K_A_IO	,	PMB_SLIDE_B2_X_AXIS		,	0	} ,
	{ "PMB_SLIDE_B2_Y_AXIS"			, _K_A_IO	,	PMB_SLIDE_B2_Y_AXIS		,	0	} ,
	                      						                   
	{ "PMC_SLIDE_A1_X_AXIS"			, _K_A_IO	,	PMC_SLIDE_A1_X_AXIS		,	0	} ,
	{ "PMC_SLIDE_A1_Y_AXIS"			, _K_A_IO	,	PMC_SLIDE_A1_Y_AXIS		,	0	} ,
	{ "PMC_SLIDE_A2_X_AXIS"			, _K_A_IO	,	PMC_SLIDE_A2_X_AXIS		,	0	} ,
	{ "PMC_SLIDE_A2_Y_AXIS"			, _K_A_IO	,	PMC_SLIDE_A2_Y_AXIS		,	0	} ,
  	                      						                   
	{ "PMC_SLIDE_B1_X_AXIS"			, _K_A_IO	,	PMC_SLIDE_B1_X_AXIS		,	0	} ,
	{ "PMC_SLIDE_B1_Y_AXIS"			, _K_A_IO	,	PMC_SLIDE_B1_Y_AXIS		,	0	} ,
	{ "PMC_SLIDE_B2_X_AXIS"			, _K_A_IO	,	PMC_SLIDE_B2_X_AXIS		,	0	} ,
	{ "PMC_SLIDE_B2_Y_AXIS"			, _K_A_IO	,	PMC_SLIDE_B2_Y_AXIS		,	0	} ,
	                      						                   
	{ "BM1_SLIDE_A1_X_AXIS"			, _K_A_IO	,	BM1_SLIDE_A1_X_AXIS		,	0	} ,
	{ "BM1_SLIDE_A1_Y_AXIS"			, _K_A_IO	,	BM1_SLIDE_A1_Y_AXIS		,	0	} ,
	{ "BM1_SLIDE_A2_X_AXIS"			, _K_A_IO	,	BM1_SLIDE_A2_X_AXIS		,	0	} ,
	{ "BM1_SLIDE_A2_Y_AXIS"			, _K_A_IO	,	BM1_SLIDE_A2_Y_AXIS		,	0	} ,
	                      						                   
	{ "BM1_SLIDE_B1_X_AXIS"			, _K_A_IO	,	BM1_SLIDE_B1_X_AXIS		,	0	} ,
	{ "BM1_SLIDE_B1_Y_AXIS"			, _K_A_IO	,	BM1_SLIDE_B1_Y_AXIS		,	0	} ,
	{ "BM1_SLIDE_B2_X_AXIS"			, _K_A_IO	,	BM1_SLIDE_B2_X_AXIS		,	0	} ,
	{ "BM1_SLIDE_B2_Y_AXIS"			, _K_A_IO	,	BM1_SLIDE_B2_Y_AXIS		,	0	} ,
	                      						                   
	{ "BM2_SLIDE_A1_X_AXIS"			, _K_A_IO	,	BM2_SLIDE_A1_X_AXIS		,	0	} ,
	{ "BM2_SLIDE_A1_Y_AXIS"			, _K_A_IO	,	BM2_SLIDE_A1_Y_AXIS		,	0	} ,
	{ "BM2_SLIDE_A2_X_AXIS"			, _K_A_IO	,	BM2_SLIDE_A2_X_AXIS		,	0	} ,
	{ "BM2_SLIDE_A2_Y_AXIS"			, _K_A_IO	,	BM2_SLIDE_A2_Y_AXIS		,	0	} ,
	                      						                   
	{ "BM2_SLIDE_B1_X_AXIS"			, _K_A_IO	,	BM2_SLIDE_B1_X_AXIS		,	0	} ,
	{ "BM2_SLIDE_B1_Y_AXIS"			, _K_A_IO	,	BM2_SLIDE_B1_Y_AXIS		,	0	} ,
	{ "BM2_SLIDE_B2_X_AXIS"			, _K_A_IO	,	BM2_SLIDE_B2_X_AXIS		,	0	} ,
	{ "BM2_SLIDE_B2_Y_AXIS"			, _K_A_IO	,	BM2_SLIDE_B2_Y_AXIS		,	0	} ,

	//]
	//]
	//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
	{ "ULVAC_AWS_LIMIT_HI"			, _K_A_IO	,	ULVAC_AWS_LIMIT_HI		,	0	} ,
	{ "ULVAC_AWS_LIMIT_LOW"			, _K_A_IO	,	ULVAC_AWS_LIMIT_LOW		,	0	} ,
	{ "ULVAC_AWS_LIMIT_PM_HI"		, _K_A_IO	,	ULVAC_AWS_LIMIT_PM_HI	,	0	} ,
	{ "ULVAC_AWS_LIMIT_BM_HI"		, _K_A_IO	,	ULVAC_AWS_LIMIT_BM_HI	,	0	} ,
	{ "ULVAC_AWS_LIMIT_PM_LOW"		, _K_A_IO	,	ULVAC_AWS_LIMIT_PM_LOW	,	0	} ,
	{ "ULVAC_AWS_LIMIT_BM_LOW"		, _K_A_IO	,	ULVAC_AWS_LIMIT_BM_LOW	,	0	} ,
	//]

	{ "ULVAC_AWS_USE"				,	_K_D_IO		,	ULVAC_AWS_USE				,	0	} ,

	{ "ULVAC_AWS_USE_V"				,	_K_D_IO		,	ULVAC_AWS_USE_V				,	0	} ,
	{ "ULVAC_AWS_LIMIT_LOW_V"		,	_K_A_IO		,	ULVAC_AWS_LIMIT_LOW_V		,	0	} ,
	{ "ULVAC_AWS_LIMIT_HI_V"		,	_K_A_IO		,	ULVAC_AWS_LIMIT_HI_V		,	0	} ,

	// PAUSEALL시 Alarm Recovery시 Success처리 (2013.01.29,LJH)
//	{ "Run_Resume_Scenario"			, _K_D_IO	,	Run_Resume_Scenario			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

/*
void File_Log( char* str ) {
	FILE *fp = NULL;
	char Path[MAX_PATH] , FileName[64];
	SYSTEMTIME st;

	GetLocalTime( &st );

	sprintf( FileName , "%04d%02d_TM_AWS.log" , st.wYear , st.wMonth );
	sprintf( Path , "F:\\DATALOG\\_Log\\%s" , FileName );
	fp = fopen( Path , "a+" );
	//[ 2008.08.21 TM AWS Log 예외 처리. ( BY LJH ) 
	if( !fp ) {
		CreateDirectory ( "F:/DATALOG/TM_AWS" , NULL );
		fp = fopen( Path , "a+" );
	}
	if( !fp ) {
		printf( "[TM Rblow - Function] : Can't Create Log File\n"  );
		return; 
	}else{
		fprintf( fp , "%04d/%02d/%02d %02d:%02d:%02d\t" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond );
		fprintf( fp , "%s\n" , str );
		fclose( fp );
	}
	//]
}
*/

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {
	char Station[41] , Arm[41] , AArm_Wafer[41] , BArm_Wafer[41];
	//---> End    2007/12/31 TP 개선  ( 9 of )

	char Buffer[256] , Buffer2[256], Buffer3[256];

	char LogMsg[256] ;
	//[ 2010/12/27 TM 7Axis Ulvac Robot AWS Data Initialize ( BY LJH )
	//int i ; 
	//]
	int	Dl , CommStatus;
	unsigned char *Ed;
	double AWS_Read1_X , AWS_Read2_X, AWS_Result1_X, AWS_Result2_X ;
	double AWS_Read1_Y , AWS_Read2_Y, AWS_Result1_Y, AWS_Result2_Y ;
	//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
	double TMRB_Aws_Limit_Hi, TMRB_Aws_Limit_Low;
	int AWS_READ_COUNT = 0 , AlmResult ;
	//]
//	double AWS_Warn_Limit_Lo , AWS_Warn_Limit_Hi ;
	double AWS_Warn_Limit_Hi ;
//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
	int PickStationA , PickStationB ; 
	char PickStrA[20] , PickStrB[20];
//]

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	STR_SEPERATE ( PROGRAM_PARAMETER_READ()	, Station     , Arm	, 40 );
	STR_SEPERATE ( Arm			, Arm			, AArm_Wafer	, 40 );
	STR_SEPERATE ( AArm_Wafer	, AArm_Wafer    , BArm_Wafer	, 40 );

	//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
	if ( STRCMP_L( Station , "SET_AWS_LIMIT" ) ) 
	{
		//Test
		_LOG("%s", Station);

		if ( STRCMP_L ( Arm , "PM" ) )
		{
			TMRB_Aws_Limit_Hi = READ_ANALOG ( ULVAC_AWS_LIMIT_PM_HI , &CommStatus ) ;
			TMRB_Aws_Limit_Low = READ_ANALOG ( ULVAC_AWS_LIMIT_PM_LOW , &CommStatus ) ;
		}
		else if ( STRCMP_L ( Arm , "BM" ) )
		{
			TMRB_Aws_Limit_Hi = READ_ANALOG ( ULVAC_AWS_LIMIT_BM_HI , &CommStatus ) ;
			TMRB_Aws_Limit_Low = READ_ANALOG ( ULVAC_AWS_LIMIT_BM_LOW , &CommStatus ) ;
		}

		while ( TRUE )
		{
			//-->2012/07/12 (ECID)AWS USE, LIMIT_LOW, LIMIT_HI Virtual IO Report
			WRITE_DIGITAL ( ULVAC_AWS_USE , READ_DIGITAL( ULVAC_AWS_USE_V, &CommStatus ) , &CommStatus ) ;

			WRITE_ANALOG ( ULVAC_AWS_LIMIT_LOW , TMRB_Aws_Limit_Low , &CommStatus ) ;

			WRITE_ANALOG ( ULVAC_AWS_LIMIT_HI , TMRB_Aws_Limit_Hi , &CommStatus ) ;

			if ( ( (int)( READ_ANALOG ( ULVAC_AWS_LIMIT_HI , &CommStatus ) * 10 ) == (int)(TMRB_Aws_Limit_Hi * 10) )
				&& ( (int)(READ_ANALOG ( ULVAC_AWS_LIMIT_LOW , &CommStatus ) * 10 ) == (int)( TMRB_Aws_Limit_Low * 10 ) )
				&& ( READ_DIGITAL ( ULVAC_AWS_USE , &CommStatus ) == READ_DIGITAL( ULVAC_AWS_USE_V, &CommStatus ) ) ) break ;

			if ( AWS_READ_COUNT >= 3 ) 
			{
				sprintf( Buffer2 , ": AWS Limit Setting Error(Set:[H]%.1f[L]%.1f|Value:[H]%.1f[L]%.1f)",TMRB_Aws_Limit_Hi,TMRB_Aws_Limit_Low,READ_ANALOG(ULVAC_AWS_LIMIT_HI,&CommStatus),READ_ANALOG(ULVAC_AWS_LIMIT_LOW,&CommStatus));
				ALARM_MESSAGE_GET( ALARM_COMM_ERROR , Buffer );
				strcat( Buffer , Buffer2 );
				ALARM_MESSAGE_SET( ALARM_COMM_ERROR , Buffer );

				AlmResult = ALARM_MANAGE ( ALARM_COMM_ERROR );
				if ( AlmResult != ALM_RETRY  ) 
				{
					//if ( READ_DIGITAL ( Run_Resume_Scenario , &CommStatus ) == Alarm_Clr_2 ) return SYS_SUCCESS ;
					return SYS_ABORTED;
				}
				else 
				{
					AWS_READ_COUNT = 0 ;
				}
			}
			AWS_READ_COUNT ++ ;
		}

		WRITE_ANALOG ( ULVAC_AWS_LIMIT_LOW_V	, TMRB_Aws_Limit_Low , &CommStatus ) ;
		WRITE_ANALOG ( ULVAC_AWS_LIMIT_HI_V		, TMRB_Aws_Limit_Hi , &CommStatus ) ;

		return SYS_SUCCESS ;
	}
	else
	{
	//]
		/*
		//remark 2014.03.16 by hyuk
		//[ 2010/12/27 TM 7Axis Ulvac Robot AWS Data Initialize ( BY LJH )
		for ( i = 0 ; i < 40 ; i ++ ) 
		{
			WRITE_ANALOG ( PMA_SLIDE_A1_X_AXIS + i   ,  0 , &CommStatus ) ;
		}
		//]
		*/

	//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
		PickStationA = READ_DIGITAL ( TM_Pick_U_Arm_Station , &CommStatus ) 	;
		PickStationB = READ_DIGITAL ( TM_Pick_L_Arm_Station , &CommStatus ) 	;

		switch ( PickStationA ) {
		case 0:
			sprintf ( PickStrA , "None" );
			break;
		case 1:
			sprintf ( PickStrA , "PMA" );
			break;
		case 2:
			sprintf ( PickStrA , "PMB" );
			break;
		case 3:
			sprintf ( PickStrA , "PMC" );
			break;
		case 4:
			sprintf ( PickStrA , "BM1" );
			break;
		case 5:
			sprintf ( PickStrA , "BM2" );
			break;
		}

		switch ( PickStationB ) {
		case 0:
			sprintf ( PickStrB , "None" );
			break;
		case 1:
			sprintf ( PickStrB , "PMA" );
			break;
		case 2:
			sprintf ( PickStrB , "PMB" );
			break;
		case 3:
			sprintf ( PickStrB , "PMC" );
			break;
		case 4:
			sprintf ( PickStrB , "BM1" );
			break;
		case 5:
			sprintf ( PickStrB , "BM2" );
			break;
		}
	//]

	//	AWS_Warn_Limit_Lo = READ_ANALOG (ULVAC_AWS_LIMIT_WARN_LO, &CommStatus) ;
		AWS_Warn_Limit_Hi = READ_ANALOG (ULVAC_AWS_LIMIT_WARN_HI, &CommStatus) ;

		//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
		TMRB_Aws_Limit_Hi   = READ_ANALOG (ULVAC_AWS_LIMIT_HI, &CommStatus) ;
		TMRB_Aws_Limit_Low  = READ_ANALOG (ULVAC_AWS_LIMIT_LOW, &CommStatus) ;
		//]

		if		( atoi(Arm) == ARM_A ) {
			WRITE_DIGITAL ( ULVAC_AWS_RESULT_A , 0 , &CommStatus ) ;
			WRITE_DIGITAL ( ULVAC_AWS_RESULT_B , 0 , &CommStatus ) ;
			WRITE_DIGITAL ( ULVAC_AWS_READ_A   , 0 , &CommStatus ) ;
			WRITE_DIGITAL ( ULVAC_AWS_READ_B   , 0 , &CommStatus ) ;

		//	_sleep (1000) ; // [2011/09/23 불필요한 Delay 제거 ( BY LJH )

			AWS_Read1_X   = READ_ANALOG (ULVAC_AWS_READ_A_X, &CommStatus) ;
			AWS_Result1_X = READ_ANALOG (ULVAC_AWS_RESULT_A_X, &CommStatus) ;
			AWS_Read1_Y   = READ_ANALOG (ULVAC_AWS_READ_A_Y, &CommStatus) ;
			AWS_Result1_Y = READ_ANALOG (ULVAC_AWS_RESULT_A_Y, &CommStatus) ;

			AWS_Read2_X   = READ_ANALOG (ULVAC_AWS_READ_B_X, &CommStatus) ;
			AWS_Result2_X = READ_ANALOG (ULVAC_AWS_RESULT_B_X, &CommStatus) ;
			AWS_Read2_Y   = READ_ANALOG (ULVAC_AWS_READ_B_Y, &CommStatus) ;
			AWS_Result2_Y = READ_ANALOG (ULVAC_AWS_RESULT_B_Y, &CommStatus) ;

 			//[ added 2009/05/28 TM Robot Slide data svid 등록
			if	    (  STRCMP_L( Station , "A_PM1"  )    ) {
					WRITE_ANALOG ( PMA_SLIDE_A1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( PMA_SLIDE_A1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( PMA_SLIDE_A2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( PMA_SLIDE_A2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(   STRCMP_L( Station , "A_PM2"  ) ) {
					WRITE_ANALOG ( PMB_SLIDE_A1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( PMB_SLIDE_A1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( PMB_SLIDE_A2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( PMB_SLIDE_A2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(   STRCMP_L( Station , "A_PM3"  ) ) {
					WRITE_ANALOG ( PMC_SLIDE_A1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( PMC_SLIDE_A1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( PMC_SLIDE_A2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( PMC_SLIDE_A2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(   STRCMP_L( Station , "A_BM1"  )   || 
						 STRCMP_L( Station , "A_BM1_1"  ) || STRCMP_L( Station , "A_BM1_2" ) ) {
					WRITE_ANALOG ( BM1_SLIDE_A1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( BM1_SLIDE_A1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( BM1_SLIDE_A2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( BM1_SLIDE_A2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(    STRCMP_L( Station , "A_BM2"  )   || 
						 STRCMP_L( Station , "A_BM2_1"  ) || STRCMP_L( Station , "A_BM2_2" ) ) {
					WRITE_ANALOG ( BM2_SLIDE_A1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( BM2_SLIDE_A1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( BM2_SLIDE_A2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( BM2_SLIDE_A2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			//]
			
			//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
			//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
			sprintf ( LogMsg, "[Pick(%s)] %s Place <A1_Arm(%s$): X_Axis[%.2f/%.2f],Y_Axis[%.2f/%.2f]> <A2_Arm(%s$): X_Axis[%.2f/%.2f],Y_Axis[%.2f/%.2f]><Limit[HI%.2f/LOW%.2f]>" 
			, PickStrA , Station , AArm_Wafer , AWS_Read1_X , AWS_Result1_X , AWS_Read1_Y , AWS_Result1_Y , 
			BArm_Wafer , AWS_Read2_X , AWS_Result2_X , AWS_Read2_Y , AWS_Result2_Y, TMRB_Aws_Limit_Hi, TMRB_Aws_Limit_Low );
			//]
			//]
			printf("%s\n", LogMsg );
			_LOG( LogMsg ) ;

			if ( (fabs(AWS_Result1_X) > AWS_Warn_Limit_Hi) || (fabs(AWS_Result1_Y) > AWS_Warn_Limit_Hi) || (fabs(AWS_Result2_X) > AWS_Warn_Limit_Hi) || (fabs(AWS_Result2_Y) > AWS_Warn_Limit_Hi) ) {

				sprintf( Buffer2 , "(A[%s] X:%.1f Y:%.1f) (B[%s] X:%.1f Y:%.1f)" , AArm_Wafer , AWS_Result1_X , AWS_Result1_Y , BArm_Wafer , AWS_Result2_X , AWS_Result2_Y );
				sprintf( Buffer3 , "/(Range = -%.1f ~ %.1f)" , AWS_Warn_Limit_Hi , AWS_Warn_Limit_Hi );

				ALARM_MESSAGE_GET( A_ARM_AWS_WARN , Buffer );
				strcat( Buffer2 , Buffer3 );
				strcat( Buffer , Buffer2 );
				ALARM_MESSAGE_SET( A_ARM_AWS_WARN , Buffer );

				if ( ALARM_STATUS ( A_ARM_AWS_WARN ) != ALM_PAUSED ) ALARM_POST ( A_ARM_AWS_WARN ) ;
			}
		}

		else if ( atoi(Arm) == ARM_B ) {
			WRITE_DIGITAL ( ULVAC_AWS_RESULT_C , 0 , &CommStatus ) ;
			WRITE_DIGITAL ( ULVAC_AWS_RESULT_D , 0 , &CommStatus ) ;
			WRITE_DIGITAL ( ULVAC_AWS_READ_C   , 0 , &CommStatus ) ;
			WRITE_DIGITAL ( ULVAC_AWS_READ_D   , 0 , &CommStatus ) ;
			
		//	_sleep (1000) ; // [2011/09/23 불필요한 Delay 제거 ( BY LJH )

			AWS_Read1_X   = READ_ANALOG (ULVAC_AWS_READ_C_X, &CommStatus) ;
			AWS_Result1_X = READ_ANALOG (ULVAC_AWS_RESULT_C_X, &CommStatus) ;
			AWS_Read1_Y   = READ_ANALOG (ULVAC_AWS_READ_C_Y, &CommStatus) ;
			AWS_Result1_Y = READ_ANALOG (ULVAC_AWS_RESULT_C_Y, &CommStatus) ;

			AWS_Read2_X   = READ_ANALOG (ULVAC_AWS_READ_D_X, &CommStatus) ;
			AWS_Result2_X = READ_ANALOG (ULVAC_AWS_RESULT_D_X, &CommStatus) ;
			AWS_Read2_Y   = READ_ANALOG (ULVAC_AWS_READ_D_Y, &CommStatus) ;
			AWS_Result2_Y = READ_ANALOG (ULVAC_AWS_RESULT_D_Y, &CommStatus) ;

			//[ added 2009/05/28 TM Robot Slide data svid 등록
			if	    (  STRCMP_L( Station , "B_PM1"  )    ) {
					WRITE_ANALOG ( PMA_SLIDE_B1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( PMA_SLIDE_B1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( PMA_SLIDE_B2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( PMA_SLIDE_B2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(   STRCMP_L( Station , "B_PM2"  ) ) {
					WRITE_ANALOG ( PMB_SLIDE_B1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( PMB_SLIDE_B1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( PMB_SLIDE_B2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( PMB_SLIDE_B2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(   STRCMP_L( Station , "B_PM3"  ) ) {
					WRITE_ANALOG ( PMC_SLIDE_B1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( PMC_SLIDE_B1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( PMC_SLIDE_B2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( PMC_SLIDE_B2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(   STRCMP_L( Station , "B_BM1"  )   || 
						 STRCMP_L( Station , "B_BM1_1"  ) || STRCMP_L( Station , "B_BM1_2" ) ) {
					WRITE_ANALOG ( BM1_SLIDE_B1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( BM1_SLIDE_B1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( BM1_SLIDE_B2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( BM1_SLIDE_B2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			else if	(    STRCMP_L( Station , "B_BM2"  )   || 
						 STRCMP_L( Station , "B_BM2_1"  ) || STRCMP_L( Station , "B_BM2_2" ) ) {
					WRITE_ANALOG ( BM2_SLIDE_B1_X_AXIS  ,  AWS_Read1_X , &CommStatus ) ;
					WRITE_ANALOG ( BM2_SLIDE_B1_Y_AXIS  ,  AWS_Read1_Y , &CommStatus ) ;
					WRITE_ANALOG ( BM2_SLIDE_B2_X_AXIS  ,  AWS_Read2_X , &CommStatus ) ;
					WRITE_ANALOG ( BM2_SLIDE_B2_Y_AXIS  ,  AWS_Read2_Y , &CommStatus ) ;
			}
			//]

			//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
			//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
	//		sprintf ( LogMsg, "%s Place B1_Arm_WfsNo[%s] : X_Axis [Read:%.2f , Result:%.2f]	Y_Axis	[Read:%.2f , Result:%.2f]	B2_Arm_WfsNo[%s] : X_Axis	[Read:%.2f , Result:%.2f]	Y_Axis	[Read:%.2f , Result:%.2f]" , Station ,
			sprintf ( LogMsg, "[Pick(%s)] %s Place <B1_Arm(%s$): X_Axis[%.2f/%.2f],Y_Axis[%.2f/%.2f]> <B2_Arm(%s$): X_Axis[%.2f/%.2f],Y_Axis[%.2f/%.2f]><Limit[HI%.2f/LOW%.2f]>" 
			, PickStrB , Station ,	AArm_Wafer , AWS_Read1_X , AWS_Result1_X , AWS_Read1_Y , AWS_Result1_Y , 
			BArm_Wafer , AWS_Read2_X , AWS_Result2_X , AWS_Read2_Y , AWS_Result2_Y, TMRB_Aws_Limit_Hi, TMRB_Aws_Limit_Low );
			//]
			//]
			printf("%s\n", LogMsg );
			_LOG( LogMsg ) ;

			if ( (fabs(AWS_Result1_X) > AWS_Warn_Limit_Hi) || (fabs(AWS_Result1_Y) > AWS_Warn_Limit_Hi) || (fabs(AWS_Result2_X) > AWS_Warn_Limit_Hi) || (fabs(AWS_Result2_Y) > AWS_Warn_Limit_Hi) ) {

				sprintf( Buffer2 , "(C[%s] X:%.1f Y:%.1f) (D[%s] X:%.1f Y:%.1f)" , AArm_Wafer , AWS_Result1_X , AWS_Result1_Y , BArm_Wafer , AWS_Result2_X , AWS_Result2_Y );
				sprintf( Buffer3 , "/(Range = -%.1f ~ %.1f)" , AWS_Warn_Limit_Hi , AWS_Warn_Limit_Hi );

				ALARM_MESSAGE_GET( B_ARM_AWS_WARN , Buffer );
				strcat( Buffer2 , Buffer3 );
				strcat( Buffer , Buffer2 );
				ALARM_MESSAGE_SET( B_ARM_AWS_WARN , Buffer );

				if ( ALARM_STATUS ( B_ARM_AWS_WARN ) != ALM_PAUSED ) ALARM_POST ( B_ARM_AWS_WARN ) ;
			}
		}	

		return SYS_SUCCESS ;
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == OFF ) SIMULATION_MODE = TRUE;
	else														 SIMULATION_MODE = FALSE;
	return TRUE;
}