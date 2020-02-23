// LeakCheckControl.h: interface for the CLeakCheckControl class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_LeakCheckControl_H__07666C0F_3B6E_41D9_81E4_FAFB348ECDF3__INCLUDED_)
#define AFX_LeakCheckControl_H__07666C0F_3B6E_41D9_81E4_FAFB348ECDF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BGChecker.h"

#define CHECK_TILL_PERIOD	1000
#define WAIT_TILL_PERIOD	0.1
#define IDLE_RUNTIME_HOUR	23
#define IDLE_RUNTIME_MIN	59
#define FUNCTION_COUNT		3
#define IN_TIME_SECOND		60
#define PM_COUNT			3		

typedef struct _sFUNCTABLE
{
	int   nVacFuncIndex;
	int   nOverPresFuncIndex;
	char* szModuleName;
	char* szLeakCheckCmd;
	char* szPumpCmd;
	char* szOpOverPresCmd;
	char* szClOverPresCmd;

} SFUNCTABLE;

class CLeakCheckControl :  public CBGChecker
{
	enum {ENABLE= 0 , DISABLE = 1};
	enum {DAILY = 0 , WEEKLY = 1 , MONTHLY = 2 , DAYPERIOD = 3 , LOTCOUNT = 4 , WAFERCOUNT = 5 , IDLETIME = 6};
	enum {NORMAL= 0 , ABORTED = 1};
	enum {USE   = 0 , NOTUSE=1};
	enum {CLOSE = 0 , OPEN = 1};
	enum {READY = 0 , RESERVED = 1 , RUNNING = 2};
	enum {Idle  = 0 , Running , Pausing  , Paused    , Aborting , Disable , Waiting , WaitHandOff };
	enum {enPM_LC_NONE = 0 , enPM_LC_RUN = 1};		//... m_nPM_LeakChk[]
	enum {ePRERUN = 0, ePOSTRUN = 1, ePREPOST = 2};
	//... 2016.04.03
	enum { 
		PMCOMM_UNUSE,
		PMCOMM_LOCAL,
		PMCOMM_DISCONN,
		PMCOMM_STANDBY,
		PMCOMM_MAINT,
		PMCOMM_UNINIT,
		PMCOMM_ENABLEPM,
		PMCOMM_CTCINUSE,
		PMCOMM_CTCDIS,
		PMCOMM_CTCDIS2,
		PMCOMM_CTCENAPM,
		PMCOMM_DISHW,
		PMCOMM_DISHW2
	};  
	enum { 
		enSTS_STOPPED,
		enSTS_RUNNING,
		enSTS_FAILED,
		enSTS_RESERVE,	
	};
	//... 
public:
	CLeakCheckControl();
	virtual ~CLeakCheckControl();

	BOOL Initialize();

private:
	BOOL IsReadyLeakCheck();
	BOOL PrepareForLeakCheck();
	BOOL DoLeakCheckPre();
	BOOL DoLeakCheck();
	BOOL IsCompleteLeakCheck();
	BOOL DoLeakCheckPost();
	BOOL IsVerify();
	BOOL EquipWaferCheck();

	void EXIT_BY_EXCEPTION (char* szFuncName);
	void EXIT_IF_ABORT     (char* szFuncName);
	void SetIdleRunTime    (int nHour , int nMin);
	void RestoreIdleRunTime();
	void SetLeakCheckParam ();
	void ChangeControlState(int nState);
	void DoLotAbort        ();
	void DoFunctionAbort   ();
	void DoPMIdleStop	   (char* szFuncName);
	void DoPMDryWait	   (char* szFuncName);
	void WaitLeakCheck	   (char* szFuncName);
	void DoPMLeakCheckStart	   (char* szFuncName);
	void DoPMLotPreStart	   (char* szFuncName);
	void DoPMLeakCheckEnd    (char* szFuncName);
	void SetForceAgainPM   ();
	void SetPMPrameter();		//... 2016.01.19
	void DoCleanBefLeakCheck(); //... 2016.04.03
private:
	int  GetAccLotCount    ();
	int  GetAccWaferCount  ();
	int  GetAccIdleTime    ();
	void SetAccLotCount    (int nLotCount);
	void SetAccWaferCount  (int nWaferCount);
	void SetAccIdleTime    (int nIdleTime);
	void ResetAccLotCount  ();
	void ResetAccWaferCount();
	void ResetAccIdleTime  ();

	BOOL IsAnyLotRunning   ();
	

private:
	static DWORD WINAPI Thread_LeakCheckCtrl (LPVOID lpParam);
	static DWORD WINAPI Thread_CountAccumlate(LPVOID lpParam);
	DWORD  m_dwThreadId;
	BOOL   m_bAbort;
	BOOL   m_bRunning;

	int    m_nPM1Hour;
	int    m_nPM1Min;
	int    m_nPM2Hour;
	int    m_nPM2Min;
	int    m_nPM3Hour;
	int    m_nPM3Min;

	//2011.12.19
	int    m_nOverPresVlvSts;

	//... 2016.01.06 Add PM LeakCheck & Lot Pre Flag
	int		m_nPM_LeakChk[PM_COUNT];

private:
	int    m_nAccLotCount;
	int    m_nAccWaferCount;
	int    m_nAccIdleTime;


};

//2008.04.02 added for exception throw & catch with information
class CLeakCheckCtrlAbort
{
public:
	CLeakCheckCtrlAbort() {memset(m_szAbortInfo , 0 , 128);}

public:
	char m_szAbortInfo[128];
};

#endif // !defined(AFX_LeakCheckControl_H__07666C0F_3B6E_41D9_81E4_FAFB348ECDF3__INCLUDED_)
