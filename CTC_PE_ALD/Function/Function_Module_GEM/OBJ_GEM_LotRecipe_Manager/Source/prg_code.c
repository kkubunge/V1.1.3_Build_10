//-----------------------------------------------------------------------------------
// Common OBJ_GEM_OperatorStartInterface_Manager
//
// File Name : prg_code.c
//
// (C) 2001-2007 IPS Ltd.
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// the function level is higher than the scheduler level
//-----------------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutlanal.h>
#include <KutlFile.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define MAX_CASTSLOT_COUNT				30
#define PPARMTX_LENGTH					64
//------------------------------------------------------------------------------------------
#define		Cluster_Recipe_Different_Error_In_LotRecipe			1
#define		Lot_Recipe_Check_Skip_Error							2
//------------------------------------------------------------------------------------------
int		ALARM_START_POINT	= 280;
//------------------------------------------------------------------------------------------
typedef struct {
	char	LOT_ID[PPARMTX_LENGTH];
	char	CLUSTER_RECIPE[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	char	CLUSTER_RECIPE2[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	int		CLUSTER_USING[MAX_CASTSLOT_COUNT];
	int		CLUSTER_DUMMY_USING;
	char	CLUSTER_DUMMY_RECIPE[PPARMTX_LENGTH+1];
} LOTStepTemplate;
LOTStepTemplate		LOT_RECIPE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
//	{ "SCHEDULER"					, _K_F_IO	,	F_SCHEDULER				,	0	} ,
	{ "e:SCHEDULER"					, _K_S_IO	,	S_SCHEDULER				,	0	} ,
	{ "CTC.Recipe_FILE"				, _K_S_IO	,	RECIPE_FILE				,	0	} ,
	{ "CTC.Recipe_FILE2"			, _K_S_IO	,	RECIPE_FILE2			,	0	} ,
	{ "CTC.Recipe_FILE3"			, _K_S_IO	,	RECIPE_FILE3			,	0	} ,
    { "CTC.OPERATOR_START"			, _K_D_IO	,	OPERATOR_START          ,	0   } ,  
    { "CTC.OPERATOR_START2"			, _K_D_IO	,	OPERATOR_START2         ,	0   } ,  
    { "CTC.OPERATOR_START3"			, _K_D_IO	,	OPERATOR_START3         ,	0   } ,
	{ "PRM_FMCM1_LOTCHECK"			, _K_D_IO	,	FMCM1_LOTCHECK			,	0   } ,
	{ "PRM_FMCM2_LOTCHECK"			, _K_D_IO	,	FMCM2_LOTCHECK			,	0   } ,
	{ "PRM_FMCM3_LOTCHECK"			, _K_D_IO	,	FMCM3_LOTCHECK			,	0   } ,
	{ "CTC.JOB_NAME"				, _K_S_IO	,	JOB_NAME				,	0   } ,
	{ "CTC.JOB_NAME2"				, _K_S_IO	,	JOB_NAME2				,	0   } ,
	{ "CTC.JOB_NAME3"				, _K_S_IO	,	JOB_NAME3				,	0   } ,
	""
};
enum { ON,		OFF };
enum { Check,	NotCheck };
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void Lot_szRecipe_Init() {
	int i;
	strcpy( LOT_RECIPE.LOT_ID , "" );
	for ( i = 0 ; i < MAX_CASTSLOT_COUNT ; i++ ) {
		strcpy( LOT_RECIPE.CLUSTER_RECIPE[i] , "" );
		strcpy( LOT_RECIPE.CLUSTER_RECIPE2[i] , "" );
		LOT_RECIPE.CLUSTER_USING[i]	= 0;
	}
	strcpy( LOT_RECIPE.CLUSTER_DUMMY_RECIPE , "" );
	LOT_RECIPE.CLUSTER_DUMMY_USING	= 0;
}
//------------------------------------------------------------------------------------------
BOOL LotszRecipe_Open( char *OpenName, char *szLotRecipe , char *szPortID) {
	HFILE	hFile;
	BOOL	bStepEndFind;
	int		nkill , ii_return, i, nCommnStatus;
	float	ff_return;
	char	str_return[256] , name_return[256] , type_return , cc_return;
	memset(str_return,		0,		sizeof(str_return));
	memset(name_return,		0,		sizeof(name_return));

	Lot_szRecipe_Init();
	if ( OpenName == NULL ) { 
		printf( "Lot Recipe File Name NULL - when Open\n" ) ;
		return FALSE ;
	}
	hFile = _lopen( OpenName , OF_READ );
	if ( hFile == -1 ) { 
		printf( "Lot Recipe File Open Error - %s\n" , OpenName ) ;
		return FALSE ; 
	}

	if ( !read_form( hFile , ""          , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return FALSE; }
	if ( !read_form( hFile , ""          , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return FALSE; }
	
	bStepEndFind = FALSE;
	nkill = 0;
	while ( !bStepEndFind ) { 
		if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) { _lclose( hFile ); return FALSE; }
		if ( ( type_return == NAME_FORMAT ) && ( strcmp( name_return , "__&&LOT_RECIPE_BEGIN&&__" ) == 0 ) ) bStepEndFind = TRUE;
		if ( nkill >= 1000 )	{ _lclose( hFile ); return FALSE; }
	}

	bStepEndFind = FALSE;
	nkill = 0;
	while( !bStepEndFind ) {
		if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) { _lclose( hFile ); return FALSE; }
		switch ( type_return ) {
			case NAME_FORMAT :
								if ( STRCMP_L( name_return , "__&&LOT_RECIPE_END&&__" ) ) bStepEndFind = TRUE;
								else	nkill++;
								break;
			case INT_FORMAT  :	
								if      ( STRCMP_L( name_return , "CLUSTER_01_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 0]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_02_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 1]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_03_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 2]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_04_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 3]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_05_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 4]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_06_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 5]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_07_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 6]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_08_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 7]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_09_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 8]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_10_USING"	) )	LOT_RECIPE.CLUSTER_USING[ 9]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_11_USING"	) )	LOT_RECIPE.CLUSTER_USING[10]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_12_USING"	) )	LOT_RECIPE.CLUSTER_USING[11]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_13_USING"	) )	LOT_RECIPE.CLUSTER_USING[12]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_14_USING"	) )	LOT_RECIPE.CLUSTER_USING[13]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_15_USING"	) )	LOT_RECIPE.CLUSTER_USING[14]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_16_USING"	) )	LOT_RECIPE.CLUSTER_USING[15]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_17_USING"	) )	LOT_RECIPE.CLUSTER_USING[16]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_18_USING"	) )	LOT_RECIPE.CLUSTER_USING[17]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_19_USING"	) )	LOT_RECIPE.CLUSTER_USING[18]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_20_USING"	) )	LOT_RECIPE.CLUSTER_USING[19]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_21_USING"	) )	LOT_RECIPE.CLUSTER_USING[20]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_22_USING"	) )	LOT_RECIPE.CLUSTER_USING[21]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_23_USING"	) )	LOT_RECIPE.CLUSTER_USING[22]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_24_USING"	) )	LOT_RECIPE.CLUSTER_USING[23]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_25_USING"	) )	LOT_RECIPE.CLUSTER_USING[24]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_26_USING"	) )	LOT_RECIPE.CLUSTER_USING[25]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_27_USING"	) )	LOT_RECIPE.CLUSTER_USING[26]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_28_USING"	) )	LOT_RECIPE.CLUSTER_USING[27]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_29_USING"	) )	LOT_RECIPE.CLUSTER_USING[28]	= ii_return;
								else if ( STRCMP_L( name_return , "CLUSTER_30_USING"	) )	LOT_RECIPE.CLUSTER_USING[29]	= ii_return;
								//
								else if ( STRCMP_L( name_return , "CLUSTER_DUMMY_USING"	) )	LOT_RECIPE.CLUSTER_DUMMY_USING	= ii_return;
								else nkill++;
								nkill++;
								break;

			case FLOAT_FORMAT : nkill++;
								break;

						case STRING_FORMAT :
								if      ( STRCMP_L( name_return , "LOT_ID"					) )	strncpy( LOT_RECIPE.LOT_ID             , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_01_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 0] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_02_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 1] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_03_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 2] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_04_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 3] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_05_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 4] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_06_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 5] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_07_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 6] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_08_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 7] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_09_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 8] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_10_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[ 9] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_11_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[10] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_12_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[11] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_13_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[12] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_14_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[13] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_15_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[14] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_16_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[15] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_17_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[16] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_18_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[17] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_19_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[18] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_20_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[19] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_21_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[20] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_22_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[21] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_23_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[22] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_24_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[23] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_25_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[24] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_26_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[25] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_27_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[26] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_28_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[27] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_29_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[28] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_30_RECIPE"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE[29] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_01_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 0] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_02_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 1] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_03_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 2] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_04_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 3] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_05_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 4] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_06_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 5] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_07_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 6] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_08_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 7] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_09_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 8] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_10_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[ 9] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_11_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[10] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_12_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[11] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_13_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[12] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_14_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[13] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_15_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[14] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_16_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[15] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_17_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[16] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_18_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[17] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_19_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[18] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_20_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[19] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_21_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[20] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_22_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[21] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_23_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[22] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_24_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[23] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_25_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[24] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_26_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[25] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_27_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[26] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_28_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[27] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_29_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[28] , str_return , 32 );
								else if ( STRCMP_L( name_return , "CLUSTER_30_RECIPE2"		) )	strncpy( LOT_RECIPE.CLUSTER_RECIPE2[29] , str_return , 32 );
								//
								else if ( STRCMP_L( name_return , "CLUSTER_DUMMY_RECIPE"	) )	strncpy( LOT_RECIPE.CLUSTER_DUMMY_RECIPE , str_return , 32 );
								else nkill++;
								break;
			default :			nkill++;
								break;
		}
		if ( nkill >= 1000 )	{ _lclose( hFile ); return FALSE; }
	}
	_lclose( hFile );

	for( i = 0; i < 25; i++)
	{
//		printf( "CLUSTER_%d_RECIPE : [%s]\n", i, LOT_RECIPE.CLUSTER_RECIPE[i]);
		if (!STRCMP_L(szLotRecipe, LOT_RECIPE.CLUSTER_RECIPE[i]))
		{
			if( STRCMP_L( szPortID , "PORT1" ) ) {
				ALARM_POST(ALARM_START_POINT + Cluster_Recipe_Different_Error_In_LotRecipe);
//				WRITE_STRING( S_SCHEDULER , "STOPW(F)" , &nCommnStatus );
				WRITE_STRING( S_SCHEDULER , "ASTOP(F)" , &nCommnStatus );
				break;
			}
			else if ( STRCMP_L( szPortID , "PORT2" ) ) {
				ALARM_POST(ALARM_START_POINT + 300 + Cluster_Recipe_Different_Error_In_LotRecipe);
//				WRITE_STRING( S_SCHEDULER , "STOPW2(F)" , &nCommnStatus );
				WRITE_STRING( S_SCHEDULER , "ASTOP2(F)" , &nCommnStatus );
				break;
			}
			else if ( STRCMP_L( szPortID , "PORT3" ) ) {
				ALARM_POST(ALARM_START_POINT + 600 + Cluster_Recipe_Different_Error_In_LotRecipe);
//				WRITE_STRING( S_SCHEDULER , "STOPW3(F)" , &nCommnStatus );
				WRITE_STRING( S_SCHEDULER , "ASTOP3(F)" , &nCommnStatus );
				break;
			}
			else{
				printf( "Cluster Recipe is different in Lot Recipe\n");
				return SYS_ABORTED;
			}
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int		nDl , nCommStatus, nOPRStartMode, nLotRecipeCheck;
	char	szRunStr[32] , szPortID[32] , szLotRecipe[256];
	char	szBuffer[512];
	unsigned char *Ed;
	char	szFileName[256];

	memset(szRunStr,		0,		sizeof(szRunStr));
	memset(szPortID,		0,		sizeof(szPortID));
	memset(szLotRecipe,		0,		sizeof(szLotRecipe));
	memset(szFileName,		0,		sizeof(szFileName));

	Ed = PROGRAM_PARAMETER_READ();
	nDl = strlen( Ed );
	Ed[ nDl ] = 0x00;

	if ( nDl < 1 ) {
		printf( szBuffer , "Start Control Parameter Error\n");
		return SYS_ABORTED;
	}

	Sleep(500);
	STR_SEPERATE( Ed , szRunStr , szPortID , 31 );
//	printf("====> Ed : [%s]\n", Ed);
	if ( STRCMP_L( szPortID , "PORT1" ) ) {
		READ_STRING( RECIPE_FILE , szLotRecipe , &nCommStatus );
		nOPRStartMode = READ_DIGITAL(OPERATOR_START, &nCommStatus);
		if ( ON == nOPRStartMode ) {
			return SYS_SUCCESS;
		}
		nLotRecipeCheck = READ_DIGITAL(FMCM1_LOTCHECK, &nCommStatus);
		if ( NotCheck == nLotRecipeCheck ) {
			ALARM_POST(ALARM_START_POINT + Lot_Recipe_Check_Skip_Error);
			return SYS_SUCCESS;
		}
	}
	else if ( STRCMP_L( szPortID , "PORT2" ) ) {
		READ_STRING( RECIPE_FILE2 , szLotRecipe , &nCommStatus );
		nOPRStartMode = READ_DIGITAL(OPERATOR_START2, &nCommStatus);
		if ( ON == nOPRStartMode ) {
			return SYS_SUCCESS;
		}
		nLotRecipeCheck = READ_DIGITAL(FMCM2_LOTCHECK, &nCommStatus);
		if ( NotCheck == nLotRecipeCheck ) {
			ALARM_POST(ALARM_START_POINT + 300 + Lot_Recipe_Check_Skip_Error);
			return SYS_SUCCESS;
		}
	}
	else if ( STRCMP_L( szPortID , "PORT3" ) ) {
		READ_STRING( RECIPE_FILE3 , szLotRecipe , &nCommStatus );
		nOPRStartMode = READ_DIGITAL(OPERATOR_START3, &nCommStatus);
		if ( ON == nOPRStartMode ) {
			return SYS_SUCCESS;
		}
		nLotRecipeCheck = READ_DIGITAL(FMCM3_LOTCHECK, &nCommStatus);
		if ( NotCheck == nLotRecipeCheck ) {
			ALARM_POST(ALARM_START_POINT + 600 + Lot_Recipe_Check_Skip_Error);
			return SYS_SUCCESS;
		}
	}
	else {
		printf(" Start Port Name Parameter Error\n");
		return SYS_ABORTED;
	}

	if ( strlen( szLotRecipe ) <= 0 ) {
		printf( "[%s] LotRecipe file is not exist\n", szPortID );
		return SYS_ABORTED;
	}

	memset( szBuffer ,		0 ,	sizeof(szBuffer));
	
	sprintf( szFileName , "RECIPE/RUN/LOT/%s" , szLotRecipe );

	if ( !LotszRecipe_Open( szFileName,  szLotRecipe, szPortID) ) {
		printf( "[%s] Lot szRecipe File Open Error\n" , szPortID );
		return SYS_ABORTED;
	}

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
