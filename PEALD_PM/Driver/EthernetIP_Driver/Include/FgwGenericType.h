/*
 *	(c) Copyright OMRON Corporation 1998
 *		All Rights Reserved
 */
#ifndef _FgwGenericType_h_
#define _FgwGenericType_h_

#pragma message(__FILE__)
#include <FgwConfig.h>

#if defined(cFgwPlatform_Windows) || defined(cFgwPlatform_WCE)
#include <windows.h>
#endif

#if defined(cFgwPlatform_QNX)
#include <unistd.h>
#endif //defined(cFgwPlatform_QNX)

#include <limits.h>
#pragma pack(1)
//--------------------------------------
// C language keyword
//--------------------------------------
#if (!defined(cFgwPlatform_Windows)) && (!defined(cFgwPlatform_WCE))
#define CONST const
#endif
//--------------------------------------
// BYTE/WORD/DWORD
//--------------------------------------
#if (!defined(cFgwPlatform_Windows)) && (!defined(cFgwPlatform_WCE))
typedef unsigned long DWORD;
typedef DWORD* PDWORD;
typedef unsigned short WORD;
typedef WORD* PWORD;
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef int INT;
typedef INT* PINT;
#else
typedef int		pid_t;
#endif
//--------------------------------------
// HANDLE/VOID
//--------------------------------------
#if (!defined(cFgwPlatform_Windows)) && (!defined(cFgwPlatform_WCE))
typedef void* HANDLE;
typedef void VOID;
typedef VOID* LPVOID;
typedef LPVOID PVOID;
#endif
//--------------------------------------
// WINAPI
//--------------------------------------
#if (!defined(cFgwPlatform_Windows))
#define WINAPI
#endif
//--------------------------------------
//BOOL/BOOLEAN
//--------------------------------------
#if defined(cFgwPlatform_QNX)
#define TRUE 1
#define FALSE 0
typedef BYTE BOOL;
typedef BYTE BOOLEAN;
typedef BOOLEAN* PBOOLEAN;
#endif //defined(cFgwPlatform_QNX)
//--------------------------------------
//STRING MANIPULATION
//
#if defined(cFgwPlatform_QNX)
typedef char* LPSTR;
typedef LPSTR LPCSTR;
typedef LPSTR LPTSTR;
typedef LPTSTR LPCTSTR;
typedef char* PCHAR;
#endif //defined(cFgwPlatform_QNX)
//--------------------------------------
//METHOD PUBLISHING
//--------------------------------------
#if defined(cFgwPlatform_QNX)
#define EXPORT
#endif //defined(cFgwPlatform_QNX)
//--------------------------------------
//Termination procedure support
//--------------------------------------
#if defined(cFgwPlatform_QNX)
typedef void (*tExitProc)(void);
typedef void (*tFgwLibMgrOnInit)(void);
typedef void (*tFgwLibMgrOnQuit)(void);
typedef void (*tFgwLibMgrOnCommand)(pid_t, void*);
typedef void (*tFgwLibMgrOnProcDeath)(pid_t);
#endif //defined(cFgwPlatform_QNX)

//--------------------------------------
//WORD manipulation
//--------------------------------------
#if defined(cFgwPlatform_QNX)
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif //defined(cFgwPlatform_QNX)
#define mNetWordToIntelWord(wordValue) \
	( (LOBYTE(wordValue)<<8) | (HIBYTE(wordValue)))

//--------------------------------------
// BCD
//--------------------------------------
#define LOBCD(b)			((BYTE)((b) & 0x0F))
#define HIBCD(b)			((BYTE)(((b) >> 4) & 0x0F))
#define mIsValidBcd(byte)	(/*((byte) >= 0x00) &&*/ (((BYTE)(byte)) <= 0x09))
#define mIsValidBcdByte(byte)	\
			( mIsValidBcd(HIBCD(byte)) && mIsValidBcd(LOBCD(byte)) )
#define mBinToBcd(hex)		((BYTE)(((_UNDER_99(hex) / 10) << 4) | ((hex) % 10)))
#define mBcdToBin(byte)	((BYTE)( (HIBCD((byte))*10) + (LOBCD((byte))) ))

#ifndef toBCD
#define toBCD(hex)	((BYTE)(((_UNDER_99(hex) / 10) << 4) | ((hex) % 10)))
#endif
#ifndef fromBCD
#define fromBCD(hex)		((((hex) >> 4) * 0x0a) + ((hex) & 0x0f))
#endif 

//--------------------------------------
// Endian
//--------------------------------------
enum
{ 
	eFgwEndian_BigEndian, 
	eFgwEndian_LittleEndian, 
	eFgwEndian_Unknown 
};
#define SWAPNETSHORT(x)		(((x)>>8) & 0x00ff) | (((x)<<8) & 0xff00)
#define SWAPHOSTSHORT(x)	(((x)>>8) & 0x00ff) | (((x)<<8) & 0xff00)
#define SWAPNETLONG(x)		(((x)>>24) & 0x000000ff) | (((x)>>8) & 0x0000ff00) | (((x)<<8) & 0x00ff0000) | (((x)<<24) & 0xff000000)
#define SWAPHOSTLONG(x)		(((x)>>24) & 0x000000ff) | (((x)>>8) & 0x0000ff00) | (((x)<<8) & 0x00ff0000) | (((x)<<24) & 0xff000000)


//--------------------------------------
typedef int tIndex;

//--------------------------------------
#ifndef INFINITE
#define INFINITE	((DWORD)0xFFFFFFFF)
#endif /*INFINITE*/

#pragma pack()
#endif //_FgwGenericType_h_
