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

BOOL g_bEditOk = FALSE;
BOOL g_bRF = FALSE;
BOOL g_bHasValveData = FALSE;
BOOL g_bHasValveData_DMY = FALSE;
BOOL g_bCopyBuffer = FALSE;

int g_VScroll_Pos = 0;
int g_HScroll_Pos = 0;
int g_Val_Current_Step = 1;
int g_Val_Total_Step = 1;
int g_ProcessStep = 0;
int g_Total_Valve_No = MAX_VAL_NO;

double g_dbTimeMap[MAX_VAL_TIME_STEP];
VALVE_RCP_DATA udValve_map;
VALVE_RCP_DATA udValve_map_DMY;

VAL_STATUS RCP_VAL_CFG[MAX_VAL_NO];
VAL_STATUS PRE_VAL_CFG[MAX_VAL_NO];
VAL_STATUS PST_VAL_CFG[MAX_VAL_NO];
VAL_STATUS IDL_VAL_CFG[MAX_VAL_NO];
VAL_STATUS CLN_VAL_CFG[MAX_VAL_NO];
VAL_STATUS PUG_VAL_CFG[MAX_VAL_NO];
VAL_STATUS LPR_VAL_CFG[MAX_VAL_NO];
VAL_STATUS LPS_VAL_CFG[MAX_VAL_NO];

// Valve Mask3 2010.02.03
VAL_STATUS RCP_VAL_CFG_ADJ[MAX_VAL_NO];
VAL_STATUS PRE_VAL_CFG_ADJ[MAX_VAL_NO];
VAL_STATUS PST_VAL_CFG_ADJ[MAX_VAL_NO];
VAL_STATUS IDL_VAL_CFG_ADJ[MAX_VAL_NO];
VAL_STATUS CLN_VAL_CFG_ADJ[MAX_VAL_NO];
VAL_STATUS PUG_VAL_CFG_ADJ[MAX_VAL_NO];
VAL_STATUS LPR_VAL_CFG_ADJ[MAX_VAL_NO];
VAL_STATUS LPS_VAL_CFG_ADJ[MAX_VAL_NO];

VAL_DATA udCopyBuffer[MAX_VAL_TIME_STEP];
VAL_DATA udCopyBuffer_DMY[MAX_VAL_TIME_STEP];
VALVE_RCP_DATA g_pRcpValveData_DMY_TMP;

WNDPROC Default_Valve_Proc;

extern void Draw_MFC_Contents(HWND hdlg);
extern void Draw_DMYMFC_Contents(HWND hdlg);
extern void Draw_MFCDMY_Title(HWND hdlg);
extern BOOL ITEM_BUTTON_MFC_SETPOINT(int nMfcNo, HWND hWnd, int i);
extern BOOL ITEM_BUTTON_MFC_OPTION(HWND hWnd, int nBtnID);

extern RecipeStepTemplateM D_RECIPE[MAX_STEP_COUNT];
extern RecipeStepTemplateM D_RECIPE_MFC[MAX_DISPLAY_COUNT];
extern RecipeStepTemplateM D_RECIPE_TMP[MAX_STEP_COUNT];
extern int g_MFC_SET_BTN_DMY[MAX_DISPLAY_COUNT][MAX_MFC_DISPLAY];
extern int FindIdx(int nValue, int pnArray[], int nCount);

extern RCP_TYPE g_RcpFileType;
extern VALVE_RCP_DATA *g_pRcpValveData;
extern VALVE_RCP_DATA *g_pRcpValveData_DMY;
extern HINSTANCE g_hInstance;
extern VAL_DATA g_Det_Valve_Data;
extern VAL_DATA g_Det_Valve_Data_DMY;
extern char g_szValveName[MAX_VAL_NO][MAX_VAL_NAME_LENGTH];

extern double g_dbValveTime_Min;
extern double g_dbValveTime_Max;
extern int g_nValveTime_DecPnt;

// 2005.03.02 by cskim
extern BOOL g_bValveStepCtrl;

// 2009.08.18 Added by jhlee
extern BOOL g_bDMY;
extern int g_bDMY_Step2;
// 2005.09.22 by cskim, for 112 Valve Recipe
extern int g_nTotalValveLineStep;

//------------ 2005.12.13 by ycchoi ----------->>
extern HWND hWnd_ValveRecipe;
extern HWND hWnd_DetailValueRecipe;
//------------ 2005.12.13 by ycchoi -----------<<

void Redraw_All_Valve(HWND hdlg);
void Redraw_All_Time_Value(HWND hdlg);
void Valve_Button_Show(HWND hdlg, BOOL bVisible);
BOOL APIENTRY Valve_Detail_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL Can_Change_Status(int dTarget_Valve, VAL_STATUS *pValveStatus, int *dStatus);
HINSTANCE KGUI_s_Get_GLIB_BACKGROUND(void);

void Edit_Button_Mapping(HWND hdlg)
{
#define BUTTON_X_START 16
#define BUTTON_X_INTERVAL 67
#define BUTTON_Y_START 440
#define BUTTON_Y_INTERVAL 67

#define BUTTON_X_INS_START 20
#define BUTTON_Y_INS_START 530

#define BUTTON_X_INS2_START 110
#define BUTTON_Y_INS2_START 530

#define BUTTON_X_REMOVE_START 200
#define BUTTON_Y_REMOVE_START 530

#define BUTTON_X_COPY_START 290
#define BUTTON_Y_COPY_START 530

#define BUTTON_X_EDIT_START 490
#define BUTTON_Y_EDIT_START 530

#define BUTTON_X_VAL_OK_START 730
#define BUTTON_Y_VAL_OK_START 530

#define BUTTON_X_VAL_CANCEL_START 820
#define BUTTON_Y_VAL_CANCEL_START 530

#define BUTTON_X_DMY_COPY_START 20
#define BUTTON_Y_DMY_COPY_START 530

	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_EDIT_START, BUTTON_Y_EDIT_START, 87, 38, hdlg, (HMENU)IDB_EDIT_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_EDIT_START, BUTTON_Y_EDIT_START, 87, 38, hdlg, (HMENU)IDB_EDIT_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_EDIT_START, BUTTON_Y_EDIT_START, 87, 38, hdlg, (HMENU)IDB_EDIT_PRESSED, GETHINST(hdlg), NULL);
	KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);

	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_VAL_OK_START, BUTTON_Y_VAL_OK_START, 87, 38, hdlg, (HMENU)IDB_VAL_OK_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_VAL_OK_START, BUTTON_Y_VAL_OK_START, 87, 38, hdlg, (HMENU)IDB_VAL_OK_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_VAL_OK_START, BUTTON_Y_VAL_OK_START, 87, 38, hdlg, (HMENU)IDB_VAL_OK_PRESSED, GETHINST(hdlg), NULL);
	KWIN_Item_Hide(hdlg, IDB_VAL_OK_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_VAL_OK_PRESSED);

	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_VAL_CANCEL_START, BUTTON_Y_VAL_CANCEL_START, 87, 38, hdlg, (HMENU)IDB_VAL_CANCEL_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_VAL_CANCEL_START, BUTTON_Y_VAL_CANCEL_START, 87, 38, hdlg, (HMENU)IDB_VAL_CANCEL_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_VAL_CANCEL_START, BUTTON_Y_VAL_CANCEL_START, 87, 38, hdlg, (HMENU)IDB_VAL_CANCEL_PRESSED, GETHINST(hdlg), NULL);
	KWIN_Item_Hide(hdlg, IDB_VAL_CANCEL_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_VAL_CANCEL_PRESSED);

	if (g_bDMY)
	{
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_DMY_COPY_START, BUTTON_Y_DMY_COPY_START, 87, 38, hdlg, (HMENU)IDB_DMY_COPY_ENABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_DMY_COPY_START, BUTTON_Y_DMY_COPY_START, 87, 38, hdlg, (HMENU)IDB_DMY_COPY_DISABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_DMY_COPY_START, BUTTON_Y_DMY_COPY_START, 87, 38, hdlg, (HMENU)IDB_DMY_COPY_PRESSED, GETHINST(hdlg), NULL);
		KWIN_Item_Hide(hdlg, IDB_DMY_COPY_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_DMY_COPY_PRESSED);
	}

	if (g_bValveStepCtrl)
	{
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS_START, BUTTON_Y_INS_START, 90, 40, hdlg, (HMENU)IDB_INSERT_ENABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS_START, BUTTON_Y_INS_START, 90, 40, hdlg, (HMENU)IDB_INSERT_DISABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS_START, BUTTON_Y_INS_START, 90, 40, hdlg, (HMENU)IDB_INSERT_PRESSED, GETHINST(hdlg), NULL);
		KWIN_Item_Hide(hdlg, IDB_INSERT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT_PRESSED);
		//
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS2_START, BUTTON_Y_INS2_START, 90, 40, hdlg, (HMENU)IDB_INSERT2_ENABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS2_START, BUTTON_Y_INS2_START, 90, 40, hdlg, (HMENU)IDB_INSERT2_DISABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS2_START, BUTTON_Y_INS2_START, 90, 40, hdlg, (HMENU)IDB_INSERT2_PRESSED, GETHINST(hdlg), NULL);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_PRESSED);
		//
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_REMOVE_START, BUTTON_Y_REMOVE_START, 90, 40, hdlg, (HMENU)IDB_REMOVE_ENABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_REMOVE_START, BUTTON_Y_REMOVE_START, 90, 40, hdlg, (HMENU)IDB_REMOVE_DISABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_REMOVE_START, BUTTON_Y_REMOVE_START, 90, 40, hdlg, (HMENU)IDB_REMOVE_PRESSED, GETHINST(hdlg), NULL);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_PRESSED);
		//
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_COPY_START, BUTTON_Y_COPY_START, 90, 40, hdlg, (HMENU)IDB_COPY_ENABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_COPY_START, BUTTON_Y_COPY_START, 90, 40, hdlg, (HMENU)IDB_COPY_DISABLE, GETHINST(hdlg), NULL);
		CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_COPY_START, BUTTON_Y_COPY_START, 90, 40, hdlg, (HMENU)IDB_COPY_PRESSED, GETHINST(hdlg), NULL);
		KWIN_Item_Hide(hdlg, IDB_COPY_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_COPY_PRESSED);
	}
	//
}

void Button_Initialize_Status(HWND hdlg)
{
	KWIN_Item_Hide(hdlg, IDB_REMOVE_ENABLE);
	KWIN_Item_Hide(hdlg, IDB_REMOVE_PRESSED);

	KWIN_Item_Hide(hdlg, IDB_COPY_ENABLE);
	KWIN_Item_Hide(hdlg, IDB_COPY_PRESSED);

	KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);

	KWIN_Item_Hide(hdlg, IDC_SCROLLBAR2);
	KWIN_Item_Hide(hdlg, IDC_SCROLLBAR3);

	if (g_bValveStepCtrl)
	{
		KWIN_Item_Show(hdlg, IDB_REMOVE_DISABLE);
		KWIN_Item_Show(hdlg, IDB_COPY_DISABLE);
		KWIN_Item_Show(hdlg, IDB_EDIT_ENABLE);
		KWIN_Item_Disable(hdlg, IDC_VALVE_STEP_CYC_COUNT);
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_VAL_STEP_LEFT);
		KWIN_Item_Hide(hdlg, IDC_VAL_STEP_RIGHT);

		KWIN_Item_Hide(hdlg, IDB_REMOVE_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_COPY_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_EDIT_ENABLE);
		KWIN_Item_Hide(hdlg, IDC_VALVE_STEP_CYC_COUNT);
	}
}

void Button_Status_Changed(HWND hdlg, int dMode)
{
	switch (dMode)
	{
	case EDIT_ON:
		KWIN_Item_Show(hdlg, IDB_EDIT_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);
		break;

	case EDIT_PRESS:
		KWIN_Item_Hide(hdlg, IDB_EDIT_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
		KWIN_Item_Show(hdlg, IDB_EDIT_PRESSED);
		break;

	case EDIT_OFF:
		KWIN_Item_Hide(hdlg, IDB_EDIT_ENABLE);
		KWIN_Item_Show(hdlg, IDB_EDIT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);
		break;

	case INSERT_ON:
		KWIN_Item_Show(hdlg, IDB_INSERT_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT_PRESSED);

		KWIN_Item_Show(hdlg, IDB_INSERT2_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_PRESSED);
		break;

	case INSERT_OFF:
		KWIN_Item_Hide(hdlg, IDB_INSERT_ENABLE);
		KWIN_Item_Show(hdlg, IDB_INSERT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT_PRESSED);

		KWIN_Item_Hide(hdlg, IDB_INSERT2_ENABLE);
		KWIN_Item_Show(hdlg, IDB_INSERT2_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_PRESSED);
		break;

	case REMOVE_ON:
		KWIN_Item_Show(hdlg, IDB_REMOVE_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_PRESSED);
		break;

	case REMOVE_PRESS:
		KWIN_Item_Hide(hdlg, IDB_REMOVE_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_DISABLE);
		KWIN_Item_Show(hdlg, IDB_REMOVE_PRESSED);
		break;

	case REMOVE_OFF:
		KWIN_Item_Hide(hdlg, IDB_REMOVE_ENABLE);
		KWIN_Item_Show(hdlg, IDB_REMOVE_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_PRESSED);
		break;

	case COPY_ON:
		KWIN_Item_Show(hdlg, IDB_COPY_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_COPY_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_COPY_PRESSED);
		break;

	case COPY_OFF:
		KWIN_Item_Hide(hdlg, IDB_COPY_ENABLE);
		KWIN_Item_Show(hdlg, IDB_COPY_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_COPY_PRESSED);
		break;

	case COPY_PRESS:
		KWIN_Item_Hide(hdlg, IDB_COPY_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_COPY_DISABLE);
		KWIN_Item_Show(hdlg, IDB_COPY_PRESSED);
		break;

	case STEP_MOVE_PREV_ON:
		KWIN_Item_Show(hdlg, IDC_VAL_STEP_LEFT);
		break;

	case STEP_MOVE_PREV_OFF:
		KWIN_Item_Disable(hdlg, IDC_VAL_STEP_LEFT);
		break;

	case STEP_MOVE_NEXT_ON:
		KWIN_Item_Show(hdlg, IDC_VAL_STEP_RIGHT);
		break;

	case STEP_MOVE_NEXT_OFF:
		KWIN_Item_Disable(hdlg, IDC_VAL_STEP_RIGHT);
		break;
	}
}

void Redraw_Valve_Information(HWND hdlg)
{
	if (g_bValveStepCtrl)
	{
		if (g_bDMY)
			KWIN_Item_Int_Display(hdlg, IDC_VALVE_STEP_CYC_COUNT, udValve_map_DMY.dValveStepCycleCount[g_Val_Current_Step - 1]);
		else
			KWIN_Item_Int_Display(hdlg, IDC_VALVE_STEP_CYC_COUNT, udValve_map.dValveStepCycleCount[g_Val_Current_Step - 1]);
		//	KWIN_Item_Int_Display( hdlg, IDC_PROCESS_STEP2, g_Val_Current_Step );
		//	KWIN_Item_Int_Display( hdlg, IDC_PROCESS_STEP3, g_Val_Total_Step );
	}
}

void Refresh_Data(HWND hdlg)
{
	if (g_Val_Total_Step > 0)
	{
		Button_Status_Changed(hdlg, REMOVE_ON);
		Button_Status_Changed(hdlg, COPY_ON);
	}

	Redraw_Valve_Information(hdlg);
	Redraw_All_Valve(hdlg);
	Redraw_All_Time_Value(hdlg);
}

// 2005.12.25
VAL_STATUS *GetValveMaskCfg()
{
	VAL_STATUS *pValMap = NULL;
	switch (g_RcpFileType)
	{
	case RCP_PRE:
		pValMap = PRE_VAL_CFG;
		break;
	case RCP_PST:
		pValMap = PST_VAL_CFG;
		break;
	case RCP_IDL:
		pValMap = IDL_VAL_CFG;
		break;
	case RCP_CLN:
		pValMap = CLN_VAL_CFG;
		break;
	case RCP_PUG:
		pValMap = PUG_VAL_CFG;
		break;
	case RCP_LPR:
		pValMap = LPR_VAL_CFG;
		break;
	case RCP_LPS:
		pValMap = LPS_VAL_CFG;
		break;
	default:
		pValMap = RCP_VAL_CFG;
		break;
	}
	return pValMap;
}

VAL_STATUS *GetValveMaskCfgAdj()
{
	int i = 0;
	VAL_STATUS *pValMap = NULL;
	switch (g_RcpFileType)
	{
	case RCP_PRE:
	{
		memcpy(&PRE_VAL_CFG_ADJ, &PRE_VAL_CFG, sizeof(PRE_VAL_CFG));
		pValMap = PRE_VAL_CFG_ADJ;
		break;
	}
	case RCP_PST:
	{
		memcpy(&PST_VAL_CFG_ADJ, &PST_VAL_CFG, sizeof(PST_VAL_CFG));
		pValMap = PST_VAL_CFG_ADJ;
		break;
	}
	case RCP_IDL:
	{
		memcpy(&IDL_VAL_CFG_ADJ, &IDL_VAL_CFG, sizeof(IDL_VAL_CFG));
		pValMap = IDL_VAL_CFG_ADJ;
		break;
	}
	case RCP_CLN:
	{
		memcpy(&CLN_VAL_CFG_ADJ, &CLN_VAL_CFG, sizeof(CLN_VAL_CFG));
		pValMap = CLN_VAL_CFG_ADJ;
		break;
	}
	case RCP_PUG:
	{
		memcpy(&PUG_VAL_CFG_ADJ, &PUG_VAL_CFG, sizeof(PUG_VAL_CFG));
		pValMap = PUG_VAL_CFG_ADJ;
		break;
	}
	case RCP_LPR:
	{
		memcpy(&LPR_VAL_CFG_ADJ, &LPR_VAL_CFG, sizeof(LPR_VAL_CFG));
		pValMap = LPR_VAL_CFG_ADJ;
		break;
	}
	case RCP_LPS:
	{
		memcpy(&LPS_VAL_CFG_ADJ, &LPS_VAL_CFG, sizeof(LPS_VAL_CFG));
		pValMap = LPS_VAL_CFG_ADJ;
		break;
	}
	default:
	{
		memcpy(&RCP_VAL_CFG_ADJ, &RCP_VAL_CFG, sizeof(RCP_VAL_CFG));
		pValMap = RCP_VAL_CFG_ADJ;
		break;
	}
	}

	for (i = 0; i < MAX_VAL_NO; i++)
	{
		if (pValMap[i] == 3)
		{
			pValMap[i] = 2;
		}
	}

	return pValMap;
}

void Data_Insert_Before(HWND hdlg, int dRes)
{
	int i;
	int dEndStep;
	int dStartStep;

	VAL_STATUS *pValMap = NULL;

	dEndStep = g_Val_Total_Step - 1;
	dStartStep = g_Val_Current_Step - 1;

	if (g_Val_Total_Step > 1)
	{
		for (i = dEndStep; i > dStartStep; i--)
		{
			udValve_map_DMY.dValveStepCycleCount[i] = udValve_map_DMY.dValveStepCycleCount[i - 1];
			memcpy(udValve_map_DMY.RCP_DATA[i], udValve_map_DMY.RCP_DATA[i - 1], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);

			udValve_map.dValveStepCycleCount[i] = udValve_map.dValveStepCycleCount[i - 1];
			memcpy(udValve_map.RCP_DATA[i], udValve_map.RCP_DATA[i - 1], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
		}
	}

	// Current Step <- Mask
	if (dRes)
	{
		switch (g_RcpFileType)
		{
			// 2010.02.03	Valve Mask3
		case RCP_PRE:
			pValMap = PRE_VAL_CFG_ADJ;
			break;
		case RCP_PST:
			pValMap = PST_VAL_CFG_ADJ;
			break;
		case RCP_IDL:
			pValMap = IDL_VAL_CFG_ADJ;
			break;
		case RCP_CLN:
			pValMap = CLN_VAL_CFG_ADJ;
			break;
		case RCP_PUG:
			pValMap = PUG_VAL_CFG_ADJ;
			break;
		case RCP_LPR:
			pValMap = LPR_VAL_CFG_ADJ;
			break;
		case RCP_LPS:
			pValMap = LPS_VAL_CFG_ADJ;
			break;
		default:
			pValMap = RCP_VAL_CFG_ADJ;
			g_RcpFileType = RCP_PRC;
			break;
		}

		for (i = 0; i < MAX_VAL_TIME_STEP; i++)
		{
			udValve_map_DMY.RCP_DATA[dStartStep][i].dbTime = 0;
			memcpy(udValve_map_DMY.RCP_DATA[dStartStep][i].RCP_DATA, pValMap, sizeof(VAL_STATUS) * MAX_VAL_NO);
			udValve_map.RCP_DATA[dStartStep][i].dbTime = 0;
			memcpy(udValve_map.RCP_DATA[dStartStep][i].RCP_DATA, pValMap, sizeof(VAL_STATUS) * MAX_VAL_NO);
		}

		// 2004-04-29  IPS 서병찬 CJ 요구사항
		// dValveStepCycleCount Setting
		udValve_map_DMY.dValveStepCycleCount[dStartStep] = 1;
		udValve_map.dValveStepCycleCount[dStartStep] = 1;
	}
	else
	{
		memcpy(udValve_map_DMY.RCP_DATA[dStartStep], udCopyBuffer_DMY, sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
		memcpy(udValve_map.RCP_DATA[dStartStep], udCopyBuffer, sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
	}
}

void Data_Insert_After(HWND hdlg, int dRes)
{
	int i;
	int dEndStep;
	int dStartStep;

	VAL_STATUS *pValMap = NULL;

	dEndStep = g_Val_Total_Step - 1;
	dStartStep = g_Val_Current_Step;

	if (g_Val_Total_Step > 1)
	{
		for (i = dEndStep; i > dStartStep; i--)
		{
			udValve_map_DMY.dValveStepCycleCount[i] = udValve_map_DMY.dValveStepCycleCount[i - 1];
			memcpy(udValve_map_DMY.RCP_DATA[i], udValve_map_DMY.RCP_DATA[i - 1], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
			udValve_map.dValveStepCycleCount[i] = udValve_map.dValveStepCycleCount[i - 1];
			memcpy(udValve_map.RCP_DATA[i], udValve_map.RCP_DATA[i - 1], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
		}
	}

	g_Val_Current_Step++;

	// Current Step <- Mask
	if (dRes)
	{
		switch (g_RcpFileType)
		{
			// Valve Mask3 2010.02.03
		case RCP_PRE:
			pValMap = PRE_VAL_CFG_ADJ;
			break;
		case RCP_PST:
			pValMap = PST_VAL_CFG_ADJ;
			break;
		case RCP_IDL:
			pValMap = IDL_VAL_CFG_ADJ;
			break;
		case RCP_CLN:
			pValMap = CLN_VAL_CFG_ADJ;
			break;
		case RCP_PUG:
			pValMap = PUG_VAL_CFG_ADJ;
			break;
		case RCP_LPR:
			pValMap = LPR_VAL_CFG_ADJ;
			break;
		case RCP_LPS:
			pValMap = LPS_VAL_CFG_ADJ;
			break;
		default:
			g_RcpFileType = RCP_PRC;
			pValMap = RCP_VAL_CFG_ADJ;
			break;
		}

		for (i = 0; i < MAX_VAL_TIME_STEP; i++)
		{
			udValve_map_DMY.RCP_DATA[dStartStep][i].dbTime = 0;
			memcpy(udValve_map_DMY.RCP_DATA[dStartStep][i].RCP_DATA, pValMap, sizeof(VAL_STATUS) * MAX_VAL_NO);
			udValve_map.RCP_DATA[dStartStep][i].dbTime = 0;
			memcpy(udValve_map.RCP_DATA[dStartStep][i].RCP_DATA, pValMap, sizeof(VAL_STATUS) * MAX_VAL_NO);
		}

		// 2004-04-29  IPS 서병찬 CJ 요구사항
		// dValveStepCycleCount Setting
		udValve_map_DMY.dValveStepCycleCount[dStartStep] = 1;
		udValve_map.dValveStepCycleCount[dStartStep] = 1;
	}
	else
	{
		memcpy(udValve_map_DMY.RCP_DATA[dStartStep], udCopyBuffer_DMY, sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
		memcpy(udValve_map.RCP_DATA[dStartStep], udCopyBuffer, sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
	}
}

BOOL Valve_Step_Insert(HWND hdlg, int dMode)
{
	int Res;

	printf("Valve_Step_Insert-----========================\n");
	if (g_Val_Total_Step >= MAX_VAL_STEP_NO)
		return FALSE;

	if (g_bCopyBuffer)
	{
		if (MODAL_DIGITAL_BOX1(hdlg, "Select", "Insert Data", "From Buffer|With Blank Data", &Res))
		{
			g_Val_Total_Step++;

			if (g_Val_Total_Step == 1)
				g_Val_Current_Step++;

			if (dMode == 0)
				Data_Insert_Before(hdlg, Res);
			else if (dMode == 1)
				Data_Insert_After(hdlg, Res);

			Redraw_Valve_Information(hdlg);
			Redraw_All_Valve(hdlg);
			Redraw_All_Time_Value(hdlg);
		}
	}
	else
	{
		g_Val_Total_Step++;

		if (dMode == 0)
		{
			if (g_Val_Total_Step == 1)
				g_Val_Current_Step++;

			Data_Insert_Before(hdlg, 1);
		}
		else if (dMode == 1)
		{
			Data_Insert_After(hdlg, 1);
		}

		Redraw_Valve_Information(hdlg);
		Redraw_All_Valve(hdlg);
		Redraw_All_Time_Value(hdlg);
	}

	Button_Status_Changed(hdlg, EDIT_ON);
	g_bEditOk = FALSE;

	return TRUE;
}

void On_Insert_Enable(HWND hdlg)
{
	BOOL bResult;

	// Button Status Change
	KWIN_Item_Enable(hdlg, IDB_INSERT_PRESSED);
	KWIN_Item_Hide(hdlg, IDB_INSERT_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_INSERT_ENABLE);

	bResult = Valve_Step_Insert(hdlg, 0);

	if (bResult)
	{
		Button_Status_Changed(hdlg, EDIT_ON);
		//		Redraw_All_Valve( hdlg );
		Refresh_Data(hdlg);
	}

	KWIN_Item_Enable(hdlg, IDB_INSERT_ENABLE);
	KWIN_Item_Hide(hdlg, IDB_INSERT_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_INSERT_PRESSED);
}

void On_Insert_Enable2(HWND hdlg)
{
	BOOL bResult;

	// Button Status Change
	KWIN_Item_Enable(hdlg, IDB_INSERT2_PRESSED);
	KWIN_Item_Hide(hdlg, IDB_INSERT2_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_INSERT2_ENABLE);

	bResult = Valve_Step_Insert(hdlg, 1);

	if (bResult)
	{
		Button_Status_Changed(hdlg, EDIT_ON);
		Refresh_Data(hdlg);
		//		Redraw_All_Valve( hdlg );
	}

	KWIN_Item_Enable(hdlg, IDB_INSERT2_ENABLE);
	KWIN_Item_Hide(hdlg, IDB_INSERT2_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_INSERT2_PRESSED);
}

void On_Remove_Enable(HWND hdlg)
{
	int i;
	int dStart;
	int dEnd;

	if (!g_bEditOk)
		return;

	Button_Status_Changed(hdlg, REMOVE_PRESS);

	dStart = g_Val_Current_Step;
	dEnd = g_Val_Total_Step;

	// 2004/04/09 thyun 변경
	if (MessageBox(hdlg, "Are you sure to delete!!", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		Button_Status_Changed(hdlg, REMOVE_ON);
		return;
	}

	for (i = dStart; i < dEnd; i++)
	{
		memcpy(udValve_map_DMY.RCP_DATA[i - 1], udValve_map_DMY.RCP_DATA[i], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
		memcpy(udValve_map.RCP_DATA[i - 1], udValve_map.RCP_DATA[i], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
	}

	// g_Val_Total_Step
	g_Val_Total_Step--;
	if (g_Val_Current_Step > g_Val_Total_Step)
		g_Val_Current_Step = g_Val_Total_Step;

	Button_Status_Changed(hdlg, REMOVE_ON);
	Refresh_Data(hdlg);

	if (g_Val_Total_Step == 0)
	{
		Button_Status_Changed(hdlg, REMOVE_OFF);
		Button_Status_Changed(hdlg, COPY_OFF);
		Valve_Button_Show(hdlg, FALSE);
	}
}

void On_Copy_Enable(HWND hdlg)
{
	int dStep = 0;

	if (!g_bEditOk)
		return;

	dStep = g_Val_Current_Step - 1;

	Button_Status_Changed(hdlg, COPY_PRESS);

	g_bCopyBuffer = TRUE;
	memcpy(udCopyBuffer_DMY, udValve_map_DMY.RCP_DATA[dStep], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);
	memcpy(udCopyBuffer, udValve_map.RCP_DATA[dStep], sizeof(VAL_DATA) * MAX_VAL_TIME_STEP);

	Button_Status_Changed(hdlg, COPY_ON);
}

void OnEditEnable(HWND hdlg)
{
	// Edit on -> Edit Off
	g_bEditOk = TRUE;
	KWIN_Item_Enable(hdlg, IDB_EDIT_PRESSED);
	KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_EDIT_ENABLE);
}

void OnEditPressed(HWND hdlg)
{
	g_bEditOk = FALSE;
	KWIN_Item_Enable(hdlg, IDB_EDIT_ENABLE);
	KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);
}

void OnOk(HWND hdlg)
{
	if (MessageBox(hdlg, "Are you sure to Set Valve Status!!", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	if (g_bDMY)
	{
		udValve_map_DMY.dTotalStep = g_Val_Total_Step;
		memcpy(g_pRcpValveData_DMY, &udValve_map_DMY, sizeof(VALVE_RCP_DATA));
	}
	else
	{
		udValve_map.dTotalStep = g_Val_Total_Step;
		memcpy(g_pRcpValveData, &udValve_map, sizeof(VALVE_RCP_DATA));
	}

	// 2004/04/09 thyun 추가
	g_bEditOk = FALSE;

	//------ 2005.12.13 by ycchoi --------->>
	hWnd_ValveRecipe = NULL;
#ifdef _DEBUG
	_IO_CIM_PRINTF("Valve Window Close With OK[hWnd_ValveRecipe is NULL]\n");
#endif
	//------ 2005.12.13 by ycchoi ---------<<

	EndDialog(hdlg, 1);
}

void Refresh_DMY_Valve_Data(HWND hdlg)
{
	Valve_Button_Show(hdlg, TRUE);
	Button_Status_Changed(hdlg, EDIT_ON);
	Button_Status_Changed(hdlg, COPY_ON);
	Button_Status_Changed(hdlg, REMOVE_ON);

	Redraw_All_Time_Value(hdlg);
}

void On_DMY_Recipe_Copy(HWND hdlg)
{
	int dMFC_Count, i, j;

	if (!g_bEditOk)
		return;

	if (MessageBox(hdlg, "Are you sure copy from main recipe(Valve, MFC)???", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	// copy from main recipe(MFC). 2009.08.21 Added by jhlee.
	for (dMFC_Count = 0; dMFC_Count < MFC_BUF_LENGTH; dMFC_Count++)
	{
		D_RECIPE_MFC[g_bDMY_Step2].dbMFC_Set_Point_DMY[dMFC_Count] = D_RECIPE[g_bDMY_Step2].dbMFC_Set_Point[dMFC_Count];
		D_RECIPE_MFC[g_bDMY_Step2].dMFC_Option_DMY[dMFC_Count] = D_RECIPE[g_bDMY_Step2].dMFC_Option[dMFC_Count];
	}

	for (i = 0; i < MAX_VAL_STEP_NO; i++)
	{
		for (j = 0; j < MAX_VAL_TIME_STEP; j++)
		{
			g_pRcpValveData_DMY_TMP.RCP_DATA[i][j] = D_RECIPE[g_bDMY_Step2].udRCP_Valve_Data.RCP_DATA[i][j];
		}
	}

	//		memcpy(g_pRcpValveData_DMY,&g_pRcpValveData_DMY_TMP,sizeof(VALVE_RCP_DATA));
	memcpy(&udValve_map_DMY, &g_pRcpValveData_DMY_TMP, sizeof(VALVE_RCP_DATA));

	Draw_DMYMFC_Contents(hdlg);
	Refresh_DMY_Valve_Data(hdlg);

	//		D_RECIPE_MFC[g_bDMY_Step2].bDMYRecipCopy = TRUE;

	g_bEditOk = FALSE;

#ifdef _DEBUG
	_IO_CIM_PRINTF("Valve Window Close With OK[hWnd_ValveRecipe is NULL]\n");
#endif
	//------ 2005.12.13 by ycchoi ---------<<

	//		EndDialog( hdlg, 1 );
}

void OnCancel(HWND hdlg)
{
	// 2004-04-29  IPS 서병찬 CJ 요구사항
	// Cancel Confirm Box

	if (IDYES == MessageBox(hdlg, "Really Cancel edited data and reload last saved data ???", "CANCEL Check", MB_ICONQUESTION | MB_YESNO))
	{

		//-------------------------------------------------------------------------------------

		// 2004/04/09 thyun 추가
		g_bEditOk = FALSE;

		//------ 2005.12.13 by ycchoi --------->>
		hWnd_ValveRecipe = NULL;
#ifdef _DEBUG
		_IO_CIM_PRINTF("Valve Window Close With CANCEL[hWnd_ValveRecipe is NULL]\n");
#endif
		//------ 2005.12.13 by ycchoi ---------<<

		EndDialog(hdlg, 0);
	}
	else
		return;
}

void Redraw_HColumn(HWND hdlg, int dStartNo)
{
	int i;
	int dTot_Valve = 0;
	int dControl = 0;
	int dValNo = 0;

	char szValName[10] = "";

	if (g_Total_Valve_No <= 21)
		KWIN_Item_Hide(hdlg, IDC_SCROLLBAR3);

	//... 2014.10.14 NotUse Fixed RF Bits
	// 	if ( g_bRF )	dTot_Valve	=	g_Total_Valve_No - 2;
	// 	else			dTot_Valve	=	g_Total_Valve_No - 1;

	dTot_Valve = g_Total_Valve_No - 1;
	//... 2014.10.14 NotUse Fixed RF Bits

	for (i = 0; i < 21; i++)
	{
		dValNo = i + 1 + dStartNo;

		// Valve 이름 셋팅 부분 ============================================================================
		// 2004 - 04 - 26 김창수 대리 요구 사항
		// 2005.07.13 by cskim, Modified
		//... 2014.10.14 NotUse Fixed RF Bits
		// 		if ( g_bRF )
		// 		{
		// 			if ( dValNo == g_Total_Valve_No && g_szValveName[dValNo-1][0] != 'R')
		// 				strcpy(g_szValveName[ dValNo - 1 ], "RF");
		// 		}

		strcpy(szValName, g_szValveName[dValNo - 1]);
		// ==================================================================================================

		dControl = IDC_ITEM_V1 + i;
		KWIN_Item_String_Display(hdlg, dControl, szValName);
	}
}

void Redraw_VColumn(HWND hdlg)
{
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP1, g_VScroll_Pos + 1);
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP2, g_VScroll_Pos + 2);
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP3, g_VScroll_Pos + 3);
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP4, g_VScroll_Pos + 4);
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP5, g_VScroll_Pos + 5);
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP6, g_VScroll_Pos + 6);
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP7, g_VScroll_Pos + 7);
	KWIN_Item_Int_Display(hdlg, IDC_COL_STEP8, g_VScroll_Pos + 8);
	//	KWIN_Item_Int_Display( hdlg, IDC_COL_STEP9 , g_VScroll_Pos+9 );
}

void Redraw_Valve_Map(HWND hdlg, int dControl, VAL_STATUS dValue, BOOL bRF)
{
	HWND hButton;
	HBRUSH hMyBrush, hOldBrush;
	HDC hDC;
	RECT rc;

	hButton = GetDlgItem(hdlg, dControl);

	if (bRF)
	{
		switch (dValue)
		{
		case VAL_NOUSE:
			hMyBrush = CreateSolidBrush(VAL_NOUSE_COLOR);
			break;
		case VAL_OPEN:
			hMyBrush = CreateSolidBrush(RF_ON_COLOR);
			break;
		case VAL_CLOSE:
			hMyBrush = CreateSolidBrush(RF_OFF_COLOR);
			break;
		}
	}
	else
	{
		switch (dValue)
		{
		case VAL_NOUSE:
			hMyBrush = CreateSolidBrush(VAL_NOUSE_COLOR);
			break;
		case VAL_OPEN:
			hMyBrush = CreateSolidBrush(VAL_ON_COLOR);
			break;
		case VAL_CLOSE:
			hMyBrush = CreateSolidBrush(VAL_OFF_COLOR);
			break;
		}
	}

	hDC = GetDC(hButton);
	hOldBrush = (HBRUSH)SelectObject(hDC, hMyBrush);
	GetClientRect(hButton, &rc);
	rc.top += 2;
	rc.left += 2;
	rc.bottom -= 2;
	rc.right -= 2;
	FillRect(hDC, &rc, hMyBrush);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hMyBrush);
	ReleaseDC(hButton, hDC);
}

void Redraw_All_Time_Value(HWND hdlg)
{
	double dbRes = 0;
	double dbRes2 = 0;
	double dbRes3 = 0;
	double dbRes4 = 0;
	double dbRes5 = 0;
	double dbRes6 = 0;
	double dbRes7 = 0;
	double dbRes8 = 0;
	double dbRes9 = 0;

	if (g_bDMY)
	{
		dbRes = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos].dbTime;
		dbRes2 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 1].dbTime;
		dbRes3 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 2].dbTime;
		dbRes4 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 3].dbTime;
		dbRes5 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 4].dbTime;
		dbRes6 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 5].dbTime;
		dbRes7 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 6].dbTime;
		dbRes8 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 7].dbTime;
		dbRes9 = udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 8].dbTime;
	}
	else
	{
		dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos].dbTime;
		dbRes2 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 1].dbTime;
		dbRes3 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 2].dbTime;
		dbRes4 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 3].dbTime;
		dbRes5 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 4].dbTime;
		dbRes6 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 5].dbTime;
		dbRes7 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 6].dbTime;
		dbRes8 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 7].dbTime;
		dbRes9 = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 8].dbTime;
	}

	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME1, dbRes, g_nValveTime_DecPnt);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME2, dbRes2, g_nValveTime_DecPnt);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME3, dbRes3, g_nValveTime_DecPnt);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME4, dbRes4, g_nValveTime_DecPnt);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME5, dbRes5, g_nValveTime_DecPnt);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME6, dbRes6, g_nValveTime_DecPnt);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME7, dbRes7, g_nValveTime_DecPnt);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME8, dbRes8, g_nValveTime_DecPnt);
	//	KWIN_Item_Double_Display( hdlg, IDC_ITEM_TIME9, dbRes9, g_nValveTime_DecPnt);
}

void Valve_Button_Show(HWND hdlg, BOOL bVisible)
{
	int i, j;
	int dStartControl;
	int dControl;
	HWND hValve;

	if (bVisible)
	{
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME1);
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME2);
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME3);
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME4);
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME5);
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME6);
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME7);
		KWIN_Item_Show(hdlg, IDC_ITEM_TIME8);
		//		KWIN_Item_Show( hdlg, IDC_ITEM_TIME9 );
		//		KWIN_Item_Show( hdlg, IDC_ITEM_TIME10);

		KWIN_Item_Show(hdlg, IDC_SCROLLBAR2);
		KWIN_Item_Show(hdlg, IDC_SCROLLBAR3);
		if (g_bValveStepCtrl)
			KWIN_Item_Enable(hdlg, IDC_VALVE_STEP_CYC_COUNT);
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME1);
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME2);
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME3);
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME4);
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME5);
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME6);
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME7);
		KWIN_Item_Hide(hdlg, IDC_ITEM_TIME8);
		//		KWIN_Item_Hide( hdlg, IDC_ITEM_TIME9 );
		//		KWIN_Item_Hide( hdlg, IDC_ITEM_TIME10);

		KWIN_Item_Hide(hdlg, IDC_SCROLLBAR2);
		KWIN_Item_Hide(hdlg, IDC_SCROLLBAR3);
		if (g_bValveStepCtrl)
			KWIN_Item_Disable(hdlg, IDC_VALVE_STEP_CYC_COUNT);
	}

	for (i = 0; i < VAL_V_MAX; i++)
	{
		for (j = 0; j < VAL_H_MAX; j++)
		{
			if (i == 0)
				dStartControl = IDC_VAL_1_1;
			else if (i == 1)
				dStartControl = IDC_VAL_2_1;
			else if (i == 2)
				dStartControl = IDC_VAL_3_1;
			else if (i == 3)
				dStartControl = IDC_VAL_4_1;
			else if (i == 4)
				dStartControl = IDC_VAL_5_1;
			else if (i == 5)
				dStartControl = IDC_VAL_6_1;
			else if (i == 6)
				dStartControl = IDC_VAL_7_1;
			else if (i == 7)
				dStartControl = IDC_VAL_8_1;
			//			else if	( i == 8 )		dStartControl	= IDC_VAL_9_1 ;

			dControl = dStartControl + j;

			hValve = GetDlgItem(hdlg, dControl);

			if (bVisible)
				KWIN_Item_Show(hdlg, dControl);
			else
				KWIN_Item_Hide(hdlg, dControl);
		}
	}
}

void Redraw_All_Valve(HWND hdlg)
{
	int i, j;
	int dStartControl;
	int dControl;
	int dIndex_X, dIndex_Y;

	char szTitle[10] = "";

	BOOL bRF = FALSE;

	if (g_Val_Total_Step > 0)
	{
		Valve_Button_Show(hdlg, TRUE); // Button Status ( Hide -> Show )
		Redraw_All_Time_Value(hdlg);   // Time Value Write

		// Valve Status Redraw
		for (i = 0; i < VAL_V_MAX; i++)
		{
			for (j = 0; j < VAL_H_MAX; j++)
			{
				if (i == 0)
					dStartControl = IDC_VAL_1_1;
				else if (i == 1)
					dStartControl = IDC_VAL_2_1;
				else if (i == 2)
					dStartControl = IDC_VAL_3_1;
				else if (i == 3)
					dStartControl = IDC_VAL_4_1;
				else if (i == 4)
					dStartControl = IDC_VAL_5_1;
				else if (i == 5)
					dStartControl = IDC_VAL_6_1;
				else if (i == 6)
					dStartControl = IDC_VAL_7_1;
				else if (i == 7)
					dStartControl = IDC_VAL_8_1;
				//				else if	( i == 8 )		dStartControl	= IDC_VAL_9_1 ;

				dControl = dStartControl + j;

				dIndex_X = i + g_VScroll_Pos;
				dIndex_Y = j + g_HScroll_Pos;

				//				printf("g_bDMY: %d, g_Val_Current_Step : %d, g_Val_Total_Step : %d \n",g_bDMY,g_Val_Current_Step,g_Val_Total_Step);
				if (g_Val_Current_Step == 0)
				{
					if (g_Val_Total_Step <= 0)
					{
						return;
					}

					g_Val_Current_Step = 1;
				}

				// 2005.07.12 by cskim
				GetDlgItemText(hdlg, IDC_ITEM_V1 + j, szTitle, 9);
				if ((!stricmp(szTitle, "RF")) || (!stricmp(szTitle, "RPG")))
					bRF = TRUE;
				else
					bRF = FALSE;

				/*
				if ( j == ( VAL_H_MAX - 1 ) )
				{
					GetDlgItemText( hdlg, IDC_ITEM_V1 + j , szTitle, 9 );
					if (!stricmp( szTitle, "RF" ))
							bRF	=	TRUE;
					else bRF =	FALSE;
				}
				else
				{
					bRF	=	FALSE;
				}
				*/
				if (g_bDMY)
					Redraw_Valve_Map(hdlg, dControl, udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][dIndex_X].RCP_DATA[dIndex_Y], bRF);
				else
					Redraw_Valve_Map(hdlg, dControl, udValve_map.RCP_DATA[g_Val_Current_Step - 1][dIndex_X].RCP_DATA[dIndex_Y], bRF);
			}
		}
	}
	else
	{
		Valve_Button_Show(hdlg, FALSE);
	}
}

void OnValve_Edit(HWND hdlg, WPARAM wParam)
{
	int indexX, indexY;
	int dInterlock_Valve;
	int dResult;

	char szMessage[50];
	BOOL bIsRF = FALSE;

	char buffer[128] = {0,};

	indexX = g_VScroll_Pos;
	indexY = g_HScroll_Pos;

	Redraw_All_Valve(hdlg);

	if (!g_bEditOk)
		return;

	if ((wParam >= IDC_VAL_1_1) && (wParam <= IDC_VAL_1_21))
	{
		indexX += 0;
		indexY = indexY + wParam - IDC_VAL_1_1;
	}
	else if ((wParam >= IDC_VAL_2_1) && (wParam <= IDC_VAL_2_21))
	{
		indexX += 1;
		indexY = indexY + wParam - IDC_VAL_2_1;
	}
	else if ((wParam >= IDC_VAL_3_1) && (wParam <= IDC_VAL_3_21))
	{
		indexX += 2;
		indexY = indexY + wParam - IDC_VAL_3_1;
	}
	else if ((wParam >= IDC_VAL_4_1) && (wParam <= IDC_VAL_4_21))
	{
		indexX += 3;
		indexY = indexY + wParam - IDC_VAL_4_1;
	}
	else if ((wParam >= IDC_VAL_5_1) && (wParam <= IDC_VAL_5_21))
	{
		indexX += 4;
		indexY = indexY + wParam - IDC_VAL_5_1;
	}
	else if ((wParam >= IDC_VAL_6_1) && (wParam <= IDC_VAL_6_21))
	{
		indexX += 5;
		indexY = indexY + wParam - IDC_VAL_6_1;
	}
	else if ((wParam >= IDC_VAL_7_1) && (wParam <= IDC_VAL_7_21))
	{
		indexX += 6;
		indexY = indexY + wParam - IDC_VAL_7_1;
	}
	else if ((wParam >= IDC_VAL_8_1) && (wParam <= IDC_VAL_8_21))
	{
		indexX += 7;
		indexY = indexY + wParam - IDC_VAL_8_1;
	}
	//	else if (( wParam >= IDC_VAL_9_1 ) && ( wParam <= IDC_VAL_9_21 )){	indexX += 8;  indexY = indexY + wParam - IDC_VAL_9_1 ; }
	else
		return;

	if (g_bDMY)
	{
		if (udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] != VAL_NOUSE)
		{
			if (Can_Change_Status(indexY, udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA, &dInterlock_Valve))
			{
				if (udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] == VAL_CLOSE)
				{
					udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] = VAL_OPEN;
					sprintf(buffer, "\t[Dummy Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_Val_Current_Step, indexX+1, g_szValveName[indexY], "Close", "Open");
				}
				else if (udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] == VAL_OPEN)
				{
					udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] = VAL_CLOSE;
					sprintf(buffer, "\t[Dummy Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_Val_Current_Step, indexX+1, g_szValveName[indexY], "Open", "Close");
				}
				// SampleLogSave(buffer, "Item");
			}
			else
			{
				InvalidateRect(hdlg, NULL, TRUE);
				sprintf(szMessage, "Interlocked[ Valve %s  &  Valve %s ]", g_szValveName[indexY], g_szValveName[dInterlock_Valve - 1]);
				MODAL_MESSAGE_BOX(hdlg, "Valve InterLock", szMessage, "Close", 0, &dResult);
			}
		}
	}
	else
	{
		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] != VAL_NOUSE)
		{
			if (Can_Change_Status(indexY, udValve_map.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA, &dInterlock_Valve))
			{
				if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] == VAL_CLOSE)
				{
					udValve_map.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] = VAL_OPEN;
					sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_Val_Current_Step, indexX+1, g_szValveName[indexY], "Close", "Open");
				}
				else if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] == VAL_OPEN)
				{
					udValve_map.RCP_DATA[g_Val_Current_Step - 1][indexX].RCP_DATA[indexY] = VAL_CLOSE;
					sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_Val_Current_Step, indexX+1, g_szValveName[indexY], "Open", "Close");
				}
				// SampleLogSave(buffer, "Item");
			}
			else
			{
				InvalidateRect(hdlg, NULL, TRUE);
				sprintf(szMessage, "Interlocked[ Valve %s  &  Valve %s ]", g_szValveName[indexY], g_szValveName[dInterlock_Valve - 1]);
				MODAL_MESSAGE_BOX(hdlg, "Valve InterLock", szMessage, "Close", 0, &dResult);
			}
		}
	}

	Redraw_All_Valve(hdlg);
}

void OnSetTime1(HWND hdlg)
{
	double dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos].dbTime;
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 1);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 1, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos].dbTime, dbRes);
			// SampleLogSave(buffer, "Item");
		}
		/*
		if(g_bDMY)
			udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos].dbTime = dbRes;
		else
			udValve_map.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos].dbTime = dbRes;
		*/

		//mgsong Modify for time interlock
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos].dbTime = dbRes;
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos].dbTime = dbRes;

		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME1, dbRes, g_nValveTime_DecPnt);
	}
}

void OnSetTime2(HWND hdlg)
{
	double dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 1].dbTime;
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 2);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 1].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 2, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 1].dbTime, dbRes);
			// SampleLogSave(buffer, "Item");
		}
		//mgsong Modify for time interlock
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 1].dbTime = dbRes;
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 1].dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME2, dbRes, g_nValveTime_DecPnt);
	}
}

void OnSetTime3(HWND hdlg)
{
	double dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 2].dbTime;
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 3);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 2].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 3, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 2].dbTime, dbRes);
			// SampleLogSave(buffer, "Item");
		}
		//mgsong Modify for time interlock
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 2].dbTime = dbRes;
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 2].dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME3, dbRes, g_nValveTime_DecPnt);
	}
}

void OnSetTime4(HWND hdlg)
{
	double dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 3].dbTime;
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 4);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 3].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 4, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 3].dbTime, dbRes);
			// SampleLogSave(buffer, "Item");
		}
		//mgsong Modify for time interlock
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 3].dbTime = dbRes;
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 3].dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME4, dbRes, g_nValveTime_DecPnt);
	}
}

void OnSetTime5(HWND hdlg)
{
	double dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 4].dbTime;
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 5);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 4].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 5, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 4].dbTime, dbRes);
			//SampleLogSave(buffer, "Item");
		}
		//mgsong Modify for time interlock
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 4].dbTime = dbRes;
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 4].dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME5, dbRes, g_nValveTime_DecPnt);
	}
}

void OnSetTime6(HWND hdlg)
{
	double dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 5].dbTime;
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 6);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 5].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 6, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 5].dbTime, dbRes);
			// SampleLogSave(buffer, "Item");
		}
		//mgsong Modify for time interlock
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 5].dbTime = dbRes;
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 5].dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME6, dbRes, g_nValveTime_DecPnt);
	}
}

void OnSetTime7(HWND hdlg)
{
	double dbRes = udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 6].dbTime;
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 7);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 6].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 7, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 6].dbTime, dbRes);
			// SampleLogSave(buffer, "Item");
		}
		//mgsong Modify for time interlock
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 6].dbTime = dbRes;
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 6].dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME7, dbRes, g_nValveTime_DecPnt);
	}
}

void OnSetTime8(HWND hdlg)
{
	double dbRes = g_dbTimeMap[g_VScroll_Pos + 7];
	char szTitle[20];

	if (!g_bEditOk)
		return;

	//mgsong Modify for time interlock
	if (g_bDMY)
		return;
	//mgsong Modify for time interlock

	sprintf(szTitle, "Valve Time [%d]", g_VScroll_Pos + 8);

	if (MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
						  g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 7].dbTime != dbRes)
		{
			sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_Val_Current_Step, g_VScroll_Pos + 8, "Valve Time", udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 7].dbTime, dbRes);
			// SampleLogSave(buffer, "Item");
		}
		//mgsong Modify for time interlock
		//udValve_map_DMY.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 7].dbTime = dbRes;
		udValve_map.RCP_DATA[g_Val_Current_Step - 1][g_VScroll_Pos + 7].dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME8, dbRes, g_nValveTime_DecPnt);
	}
}
/*
void OnSetTime9( HWND hdlg )
{
	double	dbRes	=	g_dbTimeMap[ g_VScroll_Pos + 8];
	char	szTitle[20];

	if ( !g_bEditOk	)		return;
	sprintf( szTitle, "Valve Time [%d]", g_VScroll_Pos + 9 );

	if ( MODAL_ANALOG_BOX1( hdlg, "Select", szTitle, g_dbValveTime_Min,
		g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes ) )
	{
		udValve_map.RCP_DATA[ g_Val_Current_Step - 1 ][g_VScroll_Pos + 8].dbTime = dbRes;
		KWIN_Item_Double_Display( hdlg, IDC_ITEM_TIME9, dbRes, g_nValveTime_DecPnt );

	}
}
*/
void Valve_Status_Set_From_Data_Buffer(HWND hdlg)
{
	// Process Recipe Data 중 Valve Data를 Memory Map으로 Copy
	if (g_bDMY)
	{
		memcpy(&udValve_map_DMY, g_pRcpValveData_DMY, sizeof(VALVE_RCP_DATA));
		g_Val_Total_Step = udValve_map_DMY.dTotalStep;
	}
	else
	{
		memcpy(&udValve_map, g_pRcpValveData, sizeof(VALVE_RCP_DATA));
		g_Val_Total_Step = udValve_map.dTotalStep;
	}

	g_Val_Current_Step = 1;
	g_Val_Total_Step = udValve_map.dTotalStep;

	if (g_bValveStepCtrl)
	{
		if (g_bDMY)
			KWIN_Item_Int_Display(hdlg, IDC_VALVE_STEP_CYC_COUNT, udValve_map_DMY.dValveStepCycleCount[g_Val_Current_Step - 1]);
		else
			KWIN_Item_Int_Display(hdlg, IDC_VALVE_STEP_CYC_COUNT, udValve_map.dValveStepCycleCount[g_Val_Current_Step - 1]);
	}

	Valve_Button_Show(hdlg, TRUE);
	Button_Status_Changed(hdlg, EDIT_ON);
	Button_Status_Changed(hdlg, COPY_ON);
	Button_Status_Changed(hdlg, REMOVE_ON);

	Redraw_All_Time_Value(hdlg);
}

void Buffer_Initialize(HWND hdlg)
{
	HWND hVScroll = NULL;
	HWND hHScroll = NULL;

	hHScroll = GetDlgItem(hdlg, IDC_SCROLLBAR3);
	hVScroll = GetDlgItem(hdlg, IDC_SCROLLBAR2);

	g_Val_Current_Step = 1;
	g_Val_Total_Step = 1;
	g_bCopyBuffer = FALSE;
	g_VScroll_Pos = 0;
	g_HScroll_Pos = 0;
	SetScrollPos(hVScroll, SB_CTL, g_VScroll_Pos, TRUE);
	SetScrollPos(hHScroll, SB_CTL, g_HScroll_Pos, TRUE);
}

void Valve_Status_Set_From_Mask_Buffer(HWND hdlg)
{
	int i, j;
	VAL_STATUS *pValMap;
	switch (g_RcpFileType)
	{
	// Valve Mask3 2010.02.03
	case RCP_PRE:
		pValMap = PRE_VAL_CFG_ADJ;
		break;
	case RCP_PST:
		pValMap = PST_VAL_CFG_ADJ;
		break;
	case RCP_IDL:
		pValMap = IDL_VAL_CFG_ADJ;
		break;
	case RCP_CLN:
		pValMap = CLN_VAL_CFG_ADJ;
		break;
	case RCP_PUG:
		pValMap = PUG_VAL_CFG_ADJ;
		break;
	case RCP_LPR:
		pValMap = LPR_VAL_CFG_ADJ;
		break;
	case RCP_LPS:
		pValMap = LPS_VAL_CFG_ADJ;
		break;
	default:
		g_RcpFileType = RCP_PRC;
		pValMap = RCP_VAL_CFG_ADJ;
	}

	for (i = 0; i < MAX_VAL_STEP_NO; i++)
	{
		if (g_bDMY)
			udValve_map_DMY.dValveStepCycleCount[i] = 0;
		else
			udValve_map.dValveStepCycleCount[i] = 0;
		for (j = 0; j < MAX_VAL_TIME_STEP; j++)
		{
			if (g_bDMY)
			{
				udValve_map_DMY.RCP_DATA[i][j].dbTime = 0;
				memcpy(udValve_map_DMY.RCP_DATA[i][j].RCP_DATA, pValMap, sizeof(VAL_STATUS) * MAX_VAL_NO);
			}
			else
			{
				udValve_map.RCP_DATA[i][j].dbTime = 0;
				memcpy(udValve_map.RCP_DATA[i][j].RCP_DATA, pValMap, sizeof(VAL_STATUS) * MAX_VAL_NO);
			}
		}
	}

	if (g_bDMY)
		udValve_map_DMY.dValveStepCycleCount[0] = 1; //swhan
	else
		udValve_map.dValveStepCycleCount[0] = 1; //swhan
	Redraw_All_Time_Value(hdlg);
}

void Valve_Status_Set(HWND hdlg)
{
	if (g_bHasValveData)
		Valve_Status_Set_From_Data_Buffer(hdlg); // 기존에 Valve 데이터가 있는 경우
	else
		Valve_Status_Set_From_Mask_Buffer(hdlg); // 처음으로 Valve 데이터 생성시 마스크 비트로 초기화 한다.
	//	printf("g_bHasValveData : %d \n",g_bHasValveData);
}

void Valve_Status_Set_DMY(HWND hdlg)
{
	if (g_bHasValveData_DMY)
		Valve_Status_Set_From_Data_Buffer(hdlg); // 기존에 Valve 데이터가 있는 경우
	else
		Valve_Status_Set_From_Mask_Buffer(hdlg); // 처음으로 Valve 데이터 생성시 마스크 비트로 초기화 한다.
	//	printf("g_bHasValveData_DMY : %d \n",g_bHasValveData_DMY);
}

void Step_Increase(HWND hdlg)
{
	if ((g_Val_Current_Step < 10) && (g_Val_Total_Step > g_Val_Current_Step))
	{
		g_Val_Current_Step++;
		Redraw_Valve_Information(hdlg);
		Redraw_All_Valve(hdlg);
		Redraw_All_Time_Value(hdlg);
	}
}

void Step_Decrease(HWND hdlg)
{
	if (g_Val_Current_Step > 1)
	{
		g_Val_Current_Step--;
		Redraw_Valve_Information(hdlg);
		Redraw_All_Valve(hdlg);
		Redraw_All_Time_Value(hdlg);
	}
}

BOOL OnValStepCycCount(HWND hdlg)
{
	int dRes = 0;

	// Valve Step Cycle Count
	dRes = udValve_map.dValveStepCycleCount[g_Val_Current_Step - 1];

	if (!g_bEditOk)
		return FALSE;

	if (MODAL_DIGITAL_BOX2(hdlg, "Select", "Valve Step Cycle Count", 1, 200, &dRes))
	{
		char buffer[128] = {0, };

		if (udValve_map.dValveStepCycleCount[g_Val_Current_Step - 1] != dRes)
		{
			sprintf(buffer, "\t[Valve Data] STEP:[%d] %s - From [%d] to [%d]\n", g_Val_Current_Step - 1, "Valve Step Cycle Count", udValve_map.dValveStepCycleCount[g_Val_Current_Step - 1], dRes);
			// SampleLogSave(buffer, "Item");
		}
		udValve_map.dValveStepCycleCount[g_Val_Current_Step - 1] = dRes;
		udValve_map_DMY.dValveStepCycleCount[g_Val_Current_Step - 1] = dRes;
		Redraw_Valve_Information(hdlg);
		return TRUE;
	}

	return FALSE;
}

void OnDetailSetDlg(int dButtonIndex, HWND hdlg)
{
	int dIndex = 0;

	extern int g_dCurrentProcessStep;
	extern int g_dCurrentValveStep;
	extern int g_dCurrentTimeStep;

	dIndex = dButtonIndex + g_VScroll_Pos; // Time Step

	g_dCurrentValveStep = g_Val_Current_Step;
	g_dCurrentProcessStep = g_ProcessStep;
	g_dCurrentTimeStep = dIndex;

	if (g_bDMY)
	{
		memcpy(&g_Det_Valve_Data_DMY, &udValve_map_DMY.RCP_DATA[g_Val_Current_Step - 1][dIndex], sizeof(VAL_DATA));
	}
	else
		memcpy(&g_Det_Valve_Data, &udValve_map.RCP_DATA[g_Val_Current_Step - 1][dIndex], sizeof(VAL_DATA));

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DETAIL_DIALOG), hdlg, Valve_Detail_Msg_Handler);
	Refresh_Data(hdlg);
}

// 2005.05.21 by cskim, for CVD Editor
BOOL OnCVDSchDlg(HWND hdlg, VALVE_RCP_DATA *pValveData, BOOL bEdit)
{
	int dIndex = 0;

	extern int g_dCurrentProcessStep;
	extern int g_dCurrentValveStep;
	extern int g_dCurrentTimeStep;
	extern BOOL g_bCVDValveEditUpdate;
	extern int g_bDetailEditMode;
	dIndex = 0 + g_VScroll_Pos; // Time Step

	g_dCurrentValveStep = g_Val_Current_Step;
	g_dCurrentProcessStep = g_ProcessStep;
	g_dCurrentTimeStep = dIndex;

	if (bEdit)
		g_bDetailEditMode = 2;
	else
		g_bDetailEditMode = 1;
	memcpy(&g_Det_Valve_Data, &pValveData->RCP_DATA[g_Val_Current_Step - 1][dIndex], sizeof(VAL_DATA));
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DETAIL_DIALOG), hdlg, Valve_Detail_Msg_Handler);
	memcpy(&pValveData->RCP_DATA[g_Val_Current_Step - 1][dIndex], &g_Det_Valve_Data, sizeof(VAL_DATA));
	g_bDetailEditMode = 0;
	return g_bCVDValveEditUpdate;
}

BOOL APIENTRY But_Proc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hParent = NULL;

	hParent = GetParent(hdlg);

	switch (msg)
	{
	case WM_SETFOCUS:
		InvalidateRect(hParent, NULL, TRUE);
		return 0;

	case WM_KILLFOCUS:
		InvalidateRect(hParent, NULL, TRUE);
		return 0;
	}

	return CallWindowProc(Default_Valve_Proc, hdlg, msg, wParam, lParam);
}

void Make_SubClassing(HWND hdlg)
{
	int i;
	HWND hBut;

	for (i = 0; i < VAL_H_MAX; i++)
	{
		hBut = GetDlgItem(hdlg, IDC_ITEM_V1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_1_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_2_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_3_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_4_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_5_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_6_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_7_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_8_1 + i);
		Default_Valve_Proc = (WNDPROC)SetWindowLong(hBut, GWL_WNDPROC, (LONG)But_Proc);
		//		hBut = GetDlgItem( hdlg, IDC_VAL_9_1 + i );
		//		Default_Valve_Proc = (WNDPROC) SetWindowLong( hBut, GWL_WNDPROC, (LONG) But_Proc );
	}
}

void Valve_Recipe_ShowHide(HWND hdlg)
{
	if (!g_bDMY)
	{
		// Disable Dummy MFC Value
		KWIN_Item_Hide(hdlg, IDC_STATIC_DMY);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY1_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY2_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY3_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY4_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY5_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY6_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY7_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY8_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY9_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY10_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY11_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY12_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY13_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY14_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY15_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_DMY16_SET1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD1_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD2_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD3_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD4_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD5_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD6_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD7_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD8_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD9_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD10_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD11_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD12_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD13_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD14_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD15_OPT1);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEMD16_OPT1);
		KWIN_Item_Hide(hdlg, IDC_STATIC_DMY_MFC);
		//		SetWindowText(GetDlgItem(hdlg, IDC_BUTTON_DMY_SET1), cText);
	}
	else
	{
		KWIN_Item_Show(hdlg, IDC_STATIC_DMY);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY1_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY2_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY3_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY4_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY5_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY6_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY7_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY8_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY9_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY10_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY11_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY12_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY13_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY14_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY15_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_DMY16_SET1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD1_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD2_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD3_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD4_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD5_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD6_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD7_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD8_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD9_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD10_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD11_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD12_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD13_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD14_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD15_OPT1);
		KWIN_Item_Show(hdlg, IDC_BUTTON_ITEMD16_OPT1);
		KWIN_Item_Show(hdlg, IDC_STATIC_DMY_MFC);
	}
}
//------------------------------------------------------------------------
BOOL APIENTRY Valve_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int dvPos = 0;
	int dhPos = 0;
	int dScrRange = 0;
	int dVScrRange = 0;

	int nIdx, nID;

	HWND hVScroll = NULL;
	HWND hHScroll = NULL;
	HWND hOpenRect = NULL;
	HWND hCloseRect = NULL;
	HWND hSpinCtrl = NULL;

	HDC hDC;
	HBRUSH hMyBrush, hOldBrush;
	HPEN hMyPen, hOldPen;
	HFONT hMyFont, hOldFont;

	LPDRAWITEMSTRUCT pDrawItem;

	RECT rc, rc1, rc2;
	PAINTSTRUCT ps;

	hHScroll = GetDlgItem(hdlg, IDC_SCROLLBAR3);
	hVScroll = GetDlgItem(hdlg, IDC_SCROLLBAR2);
	hSpinCtrl = GetDlgItem(hdlg, IDC_SPIN1);

	switch (msg)
	{
	case WM_INITDIALOG:

		//------ 2005.12.13 by ycchoi --------->>
		hWnd_ValveRecipe = hdlg;
#ifdef _DEBUG
		_IO_CIM_PRINTF("Valve Recipe Window Handle [%d]\n", hWnd_ValveRecipe);
#endif
		//------ 2005.12.13 by ycchoi ---------<<

		KGUI_STANDARD_Set_Function_Size(hdlg);
		Edit_Button_Mapping(hdlg);		// Control 버튼 하단 - 생성
		Button_Initialize_Status(hdlg); // Control 버튼 하단 - 초기처리
		Buffer_Initialize(hdlg);
		if (g_bDMY)
			Valve_Status_Set_DMY(hdlg); // 초기 데이터 셋팅
		else
			Valve_Status_Set(hdlg); // 초기 데이터 셋팅
		KWIN_Item_Int_Display(hdlg, IDC_PROCESS_STEP, g_ProcessStep);
		//			KWIN_Item_Int_Display( hdlg, IDC_PROCESS_STEP2, g_Val_Current_Step	);
		//			KWIN_Item_Int_Display( hdlg, IDC_PROCESS_STEP3, g_Val_Total_Step	);
		Make_SubClassing(hdlg);
		Redraw_VColumn(hdlg);
		Redraw_HColumn(hdlg, 0);
		Redraw_All_Valve(hdlg);
		Redraw_Valve_Information(hdlg);

		OnEditPressed(hdlg);

		Valve_Recipe_ShowHide(hdlg); //Added by jhlee 2009.07

		//			SelectObject( hDC, hMyFont );

		//			printf("Start Draw_DMYMFC_Contents Function \n");
		if (g_bDMY)
			Draw_DMYMFC_Contents(hdlg); //added by jhlee 2009.07

		return FALSE;

	case WM_DRAWITEM:
		Redraw_All_Valve(hdlg);
		KWIN_DRAW_BUTTON_ITEM(GetModuleHandle(NULL), (LPDRAWITEMSTRUCT)lParam);
		pDrawItem = (LPDRAWITEMSTRUCT)lParam;
		return FALSE;

	case WM_PAINT:

		hMyPen = CreatePen(PS_SOLID, 0, RGB(170, 170, 170));
		hMyFont = CreateFont(17, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");

		hDC = BeginPaint(hdlg, &ps);
		hOldFont = (HFONT)SelectObject(hDC, hMyFont);
		hOldPen = (HPEN)SelectObject(hDC, hMyPen);
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(0, 0, 0));

		hMyBrush = CreateSolidBrush(RGB(206, 207, 206));
		hOldBrush = (HBRUSH)SelectObject(hDC, hMyBrush);
		GetClientRect(hdlg, &rc);
		FillRect(hDC, &rc, hMyBrush);

		KWIN_DRAW_BACK_PAINT(KGUI_s_Get_GLIB_BACKGROUND(), ps, "IDB_VAL_BACKGRD");

		// 2004.09.09 by cskim
		SelectObject(hDC, hOldBrush);
		DeleteObject(hMyBrush);

		hOpenRect = GetDlgItem(hdlg, IDC_OPEN_COLOR);
		hMyBrush = CreateSolidBrush(RGB(0, 255, 0));
		SelectObject(hDC, hMyBrush);
		rc1.left = 858;
		rc1.top = 25;
		rc1.right = 873;
		rc1.bottom = 40;
		FillRect(hDC, &rc1, hMyBrush);

		// 2004.09.09 by cskim
		SelectObject(hDC, hOldBrush);
		DeleteObject(hMyBrush);

		hCloseRect = GetDlgItem(hdlg, IDC_CLOSE_COLOR);
		hMyBrush = CreateSolidBrush(RGB(0, 0, 255));
		SelectObject(hDC, hMyBrush);
		rc2.left = 858;
		rc2.top = 42;
		rc2.right = 873;
		rc2.bottom = 57;

		FillRect(hDC, &rc2, hMyBrush);

		/*
			TextOut( hDC,  20, 30, "Process Step", 12 );
			TextOut( hDC, 230, 30, "Valve Step No.", 15 );
			TextOut( hDC, 525, 30, "Valve Step Cycle Count", 23 );
			TextOut( hDC, 800, 23, "Open :", 8 );
			TextOut( hDC, 797, 41, "Close :", 8 );
			*/

		SelectObject(hDC, hOldBrush);
		SelectObject(hDC, hOldFont);
		SelectObject(hDC, hOldPen);

		DeleteObject(hMyBrush);
		DeleteObject(hMyFont);
		DeleteObject(hMyPen);

		EndPaint(hdlg, &ps);

		Redraw_Valve_Information(hdlg);
		if (g_bDMY)
			Draw_MFCDMY_Title(hdlg); //added by jhlee 2009.07

		return FALSE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_VAL_STEP_LEFT:
			Step_Decrease(hdlg);
			return FALSE;
		case IDC_VAL_STEP_RIGHT:
			Step_Increase(hdlg);
			return FALSE;
		case IDB_INSERT_ENABLE:
			On_Insert_Enable(hdlg);
			return FALSE;
		case IDB_INSERT2_ENABLE:
			On_Insert_Enable2(hdlg);
			return FALSE;
		case IDB_REMOVE_ENABLE:
			On_Remove_Enable(hdlg);
			return FALSE;
		case IDB_COPY_ENABLE:
			On_Copy_Enable(hdlg);
			return FALSE;
		case IDB_EDIT_ENABLE:
			OnEditEnable(hdlg);
			return FALSE;
		case IDB_EDIT_PRESSED:
			OnEditPressed(hdlg);
			return FALSE;
		case IDB_VAL_OK_ENABLE:
			OnOk(hdlg);
			return TRUE;
		case IDB_VAL_CANCEL_ENABLE:
			OnCancel(hdlg);
			return FALSE;
		case IDB_DMY_COPY_ENABLE:
			On_DMY_Recipe_Copy(hdlg);
			return FALSE;
		case IDC_ITEM_TIME1:
			OnSetTime1(hdlg);
			return FALSE;
		case IDC_ITEM_TIME2:
			OnSetTime2(hdlg);
			return FALSE;
		case IDC_ITEM_TIME3:
			OnSetTime3(hdlg);
			return FALSE;
		case IDC_ITEM_TIME4:
			OnSetTime4(hdlg);
			return FALSE;
		case IDC_ITEM_TIME5:
			OnSetTime5(hdlg);
			return FALSE;
		case IDC_ITEM_TIME6:
			OnSetTime6(hdlg);
			return FALSE;
		case IDC_ITEM_TIME7:
			OnSetTime7(hdlg);
			return FALSE;
		case IDC_ITEM_TIME8:
			OnSetTime8(hdlg);
			return FALSE;
			//				case IDC_ITEM_TIME9			 :	OnSetTime9( hdlg );				return FALSE;
		case IDC_VALVE_STEP_CYC_COUNT:
			OnValStepCycCount(hdlg);
			return FALSE;
		case IDC_COL_STEP1:
			OnDetailSetDlg(0, hdlg);
			return FALSE;
		case IDC_COL_STEP2:
			OnDetailSetDlg(1, hdlg);
			return FALSE;
		case IDC_COL_STEP3:
			OnDetailSetDlg(2, hdlg);
			return FALSE;
		case IDC_COL_STEP4:
			OnDetailSetDlg(3, hdlg);
			return FALSE;
		case IDC_COL_STEP5:
			OnDetailSetDlg(4, hdlg);
			return FALSE;
		case IDC_COL_STEP6:
			OnDetailSetDlg(5, hdlg);
			return FALSE;
		case IDC_COL_STEP7:
			OnDetailSetDlg(6, hdlg);
			return FALSE;
		case IDC_COL_STEP8:
			OnDetailSetDlg(7, hdlg);
			return FALSE;
			//				case IDC_BUTTON_DMY1_SET1	 :	OnDetailSetDlg( 8, hdlg );		return FALSE;
			//				case IDC_COL_STEP9			 :	OnDetailSetDlg( 8, hdlg );		return FALSE;

		case IDC_BUTTON_ITEMD1_OPT1:
		case IDC_BUTTON_ITEMD2_OPT1:
		case IDC_BUTTON_ITEMD3_OPT1:
		case IDC_BUTTON_ITEMD4_OPT1:
		case IDC_BUTTON_ITEMD5_OPT1:
		case IDC_BUTTON_ITEMD6_OPT1:
		case IDC_BUTTON_ITEMD7_OPT1:
		case IDC_BUTTON_ITEMD8_OPT1:
		case IDC_BUTTON_ITEMD9_OPT1:
		case IDC_BUTTON_ITEMD10_OPT1:
		case IDC_BUTTON_ITEMD11_OPT1:
		case IDC_BUTTON_ITEMD12_OPT1:
		case IDC_BUTTON_ITEMD13_OPT1:
		case IDC_BUTTON_ITEMD14_OPT1:
		case IDC_BUTTON_ITEMD15_OPT1:
		case IDC_BUTTON_ITEMD16_OPT1:
			nID = LOWORD(wParam);
			//						printf("nID = LOWRD : %d \n",nID);
			if (g_bEditOk)
				ITEM_BUTTON_MFC_OPTION(hdlg, nID);
			return TRUE;

		default:
			OnValve_Edit(hdlg, wParam);
			if (g_bDMY)
			{
				nID = LOWORD(wParam);
				nIdx = FindIdx(nID, g_MFC_SET_BTN_DMY[0], MAX_MFC_DISPLAY);
				if (nIdx >= 0 && g_bEditOk)
				{
					ITEM_BUTTON_MFC_SETPOINT(nIdx + 1, hdlg, g_bDMY_Step2);
					return FALSE;
				}
			}
		}

	case WM_HSCROLL:
		dScrRange = g_Total_Valve_No - VAL_H_MAX;
		dhPos = g_HScroll_Pos;
		SetScrollRange(hHScroll, SB_CTL, 0, dScrRange, TRUE);

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			dhPos = max(0, dhPos - 1);
			break;

		case SB_LINERIGHT:
			dhPos = min(dScrRange, dhPos + 1);
			break;

		case SB_PAGELEFT:
			dhPos = max(0, dhPos - 10);
			break;

		case SB_PAGERIGHT:
			dhPos = min(dScrRange, dhPos + 10);
			break;

		case SB_THUMBTRACK:
			dhPos = HIWORD(wParam);
			break;
		}

		SetScrollPos(hHScroll, SB_CTL, dhPos, TRUE);
		Redraw_All_Valve(hdlg);
		g_HScroll_Pos = dhPos;
		Redraw_HColumn(hdlg, dhPos);
		return FALSE;

	case WM_VSCROLL:
		dvPos = g_VScroll_Pos;
		dVScrRange = g_nTotalValveLineStep - VAL_V_MAX;
		SetScrollRange(hVScroll, SB_CTL, 0, dVScrRange, TRUE);

		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			dvPos = max(0, dvPos - 1);
			break;

		case SB_LINEDOWN:
			dvPos = min(dVScrRange, dvPos + 1);
			break;

		case SB_PAGEUP:
			dvPos = max(0, dvPos - 10);
			break;

		case SB_PAGEDOWN:
			dvPos = min(dVScrRange, dvPos + 10);
			break;

		case SB_THUMBTRACK:
			dvPos = HIWORD(wParam);
			break;
		}

		SetScrollPos(hVScroll, SB_CTL, dvPos, TRUE);
		Redraw_All_Valve(hdlg);
		g_VScroll_Pos = dvPos;
		Redraw_VColumn(hdlg);
		return FALSE;

	case WM_KEYDOWN:
		InvalidateRect(hdlg, NULL, TRUE);
		return 0;
	}

	return FALSE;
}
