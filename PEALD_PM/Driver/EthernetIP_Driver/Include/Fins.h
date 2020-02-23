/*
 *	Copyright (c) 1995,96 OMRON Corporation.
 *	All rights reserved.
 */

/*	NOTICE : THIS FILE INCLUDES JAPANESE CODE */


#ifndef _fins_h_
#define _fins_h_
/*
=====================================================================
 F I N S   B A S I C   C O N S T A N T   D E F I N I T I O N S
=====================================================================
*/
/*
----------------------------------------------------------
  Constant: ICF value
----------------------------------------------------------
*/
#define C_FINS_PCI_ICF_USE_BRIDGE		((BYTE)0x80)
#define C_FINS_PCI_ICF_RESPONSE_DATA	((BYTE)0x40)
#define C_FINS_PCI_ICF_NO_ACK_RESPONSE	((BYTE)0x01)
#define C_FINS_PCI_ICF_FIXED_MASK		((BYTE)0xc1)

#define C_FINS_PCI_ICF_DEFAULT_VALUE	(C_FINS_PCI_ICF_USE_BRIDGE)

#define C_FINS_PCI_ICF_NEED_RESPONSE	((BYTE)0x00)

/*
----------------------------------------------------------
  Constant: RSV value
----------------------------------------------------------
*/
#define	C_FINS_PCI_RSV_DEFAULT_VALUE	((BYTE)0x00)
/*
----------------------------------------------------------
  Constant: GCT value
----------------------------------------------------------
*/
#define C_FINS_PCI_GCT_DEFAULT_VALUE	((BYTE)0x07)
/*
----------------------------------------------------------
  Constant: DA (Destination Address) value
----------------------------------------------------------
*/
#define C_FINS_PCI_DA1_BROADCAST		((BYTE)0xff)
#define C_FINS_PCI_DA2_NETWORK_UNIT		((BYTE)0xfe)

/*
----------------------------------------------------------
  Constant: reserved unit number address
----------------------------------------------------------
*/
#define C_FINS_RESERVE_MIN_UNIT			((BYTE)0xB0)
#define C_FINS_RESERVE_MAX_UNIT			((BYTE)0xEF)

//reserved unit number ID
#define C_FINS_ILLEGAL_UNIT_ID			((BYTE)0xFF)


/*
=====================================================================
 F I N S   P R O T O C O L   C O N T R O L  I N F O R M A T I O N
=====================================================================
*/
/*
----------------------------------------------------------
 typedef FinsPci; FINS PCI
----------------------------------------------------------
*/
typedef BYTE	UNITID;		//Unit number
typedef BYTE	NODEID;		//Node number
typedef BYTE	NETID;		//Network number

#pragma pack(1)	//Packs into 1-byte units

/*see.FINS command reference manual*/
typedef struct _FinsPci_Struc {	
	BYTE		ICF;
	BYTE		RSV;
	BYTE		GCT;
	BYTE		DNA;
	BYTE		DA1;
	BYTE		DA2;
	BYTE		SNA;
	BYTE		SA1;
	BYTE		SA2;
	BYTE		SID;
}	FINSPCI, *PFINSPCI, *LPFINSPCI;

#pragma pack()	//Returns alignment to previous state

#endif /*_fins_h_*/
