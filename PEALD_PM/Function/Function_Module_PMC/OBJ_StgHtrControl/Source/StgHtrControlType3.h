// StgHtrControlType3.h: interface for the CStgHtrControlType3 class.
// For Eurotherm-3508 (2 Zone Heater)
// Create : 2007.03.07
// Writter : Chang-su, Kim
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STGHTRCONTROLTYPE3_H__C95596C4_1D9F_4E27_9F38_7B0BE2BA1EC1__INCLUDED_)
#define AFX_STGHTRCONTROLTYPE3_H__C95596C4_1D9F_4E27_9F38_7B0BE2BA1EC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "StgHtrControl.h"
class CStgHtrControlType3 : public CStgHtrControl, public CMonThread 
{
public:
	CStgHtrControlType3();
	virtual ~CStgHtrControlType3();

// Member Function
public:
	virtual BOOL Initialize();
	virtual BOOL Initialize(char *szSHTNum);
	virtual Module_Status CoolDown(char *szParam);
	virtual Module_Status ResetSetPoint(char *szParam);
	virtual Module_Status SetRelay(char *szParam);
	virtual Module_Status SetTemp(char *szParam);

protected:
	virtual void DoMonitor();

//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
private:
	static enum {
		SHT_TmpSpAI = IO_Type1_List_Finished,
		SHT_RatioAIO,

		StgHtrRelayDO,
		StgHtrRelayDI,

		PRMA_TO_SHTCoDn,			// Unit:Minute
		PRMA_SHTCoDnTemp,
		PRMA_SHtrTemp1,
		PRMA_SHtrTemp2,
		PRMA_SHtrTemp3,
		PRMA_SHtrTemp4,
		PRMA_SHtrTemp5,
		PRMA_SHtrTemp6,
		PRMA_SHtrRatio1,
		PRMA_SHtrRatio2,
		PRMA_SHtrRatio3,
		PRMA_SHtrRatio4,
		PRMA_SHtrRatio5,
		PRMA_SHtrRatio6,

		PRMD_SHTAutoRat,
	} IO_INDEX_ENUM;

//public:
//		int m_nSHTNum;
};

#endif // !defined(AFX_STGHTRCONTROLTYPE3_H__C95596C4_1D9F_4E27_9F38_7B0BE2BA1EC1__INCLUDED_)
