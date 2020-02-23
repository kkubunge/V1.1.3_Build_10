// DummyMonitor.h: interface for the CDummyMonitor class.
// Created 2007.05.08 
// Dummy Slot & Wafer Monitor class
// Revision Info : 
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_DUMMYMONITOR_H__746AAF35_434A_4811_B0D5_70E48FB7A2AF__INCLUDED_)
#define AFX_DUMMYMONITOR_H__746AAF35_434A_4811_B0D5_70E48FB7A2AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "DynamicIO.h"

class CDummySlotInfo;
class CDummyMonitor  
{
public:
	CDummyMonitor();
	virtual ~CDummyMonitor();
	BOOL InitializeMonitor  (int nAlarm , int nWarning);
	BOOL ChangeSlotDummyMode(int nSlot , int nMode);
	void RemoveDummy        ();

	//2007.12.06 mgsong
	void ResetCount         (char* szSlot);

private:
	static DWORD WINAPI Thread_DummyMonitoring(LPVOID lpParam);
	void   InitialSaveDummyMode();
	BOOL   InitLock();
	void   LockSharedData  (HANDLE hMutex);
	void   UnlockSharedData(HANDLE hMutex);

private:
	CDummySlotInfo* m_pDummySlotInfo;

	int m_nAlarmStartID;
	int m_nWarningStartID;

	HANDLE m_hMutex;

};

#endif // !defined(AFX_DUMMYMONITOR_H__746AAF35_434A_4811_B0D5_70E48FB7A2AF__INCLUDED_)
