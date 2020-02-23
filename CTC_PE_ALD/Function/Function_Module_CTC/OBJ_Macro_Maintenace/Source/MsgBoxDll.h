// MsgBoxDll.h: interface for the 'MsgBox.dll'
// Create : 2004.04.06
// Last Update : 2007.07.25
// Writter : Chang-su,Kim.
//////////////////////////////////////////////////////////////////////

#ifndef _MSGBOXDLL_H_
#define _MSGBOXDLL_H_
// #include <stdio.h>

//--------------------------------------------------------------------
// Initialize Function

#define _gINIT_MSGBOX(SZ_MSGTITLE)	{ _gLoadMsgBoxDll(); strcpy(g_szMsgTitle, SZ_MSGTITLE); }
// #define _gINIT_MSGBOX()		_gLoadMsgBoxDll()
#define _gDEINIT_MSGBOX()	_gFreeMsgBoxDll()

//--------------------------------------------------------------------
// Msg Low Function Definition
static void _gOutMsgBox(const char *pAppName, const char *pMsg, BOOL bWait);
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Message Box Default Title and Wait Option
static char g_szMsgTitle[80] = "Application Name";
static BOOL g_bMsgWait = FALSE;

//////////////////////////////////////////////////////////////////////
// OutMsg Type 1
#define _OUTMSG_V_LIST(P_FORMAT)						\
{														\
	char _szMsg[256];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szMsg, P_FORMAT, args);					\
	va_end(args);										\
	_gOutMsgBox(g_szMsgTitle, _szMsg, g_bMsgWait);		\
}
//--------------------------------------------------------------------
static void _OUTMSG(const char *pFormat, ...) { _OUTMSG_V_LIST(pFormat) }
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// OutMsg Type 2
/*
#define _OUTMSG_V_LIST(SZ_TITLE, P_FORMAT)				\
{														\
	char _szMsg[256];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szMsg, P_FORMAT, args);					\
	va_end(args);										\
	_gOutMsgBox(SZ_TITLE, _szMsg, g_bMsgWait);			\
}
//--------------------------------------------------------------------
static void _OUTMSG(const char *szTitle, const char *pFormat, ...) { _OUTMSG_V_LIST(szTitle, pFormat) }
//--------------------------------------------------------------------
*/

//////////////////////////////////////////////////////////////////////
// OutMsg Type 3
/*
#define _OUTMSG_V_LIST(SZ_TITLE, B_WAIT, P_FORMAT)		\
{														\
	char _szMsg[256];									\
	va_list args;										\
	va_start(args, P_FORMAT);							\
	vsprintf(_szMsg, P_FORMAT, args);					\
	va_end(args);										\
	_gOutMsgBox(SZ_TITLE, _szMsg, B_WAIT);				\
}
//--------------------------------------------------------------------
static void _OUTMSG(const char *szTitle, BOOL bWait, const char *pFormat, ...) { _OUTMSG_V_LIST(szTitle, bWait, pFormat) }
//--------------------------------------------------------------------
*/

//////////////////////////////////////////////////////////////////////
// #include <window.h>

typedef DWORD (*LPMSGBOXDLLFUNC)(const char *pAppName, const char *pMsg, BOOL bWait);
static HMODULE g_hMsgDllModule = NULL;
static LPMSGBOXDLLFUNC _gbDisplayMsgBox = NULL;
static BOOL g_bTryLoadMsgDll = FALSE;

static BOOL _gLoadMsgBoxDll()
{
	BOOL bRet = FALSE;
	do {
		g_bTryLoadMsgDll = TRUE;
		if(NULL == g_hMsgDllModule)
		{
			g_hMsgDllModule = LoadLibrary("MsgBox.dll");
			if(NULL == g_hMsgDllModule) break;
			_gbDisplayMsgBox = (LPMSGBOXDLLFUNC) GetProcAddress(g_hMsgDllModule, "_gbDisplayMsgBox");
			if(NULL == _gbDisplayMsgBox)
			{
				FreeLibrary(g_hMsgDllModule);
				g_hMsgDllModule = NULL;
				break;
			}
		}
		bRet = TRUE;
	} while(0);

	// if(! bRet) MessageBox(NULL, "'MsgBox.dll' Loading failed !", "MsgBoxDll.h", MB_OK| MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_TOPMOST);
	return bRet;
}

static void _gOutMsgBox(const char *pAppName, const char *pMsg, BOOL bWait)
{
	if(FALSE == g_bTryLoadMsgDll) _gLoadMsgBoxDll();
	if(g_hMsgDllModule != NULL && _gbDisplayMsgBox != NULL) _gbDisplayMsgBox(pAppName, pMsg, bWait);
}

static void _gFreeMsgBoxDll()
{
	if(g_hMsgDllModule != NULL)
	{
		FreeLibrary(g_hMsgDllModule);
		g_hMsgDllModule = NULL;
	}
}

#endif

//////////////////////////////////////////////////////////////////////
/* Update History
2004.04.06	Created.
2005.09.08	History 기록하기 전 마지막 수정 날짜.
2006.07.13	History 기록 시작.
			_OUTMSG(...) Function에 Variable List를 쓸수 있도록 수정.
2006.07.25	OutMsg Type 2 에 대하여 Variable List를 쓸수 있도록 수정.
			OutMsg Type 3 추가
			Initialize Func에 Title을 지정할 수 있는 정의 추가
--------------------------------------------------------------------*/