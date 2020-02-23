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
#include <cimseqnc.h>

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
#include "CVD_Rcp.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// ycchoi(2006.02.03)
// EDIT Button 추가(NRD요청사항)
BOOL gbEditMode = FALSE;
BOOL CONFIRM_OK = FALSE;
// 2004.09.09 by cskim
char g_szModule[10] = "";
// ycchoi(2006.02.03)
// Proecess Recipe의 Save As시 Group을 선택하여 저장할수 있는 기능(NRD요청사항)
BOOL APIENTRY Recipe_Group_Select_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY View_Open_Recipe_Log_Proc(HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam);
#include <direct.h>
char gszTgtGroup[MAX_PATH] = "";
char g_szRecipeName[MAX_PATH] = "";
char g_szPassword[256] = "";

//---- 2005.11.08 by ycchoi ------>>
#include "GroupAuthority.h"
//---- 2005.11.08 by ycchoi ------<<

//------------2005.11.04 by ycchoi ---------------------->>
char GroupFolder[MAX_GROUP_COUNT][MAX_PATH] = {""};
int gGroupFolderCount = 0;
BOOL gbUseGroupAuthority = FALSE;
//------------2005.11.04 by ycchoi ----------------------<<

//------------2005.11.18 by ycchoi ---------------------->>
HWND gMainHWnd = NULL;
//------------2005.11.18 by ycchoi ----------------------<<

//----- 2005.12.13 by ycchoi ----------->>
HWND hWnd_ValveRecipe = NULL;
HWND hWnd_DetailValueRecipe = NULL;
HWND HWnd_SubItemRecipe = NULL;
//----- 2005.12.13 by ycchoi -----------<<

RCP_TYPE g_RcpFileType = RCP_NONE;
HINSTANCE g_hInstance = NULL;
int g_X = 0;
int g_Y = 0;

extern int F_STEP_COUNT;
extern RecipeStepTemplate D_RECIPE0;
extern RecipeStepTemplateM D_RECIPE[MAX_STEP_COUNT];
extern RecipeStepTemplateM F_RECIPE[MAX_STEP_COUNT];
extern int g_MFCNo;
extern MFC_RCP_DATA g_MFCData[MFC_BUF_LENGTH];
extern BOOL D_HAS_STEP[MAX_DISPLAY_COUNT];
extern BOOL g_bNEW_FLAG;

void Draw_MFC_Contents(HWND hdlg);
void FILE_HANDLING_BUTTON_MAPPING(HWND hdlg, int id, int Enable);
BOOL Set_Default_Group_Item(HWND hdlg);
BOOL ITEM_BUTTON_RECIPE_TYPE(HWND hWnd);

//---> ADD 2012/08/24 Encryption by KSM
BOOL g_bTEST_GROUP_CHK = FALSE;
//---> END 2012/08/24 Encryption by KSM

// 2013.04.02 Recipe Password
extern BOOL g_bReciepPW_CHK;
extern BOOL g_bRecipePWNull;
extern PWEncrypt(char *szPassword);
char g_szCheatKey[65] = "56689E75A58E177EB3D39D7FF3AE1862";
HWND G_hdlg;
// 2013.04.02 Recipe Password

//--------------------------------------------------------------------------------
// FA Start
//--------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	////////////////// EVENT REPORT IO
	{"e:SCHEDULER", _K_S_IO, SCHEDULER, 0},

	//---> ADD 2012/08/24 Encryption by KSM
	{"CurrentUserName", _K_S_IO, CurrentUserName, 0},
	{"CurrentGroupName", _K_S_IO, CurrentGroupName, 0},
	{"SCREEN_USER_CONTROL_CHK", _K_F_IO, SCREEN_USER_CONTROL, 0},
	{"UserLevelControl", _K_D_IO, LOGIN_USER_LEVEL, 0},
	//---> END 2012/08/24 Encryption by KSM

	//////////////////
	""};

//--------------------------------------------------------------------------------
BOOL GROUP_RUN_CHECK = FALSE;
//--------------------------------------------------------------------------------
void Set_File_Filter_Name(HWND hdlg, RCP_TYPE udRecipeType, char *szExtension)
{
	if (udRecipeType == -1)
	{
		udRecipeType = SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
	}

	switch (udRecipeType)
	{
	case RCP_PRC:
		strcpy(szExtension, "*.rcp");
		return;
	case RCP_PRE:
		strcpy(szExtension, "*.pre");
		return;
	case RCP_PST:
		strcpy(szExtension, "*.pst");
		return;
	case RCP_IDL:
		strcpy(szExtension, "*.idl");
		return;
	case RCP_CLN:
		strcpy(szExtension, "*.cln");
		return;
	case RCP_PUG:
		strcpy(szExtension, "*.pug");
		return;
	case RCP_LPR:
		strcpy(szExtension, "*.lpr");
		return;
	case RCP_LPS:
		strcpy(szExtension, "*.lps");
		return;
	default:
		strcpy(szExtension, "*.*");
		return;
	}
}

BOOL DirData_ListView_Init(HWND hWnd)
{
	char szData[MAX_PATH];
	char Buffer[256];

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	BOOL bRet = TRUE;

	sprintf(Buffer, "%s/*.*", RECIPE_CONTROL_Get_MAIN_DIRECTORY());
	ListBox_ResetContent(hWnd);

	//---- 2005.11.04 by ycchoi -------------->>
	// 해당 계정의 Group Folder 정보 로드
	gGroupFolderCount = LoadGroupFolderInfo();
	//---- 2005.11.04 by ycchoi --------------<<

	hFind = FindFirstFile(Buffer, &fd);
	while ((hFind != INVALID_HANDLE_VALUE) && bRet)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			if ((strcmp(fd.cFileName, ".") != 0) &&
				(strcmp(fd.cFileName, "..") != 0))
			{
				//---------- 2005.11.08 by ycchoi ----------------------->>
				if (gbUseGroupAuthority && !authorityDir(fd.cFileName))
				{
					bRet = FindNextFile(hFind, &fd);
					continue;
				}
				//---------- 2005.11.08 by ycchoi -----------------------<<

				ListBox_AddString(hWnd, fd.cFileName);
			}
		}
		bRet = FindNextFile(hFind, &fd);
	}
	FindClose(hFind);
	ListBox_SetCurSel(hWnd, -1);

	strcpy(szData, RECIPE_CONTROL_Get_GROUP_DIRECTORY());

	if ((strcmp(szData, ".") == 0) || (strlen(szData) == 0))
	{
		//---> DEL 2012/08/24 Encryption by KSM
		//Set_Default_Group_Item( GetParent( hWnd ) );
		//---> END 2012/08/24 Encryption by KSM

		//---> ADD 2012/08/24 Encryption by KSM
		{
			int CommStatus;

			if (READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) >= USER_ACCESS_LEVEL) //--> Possible Level 8,9
			{
				Set_Default_Group_Item(GetParent(hWnd));
			}
		}
		//---> END 2012/08/24 Encryption by KSM
	}
	return TRUE;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
BOOL FileData_ListView_Init(HWND hWnd, char *pszFilter)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	BOOL bRet = TRUE;
	char Buffer[256];

	sprintf(Buffer, "%s/%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), RECIPE_CONTROL_Get_GROUP_DIRECTORY(), RECIPE_CONTROL_Get_FILE_DIRECTORY(), pszFilter);
	ListBox_ResetContent(hWnd);
	hFind = FindFirstFile(Buffer, &fd);
	while ((hFind != INVALID_HANDLE_VALUE) && bRet)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			ListBox_AddString(hWnd, fd.cFileName);
		}
		bRet = FindNextFile(hFind, &fd);
	}
	FindClose(hFind);
	ListBox_SetCurSel(hWnd, -1);
	return TRUE;
}
//--------------------------------------------------------------------------------
BOOL APIENTRY View_Open_Recipe_Log_Proc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

	int i;
	static char FileName[256];
	static char FileName_org[256];
	char Buffer[256];
	BOOL bres;
	FILE *fpt;
	//	ViewStepTemplate *readdata;

	sprintf(FileName, "system/%s_RECIPE_EDIT.cfg", g_szModule);

	switch (msg)
	{
	case WM_INITDIALOG:
		MoveCenterWindow(hdlg);
		CONFIRM_OK = FALSE;
		//		readdata = (ViewStepTemplate *) (LPARAM) lParam;
		//>>-----------------------------------------------------------
		KWIN_Item_Show(hdlg, IDD_RECIPE_EDIT_POPUP);
		//-----------------------------------------------------------------
		if ((fpt = fopen(FileName, "r")) == NULL)
		{ // 2005.03.22
			sprintf(Buffer, "[%s] File Not Found", FileName);
			MessageBox(hdlg, Buffer, "Open Error", MB_ICONHAND | MB_OK);
			EndDialog(hdlg, TRUE);
			return TRUE;
		}

		i = 0;
		do
		{
			bres = Read_Line_From_File(fpt, Buffer);

			SendDlgItemMessage(hdlg, IDC_LIST_RECIPE_CHANGE, LB_ADDSTRING, 0, (LPARAM)Buffer);
			i++;

		} while (bres);
		fclose(fpt);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			CONFIRM_OK = TRUE;
			EndDialog(hdlg, 0);
			return TRUE;

		case IDCANCEL:
			CONFIRM_OK = FALSE;
			EndDialog(hdlg, 0);
			return FALSE;
		}
		break;
	}
	return FALSE;
}
//--------------------------------------------------------------------------------
BOOL Set_Default_Group_Item(HWND hdlg)
{
	char szFileFilter[50];
	char szGroup[250] = "";
	HWND hList = NULL;

	hList = GetDlgItem(hdlg, IDC_LIST_RECIPEDIR);

	ListBox_GetText(hList, 0, szGroup);

	if (!strlen(szGroup))
	{
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 0);
		return FALSE;
	}

	RECIPE_CONTROL_Set_GROUP_DIRECTORY(szGroup);
	RECIPE_CONTROL_Set_FILE_CHANGE(FALSE);
	RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, "");
	//
	Set_File_Filter_Name(hdlg, -1, szFileFilter);

	FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFileFilter);
	SendMessage(hList, LB_SETCURSEL, 0, 0); // 2003. 5. 12 - IPS 한석원 요구 사항
	//
	RECIPE_CONTROL_ALL_DATA_DISPLAY(hdlg);
	RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, TRUE);

	return TRUE;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void Screen_Nav_Info(int no)
{
	if (no == -1)
	{
		KGUI_STANDARD_Window_Kill_Signal();
		KWIN_DIALOG_DRIVER_UNLOADING();
	}
	KGUI_STANDARD_Screen_Nav_Info(no);
}
//--------------------------------------------------------------------------------
void FILE_HANDLING_BUTTON_MAKE(HWND hdlg)
{
#define BUTTON_X_START 16
#define BUTTON_X_INTERVAL 67
#define BUTTON_Y_START 440
#define BUTTON_Y_INTERVAL 67

#define BUTTON_X_INS_START 250
#define BUTTON_Y_INS_START 530

#define BUTTON_X_INS2_START 340
#define BUTTON_Y_INS2_START 530

#define BUTTON_X_REMOVE_START 430
#define BUTTON_Y_REMOVE_START 530

#define BUTTON_X_COPY_START 520
#define BUTTON_Y_COPY_START 530

#define BUTTON_X_LEFT_START 610
#define BUTTON_Y_LEFT_START 530

#define BUTTON_X_RIGHT_START 774
#define BUTTON_Y_RIGHT_START 530

	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (0 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_NEW_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (0 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_NEW_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (0 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_NEW_PRESSED, GETHINST(hdlg), NULL);
	//

	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (1 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_SAVE_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (1 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_SAVE_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (1 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_SAVE_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (2 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_SAVEAS_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (2 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_SAVEAS_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (2 * BUTTON_X_INTERVAL) + BUTTON_X_START, (0 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_SAVEAS_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (0 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_CANCEL_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (0 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_CANCEL_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (0 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_CANCEL_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (1 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_FILE_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (1 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_FILE_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (1 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_FILE_PRESSED, GETHINST(hdlg), NULL);
	// ycchoi(2006.02.03)
	// EDIT Button 추가(NRD요청사항)
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (2 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_FEDIT_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (2 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_FEDIT_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, (2 * BUTTON_X_INTERVAL) + BUTTON_X_START, (1 * BUTTON_Y_INTERVAL) + BUTTON_Y_START, 58, 58, hdlg, (HMENU)IDB_FEDIT_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS_START, BUTTON_Y_INS_START, 87, 38, hdlg, (HMENU)IDB_INSERT_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS_START, BUTTON_Y_INS_START, 87, 38, hdlg, (HMENU)IDB_INSERT_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS_START, BUTTON_Y_INS_START, 87, 38, hdlg, (HMENU)IDB_INSERT_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS2_START, BUTTON_Y_INS2_START, 87, 38, hdlg, (HMENU)IDB_INSERT2_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS2_START, BUTTON_Y_INS2_START, 87, 38, hdlg, (HMENU)IDB_INSERT2_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_INS2_START, BUTTON_Y_INS2_START, 87, 38, hdlg, (HMENU)IDB_INSERT2_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_REMOVE_START, BUTTON_Y_REMOVE_START, 87, 38, hdlg, (HMENU)IDB_REMOVE_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_REMOVE_START, BUTTON_Y_REMOVE_START, 87, 38, hdlg, (HMENU)IDB_REMOVE_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_REMOVE_START, BUTTON_Y_REMOVE_START, 87, 38, hdlg, (HMENU)IDB_REMOVE_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_COPY_START, BUTTON_Y_COPY_START, 87, 38, hdlg, (HMENU)IDB_COPY_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_COPY_START, BUTTON_Y_COPY_START, 87, 38, hdlg, (HMENU)IDB_COPY_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_COPY_START, BUTTON_Y_COPY_START, 87, 38, hdlg, (HMENU)IDB_COPY_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_LEFT_START, BUTTON_Y_LEFT_START, 83, 38, hdlg, (HMENU)IDB_LEFT_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_LEFT_START, BUTTON_Y_LEFT_START, 83, 38, hdlg, (HMENU)IDB_LEFT_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_LEFT_START, BUTTON_Y_LEFT_START, 83, 38, hdlg, (HMENU)IDB_LEFT_PRESSED, GETHINST(hdlg), NULL);
	//
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_RIGHT_START, BUTTON_Y_RIGHT_START, 83, 38, hdlg, (HMENU)IDB_RIGHT_ENABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_RIGHT_START, BUTTON_Y_RIGHT_START, 83, 38, hdlg, (HMENU)IDB_RIGHT_DISABLE, GETHINST(hdlg), NULL);
	CreateWindowEx(0L, "button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, BUTTON_X_RIGHT_START, BUTTON_Y_RIGHT_START, 83, 38, hdlg, (HMENU)IDB_RIGHT_PRESSED, GETHINST(hdlg), NULL);
	//
}
//
void FILE_HANDLING_BUTTON_MAPPING(HWND hdlg, int id, int Enable)
{
	long index1, index2, index3;
	switch (id)
	{
	case BUTTON_NEW:
		index1 = IDB_NEW_ENABLE;
		index2 = IDB_NEW_DISABLE;
		index3 = IDB_NEW_PRESSED;
		break;
	case BUTTON_SAVE:
		index1 = IDB_SAVE_ENABLE;
		index2 = IDB_SAVE_DISABLE;
		index3 = IDB_SAVE_PRESSED;
		break;
	case BUTTON_SAVEAS:
		index1 = IDB_SAVEAS_ENABLE;
		index2 = IDB_SAVEAS_DISABLE;
		index3 = IDB_SAVEAS_PRESSED;
		break;
	case BUTTON_CANCEL:
		index1 = IDB_CANCEL_ENABLE;
		index2 = IDB_CANCEL_DISABLE;
		index3 = IDB_CANCEL_PRESSED;
		break;
	case BUTTON_FILE:
		index1 = IDB_FILE_ENABLE;
		index2 = IDB_FILE_DISABLE;
		index3 = IDB_FILE_PRESSED;
		break;
	case BUTTON_EXPORT:
		index1 = IDB_EXPORT_ENABLE;
		index2 = IDB_EXPORT_DISABLE;
		index3 = IDB_EXPORT_PRESSED;
		break;
	// ycchoi(2006.02.03)
	// EDIT Button 추가(NRD요청사항)
	case BUTTON_EDIT_ENABLE:
		index1 = IDB_FEDIT_ENABLE;
		index2 = IDB_FEDIT_DISABLE;
		index3 = IDB_FEDIT_PRESSED;
		break;
	case BUTTON_EDIT_PRESSED:
		index1 = IDB_FEDIT_PRESSED;
		index2 = IDB_FEDIT_DISABLE;
		index3 = IDB_FEDIT_ENABLE;
		break;
	}
	if (Enable == 1)
	{
		KWIN_Item_Enable(hdlg, index1);
		KWIN_Item_Hide(hdlg, index2);
		KWIN_Item_Hide(hdlg, index3);
	}
	else if (Enable == 0)
	{
		KWIN_Item_Disable(hdlg, index2);
		KWIN_Item_Hide(hdlg, index1);
		KWIN_Item_Hide(hdlg, index3);
	}
	else if (Enable == 2)
	{
		KWIN_Item_Disable(hdlg, index3);
		KWIN_Item_Hide(hdlg, index1);
		KWIN_Item_Hide(hdlg, index2);
	}
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void FileButton_Fresh_View(HWND hdlg)
{
	RECIPE_CONTROL_Set_FILE_CHANGE(FALSE);
	//
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 0);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 1);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 0);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 1);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EXPORT, 1);
	//
	RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, TRUE);
	RECIPE_CONTROL_ALL_DATA_DISPLAY(hdlg);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#define TextWrite(DC, START, Y, STR) TextOut(##DC##, 238, START + (Y * 24), ##STR##, strlen(##STR##))
#define TextWrite2(DC, START, Y, STR) TextOut(##DC##, 305, START + (Y * 25), ##STR##, strlen(##STR##))
#define TextWrite3(DC, START, Y, STR) TextOut(##DC##, 248, START + (Y * 25), ##STR##, strlen(##STR##))
#define TextWrite4(DC, START, Y, STR) TextOut(##DC##, 305, START + (Y * 24), ##STR##, strlen(##STR##))
#define TextWrite5(DC, START, X, STR) TextOut(##DC##, START + (X * 55), 420, ##STR##, strlen(##STR##))

#define TextWrite_X(DC, X, START, Y, STR) TextOut(##DC##, X, START + (Y * 29), ##STR##, strlen(##STR##))

#define TextWriteMFCSide1(DC, START, Y, STR) TextOut(##DC##, 236, START + (Y * 24), ##STR##, strlen(##STR##))
#define TextWriteMFCSide2_Rev(DC, START, Y, STR) TextOut(##DC##, 308, START - (Y * 24), ##STR##, strlen(##STR##))

void Title_Draw(HDC *phDC)
{
	HFONT hfont, holdfont;

	hfont = CreateFont(15, 0, 0, 0, /*FW_BOLD*/ 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
	holdfont = (HFONT)SelectObject(*phDC, hfont);
	SetBkMode(*phDC, TRANSPARENT);
	SetTextColor(*phDC, 0);

	//---------------------------------------------------------------------------
	// 2006.01.25
	SetTextColor(*phDC, 0);
	SelectObject(*phDC, holdfont);
	DeleteObject(hfont);

	hfont = CreateFont(16, 0, 0, 0, /*FW_BOLD*/ 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
	holdfont = (HFONT)SelectObject(*phDC, hfont);
	SetTextColor(*phDC, RGB(255, 255, 255));
	//	TextWrite_X( *phDC, 270, 114, 4, "M     F     C");
	SelectObject(*phDC, holdfont);
	DeleteObject(hfont);
}

void MFC_SCROLL_Ctrl_Initialize(HWND hdlg)
{
	int dScrollMax;
	HWND hScroll;

	dScrollMax = g_MFCNo - 5;
	hScroll = GetDlgItem(hdlg, IDC_SCROLLBAR1);

	ShowWindow(hScroll, SW_HIDE);

	/*
	if ( dScrollMax < 0 )
	{
		ShowWindow( hScroll, SW_HIDE );
		EnableWindow( hScroll, FALSE );
	}
	else
	{
		ShowWindow( hScroll, SW_SHOW );
		EnableWindow( hScroll, TRUE );

		SetScrollRange( hScroll, SB_CTL, 0, dScrollMax , TRUE );
	}
*/
}

//////////////////////////////////////////////////////////////////////////
void Draw_MFC_Title(HWND hdlg)
{
#define ALL_MFC_NUMBER 9
#define SIDE1_MFC_NUM 3
#define SIDE2_MFC_NUM 6

	int i;
	int dVal;
	int nReversIdx = 0;
	int nSide2Idx = 0;

	HWND hScroll;

	HDC hDC;
	HFONT hfont, holdfont;
	HBRUSH hBrush, hOldBrush;
	char szTitle[ALL_MFC_NUMBER][50];

	hScroll = GetDlgItem(hdlg, IDC_SCROLLBAR1);
	dVal = GetScrollPos(hScroll, SB_CTL);

	for (i = 0; i < ALL_MFC_NUMBER; i++)
	{
		sprintf(szTitle[i], "%-15s", g_MFCData[dVal + i].szMFCDisplayName);
	}

	hDC = GetDC(hdlg);

	hfont = CreateFont(15, 0, 0, 0, /*FW_BOLD*/ 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
	hBrush = CreateSolidBrush(RGB(206, 207, 206));

	holdfont = (HFONT)SelectObject(hDC, hfont);
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	//	SetBkMode( hDC, OPAQUE );				//	Scroll이 없을 때
	SetBkMode(hDC, TRANSPARENT); //	Scroll이 없을 때

	SetBkColor(hDC, RGB(206, 207, 206));
	SetTextColor(hDC, 0);

	// 	// 2006.01.25
	// 	for(i = 0; i < 16; i++)
	// 	{
	// 		if ( strlen( szTitle[i] ) ) TextWrite4( hDC, 135, i, szTitle[i] );
	// 	}

	//... 2014.10.14 Fixed MFC Display NAME for MAHA-DPT
	for (i = 0; i < SIDE1_MFC_NUM; i++)
	{
		if (strlen(szTitle[i]))
			TextWriteMFCSide1(hDC, 162, i, szTitle[i]);
	}

	for (i = 0; i < SIDE2_MFC_NUM; i++)
	{
		nSide2Idx = i + SIDE1_MFC_NUM;
		if (strlen(szTitle[nSide2Idx]))
			TextWriteMFCSide2_Rev(hDC, 282, i, szTitle[nSide2Idx]);
	}

	SelectObject(hDC, holdfont);
	SelectObject(hDC, hOldBrush);

	DeleteObject(hBrush);
	DeleteObject(hfont);

	ReleaseDC(hdlg, hDC);
}

void Draw_MFCDMY_Title(HWND hdlg)
{
#define MFC_NUMBER 16
	int i;
	int dVal;

	HWND hScroll;

	HDC hDC;
	HFONT hfont, holdfont;
	HBRUSH hBrush, hOldBrush;
	char szTitle[MFC_NUMBER][50];

	hScroll = GetDlgItem(hdlg, IDC_SCROLLBAR1);
	dVal = GetScrollPos(hScroll, SB_CTL);

	for (i = 0; i < MFC_NUMBER; i++)
	{
		sprintf(szTitle[i], "%-15s", g_MFCData[dVal + i].szMFCDisplayName);
	}

	hDC = GetDC(hdlg);

	hfont = CreateFont(15, 0, 0, 0, /*FW_BOLD*/ 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
	hBrush = CreateSolidBrush(RGB(206, 207, 206));

	holdfont = (HFONT)SelectObject(hDC, hfont);
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	//	SetBkMode( hDC, OPAQUE );				//	Scroll이 없을 때
	SetBkMode(hDC, TRANSPARENT); //	Scroll이 없을 때

	SetBkColor(hDC, RGB(206, 207, 206));
	SetTextColor(hDC, 0);

	// 2006.01.25
	for (i = 0; i < 16; i++)
	{
		if ((i >= 5 && i <= 7) && (strlen(szTitle[i])))
			TextWrite5(hDC, 30, i, szTitle[i]);
		else if ((i == 8) && (strlen(szTitle[i])))
			TextWrite5(hDC, 23, i, szTitle[i]);
		else if ((i == 10) && (strlen(szTitle[i])))
			TextWrite5(hDC, 34, i, szTitle[i]);
		else if ((i >= 12 && i <= 15) && (strlen(szTitle[i])))
			TextWrite5(hDC, 36, i, szTitle[i]);
		else if (strlen(szTitle[i]))
			TextWrite5(hDC, 27, i, szTitle[i]);
	}

	SelectObject(hDC, holdfont);
	SelectObject(hDC, hOldBrush);

	DeleteObject(hBrush);
	DeleteObject(hfont);

	ReleaseDC(hdlg, hDC);
}

void Add_Recipe_Type_To_ComboBox(HWND hdlg)
{
	int i;

	char szRecipeType[NO_OF_RECIPE_TYPE][50] = {"All (*.*)",
												"Process (*.rcp)",
												"Pre Process (*.pre)",
												"Post Process (*.pst)",
												"Idle (*.idl)",
												"Clean (*.cln)",
												"Purge (*.pug)",
												"Lot Pre Recipe (*.lpr)",
												"Lot Post Recipe (*.lps)"};

	for (i = 0; i < NO_OF_RECIPE_TYPE; i++)
		SendDlgItemMessage(hdlg, IDC_COMBO1, CB_ADDSTRING, i, (LPARAM)szRecipeType[i]);
}

//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//  2013.04.02 Recipe Password
BOOL APIENTRY Recipe_Password_Msg_Handler_Change(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	BOOL bResult;
	static char Ret_Name[65];
	static char Ret_Pass[65];
	static char Ret_Pass2[65];
	static char Ret_Pass3[65];
	static char Ret_Buffer[65];
	static char Ret_Buffer2[65];
	static char Ret_Buffer3[65];
	char Ret_PassTemp[65];
	char MessageBuffer[256];
	char MessageBuffer2[256];
	char MessageBuffer3[256];

	switch (msg)
	{
	case WM_INITDIALOG:
		MoveCenterWindow(hdlg);
		KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_OLD_INPUT, "");
		KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT1, "");
		KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT2, "");

		return TRUE;

	case WM_PAINT:

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RECIPEPW_OLD_INPUT:
			sprintf(MessageBuffer, "OLD RECIPE PASSWORD");
			strcpy(Ret_Pass, "");
			strcpy(Ret_Buffer, "");
			if (MODAL_STRING_BOX2(hdlg, MessageBuffer, "", Ret_Buffer))
			{
				strcpy(Ret_Pass, Ret_Buffer);
				j = strlen(Ret_Pass);
				if (j == 0)
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_OLD_INPUT, "");
				else
				{
					for (i = 0; (i < j && i < 64); i++)
						Ret_Buffer[i] = '*';
					Ret_Buffer[i] = 0;
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_OLD_INPUT, Ret_Buffer);
				}
			}

			if (strlen(Ret_Pass) > 16)
			{
				strcpy(Ret_Buffer, "");
				strcpy(Ret_Pass, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_OLD_INPUT, Ret_Buffer);
				sprintf(MessageBuffer, "The length of password Is more than 16 characters..\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDC_RECIPEPW_NEW_INPUT1:
			sprintf(MessageBuffer2, "NEW RECIPE PASSWORD 1");
			strcpy(Ret_Pass2, "");
			strcpy(Ret_Buffer2, "");
			if (MODAL_STRING_BOX2(hdlg, MessageBuffer2, "", Ret_Buffer2))
			{
				strcpy(Ret_Pass2, Ret_Buffer2);
				j = strlen(Ret_Pass2);
				if (j == 0)
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT1, "");
				else
				{
					for (i = 0; (i < j && i < 64); i++)
						Ret_Buffer2[i] = '*';
					Ret_Buffer2[i] = 0;
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT1, Ret_Buffer2);
				}
			}

			if (strlen(Ret_Pass2) > 16)
			{
				strcpy(Ret_Buffer2, "");
				strcpy(Ret_Pass2, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT1, Ret_Buffer2);
				sprintf(MessageBuffer, "The length of password Is more than 16 characters..\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDC_RECIPEPW_NEW_INPUT2:
			sprintf(MessageBuffer3, "NEW RECIPE PASSWORD 2");
			strcpy(Ret_Pass3, "");
			strcpy(Ret_Buffer3, "");
			if (MODAL_STRING_BOX2(hdlg, MessageBuffer3, "", Ret_Buffer3))
			{
				strcpy(Ret_Pass3, Ret_Buffer3);
				j = strlen(Ret_Pass3);
				if (j == 0)
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT2, "");
				else
				{
					for (i = 0; (i < j && i < 64); i++)
						Ret_Buffer3[i] = '*';
					Ret_Buffer3[i] = 0;
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT2, Ret_Buffer3);
				}
			}

			if (strlen(Ret_Pass3) > 16)
			{
				strcpy(Ret_Buffer3, "");
				strcpy(Ret_Pass3, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT2, Ret_Buffer3);
				sprintf(MessageBuffer, "The length of password Is more than 16 characters..\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDOK:

			strcpy(Ret_PassTemp, Ret_Pass);
			if (strcmp(Ret_Pass, "") != 0)
				PWEncrypt(Ret_Pass);

			if (strcmp(Ret_Pass, D_RECIPE0.szRecipePW) == 0 || (strcmp(Ret_Pass, g_szCheatKey) == 0))
			{
				if (strcmp(Ret_Pass2, Ret_Pass3) == 0)
				{
					bResult = TRUE;
				}
				else
				{
					strcpy(Ret_Buffer2, "");
					strcpy(Ret_Pass2, "");
					strcpy(Ret_Buffer3, "");
					strcpy(Ret_Pass3, "");
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT1, Ret_Buffer3);
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_NEW_INPUT2, Ret_Buffer3);
					strcpy(Ret_Pass, Ret_PassTemp);
					sprintf(MessageBuffer, "New Password 1 & 2 is not Correct\n\n            Try Again....");
					MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
				}
			}
			else
			{
				strcpy(Ret_Buffer, "");
				strcpy(Ret_Pass, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_OLD_INPUT, Ret_Buffer);
				sprintf(MessageBuffer, "OLD Password is not Correct\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}

			if (bResult == TRUE)
			{
				strcpy(g_szPassword, Ret_Pass2);
				g_bReciepPW_CHK = TRUE;
				EndDialog(hdlg, 1);
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hdlg, 0);
			return FALSE;
		default:
			break;
		}
		break;
	}
	return FALSE;
}

BOOL APIENTRY Recipe_Password_Msg_Handler_New(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	static char Ret_Name[65];
	static char Ret_Pass[65];
	static char Ret_Pass2[65];
	static char Ret_Buffer[65];
	static char Ret_Buffer2[65];
	char MessageBuffer[256];
	char MessageBuffer2[256];

	switch (msg)
	{
	case WM_INITDIALOG:
		MoveCenterWindow(hdlg);
		KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT1, "");
		KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT2, "");

		return TRUE;

	case WM_PAINT:

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RECIPEPW_FIRST_INPUT1:
			sprintf(MessageBuffer, "RECIPE PASSWORD 1");
			strcpy(Ret_Pass, "");
			strcpy(Ret_Buffer, "");
			if (MODAL_STRING_BOX2(hdlg, MessageBuffer, "", Ret_Buffer))
			{
				strcpy(Ret_Pass, Ret_Buffer);
				j = strlen(Ret_Pass);
				if (j == 0)
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT1, "");
				else
				{
					for (i = 0; (i < j && i < 64); i++)
						Ret_Buffer[i] = '*';
					Ret_Buffer[i] = 0;
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT1, Ret_Buffer);
				}
			}

			if (strlen(Ret_Pass) > 16)
			{
				strcpy(Ret_Buffer, "");
				strcpy(Ret_Pass, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT1, Ret_Buffer);
				sprintf(MessageBuffer, "The length of password Is more than 16 characters..\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDC_RECIPEPW_FIRST_INPUT2:
			sprintf(MessageBuffer2, "RECIPE PASSWORD 2");
			strcpy(Ret_Pass2, "");
			strcpy(Ret_Buffer2, "");
			if (MODAL_STRING_BOX2(hdlg, MessageBuffer2, "", Ret_Buffer2))
			{
				strcpy(Ret_Pass2, Ret_Buffer2);
				j = strlen(Ret_Pass2);
				if (j == 0)
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT2, "");
				else
				{
					for (i = 0; (i < j && i < 64); i++)
						Ret_Buffer2[i] = '*';
					Ret_Buffer2[i] = 0;
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT2, Ret_Buffer2);
				}
			}

			if (strlen(Ret_Pass2) > 16)
			{
				strcpy(Ret_Buffer2, "");
				strcpy(Ret_Pass2, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_FIRST_INPUT2, Ret_Buffer2);
				sprintf(MessageBuffer, "The length of password Is more than 16 characters..\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDOK:

			if (strcmp(Ret_Pass, Ret_Pass2) == 0)
			{
				strcpy(g_szPassword, Ret_Pass);
				g_bReciepPW_CHK = TRUE;
				EndDialog(hdlg, 1);
			}
			else
			{
				sprintf(MessageBuffer, "Recipe Password 1 & 2 is not Correct\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hdlg, 0);
			return FALSE;
		default:
			break;
		}
		break;
	}
	return FALSE;
}

BOOL APIENTRY Recipe_Password_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	int CommStatus;
	static char Ret_Name[65];
	static char Ret_Pass[65];
	static char Ret_Pass2[65];
	static char Ret_Buffer[65];
	static char Ret_Buffer2[65];
	char MessageBuffer[256];
	char Buffer[MAX_PATH];
	char Buffer3[MAX_PATH];
	char szFilterName[30];

	switch (msg)
	{
	case WM_INITDIALOG:
		MoveCenterWindow(hdlg);
		KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_INPUT, "");
		strcpy(Ret_Pass, "");
		return TRUE;

	case WM_PAINT:

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RECIPEPW_INPUT:
			sprintf(MessageBuffer, "RECIPE PASSWORD");
			strcpy(Ret_Buffer, "");
			if (MODAL_STRING_BOX2(hdlg, MessageBuffer, "", Ret_Buffer))
			{
				strcpy(Ret_Pass, Ret_Buffer);
				j = strlen(Ret_Pass);
				if (j == 0)
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_INPUT, "");
				else
				{
					for (i = 0; (i < j && i < 64); i++)
						Ret_Buffer[i] = '*';
					Ret_Buffer[i] = 0;
					KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_INPUT, Ret_Buffer);
				}
			}

			if (strlen(Ret_Pass) > 16)
			{
				strcpy(Ret_Buffer, "");
				strcpy(Ret_Pass, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_INPUT, Ret_Buffer);
				sprintf(MessageBuffer, "The length of password Is more than 16 characters..\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDOK:

			if (strcmp(Ret_Pass, "") != 0)
				PWEncrypt(Ret_Pass);

			if (strcmp(Ret_Pass, D_RECIPE0.szRecipePW) == 0 || (strcmp(Ret_Pass, g_szCheatKey) == 0))
			{
				g_bReciepPW_CHK = TRUE;
				EndDialog(hdlg, 1);
			}
			else
			{
				strcpy(Ret_Buffer, "");
				strcpy(Ret_Pass, "");
				KWIN_Item_String_Display(hdlg, IDC_RECIPEPW_INPUT, Ret_Buffer);
				sprintf(MessageBuffer, "Recipe Password is not Correct\n\n            Try Again....");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDCHANGE:

			if (g_bReciepPW_CHK == FALSE)
			{
				sprintf(MessageBuffer, "Available After Recipe Password Log In..");
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
				return TRUE;
			}

			if (GoModalDialogBoxParam(GETHINST(hdlg), MAKEINTRESOURCE(IDD_RECIPEPW_CHANGE), hdlg, Recipe_Password_Msg_Handler_Change, (LPARAM)NULL))
			{
				//RECIPE_CONTROL_Get_Selected_FILENAME( hdlg , Buffer , 128 );

				strcpy(D_RECIPE0.szRecipePW, g_szPassword);

				if (MessageBox(hdlg, "Are you sure to Save Recipe Data!!", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO)
					return TRUE;
				FILE_HANDLING_BUTTON_MAPPING(G_hdlg, BUTTON_SAVE, 2);

				if (!RECIPE_CONTROL_FILE_SAVE_PROC(G_hdlg, g_szRecipeName, 0))
				{
					FILE_HANDLING_BUTTON_MAPPING(G_hdlg, BUTTON_SAVE, 1);
					return FALSE;
				}
				// FA Start
				////////////////// UPDATE EVENT
				if (STRCMP(RECIPE_CONTROL_Get_GROUP_DIRECTORY(), "."))
				{ // No Group
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s|%d|%d", g_szModule, Buffer /*FileName*/, Kor_StepCount(), 1 /*Update*/);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				else
				{ // Use Group
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s:(%s)|%d|%d", g_szModule, Buffer /*FileName*/, RECIPE_CONTROL_Get_GROUP_DIRECTORY(), Kor_StepCount(), 1 /*Update*/);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				//////////////////

				FILE_HANDLING_BUTTON_MAPPING(G_hdlg, BUTTON_SAVE, 2);
				FileButton_Fresh_View(G_hdlg);
				Set_File_Filter_Name(G_hdlg, g_RcpFileType, szFilterName);
				SendDlgItemMessage(G_hdlg, IDC_COMBO1, CB_SETCURSEL, g_RcpFileType, 0);
				FileData_ListView_Init(GetDlgItem(G_hdlg, IDC_LIST_RECIPEFILE), szFilterName);
				gbEditMode = FALSE;
				FILE_HANDLING_BUTTON_MAPPING(G_hdlg, BUTTON_EDIT_ENABLE, 1);
				EndDialog(hdlg, 0);
				return TRUE;
			}
			else
			{
				return TRUE;
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hdlg, 0);
			return FALSE;
		default:
			break;
		}
		break;
	}
	return FALSE;
}
//  2013.04.02 Recipe Password
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------

//---> ADD 2012/08/24 Encryption by KSM
BOOL APIENTRY Console_Gui_For_LogIn_Proc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i, j, CommStatus, Result;
	static int id;
	PAINTSTRUCT ps;
	HDC hDC;
	static char Ret_Name[65];
	static char Ret_Pass[65];
	static char Ret_Buffer[65];
	char EnumBuffer[1025];
	char MessageBuffer[256];
		
	switch (msg)
	{
	case WM_INITDIALOG:
		MoveCenterWindow(hdlg);
		id = (int)lParam;
		READ_STRING(CurrentUserName, Ret_Name, &CommStatus);
		KWIN_Item_String_Display(hdlg, IDC_LOG_USER, Ret_Name);
		KWIN_Item_String_Display(hdlg, IDC_LOG_PASSWORD, "");
		//Password Variable INIT
		sprintf(Ret_Pass, "");
		return TRUE;

	case WM_PAINT:
		hDC = BeginPaint(hdlg, &ps);
		KWIN_DRAW_BACK_PAINT(GetModuleHandle(NULL), ps, MAKEINTRESOURCE(IDB_BACKLOGIN));
		EndPaint(hdlg, &ps);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_LOG_USER:
			return TRUE;

		case IDC_LOG_PASSWORD:
			sprintf(MessageBuffer, "PASSWORD for [%s]", Ret_Name);
			strcpy(Ret_Buffer, "");
			if (MODAL_STRING_BOX2(hdlg, MessageBuffer, "", Ret_Buffer))
			{
				strcpy(Ret_Pass, Ret_Buffer);
				j = strlen(Ret_Pass);
				if (j == 0)
					KWIN_Item_String_Display(hdlg, IDC_LOG_PASSWORD, "");
				else
				{
					for (i = 0; (i < j && i < 64); i++)
						Ret_Buffer[i] = '*';
					Ret_Buffer[i] = 0;
					KWIN_Item_String_Display(hdlg, IDC_LOG_PASSWORD, Ret_Buffer);
				}
			}
			return TRUE;

		case IDOK:
			sprintf(EnumBuffer, "CHECK %s|%s|0", Ret_Name, Ret_Pass); // 0은 PW확인. 1은 Login되어 있는 User의 Level과 비교
			printf("USER PW CHECK :[%s]\n", EnumBuffer);
			Result = RUN_FUNCTION_FREE(SCREEN_USER_CONTROL, EnumBuffer);
			//
			if (Result == SYS_SUCCESS)
			{
				EndDialog(hdlg, 1);
			}
			else
			{
				sprintf(MessageBuffer, "User / Password Information is not Correct for [%s]\n\n                     Try Again....", Ret_Name);
				MessageBox(hdlg, MessageBuffer, "Error", MB_ICONHAND | MB_OK);
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hdlg, 0);
			return FALSE;
		default:
			break;
		}
		break;
	}
	return FALSE;
}
//---> END 2012/08/24 Encryption by KSM

BOOL APIENTRY Console_Gui_For_Main_Proc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// FA Start
	int i, CommStatus, EditStatus;
	int x, y;
	int dvPos;
	int dComboIndex = 0;
	int dTempRecipeType = 0;

	char Buffer[MAX_PATH] = "";
	char Buffer2[MAX_PATH] = "";
	char Buffer3[MAX_PATH] = "";
	char szFilterName[30] = "";
	char m_szLog[128] = "";

	WPARAM Msg2;

	BOOL Change, Redraw;
	BOOL bExcel = FALSE;

	HDC hDC;
	HWND hVScroll;
	PAINTSTRUCT ps;
	WIN32_FIND_DATA fd;
	HANDLE hFind;

	HWND hWnd_s;
	// FA End

	// ycchoi(2006.02.03)
	// EDIT Button 추가(NRD요청사항)
	int mboxResult;

	//  2013.04.17 Recipe Password Added
	G_hdlg = hdlg;
	//  2013.04.17 Recipe Password Added

	switch (msg)
	{
	case WM_USER:
		switch (wParam)
		{
		case WM_USER:
			if (GROUP_RUN_CHECK)
			{
				//------------2005.11.18 by ycchoi ---------------------->>
				InitGui();
				//						DirData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_RECIPEDIR ) );
				//------------2005.11.18 by ycchoi ----------------------<<
			}
			return TRUE;

		case WM_PAINT:
			hDC = GetDC(hdlg);
			//
			ReleaseDC(hdlg, hDC);
			return TRUE;

		case WM_CLOSE:
			//<<-----------------------------------------------------------
			//>>-----------------------------------------------------------
			return TRUE;

		case WM_ACTIVATE:
			//>>-----------------------------------------------------------
			//<<-----------------------------------------------------------
			return TRUE;
		}
		break;

	case WM_INITDIALOG:

		//------------2005.11.18 by ycchoi ---------------------->>
		gMainHWnd = hdlg;
		//------------2005.11.18 by ycchoi ----------------------<<

		SetWindowText(hdlg, "GUI_RecipeEditor");
		//<<-----------------------------------------------------------
		KGUI_STANDARD_Set_Function_Size(hdlg);
		//>>-----------------------------------------------------------
		FILE_HANDLING_BUTTON_MAKE(hdlg);
		//
		FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), "*.*");

		if (GROUP_RUN_CHECK)
		{
			KWIN_Item_Show(hdlg, IDC_LIST_RECIPEDIR);
			DirData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEDIR));
		}
		else
		{
			KWIN_Item_Hide(hdlg, IDC_LIST_RECIPEDIR);
		}
		//
		RECIPE_CONTROL_INIT_PROC(hdlg);
		RECIPE_CONTROL_Set_FILE_CHANGE(FALSE);
		RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, "");
		RECIPE_CONTROL_ALL_DATA_DISPLAY(hdlg);
		RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, TRUE);
		//
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 0);
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 0);
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 0);
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 0);
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EXPORT, 0); //

		// ycchoi(2006.02.03)
		// EDIT Button 추가(NRD요청사항)
		FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_ENABLE, 1);

		Add_Recipe_Type_To_ComboBox(hdlg);
		SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, 0, 0);
		Set_Default_Group_Item(hdlg);
		MFC_SCROLL_Ctrl_Initialize(hdlg);

		return TRUE;

	case WM_PAINT:
		hDC = BeginPaint(hdlg, &ps);
		KGUI_STANDARD_Display_for_Redraw(1, ps.hdc);
		Title_Draw(&hDC);
		Draw_MFC_Title(hdlg);
		EndPaint(hdlg, &ps);
		RECIPE_CONTROL_ALL_DATA_DISPLAY(hdlg);
		return TRUE;

	case WM_DRAWITEM:
		KWIN_DRAW_BUTTON_ITEM(GetModuleHandle(NULL), (LPDRAWITEMSTRUCT)lParam);
		hDC = GetDC(hdlg);
		Title_Draw(&hDC);
		Draw_MFC_Title(hdlg);
		ReleaseDC(hdlg, hDC);
		return TRUE;

	case WM_LBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		if (((x >= 377) && (x <= 466)) && ((y >= 75) && (y <= 99)) && (D_HAS_STEP[0]))
			Msg2 = MAKEWPARAM(IDC_MAIN_RCP1, 0);
		if (((x >= 646) && (x <= 735)) && ((y >= 75) && (y <= 99)) && (D_HAS_STEP[1]))
			Msg2 = MAKEWPARAM(IDC_MAIN_RCP2, 0);

		Change = FALSE;
		Redraw = FALSE;

		if (RECIPE_CONTROL_BUTTON_HANDLING(hdlg, Msg2, &Change, &Redraw))
		{
			if (Change)
			{
				RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
				RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, FALSE);
				//
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 0);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 1);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 1);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 1);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 0);
			}
			if (Redraw)
			{
				InvalidateRect(hdlg, NULL, FALSE);
			}
		}

		return TRUE;

	case WM_VSCROLL:
		hVScroll = GetDlgItem(hdlg, IDC_SCROLLBAR1);
		dvPos = GetScrollPos(hVScroll, SB_CTL);

		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			dvPos = max(0, dvPos - 1);
			break;

		case SB_LINEDOWN:
			dvPos = min(18, dvPos + 1);
			break;

		case SB_PAGEUP:
			dvPos = max(0, dvPos - 10);
			break;

		case SB_PAGEDOWN:
			dvPos = min(18, dvPos + 10);
			break;

		case SB_THUMBTRACK:
			dvPos = HIWORD(wParam);
			break;
		}

		Draw_MFC_Title(hdlg);
		Draw_MFC_Contents(hdlg);
		//			SetScrollPos( hVScroll, SB_CTL, dvPos, TRUE );
		return FALSE;

	case WM_COMMAND:
		if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_SELCHANGE)
		{
			if (GET_WM_COMMAND_ID(wParam, lParam) == IDC_LIST_RECIPEFILE)
			{
				i = (int)ListBox_GetCurSel(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE));

				if (i >= 0)
				{
					g_bTEST_GROUP_CHK = FALSE;
					g_bReciepPW_CHK = FALSE;
					g_bRecipePWNull = FALSE;
					ListBox_GetText(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), i, Buffer);

					RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, Buffer);

					if (strcmp(g_szRecipeName, Buffer) == 0)
					{
						g_bReciepPW_CHK = TRUE;
					}
					else
					{
						strcpy(g_szRecipeName, "");
					}				
					
					RECIPE_CONTROL_FILE_OPEN_PROC(hdlg, Buffer);
					FileButton_Fresh_View(hdlg);
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 1);
				}
			}

			if (GROUP_RUN_CHECK)
			{
				if (GET_WM_COMMAND_ID(wParam, lParam) == IDC_LIST_RECIPEDIR)
				{
					i = (int)ListBox_GetCurSel(GetDlgItem(hdlg, IDC_LIST_RECIPEDIR));
					if (i >= 0)
					{
						ListBox_GetText(GetDlgItem(hdlg, IDC_LIST_RECIPEDIR), i, Buffer);

						//---> DEL 2012/08/24 Encryption by KSM
						//RECIPE_CONTROL_Set_GROUP_DIRECTORY( Buffer );
						//---> END 2012/08/24 Encryption by KSM

						//---> ADD 2012/08/24 Encryption by KSM
						if (STRCMP_L(Buffer, "RUN"))
						{
							g_bTEST_GROUP_CHK = FALSE;
							gbEditMode = FALSE;
							FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_ENABLE, 1);

							if (READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
							{
								RECIPE_CONTROL_Set_GROUP_DIRECTORY(""); // Recipe Display Clear
							}
							else
							{
								RECIPE_CONTROL_Set_GROUP_DIRECTORY(Buffer);
							}
						}
						else
						{
							if (STRCMP_L(Buffer, "TEST"))
							{
								g_bTEST_GROUP_CHK = TRUE;
							}
							else
							{
								g_bTEST_GROUP_CHK = FALSE;
							}
							RECIPE_CONTROL_Set_GROUP_DIRECTORY(Buffer);
						}
						//---> END 2012/08/24 Encryption by KSM

						//--------------------------
						RECIPE_CONTROL_Set_FILE_CHANGE(FALSE);
						RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, "");
						//
						Set_File_Filter_Name(hdlg, -1, szFilterName);
						FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);
						//
						RECIPE_CONTROL_ALL_DATA_DISPLAY(hdlg);
						RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, TRUE);
						//
						FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
						FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 0);
						FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 0);
						FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 0);
						FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 0);
						FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EXPORT, 0);
						//--------------------------
					}
				}
			}
			return TRUE;
		}

		switch (LOWORD(wParam))
		{

		case IDC_COMBO1:
			switch (HIWORD(wParam))
			{
			case CBN_SELENDOK:
				dComboIndex = SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
				Set_File_Filter_Name(hdlg, dComboIndex, szFilterName);
				FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);
				break;
			}
			return TRUE;

		case IDC_CTRL_RELOAD_RECIPE:

			//---> ADD 2012/08/24 Encryption by KSM
			//DirData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_RECIPEDIR ) );
			//---> END 2012/08/24 Encryption by KSM

			//---> ADD 2012/08/24 Encryption by KSM
			if (!g_bTEST_GROUP_CHK && READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
			{
				;
			}
			else
			{
				DirData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEDIR));
			}
			//---> END 2012/08/24 Encryption by KSM

			Set_File_Filter_Name(hdlg, -1, szFilterName);
			FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);
			return TRUE;
			//==============================================================================
			//==============================================================================

		case IDB_NEW_ENABLE:

			//---> ADD 2012/08/24 Encryption by KSM
			if (!g_bTEST_GROUP_CHK && READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
			{
				MessageBox(hdlg, "Operation Impossible(UserLevel Low)", "Operation Impossible", MB_ICONHAND);
				return TRUE;
			}
			//---> END 2012/08/24 Encryption by KSM

			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 2);
			sprintf(Buffer, "%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), RECIPE_CONTROL_Get_GROUP_DIRECTORY(), RECIPE_CONTROL_Get_FILE_DIRECTORY());

			g_bNEW_FLAG = TRUE;
			dTempRecipeType = g_RcpFileType;

			if (!ITEM_BUTTON_RECIPE_TYPE(hdlg))
			{
				// Cancel이 선택된 경우
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
				return TRUE;
			}
			else
			{
				// Recipe Type이  선택되지 않은 경우...
				if (g_RcpFileType == RCP_NONE)
				{
					g_RcpFileType = dTempRecipeType;
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
					return TRUE;
				}
			}
			g_bNEW_FLAG = FALSE;

			Set_File_Filter_Name(hdlg, g_RcpFileType, szFilterName);
			if (MODAL_FILE_SAVE_BOX(hdlg, Buffer, "Type New File Name?", szFilterName, "", Buffer2))
			{
				Kor_All2FileName(Buffer, Buffer2, 255);
				RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, Buffer);
				RECIPE_CONTROL_FILE_NEW_PROC(hdlg, Buffer);

				// FA Start
				////////////////// CREATE EVENT
				if (STRCMP(RECIPE_CONTROL_Get_GROUP_DIRECTORY(), "."))
				{ // No Group
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s|%d|%d", g_szModule, Buffer /*FileName*/, 0, 0 /*Create*/);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				else
				{ // Use Group
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s:(%s)|%d|%d", g_szModule, Buffer /*FileName*/, RECIPE_CONTROL_Get_GROUP_DIRECTORY(), 0, 0 /*Create*/);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				//////////////////

				FileButton_Fresh_View(hdlg);
				Set_File_Filter_Name(hdlg, -1, szFilterName);
				FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);

				// 2006.09.06
				gbEditMode = TRUE;
				g_bRecipePWNull = TRUE;
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_PRESSED, 1);
			}
			else
			{
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
			}
			return TRUE;

		case IDB_SAVE_ENABLE:
			// Recipe Data
			/*
					if ( g_RcpFileType == RCP_NONE )
					{
						if ( !ITEM_BUTTON_RECIPE_TYPE( hdlg ) )
						{
							MessageBox( hdlg, "Recipe File Not Saved - Recipe Type Error", "Confirm",  MB_ICONERROR );
							return TRUE;
						}
					}
					*/

			//---> ADD 2012/08/24 Encryption by KSM
			if (!g_bTEST_GROUP_CHK && READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
			{
				MessageBox(hdlg, "Operation Impossible(UserLevel Low)", "Operation Impossible", MB_ICONHAND);
				return TRUE;
			}
			//---> END 2012/08/24 Encryption by KSM
			hWnd_s = GetForegroundWindow();

			GoModalDialogBoxParam(GETHINST(hWnd_s), KWIN_MAKEINTRESOURCE(IDD_RECIPE_EDIT_POPUP, IDD_E_RECIPE_EDIT_POPUP), hWnd_s, View_Open_Recipe_Log_Proc, (LPARAM)NULL);

			RECIPE_CONTROL_Get_Selected_FILENAME(hdlg, Buffer, 128);

			if ( CONFIRM_OK )
			{
				if ( MessageBox( hdlg,"Are you sure to Save Recipe Data!!", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDNO )
				{
					return TRUE;
				}
				sprintf(m_szLog, "[Save] %s Edit End\n\n", Buffer);
				SampleLogSave(m_szLog, g_szModule);
				RcpEditLog(g_szModule);
				SampleLogDell(g_szModule);
			}
			else
			{
				return TRUE;
			}

			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 2);
			//
			/////////// It is added for check recipe step 2006.03.27 /////////////////////////////////////////////////////////
			//RECIPE_CONTROL_FILE_SAVE_PROC( hdlg , Buffer, 0 )
			if (!RECIPE_CONTROL_FILE_SAVE_PROC(hdlg, Buffer, 0))
			{
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 1);
				return FALSE;
			}
			/////////// It is added for check recipe step 2006.03.27 /////////////////////////////////////////////////////////

			// FA Start
			////////////////// UPDATE EVENT
			if (STRCMP(RECIPE_CONTROL_Get_GROUP_DIRECTORY(), "."))
			{ // No Group
				sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s|%d|%d", g_szModule, Buffer /*FileName*/, Kor_StepCount(), 1 /*Update*/);
				WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
			}
			else
			{ // Use Group
				sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s:(%s)|%d|%d", g_szModule, Buffer /*FileName*/, RECIPE_CONTROL_Get_GROUP_DIRECTORY(), Kor_StepCount(), 1 /*Update*/);
				WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
			}
			//////////////////

			FileButton_Fresh_View(hdlg);
			Set_File_Filter_Name(hdlg, g_RcpFileType, szFilterName);
			SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, g_RcpFileType, 0);
			FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);

			// 2006.09.06
			gbEditMode = FALSE;
			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_ENABLE, 1);
			
			return TRUE;

		case IDB_SAVEAS_ENABLE:
			//---> ADD 2012/08/24 Encryption by KSM
			if (!g_bTEST_GROUP_CHK && READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
			{
				MessageBox(hdlg, "Operation Impossible(UserLevel Low)", "Operation Impossible", MB_ICONHAND);
				return TRUE;
			}
			//---> END 2012/08/24 Encryption by KSM

			// Recipe Data
			if (MessageBox(hdlg, "Are you sure to Save Recipe Data!!", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO)
				return TRUE;

			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 2);

			// ycchoi(2006.02.03)
			// Save As 할 GROUP 선택
			mboxResult = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RECIPE_GROUP_DLG), hdlg, Recipe_Group_Select_Msg_Handler);
			if (mboxResult == IDOK)
			{
				sprintf(Buffer, "%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), gszTgtGroup, RECIPE_CONTROL_Get_FILE_DIRECTORY());
			}
			else
			{
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 1);
				return TRUE;
			}

			if (MODAL_FILE_SAVE_BOX(hdlg, Buffer, "Type Save as File Name?", "*.*", "", Buffer2))
			{
				Kor_All2FileName(Buffer, Buffer2, 255);
				//						RECIPE_CONTROL_Set_Selected_FILENAME( hdlg , Buffer );

				// FA Start
				////////////////// CREATE or UPDATE SEARCH

				// ycchoi(2006.02.03)
				// sprintf( Buffer3 , "%s/%s/%s/%s" , RECIPE_CONTROL_Get_MAIN_DIRECTORY() , RECIPE_CONTROL_Get_GROUP_DIRECTORY() , RECIPE_CONTROL_Get_FILE_DIRECTORY() , Buffer );
				sprintf(Buffer3, "%s/%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), gszTgtGroup, RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer);

				hFind = FindFirstFile(Buffer3, &fd);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					FindClose(hFind);
					EditStatus = 1 /*Update*/;
				}
				else
					EditStatus = 0; /*Create*/

				/////////// It is added for check recipe step 2006.03.27 /////////////////////////////////////////////////////////
				//RECIPE_CONTROL_FILE_SAVE_PROC( hdlg , Buffer, 1 );
				if (!RECIPE_CONTROL_FILE_SAVE_PROC(hdlg, Buffer, 1))
				{
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 1);
					return FALSE;
				}
				/////////// It is added for check recipe step 2006.03.27 /////////////////////////////////////////////////////////

				////////////////// CREATE or UPDATE EVENT
				if (STRCMP(RECIPE_CONTROL_Get_GROUP_DIRECTORY(), "."))
				{ // No Group
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s|%d|%d", g_szModule, Buffer /*FileName*/, Kor_StepCount(), EditStatus);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				else
				{ // Use Group
					// sprintf( Buffer3 , "EVENT_ON RECIPE_EDIT %s|%s:(%s)|%d|%d" , g_szModule, Buffer/*FileName*/ , RECIPE_CONTROL_Get_GROUP_DIRECTORY() , Kor_StepCount() , EditStatus );
					// ycchoi(2006.02.03)
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s:(%s)|%d|%d", g_szModule, Buffer /*FileName*/, gszTgtGroup, Kor_StepCount(), EditStatus);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				//////////////////

				SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, g_RcpFileType, 0);
				Set_File_Filter_Name(hdlg, g_RcpFileType, szFilterName);
				FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);

				FileButton_Fresh_View(hdlg);
				SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, g_RcpFileType, 0);
				Set_File_Filter_Name(hdlg, g_RcpFileType, szFilterName);
				FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);

				//////////////////////////////////////////////////////////////////////////
				// ycchoi(2006.02.03)
				{
					HWND hList = GetDlgItem(hdlg, IDC_LIST_RECIPEDIR);
					int itemCnt = ListBox_GetCount(hList);
					int idx, dLength;
					char szBuffer[128] = "", szBuffer2[128] = "";

					for (idx = 0; idx < itemCnt; idx++)
					{
						ListBox_GetText(hList, idx, szBuffer);

						if (STRCMP(szBuffer, gszTgtGroup))
						{
							ListBox_SetCurSel(hList, idx);
							RECIPE_CONTROL_Set_GROUP_DIRECTORY(gszTgtGroup);
							break;
						}
					}

					Set_File_Filter_Name(hdlg, g_RcpFileType, szFilterName);
					FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);

					hList = GetDlgItem(hdlg, IDC_LIST_RECIPEFILE);
					itemCnt = ListBox_GetCount(hList);

					// 확장자 확인
					dLength = strlen(Buffer);
					if (dLength > 4)
					{
						if (Buffer[dLength - 4] == '.')
							strcpy(szBuffer2, Buffer);
						else
							sprintf(szBuffer2, "%s.%s", Buffer, &szFilterName[strlen(szFilterName) - 3]);
					}
					else
					{
						sprintf(szBuffer2, "%s.%s", Buffer, &szFilterName[strlen(szFilterName) - 3]);
					}

					for (idx = 0; idx < itemCnt; idx++)
					{
						ListBox_GetText(hList, idx, szBuffer);
						if (STRCMP(szBuffer, szBuffer2))
						{
							ListBox_SetCurSel(hList, idx);
							break;
						}
					}

					RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, szBuffer2);
					FileButton_Fresh_View(hdlg);

					//... 2014.10.06 Recipe File List Update Bug Fixed
					Set_File_Filter_Name(hdlg, g_RcpFileType, szFilterName);
					FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);
					//... 2014.10.06 Recipe File List Update Bug Fixed
				}
			}
			else
			{
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 1);
			}

			// 2006.09.06
			gbEditMode = FALSE;
			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_ENABLE, 1);

			return TRUE;

		case IDB_CANCEL_ENABLE:

			//---> ADD 2012/08/24 Encryption by KSM
			if (!g_bTEST_GROUP_CHK && READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
			{
				MessageBox(hdlg, "Operation Impossible(UserLevel Low)", "Operation Impossible", MB_ICONHAND);
				return TRUE;
			}
			//---> END 2012/08/24 Encryption by KSM

			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 2);
			if (IDYES == MessageBox(hdlg, "Really Cancel edited data and reload last saved data ???", "CANCEL Check", MB_ICONQUESTION | MB_YESNO))
			{
				RECIPE_CONTROL_Get_Selected_FILENAME(hdlg, Buffer, 128);
				RECIPE_CONTROL_FILE_OPEN_PROC(hdlg, Buffer);
				FileButton_Fresh_View(hdlg);
				SampleLogDell(g_szModule);
			}
			else
			{
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 1);
			}

			// 2006.09.06
			gbEditMode = FALSE;
			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_ENABLE, 1);
			return TRUE;

		case IDB_FILE_ENABLE:

			//---> ADD 2012/08/24 Encryption by KSM
			if (!g_bTEST_GROUP_CHK && READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
			{
				MessageBox(hdlg, "Operation Impossible(UserLevel Low)", "Operation Impossible", MB_ICONHAND);
				return TRUE;
			}
			//---> END 2012/08/24 Encryption by KSM

			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 2);
			if (IDYES == MessageBox(hdlg, "Really Delete Select File", "DELETE Check", MB_ICONQUESTION | MB_YESNO))
			{
				RECIPE_CONTROL_Get_Selected_FILENAME(hdlg, Buffer, 255);

				// FA Start
				//
				sprintf(Buffer2, "%s/%s/%s/%s", RECIPE_CONTROL_Get_MAIN_DIRECTORY(), RECIPE_CONTROL_Get_GROUP_DIRECTORY(), RECIPE_CONTROL_Get_FILE_DIRECTORY(), Buffer);
				DeleteFile(Buffer2);
				//
				////////////////// DELETE EVENT
				if (STRCMP(RECIPE_CONTROL_Get_GROUP_DIRECTORY(), "."))
				{ // No Group
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s|%d|%d", g_szModule, Buffer /*FileName*/, 0, 2 /*Delete*/);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				else
				{ // Use Group
					sprintf(Buffer3, "EVENT_ON RECIPE_EDIT %s|%s:(%s)|%d|%d", g_szModule, Buffer /*FileName*/, RECIPE_CONTROL_Get_GROUP_DIRECTORY(), 0, 2 /*Delete*/);
					WRITE_STRING(SCHEDULER, Buffer3, &CommStatus);
				}
				//////////////////

				RECIPE_CONTROL_Set_FILE_CHANGE(FALSE);
				RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, "");
				//
				Set_File_Filter_Name(hdlg, -1, szFilterName);
				FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), szFilterName);
				//
				RECIPE_CONTROL_ALL_DATA_DISPLAY(hdlg);
				RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, TRUE);
				//
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 0);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 0);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 0);
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 0);
				//
			}
			else
			{
				FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 1);
			}
			
			// 2006.09.06
			gbEditMode = FALSE;
			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_ENABLE, 1);
			return TRUE;

		// ycchoi(2006.02.03)
		// EDIT Button 추가(NRD요청사항)
		case IDB_FEDIT_ENABLE:

			//---> DEL 2012/08/24 Encryption by KSM
			/*
					if(MODAL_MESSAGE_BOX(hdlg, "Message Check" , "Are you sure to make header screen editable?" ,"Yes|No" ,0 , &mboxResult))
					{
						if(mboxResult == 0)
						{
							gbEditMode = TRUE;
							FILE_HANDLING_BUTTON_MAPPING( hdlg , BUTTON_EDIT_PRESSED , 1 );
						}
					}
					*/
			//---> END 2012/08/24 Encryption by KSM
			SampleLogDell(g_szModule);
			RECIPE_CONTROL_Get_Selected_FILENAME(hdlg, Buffer, 255);
			//---> ADD 2012/08/24 Encryption by KSM
			if (!g_bTEST_GROUP_CHK)
			{
				if (READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
				{
					MessageBox(hdlg, "Operation Impossible(UserLevel Low)", "Operation Impossible", MB_ICONHAND);
					return TRUE;
				}
				
				hWnd_s = GetForegroundWindow();

				if (GoModalDialogBoxParam(GETHINST(hWnd_s), MAKEINTRESOURCE(IDD_CONTROL_USER), hWnd_s, Console_Gui_For_LogIn_Proc, (LPARAM)NULL))
				{
					sprintf(m_szLog, "[Edit] %s Edit Start \n", Buffer);
					SampleLogSave(m_szLog, g_szModule);
				}
				else
				{
					return TRUE;
				}
			}
			else
			{
				if (MODAL_MESSAGE_BOX(hdlg, "Message Check", "Are you sure to make header screen editable?", "Yes|No", 0, &mboxResult))
				{
					if (mboxResult == 0)
					{
						sprintf(m_szLog, "[Edit] %s Edit Start \n", Buffer);
						SampleLogSave(m_szLog, g_szModule);
					}
					else
						return TRUE;
				}
				else
				{
					return TRUE;
				}
			}

			gbEditMode = TRUE;
			KWIN_Item_Disable(hdlg, IDC_LIST_RECIPEFILE);
			FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_PRESSED, 1);
			//---> END 2012/08/24 Encryption by KSM			
			break;

		// ycchoi(2006.02.03)
		// EDIT Button 추가(NRD요청사항)
		case IDB_FEDIT_PRESSED:
			if (MODAL_MESSAGE_BOX(hdlg, "Message Check", "Are you sure to make header screen disable?", "Yes|No", 0, &mboxResult))
			{
				if (mboxResult == 0)
				{
					gbEditMode = FALSE;
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EDIT_ENABLE, 1);
					KWIN_Item_Enable(hdlg, IDC_LIST_RECIPEFILE);
				}
			}

			break;

		case IDC_RECIPE_PW:

			if (g_bRecipePWNull == TRUE)
			{
				HWND hWnd_s = GetForegroundWindow();
				if (GoModalDialogBoxParam(GETHINST(hWnd_s), MAKEINTRESOURCE(IDD_RECIPEPW_FIRST), hWnd_s, Recipe_Password_Msg_Handler_New, (LPARAM)NULL))
				{
					// New Password Save Enable
					RECIPE_CONTROL_Get_Selected_FILENAME(hdlg, Buffer, 128);
					strcpy(g_szRecipeName, Buffer);
					strcpy(D_RECIPE0.szRecipePW, g_szPassword);

					RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
					RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, FALSE);
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 1);
				}
				else
				{
					return TRUE;
				}
			}
			else
			{
				HWND hWnd_s = GetForegroundWindow();
				RECIPE_CONTROL_Get_Selected_FILENAME(hdlg, Buffer, 128);
				if (GoModalDialogBoxParam(GETHINST(hWnd_s), MAKEINTRESOURCE(IDD_RECIPEPW), hWnd_s, Recipe_Password_Msg_Handler, (LPARAM)NULL))
				{
					RECIPE_CONTROL_Get_Selected_FILENAME(hdlg, Buffer, 128);

					if (strcmp(g_szRecipeName, Buffer) == 0)
					{
						;
					}
					else
					{
						strcpy(g_szRecipeName, Buffer);
						RECIPE_CONTROL_FILE_OPEN_PROC(hdlg, Buffer);
						FileButton_Fresh_View(hdlg);
					}
				}
				else
				{
					return TRUE;
				}
			}
			break;

		default:
			Change = FALSE;
			Redraw = FALSE;

			// Msg -> Recipe_Code.c (Item Button)
			if (RECIPE_CONTROL_BUTTON_HANDLING(hdlg, wParam, &Change, &Redraw))
			{
				if (Change)
				{
					RECIPE_CONTROL_Set_FILE_CHANGE(TRUE);
					RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, FALSE);
					//
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 0);
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 1);
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 1);
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 1);
					FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 0);
				}
				if (Redraw)
				{
					InvalidateRect(hdlg, NULL, FALSE);
				}
			}
			return TRUE;
		}
		return TRUE;

	case WM_DESTROY:
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
extern BOOL g_bRF;
extern BOOL g_bRF_SetPointUse;
extern BOOL g_bRF2_SetPointUse; // 2014.03.31
extern BOOL g_bMFC_RampUse;

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main()
{
	char NAME1[128 + 1];
	char NAMEB[128 + 1];
	char NAME2[128 + 1];
	char NAME3[128 + 1];
	char NAME4[128 + 1];
	int nParLen = 0;
	char *pParam;

	printf("--------- Process Recipe Program Main ---------------\n");

	// 2004.09.09 by cskim
	pParam = PROGRAM_PARAMETER_READ();
	nParLen = strlen(pParam);
	if (nParLen > 0)
	{
		strcpy(g_szModule, pParam);
	}

	STR_SEPERATE_CHAR(PROGRAM_ARGUMENT_READ(), '|', NAME1, NAME4, 128);
	STR_SEPERATE_CHAR(NAME4, '|', NAME2, NAMEB, 128);
	STR_SEPERATE_CHAR(NAMEB, '|', NAME3, NAME4, 128);
	RECIPE_CONTROL_Set_MAIN_DIRECTORY(NAME2);
	RECIPE_CONTROL_Set_GROUP_DIRECTORY(".");
	RECIPE_CONTROL_Set_FILE_DIRECTORY(NAME3);
	//
	if ((strlen(NAME4) > 0) && (NAME4[0] == 'G'))
	{
		GROUP_RUN_CHECK = TRUE;
	}
	else
	{
		GROUP_RUN_CHECK = FALSE;
	}

	//>>-------------------------------------------------------------------------
	if (!KGUI_STANDARD_Display_Information_Initial(NAME1))
	{
		SYSTEM_KILL_MYSELF();
		return SYS_ABORTED;
	}
	//>>-------------------------------------------------------------------------

	Read_Ini_File();

	g_hInstance = GetModuleHandle(NULL);
	GoModalDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_MAIN_CONTROL), GET_MAIN_WINDOW(), Console_Gui_For_Main_Proc, (LPARAM)NULL);
	SYSTEM_KILL_MYSELF();
	return (SYS_SUCCESS);
}
//--------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// 2005.11.08 by ycchoi
// Group Authority 기능의 사용 여부를 환경 파일에서 읽어 온다.
//----------------------------------------------------------------------------------------------------
BOOL UseGroupAuthority()
{
	char use[16];
	FILE *fp;

	// GroupInfo.dat 파일이 존재하는지 확인하고 없으면 생성한다.
	if ((fp = fopen(GROUP_FOLDER_INFO_FILE, "r")) == NULL)
	{
		if (GetLastError() == ERROR_PATH_NOT_FOUND)
		{

			CreateDirectory("Info", NULL);
		}
	}
	else
	{
		fclose(fp);
	}

	GetPrivateProfileString("GROUP_AUTHORITY", "USE", "NO", use, sizeof(use), GROUP_FOLDER_INFO_FILE);
	WritePrivateProfileString("GROUP_AUTHORITY", "USE", use, GROUP_FOLDER_INFO_FILE);

	if (STRCMP_L(use, "YES"))
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------------------
// 2005.11.07 by ycchoi
// folder가 현재 사용자가 사용할 수 있는 지 확인한다.
//----------------------------------------------------------------------------------------------------
BOOL authorityDir(char *folder)
{
	int i;
	BOOL result = FALSE;

	for (i = 0; i < gGroupFolderCount; i++)
	{
		if (STRCMP_L(GroupFolder[i], folder))
		{
			result = TRUE;
			break;
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------------------
// 2005.11.07 by ycchoi
// 현재 User의 Recipe Group Folder정보를 메모리로 로드하고 Folder 수를 리턴한다.
//----------------------------------------------------------------------------------------------------
int LoadGroupFolderInfo()
{
	char user[65], buf1[256], Key[256];
	int i, gfolderCnt;

	// group fold info init
	memset(&GroupFolder[0][0], 0x00, sizeof(GroupFolder));

	GET_CURRENT_USERNAME(user);

#ifdef __DEBUG__
	printf("--->DirRecipeEditor::LoadGroupFolderInfo ==>> User[%s]\n", user);
#endif

	GetPrivateProfileString(user, "GROUP_FOLDER_COUNT", "0", buf1, sizeof(buf1), GROUP_FOLDER_INFO_FILE);
	gfolderCnt = atoi(buf1);
	for (i = 0; i < gfolderCnt; i++)
	{
		sprintf(Key, "GFOLD%03d", i + 1);
		GetPrivateProfileString(user, Key, "EMPTY", buf1, sizeof(buf1), GROUP_FOLDER_INFO_FILE);

#ifdef __DEBUG__
		printf("--->DirRecipeEditor::LoadGroupFolderInfo ==>> GFold[%d] - [%s]\n", i + 1, buf1);
#endif

		strcpy(GroupFolder[i], buf1);
	}

	return gfolderCnt;
}

//----------------------------------------------------------------------------------------------------
// 2005.11.18 by ycchoi
// Group Authority 기능사용으로 인해서 USER변경이나 새로운 Group추가시 화면을 초기화 시킨다.
//----------------------------------------------------------------------------------------------------
BOOL InitGui()
{
	HWND hdlg = gMainHWnd;

	//---------- 2005.12.13 by ycchoi ---------------------------------------------------->>
	// Detail Valve Recipe Dialog Close
	if (hWnd_DetailValueRecipe)
	{
#ifdef _DEBUG
		_IO_CIM_PRINTF("RecipeEditor::InitGui ==> Destroy DetailValve[hWnd:%d][Result:%d]\n",
					   hWnd_DetailValueRecipe, EndDialog(hWnd_DetailValueRecipe, 0));
#else
		EndDialog(hWnd_DetailValueRecipe, 0);
#endif
	}
	else
	{
#ifdef _DEBUG
		_IO_CIM_PRINTF("Detail Valve Recipe Dialog Wndow Handle is NULL\n");
#endif
	}

	// Valve Recipe Dialog Close
	if (hWnd_ValveRecipe)
	{
#ifdef _DEBUG
		_IO_CIM_PRINTF("RecipeEditor::InitGui ==> Destroy Valve[hWnd:%d][Result:%d]\n",
					   hWnd_ValveRecipe, EndDialog(hWnd_ValveRecipe, 0));
#else
		EndDialog(hWnd_ValveRecipe, 0);
#endif
	}
	else
	{
#ifdef _DEBUG
		_IO_CIM_PRINTF("Valve Recipe Dialog Wndow Handle is NULL\n");
#endif
	}

	// SubItem Recipe Dialog Close
	if (HWnd_SubItemRecipe)
	{
#ifdef _DEBUG
		_IO_CIM_PRINTF("RecipeEditor::InitGui ==> Destroy SubItem[hWnd:%d][Result:%d]\n",
					   HWnd_SubItemRecipe, EndDialog(HWnd_SubItemRecipe, 0));
#else
		EndDialog(HWnd_SubItemRecipe, 0);
#endif
	}
	else
	{
#ifdef _DEBUG
		_IO_CIM_PRINTF("SubItem Recipe Dialog Wndow Handle is NULL\n");
#endif
	}

	//---------- 2005.12.13 by ycchoi ----------------------------------------------------<<

	RECIPE_CONTROL_Set_GROUP_DIRECTORY("");

	FileData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEFILE), "*.*");

	if (GROUP_RUN_CHECK)
	{
		KWIN_Item_Show(hdlg, IDC_LIST_RECIPEDIR);
		DirData_ListView_Init(GetDlgItem(hdlg, IDC_LIST_RECIPEDIR));
	}
	else
	{
		KWIN_Item_Hide(hdlg, IDC_LIST_RECIPEDIR);
	}
	//
	RECIPE_CONTROL_INIT_PROC(hdlg);
	RECIPE_CONTROL_Set_FILE_CHANGE(FALSE);
	RECIPE_CONTROL_Set_Selected_FILENAME(hdlg, "");
	RECIPE_CONTROL_ALL_DATA_DISPLAY(hdlg);
	RECIPE_CONTROL_Set_Select_ANOTHERFILE(hdlg, TRUE);
	//
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_NEW, 1);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVE, 0);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_SAVEAS, 0);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_CANCEL, 0);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_FILE, 0);
	FILE_HANDLING_BUTTON_MAPPING(hdlg, BUTTON_EXPORT, 0); //

	SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, 0, 0);
	Set_Default_Group_Item(hdlg);
	MFC_SCROLL_Ctrl_Initialize(hdlg);

	return TRUE;
}

//------------------------------------------------------------------------
// ycchoi(2006.02.03)
//
BOOL APIENTRY Recipe_Group_Select_Msg_Handler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static char szGroupName[MAX_PATH];

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		int nCX = GetSystemMetrics(SM_CXSCREEN) / 2;
		int nCY = GetSystemMetrics(SM_CYSCREEN) / 2;
		RECT r;

		HANDLE hFind;
		WIN32_FIND_DATA fd;
		BOOL bRet = TRUE;
		HWND hWndList = GetDlgItem(hdlg, IDC_GROUP_LIST);

		char szRecipeFoler[512];
		sprintf(szRecipeFoler, "%c:%s\\*.*", _getdrive() + 'A' - 1, RECIPE_CONTROL_Get_MAIN_DIRECTORY());

		hFind = FindFirstFile(szRecipeFoler, &fd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			EndDialog(hdlg, IDCANCEL);
			return FALSE;
		}

		while ((hFind != INVALID_HANDLE_VALUE) && bRet)
		{
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if ((strcmp(fd.cFileName, ".") != 0) &&
					(strcmp(fd.cFileName, "..") != 0))
				{
					ListBox_AddString(hWndList, fd.cFileName);
				}
			}
			bRet = FindNextFile(hFind, &fd);
		}

		GetWindowRect(hdlg, &r);
		MoveWindow(hdlg, nCX - ((r.right - r.left) / 2), nCY - ((r.bottom - r.top) / 2), r.right - r.left, r.bottom - r.top, TRUE);
	}
		return FALSE;

	case WM_PAINT:

	case WM_COMMAND:
	{
		int i;
		switch (wParam)
		{
		case IDOK:
			i = (int)ListBox_GetCurSel(GetDlgItem(hdlg, IDC_GROUP_LIST));
			if (i >= 0)
			{
				ListBox_GetText(GetDlgItem(hdlg, IDC_GROUP_LIST), i, gszTgtGroup);
				EndDialog(hdlg, IDOK);
			}
			else
			{
				EndDialog(hdlg, IDCANCEL);
			}
			break;

		case IDCANCEL:
			EndDialog(hdlg, IDCANCEL);
			break;
		}
	}
		return FALSE;
	}

	return FALSE;
}

//---> ADD 2012/08/24 Encryption by KSM
void Event_Message_Received()
{
	unsigned char *Ed;
	int Dl, CommStatus;
	SYSTEMTIME st;

	SendMessage(gMainHWnd, WM_USER, WM_USER, 0);

	Ed = PROGRAM_EVENT_READ();
	Dl = strlen(Ed);
	Ed[Dl] = 0x00;

	if (STRCMP_L(Ed, "LOGIN"))
	{
		FILE_HANDLING_BUTTON_MAPPING(gMainHWnd, BUTTON_NEW, 1);
		FILE_HANDLING_BUTTON_MAPPING(gMainHWnd, BUTTON_SAVE, 0);
		FILE_HANDLING_BUTTON_MAPPING(gMainHWnd, BUTTON_SAVEAS, 0);
		FILE_HANDLING_BUTTON_MAPPING(gMainHWnd, BUTTON_CANCEL, 0);
		FILE_HANDLING_BUTTON_MAPPING(gMainHWnd, BUTTON_FILE, 0);
		FILE_HANDLING_BUTTON_MAPPING(gMainHWnd, BUTTON_EDIT_ENABLE, 1);
		gbEditMode = FALSE;
		g_bTEST_GROUP_CHK = FALSE;

		_sleep(1000);														 // Wait Updating IO
		if (READ_DIGITAL(LOGIN_USER_LEVEL, &CommStatus) < USER_ACCESS_LEVEL) //--> Possible Level 8,9
		{
			GetLocalTime(&st);
			printf("[%02d:%02d:%02d.%03d] : < LOGIN EVENT RECEIVED : %s >  (Level Low)\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, Ed);

			RECIPE_CONTROL_Set_GROUP_DIRECTORY("");
			DirData_ListView_Init(GetDlgItem(gMainHWnd, IDC_LIST_RECIPEFILE));
			//
			RECIPE_CONTROL_Set_FILE_CHANGE(FALSE);
			RECIPE_CONTROL_Set_Selected_FILENAME(gMainHWnd, "");
			//
			Set_File_Filter_Name(gMainHWnd, -1, "");
			FileData_ListView_Init(GetDlgItem(gMainHWnd, IDC_LIST_RECIPEFILE), "");
			//
			RECIPE_CONTROL_ALL_DATA_DISPLAY(gMainHWnd);
			RECIPE_CONTROL_Set_Select_ANOTHERFILE(gMainHWnd, TRUE);
		}
	}
}
//---> END 2012/08/24 Encryption by KSM
