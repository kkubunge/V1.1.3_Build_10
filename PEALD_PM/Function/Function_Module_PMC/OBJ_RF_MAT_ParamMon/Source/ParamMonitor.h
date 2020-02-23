// ParamMonitor.h: interface for the CParamMonitor class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_PARAMMONITOR_H__5AAC9554_03C6_4E6F_82FB_D796B619125B__INCLUDED_)
#define AFX_PARAMMONITOR_H__5AAC9554_03C6_4E6F_82FB_D796B619125B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CParamMonitor 
{
public:	
	virtual ~CParamMonitor();

	static CParamMonitor* CreateInstance(const char* szFileName);
	static void SplitString(string text, vector<string> &words);
	static void ParsingConfigure(CParamMonitor *pParamMonitor, vector<string> &words);

	bool UpdateParameters();	
	void CompareParameters();
	
private:
	int m_nAlarmId;
	int m_nUpdateIoPoint;
	int m_nUpdateStatusIoPoint;

	vector<CParamItem> m_ParamItems;

private:
	CParamMonitor();	// this class cannot make instance using new keyword.

};

#endif // !defined(AFX_PARAMMONITOR_H__5AAC9554_03C6_4E6F_82FB_D796B619125B__INCLUDED_)
