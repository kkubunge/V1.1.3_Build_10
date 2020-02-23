/*
 *	Copyright (c) 1995,96 OMRON Corporation.
 *	All rights reserved.
 */

/*	NOTICE : THIS FILE INCLUDES JAPANESE CODE */

#ifndef _FINSGW_H_
#define _FINSGW_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#include <FinsHead.h>
#include <FinsLog.h>

/*
=====================================================================
 C P U  &  N E T W O R K   P R O V I D E R S   N A M  E 
=====================================================================
*/
#define FG_UNITNAME_CPU				"CpuUnit"

#define FG_UNITNAME_SYSMACLINK		"SysmacLink"
#define FG_UNITNAME_ETHERNET		"EtnUnit"
#define	FG_UNITNAME_SERIAL			"SerialUnit"
#define	FG_UNITNAME_CONTROLLERLINK	"Controller Link"
#define	FG_UNITNAME_SYSNET			"Sysnet Unit"
#define FG_UNITNAME_SYSMACALPHA		"SysmacBoard"
#define	FG_UNITNAME_PCICONTROLLERLINK	"CLK (PCI)"
#define	FG_UNITNAME_PCISYSMACLINK		"SLK (PCI)"

/*
=====================================================================
 B A S I C   C O N S T A N T   D E F I N I T I O N S
=====================================================================
*/
/*
 * Function return value definitions (Return values for functions with the Fins_ prefix)
 */
#define C_RETURN_CODE_SUCCESS					0
#define C_RETURN_CODE_FAILED					-1

/*
 * Definition of icfBits value used with FinsHead_compose function
 */
#define C_NEED_RESPONSE					C_FINS_PCI_ICF_NEED_RESPONSE
#define C_NO_ACK_RESPONSE				C_FINS_PCI_ICF_NO_ACK_RESPONSE

/*
 * Initialization routine for version control
 */
#define FGW_STARTUP()	Fins_requestVersion(FGW_CURRENT_MAJOR_VERSION, FGW_CURRENT_MINOR_VERSION)
/*
=====================================================================
 B A S I C   T Y P E   D E F I N I T I O N S
=====================================================================
*/
typedef HANDLE	HNET;


//Store the Unit connection data
typedef struct _Connect_Info_Struc {
	BYTE		byUnitAddr;	//Unit address
	char		szUnitName[MAX_COMMENT_SIZE]; //Unit name
} CONNECTINFO, *PCONNECTINFO;

typedef struct _Network_Info_Struc {
	char		szName[MAX_COMMENT_SIZE];	//Network name
	FINSADDR	transport; // FINS transport address
	DWORD		dwMessageSize; //Message size without header	
} NETINFO, *PNETINFO;

/* 
 * Call back function called when throwing away FINS message
 *		nErrCode:	Error code	(Same as code used in error log)
 *		pFinsPci:	FINS header	(ICF to SID)
 *		lpvMessage:	FINS data	(MRC|SRC|MRES|SRES to end)
 *		dwSize:		FINS data size
 */
typedef VOID (* ERR_PROC)(ERRLOG	nErrCode, \
						PFINSPCI	pFinsPci, \
						LPVOID		lpvMessage, \
						DWORD		dwSize);

/*
******************************************************************************
 	P R O T O T Y P E S
******************************************************************************
*/

#ifdef	_EXPORT_FINS_API_
#define _EXPORT	__declspec(dllexport)
#define _EXTERN
#else
#define _EXPORT	__declspec(dllimport)
#define _EXTERN	extern
#endif 

/*
;-----------------------------------------------------------------------------
; Fins_getVersion()
;
; DESCRIPTION:	Obtain the current DLL release version
;
; ENTRY:		None
;
; EXIT:			Returns the structure storing the version
;=============================================================================
*/
_EXPORT FGW_VERSION WINAPI Fins_getVersion(VOID);
/*
;-----------------------------------------------------------------------------
; Fins_requestVersion()
;
; DESCRIPTION:	Requests operation as specified version
;				If the return value is FALSE, all the FINS-API return errors.
;
; ENTRY:		byMajor:	Major version
;				byMinor:	Minor version
;
; EXIT:			If operation is possible as the specified version, TRUE;
;				if not, FALSE.
;=============================================================================
*/
_EXPORT BOOL WINAPI Fins_requestVersion(
						BYTE		byMajor, 
						BYTE		byMinor
					);
/*
;-----------------------------------------------------------------------------
; Fins_new()
;
; DESCRIPTION:	Creates network handle
;
; ENTRY:		byUnitAddr:		Unit address to use
;				lpszUnitName:	Reference name regardless of address
;
; EXIT:			Returns network handle for normal exit,
;				NULL for error.
;=============================================================================
*/
_EXPORT HNET WINAPI	Fins_new(
						BYTE		byUnitAddr, 
						LPCSTR		lpszUnitName
					);
/*
;-----------------------------------------------------------------------------
; Fins_newReservedUnit()
;
; DESCRIPTION:	Creates network handle
;				FinsGateway automatically assigns an available unit address
;
; ENTRY:		lpszUnitName:	Reference name regardless of unit address
;
; EXIT:			Returns network handle for normal exit,
;				NULL for error.
;=============================================================================
*/
_EXPORT HNET WINAPI Fins_newReservedUnit(
						LPCSTR		lpszUnitName
					);
/*
;-----------------------------------------------------------------------------
; Fins_delete()
;
; DESCRIPTION:	Deletes network handle
;
; ENTRY:		hNet:	Network handle
;
; EXIT:			Returns C_RETURN_CODE_SUCCESS for normal exit.
;				If the network handle is invalid, returns C_RETURN_CODE_FAILED.
;=============================================================================
*/
_EXPORT int	WINAPI Fins_delete(
						HNET 		hNet
					);

					
_EXPORT int WINAPI Fins_deleteForce(
	BYTE byUnitAddr
	);
					
/*
;-----------------------------------------------------------------------------
; Fins_sendData()
;
; DESCRIPTION:	Sends FINS message
;
; ENTRY:		hNet:		Network handle
;				lpHead:		FINS header
;				lpvMessage:	Send data
;				dwMesLen:	Number of bytes of send data
;
; EXIT:			Returns the size of the send data (message).
;				If the send/receive queue is full and the response cannot be sent,
;				returns C_RETURN_CODE_FAILED.
;=============================================================================
*/
_EXPORT int	WINAPI Fins_sendData(
						HNET		hNet, 
						LPFINSHEAD	lpHead, 
						LPVOID		lpvMessage, 
						DWORD		dwMesLen
					);
/*
;-----------------------------------------------------------------------------
; Fins_receiveData()
;
; DESCRIPTION:	Receives FINS message
;
; ENTRY:		hNet:		Network handle
;				lpHead:		FINS header
;				lpvMessage:	Buffer to store receive data
;				dwMesLen:	Number of bytes of buffer to store receive data
;				dwTimeLimit: Receive wait time
;							->For no timeout, INFINITE
;							->Use 0 for receive check.
;
; EXIT:			Returns the size of the receive data (message).
;				For timeout, returns C_RETURN_CODE_FAILED.
;=============================================================================
*/
_EXPORT	int	WINAPI Fins_receiveData(
						HNET		hNet, 
						LPFINSHEAD	lpHead, 
						LPVOID		lpvMessage, 
						DWORD		dwMesLen, 
						DWORD		dwTimeLimit
					);
/*
;-----------------------------------------------------------------------------
; Fins_changeNetworkProvider()
;
; DESCRIPTION:	Reregisters a registered unit, as a Communication Unit
;
; ENTRY:		hNet:			Network handle
;				byNodeAddr:		Communication Unit node address
;				dwMessageSize:	Maximum message size for the Communication Unit
;				lpfnPutEventLog:	Callback function for error log		
;
; EXIT:			Returns C_RETURN_CODE_SUCCESS for normal exit.	
;				Returns C_RETURN_CODE_FAILED for failure.		       
;=============================================================================
*/
_EXPORT	int	WINAPI Fins_changeNetworkProvider(
						HNET		hNet, 
						BYTE		byNodeAddr, 
						DWORD		dwMessageSize,
						ERR_PROC	lpfnPutEventLog
					);
/*
;-----------------------------------------------------------------------------
; Fins_requestRoutingChangeEvent()
;
; DESCRIPTION:	Requests event at routing table change
;
; ENTRY:		hNet:			Network handle
;
; EXIT:			Returns C_RETURN_CODE_SUCCESS for normal exit.
;				Returns C_RETURN_CODE_FAILED for failure.		       
;=============================================================================
*/
_EXPORT int	WINAPI Fins_requestRoutingChangeEvent(
						HNET 		hNet 
					);
/*
;-----------------------------------------------------------------------------
; Fins_getTaddress()
;
; DESCRIPTION:	Returns FINS transport address	
;
; ENTRY:		hNet:			Network handle
;				lpFinsTaddress:	FINS transport address
;
; EXIT:			Returns C_RETURN_CODE_SUCCESS for normal exit.	
;				Returns C_RETURN_CODE_FAILED for failure.			       
;=============================================================================
*/
_EXPORT	int	WINAPI Fins_getTaddress(
						HNET		hNet, 
						LPFINSADDR	lpFinsTaddress 
					);
/*
;-----------------------------------------------------------------------------
; FinsHead_compose()
;
; DESCRIPTION:	Creates FINS command header
;
; ENTRY:		lpHead:	Command header
;				icfBits:	ICF
;				lpAddr:		FINS address
;				nSid:		SID
;
; EXIT:			Returns SID for normal exit.	
;				Returns C_RETURN_CODE_FAILED for failure.			       
;=============================================================================
*/
_EXPORT int	WINAPI FinsHead_compose(
						LPFINSHEAD	lpHead, 
						BYTE		icfBits, 
						LPFINSADDR	lpAddr, 
						int			nSid
					);
/*
;-----------------------------------------------------------------------------
; FinsHead_composeLocalMessage()
;
; DESCRIPTION:	Creates FINS command header
;				Distributes to localDU without referring to FINS address.
;
; ENTRY:		
;				hNet:		Network handle
;				byLocalDU:	Local send target
;				lpHead:		Command header
;				icfBits:	ICF
;				lpAddr:		FINS address
;				nSid:		SID
;
; EXIT:			Returns SID for normal exit.
;				Returns C_RETURN_CODE_FAILED for failure.			       
;=============================================================================
*/
_EXPORT int	WINAPI FinsHead_composeLocalMessage(
						HNET		hNet,
						BYTE		byLocalDU, 
						LPFINSHEAD	lpHead, 
						BYTE		icfBits, 
						LPFINSADDR	lpAddr, 
						int			nSid
					);
/*
;-----------------------------------------------------------------------------
; FinsHead_composeResponse()
;
; DESCRIPTION:	Creates FINS response header. Error is generated if input is response header.
;				
;
; ENTRY:		lpResHead:	Response header
;				lpComHead:	Command header		
;
; EXIT:			Returns C_RETURN_CODE_SUCCESS for normal exit.	
;				Returns C_RETURN_CODE_FAILED for failure.			       
;=============================================================================
*/
_EXPORT	int	WINAPI FinsHead_composeResponse(
						LPFINSHEAD	lpResHead,
						LPFINSHEAD	lpComHead
					);
/*
;-----------------------------------------------------------------------------
; Fins_queryRelayAddress()
;
; DESCRIPTION:	Determines the gateway node address from the FINS header data.
;				This address is stored in gwPci.
;
; ENTRY:		hNet:			Network handle
;				lpHead:			FINS header
;
; EXIT:			Returns C_RETURN_CODE_SUCCESS for normal exit.
;				Returns FINS error log code for errors.
;=============================================================================
*/
_EXPORT	ERRLOG	WINAPI Fins_queryRelayAddress(
						HNET		hNet,
						LPFINSHEAD	lpHead
					);

/*
;-----------------------------------------------------------------------------
; Fins_getMessageSize()
;
; DESCRIPTION:	Returns maximum FINS message size (excluding header)
;
; ENTRY:		hNet:			Network handle
;
; EXIT:			Returns maximum message size for normal exit.
;				Returns C_RETURN_CODE_FAILED for failure.
;=============================================================================
*/
_EXPORT	int	WINAPI Fins_getMessageSize(
						HNET		hNet
					);
/*
;-----------------------------------------------------------------------------
; Fins_IsOwnMessage()
;
; DESCRIPTION:	Determines whether FINS message is for local node
;
; ENTRY:		hNet:			Network handle
;				lpHead:			FINS message header (including finsPci, gwPci)
;
; EXIT:			For local message, TRUE; otherwise, FALSE.			       
;=============================================================================
*/
_EXPORT BOOL WINAPI Fins_IsOwnMessage(
						HNET		hNet,
						LPFINSHEAD	lpHead
					);
/*
;-----------------------------------------------------------------------------
; Fins_sendEventData()
;
; DESCRIPTION:	Sends EVENTS message
;
; ENTRY:		hNet:		Network handleÅB
;				usEventType: Event type
;				byLocalDU:	Local send target
;				lpvMessage:	Send data
;				dwMesLen:	Number of bytes of send data
;
; EXIT:			Returns size of send data
;				If send/receive queue is full and there is no target unit, 
;				returns C_RETURN_CODE_FAILED.
;=============================================================================
*/
_EXPORT int	WINAPI Fins_sendEventData(	
						HNET		hNet, 
						USHORT		usEventType,
						BYTE		byLocalDU,
						LPVOID		lpvMessage,
						DWORD		dwMesLen
					);
/*
;-----------------------------------------------------------------------------
; Fins_sendIoctlData()
;
; DESCRIPTION:	Sends IOCTL message
;
; ENTRY:		hNet:		Network handleÅB
;				usIoctlType:IOCTL type
;				byLocalDU:	Local send target
;				lpvMessage:	Send data
;				nMesLen:	Number of bytes of send data
;
; EXIT:			Returns send data size
;				If send/receive queue is full and there is no target unit, 
;				returns C_RETURN_CODE_FAILED.
;=============================================================================
*/
_EXPORT int WINAPI Fins_sendIoctlData(HNET hNet,
						USHORT		usIoctlType,//Defined for each network, independently
						BYTE		byLocalDU,
						LPVOID		lpvMessage,
						DWORD		dwMesLen
					);
/*
;-----------------------------------------------------------------------------
; Fins_BroadcastEventToNetworkProviders()
;
; DESCRIPTION:	Broadcasts event protocol message to network provider
;				
;
; ENTRY:		hNet:			Network handleÅB
;				lpHead:			FINS header
;				lpvMessage:		Send data (event message)
;				dwMesLen:		Number of bytes of send data
;				byExceptUnit:	Units to exclude from broadcast
;
; EXIT:			Returns C_RETURN_CODE_SUCCESS for successful broadcast.
;				If the send/receive queue is full and the response cannot be sent,
;				returns C_RETURN_CODE_FAILED.
;=============================================================================
*/
_EXPORT int WINAPI Fins_BroadcastEventToNetworkProviders(
						HNET		hNet, 
						LPVOID		lpvMessage,
						DWORD		dwMesLen,
						BYTE		byExceptUnit
					);
/*
;-----------------------------------------------------------------------------
; Fins_getEventHandle()
;
; DESCRIPTION:	Returns the FINS receive notification event handle
;
; ENTRY:		hNet:	Network handle
;
; EXIT:			Returns the event object handle
;=============================================================================
*/
_EXPORT HANDLE	WINAPI Fins_getEventHandle(
						HNET		hNet
					);
/*
;-----------------------------------------------------------------------------
; Fins_setMessageOnArrival()
;
; DESCRIPTION:	Sets message to post in window
;
; ENTRY:		hNet:		Network handle
;				hWnd:		Window handle
;				uMsg:		Message type
;
; EXIT:			Returns TRUE if successful, FALSE if failed.				       
;=============================================================================
*/
_EXPORT BOOL WINAPI Fins_setMessageOnArrival(
						HNET		hNet, 
						HWND		hWnd,
						UINT		uMsg
					);
/*
;-----------------------------------------------------------------------------
; Fins_setThreadMessageOnArrival()
;
; DESCRIPTION:	Sets message to post to thread
;
; ENTRY:		hNet:		Network handle
;				dwThreadId:	Thread ID
;				uMsg:		Message type
;
; EXIT:			Returns TRUE if successful, FALSE if failed.				       
;=============================================================================
*/
_EXPORT BOOL WINAPI Fins_setThreadMessageOnArrival(
						HNET		hNet, 
						DWORD		dwThreadId, 
						UINT		uMsg
					);
/*
;-----------------------------------------------------------------------------
; Fins_clearMessageOnArrival()
;
; DESCRIPTION:	Cancels message post to window/thread	
;
; ENTRY:		hNet:		Network handle
;
; EXIT:			Returns TRUE if successful, FALSE if failed.				       
;=============================================================================
*/
_EXPORT BOOL WINAPI Fins_clearMessageOnArrival(
						HNET		hNet
					);

/*
;-----------------------------------------------------------------------------
; Fins_getNetworkInfo()
;
; DESCRIPTION:	Obtains currently useable network (provider) data.
;				Network data:	network provider name,
;										network address,
;										node address,
;										unit address
;										message size excluding header	
; ENTRY:		hNet:		Network handle
;				pNetInfo:	Structure to store network data
;				cbNetworks: Number of networks that can be stored
;
; EXIT:			Returns number of networks for normal exit.
;				Returns C_RETURN_CODE_FAILED for failure.
;=============================================================================
*/
_EXPORT int WINAPI Fins_getNetworkInfo(
						HNET		hNet, 
						PNETINFO	pNetInfo, 
						DWORD		cbNetworks
					);
/*
;-----------------------------------------------------------------------------
; Fins_getConnectUnits()
;
; DESCRIPTION:	Obtains currently connected unit data
;				Connection data:	unit address,
;								unit name
;
; ENTRY:		hNet:			Network handle
;				pConnectInfo:	Structure to store unit data
;				cbUnits:	 	Number of units that can be stored
;
; EXIT:			Returns number of units for normal exit.
;				Returns C_RETURN_CODE_FAILED for failure.
;=============================================================================
*/
_EXPORT int WINAPI Fins_getConnectUnits(
						HNET			hNet, 
						PCONNECTINFO	pConnectInfo, 
						DWORD			cbUnits
					);

#undef _EXTERN
#undef _EXPORT

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif	/*_FINSGW_H_*/
