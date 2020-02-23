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

#define	 GDF_FILE_MODIFY_ON		g_bEdited	=	TRUE;	g_bApply	=	TRUE;
#define	 GDF_FILE_MODIFY_OFF	g_bEdited	=	FALSE;	g_bApply	=	FALSE;

UINT	 g_dValveNo		=	1;
int		 g_dStatus		=	0;		// Dialog 실행 시킬때 벨브 영역인지를 파악해서 셋팅할 것
BOOL	 g_bGuideLine	=	FALSE;
BOOL	 g_bEdited		=	FALSE;
BOOL	 g_bApply		=	FALSE;
HWND	 g_hDlg			=	NULL;
BOOL	 g_bSpinCkeckStart		=	FALSE;
BOOL	 g_bImageLoadSuccess	=	FALSE;

UINT	g_OldValveNo	=	1;
int		g_Old_X			=	0;
int		g_Old_Y			=	0;




WNDPROC	 Default_Message_Proc;
WNDPROC	 Spin_Default_Message_Proc;


void GUI_Main_Lib_Load( void );
void GUI_LIB_All_Draw( HWND hdlg );
void Make_GDF_Path( char* szGDFPath );
IMG_NODE* Find_Img_Node( int dValNo );
void Draw_Item_Preview( HWND hdlg, int dValveNo, int dStatus );

BOOL APIENTRY Spin_Msg_Handler( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Adjust_Msg_Handler( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );

extern	 HINSTANCE			g_hDllInstance;
extern	 BOOL				g_bGuiEdit;
extern	 char				g_szDLLPath[MAX_PATH];
extern	 IMG_NODE*			pStartNode;
extern	 HINSTANCE			g_hInstance;
extern	 char				g_szValveName[MAX_VAL_NO][MAX_VAL_NAME_LENGTH];


// Control에 정보를 표시
BOOL Adjust_Set_Old_Value( HWND hdlg )
{
	HWND			hParent	=	NULL;
	IMG_NODE*		pNode	=	NULL;

	pNode	=	Find_Img_Node( g_dValveNo );

	if ( pNode == NULL )		return FALSE;

	g_OldValveNo	=	g_dValveNo;
	g_Old_X			=	pNode->dx;
	g_Old_Y			=	pNode->dy;

	hParent			=	GetParent( hdlg );
	InvalidateRect( hParent, NULL, TRUE );


	return TRUE;
}

BOOL Adjust_Restore_Old_Value( HWND hdlg )
{
	HWND			hParent	=	NULL;
	IMG_NODE*		pNode	=	NULL;

	g_dValveNo	=	g_OldValveNo;

	pNode	=	Find_Img_Node( g_dValveNo );

	if ( pNode == NULL )		return FALSE;

	pNode->dx	=	g_Old_X;
	pNode->dy	=	g_Old_Y;

	hParent			=	GetParent( hdlg );
	InvalidateRect( hParent, NULL, TRUE );

	return TRUE;
}


void Adjust_Info_Set( HWND hdlg )
{
	UINT		uX	=	0;
	UINT		uY	=	0;
	UINT		uImgNo	=	0;

	HWND		hNextValBut, hPrevValBut;
	HWND		hNextStatus, hPrevStatus;
	HWND		hPrevImg;
	HWND		hSaveBut;
	HWND		hApplyBut;

	char		szValveName[20]	=	"";
	char		szValv_RealName[50]	=	"";
	
	IMG_NODE*	pNode	=	NULL;

	// Get Valve Select Control Button Handle
	hNextValBut = GetDlgItem( hdlg , IDC_VAL_NEXT );
	hPrevValBut = GetDlgItem( hdlg , IDC_VAL_PREV );

	// Get Status Select Control Button Handle 
	hNextStatus = GetDlgItem( hdlg , IDC_IMG_STNEXT );
	hPrevStatus = GetDlgItem( hdlg , IDC_IMG_STPREV );

	// Get Image Button Handle
	hPrevImg	= GetDlgItem( hdlg , IDC_IMG_PREV );

	hSaveBut	= GetDlgItem( hdlg, IDC_SAVE );
	hApplyBut	= GetDlgItem( hdlg, IDC_APPLY );

	EnableWindow( hSaveBut, g_bEdited );
	EnableWindow( hApplyBut, g_bApply );

	pNode = Find_Img_Node( g_dValveNo );
	if ( pNode != NULL )
	{
		uX = (UINT) pNode->dx;
		uY = (UINT) pNode->dy;
		strcpy( szValveName, pNode->szValveName );
		uImgNo	=	(UINT) pNode->dImgNo;
	}

	// Valve Select Button Control
	if (( pNode->NextNode) == NULL )	EnableWindow( hNextValBut, FALSE );
	else								EnableWindow( hNextValBut, TRUE );

	if (( pNode->PrevNode) == NULL )	EnableWindow( hPrevValBut, FALSE );
	else								EnableWindow( hPrevValBut, TRUE );



	// Status Select Control Button
	if ( g_dStatus == 0 )				EnableWindow( hPrevStatus, FALSE );
	else								EnableWindow( hPrevStatus, TRUE );

	if ( g_dStatus == VAL_STATUS_MAX )	EnableWindow( hNextStatus, FALSE );
	else								EnableWindow( hNextStatus, TRUE );

	if ( uImgNo == 0 )					EnableWindow( hPrevImg, FALSE );
	else								EnableWindow( hPrevImg, TRUE );

//	SetDlgItemInt( hdlg, IDC_VAL_NO, g_dValveNo, FALSE );

	sprintf( szValv_RealName, "Valve %s[%d]", g_szValveName[g_dValveNo - 1] , g_dValveNo );
	SetDlgItemText( hdlg, IDC_VAL_NO, szValv_RealName );

	SetDlgItemInt( hdlg, IDC_LOC_X, uX , FALSE );
	SetDlgItemInt( hdlg, IDC_LOC_Y, uY , FALSE );	
	SetDlgItemText( hdlg, IDC_VALVE_NAME, szValveName );
	SetDlgItemInt( hdlg, IDC_IMAGE_NO, uImgNo , FALSE );	
	SetDlgItemInt( hdlg, IDC_IMAGE_STATUS, g_dStatus, FALSE );

	Draw_Item_Preview( hdlg, g_dValveNo, g_dStatus );
}


void OnAdjMsgOK( HWND hdlg )
{	
	int			dx, dy;
	int			dImgNo;
	char		szValveName[20];

	BOOL		bResult	=	FALSE;



	HWND		hParents	=	NULL;
	IMG_NODE*	pNode		=	FALSE;

	hParents = GetParent( hdlg );			//	Parents Window Handle

	pNode = Find_Img_Node( g_dValveNo );
	if ( pNode == NULL )
	{
		Adjust_Info_Set( hdlg );
	}
	else
	{
		// X좌표 수정
		dx = GetDlgItemInt( hdlg, IDC_LOC_X, &bResult, FALSE );
		if ( bResult )	pNode->dx	=	dx;
		else			pNode->dx	=	0;

		// Y좌표 수정
		dy = GetDlgItemInt( hdlg, IDC_LOC_Y, &bResult, FALSE );
		if ( bResult )	pNode->dy	=	dy;
		else			pNode->dy	=	0;

		// 이미지 수정
		dImgNo = GetDlgItemInt( hdlg, IDC_IMAGE_NO, &bResult, FALSE );
		if ( bResult ) pNode->dImgNo	=	dImgNo;

		// Valve 이름 수정
		if ( GetDlgItemText( hdlg, IDC_VALVE_NAME, szValveName, 19 ) )
		{
			strcpy( pNode->szValveName, szValveName );
		}

		Adjust_Set_Old_Value( hdlg );
		InvalidateRect( hParents, NULL, TRUE );
	}


	g_bGuideLine		=	FALSE;
	g_bSpinCkeckStart	=	FALSE;
	EndDialog( hdlg, 1 );
}

void OnAdjMsgApply( HWND hdlg )
{
	int			dx, dy;
	int			dImgNo;
	char		szValveName[20];

	BOOL		bResult	=	FALSE;



	HWND		hParents	=	NULL;
	IMG_NODE*	pNode		=	FALSE;

	hParents = GetParent( hdlg );			//	Parents Window Handle
	g_bApply	=	FALSE;

	pNode = Find_Img_Node( g_dValveNo );
	if ( pNode != NULL )
	{
		// X좌표 수정
		dx = GetDlgItemInt( hdlg, IDC_LOC_X, &bResult, FALSE );
		if ( bResult )	pNode->dx	=	dx;
		else			pNode->dx	=	0;

		// Y좌표 수정
		dy = GetDlgItemInt( hdlg, IDC_LOC_Y, &bResult, FALSE );
		if ( bResult )	pNode->dy	=	dy;
		else			pNode->dy	=	0;

		// 이미지 수정
		dImgNo = GetDlgItemInt( hdlg, IDC_IMAGE_NO, &bResult, FALSE );
		if ( bResult ) pNode->dImgNo	=	dImgNo;

		// Valve 이름 수정
		if ( GetDlgItemText( hdlg, IDC_VALVE_NAME, szValveName, 19 ) )
		{
			strcpy( pNode->szValveName, szValveName );
		}

		Adjust_Set_Old_Value( hdlg );
		InvalidateRect( hParents, NULL, TRUE );

	}

	Adjust_Info_Set( hdlg );

}

void OnAdjMsgSave( HWND hdlg )
{
	char		szGDFPath[MAX_PATH];
	HWND		hSaveBut	=	NULL;
	HWND		hApplyBut	=	NULL;


	IMG_NODE*	pNode	=	NULL;

	FILE* fi;

	Make_GDF_Path( szGDFPath );
	
	fi = fopen( szGDFPath, "wt" );

	fprintf( fi, "%s\n", GDF_CONFIG_BEGIN);
	if ( g_bGuiEdit )		fprintf( fi, "EDIT\t\tON\n");
	else					fprintf( fi, "EDIT\t\tOFF\n");
	fprintf( fi, "IMAGE_DLL\t\t%s\n", g_szDLLPath );
	fprintf( fi, "%s\n", GDF_CONFIG_END);

	fprintf( fi, "%s\n", GDF_LOCATION_BEGIN);

	pNode = pStartNode;


	while( pNode != NULL )
	{
		fprintf( fi, "%-20s\t", pNode->szValveName );
		fprintf( fi, "%-3d\t",	pNode->dValNo );
		fprintf( fi, "%-3d\t",	pNode->dImgNo );
		fprintf( fi, "%5d\t",	pNode->dx );
		fprintf( fi, "%5d\n",	pNode->dy );
		pNode = pNode->NextNode;
	}
	

	fprintf( fi, "%s\n", GDF_LOCATION_END);

	fclose( fi );

	Adjust_Set_Old_Value( hdlg );
	g_bEdited = FALSE;

	hSaveBut	= GetDlgItem( hdlg, IDC_SAVE );
	hApplyBut	= GetDlgItem( hdlg, IDC_APPLY );

	EnableWindow( hSaveBut, g_bEdited );
	EnableWindow( hApplyBut, g_bApply );


	Adjust_Info_Set( hdlg );
}

void OnAdjMsgCancel( HWND hdlg )
{
	g_bGuideLine	=	FALSE;
	GDF_FILE_MODIFY_OFF;

	Adjust_Restore_Old_Value( hdlg );
	g_bSpinCkeckStart	=	FALSE;
	EndDialog( hdlg, 0 );
}

void Initial_Value_Set( HWND hdlg )
{
	int		dx, dy;
	int		dImgNo;
	char	szValveName[20];
	char	szValv_RealName[50] = "";

	IMG_NODE*	pNode	=	NULL;

//	SetDlgItemInt( hdlg, IDC_VAL_NO, g_dValveNo, FALSE );		// Valve Number Set

	sprintf( szValv_RealName, "Valve %s[%d]", g_szValveName[g_dValveNo - 1] , g_dValveNo );
	SetDlgItemText( hdlg, IDC_VAL_NO, szValv_RealName );


	pNode = Find_Img_Node( g_dValveNo );
	
	if ( pNode == NULL )
	{
		dx		=	0;
		dy		=	0;
		dImgNo	=	0;
		strcpy( szValveName, "Valve 1" );
	}
	else
	{
		dx = pNode->dx;
		dy = pNode->dy;
		dImgNo = pNode->dImgNo;
		strcpy( szValveName, pNode->szValveName );
	}

	g_OldValveNo	=	g_dValveNo;
	g_Old_X			=	dx;
	g_Old_Y			=	dy;
}

void Draw_Bitmap( HWND hdlg, char* pszImgName, int x, int y, int limitx, int limity )
{
	int				dWidth;
	int				dHeight;
	
	RECT			rc;

	HDC				hDC, hMemDC;
	HBITMAP			hBitmap, hOldBitmap;
	BITMAP			BitMapInfo;

	hDC = GetDC( hdlg );
	hMemDC	=	CreateCompatibleDC( hDC );

	if ( g_hDllInstance == NULL )	GUI_Main_Lib_Load();

	hBitmap	=	LoadBitmap( g_hDllInstance, pszImgName );

	rc.left	=	1;
	rc.top	=	1;
	rc.right	=	limitx - 1;
	rc.bottom	=	limity - 1;

	FillRect( hDC, &rc, (HBRUSH) GetSysColor( LTGRAY_BRUSH ) );

	if ( hBitmap == NULL )
	{
		DeleteDC( hMemDC );
		ReleaseDC( hdlg, hDC );
		// printf("[Draw Bitmap] Bitmap Load Error \n");
		g_bImageLoadSuccess = FALSE;
		return;
	}
	else
	{
		g_bImageLoadSuccess = TRUE;
	}

	
	hOldBitmap = (HBITMAP) SelectObject( hMemDC, hBitmap );
	GetObject( hBitmap, sizeof(BITMAP), &BitMapInfo );

	dWidth	=	BitMapInfo.bmWidth + 1;
	dHeight	=	BitMapInfo.bmHeight + 1;

	if ( dWidth > limitx )		dWidth = limitx - 1;
	if ( dHeight > limity )		dHeight = limity - 1;

	BitBlt( hDC, 1, 1, dWidth, dHeight, hMemDC, 0, 0, SRCCOPY );
	SelectObject( hMemDC, hOldBitmap );
	DeleteObject( hBitmap );

	DeleteDC( hMemDC );
	ReleaseDC( hdlg, hDC );
}

void Draw_Item_Preview( HWND hdlg, int dValveNo, int dStatus )
{
	int		dlm_Width, dlm_Height;
	int		dStartXPos, dStartYPos;
	char	szImgName[15]	=	"";

	HWND	hImgCtrl;
	RECT	rc;

	IMG_NODE*	pNode = NULL;

	hImgCtrl = GetDlgItem( hdlg, IDC_IMG_BORDER );

	GetClientRect( hImgCtrl, &rc );

	// Image Size Limit
	dlm_Width	= rc.right - rc.left;
	dlm_Height	= rc.bottom - rc.top;

	dStartXPos	= rc.left;
	dStartYPos	= rc.top;

	pNode = Find_Img_Node( dValveNo );
	
	if ( pNode == NULL )
	{
		printf("Image Adjust : Data Node Find Error\n");
		return;
	}

	
	sprintf( szImgName, "IMG%03d%03d", pNode->dImgNo, g_dStatus );
	Draw_Bitmap( hImgCtrl, szImgName, dStartXPos, dStartYPos, dlm_Width, dlm_Height );
}

void OnDecStatus( HWND hdlg )
{
	if ( g_dStatus == 0 )				return;

	g_dStatus--;
	Adjust_Info_Set( hdlg );
}

void OnIncStatus( HWND hdlg )
{
	if ( g_dStatus == VAL_STATUS_MAX )	return;

	g_dStatus++;
	Adjust_Info_Set( hdlg );
}

void OnPrevValve( HWND hdlg )
{
	HWND		hParent	=	NULL;
	HWND		hPrevButton;
	IMG_NODE*	pNode;
	
	hParent	= GetParent( hdlg );
	hPrevButton = GetDlgItem( hdlg , IDC_VAL_PREV );
	pNode = Find_Img_Node( g_dValveNo );
	if ( pNode == NULL ) return ;

	if ( ( pNode->PrevNode ) == NULL )
	{
		// 이전 노드가 없는 경우
		EnableWindow( hPrevButton, FALSE );
	}
	else
	{
		pNode = pNode->PrevNode;

		g_dValveNo	= ( pNode->dValNo );
		g_dStatus	=	0;

		Adjust_Set_Old_Value( hdlg );
		Adjust_Info_Set( hdlg );
		InvalidateRect( hParent, NULL, TRUE );
	}


}

void OnNextValve( HWND hdlg )
{
	HWND		hParent	=	NULL;
	HWND		hNextButton;
	IMG_NODE*	pNode;

	hParent	= GetParent( hdlg );
	hNextButton = GetDlgItem( hdlg , IDC_VAL_NEXT );
	pNode = Find_Img_Node( g_dValveNo );

	if ( pNode == NULL ) return ;

	if ( ( pNode->NextNode ) == NULL )
	{
		// 이전 노드가 없는 경우
		EnableWindow( hNextButton, FALSE );
	}
	else
	{
		pNode = pNode->NextNode;

		g_dValveNo	= ( pNode->dValNo );
		g_dStatus	=	0;

		Adjust_Set_Old_Value( hdlg );
		Adjust_Info_Set( hdlg );

		InvalidateRect( hParent, NULL, TRUE );
	}
}

void OnImgNext( HWND hdlg )
{
	HWND		hParent	=	NULL;
	IMG_NODE*	pNode	=	NULL;

	GDF_FILE_MODIFY_ON;
	pNode = Find_Img_Node( g_dValveNo );

	if ( pNode == NULL ) return ;

	pNode->dImgNo++;

	Adjust_Info_Set( hdlg );

	hParent = GetParent( hdlg );
	InvalidateRect( hParent, NULL, TRUE );

}

void OnImgPrev( HWND hdlg )
{
	IMG_NODE*	pNode	=	NULL;

	GDF_FILE_MODIFY_ON;
	pNode = Find_Img_Node( g_dValveNo );
	if ( pNode == NULL ) return ;

	pNode->dImgNo--;

	Adjust_Info_Set( hdlg );
}

LRESULT APIENTRY Edit_MonitorProc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam )
{
	HWND	hSaveButton;
	HWND	hApplyButton;

	hSaveButton		= GetDlgItem( g_hDlg, IDC_SAVE );
	hApplyButton	= GetDlgItem( g_hDlg, IDC_APPLY );


	switch ( msg )
	{
		case WM_KEYDOWN:
			 GDF_FILE_MODIFY_ON;	
			 EnableWindow( hSaveButton, TRUE );
			 EnableWindow( hApplyButton, TRUE );
			 break;
	}

	return CallWindowProc( Default_Message_Proc, hdlg, msg, wParam, lParam );
}

void Edit_Monitoring_Set( HWND hdlg )
{
	HWND	hCtrl_X, hCtrl_Y, hCtrl_Name;

	hCtrl_X		=	GetDlgItem( hdlg, IDC_LOC_X );
	hCtrl_Y		=	GetDlgItem( hdlg, IDC_LOC_Y );
	hCtrl_Name	=	GetDlgItem( hdlg, IDC_VALVE_NAME );

	Default_Message_Proc = (WNDPROC) SetWindowLong( hCtrl_X,    GWL_WNDPROC, (LONG) Edit_MonitorProc );
	Default_Message_Proc = (WNDPROC) SetWindowLong( hCtrl_Y,    GWL_WNDPROC, (LONG) Edit_MonitorProc );
	Default_Message_Proc = (WNDPROC) SetWindowLong( hCtrl_Name, GWL_WNDPROC, (LONG) Edit_MonitorProc );
}


void SpinControl_Set( HWND hdlg )
{
	int			dLimitX	=	0;
	int			dLimitY	=	0;

	DWORD		dwRange;

	HWND		hSpin	=	NULL;
	HWND		hSpin2	=	NULL;
	HWND		hEdit1	=	NULL;
	HWND		hEdit2	=	NULL;
	HWND		hParent	=	NULL;

	RECT		rc;

	hParent		=	GetParent( hdlg );

	GetWindowRect( hParent, &rc );


	 hSpin	=	GetDlgItem( hdlg, IDC_SPIN1 );
	 hSpin2 =	GetDlgItem( hdlg, IDC_SPIN2 );

	 hEdit1	=	GetDlgItem( hdlg, IDC_LOC_X );
	 hEdit2	=	GetDlgItem( hdlg, IDC_LOC_Y );

	 // 한계치 설정
	 dLimitX = rc.right - rc.left;
	 dLimitY = rc.bottom - rc.top;


	 // Range 설정
	 dwRange =  MAKELONG( (short) dLimitX, 0 );
	 SendMessage( hSpin, UDM_SETRANGE, 0, (LPARAM) dwRange );

 	 dwRange =  MAKELONG( (short) dLimitX, 0 );
	 SendMessage( hSpin2, UDM_SETRANGE, 0, (LPARAM) dwRange );

}

BOOL Get_Image_Size( IMG_NODE* pNode, int* dWidth, int* dHeight )
{
	char	szImageName[ 15 ] = "";

	HBITMAP		hBitmap		=	NULL;
	BITMAP		BitMapInfo;

	sprintf( szImageName, "IMG%03d%03d", pNode->dImgNo, g_dStatus );
	
	hBitmap	=	LoadBitmap( g_hDllInstance, szImageName );
	if ( hBitmap == NULL ) return FALSE;

	GetObject( hBitmap, sizeof(BITMAP), &BitMapInfo );
	
	*dWidth		= BitMapInfo.bmWidth;
	*dHeight	= BitMapInfo.bmHeight;

	DeleteObject(hBitmap);	// 2006.03.23
	return TRUE;
}

void On_Chg_Position( HWND hdlg, WPARAM wParam, LPARAM lParam )
{
	int			x, y;
	int			old_x, old_y;
	int			dWidth, dHeight;
	int			dStart_X, dStart_Y;

	RECT		rc;
	BOOL		bTranslate;

	HWND		hSaveBut	=	NULL;
	HWND		hApplyBut	=	NULL;
	HWND		hParent		=	NULL;
	IMG_NODE*	pNode		=	NULL;

	hParent	=	GetParent( hdlg );

	switch ( HIWORD( wParam ) )
	{
		case EN_CHANGE :

						if ( g_bSpinCkeckStart )
						{
							pNode = Find_Img_Node( g_dValveNo );

							if ( pNode == NULL ) return;

							 x = (int) GetDlgItemInt( hdlg, IDC_LOC_X, &bTranslate, TRUE );
							 y = (int) GetDlgItemInt( hdlg, IDC_LOC_Y, &bTranslate, TRUE );

							 old_x = pNode->dx-5;
							 old_y = pNode->dy-5;

							 pNode->dx	=	x;
							 pNode->dy	=	y;

							 if ( Get_Image_Size( pNode, &dWidth, &dHeight ) )
							 {
								 dStart_X	= old_x - 50;
								 dStart_Y	= old_y - 50;
								 rc.left	= ( dStart_X < 0 ) ? 0 : dStart_X;
								 rc.top		= ( dStart_Y < 0 ) ? 0 : dStart_Y;
								 rc.right	= x + dWidth + 5;
								 rc.bottom	= y + dHeight + 5;
								 InvalidateRect( hParent, &rc, TRUE );
							 }
							 else
							 {
								InvalidateRect( hParent, NULL, TRUE );
							 }

							 hSaveBut	= GetDlgItem( hdlg, IDC_SAVE );
							 hApplyBut	= GetDlgItem( hdlg, IDC_APPLY );

							 EnableWindow( hSaveBut, TRUE );
							 EnableWindow( hApplyBut, TRUE );
						}
						 break;
		case WM_CHAR :
						InvalidateRect( hParent, NULL, TRUE );
						break;
	}

	return ;
}


BOOL APIENTRY Adjust_Msg_Handler( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam )
{
	switch( msg )
	{
		case WM_CREATE :
			 return 0;

		case WM_INITDIALOG :
			 g_hDlg			=	hdlg;
			 g_bGuideLine	=	TRUE;

			 Initial_Value_Set( hdlg );
			 SpinControl_Set( hdlg );
			 Adjust_Info_Set( hdlg );
			 g_bSpinCkeckStart	=	TRUE;
			 Draw_Item_Preview( hdlg, g_dValveNo, g_dStatus );
			 Edit_Monitoring_Set( hdlg );
			 GUI_LIB_All_Draw( GetParent( hdlg ) );

			 return 0;

		case WM_PAINT :
			 Draw_Item_Preview( hdlg, g_dValveNo, g_dStatus );
			 return 0;

		case WM_COMMAND :
			 switch( LOWORD( wParam  ) )
			 {
//				case IDC_SEARCH :		OnAdjMsgSearch( hdlg );	return 0;
				case IDOK		:		OnAdjMsgOK( hdlg );		return 0;
				case IDC_SAVE	:		OnAdjMsgSave( hdlg );	return 0;
				case IDCANCEL	:		OnAdjMsgCancel( hdlg );	return 0;
				case IDC_APPLY  :		OnAdjMsgApply( hdlg );	return 0;
				case IDC_IMG_STPREV :	OnDecStatus( hdlg );	return 0;
				case IDC_IMG_STNEXT :	OnIncStatus( hdlg );	return 0;
				case IDC_VAL_PREV :		OnPrevValve( hdlg );	return 0;
				case IDC_VAL_NEXT :		OnNextValve( hdlg );	return 0;
				case IDC_IMG_NEXT :		OnImgNext( hdlg );		return 0;
				case IDC_IMG_PREV :		OnImgPrev( hdlg );		return 0;
				case IDC_LOC_X :		On_Chg_Position( hdlg, wParam, lParam );		return 0;
				case IDC_LOC_Y :		On_Chg_Position( hdlg, wParam, lParam );		return 0;
			 }

			 return 0;
	}
	return 0;
}