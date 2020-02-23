#include <stdio.h>
#include <process.h>
#include <conio.h>
#include <time.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <KutlFile.h>

#include <cimseqnc.h>

#include "io.h"
#include "iodefine.h"

#include "DataType.h"
#include "App_Util.h"
//===============================================================================
int	     g_LineCount	=	0;
int		 g_ItemIndex	=	0;
//===============================================================================

//============================================================================================
int	 GET_CURRENT_LINE() { return g_LineCount; }
void SET_CURRENT_LINE( int LineCount ) { g_LineCount = LineCount; }
void INC_CURRENT_LINE() { g_LineCount++; }
void RESET_CURRENT_LINE() { g_LineCount = 0; }

int  GET_ITEM_INDEX() { return g_ItemIndex; }
void SET_ITEM_INDEX( int index ) { g_ItemIndex = index; }
void INC_ITEM_INDEX() { g_ItemIndex++; }
void RESET_ITEM_INDEX(){ g_ItemIndex = 0; }
//============================================================================================

extern LOG_ITEM  LOG_DATA_LIST[MAX_LOG_ITEM];

BOOL CHECK_DIRECTORY( char* szPath )
{
	int		result;

	char	szDrive[5];
	char	szDirectory[MAX_PATH];
	char	szFileName[MAX_PATH];
	char	szExtension[10];
	char	szCheckPath[MAX_PATH];


	_splitpath( szPath, szDrive, szDirectory, szFileName, szExtension );
	sprintf( szCheckPath, "%s%s", szDrive, szDirectory );
	result = _access( szCheckPath, 0 );

	if ( result == -1 )		return FALSE;

	return TRUE;		
}

void Init_Buffer( void )
{
	RESET_ITEM_INDEX();
	memset( LOG_DATA_LIST, 0, sizeof( LOG_ITEM ) * MAX_LOG_ITEM );
}

BOOL Page_Process( char* pszData ) 
{
	return FALSE;
}

BOOL Param_Process( char* szParam )
{
	char	MAIN_BUF[MAX_PATH*2];
	char	MAIN_BUF2[MAX_PATH*2];
	char	szData[1024];
	char	szData2[1024];
	char	szData3[1024];

	char	szMark[5];
	char	szDate[10];

	STR_SEPERATE_CHAR( szParam, '#', MAIN_BUF, MAIN_BUF2, (MAX_PATH*2) );
	STR_SEPERATE_CHAR( MAIN_BUF, '"', szData, szData3, MAX_PATH );
	STR_SEPERATE_CHAR( szData3, '"',  szData, szData2, MAX_PATH );

	REMOVE_STR_HEAD_BLANK( szData2 );
	REMOVE_STR_TAIL_BLANK( szData2 );

	// Directory 존재 확인
	if ( CHECK_DIRECTORY( szData ) == FALSE )
	{
		_IO_COMMON_PRINTF( TRUE, "DEL", "DEL", "[ERROR] Invalid Directory : %s\n", szData );
		_IO_COMMON_PRINTF( TRUE, "EV", "DEL", "[ERROR] Invalid Directory : %s\n", szData );
		return FALSE;
	}

	strcpy( LOG_DATA_LIST[ GET_ITEM_INDEX() ].szLogFilePath, szData  );
//	strcpy( LOG_DATA_LIST[ GET_ITEM_INDEX() ].szDelCondition, szData2 );
	// Condition Convert

	STR_SEPERATE_CHAR( szData2, ':', szMark, szDate, 15 );

	if ( strlen( szMark ) > 1 ) {
		if		(( szMark[0] == 'f' ) || (  szMark[0] == 'F' )) {
			LOG_DATA_LIST[ GET_ITEM_INDEX() ].IsFolder = TRUE;
		}

		if		( stricmp(&szMark[1],"Y") == 0 )	LOG_DATA_LIST[ GET_ITEM_INDEX() ].dwDelCondition = atoi( szDate ) * 365;	
		else if	( stricmp(&szMark[1],"M") == 0 )	LOG_DATA_LIST[ GET_ITEM_INDEX() ].dwDelCondition = atoi( szDate ) * 30;	
		else if ( stricmp(&szMark[1],"D") == 0 )	LOG_DATA_LIST[ GET_ITEM_INDEX() ].dwDelCondition = atoi( szDate );	
		else										return FALSE;

	}
	else if ( strlen( szMark ) == 1 ) {
		if		( stricmp(szMark,"Y") == 0 )	LOG_DATA_LIST[ GET_ITEM_INDEX() ].dwDelCondition = atoi( szDate ) * 365;	
		else if	( stricmp(szMark,"M") == 0 )	LOG_DATA_LIST[ GET_ITEM_INDEX() ].dwDelCondition = atoi( szDate ) * 30;	
		else if ( stricmp(szMark,"D") == 0 )	LOG_DATA_LIST[ GET_ITEM_INDEX() ].dwDelCondition = atoi( szDate );	
		else									return FALSE;
	}
	else {
		_IO_COMMON_PRINTF( TRUE, "DEL", "DEL", "[ERROR] Invalid Delete Option : %s\n", szData );
		return FALSE;
	}


	INC_ITEM_INDEX();

	return TRUE;
}


BOOL Process_Def_Data( char *pszData )
{
	INC_CURRENT_LINE();

	REMOVE_STR_TAIL_BLANK( pszData );

	if		( !strlen( pszData ) )			return TRUE;	// 줄 바꿈만 정의 되어 있는 경우...

	if		( pszData[0] == '#' )			return TRUE;
	else if	( pszData[0] == ' ' )			return TRUE;
	else if	( pszData[0] == '\t' )			return TRUE;
	else if	( pszData[0] == '\n' )			return TRUE;
	else 
	{
			if ( strncmp( "$$$", pszData, 3 ) == 0 )	return ( Page_Process( pszData ) );
			else										return ( Param_Process( pszData ) );
	}

	return TRUE;
}


BOOL Read_Ini_File( char* pszFileName )
{
	char	ch= ' ';
	UINT	uBufCount;

	FILE*	fi;

	char	szLineBuf[ 512 ];

	uBufCount	= 0;
	
	Init_Buffer();

	fi = fopen( pszFileName, "rt" );

	if ( fi == NULL )
	{
		_IO_COMMON_PRINTF( TRUE, "DEL", "DEL", "[ERROR] \"%s\" File not Found\n", pszFileName );
		_IO_COMMON_PRINTF( TRUE, "EV", "DEL", "[ERROR] \"%s\" File not Found\n", pszFileName );

		return FALSE;
	}

	RESET_CURRENT_LINE();

	memset( szLineBuf, 0, sizeof( char) * 512 );
	
	while ( fgets( szLineBuf, 512, fi ) != NULL )
	{
		if ( Process_Def_Data( szLineBuf ) == FALSE	) {
			_IO_COMMON_PRINTF( TRUE, "DEL", "DEL", "[DEV Monitor] Configuration File Error : Line %d\n", GET_CURRENT_LINE() );
			return FALSE;
		}
		memset( szLineBuf, 0, sizeof( char) * 512 );
	}

	fclose( fi );

	return TRUE;
}
