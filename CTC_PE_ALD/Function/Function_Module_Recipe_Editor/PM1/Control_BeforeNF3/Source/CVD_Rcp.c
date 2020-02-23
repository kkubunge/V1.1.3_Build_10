///////////////////////////////////////////////////////////////////////////////////////////
// CVD_Rcp.c (CVD Type Recipe Edit Function)
// Create : 2005.05.09
// for CVD Editor

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
#include "Recipe_Code.h"
#include "interface.h"

#include "cimseqnc.h"
#include "File.h"
#include "resource.h"
#include "Read_Ini.h"

//-----------------------------------------------------------------------------------------

ITEM_BTN_STRUCT g_MfcBtnBDMY[MAX_DISPLAY_COUNT][MAX_MFC_DISPLAY] =
{
	{
		{ IDC_BUTTON_ITEMD1_OPT1,  15*1,     557, 54, 22 },
		{ IDC_BUTTON_ITEMD2_OPT1,  15*1+56,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD3_OPT1,  15+56*2,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD4_OPT1,  15+56*3,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD5_OPT1,  15+56*4,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD6_OPT1,  15+56*5,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD7_OPT1,  15+56*6,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD8_OPT1,  15+56*7,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD9_OPT1,  15+56*8,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD10_OPT1, 15+56*9,  557, 54, 22 },
		{ IDC_BUTTON_ITEMD11_OPT1, 15+56*10, 557, 54, 22 },
		{ IDC_BUTTON_ITEMD12_OPT1, 15+56*11, 557, 54, 22 },
		{ IDC_BUTTON_ITEMD13_OPT1, 15+56*12, 557, 54, 22 },
		{ IDC_BUTTON_ITEMD14_OPT1, 15+56*13, 557, 54, 22 },
		{ IDC_BUTTON_ITEMD15_OPT1, 15+56*14, 557, 54, 22 },
		{ IDC_BUTTON_ITEMD16_OPT1, 15+56*15, 557, 54, 22 }
	}
};

ITEM_BTN_STRUCT g_MfcBtnB[MAX_DISPLAY_COUNT][MAX_MFC_DISPLAY] =
{
	{
		{ IDC_BUTTON_ITEM1_OPT1, 284*2, 132+24*0, 60, 24 },
		{ IDC_BUTTON_ITEM2_OPT1, 282*2, 132+24*1, 60, 24 },
		{ IDC_BUTTON_ITEM3_OPT1, 282*2, 132+24*2, 60, 24 },
		{ IDC_BUTTON_ITEM4_OPT1, 282*2, 132+24*3, 60, 24 },
		{ IDC_BUTTON_ITEM5_OPT1, 282*2, 132+24*4, 60, 22 },
		{ IDC_BUTTON_ITEM6_OPT1, 282*2, 132+24*5, 60, 24 },
		{ IDC_BUTTON_ITEM7_OPT1, 282*2, 132+24*6, 60, 24 },
		{ IDC_BUTTON_ITEM8_OPT1, 282*2, 132+24*7, 60, 24 },
		{ IDC_BUTTON_ITEM9_OPT1, 282*2, 132+24*0, 60, 24 },
		{ IDC_BUTTON_ITEM10_OPT1, 282*2, 132+24*1, 60, 22 },
		{ IDC_BUTTON_ITEM11_OPT1, 282*2, 132+24*2, 60, 24 },
		{ IDC_BUTTON_ITEM12_OPT1, 282*2, 132+24*3, 60, 24 },
		{ IDC_BUTTON_ITEM13_OPT1, 282*2, 132+24*4, 60, 24 },
		{ IDC_BUTTON_ITEM14_OPT1, 282*2, 132+24*5, 60, 24 },
		{ IDC_BUTTON_ITEM15_OPT1, 282*2, 132+24*6, 60, 22 },
		{ IDC_BUTTON_ITEM16_OPT1, 282*2, 132+24*7, 60, 24 }
	},
	{
		{ IDC_BUTTON_ITEM1_OPT2, 416*2, 132+24*0, 60, 24 },
		{ IDC_BUTTON_ITEM2_OPT2, 350*2, 132+24*1, 60, 24 },
		{ IDC_BUTTON_ITEM3_OPT2, 350*2, 132+24*2, 60, 24 },
		{ IDC_BUTTON_ITEM4_OPT2, 350*2, 132+24*3, 60, 24 },
		{ IDC_BUTTON_ITEM5_OPT2, 350*2, 132+24*4, 60, 22 },
		{ IDC_BUTTON_ITEM6_OPT2, 350*2, 132+24*5, 60, 24 },
		{ IDC_BUTTON_ITEM7_OPT2, 350*2, 132+24*6, 60, 24 },
		{ IDC_BUTTON_ITEM8_OPT2, 350*2, 132+24*7, 60, 24 },
		{ IDC_BUTTON_ITEM9_OPT2, 413*2, 132+24*0, 60, 24 },
		{ IDC_BUTTON_ITEM10_OPT2, 413*2, 132+24*1, 60, 22 },
		{ IDC_BUTTON_ITEM11_OPT2, 413*2, 132+24*2, 60, 24 },
		{ IDC_BUTTON_ITEM12_OPT2, 413*2, 132+24*3, 60, 24 },
		{ IDC_BUTTON_ITEM13_OPT2, 413*2, 132+24*4, 60, 24 },
		{ IDC_BUTTON_ITEM14_OPT2, 413*2, 132+24*5, 60, 24 },
		{ IDC_BUTTON_ITEM15_OPT2, 413*2, 132+24*6, 60, 22 },
		{ IDC_BUTTON_ITEM16_OPT2, 413*2, 132+24*7, 60, 24 }
	}
};

//-----------------------------------------------------------------------------------------
extern VRCP_SEQ_STRUCT g_VrcpSeq[];
extern MFC_VLV_SEQ_STRUCT g_MfcVlv[];
extern	MFC_RCP_DATA	g_MFCData[20];
extern RecipeStepTemplate 	F_RECIPE0;
extern char	 g_szValveName[MAX_VAL_NO][MAX_VAL_NAME_LENGTH];

//-----------------------------------------------------------------------------------------
// 2005.05.21 by cskim, for CVD Editor
extern BOOL g_bDMY;

int GetMfcNoFromBtnID(int nID, int *pnPage)
{
	int nMfcNo = -1;
	int nPage = 0;
	int i, j;
	for(i=0; i<MAX_DISPLAY_COUNT; i++)
	{
		for(j=0; j<MAX_MFC_DISPLAY; j++)
		{
			if(g_MfcBtnB[i][j].nID == nID) { nMfcNo=j+1; nPage=i; break; }
		}

		if(nMfcNo != -1) break;
	}
	*pnPage = nPage;
	return nMfcNo;
}

int GetMfcNoFromBtnIDDMY(int nID, int *pnPage)
{
	int nMfcNo = -1;
	int nPage = 0;
	int i, j;
	for(i=0; i<MAX_DISPLAY_COUNT; i++)
	{
		for(j=0; j<MAX_MFC_DISPLAY; j++)
		{
			if(g_MfcBtnBDMY[0][j].nID == nID) { nMfcNo=j+1; nPage=i; break; }
		}

		if(nMfcNo != -1) break;
	}
	*pnPage = nPage;
	return nMfcNo;
}
