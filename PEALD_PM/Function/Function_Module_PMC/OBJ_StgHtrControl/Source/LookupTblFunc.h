// LookupTblFunc.h: interface for the CLookupTblFunc class.
// Create : 2007.03.08
// Last Update : 2007.03.08
// Writter : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOKUPTBLFUNC_H__B2DFEC6B_179E_492E_9A87_214B42E596CD__INCLUDED_)
#define AFX_LOOKUPTBLFUNC_H__B2DFEC6B_179E_492E_9A87_214B42E596CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _LOOKUP_TBL
{
	double dbValueA;
	double dbValueB;
} LOOKUP_TBL;

//////////////////////////////////////////////////////////////////////
class CLookupTblFunc
{
public:
	CLookupTblFunc();
	CLookupTblFunc(LOOKUP_TBL aLookupTbl[], int nTableCount);
	virtual ~CLookupTblFunc();

// Member Function
public:
	void SetTable(LOOKUP_TBL aLookupTbl[], int nTableCount);
	double GetScaleValue(double dbValueA);			// ValueA -> ValueB
	double GetRevScaleValue(double dbValueB);		// ValueB -> ValueA

// Member Variable
public:
protected:
	int m_nTableCount;
	LOOKUP_TBL *m_pLookupTbl;
};

#endif // !defined(AFX_LOOKUPTBLFUNC_H__B2DFEC6B_179E_492E_9A87_214B42E596CD__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2007.03.08	<cskim> Create
			정해진 Lookup Table에 따라 값을 Scale하여 연산하는 Class.
*/