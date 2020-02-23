//------------------------------------------------------------------------------------------

#ifndef __RECIPE_STRUCT_H__
#define __RECIPE_STRUCT_H__

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
// Recipe Structure Information
//

#define MAX_MFC		24
#define MAX_HEATER	64

//#define VR_MAX_STEP		10
#define VR_MAX_STEP		1

#define VR_STEP_SIZE	200
#define VR_HEADER_SIZE	100

enum RecipeTypeEnum
{
	RT_Unknown,
	RT_ProcRcp,
	RT_PreProRcp,
	RT_PostProRcp,
	RT_IdleRcp,
	RT_CleanRcp,
	RT_PurgeRcp,
	RT_LotPreRcp,
	RT_LotPostRcp
};

enum EnDisEnum
{
	ED_Disable, ED_Enable
};
//2014.04.24
enum WaferSourceEnum
{
	A,B,C,X,Y,DUMMY
};
enum TVPRS_ModeEnum
{
	TVM_POSITION,
	TVM_PRESSURE
};

enum MonEnDisEnum
{
	MED_MonDisable,						// Monitoring Disable
	MED_MonEnable						// Monitoring Enable
};
//2014.04.24
enum DummyWaferUseEnum
{
	DMYRcp_NotUse,						// Monitoring Disable
	DMYRcp_Use						// Monitoring Enable
};

enum MFC_OPT_Enum
{
	MFC_OPT_NO,
	MFC_OPT_WARNING,
	MFC_OPT_CHMDIS,
	MFC_OPT_ABORT,
	MFC_OPT_OPEN,

	// 2006.04.17
	MFC_OPT_U_WARNING,	
	MFC_OPT_U_CHMDIS,
	MFC_OPT_U_ABORT,

	// 2006.06.14
	MFC_OPT_RETRY,
};

enum CLEAN_CYC_TYPE
{
	TYPE_A = 0,
	TYPE_B
};

// 2014.05.22 
enum STEP_FUNC_Enum
{
	SF_NONE,
	SF_LEAK_CHECK,
	SF_REFILL_ALL,
};

// 2014.05.22
enum STEP_FLOW_Enum
{
	SFL_NORMAL_CYC,
	SFL_WAIT_STGTEMP,
	SFL_WAIT_IDLESTOP,
	SFL_WAIT_HTRTEMP,
	SFL_WAIT_STEPSYNC,
	SFL_WAIT_RPG_ON,		//... 2015.03.07 Add RPG ON Step Skip
	SFL_FUNCTION,
};

//... 2015.03.21 Add Position Move Option
enum PosMoveOptEnum
{
	ZPOSOPT_Wait,	// Step Start After Position Reach
	ZPOSOPT_NoWait	// Step Start After Position Set
};

typedef struct {
	char szRecipeName[40];				// Recipe Name
	int nRecipeType;					// Recipe Type
	int nTotalStep;						// Total Step

	// 2004.07.19 by cskim, 'Step Group Cycle' Added
	int nStepGrpBegin;
	int	nStepGrpEnd;
	int	nStepGrpCyc;
	BOOL bUseStepGrp;
	// 2004.11.18 by cskim, Idle Recipe Cycle
	int nIdleStopBegin;
	// 2005.05.30 by cskim, for Keep Last Con
	BOOL bKeepLastCon;
	// 2006.01.26
	double dbThickness;


} RECIPE_HEADER_STRUC;

typedef struct {
	char szStepName[40];				// Step Name
	BOOL bIsMain;						// Is Main Step ?

	//----------------------------------------------
	// 2004.11.02 by cskim
	BOOL bPreTreat;
	// 2004.11.08 by cskim
	BOOL bSubItemEnable;
	// 2004.11.09 by cskim, Step Flow
	int				nStepFlow_Option;
	int				nVRCPMode;
	// 2004.11.05 by cskim
	double			dbStgHtr1_SetPoint;
	double			dbStgHtr2_SetPoint;

	int				nStgHtr1_Option;
	int				nStgHtr2_Option;

	double			dbRamp1_SetPoint;
	double			dbRamp2_SetPoint;
	
	int				nRamp1_Option;
	int				nRamp2_Option;

	// 2004.11.11 by cskim
	double			dbStgHtr1_Tol;
	double			dbStgHtr2_Tol;

	double			dbStgHtr_Timeout;

	//
	double			dbRFLdPrst_SetPoint;
	int				nRFLdPrst_Option;
	double			dbRFTnPrst_SetPoint;
	int				nRFTnPrst_Option;
	
	//2008.09.29
	double			dbRF2LdPrst_SetPoint;
	int				nRF2LdPrst_Option;
	double			dbRF2TnPrst_SetPoint;
	int				nRF2TnPrst_Option;
	//----------------------------------------------

	//2014.05.22
	int nPrstMode_Option;
	int nPrst2Mode_Option;
	int nPrstSel_Option;
	int nPrst2Sel_Option;

	double pdbMFC_Sp[MAX_MFC];			// MFC Set Point <= // 2006.01.12
	double pdbMFC_Tol[MAX_MFC];			// MFC Tolerance
	int pnMFC_Option[MAX_MFC];			// Monitoring Enable/Disable(MonEnDisEnum)

	// 2006.01.16
	int nMFC_RampOpt;
	double			StgOffs_dbSp;
	double			StgOffs2_dbSp;

	int				StgOffs_nOpt;
	int				StgOffs2_nOpt;

	double dblCHMPRS_Pres;					// Process Pressure High Limit
	double dblCHMPRS_PresL;					// Process Pressure Low Limit
	// double dblCHMPRS_Tol;				// Chamber Pressure Tolerance
	int nCHMPRS_Option;						// NotMon/Mon Option(MonEnDisEnum)
	int nCHMPRS_Timeout;					// 2006.01.18

	double dblTVPRS_Sp;					// Pressure Control SetPoint
	// double dblTVPRS_Tol;					// Pressure Control Tolerance
	int nTVPRS_Option;					// Pressure Mode( TVPRS_ModeEnum )

	int nPositionX_Control;				// Position X Control 2014.07.02

	double dbSTGZPOS_Sp;				// Stage Heater Position SetPoint
	double dbSTGZPOS2_Sp;				// Stage Heater Position SetPoint
	
	int	nSTGZPOS_WaitOpt;				//... 2015.03.21 Add Position Move Option

	int nRF_FwSp;						// RF Forward SetPoint
	int nRF_Option;						// Monitoring Enable/Disable (MonEnDisEnum)

	int nRF2_FwSp;						// RF2 Forward SetPoint
	int nRF2_Option;					// Monitoring Enable/Disable (MonEnDisEnum)
	// 2005.03.07 by cskim
	double HTR_dbSp[MAX_HEATER];		// Heater SetPoint
	int HTR_nOpt[MAX_HEATER];			// Heater Option

	int nVR_TotalStep;					// Valve Recipe Total Step
	int nVR_CycleCount;					// Valve Recipe Cycle Count
	unsigned char ppVR_Step[VR_MAX_STEP][VR_STEP_SIZE];	// Valve Recipe Step Data
	unsigned char pVR_Header[VR_HEADER_SIZE];	// Valve Recipe Step Header Data
	int nVR_Enable;
	int nVR_Enable_DMY;					// 2014.04.24

	int nPreHeatTime;					// Pre Heating Time
	BOOL bValveCycleAccOption;			// Valve Cycle Accumulate Enable Option

	// 2004.07.19 by cskim, 'Step Group Cycle' Added
	int	nRoughValve;
	// 2005.05.31 by cskim, for Backside Ar Check
	int nBacksideArCheck;

	// 2006.01.24
	int StepTimeCompen_nOpt;
	int StepTimeCompen_nPrev;
	double StepTimeCompen_dbRate;
	double StepTimeCompen_dbMaxTime;		// 2006.05.12
	double StepTimeCompen_dbStdStepTime;	// 2006.05.12
	int  nCleanCycType;

	// 2006.06.14
	int nLoopCount;
	int nTotalLoopCycle;
	int nLoopStart;
	int nLoopEnd;
	// 2014.04.01
	double dbHte_SetPoint;
	int nHte_Option;

	// 2014.05.22
	int nSubFunc;

} RECIPE_STEP_STRUC;

// Recipe Map Information

#define RECIPE_MAP_FILE		".\\Parameter\\PM_SEQ_PARM\\RecipeConfigMap.ini"

typedef struct {
	int nAddr;
	int nBitNum;
} VRCP_MAP_STRUC;

//------------------------------------------------------------------------------------------
#endif