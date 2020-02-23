#ifndef LOTTYPE_H
#define LOTTYPE_H

#define		RECIPE_ADDR	void *

#define		MAX_STEP_COUNT			122 //2005.10.28 by cskim, 122 Step Possible
#define		MAX_DISPLAY_COUNT		2
#define		MAX_MFC_NO				20
#define		MAX_MFC_DISPLAY			16

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum STEP_FLOW_Enum
{
	SFL_NORMAL_CYC,
	SFL_WAIT_STGTEMP,
	SFL_WAIT_IDLESTOP,
	SFL_WAIT_HTRTEMP,
	SFL_WAIT_STEPSYNC,
	SFL_WAIT_RPG_ON,
	SFL_WAIT_FUNC,
};

// 2009.11.25
enum STEP_FUNC_Enum
{
	SF_NONE,
	SF_LEAK_CHECK,
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char szMFC_Descr[20][50];

typedef struct 
{
	RCP_TYPE		udRcpType;

	// 2004.07.14 by cskim, 'Step Group Cycle' Added
	int				nStepGrpBegin;
	int				nStepGrpEnd;
	int				nStepGrpCyc;
	int				nIdleStopBegin;
	// 2005.05.24 by cskim, for Keep Last Condition
	BOOL			bKeepLastCon;
	double			dbThickness;				// 2006.01.25
	// 2013.04.02 Recipe Password Added by hyuk 
	char			szRecipePW[256];

} RecipeStepTemplate;

typedef struct 
{
	char			szStepName[30];
	int				bMainRcp;

	//--------------------------------------------------
	// 2004.11.02 by cskim
	int				bPreTreat;
	// 2004.11.08 by cskim
	int				bSubItemEnable;

	int				bSubItem2Enable;		// 2006.09.04

	// 2004.11.09 by cskim, Step Flow
	int				nStepFlow_Option;
	// 2004.11.05 by cskim
	double			dbStgHtr1_SetPoint;
	double			dbStgHtr2_SetPoint;
	int				nStgHtr1_Option;
	int				nStgHtr2_Option;
	double			dbRamp1_SetPoint;
	double			dbRamp2_SetPoint;
	int				nRamp1_Option;
	int				nRamp2_Option;

	double			dbStgHtr_Timeout;		// minute
	// 2005.12.26
	double			StgOffs1_dbSp;
	double			StgOffs2_dbSp;

	int				StgOffs1_nOpt;
	int				StgOffs2_nOpt;
	//--------------------------------------------------

	//--------------------------------------------------
	// 2005.12.28
	int				StepTimeCompen_nOpt;
	double			StepTimeCompen_dbRate;
	int				StepTimeCompen_nPrev;
	int				nMfcRampOpt;
	//--------------------------------------------------
	double			StepTimeCompen_dbMaxTime;	// 2006.05.12
	double			StepCycleCompen_dbMaxCycle;	// 2009.02.02
	//--------------------------------------------------

	double			dbChamber_Pressure_Set_Point;
	double			dbChamber_Pressure_Set_Point_L;
	NOYES			udChamber_Pressure_Option;
	int				nChamber_Pressure_Timeout;

	double			dbThrottle_Valve_Set_Point;
	int				udThrottle_Valve_Option;

	double			dbRF_PWR_Set_Point;
	NOYES			udRF_PWR_Option;
	//2014.03.31
	double			dbRF2_PWR_Set_Point;
	NOYES			udRF2_PWR_Option;

	double			dbSTAGE_POS_Set_Point;
	double			dbSTAGE2_POS_Set_Point;
	int				nSTAGE_POS_WaitOpt;			//... 2015.03.21 Add Position Move Option

	double			dbRFLdPrst_SetPoint;
	int				nRFLdPrst_Option;
	double			dbRFTnPrst_SetPoint;
	int				nRFTnPrst_Option;
	// 2014.05.22
	int				nPrstMode_Option;
	int				nPrst2Mode_Option;
	int				nPrstSelMode_Option;
	int				nPrst2SelMode_Option;
	//
	//2014.03.31
	double			dbRF2LdPrst_SetPoint;
	int				nRF2LdPrst_Option;
	double			dbRF2TnPrst_SetPoint;
	int				nRF2TnPrst_Option;
	//2014.04.03
	double			dbHTE_SetPoint;
	int				nHTE_Option;

	char			szMFC_RealName[MAX_MFC_NO][50];
	double			dbMFC_Set_Point[MAX_MFC_NO];
	double			dbMFC_Tolerance[MAX_MFC_NO];	// 2006.09.04
	int				dMFC_Option[MAX_MFC_NO];

	NOYES			udVALV_CYC_PLUS_OPT;
	int				dValv_Rcp_Cycle_Cnt;
	BOOL			bHasValveData;
	VALVE_RCP_DATA	udRCP_Valve_Data;

	// 2004.07.14 by cskim, 'Step Group Cycle' Added
	int				nRoughValve;

	// 2005.03.03 by cskim
	double			Htr_pdbSetPoint[MAX_HEATER_NO];
	int				Htr_pnOption[MAX_HEATER_NO];

	//--------------------------------------------------
	// 2005.05.09 by cskim, for CVD Editor
	double			dbCVDStepTime;
	// These item are not saved.
	int				pnVrcpType[MAX_VALVE_SEQ];
	int				pnMfcVlvSeq[MAX_MFC_NO];
	// 2005.05.24 by cskim, for Backside Helium Check
	BOOL			nBacksideArCheck;
	//--------------------------------------------------

	// Added by jhlee. 2009.11.25. for ROR Check
	int				nSubFunc;
	
	// 2009.07.31 added by jhlee. for Dummy Recipe
	char			szMFC_RealName_DMY[MAX_MFC_NO][50];
	double			dbMFC_Set_Point_DMY[MAX_MFC_NO];
	double			dbMFC_Tolerance_DMY[MAX_MFC_NO];
	int				dMFC_Option_DMY[MAX_MFC_NO];
	//-----------------------------------------------------

	// 2009.07.31 added by jhlee. for Dummy Recipe
	BOOL			bHasValveData_DMY;
	VALVE_RCP_DATA	udRCP_Valve_Data_DMY;
	//-----------------------------------------------------

	// Dummy Recipe Screen에서 Main Recipe를 copy 하였는지 확인하는 Flag. 2009.08.21
//	BOOL			bDMYRecipCopy;

} RecipeStepTemplateM;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
