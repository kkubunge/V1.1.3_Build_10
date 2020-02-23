/*
******************************************************************************
(c) Copyright OMRON Corporation 1999
	All Rights Reserved

TITLE:			EvtMemLegacyAccess.h
VERSION:		1.0
DESCRIPTION:	

******************************************************************************
*/
#ifndef _FGH_AM_SHMEM_H_
#define _FGH_AM_SHMEM_H_
#pragma message(__FILE__)

#include <windows.h>
#include <TCHAR.h>

#ifdef	_AM_SHMEM_PRIVATE_
#define _EXPORT	__declspec(dllexport)
#define DllExport __declspec(dllexport)
#define _EXTERN
#else // _AM_SHMEM_PRIVATE_
#define _EXPORT	__declspec(dllimport)
#define _EXTERN	extern
#endif // _AM_SHMEM_PRIVATE_

#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
*  Shared Memory Access Method - AmShmem
*******************************************************/
/*
 * AmShmem is the method that enables access to the shared memory.
 * The methods supported by AmShmem are as follows:
 *
 *	- open: Initializes the area for each kind of data
 *	- close: Releases the area for each kind of data
 *	- read: Reads data from shared memory
 *	- write: Writes data to shared memory
 *	- readEx: Reads data from shared memory
 *	- writeEx: 
 *	- judge:
 */

//  -- AccessMethod Method Name
#define AMSHMEM_METHOD_NAME	"AmShmem"


//  -- CURRENT VERSION INFO -- 
#ifndef AMSHMEM_CURRENT_VERSION
#define AMSHMEM_CURRENT_VERSION 0x00010000
#endif

#define AMSHMEM_BUILD_VERSION 0x00010000

//  -- Access Method Specification Structure
#if ( AMSHMEM_BUILD_VERSION == 0x00010000 ) 
typedef enum {
	AMSHMEM_WRITE_MODE_ADDED,
	AMSHMEM_WRITE_MODE_HIST,
	AMSHMEM_WRITE_MODE_CLEAR
} AMSHMEM_WRITE_MODE;

typedef struct _am_shmem_write_tag {
	long*		plTime;			// UTC-formatted time; auto-sets when NULL
	PVOID		pvAddedData;	// Pointer to additional data
	WORD		wAddedDataSize;	// Number of bytes of additional data
	AMSHMEM_WRITE_MODE	fMode;	// Operating mode
} AMSHMEM_WRITE, *pAMSHMEM_WRITE;

typedef enum {
	AMSHMEM_READ_MODE_NORMAL,
	AMSHMEM_READ_MODE_CLEAR
} AMSHMEM_READ_MODE;

typedef struct _am_shmem_read_tag {
	short		nHistIndex;			// Read data history index; current data if 0
	long*		plTime;				// UTC-formatted time
	PVOID		pvAddedDataBuff;	// Pointer to additional data buffer
	WORD		wAddedDataBuffSize;	// Number of bytes of additional data buffer
	WORD		wAddedDataSize;		// Number of bytes of additional data read
	WORD		wCurHistNum;		// Number of data in history
	AMSHMEM_READ_MODE	fMode;		// Operating mode
} AMSHMEM_READ, *pAMSHMEM_READ;
#endif	// ( AMSHMEM_BUILD_VERSION == 0x00010000 ) 

typedef struct _am_shmem_write_entry_tag {
	DWORD			version;
	AMSHMEM_WRITE	writeEntry;
} AMSHMEM_WRITE_SPEC, *pAMSHMEM_WRITE_SPEC;

typedef struct _am_shmem_read_entry_tag {
	DWORD			version;
	AMSHMEM_READ	readEntry;
} AMSHMEM_READ_SPEC, *pAMSHMEM_READ_SPEC;


/*******************************************************
*  Shared Memory Access Method - Profile
*******************************************************/
typedef struct {
	BOOL 	enableAddedInfo;
	DWORD 	numAddedInfo;
	DWORD 	numHistInfo;
	DWORD 	sizeAddedData;
	CHAR	mappedAddedInofFile[MAX_PATH];
} AMSHMEM_PROFILE, *pAMSHMEM_PROFILE;

_EXPORT BOOL WINAPI AmShmem_getProfile(	//, :NULL
	LPCSTR	lpszMemName,			//
	pAMSHMEM_PROFILE	pProfile);	//

_EXPORT BOOL WINAPI AmShmem_setProfile(	//, :NULL
	LPCSTR	lpszMemName,			//
	pAMSHMEM_PROFILE	pProfile);	//


#undef _EXTERN
#undef _EXPORT

//#include <EmError.h>

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif  //  _FGH_AM_SHMEM_H_
