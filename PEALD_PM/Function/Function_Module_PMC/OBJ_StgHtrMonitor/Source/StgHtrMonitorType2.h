// StgHtrMonitorType2.h: interface for the CStgHtrMonitorType2 class.
// for Watlow 988 Heater Controller
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STGHTRMONITORTYPE2_H__43364898_CF8A_467D_9F94_65A88851B8B6__INCLUDED_)
#define AFX_STGHTRMONITORTYPE2_H__43364898_CF8A_467D_9F94_65A88851B8B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "StgHtrMonitor.h"
class CStgHtrMonitorType2 : public CStgHtrMonitor , protected CMonThread
{
public:
	CStgHtrMonitorType2();
	virtual ~CStgHtrMonitorType2();

// Member Function
public:
	virtual BOOL Initialize();
	virtual BOOL Initialize(char * szSHTNum);
protected:
	virtual void CheckHtrError();
	virtual void CheckHtrError_SW();
	virtual void DoMonitor();
private:
	BOOL InitW988HeaterChannel();
	void ParseDriverMonOption(char *szData);
	BOOL ReadPmcConfig();

// Member Variable
public:
private:
	BOOL m_bWatlow9x8DriverMon;
	int m_nStgHtrCommErrRetry;
	int m_nStgHtrACommErrCnt;

//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
private:
	static enum {
		HTA_CmdDO = IO_Type1_List_Finished,
		HTA_CommStsDI,
		HTA_AlrmStsDM,
	} IO_INDEX_ENUM;
//--------------------------------------------------------------------
};

#endif // !defined(AFX_STGHTRMONITORTYPE2_H__43364898_CF8A_467D_9F94_65A88851B8B6__INCLUDED_)
