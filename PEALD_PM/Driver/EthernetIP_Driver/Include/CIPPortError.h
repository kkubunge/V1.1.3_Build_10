/*****************************************************************************/
/* (C)	Copyright OMRON  Corporation 2008	 								 */
/*		All Rights Reserved	 												 */
/*****************************************************************************/

#ifndef	_CIP_PORT_ERROR_CODE_H_
#define	_CIP_PORT_ERROR_CODE_H_

#include <CIPCorePortRess.h>

//Macro definition to convert error codes obtained with the GetLastError function and resource designators
#define	CIP_APPLICATION_LAST_ERROR_BIT	(DWORD)(0x20000000)
#define mCipIsApplicationError(dwErrorCode)	(BOOL)(dwErrorCode & CIP_APPLICATION_LAST_ERROR_BIT)
#define mCipToErrorCode(dwResourceID)	(DWORD)(dwResourceID | CIP_APPLICATION_LAST_ERROR_BIT)
#define mCipToResourceID(dwErrorCode)	(DWORD)(dwErrorCode & ~CIP_APPLICATION_LAST_ERROR_BIT)


#define CIPPORT_ERROR_OUT_OF_RANGE					mCipToErrorCode(IDS_Port_NoMorePort)
#define	CIPPORT_ERROR_INVALID_SAVE_DATA				mCipToErrorCode(IDS_Port_InvalidSaveData)
#define	CIPPORT_ERROR_INVALID_PARAMETER				mCipToErrorCode(IDS_Port_InvalidParameter)
#define CIPPORT_ERROR_OUTBREAK_OF_EXCEPTION			mCipToErrorCode(IDS_Port_OutbreakOfException)


#endif _CIP_PORT_ERROR_CODE_H_
