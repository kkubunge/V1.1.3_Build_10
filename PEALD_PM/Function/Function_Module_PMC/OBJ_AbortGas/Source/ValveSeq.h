// ValveSeq.h: interface for the CValveSeq class.
// Valve Sequence Class
// Created : 2009.12.21
// Last Update : 2010.08.09
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALVESEQ_H__82CAF0E2_ADD4_499A_99F7_762F873C8628__INCLUDED_)
#define AFX_VALVESEQ_H__82CAF0E2_ADD4_499A_99F7_762F873C8628__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_MFC_PER_LINE	3
#define MAX_STEP			200
#define MAX_ZM				2
#define MAX_VALVE_STR		256

enum { VSEQ_IDLE, VSEQ_RUN, VSEQ_FAILED };

#include "DynamicIO.h"
#include "FuncThread.h"

class CValveSeq  : protected CDynamicIO, protected CFuncThread
{
public:
	CValveSeq();
	virtual ~CValveSeq();

public:
	BOOL Initialize(const char *szSeqName = NULL, const char *szFilePath = NULL);
private:
	BOOL ReadConfig();
	void ParseValveSeq(char *szData);

public:
	BOOL StartValveSeq(int nStartStep = 1, int nEndStep = MAX_STEP, BOOL bWait = FALSE);
	BOOL StartValveSeq(const char *szDescript, BOOL bWait = FALSE);
	void StopValveSeq(int nEndStep = 0);
	BOOL ExecValveStep(const char *szDescript, BOOL bFuncFree = FALSE);			// 2010.04.17

	BOOL ExecValveStep(int nStep);
	BOOL ExecValveStep_Free(int nStep);
	void GoNextStep();
	BOOL IsValveSeqRunning();
	int  FindStep(const char *szDescript);
	int  FindStepIdx(const char *szDescript);
	BOOL IsMFCFlow();		//2018.11.23 by kimhc

private:
	void WriteIO(const char *szIOString);

protected:
	virtual void DoFunction(WPARAM wParam, LPARAM lParam);

public:
	int		m_nNowStep;
	char	m_szFilePath[80];
	char	m_szSeqName[40];

	int		m_nStepCount;
	double	m_adbStepTime[MAX_STEP];
	int		m_anMFCNo[MAX_STEP][MAX_MFC_PER_LINE];
	double	m_adbMFCSet[MAX_STEP][MAX_MFC_PER_LINE];
	double	m_adbTVSet[MAX_STEP];
	double	m_adbZMSet[MAX_STEP][MAX_ZM];
	char	m_aszValveStr[MAX_STEP][MAX_VALVE_STR];
	char	m_aszCtrlCmd[MAX_STEP][40];
	char	m_aszDescript[MAX_STEP][40];
	double	m_alnStepEndTime[MAX_STEP];
	int		m_nValveSeqSts;

private:
	class	CTimer	*m_pSeqTimer;
	int		m_nLastStepForBreak;
	BOOL	m_bNextStepFlag;
};

#endif // !defined(AFX_VALVESEQ_H__82CAF0E2_ADD4_499A_99F7_762F873C8628__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2009.12.21	Created
			TextParser.h, DynamicIO.h, Timer.h, FuncThread.h class �ʿ�
			CimSqConIO.lib �ʿ�
			VALVE_CONTROL, MFC_CONTROL, ZMOTION_CONTROL, ���� Func��  Link�Ͽ� �����

2009.12.22	MFCSet ���� -1�� ��� Setting ���� �ʵ��� ��� �߰�(Setting ����)

2009.12.31	StartValveSeq �� Wait Option �߰�
			bWait ���� TRUE�� ��� Sequence �Ϸ�ɶ����� ��ٸ���.

2010.01.26	MFC_ALL_CLOSE �� CLOSE_ALL_MFC ��� �Ѵ� ��� �����ϵ��� ����
			ValveSeq Thread Func ȣ�� ���н� Waiting ���� �ʵ��� ����
			Descript �� '-------->STEP_END' ���� �ش� Step ���� �� Seq ���� ��� �߰�
2010.01.28	Descript �� '-------->STEP_END' ��� ���� ����(���� ����)
2010.01.29	Descript �� '-------->STEP_END' ��� ���� ����(�系 ����)
2010.04.01	Sequence ȣ�� Fail �ÿ� Name ǥ�� ���� ����
2010.05.17	ExecValveStep(.. , bFuncFree) ��� �߰�
			=> Abort ����� �����ϰ� ������ �����ϴ� ���
2010.06.15	ExecValveStep(...) return �� ���� ����
2010.08.09	Step Number�� �ǳʶ۶� ���� �ȵǴ� ���� ����
--------------------------------------------------------------------*/
