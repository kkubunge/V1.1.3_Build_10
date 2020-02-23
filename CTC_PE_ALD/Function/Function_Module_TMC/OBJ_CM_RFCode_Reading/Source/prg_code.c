#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE	= FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int			ALARM_START_POINTER = 100;
int			CASSETTE_INDEX		= 1;
int			MAX_PAGE_COUNT		= 17;
//------------------------------------------------------------------------------------------
#define		ALARM_RFID_COMM_ERROR			0
#define		ALARM_RFID_ERROR				1
#define		ALARM_RFID_UNKNOWN_RANGE		2
#define		ALARM_FOUP_ABSENT				3
#define		ALARM_FOUP_NOTUNDOCK			4
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { RUN		};
enum { OFF		, ON		};
enum { ON2		, OFF2		};
enum { SIM		, REAL		};
enum { ABSENT	, PRESENT	};
enum { UNKNOWN2 , IN1 , OUT1 , ERROR2 };
enum { PAGE1, PAGE2 ,PAGE3 , PAGE4 , PAGE5 , PAGE6 , PAGE7 , PAGE8 , PAGE9 , PAGE10 , PAGE11 , PAGE12 , PAGE13 , PAGE14 , PAGE15 , PAGE16 , PAGE17 };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char *EVENT_STRING_RFID_NOUSE		= "EVENT_ON RF_NOT_USE_MODE PORT%d";
char *EVENT_STRING_RFATREAD_COMPT	= "EVENT_ON MID_AUTO_READ_COMPLETE PORT%d";
char *EVENT_STRING_RFATREAD_FAIL	= "EVENT_ON MID_AUTO_READ_FAIL PORT%d";
char *EVENT_STRING_RFMIDREAD_COMPT	= "EVENT_ON MID_READ_COMPLETE PORT%d";
char *EVENT_STRING_RFMIDREAD_FAIL	= "EVENT_ON MID_READ_FAIL PORT%d";
char *EVENT_STRING_RFDATAREAD_COMPT = "EVENT_ON RFDATA_READ_COMPLETE PORT%d";
char *EVENT_STRING_RFDATAREAD_FAIL	= "EVENT_ON RFDATA_READ_FAIL PORT%d";
char *EVENT_STRING_RFMIDWRITE_COMPT	= "EVENT_ON MID_WRITE_COMPLETE PORT%d";
char *EVENT_STRING_RFMIDWRITE_FAIL	= "EVENT_ON MID_WRITE_FAIL PORT%d";
char *EVENT_STRING_RFDATAWRITE_COMPT= "EVENT_ON RFDATA_WRITE_COMPLETE PORT%d";
char *EVENT_STRING_RFDATAWRITE_FAIL	= "EVENT_ON RFDATA_WRITE_FAIL PORT%d";
char *EVENT_CARR_ASSOCIATED			= "EVENT_ON CARR_ASSOCIATED_F PORT%d";
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding		,	0	} ,

	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER				,	0	} ,

	{ "FC_$0_MID_CONTROL"		, _K_D_IO	,	RFID_CONTROL			,	0	} ,

	{ "GEM.$1_LotID"			, _K_S_IO	,	LOTID_NAME				,	0	} ,

	{ "CTC.MID_READ$2"			, _K_S_IO	,	MID_READ				,	0	} ,
	{ "CTC.MID_NAME$2"			, _K_S_IO	,	MID_NAME				,	0	} ,

	{ "$0.RFID_MID_PAGE"		, _K_S_IO	,	RFID_MID_PAGE			,	0	} ,
	{ "$0.RFID_PAGE1"			, _K_S_IO	,	RFID_PAGE1				,	0	} ,
	{ "$0.RFID_PAGE2"			, _K_S_IO	,	RFID_PAGE2				,	0	} ,
	{ "$0.RFID_PAGE3"			, _K_S_IO	,	RFID_PAGE3				,	0	} ,
	{ "$0.RFID_PAGE4"			, _K_S_IO	,	RFID_PAGE4				,	0	} ,
	{ "$0.RFID_PAGE5"			, _K_S_IO	,	RFID_PAGE5				,	0	} ,
	{ "$0.RFID_PAGE6"			, _K_S_IO	,	RFID_PAGE6				,	0	} ,
	{ "$0.RFID_PAGE7"			, _K_S_IO	,	RFID_PAGE7				,	0	} ,
	{ "$0.RFID_PAGE8"			, _K_S_IO	,	RFID_PAGE8				,	0	} ,
	{ "$0.RFID_PAGE9"			, _K_S_IO	,	RFID_PAGE9				,	0	} ,
	{ "$0.RFID_PAGE10"			, _K_S_IO	,	RFID_PAGE10				,	0	} ,
	{ "$0.RFID_PAGE11"			, _K_S_IO	,	RFID_PAGE11				,	0	} ,
	{ "$0.RFID_PAGE12"			, _K_S_IO	,	RFID_PAGE12				,	0	} ,
	{ "$0.RFID_PAGE13"			, _K_S_IO	,	RFID_PAGE13				,	0	} ,
	{ "$0.RFID_PAGE14"			, _K_S_IO	,	RFID_PAGE14				,	0	} ,
	{ "$0.RFID_PAGE15"			, _K_S_IO	,	RFID_PAGE15				,	0	} ,
	{ "$0.RFID_PAGE16"			, _K_S_IO	,	RFID_PAGE16				,	0	} ,
	{ "$0.RFID_PAGE17"			, _K_S_IO	,	RFID_PAGE17				,	0	} ,

	{ "HRF$2Err_Status"			, _K_D_IO	,	RFID_ERROR				,	0	} ,
	{ "HRF$2Read"				, _K_D_IO	,	RFID_READ				,	0	} ,
	{ "HRF$2Trigger"			, _K_D_IO	,	RFID_WRITE				,	0	} ,
	{ "HRF$2Reset"				, _K_D_IO	,	RFID_RESET				,	0	} ,
	{ "HRF$2PageLock"			, _K_D_IO	,	RFID_PAGELOCK			,	0	} ,

	{ "HRF$2Response1"			, _K_S_IO	,	RFID_READ_PAGE1			,	0	} ,
	{ "HRF$2Response2"			, _K_S_IO	,	RFID_READ_PAGE2			,	0	} ,
	{ "HRF$2Response3"			, _K_S_IO	,	RFID_READ_PAGE3			,	0	} ,
	{ "HRF$2Response4"			, _K_S_IO	,	RFID_READ_PAGE4			,	0	} ,
	{ "HRF$2Response5"			, _K_S_IO	,	RFID_READ_PAGE5			,	0	} ,
	{ "HRF$2Response6"			, _K_S_IO	,	RFID_READ_PAGE6			,	0	} ,
	{ "HRF$2Response7"			, _K_S_IO	,	RFID_READ_PAGE7			,	0	} ,
	{ "HRF$2Response8"			, _K_S_IO	,	RFID_READ_PAGE8			,	0	} ,
	{ "HRF$2Response9"			, _K_S_IO	,	RFID_READ_PAGE9			,	0	} ,
	{ "HRF$2Response10"			, _K_S_IO	,	RFID_READ_PAGE10		,	0	} ,
	{ "HRF$2Response11"			, _K_S_IO	,	RFID_READ_PAGE11		,	0	} ,
	{ "HRF$2Response12"			, _K_S_IO	,	RFID_READ_PAGE12		,	0	} ,
	{ "HRF$2Response13"			, _K_S_IO	,	RFID_READ_PAGE13		,	0	} ,
	{ "HRF$2Response14"			, _K_S_IO	,	RFID_READ_PAGE14		,	0	} ,
	{ "HRF$2Response15"			, _K_S_IO	,	RFID_READ_PAGE15		,	0	} ,
	{ "HRF$2Response16"			, _K_S_IO	,	RFID_READ_PAGE16		,	0	} ,
	{ "HRF$2Response17"			, _K_S_IO	,	RFID_READ_PAGE17		,	0	} ,

	{ "HRF$2SendTag"			, _K_S_IO	,	RFID_WRITE_PAGE			,	0	} ,

	{ "$3.F_ProperlyPlaceDI"	, _K_D_IO	,	FOUP_STATUS				,	0	} ,
	{ "$3.ShuttleInOutXI"		, _K_D_IO	,	DOCK_STATUS				,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL ALARM_MESSAGE_REMAPPING( int id , int error ) {
	char Buffer[256] , Buffer2[32];
	if ( error > 0 ) {
		sprintf( Buffer2 , "(Err:%d)" , error );
		ALARM_MESSAGE_GET( id , Buffer );
		strcat( Buffer , Buffer2 );
		ALARM_MESSAGE_SET( id , Buffer );
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status READ_DIGITAL_TRUE( int io , int *Data ) {
	int i , AlmResult , CommStatus;
	
	*Data = -1;
	while ( TRUE ) {
		for( i = 0 ; i < 3 ; i++ ) {
			*Data = READ_DIGITAL( io , &CommStatus );
			if ( !CommStatus ) {
				if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
			}
			else break;
		}

		if ( !CommStatus ) {
			*Data = -1;
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_RFID_COMM_ERROR );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status WRITE_DIGITAL_TRUE( int io , int Data, int nIdx) {
	int i , AlmResult , CommStatus;
	char szBuffer[100];

	memset(szBuffer,0,sizeof(szBuffer));
	
	while ( TRUE ) {
		for( i = 0 ; i < 3 ; i++ ) {
			WRITE_DIGITAL( io , Data , &CommStatus );
			if ( !CommStatus ) {
				if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
			}
			else break;
		}
		//... 2016.03.14
		READ_STRING(RFID_READ_PAGE1 + nIdx,szBuffer,&CommStatus);

		if ( !CommStatus || (0 == strlen(szBuffer) && nIdx == 0)) {
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_RFID_COMM_ERROR );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PreCheck() {
	int Data , AlmResult;
	while( TRUE ) {
		if ( READ_DIGITAL_TRUE( FOUP_STATUS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data != PRESENT ) {
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_FOUP_ABSENT );
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	while( TRUE ) {
		if ( READ_DIGITAL_TRUE( DOCK_STATUS , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data != OUT1 ) {
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_FOUP_NOTUNDOCK );
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	/* IMSI - 에러 체크 io를 확인 해야 함. 2004.5.13
	while( TRUE ) {
		if ( READ_DIGITAL_TRUE( RFID_ERROR , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data != 0 ) {
			ALARM_MESSAGE_REMAPPING( ALARM_START_POINTER + ALARM_RFID_ERROR , Data );
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_RFID_ERROR );
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	*/
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PostCheck() {
//	int Data , AlmResult;
	/* IMSI - 에러 체크 io를 확인 해야 함. 2004.5.13
	while( TRUE ) {
		if ( READ_DIGITAL_TRUE( RFID_ERROR , &Data ) == SYS_ABORTED ) return SYS_ABORTED;
		if ( Data != 0 ) {
			ALARM_MESSAGE_REMAPPING( ALARM_START_POINTER + ALARM_RFID_ERROR , Data );
			AlmResult = ALARM_MANAGE( ALARM_START_POINTER + ALARM_RFID_ERROR );
			if      ( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else break;
	}
	*/
	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status MIDRead_Control() {
	int i , CommStatus;
	char Buffer[256] , Buffer2[256] ,Page1[256];
	Module_Status Res = SYS_SUCCESS;

	printf( "=====> Start CM%d RFID All Read(Auto)\n" , CASSETTE_INDEX );
	if ( READ_DIGITAL( RFID_CONTROL  , &CommStatus ) == OFF2 ) {
//		sprintf( Buffer , EVENT_STRING_RFID_NOUSE , CASSETTE_INDEX );
//		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		return SYS_SUCCESS;
	}

	sprintf( Page1 , "" ); sprintf( Buffer2 , "" );
	if ( PreCheck() != SYS_ABORTED ) {
//		for ( i = 0 ; i < MAX_PAGE_COUNT ; i++ ) { // from MID Page to Page17
		for ( i = 0 ; i < 3 ; i++ ) { 
			sprintf( Buffer , "" );
			if ( SIMULATION_MODE ) {
				sprintf( Buffer , "PT%d_RF%02d" , CASSETTE_INDEX , i+1 );
				WRITE_STRING( RFID_READ_PAGE1 + i , Buffer , &CommStatus );
			}
			if ( WRITE_DIGITAL_TRUE( RFID_READ , PAGE1 + i , i) == SYS_ABORTED ) { Res = SYS_ABORTED; break; } 

			if ( PostCheck() == SYS_ABORTED ) { Res = SYS_ABORTED; break; }

			READ_STRING( RFID_READ_PAGE1 + i , Buffer , &CommStatus );
			WRITE_STRING( RFID_PAGE1 + i , Buffer , &CommStatus );
			if		( i == 0 ) strncpy( Page1 , Buffer , 255 );
			else if ( i == 1 ) {
				sprintf( Buffer2 , "%s%s" , Page1 , Buffer );
				WRITE_STRING( MID_READ , Buffer2 , &CommStatus );
				WRITE_STRING( MID_NAME , Buffer2 , &CommStatus );
				WRITE_STRING( RFID_MID_PAGE , Buffer2 , &CommStatus );
				WRITE_UPLOAD_MESSAGE( Buffer2 );
			}
			else if ( i == 2 ) WRITE_STRING( LOTID_NAME , Buffer , &CommStatus );
		}
		if ( SIMULATION_MODE ) {
			if ( !WAIT_SECONDS( 2 ) ) Res = SYS_ABORTED;
		}
	}
	else Res = SYS_ABORTED; 

	if ( Res == SYS_ABORTED ) {
//		sprintf( Buffer , EVENT_STRING_RFATREAD_FAIL , CASSETTE_INDEX );
//		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		return SYS_ABORTED; 
	}

//	sprintf( Buffer , EVENT_STRING_RFATREAD_COMPT , CASSETTE_INDEX );
//	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
//	sprintf( Buffer , EVENT_CARR_ASSOCIATED , CASSETTE_INDEX );
//	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );

	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status RFRead_Control( char *ReadData ) {
	int i , CommStatus , RFID;
	char Buffer[256] , Buffer2[256] , Buffer3[256] , RFIDStr[32] , TempStr[32];
	BOOL EVENT_MODE = FALSE;
	Module_Status Res = SYS_SUCCESS;

	STR_SEPERATE_CHAR( ReadData	, '|' , RFIDStr , TempStr , 31 );
	RFID = atoi( RFIDStr ); // 0 ~ 17 page number, 0 = mid page
	if ( STRCMP_L( TempStr , "E" ) ) EVENT_MODE = TRUE; else EVENT_MODE = FALSE;
	
	printf( "=====> Start CM%d RFID %d Read\n" , CASSETTE_INDEX , RFID );
	if ( RFID < 0 || RFID > MAX_PAGE_COUNT ) {
		ALARM_POST( ALARM_START_POINTER + ALARM_RFID_UNKNOWN_RANGE );
		return SYS_ABORTED;
	}

	if ( READ_DIGITAL( RFID_CONTROL  , &CommStatus ) == OFF2 ) {
		if ( EVENT_MODE ) {
			sprintf( Buffer , EVENT_STRING_RFID_NOUSE , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		return SYS_SUCCESS;
	}
	
	i = RFID;
	if ( PreCheck() != SYS_ABORTED ) {
		while ( TRUE ) {
			sprintf( Buffer , "" );
			if ( SIMULATION_MODE ) {
				if ( i == 0 ) {
					sprintf( Buffer , "PT%d_RF%02d" , CASSETTE_INDEX , 1 );
					WRITE_STRING( RFID_READ_PAGE1 , Buffer , &CommStatus );
					sprintf( Buffer , "PT%d_RF%02d" , CASSETTE_INDEX , 2 );
					WRITE_STRING( RFID_READ_PAGE2 , Buffer , &CommStatus );
				}
				else {
					sprintf( Buffer , "PT%d_RF%02d" , CASSETTE_INDEX , i );
					WRITE_STRING( RFID_READ_PAGE1 + i - 1 , Buffer , &CommStatus );
				}
			}
			if ( i == 0 ) {
				if ( WRITE_DIGITAL_TRUE( RFID_READ , PAGE1 + 0 , 0) == SYS_ABORTED ) { Res = SYS_ABORTED; break; }
				if ( WRITE_DIGITAL_TRUE( RFID_READ , PAGE1 + 1 , 1) == SYS_ABORTED ) { Res = SYS_ABORTED; break; }
			}
			else {
				if ( WRITE_DIGITAL_TRUE( RFID_READ , PAGE1 + i - 1 , i - 1 ) == SYS_ABORTED ) { Res = SYS_ABORTED; break; }
			}

			if ( PostCheck() == SYS_ABORTED ) { Res = SYS_ABORTED; break; }

			if		( i == 0 ) {
				READ_STRING( RFID_READ_PAGE1 , Buffer2 , &CommStatus );
				WRITE_STRING( RFID_PAGE1 , Buffer2 , &CommStatus );
				READ_STRING( RFID_READ_PAGE2 , Buffer3 , &CommStatus );
				WRITE_STRING( RFID_PAGE2 , Buffer3 , &CommStatus );
				sprintf( Buffer , "%s%s" , Buffer2 , Buffer3 );
//				WRITE_STRING( MID_READ , Buffer , &CommStatus );
//				WRITE_STRING( MID_NAME , Buffer , &CommStatus );
				WRITE_STRING( RFID_MID_PAGE , Buffer , &CommStatus );
			}
			else if ( i == 3 ) {
				READ_STRING( RFID_READ_PAGE1 + i - 1 , Buffer , &CommStatus );
				WRITE_STRING( RFID_PAGE1 + i - 1 , Buffer , &CommStatus );
//				WRITE_STRING( LOTID_NAME , Buffer , &CommStatus );
			}
			else {
				READ_STRING( RFID_READ_PAGE1 + i - 1 , Buffer , &CommStatus );
				WRITE_STRING( RFID_PAGE1 + i - 1 , Buffer , &CommStatus );
			}
			
			break;
		}
		if ( SIMULATION_MODE ) {
			if ( !WAIT_SECONDS( 0.5 ) ) Res = SYS_ABORTED;
		}
	}
	else Res = SYS_ABORTED; 

	if ( Res == SYS_ABORTED ) {
		if ( EVENT_MODE ) {
			if ( RFID == 0 ) { // MID page
				sprintf( Buffer , EVENT_STRING_RFMIDREAD_FAIL , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			}
			else { // Data Page
				sprintf( Buffer , EVENT_STRING_RFDATAREAD_FAIL , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			}
		}
		return SYS_ABORTED;
	}

	if ( EVENT_MODE ) {
		if ( RFID == 0 ) {
			sprintf( Buffer , EVENT_STRING_RFMIDREAD_COMPT , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		else {
			sprintf( Buffer , EVENT_STRING_RFDATAREAD_COMPT , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
	}

	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status RFWrite_Control( char *WriteData ) {
	int i , CommStatus , RFID , len , j , k , p , q;
	char Buffer[256] , RFIDStr[32] , DataStr[32] , DataStr2[64] , TempStr[32] , TempStr2[32];
	BOOL EVENT_MODE = FALSE;
	Module_Status Res = SYS_SUCCESS;

	STR_SEPERATE( WriteData	, RFIDStr , TempStr , 31 );
	STR_SEPERATE_CHAR( TempStr	, '|' , DataStr , TempStr2, 31 );
	RFID = atoi( RFIDStr ); // 0 ~ 17 page number, 0 = mid page
	if ( STRCMP_L( TempStr2 , "E" ) ) EVENT_MODE = TRUE; else EVENT_MODE = FALSE;
	
	printf( "=====> Start CM%d RFID %d Write : %s\n" , CASSETTE_INDEX , RFID , DataStr );
	if ( RFID < 0 || RFID > MAX_PAGE_COUNT ) {
		ALARM_POST( ALARM_START_POINTER + ALARM_RFID_UNKNOWN_RANGE );
		return SYS_ABORTED;
	}

	if ( READ_DIGITAL( RFID_CONTROL  , &CommStatus ) == OFF2 ) {
		if ( EVENT_MODE ) {
			sprintf( Buffer , EVENT_STRING_RFID_NOUSE , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		return SYS_SUCCESS;
	}

	if ( PreCheck() != SYS_ABORTED ) {
		i = RFID;
		if ( i == 0 ) i = 1; // MID Page
		len = strlen( DataStr );
		if ( len <= 0 ) return SYS_SUCCESS;
		p = len/8;
		q = len%8;
		if ( q > 0 ) p = p + 1; 
		for ( k = 0 ; k < p ; k++ ) {
			if ( i + k > MAX_PAGE_COUNT ) break;
			strncpy( DataStr2 , DataStr + k*8 , 8 );
			if ( k == p-1 && q != 0 ) {
				for ( j = q ; j < 8 ; j++ )	DataStr2[j] = ' ';
			}
			DataStr2[8] = 0x00;
			while ( TRUE ) {
				WRITE_STRING( RFID_WRITE_PAGE , DataStr2 , &CommStatus );
				if ( WRITE_DIGITAL_TRUE( RFID_WRITE , PAGE1 + i + k - 1 , i + k - 1) == SYS_ABORTED ) { Res = SYS_ABORTED; break; }

				if ( PostCheck() == SYS_ABORTED ) { Res = SYS_ABORTED; break; }

				if ( RFID == 0 ) WRITE_STRING( RFID_MID_PAGE , DataStr , &CommStatus );
				WRITE_STRING( RFID_PAGE1 + i + k - 1 , DataStr2 , &CommStatus );

				if ( RFID == 0 ) {
//					WRITE_STRING( MID_READ , DataStr , &CommStatus );
//					WRITE_STRING( MID_NAME , DataStr , &CommStatus );
				}
				if ( i + k - 1 == 3 ) {
//					WRITE_STRING( LOTID_NAME , Buffer , &CommStatus );
				}
				
				break;
			}
			if ( Res == SYS_ABORTED ) break; 
		}
		if ( SIMULATION_MODE ) {
			if ( !WAIT_SECONDS( 0.5 ) ) Res = SYS_ABORTED;
		}
	}
	else Res = SYS_ABORTED;

	if ( Res == SYS_ABORTED ) {
		if ( EVENT_MODE ) {
			if ( RFID == 0 ) { // MID page
				sprintf( Buffer , EVENT_STRING_RFMIDWRITE_FAIL , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			}
			else { // Data Page
				sprintf( Buffer , EVENT_STRING_RFDATAWRITE_FAIL , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			}
		}
		return SYS_ABORTED;
	}

	if ( EVENT_MODE ) {
		if ( RFID == 0 ) {
			sprintf( Buffer , EVENT_STRING_RFMIDWRITE_COMPT , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
		else {
			sprintf( Buffer , EVENT_STRING_RFDATAWRITE_COMPT , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		}
	}

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int Dl;
	unsigned char *Ed;
	char RunStr[65] , ElseStr[65] ;

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( Dl < 1 ) {
		printf( "=====> CM%d RFID Control Parameter Error\n" , CASSETTE_INDEX );
		return SYS_ABORTED;
	}

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , ElseStr , 64 );

	if		( STRCMP_L( RunStr , "MID_READ"			) ) return( MIDRead_Control() );
	else if ( STRCMP_L( RunStr , "RFID_READ"		) ) return( RFRead_Control	( ElseStr ) );
	else if ( STRCMP_L( RunStr , "RFID_WRITE"		) ) return( RFWrite_Control	( ElseStr ) );
	else if ( STRCMP_L( RunStr , "READY"			) ) return SYS_SUCCESS;
	else {
		printf( "=====> CM%d RFID Check Error - Not Support Code %s\n" , CASSETTE_INDEX , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}
	return SYS_ABORTED;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char Str1[128] , Str2[128] , Str3[128] , Str4[128] , Str5[128] , Else[128];
	STR_SEPERATE( PROGRAM_ARGUMENT_READ() , Str1 , Else , 127 ); // "CM2 Port2 2 LPMA 100"
	REPLACE_CHAR_SET( 0 , Str1 );
	CASSETTE_INDEX = Str1[2] - '0';
	STR_SEPERATE( Else , Str2 , Else , 127 );
	REPLACE_CHAR_SET( 1 , Str2 );
	STR_SEPERATE( Else , Str3 , Else , 127 );
	if ( strcmp( Str3 , "1" ) == 0 ) sprintf( Str3 , "" );
	REPLACE_CHAR_SET( 2 , Str3 );
	STR_SEPERATE( Else , Str4 , Else , 127 );
	REPLACE_CHAR_SET( 3 , Str4 );
	STR_SEPERATE( Else , Str5 , Else , 127 );
	ALARM_START_POINTER = atoi( Str5 );
}
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == SIM )	SIMULATION_MODE = TRUE;
	else															SIMULATION_MODE = FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
