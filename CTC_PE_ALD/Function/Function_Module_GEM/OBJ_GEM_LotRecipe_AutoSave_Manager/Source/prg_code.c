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
#include <string.h>
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
#include "Appprm.h"
//------------------------------------------------------------------------------------------
#include <io.h>
#include <conio.h>
//------------------------------------------------------------------------------------------
extern int SPECIAL_ENUM_COUNT;
//------------------------------------------------------------------------------------------
#define	MAX_LOTDATA_ITEM_COUNT			30
#define MAX_CASTSLOT_COUNT				30
#define PPARMTX_LENGTH					64
#define ABORT_RECIPE_KEEP_DATE			15
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef struct {
	char	LOT_ID[PPARMTX_LENGTH];
	char	CLUSTER_RECIPE[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	char	CLUSTER_RECIPE2[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	int		CLUSTER_USING[MAX_CASTSLOT_COUNT];
	int		CLUSTER_DUMMY_USING;
	char	CLUSTER_DUMMY_RECIPE[PPARMTX_LENGTH+1];
	int		SPECIAL_INSIDE_DATA;
} LOTStepTemplate;
LOTStepTemplate		LOT_RECIPE;

int  SPECIAL_ENUM_COUNT = { 0 ,};

enum    { Unknown, Absent, Present, Process,  Failure };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
//	{ "SCHEDULER"					, _K_F_IO	,	F_SCHEDULER			,	0	} ,
	{ "e:SCHEDULER"					, _K_S_IO	,	S_SCHEDULER			,	0	} ,

	{ "CM1.C01_Wafer"				, _K_D_IO	,	CM1_C01_Wafer		,	0	} ,
	{ "CM1.C02_Wafer"				, _K_D_IO	,	CM1_C02_Wafer		,	0	} ,
	{ "CM1.C03_Wafer"				, _K_D_IO	,	CM1_C03_Wafer		,	0	} ,
	{ "CM1.C04_Wafer"				, _K_D_IO	,	CM1_C04_Wafer		,	0	} ,
	{ "CM1.C05_Wafer"				, _K_D_IO	,	CM1_C05_Wafer		,	0	} ,
	{ "CM1.C06_Wafer"				, _K_D_IO	,	CM1_C06_Wafer		,	0	} ,
	{ "CM1.C07_Wafer"				, _K_D_IO	,	CM1_C07_Wafer		,	0	} ,
	{ "CM1.C08_Wafer"				, _K_D_IO	,	CM1_C08_Wafer		,	0	} ,
	{ "CM1.C09_Wafer"				, _K_D_IO	,	CM1_C09_Wafer		,	0	} ,
	{ "CM1.C10_Wafer"				, _K_D_IO	,	CM1_C10_Wafer		,	0	} ,
	{ "CM1.C11_Wafer"				, _K_D_IO	,	CM1_C11_Wafer		,	0	} ,
	{ "CM1.C12_Wafer"				, _K_D_IO	,	CM1_C12_Wafer		,	0	} ,
	{ "CM1.C13_Wafer"				, _K_D_IO	,	CM1_C13_Wafer		,	0	} ,
	{ "CM1.C14_Wafer"				, _K_D_IO	,	CM1_C14_Wafer		,	0	} ,
	{ "CM1.C15_Wafer"				, _K_D_IO	,	CM1_C15_Wafer		,	0	} ,
	{ "CM1.C16_Wafer"				, _K_D_IO	,	CM1_C16_Wafer		,	0	} ,
	{ "CM1.C17_Wafer"				, _K_D_IO	,	CM1_C17_Wafer		,	0	} ,
	{ "CM1.C18_Wafer"				, _K_D_IO	,	CM1_C18_Wafer		,	0	} ,
	{ "CM1.C19_Wafer"				, _K_D_IO	,	CM1_C19_Wafer		,	0	} ,
	{ "CM1.C20_Wafer"				, _K_D_IO	,	CM1_C20_Wafer		,	0	} ,
	{ "CM1.C21_Wafer"				, _K_D_IO	,	CM1_C21_Wafer		,	0	} ,
	{ "CM1.C22_Wafer"				, _K_D_IO	,	CM1_C22_Wafer		,	0	} ,
	{ "CM1.C23_Wafer"				, _K_D_IO	,	CM1_C23_Wafer		,	0	} ,
	{ "CM1.C24_Wafer"				, _K_D_IO	,	CM1_C24_Wafer		,	0	} ,
	{ "CM1.C25_Wafer"				, _K_D_IO	,	CM1_C25_Wafer		,	0	} ,
	{ "CM1.C26_Wafer"				, _K_D_IO	,	CM1_C26_Wafer		,	0	} ,
	{ "CM1.C27_Wafer"				, _K_D_IO	,	CM1_C27_Wafer		,	0	} ,
	{ "CM1.C28_Wafer"				, _K_D_IO	,	CM1_C28_Wafer		,	0	} ,
	{ "CM1.C29_Wafer"				, _K_D_IO	,	CM1_C29_Wafer		,	0	} ,
	{ "CM1.C30_Wafer"				, _K_D_IO	,	CM1_C30_Wafer		,	0	} ,

	{ "CM2.C01_Wafer"				, _K_D_IO	,	CM2_C01_Wafer		,	0	} ,
	{ "CM2.C02_Wafer"				, _K_D_IO	,	CM2_C02_Wafer		,	0	} ,
	{ "CM2.C03_Wafer"				, _K_D_IO	,	CM2_C03_Wafer		,	0	} ,
	{ "CM2.C04_Wafer"				, _K_D_IO	,	CM2_C04_Wafer		,	0	} ,
	{ "CM2.C05_Wafer"				, _K_D_IO	,	CM2_C05_Wafer		,	0	} ,
	{ "CM2.C06_Wafer"				, _K_D_IO	,	CM2_C06_Wafer		,	0	} ,
	{ "CM2.C07_Wafer"				, _K_D_IO	,	CM2_C07_Wafer		,	0	} ,
	{ "CM2.C08_Wafer"				, _K_D_IO	,	CM2_C08_Wafer		,	0	} ,
	{ "CM2.C09_Wafer"				, _K_D_IO	,	CM2_C09_Wafer		,	0	} ,
	{ "CM2.C10_Wafer"				, _K_D_IO	,	CM2_C10_Wafer		,	0	} ,
	{ "CM2.C11_Wafer"				, _K_D_IO	,	CM2_C11_Wafer		,	0	} ,
	{ "CM2.C12_Wafer"				, _K_D_IO	,	CM2_C12_Wafer		,	0	} ,
	{ "CM2.C13_Wafer"				, _K_D_IO	,	CM2_C13_Wafer		,	0	} ,
	{ "CM2.C14_Wafer"				, _K_D_IO	,	CM2_C14_Wafer		,	0	} ,
	{ "CM2.C15_Wafer"				, _K_D_IO	,	CM2_C15_Wafer		,	0	} ,
	{ "CM2.C16_Wafer"				, _K_D_IO	,	CM2_C16_Wafer		,	0	} ,
	{ "CM2.C17_Wafer"				, _K_D_IO	,	CM2_C17_Wafer		,	0	} ,
	{ "CM2.C18_Wafer"				, _K_D_IO	,	CM2_C18_Wafer		,	0	} ,
	{ "CM2.C19_Wafer"				, _K_D_IO	,	CM2_C19_Wafer		,	0	} ,
	{ "CM2.C20_Wafer"				, _K_D_IO	,	CM2_C20_Wafer		,	0	} ,
	{ "CM2.C21_Wafer"				, _K_D_IO	,	CM2_C21_Wafer		,	0	} ,
	{ "CM2.C22_Wafer"				, _K_D_IO	,	CM2_C22_Wafer		,	0	} ,
	{ "CM2.C23_Wafer"				, _K_D_IO	,	CM2_C23_Wafer		,	0	} ,
	{ "CM2.C24_Wafer"				, _K_D_IO	,	CM2_C24_Wafer		,	0	} ,
	{ "CM2.C25_Wafer"				, _K_D_IO	,	CM2_C25_Wafer		,	0	} ,
	{ "CM2.C26_Wafer"				, _K_D_IO	,	CM2_C26_Wafer		,	0	} ,
	{ "CM2.C27_Wafer"				, _K_D_IO	,	CM2_C27_Wafer		,	0	} ,
	{ "CM2.C28_Wafer"				, _K_D_IO	,	CM2_C28_Wafer		,	0	} ,
	{ "CM2.C29_Wafer"				, _K_D_IO	,	CM2_C29_Wafer		,	0	} ,
	{ "CM2.C30_Wafer"				, _K_D_IO	,	CM2_C30_Wafer		,	0	} ,

	{ "CM3.C01_Wafer"				, _K_D_IO	,	CM3_C01_Wafer		,	0	} ,
	{ "CM3.C02_Wafer"				, _K_D_IO	,	CM3_C02_Wafer		,	0	} ,
	{ "CM3.C03_Wafer"				, _K_D_IO	,	CM3_C03_Wafer		,	0	} ,
	{ "CM3.C04_Wafer"				, _K_D_IO	,	CM3_C04_Wafer		,	0	} ,
	{ "CM3.C05_Wafer"				, _K_D_IO	,	CM3_C05_Wafer		,	0	} ,
	{ "CM3.C06_Wafer"				, _K_D_IO	,	CM3_C06_Wafer		,	0	} ,
	{ "CM3.C07_Wafer"				, _K_D_IO	,	CM3_C07_Wafer		,	0	} ,
	{ "CM3.C08_Wafer"				, _K_D_IO	,	CM3_C08_Wafer		,	0	} ,
	{ "CM3.C09_Wafer"				, _K_D_IO	,	CM3_C09_Wafer		,	0	} ,
	{ "CM3.C10_Wafer"				, _K_D_IO	,	CM3_C10_Wafer		,	0	} ,
	{ "CM3.C11_Wafer"				, _K_D_IO	,	CM3_C11_Wafer		,	0	} ,
	{ "CM3.C12_Wafer"				, _K_D_IO	,	CM3_C12_Wafer		,	0	} ,
	{ "CM3.C13_Wafer"				, _K_D_IO	,	CM3_C13_Wafer		,	0	} ,
	{ "CM3.C14_Wafer"				, _K_D_IO	,	CM3_C14_Wafer		,	0	} ,
	{ "CM3.C15_Wafer"				, _K_D_IO	,	CM3_C15_Wafer		,	0	} ,
	{ "CM3.C16_Wafer"				, _K_D_IO	,	CM3_C16_Wafer		,	0	} ,
	{ "CM3.C17_Wafer"				, _K_D_IO	,	CM3_C17_Wafer		,	0	} ,
	{ "CM3.C18_Wafer"				, _K_D_IO	,	CM3_C18_Wafer		,	0	} ,
	{ "CM3.C19_Wafer"				, _K_D_IO	,	CM3_C19_Wafer		,	0	} ,
	{ "CM3.C20_Wafer"				, _K_D_IO	,	CM3_C20_Wafer		,	0	} ,
	{ "CM3.C21_Wafer"				, _K_D_IO	,	CM3_C21_Wafer		,	0	} ,
	{ "CM3.C22_Wafer"				, _K_D_IO	,	CM3_C22_Wafer		,	0	} ,
	{ "CM3.C23_Wafer"				, _K_D_IO	,	CM3_C23_Wafer		,	0	} ,
	{ "CM3.C24_Wafer"				, _K_D_IO	,	CM3_C24_Wafer		,	0	} ,
	{ "CM3.C25_Wafer"				, _K_D_IO	,	CM3_C25_Wafer		,	0	} ,
	{ "CM3.C26_Wafer"				, _K_D_IO	,	CM3_C26_Wafer		,	0	} ,
	{ "CM3.C27_Wafer"				, _K_D_IO	,	CM3_C27_Wafer		,	0	} ,
	{ "CM3.C28_Wafer"				, _K_D_IO	,	CM3_C28_Wafer		,	0	} ,
	{ "CM3.C29_Wafer"				, _K_D_IO	,	CM3_C29_Wafer		,	0	} ,
	{ "CM3.C30_Wafer"				, _K_D_IO	,	CM3_C30_Wafer		,	0	} ,

	{ "CTC.Recipe_FILE"				, _K_S_IO	,	RECIPE_FILE			,	0	} ,
	{ "CTC.Recipe_FILE2"			, _K_S_IO	,	RECIPE_FILE2		,	0	} ,
	{ "CTC.Recipe_FILE3"			, _K_S_IO	,	RECIPE_FILE3		,	0	} ,
    { "CTC.OPERATOR_START"			, _K_D_IO	,	OPERATOR_START      ,	0   } ,  
    { "CTC.OPERATOR_START2"			, _K_D_IO	,	OPERATOR_START2     ,	0   } ,  
    { "CTC.OPERATOR_START3"			, _K_D_IO	,	OPERATOR_START3     ,	0   } ,
	{ "PRM_FMCM1_LOTCHECK"			, _K_D_IO	,	FMCM1_LOTCHECK		,	0   } ,
	{ "PRM_FMCM2_LOTCHECK"			, _K_D_IO	,	FMCM2_LOTCHECK		,	0   } ,
	{ "PRM_FMCM3_LOTCHECK"			, _K_D_IO	,	FMCM3_LOTCHECK		,	0   } ,
	{ "CTC.JOB_NAME"				, _K_S_IO	,	JOB_NAME			,	0   } ,
	{ "CTC.JOB_NAME2"				, _K_S_IO	,	JOB_NAME2			,	0   } ,
	{ "CTC.JOB_NAME3"				, _K_S_IO	,	JOB_NAME3			,	0   } ,
	{ "CTC.MID_NAME"				, _K_S_IO	,	MID_NAME			,	0   } ,
	{ "CTC.MID_NAME2"				, _K_S_IO	,	MID_NAME2			,	0   } ,
	{ "CTC.MID_NAME3"				, _K_S_IO	,	MID_NAME3			,	0   } ,
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
	LOT_RECIPE.SPECIAL_INSIDE_DATA = 0;
}
//------------------------------------------------------------------------------------------
BOOL LotszRecipe_OpenSave( char *OpenName, char *szNewFileName,	char *szLotRecipe , char *szPortID, LOTStepTemplate LOT_RECIPE, char *szFilePath, char *szNewLotRecipe) {
	HFILE	hFile, hNewFile;
	BOOL	bStepEndFind;
	int		nIndex, nWaferState, nkill , ii_return, i, nCommStatus;
	float	ff_return;
	char	str_return[256] , name_return[256] , type_return , cc_return;
	char	szJobName[256], szBuffer[256];
	SYSTEMTIME SysTime;

	memset(str_return,		0,		sizeof(str_return));
	memset(name_return,		0,		sizeof(name_return));
	memset(szJobName,		0,		sizeof(szJobName));
	memset(szBuffer,		0,		sizeof(szBuffer));

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
								else if ( STRCMP_L( name_return , "SPECIAL_INSIDE_DATA"	) )	LOT_RECIPE.SPECIAL_INSIDE_DATA	= ii_return;
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

	if( STRCMP_L( szPortID , "PORT1" ) )		nIndex = CM1_C01_Wafer;
	else if ( STRCMP_L( szPortID , "PORT2" ) )  nIndex = CM2_C01_Wafer;
	else if ( STRCMP_L( szPortID , "PORT3" ) )  nIndex = CM3_C01_Wafer;

	hNewFile = _lcreat( szNewFileName, 0 );
	// INDICATOR
	write_nam_form( hNewFile , "" );
	write_nam_form( hNewFile , "" );

	// STEP
	write_nam_form( hNewFile , "__&&LOT_RECIPE_BEGIN&&__" );

	//================================================================================================
	// 2004.11.08
	//================================================================================================
	GetLocalTime( &SysTime );
	sprintf( szBuffer , "%04d%02d%02d%02d%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
		SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds/10 );
	write_int_form( hNewFile , "RECIPE_SAVE_USER" , 0 );
	write_str_form( hNewFile , "RECIPE_SAVE_TIME" , szBuffer );
	//================================================================================================

	write_str_form( hNewFile , "LOT_ID"				, LOT_RECIPE.LOT_ID );

	for ( i = 0 ; i < MAX_LOTDATA_ITEM_COUNT ; i++ ) {
		nWaferState = READ_DIGITAL( nIndex+i, &nCommStatus );
		if ( Present == nWaferState ) {
			sprintf( szBuffer , "CLUSTER_%02d_USING" , i + 1 );
			write_int_form( hNewFile , szBuffer , LOT_RECIPE.CLUSTER_USING[i]  );
			sprintf( szBuffer , "CLUSTER_%02d_RECIPE" , i + 1 );
			write_str_form( hNewFile , szBuffer , LOT_RECIPE.CLUSTER_RECIPE[i] );
			sprintf( szBuffer , "CLUSTER_%02d_RECIPE2" , i + 1 );
			write_str_form( hNewFile , szBuffer , LOT_RECIPE.CLUSTER_RECIPE2[i] );	
		}
		else {
			sprintf( szBuffer , "CLUSTER_%02d_USING" , i + 1 );
			write_int_form( hNewFile , szBuffer , 0  );
			sprintf( szBuffer , "CLUSTER_%02d_RECIPE" , i + 1 );
			write_str_form( hNewFile , szBuffer , "" );
			sprintf( szBuffer , "CLUSTER_%02d_RECIPE2" , i + 1 );
			write_str_form( hNewFile , szBuffer , "" );
		}
	}
	write_int_form( hNewFile , "CLUSTER_DUMMY_USING"  , LOT_RECIPE.CLUSTER_DUMMY_USING );
	write_str_form( hNewFile , "CLUSTER_DUMMY_RECIPE" , LOT_RECIPE.CLUSTER_DUMMY_RECIPE );

//	if ( SPECIAL_ENUM_COUNT > 0 ) { // 2004.11.16
		write_int_form( hNewFile , "SPECIAL_INSIDE_DATA" , LOT_RECIPE.SPECIAL_INSIDE_DATA ); // 2004.11.16
//	}
	//
	write_nam_form( hNewFile , "__&&LOT_RECIPE_END&&__" );
	_lclose( hNewFile );


	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int		nDl , nCommStatus, nOPRStartMode;
	char	szRunStr[32] , szPortID[32] , szLotRecipe[256], szNewLotRecipe[256];
	char	szBuffer[512];
	unsigned char *Ed;
	char	szFileName[256];
	char	szNewFileName[256];
	char	szFilePath[256];
	SYSTEMTIME SysTime;

	struct _finddata_t c_file;
	long hFile;
	char szAbortedLotMID[64], szAbortedLotID[64], szAbortedDate[64], szElseString[64];
	int ndate1, ndate2;
	char szCurrentDate[8], szDeleteFileName[64], szEvnetString[128];

	memset(szRunStr,				0,		sizeof(szRunStr));
	memset(szPortID,				0,		sizeof(szPortID));
	memset(szLotRecipe,				0,		sizeof(szLotRecipe));
	memset(szFileName,				0,		sizeof(szFileName));
	memset(szNewFileName,			0,		sizeof(szNewFileName));
	memset(szNewLotRecipe,			0,		sizeof(szNewLotRecipe));
	memset(szFilePath,				0,		sizeof(szFilePath));
	memset(szBuffer,				0,		sizeof(szBuffer));
	memset(szAbortedLotMID,			0,		sizeof(szAbortedLotMID));
	memset(szAbortedLotID,			0,		sizeof(szAbortedLotID));
	memset(szAbortedDate,			0,		sizeof(szAbortedDate));
	memset(szElseString,			0,		sizeof(szElseString));
	memset(szCurrentDate,			0,		sizeof(szCurrentDate));
	memset(szDeleteFileName,		0,		sizeof(szDeleteFileName));
	memset(szEvnetString,			0,		sizeof(szEvnetString));

	Ed = PROGRAM_PARAMETER_READ();
	nDl = strlen( Ed );
	Ed[ nDl ] = 0x00;

	if ( nDl < 1 ) {
//		printf( szBuffer , "Start Control Parameter Error\n");
		return SYS_ABORTED;
	}

	//================================================================================================
	GetLocalTime( &SysTime );
	//================================================================================================

	Sleep(500);
	STR_SEPERATE( Ed , szRunStr , szPortID , 31 );
//	printf("====> Ed : [%s]\n", Ed);
	if ( STRCMP_L( szPortID , "PORT1" ) ) {
		READ_STRING( RECIPE_FILE , szLotRecipe , &nCommStatus );
		READ_STRING( MID_NAME , szBuffer , &nCommStatus );
		sprintf( szNewLotRecipe, "%s_%04d%02d%02d_Aborted", szBuffer, SysTime.wYear, SysTime.wMonth, SysTime.wDay);
		nOPRStartMode = READ_DIGITAL(OPERATOR_START, &nCommStatus);
		if ( ON == nOPRStartMode ) {
			return SYS_SUCCESS;
		}
	}
	else if ( STRCMP_L( szPortID , "PORT2" ) ) {
		READ_STRING( RECIPE_FILE2 , szLotRecipe , &nCommStatus );
		READ_STRING( MID_NAME2 , szBuffer , &nCommStatus );
		sprintf( szNewLotRecipe, "%s_%04d%02d%02d_Aborted", szBuffer, SysTime.wYear, SysTime.wMonth, SysTime.wDay);
		nOPRStartMode = READ_DIGITAL(OPERATOR_START2, &nCommStatus);
		if ( ON == nOPRStartMode ) {
			return SYS_SUCCESS;
		}
	}
	else if ( STRCMP_L( szPortID , "PORT3" ) ) {
		READ_STRING( RECIPE_FILE3 , szLotRecipe , &nCommStatus );
		READ_STRING( MID_NAME3 , szBuffer , &nCommStatus );
		sprintf( szNewLotRecipe, "%s_%04d%02d%02d_Aborted", szBuffer, SysTime.wYear, SysTime.wMonth, SysTime.wDay);
		nOPRStartMode = READ_DIGITAL(OPERATOR_START3, &nCommStatus);
		if ( ON == nOPRStartMode ) {
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

	if ( strlen( szNewLotRecipe ) <= 0 ) {
		printf( "[%s] MID Name is NULL\n", szPortID );
		return SYS_ABORTED;
	}

	strcpy( szFilePath, "RECIPE/RUN/LOT/");
	sprintf( szFileName , "RECIPE/RUN/LOT/%s" , szLotRecipe );
	sprintf( szNewFileName, "RECIPE/RUN/LOT/%s" , szNewLotRecipe );

	if ( !LotszRecipe_OpenSave( szFileName,  szNewFileName,	szLotRecipe, szPortID, LOT_RECIPE, szFilePath, szNewLotRecipe) ) {
		printf( "[%s] Lot szRecipe File Open Error\n" , szPortID );
		return SYS_ABORTED;
	}
//------------------------------------------------------------------------------------------
	SetCurrentDirectory("RECIPE/RUN/LOT");
	if( ( hFile = _findfirst( "*_Aborted.*", & c_file ) ) == -1L )
        printf( "No files in current directory!\n" );
    else
    {
        do 
        {
//            printf( " %-12s %.24s %9ld\n", c_file.name, ctime( &( c_file.time_write ) ), c_file.size );
			STR_SEPERATE_CHAR( c_file.name, '_', szAbortedLotID , szElseString , 63 );
			STR_SEPERATE_CHAR( szElseString, '_', szAbortedDate , szElseString , 63 );
			sprintf( szCurrentDate, "%04d%02d%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
			ndate1 = (atoi(szCurrentDate) - atoi(szAbortedDate)) / 30;
			ndate2 = (atoi(szCurrentDate) - atoi(szAbortedDate)) % 30;
//			printf("=== Date is [%d]\n", ndate1*30+ndate2);
			if( (ndate1 > 0) || (ndate2 > ABORT_RECIPE_KEEP_DATE) ) {
				DeleteFile(c_file.name);
				sprintf( szEvnetString , "EVENT_ON RECIPE_EDIT LOT|%s|%d|%d" , c_file.name /*FileName*/ , 0 /*Step for Process*/ , 2 /*Delete*/ );
				WRITE_STRING( S_SCHEDULER , szEvnetString , &nCommStatus );
			}		
        }while( _findnext( hFile, &c_file ) == 0 );
        _findclose( hFile );
    }
//------------------------------------------------------------------------------------------
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
