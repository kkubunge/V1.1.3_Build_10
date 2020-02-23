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
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// count = 1�� �ɰ��̰�, �� ù��°�� ����, ù��° DATA�� MID�� Data�� �ȴ�.
// MID = 1,2 Page�̹Ƿ� 16Byte�� ������ �� ���̴�.
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
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// count = 1�� �ɰ��̰�, �� ù��°�� ����, ù��° DATA�� MID�� Data�� �ȴ�.
// MID = 1,2 Page�̹Ƿ� 16Byte�� ������ �� ���̴�.
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
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// ID�� Page ��ȣ�� �ǰ�, DATA�� Page�� Data�� �ȴ�.
// �̴� Page ������ ���� Verify�Ѵ�.
///////////////////
	WRITE_DIGITAL( MID_Verify_Result , SUCCESS , &CommStatus ); // success = 0 , fail = 1
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
Module_Status MID_Page_Set( int Port , int Count ) { 
//////////////////// Hear, Only User Code Fucntion
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// ID�� Page ��ȣ�� �ǰ�, DATA�� Page�� Data�� �ȴ�.
// �̴� Page ������ ���� Verify�Ѵ�.
///////////////////
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status MID_Direct_Read_Action( int Port , int Count ) { // ���� Read Action, Direct MID Read( not Data area )
//////////////////// Hear, Only User Code Fucntion
// MID_READ_COMPELTE Event�� ���� �ϴ� fucntion�� Call�ϵ��� �Ѵ�.
// ���� READ�� Call�� Fucntion���� �̷�� �� ���̴�.
// �̴� ��ǻ� count�� 0�� ���̴�. �� ���� (MID page)page 1,2�� �е��� �ϴ� Sub Fucntion�� call�Ѵ�.
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
Module_Status MID_Page_Read_Action( int Port , int Count ) { // ���� Read Action( MID, Data area �Ѵ� ���� )
//////////////////// Hear, Only User Code Fucntion
// MID_READ_COMPELTE Event�� ���� �ϴ� fucntion�� Call�ϵ��� �Ѵ�.
// ���� READ�� Call�� Fucntion���� �̷�� �� ���̴�.
// ID�� Page ��ȣ�� �ǰ�, DATA�� Page�� Data�� �ȴ�. �׷��� Data�� ���� ���̴�. read�̹Ƿ�
// Count = 0�̸� ��� Page�� read, count�� 0�� �ƴϸ� ������ Page�� Read
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
Module_Status MID_Direct_Read_IO( int Port , int Count ) { // �ܼ��� ������ IO�κ��� �� �б�(���� HW�� ���� ���� �ƴ� )( MID, Data area �Ѵ� ���� )
//////////////////// Hear, Only User Code Fucntion
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// �̴� ��ǻ� count�� 0�� ���̴�. �� ���� ���� (MID page)page 1,2�� ���� IO�� �о string�� �����Ѵ�.
// ������ string�� Buffer�� ä���.
// ���н� SYS_ABORTED return
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
Module_Status MID_Page_Read_IO( int Port , int Count ) { // �ܼ��� ������ IO�κ��� �� �б�(���� HW�� ���� ���� �ƴ� )( only MID area )
//////////////////// Hear, Only User Code Fucntion
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// ID�� Page ��ȣ�� �ǰ�, DATA�� Page�� Data�� �ȴ�. �׷��� Data�� ���� ���̴�. read�̹Ƿ�
// IO�κ��� ���� �д´�. ���� ���� Buffer�� ä���.
// Count = 0�̸� ��� Page�� read, count�� 0�� �ƴϸ� ������ Page�� Read
// ���н� SYS_ABORTED return
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
Module_Status MID_Direct_Write_Action( int Port , int Count ) {  // ���� Action, Direct MID Write( not Data area )
//////////////////// Hear, Only User Code Fucntion
// MID_WRITE_COMPELTE Event�� ���� �ϴ� fucntion�� Call�ϵ��� �Ѵ�.
// ���� WRITE�� Call�� Fucntion���� �̷�� �� ���̴�.
// �̴� count�� 1�� ���̰� ID�� �ǹ� ���� ù��° Data�� MID�� string�� �ɰ��̴�.
// ���� sub Function�� �� String�� �����Ͽ� MID Page(1,2 page)�� �����͸� 8Byte�� �и��Ͽ� 
// Write�ϵ��� �Ѵ�.
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
Module_Status MID_Page_Write_Action( int Port , int Count ) { // Page���� Write�Ѵ�, ���� Action( MID, Data area �Ѵ� ���� )
//////////////////// Hear, Only User Code Fucntion
// ���� WRITE�� Call�� Fucntion���� �̷�� �� ���̴�.
// ID�� Page ��ȣ�� �ǰ�, DATA�� Page�� Data�� �ȴ�.
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
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// �̴� count�� 1�� ���̰� ID�� �ǹ� ���� ù��° Data�� MID�� string�� �ɰ��̴�.
// ���� string�� 8Byte�� �и��Ͽ� ���� Write IO�� Write, ���� MID Page(1,2 page)
// �ܼ��� IO�� ������ �Ѵ�. �� �� IO�� �ð��� �ɸ��� ���� HW IO�̸� �ȵȴ�.
// ���н� SYS_ABORTED return
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
Module_Status MID_Page_Write_IO( int Port , int Count ) { // Page���� Write�Ѵ�( MID, Data area �Ѵ� ���� )
//////////////////// Hear, Only User Code Fucntion
// ���� ���� ��ٷ� ������ �̷�� ���� �Ѵ�.
// ID�� Page ��ȣ�� �ǰ�, DATA�� Page�� Data�� �ȴ�.
// �ܼ��� IO�� ������ �Ѵ�. �� �� IO�� �ð��� �ɸ��� ���� HW IO�̸� �ȵȴ�.
// ���н� SYS_ABORTED return
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
// Port�� ������� �ʱ� ������ 
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

	// read/write type : �̹� ������ MID�� ���ؼ� Read/Write, ���� 1,2 Page�� MID�� ���� ���� �� 
	//                   ���� �����͸� ������ 1,2page�� ����, 1,2 Page���� ������ �д´�. ���� HW���� action�̴�.
	// read2/write2 type : Ư�� page�� ���� read/write�Ѵ� ���� HW���� action�̴�.
	// read3/write3 type : �̹� ������ MID�� ���ؼ� ���� HW IO�� �ƴ� ���� �� �ܼ��� �̹� ���� ������ ������ �ִ� IO�� ���� read/write
	// read4/write4 type : Ư�� page�� ���� read/write�Ѵ� �ܼ��� ���� HW���� IO�� �ƴ� ���� IO�� ���� read/write
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
