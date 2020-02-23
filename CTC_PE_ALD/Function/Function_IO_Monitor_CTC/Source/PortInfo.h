// PortInfo.h: interface for the CPortInfo class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_PORTINFO_H__2B8688E2_B0F9_441C_BA15_3988965BA8A4__INCLUDED_)
#define AFX_PORTINFO_H__2B8688E2_B0F9_441C_BA15_3988965BA8A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Utility.h"

class CPortInfo  
{
public:
	CPortInfo();
	virtual ~CPortInfo();

	bool  SetModuleName  (char* szModuleName);
	bool  SetPortNum     (unsigned short usPortNum);
	unsigned short GetPortNum();
	char* GetModuleName  ();
	

private:
	char m_szModuleName[32];
	unsigned short m_usPortNum;
	
};

#endif // !defined(AFX_PORTINFO_H__2B8688E2_B0F9_441C_BA15_3988965BA8A4__INCLUDED_)
