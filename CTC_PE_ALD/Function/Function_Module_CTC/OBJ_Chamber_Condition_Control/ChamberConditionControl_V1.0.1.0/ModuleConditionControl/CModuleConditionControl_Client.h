// // Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CMODULECONDITIONCONTROL_CLIENT_4146957802FD_INCLUDED
#define _INC_CMODULECONDITIONCONTROL_CLIENT_4146957802FD_INCLUDED

class CModuleConditionControl_Component;

class CModuleConditionControl_Client 
{
public:

	BOOL MsgParser(string strEvent);

	CModuleConditionControl_Client();
	virtual ~CModuleConditionControl_Client();

private:
	BOOL m_bRunPost;

	BOOL CheckSequenceNotRunning(BOOL bAutoControl);

	BOOL CheckLOTNotRunning(BOOL bAutoControl);

	double m_doubIDLETime;

	BOOL CheckChamberConnection(BOOL bAutoControl);

	BOOL CheckChamberEnable(BOOL bAutoControl);

	BOOL CheckGateValve(BOOL bAutoControl);

	BOOL CheckWaferExistence(BOOL bAutoControl);

	BOOL Run(int nIndexComposite, BOOL bAutoControl);
	void SetAutoControl(BOOL bEnable, int nIndexComposite, int nIndexLeaf);
	void ClearRecipe(int nComposite , int nLeaf);
	void SetModuleModeToEnableAfterRun(BOOL bEnable, int nIndexComposite);
	void Abort(int nIndexComposite);

	virtual BOOL CheckDRYCleaningCondition(BOOL bAutoControl);

	BOOL CheckWETCleaningCondition(BOOL bAutoControl);

	BOOL CheckIDLECondition(BOOL bAutoControl);

	BOOL CheckPreTreatmentCondition(BOOL bAutoControl);

	DWORD m_dwThreadIDOfRunFunction;
	DWORD m_dwThreadIDOfCheckTime;

	static DWORD WINAPI Thread_RunFunction(LPVOID lpParam);
	static DWORD WINAPI Thread_CheckTime(LPVOID lpParam);
	static DWORD WINAPI Thread_EventHandling(LPVOID lpParam);

	vector<CModuleConditionControl_Component*> m_vecComponent;

};

#endif /* _INC_CMODULECONDITIONCONTROL_CLIENT_4146957802FD_INCLUDED */
