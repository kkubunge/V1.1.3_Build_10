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

class CGemMonitor  : public CDynamicIO, public CMonThread
{
public:
	CGemMonitor();
	virtual ~CGemMonitor();

	// Member Function
public:
	virtual BOOL Initialize();

protected:
	virtual void DoMonitor(int nThreadNo);
	void PM1_DryCleaning_Status_Check_Thread(int nThreadNo);
	void PM2_DryCleaning_Status_Check_Thread(int nThreadNo);
	void PM3_DryCleaning_Status_Check_Thread(int nThreadNo);

private:

};

#endif // !defined(AFX_TMCMONITOR_H__636DEE6B_D87E_4037_BC33_EE434E8C0F99__INCLUDED_)

