// 2006.08.18 Created by cskim for Sub Item(2)

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
extern	BOOL					g_bHeaterSetUse;

//------------ 2005.12.13 by ycchoi ----------->>
extern	HWND	hWnd_ValveRecipe;
extern	HWND	hWnd_DetailValueRecipe;
extern	HWND	HWnd_SubItemRecipe;
//------------ 2005.12.13 by ycchoi -----------<<


extern void Item_Draw_Int( HWND hdlg, int dControl, int dRes );
extern void Item_Draw_Enum( HWND hdlg, int dControl, int dRes, char *szEnum);
extern BOOL Item_Set_Point( HWND hdlg, int dControl, double *pdbValue, ITEM_STRUCT *pItemStruct);
//... 2016.02.29
extern BOOL Item_HeaterSet_Point( HWND hdlg, int dControl, double *pdbValue, ITEM_STRUCT *pItemStruct);
//...
extern BOOL Item_Set_Option( HWND hdlg, int dControl, int *pnValue, ITEM_STRUCT *pItemStruct);


int g_pnHtrCtrl[MAX_HEATER_NO][3] =
{
	IDC_BTN_HTR_NAME, IDC_BTN_HTR_SP, IDC_BTN_HTR_OPT,
	IDC_BTN_HTR_NAME2, IDC_BTN_HTR_SP2, IDC_BTN_HTR_OPT2,
	IDC_BTN_HTR_NAME3, IDC_BTN_HTR_SP3, IDC_BTN_HTR_OPT3,
	IDC_BTN_HTR_NAME4, IDC_BTN_HTR_SP4, IDC_BTN_HTR_OPT4,
	IDC_BTN_HTR_NAME5, IDC_BTN_HTR_SP5, IDC_BTN_HTR_OPT5,
	IDC_BTN_HTR_NAME6, IDC_BTN_HTR_SP6, IDC_BTN_HTR_OPT6,
	IDC_BTN_HTR_NAME7, IDC_BTN_HTR_SP7, IDC_BTN_HTR_OPT7,
	IDC_BTN_HTR_NAME8, IDC_BTN_HTR_SP8, IDC_BTN_HTR_OPT8,
	IDC_BTN_HTR_NAME9, IDC_BTN_HTR_SP9, IDC_BTN_HTR_OPT9,
	IDC_BTN_HTR_NAME10, IDC_BTN_HTR_SP10, IDC_BTN_HTR_OPT10,
	IDC_BTN_HTR_NAME11, IDC_BTN_HTR_SP11, IDC_BTN_HTR_OPT11,
	IDC_BTN_HTR_NAME12, IDC_BTN_HTR_SP12, IDC_BTN_HTR_OPT12,
	IDC_BTN_HTR_NAME13, IDC_BTN_HTR_SP13, IDC_BTN_HTR_OPT13,
	IDC_BTN_HTR_NAME14, IDC_BTN_HTR_SP14, IDC_BTN_HTR_OPT14,
	IDC_BTN_HTR_NAME15, IDC_BTN_HTR_SP15, IDC_BTN_HTR_OPT15,
	IDC_BTN_HTR_NAME16, IDC_BTN_HTR_SP16, IDC_BTN_HTR_OPT16,
	IDC_BTN_HTR_NAME17, IDC_BTN_HTR_SP17, IDC_BTN_HTR_OPT17,
	IDC_BTN_HTR_NAME18, IDC_BTN_HTR_SP18, IDC_BTN_HTR_OPT18,
	IDC_BTN_HTR_NAME19, IDC_BTN_HTR_SP19, IDC_BTN_HTR_OPT19,
	IDC_BTN_HTR_NAME20, IDC_BTN_HTR_SP20, IDC_BTN_HTR_OPT20,
	IDC_BTN_HTR_NAME21, IDC_BTN_HTR_SP21, IDC_BTN_HTR_OPT21,
	IDC_BTN_HTR_NAME22, IDC_BTN_HTR_SP22, IDC_BTN_HTR_OPT22,
	IDC_BTN_HTR_NAME23, IDC_BTN_HTR_SP23, IDC_BTN_HTR_OPT23,
	IDC_BTN_HTR_NAME24, IDC_BTN_HTR_SP24, IDC_BTN_HTR_OPT24,
	IDC_BTN_HTR_NAME25, IDC_BTN_HTR_SP25, IDC_BTN_HTR_OPT25,
	IDC_BTN_HTR_NAME26, IDC_BTN_HTR_SP26, IDC_BTN_HTR_OPT26,
	IDC_BTN_HTR_NAME27, IDC_BTN_HTR_SP27, IDC_BTN_HTR_OPT27,
	IDC_BTN_HTR_NAME28, IDC_BTN_HTR_SP28, IDC_BTN_HTR_OPT28,
	IDC_BTN_HTR_NAME29, IDC_BTN_HTR_SP29, IDC_BTN_HTR_OPT29,
	IDC_BTN_HTR_NAME30, IDC_BTN_HTR_SP30, IDC_BTN_HTR_OPT30,
	IDC_BTN_HTR_NAME31, IDC_BTN_HTR_SP31, IDC_BTN_HTR_OPT31,
	IDC_BTN_HTR_NAME32, IDC_BTN_HTR_SP32, IDC_BTN_HTR_OPT32,
	IDC_BTN_HTR_NAME33, IDC_BTN_HTR_SP33, IDC_BTN_HTR_OPT33,
	IDC_BTN_HTR_NAME34, IDC_BTN_HTR_SP34, IDC_BTN_HTR_OPT34,
	IDC_BTN_HTR_NAME35, IDC_BTN_HTR_SP35, IDC_BTN_HTR_OPT35,
	IDC_BTN_HTR_NAME36, IDC_BTN_HTR_SP36, IDC_BTN_HTR_OPT36,
	IDC_BTN_HTR_NAME37, IDC_BTN_HTR_SP37, IDC_BTN_HTR_OPT37,
	IDC_BTN_HTR_NAME38, IDC_BTN_HTR_SP38, IDC_BTN_HTR_OPT38,
	IDC_BTN_HTR_NAME39, IDC_BTN_HTR_SP39, IDC_BTN_HTR_OPT39,
	IDC_BTN_HTR_NAME40, IDC_BTN_HTR_SP40, IDC_BTN_HTR_OPT40,
	IDC_BTN_HTR_NAME41, IDC_BTN_HTR_SP41, IDC_BTN_HTR_OPT41,
	IDC_BTN_HTR_NAME42, IDC_BTN_HTR_SP42, IDC_BTN_HTR_OPT42,
	IDC_BTN_HTR_NAME43, IDC_BTN_HTR_SP43, IDC_BTN_HTR_OPT43,
	IDC_BTN_HTR_NAME44, IDC_BTN_HTR_SP44, IDC_BTN_HTR_OPT44,
	IDC_BTN_HTR_NAME45, IDC_BTN_HTR_SP45, IDC_BTN_HTR_OPT45,
	IDC_BTN_HTR_NAME46, IDC_BTN_HTR_SP46, IDC_BTN_HTR_OPT46,
	IDC_BTN_HTR_NAME47, IDC_BTN_HTR_SP47, IDC_BTN_HTR_OPT47,
	IDC_BTN_HTR_NAME48, IDC_BTN_HTR_SP48, IDC_BTN_HTR_OPT48,
};

ITEM_STRUCT g_Heater =
{
	"Heater",	0, 1000,
	"NotSet|Set"
};

ITEM_STRUCT g_MfcTolStruct =
{
	"MFC Tolerance(Torr)", 1, 100,
	"NotUse|Use"
};

void Init_SubItem2Config()
{
}

void CheckSubItem2Enable(RecipeStepTemplateM* pRcpStep) 
{   
	int i;
	BOOL bSubItem2Enable = FALSE;
	pRcpStep->bSubItem2Enable = FALSE;

	for(i=0; i<MAX_HEATER_NO; i++)
	{
		if(pRcpStep->Htr_pnOption[i] != 0) { bSubItem2Enable = TRUE; break; }
	}

	pRcpStep->bSubItem2Enable = bSubItem2Enable;

}

void Display_SubItem2_Information( HWND hdlg )
{
	KWIN_Item_Int_Display( hdlg, IDC_PRCSTEP,  g_ProcessStep ); 
}

void DRAW_Heater_List( HWND hdlg )
{
	int i;
	for(i=0; i<MAX_HEATER_NO; i++)
	{
		if(g_bHeaterSetUse && g_HtrList[i].bUse)
		{
			KWIN_Item_String_Display( hdlg, g_pnHtrCtrl[i][0], g_HtrList[i].szHtrName );
			//Item_Draw_Int( hdlg, g_pnHtrCtrl[i][1], (int)pSubItem_D_RECIPE->Htr_pdbSetPoint[i]);
			KWIN_Item_Double_Display(hdlg, g_pnHtrCtrl[i][1], pSubItem_D_RECIPE->Htr_pdbSetPoint[i], 1);  //... 2016.02.29
			Item_Draw_Enum( hdlg, g_pnHtrCtrl[i][2], pSubItem_D_RECIPE->Htr_pnOption[i], g_Heater.szOptEnum);
		}
		else
		{
			KWIN_Item_Hide(hdlg, g_pnHtrCtrl[i][0]);
			KWIN_Item_Hide(hdlg, g_pnHtrCtrl[i][1]);
			KWIN_Item_Hide(hdlg, g_pnHtrCtrl[i][2]);
		}
	}
}



BOOL Item_Set_Heater(HWND hdlg, int nCtrl, int nIdx, int nType)
{
	switch(nType)
	{
	case 1: Item_HeaterSet_Point( hdlg, nCtrl, &pSubItem_D_RECIPE->Htr_pdbSetPoint[nIdx], &g_Heater ); //... 2016.02.29
			break;
	case 2: Item_Set_Option( hdlg, nCtrl, &pSubItem_D_RECIPE->Htr_pnOption[nIdx], &g_Heater );
			break;
	}
	return TRUE;
}

BOOL IsHeaterControl(int nCtrl, int *pnIdx, int *pnType)
{
	BOOL bRet = FALSE;
	int i;
	int nType = -1;
	for(i=0; i<MAX_HEATER_NO; i++)
	{
		if(nCtrl == g_pnHtrCtrl[i][1]) { nType = 1; bRet = TRUE; break; }
		if(nCtrl == g_pnHtrCtrl[i][2]) { nType = 2; bRet = TRUE; break; }
	}
	if(pnType != 0) *pnType = nType;
	if(pnIdx !=0) *pnIdx = i;
	return bRet;
}


BOOL APIENTRY SubItem2_Msg_Handler( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam )
{
	HDC		hDC;
	PAINTSTRUCT	ps;
//	int nIdx;
	int nIdx, nType;

	switch ( msg ) 
	{
		case WM_INITDIALOG :
			 Init_SubItem2Config();
			 KGUI_STANDARD_Set_Function_Size( hdlg );
//			 Print_All_Elements();
 			 Display_SubItem2_Information( hdlg );
			 DRAW_Heater_List( hdlg );

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
					EndDialog( hdlg, 1 );
					CheckSubItem2Enable(pSubItem_D_RECIPE);
					return TRUE;
				case IDC_CANCEL :
					EndDialog( hdlg, 0 );
					CheckSubItem2Enable(pSubItem_D_RECIPE);
					return FALSE;
				default:
					if(IsHeaterControl(wParam, &nIdx, &nType))
					{
						Item_Set_Heater(hdlg, wParam, nIdx, nType);
						return FALSE;
					}
			 }
			 return FALSE;
			 
	}
	return FALSE;
}
