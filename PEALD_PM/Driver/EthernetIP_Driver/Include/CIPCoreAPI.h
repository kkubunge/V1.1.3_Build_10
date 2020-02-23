/*****************************************************************************/
/* (C)	Copyright OMRON  Corporation 2007	 								 */
/*		All Rights Reserved	 												 */
/*****************************************************************************/
#ifndef	__CIPCORE_API_H_
#define	__CIPCORE_API_H_

#ifdef	_EXPORT_CIP_API_
#define _EXPORT_CIP	__declspec(dllexport)
#define _EXTERN_CIP
#else
#define _EXPORT_CIP	__declspec(dllimport)
#define _EXTERN_CIP	extern
#if defined(_WIN32)
#if defined(_WIN32_WCE)
#pragma comment(lib,"CIPCoreLib32.lib")
#else
//#pragma comment(lib, "CIPCoreWin32.lib")
#endif //defined(_WIN32_WCE)
#endif
#endif 

#include <windows.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <CIPError.h>

/*---------------------------------------------------------------------------
**
** CIPCore API semaphore
**
**---------------------------------------------------------------------------
*/
#undef USE_API_EXCLUSIVE 

#ifdef USE_API_EXCLUSIVE
#define cipApp_initSemaphore( x )	_os_sema_init(&(x)->lock)
#define cipApp_lockSemaphore( x )	_os_sema_p(&(x)->lock)
#define cipApp_unlockSemaphore( x )	_os_sema_v(&(x)->lock)
#define cipApp_termSemaphore( x )	_os_sema_term(&(x)->lock)
#endif


/*****************************************************************************/
/* Defines																	 */
/*****************************************************************************/
#define	CIP_LOGICAL_SEGMENT_CLASS			0x20
#define	CIP_LOGICAL_SEGMENT_INSTANCE		0x24
#define CIP_LOGICAL_SEGMENT_ATTRIBUTE		0x30
#define	CIP_LOGICAL_SEGMENT_MEMBER_USINT	0x28	// 16-bit logical address
#define	CIP_LOGICAL_SEGMENT_MEMBER_UINT		0x29	// 32-bit logical address

#define	CIP_ANSI_EXTENDED_SYMBOL_SEGMENT	0x91

#define	CIP_LOGICAL_FORMAT_8BIT				0x00
#define	CIP_LOGICAL_FORMAT_16BIT			0x01
#define	CIP_LOGICAL_FORMAT_32BIT			0x02

#define	CIPAPP_MAX_APP_NAMESIZE				64
#define CIPAPP_CLASS_NAME_LENGTH_MAX		64

#define	VARIABLE_LINK_STATE_ON			2	// for supports ethernet 2 ports
#define	VARIABLE_LINK_STATE_IDLE		1	// for supports ethernet 2 ports
#define	VARIABLE_LINK_STATE_NO_VARIABLE		0	// for supports ethernet 2 ports
#define	VARIABLE_LINK_STATE_ERROR		-1	// for supports ethernet 2 ports

/*---------------------------------------------------------------------------
** CIPCore API Version numbers
**---------------------------------------------------------------------------
*/
#define CIPCORE_MAJOR_REVISION		0x01
#define CIPCORE_MINOR_REVISION		0x01	// for supports ethernet 2 ports
//#define CIPCORE_MINOR_REVISION		0x00

/*****************************************************************************/
/* Typedefs																	 */
/*****************************************************************************/
typedef	HANDLE	CIPAppHandle;
typedef HANDLE	CIPAppExplicitParam;

typedef	enum {
	CIPExplicitType_UCMM 		 = 0,	/* UCMM 								*/
	CIPExplicitType_CLASS3 	  	 = 1,	/* Class3(ForwardOpen/LargeForwardOpen)	*/
} CIPExplicitType;

typedef struct{
	BYTE	major;
	BYTE	minor;
} CIPAppVersion;

typedef struct {
	void*	path;
	DWORD	byteSize;
} CIPNetworkPath, CIPRequestPath;

typedef struct {
	void*	data;
	int		byteSize;
} CIPRequestData, CIPResponseData, CIPReceiveData;

typedef	struct {
	WORD	status[ UCHAR_MAX ];
	BYTE	statusSize;
} CIPAdditionalStatus;

typedef	struct {
	WORD	portID;
	DWORD	nodeID;
} CIPPortSegmentInfo;

typedef struct {
	CIPPortSegmentInfo*  portSegment;
	DWORD				segmentCount;
} CIPNetworkPathInfo;

typedef	struct {
	const char* name;
	BYTE 		byteSize;
} CIPTagName, CIPClassName;

typedef struct {
	BYTE	serviceCode;
	BYTE	requestPathSize;
} CIPExplicitRequestHeader;

typedef struct {
	BYTE	serviceCode;
	BYTE	reserve;
	BYTE	generalStatus;
	BYTE	additionalStatusSize;
} CIPExplicitResponseHeader;

typedef struct{
	WORD	classID;
	WORD	classNameLength;
	BYTE	className[CIPAPP_CLASS_NAME_LENGTH_MAX];
} CIPObjectListDataType;

typedef struct{
	DWORD					appID;
	char					className[CIPAPP_CLASS_NAME_LENGTH_MAX];
	CIPAppVersion		 	version;
} CIPAppInfo;

typedef struct{
	DWORD	classID;
	DWORD	classNameLength;
	char	className[CIPAPP_CLASS_NAME_LENGTH_MAX];
}CIPObjectListType;

/*****************************************************************************/
/* CIPCore System API														 */
/*****************************************************************************/
/*---------------------------------------------------------------------------*/
/* CIPApp_requestVersion											 	 	 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP BOOL WINAPI CIPApp_requestVersion(
	BYTE	major,
	BYTE	minor
);
#define CIPAPP_STARTUP() (CIPApp_requestVersion(CIPCORE_MAJOR_REVISION, CIPCORE_MINOR_REVISION))

/*---------------------------------------------------------------------------*/
/* CIPApp_new														 	 	 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP CIPAppHandle WINAPI CIPApp_new(
		LPCTSTR	appName
);

/*---------------------------------------------------------------------------*/
/* CIPApp_delete															 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP BOOL WINAPI CIPApp_delete(
		CIPAppHandle	hApp
);
/*---------------------------------------------------------------------------*/
/* CIPApp_enumApplications													 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP int WINAPI CIPApp_enumApplications(
		CIPAppHandle	hApp,
		DWORD			appListNum,
		CIPAppInfo*		cipApplicationList
);

/*---------------------------------------------------------------------------*/
/* CIPApp_getVersion														 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP CIPAppVersion WINAPI CIPApp_getVersion();

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* CIPApp_getLastErrorMessage												 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP int WINAPI CIPApp_getLastErrorMessage(
		DWORD	dwErrorCode,
		LPTSTR	lpBuffer,
		int		lNumberOfBytesBuffer
);

/*****************************************************************************/
/* CIPCore Explicit Message API												 */
/*****************************************************************************/
/*---------------------------------------------------------------------------*/
/* CIPApp_openConnectionExplicit										 	 	 	 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP CIPAppExplicitParam WINAPI CIPApp_openConnectionExplicit( 		/* Return : pMessageParam */
										   				/*		 : "NULL" = Error */
		CIPAppHandle	  		hApp,
		int						messageType,
		CIPNetworkPath*			networkPath,
		BYTE*					generalStatus,
		CIPAdditionalStatus*	additionalStatus,
		BYTE*					remainingPathSize,
		DWORD					timeoutMillisec,
		DWORD					connectionSize
);

/*---------------------------------------------------------------------------*/
/* CIPApp_closeConnectionExplicit										 	 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP BOOL WINAPI CIPApp_closeConnectionExplicit(
		CIPAppHandle			hApp,
		CIPAppExplicitParam    	hMessage,
		BYTE*					generalStatus,
		CIPAdditionalStatus*	additionalStatus,
		BYTE*				  	remainingPathSize
);
/*---------------------------------------------------------------------------*/
/* CIPApp_sendRequestExplicit									 	 		 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP BOOL WINAPI CIPApp_sendRequestExplicit(
		CIPAppHandle		hApp,
		CIPAppExplicitParam	hMessage,
		DWORD				messageID,
		DWORD				requestDataByteSize,
		BYTE*				requestData
);
/*---------------------------------------------------------------------------*/
/* CIPApp_receiveExplicit										 			 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP int WINAPI CIPApp_receiveExplicit(		/* Return : received response data size(byte)	*/
								/*		  : "-1"	 = Error 					*/
		CIPAppHandle	hApp,
		DWORD*			messageID,
		DWORD			receiveDataBufferByteSize,
		BYTE*			receiveData,					/* Received CIP Response Data :Service Code + General Status + Additional Status + Response Data */
		DWORD			timeoutMillisec,					/* Timeout period(ms) 			 */
		DWORD*			explicitResponseMaxSize
);
/*---------------------------------------------------------------------------*/
/* CIPUtil_constructNetworkPath											 	 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP DWORD WINAPI CIPUtil_constructNetworkPath(				/* Return : Network Path Size(Byte) */
												/*		  :  "0" = Error			*/
		CIPNetworkPathInfo* networkPathInfo,
		CIPNetworkPath*		networkPath
);
/*---------------------------------------------------------------------------*/
/* CIPUtil_constructRequestPathWithCIA	 # CIA : Class, Instance, Attribute	 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP DWORD WINAPI CIPUtil_constructRequestPathWithCIA(		/* Return : Request Path Size(Byte) */
												/*		  :  "0" = Error			*/
		DWORD			classID,
		DWORD			instanceID,
		DWORD			attributeID,
		CIPRequestPath*	requestPath
);
/*---------------------------------------------------------------------------*/
/* CIPUtil_constructRequestPathWithTagName									 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP DWORD WINAPI CIPUtil_constructRequestPathWithTagName(	/* Return : Request Path Size(Byte) */
												/*		  :  "0" = Error			*/
		LPCTSTR		tagName,
		CIPRequestPath*	requestPath
);
/*---------------------------------------------------------------------------*/
/* CIPUtil_constructRequestPathWithTagName									 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP DWORD WINAPI CIPUtil_constructLogicalSegment(
		DWORD	ID,
		BYTE 	SegmentType,
		BYTE*	writeBuffer,
		WORD 	bufferSize
);
/*---------------------------------------------------------------------------*/
/* CIPUtil_CalculateCRC32													 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP DWORD WINAPI CIPUtil_CalculateCRC32(	/* Return : CRC */
		BYTE*	p_Start,
		DWORD	ByteSize,
		DWORD	preset
);
/*---------------------------------------------------------------------------*/
/* CIPUtil_CalculateCRC16													 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP WORD WINAPI CIPUtil_CalculateCRC16(	/* Return : CRC */
		BYTE*	p_Start,
		WORD	ByteSize,
		WORD	preset
);

/*---------------------------------------------------------------------------*/
/* CIPUtil_getResponseErrorMessage													 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP int WINAPI CIPUtil_getResponseErrorMessage(
	BYTE	generalStatus,
	LPTSTR buffer,
	int bufferSize
	);

/*****************************************************************************/
/* CIPCore Variable Status API			                             */
/*****************************************************************************/
/*---------------------------------------------------------------------------*/
/* CIPApp_GetVariableStatus
/*---------------------------------------------------------------------------*/
_EXPORT_CIP int WINAPI CIPApp_GetVariableStatus( /* Return : Variable Status */
		CIPAppHandle	  	hApp,
		BOOL			bAsProduceVariable,
		LPTSTR			lpstrVariable
);

#ifdef __cplusplus
};
#endif	/* __cplusplus */

#endif	/* __CIPCORE_API_H_ */
