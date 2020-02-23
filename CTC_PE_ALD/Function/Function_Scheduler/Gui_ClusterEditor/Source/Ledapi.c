#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

#include <windows.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutlanal.h>
#include <KutlFile.h>
#include <Kutltime.h>
#include <Kutlgui.h>

#include "iodefine.h"
#include "Lottype.h"
#include "Ledapi.h"
#include "appprm.h"
#include "cimseqnc.h"
#include "rcpheader.h"

//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================

CLUSTERStepTemplate			CLUSTER_RECIPE;

//=========================================================================================
void All2FileName( char *Target , char *Source , int MaxCnt ) {
	int pnt , i , j;
	j = (int) strlen( Source );
	for ( pnt = j ; pnt > 0 ; pnt-- ) if ( Source[ pnt-1 ] == '/' || Source[ pnt-1 ] == '\\' ) break;
	for ( i = pnt ; ( i < j ) && ( i-pnt < MaxCnt ); i++ ) Target[ i-pnt ] = Source[ i ];
	Target[ i-pnt ] = 0x00;
}
//
//
//
//
//

void Kor_CLUSTER_Initial() {
	int i , j;
	//-----------------------------------------------
	// 2007.09.18
	//-----------------------------------------------
	strcpy( CLUSTER_RECIPE.F_RECIPE_SWVERSION , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_ETCDATA , "" );
	//
	strcpy( CLUSTER_RECIPE.F_RECIPE_CREATE_USER , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_CREATE_DATE , "" );
	//
	strcpy( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_USER , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_LEVL , "" );
	strcpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_DATE , "" );
	//-----------------------------------------------
	CLUSTER_RECIPE.HANDLING_SIDE = 0;
	for ( i = 0 ; i < USE_CLUSTERDATA_ITEM_COUNT ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE.MODULE[i][j] = 0;
			strcpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] , "" );
			strcpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] , "" );
		}
	}
	CLUSTER_RECIPE.EXTRA_STATION = 10;		//... MAHA-DPT Default (Aligner Align:10 / Skip:50)
	CLUSTER_RECIPE.EXTRA_TIME    = 0;
}
//
//
BOOL Kor_CLUSTER_Open( char* RecipeFileName , char *groupstring ) {
	HFILE hFile;
	BOOL StepEndFind;
	int  i , j , kill;
	
	char  cc_return;
	int   ii_return;
	float ff_return;
	char  str_return[1025];
	char  name_return[1025];
	char  type_return;
	char  UserBuffer[256];

	char  Buffer[1025];
	// Initial
	Kor_CLUSTER_Initial();

	if ( RecipeFileName == NULL ) {
		_IO_CIM_PRINTF( "File %s Open Fail(File name is null)\n" , RecipeFileName );
		return TRUE;
	}
	hFile = _lopen( RecipeFileName , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "File %s Open Fail(File Error)\n" , RecipeFileName );
		return TRUE;
	}

	// INDICATOR
	if ( !read_form( hFile , Get_Prm_FILE_ID()           , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		_lclose( hFile );
		_IO_CIM_PRINTF( "File %s Open Fail(File ID Error)\n" , RecipeFileName );
		return FALSE;
	}
	if ( !read_form( hFile , Get_Prm_CLUSTER_RECIPE_ID() , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		_lclose( hFile );
		_IO_CIM_PRINTF( "File %s Open Fail(File Cluster ID Error)\n" , RecipeFileName );
		return FALSE;
	}

	//========================================================================================================
	// 2007.09.17
	//========================================================================================================
//	if ( !read_form( hFile , "__&&CLUSTER_RECIPE_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
//		_lclose( hFile );
//		_IO_CIM_PRINTF( "File %s Open Fail(File Header Error)\n" , RecipeFileName );
//		return FALSE;
//	}
	while ( TRUE ) {
		//
		if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) { _lclose( hFile ); return FALSE; }
		//
		if ( ( type_return == NAME_FORMAT ) && ( strcmp( name_return , "__&&CLUSTER_RECIPE_BEGIN&&__" ) == 0 ) ) break;
		//
		if      ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "LastUpdateUser" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "LastUpdateLevel" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "LastUpdateDate" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "Last2UpdateUser" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_USER , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "Last2UpdateLevel" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_LEVL , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "Last2UpdateDate" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_DATE , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "CreateUser" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_CREATE_USER , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "CreateLevel" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "CreateDate" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_CREATE_DATE , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "SWVersion" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_SWVERSION , str_return , 64 );
		}
		else if ( ( type_return == STRING_FORMAT ) && ( strcmp( name_return , "EtcData" ) == 0 ) ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_ETCDATA , str_return , 255 );
		}
		//
	}
	//========================================================================================================
	if ( Get_Prm_VERSION_MODE() ) {
		kill = 0;
		GET_CURRENT_USERNAME( UserBuffer );
		_RECIPE_DATA_READ_USER_HEADER( 1 , &kill , RecipeFileName , GET_SYSTEM_NAME() , PROGRAM_FUNCTION_READ() , groupstring ,
								 UserBuffer , GET_CURRENT_USERLEVEL() ,
								 CLUSTER_RECIPE.F_RECIPE_CREATE_DATE , CLUSTER_RECIPE.F_RECIPE_CREATE_USER , CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL ,
								 CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL ,
								 CLUSTER_RECIPE.F_RECIPE_SWVERSION ,
								 CLUSTER_RECIPE.F_RECIPE_ETCDATA );
	}
	//========================================================================================================
	StepEndFind = FALSE;
	kill = 0;
	while( !StepEndFind ) {
		if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) {
			_lclose( hFile );
			_IO_CIM_PRINTF( "File %s Open Fail(File Data Read Error 1)\n" , RecipeFileName );
			return FALSE;
		}
		switch ( type_return ) {
			case NAME_FORMAT :
								if ( STRCMP_L( name_return , "__&&CLUSTER_RECIPE_END&&__" ) ) StepEndFind = TRUE;
								else kill++;
								break;
			case INT_FORMAT  :	
								if      ( STRCMP_L( name_return , "HANDLING_SIDE"	) )	CLUSTER_RECIPE.HANDLING_SIDE= ii_return;
								else if ( STRCMP_L( name_return , "EXTRA_STATION"	) )	CLUSTER_RECIPE.EXTRA_STATION= ii_return;
								else if ( STRCMP_L( name_return , "EXTRA_TIME"		) )	CLUSTER_RECIPE.EXTRA_TIME	= ii_return;
								//
								else {
									strcpy( Buffer , name_return );
									Buffer[7] = 0;
									if ( STRCMP_L( Buffer , "MODULE_" ) ) {
										Buffer[0] = name_return[7];
										Buffer[1] = name_return[8];
										Buffer[2] = 0;
										i = atoi( Buffer ) - 1;
										Buffer[0] = name_return[11];
										if ( name_return[12] == 0 ) {
											Buffer[1] = 0;
										}
										else {
											Buffer[1] = name_return[12];
											Buffer[2] = 0;
										}
										j = atoi( Buffer ) - 1;
										if ( ( i < 0 ) || ( j < 0 ) || ( i >= MAX_CLUSTER_DEPTH ) || ( j >= MAX_PM_CHAMBER_DEPTH ) )
											kill++;
										else
											CLUSTER_RECIPE.MODULE[i][j]	= ii_return;
									}
									else {
										kill++;
									}
								}
								break;

			case FLOAT_FORMAT :
								kill++;
								break;

			case STRING_FORMAT :
								//============================================================================
								// 2007.06.08
								//============================================================================
								if      ( STRCMP_L( name_return , "SPECIAL_USER_DATA" ) ) { // 2007.06.08
									break;
								}
								else if ( STRCMP_L( name_return , "RECIPE_TUNE_DATA" ) ) { // 2007.06.08
									break;
								}
								//============================================================================
								Buffer[0] = name_return[7];
								Buffer[1] = name_return[8];
								Buffer[2] = 0;
								i = atoi( Buffer ) - 1;
								Buffer[0] = name_return[11];
								if ( name_return[12] == '_' ) {
									Buffer[1] = 0;
								}
								else {
									Buffer[1] = name_return[12];
									Buffer[2] = 0;
								}

								j = atoi( Buffer ) - 1;
								if ( ( i < 0 ) || ( j < 0 ) || ( i >= MAX_CLUSTER_DEPTH ) || ( j >= MAX_PM_CHAMBER_DEPTH ) ) {
									kill++;
								}
								else {
									name_return[7] = ' ';
									name_return[8] = ' ';
									name_return[11] = ' ';
									if ( name_return[12] == '_' ) {
										if ( STRCMP_L( name_return , "MODULE_  _P _PR_RECIPE" ) ) {
											strncpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] , str_return , MAX_STRING_SIZE );
										}
										else if ( STRCMP_L( name_return , "MODULE_  _P _IN_RECIPE" ) ) {
											strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] , str_return , MAX_STRING_SIZE );
										}
										else if ( STRCMP_L( name_return , "MODULE_  _P _OUT_RECIPE" ) ) {
											strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] , str_return , MAX_STRING_SIZE );
										}
										else {
											kill++;
										}
									}
									else {
										name_return[12] = ' ';
										if ( STRCMP_L( name_return , "MODULE_  _P  _PR_RECIPE" ) ) {
											strncpy( CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] , str_return , MAX_STRING_SIZE );
										}
										else if ( STRCMP_L( name_return , "MODULE_  _P  _IN_RECIPE" ) ) {
											strncpy( CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] , str_return , MAX_STRING_SIZE );
										}
										else if ( STRCMP_L( name_return , "MODULE_  _P  _OUT_RECIPE" ) ) {
											strncpy( CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] , str_return , MAX_STRING_SIZE );
										}
										else {
											kill++;
										}
									}
								}
								break;

			default :			kill++;
								break;
		}
//		if ( kill >= 100 )	{ // 2007.06.08
		if ( kill >= 10000 )	{ // 2007.06.08
			_lclose( hFile );
			_IO_CIM_PRINTF( "File %s Open Fail(File Data Error 2)\n" , RecipeFileName );
			return FALSE;
		}
	}
	_lclose( hFile );
	return TRUE;
}
//
BOOL Kor_CLUSTER_Delete( char* RecipeFileName , int renamemode , char *groupstring ) {
	char UserBuffer[256];
	//========================================================================================================
	if ( Get_Prm_VERSION_MODE() ) {
		GET_CURRENT_USERNAME( UserBuffer );
		_RECIPE_DATA_DELETE_USER_HEADER( 1 , renamemode , RecipeFileName , GET_SYSTEM_NAME() , PROGRAM_FUNCTION_READ() , groupstring ,
								 UserBuffer , GET_CURRENT_USERLEVEL() ,
								 CLUSTER_RECIPE.F_RECIPE_CREATE_DATE , CLUSTER_RECIPE.F_RECIPE_CREATE_USER , CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL ,
								 CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL ,
								 CLUSTER_RECIPE.F_RECIPE_SWVERSION ,
								 CLUSTER_RECIPE.F_RECIPE_ETCDATA );
	}
	//========================================================================================================
	DeleteFile( RecipeFileName );
	//========================================================================================================
	return TRUE;
}

//
BOOL Kor_CLUSTER_Save( char* RecipeFileName , int newmode , char *groupstring ) {
	HFILE hFile;
	int i , j , ChamberLength = 0 , LevelBuffer;
	char Buffer[64];
	char UserBuffer[64];
	char TimeBuffer[64];
	SYSTEMTIME SysTime; // 2004.11.08

	if ( RecipeFileName == NULL ) return FALSE;
	hFile = _lcreat( RecipeFileName , 0 );
	if ( hFile == -1 ) return FALSE;

	for ( i = 0 ; i < USE_CLUSTERDATA_ITEM_COUNT ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( CLUSTER_RECIPE.MODULE[i][j] ) {
				if ( ChamberLength < j + 1 ) ChamberLength = j + 1;
			}
		}
	}
	if ( ChamberLength < 12 ) ChamberLength = 12;

	// INDICATOR
	write_nam_form( hFile , Get_Prm_FILE_ID() );
	write_nam_form( hFile , Get_Prm_CLUSTER_RECIPE_ID() );

	//======================================================================================
	// 2007.09.18
	//======================================================================================
	if ( Get_Prm_VERSION_MODE() ) {
		//
		write_nam_form( hFile , "__&&CLUSTER_HEADER_BEGIN&&__" );
		//------------------------------
		GET_CURRENT_USERNAME( UserBuffer );
		GetLocalTime( &SysTime );
		sprintf( TimeBuffer , "%d/%02d/%02d %02d:%02d:%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
		LevelBuffer = GET_CURRENT_USERLEVEL();
		//------------------------------------------------------------------------------------------
		strncpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_USER , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER , 64 );
		strncpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_LEVL , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL , 64 );
		strncpy( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_DATE , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE , 64 );
		//------------------------------------------------------------------------------------------
		_RECIPE_DATA_SAVE_USER_HEADER( 1 , newmode ,
					 	 	 RecipeFileName , GET_SYSTEM_NAME() , PROGRAM_FUNCTION_READ() , groupstring ,
							 UserBuffer , LevelBuffer ,
							 CLUSTER_RECIPE.F_RECIPE_CREATE_DATE , CLUSTER_RECIPE.F_RECIPE_CREATE_USER , CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL ,
							 CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL ,
							 CLUSTER_RECIPE.F_RECIPE_SWVERSION ,
							 CLUSTER_RECIPE.F_RECIPE_ETCDATA );
		//------------------------------------------------------------------------------------------
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_CREATE_DATE ) <= 0 ) {
			strncpy( CLUSTER_RECIPE.F_RECIPE_CREATE_USER , UserBuffer , 64 );
			sprintf( CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL , "%d" , LevelBuffer );
			strncpy( CLUSTER_RECIPE.F_RECIPE_CREATE_DATE , TimeBuffer , 64 );
		}
		//------------------------------------------------------------------------------------------
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_CREATE_USER ) > 0 )		write_str_form( hFile , "CreateUser"   , CLUSTER_RECIPE.F_RECIPE_CREATE_USER );
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL ) > 0 )		write_str_form( hFile , "CreateLevel"  , CLUSTER_RECIPE.F_RECIPE_CREATE_LEVL );
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_CREATE_DATE ) > 0 )		write_str_form( hFile , "CreateDate"   , CLUSTER_RECIPE.F_RECIPE_CREATE_DATE );
		//
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER ) > 0 )	write_str_form( hFile , "LastUpdateUser"   , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_USER );
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL ) > 0 )	write_str_form( hFile , "LastUpdateLevel"  , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_LEVL );
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE ) > 0 )	write_str_form( hFile , "LastUpdateDate"   , CLUSTER_RECIPE.F_RECIPE_LAST_UPDATE_DATE );
		//
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_USER ) > 0 )	write_str_form( hFile , "Last2UpdateUser"  , CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_USER );
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_LEVL ) > 0 )	write_str_form( hFile , "Last2UpdateLevel" , CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_LEVL );
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_DATE ) > 0 )	write_str_form( hFile , "Last2UpdateDate"  , CLUSTER_RECIPE.F_RECIPE_LAST2_UPDATE_DATE );
		//
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_SWVERSION ) > 0 )			write_str_form( hFile , "SWVersion"   , CLUSTER_RECIPE.F_RECIPE_SWVERSION );
		if ( strlen( CLUSTER_RECIPE.F_RECIPE_ETCDATA ) > 0 )			write_str_form( hFile , "EtcData"     , CLUSTER_RECIPE.F_RECIPE_ETCDATA );
		//------------------------------------------------------------------------------------------
		write_nam_form( hFile , "__&&CLUSTER_HEADER_END&&__" );
	}
	//======================================================================================
	// STEP
	write_nam_form( hFile , "__&&CLUSTER_RECIPE_BEGIN&&__" );

	//================================================================================================
	// 2004.11.08
	//================================================================================================
	GetLocalTime( &SysTime );
	sprintf( Buffer , "%04d%02d%02d%02d%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
		SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds/10 );
	write_int_form( hFile , "RECIPE_SAVE_USER" , 0 );
	write_str_form( hFile , "RECIPE_SAVE_TIME" , Buffer );
	//================================================================================================
	write_int_form( hFile , "HANDLING_SIDE"	, CLUSTER_RECIPE.HANDLING_SIDE );
	for ( i = 0 ; i < USE_CLUSTERDATA_ITEM_COUNT ; i++ ) {
		for ( j = 0 ; j < ChamberLength ; j++ ) {
			sprintf( Buffer , "MODULE_%02d_P%d" , i + 1 , j + 1 );
			write_int_form( hFile , Buffer	, CLUSTER_RECIPE.MODULE[i][j] );
			sprintf( Buffer , "MODULE_%02d_P%d_PR_RECIPE" , i + 1 , j + 1 );
			write_str_form( hFile , Buffer , CLUSTER_RECIPE.MODULE_PR_RECIPE[i][j] );
			sprintf( Buffer , "MODULE_%02d_P%d_IN_RECIPE" , i + 1 , j + 1 );
			write_str_form( hFile , Buffer , CLUSTER_RECIPE.MODULE_IN_RECIPE[i][j] );
			sprintf( Buffer , "MODULE_%02d_P%d_OUT_RECIPE" , i + 1 , j + 1 );
			write_str_form( hFile , Buffer , CLUSTER_RECIPE.MODULE_OUT_RECIPE[i][j] );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if(CLUSTER_RECIPE.EXTRA_STATION != 10 && CLUSTER_RECIPE.EXTRA_STATION != 50)
	{
		printf("[CLUSTER_RECIPE] Forced Changed CLUSTER_RECIPE.EXTRA_STATION[%d] -> 10 \n", CLUSTER_RECIPE.EXTRA_STATION);
		CLUSTER_RECIPE.EXTRA_STATION = 10;
	}
	//////////////////////////////////////////////////////////////////////////

	write_int_form( hFile , "EXTRA_STATION"		, CLUSTER_RECIPE.EXTRA_STATION );
	write_int_form( hFile , "EXTRA_TIME"		, CLUSTER_RECIPE.EXTRA_TIME );
	write_nam_form( hFile , "__&&CLUSTER_RECIPE_END&&__" );
	_lclose( hFile );
	return TRUE;
}
