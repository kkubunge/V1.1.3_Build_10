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

#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>

#include "cimseqnc.h"
#include "iodefine.h"

#include "resource.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int MATCH_CONTROL;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void	*R_IOP[ MAX_DRIVER_TABLE ];
int		DRIVER_CNT = 0;
//------------------------------------------------------------------------------------------
void	*D_IOP[ MAX_DIGITAL_TABLE ];
int		DIGITAL_CNT	= 0;

void	*A_IOP[ MAX_ANALOG_TABLE ];
int		ANALOG_CNT	= 0;

void	*S_IOP[ MAX_STRING_TABLE ];
int		STRING_CNT	= 0;

BOOL FIND_DATA_INITIALIZE = FALSE;
BOOL FIND_DRIVER_INITIALIZE = FALSE;

int D_SELECT_COUNT = 0;
int A_SELECT_COUNT = 0;
int S_SELECT_COUNT = 0;
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int DIGITAL_TOTAL_COUNT() {
	return DIGITAL_CNT;
}
int ANALOG_TOTAL_COUNT() {
	return ANALOG_CNT;
}
int STRING_TOTAL_COUNT() {
	return STRING_CNT;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int DIGITAL_SELECT_COUNT() {
	return D_SELECT_COUNT;
}
int ANALOG_SELECT_COUNT() {
	return A_SELECT_COUNT;
}
int STRING_SELECT_COUNT() {
	return S_SELECT_COUNT;
}
//---------------------------------------------------------------------------------------
char COMPARE_STRINGDATA[3][256];
int  COMPARE_ANDOR[3] = { 0 , 0 , 0 };
int  COMPARE_DRIVER_IN[3] = { 0 , 0 , 0 };
int  COMPARE_DRIVER_OUT[3] = { 0 , 0 , 0 };
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int DRIVER_NAME_INSERT( char *Buffer ) {
	int i;
	if ( DRIVER_CNT == 0 ) {
		R_IOP[ DRIVER_CNT ] = malloc( RIO_TOTAL );
		if ( R_IOP[ DRIVER_CNT ] == NULL ) {
			_IO_CIM_PRINTF( "Allocate memory for Driver Table Error\n" );
			return 0;
		}
		strcpy( R_IO_NAME( DRIVER_CNT ) , "ALL DRIVER" );
		DRIVER_CNT++;
	}
	for ( i = 0 ; i < DRIVER_CNT ; i++ ) {
		if ( strcmp( R_IO_NAME( i )	, Buffer ) == 0 ) {
			return i;
		}
	}
	R_IOP[ DRIVER_CNT ] = malloc( RIO_TOTAL );
	if ( R_IOP[ DRIVER_CNT ] == NULL ) {
		_IO_CIM_PRINTF( "Allocate memory for Driver Table Error\n" );
		return 0;
	}
	strncpy( R_IO_NAME( DRIVER_CNT ) , Buffer , MAX_CHANNEL_NAME_SIZE );
	DRIVER_CNT++;
	return (DRIVER_CNT - 1);
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int  READ_IO_WINDOW_POINT;

BOOL APIENTRY READ_IO_WINDOW_PROC( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static UINT uTimer;
	switch ( msg ) {
		case WM_INITDIALOG:
			MoveCenterWindow( hdlg );
			uTimer = SetTimer( hdlg , 1000 , 250 , NULL );
			return TRUE;
		case WM_TIMER :
			if ( READ_IO_WINDOW_POINT == 0 ) EndDialog( hdlg , 0 );
			return TRUE;
		case WM_COMMAND:
            switch ( LOWORD( wParam ) ) {
				case IDCANCEL:
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			break;
		case WM_DESTROY:
			if ( uTimer ) KillTimer( hdlg , uTimer );
			return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void READ_IO_INFORMATION_FOR_FINDING_THREAD( int mode ) {
	char Buffer[256];
	char BufferI[256];
	char BufferO[256];
	int i;
	for ( i = 0 ; i < DIGITAL_CNT ; i++ ) {
		if ( mode == 0 ) {
			if ( D_IO_DRVREAD( i )	) continue;
			D_IO_DRVREAD( i ) = TRUE;
			_dDIGITAL_DRIVER_NAME( i , BufferI , BufferO );
			D_IO_IN_DRIVER( i ) = DRIVER_NAME_INSERT( BufferI );
			D_IO_OUT_DRIVER( i ) = DRIVER_NAME_INSERT( BufferO );
		}
		else {
			if ( D_IO_NAMEREAD( i )	) continue;
			D_IO_NAMEREAD( i ) = TRUE;
			_dDIGITAL_NAME( i , Buffer );
			strncpy( D_IO_NAME( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			strncpy( D_IO_NAMEL( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			STRCONV_L( D_IO_NAMEL( i ) );	
		}
	}
	//
	for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
		if ( mode == 0 ) {
			if ( A_IO_DRVREAD( i )	) continue;
			A_IO_DRVREAD( i ) = TRUE;
			_dANALOG_DRIVER_NAME( i , BufferI , BufferO );
			A_IO_IN_DRIVER( i ) = DRIVER_NAME_INSERT( BufferI );
			A_IO_OUT_DRIVER( i ) = DRIVER_NAME_INSERT( BufferO );
		}
		else {
			if ( A_IO_NAMEREAD( i )	) continue;
			A_IO_NAMEREAD( i ) = TRUE;
			_dANALOG_NAME( i , Buffer );
			strncpy( A_IO_NAME( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			strncpy( A_IO_NAMEL( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			STRCONV_L( A_IO_NAMEL( i ) );	
		}
	}
	//
	for ( i = 0 ; i < STRING_CNT ; i++ ) {
		if ( mode == 0 ) {
			if ( S_IO_DRVREAD( i )	) continue;
			S_IO_DRVREAD( i ) = TRUE;
			_dSTRING_DRIVER_NAME( i , BufferI , BufferO );
			S_IO_IN_DRIVER( i ) = DRIVER_NAME_INSERT( BufferI );
			S_IO_OUT_DRIVER( i ) = DRIVER_NAME_INSERT( BufferO );
		}
		else {
			if ( S_IO_NAMEREAD( i )	) continue;
			S_IO_NAMEREAD( i ) = TRUE;
			_dSTRING_NAME( i , Buffer );
			strncpy( S_IO_NAME( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			strncpy( S_IO_NAMEL( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			STRCONV_L( S_IO_NAMEL( i ) );	
		}
	}
	READ_IO_WINDOW_POINT = 0;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void READ_IO_INFORMATION_FOR_FINDING( HWND hWnd , int mode ) {
	if ( mode == 0 ) {
		if ( FIND_DRIVER_INITIALIZE ) return;
		FIND_DRIVER_INITIALIZE = TRUE;
	}
	else {
		if ( FIND_DATA_INITIALIZE ) return;
		FIND_DATA_INITIALIZE = TRUE;
	}
	READ_IO_WINDOW_POINT = 1;
	_beginthread( (void *) READ_IO_INFORMATION_FOR_FINDING_THREAD , 0 , (void *) mode );
	if ( READ_IO_WINDOW_POINT == 0 ) return;
	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CHECK_BOX ) , hWnd , READ_IO_WINDOW_PROC , (LPARAM) mode ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CHECK_BOX ) , hWnd , READ_IO_WINDOW_PROC , (LPARAM) mode ); // 2004.01.19 // 2004.08.10
//	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CHECK_BOX ) , hWnd , READ_IO_WINDOW_PROC , (LPARAM) mode ); // 2004.01.19
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CHECK_BOX ) , hWnd , READ_IO_WINDOW_PROC , (LPARAM) mode ); // 2002.06.17
	while ( READ_IO_WINDOW_POINT != 0 ) { _sleep(10); }
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void READ_IO_INFORMATION_FOR_DISPLAY_DATA( int mode , int i ) {
	char Buffer[256];
	switch( mode ) {
	case 1 :
		if ( !D_IO_NAMEREAD( i ) ) {
			D_IO_NAMEREAD( i ) = TRUE;
			_dDIGITAL_NAME( i , Buffer );
			strncpy( D_IO_NAME( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			strncpy( D_IO_NAMEL( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			STRCONV_L( D_IO_NAMEL( i ) );
		}
		if ( !D_IO_DATAREAD( i ) ) {
			D_IO_DATAREAD( i )  = TRUE;
			D_IO_TYPE( i )			=	_dDIGITAL_TYPE( i );
			D_IO_IN_VALUE( i )		= _dREAD_MEM_DIGITAL( i , Buffer );
			strncpy( D_IO_IN_VALUE_STR( i ), Buffer , MAX_VALUE_SIZE );
			D_IO_OUT_VALUE( i )		= _dREAD_SET_DIGITAL( i , Buffer );
			strncpy( D_IO_OUT_VALUE_STR( i ), Buffer , MAX_VALUE_SIZE );
		}
		break;

	case 2 :
		if ( !A_IO_NAMEREAD( i ) ) {
			A_IO_NAMEREAD( i ) = TRUE;
			_dANALOG_NAME( i , Buffer );
			strncpy( A_IO_NAME( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			strncpy( A_IO_NAMEL( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			STRCONV_L( A_IO_NAMEL( i ) );
		}
		if ( !A_IO_DATAREAD( i ) ) {
			A_IO_DATAREAD( i )  = TRUE;
			A_IO_TYPE( i )			=	_dANALOG_TYPE( i );
			A_IO_IN_VALUE( i )		= _dREAD_MEM_ANALOG( i , Buffer );
			strncpy( A_IO_IN_VALUE_STR( i ), Buffer , MAX_VALUE_SIZE );
			A_IO_OUT_VALUE( i )		= _dREAD_SET_ANALOG( i , Buffer );
			strncpy( A_IO_OUT_VALUE_STR( i ), Buffer , MAX_VALUE_SIZE );
		}
		break;

	case 3 :
		if ( !S_IO_NAMEREAD( i ) ) {
			S_IO_NAMEREAD( i ) = TRUE;
			_dSTRING_NAME( i , Buffer );
			strncpy( S_IO_NAME( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			strncpy( S_IO_NAMEL( i ) , Buffer , MAX_CHANNEL_NAME_SIZE );
			STRCONV_L( S_IO_NAMEL( i ) );	
		}
		if ( !S_IO_DATAREAD( i ) ) {
			S_IO_DATAREAD( i )  = TRUE;
			_dREAD_MEM_STRING( i , Buffer );
//			strncpy( S_IO_DATA( i ) , Buffer , MAX_STRING_SIZE ); // 2005.05.11
			strncpy( S_IO_DATA( i ) , Buffer , MAX_STRING_SIZE+MAX_STRING_SIZE+1 ); // 2005.05.11
		}
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL READ_IO_INFORMATION() {
	int i;

	strcpy( COMPARE_STRINGDATA[0] , "" );
	strcpy( COMPARE_STRINGDATA[1] , "" );
	strcpy( COMPARE_STRINGDATA[2] , "" );

	DIGITAL_CNT = _dDIGITAL_COUNT();
	ANALOG_CNT  = _dANALOG_COUNT();
	STRING_CNT  = _dSTRING_COUNT();

	D_SELECT_COUNT = DIGITAL_CNT;
	A_SELECT_COUNT = ANALOG_CNT;
	S_SELECT_COUNT = STRING_CNT;

	for ( i = 0 ; i < DIGITAL_CNT ; i++ ) {
		D_IOP[ i ] = malloc( DIO_TOTAL );
		if ( D_IOP[ i ] == NULL ) {
			_IO_CIM_PRINTF( "Allocate memory for Digital Table Error\n" );
			return FALSE;
		}
		//----------------------------------------------------------------
		D_IO_NAMEREAD( i )		=	FALSE;
		D_IO_DATAREAD( i )		=	FALSE;
		D_IO_DRVREAD( i )		=	FALSE;
		//
		D_IO_SELECT( i )		=	TRUE;
		//
		D_IO_TYPE( i )			=	OUTPUT_IO_TYPE;
		//----------------------------------------------------------------
	}
	//
	for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
		A_IOP[ i ] = malloc( AIO_TOTAL );
		if ( A_IOP[ i ] == NULL ) {
			_IO_CIM_PRINTF( "Allocate memory for Analog Table Error\n" );
			return FALSE;
		}
		//----------------------------------------------------------------
		A_IO_NAMEREAD( i )		=	FALSE;
		A_IO_DATAREAD( i )		=	FALSE;
		A_IO_DRVREAD( i )		=	FALSE;
		//
		A_IO_SELECT( i )		=	TRUE;
		//
		A_IO_TYPE( i )			=	OUTPUT_IO_TYPE;
		//----------------------------------------------------------------
	}
	//
	for ( i = 0 ; i < STRING_CNT ; i++ ) {
		S_IOP[ i ] = malloc( SIO_TOTAL );
		if ( S_IOP[ i ] == NULL ) {
			_IO_CIM_PRINTF( "Allocate memory for String Table Error\n" );
			return FALSE;
		}
		//----------------------------------------------------------------
		S_IO_NAMEREAD( i )		=	FALSE;
		S_IO_DATAREAD( i )		=	FALSE;
		S_IO_DRVREAD( i )		=	FALSE;
		//
		S_IO_SELECT( i )		=	TRUE;
		//----------------------------------------------------------------
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SEARCH_ANDOR_REMAPPING( HWND hdlg ) {
	if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 && ( strlen( COMPARE_STRINGDATA[0] ) > 0 ) ) {
		if ( COMPARE_ANDOR[0] == 0 )
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR1 , "OR" );
		else
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR1 , "AND" );
		if ( COMPARE_ANDOR[1] == 0 )
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR2 , "OR" );
		else
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR2 , "AND" );
	}
	else if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 ) {
		if ( COMPARE_ANDOR[0] == 0 )
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR1 , "OR" );
		else
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR1 , "AND" );
		KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_ANDOR2 );
	}
	else if ( COMPARE_DRIVER_OUT[0] > 0 && ( strlen( COMPARE_STRINGDATA[0] ) > 0 ) ) {
		KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_ANDOR1 );
		if ( COMPARE_ANDOR[1] == 0 )
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR2 , "OR" );
		else
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR2 , "AND" );
	}
	else if ( COMPARE_DRIVER_IN[0] > 0 && ( strlen( COMPARE_STRINGDATA[0] ) > 0 ) ) {
		KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_ANDOR1 );
		if ( COMPARE_ANDOR[1] == 0 )
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR2 , "OR" );
		else
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_ANDOR2 , "AND" );
	}
	else {
		KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_ANDOR1 );
		KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_ANDOR2 );
	}
}
//---------------------------------------------------------------------------------------
void SEARCH_SET_ANDOR( HWND hdlg , int mode , int btn ) {
	int iRes;
	iRes = COMPARE_ANDOR[mode];
	if ( MODAL_DIGITAL_BOX1( hdlg , "Search" , "Or And" , "OR|AND" , &iRes ) ) {
		COMPARE_ANDOR[mode] = iRes;
		//
		if ( COMPARE_ANDOR[mode] == 0 )
			KWIN_Item_String_Display( hdlg , btn , "OR" );
		else
			KWIN_Item_String_Display( hdlg , btn , "AND" );
		KWIN_Item_Enable( hdlg , IDB_SEARCH );
		KWIN_Item_Hide( hdlg , IDB_SEARCH_N );
	}
}
//---------------------------------------------------------------------------------------
void SEARCH_SET_DRIVER_IN( HWND hdlg , int mode , int btn ) {
	char Buffer[1024];
	int iRes , i;
	//====================================================
	READ_IO_INFORMATION_FOR_FINDING( hdlg , 0 );
	//====================================================
	iRes = COMPARE_DRIVER_IN[mode];
	strcpy( Buffer , "" );
	for ( i = 0 ; i < DRIVER_CNT ; i++ ) {
		if ( i != 0 ) strcat( Buffer , "|" );
		strcat( Buffer , R_IO_NAME( i ) );
	}
	if ( MODAL_DIGITAL_BOX1( hdlg , "Search" , "Driver" , Buffer , &iRes ) ) {
		COMPARE_DRIVER_IN[mode] = iRes;
		//
		if ( COMPARE_DRIVER_IN[0] <= 0 ) {
			COMPARE_DRIVER_IN[0] = COMPARE_DRIVER_IN[1];
			COMPARE_DRIVER_IN[1] = 0;
		}
		if ( COMPARE_DRIVER_IN[1] <= 0 ) {
			COMPARE_DRIVER_IN[1] = COMPARE_DRIVER_IN[2];
			COMPARE_DRIVER_IN[2] = 0;
		}

		if ( COMPARE_DRIVER_IN[0] > 0 )
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRI1 , R_IO_NAME( COMPARE_DRIVER_IN[0] ) );
		else
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRI1 , "" );
		if ( COMPARE_DRIVER_IN[0] > 0 ) {
			if ( COMPARE_DRIVER_IN[1] > 0 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRI2 , R_IO_NAME( COMPARE_DRIVER_IN[1] ) );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRI2 , "" );
		}
		else {
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRI2 );
		}
		if ( COMPARE_DRIVER_IN[1] > 0 ) {
			if ( COMPARE_DRIVER_IN[2] > 0 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRI3 , R_IO_NAME( COMPARE_DRIVER_IN[2] ) );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRI3 , "" );
		}
		else {
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRI3 );
		}
		KWIN_Item_Enable( hdlg , IDB_SEARCH );
		KWIN_Item_Hide( hdlg , IDB_SEARCH_N );
		SEARCH_ANDOR_REMAPPING( hdlg );
	}
}
//---------------------------------------------------------------------------------------
void SEARCH_SET_DRIVER_OUT( HWND hdlg , int mode , int btn ) {
	char Buffer[1024];
	int iRes , i;
	//====================================================
	READ_IO_INFORMATION_FOR_FINDING( hdlg , 0 );
	//====================================================
	iRes = COMPARE_DRIVER_OUT[mode];
	strcpy( Buffer , "" );
	for ( i = 0 ; i < DRIVER_CNT ; i++ ) {
		if ( i != 0 ) strcat( Buffer , "|" );
		strcat( Buffer , R_IO_NAME( i ) );
	}
	if ( MODAL_DIGITAL_BOX1( hdlg , "Search" , "Driver" , Buffer , &iRes ) ) {
		COMPARE_DRIVER_OUT[mode] = iRes;
		//
		if ( COMPARE_DRIVER_OUT[0] <= 0 ) {
			COMPARE_DRIVER_OUT[0] = COMPARE_DRIVER_OUT[1];
			COMPARE_DRIVER_OUT[1] = 0;
		}
		if ( COMPARE_DRIVER_OUT[1] <= 0 ) {
			COMPARE_DRIVER_OUT[1] = COMPARE_DRIVER_OUT[2];
			COMPARE_DRIVER_OUT[2] = 0;
		}

		if ( COMPARE_DRIVER_OUT[0] > 0 )
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRO1 , R_IO_NAME( COMPARE_DRIVER_OUT[0] ) );
		else
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRO1 , "" );
		if ( COMPARE_DRIVER_OUT[0] > 0 ) {
			if ( COMPARE_DRIVER_OUT[1] > 0 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRO2 , R_IO_NAME( COMPARE_DRIVER_OUT[1] ) );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRO2 , "" );
		}
		else {
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRO2 );
		}
		if ( COMPARE_DRIVER_OUT[1] > 0 ) {
			if ( COMPARE_DRIVER_OUT[2] > 0 )
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRO3 , R_IO_NAME( COMPARE_DRIVER_OUT[2] ) );
			else
				KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_DRO3 , "" );
		}
		else {
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_DRO3 );
		}
		KWIN_Item_Enable( hdlg , IDB_SEARCH );
		KWIN_Item_Hide( hdlg , IDB_SEARCH_N );
		SEARCH_ANDOR_REMAPPING( hdlg );
	}
}
//---------------------------------------------------------------------------------------
void SEARCH_SET_STRING( HWND hdlg , int mode , int btn ) {
	char Buffer[1024];
	strcpy( Buffer , COMPARE_STRINGDATA[mode] );
	if ( MODAL_STRING_BOX1( hdlg , "Search" , "Data" , Buffer ) ) {
		strncpy( COMPARE_STRINGDATA[mode] , Buffer , 255 );
		//
		if ( strlen( COMPARE_STRINGDATA[0] ) <= 0 ) {
			strcpy( COMPARE_STRINGDATA[0] , COMPARE_STRINGDATA[1] ); 
			strcpy( COMPARE_STRINGDATA[1] , "" );
		}
		if ( strlen( COMPARE_STRINGDATA[1] ) <= 0 ) {
			strcpy( COMPARE_STRINGDATA[1] , COMPARE_STRINGDATA[2] ); 
			strcpy( COMPARE_STRINGDATA[2] , "" ); 
		}
		KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_S1 , COMPARE_STRINGDATA[0] );
		if ( strlen( COMPARE_STRINGDATA[0] ) > 0 ) {
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_S2 , COMPARE_STRINGDATA[1] );
		}
		else {
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_S2 );
		}
		if ( strlen( COMPARE_STRINGDATA[1] ) > 0 ) {
			KWIN_Item_String_Display( hdlg , IDC_BUTTON_SEARCH_S3 , COMPARE_STRINGDATA[2] );
		}
		else {
			KWIN_Item_Hide( hdlg , IDC_BUTTON_SEARCH_S3 );
		}
		KWIN_Item_Enable( hdlg , IDB_SEARCH );
		KWIN_Item_Hide( hdlg , IDB_SEARCH_N );
		SEARCH_ANDOR_REMAPPING( hdlg );
	}
}
//---------------------------------------------------------------------------------------
void SEARCH_CONTROL( HWND hdlg ) {
	int i , c , c1 , c2 , c3 , c1f , c2f , c3f , c1id , c2id , c3id , c1od , c2od , c3od , cf , cid , cod;
	char Comp_String_Low[3][256];

	//====================================================
	READ_IO_INFORMATION_FOR_FINDING( hdlg , 1 );
	//====================================================

	KWIN_Item_Disable( hdlg , IDB_SEARCH_N );
	KWIN_Item_Hide( hdlg , IDB_SEARCH );

	ConGui_START_POINTER_RESET();

	strncpy( Comp_String_Low[0] , COMPARE_STRINGDATA[0] , 255 );	
	strncpy( Comp_String_Low[1] , COMPARE_STRINGDATA[1] , 255 );	
	strncpy( Comp_String_Low[2] , COMPARE_STRINGDATA[2] , 255 );	

	STRCONV_L( Comp_String_Low[0] );	
	STRCONV_L( Comp_String_Low[1] );	
	STRCONV_L( Comp_String_Low[2] );	

	c1 = strlen( Comp_String_Low[0] );
	c2 = strlen( Comp_String_Low[1] );
	c3 = strlen( Comp_String_Low[2] );

	D_SELECT_COUNT = 0;
	for ( i = 0 ; i < DIGITAL_CNT ; i++ ) {
		c1f  = c2f  = c3f = FALSE;
		c1od = c2od = c3od = FALSE;
		c1id = c2id = c3id = FALSE;
		switch( MATCH_CONTROL ) {
		case 0 :
			if ( c1 > 0 ) {
				if ( strncmp( D_IO_NAMEL( i ) , Comp_String_Low[0] , c1 ) == 0 ) {
					c1f = TRUE;
				}
			}
			if ( c2 > 0 ) {
				if ( strncmp( D_IO_NAMEL( i ) , Comp_String_Low[1] , c2 ) == 0 ) {
					c2f = TRUE;
				}
			}
			if ( c3 > 0 ) {
				if ( strncmp( D_IO_NAMEL( i ) , Comp_String_Low[2] , c3 ) == 0 ) {
					c3f = TRUE;
				}
			}
			break;
		case 1 :
			if ( c1 > 0 ) {
				if ( STRFIND_SUBSTRING( D_IO_NAMEL( i ) , Comp_String_Low[0] ) ) {
					c1f = TRUE;
				}
			}
			if ( c2 > 0 ) {
				if ( STRFIND_SUBSTRING( D_IO_NAMEL( i ) , Comp_String_Low[1] ) ) {
					c2f = TRUE;
				}
			}
			if ( c3 > 0 ) {
				if ( STRFIND_SUBSTRING( D_IO_NAMEL( i ) , Comp_String_Low[2] ) ) {
					c3f = TRUE;
				}
			}
			break;
		}
		if ( COMPARE_DRIVER_OUT[0] > 0 ) {
			if ( D_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[0] ) c1od = TRUE;
		}
		if ( COMPARE_DRIVER_OUT[1] > 0 ) {
			if ( D_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[1] ) c2od = TRUE;
		}
		if ( COMPARE_DRIVER_OUT[2] > 0 ) {
			if ( D_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[2] ) c3od = TRUE;
		}
		if ( COMPARE_DRIVER_IN[0] > 0 ) {
			if ( D_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[0] ) c1id = TRUE;
		}
		if ( COMPARE_DRIVER_IN[1] > 0 ) {
			if ( D_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[1] ) c2id = TRUE;
		}
		if ( COMPARE_DRIVER_IN[2] > 0 ) {
			if ( D_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[2] ) c3id = TRUE;
		}
		cf  = c1f  || c2f  || c3f;
		cod = c1od || c2od || c3od;
		cid = c1id || c2id || c3id;
		if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[0] == 0 ) c = cod || cid;
			else                         c = cod && cid;
			if ( COMPARE_ANDOR[1] == 0 ) c = c || cf;
			else                         c = c && cf;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 ) {
			if ( COMPARE_ANDOR[0] == 0 ) c = cod || cid;
			else                         c = cod && cid;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[1] == 0 ) c = cod || cf;
			else                         c = cod && cf;
		}
		else if ( COMPARE_DRIVER_IN[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[1] == 0 ) c = cid || cf;
			else                         c = cid && cf;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 ) {
			c = cod;
		}
		else if ( COMPARE_DRIVER_IN[0] > 0 ) {
			c = cid;
		}
		else if ( c1 > 0 ) {
			c = cf;
		}
		else {
			c = TRUE;
		}
		if ( c ) {
			D_SELECT_COUNT++;
			D_IO_SELECT( i )		=	TRUE;
		}
		else {
			D_IO_SELECT( i )		=	FALSE;
		}
	}
	A_SELECT_COUNT = 0;
	for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
		c1f  = c2f  = c3f = FALSE;
		c1od = c2od = c3od = FALSE;
		c1id = c2id = c3id = FALSE;
		switch( MATCH_CONTROL ) {
		case 0 :
			if ( c1 > 0 ) {
				if ( strncmp( A_IO_NAMEL( i ) , Comp_String_Low[0] , c1 ) == 0 ) {
					c1f = TRUE;
				}
			}
			if ( c2 > 0 ) {
				if ( strncmp( A_IO_NAMEL( i ) , Comp_String_Low[1] , c2 ) == 0 ) {
					c2f = TRUE;
				}
			}
			if ( c3 > 0 ) {
				if ( strncmp( A_IO_NAMEL( i ) , Comp_String_Low[2] , c3 ) == 0 ) {
					c3f = TRUE;
				}
			}
			break;
		case 1 :
			if ( c1 > 0 ) {
				if ( STRFIND_SUBSTRING( A_IO_NAMEL( i ) , Comp_String_Low[0] ) ) {
					c1f = TRUE;
				}
			}
			if ( c2 > 0 ) {
				if ( STRFIND_SUBSTRING( A_IO_NAMEL( i ) , Comp_String_Low[1] ) ) {
					c2f = TRUE;
				}
			}
			if ( c3 > 0 ) {
				if ( STRFIND_SUBSTRING( A_IO_NAMEL( i ) , Comp_String_Low[2] ) ) {
					c3f = TRUE;
				}
			}
			break;
		}
		if ( COMPARE_DRIVER_OUT[0] > 0 ) {
			if ( A_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[0] ) c1od = TRUE;
		}
		if ( COMPARE_DRIVER_OUT[1] > 0 ) {
			if ( A_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[1] ) c2od = TRUE;
		}
		if ( COMPARE_DRIVER_OUT[2] > 0 ) {
			if ( A_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[2] ) c3od = TRUE;
		}
		if ( COMPARE_DRIVER_IN[0] > 0 ) {
			if ( A_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[0] ) c1id = TRUE;
		}
		if ( COMPARE_DRIVER_IN[1] > 0 ) {
			if ( A_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[1] ) c2id = TRUE;
		}
		if ( COMPARE_DRIVER_IN[2] > 0 ) {
			if ( A_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[2] ) c3id = TRUE;
		}
		cf  = c1f  || c2f  || c3f;
		cod = c1od || c2od || c3od;
		cid = c1id || c2id || c3id;
		if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[0] == 0 ) c = cod || cid;
			else                         c = cod && cid;
			if ( COMPARE_ANDOR[1] == 0 ) c = c || cf;
			else                         c = c && cf;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 ) {
			if ( COMPARE_ANDOR[0] == 0 ) c = cod || cid;
			else                         c = cod && cid;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[1] == 0 ) c = cod || cf;
			else                         c = cod && cf;
		}
		else if ( COMPARE_DRIVER_IN[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[1] == 0 ) c = cid || cf;
			else                         c = cid && cf;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 ) {
			c = cod;
		}
		else if ( COMPARE_DRIVER_IN[0] > 0 ) {
			c = cid;
		}
		else if ( c1 > 0 ) {
			c = cf;
		}
		else {
			c = TRUE;
		}
		if ( c ) {
			A_SELECT_COUNT++;
			A_IO_SELECT( i )		=	TRUE;
		}
		else {
			A_IO_SELECT( i )		=	FALSE;
		}
	}
	S_SELECT_COUNT = 0;
	for ( i = 0 ; i < STRING_CNT ; i++ ) {
		c1f  = c2f  = c3f = FALSE;
		c1od = c2od = c3od = FALSE;
		c1id = c2id = c3id = FALSE;
		switch( MATCH_CONTROL ) {
		case 0 :
			if ( c1 > 0 ) {
				if ( strncmp( S_IO_NAMEL( i ) , Comp_String_Low[0] , c1 ) == 0 ) {
					c1f = TRUE;
				}
			}
			if ( c2 > 0 ) {
				if ( strncmp( S_IO_NAMEL( i ) , Comp_String_Low[1] , c2 ) == 0 ) {
					c2f = TRUE;
				}
			}
			if ( c3 > 0 ) {
				if ( strncmp( S_IO_NAMEL( i ) , Comp_String_Low[2] , c3 ) == 0 ) {
					c3f = TRUE;
				}
			}
			break;
		case 1 :
			if ( c1 > 0 ) {
				if ( STRFIND_SUBSTRING( S_IO_NAMEL( i ) , Comp_String_Low[0] ) ) {
					c1f = TRUE;
				}
			}
			if ( c2 > 0 ) {
				if ( STRFIND_SUBSTRING( S_IO_NAMEL( i ) , Comp_String_Low[1] ) ) {
					c2f = TRUE;
				}
			}
			if ( c3 > 0 ) {
				if ( STRFIND_SUBSTRING( S_IO_NAMEL( i ) , Comp_String_Low[2] ) ) {
					c3f = TRUE;
				}
			}
			break;
		}
		if ( COMPARE_DRIVER_OUT[0] > 0 ) {
			if ( S_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[0] ) c1od = TRUE;
		}
		if ( COMPARE_DRIVER_OUT[1] > 0 ) {
			if ( S_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[1] ) c2od = TRUE;
		}
		if ( COMPARE_DRIVER_OUT[2] > 0 ) {
			if ( S_IO_OUT_DRIVER( i ) == COMPARE_DRIVER_OUT[2] ) c3od = TRUE;
		}
		if ( COMPARE_DRIVER_IN[0] > 0 ) {
			if ( S_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[0] ) c1id = TRUE;
		}
		if ( COMPARE_DRIVER_IN[1] > 0 ) {
			if ( S_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[1] ) c2id = TRUE;
		}
		if ( COMPARE_DRIVER_IN[2] > 0 ) {
			if ( S_IO_IN_DRIVER( i ) == COMPARE_DRIVER_IN[2] ) c3id = TRUE;
		}
		cf  = c1f  || c2f  || c3f;
		cod = c1od || c2od || c3od;
		cid = c1id || c2id || c3id;
		if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[0] == 0 ) c = cod || cid;
			else                         c = cod && cid;
			if ( COMPARE_ANDOR[1] == 0 ) c = c || cf;
			else                         c = c && cf;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 && COMPARE_DRIVER_IN[0] > 0 ) {
			if ( COMPARE_ANDOR[0] == 0 ) c = cod || cid;
			else                         c = cod && cid;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[1] == 0 ) c = cod || cf;
			else                         c = cod && cf;
		}
		else if ( COMPARE_DRIVER_IN[0] > 0 && c1 > 0 ) {
			if ( COMPARE_ANDOR[1] == 0 ) c = cid || cf;
			else                         c = cid && cf;
		}
		else if ( COMPARE_DRIVER_OUT[0] > 0 ) {
			c = cod;
		}
		else if ( COMPARE_DRIVER_IN[0] > 0 ) {
			c = cid;
		}
		else if ( c1 > 0 ) {
			c = cf;
		}
		else {
			c = TRUE;
		}
		if ( c ) {
			S_SELECT_COUNT++;
			S_IO_SELECT( i )		=	TRUE;
		}
		else {
			S_IO_SELECT( i )		=	FALSE;
		}
	}
	KWIN_Item_Int_Display( hdlg , IDC_BUTTON_SEARCH_RD , D_SELECT_COUNT );
	KWIN_Item_Int_Display( hdlg , IDC_BUTTON_SEARCH_RA , A_SELECT_COUNT );
	KWIN_Item_Int_Display( hdlg , IDC_BUTTON_SEARCH_RS , S_SELECT_COUNT );
}





