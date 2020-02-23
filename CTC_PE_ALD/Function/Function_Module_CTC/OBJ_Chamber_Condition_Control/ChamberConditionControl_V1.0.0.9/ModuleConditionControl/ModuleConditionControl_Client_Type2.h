// ModuleConditionControl_Client_Type2.h: interface for the CModuleConditionControl_Client_Type2 class.
// Create : 2006.10.18
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULECONDITIONCONTROL_CLIENT_TYPE2_H__3587365B_8BE5_46E8_94E7_302BBEB00085__INCLUDED_)
#define AFX_MODULECONDITIONCONTROL_CLIENT_TYPE2_H__3587365B_8BE5_46E8_94E7_302BBEB00085__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CModuleConditionControl_Client.h"
class CModuleConditionControl_Client_Type2 :  public CModuleConditionControl_Client
{
public:
	CModuleConditionControl_Client_Type2();
	virtual ~CModuleConditionControl_Client_Type2();

	virtual BOOL CheckDRYCleaningCondition(BOOL bAutoControl);
};

#endif // !defined(AFX_MODULECONDITIONCONTROL_CLIENT_TYPE2_H__3587365B_8BE5_46E8_94E7_302BBEB00085__INCLUDED_)
