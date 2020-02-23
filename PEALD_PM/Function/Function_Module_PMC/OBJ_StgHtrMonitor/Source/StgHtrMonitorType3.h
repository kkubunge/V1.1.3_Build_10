// StgHtrMonitorType3.h: interface for the CStgHtrMonitorType3 class.
// Create : 2007.03.09
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STGHTRMONITORTYPE3_H__D33772D0_031E_4158_9CEB_CC0653E19428__INCLUDED_)
#define AFX_STGHTRMONITORTYPE3_H__D33772D0_031E_4158_9CEB_CC0653E19428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StgHtrMonitor.h"
class CStgHtrMonitorType3 : public CStgHtrMonitor
{
public:
	CStgHtrMonitorType3();
	virtual ~CStgHtrMonitorType3();

// Member Function
public:
	virtual BOOL Initialize();
	virtual BOOL Initialize(char * szSHTNum);
protected:
	virtual void ReadHeaterTemp();
	virtual void CheckHtrError();

//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
//public:
//		int m_nSHTNum;
		
private:
	static enum {
		SHT_RatioAIO = IO_Type1_List_Finished,
		SHT_TmpSpAI,
		StgHtrRelayDO,
		StgHtrRelayDI,
		//... 2016.02.28
		StgHtr_CurrAI,
		StgHtr_VoltAI,
		STH_CurHiLimitAM,
		STH_CurLwLimitAM,
		STH_VtgHiLimitAM,
		STH_VtgLwLimitAM,
		//...
	} IO_INDEX_ENUM;
protected:
	double m_dbTmpSpAI;
	double m_dbCurrentAI;
	double m_dbVoltageAI;
	double m_dbCurrHiLimit; 
	double m_dbCurrLwLimit;
	double m_dbVtgHiLimit;
	double m_dbVtgLwLimit;
//--------------------------------------------------------------------
};

#endif // !defined(AFX_STGHTRMONITORTYPE3_H__D33772D0_031E_4158_9CEB_CC0653E19428__INCLUDED_)
