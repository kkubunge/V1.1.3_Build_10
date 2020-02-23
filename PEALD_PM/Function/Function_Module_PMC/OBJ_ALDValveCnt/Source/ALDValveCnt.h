// LotFlowControl.h: interface for the CLotFlowControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOTFLOWCONTROL_H__07DD716E_886F_4C92_A97C_D1A9DFF40547__INCLUDED_)
#define AFX_LOTFLOWCONTROL_H__07DD716E_886F_4C92_A97C_D1A9DFF40547__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SeqInterface.h"
#include "MonThread.h"

//... 2015.04.28 Fixed ALD Valve 13 EA (Top 10 EA, Canister 3 EA)
#define MAX_VALVE 13

class CALDVlvMon  : public CSeqInterface, protected CMonThread
{
public:
	CALDVlvMon();
	virtual ~CALDVlvMon();

// Member Function
public:
	virtual Module_Status Main();
	virtual void Enter_Code();
	virtual BOOL Initialize();
	void ParseALDValveConfig(char *szData);
	void ALARM_POST_WithMsg(int nAlarmID, int nValve);
	void ResetVlvCount(int nVlvNum);
	BOOL ReadConfiguration();
	void SUMVlvCount();
	int Change_IndexToVlvNum(int nIndex);

protected:

// Thread Virtual Function
protected:
	virtual void DoMonitor();

// Member Variable
protected:
	int m_anPM_Comm[2];				
	int m_nALDVlvUseCnt;            // 2014.11.28
	CRITICAL_SECTION m_csLock;
	int nIOStatus;
	long int m_nOpenCnt[MAX_VALVE],m_nOpenCnt_old[MAX_VALVE],m_nOpenDif[MAX_VALVE];
	long int m_nCloseCnt[MAX_VALVE],m_nCloseCnt_old[MAX_VALVE],m_nCloseDif[MAX_VALVE];
	long int m_nALDValveOpenCnt[MAX_VALVE];
	long int m_nALDValveCloseCnt[MAX_VALVE];

};

#endif // !defined(AFX_LOTFLOWCONTROL_H__07DD716E_886F_4C92_A97C_D1A9DFF40547__INCLUDED_)
