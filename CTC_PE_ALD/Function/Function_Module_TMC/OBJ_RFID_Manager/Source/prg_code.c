#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <KutlAnal.h>
#include <kutlgui.h>
#include <kutlStr.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		MAX_PAGE		20
#define		TOTAL_MAX_PAGE	43 // ID PAGE + DATA PAGE + DUMMY
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { RUN		};
enum { SIM		, REAL		};
enum { OFF		, ON		};
enum { ON2		, OFF2		};
enum { SUCCESS	, FAIL		};
enum { CM1		, CM2		};
enum { NONE		, PORT1		, PORT2		};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SIMULATION_MODE	= FALSE;
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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char ID[MAX_PAGE][256];
char DATA[MAX_PAGE][256];
char PortName[256];
int  CASSETTE_INDEX = PORT1; 
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding		,	0	} ,

	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER				,	0	} ,

	{ "$0_Verify_Result"		, _K_D_IO	,	MID_Verify_Result		,	0	} , 

	{ "CTC.MID_READ"			, _K_S_IO	,	LP_MID_READ				,	0	} ,
	{ "CTC.MID_READ2"			, _K_S_IO	,	LP_MID_READ2			,	0	} ,
	{ "CTC.MID_READ3"			, _K_S_IO	,	LP_MID_READ3			,	0	} ,

	{ "CTC.MID_NAME"			, _K_S_IO	,	LP_MID_NAME				,	0	} ,
	{ "CTC.MID_NAME2"			, _K_S_IO	,	LP_MID_NAME2			,	0	} ,
	{ "CTC.MID_NAME3"			, _K_S_IO	,	LP_MID_NAME3			,	0	} ,

	{ "FC_$1_MID_CONTROL"		, _K_D_IO	,	LP_RFID_USE				,	0	} ,

	{ "$1.RFID_MID_PAGE"		, _K_S_IO	,	RFID_MID_PAGE			,	0	} ,
	{ "$1.RFID_PAGE1"			, _K_S_IO	,	RFID_PAGE1				,	0	} ,
	{ "$1.RFID_PAGE2"			, _K_S_IO	,	RFID_PAGE2				,	0	} ,
	{ "$1.RFID_PAGE3"			, _K_S_IO	,	RFID_PAGE3				,	0	} ,
	{ "$1.RFID_PAGE4"			, _K_S_IO	,	RFID_PAGE4				,	0	} ,
	{ "$1.RFID_PAGE5"			, _K_S_IO	,	RFID_PAGE5				,	0	} ,
	{ "$1.RFID_PAGE6"			, _K_S_IO	,	RFID_PAGE6				,	0	} ,
	{ "$1.RFID_PAGE7"			, _K_S_IO	,	RFID_PAGE7				,	0	} ,
	{ "$1.RFID_PAGE8"			, _K_S_IO	,	RFID_PAGE8				,	0	} ,
	{ "$1.RFID_PAGE9"			, _K_S_IO	,	RFID_PAGE9				,	0	} ,
	{ "$1.RFID_PAGE10"			, _K_S_IO	,	RFID_PAGE10				,	0	} ,
	{ "$1.RFID_PAGE11"			, _K_S_IO	,	RFID_PAGE11				,	0	} ,
	{ "$1.RFID_PAGE12"			, _K_S_IO	,	RFID_PAGE12				,	0	} ,
	{ "$1.RFID_PAGE13"			, _K_S_IO	,	RFID_PAGE13				,	0	} ,
	{ "$1.RFID_PAGE14"			, _K_S_IO	,	RFID_PAGE14				,	0	} ,
	{ "$1.RFID_PAGE15"			, _K_S_IO	,	RFID_PAGE15				,	0	} ,
	{ "$1.RFID_PAGE16"			, _K_S_IO	,	RFID_PAGE16				,	0	} ,
	{ "$1.RFID_PAGE17"			, _K_S_IO	,	RFID_PAGE17				,	0	} ,

	{ "TMC_CASSCODE$2_CONTROL"	, _K_F_IO	,	RFID_CONTROL			,	0	} , 
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MID_Direct_Verification( int Port , int Count ) { 
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// count = 1이 될것이고, 즉 첫번째만 인정, 첫번째 DATA는 MID의 Data가 된다.
// MID = 1,2 Page이므로 16Byte로 구성이 될 것이다.
///////////////////
	int CommStatus , Result = SUCCESS;
	char MIDStr[256];
	if		( CASSETTE_INDEX == PORT1 ) READ_STRING( LP_MID_READ  , MIDStr , &CommStatus );
	else if ( CASSETTE_INDEX == PORT2 ) READ_STRING( LP_MID_READ2 , MIDStr , &CommStatus );
	else READ_STRING( LP_MID_READ3 , MIDStr , &CommStatus );
	if ( strcmp( DATA[0] , MIDStr ) == 0 ) Result = SUCCESS; 
	else Result = FAIL; 
	WRITE_DIGITAL( MID_Verify_Result , Result , &CommStatus ); // success = 0 , fail = 1
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Direct_Set( int Port , int Count ) { 
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// count = 1이 될것이고, 즉 첫번째만 인정, 첫번째 DATA는 MID의 Data가 된다.
// MID = 1,2 Page이므로 16Byte로 구성이 될 것이다.
///////////////////
	int CommStatus;
	if		( CASSETTE_INDEX == PORT1 ) {
		WRITE_STRING( LP_MID_READ  , DATA[0] , &CommStatus );
		WRITE_STRING( LP_MID_NAME  , DATA[0] , &CommStatus );
	}
	else if ( CASSETTE_INDEX == PORT2 ) {
		WRITE_STRING( LP_MID_READ2  , DATA[0] , &CommStatus );
		WRITE_STRING( LP_MID_NAME2  , DATA[0] , &CommStatus );
	}
	else {
		WRITE_STRING( LP_MID_READ3  , DATA[0] , &CommStatus );
		WRITE_STRING( LP_MID_NAME3  , DATA[0] , &CommStatus );
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Page_Verification( int Port , int Count ) { 
	int CommStatus;
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// ID는 Page 번호가 되고, DATA는 Page의 Data가 된다.
// 이는 Page 정보에 의해 Verify한다.
///////////////////
	WRITE_DIGITAL( MID_Verify_Result , SUCCESS , &CommStatus ); // success = 0 , fail = 1
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Page_Set( int Port , int Count ) { 
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// ID는 Page 번호가 되고, DATA는 Page의 Data가 된다.
// 이는 Page 정보에 의해 Verify한다.
///////////////////
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MID_Direct_Read_Action( int Port , int Count ) { // 실제 Read Action, Direct MID Read( not Data area )
//////////////////// Hear, Only User Code Fucntion
// MID_READ_COMPELTE Event가 존재 하는 fucntion을 Call하도록 한다.
// 실제 READ는 Call된 Fucntion에서 이루어 질 것이다.
// 이는 사실상 count가 0일 것이다. 즉 예로 (MID page)page 1,2를 읽도록 하는 Sub Fucntion을 call한다.
	char Buffer[256];
	int CommStatus;
	sprintf( Buffer , "RFID_READ 0" );
	if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
		sprintf( Buffer , EVENT_STRING_RFDATAREAD_FAIL , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		return SYS_ABORTED;
	}
	sprintf( Buffer , EVENT_STRING_RFMIDREAD_COMPT , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Page_Read_Action( int Port , int Count ) { // 실제 Read Action( MID, Data area 둘다 포함 )
//////////////////// Hear, Only User Code Fucntion
// MID_READ_COMPELTE Event가 존재 하는 fucntion을 Call하도록 한다.
// 실제 READ는 Call된 Fucntion에서 이루어 질 것이다.
// ID는 Page 번호가 되고, DATA는 Page의 Data가 된다. 그러나 Data는 없을 것이다. read이므로
// Count = 0이면 모든 Page를 read, count가 0이 아니면 지정된 Page만 Read
	char Buffer[256];
	int CommStatus , i , j , k , l , m , p , q;

	if ( Count == 0 ) {
		Count = 17;
		for ( i = 0 ; i < Count ; i++ ) { sprintf( ID[i] , "%d" , i + 1 ); sprintf( DATA[i] , "" ); }
	}
	for ( i = 0 ; i < Count ; i++ ) { // hear, DATA = Page Length Byte
		if ( strlen( DATA[i] ) <= 0 ) l = 1;
		else { 
			k = atoi( DATA[i] ); 
			if ( k <= 8 ) l = 1;
			else {
				p = k/8; q = k%8;
				if ( q != 0 ) l = p+1;
				else l = p;
			}
		}
		m = atoi( ID[i] );
		for ( j = 0 ; j < l && m+j >= 1 && m+j <= 17 ; j++ ) {
			sprintf( Buffer , "RFID_READ %d" , m+j );
			if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
				sprintf( Buffer , EVENT_STRING_RFDATAREAD_FAIL , CASSETTE_INDEX );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				return SYS_ABORTED;
			}
		}
	}
	sprintf( Buffer , EVENT_STRING_RFDATAREAD_COMPT , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Direct_Read_IO( int Port , int Count ) { // 단순히 설정된 IO로부터 값 읽기(실제 HW로 쓰는 것이 아님 )( MID, Data area 둘다 포함 )
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// 이는 사실상 count가 0일 것이다. 즉 예로 가상 (MID page)page 1,2에 대한 IO를 읽어서 string을 조합한다.
// 조합한 string을 Buffer에 채운다.
// 실패시 SYS_ABORTED return
///////////////////
	char Buffer[256] , Buffer2[256];
	int CommStatus;

	sprintf( Buffer , "RFID_READ 0" );
	if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
//		sprintf( Buffer , EVENT_STRING_RFDATAREAD_FAIL , CASSETTE_INDEX );
//		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		return SYS_ABORTED;
	}
//	sprintf( Buffer , EVENT_STRING_RFMIDREAD_COMPT , CASSETTE_INDEX );
//	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );

	sprintf( Buffer2 , "" );
	READ_STRING( RFID_MID_PAGE , Buffer2 , &CommStatus );
	WRITE_UPLOAD_MESSAGE( Buffer2 ); 
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Page_Read_IO( int Port , int Count ) { // 단순히 설정된 IO로부터 값 읽기(실제 HW로 쓰는 것이 아님 )( only MID area )
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// ID는 Page 번호가 되고, DATA는 Page의 Data가 된다. 그러나 Data는 없을 것이다. read이므로
// IO로부터 값을 읽는다. 읽은 값을 Buffer에 채운다.
// Count = 0이면 모든 Page를 read, count가 0이 아니면 지정된 Page만 Read
// 실패시 SYS_ABORTED return
///////////////////
//############# ymc
	char Buffer[256] , Buffer1[256] , Buffer2[256];
	int CommStatus , i , j , k , l , m , p , q;

	if ( Count == 0 ) {
		Count = 17;
		for ( i = 0 ; i < Count ; i++ ) { sprintf( ID[i] , "%d" , i + 1 ); sprintf( DATA[i] , "" ); }
	}
	for ( i = 0 ; i < Count ; i++ ) { // hear, DATA = Page Length Byte
		if ( strlen( DATA[i] ) <= 0 ) l = 1;
		else { 
			k = atoi( DATA[i] ); 
			if ( k <= 8 ) l = 1;
			else {
				p = k/8; q = k%8;
				if ( q != 0 ) l = p+1;
				else l = p;
			}
		}
		m = atoi( ID[i] );

		for ( j = 0 ; j < l && m+j >=1 && m+j <= 17 ; j++ ) {
			sprintf( Buffer , "RFID_READ %d" , m+j );
			if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
//				sprintf( Buffer , EVENT_STRING_RFDATAREAD_FAIL , CASSETTE_INDEX );
//				WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				return SYS_ABORTED;
			}
		}

		sprintf( Buffer2 , "" );
		for ( j = 0 ; j < l && m+j >= 1 && m+j <= 17 ; j++ ) {
			READ_STRING( RFID_PAGE1+m-1+j , Buffer1 , &CommStatus );
			strncat( Buffer2 , Buffer1 , 8 );
		}
		WRITE_UPLOAD_MESSAGE( Buffer2 ); 
	}
//	sprintf( Buffer , EVENT_STRING_RFDATAREAD_COMPT , CASSETTE_INDEX );
//	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MID_Direct_Write_Action( int Port , int Count ) {  // 실제 Action, Direct MID Write( not Data area )
//////////////////// Hear, Only User Code Fucntion
// MID_WRITE_COMPELTE Event가 존재 하는 fucntion을 Call하도록 한다.
// 실제 WRITE는 Call된 Fucntion에서 이루어 질 것이다.
// 이는 count가 1일 것이고 ID는 의미 없고 첫번째 Data가 MID의 string이 될것이다.
// 따라서 sub Function에 이 String을 전달하여 MID Page(1,2 page)에 데이터를 8Byte씩 분리하여 
// Write하도록 한다.
	char Buffer[256];
	int CommStatus;
	sprintf( Buffer , "RFID_WRITE 0 %s" , DATA[0] );
	if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
		sprintf( Buffer , EVENT_STRING_RFMIDWRITE_FAIL , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		return SYS_ABORTED;
	}
	sprintf( Buffer , EVENT_STRING_RFMIDWRITE_COMPT , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Page_Write_Action( int Port , int Count ) { // Page별로 Write한다, 실제 Action( MID, Data area 둘다 포함 )
//////////////////// Hear, Only User Code Fucntion
// 실제 WRITE는 Call된 Fucntion에서 이루어 질 것이다.
// ID는 Page 번호가 되고, DATA는 Page의 Data가 된다.
	char Buffer[256];
	int i , CommStatus;
	for ( i = 0 ; i < Count ; i++ ) {
		sprintf( Buffer , "RFID_WRITE %s %s" , ID[i] , DATA[i] );
		if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
			sprintf( Buffer , EVENT_STRING_RFDATAWRITE_FAIL , CASSETTE_INDEX );
			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			return SYS_ABORTED;
		}
	}
	sprintf( Buffer , EVENT_STRING_RFDATAWRITE_COMPT , CASSETTE_INDEX );
	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Direct_Write_IO( int Port , int Count ) { // only MID area
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// 이는 count가 1일 것이고 ID는 의미 없고 첫번째 Data가 MID의 string이 될것이다.
// 따라서 string을 8Byte로 분리하여 가상 Write IO에 Write, 가상 MID Page(1,2 page)
// 단순히 IO에 설정만 한다. 이 때 IO는 시간이 걸리는 실제 HW IO이면 안된다.
// 실패시 SYS_ABORTED return
///////////////////
	char Buffer[256];
//	int CommStatus;
	sprintf( Buffer , "RFID_WRITE 0 %s" , DATA[0] );
	if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
//		sprintf( Buffer , EVENT_STRING_RFMIDWRITE_FAIL , CASSETTE_INDEX );
//		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		return SYS_ABORTED;
	}
//	sprintf( Buffer , EVENT_STRING_RFMIDWRITE_COMPT , CASSETTE_INDEX );
//	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Page_Write_IO( int Port , int Count ) { // Page별로 Write한다( MID, Data area 둘다 포함 )
//////////////////// Hear, Only User Code Fucntion
// 지연 없이 곧바로 리턴이 이루어 져야 한다.
// ID는 Page 번호가 되고, DATA는 Page의 Data가 된다.
// 단순히 IO에 설정만 한다. 이 때 IO는 시간이 걸리는 실제 HW IO이면 안된다.
// 실패시 SYS_ABORTED return
///////////////////
	char Buffer[256];
	int i;
//	int CommStatus;
	for ( i = 0 ; i < Count ; i++ ) {
		sprintf( Buffer , "RFID_WRITE %s %s" , ID[i] , DATA[i] );
		if ( RUN_FUNCTION( RFID_CONTROL , Buffer ) == SYS_ABORTED ) {
//			sprintf( Buffer , EVENT_STRING_RFDATAWRITE_FAIL , CASSETTE_INDEX );
//			WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
			return SYS_ABORTED;
		}
	}
//	sprintf( Buffer , EVENT_STRING_RFDATAWRITE_COMPT , CASSETTE_INDEX );
//	WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	Dl , Port , Count , i;
	unsigned char *Ed;
	char RunStr[TOTAL_MAX_PAGE*256+1] , Data[TOTAL_MAX_PAGE*256+1];
	char Other[TOTAL_MAX_PAGE*256+1] , Other1[TOTAL_MAX_PAGE*256+1];
	char Other2[TOTAL_MAX_PAGE*256+1];
	char Buffer[256] , Buffer1[256];
	int CommStatus , RfMode;

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( Dl < 1 ) {
		printf( "=====> %s RFID Manager Parameter Error\n" , PortName );
		return SYS_ABORTED;
	}

	if ( CASSETTE_INDEX != PORT1 && CASSETTE_INDEX != PORT2 ) { // Port1/2
		printf( "=====> %s RFID Manager Unknown PortID : %d\n" , PortName , CASSETTE_INDEX );
		return SYS_ABORTED;
	}

	for ( i = 0 ; i < MAX_PAGE ; i++ ) { 
		sprintf( ID[i] , "" ); sprintf( DATA[i] , "" );
	}

	STR_SEPERATE( Ed , RunStr , Data , TOTAL_MAX_PAGE*256 );
	RfMode = READ_DIGITAL( LP_RFID_USE , &CommStatus );
	if ( RfMode == OFF2 ) { // Not Use
		sprintf( Buffer , EVENT_STRING_RFID_NOUSE , CASSETTE_INDEX );
		WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
		if		( STRCMP_L( RunStr , "MID_VERIFY" ) ) {
			WRITE_DIGITAL( MID_Verify_Result , SUCCESS , &CommStatus ); // success = 0 , fail = 1
			return SYS_SUCCESS;
		}
		else if ( STRCMP_L( RunStr , "MID_SET" ) ) {
			return SYS_SUCCESS;
		}
		return SYS_ABORTED;
	}

	// Command PortID|Count|PageID:Data|PageID2:Data2|... max : 20
//	STR_SEPERATE( Ed , RunStr , Data , TOTAL_MAX_PAGE*256 );
	STR_SEPERATE_CHAR( Data	, '|' , Other , Other1 , TOTAL_MAX_PAGE*256 );
	Port = atoi( Other ); 
// Port는 사용하지 않기 때문에 
//	if ( Port != CM1 && Port != CM2 ) { // Port1/2
//		printf( "=====> %s RFID Manager Unknown PortNum : %d\n" , PortName , Port );
//		return SYS_ABORTED;
//	}
	STR_SEPERATE_CHAR( Other1	, '|' , Other , Other2 , TOTAL_MAX_PAGE*256 );
	Count = atoi( Other );
	if		( Count > 20 ) Count = 20;
	else if ( Count < 0  ) Count = 0;
	for ( i = 0 ; i < Count ; i++ ) {
		STR_SEPERATE_CHAR( Other2	, '|' , Other  , Other1  , TOTAL_MAX_PAGE*256 );
		STR_SEPERATE_CHAR( Other	, ':' , Buffer , Buffer1 , 255    );
		strncpy( ID[i] , Buffer , 255 ); strncpy( DATA[i] , Buffer1 , 255 );
		strcpy( Other2 , Other1 );
	}

	// read/write type : 이미 정해진 MID에 대해서 Read/Write, 예로 1,2 Page가 MID로 정해 졌을 때 
	//                   받은 데이터를 무조건 1,2page에 쓰고, 1,2 Page에서 무조건 읽는다. 실제 HW적인 action이다.
	// read2/write2 type : 특정 page에 대해 read/write한다 실제 HW적인 action이다.
	// read3/write3 type : 이미 정해진 MID에 대해서 실제 HW IO가 아닌 더미 즉 단순히 이미 읽은 정보를 가지고 있는 IO를 통해 read/write
	// read4/write4 type : 특정 page에 대해 read/write한다 단순히 실제 HW적인 IO가 아닌 더미 IO의 값에 read/write
	if      ( STRCMP_L( RunStr , "MID_VERIFY"	) ) return MID_Direct_Verification( Port , Count );
	else if ( STRCMP_L( RunStr , "MID_SET"		) ) return MID_Direct_Set( Port , Count );
	else if ( STRCMP_L( RunStr , "PAGE_VERIFY"	) ) return MID_Page_Verification( Port , Count );
	else if ( STRCMP_L( RunStr , "PAGE_SET"		) ) return MID_Page_Set( Port , Count );
	else if ( STRCMP_L( RunStr , "MID_READ"		) ) return MID_Direct_Read_Action( Port , Count );
	else if ( STRCMP_L( RunStr , "MID_WRITE"	) ) return MID_Direct_Write_Action( Port , Count );
	else if ( STRCMP_L( RunStr , "PAGE_READ"	) ) return MID_Page_Read_Action( Port , Count );
	else if ( STRCMP_L( RunStr , "PAGE_WRITE"	) ) return MID_Page_Write_Action( Port , Count );
	else if ( STRCMP_L( RunStr , "MID_READIO"	) ) return MID_Direct_Read_IO( Port , Count );
	else if ( STRCMP_L( RunStr , "MID_WRITEIO"	) ) return MID_Direct_Write_IO( Port , Count );
	else if ( STRCMP_L( RunStr , "PAGE_READIO"	) ) return MID_Page_Read_IO( Port , Count );
	else if ( STRCMP_L( RunStr , "PAGE_WRITEIO"	) ) return MID_Page_Write_IO( Port , Count );
	else {
		printf( "=====> %s RFID Manager Check Error - Not Support Code %s\n" , PortName , RunStr );
		return SYS_ABORTED;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char Str1[21] , Str2[21] , Str3[21];
	int	Dl;
	unsigned char *Ed;

	Ed = PROGRAM_ARGUMENT_READ(); // GEM.MID1 CM1 PORT1
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	STR_SEPERATE( Ed , Str1 , Str2 , 20 ); // GEM.MID1
	REPLACE_CHAR_SET( 0 , Str1 );
	STR_SEPERATE( Str2 , Str1 , Str3 , 20 ); // CM1
	REPLACE_CHAR_SET( 1 , Str1 );
	strncpy( PortName , Str1 , 255 );
	CASSETTE_INDEX = atoi( Str1+2 );
	STR_SEPERATE( Str3 , Str1 , Str2 , 20 ); // PORT1
	REPLACE_CHAR_SET( 2 , Str1 );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == SIM ) SIMULATION_MODE = TRUE;	
	else SIMULATION_MODE = FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
