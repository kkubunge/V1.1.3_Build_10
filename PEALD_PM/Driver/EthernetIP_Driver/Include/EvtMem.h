/*
******************************************************************************
(c) Copyright OMRON Corporation 1996-2000,2003
	All Rights Reserved
******************************************************************************
*/
#ifndef _EVENT_MEMORY_SYSTEM_H_
#define _EVENT_MEMORY_SYSTEM_H_
#pragma message(__FILE__)

#include <windows.h>
#include <TCHAR.h>
#include <FgwAccessMethod.h>

#if defined(QNX)
#include <Fgw/FgwGenericType.h>
#else
#include <FgwGenericType.h>
#endif //defined(QNX)
#include <FgwConfig.h>


#ifdef	_EVTMEM_PRIVATE_
#if !defined(_MIPS_)
#define _EXPORT	__declspec(dllexport)
#define DllExport __declspec(dllexport)
#define _EXTERN
#else //!defined(_MIPS_)
#define _EXPORT	
#define DllExport 
#define _EXTERN
#endif !defined(_MIPS_)
#else
#define _EXPORT	__declspec(dllimport)
#define _EXTERN	extern
#pragma comment(lib, "EvtMem32.lib")
#endif 

#ifdef __cplusplus
extern "C" {
#endif

//=========================================================================
typedef struct TagEmVersion {
	BYTE	byMajor; 	//
	BYTE	byMinor; 	//
	BYTE	byRevision; //
	BYTE	byReserved; //
} EM_VERSION, *pEM_VERSION;

#define EM_CURRENT_MAJOR_VERSION	2
#define EM_CURRENT_MINOR_VERSION	0

//================================================================
// EM addres structure
//================================================================
typedef struct TagEmAddress {
	BYTE	byTypeOfFactor;
	BYTE	byLocateOnWord;
	DWORD	dwWordOffset;
	DWORD	dwNumberOfFactors;
} EM_ADDRESS, *pEM_ADDRESS;

#define EM_NAME_LENGTH_MAX	(16)
typedef struct TagEmMemoryArea {
	char		lpszMemoryName[EM_NAME_LENGTH_MAX];
	EM_ADDRESS	sAddress;
} EM_AREA, *pEM_AREA;

enum TagTypeOfFactor {
	EM_NO_DATA,
	EM_BIT_TYPE,
	EM_BYTE_TYPE,
	EM_WORD_TYPE,
	EM_DWORD_TYPE,
};

#define EM_BYTE_LOW		(1)	//
#define EM_BYTE_HIGH	(0)	//

typedef BYTE	EM_BYTE, *pEM_BYTE;
typedef WORD	EM_WORD, *pEM_WORD;
typedef DWORD	EM_DWORD, *pEM_DWORD;

//----- EmMemory Vector -----
typedef struct _tagEmMemoryVector
{
	EM_ADDRESS address;
	PVOID base;
	DWORD length;
} tEmMemoryVector, *pEmMemoryVector;

//==========================================================
// EventData
//==========================================================
enum EmEventTypeEnum {
	eEmEventType_Empty = 0,
	eEmEventType_Memory = 1,
	eEmEventType_Ipc = 2
};
typedef struct 
{
	INT eventId;
	BYTE eventType;
	union {
		struct {
			EM_AREA area;
			USHORT dataSize;
			BYTE memoryValue[cFGWEM_MAXEVENTDATA];
		} memory;
		struct {
			char sendPortName[cFGWMSG_APPNAMELENGTH+1];
			EM_AREA area;
			pid_t replyProxy;
			USHORT dataSize;
			BYTE data[cFGWEM_MAXEVENTDATA];
		} ipc;
	} u;
} tEmEventXfrData, *pEmEventXfrData;

#define EM_NO_ADDR_INFO	(0)	


//============================================================
typedef enum TagEmArithmeticLogicType {
	AND = 0,		//
	ANDEQ = 1,		//
	AlwaysTRUE = 2,	//
	NOP = 3,		//
	EQ = 4,			//
	LT = 5,			//
	LE = 6,			//
	GT = 7,			//
	GE = 8,			//
	GELE = 9,		//
	GTLT = 10,		//
	GELT = 11,		//
	GTLE = 12,		//
	PrevAND = 13,	//
	PrevANDEQ = 14,	//
	PrevEQ = 15,		//
	PrevLT = 16,		//
	PrevLE = 17,		//
	PrevGT = 18,		//
	PrevGE = 19,		//
	LOGIC = 20,
} EM_LOGIC_TYPE;

typedef struct TagEmArithmeticLogicArguments {
	DWORD	dwConst1;	//True/false compare constant1
	DWORD	dwConst2;	//True/false compare constant2
} EM_CONST, *pEM_CONST;

typedef struct TagEmAtithmeticLogicParameter {
	EM_LOGIC_TYPE	sLogicType;
	EM_CONST		sConst;		//True/false compare constants
} EM_LOGIC, *pEM_LOGIC;

typedef enum TagEmActionParameter {
	NothingOnTransit,
	ExecuteOnTransit,
} EM_ACTION;

enum TagEmTransitType {
	FtoF,
	FtoT,
	TtoT,
	TtoF,
	TRANSIT,
};

typedef enum TagEmPreviousResult {
	False,
	True,
	Auto,
} EM_PREVIOUS_RESULT;

typedef struct TagEmEstimation {
	EM_LOGIC			sLogic;
	EM_ACTION			sAction[TRANSIT];
	EM_PREVIOUS_RESULT	sPreviousResult;
} EM_ESTIMATION, *pEM_ESTIMATION;

//Event send target
typedef struct TagEmDestinationOnEvent {
	char	lpszEventName[EM_NAME_LENGTH_MAX];	//Event port name
} EM_DESTINATION, *pEM_DESTINATION;

//Send event data: specifies the data to send at event generation
typedef struct TagEmObjectSent {
	INT		lEventId;	//Event ID
	EM_AREA	sSendArea;	//Area to send as data
} EM_SEND_OBJECT, *pEM_SEND_OBJECT;

//================================================
//Normal event conditions
typedef struct TagEmCondition {
	EM_ESTIMATION	sEstimation;	//Judgement conditions
	EM_DESTINATION	sDestination;	//Event send target
	EM_SEND_OBJECT	sSendObject;	//Send data
} EM_CONDITION, *pEM_CONDITION;

typedef struct TagEmConditionInfo {
	DWORD			dwCndId;
	EM_ADDRESS		sAddress;
	EM_CONDITION	sCondition;
	BOOLEAN			bIsVolatile;
} EM_CND_INFO, *pEM_CND_INFO;

//================================================
//Wide event generation conditions
typedef enum TagEmWideEstimation {
	EventOnChange,	//Event generation at data change
	EventOnWrite,	//Event generation at write
} EM_WIDE_ESTIMATION;

typedef struct TagEmWideCondition {
	EM_WIDE_ESTIMATION	sWideEstimation;	//Judgement conditions
	EM_DESTINATION		sDestination;		//Event send target
	EM_SEND_OBJECT		sSendObject;		//Send data
} EM_WIDE_CONDITION, *pEM_WIDE_CONDITION;

typedef struct TagEmWideConditionInfo {
	DWORD				dwCndId;		//Condition ID
	EM_ADDRESS			sAddress;		//Address where condition is set
	EM_WIDE_CONDITION	sWideCondition;	//Wide event generation conditions
	BOOLEAN				bIsVolatile;	//Volatile/involatile
} EM_WIDE_CND_INFO, *pEM_WIDE_CND_INFO;

//================================================
//Lost event log data: Event send failure log
typedef struct TagEmLostEvent {
	SYSTEMTIME	sSystemTime;						//Local date/time
	char		lpszEventName[EM_NAME_LENGTH_MAX];	//Send target event port name
	INT			lEventId;							//Event ID
	DWORD		dwErrorCode;						//Error code
} EM_LOST_EVENT, *pEM_LOST_EVENT;


//========================================================
typedef struct TagMemoryPortUsed {
	char	lpszMemoryName[EM_NAME_LENGTH_MAX];
	DWORD	dwProcess;
} EM_MEMORY_PORT_USED, *pEM_MEMORY_PORT_USED;

typedef struct TagEventPortUsed {
	char	lpszEventName[EM_NAME_LENGTH_MAX];
} EM_EVENT_PORT_USED, *pEM_EVENT_PORT_USED;

typedef struct TagEventMemorySystemInformation {
	DWORD	dwTotalNumberOfSharedMemories;
	DWORD	dwTotalNumberOfEventPorts;
	DWORD	dwTotalNumberOfEvents;
	DWORD	dwTotalNumberOfLostEventLogs;
} EM_SYSTEM_INFO, *pEM_SYSTEM_INFO;

typedef struct TagMemorySizeInformation {
	DWORD	dwWordSizeOfMemory;
	DWORD	dwTotalNumberOfNormalConditions;
	DWORD	dwTotalNumberOfWideConditions;
} EM_MEMORY_SIZE_INFO, *pEM_MEMORY_SIZE_INFO;

typedef struct TagMemoryMappedFilePathInformation {
	char	lpszPathOfMemoryDataFile[MAX_PATH];			//Map filename
	char	lpszPathOfNormalConditionFile[MAX_PATH];	//Unused
	char	lpszPathOfWideConditionFile[MAX_PATH];		//Unused
} EM_FILE_PATH_INFO, *pEM_FILE_PATH_INFO;

typedef struct TagMemoryInformation {
	EM_MEMORY_SIZE_INFO	sMemorySizeInfo;
	EM_FILE_PATH_INFO	sFilePathInfo;
} EM_MEMORY_INFO, *pEM_MEMORY_INFO;


//========================================================
#define EM_STARTUP()	Em_requestVersion(EM_CURRENT_MAJOR_VERSION, EM_CURRENT_MINOR_VERSION)

_EXPORT BOOL WINAPI Em_requestVersion(
	BYTE	byMajor,
	BYTE	byMinor);

_EXPORT EM_VERSION WINAPI Em_getVersion(void);

//========================================================
_EXPORT HANDLE WINAPI Em_openMemory(
	LPCSTR	lpszMemName,
	PVOID	pvBaseAddr);

_EXPORT BOOL WINAPI Em_closeMemory(
	HANDLE	hMem);

//========================================================
_EXPORT BOOL WINAPI Em_readMemory(
	HANDLE		hMem,
	pEM_ADDRESS	psAddr,
	PVOID		pvBuf,
	DWORD		dwNumberOfBytesBuf);

_EXPORT BOOL WINAPI Em_writeMemory(
	HANDLE		hMem,
	pEM_ADDRESS	psAddr,
	PVOID		pvData,
	DWORD		dwNumberOfBytesData);

//========================================================
_EXPORT BOOL WINAPI EmMemory_readVector(
	HANDLE hMem,
	pEmMemoryVector vector,
	DWORD vectorCount);

_EXPORT BOOL WINAPI EmMemory_writeVector(
	HANDLE hMem,
	pEmMemoryVector vector,
	DWORD vectorCount);

//================================================================
_EXPORT BOOL WINAPI Em_readMemoryEx(
	HANDLE		hMem,
	pEM_ADDRESS	psAddr,
	PVOID		pvBuf,
	DWORD		dwNumberOfBytesBuf,
	PVOID		pvMethodSpecific
	);

_EXPORT BOOL WINAPI Em_writeMemoryEx(
	HANDLE		hMem,
	pEM_ADDRESS	psAddr,
	PVOID		pvData,
	DWORD		dwNumberOfBytesData,
	PVOID		pvMethodSpecific
	);

_EXPORT BOOL WINAPI Em_judgeCondition(
	HANDLE		hMem,
	pEM_ADDRESS	psAddr
	);

//========================================================
_EXPORT HANDLE WINAPI Em_openEvent(
	LPCSTR	lpszEvtName);

_EXPORT BOOL WINAPI Em_closeEvent(
	HANDLE	hEvt);

_EXPORT HANDLE WINAPI Em_openEventAnyPort(
	LPSTR		portName
	);

#if defined(QNX)
EXPORT HANDLE Em_duplicateHandleForSender(
	LPCSTR		nameStr
	);
#endif //defined(QNX)

_EXPORT BOOL WINAPI Em_sendEvent(
	HANDLE		hEvt,
	LPCSTR		lpszEvtName,
	INT			lEvtId,
	pEM_AREA	psAreaInformed,
	PVOID		pvData,
	DWORD		dwNumberOfBytesData);

_EXPORT BOOL WINAPI Em_receiveEvent(
	HANDLE		hEvt,
	PINT		plEvtId,
	pEM_AREA	psAreaInformed,
	PVOID		pvBuf,
	DWORD		dwNumberOfBytesBuf,
	PDWORD		pdwNumberOfBytesReceive,
	DWORD		dwTimeout);


//========================================================
_EXPORT BOOL WINAPI Em_setCondition(	//Successful: TRUE; failed: FALSE
	HANDLE			hMem,			//Memory handle
	PDWORD			pdwCndId,		//Condition ID set
	pEM_ADDRESS		psAddr,			//Address where condition is set
	pEM_CONDITION	psCnd,			//Normal event generation condition
	BOOLEAN			bIsVolatile);	//Condition volatile/involatile specification

_EXPORT BOOL WINAPI Em_clearCondition(
	HANDLE	hMem,
	DWORD	dwCndId);

_EXPORT BOOL WINAPI Em_getCondition(
	HANDLE			hMem,
	pEM_CND_INFO	psCndInfo,
	DWORD			dwCndId);

_EXPORT BOOL WINAPI Em_getConditionList(	//
	HANDLE			hMem,				//
	pEM_CND_INFO	psCndInfo,			//
	DWORD			dwNumberOfCndBufs,	//
	PDWORD			pdwNumberOfCnds);	//			

//========================================================
//Set wide event generation conditions
_EXPORT BOOL WINAPI Em_setWideCondition(	//Successful: TRUE; failed: FALSE
	HANDLE				hMem,			//Memory handle
	PDWORD				pdwCndId,		//Condition ID set
	pEM_ADDRESS			psAddr,			//Address where condition is set
	pEM_WIDE_CONDITION	psWideCnd,		//Wide event generation condition
	BOOLEAN				bIsVolatile);	//Condition volatile/involatile specification

//Obtain wide even generation condition setting data
_EXPORT BOOL WINAPI Em_getWideCondition(	//Successful: TRUE; failed: FALSE
	HANDLE				hMem,			//Memory handle
	pEM_WIDE_CND_INFO	psWideCndInfo,	//Normal event generation condition setting data
	DWORD				dwCndId);		//Condition ID set

//Determine whether condition ID is wide event condition ID
_EXPORT BOOL WINAPI Em_isWideConditionId(	//Successful: TRUE; failed: FALSE
	HANDLE		hMem,		//Memory handle
	DWORD		dwCndId,	//Condition ID
	PBOOLEAN	pbIsWide);	//Wide condition/not

//Obtain list of wide event generation condition setting data
_EXPORT BOOL WINAPI Em_getWideConditionList(	//Successful: TRUE; failed: FALSE
	HANDLE				hMem,				//Memory handle
	pEM_WIDE_CND_INFO	psWideCndInfo,		//Structure to store the wide event generation condition setting data list
	DWORD				dwNumberOfCndBufs,	//Number of structure arrays
	PDWORD				pdwNumberOfCnds);	//Number of wide event generation conditions set


//================================================
_EXPORT BOOL WINAPI Em_setMessageOnArrival(
	HANDLE	hEvt,
	HWND	hWnd,
	UINT	uMsg);

_EXPORT BOOL WINAPI Em_setThreadMessageOnArrival(
	HANDLE	hEvt,
	DWORD	dwThreadId,
	UINT	uMsg);

_EXPORT BOOL WINAPI Em_clearMessageOnArrival(
	HANDLE	hEvt);

//================================================================
_EXPORT BOOL WINAPI Em_clearLostEventLogs();

//Obtain lost event log data
_EXPORT BOOL WINAPI Em_getLostEventLogs(	//Successful: TRUE; failed: FALSE
	pEM_LOST_EVENT	psLostEvents,				//Structure to store the lost event log data
	DWORD			dwNumberOfEventBufs,		//Number of structure arrays
	PDWORD			pdwNumberOfEventLogs);		//Number of lost events logged

//================================================================
_EXPORT BOOL WINAPI Em_getMemoryPortUsage(
	pEM_MEMORY_PORT_USED	psUsedMemPort,
	DWORD					dwNumberOfPortBufs,
	PDWORD					pdwNumberOfUsedPorts);

_EXPORT BOOL WINAPI Em_getEventPortUsage(
	pEM_EVENT_PORT_USED	psUsedEvtPort,
	DWORD				dwNumberOfUsedPortBufs,
	PDWORD				pdwNumberOfUsedPorts);

//================================================================
_EXPORT BOOL WINAPI Em_getSystemInfo(
	pEM_SYSTEM_INFO	psSystemInfo);

_EXPORT BOOL WINAPI Em_getMemoryInfo(
	HANDLE			hMem,
	pEM_MEMORY_INFO	psMemInfo
	);

_EXPORT HANDLE WINAPI Em_getEventHandle(
	HANDLE	hEvt);

_EXPORT HANDLE WINAPI Em_getMutexHandle(
	HANDLE	hMem);

_EXPORT PVOID WINAPI Em_getBaseAddress(
	HANDLE	hMem);

_EXPORT int WINAPI Em_getLastErrorMessage(
	DWORD	dwErrorCode,
	LPSTR	lpBuffer,
	int		lNumberOfBytesBuffer);

//================================================================
_EXPORT BOOL WINAPI Em_flushFile(
	HANDLE	hMem);

_EXPORT BOOL WINAPI Em_swapBytes(
	PWORD	pwData,
	DWORD	dwNumberOfWords);

_EXPORT BOOL WINAPI Em_getBytesBuffer(
	pEM_ADDRESS	psAddress,
	PDWORD		pdwNumberOfBytesBuf);


//================================================================
typedef struct TagEmProfile {
	DWORD	numMemoryChanels;
	DWORD 	numConditions;
	DWORD 	numWideConditions;
	BOOL 	bNeedMapMemory;
	BOOL 	bUseEventCondition;
	char	lpszMappedFilePath[MAX_PATH];
	char	lpszAccessMethodName[MAX_PATH];
} EM_PROFILE, *pEM_PROFILE;

_EXPORT BOOL WINAPI EmProfile_getSystemProfile(
	pEM_SYSTEM_INFO	psSystemInfo
	);

_EXPORT BOOL WINAPI EmProfile_getMemoryProfile(
	LPCSTR		lpszMemName,
	pEM_PROFILE	pEmProfile
	);

_EXPORT BOOL WINAPI EmProfile_setMemoryProfile(
	LPCSTR		lpszMemName,
	pEM_PROFILE	pEmProfile
	);

_EXPORT BOOL WINAPI EmProfile_deleteMemoryProfile(
	LPCSTR		lpszMemName
	);

_EXPORT BOOL WINAPI EmProfile_existMemoryProfile(
	LPCSTR	lpszMemName
	);

_EXPORT BOOL WINAPI EmProfile_enumMemoryName(
	DWORD	iValue,
	LPSTR	lpszMemName,
	DWORD	dwStringSize
	);

_EXPORT BOOL WINAPI EmProfile_enumAccessMethod(
	DWORD	iValue,
	LPSTR	lpszAccessMethodName,
	DWORD	dwStringSize
	);


#undef _EXTERN
#undef _EXPORT

#include <EmError.h>

#ifdef __cplusplus
}
#endif

#endif //_EVENT_MEMORY_SYSTEM_H_
