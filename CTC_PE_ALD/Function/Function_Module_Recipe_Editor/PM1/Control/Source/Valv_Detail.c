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

int		 g_dItemNo;
int		 g_dCurrentProcessStep	=	0;
int		 g_dCurrentValveStep	=	0;
int		 g_dCurrentTimeStep		=	0;

BOOL		g_bDetailEdited		=	FALSE;
BOOL		g_bDetailEditOK		=	FALSE;
// 2005.05.21 by cskim, for CVD Editor
BOOL		g_bCVDValveEditUpdate	=	FALSE;
int			g_bDetailEditMode		= 0;	// 0:ALD_VlvRecipe 1:CVD_OnlyDisplay 2:CVD_&_Edit

// 2005.09.22 by cskim, for 112 Valve Recipe
extern int g_nTotalValveLineStep;

VAL_DATA	g_Det_Valve_Data;
VAL_DATA	g_Det_Valve_Data_DMY;

HWND		g_hAdjustWND			=	NULL;
IMG_NODE*	pStartNode;

extern	 BOOL				g_bRF;
extern	 BOOL				g_bEditOk;				// Valve Ctrl Dialog에서 Define
extern	 BOOL				g_bGuiEdit;
extern	 UINT				g_dValveNo;
extern	 int				g_Total_Valve_No;
extern	 char				g_szValveName[MAX_VAL_NO][MAX_VAL_NAME_LENGTH];
extern	 char				g_szDLLPath[MAX_PATH];

extern	 HINSTANCE			g_hDllInstance;

extern	 VALVE_RCP_DATA		udValve_map;
extern	 VALVE_RCP_DATA		udValve_map_DMY;
extern	 BOOL				g_bDMY;

extern	double				g_dbValveTime_Min;
extern	double				g_dbValveTime_Max;
extern	int					g_nValveTime_DecPnt;
extern  int					g_RcpFileType;

//------------ 2005.12.13 by ycchoi ----------->>
extern	HWND	hWnd_ValveRecipe;
extern	HWND	hWnd_DetailValueRecipe;
//------------ 2005.12.13 by ycchoi -----------<<

void GUI_Read_GDF(void);
void GUI_Main_Lib_Load(void);
void GUI_LIB_All_Draw(HWND hdlg);
void Remove_All_Img_Node(void);
BOOL Get_Item_No(int dx, int dy, int* dValveNo);
HINSTANCE KGUI_s_Get_GLIB_BACKGROUND(void);
BOOL APIENTRY Adjust_Msg_Handler(HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam);
BOOL Can_Change_Status(int dTarget_Valve, VAL_STATUS* pValveStatus, int* dStatus);
BOOL APIENTRY Det_But_Proc(HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam);

WNDPROC	Default_Detail_Proc;

void Set_Detail_Valve_Edited(BOOL bEdited)
{
	g_bDetailEdited	=	bEdited;
}

BOOL Get_Detail_Valve_Edited()
{
	return g_bDetailEdited;
}


BOOL IsRF(HWND hdlg, int dCtrlNo)
{
	int		dScrollPos;
	int		dValNo		=	0;

	HWND	hScroll	=	NULL;

	hScroll	   = GetDlgItem(hdlg, IDC_SCROLLBAR3);
	dScrollPos = GetScrollPos(hScroll, SB_CTL);
	dValNo = dCtrlNo + dScrollPos + 1;

	if((g_bRF) && (g_Total_Valve_No == dValNo))	return TRUE;
	else	return FALSE;
}


// 2005.07.12 by cskim
BOOL IsRF_or_RPG(HWND hdlg, int dCtrlNo)
{
	char szTitle[10] = "";
	int j = dCtrlNo;
	BOOL bRF = FALSE;
	GetDlgItemText(hdlg, IDC_ITEM_V1 + j , szTitle, 9);

	if(!stricmp(szTitle, "RF") || !stricmp(szTitle, "RF1") || !stricmp(szTitle, "RF2") || !stricmp(szTitle, "RPG"))
	{
		bRF	= TRUE;
	}

	return bRF;
}

void Load_Bitmap_Library(HWND hdlg)
{
	g_hDllInstance	= LoadLibrary(g_szDLLPath);
}

void Draw_Step_Information(HWND hdlg)
{
	KWIN_Item_Int_Display(hdlg, IDC_PROCESS_STEP, g_dCurrentProcessStep);
	KWIN_Item_Int_Display(hdlg, IDC_VAL_STEP, g_dCurrentValveStep);
}

void Draw_Time_Data(HWND hdlg)
{
	double	dbTime	=	0;

	if(g_bDMY)
		dbTime = g_Det_Valve_Data_DMY.dbTime;
	else
		dbTime = g_Det_Valve_Data.dbTime;

	if(g_bDetailEditMode == 0)
		KWIN_Item_Int_Display(hdlg, IDC_COL_STEP1, g_dCurrentTimeStep + 1);
	KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME1, dbTime, g_nValveTime_DecPnt);
}

void Draw_Valve_Status(HWND hdlg, HWND hCtrl, VAL_STATUS uValve_Status, BOOL bRF)
{
	HDC		hDC;
	HBRUSH	hValBr, hOldBr;

	RECT	rc;

	if(bRF)
	{
		switch(uValve_Status)
		{
			case VAL_NOUSE:  hValBr = CreateSolidBrush(VAL_NOUSE_COLOR);	 break;
			case VAL_OPEN :	 hValBr = CreateSolidBrush(RF_ON_COLOR);		 break;
			case VAL_CLOSE:	 hValBr = CreateSolidBrush(RF_OFF_COLOR);		 break;
		}
	}
	else
	{
		switch(uValve_Status)
		{
			case VAL_NOUSE:  hValBr = CreateSolidBrush(VAL_NOUSE_COLOR);	 break;
			case VAL_OPEN :	 hValBr = CreateSolidBrush(VAL_ON_COLOR);		 break;
			case VAL_CLOSE:	 hValBr = CreateSolidBrush(VAL_OFF_COLOR);	 break;
		}
	}


	// Make Control Device Context
	hDC	=	GetDC(hCtrl);
	hOldBr = (HBRUSH) SelectObject(hDC, hValBr);
	GetClientRect(hCtrl, &rc);					//	Get Client Area

	rc.top +=2;
	rc.left +=2;
	rc.bottom-=2;
	rc.right -=2;

	FillRect(hDC, &rc, hValBr);
	SelectObject(hDC, hOldBr);
	DeleteObject(hValBr);
	ReleaseDC(hCtrl, hDC);
}

void Draw_Valve_Data(HWND hdlg)
{
	int		i;
	int		dStart	=	0;
	int		dCtrl;

	HWND	hCtrl	=	NULL;
	HWND	hScroll	=	NULL;


	VAL_STATUS	udValData;

	hScroll = GetDlgItem(hdlg, IDC_SCROLLBAR3);
	dStart = GetScrollPos(hScroll, SB_CTL);

	for (i = 0 ; i < VAL_H_MAX ; i++)///////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		if(g_bDMY)
			udValData = g_Det_Valve_Data_DMY.RCP_DATA[dStart + i];
		else
			udValData = g_Det_Valve_Data.RCP_DATA[dStart + i];

		dCtrl  = IDC_VAL_1_1 + i;
		hCtrl = GetDlgItem(hdlg, dCtrl);

		Draw_Valve_Status(hdlg, hCtrl, udValData, IsRF_or_RPG(hdlg, i));
	}

}

void Refresh_Valve_Detail_Data(HWND hdlg)
{
	Draw_Time_Data(hdlg);
	Draw_Valve_Data(hdlg);
}

void Draw_Border_Title(HWND hdlg)
{
	int		i;
	int		dStart;
	int		dCtrl;
	int		dBorderNo;
	char	szTitle[5] = "";

	HWND	hScroll	=	NULL;

	hScroll = GetDlgItem(hdlg, IDC_SCROLLBAR3);

	dStart = GetScrollPos(hScroll, SB_CTL);

	for (i = 0 ; i < VAL_H_MAX ; i++)
	{
		dCtrl		= IDC_ITEM_V1 + i ;
		dBorderNo	= dStart + i + 1;

		strcpy(szTitle, g_szValveName[dBorderNo - 1] );

		KWIN_Item_String_Display(hdlg, dCtrl, szTitle);
	}
}

void Control_Button_Initialize(HWND hdlg)
{
	// 2005.05.24 by cskim, for CVD Editor
	if(g_bDetailEditMode == 0)
	{
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 800 , 190 , 81 , 73 , hdlg , (HMENU) IDB_ENABLE_ARROW_UP	, GETHINST(hdlg) , NULL);
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 800 , 190 , 81 , 73 , hdlg , (HMENU) IDB_PRESS_ARROW_UP		, GETHINST(hdlg) , NULL);

		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 800 , 280 , 81 , 73 , hdlg , (HMENU) IDB_ENABLE_ARROW_DOWN	, GETHINST(hdlg) , NULL);
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 800 , 280 , 81 , 73 , hdlg , (HMENU) IDB_PRESS_ARROW_DOWN	, GETHINST(hdlg) , NULL);

		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 530 , 87 , 38 , hdlg , (HMENU) IDB_VAL_CANCEL_ENABLE , GETHINST(hdlg) , NULL);
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 530 , 87 , 38 , hdlg , (HMENU) IDB_VAL_CANCEL_DISABLE , GETHINST(hdlg) , NULL);
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 530 , 87 , 38 , hdlg , (HMENU) IDB_VAL_CANCEL_PRESSED , GETHINST(hdlg) , NULL);

		 KWIN_Item_Hide(hdlg, IDB_VAL_CANCEL_DISABLE);
		 KWIN_Item_Hide(hdlg, IDB_VAL_CANCEL_PRESSED);

		 KWIN_Item_Hide(hdlg, IDB_PRESS_ARROW_UP);
		 KWIN_Item_Hide(hdlg, IDB_PRESS_ARROW_DOWN);
	}

	if(g_bDetailEditMode == 0 || g_bDetailEditMode == 2)
	{
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 436 , 87 , 38 , hdlg , (HMENU) IDB_EDIT_ENABLE , GETHINST(hdlg) , NULL);
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 436 , 87 , 38 , hdlg , (HMENU) IDB_EDIT_DISABLE , GETHINST(hdlg) , NULL);
		 CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 436 , 87 , 38 , hdlg , (HMENU) IDB_EDIT_PRESSED , GETHINST(hdlg) , NULL);
		 KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
		 KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);
	}

	CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 488 , 87 , 38 , hdlg , (HMENU) IDB_VAL_OK_ENABLE , GETHINST(hdlg) , NULL);
	CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 488 , 87 , 38 , hdlg , (HMENU) IDB_VAL_OK_DISABLE , GETHINST(hdlg) , NULL);
	CreateWindowEx(0L , "button"    , "" , WS_CHILD | WS_VISIBLE | BS_OWNERDRAW , 790 , 488 , 87 , 38 , hdlg , (HMENU) IDB_VAL_OK_PRESSED , GETHINST(hdlg) , NULL);

	KWIN_Item_Hide(hdlg, IDB_VAL_OK_DISABLE);
	KWIN_Item_Hide(hdlg, IDB_VAL_OK_PRESSED);

	if(g_bDetailEditMode == 1 || g_bDetailEditMode == 2)
	{
		KWIN_Item_Hide(hdlg, IDC_BUTTON1);
		KWIN_Item_Hide(hdlg, IDC_COL_STEP1);
	}
	g_bCVDValveEditUpdate = FALSE;
}

void OnArrowUp(HWND hdlg)
{
	if(Get_Detail_Valve_Edited())
	{
		if(MessageBox(hdlg, "Valve Step Edited! Save it?", "Save?", MB_OKCANCEL) == IDOK)
		{
			if(g_bDMY)
				memcpy (&udValve_map_DMY.RCP_DATA[g_dCurrentValveStep-1][g_dCurrentTimeStep], &g_Det_Valve_Data_DMY, sizeof(VAL_DATA));
			else
				memcpy (&udValve_map.RCP_DATA[g_dCurrentValveStep-1][g_dCurrentTimeStep], &g_Det_Valve_Data, sizeof(VAL_DATA));
		}
	}
	
	KWIN_Item_Enable(hdlg, IDB_PRESS_ARROW_UP);
	KWIN_Item_Disable(hdlg, IDB_ENABLE_ARROW_UP);
	KWIN_Item_Hide(hdlg, IDB_ENABLE_ARROW_UP);

	if(g_dCurrentTimeStep > 0)	 		g_dCurrentTimeStep--;
	else
	{
		KWIN_Item_Disable(hdlg, IDB_PRESS_ARROW_UP);
		KWIN_Item_Enable(hdlg, IDB_ENABLE_ARROW_UP);
		KWIN_Item_Hide(hdlg, IDB_PRESS_ARROW_UP);
 		return;
	}

	Set_Detail_Valve_Edited(FALSE);
	if(g_bDMY)
		memcpy (&g_Det_Valve_Data_DMY, &udValve_map_DMY.RCP_DATA[g_dCurrentValveStep - 1][g_dCurrentTimeStep], sizeof(VAL_DATA));
	else
		memcpy (&g_Det_Valve_Data, &udValve_map.RCP_DATA[g_dCurrentValveStep - 1][g_dCurrentTimeStep], sizeof(VAL_DATA));
	Refresh_Valve_Detail_Data(hdlg);

	KWIN_Item_Enable(hdlg, IDB_ENABLE_ARROW_UP);
	KWIN_Item_Disable(hdlg, IDB_PRESS_ARROW_UP);
	KWIN_Item_Hide(hdlg, IDB_PRESS_ARROW_UP);
}

void OnArrowDown(HWND hdlg)
{
	if(Get_Detail_Valve_Edited())
	{
		if(MessageBox(hdlg, "Valve Step Edited! Save it?", "Save?", MB_OKCANCEL) == IDOK)
		{
			if(g_bDMY)
				memcpy (&udValve_map_DMY.RCP_DATA[g_dCurrentValveStep-1][g_dCurrentTimeStep], &g_Det_Valve_Data_DMY, sizeof(VAL_DATA));
			else
				memcpy (&udValve_map.RCP_DATA[g_dCurrentValveStep-1][g_dCurrentTimeStep], &g_Det_Valve_Data, sizeof(VAL_DATA));
		}
	}


	KWIN_Item_Enable(hdlg, IDB_PRESS_ARROW_DOWN);
	KWIN_Item_Hide(hdlg, IDB_ENABLE_ARROW_DOWN);

	if(g_dCurrentTimeStep < (g_nTotalValveLineStep - 1))	 g_dCurrentTimeStep++;
	else
	{
		KWIN_Item_Enable(hdlg, IDB_ENABLE_ARROW_DOWN);
		KWIN_Item_Hide(hdlg, IDB_PRESS_ARROW_DOWN);
		return;
	}

	Set_Detail_Valve_Edited(FALSE);
	if(g_bDMY)
		memcpy (&g_Det_Valve_Data_DMY, &udValve_map_DMY.RCP_DATA[g_dCurrentValveStep-1][g_dCurrentTimeStep], sizeof(VAL_DATA));
	else
		memcpy (&g_Det_Valve_Data, &udValve_map.RCP_DATA[g_dCurrentValveStep-1][g_dCurrentTimeStep], sizeof(VAL_DATA));
	Refresh_Valve_Detail_Data(hdlg);


	KWIN_Item_Enable(hdlg, IDB_ENABLE_ARROW_DOWN);
	KWIN_Item_Hide(hdlg, IDB_PRESS_ARROW_DOWN);
}

void OnTimeSet(HWND hdlg)
{
	double	dbRes;

	char	szTitle[20];
	char 	buffer[128] = {0,};
	/*
	if(g_bDMY)
		dbRes = g_Det_Valve_Data_DMY.dbTime;
	else
		dbRes = g_Det_Valve_Data.dbTime;
	*/

	//mgsong Modify for time interlock
	if(g_bDMY) return;


	if(! g_bDetailEditOK) return;
	sprintf(szTitle, "Time - %d", g_dCurrentTimeStep + 1);

	if(MODAL_ANALOG_BOX1(hdlg, "Select", szTitle, g_dbValveTime_Min,
		g_dbValveTime_Max, g_nValveTime_DecPnt, &dbRes))
	{
		//if(g_bDMY)
		//	g_Det_Valve_Data_DMY.dbTime = dbRes;
		//else

		sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%f] to [%f]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, "Valve Time", g_Det_Valve_Data.dbTime, dbRes);
		// SampleLogSave(buffer, "Item");

		g_Det_Valve_Data.dbTime = dbRes;
		KWIN_Item_Double_Display(hdlg, IDC_ITEM_TIME1, dbRes, g_nValveTime_DecPnt);
	}
}



void Set_Valve_Data(HWND hdlg, WPARAM wParam)
{
	BOOL	bRF			=	FALSE;

	int		dCtrlNo		=	(int) wParam;
	int		dValveNo	=	0;
	int		dScrollPos	=	0;
	int		dInterlock_Valve = 0;
	int		dResult		=	0;

	char	szMessage[50];
	char 	buffer[128] = {0,};

	HWND	hCtrl	=	NULL;
	HWND	hScroll	=	NULL;

	VAL_STATUS	uValve_Status;

	if(!g_bDetailEditOK)		return;								//	Edit Button 선택시에만 Edit 가능

	if((dCtrlNo < IDC_VAL_1_1) && (dCtrlNo > IDC_VAL_1_21))		return;

	hCtrl	= GetDlgItem(hdlg, dCtrlNo);
	hScroll = GetDlgItem(hdlg, IDC_SCROLLBAR3);

	dScrollPos = GetScrollPos(hScroll, SB_CTL);
	dValveNo   = dScrollPos + (dCtrlNo - IDC_VAL_1_1 /* Start Valve */);

	if((dValveNo > MAX_VAL_NO) || (dValveNo < 0))	return  ;

	if(g_bDMY)
	{
		if(Can_Change_Status(dValveNo, g_Det_Valve_Data_DMY.RCP_DATA, &dInterlock_Valve))
		{
			uValve_Status = g_Det_Valve_Data_DMY.RCP_DATA[dValveNo];

			switch(uValve_Status)
			{
				case VAL_OPEN :
					uValve_Status = VAL_CLOSE;
					sprintf(buffer, "\t[Dummy Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo], "Open", "Close");
					break;
				case VAL_CLOSE :
					uValve_Status = VAL_OPEN;
					sprintf(buffer, "\t[Dummy Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo], "Close", "Open");
					break;
			}

			g_Det_Valve_Data_DMY.RCP_DATA[dValveNo] = uValve_Status;
			Set_Detail_Valve_Edited(TRUE);
			Draw_Valve_Data(hdlg);
			GUI_LIB_All_Draw(hdlg);
			// SampleLogSave(buffer, "Item");
		}
		else
		{
			InvalidateRect(hdlg, NULL, TRUE);
			sprintf(szMessage, "Interlocked [Valve %s  &  Valve %s]", g_szValveName[dValveNo], g_szValveName[dInterlock_Valve - 1]);
			MODAL_MESSAGE_BOX(hdlg, "Valve InterLock ", szMessage, "Close", 0, &dResult);
		}
		//bRF = IsRF(hdlg, (dCtrlNo - IDC_VAL_1_1));
		//Draw_Valve_Status(hdlg, hCtrl, uValve_Status, bRF);
	}
	else
	{
		if(Can_Change_Status(dValveNo, g_Det_Valve_Data.RCP_DATA, &dInterlock_Valve))
		{
			uValve_Status = g_Det_Valve_Data.RCP_DATA[dValveNo];

			switch(uValve_Status)
			{
				case VAL_OPEN :
					uValve_Status = VAL_CLOSE;
					sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo], "Open", "Close");
					break;
				case VAL_CLOSE :
					uValve_Status = VAL_OPEN;
					sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo], "Close", "Open");
					break;
			}

			g_Det_Valve_Data.RCP_DATA[dValveNo] = uValve_Status;
			Set_Detail_Valve_Edited(TRUE);
			Draw_Valve_Data(hdlg);
			GUI_LIB_All_Draw(hdlg);
			// SampleLogSave(buffer, "Item");
		}
		else
		{
			InvalidateRect(hdlg, NULL, TRUE);
			sprintf(szMessage, "Interlocked [Valve %s  &  Valve %s]", g_szValveName[dValveNo], g_szValveName[dInterlock_Valve - 1]);
			MODAL_MESSAGE_BOX(hdlg, "Valve InterLock ", szMessage, "Close", 0, &dResult);
		}
	}

}

void OnValDetOK(HWND hdlg)
{

	int		dIndex = g_dCurrentTimeStep;

	if(g_bDetailEditMode != 1)
	{
		if(IDNO == MessageBox(hdlg,"Are you sure to Set Valve Status!!", "Confirm", MB_YESNO | MB_ICONQUESTION)) return;
	}

	if(g_bDMY)
		memcpy (&udValve_map_DMY.RCP_DATA[g_dCurrentValveStep-1][dIndex], &g_Det_Valve_Data_DMY, sizeof(VAL_DATA));
	else
		memcpy (&udValve_map.RCP_DATA[g_dCurrentValveStep-1][dIndex], &g_Det_Valve_Data, sizeof(VAL_DATA));
	if(g_hDllInstance != NULL)		FreeLibrary(g_hDllInstance);
	Remove_All_Img_Node();

	//------ 2005.12.13 by ycchoi --------->>
	hWnd_DetailValueRecipe = NULL;
#ifdef _DEBUG
	_IO_CIM_PRINTF("Valve Detail Window Close With OK[hWnd_DetailValueRecipe is NULL]\n");
#endif
	//------ 2005.12.13 by ycchoi ---------<<

	EndDialog(hdlg , 1);
}

void OnValDetCancel(HWND hdlg)
{
	// 2004-04-29  IPS 서병찬 CJ 요구사항
	// Cancel Confirm Box

	if(IDNO == MessageBox(hdlg , "Really Cancel edited data and reload last saved data ???" , "CANCEL Check" , MB_ICONQUESTION | MB_YESNO)) return;
	if(g_hDllInstance != NULL)		FreeLibrary(g_hDllInstance);
	Remove_All_Img_Node();

	//------ 2005.12.13 by ycchoi --------->>
	hWnd_DetailValueRecipe = NULL;
#ifdef _DEBUG
	_IO_CIM_PRINTF("Valve Detail Window Close With CANCEL[hWnd_DetailValueRecipe is NULL]\n");
#endif
	//------ 2005.12.13 by ycchoi ---------<<

	EndDialog(hdlg , 0);
}

void OnDetailEditEnable(HWND hdlg)
{
	// 2004-05-11  IPS 한석원 요구사항
	// Append Edit Button

	 g_bDetailEditOK	=	TRUE;

	 KWIN_Item_Hide(hdlg, IDB_EDIT_ENABLE);
	 KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
	 KWIN_Item_Show(hdlg, IDB_EDIT_PRESSED);
}

void OnDetailEditDisable(HWND hdlg)
{
	// 2004-05-11  IPS 한석원 요구사항
	// Append Edit Button

	 g_bDetailEditOK	=	FALSE;

	 KWIN_Item_Show(hdlg, IDB_EDIT_ENABLE);
	 KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
	 KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);
}

void Set_Value_Status(HWND hdlg, int dValveNo)
{
	int		dInterlock_Valve;
	int		dResult;
	char	szMessage[50];
	char 	buffer[128] = {0,};

	if((dValveNo > MAX_VAL_NO) || (dValveNo < 0))	return  ;

	if(g_bDMY)
	{
		if(Can_Change_Status(dValveNo - 1, g_Det_Valve_Data_DMY.RCP_DATA, &dInterlock_Valve))
		{
			if(g_Det_Valve_Data_DMY.RCP_DATA[dValveNo - 1] == VAL_OPEN)
			{
				g_Det_Valve_Data_DMY.RCP_DATA[dValveNo - 1] = VAL_CLOSE;
				sprintf(buffer, "\t[Dummy Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo-1], "Open", "Close");
			}
			else if(g_Det_Valve_Data_DMY.RCP_DATA[dValveNo - 1] == VAL_CLOSE)
			{
				g_Det_Valve_Data_DMY.RCP_DATA[dValveNo - 1] = VAL_OPEN;
				sprintf(buffer, "\t[Dummy Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo-1], "Close", "Open");
			}

			Set_Detail_Valve_Edited(TRUE);
			// SampleLogSave(buffer, "Item");
		}
		else
		{
			InvalidateRect(hdlg, NULL, TRUE);
			sprintf(szMessage, "Interlocked [Valve %s  &  Valve %s]", g_szValveName[dValveNo -1], g_szValveName[dInterlock_Valve - 1]);
			MODAL_MESSAGE_BOX(hdlg, "Valve InterLock", szMessage, "Close", 0, &dResult);
		}
	}
	else
	{
		if(Can_Change_Status(dValveNo - 1, g_Det_Valve_Data.RCP_DATA, &dInterlock_Valve))
		{
			if(g_Det_Valve_Data.RCP_DATA[dValveNo - 1] == VAL_OPEN)
			{
				g_Det_Valve_Data.RCP_DATA[dValveNo - 1] = VAL_CLOSE;
				sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo-1], "Open", "Close");
			}
			else if(g_Det_Valve_Data.RCP_DATA[dValveNo - 1] == VAL_CLOSE)
			{
				g_Det_Valve_Data.RCP_DATA[dValveNo - 1] = VAL_OPEN;
				sprintf(buffer, "\t[Valve Data] ProcessStep:[%d] LineStep:[%d] %s - From [%s] to [%s]\n", g_dCurrentProcessStep, g_dCurrentTimeStep+1, g_szValveName[dValveNo-1], "Close", "Open");
			}

			Set_Detail_Valve_Edited(TRUE);
			// SampleLogSave(buffer, "Item");
		}
		else
		{
			InvalidateRect(hdlg, NULL, TRUE);
			sprintf(szMessage, "Interlocked [Valve %s  &  Valve %s]", g_szValveName[dValveNo -1], g_szValveName[dInterlock_Valve - 1]);
			MODAL_MESSAGE_BOX(hdlg, "Valve InterLock", szMessage, "Close", 0, &dResult);
		}
	}
}

BOOL APIENTRY Det_But_Proc(HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam)
{
	HWND		hParent	=	NULL;

    hParent = GetParent(hdlg);

	switch(msg)
	{
		case WM_SETFOCUS :
			 InvalidateRect(hParent, NULL, TRUE);
			 return 0;

		case WM_KILLFOCUS :
			 InvalidateRect(hParent, NULL, TRUE);
			 return 0;
	}

	return CallWindowProc(Default_Detail_Proc, hdlg, msg, wParam, lParam);
}

void Make_SubClassing_Detail(HWND hdlg)
{
	int		i;
	HWND	hBut;


	for (i = 0 ; i < VAL_H_MAX ; i++)
	{
		hBut = GetDlgItem(hdlg, IDC_ITEM_V1 + i);
		Default_Detail_Proc = (WNDPROC) SetWindowLong(hBut, GWL_WNDPROC, (LONG) Det_But_Proc);
		hBut = GetDlgItem(hdlg, IDC_VAL_1_1 + i);
		Default_Detail_Proc = (WNDPROC) SetWindowLong(hBut, GWL_WNDPROC, (LONG) Det_But_Proc);
	}

}

void Init_Edit_Button_Status(HWND hdlg)
{
	g_bDetailEditOK = g_bEditOk;

	 if(g_bDetailEditOK)
	 {
		 KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
		 KWIN_Item_Show(hdlg, IDB_EDIT_PRESSED);
		 KWIN_Item_Hide(hdlg, IDB_EDIT_ENABLE);
		 g_bCVDValveEditUpdate = TRUE;
	 }
	 else
	 {
		 KWIN_Item_Hide(hdlg, IDB_EDIT_DISABLE);
		 KWIN_Item_Hide(hdlg, IDB_EDIT_PRESSED);
		 KWIN_Item_Show(hdlg, IDB_EDIT_ENABLE);
	 }

}

BOOL APIENTRY Valve_Detail_Msg_Handler(HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam)
{
	int				dScrRange;
	int				dhPos;
	int				dPosX, dPosY;
	int				dValveNo;

	RECT			rc;

	HDC				hDC;
	HDC				hMemDC		=	NULL;
	HBRUSH			hMyBrush, hOldBrush;
	PAINTSTRUCT		ps;

	HWND			hScroll		=	NULL;

	HINSTANCE		hInst		=	NULL;


	hScroll = GetDlgItem(hdlg, IDC_SCROLLBAR3);

	switch (msg)
	{
		case WM_CREATE :
			 return 0;

		case WM_INITDIALOG :

			//------ 2005.12.13 by ycchoi --------->>
			hWnd_DetailValueRecipe = hdlg;
#ifdef _DEBUG
			_IO_CIM_PRINTF("Valve Detail Recipe Window Handle [%d]\n", hWnd_DetailValueRecipe);
#endif
			//------ 2005.12.13 by ycchoi ---------<<

			 KGUI_STANDARD_Set_Function_Size(hdlg);
			 Make_SubClassing_Detail(hdlg);
			 GUI_Read_GDF();
			 GUI_Main_Lib_Load();
			 Control_Button_Initialize(hdlg);
			 SetScrollRange(hScroll, SB_CTL, 0, g_Total_Valve_No - VAL_H_MAX, TRUE);
			 Set_Detail_Valve_Edited(FALSE);
			 Load_Bitmap_Library(hdlg);

			 Draw_Step_Information(hdlg);
			 Draw_Border_Title(hdlg);
			 Refresh_Valve_Detail_Data(hdlg);
			 GUI_LIB_All_Draw(hdlg);
			 Init_Edit_Button_Status(hdlg);
			 return 0;

		case WM_RBUTTONDOWN :
			 if(g_bGuiEdit)
			 {
				 dPosX = GET_X_LPARAM(lParam);
				 dPosY = GET_Y_LPARAM(lParam);
				 if(Get_Item_No(dPosX, dPosY, &dValveNo))	g_dValveNo = (UINT) dValveNo;

				DialogBox(GETHINST(hdlg), MAKEINTRESOURCE(IDD_ADJUST_IMG), hdlg, Adjust_Msg_Handler);
				InvalidateRect(hdlg, NULL, TRUE);
			 }
			 return 0;

		case WM_LBUTTONDOWN :
			 // Edit Button 선택시에만 Editing 가능
			 if(g_bDetailEditOK)
			 {
				 dPosX = GET_X_LPARAM(lParam);
				 dPosY = GET_Y_LPARAM(lParam);

				 if(Get_Item_No(dPosX, dPosY, &dValveNo))
				 {
					 Set_Value_Status(hdlg, dValveNo);
					 Refresh_Valve_Detail_Data(hdlg);
					 GUI_LIB_All_Draw(hdlg);
					 return 0;
				 }
			 }
			 return 0;

		case WM_DRAWITEM :
			 Draw_Valve_Data(hdlg);
			 KWIN_DRAW_BUTTON_ITEM(GetModuleHandle(NULL) , (LPDRAWITEMSTRUCT) lParam);
			 return 0;

		case WM_PAINT :
			 hDC = BeginPaint(hdlg, &ps);
			 hMyBrush	= CreateSolidBrush(RGB(206,207,206));
			 hOldBrush	= (HBRUSH)SelectObject(hDC, hMyBrush);
			 GetClientRect(hdlg, &rc);
			 FillRect(hDC, &rc, hMyBrush);
			 KWIN_DRAW_BACK_PAINT(KGUI_s_Get_GLIB_BACKGROUND() , ps , "IDB_VAL_DET_BACK");

			 // 2004.09.09 by cskim
			 DeleteObject(hMyBrush);

			 EndPaint(hdlg , &ps);
			 Draw_Step_Information(hdlg);
			 Draw_Border_Title(hdlg);
			 Refresh_Valve_Detail_Data(hdlg);
			 GUI_LIB_All_Draw(hdlg);
			 return 0;

		case WM_COMMAND :
 			switch(LOWORD(wParam))
			{
				case IDB_ENABLE_ARROW_UP	:	OnArrowUp(hdlg);				return 0;
				case IDB_ENABLE_ARROW_DOWN	:	OnArrowDown(hdlg);			return 0;
				case IDB_EDIT_ENABLE		:	OnDetailEditEnable(hdlg);		return 0;
				case IDB_EDIT_PRESSED		:	OnDetailEditDisable(hdlg);	return 0;
				case IDC_ITEM_TIME1			:	OnTimeSet(hdlg);				return 0;
				case IDB_VAL_OK_ENABLE		:	OnValDetOK(hdlg);				return 0;
				case IDB_VAL_CANCEL_ENABLE	:   OnValDetCancel(hdlg);			return 0;
				default						:	Set_Valve_Data(hdlg, wParam);
												InvalidateRect(hdlg, NULL, TRUE);
												return 0;
			}
			 return 0;

		case WM_HSCROLL :
			dScrRange	= g_Total_Valve_No - VAL_H_MAX;
			dhPos		= GetScrollPos(hScroll, SB_CTL);
			SetScrollRange(hScroll, SB_CTL, 0, dScrRange , TRUE);

			switch(LOWORD(wParam))
			{
				case SB_LINELEFT :
								dhPos = max(0, dhPos - 1);
								break;

				case SB_LINERIGHT :
								dhPos = min(dScrRange, dhPos + 1);
								break;

				case SB_PAGELEFT :
								dhPos = max(0, dhPos - 10);
								break;

				case SB_PAGERIGHT :
								dhPos = min(dScrRange, dhPos + 10);
								break;

				case SB_THUMBTRACK :
								dhPos = HIWORD(wParam);
								break;

			}

			SetScrollPos(hScroll, SB_CTL, dhPos, TRUE);
			Draw_Border_Title(hdlg);
			Draw_Valve_Data(hdlg);
			return 0;

	}

	return 0;
}
