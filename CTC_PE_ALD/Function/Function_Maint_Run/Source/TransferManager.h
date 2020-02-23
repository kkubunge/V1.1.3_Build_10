// TransferManager.h: interface for the CTransferManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSFERMANAGER_H__783E5236_4E01_40AD_AB7C_EC70642F8BAC__INCLUDED_)
#define AFX_TRANSFERMANAGER_H__783E5236_4E01_40AD_AB7C_EC70642F8BAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
using namespace std;
#define MAX_ORDER_COUNT			3
#define MAX_MOVEJOB_COUNT       10

#include "TransferArea.h"
#include "BGChecker.h"

class CArea;
class CTransferOrder;
class CFingerAction;

struct sTransferJob{
	 sTransferJob() {nSlot1 = 0;nSlot2=0;pArea = NULL;}
	~sTransferJob() {}
	
	 int           nSlot1;
	 int           nSlot2;
	 
	 CArea* pArea;
	 FingerACTType enFingerType;
};

struct sMovement{
	 sMovement() {SendJob=NULL;ReceiveJob=NULL;}
	~sMovement() {}
	 sTransferJob* SendJob;
	 sTransferJob* ReceiveJob;
};

//...2015.03.19
typedef struct _AutoParticleChk
{
	int nUPCCheck;
	int nJobReserv;
	int nLPMSlotSts1;
	int nLPMSlotSts2;
	int nLLSts;
	int nLLSlot1;
	int nLLSlot2;
	int nLLDoorCycNum;
	int nLLSlitCycNum;
	int nLLPumpCycNum;
	int nLLVentCycNum;
	int nTMRobotSelSts;
	int nTMRobotArmSts;
	int nTMSlitCycNum;
	int nPMSelSts;
	int nPMSlitCycNum;
	int nChuckCycNum;
	int nLoopCycNum;
	int nChuckSel;							
	double dbChuckUpPos;       		 
	double dbChuckDnPos;
} AutoParticleChk;
//...

class CTransferManager : public CBGChecker 
{
public:
	CTransferManager();
	virtual ~CTransferManager();

	
	enum{ T_CM1, T_CM2, T_CM3, T_CM4, T_PMA, T_PMB, T_PMC, T_PM4, T_PM5, T_PM6, T_PM7, T_PM8, 
		T_PM9, T_PM10, T_PM11, T_PM12, T_AL, T_IC, T_BM1, T_BM2, T_BM3, T_BM4, T_FAL};

	enum{R_TM_A, R_TM_B, R_TM_Auto, R_FM_A, R_FM_B, R_FM_ALL};
	//...2015.04.17
	enum {IO_UpArm, IO_LwArm, IO_AutoArm};
	enum {NotUse1, Para_UpArm, Para_LwArm, Para_AutoArm};
	enum {NotUse2, Para_LL1, Para_LL2};
	enum {NotUse3, Para_PMA, Para_PMB, Para_PMC};
	enum {IO_PMA, IO_PMB, IO_PMC, IO_BM1, IO_BM2};
	enum {OFF, ON};
	enum {AGV_MANUAL, AGV_AUTO};
	//...

	//... 2016.03.04
	enum { eUnknownDoor,	eUp,	eDown,	eErrorDoor };
	enum { eUnknownShutt,	eIn,	eOut,	eErrorhutt };
	//...

	BOOL InitConfigFile(const char* szFilePath);
	BOOL InitManager();
	
	//Actual
	Module_Status GetWafer();
	Module_Status PutWafer();
	Module_Status MoveWafer();
	Module_Status ClearWafer();

	//2007.12.06 due to dummy exchange
	Module_Status DummyAutoExchange();
	Module_Status DummySelectExchange();
	void          ChangeWaferState(int nSourceCM , int nSlot);

	// 2008.11.21 Dummy Select Slot Reset
	BOOL DummySelectSlotReset();
	BOOL DummyCMSelectSlotReset();

	//...2015.04.17
	Module_Status Auto_Particle_Check();
	//...

	//... 2016.03.03
	BOOL LPMErrorCheck(int i);
	//...

private:
	void   DoClearList      ();
	void   AddTransferOrder (CTransferOrder* pTransferOrder = NULL);
	BOOL   FindMoveStepInfo (char* szStartArea , char* szLastArea , int& nStartStep , int& nLastStep , CTransferOrder* pCurrentOrder);

	Module_Status DoMovement(int nMoveCount = 1);
	Module_Status FixedAlign(int nSlotA , int nSlotB);
	//2008.02.04
	BOOL   GetFromAligner(int nSelectedArm);
	BOOL   PutToAligner  (int nSelectedArm);
	//2008.02.04 End
	BOOL   FixedAlignerClear();
	BOOL   DoClearFromFM    (sArea* psArea , BOOL& bEnd , BOOL bSkipAligner = FALSE);
	BOOL   DoClearFromBM    (sArea* psArea , BOOL& bEnd);
	BOOL   DoClearFromTM    (sArea* psArea , BOOL& bEnd);
	BOOL   DoClearFromPM    (sArea* psArea , BOOL& bEnd);

	//utility
	int    GetWaferSourceStation(int nWaferSource);
	void   PopupMsgBox(const char* szAreaName , const char* szMsg1 , const char* szMsg2);

	//2007.12.06
	void   CheckSelectExchangeSlot();
	int    GetFirstCMSelectSlot();
	int    GetNextCMSelectSlot();
	int    GetFirstDummySelectSlot();
	int    GetNextDummySelectSlot();

	//2010.10.10
	void   SetDummyMode(BOOL bMode = TRUE);
	BOOL   GetDummyMode();
	BOOL   m_bDummyMode;

	//...2015.04.17
	void			IO_Reset();
	BOOL			LPM_Mode_Check(int nCheckStart, int nPCCheckLPM);
	BOOL			Read_Parameter();
	BOOL			Check_PMStatus();
	BOOL			Check_WaferStatus();
	Module_Status	ParticleTest_CM_BM(int i,int nStartModule ,int nEndModule);				
	Module_Status	ParticleTest_LL_Door_Control(int i, int nModule);
	Module_Status	ParticleTest_LL_Pump_Vent_Control(int i, int nModule);
	Module_Status	ParticleTest_SlitVlv_Control(int i, int nModule, BOOL bPMSide);								
	Module_Status	ParticleTest_TMPick(int i, int nModule);																					
	Module_Status	ParticleTest_TMPlace(int i, int nModule);											
	Module_Status	ParticleTest_PM_Chuck_Control(int i, int nModule);
	Module_Status   ParticleTest_TMRotate(int nCycleCount,int nModule);
	Module_Status   Over_Pressure_Control(int nCycleCount,int nIsControl);
	//...
	
private:
	CTransferArea*  m_pTransferArea;
	CFingerAction*  m_pFingerAction;
	CTransferOrder* m_pTransferOrder[3];
	CTransferOrder* m_pCurrentOrder;
	AutoParticleChk	m_pParticleChkJob[5];		//...2015.03.19
	int             m_nOrderCount;
	int             m_nAreaCount;
	int				m_nBMMode[MAX_BM_SLOT];
	sMovement       m_sMovementList[MAX_MOVEJOB_COUNT];

	//2007.12.06
	BOOL   m_bCMSelect[25];
	BOOL   m_bDummySelect[25];
	int    m_nCMSelectSlot;
	int    m_nDummySelectSlot;
};

#endif // !defined(AFX_TRANSFERMANAGER_H__783E5236_4E01_40AD_AB7C_EC70642F8BAC__INCLUDED_)
