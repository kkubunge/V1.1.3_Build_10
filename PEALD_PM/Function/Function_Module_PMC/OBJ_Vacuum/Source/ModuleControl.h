// ModuleControl.h: interface for the CModuleControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULECONTROL_H__BB72DEE8_16F5_4E81_A19A_A39E9D1C9E1A__INCLUDED_)
#define AFX_MODULECONTROL_H__BB72DEE8_16F5_4E81_A19A_A39E9D1C9E1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CModuleControl  : public CMonThread, public CDynamicIO
{
public:
	CModuleControl();
	virtual ~CModuleControl();

// Member Function
public:
	BOOL Initialize();
	void Deinitialize();

	void StartCarrierArFlow();
	void StopCarrierArFlow();
	BOOL RunCarrierArFlow();
	void FinishCarrierArFlow();
	BOOL CheckChamberCondition();

protected:
	virtual void DoMonitor();

private:
	BOOL ReadPmcConfig();
	void ParseCarrierArFlow(char *szData);

// Member Variable
public:
	int m_nCarrierArFlowSts;
	enum { CAF_IDLE, CAF_RUN, CAF_MONITORING, CAF_STOP, CAF_FAILED };

private:
	BOOL m_CAF_bEnable;
	#define CAF_MAX_MFC		3
	int m_CAF_nMfcCount;
	int m_CAF_anMfcNo[CAF_MAX_MFC];
	double m_CAF_adbMfcSp[CAF_MAX_MFC];

	#define CAF_MAX_VALVE	10
	int m_CAF_nValveCount;

	double m_CAF_dbChmPrsLowLimit;
};

#endif // !defined(AFX_MODULECONTROL_H__BB72DEE8_16F5_4E81_A19A_A39E9D1C9E1A__INCLUDED_)
