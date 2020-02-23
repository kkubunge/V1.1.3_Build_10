/*
 *	Copyright (c) 1995,96 OMRON Corporation.
 *	All rights reserved.
 */

/*	NOTICE : THIS FILE INCLUDES JAPANESE CODE */

#ifndef _FINSHEAD_H_
#define _FINSHEAD_H_

#include <Fins.h>

/*
=====================================================================
 V E R S I O N   I N F O R M A T I O N
=====================================================================
*/
/*
 *  Release Version 1.00 for "FinsGW32.DLL" 
 */
#define FGW_CURRENT_MAJOR_VERSION			1
#define FGW_CURRENT_MINOR_VERSION			0
#define FGW_CURRENT_REVISION				0

/*
=====================================================================
 B A S I C   D E F I N I T I O N S
=====================================================================
*/

//Comment size (Unit name)	
#define MAX_COMMENT_SIZE  			16					

/*
 * PROTOCOL TYPE ID
 */
// FINS Message
#define C_PROTOCOL_FINS				((USHORT)0x0000)
// EVENT Message
#define C_PROTOCOL_EVENT			((USHORT)0x0001)
// IO CONTROL MessageÅidepend on a Network ProviderÅj
#define C_PROTOCOL_IOCTL			((USHORT)0x0002)

//Temperature Controler RAW Protocol
#define C_PROTOCOL_TEMPERATURE_RAW	((USHORT)20)

//SYSNET/SYSMAC NET C-mode Command
#define C_PROTOCOL_SYSNET_CMODE		((USHORT)30)

// PLC SYSMAC N1 Toolbus
#define C_PROTCOL_TOOLBUS_N1_RAW	((USHORT)31)

//Maximum Types on FinsGateway
#define C_PROTOCOL_MAXTYPES			((USHORT)64)

//Invalid protocol type
#define C_PROTOCOL_INVALID_TYPE		((USHORT)0xFFFF)

//Invalid protocol type judgement macro
//Note: This macro must be modified if a new protocol type is added.
#define mInvalidProtocol(gwpci)		(C_PROTOCOL_MAXTYPES < (gwpci).usProtoType)

/*
 * Event type
 */
// Routing table change notification event
#define C_EVENT_TYPE_ROUTE_CHANGE	0


/*
 * Property type
 */
#define C_PROPERTY_INITIALIZED		((UINT)0x00000000) 

//Local node message
#define C_PROPERTY_TO_LOCAL_UNIT	((UINT)0x00000001) 
//Local network message for a different node
#define C_PROPERTY_TO_OTHER_NODE	((UINT)0x00000002) 
//Message for a different network
#define C_PROPERTY_TO_OTHER_NET		((UINT)0x00000004)
//Local network broadcast message
#define C_PROPERTY_BROADCAST		((UINT)0x00000008)

//Message from application
#define C_PROPERTY_FROM_APPLICATION	((UINT)0x00000010)

//Message distributed referring only to GATEWAY-PCI local SU/DU
#define C_PROPERTY_LOCAL_MESSAGE	((UINT)0x00000100)


/*
=====================================================================
 M A C R O   P R O C E D U R E S
=====================================================================
*/

/* 
 * usage:
 *	 mInitProperty(Var)
 * description:
 *  Initialize properties
 */
#define mInitProperty(gwpci)	(((gwpci).uProperty) &= C_PROPERTY_INITIALIZED)

/* 
 * usage:
 *	 mSetProperty(Var, C_PROPERTY_TO_MY_MESSAGE | C_PROPERTY_LOCAL_MESSAGE)
 * description:
 *  Set/reset specified property type. For multiple, delineate with OR.
 */
#define mSetProperty(gwpci,c)	(((gwpci).uProperty) |= (c))
#define mResetProperty(gwpci,c)	(((gwpci).uProperty) &= ~(c))

/* usage:
 *	 mTestProperty(Var, C_PROPETRY_TO_MY_MESSAGE | C_PROPERTY_LOCAL_MESSAGE)
 * description:
 *		Test specified property type. For multiple, delineated with OR,
 *		any one type being set will result in TRUE.
 */
#define mTestProperty(gwpci,c)	((((gwpci).uProperty) & (c)) != 0) 											


/*
 * FINS command response judgement macro
 */
#define __mTestResponseBit(pHead)	((pHead)->finsPci.ICF & C_FINS_PCI_ICF_RESPONSE_DATA)

#define mIsFinsCommand(pHead)	(__mTestResponseBit(pHead) == 0x00)
#define mIsFinsResponse(pHead)	(!mIsFinsCommand(pHead))

#define mIsFromApplication(pHead) mTestProperty((pHead)->gwPci, C_PROPERTY_FROM_APPLICATION)

#define mIsBroadcast(pHead)		mTestProperty((pHead)->gwPci, C_PROPERTY_BROADCAST)

/*
=====================================================================
 B A S I C   T Y P E   D E F I N I T I O N S
=====================================================================
*/


#pragma	pack(1)	//BYTE unit alignment

//Structure to store version data
typedef struct _Version_Struc {
	BYTE		Major;		//Major version
	BYTE		Minor;		//Minor version
	BYTE		Revision;	//Revision
	BYTE		Reserved;	//Reserved
} FGW_VERSION;

//FINS transport address
typedef struct _FinsTAddress_Struc {
	BYTE		byNetAddr;	//Network address
	BYTE		byNodeAddr;	//Node address
	BYTE		byUnitAddr;	//Unit address
} FINSADDR, *PFINSADDR, *LPFINSADDR;

//Compatibility preservation
typedef FINSADDR FINSADR, *PFINSADR, *LPFINSADR;

typedef struct _RelayNetwork_Struc {
	BYTE		byNetAddr;	//Remote network address
	BYTE		byNodeAddr;	//Gateway node address
} RELAY;  

typedef struct _TypeInfo_Struc {
	USHORT		usType;
} TYPEINFO;

//Data necessary for gateway function, and
//Data necessary for handling protocols other than FINS
typedef struct _FinsGatePci {
	UINT		uProperty;	// Properties (message attributes)
	USHORT		usProtoType;//Protocol type (FINS/EVENT/IOCTL)
	BYTE		byLocalDU;	//Local send target unit
	BYTE		byLocalSU;	//Local send source unit
	union {
		RELAY		relay;	//Remote network data
		TYPEINFO	event;	//EVENT data
		TYPEINFO	ioctl;	//IOCTL data	
	} u;
}	GWPCI, *PGWPCI, *LPGWPCI;

//Header
typedef struct _FinsHeader {
	GWPCI		gwPci;		//Data necessary for gateway
	FINSPCI		finsPci;	//FINS header
}	FINSHEAD, *PFINSHEAD, *LPFINSHEAD;


#pragma pack()	//Return alignment to previous state

#endif	/*_FINSHEAD_H_*/
