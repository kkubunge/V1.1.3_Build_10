// IOSyncMonitor.h: interface for the CIOSyncMonitor class.
// 2008.08.25
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOSYNCMONITOR_H__2562146B_7279_44FE_BB37_4DA61F65C6FB__INCLUDED_)
#define AFX_IOSYNCMONITOR_H__2562146B_7279_44FE_BB37_4DA61F65C6FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"
class CIOSyncMonitor : protected CMonThread
{
public:
	CIOSyncMonitor();
	virtual ~CIOSyncMonitor();

public:
	void SetIOSyncList(const char *szIOName1, const char *szIOName2, const char *szIOType);
	virtual BOOL Initialize();

protected:
	virtual void DoMonitor();

private:
	int m_nIOSyncCount;
	CDynamicIO m_DynIO1, m_DynIO2;
};

#endif // !defined(AFX_IOSYNCMONITOR_H__2562146B_7279_44FE_BB37_4DA61F65C6FB__INCLUDED_)
