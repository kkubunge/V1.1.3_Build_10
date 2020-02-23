/*
 *	(c) Copyright OMRON Corporation 1998,2000
 *		All Rgihts Reserved
 */
#ifndef _FGW_FgwConfig_h_
#define _FGW_FgwConfig_h_

#pragma message(__FILE__)

//-----------------------------------------------
// Target Platform definition
//-----------------------------------------------
#ifdef _WIN32
#define cFgwPlatform_Windows
#define cFgwPlatform_WNT
#define cFgwPlatform_W98
#ifdef _WIN32_WCE
#define cFgwPlatform_WCE
#endif //ifdef _WIN32_WCE
#include <windows.h>
#endif //ifdef _WIN32
#ifdef __QNX__
#define cFgwPlatform_QNX
#endif //ifdef __QNX__
//#define cFgwPlatform_OS9

#ifdef cFgwPlatform_Windows
#include <TCHAR.h>
#else
#define	_T(x)	x
#endif //cFgwPlatform_Windows

//--------------------------------------
#include <FgwGenericType.h>

//(((((((((((((((((((((((((((((((((((((((((((((((((((((((((
//--------------------------------------
//Limits
//--------------------------------------
#if !defined(MAX_PATH)
#define MAX_PATH _POSIX_PATH_MAX
#endif //!defined(MAX_PATH)

//==================================================
//	Generic Shared Memory
//==================================================
//maximum length of Shared memory name 
#define cFGWSHAREDMEM_MEMORYNAME_LENGTH	64	//bytes

//--------------------------------------
//Termination procedure support
//
#define cFGWGENERIC_EXITPROCS	32

//==================================================
//	PrivateProfile
//==================================================
#define	cFGWPROFILE_DEFAULTDIRECTORY "/etc/FgwQnx/"
#define cFGWPROFILE_FILENAME  		"FgwQnx.ini" //_T("FgwQnx.ini")
#define cFGWPROFILE_PRIORITYENTRY	"Priority"
#define cFGWPROFILE_PRIORITYSECTION	"Priority"
#define cFGWPROFILE_SCHEDULESECTION	"Schedule"
#define cFGWPROFILE_QNXPFLAGSMASK	"QnxPflagMask"
#define cFGWPROFILE_SUBPROFILEENTRY	"SubProfile"
#define cPROFILE_NODEID			"NodeID"
#define cPROFILE_UNITID			"UnitID"
#define cPROFILE_SCHEDULE_FIFO	"FIFO"
#define cPROFILE_SCHEDULE_ROUNDROBIN	"RoundRobin"
#define cPROFILE_SCHEDULE_OTHER	"OTHER"

//==================================================
//	FgwLibMgr
//==================================================
#define cFGWLIBMGR_DATASIZE 1024
#define cFGWLIBMGR_NAME "FgwLibMgr"
#define cFGWLIBMGRPROFILE_ENTRYNAME cFGWLIBMGR_NAME
#define cFgwLIBMGR_MAXSHMEM	100
#define cFgwLIBMGR_MAXSHMEMPROC	100

//==================================================
//	FgwMsg
//==================================================
#define cFGWMSG_SEGMENTNAME		"FgwMsgSegment" //_T("FgwMsgSegment")
#define cFGWMSG_APPNAMELENGTH	16
#define cFGWMSG_MAXUNITS 		 64		/**/

#define cFGWMSGPROFILE_FILENAME  	cFGWPROFILE_FILENAME
#ifdef cFgwPlatform_Windows
#define cFGWMSGPROFILE_ROUTETABLESECTION "FinsGateway\\RouteTable" //_T("FinsGateway\\RouteTable")
#else
#define cFGWMSGPROFILE_ROUTETABLESECTION "RouteTable"
#endif //ifdef cFgwPlatform_Windows
#define cFGWMSGPROFILE_LOCALNETWORKS	"LocalNetworks" //_T("LocalNetworks")
#define cFGWMSGPROFILE_RELAYNETWORKS	"RelayNetworks" //_T("RelayNetworks")
//==================================================
//	EventMemory
//==================================================
#if defined (cFgwPlatform_QNX)
#define cEMMEMORYPROFILE_SECTIONPREFIX "EmMemory_"
#else	//cFgwPlatform_QNX
#define cEMMEMORYPROFILE_SECTIONPREFIX "FinsGateway\\EventMemory\\" //_T("FinsGateway\\EventMemory\\")
#endif	//cFgwPlatform_QNX
#define cFGWEM_APPNAMELENGTH cFGWMSG_APPNAMELENGTH
#define cFGWEM_EVENTSEGMENTNAME		"FgwEmEventSegment" //_T("FgwEmEventSegment")
#define cFGWEM_MAXEVENTPORT 		700		/**/
#define cFGWEM_MAXEVENTDATA			2048	/**/

#define cFGWEMPROFILE_NUMCHANNELS "NumChannels"
#define cFGWEMPROFILE_NUMCONDITIONS "NumConditions"
#define cFGWEMPROFILE_ACCESSMETHOD "AccessMethod"
#define cFGWEMPROFILE_NEEDMAPMEMORY "NeedMapMemory"
#define cFGWEMACTUALMEMORYNAMEFORM	"EmMemory_%s" //_T("EmMemory_%s")

//==================================================
//	CPU_UNIT
//==================================================
#ifdef cFgwPlatform_QNX
#define cCPUUNIT_PROFILESECTION "CPU_UNIT"
#else
#define cCPUUNIT_PROFILESECTION "FinsGateway\\CPU_UNIT" //_T("FinsGateway\\CPU_UNIT")
#endif //ifdef cFgwPlatform_QNX
#define cCPUUNIT_PROFILEENTRY cCPUUNIT_PROFILESECTION
#define cCPUUNIT_SCHEDULEENTRY cCPUUNIT_PROFILESECTION
#define cCPUUNIT_MEMORYNAME_BYTE 64
#define cCPUUNIT_MEMORYALIAS_MAX 20
#define cCPUUNIT_PROFILE_MEMORYALIASES "MemoryAliases" //_T("MemoryAliases")
#define cCPUUNIT_PROFILE_HARDWARETYPE "HardwareType"
#define cCPUUNIT_PROFILE_HARDWAREVERSION "HardwareVersion"
#define cCPUUNIT_PROFILE_RTCSETCOMMAND "RtcSetCommand"

//==================================================
//	Service Control Manager
//==================================================
#if defined (cFgwPlatform_QNX) || defined(cFgwPlatform_WCE)
#define SERVICE_AUTO_START 0x02
#define SERVICE_DEMAND_START 0x03
#endif //defined(cFgwPlatform_QNX)

#define cSCM_MAXSERVICES 32
#define cSCM_SERVICENAMEMAX 64	//Bytes
#define cSCM_COMMANDLINEMAX 200	//Bytes
#define cSCM_TERMINATEDATAMAX 50	//Bytes
#define cSCM_SERVICEARGMAX 100

#ifdef cFgwPlatform_Windows
#define cFGWMSGPROFILE_SCMSECTION "FinsGateway\\Services" //_T("FinsGateway\\Services")
#else
#define cFGWMSGPROFILE_SCMSECTION "Services"
#endif //ifdef cFgwPlatform_Windows
#define cFGWMSGPROFILE_SCMENTRY "Services" //_T("Services")
enum {
	eScmTerminate_Signal = 1,
	eScmTerminate_Event = 2
};

//==================================================
//----------------FgwVersion::VersionCompatible------//
//==================================================
#ifndef cRegEntryName_CompatibleVersion
#define	cRegEntryName_CompatibleVersion	("CompatibleVersion")
#endif //cRegEntryName_CompatibleVersion
//----------------FgwVersion::VersionCompatible------//

//==================================================
//	Error Stack
//==================================================
#define cFGWERRORSTACK_DEFAULTSIZE  5

//(((((((((((((((((((((((((((((((((((((((((((((((((((((((((

#endif //_FGW_FgwConfig_h_
