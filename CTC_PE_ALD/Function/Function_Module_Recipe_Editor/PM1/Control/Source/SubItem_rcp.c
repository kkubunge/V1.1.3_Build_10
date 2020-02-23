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
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "iodefine.h"
#include "Prg_Param.h"
#include "Data_Type.h"
#include "lottype.h"
#include "ledapi.h"
#include "interface.h"
#include "Recipe_Code.h"
#include "File.h"

#include "Prg_Param.h"
#include "cimseqnc.h"
#include "resource.h"
#include "TextParser_C.h"

extern  int						g_ProcessStep;
extern	RecipeStepTemplateM*	pSubItem_D_RECIPE;
extern	RecipeStepTemplate		D_RECIPE0;
extern	TEMP_RCP_DATA			g_HtrList[MAX_HEATER_NO];

extern	BOOL					g_bRoughValve;
extern	BOOL					g_bStgOffs_Use;
extern	BOOL					g_bStep_Time_Compen_Use;

//------------ 2005.12.13 by ycchoi ----------->>
extern	HWND	hWnd_ValveRecipe;
extern	HWND	hWnd_DetailValueRecipe;
extern	HWND	HWnd_SubItemRecipe;
//------------ 2005.12.13 by ycchoi -----------<<


extern void Check_RecipeStep_Data_Err(RecipeStepTemplateM *pRcpStep);
extern BOOL IsOneLineStep(RecipeStepTemplateM* pRcpStep);

enum { MFC_TOL_ID, MFC_NAME_ID };
//----------------------------------------------------------------------------
// Stage Heater Item Structure

ITEM_STRUCT g_StgHtr =
{
	"Stage Heater",	0, 1000,
	"NotCheck|Check|SetTemp|TempDown|TempUp"
};
char *g_StgHtrEnum[] =
{
	"NotCheck",
	"Check",
	"SetTemp",
	"TempDown",
	"TempUp"
};
ITEM_STRUCT g_StgHtrRamp =
{
	"Stage Heater Ramping",	0, 100,
	"NotSet|Set"
};
ITEM_STRUCT g_StgHtrTol =
{
	"Stage Heater Tolerance( C)", 1, 100,
	"NotUse|Use"
};
ITEM_STRUCT g_StgHtrTmOut =
{
	"Stage Heater Timeout(min)", 1, 1200,
	"NotUse|Use"
};
// 2005.12.26
ITEM_STRUCT g_StgHtrOffs =
{
	"Stage Temp Offset( C)", -50, 50,
	"NotSet|Set"
};

// 2006.02.24
ITEM_STRUCT g_StgHtrOffs2 =
{
	"Stage Temp Offset2( C)", -50, 50,
	"NotCheck|Check"
};
char *g_StgHtrOffs2Enum[] =
{
	"NotCheck",
	"Check"
};
// 2005.12.28
ITEM_STRUCT g_Step_Time_Compen =
{
	"Step Time Compensation", 0, 0,
	"NotUse|TimeCompensation|CycleCompensation"
};
char *g_Step_Time_CompenEnum[] =
{
	"NotUse",
	"TimeCompensation",
	"CycleCompensation"
};
ITEM_STRUCT g_Step_Time_Compen_Rate =
{
	"Step Time Compensation Rate", -100, 100,
	"",
	5
};
ITEM_STRUCT g_Step_Time_Compen_Prev =
{
	"Step Time Compen Preventive", 0, 0,
	"WaferCount|SourceFeeding|Thickness"
};
char *g_Step_Time_Compen_PrevEnum[] =
{
	"WaferCount",
	"SourceFeeding",
	"Thickness"
};
ITEM_STRUCT g_Step_Standard_Time =
{
	"Step Standard Time ", 600, 0,
	"",
	1
};
ITEM_STRUCT g_Step_Standard_Cycle =
{
	"Step Standard Cycle ", 100, 0,
	"",
	1
};
char *g_UseNotUseOptEnum[] =
{
	"NotUse",
	"Use"
};
//----------------------------------------------------------------------------
// Heater Control
// 2005.03.03 by cskim
extern	MFC_RCP_DATA			g_MFCData[20];

int g_anMfcRcID[MAX_MFC_DISPLAY][2] =
{
	IDC_BTN_MFC_TOL1,  	IDC_EDT_MFC_NAME1,
	IDC_BTN_MFC_TOL2,	IDC_EDT_MFC_NAME2,
	IDC_BTN_MFC_TOL3,	IDC_EDT_MFC_NAME3,
	IDC_BTN_MFC_TOL4,	IDC_EDT_MFC_NAME4,
	IDC_BTN_MFC_TOL5,	IDC_EDT_MFC_NAME5,
	IDC_BTN_MFC_TOL6,	IDC_EDT_MFC_NAME6,
	IDC_BTN_MFC_TOL7,	IDC_EDT_MFC_NAME7,
	IDC_BTN_MFC_TOL8,	IDC_EDT_MFC_NAME8,
	IDC_BTN_MFC_TOL9,	IDC_EDT_MFC_NAME9,
	IDC_BTN_MFC_TOL10,	IDC_EDT_MFC_NAME10,
	IDC_BTN_MFC_TOL11,	IDC_EDT_MFC_NAME11,
	IDC_BTN_MFC_TOL12,	IDC_EDT_MFC_NAME12,
	IDC_BTN_MFC_TOL13,	IDC_EDT_MFC_NAME13,
	IDC_BTN_MFC_TOL14,	IDC_EDT_MFC_NAME14,
	IDC_BTN_MFC_TOL15,	IDC_EDT_MFC_NAME15,
	IDC_BTN_MFC_TOL16,	IDC_EDT_MFC_NAME16
};

//----------------------------------------------------------------------------
// RF Item Structure
ITEM_STRUCT g_RFLdPrst =
{
	"RF Position Preset Type",
	0, 100,
	"NotSet|Type1|Type2|Type3|Type4|Type5|Type6|Type7|Type8"
};
ITEM_STRUCT g_RF2LdPrst =
{
	"RF2 Position Preset Type",
	0, 100,
	"NotSet|Type1|Type2|Type3|Type4|Type5|Type6|Type7|Type8"
};
char *g_RFLdPrstOptEnum[]=
{
	"NotSet",
	"Type1",
	"Type2",
	"Type3",
	"Type4",
	"Type5",
	"Type6",
	"Type7",
	"Type8"
};
ITEM_STRUCT g_PrstMode =
{
	"RF Mode",
	0, 100,
	"NotSet|Man|Auto"
};
ITEM_STRUCT g_Prst2Mode =
{
	"RF2 Mode",
	0, 100,
	"NotSet|Man|Auto"
};
char *g_PrstModeOptEnum[] =
{
	"NotSet",
	"Man",
	"Auto"
};
ITEM_STRUCT g_PrstSelMode =
{
	"RF Preset Mode",
	0, 100,
	"NotSet|NonePrst|Preset"
};
ITEM_STRUCT g_PrstSel2Mode =
{
	"RF2 Preset Mode",
	0, 100,
	"NotSet|NonePrst|Preset"
};
char *g_PrstSelModeOptEnum[] =
{
	"NotSet",
	"NonePrst",
	"Preset"
};
ITEM_STRUCT g_RFTnPrst =
{
	"RF Tune Position Preset",
	0, 100,
	"NotSet|Set"
};
ITEM_STRUCT g_RF2TnPrst =
{
	"RF2 Tune Position Preset",
	0, 100,
	"NotSet|Set"
};
ITEM_STRUCT g_HeatChg =
{
	"Heat Exchange",
	0, 100,
	"NotSet|Set"
};
char *g_SetNoSetOptEnum[] =
{
	"NotSet",
	"Set"
};
//----------------------------------------------------------------------------
// Step Flow Item Structure

//... 2015.03.07 Check Not Use Flow Control Option in Process Type Recipe
char *g_szStepFlowEnum1_arr[] =
{
	"Normal(Cycle)",
	"StgTmpMon_UnlmtCyc",
	"(N/A)",
	"HtrTmpMon_UnlmtCyc",
	"StepSync_UnlmtCyc",
	"RPGIgnition_StepNext"
};
char *g_szStepFlowEnum1 = "Normal(Cycle)|StgTmpMon_UnlmtCyc|-(N/A)|HtrTmpMon_UnlmtCyc|StepSync_UnlmtCyc|RPGIgnition_StepNext";
// char *g_szStepFlowEnum1 = "Normal(Cycle)|StgTmpMon_UnlmtCyc|-(N/A)|HtrTmpMon_UnlmtCyc|StepSync_UnlmtCyc|-(N/A)|Wait_SubFunc";
char *g_szStepFlowEnum2_arr[] =
{
	"Normal(Cycle)",
	"StgTmpMon_UnlmtCyc",
	"IdleStopMon_UnlmtCyc",
	"HtrTmpMon_UnlmtCyc",
	"(N/A)",
	"(N/A)",
	"Wait_SubFunc"
};
char *g_szStepFlowEnum2 = "Normal(Cycle)|StgTmpMon_UnlmtCyc|IdleStopMon_UnlmtCyc|HtrTmpMon_UnlmtCyc|-(N/A)|-(N/A)|Wait_SubFunc";

//----------------------------------------------------------------------------
// Step Flow Item Structure

ITEM_STRUCT g_RoughValve =
{
	"Rough Valve",
	0, 0,
	"NotCtrl|Open|Close"
};
char *g_RoughValveEnum[] =
{
	"NotCtrl",
	"Open",
	"Close"
};
//----------------------------------------------------------------------------
// Extern Function

void FILE_HANDLING_BUTTON_MAPPING( HWND hdlg , int id , int Enable );
HINSTANCE KGUI_s_Get_GLIB_BACKGROUND( void );

//----------------------------------------------------------------------------

void Init_SubItemConfig()
{
	extern int g_nRF_MinPos;
	extern int g_nRF_MaxPos;
	//2014.03.31
	extern int g_nRF2_MinPos;
	extern int g_nRF2_MaxPos;
	extern int g_nStgHtr_MinTemp;
	extern int g_nStgHtr_MaxTemp;
	extern int g_nRamp_MinTemp;
	extern int g_nRamp_MaxTemp;
	//2014.04.03
	extern int g_nHTE_MinTemp;
	extern int g_nHTE_MaxTemp;

	// 2005.12.28
	extern double g_dbValveTime_Min;
	extern double g_dbValveTime_Max;
	extern int g_nValveTime_DecPnt;


	// 2005.12.26
	extern double g_dbStgOffs_Min;
	extern double g_dbStgOffs_Max;

	g_RFLdPrst.dbMin = g_nRF_MinPos;
	g_RFLdPrst.dbMax = g_nRF_MaxPos;
	//2014.03.31
	g_RF2LdPrst.dbMin = g_nRF2_MinPos;
	g_RF2LdPrst.dbMax = g_nRF2_MaxPos;
	//2014.04.03
	g_HeatChg.dbMin = g_nHTE_MinTemp;
	g_HeatChg.dbMax = g_nHTE_MaxTemp;

	g_StgHtr.dbMin = g_nStgHtr_MinTemp;
	g_StgHtr.dbMax = g_nStgHtr_MaxTemp;
	g_StgHtrRamp.dbMin = g_nRamp_MinTemp;
	g_StgHtrRamp.dbMax = g_nRamp_MaxTemp;

	// 2005.12.26
	g_StgHtrOffs.dbMin = g_dbStgOffs_Min;
	g_StgHtrOffs.dbMax = g_dbStgOffs_Max;

	// 2006.02.24
	g_StgHtrOffs2.dbMin = g_dbStgOffs_Min;
	g_StgHtrOffs2.dbMax = g_dbStgOffs_Max;

	g_Step_Standard_Time.dbMin = g_dbValveTime_Min;
	g_Step_Standard_Time.dbMax = 600;
	g_Step_Standard_Time.nDecPoint = g_nValveTime_DecPnt;

	g_Step_Standard_Cycle.dbMin = 1;
	g_Step_Standard_Cycle.dbMax = 100;
	g_Step_Standard_Cycle.nDecPoint = 0;
}

void CheckSubItemEnable(RecipeStepTemplateM* pRcpStep)
{
	int i;
	BOOL bSubItemEnable = FALSE;
	pRcpStep->bSubItemEnable = FALSE;

	if(pRcpStep->nStgHtr1_Option > 1) bSubItemEnable = TRUE;
	if(pRcpStep->nStgHtr2_Option > 1) bSubItemEnable = TRUE;
	if(pRcpStep->nRFLdPrst_Option != 0) bSubItemEnable = TRUE;
	if(pRcpStep->nRFTnPrst_Option != 0) bSubItemEnable = TRUE;
	if(pRcpStep->nStepFlow_Option != SFL_NORMAL_CYC)	bSubItemEnable = TRUE;
	if(pRcpStep->nRoughValve != 0) bSubItemEnable = TRUE;
	if(pRcpStep->StepTimeCompen_nOpt != 0) bSubItemEnable = TRUE;
	if(pRcpStep->StgOffs1_nOpt > 0)  bSubItemEnable = TRUE;
	if(pRcpStep->StgOffs2_nOpt > 0)  bSubItemEnable = TRUE;

	for(i=0; i<MAX_MFC_DISPLAY; i++)
	{
		if(pRcpStep->dbMFC_Tolerance[i] > 0)
		{
			bSubItemEnable = TRUE;
			pRcpStep->bSubItemEnable = bSubItemEnable;
			break;
		}
	}

	pRcpStep->bSubItemEnable = bSubItemEnable;
}

void Save_Button_Enable_On_Main_Dlg( HWND hdlg )
{
	HWND	hParent	=	NULL;
	hParent = GetParent( hdlg );
	FILE_HANDLING_BUTTON_MAPPING( hParent , BUTTON_SAVE , 1 );
}

void Item_Draw_Int( HWND hdlg, int dControl, int dRes )
{
	KWIN_Item_Int_Display( hdlg, dControl, dRes );
}

void Item_Draw_Enum( HWND hdlg, int dControl, int dRes, char *szEnum)
{
	int i;
	char szValue[40];

	tpSetText(szEnum);
	for(i=0; i<=dRes; i++) tpGetWord(szValue, '|');

	KWIN_Item_String_Display( hdlg, dControl, szValue );
}


BOOL IsMfcTolSetting(int nCtrl, int *pnIdx)
{
	BOOL bRet = FALSE;
	int i;
	for(i=0; i<MAX_MFC_DISPLAY; i++)
	{
		if(g_anMfcRcID[i][MFC_TOL_ID] == nCtrl)
		{
			*pnIdx = i;
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

BOOL Item_Set_MfcTol(HWND hdlg, int nIdx, double *pdbValue)
{
	double	dbRes;
	double  dbMin, dbMax;
	char szTitle[40];

	extern void Save_Button_Enable_On_Main_Dlg(HWND hdlg );

	// Data Set
	dbRes = *pdbValue;
	dbMin = 0;
	dbMax = g_MFCData[nIdx].dbMax;

	sprintf(szTitle, "%s Tolerance", g_MFCData[nIdx].szMFCDisplayName);

	if ( MODAL_ANALOG_BOX1( hdlg, "Select", szTitle, dbMin, dbMax, 1, &dbRes ))
	{
		char buffer[128] = {0, };

		if (*pdbValue != dbRes)
		{
			sprintf(buffer, "\t[Sub Item] MFC %s - From [%f] to [%f]\n", szTitle, *pdbValue, dbRes);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE( TRUE );
		Save_Button_Enable_On_Main_Dlg( hdlg );
		*pdbValue = dbRes;
		KWIN_Item_Double_Display( hdlg, g_anMfcRcID[nIdx][MFC_TOL_ID], dbRes, 1);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL Item_Set_StepFlow( HWND hdlg, int dControl, int *pnValue)
{
	int dRes;
	char szTitle[40];
	char szOptEnum[256];

	dRes = *pnValue;
	sprintf(szTitle, "Step Flow Option");

	//... 2015.03.07 Check Not Use Flow Control Option in Process Type Recipe
	if(D_RECIPE0.udRcpType == RCP_PRC)
	{
		*pnValue = 0;	//... Default Normal Cycle
		return TRUE;
	}

	if(D_RECIPE0.udRcpType == RCP_IDL)
	{
		strcpy(szOptEnum, g_szStepFlowEnum2);
	}
	else
	{
		strcpy(szOptEnum, g_szStepFlowEnum1);
	}

	if ( MODAL_DIGITAL_BOX1( hdlg, "Select", szTitle, szOptEnum, &dRes ) )
	{
		char buffer[128] = {0, };

		if (*pnValue != dRes)
		{
			if(D_RECIPE0.udRcpType == RCP_IDL)
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_szStepFlowEnum2_arr[*pnValue], g_szStepFlowEnum2_arr[dRes]);
			}
			else
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_szStepFlowEnum1_arr[*pnValue], g_szStepFlowEnum1_arr[dRes]);
			}

			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE( TRUE );
		Save_Button_Enable_On_Main_Dlg( hdlg );
		if(D_RECIPE0.udRcpType != RCP_IDL && dRes == 2) dRes = 0;
		*pnValue = dRes;
		Item_Draw_Enum( hdlg, dControl, dRes, szOptEnum);
	}
	return TRUE;
}

BOOL Item_Set_Option( HWND hdlg, int dControl, int *pnValue, ITEM_STRUCT *pItemStruct)
{
	int dRes;
	char szTitle[40];

	dRes = *pnValue;
	sprintf(szTitle, "%s Option", pItemStruct->szName);

	if ( MODAL_DIGITAL_BOX1( hdlg, "Select", szTitle, pItemStruct->szOptEnum, &dRes ) )
	{
		char buffer[128] = {0, };

		if (*pnValue != dRes)
		{
			if(!strcmp(pItemStruct->szName,"Stage Heater"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_StgHtrEnum[*pnValue], g_StgHtrEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Stage Heater Ramping"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_SetNoSetOptEnum[*pnValue], g_SetNoSetOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Stage Temp Offset( C)"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_SetNoSetOptEnum[*pnValue], g_SetNoSetOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Stage Temp Offset2( C)"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_StgHtrOffs2Enum[*pnValue], g_StgHtrOffs2Enum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Step Time Compensation"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_Step_Time_CompenEnum[*pnValue], g_Step_Time_CompenEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Step Time Compen Preventive"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_Step_Time_Compen_PrevEnum[*pnValue], g_Step_Time_Compen_PrevEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"RF Position Preset Type"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_RFLdPrstOptEnum[*pnValue], g_RFLdPrstOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"RF2 Position Preset Type"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_RFLdPrstOptEnum[*pnValue], g_RFLdPrstOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"RF Tune Position Preset"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_SetNoSetOptEnum[*pnValue], g_SetNoSetOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Rough Valve"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_RoughValveEnum[*pnValue], g_RoughValveEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"RF Mode"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_PrstModeOptEnum[*pnValue], g_PrstModeOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"RF2 Mode"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_PrstModeOptEnum[*pnValue], g_PrstModeOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"RF Preset Mode"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_PrstSelModeOptEnum[*pnValue], g_PrstSelModeOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"RF2 Preset Mode"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_PrstSelModeOptEnum[*pnValue], g_PrstSelModeOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Heat Exchange"))
			{
				sprintf(buffer, "\t[Sub Item] %s - From [%s] to [%s]\n", szTitle, g_SetNoSetOptEnum[*pnValue], g_SetNoSetOptEnum[dRes]);
			}
			else if(!strcmp(pItemStruct->szName,"Heater"))
			{
				sprintf(buffer, "\t[Heater Item] %s - From [%s] to [%s]\n", szTitle, g_SetNoSetOptEnum[*pnValue], g_SetNoSetOptEnum[dRes]);
			}
			
			SampleLogSave(buffer, "Item");
		}
		
		RECIPE_CONTROL_Set_FILE_CHANGE( TRUE );
		Save_Button_Enable_On_Main_Dlg( hdlg );
		*pnValue = dRes;
		Item_Draw_Enum( hdlg, dControl, dRes, pItemStruct->szOptEnum);
	}
	return TRUE;
}

BOOL Item_Set_Point( HWND hdlg, int dControl, double *pdbValue, ITEM_STRUCT *pItemStruct)
{
	double	dbRes;
	double  dbMin, dbMax;
	char szTitle[40];
	// Data Set

	dbRes = *pdbValue;
	dbMin = pItemStruct->dbMin;
	dbMax = pItemStruct->dbMax;

	sprintf(szTitle, "%s Set Point", pItemStruct->szName);
	if ( MODAL_ANALOG_BOX1( hdlg, "Select", szTitle, dbMin, dbMax, 0, &dbRes ))
	{
		char buffer[128] = {0, };

		if (*pdbValue != dbRes)
		{
			sprintf(buffer, "\t[Sub Item] %s - From [%f] to [%f]\n", szTitle, *pdbValue, dbRes);
			SampleLogSave(buffer, "Item");
		}
		
		RECIPE_CONTROL_Set_FILE_CHANGE( TRUE );
		Save_Button_Enable_On_Main_Dlg( hdlg );
		*pdbValue = dbRes;
		Item_Draw_Int( hdlg, dControl, (int) dbRes);
	}
	return TRUE;
}
//... 2016.02.29
BOOL Item_HeaterSet_Point( HWND hdlg, int dControl, double *pdbValue, ITEM_STRUCT *pItemStruct)
{
	double	dbRes;
	double  dbSetPoint = 0;
	double  dbMin, dbMax;
	char szTitle[40];

	dbRes = *pdbValue;
	dbMin = pItemStruct->dbMin;
	dbMax = pItemStruct->dbMax;

	sprintf(szTitle, "%s Set Point", pItemStruct->szName);
	if ( MODAL_ANALOG_BOX1( hdlg, "Select", szTitle, dbMin, dbMax, 1, &dbRes ))
	{
		char buffer[128] = {0, };

		if (*pdbValue != dbRes)
		{
			sprintf(buffer, "\t[Heater Item] %s - From [%f] to [%f]\n", szTitle, *pdbValue, dbRes);
			SampleLogSave(buffer, "Item");
		}	
		
		RECIPE_CONTROL_Set_FILE_CHANGE( TRUE );
		Save_Button_Enable_On_Main_Dlg( hdlg );
		*pdbValue = dbRes;
		KWIN_Item_Double_Display( hdlg, dControl, dbRes, 1);
	}
	return TRUE;
}


// 2005.12.28
BOOL Item_Set_Point2( HWND hdlg, int dControl, double *pdbValue, ITEM_STRUCT *pItemStruct)
{
	double	dbRes;
	double  dbMin, dbMax;
	char szTitle[40];
	// Data Set

	dbRes = *pdbValue;
	dbMin = pItemStruct->dbMin;
	dbMax = pItemStruct->dbMax;

	sprintf(szTitle, "%s Set Point", pItemStruct->szName);
	if ( MODAL_ANALOG_BOX1( hdlg, "Select", szTitle, dbMin, dbMax, pItemStruct->nDecPoint, &dbRes ))
	{
		char buffer[128] = {0, };

		if (*pdbValue != dbRes)
		{
			sprintf(buffer, "\t[Sub Item] %s - From [%f] to [%f]\n", szTitle, *pdbValue, dbRes);
			SampleLogSave(buffer, "Item");
		}	
		
		RECIPE_CONTROL_Set_FILE_CHANGE( TRUE );
		Save_Button_Enable_On_Main_Dlg( hdlg );
		*pdbValue = dbRes;
		KWIN_Item_Double_Display( hdlg, dControl, dbRes, pItemStruct->nDecPoint);
	}
	return TRUE;
}

Display_SubItem_Information( HWND hdlg )
{
	KWIN_Item_Int_Display( hdlg, IDC_PRCSTEP,  g_ProcessStep );
}

void DRAW_SubItem_Column( HWND hdlg )
{
	extern BOOL g_bRF;
	extern BOOL g_bHTE_USE; //2014.04.03
	char szOptEnum[256];

	int i;
	for(i=0; i<MAX_MFC_DISPLAY; i++)
	{
		if(g_MFCData[i].bUse)
		{
			KWIN_Item_String_Display(hdlg, g_anMfcRcID[i][MFC_NAME_ID], g_MFCData[i].szMFCDisplayName);
			KWIN_Item_Double_Display( hdlg, g_anMfcRcID[i][MFC_TOL_ID], pSubItem_D_RECIPE->dbMFC_Tolerance[i], 1);
		}
		else
		{
			KWIN_Item_Hide(hdlg, g_anMfcRcID[i][MFC_TOL_ID]);
			KWIN_Item_Hide(hdlg, g_anMfcRcID[i][MFC_NAME_ID]);
		}
	}

	// Stage Heater 1
 	Item_Draw_Int( hdlg, IDC_BTN_STGHTR1_SP, (int) pSubItem_D_RECIPE->dbStgHtr1_SetPoint);
	Item_Draw_Enum( hdlg, IDC_BTN_STGHTR1_OPT, pSubItem_D_RECIPE->nStgHtr1_Option, g_StgHtr.szOptEnum);
	Item_Draw_Int( hdlg, IDC_BTN_RAMP1_SP, (int) pSubItem_D_RECIPE->dbRamp1_SetPoint);
	Item_Draw_Enum( hdlg, IDC_BTN_RAMP1_OPT, (int) pSubItem_D_RECIPE->nRamp1_Option, g_StgHtrRamp.szOptEnum);


	// Stage Heater 2
	Item_Draw_Int( hdlg, IDC_BTN_STGHTR2_SP, (int) pSubItem_D_RECIPE->dbStgHtr2_SetPoint);
	Item_Draw_Enum( hdlg, IDC_BTN_STGHTR2_OPT, pSubItem_D_RECIPE->nStgHtr2_Option, g_StgHtr.szOptEnum);
	Item_Draw_Int( hdlg, IDC_BTN_RAMP2_SP, (int) pSubItem_D_RECIPE->dbRamp2_SetPoint);
	Item_Draw_Enum( hdlg, IDC_BTN_RAMP2_OPT, (int) pSubItem_D_RECIPE->nRamp2_Option, g_StgHtrRamp.szOptEnum);

	// Item_Draw_Int( hdlg, IDC_BTN_STGHTR_TOL, (int) pSubItem_D_RECIPE->dbStgHtr_Tol);
	// KWIN_Item_Hide(hdlg, IDC_BTN_STGHTR_TOL);

	if(g_bStgOffs_Use)
	{
		Item_Draw_Int( hdlg, IDC_BTN_STGOFFS1_SP, (int) pSubItem_D_RECIPE->StgOffs1_dbSp);
		if(pSubItem_D_RECIPE->nStgHtr1_Option == 1)
			Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS1_OPT, (int) pSubItem_D_RECIPE->StgOffs1_nOpt, g_StgHtrOffs2.szOptEnum);
		else Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS1_OPT, (int) pSubItem_D_RECIPE->StgOffs1_nOpt, g_StgHtrOffs.szOptEnum);

		Item_Draw_Int( hdlg, IDC_BTN_STGOFFS2_SP, (int) pSubItem_D_RECIPE->StgOffs2_dbSp);
		if(pSubItem_D_RECIPE->nStgHtr2_Option == 1)
			Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS2_OPT, (int) pSubItem_D_RECIPE->StgOffs2_nOpt, g_StgHtrOffs2.szOptEnum);
		else Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS2_OPT, (int) pSubItem_D_RECIPE->StgOffs2_nOpt, g_StgHtrOffs.szOptEnum);
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_BTN_STGOFFS1_SP);
		KWIN_Item_Hide(hdlg, IDC_BTN_STGOFFS1_OPT);
		KWIN_Item_Hide(hdlg, IDC_BTN_STGOFFS2_SP);
		KWIN_Item_Hide(hdlg, IDC_BTN_STGOFFS2_OPT);
	}

	// 2005.12.28
	if(g_bStep_Time_Compen_Use)
	{
		if(! IsOneLineStep(pSubItem_D_RECIPE)) pSubItem_D_RECIPE->StepTimeCompen_nOpt = 0;
		Item_Draw_Enum( hdlg, IDC_BTN_STEP_TM_CMP_OPT, (int) pSubItem_D_RECIPE->StepTimeCompen_nOpt, g_Step_Time_Compen.szOptEnum);
		KWIN_Item_Double_Display( hdlg, IDC_BTN_STEP_TM_CMP_RATE, pSubItem_D_RECIPE->StepTimeCompen_dbRate, g_Step_Time_Compen_Rate.nDecPoint);
		Item_Draw_Enum( hdlg, IDC_BTN_STEP_TM_CMP_PREV, (int) pSubItem_D_RECIPE->StepTimeCompen_nPrev, g_Step_Time_Compen_Prev.szOptEnum);
		KWIN_Item_Double_Display( hdlg, IDC_BTN_STEP_TM_CMP_STD_T, pSubItem_D_RECIPE->udRCP_Valve_Data.RCP_DATA[0][0].dbTime, g_Step_Standard_Time.nDecPoint);
		KWIN_Item_Double_Display( hdlg, IDC_BTN_STEP_TM_CMP_MAX, pSubItem_D_RECIPE->StepTimeCompen_dbMaxTime, g_Step_Standard_Time.nDecPoint);
		Item_Draw_Int( hdlg, IDC_BTN_STEP_CYC_CMP_STD_C, (int) pSubItem_D_RECIPE->dValv_Rcp_Cycle_Cnt);
		KWIN_Item_Double_Display( hdlg, IDC_BTN_STEP_CYC_CMP_MAX, pSubItem_D_RECIPE->StepCycleCompen_dbMaxCycle,g_Step_Standard_Cycle.nDecPoint);

		//2007.12.06
		//Item_Draw_Enum(hdlg , IDC_BTN_CYC_TYPE , (int) pSubItem_D_RECIPE->nCleanCycType , g_Clean_Cycle_Compen.szOptEnum);
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_BTN_STEP_TM_CMP_OPT);
		KWIN_Item_Hide(hdlg, IDC_BTN_STEP_TM_CMP_RATE);
		KWIN_Item_Hide(hdlg, IDC_BTN_STEP_TM_CMP_PREV);
		KWIN_Item_Hide(hdlg, IDC_BTN_STEP_TM_CMP_STD_T);
		KWIN_Item_Hide(hdlg, IDC_BTN_STEP_TM_CMP_MAX);
		KWIN_Item_Hide(hdlg, IDC_BTN_STEP_CYC_CMP_STD_C);
		KWIN_Item_Hide(hdlg, IDC_BTN_STEP_CYC_CMP_MAX);
	}

	Item_Draw_Int( hdlg, IDC_BTN_STGHTR_TMOUT, (int) pSubItem_D_RECIPE->dbStgHtr_Timeout);

	// RF Generator
	if(g_bRF)
	{
		Item_Draw_Int( hdlg, IDC_BTN_LD_PRST_SP, (int) pSubItem_D_RECIPE->dbRFLdPrst_SetPoint);
		Item_Draw_Enum( hdlg, IDC_BTN_LD_PRST_OPT, pSubItem_D_RECIPE->nRFLdPrst_Option, g_RFLdPrst.szOptEnum);
		Item_Draw_Int( hdlg, IDC_BTN_TN_PRST_SP, (int) pSubItem_D_RECIPE->dbRFTnPrst_SetPoint);
		Item_Draw_Enum( hdlg, IDC_BTN_TN_PRST_OPT, pSubItem_D_RECIPE->nRFTnPrst_Option, g_RFTnPrst.szOptEnum);
		Item_Draw_Enum( hdlg, IDC_BTN_LD_PRST_OPT2, pSubItem_D_RECIPE->nRF2LdPrst_Option, g_RF2LdPrst.szOptEnum);
		Item_Draw_Enum( hdlg, IDC_BTN_PRST1MODE, pSubItem_D_RECIPE->nPrstMode_Option, g_PrstMode.szOptEnum);
		Item_Draw_Enum( hdlg, IDC_BTN_PRST2MODE, pSubItem_D_RECIPE->nPrst2Mode_Option, g_Prst2Mode.szOptEnum);
		Item_Draw_Enum( hdlg, IDC_BTN_PRST1SEL, pSubItem_D_RECIPE->nPrstSelMode_Option, g_PrstSelMode.szOptEnum);
		Item_Draw_Enum( hdlg, IDC_BTN_PRST2SEL, pSubItem_D_RECIPE->nPrst2SelMode_Option, g_PrstSel2Mode.szOptEnum);
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_BTN_LD_PRST_SP);
		KWIN_Item_Hide(hdlg, IDC_BTN_LD_PRST_OPT);
		KWIN_Item_Hide(hdlg, IDC_BTN_TN_PRST_SP);
		KWIN_Item_Hide(hdlg, IDC_BTN_TN_PRST_OPT);
		KWIN_Item_Hide(hdlg, IDC_BTN_LD_PRST_OPT2);
		KWIN_Item_Hide(hdlg, IDC_BTN_PRST1MODE);
		KWIN_Item_Hide(hdlg, IDC_BTN_PRST2MODE);
		KWIN_Item_Hide(hdlg, IDC_BTN_PRST1SEL);
		KWIN_Item_Hide(hdlg, IDC_BTN_PRST2SEL);

	}
	//2014.04.03
	if(g_bHTE_USE){
		Item_Draw_Int( hdlg, IDC_BTN_HTE_SP, (int) pSubItem_D_RECIPE->dbHTE_SetPoint);
		Item_Draw_Enum( hdlg, IDC_BTN_HTE_OPT, pSubItem_D_RECIPE->nHTE_Option, g_HeatChg.szOptEnum);

	}else{
		KWIN_Item_Hide(hdlg, IDC_BTN_HTE_SP);
		KWIN_Item_Hide(hdlg, IDC_BTN_HTE_OPT);

	}

	if(D_RECIPE0.udRcpType == RCP_IDL) strcpy(szOptEnum, g_szStepFlowEnum2);
	else strcpy(szOptEnum, g_szStepFlowEnum1);
	Item_Draw_Enum( hdlg, IDC_BTN_STEP_FLOW_OPT, pSubItem_D_RECIPE->nStepFlow_Option, szOptEnum);

	if(g_bRoughValve)
		Item_Draw_Enum( hdlg, IDC_BUTTON_ITEM_ROUGH_VLV_SP, pSubItem_D_RECIPE->nRoughValve, g_RoughValve.szOptEnum);
	else KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEM_ROUGH_VLV_SP);

}

BOOL APIENTRY SubItem_Msg_Handler( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam )
{
	HDC		hDC;
	PAINTSTRUCT	ps;
	int nIdx;

	switch ( msg )
	{
		case WM_INITDIALOG :

			//------ 2005.12.13 by ycchoi --------->>
			HWnd_SubItemRecipe = hdlg;
#ifdef _DEBUG
			_IO_CIM_PRINTF("SubItem Window Handle [%d]\n", HWnd_SubItemRecipe);
#endif
			//------ 2005.12.13 by ycchoi ---------<<

			 Init_SubItemConfig();
			 KGUI_STANDARD_Set_Function_Size( hdlg );
//			 Print_All_Elements();
 			 Display_SubItem_Information( hdlg );
			 DRAW_SubItem_Column( hdlg );
			 return FALSE;

		case WM_PAINT :
			 hDC = BeginPaint( hdlg, &ps );
			 // KWIN_DRAW_BACK_PAINT( KGUI_s_Get_GLIB_BACKGROUND() , ps , "IDB_SUB_BACKGRD" );
			 EndPaint( hdlg , &ps );
			// 2005.10.21
			ReleaseDC( hdlg , hDC );
			return TRUE;

		case WM_COMMAND :
			 switch( wParam )
			 {
				case IDC_OK		:
					//------ 2005.12.13 by ycchoi --------->>
					HWnd_SubItemRecipe = NULL;
#ifdef _DEBUG
	_IO_CIM_PRINTF("SubItem Window Close With OK[HWnd_SubItemRecipe is NULL]\n");
#endif
					//------ 2005.12.13 by ycchoi ---------<<

					EndDialog( hdlg, 1 );	CheckSubItemEnable(pSubItem_D_RECIPE); return TRUE;

				case IDC_CANCEL :
					//------ 2005.12.13 by ycchoi --------->>
					HWnd_SubItemRecipe = NULL;
#ifdef _DEBUG
	_IO_CIM_PRINTF("SubItem Window Close With CANCEL[HWnd_SubItemRecipe is NULL]\n");
#endif
					//------ 2005.12.13 by ycchoi ---------<<

					EndDialog( hdlg, 0 );	CheckSubItemEnable(pSubItem_D_RECIPE); return FALSE;

				case IDC_BTN_STGHTR1_SP :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbStgHtr1_SetPoint, &g_StgHtr );
											return FALSE;
				case IDC_BTN_STGHTR2_SP :Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbStgHtr2_SetPoint, &g_StgHtr );
											return FALSE;
				case IDC_BTN_STGHTR1_OPT :Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nStgHtr1_Option, &g_StgHtr );
											Check_RecipeStep_Data_Err(pSubItem_D_RECIPE);
											if(g_bStgOffs_Use)
											{
												if(pSubItem_D_RECIPE->nStgHtr1_Option == 1)
													Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS1_OPT, (int) pSubItem_D_RECIPE->StgOffs1_nOpt, g_StgHtrOffs2.szOptEnum);
												else Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS1_OPT, (int) pSubItem_D_RECIPE->StgOffs1_nOpt, g_StgHtrOffs.szOptEnum);
											}
											return FALSE;
				case IDC_BTN_STGHTR2_OPT :Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nStgHtr2_Option, &g_StgHtr );
											Check_RecipeStep_Data_Err(pSubItem_D_RECIPE);
											if(g_bStgOffs_Use)
											{
												if(pSubItem_D_RECIPE->nStgHtr2_Option == 1)
													Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS2_OPT, (int) pSubItem_D_RECIPE->StgOffs2_nOpt, g_StgHtrOffs2.szOptEnum);
												else Item_Draw_Enum( hdlg, IDC_BTN_STGOFFS2_OPT, (int) pSubItem_D_RECIPE->StgOffs2_nOpt, g_StgHtrOffs.szOptEnum);
											}
											return FALSE;
				case IDC_BTN_RAMP1_SP :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbRamp1_SetPoint, &g_StgHtrRamp );
											return FALSE;
				case IDC_BTN_RAMP2_SP :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbRamp2_SetPoint, &g_StgHtrRamp );
											return FALSE;
				case IDC_BTN_RAMP1_OPT :	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nRamp1_Option, &g_StgHtrRamp );
											return FALSE;

				case IDC_BTN_RAMP2_OPT :	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nRamp2_Option, &g_StgHtrRamp );
											return FALSE;
				// 2005.12.26
				case IDC_BTN_STGOFFS1_SP :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->StgOffs1_dbSp, &g_StgHtrOffs );
											return FALSE;

				case IDC_BTN_STGOFFS2_SP :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->StgOffs2_dbSp, &g_StgHtrOffs );
											return FALSE;
				case IDC_BTN_STGOFFS1_OPT :	if(pSubItem_D_RECIPE->nStgHtr1_Option == 2) Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->StgOffs1_nOpt, &g_StgHtrOffs );
											else if(pSubItem_D_RECIPE->nStgHtr1_Option == 1) Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->StgOffs1_nOpt, &g_StgHtrOffs2 );
											return FALSE;
				case IDC_BTN_STGOFFS2_OPT :	if(pSubItem_D_RECIPE->nStgHtr2_Option == 2) Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->StgOffs2_nOpt, &g_StgHtrOffs );
											else if(pSubItem_D_RECIPE->nStgHtr2_Option == 1) Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->StgOffs2_nOpt, &g_StgHtrOffs2 );
											return FALSE;
				case IDC_BTN_STEP_TM_CMP_OPT : if(IsOneLineStep(pSubItem_D_RECIPE)) Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->StepTimeCompen_nOpt, &g_Step_Time_Compen );
											return FALSE;
				case IDC_BTN_STEP_TM_CMP_RATE :	Item_Set_Point2( hdlg, wParam, &pSubItem_D_RECIPE->StepTimeCompen_dbRate, &g_Step_Time_Compen_Rate);
											return FALSE;
				case IDC_BTN_STEP_TM_CMP_PREV :	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->StepTimeCompen_nPrev, &g_Step_Time_Compen_Prev );
											return FALSE;

				// 2006.05.12
				case IDC_BTN_STEP_TM_CMP_MAX :	Item_Set_Point2( hdlg, wParam, &pSubItem_D_RECIPE->StepTimeCompen_dbMaxTime, &g_Step_Standard_Time);
											return FALSE;
				case IDC_BTN_STEP_CYC_CMP_MAX :	Item_Set_Point2( hdlg, wParam, &pSubItem_D_RECIPE->StepCycleCompen_dbMaxCycle, &g_Step_Standard_Cycle);
											return FALSE;

				// case IDC_BTN_STGHTR_TOL :	Item_Set_Point( hdlg, IDC_BTN_STGHTR_TOL, &pSubItem_D_RECIPE->dbStgHtr_Tol, &g_StgHtrTol );
											return FALSE;
				case IDC_BTN_STGHTR_TMOUT :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbStgHtr_Timeout, &g_StgHtrTmOut );
											return FALSE;
				case IDC_BTN_LD_PRST_SP :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbRFLdPrst_SetPoint, &g_RFLdPrst );
											return FALSE;
				case IDC_BTN_LD_PRST_OPT :	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nRFLdPrst_Option, &g_RFLdPrst );
											return FALSE;
				case IDC_BTN_TN_PRST_SP :	Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbRFTnPrst_SetPoint, &g_RFTnPrst );
											return FALSE;
				case IDC_BTN_TN_PRST_OPT :	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nRFTnPrst_Option, &g_RFTnPrst );
											return FALSE;
				case IDC_BTN_LD_PRST_OPT2 :	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nRF2LdPrst_Option, &g_RF2LdPrst );
											return FALSE;
				case IDC_BTN_STEP_FLOW_OPT : Item_Set_StepFlow( hdlg, wParam, &pSubItem_D_RECIPE->nStepFlow_Option);
											return FALSE;
				case IDC_BUTTON_ITEM_ROUGH_VLV_SP : Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nRoughValve, &g_RoughValve );
											return FALSE;
				//2014.05.22
				case IDC_BTN_PRST1MODE	:	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nPrstMode_Option, &g_PrstMode );
											return FALSE;
				case IDC_BTN_PRST2MODE	:	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nPrst2Mode_Option, &g_Prst2Mode );
											return FALSE;
				case IDC_BTN_PRST1SEL	:	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nPrstSelMode_Option, &g_PrstSelMode );
											return FALSE;
				case IDC_BTN_PRST2SEL	:	Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nPrst2SelMode_Option, &g_PrstSel2Mode );
											return FALSE;
				//2014.04.03
				case IDC_BTN_HTE_SP : Item_Set_Point( hdlg, wParam, &pSubItem_D_RECIPE->dbHTE_SetPoint, &g_HeatChg );
											return FALSE;
				case IDC_BTN_HTE_OPT : Item_Set_Option( hdlg, wParam, &pSubItem_D_RECIPE->nHTE_Option, &g_HeatChg );
											return FALSE;

				default:
					if(IsMfcTolSetting(wParam, &nIdx))
					{
						Item_Set_MfcTol(hdlg, nIdx, &pSubItem_D_RECIPE->dbMFC_Tolerance[nIdx]);
					}
					break;
			 }
			 return FALSE;
	}
	return FALSE;
}
