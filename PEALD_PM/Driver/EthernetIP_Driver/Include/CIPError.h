/*****************************************************************************/
/* (C)	Copyright OMRON  Corporation 2008	 								 */
/*		All Rights Reserved	 												 */
/*****************************************************************************/

#ifndef	_CIP_ERROR_CODE_H_
#define	_CIP_ERROR_CODE_H_

#include <CIPCoreApiRess.h>

//Macro definition to convert error codes obtained with the GetLastError function and resource designators
#define	CIP_APPLICATION_LAST_ERROR_BIT	(DWORD)(0x20000000)
#define mCipIsApplicationError(dwErrorCode)	(BOOL)(dwErrorCode & CIP_APPLICATION_LAST_ERROR_BIT)
#define mCipToErrorCode(dwResourceID)	(DWORD)(dwResourceID | CIP_APPLICATION_LAST_ERROR_BIT)
#define mCipToResourceID(dwErrorCode)	(DWORD)(dwErrorCode & ~CIP_APPLICATION_LAST_ERROR_BIT)

#define	CIP_APPLICATION_CIP_ERROR_BASE	1000
#define	CIP_APPLICATION_CIP_ERROR_LATEST	(1041 - CIP_APPLICATION_CIP_ERROR_BASE)

#define mCipRespCodeToResourceID(generalStatus)	(DWORD)(generalStatus + CIP_APPLICATION_CIP_ERROR_BASE)
#define mCipRespCodeToErrorCode(generalStatus)	(DWORD)(mCipRespCodeToResourceID(generalStatus) | CIP_APPLICATION_LAST_ERROR_BIT)
#define mCipErrorCodeToRespCode(dwErrorCode)	(DWORD)((dwErrorCode & ~CIP_APPLICATION_LAST_ERROR_BIT) - CIP_APPLICATION_CIP_ERROR_BASE)

#define CIP_ERROR_NO_MORE_APP_HANDLE					mCipToErrorCode(IDS_NoMoreAppHandle)
#define CIP_ERROR_OUTBREAK_OF_EXCEPTION					mCipToErrorCode(IDS_OutbreakOfExceptionCIP)
#define CIP_ERROR_FAIL_IN_ATTATCH_MEMORY				mCipToErrorCode(IDS_FailInAttachMemoryCIP)
#define CIP_ERROR_INVALID_APPLICATION_HANDLE			mCipToErrorCode(IDS_InvalidApplicationHandle)
#define CIP_ERROR_INVALID_CONNECTION_HANDLE				mCipToErrorCode(IDS_InvalidConnectionHandle)
#define CIP_ERROR_INVALID_EVENTID						mCipToErrorCode(IDS_InvalidEventID)
#define CIP_ERROR_INVALID_CELL							mCipToErrorCode(IDS_InvalidCell)
#define CIP_ERROR_INVALID_MSGTYPE						mCipToErrorCode(IDS_InvalidMsgType)
#define CIP_ERROR_INVALID_IOVECTOR						mCipToErrorCode(IDS_InvalidIoVector)
#define CIP_ERROR_IOVECTOR_TOO_LARGE					mCipToErrorCode(IDS_IoVectorTooLarge)
#define CIP_ERROR_NOT_ENOUGH_RECEIVE_BUFFER				mCipToErrorCode(IDS_NotEnoughReceiveBuffer)
#define CIP_ERROR_INVALID_VERSION						mCipToErrorCode(IDS_InvalidVersiion)
#define CIP_ERROR_INVALID_APPLICATION_NAME				mCipToErrorCode(IDS_InvalidApplicatioName)
#define CIP_ERROR_INVALID_RECEIVE_SERVICE_CODE			mCipToErrorCode(IDS_InvalidReceiveServiceCode)
#define CIP_ERROR_INVALID_CLASSID						mCipToErrorCode(IDS_InvalidClassID)
#define CIP_ERROR_INVALID_NETWORK_PATH					mCipToErrorCode(IDS_InvalidNetworkPath)
#define CIP_ERROR_NOT_ENOUGH_BUFFER						mCipToErrorCode(IDS_NotEnoughBuffer)
#define CIP_ERROR_NOT_SUPPORTED_32BIT_LOGICAL_ADDRESS	mCipToErrorCode(IDS_NotSupported32BitLogicalAddress)
#define CIP_ERROR_INVALID_TAGNAME						mCipToErrorCode(IDS_InvalidTagname)
#define CIP_ERROR_INVALID_REQUEST_PATH					mCipToErrorCode(IDS_InvalidRequestPath)
#define CIP_ERROR_INVALID_SHMEM_HANDLE					mCipToErrorCode(IDS_InvalidShmemHandle)
#define CIP_ERROR_NETWORK_BUSY							mCipToErrorCode(IDS_NetworkBusy)
#define CIP_ERROR_INVALID_PARAMETER						mCipToErrorCode(IDS_InvalidParameter)
#define CIP_ERROR_NOT_OPEN_PORT							mCipToErrorCode(IDS_NotOpenPort)
#define CIP_ERROR_ALREADY_OPEN_PORT						mCipToErrorCode(IDS_AlreadyOpenPort)
#define CIP_ERROR_INVALID_PORT_NUMBER					mCipToErrorCode(IDS_InvalidPortNumber)
#define CIP_ERROR_FAILED_CLOSE_INTERFACE				mCipToErrorCode(IDS_FailedCloseInterface)
#define CIP_ERROR_FAILED_CREATE_INTERFACE				mCipToErrorCode(IDS_FailedCreateInterface)
#define CIP_ERROR_FAILED_ONLINE_INTERFACE				mCipToErrorCode(IDS_FailedOnlineInterface)
#define CIP_ERROR_FAILED_SET_RESPONSE_WAIT_TIME			mCipToErrorCode(IDS_FailedSetResponseWaitTime)
#define CIP_ERROR_FAILED_CREATE_INSTANCE_IMPROUTER		mCipToErrorCode(IDS_FailedCreateInstanceIMProuter)
#define CIP_ERROR_FAILED_CREATE_INSTANCE_MSGROUTER		mCipToErrorCode(IDS_FailedCreateInstanceMSGrouter)
#define CIP_ERROR_FAILED_REGISTRATION_OBJECT			mCipToErrorCode(IDS_FailedRegistrationObject)
#define CIP_ERROR_RESOURCE_UNAVAILABLE					mCipToErrorCode(IDS_ResourceUnavailable)
#define CIP_ERROR_ROUTING_TABLE_OVERFLOW				mCipToErrorCode(IDS_RoutingTableOverflow)
#define CIP_ERROR_APP_SERVER_OVERFLOW					mCipToErrorCode(IDS_AppServerOverflow)
#define CIP_ERROR_BAD_PARAMETER_VALUE					mCipToErrorCode(IDS_BadParameterValue)
#define CIP_ERROR_BAD_SERVICE							mCipToErrorCode(IDS_BadService)
#define CIP_ERROR_BAD_SERVICE_MODE						mCipToErrorCode(IDS_BadServiceMode)
#define CIP_ERROR_INVALID_PORT_ID						mCipToErrorCode(IDS_InvalidPortID)
#define CIP_ERROR_INVALID_SERVICE_ID					mCipToErrorCode(IDS_InvalidServiceID)
#define CIP_ERROR_SYSTEM_RESOURCE_UNAVAILABLE			mCipToErrorCode(IDS_SystemResourceUnavailable)
#define CIP_ERROR_UNSUPPORTED_REQUEST					mCipToErrorCode(IDS_UnsupportedRequest)
#define CIP_ERROR_BAD_OBJECT_MODE						mCipToErrorCode(IDS_BadObjectMode)
#define CIP_ERROR_UNEXPECTED_FAILED						mCipToErrorCode(IDS_Unexpected_Failed)
#define CIP_ERROR_CLASS_NAME_LENGTH_TOO_LONG			mCipToErrorCode(IDS_ClassNameLengthTooLong)
#define CIP_ERROR_FAILED_IN_SEND_REQUEST_DATA			mCipToErrorCode(IDS_FailedInSendRequestData)
#define CIP_ERROR_FAILED_IN_RECIVE_DATA					mCipToErrorCode(IDS_FailedInReceiveData)
#define CIP_ERROR_DISAGREEMENT_IN_SERVICE_CODE			mCipToErrorCode(IDS_DisagreementInServiceCode)
#define CIP_ERROR_NOT_SUPPORTED_VERSION					mCipToErrorCode(IDS_NotSupportedVersionCIP)
#define CIP_ERROR_VERSION_ALREADY_LOCKED				mCipToErrorCode(IDS_VersionAlreadyLocked)
#define CIP_ERROR_RECEIVE_TIMEOUT						mCipToErrorCode(IDS_RecieveTimeout)
#define CIP_ERROR_NO_PEER_ENTRY							mCipToErrorCode(IDS_NoPeerEntry)


//#define CIP_ERROR_		mCipToErrorCode()
//#define CIP_ERROR_		mCipToErrorCode()



#endif _CIP_ERROR_CODE_H_
