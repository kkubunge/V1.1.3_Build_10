// BGChecker.h: interface for the CBGChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BGCHECKER_H__04D0DFBC_0C5E_435E_84C1_6B1297007E5F__INCLUDED_)
#define AFX_BGCHECKER_H__04D0DFBC_0C5E_435E_84C1_6B1297007E5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBGChecker  
{
	enum {Idle  = 0 , Running , Pausing  , Paused    , Aborting , Disable , Waiting , WaitHandOff };
public:
	CBGChecker();
	virtual ~CBGChecker();

protected:
	void   CheckAndControlLot();
	bool   RunBGChecker (BOOL bCM1Run , BOOL bCM2Run , BOOL bCM3Run);
	bool   KillBGChecker();
	static DWORD WINAPI BGCheckerThread (LPVOID pVoid);

protected:
	HANDLE m_hBGCheckerHandle;
	DWORD  m_dwBGChecker;
	BOOL   m_bCM1Run , m_bCM2Run , m_bCM3Run;

};

#endif // !defined(AFX_BGCHECKER_H__04D0DFBC_0C5E_435E_84C1_6B1297007E5F__INCLUDED_)
