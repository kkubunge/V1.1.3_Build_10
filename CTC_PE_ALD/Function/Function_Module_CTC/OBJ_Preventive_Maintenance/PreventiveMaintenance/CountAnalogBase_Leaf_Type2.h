// CountAnalogBase_Leaf_Type2.h: interface for the CCountAnalogBase_Leaf_Type2 class.
// 2006.10.18
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COUNTANALOGBASE_LEAF_TYPE2_H__64E49CE9_E2BF_4A90_8AB7_5EA7D2FA956E__INCLUDED_)
#define AFX_COUNTANALOGBASE_LEAF_TYPE2_H__64E49CE9_E2BF_4A90_8AB7_5EA7D2FA956E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CCountAnalogBase_Leaf.h"
class CCountAnalogBase_Leaf_Type2  : public CCountAnalogBase_Leaf
{
public:
	CCountAnalogBase_Leaf_Type2(int nLeafType , int nCompositeType);
	virtual ~CCountAnalogBase_Leaf_Type2();

// Member Function
	virtual int ChangeCurrentValue(void* pData, int nLeafType);
};

#endif // !defined(AFX_COUNTANALOGBASE_LEAF_TYPE2_H__64E49CE9_E2BF_4A90_8AB7_5EA7D2FA956E__INCLUDED_)
