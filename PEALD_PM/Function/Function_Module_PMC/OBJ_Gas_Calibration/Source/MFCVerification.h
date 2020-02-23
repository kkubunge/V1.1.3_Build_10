// MFCVerification.h: interface for the CMFCVerification class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCVerification_H__EC377794_35AA_43D7_AA04_38BD0457CF01__INCLUDED_)
#define AFX_MFCVerification_H__EC377794_35AA_43D7_AA04_38BD0457CF01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValveSeq.h"
#include "SeqInterface.h"

class CMFCVerification  : public CSeqInterface, protected CMonThread
{
public:
	CMFCVerification();
	virtual ~CMFCVerification();

// Member Function
public:
	virtual void Enter_Code();
	virtual BOOL Initialize();
	virtual BOOL Deinitialize();
	virtual Module_Status Main();


private:
	Module_Status Verification_Control(int GasSelect, int nMfcSetIdx, int nManualFlow);	//... 2016.02.16
	Module_Status MFC_VERIFY_LOGGING();

	BOOL ReadConfig();
	void ParseMFCList(char *szData);
	void ParseChamberConfig(char *szData);

	// Not Used
	void Abort_Test();
	Module_Status Get_Base_Verify(int GasSelect, int nMfcSetIdx, int nManualFlow);   //... 2016.02.16
	Module_Status PLCInterlockControl(int nInterlockSts); //... 2016.02.16
	void CalMFCFlowRate(int nMax);
	Module_Status MFC_Verification(int nGasType);

protected:
	virtual void DoMonitor();

// Member Variable
private:
	CValveSeq m_MFCValveSeq;

	#define	MFC_COUNT			18
	#define MAX_MFC				18
	#define MAX_GAS_PER_MFC		5
	typedef struct _MFC_GAS_STRUCT
	{
		char szName[40];
		char szCalGasName[MAX_GAS_PER_MFC][40];
		double dbConvFactor[MAX_GAS_PER_MFC];
	} MFC_GAS_STRUCT;

	MFC_GAS_STRUCT m_MFC_List[MAX_MFC];

	double m_dbChamberVol;
	double m_dbPressCheckErr;
	double m_dbEndPressCheck;
	double m_dbPumpTime;			// sec
	double m_dbPurgeTime;			// sec
	double m_dbStableTime;			// sec
	double m_dbCompareTemp;
	int m_nALARM_MfcVerFail;

	int m_Step_MFCSet[5];				// 2012.08.14
	int m_nBaseMFCNo1;				//... 2016.02.16
	int m_nBaseMFCNo2;				
	BOOL m_bGetBaseVerfiyStartBit;
};

#endif // !defined(AFX_MFCVerification_H__EC377794_35AA_43D7_AA04_38BD0457CF01__INCLUDED_)
