// StgHtrControl.h: interface for the CStgHtrControl class.
// For Omron-E5ER
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CStgHtrControl_H__C26D5315_69BB_45A7_AAB9_2519A3F01FD3__INCLUDED_)
#define AFX_CStgHtrControl_H__C26D5315_69BB_45A7_AAB9_2519A3F01FD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
class CStgHtrControl  : public CDynamicIO
{
public:
	CStgHtrControl();
	virtual ~CStgHtrControl();

// Member Function
public:
	virtual BOOL Initialize();
	virtual BOOL Initialize(char *szSHTNum);
	virtual Module_Status CheckSetPoint(char *szParam);
	virtual Module_Status CheckSetPoint2(char *szParam);
	virtual Module_Status CheckTemp(char *szParam);
	virtual Module_Status CheckUpper(char *szParam);
	virtual Module_Status CheckLower(char *szParam);
	virtual Module_Status SetTemp(char *szParam);
	virtual Module_Status SetIfUpper(char *szParam);
	virtual Module_Status SetIfLower(char *szParam);
	virtual Module_Status IsUpper(char *szParam);
	virtual Module_Status IsLower(char *szParam);

	virtual Module_Status CoolDown(char *szParam) { return SYS_ABORTED; }
	virtual Module_Status ResetSetPoint(char *szParam){ return SYS_ABORTED;  }
	virtual Module_Status SetRelay(char *szParam) { return SYS_ABORTED; }

// Member Variable
public:
	enum { SHSS_Stable, SHSS_Unstable, SHSS_Moving, SHSS_CoolDown };

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
		SHT_StbStsDM,
		// 2006.01.18
		SHT_CalOffsAIO,
		IO_Type1_List_Finished			// 2007.03.07
	} IO_INDEX_ENUM;
//--------------------------------------------------------------------
public:
		int m_nSHTNum;
};

#endif // !defined(AFX_CStgHtrControl_H__C26D5315_69BB_45A7_AAB9_2519A3F01FD3__INCLUDED_)
