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

IMG_NODE*	pStartNode			=	NULL;
HINSTANCE	g_hDllInstance		=	NULL;

BOOL		g_bGuiEdit			=	FALSE;


int			g_dGDF_Flag			=	-1;
int			g_dLineCount		=	0;

char		g_szDLLPath[MAX_PATH]	=	"";

extern		BOOL		g_bGuideLine;
extern		BOOL		g_bImageLoadSuccess;
extern		UINT		g_dValveNo;

extern		VAL_DATA	g_Det_Valve_Data;
extern		VAL_DATA	g_Det_Valve_Data_DMY;

extern BOOL g_bDMY;

BOOL STR_SEPERATE_BLANK( char* pszBuf, char* pszBuf1, char* pszBuf2, int nMax );
// File Function Group Start -------------------------------------------------------------

// 2004.08.23 by cskim, Modified
void Make_GDF_Path( char* szGDFPath )
{
	if(strlen(g_szModule) > 0)
		sprintf( szGDFPath, "%s/%s/%s", GDF_PATH, g_szModule, GDF_FILE );
	else sprintf( szGDFPath, "%s/%s", GDF_PATH, GDF_FILE );
	return;
}

// File Function Group End ---------------------------------------------------------------


// Img Library Group Start----------------------------------------------------------------
void GUI_Lib_Load( char* szDllPath )
{
	g_hDllInstance	= LoadLibrary( szDllPath );

	if ( g_hDllInstance != NULL )		printf("Success\n");
	else								printf("Failure\n");

}

void GUI_Main_Lib_Load( void )
{
	g_hDllInstance	= LoadLibrary( g_szDLLPath );

	if ( g_hDllInstance != NULL )		printf("Success\n");
	else								printf("Failure\n");
}

void GUI_Lib_Free( void )
{
	FreeLibrary( g_hDllInstance );
}
// Img Library Group End------------------------------------------------------------------

// GDF Data Structure Relate Function Start ----------------------------------------------
void Init_Img_List( void )
{
	pStartNode->NextNode	=	NULL;
	pStartNode->PrevNode	=	NULL;
}

IMG_NODE* Find_Img_Node( int dValNo )
{
	IMG_NODE* pNode		=	NULL;

	pNode = pStartNode;

	while ( pNode != NULL )
	{
		if (( pNode -> dValNo ) == dValNo )
		{
			return pNode;
		}

		pNode = pNode->NextNode;
	}

	return NULL;
}

IMG_NODE* Find_Img_Last_Node( void )
{			 
	int			dCount		=	0;
	IMG_NODE*	pNode		=	NULL;

	pNode = pStartNode;

	while ( (pNode->NextNode) != NULL )
	{
		pNode = pNode->NextNode;
	}

	return pNode;
}

BOOL Add_Img_Node( char* szValveName, int dValNo, int dImgNo, int dx, int dy )
{
	IMG_NODE*	pNode	=	NULL;


	if ( Find_Img_Node( dValNo ) != NULL )		return FALSE;

	pNode	=	malloc( sizeof( IMG_NODE ) );

	if ( pNode == NULL )
	{
		printf("Image List Memory Allocation Error\n" );
		return FALSE;
	}

	pNode->dImgNo	=	dImgNo;
	pNode->dValNo	=	dValNo;
	pNode->dx		=	dx;
	pNode->dy		=	dy;
	strcpy( pNode->szValveName, szValveName );


	if ( pStartNode == NULL )
	{
		pNode->NextNode	=	NULL;
		pNode->PrevNode	=	NULL;
		pStartNode		=	pNode;
	}
	else
	{
		IMG_NODE*	pLastNode	=	NULL;

		pLastNode = Find_Img_Last_Node();
		
		pLastNode->NextNode	=	pNode;
		pNode->PrevNode		=	pLastNode;
		pNode->NextNode		=	NULL;
	}

	return TRUE;
}

BOOL Remove_Img_Node( int dValNo )
{
	IMG_NODE* pNode			=	NULL;
	IMG_NODE* pPrevNode		=	NULL;

	pNode = Find_Img_Node( dValNo );

	if ( pNode != NULL )
	{
		pPrevNode			= pNode->PrevNode;
		pPrevNode->NextNode	= pNode->NextNode;

		// 2004.09.09 by cskim
		pNode->NextNode->PrevNode = pPrevNode;

		free( pNode );
		return TRUE;
	}

	return FALSE;
}

void Remove_All_Img_Node( void )
{
	int		dCount			=	0;

	IMG_NODE* pNode			=	NULL;
	IMG_NODE* pNextNode		=	NULL;

	pNode = pStartNode;

	while ( pNode != NULL )
	{
		pNextNode = pNode->NextNode;
		free( pNode );
		pNode = pNextNode;
	}

	pStartNode = NULL;	
}

	// For Debugging 
void Print_All_Image_Buffer_Define( void )
{
	IMG_NODE*	pNode	=	pStartNode;

	while ( pNode != NULL)		pNode = pNode->NextNode;
}

// GDF Data Structure Relate Function End ------------------------------------------------

// GDF Statement Analysis Group Start ----------------------------------------------------
BOOL GUI_Define_Analysis( char* szBuf )
{
	int		dImgNo		=	0;
	int		dx			=	0;
	int		dy			=	0;
	int		dValNo		=	0;

	char	szValveName[20]	="";

	UINT	uCounter	=	0;

	char	szCmd[50]			=	"";
	char	szParam[MAX_PATH]	=	"";
	char	szCopyBuf[512];

	char*	szSeperator =	"\t ";
	char*	pszBuf		=	NULL;

	BOOL	bResult		=	FALSE;

	// 주석행 처리
	if (( szBuf[0] == '#' )  || ( szBuf[0] == '\n' ) || ( szBuf[0] == '\t' ) || ( szBuf[0] == ' ' ))	return TRUE;
	if (!strlen( szBuf ))		return TRUE;

	strcpy( szCopyBuf, szBuf );

	if		( !stricmp( szCopyBuf, GDF_CONFIG_BEGIN   ) )  { g_dGDF_Flag =  0;    return TRUE; }
	else if ( !stricmp( szCopyBuf, GDF_CONFIG_END     ) )  { g_dGDF_Flag = -1;    return TRUE; }
	else if ( !stricmp( szCopyBuf, GDF_LOCATION_BEGIN ) )  { g_dGDF_Flag =  1;    return TRUE; }
	else if ( !stricmp( szCopyBuf, GDF_LOCATION_END   ) )  { g_dGDF_Flag = -1;    return TRUE; }

	if		( g_dGDF_Flag == 0 ) 
	{
		// Data Parsing 
		STR_SEPERATE_BLANK( szCopyBuf, szCmd, szParam, MAX_PATH );

		// Data Store 
		if		( !stricmp( szCmd, "EDIT" ) )
		{
			if ( !stricmp( szParam, "ON" ) )	g_bGuiEdit		=	TRUE;
			else								g_bGuiEdit		=	FALSE;
		}
		else if ( !stricmp( szCmd, "IMAGE_DLL" ) )		strcpy( g_szDLLPath, szParam );
		else bResult = FALSE;
	
		return TRUE;
	}
	else if ( g_dGDF_Flag == 1 )
	{

		// Parsing Data Part
		pszBuf = strtok( szCopyBuf, szSeperator );

		while ( pszBuf != NULL )
		{
			uCounter++;

			if ( strcmp( pszBuf, "-" ) )
			{
				if		( uCounter == 1 )	strcpy( szValveName, pszBuf );
				else if ( uCounter == 2 )	dValNo	=	atoi( pszBuf );
				else if ( uCounter == 3 )	dImgNo	=	atoi( pszBuf );	
				else if ( uCounter == 4 )	dx		=	atoi( pszBuf );
				else if ( uCounter == 5 )	dy		=	atoi( pszBuf );
			}

			pszBuf = strtok( NULL, szSeperator );
		}

		bResult = Add_Img_Node( szValveName, dValNo, dImgNo, dx, dy );
	}

	return bResult;
}

BOOL GUI_Read_GDF( void )
{
	char	ch;
	char	szGDFPath[ MAX_PATH ];
	char	szLineBuf[ 512 ];

	int		dRead_Char_Count	=	0;

	FILE*	pFi	=	NULL;

	Make_GDF_Path( szGDFPath );
//	GUI_GDF_File_Open( szGDFPath, pFi );

	pFi = fopen( szGDFPath, "rt" );

	if ( pFi == NULL )
	{
		printf("File Pointer Error\n");
		return FALSE;
	}

	g_dLineCount	=	0;

	while ( (ch = fgetc( pFi )) != EOF )
	{
		if ( ch == '\n' )
		{
			g_dLineCount++;								// Increase Line Counter
			szLineBuf[ dRead_Char_Count++ ] = '\0';		// Write String End Char

			// Process Define Data
			if ( !GUI_Define_Analysis( szLineBuf ) )
				printf("[Line : %d] Valve Image Define Error\n", g_dLineCount );		// Line Define Error

			// Reset Character order!!
			dRead_Char_Count	=	0;
		}
		else
		{
			// Add Char to Line Buffer
			szLineBuf[ dRead_Char_Count++ ] = ch;
		}
	}

	if ( dRead_Char_Count )
	{
		    // When meet the End of file but buffer has Char
			szLineBuf[ dRead_Char_Count++ ] = '\0';
			GUI_Define_Analysis( szLineBuf );			// Buffer Data Process
	}

	fclose( pFi );
//	GUI_GDF_File_Close( pFi );

	Print_All_Image_Buffer_Define();

	return TRUE;
}
// GDF Statement Analysis Group End ----------------------------------------------------

// GUI Image Draw Function Start -------------------------------------------------------
void GUI_DRAW_Image_Lib( HWND hdlg, int dValveNo, int dStatus, BOOL bGuideLine )
{
	int				dx,	dy;
	int				dWidth, dHeight;
	char			szBitmapName[15]	=	"";
	RECT			rc;
	
	HDC				hDC, hMemDC;
	HBITMAP			hBitmap, hOldBitmap;
	BITMAP			BitMapInfo;
	HBRUSH			hOldBrush,	hTransParentBrush;
	HPEN			hOldPen,	hGuideLinePen;

	IMG_NODE*		pNode	=	NULL;
	
	pNode = Find_Img_Node( dValveNo );
	
	// Get Bitmap Pos;
	dx = pNode->dx;
	dy = pNode->dy;
	
	// Make Bitmap Name
	sprintf( szBitmapName,"IMG%03d%03d", pNode->dImgNo, dStatus );

	if ( g_hDllInstance == NULL )	GUI_Lib_Load( g_szDLLPath );
	
	hDC		=	GetDC( hdlg );
	hMemDC	=	CreateCompatibleDC( hDC );


	hBitmap	=	LoadBitmap( g_hDllInstance, szBitmapName );
//	if ( hBitmap == NULL )	printf("Bitmap Load Error\n");

	hOldBitmap = (HBITMAP) SelectObject( hMemDC, hBitmap );
	GetObject( hBitmap, sizeof(BITMAP), &BitMapInfo );

	dWidth	=	BitMapInfo.bmWidth;
	dHeight	=	BitMapInfo.bmHeight;

	BitBlt( hDC, dx, dy, dWidth, dHeight, hMemDC, 0, 0, SRCCOPY );

	if ( bGuideLine )
	{
		hTransParentBrush	= (HBRUSH) GetStockObject( NULL_BRUSH );
		hGuideLinePen		= CreatePen( PS_SOLID, 2, GUIDE_LINE_COLOR );

		hOldBrush	= (HBRUSH) SelectObject( hDC, hTransParentBrush ) ;
		hOldPen		= (HPEN) SelectObject( hDC, hGuideLinePen ) ;


		rc.left	=	dx - 1;
		rc.top	=	dy - 1;
		rc.right	=	dx + dWidth + 2;
		rc.bottom	=	dy + dHeight + 1;
		Rectangle( hDC, rc.left, rc.top, rc.right, rc.bottom );

		SelectObject( hDC, hOldBrush );
		SelectObject( hDC, hOldPen );
		DeleteObject( hTransParentBrush );
		DeleteObject( hGuideLinePen );
	}


	SelectObject( hMemDC, hOldBitmap );
	DeleteObject( hBitmap );

	DeleteDC( hMemDC );
	ReleaseDC( hdlg, hDC );
}

void GUI_LIB_All_Draw( HWND hdlg )
{
	int			dValveNo	=	1;
	int			dStatus		=	0;
	
	IMG_NODE*	pNode	=	pStartNode;

	while ( pNode != NULL)
	{
		dValveNo	= (pNode->dValNo) - 1;
		if(g_bDMY)
			dStatus		= g_Det_Valve_Data_DMY.RCP_DATA[ dValveNo ];
		else
			dStatus		= g_Det_Valve_Data.RCP_DATA[ dValveNo ];

		if ( ((pNode->dValNo)  == ((int) g_dValveNo) ) && ( g_bGuideLine ) && ( g_bImageLoadSuccess) ) GUI_DRAW_Image_Lib( hdlg, pNode->dValNo, dStatus, TRUE );
		else														GUI_DRAW_Image_Lib( hdlg, pNode->dValNo, dStatus, FALSE );
		pNode = pNode->NextNode;
	}
}
// GUI Image Draw Function End ---------------------------------------------------------


// GUI Operation Function Start --------------------------------------------------------
// X, Y 좌표를 가지고 벨브 넘버를 구하는 펑션
BOOL Get_Item_No( int dx, int dy, int* dValveNo )
{
	int				dStatus	=	0;
	char			szBitmapName[ 15 ];

	RECT			rc;

	HBITMAP			hBitmap	=	NULL;
	BITMAP			BitMapInfo;

	IMG_NODE*		pNode	=	NULL;

	pNode	=	pStartNode;

	while ( pNode != NULL )
	{
		hBitmap	=	NULL;

		for ( dStatus = 0 ; dStatus <= VAL_STATUS_MAX ; dStatus++ )
		{

			sprintf( szBitmapName, "IMG%03d%03d", pNode->dImgNo, dStatus );
			hBitmap = LoadBitmap( g_hDllInstance, szBitmapName );

			if ( hBitmap != NULL )	break;
			// DeleteObject(hBitmap);	// 2006.03.23
		}

		GetObject( hBitmap, sizeof(BITMAP), &BitMapInfo );
		DeleteObject(hBitmap);	// 2006.03.23

		rc.left = pNode->dx;
		rc.top	= pNode->dy;
		rc.right = rc.left + BitMapInfo.bmWidth;
		rc.bottom = rc.top + BitMapInfo.bmHeight;

		if  (( ( dx >= rc.left  ) && ( dx <= rc.right ) ) &&
			 ( ( dy >= rc.top   ) && ( dy <= rc.bottom ) ) )
		{
			*dValveNo	=	pNode->dValNo;
			return TRUE;
		}

		pNode = pNode->NextNode;
	}


	*dValveNo	=	-1;
	return FALSE;
}
// GUI Operation Function Start --------------------------------------------------------
