#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <conio.h>
//#include <afxcmn.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
#include "kutletc.h"
#include "kutlanal.h"
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "CimSeqncKNFS.h"
#include <math.h>
//------------------------------------------------------------------------------------------
#define MFC_MAX_COUNT 9
#define	ALARM_GAS_INFO_DIFF			2183
#define	ALARM_MFC_UPDATE_IO_FAIL	2184

int	g_ModuleInfo = 0, MFC_COUNT = 9;

char MFC_BASE[MFC_MAX_COUNT][64];	
char MFC_NAME_SIZE_CHK[MFC_MAX_COUNT][64];
char MFC_OLD_NAME[MFC_MAX_COUNT][64];
char MFC_OLD_MAXSIZE[MFC_MAX_COUNT][64];
char MFC_Name[MFC_MAX_COUNT][20]; 
char MFC_MAXSize[MFC_MAX_COUNT][20];

BOOL g_bMFCUpdateOk = FALSE;

//------------------------------------------------------------------------------------------
enum { OFF_0	,	ON_1	};  
enum { STOP_0   ,   START_1	};  
enum { Unuse, Local, DisConn, Standby, Maint, Uninit, EnablePM, CTCinUSE, CTCDis, CTCDis2, CTCEnaPM, CTCDisHW, CTCDIsHW2 } ; 

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL	SIMULATION_MODE	= FALSE;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
		
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_DRIVER_LOAD		,	0	} ,
	{ "ModuleNo"				, _K_D_IO	,	ModuleNo		,	0	} ,
	{ "SW_GbIntlckDM"			, _K_D_IO	,	SW_GbIntlckDM	,	0	} ,
	{ "MFCInfoComDO"			, _K_D_IO	,	MFCInfoComDO	,	0	} ,
	{ "MFCDataScenDI"			, _K_D_IO	,	MFCDataScenDI	,	0	} ,
	{ "PRMA_TO_MFCScan"			, _K_A_IO	,	PRMA_TO_MFCScan	,	0	} ,
	{ "PRMA_TO_MFCScanRty"		, _K_A_IO	,	PRMA_TO_MFCScanRty,	0	} ,
	{ "DN_MFCInfoRead_DO"		, _K_D_IO	,	DN_MFCInfoRead_DO ,	0	} ,
	{ "PRMD_MFC_Diagnosis"		, _K_D_IO	,	PRMD_MFC_Diagnosis,	0	} ,

	{ "MFC01_SerialNumSI"		, _K_S_IO	, 	MFC01_SerialNumSI 	,	0	} ,
	{ "MFC02_SerialNumSI"		, _K_S_IO	, 	MFC02_SerialNumSI	,	0	} ,
	{ "MFC03_SerialNumSI"		, _K_S_IO	, 	MFC03_SerialNumSI	,	0	} ,
	{ "MFC04_SerialNumSI"		, _K_S_IO	, 	MFC04_SerialNumSI	,	0	} ,
	{ "MFC05_SerialNumSI"		, _K_S_IO	, 	MFC05_SerialNumSI	,	0	} ,
	{ "MFC06_SerialNumSI"		, _K_S_IO	, 	MFC06_SerialNumSI	,	0	} ,
	{ "MFC07_SerialNumSI"		, _K_S_IO	, 	MFC07_SerialNumSI	,	0	} ,
	{ "MFC08_SerialNumSI"		, _K_S_IO	, 	MFC08_SerialNumSI	,	0	} ,
	{ "MFC09_SerialNumSI"		, _K_S_IO	, 	MFC09_SerialNumSI	,	0	} ,

	{ "MFC01_ManuFacSI"		, _K_S_IO	, 	MFC01_ManuFacSI 	,	0	} ,
	{ "MFC02_ManuFacSI"		, _K_S_IO	, 	MFC02_ManuFacSI		,	0	} ,
	{ "MFC03_ManuFacSI"		, _K_S_IO	, 	MFC03_ManuFacSI		,	0	} ,
	{ "MFC04_ManuFacSI"		, _K_S_IO	, 	MFC04_ManuFacSI		,	0	} ,
	{ "MFC05_ManuFacSI"		, _K_S_IO	, 	MFC05_ManuFacSI		,	0	} ,
	{ "MFC06_ManuFacSI"		, _K_S_IO	, 	MFC06_ManuFacSI		,	0	} ,
	{ "MFC07_ManuFacSI"		, _K_S_IO	, 	MFC07_ManuFacSI		,	0	} ,
	{ "MFC08_ManuFacSI"		, _K_S_IO	, 	MFC08_ManuFacSI		,	0	} ,
	{ "MFC09_ManuFacSI"		, _K_S_IO	, 	MFC09_ManuFacSI		,	0	} ,

	{ "MFC01_ModelNumSI"		, _K_S_IO	, 	MFC01_ModelNumSI	,	0	} ,
	{ "MFC02_ModelNumSI"		, _K_S_IO	, 	MFC02_ModelNumSI	,	0	} ,
	{ "MFC03_ModelNumSI"		, _K_S_IO	, 	MFC03_ModelNumSI	,	0	} ,
	{ "MFC04_ModelNumSI"		, _K_S_IO	, 	MFC04_ModelNumSI	,	0	} ,
	{ "MFC05_ModelNumSI"		, _K_S_IO	, 	MFC05_ModelNumSI	,	0	} ,
	{ "MFC06_ModelNumSI"		, _K_S_IO	, 	MFC06_ModelNumSI	,	0	} ,
	{ "MFC07_ModelNumSI"		, _K_S_IO	, 	MFC07_ModelNumSI	,	0	} ,
	{ "MFC08_ModelNumSI"		, _K_S_IO	, 	MFC08_ModelNumSI	,	0	} ,
	{ "MFC09_ModelNumSI"		, _K_S_IO	, 	MFC09_ModelNumSI	,	0	} ,

	{ "MFC01_GasSymbolSI"		, _K_S_IO	, 	MFC01_GasSymbolSI	,	0	} ,
	{ "MFC02_GasSymbolSI"		, _K_S_IO	, 	MFC02_GasSymbolSI	,	0	} ,
	{ "MFC03_GasSymbolSI"		, _K_S_IO	, 	MFC03_GasSymbolSI	,	0	} ,
	{ "MFC04_GasSymbolSI"		, _K_S_IO	, 	MFC04_GasSymbolSI	,	0	} ,
	{ "MFC05_GasSymbolSI"		, _K_S_IO	, 	MFC05_GasSymbolSI	,	0	} ,
	{ "MFC06_GasSymbolSI"		, _K_S_IO	, 	MFC06_GasSymbolSI	,	0	} ,
	{ "MFC07_GasSymbolSI"		, _K_S_IO	, 	MFC07_GasSymbolSI	,	0	} ,
	{ "MFC08_GasSymbolSI"		, _K_S_IO	, 	MFC08_GasSymbolSI	,	0	} ,
	{ "MFC09_GasSymbolSI"		, _K_S_IO	, 	MFC09_GasSymbolSI	,	0	} ,

	{ "MFC01_FullScaleSI"		, _K_S_IO	, 	MFC01_FullScaleSI	,	0	} ,
	{ "MFC02_FullScaleSI"		, _K_S_IO	, 	MFC02_FullScaleSI	,	0	} ,
	{ "MFC03_FullScaleSI"		, _K_S_IO	, 	MFC03_FullScaleSI	,	0	} ,
	{ "MFC04_FullScaleSI"		, _K_S_IO	, 	MFC04_FullScaleSI	,	0	} ,
	{ "MFC05_FullScaleSI"		, _K_S_IO	, 	MFC05_FullScaleSI	,	0	} ,
	{ "MFC06_FullScaleSI"		, _K_S_IO	, 	MFC06_FullScaleSI	,	0	} ,
	{ "MFC07_FullScaleSI"		, _K_S_IO	, 	MFC07_FullScaleSI	,	0	} ,
	{ "MFC08_FullScaleSI"		, _K_S_IO	, 	MFC08_FullScaleSI	,	0	} ,
	{ "MFC09_FullScaleSI"		, _K_S_IO	, 	MFC09_FullScaleSI	,	0	} ,

	{ "MFC01_SWVersionSI"		, _K_S_IO	, 	MFC01_SWVersionSI	,	0	} ,
	{ "MFC02_SWVersionSI"		, _K_S_IO	, 	MFC02_SWVersionSI	,	0	} ,
	{ "MFC03_SWVersionSI"		, _K_S_IO	, 	MFC03_SWVersionSI	,	0	} ,
	{ "MFC04_SWVersionSI"		, _K_S_IO	, 	MFC04_SWVersionSI	,	0	} ,
	{ "MFC05_SWVersionSI"		, _K_S_IO	, 	MFC05_SWVersionSI	,	0	} ,
	{ "MFC06_SWVersionSI"		, _K_S_IO	, 	MFC06_SWVersionSI	,	0	} ,
	{ "MFC07_SWVersionSI"		, _K_S_IO	, 	MFC07_SWVersionSI	,	0	} ,
	{ "MFC08_SWVersionSI"		, _K_S_IO	, 	MFC08_SWVersionSI	,	0	} ,
	{ "MFC09_SWVersionSI"		, _K_S_IO	, 	MFC09_SWVersionSI	,	0	} ,

	{ "MFC01_HWVersionSI"		, _K_S_IO	, 	MFC01_HWVersionSI	,	0	} ,
	{ "MFC02_HWVersionSI"		, _K_S_IO	, 	MFC02_HWVersionSI	,	0	} ,
	{ "MFC03_HWVersionSI"		, _K_S_IO	, 	MFC03_HWVersionSI	,	0	} ,
	{ "MFC04_HWVersionSI"		, _K_S_IO	, 	MFC04_HWVersionSI	,	0	} ,
	{ "MFC05_HWVersionSI"		, _K_S_IO	, 	MFC05_HWVersionSI	,	0	} ,
	{ "MFC06_HWVersionSI"		, _K_S_IO	, 	MFC06_HWVersionSI	,	0	} ,
	{ "MFC07_HWVersionSI"		, _K_S_IO	, 	MFC07_HWVersionSI	,	0	} ,
	{ "MFC08_HWVersionSI"		, _K_S_IO	, 	MFC08_HWVersionSI	,	0	} ,
	{ "MFC09_HWVersionSI"		, _K_S_IO	, 	MFC09_HWVersionSI	,	0	} ,

	{ "MFC01_GasNumSI"		, _K_S_IO	, 	MFC01_GasNumSI	 	,	0	} ,
	{ "MFC02_GasNumSI"		, _K_S_IO	, 	MFC02_GasNumSI	 	,	0	} ,
	{ "MFC03_GasNumSI"		, _K_S_IO	, 	MFC03_GasNumSI	 	,	0	} ,
	{ "MFC04_GasNumSI"		, _K_S_IO	, 	MFC04_GasNumSI	 	,	0	} ,
	{ "MFC05_GasNumSI"		, _K_S_IO	, 	MFC05_GasNumSI	 	,	0	} ,
	{ "MFC06_GasNumSI"		, _K_S_IO	, 	MFC06_GasNumSI	 	,	0	} ,
	{ "MFC07_GasNumSI"		, _K_S_IO	, 	MFC07_GasNumSI	 	,	0	} ,
	{ "MFC08_GasNumSI"		, _K_S_IO	, 	MFC08_GasNumSI	 	,	0	} ,
	{ "MFC09_GasNumSI"		, _K_S_IO	, 	MFC09_GasNumSI	 	,	0	} ,

	{ "MFC01_OffsetASI"		, _K_S_IO	, 	MFC01_OffsetASI	 	,	0	} ,
	{ "MFC02_OffsetASI"		, _K_S_IO	, 	MFC02_OffsetASI	 	,	0	} ,
	{ "MFC03_OffsetASI"		, _K_S_IO	, 	MFC03_OffsetASI	 	,	0	} ,
	{ "MFC04_OffsetASI"		, _K_S_IO	, 	MFC04_OffsetASI	 	,	0	} ,
	{ "MFC05_OffsetASI"		, _K_S_IO	, 	MFC05_OffsetASI	 	,	0	} ,
	{ "MFC06_OffsetASI"		, _K_S_IO	, 	MFC06_OffsetASI	 	,	0	} ,
	{ "MFC07_OffsetASI"		, _K_S_IO	, 	MFC07_OffsetASI	 	,	0	} ,
	{ "MFC08_OffsetASI"		, _K_S_IO	, 	MFC08_OffsetASI	 	,	0	} ,
	{ "MFC09_OffsetASI"		, _K_S_IO	, 	MFC09_OffsetASI	 	,	0	} ,

	{ "MFC01_OffsetBSI"		, _K_S_IO	, 	MFC01_OffsetBSI	 	,	0	} ,
	{ "MFC02_OffsetBSI"		, _K_S_IO	, 	MFC02_OffsetBSI	 	,	0	} ,
	{ "MFC03_OffsetBSI"		, _K_S_IO	, 	MFC03_OffsetBSI	 	,	0	} ,
	{ "MFC04_OffsetBSI"		, _K_S_IO	, 	MFC04_OffsetBSI	 	,	0	} ,
	{ "MFC05_OffsetBSI"		, _K_S_IO	, 	MFC05_OffsetBSI	 	,	0	} ,
	{ "MFC06_OffsetBSI"		, _K_S_IO	, 	MFC06_OffsetBSI	 	,	0	} ,
	{ "MFC07_OffsetBSI"		, _K_S_IO	, 	MFC07_OffsetBSI	 	,	0	} ,
	{ "MFC08_OffsetBSI"		, _K_S_IO	, 	MFC08_OffsetBSI	 	,	0	} ,
	{ "MFC09_OffsetBSI"		, _K_S_IO	, 	MFC09_OffsetBSI	 	,	0	} ,

	{ "MFC01_MKP_OffsetBSI"		, _K_S_IO	, 	MFC01_MKP_OffsetBSI	,	0	} ,
	{ "MFC02_MKP_OffsetBSI"		, _K_S_IO	, 	MFC02_MKP_OffsetBSI	,	0	} ,
	{ "MFC03_MKP_OffsetBSI"		, _K_S_IO	, 	MFC03_MKP_OffsetBSI	,	0	} ,
	{ "MFC04_MKP_OffsetBSI"		, _K_S_IO	, 	MFC04_MKP_OffsetBSI	,	0	} ,
	{ "MFC05_MKP_OffsetBSI"		, _K_S_IO	, 	MFC05_MKP_OffsetBSI	,	0	} ,
	{ "MFC06_MKP_OffsetBSI"		, _K_S_IO	, 	MFC06_MKP_OffsetBSI	,	0	} ,
	{ "MFC07_MKP_OffsetBSI"		, _K_S_IO	, 	MFC07_MKP_OffsetBSI	,	0	} ,
	{ "MFC08_MKP_OffsetBSI"		, _K_S_IO	, 	MFC08_MKP_OffsetBSI	,	0	} ,
	{ "MFC09_MKP_OffsetBSI"		, _K_S_IO	, 	MFC09_MKP_OffsetBSI	,	0	} ,

	{ "MFC01_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC01_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC02_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC02_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC03_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC03_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC04_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC04_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC05_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC05_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC06_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC06_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC07_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC07_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC08_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC08_HITACHI_OffsetBSI	,	0	} ,
	{ "MFC09_HITACHI_OffsetBSI"	, _K_S_IO	, 	MFC09_HITACHI_OffsetBSI	,	0	} ,

	{ "MFC01_GainSI"		, _K_S_IO	, 	MFC01_GainSI	 	,	0	} ,
	{ "MFC02_GainSI"		, _K_S_IO	, 	MFC02_GainSI	 	,	0	} ,
	{ "MFC03_GainSI"		, _K_S_IO	, 	MFC03_GainSI	 	,	0	} ,
	{ "MFC04_GainSI"		, _K_S_IO	, 	MFC04_GainSI	 	,	0	} ,
	{ "MFC05_GainSI"		, _K_S_IO	, 	MFC05_GainSI	 	,	0	} ,
	{ "MFC06_GainSI"		, _K_S_IO	, 	MFC06_GainSI	 	,	0	} ,
	{ "MFC07_GainSI"		, _K_S_IO	, 	MFC07_GainSI	 	,	0	} ,
	{ "MFC08_GainSI"		, _K_S_IO	, 	MFC08_GainSI	 	,	0	} ,
	{ "MFC09_GainSI"		, _K_S_IO	, 	MFC09_GainSI	 	,	0	} ,
	
	{ "MFC01_MacIDSI"		, _K_S_IO	, 	MFC01_MacIDSI	 	,	0	} ,
	{ "MFC02_MacIDSI"		, _K_S_IO	, 	MFC02_MacIDSI	 	,	0	} ,
	{ "MFC03_MacIDSI"		, _K_S_IO	, 	MFC03_MacIDSI	 	,	0	} ,
	{ "MFC04_MacIDSI"		, _K_S_IO	, 	MFC04_MacIDSI	 	,	0	} ,
	{ "MFC05_MacIDSI"		, _K_S_IO	, 	MFC05_MacIDSI	 	,	0	} ,
	{ "MFC06_MacIDSI"		, _K_S_IO	, 	MFC06_MacIDSI	 	,	0	} ,
	{ "MFC07_MacIDSI"		, _K_S_IO	, 	MFC07_MacIDSI	 	,	0	} ,
	{ "MFC08_MacIDSI"		, _K_S_IO	, 	MFC08_MacIDSI	 	,	0	} ,
	{ "MFC09_MacIDSI"		, _K_S_IO	, 	MFC09_MacIDSI	 	,	0	} ,

	{ "CTC_CommStatus"		, _K_D_IO  	,	PM_COMMSTATUS		,	0	} ,

	""
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ALARM_MESSAGE_REMAPPING( int id , char  Real_Name[14], char SW_Name[14] , char Real_Size[14] , char SW_Size[14] ) 
{
	char Buffer[256] = {0,}, Buffer2[256] = {0,}, Buffer3[256] = {0,}, Buffer4[256]= {0,}, Buffer5[256] = {0,};
	
	sprintf( Buffer2 , "(S/W GasName=%s"  , SW_Name );
	sprintf( Buffer3 , "/Real=%s)" , Real_Name );
	sprintf( Buffer4 , "(S/W GasMax=%s" , SW_Size );
	sprintf( Buffer5 , "/Real=%s)" , Real_Size );
	
	ALARM_MESSAGE_GET( id , Buffer );
	strcat( Buffer4 , Buffer5 );
	strcat( Buffer3 , Buffer4 );
	strcat( Buffer2 , Buffer3 );
	strcat( Buffer , Buffer2 );
	ALARM_MESSAGE_SET( id , Buffer );
}
void MFC_Log_Event_Thread () 
{
	FILE *fp = NULL; 
	FILE *fp_1 = NULL; 
	BOOL FileEnd = TRUE;
	int CommStatus ;
	int i, ReadCnt;
	char UserEventMessage[5000] ;
	char MFC_MAC_ID[MFC_MAX_COUNT][10] ; 
	char MFC_Serial[MFC_MAX_COUNT][40] , MFC_MF[MFC_MAX_COUNT][40], MFC_Model[MFC_MAX_COUNT][40];
	char MFC_SoftRev[MFC_MAX_COUNT][10] , MFC_HardRev[MFC_MAX_COUNT][10],MFC_StandardNo[MFC_MAX_COUNT][20] , MFC_OffsetA[MFC_MAX_COUNT][20] , MFC_OffsetB[MFC_MAX_COUNT][20], MFC_Gain[MFC_MAX_COUNT][20];
	char MFC_MKP_OffsetB[MFC_MAX_COUNT][20];
	char MFC_HITACHI_OffsetB[MFC_MAX_COUNT][20];
	char MFC_INFO[MFC_MAX_COUNT][64];	
	char MFC_INFO_CHECK[MFC_MAX_COUNT][64];
	char ReadLine[256] = "";
	char Buffer[256] ;  
	char chr_return[256];
	char szMACIDTemp[256] = {0,};

	//initialize.
	memset(UserEventMessage		, 0x00, sizeof(UserEventMessage) );
	memset(MFC_MAC_ID	, 0x00, sizeof(MFC_MAC_ID) );
	memset(MFC_Serial	, 0x00, sizeof(MFC_Serial));
	memset(MFC_MF	, 0x00, sizeof(MFC_MF));
	memset(MFC_Name	, 0x00, sizeof(MFC_Name));
	memset(MFC_MAXSize	, 0x00, sizeof(MFC_MAXSize));
	memset(MFC_Model	, 0x00, sizeof(MFC_Model));
	memset(MFC_SoftRev	, 0x00, sizeof(MFC_SoftRev));
	memset(MFC_HardRev	, 0x00, sizeof(MFC_HardRev));
	memset(MFC_StandardNo	, 0x00, sizeof(MFC_StandardNo));
	memset(MFC_OffsetA	, 0x00, sizeof(MFC_OffsetA));
	memset(MFC_OffsetB	, 0x00, sizeof(MFC_OffsetB));
	memset(MFC_MKP_OffsetB	, 0x00, sizeof(MFC_OffsetB));
	memset(MFC_HITACHI_OffsetB	, 0x00, sizeof(MFC_OffsetB));
	memset(MFC_Gain	, 0x00, sizeof(MFC_Gain));
	memset(MFC_INFO	, 0x00, sizeof(MFC_INFO));
	memset(MFC_INFO_CHECK	, 0x00, sizeof(MFC_INFO_CHECK));
	memset(MFC_BASE	, 0x00, sizeof(MFC_BASE));
	memset(MFC_NAME_SIZE_CHK	, 0x00, sizeof(MFC_NAME_SIZE_CHK));
	memset(MFC_OLD_NAME	, 0x00, sizeof(MFC_OLD_NAME));
	memset(MFC_OLD_MAXSIZE	, 0x00, sizeof(MFC_OLD_MAXSIZE));
	

	
	if ( ( fp = fopen( "Parameter/MFC_Info" , "a+" ) ) == NULL ) 
	{
		printf( "Cannot Open MFC_Info.log File\n" , TRUE );
	}

	else
	{					 
		fclose( fp );
		fp = fopen( "Parameter/MFC_Info" , "r" );
		for ( i = 0 ; i < MFC_COUNT ; ++i )
		{
			Get_Line_String_From_File( fp , Buffer , &ReadCnt );
			Get_Data_From_String( Buffer, chr_return, 0);
			sprintf ( MFC_INFO[i], "%s", chr_return );
			_sleep(50);
		}
		fclose( fp );	 
	}

	fp = fopen("Parameter/MFC_Info", "w" );
	
		

	//2016/05/03 MFC GAS NAME & GAS MAX SIZE CHECK.
	
	if ( ( fp_1 = fopen( "Parameter/MFC_Base" , "a+" ) ) == NULL ) 
	{
		printf( "Cannot Open MFC_Base File\n" , TRUE );
	}
	
	else
	{					 
		fclose( fp_1 );
		fp_1 = fopen( "Parameter/MFC_Base" , "r" );
		for ( i = 0 ; i < MFC_COUNT ; ++i )
		{
			Get_Line_String_From_File( fp_1 , Buffer , &ReadCnt );
			Get_Data_From_String( Buffer, chr_return, 0);
			sprintf ( MFC_BASE[i], "%s", chr_return );
			STR_SEPERATE_CHAR ( MFC_BASE[i] , '_',  MFC_OLD_NAME[i] , MFC_OLD_MAXSIZE[i] , 256 );
			_sleep(50);
		}
		fclose( fp_1 );	 
	}
	//]	

	
	for ( i = 0 ; i < MFC_COUNT ; ++i )
	{
		//READ_STRING( MFC01_MacIDSI + i , MFC_MAC_ID[i] , &CommStatus );
		//... MAC ID는 MFC에서 읽지 않고 직접 써줌.... 
		sprintf(szMACIDTemp,"%d",i+11);
		strcpy(MFC_MAC_ID[i],szMACIDTemp);
		READ_STRING( MFC01_SerialNumSI + i , MFC_Serial[i] , &CommStatus );
		READ_STRING( MFC01_ManuFacSI + i , MFC_MF[i] , &CommStatus );
		READ_STRING( MFC01_GasSymbolSI + i , MFC_Name[i] , &CommStatus );
		READ_STRING( MFC01_FullScaleSI + i , MFC_MAXSize[i] , &CommStatus );
		READ_STRING( MFC01_ModelNumSI + i , MFC_Model[i] , &CommStatus );
		READ_STRING( MFC01_SWVersionSI + i , MFC_SoftRev[i] , &CommStatus );
		READ_STRING( MFC01_HWVersionSI + i , MFC_HardRev[i] , &CommStatus );
		READ_STRING( MFC01_GasNumSI + i , MFC_StandardNo[i] , &CommStatus );
		READ_STRING( MFC01_OffsetASI + i , MFC_OffsetA[i] , &CommStatus );
		READ_STRING( MFC01_OffsetBSI + i , MFC_OffsetB[i] , &CommStatus );
		READ_STRING( MFC01_MKP_OffsetBSI + i , MFC_MKP_OffsetB[i] , &CommStatus );
		READ_STRING( MFC01_HITACHI_OffsetBSI + i , MFC_HITACHI_OffsetB[i] , &CommStatus );
		READ_STRING( MFC01_GainSI + i , MFC_Gain[i] , &CommStatus );
							
		if ( STRCMP_L ( MFC_MAC_ID[i] , "" ) )		sprintf (MFC_MAC_ID[i] ,"$") ; 
		if ( STRCMP_L ( MFC_Serial[i] , "" ) )		sprintf (MFC_Serial[i] ,"$") ; 
		if ( STRCMP_L ( MFC_MF[i] , "" ) )			sprintf (MFC_MF[i] ,"$") ; 
		if ( STRCMP_L ( MFC_Name[i] , "" ) )		sprintf (MFC_Name[i] ,"$") ; 
		if ( STRCMP_L ( MFC_MAXSize[i] , "" ) )		sprintf (MFC_MAXSize[i] ,"$") ; 
		if ( STRCMP_L ( MFC_Model[i] , "" ) )		sprintf (MFC_Model[i] ,"$") ; 
		if ( STRCMP_L ( MFC_SoftRev[i] , "" ) )		sprintf (MFC_SoftRev[i] ,"$") ; 
		if ( STRCMP_L ( MFC_HardRev[i] , "" ) )		sprintf (MFC_HardRev[i] ,"$") ; 
		if ( STRCMP_L ( MFC_StandardNo[i] , "" ) )  sprintf (MFC_StandardNo[i] ,"$") ;
		sprintf (MFC_OffsetA[i] ,"$") ; 
		//if ( STRCMP_L ( MFC_OffsetA[i] , "" ) )		sprintf (MFC_OffsetA[i] ,"$") ; 
		sprintf (MFC_OffsetB[i] ,"$") ; 
		//if ( STRCMP_L ( MFC_OffsetB[i] , "" ) )		sprintf (MFC_OffsetB[i] ,"$") ; 
		if ( STRCMP_L ( MFC_MKP_OffsetB[i] , "" ) )	sprintf (MFC_MKP_OffsetB[i] ,"$") ;  
		if ( STRCMP_L ( MFC_HITACHI_OffsetB[i] , "" ) )	sprintf (MFC_HITACHI_OffsetB[i] ,"$") ; 
		sprintf (MFC_Gain[i] ,"$") ; 
		//if ( STRCMP_L ( MFC_Gain[i] , "" ) )		sprintf (MFC_Gain[i] ,"$") ; 
			
		if ( STRCMP_L ( MFC_Model[i] , "GF125CXXC" ) )
		{
			sprintf(MFC_INFO_CHECK[i], "%s%s", MFC_Serial[i], MFC_OffsetB[i]);
		}
		else if ( STRCMP_L ( MFC_MF[i] , "Hitachi Metals, Ltd." ) )
		{
			sprintf(MFC_INFO_CHECK[i], "%s%s", MFC_Serial[i], MFC_HITACHI_OffsetB[i] );
		}
		else
		{
			sprintf(MFC_INFO_CHECK[i], "%s%s", MFC_Serial[i], MFC_MKP_OffsetB[i] );
		}	
			
		fprintf( fp, "%s\n", MFC_INFO_CHECK[i] );

		if(STRCMP_L( MFC_INFO[i], MFC_INFO_CHECK[i] ) == TRUE){}
		else
		{
			if ( STRCMP_L ( MFC_Model[i] , "GF125CXXC" ) )
			{
				sprintf( UserEventMessage , "MFC_INFO M%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s" , 
				MFC_MAC_ID[i],  MFC_Serial[i], MFC_Model[i],  MFC_MF[i], MFC_SoftRev[i], MFC_HardRev[i] ,MFC_Name[i], MFC_StandardNo[i] , MFC_MAXSize[i], MFC_OffsetB[i] , MFC_OffsetA[i],  MFC_Gain[i]);
			}
			else if ( STRCMP_L ( MFC_MF[i] , "Hitachi Metals, Ltd." ) )
			{
				sprintf( UserEventMessage , "MFC_INFO M%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s" , 
				MFC_MAC_ID[i],  MFC_Serial[i], MFC_Model[i],  MFC_MF[i], MFC_SoftRev[i], MFC_HardRev[i] ,MFC_Name[i], MFC_StandardNo[i] , MFC_MAXSize[i], MFC_HITACHI_OffsetB[i] , MFC_OffsetA[i],  MFC_Gain[i]);
			}				
			else
			{
				sprintf( UserEventMessage , "MFC_INFO M%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s" , 
				MFC_MAC_ID[i],  MFC_Serial[i], MFC_Model[i],  MFC_MF[i], MFC_SoftRev[i], MFC_HardRev[i] ,MFC_Name[i], MFC_StandardNo[i] , MFC_MAXSize[i], "$" , MFC_OffsetA[i],  MFC_Gain[i]);
			}

			if ( g_ModuleInfo ==    1 )   // PMA
			{
				g_bMFCUpdateOk = TRUE;
				_nINF_KNFS_dWRITE_FUNCTION_EVENT( "MFC_Event_PMA", UserEventMessage );	
			}
			else if ( g_ModuleInfo == 2 )   // PMB
			{
				g_bMFCUpdateOk = TRUE;
				_nINF_KNFS_dWRITE_FUNCTION_EVENT( "MFC_Event_PMB", UserEventMessage );	
			}
			else if ( g_ModuleInfo == 3 )   // PMC
			{
				g_bMFCUpdateOk = TRUE;
				_nINF_KNFS_dWRITE_FUNCTION_EVENT( "MFC_Event_PMC", UserEventMessage );	
			}
			
		}		
		_sleep(100);
	}
	fclose(fp);	
	
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void  MFC_info_Check_Thread( char *Dummy ) 
{
	int	CommStatus, i;
	int PM_Enable_check = 0 ;
	int nTimeOut;
	int nRetryCnt;

	while ( TRUE ) 
	{
		nRetryCnt = 0;

		if ( READ_DIGITAL( PM_COMMSTATUS , &CommStatus ) == Standby && PM_Enable_check == OFF_0)
		{
			//////////////////////////////////////////////////////////////////////////
			//... MFC Diagnosis USE Yes or no 
			if(ON_1 != READ_DIGITAL(PRMD_MFC_Diagnosis,&CommStatus)) 
			{
				if(OFF_0 != READ_DIGITAL(MFCInfoComDO,&CommStatus))
				{
					WRITE_DIGITAL(MFCInfoComDO,OFF_0,&CommStatus);	
				}
				PM_Enable_check = ON_1 ;
				continue;
			}
			//////////////////////////////////////////////////////////////////////////
			//... MFC 정보 IO는 한번만 읽는다. MFC Update하는데 시간이 오래 걸린다. (1 ~ 2 분)
			if(g_bMFCUpdateOk ==  FALSE)
			{
				//////////////////////////////////////////////////////////////////////////
				//... PLC Flag IO Set
				if(ON_1 != READ_DIGITAL(MFCDataScenDI,&CommStatus))
				{
					WRITE_DIGITAL(MFCInfoComDO,ON_1,&CommStatus);
				}
				do
				{
					nTimeOut = 0;
					do 
					{
						nTimeOut++;
						if(ON_1 == READ_DIGITAL(MFCDataScenDI,&CommStatus))
						{
							printf("[MFCDataScenDI %d ] time %d\n",READ_DIGITAL(MFCDataScenDI,&CommStatus),nTimeOut);
							break;
						}
						_sleep(1000);
					} 
					while (nTimeOut < (int)READ_ANALOG(PRMA_TO_MFCScan,&CommStatus));

					if(nTimeOut > (int)READ_ANALOG(PRMA_TO_MFCScan,&CommStatus)) {nRetryCnt++;}
					else break;
				}	
				while(nRetryCnt < READ_ANALOG(PRMA_TO_MFCScanRty,&CommStatus));

				if(nRetryCnt > READ_ANALOG(PRMA_TO_MFCScanRty,&CommStatus)) 
				{
					PM_Enable_check = ON_1 ;
					WRITE_DIGITAL(SW_GbIntlckDM, 1,&CommStatus);
					WRITE_DIGITAL(MFCInfoComDO,OFF_0,&CommStatus);
					ALARM_POST ( ALARM_MFC_UPDATE_IO_FAIL );
					continue;
				}

				//////////////////////////////////////////////////////////////////////////
				//... SW Flag IO On
				//... It Is Updated for MFC Information When this Flag IO Is On
				_sleep(1000);
				WRITE_DIGITAL(DN_MFCInfoRead_DO,ON_1,&CommStatus);
				//...  Waiting MFCInfo I/O Updated 
				_sleep(1000);
				//... SW Flag IO OFF
				WRITE_DIGITAL(DN_MFCInfoRead_DO,OFF_0,&CommStatus);
				//... PLC Flag IO OFF
				WRITE_DIGITAL(MFCInfoComDO,OFF_0,&CommStatus);
				//////////////////////////////////////////////////////////////////////////
			}
			//////////////////////////////////////////////////////////////////////////
			PM_Enable_check = ON_1 ; 
			
			
			MFC_Log_Event_Thread();
		
			for ( i = 0 ; i < MFC_COUNT ; ++i )
			{
				sprintf(MFC_NAME_SIZE_CHK[i], "%s_%s", MFC_Name[i], MFC_MAXSize[i]);
				if ( !STRCMP_L ( MFC_BASE[i] , MFC_NAME_SIZE_CHK[i] ) )
				{
					//... PM Disable
					WRITE_DIGITAL(SW_GbIntlckDM, 1,&CommStatus);
					g_bMFCUpdateOk = FALSE;
					ALARM_MESSAGE_REMAPPING ( ALARM_GAS_INFO_DIFF , MFC_Name[i] , MFC_OLD_NAME[i], MFC_MAXSize[i], MFC_OLD_MAXSIZE[i]);
					ALARM_POST ( ALARM_GAS_INFO_DIFF );									
				}
				_sleep(100);
			}
		}
		
		else if(READ_DIGITAL( PM_COMMSTATUS , &CommStatus ) == Maint && PM_Enable_check == ON_1 )
		{
			PM_Enable_check = OFF_0;
		}
		_sleep(500);
	} 
	_endthread ( );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() 
{
	int nIOStatus;

	WRITE_DIGITAL(MFCInfoComDO,ON_1,&nIOStatus);
	WRITE_DIGITAL(DN_MFCInfoRead_DO,OFF_0,&nIOStatus);
	g_bMFCUpdateOk = FALSE;

	_beginthread(MFC_info_Check_Thread, 0, NULL);
	while ( TRUE ) 	_sleep ( 1000 ) ;	

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() 
{
}
BOOL Program_Init_Code() 
{
	char HostName[256];
	int ServerControlMode;
	int Port1;
	int Port2;
	int ExtendPort;
	int CommStatus;
	
	GET_MODULE_CONNECTION_INFO( HostName , &ServerControlMode , &Port1 , &Port2 , &ExtendPort );

	g_ModuleInfo = READ_DIGITAL( ModuleNo, &CommStatus );	
	
	if ( (!ServerControlMode) && ( ExtendPort > 0 ) ) {
		_nINF_KNFS_SET_FILESAVE_SKIP_STATUS( SYSTEM_LOGSKIP_STATUS() );
		if ( _nINF_KNFS_CONNECTION_FOR_CLIENT( HostName , ExtendPort ) != 0 ) {
			printf( "\n\tERROR -> _nINF_KNFS_CONNECTION_FOR_CLIENT ExtendPort Initialize Error\n" );
			return FALSE;
		}
	}

	if ( READ_DIGITAL ( IO_DRIVER_LOAD , &CommStatus ) == OFF_0 )	{ 
		SIMULATION_MODE = TRUE;
	}
	else SIMULATION_MODE = FALSE;


	return TRUE;
}
