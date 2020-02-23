#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <kutlFile.h>
#include <Kutlstr.h>
#include <kutlAnal.h>
#include <Kutltime.h>
#include <kutlgui.h>

#include "cimseqnc.h"
#include "iodefine.h"

#include "resource.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//no|      NoUse       F	                                 (Last)
//--+-----------------------
// 0|U->KeyIn(S/F)     Alarm(S/F/I)=>(I)KeyIn(S/F)=>Msg(S/F) (S)
//                                   (S/F)          Msg(S/F) (S)
// 1|                  Alarm(S/F/I)=>(I)KeyIn(S/F)=>Msg(S/F) (S/F)
//                                   (S/F)          Msg(S/F) (S/F)
// 2|                  KeyIn(S/F)=>Msg(S/F)                  (S/F)
// 3|                  Msg(F)                                (S)
// 4|                  Msg(F)                                (F)

// 5|U->x              Alarm(S/F/I)=>(I)KeyIn(S/F)=>Msg(S/F) (S)
//                                   (S/F)          Msg(S/F) (S)
// 6|                  Alarm(S/F/I)=>(I)KeyIn(S/F)=>Msg(S/F) (S/F)
//                                   (S/F)          Msg(S/F) (S/F)
// 7|                  KeyIn(S/F)=>Msg(S/F)                  (S/F)
// 8|                  Msg(F)                                (S)
// 9|                  Msg(F)                                (F)
//------------------------------------------------------------------------------------------
int	MID_READ_ERROR	= 151;
char PORT_INFO_STRING[32];
int CASSETTE_INDEX = 1;
//------------------------------------------------------------------------------------------
#define	STRING_MID_NOT_USE			"EVENT_ON MID_NOT_USE_MODE"
//#define	STRING_MID_READ_SUCCESS		"EVENT_ON MID_READ_COMPLETE"
//#define	STRING_MID_READ_FAIL		"EVENT_ON MID_READ_FAIL"
#define	STRING_MID_READ_SUCCESS		"EVENT_ON MID_AUTO_READ_COMPLETE"
#define	STRING_MID_READ_FAIL		"EVENT_ON MID_AUTO_READ_FAIL"
//
#define	STRING_CARR_ASSOCIATED		"EVENT_ON CARR_ASSOCIATED_F"
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "TMC_CASSCODE$0_CONTROL"		, _K_F_IO	,	CASSCODE_FUNCTION	,	0	} ,
	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER			,	0	} ,
	{ "FC_CM$0_MID_CONTROL"			, _K_D_IO	,	MID_CONTROL			,	0	} ,
	{ "PRM_CM$0_MID_OPERATE_TYPE"	, _K_D_IO	,	OPERATE_TYPE		,	0	} ,
	{ "CTC.MID_NAME$1"				, _K_S_IO	,	CTC_MID_NAME	,	0	} ,
	{ "CTC.MID_READ$1"				, _K_S_IO	,	CTC_MID_READ	,	0	} ,
	""
};
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL MID_SET_WINDOW_CONTROL = FALSE;
char MID_SET_WINDOW_STRING[256];
//------------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_MainIO_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	char Buffer[256];
	static UINT uTimer;
	switch ( msg ) {
	case WM_INITDIALOG:
		MID_SET_WINDOW_CONTROL = FALSE;
		if		( CASSETTE_INDEX == 1 ) SetWindowPos( hdlg , HWND_TOPMOST , 0 , 0 , 0 , 0 , SWP_NOSIZE );
		else if ( CASSETTE_INDEX == 2 ) SetWindowPos( hdlg , HWND_TOPMOST , 257 , 0 , 0 , 0 , SWP_NOSIZE );
		else SetWindowPos( hdlg , HWND_TOPMOST , 515 , 0 , 0 , 0 , SWP_NOSIZE );
		sprintf( MID_SET_WINDOW_STRING , "MID Manual Setting(%s)" , PORT_INFO_STRING );
		SetWindowText( hdlg , MID_SET_WINDOW_STRING );
		strcpy( MID_SET_WINDOW_STRING , "" );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDYES :
			strcpy( Buffer , MID_SET_WINDOW_STRING );
			if ( MODAL_STRING_BOX1( hdlg , "Select" , "Select MID Value" , Buffer ) ) {
				strcpy( MID_SET_WINDOW_STRING , Buffer );
				KWIN_Item_String_Display( hdlg , IDYES , MID_SET_WINDOW_STRING );
			}
			return TRUE;

		case IDOK :
			MID_SET_WINDOW_CONTROL = TRUE;
			EndDialog( hdlg , 0 );
			return TRUE;

		case IDCANCEL :
		case IDCLOSE :
			MID_SET_WINDOW_CONTROL = FALSE;
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	case WM_DESTROY:
		break;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL MID_SETTING_WITH_MANUAL( char *ReturnStr ) {
	MID_SET_WINDOW_CONTROL = FALSE;

	GoModalDialogBoxParam( GETHINST(NULL) , MAKEINTRESOURCE( IDD_USER_PAD ) , GetTopWindow(NULL)/*GetForegroundWindow()*/ , Console_Gui_For_MainIO_Proc , (LPARAM) NULL );
	if ( MID_SET_WINDOW_CONTROL ) {
		strcpy( ReturnStr , MID_SET_WINDOW_STRING );
		return TRUE;
	}
	strcpy( ReturnStr , "" );
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	char Data_Buffer[255];
	char Str_Buffer[255];
	int Type , CommStatus;
	int AlarmStatus;

	Type = READ_DIGITAL( OPERATE_TYPE , &CommStatus );
	if ( READ_DIGITAL( MID_CONTROL , &CommStatus ) == 1 ) {
		WRITE_STRING( CTC_MID_NAME , "" , &CommStatus );
		WRITE_STRING( CTC_MID_READ , "" , &CommStatus );
		switch( Type ) {
		case 0 :
		case 1 :
		case 2 :
		case 3 :
		case 4 :
			if ( MID_SETTING_WITH_MANUAL( Data_Buffer ) ) {
				WRITE_STRING( CTC_MID_NAME , Data_Buffer , &CommStatus );
				WRITE_STRING( CTC_MID_READ , Data_Buffer , &CommStatus );
				//
				sprintf( Str_Buffer , "%s %s|%s" , STRING_MID_READ_SUCCESS , PORT_INFO_STRING , Data_Buffer );
				WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
				//
				sprintf( Str_Buffer , "%s %s" , STRING_CARR_ASSOCIATED , PORT_INFO_STRING );
				WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
				return SYS_SUCCESS;
			}
			else {
				sprintf( Str_Buffer , "%s %s" , STRING_MID_READ_FAIL , PORT_INFO_STRING );
				WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
				return SYS_SUCCESS;
//				return SYS_ABORTED;
			}
			break;

		default :
			sprintf( Str_Buffer , "%s %s" , STRING_MID_NOT_USE , PORT_INFO_STRING );
			WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
			return SYS_SUCCESS;
//			return SYS_ABORTED;
			break;
		}
		return SYS_SUCCESS;
	}

	while ( TRUE ) {
		if ( RUN_FUNCTION( CASSCODE_FUNCTION  , "MID_READ" ) == SYS_ABORTED ) {
			WRITE_STRING( CTC_MID_NAME , "" , &CommStatus );
			WRITE_STRING( CTC_MID_READ , "" , &CommStatus );
			switch( Type ) {
			case 3 :
			case 8 :
				sprintf( Str_Buffer , "%s %s" , STRING_MID_READ_FAIL , PORT_INFO_STRING );
				WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
				return SYS_SUCCESS;
				break;
			case 4 :
			case 9 :
				sprintf( Str_Buffer , "%s %s" , STRING_MID_READ_FAIL , PORT_INFO_STRING );
				WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
				return SYS_SUCCESS;
//				return SYS_ABORTED;
				break;
			case 2 :
			case 7 :
				if ( MID_SETTING_WITH_MANUAL( Data_Buffer ) ) {
					WRITE_STRING( CTC_MID_NAME , Data_Buffer , &CommStatus );
					WRITE_STRING( CTC_MID_READ , Data_Buffer , &CommStatus );
					//
					sprintf( Str_Buffer , "%s %s|%s" , STRING_MID_READ_SUCCESS , PORT_INFO_STRING , Data_Buffer );
					WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
					//
					sprintf( Str_Buffer , "%s %s" , STRING_CARR_ASSOCIATED , PORT_INFO_STRING );
					WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
					return SYS_SUCCESS;
				}
				else {
					sprintf( Str_Buffer , "%s %s" , STRING_MID_READ_FAIL , PORT_INFO_STRING );
					WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
					return SYS_SUCCESS;
//					return SYS_ABORTED;
				}
				break;
			case 0 :
			case 1 :
			case 5 :
			case 6 :
				AlarmStatus = ALARM_MANAGE( MID_READ_ERROR );
				switch ( AlarmStatus ) {
				case ALM_RETRY :
					break;
				case ALM_IGNORE :
					if ( MID_SETTING_WITH_MANUAL( Data_Buffer ) ) {
						WRITE_STRING( CTC_MID_NAME , Data_Buffer , &CommStatus );
						WRITE_STRING( CTC_MID_READ , Data_Buffer , &CommStatus );
						//
						sprintf( Str_Buffer , "%s %s|%s" , STRING_MID_READ_SUCCESS , PORT_INFO_STRING , Data_Buffer );
						WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
						//
						sprintf( Str_Buffer , "%s %s" , STRING_CARR_ASSOCIATED , PORT_INFO_STRING );
						WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
						return SYS_SUCCESS;
					}
					else {
						sprintf( Str_Buffer , "%s %s" , STRING_MID_READ_FAIL , PORT_INFO_STRING );
						WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
						if ( ( Type == 0 ) || ( Type == 5 ) ) {
							return SYS_SUCCESS;
						}
						else {
//							return SYS_ABORTED;
							return SYS_SUCCESS;
						}
					}
					break;
				default :
					sprintf( Str_Buffer , "%s %s" , STRING_MID_READ_FAIL , PORT_INFO_STRING );
					WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
					if ( ( Type == 0 ) || ( Type == 5 ) ) {
						return SYS_SUCCESS;
					}
					else {
//						return SYS_ABORTED;
						return SYS_SUCCESS;
					}
					break;
				}
			}
		}
		else {
			READ_UPLOAD_MESSAGE( CASSCODE_FUNCTION  , Data_Buffer );
			//
			WRITE_STRING( CTC_MID_NAME , Data_Buffer , &CommStatus );
			WRITE_STRING( CTC_MID_READ , Data_Buffer , &CommStatus );
			//
			sprintf( Str_Buffer , "%s %s|%s" , STRING_MID_READ_SUCCESS , PORT_INFO_STRING , Data_Buffer );
			WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
			//
			sprintf( Str_Buffer , "%s %s" , STRING_CARR_ASSOCIATED , PORT_INFO_STRING );
			WRITE_STRING( SCHEDULER , Str_Buffer , &CommStatus );
			return SYS_SUCCESS;
		}
	}
	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char Str1[21] , Str2[21] , Str3[21];
	if ( !KWIN_DIALOG_DRIVER_LOADING() ) MessageBeep(100);
	STR_SEPERATE( PROGRAM_ARGUMENT_READ() , PORT_INFO_STRING , Str1 , 20 );
	CASSETTE_INDEX = atoi(PORT_INFO_STRING+2);
	STR_SEPERATE( Str1 , Str2 , Str3 , 20 );
	MID_READ_ERROR = atoi( Str2 );
	STR_SEPERATE( Str3 , Str2 , Str1 , 20 );
	REPLACE_CHAR_SET( 0 , Str2 );
	STR_SEPERATE( Str1 , Str2 , Str3 , 20 );
	if ( strcmp( Str2 , "1" ) == 0 ) sprintf( Str2 , "" );
	REPLACE_CHAR_SET( 1 , Str2 );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
