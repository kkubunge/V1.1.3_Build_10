// DummySlotInfo.h: interface for the CDummySlotInfo class.
// Created 2007.05.08 
// Dummy Slot Info Save Class
// Revision Info : 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYSLOTINFO_H__2A368A73_AE89_4F1F_8096_941317CE8FDF__INCLUDED_)
#define AFX_DUMMYSLOTINFO_H__2A368A73_AE89_4F1F_8096_941317CE8FDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDummySlotInfo  
{
public:
	CDummySlotInfo();
	virtual ~CDummySlotInfo();

	void InitializeMember();
	void SetAlarmStatus  (int nSlotNum , long enSts);
	long GetAlarmStatus  (int nSlotNum);
	void SetDummyMode    (int nSlotNum , long enMode);
	long GetDummyMode    (int nSlotNum);
	BOOL GetChangeFlag   (int nSlotNum);
	void SetChangeFlag   (int nSlotNum);
	void ResetChangeFlag (int nSlotNum);

private:
	DUMMY_INFO m_SDummyInfo[MAX_SLOT];
};

#endif // !defined(AFX_DUMMYSLOTINFO_H__2A368A73_AE89_4F1F_8096_941317CE8FDF__INCLUDED_)
