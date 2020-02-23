#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "APP_UTIL.h"

#include "cimseqnc.h"
#include "iodefine.h"
#include "kutlFile.h"

#include "DataType.h"
#include "Read_Ini.h"

#pragma comment( lib , "lib/APP_UTIL.lib" )

#define  CHECK_INTERVAL		3600000

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE		=	FALSE;

LOG_ITEM  LOG_DATA_LIST[MAX_LOG_ITEM];

#define Log_Del_Period	7

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"				, _K_D_IO	,	IO_Driver_Loding			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------

void _Thread_Invoke( LPVOID* Param )
{
//	printf( "%d._Thread Invoke : %s\n", dData, LOG_DATA_LIST[dData].szLogFilePath );
	BOOL				bResult = TRUE;
	BOOL				bDeleteFlag;
	char				szDrive[5];
	char				szFilePath[MAX_PATH];
	char				szFileName[MAX_PATH];
	char				szExtension[10];
	char				szCheckPath[MAX_PATH];

	char				szDeleteFileName[MAX_PATH];

	BOOL				bFolderSrc = FALSE;

	HANDLE				hFind;
	WIN32_FIND_DATA		wfd;

	SYSTEMTIME			st, cst;

	DWORD	dwFileTimeCode	=	0;
	DWORD	dwCurTimeCode	=	0;

	int		dData = (int) Param , CS;

	_splitpath( LOG_DATA_LIST[dData].szLogFilePath, szDrive, szFilePath, szFileName, szExtension );
	sprintf( szCheckPath, "%s%s", szDrive, szFilePath );
	if ( End_Slash_Check( szCheckPath ) == FALSE ) {
		strcat( szCheckPath, "*.*" );
	}

	if ( IsFolderExist( szCheckPath ) == FALSE ) {
		printf( "Auto Delete Path Define Error ( Path : %s is not exist )\n", szCheckPath );
		_endthread();
	}

	bFolderSrc = LOG_DATA_LIST[dData].IsFolder;


	while( TRUE ) {
		bResult = TRUE;
		GetLocalTime( &cst );
		hFind = FindFirstFile( LOG_DATA_LIST[dData].szLogFilePath, &wfd );
	
		while( bResult ) {
			_sleep( 1 );

//-----------------------------------------------------------------------------
//			2007.08.17 Commented by dudska - Directory 검색후 삭제 기능 추가됨..
//-----------------------------------------------------------------------------
//			if ( ( wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ) || 
//				 ( wfd.dwFileAttributes == FILE_ATTRIBUTE_READONLY ) || 
//				 ( strlen( wfd.cFileName) == 0 ))	{
				// Readonly File & Directory skip
//				bResult = FindNextFile( hFind, &wfd );
//				continue;
//			}

			if ( ( wfd.dwFileAttributes == FILE_ATTRIBUTE_READONLY ) || 
				 ( strlen( wfd.cFileName) == 0 ))	{
				// Readonly File & Directory skip
				bResult = FindNextFile( hFind, &wfd );
				continue;
			}

			if ( bFolderSrc == FALSE ) {	// 대상이 Folder가 아닌 경우에는 Folder는 Skip
				if ( wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ) {
					bResult = FindNextFile( hFind, &wfd );
					continue;
				}
			}


			if ( FileTimeToSystemTime( &wfd.ftLastWriteTime, &st ) == 0 ) {
				_IO_COMMON_PRINTF( TRUE, "EV", "DEL", "FILE TIME CONVERT FAIL : [%s:%ld]\n", wfd.cFileName, GetLastError() );
				_IO_COMMON_PRINTF( TRUE, "DEL","DEL", "FILE TIME CONVERT FAIL : [%s:%ld]\n", wfd.cFileName, GetLastError() );
				_endthread();
			}

			dwFileTimeCode	= GET_DATE_CODE( &st );
			dwCurTimeCode	= GET_DATE_CODE( &cst );

			// 날짜를 앞으로 돌리면 모든 로그가 지워질 수 있는 상황이 발생한다.
			if ( dwFileTimeCode > dwCurTimeCode ) {
				bResult = FindNextFile( hFind, &wfd );
				continue;
			}

				if (( dwCurTimeCode - dwFileTimeCode ) >= LOG_DATA_LIST[dData].dwDelCondition )	{
					memset( szDeleteFileName, 0, sizeof( char ) * MAX_PATH );

					if (( wfd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY ) && ( strcmp( wfd.cFileName, "." )) && ( strcmp( wfd.cFileName, ".." ))) {
						sprintf( szDeleteFileName, "%s%s\\%s", szDrive, szFilePath, wfd.cFileName );
						bDeleteFlag = DeleteFile( szDeleteFileName );

						if ( bDeleteFlag )	_IO_COMMON_PRINTF( TRUE, "DEL","DEL", "File Delete[C:%ld/F:%ld]: %s\n", dwCurTimeCode, dwFileTimeCode, szDeleteFileName );
						else				_IO_COMMON_PRINTF( TRUE, "DEL","DEL", "File Delete Fail[C:%ld/F:%ld]: %s [%d]\n", dwCurTimeCode, dwFileTimeCode, szDeleteFileName, GetLastError() );
					}
					else if ( ( strcmp( wfd.cFileName, "." )) && ( strcmp( wfd.cFileName, ".." ))) {
						sprintf( szDeleteFileName, "%s%s\\%s", szDrive, szFilePath, wfd.cFileName );
						if ( Directory_Delete_at_Sub( szDeleteFileName ) ) {
							_IO_COMMON_PRINTF( TRUE, "DEL","DEL", "Folder Delete[C:%ld/F:%ld]: %s\n", dwCurTimeCode, dwFileTimeCode, szDeleteFileName );
						}
						else {
							_IO_COMMON_PRINTF( TRUE, "DEL","DEL", "Folder Delete Fail[C:%ld/F:%ld]: %s [%d]\n", dwCurTimeCode, dwFileTimeCode, szDeleteFileName, GetLastError() );
						}
					}
				}

			bResult = FindNextFile( hFind, &wfd );
		}

		FindClose( hFind );

		_sleep( CHECK_INTERVAL );
	}

	_endthread();
}

//------------------------------------------------------------------------------------------
Module_Status Program_Main() 
{
	int		i;
	char	szStr[MAX_PATH * 2];

	strcpy( szStr, PROGRAM_ARGUMENT_READ() );

	memset( LOG_DATA_LIST, 0, sizeof( LOG_ITEM ) * MAX_LOG_ITEM );

	if ( Read_Ini_File( szStr ) == FALSE )	{
		// Ini Read Fail Message - Read_Ini_File 에서 발생함...

		SYSTEM_KILL_MYSELF();
		return SYS_ABORTED;
	}

	for ( i = 0 ; i < MAX_LOG_ITEM ; i++ ) {
		if ( strlen( LOG_DATA_LIST[i].szLogFilePath ) )	{
			_beginthread( (void *) _Thread_Invoke, 0, (void *) i );
		}
	}


	while( TRUE )		_sleep( 1000 );

	return SYS_SUCCESS;
}


//---------------------------------------------------------------------------------------
void Program_Enter_Code() 
{
//	REGIST_FILE_INFO( "Kornic File Auto Remover (2006.06.07)" );
//	REGIST_FILE_INFO( "Kornic File Auto Remover V1.0 (2007.03.22)" );
//	REGIST_FILE_INFO( "Kornic File Auto Remover V1.2 (2007.08.17)" );
	REGIST_FILE_INFO( "Kornic File Auto Remover V1.21 (2007.09.18)" );
	REGIST_FILE_INFO( "Kornic File Auto Remover V1.22 (2008.09.25)" );
}


//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() 
{
	int CommStatus;

	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;

	return TRUE;
}