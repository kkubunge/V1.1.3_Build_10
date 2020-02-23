// TMCtrlSimulator.h: interface for the CTMCtrlSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMCTRLSIMULATOR_H__57334EED_3A2A_4DDB_BA10_F35E9F756ED8__INCLUDED_)
#define AFX_TMCTRLSIMULATOR_H__57334EED_3A2A_4DDB_BA10_F35E9F756ED8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
class CTMCtrlSimulator  : protected CDynamicIO
{
public:
	CTMCtrlSimulator();
	CTMCtrlSimulator(char *szComponentName);
	virtual ~CTMCtrlSimulator();

	// Member Function
public:
	virtual BOOL Initialize();
	virtual BOOL Main(char *szParam);
private:
	BOOL ReadSimConfig();
	void ParseCtrlConfig(char *szRead);
	BOOL ControlCM(char *szCmd);
	BOOL ControlAtmRobot(char *szCmd);
	BOOL ControlVacRobot(char *szCmd);
	BOOL ControlAligner(char *szCmd);
	BOOL ControlBM(char *szCmd);
	BOOL ControlMainBM(char *szCmd);
	BOOL ControlBMDoor(char *szCmd);
	BOOL ControlIsoGate(char *szCmd);

	BOOL SetTimeTable(char *szCmd, double fTime);
	BOOL GetTimeTable(char *szCmd, double *pdbTime);
	BOOL FindTimeTable(char *szCmd);
	// Member Variable
public:
	char m_szComponentName[40];
	char szTemp[20];
	char m_OldDest[20];
private:

};

#endif // !defined(AFX_TMCTRLSIMULATOR_H__57334EED_3A2A_4DDB_BA10_F35E9F756ED8__INCLUDED_)
