/*****************************************************************************/
/* (C)	Copyright OMRON  Corporation 2008	 								 */
/*		All Rights Reserved	 												 */
/*****************************************************************************/

/*==============================================================================*/
/* CIPCore�|�[�g�Ǘ��V�X�e��API�Ή�	 											*/
/*==============================================================================*/
#ifndef	__CIPCORE_PORT_API_PORT_H__
#define	__CIPCORE_PORT_API_PORT_H__


#ifdef	_EXPORT_CIP_API_
#define _EXPORT_CIP	__declspec(dllexport)
#define _EXTERN_CIP
#else
#include <CIPCoreAPI.h>
#define _EXPORT_CIP	__declspec(dllimport)
#define _EXTERN_CIP	extern
#if defined(_WIN32)
#if defined(_WIN32_WCE)
#pragma comment(lib,"CIPCorePort.lib")
#else
//#pragma comment(lib, "CIPCorePort.lib")
#endif //defined(_WIN32_WCE)
#endif
#endif 

#ifdef __cplusplus
extern "C" {
#endif

#include <CIPPortError.h>


/*****************************************************************************/
/* Defines																	 */
/*****************************************************************************/
#define	CIPPORT_PORTID_MAX		14
#define CIPPORT_EIP_NAME		"Ethernet I/F"
#define CIPPORT_CJ2_NAME		"CJ2 USB/Serial Port"
#define	CIPPORT_NE1S_NAME		"NE1S Serial Port"
#define CIPPORT_TIMEOUT_DEFAULT	"5000"

#define MAX_STRING_BUFFER_SIZE	256
#define MAX_SPECIFIC_DATA_SIZE	256
#define MAX_SPECIFIC_SUB_DATA_SIZE	1024

typedef enum {
	CIPInterface_NotSupported = -1,
	CIPInterface_EIP = 0,
	CIPInterface_CJ2 = 1,
	CIPInterface_NE1S = 2,
} CIPInterfaceTypes;

typedef enum {
	CIPSerialPort_USB = 0,
	CIPSerialPort_SERIAL = 1,
} CIPSerialPortTypes;

typedef	struct {
	WORD	portID;				/* �|�[�gID										*/
	BOOL	startupOpen;		/* TRUE�F�N�����I�[�v���AFALSE�F�N�����N���[�Y	*/
	CIPInterfaceTypes interfaceTypeCode;	/* �C���^�[�t�F�[�X���				*/
	TCHAR	interfaceType[MAX_STRING_BUFFER_SIZE];	/* �C���^�[�t�F�C�X���		*/
								/* ("Ethernet I/F"�A"CJ2 USB/Serial Port"�j		*/
								/* �C���^�[�t�F�C�X��ʂ̃o�b�t�@�T�C�Y			*/
	DWORD	interfaceTypeBufferSize;
	DWORD	timeOut;			/* CommLink�^�C���A�E�g����						*/
	DWORD	specificDataSize;	/* �C���^�[�t�F�[�X�ŗL���T�C�Y				*/
	DWORD	specificData[MAX_SPECIFIC_DATA_SIZE];/* �C���^�[�t�F�[�X�ŗL�|�[�g���	*/
	DWORD	specificSubDataSize;	/* �C���^�[�t�F�[�X�ŗLsub���T�C�Y		*/
	BYTE	specificSubData[MAX_SPECIFIC_SUB_DATA_SIZE];	/* �C���^�[�t�F�[�X�ŗLSub�|�[�g���	*/
} CIPPortEnumInfo;

typedef enum {
	CIPPortStatus_Closed = 0,	// Port closed
	CIPPortStatus_Open  = 1		// Port open
} CIPPortStatus;

/*****************************************************************************/
/* CIPCore Port API														 */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* CIPPort_open																 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP BOOL WINAPI CIPPort_open(
	CIPAppHandle	appHandleID,
	WORD			portID		
	);

/*---------------------------------------------------------------------------*/
/* CIPPort_close															 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP BOOL WINAPI CIPPort_close(
	CIPAppHandle	appHandleID,
	WORD			portID		
	);

/*---------------------------------------------------------------------------*/
/* CIPPort_getStatus														 */
/*---------------------------------------------------------------------------*/


_EXPORT_CIP int WINAPI CIPPort_getStatus(
	CIPAppHandle	appHandleID,
	WORD			portID		
	);

/*---------------------------------------------------------------------------*/
/* CIPPort_enumPortInfo			 											 */
/*---------------------------------------------------------------------------*/
_EXPORT_CIP	int WINAPI CIPPort_enumPortInfo (
	DWORD		portInfoNum, 
	CIPPortEnumInfo	*enumPortInfo
);

#ifdef __cplusplus
};
#endif	/* __cplusplus */

#endif	/* __CIPCORE_PORT_API_PORT_H__ */
