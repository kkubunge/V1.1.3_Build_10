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

class CMFCFlowTest  : public CSeqInterface
{
public:
	CMFCFlowTest();
	virtual ~CMFCFlowTest();
	
	// Member Function
public:
	virtual void Enter_Code();
	virtual BOOL Initialize();
	virtual BOOL Deinitialize();
	virtual Module_Status Main();
	
	
private:
	Module_Status MFCFlowTest();
	BOOL FastPumping();
	void MFC_FlowTest_Init();
	BOOL ReadConfig();
	void ParseMFCList(char *szData);
	void ParseChamberConfig(char *szData);
	Module_Status SET_MANUAL_MFC(); //... 2018.06.24
	
	// Member Variable
private:
	CValveSeq m_MFCValveSeq;
	
#define	MFC_COUNT			9
#define MAX_MFC				9
#define MAX_CHECK_LIST		5
	enum {NOTUSE_0, PURGE_1, DIVERT_2};

	typedef struct _MFC_GAS_STRUCT
	{
		char szName[40];
	} MFC_GAS_STRUCT;
	
	MFC_GAS_STRUCT m_MFC_List[MAX_MFC];
	
	double m_dbPumpTime;			// sec
	double m_dbAlarmTolerance;		// %
	int m_nALARM_MfcFlowFail;
	int m_Step_MFCSet[5];			// 2012.08.14	
	char MFCTestInfo[512];
};

#endif // !defined(AFX_MFCVerification_H__EC377794_35AA_43D7_AA04_38BD0457CF01__INCLUDED_)
