// TransferArea.h: interface for the CTransferArea class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_TRANSFERAREA_H__3F297E66_210F_4F6C_8143_9335DC174B94__INCLUDED_)
#define AFX_TRANSFERAREA_H__3F297E66_210F_4F6C_8143_9335DC174B94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CArea;
struct sArea
{
	 sArea() {memset(szAreaName , 0 , sizeof(szAreaName)); nMaxSlot = 0; nCapacity = 0; pNext = NULL; pPrev = NULL; pArea = NULL;};
	~sArea() {};

	char   szAreaName[32];
	int    nMaxSlot;
	int    nCapacity;
	int    nSWEnum;

	CArea* pArea;

	sArea* pNext;
	sArea* pPrev;
};

class CTransferArea  
{
public:
	CTransferArea();
	virtual ~CTransferArea();

	BOOL   AddArea      (char* szAreaName , int nMaxSlot , int nCapacity , int nSWEnum);
	sArea* GetFirstArea ();
	sArea* GetNextArea  ();

	sArea* GetFirstClear ();
	sArea* GetNextClear  ();

	CArea* GetArea (char* szAreaName);
	CArea* GetArea (int   nSWEnum);
	sArea* GetSArea(char* szAreaName);
	sArea* GetSArea(int   nSWEnum);

	int    GetAreaCount ();

private:
	BOOL   IsValidArea (char* szAreaName , int nSWEnum);

private:
	sArea* m_psAreaList;
	sArea* m_psCurrentList;
	sArea* m_psCurrentClear;
	sArea* m_psStartList;
};

#endif // !defined(AFX_TRANSFERAREA_H__3F297E66_210F_4F6C_8143_9335DC174B94__INCLUDED_)
