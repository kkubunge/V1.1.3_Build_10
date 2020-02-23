/*
******************************************************************************
(c) Copyright OMRON Corporation 1996
	All Rights Reserved

TITLE:			EmError.h
VERSION:		1.0
DESCRIPTION:	EventMemory library header file
				Must be included when creating applications that will use the EventMemory

DATE:			02,Apr.1996
******************************************************************************
*/

#ifndef	_EVENT_MEMORY_ERROR_CODE_H_
#define	_EVENT_MEMORY_ERROR_CODE_H_

#include <EmResrc.h>

//Macro definition to convert error codes obtained with the GetLastError function and resource designators
#define	EM_APPLICATION_LAST_ERROR_BIT	(DWORD)(0x20000000)
#define mEmToErrorCode(dwResourceID)	(DWORD)(dwResourceID | EM_APPLICATION_LAST_ERROR_BIT)
#define mEmToResourceID(dwErrorCode)	(DWORD)(dwErrorCode & ~EM_APPLICATION_LAST_ERROR_BIT)

//List of error codes obtained with the GetLastError function in an EventMemory system

//Event port name character string length is invalid.
#define EM_ERROR_INVALID_EVENT_PORT_NAME			mEmToErrorCode(IDS_InvalidEventPortName)

//No event ports available.
#define EM_ERROR_NO_MORE_EVENT_PORT					mEmToErrorCode(IDS_NoMoreEventPort)

//Invalid EventMemory event handle
#define EM_ERROR_INVALID_EVENT_HANDLE				mEmToErrorCode(IDS_InvalidEventHandle)

//EventMemory operating version is undefined.
#define EM_ERROR_NO_SET_EXECUTE_VERSION				mEmToErrorCode(IDS_NoSetExecuteVersion)

//Exception error generated during processing.
#define EM_ERROR_OUTBREAK_OF_EXCEPTION				mEmToErrorCode(IDS_OutbreakOfExceptionEm)

//Base object release failure.
#define EM_ERROR_FAIL_IN_DETACH_BASE_OBJECT			mEmToErrorCode(IDS_FailInDetachBaseObject)

//Invalid number of bytes specified for event send (at event send).
#define EM_ERROR_OUT_OF_EVENT_RANGE_ON_SEND			mEmToErrorCode(IDS_OutOfEventRangeOnSend)

//Send target event port not found.
#define EM_ERROR_NO_EVENT_PORT_TO_SEND				mEmToErrorCode(IDS_NoEventPortToSend)

//Base object send failure.
#define EM_ERROR_FAIL_IN_SEND_BASE_OBJECT			mEmToErrorCode(IDS_FailInSendBaseObject)

//Invalid base object data received.
#define EM_ERROR_ILLEGAL_BASE_OBJECT				mEmToErrorCode(IDS_illegalBaseObject)

//Timeout waiting to receive event.
#define EM_ERROR_OUTBREAK_OF_TIMEOUT_FOR_EVENT		mEmToErrorCode(IDS_OutbreakOfTimeoutForEvent)

//Event receive wait failure.
#define EM_ERROR_FAIL_IN_RECEIVE_EVENT				mEmToErrorCode(IDS_FailInReceiveEvent)

//Cannot change; operating version already locked.
#define EM_ERROR_EXECUTE_VERSION_ALREADY_LOCKED		mEmToErrorCode(IDS_ExecuteVersionAlreadylocked)

//Cannot execute; version not supported.
#define EM_ERROR_NOT_SUPPORTED_VERSION				mEmToErrorCode(IDS_NotSupportedVersionEm)

//Memory port name character string length is invalid.
#define EM_ERROR_INVALID_MEMORY_PORT_NAME			mEmToErrorCode(IDS_InvalidMemoryPortName)

//Memory port already opened.
#define EM_ERROR_ALREADY_OPENED_MEMORY_PORT			mEmToErrorCode(IDS_AlreadyOpenedMemoryPort)

//Invalid memory size set.
#define EM_ERROR_ILLEGAL_MEMORY_SIZE				mEmToErrorCode(IDS_illegalMemorySize)

//File create/open failure.
#define EM_ERROR_FAIL_IN_OPEN_FILE					mEmToErrorCode(IDS_FailInOpenFile)

//No memory ports available.
#define EM_ERROR_NO_MORE_MEMORY_PORT				mEmToErrorCode(IDS_NoMoreMemoryPort)

//Shared memory attach failure.
#define EM_ERROR_FAIL_IN_ATTACH_MEMORY				mEmToErrorCode(IDS_FailInAttachMemoryEm)

//Memory already exists.
#define EM_ERROR_ALREADY_EXISTED_MEMORY				mEmToErrorCode(IDS_AlreadyExistedMemory)

//Invalid EventMemory memory handle.
#define EM_ERROR_INVALID_MEMORY_HANDLE				mEmToErrorCode(IDS_InvalidMemoryHandle)

//Memory port record release failure.
#define EM_ERROR_FAIL_IN_RELEASE_MEMORY_RECORD		mEmToErrorCode(IDS_FailInReleaseMemoryRecord)

//Shared memory detach failure.
#define EM_ERROR_FAIL_IN_DETACH_MEMORY				mEmToErrorCode(IDS_FailInDetachMemory)

//Invalid construct to specify EventMemory address.
#define EM_ERROR_INVALID_EM_ADDRESS					mEmToErrorCode(IDS_InvalidEmAddress)

//Specified start address exceeds range.
#define EM_ERROR_FIRST_OFFSET_OUT_OF_MEMORY_RANGE	mEmToErrorCode(IDS_FirstOffsetOutOfMemoryRange)

//Last address exceeds range.
#define EM_ERROR_LAST_OFFSET_OUT_OF_MEMORY_RANGE	mEmToErrorCode(IDS_LastOffsetOutOfMemoryRange)

//Buffer size smaller than read data.
#define EM_ERROR_INSUFFICIENT_READ_BUFFER			mEmToErrorCode(IDS_InsufficientReadBuffer)

//Failed to obtain Mutex authorization.
#define EM_ERROR_FAIL_IN_GET_MUTEX					mEmToErrorCode(IDS_FailInGetMutex)

//Insufficient write data.
#define EM_ERROR_INSUFFICIENT_WRITE_DATA			mEmToErrorCode(IDS_InsufficientWriteData)

//Data other than 0/1 specified for bit data.
#define EM_ERROR_BIT_NEITHER_ONE_NOR_ZERO			mEmToErrorCode(IDS_BitNeitherOneNorZero)

//Event send failure during data write.
#define EM_ERROR_FAIL_IN_SEND_EVENT_ON_WRITE		mEmToErrorCode(IDS_FailInSendEventOnWrite)

//Word not specified for data type in wide-area event generation condition settings.
#define EM_ERROR_NOT_WORD_TYPE_OF_FACTOR			mEmToErrorCode(IDS_NotWordTypeOfFactor)

//All wide event generation conditions already used.
#define EM_ERROR_NO_MORE_WIDE_CONDITION_POOL		mEmToErrorCode(IDS_NoMoreWideConditionPool)

//Invalid construct data specifying event memory address to be sent as event data.
#define EM_ERROR_INVALID_EM_ADDRESS_FOR_DATA		mEmToErrorCode(IDS_InvalidEmAddressForData)

//Invalid number of bytes specified for event send (at event generation condition setting).
#define EM_ERROR_OUT_OF_EVENT_RANGE_ON_CONDITION	mEmToErrorCode(IDS_OutOfEventRangeOnCondition)

//Processing requested for undefined condition ID.
#define EM_ERROR_NOT_SET_CONDITION_ID				mEmToErrorCode(IDS_NotSetConditionID)

//Failure attaching event generation condition to free list.
#define EM_ERROR_FAIL_IN_ATTACH_CONDITION_TO_FREE	mEmToErrorCode(IDS_FailInAttachConditionToFree)

//Number of data other than 1 specified at event generation condition setting.
#define EM_ERROR_NOT_ONE_NUMBER_OF_FACTOR			mEmToErrorCode(IDS_NotOneNumberOfFactor)

//Bit specified for data type at event generation condition setting.
#define EM_ERROR_BIT_TYPE_ON_CONDITION				mEmToErrorCode(IDS_BitTypeOnCondition)

//Odd address specified at double-word event generation condition setting.
#define EM_ERROR_ODD_OFFSET_ON_DWORD_CONDITION		mEmToErrorCode(IDS_OddOffsetOnDwordCondition)

//Event generation condition memory attach failure.
#define EM_ERROR_FAIL_IN_ATTACH_CONDITION_TO_MEMORY	mEmToErrorCode(IDS_FailInAttachConditionToMemory)

//All normal event generation conditions already used.
#define EM_ERROR_NO_MORE_NORMAL_CONDITION_POOL		mEmToErrorCode(IDS_NoMoreNormalConditionPool)

//Invalid window handle specified.
#define EM_ERROR_NOT_WINDOW_HANDLE					mEmToErrorCode(IDS_NotWindowHandle)

//Message already set to be posted to thread.
#define EM_ERROR_ALREADY_SET_TO_POST_THREAD			mEmToErrorCode(IDS_AlreadySetToPostThread)

//Message already set to be posted to window.
#define EM_ERROR_ALREADY_SET_TO_POST_WINDOW			mEmToErrorCode(IDS_AlreadySetToPostWindow)

//Failure posting message to thread.
#define EM_ERROR_FAIL_IN_POST_THREAD_MESSAGE		mEmToErrorCode(IDS_FailInPostThreadMessage)

//Failure posting message to window.
#define EM_ERROR_FAIL_IN_POST_WINDOW_MESSAGE		mEmToErrorCode(IDS_FailInPostWindowMessage)

//Invalid constant1 to be used for compare to judge true/false value.
#define EM_ERROR_INVALID_CONSTANT1					mEmToErrorCode(IDS_InvalidConstant1)

//Invalid constant2 to be used for compare to judge true/false value.
#define EM_ERROR_INVALID_CONSTANT2					mEmToErrorCode(IDS_InvalidConstant2)

//Event port management (by process) data error.
#define EM_ERROR_PROCESS_EVENT_INFO_OUT_OF_ORDER	mEmToErrorCode(IDS_ProcessEventInfoOutOfOrder)

//Memory allocation failure.
#define EM_ERROR_FAIL_IN_ALLOC_MEMORY				mEmToErrorCode(IDS_FailInAllocMemory)

//File specified in registry is too long.
#define EM_ERROR_TOO_LONG_FILE_PATH					mEmToErrorCode(IDS_TooLongFilePath)

//No event generation condition setting file.
#define EM_ERROR_NOT_EXIST_CONDITION_FILE			mEmToErrorCode(IDS_NotExistConditionFile)

//Event port already open.
#define EM_ERROR_ALREADY_OPENED_EVENT_PORT			mEmToErrorCode(IDS_AlreadyOpenedEventPort)

//All events already used.
#define EM_ERROR_NO_MORE_EVENT_POOL					mEmToErrorCode(IDS_NoMoreEventPool)

//Buffer to small to receive data.
#define EM_ERROR_INSUFFICIENT_RECEIVE_BUFFER		mEmToErrorCode(IDS_InsufficientReceiveBuffer)

//Event condition cannot be set for this memory.
#define EM_ERROR_DISABLE_SET_EVENT_COND				mEmToErrorCode(IDS_DisableSetEventCond)

//Specified access method does not exist.
#define EM_ERROR_NOT_EXIST_ACCESS_METHOD		mEmToErrorCode(IDS_NotExistAccessMethod)

//Open method execution failure for access method.
#define EM_ERROR_FAIL_OPEN_METHOD				mEmToErrorCode(IDS_FailOpenMethod)

//Close method execution failure for access method.
#define EM_ERROR_FAIL_CLOSE_METHOD				mEmToErrorCode(IDS_FailCloseMethod)

//Read method execution failure for access method.
#define EM_ERROR_FAIL_READ_METHOD				mEmToErrorCode(IDS_FailReadMethod)

//Write method execution failure for access method.
#define EM_ERROR_FAIL_WRITE_METHOD				mEmToErrorCode(IDS_FailWriteMethod)

//Read method execution failure for access method.
#define EM_ERROR_FAIL_READEX_METHOD				mEmToErrorCode(IDS_FailReadExMethod)

//Write method execution failure for access method.
#define EM_ERROR_FAIL_WRITEEX_METHOD				mEmToErrorCode(IDS_FailWriteExMethod)

//Judge method execution failure for access method.
#define EM_ERROR_FAIL_JUDGE_METHOD				mEmToErrorCode(IDS_FailJudgeMethod)

#endif //_EVENT_MEMORY_ERROR_CODE_H_
