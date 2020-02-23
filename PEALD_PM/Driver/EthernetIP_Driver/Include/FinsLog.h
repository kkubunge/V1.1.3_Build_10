/*
 *	Copyright (c) 1995,96 OMRON Corporation.
 *	All rights reserved.
 */

/*	NOTICE : THIS FILE INCLUDES JAPANESE CODE */

#ifndef	_FinsLog_h_
#define	_FinsLog_h_

//Error log format
typedef LONG ERRLOG;

//Macro to determine whether error code can be sent
#define mIsCannotSendErrorCode(err1)	(0x0100 <= (err1) && (err1) < 0x0200)

//Creates error detailed data from header data
#define __mIsCommandIcf(icf) \
		(((BYTE)(((BYTE)(icf)) & ((BYTE)0x40))) == ((BYTE)0x00))
#define __mIsResponseIcf(icf)	(!__mIsCommandIcf(icf))

#define mMakeDetailErrorCode(icf, net, node) \
		MAKEWORD((node), ((__mIsResponseIcf(icf) << 7) | ((BYTE)(net))))

/************************************************************************/
/*					Error code list for error history data storage					*/
/************************************************************************/

/*** Error codes ***/
#define ERR_SUCCESS	0x0000	/* Normal */
#define	ERR_PLCWDT	0x0001	/* PLC WDT error */
#define	ERR_CYCACCS	0x0002	/* PLC service monitor error (cyclic) */
#define	ERR_PARITY	0x0003	/* PLC shared RAM parity error */
#define	ERR_UNITNO	0x0005	/* Unit address error */
#define	ERR_CPUERR	0x0006	/* CPU error */
#define	ERR_SYSSET	0x0009	/* System settings are read error */
#define	ERR_ROUTER	0x000c	/* Routing table read error */
#define	ERR_NODE	0x0105	/* Cannot send: node address setting error */
#define	ERR_NOUNIT	0x0108	/* Cannot send: Unit does not exist */
#define	ERR_PLCERR	0x010b	/* Cannot send: PLC error */
#define	ERR_UNITERR	0x010c	/* Cannot send: Unit setting error */
#define	ERR_ROUTCAN	0x010d	/* Routing processing error */
#define	ERR_ROUTNON	0x010e	/* No routing table */
#define	ERR_ROUTTBL	0x010f	/* Routing table error */
#define	ERR_GCNTNUM	0x0110	/* Number of hops error (response thrown away) */
#define	ERR_PKTSIZE	0x0111	/* Receive packet size error */
#define	ERR_IOSET	0x0113	/* Cannot send: I/O setting error */
#define	ERR_CPUBUS	0x0114	/* Cannot send: CPU bus error */
#define	ERR_UNITXXX	0x0116	/* Cannot send: Special I/O Unit error */
#define	ERR_BUFBUSY	0x0117	/* Internal receive buffer full */
#define	ERR_ETUNIT	0x0118	/* Invalid packet thrown away (SID error) */
#define	ERR_PANIC	0x0601	/* Unit panic */

// add for FINS/TCP 
#define	ERR_PACKET	0x03c2	/* FINS/TCP packet destroy */

/*** Error codes ***/
#define	ERR_CYCPRTY	0x0100	/* Parity error: cyclic service */
#define	ERR_EVTPRTY	0x0200	/* Parity error: event service */
#define	ERR_OVERLAP	0x4000	/* Unit address duplication */
#define	ERR_ILLEGAL	0x2000	/* Invalid unit address found */
#define	ERR_HARDTST	0x1000	/*  */
#define	ERR_NOIOTBL	0x0800	/* Corresponding unit address not found in registered I/O table */
#define	ERR_CONST	0x0000	/* Default detail code */


/*--------BACKWARD COMPATIBILITY----------------------------
 * Constant: error log code
 *----------------------------------------------------------*/
#define C_ERROR_LOG_SUCCESS				ERR_SUCCESS
#define C_ERROR_LOG_FAILED				(-1)
#define C_ERROR_LOG_PEER_NO_UNIT		ERR_NOUNIT
#define C_ERROR_LOG_ROUTE_NO_DEST		ERR_ROUTCAN
#define C_ERROR_LOG_ROUTE_NOT_SET		ERR_ROUTNON
#define C_ERROR_LOG_ROUTE_ILLOGICAL		ERR_ROUTTBL
#define C_ERROR_LOG_CONSUME_GCT			ERR_GCNTNUM
#define C_ERROR_LOG_BUSY				ERR_BUFBUSY
#define C_ERROR_LOG_NO_RESPOND_ERROR	ERR_ETUNIT
#define C_ERROR_LOG_COMMAND_TOO_LONG	ERR_PKTSIZE
/*--------BACKWARD COMPATIBILITY----------------------------*/

#endif	//_FinsLog_h_
