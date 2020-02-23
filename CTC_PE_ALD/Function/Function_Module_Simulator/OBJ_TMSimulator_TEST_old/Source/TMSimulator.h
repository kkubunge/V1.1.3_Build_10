// TMSimulator.h: interface for the CTMSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMSIMULATOR_H__5E61E7FA_5BD8_4391_B69B_C0F91BEC0D84__INCLUDED_)
#define AFX_TMSIMULATOR_H__5E61E7FA_5BD8_4391_B69B_C0F91BEC0D84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"
class CTMSimulator  : protected CDynamicIO, protected CMonThread
{
public:
	CTMSimulator();
	virtual ~CTMSimulator();

// Member Function
public:
	virtual BOOL Initialize();

private:
	BOOL ReadSimConfig();
	void ParseInitIOCfg(char *szRead);
	void ParsePmcSyncOption(char *szRead);
	void ParseAlarmClearCfg(char *szRead);
	void InitIOValue();

	void BMPinSimMonitor(int nThreadNo);
	void TmVacuumSimMonitor(int nThreadNo);
	void SlotValveSimMonitor(int nThreadNo);
	void IsoValveSyncSimMonitor(int nThreadNo);
	void AlarmClear(int nThreadNo);
	void CMFoupSensorMonitor(int nThreadNo);

	// Virtual Function
protected:
	virtual void DoMonitor(int nThreadNo);

// Member Variable
private:
	#define MAX_IO_LIST		100
	typedef struct _IO_LIST
	{
		char szIOName[40];
		int nIOType;
		int nIOPoint;
		char szValue[40];
	} IO_LIST;
	IO_LIST	m_IOList[MAX_IO_LIST];
	int m_IOList_nCount;

	BOOL m_bPM1IsoVlvSync;
	BOOL m_bPM2IsoVlvSync;
	BOOL m_bPM3IsoVlvSync;
	BOOL m_bPM4IsoVlvSync;
	BOOL m_bPM5IsoVlvSync;

	#define MAX_ALARM_CLEAR_LIST	40
	int m_pnAlarmClearList[MAX_ALARM_CLEAR_LIST];
	int m_nAlarmClearCount;
};

#endif // !defined(AFX_TMSIMULATOR_H__5E61E7FA_5BD8_4391_B69B_C0F91BEC0D84__INCLUDED_)
