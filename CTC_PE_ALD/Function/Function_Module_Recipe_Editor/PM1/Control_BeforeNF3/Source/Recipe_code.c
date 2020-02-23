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
#include "TextParser_C.h"

#include "cimseqnc.h"
#include "resource.h"

#include "CVD_Rcp.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// ycchoi(2006.02.03)
// EDIT Button 추가(NRD요청사항)
extern BOOL gbEditMode;

//  2013.04.02 Recipe Password
BOOL g_bReciepPW_CHK = FALSE;
BOOL g_bRecipePWNull = FALSE;
extern BOOL g_bRecipePW;
//  2013.04.02 Recipe Password

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2005.05.19 by cskim

int g_MFC_SET_BTN[MAX_DISPLAY_COUNT][MAX_MFC_DISPLAY] =
{
	{
		IDC_BUTTON_ITEM1_SET1,
		IDC_BUTTON_ITEM2_SET1,
		IDC_BUTTON_ITEM3_SET1,
		IDC_BUTTON_ITEM4_SET1,
		IDC_BUTTON_ITEM5_SET1,
		IDC_BUTTON_ITEM6_SET1,
		IDC_BUTTON_ITEM7_SET1,
		IDC_BUTTON_ITEM8_SET1,
		IDC_BUTTON_ITEM9_SET1,
		IDC_BUTTON_ITEM10_SET1,
		IDC_BUTTON_ITEM11_SET1,
		IDC_BUTTON_ITEM12_SET1,
		IDC_BUTTON_ITEM13_SET1,
		IDC_BUTTON_ITEM14_SET1,
		IDC_BUTTON_ITEM15_SET1,
		IDC_BUTTON_ITEM16_SET1
	},
	{
		IDC_BUTTON_ITEM1_SET2,
		IDC_BUTTON_ITEM2_SET2,
		IDC_BUTTON_ITEM3_SET2,
		IDC_BUTTON_ITEM4_SET2,
		IDC_BUTTON_ITEM5_SET2,
		IDC_BUTTON_ITEM6_SET2,
		IDC_BUTTON_ITEM7_SET2,
		IDC_BUTTON_ITEM8_SET2,
		IDC_BUTTON_ITEM9_SET2,
		IDC_BUTTON_ITEM10_SET2,
		IDC_BUTTON_ITEM11_SET2,
		IDC_BUTTON_ITEM12_SET2,
		IDC_BUTTON_ITEM13_SET2,
		IDC_BUTTON_ITEM14_SET2,
		IDC_BUTTON_ITEM15_SET2,
		IDC_BUTTON_ITEM16_SET2
	}
};
int g_MFC_SET_BTN_DMY[MAX_DISPLAY_COUNT][MAX_MFC_DISPLAY] =
{
		IDC_BUTTON_DMY1_SET1,
		IDC_BUTTON_DMY2_SET1,
		IDC_BUTTON_DMY3_SET1,
		IDC_BUTTON_DMY4_SET1,
		IDC_BUTTON_DMY5_SET1,
		IDC_BUTTON_DMY6_SET1,
		IDC_BUTTON_DMY7_SET1,
		IDC_BUTTON_DMY8_SET1,
		IDC_BUTTON_DMY9_SET1,
		IDC_BUTTON_DMY10_SET1,
		IDC_BUTTON_DMY11_SET1,
		IDC_BUTTON_DMY12_SET1,
		IDC_BUTTON_DMY13_SET1,
		IDC_BUTTON_DMY14_SET1,
		IDC_BUTTON_DMY15_SET1,
		IDC_BUTTON_DMY16_SET1
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

extern BOOL GROUP_RUN_CHECK;
extern char g_szValveName[MAX_VAL_NO][MAX_VAL_NAME_LENGTH];
extern ITEM_BTN_STRUCT g_MfcBtnB[MAX_DISPLAY_COUNT][MAX_MFC_DISPLAY];
extern ITEM_BTN_STRUCT g_MfcBtnBDMY[MAX_DISPLAY_COUNT][MAX_MFC_DISPLAY];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char MAIN_DIRECTORY[128 + 1];
char FILE_DIRECTORY[128 + 1];
char GROUP_DIRECTORY[128 + 1];
BOOL FILE_CHANGE = FALSE;

RecipeStepTemplate D_RECIPE0;
RecipeStepTemplateM D_RECIPE[MAX_DISPLAY_COUNT];
RecipeStepTemplateM D_RECIPE_MFC[MAX_DISPLAY_COUNT];
RecipeStepTemplateM D_RECIPE_TMP[MAX_DISPLAY_COUNT];
BOOL D_HAS_STEP[MAX_DISPLAY_COUNT];
BOOL D_HAS_CONTROL = FALSE;
BOOL g_bNEW_FLAG = FALSE;
BOOL g_bMainValveRecipeFlag = FALSE;

int D_FIRST_INDEX = 0;
int D_LAST_INDEX = 0;
int D_CURRENT1_INDEX = 0;
int D_CURRENT2_INDEX = 0;
int g_dMainStepNo = -1;

VALVE_RCP_DATA *g_pRcpValveData = NULL;
VALVE_RCP_DATA *g_pRcpValveData_DMY = NULL;
VALVE_RCP_DATA *g_pRcpValveData_DMY_T = NULL;
RecipeStepTemplateM *Heater_D_RECIPE = NULL;
RecipeStepTemplateM *pSubItem_D_RECIPE = NULL;

extern VALVE_RCP_DATA g_pRcpValveData_DMY_TMP;
extern RCP_TYPE g_RcpFileType;
extern HINSTANCE g_hInstance;
extern int g_ProcessStep;
extern MFC_RCP_DATA g_MFCData[20];
extern BOOL g_bHasValveData;
extern BOOL g_bHasValveData_DMY;
extern BOOL g_bRF;

extern BOOL g_bSubItemEdit;
extern BOOL g_bSubItem2Edit; // 2006.09.04

// 2004.11.08 by cskim
extern BOOL g_bPreTreat;
extern BOOL g_bStepGroup;
extern BOOL g_bRoughValve;

// 2005.05.24 by cskim, for Keep Last Condition
extern BOOL g_bKeepLastConEnable;

// 2006.01.25
extern BOOL g_bThickness;

// 2009.08.18 Added by jhlee
extern BOOL g_bDMY;
extern int g_bDMY_Step2;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/////////// It is added for check recipe step 2006.03.27 ///////////////////////////
extern BOOL IsValveRecipeTimeOK(int *pnNotOkStep);
/////////// It is added for check recipe step 2006.03.27 ///////////////////////////

extern void Check_RecipeStep_Data_Err(RecipeStepTemplateM *pRcpStep);
// 2006.01.02
extern double GetStepTime(RecipeStepTemplateM *pRcpStep);
extern double GetProcTotalTime();
extern void PutGrpStepCycleData(RecipeStepTemplate *pRcpHeader);
extern int GetMfcNoFromBtnIDDMY(int nID, int *pnPage);
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char g_szSampleLogFileName[MAX_PATH] = "";
#define RecipeEditLogFolder "F:\\DATALOG\\RecipeEdit"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Recipe Item Enumeration Definition
// 2004.11.06 by cskim
// 2006.06.30 by cskim
char *g_pszMfcOptEnum[] =
	{
		"NO",
		"Warning",
		"ChmDis",
		"Abort",
		"F-Open",
		"N/A",
		"N/A",
		"N/A",
		"Retry" // 2006.07.10
};

//-----------------------------------------------

char *g_pszTVOptEnum[] =
	{
		"Position",
		"PrsA_Warning",
		"PrsA_ChmDis",
		"PrsA_Abort",
		"PrsA_NO",
		"PrsA_Retry" // 2006.07.10
};

char *g_szTVOptEnumStr = "Position|PrsA_Warning|PrsA_ChmDis|PrsA_Abort|PrsA_NO|PrsA_Retry"; // 2006.07.10
// char *g_szTVOptEnumStr = "Position|PrsA_Warning" ;

//-----------------------------------------------
char *g_pszAlarmOptEnum[] =
	{
		"NO",
		"Warning",
		"ChmDis",
		"Abort",
		"Retry" // 2006.07.10
};
char *g_szAlarmOptEnumStr = "NO|Warning|ChmDis|Abort|Retry"; // 2006.07.10

//-----------------------------------------------
// 2005.12.28
char *g_pszMfcRampOptEnum[] =
{
	"NO",
	"MfcRamping"
};
char *g_szMfcRampOptEnumStr = "NO|MfcRamping";

// Added by jhlee. 2009.11.25. for ROR Check
char *g_pszSubFuncEnum[] =
	{
		"None",
		"LeakCheck"};
char *g_szSubFuncEnumStr = "None|LeakChk|RefAll"; //2014.05.22

//... 2015.03.21 Add Position Move Option
char *g_pszStgPosWaitOptEnum[] =
{
	"Wait",
	"NoWait"
};
char *g_szStgPosWaitOptEnumStr = "Wait|NoWait";

//... 2015.11.26 Add
char *g_szRcpTypeOptEnum[] =
{
	"Process",
	"Pre Process",
	"Post Process",
	"Idle",
	"Clean",
	"Purge",
	"Lot Pre Recipe",
	"Lot Post Recipe"
};
char *g_szRcpTypeOptEnumStr = "Process|Pre Process|Post Process|Idle|Clean|Purge|Lot Pre Recipe|Lot Post Recipe";

char *g_szKeepLastConEnum[] =
{
	"NotCheck",
	"Check"
};
char *g_szKeepLastConEnumStr = "NotCheck|Check";

char *g_szStgHtrOptEnum[] =
{
	"NotCheck",
	"Check"
};
char *g_szStgHtrOptEnumStr = "NotCheck|Check";

char *g_szValveCycOptEnum[] =
{
	"NO",
	"YES"
};
char *g_szValveCycOptEnumStr = "NO|YES";

char *g_pszSelectedEnum[] =
	{
		"Deselect",
		"Select"};
char *g_szSelectedEnumStr = "Deselect|Select";

char *g_szPreTreEnum[] =
{
	"No",
	"PreTreat"
};
char *g_szPreTreEnumStr = "No|PreTreat";

//------------------------------------------------------------------------------------------
void ITEM_DRAW_FULL_DATA(HWND hdlg);
BOOL FileData_ListView_Init(HWND hdlg, char *pszFilterName);
BOOL APIENTRY Valve_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
// BOOL APIENTRY Heater_Msg_Handler( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY SubItem_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

// 2006.08.18
BOOL APIENTRY SubItem2_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

void ITEM_DRAW_RECIPE_TYPE(HWND hdlg);
void FILE_HANDLING_BUTTON_MAPPING(HWND hParent, int id, int Enable);
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

// Fixed Handlign Part
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Set_Main_Step_Number(int i) { g_dMainStepNo = i; }
//------------------------------------------------------------------------------------------
void RECIPE_CONTROL_Get_Selected_FILENAME(HWND hdlg, char *Buffer, int Count)
{
	GetWindowText(GetDlgItem(hdlg, IDC_CURRENT_RECIPEFILE), Buffer, Count);
}
//
void RECIPE_CONTROL_Set_Selected_FILENAME(HWND hdlg, char *Buffer)
{
	extern char g_szRecipeFileName[];

	if (strlen(Buffer) > 0 && g_bRecipePWNull == TRUE)
		D_HAS_CONTROL = TRUE;
	else
		D_HAS_CONTROL = FALSE;
	KWIN_Item_String_Display(hdlg, IDC_CURRENT_RECIPEFILE, Buffer);

	strcpy(g_szRecipeFileName, Buffer);
}
//
void RECIPE_CONTROL_Set_Select_ANOTHERFILE(HWND hdlg, BOOL data)
{
	if (data)
	{
		KWIN_Item_Enable(hdlg, IDC_CTRL_RELOAD_RECIPE);
		//
		if (GROUP_RUN_CHECK)
			KWIN_Item_Enable(hdlg, IDC_LIST_RECIPEDIR);
		//
		KWIN_Item_Enable(hdlg, IDC_LIST_RECIPEFILE);
	}
	else
	{
		KWIN_Item_Disable(hdlg, IDC_CTRL_RELOAD_RECIPE);
		//
		if (GROUP_RUN_CHECK)
			KWIN_Item_Disable(hdlg, IDC_LIST_RECIPEDIR);
		//
		KWIN_Item_Disable(hdlg, IDC_LIST_RECIPEFILE);
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void RECIPE_CONTROL_Set_MAIN_DIRECTORY(char *data) { strncpy(MAIN_DIRECTORY, data, 128); }
char *RECIPE_CONTROL_Get_MAIN_DIRECTORY() { return MAIN_DIRECTORY; }
//
void RECIPE_CONTROL_Set_GROUP_DIRECTORY(char *data) { strncpy(GROUP_DIRECTORY, data, 128); }
char *RECIPE_CONTROL_Get_GROUP_DIRECTORY() { return GROUP_DIRECTORY; }
//
void RECIPE_CONTROL_Set_FILE_DIRECTORY(char *data) { strncpy(FILE_DIRECTORY, data, 128); }
char *RECIPE_CONTROL_Get_FILE_DIRECTORY() { return FILE_DIRECTORY; }
//
void RECIPE_CONTROL_Set_FILE_CHANGE(BOOL data) { FILE_CHANGE = data; }
BOOL RECIPE_CONTROL_Get_FILE_CHANGE() { return FILE_CHANGE; }

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2005.05.20 by cskim
int FindIdx(int nValue, int pnArray[], int nCount)
{
	int i;
	for (i = 0; i < nCount; i++)
		if (nValue == pnArray[i])
			break;
	if (i < nCount)
		return i;
	return -1;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SET_DISPLAY_INDEX(int i1, int i2, int i3, int i4)
{
	if (i1 >= 0)
		D_FIRST_INDEX = i1;
	if (i2 >= 0)
		D_LAST_INDEX = i2;
	if (i3 >= 0)
		D_CURRENT1_INDEX = i3;
	if (i4 >= 0)
		D_CURRENT2_INDEX = i4;
}
//
int READ_FIRST_INDEX() { return D_FIRST_INDEX; }
int READ_LAST_INDEX() { return D_LAST_INDEX; }
int READ_CURRENT1_INDEX() { return D_CURRENT1_INDEX; }
int READ_CURRENT2_INDEX() { return D_CURRENT2_INDEX; }
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void DATA_PUT_TO_FILE(BOOL All)
{
	int i;
	if (All)
		Kor_Step0DataPut(&D_RECIPE0);
	for (i = READ_CURRENT1_INDEX(); i <= READ_CURRENT2_INDEX(); i++)
	{
		Kor_StepDataPut(i, &D_RECIPE[i - READ_CURRENT1_INDEX()]);
	}
}
//
void DATA_GET_FROM_FILE(BOOL All)
{
	int i, j;
	if (All)
		Kor_Step0DataGet(&D_RECIPE0);
	for (i = 0; i < MAX_DISPLAY_COUNT; i++)
		D_HAS_STEP[i] = FALSE;
	if (READ_LAST_INDEX() < 1)
		return;
	for (i = READ_CURRENT1_INDEX(), j = 1; (i > 0) && (i <= READ_LAST_INDEX()) && (j <= MAX_DISPLAY_COUNT); i++, j++)
	{
		Kor_StepDataGet(i, &D_RECIPE[j - 1]);
		D_HAS_STEP[j - 1] = TRUE;
		SET_DISPLAY_INDEX(-1, -1, -1, i);
	}
}

//------------------------------------------------------------------------
void RECIPE_CONTROL_ALL_DATA_DISPLAY(HWND hdlg)
{
	if (!D_HAS_CONTROL)
	{
		KWIN_Item_Hide(hdlg, IDB_INSERT_ENABLE);
		KWIN_Item_Disable(hdlg, IDB_INSERT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT_PRESSED);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_ENABLE);
		KWIN_Item_Disable(hdlg, IDB_INSERT2_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_PRESSED);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_ENABLE);
		KWIN_Item_Disable(hdlg, IDB_REMOVE_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_REMOVE_PRESSED);
		KWIN_Item_Hide(hdlg, IDB_COPY_ENABLE);
		KWIN_Item_Disable(hdlg, IDB_COPY_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_COPY_PRESSED);
		//
		KWIN_Item_Hide(hdlg, IDB_LEFT_ENABLE);
		KWIN_Item_Disable(hdlg, IDB_LEFT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_LEFT_PRESSED);
		KWIN_Item_Hide(hdlg, IDB_RIGHT_ENABLE);
		KWIN_Item_Disable(hdlg, IDB_RIGHT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_RIGHT_PRESSED);
	}
	else
	{
		KWIN_Item_Enable(hdlg, IDB_INSERT_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT_PRESSED);
		KWIN_Item_Enable(hdlg, IDB_INSERT2_ENABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_DISABLE);
		KWIN_Item_Hide(hdlg, IDB_INSERT2_PRESSED);
		//
		if (READ_LAST_INDEX() < 1)
		{
			KWIN_Item_Hide(hdlg, IDB_REMOVE_ENABLE);
			KWIN_Item_Disable(hdlg, IDB_REMOVE_DISABLE);
			KWIN_Item_Hide(hdlg, IDB_REMOVE_PRESSED);
			KWIN_Item_Hide(hdlg, IDB_COPY_ENABLE);
			KWIN_Item_Disable(hdlg, IDB_COPY_DISABLE);
			KWIN_Item_Hide(hdlg, IDB_COPY_PRESSED);
			//
			KWIN_Item_Hide(hdlg, IDB_LEFT_ENABLE);
			KWIN_Item_Disable(hdlg, IDB_LEFT_DISABLE);
			KWIN_Item_Hide(hdlg, IDB_LEFT_PRESSED);
			KWIN_Item_Hide(hdlg, IDB_RIGHT_ENABLE);
			KWIN_Item_Disable(hdlg, IDB_RIGHT_DISABLE);
			KWIN_Item_Hide(hdlg, IDB_RIGHT_PRESSED);
		}
		else
		{
			KWIN_Item_Enable(hdlg, IDB_REMOVE_ENABLE);
			KWIN_Item_Hide(hdlg, IDB_REMOVE_DISABLE);
			KWIN_Item_Hide(hdlg, IDB_REMOVE_PRESSED);
			KWIN_Item_Enable(hdlg, IDB_COPY_ENABLE);
			KWIN_Item_Hide(hdlg, IDB_COPY_DISABLE);
			KWIN_Item_Hide(hdlg, IDB_COPY_PRESSED);

			//
			if (READ_FIRST_INDEX() == READ_CURRENT1_INDEX())
			{
				KWIN_Item_Hide(hdlg, IDB_LEFT_ENABLE);
				KWIN_Item_Disable(hdlg, IDB_LEFT_DISABLE);
				KWIN_Item_Hide(hdlg, IDB_LEFT_PRESSED);
			}
			else
			{
				KWIN_Item_Enable(hdlg, IDB_LEFT_ENABLE);
				KWIN_Item_Hide(hdlg, IDB_LEFT_DISABLE);
				KWIN_Item_Hide(hdlg, IDB_LEFT_PRESSED);
			}
			if (READ_LAST_INDEX() == READ_CURRENT1_INDEX())
			{
				KWIN_Item_Hide(hdlg, IDB_RIGHT_ENABLE);
				KWIN_Item_Disable(hdlg, IDB_RIGHT_DISABLE);
				KWIN_Item_Hide(hdlg, IDB_RIGHT_PRESSED);
			}
			else
			{
				KWIN_Item_Enable(hdlg, IDB_RIGHT_ENABLE);
				KWIN_Item_Hide(hdlg, IDB_RIGHT_DISABLE);
				KWIN_Item_Hide(hdlg, IDB_RIGHT_PRESSED);
			}
		}
	}
	//-------------------
	if (D_HAS_CONTROL)
	{
		KWIN_Item_Int_Display(hdlg, IDC_STATIC_STEP_ALL, READ_LAST_INDEX());
		if (READ_LAST_INDEX() < 1)
		{
			KWIN_Item_Hide(hdlg, IDC_STATIC_STEP_CURRENT1);
			KWIN_Item_Hide(hdlg, IDC_STATIC_STEP_CURRENT2);
		}
		else
		{
			KWIN_Item_Int_Display(hdlg, IDC_STATIC_STEP_CURRENT1, READ_CURRENT1_INDEX());
			KWIN_Item_Int_Display(hdlg, IDC_STATIC_STEP_CURRENT2, READ_CURRENT2_INDEX());
		}
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_STATIC_STEP_ALL);
		KWIN_Item_Hide(hdlg, IDC_STATIC_STEP_CURRENT1);
		KWIN_Item_Hide(hdlg, IDC_STATIC_STEP_CURRENT2);
	}
	ITEM_DRAW_FULL_DATA(hdlg);
}
//------------------------------------------------------------------------
void RECIPE_CONTROL_INIT_PROC(HWND hWnd)
{
	int i;
	SET_DISPLAY_INDEX(0, 0, 0, 0);
	D_HAS_CONTROL = FALSE;
	for (i = 0; i < MAX_DISPLAY_COUNT; i++)
		D_HAS_STEP[i] = FALSE;
}
//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_FILE_NEW_PROC(HWND hWnd, char *Buffer)
{
	char Filename[256];
	
	sprintf(Filename, "%s/%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), RECIPE_CONTROL_Get_GROUP_DIRECTORY(), RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer);
	Kor_Recipe_Initial();

	D_RECIPE0.udRcpType = g_RcpFileType;

	// 2004.07.14 by cskim, 'Step Group Cycle' Added
	D_RECIPE0.nStepGrpBegin = 0;
	D_RECIPE0.nStepGrpEnd = 0;
	D_RECIPE0.nStepGrpCyc = 0;
	D_RECIPE0.nIdleStopBegin = 0;

	strcpy(D_RECIPE0.szRecipePW, "");

	ITEM_DRAW_RECIPE_TYPE(hWnd);

	//	Kor_Recipe_Save( Filename );
	//	Kor_Recipe_Open( Filename );
	SET_DISPLAY_INDEX(1, Kor_StepCount(), 1, -1);
	D_HAS_CONTROL = TRUE;

	DATA_GET_FROM_FILE(TRUE);
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_FILE_SAVE_PROC(HWND hWnd, char *Buffer, int dType)
{
	int dLength = 0;
	int nNotOkStep, iRes = 0;

	char szMessage[256];
	char Filename[256];
	char szExtension[4];
	char *pszExtension;
	BOOL bHasExt = FALSE;

	// ycchoi(2006.02.03)
	extern char gszTgtGroup[MAX_PATH];

	// 2006.07.10 by cskim
	extern double g_dbStepTimeLowLimit;

	//	D_RECIPE0.udRcpType = g_RcpFileType;

	switch (D_RECIPE0.udRcpType)
	{
	case RCP_PRE:
		strcpy(szExtension, "pre");
		break;
	case RCP_PST:
		strcpy(szExtension, "pst");
		break;
	case RCP_IDL:
		strcpy(szExtension, "idl");
		break;
	case RCP_CLN:
		strcpy(szExtension, "cln");
		break;
	case RCP_PUG:
		strcpy(szExtension, "pug");
		break;
	case RCP_LPR:
		strcpy(szExtension, "lpr");
		break;
	case RCP_LPS:
		strcpy(szExtension, "lps");
		break;
	default:
		strcpy(szExtension, "rcp");
	}

	dLength = strlen(Buffer);

	if (dLength > 4)
	{
		pszExtension = Buffer + (dLength - 3);
		if ((!strcmp(pszExtension, szExtension)) && (Buffer[dLength - 4] == '.'))
			bHasExt = TRUE;
	}

	if (dType)
	{
		if (bHasExt)
			sprintf(Filename, "%s/%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), gszTgtGroup, RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer);
		else
			sprintf(Filename, "%s/%s/%s/%s.%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), gszTgtGroup, RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer, szExtension);
	}
	else
	{
		if (bHasExt)
			sprintf(Filename, "%s/%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), RECIPE_CONTROL_Get_GROUP_DIRECTORY(), RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer);
		else
			sprintf(Filename, "%s/%s/%s/%s.%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), RECIPE_CONTROL_Get_GROUP_DIRECTORY(), RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer, szExtension);
	}

	DATA_PUT_TO_FILE(TRUE);

	//// It is added for check recipe step 2006.03.27 ///////////////////////////////////////////////////////////////////////////////
	if (!IsValveRecipeTimeOK(&nNotOkStep))
	{
		// 2006.07.10 by cskim
		sprintf(szMessage, "  The recipe has an error!\n  Please check step[%d] recipe time!\n  The step time should be longer than %.2f sec!\n", nNotOkStep, g_dbStepTimeLowLimit);
		//MessageBox( hWnd,szMessage, "Error", MB_OK | MB_ICONERROR );
		MODAL_MESSAGE_BOX(hWnd, "Recipe Step Time Setting Error", szMessage, "OK", 0, &iRes);
		return FALSE;
	}
	//// It is added for check recipe step 2006.03.27 ///////////////////////////////////////////////////////////////////////////////

	return (Kor_Recipe_Save(Filename));
}
//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_FILE_OPEN_PROC(HWND hWnd, char *Buffer)
{
	char Filename[256];

	sprintf(Filename, "%s/%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), RECIPE_CONTROL_Get_GROUP_DIRECTORY(), RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer);
	Kor_Recipe_Open(Filename);
	SET_DISPLAY_INDEX(1, Kor_StepCount(), 1, -1);

	DATA_GET_FROM_FILE(TRUE);

	//  2013.04.02 Recipe Password Added by hyuk
	if (strcmp(D_RECIPE0.szRecipePW, "") == 0)
	{
		g_bRecipePWNull = TRUE;
	}
	else
	{
		g_bRecipePWNull = FALSE;
	}

	if (g_bReciepPW_CHK == TRUE || g_bRecipePWNull == TRUE)
	{
		D_HAS_CONTROL = TRUE;
	}
	else
	{
		D_HAS_CONTROL = FALSE;
	}
	//  2013.04.02 Recipe Password Added by hyuk
	return TRUE;
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_DELETE_STEP_PROC(HWND hWnd)
{
	int iRes = 0;
	char buffer[128] = {0, };

	if (READ_LAST_INDEX() < 1)
	{
		MessageBox(hWnd, "Step Delete Error..", "Last Step Error", MB_ICONQUESTION);
		return FALSE;
	}
	if (!MODAL_DIGITAL_BOX2(hWnd, "Step Select", "Delete Step", READ_CURRENT1_INDEX(), READ_CURRENT2_INDEX(), &iRes))
	{
		return FALSE;
	}

	if (iRes < READ_CURRENT1_INDEX() || iRes > READ_CURRENT2_INDEX())
	{
		MessageBox(hWnd, "Delete Step Error..", "Step ID Error", MB_ICONQUESTION);
		return FALSE;
	}
	DATA_PUT_TO_FILE(FALSE);
	Kor_StepDelete(iRes);
	SET_DISPLAY_INDEX(-1, READ_LAST_INDEX() - 1, -1, -1);
	if ((READ_CURRENT1_INDEX() > 1) && (READ_LAST_INDEX() - READ_CURRENT1_INDEX() + 1 < MAX_DISPLAY_COUNT))
	{
		SET_DISPLAY_INDEX(-1, -1, READ_CURRENT1_INDEX() - 1, -1);
	}
	if (READ_LAST_INDEX() < 1)
	{
		SET_DISPLAY_INDEX(-1, -1, 0, 0);
	}
	DATA_GET_FROM_FILE(FALSE);
	RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
	RECIPE_CONTROL_ALL_DATA_DISPLAY(hWnd);
	
	sprintf(buffer, "\t[File Edit] [%d] %s\n", iRes, "Step Delete Complete!!");
	SampleLogSave(buffer, "Item");
	
	return TRUE;
}
//------------------------------------------------------------------------
void RECIPE_CONTROL_EDIT_INSERT_PROC(HWND hWnd, int NewStepID)
{
	int Res = 1;
	char buffer[128] = {0, };
	char *g_pszInsertDataEnum[] = {"From Buffer", "With Blank Data"};

	if (Kor_BufferDataStatus())
	{
		if (!MODAL_DIGITAL_BOX1(hWnd, "Select", "Insert Data", "From Buffer|With Blank Data", &Res))
			return;
	}

	DATA_PUT_TO_FILE(FALSE);
	Kor_StepInsert(NewStepID, Res);
	SET_DISPLAY_INDEX(1, READ_LAST_INDEX() + 1, -1, -1);
	if (READ_CURRENT1_INDEX() < 1)
		SET_DISPLAY_INDEX(-1, -1, 1, -1);
	DATA_GET_FROM_FILE(FALSE);
	RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
	RECIPE_CONTROL_ALL_DATA_DISPLAY(hWnd);
	
	sprintf(buffer, "\t[File Edit] [%s] - [%d] %s\n", g_pszInsertDataEnum[Res], NewStepID, "Step Insert Complete!!");
	SampleLogSave(buffer, "Item");	
}
//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_COPY_STEP_PROC(HWND hWnd)
{
	int iRes = 0;
	char buffer[128] = {0, };

	if (READ_LAST_INDEX() < 1)
	{
		MessageBox(hWnd, "Step Copy Error..", "Last Step Error", MB_ICONQUESTION);
		return FALSE;
	}

	if (!MODAL_DIGITAL_BOX2(hWnd, "Step Select", "Copy Step", READ_CURRENT1_INDEX(), READ_CURRENT2_INDEX(), &iRes))
		return FALSE;

	if (iRes < READ_CURRENT1_INDEX() || iRes > READ_CURRENT2_INDEX())
	{
		MessageBox(hWnd, "Copy Step Error..", "Step ID Error", MB_ICONQUESTION);
		return FALSE;
	}

	Kor_BufferDataPut(&D_RECIPE[iRes - READ_CURRENT1_INDEX()]);
	
	sprintf(buffer, "\t[File Edit] [%d] %s\n", iRes, "Step Copy Complete!!");
	SampleLogSave(buffer, "Item");

	return TRUE;
}
//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_INSERT_BEFORE_PROC(HWND hWnd)
{
	int iRes = 0;

	if (READ_LAST_INDEX() >= MAX_STEP_COUNT)
	{
		MessageBox(hWnd, "Step Buffer is Full Error..", "Step Error", MB_ICONQUESTION);
		return FALSE;
	}
	else if (READ_LAST_INDEX() == 0)
	{
		RECIPE_CONTROL_EDIT_INSERT_PROC(hWnd, 1);
		return TRUE;
	}
	else if (READ_CURRENT1_INDEX() == READ_CURRENT2_INDEX())
	{
		RECIPE_CONTROL_EDIT_INSERT_PROC(hWnd, READ_CURRENT1_INDEX());
		return TRUE;
	}
	else
	{
		if (!MODAL_DIGITAL_BOX2(hWnd, "Step Select", "Insert Before", READ_CURRENT1_INDEX(), READ_CURRENT2_INDEX(), &iRes))
		{
			return FALSE;
		}
		if (iRes < READ_CURRENT1_INDEX() || iRes > READ_CURRENT2_INDEX())
		{
			MessageBox(hWnd, "Insert Before Error..", "Step ID Error", MB_ICONQUESTION);
			return FALSE;
		}
		RECIPE_CONTROL_EDIT_INSERT_PROC(hWnd, iRes);
		return TRUE;
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_INSERT_AFTER_PROC(HWND hWnd)
{
	int iRes = 0;

	if (READ_LAST_INDEX() >= MAX_STEP_COUNT)
	{
		MessageBox(hWnd, "Step Buffer is Full Error..", "Step Error", MB_ICONQUESTION);
		return FALSE;
	}
	else if (READ_LAST_INDEX() == 0)
	{
		RECIPE_CONTROL_EDIT_INSERT_PROC(hWnd, 1);
		return TRUE;
	}
	else if (READ_CURRENT1_INDEX() == READ_CURRENT2_INDEX())
	{
		RECIPE_CONTROL_EDIT_INSERT_PROC(hWnd, READ_CURRENT1_INDEX() + 1);
		return TRUE;
	}
	else
	{
		if (!MODAL_DIGITAL_BOX2(hWnd, "Step Select", "Insert After", READ_CURRENT1_INDEX(), READ_CURRENT2_INDEX(), &iRes))
		{
			return FALSE;
		}
		if (iRes < READ_CURRENT1_INDEX() || iRes > READ_CURRENT2_INDEX())
		{
			MessageBox(hWnd, "Insert After Error..", "Step ID Error", MB_ICONQUESTION);
			return FALSE;
		}
		RECIPE_CONTROL_EDIT_INSERT_PROC(hWnd, iRes + 1);
		return TRUE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void RECIPE_CONTROL_STEP_MOVE_PROC(HWND hWnd, BOOL LeftMove)
{
	DATA_PUT_TO_FILE(FALSE);
	if (LeftMove)
	{
		if (READ_FIRST_INDEX() >= READ_CURRENT1_INDEX())
			return;
		SET_DISPLAY_INDEX(-1, -1, READ_CURRENT1_INDEX() - 1, -1);
	}
	else
	{
		if (READ_LAST_INDEX() <= READ_CURRENT1_INDEX())
			return;
		SET_DISPLAY_INDEX(-1, -1, READ_CURRENT1_INDEX() + 1, -1);
	}
	DATA_GET_FROM_FILE(FALSE);
	RECIPE_CONTROL_ALL_DATA_DISPLAY(hWnd);
}
//------------------------------------------------------------------------
void RECIPE_CONTROL_STEP_MOVE_LEFT_PROC(HWND hWnd)
{
	RECIPE_CONTROL_STEP_MOVE_PROC(hWnd, TRUE);
}
//------------------------------------------------------------------------
void RECIPE_CONTROL_STEP_MOVE_RIGHT_PROC(HWND hWnd)
{
	RECIPE_CONTROL_STEP_MOVE_PROC(hWnd, FALSE);
}

//------------------------------------------------------------------------
// 2006.01.02
void ITEM_DRAW_PROC_TIME(HWND hdlg)
{
	char szBuf[40] = "";
	sprintf(szBuf, "%8.2f", GetProcTotalTime());
	KWIN_Item_String_Display(hdlg, IDC_EDT_PROC_TIME, szBuf);
}

//------------------------------------------------------------------------
// User Control Part 2. Data Item Drawing Part
//------------------------------------------------------------------------

void ITEM_DRAW_RECIPE_TYPE(HWND hdlg)
{
	char szRcp[9][20] = {"None", "Process", " Pre-Process", "Post-Process", "Idle", "Clean",
						 "Purge", "Lot Pre Recipe", "Lost Post Recipe"};
	char szBuf[40] = "";
	HWND hCheck;

	KWIN_Item_String_Display(hdlg, IDC_BUTTON_ITEM_RECIPE_TYPE, szRcp[D_RECIPE0.udRcpType]);

	// 2004.07.14 by cskim, 'Step Group Cycle' Added
	if (g_bStepGroup && D_RECIPE0.udRcpType != RCP_IDL)
	{
		KWIN_Item_Show(hdlg, IDC_STATIC_STEP_GROUP);
		sprintf(szBuf, "%d", D_RECIPE0.nStepGrpBegin);
		KWIN_Item_String_Display(hdlg, IDC_BUTTON_ITEM_STEP_GRP_BGN, szBuf);
		sprintf(szBuf, "%d", D_RECIPE0.nStepGrpEnd);
		KWIN_Item_String_Display(hdlg, IDC_BUTTON_ITEM_STEP_GRP_END, szBuf);
		sprintf(szBuf, "%d", D_RECIPE0.nStepGrpCyc);
		KWIN_Item_String_Display(hdlg, IDC_BUTTON_ITEM_STEP_GRP_CYC, szBuf);
	}
	else
	{
		if (!g_bStepGroup)
			KWIN_Item_Hide(hdlg, IDC_STATIC_STEP_GROUP);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEM_STEP_GRP_BGN);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEM_STEP_GRP_END);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEM_STEP_GRP_CYC);
	}

	// 2004.11.15 by cskim
	if (D_RECIPE0.udRcpType == RCP_IDL)
	{
		sprintf(szBuf, "%d", D_RECIPE0.nIdleStopBegin);
		KWIN_Item_Show(hdlg, IDC_STATIC_IDLE_STOP_BG);
		KWIN_Item_String_Display(hdlg, IDC_BUTTON_ITEM_IDLE_STOP_BGN, szBuf);
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_STATIC_IDLE_STOP_BG);
		KWIN_Item_Hide(hdlg, IDC_BUTTON_ITEM_IDLE_STOP_BGN);
	}

	// 2005.05.24 by cskim, for Keep Last Condition
	if (g_bKeepLastConEnable)
	{
		KWIN_Item_Show(hdlg, IDC_STATIC_KEEP_LAST_CON);
		KWIN_Item_Show(hdlg, IDC_KEEP_LAST_CON);
		hCheck = GetDlgItem(hdlg, IDC_KEEP_LAST_CON);
		if (D_RECIPE0.bKeepLastCon)
			SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else
	{
		if (!g_bKeepLastConEnable)
			KWIN_Item_Hide(hdlg, IDC_STATIC_KEEP_LAST_CON);
		KWIN_Item_Hide(hdlg, IDC_KEEP_LAST_CON);
	}

	// 2006.01.02
	ITEM_DRAW_PROC_TIME(hdlg);

	// 2006.01.25
	if (g_bThickness && (D_RECIPE0.udRcpType == RCP_PRC || D_RECIPE0.udRcpType == RCP_LPR))
	{
		KWIN_Item_Show(hdlg, IDC_STATIC_THICK);
		sprintf(szBuf, "%6.1f", D_RECIPE0.dbThickness);
		KWIN_Item_String_Display(hdlg, IDC_BTN_THICK, szBuf);
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_STATIC_THICK);
		KWIN_Item_Hide(hdlg, IDC_BTN_THICK);
	}

	// 2013.04.02 Recipe Password Added by hyuk
	if (g_bRecipePW)
		KWIN_Item_Show(hdlg, IDC_RECIPE_PW);
	else
		KWIN_Item_Hide(hdlg, IDC_RECIPE_PW);
}

//////////////////////////////////////////////////////////////////////////
// Multiple Step
//
void ITEM_DRAW_STEP(HWND hdlg, int i)
{
	int x, y;
	char szStep[10];

	HBRUSH hMyBrush, hOldBrush;
	HPEN hMyPen, hOldPen;

	HDC hDC;
	HFONT hfont, holdfont;

	hDC = GetDC(hdlg);
	hfont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
	hMyBrush = CreateSolidBrush(RGB(170, 170, 170));
	hMyPen = CreatePen(PS_SOLID, 0, RGB(170, 170, 170));

	hOldBrush = (HBRUSH)SelectObject(hDC, hMyBrush);
	hOldPen = (HPEN)SelectObject(hDC, hMyPen);
	holdfont = (HFONT)SelectObject(hDC, hfont);

	SetBkMode(hDC, OPAQUE);
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, RGB(170, 170, 170));

	if (i == 0)
	{
		x = 458;
		y = 50;
		Rectangle(hDC, 458, 50, 550, 75);
	}
	else if (i == 1)
	{
		x = 748;
		y = 50;
		Rectangle(hDC, 748, 50, 840, 75);
	}

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szStep, "Step %d  ", READ_CURRENT1_INDEX() + i);
		TextOut(hDC, x, y, szStep, strlen(szStep));
	}

	SelectObject(hDC, holdfont);
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);

	DeleteObject(hMyPen);
	DeleteObject(hMyBrush);
	DeleteObject(hfont);

	ReleaseDC(hdlg, hDC);
}

void ITEM_DRAW_CHAMBER_PRESSURE_SET_POINT(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_CHAMBER_PRES_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_CHAMBER_PRES_SET2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%10.3f", D_RECIPE[i].dbChamber_Pressure_Set_Point);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_CHAMBER_PRESSURE_SET_POINT_L(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_CHAMBER_PRES_SET_L1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_CHAMBER_PRES_SET_L2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%10.3f", D_RECIPE[i].dbChamber_Pressure_Set_Point_L);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

// 2005.12.28
void ITEM_DRAW_ITEM_B7(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BTN_ITEM_B7_1;
	else if (i == 1)
		pos = IDC_BTN_ITEM_B7_2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%d", D_RECIPE[i].nChamber_Pressure_Timeout);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_CHAMBER_PRESSURE_OPTION(HWND hdlg, int i)
{
	int pos;
	int dRes = D_RECIPE[i].udChamber_Pressure_Option;

	if (i == 0)
		pos = IDC_BUTTON_ITEM_CHAMBER_PRES_OPT1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_CHAMBER_PRES_OPT2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		KWIN_Item_String_Display(hdlg, pos, g_pszAlarmOptEnum[dRes]);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//////////////////////////////////////////////////////////////////////////
void ITEM_DRAW_MFC_RAMP_OPTION(HWND hdlg, int i)
{
	extern BOOL g_bMFC_RampUse;
	int pos;
	int dRes = D_RECIPE[i].nMfcRampOpt;

	if (!g_bMFC_RampUse)
		return;
	if (i == 0)
		pos = IDC_BTN_ITEM_B9_1;
	else if (i == 1)
		pos = IDC_BTN_ITEM_B9_2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
		KWIN_Item_String_Display(hdlg, pos, g_pszMfcRampOptEnum[dRes]);
	else
		KWIN_Item_Hide(hdlg, pos);
}

//////////////////////////////////////////////////////////////////////////
void ITEM_DRAW_THROTTLE_VALVE_SET_POINT(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_THROTTLE_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_THROTTLE_SET2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%10.3f", D_RECIPE[i].dbThrottle_Valve_Set_Point);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//////////////////////////////////////////////////////////////////////////
void ITEM_DRAW_THROTTLE_VALVE_OPTION(HWND hdlg, int i)
{
	int pos;
	int dRes = D_RECIPE[i].udThrottle_Valve_Option;

	if (i == 0)
		pos = IDC_BUTTON_ITEM_THROTTLE_OPT1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_THROTTLE_OPT2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		KWIN_Item_String_Display(hdlg, pos, g_pszTVOptEnum[dRes]);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

// ----------------------------------------------------------------------------------

void ITEM_DRAW_RF_PWR_SET_POINT(HWND hdlg, int i)
{
	int pos;
	char szData[15];
	extern BOOL g_bRF_SetPointUse;

	if (i == 0)
		pos = IDC_BUTTON_ITEM_RF_POWER_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_RF_POWER_SET2;

	// 2004/04/29 서병찬 CJ 요구사항 적용
	// RF Option이 INI 파일에서 No이면 화면에 Control이 나타나지 않는다.
	if (D_HAS_CONTROL && D_HAS_STEP[i] && g_bRF_SetPointUse)
	{
		sprintf(szData, "%5.1f", D_RECIPE[i].dbRF_PWR_Set_Point);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}
// ----------------------------------------------------------------------------------
// 2014.03.31
void ITEM_DRAW_RF2_PWR_SET_POINT(HWND hdlg, int i)
{
	int pos;
	char szData[15];
	extern BOOL g_bRF2_SetPointUse;

	if (i == 0)
		pos = IDC_BUTTON_ITEM_RF2_POWER_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_RF2_POWER_SET2;

	if (D_HAS_CONTROL && D_HAS_STEP[i] && g_bRF2_SetPointUse)
	{
		sprintf(szData, "%5.1f", D_RECIPE[i].dbRF2_PWR_Set_Point);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

// ----------------------------------------------------------------------------------
// Stage Heater Draw
// 2004.11.08 by cskim
void ITEM_DRAW_STAGE1_HTR_TEMP(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_STAGE1_TMP_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_STAGE1_TMP_SET2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%d", (int)D_RECIPE[i].dbStgHtr1_SetPoint);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_STAGE2_HTR_TEMP(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_STAGE2_TMP_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_STAGE2_TMP_SET2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%d", (int)D_RECIPE[i].dbStgHtr2_SetPoint);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_STAGE1_HTR_OPTION(HWND hdlg, int i)
{
	int pos;
	int dRes = D_RECIPE[i].nStgHtr1_Option;
	extern ITEM_STRUCT g_StgHtr;
	extern void Item_Draw_Enum(HWND hdlg, int dControl, int dRes, char *szEnum);

	if (i == 0)
		pos = IDC_BUTTON_ITEM_STAGE1_OPT1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_STAGE1_OPT2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		Item_Draw_Enum(hdlg, pos, dRes, g_StgHtr.szOptEnum);
		// KWIN_Item_String_Display( hdlg , pos , g_pszStgHtrEnum[ dRes ] );
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_STAGE2_HTR_OPTION(HWND hdlg, int i)
{
	int pos;
	int dRes = D_RECIPE[i].nStgHtr2_Option;
	extern ITEM_STRUCT g_StgHtr;
	extern void Item_Draw_Enum(HWND hdlg, int dControl, int dRes, char *szEnum);

	if (i == 0)
		pos = IDC_BUTTON_ITEM_STAGE2_OPT1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_STAGE2_OPT2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		Item_Draw_Enum(hdlg, pos, dRes, g_StgHtr.szOptEnum);
		// KWIN_Item_String_Display( hdlg , pos , g_pszStgHtrEnum[ dRes ] );
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_STAGE_POS_SET_POINT(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_STAGE_POS_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_STAGE_POS_SET2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%5.3f", D_RECIPE[i].dbSTAGE_POS_Set_Point);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_STAGE2_POS_SET_POINT(HWND hdlg, int i)
{
	int pos;
	char szData[15];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_STAGE2_POS_SET1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_STAGE2_POS_SET2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szData, "%5.3f", D_RECIPE[i].dbSTAGE2_POS_Set_Point);
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//////////////////////////////////////////////////////////////////////////
//... 2015.03.21 Add Position Move Option
void ITEM_DRAW_STG_POS_WAIT_OPT(HWND hdlg, int i)
{
	int pos;
	int dRes = D_RECIPE[i].nSTAGE_POS_WaitOpt;

	if (i == 0)
		pos = IDC_BUTTON_ITEM_STG_POS_WAIT_OPT1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_STG_POS_WAIT_OPT2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
		KWIN_Item_String_Display(hdlg, pos, g_pszStgPosWaitOptEnum[dRes]);
	else
		KWIN_Item_Hide(hdlg, pos);
}

// ----------------------------------------------------------------------------------
void ITEM_DRAW_VALV_CYC_PLUS_OPT(HWND hdlg, int i)
{
	int pos;
	char *szData[2] = {"No", "Yes"};

	if (i == 0)
		pos = IDC_BUTTON_ITEM_VALV_CYC_PLUS_OPT1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_VALV_CYC_PLUS_OPT2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		KWIN_Item_String_Display(hdlg, pos, szData[D_RECIPE[i].udVALV_CYC_PLUS_OPT]);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_RCP_CYCLE_CNT(HWND hdlg, int i)
{
	int pos;
	char szData[40];

	if (i == 0)
		pos = IDC_BUTTON_ITEM_VALV_CYC_CNT1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_VALV_CYC_CNT2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		// 		0: SFL_NORMAL_CYC,
		// 		1: SFL_WAIT_STGTEMP,
		// 		2: SFL_WAIT_IDLESTOP,
		// 		3: SFL_WAIT_HTRTEMP,
		// 		4: SFL_WAIT_STEPSYNC,
		// 		5: SFL_WAIT_RPG_ON,
		// 		6: SFL_WAIT_FUNC,

		// 2004.11.09 by cskim
		// 0:Normal 1:Unlimited Cycle And Temp Check 2: Cycle Count And Temp Check
		if (D_RECIPE[i].nStepFlow_Option == SFL_NORMAL_CYC)
			sprintf(szData, "%d", D_RECIPE[i].dValv_Rcp_Cycle_Cnt);
		else if (D_RECIPE[i].nStepFlow_Option == SFL_WAIT_STGTEMP)
			strcpy(szData, "Til_StgTmp");
		else if (D_RECIPE[i].nStepFlow_Option == SFL_WAIT_IDLESTOP)
			strcpy(szData, "Til_IdleStop");
		else if (D_RECIPE[i].nStepFlow_Option == SFL_WAIT_HTRTEMP)
			strcpy(szData, "Til_HtrTmp");
		else if (D_RECIPE[i].nStepFlow_Option == SFL_WAIT_STEPSYNC)
			strcpy(szData, "Til_StepSyn");
		//... 2015.03.07 Add RPG ON Step Skip
		else if (D_RECIPE[i].nStepFlow_Option == SFL_WAIT_RPG_ON)
			strcpy(szData, "StpNxt_RPG_On");
		else if (D_RECIPE[i].nStepFlow_Option == SFL_WAIT_FUNC)
			strcpy(szData, "Sub_Func");

		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

// 2005.05.24 by cskim, for Backside Helium Check
void ITEM_DRAW_ITEM_B6(HWND hdlg, int i)
{
	int pos;
	if (i == 0)
		pos = IDC_BTN_ITEM_B6_1;
	else if (i == 1)
		pos = IDC_BTN_ITEM_B6_2;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		KWIN_Item_Hide(hdlg, pos);
	}
	else
		KWIN_Item_Hide(hdlg, pos);
}

//--------------------------------------------------------------------------------
// 2005.05.19 by cskim

void ITEM_DRAW_MFC_SET_POINT(int nMfcNo, HWND hdlg, int i)
{
	int pos;
	double dbData = 0;
	int nIdx;

	if (nMfcNo > MAX_MFC_DISPLAY)
		return;
	nIdx = nMfcNo - 1;
	pos = g_MFC_SET_BTN[i][nIdx];

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		if (g_MFCData[nIdx].bUse)
		{
			strcpy(D_RECIPE[i].szMFC_RealName[nIdx], g_MFCData[nIdx].szMFCDisplayName);
			dbData = D_RECIPE[i].dbMFC_Set_Point[nIdx];
			KWIN_Item_Double_Display(hdlg, pos, dbData, 1);
		}
		else
		{
			KWIN_Item_Hide(hdlg, pos);
		}
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void ITEM_DRAW_MFC_OPTION(int dIndex, HWND hdlg, int i)
{
	int pos;
	char szData[10] = "";

	pos = g_MfcBtnB[i][dIndex].nID;
	strcpy(szData, g_MFCData[dIndex].szEnum[D_RECIPE[i].dMFC_Option[dIndex]]);
	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		if (g_MFCData[dIndex].bUse)
			KWIN_Item_String_Display(hdlg, pos, szData);
		else
			KWIN_Item_Hide(hdlg, pos);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_DMYMFC_SET_POINT(int nMfcNo, HWND hdlg, int i)
{
	int pos;
	double dbData = 0;
	int nIdx;

	if (nMfcNo > MAX_MFC_DISPLAY)
		return;
	nIdx = nMfcNo - 1;
	pos = g_MFC_SET_BTN_DMY[0][nIdx];

	//	printf("************************ g_MFC_SET_BTN_DMY[%d][%d] = %5.2f \n",i,nIdx,D_RECIPE[ i ].dbMFC_Set_Point_DMY[nIdx]);
	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		if (g_MFCData[nIdx].bUse)
		{
			strcpy(D_RECIPE[i].szMFC_RealName_DMY[nIdx], g_MFCData[nIdx].szMFCDisplayName);
			//			D_RECIPE_MFC[ i ].dbMFC_Set_Point_DMY[nIdx] = D_RECIPE[ i ].dbMFC_Set_Point_DMY[nIdx];
			//			printf("D_RECIPE_MFC[ %d ].dbMFC_Set_Point_DMY[%d] = %3.2f \n",i,nIdx,D_RECIPE_MFC[ i ].dbMFC_Set_Point_DMY[nIdx]);
			dbData = D_RECIPE_MFC[i].dbMFC_Set_Point_DMY[nIdx];
			//			printf("i: %d , pos: %d, mfc: %s, setpoint : %3.2f \n",i, pos,g_MFCData[ nIdx ].szMFCDisplayName,dbData);
			KWIN_Item_Double_Display(hdlg, pos, dbData, 1);
		}
		else
		{
			KWIN_Item_Hide(hdlg, pos);
		}
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void ITEM_DRAW_DMYMFC_OPTION(int dIndex, HWND hdlg, int i)
{
	int pos;
	char szData[10] = "";

	pos = g_MfcBtnBDMY[0][dIndex].nID;
	//	D_RECIPE_MFC[ i ].dMFC_Option_DMY[dIndex] = D_RECIPE[ i ].dMFC_Option_DMY[dIndex];

	strcpy(szData, g_MFCData[dIndex].szEnum[D_RECIPE_MFC[i].dMFC_Option_DMY[dIndex]]);
	//	printf("pos: %d, szData: %s \n",pos, szData);
	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		if (g_MFCData[dIndex].bUse)
			KWIN_Item_String_Display(hdlg, pos, szData);
		else
			KWIN_Item_Hide(hdlg, pos);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_SUB_FUNCTION(HWND hdlg, int i)
{
	int pos;
	int dRes = D_RECIPE[i].nSubFunc;
	extern void Item_Draw_Enum(HWND hdlg, int dControl, int dRes, char *szEnum);

	if (i == 0)
		pos = IDC_BUTTON_ITEM_SUB_FUNC1;
	else if (i == 1)
		pos = IDC_BUTTON_ITEM_SUB_FUNC2;

	if (D_RECIPE0.udRcpType != RCP_PRC && D_HAS_CONTROL && D_HAS_STEP[i])
	{
		Item_Draw_Enum(hdlg, pos, dRes, g_szSubFuncEnumStr);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void Show_Valve_Edit_Button(HWND hdlg, int nID, RecipeStepTemplateM *pRecipeStep)
{
	KWIN_Item_Show(hdlg, nID);
}

void ITEM_DRAW_VALVE_RECIPE(HWND hdlg, int i)
{
	int pos, pos2;
	HWND hCheck;

	if (i == 0)
	{
		pos = IDC_BUTTON_ITEM_VALV_RECIPE_EDIT1;
		pos2 = IDC_CHECK_ITEM_VALV_RECIPE1;
	}
	else
	{
		pos = IDC_BUTTON_ITEM_VALV_RECIPE_EDIT2;
		pos2 = IDC_CHECK_ITEM_VALV_RECIPE2;
	}

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		Show_Valve_Edit_Button(hdlg, pos, &D_RECIPE[i]);
		KWIN_Item_Show(hdlg, pos2);

		hCheck = GetDlgItem(hdlg, pos2);

		if (g_bDMY)
		{
			if (D_RECIPE[i].bHasValveData_DMY)
				SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
			else
				SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
		}
		else
		{
			if (D_RECIPE[i].bHasValveData)
				SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
			else
				SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
		}
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
		KWIN_Item_Hide(hdlg, pos2);
	}
}

void ITEM_DRAW_Sub_Item_Check(HWND hdlg, int i)
{
	int pos;
	HWND hCheck;

	if (i == 0)
	{
		pos = IDC_CHECK_SUB_ITEM1;
	}
	else
	{
		pos = IDC_CHECK_SUB_ITEM2;
	}

	hCheck = GetDlgItem(hdlg, pos);
	if (D_HAS_CONTROL && D_HAS_STEP[i] && g_bSubItemEdit)
	{
		KWIN_Item_Show(hdlg, pos);
		if (D_RECIPE[i].bSubItemEnable)
			SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_Sub_Item2_Check(HWND hdlg, int i) // 2006.09.04
{
	int pos;
	HWND hCheck;

	if (i == 0)
	{
		pos = IDC_CHECK_SUB_ITEM2_1;
	}
	else
	{
		pos = IDC_CHECK_SUB_ITEM2_2;
	}

	hCheck = GetDlgItem(hdlg, pos);
	if (D_HAS_CONTROL && D_HAS_STEP[i]) // && g_bSubItem2Edit )
	{
		KWIN_Item_Show(hdlg, pos);
		if (D_RECIPE[i].bSubItem2Enable)
			SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}
//--------------------------------------------------------------------------------
// 2006.08.18
void ITEM_DRAW_SUB_ITEM3(HWND hdlg, int i)
{
	int pos;

	if (i == 0)
		pos = IDC_BUTTON_SUB2_ITEM_1;
	else
		pos = IDC_BUTTON_SUB2_ITEM_2;

	if (D_HAS_CONTROL && D_HAS_STEP[i] && g_bSubItem2Edit)
		KWIN_Item_Show(hdlg, pos);
	else
		KWIN_Item_Hide(hdlg, pos);
}

//--------------------------------------------------------------------------------
// 2006.08.18
void ITEM_DRAW_SUB_ITEM2(HWND hdlg, int i)
{
	int pos;

	if (i == 0)
		pos = IDC_BUTTON_SUB_ITEM2_1;
	else
		pos = IDC_BUTTON_SUB_ITEM2_2;

	if (D_HAS_CONTROL && D_HAS_STEP[i] && g_bSubItem2Edit)
		KWIN_Item_Show(hdlg, pos);
	else
		KWIN_Item_Hide(hdlg, pos);
}

//--------------------------------------------------------------------------------
void ITEM_DRAW_SUB_ITEM(HWND hdlg, int i)
{
	int pos;

	if (i == 0)
		pos = IDC_BUTTON_SUB_ITEM_1;
	else
		pos = IDC_BUTTON_SUB_ITEM_2;

	if (D_HAS_CONTROL && D_HAS_STEP[i] && g_bSubItemEdit)
		KWIN_Item_Show(hdlg, pos);
	else
		KWIN_Item_Hide(hdlg, pos);
}

//--------------------------------------------------------------------------------
void ITEM_DRAW_STEP_NAME(HWND hdlg, int i)
{
	int pos;
	char szStepName[35];

	if (i == 0)
		pos = IDC_STEP1_NAME;
	else
		pos = IDC_STEP2_NAME;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		sprintf(szStepName, "%d. %s", READ_CURRENT1_INDEX() + i, D_RECIPE[i].szStepName);
		KWIN_Item_String_Display(hdlg, pos, szStepName);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//--------------------------------------------------------------------------------
// 2006.01.02

void ITEM_DRAW_RCP_STEP_TIME(HWND hdlg, int i)
{
	int pos;
	char szData[16];

	if (i == 0)
		pos = IDC_STEP1_TIME;
	else
		pos = IDC_STEP2_TIME;

	if (D_HAS_CONTROL && D_HAS_STEP[i])
	{
		if (D_RECIPE[i].nStepFlow_Option == SFL_NORMAL_CYC)
			sprintf(szData, "%6.2f", GetStepTime(&D_RECIPE[i]));
		else
			strcpy(szData, ".....");
		KWIN_Item_String_Display(hdlg, pos, szData);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

//--------------------------------------------------------------------------------

void Draw_Main_Recipe_Text(HWND hdlg, int dCtrlID)
{
	int left, top;
	char szRcp[] = "Main Rcp";

	HDC hDC;
	RECT rc, rc2;
	HWND hCtrl;

	hDC = GetDC(hdlg);
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 255));

	hCtrl = GetDlgItem(hdlg, dCtrlID);

	GetWindowRect(hdlg, &rc);
	GetWindowRect(hCtrl, &rc2);

	left = rc2.left - rc.left;
	top = rc2.top - rc.top;

	TextOut(hDC, left + 20, top, szRcp, strlen(szRcp));
	ReleaseDC(hdlg, hDC);
}

void Paint_Rect(HWND hdlg, RECT *rc)
{
	HDC hDC;
	HBRUSH hBr;

	hBr = CreateSolidBrush(RGB(172, 172, 172));

	hDC = GetDC(hdlg);
	FillRect(hDC, rc, hBr);
	ReleaseDC(hdlg, hDC);

	DeleteObject(hBr);
}

void ITEM_DRAW_Main_Recipe(HWND hdlg, int i)
{
	int pos;
	HWND hCheck;

	if (i == 0)
	{
		pos = IDC_MAIN_RCP1;
	}
	else
	{
		pos = IDC_MAIN_RCP2;
	}

	hCheck = GetDlgItem(hdlg, pos);
	if (D_HAS_CONTROL && D_HAS_STEP[i] && D_RECIPE0.udRcpType == RCP_PRC)
	{
		KWIN_Item_Show(hdlg, pos);
		if (D_RECIPE[i].bMainRcp)
			SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void ITEM_DRAW_Pre_Treat(HWND hdlg, int i)
{
	int pos;
	HWND hCheck;

	if (i == 0)
	{
		pos = IDC_PRE_TRE1;
	}
	else
	{
		pos = IDC_PRE_TRE2;
	}

	hCheck = GetDlgItem(hdlg, pos);
	if (D_HAS_CONTROL && D_HAS_STEP[i] && g_bPreTreat && D_RECIPE0.udRcpType == RCP_PRE)
	{
		KWIN_Item_Show(hdlg, pos);
		if (D_RECIPE[i].bPreTreat)
			SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else
	{
		KWIN_Item_Hide(hdlg, pos);
	}
}

void Draw_MFC_Contents(HWND hdlg)
{
	int i, j;
	for (i = 0; i < MAX_DISPLAY_COUNT; i++)
	{
		for (j = 0; j < MAX_MFC_DISPLAY; j++)
		{
			ITEM_DRAW_MFC_SET_POINT(j + 1, hdlg, i);
			ITEM_DRAW_MFC_OPTION(j, hdlg, i);
		}
	}
}

void Draw_DMYMFC_Contents(HWND hdlg)
{
	//	int i, j;
	int j;
	//	for ( i = 0 ; i < MAX_DISPLAY_COUNT ; i++ )
	//	{
	for (j = 0; j < MAX_MFC_DISPLAY; j++)
	{
		ITEM_DRAW_DMYMFC_SET_POINT(j + 1, hdlg, g_bDMY_Step2);
		ITEM_DRAW_DMYMFC_OPTION(j, hdlg, g_bDMY_Step2);
	}
	//	}
}

//--------------------------------------------------------------------------------
void ITEM_DRAW_FULL_DATA(HWND hdlg)
{
	int i, j;

	ITEM_DRAW_RECIPE_TYPE(hdlg);

	// Step Data
	for (i = 0; i < MAX_DISPLAY_COUNT; i++)
	{

		Check_RecipeStep_Data_Err(&D_RECIPE[i]);
		ITEM_DRAW_STEP_NAME(hdlg, i);
		ITEM_DRAW_RCP_STEP_TIME(hdlg, i);

		ITEM_DRAW_CHAMBER_PRESSURE_SET_POINT(hdlg, i);
		ITEM_DRAW_CHAMBER_PRESSURE_SET_POINT_L(hdlg, i);
		ITEM_DRAW_CHAMBER_PRESSURE_OPTION(hdlg, i);
		ITEM_DRAW_ITEM_B7(hdlg, i); // 2005.12.28

		ITEM_DRAW_THROTTLE_VALVE_SET_POINT(hdlg, i);
		ITEM_DRAW_THROTTLE_VALVE_OPTION(hdlg, i);
		ITEM_DRAW_RF_PWR_SET_POINT(hdlg, i);
		ITEM_DRAW_RF2_PWR_SET_POINT(hdlg, i); // 2014.03.31

		ITEM_DRAW_STAGE_POS_SET_POINT(hdlg, i);
		ITEM_DRAW_STAGE2_POS_SET_POINT(hdlg, i);
		ITEM_DRAW_STG_POS_WAIT_OPT(hdlg, i); //... 2015.03.21 Add Position Move Option

		ITEM_DRAW_VALV_CYC_PLUS_OPT(hdlg, i);

		// 2005.12.28
		ITEM_DRAW_MFC_RAMP_OPTION(hdlg, i);
		ITEM_DRAW_RCP_CYCLE_CNT(hdlg, i);

		for (j = 0; j < MAX_MFC_DISPLAY; j++)
		{
			ITEM_DRAW_MFC_SET_POINT(j + 1, hdlg, i);
			ITEM_DRAW_MFC_OPTION(j, hdlg, i);
		}

		ITEM_DRAW_VALVE_RECIPE(hdlg, i);
		ITEM_DRAW_SUB_ITEM(hdlg, i);
		ITEM_DRAW_SUB_ITEM2(hdlg, i);
		ITEM_DRAW_SUB_ITEM3(hdlg, i);
		ITEM_DRAW_Sub_Item_Check(hdlg, i);
		ITEM_DRAW_Sub_Item2_Check(hdlg, i); // 2006.09.04

		ITEM_DRAW_STAGE1_HTR_TEMP(hdlg, i);
		ITEM_DRAW_STAGE1_HTR_OPTION(hdlg, i);
		ITEM_DRAW_STAGE2_HTR_TEMP(hdlg, i);
		ITEM_DRAW_STAGE2_HTR_OPTION(hdlg, i);

		ITEM_DRAW_Main_Recipe(hdlg, i);
		ITEM_DRAW_Pre_Treat(hdlg, i);

		// 2005.05.24 by cskim, for Backside Helium Check
		ITEM_DRAW_ITEM_B6(hdlg, i);

		// Added by jhlee. 2009.11.25 for ROR Check
		ITEM_DRAW_SUB_FUNCTION(hdlg, i);
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// User Control Part 3. Data Item Setting Part
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//
//
BOOL Check_Valve_Data(void)
{
	int i;

	for (i = 0; i < MAX_DISPLAY_COUNT; i++)
	{
		if (D_RECIPE[i].bHasValveData)
			return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_RECIPE_TYPE(HWND hWnd)
{
	int iRes = g_RcpFileType - 1;

	//	if (( !D_HAS_CONTROL ) && ( !g_bNEW_FLAG ) )		return FALSE;
	if (!g_bNEW_FLAG)
		return FALSE;
	//	if ( Check_Valve_Data() == TRUE )					return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Recipe Type", g_szRcpTypeOptEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (g_RcpFileType - 1 != iRes)
		{
			sprintf(buffer, "\t[Main Item] %s - From [%s] to [%s]\n", "Recipe Type", g_szRcpTypeOptEnum[g_RcpFileType - 1], g_szRcpTypeOptEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		g_RcpFileType = iRes + 1;
		D_RECIPE0.udRcpType = iRes + 1;
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		return TRUE;
	}
	return FALSE;
}

// -----------------------------------------------------------
// 2004.07.14 by cskim, 'Step Group Cycle' Added
BOOL ITEM_BUTTON_STEP_GRP_BGN(HWND hWnd)
{
	int iData = D_RECIPE0.nStepGrpBegin;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX2(hWnd, "Select", "Step Group Begin", 0, 60, &iData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE0.nStepGrpBegin != iData)
		{
			sprintf(buffer, "\t[Main Item] %s - From [%d] to [%d]\n", "Step Group Begin", D_RECIPE0.nStepGrpBegin, iData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE0.nStepGrpBegin = iData;
		// 2006.01.02
		PutGrpStepCycleData(&D_RECIPE0);
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_STEP_GRP_END(HWND hWnd)
{
	int iData = D_RECIPE0.nStepGrpEnd;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX2(hWnd, "Select", "Step Group End", 0, 60, &iData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE0.nStepGrpEnd != iData)
		{
			sprintf(buffer, "\t[Main Item] %s - From [%d] to [%d]\n", "Step Group End", D_RECIPE0.nStepGrpEnd, iData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE0.nStepGrpEnd = iData;
		// 2006.01.02
		PutGrpStepCycleData(&D_RECIPE0);
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_STEP_GRP_CYC(HWND hWnd)
{
	int iData = D_RECIPE0.nStepGrpCyc;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX2(hWnd, "Select", "Step Group Cycle", 0, 1000, &iData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE0.nStepGrpCyc != iData)
		{
			sprintf(buffer, "\t[Main Item] %s - From [%d] to [%d]\n", "Step Group Cycle", D_RECIPE0.nStepGrpCyc, iData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE0.nStepGrpCyc = iData;
		// 2006.01.02
		PutGrpStepCycleData(&D_RECIPE0);
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------
// 2004.11.18 by cskim
BOOL ITEM_BUTTON_IDLE_STOP_BGN(HWND hWnd)
{
	int iData = D_RECIPE0.nIdleStopBegin;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX2(hWnd, "Select", "Idle Stopping Begin Step", 0, 60, &iData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE0.nIdleStopBegin != iData)
		{
			sprintf(buffer, "\t[Main Item] %s - From [%d] to [%d]\n", "Idle Stopping Begin Step", D_RECIPE0.nIdleStopBegin, iData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE0.nIdleStopBegin = iData;
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------
BOOL ITEM_CHECK_KEEP_LAST_CON(HWND hdlg)
{
	int iRes = 0;
	char szTitle[40];
	HWND hCheck;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (D_RECIPE0.bKeepLastCon)
		iRes = 1;
	strcpy(szTitle, "Keeping Last Condition Option");

	if (MODAL_DIGITAL_BOX1(hdlg, "Select", szTitle, g_szKeepLastConEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE0.bKeepLastCon != iRes)
		{
			sprintf(buffer, "\t[Main Item] %s - From [%s] to [%s]\n", "Keeping Last Condition Option", g_szKeepLastConEnum[D_RECIPE0.bKeepLastCon], g_szKeepLastConEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		hCheck = GetDlgItem(hdlg, IDC_KEEP_LAST_CON);
		if (iRes == 0)
		{
			D_RECIPE0.bKeepLastCon = FALSE;
			SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
		}
		else
		{
			D_RECIPE0.bKeepLastCon = TRUE;
			SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
		}
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------
// 2006.01.25
BOOL ITEM_BUTTON_THICKNESS(HWND hWnd)
{
	double dbData = D_RECIPE0.dbThickness;
	extern double g_dbThickness_Min;
	extern double g_dbThickness_Max;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Thickness for Process", g_dbThickness_Min, g_dbThickness_Max, 1, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE0.dbThickness != dbData)
		{
			sprintf(buffer, "\t[Main Item] %s - From [%f] to [%f]\n", "Thickness for Process", D_RECIPE0.dbThickness, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE0.dbThickness = dbData;
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		return TRUE;
	}

	return FALSE;
}

// -----------------------------------------------------------

BOOL ITEM_BUTTON_CHAMBER_PRESSURE_SETPOINT(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbChamber_Pressure_Set_Point;
	extern double g_dbChm_MinPres;
	extern double g_dbChm_MaxPres;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Process Pressure High Limit", g_dbChm_MinPres, g_dbChm_MaxPres, 3, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbChamber_Pressure_Set_Point != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Process Pressure High Limit", D_RECIPE[i].dbChamber_Pressure_Set_Point, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbChamber_Pressure_Set_Point = dbData;
		ITEM_DRAW_CHAMBER_PRESSURE_SET_POINT(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_CHAMBER_PRESSURE_SETPOINT_L(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbChamber_Pressure_Set_Point_L;
	extern double g_dbChm_MinPres;
	extern double g_dbChm_MaxPres;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Process Pressure Low Limit", g_dbChm_MinPres, g_dbChm_MaxPres, 3, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbChamber_Pressure_Set_Point_L != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Process Pressure Low Limit", D_RECIPE[i].dbChamber_Pressure_Set_Point_L, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbChamber_Pressure_Set_Point_L = dbData;
		ITEM_DRAW_CHAMBER_PRESSURE_SET_POINT_L(hWnd, i); //... 2015.10.05 Bug Fix
		return TRUE;
	}

	return FALSE;
}

// 2005.12.28
BOOL ITEM_BUTTON_ITEM_B7(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].nChamber_Pressure_Timeout;
	extern double g_dbChm_MinPres;
	extern double g_dbChm_MaxPres;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Process Pressure Timeout", 0, 100, 0, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].nChamber_Pressure_Timeout != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Process Pressure Timeout", D_RECIPE[i].nChamber_Pressure_Timeout, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].nChamber_Pressure_Timeout = (int)dbData;
		ITEM_DRAW_ITEM_B7(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_CHAMBER_PRESSURE_OPTION(HWND hWnd, int i)
{
	NOYES iRes = D_RECIPE[i].udChamber_Pressure_Option;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Process Pressure Monitoring Option", g_szAlarmOptEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].udChamber_Pressure_Option != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "Process Pressure Monitoring Option", g_pszAlarmOptEnum[D_RECIPE[i].udChamber_Pressure_Option], g_pszAlarmOptEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].udChamber_Pressure_Option = iRes;
		ITEM_DRAW_CHAMBER_PRESSURE_OPTION(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

// 2014.06.27
BOOL ITEM_BUTTON_TXT_SAVE(HWND hWnd)
{
	extern BOOL Txt_Recipe_Save(char *RecipeFileName);

	if (!D_HAS_CONTROL)
		return FALSE;
	Txt_Recipe_Save(NULL);
	MessageBox(hWnd, "Success. Save to CSV file", "Confirm", MB_OK);
	return FALSE;
}

// 2005.12.28
BOOL ITEM_BUTTON_MFC_RAMP_OPTION(HWND hWnd, int i)
{
	NOYES iRes = D_RECIPE[i].nMfcRampOpt;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "MFC Ramping Option", g_szMfcRampOptEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].nMfcRampOpt != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "MFC Ramping Option", g_pszMfcRampOptEnum[D_RECIPE[i].nMfcRampOpt], g_pszMfcRampOptEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].nMfcRampOpt = iRes;
		ITEM_DRAW_MFC_RAMP_OPTION(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------------------------------------
BOOL ITEM_BUTTON_THROTTLE_VALVE_SETPOINT(HWND hWnd, int i)
{
	double fMin = 0;
	double fMax = 9999;
	int nDev = 3;
	double dbData = D_RECIPE[i].dbThrottle_Valve_Set_Point;
	extern double g_dbTV_MinPres;
	extern double g_dbTV_MaxPres;
	extern int g_nTV_MinPos;
	extern int g_nTV_MaxPos;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (POSITION == D_RECIPE[i].udThrottle_Valve_Option)
	{
		fMin = g_nTV_MinPos;
		fMax = g_nTV_MaxPos;
		nDev = 1;
	}
	else
	{
		fMin = g_dbTV_MinPres;
		fMax = g_dbTV_MaxPres;
		nDev = 3;
	}

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Throttle Valve [Setpoint]", fMin, fMax, nDev, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbThrottle_Valve_Set_Point != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Throttle Valve [Setpoint]", D_RECIPE[i].dbThrottle_Valve_Set_Point, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbThrottle_Valve_Set_Point = dbData;
		ITEM_DRAW_THROTTLE_VALVE_SET_POINT(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_THROTTLE_VALVE_OPTION(HWND hWnd, int i)
{
	int iRes = D_RECIPE[i].udThrottle_Valve_Option;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Throttle Valve [Option]", g_szTVOptEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		// 2006.02.24
		if (D_RECIPE[i].udThrottle_Valve_Option != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "Throttle Valve [Option]", g_pszTVOptEnum[D_RECIPE[i].udThrottle_Valve_Option], g_pszTVOptEnum[iRes]);
			SampleLogSave(buffer, "Item");

			if (D_RECIPE[i].udThrottle_Valve_Option == 0 || iRes == 0)
			{
				D_RECIPE[i].dbThrottle_Valve_Set_Point = 0;
				ITEM_DRAW_THROTTLE_VALVE_SET_POINT(hWnd, i);
			}
		}
		D_RECIPE[i].udThrottle_Valve_Option = iRes;
		ITEM_DRAW_THROTTLE_VALVE_OPTION(hWnd, i);

		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------------------------------------
BOOL ITEM_BUTTON_RF_PWR_SETPOINT(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbRF_PWR_Set_Point;
	extern int g_nRF_MinPower;
	extern int g_nRF_MaxPower;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "RF 1 Power [Setpoint]", g_nRF_MinPower, g_nRF_MaxPower, 1, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbRF_PWR_Set_Point != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "RF 1 Power [Setpoint]", D_RECIPE[i].dbRF_PWR_Set_Point, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbRF_PWR_Set_Point = dbData;
		ITEM_DRAW_RF_PWR_SET_POINT(hWnd, i);
		return TRUE;
	}

	return FALSE;
}
//---------------------------------------------------------------------------------------------------------
// 2014.03.31
BOOL ITEM_BUTTON_RF2_PWR_SETPOINT(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbRF2_PWR_Set_Point;
	extern int g_nRF2_MinPower;
	extern int g_nRF2_MaxPower;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "RF 2 Power [Setpoint]", g_nRF2_MinPower, g_nRF2_MaxPower, 1, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbRF2_PWR_Set_Point != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "RF 2 Power [Setpoint]", D_RECIPE[i].dbRF2_PWR_Set_Point, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbRF2_PWR_Set_Point = dbData;
		ITEM_DRAW_RF2_PWR_SET_POINT(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

// ----------------------------------------------------------------------------------------
// Stage Heater Setting
// 2004.11.08 by cskim
BOOL ITEM_BUTTON_STAGE1_HTR_TEMP(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbStgHtr1_SetPoint;
	extern ITEM_STRUCT g_StgHtr;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Stage Heater 1 [Setpoint]", g_StgHtr.dbMin, g_StgHtr.dbMax, 0, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbStgHtr1_SetPoint != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Stage Heater 1 [Setpoint]", D_RECIPE[i].dbStgHtr1_SetPoint, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbStgHtr1_SetPoint = dbData;
		ITEM_DRAW_STAGE1_HTR_TEMP(hWnd, i);
		return TRUE;
	}
	return FALSE;
}

BOOL ITEM_BUTTON_STAGE2_HTR_TEMP(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbStgHtr2_SetPoint;
	extern ITEM_STRUCT g_StgHtr;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Stage Heater 2 [Setpoint]", g_StgHtr.dbMin, g_StgHtr.dbMax, 0, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbStgHtr2_SetPoint != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Stage Heater 2 [Setpoint]", D_RECIPE[i].dbStgHtr2_SetPoint, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbStgHtr2_SetPoint = dbData;
		ITEM_DRAW_STAGE2_HTR_TEMP(hWnd, i);
		return TRUE;
	}
	return FALSE;
}

BOOL ITEM_BUTTON_STAGE1_HTR_OPT(HWND hWnd, int i)
{
	NOYES iRes = D_RECIPE[i].nStgHtr1_Option;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Stage Heater 1 [Option]", g_szStgHtrOptEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].nStgHtr1_Option != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "Stage Heater 1 [Option]", g_szStgHtrOptEnum[D_RECIPE[i].nStgHtr1_Option], g_szStgHtrOptEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].nStgHtr1_Option = iRes;
		// 2006.01.02
		ITEM_DRAW_STAGE1_HTR_OPTION(hWnd, i);
		Check_RecipeStep_Data_Err(&D_RECIPE[i]);
		ITEM_DRAW_Sub_Item_Check(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_STAGE2_HTR_OPT(HWND hWnd, int i)
{
	NOYES iRes = D_RECIPE[i].nStgHtr2_Option;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Stage Heater 2 [Option]", g_szStgHtrOptEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].nStgHtr2_Option != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "Stage Heater 2 [Option]", g_szStgHtrOptEnum[D_RECIPE[i].nStgHtr2_Option], g_szStgHtrOptEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].nStgHtr2_Option = iRes;
		// 2006.01.02
		ITEM_DRAW_STAGE2_HTR_OPTION(hWnd, i);
		Check_RecipeStep_Data_Err(&D_RECIPE[i]);
		ITEM_DRAW_Sub_Item_Check(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_STAGE_POS_SETPOINT(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbSTAGE_POS_Set_Point;
	extern int g_nStgHtrPos_Min;
	extern int g_nStgHtrPos_Max;
	extern int g_nStgHtrPos_DecPnt;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Stage 1 Position [Setpoint]", g_nStgHtrPos_Min, g_nStgHtrPos_Max, g_nStgHtrPos_DecPnt, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbSTAGE_POS_Set_Point != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Stage 1 Position [Setpoint]", D_RECIPE[i].dbSTAGE_POS_Set_Point, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbSTAGE_POS_Set_Point = dbData;
		ITEM_DRAW_STAGE_POS_SET_POINT(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_STAGE2_POS_SETPOINT(HWND hWnd, int i)
{
	double dbData = D_RECIPE[i].dbSTAGE2_POS_Set_Point;
	extern int g_nStgHtrPos_Min;
	extern int g_nStgHtrPos_Max;
	extern int g_nStgHtrPos_DecPnt;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_ANALOG_BOX1(hWnd, "Select", "Stage 2 Position [Setpoint]", g_nStgHtrPos_Min, g_nStgHtrPos_Max, g_nStgHtrPos_DecPnt, &dbData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dbSTAGE2_POS_Set_Point != dbData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, "Stage 2 Position [Setpoint]", D_RECIPE[i].dbSTAGE2_POS_Set_Point, dbData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dbSTAGE2_POS_Set_Point = dbData;
		ITEM_DRAW_STAGE2_POS_SET_POINT(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//... 2015.03.21 Add Position Move Option
BOOL ITEM_BUTTON_STG_POS_WAIT_OPT(HWND hWnd, int i)
{
	int iData = D_RECIPE[i].nSTAGE_POS_WaitOpt;

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Stage Position Wait Opt", g_szStgPosWaitOptEnumStr, &iData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].nSTAGE_POS_WaitOpt != iData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "Stage Position Wait Opt", g_pszStgPosWaitOptEnum[D_RECIPE[i].nSTAGE_POS_WaitOpt], g_pszStgPosWaitOptEnum[iData]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].nSTAGE_POS_WaitOpt = iData;
		ITEM_DRAW_STG_POS_WAIT_OPT(hWnd, i);

		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_SUB_FUNCTION(HWND hWnd, int i)
{
	NOYES iRes = D_RECIPE[i].nSubFunc;
	extern void Valve_Data_Init2(RecipeStepTemplateM * pRecipeStep);

	if (!D_HAS_CONTROL)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Sub Function", g_szSubFuncEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].nSubFunc != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "Sub Function", g_pszSubFuncEnum[D_RECIPE[i].nSubFunc], g_pszSubFuncEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].nSubFunc = iRes;
		ITEM_DRAW_SUB_FUNCTION(hWnd, i);

		if (D_RECIPE[i].nSubFunc != SF_NONE)
		{
			D_RECIPE[i].nStepFlow_Option = SFL_WAIT_FUNC;
			switch (D_RECIPE[i].nSubFunc)
			{
			case SF_LEAK_CHECK:
				Valve_Data_Init2(&D_RECIPE[i]);
				break;
			}
		}
		else
		{
			if (D_RECIPE[i].nStepFlow_Option == SFL_WAIT_FUNC)
				D_RECIPE[i].nStepFlow_Option = SFL_NORMAL_CYC;
		}

		return TRUE;
	}
	return FALSE;
}

BOOL ITEM_BUTTON_VALV_CYC_PLUS_OPT(HWND hWnd, int i)
{
	NOYES iRes = D_RECIPE[i].udVALV_CYC_PLUS_OPT;

	extern BOOL g_bPreventiveOptEdit;

	if (!D_HAS_CONTROL || !g_bPreventiveOptEdit)
		return FALSE;

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", "Preventive Maint Option", g_szValveCycOptEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].udVALV_CYC_PLUS_OPT != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, "Sub Function", g_szValveCycOptEnum[D_RECIPE[i].udVALV_CYC_PLUS_OPT], g_szValveCycOptEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].udVALV_CYC_PLUS_OPT = iRes;
		ITEM_DRAW_VALV_CYC_PLUS_OPT(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_VALV_CYC_CNT(HWND hWnd, int i)
{
	int iData = D_RECIPE[i].dValv_Rcp_Cycle_Cnt;

	if (!D_HAS_CONTROL)
		return FALSE;
	if (D_RECIPE[i].nStepFlow_Option != SFL_NORMAL_CYC)
		return FALSE;

	if (MODAL_DIGITAL_BOX2(hWnd, "Select", "Valve Recipe Cycle Count", 1, 30000, &iData))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].dValv_Rcp_Cycle_Cnt != iData)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%d] to [%d]\n", READ_CURRENT1_INDEX() + i, "Valve Recipe Cycle Count", D_RECIPE[i].dValv_Rcp_Cycle_Cnt, iData);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		D_RECIPE[i].dValv_Rcp_Cycle_Cnt = iData;
		ITEM_DRAW_RCP_CYCLE_CNT(hWnd, i);
		// 2005.01.02
		ITEM_DRAW_RCP_STEP_TIME(hWnd, i);
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------------------
// 2005.05.19 by cskim

BOOL ITEM_BUTTON_MFC_SETPOINT(int nMfcNo, HWND hWnd, int i)
{
	char szTitle[40];
	double dbMin, dbMax;
	double dbData, dbTempData;
	int nIdx;

	nIdx = nMfcNo - 1;
	if (!D_HAS_CONTROL)
		return FALSE;

	if (g_bDMY)
	{
		dbData = D_RECIPE_MFC[i].dbMFC_Set_Point_DMY[nIdx];
		dbTempData = D_RECIPE_MFC[i].dbMFC_Set_Point_DMY[nIdx];
	}
	else
	{
		dbData = D_RECIPE[i].dbMFC_Set_Point[nIdx];
		dbTempData = D_RECIPE[i].dbMFC_Set_Point[nIdx];
	}

	dbMin = g_MFCData[nIdx].dbMin;
	dbMax = g_MFCData[nIdx].dbMax;

	sprintf(szTitle, "Set Point[%s]", g_MFCData[nIdx].szMFCDisplayName);

	if (MODAL_ANALOG_BOX1(hWnd, "Select", szTitle, dbMin, dbMax, 1, &dbData))
	{
		char buffer[128] = {0, };

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		// modified by jhlee. 2009.07.31 for dummy recipe
		if (g_bDMY)
		{
			if (dbTempData != dbData)
			{
				sprintf(buffer, "\t[Dummy Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, szTitle, dbTempData, dbData);
				SampleLogSave(buffer, "Item");
			}
			//printf("D_RECIPE_MFC[%d].dbMFC_SetPoint_DMY[%d] = %5.2f \n",i,nIdx,dbData);
			D_RECIPE_MFC[i].dbMFC_Set_Point_DMY[nIdx] = dbData;
			ITEM_DRAW_DMYMFC_SET_POINT(nMfcNo, hWnd, i);
		}
		else
		{
			if (dbTempData != dbData)
			{
				sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%f] to [%f]\n", READ_CURRENT1_INDEX() + i, szTitle, dbTempData, dbData);
				SampleLogSave(buffer, "Item");
			}
			D_RECIPE[i].dbMFC_Set_Point[nIdx] = dbData;
			ITEM_DRAW_MFC_SET_POINT(nMfcNo, hWnd, i);
		}

		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------------------

BOOL ITEM_BUTTON_MFC_OPTION(HWND hWnd, int nBtnID)
{
	int loopi;
	int dIndex, i;
	int iRes;
	char szTitle[40];
	char szEnumeration[255] = "";

	if (!D_HAS_CONTROL)
		return FALSE;

	if (g_bDMY)
		dIndex = GetMfcNoFromBtnIDDMY(nBtnID, &i) - 1;
	else
		dIndex = GetMfcNoFromBtnID(nBtnID, &i) - 1;

	if (dIndex < 0)
		return FALSE;

	sprintf(szTitle, "MFC Monitoring Option[%s]", g_MFCData[dIndex].szMFCDisplayName);

	for (loopi = 0; loopi < 9; loopi++) // 2006.06.30 by cskim
	{
		if ((!strcmp(g_MFCData[dIndex].szEnum[loopi], "-")) ||
			(strlen(g_MFCData[dIndex].szEnum[loopi]) == 0))
		{
			break;
		}
		else
		{
			if (strlen(szEnumeration) == 0)
				strcpy(szEnumeration, g_MFCData[dIndex].szEnum[loopi]);
			else
			{
				strcat(szEnumeration, "|");
				strcat(szEnumeration, g_MFCData[dIndex].szEnum[loopi]);
			}
		}
	}

	if (g_bDMY)
	{
		iRes = D_RECIPE_MFC[g_bDMY_Step2].dMFC_Option_DMY[dIndex];
		//		printf("1. D_RECIPE_MFC[%d].dMFC_Option_DMY[%d] = %d \n",g_bDMY_Step2,dIndex,iRes);
	}
	else
		iRes = D_RECIPE[i].dMFC_Option[dIndex];

	if (MODAL_DIGITAL_BOX1(hWnd, "Select", szTitle, szEnumeration, &iRes))
	{
		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		if (iRes < 5 || iRes > 7)
		{
			char buffer[128] = {0, };
			
			if (g_bDMY)
			{
				if (D_RECIPE_MFC[g_bDMY_Step2].dMFC_Option_DMY[dIndex] != iRes)
				{
					sprintf(buffer, "\t[Dummy Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, szTitle, g_pszMfcOptEnum[D_RECIPE_MFC[g_bDMY_Step2].dMFC_Option_DMY[dIndex]], g_pszMfcOptEnum[D_RECIPE_MFC[g_bDMY_Step2].dMFC_Option_DMY[iRes]]);
					SampleLogSave(buffer, "Item");
					D_RECIPE_MFC[g_bDMY_Step2].dMFC_Option_DMY[dIndex] = iRes; // jhlee
					//printf("2. D_RECIPE_MFC[%d].dMFC_Option_DMY[%d] = %d \n",g_bDMY_Step2,dIndex,iRes);
					ITEM_DRAW_DMYMFC_OPTION(dIndex, hWnd, i);
				}
			}
			else
			{
				if (D_RECIPE[i].dMFC_Option[dIndex] != iRes)
				{
					sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, szTitle, g_pszMfcOptEnum[D_RECIPE[i].dMFC_Option[dIndex]], g_pszMfcOptEnum[D_RECIPE[i].dMFC_Option[iRes]]);
					SampleLogSave(buffer, "Item");
					D_RECIPE[i].dMFC_Option[dIndex] = iRes; // 2006.06.30 by cskim
					ITEM_DRAW_MFC_OPTION(dIndex, hWnd, i);
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_VALV_RECIPE_EDIT(HWND hWnd, int i)
{
	int l, m;
	BOOL bHasData;
	BOOL bUpdate = FALSE;
	BOOL bEdit = FALSE;
	extern BOOL OnCVDSchDlg(HWND hdlg, VALVE_RCP_DATA * pValveData, BOOL bEdit);

	g_bMainValveRecipeFlag = TRUE;
	if (!D_HAS_CONTROL)
		return FALSE;
	g_ProcessStep = READ_CURRENT1_INDEX() + i;
	g_bHasValveData = D_RECIPE[i].bHasValveData;
	g_pRcpValveData = &D_RECIPE[i].udRCP_Valve_Data;

	g_bDMY = FALSE;
	bHasData = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VALVE_EDITOR), hWnd, Valve_Msg_Handler);

	if (bHasData == 1)
	{

		//mgsong
		g_pRcpValveData = &D_RECIPE[i].udRCP_Valve_Data;
		for (l = 0; l < 8; l++)
			for (m = 0; m < 21; m++)
			{
				D_RECIPE[i].udRCP_Valve_Data_DMY.RCP_DATA[l][m].dbTime = g_pRcpValveData->RCP_DATA[l][m].dbTime;
			}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);

		if (g_pRcpValveData->dTotalStep > 0 && g_pRcpValveData->RCP_DATA[0][0].dbTime > 0.0)
			D_RECIPE[i].bHasValveData = TRUE; //swhan
		else
			D_RECIPE[i].bHasValveData = FALSE;

		//		printf("dTotalStep : %d, dbTime : %d \n",g_pRcpValveData->dTotalStep,g_pRcpValveData->RCP_DATA[0][0].dbTime);

		D_RECIPE0.udRcpType = g_RcpFileType;
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		ITEM_DRAW_VALVE_RECIPE(hWnd, i);

		return TRUE;
	}

	return FALSE;
}
BOOL ITEM_BUTTON_VALV_RECIPE_EDIT_DMY(HWND hWnd, int i)
{
	BOOL bHasData;
	BOOL bUpdate = FALSE;
	BOOL bEdit = FALSE;
	int dMFC_Count, ii, jj;
	extern BOOL OnCVDSchDlg(HWND hdlg, VALVE_RCP_DATA * pValveData, BOOL bEdit);

	if (!g_bMainValveRecipeFlag)
	{
		MessageBox(hWnd, "Please Check < ValveRcp Edit >. !!!", "Confirm", MB_OK);
		return FALSE;
	}

	if (!D_HAS_CONTROL)
		return FALSE;
	g_ProcessStep = READ_CURRENT1_INDEX() + i;
	g_bHasValveData_DMY = D_RECIPE[i].bHasValveData_DMY;

	// Dummy MFC Data를 Sub Item으로 넘겨 줌.
	// Dummy Recipe Write. 2009.08.18 Added by jhlee
	for (dMFC_Count = 0; dMFC_Count < MFC_BUF_LENGTH; dMFC_Count++)
	{
		D_RECIPE_MFC[i].dbMFC_Set_Point_DMY[dMFC_Count] = D_RECIPE[i].dbMFC_Set_Point_DMY[dMFC_Count];
		D_RECIPE_MFC[i].dMFC_Option_DMY[dMFC_Count] = D_RECIPE[i].dMFC_Option_DMY[dMFC_Count];
	}
	//	printf("D_RECIPE_MFC[%d].bDMYRecipCopy = %d \n",i,D_RECIPE_MFC[i].bDMYRecipCopy);
	//	if(!D_RECIPE_MFC[i].bDMYRecipCopy);
	g_pRcpValveData_DMY = &D_RECIPE[i].udRCP_Valve_Data_DMY;

	for (ii = 0; ii < MAX_VAL_STEP_NO; ii++)
	{
		for (jj = 0; jj < MAX_VAL_TIME_STEP; jj++)
		{
			g_pRcpValveData_DMY_TMP.RCP_DATA[ii][jj] = D_RECIPE[i].udRCP_Valve_Data.RCP_DATA[ii][jj];
		}
	}

	//	printf("2. g_DummyRecipeInit = %d\n",g_DummyRecipeInit);

	g_bDMY = TRUE;
	g_bDMY_Step2 = i;

	bHasData = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VALVE_EDITOR), hWnd, Valve_Msg_Handler);

	g_bDMY = FALSE;

	if (bHasData == 1)
	{
		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);

		if (g_pRcpValveData_DMY->dTotalStep > 0 && g_pRcpValveData_DMY->RCP_DATA[0][0].dbTime > 0.0)
		{
			D_RECIPE[i].bHasValveData_DMY = TRUE; //swhan

			// Dummy Recipe Write. 2009.08.19 Added by jhlee
			for (dMFC_Count = 0; dMFC_Count < MFC_BUF_LENGTH; dMFC_Count++)
			{
				D_RECIPE[i].dbMFC_Set_Point_DMY[dMFC_Count] = D_RECIPE_MFC[i].dbMFC_Set_Point_DMY[dMFC_Count];
				D_RECIPE[i].dMFC_Option_DMY[dMFC_Count] = D_RECIPE_MFC[i].dMFC_Option_DMY[dMFC_Count];
			}

			memcpy(&D_RECIPE[i].udRCP_Valve_Data_DMY, g_pRcpValveData_DMY, sizeof(VALVE_RCP_DATA));

			D_RECIPE[i].bSubItem2Enable = TRUE;
			ITEM_DRAW_Sub_Item2_Check(hWnd, i);
		}
		else
		{
			D_RECIPE[i].bHasValveData_DMY = FALSE;
		}

		D_RECIPE0.udRcpType = g_RcpFileType;
		ITEM_DRAW_RECIPE_TYPE(hWnd);
		ITEM_DRAW_VALVE_RECIPE(hWnd, i);

		return TRUE;
	}

	return FALSE;
}

// 2004.10.25 by cskim
ITEM_BUTTON_SUB_ITEM_EDIT(HWND hdlg, int i)
{
	if (!D_HAS_CONTROL)
		return FALSE;

	g_ProcessStep = READ_CURRENT1_INDEX() + i;
	pSubItem_D_RECIPE = &D_RECIPE[i];
	DialogBox(GETHINST(hdlg), MAKEINTRESOURCE(IDD_SUB_ITEM), hdlg, SubItem_Msg_Handler);
	ITEM_DRAW_Sub_Item_Check(hdlg, i);
	return FALSE;
}

// 2006.08.18
ITEM_BUTTON_SUB_ITEM2_EDIT(HWND hdlg, int i)
{
	if (!D_HAS_CONTROL)
		return FALSE;

	g_ProcessStep = READ_CURRENT1_INDEX() + i;
	pSubItem_D_RECIPE = &D_RECIPE[i];
	DialogBox(GETHINST(hdlg), MAKEINTRESOURCE(IDD_SUB_ITEM2), hdlg, SubItem2_Msg_Handler);
	ITEM_DRAW_Sub_Item2_Check(hdlg, i); // 2006.09.04

	return FALSE;
}

BOOL ITEM_BUTTON_STEP_NAME(HWND hdlg, int i)
{
	char szTitle[40];
	char szStepName[40];

	if (!D_HAS_CONTROL)
		return FALSE;

	sprintf(szTitle, "Step %d Name", READ_CURRENT1_INDEX() + i);
	strcpy(szStepName, D_RECIPE[i].szStepName);

	if (MODAL_STRING_BOX1(hdlg, "Select", szTitle, D_RECIPE[i].szStepName))
	{
		char buffer[128] = {0, };

		if (strcmp(szStepName, D_RECIPE[i].szStepName))
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, szTitle, szStepName, D_RECIPE[i].szStepName);
			SampleLogSave(buffer, "Item");
		}

		RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
		ITEM_DRAW_STEP_NAME(hdlg, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_MAIN_RCP(HWND hdlg, int i)
{
	int iRes = D_RECIPE[i].bMainRcp;
	char szTitle[40];

	if (!D_HAS_CONTROL)
		return FALSE;

	sprintf(szTitle, "Step %d  - Select Main Recipe", READ_CURRENT1_INDEX() + i);

	if (MODAL_DIGITAL_BOX1(hdlg, "Select", szTitle, g_szSelectedEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].bMainRcp != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, szTitle, g_pszSelectedEnum[D_RECIPE[i].bMainRcp], g_pszSelectedEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		D_RECIPE[i].bMainRcp = iRes;
		ITEM_DRAW_Main_Recipe(hdlg, i);
		return TRUE;
	}

	return FALSE;
}

BOOL ITEM_BUTTON_PRE_TRE(HWND hdlg, int i)
{
	int iRes = D_RECIPE[i].bPreTreat;
	char szTitle[40];

	if (!D_HAS_CONTROL)
		return FALSE;
	sprintf(szTitle, "Step %d  - Pre-Treatment", READ_CURRENT1_INDEX() + i);

	if (MODAL_DIGITAL_BOX1(hdlg, "Select", szTitle, g_szPreTreEnumStr, &iRes))
	{
		char buffer[128] = {0, };

		if (D_RECIPE[i].bPreTreat != iRes)
		{
			sprintf(buffer, "\t[Main Item] STEP:[%d] %s - From [%s] to [%s]\n", READ_CURRENT1_INDEX() + i, szTitle, g_szPreTreEnum[D_RECIPE[i].bPreTreat], g_szPreTreEnum[iRes]);
			SampleLogSave(buffer, "Item");
		}

		D_RECIPE[i].bPreTreat = iRes;
		ITEM_DRAW_Pre_Treat(hdlg, i);
		return TRUE;
	}
	return FALSE;
}

// 2005.05.24 by cskim, for Backside Helium Check
BOOL ITEM_BUTTON_ITEM_B6(HWND hdlg, int i)
{
	BOOL bRet = FALSE;
	int nID;

	if (!D_HAS_CONTROL)
		return bRet;

	if (i == 0)
		nID = IDC_BTN_ITEM_B6_1;
	else if (i == 1)
		nID = IDC_BTN_ITEM_B6_2;

	return bRet;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// ycchoi(2006.02.03)
// EDIT Button 추가(NRD요청사항)
#define _READONLY_CHECK_                                                                               \
	if (!gbEditMode)                                                                                   \
	{                                                                                                  \
		MessageBox(hdlg, "Operation Impossible(Read Only Mode)", "Operation Impossible", MB_ICONHAND); \
		return TRUE;                                                                                   \
	}

//
//
//
//
//------------------------------------------------------------------------
//------------------------------------------------------------------------
// User Control Part 4. Button Handling Part
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOL RECIPE_CONTROL_BUTTON_HANDLING(HWND hdlg, long wParam, BOOL *Change, BOOL *Redraw)
{
	int nID;
	int nIdx;
	*Change = FALSE;
	*Redraw = FALSE;

	nID = LOWORD(wParam);
	switch (nID)
	{
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	// Fixed Item
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	case IDB_LEFT_ENABLE:
		RECIPE_CONTROL_STEP_MOVE_LEFT_PROC(hdlg);
		return TRUE;
	case IDB_RIGHT_ENABLE:
		RECIPE_CONTROL_STEP_MOVE_RIGHT_PROC(hdlg);
		return TRUE;
	case IDB_INSERT_ENABLE:
		_READONLY_CHECK_ *Change = RECIPE_CONTROL_INSERT_BEFORE_PROC(hdlg);
		return TRUE;
	case IDB_INSERT2_ENABLE:
		_READONLY_CHECK_ *Change = RECIPE_CONTROL_INSERT_AFTER_PROC(hdlg);
		return TRUE;
	case IDB_REMOVE_ENABLE:
		_READONLY_CHECK_ *Change = RECIPE_CONTROL_DELETE_STEP_PROC(hdlg);
		return TRUE;
	case IDB_COPY_ENABLE:
		_READONLY_CHECK_ *Change = RECIPE_CONTROL_COPY_STEP_PROC(hdlg);
		return TRUE;

		//----------------------------------------------------------------------------------------
		//----------------------------------------------------------------------------------------
		// User Defined Button Control
		//----------------------------------------------------------------------------------------

	case IDC_BUTTON_ITEM_STEP_GRP_BGN:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STEP_GRP_BGN(hdlg);
		return TRUE;
	case IDC_BUTTON_ITEM_STEP_GRP_END:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STEP_GRP_END(hdlg);
		return TRUE;
	case IDC_BUTTON_ITEM_STEP_GRP_CYC:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STEP_GRP_CYC(hdlg);
		return TRUE;
	case IDC_BUTTON_ITEM_IDLE_STOP_BGN:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_IDLE_STOP_BGN(hdlg);
		return TRUE;
	case IDC_KEEP_LAST_CON:
		_READONLY_CHECK_ *Change = ITEM_CHECK_KEEP_LAST_CON(hdlg);
		return TRUE;
	// 2006.01.25
	case IDC_BTN_THICK:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_THICKNESS(hdlg);
		return TRUE;

	case IDC_BUTTON_ITEM_CHAMBER_PRES_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_CHAMBER_PRESSURE_SETPOINT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_CHAMBER_PRES_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_CHAMBER_PRESSURE_SETPOINT(hdlg, 1);
		return TRUE;
	case IDC_BUTTON_ITEM_CHAMBER_PRES_SET_L1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_CHAMBER_PRESSURE_SETPOINT_L(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_CHAMBER_PRES_SET_L2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_CHAMBER_PRESSURE_SETPOINT_L(hdlg, 1);
		return TRUE;
	// 2005.12.28
	case IDC_BTN_ITEM_B7_1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_ITEM_B7(hdlg, 0);
		return TRUE;
	case IDC_BTN_ITEM_B7_2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_ITEM_B7(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_CHAMBER_PRES_OPT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_CHAMBER_PRESSURE_OPTION(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_CHAMBER_PRES_OPT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_CHAMBER_PRESSURE_OPTION(hdlg, 1);
		return TRUE;

	//2014.06.27
	case IDC_BUTTON_RCP_EXPORT:
		*Change = ITEM_BUTTON_TXT_SAVE(hdlg);
		return TRUE;

	// 2005.12.28
	case IDC_BTN_ITEM_B9_1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_MFC_RAMP_OPTION(hdlg, 0);
		return TRUE;
	case IDC_BTN_ITEM_B9_2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_MFC_RAMP_OPTION(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_THROTTLE_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_THROTTLE_VALVE_SETPOINT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_THROTTLE_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_THROTTLE_VALVE_SETPOINT(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_THROTTLE_OPT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_THROTTLE_VALVE_OPTION(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_THROTTLE_OPT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_THROTTLE_VALVE_OPTION(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_RF_POWER_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_RF_PWR_SETPOINT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_RF_POWER_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_RF_PWR_SETPOINT(hdlg, 1);
		return TRUE;
	//2014.03.31
	case IDC_BUTTON_ITEM_RF2_POWER_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_RF2_PWR_SETPOINT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_RF2_POWER_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_RF2_PWR_SETPOINT(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_STAGE1_TMP_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE1_HTR_TEMP(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_STAGE1_TMP_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE1_HTR_TEMP(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_STAGE2_TMP_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE2_HTR_TEMP(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_STAGE2_TMP_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE2_HTR_TEMP(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_STAGE1_OPT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE1_HTR_OPT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_STAGE1_OPT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE1_HTR_OPT(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_STAGE2_OPT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE2_HTR_OPT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_STAGE2_OPT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE2_HTR_OPT(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_STAGE_POS_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE_POS_SETPOINT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_STAGE_POS_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE_POS_SETPOINT(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_STAGE2_POS_SET1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE2_POS_SETPOINT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_STAGE2_POS_SET2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STAGE2_POS_SETPOINT(hdlg, 1);
		return TRUE;

	//... 2015.03.21 Add Position Move Option
	case IDC_BUTTON_ITEM_STG_POS_WAIT_OPT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STG_POS_WAIT_OPT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_STG_POS_WAIT_OPT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STG_POS_WAIT_OPT(hdlg, 1);
		return TRUE;

	// Added by jhlee 2009.121.25 ROR Check
	case IDC_BUTTON_ITEM_SUB_FUNC1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_SUB_FUNCTION(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_SUB_FUNC2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_SUB_FUNCTION(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_VALV_CYC_PLUS_OPT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_CYC_PLUS_OPT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_VALV_CYC_PLUS_OPT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_CYC_PLUS_OPT(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM_VALV_CYC_CNT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_CYC_CNT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_VALV_CYC_CNT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_CYC_CNT(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_ITEM1_OPT1:
	case IDC_BUTTON_ITEM1_OPT2:
	case IDC_BUTTON_ITEM2_OPT1:
	case IDC_BUTTON_ITEM2_OPT2:
	case IDC_BUTTON_ITEM3_OPT1:
	case IDC_BUTTON_ITEM3_OPT2:
	case IDC_BUTTON_ITEM4_OPT1:
	case IDC_BUTTON_ITEM4_OPT2:
	case IDC_BUTTON_ITEM5_OPT1:
	case IDC_BUTTON_ITEM5_OPT2:
	case IDC_BUTTON_ITEM6_OPT1:
	case IDC_BUTTON_ITEM6_OPT2:
	case IDC_BUTTON_ITEM7_OPT1:
	case IDC_BUTTON_ITEM7_OPT2:
	case IDC_BUTTON_ITEM8_OPT1:
	case IDC_BUTTON_ITEM8_OPT2:
	case IDC_BUTTON_ITEM9_OPT1:
	case IDC_BUTTON_ITEM9_OPT2:
	case IDC_BUTTON_ITEM10_OPT1:
	case IDC_BUTTON_ITEM10_OPT2:
	case IDC_BUTTON_ITEM11_OPT1:
	case IDC_BUTTON_ITEM11_OPT2:
	case IDC_BUTTON_ITEM12_OPT1:
	case IDC_BUTTON_ITEM12_OPT2:
	case IDC_BUTTON_ITEM13_OPT1:
	case IDC_BUTTON_ITEM13_OPT2:
	case IDC_BUTTON_ITEM14_OPT1:
	case IDC_BUTTON_ITEM14_OPT2:
	case IDC_BUTTON_ITEM15_OPT1:
	case IDC_BUTTON_ITEM15_OPT2:
	case IDC_BUTTON_ITEM16_OPT1:
	case IDC_BUTTON_ITEM16_OPT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_MFC_OPTION(hdlg, nID);
		return TRUE;

	case IDC_BUTTON_ITEM_VALV_RECIPE_EDIT1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_RECIPE_EDIT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_ITEM_VALV_RECIPE_EDIT2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_RECIPE_EDIT(hdlg, 1);
		return TRUE;

	case IDC_STEP1_NAME:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STEP_NAME(hdlg, 0);
		return TRUE;
	case IDC_STEP2_NAME:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_STEP_NAME(hdlg, 1);
		return TRUE;

	case IDC_MAIN_RCP1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_MAIN_RCP(hdlg, 0);
		return TRUE;
	case IDC_MAIN_RCP2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_MAIN_RCP(hdlg, 1);
		return TRUE;

	case IDC_PRE_TRE1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_PRE_TRE(hdlg, 0);
		return TRUE;
	case IDC_PRE_TRE2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_PRE_TRE(hdlg, 1);
		return TRUE;

	case IDC_BUTTON_SUB_ITEM_1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_SUB_ITEM_EDIT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_SUB_ITEM_2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_SUB_ITEM_EDIT(hdlg, 1);
		return TRUE;
	//2014.04.24
	case IDC_BUTTON_SUB2_ITEM_1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_SUB_ITEM2_EDIT(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_SUB2_ITEM_2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_SUB_ITEM2_EDIT(hdlg, 1);
		return TRUE;
	//----------------------------------------------------------------------------------------
	// 2006.08.18
	case IDC_BUTTON_SUB_ITEM2_1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_RECIPE_EDIT_DMY(hdlg, 0);
		return TRUE;
	case IDC_BUTTON_SUB_ITEM2_2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_VALV_RECIPE_EDIT_DMY(hdlg, 1);
		return TRUE;
	// 2005.05.24 by cskim, for Backside Helium Check
	case IDC_BTN_ITEM_B6_1:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_ITEM_B6(hdlg, 0);
		return TRUE;
	case IDC_BTN_ITEM_B6_2:
		_READONLY_CHECK_ *Change = ITEM_BUTTON_ITEM_B6(hdlg, 1);
		return TRUE;
	//----------------------------------------------------------------------------------------
	default:
		if (g_bDMY == FALSE)
		{
			nIdx = FindIdx(nID, g_MFC_SET_BTN[0], MAX_MFC_DISPLAY);
			if (nIdx >= 0)
			{
				_READONLY_CHECK_ *Change = ITEM_BUTTON_MFC_SETPOINT(nIdx + 1, hdlg, 0);
				return TRUE;
			}
			nIdx = FindIdx(nID, g_MFC_SET_BTN[1], MAX_MFC_DISPLAY);
			if (nIdx >= 0)
			{
				_READONLY_CHECK_ *Change = ITEM_BUTTON_MFC_SETPOINT(nIdx + 1, hdlg, 1);
				return TRUE;
			}
		}
		break;
		//----------------------------------------------------------------------------------------
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void SampleLogSave(char *buffer, char *m_stOption)
{

	FILE *fpt;
	SYSTEMTIME st;
	BOOL m_bTime;

	if (strcmp(m_stOption, "Item") == 0)
	{
		m_bTime = FALSE;
	}
	else
	{
		sprintf(g_szSampleLogFileName, "system/%s_RECIPE_EDIT.cfg", m_stOption);
		m_bTime = TRUE;
	}

	fpt = fopen(g_szSampleLogFileName, "a");

	if (m_bTime)
	{
		GetLocalTime(&st);
		fprintf(fpt, "%04d/%02d/%02d %02d:%02d:%02d:%03d\t%s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buffer);
	}
	else
	{
		fprintf(fpt, buffer);
	}
	
	fclose(fpt);
}
void SampleLogDell(char *m_szModule)
{
	FILE *fpt = NULL;
	
	sprintf(g_szSampleLogFileName, "system/%s_RECIPE_EDIT.cfg", m_szModule);
	
	if ((fpt = fopen(g_szSampleLogFileName, "w")) == NULL)
	{
		printf("Cannot Open RECIPE_EDIT.cfg File\n");
	}
	fprintf(fpt, "");
	fclose(fpt);
}
void RcpEditLog(char *m_szModule)
{
	FILE *fp = NULL;
	FILE *fpt = NULL;
	SYSTEMTIME st;
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;

	char FileName[256], RcpNameBuf[128];
	char mbuf[1024];
	long bufSize, fileSize;

	GetLocalTime(&st);
	sprintf(RcpNameBuf, "%04d%02d%02d_%s", st.wYear, st.wMonth, st.wDay, m_szModule);
	sprintf(FileName, "%s\\%s.log", RecipeEditLogFolder, RcpNameBuf);

	//Logging Create folder
	hSrch = FindFirstFile(RecipeEditLogFolder, &wfd);
	if (hSrch == INVALID_HANDLE_VALUE)
	{
		if (!CreateDirectory(RecipeEditLogFolder, NULL))
		{
			printf("[FFU_DRIVER] : Can't Create Log File\n");
			FindClose(hSrch);
			return;
		}
	}
	FindClose(hSrch);

	// File Logging
	fp = fopen(FileName, "a+");
	fpt = fopen(g_szSampleLogFileName, "r");

	if(!fp){
		printf("[RecipeEdit] Can't Create Log File\n");
		return;
	}

	if (!fp)
	{
		printf("[RecipeEdit] Can't Create Log File\n");
		return;
	}

	fseek(fpt, 0, SEEK_END);
	fileSize = ftell(fpt);
	rewind(fpt);

	// file size > 1024(mbuf size) ==> while
	while (fileSize > 0)
	{
		memset(mbuf, 0x00, sizeof(mbuf));
		if (fileSize > 1024)
		{
			bufSize = 1024;
		}
		else
		{
			bufSize = fileSize;
		}

		fread(mbuf, bufSize, 1, fpt);
		fwrite(mbuf, strlen(mbuf), 1, fp);
		fileSize -= bufSize;
	}
	
	printf("[RecipeEdit - %s] %s Save Complete!! \n", m_szModule, g_szSampleLogFileName);
	
	fclose(fpt);
	fclose(fp);
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
