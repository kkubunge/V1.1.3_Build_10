// StgHtrMonitor.h: interface for the CStgHtrMonitor class.
// for Omron E5ER Heater Controller
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STGHTRMONITOR_H__A3DEC2EE_9590_4883_A6B5_64BB876DB28D__INCLUDED_)
#define AFX_STGHTRMONITOR_H__A3DEC2EE_9590_4883_A6B5_64BB876DB28D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
class CTimer;
class CStgHtrMonitor  : public CDynamicIO
{
public:
	CStgHtrMonitor();
	virtual ~CStgHtrMonitor();

// Member Function
public:
	virtual BOOL Initialize();
	virtual BOOL Initialize(char * szSHTNum);
	virtual Module_Status StartMonitor();

protected:
	virtual void ReadHeaterTemp();
	virtual void CheckHtrError();
	virtual void CheckHtrError_SW();

	void ApplyAlarmEffect(int nMonOpt);
	void CheckTempStable();

// Member Variable
public:
	enum { MO_Ignore, MO_Warning, MO_ChmDis, MO_Abort };
	enum { SHSS_Stable, SHSS_Unstable, SHSS_Moving, SHSS_CoolDown };
public:
	int m_nMonMethod;	// 0:HW Monitoring 1:SW Monitoring

protected:
	int m_nStableCount;
	double m_dbTmpRd;
	double m_dbTmpSp;
	double m_dbHighLmt;
	double m_dbLowLmt;
	double m_dbTol;
	int m_nStableSts;	// 0:Stable 1:Unstable 2:Moving
	int m_nStableTm;
	CTimer *m_pTmr;

//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
public:
	static enum {
		SHT_TmpRdAI,
		SHT_TmpSpAIO,
		SHT_TmpHgLmtAIO,
		SHT_TmpLwLmtAIO,
		SHT_RampRateRAIO,
		SHT_RampRateFAIO,
		SHT_CalOffsAIO,

		SHT_StbStsDM,
		SHT_MonOptDM,

		SHT_OutEnDO,
		SHT_Error1DI,
		SHT_Error2DI,
		SHT_AlrmSts1DI,

		IO_Type1_List_Finished
	} IO_INDEX_ENUM;
//--------------------------------------------------------------------
public:
		int m_nSHTNum;
};

#endif // !defined(AFX_STGHTRMONITOR_H__A3DEC2EE_9590_4883_A6B5_64BB876DB28D__INCLUDED_)
