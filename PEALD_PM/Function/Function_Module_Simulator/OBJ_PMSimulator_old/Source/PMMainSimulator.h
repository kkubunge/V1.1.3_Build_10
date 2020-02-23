// PMMainSimulator.h: interface for the CPMMainSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMMAINSIMULATOR_H__C8627704_73BB_4569_9686_58BB36433496__INCLUDED_)
#define AFX_PMMAINSIMULATOR_H__C8627704_73BB_4569_9686_58BB36433496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CPMMainSimulator  : protected CMonThread
{
public:
	CPMMainSimulator(int nStgHtrType = 0);
	virtual ~CPMMainSimulator();

	// Member Function
public:
	virtual BOOL Initialize();
protected:
	virtual void DoMonitor();
private:
	void InitIOValue();
	BOOL ReadSimCfg();
	void ParseHeaterCfg(char *szRead);
	void ParseMfcCfg(char *szRead);
	void ParsePrsCtrlCfg(char *szRead);
	void ParseInitIOCfg(char *szRead);
	void ParseStageHeaterCfg(char *szRead);
	void ParseAlarmClearCfg(char *szRead);
	void ParseIOSyncList(char *szRead);

	// Member Variable
public:
	int m_nStgHtrType;		// 1:OmronE5ER, 2:Watlow988
private:
	class CPMChmPrsSimulator *m_pPMChmPrsSimulator;
	class CPMHeaterSimulator *m_pPMHeaterSimulator;
	class CPMMfcSimulator *m_pPMMfcSimulator;
	class CPMDryPumpSimulator *m_pPMDryPumpSimulator;
	class CPMStgHtrSimulator *m_pPMStgHtrSimulator;
	class CIOSyncMonitor *m_pIOSyncMonitor;				// 2008.08.25

	typedef struct _IO_INIT_LIST
	{
		char szIOName[40];
		int nIOType;
		int nIOPoint;
		char szValue[40];
	} IO_INIT_LIST;
	#define MAX_IO_LIST				200
	IO_INIT_LIST m_pIOList[MAX_IO_LIST];
	int m_IOList_nCount;

	#define MAX_ALARM_CLEAR_LIST	40
	int m_pnAlarmClearList[MAX_ALARM_CLEAR_LIST];
	int m_nAlarmClearCount;
};

#endif // !defined(AFX_PMMAINSIMULATOR_H__C8627704_73BB_4569_9686_58BB36433496__INCLUDED_)

