// TMSchStopCtrl.h: interface for the CTMSchStopCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMSchStopCtrl_H__9E829F8E_9C2C_43D8_9385_42C5DA1EC61E__INCLUDED_)
#define AFX_TMSchStopCtrl_H__9E829F8E_9C2C_43D8_9385_42C5DA1EC61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SeqInterface.h"
#define CONFIG_FILE_NAME_PM1   ".\\AutoPMDataFile\\AutoPMConfiguration_PM1.cfg"
#define CONFIG_FILE_NAME_PM2   ".\\AutoPMDataFile\\AutoPMConfiguration_PM2.cfg"
#define CONFIG_FILE_NAME_PM3   ".\\AutoPMDataFile\\AutoPMConfiguration_PM3.cfg" 
#define CONFIG_FILE_NAME_PM4   ".\\AutoPMDataFile\\AutoPMConfiguration_PM4.cfg"
#define CONFIG_FILE_NAME_PM5   ".\\AutoPMDataFile\\AutoPMConfiguration_PM5.cfg"

#define MAX_IO_LIST		22
#define PM_NUM 5
#define RECIPE_COUNT	6
enum { EN_PM1, EN_PM2, EN_PM3, EN_PM4, EN_PM5 };
/*
enum { UNKNOWN_TYPE,
	   PMJOB_CREAT,
	   PMJOB_START,	
       PREMACRO_S,
	   PREMACRO_REC_S,
	   PREMACRO_REC_E,	
	   PREMACRO_E,
	   PRESOMEJOB_S,
	   PRESOMEJOB_E,
	   
	   PHYSICAL_S,
	   PHYSICAL_E,
	   WAIT_POSTMACRO,
	   POSTMACRO_S,
	   POSTMACRO_REC_S,
	   POSTMACRO_REC_E,
	   POSTMACRO_E,
	   POSTSOMEJOB_S,
	   POSTSOMEJOB_E,
	   PMJOB_E,

	   PREMACRO_ABORT,
	   POSTMACRO_ABORT,
};
*/
enum { UNKNOWN_TYPE,
       PREMACRO_S,
	   PREMACRO_E,
	   
	   PHYSICAL_S,
	   PHYSICAL_E,
	   POSTMACRO_S,
	   POSTMACRO_E,
	   

	   PMJOB_CREAT,
	   PMJOB_START,
	   PMJOB_E,

	   WAIT_POSTMACRO,

	   PREMACRO_REC_S,
	   PREMACRO_REC_E,	
	   POSTMACRO_REC_S,
	   POSTMACRO_REC_E,

	   PRESOMEJOB_S,
	   PRESOMEJOB_E,
	   POSTSOMEJOB_S,
	   POSTSOMEJOB_E,
	   
	   PREMACRO_ABORT,
	   POSTMACRO_ABORT,

       M_PREMACRO_S,
	   M_POSTMACRO_E,
	   M_PREMACRO_ABORT,
	   M_POSTMACRO_ABORT,
};
enum { PRENONE,	PREIDLESTOP, PREIDLESTOP_CHK,
       PRE_RECCHK,PRE_RECRUN,PRERCP_STOPCHK,
	   PRE_REC2RUN,PRERCP2_STOPCHK, 
	   PRE_LKFUNC, PRE_LKEND, PRE_INTCHK,VENT, VENT_CHK,PRE_END };
			
enum { POSTNONE, VACCHKSTEP, POSTIDLESTOP,POSTIDLESTOP_CHK, 
       POST_LKFUNC, POST_LKEND, POST_MFCVERFI, POSTMFCVERCHK,
	   POST_RECCHK, POST_RECRUN, POSTRCP_STOPCHK,
	   POST_REC2RUN, POSTRCP2_STOPCHK,
	   POST_INTCHK, VP_LEAKCHK, POST_END, PMJOB_END };

enum {NONE_REV, PMJOB_REV};
enum {NotPM, WETPM, DRYCLEAN};
enum {ABSENT, PRESENT, UNKNOWN};
enum {CTRL_OFF, CTRL_ON};
enum {NOTUSE, USE};
enum {DryClean_0,WetPM_1};
typedef struct _PROFILE_LIST
{
	char szIORealname[256];
	int nIOType;
	int nIOPoint;
} PROFILE_LIST;

class CAutoPM  : public CSeqInterface
{
public:
	CAutoPM();
	virtual ~CAutoPM();

// Member Function
public:
	BOOL Initialize(const char *szModule = NULL);
	virtual Module_Status Main();
	void AutoPMEventMon(char *szCmd,char *szMacroType);

protected:	
	void ParseAutoPMConfig(char *szData);
	void ParseAutoPMConfig2(char *szData);
	void AutoPM_FlwSts(int nFlwSts);
	void AlarmMessagePost(int nAlarmID,int nMacroSts);
	void Report_Event(int nEventID);
	int Get_MacroType(char *pMacroType);
	int SeletPMMacro();
	double GetChamberPressure();
	Module_Status PMMonitor(int nAutoPMType);
	Module_Status AutoPMMacro(int nEventID);
	Module_Status PhysicalPM(int nEventID);
	Module_Status PreMacro(int nEventID);
	Module_Status PostMacro(int nEventID);
	Module_Status PLCIntOperation(int nValue);
	Module_Status Manual_START(int nEventID);
	Module_Status Manual_END(int nEventID);
	Module_Status Manual_TempDown_Rec();
	Module_Status PMStsCheck();
	Module_Status Manaul_Abort();
// For Thread
protected:
	virtual void DoMonitor();		

// Member Variable
protected:
	typedef struct _AUTOPM_CONFIG_FILE
	{
		BOOL bPMJobStartRCMD;
		BOOL bPMJobAbortRCMD;
		BOOL bPMJobStopRCMD;
		BOOL bPMJobStart;
		BOOL bPreMacroAbort;
		BOOL bPostMacroAbort;

		BOOL bAutoPMUse;
		BOOL bPreMacroUse;
		BOOL bPostMacroUse;
		BOOL bPreRecFstUse;
		BOOL bPreRecSndUse;
		BOOL bPostRecFstUse;
		BOOL bPostRecSndUse;
		BOOL bPreSomeJobUse;
		BOOL bPostSomeJobUse;
		BOOL bPhysicalPMUse;
		BOOL bPLCIntUse;
		BOOL bAutoVentUse;
		BOOL bPreRecipeStepUse;
		BOOL bPostRecipeStepUse;
		BOOL bPrePMSuccess;
        BOOL bPostPMSuccess;
		BOOL bDryCntResetUse;
		BOOL bWetCntResetUse;
		BOOL bPostMacroAutoStart;
		BOOL bAutoEnable;
		int nStartDelayTime;
	} AUTOPM_CONFIG_FILE;
	
	AUTOPM_CONFIG_FILE m_AutoPMCfgFile[PM_NUM];
	AUTOPM_CONFIG_FILE m_AutoPMCfgFile2[PM_NUM];

	char m_szModuleName[8];
	int m_nModuleIndex;
	int m_nAlarmIdx;
	int m_nIOCount;
	char m_szFilePath[256];
	int m_nMacroID;
	char m_szCmd[256];
	int m_nFlgAutoPMStop;
	int m_nFlgAutoPMAbort;
	int m_nTimeCount;
	int m_nReserveStatus;
	PROFILE_LIST* m_IOList[MAX_IO_LIST];

};

#endif // !defined(AFX_TMSchStopCtrl_H__9E829F8E_9C2C_43D8_9385_42C5DA1EC61E__INCLUDED_)
