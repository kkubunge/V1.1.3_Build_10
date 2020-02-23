//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
// Shared Memory Function

#include <stdio.h>
#include <windows.h>
//#include <stdlib.h>
//#include <process.h>
//#include <time.h>
#include "SharedMemory.h"

#define	S_MEMORY_SIZE	1024*8
#define SM_FILE_NAME	"POST_RECIPE_DATA_FILE"
HANDLE g_hFileHandle = NULL;
LPVOID g_pViewOfFile = NULL;
char g_szErrDesc[256];
unsigned char g_pSH_DATA[S_MEMORY_SIZE];
BOOL g_bSM_Result = FALSE;

BOOL CreateSharedMemory()
{
	char szLog[256];

	g_bSM_Result = FALSE;
	if(g_hFileHandle != NULL)
	{
		sprintf(g_szErrDesc, "FileMapping is already opened");
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		// _LOG(szLog);
		printf("%s\n", szLog);
		return FALSE;
	}

	g_hFileHandle = CreateFileMapping(	INVALID_HANDLE_VALUE,               // handle to file
										NULL,								// security
										PAGE_READWRITE,                     // protection
										0,						            // high-order DWORD of size
										S_MEMORY_SIZE,				            // low-order DWORD of size
										SM_FILE_NAME	  	                    // object name
									 );

	if (NULL == g_hFileHandle)
	{
		sprintf(g_szErrDesc, "Unable to create file mapping. Error is %d", GetLastError());
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		// _LOG(szLog);
		printf("%s\n", szLog);
		return FALSE;
	}

	g_pViewOfFile = MapViewOfFile(	g_hFileHandle,							// handle to file-mapping object
									FILE_MAP_ALL_ACCESS,					// access mode
									0,										// high-order DWORD of offset
									0,										// low-order DWORD of offset
									0										// number of bytes to map
								);
	if (NULL == g_pViewOfFile)
	{
		sprintf(g_szErrDesc, "Unable to map to the current address space. Error is %d", GetLastError());
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		// _LOG(szLog);
		printf("%s\n", szLog);
		return FALSE;
	}

	sprintf(szLog, "-----> Create Shared Memory Successfully");
	// _LOG(szLog); printf("%s\n", szLog);
	g_bSM_Result = TRUE;
	return TRUE;
}

BOOL OpenSharedMemory()
{
	BOOL bRet = TRUE;
	if(g_hFileHandle != NULL)
	{
		sprintf(g_szErrDesc, "FileMapping is already opened");
		return FALSE;
	}

	g_hFileHandle = OpenFileMapping(		FILE_MAP_WRITE,                     // protection
										(int) NULL,
										SM_FILE_NAME	  	                   // object name
									 );

	if (NULL == g_hFileHandle)
	{
		sprintf(g_szErrDesc, "Unable to open file mapping. Error is %d", GetLastError());
		return FALSE;
	}

	g_pViewOfFile = MapViewOfFile(	g_hFileHandle,							// handle to file-mapping object
									FILE_MAP_ALL_ACCESS,					// access mode
									0,										// high-order DWORD of offset
									0,										// low-order DWORD of offset
									0										// number of bytes to map
								);

	if (NULL == g_pViewOfFile)
	{
		sprintf(g_szErrDesc, "Unable to map to the current address space. Error is %d", GetLastError());
		bRet = FALSE;
	}
	return bRet;
}

BOOL FreeSharedMemory()
{
	BOOL bRet = TRUE;
	if (g_pViewOfFile != NULL)
	{
		if(FALSE == UnmapViewOfFile(g_pViewOfFile))
		{
			sprintf(g_szErrDesc, "Unable to unmap view of file. Error is %d", GetLastError());
			bRet = FALSE;
		}
	}
	if (NULL != g_hFileHandle)
		CloseHandle(g_hFileHandle);
	g_hFileHandle = NULL;

	g_bSM_Result = FALSE;
	return bRet;
}

BOOL ReadFromSharedMemory()
{
	char szLog[256];
	if(g_hFileHandle == NULL)
	{
		sprintf(g_szErrDesc, "FileMapping is not opened");
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		// _LOG(szLog);
		printf("%s\n", szLog);
		g_bSM_Result = FALSE;
		return FALSE;
	}

	memcpy(g_pSH_DATA, (char*)g_pViewOfFile, S_MEMORY_SIZE);
	g_bSM_Result = TRUE;
	// _LOG("-----> Read From Shared Memory Successfully");
	return TRUE;
}

BOOL WriteToSharedMemory()
{
	char szLog[256];
	if(g_hFileHandle == NULL)
	{
		sprintf(g_szErrDesc, "FileMapping is not opened");
		sprintf(szLog, "-----> Error : %d", g_szErrDesc);
		// _LOG(szLog);
		printf("%s\n", szLog);
		g_bSM_Result = FALSE;
		return FALSE;
	}

	memcpy((char*)g_pViewOfFile, g_pSH_DATA, S_MEMORY_SIZE);
	g_bSM_Result = TRUE;
	// _LOG("-----> Write To Shared Memory Successfully");
	return TRUE;
}

