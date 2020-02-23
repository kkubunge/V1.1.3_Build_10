// LotFlowSimul.h: interface for the CLotFlowSimul class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LotFlowSimul_H__9E829F8E_9C2C_43D8_9385_42C5DA1EC61E__INCLUDED_)
#define AFX_LotFlowSimul_H__9E829F8E_9C2C_43D8_9385_42C5DA1EC61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SeqInterface.h"
class CLotFlowSimul  : public CSeqInterface
{
public:
	CLotFlowSimul();
	virtual ~CLotFlowSimul();

// For Thread
protected:
	virtual void DoMonitor();

// Member Function
public:
	virtual BOOL Initialize();
	virtual Module_Status Main();
	virtual void Enter_Code();

protected:
	BOOL ReadSequence();
	void StartSimulation(char *szParam);
	void StopSimulation();

	BOOL ExecFunction(char *szCmd, char *szParam);
	BOOL LoadFoup(char *szParam);
	BOOL UnloadFoup(char *szParam);

	BOOL StartLot(char *szParam);
	BOOL CancelLot(char *szParam);
	BOOL StopLot(char *szParam);
	BOOL FinishLot(char *szParam);
	BOOL WaitLotEnd(char *szParam);

	BOOL Wait_Minutes(char *szParam);
	BOOL Wait_Seconds(char *szParam);
	BOOL Write_IO(char *szParam);
	BOOL Wait_While_IO(char *szParam);
	BOOL ExchangePMMode(char *szParam);
	BOOL AbortAllLot();
	BOOL SetSimulStopCondition(char *szParam);

	// 2011.08.23
	BOOL MID_Control_On(char *szParam);
	BOOL MID_Control_Off(char *szParam);
	BOOL ClearAlarms(const char *szParam);

	void SetWafer(int nLPM, int nWaferCount = 25);

	BOOL IsSchedulerStoped();
	BOOL IsLPMRunning(int nLPMIdx);
	BOOL IsLPMIdle(int nLPMIdx);
	BOOL IsTMWaferPresent(int nTMIdx);
	BOOL IsFEMWaferPresent();
	BOOL IsLogicValid(char *szValue_1, char *szLogic, char *szValue_2);
	
	BOOL IsWaferComplete(int nLPMIdx);				// 2011.08.23

	BOOL CheckSimStopCondition();
	


// Member Variable
protected:
	char m_szSeqFileName[256];						// 2011.08.23
	#define SEQ_MAX_SIZE	100000
	char m_szSequence[SEQ_MAX_SIZE];
	char* m_szLoopStart;

	char m_szSlot[64];
	int m_nTargetLoopCount;
	BOOL m_bLoopStart;
	BOOL m_bLoopEnd;
	BOOL m_bAbortAllLot;

	BOOL m_bStopCon_SchStop;

	BOOL m_bStopCon_Alarm;
	#define STOP_ALARM_LIST_MAX		20
	int m_anStopAlarmList[STOP_ALARM_LIST_MAX];

	// 2011.08.23
	BOOL m_bStopCon_Unload_Lock;
	#define LPM_UNLOAD_LOCK_ERROR_TM	60

	typedef struct
	{
		int nType;
		char szChName[40];
		char szLogic[40];
		char szValue[40];
	} STOP_IO_LIST;
	BOOL m_bStopCon_IO;
	#define STOP_IO_LIST_MAX		10
	STOP_IO_LIST m_aStopIoList[STOP_IO_LIST_MAX];
};

#endif // !defined(AFX_LotFlowSimul_H__9E829F8E_9C2C_43D8_9385_42C5DA1EC61E__INCLUDED_)
