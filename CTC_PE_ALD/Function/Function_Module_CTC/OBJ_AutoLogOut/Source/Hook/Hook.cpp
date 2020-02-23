#include <stdio.h>
#include <windows.h>
#include <time.h>

#define		MOUSE_POSITION_TOLERANCE	10

#pragma data_seg("SHARDATA")  
    HHOOK	_hKHook = NULL;
	HHOOK	_hMHook = NULL;		
	HANDLE _hInst;
	time_t _lastActTime=0;
	int		count = 0;
	int		_mX = 0;
	int		_mY = 0;
#pragma data_seg()

#ifndef _DEBUG
// #define _DEBUG
#endif

extern "C" __declspec(dllexport) BOOL StartHook();
extern "C" __declspec(dllexport) BOOL IsUserAction(long out_min);

void StopHook();

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HANDLE hInst, DWORD fdwReason, LPVOID lpRes)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		_hInst = hInst;
		_hKHook = _hMHook = NULL;		
#ifdef _DEBUG
		printf("--->INST[%d][%d]\n", _hInst, ++count);
#endif
		break;

	case DLL_PROCESS_DETACH:
		StopHook();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport) BOOL StartHook()
{
	_hKHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc,	(HINSTANCE)_hInst, 0);
	_hMHook = SetWindowsHookEx(WH_MOUSE,	(HOOKPROC)MouseProc,	(HINSTANCE)_hInst, 0);
	
	if(_hKHook && _hMHook)
	{
#ifdef _DEBUG
		printf("--->StartHook() Ok...\n");
#endif
		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		printf("--->StartHook() Fail...\n");
#endif
		StopHook();
		return FALSE;
	}
}


void StopHook()
{
#ifdef _DEBUG
	printf("--->StopHook()\n");
#endif
	if(_hKHook) UnhookWindowsHookEx(_hKHook);
	if(_hMHook) UnhookWindowsHookEx(_hMHook);
}

#ifdef _DEBUG
char szFilterLine[512];
char szType[512];
HDC	       hDC;
#endif

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{     
	if(nCode >=0)
	{
		time(&_lastActTime);

#ifdef _DEBUG
		if ( nCode == HC_NOREMOVE )		  strcpy(szType, "NOT Removed from Queue");
		else							  strcpy(szType, "REMOVED from Queue                                             ");

		wsprintf((LPSTR)szFilterLine,     "----->KEYBOARD\tKey:%d\t%s[%d]\n",wParam,(LPSTR)szType, _lastActTime);	  
      
		hDC = GetDC(NULL);
		TabbedTextOut(hDC, 1, 50, (LPSTR)szFilterLine,  strlen(szFilterLine), 0, NULL, 1);
		ReleaseDC(NULL, hDC);
#endif

	}
	return CallNextHookEx( _hKHook, nCode, wParam, lParam); 
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{     
	MOUSEHOOKSTRUCT *MouseHookParam = (MOUSEHOOKSTRUCT*)lParam;
	
	if(nCode >= 0)
	{
		if(	abs(_mX-MouseHookParam->pt.x) > MOUSE_POSITION_TOLERANCE ||
			abs(_mY-MouseHookParam->pt.y) > MOUSE_POSITION_TOLERANCE	)
		{
			time(&_lastActTime);			
		}
		_mX = MouseHookParam->pt.x;
		_mY = MouseHookParam->pt.y;

#ifdef _DEBUG
		if ( nCode == HC_NOREMOVE )		 strcpy(szType, "NOT Removed from Queue");
		else							 strcpy(szType, "REMOVED from Queue                                             ");
		
		wsprintf((LPSTR)szFilterLine,    "----->MOUSE\t\t(%d,%d)-(%d,%d)--%s\t--%d",_mX, _mY, MouseHookParam->pt.x,MouseHookParam->pt.y,(LPSTR)szType, _lastActTime);

		hDC = GetDC(NULL);
		TabbedTextOut(hDC, 1, 100,   (LPSTR)szFilterLine, strlen(szFilterLine),   0, NULL, 1 );
		ReleaseDC(NULL, hDC);
#endif

	}
	
    return CallNextHookEx( _hMHook, nCode, wParam, lParam); 
}

extern "C" __declspec(dllexport) BOOL IsUserAction(long out_min)
{
	time_t nowTime;

	time(&nowTime);

#ifdef _DEBUG
	wsprintf((LPSTR)szFilterLine, "----->lastActionTime[%d] , nowTime[%d], gapSec[%d], gapMin[%d]", _lastActTime, nowTime, nowTime-_lastActTime, (nowTime - _lastActTime) / 60);

	hDC = GetDC(NULL);
	TabbedTextOut(hDC, 1, 150,   (LPSTR)szFilterLine, strlen(szFilterLine),   0, NULL, 1 );
	ReleaseDC(NULL, hDC);
#endif

//	return (nowTime - _lastActTime);
	if(((nowTime - _lastActTime) / 60) >= out_min)	return FALSE; // Unit : Min
//	if(((nowTime - _lastActTime) ) >= out_min)	return FALSE;	  // Unit : Sec
	else 											return TRUE;
}
