// ProcessSequence.h: interface for the CProcessSequence class.
// For 48 Channel Valve Control
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSSEQUENCE_H__BAC77401_9D3B_46F5_83DE_14127EEC5141__INCLUDED_)
#define AFX_PROCESSSEQUENCE_H__BAC77401_9D3B_46F5_83DE_14127EEC5141__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Class Definition

class CProcessSequence  
{
public:
	CProcessSequence();
	virtual ~CProcessSequence();
	Module_Status Main();
	Module_Status Main_Process();
	Module_Status Main_LeakCheck();
	virtual void Initialize();

// Member Function

protected:
	void Init_Process();
	virtual BOOL ReadyProcess();
	virtual void AbortAllFunc();
	void StopProcess();		// 2006.06.14
	void RunThreadFunc(LPTHREAD_START_ROUTINE threadFunc, LPVOID lParam);

	//--------------------------------------------------------------------
	// 2004.08.02 by cskim, 'Preventive Maintance Sequence' Added
	BOOL CheckFeedingValve(int nVlvN, unsigned char *pData);
	virtual double GetSourceFeeding(int nStep);
	double GetValveCycleAcc(int nStep);
	void AccPrvMaintItem(int nStep, int nStepCycle);

	//--------------------------------------------------------------------
	// 2004.09.13 by cskim, Total Time of Process
	virtual double GetProcStepTime(int nStep);
	virtual double GetProcStepTimeForPLC(int nStep);
	double GetNormalProcTime();
	double GetGrpCycProcTime();
	double GetProcTotalTime();

	//--------------------------------------------------------------------
	// Sub Functions
	void InitRecipe();
	void SetRecipeInform();
	void LogRecipeInform();
	void DisplayStepInfo(int nStep);
	void AddString(char *szSrc, const char *szDest);
	Module_Status CVGSafetyWait();
	void SetACVMode(int nRcpType);
	BOOL CheckACVStatus();
	BOOL CheckPLCGlobalInterlock();
	BOOL CheckSWInterlock();

	//--------------------------------------------------------------------
	// Write valve recipe to driver through shared memory
	// 2004.09.10 by cskim
	#define	S_MEMORY_SIZE	2000
	#define SM_FILE_NAME	"VRCP_DATA_FILE"
	char m_szErrDesc[256];
	BOOL WriteVRCP2Driver(int nStep);

	//--------------------------------------------------------------------
	// Valve Recipe Download Function
	// 2004.09.10 by cskim
	int m_nDL_Thread;
	int m_nQueuedDnLdStep;
	enum { VRM_CONT, VRM_PAUSE, VRM_NOT_DEF, VRM_NO_VRCP };
	int m_nVRCPBufPage;

	// 2013.06.25
	#define VRCP_BUFF_PAGE_MAX		5
	int m_nVRCPBufPageMax;		
	int m_anVRCcpBufPageAddr[VRCP_BUFF_PAGE_MAX];
	int m_nContDL_Thread;
	CRITICAL_SECTION m_csQueueLock;
	BOOL Start_ContDownloadValveRecipe();
	BOOL ContDownloadValveRecipe();
	static DWORD WINAPI ContDL_ThreadProc(LPVOID lParam);

	BOOL DownloadValveRecipe(int nStep, const unsigned char *pData, int nSize);
	//2014.08.25
	BOOL DownloadValveRecipe_DNET(int nStep, const unsigned char *pData, int nSize);
	BOOL DownloadValveRecipe_EIP(int nStep, const unsigned char *pData, int nSize);
	//
	BOOL DownloadValveRecipe_Sim(int nStep, const unsigned char *pData, int nSize);
	BOOL DownloadProcStepVRCP(int nStep, BOOL bRetry = FALSE);
	static DWORD WINAPI DL_ThreadProc(LPVOID lParam);

	BOOL Start_DownloadThread(int nStep);

	//--------------------------------------------------------------------
	// Throttle Valve Setting Function and Thread
	// 2004.09.10 by cskim
	int m_nTV_Thread;
	int m_nTV_Step;
	void SetTV();
	static DWORD WINAPI TV_ThreadProc(LPVOID lParam);
	void StartTVSetting(int nStep);
	void StartTVMonitor(int nStep);

	//--------------------------------------------------------------------
	// Z-Motion Setting Function
	BOOL SetStagePositionEx(double dbPos, double dbPos2);

	//... 2015.03.21 Add Position Move Option
	BOOL SetStagePositionNoWait(double dbPos, double dbPos2);
	//---------------------------------------------------------------------------------------
	// Stage Heater Setting
	// 2004.11.17 by cskim
	int m_nStgHtr_Thread;
	int m_nStgHtr2_Thread;
	int m_nStgHtr_Step;
	int m_nStgHtr2_Step;

	void SetStgHtr();
	void SetStgHtr2();
	static DWORD WINAPI StgHtr_ThreadProc(LPVOID lParam);
	static DWORD WINAPI StgHtr2_ThreadProc(LPVOID lParam);
	void StartStgHtrSetting(int nStep);
	void StartStgHtr2Setting(int nStep);

	//---------------------------------------------------------------------------------------
	// Heater Setting
	// 2005.03.11 by cskim
	void StartHeaterSetting(int nStep);

	//---------------------------------------------------------------------------------------
	// Mfc Setting Function and Thread
	// 2004.09.10 by cskim
	BOOL m_bMfcMonitor;
	BOOL m_bMfcSetEnable;
	BOOL m_bMfcMonEnable;
	int m_nMFC_Thread;
	int m_nMFC_Step;
	void SetMFC();
	static DWORD WINAPI MFC_ThreadProc(LPVOID lParam);
	void StartMFCSetting(int nStep, BOOL bSetEnable, BOOL MonEnable);

	//---------------------------------------------------------------------------------------
	// Timer Thread
	// 2005.04.18 by cskim

	int m_nTmr_Thread;
	static DWORD WINAPI Tmr_ThreadProc(LPVOID lParam);
	void InitStepTimer();
	void StopStepTimer();
	void UpdateStepTime();
	void UpdateStepTotalTime(int nStep);

	//---------------------------------------------------------------------------------------
	// Data Log Start Thread
	// 2005.07.14, Data Log Timming
	static DWORD WINAPI DCOPLog_ThreadProc(LPVOID lParam);
	void StartDCOPLog();
	void DoDCOPLog();

	//---------------------------------------------------------------------------------------
	// Process Monitoring
	BOOL CheckMonitoringStatus();
	BOOL CheckSettingThread();

	//---------------------------------------------------------------------------------------
	// Sub Functions 2
	BOOL ReadyPLC();

	//---------------------------------------------------------------------------------------
	// Process Step Queue
	// 2004.09.21 by cskim
	typedef struct
	{
		int nStep;
		int nCmd;
		BOOL bDownload;
	} PROC_QUEUE;
	enum { PCMD_NOTHING, PCMD_STEP_GRP_INC, PCMD_CONT, PCMD_RESTART };

	// 2013.06.25
	#define MAX_QUEUE	8
	PROC_QUEUE m_ProcQ[MAX_QUEUE];
	// 0: Queueing New Step
	// 1: Ready(5) : Download Step when 5 page
	// 2: Ready(4)
	// 3: Ready(3)
	// 4: Ready(2)
	// 5: Ready(1) : Download Step when 2 page
	// 6: Running Step
	// 7: Finished Step
	enum { QSTEP_NEW, QSTEP_RD4, QSTEP_RD3, QSTEP_RD2, QSTEP_RD1, QSTEP_RUN, QSTEP_END };
	int m_nQStepDownload;
	int m_nQStepPreDownload;
	BOOL m_bFirstStepDownload;

	void Init_ProcStepQueue();
	BOOL Start_DownloadValveRecipe();
	virtual BOOL Preset_Controller();
	virtual BOOL Set_Controller();
	BOOL Start_ProcessStep();
	BOOL Wait_ProcStepStarted();
	BOOL Wait_ProcStepFinished();
	virtual BOOL Wait_ConditionIsOK();

	void Set_PrvMaintItem();
	Module_Status Queue_ProcStep(int nStep, int nCmd);
	// 2006.06.14
	void Queue_BackShift();
	void Push_ProcStep(int nStep, int nCmd);
	Module_Status Exec_ProcStep();
	// 2013.06.25
	BOOL IsQueueEmpty();

	BOOL IsProcStepFinished(BOOL *pbStatus);	// 2006.07.07
	BOOL IsProcStepStarted();
	BOOL IsStgHtrCondiOK(int nStep);
	BOOL IsIdleStop();
	BOOL IsStgHtrTempOK();
	BOOL IsHeaterCondiOK(int nStep);
	// 2005.05.30 by cskim, for Keep Last Condition
	void KeepLastCon(int nStep);

	//---------------------------------------------------------------------------------------
	// Run Process Sub Functions

	//------------------------------------------
	// 2006.06.14
	void InitStepGroupRange();
	virtual BOOL PauseProcess();
	virtual BOOL ResumeProcess(int nAlarmRecovery);
	void BackupCurrentProcess();
	void TrimProcessTime();
	void RecoverProcessTime();
	void IncreaseGrpCyc();

	RECIPE_STEP_STRUC	m_Backup_RcpStep;
	RECIPE_STEP_STRUC	m_Remake_RcpStep;
	int	m_Backup_nStepNum;
	int m_Backup_nCurCycle;
	double	m_Backup_dbProgressTime;
	double	m_Backup_dbRemainTime;
	//------------------------------------------

	//... 2015.02.26 RPG Alarm Auto Retry (Only Lot Pre & Dry Clean)
	//.. RPGAlmPrcRetryDM Enum (Auto Retry Flag)
	enum { eRPGALM_NONE, eRPGALM_REFLECT, eRPGALM_IGNITION };
	//... 2016.01.19
	enum { eFlow1, eFlow2 };
	enum { OFF, ON };

	int	m_nRPGPrcRetryCnt;
	void InitRPGAlarmAutoRetry();
	int	 CheckRPGAlarmAutoRetry();
	

	// 2004.07.19 by cskim, 'Step Group Cycle' Added
	int m_nStpGrpCCyc;
	Module_Status RunNormalProcess();
	// 2004.07.19 by cskim, 'Step Group Cycle' Added
	Module_Status RunGrpCycProcess();

	// 2004.11.18 by cskim, Idle Recipe Cycle
	BOOL m_bIdleStopEnable;
	Module_Status RunIdleCycProcess();

	// 2004.11.16 by cskim, for Pre-treatment
	BOOL IsProcStepValid(int nStep);
	BOOL IsPreTreatEnabled(int nStep);

	//---------------------------------------------------------------------------------------
	// Run Process Main Functions
	Module_Status RunProcess();
	Module_Status RunProcessOut();
	Module_Status RunPreProcess();
	Module_Status RunPostProcess();
	Module_Status RunIdleRcp();
	Module_Status RunCleanRcp();
	Module_Status RunPurgeRcp();
	Module_Status RunLotPreRcp();
	Module_Status RunLotPostRcp();

	// 2007.01.31
	double GetStgHtrTempSet();
	// 2005.05.05 by cskim, Auto Refill
	Module_Status DoRefill();
	// 2014.10.30
	Module_Status DoALDLeakChk();
	//... 2015.09.17
	Module_Status DoLeakChk(char *szCmd);
	//... 2016.01.19
	Module_Status DoLineLeakChk();

	// 2004.11.16 by cskim, for Pre-treatment
	BOOL IsNeededPreProc();

	//... 2014.09.13 by wjkim, Add Recheck Wafer Absent Status
	BOOL IsWaferAbsent();

	//... 2015.01.02 by wjkim, Add Lot Pre Skip Case
	BOOL IsLotPreSkipCase();
	//... 2015.01.19
	Module_Status LeakCheckManager(char *pszFlow);
	BOOL IsPurgeStop();
	//---------------------------------------------------------------------------------------
	// MFC Mapping Data Reading
	void ParseMFCMap(char *szMapData);
	void ParseRFMap(char *szMapData);
	void ParseHTEMap(char *szMapData);    // 2014.05.22
	void ParseValveRecipeMap(char *szMapData);
	void InitRcpMapData();
	void LogRcpMapData();
	BOOL ReadRcpMapData();
	
	//---------------------------------------------------------------------------------------
	//	Recipe Reading and Parsing

	#define _IF_STR(ITEM_NAME, ITEM_VALUE) if(strcmp(szItemName, ITEM_NAME) == 0) \
		strncpy(ITEM_VALUE, szValue, 255);
	#define _IF_INT(ITEM_NAME, ITEM_VALUE) if(strcmp(szItemName, ITEM_NAME) == 0) \
		{ if(INT_NUM_CHECK(szValue, 0, 0, &nValue)) ITEM_VALUE = nValue; }
	#define _IF_DBL_INT(ITEM_NAME, ITEM_VALUE) if(strcmp(szItemName, ITEM_NAME) == 0) \
		{ if(DB_NUM_CHECK(szValue, &dblValue)) ITEM_VALUE = (int) dblValue; }
	#define _ELSE_IF_STR(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
		strncpy(ITEM_VALUE, szValue, 255);
	#define _ELSE_IF_INT(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
		{ if(INT_NUM_CHECK(szValue, 0, 0, &nValue)) ITEM_VALUE = nValue; }
	#define _ELSE_IF_DBL(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
		{ if(DB_NUM_CHECK(szValue, &dblValue)) ITEM_VALUE = dblValue; }
	#define _ELSE_IF_DBL_INT(ITEM_NAME, ITEM_VALUE) else if(strcmp(szItemName, ITEM_NAME) == 0) \
		{ if(DB_NUM_CHECK(szValue, &dblValue)) ITEM_VALUE = (int) dblValue; }
	#define _ELSE		else
	#define _IF			if
	#define _ELSE_IF	else if

	virtual BOOL ParseValveRecipe(int nStep, const char* szItem, char* szValue);
	BOOL ParseMFC(int nStep, const char* szItem, char* szValue);
	BOOL ParseHeater(int nStep, const char* szItem, char* szValue);
	BOOL ParseRecipeStepItem(int nStep, const char* szItem, char* szValue);
	Module_Status Recipe_Reading(BOOL *appenddata); 
	BOOL CheckRecipeData();
	
	//---------------------------------------------------------------------------------------
	// 2004.09.24
	// Valve Recipe Step Header Packet
	virtual void MakeVRCPStepHeader();

	//--------------------------------------------------------------------
	// 2006.01.24
	BOOL CalcStepTimeCompensation();
	BOOL CalcStepTimeCompensation2();
	//2007.12.06
	BOOL CalcStepCycleCompensation();
	BOOL m_bStepTimeCompenMaxAlarm;				// 2007.01.19
	int  m_nStepTimeCompenMode;					// 2007.02.02
	enum { STCM_Normal, STCM_StepCycleInc };

	//---------------------------------------------------------------------------------------
	// 2004.11.25
	// PMC Configration Function
	BOOL ReadPmcConfig();
	void ParseSrcFeedMfc(char *szData);
	void ParseVrcpConfig(char *szData);
	void ParseZMConfig(char *szData);
	void ParseProcessConfig(char *szData);
	//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
	void ParseDoubleOpenIntlkConfig(char *szData);
	//... 2017.12.13
	void ParseALDVlvConfig(char *szData);
	void GetALDVlvOnCount(int nStep);
	//---------------------------------------------------------------------------------------
	// 2005.02.18
	int GetMainStepNumber(int nStep);

	#define MAX_MAIN_STEP		5
	int m_pnMainStep[MAX_MAIN_STEP];
	double m_pdbMainStepTime[MAX_MAIN_STEP];
	int m_nMainStepCount;

	//--------------------------------------------------------------------
	// 2007.01.25
	// 2007.02.09
	void CheckPostRunReservation();
	BOOL CheckPostRunStart();			
	void SetPostRunCompleted();
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// 2014.05.22
	virtual Module_Status GetSubFuncStatus(int nStep);
	virtual void StopSubFunction(int nStep = 0);
	virtual BOOL RunSubFunction(int nStep);
	//--------------------------------------------------------------------

	//... 2014.09.29 wjkim, RF Count Monitor
	void CheckRFPostCount(int nStep);
	BOOL GetStepRFOnCount(int nStep, int *pnRF1Count, int *pnRF2Count);
	int m_nRF1_OnCountBitNum;
	int m_nRF2_OnCountBitNum;

	int m_nFunctionSetCnt;

	void SetSwapPurge();     //... 2015.09.28

	BOOL ZmotionCtrlBaseOnParameter();

// Member Variable
public:
	BOOL m_DummyWafer;		//2014.04.23
	char m_szEventID[20];	//2015.06.26
	int m_nProcessType;      //... 2016.04.01
	enum {eUnknownType, eProcess_In, eProcess_Out, eM_Process};

protected:
	class CTimer *m_pStepTimer;
	// 2007.01.31
	BOOL m_bLotPreAutoDisable;
	// 2007.02.09 by cskim, 0:Single, 1:Multi
	int m_nOnlineAutoPurgeType;
	enum { ONLINE_PURGE_SINGLE, ONLINE_PURGE_MULTI };
	// 2007.01.25
	#define POST_RUN_RECIPE_MAX		10
	enum { POST_RUN_OFF, POST_RUN_RESERVE, POST_RUN_START	};
	int m_nRecipeIndex;

	enum { SM_NOT_SIM, SM_SIMUL, SM_PLC_TEST };
	int m_nSimMode;

	int m_nRF_UseDM;
	int m_nHTE_UseDM; //2014.04.01
	// m_nVRCP_DnMethodDM => 0:Channel 1:SharedMemory 2:Enther Net
	int m_nVRCP_DnMethodDM;	// 2004.09.21, VRCP Download Method
	int m_nVRCP_DnLdWait;	// 0:NotUse 1:Use
	int m_nProMfcSetCtrl;
	int m_nProRfSetCtrl;
	int m_nProHteSetCtrl;	// 2014.05.22
	int m_nCVGIsoUseDM;		// CVGIsoUseDM => 0:NotUse 1:Use

	// Recipe Infromation
	#define		MAX_RECIPE_STEP			60		// 2004.07.06 by cskim : MaxStep is 60
	#define		MAX_RECIPENAME_LEN		255

	BOOL m_bRcpInit;
	RECIPE_HEADER_STRUC m_RcpHeader;
	RECIPE_STEP_STRUC m_RcpStep[MAX_RECIPE_STEP];

	int m_nLastStep;
	int m_nPreTreatEnable;	// m_nPreTreatEnable => 0:Disable 1:Enable

	char	RECIPE_NAME[ MAX_RECIPENAME_LEN + 1 ];
	int		STEP_COUNT;
	int		WAFER_INFORMATION;
	int		T_TOTAL_TIME;
	int		T_CURRENT_TIME;

	// Recipe Map Information
	#define	MAX_VALVE_SIZE				112

	int					m_nMaxValve;
	int					m_nMaxMfc;
	int					m_pnMfcMap[MAX_MFC];
	VRCP_MAP_STRUC		m_VRcpMap[MAX_VALVE_SIZE];
	BOOL				m_bRcpMapRead;

	CRITICAL_SECTION m_csLock;

	// Mfc Scale Information
	int m_pnMFC_Scale[MAX_MFC];
	int m_pnMFC_PhyMax[MAX_MFC];

	// RF Scale Information
	int m_nRF_Scale;
	int m_nRF_PhyMax;

	// Heat Exchanger 2014.05.22
	int m_nHTE_Scale;
	int m_nHTE_PhyMax;

	// Progressing Step Count
	int m_nDnLdStepCount;
	int m_nDnRdyStepCount;
	int m_nRunStepCount;

	// 2005.05.30 by cskim, for Keep Last Condition
	char m_szLastValveStatus[256];

	//---------------------------------------------------------------------------------------
	// 2004.11.25
	// PMC Configration Data
	#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\PMCConfig.ini"
	typedef struct _SRC_FEED_MFC
	{
		int nMfcN;
		int nValveN1;
		int nValveN2;
		int nValveN3;
		float fDrift;
	} SRC_FEED_MFC;

	SRC_FEED_MFC	m_SrcFeedMfc[5];
	
	double m_dbVlvCycSum;
	double m_dbSrcFeedingSum;
	enum { VTS_10MS, VTS_100MS };
	int m_nValveTimeRes;
	// Z_Motion
	BOOL m_bZM_PreLoadPos;
	// Process
	BOOL m_bAbortNotif;
	BOOL m_bProcFailInterlock;
	BOOL m_bAbortGasBeforeProc;		// 2005.06.10 by cskim, Abort Gas Before Process
	BOOL m_bFastRoughBeforeProc;
	BOOL m_bFastRoughAfterProc;
	BOOL m_bPostSkipEvent;
	int m_nStepTimeMode;			// 0:TotalTime 1:StepTime 2:StepTime*2 
	int m_nLotPreFailOption;		// 0:LotAbort 1:LotContinuous
	// 2006.02.13
	int m_nPrevAmountOption;		// 0:AllRecipe 1:OnlyProcessRecipe

	// 2005.07.22, for Step Time Interlock
	double m_dbStepTimeTol;		// Step Time Tolerance
	// 2006.07.20 by cskim
	double m_dbStepTimeTolRate;

	int m_Clean_nACVControl;
	BOOL m_Clean_bACVCheck;

	// 2006.09.05
	int m_nMfcControlMode;
	enum { MFCCM_PARAM_TOL, MFCCM_RECIPE_TOL };

	// 2013.06.02
	BOOL m_bStablePurgeAfterProc;

	//... 2014.10.22 Dry Clean Chamber Status
	enum { DRYCLN_OFF, DRYCLN_RESERVE, DRYCLN_RUN, DRYCLN_FAILED };
	enum { RUNSTS_Stop, RUNSTS_Running };

	//... 2015.01.02 by wjkim, Add Lot Pre Skip Case
	char m_szOldLotPreRcpName[40];

	//... 2015.01.06 by wjkim, Add Process Stop Channel
	enum { enOFFON_OFF, enOFFON_ON };

	//... 2015.05.30 Add CTC Download Recipe Valve Verification by Valve Interlock
	#define MAX_INTLK_VALVE_LIST		300
	typedef struct _INTLK_VLV_LIST
	{
		int nValveNo_A;
		int nValveNo_B;
	} INTLK_VLV_LIST;	
	INTLK_VLV_LIST	m_DBOpenIntlkVlv[MAX_INTLK_VALVE_LIST];

	BOOL m_bDBOpenIntlkVlv;

	//... 2017.03.07
	int m_nZCtrl_RCPType;
	int m_nZCtrl_PrcStartDelayTime;
	int m_nZCtrl_RCPSkipOption;

	//... 2017.12.13
	#define MAX_ALD_VALVE_LIST  13
	typedef struct _ALD_VLV_LIST
	{
		int nALDOpenCnt;
		int nIOEnum;
		int nVlvNum;
		BOOL bALDVlvSumReady;
		int nStepFinalVlvSts;
		int szPrmIO;
		int nRealVlvNum;
	} ALD_VLV_LIST;	

	ALD_VLV_LIST m_ALDVlvList[MAX_ALD_VALVE_LIST];

protected:
	bool m_bProcTimeInterlock;

	// 2006.05.17
	int m_nVR_HDR_BASE;
	int m_nVRHD_MFC_SP_IDX;
	int m_nVRHD_MFC_VL_IDX;
	int m_nVRHD_RF_SP_IDX;
	//2008.09.29
	int m_nVRHD_RF2_SP_IDX;
	int m_nVRHD_HTE_SP_IDX;    //2014.05.22
	int m_nBASE_IDX;
	int m_nLINE_STEP_BASE;
	int m_nLINE_STEP_SIZE;
protected:
	//--------------------------------------------------------------------
	// 2005.11.19, Dynamic IO Class
	class CDynamicIO *m_pDynIO;
	//--------------------------------------------------------------------
	// Dynamic IO Enum List Definition
	static enum {
		Fixed_IO_End,
		//-----------------Fixed I/O End -------------------
		// 2007.01.31
		PRO_LotPreCtrlDM,
		
		//-------------------------
		// 2007.01.25
		PRO_PostRunDM,
		PRO_PostRunDM1,
		PRO_PostRunDM2,
		PRO_PostRunDM3,
		PRO_PostRunDM4,
		PRO_PostRunDM5,
		PRO_PostRunDM6,
		PRO_PostRunDM7,
		PRO_PostRunDM8,
		PRO_PostRunDM9,

		PRO_PostCmptDM,
		PRO_PostCmptDM1,
		PRO_PostCmptDM2,
		PRO_PostCmptDM3,
		PRO_PostCmptDM4,
		PRO_PostCmptDM5,
		PRO_PostCmptDM6,
		PRO_PostCmptDM7,
		PRO_PostCmptDM8,
		PRO_PostCmptDM9,
		//-------------------------
		//2007.12.06
		PRO_ChamberStatus,
		

		PRMD_Refill_Opt,
		// 2005.10.19, Process Step Sync between Modules
		PRO_PMStepSyncDM,
		PRO_PMStepWaitDM,
		// 2006.05.12
		PRO_StdStpTmAM,

		// 2006.01.24
		PRVD_DRYCWfrCnt,
		PRVA_DRYCSrcFeed,
		PRVA_DRYCThick,
		// 2006.01.26
		PRO_PrvThickAM,
		// 2006.06.14
		PRO_FlowControlDM,
		CTC_CommStatus,
	} IO_INDEX_ENUM;

};

#endif // !defined(AFX_PROCESSSEQUENCE_H__BAC77401_9D3B_46F5_83DE_14127EEC5141__INCLUDED_)

