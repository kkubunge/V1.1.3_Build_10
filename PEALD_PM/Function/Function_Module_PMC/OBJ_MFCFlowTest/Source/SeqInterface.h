// SeqInterface.h: interface for the CSeqInterface class.
// Create : 2010.02.10
// Last Update : 2012.07.18
// Created by Changsu,Kim
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SeqInterface_H__5AA5DD2D_2027_4BB2_AC04_93D5467A2DB1__INCLUDED_)
#define AFX_SeqInterface_H__5AA5DD2D_2027_4BB2_AC04_93D5467A2DB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
class CSeqInterface  : public CMonThread
{
public:
	CSeqInterface();
	CSeqInterface(const char *szFuncName);
	virtual ~CSeqInterface();


// Member Function
public:
	BOOL Initialize(const char *szFuncName);

	virtual void Enter_Code();
	virtual BOOL Initialize();
	virtual BOOL Deinitialize();
	virtual Module_Status Main();

	// for Simulation
	virtual BOOL Sim_Initialize();			// 2010.04.28
	virtual Module_Status Sim_Main();

protected:
	double READ_ANALOG_TRUE(int nIoPoint, BOOL *pbResult);
	int READ_DIGITAL_TRUE(int nIoPoint, BOOL *pbResult);
	void ALARM_POST_WithMsg(int nAlarmID, char *szMsg);

	void printf_LOG(const char *pFormat, ...);					// 2010.04.28
	void printf_LOG_Hex(char pData[], int nSize);
	void _LOG(const char *pFormat, ...);
	void PopupMessage(const char *szTitle, const char *szMsg);	// 2011.04.14

// Member Variable
public:
	char m_szFuncName[40];
	BOOL m_bSimulMode;

protected:
};

#endif // !defined(AFX_SeqInterface_H__5AA5DD2D_2027_4BB2_AC04_93D5467A2DB1__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2010.02.10	Created
			APSystem 사의 EasyCluster Interface를 C++로 Attach 시키는 Class
2010.04.26	공용 Function 추가
			- ALARM_POST_WithMsg
			- READ_DIGITAL_TRUE
			- READ_ANALOG_TRUE
2010.04.26  Function Name 처리 추가
			TextLog.dll 과 자동 Link하여 Log File생성 기능 추가
			printf_LOG(...) - 화면에 표시 하면에 Log도 기록 하는 Func 추가
			Sim_Initialize() - Simul Code 전용 초기화 Func 추가
2010.09.06	_Log(...) 추가
2011.04.14	PopupMessage(...) 추가
2012.02.26	Main() Function 무조건 Log Function 추가 기능 추가
2012.07.18	Sim_Main 호출 시점 변경
--------------------------------------------------------------------*/
