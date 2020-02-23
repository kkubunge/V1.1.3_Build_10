// CountBase_Leaf_Type2.h: interface for the CCountBase_Leaf_Type2 class.
// 2006.10.18
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COUNTBASE_LEAF_TYPE2_H__2D4C4003_EAE7_41FD_9E79_FCA9A399DEDE__INCLUDED_)
#define AFX_COUNTBASE_LEAF_TYPE2_H__2D4C4003_EAE7_41FD_9E79_FCA9A399DEDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CCountBase_Leaf.h"
class CCountBase_Leaf_Type2  : public CCountBase_Leaf
{
public:
	CCountBase_Leaf_Type2(int nLeafType , int nCompositeType);
	virtual ~CCountBase_Leaf_Type2();

// Member Function
	virtual int  ChangeCurrentValue(void* pData, int nLeafType);
	virtual bool IsPostRunTime(int nType);

};

#endif // !defined(AFX_COUNTBASE_LEAF_TYPE2_H__2D4C4003_EAE7_41FD_9E79_FCA9A399DEDE__INCLUDED_)
