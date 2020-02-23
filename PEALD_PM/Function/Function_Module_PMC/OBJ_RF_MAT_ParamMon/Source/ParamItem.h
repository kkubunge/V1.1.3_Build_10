// ParamItem.h: interface for the CParamItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAMITEM_H__ADA19CE5_4EBF_441E_9286_82A6A43FFE72__INCLUDED_)
#define AFX_PARAMITEM_H__ADA19CE5_4EBF_441E_9286_82A6A43FFE72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CParamItem  
{
public:
	CParamItem();
	virtual ~CParamItem();

	const char* GetVirtualIOName() { return m_szVirtualIOName; }
	const char* GetRealIOName() { return m_szRealIOName; }
	const int GetVirtualIOPoint() { return m_nVirtualIOPoint; }
	const int GetRealIOPoint() { return m_nRealIOPoint; }

	void SetVirtualIOName(const char *szVirtualIOName);
	void SetRealIOName(const char *szRealIoName);
	void SetIOType(string strType);

	bool Compare();

	string ToString();


private:
	char m_szVirtualIOName[80];
	char m_szRealIOName[80];
	int m_nVirtualIOPoint;
	int m_nRealIOPoint;

	IO_Status m_nIoType;
};

#endif // !defined(AFX_PARAMITEM_H__ADA19CE5_4EBF_441E_9286_82A6A43FFE72__INCLUDED_)
