// PMChmPrsSimulator.h: interface for the CPMChmPrsSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMCHMPRSSIMULATOR_H__4BA2A2D9_95A8_41F5_8FD2_18CC9C1CD54C__INCLUDED_)
#define AFX_PMCHMPRSSIMULATOR_H__4BA2A2D9_95A8_41F5_8FD2_18CC9C1CD54C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"

class CPMChmPrsSimulator  : protected CDynamicIO, protected CMonThread
{
public:
	CPMChmPrsSimulator();
	virtual ~CPMChmPrsSimulator();

// Member Function
public:
	virtual BOOL Initialize();
protected:
	virtual void DoMonitor();
private:
	// 2004.04.17 by cskim
	void ControlPressure(double dblRate);
	// 2004.04.17 by cskim
	void ControlPressureByTVPres();
	void ControlPressureByPos(double fPosition_SetPoint, double fTargetPress);

// Member Variable
public:
	bool m_bChm_SimEnable;
	double m_dbTargetPress;
private:
	double m_dblChmbPress;
	double m_dblDPPress;
};

#endif // !defined(AFX_PMCHMPRSSIMULATOR_H__4BA2A2D9_95A8_41F5_8FD2_18CC9C1CD54C__INCLUDED_)
