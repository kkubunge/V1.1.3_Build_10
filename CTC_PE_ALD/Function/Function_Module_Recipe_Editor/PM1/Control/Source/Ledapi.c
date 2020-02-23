#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

#include <windows.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutlanal.h>
#include <KutlFile.h>
#include <Kutltime.h>
#include <Kutlgui.h>

#include "iodefine.h"
#include "Prg_Param.h"
#include "Data_Type.h"
#include "Lottype.h"
#include "CVD_Rcp.h"
#include "SEED_KISA.h"

extern RCP_TYPE		g_RcpFileType;
extern BOOL			D_HAS_CONTROL;
extern int			g_Total_Valve_No;
extern BOOL			g_bSubItemEdit;
extern BOOL			g_bSubItem2Edit;	// 2006.09.04

extern int g_nStgHtrPos_Min;
extern int g_nStgHtrPos_Max;
extern double g_dbChm_MinPres;
extern double g_dbChm_MaxPres;
extern BOOL	 g_bHeaterSetUse;
// 2005.12.26
extern BOOL g_bStgOffs_Use;

// 2005.12.28
extern BOOL g_bStep_Time_Compen_Use;
extern BOOL g_bMFC_RampUse;

// 2005.05.30 by cskim, for CVD Step Time & BacksideAr
extern BOOL g_bCVDStepTimeUse;
extern BOOL g_bBacksideArUse;

// 2005.09.22 by cskim, for 112 Valve Recipe
extern int g_nTotalValveLineStep;

extern void	SET_DISPLAY_INDEX( int i1 , int i2 , int i3 , int i4 );

extern char *RECIPE_CONTROL_Get_MAIN_DIRECTORY();
extern char *RECIPE_CONTROL_Get_FILE_DIRECTORY();
extern char *RECIPE_CONTROL_Get_GROUP_DIRECTORY();

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define FILE_ID   ""
#define RECIPE_ID ""

extern RecipeStepTemplate 	F_RECIPE0;
extern RecipeStepTemplateM	F_RECIPE[ MAX_STEP_COUNT ];
extern int					F_STEP_COUNT;
extern MFC_RCP_DATA			g_MFCData[MFC_BUF_LENGTH];
extern TEMP_RCP_DATA		g_HtrList[MAX_HEATER_NO];
extern BOOL g_bKeepLastConEnable;
extern BOOL g_bThickness;
extern BOOL g_bDMY;
char g_szRecipeFileName[80] = "";
//  2013.04.02 Recipe Password
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
DWORD pdwRoundKey[32];														// Round keys for encryption or decryption
BYTE  pbUserKey[16] = {0x57, 0x4F, 0x4E, 0x49, 0x4B, 0x49, 0x50, 0x53, 		// User secret key
					   0x32, 0x30, 0x31, 0x33, 0x00, 0x00, 0x00, 0x00};
//------------------------------------------------------------------------------------------
//=========================================================================================
BOOL PWEncrypt(char *szPassword)
{
	BYTE pbData[16] = "";
	char szTemp[256] ="";
	char szTemp2[256] ="";
	int i;
	int nLenth;

	nLenth = strlen(szPassword);
	for(i=0;i<nLenth;i++)
		pbData[i] = (BYTE) szPassword[i];
	
#ifdef _DEBUG
	printf ("\nPlaintext  : ");
	for (i=0;i<16;i++)	
		printf("%02X ",pbData[i]);
#endif
	// Derive roundkeys from user secret key
	SeedRoundKey(
		pdwRoundKey, 
		pbUserKey);

	// Encryption
	SeedEncrypt(
		pbData, 
		pdwRoundKey);

#ifdef _DEBUG
	// print encrypted data(ciphertext)
	printf ("\nCiphertext : ");
	for (i=0;i<16;i++)	
		printf("%02X ",pbData[i]);
#endif
	
	for (i=0;i<16;i++)	{
		sprintf(szTemp, "%02X", pbData[i]);
		strcat(szTemp2, szTemp);
		strcpy(szTemp, "");
	}
	strcpy(szPassword, szTemp2);

	return TRUE;
}
//  2013.04.02 Recipe Password
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// 2005.12.28
BOOL IsOneLineStep(RecipeStepTemplateM* pRcpStep)
{
	BOOL bRet = FALSE;

	do {
		if(pRcpStep->udRCP_Valve_Data.RCP_DATA[0][1].dbTime != 0) break;
		if(pRcpStep->udRCP_Valve_Data.RCP_DATA[1][0].dbTime != 0) break;
		bRet = TRUE;
	} while(0);

	return bRet;
}

//=========================================================================================
// 2006.01.02

double GetStepTime(RecipeStepTemplateM* pRcpStep)
{
	int i, j;
	double dbStepTime = 0;
	double dbValveStepTime;

	// 2006.02.24
	if(pRcpStep->nStepFlow_Option != SFL_NORMAL_CYC) return 0;

	for(i=0; i<pRcpStep->udRCP_Valve_Data.dTotalStep; i++)
	{
		dbValveStepTime = 0;
		for(j=0; j<g_nTotalValveLineStep ; j++)
		{
			if(pRcpStep->udRCP_Valve_Data.RCP_DATA[ i ][ j ].dbTime == 0) break;
			dbValveStepTime += pRcpStep->udRCP_Valve_Data.RCP_DATA[ i ][ j ].dbTime;
		}
		dbStepTime += (dbValveStepTime * pRcpStep->udRCP_Valve_Data.dValveStepCycleCount[i]);
	}
	dbStepTime *= pRcpStep->dValv_Rcp_Cycle_Cnt;
	return dbStepTime;
}

void Check_RecipeStep_Data_Err(RecipeStepTemplateM *pRcpStep)
{
	extern void CheckSubItemEnable(RecipeStepTemplateM* pRcpStep);
	extern BOOL IsOneLineStep(RecipeStepTemplateM* pRcpStep);
	extern BOOL g_bStgOffs_Use;

	if(g_bStgOffs_Use)
	{
		if(pRcpStep->nStgHtr1_Option != 2 && pRcpStep->nStgHtr1_Option != 1)
		{
			pRcpStep->nStgHtr1_Option = 0;
		}
		if(pRcpStep->nStgHtr2_Option != 2 && pRcpStep->nStgHtr2_Option != 1)
		{
			pRcpStep->nStgHtr2_Option = 0;
		}
	}
	if(! IsOneLineStep(pRcpStep)) pRcpStep->StepTimeCompen_nOpt = 0;
	CheckSubItemEnable(pRcpStep);
}

//=========================================================================================
//=========================================================================================
//
//
//
void MFC_DATA_Init( int i, int j )
{
	strcpy( F_RECIPE[ i ].szMFC_RealName[ j ], g_MFCData[ j ].szMFCRealName );
	F_RECIPE[ i ].dbMFC_Set_Point[ j ]	= g_MFCData[ j ].dbSetPoint;
	F_RECIPE[ i ].dMFC_Option[ j ]		=	8;		// 2006.08.11
	F_RECIPE[ i ].dbMFC_Tolerance[ j ] = 0;			// 2006.09.04

	// Dummy Recipe Data Init. 2009.08.18. Added by jhlee
	strcpy( F_RECIPE[ i ].szMFC_RealName_DMY[ j ], g_MFCData[ j ].szMFCRealName );
	F_RECIPE[ i ].dbMFC_Set_Point_DMY[ j ]	= g_MFCData[ j ].dbSetPoint_DMY;
	F_RECIPE[ i ].dMFC_Option_DMY[ j ]		=	8;		// 2006.08.11

}

void Heater_DATA_Init( int i, int j )
{
	F_RECIPE[ i ].Htr_pdbSetPoint[j] = 0;
	F_RECIPE[ i ].Htr_pnOption[j] = 0;
}

void Valve_Data_Init( int i, int j )
{
    // Valve Mask3 2010.02.03
	//extern VAL_STATUS *GetValveMaskCfg();
	extern VAL_STATUS *GetValveMaskCfgAdj();
	int	 k;

	for ( k = 0 ; k < MAX_VAL_TIME_STEP ; k++ )
	{
		F_RECIPE[ i ].udRCP_Valve_Data.dValveStepCycleCount[j] = 1;
		F_RECIPE[ i ].udRCP_Valve_Data.RCP_DATA[ j ][ k ].dbTime = 0;
      // 2010.02.03
      //memcpy(F_RECIPE[ i ].udRCP_Valve_Data.RCP_DATA[ j ][ k ].RCP_DATA, GetValveMaskCfg(), sizeof( VAL_STATUS ) * MAX_VAL_NO );
		memcpy(F_RECIPE[ i ].udRCP_Valve_Data.RCP_DATA[ j ][ k ].RCP_DATA, GetValveMaskCfgAdj(), sizeof( VAL_STATUS ) * MAX_VAL_NO );
		F_RECIPE[ i ].udRCP_Valve_Data_DMY.dValveStepCycleCount[j] = 1;
		F_RECIPE[ i ].udRCP_Valve_Data_DMY.RCP_DATA[ j ][ k ].dbTime = 0;
	}

	// 2005.12.28
	F_RECIPE[ i ].udRCP_Valve_Data.RCP_DATA[ 0 ][ 0 ].dbTime = 2;
	F_RECIPE[ i ].udRCP_Valve_Data.dTotalStep = 1;
	F_RECIPE[ i ].udRCP_Valve_Data.dValveStepCycleCount[0] = 1;
	F_RECIPE[ i ].udRCP_Valve_Data_DMY.RCP_DATA[ 0 ][ 0 ].dbTime = 2;
	F_RECIPE[ i ].udRCP_Valve_Data_DMY.dTotalStep = 1;
	F_RECIPE[ i ].udRCP_Valve_Data_DMY.dValveStepCycleCount[0] = 1;
}

// Added by jhlee. 2009.11.25 for ROR Check
void Valve_Data_Init2(RecipeStepTemplateM *pRecipeStep)
{
	extern VAL_STATUS *GetValveMaskCfg();
	int	 j, k;

	for ( j = 0 ; j < MAX_VAL_STEP_NO; j++ )
	{
		for ( k = 0 ; k < MAX_VAL_TIME_STEP ; k++ )
		{
			pRecipeStep->udRCP_Valve_Data.dValveStepCycleCount[j] = 1;
			pRecipeStep->udRCP_Valve_Data.RCP_DATA[ j ][ k ].dbTime = 0;
			memcpy(pRecipeStep->udRCP_Valve_Data.RCP_DATA[ j ][ k ].RCP_DATA, GetValveMaskCfg(), sizeof( VAL_STATUS ) * MAX_VAL_NO );
		}
	}

	// 2007.10.05, by cskim, DefaultTime:3sec
	pRecipeStep->udRCP_Valve_Data.RCP_DATA[ 0 ][ 0 ].dbTime = 3;
	pRecipeStep->udRCP_Valve_Data.dTotalStep = 1;
	pRecipeStep->udRCP_Valve_Data.dValveStepCycleCount[0] = 1;
}

void Kor_RecipeStep_Initial( int i ) 
{
	int		j;

	F_RECIPE[ i ].bHasValveData					=	FALSE;
	F_RECIPE[ i ].bHasValveData_DMY				=	FALSE;
	F_RECIPE[ i ].dbChamber_Pressure_Set_Point	=	g_dbChm_MaxPres;
	F_RECIPE[ i ].dbChamber_Pressure_Set_Point_L =	g_dbChm_MinPres;
	F_RECIPE[ i ].udChamber_Pressure_Option		=	4;		// 2006.08.11
	// 2005.12.28
	F_RECIPE[ i ].nChamber_Pressure_Timeout		=	5;

	F_RECIPE[ i ].dbThrottle_Valve_Set_Point	=	100;
	F_RECIPE[ i ].udThrottle_Valve_Option		=	0;		// 2006.08.11

	F_RECIPE[ i ].dbRF_PWR_Set_Point			=	0;
	F_RECIPE[ i ].udRF_PWR_Option				=	0;
	// 2014.03.31
	F_RECIPE[ i ].dbRF2_PWR_Set_Point			=	0;
	F_RECIPE[ i ].udRF2_PWR_Option				=	0;

	F_RECIPE[ i ].dbSTAGE_POS_Set_Point			=	(g_nStgHtrPos_Min + g_nStgHtrPos_Max) / 2;
	F_RECIPE[ i ].dbSTAGE2_POS_Set_Point		=	(g_nStgHtrPos_Min + g_nStgHtrPos_Max) / 2;
	F_RECIPE[ i ].nSTAGE_POS_WaitOpt			=	0;		//... 2015.03.21 Add Position Move Option (0:Wait)

	F_RECIPE[ i ].udVALV_CYC_PLUS_OPT			=	1;

	// 2004-04-29 IPS 서병찬 CJ 요구사항
	// Valve Rceipe Cycle Count 기본 값		1
	F_RECIPE[ i ].dValv_Rcp_Cycle_Cnt			=	1;
	// 2005.12.28
	F_RECIPE[ i ].bHasValveData					=	TRUE;

	F_RECIPE[ i ].bMainRcp						=	DESELECT;

	strcpy( F_RECIPE[ i ].szStepName, " " );

	// 2004.11.02 by cskim
	F_RECIPE[ i ].bPreTreat						=	FALSE;
	F_RECIPE[ i ].bSubItemEnable				=	FALSE;

	// 2006.09.04
	F_RECIPE[ i ].bSubItem2Enable				=	FALSE;

	// 2004.07.14 by cskim, 'Step Group Cycle' Added
	F_RECIPE[ i ].nRoughValve = 0;

	// 2004.11.05 by cskim
	// Stage Heater Data
	F_RECIPE[ i ].dbStgHtr1_SetPoint = 0;
	F_RECIPE[ i ].dbStgHtr2_SetPoint = 0;
	F_RECIPE[ i ].nStgHtr1_Option = 0;
	F_RECIPE[ i ].nStgHtr2_Option = 0;
	F_RECIPE[ i ].dbRamp1_SetPoint = 5;
	F_RECIPE[ i ].dbRamp2_SetPoint = 5;
	F_RECIPE[ i ].nRamp1_Option = 0;
	F_RECIPE[ i ].nRamp2_Option = 0;
	// F_RECIPE[ i ].dbStgHtr_Tol = 5;			// Temp( C)
	F_RECIPE[ i ].dbStgHtr_Timeout = 120;		// minute

	// 2005.12.26
	F_RECIPE[ i ].StgOffs1_dbSp = 0;
	F_RECIPE[ i ].StgOffs2_dbSp = 0;
	F_RECIPE[ i ].StgOffs1_nOpt = 0;
	F_RECIPE[ i ].StgOffs2_nOpt = 0;

	// 2005.12.28
	F_RECIPE[ i ].StepTimeCompen_nOpt = 0;
	F_RECIPE[ i ].StepTimeCompen_dbRate = 0;
	F_RECIPE[ i ].StepTimeCompen_nPrev = 0;
	F_RECIPE[ i ].nMfcRampOpt = 0;

	// 2006.05.12
	F_RECIPE[ i ].StepTimeCompen_dbMaxTime = 600;
	F_RECIPE[ i ].StepCycleCompen_dbMaxCycle = 100;

	//2007.12.06
	//F_RECIPE[ i ].nCleanCycType = 0;

	// RF Preset Data
	F_RECIPE[ i ].dbRFLdPrst_SetPoint = 0;
	F_RECIPE[ i ].nRFLdPrst_Option = 0;
	F_RECIPE[ i ].dbRFTnPrst_SetPoint = 0;
	F_RECIPE[ i ].nRFTnPrst_Option = 0;
	// RF2 Preset Data 2014.03.31
	F_RECIPE[ i ].dbRF2LdPrst_SetPoint = 0;
	F_RECIPE[ i ].nRF2LdPrst_Option = 0;
	F_RECIPE[ i ].dbRF2TnPrst_SetPoint = 0;
	F_RECIPE[ i ].nRF2TnPrst_Option = 0;
	// Heat Exchange 2014.04.03
	F_RECIPE[ i ].dbHTE_SetPoint = 0;
	F_RECIPE[ i ].nHTE_Option = 0;
	// 2014.05.22 Preset Mode
	F_RECIPE[ i ].nPrstMode_Option = 0;
	F_RECIPE[ i ].nPrst2Mode_Option = 0;
	F_RECIPE[ i ].nPrstSelMode_Option = 0;
	F_RECIPE[ i ].nPrst2SelMode_Option = 0;

	// 2004.11.09 by cskim, Step Flow
	F_RECIPE[ i ].nStepFlow_Option = SFL_NORMAL_CYC;

	// 2005.05.09 by cskim, for CVD Editor
	F_RECIPE[ i ].dbCVDStepTime = 0;
	for(j=0; j<MAX_VALVE_SEQ; j++) F_RECIPE[ i ].pnVrcpType[j] = 0;
	for(j=0; j<MAX_MFC_NO; j++) F_RECIPE[ i ].pnMfcVlvSeq[j] = 0;

	for ( j = 0 ; j < MAX_VAL_STEP_NO; j++ )	Valve_Data_Init( i, j );
	for ( j = 0 ; j < MFC_BUF_LENGTH ; j++ )	MFC_DATA_Init( i, j );
	for ( j = 0 ; j < MAX_HEATER_NO  ; j++ )	Heater_DATA_Init( i, j );

	// 2005.05.24 by cskim, for Backside Helium Check
	F_RECIPE[ i ].nBacksideArCheck = 0;

	// Added by jhlee 2009.11.25 for ROR Check
	F_RECIPE[ i ].nSubFunc = SF_NONE;
}

void Kor_RecipeStep0_Initial() 
{
	int i;
	F_RECIPE0.udRcpType			=	g_RcpFileType;
	
	// 2004.07.14 by cskim, 'Step Group Cycle' Added
	F_RECIPE0.nStepGrpBegin = 0;
	F_RECIPE0.nStepGrpEnd = 0;
	F_RECIPE0.nStepGrpCyc = 0;
	// 2004.11.18 by cskim
	F_RECIPE0.nIdleStopBegin = 0;
	// 2005.05.24 by cskim, for Keep Last Condition
	F_RECIPE0.bKeepLastCon = FALSE;
	// 2006.01.25
	F_RECIPE0.dbThickness = 0;


	// 2013.04.02 Recipe Password Added by hyuk 
	strcpy( F_RECIPE0.szRecipePW, "" );

	for ( i = 0 ; i < MAX_STEP_COUNT ; i++ ) Kor_RecipeStep_Initial( i );
}

void Process_MFC_Integer_Data( int i, char* pszNameTag, int Data )
{
	int		dIndex;
	char	szTemp1[50], szTemp2[50], szType[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szTemp1, szType, 50 );

	dIndex	=	atoi( szTemp1 );

	if	( !strcmp( szType, "OPTION" ) )			F_RECIPE[ i ].dMFC_Option[ dIndex]  = Data;  //2014.04.04
}

void Process_MFC_Integer_Data_DMY( int i, char* pszNameTag, int Data )
{
	int		dIndex;
	char	szTemp0[50], szTemp1[50], szTemp2[50], szType[50], szTemp[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp, szTemp0, 50 );
	STR_SEPERATE_CHAR( szTemp0, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szTemp1,szType, 50 );

	dIndex	=	atoi( szTemp1 );

//	printf("pszNamTag : %s, szTemp: %s, szTemp0: %s, szTemp1: %s, szTEmp2: %s \n",pszNameTag, szTemp, szTemp0,szTemp1,szTemp2);
//	printf("DMY MFC Data from FILE. szType : %s \n",szType);
//	printf("Option dIndex : %d, Dummy Option Data: %d \n",dIndex, Data);

	if	( !strcmp( szType, "OPTION" ) )			F_RECIPE[ i ].dMFC_Option_DMY[dIndex]  = Data;

}

//2008.10.15 mgsong hard code due to first mfc is spare but mfc map 
//           have to count spare mfc but there is no space so code modify its only for bw
void Process_MFC_Real_Data( int i, char* pszNameTag, float fData )
{
	int		dIndex;
	char	szTemp1[50], szTemp2[50], szType[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szTemp1, szType, 50 );

	dIndex	=	atoi( szTemp1 );
	//2008.10.15
	//if (0 == dIndex) return;

	if		( !strcmp( szType, "SETPOINT" ) )			F_RECIPE[i].dbMFC_Set_Point[dIndex] = fData;    //2014.04.01
	else if ( !strcmp( szType, "TOLERANCE" ) )			F_RECIPE[i].dbMFC_Tolerance[dIndex] = fData;	//2014.04.01
}

void Process_MFC_Real_Data_DMY( int i, char* pszNameTag, float fData )
{
	int		dIndex;
	char	szTemp0[50], szTemp1[50], szTemp2[50], szType[50], szTemp[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp, szTemp0, 50 );
	STR_SEPERATE_CHAR( szTemp0, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szTemp1,szType, 50 );

	dIndex	=	atoi( szTemp1 );
	//2008.10.15
	//if (0 == dIndex) return;

//	printf("Dummy MFC Data. pszNameTag: %s, szType: %s, fData: %5.2f \n",pszNameTag,szType,fData);


	if		( !strcmp( szType, "SETPOINT" ) )			F_RECIPE[i].dbMFC_Set_Point_DMY[dIndex] = fData;
	else if ( !strcmp( szType, "TOLERANCE" ) )			F_RECIPE[i].dbMFC_Tolerance_DMY[dIndex] = fData;	// 2006.09.04
}

void Process_Heater_Integer_Data( int i, char* pszNameTag, int Data )
{
	int		dIndex;
	char	szTemp1[50], szTemp2[50], szType[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szTemp1, szType, 50 );

	dIndex	=	atoi( szTemp1 );

	if		( !strcmp( szType, "OPT" ) )		F_RECIPE[ i ].Htr_pnOption[ dIndex ] = Data;
	// else if ( !strcmp( szType, "SET" ) )		F_RECIPE[ i ].dSetValue[ dIndex ]	= Data;
}

void Process_Heater_Real_Data( int i, char* pszNameTag, float fData )
{
	int		dIndex;
	char	szTemp1[50], szTemp2[50], szType[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szTemp1, szType, 50 );

	dIndex	=	atoi( szTemp1 );

	if		( !strcmp( szType, "SP" ) )	F_RECIPE[ i ].Htr_pdbSetPoint[ dIndex ] = fData;
	// else if ( !strcmp( szType, "TOLERANCE" ) )			F_RECIPE[ i ].dbTolerance[ dIndex ] = fData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//						RECIPE EDITOR VAVLE STEP  -  Read Routine
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------
// 2007.02.08
int CheckValveConfig(int nRcpType, int nValveIndex)
{
	VAL_STATUS* pDest_Valve_Map;
	extern VAL_STATUS RCP_VAL_CFG[MAX_VAL_NO];
	extern VAL_STATUS PRE_VAL_CFG[MAX_VAL_NO];
	extern VAL_STATUS PST_VAL_CFG[MAX_VAL_NO];
	extern VAL_STATUS IDL_VAL_CFG[MAX_VAL_NO];
	extern VAL_STATUS CLN_VAL_CFG[MAX_VAL_NO];
	extern VAL_STATUS PUG_VAL_CFG[MAX_VAL_NO];
	extern VAL_STATUS LPR_VAL_CFG[MAX_VAL_NO];
	extern VAL_STATUS LPS_VAL_CFG[MAX_VAL_NO];

	switch( nRcpType )
	{
		case RCP_PRC :	pDest_Valve_Map = RCP_VAL_CFG;	break;
		case RCP_PRE :	pDest_Valve_Map = PRE_VAL_CFG;	break;
		case RCP_PST :	pDest_Valve_Map = PST_VAL_CFG;	break;
		case RCP_IDL :	pDest_Valve_Map = IDL_VAL_CFG;	break;
		case RCP_CLN :	pDest_Valve_Map = CLN_VAL_CFG;	break;
		case RCP_PUG :	pDest_Valve_Map = PUG_VAL_CFG;	break;
		case RCP_LPR :	pDest_Valve_Map = LPR_VAL_CFG;	break;
		case RCP_LPS :	pDest_Valve_Map = LPS_VAL_CFG;	break;
		default : return VAL_CLOSE;
	}

	// printf("-----> %d %d %d\n", nRcpType, nValveIndex, pDest_Valve_Map[nValveIndex]);
	return pDest_Valve_Map[nValveIndex];
}

//------------------------------------------------------------------------------------------
void Process_Valve_String_Data( int ProcessStep, char* pszNameTag, char* szData )
{

//	VAL_STATUS_%02d_%02d
	int		i, dIndex, dIndex2;
	int		dData;
	int		dLength;
	char	szTemp1[50], szTemp2[50], szType[50];

	extern BOOL g_bOldRcpValveAutoEnDis;	// 2007.02.08

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szType, szTemp1, 50 );

	if ( !stricmp( szType, "STATUS" ) )
	{
		STR_SEPERATE_CHAR( szTemp1	 , '_', szType, szTemp2, 50 );
		dLength	=	strlen( szData );

		dIndex	=	atoi( szType );
		dIndex2	=	atoi( szTemp2 );

		for ( i = 0 ; i < dLength ; i++ )
		{
			dData  = szData[ i ] - '0';
			// 2010.02.03

		    if(! g_bOldRcpValveAutoEnDis)
			{
				if(CheckValveConfig(F_RECIPE0.udRcpType, i) == 3)
				{

					 if(dData == VAL_NOUSE || dData == 3)
					 {   // RecipeConfig->VALVE_MASK:VAL_USE(1) && RecipeData->ValveStatus:VAL_NOUSE
						 // It was modified to make eidtable to bit on VALVE_MASK 2008.09.25
						 F_RECIPE[ProcessStep].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_CLOSE;
					 }
					 else
					 {
						 F_RECIPE[ProcessStep].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = dData;
					 }
				}
				else
				{
					if(dData == 3) F_RECIPE[ProcessStep].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_CLOSE;
					else F_RECIPE[ProcessStep].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = dData;
				}
			}
			else
			{
				if(CheckValveConfig(F_RECIPE0.udRcpType, i) != VAL_NOUSE)
				{
					if(CheckValveConfig(F_RECIPE0.udRcpType, i) == 3)
					{
						F_RECIPE[ProcessStep].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_CLOSE;
					}
					else
					{
						F_RECIPE[ProcessStep].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = dData;
					}
				}
				else F_RECIPE[ProcessStep].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_NOUSE;
			}
		}

	}
}

void Process_Valve_String_Data_DMY( int ProcessStep, char* pszNameTag, char* szData )                                
{                                                                                                                    
                                                                                                                     
//	VAL_STATUS_%02d_%02d                                                                                         
	int		i, dIndex, dIndex2;                                                                          
	int		dData;                                                                                       
	int		dLength;                                                                                     
	char	szTemp0[50], szTemp1[50], szTemp2[50], szType[50], szTemp[50];                                       
                                                                                                                     
	extern BOOL g_bOldRcpValveAutoEnDis;	// 2007.02.08                                                                                       
                                                                                                                                                    
	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp, szTemp0, 50 );                                                   
	STR_SEPERATE_CHAR( szTemp0, '_', szTemp1, szTemp2, 50 );                                                     
	STR_SEPERATE_CHAR( szTemp2	 , '_', szType, szTemp1, 50 );                                               
                                                                                                                     
//	printf("DMY Data from FILE. szType : %s \n",szType);                                                         
	if ( !stricmp( szType, "STATUS" ) )                                                                          
	{                                                                                                            
		STR_SEPERATE_CHAR( szTemp1	 , '_', szType, szTemp2, 50 );                                       
		dLength	=	strlen( szData );                                                                    
                                                                                                                     
		dIndex	=	atoi( szType );                                                                      
		dIndex2	=	atoi( szTemp2 );                                                                     
                                                                                                                     
//		printf("szData : %s \n",szData);                                                                     
		for ( i = 0 ; i < dLength ; i++ )                                                                    
		{                                                                                                    
			dData  = szData[ i ] - '0';                                                                  
			// 2010.02.03                                                                                                               
		    if(! g_bOldRcpValveAutoEnDis)                                                                                                   
			{                                                                                                                           
				if(CheckValveConfig(F_RECIPE0.udRcpType, i) == 3)                                                                   
				{                                                                                                                   
					 if(dData == VAL_NOUSE || dData == 3)                                                                       
					 {   // RecipeConfig->VALVE_MASK:VAL_USE(1) && RecipeData->ValveStatus:VAL_NOUSE                            
						 // It was modified to make eidtable to bit on VALVE_MASK 2008.09.25                                
						 F_RECIPE[ProcessStep].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_CLOSE;      
					 }                                                                                                          
					 else                                                                                                       
					 {                                                                                                          
					     F_RECIPE[ProcessStep].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = dData;
					 }                                                                                                          
				}                                                                                                                   
				else                                                                                                                
				{                                                                                                                   
					if(dData == 3) F_RECIPE[ProcessStep].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_CLOSE;
					else F_RECIPE[ProcessStep].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = dData;              
				}                                                                                                                   
			}                                                                                                                           
			else                                                                                                                        
			{                                                                                                                           
				if(CheckValveConfig(F_RECIPE0.udRcpType, i) != VAL_NOUSE)                                                           
				{                                                                                                                   
					if(CheckValveConfig(F_RECIPE0.udRcpType, i) == 3)                                                           
					{                                                                                                           
						F_RECIPE[ProcessStep].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_CLOSE;       
					}                                                                                                           
					else                                                                                                        
					{                                                                                                           
						F_RECIPE[ProcessStep].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = dData;           
					}                                                                                                           
				}                                                                                                                   
				else F_RECIPE[ProcessStep].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[i] = VAL_NOUSE;                  
			}                                                                                                                           
		}                                                                                                                                   
                                                                                                                                                    
	}                                                                                                                                           
}  

void Process_Valve_Integer_Data( int i, char* pszNameTag, int Data )
{
//	int		dIndex, dIndex2, dIndex3;
	int		dIndex;
	char	szTemp1[50], szTemp2[50], szType[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szType, szTemp1, 50 );

	if		( !strcmp( szType, "CYCCNT" ) )			
	{
		dIndex	=	atoi( szTemp1 );
		F_RECIPE[ i ].udRCP_Valve_Data.dValveStepCycleCount[ dIndex ]		= Data;
	}
	/*
	else if ( !strcmp( szType, "STATUS" ) )
	{
		/////////////////////////////////////////////////////////////////////////////////////
		//
		//			Valve Status Part - 수정 부분
		//
		/////////////////////////////////////////////////////////////////////////////////////

		STR_SEPERATE_CHAR( szTemp1	 , '_', szTemp2, szType, 50 );
		dIndex = atoi( szTemp2 );
		STR_SEPERATE_CHAR( szType	 , '_', szTemp1, szType, 50 );
		dIndex2 = atoi( szTemp1 );
		dIndex3 = atoi( szType );
		
		F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[dIndex3] = Data;

		/////////////////////////////////////////////////////////////////////////////////////
	}
	*/
}

void Process_Valve_Integer_Data_DMY( int i, char* pszNameTag, int Data )
{
//	int		dIndex, dIndex2, dIndex3;
	int		dIndex;
	char	szTemp0[50], szTemp1[50], szTemp2[50], szType[50], szTemp[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp, szTemp0, 50 );
	STR_SEPERATE_CHAR( szTemp0, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szType, szTemp1, 50 );

	if		( !strcmp( szType, "CYCCNT" ) )			
	{
		dIndex	=	atoi( szTemp1 );
		F_RECIPE[ i ].udRCP_Valve_Data_DMY.dValveStepCycleCount[ dIndex ]		= Data;
	}
	/*
	else if ( !strcmp( szType, "STATUS" ) )
	{
		/////////////////////////////////////////////////////////////////////////////////////
		//
		//			Valve Status Part - 수정 부분
		//
		/////////////////////////////////////////////////////////////////////////////////////

		STR_SEPERATE_CHAR( szTemp1	 , '_', szTemp2, szType, 50 );
		dIndex = atoi( szTemp2 );
		STR_SEPERATE_CHAR( szType	 , '_', szTemp1, szType, 50 );
		dIndex2 = atoi( szTemp1 );
		dIndex3 = atoi( szType );
		
		F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].RCP_DATA[dIndex3] = Data;

		/////////////////////////////////////////////////////////////////////////////////////
	}
	*/
}

void Process_Valve_Real_Data( int i, char* pszNameTag, float fData )
{
	int		dIndex, dIndex2;
	char	szTemp1[50], szTemp2[50], szType[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szType, szTemp1, 50 );

	if		( !strcmp( szType, "TIME" ) )
	{
		STR_SEPERATE_CHAR( szTemp1	 , '_', szType, szTemp2, 50 );
		dIndex	=	atoi( szType );
		dIndex2 =	atoi( szTemp2 );
		F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ dIndex ][ dIndex2 ].dbTime = fData;
	}

}

void Process_Valve_Real_Data_DMY( int i, char* pszNameTag, float fData )
{
	int		dIndex, dIndex2;
	char	szTemp0[50], szTemp1[50], szTemp2[50], szType[50], szTemp[50];

	STR_SEPERATE_CHAR( pszNameTag, '_', szTemp, szTemp0, 50 );
	STR_SEPERATE_CHAR( szTemp0, '_', szTemp1, szTemp2, 50 );
	STR_SEPERATE_CHAR( szTemp2	 , '_', szType, szTemp1, 50 );

	if		( !strcmp( szType, "TIME" ) )
	{
		STR_SEPERATE_CHAR( szTemp1	 , '_', szType, szTemp2, 50 );
		dIndex	=	atoi( szType );
		dIndex2 =	atoi( szTemp2 );
		F_RECIPE[i].udRCP_Valve_Data_DMY.RCP_DATA[ dIndex ][ dIndex2 ].dbTime = fData;
	}

}

BOOL Kor_Recipe_Open( char* RecipeFileName )
{
	BOOL StepEndFind;
	int  i,kill;

	char  cc_return;
	int   ii_return;
	float ff_return;
	char  str_return[256];
	char  name_return[256];
	char  type_return;

	HFILE hFile;


	// Initial
	Kor_RecipeStep0_Initial();

	if ( RecipeFileName == NULL ) return FALSE;
	hFile = _lopen( RecipeFileName , OF_READ );
	if ( hFile == -1 ) return FALSE;

	// INDICATOR
	if ( !read_form( hFile , FILE_ID                , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return FALSE; }
	if ( !read_form( hFile , RECIPE_ID              , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return FALSE; }

	// HEADER
	if ( !read_form( hFile , "__&&HEADER_BEGIN&&__" , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return TRUE; }
	if ( !read_form( hFile , "NumberOfSteps"        , INT_FORMAT    , &cc_return , &F_STEP_COUNT		, &ff_return , str_return		 ) ) { _lclose( hFile ); return TRUE; }
	if ( !read_form( hFile , "__&&HEADER_END&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return TRUE; }


	// Step 0 Start
	if ( !read_form( hFile , "__&&STEP0_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return       ) ) { _lclose( hFile ); return TRUE; }
	//StepRead
	StepEndFind = FALSE;
	kill = 0;
	while( !StepEndFind ) {
		if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) { _lclose( hFile ); return TRUE; printf( "return Error in\n" ); }
		switch (type_return) {
			case NAME_FORMAT :
								if ( strcmp( name_return , "__&&STEP0_END&&__" ) == 0 ) StepEndFind = TRUE;
								else	kill++;
								break;
			case CHAR_FORMAT :
								kill++;
								break;
			case INT_FORMAT  :	
					if		( strcmp( name_return , "RECIPE_TYPE"				) == 0 )
					{
						g_RcpFileType			=	ii_return;
						F_RECIPE0.udRcpType  	=	ii_return;
					}

					// 2004.07.14 by cskim, 'Step Group Cycle' Added
					else if		( strcmp( name_return , "STEP_GRP_BGN"				) == 0 )
					{
						F_RECIPE0.nStepGrpBegin  	=	ii_return;
					}
					else if		( strcmp( name_return , "STEP_GRP_END"				) == 0 )
					{
						F_RECIPE0.nStepGrpEnd  	=	ii_return;
					}
					else if		( strcmp( name_return , "STEP_GRP_CYC"				) == 0 )
					{
						F_RECIPE0.nStepGrpCyc  	=	ii_return;
					}
					else if		( strcmp( name_return , "IDLE_STOP_BGN"				) == 0 )
					{
						F_RECIPE0.nIdleStopBegin  	=	ii_return;
					}
					else if		( strcmp( name_return , "KEEP_LAST_CON"				) == 0 )
					{
						F_RECIPE0.bKeepLastCon  	=	ii_return;
					}

					else kill++;
					break;
			case FLOAT_FORMAT :
					if(strcmp( name_return , "PRO_THICKNESS") == 0)									// 2006.01.25
					{
						F_RECIPE0.dbThickness = ff_return;
					}
					else kill++;
					break;

			case STRING_FORMAT :
					//  2013.04.02 Recipe Password
					if(strcmp( name_return, "RECIPE_PW") == 0 )
					{
						strcpy( F_RECIPE0.szRecipePW, str_return );
					}
					//  2013.04.02 Recipe Password
					else kill++;
					break;
		}
		if ( kill >= 100 )	{ _lclose( hFile );	return TRUE; }
	}

	// STEP
	for ( i = 0 ; ( i < F_STEP_COUNT ) && ( i < MAX_STEP_COUNT ) ; i++ ) 
	{
		if ( !read_form( hFile , "__&&STEP_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return TRUE; }
		// StepRead
		StepEndFind = FALSE;
		kill = 0;

		while( !StepEndFind ) 
		{
			if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) 
			{ 
				_lclose( hFile ); 
				return TRUE;  
			}

			
			switch (type_return) 
			{
				case NAME_FORMAT :
									if ( strcmp( name_return , "__&&STEP_END&&__" ) == 0 ) StepEndFind = TRUE;
									else	kill++;
									break;
				case CHAR_FORMAT :
									kill++;
									break;


				case INT_FORMAT  :	
									if      ( !strcmp( name_return , "CH_PRES_OPTION"	) )		F_RECIPE[i].udChamber_Pressure_Option  			= ii_return;
									// 2005.12.28
									else if ( !strcmp( name_return , "CH_PRES_TIMEOUT"	) )		F_RECIPE[i].nChamber_Pressure_Timeout	 		= ii_return;

									else if ( !strcmp( name_return , "THROTTLE_OPTION"	) )		F_RECIPE[i].udThrottle_Valve_Option		 		= ii_return;
									else if ( !strcmp( name_return , "RF_OPTION"		) )		F_RECIPE[i].udRF_PWR_Option				 		= ii_return;
									else if ( !strcmp( name_return , "VAL_CYC_PLUS_OPT"	) )		F_RECIPE[i].udVALV_CYC_PLUS_OPT					= ii_return;
									else if ( !strcmp( name_return , "VAL_RCP_CYC_CNT"	) )		F_RECIPE[i].dValv_Rcp_Cycle_Cnt					= ii_return;
									else if ( !strcmp( name_return , "HAS_VALVE_DATA"	) )		F_RECIPE[i].bHasValveData						= ii_return;
									else if ( !strcmp( name_return , "VAL_TOT_STEP"		) )		F_RECIPE[i].udRCP_Valve_Data.dTotalStep			= ii_return;
									else if ( !strcmp( name_return , "MAIN_RCP"			) )		F_RECIPE[i].bMainRcp							= ii_return;
									else if ( !strcmp( name_return , "ROUGH_VLV"		) )		F_RECIPE[i].nRoughValve							= ii_return;
									else if ( !strcmp( name_return , "DMY_HAS_VALVE_DATA"	) )		F_RECIPE[i].bHasValveData_DMY						= ii_return;
									else if ( !strcmp( name_return , "DMY_VAL_TOT_STEP"		) )		F_RECIPE[i].udRCP_Valve_Data_DMY.dTotalStep			= ii_return;
									else if ( !strncmp( name_return, "MFC_"   , 4 )		  )		Process_MFC_Integer_Data( i, name_return, ii_return );		// MFC Data
									else if ( !strncmp( name_return, "DMY_MFC_"   , 8 )	  )		Process_MFC_Integer_Data_DMY( i, name_return, ii_return );		// Dummy MFC Data
									else if ( !strncmp( name_return, "HEATER_", 7 )		  )		Process_Heater_Integer_Data( i, name_return, ii_return );	// Heater Data
									else if ( !strncmp( name_return, "VAL_", 4 )		  )		Process_Valve_Integer_Data( i, name_return, ii_return );	// Valve Data
									else if ( !strncmp( name_return, "DMY_VAL_", 8 )	  )		Process_Valve_Integer_Data_DMY( i, name_return, ii_return );	// Dummay Valve Data

									else if ( !strcmp( name_return , "PRE_TREAT"		) )		F_RECIPE[i].bPreTreat						= ii_return;
									else if ( !strcmp( name_return , "SUB_ITEM"		) )			F_RECIPE[i].bSubItemEnable					= ii_return;

									else if ( !strcmp( name_return, "STGHTR1_OPTION") )		F_RECIPE[i].nStgHtr1_Option					    = ii_return;
									else if ( !strcmp( name_return, "STGHTR2_OPTION") )		F_RECIPE[i].nStgHtr2_Option					    = ii_return;

									else if ( !strcmp( name_return, "RAMP1_OPTION") )			F_RECIPE[i].nRamp1_Option					= ii_return;
									else if ( !strcmp( name_return, "RAMP2_OPTION") )		F_RECIPE[i].nRamp2_Option					    = ii_return;
					

									// 2006.09.04
									else if ( !strcmp( name_return , "SUB_ITEM2"		) )			F_RECIPE[i].bSubItem2Enable				= ii_return;

									// 2005.12.26
									else if ( !strcmp( name_return, "STGTMP_OFFSET1_OPT"))	F_RECIPE[i].StgOffs1_nOpt						= ii_return;
									else if ( !strcmp( name_return, "STGTMP_OFFSET2_OPT"))	F_RECIPE[i].StgOffs2_nOpt					    = ii_return;
									
									// 2005.12.28
									else if ( !strcmp( name_return, "STEP_TM_CMP_OPT"))			F_RECIPE[i].StepTimeCompen_nOpt				= ii_return;
									else if ( !strcmp( name_return, "STEP_TM_CMP_PREV"))		F_RECIPE[i].StepTimeCompen_nPrev			= ii_return;
									else if ( !strcmp( name_return, "_MFC_RAMP_OPT"))			F_RECIPE[i].nMfcRampOpt						= ii_return;

									else if ( !strcmp( name_return, "RF_LDPRST_OPTION") )		F_RECIPE[i].nRFLdPrst_Option				= ii_return;
									else if ( !strcmp( name_return, "RF_TNPRST_OPTION") )		F_RECIPE[i].nRFTnPrst_Option				= ii_return;
									else if ( !strcmp( name_return, "RF2_LDPRST_OPTION") )		F_RECIPE[i].nRF2LdPrst_Option				= ii_return;
									else if ( !strcmp( name_return, "RF2_TNPRST_OPTION") )		F_RECIPE[i].nRF2TnPrst_Option				= ii_return;
									// 2014.04.03
									else if ( !strcmp( name_return, "HTE_OPTION"))				F_RECIPE[i].nHTE_Option						= ii_return;
									// 2004.11.09 by cskim, Step Flow
									else if ( !strcmp( name_return, "STEP_FLOW_OPTION") )		F_RECIPE[i].nStepFlow_Option				= ii_return;
									// 2005.05.24 by cskim, for Backside Helium Check
									else if ( !strcmp( name_return, "BACKSIDE_AR_CHK"))			F_RECIPE[i].nBacksideArCheck				= ii_return;
									// 2014.05.22 Preset Mode
									else if ( !strcmp( name_return, "PRSTMODE"))				F_RECIPE[ i ].nPrstMode_Option				= ii_return;
									else if ( !strcmp( name_return, "PRST2MODE"))				F_RECIPE[ i ].nPrst2Mode_Option				= ii_return;
									else if ( !strcmp( name_return, "PRSTSELMODE"))				F_RECIPE[ i ].nPrstSelMode_Option				= ii_return;
									else if ( !strcmp( name_return, "PRST2SELMODE"))				F_RECIPE[ i ].nPrst2SelMode_Option				= ii_return;

									else if ( !strcmp( name_return, "SUB_FUNCTION") )			F_RECIPE[i].nSubFunc							= ii_return;
									//... 2015.03.21 Add Position Move Option
									else if (! strcmp( name_return, "STG_POS_WAIT_OPT"))		F_RECIPE[i].nSTAGE_POS_WaitOpt				= ii_return;

									else kill++;
									break;

				case FLOAT_FORMAT :
									if		(!strcmp( name_return , "CH_PRES_SETPOINT"  ))		F_RECIPE[i].dbChamber_Pressure_Set_Point	= ff_return;
									else if	(!strcmp( name_return , "CH_PRES_SETPOINT_L"  ))	F_RECIPE[i].dbChamber_Pressure_Set_Point_L	= ff_return;
									else if (!strcmp( name_return , "CH_PRES_TOLERANCE"))		0; // F_RECIPE[i].dbChamber_Pressure_Tolerance	= ff_return;
									else if (!strcmp( name_return , "CH_PRES_TIMEOUT"))			0; // F_RECIPE[i].dbChamber_Pressure_TimeOut		= ff_return;

									// Throttle Valve 
									else if (!strcmp( name_return , "THROTTLE_SETPOINT"))		F_RECIPE[i].dbThrottle_Valve_Set_Point		= ff_return;
									else if (!strcmp( name_return , "THROTTLE_TOLERANCE"))		0; // F_RECIPE[i].dbThrottle_Valve_Tolerance		= ff_return;
									else if (!strcmp( name_return , "THROTTLE_TIMEOUT"))		0; // F_RECIPE[i].dbThrottle_Valve_TimeOut		= ff_return;

									// RF Power
									else if (!strcmp( name_return , "RF_SETPOINT"))				F_RECIPE[i].dbRF_PWR_Set_Point				= ff_return;
									else if (!strcmp( name_return , "RF_TOLERANCE"))			0; // F_RECIPE[i].dbRF_PWR_Tolerance				= ff_return;
									else if (!strcmp( name_return , "RF_TIMEOUT"))				0; // F_RECIPE[i].dbRF_PWR_TimeOut				= ff_return;
									// RF2 Power 2014.03.31
									else if (!strcmp( name_return , "RF2_SETPOINT"))			F_RECIPE[i].dbRF2_PWR_Set_Point				= ff_return;
									else if (!strcmp( name_return , "RF2_TOLERANCE"))			0; // F_RECIPE[i].dbRF_PWR_Tolerance				= ff_return;
									else if (!strcmp( name_return , "RF2_TIMEOUT"))				0; // F_RECIPE[i].dbRF_PWR_TimeOut				= ff_return;

									// Stage Position
									else if (!strcmp( name_return , "STG_POS_SETPOINT"))		F_RECIPE[i].dbSTAGE_POS_Set_Point			= ff_return;
									else if (!strcmp( name_return , "STG2_POS_SETPOINT"))		F_RECIPE[i].dbSTAGE2_POS_Set_Point			= ff_return;
									else if (!strcmp( name_return , "STG_POS_TOLERANCE"))		0; // F_RECIPE[i].dbSTAGE_POS_Tolerance			= ff_return;
									else if (!strcmp( name_return , "STG_POS_TIMEOUT"))			0; // F_RECIPE[i].dbSTAGE_POS_TimeOut				= ff_return;

									else if ( !strncmp( name_return, "MFC_", 4 ))				Process_MFC_Real_Data( i, name_return,    ff_return );	// MFC Data
									else if ( !strncmp( name_return, "DMY_MFC_", 8 ))			Process_MFC_Real_Data_DMY( i, name_return,    ff_return );	// Dummy MFC Data
									else if ( !strncmp( name_return, "HEATER_", 7 )		  )		Process_Heater_Real_Data( i, name_return, ff_return );	// Heater Data
									else if ( !strncmp( name_return, "VAL_", 4 )		  )		Process_Valve_Real_Data( i, name_return,  ff_return );	// Valve Data
									else if ( !strncmp( name_return, "DMY_VAL_", 8 )	  )		Process_Valve_Real_Data_DMY( i, name_return,  ff_return );	// Dummay Valve Data

									// 2004.11.05 by cskim
									else if ( !strcmp( name_return, "STGHTR1_SETPOINT"))		F_RECIPE[i].dbStgHtr1_SetPoint			    = ff_return;
									else if ( !strcmp( name_return, "STGHTR2_SETPOINT"))		F_RECIPE[i].dbStgHtr2_SetPoint			    = ff_return;

									else if ( !strcmp( name_return, "RAMP1_SETPOINT"))			F_RECIPE[i].dbRamp1_SetPoint				= ff_return;
									else if ( !strcmp( name_return, "RAMP2_SETPOINT"))			F_RECIPE[i].dbRamp2_SetPoint				= ff_return;

									else if ( !strcmp( name_return, "STGHTR_TOL"))				0; // F_RECIPE[i].dbStgHtr_Tol				= ff_return;
									else if ( !strcmp( name_return, "STGHTR_TIMEOUT"))			F_RECIPE[i].dbStgHtr_Timeout				= ff_return;
									// 2005.12.26
									else if ( !strcmp( name_return, "STGTMP_OFFSET1_SP"))		F_RECIPE[i].StgOffs1_dbSp				    = ff_return;
									else if ( !strcmp( name_return, "STGTMP_OFFSET2_SP"))		F_RECIPE[i].StgOffs2_dbSp				    = ff_return;


									// 2005.12.28
									else if ( !strcmp( name_return, "STEP_TM_CMP_RATE"))		F_RECIPE[i].StepTimeCompen_dbRate			= ff_return;
									// 2005.12.28
									else if ( !strcmp( name_return, "STEP_TM_CMP_MAX"))			F_RECIPE[i].StepTimeCompen_dbMaxTime		= ff_return;

									//2009.02.02
									else if ( !strcmp( name_return, "STEP_CYC_CMP_MAX"))			F_RECIPE[i].StepCycleCompen_dbMaxCycle		= ff_return;
									//2014.04.03
									else if ( !strcmp( name_return, "HTE_SETPOINT"))		F_RECIPE[i].dbHTE_SetPoint				= ff_return;

									// 2005.05.18 by cskim, for CVD Recipe Editor
									else if ( !strcmp( name_return, "CVD_STEP_TIME"))			F_RECIPE[i].dbCVDStepTime					= ff_return;
									else kill++;
									break;


				case STRING_FORMAT :
									if		( !strcmp( name_return, "STEP_NAME" ))				strcpy( F_RECIPE[i].szStepName			, str_return );	    // Step Name Return
									else if ( !strncmp( name_return, "VAL_", 4 )		  )		Process_Valve_String_Data( i, name_return,  str_return );	// Valve Data
									else if ( !strncmp( name_return, "DMY_VAL_", 8 )	  )		Process_Valve_String_Data_DMY( i, name_return,  str_return );	// Dummay Valve Data
									else	kill++;
									break;
			}

			if ( kill >= 100 )	
			{ 
				_lclose( hFile );	
				return TRUE; 
			}
		}
	}
	_lclose( hFile );
	return TRUE;
}

void Check_Valve_Total_Step( RecipeStepTemplateM* pMultiRecipeStep )
{
	int			i;
	int			dLimit	=	pMultiRecipeStep->udRCP_Valve_Data.dTotalStep;


	for ( i = 0 ; i < dLimit ; i++ )
	{
		if ( pMultiRecipeStep->udRCP_Valve_Data.RCP_DATA[ i ][ 0 ].dbTime == 0 )	break;
	}

	if ( i > 0 )	pMultiRecipeStep->udRCP_Valve_Data.dTotalStep = i;
	else
	{
		pMultiRecipeStep->bHasValveData	=	FALSE;
		pMultiRecipeStep->udRCP_Valve_Data.dTotalStep = 0;
	}
}

void Check_Valve_Total_Step_DMY( RecipeStepTemplateM* pMultiRecipeStep )
{
	int			i;
	int			dLimit	=	pMultiRecipeStep->udRCP_Valve_Data_DMY.dTotalStep;


	for ( i = 0 ; i < dLimit ; i++ )
	{
		if ( pMultiRecipeStep->udRCP_Valve_Data_DMY.RCP_DATA[ i ][ 0 ].dbTime == 0 )	break;
	}

	if ( i > 0 )	pMultiRecipeStep->udRCP_Valve_Data_DMY.dTotalStep = i;
	else
	{
		pMultiRecipeStep->bHasValveData_DMY	=	FALSE;
		pMultiRecipeStep->udRCP_Valve_Data_DMY.dTotalStep = 0;
	}
}

/////////// It is added for check recipe step 2006.03.27 /////////////////////////////////////////////////////////
BOOL IsValveRecipeTimeOK( int *pnNotOkStep)
{
	int		i;
	BOOL	bRecipeOK = TRUE;
	double  dbStepTime;
	extern  double g_dbStepTimeLowLimit;	// 2006.07.10 by cskim

	for ( i = 0 ; ( i < F_STEP_COUNT ) && ( i < MAX_STEP_COUNT ) ; i++ ) 
	{
		*pnNotOkStep = i+1;
		dbStepTime = GetStepTime(&F_RECIPE[i]);
		if(F_RECIPE[i].nStepFlow_Option == SFL_NORMAL_CYC && dbStepTime < g_dbStepTimeLowLimit)	// 2006.08.11
		{
			bRecipeOK = FALSE;
			break;
		}
	}

    return bRecipeOK;
}
/////////// It is added for check recipe step 2006.03.27 /////////////////////////////////////////////////////////


BOOL Kor_Recipe_Save( char* RecipeFileName ) 
{
	int		i, dMFC_Count;
	int	dHeater_Count;
	int		dValStep;
	int		dValTimeStep;
	int		dValCount;
	char	szMFC_TAG[30];
	char	szHeater_TAG[30];
	char	szValTag[50];
	char	szValStatus[ MAX_VAL_NO + 1 ];
	// 2005.12.28
	int		nMaxValStepNo;
	BOOL	bRemove_Data;
	HFILE	hFile;
	extern BOOL g_bValveStep_AllSave;
	// 2006.01.02
	extern BOOL g_bPreTreat;

	if ( RecipeFileName == NULL ) return FALSE;
	hFile = _lcreat( RecipeFileName , 0 );

	// INDICATOR
	write_nam_form( hFile , FILE_ID );
	write_nam_form( hFile , RECIPE_ID );
	// HEADER
	write_nam_form( hFile , "__&&HEADER_BEGIN&&__" );
	write_int_form( hFile , "NumberOfSteps" , F_STEP_COUNT );
	write_nam_form( hFile , "__&&HEADER_END&&__" );

	// Single Step 0
	write_nam_form( hFile , "__&&STEP0_BEGIN&&__" );
	write_int_form( hFile , "RECIPE_TYPE"				, F_RECIPE0.udRcpType );
	write_int_form( hFile , "STEP_GRP_BGN"				, F_RECIPE0.nStepGrpBegin);
	write_int_form( hFile , "STEP_GRP_END"				, F_RECIPE0.nStepGrpEnd);
	write_int_form( hFile , "STEP_GRP_CYC"				, F_RECIPE0.nStepGrpCyc);
	write_int_form( hFile , "IDLE_STOP_BGN"				, F_RECIPE0.nIdleStopBegin);

	// Not Use 2013.11.05 Recipe Download by Hyuk
	// 2005.05.24 by cskim, for Keep Last Condition
	if(g_bKeepLastConEnable) write_int_form( hFile , "KEEP_LAST_CON", F_RECIPE0.bKeepLastCon);
	// Not Use 2013.11.05 Recipe Download by Hyuk

	// 2006.01.25
	if(g_bThickness) write_flt_form( hFile , "PRO_THICKNESS", (float) F_RECIPE0.dbThickness);
		
	//  2013.04.02 Recipe Password
	if(strcmp(F_RECIPE0.szRecipePW, "") != 0 && strlen(F_RECIPE0.szRecipePW) <=16)	PWEncrypt(F_RECIPE0.szRecipePW);
	write_str_form( hFile, "RECIPE_PW"					, F_RECIPE0.szRecipePW);
	//  2013.04.02 Recipe Password

	write_nam_form( hFile , "__&&STEP0_END&&__" );

	// STEP
	for ( i = 0 ; ( i < F_STEP_COUNT ) && ( i < MAX_STEP_COUNT ) ; i++ ) 
	{
		write_nam_form( hFile , "__&&STEP_BEGIN&&__" );

		write_str_form( hFile, "STEP_NAME"			, F_RECIPE[i].szStepName							);
		write_int_form( hFile, "MAIN_RCP"			, F_RECIPE[i].bMainRcp								);
		
		// Not Use 2013.11.05 Recipe Download by Hyuk
		//if(g_bValveStep_AllSave && g_bPreTreat)
		//{
			// 2004.11.08 by cskim
		//	write_int_form( hFile, "PRE_TREAT"			, F_RECIPE[i].bPreTreat								);
		//}
		// Not Use 2013.11.05 Recipe Download by Hyuk

		write_int_form( hFile, "SUB_ITEM"			, F_RECIPE[i].bSubItemEnable							);

		// Chamber Pressure 
		write_flt_form( hFile , "CH_PRES_SETPOINT"	, (float) F_RECIPE[i].dbChamber_Pressure_Set_Point	);
		write_flt_form( hFile , "CH_PRES_SETPOINT_L"	, (float) F_RECIPE[i].dbChamber_Pressure_Set_Point_L );
		write_int_form( hFile , "CH_PRES_OPTION"	, F_RECIPE[i].udChamber_Pressure_Option				);
		write_int_form( hFile , "CH_PRES_TIMEOUT"	, F_RECIPE[i].nChamber_Pressure_Timeout				);

		// Throttle Valve 
		write_flt_form( hFile , "THROTTLE_SETPOINT"	, (float) F_RECIPE[i].dbThrottle_Valve_Set_Point	);
		write_int_form( hFile , "THROTTLE_OPTION"	, F_RECIPE[i].udThrottle_Valve_Option				);

		// RF Power
		write_flt_form( hFile , "RF_SETPOINT"		, (float) F_RECIPE[i].dbRF_PWR_Set_Point			);
		write_int_form( hFile , "RF_OPTION"			, F_RECIPE[i].udRF_PWR_Option						);

		// RF2 Power 2014.03.31
		write_flt_form( hFile , "RF2_SETPOINT"		, (float) F_RECIPE[i].dbRF2_PWR_Set_Point			);
		write_int_form( hFile , "RF2_OPTION"			, F_RECIPE[i].udRF2_PWR_Option						);

		// Stage Position
		write_flt_form( hFile , "STG_POS_SETPOINT"	, (float) F_RECIPE[i].dbSTAGE_POS_Set_Point			);
		write_flt_form( hFile , "STG2_POS_SETPOINT"	, (float) F_RECIPE[i].dbSTAGE2_POS_Set_Point			);
		write_int_form( hFile , "STG_POS_WAIT_OPT"	, F_RECIPE[i].nSTAGE_POS_WaitOpt);		//... 2015.03.21 Add Position Move Option

		// 2004.07.14 by cskim, 'Step Group Cycle' Added
		write_int_form( hFile , "ROUGH_VLV"	, F_RECIPE[i].nRoughValve	);

		// 2004.11.05 by cskim
		// Stage Heater Data Set
		write_flt_form( hFile, "STGHTR1_SETPOINT"	, (float) F_RECIPE[i].dbStgHtr1_SetPoint);
		write_flt_form( hFile, "STGHTR2_SETPOINT", (float) F_RECIPE[i].dbStgHtr2_SetPoint);

		write_int_form( hFile, "STGHTR1_OPTION"	, F_RECIPE[i].nStgHtr1_Option);
		write_int_form( hFile, "STGHTR2_OPTION"	, F_RECIPE[i].nStgHtr2_Option);

		write_flt_form( hFile, "STGHTR_TIMEOUT"		, (float) F_RECIPE[i].dbStgHtr_Timeout);

		write_flt_form( hFile, "RAMP1_SETPOINT"	, (float) F_RECIPE[i].dbRamp1_SetPoint);
		write_flt_form( hFile, "RAMP2_SETPOINT"	, (float) F_RECIPE[i].dbRamp2_SetPoint);
		write_int_form( hFile, "RAMP1_OPTION"		, F_RECIPE[i].nRamp1_Option);
		write_int_form( hFile, "RAMP2_OPTION"		, F_RECIPE[i].nRamp2_Option);

		//---------------------------------------------------------------------------------
		// 2005.12.26
		if(g_bStgOffs_Use)
		{
			write_flt_form( hFile, "STGTMP_OFFSET1_SP"	, (float) F_RECIPE[i].StgOffs1_dbSp);
			write_flt_form( hFile, "STGTMP_OFFSET2_SP"	, (float) F_RECIPE[i].StgOffs2_dbSp);

			write_int_form( hFile, "STGTMP_OFFSET1_OPT"	, F_RECIPE[i].StgOffs1_nOpt);
			write_int_form( hFile, "STGTMP_OFFSET2_OPT"	, F_RECIPE[i].StgOffs2_nOpt);
		}

		if(g_bMFC_RampUse)
		{
			write_int_form( hFile, "_MFC_RAMP_OPT"	, F_RECIPE[i].nMfcRampOpt);
		}
		//---------------------------------------------------------------------------------

		// Not Use 2013.11.05 Recipe Download by Hyuk
		// RF Data Set
		//2014.05.16
		//write_flt_form( hFile, "RF_LDPRST_SETPOINT"	, (float) F_RECIPE[i].dbRFLdPrst_SetPoint);
		write_int_form( hFile, "RF_LDPRST_OPTION"	, F_RECIPE[i].nRFLdPrst_Option);
		//write_flt_form( hFile, "RF_TNPRST_SETPOINT"	, (float) F_RECIPE[i].dbRFTnPrst_SetPoint);
		//write_int_form( hFile, "RF_TNPRST_OPTION"	, F_RECIPE[i].nRFTnPrst_Option);
		// Not Use 2013.11.05 Recipe Download by Hyuk
		//write_flt_form( hFile, "RF2_LDPRST_SETPOINT"	, (float) F_RECIPE[i].dbRF2LdPrst_SetPoint);
		write_int_form( hFile, "RF2_LDPRST_OPTION"	, F_RECIPE[i].nRF2LdPrst_Option);
		//write_flt_form( hFile, "RF2_TNPRST_SETPOINT"	, (float) F_RECIPE[i].dbRF2TnPrst_SetPoint);
		//write_int_form( hFile, "RF2_TNPRST_OPTION"	, F_RECIPE[i].nRF2TnPrst_Option);
		//
		//2014.05.22
		write_int_form( hFile, "PRSTMODE"	, F_RECIPE[ i ].nPrstMode_Option);
		write_int_form( hFile, "PRST2MODE"	, F_RECIPE[ i ].nPrst2Mode_Option);
		write_int_form( hFile, "PRSTSELMODE"	, F_RECIPE[ i ].nPrstSelMode_Option);
		write_int_form( hFile, "PRST2SELMODE"	, F_RECIPE[ i ].nPrst2SelMode_Option);
		//
		// 2014.04.03
		write_flt_form( hFile, "HTE_SETPOINT"	, (float) F_RECIPE[i].dbHTE_SetPoint);
		write_int_form( hFile, "HTE_OPTION"	  , F_RECIPE[i].nHTE_Option);

		// 2004.11.09 by cskim, Step Flow
		write_int_form( hFile, "STEP_FLOW_OPTION"	, F_RECIPE[i].nStepFlow_Option);
		
		write_int_form( hFile, "VAL_CYC_PLUS_OPT"	, F_RECIPE[i].udVALV_CYC_PLUS_OPT			);
		write_int_form( hFile, "VAL_RCP_CYC_CNT"	, F_RECIPE[i].dValv_Rcp_Cycle_Cnt			);
		//2014.03.31
		write_int_form( hFile, "DMY_VAL_RCP_CYC_CNT"	, F_RECIPE[i].dValv_Rcp_Cycle_Cnt			);

		// MFC Data Write
		//2008.10.15 mgsong hard code due to first mfc is spare but mfc map 
		//           have to count spare mfc but there is no space so code modify its only for bw
		for ( dMFC_Count = 0 ; dMFC_Count < MFC_BUF_LENGTH ; dMFC_Count++ ) 
		{
			sprintf( szMFC_TAG, "MFC_%02d_SETPOINT", dMFC_Count );
			write_flt_form( hFile , szMFC_TAG, (float) F_RECIPE[i].dbMFC_Set_Point[dMFC_Count] );  //2014.04.01
			sprintf( szMFC_TAG, "MFC_%02d_OPTION", dMFC_Count );
			write_int_form( hFile , szMFC_TAG, F_RECIPE[i].dMFC_Option[dMFC_Count] ); //2014.04.01
		}
		// 2014.03.31
		// Dummy Recipe Write. 2009.08.18 Added by jhlee
		for ( dMFC_Count = 0 ; dMFC_Count < MFC_BUF_LENGTH ; dMFC_Count++ ) 
		{
			sprintf( szMFC_TAG, "DMY_MFC_%02d_SETPOINT", dMFC_Count );
			write_flt_form( hFile , szMFC_TAG, (float) F_RECIPE[i].dbMFC_Set_Point_DMY[dMFC_Count] );
			sprintf( szMFC_TAG, "DMY_MFC_%02d_OPTION", dMFC_Count );
			write_int_form( hFile , szMFC_TAG, F_RECIPE[i].dMFC_Option_DMY[dMFC_Count] );
		}
		// 2014.03.31
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//
		//						RECIPE EDITOR VAVLE STEP  -  Write Routine
		//
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		
		// Check Valve Total Step
		// 2004 - 05 - 12  IPS 한석원 요구 사항
		Check_Valve_Total_Step( &F_RECIPE[i] );

		// Valve Data Set
		write_int_form( hFile, "HAS_VALVE_DATA"		, F_RECIPE[i].bHasValveData				  );
		write_int_form( hFile, "VAL_TOT_STEP"		, F_RECIPE[i].udRCP_Valve_Data.dTotalStep );	// Valve Total Step

		// 2009.08.14 Added by jhlee (Dummy Valve & MFC Recipe)
		if(g_bValveStep_AllSave) nMaxValStepNo = MAX_VAL_STEP_NO;
		else nMaxValStepNo = F_RECIPE[i].udRCP_Valve_Data.dTotalStep;

		for ( dValStep = 0 ; dValStep < nMaxValStepNo ; dValStep++ )
		{
			// Valve Step Cycle Count
			sprintf( szValTag, "VAL_CYCCNT_%02d", dValStep );
			write_int_form( hFile, szValTag, F_RECIPE[i].udRCP_Valve_Data.dValveStepCycleCount[ dValStep ] );
			bRemove_Data	=	TRUE;

			// Time Step Count
			for ( dValTimeStep = 0 ; dValTimeStep < g_nTotalValveLineStep ; dValTimeStep++ )
			{
				// Valve Time Data Write
				/*	2004 - 04 - 26
					IPS 요구 사항 Time이 0이면 그 이하는 저장하지 않는다.	*/

				if ( !F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ dValStep ][ dValTimeStep ].dbTime )	bRemove_Data = FALSE;		// 중간에 Time이 0일 경우의 Flag


				if ( bRemove_Data ) 
				{
					for ( dValCount = 0 ; dValCount < g_Total_Valve_No ; dValCount++ )
					{
						szValStatus[ dValCount ] = F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ dValStep ][ dValTimeStep ].RCP_DATA[dValCount] + '0';
					}
				}
				else
				{
					for ( dValCount = 0 ; dValCount < g_Total_Valve_No ; dValCount++ )
					{

						if ( F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ dValStep ][ dValTimeStep ].RCP_DATA[dValCount] == VAL_NOUSE )	szValStatus[ dValCount ] = VAL_NOUSE + '0';
						else																										szValStatus[ dValCount ] = VAL_CLOSE + '0';

					}
				}

				szValStatus[ g_Total_Valve_No ] = '\0';

				sprintf( szValTag, "VAL_TIME_%02d_%02d", dValStep, dValTimeStep );
				write_flt_form( hFile, szValTag, (float) F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ dValStep ][ dValTimeStep ].dbTime );
				sprintf( szValTag, "VAL_STATUS_%02d_%02d", dValStep, dValTimeStep );
				write_str_form( hFile, szValTag, szValStatus );

				memset( szValStatus, 0, MAX_VAL_NO + 1 );

				// =====================================================================================================================================
			}
		}

		// Dummy Recipe Write. 2009.08.17 by jhlee
		Check_Valve_Total_Step_DMY( &F_RECIPE[i] );

		// Valve Data Set
		//2013.03.31
		write_int_form( hFile, "DMY_HAS_VALVE_DATA"		, F_RECIPE[i].bHasValveData_DMY				  );
		write_int_form( hFile, "DMY_VAL_TOT_STEP"		, F_RECIPE[i].udRCP_Valve_Data_DMY.dTotalStep );	// Valve Total Step
		if(g_bValveStep_AllSave) nMaxValStepNo = MAX_VAL_STEP_NO;
		else nMaxValStepNo = F_RECIPE[i].udRCP_Valve_Data_DMY.dTotalStep;
		for ( dValStep = 0 ; dValStep < nMaxValStepNo ; dValStep++ )
		{
			// Valve Step Cycle Count
			sprintf( szValTag, "DMY_VAL_CYCCNT_%02d", dValStep );
//			write_int_form( hFile, szValTag, F_RECIPE[i].udRCP_Valve_Data_DMY.dValveStepCycleCount[ dValStep ] );
			write_int_form( hFile, szValTag, F_RECIPE[i].udRCP_Valve_Data.dValveStepCycleCount[ dValStep ] );
			bRemove_Data	=	TRUE;

			// Time Step Count
			for ( dValTimeStep = 0 ; dValTimeStep < g_nTotalValveLineStep ; dValTimeStep++ )
			{
				// Valve Time Data Write
				/*	2004 - 04 - 26
					IPS 요구 사항 Time이 0이면 그 이하는 저장하지 않는다.	*/

				if ( !F_RECIPE[i].udRCP_Valve_Data_DMY.RCP_DATA[ dValStep ][ dValTimeStep ].dbTime )	bRemove_Data = FALSE;		// 중간에 Time이 0일 경우의 Flag


				if ( bRemove_Data ) 
				{
					for ( dValCount = 0 ; dValCount < g_Total_Valve_No ; dValCount++ )
					{
							szValStatus[ dValCount ] = F_RECIPE[i].udRCP_Valve_Data_DMY.RCP_DATA[ dValStep ][ dValTimeStep ].RCP_DATA[dValCount] + '0';
					}
				}
				else
				{
					for ( dValCount = 0 ; dValCount < g_Total_Valve_No ; dValCount++ )
					{

						if ( F_RECIPE[i].udRCP_Valve_Data_DMY.RCP_DATA[ dValStep ][ dValTimeStep ].RCP_DATA[dValCount] == VAL_NOUSE )	szValStatus[ dValCount ] = VAL_NOUSE + '0';
						else																										szValStatus[ dValCount ] = VAL_CLOSE + '0';
					}
				}

				szValStatus[ g_Total_Valve_No ] = '\0';
				//2013.03.31
				sprintf( szValTag, "DMY_VAL_TIME_%02d_%02d", dValStep, dValTimeStep );
				write_flt_form( hFile, szValTag, (float) F_RECIPE[i].udRCP_Valve_Data_DMY.RCP_DATA[ dValStep ][ dValTimeStep ].dbTime );
				sprintf( szValTag, "DMY_VAL_STATUS_%02d_%02d", dValStep, dValTimeStep );
				write_str_form( hFile, szValTag, szValStatus );

				memset( szValStatus, 0, MAX_VAL_NO + 1 );

				// =====================================================================================================================================
			}
		}

		// Not Use 2013.11.05 Recipe Download by Hyuk
		if(g_bHeaterSetUse)
		{
			// Heater Data Set
			for ( dHeater_Count = 0 ; dHeater_Count < MAX_HEATER_NO ; dHeater_Count++ )
			{
				sprintf( szHeater_TAG, "HEATER_%02d_SP", dHeater_Count );
				write_flt_form( hFile, szHeater_TAG, (float) F_RECIPE[ i ].Htr_pdbSetPoint[ dHeater_Count ] );

				sprintf( szHeater_TAG, "HEATER_%02d_OPT", dHeater_Count );
				write_int_form( hFile , szHeater_TAG, F_RECIPE[ i ].Htr_pnOption[ dHeater_Count ] );
			}
		}
		// Not Use 2013.11.05 Recipe Download by Hyuk

		//---------------------------------------------------------------------------------
		// 2005.12.28
		// 2006.06.30 Modified by cskim
		if(g_bStep_Time_Compen_Use)
		{
			write_int_form( hFile, "STEP_TM_CMP_OPT"	, F_RECIPE[i].StepTimeCompen_nOpt);
			write_flt_form( hFile, "STEP_TM_CMP_RATE"	, (float) F_RECIPE[i].StepTimeCompen_dbRate);
			write_int_form( hFile, "STEP_TM_CMP_PREV"	, F_RECIPE[i].StepTimeCompen_nPrev);
			write_flt_form( hFile, "STEP_TM_CMP_MAX"	, (float) F_RECIPE[i].StepTimeCompen_dbMaxTime);	// 2006.05.12
			write_flt_form( hFile, "STEP_CYC_CMP_MAX"	, (float) F_RECIPE[i].StepCycleCompen_dbMaxCycle);			// 2009.02.02

			//2007.12.06 by mgsong
			//write_int_form( hFile, "CLEAN_CYCLE_TYPE"   , F_RECIPE[i].nCleanCycType);
		}

		//---------------------------------------------------------------------------------
		// Not Use 2013.11.05 Recipe Download by Hyuk
		// 2005.05.30 by cskim, for CVD Recipe Editor
		if(g_bCVDStepTimeUse) write_flt_form( hFile, "CVD_STEP_TIME", (float) F_RECIPE[ i ].dbCVDStepTime);
		// Not Use 2013.11.05 Recipe Download by Hyuk
		if(g_bBacksideArUse) write_int_form( hFile, "BACKSIDE_AR_CHK", F_RECIPE[ i ].nBacksideArCheck);

		//---------------------------------------------------------------------------------
		// 2006.09.04, MFC Tolerance Write
		if(g_bSubItemEdit)
		{
			write_int_form( hFile, "SUB_ITEM2"			, F_RECIPE[i].bSubItem2Enable);

			for ( dMFC_Count = 0 ; dMFC_Count < MFC_BUF_LENGTH ; dMFC_Count++ ) 
			{
				sprintf( szMFC_TAG, "MFC_%02d_TOLERANCE", dMFC_Count );
				write_flt_form( hFile , szMFC_TAG, (float) F_RECIPE[ i ].dbMFC_Tolerance[ dMFC_Count ] );
			}
		}
		//---------------------------------------------------------------------------------
		// 2009.08.17 Dummy Recipe Write
		
		// Adeed by jhlee. 2009.11.25 for ROR Check
		write_int_form( hFile, "SUB_FUNCTION", F_RECIPE[i].nSubFunc);
		//---------------------------------------------------------------------------------
		write_nam_form( hFile , "__&&STEP_END&&__" );
	}
	_lclose( hFile );
	return TRUE;
}
//2014.06.27
void RemoveSpace(char *szDesc, char *szSrc)
{
	#define MAX_CHAR		80
	#define ASC_TAB			0x09

	char *pSrc, *pDesc;
	int i;

	pSrc = szSrc;
	pDesc = szDesc;
	i = 0;
	while(0x00 != *pSrc && i < MAX_CHAR-1)
	{
		switch(*pSrc)
		{
		case ' ':		*pDesc = '_'; break;
		case ASC_TAB:	*pDesc = '_'; break;
		default:		*pDesc = *pSrc; break;
		}

		pSrc++;
		pDesc++;
		i++;
	}
	*pDesc = 0x00;
}

void RemoveExtName(char *szDescFileName, char *szSrcFileName)
{
	int nLength;
	BOOL bExtName = FALSE;

	nLength = strlen(szSrcFileName);
	if(nLength > 4)
	{
		strncpy(szDescFileName, szSrcFileName, nLength-4);
		*(szDescFileName+nLength-4) = 0x00;
	}
	else strcpy(szDescFileName, "NotDefined");

}

BOOL Txt_Recipe_Save( char* szRecipeFileName )				
{
	extern	MFC_RCP_DATA			g_MFCData[20];

	FILE *fp;
	int i, j;
	int dValTimeStep, dValCount;
	char szTemp[80];
	char szFilePath[80] = "";
	int nValve;

	if(NULL == szRecipeFileName)
	{
		if(*g_szRecipeFileName != 0x00)
		{
			RemoveExtName(szTemp, g_szRecipeFileName);
		}
		else strcpy(szTemp, "NotDefined");
		
	}
	else strcpy(szTemp, szRecipeFileName);
	sprintf(szFilePath, "Recipe\\%s.csv", szTemp);

	fp = fopen( szFilePath, "wt" );
	if ( fp == NULL )
	{
		printf("-----> Text Recipe File Creation Failed[%s]\n", szFilePath);
		return FALSE;
	}
	printf("-----> Text Recipe File Creation[%s]\n", szFilePath);

	do {
		fprintf(fp, "OriginalRecipeName: %s\n\n", g_szRecipeFileName);
		fprintf(fp, "TotalStep,RcpType,StepGrpBgn,StepGrpEnd,StepGrpCyc,IdleStopBegin,KeepLastCon,Thickness\n");
		fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%.1f\n",
				F_STEP_COUNT, F_RECIPE0.udRcpType,
				F_RECIPE0.nStepGrpBegin, F_RECIPE0.nStepGrpEnd, F_RECIPE0.nStepGrpCyc,
				F_RECIPE0.nIdleStopBegin, F_RECIPE0.bKeepLastCon,
				(float) F_RECIPE0.dbThickness
				);
		fprintf(fp,"\n");

		fprintf(fp,"StepNum");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", i+1);
		fprintf(fp,"\n");

		fprintf(fp,"StepName");
		for(i=0; i<F_STEP_COUNT; i++)	{ RemoveSpace(szTemp, F_RECIPE[i].szStepName); fprintf(fp,",%s", szTemp); }
		fprintf(fp,"\n");

		fprintf(fp,"MainStep");
		for(i=0; i<F_STEP_COUNT; i++)	{ if(F_RECIPE[i].bMainRcp) fprintf(fp,",YES"); else fprintf(fp, ",NO"); }
		fprintf(fp,"\n");

		fprintf(fp,"RecipeCycle");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].dValv_Rcp_Cycle_Cnt);
		fprintf(fp,"\n");

		fprintf(fp,"StagePos");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbSTAGE_POS_Set_Point);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtrSet1");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbStgHtr1_SetPoint);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtrSet2");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbStgHtr2_SetPoint);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtrOpt1");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].nStgHtr1_Option);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtrOpt2");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].nStgHtr2_Option);
		fprintf(fp,"\n");


		fprintf(fp,"StgHtrRampSet");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbRamp1_SetPoint);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtrRampOp");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].nRamp1_Option);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtr2RampSet");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbRamp2_SetPoint);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtr2RampOp");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].nRamp2_Option);
		fprintf(fp,"\n");

		fprintf(fp,"StgHtrTimeOut");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbStgHtr_Timeout);
		fprintf(fp,"\n");

		fprintf(fp,"StepFlowOpt");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].nStepFlow_Option);
		fprintf(fp,"\n");

		fprintf(fp,"TV-Set");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbThrottle_Valve_Set_Point);
		fprintf(fp,"\n");

		fprintf(fp,"TV-Opt");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].udThrottle_Valve_Option);
		fprintf(fp,"\n");

		fprintf(fp,"PrsHi");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbChamber_Pressure_Set_Point);
		fprintf(fp,"\n");

		fprintf(fp,"PrsLo");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbChamber_Pressure_Set_Point_L);
		fprintf(fp,"\n");

		fprintf(fp,"PrsTimeout");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].nChamber_Pressure_Timeout);
		fprintf(fp,"\n");

		fprintf(fp,"PrsMonOpt");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].udChamber_Pressure_Option);
		fprintf(fp,"\n");
		fprintf(fp,"MFCRampOpt");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].nMfcRampOpt);
		fprintf(fp,"\n");

		fprintf(fp,"RF1_Set");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbRF_PWR_Set_Point);
		fprintf(fp,"\n");

		fprintf(fp,"RF1_Option");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d",  F_RECIPE[i].udRF_PWR_Option);
		fprintf(fp,"\n");

		fprintf(fp,"RF2_Set");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbRF2_PWR_Set_Point);
		fprintf(fp,"\n");

		fprintf(fp,"RF2_Option");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d",  F_RECIPE[i].udRF2_PWR_Option);
		fprintf(fp,"\n");

		fprintf(fp,"PresetMode");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[ i ].nPrstMode_Option);
		fprintf(fp,"\n");

		fprintf(fp,"PresetSel");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d",  F_RECIPE[ i ].nPrstSelMode_Option);
		fprintf(fp,"\n");

		fprintf(fp,"PresetType");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d",  F_RECIPE[i].nRFLdPrst_Option);
		fprintf(fp,"\n");

		fprintf(fp,"Preset2Mode");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[ i ].nPrst2Mode_Option);
		fprintf(fp,"\n");

		fprintf(fp,"Preset2Sel");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d",  F_RECIPE[ i ].nPrst2SelMode_Option);
		fprintf(fp,"\n");

		fprintf(fp,"Preset2Type");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d",  F_RECIPE[i].nRF2LdPrst_Option);
		fprintf(fp,"\n");



		fprintf(fp,"HeatExg_Set");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.1f", F_RECIPE[i].dbHTE_SetPoint);
		fprintf(fp,"\n");

		fprintf(fp,"HeatExg_Op");
		for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d",  F_RECIPE[i].nHTE_Option);
		fprintf(fp,"\n");

		for(j=0; j<20; j++)
		{
			if(g_MFCData[j].bUse)
			{
				RemoveSpace(szTemp, g_MFCData[j].szMFCDisplayName);
				fprintf(fp,"MFC%02d_Set[%s]", j+1, szTemp);
				for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.3f", F_RECIPE[i].dbMFC_Set_Point[j]);
				fprintf(fp,"\n");
			}
		}

		for(j=0; j<20; j++)
		{
			if(g_MFCData[j].bUse)
			{
				RemoveSpace(szTemp, g_MFCData[j].szMFCDisplayName);
				fprintf(fp,"MFC%02d_Opt[%s]", j+1, szTemp);
				for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%d", F_RECIPE[i].dMFC_Option[j]);
				fprintf(fp,"\n");
			}
		}

		// Time Step Count
		for ( dValTimeStep = 0 ; dValTimeStep < g_nTotalValveLineStep ; dValTimeStep++ )
		{
			fprintf(fp, "L%02d-Time", dValTimeStep+1);
			for(i=0; i<F_STEP_COUNT; i++)	fprintf(fp,",%.2f",
							F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ 0 ][ dValTimeStep ].dbTime);
			fprintf(fp,"\n");

			for ( dValCount = 0 ; dValCount < g_Total_Valve_No ; dValCount++ )
			{

				if ( F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ 0 ][ dValTimeStep ].RCP_DATA[dValCount] != VAL_NOUSE)
				{
					fprintf(fp, "L%02d-V%02d", dValTimeStep+1, dValCount+1);
					for(i=0; i<F_STEP_COUNT; i++)
					{
						if(F_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ 0 ][ dValTimeStep ].RCP_DATA[dValCount] == VAL_OPEN)
							nValve = 1;
						else nValve = 0;
						fprintf(fp,",%d", nValve);
					}
					fprintf(fp,"\n");
				}
			}
			fprintf(fp,"\n");
		}
	} while(0);

	fclose(fp);
	return TRUE;
}

