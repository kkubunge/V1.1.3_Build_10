// PLCSimulator.h: interface for the CPLCSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLCSIMULATOR_H__3B07B202_34A1_4E23_A712_D05CC7C183FB__INCLUDED_)
#define AFX_PLCSIMULATOR_H__3B07B202_34A1_4E23_A712_D05CC7C183FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonThread.h"
#include "DynamicIO.h"
class CPLCSimulator  : protected CDynamicIO, protected CMonThread
{
public:
	CPLCSimulator();
	virtual ~CPLCSimulator();

	// Member Function
public:
	virtual BOOL Initialize();
	BOOL ReadSimCfg();
	void ParseVrcpConfig(char *szRead);
	void ParseMfcCfg(char *szRead);
	void ParseValveListCfg(char *szRead);
	void ParseRFCfg(char *szParam);			// 2006.03.06

	Module_Status SetValveData(char *pParam);
	Module_Status Download(char *pParam);
	Module_Status CloseAllValve();
	Module_Status StartValveRecipe();
	Module_Status ResumeValveRecipe();
	Module_Status StopValveRecipe();
	Module_Status GoNextStep();

protected:
	virtual void DoMonitor();
	virtual BOOL ExcuteVRCPLine(unsigned char *pData);
	virtual void SetMFC();
	virtual Module_Status RunValveRecipe();
	virtual void SetRF();				// 2006.03.06
	virtual void SetRF2();
	virtual void SetHTE();
	BOOL WaitPLCTm(double dbTime);		// 2006.07.31

	virtual void DoMonitor_B();
	int CheckAlmOpt(int nAlarm, int nAlmOptChannel);
	enum {
		AOPT_IGNORE, AOPT_WARNING, AOPT_ABORT
	};

	// Member Variable
public:

protected:
	int m_nValveTimeRes;		// 0:10ms, 1:100ms

	#define VR_MAX_STEP		10
	#define VR_STEP_SIZE	200
//	#define VR_PAGE_SIZE	2
    #define VR_PAGE_SIZE	5
	#define VR_HEADER_SIZE	100
	int m_nRunStep;
	int m_nRunPage;
	unsigned char m_VR_Step[VR_PAGE_SIZE][VR_MAX_STEP][VR_STEP_SIZE];
	unsigned char m_VR_Header[VR_PAGE_SIZE][VR_HEADER_SIZE];
	int m_nPLC_VrcpSts;

	#define MAX_MFC		20
	BOOL m_bGoNext;
	int m_pnMfcScale[MAX_MFC];
	int m_pnMfcPhyMax[MAX_MFC];

	//-----------------------------------------------------------------
	// 2006.03.06
	int m_nRfScale;
	int m_nRfPhyMax;

	//-----------------------------------------------------------------
	// for Valve I/O Control
	#define MAX_VALVE_IO	150
protected:
	void InitValveIO();
	void SetValveStatus(int nValveNum, int nValue);		// 0:Close 1:Open
	int GetValveStatus(int nValveNum);
	IO_LIST m_ValveList[MAX_VALVE_IO+1];
	CDynamicIO m_DynValveCtrl;

	//-----------------------------------------------------------------
	// 2006.07.31
protected:
	class CTimer *m_pTimer;
	double m_dbVrcpProgress;

	//... 2015.04.08 Add RF Forward/Reflect Count Up
	int		m_nRF1LoadCnt;
	int		m_nRF2LoadCnt;
	int		m_nRF1ReflCnt;
	int		m_nRF2ReflCnt;

};

#endif // !defined(AFX_PLCSIMULATOR_H__3B07B202_34A1_4E23_A712_D05CC7C183FB__INCLUDED_)

