// InterlockMonitorTM.h: interface for the CInterlockMonitorTM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERLOCKMONITORTM_H__9DDB6080_1A6C_491D_B233_DAAF43B06CFB__INCLUDED_)
#define AFX_INTERLOCKMONITORTM_H__9DDB6080_1A6C_491D_B233_DAAF43B06CFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InterlockMonitor.h"

class CInterlockMonitorTM  : public CInterlockMonitor
{
public:
	CInterlockMonitorTM();
	virtual ~CInterlockMonitorTM();

// Member Function
public:
	virtual BOOL Initialize();
	virtual void Deinitialize();

protected:
	void AbortLot();
	virtual void ExecInterlock(int nIntType);


// Member Variable
protected:
	CDynamicIO m_DynIO;

};

#endif // !defined(AFX_INTERLOCKMONITORTM_H__9DDB6080_1A6C_491D_B233_DAAF43B06CFB__INCLUDED_)
