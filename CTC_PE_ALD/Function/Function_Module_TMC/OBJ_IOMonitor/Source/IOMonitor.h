// IOMonitor.h: interface for the CIOMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOMONITOR_H__9D6D750B_AB18_43AF_8E3B_0D044787FBAC__INCLUDED_)
#define AFX_IOMONITOR_H__9D6D750B_AB18_43AF_8E3B_0D044787FBAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
#include "MonThread.h"

class CIOMonitor  : protected CDynamicIO, protected CMonThread
{
public:
	CIOMonitor();
	virtual ~CIOMonitor();

// Member Function
public:
	bool Initialize(char *szConfigPath,  char *szLogFilePath = NULL);
	void Deinitialize();

private:
	bool ReadPmcConfig(char *szConfigPath);
	void ParseIOMonitorCfg(char *szData);

	void LogIOName();
	void LogIOValue();

	// Virtual Function
protected:
	virtual void DoMonitor();

// Member Variable
private:
	#define MAX_MON_IO_LIST		30
	IO_LIST	m_pIOList[MAX_MON_IO_LIST+1];
	int m_nIOCount;
	int m_nMonitoringPeriold;
};

#endif // !defined(AFX_IOMONITOR_H__9D6D750B_AB18_43AF_8E3B_0D044787FBAC__INCLUDED_)
