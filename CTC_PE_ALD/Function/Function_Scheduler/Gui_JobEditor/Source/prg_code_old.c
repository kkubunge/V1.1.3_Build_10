
/*

LOTLOG FOLDER의 PROCESS_01_PM1 의 쌍으로

		%s_STEPINFO	가 존재할때 Tree View의 PM정보 밑에 STEP 별 Node 생성

			Format (Step No는 0부터 1 Step)

			시간				StepNo	FileStartIndex	FileLastIndex	StepName
			2018/2/13 18:16:32	0		50				200				STEP_TEST1
			2018/2/13 18:16:32	1		200				400				STEP_TEST2
			2018/2/13 18:16:32	2		400				800				STEP_TEST3

		%s_STEPVIEW	가 존재할때


			Format

			TARGET		IOName Data ....			double ...

			STABLE		IOName Data ....			double ...

			USER		IOName Data ....			string ...

			TIMESTABLE	IOName Data ....			string time ...

			TIMESTEP	IOName Data ....			string time ...

			STEP		SETTIME Data ....			string ...
			STEP		RESULT  Data ....			string ...

			OPTION		User_Title	TimeView_Option	DecimalPoint_Option

						User_Title					string
						TimeView_Option				int		xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
						DecimalPoint_Option			int		x.x x.xx x.xxx x.xxxx x


*/




//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

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
#include <Kutlgui2.h>

#include "appCTC.h"

#include "iodefine.h"
#include "lottype.h"
#include "ledapi.h"
#include "appinit.h"
#include "appprm.h"
#include "Jobdata.h"
#include "logview.h"
#include "Stepview.h"
#include "AppPlotting.h"

#include "cimseqnc.h"
#include "cimseqcdllmain.h"
#include "resource.h"

#include <CimSeqncKNFS.h>
//

void Time_2_String( double , char *szString );



BOOL LogView_Control_View_Change( HWND hWnd , BOOL multi );


BOOL JOB_User_Fail_Check( int index ); // 2015.11.12
int JOB_User_Get_Icon( int index ); // 2016.03.29
int JOB_User_Get_Fail_Data( int index , int mode ); // 2016.03.29
//
BOOL Log_Get_DiffTime_from_Time( char *dataE , char *dataS , double minoffset , char *diffdata ); // 2016.08.23
BOOL Log_Get_ExtractTime_from_Time( char *srcdata , char *trgdata ); // 2016.10.31

BOOL NAME_MANUAL_to_TIME = FALSE; // 2016.04.28

extern BOOL							JOB_HANDOFF[MAX_JOB_COUNT];
extern int							JOB_RUN[MAX_JOB_COUNT];

void Get_Elapsed_String( int index , char *szString , BOOL avr );


int IDC_CTRL_SPM_MODE = 0;

BOOL AUTO_STEPVIEW = FALSE; // 2018.04.24

//------------------------------------------------------------------------------------------
int GLISTVIEW_HEIGHT = -1; // 2017.03.16
//------------------------------------------------------------------------------------------

int LISTVIEW_HEIGHT; // 2015.07.23
int LISTVIEW_HEIGHT_UPDATE = FALSE; // 2015.07.29
double LISTVIEW_HEIGHT_RATIO = 1; // 2015.07.29
//
BOOL Log_Get_Save_File_Name( HWND hWnd , char *FileName );
void Log_Get_String_Form( HWND hWnd , int index , int start , int end , FILE *fpt );

HIMAGELIST		g_Tree_Image;
//


int FLOW_HISTORY_M_INDEX[MAX_MULTI_CHECK_JOB];

int IDC_CONTROL_S2_AREA_LAST_OPEN;
char IDC_CONTROL_S2_AREA_LAST_DATA[256] = { 0 , };
char IDC_CONTROL_S2_AREA_LAST_MODULE[256] = { 0 , };

int IDC_CONTROL_S2_FROM_LAST_OPEN;

int CTRL_1_SELECT_DATA	=	0;
int CTRL_2_SELECT_DATA	=	0;
int CTRL_3_SELECT_DATA	=	0;
int CTRL_4_SELECT_DATA	=	0;
int CTRL_5_SELECT_DATA	=	0;
int CTRL_6_SELECT_DATA	=	0;
int CTRL_7_SELECT_DATA	=	0;
int CTRL_8_SELECT_DATA	=	0;

int CTRL_SELECT_MAX_USE =	0;

//
extern JOBStepTemplate				JOB_DATA[MAX_JOB_COUNT];

//#include <fcntl.h>   // 2010.10.27 jiahn ( Required by T ) // 2010.11.18
//#include <io.h>      // 2010.10.27 jiahn ( Required by T ) // 2010.11.18
//
int LogView_GetParam( HWND hList , int r );
LRESULT LogView_Open_Method_History_Notify( HWND hWnd , LPARAM lParam );
LRESULT LogView_Open_Method_History_Notify3( HWND hWnd , LPARAM lParam );
LRESULT LogView_Open_Method_History_NotifySPM( HWND hWnd , LPARAM lParam );
LRESULT LogView_Open_Method_History_NotifySPMS( HWND hWnd , LPARAM lParam );
void ListView_Draw_User_with_WM_DRAWITEM( HWND hwnd , LPARAM lParam , int xs , BOOL error , int align , char *font , char *buffer , int buffercount , int colorset , int lineset , int spcolor1 , int spcolor2 , int spcolor3 , int spcolor4 );
void ListView_Draw_User_with_WM_DRAWITEM_User9000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int mode );
void ListView_Draw_User_with_WM_DRAWITEM_User8000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount );
void ListView_Draw_User_with_WM_DRAWITEM_User7000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount );
void ListView_Draw_User_with_WM_DRAWITEM_User6000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int mode );
void ListView_Draw_User_with_WM_DRAWITEM_User5000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int index , int wfrid );
void ListView_Draw_User_with_WM_DRAWITEM_User4000( HWND hwnd , LPARAM lParam , int xs , char *font , char *buffer , int buffercount , int index , int wfrid );
void ListView_Draw_User_with_WM_DRAWITEM_SPM( HWND hwnd , LPARAM lParam , int prm , char *buffer , int buffercount ); // 2016.08.18
void ListView_Draw_User_with_WM_DRAWITEM_SPMS( HWND hwnd , LPARAM lParam , char *buffer , int buffercount ); // 2016.08.18
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int STINFO_StepCount; // 2016.03.24
int STINFO_StepData[MAX_STEP_COUNT]; // 2016.03.24
char STINFO_StepName[MAX_STEP_COUNT][64]; // 2016.03.24 // 2016.09.21
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Log_Get_Step_String( int step , char *Buf ) {
	int i;
	//
	for ( i = 0 ; i < STINFO_StepCount ; i++ ) {
		if ( step == STINFO_StepData[i] ) {
			//
			if ( STINFO_StepName[i][0] == 0 ) {
				sprintf( Buf , "STEP %d" , STINFO_StepData[i] + 1 );
			}
			else {
				sprintf( Buf , "STEP %d (%s)" , STINFO_StepData[i] + 1 , STINFO_StepName[i] );
			}
			//
			return;
		}
	}
	//
	sprintf( Buf , "STEP %d" , step + 1 );
	//
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  LOG_ENCRIPTION = 0; // 2010.04.14
//------------------------------------------------------------------------------------------
HWND Main_Handle = NULL;
//------------------------------------------------------------------------------------------
char JOB_COMP_FILENAME[256]; // 2016.12.28

char JOB_DATA_FILENAME[256];
char JOB_DATA_FILENAME_ORG[256];
//
char JOB_INFO_FILENAME[256];
//=============================================================
BOOL LOG_WINDOW_OPEN = FALSE; // 2007.05.16
BOOL LOG_AUTODEL_OPEN = FALSE; // 2007.05.16
//=============================================================

// BOOL JOB_W_OPEN = FALSE; // 2003.09.16
// BOOL JOB_W_DELETE = FALSE; // 2003.09.16
// BOOL JOB_W_KILL = FALSE; // 2003.09.16

CRITICAL_SECTION CR_SC;
CRITICAL_SECTION CR_SCREG;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  Global_Pos_Font = 0;
int  Global_Pos_Font2 = 0;
char Global_Pos_Font_Name[256] = { 0 , };

int Global_Pos_X[MAX_BUTTON_COUNT]; // 2005.04.08
int Global_Pos_Y[MAX_BUTTON_COUNT]; // 2005.04.08
int Global_Pos_XS[MAX_BUTTON_COUNT]; // 2005.04.08
int Global_Pos_YS[MAX_BUTTON_COUNT]; // 2005.04.08
//------------------------------------------------------------------------------------------
//#define	JobLog_Info_File	"System\\Job_Editor_Info.inf"   // 2010.10.27 jiahn ( Required by T ) // 2010.11.18
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

HINSTANCE	BackBmpLib = NULL;
void		KGUI_Set_BACKGROUND( char *file ) { BackBmpLib = LoadLibrary( file ); }
HINSTANCE	KGUI_Get_BACKGROUND() { return BackBmpLib; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL TreeData_Get_Select_Slot_PM_Info( int index , int subid , char *waferstr , char *pmname , char *pmnameorg );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Get_Step_Info_Data_From_File( char *filename , BOOL allread );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  PROCESSDATA_STEPVIEW_INDEX  = 0; // 2016.08.17
int  PROCESSDATA_STEPVIEW_PRCS  = 0; // 2016.08.17
int  PROCESSDATA_STEPVIEW_SUB  = 0; // 2016.08.17
int  PROCESSDATA_STEPVIEW_LAST_INDEX  = 0; // 2016.08.17

BOOL PROCESSDATA_HAS_STEPVIEW( int index , int SubID , int Step ) { // 2016.08.17
	//
	char ResultString1[512];
	char ResultString2[512];
	char ResultString3[512];
	char pmorg[512];
	char Buffer[512];
	char folderstr[256];
	//
	FILE *fpt;
	BOOL bres;
	//
	STEP_CHECK_FILE_START();
	//
	if ( !TreeData_Get_Select_Slot_PM_Info( index , SubID , ResultString1 , ResultString2 , pmorg ) ) return FALSE;
	//
	JOB_Get_Directory( index , folderstr );
	//----------------------------------------------------------------------------------------------
	Get_Log_File_Name( Buffer , "PROCESS" , 0 , folderstr , TRUE , 0 , 0 , ResultString1 , 0 , pmorg , "" );
	//----------------------------------------------------------------------------------------------
	//
	// 2016.09.21
	//
	sprintf( ResultString3 , "%s_STEPINFO" , Buffer );
	//
//	STINFO_StepCount = 0; // 2018.04.23
	//
	Get_Step_Info_Data_From_File( ResultString3 , Step == -1 );
	//
	//
	//
	sprintf( ResultString3 , "%s_STEPVIEW" , Buffer );
	//
	if ( ( fpt = fopen( ResultString3 , "r" ) ) == NULL ) {
		//
		if ( AUTO_STEPVIEW ) return TRUE; // 2018.04.24
		//
		return FALSE;
	}
	//
	do {
		//
		bres = Read_Line_From_File4( fpt , Buffer , 255 );
		//
		if ( *Buffer != 0 ) {
			//
			STR_SEPERATE_CHAR( Buffer , 9 , ResultString1 , ResultString2 , 511 );
			//
			if      ( STRCMP_L( ResultString1 , "TARGET" ) ) {
				//
				STR_SEPERATE_CHAR( ResultString2 , 9 , ResultString1 , ResultString3 , 511 );
				//
				STEP_CHECK_FILE_APPEND_TARGET( ResultString1 , ResultString3 );
				//
			}
			else if ( STRCMP_L( ResultString1 , "STABLE" ) ) {
				//
				STR_SEPERATE_CHAR( ResultString2 , 9 , ResultString1 , ResultString3 , 511 );
				//
				STEP_CHECK_FILE_APPEND_STABLE( ResultString1 , ResultString3 );
				//
			}
			else if ( STRCMP_L( ResultString1 , "USER" ) ) {
				//
				STR_SEPERATE_CHAR( ResultString2 , 9 , ResultString1 , ResultString3 , 511 );
				//
				STEP_CHECK_FILE_APPEND_USER( ResultString1 , ResultString3 );
				//
			}
			else if ( STRCMP_L( ResultString1 , "TIMESTABLE" ) ) {
				//
				STR_SEPERATE_CHAR( ResultString2 , 9 , ResultString1 , ResultString3 , 511 );
				//
				STEP_CHECK_FILE_APPEND_TIMESTABLE( ResultString1 , ResultString3 );
				//
			}
			else if ( STRCMP_L( ResultString1 , "TIMESTEP" ) ) {
				//
				STR_SEPERATE_CHAR( ResultString2 , 9 , ResultString1 , ResultString3 , 511 );
				//
				STEP_CHECK_FILE_APPEND_TIMESTEP( ResultString1 , ResultString3 );
				//
			}
			else if ( STRCMP_L( ResultString1 , "STEP" ) ) {
				//
				STR_SEPERATE_CHAR( ResultString2 , 9 , ResultString1 , ResultString3 , 511 );
				//
				STEP_CHECK_FILE_APPEND_STEP( ResultString1 , ResultString3 );
				//
			}
			else if ( STRCMP_L( ResultString1 , "OPTION" ) ) {
				STEP_CHECK_FILE_APPEND_OPTION( ResultString2 );
			}
			//
		}
	}
	while ( bres );
	//
	fclose( fpt );
	//
	return TRUE;
}


BOOL JOBDATA_SPMS_INIT( HWND hwndListView ) {
	double dvratio;
	LV_COLUMN   lvColumn;
	//
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break;
//	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ); // 2016.10.26
	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT ); // 2016.10.26
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//
	switch( STEPVIEW_DATA.SV_Opt_Summary_CharSize ) {
	case 1 :
		 dvratio = 16.0 / 14.0;
		break;
	case 2 :
		 dvratio = 18.0 / 14.0;
		break;
	case 3 :
		 dvratio = 20.0 / 14.0;
		break;
	case 4 :
		 dvratio = 22.0 / 14.0;
		break;
	default :
		 dvratio = 1;
		break;
	}
	//
	lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Summary_ColumnSize_NameJob * dvratio );	lvColumn.pszText = "Name";		ListView_InsertColumn( hwndListView , 0 , &lvColumn );
	lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Summary_ColumnSize_DataJob * dvratio );	lvColumn.pszText = "Data";		ListView_InsertColumn( hwndListView , 1 , &lvColumn );
	lvColumn.cx = 8;																lvColumn.pszText = "";			ListView_InsertColumn( hwndListView , 2 , &lvColumn );
	lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Summary_ColumnSize_NamePrcs * dvratio );	lvColumn.pszText = "Name";		ListView_InsertColumn( hwndListView , 3 , &lvColumn );
	lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Summary_ColumnSize_DataPrcs * dvratio );	lvColumn.pszText = "Data";		ListView_InsertColumn( hwndListView , 4 , &lvColumn );
	//
	return TRUE;
}


BOOL JOBDATA_SPMS_INSERT( HWND hWnd , int i , int x , char *Name , char *Data ) {
	//
	LV_ITEM lvi;
	//
	if ( x == 0 ) {
		//
		lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lvi.state = lvi.stateMask = 0;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		//
		lvi.pszText		= Name;
		//
		lvi.cchTextMax	= 256;
		lvi.iImage		= i;
		lvi.lParam		= ( i * 10000 ) + 0;
		//
		if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
		//
		ListView_SetItemText( hWnd , i , 1 , Data );
	}
	else {
		//
		ListView_SetItemText( hWnd , i , 3 , Name );
		ListView_SetItemText( hWnd , i , 4 , Data );
		//
	}
	return TRUE;
}


BOOL JOBDATA_SPM_STEP_INIT( HWND hwndListView ) {
	double dvratio;
	LV_COLUMN   lvColumn;
	int c;
	//
	//	STEP StartTime EndTime ActualTime StableTime StepTime RcpSetTime Result
	//
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break;
//	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ); // 2016.10.26
	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT ); // 2016.10.26
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//
	switch( STEPVIEW_DATA.SV_Opt_StepItem_CharSize ) {
	case 1 :
		 dvratio = 16.0 / 14.0;
		break;
	case 2 :
		 dvratio = 18.0 / 14.0;
		break;
	case 3 :
		 dvratio = 20.0 / 14.0;
		break;
	case 4 :
		 dvratio = 22.0 / 14.0;
		break;
	default :
		 dvratio = 1;
		break;
	}
	//
	c = 0;
	//
	lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_Step * dvratio );				lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStartTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_StartTime * dvratio );	lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideEndTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_EndTime * dvratio );		lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideActualTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_ActualTime * dvratio );	lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStableTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_StableTime * dvratio );	lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStepTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_StepTime * dvratio );		lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideSetTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_SetTime * dvratio );		lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Step_View_HideResult == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Step_ColumnSize_Result * dvratio );		lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( c < 2 ) {
		lvColumn.cx = (int) ( (double) 140 * dvratio );									lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	return TRUE;
}


BOOL JOBDATA_SPM_ITEM_INIT( HWND hwndListView ) {
	double dvratio;
	LV_COLUMN   lvColumn;
	int c;
	//
	//	NO ITEM COUNT Target Start End Min Max Avr StdDev Stable Step User
	//
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break;
//	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ); // 2016.10.26
	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT ); // 2016.10.26
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//
	switch( STEPVIEW_DATA.SV_Opt_StepItem_CharSize ) {
	case 1 :
		 dvratio = 16.0 / 14.0;
		break;
	case 2 :
		 dvratio = 18.0 / 14.0;
		break;
	case 3 :
		 dvratio = 20.0 / 14.0;
		break;
	case 4 :
		 dvratio = 22.0 / 14.0;
		break;
	default :
		 dvratio = 1;
		break;
	}
	//
	c = 0;
	//
	lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_No * dvratio );					lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	//
	lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Name * dvratio );				lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideCount == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Count * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideTarget == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Target * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStart == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Start * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideEnd == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_End * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMin == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Min * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMax == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Max * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideAvr == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Avr * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStdDev == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_StdDev * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStblTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_StblTime * dvratio );		lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStepTime == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_StepTime * dvratio );		lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	if ( STEPVIEW_DATA.SV_Opt_Item_View_HideResult == 0 ) {
		lvColumn.cx = (int) ( (double) STEPVIEW_DATA.SV_Opt_Item_ColumnSize_Result * dvratio );			lvColumn.pszText = "";		ListView_InsertColumn( hwndListView , c , &lvColumn );	c++;
	}
	//
	return TRUE;
}


BOOL JOBDATA_SPM_INSERT( HWND hWnd , int i , int prm , char *Data1 , char *Data2 , char *Data3 , char *Data4 , char *Data5 , char *Data6 ) {
	//
	LV_ITEM lvi;
	//
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	//
	lvi.pszText		= Data1;
	//
	lvi.cchTextMax	= 256;
	lvi.iImage		= i;
	lvi.lParam		= prm;
	//
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	//
	if ( Data2 != NULL ) ListView_SetItemText( hWnd , i , 1 , Data2 );
	if ( Data3 != NULL ) ListView_SetItemText( hWnd , i , 2 , Data3 );
	if ( Data4 != NULL ) ListView_SetItemText( hWnd , i , 3 , Data4 );
	if ( Data5 != NULL ) ListView_SetItemText( hWnd , i , 4 , Data5 );
	if ( Data6 != NULL ) ListView_SetItemText( hWnd , i , 5 , Data6 );
	//
	return TRUE;
}

BOOL JOBDATA_SPM_UPDATE( HWND hWnd , int i , int x , char *Data1 , char *Data2 , char *Data3 , char *Data4 , char *Data5 , char *Data6 ) {
	//
	if ( Data1 != NULL ) ListView_SetItemText( hWnd , i , x   , Data1 );
	if ( Data2 != NULL ) ListView_SetItemText( hWnd , i , x+1 , Data2 );
	if ( Data3 != NULL ) ListView_SetItemText( hWnd , i , x+2 , Data3 );
	if ( Data4 != NULL ) ListView_SetItemText( hWnd , i , x+3 , Data4 );
	if ( Data5 != NULL ) ListView_SetItemText( hWnd , i , x+4 , Data5 );
	if ( Data6 != NULL ) ListView_SetItemText( hWnd , i , x+5 , Data6 );
	//
	return TRUE;
}


extern int    Step_Log_Item_Index[MAX_X_TABLE];
extern char   *Step_Log_Item_Name[MAX_X_TABLE];
extern int    Step_Log_Step_Count;
extern int    Step_Log_Item_Count;

extern char   Step_Log_FileName[128]; // 2018.04.24
extern char   Step_Log_StepName[MAX_STEP_COUNT][64]; // 2018.04.24
extern int    Step_Log_StepStart[MAX_STEP_COUNT]; // 2018.04.24
extern int    Step_Log_StepEnd[MAX_STEP_COUNT]; // 2018.04.24

//

extern int    StepView_TimeView_Option; // xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
extern int    StepView_DecimalPoint_Option; // x.x x.xx x.xxx x.xxxx x
extern char   *StepView_User_Title;

extern BOOL   StepView_HasTarget[MAX_STEP_COUNT][MAX_X_TABLE];
extern double StepView_TargetData[MAX_STEP_COUNT][MAX_X_TABLE];

extern char   *StepView_TimeStable[MAX_STEP_COUNT][MAX_X_TABLE];
extern char   *StepView_TimeStep[MAX_STEP_COUNT][MAX_X_TABLE];


extern char	*StepView_Run_StartTime;
extern char	*StepView_Run_EndTime;

extern char	*StepView_Pre_StartTime;
extern char	*StepView_Pre_EndTime;

extern char	*StepView_Step_StartTime;
extern char	*StepView_Step_EndTime;

extern char	*StepView_Post_StartTime;
extern char	*StepView_Post_EndTime;

extern char	*StepView_StartTime[MAX_STEP_COUNT];
extern char	*StepView_EndTime[MAX_STEP_COUNT];
extern int 	StepView_Count[MAX_STEP_COUNT];
extern double	StepView_Time[MAX_STEP_COUNT];

extern char	*StepView_SetTime[MAX_STEP_COUNT];
extern char	*StepView_Result[MAX_STEP_COUNT];

extern int 	StepView_Count_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double	StepView_Time_Step[MAX_STEP_COUNT][MAX_X_TABLE];

extern double	StepView_Time_Step_AllItem[MAX_STEP_COUNT];

extern char   *StepView_UserData[MAX_STEP_COUNT][MAX_X_TABLE];

extern int    Dat_Count_Total;
extern int    Dat_Count_Total_Step[MAX_STEP_COUNT];

extern BOOL   Dat_Set_StepUse[MAX_STEP_COUNT];

extern BOOL   Dat_Set_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Sum_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Sum2_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_First_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Last_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Min_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Max_Step[MAX_STEP_COUNT][MAX_X_TABLE];


extern int    Dat_Count_Sum_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern int    Dat_Count_MinMax_Step[MAX_STEP_COUNT][MAX_X_TABLE];


BOOL StringOnlyNo( char *data ) {
	int i , l;
	//
	l = strlen( data );
	//
	if ( l <= 0 ) return FALSE;
	//
	for ( i = 0 ; i < l ; i++ ) {
		if ( data[i] < '0' ) return FALSE;
		if ( data[i] > '9' ) return FALSE;
	}
	//
	return TRUE;
}

void StringConvData( char *Buf , double data ) {
	//
	switch( STEPVIEW_DATA.SV_Opt_DecimalPoint ) { // Def x.x x.xx x.xxx x.xxxx x
	case 1 :
		sprintf( Buf , "%.1f" , data );
		break;
	case 2 :
		sprintf( Buf , "%.2f" , data );
		break;
	case 3 :
		sprintf( Buf , "%.3f" , data );
		break;
	case 4 :
		sprintf( Buf , "%.4f" , data );
		break;
	case 5 :
		sprintf( Buf , "%.0f" , data );
		break;
	default :
		switch( StepView_DecimalPoint_Option ) { // x.x x.xx x.xxx x.xxxx x
		case 1 :
			sprintf( Buf , "%.2f" , data );
			break;
		case 2 :
			sprintf( Buf , "%.3f" , data );
			break;
		case 3 :
			sprintf( Buf , "%.4f" , data );
			break;
		case 4 :
			sprintf( Buf , "%.0f" , data );
			break;
		default :
			sprintf( Buf , "%.1f" , data );
			break;
		}
		break;
	}
}


void JOBDATA_S_DISPLAY_CONTROL( HWND hWnd , int index0 , int PrcsStepData , int SubID , int *mindexdata ) {
	int index , rc , i , j , jx , clm;
	char message[256];
	char Buf1[256];
	char Buf2[256];
	char Buf3[256];
	char Buf4[256];
	char Buf5[256];
	char Buf6[256];
	//
	char Buf11[256];
	char Buf12[256];
	char Buf13[256];

	char pmorg[256];
	//
	char folderstr[256];

	double avr , var;
	//
	if      ( index0 == -999 ) {
		index = index0;
	}
	else if ( index0 == -100 ) {
		//
		index = PROCESSDATA_STEPVIEW_LAST_INDEX;
		//
		if ( index < 0 ) return;
		//
		PrcsStepData = PROCESSDATA_STEPVIEW_PRCS;
		//
		SubID = PROCESSDATA_STEPVIEW_SUB;
		//
	}
	else if ( index0 == -99 ) {
		//
		if ( PROCESSDATA_STEPVIEW_INDEX == -1 ) {
			//
			if ( !IsWindowVisible( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) ) ) {
				//
				KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_SPMS );
				KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_SPM );
				//
				KWIN_Item_Resize( hWnd , IDC_CTRL_SPM ,
					Global_Pos_X[BUTTON_SPM] ,
					Global_Pos_Y[BUTTON_SPM] ,
					Global_Pos_XS[BUTTON_SPM] ,
					Global_Pos_YS[BUTTON_SPM] ); // View Area
				//
				KWIN_Item_String_Display(  hWnd , IDC_CTRL_SPM , ( STEPVIEW_DATA.SV_Opt_Button_Char1[0] == 0 ) ? "View" : STEPVIEW_DATA.SV_Opt_Button_Char1 );
				KWIN_Item_Show(  hWnd , IDC_CTRL_SPM );
				IDC_CTRL_SPM_MODE = 1;
				//
				//-------------------------------------------------------------------
				//
				KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_S , Global_Pos_X[BUTTON_DATA_S] , Global_Pos_Y[BUTTON_DATA_S] , Global_Pos_XS[BUTTON_DATA_S] , Global_Pos_YS[BUTTON_DATA_S] ); // View Area

				KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S );
				//				
				KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_SS, Global_Pos_X[BUTTON_DATA_Sstr] , Global_Pos_Y[BUTTON_DATA_Sstr] , Global_Pos_XS[BUTTON_DATA_Sstr] - Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
				//
				KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SS );
				//
				KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2 );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2S );
				//
			}
			else {
				//
				KWIN_Item_Resize( hWnd , IDC_CTRL_SPM ,
					Global_Pos_X[BUTTON_SPM] ,
					Global_Pos_Y[BUTTON_SPM] ,
					Global_Pos_XS[BUTTON_SPM] ,
					Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
					//
				KWIN_Item_String_Display(  hWnd , IDC_CTRL_SPM , ( STEPVIEW_DATA.SV_Opt_Button_Char2[0] == 0 ) ? "View" : STEPVIEW_DATA.SV_Opt_Button_Char2 );
				KWIN_Item_Show(  hWnd , IDC_CTRL_SPM );	// 2016.08.17
				IDC_CTRL_SPM_MODE = 1;
				//
				KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_S , Global_Pos_X[BUTTON_DATA_S] , Global_Pos_Y[BUTTON_DATA_S] , Global_Pos_XS[BUTTON_DATA_S] , Global_Pos_YS[BUTTON_DATA_S] ); // View Area
				//
				KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_S );
				//				
				KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_SS, Global_Pos_X[BUTTON_DATA_Sstr] , Global_Pos_Y[BUTTON_DATA_Sstr] , Global_Pos_XS[BUTTON_DATA_Sstr] - Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
				
				KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_SS );
				//
				KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2 );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2S );
				//
				KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPM );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPMS );
				//
			}
			//
			return;
		}
		else {
			//
			index = PROCESSDATA_STEPVIEW_INDEX;
			//
		}
	}
	else {
		index = index0;
	}
	//
	if ( ( index >= 0 ) && ( PrcsStepData >= 0 ) ) { // 2016.08.17
		//
		JOBDATA_SPMS_INIT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) );
		//
		//--------------------------------------------------------------------------------------------------------------------
		//
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 0 , 0 , "<JOB NAME>" , JOB_DATA[ index ].JOB_NAME );
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 1 , 0 , "<LOT RECIPE>" , JOB_DATA[ index ].LOT_RECIPE );
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 2 , 0 , "<MID NAME>" , JOB_DATA[ index ].MID_NAME );
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 3 , 0 , "" , "" );
		//
		Log_Get_ExtractTime_from_Time( JOB_DATA[ index ].START_TIME , Buf11 );
		Log_Get_ExtractTime_from_Time( JOB_DATA[ index ].RUN_TIME , Buf12 );
		Log_Get_ExtractTime_from_Time( JOB_DATA[ index ].END_TIME , Buf13 );
		//
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 4 , 0 , "      [START]" , Buf11 );
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 5 , 0 , "      [RUN]" , Buf12 );
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 6 , 0 , "      [END]" , Buf13 );
		//
		sprintf( Buf1 , "%d-%d (%d)" , JOB_DATA[ index ].START_SLOT , JOB_DATA[index].END_SLOT , JOB_DATA[index].METHOD_COUNT );
		//
		JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 7 , 0 , "      [SLOT]" , Buf1 );
		//
		//--------------------------------------------------------------------------------------------------------------------
		if ( TreeData_Get_Select_Slot_PM_Info( index , SubID , Buf1 , Buf2 , pmorg ) ) {
			//
			JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 0 , 1 , "PM" , Buf2 );
			JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 1 , 1 , "ID" , Buf1 );
			//
			if ( PrcsStepData == 0 ) {
				//
				JOB_Get_Directory( index , folderstr );
				//----------------------------------------------------------------------------------------------
				Get_Log_File_Name( message , "PROCESS" , 0 , folderstr , TRUE , 0 , 0 , Buf1 , 0 , pmorg , "" );
				//----------------------------------------------------------------------------------------------
				//
				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 3 , 1 , "FOLDER" , folderstr );
				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 4 , 1 , "FileName" , message );
			}
			else {
				//
				Log_Get_Step_String( PrcsStepData - 1 , Buf1 );
				//
				Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_StartTime[PrcsStepData-1] ) , Buf11 );
				Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_EndTime[PrcsStepData-1] ) , Buf12 );
				//
				Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time[PrcsStepData-1] , Buf2 );	// Actual
				Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time[PrcsStepData-1] - StepView_Time_Step_AllItem[PrcsStepData-1] , Buf3 ); // Stable
				Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time_Step_AllItem[PrcsStepData-1] , Buf4 ); // Step

				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 2 , 1 , "STEP" , Buf1 );
				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 3 , 1 , "      [Start]" , Buf11 );
				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 4 , 1 , "      [End]" , Buf12 );
				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 5 , 1 , "      [Actual]" , Buf2 );
				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 6 , 1 , "      [Stable]" , Buf3 );
				JOBDATA_SPMS_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPMS ) , 7 , 1 , "      [Step]" , Buf4 );
				//
			}
			//
		}
		//
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------
		//
		rc = 0;
		//
		//---------------------------------------------
		//
		if ( PrcsStepData == 0 ) {
			//
			JOBDATA_SPM_STEP_INIT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) );
			//
			Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_Run_StartTime ) , Buf11 );
			Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_Run_EndTime ) , Buf12 );
			//
			JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 1 , "Total Start Time" , Buf11 , NULL , NULL , NULL , NULL );	rc++;
			JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 1 , "Total End Time"   , Buf12 , NULL , NULL , NULL , NULL );	rc++;
			//
			Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_Run_EndTime ) , STR_MEM_GET_STR( StepView_Run_StartTime ) , 0 , Buf1 ); // 2016.08.23
			//
			JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 1 , "Total Run Time"   , Buf1 , NULL , NULL , NULL , NULL );	rc++;
			//
			//
			JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 0 , "" , NULL , NULL , NULL , NULL , NULL );	rc++;
			//
			//---------------------------------------------
			//
			clm = 0;
			//
			JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 2 , "Step" , NULL , NULL , NULL , NULL , NULL );	clm++;
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStartTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Start Time" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HideEndTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "End Time" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HideActualTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Actual Time" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStableTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Stabilizing Time" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStepTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Step Time" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HideSetTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Recipe SetTime" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HideResult == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Result" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			//
			rc++;
			//
			//-----------------------------
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HidePrePost == 0 ) {
				//
				if ( STR_MEM_SIZE( StepView_Pre_StartTime ) != 0 ) {
					//
					Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_Pre_EndTime ) , STR_MEM_GET_STR( StepView_Pre_StartTime ) , 0 , Buf1 );
					//
					clm = 0;
					//
					JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 3 , "PRE" , NULL , NULL , NULL , NULL , NULL );	clm++;
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStartTime == 0 ) {
						//
						Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_Pre_StartTime ) , Buf11 );
						//
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , STR_MEM_GET_STR( StepView_Pre_StartTime ) , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideEndTime == 0 ) {
						//
						Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_Pre_EndTime ) , Buf12 );
						//
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , STR_MEM_GET_STR( StepView_Pre_EndTime ) , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideActualTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStableTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStepTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideSetTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideResult == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					rc++;
					//
				}
				//
			}
			//
			//-----------------------------
			//
			for ( i = 0 ; i < Step_Log_Step_Count ; i++ ) {
				//
				Log_Get_Step_String( i , Buf1 );
				//
				clm = 0;
				//
				JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 4 , Buf1 , NULL , NULL , NULL , NULL , NULL );	clm++;
				//
				Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time[i] , Buf2 );
//				if ( ( StepView_Time[i] - StepView_Time_Step_AllItem[i] ) <= 0 ) { // 2016.10.31
//					strcpy( Buf3 , "" );
//					strcpy( Buf4 , "" );
//				}
//				else {
					Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time[i] - StepView_Time_Step_AllItem[i] , Buf3 );
					Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time_Step_AllItem[i] , Buf4 );
//				}

				//
				if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStartTime == 0 ) {
					//
					Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_StartTime[i] ) , Buf11 );
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf11 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Step_View_HideEndTime == 0 ) {
					//
					Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_EndTime[i] ) , Buf12 );
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf12 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Step_View_HideActualTime == 0 ) {
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf2 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStableTime == 0 ) {
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf3 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStepTime == 0 ) {
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf4 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Step_View_HideSetTime == 0 ) {
					//
					if ( StringOnlyNo( STR_MEM_GET_STR( StepView_SetTime[i] ) ) ) {
						Log_Get_DiffTime_from_Time( NULL , NULL , atof(STR_MEM_GET_STR( StepView_SetTime[i] )) , Buf5 );
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf5 , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					else {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , STR_MEM_GET_STR( StepView_SetTime[i] ) , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Step_View_HideResult == 0 ) {
					if ( STR_MEM_GET_STR( StepView_Result[i] )[0] != 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , STR_MEM_GET_STR( StepView_Result[i] ) , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					else {
						//
						if ( STR_MEM_GET_STR( StepView_SetTime[i] )[0] != 0 ) {
							if      ( StepView_Time_Step_AllItem[i] > ( atof(STR_MEM_GET_STR( StepView_SetTime[i] )) + 1 ) ) {
								JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "NG" , NULL , NULL , NULL , NULL , NULL );	clm++;
							}
							else if ( StepView_Time_Step_AllItem[i] < ( atof(STR_MEM_GET_STR( StepView_SetTime[i] )) - 1 ) ) {
								JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "NG" , NULL , NULL , NULL , NULL , NULL );	clm++;
							}
							else {
								JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "OK" , NULL , NULL , NULL , NULL , NULL );	clm++;
							}
						}
						else {
							JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
						}
					}
				}
				//
				rc++;
				//
			}
			//
			if ( STEPVIEW_DATA.SV_Opt_Step_View_HidePrePost == 0 ) {
				//
				if ( STR_MEM_SIZE( StepView_Post_StartTime ) != 0 ) {
					//
					Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_Post_EndTime ) , STR_MEM_GET_STR( StepView_Post_StartTime ) , 0 , Buf1 );
					//
					clm = 0;
					//
					JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 5 , "POST" , NULL , NULL , NULL , NULL , NULL );	clm++;
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStartTime == 0 ) {
						//
						Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_Post_StartTime ) , Buf11 );
						//
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf11 , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideEndTime == 0 ) {
						//
						Log_Get_ExtractTime_from_Time( STR_MEM_GET_STR( StepView_Post_EndTime ) , Buf12 );
						//
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf12 , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideActualTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStableTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideStepTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideSetTime == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					if ( STEPVIEW_DATA.SV_Opt_Step_View_HideResult == 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "" , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
					//
					rc++;
					//
				}
				//
			}
			//
			//---------------------------------------------
			//
		}
		else {
			//
			JOBDATA_SPM_ITEM_INIT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) );
			//
			i = PrcsStepData - 1;
			//
			clm = 0;
			//
			JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 6 , "No" , "Item" , NULL , NULL , NULL , NULL );	clm++;	clm++;
			//
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideCount == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Count" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideTarget == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Target" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStart == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Start" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideEnd == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "End" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMin == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Min" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMax == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Max" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideAvr == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "Avr" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStdDev == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "StdDev" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStblTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "StableTime" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStepTime == 0 ) {
				JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "StepTime" , NULL , NULL , NULL , NULL , NULL );	clm++;
			}
			if ( STEPVIEW_DATA.SV_Opt_Item_View_HideResult == 0 ) {
				//
				if ( STR_MEM_SIZE( StepView_User_Title ) != 0 ) {
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , STR_MEM_GET_STR( StepView_User_Title ) , NULL , NULL , NULL , NULL , NULL );
				}
				else {
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , "-" , NULL , NULL , NULL , NULL , NULL );
				}
				//
				clm++;
				//
			}
			//
			rc++;
			//
			//-------------------------------------------------------------
			//
			for ( j = 0 ; j < Step_Log_Item_Count ; j++ ) {
				//
				sprintf( Buf1 , "%d" , j + 1 );
				sprintf( Buf2 , "%s" , STR_MEM_GET_STR( Step_Log_Item_Name[j] ) );
				//
				//
				clm = 0;
				//
				JOBDATA_SPM_INSERT( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , 7 , Buf1 , Buf2 , NULL , NULL , NULL , NULL );	clm++;	clm++;
				//
				//
				jx = Step_Log_Item_Index[j];
				//
				//
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideCount == 0 ) {
					//
					if ( jx < 0 ) {
						//
						sprintf( Buf3 , "%d" , Dat_Count_Sum_Step[i][j] );
						//
					}
					else {
						//
						sprintf( Buf3 , "%d" , StepView_Count_Step[i][jx] );
						//
					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf3 , NULL , NULL , NULL , NULL , NULL );	clm++;
					//
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideTarget == 0 ) {
					if ( jx < 0 ) {
						strcpy(  Buf4 , "-" );
					}
					else {
						if ( StepView_HasTarget[i][jx] ) {
							StringConvData( Buf4 , StepView_TargetData[i][jx] );
						}
						else {
							strcpy(  Buf4 , "-" );
						}
					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf4 , NULL , NULL , NULL , NULL , NULL );	clm++;
					//
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStart == 0 ) {
					//
					if ( Dat_Count_Sum_Step[i][j] > 0 ) {
						StringConvData( Buf5 , Dat_First_Step[i][j] );
					}
					else {
						strcpy(  Buf5 , "-" );
					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf5 , NULL , NULL , NULL , NULL , NULL );	clm++;
					//
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideEnd == 0 ) {
					//
					if ( Dat_Count_Sum_Step[i][j] > 0 ) {
						StringConvData( Buf6 , Dat_Last_Step[i][j] );
					}
					else {
						strcpy(  Buf6 , "-" );
					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf6 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMin == 0 ) {
					//
					if ( Dat_Count_MinMax_Step[i][j] > 0 ) {
						StringConvData( Buf1 , Dat_Min_Step[i][j] );
					}
					else {
						strcpy(  Buf1 , "-" );
					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf1 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideMax == 0 ) {
					//
					if ( Dat_Count_MinMax_Step[i][j] > 0 ) {
						StringConvData( Buf2 , Dat_Max_Step[i][j] );
					}
					else {
						strcpy(  Buf2 , "-" );
					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf2 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( Dat_Count_Sum_Step[i][j] > 0 ) {
					//
					STEP_Get_AVR_VAR_Value( Dat_Count_Sum_Step[i][j] , Dat_Sum_Step[i][j] , Dat_Sum2_Step[i][j] , &avr , &var );
					//
					StringConvData( Buf3 , avr );
					StringConvData( Buf4 , var );
				}
				else {
					strcpy(  Buf3 , "-" );
					strcpy(  Buf4 , "-" );
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideAvr == 0 ) {
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf3 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStdDev == 0 ) {
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf4 , NULL , NULL , NULL , NULL , NULL );	clm++;
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStblTime == 0 ) {
					//
					if ( jx < 0 ) {
						if ( STEPVIEW_DATA.SV_Opt_TimeDisplay_Unknown == 1 ) {
							Log_Get_DiffTime_from_Time( NULL , NULL , 0 , Buf5 );
						}
						else {
							strcpy( Buf5 , "" );
						}
					}
					else {
						//
						if ( ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 0 ) || ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 1 ) || ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 2 ) ) {
							//
							if ( ( StepView_TimeStable[i][jx] == NULL ) || ( StepView_TimeStable[i][jx][0] == 0 ) ) {
								//
								if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 0 ) {
									Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_EndTime[i] ) , STR_MEM_GET_STR( StepView_StartTime[i] ) , StepView_Time_Step[i][jx] , Buf5 );
								}
								else if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 1 ) {
									Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_EndTime[i] ) , STR_MEM_GET_STR( StepView_StartTime[i] ) , StepView_Time_Step[i][jx] , Buf5 );
								}
								else {
									if ( STEPVIEW_DATA.SV_Opt_TimeDisplay_Unknown == 1 ) {
										Log_Get_DiffTime_from_Time( NULL , NULL , 0 , Buf5 );
									}
									else {
										strcpy( Buf5 , "" );
									}
								}
								//
							}
							else {
								//
								if ( StringOnlyNo( STR_MEM_GET_STR( StepView_TimeStable[i][jx] ) ) ) {
									Log_Get_DiffTime_from_Time( NULL , NULL , atof(STR_MEM_GET_STR( StepView_TimeStable[i][jx] )) , Buf5 );
								}
								else {
									strcpy( Buf5 , STR_MEM_GET_STR( StepView_TimeStable[i][jx] ) );
								}
								//
							}
							//
						}
						else if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 3 ) {
							Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_EndTime[i] ) , STR_MEM_GET_STR( StepView_StartTime[i] ) , StepView_Time_Step[i][jx] , Buf5 );
						}
						else if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 4 ) {
							Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_EndTime[i] ) , STR_MEM_GET_STR( StepView_StartTime[i] ) , StepView_Time_Step[i][jx] , Buf5 );
						}
						else {
							if ( STEPVIEW_DATA.SV_Opt_TimeDisplay_Unknown == 1 ) {
								Log_Get_DiffTime_from_Time( NULL , NULL , 0 , Buf5 );
							}
							else {
								strcpy( Buf5 , "" );
							}
						}

					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf5 , NULL , NULL , NULL , NULL , NULL );	clm++;
					//
				}
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideStepTime == 0 ) {
					//
					if ( jx < 0 ) {
						Log_Get_DiffTime_from_Time( STR_MEM_GET_STR( StepView_EndTime[i] ) , STR_MEM_GET_STR( StepView_StartTime[i] ) , 0 , Buf6 );
					}
					else {
						//
						if ( ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 0 ) || ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 1 ) || ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 2 ) ) {
							//
//							if ( ( StepView_TimeStable[i][jx] == NULL ) || ( StepView_TimeStable[i][jx][0] == 0 ) ) { // 2018.04.24
							if ( ( StepView_TimeStep[i][jx] == NULL ) || ( StepView_TimeStep[i][jx][0] == 0 ) ) { // 2018.04.24
								//
								if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 0 ) {
									Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time_Step[i][jx] , Buf6 );
								}
								else if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 1 ) {
									Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time_Step_AllItem[i] , Buf6 );
								}
								else {
									if ( STEPVIEW_DATA.SV_Opt_TimeDisplay_Unknown == 1 ) {
										Log_Get_DiffTime_from_Time( NULL , NULL , 0 , Buf6 );
									}
									else {
										strcpy( Buf6 , "" );
									}
								}
								//
							}
							else {
								//
								if ( StringOnlyNo( STR_MEM_GET_STR( StepView_TimeStep[i][jx] ) ) ) {
									Log_Get_DiffTime_from_Time( NULL , NULL , atof(STR_MEM_GET_STR( StepView_TimeStep[i][jx] )) , Buf6 );
								}
								else {
									strcpy( Buf6 , STR_MEM_GET_STR( StepView_TimeStep[i][jx] ) );
								}
								//
							}
							//
						}
						else if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 3 ) {
							Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time_Step[i][jx] , Buf6 );
						}
						else if ( STEPVIEW_DATA.SV_Opt_Item_Time_Calculation == 4 ) {
							Log_Get_DiffTime_from_Time( NULL , NULL , StepView_Time_Step_AllItem[i] , Buf6 );
						}
						else {
							if ( STEPVIEW_DATA.SV_Opt_TimeDisplay_Unknown == 1 ) {
								Log_Get_DiffTime_from_Time( NULL , NULL , 0 , Buf6 );
							}
							else {
								strcpy( Buf6 , "" );
							}
						}

					}
					//
					JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , Buf6 , NULL , NULL , NULL , NULL , NULL );	clm++;
					//
				}
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_View_HideResult == 0 ) {
					//
					if ( jx >= 0 ) {
						JOBDATA_SPM_UPDATE( GetDlgItem( hWnd , IDC_LIST_JOBDATA_SPM ) , rc , clm , STR_MEM_GET_STR( StepView_UserData[i][jx] ) , NULL , NULL , NULL , NULL , NULL );	clm++;
					}
				}
				//
				rc++;
				//
			}
			//
			//-------------------------------------------------------------
			//
		}
		//
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------
		//
		if ( index0 != -100 ) {
			//
			KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_SPMS );
			KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_SPM );
			//
			KWIN_Item_Resize( hWnd , IDC_CTRL_SPM ,
				Global_Pos_X[BUTTON_SPM] ,
				Global_Pos_Y[BUTTON_SPM] ,
				Global_Pos_XS[BUTTON_SPM] ,
				Global_Pos_YS[BUTTON_SPM] ); // View Area
			//
			KWIN_Item_String_Display(  hWnd , IDC_CTRL_SPM , ( STEPVIEW_DATA.SV_Opt_Button_Char1[0] == 0 ) ? "View" : STEPVIEW_DATA.SV_Opt_Button_Char1 );
			KWIN_Item_Show(  hWnd , IDC_CTRL_SPM );
			IDC_CTRL_SPM_MODE = 1;
			//
			//-------------------------------------------------------------------
			//
			KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_S , Global_Pos_X[BUTTON_DATA_S] , Global_Pos_Y[BUTTON_DATA_S] , Global_Pos_XS[BUTTON_DATA_S] , Global_Pos_YS[BUTTON_DATA_S] ); // View Area

			KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SS );
			//
			KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2 );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2S );
			//
			if ( Get_Prm_STATUS_VIEW() ) KWIN_Item_String_Display( hWnd , IDC_LIST_JOBDATA_SPS , "" );
			//
			KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA );
			//
			PROCESSDATA_STEPVIEW_INDEX = index;
			PROCESSDATA_STEPVIEW_PRCS = PrcsStepData;
			PROCESSDATA_STEPVIEW_SUB = SubID;
			//
			PROCESSDATA_STEPVIEW_LAST_INDEX = index;
			//
		}
		//
		return;
	}
	//
	if ( index == -999 ) {
		//
		sprintf( message , " MULTI VIEW %d JOB(s)" , PrcsStepData );
		//
		rc = strlen( message );
		//
		for ( i = 0 ; i < PrcsStepData ; i++ ) {
			//
			sprintf( Buf1 , " (J%d:%s)" , mindexdata[i] + 1 , JOB_DATA[ mindexdata[i] ].JOB_NAME );
			//
			j = strlen( Buf1 );
			//
			rc = rc + j;
			//
			if ( rc > 250 ) {
				strcat( message , "..." );
				break;
			}
			//
			strcat( message , Buf1 );
			//
		}
		//
		KWIN_Item_String_Display(  hWnd , IDC_LIST_JOBDATA_SS , message );
		//
		KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_SS, Global_Pos_X[BUTTON_DATA_Sstr] , Global_Pos_Y[BUTTON_DATA_Sstr] , Global_Pos_XS[BUTTON_DATA_Sstr] - Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
		//
		KWIN_Item_Resize( hWnd , IDC_CTRL_SPM ,	Global_Pos_X[BUTTON_SPM] , Global_Pos_Y[BUTTON_SPM] , Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
		//
		KWIN_Item_String_Display(  hWnd , IDC_CTRL_SPM , "View" );
		//
		KWIN_Item_Show(  hWnd , IDC_CTRL_SPM );
		IDC_CTRL_SPM_MODE = 3;
		//
	}
	else if ( index != -1 ) {
		//
		//SendMessage( GetDlgItem( hWnd ,IDC_LIST_JOBDATA_SS ) , EM_SETSEL, 0, -1 );
		//
		switch ( Get_Info_END_TIME_SHOW_MODE() ) {
		case 1 :
			sprintf( message , " <JOB NAME> %s   <LOT RECIPE> %s   <MID NAME> %s\r\n [START: %s]  [RUN: %s]  [ELAPSED: %s]  [SLOT: %d-%d(%d)]" ,
				JOB_DATA[ index ].JOB_NAME ,
				JOB_DATA[ index ].LOT_RECIPE ,
				JOB_DATA[ index ].MID_NAME ,
				JOB_DATA[ index ].START_TIME ,
				JOB_DATA[ index ].RUN_TIME ,
				JOB_DATA[ index ].ELAPSED_TIME ,
				JOB_DATA[ index ].START_SLOT , JOB_DATA[index].END_SLOT , JOB_DATA[index].METHOD_COUNT 
				);
			break;
		case 2 :
			Get_Elapsed_String( index , Buf1 , FALSE );
			sprintf( message , " <JOB NAME> %s   <LOT RECIPE> %s   <MID NAME> %s\r\n [START: %s]  [RUN: %s]  [ELAPSED: %s]  [SLOT: %d-%d(%d)]" ,
				JOB_DATA[ index ].JOB_NAME ,
				JOB_DATA[ index ].LOT_RECIPE ,
				JOB_DATA[ index ].MID_NAME ,
				JOB_DATA[ index ].START_TIME ,
				JOB_DATA[ index ].RUN_TIME ,
				Buf1 ,
				JOB_DATA[ index ].START_SLOT , JOB_DATA[index].END_SLOT , JOB_DATA[index].METHOD_COUNT 
				);
			break;
		default :
			sprintf( message , " <JOB NAME> %s   <LOT RECIPE> %s   <MID NAME> %s\r\n [START: %s]  [RUN: %s]  [END: %s]  [SLOT: %d-%d(%d)]" ,
				JOB_DATA[ index ].JOB_NAME ,
				JOB_DATA[ index ].LOT_RECIPE ,
				JOB_DATA[ index ].MID_NAME ,
				JOB_DATA[ index ].START_TIME ,
				JOB_DATA[ index ].RUN_TIME ,
				JOB_DATA[ index ].END_TIME ,
				JOB_DATA[ index ].START_SLOT , JOB_DATA[index].END_SLOT , JOB_DATA[index].METHOD_COUNT 
				);
			break;
		}
		//
		//SendMessage( GetDlgItem( hWnd ,IDC_LIST_JOBDATA_SS ) , EM_REPLACESEL, (WPARAM) TRUE, (LPARAM) message);

		KWIN_Item_String_Display(  hWnd , IDC_LIST_JOBDATA_SS , message );
		//
		if ( index0 == -99 ) {
			//
			KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_SS, Global_Pos_X[BUTTON_DATA_Sstr] , Global_Pos_Y[BUTTON_DATA_Sstr] , Global_Pos_XS[BUTTON_DATA_Sstr] - Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
			//
			KWIN_Item_Resize( hWnd , IDC_CTRL_SPM ,	Global_Pos_X[BUTTON_SPM] , Global_Pos_Y[BUTTON_SPM] , Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
			//
			KWIN_Item_String_Display(  hWnd , IDC_CTRL_SPM , ( STEPVIEW_DATA.SV_Opt_Button_Char2[0] == 0 ) ? "View" : STEPVIEW_DATA.SV_Opt_Button_Char2 );
			KWIN_Item_Show(  hWnd , IDC_CTRL_SPM );	// 2016.08.17
			IDC_CTRL_SPM_MODE = 1;
			//
		}
		else {
			//
			if ( SubID == 1 ) {
				//
				KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_SS, Global_Pos_X[BUTTON_DATA_Sstr] , Global_Pos_Y[BUTTON_DATA_Sstr] , Global_Pos_XS[BUTTON_DATA_Sstr] - Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
				//
				KWIN_Item_Resize( hWnd , IDC_CTRL_SPM ,	Global_Pos_X[BUTTON_SPM] , Global_Pos_Y[BUTTON_SPM] , Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
				//
				KWIN_Item_String_Display(  hWnd , IDC_CTRL_SPM , "View" );
				//
				KWIN_Item_Show(  hWnd , IDC_CTRL_SPM );
				IDC_CTRL_SPM_MODE = 2;
				//
			}
			else {
				//
				KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_SS, Global_Pos_X[BUTTON_DATA_Sstr] , Global_Pos_Y[BUTTON_DATA_Sstr] , Global_Pos_XS[BUTTON_DATA_Sstr] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
				//
				KWIN_Item_Hide(  hWnd , IDC_CTRL_SPM );	// 2016.08.17
				IDC_CTRL_SPM_MODE = 0;
				//
			}
		}
	}
	//
	KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_S , Global_Pos_X[BUTTON_DATA_S] , Global_Pos_Y[BUTTON_DATA_S] , Global_Pos_XS[BUTTON_DATA_S] , Global_Pos_YS[BUTTON_DATA_S] ); // View Area
	//
	KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_S );
	//
	KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_SS );
	//
	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2 );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S2S );
	//
	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPM );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPMS );
	//
	//
//	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPS ); // 2014.02.10 // 2014.07.17
	if ( Get_Prm_STATUS_VIEW() ) KWIN_Item_String_Display( hWnd , IDC_LIST_JOBDATA_SPS , "" ); // 2014.07.17
	//
	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA );
	//
	PROCESSDATA_STEPVIEW_INDEX = -1;
	//
}

void JOBDATA_S2_DISPLAY_CONTROL( HWND hWnd , int wParam , int from , char *message , char *data , char *data2 ) {
	//
	KWIN_Item_Resize( hWnd , IDC_LIST_JOBDATA_S2, Global_Pos_X[BUTTON_DATA_S2] , Global_Pos_Y[BUTTON_DATA_S2] , Global_Pos_XS[BUTTON_DATA_S2] , Global_Pos_YS[BUTTON_DATA_S2] ); // View Area
	//
	KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_S2 );	KWIN_Item_Show(  hWnd , IDC_LIST_JOBDATA_S2S );
	//
	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_S );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SS );
	//
	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPM );	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPMS ); 	KWIN_Item_Hide(  hWnd , IDC_CTRL_SPM );	// 2016.08.17
	//
	IDC_CTRL_SPM_MODE = 0;

	//
//	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA_SPS ); // 2014.02.10 // 2014.07.17
	if ( Get_Prm_STATUS_VIEW() ) KWIN_Item_String_Display( hWnd , IDC_LIST_JOBDATA_SPS , "" ); // 2014.07.17
	//
	KWIN_Item_Hide(  hWnd , IDC_LIST_JOBDATA );
	//
	IDC_CONTROL_S2_AREA_LAST_OPEN = wParam;
	//
	if ( from >= 0 ) {
		//
		KWIN_Item_String_Display(  hWnd , IDC_LIST_JOBDATA_S2S , message );
		//
		IDC_CONTROL_S2_FROM_LAST_OPEN = from;
		//
		strcpy( IDC_CONTROL_S2_AREA_LAST_DATA , data );
		strcpy( IDC_CONTROL_S2_AREA_LAST_MODULE , data2 ); // 2016.12.22
		//
	}
	//
}


//
int    CURRENT_WAFER_Data_Set_Min_Dev = 100; // 2012.11.26
//
BOOL   CURRENT_WAFER_Data_Set_MultiMode;
int    CURRENT_WAFER_Data_Set_Min_Mode; // 2012.11.26
int    CURRENT_WAFER_Data_Set_Min_Index; // 2012.11.26
int    CURRENT_WAFER_Data_Set_View_Mode = 0; // TimeLine / SameLine 2017.03.17


int  CURRENT_WAFER_VIEW_COUNT = 0;
int  CURRENT_WAFER_JOBINDEX[MAX_CURRENT_WAFER_VIEW];
int  CURRENT_WAFER_MINDEX[MAX_CURRENT_WAFER_VIEW];
int  CURRENT_WAFER_ID[MAX_CURRENT_WAFER_VIEW];
int  CURRENT_WAFER_DUMMYIN[MAX_CURRENT_WAFER_VIEW]; // 2013.12.05
int  CURRENT_WAFER_ALARM_COUNT[MAX_CURRENT_WAFER_VIEW];

char *CURRENT_WAFER_NAME[MAX_CURRENT_WAFER_VIEW];
int   CURRENT_WAFER_MODULE_DEPTH[MAX_CURRENT_WAFER_VIEW];
int   CURRENT_WAFER_MODULE_TM[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH];
int   CURRENT_WAFER_MODULE_MODE[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH];


double CURRENT_WAFER_Data_M_Min_Sec[MAX_MULTI_CHECK_JOB]; // 2012.11.26
double CURRENT_WAFER_Data_M_Max_Sec[MAX_MULTI_CHECK_JOB]; // 2012.11.26
double CURRENT_WAFER_Data_1_Min_Sec; // 2012.11.26
double CURRENT_WAFER_Data_1_Max_Sec; // 2012.11.26


double CURRENT_WAFER_Get_Min_Sec( int currview ) {
	int x;
	//
	if ( CURRENT_WAFER_Data_Set_View_Mode == 0 ) return CURRENT_WAFER_Data_1_Min_Sec;
	//
	x = CURRENT_WAFER_MINDEX[currview];
	//
	if ( x < 0 ) {
		return CURRENT_WAFER_Data_1_Min_Sec;
	}
	else {
		return CURRENT_WAFER_Data_M_Min_Sec[x];
	}
	//
}

double CURRENT_WAFER_Get_Max_Sec( int currview ) {
	int x;
	//
	if ( CURRENT_WAFER_Data_Set_View_Mode == 0 ) return CURRENT_WAFER_Data_1_Max_Sec;
	//
	x = CURRENT_WAFER_MINDEX[currview];
	//
	if ( x < 0 ) {
		return CURRENT_WAFER_Data_1_Max_Sec;
	}
	else {
		return CURRENT_WAFER_Data_M_Max_Sec[x];
	}
}

void CURRENT_WAFER_Set_MinMax_Sec( int mindex , double data ) {
	//
	if ( CURRENT_WAFER_Data_1_Min_Sec <= 0 ) {
		CURRENT_WAFER_Data_1_Min_Sec = data;
	}
	else {
		if ( CURRENT_WAFER_Data_1_Min_Sec > data ) {
			CURRENT_WAFER_Data_1_Min_Sec = data;
		}
	}
	//
	if ( CURRENT_WAFER_Data_1_Max_Sec <= 0 ) {
		CURRENT_WAFER_Data_1_Max_Sec = data;
	}
	else {
		if ( CURRENT_WAFER_Data_1_Max_Sec < data ) {
			CURRENT_WAFER_Data_1_Max_Sec = data;
		}
	}
	//
	if ( mindex != -1 ) {
		//
		if ( CURRENT_WAFER_Data_M_Min_Sec[mindex] <= 0 ) {
			CURRENT_WAFER_Data_M_Min_Sec[mindex] = data;
		}
		else {
			if ( CURRENT_WAFER_Data_M_Min_Sec[mindex] > data ) {
				CURRENT_WAFER_Data_M_Min_Sec[mindex] = data;
			}
		}
		//
		if ( CURRENT_WAFER_Data_M_Max_Sec[mindex] <= 0 ) {
			CURRENT_WAFER_Data_M_Max_Sec[mindex] = data;
		}
		else {
			if ( CURRENT_WAFER_Data_M_Max_Sec[mindex] < data ) {
				CURRENT_WAFER_Data_M_Max_Sec[mindex] = data;
			}
		}
		//
	}
}



int  CURRENT_WAFER_VIEW2;
char *CURRENT_WAFER_ID2;
char *CURRENT_WAFER_NAME2;
char *CURRENT_WAFER_NAME2_ORG;

//
//0 = PRCS
//1 = PRCS(?)
//2 = IN
//3 =       PLACE
//4 =       PICK
//5 =       PLACE(BM)
//6 =       PICK(BM)
//7 = OUT

char *CURRENT_WAFER_MODULE_NAME[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH];
double CURRENT_WAFER_MODULE_START[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH]; // 2012.11.26
double CURRENT_WAFER_MODULE_END[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_DEPTH]; // 2012.11.26

int   CURRENT_MODULE_VIEW_COUNT = 0;
char *CURRENT_MODULE_VIEW_NAME[MAX_CURRENT_WAFER_VIEW];
int   CURRENT_MODULE_VIEW_DEPTH[MAX_CURRENT_WAFER_VIEW];
int   CURRENT_MODULE_VIEW_MINDEX[MAX_CURRENT_WAFER_VIEW];
int   CURRENT_MODULE_VIEW_1[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_VIEW];
int   CURRENT_MODULE_VIEW_2[MAX_CURRENT_WAFER_VIEW][MAX_CURRENT_WAFER_VIEW];



int  CURRENT_WAFER2_ALARM_COUNT = 0;
int  CURRENT_WAFER2_ALARM_WFRID[MAX_CURRENT_WAFER_VIEW];
int  CURRENT_WAFER2_ALARM_HAPPEN[MAX_CURRENT_WAFER_VIEW];

double CURRENT_WAFER_GET_TIME( int mindex , char *data ) { // 2012.11.26
	int ryear , rmonth , rday , rhour , rmin , rsec , rmsec;
	double rst = 0;
	//
	if ( _UTIL_TIME_GET_DATETIME_from_StringMS( data , &ryear , &rmonth , &rday , &rhour , &rmin , &rsec , &rmsec ) == 0 ) {
		if ( _UTIL_TIME_GET_SEC_with_0BasedTimeMS( ryear , rmonth , rday , rhour , rmin , rsec , rmsec , &rst ) == 0 ) {
			//
			CURRENT_WAFER_Set_MinMax_Sec( mindex , rst );
			//
		}
		else {
			rst = -1;
		}
	}
	return rst;
}

void CURRENT_WAFER_INIT( int init ) {
	int i , j;
	//
	CURRENT_WAFER_Data_1_Min_Sec = 0; // 2012.11.26
	CURRENT_WAFER_Data_1_Max_Sec = 0; // 2012.11.26
	//
	for ( i = 0 ; i < MAX_MULTI_CHECK_JOB ; i++ ) {
		CURRENT_WAFER_Data_M_Min_Sec[i] = 0;
		CURRENT_WAFER_Data_M_Max_Sec[i] = 0;
	}
	//
	if ( init != -1 ) {
		//
		CURRENT_WAFER_VIEW_COUNT = 0;
		//
		for ( i = 0 ; i < MAX_CURRENT_WAFER_VIEW ; i++ ) {
			if ( init == 0 ) {
				if ( CURRENT_WAFER_NAME[i] != NULL ) free( CURRENT_WAFER_NAME[i] );
				//
//				if ( CURRENT_WAFER_ID2 != NULL ) free( CURRENT_WAFER_ID2 ); // 2017.05.19
//				if ( CURRENT_WAFER_NAME2 != NULL ) free( CURRENT_WAFER_NAME2 ); // 2017.05.19
				//
			}
			//
			CURRENT_WAFER_NAME[i] = NULL;
			CURRENT_WAFER_MODULE_DEPTH[i] = 0;
			//
//			CURRENT_WAFER_ID2 = NULL; // 2017.03.16
//			CURRENT_WAFER_NAME2 = NULL; // 2017.03.16
			//
			for ( j = 0 ; j < MAX_CURRENT_WAFER_DEPTH ; j++ ) {
				//
				if ( init == 0 ) {
					if ( CURRENT_WAFER_MODULE_NAME[i][j] != NULL ) free( CURRENT_WAFER_MODULE_NAME[i][j] );
				}
				//
				CURRENT_WAFER_MODULE_NAME[i][j] = NULL;
			}
		}
		//
		if ( init == 0 ) { // 2017.05.19
			//
			if ( CURRENT_WAFER_ID2 != NULL ) free( CURRENT_WAFER_ID2 );
			if ( CURRENT_WAFER_NAME2 != NULL ) free( CURRENT_WAFER_NAME2 );
			if ( CURRENT_WAFER_NAME2_ORG != NULL ) free( CURRENT_WAFER_NAME2_ORG );
			//
		}
		//
		CURRENT_WAFER_VIEW2 = FALSE; // 2017.03.16
		CURRENT_WAFER_ID2 = NULL; // 2017.03.16
		CURRENT_WAFER_NAME2 = NULL; // 2017.03.16
		CURRENT_WAFER_NAME2_ORG = NULL; // 2017.03.16
		//
		CURRENT_MODULE_VIEW_COUNT = 0;
		//
		for ( i = 0 ; i < MAX_CURRENT_WAFER_VIEW ; i++ ) {
			//
			CURRENT_MODULE_VIEW_DEPTH[i] = 0;
			//
			if ( init == 0 ) {
				if ( CURRENT_MODULE_VIEW_NAME[i] != NULL ) free( CURRENT_MODULE_VIEW_NAME[i] );
			}
			//
			CURRENT_MODULE_VIEW_NAME[i] = NULL;
		}
		//
	}
	//
}

void CURRENT_WAFER_APPEND_ID2( char *id , char *name , char *nameorg ) {
	//
	CURRENT_WAFER_VIEW2 = TRUE;
	//
	STR_MEM_MAKE_COPY2( &(CURRENT_WAFER_ID2) , id );
	STR_MEM_MAKE_COPY2( &(CURRENT_WAFER_NAME2) , name );
	STR_MEM_MAKE_COPY2( &(CURRENT_WAFER_NAME2_ORG) , nameorg );
	//
}

void CURRENT_WAFER_APPEND_ID( int jobindex , int mindex , int id , char *name , int dummyin ) { // 2013.12.05
	//
	if ( CURRENT_WAFER_VIEW_COUNT >= MAX_CURRENT_WAFER_VIEW ) return;
	//
	CURRENT_WAFER_JOBINDEX[CURRENT_WAFER_VIEW_COUNT] = jobindex;
	CURRENT_WAFER_MINDEX[CURRENT_WAFER_VIEW_COUNT] = mindex;
	CURRENT_WAFER_ID[CURRENT_WAFER_VIEW_COUNT] = id;
	CURRENT_WAFER_DUMMYIN[CURRENT_WAFER_VIEW_COUNT] = dummyin; // 2013.12.05
	//
	STR_MEM_MAKE_COPY2( &(CURRENT_WAFER_NAME[CURRENT_WAFER_VIEW_COUNT]) , name );

	CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT] = 0;
	//
	CURRENT_WAFER_VIEW_COUNT++;
}

void CURRENT_WAFER_APPEND_MODULE( int mode , int tm , char *name , double st , double et ) {
//	int i;
	//
	if ( ( CURRENT_WAFER_VIEW_COUNT - 1 ) < 0 ) return;
	if ( ( CURRENT_WAFER_VIEW_COUNT - 1 ) >= MAX_CURRENT_WAFER_VIEW ) return;
	//
	if ( ( CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT - 1] ) >= MAX_CURRENT_WAFER_DEPTH ) return;
	//
	CURRENT_WAFER_MODULE_MODE[CURRENT_WAFER_VIEW_COUNT-1][CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT - 1]] = mode;
	CURRENT_WAFER_MODULE_TM[CURRENT_WAFER_VIEW_COUNT-1][CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT - 1]] = tm;
	//
	CURRENT_WAFER_MODULE_START[CURRENT_WAFER_VIEW_COUNT-1][CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT - 1]] = st; // 2012.11.26
	CURRENT_WAFER_MODULE_END[CURRENT_WAFER_VIEW_COUNT-1][CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT - 1]] = et; // 2012.11.26
	//
	STR_MEM_MAKE_COPY2( &(CURRENT_WAFER_MODULE_NAME[CURRENT_WAFER_VIEW_COUNT-1][CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT - 1]]) , name );
	//
	CURRENT_WAFER_MODULE_DEPTH[CURRENT_WAFER_VIEW_COUNT - 1]++;
	//
}

int CURRENT_WAFER_GET_COUNT() {
	return CURRENT_WAFER_VIEW_COUNT;
}


int CURRENT_WAFER_APPEND_MODULE_F( int jobindex , int id , int sx , int mode , int tm , char *name , double st , double et ) {
	int i;
	//
	for ( i = sx ; i < CURRENT_WAFER_VIEW_COUNT ; i++ ) {
		if ( CURRENT_WAFER_JOBINDEX[i] == jobindex ) {
			if ( CURRENT_WAFER_ID[i] == id ) {
				break;
			}
		}
	}
	if ( i == CURRENT_WAFER_VIEW_COUNT ) return 1;
	//
	if ( ( CURRENT_WAFER_MODULE_DEPTH[i] ) >= MAX_CURRENT_WAFER_DEPTH ) return 2;
	//
	CURRENT_WAFER_MODULE_MODE[i][CURRENT_WAFER_MODULE_DEPTH[i]] = mode;
	CURRENT_WAFER_MODULE_TM[i][CURRENT_WAFER_MODULE_DEPTH[i]] = tm;
	//
	CURRENT_WAFER_MODULE_START[i][CURRENT_WAFER_MODULE_DEPTH[i]] = st;
	CURRENT_WAFER_MODULE_END[i][CURRENT_WAFER_MODULE_DEPTH[i]] = et;
	//
	STR_MEM_MAKE_COPY2( &(CURRENT_WAFER_MODULE_NAME[i][CURRENT_WAFER_MODULE_DEPTH[i]]) , name );
	//
	CURRENT_WAFER_MODULE_DEPTH[i]++;
	//
	return 0;
	//
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Get_File_Time_String( char *filename ,  char *data ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	SYSTEMTIME sysTime;
	FILETIME fTime;

	hFind = FindFirstFile( filename , &fd );
	if ( hFind == INVALID_HANDLE_VALUE ) {
		strcpy( data , "" );
		return FALSE;
	}
	FileTimeToLocalFileTime( &(fd.ftLastWriteTime) , &fTime );
	FileTimeToSystemTime( &fTime , &sysTime );
	sprintf( data , "%04d/%02d/%02d %02d:%02d:%02d" ,
		sysTime.wYear ,
		sysTime.wMonth ,
		sysTime.wDay ,
		sysTime.wHour ,
		sysTime.wMinute ,
		sysTime.wSecond );
	FindClose( hFind );
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if ( no == -1 ) {
//		EnterCriticalSection( &CR_SC ); // 2003.09.16
//		JOB_W_KILL = TRUE; // 2003.09.16
//		LeaveCriticalSection( &CR_SC ); // 2003.09.16
		KGUI_STANDARD_Window_Kill_Signal();
		KGUI_PALETTE_UNLOADING( NULL );
		KWIN_DIALOG_DRIVER_UNLOADING();
		return;
	}
	if ( no > 0 ) Set_Prm_SIMPLE_DISPLAY( no - 1 );
	KGUI_STANDARD_Screen_Nav_Info( no );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
/*
// 2009.07.20
int	FindData_Conv_ListView_to_JobIndex( int list_index ) {
	return ( JOB_FIND_Get_Real_Index( JOB_FIND_Get_Current_Count() - list_index - 1 ) );
}
*/
//------------------------------------------------------------------------------------------
//int	JobData_Conv_ListView_to_JobIndex( int list_index ) { // 2009.07.20
int	JobData_Conv_ListView_to_JobIndex( HWND hList , int list_index ) { // 2009.07.20
	//--------------------------------------
	// 2009.07.20
	//--------------------------------------
	LV_ITEM lvi;
	int r;
	if ( list_index < 0 ) {
		r = ListView_GetNextItem( hList , -1 , LVNI_SELECTED );
	}
	else {
		r = list_index;
	}
	lvi.mask = LVIF_PARAM;
//	lvi.iItem = r;
//	lvi.iItem = (JOB_Get_Current_Count() - r - 1);// 2011.06.20
	//
	lvi.iItem = r;// 2011.06.20
	ListView_GetItem( hList , &lvi );
	return lvi.lParam / 10000;
//	return (JOB_Get_Current_Count() - (lvi.lParam / 10000) - 1);
	//--------------------------------------
//	return ( JOB_Get_Current_Count() - list_index - 1 ); // 2009.07.20
}



void JobData_ListView_Get_Multi_Index( HWND hList , int *mindex , int *mcount , int max ) {
	LV_ITEM lvi;
	int r;
	//
	r = ListView_GetNextItem( hList , -1 , LVNI_SELECTED );
	//
	if ( r < 0 ) return;
	//
	while ( r >= 0 ) {
		//
		lvi.mask = LVIF_PARAM;
		lvi.iItem = r;
		ListView_GetItem( hList , &lvi );
		//
		if ( *mcount >= max ) return;
		//
		mindex[*mcount] = lvi.lParam / 10000;
		(*mcount)++;
		//
		r = ListView_GetNextItem( hList , r , LVNI_SELECTED );
	}
}

//------------------------------------------------------------------------------------------
//int JobData_ListView_SelectData( HWND hWnd , int no ) { // 2009.07.20
//	return( ListView_GetNextItem( hWnd , no , LVNI_SELECTED ) ); // 2009.07.20
//} // 2009.07.20
//------------------------------------------------------------------------------------------
BOOL TreeData_ListView_DeleteItem( HWND hWnd , int Count ) {
	int i;
	HTREEITEM rhitm;
	HTREEITEM hitm;
	HTREEITEM hitm2;
	//
	if ( Get_Prm_DISPLAY_MODE() < 2 ) return TRUE;
	//
	rhitm = TreeView_GetChild( hWnd , TVI_ROOT );
	//
	if ( rhitm == NULL ) return FALSE;
	//
	for ( i = 0 ; i < Count ; i++ ) {
		//
		hitm = TreeView_GetChild( hWnd , rhitm );
		//
		if ( hitm == NULL ) break;
		//
		if ( !TreeView_DeleteItem( hWnd , hitm ) ) break;
	}
	//
	hitm = TreeView_GetChild( hWnd , rhitm );
	//
	while ( TRUE ) {
		//
		if ( hitm == NULL ) break;
		//
		while ( TRUE ) {
			hitm2 = TreeView_GetChild( hWnd , hitm );
			if ( hitm2 == NULL ) break;
			if ( !TreeView_DeleteItem( hWnd , hitm2 ) ) break;
		}
		//
		hitm = TreeView_GetNextItem( hWnd , hitm , TVGN_NEXTVISIBLE );
		//
	}
	//
	return TRUE;
}
//------------------------------------------------------------------------------------------
BOOL JobData_ListView_DeleteItem( HWND hWnd , HWND hWnd2 , int Count ) {
	int i; // 2011.06.20
	//
	if ( Count <= 0 ) return FALSE;

	for ( i = 0 ; i < Count ; i++ ) { // 2011.06.20
		ListView_DeleteItem( hWnd , 0 );
	}
	//
	TreeData_ListView_DeleteItem( hWnd2 , Count );

	return TRUE;
}
//------------------------------------------------------------------------------------------
BOOL JobData_ListView_SelectMode( HWND hWnd , BOOL one ) {
	int i;
	i = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED ); // 2009.07.20
	if ( i >= 0 ) {
		if ( one ) {
			i = ListView_GetNextItem( hWnd , i , LVNI_SELECTED ); // 2009.07.20
			if ( i >= 0 ) return FALSE;
			else          return TRUE;
		}
		else { // 2011.06.20
			return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL JobData_ListView_DeleteEnable( HWND hWnd ) {
	int i;
//	i = JobData_ListView_SelectData( hWnd , -1 ); // 2009.07.20
	i = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED ); // 2009.07.20
	if ( i < 0 ) return FALSE;
	while ( i >= 0 ) {
//		if ( !JOB_Question_Data_Delete_Enable( JobData_Conv_ListView_to_JobIndex( i ) ) ) return FALSE; // 2009.07.20
		if ( !JOB_Question_Data_Delete_Enable( JobData_Conv_ListView_to_JobIndex( hWnd , i ) ) ) return FALSE; // 2009.07.20
//		i = JobData_ListView_SelectData( hWnd , i ); // 2009.07.20
		i = ListView_GetNextItem( hWnd , i , LVNI_SELECTED ); // 2009.07.20
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
int TreeData_DeleteData( HWND hWnd , HWND hWnd2 , int index , char *data , int *failcount ) {
	int i;
	//
	*failcount = 0;
	//
	strcpy( data , "" );
	//
	JOB_Get_Display_String( index , data );
	//
	if ( !JOB_Make_Delete_Item( index ) ) (*failcount)++;
	//
	i = JOB_DATA_REMAPPING();
	//
	JobData_ListView_DeleteItem( hWnd , hWnd2 , i );
	//
	return i;
}
//------------------------------------------------------------------------------------------
int JobData_ListView_DeleteData( HWND hWnd , HWND hWnd2 , char *data , int *failcount ) {
	int i , index;
	//
	*failcount = 0;
	//
	strcpy( data , "" );
	//
//	i = JobData_ListView_SelectData( hWnd , -1 ); // 2009.07.20
	i = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED ); // 2009.07.20
	//
	while ( i >= 0 ) {
//		index = JobData_Conv_ListView_to_JobIndex( i ); // 2009.07.20
		index = JobData_Conv_ListView_to_JobIndex( hWnd , i ); // 2009.07.20
		if ( index >= 0 ) {
			if ( data[0] == 0 ) {
				JOB_Get_Display_String( index , data );
			}
			//
			if ( !JOB_Make_Delete_Item( index ) ) { // 2003.09.17
				(*failcount)++; // 2010.11.16
			}
			//
		}
//		i = JobData_ListView_SelectData( hWnd , i ); // 2009.07.20
		i = ListView_GetNextItem( hWnd , i , LVNI_SELECTED ); // 2009.07.20
	}
	//
	i = JOB_DATA_REMAPPING();
	//
	JobData_ListView_DeleteItem( hWnd , hWnd2 , i );
	//
	return i;
}
//------------------------------------------------------------------------------------------
BOOL JobData_ListView_Old_DeleteData( HWND hWnd , HWND hWnd2 , int count ) {
	JobData_ListView_DeleteItem( hWnd , hWnd2 , JOB_Delete_Old_Data( count ) );
	return TRUE;
}
//------------------------------------------------------------------------------------------
BOOL JobData_ListView_Insert( HWND hWnd ) {
	LV_ITEM lvi;
	//
	if ( Get_Prm_FIND_MODE() ) return FALSE; // 2011.07.16
	//
//	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;				// 2009.11.18
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;	// 2009.11.18
	lvi.state = lvi.stateMask = 0;
	//
//	lvi.iItem		= JOB_Get_Current_Count() - 1; // 2011.06.20
	lvi.iItem		= 0; // 2011.06.20
	//
	lvi.iSubItem	= 0;
	lvi.pszText		= LPSTR_TEXTCALLBACK;
	lvi.cchTextMax	= 0;
	lvi.iImage		= JOB_Get_Current_Count() - 1;
	lvi.lParam		= (JOB_Get_Current_Count() - 1) * 10000 + 0;	// 2009.11.18
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
BOOL JobData_ListView_Clear( HWND hwndListView ) {
//	int         i;
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break; // 2011.06.16
//	for ( i = 0 ; i < 17 ; i++ ) ListView_DeleteColumn( hwndListView , 0 );
	return TRUE;
}
//
HTREEITEM TreeData_Sub_Insert( HWND hwndTreeView , HTREEITEM parent , BOOL first , BOOL fixedstr , char *fixedstrdata , int param , int i1 , int i2 ) {
	TV_INSERTSTRUCT TI;

	//---------------------------------------------------------
	TI.hParent				=	parent;
	TI.hInsertAfter			=	first ? TVI_FIRST : TVI_LAST;
	TI.item.mask			=	TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	TI.item.iImage			=	i1;
	TI.item.iSelectedImage	=	i2;
	//---------------------------------------------------------
	TI.item.lParam			= param;
	//---------------------------------------------------------
	if ( fixedstr ) {
		TI.item.pszText = fixedstrdata;
	}
	else {
		TI.item.pszText = LPSTR_TEXTCALLBACK;
	}
	return TreeView_InsertItem( hwndTreeView , &TI );
	//---------------------------------------------------------
}

//------------------------------------------------------------------------------------------
BOOL JobData_TreeView_Insert( HWND hWnd ) {
	HTREEITEM hitm;
	int index;
	if ( Get_Prm_FIND_MODE() ) return FALSE;
	if ( Get_Prm_DISPLAY_MODE() < 2 ) return FALSE;
	//
	hitm = TreeView_GetChild( hWnd , 0 );
	//
	index = JOB_Get_Current_Count() - 1;
	//
	if ( JOB_Has_LoopCount( index ) ) { // 2016.11.04
		TreeData_Sub_Insert( hWnd , hitm , TRUE , FALSE , "" , ( index * 10000 ) + 1 , 50 , 51 );
	}
	else {
		TreeData_Sub_Insert( hWnd , hitm , TRUE , FALSE , "" , ( index * 10000 ) + 1 , 1 , 8 );
	}
	//
	return TRUE;
}

BOOL JobData_ListView_Init( HWND hwndListView , HWND hwndTreeView ) {
	int         i , j , p;
	int rx; // 2015.05.27
	LV_ITEM lvi;
	LV_COLUMN   lvColumn;
	int multiicon; // 2016.03.29
	//
	HTREEITEM RNode;
	HTREEITEM RNode2;
	//
	TCHAR       *sz_C1_String[]    = { "ID" , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "START TIME" , "END TIME" , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };
	TCHAR       *sz_C1_String_L[]  = { "ID" , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "START TIME" , "ELAPSED"  , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };

	int         sz_C1_Size[14]     = {  65  ,        160 ,          150 ,       115 ,     80 ,   140 ,        140 ,          205 ,        230 ,       66 ,             205 ,    120 ,  30 ,        300 };
	int         sz_C1_Size_L[14]   = {  65  ,        160 ,          150 ,       115 ,     80 ,   140 ,        140 ,          205 ,        140 ,       66 ,             205 ,    120 ,  30 ,        300 };
	int         sz_C1_Size_M[14]   = {  50  ,        130 ,          130 ,        80 ,     50 ,    80 ,        120 ,          140 ,        155 ,       40 ,             140 ,     80 ,  25 ,        150 };
	int         sz_C1_Size_ML[14]  = {  50  ,        130 ,          130 ,        80 ,     50 ,    80 ,        120 ,          140 ,        100 ,       40 ,             140 ,     80 ,  25 ,        150 };
	//
	TCHAR       *sz_C2_String[]    = { "ID" , "START TIME" , "END TIME" , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };
	TCHAR       *sz_C2_String_L[]  = { "ID" , "START TIME" , "ELAPSE"   , "JOB NAME" , "LOT RECIPE" , "CM PATH" , "SLOT" , "RUN" , "MID NAME" , "Method" , "START(R) TIME" , "USER" , "T" , "LOG Info" };

	int         sz_C2_Size[14]     = {  65  ,          205 ,        230 ,        160 ,          150 ,       115 ,     80 ,   140 ,        140 ,       66 ,             205 ,    120 ,  30 ,        300 };
	int         sz_C2_Size_L[14]   = {  65  ,          205 ,        100 ,        160 ,          150 ,       115 ,     80 ,   140 ,        140 ,       66 ,             205 ,    120 ,  30 ,        300 };
	int         sz_C2_Size_M[14]   = {  50  ,          140 ,        155 ,        130 ,          130 ,        80 ,     50 ,    80 ,        120 ,       40 ,             140 ,     80 ,  25 ,        150 };
	int         sz_C2_Size_ML[14]  = {  50  ,          140 ,         65 ,        130 ,          130 ,        80 ,     50 ,    80 ,        120 ,       40 ,             140 ,     80 ,  25 ,        150 };

	//

	TCHAR       *sz_E_C1_String[]    = { "ID" , "SLOT" , "MID NAME" , "JOB NAME" , "RECIPE" , "START TIME" , "END TIME" , "LOG Info" };
	TCHAR       *sz_E_C1_String_L[]  = { "ID" , "SLOT" , "MID NAME" , "JOB NAME" , "RECIPE" , "START TIME" , "ELAPSED"  , "LOG Info" };

	int         sz_E_C1_Size[8]      = {  65  ,     80 ,        140 ,        160 ,      150 ,          205 ,        230 ,        300 };
	int         sz_E_C1_Size_L[8]    = {  65  ,     80 ,        140 ,        160 ,      150 ,          205 ,        140 ,        300 };
	int         sz_E_C1_Size_M[8]    = {  50  ,     50 ,        120 ,        130 ,      130 ,          140 ,        155 ,        150 };
	int         sz_E_C1_Size_ML[8]   = {  50  ,     50 ,        120 ,        130 ,      130 ,          140 ,        100 ,        150 };
	//
	TCHAR       *sz_E_C2_String[]    = { "ID" , "SLOT" , "MID NAME" , "START TIME" , "END TIME" , "JOB NAME" , "RECIPE" , "LOG Info" };
	TCHAR       *sz_E_C2_String_L[]  = { "ID" , "SLOT" , "MID NAME" , "START TIME" , "ELAPSE"   , "JOB NAME" , "RECIPE" , "LOG Info" };

	int         sz_E_C2_Size[8]      = {  65  ,     80 ,        140 ,          205 ,        230 ,        160 ,      150 ,        300 };
	int         sz_E_C2_Size_L[8]    = {  65  ,     80 ,        140 ,          205 ,        140 ,        160 ,      150 ,        300 };
	int         sz_E_C2_Size_M[8]    = {  50  ,     50 ,        120 ,          140 ,        155 ,        130 ,      130 ,        150 };
	int         sz_E_C2_Size_ML[8]   = {  50  ,     50 ,        120 ,          140 ,        100 ,        130 ,      130 ,        150 };

	//
	ListView_DeleteAllItems( hwndListView );
	//
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break; // 2011.06.16
//	for ( i = 0 ; i < 14 ; i++ ) ListView_DeleteColumn( hwndListView , 0 );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//--------------------------------------------------------------
	if ( ( Get_Prm_DISPLAY_MODE() == 1 ) || ( Get_Prm_DISPLAY_MODE() == 3 ) ) { // 2011.06.16
		//
		j = 8;
		//
		if ( ( Get_Info_DISPLAY_MODE() / 2 ) == 1 ) {
			if ( Get_Prm_MINISCREEN_MODE() ) {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 10 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_E_C2_Size_M[i]; // 10 Based
						}
						else {
							lvColumn.cx = ( sz_E_C2_Size_M[i] * Global_Pos_Font2 ) / 10;
						}
						switch( i ) {
						case 5 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C2_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						//
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 16 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_E_C2_Size_ML[i]; // 16 Based
						}
						else {
							lvColumn.cx = ( sz_E_C2_Size_M[i] * Global_Pos_Font2 ) / 16;
						}
						switch( i ) {
						case 5 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C2_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						//
						p++;
						//
					}
				}
			}
			else {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_E_C2_Size[i];
						switch( i ) {
						case 5 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C2_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_E_C2_Size_L[i];
						switch( i ) {
						case 5 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C2_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
			}
		}
		else {
			if ( Get_Prm_MINISCREEN_MODE() ) {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 10 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_E_C1_Size_M[i]; // 10 Based
						}
						else {
							lvColumn.cx = ( sz_E_C1_Size_M[i] * Global_Pos_Font2 ) / 10;
						}
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C1_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 16 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_E_C1_Size_ML[i]; // 16 Based
						}
						else {
							lvColumn.cx = ( sz_E_C1_Size_M[i] * Global_Pos_Font2 ) / 16;
						}
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C1_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
			}
			else {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_E_C1_Size[i];
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C1_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_E_C1_Size_L[i];
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						default :	lvColumn.pszText = sz_E_C1_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
			}
		}

	}
	else {
		//
		switch( Get_Info_DISPLAY_MODE() ) {
		case 1 : // Min
			j = 10;
			break;
		case 2 : // Time
			j = 14;
			break;
		case 3 : // Time/Min
			j = 10;
			break;
		default : // std
			j = 14;
			break;
		}
		//--------------------------------------------------------------
		if ( ( Get_Info_DISPLAY_MODE() / 2 ) == 1 ) {
			if ( Get_Prm_MINISCREEN_MODE() ) {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 10 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_C2_Size_M[i]; // 10 Based
						}
						else {
							lvColumn.cx = ( sz_C2_Size_M[i] * Global_Pos_Font2 ) / 10;
						}
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 8 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C2_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 16 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_C2_Size_ML[i]; // 16 Based
						}
						else {
							lvColumn.cx = ( sz_C2_Size_M[i] * Global_Pos_Font2 ) / 16;
						}
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 8 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C2_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
			}
			else {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_C2_Size[i];
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 8 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C2_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_C2_Size_L[i];
						switch( i ) {
						case 3 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 4 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 8 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C2_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
			}
		}
		else {
			if ( Get_Prm_MINISCREEN_MODE() ) {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 10 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_C1_Size_M[i]; // 10 Based
						}
						else {
							lvColumn.cx = ( sz_C1_Size_M[i] * Global_Pos_Font2 ) / 10;
						}
						switch( i ) {
						case 1 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C1_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						if ( ( Global_Pos_Font <= 16 ) || ( Global_Pos_Font2 < 1 ) ) {
							lvColumn.cx = sz_C1_Size_ML[i]; // 16 Based
						}
						else {
							lvColumn.cx = ( sz_C1_Size_M[i] * Global_Pos_Font2 ) / 16;
						}
						switch( i ) {
						case 1 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C1_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
			}
			else {
				if ( Get_Info_END_TIME_SHOW_MODE() == 0 ) {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_C1_Size[i];
						switch( i ) {
						case 1 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C1_String[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
				else {
					for ( i = 0 , p = 0 ; i < j ; i++ ) {
						lvColumn.cx = sz_C1_Size_L[i];
						switch( i ) {
						case 1 :	lvColumn.pszText = Get_Prm_JID_STRING();	break;
						case 2 :	lvColumn.pszText = Get_Prm_LID_STRING();	break;
						case 6 :	lvColumn.pszText = Get_Prm_MID_STRING();	break;
						case 9 :	lvColumn.pszText = Get_Prm_METHOD_STRING();	break;
						default :	lvColumn.pszText = sz_C1_String_L[i];	break;
						}
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , p , &lvColumn );
						p++;
						//
					}
				}
			}
		}
	}
	//
//	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE; // 2009.07.20
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM; // 2009.07.20
	//
	lvi.state = lvi.stateMask = 0;
	//
	if ( Get_Prm_FIND_MODE() ) j = JOB_FIND_Get_Current_Count();
	else                       j = JOB_Get_Current_Count();
	//
	// 2011.06.20
	for ( i = (j-1) , p = 0 ; i >= 0; i-- , p++ ) {
		lvi.iItem		= p;
		lvi.iSubItem	= 0;
		lvi.pszText		= LPSTR_TEXTCALLBACK;
		lvi.cchTextMax	= 0;
		lvi.iImage		= i;
		//
		//---------------------------------------------------------
		// 2009.07.20
		//---------------------------------------------------------
		if ( Get_Prm_FIND_MODE() ) {
			lvi.lParam		= ( JOB_FIND_Get_Real_Index( i ) * 10000 ) + 0;
		}
		else {
			lvi.lParam		= ( i * 10000 ) + 0;
		}
		//---------------------------------------------------------
		if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
		//---------------------------------------------------------
	}
	//
	if ( Get_Prm_DISPLAY_MODE() < 2 ) return TRUE; // 2011.07.15
	//
	TreeView_DeleteItem( hwndTreeView , TVI_ROOT );
	//
	//=======================================================================================================
	RNode = TreeData_Sub_Insert( hwndTreeView , 0 , FALSE , FALSE , "" , 0 , 0 , 7 );
	//=======================================================================================================
	if ( Get_Prm_FIND_MODE() ) j = JOB_FIND_Get_Current_Count();
	else                       j = JOB_Get_Current_Count();
	//
	for ( i = (j-1) ; i >= 0; i-- ) {
		//=======================================================================================================
		//
		// 2015.05.27
		//
//		if ( Get_Prm_FIND_MODE() ) {
//			p		= ( JOB_FIND_Get_Real_Index( i ) * 10000 );
//		}
//		else {
//			p		= ( i * 10000 );
//		}
//		//=======================================================================================================
//		RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , 1 , 8 );
//		//=======================================================================================================
//
		//
		if ( Get_Prm_FIND_MODE() ) {
			rx = JOB_FIND_Get_Real_Index( i );
		}
		else {
			rx = i;
		}
		//
		p = ( rx * 10000 );
		//
//		switch( JOB_Get_Run_Extern_Status( rx ) )	{ // 2016.03.29
		switch( JOB_Get_Run_Extern_Status( rx , &multiicon ) )	{ // 2016.03.29
		case 2 :
		case 3 :
			//=======================================================================================================
			if ( JOB_Has_LoopCount( rx ) ) { // 2016.11.04
				RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , 52 , 53 );
			}
			else {
				RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , 18 , 19 );
			}
			//=======================================================================================================
			break;
		case 4 : // 2015.11.12
			//=======================================================================================================
			if ( multiicon >= 0 ) {
				if ( JOB_Has_LoopCount( rx ) ) { // 2016.11.04
					RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , multiicon + 60 , multiicon + 60 );
				}
				else {
					RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , multiicon + 40 , multiicon + 40 );
				}
			}
			else {
				if ( JOB_Has_LoopCount( rx ) ) { // 2016.11.04
					RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , 54 , 55 );
				}
				else {
					RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , 20 , 21 );
				}
			}
			//=======================================================================================================
			break;
		default :
			//=======================================================================================================
			if ( JOB_Has_LoopCount( rx ) ) { // 2016.11.04
				RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , 50 , 51 );
			}
			else {
				RNode2 = TreeData_Sub_Insert( hwndTreeView , RNode , FALSE , FALSE , "" , p + 1 , 1 , 8 );
			}
			//=======================================================================================================
			break;
		}
	}


	return TRUE;
}
//

int TreeView_SelectionCheck( HWND hWnd , int *mode , int *subid ) {
	HTREEITEM hitm , hitm2;
	TVITEMEX TvEx;
	//
	*mode = -1;
	//
	hitm = TreeView_GetSelection( hWnd );
	//
	if ( hitm == NULL ) return -1;
	//
	TvEx.mask = TVIF_PARAM;
	TvEx.hItem = hitm;
	//
	if ( !TreeView_GetItem( hWnd , &TvEx ) ) return -1;
	//
	*mode  = TvEx.lParam % 10;
	//
	switch( TvEx.lParam % 10 ) {
	case 0 :
		*subid = (TvEx.lParam % 10000 ) / 10;
		return 0;
		break;
	case 1 :
		*subid = (TvEx.lParam % 10000 ) / 10;
		return TvEx.lParam / 10000;
		break;
	case 2 :
		*subid = (TvEx.lParam % 10000 ) / 10;
		return TvEx.lParam / 10000;
		break;
	case 3 :
		*subid = CURRENT_WAFER_ID[(TvEx.lParam % 10000 ) / 10]; // 2012.12.05
		return TvEx.lParam / 10000;
		break;
	case 4 :
		*subid = (TvEx.lParam % 10000 ) / 10;
		return TvEx.lParam / 10000;
		break;
	case 5 :
		*subid = CURRENT_WAFER_ID[(TvEx.lParam % 10000 ) / 10]; // 2012.12.05
		return TvEx.lParam / 10000;
		break;
	case 6 : // 2014.01.16
		*subid = (TvEx.lParam % 10000 ) / 10;
		return TvEx.lParam / 10000;
		break;

	case 7 : // 2016.08.17
	case 8 : // 2016.11.04
		//
		hitm2 = TreeView_GetParent( hWnd , TvEx.hItem );
		//
		if ( hitm2 == NULL ) return -1;
		//
		TvEx.mask = TVIF_PARAM | TVIF_IMAGE;
		TvEx.hItem = hitm2;
		//
		if ( !TreeView_GetItem( hWnd , &TvEx ) ) return -1;
		//
		*subid = (TvEx.lParam % 10000 ) / 10;
		return TvEx.lParam / 10000;
		break;
	}
	//
	return -1;
}

//
void JobData_Button_Check( HWND hWnd ) {
	int index , mode , subid;
	if ( Get_Prm_FIND_MODE() ) {
		KWIN_Item_Hide(  GetParent(hWnd) , IDC_CTRL_ADD );
		//
		if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.09.05
			KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
		}
		else {
			KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
		}
		//
		KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_START );
		KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_ABORT );
		KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_END );
		KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_CANCEL );
		//
		KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_GO_BACK );
	}
	else {
		if ( Get_Prm_SIMPLE_DISPLAY() == 2 ) { // 2005.04.08
			//
			if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.09.05
				KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_GO_BACK );
			}
			else {
				KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_GO_BACK );
			}
			//
			KWIN_Item_Hide(  GetParent(hWnd) , IDC_CTRL_ADD );
			KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_START );
			KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_ABORT );
			KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_END );
			KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_CANCEL );
			//
			if ( Get_Prm_NETMONITOR_MODE() ) {
				//=============================================================
				// 2006.08.30
				//=============================================================
				if ( Get_Prm_DELETE_HIDE() ) {
					KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
				}
				//=============================================================
				else {
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
				}
				//=============================================================
			}
			else {
				if ( JOB_Question_Count_Empty() ) {
					//=============================================================
					// 2006.08.30
					//=============================================================
					if ( Get_Prm_DELETE_HIDE() ) {
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
					}
					//=============================================================
					else {
						KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
					}
					//=============================================================
				}
				else {
					if ( JobData_ListView_DeleteEnable( hWnd ) ) {
						//=============================================================
						// 2006.08.30
						//=============================================================
						if ( Get_Prm_DELETE_HIDE() ) {
							KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
						}
						//=============================================================
						else {
							KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_DELETE );
						}
						//=============================================================
					}
					else {
						//=============================================================
						// 2006.08.30
						//=============================================================
						if ( Get_Prm_DELETE_HIDE() ) {
							KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
						}
						//=============================================================
						else {
							KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
						}
						//=============================================================
					}
				}
			}
		}
		else {
			//
			if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.09.05
				KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_GO_BACK );
			}
			else {
				KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_GO_BACK );
			}
			//
			if ( Get_Prm_NETMONITOR_MODE() ) {
				if ( Get_Prm_DISPLAY_MODE() != 0 ) { // 2011.06.15
					KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_ADD );
					KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_START );
					KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_ABORT );
					KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_END );
					KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_CANCEL );
				}
				else {
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ADD );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_START );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ABORT );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_END );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_CANCEL );
				}
				//=============================================================
				// 2006.08.30
				//=============================================================
				if ( Get_Prm_DELETE_HIDE() ) {
					KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
				}
				//=============================================================
				else {
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
				}
				//=============================================================
			}
			else {
				if ( JOB_Question_Count_Empty() ) {
					if ( Get_Prm_DISPLAY_MODE() != 0 ) { // 2011.06.15
						KWIN_Item_Hide(  GetParent(hWnd) , IDC_CTRL_ADD );
					}
					else {
						if ( Get_Prm_SIMPLE_DISPLAY() == 1 ) {
							KWIN_Item_Disable(  GetParent(hWnd) , IDC_CTRL_ADD );
						}
						else {
							KWIN_Item_Enable(  GetParent(hWnd) , IDC_CTRL_ADD );
						}
					}
					//=============================================================
					// 2006.08.30
					//=============================================================
					if ( Get_Prm_DELETE_HIDE() ) {
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
					}
					//=============================================================
					else {
						KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
					}
					//=============================================================
					if ( Get_Prm_DISPLAY_MODE() != 0 ) { // 2011.06.15
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_START );
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_ABORT );
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_END );
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_CANCEL );
					}
					else {
						KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_START );
						KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ABORT );
						KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_END );
						KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_CANCEL );
					}
				}
				else {
					if ( Get_Prm_DISPLAY_MODE() != 0 ) { // 2011.06.15
						KWIN_Item_Hide(  GetParent(hWnd) , IDC_CTRL_ADD );
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_START );
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_ABORT );
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_END );
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_CANCEL );
					}
					else {
						if ( JOB_Question_Count_Full() ) {
							KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ADD );
						}
						else {
							if ( Get_Prm_SIMPLE_DISPLAY() == 1 ) {
								KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ADD );
							}
							else {
								KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_ADD );
							}
						}
						//
						if ( JobData_ListView_SelectMode( hWnd , TRUE ) ) {
	//						switch( JOB_Get_Run_Status( JobData_Conv_ListView_to_JobIndex( JobData_ListView_SelectData( hWnd , -1 ) ) ) ) { // 2009.07.20
							switch( JOB_Get_Run_Status( JobData_Conv_ListView_to_JobIndex( hWnd , -1 ) ) ) { // 2009.07.20
							case JOB_READY :
									if ( Get_Prm_SIMPLE_DISPLAY() == 1 ) {
										KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_START );
									}
									else {
										KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_START );
									}
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ABORT );
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_END );
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_CANCEL );
									break;
							case JOB_RUNNING :
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_START );
									if ( Get_Prm_SIMPLE_DISPLAY() == 1 ) {
										KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ABORT );
									}
									else {
										KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_ABORT );
									}
									if ( Get_Prm_SIMPLE_DISPLAY() == 1 ) {
										KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_END );
									}
									else {
										KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_END );
									}
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_CANCEL );
									break;
							case JOB_ABORTING :
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_START );
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ABORT );
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_END );
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_CANCEL );
									break;
							default :
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_START );
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ABORT );
									KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_END );
									if ( Get_Prm_SIMPLE_DISPLAY() == 1 )
										KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_CANCEL );
									else
										KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_CANCEL );
							}
						}
						else {
							KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_START );
							KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_ABORT );
							KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_END );
							KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_CANCEL );
						}
					}
					//=============================================================
					// 2006.08.30
					//=============================================================
					if ( Get_Prm_DELETE_HIDE() ) {
						KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_DELETE );
					}
					//=============================================================
					else {
						if ( Get_Prm_DISPLAY_MODE() < 2 ) { // 2011.07.14
							if ( JobData_ListView_DeleteEnable( hWnd ) ) {
								KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_DELETE );
							}
							else {
								KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
							}
						}
						else { // 2011.07.14
							//-------------------------------------
							index = TreeView_SelectionCheck( GetDlgItem( GetParent(hWnd) , IDC_TREE ) , &mode , &subid );
							//-------------------------------------
							if ( mode == 1 ) {
								KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_DELETE );
							}
							else {
								KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_DELETE );
							}
							//-------------------------------------
						}
					}
					//=============================================================
				}
			}
		}
	}
	//
	if ( Get_Prm_DISPLAY_MODE() == 1 ) { // 2011.06.15
		if ( JobData_ListView_SelectMode( hWnd , FALSE ) ) {
			KWIN_Item_Show( GetParent(hWnd) , IDC_LIST_JOBDATA_S );
		}
		else {
			KWIN_Item_Hide( GetParent(hWnd) , IDC_LIST_JOBDATA_S );
		}
		//
		KWIN_Item_Hide( GetParent(hWnd) , IDC_LIST_JOBDATA_S2 );
		KWIN_Item_Hide( GetParent(hWnd) , IDC_LIST_JOBDATA_S2S );
		//
//		KWIN_Item_Hide( GetParent(hWnd) , IDC_LIST_JOBDATA_SPS ); // 2014.02.10 // 2014.07.17
		//
//		if ( Get_Prm_STATUS_VIEW() ) KWIN_Item_String_Display( GetParent(hWnd) , IDC_LIST_JOBDATA_SPS , "" ); // 2014.07.17 // 2015.10.28
		//
		KWIN_Item_Hide( GetParent(hWnd) , IDC_LIST_JOBDATA_SPS ); // 2015.10.28
		//
	}
	//
}
//

int  _JobData_ListView_SortDir[32];
HWND _JobData_ListView_SortHwnd;

int CALLBACK CallBack_JobData_ListView_Sort( LPARAM lParam1 , LPARAM lParam2 , LPARAM lParamSort ) {
	static LV_FINDINFO fi;
	static char szBuf1[256] , szBuf2[256];
	static int nItem1 , nItem2 , Res;

	if ( lParamSort == 0 ) {
		if ( _JobData_ListView_SortDir[lParamSort] == 0 )
			return ( ( lParam1 / 10000 ) < ( lParam2 / 10000 ) ? -1 : ( lParam1 / 10000 ) > ( lParam2 / 10000 ) ? 1 : 0 );
		else
			return ( ( lParam1 / 10000 ) > ( lParam2 / 10000 ) ? -1 : ( lParam1 / 10000 ) < ( lParam2 / 10000 ) ? 1 : 0 );
	}
	else {
		fi.flags = LVFI_PARAM;
		fi.lParam = lParam1;	nItem1 = ListView_FindItem( _JobData_ListView_SortHwnd , -1 , &fi );
		fi.lParam = lParam2;	nItem2 = ListView_FindItem( _JobData_ListView_SortHwnd , -1 , &fi );
		//
		ListView_GetItemText( _JobData_ListView_SortHwnd , nItem1 , lParamSort , szBuf1 , sizeof( szBuf1 ) );
		ListView_GetItemText( _JobData_ListView_SortHwnd , nItem2 , lParamSort , szBuf2 , sizeof( szBuf2 ) );
		//
		Res = strcmp( szBuf1 , szBuf2 );

		if ( Res == 0 ) {
			if ( ( lParam1 / 10000 ) > ( lParam2 / 10000 ) ) Res = 1;
			else Res = 0;
		}
		//
		if ( _JobData_ListView_SortDir[lParamSort] == 0 )
			return ( Res );
		else
			return ( Res * -1 );
	}
}
//
extern int ZOFFSET_5000_and_8000;


void Get_Module_Run_Time( int currview , char *stract ) {
	double s[1024] , ds , de , rate , rate2;
	double e[1024] , min , max;
	int i , j , a , c , f , fx;
	char Buffer[64];

	a = CURRENT_MODULE_VIEW_DEPTH[currview];
	//
	c = 0;
	//
	for ( i = 0 ; i < a ; i++ ) {
		//
		f = 0;
		//
		ds = CURRENT_WAFER_MODULE_START[CURRENT_MODULE_VIEW_1[currview][i]][CURRENT_MODULE_VIEW_2[currview][i]];
		de = CURRENT_WAFER_MODULE_END[CURRENT_MODULE_VIEW_1[currview][i]][CURRENT_MODULE_VIEW_2[currview][i]];
		//
		if ( i == 0 ) {
			min = ds;
			max = de;
		}
		else {
			if ( min > ds ) min = ds;
			if ( max < de ) max = de;
		}
		//
		for ( j = 0 ; j < c ; j++ ) {
			//
			if ( ds < s[j] ) {
				if ( de < s[j] ) {
					f = 1;
					fx = j;
					break;
				}
				else {
					if ( de > e[j] ) e[j] = de;
					f = -1;
					break;
				}
			}
			else {
				if ( ds <= e[j] ) {
					if ( de > e[j] ) e[j] = de;
					f = -1;
					break;
				}
			}
		}
		//
		if ( f == 0 ) { // 맨뒤에 추가
			//
			if ( c >= 1024 ) break;
			//
			s[c] = ds;
			e[c] = de;
			//
			c++;
			//
		}
		else if ( f == 1 ) { // 위치에 추가
			//
			if ( c >= 1024 ) break;
			//
			for ( j = (c-1) ; j >= fx ; j-- ) {
				s[j+1] = s[j];
				e[j+1] = e[j];
			}
			//
			s[fx] = ds;
			e[fx] = de;
			//
			c++;
			//
		}
		//
	}
	//
	while ( TRUE ) {
		//
		f = 0;
		//
		for ( j = 1 ; j < c ; j++ ) {
			//
			if ( s[j-1] > s[j] ) {
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
			}
			//
			if ( e[j-1] >= s[j] ) {
				if ( e[j-1] < e[j] ) e[j-1] = e[j];
				f = 1;
				fx = j;
				break;
			}
		}
		//
		if ( f == 1 ) {
			//
			c--;
			//
			for ( j = fx ; j < c ; j++ ) {
				s[j] = s[j+1];
				e[j] = e[j+1];
			}
			//
		}
		//
		if ( f == 0 ) break;
	}
	//
	rate = 0;
	//
	for ( i = 0 ; i < c ; i++ ) {
		//
		rate2 = e[i] - s[i];
		//
		if ( rate2 > 0 ) rate = rate + rate2;
		//
	}
	//
	//====================================================================================
	//
	Time_2_String( rate , Buffer );
	//
	rate2 = max - min;
	//
	if ( rate2 > 0 ) {
		//
		ds = ( rate * 100 ) / rate2;
		//
	}
	else {
		ds = 0;
		//
	}
	//
	rate2 = CURRENT_WAFER_Get_Max_Sec( currview ) - CURRENT_WAFER_Get_Min_Sec( currview );
	//
	if ( rate2 > 0 ) {
		//
		de = ( rate * 100 ) / rate2;
		//
	}
	else {
		de = 0;
		//
	}
	//
	//====================================================================================
	sprintf( stract , "%s (%.0f%c) (%.0f%c)" , Buffer , ds , '%'  , de , '%' );

}

void Get_Module_Idle_Time( int currview , char *stract , char *strall ) {
	double s[1024] , ds , de , rate , rate2;
	double e[1024] , min , max;
	int i , j , a , c , f , fx;
	char Buffer[64];

	a = CURRENT_MODULE_VIEW_DEPTH[currview];
	//
	c = 0;
	//
	for ( i = 0 ; i < a ; i++ ) {
		//
		f = 0;
		//
		ds = CURRENT_WAFER_MODULE_START[CURRENT_MODULE_VIEW_1[currview][i]][CURRENT_MODULE_VIEW_2[currview][i]];
		de = CURRENT_WAFER_MODULE_END[CURRENT_MODULE_VIEW_1[currview][i]][CURRENT_MODULE_VIEW_2[currview][i]];
		//
		if ( i == 0 ) {
			min = ds;
			max = de;
		}
		else {
			if ( min > ds ) min = ds;
			if ( max < de ) max = de;
		}
		//
		for ( j = 0 ; j < c ; j++ ) {
			//
			if ( ds < s[j] ) {
				if ( de < s[j] ) {
					f = 1;
					fx = j;
					break;
				}
				else {
					if ( de > e[j] ) e[j] = de;
					f = -1;
					break;
				}
			}
			else {
				if ( ds <= e[j] ) {
					if ( de > e[j] ) e[j] = de;
					f = -1;
					break;
				}
			}
		}
		//
		if ( f == 0 ) { // 맨뒤에 추가
			//
			if ( c >= 1024 ) break;
			//
			s[c] = ds;
			e[c] = de;
			//
			c++;
			//
		}
		else if ( f == 1 ) { // 위치에 추가
			//
			if ( c >= 1024 ) break;
			//
			for ( j = (c-1) ; j >= fx ; j-- ) {
				s[j+1] = s[j];
				e[j+1] = e[j];
			}
			//
			s[fx] = ds;
			e[fx] = de;
			//
			c++;
			//
		}
		//
	}
	//
	while ( TRUE ) {
		//
		f = 0;
		//
		for ( j = 1 ; j < c ; j++ ) {
			//
			if ( s[j-1] > s[j] ) {
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
				printf("====================ERROR==========================\n" );
			}
			//
			if ( e[j-1] >= s[j] ) {
				if ( e[j-1] < e[j] ) e[j-1] = e[j];
				f = 1;
				fx = j;
				break;
			}
		}
		//
		if ( f == 1 ) {
			//
			c--;
			//
			for ( j = fx ; j < c ; j++ ) {
				s[j] = s[j+1];
				e[j] = e[j+1];
			}
			//
		}
		//
		if ( f == 0 ) break;
	}
	//
	rate = 0;
	//
	for ( i = 1 ; i < c ; i++ ) {
		//
		rate2 = s[i] - e[i-1];
		//
		if ( rate2 > 0 ) rate = rate + rate2;
	}
	//
	//====================================================================================
	//
	Time_2_String( rate , Buffer );
	//
	rate2 = max - min;
	//
	if ( rate2 > 0 ) {
		//
		ds = ( rate * 100 ) / rate2;
		//
		sprintf( stract , "%s (%.0f%c)" , Buffer , ds , '%' );
		//
	}
	else {
		ds = 0;
		//
		sprintf( stract , "%s" , Buffer );
	}
	//
	//====================================================================================
	//
	ds = CURRENT_WAFER_Get_Min_Sec( currview );	if ( ds < min ) rate = rate + ( min - ds );
	de = CURRENT_WAFER_Get_Max_Sec( currview );	if ( de > max ) rate = rate + ( de - max );
	//
	Time_2_String( rate , Buffer );
	//
	rate2 = de - ds;
	//
	if ( rate2 > 0 ) {
		//
		ds = ( rate * 100 ) / rate2;
		//
		sprintf( strall , "%s (%.0f%c)" , Buffer , ds , '%' );
		//
	}
	else {
		ds = 0;
		//
		sprintf( strall , "%s" , Buffer );
	}
	//

	strcat( stract , " " );
	strcat( stract , strall );

}

BOOL LogView_Open_Method_History_Insert_CURRENT_WAFER( HWND hwndListView , BOOL multimode , int index0 ) {
	LV_COLUMN   lvColumn;
	LV_ITEM lvi;
	int i , j , k , s , index , dev , off , clmoff , tm , lp , si , ei;
	//
	char szString[256];
	char szString2[256];
	//
	int cset = 0;
	double rate;

	ZOFFSET_5000_and_8000 = 0;

	CURRENT_WAFER_Data_Set_MultiMode = multimode;

	if ( index0 == -99 ) {
		//
		CURRENT_WAFER_Data_Set_Min_Mode = 4;
		//
		index = 0;
		//
		dev = CURRENT_WAFER_Data_Set_Min_Dev; // 2017.03.17
		//

	}
	else if ( index0 < 0 ) {
		//
		index = CURRENT_WAFER_Data_Set_Min_Index;
		//
		if      ( CURRENT_WAFER_Data_Set_Min_Mode == 0 ) {
			CURRENT_WAFER_Data_Set_Min_Mode = 4;
		}
		else if ( CURRENT_WAFER_Data_Set_Min_Mode == 1 ) {
			CURRENT_WAFER_Data_Set_Min_Mode = 5;
		}
		else if ( CURRENT_WAFER_Data_Set_Min_Mode == 2 ) {
			CURRENT_WAFER_Data_Set_Min_Mode = 6;
		}
		else if ( CURRENT_WAFER_Data_Set_Min_Mode == 3 ) {
			CURRENT_WAFER_Data_Set_Min_Mode = 7;
		}
		else {
			CURRENT_WAFER_Data_Set_Min_Mode = 4;
		}
		//
		dev = CURRENT_WAFER_Data_Set_Min_Dev;
		//
	}
	else {
		//
		CURRENT_WAFER_Data_Set_Min_Mode = 0;
		//
		index = index0;
		//
		dev = CURRENT_WAFER_Data_Set_Min_Dev; // 2017.03.17
		//
	}

	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break;
	//
	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;

	//
	lvColumn.cx = 40;
	//
	lvColumn.pszText = "No";
	//
	if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
	//
	//
	ListView_InsertColumn( hwndListView , 0 , &lvColumn );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;

//	lvColumn.cx = 50; // 2013.12.05
	//
	clmoff = 0;
	//
	if ( CURRENT_WAFER_Data_Set_Min_Mode == 5 ) {
		lvColumn.cx = 150; // 2013.12.05
		lvColumn.pszText = "Run Time";
	}
	else if ( CURRENT_WAFER_Data_Set_Min_Mode == 7 ) {
		lvColumn.cx = 200; // 2013.12.05
		lvColumn.pszText = "Idle Time";
	}
	else {
		if ( multimode ) {
			//
			lvColumn.cx = 160;
			lvColumn.pszText = "Job";
			//
			if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO );
			//
			ListView_InsertColumn( hwndListView , 1 , &lvColumn );
			//
			clmoff = 1;
			//
			ZOFFSET_5000_and_8000 = 1;
			//
		}
		//
		lvColumn.cx = 70; // 2013.12.05
		lvColumn.pszText = "Slot";
	}
	//
	if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
	//
	ListView_InsertColumn( hwndListView , 1 + clmoff , &lvColumn );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;

	if ( ( CURRENT_WAFER_Data_Set_Min_Mode == 5 ) || ( CURRENT_WAFER_Data_Set_Min_Mode == 7 ) ) {
		lvColumn.cx = 80;
		lvColumn.pszText = "Module";
	}
	else {
		lvColumn.cx = 130;
		lvColumn.pszText = Get_Prm_METHOD_STRING();
	}
	//
	if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
	//
	ListView_InsertColumn( hwndListView , 2 + clmoff , &lvColumn );
	//
	if ( CURRENT_WAFER_Data_Set_Min_Mode != 0 ) {
		//
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		//
		rate = (double) dev / 100.0;
		//
		lvColumn.cx = (int) ( ( CURRENT_WAFER_Data_1_Max_Sec - CURRENT_WAFER_Data_1_Min_Sec ) * rate );
		//
		sprintf( szString , "TimeTable (%d %c)" , dev , '%' );
		lvColumn.pszText = szString;
		//
		if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
		//
		ListView_InsertColumn( hwndListView , 3 + clmoff , &lvColumn );
		//
	}

	if ( ( CURRENT_WAFER_Data_Set_Min_Mode == 5 ) || ( CURRENT_WAFER_Data_Set_Min_Mode == 7 ) ) { // Module / Module(idle)
		//
		CURRENT_MODULE_VIEW_COUNT = 0;
		//
		if ( CURRENT_WAFER_Data_Set_View_Mode == 0 ) {
			//
			j = 0;
			//
			while ( TRUE ) { // TRNASFER
				//
				s = 0;
				//
				for ( i = 0 ; i < CURRENT_WAFER_VIEW_COUNT ; i++ ) {
					//
					if ( j >= CURRENT_WAFER_MODULE_DEPTH[i] ) continue;
					//
					s = 1;
					//
					tm = CURRENT_WAFER_MODULE_TM[i][j];
					//
					if ( tm < 0 ) continue;
					//
					if ( tm == 0 ) {
						strcpy( szString , "FM" );
					}
					else {
						sprintf( szString , "TM%d" , tm );
					}
					//
					for ( k = 0 ; k < CURRENT_MODULE_VIEW_COUNT ; k++ ) {
						if ( STRCMP( szString , STR_MEM_GET_STR( CURRENT_MODULE_VIEW_NAME[k] ) ) ) break;
					}
					//
					if ( k == CURRENT_MODULE_VIEW_COUNT ) {
						if ( CURRENT_MODULE_VIEW_COUNT < MAX_CURRENT_WAFER_VIEW ) {
							//
							STR_MEM_MAKE_COPY2( &(CURRENT_MODULE_VIEW_NAME[CURRENT_MODULE_VIEW_COUNT]) , szString );
							//
							CURRENT_MODULE_VIEW_MINDEX[CURRENT_MODULE_VIEW_COUNT] = CURRENT_WAFER_MINDEX[i];
							//
							CURRENT_MODULE_VIEW_1[CURRENT_MODULE_VIEW_COUNT][0] = i;
							CURRENT_MODULE_VIEW_2[CURRENT_MODULE_VIEW_COUNT][0] = j;
							CURRENT_MODULE_VIEW_DEPTH[CURRENT_MODULE_VIEW_COUNT] = 1;
							//
							CURRENT_MODULE_VIEW_COUNT++;
						}
					}
					else {
						if ( CURRENT_MODULE_VIEW_DEPTH[k] < MAX_CURRENT_WAFER_VIEW ) {
							//
							CURRENT_MODULE_VIEW_1[k][CURRENT_MODULE_VIEW_DEPTH[k]] = i;
							CURRENT_MODULE_VIEW_2[k][CURRENT_MODULE_VIEW_DEPTH[k]] = j;
							CURRENT_MODULE_VIEW_DEPTH[k]++;
							//
						}
					}
				}
				//
				if ( s == 0 ) break;
				//
				j++;
			}
			//
			//
			for ( tm = 0 ; tm < 3 ; tm++ ) {
				//
				j = 0;
				//
				while ( TRUE ) {
					//
					s = 0;
					//
					for ( i = 0 ; i < CURRENT_WAFER_VIEW_COUNT ; i++ ) {
						//
						if ( j >= CURRENT_WAFER_MODULE_DEPTH[i] ) continue;
						//
						s = 1;
						//
						if ( CURRENT_WAFER_MODULE_MODE[i][j] == TCH_CM_IN ) continue;
						if ( CURRENT_WAFER_MODULE_MODE[i][j] == TCH_CM_OUT ) continue;
						//
						if      ( tm == 0 ) {
							if ( !STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "P" , 1 ) ) continue;
						}
						else if ( tm == 1 ) {
							if ( !STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "B" , 1 ) ) continue;
						}
						else {
							if ( STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "P" , 1 ) ) continue;
							if ( STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "B" , 1 ) ) continue;
						}
						//
						STR_SEPERATE_CHAR( STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , ':' , szString , szString2 , 128 );
						//
						for ( k = 0 ; k < CURRENT_MODULE_VIEW_COUNT ; k++ ) {
							if ( STRCMP( szString , STR_MEM_GET_STR( CURRENT_MODULE_VIEW_NAME[k] ) ) ) break;
						}
						//
						if ( k == CURRENT_MODULE_VIEW_COUNT ) {
							if ( CURRENT_MODULE_VIEW_COUNT < MAX_CURRENT_WAFER_VIEW ) {
								STR_MEM_MAKE_COPY2( &(CURRENT_MODULE_VIEW_NAME[CURRENT_MODULE_VIEW_COUNT]) , szString );
								//
								CURRENT_MODULE_VIEW_MINDEX[CURRENT_MODULE_VIEW_COUNT] = CURRENT_WAFER_MINDEX[i];
								//
								CURRENT_MODULE_VIEW_1[CURRENT_MODULE_VIEW_COUNT][0] = i;
								CURRENT_MODULE_VIEW_2[CURRENT_MODULE_VIEW_COUNT][0] = j;
								CURRENT_MODULE_VIEW_DEPTH[CURRENT_MODULE_VIEW_COUNT] = 1;
								//
								CURRENT_MODULE_VIEW_COUNT++;
							}
						}
						else {
							if ( CURRENT_MODULE_VIEW_DEPTH[k] < MAX_CURRENT_WAFER_VIEW ) {
								//
								CURRENT_MODULE_VIEW_1[k][CURRENT_MODULE_VIEW_DEPTH[k]] = i;
								CURRENT_MODULE_VIEW_2[k][CURRENT_MODULE_VIEW_DEPTH[k]] = j;
								CURRENT_MODULE_VIEW_DEPTH[k]++;
								//
							}
						}
					}
					//
					if ( s == 0 ) break;
					//
					j++;
				}
				//
			}
		}
		else {
			//
			ei = 0;
			//
			for ( off = 0 ; off < CURRENT_WAFER_VIEW_COUNT ; off++ ) {
				//
				if ( CURRENT_WAFER_MINDEX[ei] != CURRENT_WAFER_MINDEX[off] ) {
					//
					si = ei;
					ei = off;
					//
				}
				else {
					if ( off == (CURRENT_WAFER_VIEW_COUNT-1) ) {
						//
						si = ei;
						ei = off+1;
						//
					}
					else {
						continue;
					}
				}
				//
				s = CURRENT_MODULE_VIEW_COUNT;
				//
				for ( lp = 0 ; lp < 4 ; lp++ ) {
					//
					for ( i = si ; i < ei ; i++ ) {
						//
						for ( j = 0 ; j < CURRENT_WAFER_MODULE_DEPTH[i] ; j++ ) {
							//
							if ( lp == 0 ) { // fm/tm
								//
								tm = CURRENT_WAFER_MODULE_TM[i][j];
								//
								if ( tm < 0 ) continue;
								//
								if ( tm == 0 ) {
									strcpy( szString , "FM" );
								}
								else {
									sprintf( szString , "TM%d" , tm );
								}
							}
							else {
								//
								if ( CURRENT_WAFER_MODULE_MODE[i][j] == TCH_CM_IN ) continue;
								if ( CURRENT_WAFER_MODULE_MODE[i][j] == TCH_CM_OUT ) continue;
								//
								if      ( lp == 1 ) { // pm
									if ( !STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "P" , 1 ) ) continue;
								}
								else if ( lp == 2 ) {
									if ( !STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "B" , 1 ) ) continue;
								}
								else {
									if ( STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "P" , 1 ) ) continue;
									if ( STRNCMP_L( CURRENT_WAFER_MODULE_NAME[i][j] , "B" , 1 ) ) continue;
								}
								//
								STR_SEPERATE_CHAR( STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , ':' , szString , szString2 , 128 );
								//
							}
							//
							for ( k = s ; k < CURRENT_MODULE_VIEW_COUNT ; k++ ) {
								if ( STRCMP( szString , STR_MEM_GET_STR( CURRENT_MODULE_VIEW_NAME[k] ) ) ) break;
							}
							//
							if ( k == CURRENT_MODULE_VIEW_COUNT ) {
								if ( CURRENT_MODULE_VIEW_COUNT < MAX_CURRENT_WAFER_VIEW ) {
									//
									STR_MEM_MAKE_COPY2( &(CURRENT_MODULE_VIEW_NAME[CURRENT_MODULE_VIEW_COUNT]) , szString );
									//
									CURRENT_MODULE_VIEW_MINDEX[CURRENT_MODULE_VIEW_COUNT] = CURRENT_WAFER_MINDEX[i];
									//
									CURRENT_MODULE_VIEW_1[CURRENT_MODULE_VIEW_COUNT][0] = i;
									CURRENT_MODULE_VIEW_2[CURRENT_MODULE_VIEW_COUNT][0] = j;
									CURRENT_MODULE_VIEW_DEPTH[CURRENT_MODULE_VIEW_COUNT] = 1;
									//
									CURRENT_MODULE_VIEW_COUNT++;
								}
							}
							else {
								if ( CURRENT_MODULE_VIEW_DEPTH[k] < MAX_CURRENT_WAFER_VIEW ) {
									//
									CURRENT_MODULE_VIEW_1[k][CURRENT_MODULE_VIEW_DEPTH[k]] = i;
									CURRENT_MODULE_VIEW_2[k][CURRENT_MODULE_VIEW_DEPTH[k]] = j;
									CURRENT_MODULE_VIEW_DEPTH[k]++;
									//
								}
							}
						}
						//
					}
					//
				}
				//
			}
		}
		//
		//
		/*
		//
		strcpy( szString , "" );
		//
		lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lvi.state = lvi.stateMask = 0;
		lvi.iItem		= 0;
		lvi.iSubItem	= 0;
		lvi.pszText		= szString;
		lvi.cchTextMax	= 2560;
		lvi.iImage		= 0;
		//
		lvi.lParam		= ( 2 * 10000 ) + 4000 + dev;
		//
		if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
		//
		off = 1;
		//
		for ( i = 0 ; i < CURRENT_MODULE_VIEW_COUNT ; i++ ) {
			//
			sprintf( szString , "%d" , i + 1 );
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= i + off;
			lvi.iSubItem	= 0;
			lvi.pszText		= szString;
			lvi.cchTextMax	= 2560;
			lvi.iImage		= i + off;
			//
			if ( CURRENT_WAFER_Data_Set_Min_Mode == 5 ) {
				lvi.lParam		= ( i * 100000 ) + ( 0 * 10000 ) + 4000 + dev;
			}
			else {
				lvi.lParam		= ( i * 100000 ) + ( 1 * 10000 ) + 4000 + dev;
			}
			//
			if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
			//
			if ( CURRENT_WAFER_Data_Set_Min_Mode == 5 ) {
				Get_Module_Run_Time( i , szString );
			}
			else {
				Get_Module_Idle_Time( i , szString , szString2 );
			}
			//
			ListView_SetItemText( hwndListView , i + off , 1 , szString );
			//
			ListView_SetItemText( hwndListView , i + off , 2 , STR_MEM_GET_STR( CURRENT_MODULE_VIEW_NAME[i] ) );
			//
		}
		*/
		//
		j = -1;
		//
		off = 0;
		//
		if ( CURRENT_WAFER_Data_Set_View_Mode == 0 ) {
			//
			strcpy( szString , "" );
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= off;
			lvi.iSubItem	= 0;
			lvi.pszText		= szString;
			lvi.cchTextMax	= 2560;
			lvi.iImage		= off;
			//
			lvi.lParam		= ( 2 * 10000 ) + 4000 + dev;
			//
			if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
			//
			off++;
			//
		}
		//
		for ( i = 0 ; i < CURRENT_MODULE_VIEW_COUNT ; i++ ) {
			//
			if ( CURRENT_WAFER_Data_Set_View_Mode != 0 ) {
				//
				if ( j != CURRENT_MODULE_VIEW_MINDEX[i] ) {
					//
					j = CURRENT_MODULE_VIEW_MINDEX[i];
					//
					strcpy( szString , ">>" );
					//
					lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
					lvi.state = lvi.stateMask = 0;
					lvi.iItem		= off;
					lvi.iSubItem	= 0;
					lvi.pszText		= szString;
					lvi.cchTextMax	= 2560;
					lvi.iImage		= 0;
					//
					lvi.lParam		= ( 2 * 10000 ) + 4000 + dev;
					//
					if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
					//
					if ( CURRENT_MODULE_VIEW_DEPTH[i] > 0 ) {
						sprintf( szString , "%s [J%d]" , JOB_DATA[CURRENT_WAFER_JOBINDEX[CURRENT_MODULE_VIEW_1[i][0]]].JOB_NAME , CURRENT_WAFER_JOBINDEX[CURRENT_MODULE_VIEW_1[i][0]] + 1 );
						ListView_SetItemText( hwndListView , off , 1 , szString );
					}
					//
					off++;
					//
				}
				//
			}
			//
			sprintf( szString , "%d" , i + 1 );
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= off;
			lvi.iSubItem	= 0;
			lvi.pszText		= szString;
			lvi.cchTextMax	= 2560;
			lvi.iImage		= 0;
			//
			if ( CURRENT_WAFER_Data_Set_Min_Mode == 5 ) {
				lvi.lParam		= ( i * 100000 ) + ( 0 * 10000 ) + 4000 + dev;
			}
			else {
				lvi.lParam		= ( i * 100000 ) + ( 1 * 10000 ) + 4000 + dev;
			}
			//
			if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
			//
			if ( CURRENT_WAFER_Data_Set_Min_Mode == 5 ) {
				Get_Module_Run_Time( i , szString );
			}
			else {
				Get_Module_Idle_Time( i , szString , szString2 );
			}
			//
			ListView_SetItemText( hwndListView , off , 1 , szString );
			//
			ListView_SetItemText( hwndListView , off , 2 , STR_MEM_GET_STR( CURRENT_MODULE_VIEW_NAME[i] ) );
			//
			off++;
			//
		}

	}
	else {
		//
		off = 0;
		//
		if ( CURRENT_WAFER_Data_Set_Min_Mode != 0 ) {
			//
			strcpy( szString , "" );
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= 0;
			lvi.iSubItem	= 0;
			lvi.pszText		= szString;
			lvi.cchTextMax	= 2560;
			lvi.iImage		= 0;
			//
			lvi.lParam		= ( 2 * 10000 ) + 5000 + dev;
			//
			if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
			//
			off = 1;
			//
		}
		//
		for ( i = 0 ; i < CURRENT_WAFER_VIEW_COUNT ; i++ ) {
			//
			sprintf( szString , "%d" , i + 1 );
			//
			lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvi.state = lvi.stateMask = 0;
			lvi.iItem		= i + off;
			lvi.iSubItem	= 0;
			lvi.pszText		= szString;
			lvi.cchTextMax	= 2560;
			lvi.iImage		= i + off;
			//
			if      ( CURRENT_WAFER_Data_Set_Min_Mode == 4 ) {
				lvi.lParam		= ( 0 * 10000 ) + 5000 + dev;
			}
			else if ( CURRENT_WAFER_Data_Set_Min_Mode == 6 ) {
				lvi.lParam		= ( 1 * 10000 ) + 5000 + dev;
			}
			else {
				//
				if ( multimode ) {
					lvi.lParam		= ( CURRENT_WAFER_JOBINDEX[i] * 10000 ) + 8000 + CURRENT_WAFER_ID[i];
				}
				else {
					lvi.lParam		= ( index * 10000 ) + 8000 + CURRENT_WAFER_ID[i];
				}
				//
			}
			//
			if ( ListView_InsertItem( hwndListView , &lvi ) == -1 ) return FALSE;
			//
			if ( multimode ) {
				sprintf( szString , "%dJ%d (%s)" , ( CURRENT_WAFER_MINDEX[i] % 2 ) , CURRENT_WAFER_JOBINDEX[i] + 1 , JOB_DATA[ CURRENT_WAFER_JOBINDEX[i] ].JOB_NAME );
				ListView_SetItemText( hwndListView , i + off , 1 , szString );
				//
			}
			//
			if ( CURRENT_WAFER_ID[i] >= 100 ) {
				//
				if ( CURRENT_WAFER_DUMMYIN[i] > 0 ) { // 2013.12.05
					sprintf( szString , "D:BM%d:%d" , CURRENT_WAFER_DUMMYIN[i] , CURRENT_WAFER_ID[i] % 100 );
				}
				else {
					sprintf( szString , "D%d" , CURRENT_WAFER_ID[i] % 100 );
				}
				//
			}
			else {
				sprintf( szString , "%d" , CURRENT_WAFER_ID[i] );
			}
			//
			ListView_SetItemText( hwndListView , i + off , 1 + clmoff , szString );
			//
			ListView_SetItemText( hwndListView , i + off , 2 + clmoff , STR_MEM_GET_STR( CURRENT_WAFER_NAME[i] ) );
			//
			if ( CURRENT_WAFER_Data_Set_Min_Mode != 0 ) {
				//
				if ( multimode ) {
					sprintf( szString2 , "%s %s [J%d]" , STR_MEM_GET_STR( CURRENT_WAFER_NAME[i] ) , szString , CURRENT_WAFER_JOBINDEX[i] + 1 );
				}
				else {
					sprintf( szString2 , "%s %s" , STR_MEM_GET_STR( CURRENT_WAFER_NAME[i] ) , szString );
				}
				//
				ListView_SetItemText( hwndListView , i + off , 3 + clmoff , szString2 );
				//
				continue;
			}
			//
			s = 0;
			// In
			for ( k = 0 ; k < 3 ; k++ ) {
				//
				for ( j = 0 ; j < CURRENT_WAFER_MODULE_DEPTH[i] ; j++ ) {
					//
					if      ( k == 0 ) { // IN
						if ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_CM_IN ) continue;
					}
					else if ( k == 1 ) {
	//					if ( ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_PROCESS_X ) && ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_PM_PLACE ) && ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_PM_PICK ) ) continue;
						if ( ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_PROCESS ) && ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_BM_PLACE ) && ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_BM_PICK ) ) continue;
					}
					else { // OUT
						if ( CURRENT_WAFER_MODULE_MODE[i][j] != TCH_CM_OUT ) continue;
					}
					//
					if ( s >= cset ) {
						//
						cset = s + 1;
						//
						lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
						lvColumn.fmt = LVCFMT_CENTER;

						lvColumn.cx = 80;
						//
						if ( s == 0 ) {
							sprintf( szString , "IN" );
						}
						else {
							sprintf( szString , "%d" , s );
						}
						lvColumn.pszText = szString;
						//
						if ( LISTVIEW_HEIGHT_UPDATE ) lvColumn.cx = (int) ( lvColumn.cx * LISTVIEW_HEIGHT_RATIO ); // 2015.07.29
						//
						ListView_InsertColumn( hwndListView , s + 3 + clmoff , &lvColumn );
						//
					}
					//
					if ( ( CURRENT_WAFER_Data_Set_Min_Mode == 4 ) || ( CURRENT_WAFER_Data_Set_Min_Mode == 6 ) ) {
						ListView_SetItemText( hwndListView , i + off , s + 3 + clmoff , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) );
					}
					else {
						if ( STRNCMP_L( STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , "P" , 1 ) ) {
							if ( ( CURRENT_WAFER_MODULE_END[i][j] <= 0 ) || ( CURRENT_WAFER_MODULE_START[i][j] <= 0 ) ) {
								sprintf( szString , "%s(?)" , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) );
							}
							else {
								sprintf( szString , "%s(%.0f)" , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , CURRENT_WAFER_MODULE_END[i][j] - CURRENT_WAFER_MODULE_START[i][j] );
							}
							//
							ListView_SetItemText( hwndListView , i + off , s + 3 + clmoff , szString );
						}
						else {
							ListView_SetItemText( hwndListView , i + off , s + 3 + clmoff , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) );
						}
					}
					//
					s++;
					//
				}
			}
		}
	}
	return TRUE;
}
//
//----------------------
BOOL LogView_Open_Method_History_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName0 , int jobindex , int infoinsert , int findwid , int fileorder , int mindex , int option6 );


BOOL TreeData_Get_Select_Slot_PM_Info( int index , int subid , char *waferstr , char *pmname , char *pmnameorg ) {
	//
	CURRENT_WAFER_VIEW2 = FALSE;
	//----------------------------------------------------------------------------------------------
	LogView_Open_Method_History_Data_Set_Callback( NULL , NULL , 0 , NULL , index , 2 , subid , -1 , 0 , 0 );
	//----------------------------------------------------------------------------------------------
	if ( !CURRENT_WAFER_VIEW2 ) return FALSE;
	//----------------------------------------------------------------------------------------------
	strcpy( waferstr , STR_MEM_GET_STR( CURRENT_WAFER_ID2 ) );
	strcpy( pmname , STR_MEM_GET_STR( CURRENT_WAFER_NAME2 ) );
	strcpy( pmnameorg , STR_MEM_GET_STR( CURRENT_WAFER_NAME2_ORG ) );
	//----------------------------------------------------------------------------------------------
	return TRUE;
}

int TREEVIEW_LAST_PARAM = -1;

int Get_Alarm_Wafer_Count( int slot ) {
	int i;
	for ( i = 0 ; i < CURRENT_WAFER2_ALARM_COUNT ; i++ ) {
		if ( CURRENT_WAFER2_ALARM_WFRID[i] == slot ) {
			return CURRENT_WAFER2_ALARM_HAPPEN[i];
		}
	}
	return 0;
}

int Get_Alarm_Count( int index ) {
	int c , c1 , c2;
	char FileName[256];
	char Buffer[1025];
	BOOL bres;
	FILE *fpt;
	char ResultString1[64];
	char ResultString2[64];
	char ResultString3[64];
	//
	char folderstr[256];
	//===================================================
	CURRENT_WAFER2_ALARM_COUNT = 0;
	//===================================================
	if ( index < 0 ) return 0;
	if ( JOB_Get_Current_Count() <= index ) return 0;
	//----------------------------------------------------------------------------------------------
	JOB_Get_Directory( index , folderstr );
	//
	sprintf( FileName , "%s/LotLog/%s/AlarmCount.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
	//----------------------------------------------------------------------------------------------
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) return 0;
	//
	c = 0;
	//
	do {
		bres = Read_Line_From_File4( fpt , Buffer , 1024 );
		if ( *Buffer != 0 ) {
			//
			STR_SEPERATE_CHAR3( Buffer , 9 , ResultString1 , ResultString2 , ResultString3 , 63 );
			//
			if ( STRCMP_L( ResultString1 , "SLOT_ALARM" ) ) {
				//
				c1 = atoi( ResultString2 );
				c2 = atoi( ResultString3 );
				//
				if ( ( c1 > 0 ) && ( c2 > 0 ) ) {
					if ( CURRENT_WAFER2_ALARM_COUNT < MAX_CURRENT_WAFER_VIEW ) {
						CURRENT_WAFER2_ALARM_WFRID[CURRENT_WAFER2_ALARM_COUNT] = c1;
						CURRENT_WAFER2_ALARM_HAPPEN[CURRENT_WAFER2_ALARM_COUNT] = c2;
						CURRENT_WAFER2_ALARM_COUNT++;
					}
				}
				//
			}
			else {
				if ( c == 0 ) {
					c = atoi( Buffer );
				}
			}
			//
		}
	}
	while ( bres );
	fclose( fpt );
	return c;
}
//
int Get_User_File( int index , char *name ) { // 2014.01.16
	int c , encflag;
	char FileName[256];
	char Buffer[1025];
	BOOL bres;
	FILE *fpt;
	char ResultString1[64];
	char ResultString2[64];

	char folderstr[256];
	//===================================================
	if ( index < 0 ) return 0;
	if ( JOB_Get_Current_Count() <= index ) return 0;
	//----------------------------------------------------------------------------------------------
	JOB_Get_Directory( index , folderstr );

	sprintf( FileName , "%s/LotLog/%s/User.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
	//----------------------------------------------------------------------------------------------
	if ( ( fpt = fopen( FileName , "r" ) ) == NULL ) return 0;
	//
	strcpy( name , "User Info" );
	//
	c = 0;
	//
	encflag = 0;
	//
	//
	do {
		//
		bres = Read_Line_From_File4( fpt , Buffer , 1024 );
		//
		if ( *Buffer != 0 ) {
			//
			Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer );
			//
			if ( Buffer[0] == '#' ) continue; // 2014.02.26
			//
			STR_SEPERATE_CHAR( Buffer , 9 , ResultString1 , ResultString2 , 63 );
			//
			if      ( STRCMP_L( ResultString1 , "$TITLE" ) ) {
				strcpy( name , ResultString2 );
			}
			else if ( STRCMP_L( ResultString1 , "$COLUMN" ) ) {
			}
			else if ( STRCMP_L( ResultString1 , "$SIZE" ) ) {
			}
			else {
				c++;
			}
			//
		}
	}
	while ( bres );
	fclose( fpt );
	return ( c > 0 );
}
//


int TreeData_Sub_ImgChange( HWND hwndTreeView , HTREEITEM hItem0 , int index , int i1 , int i2 ) {
	TVITEM TI;
	TVITEMEX TvEx;
	HTREEITEM hItem , rhitm;
	//
	if ( hItem0 == NULL ) {
		//
		hItem = TreeView_GetChild( hwndTreeView , TVI_ROOT );
		//
		rhitm = TreeView_GetChild( hwndTreeView , hItem );
		//
		while ( TRUE ) {
			//
			hItem = TreeView_GetNextSibling( hwndTreeView , rhitm );
			//
			if ( hItem == NULL ) break;
			//
			TvEx.mask = TVIF_PARAM;
			TvEx.hItem = hItem;
			//
			if ( TreeView_GetItem( hwndTreeView , &TvEx ) ) {
				//
				if ( ( TvEx.lParam % 10 ) == 1 ) {
					//
					if ( ( TvEx.lParam / 10000 ) == index ) {
						//
						break;
						//
					}
					//
				}
				//
			}
			//
			rhitm = hItem;
			//
		}
		//
		if ( hItem == NULL ) return 1;
		//
	}
	else {
		hItem = hItem0;
		//
		if ( hItem == NULL ) return 2;
		//
	}
	//
	//---------------------------------------------------------
	TI.mask				= TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.hItem			=	hItem;
	TI.iImage			=	i1;
	TI.iSelectedImage	=	i2;
	//---------------------------------------------------------
	if ( TreeView_SetItem( hwndTreeView , &TI ) ) return 0;
	//---------------------------------------------------------
	return 3;
}



BOOL TreeData_Select_Index( HWND hwndTreeView , int index ) {
	TVITEMEX TvEx;
	HTREEITEM hItem , rhitm;
	//
	hItem = TreeView_GetChild( hwndTreeView , TVI_ROOT );
	//
	rhitm = TreeView_GetChild( hwndTreeView , hItem );
	//
	while ( TRUE ) {
		//
		hItem = TreeView_GetNextSibling( hwndTreeView , rhitm );
		//
		if ( hItem == NULL ) break;
		//
		TvEx.mask = TVIF_PARAM;
		TvEx.hItem = hItem;
		//
		if ( TreeView_GetItem( hwndTreeView , &TvEx ) ) {
			//
			if ( ( TvEx.lParam % 10 ) == 1 ) {
				//
				if ( ( TvEx.lParam / 10000 ) == index ) {
					//
					break;
					//
				}
				//
			}
			//
		}
		//
		rhitm = hItem;
		//
	}
	//
	if ( hItem == NULL ) return FALSE;
	//
	TreeView_Select( hwndTreeView , hItem , TVGN_CARET );
	//
	return TRUE;
	//
}

//
BOOL Get_Step_Info_Data_From_DataFile_Pre_Reading( char *filename , BOOL allread ) { // 2018.04.23
	int i , l;
	//
	l = strlen( Step_Log_FileName );
	//
	if ( !STRNCMP_L( Step_Log_FileName , filename , l ) ) return FALSE;
	//
	if ( allread ) {
		//
		STINFO_StepCount = 0;
		//
		for ( i = 0 ; i < Step_Log_Step_Count ; i++ ) {
			//
			if ( STRCMP_L( Step_Log_StepName[i] , "?" ) ) continue;
			//
			STINFO_StepData[STINFO_StepCount] = i;
			//
			strcpy( STINFO_StepName[STINFO_StepCount] , Step_Log_StepName[i] );
			//
			STINFO_StepCount++;
			//
		}
		//
	}
	//
	return ( STINFO_StepCount > 0 );
}

BOOL Get_Step_Info_Pointer_From_DataFile_Pre_Reading( char *filename , int Step , int *fp_d , int *fp_a ) { // 2018.04.23
	int l;
	//
	l = strlen( Step_Log_FileName );
	//
	if ( !STRNCMP_L( Step_Log_FileName , filename , l ) ) return FALSE;
	//
	if ( Step < 0 ) return FALSE;
	if ( Step >= MAX_STEP_COUNT ) return FALSE;
	//
	if ( Step_Log_StepStart[Step] < 0 ) return FALSE;
	if ( Step_Log_StepEnd[Step] < 0 ) return FALSE;
	//
	*fp_d = Step_Log_StepStart[Step];
	*fp_a = Step_Log_StepEnd[Step];
	//
	return TRUE;
}

BOOL Get_Step_Info_Data_From_File( char *filename , BOOL allread ) {
	//
	FILE *fpt;
	BOOL bres;
	char Buffer[256];
	//
	char Bufferx[64];
	//
	int z , encflag;
	//
	STINFO_StepCount = 0; // 2018.04.23
	//
	if ( ( fpt = fopen( filename , "r" ) ) == NULL ) {
		//
		if ( !AUTO_STEPVIEW ) return FALSE; // 2018.04.24
		//
		return Get_Step_Info_Data_From_DataFile_Pre_Reading( filename , allread );
		//
//		return FALSE; // 2018.04.23
	}
	//
	encflag = 0;
	//
	do {
		//
		if ( STINFO_StepCount >= MAX_STEP_COUNT ) break;
		//
		bres = Read_Line_From_File4( fpt , Buffer , 255 );
		//
		if ( *Buffer != 0 ) {
			//
			if ( Buffer[0] != '#' ) {
				//
				Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer );
				//
				z = 0;
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 ); // time
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 ); // step
				//
				STINFO_StepData[STINFO_StepCount] = atoi( Bufferx );
				//
				if ( ( STINFO_StepData[STINFO_StepCount] < 0 ) || ( STINFO_StepData[STINFO_StepCount] >= MAX_STEP_COUNT ) ) continue;
				//
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 );	// fp_d
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 );	// fp_a
				//
//				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , STINFO_StepName[STINFO_StepCount] , z , 31 );	// Name // 2016.09.21
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , STINFO_StepName[STINFO_StepCount] , z , 63 );	// Name // 2016.09.21
				//
				STINFO_StepCount++;
				//
			}
		}
	}
	while ( bres );
	//
	fclose( fpt );
	//
	return ( STINFO_StepCount > 0 );
	//
}

BOOL Get_Step_Info_Pointer_From_File( char *filename , int S , int Step , int *fp_d , int *fp_a ) {
	//
	FILE *fpt;
	BOOL bres;
	char Buffer[256];
	//
	char Bufferx[64];
	//
	int z , encflag , St , Rs;
	//
	if ( ( fpt = fopen( filename , "r" ) ) == NULL ) {
//		return FALSE; // 2018.04.24
		return Get_Step_Info_Pointer_From_DataFile_Pre_Reading( filename , Step , fp_d , fp_a ); // 2018.04.23
	}
	//
	Rs = 0;
	//
	encflag = 0;
	//
	do {
		//
		if ( STINFO_StepCount >= MAX_STEP_COUNT ) break;
		//
		bres = Read_Line_From_File4( fpt , Buffer , 255 );
		//
		if ( *Buffer != 0 ) {
			//
			if ( Buffer[0] != '#' ) {
				//
				Read_Simple_Encryption( &encflag , LOG_ENCRIPTION , Buffer );
				//
				z = 0;
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 ); // time
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 ); // step
				//
				St = atoi( Bufferx );
				//
				if ( ( St < 0 ) || ( St >= MAX_STEP_COUNT ) ) continue;
				//
				if ( S >= 0 ) {
					if ( Rs < S ) {
						Rs++;
						continue;
					}
					if ( Rs > S ) break;
				}
				//
				if ( Step >= 0 ) {
					if ( St != Step ) {
						Rs++;
						continue;
					}
				}
				//
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 );	// fp_d
				*fp_d = atoi( Bufferx );
				//
				z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Bufferx , z , 63 );	// fp_a
				*fp_a = atoi( Bufferx );
				//
				fclose( fpt );
				//
				return TRUE;
				//
			}
		}
	}
	while ( bres );
	//
	fclose( fpt );
	//
	return FALSE;
	//
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL LogView_Change_Loop_Sel_Count( HWND hdlg , int index , int lpcnt_max , int lpcnt_curr ) {
	int sel;
	//
	sel = lpcnt_curr + 1;
	//
	if ( !MODAL_DIGITAL_BOX2( hdlg , "Select" , "Select Loop Index" , 1 , lpcnt_max , &sel ) ) return FALSE;
	//
	if ( JOB_Get_Current_Count() <= index ) {
		MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed" , "Try Again" , MB_ICONHAND );
		return FALSE;
	}
	if ( JOB_Get_Directory_Type( index ) != 1 ) {
		MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] Item Count Changed & Data not exist" , "Try Again" , MB_ICONHAND );
		return FALSE;
	}
	//
	JOB_Set_Sel_LoopCount( index , sel - 1 );
	//
	return TRUE;
}


BOOL LogView_Open_Lot_History_Data_Set_Callback( HWND hWnd , HWND hlistview , int sepload , char *FileName , int mode , int wfrid , int append , int jobindex , int mindex , int analstartindex );

void JobData_TreeView_Job_Insert( HWND hWnd , HWND hWndPr , int index , HTREEITEM hMainitm ) { // 2016.11.04
	//
	int i , j , k;
	//
	TCHAR szString[ 256 ];
	TCHAR szString2[ 256 ];
	TCHAR szString3[ 256 ];
	TCHAR szString4[ 256 ];
	TCHAR pmorg[256];
	//
	char folderstr[256];
	//
	HTREEITEM hitm;
	HTREEITEM hitm2; // 2016.03.24
	//
	BOOL savedata; // 2015.11.18
	//
	int S; // 2016.03.24
	//
	savedata = FALSE; // 2015.11.18
	//
	CURRENT_WAFER_Data_Set_Min_Mode = 0; // 2012.11.26
	CURRENT_WAFER_Data_Set_Min_Index = index; // 2012.11.26
	//
	CURRENT_WAFER_INIT( 0 );
	//
	//----------------------------------------------------------------------------------------------
	if ( JOB_User_Fail_Check( index ) ) { // 2015.11.12
		//
		k = JOB_User_Get_Icon( index ); // 2016.03.29
		//
		TreeData_Sub_ImgChange( hWnd , hMainitm , 0 , k + 40 , k + 40 ); // 2016.03.29
		//
		savedata = TRUE; // 2015.11.18
	}
	//----------------------------------------------------------------------------------------------
	//
	LogView_Open_Method_History_Data_Set_Callback( NULL , NULL , 0 , NULL , index , 1 , -1 , -1 , 0 , 0 );
	//
	JOB_Get_Directory( index , folderstr );
	//----------------------------------------------------------------------------------------------
	sprintf( szString , "%s/LotLog/%s/LotData.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
	//----------------------------------
	//
	LogView_Open_Lot_History_Data_Set_Callback( NULL , NULL , 0 , szString , 0 , 0 , 0 , index , -1 , 0 );
	//
	LogView_Open_Method_History_Insert_CURRENT_WAFER( GetDlgItem( hWndPr , IDC_LIST_JOBDATA_S ) , FALSE , index );
	//----------------------
	while ( TRUE ) {
		hitm = TreeView_GetChild( hWnd , hMainitm );
		if ( hitm == NULL ) break;
		if ( !TreeView_DeleteItem( hWnd , hitm ) ) break;
	}
	//
	CURRENT_WAFER2_ALARM_COUNT = 0;
	//
	k = JOB_Get_LoopCount( index , &i );
	//
	if ( ( CURRENT_WAFER_VIEW_COUNT <= 0 ) && ( k <= 0 ) ) {
		//
		if ( JOB_DATA[ index ].OUT_CASS < 0 ) {
			if ( JOB_HANDOFF[index] )	  wsprintf( szString, "Running" );
			else                          wsprintf( szString, "Fail..." );
		}
		else {
			if      ( JOB_RUN[ index ] == 0 ) wsprintf( szString, "Fail..." );
			else if ( JOB_RUN[ index ] == 1 ) {
				if ( JOB_HANDOFF[index] )	  wsprintf( szString, "Wait-Run" );
				else                          wsprintf( szString, "Wait-HandOff" );
			}
			else if ( JOB_RUN[ index ] == 2 ) {
				wsprintf( szString, "Running" );
			}
			else {
				wsprintf( szString, "Fail..." );
			}
		}
		//
		TreeData_Sub_Insert( hWnd , hMainitm , FALSE , TRUE , szString , ( index * 10000 ) + 9 , 4 , 11 );
	}
	else {
		//
		// 2016.11.04
		//
		if ( k > 0 ) {
			//
			sprintf( szString , "LOOP:%d/MAX:%d" , i+1 , k );
			//
			hitm = TreeData_Sub_Insert( hWnd , hMainitm , FALSE , TRUE , szString , ( index * 10000 ) + 8 , 48 , 49 );
			//
		}
		//
		//
		//
		//
		k = Get_Alarm_Count( index );
		//
		if ( k > 0 ) {
			//
			sprintf( szString , "Alarm %d(s)" , k );
			//
			hitm = TreeData_Sub_Insert( hWnd , hMainitm , FALSE , TRUE , szString , ( index * 10000 ) + 2 , 5 , 12 );
			//
		}
		//
		if ( Get_User_File( index , szString ) ) { // 2014.01.16
			//
			hitm = TreeData_Sub_Insert( hWnd , hMainitm , FALSE , TRUE , szString , ( index * 10000 ) + 6 , 16 , 17 );
			//
		}
		//
		k = 0;
		//
		for ( i = 0 ; i < CURRENT_WAFER_VIEW_COUNT ; i++ ) {
			//
			if ( CURRENT_WAFER_ID[i] >= 100 ) {
				//
				if ( CURRENT_WAFER_DUMMYIN[i] > 0 ) { // 2013.12.05
					sprintf( szString , "D:BM%d:%d (%s)" , CURRENT_WAFER_DUMMYIN[i] , CURRENT_WAFER_ID[i] % 100 , STR_MEM_GET_STR( CURRENT_WAFER_NAME[i] ) );
				}
				else {
					sprintf( szString , "D%d (%s)" , CURRENT_WAFER_ID[i] % 100 , STR_MEM_GET_STR( CURRENT_WAFER_NAME[i] ) );
				}
				//
				hitm = TreeData_Sub_Insert( hWnd , hMainitm , FALSE , TRUE , szString , ( index * 10000 ) + ( i * 10 ) + 3 , 14 , 15 ); // 2012.11.29 // 2013.12.05
				//
			}
			else {
				sprintf( szString , "%d (%s)" , CURRENT_WAFER_ID[i] , STR_MEM_GET_STR( CURRENT_WAFER_NAME[i] ) );
				//
				hitm = TreeData_Sub_Insert( hWnd , hMainitm , FALSE , TRUE , szString , ( index * 10000 ) + ( i * 10 ) + 3 , 2 , 9 ); // 2012.11.29 // 2013.12.05
				//
			}
			//
			j = Get_Alarm_Wafer_Count( CURRENT_WAFER_ID[i] );
			//
			if ( j > 0 ) {
				sprintf( szString , "Alarm %d(s)" , j );
				TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , szString , ( index * 10000 ) + ( i * 10 ) + 5 , 6 , 13 ); // Wafer Alarm // 2012.11.29
			}
			//
			for ( j = 0 ; j < CURRENT_WAFER_MODULE_DEPTH[i] ; j++ ) {
				//
				if ( CURRENT_WAFER_MODULE_MODE[i][j] > TCH_PROCESS_X ) continue;
				//
				if ( TreeData_Get_Select_Slot_PM_Info( index , k , szString3 , szString2 , pmorg ) ) { // 2016.03.24
					//
					JOB_Get_Directory( index , folderstr );
					//----------------------------------------------------------------------------------------------
					Get_Log_File_Name( szString4 , "PROCESS" , 0 , folderstr , TRUE , 0 , 0 , szString3 , 0 , pmorg , "" );
					//----------------------------------------------------------------------------------------------
					//
					if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
						//
						sprintf( szString , "%s_PRE" , szString4 );
						//
						if ( File_Check_For_Exist( TRUE , szString ) ) {
							//
							if ( Get_Prm_PRE_STRING()[0] == 0 ) { // 2018.09.04
								sprintf( szString , "%s (PRE)" , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) );
							}
							else { // 2018.09.04
								sprintf( szString , "%s (%s)" , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , Get_Prm_PRE_STRING() );
							}
							//
							switch( JOB_User_Get_Fail_Data( index , 2 ) ) {
							case 1 : // fail
								hitm2 = TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , szString , ( index * 10000 ) + ( k * 10 ) + 4 , 29 , 29 );
								break;
							default :
								hitm2 = TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , szString , ( index * 10000 ) + ( k * 10 ) + 4 , 22 , 23 );
								break;
							}
							//
							sprintf( szString , "%s_PRE_STEPINFO" , szString4 );
							//
//							if ( File_Check_For_Exist( TRUE , szString ) ) { // 2018.04.23
								//
//								STINFO_StepCount = 0; // 2018.04.23
								//
								if ( Get_Step_Info_Data_From_File( szString , TRUE ) ) {
									//
									for ( S = 0 ; S < STINFO_StepCount ; S++ ) {
										//
										if ( STINFO_StepName[S][0] == 0 ) {
											sprintf( szString , "STEP %d" , STINFO_StepData[S] + 1 );
										}
										else {
											sprintf( szString , "STEP %d (%s)" , STINFO_StepData[S] + 1 , STINFO_StepName[S] );
										}
										//
										TreeData_Sub_Insert( hWnd , hitm2 , FALSE , TRUE , szString , ( S * 10000 ) + ( STINFO_StepData[S] * 10 ) + 7 , 26 , 27 ); // Step
										//
									}
									//
								}
								//
//							} // 2018.04.23
							//
						}
						//
					}
					//
					switch( JOB_User_Get_Fail_Data( index , 0 ) ) {
					case 1 : // fail
						hitm2 = TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , ( index * 10000 ) + ( k * 10 ) + 4 , 28 , 28 ); // pm
						break;
					default :
						hitm2 = TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , ( index * 10000 ) + ( k * 10 ) + 4 , 3 , 10 ); // pm
						break;
					}
					//
					sprintf( szString , "%s_STEPINFO" , szString4 );
					//
//					if ( File_Check_For_Exist( TRUE , szString ) ) { // 2018.04.23
						//
//						STINFO_StepCount = 0; // 2018.04.23
						//
						if ( Get_Step_Info_Data_From_File( szString , TRUE ) ) {
							//
							for ( S = 0 ; S < STINFO_StepCount ; S++ ) {
								//
								if ( STINFO_StepName[S][0] == 0 ) {
									sprintf( szString , "STEP %d" , STINFO_StepData[S] + 1 );
								}
								else {
									sprintf( szString , "STEP %d (%s)" , STINFO_StepData[S] + 1 , STINFO_StepName[S] );
								}
								//
								TreeData_Sub_Insert( hWnd , hitm2 , FALSE , TRUE , szString , ( S * 10000 ) + ( STINFO_StepData[S] * 10 ) + 7 , 26 , 27 ); // Step
								//
							}
							//
						}
						//
//					} // 2018.04.23
					//
					if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
						//
						sprintf( szString , "%s_POST" , szString4 );
						//
						if ( File_Check_For_Exist( TRUE , szString ) ) {
							//
							if ( Get_Prm_POST_STRING()[0] == 0 ) { // 2018.09.04
								sprintf( szString , "%s (POST)" , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) );
							}
							else { // 2018.09.04
								sprintf( szString , "%s (%s)" , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , Get_Prm_POST_STRING() );
							}
							//
							switch( JOB_User_Get_Fail_Data( index , 1 ) ) {
							case 1 : // fail
								hitm2 = TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , szString , ( index * 10000 ) + ( k * 10 ) + 4 , 30 , 30 );
								break;
							default :
								hitm2 = TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , szString , ( index * 10000 ) + ( k * 10 ) + 4 , 24 , 25 );
								break;
							}
							//
							sprintf( szString , "%s_POST_STEPINFO" , szString4 );
							//
//							if ( File_Check_For_Exist( TRUE , szString ) ) { // 2018.04.23
								//
//								STINFO_StepCount = 0; // 2018.04.23
								//
								if ( Get_Step_Info_Data_From_File( szString , TRUE ) ) {
									//
									for ( S = 0 ; S < STINFO_StepCount ; S++ ) {
										//
										if ( STINFO_StepName[S][0] == 0 ) {
											sprintf( szString , "STEP %d" , STINFO_StepData[S] + 1 );
										}
										else {
											sprintf( szString , "STEP %d (%s)" , STINFO_StepData[S] + 1 , STINFO_StepName[S] );
										}
										//
										TreeData_Sub_Insert( hWnd , hitm2 , FALSE , TRUE , szString , ( S * 10000 ) + ( STINFO_StepData[S] * 10 ) + 7 , 26 , 27 ); // Step
										//
									}
									//
								}
								//
//							} // 2018.04.23
							//
						}
					}
					//
				}
				else {
					//
					TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , STR_MEM_GET_STR( CURRENT_WAFER_MODULE_NAME[i][j] ) , ( index * 10000 ) + ( k * 10 ) + 4 , 3 , 10 ); // pm
					//
				}
				//
				k++;
				//
			}
			//
		}
	}
	//
	JOBDATA_S_DISPLAY_CONTROL( hWndPr , index , -1 , 1 , NULL ); // Wafer History
	//
	KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
	KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
	KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
	KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
	if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
	if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
	if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
	if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
	//
	if ( savedata ) Kor_JOB_Save( JOB_DATA_FILENAME ); // 2015.11.18
	//
}



void JobData_TreeView_PM_Main_Step_Insert( HWND hWnd , HTREEITEM hitm , char *filename ) { // 2018.04.24
	//
	TCHAR szString[ 256 ];
	int S;
	//
	sprintf( szString , "%s_STEPINFO" , filename );
	//
	if ( !Get_Step_Info_Data_From_File( szString , TRUE ) ) return;
	//
	for ( S = 0 ; S < STINFO_StepCount ; S++ ) {
		//
		if ( STINFO_StepName[S][0] == 0 ) {
			sprintf( szString , "STEP %d" , STINFO_StepData[S] + 1 );
		}
		else {
			sprintf( szString , "STEP %d (%s)" , STINFO_StepData[S] + 1 , STINFO_StepName[S] );
		}
		//
		TreeData_Sub_Insert( hWnd , hitm , FALSE , TRUE , szString , ( S * 10000 ) + ( STINFO_StepData[S] * 10 ) + 7 , 26 , 27 ); // Step
		//
	}
	//
}



LRESULT JobData_TreeView_Notify( HWND hWnd , HWND hWndMain , LPARAM lParam ) {
	//
	int i , j , k , index;
	HWND hWndPr;
	//
	TVITEMEX TvEx;
	//
	LPNMHDR  lpnmh;
	LPNMTREEVIEW ntv;
	//
	TCHAR szString[ 256 ];
	TCHAR szString2[ 256 ];
	TCHAR szString3[ 256 ];
	TCHAR szString4[ 256 ];
	TCHAR pmorg[ 256 ];
	//
	char folderstr[256];
	//
//	HTREEITEM hitm; // 2016.11.04
	HTREEITEM hitm2; // 2016.03.24
	NMTVDISPINFO *lp_dispinfo;
	NMTVGETINFOTIP *lp_infotip;
	//
//	BOOL savedata; // 2015.11.18 // 2016.11.04
	//
	int S , Step; // 2016.03.24
	int fp_d , fp_a; // 2016.03.24
	//
	BOOL stepview; // 2016.08.18

	lpnmh = (LPNMHDR) lParam;
	//
	hWndPr = GetParent(hWnd);
	//
	switch ( lpnmh->code ) {
	case TVN_GETINFOTIP :
		//
		lp_infotip = (NMTVGETINFOTIP *) lParam;
		//
		switch( lp_infotip->lParam % 10 ) {
		case 1 :
			//
			k = (lp_infotip->lParam) / 10000;
			//
			if ( JOB_DATA[k].START_SLOT == JOB_DATA[k].END_SLOT ) {
				switch ( Get_Info_END_TIME_SHOW_MODE() ) {
				case 1 :
					sprintf( lp_infotip->pszText , "*START : %s\n*RUN : %s\n*ELAPSED : %s\n*%s : %s\n*%s : %s\n*%s : %s\n*No : %d" ,
						JOB_DATA[ k ].START_TIME ,
						JOB_DATA[k].RUN_TIME ,
						JOB_DATA[ k ].ELAPSED_TIME ,
						Get_Prm_JID_STRING() , JOB_DATA[ k ].JOB_NAME ,
						Get_Prm_LID_STRING() , JOB_DATA[ k ].LOT_RECIPE ,
						Get_Prm_MID_STRING() , JOB_DATA[ k ].MID_NAME ,
						JOB_DATA[k].START_SLOT
					);
					break;
				case 2 :
					Get_Elapsed_String( k , szString , FALSE );
					sprintf( lp_infotip->pszText , "*START : %s\n*RUN : %s\n*ELAPSED : %s\n*%s : %s\n*%s : %s\n*%s : %s\n*No : %d" ,
						JOB_DATA[ k ].START_TIME ,
						JOB_DATA[k].RUN_TIME ,
						szString ,
						Get_Prm_JID_STRING() , JOB_DATA[ k ].JOB_NAME ,
						Get_Prm_LID_STRING() , JOB_DATA[ k ].LOT_RECIPE ,
						Get_Prm_MID_STRING() , JOB_DATA[ k ].MID_NAME ,
						JOB_DATA[k].START_SLOT
					);
					break;
				default :
					sprintf( lp_infotip->pszText , "*START : %s\n*RUN : %s\n*END : %s\n*%s : %s\n*%s : %s\n*%s : %s\n*No : %d" ,
						JOB_DATA[ k ].START_TIME ,
						JOB_DATA[k].RUN_TIME ,
						JOB_DATA[ k ].END_TIME ,
						Get_Prm_JID_STRING() , JOB_DATA[ k ].JOB_NAME ,
						Get_Prm_LID_STRING() , JOB_DATA[ k ].LOT_RECIPE ,
						Get_Prm_MID_STRING() , JOB_DATA[ k ].MID_NAME ,
						JOB_DATA[k].START_SLOT
					);
					break;
				}
			}
			else {
				switch ( Get_Info_END_TIME_SHOW_MODE() ) {
				case 1 :
					sprintf( lp_infotip->pszText , "*START : %s\n*RUN : %s\n*ELAPSED : %s\n*%s : %s\n*%s : %s\n*%s : %s\n* %d %s(s) (%d-%d)" ,
						JOB_DATA[ k ].START_TIME ,
						JOB_DATA[k].RUN_TIME ,
						JOB_DATA[ k ].ELAPSED_TIME ,
						Get_Prm_JID_STRING() , JOB_DATA[ k ].JOB_NAME ,
						Get_Prm_LID_STRING() , JOB_DATA[ k ].LOT_RECIPE ,
						Get_Prm_MID_STRING() , JOB_DATA[ k ].MID_NAME ,
						JOB_DATA[k].METHOD_COUNT , Get_Prm_METHOD_STRING() , JOB_DATA[k].START_SLOT , JOB_DATA[k].END_SLOT
					);
					break;
				case 2 :
					Get_Elapsed_String( k , szString , FALSE );
					sprintf( lp_infotip->pszText , "*START : %s\n*RUN : %s\n*ELAPSED : %s\n*%s : %s\n*%s : %s\n*%s : %s\n* %d %s(s) (%d-%d)" ,
						JOB_DATA[ k ].START_TIME ,
						JOB_DATA[k].RUN_TIME ,
						szString ,
						Get_Prm_JID_STRING() , JOB_DATA[ k ].JOB_NAME ,
						Get_Prm_LID_STRING() , JOB_DATA[ k ].LOT_RECIPE ,
						Get_Prm_MID_STRING() , JOB_DATA[ k ].MID_NAME ,
						JOB_DATA[k].METHOD_COUNT , Get_Prm_METHOD_STRING() , JOB_DATA[k].START_SLOT , JOB_DATA[k].END_SLOT
					);
					break;
				default :
					sprintf( lp_infotip->pszText , "*START : %s\n*RUN : %s\n*END : %s\n*%s : %s\n*%s : %s\n*%s : %s\n* %d %s(s) (%d-%d)" ,
						JOB_DATA[ k ].START_TIME ,
						JOB_DATA[k].RUN_TIME ,
						JOB_DATA[ k ].END_TIME ,
						Get_Prm_JID_STRING() , JOB_DATA[ k ].JOB_NAME ,
						Get_Prm_LID_STRING() , JOB_DATA[ k ].LOT_RECIPE ,
						Get_Prm_MID_STRING() , JOB_DATA[ k ].MID_NAME ,
						JOB_DATA[k].METHOD_COUNT , Get_Prm_METHOD_STRING() , JOB_DATA[k].START_SLOT , JOB_DATA[k].END_SLOT
					);
					break;
				}
			}
			break;
		}
		break;
	case TVN_GETDISPINFO:
		//
		lp_dispinfo = (NMTVDISPINFO *) lParam;
		//
		switch( lp_dispinfo->item.lParam % 10 ) {
		case 0 :			
			lstrcpy( lp_dispinfo->item.pszText , "Job List" );
			//
			break;
		case 1 :
			//
			k = (lp_dispinfo->item.lParam) / 10000;
			//
			i = Get_Prm_TREE_ITEM_DISPLAY();
			//
			if ( i == 0 ) {
				if ( NAME_MANUAL_to_TIME ) { // 2016.04.28
					//
					if ( STRCMP_L( JOB_DATA[ k ].JOB_NAME , "-MANUAL-" ) ) {
						if ( STRCMP_L( JOB_DATA[ k ].START_TIME , "" ) ) {
							lstrcpy( lp_dispinfo->item.pszText , JOB_DATA[ k ].JOB_NAME );
						}
						else {
							lstrcpy( lp_dispinfo->item.pszText , JOB_DATA[ k ].START_TIME );
						}
					}
					else {
						lstrcpy( lp_dispinfo->item.pszText , JOB_DATA[ k ].JOB_NAME );
					}
					//
				}
				else {
					lstrcpy( lp_dispinfo->item.pszText , JOB_DATA[ k ].JOB_NAME );
				}
			}
			else {
				//
				strcpy( szString , "" );
				//
				while( TRUE ) {
					//
					if ( i == 0 ) break;
					//
					j = i % 10;
					//
					i = i / 10;
					//
					switch( j ) {
					case 1 : // jobname
						if ( JOB_DATA[ k ].JOB_NAME[0] == 0 ) break;
						if ( szString[0] != 0 ) strcat( szString , " " );
						strcat( szString , JOB_DATA[ k ].JOB_NAME );
						break;
					case 2 : // lotrecipe
						if ( JOB_DATA[ k ].LOT_RECIPE[0] == 0 ) break;
						if ( szString[0] != 0 ) strcat( szString , " " );
						strcat( szString , JOB_DATA[ k ].LOT_RECIPE );
						break;
					case 3 : // mid
						if ( JOB_DATA[ k ].MID_NAME[0] == 0 ) break;
						if ( szString[0] != 0 ) strcat( szString , " " );
						strcat( szString , JOB_DATA[ k ].MID_NAME );
						break;
					case 4 : // starttime
						if ( JOB_DATA[ k ].START_TIME[0] == 0 ) break;
						if ( szString[0] != 0 ) strcat( szString , " " );
						strcat( szString , JOB_DATA[ k ].START_TIME );
						break;
					case 5 : // runtime
						if ( JOB_DATA[ k ].RUN_TIME[0] == 0 ) break;
						if ( szString[0] != 0 ) strcat( szString , " " );
						strcat( szString , JOB_DATA[ k ].RUN_TIME );
						break;
					case 6 : // endtime
						if ( JOB_DATA[ k ].END_TIME[0] != 0 ) break;
						if ( szString[0] != 0 ) strcat( szString , " " );
						strcat( szString , JOB_DATA[ k ].END_TIME );
						break;
					case 7 : // startslot
						if ( szString[0] != 0 ) strcat( szString , " " );
						sprintf( szString2 , "%d" , JOB_DATA[k].START_SLOT );
						strcat( szString , szString2 );
						break;
					case 8 : // slot range
						if ( szString[0] != 0 ) strcat( szString , " " );
						sprintf( szString2 , "%d-%d(%d)" , JOB_DATA[k].START_SLOT , JOB_DATA[k].END_SLOT , JOB_DATA[k].METHOD_COUNT );
						strcat( szString , szString2 );
						break;
					}
				}
				lstrcpy( lp_dispinfo->item.pszText , szString );
				//
			}
			break;
		case 9 :
			lstrcpy( lp_dispinfo->item.pszText , "Fail..." );
			break;
		default :
			lstrcpy( lp_dispinfo->item.pszText , "?..." );
			break;
		}
		break;


	case NM_RCLICK : // 2017.02.04
		TvEx.hItem = TreeView_GetSelection( hWnd );
		TvEx.mask = TVIF_PARAM;
		TreeView_GetItem( hWnd , &TvEx );

		if ( ( TvEx.lParam % 10 ) == 4 ) {
			//
			CURRENT_WAFER_INIT( 0 );
			//
			index = TvEx.lParam / 10000;
			j     = (TvEx.lParam % 10000 ) / 10;
			//
			if ( TreeData_Get_Select_Slot_PM_Info( index , j , szString3 , szString2 , pmorg ) ) {
				//
				JOB_Get_Directory( index , folderstr );
				//----------------------------------------------------------------------------------------------
				Get_Log_File_Name( szString  , "PROCESS" ,  0 , folderstr , TRUE , 0 , 0 , szString3 , 0 , pmorg , "" );
				Get_Log_File_Name( szString4 , "PROCESS" , 10 , folderstr , TRUE , 0 , 0 , szString3 , 0 , pmorg , "" );
				//----------------------------------------------------------------------------------------------
				//
				if      ( TvEx.iImage == 22 ) {
					strcat( szString , "_PRE" );
					strcat( szString4 , "_PRE" );
				}
				else if ( TvEx.iImage == 24 ) {
					strcat( szString , "_POST" );
					strcat( szString4 , "_POST" );
				}
				//
				COMP_EDITING( hWnd , hWndMain , index , szString3 , szString4 , szString , szString2 , 0 );
				//
			}
			else {
				//
				COMP_EDITING( hWnd , hWndMain , -1 , "" , "" , "" , "" , 0 );
				//
			}
		}
		break;











/*
//
case NM_CLICK : // 2016.11.04
		//
		//
		TvEx.hItem = TreeView_GetSelection( hWnd );
		TvEx.mask = TVIF_PARAM;
		TreeView_GetItem( hWnd , &TvEx );
		//
		if ( ( TvEx.lParam % 10 ) != 8 ) break;
		//
		hitm2 = TreeView_GetParent( hWnd , TvEx.hItem );
		//
		if ( hitm2 == NULL ) break;
		//
		TvEx.mask = TVIF_PARAM | TVIF_IMAGE;
		TvEx.hItem = hitm2;
		//
		if ( !TreeView_GetItem( hWnd , &TvEx ) ) break;
		//
		TREEVIEW_LAST_PARAM = TvEx.lParam;
		//
		index = TvEx.lParam / 10000;
		//
		j = JOB_Get_LoopCount( index , &i );
		//
		if ( LogView_Change_Loop_Sel_Count( hWnd , index , j , i ) ) {
			JobData_TreeView_Job_Insert( hWnd , hWndPr , index , TvEx.hItem );
		}
		//
		break;
*/
	case NM_SETFOCUS :
		//
		TvEx.hItem = TreeView_GetSelection( hWnd );
		TvEx.mask = TVIF_PARAM;
		TreeView_GetItem( hWnd , &TvEx );
		//
		if ( TREEVIEW_LAST_PARAM == TvEx.lParam ) {
			//
			if ( IsWindowVisible( GetDlgItem( hWndPr , IDC_LIST_JOBDATA_S2 ) ) ) {
				//
				if ( ( TREEVIEW_LAST_PARAM % 10 ) == 0 ) {
					//
					if ( !LOG_OPEN_TERMINATE() ) break; // 2014.02.05
					//
					KWIN_Item_Show(  hWndPr , IDC_LIST_JOBDATA );
					//
					KWIN_Item_Hide(  hWndPr , IDC_LIST_JOBDATA_S2 );
					KWIN_Item_Hide(  hWndPr , IDC_LIST_JOBDATA_S2S );
//					KWIN_Item_Hide(  hWndPr , IDC_LIST_JOBDATA_SPS ); // 2014.02.10 // 2014.07.17
					if ( Get_Prm_STATUS_VIEW() ) KWIN_Item_String_Display( hWndPr , IDC_LIST_JOBDATA_SPS , "" ); // 2014.07.17
					//
					KWIN_Item_Hide(  hWndPr , IDC_LIST_JOBDATA_S );
					KWIN_Item_Hide(  hWndPr , IDC_LIST_JOBDATA_SS );
					//
					KWIN_Item_Hide(  hWndPr , IDC_LIST_JOBDATA_SPM );	KWIN_Item_Hide(  hWndPr , IDC_LIST_JOBDATA_SPMS ); 	KWIN_Item_Hide(  hWndPr , IDC_CTRL_SPM );	// 2016.08.17
					IDC_CTRL_SPM_MODE = 0;
				}
				else {
					JOBDATA_S_DISPLAY_CONTROL( hWndPr , -1 , -1 , -1 , NULL ); // First
				}
			}
		}
		break;

	case TVN_ITEMEXPANDED :
		//
		ntv = (LPNMTREEVIEW)lParam;
		//
		TvEx.mask = TVIF_PARAM | TVIF_IMAGE;
		TvEx.hItem = ntv->itemNew.hItem;
		TreeView_GetItem( hWnd , &TvEx );
		//
		switch( TvEx.lParam % 10 ) {
		case 0 : // [ROOT]
			//
			KWIN_Item_Show( hWndPr , IDC_LIST_JOBDATA );
			//
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_S );
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_SS );
			//
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_S2 );
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_S2S );
			//
			KWIN_Item_Hide( hWndPr , IDC_CTRL_SPM );
			IDC_CTRL_SPM_MODE = 0;
			//
			break;
		}
		break;

/*	case TVN_ITEMEXPANDING :
		//
		ntv = (LPNMTREEVIEW)lParam;
		//
		TvEx.mask = TVIF_PARAM | TVIF_IMAGE;
		TvEx.hItem = ntv->itemNew.hItem;
		TreeView_GetItem( hWnd , &TvEx );
		//
		printf( "TVN_ITEMEXPANDING[%d]\n" , TvEx.lParam % 10 );
		break;
*/

	case TVN_SELCHANGED:
		//
		ntv = (LPNMTREEVIEW)lParam;
		//
//		TvEx.mask = TVIF_PARAM; // 2016.03.24
		TvEx.mask = TVIF_PARAM | TVIF_IMAGE; // 2016.03.24
		TvEx.hItem = ntv->itemNew.hItem;
		TreeView_GetItem( hWnd , &TvEx );
		//
		if ( !LOG_OPEN_TERMINATE() ) break; // 2014.02.05
		//
		TREEVIEW_LAST_PARAM = TvEx.lParam;
		//
		switch( TvEx.lParam % 10 ) {
		case 0 : // [ROOT]
			//
			KWIN_Item_Show( hWndPr , IDC_LIST_JOBDATA );
			//
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_S );
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_SS );
			KWIN_Item_Hide( hWndPr , IDC_CTRL_SPM );
			IDC_CTRL_SPM_MODE = 0;
			//
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_S2 );
			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_S2S );
//			KWIN_Item_Hide( hWndPr , IDC_LIST_JOBDATA_SPS ); // 2014.02.10 // 2014.07.17
			if ( Get_Prm_STATUS_VIEW() ) KWIN_Item_String_Display( hWndPr , IDC_LIST_JOBDATA_SPS , "" ); // 2014.07.17
			//
			if ( JobData_ListView_SelectMode( GetDlgItem( hWndPr , IDC_LIST_JOBDATA ) , FALSE ) ) {
				KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
				KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
				KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
				KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
				if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
				if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
				if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
				if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
			}
			else {
				KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG1 );
				KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG2 );
				KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG3 );
				KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG4 );
				if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG5 );
				if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG6 );
				if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG7 );
				if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Disable( hWndPr , IDC_CTRL_LOG8 );
			}
			//
			break;
		case 1 : // [JOB LIST]
			//
			JobData_TreeView_Job_Insert( hWnd , hWndPr , TvEx.lParam / 10000 , TvEx.hItem ); // 2016.11.04
			//
			break;

		case 2 : // Alarm ALL
			//
			index = TvEx.lParam / 10000;
			j     = (TvEx.lParam % 10000 ) / 10;
			//
			JOB_Get_Directory( index , folderstr );
			//----------------------------------------------------------------------------------------------
			sprintf( szString , "%s/LotLog/%s/LotData.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
			//----------------------------------------------------------------------------------------------
			//
			LogView_Open_Lot_History_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , 1 , 0 , szString , szString2 , index , TRUE );
			//
			JOBDATA_S_DISPLAY_CONTROL( hWndPr , index , -1 , -1 , NULL ); // ALARM
			//
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
			if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
			if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
			if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
			if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
			//
			break;

		case 3 : // Wafer Info
			//
			index = TvEx.lParam / 10000;
			j     = (TvEx.lParam % 10000 ) / 10;
			//
			CURRENT_WAFER_INIT( -1 );
			//
			LogView_Open_Method_History_Init( GetDlgItem( hWndPr , IDC_LIST_JOBDATA_S ) , 1 );
			//
			LogView_Open_Method_History_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , TRUE , index , 0 , CURRENT_WAFER_ID[j] , j ); // 2012.11.29
			//
			JOBDATA_S_DISPLAY_CONTROL( hWndPr , index , -1 , -1 , NULL ); // Wafer Info
			//
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
			if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
			if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
			if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
			if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
			//
			break;
		case 4 : // PM Info
			//
//printf( "START\n" );
			CURRENT_WAFER_INIT( 0 );
//printf( "START2\n" );
			//
			stepview = FALSE;
			//
			index = TvEx.lParam / 10000;
			j     = (TvEx.lParam % 10000 ) / 10;
			//
//printf( "A [%d][%d]\n" , index , j );
			if ( TreeData_Get_Select_Slot_PM_Info( index , j , szString3 , szString2 , pmorg ) ) {
				//
				JOB_Get_Directory( index , folderstr );
				//----------------------------------------------------------------------------------------------
				Get_Log_File_Name( szString , "PROCESS" , 0 , folderstr , TRUE , 0 , 0 , szString3 , 0 , pmorg , "" );
				//----------------------------------------------------------------------------------------------
				//
				if      ( TvEx.iImage == 22 ) { // PRE // 2016.03.24
					strcat( szString , "_PRE" );
				}
				else if ( TvEx.iImage == 24 ) { // POST // 2016.03.24
					strcat( szString , "_POST" );
				}
				//
				stepview = PROCESSDATA_HAS_STEPVIEW( index , j , -1 );
				//

//printf( "B [%s][%s] [%d]\n" , szString , szString2 , stepview );
				//----------------------------------------------------------------------------------------------
//				if ( !LogView_Open_Process_Log_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , szString , szString2 , FALSE , 0 , 0 , TRUE , stepview ) ) break;
				if ( !LogView_Open_Process_Log_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , szString , szString2 , FALSE , 0 , 0 , 0 , TRUE , stepview ) ) {
//printf( "C [%s][%s] [%d]\n" , szString , szString2 , stepview );
					break;
				}
//printf( "D [%s][%s] [%d]\n" , szString , szString2 , stepview );
				//----------------------------------------------------------------------------------------------


				//
				// 2018.04.24
				//
				if ( NULL == TreeView_GetChild( hWnd , TvEx.hItem ) ) {

					JobData_TreeView_PM_Main_Step_Insert( hWnd , TvEx.hItem , szString ); // 2018.04.24







				}
				//
				//
				//



			}
			//
//printf( "E [%d][%d]\n" , index , j );
			JOBDATA_S_DISPLAY_CONTROL( hWndPr , index , stepview ? 0 : -1 , j , NULL ); // Process(NotStep) *
			//
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
			//
			if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
			if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
			if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
			if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
			//
			break;

		case 5 : // Alarm Wafer
			//
			index = TvEx.lParam / 10000;
			j     = (TvEx.lParam % 10000 ) / 10;
			//
			JOB_Get_Directory( index , folderstr );
			//----------------------------------------------------------------------------------------------
			sprintf( szString , "%s/LotLog/%s/LotData.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
			//----------------------------------------------------------------------------------------------
			//
//			LogView_Open_Lot_History_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , 1 , j , szString , szString2 ); // 2012.11.29
			LogView_Open_Lot_History_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , 1 , CURRENT_WAFER_ID[j] , szString , szString2 , index , TRUE ); // 2012.11.29
			//
			JOBDATA_S_DISPLAY_CONTROL( hWndPr , index , -1 , -1 , NULL ); // Alarm Wafer
			//
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
			if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
			if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
			if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
			if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
			//
			break;

		case 6 : // User File
			//
			index = TvEx.lParam / 10000;
			j     = (TvEx.lParam % 10000 ) / 10;
			//
			JOB_Get_Directory( index , folderstr );
			//----------------------------------------------------------------------------------------------
			sprintf( szString , "%s/LotLog/%s/User.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
			//----------------------------------------------------------------------------------------------
			//
			LogView_Open_Lot_User_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , szString , szString2 , TRUE );
			//
			JOBDATA_S_DISPLAY_CONTROL( hWndPr , index , -1 , -1 , NULL ); // UserLog
			//
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
			if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
			if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
			if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
			if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
			//
			break;

		case 7 : // step 2016.03.25
			//
			S    = TvEx.lParam / 10000;
			Step = (TvEx.lParam % 10000 ) / 10;
			//
			hitm2 = TreeView_GetParent( hWnd , TvEx.hItem );
			//
			if ( hitm2 == NULL ) break;
			//
			TvEx.mask = TVIF_PARAM | TVIF_IMAGE;
			TvEx.hItem = hitm2;
			//
			if ( !TreeView_GetItem( hWnd , &TvEx ) ) break;
			//
			TREEVIEW_LAST_PARAM = TvEx.lParam;
			//
			index = TvEx.lParam / 10000;
			j     = (TvEx.lParam % 10000 ) / 10;
			//
//			printf( "Index=%d , pm = %d , S , %d , Step = %d\n" , index , j , S , Step );
			//
			CURRENT_WAFER_INIT( 0 );
			//
			stepview = FALSE;
			//
			if ( TreeData_Get_Select_Slot_PM_Info( index , j , szString3 , szString2 , pmorg ) ) {
				//
				JOB_Get_Directory( index , folderstr );
				//----------------------------------------------------------------------------------------------
				Get_Log_File_Name( szString , "PROCESS" , 0 , folderstr , TRUE , 0 , 0 , szString3 , 0 , pmorg , "" );
				//----------------------------------------------------------------------------------------------
				//
				if      ( TvEx.iImage == 22 ) { // PRE
					strcat( szString , "_PRE" );
				}
				else if ( TvEx.iImage == 24 ) { // POST
					strcat( szString , "_POST" );
				}
				//
				sprintf( szString4 , "%s_STEPINFO" , szString );
				//
				if ( Get_Step_Info_Pointer_From_File( szString4 , S , Step , &fp_d , &fp_a ) ) {
					//
					stepview = PROCESSDATA_HAS_STEPVIEW( index , j , Step );
					//
					//----------------------------------------------------------------------------------------------
					if ( !LogView_Open_Process_Log_Data_Set( hWndPr , IDC_LIST_JOBDATA_S , szString , szString2 , TRUE , Step , fp_d , fp_a , TRUE , stepview ) ) break;
					//----------------------------------------------------------------------------------------------
				}
				//
			}
			//
			JOBDATA_S_DISPLAY_CONTROL( hWndPr , index , stepview ? Step + 1 : -1 , j , NULL ); // Process(Step) *
			//
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG1 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG2 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG3 );
			KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG4 );
			//
			if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG5 );
			if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG6 );
			if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG7 );
			if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( hWndPr , IDC_CTRL_LOG8 );
			//
			break;

		case 8 : // LoopCount 2016.11.04
			//
			index = TvEx.lParam / 10000;
			//
			j = JOB_Get_LoopCount( index , &i );
			//
			if ( LogView_Change_Loop_Sel_Count( hWnd , index , j , i ) ) {
				JobData_TreeView_Job_Insert( hWnd , hWndPr , index , TreeView_GetParent( hWnd , TvEx.hItem ) );
			}
			//
			break;

		default :
			break;
		}
		//
		JobData_Button_Check( hWnd );
		//
		break;
	}
	return 0;
}

LRESULT JobData_ListView_Notify( HWND hWnd , HWND hWndTree , LPARAM lParam ) {
	int index , pos;
//	int last;
	TCHAR szString[ 256 ];

	LPNMHDR  lpnmh = (LPNMHDR) lParam;
	LV_DISPINFO *lpdi = (LV_DISPINFO *)lParam;
	//
	if ( lpnmh->hwndFrom != hWnd ) return 0;
	//
	switch( lpnmh->code ) {
		case LVN_GETDISPINFO:
				if ( lpdi->item.iSubItem ) {
					if ( lpdi->item.mask & LVIF_TEXT ) {
						//
//						if ( Get_Prm_FIND_MODE() ) { // 2009.07.20
//							index = FindData_Conv_ListView_to_JobIndex( lpdi->item.iItem ); // 2009.07.20
//						} // 2009.07.20
//						else { // 2009.07.20
//							index = JobData_Conv_ListView_to_JobIndex( lpdi->item.iItem ); // 2009.07.20
//						} // 2009.07.20
						//
//						index = lpdi->item.lParam / 10000; // 2009.07.20	// 2009.11.20
//						index = JOB_Get_Current_Count() - (lpdi->item.lParam / 10000) - 1; // 2009.11.20 // 2011.06.20
						index = lpdi->item.lParam / 10000; // 2009.11.20 // 2011.06.20
						//
						JOB_Display_Data_Area( index , lpdi->item.iSubItem , szString , lpdi->item.lParam );
						lstrcpy( lpdi->item.pszText , szString );
					}
				}
				else {
					if ( lpdi->item.mask & LVIF_TEXT ) {
/*
// 2011.06.20
						if ( Get_Prm_FIND_MODE() ) {
							index = JOB_FIND_Get_Current_Count();
						}
						else {
							index = JOB_Get_Current_Count();
						}
//						wsprintf( szString, "%4d", index - lpdi->item.iItem ); // 2009.09.04
						wsprintf( szString, "%4d", index - ( lpdi->item.lParam / 10000 ) ); // 2009.09.04
*/

						wsprintf( szString, "%4d", ( lpdi->item.lParam / 10000 ) + 1 ); // 2011.06.20
						lstrcpy( lpdi->item.pszText , szString );
					}
					if ( lpdi->item.mask & LVIF_IMAGE ) {
						lpdi->item.iImage = 0;
					}
				}
			return 0;

		case NM_DBLCLK :
			//
			if ( hWndTree != NULL ) {
				//
				if ( ListView_GetSelectedCount( hWnd ) == 1 ) {
					//
					pos = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED );
					//
					if ( pos < 0 ) {
						InvalidateRect( hWnd , NULL , FALSE );
					}
					else {
						index = JobData_Conv_ListView_to_JobIndex( hWnd , pos );
						TreeData_Select_Index( hWndTree , index );
					}
				}
				else {
					InvalidateRect( hWnd , NULL , FALSE );
				}
			}
			else {
				InvalidateRect( hWnd , NULL , FALSE );
			}
			return 0;

		case LVN_ITEMCHANGING:
			return 0;

		case LVN_ITEMCHANGED :
			//
			if ( Get_Prm_DISPLAY_MODE() == 1 ) { // 2011.06.15
				//
				CURRENT_WAFER_INIT( -1 );
				//
				LogView_Open_Method_History_Init( GetDlgItem( GetParent(hWnd) , IDC_LIST_JOBDATA_S ) , 1 );
				//
				if ( JobData_ListView_SelectMode( hWnd , FALSE ) ) {
					//
					pos = -1;
					//
					while ( TRUE ) {
						//
						pos = ListView_GetNextItem( hWnd , pos , LVNI_SELECTED );
						//
						if ( pos < 0 ) break;
						//
						index = LogView_GetParam(hWnd,pos) / 10000;
						//
						LogView_Open_Method_History_Data_Set( GetParent(hWnd) , IDC_LIST_JOBDATA_S , FALSE , index , 0 , -1 , -1 );
						//
					}
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG1 );
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG2 );
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG3 );
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG4 );
				}
				else {
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG1 );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG2 );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG3 );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG4 );
				}
				KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_LOG5 );
				KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_LOG6 );
				KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_LOG7 );
				KWIN_Item_Hide( GetParent(hWnd) , IDC_CTRL_LOG8 );
			}
			else if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.06.15
				if ( JobData_ListView_SelectMode( hWnd , FALSE ) ) {
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG1 );
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG2 );
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG3 );
					KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG4 );
					if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG5 );
					if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG6 );
					if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG7 );
					if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Enable( GetParent(hWnd) , IDC_CTRL_LOG8 );
				}
				else {
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG1 );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG2 );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG3 );
					KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG4 );
					if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG5 );
					if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG6 );
					if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG7 );
					if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Disable( GetParent(hWnd) , IDC_CTRL_LOG8 );
				}
			}
			//
			JobData_Button_Check( hWnd );
			//
			return 0;

		case LVN_KEYDOWN :
			return 0;

		case LVN_ODCACHEHINT: {
				LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT) lParam;
			}
			return 0;

		case LVN_ODFINDITEM: {
				LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM) lParam;
			}
			return 0;

		case LVN_INSERTITEM:			return 0;
		case LVN_DELETEITEM:			return 0;
		case LVN_DELETEALLITEMS:		return 0;
		case LVN_BEGINLABELEDITA:		return 0;
		case LVN_BEGINLABELEDITW:		return 0;
		case LVN_ENDLABELEDITA:			return 0;
		case LVN_ENDLABELEDITW:			return 0;

		case LVN_COLUMNCLICK: { // 2009.09.04
				NM_LISTVIEW *pNM = (NM_LISTVIEW*) lParam;
				//
				if ( Get_Prm_SORT_MODE() ) {
					//
					if ( _JobData_ListView_SortDir[ pNM->iSubItem ] == 0 )
						_JobData_ListView_SortDir[ pNM->iSubItem ] = 1;
					else
						_JobData_ListView_SortDir[ pNM->iSubItem ] = 0;
					//
					_JobData_ListView_SortHwnd = hWnd;
					//
					ListView_SortItems( hWnd , CallBack_JobData_ListView_Sort , pNM->iSubItem );
					//
				}
			}
			return 0;

		case LVN_BEGINDRAG:				return 0;
		case LVN_BEGINRDRAG:			return 0;
		case LVN_ODFINDITEMW:			return 0;
		case LVN_ITEMACTIVATE:			return 0;
		case LVN_ODSTATECHANGED:		return 0;
		case LVN_HOTTRACK:				return 0;
		case LVN_GETDISPINFOW:			return 0;
		case LVN_SETDISPINFOA:			return 0;
		case LVN_SETDISPINFOW:			return 0;
		case LVN_MARQUEEBEGIN :			return 0;
		case NM_CLICK :					return 0;
		case NM_RELEASEDCAPTURE :		return 0;
		case NM_SETFOCUS :				return 0;
		case NM_CUSTOMDRAW :			return 0;
		default :						return 0;
	}
	return 0;
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL DirData_ListView_Init( HWND hWnd ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	BOOL Find;
	char Buffer[256];
	int i;
	sprintf( Buffer , "%s/*.*" , Get_Prm_MAIN_RECIPE_PATH() );
	ListBox_ResetContent( hWnd );
	hFind = FindFirstFile( Buffer , &fd );
	while ( (hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) {
			if ( ( strcmp( fd.cFileName , "." ) != 0 ) &&
				( strcmp( fd.cFileName , ".." ) != 0 ) ) {
				Find = FALSE;
				if ( STRCMP_L( fd.cFileName , Get_Prm_LOT_RECIPE_PATH() ) ) Find = TRUE;
				if ( !Find ) {
					if ( STRCMP_L( fd.cFileName , Get_Prm_CLUSTER_RECIPE_PATH() ) ) Find = TRUE;
				}
				if ( !Find ) {
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						if ( STRCMP_L( fd.cFileName , Get_Prm_PROCESS_RECIPE_HEAD_PATH( i + PM1 ) ) ) {
							Find = TRUE;
							break;
						}
					}
				}
				if ( !Find ) ListBox_AddString( hWnd , fd.cFileName );
			}
		}
		bRet = FindNextFile( hFind , &fd );
	}
	FindClose( hFind );
	ListBox_SetCurSel( hWnd , -1 );
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL FileData_ListView_Init( HWND hWnd , char *data ) {
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	char Buffer[256];
	sprintf( Buffer , "%s/%s/%s/*.*" , Get_Prm_MAIN_RECIPE_PATH() , Get_Prm_GROUP_RECIPE_PATH() , data );
	ListBox_ResetContent( hWnd );
	hFind = FindFirstFile( Buffer , &fd );
	while ( (hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			ListBox_AddString( hWnd , fd.cFileName );
		}
		bRet = FindNextFile( hFind , &fd );
	}
	FindClose( hFind );
	ListBox_SetCurSel( hWnd , -1 );
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_Parameter_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int i , j , dm , ad , es , l , pr , dc;
	static BOOL chg;
	char Buffer[64];
	char Buffer2[8];
	int x;
	switch ( msg ) {
		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			dm = Get_Info_DISPLAY_MODE();
			i = Get_Info_LOG_SAVE_MONTH() - 1;
			j = Get_Info_DUAL_LOT_MODE();
			ad = Get_Info_AUTO_DELETE_MODE();
			es = Get_Info_END_TIME_SHOW_MODE();
			pr = Get_Info_PRE_POST_SHOW_MODE(); // 2006.02.23
			dc = Get_Info_DELETE_COUNT(); // 2014.10.15
			//
			chg = FALSE;
			KWIN_Item_String_Display( hdlg , IDC_STATIC_FILEID , Get_Prm_FILE_ID() );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_LOTID , Get_Prm_LOT_RECIPE_ID() );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_LOTPATH , Get_Prm_LOT_RECIPE_PATH() );
			KWIN_Item_Int_Display( hdlg , IDC_STATIC_MAXCASS , Get_Prm_MAX_CASSETTE_SLOT() );
			strcpy( Buffer , "" );
			for ( x = 0 ; x < MAX_SIDE ; x++ ) {
				if ( !Get_Prm_EXCLUSIVE_JOB_CHECK( x ) ) {
					sprintf( Buffer2 , "%d " , x + 1 );
					strcat( Buffer , Buffer2 );
				}
			}
			KWIN_Item_String_Display( hdlg , IDC_STATIC_CHECKSIDE , Buffer );
		
			if ( !j )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DUALLOT , "Disable" );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DUALLOT , "Enable" );

			if      ( dm == 1 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "Minimum" );
			else if ( dm == 2 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "TimeBased" );
			else if ( dm == 3 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "Time/Min" );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "Standard" );

			if ( ad == 0 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_AUTODELETEMODE , "Log & Info All" );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_AUTODELETEMODE , "Log File Only" );
			if ( es == 1 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_ENDTIMESHOWMODE , "Elapsed Time" );
			else if ( es == 2 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_ENDTIMESHOWMODE , "Elapsed Time(2)" );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_ENDTIMESHOWMODE , "End Time" );
			//
			if      ( pr == 0 ) // 2006.02.23
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "N/A" );
			else if ( pr == 1 ) // 2006.02.23
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Pre" );
			else if ( pr == 2 ) // 2006.02.23
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Post" );
			else if ( pr == 3 ) // 2007.06.25
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Pre/Post" );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Pre/Post(+Lot)" );
			//
			// 2014.10.15
			if      ( dc == 0 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "Default" );
			else if ( dc == 1 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "90" );
			else if ( dc == 2 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "80" );
			else if ( dc == 3 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "70" );
			else if ( dc == 4 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "60" );
			else if ( dc == 5 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "50" );
			else if ( dc == 6 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "40" );
			else if ( dc == 7 ) // 2017.03.13
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "30" );
			else if ( dc == 8 ) // 2017.03.13
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "20" );
			else // 2017.03.13
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "10" );
			//
			KWIN_Item_Int_Display( hdlg , IDC_BUTTON_DELETEDATE , Get_Info_LOG_SAVE_MONTH() );
			//
			KWIN_Item_Hide( hdlg , IDOK );
			//
			if ( STEP_VIEW_RUN_EDITING() ) {
				KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW );
			}
			//
			return TRUE;
			
		case WM_COMMAND :
			switch( wParam ) {
				//
				case IDC_BUTTON_STEPVIEW : // 2016.10.11
					//
					EndDialog( hdlg , 1 );
					//
					return TRUE;

				case IDC_BUTTON_DELETEDATE :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Month" , "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36" , &i ) ) {
						chg = TRUE;
						//
						KWIN_Item_Int_Display( hdlg , IDC_BUTTON_DELETEDATE , i + 1 );
						//
						KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW ); // 2016.10.11
						//
						KWIN_Item_Show( hdlg , IDOK );
					}
					return TRUE;

				case IDC_BUTTON_DUALLOT :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Multiple Run" , "Disable|Enable" , &j ) ) {
						chg = TRUE;
						//
						if ( !j )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DUALLOT , "Disable" );
						else
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DUALLOT , "Enable" );
						//
						KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW ); // 2016.10.11
						//
						KWIN_Item_Show( hdlg , IDOK );
					}
					return TRUE;

				case IDC_BUTTON_DISPLAYMODE :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Standard|Minimum|TimeBased|Time/Min" , &dm ) ) {
						chg = TRUE;
						//
						if      ( dm == 1 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "Minimum" );
						else if ( dm == 2 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "TimeBased" );
						else if ( dm == 3 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "Time/Min" );
						else
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DISPLAYMODE , "Standard" );
						//
						KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW ); // 2016.10.11
						//
						KWIN_Item_Show( hdlg , IDOK );
					}
					return TRUE;

				case IDC_BUTTON_AUTODELETEMODE :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Log & Infor All|Log File Only" , &ad ) ) {
						chg = TRUE;
						//
						if ( ad == 0 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_AUTODELETEMODE , "Log& Info All" );
						else
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_AUTODELETEMODE , "Log File Only" );
						//
						KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW ); // 2016.10.11
						//
						KWIN_Item_Show( hdlg , IDOK );
					}
					return TRUE;

				case IDC_BUTTON_ENDTIMESHOWMODE :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "End Time|Elapsed TIme|Elapsed TIme2" , &es ) ) {
						chg = TRUE;
						//
						if ( es == 1 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_ENDTIMESHOWMODE , "Elapsed Time" );
						else if ( es == 2 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_ENDTIMESHOWMODE , "Elapsed Time(2)" );
						else
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_ENDTIMESHOWMODE , "End Time" );
						//
						KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW ); // 2016.10.11
						//
						KWIN_Item_Show( hdlg , IDOK );
					}
					return TRUE;

				case IDC_BUTTON_PREPOSTSHOWMODE :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "N/A|Pre Log Data File Check|Post Log Data File Check|Pre/Post Log Data File Check|Pre/Post/LotPre/LotPost Log Data File Check" , &pr ) ) {
						chg = TRUE;
						//
						if      ( pr == 0 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "N/A" );
						else if ( pr == 1 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Pre" );
						else if ( pr == 2 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Post" );
						else if ( pr == 3 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Pre/Post" );
						else
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_PREPOSTSHOWMODE , "Pre/Post(+Lot)" );
						//
						KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW ); // 2016.10.11
						//
						KWIN_Item_Show( hdlg , IDOK );
					}
					return TRUE;

				case IDC_BUTTON_DELETECOUNT : // 2014.10.15
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Count" , "100(Default)|90|80|70|60|50|40|30|20|10" , &dc ) ) {
						chg = TRUE;
						//
						if      ( dc == 0 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "Default" );
						else if ( dc == 1 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "90" );
						else if ( dc == 2 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "80" );
						else if ( dc == 3 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "70" );
						else if ( dc == 4 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "60" );
						else if ( dc == 5 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "50" );
						else if ( dc == 6 )
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "40" );
						else if ( dc == 7 ) // 2017.03.13
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "30" );
						else if ( dc == 8 ) // 2017.03.13
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "20" );
						else // 2017.03.13
							KWIN_Item_String_Display( hdlg , IDC_BUTTON_DELETECOUNT , "10" );
						//
						KWIN_Item_Disable( hdlg , IDC_BUTTON_STEPVIEW ); // 2016.10.11
						//
						KWIN_Item_Show( hdlg , IDOK );
					}
					return TRUE;

				case IDOK :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Edit Impossible(Read Only Mode)" , "Edit Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( chg ) {
						//--------------------------------------------
						// 2010.10.27 jiahn Begin ( Required by T ) 
						// Comment - Save to File month setting ("Job_Editor_Info.inf")
						Set_Info_LOG_SAVE_MONTH( i + 1 ); 
//						write_JobLog_Auto_Delete_Time(); // 2010.11.18
						// 2010.10.27 jiahn End ( Required by T ) 
						//--------------------------------------------
						Set_Info_DUAL_LOT_MODE( j );
						Set_Info_AUTO_DELETE_MODE( ad );
						Set_Info_PRE_POST_SHOW_MODE( pr ); // 2006.02.23
						Set_Info_DELETE_COUNT( dc ); // 2014.10.15
						//
						if ( ( dm != Get_Info_DISPLAY_MODE() ) || ( es != Get_Info_END_TIME_SHOW_MODE() ) ) {
							Set_Info_DISPLAY_MODE( dm );
							Set_Info_END_TIME_SHOW_MODE( es );
							SendMessage( GetParent( hdlg ) , WM_COMMAND , IDYES , (LPARAM) NULL );
						}
						else {
							InvalidateRect( GetParent( hdlg ) , NULL , FALSE );
						}
						Kor_Info_Save( JOB_INFO_FILENAME );
					}
					EndDialog( hdlg , 0 );
					return TRUE;

				case IDCANCEL :
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
char ADD_JOB_NAME[256];
char ADD_JOB_RUN_RECIPE[256];
int  ADD_JOB_IN_CASS , ADD_JOB_OUT_CASS , ADD_JOB_START_SLOT , ADD_JOB_END_SLOT , ADD_JOB_PROGRESS;
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_Add_Job_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int i;
	char Buffer[256];
	switch ( msg ) {
		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			if ( !Get_GROUP_RUN_CHECK() )
				KWIN_Item_Hide( hdlg , IDC_LIST_GROUPFILE );
			else
				KWIN_Item_Enable( hdlg , IDC_LIST_GROUPFILE );

			i = Get_Prm_CONTROL_FIXED_CASSETTE_IN();
			if ( i >= 0 )	ADD_JOB_IN_CASS = i - CM1;
			else			ADD_JOB_IN_CASS = 0;
			sprintf( Buffer , "CM%d" , ADD_JOB_IN_CASS + 1 );
			KWIN_Item_String_Display( hdlg , IDC_JOB_INCASS , Buffer );
			if ( i >= 0 )	KWIN_Item_Disable( hdlg , IDC_JOB_INCASS );

			i = Get_Prm_CONTROL_FIXED_CASSETTE_OUT();
			if ( i >= 0 )	ADD_JOB_OUT_CASS = i - CM1;
			else			ADD_JOB_OUT_CASS = 0;
			sprintf( Buffer , "CM%d" , ADD_JOB_OUT_CASS + 1 );
			KWIN_Item_String_Display( hdlg , IDC_JOB_OUTCASS , Buffer );
			if ( i >= 0 )	KWIN_Item_Disable( hdlg , IDC_JOB_OUTCASS );

			i = Get_Prm_CONTROL_FIXED_SLOT_START();
			if ( i >  0 )	ADD_JOB_START_SLOT = i;
			else			ADD_JOB_START_SLOT = 1;
			if ( ADD_JOB_START_SLOT > Get_Prm_MAX_CASSETTE_SLOT() ) ADD_JOB_START_SLOT = Get_Prm_MAX_CASSETTE_SLOT();
			KWIN_Item_Int_Display( hdlg , IDC_JOB_STARTSLOT , ADD_JOB_START_SLOT );
			if ( i >  0 )	KWIN_Item_Disable( hdlg , IDC_JOB_STARTSLOT );
			
			i = Get_Prm_CONTROL_FIXED_SLOT_END();
			if ( i >  0 )	ADD_JOB_END_SLOT = i;
			else			ADD_JOB_END_SLOT = 1;
			if ( ADD_JOB_END_SLOT > Get_Prm_MAX_CASSETTE_SLOT() ) ADD_JOB_END_SLOT = Get_Prm_MAX_CASSETTE_SLOT();
			KWIN_Item_Int_Display( hdlg , IDC_JOB_ENDSLOT , ADD_JOB_END_SLOT );
			if ( i >  0 )	KWIN_Item_Disable( hdlg , IDC_JOB_ENDSLOT );
			//
			ADD_JOB_PROGRESS = 0;
			strcpy( ADD_JOB_NAME , "" );
			strcpy( ADD_JOB_RUN_RECIPE , "" );
			//
			if ( Get_GROUP_RUN_CHECK() ) {
				DirData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_GROUPFILE ) );
			}
			switch( Get_Prm_CONTROL_RECIPE_TYPE() ) {
			case CONTROL_RECIPE_ALL :
			case CONTROL_RECIPE_LOT_PROCESS :
				FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_LOT_RECIPE_PATH() );
				break;
			case CONTROL_RECIPE_CLUSTER :
				FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() );
				break;
			case CONTROL_RECIPE_PROCESS :
				i = Check_Single_Chamber_Use();
				if ( i >= 0 ) {
					FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_PROCESS_RECIPE_PATH( i ) );
				}
				else {
					FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_LOT_RECIPE_PATH() );
				}
				break;
			}
			//
			KWIN_Item_String_Display( hdlg , IDC_JOB_NAME , ADD_JOB_NAME );
			KWIN_Item_String_Display( hdlg , IDC_JOB_LOTRECIPE , ADD_JOB_RUN_RECIPE );
			//
			//
			if ( !Get_Info_DUAL_LOT_MODE() ) {
				KWIN_Item_Hide( hdlg , IDC_JOB_PROGRESS );
			}
			else {
				KWIN_Item_String_Display( hdlg , IDC_JOB_PROGRESS , "Concurrent" );
			}
			return TRUE;
			
		case WM_COMMAND :
			if ( GET_WM_COMMAND_CMD( wParam , lParam ) == LBN_SELCHANGE ) {
				if ( !Get_GROUP_RUN_CHECK() ) {
					if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_LIST_LOTFILE ) {
						i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) );
						if ( i >= 0 ) {
							ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , i , ADD_JOB_RUN_RECIPE );
							KWIN_Item_String_Display( hdlg , IDC_JOB_LOTRECIPE , ADD_JOB_RUN_RECIPE );
						}
					}
				}
				else {
					if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_LIST_GROUPFILE ) {
						i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_GROUPFILE ) );
						if ( i >= 0 ) {
							ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_GROUPFILE ) , i , Buffer );
							Set_Prm_GROUP_RECIPE_PATH( Buffer );
							switch( Get_Prm_CONTROL_RECIPE_TYPE() ) {
							case CONTROL_RECIPE_ALL :
							case CONTROL_RECIPE_LOT_PROCESS :
								FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_LOT_RECIPE_PATH() );
								break;
							case CONTROL_RECIPE_CLUSTER :
								FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_CLUSTER_RECIPE_PATH() );
								break;
							case CONTROL_RECIPE_PROCESS :
								i = Check_Single_Chamber_Use();
								if ( i >= 0 ) {
									FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_PROCESS_RECIPE_PATH( i ) );
								}
								else {
									FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , Get_Prm_LOT_RECIPE_PATH() );
								}
								break;
							}
						}
					}
					if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_LIST_LOTFILE ) {
						i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) );
						if ( i >= 0 ) {
							ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_LOTFILE ) , i , Buffer );
							sprintf( ADD_JOB_RUN_RECIPE , "%s:(%s)" , Buffer , Get_Prm_GROUP_RECIPE_PATH() );
							KWIN_Item_String_Display( hdlg , IDC_JOB_LOTRECIPE , ADD_JOB_RUN_RECIPE );
						}
					}
				}
				return TRUE;
			}
			switch( wParam ) {
				case IDC_JOB_NAME :
					strncpy( Buffer , ADD_JOB_NAME , 255 );
					if ( MODAL_STRING_BOX1( hdlg , "Edit" , "Type Job ID?" , Buffer ) ) {
						strncpy( ADD_JOB_NAME , Buffer , 255 );
						KWIN_Item_String_Display( hdlg , IDC_JOB_NAME , ADD_JOB_NAME );
					}
					return TRUE;

				case IDC_JOB_INCASS :
					if ( Get_Prm_MAX_CASSETTE_COUNT() > 1 ) {
						if ( Get_Prm_MAX_CASSETTE_COUNT() == 2 ) {
							if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2" , &ADD_JOB_IN_CASS ) ) {
								sprintf( Buffer , "CM%d" , ADD_JOB_IN_CASS + 1 );
								KWIN_Item_String_Display( hdlg , IDC_JOB_INCASS , Buffer );
							}
						}
						else if ( Get_Prm_MAX_CASSETTE_COUNT() == 3 ) {
							if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2|CM3" , &ADD_JOB_IN_CASS ) ) {
								sprintf( Buffer , "CM%d" , ADD_JOB_IN_CASS + 1 );
								KWIN_Item_String_Display( hdlg , IDC_JOB_INCASS , Buffer );
							}
						}
						else {
							if ( MAX_SIDE <= 4 ) {
								if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2|CM3|CM4" , &ADD_JOB_IN_CASS ) ) {
									sprintf( Buffer , "CM%d" , ADD_JOB_IN_CASS + 1 );
									KWIN_Item_String_Display( hdlg , IDC_JOB_INCASS , Buffer );
								}
							}
							else { // 2011.01.17
								if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2|CM3|CM4|CM5|CM6|CM7|CM8|CM9|CM10" , &ADD_JOB_IN_CASS ) ) {
									sprintf( Buffer , "CM%d" , ADD_JOB_IN_CASS + 1 );
									KWIN_Item_String_Display( hdlg , IDC_JOB_INCASS , Buffer );
								}
							}
						}
					}
					return TRUE;

				case IDC_JOB_OUTCASS :
					if ( Get_Prm_MAX_CASSETTE_COUNT() > 1 ) {
						if ( Get_Prm_MAX_CASSETTE_COUNT() == 2 ) {
							if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2" , &ADD_JOB_OUT_CASS ) ) {
								sprintf( Buffer , "CM%d" , ADD_JOB_OUT_CASS + 1 );
								KWIN_Item_String_Display( hdlg , IDC_JOB_OUTCASS , Buffer );
							}
						}
						else if ( Get_Prm_MAX_CASSETTE_COUNT() == 3 ) {
							if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2|CM3" , &ADD_JOB_OUT_CASS ) ) {
								sprintf( Buffer , "CM%d" , ADD_JOB_OUT_CASS + 1 );
								KWIN_Item_String_Display( hdlg , IDC_JOB_OUTCASS , Buffer );
							}
						}
						else {
							if ( MAX_SIDE <= 4 ) {
								if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2|CM3|CM4" , &ADD_JOB_OUT_CASS ) ) {
									sprintf( Buffer , "CM%d" , ADD_JOB_OUT_CASS + 1 );
									KWIN_Item_String_Display( hdlg , IDC_JOB_OUTCASS , Buffer );
								}
							}
							else { // 2011.01.17
								if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Cassette" , "CM1|CM2|CM3|CM4|CM5|CM6|CM7|CM8|CM9|CM10" , &ADD_JOB_OUT_CASS ) ) {
									sprintf( Buffer , "CM%d" , ADD_JOB_OUT_CASS + 1 );
									KWIN_Item_String_Display( hdlg , IDC_JOB_OUTCASS , Buffer );
								}
							}
						}
					}
					return TRUE;

				case IDC_JOB_STARTSLOT :
					if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Select Start Slot" , 1 , ADD_JOB_END_SLOT , &ADD_JOB_START_SLOT ) ) {
						KWIN_Item_Int_Display( hdlg , IDC_JOB_STARTSLOT , ADD_JOB_START_SLOT );
					}
					return TRUE;

				case IDC_JOB_ENDSLOT :
					if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Select End Slot" , ADD_JOB_START_SLOT , Get_Prm_MAX_CASSETTE_SLOT() , &ADD_JOB_END_SLOT ) ) {
						KWIN_Item_Int_Display( hdlg , IDC_JOB_ENDSLOT , ADD_JOB_END_SLOT );
					}
					return TRUE;

				case IDC_JOB_PROGRESS :
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Mode" , "CONCURRENT|CONTINUE|SEQUENTIAL" , &ADD_JOB_PROGRESS ) ) {
						if      ( ADD_JOB_PROGRESS == 0 )
							KWIN_Item_String_Display( hdlg , IDC_JOB_PROGRESS , "Concurrent" );
						else if ( ADD_JOB_PROGRESS == 1 )
							KWIN_Item_String_Display( hdlg , IDC_JOB_PROGRESS , "Continue" );
						else
							KWIN_Item_String_Display( hdlg , IDC_JOB_PROGRESS , "Sequential" );
					}
					return TRUE;

				case IDOK :
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Job Add/Edit Impossible(Read Only Mode)" , "Job Add/Edit Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( strlen( ADD_JOB_NAME ) <= 0 ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Job Name Data is NULL" , "Reject" , MB_ICONHAND | MB_OK );
						return TRUE;
					}
					if ( JOB_Question_Name_Exist( ADD_JOB_NAME ) ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Job Name has a Same Data" , "Reject" , MB_ICONHAND | MB_OK );
						return TRUE;
					}
					if ( strlen( ADD_JOB_RUN_RECIPE ) <= 0 ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Lot Recipe Data is NULL" , "Reject" , MB_ICONHAND | MB_OK );
						return TRUE;
					}
					EndDialog( hdlg , TRUE );
					return TRUE;

				case IDCANCEL :
					EndDialog( hdlg , FALSE );
					return TRUE;

			}
			return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
extern	int	 FIND_COUNT;
extern	int	 FIND_INDEX[MAX_JOB_COUNT];
extern	char FIND_JID[256];		extern	BOOL FIND_JID_RUN;	extern	int FIND_OPTION1;
extern	char FIND_LID[256];		extern	BOOL FIND_LID_RUN;	extern	int FIND_OPTION2;
extern	char FIND_MID[256];		extern	BOOL FIND_MID_RUN;	extern	int FIND_OPTION3;
extern	char FIND_WID[256];		extern	BOOL FIND_WID_RUN;	extern	int FIND_OPTION4;
extern	BOOL FIND_START_RUN;
extern	int FIND_START_DATE_Y;
extern	int FIND_START_DATE_M;
extern	int FIND_START_DATE_D;
extern	BOOL FIND_END_RUN;
extern	int FIND_END_DATE_Y;
extern	int FIND_END_DATE_M;
extern	int FIND_END_DATE_D;
extern	int FIND_OPTION5;
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_Find_Data_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int Res , y , m , d , h , n, s;
	char Buffer[512];
	char Buffer2[64];
	SYSTEMTIME SysTime;
	//
	switch ( msg ) {
		case WM_DESTROY:
			//
			KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.10
			//
			return TRUE;

		case WM_INITDIALOG:
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.10
			//
			KWIN_Item_String_Display( hdlg , IDC_FIND_JOBID_STRING   , Get_Prm_JID_STRING() );
			KWIN_Item_String_Display( hdlg , IDC_FIND_LOTID_STRING   , Get_Prm_LID_STRING() );
			KWIN_Item_String_Display( hdlg , IDC_FIND_MID_STRING     , Get_Prm_MID_STRING() );
			KWIN_Item_String_Display( hdlg , IDC_FIND_WAFERID_STRING , Get_Prm_WID_STRING() );
			//
			KWIN_Item_String_Display( hdlg , IDRETRY , "Period" );
			//
			FIND_START_RUN = FALSE;
			if ( ( FIND_START_DATE_Y >= 50 ) && ( FIND_START_DATE_Y <= 1000 ) ) {
				if ( ( FIND_START_DATE_M >= 0 ) && ( FIND_START_DATE_M <= 11 ) ) {
					if ( ( FIND_START_DATE_D >= 1 ) && ( FIND_START_DATE_D <= 31 ) ) {
						FIND_START_RUN = TRUE;
					}
				}
			}
			FIND_END_RUN = FALSE;
			if ( ( FIND_END_DATE_Y >= 50 ) && ( FIND_END_DATE_Y <= 1000 ) ) {
				if ( ( FIND_END_DATE_M >= 0 ) && ( FIND_END_DATE_M <= 11 ) ) {
					if ( ( FIND_END_DATE_D >= 1 ) && ( FIND_END_DATE_D <= 31 ) ) {
						FIND_END_RUN = TRUE;
					}
				}
			}
			SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
			return TRUE;
			
		case WM_COMMAND :
			switch( wParam ) {
				case IDC_FIND_JOBNAME :
					strncpy( Buffer , FIND_JID , 255 );
					if ( MODAL_STRING_BOX1( hdlg , "Edit" , Get_Prm_JID_STRING() , Buffer ) ) {
						strncpy( FIND_JID , Buffer , 255 );
						if ( strlen( FIND_JID ) <= 0 ) FIND_JID_RUN = FALSE;
						else                           FIND_JID_RUN = TRUE;
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_LOTRECIPE :
					strncpy( Buffer , FIND_LID , 255 );
					if ( MODAL_STRING_BOX1( hdlg , "Edit" , Get_Prm_LID_STRING() , Buffer ) ) {
						strncpy( FIND_LID , Buffer , 255 );
						if ( strlen( FIND_LID ) <= 0 ) FIND_LID_RUN = FALSE;
						else                           FIND_LID_RUN = TRUE;
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_METHODID :
					strncpy( Buffer , FIND_MID , 255 );
					if ( MODAL_STRING_BOX1( hdlg , "Edit" , Get_Prm_MID_STRING() , Buffer ) ) {
						strncpy( FIND_MID , Buffer , 255 );
						if ( strlen( FIND_MID ) <= 0 ) FIND_MID_RUN = FALSE;
						else                           FIND_MID_RUN = TRUE;
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_STARTTIME_Y :
					//
					GetLocalTime( &SysTime );
					//
					strcpy( Buffer , "NOTUSE" );
					//
					for ( m = SysTime.wYear ; m >= 2000 ; m-- ) {
						//
						sprintf( Buffer2 , "|%d" , m );
						//
						strcat( Buffer , Buffer2 );
						//
					}
					//
					Res = -1;
					//
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Time(Year)" , Buffer , &Res ) ) {
						//
						//
						KWIN_Item_String_Display( hdlg , IDRETRY , "Period" );
						//
						//
						if ( Res == 0 ) {
							FIND_START_DATE_Y = 0;
							KWIN_Item_String_Display( hdlg , IDC_FIND_STARTTIME_Y , "" );
							KWIN_Item_String_Display( hdlg , IDC_FIND_STARTTIME_M , "" ); // 2018.05.06
							KWIN_Item_String_Display( hdlg , IDC_FIND_STARTTIME_D , "" ); // 2018.05.06
						}
						else {
							//
							FIND_START_DATE_Y = ( SysTime.wYear - 2000 ) - Res + 101;
							//
							KWIN_Item_Int_Display( hdlg , IDC_FIND_STARTTIME_Y , FIND_START_DATE_Y + 1900 );
						}
						FIND_START_RUN = FALSE;
						if ( ( FIND_START_DATE_Y >= 50 ) && ( FIND_START_DATE_Y <= 1000 ) ) {
							if ( ( FIND_START_DATE_M >= 0 ) && ( FIND_START_DATE_M <= 11 ) ) {
								if ( ( FIND_START_DATE_D >= 1 ) && ( FIND_START_DATE_D <= 31 ) ) {
									FIND_START_RUN = TRUE;
								}
							}
						}
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_STARTTIME_M :
					Res = -1;
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Time(Month)" , "1|2|3|4|5|6|7|8|9|10|11|12" , &Res ) ) {
						//
						//
						KWIN_Item_String_Display( hdlg , IDRETRY , "Period" );
						//
						//
						FIND_START_DATE_M = Res;
						KWIN_Item_Int_Display( hdlg , IDC_FIND_STARTTIME_M , FIND_START_DATE_M + 1 );
						FIND_START_RUN = FALSE;
						if ( ( FIND_START_DATE_Y >= 50 ) && ( FIND_START_DATE_Y <= 1000 ) ) {
							if ( ( FIND_START_DATE_M >= 0 ) && ( FIND_START_DATE_M <= 11 ) ) {
								if ( ( FIND_START_DATE_D >= 1 ) && ( FIND_START_DATE_D <= 31 ) ) {
									FIND_START_RUN = TRUE;
								}
							}
						}
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_STARTTIME_D :
					Res = -1;
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Time(Day)" , "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31" , &Res ) ) {
						//
						//
						KWIN_Item_String_Display( hdlg , IDRETRY , "Period" );
						//
						//
						FIND_START_DATE_D = 1+Res;
						KWIN_Item_Int_Display( hdlg , IDC_FIND_STARTTIME_D , FIND_START_DATE_D );
						FIND_START_RUN = FALSE;
						if ( ( FIND_START_DATE_Y >= 50 ) && ( FIND_START_DATE_Y <= 1000 ) ) {
							if ( ( FIND_START_DATE_M >= 0 ) && ( FIND_START_DATE_M <= 11 ) ) {
								if ( ( FIND_START_DATE_D >= 1 ) && ( FIND_START_DATE_D <= 31 ) ) {
									FIND_START_RUN = TRUE;
								}
							}
						}
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_ENDTIME_Y :
					//
					GetLocalTime( &SysTime );
					//
					strcpy( Buffer , "NOTUSE" );
					//
					for ( m = SysTime.wYear ; m >= 2000 ; m-- ) {
						//
						sprintf( Buffer2 , "|%d" , m );
						//
						strcat( Buffer , Buffer2 );
						//
					}
					//
					Res = -1;
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Time(Year)" , Buffer , &Res ) ) {
						//
						//
						KWIN_Item_String_Display( hdlg , IDRETRY , "Period" );
						//
						//
						if ( Res == 0 ) {
							FIND_END_DATE_Y = 0;
							KWIN_Item_String_Display( hdlg , IDC_FIND_ENDTIME_Y , "" );
							KWIN_Item_String_Display( hdlg , IDC_FIND_ENDTIME_M , "" ); // 2018.05.06
							KWIN_Item_String_Display( hdlg , IDC_FIND_ENDTIME_D , "" ); // 2018.05.06
						}
						else {
							//
							FIND_END_DATE_Y = ( SysTime.wYear - 2000 ) - Res + 101;
							//
							KWIN_Item_Int_Display( hdlg , IDC_FIND_ENDTIME_Y , FIND_END_DATE_Y + 1900 );
						}
						FIND_END_RUN = FALSE;
						if ( ( FIND_END_DATE_Y >= 50 ) && ( FIND_END_DATE_Y <= 1000 ) ) {
							if ( ( FIND_END_DATE_M >= 0 ) && ( FIND_END_DATE_M <= 11 ) ) {
								if ( ( FIND_END_DATE_D >= 1 ) && ( FIND_END_DATE_D <= 31 ) ) {
									FIND_END_RUN = TRUE;
								}
							}
						}
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_ENDTIME_M :
					Res = -1;
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Time(Month)" , "1|2|3|4|5|6|7|8|9|10|11|12" , &Res ) ) {
						//
						//
						KWIN_Item_String_Display( hdlg , IDRETRY , "Period" );
						//
						//
						FIND_END_DATE_M = Res;
						KWIN_Item_Int_Display( hdlg , IDC_FIND_ENDTIME_M , FIND_END_DATE_M + 1 );
						FIND_END_RUN = FALSE;
						if ( ( FIND_END_DATE_Y >= 50 ) && ( FIND_END_DATE_Y <= 1000 ) ) {
							if ( ( FIND_END_DATE_M >= 0 ) && ( FIND_END_DATE_M <= 11 ) ) {
								if ( ( FIND_END_DATE_D >= 1 ) && ( FIND_END_DATE_D <= 31 ) ) {
									FIND_END_RUN = TRUE;
								}
							}
						}
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_ENDTIME_D :
					Res = -1;
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Time(Day)" , "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31" , &Res ) ) {
						//
						//
						KWIN_Item_String_Display( hdlg , IDRETRY , "Period" );
						//
						//
						FIND_END_DATE_D = 1+Res;
						KWIN_Item_Int_Display( hdlg , IDC_FIND_ENDTIME_D , FIND_END_DATE_D );
						FIND_END_RUN = FALSE;
						if ( ( FIND_END_DATE_Y >= 50 ) && ( FIND_END_DATE_Y <= 1000 ) ) {
							if ( ( FIND_END_DATE_M >= 0 ) && ( FIND_END_DATE_M <= 11 ) ) {
								if ( ( FIND_END_DATE_D >= 1 ) && ( FIND_END_DATE_D <= 31 ) ) {
									FIND_END_RUN = TRUE;
								}
							}
						}
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_WAFERID :
					strncpy( Buffer , FIND_WID , 255 );
					if ( MODAL_STRING_BOX1( hdlg , "Edit" , Get_Prm_WID_STRING() , Buffer ) ) {
						strncpy( FIND_WID , Buffer , 255 );
						if ( strlen( FIND_WID ) <= 0 ) FIND_WID_RUN = FALSE;
						else                           FIND_WID_RUN = TRUE;
						//
						SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					}
					return TRUE;

				case IDC_FIND_OPTION1 :
					if ( FIND_OPTION1 ) FIND_OPTION1 = FALSE;
					else                FIND_OPTION1 = TRUE;
					//
					SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					return TRUE;

				case IDC_FIND_OPTION2 :
					if ( FIND_OPTION2 ) FIND_OPTION2 = FALSE;
					else                FIND_OPTION2 = TRUE;
					//
					SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					return TRUE;

				case IDC_FIND_OPTION3 :
					if ( FIND_OPTION3 ) FIND_OPTION3 = FALSE;
					else                FIND_OPTION3 = TRUE;
					//
					SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					return TRUE;

				case IDC_FIND_OPTION4 :
					if ( FIND_OPTION4 ) FIND_OPTION4 = FALSE;
					else                FIND_OPTION4 = TRUE;
					//
					SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					return TRUE;

				case IDC_FIND_OPTION5 :
					if      ( FIND_OPTION5 == 0 ) FIND_OPTION5 = 1;
					else if ( FIND_OPTION5 == 1 ) FIND_OPTION5 = 2;
					else if ( FIND_OPTION5 == 2 ) FIND_OPTION5 = 3;
					else if ( FIND_OPTION5 == 3 ) FIND_OPTION5 = 0;
					else                          FIND_OPTION5 = 0;
					//
					SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					return TRUE;


				case IDRETRY : // 2018.05.06
					//
					Res = -1;
					//
					if ( !MODAL_DIGITAL_BOX1( hdlg , "Select" , "Range" , "Today|2Days|3Days|1 Weeks|10 Days|15 Days|1 Month|2 Months|3 Months|6 Months|1 Year|2 Years|3 Years" , &Res ) ) return FALSE;
					//
					GetLocalTime( &SysTime );
					//
					y = SysTime.wYear;
					m = SysTime.wMonth;
					d = SysTime.wDay;
					//
					h = 0;
					n = 0;
					s = 0;
					//
					switch( Res ) {
					case 1 : // 2Days
						d = d - 1;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{2 Days}" );
						break;
					case 2 : // 3Days
						d = d - 2;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{3 Days}" );
						break;
					case 3 : // 1 Weeks
						d = d - 6;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{1 Week}" );
						break;
					case 4 : // 10 Days
						d = d - 9;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{10 Days}" );
						break;
					case 5 : // 15 Days
						d = d - 14;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{15 Days}" );
						break;
					case 6 : // 1 Month
						m = m - 1;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{1 Month}" );
						break;
					case 7 : // 2 Month
						m = m - 2;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{2 Months}" );
						break;
					case 8 : // 3 Month
						m = m - 3;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{3 Months}" );
						break;
					case 9 : // 6 Month
						m = m - 6;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{6 Months}" );
						break;
					case 10 : // 1 Year
						y = y - 1;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{1 Year}" );
						break;
					case 11 : // 2 Years
						y = y - 2;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{2 Years}" );
						break;
					case 12 : // 3 Years
						y = y - 3;	_UTIL_TIME_GET_DATETIME_from_CAL( &y , &m , &d , &h , &n , &s , 0 );	KWIN_Item_String_Display( hdlg , IDRETRY , "{3 Years}" );
						break;
					default : // today
																											KWIN_Item_String_Display( hdlg , IDRETRY , "{Today}" );
						break;
					}
					//
					FIND_START_RUN = TRUE;
					//
					FIND_START_DATE_Y = y - 1900;
					FIND_START_DATE_M = m - 1;
					FIND_START_DATE_D = d;
					//
					FIND_END_DATE_Y = 0;
					FIND_END_RUN = FALSE;
					//
					KWIN_Item_String_Display( hdlg , IDC_FIND_ENDTIME_Y , "" );
					KWIN_Item_String_Display( hdlg , IDC_FIND_ENDTIME_M , "" );
					KWIN_Item_String_Display( hdlg , IDC_FIND_ENDTIME_D , "" );
					//
					SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) NULL );
					//
					return TRUE;

				case IDOK :
					EndDialog( hdlg , TRUE );
					return TRUE;

				case IDCANCEL :
					EndDialog( hdlg , FALSE );
					return TRUE;

				case IDYES :
					KWIN_Item_String_Display( hdlg , IDC_FIND_JOBNAME , FIND_JID );
					if ( FIND_JID_RUN ) {
						if ( FIND_LID_RUN || FIND_MID_RUN || FIND_START_RUN || FIND_END_RUN || FIND_WID_RUN ) {
							if ( FIND_OPTION1 )
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION1 , "and" );
							else
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION1 , "or" );
						}
						else {
							KWIN_Item_Hide( hdlg , IDC_FIND_OPTION1 );
						}
					}
					else {
						KWIN_Item_Hide( hdlg , IDC_FIND_OPTION1 );
					}
					//
					KWIN_Item_String_Display( hdlg , IDC_FIND_LOTRECIPE , FIND_LID );
					if ( FIND_LID_RUN ) {
						if ( FIND_MID_RUN || FIND_START_RUN || FIND_END_RUN || FIND_WID_RUN ) {
							if ( FIND_OPTION2 )
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION2 , "and" );
							else
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION2 , "or" );
						}
						else {
							KWIN_Item_Hide( hdlg , IDC_FIND_OPTION2 );
						}
					}
					else {
						KWIN_Item_Hide( hdlg , IDC_FIND_OPTION2 );
					}
					//
					KWIN_Item_String_Display( hdlg , IDC_FIND_METHODID , FIND_MID );
					if ( FIND_MID_RUN ) {
						if ( FIND_START_RUN || FIND_END_RUN || FIND_WID_RUN ) {
							if ( FIND_OPTION3 )
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION3 , "and" );
							else
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION3 , "or" );
						}
						else {
							KWIN_Item_Hide( hdlg , IDC_FIND_OPTION3 );
						}
					}
					else {
						KWIN_Item_Hide( hdlg , IDC_FIND_OPTION3 );
					}
					//
					if ( FIND_START_RUN ) {
						KWIN_Item_Int_Display( hdlg , IDC_FIND_STARTTIME_Y , FIND_START_DATE_Y + 1900 );
						KWIN_Item_Int_Display( hdlg , IDC_FIND_STARTTIME_M , FIND_START_DATE_M + 1 );
						KWIN_Item_Int_Display( hdlg , IDC_FIND_STARTTIME_D , FIND_START_DATE_D );
					}
					//
					if ( FIND_END_RUN ) {
						KWIN_Item_Int_Display( hdlg , IDC_FIND_ENDTIME_Y , FIND_END_DATE_Y + 1900 );
						KWIN_Item_Int_Display( hdlg , IDC_FIND_ENDTIME_M , FIND_END_DATE_M + 1 );
						KWIN_Item_Int_Display( hdlg , IDC_FIND_ENDTIME_D , FIND_END_DATE_D );
					}
					//
					if ( FIND_START_RUN || FIND_END_RUN ) {
						if ( FIND_WID_RUN ) {
							if ( FIND_OPTION4 )
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION4 , "and" );
							else
								KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION4 , "or" );
						}
						else {
							KWIN_Item_Hide( hdlg , IDC_FIND_OPTION4 );
						}
					}
					else {
						KWIN_Item_Hide( hdlg , IDC_FIND_OPTION4 );
					}
					//
					KWIN_Item_String_Display( hdlg , IDC_FIND_WAFERID , FIND_WID );
					//
					switch ( FIND_OPTION5 ) {
					case 1 :
						KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION5 , "Exclusive" );
						break;
					case 2 :
						KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION5 , "Normal(First)" );
						break;
					case 3 :
						KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION5 , "Exclusive(First)" );
						break;
					default :
						KWIN_Item_String_Display( hdlg , IDC_FIND_OPTION5 , "Normal" );
						break;
					}
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}

void Get_CTR_Display_String( int sel , char *data ) {
	switch( sel ) {
	case CTR_PROCESS_DATA_1_WAFER	:	sprintf( data , "Process Data\n(1 %s)" , Get_Prm_METHOD_STRING() );	break;
	case CTR_PROCESS_PLOT_1_WAFER	:	sprintf( data , "Draw Plotting\n(1 %s)" , Get_Prm_METHOD_STRING() );	break;
	case CTR_RECIPE_DATA_1_WAFER	:	sprintf( data , "Recipe Data\n(1 %s)" , Get_Prm_METHOD_STRING() );	break;
	case CTR_LOT_SUMMARY_DATA		:	sprintf( data , "Lot Summary\nData" );	break;
	case CTR_LOT_SUMMARY_MODULE		:	sprintf( data , "Lot Data\n(Module)" );	break;
	case CTR_PROCESS_DATA_MODULE	:	sprintf( data , "Process Data\n(Module)" );	break;
	case CTR_FLOW_HISTORY			:	sprintf( data , "Flow\nHistory" );	break;
	default							:	sprintf( data , "Unknown" );	break;
	}
}

int Get_CTR_Button_Data( int wprm ) {
	switch( wprm ) {
	case IDC_CTRL_LOG1	:	return CTRL_1_SELECT_DATA;
	case IDC_CTRL_LOG2	:	return CTRL_2_SELECT_DATA;
	case IDC_CTRL_LOG3	:	return CTRL_3_SELECT_DATA;
	case IDC_CTRL_LOG4	:	return CTRL_4_SELECT_DATA;
	case IDC_CTRL_LOG5	:	return CTRL_5_SELECT_DATA;
	case IDC_CTRL_LOG6	:	return CTRL_6_SELECT_DATA;
	case IDC_CTRL_LOG7	:	return CTRL_7_SELECT_DATA;
	case IDC_CTRL_LOG8	:	return CTRL_8_SELECT_DATA;
	}
	return 0;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void GET_SYSTEM_FILE( char *retfilename , char *name1 , char *name2 ) { // 2017.03.12
	//
	char Buffer[256];
	char Buffer2[256];
	//
	strcpy( Buffer , "$REQUEST_SYSTEM$" );
	//
	_IO_GET_SYSTEM_FOLDER( Buffer , 255 );
	//
	if ( name2 == NULL ) {
		sprintf( retfilename , "%s\\%s" , Buffer , name1 );
	}
	else {
		sprintf( Buffer2 , name1 , name2 );
		sprintf( retfilename , "%s\\%s" , Buffer , Buffer2 );
	}
	//
}


BOOL APIENTRY Console_Gui_For_Main_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT	ps;
	int i , j , s;
	static UINT uTimer;
	static UINT uTimer2;
	static UINT ttt , tuse;
	char MessageBuffer[512];
	char MessageBuffer2[512];
//	char MessageBuffer3[256]; // 2012.04.13
	char MessageBuffer3[512]; // 2012.04.13
	char MessageBufferM[512]; // 2012.04.13
	char ModuleStr[64]; // 2016.12.17

	char folderstr[256];

	HBITMAP hBitmap;
	RECT rect;
//	static BOOL FIRSTTIME; // 2017.03.13
	static int FIRSTTIME; // 2017.03.13
	static int status_save , FORCE_1HOUR_DELETE;
	//
	// 2005.04.08
	HFILE hFile;
	int  ReadCnt , posindex , posdata;
	BOOL FileEnd;
	int retindexs[256];
	int retindexsz[256];
	//
	int  color_error; // 2011.06.15
	int  color_error2; // 2011.06.20
	int  color_Index; // 2011.06.20
	LPDRAWITEMSTRUCT dis; // 2011.06.15
	LPMEASUREITEMSTRUCT dim; // 2011.06.15
	LPNMHDR  lpnmh; // 2011.06.15
	char Temp_Recipe[2560]; // 2011.06.15
	char Temp_SlotStr[256]; // 2012.11.29

	char pmorg[256]; // 2017.08.18

	int index , mode , subid , selfrom;
	int wxsize , wysize , wxs , xgap;
	HTREEITEM hitm;
	LV_COLUMN   lvColumn; // 2015.05.27

	int lpcnt_curr , lpcnt_max , lpcnt_selindex; // 2012.04.13

	FILE *fpt; // 2012.06.26

	static double wxp , wyp , pchg; // 2012.04.23

	int layoutxoffset , layoutyoffset , layoutxsoffset , layoutysoffset; // 2017.03.01
	int flowhistory_multi;

//if ( msg == WM_DISPLAYCHANGE ) printf( "[WM_DISPLAYCHANGE  image depth=%d , screen resolution=%d]\n" , msg , wParam , lParam );

	//
	KGUI_STANDARD_Check_Enable_Monitoring_Function( hdlg ); // 2018.01.10
	//

	switch ( msg ) {

		case WM_ERASEBKGND : // 2012.12.07
			return TRUE;

		case WM_USER:
            switch ( wParam ) {
				case WM_PAINT:
					return TRUE;

				case WM_CLOSE:
						//<<-----------------------------------------------------------
						//>>-----------------------------------------------------------
						return TRUE;

				case WM_ACTIVATE:
						//>>-----------------------------------------------------------
						JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
						InvalidateRect( hdlg , NULL , FALSE );
						//<<-----------------------------------------------------------
						return TRUE;
			}
			break;

		case WM_TIMER :
			//-------------------------------------
			if ( tuse < 60 ) {
				tuse++;
				return TRUE;
			}
			//-------------------------------------
			ttt++;
//			if ( ttt > 30 ) ttt = 0; // 2003.09.17
//				if ( !JOB_W_OPEN && ( ttt == 0 ) ) { // 2003.09.16
			if ( ttt > 20 ) { // 2003.09.17
				ttt = 0; // 2003.09.17
//					if ( JOB_W_DELETE ) { // 2003.09.16
//						return TRUE; // 2003.09.16
//					} // 2003.09.16
//					else { // 2003.09.16
					if ( !Get_Prm_NETMONITOR_MODE() ) {
						if ( JOB_Question_Count_Warning( 15 ) ) {
							if ( Get_Prm_FIND_MODE() ) { // 2003.09.17
								SendMessage( hdlg , WM_COMMAND , IDC_CTRL_GO_BACK , (LPARAM) NULL ); // 2003.09.17
							}
							else {
								//===================================================================================
								//
								if ( LOG_WINDOW_OPEN ) LOG_WINDOW_OPEN = FALSE; // 2017.03.13
								//
//								if ( !LOG_WINDOW_OPEN ) { // 2007.05.16 // 2017.03.13
									//===================================================================================
									// SendMessage( hdlg , WM_COMMAND , IDABORT , (LPARAM) NULL ); // 2003.09.16
									//-------------------------------------
									// 2003.09.16
									//-------------------------------------
									EnterCriticalSection( &CR_SC );
									//-------------------------------------
									//if ( !JOB_W_KILL ) { // 2003.09.16
										JobData_ListView_Old_DeleteData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) , 50 );
										Kor_JOB_Save( JOB_DATA_FILENAME );
									//} // 2003.09.16
									//-------------------------------------
									LeaveCriticalSection( &CR_SC );
									//-------------------------------------
//								}
							}
						}
						//==============================================================
						if ( !Get_Prm_FIND_MODE() ) { // 2003.09.17
							if ( JOB_Display_Data_Save_per_Each_Check( &status_save ) == 1 ) { // 2003.06.09
								Kor_JOB_Save( JOB_DATA_FILENAME );
							}
						}
						//==============================================================
					}
					else {
						switch( _nINF_KNFS_CompareFile( JOB_DATA_FILENAME , JOB_DATA_FILENAME_ORG ) ) { // -1:InvalidError 0:Different 1:Same 2:LocalFileError 3:RemoteFileError
						case -1 :
						case 1 :
							return TRUE;
							break;
						case 0 :
						case 2 :
							break;
						case 3 :
							DeleteFile( JOB_DATA_FILENAME );
							break;
						}
						if ( Get_Prm_FIND_MODE() ) { // 2003.09.17
							SendMessage( hdlg , WM_COMMAND , IDC_CTRL_GO_BACK , (LPARAM) NULL ); // 2003.09.17
						}
						else {
							// SendMessage( hdlg , WM_COMMAND , IDRETRY , (LPARAM) NULL ); // 2003.09.16
							//-------------------------------------
							// 2003.09.16
							//-------------------------------------
							EnterCriticalSection( &CR_SC );
							//-------------------------------------
							_nINF_KNFS_PrepareLocalFolder_for_Filename( JOB_DATA_FILENAME );
							_nINF_KNFS_GetFile( JOB_DATA_FILENAME_ORG , JOB_DATA_FILENAME , FALSE );
							//
							JobData_ListView_Clear( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							Kor_JOB_Open( JOB_DATA_FILENAME );
							//
							JobData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) );
							//
							JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							//-------------------------------------
							LeaveCriticalSection( &CR_SC );
							//-------------------------------------
						}
					}
					return TRUE;
//					} // 2003.09.16
			}
			if ( Get_Prm_NETMONITOR_MODE() ) return TRUE;

			//-------------------------------------
			EnterCriticalSection( &CR_SC );
			//-------------------------------------
			//if ( JOB_W_KILL ) { // 2003.09.16
			//	LeaveCriticalSection( &CR_SC ); // 2003.09.16
			//	return TRUE; // 2003.09.16
			//} // 2003.09.16
            switch ( wParam ) {
			case 1000 : // 0.5 sec
				//
//				if ( ( Get_Prm_DISPLAY_MODE() == 1 ) || ( Get_Prm_DISPLAY_MODE() == 3 ) ) return TRUE; // 2011.06.15 // 2016.06.01
//				if ( ( Get_Prm_DISPLAY_MODE() == 1 ) || ( Get_Prm_DISPLAY_MODE() == 3 ) ) break; // 2011.06.15 // 2016.06.01 // 2017.03.13
				//
//				if ( !FIRSTTIME ) { // 2002.10.18 // 2017.03.13
//					FIRSTTIME = TRUE; // 2017.03.13
//				} // 2017.03.13
				//
				if ( FIRSTTIME != 0 ) { // 2002.10.18 // 2017.03.13
					//
					if ( ( Get_Prm_DISPLAY_MODE() != 1 ) && ( Get_Prm_DISPLAY_MODE() != 3 ) ) { // 2017.03.13
						//
						i = JOB_RUNNING_CHECK();
						//
						if ( i == -1 ) {
							if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
								InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
							}
							Kor_JOB_Save( JOB_DATA_FILENAME );
							JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
						}
						else if ( i == -2 ) {
							if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
								InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
							}
						}
						else if ( i == -3 ) { // 2010.12.02
							if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
								InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
							}
							Kor_JOB_Save( JOB_DATA_FILENAME );
						}
						else if ( i == -4 ) { // 2010.12.02
							if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
								InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
							}
							Kor_JOB_Save( JOB_DATA_FILENAME );
							//
							status_save = 0;
							//
						}
						else if ( ( i <= -11 ) && ( i >= ( -( 11 + MAX_SIDE - 1 ) ) ) ) {
	//						if ( JOB_W_OPEN ) break; // 2003.09.16
	//						JOB_W_OPEN = TRUE; // 2003.09.16
							if ( JOB_RunMode_Maint_Insert_Action( hdlg , 0 - ( 11 + i ) ) ) {
								JobData_ListView_Insert( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
								if ( JobData_TreeView_Insert( GetDlgItem( hdlg , IDC_TREE ) ) ) {
									if ( KGUI_STANDARD_Window_Signal_Mode() ) {
										InvalidateRect( GetDlgItem( hdlg , IDC_TREE ) , NULL , FALSE );
									}
								}
								if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
									InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
								}
								Kor_JOB_Save( JOB_DATA_FILENAME );
								JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							}
	//						JOB_W_OPEN = FALSE; // 2003.09.16
						}
						else if ( i >= 0 ) {
	//						if ( JOB_W_OPEN ) break; // 2003.09.16
	//						JOB_W_OPEN = TRUE; // 2003.09.16
							if ( JOB_RunMode_Start_Action( hdlg , i ) ) {
								if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
									InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
								}
								Kor_JOB_Save( JOB_DATA_FILENAME );
								JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							}
	//						JOB_W_OPEN = FALSE; // 2003.09.16
						}
						//
					}
					//
//					break; // 2017.03.13
					if ( FIRSTTIME == 1 ) break; // 2017.03.13
					//
				}
				//
				//
			case 1001 : // 1 hour
				//
				if ( Get_Prm_FIND_MODE() ) { // 2017.03.13
					//
					if ( ( FORCE_1HOUR_DELETE > 0 ) && ( FORCE_1HOUR_DELETE <= 32 ) ) {
						//
						_IO_CIM_PRINTF( "FORCE_1HOUR_DELETE TAG NOT RELEASED at FIND_MODE from 1Hour Auto Delete [%d]\n" , FORCE_1HOUR_DELETE );
						//
						FIRSTTIME = 0;
						//
					}
					else if ( FORCE_1HOUR_DELETE > 32 ) {
						//
						FIRSTTIME = 2;
						//
					}
					else if ( FORCE_1HOUR_DELETE > 64 ) {
						//
						FIRSTTIME = 1;
						FORCE_1HOUR_DELETE = 0;
						//
					}
					else {
						//
						FIRSTTIME = 0;
						//
					}
					//
					if ( ( FORCE_1HOUR_DELETE % 2 ) == 0 ) {
						//
						SendMessage( hdlg , WM_COMMAND , IDC_CTRL_GO_BACK , (LPARAM) NULL );
					}
					//
					FORCE_1HOUR_DELETE++;
					//
					break;
				}

//				if ( !Get_Prm_FIND_MODE() ) { // 2017.03.13
					//===================================================================================
//					if ( LOG_WINDOW_OPEN ) break; // 2007.05.16 // 2017.03.13
					//
					//===================================================================================
					LOG_AUTODEL_OPEN = TRUE; // 2007.05.16
					//
					if ( LOG_WINDOW_OPEN ) LOG_WINDOW_OPEN = FALSE; // 2017.03.13
					//
					//===================================================================================
//					if ( LOG_WINDOW_OPEN ) { // 2007.05.16 // 2017.03.13
//						LOG_AUTODEL_OPEN = FALSE; // 2007.05.16 // 2017.03.13
//						break; // 2017.03.13
//					} // 2017.03.13
					//===================================================================================
	//				if ( JOB_W_OPEN ) break; // 2003.09.16
	//				JOB_W_OPEN = TRUE; // 2003.09.16
//					j = JOB_TIMEOVER_DELETE_CHECK( hdlg ); // 2003.09.17
//					j = JOB_TIMEOVER_DELETE_CHECK( 100 ); // 2003.09.17 // 2004.01.14 // 2014.10.15

					// 2014.10.15
					switch( Get_Info_DELETE_COUNT() ) {
					case 0 : i = 100;	break;
					case 1 : i = 90;	break;
					case 2 : i = 80;	break;
					case 3 : i = 70;	break;
					case 4 : i = 60;	break;
					case 5 : i = 50;	break;
					case 6 : i = 40;	break;
					case 7 : i = 30;	break; // 2017.03.13
					case 8 : i = 20;	break; // 2017.03.13
					default: i = 10;	break; // 2017.03.13
					}
					//
					j = JOB_TIMEOVER_DELETE_CHECK( i ); // 2003.09.17 // 2004.01.14 // 2014.10.15
					//
					if ( j >= 0 ) {
						JobData_ListView_DeleteItem( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) , j );
						if ( KGUI_STANDARD_Window_Signal_Mode() ) { // 2004.05.21
							InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
						}
						Kor_JOB_Save( JOB_DATA_FILENAME );
						JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
					}
	//				JOB_W_OPEN = FALSE; // 2003.09.16
					//===================================================================================
					LOG_AUTODEL_OPEN = FALSE; // 2007.05.16
					//===================================================================================
					//
					FIRSTTIME = 1; // 2017.03.13
					FORCE_1HOUR_DELETE = 0; // 2017.03.13
					//
//				} // 2017.03.13
				break;
			}
			//-------------------------------------
			LeaveCriticalSection( &CR_SC );
			//-------------------------------------
			return TRUE;

		case WM_INITDIALOG:
//<929,586> GET_WINDOW_POSITION_FOR_GUI_XS(3) , GET_WINDOW_POSITION_FOR_GUI_YS(3) );
//
			//
			layoutxoffset = layoutyoffset = layoutxsoffset = layoutysoffset = 0; // 2017.03.01
			//
//			wxsize = GET_WINDOW_POSITION_FOR_GUI_XS(3);
//			wysize = GET_WINDOW_POSITION_FOR_GUI_YS(3);
			//
			wxsize = 929;
			wysize = 586;
			//
			wxp = 1; // 2012.04.23
			wyp = 1; // 2012.04.23
			pchg = 0; // 2012.04.23
			//

			FIRSTTIME = 0; // 2017.03.13
			FORCE_1HOUR_DELETE = 0; // 2017.03.13
			//
			ttt = 0;
			status_save = 0;
			Main_Handle = hdlg;
			//-------------------------------------
			SetWindowText( hdlg , "GUI_JobEditor" );
			//<<-----------------------------------------------------------
			KGUI_STANDARD_Set_Function_Size( hdlg );
			//>>-----------------------------------------------------------

			if ( Get_Prm_TITLE_MODE() ) i = 40;
			else                        i = 0;
			j = i / 4;

/*
			// 2005.04.08
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA   ,  30 ,  30 + i , 835 , 416 - i ); // View Area
			KWIN_Item_Resize( hdlg , IDC_CTRL_ADD       ,  47 , 485 , 120 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_DELETE    , 188 , 485 , 120 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_START     , 406 , 485 ,  80 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_ABORT     , 487 , 485 ,  80 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_END       , 568 , 485 ,  80 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_CANCEL    , 649 , 485 ,  80 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG       , 731 , 485 ,  60 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_FIND      , 792 , 485 ,  60 ,  40 );
			KWIN_Item_Resize( hdlg , IDC_CTRL_GO_BACK   , 406 , 485 , 323 ,  40 );
*/

			if ( Get_Prm_DISPLAY_MODE() == 3 ) { // 2011.07.15
				//
				CTRL_1_SELECT_DATA		=	CTR_PROCESS_DATA_1_WAFER;
				CTRL_2_SELECT_DATA		=	CTR_PROCESS_PLOT_1_WAFER;
				CTRL_3_SELECT_DATA		=	CTR_RECIPE_DATA_1_WAFER;
				CTRL_4_SELECT_DATA		=	CTR_PROCESS_DATA_MODULE;
				CTRL_5_SELECT_DATA		=	CTR_FLOW_HISTORY;

				CTRL_6_SELECT_DATA		=	CTR_LOT_SUMMARY_DATA;
				CTRL_7_SELECT_DATA		=	CTR_LOT_SUMMARY_MODULE;

				CTRL_SELECT_MAX_USE		= 5;
			}
			else if ( Get_Prm_DISPLAY_MODE() == 1 ) { // 2011.07.15
				//
				CTRL_1_SELECT_DATA		=	CTR_PROCESS_DATA_1_WAFER;
				CTRL_2_SELECT_DATA		=	CTR_RECIPE_DATA_1_WAFER;
				CTRL_3_SELECT_DATA		=	CTR_PROCESS_DATA_MODULE;
				CTRL_4_SELECT_DATA		=	CTR_FLOW_HISTORY;

				CTRL_5_SELECT_DATA		=	CTR_PROCESS_PLOT_1_WAFER;
				CTRL_6_SELECT_DATA		=	CTR_LOT_SUMMARY_DATA;
				CTRL_7_SELECT_DATA		=	CTR_LOT_SUMMARY_MODULE;

				CTRL_SELECT_MAX_USE		= 4;
				//
			}
			else {
				//
				CTRL_1_SELECT_DATA		=	CTR_PROCESS_DATA_1_WAFER;
				CTRL_2_SELECT_DATA		=	CTR_PROCESS_PLOT_1_WAFER;
				CTRL_3_SELECT_DATA		=	CTR_RECIPE_DATA_1_WAFER;
				CTRL_4_SELECT_DATA		=	CTR_PROCESS_DATA_MODULE;
				CTRL_5_SELECT_DATA		=	CTR_LOT_SUMMARY_DATA;
				CTRL_6_SELECT_DATA		=	CTR_LOT_SUMMARY_MODULE;
				CTRL_7_SELECT_DATA		=	CTR_FLOW_HISTORY;

				CTRL_SELECT_MAX_USE		= 7;
				//
			}
			//
			Get_CTR_Display_String( CTRL_1_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG1	, MessageBuffer );
			Get_CTR_Display_String( CTRL_2_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG2	, MessageBuffer );
			Get_CTR_Display_String( CTRL_3_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG3	, MessageBuffer );
			Get_CTR_Display_String( CTRL_4_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG4	, MessageBuffer );
			Get_CTR_Display_String( CTRL_5_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG5	, MessageBuffer );
			Get_CTR_Display_String( CTRL_6_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG6	, MessageBuffer );
			Get_CTR_Display_String( CTRL_7_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG7	, MessageBuffer );
			Get_CTR_Display_String( CTRL_8_SELECT_DATA , MessageBuffer );	KWIN_Item_String_Display( hdlg , IDC_CTRL_LOG8	, MessageBuffer );
			//====================================================================================================================================================
			// 2005.04.08
			//====================================================================================================================================================
			Global_Pos_X[BUTTON_TREE] = 0;		Global_Pos_Y[BUTTON_TREE] = 0;			Global_Pos_XS[BUTTON_TREE] = 0;		Global_Pos_YS[BUTTON_TREE] = 0;
			//
			if ( Get_Prm_STATUS_VIEW() ) { // 2014.07.17
				Global_Pos_X[BUTTON_DATA] = 30;		Global_Pos_Y[BUTTON_DATA] = 30 + i;		Global_Pos_XS[BUTTON_DATA] = 835;	Global_Pos_YS[BUTTON_DATA] = 416 - i - 22;
				Global_Pos_X[BUTTON_DATA_S] = 30;	Global_Pos_Y[BUTTON_DATA_S] = 30 + i  ;	Global_Pos_XS[BUTTON_DATA_S] = 835;	Global_Pos_YS[BUTTON_DATA_S] = 416 - i - 22; 
				Global_Pos_X[BUTTON_DATA_S2] = 30;	Global_Pos_Y[BUTTON_DATA_S2] = 30 + i;	Global_Pos_XS[BUTTON_DATA_S2] = 835;	Global_Pos_YS[BUTTON_DATA_S2] = 416 - i - 22;
			}
			else {
				Global_Pos_X[BUTTON_DATA] = 30;		Global_Pos_Y[BUTTON_DATA] = 30 + i;		Global_Pos_XS[BUTTON_DATA] = 835;	Global_Pos_YS[BUTTON_DATA] = 416 - i;
				Global_Pos_X[BUTTON_DATA_S] = 30;	Global_Pos_Y[BUTTON_DATA_S] = 30 + i  ;	Global_Pos_XS[BUTTON_DATA_S] = 835;	Global_Pos_YS[BUTTON_DATA_S] = 416 - i  ; 
				Global_Pos_X[BUTTON_DATA_S2] = 30;	Global_Pos_Y[BUTTON_DATA_S2] = 30 + i;	Global_Pos_XS[BUTTON_DATA_S2] = 835;	Global_Pos_YS[BUTTON_DATA_S2] = 416 - i;
			}
			//
			Global_Pos_X[BUTTON_DATA_Sstr] = 30;	Global_Pos_Y[BUTTON_DATA_Sstr] = 30 + i;	Global_Pos_XS[BUTTON_DATA_Sstr] = 835;	Global_Pos_YS[BUTTON_DATA_Sstr] = 416 - i; 
			Global_Pos_X[BUTTON_DATA_S2str] = 30;	Global_Pos_Y[BUTTON_DATA_S2str] = 30 + i;	Global_Pos_XS[BUTTON_DATA_S2str] = 835;	Global_Pos_YS[BUTTON_DATA_S2str] = 416 - i;
			Global_Pos_X[BUTTON_DATA_SPstr] = 30;	Global_Pos_Y[BUTTON_DATA_SPstr] = Global_Pos_Y[BUTTON_DATA_S2] + Global_Pos_YS[BUTTON_DATA_S2];	Global_Pos_XS[BUTTON_DATA_SPstr] = 835;	Global_Pos_YS[BUTTON_DATA_SPstr] = 22; // 2014.02.10
			//
			Global_Pos_X[BUTTON_ADD] = 47;		Global_Pos_Y[BUTTON_ADD] = 485;			Global_Pos_XS[BUTTON_ADD] = 120;	Global_Pos_YS[BUTTON_ADD] = 40;
			Global_Pos_X[BUTTON_DELETE] = 188;	Global_Pos_Y[BUTTON_DELETE] = 485;		Global_Pos_XS[BUTTON_DELETE] = 120;	Global_Pos_YS[BUTTON_DELETE] = 40;
			Global_Pos_X[BUTTON_START] = 406;	Global_Pos_Y[BUTTON_START] = 485;		Global_Pos_XS[BUTTON_START] = 80;	Global_Pos_YS[BUTTON_START] = 40;
			Global_Pos_X[BUTTON_ABORT] = 487;	Global_Pos_Y[BUTTON_ABORT] = 485;		Global_Pos_XS[BUTTON_ABORT] = 80;	Global_Pos_YS[BUTTON_ABORT] = 40;
			Global_Pos_X[BUTTON_END] = 568;		Global_Pos_Y[BUTTON_END] = 485;			Global_Pos_XS[BUTTON_END] = 80;		Global_Pos_YS[BUTTON_END] = 40;
			Global_Pos_X[BUTTON_CANCEL] = 649;	Global_Pos_Y[BUTTON_CANCEL] = 485;		Global_Pos_XS[BUTTON_CANCEL] = 80;	Global_Pos_YS[BUTTON_CANCEL] = 40;
			Global_Pos_X[BUTTON_LOG] = 731;		Global_Pos_Y[BUTTON_LOG] = 485;			Global_Pos_XS[BUTTON_LOG] = 60;		Global_Pos_YS[BUTTON_LOG] = 40;
			Global_Pos_X[BUTTON_FIND] = 792;	Global_Pos_Y[BUTTON_FIND] = 485;		Global_Pos_XS[BUTTON_FIND] = 60;	Global_Pos_YS[BUTTON_FIND] = 40;
			Global_Pos_X[BUTTON_BACK] = 406;	Global_Pos_Y[BUTTON_BACK] = 485;		Global_Pos_XS[BUTTON_BACK] = 323;	Global_Pos_YS[BUTTON_BACK] = 40;
			//
//			s = 875;
			s = wxsize - 54;
			//
			Global_Pos_X[BUTTON_UP] = s;		Global_Pos_Y[BUTTON_UP]    = 50 + i + (58*0);	Global_Pos_XS[BUTTON_UP] = 40;		Global_Pos_YS[BUTTON_UP] = 58;
			Global_Pos_X[BUTTON_SUP] = s;		Global_Pos_Y[BUTTON_SUP]   = 50 + i + (58*1);	Global_Pos_XS[BUTTON_SUP] = 40;		Global_Pos_YS[BUTTON_SUP] = 58;
			Global_Pos_X[BUTTON_SDOWN] = s;		Global_Pos_Y[BUTTON_SDOWN] = 50 + i + (58*2);	Global_Pos_XS[BUTTON_SDOWN] = 40;	Global_Pos_YS[BUTTON_SDOWN] = 58;
			Global_Pos_X[BUTTON_DOWN] = s;		Global_Pos_Y[BUTTON_DOWN]  = 50 + i + (58*3);	Global_Pos_XS[BUTTON_DOWN] = 40;	Global_Pos_YS[BUTTON_DOWN] = 58;
			Global_Pos_X[BUTTON_RIGHT] = s;		Global_Pos_Y[BUTTON_RIGHT] = 60 + i + (58*4);	Global_Pos_XS[BUTTON_RIGHT] = 40;	Global_Pos_YS[BUTTON_RIGHT] = 58;
			Global_Pos_X[BUTTON_LEFT] = s;		Global_Pos_Y[BUTTON_LEFT]  = 60 + i + (58*5);	Global_Pos_XS[BUTTON_LEFT] = 40;	Global_Pos_YS[BUTTON_LEFT] = 58;
			//
			if ( Get_Prm_DISPLAY_MODE() == 1 ) { // 2011.06.15
				//
				Global_Pos_X[BUTTON_DATA] = 30;						Global_Pos_Y[BUTTON_DATA] = 30 + i;		Global_Pos_XS[BUTTON_DATA] = (wxsize-94) / 2;			Global_Pos_YS[BUTTON_DATA] = (wysize-170) - i;
				Global_Pos_X[BUTTON_DATA_S] = 30 + ( (wxsize-94) / 2 ) + 5;	Global_Pos_Y[BUTTON_DATA_S] = 30 + i;	Global_Pos_XS[BUTTON_DATA_S] = ( (wxsize-94) / 2 ) - 5;	Global_Pos_YS[BUTTON_DATA_S] = (wysize-170) - i;
				Global_Pos_X[BUTTON_DATA_S2] = 30 + ( (wxsize-94) / 2 ) + 5;	Global_Pos_Y[BUTTON_DATA_S2] = 30 + i;	Global_Pos_XS[BUTTON_DATA_S2] = ( (wxsize-94) / 2 ) - 5;	Global_Pos_YS[BUTTON_DATA_S2] = (wysize-170) - i;
				//
				Global_Pos_X[BUTTON_DELETE] = 30 + 10;				Global_Pos_Y[BUTTON_DELETE] = (wysize-101);		Global_Pos_XS[BUTTON_DELETE] = 60;	Global_Pos_YS[BUTTON_DELETE] = 40;
				Global_Pos_X[BUTTON_BACK] = 115;					Global_Pos_Y[BUTTON_BACK] = (wysize-101);		Global_Pos_XS[BUTTON_BACK] = 160;	Global_Pos_YS[BUTTON_BACK] = 40;
				Global_Pos_X[BUTTON_FIND] = 275;					Global_Pos_Y[BUTTON_FIND] = (wysize-101);		Global_Pos_XS[BUTTON_FIND] = 80;	Global_Pos_YS[BUTTON_FIND] = 40;
				Global_Pos_X[BUTTON_LOG] = 365;						Global_Pos_Y[BUTTON_LOG] = (wysize-101);			Global_Pos_XS[BUTTON_LOG] = 80;		Global_Pos_YS[BUTTON_LOG] = 40;
			}
			else if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.15
				//
				pchg = 1;
				//
				wxs = (wxsize*100) / 464; // tree view XSIZE
				//
				xgap = wxs/3;
				//
				Global_Pos_X[BUTTON_FIND] = 8;															Global_Pos_Y[BUTTON_FIND] = 8;		Global_Pos_XS[BUTTON_FIND] = xgap;				Global_Pos_YS[BUTTON_FIND] = 50;
				Global_Pos_X[BUTTON_BACK] = Global_Pos_X[BUTTON_FIND] + Global_Pos_XS[BUTTON_FIND];		Global_Pos_Y[BUTTON_BACK] =	8;		Global_Pos_XS[BUTTON_BACK] = xgap;				Global_Pos_YS[BUTTON_BACK] = 50;
				Global_Pos_X[BUTTON_DELETE] = Global_Pos_X[BUTTON_BACK] + Global_Pos_XS[BUTTON_BACK];	Global_Pos_Y[BUTTON_DELETE] = 8;	Global_Pos_XS[BUTTON_DELETE] = wxs - (xgap*2);				Global_Pos_YS[BUTTON_DELETE] = 50;

				Global_Pos_X[BUTTON_TREE] = 8;						Global_Pos_Y[BUTTON_TREE] = 59;		Global_Pos_XS[BUTTON_TREE] = wxs;				Global_Pos_YS[BUTTON_TREE] = (wysize-71);
				//
				KWIN_Item_String_Display( hdlg , IDC_CTRL_GO_BACK , "Reload" );
				//
				if ( Get_Prm_SCROLLBUTTON_MODE() ) {
					//
					xgap = 40;
					//
				}
				else {
					//
					xgap = 0;
					//
				}
				//
				if ( Get_Prm_STATUS_VIEW() ) { // 2014.07.17
					Global_Pos_X[BUTTON_DATA] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA] = 59;				Global_Pos_XS[BUTTON_DATA] = (wxsize-(wxs+xgap+22));		Global_Pos_YS[BUTTON_DATA] = (wysize-71) - 22;
					Global_Pos_X[BUTTON_DATA_S] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA_S] = 59 + 22 + 8;	Global_Pos_XS[BUTTON_DATA_S] = (wxsize-(wxs+xgap+22));		Global_Pos_YS[BUTTON_DATA_S] = (wysize-93) - 8 - 22; 
					Global_Pos_X[BUTTON_DATA_S2] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA_S2] = 59 + 22;		Global_Pos_XS[BUTTON_DATA_S2] = (wxsize-(wxs+xgap+22));		Global_Pos_YS[BUTTON_DATA_S2] = (wysize-93) - 22;
				}
				else {
					Global_Pos_X[BUTTON_DATA] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA] = 59;				Global_Pos_XS[BUTTON_DATA] = (wxsize-(wxs+xgap+22));		Global_Pos_YS[BUTTON_DATA] = (wysize-71);
					Global_Pos_X[BUTTON_DATA_S] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA_S] = 59 + 22 + 8;	Global_Pos_XS[BUTTON_DATA_S] = (wxsize-(wxs+xgap+22));		Global_Pos_YS[BUTTON_DATA_S] = (wysize-93) - 8; 
					Global_Pos_X[BUTTON_DATA_S2] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA_S2] = 59 + 22;		Global_Pos_XS[BUTTON_DATA_S2] = (wxsize-(wxs+xgap+22));		Global_Pos_YS[BUTTON_DATA_S2] = (wysize-93);
				}
				//
				Global_Pos_X[BUTTON_DATA_Sstr] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA_Sstr] = 59;	Global_Pos_XS[BUTTON_DATA_Sstr] = (wxsize-(wxs+xgap+22));	Global_Pos_YS[BUTTON_DATA_Sstr] = 22 + 8;
				Global_Pos_X[BUTTON_DATA_S2str] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA_S2str] = 59;	Global_Pos_XS[BUTTON_DATA_S2str] = (wxsize-(wxs+xgap+22));	Global_Pos_YS[BUTTON_DATA_S2str] = 22;
				Global_Pos_X[BUTTON_DATA_SPstr] = 8 + wxs + 2;		Global_Pos_Y[BUTTON_DATA_SPstr] = Global_Pos_Y[BUTTON_DATA_S2] + Global_Pos_YS[BUTTON_DATA_S2];	Global_Pos_XS[BUTTON_DATA_SPstr] = (wxsize-(wxs+xgap+22));	Global_Pos_YS[BUTTON_DATA_SPstr] = 22; // 2014.02.10
				//
				//
				if ( Get_Prm_SCROLLBUTTON_MODE() ) {
					//
					index = ( Global_Pos_YS[BUTTON_DATA_S2] - 50 ) / 6;
					s = (wxsize-50);
					subid = Global_Pos_Y[BUTTON_DATA_S2] + 20;
					//
					Global_Pos_X[BUTTON_UP] = s+2;		Global_Pos_Y[BUTTON_UP]    = subid + (index*0);	Global_Pos_XS[BUTTON_UP] = 40;		Global_Pos_YS[BUTTON_UP] = index;
					Global_Pos_X[BUTTON_SUP] = s+2;		Global_Pos_Y[BUTTON_SUP]   = subid + (index*1);	Global_Pos_XS[BUTTON_SUP] = 40;		Global_Pos_YS[BUTTON_SUP] = index;
					Global_Pos_X[BUTTON_SDOWN] = s+2;		Global_Pos_Y[BUTTON_SDOWN] = subid + (index*2);	Global_Pos_XS[BUTTON_SDOWN] = 40;	Global_Pos_YS[BUTTON_SDOWN] = index;
					Global_Pos_X[BUTTON_DOWN] = s+2;		Global_Pos_Y[BUTTON_DOWN]  = subid + (index*3);	Global_Pos_XS[BUTTON_DOWN] = 40;	Global_Pos_YS[BUTTON_DOWN] = index;
					Global_Pos_X[BUTTON_RIGHT] = s+2;		Global_Pos_Y[BUTTON_RIGHT] = subid + 10 + (index*4);	Global_Pos_XS[BUTTON_RIGHT] = 40;	Global_Pos_YS[BUTTON_RIGHT] = index;
					Global_Pos_X[BUTTON_LEFT] = s+2;		Global_Pos_Y[BUTTON_LEFT]  = subid + 10 + (index*5);	Global_Pos_XS[BUTTON_LEFT] = 40;	Global_Pos_YS[BUTTON_LEFT] = index;
					//
				}
				//
				if ( Get_Prm_SAVEFILE_MODE() ) { // 2012.06.26
//					s = (wxsize-(wxs+59)) / (CTRL_SELECT_MAX_USE+1);
					s = (wxsize-(wxs+xgap+22+2)) / (CTRL_SELECT_MAX_USE+1);
					j = (wxsize-(wxs+xgap+22+2)) - ( s * (CTRL_SELECT_MAX_USE) );
				}
				else {
//					s = (wxsize-(wxs+59)) / CTRL_SELECT_MAX_USE;
					s = (wxsize-(wxs+xgap+22+2)) / CTRL_SELECT_MAX_USE;
					j = (wxsize-(wxs+xgap+22+2)) - ( s * (CTRL_SELECT_MAX_USE-1) );
				}
				//
				index = (wxs+8+2+2);
				//
				// mok 2012.05.14
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				Global_Pos_X[BUTTON_CTRL1] = index;		Global_Pos_Y[BUTTON_CTRL1]  = 8;	Global_Pos_XS[BUTTON_CTRL1] = s;	Global_Pos_YS[BUTTON_CTRL1] = 50;		index = index + s;
				Global_Pos_X[BUTTON_CTRL2] = index;		Global_Pos_Y[BUTTON_CTRL2]  = 8;	Global_Pos_XS[BUTTON_CTRL2] = s;	Global_Pos_YS[BUTTON_CTRL2] = 50;		index = index + s;
				Global_Pos_X[BUTTON_CTRL3] = index;		Global_Pos_Y[BUTTON_CTRL3]  = 8;	Global_Pos_XS[BUTTON_CTRL3] = s;	Global_Pos_YS[BUTTON_CTRL3] = 50;		index = index + s;
				Global_Pos_X[BUTTON_CTRL4] = index;		Global_Pos_Y[BUTTON_CTRL4]  = 8;	Global_Pos_XS[BUTTON_CTRL4] = s;	Global_Pos_YS[BUTTON_CTRL4] = 50;		index = index + s;
				Global_Pos_X[BUTTON_CTRL5] = index;		Global_Pos_Y[BUTTON_CTRL5]  = 8;	Global_Pos_XS[BUTTON_CTRL5] = s;	Global_Pos_YS[BUTTON_CTRL5] = 50;		index = index + s;
				Global_Pos_X[BUTTON_CTRL6] = index;		Global_Pos_Y[BUTTON_CTRL6]  = 8;	Global_Pos_XS[BUTTON_CTRL6] = s;	Global_Pos_YS[BUTTON_CTRL6] = 50;		index = index + s;
				Global_Pos_X[BUTTON_CTRL7] = index;		Global_Pos_Y[BUTTON_CTRL7]  = 8;	Global_Pos_XS[BUTTON_CTRL7] = s;	Global_Pos_YS[BUTTON_CTRL7] = 50;		index = index + s;
				Global_Pos_X[BUTTON_CTRL8] = index;		Global_Pos_Y[BUTTON_CTRL8]  = 8;	Global_Pos_XS[BUTTON_CTRL8] = s;	Global_Pos_YS[BUTTON_CTRL8] = 50;		index = index + s;
				//
				if ( !Get_Prm_SAVEFILE_MODE() ) {
					if      ( CTRL_SELECT_MAX_USE == 5 ) Global_Pos_XS[BUTTON_CTRL5] = j;
					else if ( CTRL_SELECT_MAX_USE == 6 ) Global_Pos_XS[BUTTON_CTRL6] = j;
					else if ( CTRL_SELECT_MAX_USE == 7 ) Global_Pos_XS[BUTTON_CTRL7] = j;
					else if ( CTRL_SELECT_MAX_USE == 8 ) Global_Pos_XS[BUTTON_CTRL8] = j;
				}
				//
				Global_Pos_X[BUTTON_CTRL_SAVE] = (wxs+12) + ( s * CTRL_SELECT_MAX_USE ) ;	Global_Pos_Y[BUTTON_CTRL_SAVE]  = 8;	Global_Pos_XS[BUTTON_CTRL_SAVE] = j;	Global_Pos_YS[BUTTON_CTRL_SAVE] = 50;		index = index + s; // 2012.06.26

				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG1   , index , 10 , s , 50 );	index = index + s;	
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG2   , index , 10 , s , 50 );	index = index + s;
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG3   , index , 10 , s , 50 );	index = index + s;
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG4   , index , 10 , s , 50 );	index = index + s;
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG5   , index , 10 , s , 50 );	index = index + s;
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG6   , index , 10 , s , 50 );	index = index + s;
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG7   , index , 10 , s , 50 );	index = index + s;
//				KWIN_Item_Resize( hdlg , IDC_CTRL_LOG8   , index , 10 , s , 50 );	index = index + s;
				//
				//
				//
				Global_Pos_X[BUTTON_LOG] = 365;						Global_Pos_Y[BUTTON_LOG] = (wysize-101);		Global_Pos_XS[BUTTON_LOG] = 80;					Global_Pos_YS[BUTTON_LOG] = 40;
				//
			}
			//
			//
			// 2016.08.17
			//
			Global_Pos_X[BUTTON_DATA_SPMS] = Global_Pos_X[BUTTON_DATA_Sstr];
			Global_Pos_Y[BUTTON_DATA_SPMS] = Global_Pos_Y[BUTTON_DATA_Sstr];
			Global_Pos_XS[BUTTON_DATA_SPMS] = Global_Pos_XS[BUTTON_DATA_Sstr] - 50;
			Global_Pos_YS[BUTTON_DATA_SPMS] = Global_Pos_YS[BUTTON_DATA_Sstr] + MAX_BUTTON_Y_SPM_MORE;
			//
			Global_Pos_X[BUTTON_DATA_SPM] = Global_Pos_X[BUTTON_DATA_S];
			Global_Pos_Y[BUTTON_DATA_SPM] = Global_Pos_Y[BUTTON_DATA_S] + MAX_BUTTON_Y_SPM_MORE;
			Global_Pos_XS[BUTTON_DATA_SPM] = Global_Pos_XS[BUTTON_DATA_S];
			Global_Pos_YS[BUTTON_DATA_SPM] = Global_Pos_YS[BUTTON_DATA_S] - MAX_BUTTON_Y_SPM_MORE;
			//
			Global_Pos_X[BUTTON_SPM] = Global_Pos_X[BUTTON_DATA_SPMS] + Global_Pos_XS[BUTTON_DATA_SPMS];
			Global_Pos_Y[BUTTON_SPM] = Global_Pos_Y[BUTTON_DATA_SPMS];
			Global_Pos_XS[BUTTON_SPM] = 50;
			Global_Pos_YS[BUTTON_SPM] = Global_Pos_YS[BUTTON_DATA_SPMS];
			//
			//====================================================================================================================================================
			//
			FileEnd = FALSE;
			//
			if( Get_Prm_DISPLAY_MODE() == 2 ) // mok 2012.05.14
			{
				GET_SYSTEM_FILE( MessageBuffer , "JobEditor2(%s).cfg" , PROGRAM_FUNCTION_READ() ); // 2017.03.12
			}
			else
			{
				GET_SYSTEM_FILE( MessageBuffer , "JobEditor(%s).cfg" , PROGRAM_FUNCTION_READ() ); // 2017.03.12
			}
			//
			if ( ( hFile = _lopen( MessageBuffer , OF_READ ) ) != -1 ) {
				FileEnd = TRUE;
			}
			else {
				if( Get_Prm_DISPLAY_MODE() == 2 ) // mok 2012.05.14
				{
					//
					GET_SYSTEM_FILE( MessageBuffer , "Layout\\JobEditor2\\Layout.cfg" , NULL ); // 2017.03.12
					//
					if ( ( hFile = _lopen( MessageBuffer , OF_READ ) ) != -1 ) {
						FileEnd = TRUE;
					}
				}
				else
				{
					//
					GET_SYSTEM_FILE( MessageBuffer , "Layout\\JobEditor\\Layout.cfg" , NULL ); // 2017.03.12
					//
					if ( ( hFile = _lopen( MessageBuffer , OF_READ ) ) != -1 ) {
						FileEnd = TRUE;
					}
				}
				//
			}
			//
			if ( FileEnd ) {
				//
				while( FileEnd ) {
					FileEnd = H_Get_Line_String_From_File2_Include_Index( hFile , MessageBuffer2 , 255 , &ReadCnt , retindexs , retindexsz );
					if ( ReadCnt > 1 ) {
						if ( Get_Data_From_String_with_index( MessageBuffer2 , MessageBuffer , retindexs[0] , retindexsz[0] ) ) {
							posindex = -1; 
							if      ( STRCMP_L( "BUTTON_OFFSET" , MessageBuffer ) ) {
								//
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[1] , retindexsz[1] , MessageBuffer , -999 , 999 , &posdata ) ) {
									//
									layoutxoffset = posdata;
									layoutxsoffset = - ( posdata * 2 );
									//
								}
								//
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[2] , retindexsz[2] , MessageBuffer , -999 , 999 , &posdata ) ) {
									//
									layoutyoffset = posdata;
									layoutysoffset = - ( posdata * 2 );
									//
								}
								//
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[3] , retindexsz[3] , MessageBuffer , -999 , 999 , &posdata ) ) {
									//
									if ( posdata != -999 ) layoutxsoffset = posdata;
									//
								}
								//
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[4] , retindexsz[4] , MessageBuffer , -999 , 999 , &posdata ) ) {
									//
									if ( posdata != -999 ) layoutysoffset = posdata;
									//
								}
								//
								continue;
								//
							}

							if      ( STRCMP_L( "BUTTON_DATA" , MessageBuffer ) ) posindex = BUTTON_DATA;
							else if ( STRCMP_L( "BUTTON_DATAS" , MessageBuffer ) ) posindex = BUTTON_DATA_S;
							else if ( STRCMP_L( "BUTTON_DATAS2" , MessageBuffer ) ) posindex = BUTTON_DATA_S2;
							else if ( STRCMP_L( "BUTTON_DATASS" , MessageBuffer ) ) posindex = BUTTON_DATA_Sstr;
							else if ( STRCMP_L( "BUTTON_DATAS2S" , MessageBuffer ) ) posindex = BUTTON_DATA_S2str;
							else if ( STRCMP_L( "BUTTON_DATASPS" , MessageBuffer ) ) posindex = BUTTON_DATA_SPstr; // 2014.02.10
							//
							else if ( STRCMP_L( "BUTTON_DATASPM" , MessageBuffer ) ) posindex = BUTTON_DATA_SPM; // 2016.08.17
							else if ( STRCMP_L( "BUTTON_DATASPMS" , MessageBuffer ) ) posindex = BUTTON_DATA_SPMS; // 2016.08.17
							//
							else if ( STRCMP_L( "BUTTON_TREE" , MessageBuffer ) ) posindex = BUTTON_TREE;
							else if ( STRCMP_L( "BUTTON_ADD" , MessageBuffer ) )  posindex = BUTTON_ADD;
							else if ( STRCMP_L( "BUTTON_DELETE" , MessageBuffer ) )  posindex = BUTTON_DELETE;
							else if ( STRCMP_L( "BUTTON_START" , MessageBuffer ) )  posindex = BUTTON_START;
							else if ( STRCMP_L( "BUTTON_ABORT" , MessageBuffer ) )  posindex = BUTTON_ABORT;
							else if ( STRCMP_L( "BUTTON_END" , MessageBuffer ) )  posindex = BUTTON_END;
							else if ( STRCMP_L( "BUTTON_CANCEL" , MessageBuffer ) )  posindex = BUTTON_CANCEL;
							else if ( STRCMP_L( "BUTTON_LOG" , MessageBuffer ) )  posindex = BUTTON_LOG;
							else if ( STRCMP_L( "BUTTON_FIND"  , MessageBuffer ) )  posindex = BUTTON_FIND;
							else if ( STRCMP_L( "BUTTON_BACK" , MessageBuffer ) )  posindex = BUTTON_BACK;
							else if ( STRCMP_L( "BUTTON_UP" , MessageBuffer ) )  posindex = BUTTON_UP;
							else if ( STRCMP_L( "BUTTON_SUP" , MessageBuffer ) )  posindex = BUTTON_SUP;
							else if ( STRCMP_L( "BUTTON_SDOWN" , MessageBuffer ) )  posindex = BUTTON_SDOWN;
							else if ( STRCMP_L( "BUTTON_DOWN" , MessageBuffer ) )  posindex = BUTTON_DOWN;
							else if ( STRCMP_L( "BUTTON_RIGHT" , MessageBuffer ) )  posindex = BUTTON_RIGHT;
							else if ( STRCMP_L( "BUTTON_LEFT" , MessageBuffer ) )  posindex = BUTTON_LEFT;
							//
							else if ( STRCMP_L( "BUTTON_SPM" , MessageBuffer ) ) posindex = BUTTON_SPM; // 2016.08.17
							//---------------------------------------------------------------------------------------------------------mok 2012.05.14
							else if ( STRCMP_L( "BUTTON_CTRL1" , MessageBuffer ) )  posindex = BUTTON_CTRL1;
							else if ( STRCMP_L( "BUTTON_CTRL2" , MessageBuffer ) )  posindex = BUTTON_CTRL2;
							else if ( STRCMP_L( "BUTTON_CTRL3" , MessageBuffer ) )  posindex = BUTTON_CTRL3;
							else if ( STRCMP_L( "BUTTON_CTRL4" , MessageBuffer ) )  posindex = BUTTON_CTRL4;
							else if ( STRCMP_L( "BUTTON_CTRL5" , MessageBuffer ) )  posindex = BUTTON_CTRL5;
							else if ( STRCMP_L( "BUTTON_CTRL6" , MessageBuffer ) )  posindex = BUTTON_CTRL6;
							else if ( STRCMP_L( "BUTTON_CTRL7" , MessageBuffer ) )  posindex = BUTTON_CTRL7;
							else if ( STRCMP_L( "BUTTON_CTRL8" , MessageBuffer ) )  posindex = BUTTON_CTRL8;
							else if ( STRCMP_L( "BUTTON_SAVE"  , MessageBuffer ) )  posindex = BUTTON_CTRL_SAVE; // 2012.06.26
							//---------------------------------------------------------------------------------------------------------
							else if ( STRCMP_L( "BUTTON_FONT" , MessageBuffer ) ) {
								posindex = -1;
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[1] , retindexsz[1] , MessageBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_Font = posdata;
									if ( ReadCnt > 2 ) {
										if ( !Get_Data_From_String_for_STRING_with_index( MessageBuffer2 , retindexs[2] , retindexsz[2] , MessageBuffer , 255 , MessageBuffer3 ) ) {
											strcpy( Global_Pos_Font_Name , MessageBuffer3 );
										}
										if ( ReadCnt > 3 ) {
											if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[3] , retindexsz[3] , MessageBuffer , 0 , 9999 , &posdata ) ) {
												Global_Pos_Font2 = posdata;
											}
										}
									}
								}
							}
							//
							if ( ( posindex != -1 ) && ( ReadCnt > 4 ) ) {
								//
								pchg = 2;
								//
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[1] , retindexsz[1] , MessageBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_X[posindex] = posdata;
								}
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[2] , retindexsz[2] , MessageBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_Y[posindex] = posdata;
								}
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[3] , retindexsz[3] , MessageBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_XS[posindex] = posdata;
								}
								if ( Get_Data_From_String_for_INT_with_index( MessageBuffer2 , retindexs[4] , retindexsz[4] , MessageBuffer , 0 , 9999 , &posdata ) ) {
									Global_Pos_YS[posindex] = posdata;
								}
							}
						}
					}
				}
				_lclose( hFile );
			}
			//===============================================================================
			//
			// 2017.02.22
			//
			if( Get_Prm_DISPLAY_MODE() == 2 ) // mok 2012.05.14
			{
				//
				GET_SYSTEM_FILE( MessageBuffer , "Layout\\JobEditor2" , NULL ); // 2017.03.12
				//
				KWIN_SET_USER_BUTTON_GROUP( 0 , MessageBuffer , hdlg , "background.bmp" );
			}
			else {
				//
				GET_SYSTEM_FILE( MessageBuffer , "Layout\\JobEditor" , NULL ); // 2017.03.12
				//
				KWIN_SET_USER_BUTTON_GROUP( 0 , MessageBuffer , hdlg , "background.bmp" );
			}
			//
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_ADD    ) , "add_Ena.bmp"		, "add_Dis.bmp"		);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_START  ) , "Start_Ena.bmp"		, "Start_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_ABORT  ) , "Abort_Ena.bmp"		, "Abort_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_END    ) , "End_Ena.bmp"		, "End_Dis.bmp"		);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_CANCEL ) , "Cancel_Ena.bmp"	, "Cancel_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG    ) , "Log_Ena.bmp"		, "Log_Dis.bmp"		);

			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_DELETE ) , "Delete_Ena.bmp"	, "Delete_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_FIND   ) , "Find_Ena.bmp"		, "Find_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_GO_BACK) , "BackReload_Ena.bmp", "BackReload_Dis.bmp"	);

			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG1	) , "Log1_Ena.bmp"		, "Log1_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG2   ) , "Log2_Ena.bmp"		, "Log2_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG3   ) , "Log3_Ena.bmp"		, "Log3_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG4   ) , "Log4_Ena.bmp"		, "Log4_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG5   ) , "Log5_Ena.bmp"		, "Log5_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG6   ) , "Log6_Ena.bmp"		, "Log6_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG7   ) , "Log7_Ena.bmp"		, "Log7_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LOG8   ) , "Log8_Ena.bmp"		, "Log8_Dis.bmp"	);

			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_SAVE   ) , "Save_Ena.bmp"		, "Save_Dis.bmp"	);

			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_UP     ) , "Up_Ena.bmp"		, "Up_Dis.bmp"		);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_SUP    ) , "SUp_Ena.bmp"		, "SUp_Dis.bmp"		);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_SDOWN  ) , "SDown_Ena.bmp"		, "SDown_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_DOWN   ) , "Down_Ena.bmp"		, "Down_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_RIGHT  ) , "Right_Ena.bmp"		, "Right_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CTRL_LEFT   ) , "Left_Ena.bmp"		, "Left_Dis.bmp"	);


			//===============================================================================
			if ( ( Global_Pos_Font > 0 ) && ( Global_Pos_Font <= 128 ) ) {
				HFONT ixhFont;
				if ( strlen( Global_Pos_Font_Name ) <= 0 ) {
					ixhFont = CreateFont( Global_Pos_Font , 0 , 0 , 0 , 0 , 0 , 0 , 0 , DEFAULT_CHARSET , 0 , 0 , 0 , 0 , "Arial" );
				}
				else {
					ixhFont = CreateFont( Global_Pos_Font , 0 , 0 , 0 , 0 , 0 , 0 , 0 , DEFAULT_CHARSET , 0 , 0 , 0 , 0 , Global_Pos_Font_Name );
				}
				SendMessage( GetDlgItem( hdlg ,IDC_TREE            ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA    ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA_S  ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA_SS ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA_S2 ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA_S2S) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA_SPS) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) ); // 2014.02.10
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_ADD        ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_DELETE     ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_START      ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_ABORT      ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_END        ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_CANCEL     ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_LOG        ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_FIND       ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_GO_BACK    ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
				//
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA_SPM  ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) ); // 2016.08.17
				SendMessage( GetDlgItem( hdlg ,IDC_LIST_JOBDATA_SPMS ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) ); // 2016.08.17
				SendMessage( GetDlgItem( hdlg ,IDC_CTRL_SPM			 ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) ); // 2016.08.17
				//
			}
			else {// mok 2011. 9. 8
				HFONT ixhFont;
				ixhFont = CreateFont( 17 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , DEFAULT_CHARSET , 0 , 0 , 0 , 0 , "Arial" );
				SendMessage( GetDlgItem( hdlg ,IDC_TREE            ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
			
			}
			//====================================================================================================================================================
			KWIN_Item_Resize( hdlg , IDC_TREE           , Global_Pos_X[BUTTON_TREE]   , Global_Pos_Y[BUTTON_TREE]   , Global_Pos_XS[BUTTON_TREE]   , Global_Pos_YS[BUTTON_TREE] );
		
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA   , Global_Pos_X[BUTTON_DATA]   , Global_Pos_Y[BUTTON_DATA]   , Global_Pos_XS[BUTTON_DATA]   , Global_Pos_YS[BUTTON_DATA] ); // View Area
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA_S , Global_Pos_X[BUTTON_DATA_S] , Global_Pos_Y[BUTTON_DATA_S] , Global_Pos_XS[BUTTON_DATA_S] , Global_Pos_YS[BUTTON_DATA_S] ); // View Area
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA_S2, Global_Pos_X[BUTTON_DATA_S2] , Global_Pos_Y[BUTTON_DATA_S2] , Global_Pos_XS[BUTTON_DATA_S2] , Global_Pos_YS[BUTTON_DATA_S2] ); // View Area
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA_SS, Global_Pos_X[BUTTON_DATA_Sstr] , Global_Pos_Y[BUTTON_DATA_Sstr] , Global_Pos_XS[BUTTON_DATA_Sstr] , Global_Pos_YS[BUTTON_DATA_Sstr] ); // View Area
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA_S2S, Global_Pos_X[BUTTON_DATA_S2str] , Global_Pos_Y[BUTTON_DATA_S2str] , Global_Pos_XS[BUTTON_DATA_S2str] , Global_Pos_YS[BUTTON_DATA_S2str] ); // View Area
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA_SPS, Global_Pos_X[BUTTON_DATA_SPstr] , Global_Pos_Y[BUTTON_DATA_SPstr] , Global_Pos_XS[BUTTON_DATA_SPstr] , Global_Pos_YS[BUTTON_DATA_SPstr] ); // View Area
			//
			KWIN_Item_Resize( hdlg , IDC_CTRL_ADD       , Global_Pos_X[BUTTON_ADD] , Global_Pos_Y[BUTTON_ADD] , Global_Pos_XS[BUTTON_ADD] , Global_Pos_YS[BUTTON_ADD] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_DELETE    , Global_Pos_X[BUTTON_DELETE] , Global_Pos_Y[BUTTON_DELETE] , Global_Pos_XS[BUTTON_DELETE] , Global_Pos_YS[BUTTON_DELETE] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_START     , Global_Pos_X[BUTTON_START] , Global_Pos_Y[BUTTON_START] , Global_Pos_XS[BUTTON_START] , Global_Pos_YS[BUTTON_START] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_ABORT     , Global_Pos_X[BUTTON_ABORT] , Global_Pos_Y[BUTTON_ABORT] , Global_Pos_XS[BUTTON_ABORT] , Global_Pos_YS[BUTTON_ABORT] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_END       , Global_Pos_X[BUTTON_END] , Global_Pos_Y[BUTTON_END] , Global_Pos_XS[BUTTON_END] , Global_Pos_YS[BUTTON_END] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_CANCEL    , Global_Pos_X[BUTTON_CANCEL] , Global_Pos_Y[BUTTON_CANCEL] , Global_Pos_XS[BUTTON_CANCEL] , Global_Pos_YS[BUTTON_CANCEL] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG       , Global_Pos_X[BUTTON_LOG] , Global_Pos_Y[BUTTON_LOG] , Global_Pos_XS[BUTTON_LOG] , Global_Pos_YS[BUTTON_LOG] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_FIND      , Global_Pos_X[BUTTON_FIND] , Global_Pos_Y[BUTTON_FIND] , Global_Pos_XS[BUTTON_FIND] , Global_Pos_YS[BUTTON_FIND] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_GO_BACK   , Global_Pos_X[BUTTON_BACK] , Global_Pos_Y[BUTTON_BACK] , Global_Pos_XS[BUTTON_BACK] , Global_Pos_YS[BUTTON_BACK] );
			//-------------------------------------------------------------------------------------------------------------------------------------------------------------------- mok 2012.05.14
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG1   , Global_Pos_X[BUTTON_CTRL1] , Global_Pos_Y[BUTTON_CTRL1] , Global_Pos_XS[BUTTON_CTRL1] , Global_Pos_YS[BUTTON_CTRL1] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG2   , Global_Pos_X[BUTTON_CTRL2] , Global_Pos_Y[BUTTON_CTRL2] , Global_Pos_XS[BUTTON_CTRL2] , Global_Pos_YS[BUTTON_CTRL2] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG3   , Global_Pos_X[BUTTON_CTRL3] , Global_Pos_Y[BUTTON_CTRL3] , Global_Pos_XS[BUTTON_CTRL3] , Global_Pos_YS[BUTTON_CTRL3] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG4   , Global_Pos_X[BUTTON_CTRL4] , Global_Pos_Y[BUTTON_CTRL4] , Global_Pos_XS[BUTTON_CTRL4] , Global_Pos_YS[BUTTON_CTRL4] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG5   , Global_Pos_X[BUTTON_CTRL5] , Global_Pos_Y[BUTTON_CTRL5] , Global_Pos_XS[BUTTON_CTRL5] , Global_Pos_YS[BUTTON_CTRL5] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG6   , Global_Pos_X[BUTTON_CTRL6] , Global_Pos_Y[BUTTON_CTRL6] , Global_Pos_XS[BUTTON_CTRL6] , Global_Pos_YS[BUTTON_CTRL6] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG7   , Global_Pos_X[BUTTON_CTRL7] , Global_Pos_Y[BUTTON_CTRL7] , Global_Pos_XS[BUTTON_CTRL7] , Global_Pos_YS[BUTTON_CTRL7] );
			KWIN_Item_Resize( hdlg , IDC_CTRL_LOG8   , Global_Pos_X[BUTTON_CTRL8] , Global_Pos_Y[BUTTON_CTRL8] , Global_Pos_XS[BUTTON_CTRL8] , Global_Pos_YS[BUTTON_CTRL8] );
			//
			KWIN_Item_Resize( hdlg , IDC_CTRL_SAVE   , Global_Pos_X[BUTTON_CTRL_SAVE] , Global_Pos_Y[BUTTON_CTRL_SAVE] , Global_Pos_XS[BUTTON_CTRL_SAVE] , Global_Pos_YS[BUTTON_CTRL_SAVE] );
			//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
			//
			// 2016.08.17
			//
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA_SPM  , Global_Pos_X[BUTTON_DATA_SPM] , Global_Pos_Y[BUTTON_DATA_SPM] , Global_Pos_XS[BUTTON_DATA_SPM] , Global_Pos_YS[BUTTON_DATA_SPM] ); // View Area
			KWIN_Item_Resize( hdlg , IDC_LIST_JOBDATA_SPMS , Global_Pos_X[BUTTON_DATA_SPMS] , Global_Pos_Y[BUTTON_DATA_SPMS] , Global_Pos_XS[BUTTON_DATA_SPMS] , Global_Pos_YS[BUTTON_DATA_SPMS] ); // View Area
			KWIN_Item_Resize( hdlg , IDC_CTRL_SPM		   , Global_Pos_X[BUTTON_SPM] , Global_Pos_Y[BUTTON_SPM] , Global_Pos_XS[BUTTON_SPM] , Global_Pos_YS[BUTTON_SPM] );
			//
			//====================================================================================================================================================
			//
			if ( Get_Prm_SCROLLBUTTON_MODE() ) {
//				KWIN_Item_Resize( hdlg , IDC_CTRL_UP        , 875 ,  50 + i ,  50 ,  78 - j );
//				KWIN_Item_Resize( hdlg , IDC_CTRL_SUP       , 875 , 149 + i - j ,  50 ,  78 - j );
//				KWIN_Item_Resize( hdlg , IDC_CTRL_SDOWN     , 875 , 248 + i - j - j ,  50 ,  78 - j );
//				KWIN_Item_Resize( hdlg , IDC_CTRL_DOWN      , 875 , 347 + i - j - j - j ,  50 ,  78 - j );
/*
				// 2005.04.08
				KWIN_Item_Resize( hdlg , IDC_CTRL_UP        , 875 ,  50 + i ,  50 ,  50 - j );
				KWIN_Item_Resize( hdlg , IDC_CTRL_SUP       , 875 , 120 + i - j ,  50 ,  50 - j );
				KWIN_Item_Resize( hdlg , IDC_CTRL_SDOWN     , 875 , 190 + i - j - j ,  50 ,  50 - j );
				KWIN_Item_Resize( hdlg , IDC_CTRL_DOWN      , 875 , 260 + i - j - j - j ,  50 ,  50 - j );
				KWIN_Item_Resize( hdlg , IDC_CTRL_RIGHT     , 875 , 330 + i - j - j - j - j ,  50 ,  50 - j );
				KWIN_Item_Resize( hdlg , IDC_CTRL_LEFT      , 875 , 400 + i - j - j - j - j - j,  50 ,  50 - j );
*/
				//====================================================================================================================================================
				// 2005.04.08
				//====================================================================================================================================================
				KWIN_Item_Resize( hdlg , IDC_CTRL_UP        , Global_Pos_X[BUTTON_UP] , Global_Pos_Y[BUTTON_UP] , Global_Pos_XS[BUTTON_UP] , Global_Pos_YS[BUTTON_UP] );
				KWIN_Item_Resize( hdlg , IDC_CTRL_SUP       , Global_Pos_X[BUTTON_SUP] , Global_Pos_Y[BUTTON_SUP] , Global_Pos_XS[BUTTON_SUP] , Global_Pos_YS[BUTTON_SUP] );
				KWIN_Item_Resize( hdlg , IDC_CTRL_SDOWN     , Global_Pos_X[BUTTON_SDOWN] , Global_Pos_Y[BUTTON_SDOWN] , Global_Pos_XS[BUTTON_SDOWN] , Global_Pos_YS[BUTTON_SDOWN] );
				KWIN_Item_Resize( hdlg , IDC_CTRL_DOWN      , Global_Pos_X[BUTTON_DOWN] , Global_Pos_Y[BUTTON_DOWN] , Global_Pos_XS[BUTTON_DOWN] , Global_Pos_YS[BUTTON_DOWN] );
				KWIN_Item_Resize( hdlg , IDC_CTRL_RIGHT     , Global_Pos_X[BUTTON_RIGHT] , Global_Pos_Y[BUTTON_RIGHT] , Global_Pos_XS[BUTTON_RIGHT] , Global_Pos_YS[BUTTON_RIGHT] );
				KWIN_Item_Resize( hdlg , IDC_CTRL_LEFT      , Global_Pos_X[BUTTON_LEFT] , Global_Pos_Y[BUTTON_LEFT] , Global_Pos_XS[BUTTON_LEFT] , Global_Pos_YS[BUTTON_LEFT] );
				//====================================================================================================================================================
			}
			else {
				KWIN_Item_Hide( hdlg , IDC_CTRL_UP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SUP );
				KWIN_Item_Hide( hdlg , IDC_CTRL_SDOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_DOWN );
				KWIN_Item_Hide( hdlg , IDC_CTRL_RIGHT );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LEFT );
			}
			//
			//=============================================================
			// 2006.08.30
			//=============================================================
			if ( Get_Prm_DELETE_HIDE() ) {
				KWIN_Item_Hide( hdlg , IDC_CTRL_DELETE );
			}
			//
			KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_S );
			KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_SS );
			//
			KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_S2 );
			KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_S2S );
			//
//			KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_SPS ); // 2014.02.13 // 2014.07.17
//			if ( !Get_Prm_STATUS_VIEW() ) KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_SPS ); // 2014.07.17 // 2015.10.28
			if ( ( Get_Prm_DISPLAY_MODE() < 2 ) || !Get_Prm_STATUS_VIEW() ) KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_SPS ); // 2014.07.17 // 2015.10.28
			//
			if ( ( Get_Prm_DISPLAY_MODE() < 2 ) || ( !Get_Prm_SAVEFILE_MODE() ) ) { // 2012.06.26
				KWIN_Item_Hide( hdlg , IDC_CTRL_SAVE );
			}
			//
			if ( Get_Prm_DISPLAY_MODE() < 2 ) {
				KWIN_Item_Hide( hdlg , IDC_TREE );
			}
			//
			if ( Get_Prm_DISPLAY_MODE() == 0 ) { // 2011.06.15
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG1 );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG2 );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG3 );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG4 );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG5 );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG6 );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG7 );
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG8 );
			}
			else {
				KWIN_Item_Hide( hdlg , IDC_CTRL_LOG );
				KWIN_Item_Disable( hdlg , IDC_CTRL_LOG1 );
				KWIN_Item_Disable( hdlg , IDC_CTRL_LOG2 );
				KWIN_Item_Disable( hdlg , IDC_CTRL_LOG3 );
				KWIN_Item_Disable( hdlg , IDC_CTRL_LOG4 );
				if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
					if ( CTRL_SELECT_MAX_USE >= 5 ) KWIN_Item_Disable( hdlg , IDC_CTRL_LOG5 ); else KWIN_Item_Hide( hdlg , IDC_CTRL_LOG5 );
					if ( CTRL_SELECT_MAX_USE >= 6 ) KWIN_Item_Disable( hdlg , IDC_CTRL_LOG6 ); else KWIN_Item_Hide( hdlg , IDC_CTRL_LOG6 );
					if ( CTRL_SELECT_MAX_USE >= 7 ) KWIN_Item_Disable( hdlg , IDC_CTRL_LOG7 ); else KWIN_Item_Hide( hdlg , IDC_CTRL_LOG7 );
					if ( CTRL_SELECT_MAX_USE >= 8 ) KWIN_Item_Disable( hdlg , IDC_CTRL_LOG8 ); else KWIN_Item_Hide( hdlg , IDC_CTRL_LOG8 );
				}
				else {
					KWIN_Item_Hide( hdlg , IDC_CTRL_LOG5 );
					KWIN_Item_Hide( hdlg , IDC_CTRL_LOG6 );
					KWIN_Item_Hide( hdlg , IDC_CTRL_LOG7 );
					KWIN_Item_Hide( hdlg , IDC_CTRL_LOG8 );
				}
			}
			//=============================================================
			if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
				g_Tree_Image = ImageList_LoadBitmap( GetModuleHandle( NULL ) , MAKEINTRESOURCE( IDB_ICON_BITMAP ) , 16 , 1 , RGB(255,255,255) );
				TreeView_SetImageList( GetDlgItem( hdlg , IDC_TREE ) , g_Tree_Image , TVSIL_NORMAL );
			}
			//
			JobData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) );
			//
			_UTIL_LV_SET_EXTEND_STYLE( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , FALSE , 1 );
			//=============================================================

			if ( pchg == 0 ) { // 2012.04.23
				//
				wxsize = GET_WINDOW_POSITION_FOR_GUI_XS(3);
				wysize = GET_WINDOW_POSITION_FOR_GUI_YS(3);
				//
				if ( KWIN_Item_Size_ReSetM_Check( wxsize + layoutxsoffset, wysize + layoutysoffset, 0 , 0 , &wxp , &wyp ) ) {
					//
					KWIN_Item_Size_ReSetM_Init( hdlg , wxp , wyp , layoutxoffset , layoutyoffset , ( wxp == 1 ) ? 0 : -2 , ( wyp == 1 ) ? 0 : -2 );
					//
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA     );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_S   );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SS  );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_S2  );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_S2S );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SPS ); // 2014.02.13
					//
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_ADD         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_DELETE      );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_START       );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_ABORT       );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_END         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_CANCEL      );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_FIND        );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_GO_BACK     );
					KWIN_Item_Size_ReSetM_Add( IDC_TREE			    );
					//
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SPM ); // 2016.08.17
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SPMS ); // 2016.08.17
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_SPM         ); // 2016.08.17
					//
					KWIN_Item_Size_ReSetM_Run( TRUE );
				}
			}
			else if ( pchg == 1 ) { // 2017.03.10
				//
				wxsize = GET_WINDOW_POSITION_FOR_GUI_XS(3);
				wysize = GET_WINDOW_POSITION_FOR_GUI_YS(3);
				//
				if ( KWIN_Item_Size_ReSetM_Check( wxsize + layoutxsoffset, wysize + layoutysoffset, 0 , 0 , &wxp , &wyp ) ) {
					//
					KWIN_Item_Size_ReSetM_Init( hdlg , wxp , wyp , layoutxoffset , layoutyoffset , ( wxp == 1 ) ? 0 : -2 , ( wyp == 1 ) ? 0 : -2 );
					//
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA     );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_S   );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SS  );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_S2  );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_S2S );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SPS ); // 2014.02.13
					//
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_ADD         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_DELETE      );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_START       );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_ABORT       );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_END         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_CANCEL      );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_FIND        );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_GO_BACK     );
					KWIN_Item_Size_ReSetM_Add( IDC_TREE			    );
					//
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SPM );
					KWIN_Item_Size_ReSetM_Add( IDC_LIST_JOBDATA_SPMS );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_SPM         );
					//
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG1         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG2         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG3         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG4         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG5         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG6         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG7         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LOG8         );
					//
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_UP         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_SUP         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_SDOWN         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_DOWN         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_RIGHT         );
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_LEFT         );
					//
					KWIN_Item_Size_ReSetM_Add( IDC_CTRL_SAVE         );
					//
					KWIN_Item_Size_ReSetM_Run( TRUE );
					//
					//
					KWIN_Item_GetSize( hdlg , IDC_CTRL_SPM , &Global_Pos_X[BUTTON_SPM] , &Global_Pos_Y[BUTTON_SPM] , &Global_Pos_XS[BUTTON_SPM] , &Global_Pos_YS[BUTTON_SPM] );
					KWIN_Item_GetSize( hdlg , IDC_LIST_JOBDATA_S , &Global_Pos_X[BUTTON_DATA_S] , &Global_Pos_Y[BUTTON_DATA_S] , &Global_Pos_XS[BUTTON_DATA_S] , &Global_Pos_YS[BUTTON_DATA_S] );
					KWIN_Item_GetSize( hdlg , IDC_LIST_JOBDATA_SS, &Global_Pos_X[BUTTON_DATA_Sstr] , &Global_Pos_Y[BUTTON_DATA_Sstr] , &Global_Pos_XS[BUTTON_DATA_Sstr] , &Global_Pos_YS[BUTTON_DATA_Sstr] );
					KWIN_Item_GetSize( hdlg , IDC_LIST_JOBDATA_S2, &Global_Pos_X[BUTTON_DATA_S2] , &Global_Pos_Y[BUTTON_DATA_S2] , &Global_Pos_XS[BUTTON_DATA_S2] , &Global_Pos_YS[BUTTON_DATA_S2] );
					//
				}
			}
			//=============================================================
			//
			// 2016.08.17
			//
			IDC_CTRL_SPM_MODE = 0;
			KWIN_Item_Hide( hdlg , IDC_CTRL_SPM );
			KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_SPM );
			KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_SPMS );
			//
			//
			JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );

			uTimer  = SetTimer( hdlg , 1000 , 500 , NULL );		// 0.5 sec
			uTimer2 = SetTimer( hdlg , 1001 , 3600000 , NULL );	// 1   hour
			tuse = 0;
			//
			// 2011. 9. 19 ( Job List Status is Selected. )  mok
			//
			TreeView_Select( GetDlgItem( hdlg , IDC_TREE ), TreeView_GetRoot(GetDlgItem( hdlg , IDC_TREE )), TVGN_CARET );
			//
			hitm = TreeView_GetSelection( GetDlgItem( hdlg , IDC_TREE ) );
			TreeView_Expand( GetDlgItem( hdlg , IDC_TREE ), hitm, TVE_EXPAND );
			//
			return TRUE;
			
		case WM_MEASUREITEM:
			//
			//
			if ( ( GLISTVIEW_HEIGHT > 0 ) && ( ( GLISTVIEW_HEIGHT % 1000 ) >= 10 ) ) {
				if ( Get_Prm_DISPLAY_MODE() > 0 ) {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_JOBDATA ) {
							 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
						}
						else if ( dim->CtlID == IDC_LIST_JOBDATA_S ) {
							 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
						}
						else if ( dim->CtlID == IDC_LIST_JOBDATA_S2 ) {
							 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
						}
						//
						else if ( dim->CtlID == IDC_LIST_JOBDATA_SPMS ) {
							//
							switch( STEPVIEW_DATA.SV_Opt_Summary_CharSize ) {
							case 1 :
								 dim->itemHeight = 16;
								break;
							case 2 :
								 dim->itemHeight = 18;
								break;
							case 3 :
								 dim->itemHeight = 20;
								break;
							case 4 :
								 dim->itemHeight = 22;
								break;
							default :
								 dim->itemHeight = 14;
								break;
							}
							//
						}
						else if ( dim->CtlID == IDC_LIST_JOBDATA_SPM ) {
							//
							switch( STEPVIEW_DATA.SV_Opt_StepItem_CharSize ) {
							case 1 :
								 dim->itemHeight = 16;
								break;
							case 2 :
								 dim->itemHeight = 18;
								break;
							case 3 :
								 dim->itemHeight = 20;
								break;
							case 4 :
								 dim->itemHeight = 22;
								break;
							default :
								 dim->itemHeight = 14;
								break;
							}
							//
						}
						//
					}
					//
				}
				else {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_JOBDATA ) {
							 dim->itemHeight = GLISTVIEW_HEIGHT % 1000;
						}
					}
				}
				//
			}
			else {
				if ( Get_Prm_DISPLAY_MODE() > 0 ) {
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_JOBDATA ) {
	//						 dim->itemHeight = 21;
	//						 dim->itemHeight = 19; // 2015.07.23
							 dim->itemHeight = LISTVIEW_HEIGHT; // 2015.07.23
						}
						else if ( dim->CtlID == IDC_LIST_JOBDATA_S ) {
	//						 dim->itemHeight = 16;
	//						 dim->itemHeight = 19; // 2015.07.23
							 dim->itemHeight = LISTVIEW_HEIGHT; // 2015.07.23
						}
						else if ( dim->CtlID == IDC_LIST_JOBDATA_S2 ) {
	//						 dim->itemHeight = 16;
	//						 dim->itemHeight = 19; // 2015.07.23
							 dim->itemHeight = LISTVIEW_HEIGHT; // 2015.07.23
						}
						//
						else if ( dim->CtlID == IDC_LIST_JOBDATA_SPMS ) { // 2016.10.07
							//
							switch( STEPVIEW_DATA.SV_Opt_Summary_CharSize ) {
							case 1 :
								 dim->itemHeight = 16;
								break;
							case 2 :
								 dim->itemHeight = 18;
								break;
							case 3 :
								 dim->itemHeight = 20;
								break;
							case 4 :
								 dim->itemHeight = 22;
								break;
							default :
								 dim->itemHeight = 14;
								break;
							}
							//
						}
						else if ( dim->CtlID == IDC_LIST_JOBDATA_SPM ) { // 2016.10.07
							//
							switch( STEPVIEW_DATA.SV_Opt_StepItem_CharSize ) {
							case 1 :
								 dim->itemHeight = 16;
								break;
							case 2 :
								 dim->itemHeight = 18;
								break;
							case 3 :
								 dim->itemHeight = 20;
								break;
							case 4 :
								 dim->itemHeight = 22;
								break;
							default :
								 dim->itemHeight = 14;
								break;
							}
							//
						}
						//
					}
					//
				}
				else { // 2015.07.23
					//
					dim = (LPMEASUREITEMSTRUCT) lParam;
					//
					if ( dim->CtlType == ODT_LISTVIEW ) {
						if ( dim->CtlID == IDC_LIST_JOBDATA ) {
							 dim->itemHeight = LISTVIEW_HEIGHT;
						}
					}
				}
				//
			}
			return TRUE;

		case WM_NOTIFY:
			lpnmh = (LPNMHDR) lParam;
			if      ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_JOBDATA ) ) {
				if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
					JobData_ListView_Notify( lpnmh->hwndFrom , GetDlgItem( hdlg , IDC_TREE ) , lParam );
				}
				else {
					JobData_ListView_Notify( lpnmh->hwndFrom , NULL , lParam );
				}
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) ) {
				LogView_Open_Method_History_Notify( lpnmh->hwndFrom , lParam );
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) ) {
				LogView_Open_Method_History_Notify3( lpnmh->hwndFrom , lParam );
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_JOBDATA_SPM ) ) { // 2016.08.17
				LogView_Open_Method_History_NotifySPM( lpnmh->hwndFrom , lParam );
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_JOBDATA_SPMS ) ) { // 2016.08.17
				LogView_Open_Method_History_NotifySPMS( lpnmh->hwndFrom , lParam );
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_TREE ) ) {
				JobData_TreeView_Notify( lpnmh->hwndFrom , hdlg , lParam );
			}
			return TRUE;

		case WM_PAINT:
			//=============================================================================================
			// 2004.05.21
			//=============================================================================================
//			if ( !KGUI_STANDARD_Window_Signal_Mode() ) return TRUE; // 2005.02.28
			if ( !KGUI_STANDARD_Window_Signal_Mode() ) { // 2005.02.28
				BeginPaint( hdlg, &ps );
				EndPaint( hdlg , &ps );
				return TRUE;
			}
			//=============================================================================================
			BeginPaint( hdlg, &ps );
			/*
			// 2012.04.23
			if ( KGUI_Get_BACKGROUND() == NULL ) {
				KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
			}
			else {
				hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND" );
				if ( hBitmap ) {
					KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
					DeleteObject( hBitmap );
				}
				else {
					KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
				}
			}
			*/
			//
			if ( !KWIN_DRAW_USER_BACK_BITMAP( 0 , ps.hdc , 0 , 0 ) ) {
				//
				// 2012.04.23
				if ( KGUI_Get_BACKGROUND() == NULL ) {
					if ( pchg == 2 ) {
						KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
					}
					else {
						KWIN_DRAW_BACK_PAINT_RESET( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) , wxp , wyp );
					}
				}
				else {
					hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUNDF" );
					if ( hBitmap ) {
						KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
						DeleteObject( hBitmap );
					}
					else {
						hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUNDA" );
						if ( hBitmap ) {
							KWIN_DRAW_BITMAP_RESET( ps.hdc , 0 , 0 , wxp , wyp , hBitmap , SRCCOPY );
							DeleteObject( hBitmap );
						}
						else {
							hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND" );
							if ( hBitmap ) {
								if ( pchg == 2 ) {
									KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
								}
								else {
									KWIN_DRAW_BITMAP_RESET( ps.hdc , 0 , 0 , wxp , wyp , hBitmap , SRCCOPY );
								}
								DeleteObject( hBitmap );
							}
							else {
								if ( pchg == 2 ) {
									KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) );
								}
								else {
									KWIN_DRAW_BACK_PAINT_RESET( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) , wxp , wyp );
								}
							}
						}
					}
				}
			}
			//
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_LBUTTONDBLCLK :
			{
				POINT ptcursor;
				GetCursorPos( &ptcursor );
				ScreenToClient( hdlg , &ptcursor );
				if ( ( ptcursor.x < 280 ) || ( ptcursor.x > 650 ) || ( ptcursor.y < 5 ) || ( ptcursor.y > 40 ) ) return TRUE;
			}
		case WM_RBUTTONDOWN:
			if ( Get_Prm_NETMONITOR_MODE() )  return TRUE; // 2003.12.18
			if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2002.11.08
//			JOB_W_OPEN = TRUE; // 2003.09.16
//			GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_JOB_PARAMETER ) , hdlg , Console_Gui_For_Parameter_Proc , (LPARAM) NULL ); // 2002.06.17
			//===================================================================
			_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Parameter Popup Screen\n" );
			//===================================================================
//			GoModalDialogBoxParam( GETHINST( hdlg ) , MAKEINTRESOURCE( IDD_JOB_PARAMETER ) , hdlg , Console_Gui_For_Parameter_Proc , (LPARAM) NULL ); // 2004.01.19
//			GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_JOB_PARAMETER ) , hdlg , Console_Gui_For_Parameter_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//			GoModalDialogBoxParam( GETHINST( hdlg ) , MAKEINTRESOURCE( IDD_JOB_PARAMETER ) , hdlg , Console_Gui_For_Parameter_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10 // 2016.10.11
			if ( GoModalDialogBoxParam( GETHINST( hdlg ) , MAKEINTRESOURCE( IDD_JOB_PARAMETER ) , hdlg , Console_Gui_For_Parameter_Proc , (LPARAM) NULL ) ) { // 2004.01.19 // 2004.08.10 // 2016.10.11
				//
				STEP_VIEW_EDITING( hdlg );
				//
			}
			//===================================================================
			_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[CLOSE] Parameter Popup Screen\n" );
			//===================================================================
//			JOB_W_OPEN = FALSE; // 2003.09.16
			return TRUE;

		case WM_DRAWITEM:
			//
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
//			KWIN_DRAW_BUTTON_ITEM( GetModuleHandle( NULL ) , (LPDRAWITEMSTRUCT) lParam ); // 2017.02.22
			//
			if      ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_JOBDATA ) ) {
				//
				color_error = LogView_GetParam( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , dis->itemID );
				//
				color_Index = color_error / 10000;
				//
//				switch ( JOB_Get_Run_Extern_Status( color_Index ) )	{ // 2016.03.29
				switch ( JOB_Get_Run_Extern_Status( color_Index , &s ) )	{ // 2016.03.29
				case 1 : // run
					color_error  = 0;
					color_error2 = 3;
					break;
				case 2 : // abort
//				case 4 : // 2015.11.12 // 2016.04.15
					color_error  = 1;
					color_error2 = 1;
					break;
				case 3 : // invalid
					color_error  = 1;
					color_error2 = 3;
					break;
				case 4 : // 2016.04.15
					//
					switch( JOB_User_Get_Fail_Data( color_Index , -1 ) ) { // 2016.04.15
					case 1 : // hasfail
						color_error  = 1;
						color_error2 = 1;
						break;

					case 2 : // hasprepost
						color_error  = 0;
						color_error2 = 2;
						break;

					default : // Normal
						color_error2 = ( color_error / 10 ) % 10;
						break;
					}
					//
					break;
				default :
					color_error2 = ( color_error / 10 ) % 10;
					break;
				}
				//
//				ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , lParam , Global_Pos_XS[BUTTON_DATA] , color_error % 10 , 1 , "" , Temp_Recipe , 2559 , color_error2 , 0 , -1 , -1 , -1 , -1 ); // 2015.07.23
				ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , lParam , -1 , color_error % 10 , 1 , "" , Temp_Recipe , 2559 , color_error2 , 0 , -1 , -1 , -1 , -1 ); // 2015.07.23
				//
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) ) {
				//
				color_error = LogView_GetParam( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , dis->itemID );
				//
				if      ( ( color_error % 10000 ) >= 9000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User9000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , Global_Pos_XS[BUTTON_DATA_S] , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User9000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 8000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User8000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , Global_Pos_XS[BUTTON_DATA_S] , "" , Temp_Recipe , 2559 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User8000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , -1 , "" , Temp_Recipe , 2559 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 7000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User7000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , Global_Pos_XS[BUTTON_DATA_S] , "" , Temp_Recipe , 2559 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User7000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , -1 , "" , Temp_Recipe , 2559 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 6000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User6000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , Global_Pos_XS[BUTTON_DATA_S] , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User6000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 5000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User5000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , Global_Pos_XS[BUTTON_DATA_S] , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User5000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 4000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User4000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , Global_Pos_XS[BUTTON_DATA_S] , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User4000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
				}
				else {
//					ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , Global_Pos_XS[BUTTON_DATA_S] , color_error % 10 , 1 , "" , Temp_Recipe , 2559 , ( color_error / 10 ) % 10 , 0 , -1 , -1 , -1 , -1 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , lParam , -1 , color_error % 10 , 1 , "" , Temp_Recipe , 2559 , ( color_error / 10 ) % 10 , 0 , -1 , -1 , -1 , -1 ); // 2015.07.23
				}
				//
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) ) {
				//
				color_error = LogView_GetParam( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , dis->itemID );
				//
				if      ( ( color_error % 10000 ) >= 9000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User9000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , Global_Pos_XS[BUTTON_DATA_S2] , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User9000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 8000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User8000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , Global_Pos_XS[BUTTON_DATA_S2] , "" , Temp_Recipe , 2559 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User8000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , -1 , "" , Temp_Recipe , 2559 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 7000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User7000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , Global_Pos_XS[BUTTON_DATA_S2] , "" , Temp_Recipe , 2559 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User7000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , -1 , "" , Temp_Recipe , 2559 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 6000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User6000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , Global_Pos_XS[BUTTON_DATA_S2] , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User6000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error % 1000 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 5000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User5000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , Global_Pos_XS[BUTTON_DATA_S2] , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User5000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
				}
				else if ( ( color_error % 10000 ) >= 4000 ) {
//					ListView_Draw_User_with_WM_DRAWITEM_User4000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , Global_Pos_XS[BUTTON_DATA_S2] , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM_User4000( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , -1 , "" , Temp_Recipe , 2559 , color_error / 10000 , color_error % 1000 ); // 2015.07.23
				}
				else {
//					ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , Global_Pos_XS[BUTTON_DATA_S2] , color_error % 10 , 1 , "" , Temp_Recipe , 2559 , ( color_error / 10 ) % 10 , 0 , -1 , -1 , -1 , -1 ); // 2015.07.23
					ListView_Draw_User_with_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) , lParam , -1 , color_error % 10 , 1 , "" , Temp_Recipe , 2559 , ( color_error / 10 ) % 10 , 0 , -1 , -1 , -1 , -1 ); // 2015.07.23
				}
				//
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_JOBDATA_SPM ) ) { // 2016.08.17
				//
				ListView_Draw_User_with_WM_DRAWITEM_SPM( GetDlgItem( hdlg , IDC_LIST_JOBDATA_SPM ) , lParam , LogView_GetParam( GetDlgItem( hdlg , IDC_LIST_JOBDATA_SPM ) , dis->itemID ) , Temp_Recipe , 2559 );
				//
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_JOBDATA_SPMS ) ) { // 2016.08.17
				//
				ListView_Draw_User_with_WM_DRAWITEM_SPMS( GetDlgItem( hdlg , IDC_LIST_JOBDATA_SPMS ) , lParam , Temp_Recipe , 2559 );
				//
			}
			else { // 2017.02.22
				if ( !KWIN_DRAW_USER_BUTTON_BITMAP( 0 , (LPDRAWITEMSTRUCT) lParam ) ) {
					KWIN_DRAW_BUTTON_ITEM( GetModuleHandle( NULL ) , (LPDRAWITEMSTRUCT) lParam );
				}
			}
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
//				// crkim add : popup close after 6 item select popup display // 2003.09.16
//				case 2000 : // 2003.09.16
//					JOB_W_OPEN = FALSE; // 2003.09.16
//					return TRUE; // 2003.09.16

				case WM_PAINT : // 2016.10.13
					//
					STEPVIEW_REVIEW( hdlg , IDC_LIST_JOBDATA_SPM );
					//
					STEPVIEW_REVIEW( hdlg , IDC_LIST_JOBDATA_SPMS );
					//
					JOBDATA_S_DISPLAY_CONTROL( hdlg , -100 , 0 , 0 , NULL );
					//
					return TRUE;
					
				case IDC_CTRL_SPM : // 2016.08.17
					//

					if ( IDC_CTRL_SPM_MODE == 1 ) {
						JOBDATA_S_DISPLAY_CONTROL( hdlg , -99 , -1 , -1 , NULL );
					}
					else if ( IDC_CTRL_SPM_MODE == 2 ) {
						LogView_Control_View_Change( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , FALSE );
					}
					else if ( IDC_CTRL_SPM_MODE == 3 ) {
						LogView_Control_View_Change( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) , TRUE );
					}
					//
					return TRUE;
					
				case IDC_CTRL_SAVE : // 2012.06.26
					//==================================================================================================================================
					if      ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) ) ) {
						s = IDC_LIST_JOBDATA;
					}
					else if ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) ) ) {
						s = IDC_LIST_JOBDATA_S;
					}
					else if ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) ) ) {
						s = IDC_LIST_JOBDATA_S2;
					}
					else {
						return TRUE;
					}
					//==================================================================================================================================
					if ( !Log_Get_Save_File_Name( hdlg , MessageBuffer ) ) return TRUE;
					//==================================================================================================================================
					fpt = fopen( MessageBuffer , "w" );
					if ( fpt == NULL ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "[ERROR] File Save Error!" , MessageBuffer , MB_ICONHAND );
						return TRUE;
					}
					//==================================================================================================================================
					// 2015.05.27
					//
					i = 0;
					//
					memset( MessageBuffer , 0 , 256 );
					//
					lvColumn.mask = LVCF_WIDTH | LVCF_TEXT;
					lvColumn.pszText = MessageBuffer;
					lvColumn.cchTextMax = 255;
					lvColumn.cx = 0;
					//
					while ( TRUE ) {
						//
						if ( !ListView_GetColumn( GetDlgItem( hdlg , s ) , i , &lvColumn ) ) {
							if ( i != 0 ) fprintf( fpt , "\n" );
							break;
						}
						//
						if ( i == 0 ) {
							fprintf( fpt , "%s" , lvColumn.pszText );
						}
						else {
							fprintf( fpt , "\t%s" , lvColumn.pszText );
						}
						//
						i++;
						//
					}
					//==================================================================================================================================
					j = ListView_GetItemCount( GetDlgItem( hdlg , s ) );
					for ( i = 0 ; i < j ; i++ ) {
						Log_Get_String_Form( GetDlgItem( hdlg , s ) , i , 0 , 999 , fpt );
					}
					fclose( fpt );
					return TRUE;
					//============================================================================
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				case IDC_CTRL_UP :
				case IDC_CTRL_DOWN :
				case IDC_CTRL_SUP :
				case IDC_CTRL_SDOWN :
				case IDC_CTRL_RIGHT :
				case IDC_CTRL_LEFT :

					if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.15
						if      ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) ) ) {
							i = IDC_LIST_JOBDATA;
						}
						else if ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S ) ) ) {
							i = IDC_LIST_JOBDATA_S;
						}
						else if ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) ) ) {
							i = IDC_LIST_JOBDATA_S2;
						}
						else {
							return TRUE;
						}
					}
					else {
						i = IDC_LIST_JOBDATA;
					}
					//
					subid = ListView_GetCountPerPage( GetDlgItem( hdlg , i ) );
					ListView_GetItemRect( GetDlgItem( hdlg , i ) , 0 , &rect , LVIR_LABEL );
					//
					switch( wParam ) {
					case IDC_CTRL_UP	:	s = 0;	j = -(rect.bottom - rect.top);	break;
					case IDC_CTRL_DOWN	:	s = 0;	j = rect.bottom - rect.top;	break;
					case IDC_CTRL_SUP	:	s = 0;	j = -( ( rect.bottom - rect.top ) * subid );	break;
					case IDC_CTRL_SDOWN	:	s = 0;	j = ( ( rect.bottom - rect.top ) * subid );	break;
					case IDC_CTRL_RIGHT :	s = ( rect.right - rect.left ) * 4;	j = 0; break;
					case IDC_CTRL_LEFT :	s = ( rect.left - rect.right ) * 4;	j = 0; break;
					}
					//
					ListView_Scroll( GetDlgItem( hdlg , i ) , s , j );
					//
					return TRUE;
				
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				case IDC_CTRL_FIND :
//					if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//					JOB_W_OPEN = TRUE; // 2003.09.16
//					GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_FIND_DATA , IDD_E_FIND_DATA ) , hdlg , Console_Gui_For_Find_Data_Proc , (LPARAM) NULL ); // 2002.06.17
					//===================================================================
					_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Search Popup Screen\n" );
					//===================================================================
//					if ( GoModalDialogBoxParam( GETHINST( hdlg ) , KWIN_MAKEINTRESOURCE( IDD_FIND_DATA , IDD_E_FIND_DATA ) , hdlg , Console_Gui_For_Find_Data_Proc , (LPARAM) NULL ) ) { // 2004.01.19
//					if ( GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_FIND_DATA , IDD_E_FIND_DATA ) , hdlg , Console_Gui_For_Find_Data_Proc , (LPARAM) NULL ) ) { // 2004.01.19 // 2004.08.10
					if ( GoModalDialogBoxParam( GETHINST( hdlg ) , KWIN_MAKEINTRESOURCE( IDD_FIND_DATA , IDD_E_FIND_DATA ) , hdlg , Console_Gui_For_Find_Data_Proc , (LPARAM) NULL ) ) { // 2004.01.19 // 2004.08.10
						//
						if ( LOG_OPEN_TERMINATE() ) { // 2014.02.05
							//-------------------------------------
							EnterCriticalSection( &CR_SC ); // 2003.09.17
							//-------------------------------------
							//===================================================================
							_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Search Action & Go Search Screen\n" );
							//===================================================================
							//
							Set_Prm_FIND_MODE( TRUE );
							//
							JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							//
							KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_S ); // 2011.06.16
							//
							JOB_FIND_SEARCH_START( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							//
							JobData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) );
							//
							InvalidateRect( hdlg , NULL , FALSE );
							//
							//-------------------------------------
							LeaveCriticalSection( &CR_SC ); // 2003.09.17
							//-------------------------------------
							sprintf( MessageBuffer , "Find %d Item Matched" , JOB_FIND_Get_Current_Count() );
							//
							MODAL_WINDOW_MESSAGE_BOX( hdlg , MessageBuffer , "Find" , MB_ICONQUESTION );
							//
						}
					}
//					JOB_W_OPEN = FALSE; // 2003.09.16
					//===================================================================
					_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[CLOSE] Search Popup Screen\n" );
					//===================================================================
					return TRUE;

				case IDC_CTRL_GO_BACK :
					//if ( Get_Prm_FIND_MODE() ) { // 2011.09.05
					if ( Get_Prm_FIND_MODE() || ( Get_Prm_DISPLAY_MODE() >= 2 ) ) { // 2011.09.05
						//===================================================================
						_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Go Back Normal History Screen\n" );
						//===================================================================
						JOB_FIND_GO_BACK( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
						//
						Set_Prm_FIND_MODE( FALSE );
						//
						JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
						//
						JobData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) );
						//
						InvalidateRect( hdlg , NULL , FALSE );
						// 2011. 9. 21 by mok
						TreeView_Select( GetDlgItem( hdlg , IDC_TREE ), TreeView_GetRoot(GetDlgItem( hdlg , IDC_TREE )), TVGN_CARET );
						hitm = TreeView_GetSelection( GetDlgItem( hdlg , IDC_TREE ) );
						TreeView_Expand( GetDlgItem( hdlg , IDC_TREE ), hitm, TVE_EXPAND );
					}
					return TRUE;

				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				case IDC_CTRL_DELETE :
					//=============================================================
					if ( Get_Prm_DELETE_HIDE() ) return TRUE; // 2006.08.30
					//=============================================================
					if ( Get_Prm_NETMONITOR_MODE() ) { // 2003.12.18
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Delete Impossible(Monitoring Mode)" , "Delete Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Delete Impossible(Read Only Mode)" , "Delete Impossible" , MB_ICONHAND );
						return TRUE;
					}
					//
//					if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//					JOB_W_OPEN = TRUE; // 2003.09.16
					if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
						//-------------------------------------
						index = TreeView_SelectionCheck( GetDlgItem( hdlg , IDC_TREE ) , &mode , &subid );
						//-------------------------------------
						if ( mode == 1 ) {
							//-------------------------------------
							if ( index >= 0 ) {
								//-------------------------------------
								if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Delete Selected item ???" , "DELETE Check" , MB_ICONQUESTION | MB_YESNO ) ) {
									//
									if ( LOG_OPEN_TERMINATE() ) { // 2014.02.05
										//
										//-------------------------------------
										EnterCriticalSection( &CR_SC );
										//-------------------------------------
										Data_Read_Start( hdlg );
										//
										j = TreeData_DeleteData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) , index , MessageBuffer , &i );
										//
										if ( j > 0 ) {
											//
											if ( i > 0 ) {
												//===================================================================
												_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Delete Action & %d Item(s) Deleted, %d Item(s) Fail(%s...)\n" , j , i , MessageBuffer );
												//===================================================================
											}
											else {
												//===================================================================
												_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Delete Action & %d Item(s) Deleted(%s...)\n" , j , MessageBuffer );
												//===================================================================
											}
											InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
											InvalidateRect( GetDlgItem( hdlg , IDC_TREE ) , NULL , FALSE );
											//
											Kor_JOB_Save( JOB_DATA_FILENAME );
											//
											JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
											//-------------------------------------
											KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_S );
											KWIN_Item_Show( hdlg , IDC_LIST_JOBDATA );
											//
											KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_SS ); // 2016.11.04
											KWIN_Item_Hide( hdlg , IDC_CTRL_SPM );
											IDC_CTRL_SPM_MODE = 0;
											//
											//-------------------------------------
										}
										//
										Data_Read_End( hdlg );
										//
										LeaveCriticalSection( &CR_SC );
										//-------------------------------------
										if ( i > 0 ) { // 2010.11.16
											sprintf( MessageBuffer , "Delete Operation %d Item(s) Success , %d Item(s) Fail!" , j , i );
											MODAL_WINDOW_MESSAGE_BOX( hdlg , MessageBuffer , "ERROR" , MB_ICONHAND );
										}
									}
								}
							}
						}
						else {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Delete Data Invalid!" , "ERROR" , MB_ICONHAND );
							return TRUE;
						}
					}
					else {
						//-------------------------------------
						EnterCriticalSection( &CR_SC );
						//-------------------------------------
						if ( JobData_ListView_DeleteEnable( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) ) ) {
							//-------------------------------------
							LeaveCriticalSection( &CR_SC );
							//-------------------------------------
							if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Delete Selected item ???" , "DELETE Check" , MB_ICONQUESTION | MB_YESNO ) ) {
								//
								//-------------------------------------
								EnterCriticalSection( &CR_SC );
								//-------------------------------------
								Data_Read_Start( hdlg );
								//
								j = JobData_ListView_DeleteData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) , MessageBuffer , &i );
								//
								if ( j > 0 ) {
									//
									if ( i > 0 ) {
										//===================================================================
										_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Delete Action & %d Item(s) Deleted, %d Item(s) Fail(%s...)\n" , j , i , MessageBuffer );
										//===================================================================
									}
									else {
										//===================================================================
										_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Delete Action & %d Item(s) Deleted(%s...)\n" , j , MessageBuffer );
										//===================================================================
									}
									InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
									//
									Kor_JOB_Save( JOB_DATA_FILENAME );
									//
									JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
									//-------------------------------------
									if ( Get_Prm_DISPLAY_MODE() != 0 ) { // 2011.06.20
										KWIN_Item_Hide( hdlg , IDC_LIST_JOBDATA_S );
									}
									//-------------------------------------
								}
								//
								Data_Read_End( hdlg );
								//
								LeaveCriticalSection( &CR_SC );
								//-------------------------------------
								if ( i > 0 ) { // 2010.11.16
									sprintf( MessageBuffer , "Delete Operation %d Item(s) Success , %d Item(s) Fail!" , j , i );
									MODAL_WINDOW_MESSAGE_BOX( hdlg , MessageBuffer , "ERROR" , MB_ICONHAND );
								}
							}
						}
						else {
							//-------------------------------------
							LeaveCriticalSection( &CR_SC );
							//-------------------------------------
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Delete Operation can not run!" , "ERROR" , MB_ICONHAND );
						}
					}
//					JOB_W_OPEN = FALSE; // 2003.09.16
					return TRUE;

				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				case IDC_CTRL_LOG :
					//=============================================================
					if ( LOG_AUTODEL_OPEN ) { // 2007.05.16
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Auto Delete is Running(Wait and Try Again)!" , "ERROR" , MB_ICONHAND );
						break;
					}
					//=============================================================
					LOG_WINDOW_OPEN = TRUE; // 2007.05.16
					//=============================================================
//					if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//					JOB_W_OPEN = TRUE; // 2003.09.16
					//===================================================================
//					_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Log View Popup Screen\n" ); // 2007.05.16
					//===================================================================
					if ( LOG_AUTODEL_OPEN ) { // 2007.05.16
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Auto Delete is Running(Wait and Try Again)!" , "ERROR" , MB_ICONHAND );
						//=============================================================
						LOG_WINDOW_OPEN = FALSE;
						//=============================================================
						break;
					}
					//=============================================================
					if ( !JobData_ListView_SelectMode( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , TRUE ) ) {
						//===================================================================
						_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Log View Popup Screen (All)\n" ); // 2007.05.16
						//===================================================================
						sprintf( MessageBuffer , "Total %s Log|Total Error Message" , Get_Prm_METHOD_STRING() );
						j = -1;
						if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Log Part" , MessageBuffer , &j ) ) {
							if ( j == 0 ) {
								LogView_Open_Full_Method( hdlg );
							}
							else if ( j == 1 ) {
								LogView_Open_Full_Error_Message( hdlg );
							}
						}
					}
					else {
						if ( Get_Prm_FIND_MODE() ) {
//							i = FindData_Conv_ListView_to_JobIndex( JobData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ) ); // 2009.07.20
							i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
							//===================================================================
							_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Log View Popup Screen (F:%d/%d)\n" , i , JOB_FIND_Get_Current_Count() ); // 2007.05.16
							//===================================================================
						}
						else {
//							i = JobData_Conv_ListView_to_JobIndex( JobData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ) ); // 2009.07.20
							i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
							//===================================================================
							_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Log View Popup Screen (%d/%d)\n" , i , JOB_Get_Current_Count() ); // 2007.05.16
							//===================================================================
						}
						//
						// 2017.03.13
						if ( !LOG_WINDOW_OPEN ) return TRUE;
						//
//						if ( JOB_Get_Run_Status( i ) != JOB_READY ) {	// 2003.09.16
//							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Error(Running)!" , "ERROR" , MB_ICONHAND );	// 2003.09.16
//						}	// 2003.09.16
//						else	// 2003.09.16
						if ( JOB_Get_Directory_Type( i ) != 1 ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Error(Log Data is not exist)!" , "ERROR" , MB_ICONHAND );
						}
						else {
							//=============================================================
							j = -1;
							//--------------------------------------------------
							JOB_Display_Data_Save_per_Each( FALSE , i ); // 2003.06.09
							//--------------------------------------------------
							//
							JOB_Get_Display_String( i , MessageBuffer2 );
							//
							if ( Get_Prm_NOTANAL_MODE() ) {
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) { // 2005.02.15
									sprintf( MessageBuffer , "Lot Flow History|%s Run History|Process Data per %s|Process Data per %s[ALL]|Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									lpcnt_selindex = 5; // 2012.04.13
								}
								else {
									sprintf( MessageBuffer , "Lot Flow History|%s Run History|Process Data per %s|Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									lpcnt_selindex = 4; // 2012.04.13
								}
								//=========================================================================================
								// 2006.02.23
								//=========================================================================================
								if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
									sprintf( MessageBuffer3 , "|[PRE]-Process Data per %s|[PRE]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									strcat( MessageBuffer , MessageBuffer3 );
									lpcnt_selindex += 2; // 2012.04.13
								}
								if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
									sprintf( MessageBuffer3 , "|[POST]-Process Data per %s|[POST]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									strcat( MessageBuffer , MessageBuffer3 );
									lpcnt_selindex += 2; // 2012.04.13
								}
								if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25 Lot Pre/Post
									sprintf( MessageBuffer3 , "|[LOTPRE]-Process Data|[LOTPRE]-Recipe Data per|[LOTPOST]-Process Data|[LOTPOST]-Recipe Data per" );
									strcat( MessageBuffer , MessageBuffer3 );
									lpcnt_selindex += 4; // 2012.04.13
								}
								//=========================================================================================
							}
							else {
								if      ( Get_Prm_EXTENDVIEW_MODE() == 1 ) { // 2005.02.15
									sprintf( MessageBuffer , "Lot Flow History|%s Run History|Process Data per Lot|Process Data per Lot[PM]|Process Data per Lot[ALL]|Process Data per Lot[PM/ALL]|Process Data per %s|Process Data per %s[ALL]|Process Data per Module|Process Data per Module[ALL]|Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									lpcnt_selindex = 11; // 2012.04.13
									//=========================================================================================
									// 2006.02.23
									//=========================================================================================
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										sprintf( MessageBuffer3 , "|[PRE]-Process Data per Lot|[PRE]-Process Data per Lot[PM]|[PRE]-Process Data per %s|[PRE]-Process Data per Module|[PRE]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 5; // 2012.04.13
									}
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
										sprintf( MessageBuffer3 , "|[POST]-Process Data per Lot|[POST]-Process Data per Lot[PM]|[POST]-Process Data per %s|[POST]-Process Data per Module|[POST]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 5; // 2012.04.13
									}
									if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25 Lot Pre/Post
										sprintf( MessageBuffer3 , "|[LOTPRE]-Process Data|[LOTPRE]-Recipe Data per|[LOTPOST]-Process Data|[LOTPOST]-Recipe Data per" );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 4; // 2012.04.13
									}
									//=========================================================================================
								}
								else if ( Get_Prm_EXTENDVIEW_MODE() == 2 ) { // 2005.02.15
									sprintf( MessageBuffer , "Lot Flow History|%s Run History|Process Data per Lot|Process Data per Lot[PM]|Process Data per %s|Process Data per Module|Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									lpcnt_selindex = 7; // 2012.04.13
									//=========================================================================================
									// 2006.02.23
									//=========================================================================================
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										sprintf( MessageBuffer3 , "|[PRE]-Process Data per Lot|[PRE]-Process Data per Lot[PM]|[PRE]-Process Data per %s|[PRE]-Process Data per Module|[PRE]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 5; // 2012.04.13
									}
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
										sprintf( MessageBuffer3 , "|[POST]-Process Data per Lot|[POST]-Process Data per Lot[PM]|[POST]-Process Data per %s|[POST]-Process Data per Module|[POST]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 5; // 2012.04.13
									}
									if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25 Lot Pre/Post
										sprintf( MessageBuffer3 , "|[LOTPRE]-Process Data|[LOTPRE]-Recipe Data per|[LOTPOST]-Process Data|[LOTPOST]-Recipe Data per" );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 4; // 2012.04.13
									}
									//=========================================================================================
								}
								else {
									sprintf( MessageBuffer , "Lot Flow History|%s Run History|Process Data per Lot|Process Data per %s|Process Data per Module|Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
									lpcnt_selindex = 6; // 2012.04.13
									//=========================================================================================
									// 2006.02.23
									//=========================================================================================
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										sprintf( MessageBuffer3 , "|[PRE]-Process Data per Lot|[PRE]-Process Data per %s|[PRE]-Process Data per Module|[PRE]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 4; // 2012.04.13
									}
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
										sprintf( MessageBuffer3 , "|[POST]-Process Data per Lot|[POST]-Process Data per %s|[POST]-Process Data per Module|[POST]-Recipe Data per %s" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 4; // 2012.04.13
									}
									if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25 Lot Pre/Post
										sprintf( MessageBuffer3 , "|[LOTPRE]-Process Data|[LOTPRE]-Recipe Data per|[LOTPOST]-Process Data|[LOTPOST]-Recipe Data per" );
										strcat( MessageBuffer , MessageBuffer3 );
										lpcnt_selindex += 4; // 2012.04.13
									}
									//=========================================================================================
								}
							}
							//
							while ( TRUE ) {
								//
								lpcnt_max = JOB_Get_LoopCount( i , &lpcnt_curr ); // 2012.04.13
								//
								strcpy( MessageBufferM , MessageBuffer );
								//
								if ( lpcnt_max > 1 ) {
									sprintf( MessageBuffer3 , "|* Change Loop Index(Current=%d,Max=%d)" , lpcnt_curr+1 , lpcnt_max );
									strcat( MessageBufferM , MessageBuffer3 );
								}
								//
								if ( Get_Prm_NOTANAL_MODE() ) {
									if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) { // 2005.02.15
										if ( MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBufferM , &j ) ) {
											//
											// 2017.03.13
											if ( !LOG_WINDOW_OPEN ) return TRUE;
											//
											if ( j == lpcnt_selindex ) { // 2012.04.13
												if ( !LogView_Change_Loop_Sel_Count( hdlg , i , lpcnt_max , lpcnt_curr ) ) break;
											}
											//
											if      ( j == 0 ) {
												if ( !LogView_Open_Lot_History( hdlg , i ) ) break;
											}
											else if ( j == 1 ) {
												if ( !LogView_Open_Method_History( hdlg , i ) ) break;
											}
											else if ( j == 2 ) {
												if ( !LogView_Open_Process_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 3 ) {
												if ( !LogView_Open_Process_Log( hdlg , i , 1 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 4 ) {
												if ( !LogView_Open_Recipe_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											//=========================================================================================
											// 2006.02.23
											//=========================================================================================
											if      ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) {
												if      ( j == 5 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 6 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) {
												if      ( j == 5 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 6 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) {
												if      ( j == 5 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 6 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 7 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 8 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25
												if      ( j == 9 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 10 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 11 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 12 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											//=========================================================================================
										}
										else {
											break;
										}
									}
									else {
										if ( MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBufferM , &j ) ) {
											//
											// 2017.03.13
											if ( !LOG_WINDOW_OPEN ) return TRUE;
											//
											if ( j == lpcnt_selindex ) { // 2012.04.13
												if ( !LogView_Change_Loop_Sel_Count( hdlg , i , lpcnt_max , lpcnt_curr ) ) break;
											}
											//
											if      ( j == 0 ) {
												if ( !LogView_Open_Lot_History( hdlg , i ) ) break; // 2003.09.16
											}
											else if ( j == 1 ) {
												if ( !LogView_Open_Method_History( hdlg , i ) ) break; // 2003.09.16
											}
											else if ( j == 2 ) {
												if ( !LogView_Open_Process_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
											}
											else if ( j == 3 ) {
												if ( !LogView_Open_Recipe_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
											}
											//=========================================================================================
											// 2006.02.23
											//=========================================================================================
											if      ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) {
												if      ( j == 4 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 5 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) {
												if      ( j == 4 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 5 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) {
												if      ( j == 4 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 5 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 6 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 7 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25
												if      ( j == 8 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 9 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 10 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 11 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											//=========================================================================================
										}
										else {
											break;
										}
									}
								}
								else {
									if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) { // 2005.02.15
										if ( MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBufferM , &j ) ) {
											//
											// 2017.03.13
											if ( !LOG_WINDOW_OPEN ) return TRUE;
											//
											if ( j == lpcnt_selindex ) { // 2012.04.13
												if ( !LogView_Change_Loop_Sel_Count( hdlg , i , lpcnt_max , lpcnt_curr ) ) break;
											}
											//
											if      ( j == 0 ) {
												if ( !LogView_Open_Lot_History( hdlg , i ) ) break;
											}
											else if ( j == 1 ) {
												if ( !LogView_Open_Method_History( hdlg , i ) ) break;
											}
											else if ( j == 2 ) {
												if ( !LogView_Open_Lot_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 3 ) {
												if ( !LogView_Open_LotPM_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 4 ) {
												if ( !LogView_Open_Lot_Log( hdlg , i , 1 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 5 ) {
												if ( !LogView_Open_LotPM_Log( hdlg , i , 1 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 6 ) {
												if ( !LogView_Open_Process_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 7 ) {
												if ( !LogView_Open_Process_Log( hdlg , i , 1 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 8 ) {
												if ( !LogView_Open_Module_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 9 ) {
												if ( !LogView_Open_Module_Log( hdlg , i , 1 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 10 ) {
												if ( !LogView_Open_Recipe_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											//=========================================================================================
											// 2006.02.23
											//=========================================================================================
											if      ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) {
												if      ( j == 11 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 12 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 13 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 14 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 15 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) {
												if      ( j == 11 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 12 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 13 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 14 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 15 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) {
												if      ( j == 11 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 12 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 13 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 14 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 15 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 16 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 17 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 18 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 19 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 20 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25
												if      ( j == 21 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 22 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 23 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 24 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											//=========================================================================================
										}
										else {
											break;
										}
									}
									else if ( Get_Prm_EXTENDVIEW_MODE() == 2 ) { // 2005.02.15
										if ( MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBufferM , &j ) ) {
											//
											// 2017.03.13
											if ( !LOG_WINDOW_OPEN ) return TRUE;
											//
											if ( j == lpcnt_selindex ) { // 2012.04.13
												if ( !LogView_Change_Loop_Sel_Count( hdlg , i , lpcnt_max , lpcnt_curr ) ) break;
											}
											//
											if      ( j == 0 ) {
												if ( !LogView_Open_Lot_History( hdlg , i ) ) break;
											}
											else if ( j == 1 ) {
												if ( !LogView_Open_Method_History( hdlg , i ) ) break;
											}
											else if ( j == 2 ) {
												if ( !LogView_Open_Lot_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 3 ) {
												if ( !LogView_Open_LotPM_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 4 ) {
												if ( !LogView_Open_Process_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 5 ) {
												if ( !LogView_Open_Module_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											else if ( j == 6 ) {
												if ( !LogView_Open_Recipe_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break;
											}
											//=========================================================================================
											// 2006.02.23
											//=========================================================================================
											if      ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) {
												if      ( j == 7 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 8 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 9 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 10 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 11 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) {
												if      ( j == 7 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 8 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 9 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 10 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 11 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) {
												if      ( j == 7 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 8 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 9 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 10 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 11 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 12 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 13 ) {
													if ( !LogView_Open_LotPM_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 14 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 15 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 16 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25
												if      ( j == 17 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 18 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 19 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 20 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											//=========================================================================================
										}
										else {
											break;
										}
									}
									else {
										if ( MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBufferM , &j ) ) {
											//
											// 2017.03.13
											if ( !LOG_WINDOW_OPEN ) return TRUE;
											//
											if ( j == lpcnt_selindex ) { // 2012.04.13
												if ( !LogView_Change_Loop_Sel_Count( hdlg , i , lpcnt_max , lpcnt_curr ) ) break;
											}
											//
											if      ( j == 0 ) {
												if ( !LogView_Open_Lot_History( hdlg , i ) ) break; // 2003.09.16
											}
											else if ( j == 1 ) {
												if ( !LogView_Open_Method_History( hdlg , i ) ) break; // 2003.09.16
											}
											else if ( j == 2 ) {
												if ( !LogView_Open_Lot_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
											}
											else if ( j == 3 ) {
												if ( !LogView_Open_Process_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
											}
											else if ( j == 4 ) {
												if ( !LogView_Open_Module_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
											}
											else if ( j == 5 ) {
												if ( !LogView_Open_Recipe_Log( hdlg , i , 0 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
											}
											//=========================================================================================
											// 2006.02.23
											//=========================================================================================
											if      ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) {
												if      ( j == 6 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 7 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 8 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
												else if ( j == 9 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) {
												if      ( j == 6 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 7 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 8 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
												else if ( j == 9 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
											}
											else if ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) {
												if      ( j == 6 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 7 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 8 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
												else if ( j == 9 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 2 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
												else if ( j == 10 ) {
													if ( !LogView_Open_Lot_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 11 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 12 ) {
													if ( !LogView_Open_Module_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
												else if ( j == 13 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 3 /*Main,All,Pre,Post*/ ) ) break; // 2003.09.16
												}
											}
											if ( Get_Info_PRE_POST_SHOW_MODE() == 4 ) { // 2007.06.25
												if      ( j == 14 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 15 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 4 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 16 ) {
													if ( !LogView_Open_Process_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
												else if ( j == 17 ) {
													if ( !LogView_Open_Recipe_Log( hdlg , i , 5 /*Main,All,Pre,Post*/ ) ) break;
												}
											}
											//=========================================================================================
										}
										else {
											break;
										}
									}
								}
							}
						}
					}
					//=============================================================
					LOG_WINDOW_OPEN = FALSE; // 2007.05.16
					//=============================================================
					//=============================================================
//					JOB_W_OPEN = FALSE; // 2003.09.16
					//===================================================================
					_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[CLOSE] Log View Popup Screen\n" );
					//===================================================================
					return TRUE;
			
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				case IDC_CTRL_LOG1 :
				case IDC_CTRL_LOG2 :
				case IDC_CTRL_LOG3 :
				case IDC_CTRL_LOG4 :
				case IDC_CTRL_LOG5 :
				case IDC_CTRL_LOG6 :
				case IDC_CTRL_LOG7 :
				case IDC_CTRL_LOG8 :
					//=============================================================
					if ( LOG_AUTODEL_OPEN ) { // 2007.05.16
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Auto Delete is Running(Wait and Try Again)!" , "ERROR" , MB_ICONHAND );
						return TRUE;
					}
					//=============================================================
					LOG_WINDOW_OPEN = TRUE; // 2007.05.16
					//=============================================================
					if ( LOG_AUTODEL_OPEN ) { // 2007.05.16
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Auto Delete is Running(Wait and Try Again)!" , "ERROR" , MB_ICONHAND );
						//=============================================================
						return TRUE;
					}
					//=============================================================
					i = 0;
					selfrom = 0;
					//
					flowhistory_multi = FALSE;
					//
					if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.14
						//
						//-------------------------------------
						index = TreeView_SelectionCheck( GetDlgItem( hdlg , IDC_TREE ) , &mode , &subid );
						//-------------------------------------
						if      ( mode == 0 ) { // view All
							selfrom = 1;
						}
						else if ( mode == 1 ) { // Select Job
							selfrom = 0;
							i = index;
						}
						else if ( mode == 2 ) { // Alarm
							selfrom = 0;
							i = index;
						}
						else if ( mode == 3 ) { // Select Slot
							selfrom = 0;
							i = index;
						}
//						else if ( mode == 4 ) { // Select PM // 2016.08.17
						else if ( ( mode == 4 ) || ( mode == 7 ) ) { // Select PM // 2016.08.17
							selfrom = 0;
							i = index;
						}
						else if ( mode == 5 ) { // Select Slot Alarm
							selfrom = 0;
							i = index;
						}
						else if ( mode == 6 ) { // Select User
							selfrom = 0;
							i = index;
						}
						else {
							mode = 0;
							selfrom = 1;
						}
					}
					else {
						mode = 0;
						selfrom = 1;
					}
					//
					if ( selfrom == 1 ) {
						if ( !JobData_ListView_SelectMode( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , TRUE ) ) {
							//
							if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
								//
								if ( Get_CTR_Button_Data( wParam ) == CTR_FLOW_HISTORY ) {
									//
									i = 0;
									//
									JobData_ListView_Get_Multi_Index( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , FLOW_HISTORY_M_INDEX , &i , MAX_MULTI_CHECK_JOB );
									//
									if ( i >= 1 ) flowhistory_multi = TRUE;
									//
								}
							}
							//
							if ( !flowhistory_multi ) {
								//
								MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Data Invalid! (Please Select one item)" , "ERROR" , MB_ICONHAND );
								//=============================================================
								LOG_WINDOW_OPEN = FALSE;
								//=============================================================
								//
								return TRUE;
							}
							//
						}
						//
						if ( !flowhistory_multi ) {
							//
							if ( Get_Prm_FIND_MODE() ) {
								i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
								//===================================================================
								_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Log View Popup Screen (F:%d/%d)\n" , i , JOB_FIND_Get_Current_Count() ); // 2007.05.16
								//===================================================================
							}
							else {
								i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
								//===================================================================
								_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Log View Popup Screen (%d/%d)\n" , i , JOB_Get_Current_Count() ); // 2007.05.16
								//===================================================================
							}
							//
						}
						//
					}
					//
					if ( !flowhistory_multi ) {
						//
						if ( JOB_Get_Directory_Type( i ) != 1 ) {
							//
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Log View Error(Log Data is not exist)!" , "ERROR" , MB_ICONHAND );
							//=============================================================
							LOG_WINDOW_OPEN = FALSE;
							//=============================================================
							return TRUE;
						}
						//=============================================================
						//
					}
					//--------------------------------------------------
					JOB_Display_Data_Save_per_Each( FALSE , i );
					//--------------------------------------------------
					JOB_Get_Display_String( i , MessageBuffer2 );
					//=============================================================
					switch( Get_CTR_Button_Data( wParam ) ) {
					case CTR_FLOW_HISTORY :
						//
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.14
							//
							if ( !flowhistory_multi ) {
								//
								JOB_Get_Directory( i , folderstr );
								//----------------------------------------------------------------------------------------------
								sprintf( MessageBuffer , "%s/LotLog/%s/LotData.log" , Get_Prm_LOG_DIRECTORY() , folderstr );
								//----------------------------------------------------------------------------------------------
								if ( LogView_Open_Lot_History_Data_Set( hdlg , IDC_LIST_JOBDATA_S2 , 0 , subid , MessageBuffer , MessageBuffer3 , i , TRUE ) ) {
									//
									sprintf( MessageBuffer3 , "* LOT FLOW HISTORY : %s" , MessageBuffer2 );
									JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer3 , MessageBuffer , "" );
									//
								}
								//
							}
							else {
								//
								if ( LogView_Open_Lot_History_Data_Set( hdlg , IDC_LIST_JOBDATA_S , 9999 , 0 , MessageBuffer , MessageBuffer3 , i , TRUE ) ) {
									//
									JOBDATA_S_DISPLAY_CONTROL( hdlg , -999 , i , 1 , FLOW_HISTORY_M_INDEX ); // Wafer History
									//
								}
							}
						}
						else {
							LogView_Open_Lot_History( hdlg , i );
						}
						break;

					case CTR_PROCESS_DATA_1_WAFER :
						//
						s = 0;
						//=========================================================================================
						if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
							s = 1;
							sprintf( MessageBuffer , "Process Data per %s|Process Data per %s[ALL]" , Get_Prm_METHOD_STRING() , Get_Prm_METHOD_STRING() );
						}
						else {
							sprintf( MessageBuffer , "Process Data per %s" , Get_Prm_METHOD_STRING() );
						}
						//=========================================================================================
						if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
							s = 1;
							sprintf( MessageBuffer3 , "|[PRE]-Process Data per %s" , Get_Prm_METHOD_STRING() );
							strcat( MessageBuffer , MessageBuffer3 );
						}
						if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
							s = 1;
							sprintf( MessageBuffer3 , "|[POST]-Process Data per %s" , Get_Prm_METHOD_STRING() );
							strcat( MessageBuffer , MessageBuffer3 );
						}
						//=========================================================================================
						if ( s == 1 ) {
							//
							j = -1;
							if ( !MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBuffer , &j ) ) {
								//=============================================================
								LOG_WINDOW_OPEN = FALSE;
								//=============================================================
								return TRUE;
								//=============================================================
							}
							//
							// 2017.03.13
							if ( !LOG_WINDOW_OPEN ) return TRUE;
							//
							switch( j ) {
							case 0 :
								s = 0;
								break;
								//
							case 1 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									s = 1;
								}
								else {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								break;
							case 2 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								else {
									s = 3;
								}
								break;
							case 3 :
								s = 3;
								break;
							}
							//=========================================================================================
						}
						//
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.14
							if      ( mode == 0 ) { // view All
								j = -1;
							}
							else if ( mode == 1 ) { // Select Job
								j = -1;
							}
							else if ( mode == 2 ) { // Alarm
								j = -1;
							}
							else if ( mode == 3 ) { // Select Slot
								j = subid;
							}
							else if ( mode == 5 ) { // Select Alarm Slot
								j = subid;
							}
//							else if ( mode != 4 ) { // ?? // 2016.08.17
							else if ( ( mode != 4 ) && ( mode != 7 ) ) { // ?? // 2016.08.17
								j = -1;
							}
							//
//							if ( mode == 4 ) { // Select PM // 2016.08.17
							if ( ( mode == 4 ) || ( mode == 7 ) ) { // Select PM // 2016.08.17
								//
								if ( TreeData_Get_Select_Slot_PM_Info( i , subid , Temp_SlotStr , Temp_Recipe , pmorg ) ) {
									//
									//----------------------------------------------------------------------------------------------
//									Get_Log_File_Name( MessageBuffer3 , "PROCESS" , 0 , JOB_Get_Directory( index ) , TRUE , 0 , 0 , Temp_SlotStr , 0 , Temp_Recipe , "" ); // 2017.03.13

									JOB_Get_Directory( i , folderstr );

									Get_Log_File_Name( MessageBuffer3 , "PROCESS" , 0 , folderstr , TRUE , 0 , 0 , Temp_SlotStr , 0 , pmorg , "" ); // 2017.03.13
									//----------------------------------------------------------------------------------------------
									if ( LogView_Open_Process_Log_Data_Set( hdlg , IDC_LIST_JOBDATA_S2 , MessageBuffer3 , MessageBuffer , FALSE , 0 , 0 , 0 , TRUE , FALSE ) ) {
										//
										sprintf( MessageBuffer , "* PROCESS DATA : %s %s : %s" , Temp_Recipe , Temp_SlotStr , MessageBuffer2 );
										//
										JOBDATA_S2_DISPLAY_CONTROL( hdlg , wParam , selfrom , MessageBuffer , MessageBuffer3 , Temp_Recipe );
										//
									}
									//----------------------------------------------------------------------------------------------
								}
							}
							else {
								//
//								if ( LogView_Open_Process_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , j , MessageBuffer , MessageBuffer3 ) ) { // 2016.12.22
								if ( LogView_Open_Process_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , j , MessageBuffer , MessageBuffer3 , Temp_Recipe ) ) { // 2016.12.22
									//
//									JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , MessageBuffer3 ); // 2016.12.22
									JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , MessageBuffer3 , Temp_Recipe ); // 2016.12.22
									//
								}
							}
							//
						}
						else {
							LogView_Open_Process_Log( hdlg , i , s /*Main,All,Pre,Post*/ );
						}
						//=========================================================================================
						break;

					case CTR_PROCESS_PLOT_1_WAFER :
						//
						strcpy( ModuleStr , "" ); // 2016.12.17
						//
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
							//
							j = 1;
							//
							if ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) ) ) {
								if ( IDC_CONTROL_S2_AREA_LAST_OPEN == CTR_PROCESS_DATA_1_WAFER ) {
									if ( IDC_CONTROL_S2_AREA_LAST_DATA[0] != 0 ) {
										//
										strcpy( MessageBuffer3 , IDC_CONTROL_S2_AREA_LAST_DATA );
										strcpy( ModuleStr , IDC_CONTROL_S2_AREA_LAST_MODULE ); // 2016.12.22
										j = 2;
										//
									}
									else {
										j = 0;
									}
								}
							}
							//
							if ( j == 1 ) {
								if      ( mode == 1 ) { // Select Job
									if ( !LogView_Open_Process_Log_Get_FileName( hdlg , i , -1 , MessageBuffer3 , ModuleStr ) ) {
										j = -1;
									}
								}
								else if ( mode == 2 ) { // Alarm
									if ( !LogView_Open_Process_Log_Get_FileName( hdlg , i , -1 , MessageBuffer3 , ModuleStr ) ) {
										j = -1;
									}
								}
								else if ( mode == 3 ) { // Select Slot
									if ( !LogView_Open_Process_Log_Get_FileName( hdlg , i , subid , MessageBuffer3 , ModuleStr ) ) {
										j = -1;
									}
								}
//								else if ( mode == 4 ) { // Select PM // 2016.08.17
								else if ( ( mode == 4 ) || ( mode == 7 ) ) { // Select PM // 2016.08.17
									//
//									if ( TreeData_Get_Select_Slot_PM_Info( i , subid , Temp_SlotStr , Temp_Recipe ) ) { // 2016.12.17
									if ( TreeData_Get_Select_Slot_PM_Info( i , subid , Temp_SlotStr , ModuleStr , pmorg ) ) { // 2016.12.17
										//----------------------------------------------------------------------------------------------
//										Get_Log_File_Name( MessageBuffer3 , "PROCESS" , 0 , JOB_Get_Directory( index ) , TRUE , 0 , 0 , Temp_SlotStr , 0 , Temp_Recipe , "" ); // 2016.12.17
//										Get_Log_File_Name( MessageBuffer3 , "PROCESS" , 0 , JOB_Get_Directory( index ) , TRUE , 0 , 0 , Temp_SlotStr , 0 , ModuleStr , "" ); // 2016.12.17 // 2017.03.13

										JOB_Get_Directory( i , folderstr );

										Get_Log_File_Name( MessageBuffer3 , "PROCESS" , 0 , folderstr , TRUE , 0 , 0 , Temp_SlotStr , 0 , pmorg , "" ); // 2016.12.17 // 2017.03.13
										//----------------------------------------------------------------------------------------------
									}
									else {
										j = 0;
									}
								}
								else if ( mode == 5 ) { // Select Alarm Slot
									if ( !LogView_Open_Process_Log_Get_FileName( hdlg , i , subid , MessageBuffer3 , ModuleStr ) ) {
										j = -1;
									}
								}
								else if ( mode == 0 ) { // Job
									if ( !LogView_Open_Process_Log_Get_FileName( hdlg , i , -1 , MessageBuffer3 , ModuleStr ) ) {
										j = -1;
									}
								}
								else { // view All , etc
									j = 0;
								}
							}
						}
						else {
							j = -1;
						}
						//
						if ( j > 0 ) {
							//
							if ( COMP_HAS_PLOT_FUNC() ) {
								//
								if ( LOG_OPEN_TERMINATE() ) { // 2014.02.05
									//
									//HANDLE|DESP|NAME|PollTime|FileName|STARTTIME|ENDTIME|IOCOUNT|LOGCOUNT|item1|item2|item3|item4
									//
									JOB_Get_Display_String2( i , "PROCESS_LOG" , MessageBuffer2 );
									//
//printf( "[MDL=%s][FILE=%s]\n" , ModuleStr , MessageBuffer3 );
//									sprintf( MessageBuffer , "TABLE_USER_APPEND%d %d|%s|%s||%s|%s||||" , ( LOG_ENCRIPTION == 0 ) ? 1 : 2 , (int) hdlg , MessageBuffer2 , MessageBuffer3 , MessageBuffer3 , JOB_DATA[ i ].RUN_TIME ); // 2012.06.26 // 2016.12.17



									sprintf( MessageBuffer , "TABLE_USER_APPEND%d %d|%s|%s||%s|%s||||%s|" , ( LOG_ENCRIPTION == 0 ) ? 1 : 2 , (int) hdlg , MessageBuffer2 , MessageBuffer3 , MessageBuffer3 , JOB_DATA[ i ].RUN_TIME , ModuleStr ); // 2012.06.26 // 2016.12.17


//									sprintf( MessageBuffer , "TABLE_USER_COMPARE %d|%s" , (int) hdlg , JOB_COMP_FILENAME );




									COMP_CALL_PLOT_FUNC( hdlg , MessageBuffer );
								}
							}
							else {
								j = 0;
							}
							//
						}
						//
						if ( j == 0 ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Cannot Open File" , "Plot View Impossible\nPlease use PROCESS_PLOT_VIEWER Function" , MB_ICONHAND );
						}
						//
						break;

					case CTR_PROCESS_DATA_MODULE :
						//
						s = 0;
						//=========================================================================================
						if      ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
							s = 1;
							sprintf( MessageBuffer , "Process Data per Module|Process Data per Module[ALL]" );
							//=========================================================================================
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
								sprintf( MessageBuffer3 , "|[PRE]-Process Data per Module" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
								sprintf( MessageBuffer3 , "|[POST]-Process Data per Module" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							//=========================================================================================
						}
						else {
							sprintf( MessageBuffer , "Process Data per Module" );
							//=========================================================================================
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
								s = 1;
								sprintf( MessageBuffer3 , "|[PRE]-Process Data per Module" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
								s = 1;
								sprintf( MessageBuffer3 , "|[POST]-Process Data per Module" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							//=========================================================================================
						}
						//=========================================================================================
						if ( s == 1 ) {
							//
							j = -1;
							if ( !MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBuffer , &j ) ) {
								//=============================================================
								LOG_WINDOW_OPEN = FALSE;
								//=============================================================
								return TRUE;
								//=============================================================
							}
							//
							// 2017.03.13
							if ( !LOG_WINDOW_OPEN ) return TRUE;
							//
							switch( j ) {
							case 0 :
								s = 0;
								break;
								//
							case 1 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									s = 1;
								}
								else {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								break;
							case 2 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								else {
									s = 3;
								}
								break;
							case 3 :
								s = 3;
								break;
							}
							//=========================================================================================
						}
						//
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.14
							if      ( mode == 0 ) { // view All
								j = -1;
								posdata = -1;
							}
							else if ( mode == 1 ) { // Select Job
								j = -1;
								posdata = -1;
							}
							else if ( mode == 2 ) { // alarm
								j = -1;
								posdata = -1;
							}
							else if ( mode == 3 ) { // Select Slot
								j = subid;
								posdata = -1;
							}
//							else if ( mode == 4 ) { // pm // 2016.08.17
							else if ( ( mode == 4 ) || ( mode == 7 ) ) { // pm // 2016.08.17
								if ( !TreeData_Get_Select_Slot_PM_Info( i , subid , Temp_SlotStr , Temp_Recipe , pmorg ) ) break;
								//
								posdata = atoi( Temp_Recipe + 2 );
							}
							else {
								break;
							}
							//
//							if ( LogView_Open_Module_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , j , posdata , MessageBuffer ) ) { // 2012.06.26
							if ( LogView_Open_Module_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , IDC_CONTROL_S2_AREA_LAST_OPEN == CTR_PROCESS_DATA_MODULE , j , posdata , MessageBuffer ) ) { // 2012.06.26
								//
								JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , "" , "" );
								//
							}
							//
						}
						else {
//							LogView_Open_Module_Log( hdlg , i , s /*Main,All,Pre,Post*/ ); // 2012.06.26
							LogView_Open_Module_Log_S( hdlg , i , s /*Main,All,Pre,Post*/ , ( Get_Prm_DISPLAY_MODE() == 0 ) ); // 2012.06.26
						}
						//=========================================================================================
						break;

					case CTR_RECIPE_DATA_1_WAFER :
						//
						posindex = 0;
						if ( IsWindowVisible( GetDlgItem( hdlg , IDC_LIST_JOBDATA_S2 ) ) ) {
							if ( IDC_CONTROL_S2_AREA_LAST_OPEN == CTR_RECIPE_DATA_1_WAFER ) {
								if ( strlen( IDC_CONTROL_S2_AREA_LAST_DATA ) > 0 ) {
									posindex = 1;
								}
							}
						}
						//
						if ( posindex == 1 ) {
							//
							sprintf( MessageBuffer3 , IDC_CONTROL_S2_AREA_LAST_DATA );
							//
							if ( LogView_Open_Recipe_Log_Data_Set( hdlg , IDC_LIST_JOBDATA_S2 , MessageBuffer3 , MessageBuffer , 1 , TRUE ) ) {
								//
								JOBDATA_S2_DISPLAY_CONTROL( hdlg , 0 , -1 , "" , "" , "" );
								//
							}
							//
						}
						else {
							//
							s = 0;
							//=========================================================================================
							sprintf( MessageBuffer , "Recipe Data per %s" , Get_Prm_METHOD_STRING() );
							//=========================================================================================
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
								s = 1;
								sprintf( MessageBuffer3 , "|[PRE]-Recipe Data per %s" , Get_Prm_METHOD_STRING() );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
								s = 1;
								sprintf( MessageBuffer3 , "|[POST]-Recipe Data per %s" , Get_Prm_METHOD_STRING() );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							//=========================================================================================
							if ( s == 1 ) {
								//
								j = -1;
								if ( !MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBuffer , &j ) ) {
									//=============================================================
									LOG_WINDOW_OPEN = FALSE;
									//=============================================================
									return TRUE;
									//=============================================================
								}
								//
								// 2017.03.13
								if ( !LOG_WINDOW_OPEN ) return TRUE;
								//
								switch( j ) {
								case 0 :
									s = 0;
									break;
									//
								case 1 :
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
									break;
								case 2 :
									s = 3;
									break;
								}
								//=========================================================================================
							}
							//
							if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.14
								if      ( mode == 0 ) { // view All
									j = -1;
								}
								else if ( mode == 1 ) { // Select Job
									j = -1;
								}
								else if ( mode == 2 ) { // Alarm
									j = -1;
								}
								else if ( mode == 3 ) { // Select Slot
									j = subid;
								}
								else if ( mode == 5 ) { // Select Alarm Slot
									j = subid;
								}
								//
//								if ( mode == 4 ) { // Select PM // 2016.08.17
								if ( ( mode == 4 ) || ( mode == 7 ) ) { // Select PM // 2016.08.17
									//
									//----------------------------------------------------------------------------------------------
									if ( TreeData_Get_Select_Slot_PM_Info( index , subid , Temp_SlotStr , Temp_Recipe , pmorg ) ) {
										//
										//----------------------------------------------------------------------------------------------
										JOB_Get_Directory( index , folderstr );
										//
										Get_Log_File_Name( MessageBuffer3 , "RECIPE" , 0 , folderstr , TRUE , 0 , 0 , Temp_SlotStr , 0 , pmorg , "" );
										//----------------------------------------------------------------------------------------------
										if ( LogView_Open_Recipe_Log_Data_Set( hdlg , IDC_LIST_JOBDATA_S2 , MessageBuffer3 , MessageBuffer , 0 , TRUE ) ) {
											//
											sprintf( MessageBuffer , "* RECIPE DATA : %s %s : %s" , Temp_Recipe , Temp_SlotStr , MessageBuffer2 );
											//
											JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , MessageBuffer3 , Temp_Recipe );
											//
										}
										//----------------------------------------------------------------------------------------------
									}
								}
								else {
									//
//									if ( LogView_Open_Recipe_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , j , MessageBuffer , MessageBuffer3 , 0 ) ) { // 2016.11.22
									if ( LogView_Open_Recipe_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , j , MessageBuffer , MessageBuffer3 , 0 , Temp_Recipe ) ) { // 2016.11.22
										//
//										JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , MessageBuffer3 ); // 2016.11.22
										JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , MessageBuffer3 , Temp_Recipe ); // 2016.11.22
										//
									}
									//
								}
								//
							}
							else {
								LogView_Open_Recipe_Log( hdlg , i , s /*Main,All,Pre,Post*/ );
							}
						}
						//=========================================================================================
						break;

					case CTR_LOT_SUMMARY_DATA :
						//
						s = 0;
						//=========================================================================================
						if      ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
							s = 1;
							sprintf( MessageBuffer , "Process Data per Lot|Process Data per Lot[ALL]" );
							//=========================================================================================
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
								sprintf( MessageBuffer3 , "|[PRE]-Process Data per Lot" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
								sprintf( MessageBuffer3 , "|[POST]-Process Data per Lot|" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							//=========================================================================================
						}
						else {
							sprintf( MessageBuffer , "Process Data per Lot" );
							//=========================================================================================
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
								//
								s = 1;
								//
								sprintf( MessageBuffer3 , "|[PRE]-Process Data per Lot" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
								//
								s = 1;
								//
								sprintf( MessageBuffer3 , "|[POST]-Process Data per Lot" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							//=========================================================================================
						}
						//=========================================================================================
						if ( s == 1 ) {
							//
							j = -1;
							if ( !MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBuffer , &j ) ) {
								//=============================================================
								LOG_WINDOW_OPEN = FALSE;
								//=============================================================
								return TRUE;
								//=============================================================
							}
							//
							// 2017.03.13
							if ( !LOG_WINDOW_OPEN ) return TRUE;
							//
							switch( j ) {
							case 0 :
								s = 0;
								break;
								//
							case 1 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									s = 1;
								}
								else {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								break;
							case 2 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								else {
									s = 3;
								}
								break;
							case 3 :
								s = 3;
								break;
							}
							//=========================================================================================
						}
						//
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.14
							//
							if ( LogView_Open_Lot_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , IDC_CONTROL_S2_AREA_LAST_OPEN == CTR_LOT_SUMMARY_DATA , MessageBuffer ) ) {
								//
								JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , "" , "" );
								//
							}
							//
						}
						else {
//							LogView_Open_Lot_Log( hdlg , i , s /*Main,All,Pre,Post*/ ); // 2012.06.26
							LogView_Open_Lot_Log_S( hdlg , i , s /*Main,All,Pre,Post*/ , ( Get_Prm_DISPLAY_MODE() == 0 ) ); // 2012.06.26
						}
						//=========================================================================================
						break;

					case CTR_LOT_SUMMARY_MODULE :
						//
						s = 0;
						//=========================================================================================
						if      ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
							//
							s = 1;
							//
							sprintf( MessageBuffer , "Process Data per Lot[PM]|Process Data per Lot[PM/ALL]" );
							//=========================================================================================
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
								sprintf( MessageBuffer3 , "|[PRE]-Process Data per Lot[PM]" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
								sprintf( MessageBuffer3 , "|[POST]-Process Data per Lot[PM]" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							//=========================================================================================
						}
						else {
							//
							sprintf( MessageBuffer , "Process Data per Lot[PM]" );
							//=========================================================================================
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
								//
								s = 1;
								//
								sprintf( MessageBuffer3 , "|[PRE]-Process Data per Lot[PM]" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							if ( ( Get_Info_PRE_POST_SHOW_MODE() == 2 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Post
								//
								s = 1;
								//
								sprintf( MessageBuffer3 , "|[POST]-Process Data per Lot[PM]" );
								strcat( MessageBuffer , MessageBuffer3 );
							}
							//=========================================================================================
						}
						//=========================================================================================
						if ( s == 1 ) {
							//
							j = -1;
							if ( !MODAL_DIGITAL_BOX1( hdlg , MessageBuffer2 , "Select Log Part" , MessageBuffer , &j ) ) {
								//=============================================================
								LOG_WINDOW_OPEN = FALSE;
								//=============================================================
								return TRUE;
								//=============================================================
							}
							//
							// 2017.03.13
							if ( !LOG_WINDOW_OPEN ) return TRUE;
							//
							switch( j ) {
							case 0 :
								s = 0;
								break;
								//
							case 1 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									s = 1;
								}
								else {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								break;
							case 2 :
								if ( Get_Prm_EXTENDVIEW_MODE() == 1 ) {
									if ( ( Get_Info_PRE_POST_SHOW_MODE() == 1 ) || ( Get_Info_PRE_POST_SHOW_MODE() >= 3 ) ) { // Pre
										s = 2;
									}
									else {
										s = 3;
									}
								}
								else {
									s = 3;
								}
								break;
							case 3 :
								s = 3;
								break;
							}
							//=========================================================================================
						}
						//
						if ( Get_Prm_DISPLAY_MODE() >= 2 ) { // 2011.07.14
							//
							if ( LogView_Open_LotPM_Log_Ext( hdlg , IDC_LIST_JOBDATA_S2 , i , s /*Main,All,Pre,Post*/ , MessageBuffer ) ) {
								//
								JOBDATA_S2_DISPLAY_CONTROL( hdlg , Get_CTR_Button_Data( wParam ) , selfrom , MessageBuffer , "" , "" );
								//
							}
							//
						}
						else {
							LogView_Open_LotPM_Log( hdlg , i , s /*Main,All,Pre,Post*/ );
						}
						//=========================================================================================
						break;
					}
					//=============================================================
					LOG_WINDOW_OPEN = FALSE; // 2007.05.16
					//=============================================================
					//===================================================================
					_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[CLOSE] Log View Popup Screen\n" );
					//===================================================================
					return TRUE;
			
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				case IDC_CTRL_ADD :
					if ( Get_Prm_NETMONITOR_MODE() ) { // 2003.12.18
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Add Impossible(Monitoring Mode)" , "Add Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Add Impossible(Read Only Mode)" , "Add Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( JOB_Question_Count_Full() ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Add Data Buffer is Full!" , "ERROR" , MB_ICONHAND );
					}
					else {
//						if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//						JOB_W_OPEN = TRUE; // 2003.09.16
//						GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ADD_JOB , IDD_E_ADD_JOB ) , hdlg , Console_Gui_For_Add_Job_Proc , (LPARAM) NULL ); // 2002.06.17
						//===================================================================
						_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[OPEN] Add Item Popup Screen\n" );
						//===================================================================
//						if ( GoModalDialogBoxParam( GETHINST( hdlg ) , KWIN_MAKEINTRESOURCE( IDD_ADD_JOB , IDD_E_ADD_JOB ) , hdlg , Console_Gui_For_Add_Job_Proc , (LPARAM) NULL ) ) { // 2004.01.19
//						if ( GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ADD_JOB , IDD_E_ADD_JOB ) , hdlg , Console_Gui_For_Add_Job_Proc , (LPARAM) NULL ) ) { // 2004.01.19 // 2004.08.10
						if ( GoModalDialogBoxParam( GETHINST( hdlg ) , KWIN_MAKEINTRESOURCE( IDD_ADD_JOB , IDD_E_ADD_JOB ) , hdlg , Console_Gui_For_Add_Job_Proc , (LPARAM) NULL ) ) { // 2004.01.19 // 2004.08.10
							//-------------------------------------
							EnterCriticalSection( &CR_SC );
							//-------------------------------------
							if ( !JOB_Insert_New_Job_Data(
								ADD_JOB_NAME ,
								ADD_JOB_RUN_RECIPE ,
								ADD_JOB_IN_CASS ,
								ADD_JOB_OUT_CASS ,
								ADD_JOB_START_SLOT ,
								ADD_JOB_END_SLOT , ADD_JOB_PROGRESS ) ) {
								LeaveCriticalSection( &CR_SC );
								MODAL_WINDOW_MESSAGE_BOX( hdlg , "Add Data Buffer is Full!" , "ERROR" , MB_ICONHAND );
								return TRUE;
							}
							//===================================================================
							_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Added Item %s - %s\n" , ADD_JOB_NAME , ADD_JOB_RUN_RECIPE );
							//===================================================================
							JobData_ListView_Insert( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							//
							InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
							//
							Kor_JOB_Save( JOB_DATA_FILENAME );
							//
							JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
							//-------------------------------------
							LeaveCriticalSection( &CR_SC );
							//-------------------------------------
						}
//						JOB_W_OPEN = FALSE; // 2003.09.16
						//===================================================================
						_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[CLOSE] Add Item Popup Screen\n" );
						//===================================================================
					}
					return TRUE;

				case IDC_CTRL_START :
					if ( Get_Prm_NETMONITOR_MODE() ) { // 2003.12.18
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Start Impossible(Monitoring Mode)" , "Start Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Start Impossible(Read Only Mode)" , "Start Impossible" , MB_ICONHAND );
						return TRUE;
					}
//					if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//					JOB_W_OPEN = TRUE; // 2003.09.16
					if ( !JobData_ListView_SelectMode( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , TRUE ) ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Please Select only one item!" , "ERROR" , MB_ICONHAND );
					}
					else {
//						i = JobData_Conv_ListView_to_JobIndex( JobData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ) ); // 2009.07.20
						i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
						if ( JOB_Get_Run_Status( i ) != JOB_READY ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Starting Error(already running)!" , "ERROR" , MB_ICONHAND );
						}
						else {
							if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Start Selected item ???" , "Start Check" , MB_ICONQUESTION | MB_YESNO ) ) {
								//-------------------------------------
								EnterCriticalSection( &CR_SC );
								//-------------------------------------
								JOB_Get_Display_String( i , MessageBuffer );
								//===================================================================
								_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Start Action with Selected Item (%s)\n" , MessageBuffer );
								//===================================================================
								if ( JOB_RunMode_Start_Action( hdlg , i ) ) {
									InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
									//
									Kor_JOB_Save( JOB_DATA_FILENAME );
									//
									JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
								}
								//-------------------------------------
								LeaveCriticalSection( &CR_SC );
								//-------------------------------------
							}
						}
					}
//					JOB_W_OPEN = FALSE; // 2003.09.16
					return TRUE;
			
				case IDC_CTRL_ABORT :
					if ( Get_Prm_NETMONITOR_MODE() ) { // 2003.12.18
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Abort Impossible(Monitoring Mode)" , "Abort Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Abort Impossible(Read Only Mode)" , "Abort Impossible" , MB_ICONHAND );
						return TRUE;
					}
//					if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//					JOB_W_OPEN = TRUE; // 2003.09.16
					if ( !JobData_ListView_SelectMode( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , TRUE ) ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Please Select only one item!" , "ERROR" , MB_ICONHAND );
					}
					else {
//						i = JobData_Conv_ListView_to_JobIndex( JobData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ) ); // 2009.07.20
						i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
						if ( JOB_Get_Run_Status( i ) != JOB_RUNNING ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Aborting Error (not running)!" , "ERROR" , MB_ICONHAND );
						}
						else {
							if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Abort Selected item ???" , "Abort Check" , MB_ICONQUESTION | MB_YESNO ) ) {
								//-------------------------------------
								EnterCriticalSection( &CR_SC );
								//-------------------------------------
								JOB_Get_Display_String( i , MessageBuffer );
								//===================================================================
								_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Abort Action with Selected Item (%s)\n" , MessageBuffer );
								//===================================================================
								if ( JOB_RunMode_Abort_Action( hdlg , i ) ) {
									InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
									//
									JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
								}
								//-------------------------------------
								LeaveCriticalSection( &CR_SC );
								//-------------------------------------
							}
						}
					}
//					JOB_W_OPEN = FALSE; // 2003.09.16
					return TRUE;

				case IDC_CTRL_END :
					if ( Get_Prm_NETMONITOR_MODE() ) { // 2003.12.18
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "End Impossible(Monitoring Mode)" , "End Impossible" , MB_ICONHAND );
						return TRUE;
					}
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) { // 2002.11.08
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "End Impossible(Read Only Mode)" , "End Impossible" , MB_ICONHAND );
						return TRUE;
					}
//					if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//					JOB_W_OPEN = TRUE; // 2003.09.16
					if ( !JobData_ListView_SelectMode( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , TRUE ) ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Please Select only one item!" , "ERROR" , MB_ICONHAND );
					}
					else {
//						i = JobData_Conv_ListView_to_JobIndex( JobData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ) ); // 2009.07.20
						i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
						if ( JOB_Get_Run_Status( i ) != JOB_RUNNING ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Ending Error (not running)!" , "ERROR" , MB_ICONHAND );
						}
						else {
							if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really End Selected item ???" , "Abort Check" , MB_ICONQUESTION | MB_YESNO ) ) {
								//-------------------------------------
								EnterCriticalSection( &CR_SC );
								//-------------------------------------
								JOB_Get_Display_String( i , MessageBuffer );
								//===================================================================
								_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] End Action with Selected Item (%s)\n" , MessageBuffer );
								//===================================================================
								if ( JOB_RunMode_End_Action( hdlg , i ) ) {
									InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
									//
									JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
								}
								//-------------------------------------
								LeaveCriticalSection( &CR_SC );
								//-------------------------------------
							}
						}
					}
//					JOB_W_OPEN = FALSE; // 2003.09.16
					return TRUE;

				case IDC_CTRL_CANCEL :
					if ( Get_Prm_NETMONITOR_MODE() ) { // 2003.12.18
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Cancel Impossible(Monitoring Mode)" , "Cancel Impossible" , MB_ICONHAND );
						return TRUE;
					}
//					if ( JOB_W_OPEN ) return TRUE; // 2003.09.16
//					JOB_W_OPEN = TRUE; // 2003.09.16
					if ( !JobData_ListView_SelectMode( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , TRUE ) ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Please Select only one item!" , "ERROR" , MB_ICONHAND );
					}
					else {
//						i = JobData_Conv_ListView_to_JobIndex( JobData_ListView_SelectData( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ) ); // 2009.07.20
						i = JobData_Conv_ListView_to_JobIndex( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , -1 ); // 2009.07.20
						if ( JOB_Get_Run_Status( i ) != JOB_WAITING ) {
							MODAL_WINDOW_MESSAGE_BOX( hdlg , "Cancel Error (not running)!" , "ERROR" , MB_ICONHAND );
						}
						else {
							if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Cancel Selected item ???" , "Cancel Check" , MB_ICONQUESTION | MB_YESNO ) ) {
								//-------------------------------------
								EnterCriticalSection( &CR_SC );
								//-------------------------------------
								JOB_Get_Display_String( i , MessageBuffer );
								//===================================================================
								_IO_OPERATE_PRINTF( FALSE , "JOBHISTORY" , "[RUN] Cancel Action with Selected Item (%s)\n" , MessageBuffer );
								//===================================================================
								JOB_RunMode_Cancel_Action( hdlg , i );
								//
								InvalidateRect( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , NULL , FALSE );
								//
								JobData_Button_Check( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) );
								//-------------------------------------
								LeaveCriticalSection( &CR_SC );
								//-------------------------------------
							}
						}
					}
//					JOB_W_OPEN = FALSE; // 2003.09.16
					return TRUE;

				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				//------------------------------------------------------------------------------------------------------------
				case IDYES :
					//-------------------------------------
					EnterCriticalSection( &CR_SC );
					//-------------------------------------
					JobData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_JOBDATA ) , GetDlgItem( hdlg , IDC_TREE ) );
					//-------------------------------------
					LeaveCriticalSection( &CR_SC );
					//-------------------------------------
					return TRUE;
			}					

			return TRUE;

		case WM_DESTROY:
			KWIN_DELETE_USER_BUTTON_BITMAP( 0 );
			if ( Get_Prm_DISPLAY_MODE() >= 2 ) ImageList_Destroy( g_Tree_Image );
			if ( uTimer ) KillTimer( hdlg , uTimer );
			if ( uTimer2 ) KillTimer( hdlg , uTimer2 );
			return TRUE;

	}
	return FALSE;
}

BOOL Extern_Appen_Log( char *strdata ) { // 2002.10.02
	int ic , ss , es , cnt , z , mode;
//	int failindex; // 2015.11.13 // 2016.03.29
	int failindex , failicon; // 2015.11.13 // 2016.03.29
	char jname[65];
	char mname[65];
	char recipe[65];
	char buffer[65];
	char starttime[65];
	char endtime[65];
	char elapsedtime[65];
	char folder[65]; // 2010.12.06

//APPENDLOG	Mode|JobName|MidName|LotRecipe|incm(side(from0))|StartSlot|ESlot|Cnt|StartTime|EndTime|ElapsedTime|Folder

	if ( Get_Prm_NETMONITOR_MODE() ) return FALSE; // 2003.12.18
	z = 0;
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , buffer , z , 64 ); mode = atoi(buffer);
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , jname , z , 64 );
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , mname , z , 64 );
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , recipe , z , 64 );
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , buffer , z , 64 ); ic = atoi(buffer);
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , buffer , z , 64 ); ss = atoi(buffer);
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , buffer , z , 64 ); es = atoi(buffer);
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , buffer , z , 64 ); cnt = atoi(buffer);
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , starttime , z , 64 );
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , endtime , z , 64 );
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , elapsedtime , z , 64 );
	z = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , folder , z , 64 ); // 2010.12.06

	if ( recipe[0] == 0 ) strcpy( recipe , " " ); // 2012.04.19

//	z = JOB_Insert_New_Job_Data_for_Extern( mode , jname , mname , recipe , ic , ss , es , starttime , starttime , endtime , elapsedtime , folder , cnt ); // 2015.11.13
//	z = JOB_Insert_New_Job_Data_for_Extern( mode , jname , mname , recipe , ic , ss , es , starttime , starttime , endtime , elapsedtime , folder , cnt , &failindex ); // 2015.11.13 // 2016.03.29
	z = JOB_Insert_New_Job_Data_for_Extern( mode , jname , mname , recipe , ic , ss , es , starttime , starttime , endtime , elapsedtime , folder , cnt , &failindex , &failicon ); // 2015.11.13 // 2016.03.29
	//

//_IO_CIM_PRINTF( "APPLOG S [mode=%d][z=%d][Fail=%d][Main_Handle=%d][disp=%d][%s]\n" , mode , z , failindex , Main_Handle , Get_Prm_DISPLAY_MODE() , strdata );


	if      ( z == 1 ) {
		if ( Main_Handle != NULL ) {
			JobData_ListView_Insert( GetDlgItem( Main_Handle , IDC_LIST_JOBDATA ) );
			InvalidateRect( GetDlgItem( Main_Handle , IDC_LIST_JOBDATA ) , NULL , FALSE );
			//
			if ( JobData_TreeView_Insert( GetDlgItem( Main_Handle , IDC_TREE ) ) ) InvalidateRect( GetDlgItem( Main_Handle , IDC_TREE ) , NULL , FALSE );
			//
			JobData_Button_Check( GetDlgItem( Main_Handle , IDC_LIST_JOBDATA ) );
		}
		Kor_JOB_Save( JOB_DATA_FILENAME );
		return TRUE;
	}
	else if ( z == 0 ) {
		if ( Main_Handle != NULL ) {
			//
			if ( failindex >= 0 ) { // 2015.11.13
				//
				if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
					//
//					z = TreeData_Sub_ImgChange( GetDlgItem( Main_Handle , IDC_TREE ) , NULL , failindex , 20 , 21 ); // 2016.03.29
					z = TreeData_Sub_ImgChange( GetDlgItem( Main_Handle , IDC_TREE ) , NULL , failindex , failicon + 40 , failicon + 40 ); // 2016.03.29
					//
//_IO_CIM_PRINTF( "APPLOG 0 [z=%d][Fail=%d][Trh=%d]\n" , z , failindex , GetDlgItem( Main_Handle , IDC_TREE ) );
				}
			}
			//
			InvalidateRect( GetDlgItem( Main_Handle , IDC_LIST_JOBDATA ) , NULL , FALSE );
		}
		Kor_JOB_Save( JOB_DATA_FILENAME );
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
Module_Status Program_Main() {
	HWND hWnd;
	//----------------------------------
	//
	GLISTVIEW_HEIGHT = _GET_GLOBAL_VARIABLE_INT( "$LISTVIEW_HEIGHT" , NULL );
	//
	//----------------------------------
	InitCommonControls();
	//----------------------------------
	KGUI_Set_BACKGROUND( PROGRAM_PARAMETER_READ() );
	//
	if ( Get_Prm_DISPLAY_MODE() > 1 ) {
		KGUI_PALETTE_Regist_ColorTbl(  79 , 129 , 189 );	// 파랑,강조1
		KGUI_PALETTE_Regist_ColorTbl( 192 ,  80 ,  77 );	// 빨강,강조2
		KGUI_PALETTE_Regist_ColorTbl( 155 , 187 ,  89 );	// 황록색,강조3
		KGUI_PALETTE_Regist_ColorTbl( 128 , 100 , 162 );	// 자주,강조4
		KGUI_PALETTE_Regist_ColorTbl(  75 , 172 , 198 );	// 바다색,강조5
		KGUI_PALETTE_Regist_ColorTbl( 247 , 150 ,  70 );	// 주황,강조6
		KGUI_PALETTE_Regist_ColorTbl( 149 , 179 , 215 );	// 파랑,강조1, 40% 더 밝게
		KGUI_PALETTE_Regist_ColorTbl( 217 , 150 , 148 );	// 빨강,강조2, 40% 더 밝게
		KGUI_PALETTE_Regist_ColorTbl( 195 , 214 , 155 );	// 황록색,강조3, 40% 더 밝게
		KGUI_PALETTE_Regist_ColorTbl( 178 , 162 , 199 );	// 자주,강조4, 40% 더 밝게
		KGUI_PALETTE_Regist_ColorTbl( 146 , 205 , 220 );	// 바다색,강조5, 40% 더 밝게
		KGUI_PALETTE_Regist_ColorTbl( 250 , 192 , 143 );	// 주황,강조6, 40% 더 밝게
		//
		KGUI_PALETTE_LOADING( NULL );
	}
	else {
		KGUI_PALETTE_LOADING_SYSTEM( NULL );
	}
	//
	InitializeCriticalSection( &CR_SC );
	InitializeCriticalSection( &CR_SCREG );
	KWIN_DIALOG_DRIVER_LOADING();
	// 2002.06.17
	hWnd = GET_MAIN_WINDOW();
	//
	if ( Get_Prm_MINISCREEN_MODE() ) {
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_MAIN_CONTROL_M , IDD_E_MAIN_CONTROL_M ) , hWnd , Console_Gui_For_Main_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
	else {
		GoModalDialogBoxParam( GETHINST( hWnd ) , KWIN_MAKEINTRESOURCE( IDD_MAIN_CONTROL , IDD_E_MAIN_CONTROL ) , hWnd , Console_Gui_For_Main_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
	//
	DeleteCriticalSection( &CR_SC );
	DeleteCriticalSection( &CR_SCREG );
	//
//	SYSTEM_KILL_MYSELF(); // 2016.09.19
	//
	return( SYS_SUCCESS );
}
//
void Program_Enter_Code() {
	int	i , j , z1 , z2 , d , lvs;
	char	Buffer[ 256 + 1 ];
	char	Buffer1[ 128 + 1 ];
	char	Buffer2[ 128 + 1 ];
	char	Buffer3[ 128 + 1 ];
	char	Buffer4[ 128 + 1 ];


	LOG_OPEN_INIT(); // 2014.02.12

	COMP_INIT(); // 2016.12.28

	//--------------------------------------
	//             1          |       2       |      3      |      4       |       5        |    6     |         7         |      8       |    9     |       10        |          11
	// SYSTEM_FOLDER:LOGFOLDER|OPTION:LEVEL_IO|METHOD_STRING|JOBNAME_STRING|LOTRECIPE_STRING|MID_STRING|EXCLUSIVE_JOB_CHECK|RCS_NET_CONFIG|WID_STRING|TREE_ITEM_DISPLAY|POST_STRING:PRE_STRING
	//--------------------------------------
	Set_Prm_SYSTEM_DIRECTORY( "" );
	Set_Prm_LOG_DIRECTORY( "" );
	//
	Set_Prm_METHOD_STRING( "" );
	Set_Prm_JID_STRING( "" );
	Set_Prm_LID_STRING( "" );
	Set_Prm_MID_STRING( "" );
	Set_Prm_WID_STRING( "" );
	//
	for ( i = 0 ; i < MAX_SIDE ; i++ ) Set_Prm_EXCLUSIVE_JOB_CHECK( i , FALSE );
	//--------------------------------------
	//--------------------------------------
	z1 = 0;
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO1
	z1 = z2;
	STR_SEPERATE_CHAR( Buffer , ':' , Buffer1 , Buffer2 , 128 );
	Set_Prm_SYSTEM_DIRECTORY( Buffer1 );																//				NO1 - 1
	//=====================================================================
	if ( strlen( Buffer2 ) > 0 ) {																		//				NO1 - 2
		//==========================================================
		if ( strcmp( Buffer2 , "$" ) == 0 ) { // 2006.02.16
			_IO_GET_LOG_FOLDER( Buffer2 , 63 ); // 2006.02.16
		}
		//==========================================================
		Set_Prm_LOG_DIRECTORY( Buffer2 );
	}
	else {
		Set_Prm_LOG_DIRECTORY( Buffer1 );
	}
	//=====================================================================
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO2
	z1 = z2;
	//
	STR_SEPERATE_CHAR( Buffer , ':' , Buffer3 , Buffer4 , 128 ); // 2003.12.28
	//
	Set_Prm_READONLY_LEVEL_IO_NAME( Buffer4 ); // 2003.12.28
	//
	Set_GROUP_RUN_CHECK( FALSE );
	Set_Prm_MINISCREEN_MODE( FALSE );
	Set_Prm_NOTANAL_MODE( FALSE );
	Set_Prm_SAVEFILE_MODE( FALSE );
	Set_Prm_SCROLLBUTTON_MODE( FALSE );
	Set_Prm_TITLE_MODE( FALSE );
	Set_Prm_WINDOW_RETURN_MODE( FALSE );
	Set_Prm_READONLY_LEVEL( 0 );
	Set_Prm_EXTENDVIEW_MODE( 0 );
	Set_Prm_DELETE_HIDE( FALSE ); // 2006.08.30
	Set_Prm_SORT_MODE( FALSE ); // 2009.09.04
	Set_Prm_DISPLAY_MODE( 0 ); // 2010.11.30
	Set_Prm_STATUS_VIEW( TRUE ); // 2014.07.14

	AUTO_STEPVIEW = FALSE; // 2018.04.24

	d = 0;

	lvs = 0; // 2015.07.23
	//
	for ( i = 0 ; i < (signed) strlen( Buffer3 ) ; i++ ) {
		//
		if ( tolower( Buffer[i] ) == 'w' ) {
			//
			if ( ( tolower( Buffer[i+1] ) >= '0' ) && ( tolower( Buffer[i+1] ) <= '9' ) ) {
				//
				lvs = ( lvs * 10 ) + (tolower( Buffer[i+1] ) - '0');
				//
				i++;
				//
				if ( ( tolower( Buffer[i+1] ) >= '0' ) && ( tolower( Buffer[i+1] ) <= '9' ) ) {
					//
					lvs = ( lvs * 10 ) + (tolower( Buffer[i+1] ) - '0');
					//
					i++;
					//
				}
			}
			//
			continue;
		}
		//
		if ( tolower( Buffer[i] ) == 'g' ) Set_GROUP_RUN_CHECK( TRUE );
		if ( tolower( Buffer[i] ) == 'm' ) Set_Prm_MINISCREEN_MODE( TRUE );
		if ( tolower( Buffer[i] ) == 'x' ) Set_Prm_NOTANAL_MODE( TRUE );
		if ( tolower( Buffer[i] ) == 's' ) Set_Prm_SAVEFILE_MODE( TRUE );
		if ( tolower( Buffer[i] ) == 'b' ) Set_Prm_SCROLLBUTTON_MODE( TRUE );
		if ( tolower( Buffer[i] ) == 't' ) Set_Prm_TITLE_MODE( TRUE );
		if ( tolower( Buffer[i] ) == 'r' ) Set_Prm_WINDOW_RETURN_MODE( TRUE );
		if ( tolower( Buffer[i] ) == 'n' ) Set_Prm_NETMONITOR_MODE( TRUE );
		if ( tolower( Buffer[i] ) == 'a' ) Set_Prm_EXTENDVIEW_MODE( 1 ); // 2005.02.15
		if ( tolower( Buffer[i] ) == 'p' ) Set_Prm_EXTENDVIEW_MODE( 2 ); // 2005.02.15
		if ( tolower( Buffer[i] ) == 'h' ) Set_Prm_DELETE_HIDE( TRUE ); // 2006.08.30
		if ( tolower( Buffer[i] ) == 'o' ) Set_Prm_SORT_MODE( TRUE ); // 2009.09.04
		if ( tolower( Buffer[i] ) == 'd' ) d = 1; // 2006.01.25
		if ( tolower( Buffer[i] ) == 'e' ) LOG_ENCRIPTION = 1; // 2010.04.15
		if ( tolower( Buffer[i] ) == '0' ) Set_Prm_READONLY_LEVEL( 0 );
		if ( tolower( Buffer[i] ) == '1' ) Set_Prm_READONLY_LEVEL( 1 );
		if ( tolower( Buffer[i] ) == '2' ) Set_Prm_READONLY_LEVEL( 2 );
		if ( tolower( Buffer[i] ) == '3' ) Set_Prm_READONLY_LEVEL( 3 );
		if ( tolower( Buffer[i] ) == '4' ) Set_Prm_READONLY_LEVEL( 4 );
		if ( tolower( Buffer[i] ) == '5' ) Set_Prm_READONLY_LEVEL( 5 );
		if ( tolower( Buffer[i] ) == '6' ) Set_Prm_READONLY_LEVEL( 6 );
		if ( tolower( Buffer[i] ) == '7' ) Set_Prm_READONLY_LEVEL( 7 );
		if ( tolower( Buffer[i] ) == '8' ) Set_Prm_READONLY_LEVEL( 8 );
		if ( tolower( Buffer[i] ) == '9' ) Set_Prm_READONLY_LEVEL( 9 );
		if ( tolower( Buffer[i] ) == 'l' ) Set_Prm_DISPLAY_MODE( 1 ); // 2010.11.30
		if ( tolower( Buffer[i] ) == 'v' ) Set_Prm_DISPLAY_MODE( 2 ); // 2011.07.14
		if ( tolower( Buffer[i] ) == 'i' ) Set_Prm_DISPLAY_MODE( 3 ); // 2011.07.16
		if ( tolower( Buffer[i] ) == 'u' ) Set_Prm_STATUS_VIEW( TRUE ); // 2014.07.14
		if ( tolower( Buffer[i] ) == 'f' ) NAME_MANUAL_to_TIME = TRUE; // 2016.04.28
		if ( tolower( Buffer[i] ) == 'q' ) AUTO_STEPVIEW = TRUE; // 2018.04.24
	}
	//
	if ( lvs == 0 ) {
		if ( Get_Prm_DISPLAY_MODE() > 0 )
			lvs = 19;
		else
			lvs = 14;
		//
		LISTVIEW_HEIGHT_UPDATE = FALSE; // 2015.07.29
		//
	}
	else {
		//
		LISTVIEW_HEIGHT_UPDATE = TRUE; // 2015.07.29
		//
	}
	//
	LISTVIEW_HEIGHT = lvs;
	//
	if ( LISTVIEW_HEIGHT < 12 ) LISTVIEW_HEIGHT = 12;
	if ( LISTVIEW_HEIGHT > 48 ) LISTVIEW_HEIGHT = 48;
	//
	// Test
//	LISTVIEW_HEIGHT_UPDATE = TRUE;
//	LISTVIEW_HEIGHT = 21;
	//
	LISTVIEW_HEIGHT_RATIO = (double) LISTVIEW_HEIGHT / 19.0;
	if ( LISTVIEW_HEIGHT_RATIO < 1 ) LISTVIEW_HEIGHT_RATIO = 1;
	//--------------------------------------
	//
	// Test
	//
//	Set_Prm_DISPLAY_MODE( 3 );
	//--------------------------------------
	if ( Get_Prm_DISPLAY_MODE() != 0 ) { // 2011.06.20
		Set_Prm_SORT_MODE( TRUE );
	}
	if ( Get_Prm_DISPLAY_MODE() >= 2 ) {
//		Set_Prm_SCROLLBUTTON_MODE( TRUE ); // 2017.03.15
		Set_Prm_TITLE_MODE( FALSE );
	}
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO3
	z1 = z2;
	Set_Prm_METHOD_STRING( Buffer );
	//
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO4
	z1 = z2;
	Set_Prm_JID_STRING( Buffer );
	//
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO5
	z1 = z2;
	Set_Prm_LID_STRING( Buffer );
	//
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO6
	z1 = z2;
	Set_Prm_MID_STRING( Buffer );
	//
	if ( strlen( Get_Prm_METHOD_STRING() ) <= 0 ) Set_Prm_METHOD_STRING( "Wafer" );
	if ( strlen( Get_Prm_JID_STRING() ) <= 0 ) Set_Prm_JID_STRING( "JOB NAME" );
	if ( strlen( Get_Prm_LID_STRING() ) <= 0 ) Set_Prm_LID_STRING( "LOT RECIPE" );
	if ( strlen( Get_Prm_MID_STRING() ) <= 0 ) Set_Prm_MID_STRING( "MID NAME" );
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO7
	z1 = z2;
	for ( i = 0 ; i < (signed) strlen( Buffer ) ; i++ ) {
		j = Buffer[i] - '0' - 1;
		if ( ( j >= 0 ) && ( j < MAX_SIDE ) ) Set_Prm_EXCLUSIVE_JOB_CHECK( j , TRUE );
	}

	//--------------------------------------
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO8
	z1 = z2;

	if ( strlen( Buffer ) > 0 ) { // 2002.10.21
		Set_Prm_NETMONITOR_MODE( TRUE );
		STR_SEPERATE_CHAR( Buffer , ':' , Buffer3 , Buffer4 , 128 );
		sprintf( Buffer , "$Remote$\\[%s][%s]" , Buffer3 , Buffer4 );
		Set_Prm_NETMONITOR_FOLDER( Buffer );
		_nINF_KNFS_Prepare_for_ConnectionName( PROGRAM_FUNCTION_READ() );
		if ( _nINF_KNFS_CONNECTION_FOR_CLIENT( Buffer3 , atoi(Buffer4) ) != 0 ) {
			_IO_CIM_PRINTF( "Network KNFS Initialization Error\n" );
			return;
		}
	}
	else {
		Set_Prm_NETMONITOR_MODE( FALSE );
	}
	//--------------------------------------
	// 2005.02.15
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO9
	z1 = z2;
	//
	if ( strlen( Buffer ) <= 0 ) sprintf( Buffer , "%s ID" , Get_Prm_METHOD_STRING() );
	Set_Prm_WID_STRING( Buffer );
	//--------------------------------------
	// 2011.07.15
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO10
	z1 = z2;
	//
	Set_Prm_TREE_ITEM_DISPLAY( atoi( Buffer ) );
	//


	//--------------------------------------
	// 2018.09.04
	//--------------------------------------
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( PROGRAM_ARGUMENT_READ() , '|' , Buffer , z1 , 256 );			//	NO11
	z1 = z2;
	//
	STR_SEPERATE_CHAR( Buffer , ':' , Buffer1 , Buffer2 , 128 );
	//
	Set_Prm_POST_STRING( Buffer1 );
	Set_Prm_PRE_STRING( Buffer2 );
	//
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		if ( !FILE_ALL_SETUP( Get_Prm_SYSTEM_DIRECTORY() ) ) MessageBeep(MB_ICONHAND);
	}
	if ( strlen( Get_Prm_LOG_DIRECTORY() ) == 0 ) {
		if ( d == 1 ) { // 2006.01.25
			sprintf( JOB_DATA_FILENAME , "lotlog\\%s\\Job_list.dat" , GET_SYSTEM_NAME() );
			sprintf( JOB_INFO_FILENAME , "lotlog\\%s\\Job_Info.dat" , GET_SYSTEM_NAME() );
			sprintf( JOB_COMP_FILENAME , "lotlog\\%s\\Job_Comp.dat" , GET_SYSTEM_NAME() ); // 2016.12.28
		}
		else {
			sprintf( JOB_DATA_FILENAME , "lotlog\\Job_list.dat" );
			sprintf( JOB_INFO_FILENAME , "lotlog\\Job_Info.dat" );
			sprintf( JOB_COMP_FILENAME , "lotlog\\Job_Comp.dat" ); // 2016.12.28
		}
	}
	else {
		if ( d == 1 ) { // 2006.01.25
			sprintf( JOB_DATA_FILENAME , "%s\\lotlog\\%s\\Job_list.dat" , Get_Prm_LOG_DIRECTORY() , GET_SYSTEM_NAME() );
			sprintf( JOB_INFO_FILENAME , "%s\\lotlog\\%s\\Job_Info.dat" , Get_Prm_LOG_DIRECTORY() , GET_SYSTEM_NAME() );
			sprintf( JOB_COMP_FILENAME , "%s\\lotlog\\%s\\Job_Comp.dat" , Get_Prm_LOG_DIRECTORY() , GET_SYSTEM_NAME() ); // 2016.12.28
		}
		else {
			sprintf( JOB_DATA_FILENAME , "%s\\lotlog\\Job_list.dat" , Get_Prm_LOG_DIRECTORY() );
			sprintf( JOB_INFO_FILENAME , "%s\\lotlog\\Job_Info.dat" , Get_Prm_LOG_DIRECTORY() );
			sprintf( JOB_COMP_FILENAME , "%s\\lotlog\\Job_Comp.dat" , Get_Prm_LOG_DIRECTORY() ); // 2016.12.28
		}
	}
/*
	if ( strlen( Buffer2 ) == 0 ) {
		if ( strlen( Get_Prm_JOB_DIRECTORY() ) == 0 ) {
			if ( d == 1 ) { // 2006.01.25
				sprintf( JOB_DATA_FILENAME , "lotlog\\%s\\Job_list.dat" , GET_SYSTEM_NAME() );
				sprintf( JOB_INFO_FILENAME , "lotlog\\%s\\Job_Info.dat" , GET_SYSTEM_NAME() );
			}
			else {
				sprintf( JOB_DATA_FILENAME , "lotlog\\Job_list.dat" );
				sprintf( JOB_INFO_FILENAME , "lotlog\\Job_Info.dat" );
			}
		}
		else {
			if ( d == 1 ) { // 2006.01.25
				sprintf( JOB_DATA_FILENAME , "%s\\lotlog\\%s\\Job_list.dat" , Get_Prm_JOB_DIRECTORY() , GET_SYSTEM_NAME() );
				sprintf( JOB_INFO_FILENAME , "%s\\lotlog\\%s\\Job_Info.dat" , Get_Prm_JOB_DIRECTORY() , GET_SYSTEM_NAME() );
			}
			else {
				sprintf( JOB_DATA_FILENAME , "%s\\lotlog\\Job_list.dat" , Get_Prm_JOB_DIRECTORY() );
				sprintf( JOB_INFO_FILENAME , "%s\\lotlog\\Job_Info.dat" , Get_Prm_JOB_DIRECTORY() );
			}
		}
	}
	else {
//		if ( strlen( Get_Prm_JOB_DIRECTORY() ) == 0 ) { // 2006.02.06
			if ( d == 1 ) { // 2006.01.25
//				sprintf( JOB_DATA_FILENAME , "lotlog\\%s\\%s\\Job_list.dat" , GET_SYSTEM_NAME() , Buffer2 ); // 2006.02.06
//				sprintf( JOB_INFO_FILENAME , "lotlog\\%s\\%s\\Job_Info.dat" , GET_SYSTEM_NAME() , Buffer2 ); // 2006.02.06
				sprintf( JOB_DATA_FILENAME , "%s\\%s\\Job_list.dat" , Buffer2 , GET_SYSTEM_NAME() ); // 2006.02.06
				sprintf( JOB_INFO_FILENAME , "%s\\%s\\Job_Info.dat" , Buffer2 , GET_SYSTEM_NAME() ); // 2006.02.06
			}
			else {
//				sprintf( JOB_DATA_FILENAME , "lotlog\\%s\\Job_list.dat" , Buffer2 ); // 2006.02.06
//				sprintf( JOB_INFO_FILENAME , "lotlog\\%s\\Job_Info.dat" , Buffer2 ); // 2006.02.06
				sprintf( JOB_DATA_FILENAME , "%s\\Job_list.dat" , Buffer2 ); // 2006.02.06
				sprintf( JOB_INFO_FILENAME , "%s\\Job_Info.dat" , Buffer2 ); // 2006.02.06
			}
//		} // 2006.02.06
//		else { // 2006.02.06
//			if ( d == 1 ) { // 2006.01.25
//				sprintf( JOB_DATA_FILENAME , "%s\\lotlog\\%s\\%s\\Job_list.dat" , Get_Prm_JOB_DIRECTORY() , GET_SYSTEM_NAME() , Buffer2 ); // 2006.02.06
//				sprintf( JOB_INFO_FILENAME , "%s\\lotlog\\%s\\%s\\Job_Info.dat" , Get_Prm_JOB_DIRECTORY() , GET_SYSTEM_NAME() , Buffer2 ); // 2006.02.06
//			}
//			else {
//				sprintf( JOB_DATA_FILENAME , "%s\\lotlog\\%s\\Job_list.dat" , Get_Prm_JOB_DIRECTORY() , Buffer2 ); // 2006.02.06
//				sprintf( JOB_INFO_FILENAME , "%s\\lotlog\\%s\\Job_Info.dat" , Get_Prm_JOB_DIRECTORY() , Buffer2 ); // 2006.02.06
//			}
//		}
	}
*/
	if ( !Get_Prm_NETMONITOR_MODE() ) {
		Kor_Info_Open( JOB_INFO_FILENAME );
	}
	else {
		strcpy( JOB_DATA_FILENAME_ORG , JOB_DATA_FILENAME );
		sprintf( JOB_DATA_FILENAME , "%s\\%s" , Get_Prm_NETMONITOR_FOLDER() , JOB_DATA_FILENAME_ORG );
	}
	Kor_JOB_Open( JOB_DATA_FILENAME );
	
	// 2010.10.27 jiahn Begin ( Required by T )
	// Comment - auto delete month set file("Job_Editor_Info.inf") read when loading
//	Read_JobLog_Auto_Delete_Time();  // 2010.11.18
	// 2010.10.27 jiahn End ( Required by T )
}
//-------------------------------------------------------------------------
BOOL Program_Init_Code() {
	//---------------------------------------------------
	char BufBuild[32];
	sprintf( BufBuild , "<BUILD>%s %s" , __DATE__ , __TIME__ );	REGIST_FILE_INFO( BufBuild );
	//----------------------------------
	//REGIST_FILE_INFO( "Gui Job Editor 2.1(2001.08.09)" ); // Update Find + Update View
	//REGIST_FILE_INFO( "Gui Job Editor 2.2(2001.08.21)" ); // Update Wafer ID Search
	//REGIST_FILE_INFO( "Gui Job Editor 2.3(2001.11.11)" ); // Update read Error
	//REGIST_FILE_INFO( "Gui Job Editor 2.4(2001.11.20)" ); // File Read upto 3 times
	//REGIST_FILE_INFO( "Gui Job Editor 2.5(2001.12.04)" ); // File Read
	//REGIST_FILE_INFO( "Gui Job Editor 3.0(2002.01.01)" ); // CimSqCon Change Rebuild
	//REGIST_FILE_INFO( "Gui Job Editor 3.1(2002.02.15)" ); // Button Position/Size Fixed
	//REGIST_FILE_INFO( "Gui Job Editor 3.2(2002.03.10)" ); // Scroll Button Append
//	REGIST_FILE_INFO( "Gui Job Editor 3.21(2002.03.19)" ); // Average Div Bug Fixed
//	REGIST_FILE_INFO( "Gui Job Editor 3.3(2002.06.17)" ); // Loop Wafer History View Part Update
	                                                      // Window Instance Change
//	REGIST_FILE_INFO( "Gui Job Editor 3.31(2002.07.08)" ); // Log View expand 3 -> 4 column
//	REGIST_FILE_INFO( "Gui Job Editor 3.32(2002.08.09)" ); // Progress mode Display Remove
//	REGIST_FILE_INFO( "Gui Job Editor 3.4(2002.10.02)" ); // Append Extern Insert Log & Title Show
//	REGIST_FILE_INFO( "Gui Job Editor 3.41(2002.10.07)" ); // Plot Max Overflow Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 3.5(2002.10.10)" ); // Display ID at Dialog Window & Back Select
//	REGIST_FILE_INFO( "Gui Job Editor 3.6(2002.10.18)" ); // Auto Delete Part Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 3.7(2002.10.25)" ); // Append Possible network monitor
	                                                      // Display Wafer ID at Dialog Window & Process Back Select modify
//	REGIST_FILE_INFO( "Gui Job Editor 3.8(2002.11.08)" ); // Append Readonly mode
//	REGIST_FILE_INFO( "Gui Job Editor 3.81(2002.11.11)" ); // Scroll Button Append(All Popup screen) : crkim
	                                                       // ListView screen close after Six item select screen display
//	REGIST_FILE_INFO( "Gui Job Editor 3.9(2003.06.09)" ); // Log Append for Item Status
//	REGIST_FILE_INFO( "Gui Job Editor 3.91(2003.07.25)" ); // Log Message separate
//	REGIST_FILE_INFO( "Gui Job Editor 3.95(2003.07.30)" ); // File Version 3.0 for small size
//	REGIST_FILE_INFO( "Gui Job Editor 3.96(2003.08.01)" ); // IO Pointer Read point change(at run)
//	REGIST_FILE_INFO( "Gui Job Editor 3.97(2003.09.16)" ); // Run Check(for Log) Part Update & Overflow Bug Update & Append Operate Log
//	REGIST_FILE_INFO( "Gui Job Editor 4.0(2003.12.15)" ); // Append MonitorMode
//	REGIST_FILE_INFO( "Gui Job Editor 4.01(2004.01.06)" ); // Bug Update for process log view at Waferlog & Update Access Control Part
//	REGIST_FILE_INFO( "Gui Job Editor 4.1(2004.01.14)" ); // Expand Buffer Size & Auto Delete Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.11(2004.01.19)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Job Editor 4.2(2004.02.27)" ); // Log View Part Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.21(2004.08.10)" ); // Window Instance Change
//	REGIST_FILE_INFO( "Gui Job Editor 4.3(2004.10.12)" ); // Display X Size Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.4(2005.02.15)" ); // Append Extend View Mode
//	REGIST_FILE_INFO( "Gui Job Editor 4.41(2005.02.28)" ); // CPU OverHead Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.5(2005.04.08)" ); // Button Position remap part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.51(2006.02.23)" ); // Pre/Post Log View Part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.52(2006.03.09)" ); // Log Part File Exist Check Part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.53(2006.08.30)" ); // Delete Button Hide Mode Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.54(2006.09.05)" ); // Save As Button Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.55(2006.10.24)" ); // CM Log Module Name Part Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.56(2007.01.03)" ); // BM Log Part Append & Lot Recipe Nothing Trouble Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.57(2007.05.16)" ); // X Size Expand to 80 & Auto Delete Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.58(2007.06.20)" ); // Invalid Wafer Display Color Change
//	REGIST_FILE_INFO( "Gui Job Editor 4.59(2007.06.25)" ); // LotPre,LotPost Log View part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.6(2007.07.31)" ); // Open File Size Detect Part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.7(2007.11.07)" ); // Dummy Wafer Part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.71(2007.12.07)" ); // Time Based Order Part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.72(2008.02.27)" ); // CM Name Converting Part Bug Update / log calculate part
//	REGIST_FILE_INFO( "Gui Job Editor 4.73(2008.04.16)" ); // BM Name Converting Part Bug Update / Wafer ID Memory Size Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.74(2009.07.21)" ); // User Log part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.75(2009.09.04)" ); // Sort part Append
//	REGIST_FILE_INFO( "Gui Job Editor 4.76(2009.09.20)" ); // Job Insert Part Debug, Job_name extend to 64 ch.
//	REGIST_FILE_INFO( "Gui Job Editor 4.77(2010.04.15)" ); // Encryption
//	REGIST_FILE_INFO( "Gui Job Editor 4.78(2010.05.19)" ); // Encryption Save Part
//	REGIST_FILE_INFO( "Gui Job Editor 4.8(2010.07.15)" ); //  window MessageBox
//	REGIST_FILE_INFO( "Gui Job Editor 4.81(2010.10.27)" ); //  Old Data Auto Delete File Save added ("System\\Job_Editor_Info.inf")
//	REGIST_FILE_INFO( "Gui Job Editor 4.82(2010.11.16)" ); //  Delete Check Detail
//	REGIST_FILE_INFO( "Gui Job Editor 4.83(2010.12.02)" ); //  EIL FirstIn Refresh
//	REGIST_FILE_INFO( "Gui Job Editor 4.84(2011.01.17)" ); //  Expand Cassette Size
//	REGIST_FILE_INFO( "Gui Job Editor 4.85(2011.07.27)" ); //  Expand
//	REGIST_FILE_INFO( "Gui Job Editor 4.86(2011.12.07)" ); //  Encryption
//	REGIST_FILE_INFO( "Gui Job Editor 4.87(2012.03.22)" ); //  Alarm Info
//	REGIST_FILE_INFO( "Gui Job Editor 4.88(2012.04.13)" ); //  Multi Log for Loop Count
//	REGIST_FILE_INFO( "Gui Job Editor 4.89(2012.04.23)" ); //  GUI Reposition
//	REGIST_FILE_INFO( "Gui Job Editor 4.90(2012.11.26)" ); //  Prcs/Flow Grph
//	REGIST_FILE_INFO( "Gui Job Editor 4.91(2013.12.05)" ); //  Multi PM Slot
//	REGIST_FILE_INFO( "Gui Job Editor 4.92(2014.01.16)" ); //  User Log
//	REGIST_FILE_INFO( "Gui Job Editor 4.93(2014.02.13)" ); //  Big Size Log
//	REGIST_FILE_INFO( "Gui Job Editor 4.94(2014.07.17)" ); //  Big Size Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 4.95(2014.10.10)" ); //  File save size bug update
//	REGIST_FILE_INFO( "Gui Job Editor 4.96(2015.04.10)" ); //  PopupAutoClose,VC90
//	REGIST_FILE_INFO( "Gui Job Editor 4.97(2015.05.27)" ); //  Save Title , Fail Color
//	REGIST_FILE_INFO( "Gui Job Editor 4.98(2015.06.04)" ); //  Method View Color Bug
//	REGIST_FILE_INFO( "Gui Job Editor 4.99(2015.07.23)" ); //  Data Show Bug when expand screen
//	REGIST_FILE_INFO( "Gui Job Editor 5.0(2015.11.11)" ); //  Process log slow load update
//	REGIST_FILE_INFO( "Gui Job Editor 5.1(2016.03.29)" ); //  Process log step , MultiIcon
//	REGIST_FILE_INFO( "Gui Job Editor 5.2(2016.04.08)" ); //  ListView_GetColumnWidth 잘못된 사용
//	REGIST_FILE_INFO( "Gui Job Editor 5.21(2016.06.01)" ); // ListView_GetColumnWidth 잘못된 사용(Critical Section)
//	REGIST_FILE_INFO( "Gui Job Editor 5.3(2016.08.26)" ); //  Step View
//	REGIST_FILE_INFO( "Gui Job Editor 5.31(2016.10.26)" ); //  Step View Color Edit
//	REGIST_FILE_INFO( "Gui Job Editor 5.32(2017.01.31)" ); //  Step Time Calculate Bug 수정
//	REGIST_FILE_INFO( "Gui Job Editor 6.0(2017.03.01)" ); // Multi Screen,Refresh Update,bmp_button
//	REGIST_FILE_INFO( "Gui Job Editor 6.01(2017.05.18)" ); // Process log File Error Popup Delete
//	REGIST_FILE_INFO( "Gui Job Editor 6.1(2017.08.18)" ); // Module_Log_Name Set Bug Update
//	REGIST_FILE_INFO( "Gui Job Editor 6.2(2018.01.10)" ); // winlock
//	REGIST_FILE_INFO( "Gui Job Editor 6.3(2018.02.10)" ); // flowhistory script
//	REGIST_FILE_INFO( "Gui Job Editor 6.4(2018.05.05)" ); // Auto Step View
	REGIST_FILE_INFO( "Gui Job Editor 6.5(2018.09.04)" ); // Post Pre String
	//----------------------------------
	//
	IO_READ_PART();
	//
	CURRENT_WAFER_INIT( 1 );
	//
	STEP_CHECK_INIT(); // 2016.08.18
	//
	return TRUE;
}

