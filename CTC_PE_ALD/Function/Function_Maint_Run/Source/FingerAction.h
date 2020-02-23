// FingerAction.h: interface for the CFingerAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINGERACTION_H__E060B048_DEFB_40E0_993B_2A8D7AE210BF__INCLUDED_)
#define AFX_FINGERACTION_H__E060B048_DEFB_40E0_993B_2A8D7AE210BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct sFinger
{
	 sFinger() {memset(szFingerName , 0 , sizeof(szFingerName)); nSWEnum = 0; enFingerAction = SLOT1; pNext = NULL; pPrev = NULL;};
	~sFinger() {};

	char szFingerName[32];
	int  nSWEnum;
	FingerACTType enFingerAction; 

	sFinger* pNext;
	sFinger* pPrev;
};

class CFingerAction  
{
public:
	CFingerAction();
	virtual ~CFingerAction();

	BOOL   AddFingerAction(char* szFingerName , char* szActionType , int nSWEnum);
	BOOL   GetFingerInfo  (int nSWEnum/*IN*/ , char* szFingerName/*OUT*/ , FingerACTType& enType/*OUT*/);
	BOOL   GetFingerEnum  (FingerACTType enType/*IN*/ , char* szFingerName/*IN*/ ,  int& nSWEnum/*OUT*/);

private:
	BOOL   IsValidFingerAction(int nSWEnum);

private:
	sFinger* m_psFingerList;
	sFinger* m_psCurrentList;
	sFinger* m_psStartList;

};

#endif // !defined(AFX_FINGERACTION_H__E060B048_DEFB_40E0_993B_2A8D7AE210BF__INCLUDED_)
