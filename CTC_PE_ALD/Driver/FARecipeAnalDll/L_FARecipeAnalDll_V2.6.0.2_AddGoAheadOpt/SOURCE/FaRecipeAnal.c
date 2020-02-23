#include "FaRecipeAnal.h"
#include "CimsInterface.h"
#include "kutlfile.h"

// BW Cleaning Related Added /////////////////////////////////////////////////////////////
#include "TextParser_C.h"
#include "SharedMemory.h"
// BW Cleaning Related Added /////////////////////////////////////////////////////////////

// 2006.10.20
#include "TextLogDll_M.h"
#include "MsgBoxDll.h"

#ifndef _DEBUG
#define _DEBUG
#endif

//=========================================================================================
LotPreInfo LOTPREINFO_CM1;
LotPreInfo LOTPREINFO_CM2;
LotPreInfo LOTPREINFO_CM3;
LotPreInfoToScheduler WRITEINFO_SCH_CM1;
LotPreInfoToScheduler WRITEINFO_SCH_CM2;
LotPreInfoToScheduler WRITEINFO_SCH_CM3;
PMOnlineStsInfo PMMODE_INFO_Prev; // 2006.12.21 Added
PMOnlineStsInfo PMMODE_INFO_Curr; // 2006.12.21 Added
//=========================================================================================
int  User_Function_Point = -2;
int	 gPreviousBeforeLotPreType = 0 ;
int  gPreviousLotPreType = 0;
int  gCurrentLotPreType = 0;
int  gCM1LotPreType = 0;
int  gCM2LotPreType = 0;
int  gCM3LotPreType = 0;
int  gRunningLotPreType = 0;
char gPreviousBefore_Route[256] = "";
char gPrevious_Route[256] = "";
char gCurrent_Route[256] = "";
char gPrevious_BeforeCM[256] = "";
char gPrevious_CM[256] = "";
char gCurrent_CM[256] = "";
char gCM1_Route[256] = "";
char gCM2_Route[256] = "";
char gCM3_Route[256] = "";
char gRunning_Route[256] = "";
char gVisitPM[256] = "";
char szVisitPM[256]  ="";
char szCM1VisitPM[256]   ="";
char szCM2VisitPM[256]   ="";
char szCM3VisitPM[256]   ="";
char szLog[256] = "";

// Start된 Lot 순서를 저장
int gCMStart[3];
int gCMLotPre[3];
int gLCStart = 0;  // 배열을 가리키는 위치 
char gLocal_Route[256] = "";
//

//2013.11.12 Visit PM For MARS by Hyuk
char szCM1VisitPM_2[256]   ="";
char szCM2VisitPM_2[256]   ="";
char szCM3VisitPM_2[256]   ="";
//2013.11.12 Visit PM For MARS by Hyuk

//=========================================================================================

//=========================================================================================
//__declspec(dllexport) void USER_RECIPE_DATA_INIT() {
//}
//=========================================================================================
//__declspec(dllexport) int USER_RECIPE_DATA_MODE() {
//	return TYPE_LOT_FILE_CLUSTER_FILE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_DEFAULT_DATA_READ( int REGID , char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_DEFAULT_DATA_REGIST( char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC , char *DispStr ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_PM_MODE( int REGID , int side ) {
//	return FALSE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_INOUT_DATA_READ( int REGID , int side , OUTCASSETTETemplate *OUT_CASSETTE ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_LOT_DATA_READ( int REGID , int side , LOTStepTemplate *LOT_RECIPE , char *RunName ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_LOT_DUMMY_DATA_READ( int REGID , int side , LOTStepTemplate2 *LOT_RECIPE_DUMMY , char *RunName ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *SlotName ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MAPPING_VERIFICATION( int REGID , int side , int position , CASSETTEVerTemplate MAPPING_INFO ) {
//	return TRUE;
//}

//=========================================================================================
//=========================================================================================
//__declspec(dllexport) int USER_RECIPE_MANUAL_DATA_MODE() {
//	return TYPE_LOT_FILE_CLUSTER_FILE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MANUAL_DEFAULT_DATA_READ( int REGID , char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MANUAL_PM_MODE( int REGID , int side ) {
//	return FALSE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MANUAL_INOUT_DATA_READ( int REGID , int side , OUTCASSETTETemplate *OUT_CASSETTE ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MANUAL_LOT_DATA_READ( int REGID , int side , LOTStepTemplate *LOT_RECIPE , char *RunName ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ( int REGID , int side , LOTStepTemplate2 *LOT_RECIPE_DUMMY , char *RunName ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MANUAL_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *SlotName ) {
//	return TRUE;
//}
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_MANUAL_MAPPING_VERIFICATION( int REGID , int side , int position , CASSETTEVerTemplate MAPPING_INFO ) {
//	return TRUE;
//}
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
//__declspec(dllexport) BOOL USER_RECIPE_INFO_VERIFICATION( int REGID , int side , int loopcount , int id , CLUSTERStepTemplate CLUSTER_INFO ) {
//	return TRUE;
//}
////////////////////////////////////////////////////////////////////////////////////////////////
// 2006.10.20
#define CONFIG_FILE_NAME	".\\Driver\\FaRecipeAnal.ini"
int g_nLotPreNum = 5;									// LotPre Number
enum { CType1_MAINT_CLEAN, CType2_ONLINE_CLEAN };
const char* aSzClnTypeEnum[] = { "CType1_MAINT_CLEAN", "CType2_ONLINE_CLEAN" };
int g_nCleanType = CType1_MAINT_CLEAN;					// Cleaning Type

int g_GoAheadOption = 0;
////////////////////////////////////////////////////////////////////////////////////////////////
// 2006.10.20

void ParseLotPreConfig(char *szData)
{
	char szItem[40];
	_LOG( "Pass ParseLotPreConfig start");

	if(sizeof(szData) < 1) {_LOG( "Pass ParseLotPreConfig finished");  return;}

	tpSetText(szData);
	if(!tpGetWord(szItem, 0)) {_LOG( "Pass ParseLotPreConfig finished"); return;}

	if(strcmp(szItem, "LotPreRcpNumber") == 0) tpGetInt(&g_nLotPreNum, 0);

	_LOG( "Pass ParseLotPreConfig finished");
}

void ParseCleaningConfig(char *szData)
{
	char szItem[40];
	_LOG( "Pass ParseCleaningConfig Started");

	if(sizeof(szData) < 1) {_LOG( "Pass ParseCleaningConfig finished"); return;}
	tpSetText(szData);
	if(!tpGetWord(szItem, 0)) {_LOG( "Pass ParseCleaningConfig finished");return;}

	if(strcmp(szItem, "AutoCleanType") == 0) tpGetInt(&g_nCleanType, 0);

	_LOG( "Pass ParseCleaningConfig finished");
}

void ParseGoAheadConfig(char *szData)
{
	char szItem[64];
	if(sizeof(szData) < 1) return;
	tpSetText(szData);
	if(!tpGetWord(szItem, 0)) return;
	if(strcmp(szItem, "GoAheadOpt") == 0) tpGetInt(&g_GoAheadOption, 0);
}


BOOL ReadConfigration()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(CONFIG_FILE_NAME, "rt");
		if(fp == NULL) break;
		// printf("-----> Read PMC Configration Data\n");

		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				tpSetText(szRead);
				tpGetChar();
				tpGetString(szItem, ']');

				if(strcmp("LotPre Configuration", szItem) == 0)			nTitle = 1;
				else if(strcmp("Cleaning Configuration", szItem) == 0)	nTitle = 2;
				else if(strcmp("WaferGoAhead Configuration", szItem) == 0)		nTitle = 5;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseLotPreConfig(szRead); break;
					case 2: ParseCleaningConfig(szRead); break;
					case 5: ParseGoAheadConfig(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);

	_LOG("-----> LotPreRcpN[%d] AutoCleanType[%d] WaferGoAhead[%d]", g_nLotPreNum, g_nCleanType, g_GoAheadOption);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s", CONFIG_FILE_NAME);
	}
	return bRet;
}




/////////// BW Cleaning Related Added /////////////////////////////////////////////////////////////////////////////////////
typedef struct _POST_RCP_DATA_STRUCT
{
	short int nEnable;
	char pRcpName[80];
} POST_RCP_DATA_STRUCT;

POST_RCP_DATA_STRUCT g_RcpData[10][5]; // Online dry cleaning items * Process Module

BOOL GetCleanOption_SM(char* szPMName, char *szRcpName)
{
	BOOL bRet = FALSE;
	int nMax = 4;
	int nIdx,i = 0;
	int nChValue = 0;
	char szTempName[256] ="";
	char szTotRcpName[256] ="";
	char szOutMsg[64]     ="";
	char szAppName[64] ="Recipe Name Check";
	
	_LOG( "Pass GetCleanOption_SM Started");

	do {
		if(! OpenSharedMemory()) break;

		ReadFromSharedMemory();
		memcpy(g_RcpData, g_pSH_DATA, sizeof(g_RcpData));

		if( 0 == strcmp(szPMName,"PM1")) nIdx = 0;
		else if( 0 == strcmp(szPMName,"PM2")) nIdx = 1;
		else if( 0 == strcmp(szPMName,"PM3")) nIdx = 2;
		else if( 0 == strcmp(szPMName,"PM4")) nIdx = 3;
		else if( 0 == strcmp(szPMName,"PM5")) nIdx = 4;

		for(i=0; i<nMax; i++)
		{
			nChValue = g_RcpData[i][nIdx].nEnable;
			strcpy(szTempName, g_RcpData[i][nIdx].pRcpName);

			if(nChValue == 1)
			{
				strcat(szTotRcpName, szTempName);
				if(i < nMax-1) strcat(szTotRcpName,"|");
			}
			else 
			{
				if(i < nMax-1) strcat(szTotRcpName,"|");
			}
			strcpy(szTempName, "");
			printf("-----> %s En:%d RcpName:%s\n", szPMName, nChValue, szRcpName);
		}

		//strcat(szTotRcpName,"|");
		strcpy(szRcpName, szTotRcpName);

		FreeSharedMemory();
		bRet = TRUE;
	} while(0);

	if( strlen(szRcpName) > 60)
	{
		strcat(szOutMsg,"Total online dry cleaning recipe name is too long ! ");
		_gOutMsgBox(szAppName, szOutMsg, FALSE);
		bRet = FALSE;
	}

	_LOG( "Pass GetCleanOption_SM finished");


	Sleep(10);
	return bRet;
}

// It was added for Online Cleaning 2007.01.15 ////////////////////////////////////////////////////
BOOL IsPMCleaning(int nPM)
{
	BOOL bRet = FALSE;
	char szIOName[64]		= "";	 
	int  nRcpType	        = 0;	
	int  nProcessRunSts     = 0;
	int nDIO_Point = -2;
	int CommStatus = 0;	

	_LOG( "Pass IsPMCleaning Started");

	nRcpType = 0;
	strcpy(  szIOName , "");
	sprintf( szIOName , "PM%d." , nPM+1 );
	strcat(  szIOName , "PRO_RcpTypeDM"); // RcpTypeEnum Unknown Proc PreProc	PostProc Idle Clean	Purge LotPre LotPost

	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, szIOName ); // Read Lot Pre Enable Option per PM
	if(nDIO_Point != -2 )
	{
		nRcpType =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	}

	nDIO_Point	   = -2;
	nProcessRunSts =  0;
	strcpy(  szIOName , "");
	sprintf( szIOName , "PM%d." , nPM+1 );
	strcat(  szIOName , "PRO_RunStsDM"); // ProcStsEnum	Stop	Running

	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, szIOName ); // Read Lot Pre Enable Option per PM
	if(nDIO_Point != -2 )
	{
		nProcessRunSts =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	}

//	2013.01.03 Auto Leak Check Recipe is Purge Recipe
//	if(nRcpType > 4 && nRcpType < 7 &&  nProcessRunSts == 1)	bRet = TRUE;
	if(nRcpType > 4 && nRcpType < 6 &&  nProcessRunSts == 1)	bRet = TRUE;
	else														bRet = FALSE;

	_LOG( "Pass IsPMCleaning finished");

	return bRet;
}
// It was added for Online Cleaning 2007.01.15 /////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
__declspec(dllexport) void USER_RECIPE_DATA_INIT() {
	_LOG( "Pass USER_RECIPE_DATA_INIT Started");

}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
__declspec(dllexport) int USER_RECIPE_DATA_MODE() {
	_LOG( "Pass USER_RECIPE_DATA_MODE Started");

	return  TYPE_LOT_FILE_CLUSTER_ALL; //TYPE_LOT_FILE_CLUSTER_FILE;

	//Mode TYPE_LOT_FILE_CLUSTER_ALL : Crash발생
	//TYPE_LOT_FILE_CLUSTER_FILE     : Crash발생 안하나 Post Recipe진행안됨
}
//-----------------------------------------------------------------------------------------

// CLUSTER_DATA_READ called after LOT_DATA_READ called, this function is called wafer numbers in continue
__declspec(dllexport) BOOL USER_RECIPE_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *SlotName )
{

	int  i , j ,nDIO_Point , nOpt , CommStatus;
	char PM1CleanRecipe[256];
	char PM2CleanRecipe[256];
	char PM3CleanRecipe[256];
	char PM4CleanRecipe[256];
	char PM5CleanRecipe[256];

	_LOG( "Pass USER_RECIPE_CLUSTER_DATA_READ Started");

	//if(g_nCleanType != CType2_ONLINE_CLEAN) {_LOG( "Pass USER_RECIPE_CLUSTER_DATA_READ finished");return TRUE;}
	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "DRY_CONTROL_OPTION" );
	if(nDIO_Point != -2 )
	{
		nOpt =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	} else {
		printf ("o,.O;; IO Point Get Error \n");
	}

	if (1 != nOpt) return TRUE;

	strcpy(PM1CleanRecipe,"");
	strcpy(PM2CleanRecipe,"");	
	strcpy(PM3CleanRecipe,"");
	strcpy(PM4CleanRecipe,"");
	strcpy(PM5CleanRecipe,"");

	if(!GetCleanOption_SM("PM1", PM1CleanRecipe)) return FALSE;
	_LOG( "GetCleanOption_SM of PM1:%s",PM1CleanRecipe);
	if(!GetCleanOption_SM("PM2", PM2CleanRecipe)) return FALSE;
	_LOG( "GetCleanOption_SM of PM2:%s",PM2CleanRecipe);
	if(!GetCleanOption_SM("PM3", PM3CleanRecipe)) return FALSE;
	_LOG( "GetCleanOption_SM of PM3:%s",PM3CleanRecipe);
	//if(!GetCleanOption_SM("PM4", PM4CleanRecipe)) return FALSE;
	//_LOG( "GetCleanOption_SM of PM4:%s",PM4CleanRecipe);
	//if(!GetCleanOption_SM("PM5", PM5CleanRecipe)) return FALSE;
	//_LOG( "GetCleanOption_SM of PM5:%s",PM5CleanRecipe);
	
	// This is test for multi clean recipe running ///////////////////
	//strcpy(PM3CleanRecipe,"C_PM3-1.cln:(CLEAN)|LCHK|C_PM3-3.cln:(CLEAN)");
	// This is test for multi clean recipe running ///////////////////


	// Set Post Process as a Force Mode
	for ( i = 0 ; i < 1 ; i++ ) {
		for ( j = 0 ; j < 5 ; j++ ) {
			if	( j == 0 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM1CleanRecipe) ,"","" , PM1CleanRecipe );
				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM1PreRecipe , strlen(PM1PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM1CleanRecipe , strlen(PM1CleanRecipe) );
			}
			else if ( j == 1 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM2CleanRecipe) ,"","" , PM2CleanRecipe );

				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM2PreRecipe , strlen(PM2PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM2CleanRecipe , strlen(PM2CleanRecipe) );
			}
			else if ( j == 2 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){

				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM3CleanRecipe) ,"","" , PM3CleanRecipe );

				 //strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM3PreRecipe , strlen(PM3PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM3CleanRecipe , strlen(PM3CleanRecipe) );
			}
			else if ( j == 3 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM4CleanRecipe) ,"", "", PM4CleanRecipe );

				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM4PreRecipe , strlen(PM4PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM4CleanRecipe , strlen(PM4CleanRecipe) );
			}
			else if ( j == 4 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM5CleanRecipe) ,"", "", PM5CleanRecipe );

				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM5PreRecipe , strlen(PM5PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM5CleanRecipe , strlen(PM5CleanRecipe) );
			}
		}
	}

	_LOG( "Pass USER_RECIPE_CLUSTER_DATA_READ finished");
	
	return TRUE;
}
//=========================================================================================																	  StartSlot	EndSlot   LoopCount									
__declspec(dllexport) int USER_RECIPE_MANUAL_DATA_MODE() {
	_LOG( "Pass USER_RECIPE_MANUAL_DATA_MODE started");

	return TYPE_LOT_ALL_CLUSTER_ALL;
}
//=========================================================================================
__declspec(dllexport) BOOL USER_RECIPE_MANUAL_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *SlotName ) {

	int  i , j , nDIO_Point , nOpt , CommStatus ;
	char PM1CleanRecipe[256];
	char PM2CleanRecipe[256];
	char PM3CleanRecipe[256];
	char PM4CleanRecipe[256];
	char PM5CleanRecipe[256];

	_LOG( "Pass USER_RECIPE_MANUAL_CLUSTER_DATA_READ started");

	//if(g_nCleanType != CType2_ONLINE_CLEAN) {_LOG( "Pass USER_RECIPE_MANUAL_CLUSTER_DATA_READ finished"); return TRUE;}
	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "DRY_CONTROL_OPTION" );
	if(nDIO_Point != -2 )
	{
		nOpt =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	} else {
		printf ("o,.O;; IO Point Get Error \n");
	}

	if (1 != nOpt) return TRUE;

	strcpy(PM1CleanRecipe,"");
	strcpy(PM2CleanRecipe,"");	
	strcpy(PM3CleanRecipe,"");
	strcpy(PM4CleanRecipe,"");
	strcpy(PM5CleanRecipe,"");

	if(!GetCleanOption_SM("PM1", PM1CleanRecipe)) return FALSE;
 	_LOG( "GetCleanOption_SM of PM1:%s",PM1CleanRecipe);
	if(!GetCleanOption_SM("PM2", PM2CleanRecipe)) return FALSE;
	_LOG( "GetCleanOption_SM of PM2:%s",PM2CleanRecipe);
	if(!GetCleanOption_SM("PM3", PM3CleanRecipe)) return FALSE;
	_LOG( "GetCleanOption_SM of PM3:%s",PM3CleanRecipe);
	//if(!GetCleanOption_SM("PM4", PM4CleanRecipe)) return FALSE;
	//_LOG( "GetCleanOption_SM of PM4:%s",PM4CleanRecipe);
	//if(!GetCleanOption_SM("PM5", PM5CleanRecipe)) return FALSE;
	//_LOG( "GetCleanOption_SM of PM5:%s",PM5CleanRecipe);
	
	// Set Post Process as a Force Mode
	for ( i = 0 ; i < 1 ; i++ ) {
		for ( j = 0 ; j < 5 ; j++ ) {
			if	( j == 0 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM1CleanRecipe) ,"","" , PM1CleanRecipe );
				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM1PreRecipe , strlen(PM1PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM1CleanRecipe , strlen(PM1CleanRecipe) );
			}
			else if ( j == 1 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM2CleanRecipe) ,"","" , PM2CleanRecipe );

				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM2PreRecipe , strlen(PM2PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM2CleanRecipe , strlen(PM2CleanRecipe) );
			}
			else if ( j == 2 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){

				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM3CleanRecipe) ,"", "", PM3CleanRecipe );

				 //strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM3PreRecipe , strlen(PM3PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM3CleanRecipe , strlen(PM3CleanRecipe) );
			}
			else if ( j == 3 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM4CleanRecipe) ,"", "", PM4CleanRecipe );

				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM4PreRecipe , strlen(PM4PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM4CleanRecipe , strlen(PM4CleanRecipe) );
			}
			else if ( j == 4 && strlen( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ) > 0 ){
				//Modified by jhlee. 2008/09/04
				 Set_ECR_Mixed_String_From_Recipe_Data( 
					 CLUSTER_RECIPE->MODULE_IN_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_PR_RECIPE[i][6] ,
					 CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , strlen(PM5CleanRecipe) ,"", "", PM5CleanRecipe );

				//strncpy ( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j]  , PM5PreRecipe , strlen(PM5PreRecipe) );
//				 strncpy ( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j]  , PM5CleanRecipe , strlen(PM5CleanRecipe) );
			}
		}
	}

	_LOG( "Pass USER_RECIPE_MANUAL_CLUSTER_DATA_READ finished");
	
	return TRUE;
}
/////////// BW Cleaning Related Added /////////////////////////////////////////////////////////////////////////////////////

///////// Function Define ///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WriteLotPreInfoToScheduler(int nPort)
{
	int nDIO_Point = -2;
	int nSIO_Point = -2;
	int CommStatus = 0;
	int	nCounter , i = 0;
	char pszFtnName[256] ="";

	BOOL bRet = FALSE;

	_LOG( "Pass WriteLotPreInfoToScheduler started");

	switch( nPort ) 
	{
		case 0 : // CM1  WRITEINFO_SCH_CM1

			for ( i = 0 ; i < 5 ; i++ ) 
			{
				nSIO_Point = -2;
				nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "e:SCHEDULER" ); 
				if(nSIO_Point != -2 )
				{
					_dll_dWRITE_STRING( nSIO_Point , WRITEINFO_SCH_CM1.szPMLotPreFullRcpName[i] , &CommStatus );
					_LOG( "Scheduler Write Info of CM1:%s",WRITEINFO_SCH_CM1.szPMLotPreFullRcpName[i]);
				}
				_sleep(100);
			}
			break;

		case 1 : // CM2  WRITEINFO_SCH_CM2

			for ( i = 0 ; i < 5 ; i++ ) 
			{
				nSIO_Point = -2;
				nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "e:SCHEDULER" ); 
				if(nSIO_Point != -2 )
				{
					_dll_dWRITE_STRING( nSIO_Point , WRITEINFO_SCH_CM2.szPMLotPreFullRcpName[i] , &CommStatus );
					_LOG( "Scheduler Write Info of CM2:%s",WRITEINFO_SCH_CM2.szPMLotPreFullRcpName[i]);
				}
				_sleep(100);
			}
			break;

		case 2 : // CM3  WRITEINFO_SCH_CM3
			for ( i = 0 ; i < 5 ; i++ ) 
			{
				nSIO_Point = -2;
				nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "e:SCHEDULER" ); 
				if(nSIO_Point != -2 )
				{
					_dll_dWRITE_STRING( nSIO_Point , WRITEINFO_SCH_CM3.szPMLotPreFullRcpName[i] , &CommStatus ); 
					_LOG( "Scheduler Write Info of CM3:%s",WRITEINFO_SCH_CM3.szPMLotPreFullRcpName[i]);
				}
				_sleep(100);
			}
			break;

		default:
			return bRet;
	}

	nCounter = 0;
	while( TRUE ) 
	{
		nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "v:SCHEDULER" ); 
		if(nDIO_Point != -2 )
		{
			if(_dll_dREAD_DIGITAL( nDIO_Point , &CommStatus ) != SYS_RUNNING ) break;
		}

		_sleep(10);
		nCounter += 10;
		if(nCounter > 10000)
		{
			return bRet;	
		}
	}

	_LOG( "Pass WriteLotPreInfoToScheduler finished");

	bRet = TRUE;
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
int GetLotStartOption(int side)
{
	int startmode  = 2;
	int nIO_Point1 = -2;
	int nIO_Point2 = -2;
	int nIO_Point3 = -2;
	int CommStatus;
	int nRunSts  = 0;
	int nRunSts2 = 0;
	int nCM1Sts  = 0;
	int nCM2Sts  = 0;
	int nCM3Sts  = 0;

	int nStartNowLotPreType = -2;
	int nPrevLotPretype = -1;
	int nCurrentRunningLotPreType = 0;
	int nFlowLength = 0;

	// Modified by jhlee 2009/02/19
	int nLCStart = 0;
	int i = 0, cnt = 0;
	int nSizeLocal_Route = 0;
	char strTemp[256] = "";
	//////////////////////////////////////
	
	char *token;
	char seps[]   = ":";
	char Route2[256] = "";
	char Route3[256] = "";
    char szTemp[256] = "";
	char pszFtnName[256] ="";
	char strRoute[256] = "";
	char strRoute1[256] = "";
	char strRoute2[256] = "";
	char strRoute3[256] = "";
	char strStartMode[256] = "";

	BOOL bIsContinusLot   = TRUE;
	BOOL bIsTwoLotRunning = FALSE;
	BOOL bIdentical		  = FALSE;
	BOOL bIdentical2	  = FALSE;

	_LOG( "Pass GetLotStartOption started");
	
	if(0 == side || 1 == side || 2 == side)
	{
		if(0 == side) // CM1
		{
			nStartNowLotPreType = gCM1LotPreType;
			nIO_Point2 = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS2" );
			nRunSts =  _dll_dREAD_DIGITAL( nIO_Point2 , &CommStatus );
			nCM2Sts = nRunSts;
			_LOG( "GetLotStartOption: nCM2Sts=>%d \n",nCM2Sts);

			nIO_Point3 = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS3" );
			nRunSts2 =  _dll_dREAD_DIGITAL( nIO_Point3 , &CommStatus );
			nCM3Sts = nRunSts2;
			_LOG( "GetLotStartOption: nCM3Sts=>%d ",nCM3Sts);
			strcpy(Route2,gCM2_Route);
			strcpy(Route3,gCM3_Route);
		}
		else if(1 == side) // CM2
		{
			nStartNowLotPreType = gCM2LotPreType;
			nIO_Point1 = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS" );
			nRunSts =  _dll_dREAD_DIGITAL( nIO_Point1 , &CommStatus );
			nCM1Sts = nRunSts;
			_LOG( "GetLotStartOption: nCM1Sts=>%d ",nCM1Sts);

			nIO_Point3 = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS3" );
			nRunSts2 =  _dll_dREAD_DIGITAL( nIO_Point3 , &CommStatus );
			nCM3Sts = nRunSts2;
			_LOG( "GetLotStartOption: nCM3Sts=>%d ",nCM3Sts);
			strcpy(Route2,gCM1_Route);
			strcpy(Route3,gCM3_Route);
		}
		else if(2 == side) // CM3
		{
			nStartNowLotPreType = gCM3LotPreType;			
			nIO_Point1 = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS" );
			nRunSts =  _dll_dREAD_DIGITAL( nIO_Point1 , &CommStatus );
			nCM1Sts = nRunSts;
			_LOG( "GetLotStartOption: nCM1Sts=>%d ",nCM1Sts);			

			nIO_Point2 = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS2" );
			nRunSts2 =  _dll_dREAD_DIGITAL( nIO_Point2 , &CommStatus );
			nCM2Sts = nRunSts2;
			_LOG( "GetLotStartOption: nCM2Sts=>%d ",nCM2Sts);
			strcpy(Route2,gCM1_Route);
			strcpy(Route3,gCM2_Route);
		}

		_LOG("GetLotStartOption: nRunSts=>%d  nRunSts2=>%d ",nRunSts,nRunSts2);

		if((nRunSts > 3 && nRunSts < 16) || (nRunSts2 > 3 && nRunSts2 < 16)) // Previous Lot Running
		{
			bIsContinusLot = TRUE;
			if((nRunSts > 3 && nRunSts < 16) && (nRunSts2 > 3 && nRunSts2 < 16))// Two Previous Lot Running
			{
				bIsTwoLotRunning = TRUE;
				_LOG( "GetLotStartOption: bIsTwoLotRunning=>TRUE");
			}
		}
		else
		{
			bIsContinusLot = FALSE;
			_LOG( "GetLotStartOption: bIsTwoLotRunning=>FALSE");
		}	
	}
	else
	{
		_LOG( "GetLotStartOption: Return False side=>%d",side);
		_LOG( "Pass GetLotStartOption finished");
		return 2;
	}
	////////// Check if it is continues Lot ///////////////////////////////////////////////////////////////////////////


	////////// Check if wafer flow sharing ///////////////////////////////////////////////////////////////////////////
	if(bIsContinusLot)
	{
		if(0 == side || 1 == side || 2 == side) // CM1   gPreviousBefore_Route,gPrevious_Route,gCurrent_Route
		{
			// Modified by jhlee 2009/02/19
			if((nRunSts > 3 && nRunSts < 16) || (nRunSts2 > 3 && nRunSts2 < 16) )
			{
				if((gLCStart - 1) >= 1)
				{
					switch(gCMStart[gLCStart-1]) // 선행 진행되고 있는 Lot의 Route 저장
					{
						case 0:	strcpy(gRunning_Route,gCM1_Route);	break;
						case 1:	strcpy(gRunning_Route,gCM2_Route);	break;
						case 2:	strcpy(gRunning_Route,gCM3_Route);	break;
					}
					nCurrentRunningLotPreType = gCMLotPre[gLCStart-1];
				}
				else
				{
					switch(gCMStart[gLCStart]) // 선행 진행되고 있는 Lot의 Route 저장
					{
						case 0:	strcpy(gRunning_Route,gCM1_Route);	break;
						case 1:	strcpy(gRunning_Route,gCM2_Route);	break;
						case 2:	strcpy(gRunning_Route,gCM3_Route);	break;
					}
					nCurrentRunningLotPreType = gCMLotPre[gLCStart];
				}
			}
			switch(gCMStart[gLCStart])  // 지금 Start된 Lot의 Route를 저장
			{
				case 0:	strcpy(gLocal_Route,gCM1_Route);	break;
				case 1:	strcpy(gLocal_Route,gCM2_Route);	break;
				case 2:	strcpy(gLocal_Route,gCM3_Route);	break;
			}

			if(bIsTwoLotRunning) // Two Prevoius Lot Running
			{
				do{
						////// It was added 2006.12.06 to check if the route is identical //////////////////////////////////
						if(strlen(gCurrent_Route) > 3)  // check current route is identical					   
						{  	// A1 Request:: PM3:PM5(A),PM3:PM5(A),PM2:PM4(C)  PM3:PM5(A),PM2:PM4(C),PM2:PM4(C)  READY3
							bIdentical = FALSE;
							token = NULL;
							strcpy(szTemp,gCurrent_Route);

							//// Compare current route vs. route2 ///////////////////////////////////////////
							token = strtok( szTemp, seps );
							while( token != NULL ) /* While there are tokens in "string" */
							{  
							  if( strlen(token) < 3) break;
							  if(NULL == strstr(Route2,token)) // route2 has no token(PM1,PM2,PM3,PM4...)
							  { // route2 not shares PM with current route ,
								bIdentical = TRUE;
							  }
							  else
							  { // route2 shares PM with current route
								bIdentical = FALSE;
								break;
							  }
							  token = strtok( NULL, seps ); // Get next token: 
							}
							//// Compare current route vs. route2 ///////////////////////////////////////////

							//// Compare current route vs. route3 ///////////////////////////////////////////
							bIdentical2 = FALSE;
							token = NULL;
							strcpy(szTemp,gCurrent_Route);

							token = strtok( szTemp, seps );
							while( token != NULL ) /* While there are tokens in "string" */
							{  
							  if( strlen(token) < 3) break;
							  if(NULL == strstr(Route3,token)) // route2 has no token(PM1,PM2,PM3,PM4...)
							  { // route2 not shares PM with current route ,
								bIdentical2 = TRUE;
							  }
							  else
							  { // route2 shares PM with current route
								bIdentical2 = FALSE;
								break;
							  }
							  token = strtok( NULL, seps ); /* Get next token(PM): */
							}
						
							//// Compare current route vs. route3 ///////////////////////////////////////////
							if(bIdentical == TRUE && bIdentical2 == TRUE)  // 3 Lot Not Sharing
							{
								startmode = 0;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
								_LOG("GetLotStartOption: 3Lot Lot Not Sharing => Concurrent");
								break;
							}
							
							////////////////////////////////////////////////////////////////////////////////////////////////////////////
							if((gLCStart - 1) >= 1)
							{
								nLCStart = gLCStart - 1;
							}
							else
								nLCStart = gLCStart;     // nLCStart : 선행 Lot을 가리키기 위한 배열 포인트 설정

							// Lot-pre 먼저 Check 후 Route 확인
							if(gCMLotPre[nLCStart] == gCMLotPre[gLCStart])   // 바로 앞에 진행되는 Lot의 Lot-pre와 지금 Start된 Lot의 Lot-pre 비교
							{
								if(0 == strcmp(gRunning_Route,gLocal_Route) ||  // 바로 앞에 진행되는 Lot의 Route와 지금 Start된 Lot의 Route 비교
								   NULL != strstr(gRunning_Route,gLocal_Route))
								{
									startmode = 1;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
									sprintf(szLog,"Continue. gRunning_Route => %s, gLocal_Route => %s",gRunning_Route,gLocal_Route);
									_LOG(szLog);
									break;
								}
								else
								{
									nSizeLocal_Route = strlen(gLocal_Route);
									for(i = 0; i < nSizeLocal_Route ; i++)
									{
										strTemp[cnt] = gLocal_Route[i];
									    cnt++;
										if(cnt == 4)
										{
											if(NULL != strstr(gRunning_Route, strTemp))
											{
												startmode = 2;
												sprintf(szLog,"Sequential_2_LotPre. gRunning_Route => %s, gLocal_Route => %s",gRunning_Route,gLocal_Route);
												_LOG(szLog);
												break;
											}
											else
												startmode = 0;
											cnt = 0;
										}

									}
									break;
								}
							}
							// Route 먼저 Check 후 Lot-Pre 확인
							else //if(0 == strcmp(gRunning_Route,gLocal_Route) ||		// 바로 앞에 진행되는 Lot의 Route와 지금 Start된 Lot의 Route 비교
								  // NULL != strstr(gRunning_Route,gLocal_Route))
								{
									if(0 == nStartNowLotPreType) // In case of Lot Pre Not Use
									{
										startmode = 1;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
										_LOG( "GetLotStartOption: 3Lot_Run Current_LotPre_No_Use => Continue");
										break;
									}
									else
									{
										nSizeLocal_Route = strlen(gLocal_Route);
										for(i = 0; i < nSizeLocal_Route ; i++)
										{
											strTemp[cnt] = gLocal_Route[i];
											cnt++;
											if(cnt == 4)
											{
												if(NULL != strstr(gRunning_Route, strTemp))
												{
													startmode = 2;
													sprintf(szLog,"Sequential_2_Route. gRunning_Route => %s, gLocal_Route => %s",gRunning_Route,gLocal_Route);
													_LOG(szLog);
													break;
												}
												else
													startmode = 0;
												cnt = 0;
											}

										}
										break;
									}

								}
//							else
//								break;
						}
						else if( 
							     (NULL != strstr(gCurrent_Route,"PM1") && NULL != strstr(Route2,"PM1"))	||  // PM1 Sharing
							     (NULL != strstr(gCurrent_Route,"PM2") && NULL != strstr(Route2,"PM2"))	||  // PM2 Sharing
						         (NULL != strstr(gCurrent_Route,"PM3") && NULL != strstr(Route2,"PM3"))	||  // PM3 Sharing
							     (NULL != strstr(gCurrent_Route,"PM4") && NULL != strstr(Route2,"PM4"))	||  // PM4 Sharing
						         (NULL != strstr(gCurrent_Route,"PM5") && NULL != strstr(Route2,"PM5"))	||  // PM5 Sharing
								 (NULL != strstr(gCurrent_Route,"PM1") && NULL != strstr(Route3,"PM1")) ||  // PM1 Sharing
							     (NULL != strstr(gCurrent_Route,"PM2") && NULL != strstr(Route3,"PM2")) ||  // PM2 Sharing
						         (NULL != strstr(gCurrent_Route,"PM3") && NULL != strstr(Route3,"PM3")) ||  // PM3 Sharing
							     (NULL != strstr(gCurrent_Route,"PM4") && NULL != strstr(Route3,"PM4")) ||  // PM4 Sharing
						         (NULL != strstr(gCurrent_Route,"PM5") && NULL != strstr(Route3,"PM5"))     // PM5 Sharing								 
								 )   
						{	    // Partial Sharing => Sequential
								if(0 == nStartNowLotPreType) // In case of Lot Pre Not Use
								{
									startmode = 1;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
									_LOG( "GetLotStartOption: 3Lot_Run_Partial_Share No_LotPre =>  Continue");
									break;
								}
								else
								{
									nSizeLocal_Route = strlen(gLocal_Route);
									for(i = 0; i < nSizeLocal_Route ; i++)
									{
										strTemp[cnt] = gLocal_Route[i];
									    cnt++;
										if(cnt == 4)
										{
											if(NULL != strstr(gRunning_Route, strTemp))
											{
												startmode = 2;
												sprintf(szLog,"Sequential_2. gRunning_Route => %s, gLocal_Route => %s",gRunning_Route,gLocal_Route);
												_LOG(szLog);
												break;
											}
											else
												startmode = 0;
											cnt = 0;
										}

									}
//									startmode = 2;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
//									_LOG( "GetLotStartOption: 3Lot_Run_Partial_Share LotPre_Use =>  Sequence");
									break;
								}
						}
						else // Not Sharing => Concurrent
						{
							startmode = 0;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
							_LOG( "GetLotStartOption:  3Lot_Run No_Share => Concurrent");
						}

				}while(0);				
			}
			else // One Prevoius Lot Running
			{
				do{
						// Modified by jhlee 2009/02/19
						if((nRunSts > 3 && nRunSts < 16) || (nRunSts2 > 3 && nRunSts2 < 16))
						{
							if((gLCStart - 1) >= 1)
							{
								switch(gCMStart[gLCStart-1])
								{
									case 0:	strcpy(gRunning_Route,gCM1_Route);	break;
									case 1:	strcpy(gRunning_Route,gCM2_Route);	break;
									case 2:	strcpy(gRunning_Route,gCM3_Route);	break;
								}
								nCurrentRunningLotPreType = gCMLotPre[gLCStart-1];
							}
							else
							{
								switch(gCMStart[gLCStart])
								{
									case 0:	strcpy(gRunning_Route,gCM1_Route);	break;
									case 1:	strcpy(gRunning_Route,gCM2_Route);	break;
									case 2:	strcpy(gRunning_Route,gCM3_Route);	break;
								}
								nCurrentRunningLotPreType = gCMLotPre[gLCStart];
							}

						}
						////////////////////////////////////////////////////////////////////////////////////////////////////////////
						if((gLCStart - 1) >= 1)
						{
							nLCStart = gLCStart - 1;
						}
						else
							nLCStart = gLCStart;     // nLCStart : 선행 Lot을 가리키기 위한 배열 포인트 설정
					
						////////////////////////////////////////////////////////////////////////////////////////////////////////////

						////// It was added 2006.12.06 to check if the route is identical //////////////////////////////////
						if(strlen(gCurrent_Route) > 3)  // check current route is identical ////////////////////////////					   
						{  	
							bIdentical = FALSE;
							token = NULL;
							strcpy(szTemp,gCurrent_Route);

							//// Compare current route vs. route2 ///////////////////////////////////////////
							token = strtok( szTemp, seps );
							while( token != NULL ) // While there are tokens in "string" 
							{  
							  printf( " %s\n", token );
							  if( strlen(token) < 3) break;
							  if(NULL == strstr(gRunning_Route,token)) // route2 has no token(PM1,PM2,PM3,PM4...)
							  { // current route not shares PM with current route ,
								bIdentical = TRUE;
							  }
							  else
							  { // route2 shares PM with current route
								bIdentical = FALSE;
								break;
							  }
							  token = strtok( NULL, seps ); // Get next token: 
							}

							if(bIdentical == TRUE)
							{
								startmode = 0;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
								_LOG("GetLotStartOption: 1 Lot Run  Lot Not Sharing => Concurrent");
								break;
							}
							//// Compare current route vs. route2 ///////////////////////////////////////////
						}
						////// It was added 2006.12.06 to check if the route is identical //////////////////////////////////						

						// Route 먼저 Check 후 Lot-Pre 확인
						sprintf(szLog,"CM: %d, gRunning_Route : %s, gLocal_Route : %s",side,gRunning_Route,gLocal_Route);
						_LOG(szLog);
						if(0 == strcmp(gRunning_Route,gLocal_Route) ||		// 바로 앞에 진행되는 Lot의 Route와 지금 Start된 Lot의 Route 비교
						   NULL != strstr(gRunning_Route,gLocal_Route))

						{
							sprintf(szLog, "CM: %d, gCMLotPre[%d] = %d, gCMLotPre[%d] = %d",side,nLCStart,gCMLotPre[nLCStart], gLCStart,gCMLotPre[gLCStart]);
							_LOG(szLog);
							if(gCMLotPre[nLCStart] == gCMLotPre[gLCStart])  // 바로 앞에 진행되는 Lot의 Lot-pre와 지금 Start된 Lot의 Lot-pre 비교
							{
								startmode = 1;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
								break;
							}
							else
							{
								if(0 == nStartNowLotPreType) // In case of Lot Pre Not Use
								{
									startmode = 1;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
									_LOG( "GetLotStartOption: 2Lot_Run Current_LotPre_No_Use => Continue");
									break;
								}
								else
								{
									nSizeLocal_Route = strlen(gLocal_Route);
									for(i = 0; i < nSizeLocal_Route ; i++)
									{
										strTemp[cnt] = gLocal_Route[i];
										cnt++;
										if(cnt == 4)
										{
											if(NULL != strstr(gRunning_Route, strTemp))
											{
												startmode = 2;
												sprintf(szLog,"Sequential_2_Route. gRunning_Route => %s, gLocal_Route => %s",gRunning_Route,gLocal_Route);
												_LOG(szLog);
												break;
											}
											else
												startmode = 0;
											cnt = 0;
										}

									}
									break;
								}

							}
						}

						else if( (NULL != strstr(gCurrent_Route,"PM1") && NULL != strstr(gRunning_Route,"PM1")) ||  // PM1 Sharing
							     (NULL != strstr(gCurrent_Route,"PM2") && NULL != strstr(gRunning_Route,"PM2")) ||  // PM2 Sharing
						         (NULL != strstr(gCurrent_Route,"PM3") && NULL != strstr(gRunning_Route,"PM3")) ||  // PM3 Sharing
							     (NULL != strstr(gCurrent_Route,"PM4") && NULL != strstr(gRunning_Route,"PM4")) ||  // PM4 Sharing
						         (NULL != strstr(gCurrent_Route,"PM5") && NULL != strstr(gRunning_Route,"PM5")) )   // PM5 Sharing
						{	    // Partial Sharing => Sequential
							if(0 == nStartNowLotPreType) // In case of Lot Pre Not Use
							{
								startmode = 1;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
								_LOG( "GetLotStartOption:  2Lot_Run_Partial_Same Current_No_LotPre => Continue");
								break;
							}
							else
							{
								nSizeLocal_Route = strlen(gLocal_Route);
								for(i = 0; i < nSizeLocal_Route ; i++)
								{
									strTemp[cnt] = gLocal_Route[i];
									cnt++;
									if(cnt == 4)
									{
										if(NULL != strstr(gRunning_Route, strTemp)) 
										{
											startmode = 2;
											sprintf(szLog,"Sequential_1. gRunning_Route => %s, gLocal_Route => %s",gRunning_Route,gLocal_Route);
											_LOG(szLog);
											break;
										}
										else
											startmode = 0;
										cnt = 0;
									}
								}

								break;
							}
						}
						else // Not Sharing => Concurrent
						{
							startmode = 0;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
							_LOG( "GetLotStartOption: 2Lot_Run_Not_Sharing => Concurrent");
						}

				}while(0);
			}		
		}
	}
	else
	{
		startmode = 1;		// 0 - Concurrent ,1 - Continue , 2 - Sequence
		_LOG( "GetLotStartOption: No Continues Lot =>  Continue");
	}

	//.....
	for(i = 1; i <= 3 ; i++)
	{
		switch(gCMStart[i])   // CM 별로 Route가 어떻게 되는지 알고 Print Out 하기 위함.
		{
			case 0:		strcpy(strRoute,gCM1_Route);		break;
			case 1:		strcpy(strRoute,gCM2_Route);		break;
			case 2:		strcpy(strRoute,gCM3_Route);		break;
		}

		//...
		if     (i == 1)		strcpy(strRoute1, strRoute);
		else if(i == 2)		strcpy(strRoute2, strRoute);
		else if(i == 3)		strcpy(strRoute3, strRoute);
	}

	//... 2014.09.06 Add GoAhreadOpt Only First Lot
	if(g_GoAheadOption == 1 && FALSE == bIsContinusLot)
	{	//... Change Lot Start Option (Fixed Continue)
		
		_LOG( "GetLotStartOption: Changed Start Mode[%d] -> [1] by GoAheadOption", startmode);
		startmode = 1;		// 0 - Concurrent, 1 - Continue, 2 - Sequence
	}

	//.....
	if     (startmode == 0)		sprintf(strStartMode, "ConCurrent.");
	else if(startmode == 1)		sprintf(strStartMode, "Continue.  ");
	else if(startmode == 2)		sprintf(strStartMode, "Sequential.");

	sprintf(szLog,"@@@@@@@@@@@ Startmode = %s NowStart => [CM%d]", strStartMode, gCMStart[gLCStart]+1);
	_LOG(szLog);
	sprintf(szLog,"@@@@@@@@@@@ Startmode = %s CMStart  => [CM%d], [CM%d], [CM%d]", strStartMode, gCMStart[1]+1, gCMStart[2]+1, gCMStart[3]+1);
	_LOG(szLog);
	sprintf(szLog,"@@@@@@@@@@@ Startmode = %s CMLotPre => [CM%d]%d, [CM%d]%d, [CM%d]%d",strStartMode,gCMStart[1]+1,gCMLotPre[1],gCMStart[2]+1,gCMLotPre[2],gCMStart[3]+1,gCMLotPre[3]);
	_LOG(szLog);
	sprintf(szLog,"@@@@@@@@@@@ Startmode = %s CMRoute  => [CM%d]%s, [CM%d]%s, [CM%d]%s",strStartMode,gCMStart[1]+1,strRoute1,gCMStart[2]+1,strRoute2,gCMStart[3]+1,strRoute3);
	_LOG(szLog);

	_LOG( "Pass GetLotStartOption finished");
	////////// Check if wafer flow sharing ///////////////////////////////////////////////////////////////////////////

	return startmode;
}

//////////////////////////////////////////////////////////////////////////
int GetLotPreOption(int nPort)
{
	int nDIO_Point  = -2;
	int nRunSts  = 0;
	int nRunSts2 = 0;
	int nCurrentRunSts = 0;
	int nRunStsTmp = 0;
	int nLotPreOpt = 7;
	int CommStatus = 0;
	int nStartLotPreType = 0;
	int nGetLotStartOption = 0;
	int nRunningLotPreType  = 0;
	int nRunningLotPreType2 = 0;
	char pszFtnName[256] = "";

	//Start된 Lot 순서 저장  Modified by jhlee 2009/02/19
	int lc = 1;
	int i = 0;
	int nLoopCnt = 0;
	int nArrayStartCnt = 0;

	BOOL bFirstLot = TRUE;

	//...
	if(gLCStart == 0)					// 처음 시작시 배열을 99로 초기화 함. 아마도 초기화 하지 않아도 될것임.
	{
		for( lc = 1 ; lc <= 3 ; lc++)
		{
			gCMLotPre[lc] = 0;
			gCMStart[lc] = 99;
		}
	}

	gLCStart++;							// 배열을 가리키는 포인터를 하나 증가함.
	if(gLCStart > 3)					// 배열이 3개임으로 배열을 가리키는 포인터가 3 이상되면 
	{									// 배열의 값을 하나씩 앞으로 옮긴다. 첫번째 배열값은 삭제됨.
		for(i = 1 ; i <= 3 ; i++)
		{
			if(nPort == gCMStart[i])	// 지금 Start된 CM을 배열에서 찾아서 삭제하고 배열의 자료를 옮긴다.
			{
				switch(i)
				{
					case 1:
						gCMLotPre[i]   = gCMLotPre[i+1];
						gCMLotPre[i+1] = gCMLotPre[i+2];
						gCMLotPre[i+2] = 0;
						gCMStart[i]    = gCMStart[i+1];
						gCMStart[i+1]  = gCMStart[i+2];
						gCMStart[i+2]  = 99;
						break;
					case 2:
						gCMLotPre[i]   = gCMLotPre[i+1];
						gCMLotPre[i+1] = 0;
						gCMStart[i]    = gCMStart[i+1];
						gCMStart[i+1]  = 99;
						break;
					case 3:
						gCMLotPre[i]   = 0;
						gCMStart[i]    = 99;
						break;
				}
			}
		}
		gLCStart = 3;					// 배열의 값을 앞 배열로 하나씩 옮긴 후 배열을 가리키는 포인터를 3 으로 변경한다.
	}
	else
		for(i = 1 ; i < gLCStart ; i++)
		{
			if(nPort == gCMStart[i])	// 지금 Start된 CM을 배열에서 찾아서 삭제하고 배열의 자료를 옮긴다.
			{
				nArrayStartCnt++;		// 지금 배열에서 같은 CM이 있을 경우 gLCStart 값을 -1을 해서 나중에 gLCStart 값을 변경하기 위한 변수
				switch(i)
				{
					case 1:
						gCMLotPre[i]   = gCMLotPre[i+1];
						gCMLotPre[i+1] = gCMLotPre[i+2];
						gCMLotPre[i+2] = 0;
						gCMStart[i]    = gCMStart[i+1];
						gCMStart[i+1]  = gCMStart[i+2];
						gCMStart[i+2]  = 99;
						break;
					case 2:
						gCMLotPre[i]   = gCMLotPre[i+1];
						gCMLotPre[i+1] = 0;
						gCMStart[i]    = gCMStart[i+1];
						gCMStart[i+1]  = 99;
						break;
					case 3:
						gCMLotPre[i]   = 0;
						gCMStart[i]    = 99;
						break;
				}
			}
		}

	gLCStart = gLCStart - nArrayStartCnt;
	nLoopCnt = gLCStart;
	sprintf(szLog,"++++++++++++++++++ First gCMStart[1] = %d, gCMStart[2] = %d, gCMStart[3] = %d, gLCStart = %d",gCMStart[1],gCMStart[2],gCMStart[3], gLCStart);
	_LOG(szLog);

	// Modified by jhlee 2009/02/19
	for(i = 1 ; i < nLoopCnt ; i++)
	{
		nDIO_Point  = -2;
		switch(gCMStart[i])
		{
			case 0:
					nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS" );  // Read if CM1 is running
					break;
			case 1:
					nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS2" ); // Read if CM2 is running
					break;
			case 2:
					nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS3" ); // Read if CM3 is running
					break;
		}

		if(nDIO_Point != -2 )
		{
			nCurrentRunSts =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
			sprintf(szLog,"*********** Current CM%d Run Status => %d, << gLCStart : %d, gCMStart[%d] >>",gCMStart[i],nCurrentRunSts, gLCStart,i);
			_LOG(szLog);
		}
		else
		{
			sprintf(szLog,"CTC.MAIN_STATUS%d Read Failed.!",gCMStart[i]);
			_LOG(szLog);
			break;
		}

		if(nCurrentRunSts <= 3 || nCurrentRunSts >= 16)
		{
			if((i == 1) && (gCMStart[i] != 99))
			{
				gCMLotPre[1]   = gCMLotPre[2];
				gCMLotPre[2]   = gCMLotPre[3];
				gCMLotPre[3]   = 0;
				gCMStart[1]    = gCMStart[2];
				gCMStart[2]    = gCMStart[3];
				gCMStart[3]    = 99;
				if((gLCStart - 1) >= 1)
				{
					gLCStart--;
					i = 0;
				}
			sprintf(szLog,"=============== Check Array 1. gCMStart[1] = %d, gCMStart[2] = %d, gCMStart[3] = %d, gLCStart = %d",gCMStart[1],gCMStart[2],gCMStart[3], gLCStart);
			_LOG(szLog);
			}
			else if((i == 2) && (gCMStart[i] != 99))
			{
				gCMLotPre[2]   = gCMLotPre[3];
				gCMLotPre[3]   = 0;
				gCMStart[2]    = gCMStart[3];
				gCMStart[3]    = 99;
				if((gLCStart - 1) >= 1)
				{
					gLCStart--;
					i = 0;
				}
			sprintf(szLog,"=============== Check Array 2. gCMStart[1] = %d, gCMStart[2] = %d, gCMStart[3] = %d, gLCStart = %d",gCMStart[1],gCMStart[2],gCMStart[3], gLCStart);
			_LOG(szLog);
			}
		}
	}

	switch(nPort)
	{
		case 0:
			gCMStart[gLCStart]  = nPort;
			gCMLotPre[gLCStart] = gCM1LotPreType;		// 현재의 Lot Pre Type을 배열에 저장한다.
														// gLCStart는 현재의 배열 위치를 가리키는 포인트임. 
														// 바로 앞의 Lot을 확인 하려면 gLCStart -1 을 하여야 함.

			break;
		case 1:
			gCMStart[gLCStart]  = nPort;
			gCMLotPre[gLCStart] = gCM2LotPreType;		// 현재의 Lot Pre Type을 배열에 저장한다.
														// gLCStart는 현재의 배열 위치를 가리키는 포인트임. 
														// 바로 앞의 Lot을 확인 하려면 gLCStart -1 을 하여야 함.
			break;
		case 2:
			gCMStart[gLCStart]  = nPort;
			gCMLotPre[gLCStart] = gCM3LotPreType;		// 현재의 Lot Pre Type을 배열에 저장한다.
														// gLCStart는 현재의 배열 위치를 가리키는 포인트임. 
														// 바로 앞의 Lot을 확인 하려면 gLCStart -1 을 하여야 함.
			break;
	}

	sprintf(szLog,"++++++++++++++++++ Last gCMStart[1] = %d, gCMStart[2] = %d, gCMStart[3] = %d, gLCStart = %d",gCMStart[1],gCMStart[2],gCMStart[3], gLCStart);
	_LOG(szLog);
	////////////////////////////////////////////////////////////////

	_LOG( "Pass GetLotPreOption started");

	if(nPort == 0 || nPort == 1 || nPort == 2)
	{
		if(nPort == 0) // CM1 jhlee Check
		{
			nStartLotPreType = gCM1LotPreType;
			nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS2" ); // Read if CM2 is running
			if(nDIO_Point != -2 )
			{
				nRunSts =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
			}
			else
			{
				_LOG( "CTC.MAIN_STATUS2 Read Failed");
				_LOG( "Pass GetLotPreOption finished");
				return 0;
			}

			nDIO_Point  = -2;
			nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS3" ); // Read if CM3 is running
			if(nDIO_Point != -2 )
			{
				nRunSts2 =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
			}
			else
			{
				_LOG( "CTC.MAIN_STATUS3 Read Failed");
				_LOG( "Pass GetLotPreOption finished");
				return 0;
			}
		}
		else if(nPort == 1) // CM2
		{
			nStartLotPreType = gCM2LotPreType;
			nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS" ); // Read if CM1 is running
			if(nDIO_Point != -2 )
			{
				nRunSts =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
			}
			else
			{
				_LOG( "CTC.MAIN_STATUS Read Failed");
				_LOG( "Pass GetLotPreOption finished");
				return 0;
			}

			nDIO_Point  = -2;
			nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS3" ); // Read if CM3 is running
			if(nDIO_Point != -2 )
			{
				nRunSts2 =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
			}
			else
			{
				_LOG( "CTC.MAIN_STATUS3 Read Failed");
				_LOG( "Pass GetLotPreOption finished");
				return 0;
			}
		}
		else if(nPort == 2) // CM3
		{
			nStartLotPreType = gCM3LotPreType;
			nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS" ); // Read if CM1 is running
			if(nDIO_Point != -2 )
			{
				nRunSts =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
			}
			else
			{
				_LOG( "CTC.MAIN_STATUS Read Failed");
				_LOG( "Pass GetLotPreOption finished");
				return 0;
			}

			nDIO_Point  = -2;
			nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, "CTC.MAIN_STATUS2" ); // Read if CM2 is running
			if(nDIO_Point != -2 )
			{
				nRunSts2 =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
			}
			else
			{
				_LOG( "CTC.MAIN_STATUS2 Read Failed");
				_LOG( "Pass GetLotPreOption finished");
				return 0;
			}
		}

		////////////////////////////////////////////////////////////////////////////
		if((nRunSts > 3 && nRunSts < 16) || (nRunSts2 > 3 && nRunSts2 < 16))		// Previous Lot Running
		{
			bFirstLot = FALSE;
			do
			{
				if(0 == nStartLotPreType)
				{
					nLotPreOpt = 3;
					break;
				}


				if( (nRunSts > 3 && nRunSts < 16) && (nRunSts2 > 3 && nRunSts2 < 16) ) // 2 Lot are running
				{
					if((gCM1LotPreType == gCM2LotPreType) && (gCM1LotPreType == gCM3LotPreType)) // 3 Lot Pre Type are Same
					{
						nLotPreOpt = 3;
						break;
					}
					else
					{
						nLotPreOpt = 7;
						break;
					}
				}

				// Modified by jhlee 2009/02/19
				if((nRunSts > 3 && nRunSts < 16) || (nRunSts2 > 3 && nRunSts2 < 16) ) // 7 == Lot Starting, 15 == Run Complete 
				{
					if((gLCStart - 1) >= 1)		nRunningLotPreType = gCMLotPre[gLCStart-1];
					else						nRunningLotPreType = gCMLotPre[gLCStart];

				}

				if(nRunningLotPreType == nStartLotPreType)
				{
					nLotPreOpt = 3;
					_LOG( "[FUNCTION] gRunningLotPreType is %d" , nRunningLotPreType);
				}

			}while(0);
		}
	}

	nGetLotStartOption = GetLotStartOption(nPort);

	if     (2 == nGetLotStartOption)								nLotPreOpt = 7;
	else if(1 == nGetLotStartOption || 0 == nGetLotStartOption)		nLotPreOpt = 3;

	//... 2014.09.06 Add GoAhreadOpt Only First Lot
	if(g_GoAheadOption == 1 && TRUE == bFirstLot)
	{	//... Change Lot Start Option (Fixed Continue)		
		_LOG( "GetLotPreOption: Changed LotPreOpt[%d] -> [1] by GoAheadOption", nLotPreOpt);
		nLotPreOpt = 1;		// READY
	}

	_LOG( "Pass GetLotPreOption finished");

	return nLotPreOpt;
}

void ResetMakeFullLotPreSetInfo( int nSide)
{
	int i = 0;
	_LOG( "Pass ResetMakeFullLotPreSetInfo started");

	if(nSide == 0) // CM1
	{
		for ( i = 0 ; i < 8 ; i++ ) 
		{
			WRITEINFO_SCH_CM1.nLotPreType = 0;
			strcpy(WRITEINFO_SCH_CM1.szPMLotPreFullRcpName[i],"");
		}
	}
	else if(nSide == 1) // CM2
	{
		for ( i = 0 ; i < 8 ; i++ ) 
		{
			WRITEINFO_SCH_CM2.nLotPreType = 0;
			strcpy(WRITEINFO_SCH_CM2.szPMLotPreFullRcpName[i],"");
		}
	}
	else if(nSide == 2) // CM3
	{
		for ( i = 0 ; i < 8 ; i++ ) 
		{
			WRITEINFO_SCH_CM3.nLotPreType = 0;
			strcpy(WRITEINFO_SCH_CM3.szPMLotPreFullRcpName[i],"");
		}
	}

	_LOG( "Pass ResetMakeFullLotPreSetInfo finished");
}

//////////////////////////////////////////////////////////////////////////
BOOL MakeFullLotPreSetInfo(int nPort, int nLotPreType)
{
	char Buffer[64]			= "";
	char szLotPre[256]		= "";
	char szLotPreFull[256]	= "";
	char pszFtnName[256]	="";

	int  nLotPreOpt, i		= 0;
	int  nLotPreMax         = 5;

	BOOL bRet = FALSE;

	_LOG( "Pass MakeFullLotPreSetInfo started");

	if(g_nLotPreNum == 10)		nLotPreMax = 10;
	else						nLotPreMax = 5;

    nLotPreOpt = GetLotPreOption(nPort);

	_LOG( "[FUNCTION] MakeFullLotPreSetInfo of GetLotPreOption:%d" , nLotPreOpt);
	_LOG( " g_nLotPreNum:%d   nLotPreMax:%d " , g_nLotPreNum,nLotPreMax );
	
	switch(nLotPreOpt) 
	{
		case 0 :	sprintf( szLotPre , "");								break;
		case 1 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY");		break;
		case 2 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY2");	break;
		case 3 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY3");	break;
		case 4 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY4");	break;
		case 5 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY5");	break;
		case 6 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY6");	break;
		case 7 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY7");	break;
		case 8 :	sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY8");	break;
		default:	
			_LOG( "Pass MakeFullLotPreSetInfo finished");		
			return bRet;
	}

	if(nLotPreType > 0 && nLotPreType < nLotPreMax+1)
	{
		switch( nPort ) 
		{
			case 0 : // CM1  WRITEINFO_SCH_CM1

				for ( i = 0 ; i < 5 ; i++ ) 
				{
					strcpy(Buffer,""); 
					strcpy(szLotPreFull,""); 
					if(LOTPREINFO_CM1.nPMLotPreEnableOpt[i] == 0)
					{
						sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use [0] <= PM1 Info
					}
					else 
					{
						/*	
						if(IsPMCleaning(i)) sprintf( Buffer , "|PM%d|" , i + 1 );
						else sprintf( Buffer , "|PM%d|%s" , i + 1 , LOTPREINFO_CM1.szPMLotPreName[i] );
						*/
						// 2012.02.01 DryCleaning 진행 시 Lot Start 되었을때 Cleaning 진행 중인 PM Lot Pre 미진행건 수정
						if (nLotPreOpt == 3 && IsPMCleaning(i)) {
							sprintf( Buffer , "|PM%d|" , i + 1 );
						}
						else sprintf( Buffer , "|PM%d|%s" , i + 1 , LOTPREINFO_CM1.szPMLotPreName[i] );
					}

					strcpy(szLotPreFull,szLotPre); 
					strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName

					_LOG("szLotPreFull : %s\n", szLotPreFull);

					strcpy(WRITEINFO_SCH_CM1.szPMLotPreFullRcpName[i],szLotPreFull); // Set Full Rcp Name to Sch
				}
				break;
			case 1 : // CM2  WRITEINFO_SCH_CM2
				for ( i = 0 ; i < 5 ; i++ ) 
				{
					strcpy(Buffer,""); 
					strcpy(szLotPreFull,"");
					if(LOTPREINFO_CM2.nPMLotPreEnableOpt[i] == 0)
					{		
						sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use [0] <= PM1 Info
					}
					else 
					{
						/*
						if(IsPMCleaning(i)) sprintf( Buffer , "|PM%d|" , i + 1 );
						else sprintf( Buffer , "|PM%d|%s" , i + 1 , LOTPREINFO_CM2.szPMLotPreName[i] );
						*/
						if (nLotPreOpt == 3 && IsPMCleaning(i)) {
							sprintf( Buffer , "|PM%d|" , i + 1 );
						}
						else sprintf( Buffer , "|PM%d|%s" , i + 1 , LOTPREINFO_CM2.szPMLotPreName[i] );
					}

					strcpy(szLotPreFull,szLotPre); 
					strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName

					_LOG("szLotPreFull : %s\n", szLotPreFull);

					strcpy(WRITEINFO_SCH_CM2.szPMLotPreFullRcpName[i],szLotPreFull); // Set Full Rcp Name to Sch
				}
				break;
			case 2 : // CM3  WRITEINFO_SCH_CM3
				for ( i = 0 ; i < 5 ; i++ ) 
				{

					strcpy(Buffer,""); 
					strcpy(szLotPreFull,""); 
					if(LOTPREINFO_CM3.nPMLotPreEnableOpt[i] == 0)
					{		
						sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use [0] <= PM1 Info
					}
					else 
					{
						/*
						if(IsPMCleaning(i)) sprintf( Buffer , "|PM%d|" , i + 1 );
						else sprintf( Buffer , "|PM%d|%s" , i + 1 , LOTPREINFO_CM3.szPMLotPreName[i] );
						*/
						if (nLotPreOpt == 3 && IsPMCleaning(i)) {
							sprintf( Buffer , "|PM%d|" , i + 1 );
						}
						else sprintf( Buffer , "|PM%d|%s" , i + 1 , LOTPREINFO_CM3.szPMLotPreName[i] );
					}

					strcpy(szLotPreFull,szLotPre); 
					strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName

					_LOG("szLotPreFull : %s\n", szLotPreFull);

					strcpy(WRITEINFO_SCH_CM3.szPMLotPreFullRcpName[i],szLotPreFull); // Set Full Rcp Name to Sch
				}
				break;
			default:
				_LOG( "Pass MakeFullLotPreSetInfo finished");
				return bRet;
		}
	}
	else
	{
		switch( nPort ) 
		{
			case 0 : // CM1  WRITEINFO_SCH_CM1

				for ( i = 0 ; i < 5 ; i++ ) 
				{
					strcpy(Buffer,""); 
					strcpy(szLotPreFull,""); 
					sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use [0] <= PM1 Info
					strcpy(szLotPreFull,"DEFAULT_RECIPE_CHANGE READY3"); 
					strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName
					strcpy(WRITEINFO_SCH_CM1.szPMLotPreFullRcpName[i],szLotPreFull); // Set Full Rcp Name to Sch
				}
				break;
			case 1 : // CM2  WRITEINFO_SCH_CM2
				for ( i = 0 ; i < 5 ; i++ ) 
				{
					strcpy(Buffer,""); 
					strcpy(szLotPreFull,""); 
					sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use [0] <= PM1 Info
					strcpy(szLotPreFull,"DEFAULT_RECIPE_CHANGE READY3");  
					strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName
					strcpy(WRITEINFO_SCH_CM2.szPMLotPreFullRcpName[i],szLotPreFull); // Set Full Rcp Name to Sch
				}
				break;
			case 2 : // CM3  WRITEINFO_SCH_CM3
				for ( i = 0 ; i < 5 ; i++ ) 
				{
					strcpy(Buffer,""); 
					strcpy(szLotPreFull,""); 
					sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use [0] <= PM1 Info
					strcpy(szLotPreFull,"DEFAULT_RECIPE_CHANGE READY3"); 
					strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName
					strcpy(WRITEINFO_SCH_CM3.szPMLotPreFullRcpName[i],szLotPreFull); // Set Full Rcp Name to Sch
				}
				break;
			default:
				_LOG( "Pass MakeFullLotPreSetInfo finished");
				return bRet;
		}

	}

	_LOG( "Pass MakeFullLotPreSetInfo finished");
	bRet = TRUE;
	return bRet;
}

void ResetLotPreInfo( int nSide)
{
	int i = 0;
	_LOG( "Pass ResetLotPreInfo started");

	if(nSide == 0) // CM1
	{
		for ( i = 0 ; i < 8 ; i++ ) 
		{
			LOTPREINFO_CM1.nLotPreType = 0;
			LOTPREINFO_CM1.nPMLotPreEnableOpt[i]	= 0;
			strcpy(LOTPREINFO_CM1.szPMLotPreName[i],"");
		}
	}
	else if(nSide == 1) // CM2
	{
		for ( i = 0 ; i < 8 ; i++ ) 
		{
			LOTPREINFO_CM2.nLotPreType = 0;
			LOTPREINFO_CM2.nPMLotPreEnableOpt[i]	= 0;
			strcpy(LOTPREINFO_CM2.szPMLotPreName[i],"");
		}
	}
	else if(nSide == 2) // CM3
	{
		for ( i = 0 ; i < 8 ; i++ ) 
		{
			LOTPREINFO_CM3.nLotPreType = 0;
			LOTPREINFO_CM3.nPMLotPreEnableOpt[i]	= 0;
			strcpy(LOTPREINFO_CM1.szPMLotPreName[i],"");
		}
	}

	_LOG( "Pass ResetLotPreInfo finished");
}

BOOL SetLotPreInfo( int nSide, int nLotSpecialData)
{
	BOOL bRet = FALSE;
	int nDIO_Point = -2;
	int nSIO_Point = -2;
    int CommStatus;
    int nOpt = 0;
	char szLotPreRcpName[64]      = "";
	char szLotPreRcpIONamePM1[64] = "";
	char szLotPreRcpIONamePM2[64] = "";
	char szLotPreRcpIONamePM3[64] = "";
	char szLotPreRcpIONamePM4[64] = "";
	char szLotPreRcpIONamePM5[64] = "";
	char szLotPreRcpIONamePM6[64] = "";
	char szLotPreRcpOptionNamePM1[64] = "";
	char szLotPreRcpOptionNamePM2[64] = "";
	char szLotPreRcpOptionNamePM3[64] = "";
	char szLotPreRcpOptionNamePM4[64] = "";
	char szLotPreRcpOptionNamePM5[64] = "";
	char szLotPreRcpOptionNamePM6[64] = "";

	char pszFtnName[256] ="";

	_LOG( "Pass SetLotPreInfo started");

	switch( nLotSpecialData ) 
	{
		case 0 :  // Lot Pre Type None
			break;
		case 1 :  // Lot Pre Type A
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseA");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseA");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseA");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseA");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseA");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseA");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeA");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeA");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeA");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeA");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeA");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeA");
			break;
		case 2 : // Lot Pre Type B
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseB");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseB");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseB");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseB");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseB");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseB");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeB");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeB");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeB");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeB");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeB");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeB");
			break;
		case 3 :  // Lot Pre Type C
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseC");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseC");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseC");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseC");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseC");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseC");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeC");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeC");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeC");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeC");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeC");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeC");
			break;
		case 4 :  // Lot Pre Type D
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseD");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseD");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseD");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseD");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseD");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseD");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeD");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeD");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeD");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeD");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeD");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeD");
			break;
		case 5 :
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseE");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseE");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseE");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseE");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseE");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseE");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeE");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeE");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeE");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeE");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeE");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeE");
			break;
		case 6 :
			if(g_nLotPreNum != 10) break;
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseF");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseF");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseF");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseF");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseF");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseF");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeF");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeF");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeF");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeF");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeF");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeF");
			break;
		case 7 :
			if(g_nLotPreNum != 10) break;
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseG");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseG");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseG");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseG");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseG");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseG");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeG");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeG");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeG");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeG");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeG");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeG");
			break;
		case 8 :
			if(g_nLotPreNum != 10) break;
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseH");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseH");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseH");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseH");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseH");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseH");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeH");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeH");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeH");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeH");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeH");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeH");
			break;
		case 9 :
			if(g_nLotPreNum != 10) break;
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseI");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseI");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseI");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseI");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseI");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseI");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeI");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeI");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeI");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeI");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeI");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeI");
			break;
		case 10 :
			if(g_nLotPreNum != 10) break;
			strcpy(szLotPreRcpOptionNamePM1,"CTC.PM1_PreRecipeUseJ");
			strcpy(szLotPreRcpOptionNamePM2,"CTC.PM2_PreRecipeUseJ");
			strcpy(szLotPreRcpOptionNamePM3,"CTC.PM3_PreRecipeUseJ");
			strcpy(szLotPreRcpOptionNamePM4,"CTC.PM4_PreRecipeUseJ");
			strcpy(szLotPreRcpOptionNamePM5,"CTC.PM5_PreRecipeUseJ");
			//strcpy(szLotPreRcpOptionNamePM6,"CTC.PM6_PreRecipeUseJ");
			strcpy(szLotPreRcpIONamePM1,"CTC.PM1_PreRecipeJ");
			strcpy(szLotPreRcpIONamePM2,"CTC.PM2_PreRecipeJ");
			strcpy(szLotPreRcpIONamePM3,"CTC.PM3_PreRecipeJ");
			strcpy(szLotPreRcpIONamePM4,"CTC.PM4_PreRecipeJ");
			strcpy(szLotPreRcpIONamePM5,"CTC.PM5_PreRecipeJ");
			//strcpy(szLotPreRcpIONamePM6,"CTC.PM6_PreRecipeJ");
			break;
		default:
			_LOG( "Invalid LotPre Type Entered => %d",nLotSpecialData);
			_LOG( "Pass SetLotPreInfo finished");
			return bRet;	
			break;
	}

	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////
	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, szLotPreRcpOptionNamePM1 ); // Read Lot Pre Enable Optiuon per PM
	if(nDIO_Point != -2 )
	{
		nOpt =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpOptionName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}

 	nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, szLotPreRcpIONamePM1 );
	if(nSIO_Point != -2 )
	{
		_dll_dREAD_STRING( nSIO_Point , szLotPreRcpName , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpIOName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}
	
	nDIO_Point = -2;
	nSIO_Point = -2;

	if(nSide == 0) // CM1
	{
		LOTPREINFO_CM1.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM1.nPMLotPreEnableOpt[0]	= nOpt;
		strcpy(LOTPREINFO_CM1.szPMLotPreName[0],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM1.szPMLotPreName[0]:%s",LOTPREINFO_CM1.szPMLotPreName[0]);
	}
	else if(nSide == 1) // CM2
	{
		LOTPREINFO_CM2.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM2.nPMLotPreEnableOpt[0]	= nOpt;
		strcpy(LOTPREINFO_CM2.szPMLotPreName[0],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM2.szPMLotPreName[0]:%s",LOTPREINFO_CM2.szPMLotPreName[0]);
	}
	else if(nSide == 2) // CM3
	{
		LOTPREINFO_CM3.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM3.nPMLotPreEnableOpt[0]	= nOpt;
		strcpy(LOTPREINFO_CM3.szPMLotPreName[0],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM3.szPMLotPreName[0]:%s",LOTPREINFO_CM3.szPMLotPreName[0]);
	}
	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////

	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////
	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, szLotPreRcpOptionNamePM2 ); // Read Lot Pre Enable Optiuon per PM2
	if(nDIO_Point != -2 )
	{
		nOpt =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpOptionName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}

 	nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, szLotPreRcpIONamePM2 );
	if(nSIO_Point != -2 )
	{
		_dll_dREAD_STRING( nSIO_Point , szLotPreRcpName , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpIOName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}
	
	nDIO_Point = -2;
	nSIO_Point = -2;

	if(nSide == 0) // CM1
	{
		LOTPREINFO_CM1.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM1.nPMLotPreEnableOpt[1]	= nOpt;
		strcpy(LOTPREINFO_CM1.szPMLotPreName[1],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM1.szPMLotPreName[1]:%s",LOTPREINFO_CM1.szPMLotPreName[1]);
	}
	else if(nSide == 1) // CM2
	{
		LOTPREINFO_CM2.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM2.nPMLotPreEnableOpt[1]	= nOpt;
		strcpy(LOTPREINFO_CM2.szPMLotPreName[1],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM2.szPMLotPreName[1]:%s",LOTPREINFO_CM2.szPMLotPreName[1]);
	}
	else if(nSide == 2) // CM3
	{
		LOTPREINFO_CM3.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM3.nPMLotPreEnableOpt[1]	= nOpt;
		strcpy(LOTPREINFO_CM3.szPMLotPreName[1],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM3.szPMLotPreName[1]:%s",LOTPREINFO_CM3.szPMLotPreName[1]);
	}
	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////

	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////
	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, szLotPreRcpOptionNamePM3 ); // Read Lot Pre Enable Optiuon per PM3
	if(nDIO_Point != -2 )
	{
		nOpt =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpOptionName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}

 	nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, szLotPreRcpIONamePM3 );
	if(nSIO_Point != -2 )
	{
		_dll_dREAD_STRING( nSIO_Point , szLotPreRcpName , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpIOName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}
	
	nDIO_Point = -2;
	nSIO_Point = -2;

	if(nSide == 0) // CM1
	{
		LOTPREINFO_CM1.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM1.nPMLotPreEnableOpt[2]	= nOpt;
		strcpy(LOTPREINFO_CM1.szPMLotPreName[2],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM1.szPMLotPreName[2]:%s",LOTPREINFO_CM1.szPMLotPreName[2]);
	}
	else if(nSide == 1) // CM2
	{
		LOTPREINFO_CM2.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM2.nPMLotPreEnableOpt[2]	= nOpt;
		strcpy(LOTPREINFO_CM2.szPMLotPreName[2],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM2.szPMLotPreName[2]:%s",LOTPREINFO_CM2.szPMLotPreName[2]);
	}
	else if(nSide == 2) // CM3
	{
		LOTPREINFO_CM3.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM3.nPMLotPreEnableOpt[2]	= nOpt;
		strcpy(LOTPREINFO_CM3.szPMLotPreName[2],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM3.szPMLotPreName[2]:%s",LOTPREINFO_CM3.szPMLotPreName[2]);
	}
	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////
	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////
	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, szLotPreRcpOptionNamePM4 ); // Read Lot Pre Enable Optiuon per PM4
	if(nDIO_Point != -2 )
	{
		nOpt =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpOptionName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}

 	nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, szLotPreRcpIONamePM4 );
	if(nSIO_Point != -2 )
	{
		_dll_dREAD_STRING( nSIO_Point , szLotPreRcpName , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpIOName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}
	
	nDIO_Point = -2;
	nSIO_Point = -2;

	if(nSide == 0) // CM1
	{
		LOTPREINFO_CM1.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM1.nPMLotPreEnableOpt[3]	= nOpt;
		strcpy(LOTPREINFO_CM1.szPMLotPreName[3],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM1.szPMLotPreName[3]:%s",LOTPREINFO_CM1.szPMLotPreName[3]);
	}
	else if(nSide == 1) // CM2
	{
		LOTPREINFO_CM2.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM2.nPMLotPreEnableOpt[3]	= nOpt;
		strcpy(LOTPREINFO_CM2.szPMLotPreName[3],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM2.szPMLotPreName[3]:%s",LOTPREINFO_CM2.szPMLotPreName[3]);
	}
	else if(nSide == 2) // CM3
	{
		LOTPREINFO_CM3.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM3.nPMLotPreEnableOpt[3]	= nOpt;
		strcpy(LOTPREINFO_CM3.szPMLotPreName[3],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM3.szPMLotPreName[3]:%s",LOTPREINFO_CM3.szPMLotPreName[3]);
	}
	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////
	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////
	nDIO_Point = _dll_FIND_FROM_STRING( _K_D_IO, szLotPreRcpOptionNamePM5 ); // Read Lot Pre Enable Optiuon per PM5
	if(nDIO_Point != -2 )
	{
		nOpt =  _dll_dREAD_DIGITAL( nDIO_Point , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpOptionName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}

 	nSIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, szLotPreRcpIONamePM5 );
	if(nSIO_Point != -2 )
	{
		_dll_dREAD_STRING( nSIO_Point , szLotPreRcpName , &CommStatus );
	}
	else
	{
		_LOG( "LotPreRcpIOName Read Failed");
		_LOG( "Pass SetLotPreInfo finished");
		return bRet;
	}
	
	nDIO_Point = -2;
	nSIO_Point = -2;

	if(nSide == 0) // CM1
	{
		LOTPREINFO_CM1.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM1.nPMLotPreEnableOpt[4]	= nOpt;
		strcpy(LOTPREINFO_CM1.szPMLotPreName[4],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM1.szPMLotPreName[4]:%s",LOTPREINFO_CM1.szPMLotPreName[4]);
	}
	else if(nSide == 1) // CM2
	{
		LOTPREINFO_CM2.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM2.nPMLotPreEnableOpt[4]	= nOpt;
		strcpy(LOTPREINFO_CM2.szPMLotPreName[4],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM2.szPMLotPreName[4]:%s",LOTPREINFO_CM2.szPMLotPreName[4]);
	}
	else if(nSide == 2) // CM3
	{
		LOTPREINFO_CM3.nLotPreType = nLotSpecialData;
		LOTPREINFO_CM3.nPMLotPreEnableOpt[4]	= nOpt;
		strcpy(LOTPREINFO_CM3.szPMLotPreName[4],szLotPreRcpName);
		//_LOG( "LOTPREINFO_CM3.szPMLotPreName[4]:%s",LOTPREINFO_CM3.szPMLotPreName[4]);
	}
	//// Set Recipe Info to Struct ////////////////////////////////////////////////////////////////////////////////

	_LOG( "Pass SetLotPreInfo finished");

	bRet = TRUE;

	return bRet;
}

///////// Function Define ///////////////////////////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) BOOL USER_RECIPE_ACTION_AFTER_READY( int side ) 
{
	char pszFtnName[256] ="";
	_LOG( "USER_RECIPE_ACTION_AFTER_READY Entered Side:%d" , side);

    if(!WriteLotPreInfoToScheduler(side))
	{
		_LOG( "WriteLotPreInfoToScheduler(%d) failed" , side);
		return FALSE;
	}
	_LOG( "USER_RECIPE_ACTION_AFTER_READY Leaved Side:%d" , side);

    return TRUE;
}

__declspec(dllexport) BOOL USER_RECIPE_START_MODE_CHANGE( int REGID , int side , int loopcount , int firstmode , int *startmode ) 
{
	// 0 - Concurrent ,1 - Continue , 2 - Sequence
	char pszFtnName[256] ="";
	int nLotStartMode  = 2;
	_LOG( "USER_RECIPE_START_MODE_CHANGE Entered");
	////////// Check if it is continus Lot ///////////////////////////////////////////////////////////////////////////
	_LOG( "USER_RECIPE_START_MODE_CHANGE: REGID=>%d side=>%d loopcount=>%d firstmode=>%d",REGID,side,loopcount,firstmode);

	nLotStartMode = GetLotStartOption(side);

	*startmode = nLotStartMode;
	_LOG( "> Start Mode %d Returned To Scheduler <",nLotStartMode);
	_LOG( "USER_RECIPE_START_MODE_CHANGE Leaved");
	
	return TRUE;
}

__declspec(dllexport) BOOL USER_RECIPE_INFO_VERIFICATION_EX( int REGID , int side , int pos , int loopcount , int orderindex , int id , CLUSTERStepTemplate CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData ) {
	int i , j , vPM , CommStatus = 0;
	int PathRange;
	int PathIn;
	int SlotIn;
	int PathOut;
	int SlotOut;
	int nIO_Point = -2;
	int nPMCounter = 1;
	int nCurLotSpecialData = 0;
	
	char szPM[32] ="";
	char Buffer[32] ="";
	char szReadData[256] ="";
	char pszFtnName[256] ="";
	char chPrintOut[256] ="";

	char szVisitPMTemp[256]  ="";
	char szCM1VisitPMTemp[256]   ="";
	char szCM2VisitPMTemp[256]   ="";
	char szCM3VisitPMTemp[256]   ="";

	char szMoudle_In_Recipe[256]  ="";
	char szMoudle_Out_Recipe[256]  ="";	
	char szMoudle_Pr_Recipe[256]  ="";

	char *pdest = NULL;

	char szIOName[64]		= "";	 
	int nDIO_Point = -2;

	//if ( loopcount != 0 ) return TRUE;
	if ( ( side < 0 ) || ( side >= 3 ) )
	{
		_LOG( " USER_RECIPE_INFO_VERIFICATION_EX:return FALSE: side=>%d",side);
		return FALSE;
	}

	_LOG( "Pass 1 function start");

    //_LOG( " ######### USER_RECIPE_INFO_VERIFICATION_EX Entered ##########");
	//sprintf(chPrintOut , "REGID :%d  side:%d  pos:%d  orderindex:%d  id:%d  LotSpecialData:%d",REGID,side,pos,orderindex,id,LotSpecialData);
	//_LOG( chPrintOut);

	//==============================================================================================================
    // Save Lot Special Data per Load Port
	if ( pos == 0  && loopcount == 0 && orderindex == 1) 
	{
	    sprintf(chPrintOut , "REGID :%d  side:%d  pos:%d  orderindex:%d  id:%d  LotSpecialData:%d",REGID,side,pos,orderindex,id,LotSpecialData);
	    _LOG( chPrintOut);

		if(side == 0)
		{
			gCM1LotPreType = LotSpecialData;
		}
		else if(side == 1)
		{
			gCM2LotPreType = LotSpecialData;
		}
		else if(side == 2)
		{
			gCM3LotPreType = LotSpecialData;
		}
		else
		{
			_LOG( " ######### Wrong Port Info Entered ! ##########");
			return FALSE;
		}
	}


	//==============================================================================================================
	//if( pos != 0 )
	if( pos == 0 )
	{
		if ( id == 0 ) 
		{   _LOG( "Pass 2 started"); 
			// First Init Buffer
			//========================================================================================
			strcpy(szVisitPM,"");
			strcpy(szCM1VisitPM,"");
			strcpy(szCM2VisitPM,"");
			strcpy(szCM3VisitPM,"");

			//2013.11.12 Visit PM For MARS by Hyuk
			strcpy(szCM1VisitPM_2,"");
			strcpy(szCM2VisitPM_2,"");
			strcpy(szCM3VisitPM_2,"");
			//2013.11.12 Visit PM For MARS by Hyuk

			//========================================================================================
			// To Do First Init for Inside Buffer
			//========================================================================================
			//========================================================================================
			printf( "==============================================================================\n" );
			printf( "[SIDE %d] data Start\n" , side );
			_LOG( "SIDE %d data Start" , side);
			printf( "==============================================================================\n" );

			_LOG( "Pass 2 finished");

		}

		printf( "========= Print id %d\n",id);

		if ( CLUSTER_INFO.HANDLING_SIDE != -1 ) 
		{
			_LOG( "Pass 3 started");
			
			PathRange = CLUSTER_INFO.HANDLING_SIDE;

			PathIn = CLUSTER_INFO.EXTRA_STATION / 100;
			SlotIn = CLUSTER_INFO.EXTRA_STATION % 100;

			PathOut = CLUSTER_INFO.EXTRA_TIME / 100;
			SlotOut = CLUSTER_INFO.EXTRA_TIME % 100;
			//
			printf( "[%02d][CM%d(%d)->CM%d(%d)]\n" , id , PathIn , SlotIn , PathOut , SlotOut );
			
			// SlotIn => Slot Number
			
			for ( i = 0 ; i < PathRange ; i++ ) 
			{
				for ( j = 0 ; j < MAX_CHAMBER_DEPTH ; j++ ) 
				{
					strcpy(szMoudle_In_Recipe,"");
					strcpy(szMoudle_Out_Recipe,"");
					strcpy(szMoudle_Pr_Recipe,"");

					if ( CLUSTER_INFO.MODULE[i][j] > 0 ) 
					{ // from 5 : 5 is PM1
						printf( "        [%d][PM%d] RECIPE=[%s][%s][%s]\n" , i , CLUSTER_INFO.MODULE[i][j] - 5 + 1 , CLUSTER_INFO.MODULE_IN_RECIPE[i][j] , CLUSTER_INFO.MODULE_OUT_RECIPE[i][j] , CLUSTER_INFO.MODULE_PR_RECIPE[i][j] );
						sprintf(szMoudle_In_Recipe,"%s", CLUSTER_INFO.MODULE_IN_RECIPE[i][j]);
						sprintf(szMoudle_Out_Recipe,"%s", CLUSTER_INFO.MODULE_OUT_RECIPE[i][j]);
						sprintf(szMoudle_Pr_Recipe,"%s", CLUSTER_INFO.MODULE_PR_RECIPE[i][j]);
						
						if(0 == side) // CM1
						{
							strcpy(szCM1VisitPMTemp,"");
							vPM = (int)CLUSTER_INFO.MODULE[i][j] - 5 + 1;	

							pdest = NULL;
							pdest = strstr( szMoudle_In_Recipe, "rcp" );
							if(pdest != NULL)
							{
							   //printf(" ########## szCM1VisitPMTemp=>PM%d\n",vPM);
							   pdest = NULL;
							   sprintf(szCM1VisitPMTemp,"PM%d",vPM);
							   pdest = strstr( szCM1VisitPM, szCM1VisitPMTemp);
							   if(pdest != NULL)  // if the module is not exist, add it 
							   {
									//printf(" ########## The Module Exist Info szCM1VisitPM=>%s szCM1VisitPMTemp=>%s\n",szCM1VisitPM,szCM1VisitPMTemp);								
							   }
							   else
							   {
								   strcat(szCM1VisitPM,szCM1VisitPMTemp);	// Add exist module
								   strcat(szCM1VisitPM,":");

								   //2013.11.12 Visit PM For MARS by Hyuk
								   strcat(szCM1VisitPM_2,szCM1VisitPMTemp);	// Add exist module
								   strcat(szCM1VisitPM_2,"|");
								   //2013.11.12 Visit PM For MARS by Hyuk

								   //printf(" ########## The Module Exist szCM1VisitPM=>%s szCM1VisitPMTemp=>%s\n",szCM1VisitPM,szCM1VisitPMTemp);				
							   }
							}
						}
						else if(1 == side) // CM2
						{
							strcpy(szCM2VisitPMTemp,"");
							vPM = (int)CLUSTER_INFO.MODULE[i][j] - 5 + 1;	

							pdest = NULL;
							pdest = strstr( szMoudle_In_Recipe, "rcp" );
							if(pdest != NULL)
							{
							   //printf(" ########## szCM2VisitPMTemp=>PM%d\n",vPM);
							   pdest = NULL;
							   sprintf(szCM2VisitPMTemp,"PM%d",vPM);
							   pdest = strstr( szCM2VisitPM, szCM2VisitPMTemp);
							   if(pdest != NULL)  // if the module is not exist, add it 
							   {
									//printf(" ########## The Module Exist Info szCM2VisitPM=>%s szCM2VisitPMTemp=>%s\n",szCM2VisitPM,szCM2VisitPMTemp);								
							   }
							   else
							   {
								   strcat(szCM2VisitPM,szCM2VisitPMTemp);	// Add exist module
								   strcat(szCM2VisitPM,":");

								   //2013.11.12 Visit PM For MARS by Hyuk
								   strcat(szCM2VisitPM_2,szCM2VisitPMTemp);	// Add exist module
								   strcat(szCM2VisitPM_2,"|");
								   //2013.11.12 Visit PM For MARS by Hyuk

								   //printf(" ########## The Module Exist szCM2VisitPM=>%s szCM2VisitPMTemp=>%s\n",szCM2VisitPM,szCM2VisitPMTemp);				
							   }
							}
						}
						else if(2 == side) // CM3
						{
							strcpy(szCM3VisitPMTemp,"");
							vPM = (int)CLUSTER_INFO.MODULE[i][j] - 5 + 1;	

							pdest = NULL;
							pdest = strstr( szMoudle_In_Recipe, "rcp" );
							if(pdest != NULL)
							{
							   //printf(" ########## szCM3VisitPMTemp=>PM%d\n",vPM);
							   pdest = NULL;
							   sprintf(szCM3VisitPMTemp,"PM%d",vPM);
							   pdest = strstr( szCM3VisitPM, szCM3VisitPMTemp);
							   if(pdest != NULL)  // if the module is not exist, add it 
							   {
									//printf(" ########## The Module Exist Info szCM3VisitPM=>%s szCM3VisitPMTemp=>%s\n",szCM3VisitPM,szCM3VisitPMTemp);								
							   }
							   else
							   {
								   strcat(szCM3VisitPM,szCM3VisitPMTemp);	// Add exist module
								   strcat(szCM3VisitPM,":");

								   //2013.11.12 Visit PM For MARS by Hyuk
								   strcat(szCM3VisitPM_2,szCM3VisitPMTemp);	// Add exist module
								   strcat(szCM3VisitPM_2,"|");
								   //2013.11.12 Visit PM For MARS by Hyuk

								   //printf(" ########## The Module Exist szCM3VisitPM=>%s szCM3VisitPMTemp=>%s\n",szCM3VisitPM,szCM3VisitPMTemp);				
							   }
							}
						}
						else
						{
							_LOG( "Wrong SIDE %d entered" , side);
							return FALSE;
						}
					}
				}
			}
			
			if(0 == side) // CM1
			{
				//strcpy(gCM1_Route,szVisitPM);
				strcpy(gCM1_Route,szCM1VisitPM);
				//2013.11.12 Visit PM For MARS by Hyuk
				printf("szCM1VisitPM_2 : %s\n", szCM1VisitPM_2);
				nDIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "CTC.LPM1_LotPMPath" ); 
				if(nDIO_Point != -2 )
				{
					_dll_dWRITE_STRING( nDIO_Point, szCM1VisitPM_2, &CommStatus );
				}
				else	_LOG("LPM1 LotPMPath IO Read Fail : %d\n", __LINE__);
			}
			else if(1 == side) // CM2
			{
				//strcpy(gCM2_Route,szVisitPM);
				strcpy(gCM2_Route,szCM2VisitPM);

				//2013.11.12 Visit PM For MARS by Hyuk
				printf("szCM2VisitPM_2 : %s\n", szCM2VisitPM_2);
				nDIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "CTC.LPM2_LotPMPath" ); 
				if(nDIO_Point != -2 )
				{
					_dll_dWRITE_STRING( nDIO_Point, szCM2VisitPM_2, &CommStatus );
				}
				else	_LOG("LPM2 LotPMPath IO Read Fail : %d\n", __LINE__);
			}
			else if(2 == side) // CM3
			{
				//strcpy(gCM3_Route,szVisitPM);
				strcpy(gCM3_Route,szCM3VisitPM);

				//2013.11.12 Visit PM For MARS by Hyuk
				printf("szCM3VisitPM_2 : %s\n", szCM3VisitPM_2);
				nDIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "CTC.LPM3_LotPMPath" ); 
				if(nDIO_Point != -2 )
				{
					_dll_dWRITE_STRING( nDIO_Point, szCM3VisitPM_2, &CommStatus );
				}
				else	_LOG("LPM3 LotPMPath IO Read Fail : %d\n", __LINE__);
			}
			else
			{
				_LOG( "Wrong SIDE %d entered" , side);
				return FALSE;
			}

			_LOG( "Pass 3 finished");
		}
		//=========================================================================================================================================
		if ( id == ( MAX_CASSETTE_SLOT - 1 ) ) 
		{ 
			_LOG( "Pass 4 started");
			// Re-ordering PM Visit Route ///////////////////////////////////////////////////////////
			strcpy(gVisitPM,"");
			do{
				strcpy(szPM,"");
				sprintf(szPM,"PM%d",nPMCounter);

				pdest = NULL;
				if(0 == side) // CM1
				{
					pdest = strstr( szCM1VisitPM, szPM );
				}
				else if(1 == side) // CM2
				{
					pdest = strstr( szCM2VisitPM, szPM );
				}
				else if(2 == side) // CM3
				{
					pdest = strstr( szCM3VisitPM, szPM );
				}
				else
				{
					_LOG( "Wrong SIDE %d entered" , side);
					return FALSE;
				}

				if(pdest != NULL)
				{
					strcat(gVisitPM,szPM);	// Add exist module
					strcat(gVisitPM,":");
				}	
				nPMCounter++;
				printf("gVisitPM=>%s\n",gVisitPM);
				if(nPMCounter > 6) break;
			}while(1);
			// Re-ordering PM Visit Route ///////////////////////////////////////////////////////////

			if(0 == side) // CM1
			{
				nIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "CTC.RECIPE_FILE" );
				_dll_dREAD_STRING( nIO_Point , szReadData , &CommStatus );

				strcpy(gPrevious_BeforeCM,gPrevious_CM);
				strcpy(gPrevious_CM,gCurrent_CM);	// Move Current CM -> Previous PM
				strcpy(gCurrent_CM,"CM1");
				strcpy(gCM1_Route,gVisitPM);
			}
			else if(1 == side) // CM2
			{
				nIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "CTC.RECIPE_FILE2" );
				_dll_dREAD_STRING( nIO_Point , szReadData , &CommStatus );
				strcpy(gPrevious_BeforeCM,gPrevious_CM);
				strcpy(gPrevious_CM,gCurrent_CM);	// Move Current CM -> Previous PM
				strcpy(gCurrent_CM,"CM2");
				strcpy(gCM2_Route,gVisitPM);
			}
			else if(2 == side) // CM3
			{
				nIO_Point = _dll_FIND_FROM_STRING( _K_S_IO, "CTC.RECIPE_FILE3" );
				_dll_dREAD_STRING( nIO_Point , szReadData , &CommStatus );
				strcpy(gPrevious_BeforeCM,gPrevious_CM);
				strcpy(gPrevious_CM,gCurrent_CM);	// Move Current CM -> Previous PM
				strcpy(gCurrent_CM,"CM3");
				strcpy(gCM3_Route,gVisitPM);
			}

			_LOG( " #### Current Lot Recipe Name:%s",szReadData);

			strcpy(gPreviousBefore_Route,gPrevious_Route);			
			strcpy(gPrevious_Route,gCurrent_Route);// Move Current Route -> Previous Route
			strcpy(gCurrent_Route,gVisitPM);

			_LOG( " #### PreviousBefore_Route : %s",gPreviousBefore_Route);
			_LOG( " #### Previous_Route : %s",gPrevious_Route);
			_LOG( " #### Current_Route : %s",gCurrent_Route);
			//_LOG( " #### szVisitPM : %s",szVisitPM);
			_LOG( " #### szCM1VisitPM : %s",szCM1VisitPM);
			_LOG( " #### szCM2VisitPM : %s",szCM2VisitPM);
			_LOG( " #### szCM3VisitPM : %s",szCM3VisitPM);
			_LOG( " #### CM1_Route : %s",gCM1_Route);			
			_LOG( " #### CM2_Route : %s",gCM2_Route);	
			_LOG( " #### CM3_Route : %s",gCM3_Route);
			_LOG( " #### Current_CM : %s",gCurrent_CM);
			_LOG( " #### Previous_CM : %s",gPrevious_CM);

			// Last Set Buffer
			//========================================================================================
			strcpy(szVisitPM,"");
			strcpy(szCM1VisitPM,"");
			strcpy(szCM2VisitPM,"");
			strcpy(szCM3VisitPM,"");
			//========================================================================================
			// To Do First Init for Inside Buffer
			//========================================================================================
			//========================================================================================
			printf( "==============================================================================\n" );
			printf( "[SIDE %d] data Finished\n" , side );
			printf( "==============================================================================\n" );

			// Get LotSpecialData From File //////////////////////////////////////
			//read_int_file_form( HFILE hFilePointer , int *ResultValue );

			// Put Here Lot Pre Info Data ///////////////////////////////////////////////////////
			if(side == 0) nCurLotSpecialData = gCM1LotPreType;
			else if(side == 1) nCurLotSpecialData = gCM2LotPreType;
			else if(side == 2) nCurLotSpecialData = gCM3LotPreType;
			else _LOG( "Wrong Port Entered %d" , side);

			strcpy(Buffer,"");
			//_LOG( chPrintOut);

			sprintf( Buffer , "%d" , nCurLotSpecialData );

			// Add Port info and pass to the sequence //////////////////////////////////////////
			if(side == 0) strcat(Buffer,":CM1");
			else if(side == 1) strcat(Buffer,":CM2");
			else if(side == 2) strcat(Buffer,":CM3");
			else _LOG( "Wrong Port Entered %d" , side);

			// Add Port info and pass to the sequence //////////////////////////////////////////
			//==================================================================================
			printf( "[DLL] LotSpecialData is %s\n" , Buffer );
			_LOG( "[DLL] LotSpecialData is %s" , Buffer);
			//==================================================================================
			//=====  Update Lot Special Data =========================================================================
			gPreviousBeforeLotPreType = gPreviousLotPreType;
			gPreviousLotPreType = gCurrentLotPreType;
			gCurrentLotPreType = nCurLotSpecialData;
			//=====  Update Lot Special Data =========================================================================

			ResetLotPreInfo(side);
			if(!SetLotPreInfo(side,nCurLotSpecialData)) // Set value to LotPreInfo per port
			{
				_LOG( " SetLotPreInfo(%d,%d) failed",side,nCurLotSpecialData);
				return FALSE;
			}
			else
			{
				//_LOG( " SetLotPreInfo(%d,%d) success ! ",side,nCurLotSpecialData);
			}

			ResetMakeFullLotPreSetInfo(side);
			if(!MakeFullLotPreSetInfo(side, nCurLotSpecialData))
			{
				_LOG( " MakeFullLotPreSetInfo(%d,%d) failed",side,nCurLotSpecialData);
				return FALSE;
			}
			else
			{
				//_LOG( " MakeFullLotPreSetInfo(%d,%d) success !",side,nCurLotSpecialData);
			}
			// Put Here Lot Pre Info Data ///////////////////////////////////////////////////////

			_LOG( "Pass 4 finished");
		}	
	}

	//==============================================================================================================
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//20070508 added for dummy operation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) int USER_RECIPE_INVALID_DUMMY_OPERATE(  int bmindex , int bmslot ) {

	int ret;
	printf ("Dummy Operate Start \n");
	_LOG( "Pass USER_RECIPE_INVALID_DUMMY_OPERATE started");

	//Maybe used later
	//AlmResult = _dll_ALARM_MANAGE( 100 );

	User_Function_Point = _dll_FIND_FROM_STRING(_K_F_IO, "DUMMY_MANAGEMENT");
	if (_dll_dWRITE_FUNCTION( User_Function_Point , "ALARM_RECOVERY" ) == 0) {
		printf ("Alarm Recovery Function Call Fail Just Abort \n");
		return 2;
	}

	_sleep(1000);

	while (TRUE) {
		_sleep(100);
		ret = _dll_dREAD_FUNCTION(User_Function_Point);
		if (ret != 1) {
			break;
		}
	}

	printf("Alarm Recovery Result is %d \n" , ret);
	
	if      (3 == ret) return 1; //when return SYS_SUCCESS
	else               return 2;	
}
