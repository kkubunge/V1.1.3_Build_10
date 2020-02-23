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
			APSystem ���� EasyCluster Interface�� C++�� Attach ��Ű�� Class
2010.04.26	���� Function �߰�
			- ALARM_POST_WithMsg
			- READ_DIGITAL_TRUE
			- READ_ANALOG_TRUE
2010.04.26  Function Name ó�� �߰�
			TextLog.dll �� �ڵ� Link�Ͽ� Log File���� ��� �߰�
			printf_LOG(...) - ȭ�鿡 ǥ�� �ϸ鿡 Log�� ��� �ϴ� Func �߰�
			Sim_Initialize() - Simul Code ���� �ʱ�ȭ Func �߰�
2010.09.06	_Log(...) �߰�
2011.04.14	PopupMessage(...) �߰�
2012.02.26	Main() Function ������ Log Function �߰� ��� �߰�
2012.07.18	Sim_Main ȣ�� ���� ����
--------------------------------------------------------------------*/
