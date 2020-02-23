#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include "cimseqnc.h"
#include "iodefine.h"
#include "resource.h"
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

enum { OFF_0	,	ON_1	};  

char FileName[64] ={0,}, OldFileName[64] = {0,};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	
	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER				,	0	} ,
	{ "EQ_NAME"					, _K_S_IO	,	EQ_NAME					,	0	} ,	
	{ "INFO_MFCPATH_NAME"		, _K_S_IO	,	INFO_MFCPATH_NAME		,	0	} ,	

	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void File_Log( char* str , char* str2 , char* str3 , char* str4 , char* str5 , char* str6, char* str7, char* str8, char* str9, char* str10, char* str11 , char* str12 , char* str13 , char* str14 ) 
{
	FILE *fp = NULL;
	char Path[MAX_PATH] = {0,};
	char EQ_Name[128] = {0,};
	int CommStatus ;
	SYSTEMTIME st;

	READ_STRING ( EQ_NAME  , EQ_Name  , &CommStatus );

	GetLocalTime( &st );

	sprintf( FileName , "%04d-%02d-%02d_%s_PartInfo.log" , st.wYear , st.wMonth, st.wDay , EQ_Name );
	sprintf( Path , "F:\\Mars\\PartsLog\\%s" ,FileName );
	
	WRITE_STRING ( INFO_MFCPATH_NAME , Path , &CommStatus ) ;

	fp = fopen( Path , "a+" );
	if( !fp ) 
	{
		CreateDirectory ( Path , NULL );
		fp = fopen( Path , "a+" );
		if( !fp ) 
		{
			printf( "[MFC_Log] : Can't Create Log File\n"  );
			return; 
		}			
	   
	}
	fprintf( fp , "[%04d-%02d-%02d %02d:%02d:%02d.%03d]" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond, st.wMilliseconds );
	fprintf( fp , "%-10s%-10s%-20s%-20s%-20s%-20s%-10s%-10s%-20s%-20s%-20s%-20s%-20s%-20s\n" , str ,str3,str2,str4,str5,str6,str7,str8,str9,str10,str11,str14,str13,str12 );
	fclose( fp );
 }


//	MFC_MAC_ID[i],  MFC_Serial[i], MFC_Model[i],  MFC_MF[i], MFC_SoftRev[i], MFC_HardRev[i] ,MFC_Name[i], MFC_StandardNo[i] , MFC_MAXSize[i], MFC_OffsetB[i] , MFC_OffsetA[i],  MFC_Gain[i]);
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void MFC_LOG_Event_PMA ( char *Para ) {

	char Str1[5000] ;
	char MFC_MAC_ID[10] ;
	char Serial_Info[40] , Name_Info[20] , MAXSize_Info[20] , Model_Info[40] , MF_Info[40] ; 
	char SoftRev_Info[10] , HardRev_Info[10] , StandardNo_Info[20] , OffsetA_Info[20] , OffsetB_Info[20] , Gain_Info[20] ; 
	char ModuleName[10];
	char PartName[20];

	memset(Str1			, 0x00, sizeof(Str1) );
	memset(Serial_Info	, 0x00, sizeof(Serial_Info) );
	memset(Name_Info	, 0x00, sizeof(Name_Info));
	memset(MAXSize_Info	, 0x00, sizeof(MAXSize_Info));
	memset(Model_Info	, 0x00, sizeof(Model_Info));
	memset(MF_Info		, 0x00, sizeof(MF_Info));
	memset(SoftRev_Info		, 0x00, sizeof(SoftRev_Info));
	memset(HardRev_Info		, 0x00, sizeof(HardRev_Info));
	memset(StandardNo_Info	, 0x00, sizeof(StandardNo_Info));
	memset(OffsetA_Info		, 0x00, sizeof(OffsetA_Info));
	memset(OffsetB_Info		, 0x00, sizeof(OffsetB_Info));
	memset(Gain_Info		, 0x00, sizeof(Gain_Info));


//				                         MFC_MAC_ID[i],  MFC_Serial[i], MFC_Model[i],  MFC_MF[i], MFC_SoftRev[i], MFC_HardRev[i] ,MFC_Name[i], MFC_StandardNo[i] , MFC_MAXSize[i], MFC_OffsetB[i] , MFC_OffsetA[i],  MFC_Gain[i]);
// sprintf( UserEventMessage , "MFC_INFO M%s,            %s,            %s,             %s,       %s,             %s,             %s,          %s,                 %s,             %s,              %s,              %s" , 


		STR_SEPERATE_CHAR( Para , ';' , MFC_MAC_ID	 , Str1 , 4999 ); // Serial
		STR_SEPERATE_CHAR( Str1 , ';' , Serial_Info	 , Str1 , 4999 ); // Serial
		STR_SEPERATE_CHAR( Str1 , ';' , Model_Info	 , Str1 , 4999 ); // ModelNo
		STR_SEPERATE_CHAR( Str1 , ';' , MF_Info		 , Str1 , 4999 ); // Manufacturer
		STR_SEPERATE_CHAR( Str1 , ';' , SoftRev_Info		 , Str1 , 4999 ); // software Rev 
		STR_SEPERATE_CHAR( Str1 , ';' , HardRev_Info		 , Str1 , 4999 ); // Hardware Rev
		STR_SEPERATE_CHAR( Str1 , ';' , Name_Info	 , Str1 , 4999 ); // GASName
		STR_SEPERATE_CHAR( Str1 , ';' , StandardNo_Info		 , Str1 , 4999 ); //  StandarNo
		STR_SEPERATE_CHAR( Str1 , ';' , MAXSize_Info , Str1 , 4999 ); // MaxSize
		STR_SEPERATE_CHAR( Str1 , ';' , OffsetB_Info		 , Str1 , 4999 ); // OffsetB
		STR_SEPERATE_CHAR( Str1 , ';' , OffsetA_Info		 , Str1 , 4999 ); // OffsetA 
		STR_SEPERATE_CHAR( Str1 , ';' , Gain_Info		 , Str1 , 4999 ); // Gain

		sprintf( ModuleName , "PM1" );
		sprintf( PartName , "MFC" );

		File_Log( ModuleName, PartName , MFC_MAC_ID , Serial_Info , Model_Info , MF_Info , SoftRev_Info , HardRev_Info ,Name_Info ,StandardNo_Info ,MAXSize_Info ,OffsetB_Info ,OffsetA_Info, Gain_Info ) ;
	

}
//------------------------------------------------------------------------------------------
void MFC_LOG_Event_PMB ( char *Para ) {

	char Str1[5000] ;
	char MFC_MAC_ID[10] ;
	char Serial_Info[40] , Name_Info[20] , MAXSize_Info[20] , Model_Info[40] , MF_Info[40] ; 
	char SoftRev_Info[10] , HardRev_Info[10] , StandardNo_Info[20] , OffsetA_Info[20] , OffsetB_Info[20] , Gain_Info[20] ; 
	char ModuleName[10];
	char PartName[20];

	memset(Str1			, 0x00, sizeof(Str1) );
	memset(Serial_Info	, 0x00, sizeof(Serial_Info) );
	memset(Name_Info	, 0x00, sizeof(Name_Info));
	memset(MAXSize_Info	, 0x00, sizeof(MAXSize_Info));
	memset(Model_Info	, 0x00, sizeof(Model_Info));
	memset(MF_Info		, 0x00, sizeof(MF_Info));
	memset(SoftRev_Info		, 0x00, sizeof(SoftRev_Info));
	memset(HardRev_Info		, 0x00, sizeof(HardRev_Info));
	memset(StandardNo_Info		, 0x00, sizeof(StandardNo_Info));
	memset(OffsetA_Info		, 0x00, sizeof(OffsetA_Info));
	memset(OffsetB_Info		, 0x00, sizeof(OffsetB_Info));
	memset(Gain_Info		, 0x00, sizeof(Gain_Info));


//				                         MFC_MAC_ID[i],  MFC_Serial[i], MFC_Model[i],  MFC_MF[i], MFC_SoftRev[i], MFC_HardRev[i] ,MFC_Name[i], MFC_StandardNo[i] , MFC_MAXSize[i], MFC_OffsetB[i] , MFC_OffsetA[i],  MFC_Gain[i]);
// sprintf( UserEventMessage , "MFC_INFO M%s,            %s,            %s,             %s,       %s,             %s,             %s,          %s,                 %s,             %s,              %s,              %s" , 


		STR_SEPERATE_CHAR( Para , ';' , MFC_MAC_ID	 , Str1 , 4999 ); // Serial
		STR_SEPERATE_CHAR( Str1 , ';' , Serial_Info	 , Str1 , 4999 ); // Serial
		STR_SEPERATE_CHAR( Str1 , ';' , Model_Info	 , Str1 , 4999 ); // ModelNo
		STR_SEPERATE_CHAR( Str1 , ';' , MF_Info		 , Str1 , 4999 ); // Manufacturer
		STR_SEPERATE_CHAR( Str1 , ';' , SoftRev_Info		 , Str1 , 4999 ); // software Rev 
		STR_SEPERATE_CHAR( Str1 , ';' , HardRev_Info		 , Str1 , 4999 ); // Hardware Rev
		STR_SEPERATE_CHAR( Str1 , ';' , Name_Info	 , Str1 , 4999 ); // GASName
		STR_SEPERATE_CHAR( Str1 , ';' , StandardNo_Info		 , Str1 , 4999 ); //  StandarNo
		STR_SEPERATE_CHAR( Str1 , ';' , MAXSize_Info , Str1 , 4999 ); // MaxSize
		STR_SEPERATE_CHAR( Str1 , ';' , OffsetB_Info		 , Str1 , 4999 ); // OffsetB
		STR_SEPERATE_CHAR( Str1 , ';' , OffsetA_Info		 , Str1 , 4999 ); // OffsetA 
		STR_SEPERATE_CHAR( Str1 , ';' , Gain_Info		 , Str1 , 4999 ); // Gain

		sprintf( ModuleName , "PM2" );
		sprintf( PartName , "MFC" );

		File_Log( ModuleName, PartName , MFC_MAC_ID , Serial_Info , Model_Info , MF_Info , SoftRev_Info , HardRev_Info ,Name_Info ,StandardNo_Info ,MAXSize_Info ,OffsetB_Info ,OffsetA_Info, Gain_Info ) ;
	

}
//------------------------------------------------------------------------------------------
void MFC_LOG_Event_PMC ( char *Para ) {

	char Str1[5000] ;
	char MFC_MAC_ID[10] ;
	char Serial_Info[40] , Name_Info[20] , MAXSize_Info[20] , Model_Info[40] , MF_Info[40] ; 
	char SoftRev_Info[10] , HardRev_Info[10] , StandardNo_Info[20] , OffsetA_Info[20] , OffsetB_Info[20] , Gain_Info[20] ; 
	char ModuleName[10];
	char PartName[20];

	memset(Str1			, 0x00, sizeof(Str1) );
	memset(Serial_Info	, 0x00, sizeof(Serial_Info) );
	memset(Name_Info	, 0x00, sizeof(Name_Info));
	memset(MAXSize_Info	, 0x00, sizeof(MAXSize_Info));
	memset(Model_Info	, 0x00, sizeof(Model_Info));
	memset(MF_Info		, 0x00, sizeof(MF_Info));
	memset(SoftRev_Info		, 0x00, sizeof(SoftRev_Info));
	memset(HardRev_Info		, 0x00, sizeof(HardRev_Info));
	memset(StandardNo_Info		, 0x00, sizeof(StandardNo_Info));
	memset(OffsetA_Info		, 0x00, sizeof(OffsetA_Info));
	memset(OffsetB_Info		, 0x00, sizeof(OffsetB_Info));
	memset(Gain_Info		, 0x00, sizeof(Gain_Info));


//				                         MFC_MAC_ID[i],  MFC_Serial[i], MFC_Model[i],  MFC_MF[i], MFC_SoftRev[i], MFC_HardRev[i] ,MFC_Name[i], MFC_StandardNo[i] , MFC_MAXSize[i], MFC_OffsetB[i] , MFC_OffsetA[i],  MFC_Gain[i]);
// sprintf( UserEventMessage , "MFC_INFO M%s,            %s,            %s,             %s,       %s,             %s,             %s,          %s,                 %s,             %s,              %s,              %s" , 


		STR_SEPERATE_CHAR( Para , ';' , MFC_MAC_ID	 , Str1 , 4999 ); // Serial
		STR_SEPERATE_CHAR( Str1 , ';' , Serial_Info	 , Str1 , 4999 ); // Serial
		STR_SEPERATE_CHAR( Str1 , ';' , Model_Info	 , Str1 , 4999 ); // ModelNo
		STR_SEPERATE_CHAR( Str1 , ';' , MF_Info		 , Str1 , 4999 ); // Manufacturer
		STR_SEPERATE_CHAR( Str1 , ';' , SoftRev_Info		 , Str1 , 4999 ); // software Rev 
		STR_SEPERATE_CHAR( Str1 , ';' , HardRev_Info		 , Str1 , 4999 ); // Hardware Rev
		STR_SEPERATE_CHAR( Str1 , ';' , Name_Info	 , Str1 , 4999 ); // GASName
		STR_SEPERATE_CHAR( Str1 , ';' , StandardNo_Info		 , Str1 , 4999 ); //  StandarNo
		STR_SEPERATE_CHAR( Str1 , ';' , MAXSize_Info , Str1 , 4999 ); // MaxSize
		STR_SEPERATE_CHAR( Str1 , ';' , OffsetB_Info		 , Str1 , 4999 ); // OffsetB
		STR_SEPERATE_CHAR( Str1 , ';' , OffsetA_Info		 , Str1 , 4999 ); // OffsetA 
		STR_SEPERATE_CHAR( Str1 , ';' , Gain_Info		 , Str1 , 4999 ); // Gain

		sprintf( ModuleName , "PM3" );
		sprintf( PartName , "MFC" );

		File_Log( ModuleName, PartName , MFC_MAC_ID , Serial_Info , Model_Info , MF_Info , SoftRev_Info , HardRev_Info ,Name_Info ,StandardNo_Info ,MAXSize_Info ,OffsetB_Info ,OffsetA_Info, Gain_Info ) ;
	


}
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {

}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
	char  ParamStr[1024] , RunStr[1024] , str[128], str1[128] ;
	
	memset( ParamStr , 0x00 , sizeof(ParamStr) );
	memset( RunStr , 0x00 , sizeof(RunStr) );
	memset( str , 0x00 , sizeof(str) );
	memset( str1 , 0x00 , sizeof(str1) );


	if ( !KWIN_DIALOG_DRIVER_LOADING() ) MessageBeep(100);

	STR_SEPERATE( PROGRAM_ARGUMENT_READ() , str , str1 , 31 ); 

	STR_SEPERATE( PROGRAM_EVENT_READ() , RunStr , ParamStr ,4999 ); 
	
	if ( STRCMP_L ( str , "PM1" ) ){
		if ( STRCMP_L ( RunStr , "MFC_INFO" ) ) MFC_LOG_Event_PMA ( ParamStr );
	}
	else if ( STRCMP_L ( str , "PM2" ) ){ 
		 if ( STRCMP_L ( RunStr , "MFC_INFO" ) ) MFC_LOG_Event_PMB ( ParamStr );
	}
	else if ( STRCMP_L ( str , "PM3" ) ){ 
		if ( STRCMP_L ( RunStr , "MFC_INFO" ) ) MFC_LOG_Event_PMC ( ParamStr );
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
