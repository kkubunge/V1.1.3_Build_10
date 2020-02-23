/*
******************************************************************************
(c) Copyright OMRON Corporation 1999
	All Rights Reserved

TITLE:			EvtMemLegacyAccess.h
VERSION:		1.0
DESCRIPTION:	

******************************************************************************
*/
#ifndef _FGW_FINS_SHMEM_H_
#define _FGW_FINS_SHMEM_H_
#pragma message(__FILE__)

#include <windows.h>
#include <TCHAR.h>
#include "FinsGw.h"

#ifdef	_AM_FINS_PRIVATE_
#define _EXPORT	__declspec(dllexport)
#define DllExport __declspec(dllexport)
#define _EXTERN
#else // _AM_FINS_PRIVATE_
#define _EXPORT	__declspec(dllimport)
#define _EXTERN	extern
#endif // _AM_FINS_PRIVATE_

#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
*  FINS remote access method - AmFinsRemote
*******************************************************/
/*
 * AmFinsRemote is the method that enables access to the shared memory.
 * The methods supported by AmFinsRemote are as follows:
 *
 *	- open: Initializes the area for each kind of data
 *	- close: Releases the area for each kind of data
 *	- read: Reads data from a device using FINS communications
 *	- write: Writes data to a device using FINS communications
 *	- readEx: Reads data from a device using FINS communications (with error data)
 *	- writeEx: Writes data to a device using FINS communications (with error data)
 *	- judge: Evaluates conditions
 */

//  -- AccessMethod Method Name
#define AMFINS_METHOD_NAME	"AmFinsRemote"

//  -- CURRENT VERSION INFO -- 
#ifndef AMFINS_CURRENT_VERSION
#define AMFINS_CURRENT_VERSION 0x00010000
#endif

#define AMFINS_BUILD_VERSION 0x00010000


//==========================================================
// Expansion data read/write
//==========================================================
//  -- Access Method Specification Structure

#if ( AMFINS_BUILD_VERSION == 0x00010000 ) 
// Error type
typedef enum {
	AMFINS_ERROR_NON,			// Normal
	AMFINS_ERROR_FINS_RESP,		// FINS communication error
	AMFINS_ERROR_FINS_API,		// FINS communication API error
	AMFINS_ERROR_INTERNAL_API	// Internal API error
} AMFINS_ERROR_TYPE;

typedef struct {
	AMFINS_ERROR_TYPE	errorType;	// Error type
	struct {
		BYTE	MRES;				// FINS communication error main response code
		BYTE	SRES;				// FINS communication error sub response code
	} finsResp;
	DWORD	apiLastError;			// FINS communication API error last error code
	CHAR	description[MAX_PATH];	// Error details
} AMFINS_ERROR_INFO, *pAMFINS_ERROR_INFO;

typedef struct _am_fins_read_entry_tag {
	DWORD				version;
	AMFINS_ERROR_INFO	accessError;
} AMFINS_SPEC, *pAMFINS_SPEC;
#endif

/*******************************************************
*  FINS Remote Access Method - Profile
*******************************************************/
#define AMFINS_DEVICE_NAME_LEN			64
typedef struct {
	CHAR	 deviceName[AMFINS_DEVICE_NAME_LEN];
	FINSADDR peerFinsAddr;
	DWORD	 variableOffset;
	BYTE	 variableType;
	BYTE	 retryCount;
	int		 timeoutSecond;
	BOOL	 bWordAccess;
} AMFINS_PROFILE, *pAMFINS_PROFILE;

_EXPORT BOOL WINAPI AmFins_getProfile(	//, :NULL
	LPCSTR	lpszMemName,			//
	pAMFINS_PROFILE	pProfile);	//

_EXPORT BOOL WINAPI AmFins_setProfile(	//, :NULL
	LPCSTR	lpszMemName,			//
	pAMFINS_PROFILE	pProfile);	//


#undef _EXTERN
#undef _EXPORT

//#include <EmError.h>

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif  //  _FGW_FINS_SHMEM_H_
