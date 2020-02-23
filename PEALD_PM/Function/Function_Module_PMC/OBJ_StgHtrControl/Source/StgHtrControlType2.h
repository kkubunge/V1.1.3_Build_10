// StgHtrControlType2.h: interface for the CStgHtrControlType2 class.
// For Watlow988
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CStgHtrControlType2_H__70C1E456_B057_4B25_B436_DADDD87B349B__INCLUDED_)
#define AFX_CStgHtrControlType2_H__70C1E456_B057_4B25_B436_DADDD87B349B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StgHtrControl.h"
class CStgHtrControlType2 : public CStgHtrControl 
{
public:
	CStgHtrControlType2();
	virtual ~CStgHtrControlType2();

// Member Function
public:
	virtual BOOL Initialize();
	virtual BOOL Initialize(char *szSHTNum);
};

#endif // !defined(AFX_CStgHtrControlType2_H__70C1E456_B057_4B25_B436_DADDD87B349B__INCLUDED_)
