#ifndef __IPS_LOG_H__
#define __IPS_LOG_H__

#include <stdio.h>
#include <windows.h>

const int _ELT_NoLog		= 0;
const int _ELT_File			= 1;
const int _ELT_Console		= 2;
const int _ELT_FileConsole	= 3;

typedef enum
{
	ELT_NoLog = 0,
	ELT_File,
	ELT_Console,
	ELT_FileConsole
} ELogType;

static void _PWD_LOG(const char * pszAppName, ELogType eLogOption, const char * pFormat, ...)
{
	// 0. declaration
	va_list args;		
	char szFileName[256];
	char szTimeStamp[256];
	char szFileAppName[256];
	char pszErrorText[2048];
	FILE* fpText = NULL;
	SYSTEMTIME strtSysTime;
	char * pcInvalidChar = NULL;
	
	va_start(args, pFormat);
	vsprintf(pszErrorText,pFormat,args);
	va_end(args);

	// 1. implementation
	if( ELT_NoLog == eLogOption || NULL == pszAppName || NULL == pszErrorText) return;	

	// 2. Make Time Stamp
	GetLocalTime( &strtSysTime );

	sprintf( szTimeStamp, "%02d/%02d %d:%02d:%02d.%03d",	strtSysTime.wMonth, 
												strtSysTime.wDay,
												strtSysTime.wHour,
												strtSysTime.wMinute,
												strtSysTime.wSecond,
												strtSysTime.wMilliseconds);

	
	if( ELT_File == eLogOption || ELT_FileConsole == eLogOption)
	{
		strcpy( szFileAppName, pszAppName);		
		pcInvalidChar = strpbrk( szFileAppName, "\\/:*?\"<>|");
		if( NULL != pcInvalidChar )		
			*pcInvalidChar = 0;
		/*													
		sprintf( szFileName, "D:\\Bluetain-CTC\\_log\\PWDLog_%04d%02d%02d.log",														
															strtSysTime.wYear,
															strtSysTime.wMonth, 
															strtSysTime.wDay);
															*/
		
		sprintf( szFileName, "D:\\Bluetain-CTC\\_Log\\GEMMonitor_%04d%02d.log",														
															strtSysTime.wYear,
															strtSysTime.wMonth);

		fpText = fopen(szFileName,"a+");
		if(NULL != fpText && NULL != pszErrorText && 0 != strcmp(pszErrorText,""))
		{
			fprintf(fpText,"%s	%s\n", szTimeStamp, pszErrorText);
			fflush(fpText);
		}
		if( NULL != fpText)
		{
			fclose(fpText);
		}
	}
	if( ELT_Console == eLogOption || ELT_FileConsole == eLogOption)
	{		
		printf("-> %s; %s, %s\n", pszAppName, szTimeStamp, pszErrorText);
	}
		
}

static void MESSAGE_OUT(const char * pszAppName, const char* pFormat, ...)
{
	char szTitle[256];
	char szTimeStamp[256];
	char szError[2048];
	char pszErrorText[2048];
	SYSTEMTIME strtSysTime;	

	va_list args;	
	va_start(args, pFormat);
	vsprintf(pszErrorText,pFormat,args);
	va_end(args);

	
	GetLocalTime( &strtSysTime );
	sprintf( szTimeStamp, "%d:%02d:%02d:%03d",	strtSysTime.wHour,
														strtSysTime.wMinute,
														strtSysTime.wSecond,
														strtSysTime.wMilliseconds);

	
	sprintf( szTitle, "[%s]", pszAppName);	
	sprintf( szError, "%s: %s", szTimeStamp, pszErrorText);


	MessageBox( NULL, szError, szTitle, MB_OK| MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_TOPMOST);

}

#endif