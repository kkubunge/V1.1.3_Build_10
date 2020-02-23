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
#include <windows.h>
#include <tlhelp32.h>

#include "kutlgui.h"
#include <Kutlstr.h>
#include <Kutlanal.h>
#include <Kutltime.h>

#include "resource.h"
#include "cimseqnc.h"
#include <TLHELP32.H>

BOOL g_bExcuteCheck = FALSE;

//
//

enum { Chiller_None, Chiller_Ground, Chiller_Ground2, Chiller_UnGround };
enum { HF_None, HF_Ground, HF_UnGround };
enum { ForeGauge_None, ForeGauge_Bara, ForeGauge_Conv };
enum { Pump_None, Pump_LOT_HD, Pump_LOT_DD, Pump_EBARA};
enum { PLC_None, PLC_Enable, PLC_Disable };
enum { TV_None, TV_Usys, TV_MKS };


BOOL APIENTRY InfoDisplay_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {

	static int NF3Seprate=0, MKS_Sel=0, PMSelect=0, result=0;
	int Radio_Bt_Chk = 0 ;
	
	FILE *fpt = NULL;	
	FILE *fpt2 = NULL; 	

	char Path[MAX_PATH] ;
	char Buffer[256] ;  
	char chr_return1[ 256 ];
	char chr_return2[ 256 ];
	int Line , ReadCnt ;
	BOOL FileEnd = TRUE;
	char Radio_Bt_Chk_Msg[ 256 ] ;
	char chr_latter = 37;	// '%'

	if (g_bExcuteCheck == TRUE)
	{
		ShowWindow(GetDlgItem(hdlg , IDOK), SW_HIDE);
		
		//for ( i = IDC_RADIO_RCPDOWN_NO; i <= IDC_RADIO_NF3_YES; i++ )	EnableWindow(GetDlgItem(hdlg, i), FALSE);
	}

	switch ( msg ) {
	case WM_INITDIALOG:

		if ( ( fpt = fopen( "System/Equipment_Set.cfg" , "r+t" ) ) == NULL ) {
		//	printf( "Cannot Find Equipment_Set.cfg File\n" );
			sprintf( Buffer , "0" );
			MessageBox( hdlg , "Cannot Find Equipment_Set.cfg File!!" , "ERROR" , MB_ICONHAND );
		//	CheckRadioButton(hdlg, IDC_RADIO1 , IDC_RADIO2 , IDC_RADIO1 ) ;
			NF3Seprate = 0;
			MKS_Sel=0;
			PMSelect=0;
			MoveCenterWindow( hdlg );
			return TRUE;
		}
		else { //---> 파일을 열어 값을 읽는다.
			for ( Line = 1 ; FileEnd ; Line++ ) {
				FileEnd = Get_Line_String_From_File( fpt , Buffer , &ReadCnt );
				if ( ReadCnt > 0 ) {
					if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
						printf( "Unknown Error 1 - [%s]-[%d]\n" , Buffer , Line );
					}else {	
						
						if( STRCMP_L( chr_return1 , "NF3SEPA_USE:" )){
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
								printf( "Unknown Error 2 - [%s]-[%d]\n" , Buffer , Line );
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_NF3_NO , IDC_RADIO_NF3_YES , IDC_RADIO_NF3_NO ) ;
									NF3Seprate = 1 ;
								}else if( atoi(chr_return2) == 2 ) {    
									CheckRadioButton(hdlg, IDC_RADIO_NF3_NO , IDC_RADIO_NF3_YES , IDC_RADIO_NF3_YES ) ;
									NF3Seprate = 2 ;
								}else {
									NF3Seprate = 0 ;
								}
							}
						}

						//[ 2010/04/08 PMC SELECT 기능 추가 ( BY JTM )
						else if( STRCMP_L( chr_return1 , "PMC_TYPE:" )){
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
								printf( "Unknown Error 2 - [%s]-[%d]\n" , Buffer , Line );
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_SELECT_PMA , IDC_RADIO_SELECT_PMC , IDC_RADIO_SELECT_PMA ) ;
									PMSelect = 1 ;
								}else if( atoi(chr_return2) == 2 ) {    
									CheckRadioButton(hdlg, IDC_RADIO_SELECT_PMA , IDC_RADIO_SELECT_PMC , IDC_RADIO_SELECT_PMB ) ;
									PMSelect = 2 ;
								}else if( atoi(chr_return2) == 3 ) {    
									CheckRadioButton(hdlg, IDC_RADIO_SELECT_PMA , IDC_RADIO_SELECT_PMC , IDC_RADIO_SELECT_PMC ) ;
									PMSelect = 3 ;
								}else {
									PMSelect = 0 ;
								}
							}
						}

						// MKS651 Fan Interlock 
						else if( STRCMP_L( chr_return1 , "MKSFAN_USE:" )){
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
								printf( "Unknown Error 2 - [%s]-[%d]\n" , Buffer , Line );
							}else {	
								if( atoi(chr_return2) == 1 ) {								
									CheckRadioButton(hdlg, IDC_RADIO_NF3_NO2 , IDC_RADIO_NF3_YES2  , IDC_RADIO_NF3_NO2 ) ;
									MKS_Sel = 1 ;
								}else if( atoi(chr_return2) == 2 ) { 								
									CheckRadioButton(hdlg, IDC_RADIO_NF3_NO2 , IDC_RADIO_NF3_YES2 , IDC_RADIO_NF3_YES2 ) ;
									MKS_Sel = 2 ;
								}else {
									MKS_Sel = 0 ;
								}
							}
						}
						//]
					}
				}
			}
			fclose ( fpt ) ;
		}
		
		//
		MoveCenterWindow( hdlg );
		//
		return TRUE;

	case WM_COMMAND :

		if ( HIWORD(wParam) == BN_CLICKED )
		{
			switch(LOWORD(wParam))
			{
			case IDC_RADIO_NF3_NO:
				NF3Seprate = 1 ;
				break ;
			case IDC_RADIO_NF3_YES:
				NF3Seprate = 2 ;
				break ;
			case IDC_RADIO_NF3_NO2:
				MKS_Sel = 1 ;
				break ;
			case IDC_RADIO_NF3_YES2:
				MKS_Sel = 2 ;
				break ;
			case IDC_RADIO_SELECT_PMA:
				PMSelect = 1 ;
				break ;
			case IDC_RADIO_SELECT_PMB:
				PMSelect = 2 ;
				break ;
			case IDC_RADIO_SELECT_PMC:
				PMSelect = 3 ;
				break ;
			}		
		}
		else
		{
			NF3Seprate = 0 ;
			MKS_Sel = 0 ;
			PMSelect = 0 ;
		}
		
		switch( wParam ) {
		case IDOK :
			result = TRUE;

			// EQ 설정하지 않은 항목 존재 시 Error Message 처리 (2012/12/21,LJH)
			// File 변경 전 Check해야, EasyCluster.cfg 내용이 삭제되지 않음.
			sprintf ( Radio_Bt_Chk_Msg , "# NO SELECT ERROR\n- LIST\n" ) ;
			if ( !NF3Seprate ) { strcat ( Radio_Bt_Chk_Msg , ". NF3 SEPARATE USE OPTION\n" ); }
			//if ( !SIM_Sel ) { strcat ( Radio_Bt_Chk_Msg , ". SIMMULATION MODE\n" ); }
			if ( !PMSelect ) { strcat ( Radio_Bt_Chk_Msg , ". PMC NOT SELECTED\n" ); }
			if ( !MKS_Sel ) { strcat ( Radio_Bt_Chk_Msg , ". MKS FAN NOT SELECTED\n" ); }

			if ( !NF3Seprate || !MKS_Sel || !PMSelect ) 
			{
				MessageBox ( hdlg , Radio_Bt_Chk_Msg , "ERROR" , MB_ICONHAND ) ;
				return TRUE ;
			}

			// PMABC
			if (PMSelect == 1)	// PMA
			{
				// Launcher
				if ( result ) result = CopyFile( "System\\PMA\\EasyCluster.cfg" , "EasyCluster.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\EasyCluster.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMA\\Startup.cfg" , "Startup.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\Startup.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				if ( result ) result = CopyFile( "System\\PMA\\SIM-EasyCluster.cfg" , "SIM-EasyCluster.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\SIM-EasyCluster.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMA\\TEST-Startup.cfg" , "TEST-Startup.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\TEST-Startup.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// System
				// IO
				if ( result ) result = CopyFile( "System\\PMA\\System\\IO\\PMABC_Difference.con" , "System\\IO\\PMABC_Difference.con", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\System\\PMABC_Difference.con)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
			

				// NF3 Separate 
				if (NF3Seprate == 1)	// NO
				{
					// Function
					if ( result ) result = CopyFile( "System\\NF3Separate\\Function\\F_GUI_beforeNF3.dat" , "Function\\F_GUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(Startup\\NF3Separate\\Function\\F_GUI.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}
				else if (NF3Seprate == 2)	// YES
				{					
					// Function
					if ( result ) result = CopyFile( "System\\NF3Separate\\Function\\F_GUI.dat" , "Function\\F_GUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(Startup\\NF3Separate\\Function\\F_GUI.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}

			}
			else if (PMSelect == 2)	// PMB
			{
				// Launcher
				if ( result ) result = CopyFile( "System\\PMB\\EasyCluster.cfg" , "EasyCluster.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMB\\EasyCluster.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMB\\Startup.cfg" , "Startup.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMB\\Startup.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMB\\SIM-EasyCluster.cfg" , "SIM-EasyCluster.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\SIM-EasyCluster.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMB\\TEST-Startup.cfg" , "TEST-Startup.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\TEST-Startup.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// System
				// IO
				if ( result ) result = CopyFile( "System\\PMB\\System\\IO\\PMABC_Difference.con" , "System\\IO\\PMABC_Difference.con", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMB\\System\\PMABC_Difference.con)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// NF3 Separate 
				if (NF3Seprate == 1)	// NO
				{
					// Function
					if ( result ) result = CopyFile( "System\\NF3Separate\\Function\\F_GUI_beforeNF3.dat" , "Function\\F_GUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(System\\NF3Separate\\Function\\F_GUI_beforeNF3.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}
				else if (NF3Seprate == 2)	// YES
				{
					// Function
					if ( result ) result = CopyFile( "System\\NF3Separate\\Function\\F_GUI.dat" , "Function\\F_GUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(System\\NF3Separate\\Function\\F_GUI.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}

			}
			else if (PMSelect == 3)	// PMC
			{
				// Launcher
				if ( result ) result = CopyFile( "System\\PMC\\EasyCluster.cfg" , "EasyCluster.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMC\\EasyCluster.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMC\\Startup.cfg" , "Startup.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMC\\Startup.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMC\\SIM-EasyCluster.cfg" , "SIM-EasyCluster.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\SIM-EasyCluster.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\PMC\\TEST-Startup.cfg" , "TEST-Startup.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMA\\TEST-Startup.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// System
				// IO
				if ( result ) result = CopyFile( "System\\PMC\\System\\IO\\PMABC_Difference.con" , "System\\IO\\PMABC_Difference.con", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\PMC\\System\\PMABC_Difference.con)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }


				// NF3 Separate 
				if (NF3Seprate == 1)	// NO
				{
					// Function
					if ( result ) result = CopyFile( "System\\NF3Separate\\Function\\F_GUI_beforeNF3.dat" , "Function\\F_GUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(System\\NF3Separate\\Function\\F_GUI_beforeNF3.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}
				else if (NF3Seprate == 2)	// YES
				{					
					// Function
					if ( result ) result = CopyFile( "System\\NF3Separate\\Function\\F_GUI.dat" , "Function\\F_GUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(System\\NF3Separate\\Function\\F_GUI.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}
			}

			// MKS651 Fan Interlock
			if (MKS_Sel == 1)	// OFF
			{
				if ( result ) result = CopyFile( "System\\IO\\Cfgs\\Mks651_old.cfg" , "IO\\Cfgs\\Mks651.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\IO\\Cfgs\\Mks651_old.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
			}
			else if (MKS_Sel == 2)	// ON
			{
				if ( result ) result = CopyFile( "System\\IO\\Cfgs\\Mks651.cfg" , "IO\\Cfgs\\Mks651.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\IO\\Cfgs\\Mks651.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
			}

			sprintf( Path , "System\\Equipment_Set.cfg");
			fpt = fopen( Path , "w" );
			if( fpt )
			{
				fprintf( fpt , "#-- NF3 SEPARATE USE   ==> NO       : 1 , USE : 2 , Not Select : 0 --\n");
				fprintf( fpt , "#-- PMC SELECT  TYPE   ==> PMA : 1 , PMB : 2 , PMC : 3 , Not Select : 0 --\n");
				fprintf( fpt , "#-- MKS FAN INTK USE   ==> OFF : 1 , ON : 2 , Not Select : 0 --\n");
								
				fprintf( fpt , "NF3SEPA_USE: %d\n" , NF3Seprate );
				fprintf( fpt , "PMC_TYPE: %d\n"	   , PMSelect );
				fprintf( fpt , "MKSFAN_USE: %d\n"	   , MKS_Sel );
				
				fclose( fpt );
			}
	
			if ( !result )
			{
				MessageBox( hdlg , "Definition File Copy Fail! Last Success Information will be loaded." , "ERROR" , MB_ICONHAND );				
			}			 
			
			EndDialog( hdlg , 0 );
			return TRUE;
			
		case IDC_CANCEL :
			EndDialog( hdlg , 0 );
			return TRUE;
		}

		return TRUE;
	}
	return FALSE;
}

int APIENTRY WinMain( HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdLine , int nCmdShow ) {

	// 2018.01.11 added by MJ [엔진 실행중이면 Equipment Setting 설정 안되게끔]
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    PROCESSENTRY32 processEntry32;
	char Process[24] = "Cims97_h.exe";
	
    if(hProcessSnap == INVALID_HANDLE_VALUE)
    {	
        exit(EXIT_FAILURE);	
    }
	
    processEntry32.dwSize = sizeof(PROCESSENTRY32);
	
    if( !Process32First(hProcessSnap, &processEntry32) )	
    {
        CloseHandle(hProcessSnap);	
        exit(EXIT_FAILURE);  
    }
	
    while(Process32Next(hProcessSnap, &processEntry32))	
    {	
        // 실행중인 프로세스 비교
        if(strcmp(Process, processEntry32.szExeFile) == 0)	
        {		
            // 실행중인 프로세스가 있다면	
			g_bExcuteCheck = TRUE;
			break;
            //exit(EXIT_FAILURE);	
        }	
    }
	
	CloseHandle(hProcessSnap);
    // 실행중인 프로세스 중에 찾는 프로세스가 없다면	
    //return FALSE;

	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_DIALOG ) , NULL , InfoDisplay_Proc , (LPARAM) NULL );
	return 0;
}
