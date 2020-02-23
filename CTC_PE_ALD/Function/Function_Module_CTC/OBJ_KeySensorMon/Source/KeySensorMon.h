// LotFlowControl.h: interface for the CLotFlowControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOTFLOWCONTROL_H__07DD716E_886F_4C92_A97C_D1A9DFF40547__INCLUDED_)
#define AFX_LOTFLOWCONTROL_H__07DD716E_886F_4C92_A97C_D1A9DFF40547__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SeqInterface.h"
#include "MonThread.h"

class CKeySensorMon  : public CSeqInterface, protected CMonThread
{
public:
	CKeySensorMon();
	virtual ~CKeySensorMon();

// Member Function
public:
	virtual Module_Status Main();
	virtual void Enter_Code();
	virtual BOOL Initialize();
	virtual void Check_EFEM_KeySensor(int nMAX_WFR_EFEM);
	virtual void Check_LL_KeySensor(int nMAX_WFR_LL);
	virtual void Check_TM_KeySensor(int nMAX_WFR_TM);
	virtual BOOL Check_LPM_INFO(int nMAX_LPM);
	virtual void Check_LLPumpStsMon();
	virtual void Check_TMPumpStsMon();
protected:

// Thread Virtual Function
protected:
	virtual void DoMonitor();

// Member Variable
protected:
	#define MAX_WFR_EFEM		3
	#define MAX_WFR_LL			8				
	#define MAX_WFR_TM			4				

	typedef struct _KEY_SENSOR_EFEM_STRU
	{
		int  nszWaferSts;
		int  nszWaferRlt;
		int  nszWaferSrc;
		char szLotID[40];
		char szPPID[40];
	} KEY_SENSOR_EFEM_STRU;

	KEY_SENSOR_EFEM_STRU m_KeySenosrEFEM[MAX_WFR_EFEM];

	typedef struct _KEY_SENSOR_LL_STRU
	{
		int  nszWaferSts;
		int  nszWaferRlt;
		int  nszWaferSrc;
		char szLotID[40];
		char szPPID[40];
	} KEY_SENSOR_LL_STRU;

	_KEY_SENSOR_LL_STRU m_KeySenosrLL[MAX_WFR_LL];

	typedef struct _KEY_SENSOR_TM_STRU
	{
		int  nszWaferSts;
		int  nszWaferRlt;
		int  nszWaferSrc;
		char szLotID[40];
		char szPPID[40];
	} KEY_SENSOR_TM_STRU;

	_KEY_SENSOR_TM_STRU m_KeySenosrTM[MAX_WFR_TM];
	
	//2015.06.10
	char m_szLPM1PJID[256];
	char m_szLPM2PJID[256];
	char m_szLPM3PJID[256];

	double m_LP1_PM1WaferDCOPSUM[25];
	double m_LP1_PM2WaferDCOPSUM[25];
	double m_LP1_PM3WaferDCOPSUM[25];	

	double m_LP2_PM1WaferDCOPSUM[25];
	double m_LP2_PM2WaferDCOPSUM[25];
	double m_LP2_PM3WaferDCOPSUM[25];	

	double m_LP3_PM1WaferDCOPSUM[25];
	double m_LP3_PM2WaferDCOPSUM[25];
	double m_LP3_PM3WaferDCOPSUM[25];	

	int m_nLP1_PM1WaferCount;
	int m_nLP1_PM2WaferCount;
	int m_nLP1_PM3WaferCount;

	int m_nLP2_PM1WaferCount;
	int m_nLP2_PM2WaferCount;
	int m_nLP2_PM3WaferCount;

	int m_nLP3_PM1WaferCount;
	int m_nLP3_PM2WaferCount;
	int m_nLP3_PM3WaferCount;
};

#endif // !defined(AFX_LOTFLOWCONTROL_H__07DD716E_886F_4C92_A97C_D1A9DFF40547__INCLUDED_)
