// TmcMonitor.h: interface for the CTmcMonitor class.
// For GX7000 Backbone(300 mm)
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMCMONITOR_H__636DEE6B_D87E_4037_BC33_EE434E8C0F99__INCLUDED_)
#define AFX_TMCMONITOR_H__636DEE6B_D87E_4037_BC33_EE434E8C0F99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CTmcMonitor  : public CDynamicIO, public CMonThread
{
public:
	CTmcMonitor();
	virtual ~CTmcMonitor();

	// Member Function
public:
	virtual BOOL Initialize();
protected:
	virtual void DoMonitor(int nThreadNo);
	
	//--------------------------------------------------------------------
	// 2005.05.24 by cskim, CM Status Monitoring
	char m_szLotID[256];

	//--------------------------------------------------------------------
	// 2005.07.29
	BOOL m_bPMIntModuleDis;
	BOOL m_bPLCIntModuleDis;

	//--------------------------------------------------------------------
	void PM_COMM_STATUS_MONITORING(int nThreadNo, int nPM);

	void PM1_COMM_STATUS_MONITORING(int nThreadNo);
	void PM2_COMM_STATUS_MONITORING(int nThreadNo);
	void PM3_COMM_STATUS_MONITORING(int nThreadNo);

	//--------------------------------------------------------------------
	// 2005.01.27 by cskim
	BOOL m_bPM1WaferSync;
	BOOL m_bPM2WaferSync;
	BOOL m_bPM3WaferSync;
	void WaferSyncThread(int nThreadNo);

	//--------------------------------------------------------------------
	// 2005.05.24 by cskim, CM Status Monitoring
	void CM_MonitorThread(int nThreadNo);

	//--------------------------------------------------------------------
	// 2005.07.28 PM SW Interlock Monitoring
	void PM_SWIntMonThread(int nThreadNo);

	//--------------------------------------------------------------------
	// 2005.07.29 PM PLC Interlock Monitoring
	void PM_PLCIntMonThread(int nThreadNo);

	//--------------------------------------------------------------------
	// 2005.10.28 PM Step Sync
	int m_nPMSyncWaitTime;
	void PM_StepSyncMonThread(int nThreadNo);

	//--------------------------------------------------------------------
	// 2004.12.24 by cskim
	void ParsePmcSyncOption(char *szData);
	void ParseMonitoringOption(char *szData);
	BOOL ReadTmcConfig();

	//--------------------------------------------------------------------
	// 2005.01.27 by cskim
	typedef struct _POST_RCP_DATA_STRUCT
	{
		short int nEnable;
		char pRcpName[80];
	} POST_RCP_DATA_STRUCT;
	POST_RCP_DATA_STRUCT m_RcpData[10][5];
	void PostRunMonThread(int nThreadNo);

	void SetPMModeEnable(int nPM);
	//--------------------------------------------------------------------
	// 2005.12.20
	void CTC_AbortControlThread(int nThreadNo);
	void WPD_Count(int nThreadNo);
};

#endif // !defined(AFX_TMCMONITOR_H__636DEE6B_D87E_4037_BC33_EE434E8C0F99__INCLUDED_)

